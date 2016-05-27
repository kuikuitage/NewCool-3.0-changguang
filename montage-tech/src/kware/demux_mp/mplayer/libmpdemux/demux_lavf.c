/*
 * Copyright (C) 2004 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <limits.h>
#ifndef __LINUX__
#include "mp_func_trans.h"
#else
#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>
#include <limits.h>
#endif

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"
#include "av_opts.h"
#include "av_helpers.h"

#include "stream/stream.h"
#include "aviprint.h"
#include "demuxer.h"
#include "stheader.h"
#include "m_option.h"
#include "sub/sub.h"

#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavutil/avutil.h"
#include "libavutil/avstring.h"
#include "libavutil/mathematics.h"
#include "libavutil/opt.h"

#include "mp_taglists.h"
#ifdef __LINUX__
#define OS_PRINTF printf
#endif
#define INITIAL_PROBE_SIZE io_stream_buffer_size //STREAM_BUFFER_SIZE
#define SMALL_MAX_PROBE_SIZE (32 * 1024)
#define PROBE_BUF_SIZE (2*1024*1024)
//#define mp_msg(t, l,m, args...) OS_PRINTF(m, ##args)

//#define SEEK_DEBUG
#ifdef SEEK_DEBUG
extern flag_debug, while_cnt;
#endif

static unsigned int opt_probesize = 0;
static unsigned int opt_analyzeduration = 0;
static char *opt_format;
static char *opt_cryptokey;
static char *opt_avopt = NULL;

const m_option_t lavfdopts_conf[] = {
	{"probesize", &(opt_probesize), CONF_TYPE_INT, CONF_RANGE, 32, INT_MAX, NULL},
	{"format",    &(opt_format),    CONF_TYPE_STRING,       0,  0,       0, NULL},
	{"analyzeduration",    &(opt_analyzeduration),    CONF_TYPE_INT,       CONF_RANGE,  0,       INT_MAX, NULL},
	{"cryptokey", &(opt_cryptokey), CONF_TYPE_STRING,       0,  0,       0, NULL},
	{"o",                  &opt_avopt,                CONF_TYPE_STRING,    0,           0,             0, NULL},
	{NULL, NULL, 0, 0, 0, 0, NULL}
};

#define BIO_BUFFER_SIZE 32768

typedef struct lavf_priv {
	AVInputFormat *avif;
	AVFormatContext *avfc;
	AVIOContext *pb;
	uint8_t *buffer;
	int audio_streams;
	int video_streams;
	int sub_streams;
	int64_t last_pts;
	int astreams[MAX_A_STREAMS];
	int vstreams[MAX_V_STREAMS];
	int sstreams[MAX_S_STREAMS];
	int cur_program;
	int nb_streams_last;
      int vc1_first_video_frame;
}lavf_priv_t;
extern ff_pmt_id ff_mpegts_pmt;
extern int io_isnetworkstream;
extern int special_ts_vpid;
extern int special_ts_apid;
extern int hls_protocol;

extern void search_id_in_pmt();

static int mp_read(void *opaque, uint8_t *buf, int size) {
	demuxer_t *demuxer = opaque;
	stream_t *stream = demuxer->stream;
	int ret;

	//OS_PRINTF("%s    %d  start\n",__func__,__LINE__);
	ret=stream_read(stream, buf, size);
	//OS_PRINTF("%d=mp_read(%p, %p, %d), pos: %"PRId64", eof:%d\n",
	//		ret, stream, buf, size, stream_tell(stream), stream->eof);
	mp_msg(MSGT_HEADER,MSGL_DBG2,"%d=mp_read(%p, %p, %d), pos: %"PRId64", eof:%d\n",
			ret, stream, buf, size, stream_tell(stream), stream->eof);
	return ret;
}

static int64_t mp_seek(void *opaque, int64_t pos, int whence) {
	demuxer_t *demuxer = opaque;
	stream_t *stream = demuxer->stream;
	int64_t current_pos;
	mp_msg(MSGT_HEADER,MSGL_DBG2,"mp_seek(%p, %"PRId64", %d)\n", stream, pos, whence);
	OS_PRINTF("[%s] mp_seek(%p, %"PRId64", %d)\n", __func__,stream, pos, whence);
	if(whence == SEEK_CUR)
		pos +=stream_tell(stream);
	else if(whence == SEEK_END && stream->end_pos > 0)
		pos += stream->end_pos;
	else if(whence == SEEK_SET)
		pos += stream->start_pos;
	else if(whence == AVSEEK_SIZE && stream->end_pos > 0) {
		uint64_t size;
		stream_control(stream, STREAM_CTRL_GET_SIZE, &size);
		if (size > stream->end_pos)
			stream->end_pos = size;
		return stream->end_pos - stream->start_pos;
	} else
		return -1;

	if(pos<0)
		return -1;
	current_pos = stream_tell(stream);
	if(stream_seek(stream, pos)==0) {
		stream_reset(stream);
		stream_seek(stream, current_pos);
		return -1;
	}

	return pos - stream->start_pos;
}

static int64_t mp_read_seek(void *opaque, int stream_idx, int64_t ts, int flags) {
	demuxer_t *demuxer = opaque;
	stream_t *stream = demuxer->stream;
	lavf_priv_t *priv = demuxer->priv;
	AVStream *st = priv->avfc->streams[stream_idx];
	int ret;
	double pts;

	pts = (double)ts * st->time_base.num / st->time_base.den;
	ret = stream_control(stream, STREAM_CTRL_SEEK_TO_TIME, &pts);
	if (ret < 0)
		ret = AVERROR(ENOSYS);
	return ret;
}

static void list_formats(void) {
	AVInputFormat *fmt;
	mp_msg(MSGT_DEMUX, MSGL_INFO, "Available lavf input formats:\n");
	for (fmt = av_iformat_next(NULL); fmt; fmt = av_iformat_next(fmt))
		mp_msg(MSGT_DEMUX, MSGL_INFO, "%15s : %s\n", fmt->name, fmt->long_name);
}

static int lavf_check_file(demuxer_t *demuxer)
{
	AVProbeData avpd;
	lavf_priv_t *priv;
	int probe_data_size = 0;
	int read_size = INITIAL_PROBE_SIZE;
	int score;
	int probe_size = PROBE_BUF_SIZE;
	
	OS_PRINTF("[%s] ----- start\n",__func__);
	
	if(!demuxer->priv)
		demuxer->priv=calloc(sizeof(lavf_priv_t),1);
	
	priv= demuxer->priv;
	if(!priv->buffer)
       priv->buffer = calloc(sizeof(uint8_t),BIO_BUFFER_SIZE);
	init_avformat();

	if (opt_format) {
		if (strcmp(opt_format, "help") == 0) {
			list_formats();
			return 0;
		}
		priv->avif= av_find_input_format(opt_format);
		if (!priv->avif) {
			mp_msg(MSGT_DEMUX,MSGL_FATAL,"Unknown lavf format %s\n", opt_format);
			return 0;
		}
		mp_msg(MSGT_DEMUX,MSGL_INFO,"Forced lavf %s demuxer\n", priv->avif->long_name);
		return DEMUXER_TYPE_LAVF;
	}

	avpd.buf = av_mallocz(FFMAX(BIO_BUFFER_SIZE, probe_size) + FF_INPUT_BUFFER_PADDING_SIZE);

	while(avpd.buf == NULL && probe_size > BIO_BUFFER_SIZE)
	{
	    OS_PRINTF("[%s] av_mallocz fail\n", __func__);
		probe_size = probe_size/2;
		avpd.buf = av_mallocz(FFMAX(BIO_BUFFER_SIZE, probe_size) + FF_INPUT_BUFFER_PADDING_SIZE);
	}
	
   //yliu add
    if(avpd.buf == 0)
       return 0;
	do {
		read_size = stream_read(demuxer->stream, avpd.buf + probe_data_size, read_size);
		OS_PRINTF("[%s] --in while--- read_size: %d\n",__func__,read_size);
		if(read_size < 0) {
			av_free(avpd.buf);
			OS_PRINTF("[%s] --1--- return 0\n",__func__);
			return 0;
		}
		probe_data_size += read_size;
		avpd.filename= demuxer->stream->url;
		if (!avpd.filename) {
			mp_msg(MSGT_DEMUX, MSGL_WARN, "Stream url is not set!\n");
			avpd.filename = "";
		}
		if (!strncmp(avpd.filename, "ffmpeg://", 9))
			avpd.filename += 9;
		avpd.buf_size= probe_data_size;

		score = 0;
		priv->avif= av_probe_input_format2(&avpd, probe_data_size > 0, &score);
		read_size = FFMIN(2*read_size, probe_size - probe_data_size);
	} while ((demuxer->desc->type != DEMUXER_TYPE_LAVF_PREFERRED ||
				probe_data_size < SMALL_MAX_PROBE_SIZE) &&
			score <= AVPROBE_SCORE_MAX / 4 &&
			read_size > 0 && probe_data_size < probe_size);
	OS_PRINTF("[%s] --out of while--- read_size: %d\n",__func__,read_size);
	av_free(avpd.buf);

	if(!priv->avif){
		mp_msg(MSGT_HEADER,MSGL_V,"LAVF_check: no clue about this gibberish!\n");
		OS_PRINTF("[%s] --2--- return 0\n",__func__);
		return 0;
	}else
		mp_msg(MSGT_HEADER,MSGL_V,"LAVF_check: %s\n", priv->avif->long_name);

		OS_PRINTF("[%s] ----- end, return DEMUXER_TYPE_LAVF\n",__func__);

	return DEMUXER_TYPE_LAVF;
}

/* Before adding anything to this list please stop and consider why.
 * There are two good reasons
 * 1) to reduce startup time when streaming these file types
 * 2) workarounds around bugs in our native demuxers that are not reasonable to
 *    fix
 * For the case 2) that means the issue should be understood well
 * enough to be able to decide that a fix is not reasonable.
 */
static const char * const preferred_list[] = {
	"cdxl",
	"dxa",
	"flv",
	"gxf",
	"nut",
	"nuv",
	"matroska,webm",
	"mov,mp4,m4a,3gp,3g2,mj2",
	"mpc",
	"mpc8",
	"mxf",
	"ogg",
	"pva",
	"qcp",
	"swf",
	"vqf",
	"w64",
	"wv",
	"yuv4mpegpipe",
	NULL
};

static int lavf_check_preferred_file(demuxer_t *demuxer){
	if (lavf_check_file(demuxer)) {
		const char * const *p = preferred_list;
		lavf_priv_t *priv = demuxer->priv;
		while (*p) {
			if (strcmp(*p, priv->avif->name) == 0)
				return DEMUXER_TYPE_LAVF_PREFERRED;
			p++;
		}
	}
	return 0;
}

static uint8_t char2int(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return 0;
}

static void parse_cryptokey(AVFormatContext *avfc, const char *str) {
	int len = strlen(str) / 2;
	uint8_t *key = av_mallocz(len);
	int i;
	avfc->keylen = len;
	avfc->key = key;
	for (i = 0; i < len; i++, str += 2)
		*key++ = (char2int(str[0]) << 4) | char2int(str[1]);
}

static void handle_stream(demuxer_t *demuxer, AVFormatContext *avfc, int i) {
	lavf_priv_t *priv= demuxer->priv;
	AVStream *st= avfc->streams[i];
	AVCodecContext *codec= st->codec;
	char *stream_type = NULL;
	int stream_id;
	AVDictionaryEntry *lang = av_dict_get(st->metadata, "language", NULL, 0);
	AVDictionaryEntry *title= av_dict_get(st->metadata, "title",    NULL, 0);
	int g;

	switch(codec->codec_type){
		case AVMEDIA_TYPE_AUDIO:{
						WAVEFORMATEX *wf;
						sh_audio_t* sh_audio;

						printf("[%s] ----- AVMEDIA_TYPE_AUDIO: audio track %d: %s \n",__func__,i,lang ? lang->value : NULL);

						sh_audio = new_sh_audio_aid(demuxer, i, priv->audio_streams, lang ? lang->value : NULL);
						if(!sh_audio)
							break;
						stream_type = "audio";
						priv->astreams[priv->audio_streams] = i;
						wf= calloc(sizeof(*wf) + codec->extradata_size, 1);
						codec->codec_tag = mp_codec_id2tag(codec->codec_id, codec->codec_tag, 1);
						wf->wFormatTag= codec->codec_tag;
						wf->nChannels= codec->channels;
						wf->nSamplesPerSec= codec->sample_rate;
						wf->nAvgBytesPerSec= codec->bit_rate/8;
						wf->nBlockAlign= codec->block_align ? codec->block_align : 1;
						wf->wBitsPerSample= codec->bits_per_coded_sample;
						wf->cbSize= codec->extradata_size;
						if(codec->extradata_size)
							memcpy(wf + 1, codec->extradata, codec->extradata_size);
						sh_audio->wf= wf;
						sh_audio->audio.dwSampleSize= codec->block_align;
						if(codec->frame_size && codec->sample_rate){
							sh_audio->audio.dwScale=codec->frame_size;
							sh_audio->audio.dwRate= codec->sample_rate;
						}else{
							sh_audio->audio.dwScale= codec->block_align ? codec->block_align*8 : 8;
							sh_audio->audio.dwRate = codec->bit_rate;
						}
						g= av_gcd(sh_audio->audio.dwScale, sh_audio->audio.dwRate);
						sh_audio->audio.dwScale /= g;
						sh_audio->audio.dwRate  /= g;
						//          printf("sca:%d rat:%d fs:%d sr:%d ba:%d\n", sh_audio->audio.dwScale, sh_audio->audio.dwRate, codec->frame_size, codec->sample_rate, codec->block_align);
						sh_audio->ds= demuxer->audio;
						//OS_PRINTF("[%s] --1--ch: %d, samplerate: %d, ibps: %d\n",__func__,sh_audio->channels,sh_audio->samplerate,sh_audio->i_bps);
						sh_audio->format= codec->codec_tag;
						sh_audio->channels= codec->channels;
						sh_audio->samplerate= codec->sample_rate;
						sh_audio->i_bps= codec->bit_rate/8;
						//OS_PRINTF("[%s] --2--ch: %d, samplerate: %d, ibps: %d\n",__func__,sh_audio->channels,sh_audio->samplerate,sh_audio->i_bps);
						switch (codec->codec_id) {
							case CODEC_ID_PCM_S8:
							case CODEC_ID_PCM_U8:
								sh_audio->samplesize = 1;
								break;
							case CODEC_ID_PCM_S16LE:
							case CODEC_ID_PCM_S16BE:
							case CODEC_ID_PCM_U16LE:
							case CODEC_ID_PCM_U16BE:
								sh_audio->samplesize = 2;
								break;
							case CODEC_ID_PCM_ALAW:
								sh_audio->format = 0x6;
								break;
							case CODEC_ID_PCM_MULAW:
								sh_audio->format = 0x7;
								break;
						}
						if (title && title->value)
						{
							mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_AID_%d_NAME=%s\n", priv->audio_streams, title->value);
							//printf("[%s] ----- audio track %d title: %s, title len:%d \n",__func__,i,title->value,strlen(title->value));
							sh_audio->title = strdup(title->value);
							//printf("[%s] ----- sh_audio->title len:%d \n",__func__,strlen(sh_audio->title));
						}
                        else
                            sh_audio->title = NULL;
						if (st->disposition & AV_DISPOSITION_DEFAULT)
							sh_audio->default_track = 1;
						if(mp_msg_test(MSGT_HEADER,MSGL_V) ) print_wave_header(sh_audio->wf, MSGL_V);
						// select the first audio stream if auto-selection is requested
						if (demuxer->audio->id == -1) {
                            if(io_isnetworkstream && ff_mpegts_pmt.use_ff_mpegts && ff_mpegts_pmt.audio_id != -1)
                            {
							    demuxer->audio->id = ff_mpegts_pmt.audio_id;
                            }
                            else
                            {
							    demuxer->audio->id = i;
                            }
							    demuxer->audio->sh= demuxer->a_streams[i];
						}
						if (demuxer->audio->id != i)
							st->discard= AVDISCARD_ALL;
						stream_id = priv->audio_streams++;
						break;
					}
		case AVMEDIA_TYPE_VIDEO:{
						sh_video_t* sh_video;
						BITMAPINFOHEADER *bih;
						sh_video=new_sh_video_vid(demuxer, i, priv->video_streams);
						if(!sh_video) break;
						stream_type = "video";
						priv->vstreams[priv->video_streams] = i;
						bih=calloc(sizeof(*bih) + codec->extradata_size,1);

						if(codec->codec_id == CODEC_ID_RAWVIDEO) {
							switch (codec->pix_fmt) {
								case PIX_FMT_RGB24:
									codec->codec_tag= MKTAG(24, 'B', 'G', 'R');
								case PIX_FMT_BGR24:
									codec->codec_tag= MKTAG(24, 'R', 'G', 'B');
							}
						}
						codec->codec_tag = mp_codec_id2tag(codec->codec_id, codec->codec_tag, 0);
						bih->biSize= sizeof(*bih) + codec->extradata_size;
						bih->biWidth= codec->width;
						bih->biHeight= codec->height;
						bih->biBitCount= codec->bits_per_coded_sample;
						bih->biSizeImage = bih->biWidth * bih->biHeight * bih->biBitCount/8;
						bih->biCompression= codec->codec_tag;
						sh_video->bih= bih;
						sh_video->disp_w= codec->width;
						sh_video->disp_h= codec->height;
						if (st->time_base.den) { /* if container has time_base, use that */
							sh_video->video.dwRate= st->time_base.den;
							sh_video->video.dwScale= st->time_base.num;
						} else {
							sh_video->video.dwRate= codec->time_base.den;
							sh_video->video.dwScale= codec->time_base.num;
						}
						sh_video->fps=av_q2d(st->avg_frame_rate);
						sh_video->frametime=1/av_q2d(st->r_frame_rate);
						// sh_video->frametime=0.04;
						sh_video->format=bih->biCompression;
						if(st->sample_aspect_ratio.num)
							sh_video->aspect = codec->width  * st->sample_aspect_ratio.num
								/ (float)(codec->height * st->sample_aspect_ratio.den);
						else
							sh_video->aspect=codec->width  * codec->sample_aspect_ratio.num
								/ (float)(codec->height * codec->sample_aspect_ratio.den);
						sh_video->i_bps=codec->bit_rate/8;
						if (title && title->value)
							mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_VID_%d_NAME=%s\n", priv->video_streams, title->value);
						mp_msg(MSGT_DEMUX,MSGL_DBG2,"aspect= %d*%d/(%d*%d)\n",
								codec->width, codec->sample_aspect_ratio.num,
								codec->height, codec->sample_aspect_ratio.den);

						sh_video->ds= demuxer->video;
						if(codec->extradata_size)
							memcpy(sh_video->bih + 1, codec->extradata, codec->extradata_size);
						if( mp_msg_test(MSGT_HEADER,MSGL_V) ) print_video_header(sh_video->bih, MSGL_V);
						/*
						   short biPlanes;
						   int  biXPelsPerMeter;
						   int  biYPelsPerMeter;
						   int biClrUsed;
						   int biClrImportant;
						 */
						// select the first video stream if auto-selection is requested
						if(demuxer->video->id == -1) {
                        if(io_isnetworkstream && ff_mpegts_pmt.use_ff_mpegts && ff_mpegts_pmt.video_id != -1)
                        {
							    demuxer->video->id = ff_mpegts_pmt.video_id;
                        }
                        else
                        {
							    demuxer->video->id = i;
                        }
							    demuxer->video->sh= demuxer->v_streams[i];
						}
						if(demuxer->video->id != i)
							st->discard= AVDISCARD_ALL;
						stream_id = priv->video_streams++;
						break;
					}
		case AVMEDIA_TYPE_SUBTITLE:{
						   sh_sub_t* sh_sub;
						   char type;
						   if(demuxer->sub->id == -1)
						   {
							   demuxer->sub->id = i;
						   }
						   if(codec->codec_id == CODEC_ID_TEXT ||
								   codec->codec_id == AV_CODEC_ID_SUBRIP)
							   type = 't';
						   else if(codec->codec_id == CODEC_ID_MOV_TEXT)
							   type = 'm';
						   else if(codec->codec_id == CODEC_ID_SSA)
							   type = 'a';
						   else if(codec->codec_id == CODEC_ID_DVD_SUBTITLE)
							   type = 'v';
						   else if(codec->codec_id == CODEC_ID_XSUB)
							   type = 'x';
						   else if(codec->codec_id == CODEC_ID_DVB_SUBTITLE)
							   type = 'b';
						   else if(codec->codec_id == CODEC_ID_DVB_TELETEXT)
							   type = 'd';
						   else if(codec->codec_id == CODEC_ID_HDMV_PGS_SUBTITLE)
							   type = 'p';
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(54, 14, 100)
						   else if(codec->codec_id == CODEC_ID_EIA_608)
							   type = 'c';
#endif
						   else if(codec->codec_tag == MKTAG('c', '6', '0', '8'))
							   type = 'c';
						   else
							   break;
						   sh_sub = new_sh_sub_sid(demuxer, i, priv->sub_streams, lang ? lang->value : NULL);
						   if(!sh_sub) break;
						   stream_type = "subtitle";
						   priv->sstreams[priv->sub_streams] = i;
						   sh_sub->type = type;
						   if (codec->extradata_size) {
							   sh_sub->extradata = malloc(codec->extradata_size);
							   memcpy(sh_sub->extradata, codec->extradata, codec->extradata_size);
							   sh_sub->extradata_len = codec->extradata_size;
						   }
						   if (title && title->value)
							   mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_SID_%d_NAME=%s\n", priv->sub_streams, title->value);
						   if (st->disposition & AV_DISPOSITION_DEFAULT)
							   sh_sub->default_track = 1;
						   stream_id = priv->sub_streams++;
						   break;
					   }
		case AVMEDIA_TYPE_ATTACHMENT:{
						     if (st->codec->codec_id == CODEC_ID_TTF) {
							     AVDictionaryEntry *fnametag = av_dict_get(st->metadata, "filename", NULL, 0);
							     demuxer_add_attachment(demuxer, fnametag ? fnametag->value : NULL,
									     "application/x-truetype-font",
									     codec->extradata, codec->extradata_size);
						     }
						     break;
					     }
		default:
					     st->discard= AVDISCARD_ALL;
	}
	if (stream_type) {
		AVCodec *avc = avcodec_find_decoder(codec->codec_id);
		const char *codec_name = avc ? avc->name : "unknown";
		if (!avc && *stream_type == 's' && demuxer->s_streams[i])
			codec_name = sh_sub_type2str(((sh_sub_t *)demuxer->s_streams[i])->type);
		mp_msg(MSGT_DEMUX, MSGL_INFO, "[lavf] stream %d: %s (%s), -%cid %d", i, stream_type, codec_name, *stream_type, stream_id);
		if (lang && lang->value && *stream_type != 'v')
			mp_msg(MSGT_DEMUX, MSGL_INFO, ", -%clang %s", *stream_type, lang->value);
		if (title && title->value)
		{
			mp_msg(MSGT_DEMUX, MSGL_INFO, ", %s", title->value);
			printf( "[%s]----------- title->value: %s",__func__, title->value);
		}
		if(((lang && lang->value) || (title && title->value) )&& (*stream_type == 's' )
				&&((0 == strcmp(codec_name,  "ass")) ||(0 == strcmp(codec_name,  "ssa")) 
					||(0 == strcmp(codec_name,  "text")) ||(0 == strcmp(codec_name,  "srt"))))          
		{
			demuxer->subt_info.subtitle[demuxer->subt_info.cnt].id = i;
			memcpy(demuxer->subt_info.subtitle[demuxer->subt_info.cnt].code, codec_name, DEMUX_SUBTITLE_LEN);

			if(lang && lang->value)
				memcpy(demuxer->subt_info.subtitle[demuxer->subt_info.cnt].lang, lang->value, DEMUX_SUBTITLE_LEN);
			if(title && title->value)
			{
				memcpy(demuxer->subt_info.subtitle[demuxer->subt_info.cnt].title, title->value, DEMUX_SUBTITLE_LEN);
			}
			else
			{
				memcpy(demuxer->subt_info.subtitle[demuxer->subt_info.cnt].title, "unknown", 7);
			}
			//printf("\n   [%d] [%s] [%s]  [%s]   ",demuxer->subt_info.subtitle[demuxer->subt_info.cnt].id,demuxer->subt_info.subtitle[demuxer->subt_info.cnt].code,demuxer->subt_info.subtitle[demuxer->subt_info.cnt].lang,demuxer->subt_info.subtitle[demuxer->subt_info.cnt].title);

			demuxer->subt_info.cnt++;
			//  printf("\n  %s    %d   %s\n",__func__,__LINE__,codec_name);
		}
		mp_msg(MSGT_DEMUX, MSGL_INFO, "\n");
	}
}
extern int g_is_live_broadcast;
static demuxer_t* demux_open_lavf(demuxer_t *demuxer){
	AVFormatContext *avfc;
	AVDictionaryEntry *t = NULL;
	lavf_priv_t *priv= demuxer->priv;
	int i,ret;
	char mp_filename[4096]="mp:";

#ifdef USE_PB_FIFO	
       FILE_SEQ_T * p_seq = file_seq_get_instance();
#endif      

	//OS_PRINTF("[%s]---start![%d %d %d %d %d %d]\n",__func__,opt_cryptokey,user_correct_pts,index_mode,opt_probesize,opt_analyzeduration,opt_avopt);
	stream_seek(demuxer->stream, 0);

      memset(&ff_mpegts_pmt,0,sizeof(ff_pmt_id));
      if(special_ts_vpid != -1)
        ff_mpegts_pmt.vpid = special_ts_vpid;
      if(special_ts_apid != -1)
        ff_mpegts_pmt.apid = special_ts_apid;
      
	avfc = avformat_alloc_context();

	if (opt_cryptokey)
		parse_cryptokey(avfc, opt_cryptokey);
	if (user_correct_pts != 0)
		avfc->flags |= AVFMT_FLAG_GENPTS;
	if (index_mode == 0)
		avfc->flags |= AVFMT_FLAG_IGNIDX;

	if(opt_probesize) {
		if (av_opt_set_int(avfc, "probesize", opt_probesize, 0) < 0)
			mp_msg(MSGT_HEADER,MSGL_ERR, "demux_lavf, couldn't set option probesize to %u\n", opt_probesize);
	}
	if(opt_analyzeduration) {
		if (av_opt_set_int(avfc, "analyzeduration", opt_analyzeduration * AV_TIME_BASE, 0) < 0)
			mp_msg(MSGT_HEADER,MSGL_ERR, "demux_lavf, couldn't set option analyzeduration to %u\n", opt_analyzeduration);
	}

	if(opt_avopt){
		if(parse_avopts(avfc, opt_avopt) < 0){
			mp_msg(MSGT_HEADER,MSGL_ERR, "Your options /%s/ look like gibberish to me pal\n", opt_avopt);
              demuxer->stream->end_pos = 0;
			return NULL;
		}
	}

	if(demuxer->stream->url) {
            ////add by libin, 20150119 fix bug55361, rtmp need probesize to get codec id
             if(!strncmp(demuxer->stream->url, "rtmp://", 7) && !opt_probesize){
                unsigned int probesize = 1024*1024;
                if (av_opt_set_int(avfc, "probesize", probesize, 0) < 0)
			mp_msg(MSGT_HEADER,MSGL_ERR, "demux_lavf, couldn't set option probesize to %u\n", probesize);
             }
             
		if (!strncmp(demuxer->stream->url, "ffmpeg://rtsp:", 14))
			av_strlcpy(mp_filename, demuxer->stream->url + 9, sizeof(mp_filename));
#ifdef USE_PB_FIFO	
	       else if(strlen(p_seq->hls_parser.m3u8_ur.url) != 0){

                      av_strlcat(mp_filename, p_seq->hls_parser.m3u8_ur.url, sizeof(p_seq->hls_parser.m3u8_ur.url));
                      OS_PRINTF("%s url redirect %s \n",__func__,mp_filename);
	       }
#endif	       
		else{
                if(demuxer->stream->streaming_ctrl == NULL){
                        av_strlcat(mp_filename, demuxer->stream->url, sizeof(mp_filename));
                }
                else{
                    if(demuxer->stream->streaming_ctrl->url){
                        if(demuxer->stream->streaming_ctrl->url->url)                     
			      av_strlcat(mp_filename, demuxer->stream->streaming_ctrl->url->url, sizeof(mp_filename));
                        else
                           av_strlcat(mp_filename, demuxer->stream->url, sizeof(mp_filename)); 
                        }
                }
            }
        } else
		av_strlcat(mp_filename, "foobar.dummy", sizeof(mp_filename));

	if (!(priv->avif->flags & AVFMT_NOFILE)) {
		priv->pb = avio_alloc_context(priv->buffer, BIO_BUFFER_SIZE, 0,
				demuxer, mp_read, NULL, mp_seek);
		priv->pb->read_seek = mp_read_seek;
		if (!demuxer->stream->end_pos || (demuxer->stream->flags & MP_STREAM_SEEK) != MP_STREAM_SEEK)
			priv->pb->seekable = 0;
		avfc->pb = priv->pb;
		OS_PRINTF("[%s] %d------priv->pb->seekable: %d, stream->end_pos: %ld, stream->flags: 0x%lx\n",__func__, __LINE__, priv->pb->seekable,demuxer->stream->end_pos,demuxer->stream->flags);
	}

	ret = avformat_open_input(&avfc, mp_filename, priv->avif, NULL);
    //yliu add:for youtube child video
    demuxer->stream->end_pos = 0;
	//OS_PRINTF("[%s]------avformat_open_input ret: %d\n",__func__,ret);
	if(ret<0){
		mp_msg(MSGT_HEADER,MSGL_ERR,"LAVF_header: av_open_input_stream() failed\n");
		return NULL;
	}

	priv->avfc= avfc;
	if(avformat_find_stream_info(avfc, NULL) < 0){
		mp_msg(MSGT_HEADER,MSGL_ERR,"LAVF_header: av_find_stream_info() failed\n");
		OS_PRINTF("[%s]------LAVF_header: av_find_stream_info() failed\n",__func__);
             //yliu modify
            //yliu add
		  //demuxer->stream->end_pos = 0;
       
		//return NULL;
	}
	
       mtos_printk("\n%s %d %s\n",__func__,__LINE__,priv->avif->name);
	   if(strstr(priv->avif->name, "mpegts")&&g_is_live_broadcast==0)
	   	{
	   	 demuxer->stream->end_pos = 0;
	   	return 0x4444;
	   	}
	/* Add metadata. */
	while((t = av_dict_get(avfc->metadata, "", t, AV_DICT_IGNORE_SUFFIX)))
		demux_info_add(demuxer, t->key, t->value);

	for(i=0; i < avfc->nb_chapters; i++) {
		AVChapter *c = avfc->chapters[i];
		uint64_t start = av_rescale_q(c->start, c->time_base, (AVRational){1,1000});
		uint64_t end   = av_rescale_q(c->end, c->time_base, (AVRational){1,1000});
		t = av_dict_get(c->metadata, "title", NULL, 0);
		demuxer_add_chapter(demuxer, t ? t->value : NULL, start, end);
	}
       if(io_isnetworkstream && ff_mpegts_pmt.use_ff_mpegts)
           search_id_in_pmt();
	for(i=0; i<avfc->nb_streams; i++)
		handle_stream(demuxer, avfc, i);
	priv->nb_streams_last = avfc->nb_streams;

	if(avfc->nb_programs) {
		int p;
		for (p = 0; p < avfc->nb_programs; p++) {
			AVProgram *program = avfc->programs[p];
			t = av_dict_get(program->metadata, "title", NULL, 0);
			mp_msg(MSGT_HEADER,MSGL_INFO,"LAVF: Program %d %s\n", program->id, t ? t->value : "");
			mp_msg(MSGT_IDENTIFY, MSGL_V, "PROGRAM_ID=%d\n", program->id);
		}
	}

	mp_msg(MSGT_HEADER,MSGL_V,"LAVF: %d audio and %d video streams found\n",priv->audio_streams,priv->video_streams);
	mp_msg(MSGT_HEADER,MSGL_V,"LAVF: build %d\n", LIBAVFORMAT_BUILD);
	if(!priv->audio_streams) demuxer->audio->id=-2;  // nosound
	//    else if(best_audio > 0 && demuxer->audio->id == -1) demuxer->audio->id=best_audio;
	if(!priv->video_streams){
		if(!priv->audio_streams){
			mp_msg(MSGT_HEADER,MSGL_ERR,"LAVF: no audio or video headers found - broken file?\n");
                //yliu add
			  demuxer->stream->end_pos = 0;
			return NULL;
		}
		demuxer->video->id=-2; // audio-only
	} //else if (best_video > 0 && demuxer->video->id == -1) demuxer->video->id = best_video;

	demuxer->seekable = priv->pb->seekable;

       //for hls vod, seek is support
	if(strstr(priv->avif->name, "hls"))
      {
         extern int g_hls_playmode;
         hls_protocol = 1;
         if(g_hls_playmode == 1)
        {
            demuxer->seekable = 1;
            OS_PRINTF("[%s]this is hls vod ,seek is support \n",__func__);
        }
        else
        {
            demuxer->seekable = 0;
            OS_PRINTF("[%s]this is hls live ,seek is NOT support \n",__func__);
        }
      }

	if(strstr(priv->avif->name, "mp3"))
      {
          //mp3 demuxer has no pts output,
          //so disable sekk
          demuxer->seekable = 0;
          OS_PRINTF("[%s]this is ffmpeg mp3 ,seek is NOT support \n",__func__);
      }

    
    #ifndef __LINUX__
    {
    FILE_SEQ_T * p_seq = file_seq_get_instance();
    if(demuxer->seekable)
        
        p_seq->stream_type = STREAM_VOD;
    mtos_printk("\n%s %d  seekable:%d\n",__func__,__LINE__, p_seq->stream_type);
            }
    #endif
        //yliu add
	  demuxer->stream->end_pos = 0;
	//OS_PRINTF("[%s]-------end",__func__);

      priv->vc1_first_video_frame = 1;
	return demuxer;
}
/**************************vc1 use******************************/
#define VC1_SEQUENCE_HEADER_LEN     36
#define VC1_FRAME_LAYER_HEADER_LEN  8

#define VC1_PROFILE_SIMPLE 0
#define VC1_PROFILE_MAIN 1
#define VC1_PROFILE_COMPLEX 2
#define VC1_PROFILE_ADVANCED 3

#ifndef AV_WL32
#   define AV_WL32(p, d) do {                   \
        ((uint8_t*)(p))[0] = (d);               \
        ((uint8_t*)(p))[1] = (d)>>8;            \
        ((uint8_t*)(p))[2] = (d)>>16;           \
        ((uint8_t*)(p))[3] = (d)>>24;           \
    } while(0)
#endif
#ifndef AV_WB32
#   define AV_WB32(p, d) do {                   \
        ((uint8_t*)(p))[3] = (d);               \
        ((uint8_t*)(p))[2] = (d)>>8;            \
        ((uint8_t*)(p))[1] = (d)>>16;           \
        ((uint8_t*)(p))[0] = (d)>>24;           \
    } while(0)
#endif

typedef struct MTGVC1Context
{
    int num_frames;
    int struct_a_ver_size;
    int struct_a_horz_size;
    int struct_b_byte1_and_hrd_buffer;///level-3bits,cbr-1bits,res1-4bits,hrdbuffer-3bytes
    int struct_b_hrd_rate;
    int struct_b_framerate;
    int struct_c;
}MTGVC1Context;

static int av_grow_packet_align_right(AVPacket *pkt, int grow_by)
{
    void *new_ptr;
    //MT_ASSERT((unsigned)pkt->size <= INT_MAX - FF_INPUT_BUFFER_PADDING_SIZE);
    if (!pkt->size)
        return av_new_packet(pkt, grow_by);
    if ((unsigned)grow_by >
        INT_MAX - (pkt->size + FF_INPUT_BUFFER_PADDING_SIZE))
        return -1;
    new_ptr = av_malloc(pkt->size + grow_by + FF_INPUT_BUFFER_PADDING_SIZE);
    if (!new_ptr)
        return AVERROR(ENOMEM);
    memcpy((char *)new_ptr+grow_by, pkt->data, pkt->size);
    av_free(pkt->data);
    pkt->data  = new_ptr;
    pkt->size += grow_by;
    memset(pkt->data + pkt->size, 0, FF_INPUT_BUFFER_PADDING_SIZE);
    return 0;
}
static void mtg_update_fps(demuxer_t *demux,AVPacket *now_pkt)
{
    lavf_priv_t *priv = demux->priv;
    off_t curr_offset = stream_tell(demux->stream);
    AVPacket tmp;
    int video_frame_cnt = 1;
    sh_video_t* sh_video = (sh_video_t*)(demux->v_streams[demux->video->id]);
    while(1){
        av_read_frame(priv->avfc, &tmp);
        if(tmp.stream_index == demux->video->id)
            video_frame_cnt++;
        if(video_frame_cnt == 5)
            break;
    }
    ///read 5 video frames, and get avg fps
    if(tmp.pts == now_pkt->pts){
        mtos_printk("[%s] read frame can not get valid pts value, and also we can not get fps! set default value 25\n",__func__);
        priv->avfc->streams[demux->video->id]->avg_frame_rate.num = 25;
        priv->avfc->streams[demux->video->id]->avg_frame_rate.den = 1;
        sh_video->fps = 25.0;
    }
    priv->avfc->streams[demux->video->id]->avg_frame_rate.num = (int64_t)(priv->avfc->streams[demux->video->id]->time_base.den) 
        /(((tmp.pts-now_pkt->pts)>>2)*(int64_t)(priv->avfc->streams[demux->video->id]->time_base.num));
    priv->avfc->streams[demux->video->id]->avg_frame_rate.den = 1;
    sh_video->fps = priv->avfc->streams[demux->video->id]->avg_frame_rate.num;

    stream_seek(demux->stream,curr_offset);

    return;
}
static int get_vc1_level(unsigned int profile, unsigned int width, unsigned int height)
{
#define LEVEL_LOW           0
#define LEVEL_MEDIUM      2
#define LEVEL_HIGH          4
    int total_mbs;
    total_mbs = (width*height)>>8;

    if(profile == VC1_PROFILE_SIMPLE)
    {
        if(total_mbs <= 99)//low
            return LEVEL_LOW;
        else if(total_mbs <= 396)//medium
            return LEVEL_MEDIUM;
        else
            return -1;
    }
    else if(profile == VC1_PROFILE_MAIN)
    {
        if(total_mbs <= 396)//low
            return LEVEL_LOW;
        else if(total_mbs <= 1620)//medium
            return LEVEL_MEDIUM;
        else if(total_mbs <= 8192)//high
            return LEVEL_HIGH;
        else
            return -1;
    }
    else
        return -1;
}
static int add_vc1_sequence_header(AVPacket *pkt, AVCodecContext *codec, AVStream *st)
{
    int profile;
    int level;
    MTGVC1Context ctx;
    uint8_t *p = NULL;


    if(codec->extradata_size < 4)
    {
        printf("no enough extra data found !\n");
        return -1;
    }

    profile = codec->extradata[0]>>6;//2bits
    if(profile >= 2)//not simple or main profile
    {
        printf("invalid profile %d\n",profile);
        return -1;
    }    

    if(av_grow_packet_align_right(pkt,VC1_SEQUENCE_HEADER_LEN) < 0)
    {
        printf("realloc packet failed\n");
        return -1;
    }

    ctx.struct_c = codec->extradata[0];
    ctx.struct_c = (ctx.struct_c<<8)+codec->extradata[1];
    ctx.struct_c = (ctx.struct_c<<8)+codec->extradata[2];
    ctx.struct_c = (ctx.struct_c<<8)+codec->extradata[3];

    level = get_vc1_level(profile,codec->width,codec->height);
    if(level < 0)
    {
        printf("invalid level in current profile or invalid profile\n");
        return -1;
    }
    
    ctx.struct_b_byte1_and_hrd_buffer =  (level<<29);
    ctx.struct_b_hrd_rate = 0;
    ctx.struct_b_framerate = st->avg_frame_rate.num/st->avg_frame_rate.den;

    ctx.struct_a_ver_size = codec->height;
    ctx.struct_a_horz_size = codec->width;

    ctx.num_frames = st->duration*st->avg_frame_rate.num/st->avg_frame_rate.den;
    ctx.num_frames |= 0xc5000000;

    p = pkt->data;

    //write num frames
    AV_WL32(p,ctx.num_frames);
    p += 4;

    ///write 0x00000004
    AV_WL32(p,0x4);
    p += 4;

    ///write struct c
    AV_WB32(p,ctx.struct_c);
    p += 4;

    ///write struct a
    AV_WL32(p,ctx.struct_a_ver_size);
    p += 4;
    AV_WL32(p,ctx.struct_a_horz_size);
    p += 4;

    ///write 0x0000000c
    AV_WL32(p,0xc);
    p += 4;

    //write struct b
    AV_WL32(p,ctx.struct_b_byte1_and_hrd_buffer);
    p += 4;
    AV_WL32(p,ctx.struct_b_hrd_rate);
    p += 4;
    AV_WL32(p,ctx.struct_b_framerate);

    return 0;
}
static int add_vc1_frame_header(AVPacket *pkt, AVStream *st)
{
    AVRational refTimebase;
    unsigned int framesize = pkt->size;
    int timestamp;
    uint8_t *p = NULL;

    refTimebase.num = 1;
    refTimebase.den = 1000;
    
    if(av_grow_packet_align_right(pkt,VC1_FRAME_LAYER_HEADER_LEN) < 0)
    {
        printf("realloc packet failed\n");
        return -1;
    }

    p = pkt->data;

    //set key flag
    if(pkt->flags&AV_PKT_FLAG_KEY)
        framesize |= 0x80000000;
    
    //write frame size
    AV_WL32(p,framesize);
    p += 4;

    timestamp = (int)av_rescale_q(pkt->pts,st->time_base,refTimebase);
    //write timestamp
    AV_WL32(p,timestamp);

    return 0;
}
/**************************vc1 use end******************************/
typedef struct H264BSFContext {
	uint8_t  length_size;
	uint8_t  first_idr;
	int      extradata_parsed;
       int add_header;
} H264BSFContext;
static H264BSFContext ctx1;
static int demux_lavf_fill_buffer(demuxer_t *demux, demux_stream_t *dsds) {
	lavf_priv_t *priv = demux->priv;
	AVPacket pkt;
	demux_packet_t *dp;
	demux_stream_t *ds;
	int id;
	//OS_PRINTF("[%s] start start ...\n", __func__);
	mp_msg(MSGT_DEMUX,MSGL_DBG2,"demux_lavf_fill_buffer()\n");
	ctx1.add_header = 1; //linda zhu add, add sps and pps to flv file start pos, in h264_mp4toannexb_filter

	demux->filepos=stream_tell(demux->stream);
#ifdef SEEK_DEBUG
	//if(flag_debug == 1 )
	OS_PRINTF("[%s]---------in: priv->last_pts = %d ms\n",__func__,(int)(priv->last_pts/1000));

#endif

	//for hls ,you dont need return eof
	AVInputFormat *avif = priv->avif;
	if(strstr(avif->name, "hls"))
	{
		int  ret = -1;
             int errorcnt = 0;
		while(ret < 0)
		{
			ret = av_read_frame(priv->avfc, &pkt);
			if(ret < 0)
			{
				OS_PRINTF("av_read_frame flag %d error ret %d\n",priv->avfc->flags,ret);
             
                    if (is_file_seq_exit()) {
                        OS_PRINTF("\n[%s] ---detect stop commond at line %d! \n", __func__, __LINE__);
                         return 0;
                    }
                    if(ret == AVERROR_EOF || ret == AVERROR_PROTOCOL_NOT_FOUND) // for some special stream
                    {
                         return 0;
                         errorcnt ++;
                         if(errorcnt > 15)
                         {
                            //p_seq->is_play_to_end = 1;
                            return 0;
                         }
                    }
                    else
                    {
                         errorcnt = 0;
                    }

		     }
                   if(demux->audio->eof ||demux->video->eof)
                   {
                     if(pkt.data){
                        av_free_packet(&pkt);
                     }
                     OS_PRINTF("audio->eof %d video->eof %d\n",
                        demux->audio->eof ,demux->video->eof);
                     return 0;
                   }
		}
	}
	else
	{
		if(av_read_frame(priv->avfc, &pkt) < 0)
		{
			//OS_PRINTF("av_read_frame flag %d error ret %d\n",priv->avfc->flags,ret);
			return 0;
		}

	}

	//OS_PRINTF("[%s]---------after av_read_frame, pkt.pts: %lld, pkt.dts: %lld\n",__func__,pkt.pts,pkt.dts);
#if 1
	//if (pkt.stream_index == 0) {
	if (pkt.stream_index == demux->video->id) {
		AVBitStreamFilterContext *ctx = (AVBitStreamFilterContext *) malloc(sizeof(AVBitStreamFilterContext));
		AVPacket new_pkt = pkt;
		AVStream *st = priv->avfc->streams[demux->video->id];
		AVCodecContext *codec = st->codec;
		memset(ctx, 0, sizeof(AVBitStreamFilterContext));
		ctx->priv_data = &ctx1;
		new_pkt.data = pkt.data;
		av_bitstream_filter_filter(ctx, codec, NULL, &new_pkt.data, &new_pkt.size, pkt.data, pkt.size, 1);
		free(ctx); 
		if(pkt.data != new_pkt.data)
			av_free_packet(&pkt);
		pkt = new_pkt;
	}
#endif
	// handle any new streams that might have been added
	for (id = priv->nb_streams_last; id < priv->avfc->nb_streams; id++)
		handle_stream(demux, priv->avfc, id);
	priv->nb_streams_last = priv->avfc->nb_streams;

      //vc1 package to firmware
      if(pkt.stream_index == demux->video->id 
            && priv->avfc->streams[demux->video->id]->codec->codec_id == CODEC_ID_WMV3){
          add_vc1_frame_header(&pkt, priv->avfc->streams[demux->video->id]);
          if(priv->vc1_first_video_frame == 1){
            if(priv->avfc->streams[demux->video->id]->avg_frame_rate.den == 0){
                mtg_update_fps(demux,&pkt);
            }
            add_vc1_sequence_header(&pkt, priv->avfc->streams[demux->video->id]->codec, 
                priv->avfc->streams[demux->video->id]);
            priv->vc1_first_video_frame = 0;
          }
      }
      //vc1 package end

	id= pkt.stream_index;

	if(id==demux->audio->id){
		// audio
		ds=demux->audio;
		if(!ds->sh){
			ds->sh=demux->a_streams[id];
			mp_msg(MSGT_DEMUX,MSGL_V,"Auto-selected LAVF audio ID = %d\n",ds->id);
		}
	} else if(id==demux->video->id){
		// video
		ds=demux->video;
		if(!ds->sh){
			ds->sh=demux->v_streams[id];
			mp_msg(MSGT_DEMUX,MSGL_V,"Auto-selected LAVF video ID = %d\n",ds->id);
		}
	} else if(id==demux->sub->id){
		// subtitle
		ds=demux->sub;
		sub_utf8=1;
	} else {
		av_free_packet(&pkt);
		return 1;
	}

	if(pkt.destruct == av_destruct_packet && !CONFIG_MEMALIGN_HACK){
		dp=new_demux_packet(0);
		dp->len=pkt.size;
		dp->buffer=pkt.data;
		pkt.destruct= NULL;
	}else{
		dp=new_demux_packet(pkt.size);
		memcpy(dp->buffer, pkt.data, pkt.size);
		av_free_packet(&pkt);
	}

	if(pkt.pts != AV_NOPTS_VALUE){
		dp->pts=pkt.pts * av_q2d(priv->avfc->streams[id]->time_base);
		priv->last_pts= dp->pts * AV_TIME_BASE;
#ifdef SEEK_DEBUG
		//if(flag_debug == 1 )
		OS_PRINTF("[%s]--------- set : dp->pts = %d ms, pkt.pts: %d\n",__func__,(int)(dp->pts*1000), (int)(pkt.pts*1000));
#endif          
		if(pkt.duration > 0)
			dp->endpts = dp->pts + pkt.duration * av_q2d(priv->avfc->streams[id]->time_base);
		/* subtitle durations are sometimes stored in convergence_duration */
		if(ds == demux->sub && pkt.convergence_duration > 0)
			dp->endpts = dp->pts + pkt.convergence_duration * av_q2d(priv->avfc->streams[id]->time_base);
	}
	//else
		//OS_PRINTF("[%s]--------- pkt.pts == AV_NOPTS_VALUE \n",__func__);
	dp->pos=demux->filepos;
	dp->flags= !!(pkt.flags&AV_PKT_FLAG_KEY);

	// append packet to DS stream:
	ds_add_packet(ds,dp);
#ifdef SEEK_DEBUG
	// if(flag_debug == 1 )
	OS_PRINTF("[%s]---------out : priv->last_pts = %d ms\n",__func__,(int)(priv->last_pts/1000));
#endif    
	return 1;
}

static void demux_seek_lavf(demuxer_t *demuxer, float rel_seek_secs, float audio_delay, int flags){
	lavf_priv_t *priv = demuxer->priv;
	int avsflags = 0;
	mp_msg(MSGT_DEMUX,MSGL_DBG2,"demux_seek_lavf(%p, %f, %f, %d)\n", demuxer, rel_seek_secs, audio_delay, flags);
	//OS_PRINTF("[%s]---------start!\n",__func__);
       ctx1.add_header = 1;
	if (flags & SEEK_ABSOLUTE) {
		priv->last_pts = priv->avfc->start_time != AV_NOPTS_VALUE ?
			priv->avfc->start_time : 0;
	} else {
		if (rel_seek_secs < 0) avsflags = AVSEEK_FLAG_BACKWARD;
	}
	if (flags & SEEK_FACTOR) {
		if (priv->avfc->duration == 0 || priv->avfc->duration == AV_NOPTS_VALUE)
			return;
		priv->last_pts += rel_seek_secs * priv->avfc->duration;
	} else {
		//OS_PRINTF("[%s]---------priv->last_pts = %d ms\n",__func__,(int)(priv->last_pts/1000));
		priv->last_pts += rel_seek_secs * AV_TIME_BASE;
		//OS_PRINTF("[%s]---------seek time: %d, priv->last_pts = %d ms\n",__func__,(int)(rel_seek_secs*1000),(int)(priv->last_pts/1000));
	}
	if (av_seek_frame(priv->avfc, -1, priv->last_pts, avsflags) < 0) {
		avsflags ^= AVSEEK_FLAG_BACKWARD;
		av_seek_frame(priv->avfc, -1, priv->last_pts, avsflags);
	}

	//OS_PRINTF("[%s]---------end!\n",__func__);
}

static int demux_lavf_control(demuxer_t *demuxer, int cmd, void *arg)
{
	lavf_priv_t *priv = demuxer->priv;

	switch (cmd) {
		case DEMUXER_CTRL_CORRECT_PTS:
			return DEMUXER_CTRL_OK;
		case DEMUXER_CTRL_GET_TIME_LENGTH:
			if (priv->avfc->duration == 0 || priv->avfc->duration == AV_NOPTS_VALUE)
				return DEMUXER_CTRL_DONTKNOW;

			*((double *)arg) = (double)priv->avfc->duration / AV_TIME_BASE;
			return DEMUXER_CTRL_OK;

		case DEMUXER_CTRL_GET_PERCENT_POS:
			if (priv->avfc->duration == 0 || priv->avfc->duration == AV_NOPTS_VALUE)
				return DEMUXER_CTRL_DONTKNOW;

			*((int *)arg) = (int)((priv->last_pts - priv->avfc->start_time)*100 / priv->avfc->duration);
			return DEMUXER_CTRL_OK;
		case DEMUXER_CTRL_SWITCH_AUDIO:
		case DEMUXER_CTRL_SWITCH_VIDEO:
			{
				int id = *((int*)arg);
				int newid = -2;
				int i, curridx = -1;
				int nstreams, *pstreams;
				demux_stream_t *ds;

				if(cmd == DEMUXER_CTRL_SWITCH_VIDEO)
				{
					ds = demuxer->video;
					nstreams = priv->video_streams;
					pstreams = priv->vstreams;
				}
				else
				{
					ds = demuxer->audio;
					nstreams = priv->audio_streams;
					pstreams = priv->astreams;
				}
				for(i = 0; i < nstreams; i++)
				{
					if(pstreams[i] == ds->id) //current stream id
					{
						curridx = i;
						break;
					}
				}

				if(id == -2) { // no sound
					i = -1;
				} else if(id == -1) { // next track
					i = (curridx + 2) % (nstreams + 1) - 1;
					if (i >= 0)
						newid = pstreams[i];
				}
				else // select track by id
				{
					if (id >= 0 && id < nstreams) {
						i = id;
						newid = pstreams[i];
					}
				}
				if(i == curridx)
					return DEMUXER_CTRL_NOTIMPL;
				else
				{
					ds_free_packs(ds);
					if(ds->id >= 0)
						priv->avfc->streams[ds->id]->discard = AVDISCARD_ALL;
					*((int*)arg) = ds->id = newid;
					if(newid >= 0)
						priv->avfc->streams[newid]->discard = AVDISCARD_NONE;
					return DEMUXER_CTRL_OK;
				}
			}
		case DEMUXER_CTRL_IDENTIFY_PROGRAM:
			{
				demux_program_t *prog = arg;
				AVProgram *program;
				int p, i;
				int start;

				prog->vid = prog->aid = prog->sid = -2;	//no audio and no video by default
				if(priv->avfc->nb_programs < 1)
					return DEMUXER_CTRL_DONTKNOW;

				if(prog->progid == -1)
				{
					p = 0;
					while(p<priv->avfc->nb_programs && priv->avfc->programs[p]->id != priv->cur_program)
						p++;
					p = (p + 1) % priv->avfc->nb_programs;
				}
				else
				{
					for(i=0; i<priv->avfc->nb_programs; i++)
						if(priv->avfc->programs[i]->id == prog->progid)
							break;
					if(i==priv->avfc->nb_programs)
						return DEMUXER_CTRL_DONTKNOW;
					p = i;
				}
				start = p;
redo:
				program = priv->avfc->programs[p];
				for(i=0; i<program->nb_stream_indexes; i++)
				{
					switch(priv->avfc->streams[program->stream_index[i]]->codec->codec_type)
					{
						case AVMEDIA_TYPE_VIDEO:
							if(prog->vid == -2)
								prog->vid = program->stream_index[i];
							break;
						case AVMEDIA_TYPE_AUDIO:
							if(prog->aid == -2)
								prog->aid = program->stream_index[i];
							break;
						case AVMEDIA_TYPE_SUBTITLE:
							if(prog->sid == -2)
								prog->sid = program->stream_index[i];
							break;
					}
				}
				if (prog->aid >= 0 && prog->aid < MAX_A_STREAMS &&
						demuxer->a_streams[prog->aid]) {
					sh_audio_t *sh = demuxer->a_streams[prog->aid];
					prog->aid = sh->aid;
				} else
					prog->aid = -2;
				if (prog->vid >= 0 && prog->vid < MAX_V_STREAMS &&
						demuxer->v_streams[prog->vid]) {
					sh_video_t *sh = demuxer->v_streams[prog->vid];
					prog->vid = sh->vid;
				} else
					prog->vid = -2;
				if(prog->progid == -1 && prog->vid == -2 && prog->aid == -2)
				{
					p = (p + 1) % priv->avfc->nb_programs;
					if (p == start)
						return DEMUXER_CTRL_DONTKNOW;
					goto redo;
				}
				priv->cur_program = prog->progid = program->id;
				return DEMUXER_CTRL_OK;
			}
		default:
			return DEMUXER_CTRL_NOTIMPL;
	}
}

static void demux_close_lavf(demuxer_t *demuxer)
{
	lavf_priv_t* priv = demuxer->priv;
	memset(&ctx1,0,sizeof(ctx1));
	if (priv){
		if(priv->avfc)
		{
			av_freep(&priv->avfc->key);
			avformat_close_input(&priv->avfc);
		}
		av_freep(&priv->pb);
		if(priv->buffer)
		free(priv->buffer);	
		free(priv); 
		demuxer->priv= NULL;
	}
}


const demuxer_desc_t demuxer_desc_lavf = {
	"libavformat demuxer",
	"lavf",
	"libavformat",
	"Michael Niedermayer",
	"supports many formats, requires libavformat",
	DEMUXER_TYPE_LAVF,
	0, // Check after other demuxer
	lavf_check_file,
	demux_lavf_fill_buffer,
	demux_open_lavf,
	demux_close_lavf,
	demux_seek_lavf,
	demux_lavf_control
};

const demuxer_desc_t demuxer_desc_lavf_preferred = {
	"libavformat preferred demuxer",
	"lavfpref",
	"libavformat",
	"Michael Niedermayer",
	"supports many formats, requires libavformat",
	DEMUXER_TYPE_LAVF_PREFERRED,
	1,
	lavf_check_preferred_file,
	demux_lavf_fill_buffer,
	demux_open_lavf,
	demux_close_lavf,
	demux_seek_lavf,
	demux_lavf_control
};
