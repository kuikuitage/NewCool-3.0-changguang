#if 1//def __LINUX__
#include "config.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#include "sys_define.h"
#include <string.h>
#else
//#include "mp_func_trans.h"
#endif

#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include "libmpdemux/stheader.h"
#include <stddef.h>
#include "mplayer/codec-cfg.h"
#ifdef __LINUX__
#include "mplayer/ffmpeg/libavcodec/mpegvideo.h"
#endif

#include "mpcommon.h"
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_mem.h"
#include "ts_sequence.h"
#include "file_playback_sequence.h"
#include "demux_mp.h"
#include "mplayer/ffmpeg/libavformat/avformat.h"
#include "aud_vsb.h"
#include "hal_misc.h"
//#include "common.h"
//#include "vdec.h"
static  float gop_time , gop_start, gop_end ;
static int  write_flag = 0;
static int seek_keyframe = 0;
//for trick control
static int speed_ctrl_cnt = 0;

static int  seek_back_end = 0;
static int  seek_back_cnt = 0;
static int seek_cnt = 0;
static int  key_last = 0;

static unsigned int seek_back_tick1 = 0;
static unsigned int seek_back_tick2 = 0;

#define TS_PH_PACKET_SIZE 192
#define TS_FEC_PACKET_SIZE 204

#define NB_PID_MAX 8192
extern unsigned int video_codec_id;
extern unsigned int audio_codec_id;
extern unsigned short selected_apid;
extern unsigned short selected_vpid;
extern int io_isnetworkstream;
extern ff_pmt_id ff_mpegts_pmt;
//#define SEEK_TICK_CACULATE

typedef enum {
    UNKNOWN_MP     = -1,
    VIDEO_MPEG1_MP     = 0x10000001,
    VIDEO_MPEG2_MP     = 0x10000002,
    VIDEO_MPEG4_MP     = 0x10000004,
    VIDEO_H264_MP  = 0x10000005,
    VIDEO_AVC_MP   = mmioFOURCC('a', 'v', 'c', '1'),
    VIDEO_DIRAC_MP = mmioFOURCC('d', 'r', 'a', 'c'),
    VIDEO_VC1_MP   = mmioFOURCC('W', 'V', 'C', '1'),
    AUDIO_MP2_MP       = 0x50,
    AUDIO_A52_MP       = 0x2000,
    AUDIO_DTS_MP   = 0x2001,
    AUDIO_LPCM_BE_MP   = 0x10001,
    AUDIO_AAC_MP   = mmioFOURCC('M', 'P', '4', 'A'),
    AUDIO_AAC_LATM_MP  = mmioFOURCC('M', 'P', '4', 'L'),
    AUDIO_TRUEHD_MP    = mmioFOURCC('T', 'R', 'H', 'D'),
    AUDIO_S302M_MP     = mmioFOURCC('B', 'S', 'S', 'D'),
    AUDIO_PCM_BR_MP    = mmioFOURCC('B', 'P', 'C', 'M'),
    SPU_DVD_MP     = 0x3000000,
    SPU_DVB_MP     = 0x3000001,
    SPU_TELETEXT_MP    = 0x3000002,
    SPU_PGS_MP     = 0x3000003,
    PES_PRIVATE1_MP    = 0xBD00000,
    SL_PES_STREAM_MP   = 0xD000000,
    SL_SECTION_MP  = 0xD100000,
    MP4_OD_MP      = 0xD200000,
} es_stream_type_t;

typedef struct {
    uint8_t * buffer;
    uint16_t buffer_len;
} ts_section_t;

typedef struct {
    int size;
    unsigned char * start;
    uint16_t payload_size;
    es_stream_type_t type, subtype;
    double pts, last_pts;
    unsigned int pts31bit;
    int pid;
    char lang[4];
    int last_cc;                // last cc code (-1 if first packet)
    int is_synced;
    ts_section_t section;
    uint8_t * extradata;
    int extradata_alloc, extradata_len;
    struct {
        uint8_t au_start, au_end, last_au_end;
    } sl;
} ES_stream_t;

typedef struct {
    void * sh;
    int id;
    int type;
} sh_av_t;

typedef struct MpegTSContext {
    int packet_size;        // raw packet size, including FEC if present e.g. 188 bytes
    ES_stream_t * pids[NB_PID_MAX];
    sh_av_t streams[NB_PID_MAX];
} MpegTSContext;


typedef struct {
    demux_stream_t * ds;
    demux_packet_t * pack;
    int offset, buffer_size;
} av_fifo_t;

#define MAX_EXTRADATA_SIZE 64*1024
typedef struct {
    int32_t object_type;    //aka codec used
    int32_t stream_type;    //video, audio etc.
    uint8_t buf[MAX_EXTRADATA_SIZE];
    uint16_t buf_size;
    uint8_t szm1;
} mp4_decoder_config_t;

typedef struct {
    //flags
    uint8_t flags;
    uint8_t au_start;
    uint8_t au_end;
    uint8_t random_accesspoint;
    uint8_t random_accesspoint_only;
    uint8_t padding;
    uint8_t use_ts;
    uint8_t idle;
    uint8_t duration;

    uint32_t ts_resolution, ocr_resolution;
    uint8_t ts_len, ocr_len, au_len, instant_bitrate_len, degr_len, au_seqnum_len, packet_seqnum_len;
    uint32_t timescale;
    uint16_t au_duration, cts_duration;
    uint64_t ocr, dts, cts;
} mp4_sl_config_t;

typedef struct {
    uint16_t id;
    uint8_t flags;
    mp4_decoder_config_t decoder;
    mp4_sl_config_t sl;
} mp4_es_descr_t;

typedef struct {
    uint16_t id;
    uint8_t flags;
    mp4_es_descr_t * es;
    uint16_t es_cnt;
} mp4_od_t;
typedef struct {
    int audio_is_pcm;
    int bits_per_coded_sample;
    int sample_fmt;
    int bits_per_raw_sample;
    int sample_rate;
    uint64_t channel_layout;
    int channels;
    int bit_rate;
    int is_big_endian;
} pcm_info_t;

typedef struct {
    uint8_t skip;
    uint8_t table_id;
    uint8_t ssi;
    uint16_t section_length;
    uint16_t ts_id;
    uint8_t version_number;
    uint8_t curr_next;
    uint8_t section_number;
    uint8_t last_section_number;
    struct pat_progs_t {
        uint16_t id;
        uint16_t pmt_pid;
    } * progs;
    uint16_t progs_cnt;
    ts_section_t section;
} pat_t;

typedef struct {
    uint16_t progid;
    uint8_t skip;
    uint8_t table_id;
    uint8_t ssi;
    uint16_t section_length;
    uint8_t version_number;
    uint8_t curr_next;
    uint8_t section_number;
    uint8_t last_section_number;
    uint16_t PCR_PID;
    uint16_t prog_descr_length;
    ts_section_t section;
    uint16_t es_cnt;
    pcm_info_t pcm_info;
    struct pmt_es_t {
        uint16_t pid;
        uint32_t type;  //it's 8 bit long, but cast to the right type as FOURCC
        uint16_t descr_length;
        uint8_t format_descriptor[5];
        uint8_t lang[4];
        uint16_t mp4_es_id;
        uint16_t audio_eac3_flag;

    } * es;
    mp4_od_t iod, *od;
    mp4_es_descr_t * mp4es;
    int od_cnt, mp4es_cnt;
} pmt_t;

typedef struct {
    uint64_t size;
    float duration;
    double first_pts;
    double last_pts;
} TS_stream_info;

typedef struct {
    MpegTSContext ts;
    int last_pid;
    av_fifo_t fifo[3];  //0 for audio, 1 for video, 2 for subs
    pat_t pat;
    pmt_t * pmt;
    uint16_t pmt_cnt;
    uint32_t prog;
    uint32_t vbitrate;
    int keep_broken;
    int last_aid;
    int last_vid;
    int last_sid;
    char packet[TS_FEC_PACKET_SIZE];
    TS_stream_info vstr, astr;
} ts_priv_t;


typedef struct {
    es_stream_type_t type;
    ts_section_t section;
} TS_pids_t;
 int get_seek_state()
    {
        FILE_SEQ_T * p_seq = file_seq_get_instance();

        if ((p_seq->cur_speed != 0) || (p_seq->is_play_at_time == 1) || p_seq->m_play_state != FILE_SEQ_PLAY) {
            return 1;
        } else {
            return 0;
        }
    }

 unsigned long get_vpts_upload()
 	{
 	FILE_SEQ_T * p_seq = file_seq_get_instance();
	return p_seq->vpts_upload;
 	}
int prog_id_index;
static int speed_start;
int audio_id_index, video_id_index, dvdsub_id_index;
extern  int audio_id, video_id, dvdsub_id;
sub_data * subdata = NULL;
subtitle * vo_sub = NULL;
subtitle * sub_temp = NULL;
static int sub_flag = 0;
int bsf_vcodec_flag = -1;
extern void set_ts_prog(demuxer_t * demuxer);
ts_info_t * ds_ts_prog(demuxer_t * demuxer)
{
    ts_priv_t * priv = (ts_priv_t *) demuxer->priv;
    int k = 0;
    int g = 0;
    pmt_t * pmt1;
    ts_info_t * priv_res;
    prog_id_index = 0;
    priv_res = mtos_malloc(sizeof(ts_info_t));
    memset(priv_res, 0, sizeof(ts_info_t));
    priv_res->pmt_cnt = priv->pmt_cnt;
    printf("\n pmt cnt %d \n", priv_res->pmt_cnt);
    priv_res->p_pmt = mtos_malloc(sizeof(pmt_ts_t) * (priv->pmt_cnt + 1));
    memset(priv_res->p_pmt, 0, sizeof(pmt_ts_t) * (priv->pmt_cnt + 1));

    for (k = 0 ; k < priv->pmt_cnt; k++) {
        g = 0;
        pmt1 = (pmt_t *)(&priv->pmt[k]);
        priv_res->p_pmt[k].progid = pmt1->progid;
        priv_res->p_pmt[k].pcr_pid = pmt1->PCR_PID;
        priv_res->p_pmt[k].es_cnt = pmt1->es_cnt;
        printf("\n prog id %d \n", priv_res->p_pmt[k].progid);
        printf("\n es_cnt %d \n", priv_res->p_pmt[k].es_cnt);

        for (g = 0; g < pmt1->es_cnt; g++) {
            priv_res->p_pmt[k].es[g].pid = pmt1->es[g].pid;

            if ((selected_vpid != -1) && (pmt1->es[g].pid == selected_vpid)) {
                prog_id_index = k;
            }

            priv_res->p_pmt[k].es[g].type = pmt1->es[g].type;
            priv_res->p_pmt[k].es[g].audio_eac3_flag = pmt1->es[g].audio_eac3_flag;
            strcpy(priv_res->p_pmt[k].es[g].lang, pmt1->es[g].lang);
            printf("\n pid %d  pid type % x lang: %s,eac3[%d]\n", priv_res->p_pmt[k].es[g].pid, priv_res->p_pmt[k].es[g].type, priv_res->p_pmt[k].es[g].lang, pmt1->es[g].audio_eac3_flag);
        }
    }

    set_ts_prog(demuxer);
    printf("select the %d pmt prog id  \n", prog_id_index);
    return priv_res;
}

void ts_get_pcm_info(demuxer_t * demuxer, int * is_big_endian, int * bits, int * channel, int * sample_rate)
{
    ts_priv_t * priv = (ts_priv_t *) demuxer->priv;
    pmt_t  * pmt = (pmt_t *)(&priv->pmt[prog_id_index]);
    *is_big_endian = pmt->pcm_info.is_big_endian;
    *bits = pmt->pcm_info.bits_per_coded_sample;
    *channel = pmt->pcm_info.channels;
    *sample_rate = pmt->pcm_info.sample_rate;
}

void set_ts_pmt_avs_info(ts_info_t * p_ts_priv)
{
    OS_PRINTF("[%s] start start!!!!!\n", __func__);
    OS_PRINTF("[%s] p_ts_priv = 0x%x\n", __func__, p_ts_priv);
    ts_info_t * ts_priv = p_ts_priv;
    pmt_ts_t * pmt;
    int i, j;

    for (i = 0; i < ts_priv->pmt_cnt; i++) {
        pmt = ts_priv->p_pmt + i;
        pmt->aud_stream_cnt = 0;
        pmt->vid_stream_cnt = 0;
        pmt->sub_title_cnt = 0;
        OS_PRINTF("[%s] ---------for1 i=%d,pmtcnt[%d],escnt[%d]\n", __func__, i, ts_priv->pmt_cnt, pmt->es_cnt);

        for (j = 0; j < pmt->es_cnt; j++) {
            //OS_PRINTF("[%s] ---------for2 j=%d,es_cnt[%d]\n",__func__,j,pmt->es_cnt);
            switch (pmt->es[j].type) {
                case VIDEO_MPEG1_MP:
                case VIDEO_MPEG2_MP:
                case VIDEO_H264_MP:
                    pmt->vid_stream[pmt->vid_stream_cnt].type = pmt->es[j].type;
                    pmt->vid_stream[pmt->vid_stream_cnt].pid = pmt->es[j].pid;
                    strcpy(pmt->vid_stream[pmt->vid_stream_cnt].lang, pmt->es[j].lang);
                    pmt->vid_stream_cnt++;
                    break;
                case AUDIO_MP2_MP:
                case AUDIO_A52_MP:
                case AUDIO_AAC_MP:
                    pmt->aud_stream[pmt->aud_stream_cnt].type = pmt->es[j].type;
                    pmt->aud_stream[pmt->aud_stream_cnt].pid = pmt->es[j].pid;
                    strcpy(pmt->aud_stream[pmt->aud_stream_cnt].lang, pmt->es[j].lang);
                    pmt->aud_stream_cnt++;
                    break;
                case SPU_DVD_MP:
                case SPU_DVB_MP:
                case SPU_TELETEXT_MP:
                case SPU_PGS_MP:
                    pmt->sub_title[pmt->sub_title_cnt].type = pmt->es[j].type;
                    pmt->sub_title[pmt->sub_title_cnt].pid = pmt->es[j].pid;
                    strcpy(pmt->sub_title[pmt->sub_title_cnt].lang, pmt->es[j].lang);
                    pmt->sub_title_cnt++;
                    break;
                default:
                    break;
            }
        }
    }

    OS_PRINTF("[%s] end end!!!!!\n", __func__);
}

int ts_get_audio_track_pid(ts_info_t * p_ts_priv, int id)
{
    ts_info_t * ts_priv = p_ts_priv;
    pmt_ts_t * pmt = (pmt_t *)(&ts_priv->p_pmt[prog_id_index]);

    if (!pmt->aud_stream_cnt) {
        return 0;
    }

    if (id > pmt->aud_stream_cnt - 1) {
        return pmt->aud_stream[0].pid;
    } else {
        return pmt->aud_stream[id].pid;
    }
}
int ds_ts_prog_select(demuxer_t * demuxer, int prog_index)
{
    ts_priv_t * priv = (ts_priv_t *) demuxer->priv;
    int k = 0;
    pmt_t * pmt;

    if (prog_index < priv->pmt_cnt) {
        pmt = (pmt_t *)(&priv->pmt[prog_index]);
        int g = 0;
        int video_found = 0 ;
        int audio_found = 0 ;
        int sub_found = 0;

        for (g = 0; g < pmt->es_cnt; g++) {
            if (0 == video_found) {
                if ((pmt->es[g].type == VIDEO_MPEG1_MP) || (pmt->es[g].type == VIDEO_MPEG2_MP) ||
                    (pmt->es[g].type == VIDEO_H264_MP)) {
                    video_found = 1;
                    video_id = pmt->es[g].pid;
                    video_id_index = g;
                }
            }

            if (0 == audio_found) {
                if ((pmt->es[g].type == AUDIO_MP2_MP) || (pmt->es[g].type == AUDIO_A52_MP) ||
                    (pmt->es[g].type == AUDIO_AAC_MP)) {
                    audio_found = 1;
                    audio_id = pmt->es[g].pid;
                    audio_id_index = g;
                }
            }

            if (0 == sub_found) {
                if ((pmt->es[g].type == SPU_TELETEXT_MP)
                   ) {
                    sub_found = 1;
                    dvdsub_id = pmt->es[g].pid;
                    dvdsub_id_index = g;
                }
            }
        }
    }
}
demuxer_t * demux_mp_open(stream_t * vs, int file_format, int audio_id_mp,
                          int video_id_mp, int dvdsub_id_mp, char * filename)
{
    //demuxer_t *
    demuxer_t * res;
    OS_PRINTF("[%s] -----file_format=%d\n", __func__, file_format);
    res = demux_open(vs,  file_format,  -1, -1, -1, filename);
    OS_PRINTF("[%s] -----res=%x\n", __func__, res);

    if (res) {
        OS_PRINTF("[%s] -----res->type=%d, res->file_format=%d\n", __func__, res->type, res->file_format);

        if (DEMUXER_TYPE_MPEG_TS == res->type) {
            // ds_ts_prog(res);
            //prog_id_index = 1;
            //ds_ts_prog_select(res, prog_id_index);
            //if (res) free_demuxer(res);
            //res = demux_open(vs, file_format, -1, -1, -1, filename);
            return res;
        }
    }

    return res;
}

int ds_get_audio_bps(demux_stream_t * ds)
{
    sh_audio_t * sh_audio = ds->sh;

    if (NULL == sh_audio) {
        return 0;
    }

    return sh_audio->i_bps;
}

int ds_get_video_bps(demux_stream_t * ds)
{
    sh_video_t * sh_video = ds->sh;
    double duration = 0;
    uint64_t file_size = 0;
#ifdef __LINUX__
    struct stat stat_buf;
#endif

    if (NULL == sh_video) {
        return 0;
    }

    if (!sh_video->i_bps) {
        demux_control(ds->demuxer, DEMUXER_CTRL_GET_TIME_LENGTH, (void *) &duration);
#ifndef __LINUX__
        file_size = ((ufs_file_t *)(ds->demuxer->stream->fd))->file_size;
#else
        stat(ds->demuxer->filename, &stat_buf);
        file_size = stat_buf.st_size;
#endif
        sh_video->i_bps = file_size / duration;
    }

    return sh_video->i_bps;
}
void   ds_get_vfilter_type(demux_stream_t * ds)
{
    sh_video_t * sh_video = ds->sh;
    int biComp = 0, i;
    char * p_str = NULL;
    bsf_vcodec_flag = -1;
    FILE_SEQ_T * p_file_seq = file_seq_get_instance();

    if (DEMUXER_TYPE_LAVF == ds->demuxer->type && sh_video) {
        if (sh_video->bih) {
            biComp = le2me_32(sh_video->bih->biCompression);
            p_str  = (char *)(&biComp);

            if ((strstr(((char *) &biComp), "mpg2")) || (strstr(((char *) &biComp), "mpg1"))) {
                bsf_vcodec_flag = 0;
            } else if (sh_video->bih->biCompression == 1) {
                bsf_vcodec_flag = 0;
            } else if (strstr(((char *) &biComp), "MPG2")) {
                bsf_vcodec_flag = 0;
            } else if ((strstr(((char *) &biComp), "H264")) || (strstr(((char *) &biComp), "avc1"))
                       || (strstr(((char *) &biComp), "h264"))) {
                bsf_vcodec_flag = 1;
            } else if ((strstr(((char *) &biComp), "WVC1")) || (strstr(((char *) &biComp), "WMV"))) {
                bsf_vcodec_flag = 4;
            } else if ((strstr(((char *) &biComp), "MP4V")) || (strstr(((char *) &biComp), "XVID")) || \
                       (strstr(((char *) &biComp), "DIV")) || (strstr(((char *) &biComp), "DX"))) {
                bsf_vcodec_flag = 3;
            }
        }
    }
}

void   ds_get_video_codec_type(demux_stream_t * ds, int * p_video_type, int * vpid, int * pcr_pid)
{
    sh_video_t * sh_video = ds->sh;
    int biComp = 0, i;
    char * p_str = NULL;
    unsigned int ic_id = IC_WARRIORS;
    video_codec_id = -1;
    ic_id = hal_get_chip_ic_id();
    FILE_SEQ_T * p_file_seq = file_seq_get_instance();

    if (DEMUXER_TYPE_MPEG_TS == ds->demuxer->type) {
        ts_priv_t * priv = (ts_priv_t *) ds->demuxer->priv;
        int k = 0;
        pmt_t * pmt;
        //ds_ts_prog(ds->demuxer);
        /*
        audio_id, video_id, dvdsub_id
        */
        pmt = (pmt_t *)(&priv->pmt[prog_id_index]);

        if(pmt == NULL)
        {
            *p_video_type = 0xffff;
            if (sh_video) {
                switch (sh_video->format) {
                    case VIDEO_MPEG1_MP:
                        *p_video_type = 0;
                        video_codec_id = vVIDEO_MPEG1;
                        break;
                    case VIDEO_MPEG2_MP:
                        *p_video_type = 0;
                        video_codec_id = vVIDEO_MPEG2;
                        break;
                    case VIDEO_MPEG4_MP:

                        if (ic_id == IC_SONATA || ic_id == IC_CONCERTO) {
                            *p_video_type = 0;
                            video_codec_id = vVIDEO_MPEG4;
                        }

                        break;
                    case VIDEO_H264_MP:
                        *p_video_type = 0;
                        video_codec_id = vVIDEO_H264;
                        break;
                    case VIDEO_VC1_MP:
                        *p_video_type = 0;
                        video_codec_id = vVIDEO_VC1;
                        break;
                    default:
                        mtos_printk("\n%s %d\n", __func__, __LINE__);
                        break;
                }
            }

            return ;
        }

        sh_video->i_bps /= priv->pmt_cnt;

        for (i = 0; i < pmt->es_cnt; i++) {
            if ((pmt->es[i].type == VIDEO_MPEG1_MP)) {
                *p_video_type = 0;
                *vpid = pmt->es[i].pid;
                *pcr_pid = pmt->PCR_PID;
                video_codec_id = vVIDEO_MPEG1;
                break;
            } else if ((pmt->es[i].type == VIDEO_MPEG2_MP)) {
                *p_video_type = 0;
                *vpid = pmt->es[i].pid;
                *pcr_pid = pmt->PCR_PID;
                video_codec_id = vVIDEO_MPEG2;
                break;
            } else if ((pmt->es[i].type == VIDEO_H264_MP)) {
                *p_video_type = 1;
                *vpid = pmt->es[i].pid;
                *pcr_pid = pmt->PCR_PID;
                video_codec_id = vVIDEO_H264;
                break;
            } else {
                printf("ERROR may unsupport %d !!!!!!!!!!!\n", pmt->es[video_id_index].type);
                *pcr_pid = pmt->PCR_PID;
                *p_video_type = 0xffff;

                if (pmt->es[i].type == VIDEO_MPEG4_MP) {
                    video_codec_id = vVIDEO_MPEG4;

                    if (ic_id == IC_SONATA || ic_id == IC_CONCERTO) {
                        *vpid = pmt->es[i].pid;
                        *p_video_type = 3;
                        break;
                    }
                } else if (pmt->es[i].type == VIDEO_VC1_MP) {
                    video_codec_id = vVIDEO_VC1;
                }
            }
        }
    } else if (DEMUXER_TYPE_MPEG_PS == ds->demuxer->type) {
        *p_video_type = 0;
        video_codec_id = vVIDEO_MPEG2;
    } else if (DEMUXER_TYPE_MPEG4_ES == ds->demuxer->type) {
        video_codec_id = vVIDEO_MPEG4;

        if (ic_id == IC_SONATA || ic_id == IC_CONCERTO) {
            *p_video_type = 3;
        }
    } else if (DEMUXER_TYPE_RTP == ds->demuxer->type) {
        if (sh_video->bih) {
            biComp = le2me_32(sh_video->bih->biCompression);
        }

        char * p_str = (char *)(&biComp);
        printf("[%s] p_str:%s,sh_video->bih->biCompression=%d\n", __func__, p_str, sh_video->bih->biCompression);

        if ((strstr(((char *) &biComp), "mpg2")) || (strstr(((char *) &biComp), "mpg1"))) {
            *p_video_type = 0;
        } else if (sh_video->bih->biCompression == 1) {
            *p_video_type = 0;
        } else if (strstr(((char *) &biComp), "MPG2")) {
            *p_video_type = 0;
        } else if (strstr(((char *) &biComp), "H264")) {
            *p_video_type = 1;
#if defined(ENABLE_DEMUX_RTSP)
        } else if (demux_is_mpeg_rtp_stream(ds->demuxer)) {
            *p_video_type = 0;
            printf("[%s]-------demux_is_mpeg_rtp_stream!\n", __func__);
#endif
        } else {
            printf("ERROR not support %.4s !!!!!!!!!!!\n", (char *)&biComp);
            *p_video_type = 0xffff;
            //*p_video_type = 0; ----add for decodint zx_vmpeg_ampg.mp4 by rtsp ok!
        }
    } else {
        if (sh_video->bih) {
            biComp = le2me_32(sh_video->bih->biCompression);
            p_str  = (char *)(&biComp);

            if ((strstr(((char *) &biComp), "mpg2")) || (strstr(((char *) &biComp), "mpg1"))) {
                *p_video_type = 0;
                video_codec_id = vVIDEO_MPEG2;
            } else if (sh_video->bih->biCompression == 1) {
                *p_video_type = 0;
                video_codec_id = vVIDEO_MPEG1;
            } else if (strstr(((char *) &biComp), "MPG2")) {
                *p_video_type = 0;
                video_codec_id = vVIDEO_MPEG2;
            } else if ((strstr(((char *) &biComp), "H264")) || (strstr(((char *) &biComp), "avc1"))
                       || (strstr(((char *) &biComp), "h264"))) {
                *p_video_type = 1;
                video_codec_id = vVIDEO_H264;
            } else if (strstr(((char *) &biComp), "RV30")) {
                *p_video_type = 10;
                video_codec_id = vVIDEO_RV30;
            } else if (strstr(((char *) &biComp), "RV40")) {
                *p_video_type = 11;
                video_codec_id = vVIDEO_RV40;
            } else if (strstr(((char *) &biComp), "VP80")) {
                printf("warning may unsupport %.4s !!!!!!!!!!!\n", (char *)&biComp);
                *p_video_type = 9;
                video_codec_id = vVIDEO_VP8;
            } else {
                *p_video_type = 0xffff;
                printf("ERROR may unsupport %.4s !!!!!!!!!!!\n", (char *)&biComp);

                if ((strstr(((char *) &biComp), "WVC1")) || (strstr(((char *) &biComp), "WMV"))) {
                    video_codec_id = vVIDEO_VC1;
                } else if ((strstr(((char *) &biComp), "MP4V")) || (strstr(((char *) &biComp), "XVID")) || \
                           (strstr(((char *) &biComp), "DIV") && !strstr(((char *) &biComp), "DIV3")) || \
                           (strstr(((char *) &biComp), "DX")) || (strstr(((char *) &biComp), "263"))) {
                    video_codec_id = vVIDEO_MPEG4;

                    if (ic_id == IC_SONATA) {
                        *p_video_type = 3;
                    }
                    else if (ic_id == IC_CONCERTO) {
                        if(strstr(((char *) &biComp), "263"))
                            *p_video_type = 12;
                        else
                            *p_video_type = 3;
                    }
                }
            }
        } else {
            printf("ERROR may unsupport %.4s !!!!!!!!!!!\n", (char *)&biComp);
            *p_video_type = 0xffff;
        }
    }

    if (p_file_seq->is_network_stream(p_file_seq) == FALSE && *p_video_type == 1
        && sh_video->i_bps > 5000000 && sh_video->i_bps < 50000000) { //bps > 40Mbps,unsupport
        *p_video_type = 0xffff;
        printf("ERROR not support %.4s !bps == %d\n", (char *)&biComp, sh_video->i_bps);
    }

    if (DEMUXER_TYPE_MPEG_PS == ds->demuxer->type) {
        if (sh_video->i_bps > 3000000 || sh_video->i_bps == 0) {
            *p_video_type = 0xffff;
            OS_PRINTF("ERROR not support mpegps !bps == %d\n", sh_video->i_bps);
        }
    }
    if (strstr(((char *) &biComp), "MJPG"))
	    *p_video_type = 99;
    printf("[%s] biComp:%.4s,id[%x]\n", __func__, (char *)&biComp, video_codec_id);
}

void   ds_get_audio_codec_type(demux_stream_t * ds, int * p_audio_type, int * apid,
                               AUDIO_OUT_MODE aout_mode)
{
    sh_audio_t * sh_audio = ds->sh;
    audio_codec_id = -1;
    int audio_eac3_flag = 0;

    //no audio
    if (NULL == sh_audio) {
        *p_audio_type = 0xffff;
        return 0;
    }

    if (DEMUXER_TYPE_MPEG_TS == ds->demuxer->type) {
        ts_priv_t * priv = (ts_priv_t *) ds->demuxer->priv;
        int k = 0, i;
        pmt_t * pmt;
        //ds_ts_prog(ds->demuxer);
        /*
        audio_id, video_id, dvdsub_id
        */
        pmt = (pmt_t *)(&priv->pmt[prog_id_index]);

   if(NULL == pmt)
        {
            *p_audio_type = 0xffff;

            if (sh_audio) {
                switch (sh_audio->format) {
                    case AUDIO_MP2_MP:
                        *p_audio_type = AUDIO_MP2;
                        audio_codec_id = aAUDIO_MP2;
                        break;
                    case MKTAG('B', 'P', 'C', 'M'):
                        *p_audio_type = AUDIO_PCM;
                        audio_codec_id = aAUDIO_PCM_BR;
                        break;
                    case AUDIO_A52_MP:

                        if (aout_mode == AUDIO_LPCM) {
                            if (audio_eac3_flag) {
                                *p_audio_type = AUDIO_EAC3;    //5;//AUDIO_EAC3
                            } else {
                                *p_audio_type = AUDIO_AC3_VSB;    //4;//AUDIO_AC3_VSB
                            }
                        } else {
                            *p_audio_type = AUDIO_SPDIF;    //7;//AUDIO_SPDIF
                        }

                        audio_codec_id = aAUDIO_AC3;
                        break;
                    case AUDIO_AAC_MP:
                    case AUDIO_AAC_LATM_MP:
                        *p_audio_type = AUDIO_AAC;//6;
                        audio_codec_id = aAUDIO_AAC;
                        break;
                    default:
                        mtos_printk("\n%s %d\n", __func__, __LINE__);
                        break;
                }
            }
            return ;
        }

        for (i = 0; i < pmt->es_cnt; i++) {
            if ((pmt->es[i].type == AUDIO_MP2_MP)) {
                *p_audio_type = AUDIO_MP2;
                *apid = pmt->es[i].pid;
                audio_codec_id = aAUDIO_MP2;
                break;
            } else if ((pmt->es[i].type == MKTAG('B', 'P', 'C', 'M'))) {
                *p_audio_type = AUDIO_PCM;
                *apid = pmt->es[i].pid;
                audio_codec_id = aAUDIO_PCM_BR;
                break;
            } else if ((pmt->es[i].type == AUDIO_A52_MP)) {
                audio_eac3_flag = pmt->es[i].audio_eac3_flag;
#ifdef __LINUX__

                if (audio_eac3_flag) {
                    *p_audio_type = AUDIO_EAC3;    //5;//AUDIO_EAC3
                } else {
                    *p_audio_type = AUDIO_AC3_VSB;    //4;//AUDIO_AC3_VSB
                }

#else

                if (aout_mode == AUDIO_LPCM) {
                    if (audio_eac3_flag) {
                        *p_audio_type = AUDIO_EAC3;    //5;//AUDIO_EAC3
                    } else {
                        *p_audio_type = AUDIO_AC3_VSB;    //4;//AUDIO_AC3_VSB
                    }
                } else {
                    *p_audio_type = AUDIO_SPDIF;    //7;//AUDIO_SPDIF
                }

#endif
                *apid = pmt->es[i].pid;
                audio_codec_id = aAUDIO_AC3;
                break;
            } else if ((pmt->es[i].type == AUDIO_AAC_MP) || (pmt->es[i].type == AUDIO_AAC_LATM_MP)) {
                *p_audio_type = AUDIO_AAC;//6;
                *apid = pmt->es[i].pid;
                audio_codec_id = aAUDIO_AAC;
                break;
            } else {
                printf("ERROR not support %d !!!!!!!!!!!\n", pmt->es[audio_id_index].type);
                *p_audio_type = 0xffff;

                if (pmt->es[i].type == AUDIO_DTS_MP) {
                    audio_codec_id = aAUDIO_DTS;
                }
            }
        }
    } else {
        if (sh_audio->format == MKTAG('M', 'P', '4', 'A') || sh_audio->format == MKTAG('M', 'P', '4', 'A') || sh_audio->format == MKTAG('m', 'p', '4', 'l') || sh_audio->format == MKTAG('M', 'P', '4', 'L')) {
            *p_audio_type = AUDIO_AAC;//6;
            audio_codec_id = aAUDIO_AAC;
            printf("[%s] audio format : mp4a, p_audio_type: 6\n", __func__);
        } else if (sh_audio->format == 0x2000 || sh_audio->format == MKTAG('E', 'A', 'C', '3')) {
            if (sh_audio->format == MKTAG('E', 'A', 'C', '3')) {
                audio_eac3_flag = 1;
            }

#ifdef __LINUX__

            if (audio_eac3_flag) {
                *p_audio_type = AUDIO_EAC3;
            } else {
                *p_audio_type = AUDIO_AC3_VSB;
            }

#else

            if (aout_mode == AUDIO_LPCM) {
                if (audio_eac3_flag) {
                    *p_audio_type = AUDIO_EAC3;
                } else {
                    *p_audio_type = AUDIO_AC3_VSB;
                }
            } else {
                *p_audio_type = AUDIO_SPDIF;
            }

#endif
            audio_codec_id = aAUDIO_AC3;
        } else if (sh_audio->format == 0x55 || sh_audio->format == 0x50) {
            *p_audio_type = AUDIO_MP3;    //MP3 MP2
            audio_codec_id = aAUDIO_MP2;
        } else if (sh_audio->format == 0x2001) {//CODEC_ID_DTS
            *p_audio_type = 0xffff;//
            audio_codec_id = aAUDIO_DTS;
        } else if (sh_audio->format == 0x01 || sh_audio->format == MKTAG('B', 'P', 'C', 'M')) { //CODEC_ID_PCM
            *p_audio_type = AUDIO_PCM;
            audio_codec_id = aAUDIO_PCM_BR;
        } else if (sh_audio->format == 0x11) { //ADPCM unsupport
            *p_audio_type = 0xffff;
            audio_codec_id = aUNKNOWN;
        } else if (sh_audio->format == 0x566f) { //vorbis is 0x566f
            printf("[%s] ERROR unsupport vorbis!!! sh_audio->format[0x%x]\n", __func__, sh_audio->format);
            *p_audio_type = 0xffff;
        } else {
            printf("[%s] sh_audio->format[0x%x]\n", __func__, sh_audio->format);
            *p_audio_type = 0xffff;
        }
    }

    printf("[%s] audio format 0x%x,id[%x]\n", __func__, sh_audio->format, audio_codec_id);
}
int ds_get_packet_subt(demux_stream_t * ds, unsigned char ** start, double * pts)
{
    int in_size;
    in_size = ds_get_packet(ds, start);
    *pts = ds->pts;
    return in_size;
}

void add_subtitles(char * filename, float fps, int noerr)
{
    sub_data * subd;
#ifdef CONFIG_ASS
    ASS_Track * asst = 0;
#endif

    if (filename == NULL) {
        return;
    }

    subd = sub_read_file(filename, fps);
#ifdef CONFIG_ASS

    if (ass_enabled && subd && !asst)

        //asst = ass_read_subdata(ass_library, subd, fps);
        if (!asst && !subd)
#else
    if (!subd)
#endif
#ifdef CONFIG_ASS
            if (!asst && !subd) {
                return;
            }

#else

            if (!subd) {
                return;
            }

#endif
    subdata = subd;
    //   printf("%s    %d\n",__func__,__LINE__);
}
int get_osd_subtitle(char ** start , int * startpts, int * endpts)
{
    int i;
    int sub_size = 0;
    //  char *tmp;
    subtitle * subs = NULL;
    subs = sub_temp ;

    if (sub_flag == 1) {
        // reverse order, since newest set_osd_msg is displayed first
        for (i = SUB_MAX_TEXT - 1; i >= 0; i--) {
            if (!subs || i >= subs->lines || !subs->text[i]) {
            } else {
                // HACK: currently display time for each sub line except the last is set to 2 seconds.
                int display_time = i == subs->lines - 1 ? 180000 : 2000;
                *startpts = subs->start;
                *endpts = subs->end;
                *start = subs->text[i];

                //printf("\n$$$$$$$$$SUB  %s  [%d]\n",*start , *start);
                while (*(subs->text[i]++)) {
                    sub_size++;
                }

                sub_flag = 0;
            }
        }

        return sub_size;
    } else {
        return 0;
    }
}
void set_osd_subtitle(subtitle * subs)
{
    int i;
    vo_sub = subs;
    sub_temp = NULL;

    // reverse order, since newest set_osd_msg is displayed first
    for (i = SUB_MAX_TEXT - 1; i >= 0; i--) {
        if (!subs || i >= subs->lines || !subs->text[i]) {
        } else {
            // HACK: currently display time for each sub line except the last is set to 2 seconds.
            int display_time = i == subs->lines - 1 ? 180000 : 2000;
            //      printf("##SUB   %s\n", subs->text[i]);
            sub_temp = subs;
            sub_flag = 1;
        }
    }
}

int load_subtitles_mp(const char * fname, float fps)
{
    vo_sub = NULL;
    sub_temp = NULL;
    subdata = NULL;
    sub_fps = 0;
    sub_flag = 0;
    load_subtitles(fname, fps, add_subtitles);

    if (subdata != NULL) {
        return 1;
    } else {
        return 0;
    }
}

void update_subtitles_mp(demux_stream_t * d_video, demux_stream_t * d_sub)
{
    sh_video_t * sh_video = d_video->sh;
    update_subtitles(sh_video, d_video->pts, d_sub, 0);
}


unsigned char  ds_check_packet_buffer_overflow(demux_stream_t * ds)
{
    struct demuxer * demuxer = NULL;

    //sunny, fix a bug for youku when preload data
    if (ds == NULL) {
        return 0;
    }

    demuxer = ds->demuxer;

    if (demuxer == NULL || demuxer->filename == NULL) {
        return 0;
    }

    if (strstr(demuxer->filename, "mms://")) {
        return 1;
    }

    if (ds->bytes >= MAX_PACK_BYTES / 4) {
        return 1;
    }

    return 0;
}

static void ds_reset_trickplay_para()
{
    gop_start = 0;
    gop_end = 0;
    write_flag = 0;
    speed_ctrl_cnt = 0;
    seek_back_end = 0;
    seek_back_cnt = 0;
    seek_cnt = 0;
    key_last = 0;
    seek_back_tick1 = 0;
    seek_back_tick2 = 0;
}
extern float trick_pts;
static float  video_pactet_interval = 0 ;
int ds_get_packet_video(demux_stream_t * ds, unsigned char ** start, int8_t speed)
{
    int in_size;
    sh_video_t * sh_video = ds->sh;
    static int8_t speed_old = TS_SEQ_NORMAL_PLAY;
    int ret = 0;
    float trick_pts_check = 0.0;
#ifdef SEEK_TICK_CACULATE
    u32 seek_tick1 = 0;
    u32 seek_tick2 = 0;
#endif
    static int tickets_last = 0;
    static int tickets_now = 0;

    if (speed_old != speed) {
        tickets_last = 0;
    }

    if (speed != TS_SEQ_NORMAL_PLAY) {
        tickets_now = mtos_ticks_get();

        if (tickets_last != 0) {
            //mtos_printk("\n%s %d now:%d last:%d\n",__func__,__LINE__,tickets_now,tickets_last);
            video_pactet_interval = (-tickets_last + tickets_now) * 1.f / 100;
            //demux_seek(ds->demuxer, (tickets_last-tickets_now)*1.f/100, 0, 0);
        }

        tickets_last = tickets_now;
    }

    while (1) {
        if (speed_old != speed) {
            //printf("ssssssssssssssspeed %d\n",speed);
            if (speed > TS_SEQ_NORMAL_PLAY && speed < TS_SEQ_REV_FAST_PLAY_2X) {
                if (!(speed == TS_SEQ_FAST_PLAY_2X && speed_old == TS_SEQ_NORMAL_PLAY)) {
                    ds_get_packet(ds, start);
                    trick_pts_check = trick_pts - ds->pts;
                    //                  if(trick_pts_check < 0) trick_pts_check = 0;
                    trick_pts_check += 2;
#ifdef SEEK_TICK_CACULATE
                    seek_tick1 = mtos_ticks_get();
#endif
//if(get_vpts_upload()>2)
{
                    demux_seek(ds->demuxer, trick_pts_check, 0, 0) ;
}
#ifdef SEEK_TICK_CACULATE
                    seek_tick2 = mtos_ticks_get();
                    OS_PRINTF("forward trick seek tick %d\n", seek_tick2 - seek_tick1);
#endif
                    gop_end = 0;
                    //ds_get_packet(ds, start);
                    //printf("#############speed change2 %d %f\n",speed,ds->pts);
                    speed_start = 1;
                }
            } else {
                if (!(speed == TS_SEQ_NORMAL_PLAY && speed_old == TS_SEQ_FAST_PLAY_2X)) {
#ifdef SEEK_TICK_CACULATE
                    seek_tick1 = mtos_ticks_get();
#endif
                    ds_get_packet(ds, start);
//		if(get_vpts_upload()>2)
			{
                    trick_pts_check = trick_pts - ds->pts;
                    demux_seek(ds->demuxer, trick_pts_check, 0, 0) ;
			}
                    //printf("############speed change1 %d %f\n",speed,ds->pts);
#ifdef SEEK_TICK_CACULATE
                    seek_tick2 = mtos_ticks_get();
                    OS_PRINTF("back trick seek tick %d\n", seek_tick2 - seek_tick1);
#endif
                }
            }

            ds_reset_trickplay_para();
        }

        speed_old = speed;
        in_size = ds_get_packet(ds, start);
        //in_size = ds_get_packet_audio(ds, &start, &p_extra_aud_buf, &extra_audio_size);

        //system("top -n 1 |grep Mem  >>memboard.txt");
        //OS_PRINTF("video packet, size: %d, pts:%d\n",in_size,(int)(ds->pts*1000));
        if ((in_size > 0)) {
            //if( ds_packet_seekforward(ds,2))
#ifdef SEEK_TICK_CACULATE
            seek_tick1 = mtos_ticks_get();
#endif
            ret = ds_packet_select(ds, start, speed, in_size);
#ifdef SEEK_TICK_CACULATE
            seek_tick2 = mtos_ticks_get();

            if (speed != TS_SEQ_NORMAL_PLAY) {
                OS_PRINTF("ds_packet_select seek tick %d\n", seek_tick2 - seek_tick1);
            }

#endif

            if (ret) {
                break;
            }
        } else {
            break;
        }
    }

    return in_size;
}
int ds_get_audio_count(demuxer_t * d)
{
    int i;
    int cnt = 0;

    if (d) {
        for (i = 0; i < MAX_A_STREAMS; ++i) {
            sh_audio_t * sh = d->a_streams[i];

            if (sh) {
                cnt++;
            }
        }
    }

    return cnt;
}


/**************************************************************************************
 * Function:    adts_UnpackFrmSize
 *
 * Description: parse the ADTS frame header and get÷°≥§
 *
 * Inputs:      readBuf
 *
 * Outputs:     framesize
 *
 * Return:      0 if successful, error code (< 0) if error
 *
 **************************************************************************************/
static int adts_UnpackFrmSize(unsigned char * buf, int * framesize)
{
    unsigned char tmp = 0;
    //buf[0,1]=0xFF F0
    tmp = buf[1] & 0x06;

    if (tmp != 0x0) { //layer != 0
        return -3;
    }

    tmp = (buf[2] & 0xc0) >> 6;

    if (tmp != 1 && tmp != 0) { //AAC_PROFILE_LC && tmp != AAC_PROFILE_MP
        return -3;
    }

    tmp = (buf[2] & 0x3c) >> 2;

    if (tmp >= 12) { //sampRateIdx >= NUM_SAMPLE_RATES
        return -3;
    }

    tmp = (buf[2] & 0x1) << 2;
    tmp |= (buf[3] & 0xc0) >> 6;

    if (tmp >= 12) { //channelConfig >= NUM_DEF_CHAN_MAPS
        return -3;
    }

    return 0;
}


int ds_get_packet_audio(demux_stream_t * ds, unsigned char ** start, uint8_t ** extra_buf, uint8_t ** extra_size)
{
    int in_size = ds_get_packet(ds, start);
    sh_audio_t * sh_audio = ds->sh;
    *extra_size = 0;

    if ((DEMUXER_TYPE_MPEG_TS == (ds->demuxer)->type)
        || (sh_audio && (sh_audio->format == 0x01 || sh_audio->format == MKTAG('B', 'P', 'C', 'M')))) {
        if (audio_codec_id == aAUDIO_PCM_BR) { //pcm
            unsigned char * p_fix_start = NULL;
            p_fix_start = *start;

            if (p_fix_start && in_size > 4) {
                memcpy(p_fix_start, p_fix_start + 4, in_size - 4);
                in_size = in_size - 4;
            }

            return in_size;
        }

        return in_size;
    }

    //OS_PRINTF("audio packet, size: %d, pts:%d\n",in_size,(int)(ds->pts*1000));
    //if((ds->pts*1000-debug_tmp)>50)
    //  printf("[%s]---------------apts[%d], delta [%d], while_cnt[%d]\n",__func__,(int)(ds->pts*1000),(int)(ds->pts*1000-debug_tmp),while_cnt);
    if (sh_audio == NULL) {
        return in_size;
    }

    if (in_size > 0)
        if (sh_audio->format == MKTAG('M', 'P', '4', 'A') || sh_audio->format == MKTAG('m', 'p', '4', 'a')) {
            unsigned char * p = *start;

            //hls doesnt needed
            if (ds->demuxer->type == DEMUXER_TYPE_LAVF) {
                unsigned int * priv = (unsigned int *)(ds->demuxer->priv);
                AVInputFormat * avif = (AVInputFormat *)(* priv);

                if (strstr(avif->name, "hls") || (io_isnetworkstream && ff_mpegts_pmt.use_ff_mpegts)) {
                    return in_size;
                }

                //else printf("ds_get_packet_audio error \n\n\n");
            }

            if (p && p[0] == 0xFF && (p[1] & 0xF0 == 0xF0)) {
                //there are some pkts all data is 0xff, so parser head
                if (adts_UnpackFrmSize(p, in_size) == 0) {
                    return in_size;
                }
            }

            uint8_t aac_buf[7] = {0xff, 0xf1, 0x40, 0x00, 0x00, 0x1f, 0xfc};
            unsigned int num_data_block = in_size / 1024;
            uint16_t frame_Length;
            int i = 0;
            int aud_sample_rate = sh_audio->samplerate;
            int aud_channels = sh_audio->channels;
            const int aac_sample_rates[16] = {
                96000, 88200, 64000, 48000, 44100, 32000,
                24000, 22050, 16000, 12000, 11025, 8000, 7350
            };
            *extra_size = 7;
            *extra_buf = malloc(*extra_size);

            if ((*extra_buf) == NULL) {
                printf("\n%s malloc failed\n", __FUNCTION__);
            }

            if (!aud_sample_rate && sh_audio->wf) {
                aud_sample_rate = sh_audio->wf->nSamplesPerSec;
                aud_channels = sh_audio->wf->nChannels;
                /*printf("[%s]---------------sh_audio->wf=%d,ch[%d],samplerate[%d]\n",\
                __func__,sh_audio->wf,sh_audio->wf->nChannels,aud_sample_rate);*/
            }

            for (i = 0; i < 16; i++)
                if (aud_sample_rate == aac_sample_rates[i]) {
                    break;
                }

            //printf("\ni%d\n",i);
            frame_Length = in_size + 7;
            /* frame size over last 2 bits */
            aac_buf[2] |= ((i & 0xf) << 2);
            aac_buf[3] |= (aud_channels << 6);

            if (aud_channels > 3) {
                aac_buf[2] |= (aud_channels >> 2);
            }

            aac_buf[3] |= (frame_Length & 0x1800) >> 11;// the upper 2 bit
            /* frame size continued over full byte */
            aac_buf[4] = (frame_Length & 0x1FF8) >> 3;// the middle 8 bit
            /* frame size continued first 3 bits */
            aac_buf[5] |= (frame_Length & 0x7) << 5;// < span style='font-size:12px;font-style:normal;font-weight:normal;font-family:'  Courier New monospacecolorrgb  >//the last 3 bit
            aac_buf[6] |= num_data_block & 0x03; //Set raw Data blocks.
            memcpy(*extra_buf, aac_buf, *extra_size);
            // printf("[%s] --------- i=%d, aud_sample_rate =%d, channels =%d, frame_len=%d\n",__func__,i,aud_sample_rate,aud_channels,frame_Length);
        }

    return in_size;
}


int ds_packet_seekforward(demux_stream_t * ds, int speed_cntl)
{
    static int key_sel = 0;
    int ret = 0;

    if (key_sel >= speed_cntl) {
        key_sel = 0;
    }

    if (ds->flags) {
        key_sel ++;
    }

    if ((key_sel == 1)) {
        ret = 1;
    }

    return ret;
}
/* NAL unit types */
enum {
    NAL_SLICE = 1,
    NAL_DPA,
    NAL_DPB,
    NAL_DPC,
    NAL_IDR_SLICE,
    NAL_SEI,
    NAL_SPS,
    NAL_PPS,
    NAL_AUD,
    NAL_END_SEQUENCE,
    NAL_END_STREAM,
    NAL_FILLER_DATA,
    NAL_SPS_EXT,
    NAL_AUXILIARY_SLICE = 19
};

static const uint8_t golomb_to_pict_type[5] = {
    AV_PICTURE_TYPE_P, AV_PICTURE_TYPE_B, AV_PICTURE_TYPE_I,
    AV_PICTURE_TYPE_SP, AV_PICTURE_TYPE_SI
};
int parse_nal_units(const uint8_t * buf)
{
    int h264_start ;
    memcpy(&h264_start, buf, 4);

    if (h264_start == 0x1000000)
        if ((*(buf + 4) & 0x1f) == NAL_SLICE) {
            uint8_t * NAL_start = buf + 5;
            {
                unsigned buf, log;
                int bit_skip;
                int byte_skip;
                unsigned int slice_type = 0;
                buf = ((*NAL_start) << 24) + ((*(NAL_start + 1)) << 16) + ((*(NAL_start + 2)) << 8) + ((*(NAL_start + 3)) << 0);
                byte_skip = (2 * log + 1) / 8;
                bit_skip = (2 * log + 1) % 8;
                NAL_start += byte_skip;
                buf = ((*NAL_start) << (24 + bit_skip)) + ((*(NAL_start + 1)) << (16 + bit_skip)) + ((*(NAL_start + 2)) << (8 + bit_skip)) + ((*(NAL_start + 3)) << (bit_skip)) + ((*(NAL_start + 4)) >> (8 - bit_skip));
                log = 31 - av_log2(buf);

                if (log > 16) {
                    printf("\nSLICE PARSER ERROR\n");
                }

                slice_type = ((buf << log) >> (31 - log)) - 1;
                slice_type = golomb_to_pict_type[slice_type % 5];
                //printf("\nbuf %x log %d  slice_type:%d\n",buf,log,slice_type);
                return slice_type;
                // skip_bits_long(gb, log);
                //return get_bits_long(gb, log + 1) - 1;
            }
        }
}

/* Start codes. */
#define SEQ_END_CODE            0x000001b7
#define SEQ_START_CODE          0x000001b3
#define GOP_START_CODE          0x000001b8
#define PICTURE_START_CODE      0x00000100
#define SLICE_MIN_START_CODE    0x00000101
#define SLICE_MAX_START_CODE    0x000001af
#define EXT_START_CODE          0x000001b5
#define USER_START_CODE         0x000001b

#   define AV_RB32(x)                                \
    (((uint32_t)((const uint8_t*)(x))[0] << 24) |    \
               (((const uint8_t*)(x))[1] << 16) |    \
               (((const uint8_t*)(x))[2] <<  8) |    \
                ((const uint8_t*)(x))[3])

static const uint8_t * mpv_find_start_code(const uint8_t * p,
        const uint8_t * end,
        uint32_t * state)
{
    int i;
    assert(p <= end);

    if (p >= end) {
        return end;
    }

    for (i = 0; i < 3; i++) {
        uint32_t tmp = *state << 8;
        *state = tmp + *(p++);

        if (tmp == 0x100 || p == end) {
            return p;
        }
    }

    while (p < end) {
        if (p[-1] > 1) {
            p += 3;
        } else if (p[-2]) {
            p += 2;
        } else if (p[-3] | (p[-1] - 1)) {
            p++;
        } else {
            p++;
            break;
        }
    }

    p = FFMIN(p, end) - 4;
    *state = AV_RB32(p);
    return p + 4;
}


/******************************
ret 0 :drop
     1 :not drop
*******************************/
static int mpeg_drop_b_frame(unsigned char * buf, int size)
{
    uint32_t start_code;
    uint8_t * buf_end;
    int bytes_left = 0;
    buf_end = buf + size;
    int pic_type = 0;
    start_code = -1;
    while (buf < buf_end) {
        buf = mpv_find_start_code(buf, buf_end, &start_code);
        bytes_left = buf_end - buf;
 
        switch (start_code) {
            case PICTURE_START_CODE:
                if (bytes_left >= 2) {
                    pic_type = (buf[1] >> 3) & 7;
                    if (pic_type == 3) {
                        return 0; //drop b frame
                    } else {
                        return 1;
                    }
                }
                break;
            default:
                break;
        }
    }
    return 1;
}

int ds_packet_select(demux_stream_t * ds, unsigned char ** start, int speed, int size)
{
#define SEEK_BACK_FILE_BEGIN_TIME   (-100)
    int sel = 1;
    sh_video_t * sh_video = ds->sh;
    int biComp;
    static int gop_cnt = 0;
    int speed_index = 0;
    int i = 0;
    int v_dec = 0;
    static float  last_key_pts = -2;
    static int last_speed_index = 0;
    static int seek_before_tick = 0;
    static int seek_after_tick = 0;

    if (speed == TS_SEQ_NORMAL_PLAY || speed == TS_SEQ_FAST_PLAY_2X) {
        last_key_pts = -2;
        seek_before_tick = 0;
        seek_after_tick = 0;
        return 1;
    }

    switch (speed) {
        case TS_SEQ_FAST_PLAY_4X:
            speed_index = 2;
            break;
        case TS_SEQ_FAST_PLAY_8X:
            speed_index = 4;
            break;
        case TS_SEQ_FAST_PLAY_16X:
            speed_index = 8;
            break;
        case TS_SEQ_FAST_PLAY_32X:
            speed_index = 16;
            break;
        case TS_SEQ_REV_FAST_PLAY_2X:
            speed_index = -2;
            break;
        case TS_SEQ_REV_FAST_PLAY_4X:
            speed_index = -4;
            break;
        case TS_SEQ_REV_FAST_PLAY_8X:
            speed_index = -8;
            break;
        case TS_SEQ_REV_FAST_PLAY_16X:
            speed_index = -16;
            break;
        case TS_SEQ_REV_FAST_PLAY_32X:
            speed_index = -32;
            break;
        default:
            speed_index = 1;
            last_key_pts = -2;
            seek_before_tick = 0;
            seek_after_tick = 0;
            return 1;
    }

    if (last_speed_index != speed_index) {
        last_speed_index = speed_index;
        last_key_pts = -2;
        seek_before_tick = 0;
        seek_after_tick = 0;
    }

    // printf("\n %f\n",sh_video->frametime);
    if (ds->demuxer->type == DEMUXER_TYPE_MPEG_PS) {
        v_dec = 0;
        return 1;
    } else if (DEMUXER_TYPE_MPEG_TS == ds->demuxer->type) {
        ts_priv_t * priv = (ts_priv_t *) ds->demuxer->priv;
        pmt_t * pmt;
        //ds_ts_prog(ds->demuxer);
        /*
        audio_id, video_id, dvdsub_id
        */
        pmt = (pmt_t *)(&priv->pmt[prog_id_index]);
        if(pmt)
        	{
        for (i = 0; i < pmt->es_cnt; i++) {
            if ((pmt->es[i].type == VIDEO_MPEG1_MP) || (pmt->es[i].type == VIDEO_MPEG2_MP)) {
                v_dec = 0;
                goto TS_TRICK;
            } else if (pmt->es[i].type == VIDEO_H264_MP) {
                v_dec = 1;
                goto TS_TRICK;
            } else {
                printf("ts trick ERROR not support %d !!!!!!!!!!!\n",
                       pmt->es[video_id_index].type);
            }
        }
        	}
		else
			{
			if(sh_video)
				{
			  switch (sh_video->format) {
                    case VIDEO_MPEG1_MP:
						                 case VIDEO_MPEG2_MP:
			             v_dec = 0;
						 case VIDEO_H264_MP:
						 	v_dec = 1;
						 	
			  	}
                goto TS_TRICK;
				}
			}
        return 1;
    } else if (sh_video->bih) {
        biComp = le2me_32(sh_video->bih->biCompression);

        if (strstr(((char *) &biComp), "mpg2") || (strstr(((char *) &biComp), "MPG2"))
            || (strstr(((char *) &biComp), "mpg1")) || (sh_video->bih->biCompression == 1)) {
            v_dec = 0;
            goto NORMAL_TRICK;
        } else if (strstr(((char *) &biComp), "H264")  || (strstr(((char *) &biComp), "avc1"))
                   || strstr(((char *) &biComp), "h264")) {
            v_dec = 1;
            goto NORMAL_TRICK;
        } else { //mpeg 4
            v_dec = 1;
            goto NORMAL_TRICK;
            //printf("ERROR not support %s !!!!!!!!!!!6\n", (char *)&biComp);
            return 1;
        }
    } else {
        printf("ERROR not support %.4s !!!!!!!!!!!\n", (char *)&biComp);
        return 1;
    }

TS_TRICK:

    if (ds->demuxer->type == DEMUXER_TYPE_MPEG_PS ||
        DEMUXER_TYPE_MPEG_TS == ds->demuxer->type) {
        static int seek_error_protect = 0;
        int frm_max = 0;
        //printf("pts %f  size %d \n",ds->pts,size);

        if (speed_index > 0) {
            frm_max = (2 * 25 / speed_index);

            if (write_flag == 1 || write_flag == 2) {
                //this frame must be the first 2frames after the I frame
                if (v_dec == 0) { //mpeg
                    if (mpeg_drop_b_frame(*start, size) == 0) {
                        write_flag++;
                        return 0;
                    }
                }

                //h264
            }

            write_flag ++;

            if (write_flag > frm_max) {
                // yliu add for ts trick
                float seek_modify = 0;

                if (seek_after_tick != 0) {
                    seek_modify = (seek_after_tick - seek_before_tick) * 1.f / 100;
                }

                seek_before_tick = mtos_ticks_get();
                demux_seek(ds->demuxer, 2 + seek_modify, 0, 0); //forward 1s
                seek_after_tick = mtos_ticks_get();
                write_flag = 0;
                //OS_PRINTF("seek here %d frm_max \n",frm_max);
                return 0;
            }

            return 1;
        } else {
            frm_max = -(30 / speed_index);

            if (seek_back_end == 1) {
                //this is the frame right after i frame
                if (write_flag >= frm_max) {
                    //need to seek back 2 s
                    demux_seek(ds->demuxer, -2 - video_pactet_interval, 0, 0); //forward -2s
                    video_pactet_interval = 0;
                    write_flag = 0;
                    //OS_PRINTF("seek here %d frm_max \n",frm_max);
                } else {
                    //need to seek back to the i frame just before
                    demux_seek(ds->demuxer, -0.4 - video_pactet_interval, 0, 0); //forward 1s
                    video_pactet_interval = 0;
                }

                seek_back_end = 0;
                return 0;
            }

            if (seek_back_end == 0) {
                if (ds->pts - gop_end > -0.01 && gop_end != 0) {
                    //this is error
                    //OS_PRINTF("this is error ,seek back \n");
                    seek_error_protect ++;
                    // yliu add for ts trick
                    demux_seek(ds->demuxer, -2 * seek_error_protect - video_pactet_interval, 0, 0); //forward 1s
                    video_pactet_interval = 0;

                    if (seek_error_protect > 50) {
                        return 1;
                    }

                    return 0;
                }

                seek_error_protect = 0;
                gop_end = ds->pts;
                //this is i frame
                seek_back_end = 1;
            }

            write_flag ++;
            return 1;
        }
    }

    return 1;
NORMAL_TRICK:

    if (speed_index > 0) {
        if (ds->flags && write_flag == 0) {
            seek_keyframe = 1;

            if (ds->pts - gop_start < 0.01 && ds->pts - gop_start > -0.01) {
                //mtos_printk_f("444 pts %f \n",gop_end);
                return 0; //file is end
            }

            gop_end = ds->pts + speed_index; //not read to save time
            gop_start = ds->pts;
            // float pts_seek;
            // pts_seek =ds->pts;
            //mtos_printk_f("111before pts %f \n",ds->pts);
            //demux_seek(ds->demuxer, speed_index, 0, 0) ;
            //ds_get_packet(ds, start);
            // mtos_printk_f("111after pts %f  \n",ds->pts);
            //demux_seek(ds->demuxer, gop_start, 0, 1) ;
            //ds_get_packet(ds, start);
            //if(gop_start -ds->pts > 0.1 || gop_start -ds->pts < -0.1)
            //mtos_printk_f("222seek back gopstart error pts %f  \n",ds->pts);
            gop_time = speed_index;
            write_flag = 1;
            return 1;
        } else {
            if ((ds->pts - gop_start < gop_time / speed_index) && (write_flag)) {
                write_flag ++;

                if (write_flag == 2 || write_flag == 3) {
                    //this frame must be the first 2frames after the I frame
                    if (v_dec == 0) { //mpeg
                        if (mpeg_drop_b_frame(*start, size) == 0) {
                            return 0;
                        }
                    }

                    //h264
                }
            } else {
                if (gop_end - gop_start < 0.01 && gop_end - gop_start > -0.01) {
                    return 0; //file is end
                }

                if (write_flag) {
                    demux_seek(ds->demuxer, gop_end, 0, 1) ;
                    seek_keyframe = 0;
                    write_flag = 0;
                    //yliu add
                    return 0;
                }
            }
        }

        return seek_keyframe;
    } else if (speed_index < 0) {
        float seek_back_time = 0;
        static float seek_adj = 0;

        // mtos_printk("\n%s %d pts:%d flags:%d\n",__func__,__LINE__,(int)(ds->pts*1000),(int)(ds->flags));
        if (ds->flags & (seek_back_end == 0)) {
            seek_cnt++;
            // gop_time = ds->pts  -gop_start;
            /*
                            if (seek_cnt > 1) {
                                demux_seek(ds->demuxer, -2*sh_video->frametime, 0, 0);
                                ds_get_packet(ds, start);
                            }
            */
            gop_end = ds->pts;

            //yliu modify for after seek not keyframe
            if (fabs(ds->pts - last_key_pts) >= 0.1 || last_key_pts < 0) {
                // mtos_printk("\n%s %d pts:%d key pts:%d\n",__func__,__LINE__,
                //  (int)(ds->pts*1000),(int)(last_key_pts*1000));
                last_key_pts = ds->pts;
                seek_adj = 0;
            } else if (fabs(last_key_pts - ds->pts) < 0.1) {
                seek_adj += speed_index;
                demux_seek(ds->demuxer, seek_adj, 0, 0);
            }

            //back seek need 3 frame?
            //ds_get_packet(ds, start);
            demux_seek(ds->demuxer, speed_index, 0, 0);
            ds_get_packet(ds, start);
            gop_start = ds->pts;
            gop_time = gop_end - gop_start;

            if (gop_time < 0.1) {
                //dmxseek error
                gop_start = gop_start - 10;

                if (gop_start < 0) {
                    gop_start = 0;
                }

                seek_back_time = gop_start > 1 ? gop_start : -0.1;
                //yliu modify for after seek not keyframe
                demux_seek(ds->demuxer, seek_back_time, 0, 1);
                ds_get_packet(ds, start);
                //mtos_printk_f("555after pts %f  \n",ds->pts);
                gop_start = ds->pts;
                gop_time = gop_end - gop_start;
            }

#if 0

            if (sh_video) {
                gop_cnt = gop_time / sh_video->frametime;
            } else
#endif
                gop_cnt = gop_time / 0.05;

            //printf("\n this gop frame cnt :%d %f start  %f end %f\n",gop_cnt,sh_video->frametime,gop_start,gop_end);
            ds_get_packet(ds, start);
            //yliu modify for after seek not keyframe
            seek_back_time = (gop_start - 4 * 0.4) > 1 ? (gop_start - 4 * 0.4) : SEEK_BACK_FILE_BEGIN_TIME;
            demux_seek(ds->demuxer, seek_back_time, 0, 1);
            //ds_get_packet(ds, start);
            seek_back_end = 1;
            seek_back_cnt = gop_cnt;
            seek_back_tick1 = mtos_ticks_get();
            return 0;
        } else if (ds->flags & (seek_back_end == 1)) {
            seek_back_end = 2 ;
            return 1;
        } else if (seek_back_end == 2) {
            seek_back_tick2 = mtos_ticks_get();

            if ((seek_back_cnt > gop_cnt + gop_cnt / (speed_index))
                && seek_back_tick2 - seek_back_tick1 < gop_time * 100 / (-speed_index)) {
                //yliu modify for after seek not keyframe
                seek_back_time = (gop_start - 4 * 0.4) > 1 ? (gop_start - 4 * 0.4) : SEEK_BACK_FILE_BEGIN_TIME;
                demux_seek(ds->demuxer, seek_back_time, 0, 1);
                seek_back_cnt --;
                seek_back_end = 1;
                return 0;
            }

            //yliu modify for after seek not keyframe
            seek_back_time = (gop_start - 4 * 0.4) > 1 ? (gop_start - 4 * 0.4) : SEEK_BACK_FILE_BEGIN_TIME;
            demux_seek(ds->demuxer, seek_back_time, 0, 1);
            seek_back_end = 0;
            return 0;
        }

        //yliu modify for after seek not keyframe
        return 0;
    }

    return 1;
}
#endif
