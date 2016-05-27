/*
 * DEMUXER v2.5
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

#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#else
#include "mp_func_trans.h"
#endif

#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include "sys_types.h"
#include "sys_define.h"

#include "config.h"
#include "mp_msg.h"
#include "help_mp.h"
#include "m_config.h"
#include "mpcommon.h"

#include "libvo/fastmemcpy.h"

#include "stream/stream.h"
#include "demuxer.h"
#include "stheader.h"
#include "mf.h"
#include "demux_audio.h"

#include "libaf/af_format.h"
#include "libmpcodecs/dec_audio.h"
#include "libmpcodecs/dec_video.h"
#include "libmpcodecs/dec_teletext.h"
#include "sub/ass_mp.h"

#ifdef CONFIG_FFMPEG
#include "libavcodec/avcodec.h"
#if MP_INPUT_BUFFER_PADDING_SIZE < FF_INPUT_BUFFER_PADDING_SIZE
#error MP_INPUT_BUFFER_PADDING_SIZE is too small!
#endif
#include "av_helpers.h"
#endif
#include "libavutil/avstring.h"

#include "sys_define.h"
#include "file_playback_sequence.h"

// This is quite experimental, in particular it will mess up the pts values
// in the queue - on the other hand it might fix some issues like generating
// broken files with mencoder and stream copy.
// Better leave it disabled for now, if we find no use for it this code should
// just be removed again.
#define PARSE_ON_ADD 0
#if  0// defined(DEBUG_FILE_PLAYBACK)	//peacer add
extern int flag_debug;
#endif
extern int while_cnt;
extern int io_isnetworkstream;
//for debug avi_desc
#define NEW_AVI
 inline demux_packet_t* clone_demux_packet(demux_packet_t* pack){
  demux_packet_t* dp=(demux_packet_t*)malloc(sizeof(demux_packet_t));
  while(pack->master) pack=pack->master; // find the master
  memcpy(dp,pack,sizeof(demux_packet_t));
  dp->next=NULL;
  dp->refcount=0;
  dp->master=pack;
  pack->refcount++;
  return dp;
}

inline void free_demux_packet(demux_packet_t* dp){
    //yliu add
    if(dp==NULL)
        return;
  if (dp->master==NULL){  //dp is a master packet
    dp->refcount--;
    if (dp->refcount==0){
      free(dp->buffer);
      free(dp);
    }
    return;
  }
  // dp is a clone:
  free_demux_packet(dp->master);
  free(dp);
}
 inline void resize_demux_packet(demux_packet_t* dp, int len)
{
  if(len > 0)
  {
     dp->buffer=(unsigned char *)realloc(dp->buffer,len + MP_INPUT_BUFFER_PADDING_SIZE);
  }
  else
  {
     free(dp->buffer);
     dp->buffer=NULL;
  }
  dp->len=len;
  if (dp->buffer)
     memset(dp->buffer + len, 0, MP_INPUT_BUFFER_PADDING_SIZE);
  else
     dp->len = 0;
}
 inline demux_packet_t* new_demux_packet(int len){
  demux_packet_t* dp=(demux_packet_t*)malloc(sizeof(demux_packet_t));
  dp->len=len;
  dp->next=NULL;
  dp->pts=MP_NOPTS_VALUE;
  dp->endpts=MP_NOPTS_VALUE;
  dp->stream_pts = MP_NOPTS_VALUE;
  dp->pos=0;
  dp->flags=0;
  dp->refcount=1;
  dp->master=NULL;
  dp->buffer=NULL;
  if (len > 0 && (dp->buffer = (unsigned char *)malloc(len + MP_INPUT_BUFFER_PADDING_SIZE)))
    memset(dp->buffer + len, 0, MP_INPUT_BUFFER_PADDING_SIZE);
  else if (len) {
    // do not even return a valid packet if allocation failed
    free(dp);
    return NULL;
  }
  return dp;
}
   inline void *realloc_struct(void *ptr, size_t nmemb, size_t size) {
  if (nmemb > SIZE_MAX / size) {
    free(ptr);
    return NULL;
  }
  return realloc(ptr, nmemb * size);
}



static void clear_parser(sh_common_t *sh);

// Demuxer list
//extern const demuxer_desc_t demuxer_desc_rawaudio;
//extern const demuxer_desc_t demuxer_desc_rawvideo;
//extern const demuxer_desc_t demuxer_desc_tv;
//extern const demuxer_desc_t demuxer_desc_mf;
#ifdef NEW_AVI
extern const demuxer_desc_t demuxer_desc_avi;
#endif
//extern const demuxer_desc_t demuxer_desc_y4m;
extern const demuxer_desc_t demuxer_desc_asf;
extern const demuxer_desc_t demuxer_desc_real;
//extern const demuxer_desc_t demuxer_desc_smjpeg;
extern const demuxer_desc_t demuxer_desc_matroska;
//extern const demuxer_desc_t demuxer_desc_realaudio;
//extern const demuxer_desc_t demuxer_desc_vqf;
extern const demuxer_desc_t demuxer_desc_mov;
//extern const demuxer_desc_t demuxer_desc_vivo;
//extern const demuxer_desc_t demuxer_desc_fli;
//extern const demuxer_desc_t demuxer_desc_film;
//extern const demuxer_desc_t demuxer_desc_roq;
//extern const demuxer_desc_t demuxer_desc_gif;
//extern const demuxer_desc_t demuxer_desc_ogg;
//extern const demuxer_desc_t demuxer_desc_avs;
//extern const demuxer_desc_t demuxer_desc_pva;
//extern const demuxer_desc_t demuxer_desc_nsv;
extern const demuxer_desc_t demuxer_desc_mpeg_ts;
//extern const demuxer_desc_t demuxer_desc_lmlm4;
extern const demuxer_desc_t demuxer_desc_mpeg_ps;
//extern const demuxer_desc_t demuxer_desc_mpeg_pes;
//extern const demuxer_desc_t demuxer_desc_mpeg_es;
//extern const demuxer_desc_t demuxer_desc_mpeg_gxf;
extern const demuxer_desc_t demuxer_desc_mpeg4_es;
//extern const demuxer_desc_t demuxer_desc_h264_es;
//extern const demuxer_desc_t demuxer_desc_rawdv;
//extern const demuxer_desc_t demuxer_desc_mpc;
extern const demuxer_desc_t demuxer_desc_audio;
//extern const demuxer_desc_t demuxer_desc_xmms;
//extern const demuxer_desc_t demuxer_desc_mpeg_ty;
#if  defined(ENABLE_DEMUX_RTSP) 
extern const demuxer_desc_t demuxer_desc_rtp;
#endif
//extern const demuxer_desc_t demuxer_desc_rtp_nemesi;
extern const demuxer_desc_t demuxer_desc_lavf;
extern const demuxer_desc_t demuxer_desc_lavf_preferred;
//extern const demuxer_desc_t demuxer_desc_aac;
//extern const demuxer_desc_t demuxer_desc_nut;
//extern const demuxer_desc_t demuxer_desc_mng;

/* Please do not add any new demuxers here. If you want to implement a new
 * demuxer, add it to libavformat, except for wrappers around external
 * libraries and demuxers requiring binary support. */

const demuxer_desc_t *const demuxer_list[] = {
	//&demuxer_desc_rawaudio,
	//&demuxer_desc_rawvideo,
#ifdef CONFIG_TV
	//&demuxer_desc_tv,
#endif
	//&demuxer_desc_mf,

#ifdef CONFIG_FFMPEG
	//&demuxer_desc_lavf_preferred,
#endif

#ifdef NEW_AVI
	&demuxer_desc_avi,
#endif
	&demuxer_desc_mpeg_ts,
#if 1//def __LINUX__   
	//&demuxer_desc_lavf_preferred,
	&demuxer_desc_lavf,
#endif   

	//&demuxer_desc_y4m,
	//#ifdef __LINUX__
	//&demuxer_desc_asf,
	//#endif
	//&demuxer_desc_nsv,
	&demuxer_desc_real,
	//&demuxer_desc_smjpeg,
	//&demuxer_desc_matroska,
	//&demuxer_desc_realaudio,
	//&demuxer_desc_vqf,
	//  &demuxer_desc_mov,
	//&demuxer_desc_vivo,
	//&demuxer_desc_fli,
	//&demuxer_desc_film,
	//&demuxer_desc_roq,
#ifdef CONFIG_GIF
	//&demuxer_desc_gif,
#endif
#ifdef CONFIG_OGGVORBIS
	//&demuxer_desc_ogg,
#endif
#ifdef CONFIG_WIN32DLL
	//&demuxer_desc_avs,
#endif
	//&demuxer_desc_pva,
	//&demuxer_desc_lmlm4,
	&demuxer_desc_mpeg_ps,
	//&demuxer_desc_mpeg_pes,
	//&demuxer_desc_mpeg_es,
	//&demuxer_desc_mpeg_gxf,
	&demuxer_desc_mpeg4_es,
	//&demuxer_desc_h264_es,
	&demuxer_desc_audio,
	//&demuxer_desc_mpeg_ty,
#ifdef CONFIG_LIVE555

#if  defined(ENABLE_DEMUX_RTSP)
	&demuxer_desc_rtp,
#endif

#endif
#ifdef CONFIG_LIBNEMESI
	//&demuxer_desc_rtp_nemesi,
#endif
	//  &demuxer_desc_lavf,
#ifdef CONFIG_FFMPEG

#endif
#ifdef CONFIG_MUSEPACK
	//&demuxer_desc_mpc,
#endif
#ifdef CONFIG_LIBDV095
	//&demuxer_desc_rawdv,
#endif
	//&demuxer_desc_aac,
#ifdef CONFIG_LIBNUT
	//&demuxer_desc_nut,
#endif
#ifdef CONFIG_XMMS
	//&demuxer_desc_xmms,
#endif
#ifdef CONFIG_MNG
	//&demuxer_desc_mng,
#endif
	/* Please do not add any new demuxers here. If you want to implement a new
	 * demuxer, add it to libavformat, except for wrappers around external
	 * libraries and demuxers requiring binary support. */
	NULL
};

void free_demuxer_stream(demux_stream_t *ds)
{
	ds_free_packs(ds);
	free(ds);
}

demux_stream_t *new_demuxer_stream(struct demuxer *demuxer, int id)
{
	demux_stream_t *ds = malloc(sizeof(demux_stream_t));
	*ds = (demux_stream_t){
		.id = id,
			.demuxer = demuxer,
			.asf_seq = -1,
	};
	return ds;
}


/**
 * Get demuxer description structure for a given demuxer type
 *
 * @param file_format    type of the demuxer
 * @return               structure for the demuxer, NULL if not found
 */
static const demuxer_desc_t *get_demuxer_desc_from_type(int file_format)
{
	int i;

	for (i = 0; demuxer_list[i]; i++)
		if (file_format == demuxer_list[i]->type)
			return demuxer_list[i];

	return NULL;
}

demuxer_t *alloc_demuxer(stream_t *stream, int type, const char *filename)
{
	demuxer_t *d = calloc(1, sizeof(*d));
	d->stream = stream;
	d->stream_pts = MP_NOPTS_VALUE;
	d->reference_clock = MP_NOPTS_VALUE;
	d->movi_start = stream->start_pos;
	d->movi_end = stream->end_pos;
	d->seekable = 1;
	d->synced = 0;
	d->filepos = -1;
	d->type = type;
	if (type)
		if (!(d->desc = get_demuxer_desc_from_type(type)))
			mp_msg(MSGT_DEMUXER, MSGL_ERR,
					"BUG! Invalid demuxer type in new_demuxer(), "
					"big troubles ahead.\n");
	if (filename) // Filename hack for avs_check_file
		d->filename = strdup(filename);
	return d;
}

demuxer_t *new_demuxer(stream_t *stream, int type, int a_id, int v_id,
		int s_id, char *filename)
{
	OS_PRINTF("[%s]---start, type = %d\n",__func__,type);
	demuxer_t *d = alloc_demuxer(stream, type, filename);
	d->audio = new_demuxer_stream(d, a_id);
	d->video = new_demuxer_stream(d, v_id);
	d->sub = new_demuxer_stream(d, s_id);
	stream->eof = 0;
	stream_seek(stream, stream->start_pos);
	OS_PRINTF("[%s]---end\n",__func__);
	return d;
}

const char *sh_sub_type2str(int type)
{
	switch (type) {
		case 't': return "text";
		case 'm': return "movtext";
		case 'a': return "ass";
		case 'v': return "vobsub";
		case 'x': return "xsub";
		case 'b': return "dvb";
		case 'd': return "dvb-teletext";
		case 'p': return "hdmv pgs";
	}
	return "unknown";
}

sh_sub_t *new_sh_sub_sid(demuxer_t *demuxer, int id, int sid, const char *lang)
{
	if (id > MAX_S_STREAMS - 1 || id < 0) {
		mp_msg(MSGT_DEMUXER, MSGL_WARN,
				"Requested sub stream id overflow (%d > %d)\n", id,
				MAX_S_STREAMS);
		return NULL;
	}
	if (demuxer->s_streams[id])
		mp_msg(MSGT_DEMUXER, MSGL_WARN, "Sub stream %i redefined\n", id);
	else {
		sh_sub_t *sh = calloc(1, sizeof(sh_sub_t));
		demuxer->s_streams[id] = sh;
		sh->sid = sid;
		mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_SUBTITLE_ID=%d\n", sid);
		if (lang && lang[0] && strcmp(lang, "und")) {
			sh->lang = strdup(lang);
			mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_SID_%d_LANG=%s\n", sid, lang);
		}
		if (sid == dvdsub_id) {
			demuxer->sub->id = id;
			demuxer->sub->sh = demuxer->s_streams[id];
		}
	}
	return demuxer->s_streams[id];
}

static void free_sh_sub(sh_sub_t *sh)
{
	mp_msg(MSGT_DEMUXER, MSGL_DBG2, "DEMUXER: freeing sh_sub at %p\n", sh);
	free(sh->extradata);
#ifdef CONFIG_ASS
	if (sh->ass_track)
		ass_free_track(sh->ass_track);
#endif
	free(sh->lang);
//#ifdef CONFIG_FFMPEG
	clear_parser((sh_common_t *)sh);
//#endif
	free(sh);
}

sh_audio_t *new_sh_audio_aid(demuxer_t *demuxer, int id, int aid, const char *lang)
{
	if (id > MAX_A_STREAMS - 1 || id < 0) {
		mp_msg(MSGT_DEMUXER, MSGL_WARN,
				"Requested audio stream id overflow (%d > %d)\n", id,
				MAX_A_STREAMS);
		return NULL;
	}
	if (demuxer->a_streams[id])
		mp_msg(MSGT_DEMUXER, MSGL_WARN, MSGTR_AudioStreamRedefined, id);
	else {
		sh_audio_t *sh = calloc(1, sizeof(sh_audio_t));
		mp_msg(MSGT_DEMUXER, MSGL_V, "==> Found audio stream: %d\n", id);
		demuxer->a_streams[id] = sh;
		sh->aid = aid;
		sh->ds = demuxer->audio;
		// set some defaults
		sh->samplesize = 2;
		sh->sample_format = AF_FORMAT_S16_NE;
		sh->audio_out_minsize = 8192;   /* default size, maybe not enough for Win32/ACM */
		sh->pts = MP_NOPTS_VALUE;
		mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_AUDIO_ID=%d\n", aid);
		if (lang && lang[0] && strcmp(lang, "und")) {
			sh->lang = strdup(lang);
			mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_AID_%d_LANG=%s\n", aid, lang);
		}
		if (aid == audio_id) {
			demuxer->audio->id = id;
			demuxer->audio->sh = demuxer->a_streams[id];
		}
	}
	return demuxer->a_streams[id];
}

void free_sh_audio(demuxer_t *demuxer, int id)
{
	sh_audio_t *sh = demuxer->a_streams[id];
	demuxer->a_streams[id] = NULL;
	mp_msg(MSGT_DEMUXER, MSGL_DBG2, "DEMUXER: freeing sh_audio at %p\n", sh);
	free(sh->wf);
	free(sh->codecdata);
	free(sh->lang);
//#ifdef CONFIG_FFMPEG
	clear_parser((sh_common_t *)sh);
//#endif
	free(sh);
}

sh_video_t *new_sh_video_vid(demuxer_t *demuxer, int id, int vid)
{
	if (id > MAX_V_STREAMS - 1 || id < 0) {
		mp_msg(MSGT_DEMUXER, MSGL_WARN,
				"Requested video stream id overflow (%d > %d)\n", id,
				MAX_V_STREAMS);
		return NULL;
	}
	if (demuxer->v_streams[id])
		mp_msg(MSGT_DEMUXER, MSGL_WARN, MSGTR_VideoStreamRedefined, id);
	else {
		sh_video_t *sh = calloc(1, sizeof(sh_video_t));
		mp_msg(MSGT_DEMUXER, MSGL_V, "==> Found video stream: %d\n", id);
		demuxer->v_streams[id] = sh;
		sh->vid = vid;
		sh->ds = demuxer->video;
		mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_VIDEO_ID=%d\n", vid);
		if (vid == video_id) {
			demuxer->video->id = id;
			demuxer->video->sh = demuxer->v_streams[id];
		}
	}
	return demuxer->v_streams[id];
}

void free_sh_video(sh_video_t *sh)
{
	mp_msg(MSGT_DEMUXER, MSGL_DBG2, "DEMUXER: freeing sh_video at %p\n", sh);
	free(sh->bih);
       if(sh->sh.extra_data)
       {
	    free(sh->sh.extra_data);
           sh->sh.extra_data = NULL;
       }
//#ifdef CONFIG_FFMPEG
	clear_parser((sh_common_t *)sh);
//#endif
       
	free(sh);
}

void free_demuxer(demuxer_t *demuxer)
{
	int i;
	if (!demuxer)
		return;
	mp_msg(MSGT_DEMUXER, MSGL_DBG2, "DEMUXER: freeing %s demuxer at %p\n",
			demuxer->desc->shortdesc, demuxer);
	OS_PRINTF( "[%s] DEMUXER: freeing %s demuxer at %p\n",__func__,demuxer->desc->info, demuxer);	
	if (demuxer->desc->close){
		OS_PRINTF("[%s] -- call demuxer->desc->close(): 0x%x\n",__func__,demuxer->desc->close);
		demuxer->desc->close(demuxer);
	}
	OS_PRINTF("[%s] -- demuxer->desc->type = %d\n",__func__,demuxer->desc->type);
	// Very ugly hack to make it behave like old implementation
	if (demuxer->desc->type == DEMUXER_TYPE_DEMUXERS)
		goto skip_streamfree;
	// free streams:
	for (i = 0; i < MAX_A_STREAMS; i++)
		if (demuxer->a_streams[i])
			free_sh_audio(demuxer, i);
	for (i = 0; i < MAX_V_STREAMS; i++)
		if (demuxer->v_streams[i])
			free_sh_video(demuxer->v_streams[i]);
	for (i = 0; i < MAX_S_STREAMS; i++)
		if (demuxer->s_streams[i])
			free_sh_sub(demuxer->s_streams[i]);
	// free demuxers:
	free_demuxer_stream(demuxer->audio);
	free_demuxer_stream(demuxer->video);
	free_demuxer_stream(demuxer->sub);
skip_streamfree:
	if (demuxer->info) {
		for (i = 0; demuxer->info[i] != NULL; i++)
			free(demuxer->info[i]);
		free(demuxer->info);
	}
      free(demuxer->filename);
      if (demuxer->chapters) {
		for (i = 0; i < demuxer->num_chapters; i++)
			free(demuxer->chapters[i].name);
		free(demuxer->chapters);
	}
	if (demuxer->attachments) {
		for (i = 0; i < demuxer->num_attachments; i++) {
			free(demuxer->attachments[i].name);
			free(demuxer->attachments[i].type);
			free(demuxer->attachments[i].data);
		}
		free(demuxer->attachments);
	}
	if (demuxer->teletext)
		teletext_control(demuxer->teletext, TV_VBI_CONTROL_STOP, NULL);
	free(demuxer);
}

extern FILE_SEQ_T * x_get_cur_instance();
static void ds_add_packet_internal(demux_stream_t *ds, demux_packet_t *dp)
{
	demuxer_t *demux = ds->demuxer;
       if(io_isnetworkstream)
       {
            int ds_v_packs = (demux->video == NULL) ? 0 : demux->video->packs;
            int ds_a_packs = (demux->audio == NULL) ? 0 : demux->audio->packs;
            int ds_v_bytes = (demux->video == NULL) ? 0 : demux->video->bytes;
            int ds_a_bytes = (demux->audio == NULL) ? 0 : demux->audio->bytes;
            int default_size = 3000000;
            FILE_SEQ_T * p_file_seq = x_get_cur_instance();

            if(p_file_seq->video_disp_w >= 1920)
                default_size = 10000000;
                
            if(ds_a_bytes > 3000000)
            {
                mtos_printk("[%s]apks[%d],amem[%d],vpks[%d],vmem[%d]\n",__func__,ds_a_packs,ds_a_bytes,ds_v_packs,ds_v_bytes);
                ds_free_packs(demux->audio);
            }
            if(ds_v_bytes > default_size)
            {
                mtos_printk("[%s]apks[%d],amem[%d],vpks[%d],vmem[%d]\n",__func__,ds_a_packs,ds_a_bytes,ds_v_packs,ds_v_bytes);
                ds_free_packs(demux->video);           
            }
       }
	// append packet to DS stream:
	++ds->packs;
	ds->bytes += dp->len;
	//printf("\n-------------------ds_add_packet_internal----dp->len =%d\n",dp->len);
	if (ds->last) {
		// next packet in stream
		ds->last->next = dp;
		ds->last = dp;
	} else {
		// first packet in stream
		ds->first = ds->last = dp;
	}
	mp_dbg(MSGT_DEMUXER, MSGL_DBG2,
			"DEMUX: Append packet to %s, len=%d  pts=%5.3f  pos=%u  [packs: A=%d V=%d]\n",
			(ds == ds->demuxer->audio) ? "d_audio" : "d_video", dp->len,
			dp->pts, (unsigned int) dp->pos, ds->demuxer->audio->packs,
			ds->demuxer->video->packs);
#if 0			
       OS_PRINTF("--- DEMUX: Append packet to %s, len=%d  pts=%d  pos=%u  [packs: A=%d V=%d] [bytes: A=%d V=%d]\n",
			(ds == ds->demuxer->audio) ? "d_audio" : "d_video", dp->len,
			(int)(dp->pts*1000), (unsigned int) dp->pos, ds->demuxer->audio->packs,
			ds->demuxer->video->packs, ds->demuxer->audio->bytes,
			ds->demuxer->video->bytes);	
#endif			
}

#ifdef CONFIG_FFMPEG
static void allocate_parser(AVCodecContext **avctx, AVCodecParserContext **parser, unsigned format)
{
	enum CodecID codec_id = CODEC_ID_NONE;

	init_avcodec();

	switch (format) {
		case 0x1600:
		case MKTAG('M', 'P', '4', 'A'):
			codec_id = CODEC_ID_AAC;
			break;
		case 0x1602:
		case MKTAG('M', 'P', '4', 'L'):
			codec_id = CODEC_ID_AAC_LATM;
			break;
		case 0x2000:
		case 0x332D6361:
		case 0x332D4341:
		case 0x20736D:
		case MKTAG('s', 'a', 'c', '3'):
			codec_id = CODEC_ID_AC3;
			break;
		case MKTAG('d', 'n', 'e', 't'):
			// DNET/byte-swapped AC-3 - there is no parser for that yet
			//codec_id = CODEC_ID_DNET;
			break;
		case MKTAG('E', 'A', 'C', '3'):
		case MKTAG('e', 'c', '-', '3'):
			codec_id = CODEC_ID_EAC3;
			break;
		case 0x2001:
		case 0x86:
		case MKTAG('D', 'T', 'S', ' '):
		case MKTAG('d', 't', 's', ' '):
		case MKTAG('d', 't', 's', 'b'):
		case MKTAG('d', 't', 's', 'c'):
			codec_id = CODEC_ID_DTS;
			break;
		case MKTAG('f', 'L', 'a', 'C'):
			codec_id = CODEC_ID_FLAC;
			break;
		case MKTAG('M', 'L', 'P', ' '):
			codec_id = CODEC_ID_MLP;
			break;
		case 0x55:
		case 0x5500736d:
		case 0x55005354:
		case MKTAG('.', 'm', 'p', '3'):
		case MKTAG('M', 'P', '3', ' '):
		case MKTAG('L', 'A', 'M', 'E'):
			codec_id = CODEC_ID_MP3;
			break;
		case 0x50:
		case 0x5000736d:
		case MKTAG('.', 'm', 'p', '2'):
		case MKTAG('.', 'm', 'p', '1'):
			codec_id = CODEC_ID_MP2;
			break;
		case MKTAG('T', 'R', 'H', 'D'):
			codec_id = CODEC_ID_TRUEHD;
			break;
	}
	if (codec_id != CODEC_ID_NONE) {
		*avctx = avcodec_alloc_context3(NULL);
		if (!*avctx)
			return;
		*parser = av_parser_init(codec_id);
		if (!*parser)
			av_freep(avctx);
	}
}

static void get_parser(sh_common_t *sh, AVCodecContext **avctx, AVCodecParserContext **parser)
{
	*avctx  = NULL;
	*parser = NULL;

	if (!sh || !sh->needs_parsing)
		return;

	*avctx  = sh->avctx;
	*parser = sh->parser;
	if (*parser)
		return;

	allocate_parser(avctx, parser, sh->format);
	sh->avctx  = *avctx;
	sh->parser = *parser;
}

int ds_parse(demux_stream_t *ds, uint8_t **buffer, int *len, double pts, off_t pos)
{
	AVCodecContext *avctx;
	AVCodecParserContext *parser;
	get_parser(ds->sh, &avctx, &parser);
	if (!parser)
		return *len;
	return av_parser_parse2(parser, avctx, buffer, len, *buffer, *len, pts, pts, pos);
}



void ds_clear_parser(demux_stream_t *ds)
{
	if (!ds->sh)
		return;
	clear_parser(ds->sh);
}
#endif
static void clear_parser(sh_common_t *sh)
{
	av_parser_close(sh->parser);
	sh->parser = NULL;
	av_freep(&sh->avctx);
}
void ds_add_packet(demux_stream_t *ds, demux_packet_t *dp)
{
#if PARSE_ON_ADD && defined(CONFIG_FFMPEG)
	int len = dp->len;
	int pos = 0;
	while (len > 0) {
		uint8_t *parsed_start = dp->buffer + pos;
		int parsed_len = len;
		int consumed = ds_parse(ds->sh, &parsed_start, &parsed_len, dp->pts, dp->pos);
		pos += consumed;
		len -= consumed;
		if (parsed_start == dp->buffer && parsed_len == dp->len) {
			ds_add_packet_internal(ds, dp);
		} else if (parsed_len) {
			demux_packet_t *dp2 = new_demux_packet(parsed_len);
			if (!dp2) return;
			dp2->pos = dp->pos;
			dp2->pts = dp->pts; // should be parser->pts but that works badly
			memcpy(dp2->buffer, parsed_start, parsed_len);
			ds_add_packet_internal(ds, dp2);
		}
	}
#else
	//OS_PRINTF("[%s]-----------before ds_add_packet_internal, dp->len = %d \n",__func__,dp->len);
	ds_add_packet_internal(ds, dp);
#endif
}

void ds_read_packet(demux_stream_t *ds, stream_t *stream, int len,
		double pts, off_t pos, int flags)
{
	demux_packet_t *dp = new_demux_packet(len);
	if (!dp) return;
	len = stream_read(stream, dp->buffer, len);
	if(ds == ds->demuxer->video && ds->sh)
	{
		sh_video_t *sh = ds->sh;
             int type = ds->demuxer->type;
		int biComp =0;
             if(sh->bih)
                 biComp = le2me_32(sh->bih->biCompression);

		if((type ==DEMUXER_TYPE_MPEG4_ES )|| (sh->sh.needfilter && (strstr(((char *) &biComp), "avc1")))) {
			demux_packet_t *new_pkt = new_demux_packet(0);
			unsigned char* p_tmp = dp->buffer;
			add_video_header_h264annexb(&(sh->sh),&(new_pkt->buffer), &(new_pkt->len), dp->buffer, dp->len, 1);
			len= new_pkt->len;
			if(dp->buffer != new_pkt->buffer)
			{
				dp->buffer = new_pkt->buffer;
				new_pkt->buffer = p_tmp;
				free_demux_packet(new_pkt);
			}
			else
			{
				new_pkt->buffer = NULL;
				free_demux_packet(new_pkt);
			}
		}
            else if(sh->sh.needfilter == 2)
            {
                     if(!sh->sh.extradata_parsed)
                     {
                         int i;
                        unsigned int state= -1;
                        unsigned char *buf =dp->buffer;
                        int vol_len = 0;
                        for(i=0; i<dp->len; i++){
                            state= (state<<8) | buf[i];
                            if(state == 0x1B3 || state == 0x1B6)
                            {
                                vol_len = i-3;
                                break;
                            }
                        }
                        if(vol_len > 0)
                        {
                            sh->sh.extradata_size = vol_len;
                            sh->sh.extra_data = (unsigned char *)malloc(vol_len);
                            memcpy(sh->sh.extra_data,dp->buffer,vol_len);
                        }
                        sh->sh.extradata_parsed = 1;
					 }
                     if(sh->sh.extradata_size && flags)
                     {
        			demux_packet_t *new_pkt = new_demux_packet(sh->sh.extradata_size + dp->len);
        			unsigned char* p_tmp = dp->buffer;
                            unsigned int ppts=pts*1000;
                            memcpy(new_pkt->buffer,sh->sh.extra_data,sh->sh.extradata_size);
                            memcpy(new_pkt->buffer + sh->sh.extradata_size,p_tmp,dp->len);
        			len = sh->sh.extradata_size + dp->len;
        			if(dp->buffer != new_pkt->buffer)
        			{
        				dp->buffer = new_pkt->buffer;
        				new_pkt->buffer = p_tmp;
        				free_demux_packet(new_pkt);
        			}
			}
		}
	}
	resize_demux_packet(dp, len);
	dp->pts = pts;
	dp->pos = pos;
	dp->flags = flags;
	// append packet to DS stream:
	ds_add_packet(ds, dp);
}



int hls_fps_first = 0;
int hls_update_fps = 0;
int hls_detect_fps = 0;
int has_hls_fps = 0;
int detect_pks_num;
int hls_fps_done = 0;
int hls_protocol = 0;
int revise_fps = 0;
double detect_last_pts;
double detect_start_pts;

void ds_detect_hls_reset()
{
    hls_update_fps = 0;
    hls_detect_fps = 0;
    has_hls_fps = 0;
    detect_pks_num = 0;
    hls_fps_done = 0;
    detect_last_pts = 0;
    detect_start_pts = 0;
    hls_fps_first = 0;
    hls_protocol = 0;
    revise_fps = 0;
}
void ds_detect_hls_fps(demux_packet_t *p)
{
    if (p->len && !hls_fps_done) {
        hls_update_fps = 1;
        if(!has_hls_fps){
            detect_start_pts =  p->pts;
            detect_last_pts = p->pts; 
            has_hls_fps = 1;
            detect_pks_num = 0;
            hls_detect_fps = 0;
        }
        if(p->pts -detect_last_pts > 1.0 || p->pts -detect_last_pts< -1.0){
           has_hls_fps = 0;
           detect_pks_num = 0;
        }
        else{
            detect_pks_num++;
            detect_last_pts = p->pts;
        }
        if(detect_pks_num >=50){
            hls_detect_fps = detect_pks_num/(p->pts - detect_start_pts)+0.5;
        }
        if(detect_pks_num >=600)
            hls_fps_done = 1;
    }
}
// return value:
//     0 = EOF or no stream found or invalid type
//     1 = successfully read a packet

int demux_fill_buffer(demuxer_t *demux, demux_stream_t *ds)
{
	//printf("\n[%s]------------ start!\n",__FUNCTION__);
	// Note: parameter 'ds' can be NULL!
	return demux->desc->fill_buffer(demux, ds);
}
int fifo_fill_buff(demuxer_t *demux, demux_stream_t *ds)
{
    return demux_fill_buffer(demux,ds);
}

// return value:
//     0 = EOF
//     1 = successful
#define MAX_ACCUMULATED_PACKETS 64
int ds_fill_buffer(demux_stream_t *ds)
{
	demuxer_t *demux = ds->demuxer;
	int break_flag=0;
	//OS_PRINTF("[%s] start start ...\n", __func__);
	if (ds->current)
		free_demux_packet(ds->current);
	ds->current = NULL;
	if (mp_msg_test(MSGT_DEMUXER, MSGL_DBG3)) {
		if (ds == demux->audio)
			mp_dbg(MSGT_DEMUXER, MSGL_DBG3,
					"ds_fill_buffer(d_audio) called\n");
		else if (ds == demux->video)
			mp_dbg(MSGT_DEMUXER, MSGL_DBG3,
					"ds_fill_buffer(d_video) called\n");
		else if (ds == demux->sub)
			mp_dbg(MSGT_DEMUXER, MSGL_DBG3, "ds_fill_buffer(d_sub) called\n");
		else
			mp_dbg(MSGT_DEMUXER, MSGL_DBG3,
					"ds_fill_buffer(unknown 0x%X) called\n", (unsigned int) ds);
	}

	while (is_file_seq_exit()==FALSE) {

		if (ds->packs) {
			demux_packet_t *p = ds->first;
			// obviously not yet EOF after all
			ds->eof = 0;
#if 0
			if (demux->reference_clock != MP_NOPTS_VALUE) {
				if (   p->pts != MP_NOPTS_VALUE
						&& p->pts >  demux->reference_clock
						&& ds->packs < MAX_ACCUMULATED_PACKETS) {
					if (demux_fill_buffer(demux, ds))
						continue;
				}
			}
#endif
			// copy useful data:
			ds->buffer = p->buffer;
			ds->buffer_pos = 0;
			ds->buffer_size = p->len;
			ds->pos = p->pos;
			ds->dpos += p->len; // !!!
			ds->pts31bit = p->pts31bit;
			++ds->pack_no;
			if (p->pts != MP_NOPTS_VALUE) {
				ds->pts = p->pts;
				//OS_PRINTF("[%s] ds->pts: %d\n", __func__,(int)(ds->pts*1000));
				ds->pts_bytes = 0;
			}
			ds->pts_bytes += p->len;    // !!!
			if (p->stream_pts != MP_NOPTS_VALUE)
				demux->stream_pts = p->stream_pts;
			ds->flags = p->flags;
			// unlink packet:
			ds->bytes -= p->len;
			ds->current = p;
			ds->first = p->next;
			if (!ds->first)
				ds->last = NULL;
			--ds->packs;
                     return 1;
		}
		// avoid printing the "too many ..." message over and over
		if (ds->eof)
		{  
			break_flag = 1;

			break;
		}




	if (DEMUXER_TYPE_MPEG_TS == (ds->demuxer)->type)
		{
		int max_bytes = (0x300000);
		if (demux->audio->packs >= MAX_PACKS
				|| demux->audio->bytes >= max_bytes) {
			//mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_TooManyAudioInBuffer,
			// demux->audio->packs, demux->audio->bytes);
			//yliu modify :add packet info
			OS_PRINTF( MSGTR_TooManyAudioInBuffer,
					demux->audio->packs, demux->audio->bytes);    
			mp_msg(MSGT_DEMUXER, MSGL_HINT, MSGTR_MaybeNI);
			break_flag = 2;

#if  0//defined(DEBUG_FILE_PLAYBACK)		
			if(flag_debug)
				OS_PRINTF("[%S]-------break_flag=%d, demux->audio->packs=%d,demux->audio->bytes=%d \n",\
						__func__,break_flag,demux->audio->packs,demux->audio->bytes);
#endif
			break;
		}
		if (demux->video->packs >= MAX_PACKS
				|| demux->video->bytes >= max_bytes) {
			// mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_TooManyVideoInBuffer,
			// demux->video->packs, demux->video->bytes);
			//yliu modify :add packet info 
			OS_PRINTF(MSGTR_TooManyVideoInBuffer,
					demux->video->packs, demux->video->bytes);
			mp_msg(MSGT_DEMUXER, MSGL_HINT, MSGTR_MaybeNI);
			int k = 0;
			for(k=0;k<MIN(demux->video->packs,50);k++)
				{
				char *pkt_buf;
			ds_get_packet(demux->video,&pkt_buf);
				}
			continue;
			break_flag = 3;

#if  0// defined(DEBUG_FILE_PLAYBACK)		
			if(flag_debug)
				OS_PRINTF("[%s]-------break_flag=%d, demux->video->packs=%d,demux->video->bytes=%d \n",\
						__func__,break_flag,demux->video->packs,demux->video->bytes);
#endif

			break;
		}
		}

else
{
		int v_a_bytes = 0;
		v_a_bytes = demux->audio->bytes + demux->video->bytes;
		if(v_a_bytes > MAX_PACK_BYTES)
			return 0;
}



		//add by ybc
		if(ds != NULL && ds == ds->demuxer->sub){
		      // OS_PRINTF("[%s]---%d  \n",__func__,__LINE__);
			return 0;
		}

		if (!demux_fill_buffer(demux, ds)) {
#if PARSE_ON_ADD && defined(CONFIG_FFMPEG)
			OS_PRINTF("[%s]---111----test: ds_fill_buffer()->demux_fill_buffer() failed  \n",__func__);
			uint8_t *parsed_start = NULL;
			int parsed_len = 0;
			ds_parse(ds->sh, &parsed_start, &parsed_len, MP_NOPTS_VALUE, 0);
			if (parsed_len) {
				demux_packet_t *dp2 = new_demux_packet(parsed_len);
				if (!dp2) continue;
				dp2->pts = MP_NOPTS_VALUE;
				memcpy(dp2->buffer, parsed_start, parsed_len);
				ds_add_packet_internal(ds, dp2);
				continue;
			}
#endif
			mp_dbg(MSGT_DEMUXER, MSGL_DBG2,
					"ds_fill_buffer()->demux_fill_buffer() failed\n");

			break_flag = 4;
			OS_PRINTF("[%s]----222---test: ds_fill_buffer()->demux_fill_buffer() failed  \n",__func__);

#if   0//defined(DEBUG_FILE_PLAYBACK)			
			if(flag_debug)
				OS_PRINTF("[%s]---333----ds_fill_buffer()->demux_fill_buffer() failed  \n",__func__);
#endif

			break; // EOF
		}
	}
	ds->buffer_pos = ds->buffer_size = 0;
	ds->buffer = NULL;
	mp_msg(MSGT_DEMUXER, MSGL_V,
			"ds_fill_buffer: EOF reached (stream: %s)  \n",
			ds == demux->audio ? "audio" : "video");
	ds->eof = 1;


#if  1// defined(DEBUG_FILE_PLAYBACK)	
	//if(flag_debug)
	OS_PRINTF("[%s]-------ds_fill_buffer: EOF reached (stream: %s), break_flag = %d, audio/video packs=[%d,%d]\n",\
			__func__,ds == demux->audio ? "audio" : (ds==demux->video?"video":"subtitle"),break_flag,demux->audio->packs,demux->video->packs);
#endif

	return 0;
}

int demux_read_data(demux_stream_t *ds, unsigned char *mem, int len)
{
	int x;
	int bytes = 0;
	while (len > 0) {
		x = ds->buffer_size - ds->buffer_pos;
		if (x == 0) {
			if (!ds_fill_buffer(ds))
				return bytes;
		} else {
			if (x > len)
				x = len;
			if (mem)
				fast_memcpy(mem + bytes, &ds->buffer[ds->buffer_pos], x);
			bytes += x;
			len -= x;
			ds->buffer_pos += x;
		}
	}
	return bytes;
}

/**
 * \brief read data until the given 3-byte pattern is encountered, up to maxlen
 * \param mem memory to read data into, may be NULL to discard data
 * \param maxlen maximum number of bytes to read
 * \param read number of bytes actually read
 * \param pattern pattern to search for (lowest 8 bits are ignored)
 * \return whether pattern was found
 */
int demux_pattern_3(demux_stream_t *ds, unsigned char *mem, int maxlen,
		int *read, uint32_t pattern)
{
	register uint32_t head = 0xffffff00;
	register uint32_t pat = pattern & 0xffffff00;
	int total_len = 0;
	do {
		register unsigned char *ds_buf = &ds->buffer[ds->buffer_size];
		int len = ds->buffer_size - ds->buffer_pos;
		register long pos = -len;
		if (unlikely(pos >= 0)) { // buffer is empty
			ds_fill_buffer(ds);
			continue;
		}
		do {
			head |= ds_buf[pos];
			head <<= 8;
		} while (++pos && head != pat);
		len += pos;
		if (total_len + len > maxlen)
			len = maxlen - total_len;
		len = demux_read_data(ds, mem ? &mem[total_len] : NULL, len);
		total_len += len;
	} while ((head != pat || total_len < 3) && total_len < maxlen && !ds->eof);
	if (read)
		*read = total_len;
	return total_len >= 3 && head == pat;
}

void ds_free_packs(demux_stream_t *ds)
{
	demux_packet_t *dp = ds->first;
	while (dp) {
		demux_packet_t *dn = dp->next;
		free_demux_packet(dp);
		dp = dn;
	}
	if (ds->asf_packet) {
		// free unfinished .asf fragments:
		free(ds->asf_packet->buffer);
		free(ds->asf_packet);
		ds->asf_packet = NULL;
	}
	ds->first = ds->last = NULL;
	ds->packs = 0; // !!!!!
	ds->bytes = 0;
	if (ds->current)
		free_demux_packet(ds->current);
	ds->current = NULL;
	ds->buffer = NULL;
	ds->buffer_pos = ds->buffer_size;
	ds->pts = 0;
	ds->pts_bytes = 0;
}

int ds_get_packet(demux_stream_t *ds, unsigned char **start)
{
	int len;
	//OS_PRINTF("[%s] start start ...\n", __func__);
	if (ds->buffer_pos >= ds->buffer_size) {


#if  0//defined(DEBUG_FILE_PLAYBACK)//peacer add
		if(flag_debug)
			OS_PRINTF("[%s] -------while_cnt=%d, ds->buffer_pos = %d, ds->buffer_size = %d\n",__func__,while_cnt,ds->buffer_pos,ds->buffer_size);
#endif

		if (!ds_fill_buffer(ds)) {

#if  0
			if(flag_debug)
				OS_PRINTF("[%s] -------while_cnt=%d, EOF ,return -1\n",__func__,while_cnt);
#endif
			// EOF
			*start = NULL;
			return -1;
		}
	}
	len = ds->buffer_size - ds->buffer_pos;
	*start = &ds->buffer[ds->buffer_pos];
	ds->buffer_pos += len;
	//OS_PRINTF("[%s] end end ...\n", __func__);
	return len;
}

int ds_get_packet_pts(demux_stream_t *ds, unsigned char **start, double *pts)
{
	int len;
	*pts = MP_NOPTS_VALUE;
	len = ds_get_packet(ds, start);
	if (len < 0)
		return len;
	// Return pts unless this read starts from the middle of a packet
	if (len == ds->buffer_pos)
		*pts = ds->current->pts;
	return len;
}

/**
 * Get a subtitle packet. In particular avoid reading the stream.
 * \param pts input: maximum pts value of subtitle packet. NOPTS or NULL for any.
 *            output: start/reference pts of subtitle
 *            May be NULL.
 * \param endpts output: pts for end of display time. May be NULL.
 * \return -1 if no packet is available
 */
int ds_get_packet_sub(demux_stream_t *ds, unsigned char **start,
		double *pts, double *endpts)
{
	double max_pts = MP_NOPTS_VALUE;
	int len;
	*start = NULL;
	// initialize pts
	if (pts) {
		max_pts = *pts;
		*pts    = MP_NOPTS_VALUE;
	}
	if (endpts)
		*endpts = MP_NOPTS_VALUE;
	if (ds->buffer_pos >= ds->buffer_size) {
		if (!ds->packs)
			return -1;  // no sub
		if (!ds_fill_buffer(ds))
			return -1;  // EOF
	}
	// only start of buffer has valid pts
	if (ds->buffer_pos == 0) {
		if (endpts)
			*endpts = ds->current->endpts;
		if (pts) {
			*pts    = ds->current->pts;
			// check if we are too early
			if (*pts != MP_NOPTS_VALUE && max_pts != MP_NOPTS_VALUE &&
					*pts > max_pts)
				return -1;
		}
	}
	len = ds->buffer_size - ds->buffer_pos;
	*start = &ds->buffer[ds->buffer_pos];
	ds->buffer_pos += len;
	return len;
}

double ds_get_next_pts(demux_stream_t *ds)
{
	demuxer_t *demux = ds->demuxer;
	int max_bytes = MAX_PACK_BYTES;
	if (DEMUXER_TYPE_MPEG_TS == (ds->demuxer)->type)
		max_bytes = (0x300000);
	// if we have not read from the "current" packet, consider it
	// as the next, otherwise we never get the pts for the first packet.
	while (!ds->first && (!ds->current || ds->buffer_pos)) {
		if (demux->audio->packs >= MAX_PACKS
				|| demux->audio->bytes >= max_bytes) {
			mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_TooManyAudioInBuffer,
					demux->audio->packs, demux->audio->bytes);
			mp_msg(MSGT_DEMUXER, MSGL_HINT, MSGTR_MaybeNI);
			return MP_NOPTS_VALUE;
		}
		if (demux->video->packs >= MAX_PACKS
				|| demux->video->bytes >= max_bytes) {
			mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_TooManyVideoInBuffer,
					demux->video->packs, demux->video->bytes);
			mp_msg(MSGT_DEMUXER, MSGL_HINT, MSGTR_MaybeNI);
			return MP_NOPTS_VALUE;
		}
		if (!demux_fill_buffer(demux, ds))
			return MP_NOPTS_VALUE;
	}
	// take pts from "current" if we never read from it.
	if (ds->current && !ds->buffer_pos)
		return ds->current->pts;
	return ds->first->pts;
}

// ====================================================================

void demuxer_help(void)
{
	int i;

	mp_msg(MSGT_DEMUXER, MSGL_INFO, "Available demuxers:\n");
	mp_msg(MSGT_DEMUXER, MSGL_INFO, " demuxer:  type  info:  (comment)\n");
	mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_DEMUXERS\n");
	for (i = 0; demuxer_list[i]; i++) {
		if (demuxer_list[i]->type > DEMUXER_TYPE_MAX)   // Don't display special demuxers
			continue;
		if (demuxer_list[i]->comment && strlen(demuxer_list[i]->comment))
			mp_msg(MSGT_DEMUXER, MSGL_INFO, "%10s  %2d   %s (%s)\n",
					demuxer_list[i]->name, demuxer_list[i]->type,
					demuxer_list[i]->info, demuxer_list[i]->comment);
		else
			mp_msg(MSGT_DEMUXER, MSGL_INFO, "%10s  %2d   %s\n",
					demuxer_list[i]->name, demuxer_list[i]->type,
					demuxer_list[i]->info);
	}
}


/**
 * Get demuxer type for a given demuxer name
 *
 * @param demuxer_name    string with demuxer name of demuxer number
 * @param force           will be set if demuxer should be forced.
 *                        May be NULL.
 * @return                DEMUXER_TYPE_xxx, -1 if error or not found
 */
int get_demuxer_type_from_name(char *demuxer_name, int *force)
{
	int i;
	long type_int;
	char *endptr;

	if (!demuxer_name || !demuxer_name[0])
		return DEMUXER_TYPE_UNKNOWN;
	if (force)
		*force = demuxer_name[0] == '+';
	if (demuxer_name[0] == '+')
		demuxer_name = &demuxer_name[1];
	for (i = 0; demuxer_list[i]; i++) {
		if (demuxer_list[i]->type > DEMUXER_TYPE_MAX)   // Can't select special demuxers from commandline
			continue;
		if (strcmp(demuxer_name, demuxer_list[i]->name) == 0)
			return demuxer_list[i]->type;
	}

	// No match found, try to parse name as an integer (demuxer number)
	type_int = strtol(demuxer_name, &endptr, 0);
	if (*endptr)  // Conversion failed
		return -1;
	if ((type_int > 0) && (type_int <= DEMUXER_TYPE_MAX))
		return (int) type_int;

	return -1;
}

static int demux_music_check(char *filename)
{
    //is mp3 or wav ?  ,fix by ybc
    int filename_len;
    char *p_type = NULL;

    if(filename == NULL)
    {
        return FALSE;
    }
    
    filename_len = strlen(filename);

    if(filename_len > 4)
    {
          p_type = filename + filename_len - 4;

          if((strcmp(p_type,".mp3") == 0) || (strcmp(p_type,".MP3") == 0) \
             || (strcmp(p_type,".wav") == 0) || (strcmp(p_type,".WAV") == 0))
          {
                return TRUE;
          }
    }
    
    return FALSE;
}

int extension_parsing = 1; // 0=off 1=mixed (used only for unstable formats)

int correct_pts = 0;
int user_correct_pts = -1;

/*
NOTE : Several demuxers may be opened at the same time so
demuxers should NEVER rely on an external var to enable them
self. If a demuxer can't do any auto-detection it should only use
file_format. The user can explicitly set file_format with the -demuxer
option so there is really no need for another extra var.
For convenience an option can be added to set file_format directly
to the right type (ex: rawaudio,rawvideo).
Also the stream can override the file_format so a demuxer which rely
on a special stream type can set file_format at the stream level
(ex: tv,mf).
 */

static demuxer_t *demux_open_stream(stream_t *stream, int file_format,
		int force, int audio_id, int video_id,
		int dvdsub_id, char *filename)
{
	demuxer_t *demuxer = NULL;

	sh_video_t *sh_video = NULL;

	const demuxer_desc_t *demuxer_desc;
	int fformat = 0;
	int i;
//       int raw_format = file_format;

	OS_PRINTF("[%s] ----- para. in: stream=%x, file_format=%d, force=%d, audio_id=%d, video_id=%d,dvdsub_id=%d",\
			__func__,stream,file_format,force,audio_id,video_id, dvdsub_id);
	if(filename)
		OS_PRINTF(" filename=%s\n",filename);
       else
       	OS_PRINTF("filename=NULL!\n");
	
		//yliu add
		// for identifying net stream
		//sometime in file stream, this var maybe (STREAMTYPE_STREAM),but not found now
		if(file_format ==0)
			{
	if(filename && strstr(filename,"rtsp://"))
       {   
   		 file_format = DEMUXER_TYPE_RTP;
       }
       else if((file_format == DEMUXER_TYPE_AUDIO) ||strstr(filename,"udp://") || (filename && demux_music_check(filename)))
       {
               file_format = DEMUXER_TYPE_AUDIO;//DEMUXER_TYPE_LAVF;//
       }
	else if(stream->type == STREAMTYPE_STREAM)
	{
		file_format = DEMUXER_TYPE_LAVF;
			
	}
			}
	//file_format = DEMUXER_TYPE_LAVF;
	// If somebody requested a demuxer check it
	if (file_format) 
	{
		if ((demuxer_desc = get_demuxer_desc_from_type(file_format))) {
			demuxer = new_demuxer(stream, demuxer_desc->type, audio_id,
					video_id, dvdsub_id, filename);
			if (demuxer_desc->check_file)
				fformat = demuxer_desc->check_file(demuxer);
			if (force || !demuxer_desc->check_file)
				fformat = demuxer_desc->type;

			OS_PRINTF("[%s] ----- demuxer_desc->type=%d\n",__func__,fformat);
                      demuxer_t *demux2 = demuxer;
			if (fformat != 0) {
				if (fformat == demuxer_desc->type) {
					
					// Move messages to demuxer detection code?
					mp_msg(MSGT_DEMUXER, MSGL_INFO,
							MSGTR_Detected_XXX_FileFormat,
							demuxer_desc->shortdesc);
					file_format = fformat;
					if (!demuxer->desc->open
							|| ((demux2 = demuxer->desc->open(demuxer))!=NULL)&&(demux2!=0x4444)) {
						if(demuxer != demux2){
							FILE_SEQ_T * pFileSeq =  file_seq_get_instance();
							pFileSeq->p_demuxer2 = demuxer;
						}
						demuxer = demux2;
						OS_PRINTF("[%s] ----- go to dmx_open\n",__func__);
						goto dmx_open;
					}
				} else {
					// Format changed after check, recurse
					free_demuxer(demuxer);
					OS_PRINTF("[%s] ----- Format changed after check, recurs\n",__func__);
					return demux_open_stream(stream, fformat, force, audio_id,
							video_id, dvdsub_id, filename);
				}
			}
			// Check failed for forced demuxer, quit
			   if(demux2==0x4444)
			   	{
			   	stream->end_pos = demuxer->movi_end;
			   	 free_demuxer(demuxer);
			   	return demux_open_stream(stream, DEMUXER_TYPE_MPEG_TS, force, audio_id,
							video_id, dvdsub_id, filename);
			   	}
				 free_demuxer(demuxer);	
			OS_PRINTF("[%s] ----- Check failed for forced demuxer, quit\n",__func__);
                     if(file_format == fformat)
			    return NULL;
		}
	}


	// Test demuxers with safe file checks
	for (i = 0; (demuxer_desc = demuxer_list[i]); i++) 
	{

		OS_PRINTF("[%s] ----- for i=%d, demuxer_desc->safe_check=%d\n",__func__,i,demuxer_desc->safe_check);
		//yliu add for load exit

		if(is_file_seq_exit())
		{
			OS_PRINTF("\n%s %d demuxer check :%d\n",__func__,__LINE__,i);
			return NULL;
		}

		//

		//doreen add for rtmp open inexist addr, 2013-09-23
#if 1
		if(file_format == DEMUXER_TYPE_LAVF_PREFERRED  && strncmp(filename, "rtmp://", 7) == 0)
		{
			OS_PRINTF("\n[%s] -------- rtmp demux open stream error! i=%d\n",__func__,i);
			return NULL;
		}
#endif


		if (demuxer_desc->safe_check) 
		{
			demuxer = new_demuxer(stream, demuxer_desc->type, audio_id,
					video_id, dvdsub_id, filename);
			OS_PRINTF("[%s]---demuxer_desc->check_file:0x%x\n",__func__,demuxer_desc->check_file);
			if ((fformat = demuxer_desc->check_file(demuxer)) != 0) 
			{
				if (fformat == demuxer_desc->type) 
				{
					demuxer_t *demux2 = demuxer;
					mp_msg(MSGT_DEMUXER, MSGL_INFO,
							MSGTR_Detected_XXX_FileFormat,
							demuxer_desc->shortdesc);
					OS_PRINTF(MSGTR_Detected_XXX_FileFormat, demuxer_desc->shortdesc);
					file_format = fformat;
					OS_PRINTF("[%s]---demuxer_desc->open:0x%x\n",__func__,demuxer_desc->open);
					if (!demuxer->desc->open
							|| (demux2 = demuxer->desc->open(demuxer))) {
						demuxer = demux2;
						goto dmx_open;
					}
				} else {
					if (fformat == DEMUXER_TYPE_PLAYLIST)
						return demuxer; // handled in mplayer.c
					// Format changed after check, recurse
					free_demuxer(demuxer);
					demuxer = demux_open_stream(stream, fformat, force,
							audio_id, video_id,
							dvdsub_id, filename);
					if (demuxer)
						return demuxer; // done!
					file_format = DEMUXER_TYPE_UNKNOWN;
				}
			}
			OS_PRINTF("[%s]----- free_demuxer\n",__func__);
			free_demuxer(demuxer);
			demuxer = NULL;
		}
	}

	
	OS_PRINTF("[%s] ----- out of for: file_format=%d\n",__func__,file_format);

	// If no forced demuxer perform file extension based detection
	// Ok. We're over the stable detectable fileformats, the next ones are
	// a bit fuzzy. So by default (extension_parsing==1) try extension-based
	// detection first:
	if (file_format == DEMUXER_TYPE_UNKNOWN && filename
			&& extension_parsing == 1) {
		file_format = demuxer_type_by_filename(filename);
		OS_PRINTF("[%s] ----- after demuxer_type_by_filename: file_format=%d\n",__func__,file_format);
		if (file_format != DEMUXER_TYPE_UNKNOWN) {
			// we like recursion :)
			demuxer = demux_open_stream(stream, file_format, force, audio_id,
					video_id, dvdsub_id, filename);
			if (demuxer)
				return demuxer; // done!
			file_format = DEMUXER_TYPE_UNKNOWN; // continue fuzzy guessing...
			mp_msg(MSGT_DEMUXER, MSGL_V,
					"demuxer: continue fuzzy content-based format guessing...\n");
		}
	}

	
	// Try detection for all other demuxers
	for (i = 0; (demuxer_desc = demuxer_list[i]); i++) 
	{
		if (!demuxer_desc->safe_check && demuxer_desc->check_file) {
			demuxer = new_demuxer(stream, demuxer_desc->type, audio_id,
					video_id, dvdsub_id, filename);
			if ((fformat = demuxer_desc->check_file(demuxer)) != 0) {
				if (fformat == demuxer_desc->type) {
					demuxer_t *demux2 = demuxer;
					mp_msg(MSGT_DEMUXER, MSGL_INFO,
							MSGTR_Detected_XXX_FileFormat,
							demuxer_desc->shortdesc);
					file_format = fformat;
					if (!demuxer->desc->open
							|| (demux2 = demuxer->desc->open(demuxer))) {
						demuxer = demux2;
						goto dmx_open;
					}
				} else {
					if (fformat == DEMUXER_TYPE_PLAYLIST)
						return demuxer; // handled in mplayer.c
					// Format changed after check, recurse
					free_demuxer(demuxer);
					demuxer = demux_open_stream(stream, fformat, force,
							audio_id, video_id,
							dvdsub_id, filename);
					if (demuxer)
						return demuxer; // done!
					file_format = DEMUXER_TYPE_UNKNOWN;
				}
			}
			free_demuxer(demuxer);
			demuxer = NULL;
		}
	}

	return NULL;
	//====== File format recognized, set up these for compatibility: =========
dmx_open:

	OS_PRINTF("[%s]------dmx_open: file_format = %d\n",__func__, file_format);
	demuxer->file_format = file_format;

	if ((sh_video = demuxer->video->sh) && sh_video->bih) {
		int biComp = le2me_32(sh_video->bih->biCompression);
		mp_msg(MSGT_DEMUX, MSGL_INFO,
				"VIDEO:  [%.4s]  %dx%d  %dbpp  %5.3f fps  %5.1f kbps (%4.1f kbyte/s)\n",
				(char *) &biComp, sh_video->bih->biWidth,
				sh_video->bih->biHeight, sh_video->bih->biBitCount,
				sh_video->fps, sh_video->i_bps * 0.008f,
				sh_video->i_bps / 1024.0f);
	}
#ifdef CONFIG_ASS
	if (ass_enabled && ass_library) {
		for (i = 0; i < MAX_S_STREAMS; ++i) {
			sh_sub_t *sh = demuxer->s_streams[i];
			if (sh && sh->type == 'a') {
				sh->ass_track = ass_new_track(ass_library);
				if (sh->ass_track && sh->extradata)
					ass_process_codec_private(sh->ass_track, sh->extradata,
							sh->extradata_len);
			} else if (sh && sh->type != 'v')
				sh->ass_track = ass_default_track(ass_library);
		}
	}
#endif
	return demuxer;
}

char *audio_stream = NULL;
char *sub_stream = NULL;
int audio_stream_cache = 0;

char *demuxer_name = NULL;       // parameter from -demuxer
char *audio_demuxer_name = NULL; // parameter from -audio-demuxer
char *sub_demuxer_name = NULL;   // parameter from -sub-demuxer

demuxer_t *demux_open(stream_t *vs, int file_format, int audio_id,
		int video_id, int dvdsub_id, char *filename)
{
	stream_t *as = NULL, *ss = NULL;
	demuxer_t *vd, *ad = NULL, *sd = NULL;
	demuxer_t *res;
	int afmt = DEMUXER_TYPE_UNKNOWN, sfmt = DEMUXER_TYPE_UNKNOWN;
	int demuxer_type;
	int audio_demuxer_type = 0, sub_demuxer_type = 0;
	int demuxer_force = 0, audio_demuxer_force = 0, sub_demuxer_force = 0;

	OS_PRINTF("[%s] --xxxxx---start!file_format:%d\n",__func__,file_format);
	OS_PRINTF("[%s] -----demuxer_name=%x, audio_demuxer_name=%x, sub_demuxer_name=%x\n",\
			__func__,demuxer_name,audio_demuxer_name,sub_demuxer_name);

	if ((demuxer_type =
				get_demuxer_type_from_name(demuxer_name, &demuxer_force)) < 0) {
		mp_msg(MSGT_DEMUXER, MSGL_ERR, "-demuxer %s does not exist.\n",
				demuxer_name);
		OS_PRINTF("[%s] ----- -demuxer %s does not exist.\n",__func__,demuxer_name);	
	}

	if ((audio_demuxer_type =
				get_demuxer_type_from_name(audio_demuxer_name,
					&audio_demuxer_force)) < 0) {
		mp_msg(MSGT_DEMUXER, MSGL_ERR, "-audio-demuxer %s does not exist.\n",
				audio_demuxer_name);
		OS_PRINTF("[%s] ----- -audio-demuxer %s does not exist.\n",__func__,audio_demuxer_name);
	}

	if ((sub_demuxer_type =
				get_demuxer_type_from_name(sub_demuxer_name,
					&sub_demuxer_force)) < 0) {
		mp_msg(MSGT_DEMUXER, MSGL_ERR, "-sub-demuxer %s does not exist.\n",
				sub_demuxer_name);
		OS_PRINTF("[%s] ----- -sub-demuxer %s does not exist.\n",__func__,sub_demuxer_name);
	}

	OS_PRINTF("[%s] ----- demuxer_type=%d, audio_demuxer_type=%d, sub_demuxer_type=%d\n",__func__,demuxer_type,audio_demuxer_type,sub_demuxer_type);
	OS_PRINTF("[%s] ----- audio_stream=%x\n",__func__,audio_stream);

	if (audio_stream) {
		as = open_stream(audio_stream, 0, &afmt);
		if (!as) {
			mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_CannotOpenAudioStream,
					audio_stream);
			OS_PRINTF("[%s] ----- Can not Open Audio Stream\n",__func__);
			return NULL;
		}
#if 0
		if (audio_stream_cache) {
			if (!stream_enable_cache
					(as, audio_stream_cache * 1024,
					 audio_stream_cache * 1024 * (stream_cache_min_percent /
						 100.0),
					 audio_stream_cache * 1024 * (stream_cache_seek_min_percent /
						 100.0))) {
				free_stream(as);
				mp_msg(MSGT_DEMUXER, MSGL_ERR,
						"Can't enable audio stream cache\n");
				return NULL;
			}
		}
#endif
	}

	OS_PRINTF("[%s] ----- sub_stream=%x\n",__func__,sub_stream);
	if (sub_stream) {
		ss = open_stream(sub_stream, 0, &sfmt);
		if (!ss) {
			mp_msg(MSGT_DEMUXER, MSGL_ERR, MSGTR_CannotOpenSubtitlesStream,
					sub_stream);
			return NULL;
		}
	}

	vd = demux_open_stream(vs, demuxer_type ? demuxer_type : file_format,
			demuxer_force, audio_stream ? -2 : audio_id,
			video_id, sub_stream ? -2 : dvdsub_id, filename);
	OS_PRINTF("[%s] ----- out of demux_open_stream, vd = %p\n",__func__,vd);
	if (!vd) {
		if (as)
			free_stream(as);
		if (ss)
			free_stream(ss);
		OS_PRINTF("[%s] ----- vd = %p, retrun NULL\n",__func__,vd);  
		return NULL;
	}
	if (as) {
		ad = demux_open_stream(as,
				audio_demuxer_type ? audio_demuxer_type : afmt,
				audio_demuxer_force, audio_id, -2, -2,
				audio_stream);
		if (!ad) {
			mp_msg(MSGT_DEMUXER, MSGL_WARN, MSGTR_OpeningAudioDemuxerFailed,
					audio_stream);
			free_stream(as);
		} else if (ad->audio->sh
				&& ((sh_audio_t *) ad->audio->sh)->format == 0x55) // MP3
			hr_mp3_seek = 1;    // Enable high res seeking
	}
	if (ss) {
		sd = demux_open_stream(ss, sub_demuxer_type ? sub_demuxer_type : sfmt,
				sub_demuxer_force, -2, -2, dvdsub_id,
				sub_stream);
		if (!sd) {
			mp_msg(MSGT_DEMUXER, MSGL_WARN,
					MSGTR_OpeningSubtitlesDemuxerFailed, sub_stream);
			free_stream(ss);
		}
	}

	if (ad && sd)
		res = new_demuxers_demuxer(vd, ad, sd);
	else if (ad)
		res = new_demuxers_demuxer(vd, ad, vd);
	else if (sd)
		res = new_demuxers_demuxer(vd, vd, sd);
	else
		res = vd;

	correct_pts = user_correct_pts;
	if (correct_pts < 0)
		correct_pts = !force_fps && demux_control(res, DEMUXER_CTRL_CORRECT_PTS, NULL)
			== DEMUXER_CTRL_OK;

	OS_PRINTF("[%s] --res = %p, retrun \n",__func__,res);  
	return res;
}

/**
 * Do necessary reinitialization after e.g. a seek.
 * Do _not_ call ds_fill_buffer between the seek and this, it breaks at least
 * seeking with ASF demuxer.
 */
static void demux_resync(demuxer_t *demuxer)
{
	sh_video_t *sh_video = demuxer->video->sh;
	sh_audio_t *sh_audio = demuxer->audio->sh;
	demux_control(demuxer, DEMUXER_CTRL_RESYNC, NULL);
	if (sh_video) {
		resync_video_stream(sh_video);
	}
	if (sh_audio) {
		resync_audio_stream(sh_audio);
	}
}

void demux_flush(demuxer_t *demuxer)
{
#if PARSE_ON_ADD
	ds_clear_parser(demuxer->video);
	ds_clear_parser(demuxer->audio);
	ds_clear_parser(demuxer->sub);
#endif
	ds_free_packs(demuxer->video);
	ds_free_packs(demuxer->audio);
	ds_free_packs(demuxer->sub);
}

int demux_seek(demuxer_t *demuxer, float rel_seek_secs, float audio_delay,
		int flags)
{
	double tmp = 0;
	double pts;

	if (!demuxer->seekable) {
		if (demuxer->file_format == DEMUXER_TYPE_AVI)
			mp_msg(MSGT_SEEK, MSGL_WARN, MSGTR_CantSeekRawAVI);
#ifdef CONFIG_TV
		else if (demuxer->file_format == DEMUXER_TYPE_TV)
			mp_msg(MSGT_SEEK, MSGL_WARN, MSGTR_TVInputNotSeekable);
#endif
		else
			mp_msg(MSGT_SEEK, MSGL_WARN, MSGTR_CantSeekFile);
		return 0;
	}

	demux_flush(demuxer);

	demuxer->stream->eof = 0;
	demuxer->video->eof = 0;
	demuxer->audio->eof = 0;
	demuxer->sub->eof = 0;

	if (flags & SEEK_ABSOLUTE)
		pts = 0.0f;
	else {
		if (demuxer->stream_pts == MP_NOPTS_VALUE)
			goto dmx_seek;
		pts = demuxer->stream_pts;
	}

	if (flags & SEEK_FACTOR) {
		if (stream_control(demuxer->stream, STREAM_CTRL_GET_TIME_LENGTH, &tmp)
				== STREAM_UNSUPPORTED)
			goto dmx_seek;
		pts += tmp * rel_seek_secs;
	} else
		pts += rel_seek_secs;

	if (stream_control(demuxer->stream, STREAM_CTRL_SEEK_TO_TIME, &pts) !=
			STREAM_UNSUPPORTED) {
		demux_resync(demuxer);
		return 1;
	}

dmx_seek:
	if (demuxer->desc->seek)
		demuxer->desc->seek(demuxer, rel_seek_secs, audio_delay, flags);

	demux_resync(demuxer);

	return 1;
}

int demux_info_add(demuxer_t *demuxer, const char *opt, const char *param)
{
	char **info = demuxer->info;
	int n = 0;


	for (n = 0; info && info[2 * n] != NULL; n++) {
		if (!strcasecmp(opt, info[2 * n])) {
			if (!strcmp(param, info[2 * n + 1])) {
				mp_msg(MSGT_DEMUX, MSGL_V, "Demuxer info %s set to unchanged value %s\n", opt, param);
				return 0;
			}
			mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_DemuxerInfoChanged, opt,
					param);
			free(info[2 * n + 1]);
			info[2 * n + 1] = strdup(param);
			return 0;
		}
	}

	info = demuxer->info = realloc(info, (2 * (n + 2)) * sizeof(char *));
	info[2 * n] = strdup(opt);
	info[2 * n + 1] = strdup(param);
	memset(&info[2 * (n + 1)], 0, 2 * sizeof(char *));

	return 1;
}

int demux_info_print(demuxer_t *demuxer)
{
	char **info = demuxer->info;
	int n;

	if (!info)
		return 0;

	mp_msg(MSGT_DEMUX, MSGL_INFO, MSGTR_ClipInfo);
	for (n = 0; info[2 * n] != NULL; n++) {
		mp_msg(MSGT_DEMUX, MSGL_INFO, " %s: %s\n", info[2 * n],
				info[2 * n + 1]);
		mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_CLIP_INFO_NAME%d=%s\n", n,
				info[2 * n]);
		mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_CLIP_INFO_VALUE%d=%s\n", n,
				info[2 * n + 1]);
	}
	mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_CLIP_INFO_N=%d\n", n);

	return 0;
}

char *demux_info_get(demuxer_t *demuxer, const char *opt)
{
	int i;
	char **info = demuxer->info;

	for (i = 0; info && info[2 * i] != NULL; i++) {
		if (!strcasecmp(opt, info[2 * i]))
			return info[2 * i + 1];
	}

	return NULL;
}

int demux_control(demuxer_t *demuxer, int cmd, void *arg)
{

	if (demuxer->desc->control)
		return demuxer->desc->control(demuxer, cmd, arg);

	return DEMUXER_CTRL_NOTIMPL;
}



double demuxer_get_time_length(demuxer_t *demuxer)
{
	double get_time_ans;
	sh_video_t *sh_video = demuxer->video->sh;
	sh_audio_t *sh_audio = demuxer->audio->sh;
	// <= 0 means DEMUXER_CTRL_NOTIMPL or DEMUXER_CTRL_DONTKNOW
	if (demux_control
			(demuxer, DEMUXER_CTRL_GET_TIME_LENGTH, (void *) &get_time_ans) <= 0) {
		if (sh_video && sh_video->i_bps && sh_audio && sh_audio->i_bps)
			get_time_ans = (double) (demuxer->movi_end -
					demuxer->movi_start) / (sh_video->i_bps +
						sh_audio->i_bps);
		else if (sh_video && sh_video->i_bps)
			get_time_ans = (double) (demuxer->movi_end -
					demuxer->movi_start) / sh_video->i_bps;
		else if (sh_audio && sh_audio->i_bps)
			get_time_ans = (double) (demuxer->movi_end -
					demuxer->movi_start) / sh_audio->i_bps;
		else
			get_time_ans = 0;
	}
	return get_time_ans;
}

/**
 * \brief demuxer_get_current_time() returns the time of the current play in three possible ways:
 *        either when the stream reader satisfies STREAM_CTRL_GET_CURRENT_TIME (e.g. dvd)
 *        or using sh_video->pts when the former method fails
 *        0 otherwise
 * \return the current play time
 */
double demuxer_get_current_time(demuxer_t *demuxer)
{
	double get_time_ans = 0;
	sh_video_t *sh_video = demuxer->video->sh;
	if (demuxer->stream_pts != MP_NOPTS_VALUE)
		get_time_ans = demuxer->stream_pts;
	else if (sh_video)
		get_time_ans = sh_video->pts;
	return get_time_ans;
}

int demuxer_get_percent_pos(demuxer_t *demuxer)
{
	int ans = 0;
	int res = demux_control(demuxer, DEMUXER_CTRL_GET_PERCENT_POS, &ans);
	int len = (demuxer->movi_end - demuxer->movi_start) / 100;
	if (res <= 0) {
		off_t pos = demuxer->filepos > 0 ? demuxer->filepos : stream_tell(demuxer->stream);
		if (len > 0)
			ans = (pos - demuxer->movi_start) / len;
		else
			ans = 0;
	}
	if (ans < 0)
		ans = 0;
	if (ans > 100)
		ans = 100;
	return ans;
}

int demuxer_switch_audio(demuxer_t *demuxer, int index)
{
	OS_PRINTF("[%s]---------start!\n",__func__);
	OS_PRINTF("[%s]---------audio_id=%d\n",__func__,index);

	int res = demux_control(demuxer, DEMUXER_CTRL_SWITCH_AUDIO, &index);
	OS_PRINTF("[%s]---------demux_control() ret: %d\n",__func__,res);

	if (res == DEMUXER_CTRL_NOTIMPL)
		index = demuxer->audio->id;
	if (demuxer->audio->id >= 0)
		demuxer->audio->sh = demuxer->a_streams[demuxer->audio->id];
	else
		demuxer->audio->sh = NULL;

	OS_PRINTF("[%s]---------demuxer->audio->id=%d\n",__func__,demuxer->audio->id);
	OS_PRINTF("[%s]---------audio_id=%d\n",__func__,index);
	OS_PRINTF("[%s]---------end!\n",__func__);
	return index;
}

int demuxer_switch_video(demuxer_t *demuxer, int index)
{
	int res = demux_control(demuxer, DEMUXER_CTRL_SWITCH_VIDEO, &index);
	if (res == DEMUXER_CTRL_NOTIMPL)
		index = demuxer->video->id;
	if (demuxer->video->id >= 0)
		demuxer->video->sh = demuxer->v_streams[demuxer->video->id];
	else
		demuxer->video->sh = NULL;
	return index;
}

int demuxer_add_attachment(demuxer_t *demuxer, const char *name,
		const char *type, const void *data, size_t size)
{
	if (!(demuxer->num_attachments & 31))
		demuxer->attachments = realloc(demuxer->attachments,
				(demuxer->num_attachments + 32) * sizeof(demux_attachment_t));

	demuxer->attachments[demuxer->num_attachments].name = name ? strdup(name) : NULL;
	demuxer->attachments[demuxer->num_attachments].type = strdup(type);
	demuxer->attachments[demuxer->num_attachments].data = malloc(size);
	memcpy(demuxer->attachments[demuxer->num_attachments].data, data, size);
	demuxer->attachments[demuxer->num_attachments].data_size = size;

	return demuxer->num_attachments++;
}

int demuxer_add_chapter(demuxer_t *demuxer, const char *name, uint64_t start,
		uint64_t end)
{
	if (demuxer->chapters == NULL)
		demuxer->chapters = malloc(32 * sizeof(*demuxer->chapters));
	else if (!(demuxer->num_chapters % 32))
		demuxer->chapters = realloc(demuxer->chapters,
				(demuxer->num_chapters + 32) *
				sizeof(*demuxer->chapters));

	demuxer->chapters[demuxer->num_chapters].start = start;
	demuxer->chapters[demuxer->num_chapters].end = end;
	demuxer->chapters[demuxer->num_chapters].name = strdup(name ? name : MSGTR_Unknown);

	mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_CHAPTER_ID=%d\n", demuxer->num_chapters);
	mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_CHAPTER_%d_START=%"PRIu64"\n", demuxer->num_chapters, start);
	if (end)
		mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_CHAPTER_%d_END=%"PRIu64"\n", demuxer->num_chapters, end);
	if (name)
		mp_msg(MSGT_IDENTIFY, MSGL_INFO, "ID_CHAPTER_%d_NAME=%s\n", demuxer->num_chapters, name);

	return demuxer->num_chapters++;
}

/**
 * \brief demuxer_seek_chapter() seeks to a chapter in two possible ways:
 *        either using the demuxer->chapters structure set by the demuxer
 *        or asking help to the stream layer (e.g. dvd)
 * \param chapter - chapter number wished - 0-based
 * \param mode 0: relative to current main pts, 1: absolute
 * \param seek_pts set by the function to the pts to seek to (if demuxer->chapters is set)
 * \param num_chapters number of chapters present (set by this function is param is not null)
 * \param chapter_name name of chapter found (set by this function is param is not null)
 * \return -1 on error, current chapter if successful
 */

int demuxer_seek_chapter(demuxer_t *demuxer, int chapter, int mode,
		float *seek_pts, int *num_chapters,
		char **chapter_name)
{
	int ris;
	int current, total;

	if (!demuxer->num_chapters || !demuxer->chapters) {
		if (!mode) {
			ris = stream_control(demuxer->stream,
					STREAM_CTRL_GET_CURRENT_CHAPTER, &current);
			if (ris == STREAM_UNSUPPORTED)
				return -1;
			chapter += current;
		}

		demux_flush(demuxer);

		ris = stream_control(demuxer->stream, STREAM_CTRL_SEEK_TO_CHAPTER,
				&chapter);

		demux_resync(demuxer);

		// exit status may be ok, but main() doesn't have to seek itself
		// (because e.g. dvds depend on sectors, not on pts)
		*seek_pts = -1.0;

		if (num_chapters) {
			if (stream_control(demuxer->stream, STREAM_CTRL_GET_NUM_CHAPTERS,
						num_chapters) == STREAM_UNSUPPORTED)
				*num_chapters = 0;
		}

		if (chapter_name) {
			*chapter_name = NULL;
			if (num_chapters && *num_chapters) {
				char *tmp = malloc(16);
				if (tmp) {
					sprintf(tmp, " of %3d", *num_chapters);
					*chapter_name = tmp;
				}
			}
		}

		return ris != STREAM_UNSUPPORTED ? chapter : -1;
	} else {  // chapters structure is set in the demuxer
		sh_video_t *sh_video = demuxer->video->sh;
		sh_audio_t *sh_audio = demuxer->audio->sh;

		total = demuxer->num_chapters;

		if (mode == 1)  //absolute seeking
			current = chapter;
		else {          //relative seeking
			uint64_t now;
			now = (sh_video ? sh_video->pts : (sh_audio ? sh_audio->pts : 0.))
				* 1000 + .5;

			for (current = total - 1; current >= 0; --current) {
				demux_chapter_t *chapter = demuxer->chapters + current;
				if (chapter->start <= now)
					break;
			}
			current += chapter;
		}

		if (current >= total)
			return -1;
		if (current < 0)
			current = 0;

		*seek_pts = demuxer->chapters[current].start / 1000.0;

		if (num_chapters)
			*num_chapters = demuxer->num_chapters;

		if (chapter_name) {
			if (demuxer->chapters[current].name)
				*chapter_name = strdup(demuxer->chapters[current].name);
			else
				*chapter_name = NULL;
		}

		return current;
	}
}

int demuxer_get_current_chapter(demuxer_t *demuxer)
{
	int chapter = -1;
	if (!demuxer->num_chapters || !demuxer->chapters) {
		if (stream_control(demuxer->stream, STREAM_CTRL_GET_CURRENT_CHAPTER,
					&chapter) == STREAM_UNSUPPORTED)
			chapter = -1;
	} else {
		sh_video_t *sh_video = demuxer->video->sh;
		sh_audio_t *sh_audio = demuxer->audio->sh;
		uint64_t now;
		now = (sh_video ? sh_video->pts : (sh_audio ? sh_audio->pts : 0))
			* 1000 + 0.5;
		for (chapter = demuxer->num_chapters - 1; chapter >= 0; --chapter) {
			if (demuxer->chapters[chapter].start <= now)
				break;
		}
	}
	return chapter;
}

char *demuxer_chapter_name(demuxer_t *demuxer, int chapter)
{
	if (demuxer->num_chapters && demuxer->chapters) {
		if (chapter >= 0 && chapter < demuxer->num_chapters
				&& demuxer->chapters[chapter].name)
			return strdup(demuxer->chapters[chapter].name);
	}
	return NULL;
}

char *demuxer_chapter_display_name(demuxer_t *demuxer, int chapter)
{
	char *chapter_name = demuxer_chapter_name(demuxer, chapter);
	if (chapter_name) {
		char *tmp = malloc(strlen(chapter_name) + 14);
		snprintf(tmp, 63, "(%d) %s", chapter + 1, chapter_name);
		free(chapter_name);
		return tmp;
	} else {
		int chapter_num = demuxer_chapter_count(demuxer);
		char tmp[30];
		if (chapter_num <= 0)
			sprintf(tmp, "(%d)", chapter + 1);
		else
			sprintf(tmp, "(%d) of %d", chapter + 1, chapter_num);
		return strdup(tmp);
	}
}

float demuxer_chapter_time(demuxer_t *demuxer, int chapter, float *end)
{
	if (demuxer->num_chapters && demuxer->chapters && chapter >= 0
			&& chapter < demuxer->num_chapters) {
		if (end)
			*end = demuxer->chapters[chapter].end / 1000.0;
		return demuxer->chapters[chapter].start / 1000.0;
	}
	return -1.0;
}

int demuxer_chapter_count(demuxer_t *demuxer)
{
	if (!demuxer->num_chapters || !demuxer->chapters) {
		int num_chapters = 0;
		if (stream_control(demuxer->stream, STREAM_CTRL_GET_NUM_CHAPTERS,
					&num_chapters) == STREAM_UNSUPPORTED)
			num_chapters = 0;
		return num_chapters;
	} else
		return demuxer->num_chapters;
}

int demuxer_angles_count(demuxer_t *demuxer)
{
	int ris, angles = -1;

	ris = stream_control(demuxer->stream, STREAM_CTRL_GET_NUM_ANGLES, &angles);
	if (ris == STREAM_UNSUPPORTED)
		return -1;
	return angles;
}

int demuxer_get_current_angle(demuxer_t *demuxer)
{
	int ris, curr_angle = -1;
	ris = stream_control(demuxer->stream, STREAM_CTRL_GET_ANGLE, &curr_angle);
	if (ris == STREAM_UNSUPPORTED)
		return -1;
	return curr_angle;
}


int demuxer_set_angle(demuxer_t *demuxer, int angle)
{
	int ris, angles = -1;

	angles = demuxer_angles_count(demuxer);
	if ((angles < 1) || (angle > angles))
		return -1;

	demux_flush(demuxer);

	ris = stream_control(demuxer->stream, STREAM_CTRL_SET_ANGLE, &angle);
	if (ris == STREAM_UNSUPPORTED)
		return -1;

	demux_resync(demuxer);

	return angle;
}

int demuxer_audio_lang(demuxer_t *d, int id, char *buf, int buf_len)
{
	struct stream_lang_req req;
	sh_audio_t *sh;
	if (id < 0 || id >= MAX_A_STREAMS)
		return -1;
	sh = d->a_streams[id];
	if (!sh)
		return -1;
	if (sh->lang) {
		av_strlcpy(buf, sh->lang, buf_len);
		return 0;
	}
	req.type = stream_ctrl_audio;
	req.id = sh->aid;
	if (stream_control(d->stream, STREAM_CTRL_GET_LANG, &req) == STREAM_OK) {
		av_strlcpy(buf, req.buf, buf_len);
		return 0;
	}
	return -1;
}

int demuxer_sub_lang(demuxer_t *d, int id, char *buf, int buf_len)
{
	struct stream_lang_req req;
	sh_sub_t *sh;
	if (id < 0 || id >= MAX_S_STREAMS)
		return -1;
	sh = d->s_streams[id];
	if (sh && sh->lang) {
		av_strlcpy(buf, sh->lang, buf_len);
		return 0;
	}
	req.type = stream_ctrl_sub;
	// assume 1:1 mapping so we can show the language of
	// DVD subs even when we have not yet created the stream.
	req.id = sh ? sh->sid : id;
	if (stream_control(d->stream, STREAM_CTRL_GET_LANG, &req) == STREAM_OK) {
		av_strlcpy(buf, req.buf, buf_len);
		return 0;
	}
	return -1;
}

int demuxer_audio_track_by_lang(demuxer_t *d, char *lang)
{
	int i, len;
	lang += strspn(lang, ",");
	while ((len = strcspn(lang, ",")) > 0) {
		for (i = 0; i < MAX_A_STREAMS; ++i) {
			sh_audio_t *sh = d->a_streams[i];
			if (sh && sh->lang && strncmp(sh->lang, lang, len) == 0)
				return sh->aid;
		}
		lang += len;
		lang += strspn(lang, ",");
	}
	return -1;
}

int demuxer_sub_track_by_lang(demuxer_t *d, char *lang)
{
	int i, len;
	lang += strspn(lang, ",");
	while ((len = strcspn(lang, ",")) > 0) {
		for (i = 0; i < MAX_S_STREAMS; ++i) {
			sh_sub_t *sh = d->s_streams[i];
			if (sh && sh->lang && strncmp(sh->lang, lang, len) == 0)
				return sh->sid;
		}
		lang += len;
		lang += strspn(lang, ",");
	}
	return -1;
}

int demuxer_default_audio_track(demuxer_t *d)
{
	int i;
	for (i = 0; i < MAX_A_STREAMS; ++i) {
		sh_audio_t *sh = d->a_streams[i];
		if (sh && sh->default_track)
			return sh->aid;
	}
	for (i = 0; i < MAX_A_STREAMS; ++i) {
		sh_audio_t *sh = d->a_streams[i];
		if (sh)
			return sh->aid;
	}
	return -1;
}

int demuxer_default_sub_track(demuxer_t *d)
{
	int i;
	for (i = 0; i < MAX_S_STREAMS; ++i) {
		sh_sub_t *sh = d->s_streams[i];
		if (sh && sh->default_track)
			return sh->sid;
	}
	return -1;
}
