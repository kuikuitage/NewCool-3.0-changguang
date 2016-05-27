/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

// system
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
// driver
#include "driver.h"
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_misc.h"
#include "hal_uart.h"

#include "drv_dev.h"
#include "drv_misc.h"
#include "uio.h"
#include "nim.h"
#include "dmx.h"
#include "lib_rect.h"

#include "pti.h"

#include "common.h"
#include "region.h"
#include "display.h"
#include "gpe_vsb.h"
#include "vdec.h"
#include "aud_vsb.h"
//#include "audio.h"   //not used, please aud_vsb.h instead


#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "list.h"
#include "drv_dev.h"
#include "sys_types.h"
#include "sys_define.h"
#include "block.h"
#include "ff.h"
#include "fsioctl.h"
#include "ufs.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#else
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_printk.h"
#include "mp_func_trans.h"
#endif

#include "../ts_seq/fifo_kw.h"
#include "libmpdemux/stheader.h"

#include "stheader.h"

#define TEST_VIDEO  1
#define TEST_AUDIO  1
#define USE_MPLAYER_DEMUXER
#define DUMP_AUDIO 1
#define AUTO_SEND_VPTS
#define AUTO_SEND_VIDEO_INFO
#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))

#ifdef USE_MPLAYER_DEMUXER
#include "demuxer.h"
#include "stream.h"
#else
#include "list.h"
#include "block.h"
#include "ff.h"
#include "fsioctl.h"
#include "lib_util.h"
#include "ufs.h"
#endif
#include "lib_memp.h"
#include "ts_sequence.h"
#include "file_playback_sequence.h"
#include "demux_mp.h"
#include  "file_seq_internal.h"
#include "download_api.h"

#if  defined(USE_SCRIPT_FUNCTION)
#include "simple_script_api.h"
#endif
extern void srand(unsigned int seed);
extern int rand(void);
#ifndef  SDK_1
#include ".hg_ver.h"
#endif
#ifndef __LINUX__
#include "mp_func_trans.h"
#else
#define MT_ASSERT
#endif
//#include  "record_seq.h"

#define RECORD_FILE_TAIL    ".musbc"

#ifndef USE_MPLAYER_DEMUXER
char * aes_file_name = "stream.dump.audio2";
char * ves_file_name = "stream.dump.video2";
#endif
extern double first_vpts;
#define   DBG_FILE_PLAYBACK_SEQ

#ifdef     DBG_FILE_PLAYBACK_SEQ
#define FILE_SEQ_LOG(format, args...)              mtos_printk(format, ##args)
#define FILE_SEQ_WARNING(format, args...)     OS_PRINTF(format, ##args)
#define FILE_SEQ_DEBUG(format, args...)          OS_PRINTF(format, ##args)
#define FILE_SEQ_ERROR(format, args...)          mtos_printk(format, ##args)
#else
#define FILE_SEQ_LOG(format, args...)               OS_PRINTF(format, ##args)
#define FILE_SEQ_WARNING(format, args...)
#define FILE_SEQ_DEBUG(format, args...)
#define FILE_SEQ_ERROR(format, args...)           mtos_printk(format, ##args)
#endif
extern  int register_http_stream_is();
extern void fill_camera_es_task(void * p_para);

#define FILE_PLAYBACK_DEBUG

//#define  NOT_PUSH_ES_TO_DECODER   //only for debug rtsp live on
//#define   DEBUG_FILE_SEQ

#define RECORD_FILE_TAIL    ".musbc"

#define SWITCH_DEBUG


#define PRELOAD_VIDEO_BUFFER_SIZE               (1*1024*1024)
#define PRELOAD_AUDIO_BUFFER_SIZE               (256*1024)



#define  VES_SEG_LEN  (1024)
#define  AES_SEG_LEN   (64)
//global variable


#define AUDIO_SEND 2

#define CONTINUE_READ_AUD_PACKETS_THRESHOLD  100
#define CONTINUE_READ_VID_PACKETS_THRESHOLD  50

#define SEG_AUD_SIZE (AUD_ASSEMBLE_BUF_LEN - AUDIO_ES_BUF_OVERFLOW_THRESHOLD)



#ifdef TIME_DEBUG
static struct timeval dbg_time_start;
static struct timeval dbg_time_stop;
#endif

static int load_task_state = 0;
int pbfifo_task_state = 0;

extern int demux_state;
//yliu
#define MUL_SEEK 1
static char * stream_dump_name = "mt.dump";
static char * video_dump = "video.dump";
static char * audio_dump = "audio.dump";
static char * filename = "file_playback_test_file/avi/rzdfc.avc_aac_480x272.avi"; //"zx.ts";
static int self_exit = 0;
extern  int audio_id, video_id, dvdsub_id;
extern int revise_fps;
int h264_frm_only_flag = 1;
int mp_field_pic_flag = 0;
int io_stream_buffer_size = 256 * 1024;
int io_isnetworkstream = 0;
int special_ts_vpid = -1;///special video pid¡ê?ts use, add by libin 20140818
int special_ts_apid = -1;///special audio pid¡ê?ts use, add by libin 20140818
static  void * g_player_handle = NULL;
int num_fill_buffer=0;


ERR_LOAD g_load_err = ERR_LOAD_UNKNOW;

#ifdef  AUTO_SEND_VPTS
//static unsigned long vpts_upload=0;
static unsigned long  last_send_orig_vpts = 0;
#endif
static unsigned long int g_last_get_vpts = 0;
static void file_seq_set_muti_path(void * p_handle, char * p_path[],  u32 number);
extern int  run_memory(BOOL isflushCache);
extern unsigned short   *  Convert_Utf8_To_Unicode(unsigned char * putf8, unsigned short * out) ;
extern int file_get_audio_mute_onoff();

//doreen add for debugging
#ifdef FILE_PLAYBACK_DEBUG
int flag_debug = 0;
int while_cnt = 0;
int debug_tmp = 0;
int total_tcp_recv_size = 0;
int compute_Bps = 0;
int a_ticks = 0;
int v_ticks = 0;
int ves_left = 0;
int aes_left = 0;
int max_video_pkt = 0, max_audio_pkt = 0;
#endif

extern void set_audio_param_to_vsb(void);

unsigned int backup_avsync_status;
int g_is_live_broadcast = 0;


//static void x_stop_av_decoder(void * pHandle);
//static  void x_unLoadMedia(void * p_handle);
//static void x_unmap_es_buffer(void * pHandle);
//static void    handle_exit_fill_es_task();
//static BOOL isEsWaterLevelEnough(int sec);
//static BOOL  x_check_av_codec_type(void * p_param);
//static BOOL  x_check_aud_codec_type(void * p_handle);
//static void x_stop_av_decoder(void * pHandle);
//static  void x_clear_pti_es_counter();
static void file_seq_get_subt_info(void * p_handle, void ** subt_info);
static  int file_seq_pause(void * p_handle);
//static  BOOL adec_push_audio_es(u8 * start_a, int in_size_a, u32 apts);
static s8 file_seq_parser_speed(void * p_handle, s8 speed);
//static void file_seq_check_trickplay(void * p_handle);

#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
//static void * file_seq_mem_alloc(void * p_handle, u32 size);
//static void file_seq_mem_free(void * p_handle, void * p_buf);
static void file_seq_mem_init(void * p_handle, u8 * p_ext_buf_addr, u32 ext_buf_size);
static void file_seq_mem_release(void * p_handle);
#endif

#ifndef TEST_SPEED_OPEN
static char * parse_html_to_get_inner_string(char * string, char * front, char * end, char ** new_start)
{
    char * start, *tail, *tmp;
    int front_len, end_len, len;
    int i = 0 ;
    int j = 0 ;
    char * url_tmp = NULL;

    if (string == NULL) {
        return NULL;
    }

    front_len = strlen(front);
    end_len = strlen(end);
    start = strstr(string, front);

    if (start == NULL) {
        return NULL;
    }

    start += front_len;
    tail = strstr(start, end);

    if (tail == NULL) {
        return NULL;
    }

    len = tail - start;
    url_tmp = (char *)mtos_malloc(len + 1);
    memset(url_tmp, 0x00 , len + 1);
    tmp = (char *)mtos_malloc(len + 1);
    memset(tmp, 0x00 , len + 1);
    memcpy(url_tmp, start, len);

    for (i = 0 ; i < len ; i ++) {
        if (url_tmp[i] != '\\') {
            tmp[j] = url_tmp[i] ;
            j++ ;
        }
    }

    *new_start = tail + end_len;
    mtos_free(url_tmp);
    return tmp;
}
#endif
BOOL  x_check_aud_codec_type(void * p_handle)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    int audio_codec = 0xffff ;
    BOOL isSupportAudioType = FALSE;
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);
    FILE_SEQ_DEBUG("[%s] -----para in:  p_ds_audio=%x, p_ds_video=%x\n", __func__, p_file_seq->p_ds_audio, p_file_seq->p_ds_video);
    p_file_seq->audio_bps = ds_get_audio_bps(p_file_seq->p_ds_audio);
    FILE_SEQ_DEBUG("[%s] ----- audio_bps=%d\n", __func__, p_file_seq->audio_bps);
    ds_get_audio_codec_type(p_file_seq->p_ds_audio, &p_file_seq->m_audio_codec_type,
                            &p_file_seq->audio_pid, p_file_seq->audio_output_mode);
    FILE_SEQ_DEBUG("[%s] ----- m_audio_codec_type=%d, audio_pid=%d\n", \
                   __func__, p_file_seq->m_audio_codec_type, p_file_seq->audio_pid);
    audio_codec = p_file_seq->m_audio_codec_type;

    switch (audio_codec) {
        case  AUDIO_MP2:
        case  AUDIO_MP3:
        case  AUDIO_AC3_VSB:
        case  AUDIO_EAC3:
        case  AUDIO_AAC:
        case  AUDIO_SPDIF:
        case  AUDIO_PCM:
            isSupportAudioType = TRUE;
            p_file_seq->is_av_codec_support = 1;
            break;
        default:
            break;
    }

    if (isSupportAudioType) {
        p_file_seq->checkAvTypeOK = TRUE;
        FILE_SEQ_DEBUG("[%s] -----end, returen TRUE  \n", __func__);
        return TRUE;
    } else {
        FILE_SEQ_DEBUG("[%s] fail to check aud codec type !!!\n", __func__);
        FILE_SEQ_DEBUG("[%s] audio_codec[%d] !!!\n", __func__, audio_codec);
    }

    FILE_SEQ_DEBUG("[%s] -----end, return FALSE! \n", __func__);
    return FALSE;
}


/********************************************************************
 *
 *
 *     NOTICE:     The fowwing static routing can not be exposed
 *
 *                peacer 2013-02-07
 *
 ***********************************************************************
 */


FILE_SEQ_T * x_get_cur_instance()
{
    return g_player_handle;
}




FILE_SEQ_T * file_seq_get_instance()
{
    return g_player_handle;
}
BOOL is_file_seq_exit()
{
    FILE_SEQ_T * p_seq = file_seq_get_instance();

    if (p_seq && p_seq->m_play_state ==  FILE_SEQ_EXIT && (p_seq->is_task_alive || p_seq->is_load_task_alive)) {
        return TRUE;
    } else {
        return FALSE;
    }
}
int fp_set_file_timeshift(int is_timeshift)
{
    FILE_SEQ_T * p_seq = file_seq_get_instance();
((PLAYBACK_INTERNAL_T *)(p_seq->pb_internal))->is_timeshift = is_timeshift;

}
extern int socket_err;
int fp_get_socket_error()
{
  return socket_err;
}
int  fp_is_timeshift_file()
{
    FILE_SEQ_T * p_seq = file_seq_get_instance();
return ((PLAYBACK_INTERNAL_T *)(p_seq->pb_internal))->is_timeshift ;

}
int  fp_is_trick_mode()
{
    FILE_SEQ_T * p_seq = file_seq_get_instance();
    return (p_seq->tmp_speed!=0)&&(p_seq->tmp_speed!=1);

}
BOOL  fp_is_loadmedia_state()
{
    FILE_SEQ_T * p_seq = file_seq_get_instance();

    if (p_seq && p_seq->m_play_state ==  FILE_SEQ_LOADMEDIA ) {
        return TRUE;
    } else {
        return FALSE;
    }

}
#include "commonData.h"
#include  "tvLiveDataProvider.h"
#ifdef USE_MPLAYER_DEMUXER
typedef struct {
    unsigned char * buf;
    unsigned int bufsize;
    unsigned int curr_offset;
} mem_cache_t;
int mem_cache_create(mem_cache_t * cache, unsigned int cache_size)
{
    if (cache == NULL || cache_size == 0) {
        return -1;
    }

    cache->buf = (unsigned char *)mtos_malloc(cache_size);

    if (cache->buf == NULL) {
        return -1;
    }

    cache->bufsize = cache_size;
    cache->curr_offset = 0;
    return 0;
}

int mem_cache_push(mem_cache_t * cache, unsigned char * data, unsigned int data_size)
{
    unsigned int tmp;

    if (cache == NULL || data == NULL || data_size == 0) {
        return -1;
    }

    if (cache->curr_offset + data_size <=  cache->bufsize) {
        memcpy(cache->buf + cache->curr_offset, data, data_size);
        cache->curr_offset += data_size;
        return data_size;
    } else {
        tmp = cache->bufsize - cache->curr_offset;
        memcpy(cache->buf + cache->curr_offset, data, tmp);
        cache->curr_offset += tmp;
        return tmp;
    }
}

void mem_cache_del(mem_cache_t * cache)
{
    if (cache == NULL) {
        return;
    }

    if (cache->buf) {
        mtos_free(cache->buf);
        cache->buf = NULL;
    }

    cache->bufsize = 0;
    cache->curr_offset = 0;
    return;
}
/*
 *
 *
 *dump video
 *
 *
 */
static void file_seq_mp_dump(void)
{
    FILE_SEQ_DEBUG("[%s] ==start start\n", __func__);
    // current_module = "demux_open";
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    mem_cache_t cache = {0};
    demuxer_t * demuxer;
    stream_t * stream;
    int file_format;
    int usedsize;
    stream  = open_stream(p_file_seq->m_path[0], 0, &file_format);

    if (stream == NULL) {
        return;
    }

    FILE_SEQ_DEBUG("\nmpctx->stream:%x,filename:%s\n", stream, p_file_seq->m_path[0]);
    demuxer = demux_mp_open(stream, file_format, audio_id, video_id, dvdsub_id, p_file_seq->m_path[0]);

    if (demuxer == NULL) {
        return;
    }

    //   if(demuxer ==NULL)
    if (DEMUXER_TYPE_MPEG_TS == demuxer->type) {
        p_file_seq ->ts_priv = ds_ts_prog(demuxer);
        FILE_SEQ_DEBUG("\nthis is ts file\n");

        if (demuxer) {
            free_demuxer(demuxer);
        }

        if (stream) {
            free_stream(stream);
        }

        return ;
    }

    {
        FILE_SEQ_DEBUG("\ndemux open %d %d\n", demuxer->audio->id, demuxer->video->id);
    }

    {
#ifdef __LINUX__
        FILE * f;
#else
        ufs_file_t file_handle;
        u32 op_size = 0;
#endif
        demux_stream_t * ds = NULL;
        demux_stream_t * ds2 = NULL;
        int in_size = 0;
        unsigned char * start;
        unsigned char * start2;
        // select stream to dump
        ds = demuxer->video;
        ds2 = demuxer->audio;
        // let's dump it!
#ifdef __LINUX__
        f = fopen(stream_dump_name, "wb");
#else
        memset(&file_handle, 0, sizeof(ufs_file_t));
        unsigned short  path_tmp[256] = {0};
        u16 * p_name1 = Convert_Utf8_To_Unicode(video_dump, path_tmp);
        u8 ret = ufs_open(&file_handle, p_name1, UFS_WRITE | UFS_CREATE_NEW_COVER);

        if (ret == FR_OK) {
            FILE_SEQ_DEBUG("Open Ufs dump file OK\n");
        } else {
            FILE_SEQ_DEBUG("Open Ufs dump file FAIL\n");
        }

#endif
        //stream_dump_progress_start();

        if (mem_cache_create(&cache, 4 * 1024) == -1) {
            FILE_SEQ_DEBUG("[%s] mem_cache_create 4*1024 error\n", __func__);
        } else {
            while (!ds->eof) {
                in_size = ds_get_packet(ds, &start);

                //skip audio if audio pts is smaller than video pts
                while (((ds->pts - ds2->pts) > 1) && (ds_get_packet(ds2, &start2) != -1)) {
                    ;
                }

                //printf("***************get pkt size %d\n",in_size);
                if (in_size > 0) {
                    usedsize = mem_cache_push(&cache, start, in_size);

                    //printf("push size %d\n",usedsize);
                    while (usedsize != in_size && usedsize != -1) {
#ifdef __LINUX__
                        fwrite(cache.buf, cache.bufsize, 1, f);
#else
                        ufs_write(&file_handle, cache.buf, cache.bufsize, &op_size);
                        //printf("[%s]--------write data to file, len = %d\n", __func__,cache.bufsize);
#endif
                        cache.curr_offset = 0;
                        in_size -= usedsize;
                        start += usedsize;
                        usedsize = mem_cache_push(&cache, start, in_size);
                        //printf("push size %d\n",usedsize);
                    }
                }
            }
        }

        if (cache.curr_offset) {
#ifdef __LINUX__
            fwrite(cache.buf, cache.curr_offset, 1, f);
#else
            ufs_write(&file_handle, cache.buf, cache.curr_offset, &op_size);
            printf("[%s]--------write left data to file, len = %d\n", __func__, cache.curr_offset);
#endif
        }

#ifdef __LINUX__
        fclose(f);
#else
        ufs_close(&file_handle);
#endif
        mem_cache_del(&cache);

        if (demuxer) {
            free_demuxer(demuxer);
        }

        if (stream) {
            free_stream(stream);    // kill cache thread
        }

        //stream_dump_progress_end();
    }

    // initialized_flags |= INITIALIZED_DEMUXER;
}

/*
 *
 *
 *
 */
//#define AUD_EXTRA_HEADER_WRITE
static void file_seq_mp_dump_aud(void)
{
    FILE_SEQ_DEBUG("[%s] ==start start\n", __func__);
    // current_module = "demux_open";
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    mem_cache_t cache = {0};
    demuxer_t * demuxer;
    stream_t * stream;
    int file_format;
    int usedsize;
    stream  = open_stream(p_file_seq->m_path[0], 0, &file_format);

    if (stream == NULL) {
        return;
    }

    FILE_SEQ_DEBUG("\nmpctx->stream:%x,filename:%s\n", stream, p_file_seq->m_path[0]);
    demuxer = demux_mp_open(stream, file_format, audio_id, video_id, dvdsub_id, p_file_seq->m_path[0]);

    if (demuxer == NULL) {
        return;
    }

    //   if(demuxer ==NULL)
    if (DEMUXER_TYPE_MPEG_TS == demuxer->type) {
        p_file_seq ->ts_priv = ds_ts_prog(demuxer);
        FILE_SEQ_DEBUG("\nthis is ts file\n");

        if (demuxer) {
            free_demuxer(demuxer);
        }

        if (stream) {
            free_stream(stream);
        }

        return ;
    }

    {
        FILE_SEQ_DEBUG("\ndemux open %d %d\n", demuxer->audio->id, demuxer->video->id);
    }

    {
#ifdef __LINUX__
        FILE * f;
#else
        ufs_file_t file_handle;
        u32 op_size = 0;
#endif
        demux_stream_t * ds = NULL;
        demux_stream_t * ds2 = NULL;
        int in_size = 0;
        unsigned char * start;
        unsigned char * start2;
        // select stream to dump
        ds = demuxer->audio;
        ds2 = demuxer->video;
        // let's dump it!
#ifdef __LINUX__
        f = fopen(stream_dump_name, "wb");
#else
        memset(&file_handle, 0, sizeof(ufs_file_t));
        unsigned short  path_tmp[256] = {0};
        u16 * p_name1 = Convert_Utf8_To_Unicode(audio_dump, path_tmp);
        u8 ret = ufs_open(&file_handle, p_name1, UFS_WRITE | UFS_CREATE_NEW_COVER);

        if (ret == FR_OK) {
            FILE_SEQ_DEBUG("Open Ufs dump file OK\n");
        } else {
            FILE_SEQ_DEBUG("Open Ufs dump file FAIL\n");
        }

#endif
        //stream_dump_progress_start();

        if (mem_cache_create(&cache, 4 * 1024) == -1) {
            FILE_SEQ_DEBUG("[%s] mem_cache_create 4*1024 error\n", __func__);
        } else {
            while (!ds->eof) {
                in_size = ds_get_packet(ds, &start);

                //skip video if video pts is smaller than audio pts
                while (((ds->pts - ds2->pts) > 1) && (ds_get_packet(ds2, &start2) != -1)) {
                    ;
                }

                //printf("***************get pkt size %d\n",in_size);
                if (in_size > 0) {
#ifdef AUD_EXTRA_HEADER_WRITE
                    usedsize = mem_cache_push(&cache, p_file_seq->p_extra_aud_buf, (unsigned int)p_file_seq->extra_audio_size);

                    //printf("********header size %d\n",p_file_seq->extra_audio_size);
                    if (usedsize != in_size && usedsize != -1) {
#ifdef __LINUX__
                        fwrite(cache.buf, cache.bufsize, 1, f);
#else
                        ufs_write(&file_handle, cache.buf, cache.bufsize, &op_size);
                        //printf("[%s]--------write data to file, len = %d\n", __func__,cache.bufsize);
#endif
                        cache.curr_offset = 0;
                        usedsize = mem_cache_push(&cache, p_file_seq->p_extra_aud_buf + usedsize, (unsigned int)p_file_seq->extra_audio_size - usedsize);
                    }

#endif
                    usedsize = mem_cache_push(&cache, start, in_size);

                    //printf("push size %d\n",usedsize);
                    while (usedsize != in_size && usedsize != -1) {
#ifdef __LINUX__
                        fwrite(cache.buf, cache.bufsize, 1, f);
#else
                        ufs_write(&file_handle, cache.buf, cache.bufsize, &op_size);
                        //printf("[%s]--------write data to file, len = %d\n", __func__,cache.bufsize);
#endif
                        cache.curr_offset = 0;
                        in_size -= usedsize;
                        start += usedsize;
                        usedsize = mem_cache_push(&cache, start, in_size);
                        //printf("push size %d\n",usedsize);
                    }
                }
            }
        }

        if (cache.curr_offset) {
#ifdef __LINUX__
            fwrite(cache.buf, cache.curr_offset, 1, f);
#else
            ufs_write(&file_handle, cache.buf, cache.curr_offset, &op_size);
            printf("[%s]--------write left data to file, len = %d\n", __func__, cache.curr_offset);
#endif
        }

#ifdef __LINUX__
        fclose(f);
#else
        ufs_close(&file_handle);
#endif
        mem_cache_del(&cache);

        if (demuxer) {
            free_demuxer(demuxer);
        }

        if (stream) {
            free_stream(stream);    // kill cache thread
        }

        //stream_dump_progress_end();
    }

    // initialized_flags |= INITIALIZED_DEMUXER;
}
#endif

#ifndef USE_MPLAYER_DEMUXER

typedef struct {
    int eof;
    int type;
    u32 pts;
} demux_stream_t;

typedef struct {
    u32 len;
    u32 pts;
} es_file_header;


static ufs_file_t audio_fp;
static ufs_file_t video_fp;
static u8 * demux_tmp_buffer;


/*
 *
 *
 *
 */
static int demux_open()
{
    u8 ret;
    FILE_SEQ_DEBUG("[%s] --------start%d\n", __func__);
    printf("[%s] --------start%d\n", __func__);
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
#ifdef TEST_VIDEO
    ret = ufs_open(&video_fp, p_file_seq->m_path[0],  UFS_READ);

    if (ret == FR_OK) {
        FILE_SEQ_DEBUG("open video es file OK\n", ret);
    } else {
        FILE_SEQ_DEBUG("open video es file FAILED\n", ret);
    }

#endif
#ifdef TEST_AUDIO
    ret = ufs_open(&audio_fp, p_file_seq->m_path[0],  UFS_READ);

    if (ret == FR_OK) {
        FILE_SEQ_DEBUG("open audio es file OK\n", ret);
    } else {
        FILE_SEQ_DEBUG("open audio es file FAILED\n", ret);
    }

#endif
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
    demux_tmp_buffer = file_seq_mem_alloc(p_file_seq, 16 * 1024);
#else
    demux_tmp_buffer = mtos_malloc(16 * 1024);
#endif
    MT_ASSERT(demux_tmp_buffer != NULL);
    memset(demux_tmp_buffer, 0x00, 16 * 1024);
    FILE_SEQ_DEBUG("demux_open success and return\n");
    return 0;
}


/*
 *
 *
 *
 */
static int ds_get_packet(demux_stream_t * ds, unsigned char ** start)
{
    static u32 dbg_pos = 0;
    u32 read_size;
    u32 size;
    u8 header[PACKET_HEADER_LEN];
    ufs_file_t file_handle;
    u32 ret_size;
    u8 ret;

    if (ds->type == 0) { //audio
        file_handle = audio_fp;
    } else if (ds->type == 1) {
        file_handle = video_fp;    //video
    } else {
        FILE_SEQ_DEBUG("error file type");
        MT_ASSERT(0);
    }

    ret = ufs_read(&file_handle, (void *)demux_tmp_buffer, PACKET_HEADER_LEN, &ret_size);

    if ((ret_size != PACKET_HEADER_LEN) || (FR_OK != ret)) {
        FILE_SEQ_DEBUG("!!!!read file end\n");
        ds->eof = 1;
        return 0;
    }

    size = ((es_file_header *)demux_tmp_buffer)->len;
    ds->pts = ((es_file_header *)demux_tmp_buffer)->pts ;
    ret = ufs_read(&file_handle, demux_tmp_buffer, size, &ret_size);

    if ((ret_size != size) || (FR_OK != ret)) {
        FILE_SEQ_DEBUG("!!!!read file end111\n");
        ds->eof = 1;
        return 0;
    }

    *start = demux_tmp_buffer;
    FILE_SEQ_DEBUG("pts=%d, size=%d\n", ds->pts, size);
    return size;
}
#endif

extern FW_MEM_CFG_T  g_pFwMemCfg ;
/*
 *
 *
 *
 */
 extern int play_to_recpos;
int  ts_rec_sec = 0;
int do_user_cmd()
{
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    demuxer_t * p_demuxer = (demuxer_t *)p_file_seq->p_demuxer;
    u32 freesize = 0;
	int ret = 0;
    FP_USER_CMD_T cmd;
    memset(&cmd, 0, sizeof(FP_USER_CMD_T));
    static int seek_tick= 0;
    if (x_pop_user_cmd(p_file_seq, &cmd) == 0) {
        switch (cmd.type) {
            case CMD_PAUSE:
                FILE_SEQ_DEBUG("[%s] ===do pause\n", __func__);
                ClearFlag(p_file_seq->m_user_cmd, CMD_PAUSE);
                aud_pause_vsb(p_file_seq->p_audio_dev);

                if (p_file_seq->only_audio_mode == FALSE) {
                    vdec_pause(p_file_seq->p_vdec_dev);
                }

                p_file_seq->m_play_state = FILE_SEQ_PAUSE;
		ret = 0;
                break;
            case CMD_RESUME:
                FILE_SEQ_DEBUG("[%s] ===do resume!!!!!\n", __func__);
                ClearFlag(p_file_seq->m_user_cmd, CMD_RESUME);

                if (p_file_seq->only_audio_mode == FALSE) {
                    vdec_resume(p_file_seq->p_vdec_dev);
                }

                mtos_task_sleep(30);
                aud_resume_vsb(p_file_seq->p_audio_dev);
                p_file_seq->m_play_state = FILE_SEQ_PLAY;
				ret = 0;
                break;
            case  CMD_PLAY_AT_TIME:
                FILE_SEQ_DEBUG("[%s] ===do play at time!!!!!\n", __func__);
                do_playAtTime();
#if (defined(CONFIG_YINHE_WARRIOS_SDK))
                p_file_seq->m_play_state = FILE_SEQ_PLAY;
#endif
                ret = 1;
                break;
            default:
                break;
        }

        return 0;
    }

    if (IS_SET(p_file_seq->m_user_cmd, CMD_CHANGE_SPEED)) {

	//mtos_printk("\n%s %d  %d\n",__func__,__LINE__,p_file_seq->tmp_speed);
	
        if(p_file_seq->only_audio_mode&&(p_demuxer->type == DEMUXER_TYPE_MPEG_TS))
        {         

			  // p_file_seq->cur_speed = p_file_seq->tmp_speed;
        	//mtos_printk("\n%s %d  %d\n",__func__,__LINE__,p_file_seq->tmp_speed);
if(play_to_recpos ==0&&p_file_seq->tmp_speed!=0&&p_file_seq->tmp_speed!=1)
{
             if((mtos_ticks_get() - seek_tick) >= 100)  //1tick = 10ms, play 1s music
            {
            int seek_time = p_file_seq->apts_upload/1000 + p_file_seq->tmp_speed;
             
           
			 if(seek_time>1)
			 	{
			 	if(fp_is_timeshift_file())
			 		{
			 		if(seek_time<ts_rec_sec)
			 		             p_file_seq->play_at_time(p_file_seq,seek_time);
					else
						{
			 		if (NULL != p_file_seq->event_cb) { 
			p_file_seq->event_cb(FILE_PLAYBACK_SEQ_INVALID, 0); 
			FILE_SEQ_DEBUG("send event playback to head\n");
		}
						 play_to_recpos = 0;
			 p_file_seq->tmp_speed= 0;		
			 	}
			 		}
					else
             p_file_seq->play_at_time(p_file_seq,seek_time);
			 	}
			 else
			 	{
			 		if (NULL != p_file_seq->event_cb) { 
			p_file_seq->event_cb(FILE_PLAYBACK_SEQ_INVALID, 0); 
			FILE_SEQ_DEBUG("send event playback to head\n");
		}
			 p_file_seq->tmp_speed= 0;		
			 	}
       
             seek_tick = mtos_ticks_get();
            }
        	}
		else
			{
					 		if (NULL != p_file_seq->event_cb&&play_to_recpos==1) { 
			p_file_seq->event_cb(FILE_PLAYBACK_SEQ_ES_TASK_DEAD, 0); 
			FILE_SEQ_DEBUG("send event playback to head\n");
		}
			 p_file_seq->tmp_speed= 0;		
			 play_to_recpos = 0;
			}
		        	  if(p_file_seq->tmp_speed==1||p_file_seq->tmp_speed==0)
        	  	{
        	  	 p_file_seq->last_speed = TS_SEQ_NORMAL_PLAY;
           
                //aud_mute_onoff_vsb(p_file_seq->p_audio_dev, FALSE);
                //mute set by up layer
                //send to es open
                //audio start
                p_file_seq->isNormalPlay = TRUE;
                p_file_seq->isTrickPlay = FALSE;
                p_file_seq->isTrickToNormal = TRUE;
                //mtos_task_delay_ms(50);
                p_file_seq->last_speed = p_file_seq->cur_speed;
			  	ClearFlag(p_file_seq->m_user_cmd, CMD_CHANGE_SPEED);
				//aud_stop_vsb(p_file_seq->p_audio_dev);
				 // aud_start_vsb(p_file_seq->p_audio_dev, p_file_seq->m_audio_codec_type, AUDIO_ES_FILE);
        	  	}
             return 0;
        }
		
         if ((p_demuxer->type == DEMUXER_TYPE_MPEG_PS) || (p_demuxer->type == DEMUXER_TYPE_AUDIO) || p_file_seq->only_audio_mode) { // linda zhu add, fix bug 49003, MPEG_PS not support FAST_PLAY and REV_FAST_PLAY
            if (NULL != p_file_seq->event_cb) {
                p_file_seq->event_cb(FILE_PLAYBACK_UNSUPPORT_TRICK, 0);
            }

            ClearFlag(p_file_seq->m_user_cmd, CMD_CHANGE_SPEED);
            FILE_SEQ_DEBUG("[%s] [%d] not support\n", __func__, __LINE__);
            return 0;
        }

        if (p_file_seq->unable_trickplay) {
            if (NULL != p_file_seq->event_cb) {
                p_file_seq->event_cb(FILE_PLAYBACK_UNSUPPORT_TRICK, 0);
            }
        } else {
            FILE_SEQ_DEBUG("[%s] chang speed  !!!!!!\n", __func__);
            p_file_seq->cur_speed = file_seq_parser_speed((void *)p_file_seq,
                                    p_file_seq->tmp_speed);

            if (p_file_seq->cur_speed < 0) {
                FILE_SEQ_DEBUG("[%s] not support %d\n", __func__, p_file_seq->tmp_speed);
               ret = 0;
            }
		     mtos_printk("\n%s %d\n",__func__,__LINE__);
	    if(p_file_seq->last_speed == TS_SEQ_NORMAL_PLAY)
	    {
	    		     mtos_printk("\n%s %d\n",__func__,__LINE__);
		    p_file_seq->isAudMute = file_get_audio_mute_onoff();
					     mtos_printk("\n%s %d %d\n",__func__,__LINE__,p_file_seq->isAudMute);
	    }

            if (p_file_seq->cur_speed != p_file_seq->last_speed) {
                FILE_SEQ_DEBUG("last_speed:%d  cur_speed:%d!!!!\n",
                               p_file_seq->last_speed, p_file_seq->cur_speed);

                //1. if stream ends when fastplay or stream begin when backplay
                //TODO
                //...
                //2. mute when normal or not
                if (p_file_seq->cur_speed == TS_SEQ_NORMAL_PLAY) {
			// p_file_seq->isAudMute = FALSE;
                    //aud_mute_onoff_vsb(p_file_seq->p_audio_dev, FALSE);
                    //mute set by up layer
                    p_file_seq->vpts_upload = 1;
                } else {
			// p_file_seq->isAudMute = TRUE;
                    aud_mute_onoff_vsb(p_file_seq->p_audio_dev, TRUE);
                }

                //3.        2x <---->normal do nothing
                if ((p_file_seq->cur_speed == TS_SEQ_FAST_PLAY_2X &&
                     p_file_seq->last_speed == TS_SEQ_NORMAL_PLAY) ||
                    (p_file_seq->cur_speed == TS_SEQ_NORMAL_PLAY &&
                     p_file_seq->last_speed == TS_SEQ_FAST_PLAY_2X)) {
                    OS_PRINTF("[%s] do nothing \n", __func__);
                } else {
                    vdec_freeze_stop(p_file_seq->p_vdec_dev);
                    vdec_file_clearesbuffer(p_file_seq->p_vdec_dev);
                    vdec_get_es_buf_space(p_file_seq->p_vdec_dev, &freesize);
                    FILE_SEQ_LOG("[%s] 111trick es free: 0x%x\n", __func__, freesize);
                    vdec_set_avsync_mode(p_file_seq->p_vdec_dev, VDEC_AVSYNC_FILEPLAY);
                    vdec_start(p_file_seq->p_vdec_dev, p_file_seq->m_video_codec_type, 2);
                    FILE_SEQ_LOG("[%s] p_file_seq->m_video_codec_type:%d\n", __func__, p_file_seq->m_video_codec_type);
                    vdec_resume(p_file_seq->p_vdec_dev);
                    file_seq_parser_speed((void *)p_file_seq, p_file_seq->tmp_speed);

                    if (p_file_seq->cur_speed == TS_SEQ_NORMAL_PLAY ||
                        p_file_seq->cur_speed == TS_SEQ_FAST_PLAY_2X) {
                        //send to es open
                        //audio start
                        p_file_seq->isNormalPlay = TRUE;
                        p_file_seq->isTrickPlay = FALSE;
                        p_file_seq->isTrickToNormal = TRUE;

                        if (p_file_seq->m_audio_codec_type != 0xffff) {
                            u32 size = 0;
                            u32 align = 0;
                            aud_get_buf_requirement_vsb(p_file_seq->p_audio_dev, &size, &align);
                            aud_set_buf_vsb(p_file_seq->p_audio_dev,
                                            (g_pFwMemCfg.p_audio_start) | 0xa0000000,
                                            size);

                            set_audio_param_to_vsb();
/*                            
                            if (p_file_seq->m_audio_codec_type == 0) {
                                aud_pcm_param_vsb_t audio_param;
                                int is_big_endian = 0;
                                int bits = 8;
                                int channels = 1;
                                int sample_rate = 48000;

                                if (p_file_seq->is_ts) {
                                    ts_get_pcm_info(p_file_seq->p_demuxer, &is_big_endian, &bits, &channels, &sample_rate);
                                    audio_param.is_big_endian = is_big_endian;
                                    audio_param.is_stereo = channels >= 2;
                                    audio_param.bits = bits;
                                } else {
                                       else
                                       {
                                    //set default
                                    audio_param.is_big_endian = 0;
                                    audio_param.is_stereo = 1;
                                    audio_param.bits = 16;
                                }

                                if (sample_rate == 48000) {
                                    audio_param.sample = AUDIO_SAMPLE_48;
                                } else if (sample_rate == 32000) {
                                    audio_param.sample = AUDIO_SAMPLE_32;
                                } else if (sample_rate == 44100) {
                                    audio_param.sample = AUDIO_SAMPLE_44;
                                } else if (sample_rate == 16000) {
                                    audio_param.sample = AUDIO_SAMPLE_16;
                                } else if (sample_rate == 24000) {
                                    audio_param.sample = AUDIO_SAMPLE_24;
                                } else if (sample_rate == 22050) {
                                    audio_param.sample = AUDIO_SAMPLE_22;
                                }

                                FILE_SEQ_DEBUG("pcm_header: %d channels, %d bits per sample, %d Hz\n", channels, bits, sample_rate);
                                aud_start_vsb(p_file_seq->p_audio_dev, AUDIO_PCM, AUDIO_PCM_FILE);
                                aud_set_pcm_param_vsb(p_file_seq->p_audio_dev, &audio_param);
                            } else {
                                aud_start_vsb(p_file_seq->p_audio_dev, p_file_seq->m_audio_codec_type, AUDIO_ES_FILE);
                            }
*/                            
                        }
                    } else if (p_file_seq->last_speed == TS_SEQ_FAST_PLAY_2X ||
                               p_file_seq->last_speed == TS_SEQ_NORMAL_PLAY) {
                        //stop es send
                        //audio stop
                        p_file_seq->isNormalPlay = FALSE;
                        p_file_seq->isTrickPlay = TRUE;
                        p_file_seq->isTrickToNormal = FALSE;

                        if (p_file_seq->m_audio_codec_type != 0xffff) {
                            //printf("!!!!!!!!!stop audio\n");
                            aud_stop_vsb(p_file_seq->p_audio_dev);
                        }

                        if (p_file_seq->last_speed == 0) {
                            p_file_seq->last_v_average_bps = p_file_seq->video_average_bps;
                        }
                    }
                }
		    if((p_file_seq->cur_speed == TS_SEQ_NORMAL_PLAY) && (p_file_seq->isAudMute==0))
		    {
			    aud_mute_onoff_vsb(p_file_seq->p_audio_dev, FALSE);
		    }

                //mtos_task_delay_ms(50);
                p_file_seq->last_speed = p_file_seq->cur_speed;
                p_file_seq->needNewAudioData = TRUE;
                p_file_seq->needNewVideoData = TRUE;
                //p_file_seq->m_tmp_ves_buf_pos = 0;
                p_file_seq->isAudioBufferFull = FALSE;
                p_file_seq->isVideoBufferFull = FALSE;
                SetFlag(p_file_seq->internal_event, CHECK_TRICKPLAY);
            } else {
                FILE_SEQ_DEBUG("[%s] do nothing  !!!!!!\n", __func__);
            }
        }

        ClearFlag(p_file_seq->m_user_cmd, CMD_CHANGE_SPEED);
        ret = 0;
    }

    if (IS_SET(p_file_seq->m_user_cmd, CMD_CHANGE_AUDIO_TRACK)) {
        if (!p_file_seq->needNewVideoData) {
            ret = 0;
        }

        FILE_SEQ_DEBUG("[%s] ===do change audio track!!!!!\n", __func__);
#if 0
#else
        //new shceme
        unsigned int vpts = 0 ;
        unsigned int apts = 0 ;
        unsigned int cur_play_pts = 0;
        unsigned int get_packet_pts = 0;
        unsigned int get_packet_pts2 = 0;
        unsigned int get_packet_pts3 = 0;
        unsigned int get_packet_pts_pre = 0;
        unsigned int es_buf_size = 0;
        unsigned int video_free_space_size = 0;
        demux_stream_t * ds_a = NULL;
        demux_stream_t * ds_v = NULL;
        demux_stream_t * ds_s = NULL;
        ds_a = p_file_seq->p_cur_ds_audio;
        ds_v = p_file_seq->p_cur_ds_video;
        ds_s = p_file_seq->p_cur_ds_sub;
        int cur_audio_packet_size = 0;
        int cur_video_packet_size = 0;
        float orig_apts_sv;
        float ts_step = 0.1;
        int seek_times = 0;
        int ts_apid = 0;
        unsigned int t1 = mtos_ticks_get();
        unsigned int t2 = 0 ;
#ifdef FILE_PLAYBACK_DEBUG
        //flag_debug = 1;
        int     ticks_in, ticks_out;
        //ticks_in= mtos_ticks_get();
        //FILE_SEQ_DEBUG("[%s] ---------------- ticks_in = %d\n",__func__,ticks_in);
#endif

        //get current pts
        while (!vpts && !apts) {
#if 0
#else
            vdec_info_t vstate;
            vdec_get_info(p_file_seq->p_vdec_dev, &vstate);
            vpts = vstate.pts;

            if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
                vpts = vpts & (0x7fffffff);
            }

#endif

            if (vpts != 0) {
                cur_play_pts = vpts / TIME_BASE;
                break;
            } else { //if there is no data in es buf for a long time, H264DecInit int will set pts_in_slot[loop] = 0; then use p_file_seq->orig_vpts instead.
                vdec_get_es_buf_space(p_file_seq->p_vdec_dev, (u32 *)&video_free_space_size);
                vdec_get_es_buf_size(p_file_seq->p_vdec_dev, (u32 *)&es_buf_size);

                if (video_free_space_size == es_buf_size) {
                    cur_play_pts = p_file_seq->orig_vpts;
                    break;
                }
            }
            t2 = mtos_ticks_get();
            if(ds_a->eof ||ds_v->eof || (t2 > t1 && t2 - t1 > 150) || (t2 < t1 && t1 - t2 > 150))
            {
                cur_play_pts = p_file_seq->orig_vpts;
                ClearFlag(p_file_seq->m_user_cmd, CMD_CHANGE_AUDIO_TRACK);
                ret = 0;
                if(ds_a->eof)
                    ds_a->eof = 0;
                return ret;

            }

            //OS_PRINTF("[%s] ------ get the non-zero vpts&apts, vpts = %ld  ,  apts = %ld \n",__func__,vpts,apts);
        }

        //else if(apts!=0)
        //  cur_play_pts = apts/TIME_BASE;
        //FILE_SEQ_DEBUG("[%s] ------ cur_play_pts = %d ms,  vpts = %ld  ,  apts = %ld \n",__func__,cur_play_pts,vpts,apts);
        //change audio track
        //FILE_SEQ_DEBUG("[%s] ------- change audio track, call demuxer_switch_audio()\n",__func__);
        if (p_file_seq->is_ts) {
            ts_apid = ts_get_audio_track_pid(p_file_seq->ts_priv, p_file_seq->audio_track_id);
            demuxer_switch_audio(p_file_seq->p_demuxer, ts_apid);
        } else {
            demuxer_switch_audio(p_file_seq->p_demuxer, p_file_seq->audio_track_id);
        }

        if (p_demuxer->seekable) {
            //seek back until find the audio packet of current pts
            orig_apts_sv = p_file_seq->orig_apts;
            FILE_SEQ_DEBUG("[%s] ------ cur_play_pts = %d ms, orig_apts = %d ms, orig_vpts = %d ms\n", \
                           __func__, cur_play_pts, (int)(p_file_seq->orig_apts), (int)(p_file_seq->orig_vpts));
            p_file_seq->seek_seconds = (cur_play_pts - orig_apts_sv) / 1000.0;//-33;

            if (p_file_seq->is_ts) {
                p_file_seq->seek_seconds -= ts_step;
            }

            do {
                //seek back
                seek_times++;
                //seek back
                demux_seek(p_file_seq->p_demuxer, p_file_seq->seek_seconds, 0, 0) ;
                FILE_SEQ_DEBUG("[%s] --------seek back %d ms\n", __func__, (int)(p_file_seq->seek_seconds * 1000));
                //get audio packet
                p_file_seq->extra_audio_size = 0;
                p_file_seq->p_extra_aud_buf = NULL;
                cur_audio_packet_size = ds_get_packet_audio(ds_a, &(p_file_seq->p_a_pkt_start), &(p_file_seq->p_extra_aud_buf), (uint8_t **)(&(p_file_seq->extra_audio_size)));

                if (cur_audio_packet_size <= 0 || p_file_seq->p_a_pkt_start == NULL) {
                    FILE_SEQ_ERROR("[%s][ERROR] audio packet size [%d]!!!!\n", __func__, cur_audio_packet_size);
                    break;
                }

                get_packet_pts = ds_a->pts * 1000;//ms
                p_file_seq->left_a_pkt_bytes = cur_audio_packet_size;
                FILE_SEQ_DEBUG("[%s] ------ get_packet_pts = %d ms,cur_audio_packet_size=%d\n", __func__, get_packet_pts, cur_audio_packet_size);

                //check pts
                if (get_packet_pts <= cur_play_pts || get_packet_pts == get_packet_pts_pre) {
                    p_file_seq->seek_seconds = 0;
                    OS_PRINTF("[%s] --------seek back ok!  get_packet_pts = %ld, cur_play_pts = %d\n", __func__, get_packet_pts, cur_play_pts);
                    break;
                }

                if (p_file_seq->is_ts && seek_times > 3) {
                    break;
                }

                p_file_seq->seek_seconds = (cur_play_pts * 1.0 - get_packet_pts * 1.0) / 1000.0;//-1;

                if (p_file_seq->is_ts) {
                    p_file_seq->seek_seconds -= seek_times * ts_step;
                }

                get_packet_pts_pre = get_packet_pts;
            } while (1);

            //get the right audio packet, and discard the non-necessory video packets
            while (get_packet_pts < cur_play_pts) {
                p_file_seq->extra_audio_size = 0;
                p_file_seq->p_extra_aud_buf = NULL;
                cur_audio_packet_size = ds_get_packet_audio(ds_a, &(p_file_seq->p_a_pkt_start), &(p_file_seq->p_extra_aud_buf), (uint8_t **)(&(p_file_seq->extra_audio_size)));

                if (cur_audio_packet_size <= 0 || p_file_seq->p_a_pkt_start == NULL) {
                    FILE_SEQ_ERROR("[%s][ERROR] audio packet size [%d]!!!!\n", __func__, cur_audio_packet_size);
                    break;
                }

                get_packet_pts = ds_a->pts * 1000;//ms
                p_file_seq->left_a_pkt_bytes = cur_audio_packet_size;
                //FILE_SEQ_DEBUG("[%s] ------ to get the right audio packet    get_packet_pts = %d ms,cur_audio_packet_size=%d\n",__func__,get_packet_pts,cur_audio_packet_size);
#if 0

                if (ds_v->pts < ds_a->pts) {
                    cur_video_packet_size = ds_get_packet_video(ds_v, &(p_file_seq->p_v_pkt_start), p_file_seq->cur_speed);
                    get_packet_pts2 = ds_v->pts * 1000;//ms
                    //OS_PRINTF("[%s] ===== discard video packet     get_packet_pts2 = %d ms,cur_video_packet_size=%d\r",__func__,get_packet_pts2,cur_video_packet_size);
                }

#endif
#if 0

                if (ds_s->pts < ds_a->pts) {
                    cur_video_packet_size = ds_get_packet_subt(ds_s, &(p_file_seq->p_sub_pkt_start), &(p_file_seq->orig_spts));
                    get_packet_pts3 = ds_s->pts * 1000;//ms
                    //OS_PRINTF("[%s] ===== discard sub packet     get_packet_pts2 = %d ms,cur_video_packet_size=%d\r",__func__,get_packet_pts2,cur_video_packet_size);
                }

#endif
            }

            //FILE_SEQ_DEBUG("[%s] ------ get the right audio packet!    packet pts[a:%d,v:%d,s:%d] ms \n",__func__,get_packet_pts,get_packet_pts2,get_packet_pts3);

            //clear the audio es buffer
            if (p_file_seq->m_audio_codec_type != 0xffff) {
                //vdec_set_avsync(p_file_seq->p_vdec_dev, 0);
                aud_stop_vsb(p_file_seq->p_audio_dev);

                //vdec_set_avsync(p_file_seq->p_vdec_dev, 1);

                set_audio_param_to_vsb();
/*
                if (p_file_seq->m_audio_codec_type == 0) {
                    aud_pcm_param_vsb_t audio_param;
                    int is_big_endian = 0;
                    int bits = 8;
                    int channels = 1;
                    int sample_rate = 48000;

                    if (p_file_seq->is_ts) {
                        ts_get_pcm_info(p_file_seq->p_demuxer, &is_big_endian, &bits, &channels, &sample_rate);
                        audio_param.is_big_endian = is_big_endian;
                        audio_param.is_stereo = channels >= 2;
                        audio_param.bits = bits;
                    } else {
                           else
                           {
                        //set default
                        audio_param.is_big_endian = 0;
                        audio_param.is_stereo = 1;
                        audio_param.bits = 16;
                    }

                    if (sample_rate == 48000) {
                        audio_param.sample = AUDIO_SAMPLE_48;
                    } else if (sample_rate == 32000) {
                        audio_param.sample = AUDIO_SAMPLE_32;
                    } else if (sample_rate == 44100) {
                        audio_param.sample = AUDIO_SAMPLE_44;
                    } else if (sample_rate == 16000) {
                        audio_param.sample = AUDIO_SAMPLE_16;
                    } else if (sample_rate == 24000) {
                        audio_param.sample = AUDIO_SAMPLE_24;
                    } else if (sample_rate == 22050) {
                        audio_param.sample = AUDIO_SAMPLE_22;
                    }

                    FILE_SEQ_DEBUG("pcm_header: %d channels, %d bits per sample, %d Hz\n", channels, bits, sample_rate);
                    aud_start_vsb(p_file_seq->p_audio_dev, AUDIO_PCM, AUDIO_PCM_FILE);
                    aud_set_pcm_param_vsb(p_file_seq->p_audio_dev, &audio_param);
                } else {
                    aud_start_vsb(p_file_seq->p_audio_dev, p_file_seq->m_audio_codec_type, AUDIO_ES_FILE);
                }
*/
                //aud_start_vsb(p_file_seq->p_audio_dev, p_file_seq->m_audio_codec_type, AUDIO_ES_FILE);
                //FILE_SEQ_DEBUG("[%s] ------- restart audio decoder !\n",__func__);
            }

            if (p_file_seq->m_play_state == FILE_SEQ_PAUSE) {
                aud_pause_vsb(p_file_seq->p_audio_dev);
            }

            //push the packets into audio es buffer , and discard the non-necessory video packets
            unsigned int audFreeSpaceSize = 0;
            int puch_packet_cnt = 0;

            do {
                if (ds_a->pts < 0.0) {
                    ds_a->pts = 0.0;
                }

                p_file_seq->orig_apts =  ds_a->pts * 1000.0;
                p_file_seq->sys_apts = (u32)((p_file_seq->orig_apts * PTS_BASE)) | VALID_PTS_MASK;
#ifdef ES_END_IN_TASK

                if (p_file_seq->init_buffering_finish != 1) {
                    aud_file_getleftes_vitual_buffer(p_file_seq, (u32 *)&audFreeSpaceSize);
                } else
#endif
                    aud_file_getleftesbuffer_vsb(p_file_seq->p_audio_dev, (u32 *)&audFreeSpaceSize);

                if (audFreeSpaceSize >= (p_file_seq->left_a_pkt_bytes + p_file_seq->extra_audio_size) + AUDIO_ES_BUF_OVERFLOW_THRESHOLD) {
                    if (p_file_seq->p_extra_aud_buf && p_file_seq->extra_audio_size) {
                        memcpy(p_file_seq->p_aud_assemble_buf, p_file_seq->p_extra_aud_buf, p_file_seq->extra_audio_size);
                        free(p_file_seq->p_extra_aud_buf);
                        p_file_seq->p_extra_aud_buf = NULL;
                    }

                    if ((void *)(p_file_seq->p_a_pkt_start) == NULL) {
                        return -1;
                    }

                    memcpy(p_file_seq->p_aud_assemble_buf + p_file_seq->extra_audio_size, (void *)(p_file_seq->p_a_pkt_start), p_file_seq->left_a_pkt_bytes);
                    adec_push_audio_es(p_file_seq->p_aud_assemble_buf, p_file_seq->left_a_pkt_bytes + p_file_seq->extra_audio_size, p_file_seq->sys_apts);
                    p_file_seq->totalAesNum += p_file_seq->left_a_pkt_bytes;
                    p_file_seq->available_aes_bytes = (p_file_seq->dec_cap.max_aes_num - audFreeSpaceSize)
                                                      + (p_file_seq->left_a_pkt_bytes + p_file_seq->extra_audio_size);
                    p_file_seq->left_a_pkt_bytes = 0;
                    p_file_seq->isAudioBufferFull = FALSE;
                    p_file_seq->needNewAudioData = TRUE;
                    puch_packet_cnt++;
                } else {
                    FILE_SEQ_DEBUG("\n[%s] ------- audio buffer is nearly full( audFreeSpaceSize = %d ), break !puch_packet_cnt=%d\n", __func__, audFreeSpaceSize, puch_packet_cnt);
                    break;
                }

                if (get_packet_pts >= orig_apts_sv) {
                    FILE_SEQ_DEBUG("\n[%s] ------ pushed the last packet( apts=orig_apts )to aes buffer!! break!  get_packet_pts = %d ms,orig_apts=%d, puch_packet_cnt=%d\n", \
                                   __func__, get_packet_pts, (int)(orig_apts_sv), puch_packet_cnt);
                    break;
                }

                p_file_seq->extra_audio_size = 0;
                p_file_seq->p_extra_aud_buf = NULL;
                cur_audio_packet_size = ds_get_packet_audio(ds_a, &(p_file_seq->p_a_pkt_start), &(p_file_seq->p_extra_aud_buf), (uint8_t **) & (p_file_seq->extra_audio_size));

                if (cur_audio_packet_size <= 0 || p_file_seq->p_a_pkt_start == NULL) {
                    FILE_SEQ_ERROR("[%s][ERROR] audio packet size [%d]!!!!\n", __func__, cur_audio_packet_size);
                    FILE_SEQ_DEBUG("[%s] ------ pushed %d audio packets:  cur_audio_packet_size=%d, p_file_seq->p_a_pkt_start=%x\n", \
                                   __func__, puch_packet_cnt, cur_audio_packet_size, p_file_seq->p_a_pkt_start);
                    FILE_SEQ_DEBUG("[%s] ------ ds_a->packs=%d, ds_a->bytes=%d\n", __func__, ds_a->packs, ds_a->bytes);
                    break;
                }

                get_packet_pts = ds_a->pts * 1000;//ms
                p_file_seq->left_a_pkt_bytes = cur_audio_packet_size;
                //OS_PRINTF("[%s] ------ get_packet_pts = %d ms,cur_audio_packet_size=%d,ds_a->packs=%d, ds_a->bytes=%d\n",__func__,get_packet_pts,cur_audio_packet_size,ds_a->packs, ds_a->bytes);
#if 0

                if (ds_v->pts < ds_a->pts && ds_v->pts < (p_file_seq->orig_vpts / 1000)) {
                    //OS_PRINTF("\n[%s] ------ before call ds_get_packet_video: ds_v->packs=%d, ds_v->bytes=%d\n",__func__,ds_v->packs, ds_v->bytes);
                    cur_video_packet_size = ds_get_packet_video(ds_v, &(p_file_seq->p_v_pkt_start), p_file_seq->cur_speed);
                    get_packet_pts2 = ds_v->pts * 1000;//ms
                }

#endif
            } while (1);

            int packet_count = 0;

            //discard the video packet until find the video packt of orig_vpts
            do {
                if (get_packet_pts2 >= (unsigned int)p_file_seq->orig_vpts) {
                    for (packet_count = 0; packet_count < 10; packet_count++) {
                        if (get_packet_pts2 == (unsigned int)p_file_seq->orig_vpts) {
                            break;
                        }

                        cur_video_packet_size = ds_get_packet_video(ds_v, &(p_file_seq->p_v_pkt_start), p_file_seq->cur_speed);

                        if (cur_video_packet_size <= 0 || p_file_seq->p_v_pkt_start == NULL) {
                            FILE_SEQ_ERROR("[%s][ERROR] video packet size [%d]!!!!\n", __func__, cur_video_packet_size);
                            break;
                        }

                        get_packet_pts2 = ds_v->pts * 1000;//ms
                    }

                    FILE_SEQ_DEBUG("\n%s %d------ find the last packet that pushed to ves buffer!! break!  get_packet_pts = %d ms,orig_vpts=%d\n", \
                                   __func__, __LINE__, get_packet_pts2, (int)(p_file_seq->orig_vpts));
                    break;
                }

                cur_video_packet_size = ds_get_packet_video(ds_v, &(p_file_seq->p_v_pkt_start), p_file_seq->cur_speed);

                if (cur_video_packet_size <= 0 || p_file_seq->p_v_pkt_start == NULL) {
                    FILE_SEQ_ERROR("[%s][ERROR] video packet size [%d]!!!!\n", __func__, cur_video_packet_size);
                    break;
                }

                get_packet_pts2 = ds_v->pts * 1000;//ms
                //OS_PRINTF("[%s] ===== get_packet_pts2 = %d ms,cur_video_packet_size=%d\r",__func__,get_packet_pts2,cur_video_packet_size);
            } while (1);

#endif
        p_file_seq->needNewAudioData = TRUE;
        p_file_seq->needNewVideoData = TRUE;
        FILE_SEQ_DEBUG("[%s] ------ change audio track ok, return! [packs,bytes][aud: %d,%d ] [video: %d, %d],  [orig_pts][a:%d,v:%d]\n", \
                       __func__, ds_a->packs, ds_a->bytes, ds_v->packs, ds_v->bytes, (int)(p_file_seq->orig_apts), (int)(p_file_seq->orig_vpts));
#ifdef FILE_PLAYBACK_DEBUG
        //ticks_out= mtos_ticks_get();
        //FILE_SEQ_DEBUG("[%s] ---------------- ticks_out = %d, ticks delta = %d\n",__func__,ticks_out,ticks_out-ticks_in);
#endif
    }

    ClearFlag(p_file_seq->m_user_cmd, CMD_CHANGE_AUDIO_TRACK);
    ret = 0;
}

    if (IS_SET(p_file_seq->m_user_cmd, CMD_CHANGE_SUBT_ID)) {

        FILE_SEQ_DEBUG("[%s] ===do change sub tittle id!!!!!\n", __func__);
#if 1
        unsigned int vpts = 0 ;
        unsigned int cur_play_pts = 0;
        unsigned int get_packet_pts = 0;
        unsigned int es_buf_size = 0;
        unsigned int video_free_space_size = 0;
        demux_stream_t * ds_a = NULL;
        demux_stream_t * ds_v = NULL;
        demux_stream_t * ds_s = NULL;
        ds_a = p_file_seq->p_cur_ds_audio;
        ds_v = p_file_seq->p_cur_ds_video;
        ds_s = p_file_seq->p_cur_ds_sub;
        int cur_audio_packet_size = 0;
        int cur_video_packet_size = 0;
        int cur_sub_packet_size = 0;
        int orig_vpts_sv;
        int orig_apts_sv;
        int orig_spts_sv;
        int seek_times = 0;

	 if (!(p_demuxer->seekable)){
		 FILE_SEQ_DEBUG("[%s] === do nothing! seekable[%d]\n", __func__,p_demuxer->seekable);
		 ClearFlag(p_file_seq->m_user_cmd, CMD_CHANGE_SUBT_ID);
    		 ret = 0;
	 }else{
	 
        //get current play pts
	 //while (!vpts) {

            vdec_info_t vstate;
            vdec_get_info(p_file_seq->p_vdec_dev, &vstate);
            vpts = vstate.pts;

            if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
                vpts = vpts & (0x7fffffff);
            }


            if (vpts == 0) {
               FILE_SEQ_DEBUG("[%s] === do nothing! vpts[%d]\n", __func__,vpts);
		 ClearFlag(p_file_seq->m_user_cmd, CMD_CHANGE_SUBT_ID);
    		 ret = 0;
            }
            else{
	      cur_play_pts = vpts / TIME_BASE;
           // FILE_SEQ_DEBUG("[%s] ------ get the non-zero vpts=%d \n",__func__,vpts);
        //}

        //seek back to the point of playing pts
        orig_vpts_sv = p_file_seq->orig_vpts;
        orig_apts_sv = p_file_seq->orig_apts;
        orig_spts_sv = p_file_seq->orig_spts;
        FILE_SEQ_DEBUG("[%s] ------ cur_play_pts = %d ms, orig_apts = %d ms, orig_vpts = %d ms, orig_spts = %d ms\n", __func__, cur_play_pts, orig_apts_sv, orig_vpts_sv, orig_spts_sv);

	 if(p_file_seq->orig_spts == 0){
		orig_spts_sv = p_file_seq->orig_vpts;
		FILE_SEQ_DEBUG("[%s] orig_spts == 0, set orig_spts_sv to orig_vpts: %d\n", __func__,orig_spts_sv);
	 }
	 
        p_file_seq->seek_seconds = ((float)cur_play_pts - orig_spts_sv) / 1000.0;//-33;
        if(p_file_seq->seek_seconds < 0){
        demux_seek(p_file_seq->p_demuxer, p_file_seq->seek_seconds, 0, 0) ;
        FILE_SEQ_DEBUG("[%s] --------seek back %d ms\n", __func__, (int)(p_file_seq->seek_seconds * 1000));

        //discard the video packets before the orig pts
        while(get_packet_pts < orig_vpts_sv){
		 cur_video_packet_size = ds_get_packet_video(ds_v, &(p_file_seq->p_v_pkt_start), p_file_seq->cur_speed);
		 get_packet_pts = ds_v->pts * 1000;//ms
		 if(get_packet_pts < 0)
		  	get_packet_pts = 0;
		 //FILE_SEQ_ERROR("[%s] --pts[%d] size[%d] V\n", __func__, get_packet_pts, cur_video_packet_size);
		 
               if (cur_video_packet_size <= 0 || p_file_seq->p_v_pkt_start == NULL) {
                    FILE_SEQ_ERROR("[%s][ERROR] video packet size [%d]!!!!\n", __func__, cur_video_packet_size);
                    break;
                }
                
        }
        FILE_SEQ_DEBUG("[%s] ------found the v pkt[%d] of orig_vpts_sv[ %d] !\n", __func__, get_packet_pts, orig_vpts_sv);

	//discard the audio packets before the orig pts
	get_packet_pts = 0;
        while(get_packet_pts < orig_apts_sv){
		  p_file_seq->extra_audio_size = 0;
                p_file_seq->p_extra_aud_buf = NULL;
                cur_audio_packet_size = ds_get_packet_audio(ds_a, &(p_file_seq->p_a_pkt_start), &(p_file_seq->p_extra_aud_buf), (uint8_t **)(&(p_file_seq->extra_audio_size)));
		  get_packet_pts = ds_a->pts * 1000;//ms
		  if(get_packet_pts < 0)
		  	get_packet_pts = 0;
		  //FILE_SEQ_ERROR("[%s] --pts[%d] size[%d] A\n", __func__, get_packet_pts, cur_audio_packet_size);
		  
                if (cur_audio_packet_size <= 0 || p_file_seq->p_a_pkt_start == NULL) {
                    FILE_SEQ_ERROR("[%s][ERROR] audio packet size [%d]!!!!\n", __func__, cur_audio_packet_size);
                    break;
                }

                if(p_file_seq->p_extra_aud_buf && p_file_seq->extra_audio_size){
                	//free(p_file_seq->p_extra_aud_buf);
                	p_file_seq->p_extra_aud_buf = NULL;
            	}
                	
        }
	 FILE_SEQ_DEBUG("[%s] ------found the a pkt[%d] of orig_apts_sv[ %d] !\n", __func__, get_packet_pts, orig_apts_sv);

        //discard the subt packet before the playing pts point
        get_packet_pts = 0;
        while(get_packet_pts < cur_play_pts){
		ds_s->id = p_file_seq->subt_id;
        	cur_sub_packet_size = ds_get_packet_subt(ds_s, &(p_file_seq->p_sub_pkt_start), &(p_file_seq->orig_spts));
        	get_packet_pts = ds_s->pts * 1000;//ms
        	FILE_SEQ_DEBUG("[%s] --pts[%d] size[%d] S\n", __func__, get_packet_pts, cur_sub_packet_size);
        	if(get_packet_pts < 0)
		  	get_packet_pts = 0;

	  	if( get_packet_pts >= orig_spts_sv ){
	  		FILE_SEQ_DEBUG("[%s] ------found the subt pkt[%d] of orig_spts_sv[ %d] !\n", __func__, get_packet_pts, orig_spts_sv);
	  		break;
  		}

  		if (cur_sub_packet_size <= 0 || p_file_seq->p_sub_pkt_start == NULL) {
                    FILE_SEQ_ERROR("[%s][ERROR] subt packet size [%d]!!!!\n", __func__, cur_sub_packet_size);
                    //flag_debug = 1;
                    break;
                }
        }
	FILE_SEQ_DEBUG("[%s] ------found the subt pkt[%d] >= cur_play_pts[ %d] !\n", __func__, get_packet_pts, cur_play_pts);
	if(cur_sub_packet_size){
		((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->change_subt_got_pkt = 1;
	  	((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->cur_sub_packet_size = cur_sub_packet_size;
	  	FILE_SEQ_DEBUG("[%s] ------set change_subt_got_pkt =1,  cur_sub_packet_size[ %d] !\n", __func__, cur_sub_packet_size);

	}

       //clear sub fifo
       if (((demuxer_t *)(p_file_seq->p_cur_demuxer))->subt_info.cnt > 0) {
       	     FILE_SEQ_DEBUG("[%s] clear sub fifo when change subt id\n", __func__);
        	     clear_sub_fifo_kw(p_file_seq->p_sub_fifo_handle);    //clear fifo when do seek
	}

	p_file_seq->orig_spts = get_packet_pts;

}
}
}
#endif

     FILE_SEQ_DEBUG("[%s] ===do change sub tittle id end!!!!!\n", __func__);
    ClearFlag(p_file_seq->m_user_cmd, CMD_CHANGE_SUBT_ID);
    ret = 0;
}
 

return ret;
}




/*
 *
 *
 *
 */
//extern    int64_t m3u8_real_play_pos_t;
//extern int m3u8_pre_seq_no;
static void reset_file_seq_param(FILE_SEQ_T * pHandle)
{
    FILE_SEQ_T * p_file_seq =  pHandle ;
    ((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->is_timeshift = 0;
    p_file_seq->isAudioEsEnd = FALSE;
    p_file_seq->isVideoEsEnd = FALSE;
    p_file_seq->isAudioBufferFull = FALSE;
    p_file_seq->isVideoBufferFull = FALSE;
    p_file_seq->exsubtitle = 0;
    p_file_seq->totalVesNum = 0;
    p_file_seq->totalAesNum = 0;
    p_file_seq->p_ds_audio = NULL;
    p_file_seq->p_ds_video = NULL;
    p_file_seq->p_ds_sub = NULL;
    p_file_seq->audio_average_bps = 0;
    p_file_seq->video_average_bps = 0;
    p_file_seq->max_audio_pts = 0;
    p_file_seq->max_video_pts = 0;
    p_file_seq->available_aes_bytes = 0;
    p_file_seq->available_ves_bytes = 0;
    p_file_seq->video_bps = 0;
    p_file_seq->audio_bps = 0;
    p_file_seq->cur_speed = 0;
    p_file_seq->last_speed = 0;
    p_file_seq->tmp_speed = 0;
    p_file_seq->isTrickPlay = FALSE;
    p_file_seq->isNormalPlay = TRUE;
    p_file_seq->m_user_cmd = 0;
    p_file_seq->internal_event = 0;
    p_file_seq->m_tmp_ves_buf_pos = 0;
    p_file_seq->m_aes_left = 0;
    //p_file_seq->p_tmp_ves_buf = NULL;
    //p_file_seq->p_tmp_aes_buf = NULL;
    p_file_seq->checkAvTypeOK = FALSE;
    p_file_seq->checkDefinitionOK = FALSE;
    p_file_seq->is_av_codec_support = 0;
    p_file_seq->loadMedaiOK = FALSE;
    p_file_seq->isTrickToNormal = FALSE;
    p_file_seq->ref_first_audio_pts = 0;
    p_file_seq->ref_first_video_pts = 0;
    p_file_seq->isAudMute = FALSE;
    p_file_seq->is_ts = 0;
    p_file_seq->is_play_at_time = 0;
    p_file_seq->audio_track_id = 0;
    p_file_seq->orig_apts = 0;
    p_file_seq->first_vpts = 0;
    p_file_seq->orig_vpts = 0;
    p_file_seq->orig_spts = 0;
    p_file_seq->sys_apts = 0;
    p_file_seq->sys_vpts = 0;
    p_file_seq->vpts_upload = 0;
    p_file_seq->apts_upload = 0;
    //    p_file_seq->audio_output_mode = 0;
    p_file_seq->only_audio_mode = FALSE;
    p_file_seq->p_v_pkt_start = 0;
    p_file_seq->p_a_pkt_start = 0;
    p_file_seq->p_sub_pkt_start = 0;
    p_file_seq->left_v_pkt_bytes = 0;
    p_file_seq->left_a_pkt_bytes = 0;
    p_file_seq->p_extra_aud_buf = 0;
    p_file_seq->extra_audio_size = 0;
    //p_file_seq->max_ves_size = 0;
    p_file_seq->is_stable = FALSE;
    p_file_seq->camera_rtsp_record = 0;
#ifdef USE_PB_FIFO	    
    p_file_seq->is_fifo_playback = FALSE;
    p_file_seq->pbfifo_buffering_ok = FALSE;
    p_file_seq->pbfifo_http_need_reconnect = FALSE;
    p_file_seq->pbfifo_connect_off = 0;
    p_file_seq->pbfifo_stream_pos = 0;
    p_file_seq->pbfifo_http_state = HTTP_INT;
    p_file_seq->pbfifo_content_len = 0;    
    p_file_seq->hls_parser.enable = 0;
    memset(p_file_seq->hls_parser.m3u8_ur.url,0,4096);
#endif
    if (p_file_seq->p_buffering) {
        memset(p_file_seq->p_buffering, 0, sizeof(BUFFERING_PARAM_T));
    }

    if (p_file_seq->total_path > 1) {
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        file_seq_mem_free(p_file_seq, p_file_seq->p_cur_demuxer);
        file_seq_mem_free(p_file_seq, p_file_seq->p_cur_stream) ;
        file_seq_mem_free(p_file_seq, p_file_seq->p_cur_ds_audio) ;
        file_seq_mem_free(p_file_seq, p_file_seq->p_cur_ds_video) ;
        file_seq_mem_free(p_file_seq, p_file_seq->p_cur_ds_sub) ;
#else
        mtos_free(p_file_seq->p_cur_demuxer);
        mtos_free(p_file_seq->p_cur_stream) ;
        mtos_free(p_file_seq->p_cur_ds_audio) ;
        mtos_free(p_file_seq->p_cur_ds_video) ;
        mtos_free(p_file_seq->p_cur_ds_sub) ;
#endif

        if (p_file_seq->p_m_duration) {
            mtos_free((void *)p_file_seq->p_m_duration);
            p_file_seq->p_m_duration = NULL;
        }

        p_file_seq->p_cur_demuxer = NULL;
        p_file_seq->p_cur_stream = NULL;
        p_file_seq->p_cur_ds_audio = NULL;
        p_file_seq->p_cur_ds_video = NULL;
        p_file_seq->p_cur_ds_sub = NULL;
        p_file_seq->is_preload_task_alive = FALSE;
        p_file_seq->max_duration = 0;
        p_file_seq->m_preload_state = FILE_SEQ_STOP;
    }

    if (p_file_seq->audio_track_num > 0 && p_file_seq->audio_lang_array != NULL) {
        int i;
        TRACK_LANG * p_audio_lang;

        for (i = 0; i < p_file_seq->audio_track_num; i++) {
            p_audio_lang = p_file_seq->audio_lang_array + i;

            if (p_audio_lang->lang) {
#ifdef __LINUX__
                free(p_audio_lang->lang);
#else
                mtos_free(p_audio_lang->lang);//free strdup
#endif
                FILE_SEQ_DEBUG("[%s] -----------  free: audio track lang %d: %s\n", __func__, i, p_audio_lang->lang);
            }

            if (p_audio_lang->title) {
#ifdef __LINUX__
                free(p_audio_lang->title);
#else
                mtos_free(p_audio_lang->title);//free strdup
#endif
                FILE_SEQ_DEBUG("[%s] -----------  free: audio track title %d: %s\n", __func__, i, p_audio_lang->title);
            }
        }

        mtos_free(p_file_seq->audio_lang_array);
        FILE_SEQ_DEBUG("[%s] -----------  free: p_file_seq->audio_lang_array\n", __func__);
        p_file_seq->audio_track_num = 0;
        p_file_seq->audio_lang_array = NULL;
    }
    ds_detect_hls_reset();

    io_isnetworkstream = 0;
    mp_field_pic_flag = 0;
    h264_frm_only_flag = 1;
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
void file_seq_set_live_broadcast(void * p_handle, BOOL value)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);
    g_is_live_broadcast = value;
}
#ifdef SWITCH_DEBUG
extern  int exit_ticks;
extern int switch_mode_flag;

#endif
static int event_sended = 0;
                          void    handle_exit_fill_es_task()
{
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);
#ifdef FILE_PLAYBACK_DEBUG
    FILE_SEQ_LOG("[%s] \n -------- computed bitrate: %d Bps\n", __func__, compute_Bps);
#endif
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    MT_ASSERT(p_file_seq != NULL);

    if (p_file_seq->force_stopped || (p_file_seq->ott_playmode != OTT_LIVE_MODE)) {
        x_stop_av_decoder(p_file_seq);
    }

    if (event_sended != 1) {
        if (p_file_seq->p_sub_fifo_handle) {
			void * p_start_pos = NULL;
			p_start_pos = ((fifo_type_t *)(p_file_seq->p_sub_fifo_handle))->start_pos;
			if(p_start_pos)
			{
             	#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        		file_seq_mem_free(p_file_seq, p_start_pos);
				#else
        		mtos_free(p_start_pos);
				#endif
				((fifo_type_t *)(p_file_seq->p_sub_fifo_handle))->start_pos = 0;			
			}
            deinit_fifo_kw(p_file_seq->p_sub_fifo_handle);
            p_file_seq->p_sub_fifo_handle = NULL;
        }

        //mtos_sem_destroy(&(p_file_seq->sub_fifo_mutex), 0);
        mtos_printk("\n%s %d\n", __func__, __LINE__);
        x_unLoadMedia(p_file_seq);
        mtos_printk("\n%s %d\n", __func__, __LINE__);
        //  x_unmap_es_buffer(p_file_seq);
        reset_file_seq_param(p_file_seq);
        file_seq_set_live_broadcast(p_file_seq, FALSE);
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        file_seq_mem_release(p_file_seq);
#endif
        demux_state = 0;
        mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);

        if (NULL != p_file_seq->event_cb && self_exit == 0) {
            if (p_file_seq->m_play_state ==  FILE_SEQ_EXIT) {
                p_file_seq->m_play_state = FILE_SEQ_STOP;
                FILE_SEQ_LOG("[%s] drv send FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT!!\n", __func__);
                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT, 0);
            } else {
                p_file_seq->m_play_state = FILE_SEQ_STOP;
                FILE_SEQ_LOG("[%s] drv send FILE_PLAYBACK_SEQ_STOP!!\n", __func__);
                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_STOP, 0);
            }
        }

        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    }

    self_exit = 0;
    event_sended = 0;
    p_file_seq->force_stopped = 0;
    //yliu add :for task wait in start;some time this task will too fast to end
    p_file_seq->is_task_alive =  FALSE;
    OS_PRINTF("%s end!!!!\n", __func__);

    mtos_task_exit();

}



/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
BOOL isNetworkStream(void * p_handle)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    int index = p_file_seq->cur_play_index;
    char * p_path = p_file_seq->m_path[index];

    if (p_path) {
        if (strncmp(p_path, "http://", 7) == 0) {
            //FILE_SEQ_DEBUG("[%s] http!!!\n", __func__);
            return TRUE;
        } else if (strncmp(p_path, "rtsp://", 7) == 0) {
            //FILE_SEQ_DEBUG("[%s] rtsp!!!\n", __func__);
            return TRUE;
        } else if (strncmp(p_path, "rtmp://", 7) == 0) {
            //FILE_SEQ_DEBUG("[%s] rtmp!!!\n", __func__);
            return TRUE;
        } else if (strncmp(p_path, "rtmpe://", 8) == 0) {
            //FILE_SEQ_DEBUG("[%s] rtmpe!!!\n", __func__);
            return TRUE;
        } else if (strncmp(p_path, "mms://", 6) == 0) {
            return TRUE;
        } else if (strncmp(p_path, "fifo:http://", 12) == 0) {
            return TRUE;
        }else {
            return FALSE;
        }
    }

    return FALSE;
}


/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
void x_stop_av_decoder(void * pHandle)
{
    FILE_SEQ_T * p_file_seq = pHandle;
    MT_ASSERT(p_file_seq != NULL);
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);

    if (p_file_seq->only_audio_mode == FALSE) {
		if(((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->is_timeshift==1 )
		{
			vdec_freeze_stop(p_file_seq->p_vdec_dev);
		}
		else
		{
			if(p_file_seq->vdec_stop_type == VDEC_FREEZE_STOP)
			{
				vdec_freeze_stop(p_file_seq->p_vdec_dev);
			}				
			else if(p_file_seq->vdec_stop_type == VDEC_SWITCH_CH_STOP)
			{
				vdec_switch_ch_stop(p_file_seq->p_vdec_dev);
			}
			else
			{
				vdec_stop(p_file_seq->p_vdec_dev);
			}
		}
        vdec_set_file_playback_framerate(p_file_seq->p_vdec_dev, 0);
    }

    if (p_file_seq->m_audio_codec_type != 0xffff) {
        aud_stop_vsb(p_file_seq->p_audio_dev);
    }

    if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
        if (p_file_seq->p_vdec_dev) {
            vdec_set_data_input(p_file_seq->p_vdec_dev, 0);
        }
    }

    vdec_set_avsync_mode(p_file_seq->p_vdec_dev, VDEC_AVSYNC_DEFAULT_TS);
    p_file_seq->isAudMute = FALSE;
    p_file_seq->m_aes_left = 0;
    FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);
}
/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
void x_unLoadMedia(void * p_handle)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);
#ifdef FILE_PLAYBACK_DEBUG
    FILE_SEQ_LOG("\n[%s]  ----@@@---- computed bitrate: %d Bps\n", __func__, compute_Bps);
#endif

    if (p_file_seq->p_demuxer) {
        free_demuxer(p_file_seq->p_demuxer);
        p_file_seq->p_demuxer = NULL;
        OS_PRINTF("[%s] free_demuxer\n", __func__);
    }

    //add for teardown of rtsp ts stream by doreen, 14-05-22
    if (p_file_seq->p_demuxer2) {
        demuxer_t * p_demux2 = (demuxer_t *)(p_file_seq->p_demuxer2);
        OS_PRINTF("[%s] p_demux2->file_format: %d, p_demux2->type: %d\n", __func__, p_demux2->file_format, p_demux2->type);

        if (p_demux2->type == DEMUXER_TYPE_RTP) {
            OS_PRINTF("[%s] call free_demuxer(p_file_seq->p_demuxer2)\n", __func__);
            free_demuxer(p_file_seq->p_demuxer2);
        }

        p_file_seq->p_demuxer2 = NULL;
        OS_PRINTF("[%s] free_demuxer2\n", __func__);
    }

    if (p_file_seq->p_stream) {
        free_stream(p_file_seq->p_stream);
        p_file_seq->p_stream = NULL;
        OS_PRINTF("[%s] free_stream\n", __func__);
    }// kill cache thread

    if (p_file_seq ->ts_priv) {
        mtos_free(((ts_info_t *)p_file_seq->ts_priv)->p_pmt) ;
        mtos_free(p_file_seq ->ts_priv);
        p_file_seq ->ts_priv = NULL;
    }

    p_file_seq->loadMedaiOK = FALSE;
    p_file_seq->isAudioEsEnd = FALSE;
    p_file_seq->only_audio_mode = FALSE;
    //m3u8_real_play_pos_t = 0;
    //m3u8_pre_seq_no = -1;
    FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);
}


/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
char * get_demuxer_tag_info(void * demux_handle, char * tag)
{
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);
    char ** info = ((demuxer_t *)demux_handle)->info;
    int n;

    if (!info || !tag) {
        return NULL;
    }

    for (n = 0; info[2 * n] != NULL; n++)
        if (!strcasecmp(info[2 * n], tag)) {
            break;
        }

    FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);
    return info[2 * n + 1] ? strdup(info[2 * n + 1]) : NULL;
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
BOOL set_demux_media_info(void * p_handle)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    demuxer_t * p_demuxer = (demuxer_t *)(p_file_seq->p_demuxer);
    sh_audio_t * p_sh_audio = p_demuxer->audio->sh;
    sh_video_t * p_sh_video = p_demuxer->video->sh;
    double video_start_pts = MP_NOPTS_VALUE;
    double start_pts   = MP_NOPTS_VALUE;
#ifdef __LINUX__
    struct stat stat_buf;
    struct tm * p = NULL;
#endif
    char * p_tag = NULL;
    FILE_SEQ_DEBUG("[%s] start start!!!!!\n", __func__);

    if (p_demuxer->desc->name) {
        FILE_SEQ_DEBUG("[%s] -----ID_DEMUXER=%s\n", __func__, p_demuxer->desc->name);
    } else {
        FILE_SEQ_DEBUG("[%s] ----not find name !!!\n", __func__);
    }

    if (p_sh_video) {
        p_file_seq->video_bps = p_sh_video->i_bps * 8;
        p_file_seq->video_disp_h = p_sh_video->disp_h;
        p_file_seq->video_disp_w = p_sh_video->disp_w;
        p_file_seq->video_fps = (p_sh_video->fps + 0.5);
        video_start_pts = ds_get_next_pts(p_demuxer->video);
        FILE_SEQ_DEBUG("[%s] -----[video] bps=%d, disp_h=%d, disp_w=%d, fps=%d,v_start_pts: %d\n", __func__, \
                       p_file_seq->video_bps, p_file_seq->video_disp_h, p_file_seq->video_disp_w, p_file_seq->video_fps, (int)(video_start_pts * 1000));

        if (p_file_seq->video_fps >= 31) {
            p_file_seq->video_fps = 0;
        }

        if (!p_file_seq->video_disp_w) {
            p_file_seq->internal_event = GET_ES_VIDEO_W_H;
            FILE_SEQ_DEBUG("[%s] -----[video] get video from decoder\n", __func__);
        }
    }

    if (p_sh_audio) {
        /* Assume FOURCC if all bytes >= 0x20 (' ') */
        p_file_seq->audio_bps = p_sh_audio->i_bps * 8;
        p_file_seq->audio_samplerate = p_sh_audio->samplerate;
        p_file_seq->audio_channels = p_sh_audio->channels;
        p_file_seq->audio_samplesize = p_sh_audio->samplesize;
        p_file_seq->audio_language = p_sh_audio->lang;

        if (!p_file_seq->audio_samplerate) {
            if (p_sh_audio->wf) {
                p_file_seq->audio_samplerate = p_sh_audio->wf->nSamplesPerSec;//TODO: sbr: *2
                p_file_seq->audio_channels = p_sh_audio->wf->nChannels;
                FILE_SEQ_DEBUG("[%s] -----[audio] set sample rate & ch with wf(WAVEFORMATEX)\n", __func__);
            }
        }

        start_pts = ds_get_next_pts(p_demuxer->audio);
        FILE_SEQ_DEBUG("[%s] -----[audio] bps=%d, samplerate=%d, channels=%d, samplesize=%d, language: %s, a_start_pts: %d\n", __func__, \
                       p_file_seq->audio_bps, p_file_seq->audio_samplerate, p_file_seq->audio_channels, p_file_seq->audio_samplesize, p_file_seq->audio_language, (int)(start_pts * 1000));
    }

    if (video_start_pts != MP_NOPTS_VALUE) {
        if (start_pts == MP_NOPTS_VALUE || !p_sh_audio ||
        (p_sh_video && video_start_pts < start_pts)) {
            start_pts = video_start_pts;
        }
    }

    if (start_pts != MP_NOPTS_VALUE) {
        FILE_SEQ_DEBUG("[%s] -----file start time =%d ms\n", __func__, (int)(start_pts * 1000));
    } else {
        FILE_SEQ_DEBUG("[%s] -----file start time =unknown\n", __func__);
    }

    p_file_seq->file_start_time = start_pts;
    p_file_seq->file_duration = demuxer_get_time_length(p_demuxer);
    p_file_seq->duration = p_file_seq->file_duration;

    if (p_file_seq->duration - p_file_seq->max_duration > 0) {
        p_file_seq->max_duration = p_file_seq->duration;
    }

    FILE_SEQ_DEBUG("[%s] -----file duration =%d ms\n", __func__, (int)(p_file_seq->file_duration * 1000));
#ifdef __LINUX__
    stat(p_file_seq->m_path[0], &stat_buf);
    p_file_seq->file_size = stat_buf.st_size;
    p = localtime(&stat_buf.st_mtime);//last modify time
    p_file_seq->file_modify_time.Year = p->tm_year + 1900;
    p_file_seq->file_modify_time.Month = p->tm_mon + 1;
    p_file_seq->file_modify_time.Day = p->tm_mday;
    p_file_seq->file_modify_time.Hour = p->tm_hour;
    p_file_seq->file_modify_time.Minute = p->tm_min;
    p_file_seq->file_modify_time.Second = p->tm_sec;
    FILE_SEQ_DEBUG("[%s] -----file size =%d bytes\n", __func__, p_file_seq->file_size);
    FILE_SEQ_DEBUG("[%s] -----file modify time: %d-%d-%d  %2d:%2d:%2d \n", __func__, \
                   p_file_seq->file_modify_time.Year, p_file_seq->file_modify_time.Month, p_file_seq->file_modify_time.Day, \
                   p_file_seq->file_modify_time.Hour, p_file_seq->file_modify_time.Minute, p_file_seq->file_modify_time.Second);
#endif

    if (p_demuxer->type == DEMUXER_TYPE_MPEG_TS) {
        FILE_SEQ_DEBUG("\n[%s] -----p_demuxer->type == DEMUXER_TYPE_MPEG_TS\n", __func__);
        p_file_seq->is_ts = 1;
        set_ts_pmt_avs_info(p_file_seq->ts_priv);
        p_file_seq->internal_event = GET_TS_MEDIA_INFO;
    }

    FILE_SEQ_DEBUG("[%s] end end!!!!!\n", __func__);
    return TRUE;
}








/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
int get_preload_buffer(preload_buffer_t * buffer_start, u32 size, u8 ** buffer)
{
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    MT_ASSERT(p_file_seq != NULL);
    preload_buffer_t * p_buffer = buffer_start;
    *buffer = 0;

    while (p_file_seq->m_preload_state != FILE_SEQ_EXIT) {
        if ((p_buffer->write_pos < p_buffer->read_pos) &&
        ((p_buffer->write_pos +  size) >= p_buffer->read_pos)) {
            mtos_task_sleep(10);
            return -1;
        } else {
            break;
        }

        if (p_file_seq->m_preload_state == FILE_SEQ_EXIT) {
            return 0;
        }
    }

    while (((p_buffer->buffer_len - p_buffer->write_pos) < size) && (p_buffer->read_pos <= size) && (p_file_seq->m_preload_state != FILE_SEQ_EXIT)) {
        if (p_file_seq->m_preload_state != FILE_SEQ_EXIT) {
            mtos_task_sleep(10);
            return -1;
        } else {
            return 0;
        }
    }

    if ((p_buffer->buffer_len - p_buffer->write_pos) >= size) {
        *buffer = p_buffer->buffer_start + p_buffer->write_pos;
        p_buffer->write_pos += size;
        return 0;
    }

    if (p_buffer->read_pos > size) {
        *buffer = p_buffer->buffer_start;
        p_buffer->write_pos = size;
        printf("########### buffer rollback\n");
    }

    return 0;
}
BOOL check_es_finish(FILE_SEQ_T * p_file_seq)
{
    int freespace = 0;
    int ves_buf_size = 0;
    ///
    u32 last_vpts = p_file_seq->vpts_upload;
    int max_freespace = 0;
    double seq_pts = p_file_seq->first_vpts ;
    vdec_get_es_buf_space(p_file_seq->p_vdec_dev, (u32 *)&freespace);
    vdec_get_es_buf_size(p_file_seq->p_vdec_dev, (u32 *)&ves_buf_size);
    max_freespace = freespace;

    ///
    while (freespace < ves_buf_size) {
        u32 vpts = 0;
        int check_cnt = 0;
        int check_stop_cnt = 0;
        int check_stable_vpts_cnt = 0;
        vdec_info_t vstate;
        vdec_get_info(p_file_seq->p_vdec_dev, &vstate);
        vpts = vstate.pts;

        if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
            vpts = vpts & (0x7fffffff);
        }

        //#ifdef SWITCH_DEBUG
        if ((p_file_seq->load_media_state == FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS
             && p_file_seq->ott_playmode == OTT_LIVE_MODE)
            || (p_file_seq->ott_playmode != OTT_LIVE_MODE && switch_mode_flag)
            || (p_file_seq->force_stopped == 1)
            //||(p_file_seq->load_media_state == FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR)
        ) {
            return TRUE;
        }

        //#endif
        if (vpts > seq_pts * TIME_BASE) {
            vpts -= seq_pts * TIME_BASE;
        } else {
            vpts = 0;
        }

        mtos_task_sleep(200);

        if (vpts > last_vpts + 45000) {
            if (p_file_seq->event_cb) {
                FILE_SEQ_DEBUG("[%s] 2222aaa2!!!\n", __func__);
                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS, vpts / TIME_BASE);
            }

            last_vpts = vpts;
        }

        if (last_vpts == vpts) {
            check_stable_vpts_cnt++;
        }

        if (check_stable_vpts_cnt > 15) {
            //break;
        }

        //mtos_task_sleep(100);
        //}
        //while(freespace < 3072)
        //{
        if (do_user_cmd()) {
            return FALSE;
        }

        if (OTT_LIVE_MODE != p_file_seq->ott_playmode && !switch_mode_flag) {
            handle_pending_event();
        }

        //#ifndef SWITCH_DEBUG

        if (p_file_seq->m_play_state == FILE_SEQ_EXIT && (p_file_seq->ott_playmode != OTT_LIVE_MODE || p_file_seq->force_stopped == 1)) {
            return TRUE;
        }

        //#endif

        if (p_file_seq->m_play_state == FILE_SEQ_PAUSE) {
            continue;
        }

        //mtos_task_sleep(10);
        vdec_get_es_buf_space(p_file_seq->p_vdec_dev, (u32 *)&freespace);

        if (max_freespace == freespace) {
            check_stop_cnt++;
        }

        if (freespace > max_freespace) {
            max_freespace = freespace;
            check_stop_cnt = 0;
        }

        if (p_file_seq->m_play_state == FILE_SEQ_EXIT && check_stop_cnt > 10) {
            return TRUE;
        }

        //yliu modify
        if (freespace >= (ves_buf_size - 21)) {
            if (check_cnt++ > 5) {
                if (NULL != p_file_seq->event_cb && p_file_seq->max_video_pts > p_file_seq->first_vpts) {
                    p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS,
                                         (u32)(p_file_seq->max_video_pts - p_file_seq->first_vpts));
                }

                p_file_seq->is_play_to_end = 1;
                return TRUE;
            }
        }

        FILE_SEQ_DEBUG("[%s] free space %d\n", __func__, freespace);
    }

    if (NULL != p_file_seq->event_cb && p_file_seq->max_video_pts > p_file_seq->first_vpts) {
        p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS,
                             (u32)(p_file_seq->max_video_pts - p_file_seq->first_vpts));
    }

    FILE_SEQ_DEBUG("[%s] av decoder finished !!!\n", __func__);
    p_file_seq->is_play_to_end = 1;
    //        if(NULL != p_file_seq->event_cb)
    //        {
    //            p_file_seq->event_cb(FILE_PLAYBACK_SEQ_STOP, 0);
    //        }
    return TRUE;
}
void  check_es_thread(void * p_param)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_param;
    p_file_seq->check_task_state = CHECK_ES_TRUE;

    while (p_file_seq->check_task_state != CHECK_ES_IDLE) {
        mtos_task_sleep(300);
    }

    while (p_file_seq->check_task_state != CHECK_ES_EXIT) {
        if (p_file_seq->check_task_state == CHECK_ES_IDLE) {
            mtos_task_sleep(300);
            //mtos_printk("\n!!!!!@@%s  %d\n",__func__,__LINE__);
        } else if (p_file_seq->check_task_state == CHECK_ES_RUNNING) {
            p_file_seq->check_task_result = check_es_finish(p_file_seq);
            p_file_seq->check_task_state = CHECK_ES_IDLE;
            // mtos_printk("\n!!!!!@@%s  %d\n",__func__,__LINE__);
        }
    }

    p_file_seq->check_task_state = CHECK_ES_FALSE;
    mtos_task_exit();
}

/*
 *
 *
 *
 *
 *
 */

BOOL check_task_finish(FILE_SEQ_T * p_file_seq)
{
    /*
     *
     *  when all the audio and video es data has been demuxed,
     *    sequencer should wait av_decoder finished !!!
     *                                    peacer add 20130427-1500
     */
    if (p_file_seq->only_audio_mode && p_file_seq->isAudioEsEnd) {
        FILE_SEQ_DEBUG("[%s] audio  demux stream is finished  !!\n", __func__);
        int check_cnt = 0;

        do {
            int  cur_aud_freespace = 0;
            float   aes_buf_consume_ms = 0.0;
	    if (p_file_seq->m_play_state == FILE_SEQ_EXIT) {
		    return TRUE;
	    }
            if (do_user_cmd()) {
                return FALSE;
            }

#ifdef ES_END_IN_TASK

            if (p_file_seq->init_buffering_finish != 1) {
                aud_file_getleftes_vitual_buffer(p_file_seq, (u32 *)&cur_aud_freespace);
            } else
#endif
                aud_file_getleftesbuffer_vsb(p_file_seq->p_audio_dev, (u32 *) &cur_aud_freespace);

            aes_buf_consume_ms = (((float)(p_file_seq->dec_cap.max_aes_num - cur_aud_freespace) * 1.0) / (p_file_seq->audio_bps) * 1.0) * 1000.0;

            if (p_file_seq->orig_apts -  aes_buf_consume_ms  > (float)(p_file_seq->apts_upload) + 1500.0) {
                p_file_seq->apts_upload = (u32)(p_file_seq->orig_apts -  aes_buf_consume_ms);

                if (NULL != p_file_seq->event_cb) {
                	if(aes_buf_consume_ms>0){
                    p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS, p_file_seq->apts_upload);
                    		FILE_SEQ_DEBUG("[%s] send apts %ld,%u,%u\n",__func__,p_file_seq->apts_upload,(u32)(p_file_seq->orig_apts),(u32)(aes_buf_consume_ms));
	              }
                }
            }

            if (cur_aud_freespace >= (p_file_seq->dec_cap.max_aes_num - 1024)) {
                if (check_cnt++ > 5) {
                    if (NULL != p_file_seq->event_cb) {
                        if(aes_buf_consume_ms>0){
                        p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS, (u32)(p_file_seq->max_audio_pts));
	                  FILE_SEQ_DEBUG("[%s]send apts %u, %d, %u\n",__func__,(u32)(p_file_seq->max_audio_pts),cur_aud_freespace,(u32)aes_buf_consume_ms);
                    	}
                    }

                    p_file_seq->is_play_to_end = 1;
                    FILE_SEQ_LOG("[%s] audio decoder finished !!!\n", __func__);
                    return TRUE;
                }
            } else {
                mtos_task_sleep(100);
                check_cnt = 0;
                //FILE_SEQ_DEBUG("[%s] wait decoder finished f[%ld] h[%ld] !!!\n",__func__,cur_aud_freespace,p_file_seq->dec_cap.max_aes_num-64);
            }
        } while (1);
    }

    if (p_file_seq->isVideoEsEnd  && p_file_seq->isAudioEsEnd) {
        FILE_SEQ_DEBUG("[%s] audio and video demux stream is finished  !!\n", __func__);
#ifdef SWITCH_DEBUG
        exit_ticks = mtos_ticks_get();
#endif

        do {
            vdec_info_t  cur_vdec_info;
            memset(&cur_vdec_info, 0, sizeof(vdec_info_t));

            //vdec_get_info(p_file_seq->p_vdec_dev,&cur_vdec_info);
            if (1) { //cur_vdec_info.err == VDEC_ERROR_THIRSTY)
                //mtos_task_sleep(200);
                u32 last_vpts = p_file_seq->vpts_upload;
                int check_stable_vpts_cnt = 0;
                u32 vpts = 0;
                int freespace = 0;
                int max_freespace = 0;
                int check_cnt = 0;
                int check_stop_cnt = 0;
                int check_free_cnt = 0;
                int oldfreespace = 0;
                int ves_buf_size = 0;
                double seq_pts = p_file_seq->first_vpts ;
                vdec_get_es_buf_space(p_file_seq->p_vdec_dev, (u32 *)&freespace);
                vdec_get_es_buf_size(p_file_seq->p_vdec_dev, (u32 *)&ves_buf_size);
                max_freespace = freespace;

                //#ifdef SWITCH_DEBUG
                if (p_file_seq->ott_playmode == OTT_LIVE_MODE && (p_file_seq->force_stopped != 1) && (p_file_seq->m_play_state == FILE_SEQ_EXIT)) {
                    if ((NULL != p_file_seq->event_cb)) {
                        if (p_file_seq->p_sub_fifo_handle) {
							void * p_start_pos = NULL;
							p_start_pos = ((fifo_type_t *)(p_file_seq->p_sub_fifo_handle))->start_pos;
							if(p_start_pos)
							{
								#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
								file_seq_mem_free(p_file_seq, p_start_pos);
								#else
								mtos_free(p_start_pos);
								#endif
								((fifo_type_t *)(p_file_seq->p_sub_fifo_handle))->start_pos = 0;			
							}							
                            deinit_fifo_kw(p_file_seq->p_sub_fifo_handle);
                            p_file_seq->p_sub_fifo_handle = NULL;
                        }

                        //mtos_sem_destroy(&(p_file_seq->sub_fifo_mutex), 0);
                        mtos_printk("\n%s %d\n", __func__, __LINE__);
                        x_unLoadMedia(p_file_seq);
                        mtos_printk("\n%s %d\n", __func__, __LINE__);
                        // x_unmap_es_buffer(p_file_seq);

                        if (p_file_seq->p_aud_assemble_buf) {
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
                            file_seq_mem_free(p_file_seq, p_file_seq->p_aud_assemble_buf);
#else
                            mtos_free(p_file_seq->p_aud_assemble_buf);
#endif
                            p_file_seq->p_aud_assemble_buf = NULL;
                        }

                        if (p_file_seq->ves_seg_buf) {
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
                            file_seq_mem_free(p_file_seq, p_file_seq->ves_seg_buf);
#else
                            mtos_free(p_file_seq->ves_seg_buf);
#endif
                            p_file_seq->ves_seg_buf = NULL;
                        }

                        reset_file_seq_param(p_file_seq);
                        file_seq_set_live_broadcast(p_file_seq, FALSE);
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
                        file_seq_mem_release(p_file_seq);
#endif
                        exit_ticks  = mtos_ticks_get() - exit_ticks;
                        mtos_printk("\n@@@@@%s %d exit:%d\n", __func__, __LINE__, exit_ticks);
                        switch_mode_flag = 1;
                        p_file_seq->load_media_state = FILE_PLAYBACK_SEQ_STOP;
                        event_sended = 1;
                        demux_state = 0;
                        mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);

                        if (NULL != p_file_seq->event_cb && self_exit == 0) {
                            if (p_file_seq->m_play_state == FILE_SEQ_EXIT) {
                                p_file_seq->m_play_state = FILE_SEQ_STOP;
                                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT, 0);
                            } else {
                                p_file_seq->m_play_state = FILE_SEQ_STOP;
                                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_STOP, 0);
                            }
                        }

                        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
                    }
                }

                //#endif
#ifdef ES_END_IN_TASK
                p_file_seq->check_task_state = CHECK_ES_RUNNING;
                //while(p_file_seq->check_task_state!=CHECK_ES_IDLE)
                {
                    mtos_task_sleep(300);
                }
                mtos_printk("\n!!!@@@%s %d\n", __func__, __LINE__);
                return p_file_seq->check_task_result;
#else

                while (freespace < ves_buf_size) {
#if 0//delete by doreen, 2013-06-24                 
                    //#ifdef __LINUX__
                    //                  vpts = *(p_file_seq->p_current_sys_vpts);
                    //#else
                    vpts = vdec_dec_get_vpts(p_file_seq->p_vdec_dev);
                    //#endif
#else
                    vdec_info_t vstate;
                    vdec_get_info(p_file_seq->p_vdec_dev, &vstate);
                    vpts = vstate.pts;

                    if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
                        vpts = vpts & (0x7fffffff);
                    }

                    //#ifdef SWITCH_DEBUG
                    if ((p_file_seq->load_media_state == FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS
                         && p_file_seq->ott_playmode == OTT_LIVE_MODE)
                        || (p_file_seq->ott_playmode != OTT_LIVE_MODE && switch_mode_flag)
                        || (p_file_seq->force_stopped == 1)
                        //||(p_file_seq->load_media_state == FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR)
                    ) {
                        return TRUE;
                    }

                    //#endif
                    if (vpts > seq_pts * TIME_BASE) {
                        vpts -= seq_pts * TIME_BASE;
                    } else {
                        vpts = 0;
                    }

#endif
                    mtos_task_sleep(200);

                    if (vpts > last_vpts + 45000) {
                        if (p_file_seq->event_cb) {
                            FILE_SEQ_DEBUG("[%s] 2222aaa2!!!\n", __func__);
                            p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS, vpts / TIME_BASE);
                        }

                        last_vpts = vpts;
                    }

                    if (last_vpts == vpts) {
                        check_stable_vpts_cnt++;
                    }

                    if (check_stable_vpts_cnt > 15) {
                        //break;
                    }

                    //mtos_task_sleep(100);
                    //}
                    //while(freespace < 3072)
                    //{
                    if (do_user_cmd()) {
                        return FALSE;
                    }

                    if (OTT_LIVE_MODE != p_file_seq->ott_playmode && !switch_mode_flag) {
                        handle_pending_event();
                    }

                    //#ifndef SWITCH_DEBUG

                    if ((p_file_seq->m_play_state == FILE_SEQ_EXIT || p_file_seq->m_play_state == FILE_SEQ_STOP) && (p_file_seq->ott_playmode != OTT_LIVE_MODE || p_file_seq->force_stopped == 1)) {
                        return TRUE;
                    }

                    //#endif

                    if (p_file_seq->m_play_state == FILE_SEQ_PAUSE) {
                        continue;
                    }

                    //mtos_task_sleep(10);
                    vdec_get_es_buf_space(p_file_seq->p_vdec_dev, (u32 *)&freespace);

                    if (max_freespace == freespace) {
                        check_stop_cnt++;
                    }

                    if (freespace > max_freespace) {
                        max_freespace = freespace;
                        check_stop_cnt = 0;
                    }

                    if (p_file_seq->m_play_state == FILE_SEQ_EXIT && check_stop_cnt > 10) {
                        return TRUE;
                    }

                    if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
                        if (freespace == oldfreespace) {
                            if (check_free_cnt++ > 5) {
                                if (NULL != p_file_seq->event_cb && p_file_seq->max_video_pts > p_file_seq->first_vpts) {
                                    p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS,
                                                         (u32)(p_file_seq->max_video_pts - p_file_seq->first_vpts));
                                }

                                p_file_seq->is_play_to_end = 1;
                                return TRUE;
                            }
                        } else {
                            oldfreespace = freespace;
                            check_free_cnt = 0;
                        }
                    }

                    //yliu modify
                    if (freespace >= (ves_buf_size - 21)) {
                        if (check_cnt++ > 5) {
                            if (NULL != p_file_seq->event_cb && p_file_seq->max_video_pts > p_file_seq->first_vpts) {
                                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS,
                                                     (u32)(p_file_seq->max_video_pts - p_file_seq->first_vpts));
                            }

                            p_file_seq->is_play_to_end = 1;
                            return TRUE;
                        }
                    }

                    FILE_SEQ_DEBUG("[%s] free space %d\n", __func__, freespace);
                }

                if (NULL != p_file_seq->event_cb && p_file_seq->max_video_pts > p_file_seq->first_vpts) {
                    p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS,
                                         (u32)(p_file_seq->max_video_pts - p_file_seq->first_vpts));
                }

                FILE_SEQ_DEBUG("[%s] av decoder finished !!!\n", __func__);
                p_file_seq->is_play_to_end = 1;
                //        if(NULL != p_file_seq->event_cb)
                //        {
                //            p_file_seq->event_cb(FILE_PLAYBACK_SEQ_STOP, 0);
                //        }
#endif
                return TRUE;
            } else {
                mtos_task_sleep(200);
                FILE_SEQ_DEBUG("[%s] wait decoder finished !!!\n", __func__);
            }
        } while (1);
    }

    return FALSE;
}

#ifdef ENABLE_DEMUX_RTSP

//#define RECORD_CAMERA_VIDEO
#define RTSP_RING_BUF_LEN  256*1024

extern int  set_camera_buffer(camera_buffer_t * handle, u8 * data, int len);


/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static int file_seq_rtsp_video_rec(void * p_handle, int rec_flag)
{
    OS_PRINTF("[%s] start start ...\n", __func__);
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    p_file_seq->camera_rtsp_record = rec_flag;
    OS_PRINTF("[%s] set rec_flag: %d\n", __func__, rec_flag);
    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    return 0;
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
int flag_rtsp_fill_start = 0;
                           int total_malloc_size = 0;

                                   static void rtsp_fill_es_task(void * p_param)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_param;
    MT_ASSERT(p_file_seq != NULL);
    OS_PRINTF("[%s] start start ...\n", __func__);
    int cur_video_packet_size = 0;
    int  cur_audio_packet_size = 0;
    int wr_cnt  = 0;
    short get_av_pkt = 1;//0: video, 1:audio
    short get_av_pkt_pre = 0;//0: video, 1:audio
    short con_get_aud_pkts = 0;
    short con_get_vid_pkts = 0;
    int loop_count = 0;
    demux_stream_t * ds_a = NULL;
    demux_stream_t * ds_v = NULL;
    demux_stream_t * ds_sub = NULL;
    short is_support_audio_codec = 0;
    short a_v_exist_flag = -1;//0:audio video exist; 1:only audio exist; 2:only video exist;
    //set camera record mode
    //p_file_seq->camera_rtsp_record = 1;
#ifdef RECORD_CAMERA_VIDEO
    camera_buffer_t rtsp_v_ring_buffer;
    memset(&rtsp_v_ring_buffer, 0, sizeof(camera_buffer_t));
    rtsp_v_ring_buffer.buffer_len = RTSP_RING_BUF_LEN;
    rtsp_v_ring_buffer.read_pos = 0;
    rtsp_v_ring_buffer.write_pos = 0;
    rtsp_v_ring_buffer.total_data = 0;
    rtsp_v_ring_buffer.buffer = mtos_malloc(RTSP_RING_BUF_LEN);
    MT_ASSERT(rtsp_v_ring_buffer.buffer != NULL);
    memset(rtsp_v_ring_buffer.buffer, 0, RTSP_RING_BUF_LEN);
    int write_len = 0;
    char * p_write = NULL;
    u8  record_camera_video_file[32] = "rtsp.dump.video.es.musbc";
#ifdef __LINUX__
    FILE  * p_record_video = NULL;
    p_record_video = fopen(record_camera_video_file, "wb");

    if (!p_record_video) {
        FILE_SEQ_DEBUG("[%s] ---------open rtsp.dump.video.es error\n", __func__);
    }

#else
    static  ufs_file_t  * p_record_video = NULL;
    u16 * p_name2 = ConvertUtf8ToUnicode(record_camera_video_file);
    p_record_video = mtos_malloc(sizeof(ufs_file_t));
    memset(p_record_video, 0, sizeof(ufs_file_t));

    if (ufs_open(p_record_video, p_name2, UFS_WRITE | UFS_CREATE_NEW_COVER) != FR_OK) {
        FILE_SEQ_DEBUG("[%s] ERROR fail open rtsp.dump.video.es...\n", __func__);
    } else {
        FILE_SEQ_DEBUG("[%s] open :%s ok...\n", __func__, record_camera_video_file);
    }

#endif
#endif
    is_support_audio_codec = p_file_seq->is_av_codec_support & 0x1;
    ds_a = p_file_seq->p_ds_audio;
    ds_v = p_file_seq->p_ds_video;
    ds_sub = p_file_seq->p_ds_sub;
    p_file_seq->needNewVideoData = TRUE;
    p_file_seq->needNewAudioData = TRUE;
    p_file_seq->p_aud_assemble_buf = mtos_malloc(AUD_ASSEMBLE_BUF_LEN);//fix me
    p_file_seq->ves_seg_buf = mtos_malloc(VES_SEG_LEN);

    if (p_file_seq->p_aud_assemble_buf) {
        memset(p_file_seq->p_aud_assemble_buf, 0, AUD_ASSEMBLE_BUF_LEN);
    } else {
        FILE_SEQ_DEBUG("[%s][ERROR] fail to malloc audie tmp buffer!!!\n", __func__);
        return;
    }

    if (p_file_seq->ves_seg_buf) {
        memset(p_file_seq->ves_seg_buf, 0, VES_SEG_LEN);
    } else {
        FILE_SEQ_DEBUG("[%s][ERROR] fail to malloc audie tmp buffer!!!\n", __func__);
        return;
    }

    p_file_seq->m_play_state = FILE_SEQ_PLAY;
    p_file_seq->is_task_alive = TRUE;

    if(ds_a->sh && ds_v->sh)
    	a_v_exist_flag = 0;
    else if(ds_a->sh)
    	a_v_exist_flag = 1;//only audio
    else if(ds_v->sh)
    	a_v_exist_flag = 2;//only video
    else
    	a_v_exist_flag = -1;

    if(a_v_exist_flag == -1){
	FILE_SEQ_ERROR("[%s][ERROR] No found audio and video, exit task!!!\n", __func__);
	goto EXIT_RTSP_FILL_ES_TASK;
    }

    
#if    defined(NOT_PUSH_ES_TO_DECODER)
    int  totalVideoFrame = 0;
    int  cur_frame_num = 0;
    u32  last_tick = 0;
    u32  cur_tick = 0;
#endif
    p_file_seq->orig_vpts = p_file_seq->orig_apts = p_file_seq->file_start_time;

    if(a_v_exist_flag == 1){//only audio
    		get_av_pkt = 1;
	}
    else if(a_v_exist_flag == 2)//only video
    		get_av_pkt = 0;

    if (p_file_seq->file_start_time < 0) {
        OS_PRINTF("[%s] --------p_file_seq->file_start_time < 0!!! %d\n", __func__, (int)(p_file_seq->file_start_time));
    }

    OS_PRINTF("[%s] --------start to while loop... is_support_audio_codec: %d, camera_rtsp_record: %d, a_v_exist_flag: %d(0: av, 1:a, 2:v)\n", \
              __func__, is_support_audio_codec, p_file_seq->camera_rtsp_record,a_v_exist_flag);

    //flag_rtsp_fill_start = 2;//1:while 1 get udp packets

    while (1) {
#if 1
        /*
         *   excute user's command
         */
        do_user_cmd();
        /*
         *   excute user's command
         */
        handle_pending_event();
#endif

        if (p_file_seq->m_play_state == FILE_SEQ_EXIT) {
            break;
        } else if (p_file_seq->m_play_state == FILE_SEQ_PAUSE) {
            mtos_task_sleep(30);
            continue;
        }

        if (loop_count++ > 32) {
            loop_count = 0;
            mtos_task_sleep(20); //for lower prio task
        }

        //u32 interval = 500;//ms
        //check_es_water_level(p_file_seq,interval);
        //OS_PRINTF("[pts:v/a][%ld/%ld]\n",(u32)(p_file_seq->orig_vpts),(u32)(p_file_seq->orig_apts));

        if (a_v_exist_flag!=1 && ((p_file_seq->orig_vpts   <=   p_file_seq->orig_apts)  || (get_av_pkt == 0))) {

        //OS_PRINTF("[%s] [%d]  ds_a:0x%x\n", __func__, __LINE__,ds_a);
            //if(get_av_pkt == 0 ){
            cur_video_packet_size = 0;
            cur_video_packet_size = ds_get_packet_video(ds_v, &(p_file_seq->p_v_pkt_start), 0);

            if (get_av_pkt_pre == 0) {
                con_get_vid_pkts++;
            } else {
                con_get_vid_pkts = 0;
            }

            //if(ds_a->pts < ds_v->pts)
            //  get_av_pkt = 1;
            if (a_v_exist_flag == 0 && con_get_vid_pkts > CONTINUE_READ_VID_PACKETS_THRESHOLD) {
                //OS_PRINTF("[%s] force to get audio packet! con_get_vid_pkts[%d]\n", __func__, con_get_vid_pkts);
                get_av_pkt = 1;
            }

            get_av_pkt_pre = 0;
            // if (ds_v->pts < 0.0)
            {
                //OS_PRINTF("[%s] --------%d:[vpts:%d][apts:%d] get video packet, %d, con_get_vid_pkts[%d], V\n", __func__, while_cnt, (int)(ds_v->pts * 1000.0), (int)(ds_a->pts * 1000.0), cur_video_packet_size, con_get_vid_pkts);
            }

            if (cur_video_packet_size <= 0 || p_file_seq->p_v_pkt_start == NULL) {
                FILE_SEQ_ERROR("[%s][ERROR][ERROR]!!!!!\n", __func__);
                FILE_SEQ_ERROR("[%s][ERROR][ERROR] video packet size [%d]!!\n", __func__, cur_video_packet_size);
                FILE_SEQ_ERROR("[%s][ERROR][ERROR] videop_v_pkt_start [0x%x]!!\n", __func__, p_file_seq->p_v_pkt_start);

                if (ds_v->eof) {
                    break;
                }
            } else {
#if 0

                if (ds_a->pts < 0.0) {
                    p_file_seq->orig_apts = 0.0;
                } else {
                    p_file_seq->orig_apts =  ds_a->pts * 1000.0;
                }

                if (ds_v->pts < 0.0) {
                    p_file_seq->orig_vpts = 0.0;
                } else {
                    p_file_seq->orig_vpts =  ds_v->pts * 1000.0;
                }

#else
                p_file_seq->orig_apts =  ds_a->pts * 1000.0;
                p_file_seq->orig_vpts =  ds_v->pts * 1000.0;
#endif
                p_file_seq->sys_vpts = ((u32)(p_file_seq->orig_vpts * PTS_BASE)) | VALID_PTS_MASK;

                if (cur_video_packet_size > 0) {
                    if (cur_video_packet_size > max_video_pkt) {
                        max_video_pkt = cur_video_packet_size;
                    }

#ifdef __LINUX__
                    //   memcpy(p_file_seq->p_tmp_ves_buf, p_file_seq->p_v_pkt_start + (0), cur_video_packet_size);
#endif
#if    defined(NOT_PUSH_ES_TO_DECODER)
                    cur_frame_num ++ ;
                    cur_tick = mtos_ticks_get();

                    if ((cur_tick - last_tick) * 10  > (1000)) { //one seconds la
                        last_tick = cur_tick;
                        cur_frame_num = 0;
                    }

                    OS_PRINTF("fn:%d\n", cur_frame_num);
                    OS_PRINTF("v:%ld %ld\n", cur_video_packet_size, (u32)p_file_seq->orig_vpts);
#else
                    //push data to ves buffer
#ifdef __LINUX__
                    vdec_dec_push_es(p_file_seq->p_vdec_dev, (u32)(p_file_seq->p_v_pkt_start + (0)), cur_video_packet_size, p_file_seq->sys_vpts);
#else
                    vdec_dec_push_es(p_file_seq->p_vdec_dev, (u32)p_file_seq->p_v_pkt_start, cur_video_packet_size, p_file_seq->sys_vpts);
#endif
#endif

                    //push data to ring buffer and write data to USB
                    if (p_file_seq->camera_rtsp_record == 1) {
#ifdef RECORD_CAMERA_VIDEO
                        set_camera_buffer(&rtsp_v_ring_buffer, p_file_seq->p_v_pkt_start, cur_video_packet_size);
                        //FILE_SEQ_DEBUG("[%s]------write video data to ring buffer!, size[%d]!!\n", __func__, cur_video_packet_size);
                        //rtsp_v_ring_buffer.read_pos = rtsp_v_ring_buffer.write_pos;
                        //rtsp_v_ring_buffer.total_data = 0;
                        write_len = rtsp_v_ring_buffer.buffer_len / 2;

                        if (rtsp_v_ring_buffer.total_data >= write_len) {
                            p_write = rtsp_v_ring_buffer.buffer + rtsp_v_ring_buffer.read_pos;
                            //write data to usb
#ifdef __LINUX__

                            if (p_record_video) {
                                fwrite(p_write, write_len, 1, p_record_video);
                                FILE_SEQ_DEBUG("[%s]------write data to file!, size[%d]!!\n", __func__, write_len);
                            }

#else
                            int r = 0;

                            if (p_record_video) {
                                if (ufs_write(p_record_video, p_write, write_len, (u32 *) &r) != FR_OK) {
                                    FILE_SEQ_ERROR("[%s][error]------ufs_write error!!\n", __func__);
                                } else {
                                    FILE_SEQ_DEBUG("[%s]------write data to file!, size[%d]!!\n", __func__, write_len);
                                }
                            }

#endif
                            rtsp_v_ring_buffer.read_pos += write_len;

                            if (rtsp_v_ring_buffer.read_pos == rtsp_v_ring_buffer.buffer_len) {
                                rtsp_v_ring_buffer.read_pos = 0;
                            }

                            rtsp_v_ring_buffer.total_data -= write_len;
                        }

#endif
                    }
                }
            }
        } else { //if((p_file_seq->orig_vpts   >=   p_file_seq->orig_apts)  || (get_av_pkt == 1 ))
            p_file_seq->p_a_pkt_start = NULL;
            cur_audio_packet_size = 0;
            p_file_seq->p_extra_aud_buf = NULL;
            p_file_seq->extra_audio_size = 0;
            cur_audio_packet_size = ds_get_packet_audio(ds_a, &(p_file_seq->p_a_pkt_start), &(p_file_seq->p_extra_aud_buf), (uint8_t **) & (p_file_seq->extra_audio_size));

            if (get_av_pkt_pre == 1) {
                con_get_aud_pkts++;
            } else {
                con_get_aud_pkts = 0;
            }

            if (a_v_exist_flag == 0 && con_get_aud_pkts > CONTINUE_READ_AUD_PACKETS_THRESHOLD) {
                OS_PRINTF("[%s] force to get video packet! con_get_aud_pkts[%d]\n", __func__, con_get_aud_pkts);
                get_av_pkt = 0;
            }

            get_av_pkt_pre = 1;
            //if (ds_a->pts < 0.0)
            {
                //OS_PRINTF("[%s] --------%d:[vpts:%d][apts:%d] get audio packet, %d, con_get_aud_pkts[%d], A\n", __func__, while_cnt, (int)(ds_v->pts * 1000.0), (int)(ds_a->pts * 1000.0), cur_audio_packet_size, con_get_aud_pkts);
            }

            if (cur_audio_packet_size <= 0) {
                FILE_SEQ_ERROR("[%s][ERROR][ERROR] audio packet size [%d]!!\n", __func__, cur_audio_packet_size);

                if (ds_a->eof) {
                    break;
                }
            }

#if 0

            if (ds_a->pts < 0.0) {
                p_file_seq->orig_apts = 0.0;
            } else {
                p_file_seq->orig_apts =  ds_a->pts * 1000.0;
            }

            if (ds_v->pts < 0.0) {
                p_file_seq->orig_vpts = 0.0;
            } else {
                p_file_seq->orig_vpts =  ds_v->pts * 1000.0;
            }

#else
            p_file_seq->orig_apts =  ds_a->pts * 1000.0;
            p_file_seq->orig_vpts =  ds_v->pts * 1000.0;
#endif

            if (cur_audio_packet_size > max_audio_pkt) {
                max_audio_pkt = cur_audio_packet_size;
            }

            //if support audio codec, copy packet to aes buffer and then push it to the decoder
            if (is_support_audio_codec == 1) {
                if (p_file_seq->p_extra_aud_buf && p_file_seq->extra_audio_size) {
                    memcpy(p_file_seq->p_aud_assemble_buf, p_file_seq->p_extra_aud_buf, p_file_seq->extra_audio_size);
                    free(p_file_seq->p_extra_aud_buf);
                }

                if (p_file_seq->p_a_pkt_start && cur_audio_packet_size) {
                    wr_cnt = 0;
                    p_file_seq->sys_apts = (u32)((p_file_seq->orig_apts * PTS_BASE)) | VALID_PTS_MASK;

                    if ((cur_audio_packet_size + p_file_seq->extra_audio_size) > AUD_ASSEMBLE_BUF_LEN) {
                        FILE_SEQ_ERROR("[%s][ERROR][ERROR] audio packet[%d] too large(>16k), memory crossed!!\n", __func__, cur_audio_packet_size);
                    }

                    memcpy(p_file_seq->p_aud_assemble_buf + p_file_seq->extra_audio_size, p_file_seq->p_a_pkt_start, cur_audio_packet_size);
#if    defined(NOT_PUSH_ES_TO_DECODER)
                    OS_PRINTF("a:%ld  %ld\n", cur_audio_packet_size, (u32)p_file_seq->orig_apts);
#else
                    adec_push_audio_es(p_file_seq->p_aud_assemble_buf, cur_audio_packet_size + p_file_seq->extra_audio_size, p_file_seq->sys_apts);
#endif
                }
            } else {
                //OS_PRINTF("[%s] --------throw the audio packet away for unsupported audio codec\n", __func__);
            }
        }

        while_cnt++;
        //OS_PRINTF("[%s] --------while_cnt: %d, orig_vpts[%d] orig_apts[%d] get_av_pkt[%d]\n", __func__,while_cnt,(int)(p_file_seq->orig_vpts),(int)(p_file_seq->orig_apts),get_av_pkt);
    }

EXIT_RTSP_FILL_ES_TASK:

    if (p_file_seq->p_aud_assemble_buf) {
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        file_seq_mem_free(p_file_seq, p_file_seq->p_aud_assemble_buf);
#else
        mtos_free(p_file_seq->p_aud_assemble_buf);
#endif
        p_file_seq->p_aud_assemble_buf = NULL;
    }

    if (p_file_seq->ves_seg_buf) {
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        file_seq_mem_free(p_file_seq, p_file_seq->ves_seg_buf);
#else
        mtos_free(p_file_seq->ves_seg_buf);
#endif
        p_file_seq->ves_seg_buf = NULL;
    }

    if (p_file_seq->isAudMute) {
        aud_mute_onoff_vsb(p_file_seq->p_audio_dev, FALSE);
        p_file_seq->isAudMute = FALSE;
    }

#ifdef  RECORD_CAMERA_VIDEO

    if (p_record_video) {
#ifdef __LINUX__
        fclose(p_record_video);
        FILE_SEQ_DEBUG("[%s] fclose p_record_video\n", __func__);
#else
        ufs_close(p_record_video);
        mtos_free(p_record_video);
        FILE_SEQ_DEBUG("[%s] fclose p_record_video\n", __func__);
#endif
        p_record_video = NULL;
    }

#endif
    handle_exit_fill_es_task();
    p_file_seq->is_task_alive = FALSE;
    FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);
}

#endif

/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
static  void  load_media_task(void * p_param)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_param;
    p_file_seq->is_load_task_alive = TRUE;
    BOOL  load_stat = FALSE;
    int  load_cnt = 2;//peacer modify
    BOOL unloadmedia = FALSE;
    FILE_SEQ_DEBUG("\n @@@@@@@%s %d waiting ~~  %d\n", __func__, __LINE__, p_file_seq->m_play_state);

    while (load_task_state != 1) {
        mtos_task_sleep(100);
    }

#ifdef USE_PB_FIFO
    if(p_file_seq->is_fifo_playback){
	int wait_cnt=0;
	//wait fifo data to threshold
	do{
		if(p_file_seq->p_pb_fifo_handle==NULL){
			mtos_task_sleep(100);
			wait_cnt ++;

			if(wait_cnt > 10){
				//break;
			     p_file_seq->m_play_state  = FILE_SEQ_STOP;
		            FILE_SEQ_DEBUG("[%s]  p_file_seq->p_pb_fifo_handle==NULL, exit!!\n", __func__);
		            p_file_seq->event_cb(FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT, p_file_seq->path_id);
		            p_file_seq->is_load_task_alive = FALSE;
            		     mtos_task_exit();
	        	     return 0;

			}
		}
		else{
		#if 0
			fifo_data_filled = tell_pb_fifo_kw(p_file_seq->p_pb_fifo_handle);
			FILE_SEQ_LOG("[%s] fifo_data_filled: %ld  \n", __func__, fifo_data_filled);
			if(fifo_data_filled > T_PB_FIFO_START){
				FILE_SEQ_LOG("[%s] fifo is filled to threshold(%ld), break to start load media task!  \n", __func__, fifo_data_filled);
				break;
			}
		#else
			if(p_file_seq->pbfifo_buffering_ok){
				FILE_SEQ_LOG("[%s] fifo is buffered ok, break to start load media task!  \n", __func__);
				break;
		}
		#endif
			if (p_file_seq->m_play_state == FILE_SEQ_EXIT) {
		            //p_file_seq->m_play_state = FILE_SEQ_LOADED;
	        	     p_file_seq->m_play_state  = FILE_SEQ_STOP;
		            FILE_SEQ_DEBUG("[%s] %d download too slow, exit! \n", __func__, __LINE__);
		            p_file_seq->event_cb(FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT, p_file_seq->path_id);

			     p_file_seq->is_load_task_alive = FALSE;
            		     mtos_task_exit();
	        	     return 0;
	    }

			mtos_task_sleep(200);
			FILE_SEQ_DEBUG("[%s] %d wait m_play_state: %d\n", __func__, __LINE__,p_file_seq->m_play_state);
		}
		
	}while(1);
}
#endif	
    // yliu add
    if (p_file_seq->loadMedaiOK) {
        x_unLoadMedia(p_file_seq);
    }

    if (p_file_seq->p_sub_fifo_handle) {
		void * p_start_pos = NULL;
		p_start_pos = ((fifo_type_t *)(p_file_seq->p_sub_fifo_handle))->start_pos;
		if(p_start_pos)
		{
			#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
			file_seq_mem_free(p_file_seq, p_start_pos);
			#else
        	mtos_free(p_start_pos);
			#endif
			((fifo_type_t *)(p_file_seq->p_sub_fifo_handle))->start_pos = 0;			
		}		
        deinit_fifo_kw(p_file_seq->p_sub_fifo_handle);
        p_file_seq->p_sub_fifo_handle = NULL;
    }

#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
    file_seq_mem_init(p_file_seq, (u8 *)p_file_seq->file_seq_mem_start, p_file_seq->file_seq_mem_size);
#endif
    FILE_SEQ_DEBUG("\n @@@@@@@%s %d waiting ~[%d]~\n", __func__, __LINE__, p_file_seq->m_play_state);

    if (strstr(p_file_seq->m_path[0], "rtmp://")) {
        load_cnt = 2;
    }

    if (p_file_seq->load_media_times > 0) {
        load_cnt = p_file_seq->load_media_times;
    }

    FILE_SEQ_LOG("[%s]  start start [%d].\n", __func__, __LINE__);

#ifndef CHIP_CONCERTO
#ifdef WITH_TCPIP_PROTOCOL

    if (strstr(p_file_seq->m_path[0], "vdn.live.cntv.cn") || strstr(p_file_seq->m_path[0], "web-play.pptv.com")
        || strstr(p_file_seq->m_path[0], "giniko.com") || strstr(p_file_seq->m_path[0], "malimar.tv") || strstr(p_file_seq->m_path[0], "filmon.com")
        || strstr(p_file_seq->m_path[0], "dailymotion.com/live/video/")
    || strstr(p_file_seq->m_path[0], "youtube.com")) {
        replace_real_play_path(p_file_seq->m_path[0]);
        FILE_SEQ_LOG("[%s]  replace_real_play_path done\n", __func__);
    }


#endif
#endif
	if(fp_is_timeshift_file() == 1)
		load_cnt = 10;
    do {
        load_cnt--;
        FILE_SEQ_DEBUG("\n @@@@@@@%s %d waiting ~~  %d\n", __func__, __LINE__, p_file_seq->m_play_state);

        if (load_cnt < 0 || p_file_seq->m_play_state == FILE_SEQ_EXIT) {
            FILE_SEQ_ERROR("[%s] fail to x_loadMedia ...!!!!!!!!!\n", __func__);
            mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);

            if (p_file_seq->m_play_state == FILE_SEQ_EXIT) {
                p_file_seq->m_play_state = FILE_SEQ_LOADED;
                FILE_SEQ_DEBUG("[%s] %d.\n", __func__, __LINE__);
                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT, p_file_seq->path_id);
            } else {
                p_file_seq->m_play_state = FILE_SEQ_LOADED;
                FILE_SEQ_DEBUG("[%s] %d.\n", __func__, __LINE__);
                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR, p_file_seq->path_id);
            }

            mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
            //
            p_file_seq->is_load_task_alive = FALSE;

            mtos_task_exit();

        }

        FILE_SEQ_ERROR("\n!@@@!!!![%s] %d  load:%d \n", __func__, __LINE__, load_cnt);
        p_file_seq->cur_play_index = 0;
        load_stat =  x_loadMedia(p_file_seq);
    } while (load_stat == FALSE);

    //FILE_SEQ_DEBUG("[%s] test memory alloc\n", __func__);
    if (p_file_seq->p_sub_fifo_handle) {
		void * p_start_pos = NULL;
		p_start_pos = ((fifo_type_t *)(p_file_seq->p_sub_fifo_handle))->start_pos;
		if(p_start_pos)
		{
			#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        	file_seq_mem_free(p_file_seq, p_start_pos);
			#else
        	mtos_free(p_start_pos);
			#endif
			((fifo_type_t *)(p_file_seq->p_sub_fifo_handle))->start_pos = 0;			
		}		
        deinit_fifo_kw(p_file_seq->p_sub_fifo_handle);
        p_file_seq->p_sub_fifo_handle = NULL;
    }
	
    //sunny remove it from x_loadmedia, so that x_loadmedia can reuse in
    // muti-URLs play
    void * p_tmp = NULL;
  
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        p_tmp = file_seq_mem_alloc(p_file_seq, SUB_FIFO_LEN);
#else
        p_tmp = mtos_malloc(SUB_FIFO_LEN);
#endif

        if (p_tmp) {
            memset(p_tmp, 0, SUB_FIFO_LEN);
        }

	p_file_seq->p_sub_fifo_handle = init_fifo_kw(p_tmp, SUB_FIFO_LEN);
    
    if (((demux_stream_t *)(p_file_seq->p_ds_video))->sh == NULL) {
        p_file_seq->only_audio_mode = TRUE;
        FILE_SEQ_ERROR("[%s] p_ds_audio[0x%x] p_ds_video[0x%x] !! \n",
                       __func__, p_file_seq->p_ds_audio, p_file_seq->p_ds_video);
    }

    FILE_SEQ_LOG("[%s] -----out of x_loadMedia TRUE \n", __func__);
    FILE_SEQ_LOG("[%s] --media type [%d] \n", __func__,
                 ((demuxer_t *)(p_file_seq->p_demuxer))->type);

    if (DEMUXER_TYPE_AUDIO == ((demuxer_t *)(p_file_seq->p_demuxer))->type) {
        if (x_check_aud_codec_type(p_file_seq) == FALSE) {
            FILE_SEQ_ERROR("[%s] fail to check_aud_codec_type ...\n", __func__);
            unloadmedia = TRUE;
            g_load_err = ERR_LOAD_AV_UNSUPPORT;
        }
    } else if (DEMUXER_TYPE_RTP == ((demuxer_t *)(p_file_seq->p_demuxer))->type) {
        p_file_seq->rtsp_play_mode = TRUE;
        //p_file_seq->only_audio_mode = FALSE;

        if (x_check_av_codec_type(p_file_seq) == FALSE) {
            FILE_SEQ_ERROR("[%s] fail to check_av_codec_type ...\n", __func__);
            unloadmedia = TRUE;
            g_load_err = ERR_LOAD_AV_UNSUPPORT;
        }
    } else {
        if (x_check_av_codec_type(p_file_seq) == FALSE) {
            FILE_SEQ_ERROR("[%s] fail to check_av_codec_type ...\n", __func__);
            unloadmedia = TRUE;
            g_load_err = ERR_LOAD_AV_UNSUPPORT;
        }
    }

    //
    if (TRUE == unloadmedia) {
        x_unLoadMedia(p_file_seq);
        mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);

        if (p_file_seq->m_play_state == FILE_SEQ_EXIT) {
            p_file_seq->m_play_state = FILE_SEQ_LOADED;
            FILE_SEQ_DEBUG("[%s] %d.\n", __func__, __LINE__);
            p_file_seq->event_cb(FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT, p_file_seq->path_id);
        } else {
            p_file_seq->m_play_state = FILE_SEQ_LOADED;
            FILE_SEQ_DEBUG("[%s] %d.\n", __func__, __LINE__);
            p_file_seq->event_cb(FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR, p_file_seq->path_id);
        }

        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        FILE_SEQ_DEBUG("[%s] %d.\n", __func__, __LINE__);
        p_file_seq->is_load_task_alive = FALSE;

        mtos_task_exit();

    }

    //
    FILE_SEQ_LOG("number is %d\n", p_file_seq->total_path);

    if (p_file_seq->total_path > 1) {
        int j;

        // This is a workround method for duration when in Multi-URLs play
        if (p_file_seq->p_m_duration[0] == 0) {
            for (j = 0; j < p_file_seq->total_path; j++) {
                p_file_seq->p_m_duration[j] = ceil(p_file_seq->duration);
            }

            p_file_seq->file_duration = p_file_seq->file_duration * p_file_seq->total_path;
        } else {
            p_file_seq->file_duration = 0;
            p_file_seq->p_m_duration[0] = ceil(p_file_seq->duration);

            // mtos_printk("\n%s %d  dur :%ddur :%d\n",__func__,__LINE__,(int)p_file_seq->duration,p_file_seq->p_m_duration[0]);
            for (j = 0; j < p_file_seq->total_path; j++) {
                p_file_seq->file_duration += p_file_seq->p_m_duration[j];
            }

            //mtos_printk("\n%s %d  dur :%d\n",__func__,__LINE__,(int)p_file_seq->file_duration);
        }

#if   defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        p_file_seq->p_cur_demuxer = file_seq_mem_alloc(p_file_seq, sizeof(demuxer_t));
        MT_ASSERT(p_file_seq->p_cur_demuxer != NULL);
        p_file_seq->p_cur_stream  = file_seq_mem_alloc(p_file_seq, sizeof(stream_t));
        MT_ASSERT(p_file_seq->p_cur_stream != NULL);
        p_file_seq->p_cur_ds_audio = file_seq_mem_alloc(p_file_seq, sizeof(demux_stream_t));
        MT_ASSERT(p_file_seq->p_cur_ds_audio != NULL);
        p_file_seq->p_cur_ds_video = file_seq_mem_alloc(p_file_seq, sizeof(demux_stream_t));
        MT_ASSERT(p_file_seq->p_cur_ds_video != NULL);
        p_file_seq->p_cur_ds_sub = file_seq_mem_alloc(p_file_seq, sizeof(demux_stream_t));
        MT_ASSERT(p_file_seq->p_cur_ds_sub != NULL);
#else
        p_file_seq->p_cur_demuxer = mtos_malloc(sizeof(demuxer_t));
        MT_ASSERT(p_file_seq->p_cur_demuxer != NULL);
        p_file_seq->p_cur_stream  = mtos_malloc(sizeof(stream_t));
        MT_ASSERT(p_file_seq->p_cur_stream != NULL);
        p_file_seq->p_cur_ds_audio = mtos_malloc(sizeof(demux_stream_t));
        MT_ASSERT(p_file_seq->p_cur_ds_audio != NULL);
        p_file_seq->p_cur_ds_video = mtos_malloc(sizeof(demux_stream_t));
        MT_ASSERT(p_file_seq->p_cur_ds_video != NULL);
        p_file_seq->p_cur_ds_sub = mtos_malloc(sizeof(demux_stream_t));
        MT_ASSERT(p_file_seq->p_cur_ds_sub != NULL);
#endif
        memset(p_file_seq->p_cur_demuxer, 0x00, sizeof(demuxer_t));
        memset(p_file_seq->p_cur_stream, 0x00, sizeof(stream_t));
        memset(p_file_seq->p_cur_ds_audio, 0x00, sizeof(demux_stream_t));
        memset(p_file_seq->p_cur_ds_video, 0x00, sizeof(demux_stream_t));
        memset(p_file_seq->p_cur_ds_sub, 0x00, sizeof(demux_stream_t));
        memcpy(p_file_seq->p_cur_demuxer, p_file_seq->p_demuxer, sizeof(demuxer_t));
        memcpy(p_file_seq->p_cur_stream, p_file_seq->p_stream, sizeof(stream_t));
        memcpy(p_file_seq->p_cur_ds_audio, p_file_seq->p_ds_audio, sizeof(demux_stream_t));
        memcpy(p_file_seq->p_cur_ds_video, p_file_seq->p_ds_video, sizeof(demux_stream_t));
        memcpy(p_file_seq->p_cur_ds_sub, p_file_seq->p_ds_sub, sizeof(demux_stream_t));
        FILE_SEQ_LOG("[%s] multi-url mode!!\n", __func__);
    } else {
        p_file_seq->p_cur_demuxer = p_file_seq->p_demuxer;
        p_file_seq->p_cur_stream = p_file_seq->p_stream;
        p_file_seq->p_cur_ds_audio = p_file_seq->p_ds_audio;
        p_file_seq->p_cur_ds_video = p_file_seq->p_ds_video;
        p_file_seq->p_cur_ds_sub = p_file_seq->p_ds_sub;
        FILE_SEQ_LOG("[%s] single url mode\n", __func__);
    }

    p_file_seq->m_loaded_pts = 0;
    FILE_SEQ_DEBUG("[%s] -----out of x_check_av_codec_type TRUE \n", __func__);
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);

    if (p_file_seq->m_play_state == FILE_SEQ_EXIT) {
        FILE_SEQ_DEBUG("[%s] %d.\n", __func__, __LINE__);
        x_unLoadMedia(p_file_seq);

        if (p_file_seq->total_path > 1) {
            p_file_seq->m_preload_state = FILE_SEQ_EXIT;
        }

        FILE_SEQ_LOG("[%s] %d...\n", __func__, __LINE__);
        FILE_SEQ_LOG("[%s] %d...\n", __func__, __LINE__);
        FILE_SEQ_LOG("[%s] drv send event 'FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT'\n", __func__);
        p_file_seq->m_play_state = FILE_SEQ_LOADED;
        p_file_seq->event_cb(FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT, p_file_seq->path_id);
    } else {
        FILE_SEQ_LOG("[%s] %d...\n", __func__, __LINE__);
        FILE_SEQ_LOG("[%s] %d...\n", __func__, __LINE__);
        p_file_seq->load_media_state = FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS;
        p_file_seq->m_play_state = FILE_SEQ_LOADED;
        FILE_SEQ_LOG("[%s] drv send event 'FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS'\n", __func__);
        p_file_seq->event_cb(FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS, p_file_seq->path_id);
    }

    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    p_file_seq->is_load_task_alive = FALSE;
    FILE_SEQ_LOG("[%s]  end end [%d].\n", __func__, __LINE__);

    mtos_task_exit();

}



/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
static  void  fill_aud_es_task(void * p_param)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_param;
    MT_ASSERT(p_file_seq != NULL);
    unsigned char * start_a = NULL;
    uint8_t * p_extra_aud_buf = NULL;
    uint8_t   extra_audio_size = 0;
    int cur_audio_packet_size = 0;
    int tmp_seg_aud_size = 0; //practically audio segment size
    unsigned int audio_seg_packet_len = 0;
    unsigned int audFreeSpaceSize = 0;
    unsigned int audTotalSpaceSize = 0;
    unsigned int long  apts = 0;
    unsigned int orig_apts = 0;
    unsigned char * tmp_aud_buf = NULL;
    demux_stream_t * ds_a = NULL;
    demux_stream_t * ds_v = NULL;
    int loop_count = 0;
    double read_pkt_size = 0;
    switch_mode_flag = 0;
    first_vpts = -2;
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);
    p_file_seq->is_task_alive = TRUE;
    p_file_seq->needNewVideoData = TRUE;
    p_file_seq->needNewAudioData = TRUE;
    p_file_seq->isTrickToNormal = FALSE;
    p_file_seq->isNormalPlay = TRUE;
    p_file_seq->isTrickPlay = FALSE;
    p_file_seq->ref_first_audio_pts = 0;
    p_file_seq->ref_first_video_pts = 0;
    p_file_seq->isAudMute = FALSE;
    p_file_seq->is_play_to_end = 0;
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
    tmp_aud_buf = p_file_seq->mem_alloc(p_file_seq, AUD_ASSEMBLE_BUF_LEN);
#else
    tmp_aud_buf = mtos_malloc(AUD_ASSEMBLE_BUF_LEN);
#endif

    if (tmp_aud_buf) {
        memset(tmp_aud_buf, 0, AUD_ASSEMBLE_BUF_LEN);
    } else {
        FILE_SEQ_DEBUG("[%s][ERROR] fail to malloc AUDIO tmp buffer!!!\n", __func__);
        return;
    }

    cur_audio_packet_size = 0;
    ds_a = p_file_seq->p_ds_audio;
    ds_v = p_file_seq->p_ds_video;
    p_file_seq->m_play_state = FILE_SEQ_PLAY;
    aud_file_getleftesbuffer_vsb(p_file_seq->p_audio_dev, (u32 *)&audFreeSpaceSize);
    FILE_SEQ_DEBUG("audFreeSpaceSize[%d]\n", audFreeSpaceSize);

    if (p_file_seq->start_seconds > 0) {
        demux_seek(p_file_seq->p_demuxer, p_file_seq->start_seconds, 0, 0) ;
    }

    FILE_SEQ_DEBUG("start main loop !!!!!!!!!!!\n");

    while (1) {
        do_user_cmd();

        if (p_file_seq->m_play_state == FILE_SEQ_EXIT) {
            break;
        } else if (p_file_seq->m_play_state == FILE_SEQ_PAUSE) {
            mtos_task_sleep(30);
            continue;
        }

        if (loop_count++ > 32) {
            loop_count = 0;
            mtos_task_sleep(20); //for lower prio task
        }

        if (p_file_seq->is_play_to_end == 1) {
            break;
        }

        if (check_task_finish(p_file_seq)) {
            break;
        }

        if (p_file_seq->isAudioEsEnd == FALSE) {
            /* get audio frame through ffmpeg demuxer */
            if (p_file_seq->needNewAudioData) {
                if (!ds_a->eof) {
                    if (p_file_seq->isTrickPlay) {
                        while ((!ds_a->eof)  && ((ds_v->pts - ds_a->pts) > 0.0)) {
                            ds_get_packet(ds_a, &start_a);
                        }
                    }

                    if (p_file_seq->isTrickToNormal) {
                        while ((!ds_a->eof)  && ((ds_v->pts - ds_a->pts) > 0.0)) {
                            ds_get_packet(ds_a, &start_a);
                        }
                    }

                    if (ds_a->eof) {
                        FILE_SEQ_DEBUG("break for audio stream is finished !!!\n");
                        break;
                    }

                    extra_audio_size = 0;
                    p_extra_aud_buf = NULL;
                    cur_audio_packet_size = ds_get_packet_audio(ds_a, &start_a, &p_extra_aud_buf, (uint8_t **)&extra_audio_size);

                    if (cur_audio_packet_size <= 0) {
                        FILE_SEQ_DEBUG("[%s][WARNING] audio packet size [%d]!!!!\n", __func__, cur_audio_packet_size);
                        continue;
                    }
                    else
                    		read_pkt_size += cur_audio_packet_size;
                } else {
                    FILE_SEQ_LOG("[%s][ok]  >>>> Audio Stream Demuxer Is Finished !!<<<<\n", __func__);
                    p_file_seq->isAudioEsEnd = TRUE;
                    continue;
                }

                if (ds_a->pts < 0.0) {
                    ds_a->pts = 0.0;
                }

                if ((first_vpts) < -1) {
                    first_vpts = ds_a->pts;
                }

                p_file_seq->orig_apts =  ds_a->pts * 1000.0;
		  if(p_file_seq->orig_apts ==0 && p_file_seq->audio_bps){
		  	p_file_seq->orig_apts = read_pkt_size*1000/p_file_seq->audio_bps;//ms

		  }

                
               // FILE_SEQ_DEBUG("[%s]----p_file_seq->orig_apts [%d], pkt size: %d!!!!\n", __func__, (int)(p_file_seq->orig_apts*1000), cur_audio_packet_size);
                p_file_seq->sys_apts = (u32)(orig_apts * PTS_BASE) | VALID_PTS_MASK;

                if (p_file_seq->orig_apts > p_file_seq->max_audio_pts) {
                    p_file_seq->max_audio_pts = p_file_seq->orig_apts;

                    if (p_file_seq->isTrickToNormal) {
                        p_file_seq->ref_first_audio_pts = p_file_seq->max_audio_pts;
                        p_file_seq->isTrickToNormal = FALSE;
                    }
                }

                /*normal play*/
                if (p_file_seq->isNormalPlay) {
                    p_file_seq->needNewAudioData = FALSE;
                } else {
                    p_file_seq->needNewAudioData = TRUE;

                    if (p_extra_aud_buf && extra_audio_size) {
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
                        p_file_seq->mem_free(p_file_seq, p_extra_aud_buf);
#else
                        mtos_free(p_extra_aud_buf);
#endif
                        p_extra_aud_buf = NULL;
                        p_file_seq->totalAesNum += (cur_audio_packet_size + extra_audio_size);
                    }

                    p_file_seq->isAudioBufferFull = FALSE;
                    continue;
                }
            }

            /*push aes to dma ring buffer */
            if (p_file_seq->isNormalPlay) {
                aud_file_getleftesbuffer_vsb(p_file_seq->p_audio_dev, (u32 *)&audFreeSpaceSize);
                aud_file_gettotalesbuffer_vsb(p_file_seq->p_audio_dev, (u32 *)&audTotalSpaceSize);

                //mtos_printk("[%s] %d audFreeSpaceSize: %d cur_audio_packet_size: %d extra_audio_size:%d \n",__func__,__LINE__,audFreeSpaceSize,cur_audio_packet_size,extra_audio_size);
                if (audFreeSpaceSize > audTotalSpaceSize) {
                    FILE_SEQ_ERROR("\n[%s]:%d Error ! audio ES buffer free size > Total size !! \n\n", __func__, __LINE__);
                    break;
                } else if (0 == audFreeSpaceSize) {
                    p_file_seq->isAudioBufferFull = TRUE;
                    FILE_SEQ_LOG("\n[%s]:%d Audio ES buffer is FULL!! \n\n", __func__, __LINE__);
                } else if (AUDIO_PCM == p_file_seq->m_audio_codec_type) {
                    if (audFreeSpaceSize > AUDIO_ES_BUF_OVERFLOW_THRESHOLD) {
                        if (cur_audio_packet_size <= SEG_AUD_SIZE) {
                            tmp_seg_aud_size = cur_audio_packet_size;
                        } else {
                            tmp_seg_aud_size = MIN((audFreeSpaceSize - AUDIO_ES_BUF_OVERFLOW_THRESHOLD), SEG_AUD_SIZE);
                        }

                        //mtos_printk("[%s]:%d audFreeSpaceSize = %d, tmp_seg_aud_size= %d audio_seg_packet_len = %d\n ",__func__,__LINE__,audFreeSpaceSize,tmp_seg_aud_size,audio_seg_packet_len);
                        memcpy(tmp_aud_buf, (void *)start_a + audio_seg_packet_len, tmp_seg_aud_size);
                        adec_push_audio_es(tmp_aud_buf, tmp_seg_aud_size, apts);
                        p_file_seq->totalAesNum += tmp_seg_aud_size;
                        audFreeSpaceSize -= tmp_seg_aud_size;
                        p_file_seq->available_aes_bytes = p_file_seq->dec_cap.max_aes_num - audFreeSpaceSize;
                        cur_audio_packet_size -= tmp_seg_aud_size;

                        //mtos_printk("[%s]:%d cur_audio_packet_size= %d\n ",__func__,__LINE__,cur_audio_packet_size);
                        if (cur_audio_packet_size > 0) {
                            audio_seg_packet_len += tmp_seg_aud_size;
                            p_file_seq->needNewAudioData = FALSE;
                        } else {
                            audio_seg_packet_len = 0;
                            p_file_seq->needNewAudioData = TRUE;
                        }

                        if (audFreeSpaceSize <= 0) {
                            p_file_seq->isAudioBufferFull = TRUE;
                        } else {
                            p_file_seq->isAudioBufferFull = FALSE;
                        }
                    }
                } else {
                    //aud_file_getleftesbuffer_vsb(p_file_seq->p_audio_dev, (u32 *)&audFreeSpaceSize);
                    if (audFreeSpaceSize >= (cur_audio_packet_size + extra_audio_size) + AUDIO_ES_BUF_OVERFLOW_THRESHOLD) {
                        if (p_extra_aud_buf && extra_audio_size) {
                            memcpy(tmp_aud_buf, p_extra_aud_buf, extra_audio_size);
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
                            p_file_seq->mem_free(p_file_seq, p_extra_aud_buf);
#else
                            mtos_free(p_extra_aud_buf);
#endif
                            p_extra_aud_buf = NULL;
                        }

                        memcpy(tmp_aud_buf + extra_audio_size, (void *)start_a, cur_audio_packet_size);
                        adec_push_audio_es(tmp_aud_buf, cur_audio_packet_size + extra_audio_size, apts);
                        p_file_seq->needNewAudioData = TRUE;
                        p_file_seq->totalAesNum += cur_audio_packet_size;
                        p_file_seq->isAudioBufferFull = FALSE;
                        audFreeSpaceSize -= (cur_audio_packet_size + extra_audio_size);
                        p_file_seq->available_aes_bytes = p_file_seq->dec_cap.max_aes_num - audFreeSpaceSize;
                    } else {
                        p_file_seq->isAudioBufferFull = TRUE;
                    }
                }
            }
        }

        handle_pending_event();
#ifdef AUTO_SEND_VPTS
        SetFlag(p_file_seq->internal_event, AUTO_DELIVER_VPTS);
#endif
    }

    FILE_SEQ_DEBUG("[%s] end  main loop !!!!!!!!!!!\n", __func__);
    FILE_SEQ_DEBUG("[%s] ds_a->eof[%d] \n", __func__, ds_a->eof);
    FILE_SEQ_DEBUG("[%s] totalAesNum[%d] \n", __func__, p_file_seq->totalAesNum);
    FILE_SEQ_DEBUG("[%s] exit from fill es task !!!!!!!!!!!\n", __func__);
EXIT_FILL_ES_TASK:

    //#ifndef SWITCH_DEBUG
    if (p_file_seq->ott_playmode != OTT_LIVE_MODE) {
        if (tmp_aud_buf) {
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
            p_file_seq->mem_free(p_file_seq, tmp_aud_buf);
#else
            mtos_free(tmp_aud_buf);
#endif
            tmp_aud_buf = NULL;
        }
    }

    //#endif

    if (p_file_seq->isAudMute) {
        aud_mute_onoff_vsb(p_file_seq->p_audio_dev, FALSE);
        p_file_seq->isAudMute = FALSE;
    }

    handle_exit_fill_es_task();
    FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);
}






/*********************************************************************************
 ***********************************************************************************
 *
 *
 *
 *
 *
 *
 *
 *                         The fowwing APIs will be exposed  to upper layer code
 *
 *                                                          peacer 2013-02-07
 *
 *
 *
 *
 *
 *
 *
 *
 ****************************************************************************************
 ********************************************************************************************
 */

static int detect_meminfo()
{
#if 1
    mem_user_dbg_info_t dbg_info_old;
    memset((void *)(&dbg_info_old), 0x00, sizeof(dbg_info_old));
    mtos_mem_user_debug(&dbg_info_old);
    mtos_printk("memory rest is %d\n", dbg_info_old.rest_size);
#endif
    return 0;
}
static int file_seq_loadmedia_task(void * p_handle)
{
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    BOOL ret = FALSE;
    BOOL load_in_task = FALSE;
    long fifo_data_filled = 0;
    int wait_cnt =0;
     detect_meminfo();
    FILE_SEQ_DEBUG("[%s] m_play_state: %d\n", __func__,p_file_seq->m_play_state);
    if ((p_file_seq->m_play_state == FILE_SEQ_LOADMEDIA)) {
        FILE_SEQ_ERROR("[%s][ERROR] do nothing\n", __func__);
        FILE_SEQ_ERROR("[%s]m_play_state[%d]\n", __func__, p_file_seq->m_play_state);
        FILE_SEQ_ERROR("[%s]is_task_alive[%d]\n", __func__, p_file_seq->is_load_task_alive);
        mtos_task_sleep(300);
        mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
        g_load_err = ERR_LOAD_LAST_LOAD_RUNNING;
        p_file_seq->event_cb(FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR, p_file_seq->path_id);
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return 0;
    } else {
        while (p_file_seq->is_load_task_alive || ((p_file_seq->net_task_prio == p_file_seq->task_prio) && (p_file_seq->is_task_alive))) {
            FILE_SEQ_ERROR("[%s][ERROR] do nothing\n", __func__);
            FILE_SEQ_ERROR("[%s]m_play_state[%d]\n", __func__, p_file_seq->m_play_state);
            FILE_SEQ_ERROR("[%s]is_task_alive[%d]\n", __func__, p_file_seq->is_load_task_alive);
            mtos_task_sleep(300);
        }
    }

    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);

    if (p_file_seq->m_path[0] == NULL) {
        mtos_task_sleep(300);
         g_load_err = ERR_LOAD_NAME_NULL;
        p_file_seq->event_cb(FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR, p_file_seq->path_id);
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return 0;
    }

    if (strncmp(p_file_seq->m_path[0], "camera://", 9) == 0 ||
    strstr(p_file_seq->m_path[0], RECORD_FILE_TAIL) != NULL) {
        p_file_seq->is_task_alive = FALSE;
        p_file_seq->m_play_state = FILE_SEQ_LOADED;
        p_file_seq->loadMedaiOK = TRUE;
        FILE_SEQ_DEBUG("[%s] %d.\n", __func__, __LINE__);
        p_file_seq->event_cb(FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS, p_file_seq->path_id);
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return 0;
    }

    BUFFERING_PARAM_T  *  p_buffering_param = NULL;
    p_buffering_param = p_file_seq->p_buffering;
    p_buffering_param->old_tick = 0;
    p_buffering_param->start_tick = 0;
    p_buffering_param->start_show_loadmedia_time = FALSE;
    p_buffering_param->start_show_buffering_in_playstate = FALSE;
    p_buffering_param->cur_ves_num = 0;
    p_buffering_param->old_ves_num = 0;
    p_buffering_param->start_buffering_wl = 0;
    p_buffering_param->end_buffering_wl = 0;
    p_buffering_param->cur_ves_num = 0;
    p_buffering_param->old_ves_num = 0;
    p_buffering_param->start_buffering_wl = 0;
    p_buffering_param->end_buffering_wl = 0;

    if ((demux_state == 1)) {
        FILE_SEQ_ERROR("[%s][ERROR] fail to loadmedia_task !!!\n", __func__);
        FILE_SEQ_ERROR("[%s][ERROR] m_play_state[%d] !!!\n", __func__, p_file_seq->m_play_state);

        if ((demux_state == 1) && (p_file_seq->m_play_state  != FILE_SEQ_EXIT)) {
            p_file_seq->m_play_state  = FILE_SEQ_EXIT;
            self_exit = 1;
        }

        //yliu add
        mtos_task_sleep(200);
        p_file_seq->event_cb(FILE_PLAYBACK_SEQ_NOT_READY, p_file_seq->path_id);
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return -1;
    }

	//p_file_seq->m_play_state = FILE_SEQ_PB_FIFO_BUFFERING;
	// p_file_seq->m_play_state = FILE_SEQ_LOADMEDIA;
#ifdef USE_PB_FIFO

  if(strncmp("fifo:http", p_file_seq->m_path[0], 9) == 0){
  		FILE_SEQ_LOG("[%s] is fifo:http path: %s\n", __func__, p_file_seq->m_path[0]);
  		p_file_seq->is_fifo_playback = TRUE;
		
#if 0//def __LINUX__
        mtos_task_create((u8 *)"pbfifo thread", playback_fifo_thread_lite, p_file_seq, 0, 0, 0);
#endif
	 //p_file_seq->task_pb_fifo_prio = PLAYBACK_FIFO_THREAD_PRIORITY;
	 //p_file_seq->task_pb_fifo_size = 32*1024;
	 //p_file_seq->p_pbfifostack = mtos_malloc(p_file_seq->task_pb_fifo_size);
        OS_PRINTF("%d %x %d\n", p_file_seq->task_pb_fifo_prio,
                  p_file_seq->p_pbfifostack, p_file_seq->task_pb_fifo_size);

        while (p_file_seq->is_pbfifo_task_alive) {
            FILE_SEQ_DEBUG("[%s]wait  playback_fifo_thread_lite task exit\n", __func__);
            mtos_task_sleep(100);
        }

        ret = mtos_task_delete(p_file_seq->task_pb_fifo_prio);
        if (ret) {
	        FILE_SEQ_LOG("[%s]delete failed =%d \n", __func__, ret);
	        FILE_SEQ_LOG("[%s]task_prio[%d] \n", __func__, p_file_seq->task_pb_fifo_prio);
    	}


	pbfifo_task_state = 0;
        ret = mtos_task_create((u8 *)"pbfifo thread", playback_fifo_thread_lite, p_file_seq,
                         p_file_seq->task_pb_fifo_prio, p_file_seq->p_pbfifostack,
                         p_file_seq->task_pb_fifo_size);


        while (p_file_seq->is_pbfifo_task_alive != TRUE) {
            mtos_task_sleep(100);
            FILE_SEQ_LOG("\n%s %d wait playback_fifo_thread start\n", __func__, __LINE__);
        }
        pbfifo_task_state = 1;
	FILE_SEQ_LOG("[%s] playback_fifo_thread start ok!  \n", __func__, ret);

    }
    else{
    	p_file_seq->is_fifo_playback = FALSE;
    	FILE_SEQ_LOG("[%s] not fifo path:   %s\n", __func__, p_file_seq->m_path[0]);
    }	
#endif

    p_file_seq->m_play_state = FILE_SEQ_LOADMEDIA;
     g_load_err = ERR_LOAD_UNKNOW;
    #if 1
   
    //load_in_taskload_in_task = register_http_stream_is();
    load_in_task = 1;
    FILE_SEQ_LOG("\n!!!@@@%s %d load:%d\n", __func__, __LINE__, load_in_task);

    if (load_in_task == 1) {

        //yliu :make sure task deleted befor create ,modifyed to delete by task_flag in future
        ret = mtos_task_delete(p_file_seq->net_task_prio);


        if (ret) {
            FILE_SEQ_ERROR("delete failed =%d \n", ret);
        }

        load_task_state = 0;
        p_file_seq->load_media_times = 0;
        ret = mtos_task_create((u8 *)"pulling thread", load_media_task, p_handle,
                               p_file_seq->net_task_prio, p_file_seq->p_loadstack, p_file_seq->task_size);
        FILE_SEQ_DEBUG("\n @@@@@@@%s %d waiting ~~\n", __func__, __LINE__);

        while (p_file_seq->is_load_task_alive == FALSE) {
            FILE_SEQ_DEBUG("\n %s %d waiting ~~\n", __func__, __LINE__);
            mtos_task_sleep(100);
        }

        load_task_state = 1;
        FILE_SEQ_DEBUG("\n @@@@@@@%s %d waiting ~~\n", __func__, __LINE__);
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        FILE_SEQ_LOG("\n %s %d task begin\n", __func__, __LINE__);
        return 0;
    }
#endif
    return 0;
}
/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
static int file_seq_loadmedia(void * p_handle)
{
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);
    FILE_SEQ_LOG("[%s] ~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~...\n", __func__);
    FILE_SEQ_LOG("[%s] ~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~...\n", __func__);
    FILE_SEQ_LOG("[%s] ~~~~please call loadmedia_task ...!!!!!!!!!!!!!\n", __func__);
    FILE_SEQ_LOG("[%s] ~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~...\n", __func__);
    FILE_SEQ_LOG("[%s] ~~~~~~~~~~~~~~~~~ ~~~~~~~~~~~~...\n", __func__);
#ifdef __LINUX__
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);
    BOOL ret = FALSE;
    BOOL  load_stat = FALSE;
    int  load_cnt = 4;
    BOOL load_in_task = FALSE;
    int bak_status;
    bak_status = p_file_seq->m_play_state;
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    p_file_seq->m_play_state =  FILE_SEQ_LOADMEDIA;
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
    file_seq_mem_init(p_file_seq, p_file_seq->file_seq_mem_start, p_file_seq->file_seq_mem_size);
#endif
    load_in_task = register_http_stream_is();
    OS_PRINTF("\n!!!@@@%s %d load:%d\n", __func__, __LINE__, load_in_task);

    if (strncmp(p_file_seq->m_path[0], "rtmp://", 7) == 0) {
        load_cnt = 1;
        OS_PRINTF("\n rtmp play! load media once! \n", __func__);
    }

    do {
        load_cnt--;

        if (load_cnt < 0) {
            FILE_SEQ_ERROR("[%s] fail to x_loadMedia ...!!!!!!!!!\n", __func__);
            p_file_seq->m_play_state = bak_status;
            mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
            return -1;
        }

        FILE_SEQ_ERROR("\n!@@@!!!![%s] %d  load:%d \n", __func__, __LINE__, load_cnt);
        p_file_seq->cur_play_index = 0;
        load_stat =  x_loadMedia(p_file_seq);
    } while (load_stat == FALSE);

    /*
       if (x_loadMedia(p_file_seq) == FALSE) {
       FILE_SEQ_ERROR("[%s] fail to x_loadMedia ...!!!!!!!!!\n", __func__);
       mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
       return -1;
       }
     */

    FILE_SEQ_DEBUG("[%s] test memory alloc\n", __func__);

    if (p_file_seq->p_sub_fifo_handle) {
		void * p_start_pos = NULL;
		p_start_pos = ((fifo_type_t *)(p_file_seq->p_sub_fifo_handle))->start_pos;
		if(p_start_pos)
		{
			#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        	file_seq_mem_free(p_file_seq, p_start_pos);
			#else
        	mtos_free(p_start_pos);
			#endif
			((fifo_type_t *)(p_file_seq->p_sub_fifo_handle))->start_pos = 0;			
		}			
        deinit_fifo_kw(p_file_seq->p_sub_fifo_handle);
        p_file_seq->p_sub_fifo_handle = NULL;
    }
	
    //sunny remove it from x_loadmedia, so that x_loadmedia can reuse in
    // muti-URLs play
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
    void * p_tmp = file_seq_mem_alloc(p_file_seq, SUB_FIFO_LEN);
#else
    void * p_tmp = mtos_malloc(SUB_FIFO_LEN);
#endif

    if (p_tmp) {
        memset(p_tmp, 0, SUB_FIFO_LEN);
    }

    p_file_seq->p_sub_fifo_handle = init_fifo_kw(p_tmp, SUB_FIFO_LEN);

    if (((demux_stream_t *)(p_file_seq->p_ds_video))->id  < 0
    || ((demux_stream_t *)(p_file_seq->p_ds_video))->sh == NULL) {
        p_file_seq->only_audio_mode = TRUE;
        FILE_SEQ_LOG("[%s] p_ds_audio[0x%x] p_ds_video[0x%x] !! \n",
                     __func__, p_file_seq->p_ds_audio, p_file_seq->p_ds_video);
    }

    FILE_SEQ_DEBUG("[%s] -----out of x_loadMedia TRUE \n", __func__);
    FILE_SEQ_DEBUG("[%s] --media type [%d] \n", __func__,
                   ((demuxer_t *)(p_file_seq->p_demuxer))->type);

    if (DEMUXER_TYPE_AUDIO == ((demuxer_t *)(p_file_seq->p_demuxer))->type) {
        if (x_check_aud_codec_type(p_file_seq) == FALSE) {
            FILE_SEQ_ERROR("[%s] fail to check_aud_codec_type ...\n", __func__);
            x_unLoadMedia(p_file_seq);
            p_file_seq->m_play_state = bak_status;
            mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
            return -2;
        }
    } else if (DEMUXER_TYPE_RTP == ((demuxer_t *)(p_file_seq->p_demuxer))->type) {
        p_file_seq->rtsp_play_mode = TRUE;
        p_file_seq->only_audio_mode = FALSE;

        if (x_check_av_codec_type(p_file_seq) == FALSE) {
            FILE_SEQ_ERROR("[%s] fail to check_av_codec_type ...\n", __func__);
            x_unLoadMedia(p_file_seq);
            p_file_seq->m_play_state = bak_status;
            mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
            return -2;
        }
    } else {
        if (x_check_av_codec_type(p_file_seq) == FALSE) {
            FILE_SEQ_ERROR("[%s] fail to check_av_codec_type ...\n", __func__);
            x_unLoadMedia(p_file_seq);
            p_file_seq->m_play_state = bak_status;
            mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
            return -2;
        }
    }

    OS_PRINTF("number is %d\n", p_file_seq->total_path);

    if (p_file_seq->total_path > 1) {
        // This is a workround method for duration when in Multi-URLs play
        p_file_seq->file_duration = p_file_seq->file_duration * p_file_seq->total_path;
#if   defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        p_file_seq->p_cur_demuxer = file_seq_mem_alloc(p_file_seq, sizeof(demuxer_t));
        MT_ASSERT(p_file_seq->p_cur_demuxer != NULL);
        p_file_seq->p_cur_stream  = file_seq_mem_alloc(p_file_seq, sizeof(stream_t));
        MT_ASSERT(p_file_seq->p_cur_stream != NULL);
        p_file_seq->p_cur_ds_audio = file_seq_mem_alloc(p_file_seq, sizeof(demux_stream_t));
        MT_ASSERT(p_file_seq->p_cur_ds_audio != NULL);
        p_file_seq->p_cur_ds_video = file_seq_mem_alloc(p_file_seq, sizeof(demux_stream_t));
        MT_ASSERT(p_file_seq->p_cur_ds_video != NULL);
        p_file_seq->p_cur_ds_sub = file_seq_mem_alloc(p_file_seq, sizeof(demux_stream_t));
        MT_ASSERT(p_file_seq->p_cur_ds_sub != NULL);
#else
        p_file_seq->p_cur_demuxer = mtos_malloc(sizeof(demuxer_t));
        MT_ASSERT(p_file_seq->p_cur_demuxer != NULL);
        p_file_seq->p_cur_stream  = mtos_malloc(sizeof(stream_t));
        MT_ASSERT(p_file_seq->p_cur_stream != NULL);
        p_file_seq->p_cur_ds_audio = mtos_malloc(sizeof(demux_stream_t));
        MT_ASSERT(p_file_seq->p_cur_ds_audio != NULL);
        p_file_seq->p_cur_ds_video = mtos_malloc(sizeof(demux_stream_t));
        MT_ASSERT(p_file_seq->p_cur_ds_video != NULL);
        p_file_seq->p_cur_ds_sub = mtos_malloc(sizeof(demux_stream_t));
        MT_ASSERT(p_file_seq->p_cur_ds_sub != NULL);
#endif
        memset(p_file_seq->p_cur_demuxer, 0x00, sizeof(demuxer_t));
        memset(p_file_seq->p_cur_stream, 0x00, sizeof(stream_t));
        memset(p_file_seq->p_cur_ds_audio, 0x00, sizeof(demux_stream_t));
        memset(p_file_seq->p_cur_ds_video, 0x00, sizeof(demux_stream_t));
        memset(p_file_seq->p_cur_ds_sub, 0x00, sizeof(demux_stream_t));
        memcpy(p_file_seq->p_cur_demuxer, p_file_seq->p_demuxer, sizeof(demuxer_t));
        memcpy(p_file_seq->p_cur_stream, p_file_seq->p_stream, sizeof(stream_t));
        memcpy(p_file_seq->p_cur_ds_audio, p_file_seq->p_ds_audio, sizeof(demux_stream_t));
        memcpy(p_file_seq->p_cur_ds_video, p_file_seq->p_ds_video, sizeof(demux_stream_t));
        memcpy(p_file_seq->p_cur_ds_sub, p_file_seq->p_ds_sub, sizeof(demux_stream_t));
        OS_PRINTF("new way@@@@@@@@@@@@@@@@@\n");
        p_file_seq->m_preload_state = FILE_SEQ_PLAY;
        p_file_seq->preload_audio_buffer.read_pos = 0;
        p_file_seq->preload_audio_buffer.write_pos = 0;
        p_file_seq->preload_video_buffer.read_pos = 0;
        p_file_seq->preload_video_buffer.write_pos = 0;
        p_file_seq->p_video_buf = p_file_seq->p_video_buf_tail = NULL;
        p_file_seq->p_audio_buf = p_file_seq->p_audio_buf_tail = NULL;
#ifdef __LINUX__
        mtos_task_create((u8 *)"preload thread", mutipath_preload_thread, p_handle, 0, 0, 0);
#else
        OS_PRINTF("%d %x %d\n", p_file_seq->task_preload_prio,
                  p_file_seq->p_preloadstack, p_file_seq->task_preload_size);

        while (p_file_seq->is_preload_task_alive) {
            mtos_task_sleep(100);
        }

        mtos_task_delete(p_file_seq->task_preload_prio);
        mtos_task_create((u8 *)"preload thread", mutipath_preload_thread, p_handle,
                         p_file_seq->task_preload_prio, p_file_seq->p_preloadstack,
                         p_file_seq->task_preload_size);
#endif

        while (p_file_seq->is_preload_task_alive != TRUE) {
            mtos_task_sleep(100);
            mtos_printk("\n%s %d\n", __func__, __LINE__);
        }
    } else {
        p_file_seq->p_cur_demuxer = p_file_seq->p_demuxer;
        p_file_seq->p_cur_stream = p_file_seq->p_stream;
        p_file_seq->p_cur_ds_audio = p_file_seq->p_ds_audio;
        p_file_seq->p_cur_ds_video = p_file_seq->p_ds_video;
        p_file_seq->p_cur_ds_sub = p_file_seq->p_ds_sub;
        OS_PRINTF("old way@@@@@@@@@@@@@@@@@\n");
    }

    FILE_SEQ_DEBUG("[%s] -----out of x_check_av_codec_type TRUE \n", __func__);
    FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);
    p_file_seq->m_play_state = bak_status;
    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
#endif
    return 0;
}



/*
 *
 *
 *
 *
 *
 *
 *
 *
 */



/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static int is_sort_init = 0;
                          static   LIVE_TV_CLIENT_SORT_T * p_sort_test = NULL ;
                                  extern  int  av_decoder_init(void * p_handle);
                                  static  int file_seq_start(void * p_handle, unsigned int start_seconds)
{
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    int size     = 0;
    int align    = 0;
    int ret       = 0;
    void (* p_fill_es_task)(void * p_param) = NULL;
#ifdef CLIENT_SORT_TEST
    {
        if (is_sort_init == 0) {
            p_sort_test = LiveTV_Client_Sort_Create();
            is_sort_init = 1;
        }
    }
#endif

    if ((p_file_seq->m_play_state  != FILE_SEQ_LOADED) || p_file_seq->loadMedaiOK == FALSE) {
        FILE_SEQ_ERROR("[%s][ERROR] do nothing\n", __func__);
        FILE_SEQ_ERROR("[%s][ERROR] is_task_alive[%d]  \
    				loadMedaiOK[%d]  \
    				state[%d] \n",
                       __func__,
                       p_file_seq->is_task_alive,
                       p_file_seq->loadMedaiOK,
                       p_file_seq->m_play_state);
        mtos_task_sleep(300);
        return -1;
    } else {
        while (p_file_seq->is_task_alive) {
            FILE_SEQ_ERROR("[%s][ERROR] do nothing\n", __func__);
            FILE_SEQ_ERROR("[%s]m_play_state[%d]\n", __func__, p_file_seq->m_play_state);
            FILE_SEQ_ERROR("[%s]is_task_alive[%d]\n", __func__, p_file_seq->is_task_alive);
            mtos_task_sleep(300);
        }

        while (p_file_seq->net_task_prio == p_file_seq->task_prio && p_file_seq->is_load_task_alive) {
            FILE_SEQ_ERROR("[%s][ERROR] do nothing\n", __func__);
            FILE_SEQ_ERROR("[%s]m_play_state[%d]\n", __func__, p_file_seq->m_play_state);
            FILE_SEQ_ERROR("[%s]is_task_alive[%d]\n", __func__, p_file_seq->is_task_alive);
            mtos_task_sleep(300);
        }
    }

    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    // if(p_file_seq->total_path>1)
    p_file_seq->seek_seconds = -1;
    p_file_seq->m_seek_adj = -1;
    p_file_seq->m_seek_cnt = -1;
    p_file_seq->start_seconds = start_seconds;
    x_reset_cmd_fifo(p_file_seq);
#ifdef ENABLE_DEMUX_RTSP

    if (strncmp(p_file_seq->m_path[0], "rtsp://", 7) == 0) {
        p_fill_es_task = rtsp_fill_es_task;
        av_decoder_init(p_file_seq);
    } else
#endif
    {
        p_fill_es_task = fill_es_task;
    }

    if (((demuxer_t *)(p_file_seq->p_demuxer)) && (DEMUXER_TYPE_AUDIO == ((demuxer_t *)(p_file_seq->p_demuxer))->type)
    || (p_file_seq->only_audio_mode && (DEMUXER_TYPE_RTP !=((demuxer_t *)(p_file_seq->p_demuxer))->type))) {
            FILE_SEQ_LOG("[%s] this movie only include audio es !!!! type: %d\n, only_audio_mode:%d\n", __func__, ((demuxer_t *)(p_file_seq->p_demuxer))->type, p_file_seq->only_audio_mode);
        p_fill_es_task = fill_aud_es_task;
        av_decoder_init(p_file_seq);
    }

    //

    if (p_file_seq->m_play_state == FILE_SEQ_PLAY
        || p_file_seq->is_task_alive
    || p_file_seq->loadMedaiOK == FALSE) {
        FILE_SEQ_ERROR("[%s][ERROR] file seq state is not stop !!!!\n", __func__);
        FILE_SEQ_ERROR("[%s][ERROR] is_task_alive[%d]  \
		loadMedaiOK[%d]  \
    				state[%d] \n",
                       __func__,
                       p_file_seq->is_task_alive,
                       p_file_seq->loadMedaiOK,
                       p_file_seq->m_play_state);
        mtos_task_sleep(300);
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return -1;
    }

    //
    if (strncmp(p_file_seq->m_path[0], "camera://", 9) != 0 &&
    strstr(p_file_seq->m_path[0], RECORD_FILE_TAIL) == NULL) {
        //av_decoder_init(p_file_seq);
    } else { //usb camera
        p_fill_es_task = fill_camera_es_task;

        if (p_file_seq->init_av_dev == FALSE) {
            p_file_seq->init_av_dev = TRUE;
            x_init_av_device(p_file_seq);
        }
        vdec_stop(p_file_seq->p_vdec_dev);
        //  ret = x_map_es_buffer(p_file_seq);
        vdec_set_avsync_mode(p_file_seq->p_vdec_dev, VDEC_AVSYNC_FILEPLAY);

        if (p_file_seq->vdec_policy == 0) {
            vdec_get_buf_requirement(p_file_seq->p_vdec_dev, VDEC_BUFFER_AD, (u32 *)&size, (u32 *)&align);
        } else {
            vdec_get_buf_requirement(p_file_seq->p_vdec_dev, p_file_seq->vdec_policy, (u32 *)&size, (u32 *)&align);
        }

        MT_ASSERT(g_pFwMemCfg.vdec_mem_size >= size);
        OS_PRINTF("[%s] vdec fw require memory resource:  %ld bytes\n", __func__, size);
        OS_PRINTF("[%s] system allocate memory for vdec fw:  %ld bytes\n", __func__, g_pFwMemCfg.vdec_mem_size);

        if (p_file_seq->vdec_policy == 0) {
            vdec_set_buf(p_file_seq->p_vdec_dev, VDEC_BUFFER_AD, g_pFwMemCfg.p_vdec_start);
        } else {
            vdec_set_buf(p_file_seq->p_vdec_dev, p_file_seq->vdec_policy, g_pFwMemCfg.p_vdec_start);
        }

        OS_PRINTF("[%s] video fw start buffer: 0x%x\n", __func__, g_pFwMemCfg.p_vdec_start);
#ifndef ES_END_IN_TASK
        ret = vdec_start(p_file_seq->p_vdec_dev, 1, 2); //h264
        OS_PRINTF("[%s] p_file_seq->m_video_codec_type:%d\n", __func__, p_file_seq->m_video_codec_type);
        MT_ASSERT(SUCCESS == ret);
#endif
    }
    if(p_fill_es_task == fill_es_task)
    {
        if (p_file_seq->p_vdec_dev) {
            if(((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->is_timeshift == 1)
                vdec_freeze_stop(p_file_seq->p_vdec_dev);
            else
                vdec_stop(p_file_seq->p_vdec_dev);
        }
    }
    
    p_file_seq->force_stopped = 0;

    if ((p_file_seq->total_path > 1)) {
        p_file_seq->m_preload_state = FILE_SEQ_PLAY;
        p_file_seq->preload_audio_buffer.read_pos = 0;
        p_file_seq->preload_audio_buffer.write_pos = 0;
        p_file_seq->preload_video_buffer.read_pos = 0;
        p_file_seq->preload_video_buffer.write_pos = 0;
        p_file_seq->p_video_buf = p_file_seq->p_video_buf_tail = NULL;
        p_file_seq->p_audio_buf = p_file_seq->p_audio_buf_tail = NULL;
#ifdef __LINUX__
        mtos_task_create((u8 *)"preload thread", mutipath_preload_thread, p_file_seq, 0, 0, 0);
#else
        OS_PRINTF("%d %x %d\n", p_file_seq->task_preload_prio,
                  p_file_seq->p_preloadstack, p_file_seq->task_preload_size);

        while (p_file_seq->is_preload_task_alive) {
            mtos_task_sleep(100);
        }

        mtos_task_delete(p_file_seq->task_preload_prio);
        mtos_task_create((u8 *)"preload thread", mutipath_preload_thread, p_file_seq,
                         p_file_seq->task_preload_prio, p_file_seq->p_preloadstack,
                         p_file_seq->task_preload_size);
#endif

        while (p_file_seq->is_preload_task_alive != TRUE) {
            mtos_task_sleep(100);
            mtos_printk("\n%s %d\n", __func__, __LINE__);
        }
    }


#ifdef __LINUX__
    p_file_seq->m_play_state = FILE_SEQ_PLAY;
    ret = mtos_task_create((u8 *)"fill_es", p_fill_es_task, p_handle, 0, 0, 0);
#else
    //yliu :make sure task deleted befor create ,modifyed to delete by task_flag in future
    ret = mtos_task_delete(p_file_seq->task_prio);

    if (ret) {
        FILE_SEQ_LOG("[%s]delete failed =%d \n", __func__, ret);
        FILE_SEQ_LOG("[%s]task_prio[%d] \n", __func__, p_file_seq->task_prio);
    }

    mtos_printk("\n@@@@@%s %d  state:%d\n", __func__, __LINE__, p_file_seq->m_play_state);
    p_file_seq->m_play_state = FILE_SEQ_PLAY;
    ret = mtos_task_create((u8 *)"fill_es", p_fill_es_task, p_handle,
                           p_file_seq->task_prio, p_file_seq->pstack, p_file_seq->task_size);
#endif

    while (p_file_seq->is_task_alive == FALSE) {
        FILE_SEQ_DEBUG("\n %s %d waiting ~~\n", __func__, __LINE__);
        mtos_task_sleep(50);
    }

    FILE_SEQ_LOG("[%s] end end ...\n", __func__);
    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    return 0;
}



/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
int file_seq_preview(void * p_handle)
{
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    vdec_info_t vstate;
    BOOL tv_sys_auto_set_tmp = p_file_seq->tv_sys_auto_set;
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    p_file_seq->tv_sys_auto_set = FALSE;
    int ret = file_seq_start(p_file_seq, 0);

    if (ret != 0) {
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        FILE_SEQ_LOG("[%s][ERROR] fail to file_seq_start...\n", __func__);
        FILE_SEQ_LOG("[%s]do nothing...\n", __func__);
        return -1;
    }

    do {
        vdec_get_info(p_file_seq->p_vdec_dev, &vstate);
        mtos_task_sleep(20);
    } while (vstate.decoder_frame_count < 20 && p_file_seq->m_play_state != FILE_SEQ_EXIT);

    file_seq_pause(p_file_seq);
    p_file_seq->tv_sys_auto_set = tv_sys_auto_set_tmp;
    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    FILE_SEQ_LOG("[%s] end end ...\n", __func__);
    return 0;
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static BOOL  file_seq_is_fill_es_task_alive(void * p_handle)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;

    if (p_file_seq->is_task_alive == FALSE) {
        return FALSE;
    } else {
        FILE_SEQ_LOG("[%s] m_play_state ...\n", __func__, p_file_seq->m_play_state);
        FILE_SEQ_LOG("[%s] is_task_alive ...\n", __func__, p_file_seq->is_task_alive);
        return TRUE;
    }
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static  int file_seq_stop(void * p_handle)
{
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
#if (defined(CONFIG_YINHE_WARRIOS_SDK))
    FILE_SEQ_LOG("[%s] %d...\n", __func__, __LINE__);

    if (p_file_seq->m_play_state !=  FILE_SEQ_EXIT) {
        while (p_file_seq->m_play_state !=  FILE_SEQ_PAUSE) {
            mtos_task_sleep(30);
        }

        FILE_SEQ_LOG("[%s] stop !!!\n", __func__);
        p_file_seq->m_play_state = FILE_SEQ_EXIT;
    }

#else

    //#ifndef SWITCH_DEBUG
    if (p_file_seq->ott_playmode != OTT_LIVE_MODE) {
        if (p_file_seq->p_audio_dev) {
            aud_stop_vsb(p_file_seq->p_audio_dev);
        }

        if (p_file_seq->p_vdec_dev) {
			if(((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->is_timeshift==1 )
				         vdec_freeze_stop(p_file_seq->p_vdec_dev);
				else
            vdec_stop(p_file_seq->p_vdec_dev);
        }
    }

    //#endif
    //
#ifndef __LINUX__
    modify_sockets(FILE_PLAY_SOCKET);
#endif
    //
    x_clear_cmd_fifo(p_file_seq);

    if (p_file_seq->m_play_state ==  FILE_SEQ_PLAY
        || p_file_seq->m_play_state ==  FILE_SEQ_PAUSE
    || p_file_seq->m_play_state ==  FILE_SEQ_LOADMEDIA) {
        FILE_SEQ_LOG("[%s] %d...\n", __func__, __LINE__);
        p_file_seq->m_play_state = FILE_SEQ_EXIT;

        if (p_file_seq->total_path > 1) {
            p_file_seq->m_preload_state = FILE_SEQ_EXIT;
        }
    } else {
        FILE_SEQ_ERROR("[%s] do nothing...\n", __func__);

        if (NULL != p_file_seq->event_cb) {
            if (p_file_seq->m_play_state == FILE_SEQ_EXIT) {
                FILE_SEQ_LOG("[%s] drv send FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT!!\n", __func__);
                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT, 0);
            } else if (p_file_seq->m_play_state == FILE_SEQ_STOP) {
                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT, 0);
            } else if (p_file_seq->m_play_state == FILE_SEQ_LOADED) {
                p_file_seq->m_play_state = FILE_SEQ_STOP;
                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT, 0);
            }
        }

        FILE_SEQ_ERROR("[%s] state[%d].\n", __func__, p_file_seq->m_play_state);
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return 0;
    }

#endif
    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    FILE_SEQ_LOG("[%s] stop stop ...\n", __func__);
    return 0;
}
static  int file_seq_force_stop(void * p_handle)
{
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;

#ifndef CHIP_CONCERTO
#ifdef WITH_TCPIP_PROTOCOL    

    if(p_file_seq->is_load_task_alive){
        Abort_Download_Task(p_file_seq->net_task_prio, TRUE);
    }

#endif

#endif
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);

    if (p_file_seq->p_audio_dev) {
        aud_stop_vsb(p_file_seq->p_audio_dev);
    }

    if (p_file_seq->p_vdec_dev) {
        mtos_printk("\n%s %d\n", __func__, __LINE__);
        vdec_stop(p_file_seq->p_vdec_dev);
        mtos_printk("\n%s %d\n", __func__, __LINE__);
    }

    x_clear_cmd_fifo(p_file_seq);
#ifndef __LINUX__
    modify_sockets(FILE_PLAY_SOCKET);
#endif

    if (p_file_seq->m_play_state ==  FILE_SEQ_PLAY
        || p_file_seq->m_play_state ==  FILE_SEQ_PAUSE
    || p_file_seq->m_play_state ==  FILE_SEQ_LOADMEDIA) {
        FILE_SEQ_LOG("[%s] %d...\n", __func__, __LINE__);
        p_file_seq->m_play_state = FILE_SEQ_EXIT;
        p_file_seq->force_stopped = 1;

        if (p_file_seq->total_path > 1) {
            p_file_seq->m_preload_state = FILE_SEQ_EXIT;
        }
    } else {
        FILE_SEQ_ERROR("[%s] do nothing...\n", __func__);
        p_file_seq->force_stopped = 1;

        if (NULL != p_file_seq->event_cb) {
            if (p_file_seq->m_play_state == FILE_SEQ_EXIT) {
                FILE_SEQ_LOG("[%s] drv send FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT!!\n", __func__);
                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT, 0);
            } else if (p_file_seq->m_play_state == FILE_SEQ_STOP) {
                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT, 0);
            } else if (p_file_seq->m_play_state == FILE_SEQ_LOADED) {
                p_file_seq->m_play_state = FILE_SEQ_STOP;
                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT, 0);
            }
        }

        FILE_SEQ_ERROR("[%s] state[%d].\n", __func__, p_file_seq->m_play_state);
    }

    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    FILE_SEQ_LOG("[%s] stop stop ...\n", __func__);
    return 0;
}
/*
 *
 *
 *
 *
 *
 *
 *
 *
 */

/*
 *
 *
 *     pause in no block mode
 *
 *
 *
 *
 *
 */
static  int file_seq_pause(void * p_handle)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);

    if (p_file_seq->m_play_state ==  FILE_SEQ_PLAY) {
        if (p_file_seq->m_audio_codec_type != 0xffff) {
            aud_pause_vsb(p_file_seq->p_audio_dev);
        }

        if (p_file_seq->only_audio_mode == FALSE) {
            vdec_pause(p_file_seq->p_vdec_dev);
        }

        p_file_seq->m_play_state = FILE_SEQ_PAUSE;
    } else {
        FILE_SEQ_LOG("[%s] now state is not FILE_SEQ_PLAY...!!!\n", __func__);
        FILE_SEQ_LOG("[%s] do nothing...\n", __func__);
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return 0;
    }

    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    FILE_SEQ_LOG("[%s] stop stop ...\n", __func__);
    return 0;
}

/*
 *
 *
 *         resume in no block mode
 *
 *
 *
 *
 *
 */
static  int file_seq_resume(void * p_handle)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);

    if (p_file_seq->m_play_state ==  FILE_SEQ_PAUSE) {
        if (p_file_seq->only_audio_mode == FALSE) {
            vdec_resume(p_file_seq->p_vdec_dev);
        }

        mtos_task_sleep(10);

        if (p_file_seq->m_audio_codec_type != 0xffff) {
            aud_resume_vsb(p_file_seq->p_audio_dev);
        }

        p_file_seq->m_play_state = FILE_SEQ_PLAY;
    } else {
        FILE_SEQ_LOG("[%s] do nothing...\n", __func__);
        FILE_SEQ_LOG("[%s] the last state is not play ...!!!!\n", __func__);
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return 0;
    }

    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    FILE_SEQ_LOG("[%s] stop stop ...\n", __func__);
    return 0;
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static   int file_seq_set_speed(void * p_handle, int speed)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);
    p_file_seq->tmp_speed = speed;

    /*
     *    ensure that no 'CMD_CHANGE_SPEED' to be processed !!!!
     */
    if (IS_SET(p_file_seq->m_user_cmd, CMD_CHANGE_SPEED)) {
        do {
            mtos_task_sleep(10);

            if (IS_SET(p_file_seq->m_user_cmd, CMD_CHANGE_SPEED) == FALSE) {
                break;
            }
        } while (0);
    }

    SetFlag(p_file_seq->m_user_cmd, CMD_CHANGE_SPEED);
    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    FILE_SEQ_LOG("[%s] stop stop ...\n", __func__);
    return 0;
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static void file_seq_get_subt_info(void * p_handle, void ** subt_info)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);

    if (strncmp(p_file_seq->m_path[0], "camera://", 9) == 0 ||
    strstr(p_file_seq->m_path[0], RECORD_FILE_TAIL) != NULL) {
        *subt_info = NULL;
        return;
    }

    *(file_seq_video_subtitle_t **)subt_info = (void *) & (((demuxer_t *)(p_file_seq->p_demuxer))->subt_info);
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static void file_seq_set_subt_id(void * p_handle, int subt_id)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);
    
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    FILE_SEQ_DEBUG("[%s] start start ...subt_id:%d\n", __func__, subt_id);

    /*
     *    ensure that no 'CMD_CHANGE_AUDIO_TRACK' to be processed !!!!
     */
    if (IS_SET(p_file_seq->m_user_cmd, CMD_CHANGE_SUBT_ID)) {

        FILE_SEQ_DEBUG("[%s] ----------- it is already changing subt id now, do nothing!\n", __func__);


    } else {
    
    	if(p_file_seq->subt_id != subt_id){
	       SetFlag(p_file_seq->m_user_cmd, CMD_CHANGE_SUBT_ID);
    p_file_seq->subt_id = subt_id;
}

    }

    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);
    
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static int file_seq_get_subt(void * p_handle, unsigned char ** start, int * pts)
{
    int size_sub = 0;
    int size = 0;
    demux_stream_t * ds_sub = NULL;
    unsigned char p_data[2048];
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;

    if (p_file_seq->is_task_alive == FALSE) {
        return 0;
    }

    mtos_sem_take((os_sem_t *)(&(p_file_seq->sub_fifo_mutex)), 0);
    *start =  p_file_seq->subt_buf;
    memset(p_file_seq->subt_buf,0,2048);
    size = read_sub_fifo_kw(p_file_seq->p_sub_fifo_handle, p_data);

    if (size == 0) {
        mtos_sem_give((os_sem_t *)(&(p_file_seq->sub_fifo_mutex)));
        return 0;
    } else {
        size_sub = (p_data[0] << 24) + (p_data[1] << 16) + (p_data[2] << 8) + p_data[3];
        *pts = (p_data[4] << 24) + (p_data[5] << 16) + (p_data[6] << 8) + p_data[7];

        if (size_sub > 2048) {
            size_sub = 2048;
        }

        memcpy(*start, (p_data + 8), size_sub);
    }

    mtos_sem_give((os_sem_t *)(&(p_file_seq->sub_fifo_mutex)));
    return size_sub;
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static  unsigned long int file_seq_get_cur_vpts(void * p_handle)
{
    unsigned long int vpts = 0;
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);

    if (p_file_seq->is_task_alive) {
#if 0//delete by doreen, 2013-06-24         
        //#ifdef __LINUX__
        //      vpts = *(p_file_seq->p_current_sys_vpts);
        //#else
        vpts = vdec_dec_get_vpts(p_file_seq->p_vdec_dev);
        //#endif
#else
        vdec_info_t vstate;
        vdec_get_info(p_file_seq->p_vdec_dev, &vstate);
        vpts = vstate.pts;

        if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
            vpts = vpts & (0x7fffffff);
        }

#endif
    } else {
        FILE_SEQ_DEBUG("[%s] do nothing ...!!!\n", __func__);
    }

    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    //FILE_SEQ_DEBUG("[%s] stop stop ...\n",__func__);
    return vpts;
}
/*
 *
 *
 *
 *   play at time in no block mode
 *
 *
 *
 *
 */
static  int file_seq_playAtTime(void * p_handle, int sec)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);
#if 0

    if (p_file_seq->vpts_upload < 2 && p_file_seq->total_path > 1) {
        OS_PRINTF("\n havn't upload pts ,pls dont seek\n");
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return -1;
    }

#endif
#ifndef MUL_SEEK

    if (p_file_seq->total_path > 1) {
        OS_PRINTF("DRV:DOES NOT SUPPORT PlayAtTime number=%d\n", p_file_seq->total_path);
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return -1;
    }

#endif

    /*
     *
     *     ensure that no 'CMD_CHANGE_SPEED' to be processed !!!!
     *
     */
    if (IS_SET(p_file_seq->m_user_cmd, CMD_PLAY_AT_TIME)) {
        //yliu modify for nonblock
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return 0;
    }

    FP_USER_CMD_T cmd;
    memset(&cmd, 0, sizeof(FP_USER_CMD_T));
    cmd.type = CMD_PLAY_AT_TIME;
    cmd.param = sec;

    if (x_push_user_cmd(p_file_seq, &cmd) == 0) {
        p_file_seq->is_play_at_time = 1;

        if (p_file_seq->total_path > 1) {
            p_file_seq->seek_seconds = sec * 1.0;
        } else {
        if(fp_is_timeshift_file() == 1)
			p_file_seq->seek_seconds = sec * 1.0;
            if (p_file_seq->duration - 5 > 0.001) {
                p_file_seq->seek_seconds = min(sec * 1.0, p_file_seq->duration - 5);    //-p_file_seq->vpts_upload*1.f/1000*1.f/TIME_BASE;
            }
        }

        //yliu rm for noblock
    }

    FILE_SEQ_DEBUG("[%s] --------play at time: next %d seconds\n", __func__, (int)sec);
    FILE_SEQ_DEBUG("[%s] stop stop ...\n", __func__);
    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    return 0;
}


/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static FILE_SEQ_STATUS file_seq_get_status(void * p_handle)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);
    return p_file_seq->m_play_state;
}


/*
 *
 *
 *
 *
 *
 *
 *
 *
 */



/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static int file_seq_change_audio_track(void * p_handle, int trackId)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    int ret = 0;
    MT_ASSERT(p_file_seq != NULL);
    //demuxer_switch_audio(p_file_seq->p_demuxer,trackId);
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);

    /*
     *    ensure that no 'CMD_CHANGE_AUDIO_TRACK' to be processed !!!!
     */
    if (IS_SET(p_file_seq->m_user_cmd, CMD_CHANGE_AUDIO_TRACK)) {
#if 0

        do {
            mtos_task_sleep(30);

            if (IS_SET(p_file_seq->m_user_cmd, CMD_CHANGE_AUDIO_TRACK) == FALSE) {
                break;
            }
        } while (1);

#else
        FILE_SEQ_DEBUG("[%s] ----------- it is already changing audio track now, do nothing!\n", __func__);
        ret = 1;//haven't processed this command.
#endif
    } else {
        SetFlag(p_file_seq->m_user_cmd, CMD_CHANGE_AUDIO_TRACK);
        p_file_seq->audio_track_id = trackId;
    }

    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);
    return ret;
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static int file_seq_get_audio_track_lang(void * p_handle, TRACK_LANG * lang_array)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);
    demuxer_t * p_demuxer = (demuxer_t *)(p_file_seq->p_demuxer);
    sh_audio_t * sh = NULL;
    TRACK_LANG lang_array_tmp[MAX_A_STREAMS] ;//= {0};
    TRACK_LANG * p_lang_array = NULL;
    char * lang = NULL;
    char * title = NULL;
    int i, track_num = 0;
    memset(lang_array_tmp, 0, MAX_A_STREAMS * sizeof(TRACK_LANG));

    if (p_file_seq->audio_track_num == 0) {
        for (i = 0; i < MAX_A_STREAMS; i++) {
            sh = p_demuxer->a_streams[i];

            //FILE_SEQ_DEBUG("[%s] -----------  for 1: i=%d\n",__func__,i);
            if (sh) {
                FILE_SEQ_DEBUG("[%s] -----------  for 1: i=%d sh exist!\n", __func__, i);

                if (sh->lang) {
                    //FILE_SEQ_DEBUG("[%s] -----------  for 1: %s\n",__func__,sh->lang);
                    //lang = strdup(sh->lang);
                    lang = mtos_malloc(strlen(sh->lang) + 1);
                    memset(lang, 0, strlen(sh->lang) + 1);
                    strcpy(lang, sh->lang);
                    FILE_SEQ_DEBUG("[%s] -----------  for 1: audio track lang %d: %s\n", __func__, i, lang);
                    lang_array_tmp[track_num].track_id = i;
                    lang_array_tmp[track_num].lang = lang;

                    if (sh->title) {
                        //title = strdup(sh->title);
                        title = mtos_malloc(strlen(sh->title) + 1);
                        memset(title, 0, strlen(sh->title) + 1);
                        strcpy(title, sh->title);
                        lang_array_tmp[track_num].title = title;
                        FILE_SEQ_DEBUG("[%s] -----------  for 1: audio track title %d: %s\n", __func__, i, sh->title);
                    }

                    track_num++;
                }
            }
        }

        p_file_seq->audio_track_num = track_num;

        if (track_num > 0) {
            p_lang_array = (TRACK_LANG *)mtos_malloc(sizeof(TRACK_LANG) * track_num);
            memset(p_lang_array, 0, sizeof(TRACK_LANG)*track_num);

            for (i = 0; i < track_num; i++) {
                p_lang_array[i].track_id = lang_array_tmp[i].track_id;
                p_lang_array[i].lang = lang_array_tmp[i].lang;
                p_lang_array[i].title = lang_array_tmp[i].title;
            }
        }

        p_file_seq->audio_lang_array = p_lang_array;
    }

    FILE_SEQ_DEBUG("[%s] -----------  audio track (with lang) num: %d\n", __func__, p_file_seq->audio_track_num);

    if (lang_array && p_file_seq->audio_track_num > 0) {
        for (i = 0; i < p_file_seq->audio_track_num; i++) {
            lang_array[i].track_id = p_file_seq->audio_lang_array[i].track_id;
            lang_array[i].lang = p_file_seq->audio_lang_array[i].lang;
            lang_array[i].title = p_file_seq->audio_lang_array[i].title;
            FILE_SEQ_DEBUG("[%s] -----------  for 2: audio track %d: %s %s\n", __func__, lang_array[i].track_id, lang_array[i].lang, lang_array[i].title);
        }
    }

    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);
    return 0;
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static int file_seq_change_video_track(void * p_handle, int trackId)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    // TODO: liuyong  implement in future
    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    return 0;
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static BOOL file_seq_get_film_info(void * p_handle, FILM_INFO_T * pResult)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    double totalTime = 0.0;
    FILM_INFO_T  film_info;
    MT_ASSERT(p_file_seq != NULL);
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    memset(&film_info, 0, sizeof(FILM_INFO_T));
    if(pResult)
    {
	    memset(pResult, 0x00, sizeof(FILM_INFO_T));
    }
	if (p_file_seq->m_path[0] == NULL) {
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return FALSE;
    }

	if (strncmp(p_file_seq->m_path[0], "camera://", 9) == 0 ||
			strstr(p_file_seq->m_path[0], RECORD_FILE_TAIL) != NULL) {
		mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return FALSE;
    }

    if (!(p_file_seq->checkAvTypeOK && p_file_seq->loadMedaiOK)) {
        FILE_SEQ_DEBUG("[%s] fail to check avypte and load medai!!!\n", __func__);
        FILE_SEQ_DEBUG("do nothing !!!\n");
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return FALSE;
    }

    memset(&film_info, 0, sizeof(FILM_INFO_T));
    /* totalTime = demuxer_get_time_length(p_file_seq->p_demuxer);
       p_file_seq->duration = (u32)totalTime;*/

    if (p_file_seq->total_path > 1) {
        int j = 0;

        for (j = 0; j < p_file_seq->total_path; j++) {
            film_info.film_duration += p_file_seq->p_m_duration[j];
        }
    } else {
        film_info.film_duration = p_file_seq->file_duration;
    }

    film_info.file_size = p_file_seq->file_size;
    film_info.audio_type = p_file_seq->m_audio_codec_type;
    film_info.video_type = p_file_seq->m_video_codec_type;
    film_info.audio_track_num = ds_get_audio_count(p_file_seq->p_demuxer);
    film_info.video_track_num  = 0;
    film_info.canTrickPlay = TRUE;
    film_info.video_disp_w = p_file_seq->video_disp_w;
    film_info.video_disp_h = p_file_seq->video_disp_h;
    film_info.video_bps = p_file_seq->video_bps;
    film_info.video_fps = p_file_seq->video_fps;
	film_info.audio_track_id = p_file_seq->audio_track_id;
	film_info.audio_bps = p_file_seq->audio_bps;
	film_info.audio_samplerate = p_file_seq->audio_samplerate;
	film_info.file_name = p_file_seq->m_path[p_file_seq->cur_play_index];
	film_info.audio_language = p_file_seq->audio_language;	
    /* if(p_file_seq->is_ts)
       {
       p_file_seq->internal_event = GET_TS_MEDIA_INFO;
       }*/

    if (pResult) {
        memcpy(pResult, &film_info, sizeof(FILM_INFO_T));
    }

    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    return TRUE;
}

 static void file_seq_set_vdec_stop_type(void * p_handle, unsigned int vdec_stop_type)
 {
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
	FILE_SEQ_DEBUG("[%s] [%d] start start\n", __func__, __LINE__);
	MT_ASSERT(p_file_seq != NULL);
	
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
	
	if(vdec_stop_type == VDEC_FREEZE_STOP)
    {
		p_file_seq->vdec_stop_type = VDEC_FREEZE_STOP;
	}
	else if(vdec_stop_type == VDEC_SWITCH_CH_STOP)
	{
		p_file_seq->vdec_stop_type = VDEC_SWITCH_CH_STOP;
	}
	else
	{
		p_file_seq->vdec_stop_type = VDEC_STOP;
	}
    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
	
    FILE_SEQ_DEBUG("[%s] [%d] end end ...\n", __func__, __LINE__);
 }
#if 0
///save the download url to m_path in FILE_SEQ_T
static  BOOL http_request_url_for_website(void * p_handle, u8 * req_url, u8 size)//
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);
    u8 request_video_level = 0;

    if (size == 0) {
        request_video_level = QEQUEST_VIDEO_LD;
    } else if (size == 1) {
        request_video_level = QEQUEST_VIDEO_SD;
    } else if (size == 2) {
        request_video_level = QEQUEST_VIDEO_HD;
    }

    u8 website_type = 255;
    char * p_website = strstr(req_url, "youtube");

    if (p_website) {
        website_type = WEBSITE_YOUTUBE;
    } else if (p_website = strstr(req_url, "dailymotion")) {
        website_type = WEBSITE_DAILYMOTION;
    } else if (p_website = strstr(req_url, "yahoo.com")) {
        website_type = WEBSITE_YAHOO;
    } else if (p_website = strstr(req_url, "add-anime")) {
        website_type = WEBSITE_ADDANIME;
    }

    if (website_type == 255) {
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return FALSE;
    }

    request_url_t * p_request_url;
    p_request_url = (request_url_t *)mtos_malloc(sizeof(request_url_t));

    if (p_request_url) {
        memset(p_request_url, 0, sizeof(request_url_t));
    }

    int ret = Nw_Request_Website_DownloadURL(website_type, req_url, request_video_level, p_request_url, 15);

    if (ret == 0) {
        memset(p_file_seq->m_path[0], 0, MAX_PATH_NUM);
        memcpy(p_file_seq->m_path[0], p_request_url->playUrlArray[size], strlen(p_request_url->playUrlArray[size]));
        FILE_SEQ_LOG("[%s] success get download url[%s]\n", __func__, p_file_seq->m_path[0]);
    } else {
        if (p_request_url) {
            mtos_free(p_request_url);
            p_request_url = NULL;
        }

        FILE_SEQ_ERROR("[%s] fail to get download url\n", __func__);
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return FALSE;
    }

    if (p_request_url) {
        mtos_free(p_request_url);
        p_request_url = NULL;
    }

    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    FILE_SEQ_LOG("[%s] stop stop ...\n", __func__);
    return TRUE;
}
#endif
#if 1
#define   NET_MEDIA_MAX_REAL_URL_NUM  128
typedef struct {
    int real_url_count;
    char * real_url[NET_MEDIA_MAX_REAL_URL_NUM];
} NETPLAY_REAL_URL;
int is_netplay_url(char * url);
int netplay_get_real_url(char * url, NETPLAY_REAL_URL * info);
int netplay_free_real_url(NETPLAY_REAL_URL * info);
#endif
#define    FILM_URL_MAX_SIZE    (4096)
#define    FILM_ARRAY_LEN         (128)
#define    TMP_BUF_MAX_LEN      (FILM_URL_MAX_SIZE*FILM_ARRAY_LEN)
#define    MULTI_URL_LEN            512
#define    RAM_FS_USE  1
#define    FILE_FREE(p)  {if(p){mtos_free(p);}p=NULL ;}

#ifdef RAM_FS_USE

#ifndef __LINUX__
#define REPLACE_PATH_FILE       "r:replace_path_file.xml"
#else
#define REPLACE_PATH_FILE       "replace_path_file.xml"
#endif
#else
#define REPLACE_PATH_FILE       "replace_path_file.xml"
#endif

#ifndef CHIP_CONCERTO
#ifdef WITH_TCPIP_PROTOCOL
void replace_real_play_path(char * p_play_path)
{
    char  *  p_url_start = NULL;
    char  * p_url_end = NULL;

    if (strstr(p_play_path, "edge.filmon.com/live")) {
        return;
    }

    if (strstr(p_play_path, "youtube.com")) {
        if (strstr(p_play_path, "|live") == NULL) {
            return;
        }
    }

    FILE_SEQ_LOG("[%s] url[%s] get the real play path.\n", __func__, p_play_path);
    p_url_start = strstr(p_play_path, "|");
    char size_str[10];
    char tmp_url[NET_MEDIA_MAX_REAL_URL_NUM];
    int  req_ret = 0, i = 0 ;
    int  is_giniko = 0;
    int  is_malimar = 0;
    int  is_filmon = 0;
    int  is_live_youtube = 0;
    int  is_live_dailymotion = 0;
    char filmon_channle_id[10];
    memset(tmp_url, 0, NET_MEDIA_MAX_REAL_URL_NUM);
    memset(size_str, 0, 10);
    memset(filmon_channle_id, 0, 10);
    if(strstr(p_play_path, "giniko.com") )
    {
        is_giniko = 1;
    }
    if(strstr(p_play_path, "malimar.tv"))
    {
        is_malimar = 1;
    }
    if(strstr(p_play_path, "filmon.com"))
    {
        is_filmon = 1;

    }
    if(strstr(p_play_path, "dailymotion.com/live/video/"))
    {
        is_live_dailymotion = 1;
    }
    if(strstr(p_play_path, "youtube.com"))
    {
        is_live_youtube = 1;
    }

    if (p_url_start || is_giniko || is_malimar || is_filmon || is_live_dailymotion) {
        if (is_giniko == 1 || is_malimar == 1) {
            memcpy(tmp_url, p_play_path, strlen(p_play_path));
        } else if (is_filmon == 1) {
            if (!p_url_start) {
                return;
            }

            p_url_end = strstr(p_url_start + 1, "|");

            if (!p_url_end) {
                return;
            }

            char * p_url_end2 = (char *)(p_play_path + strlen(p_play_path));

            if (!p_url_end2) {
                return;
            }

            memcpy(size_str, p_url_end + 1, (p_url_end2 - p_url_end - 1 + 1));
            memcpy(filmon_channle_id, p_url_start + 1, (p_url_end - p_url_start - 1));
            memcpy(tmp_url, p_play_path, (p_url_start - p_play_path));
            FILE_SEQ_ERROR("zx size_str[%s] filmon_channle_id[%s]\n", size_str, filmon_channle_id);
        } else if (is_live_dailymotion == 1) {
            char p_videoid[32];
            memset(p_videoid, 0, 32);
            p_url_start = strstr(p_play_path, "dailymotion.com/live/video/");

            if (!p_url_start) {
                return;
            }

            p_url_start = p_url_start + strlen("dailymotion.com/live/video/");
            p_url_end = strstr(p_url_start, "_");

            if (p_url_end == NULL) {
                p_url_end = (char *)(p_play_path + strlen(p_play_path));
                memcpy(p_videoid, p_url_start, (p_url_end - p_url_start + 1));
            } else {
                memcpy(p_videoid, p_url_start, (p_url_end - p_url_start));
            }

            sprintf(tmp_url, "http://www.dailymotion.com/sequence/full/%s", p_videoid);
        } else {
            p_url_end = (char *)(p_play_path + strlen(p_play_path));

            if (!p_url_start) {
                return;
            }

            memcpy(size_str, p_url_start + 1, (p_url_end - p_url_start - 1 + 1));
            memcpy(tmp_url, p_play_path, (p_url_start - p_play_path));
        }

        if (is_filmon) {
            char p_post_buf[32];
            memset(p_post_buf, 0, 32);
            sprintf(p_post_buf, "channel_id=%s", filmon_channle_id);
            char extraheader_buf[512];
            memset(extraheader_buf, 0, 512);
            //sprintf(extraheader_buf,"Cookie: flash-player-type=hls\r\nReferer: http://www.filmon.com\r\nContent-Type: application/x-www-form-urlencoded\r\n");
            sprintf(extraheader_buf, "Content-Length: %d\r\nX-Requested-With: XMLHttpRequest\r\nCookie: lang=en; PHPSESSID=uc3p8u4d63btj0uu3koejg94b5; flash-player-type=%s\r\nReferer: http://www.filmon.com\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n%s"
                    , strlen(p_post_buf), size_str, p_post_buf);

            for (i = 0; i < 5 ; i++) {
                req_ret = Nw_DownloadURL_POST_ex(tmp_url, NULL, 0, REPLACE_PATH_FILE, extraheader_buf, NULL, NULL, 30); ///30 second time out

                if (req_ret == 1) {
                    break;
                } else if (req_ret == -1) {
                    FILE_SEQ_ERROR("ERR:Abort by user!!! \n");
                    break;
                } else {
                    FILE_SEQ_ERROR("ERR: request %s fail index = %d ,by url %s \n", REPLACE_PATH_FILE, i, tmp_url);
                    mtos_task_sleep(500);
                }
            }
        } else if (is_live_youtube) {
            memcpy(tmp_url + strlen(tmp_url), "_live", strlen("_live")); //add live mrak
            request_url_t request_url;
            memset(&request_url, 0, sizeof(request_url_t));
            req_ret = Nw_Request_Website_DownloadURL(WEBSITE_YOUTUBE, tmp_url, (QEQUEST_VIDEO_SD + QEQUEST_VIDEO_HD), &request_url, 15);

            if (req_ret == 0) {
                char * downlaod_url = NULL;

                if (strlen(request_url.playUrlArray[1]) > 0) {
                    downlaod_url = request_url.playUrlArray[1];
                } else if (strlen(request_url.playUrlArray[2]) > 0) {
                    downlaod_url = request_url.playUrlArray[2];
                } else if (strlen(request_url.playUrlArray[0]) > 0) {
                    downlaod_url = request_url.playUrlArray[0];
                }

                memset(p_play_path, 0, NET_MEDIA_MAX_REAL_URL_NUM);
                memcpy(p_play_path, downlaod_url, strlen(downlaod_url));
                req_ret = 1;
                FILE_SEQ_DEBUG("youtube choose h264EncUrlArray: [%s].\n", p_play_path);
            }
        } else {
            //start download data
            for (i = 0; i < 3 ; i++) {
                req_ret = Nw_DownloadURLTimeout(tmp_url, REPLACE_PATH_FILE, 10, NULL, NULL, NULL, NULL, 0);

                if (req_ret == 1) {
                    break;
                } else if (req_ret == -1) {
                    FILE_SEQ_ERROR("ERR:Abort by user!!! \n");
                    break;
                } else {
                    FILE_SEQ_ERROR("ERR: request %s fail index = %d ,by url %s \n", REPLACE_PATH_FILE, i, tmp_url);
                }
            }
        }

        if (req_ret == 1) {
            FILE_SEQ_DEBUG("[%s] success post !!!\n", __func__);
        } else {
            FILE_SEQ_ERROR("[%s:%d][ERROR] fail POST in url = %s  !!!\n", __func__, __LINE__, tmp_url);
            return;
        }

        ufs_file_t * fd = (ufs_file_t *)mtos_malloc(sizeof(ufs_file_t));
        memset(fd, 0, sizeof(ufs_file_t));
        unsigned short  path_tmp[NET_MEDIA_MAX_REAL_URL_NUM] = {0};
        unsigned short * p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)REPLACE_PATH_FILE, path_tmp);
        int ret = ufs_open(fd, p_filename1, UFS_READ);

        if (ret != FR_OK) {
            FILE_SEQ_ERROR("%s open file %s failed\n", __func__, REPLACE_PATH_FILE);
            FILE_FREE(fd);
            return;
        }

        int file_size = fd->file_size;
        char * file_buffer = (char *)mtos_malloc(file_size + 1);

        if (file_buffer) {
            int rdNum = 0;
            memset(file_buffer, 0x00, file_size + 1);
            ufs_read(fd, file_buffer, file_size, (u32 *)&rdNum);

            if (strstr(p_play_path, "vdn.live.cntv.cn")) {
                strcat(size_str, "\":\"");
                p_url_start = strstr(file_buffer, size_str);

                if (p_url_start) {
                    p_url_start = p_url_start + strlen(size_str);
                    p_url_end = strstr(p_url_start, "\"");

                    if (p_url_end) {
                        ///replace the p_play_path!!!!
                        memset(p_play_path, 0, NET_MEDIA_MAX_REAL_URL_NUM);
                        memcpy(p_play_path, p_url_start, (p_url_end - p_url_start));
                    }

                    FILE_SEQ_LOG("[%s] cntv real play url:%s\n", __func__, p_play_path);
                }
            } else if (strstr(p_play_path, "web-play.pptv.com")) {
                p_url_start = strstr(file_buffer, "http");

                if (p_url_start) {
                    if (strcmp(size_str, "sd") == 0) {
                        //do nothing
                    } else {
                        //hd
                        p_url_start = p_url_start + 10;//skip http
                        p_url_start = strstr(p_url_start, "http");
                    }

                    p_url_end = strstr(p_url_start, "m3u8?");
                    p_url_end = p_url_end + 5;
                    memcpy(p_play_path, p_url_start, (p_url_end - p_url_start));
                    strcat(p_play_path, "type=m3u8.web.pad&pre=tv");
                    FILE_SEQ_LOG("[%s] pptv real url:%s\n", __func__, p_play_path);
                }
            } else if (is_giniko) {
                p_url_start = strstr(file_buffer, "file: \"");

                if (p_url_start) {
                    p_url_start = p_url_start + strlen("file: \"");
                    p_url_end = strstr(p_url_start, "\"");
                    memcpy(p_play_path, p_url_start, (p_url_end - p_url_start));
                    FILE_SEQ_LOG("[%s] giniko real url:%s\n", __func__, p_play_path);
                }
            } else if (is_malimar) {
                p_url_start = strstr(file_buffer, "file: \"");

                if (p_url_start) {
                    p_url_start = p_url_start + strlen("file: \"");
                    p_url_end = strstr(p_url_start, "\"");
                    memcpy(p_play_path, p_url_start, (p_url_end - p_url_start));
                    FILE_SEQ_LOG("[%s] malimar.tv real url:%s\n", __func__, p_play_path);
                }
            } else if (is_filmon) {
                p_url_start = strstr(file_buffer, "streams\"");
                char * new_start = NULL;
                char * url = parse_html_to_get_inner_string(p_url_start, (char *)"low\",\"url\":\"", (char *)"\"", &new_start);

                if (url) {
                    if (strcmp(size_str, "hls") == 0) {
                        memset(p_play_path, 0, NET_MEDIA_MAX_REAL_URL_NUM);
                        memcpy(p_play_path, url, strlen(url));
                    } else {
                        FILE_SEQ_LOG("[%s] filmon rtmp url:%s\n", __func__, url);
                        char app_str[512];
                        p_url_start = strstr(url, "live/");
                        p_url_end = (char *)(url + strlen(url));
                        memcpy(app_str, p_url_start, (p_url_end - p_url_start + 1));
                        memset(p_play_path, 0, NET_MEDIA_MAX_REAL_URL_NUM);
                        sprintf(p_play_path, "%s -y %s.low.stream -a %s", url, filmon_channle_id, app_str);
                    }

                    FILE_FREE(url);
                }
            } else if (is_live_dailymotion == 1) {
                char * new_start;
                char * url = parse_html_to_get_inner_string(file_buffer, (char *)"autoURL\":\"", (char *)"\"", &new_start);

                if (url) {
                    memset(p_play_path, 0, NET_MEDIA_MAX_REAL_URL_NUM);
                    memcpy(p_play_path, url, strlen(url));
                }

                FILE_FREE(url);
            }

            FILE_FREE(file_buffer);
        }

        ufs_close(fd);
        ufs_delete(p_filename1, 0);
        FILE_FREE(fd);
    }

    return;
}
#endif
#endif
static void malloc_preload_buffer(void * p_handle)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;

    if(p_file_seq->preload_video_buffer_malloc_internal == 1 && !p_file_seq->preload_video_buffer.buffer_start)
    {
        p_file_seq->preload_video_buffer.buffer_start = av_mallocz(PRELOAD_VIDEO_BUFFER_SIZE);
        p_file_seq->preload_video_buffer_size = PRELOAD_VIDEO_BUFFER_SIZE;
        p_file_seq->preload_video_buffer.buffer_len = PRELOAD_VIDEO_BUFFER_SIZE;
        MT_ASSERT(p_file_seq->preload_video_buffer.buffer_start != NULL);
    }

    if(p_file_seq->preload_audio_buffer_malloc_internal == 1 && !p_file_seq->preload_audio_buffer.buffer_start)
    {
        p_file_seq->preload_audio_buffer.buffer_start = av_mallocz(PRELOAD_AUDIO_BUFFER_SIZE);
        p_file_seq->preload_audio_buffer_size = PRELOAD_AUDIO_BUFFER_SIZE;
        p_file_seq->preload_audio_buffer.buffer_len = PRELOAD_AUDIO_BUFFER_SIZE;
        MT_ASSERT(p_file_seq->preload_audio_buffer.buffer_start != NULL);
    }
    return;
}
static void free_preload_buffer(void * p_handle)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    if(p_file_seq->preload_video_buffer_malloc_internal == 1 && p_file_seq->preload_video_buffer.buffer_start)
    {
        av_freep(&(p_file_seq->preload_video_buffer.buffer_start));
        p_file_seq->preload_video_buffer.buffer_start = NULL;
    }

    if(p_file_seq->preload_audio_buffer_malloc_internal == 1 && p_file_seq->preload_audio_buffer.buffer_start)
    {
        av_freep(&(p_file_seq->preload_audio_buffer.buffer_start));
        p_file_seq->preload_audio_buffer.buffer_start = NULL;
    }
    return;
}





/*
 *
 *
 *
 *
 *
 *
 *
 *
 */

#ifdef WITH_OTT_DATA
int encry_delay = 0;
#endif
                  void file_seq_set_ott_playmode(void * p_handle, int playmode)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    p_file_seq->ott_playmode = playmode;
    FILE_SEQ_LOG("[%s] ott_playmode[%d],playmode[%d]\n", __func__, p_file_seq->ott_playmode, playmode);
}
static void get_av_ts_pid(char * buf)
{
    char * pV = NULL;
    char * pA = NULL;
    char * p = NULL;
    char data[4];
    char len;
    char i;
    pV = strstr(buf, "vpid:");

    if (!pV) {
        return;
    }

    pA = strstr(buf, "apid:");

    if (!pA) {
        return;
    }

    memset(data, 0, 4);
    len = pA - pV - 5 - 2;

    if (len > 3) {
        return;
    }

    p = pV + 5;

    for (i = 0; i < len; i++) {
        data[i] = p[i];
    }

    special_ts_vpid = atoi(data);
    memset(data, 0, 4);
    len = (buf + strlen(buf)) - pA - 5;

    if (len > 3) {
        special_ts_vpid = -1;
        return;
    }

    p = pA + 5;

    for (i = 0; i < len; i++) {
        data[i] = p[i];
    }

    special_ts_apid = atoi(data);
    return;
}
#ifdef WITH_TCPIP_PROTOCOL
extern  LIVE_TV_DP_HDL_T * LiveTV_GetInstance();
#endif
static int channle_id = 0;
extern int g_hls_playmode ;
                        static void file_seq_set_path_ex(void * p_handle, char * p_play_path, u32 path_id)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    char * p_path = p_play_path;
    char * tmp_buf = NULL;
    //char  tmp_multi_urls[50][MULTI_URL_LEN];
    char * tmp_multi_urls_array[50];
    int i;
    int path_len;
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);
    //FILE_SEQ_LOG("[%s] start start ...\n", __func__);
    FILE_SEQ_LOG("[%s] %d...\n", __func__, __LINE__);
    special_ts_vpid = -1;
    special_ts_apid = -1;
	g_hls_playmode = -1;
    ((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->is_timeshift = 0;
	//




	///
#ifdef CLIENT_SORT_TEST

    if (p_sort_test) {
        p_path = LiveTV_Client_Sort_Source(p_sort_test, channle_id, 0);
        channle_id++;

        if (p_path == NULL) {
            p_path = p_play_path;
        }

        if (channle_id % 5 == 0)
            //if(0)
        {
            LiveTV_Client_Sort_Destroy(p_sort_test);
            p_sort_test = NULL;
            p_sort_test = LiveTV_Client_Sort_Create();
        }
    }

#endif
#if 0
#ifdef WITH_OTT_DATA
#ifdef WITH_TCPIP_PROTOCOL

    if (LiveTV_GetInstance()) {
        int Live_type = 0;
        LIVE_TV_DP_HDL_T * LiveTV_P = (LIVE_TV_DP_HDL_T *)(LiveTV_GetInstance());

        if ((LiveTV_P->cur_dp_type == 2 && LiveTV_P->cur_sub_dp_type == 13572) || (LiveTV_P->cur_dp_type == 2 && LiveTV_P->cur_sub_dp_type == 85672)) {
            Live_type = 1;
        }

#if 0
        mtos_printk("\n%s %d dp:%d sub:%d\n", __func__, __LINE__, LiveTV_P->cur_dp_type, LiveTV_P->cur_sub_dp_type);

        if ((LiveTV_P->cur_dp_type == 2 && LiveTV_P->cur_sub_dp_type == 7231) || (LiveTV_P->cur_dp_type == 2 && LiveTV_P->cur_sub_dp_type == 85672)) {
            Live_type = 1;
        }

#endif

        if (Live_type && (rand() % 2 == 0)) {
            OS_PRINTF("\n%s %d encry ddddl\n", __func__, __LINE__);
            encry_delay = 1;
        } else {
            encry_delay = 0;
        }
    }

#endif
#endif
#endif
    //play mode
    file_seq_set_ott_playmode(p_file_seq, path_id);

    if (p_play_path == NULL) {
        FILE_SEQ_ERROR("[%s] p_play_path == NULL...\n", __func__);
        return;
    }

    if (strstr(p_path, ".mont_rtmp")) {
        FILE_SEQ_LOG("[mont_rtmp]THIS FILE SHOULD BE REDIRECTED !!!!!\n");
        ufs_file_t video_fp;
        memset(&video_fp, 0, sizeof(ufs_file_t));
        unsigned short  path_tmp[256] = {0};
        unsigned short * p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)p_path, path_tmp);

        if (NULL == p_filename1) {
            FILE_SEQ_DEBUG("[ERROR][LINE:%d]\n", __LINE__);
            return ;
        }

        u8 ret = ufs_open(&video_fp, p_filename1,  UFS_READ);

        if (ret == FR_OK) {
            FILE_SEQ_LOG("open[%s] OK\n", p_path);
            tmp_buf = (char *)mtos_malloc(1024);
            memset(tmp_buf, 0, 1024);
            int r = 0;
            ufs_read(&video_fp, tmp_buf, 1024, (u32 *) &r);
            ufs_close(&video_fp);
            get_av_ts_pid(tmp_buf);
            p_path = strstr(tmp_buf, "vpid:");

            if (p_path) {
                p_path -= 2;
                memset(p_path, 0, 1024 - (p_path - tmp_buf));
            }

            p_path = tmp_buf;
            FILE_SEQ_DEBUG("RTMP URL: %s, special_vpid %d,special_apid %d\n", p_path, special_ts_vpid, special_ts_apid);
        } else {
            FILE_SEQ_ERROR("[%s][ERROR][%d] open %s FAILED!!!!!!!!!\n", __func__, p_path, __LINE__);
            return;
        }
    }

#if 0//test   
     if (strstr(p_path, "http://")) {
     
		FILE_SEQ_LOG("[%s] http URL, add fifo: at the head\n",__func__);
		path_len = strlen(p_path);
		tmp_buf = (char *)mtos_malloc(path_len);
	       memset(tmp_buf, 0, path_len);
	       strcpy(tmp_buf,"fifo:");
	       strcpy(tmp_buf+5, p_path);
	       FILE_SEQ_LOG("[%s] 1 p_path[%s]\n", __func__, p_path);
	       p_path = tmp_buf;
	       FILE_SEQ_LOG("[%s] 2 p_path[%s]\n", __func__, p_path);

	}
#endif
     
    //MT_ASSERT(p_file_seq != NULL);
    NETPLAY_REAL_URL info;
    memset(&info, 0x00, sizeof(info));
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);

    if (p_path && strlen(p_path) < LOCAL_FILE_PATH_LEN) {
        char * descrypt_path = NULL;

        if (p_file_seq->m_path[0] == NULL) {
            p_file_seq->m_path[0] = mtos_malloc(LOCAL_FILE_PATH_LEN);
        }

        memset(p_file_seq->m_path[0], 0, LOCAL_FILE_PATH_LEN);
#ifndef __LINUX__

        if (p_file_seq->needTranslateUrl == FALSE) {
            FILE_SEQ_DEBUG("[%s] %d : needTranslateUrl Is False !!!\n", __func__, __LINE__);
            strcpy(p_file_seq->m_path[0], p_path);
            p_file_seq->total_path = 1;
            p_file_seq->path_id = path_id;

            if (tmp_buf) {
                mtos_free(tmp_buf);
                tmp_buf = NULL;
            }

            mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
            return ;
        }

#ifdef WITH_TCPIP_PROTOCOL

        if (NULL != (descrypt_path = strstr(p_path, "http://www.youtube.com/"))) {
            if (strstr(p_path, "live") == NULL) {
                char path_index;
                char * fs_buf_bak;
                char * str1, *str2;
                char * fs_buf;
                fs_buf_bak = mtos_malloc(TMP_BUF_MAX_LEN);
                fs_buf = fs_buf_bak;
                memset(fs_buf, 0, TMP_BUF_MAX_LEN);
                FILE_SEQ_DEBUG("\n ~~~ %d    %c    ~~\n", path_index, path_index);
                strcpy(&path_index, p_path + strlen(p_path) - 1);
                FILE_SEQ_DEBUG("\n ~~~ %d    %c    ~~\n", path_index, path_index);
                {
                    ufs_file_t * fp1 = mtos_malloc(sizeof(ufs_file_t));//= stream->fd;
                    memset(fp1, 0, sizeof(ufs_file_t));
                    FILE_SEQ_DEBUG("[%s] %d\n", __func__, __LINE__);
                    unsigned short  path_tmp[256] = {0};
                    unsigned short * p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)"r:raw_url.txt", path_tmp);
                    FILE_SEQ_DEBUG("[%s] %d\n", __func__, __LINE__);

                    if (ufs_open(fp1, p_filename1, UFS_READ | UFS_WRITE) != FR_OK) {
                        FILE_SEQ_DEBUG("[%s] fail to open local file !!!\n", __func__);
                        mtos_free(fp1);
                        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
                        return ;
                    }

                    OS_PRINTF("[%s] %d\n", __func__, __LINE__);
                    int r = 0;
                    FILE_SEQ_DEBUG("[%s] %d\n", __func__, __LINE__);
                    ufs_read(fp1, fs_buf, TMP_BUF_MAX_LEN, (u32 *)&r);
                    FILE_SEQ_DEBUG("[%s] %d  r %d\n", __func__, __LINE__, r);
                    ufs_close(fp1);
                    mtos_free(fp1);
                }
                FILE_SEQ_DEBUG("[%s] %d\n", __func__, __LINE__);

                do {
                    str1 =  strstr(fs_buf, "<film>");
                    str2 = strstr(fs_buf, "</film>");
                    fs_buf = strstr(str2, "<film>");
                    FILE_SEQ_DEBUG("[%s] %d  %s\n", __func__, __LINE__, str1);
                } while ((path_index--) > 48);

                FILE_SEQ_DEBUG("[%s] %d %x %x %d\n", __func__, __LINE__, p_file_seq->m_path[0], str1 + strlen("<film>"), strlen(str1) - strlen(str2) - strlen("<film>"));
                memcpy(p_file_seq->m_path[0], str1 + strlen("<film>"), strlen(str1) - strlen(str2) - strlen("<film>"));
                FILE_SEQ_DEBUG("[%s] %d %x %x %d\n", __func__, __LINE__, p_file_seq->m_path[0], str1 + strlen("<film>"), strlen(str1) - strlen(str2) - strlen("<film>"));
                mtos_free(fs_buf_bak);
                FILE_SEQ_DEBUG("[%s] %d\n", __func__, __LINE__);
                //strcpy(p_file_seq->m_path, p_path+strlen("http://www.youtube.com/"));
            }

            strcpy(p_file_seq->m_path[0], p_path);
            p_file_seq->total_path = 1;
            p_file_seq->path_id = path_id;
        } else if (is_netplay_url(p_path) == 1) {
            FILE_SEQ_DEBUG("Multi-URLs get %s\n", p_path);
            netplay_get_real_url(p_path, &info);

            for (i = 0; i < info.real_url_count; i++) {
                //strcpy(tmp_multi_urls[i],info.real_url[i]);
                tmp_multi_urls_array[i] = info.real_url[i];
                FILE_SEQ_DEBUG("\n[%s] -- url %d: %s\n", __func__, i, tmp_multi_urls_array[i]);
            }
        } else
#endif
#endif
            strcpy(p_file_seq->m_path[0], p_path);

        p_file_seq->path_id = path_id;
        p_file_seq->total_path = 1;
    } else {
        FILE_SEQ_DEBUG("[%s][ERROR] fail to set file path !!!\n", __func__);
    }

    FILE_SEQ_DEBUG("[%s] %d\n", __func__, __LINE__);
    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    FILE_SEQ_DEBUG("Multi-URLs number %d\n", info.real_url_count);
#if  1//(!defined(NETWORK_PLAYER_DISABLE))
#ifdef  WITH_TCPIP_PROTOCOL

    if (info.real_url_count > 0) {
        p_file_seq->total_path = info.real_url_count;
        if(p_file_seq->total_path > 1){
            malloc_preload_buffer((void*)(p_file_seq));
        }
        file_seq_set_muti_path(p_file_seq, tmp_multi_urls_array, info.real_url_count);
#ifndef __LINUX__	
        netplay_free_real_url(&info);
#endif
    }

#endif

    if (isNetworkStream(p_file_seq) == FALSE) {
        io_stream_buffer_size = 256 * 1024;
        io_isnetworkstream = 0;
    } else
        //yliu modify to mplayer default value
    {
        io_stream_buffer_size = 2 * 1024;
        io_isnetworkstream = 1;
    }
	if(((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->is_timeshift ==1)
		io_stream_buffer_size = 256 * 1024;
num_fill_buffer = 0;
#else
    //add by ybc for usb player
    io_stream_buffer_size = 256 * 1024;
    io_isnetworkstream = 0;
#endif

    if (tmp_buf) {
        mtos_free(tmp_buf);
        tmp_buf = NULL;
    }

    
    mtos_printk("[%s][success] m_path[%s][%ld]\n", __func__, p_file_seq->m_path[0], path_id);
    FILE_SEQ_LOG("[%s] end end ...\n", __func__);
}

static void file_seq_set_path(void * p_handle, char * p_play_path)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;

    char * p_path = p_play_path;
    char * tmp_buf = NULL;
    char * tmp_buf2 = NULL;
    char  tmp_multi_urls[50][MULTI_URL_LEN];
    int i;
    u8 ret = 0;
    file_seq_set_path_ex(p_file_seq,p_play_path,0);
    return;
}
/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
extern int atoi(__const char * __nptr);
static void file_seq_set_muti_path(void * p_handle, char * p_path[],  u32 number)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);
    int i = 0;
    //char tmp_url[50][MULTI_URL_LEN];
    //memset(tmp_url,0,50*MULTI_URL_LEN);
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);
    // for(i=0;i<number;i++)
    //  {
    //   memcpy(tmp_url[i],p_path[i],strlen(p_path[i]));
    //  OS_PRINTF("\n%s %d %s \n",__func__,__LINE__,p_path[i]);
    //   }
    file_seq_set_ott_playmode(p_file_seq, OTT_VOD_MODE);
    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);

    if (p_file_seq->total_path > 0) {
        for (i = 0; i < p_file_seq->total_path; i++) {
            if (p_file_seq->m_path[i]) {
                mtos_free(p_file_seq->m_path[i]);
                p_file_seq->m_path[i] = NULL;
            }
        }

        if (p_file_seq->p_m_duration) {
            mtos_free(p_file_seq->p_m_duration);
            p_file_seq->p_m_duration = NULL;
        }
    }

    memset(p_file_seq->m_path, 0x00, sizeof(p_file_seq->m_path));
    p_file_seq->total_path = number;

    if (NULL != (p_file_seq->p_m_duration = mtos_malloc(p_file_seq->total_path * sizeof(int)))) {
        memset(p_file_seq->p_m_duration, 0, p_file_seq->total_path * sizeof(int));
    } else {
        mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
        return ;
    }

    for (i = 0; i < number; i++) {
        p_file_seq->m_path[i] = mtos_malloc(LOCAL_FILE_PATH_LEN);

        if (p_file_seq->m_path[i] == NULL) {
            mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
            return ;
        }

        //OS_PRINTF("@@ %d  %s\n", i, p_path[i]);
        if (p_path[i] && strlen(p_path[i]) < LOCAL_FILE_PATH_LEN) {
            char * p_duration = NULL;
            memset(p_file_seq->m_path[i], 0, LOCAL_FILE_PATH_LEN);
            strcpy(p_file_seq->m_path[i], p_path[i]);

            if (NULL != (p_duration = strstr(p_path[i], "mtvoddur="))) {
                p_file_seq->p_m_duration[i] = atoi(p_duration + strlen("mtvoddur="));
                // mtos_printk("\n%s %d %d\n",__func__,__LINE__,p_file_seq->p_m_duration[i]);
            }
        } else {
            mtos_free(p_file_seq->m_path[i]);
            FILE_SEQ_DEBUG("[%s][ERROR] fail to set file path !!!\n", __func__);
        }
    }

    io_stream_buffer_size = 2 * 1024;
    io_isnetworkstream = 1;
    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    FILE_SEQ_LOG("[%s] m_path[%s]\n", __func__, p_file_seq->m_path[0]);
    FILE_SEQ_LOG("[%s] end end ...\n", __func__);
}
/*
 *
 *
 *
 *
 *
 *
 *
 *
 */




/*
 *
 *
 *
 *
 *
 *  only on  , file_seq_mem_xxx will be used
 *   on linux, these APIs will not be implemented
 *
 */

#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))

static void file_seq_mem_init(void * p_handle, u8 * p_ext_buf_addr, u32 ext_buf_size)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);
    FILE_SEQ_DEBUG("[%s] p_ext_buf_addr[0x%x]...\n", __func__, p_ext_buf_addr);
    FILE_SEQ_DEBUG("[%s] ext_buf_size[%ld]...\n", __func__, ext_buf_size);

    if (p_ext_buf_addr != NULL
    && ext_buf_size != 0) {
        memset(p_file_seq->p_ext_heap_hdl, 0, sizeof(lib_memp_t));

        if (lib_memp_create(p_file_seq->p_ext_heap_hdl, (u32)p_ext_buf_addr, ext_buf_size) != SUCCESS) {
            MT_ASSERT(0);
        }

        p_file_seq->use_ext_heap = TRUE;
    } else {
        p_file_seq->use_ext_heap = FALSE;
    }
}
/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static void file_seq_mem_release(void * p_handle)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);

    if (p_file_seq->use_ext_heap) {
        lib_memp_destroy(p_file_seq->p_ext_heap_hdl);
    }

    p_file_seq->use_ext_heap = FALSE;
}
/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
void * file_seq_mem_alloc(void * p_handle, u32 size)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);
    void * p_buf = NULL;

    if (p_file_seq->use_ext_heap) {
        p_buf = lib_memp_alloc(p_file_seq->p_ext_heap_hdl, size);
    } else {
        p_buf = mtos_malloc(size);
    }

    //MT_ASSERT(p_buf != NULL);
    return p_buf;
}
/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
void file_seq_mem_free(void * p_handle, void * p_buf)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    MT_ASSERT(p_file_seq != NULL);
    RET_CODE ret = SUCCESS;

    if (p_file_seq->use_ext_heap) {
        ret = lib_memp_free(p_file_seq->p_ext_heap_hdl, p_buf);
        MT_ASSERT(ret == SUCCESS);
    } else {
        mtos_free(p_buf);
    }
}

#endif
/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static void file_seq_register_cb(void * p_handle, void * p_cb)
{
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    p_file_seq->event_cb = p_cb;
    FILE_SEQ_LOG("[%s] end end ...\n", __func__);
}
/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static void file_seq_set_tv_sys(void * p_handle, BOOL auto_set_tv_sys)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    p_file_seq->tv_sys_auto_set = auto_set_tv_sys;
}
/*
 *
 *
 *
 *
 */


/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static FILE_SEQ_CHIPTYPE get_chip_type()
{
    FILE_SEQ_CHIPTYPE chip_type = 0;
    int hal_chip_type = hal_get_chip_ic_id();

    if (hal_chip_type == IC_WARRIORS ) {
        chip_type = CHIPTYPE_WARRIORS;
    }

    if (hal_chip_type == IC_SONATA) {
        chip_type = CHIPTYPE_SONATA;
    }

    if (hal_chip_type == IC_CONCERTO) {
        chip_type = CHIPTYPE_CONCERTO;
    }

    mtos_printk("\n%s %d chip type:%d\n", __func__, __LINE__, chip_type);
    return chip_type;
}
void * file_seq_create(PB_SEQ_PARAM_T * p_param)
{
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);
    FILE_SEQ_LOG("[%s] [%s] ...\n", __func__, p_hg_ver);
#if defined(WARRIORS_3_0)
    FILE_SEQ_LOG("[%s] [MainBranchDev_Drv:warriors_3.0]...\n", __func__);
#elif defined(DRV_DEFAULT)
    FILE_SEQ_LOG("[%s] [MainBranchDev_Drv:default]...\n", __func__);
#elif  defined(SDK_2_0)
    FILE_SEQ_LOG("[%s] [MainBranchDev_Drv:Flounder_SDK_2.0]...\n", __func__);
#elif  defined(SDK_4_0)
    FILE_SEQ_LOG("[%s] [MainBranchDev_Drv:sonata_sdk_4.0]...\n", __func__);
#endif
#if defined(WITH_TCPIP_PROTOCOL)
    FILE_SEQ_LOG("[%s] Player Support Network Stream  !!!!\n", __func__);
#else
    FILE_SEQ_LOG("[%s] Warning: Player Do Not Support Network Stream !!!!\n", __func__);
    FILE_SEQ_LOG("[%s] Warning: Please Check Your Platform  !!!!\n", __func__);
#endif
    BOOL ret = FALSE;

    if (p_param) {
        FILE_SEQ_LOG("[%s] pb_seq_mem_size[%ld] ...\n", __func__, p_param->pb_seq_mem_size);
        FILE_SEQ_LOG("[%s] pb_seq_mem_start[0x%x] ...\n", __func__, p_param->pb_seq_mem_start);
        FILE_SEQ_LOG("[%s] stack_start[0x%x] ...\n", __func__, p_param->stack_start);
        FILE_SEQ_LOG("[%s] stack_size[%ld] ...\n", __func__, p_param->stack_size);
        FILE_SEQ_LOG("[%s] task_priority[%ld] ...\n", __func__, p_param->task_priority);
        FILE_SEQ_LOG("[%s] stack_preload_size[%ld] ...\n", __func__, p_param->stack_preload_size);
        FILE_SEQ_LOG("[%s] task_preload_priority[%ld] ...\n", __func__, p_param->task_preload_priority);
        FILE_SEQ_LOG("[%s] vdec_policy[%ld] ...\n", __func__, p_param->vdec_policy);
    } else {
        FILE_SEQ_ERROR("[%s] do nothing ...\n", __func__);
        MT_ASSERT(0);
        //return NULL;
    }

#if  0//def WITH_OTT_DATA
    srand(mtos_ticks_get());
#endif
    FILE_SEQ_T * p_handle = (FILE_SEQ_T *)mtos_malloc(sizeof(FILE_SEQ_T));

    if (p_handle) {
        memset(p_handle, 0, sizeof(FILE_SEQ_T));
	p_handle->task_prio = p_param->task_priority;
	p_handle->hls_server_prio = p_param->hls_server_prio;
	p_handle->hls_server_size = p_param->hls_server_size;
	p_handle->p2p_server_recv_prio = p_param->p2p_server_recv_prio;
	p_handle->p2p_server_recv_size = p_param->p2p_server_recv_size;
	p_handle->p2p_server_deal_prio = p_param->p2p_server_deal_prio;
	p_handle->p2p_server_deal_size = p_param->p2p_server_deal_size;
	p_handle->p2p_server_send_prio = p_param->p2p_server_send_prio;
	p_handle->p2p_server_send_size = p_param->p2p_server_send_size;
	p_handle->p2p_server_monitor_prio = p_param->p2p_server_monitor_prio;
	p_handle->p2p_server_monitor_size = p_param->p2p_server_monitor_size;
	p_handle->p2p_server_bw_prio = p_param->p2p_server_bw_prio;
	p_handle->p2p_server_bw_size = p_param->p2p_server_bw_size;
	p_handle->p2p_server_bw_prio = p_param->p2p_server_bw_prio;
	p_handle->p2p_server_bw_size = p_param->p2p_server_bw_size;
	p_handle->vod_heartbeat_login_prio = p_param->vod_heartbeat_login_prio;
	p_handle->vod_heartbeat_login_size = p_param->vod_heartbeat_login_size;
	if (p_param->net_task_priority != 0) {
		p_handle->net_task_prio = p_param->net_task_priority;
	} else {
		p_handle->net_task_prio = 128;
	}

        p_handle->preload_audio_buffer_malloc_internal = 0;
        p_handle->preload_video_buffer_malloc_internal = 0;
#ifndef SWITCH_DEBUG
        p_handle->net_task_prio = p_handle->task_prio;
#endif
        p_handle->task_size = p_param->stack_size;
        p_handle->task_preload_prio = p_param->task_preload_priority;
        p_handle->task_preload_size = p_param->stack_preload_size;

#ifdef USE_PB_FIFO	        
        p_handle->task_pb_fifo_prio = p_param->task_pbfifo_priority;
        p_handle->task_pb_fifo_size = p_param->stack_pbfifo_size;
#endif        

        p_handle->preload_audio_buffer_size = p_param->preload_audio_buffer_size;
        p_handle->preload_video_buffer_size = p_param->preload_video_buffer_size;
        p_handle->cur_speed = 0;
        p_handle->audio_output_mode = p_param->audio_output_mode;
        p_handle->vdec_policy = p_param->vdec_policy;
        p_handle->camera_open = p_param->camera_open;
        p_handle->camera_read = p_param->camera_read;
        p_handle->camera_close = p_param->camera_close;
#ifdef   ENABLE_DEMUX_RTSP
        p_handle->rtsp_video_rec = file_seq_rtsp_video_rec;
#endif
        p_handle->loadmedia = file_seq_loadmedia;
        p_handle->loadmedia_task = file_seq_loadmedia_task;
        p_handle->loadmedia_times = file_seq_loadmedia_task;
        p_handle->unloadmedia = x_unLoadMedia;
        p_handle->register_event_cb = file_seq_register_cb;
        p_handle->start = file_seq_start;
        p_handle->preview = file_seq_preview;
        p_handle->stop = file_seq_stop;
        p_handle->force_stop = file_seq_force_stop;
        p_handle->pause = file_seq_pause;
        p_handle->set_speed = file_seq_set_speed;
        p_handle->resume = file_seq_resume;
        p_handle->check_bg_task_alive = file_seq_is_fill_es_task_alive;
        p_handle->set_file_path = file_seq_set_path;
        p_handle->set_file_path_ex = file_seq_set_path_ex;
        p_handle->set_muti_file_path = file_seq_set_muti_path;
        p_handle->get_film_info = file_seq_get_film_info;
        p_handle->change_video_track = file_seq_change_video_track;
        p_handle->change_audio_track = file_seq_change_audio_track;
        p_handle->get_audio_track_lang = file_seq_get_audio_track_lang;
        p_handle->mp_dump = file_seq_mp_dump;
        p_handle->mp_dumpaudio = file_seq_mp_dump_aud;
        p_handle->get_status = file_seq_get_status;
        p_handle->get_vpts = file_seq_get_cur_vpts;
        p_handle->get_subt = file_seq_get_subt;
        p_handle->get_subt_info = file_seq_get_subt_info;
        p_handle->set_subt_id = file_seq_set_subt_id;
        p_handle->play_at_time = file_seq_playAtTime;
        p_handle->set_tv_sys = file_seq_set_tv_sys;
        p_handle->set_ott_playmode = file_seq_set_ott_playmode;
        p_handle->is_network_stream = isNetworkStream;
        p_handle->set_live_broadcast = file_seq_set_live_broadcast;
		p_handle->set_vdec_stop_type = file_seq_set_vdec_stop_type;
        /*initialize method for allocate/free memory */
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        FILE_SEQ_DEBUG("[%s] Use External Heap allocated by upper layer !!!\n ", __func__);
        p_handle->file_seq_mem_size = p_param->pb_seq_mem_size;
        p_handle->file_seq_mem_start = p_param->pb_seq_mem_start;
        p_handle->mem_init = file_seq_mem_init;
        p_handle->mem_alloc = file_seq_mem_alloc;
        p_handle->mem_free = file_seq_mem_free;
        p_handle->mem_release = file_seq_mem_release;
        p_handle->p_ext_heap_hdl = (lib_memp_t *)mtos_malloc(sizeof(lib_memp_t));
        FILE_SEQ_DEBUG("[%s] p_ext_heap_hdl[0x%x]\n", __func__, p_handle->p_ext_heap_hdl);
        //MT_ASSERT(p_handle->p_ext_heap_hdl);
        memset(p_handle->p_ext_heap_hdl, 0, sizeof(lib_memp_t));
#endif
        PLAYBACK_INTERNAL_T * p_pb_internal = NULL;
        p_handle->pb_internal = p_pb_internal = (PLAYBACK_INTERNAL_T *) mtos_malloc(sizeof(PLAYBACK_INTERNAL_T));
        memset(p_pb_internal, 0, sizeof(PLAYBACK_INTERNAL_T));
        p_pb_internal->chip_type = get_chip_type();

	 p_pb_internal->task_idle = p_param->task_idle;
#ifdef ES_END_IN_TASK
        p_pb_internal->vitual_video_es_max_size = 3 * 1024 * 1024;
        p_pb_internal->vitual_video_es_buf = mtos_malloc(p_pb_internal->vitual_video_es_max_size);
        memset(p_pb_internal->vitual_video_es_buf, 0, p_pb_internal->vitual_video_es_max_size);
        p_pb_internal->vitual_video_es_pos = p_pb_internal->vitual_video_es_buf;
        p_pb_internal->vitual_audio_es_max_size = 128 * 1024;
        p_pb_internal->vitual_audio_es_buf = mtos_malloc(p_pb_internal->vitual_audio_es_max_size);
        p_pb_internal->vitual_audio_es_pos  = p_pb_internal->vitual_audio_es_buf ;
        memset(p_pb_internal->vitual_audio_es_buf, 0, p_pb_internal->vitual_audio_es_max_size);
        p_pb_internal->vitual_audio_size = 0;
#endif
#ifdef __LINUX__
        p_handle->check_mem = run_memory;
        u32    value = reg_get_u32(0xbfef0720);//bit26   0:with time info
#else
        u32    value = 0;

        if (p_pb_internal->chip_type == CHIPTYPE_WARRIORS) {
            value = *(volatile unsigned int *)(0xbfef0720);
        } else if (p_pb_internal->chip_type == CHIPTYPE_SONATA) {
        } else if (p_pb_internal->chip_type == CHIPTYPE_CONCERTO) {
            value = 0  ;
        }

        backup_avsync_status = value;
#endif
        ret = mtos_sem_create(&(p_handle->lock), TRUE);
        MT_ASSERT(ret == TRUE);
        ret = mtos_sem_create(&(p_handle->sub_fifo_mutex), TRUE);
        MT_ASSERT(ret == TRUE);

#ifdef USE_PB_FIFO	        
        ret = mtos_sem_create(&(p_handle->pb_fifo_mutex), TRUE);
        MT_ASSERT(ret == TRUE);
#endif        

#ifndef __LINUX__
        /*focrce to enable av sync bit*/
        FILE_SEQ_LOG("[%s]----------value[0x%x]\n", __func__, value);

        if (p_pb_internal->chip_type == CHIPTYPE_WARRIORS) {
            value = value & 0xfbffffff;
            *(volatile unsigned int *)(0xbfef0720) = value;
        } else if (p_pb_internal->chip_type == CHIPTYPE_SONATA) {
        } else if (p_pb_internal->chip_type == CHIPTYPE_CONCERTO) {
        }

        FILE_SEQ_LOG("[%s]----------value[%x]\n", __func__, value);
#endif
        p_handle->disable_avsync = ((value & 0x04000000) != 0) ? TRUE : FALSE;
        FILE_SEQ_LOG("[%s] disable_avsync[%d]\n", __func__, p_handle->disable_avsync);
        p_handle->m_play_state = FILE_SEQ_STOP;
        memset(p_handle->m_path, 0x00, MAX_PATH_NUM * sizeof(int));
        g_player_handle = p_handle;
    } else {
        FILE_SEQ_ERROR("[%s] fail to malloc FILE_SEQ_T!!!\n", __func__);
        return NULL;
    }

#ifndef __LINUX__

    /*allocate memory for fill_es_task as stack*/
    if (p_param->stack_size) {
#if defined(WITH_TCPIP_PROTOCOL)
        // p_handle->pstack = mtos_malloc(p_param->stack_size);
        //MT_ASSERT(p_handle->pstack != NULL);
        //memset(p_handle->pstack,0,p_param->stack_size);
        //p_handle->p_loadstack = mtos_malloc(p_param->stack_size);
        //MT_ASSERT(p_handle->p_loadstack != NULL);
        //memset(p_handle->p_loadstack,0,p_param->stack_size);
#else

        if (p_param->stack_start) {
            p_handle->pstack = p_param->stack_start;
            p_handle->net_task_prio = p_handle->task_prio;
            //p_handle->p_loadstack = mtos_malloc(p_param->stack_size);
            p_handle->p_loadstack = p_handle->pstack;
            MT_ASSERT(p_handle->p_loadstack != NULL);
            memset(p_handle->p_loadstack, 0, p_param->stack_size);
        } else {
            p_handle->net_task_prio = p_handle->task_prio;
            FILE_SEQ_ERROR("[%s] hi please config memory resource for internal stack !!!!!\n", __func__);
            FILE_SEQ_ERROR("[%s] Thanks A lot  !!!!!\n", __func__);
            //MT_ASSERT(FALSE);
        }

#endif
    }

    /*allocate memory for preload_es task  as stack*/
    if (p_handle->task_preload_size) {
        p_handle->p_preloadstack = mtos_malloc(p_handle->task_preload_size);
        MT_ASSERT(p_handle->p_preloadstack != NULL);
        memset(p_handle->p_preloadstack, 0, p_handle->task_preload_size);
    }

#ifdef USE_PB_FIFO	
     /*allocate memory for pb_fifo_task  as stack*/
    if (p_handle->task_pb_fifo_size) {
        p_handle->p_pbfifostack = mtos_malloc(p_handle->task_pb_fifo_size);
        MT_ASSERT(p_handle->p_pbfifostack != NULL);
        memset(p_handle->p_pbfifostack, 0, p_handle->task_pb_fifo_size);
    }
#endif

#endif
    memset(&p_handle->preload_audio_buffer, 0x00, sizeof(preload_buffer_t));
    memset(&p_handle->preload_video_buffer, 0x00, sizeof(preload_buffer_t));

    if (p_handle->preload_audio_buffer_size) {
        p_handle->preload_audio_buffer.buffer_start = p_param->preload_audio_buffer;
        p_handle->preload_audio_buffer_malloc_internal = 0;
        MT_ASSERT(p_handle->preload_audio_buffer.buffer_start != NULL);
    }else{
        p_handle->preload_audio_buffer_malloc_internal = 1;
    }

    if (p_handle->preload_video_buffer_size) {
        p_handle->preload_video_buffer.buffer_start = p_param->preload_video_buffer;
        p_handle->preload_video_buffer_malloc_internal = 0;
        MT_ASSERT(p_handle->preload_video_buffer.buffer_start != NULL);
    }else{
        p_handle->preload_video_buffer_malloc_internal = 1;
    }

    p_handle->preload_audio_buffer.buffer_len = p_handle->preload_audio_buffer_size;
    p_handle->preload_video_buffer.buffer_len = p_handle->preload_video_buffer_size;

    if (p_param->is_direct_url) {
        p_handle->needTranslateUrl = FALSE;
    } else {
        p_handle->needTranslateUrl = TRUE;
    }

    p_handle->p_buffering = (BUFFERING_PARAM_T *)mtos_malloc(sizeof(BUFFERING_PARAM_T));
    memset(p_handle->p_buffering, 0, sizeof(BUFFERING_PARAM_T));
#if 0
    /*initalize audio buffering packet*/
    p_handle->p_audio_buffering_packet_list = audioBuffering_create_list();
#endif
    p_handle->cmd_fifo = (FP_USER_CMD_T *)mtos_malloc(USER_CMD_FIFO_LEN * sizeof(FP_USER_CMD_T));
    memset(p_handle->cmd_fifo, 0, USER_CMD_FIFO_LEN * sizeof(FP_USER_CMD_T));
    FILE_SEQ_LOG("[%s] stop stop ...\n", __func__);
    //
    //
    {
        int ves_buf_size = 0;

        if (p_handle->init_av_dev == FALSE) {
            p_handle->init_av_dev = TRUE;
            x_init_av_device(p_handle);
        }

        vdec_freeze_stop(p_handle->p_vdec_dev);
        vdec_get_es_buf_size(p_handle->p_vdec_dev, (u32 *)&ves_buf_size);
        p_handle->max_ves_size = ves_buf_size;
        x_get_av_dec_cap(p_handle);
        ret = x_map_es_buffer(p_handle);
    }
    //
#ifdef ES_END_IN_TASK

    while (p_handle->check_task_state != CHECK_ES_FALSE) {
        mtos_task_sleep(100);
    }

    if (p_handle->task_checkes_prio == 0) {
        p_handle->task_checkes_prio = 235;
    }

    p_handle->task_checkes_size = 32 * 1024;
    p_handle->p_checkesstack = mtos_malloc(p_handle->task_checkes_size);
    mtos_task_delete(p_handle->task_checkes_prio);
    p_handle->check_task_state = CHECK_ES_FALSE;
    mtos_task_create((u8 *)"check es thread", check_es_thread, p_handle,
                     p_handle->task_checkes_prio, p_handle->p_checkesstack,
                     p_handle->task_checkes_size);

    while (p_handle->check_task_state != CHECK_ES_TRUE) {
        mtos_task_sleep(100);
    }

    p_handle->check_task_state = CHECK_ES_IDLE;
#endif
    return (void *)p_handle;
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
void  file_seq_destroy()
{
    FILE_SEQ_LOG("[%s] start start ...\n", __func__);
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    file_seq_force_stop(p_file_seq);
    vdec_set_buf(p_file_seq->p_vdec_dev, p_file_seq->vdec_policy, g_pFwMemCfg.p_vdec_start);

    if (p_file_seq->is_task_alive ||
    (p_file_seq->m_play_state != FILE_SEQ_STOP) || p_file_seq->is_load_task_alive || p_file_seq->is_preload_task_alive || p_file_seq->is_pbfifo_task_alive) {
        int i = 0;

        while (1) {
            if (p_file_seq->is_task_alive || p_file_seq->is_load_task_alive || p_file_seq->is_preload_task_alive || p_file_seq->is_pbfifo_task_alive) {
                i++;

#ifndef CHIP_CONCERTO

#ifdef WITH_TCPIP_PROTOCOL

                if (p_file_seq->is_load_task_alive) {
                    Abort_Download_Task(p_file_seq->net_task_prio, TRUE);
                }


#endif

#endif
                mtos_task_sleep(200);

                if (i % 5 == 0) {
                    FILE_SEQ_LOG("[%s] wait fill es task exit ...!!!!\n", __func__);
                    FILE_SEQ_LOG("[%s] task alive[%d,%d,%d,%d], m_play_state:%d\n", __func__,p_file_seq->is_task_alive, p_file_seq->is_load_task_alive, p_file_seq->is_preload_task_alive, p_file_seq->is_pbfifo_task_alive,p_file_seq->m_play_state);
                    FILE_SEQ_LOG("wait %d sec\n", ((i + 1) * 200) / 1000);
                }
            } else {
                break;
            }
        }

        FILE_SEQ_DEBUG("[%s] %d...\n", __func__, __LINE__);

        if (p_file_seq->total_path > 1) {
            while (1) {
                if (p_file_seq->is_preload_task_alive) {
                    mtos_task_sleep(100);
                    FILE_SEQ_LOG("[%s] wait preload fill es task exit ...!!!!\n", __func__);
                } else {
                    break;
                }
            }
        }

        FILE_SEQ_DEBUG("[%s] %d...\n", __func__, __LINE__);

        if (p_file_seq->total_path > 0) {
            int i = 0;

            for (i = 0; i < p_file_seq->total_path; i++) {
                mtos_free(p_file_seq->m_path[i]);
                p_file_seq->m_path[i] = NULL;
            }
        }

        //return ;
    }

    //
    {
        PLAYBACK_INTERNAL_T * p_pb_internal =  p_file_seq->pb_internal;

        if (p_pb_internal) {
            if (p_pb_internal->vitual_video_es_buf) {
                mtos_free(p_pb_internal->vitual_video_es_buf);
                p_pb_internal->vitual_video_es_buf = NULL;
            }

            if (p_pb_internal->vitual_audio_es_buf) {
                mtos_free(p_pb_internal->vitual_audio_es_buf);
                p_pb_internal->vitual_audio_es_buf = NULL;
            }

#ifndef __LINUX__
            u32    value = 0;

            if (p_pb_internal->chip_type == CHIPTYPE_WARRIORS) {
                value = *(volatile unsigned int *)(0xbfef0720);//bit26   0:with time info
                FILE_SEQ_LOG("[%s]----------value[0x%x]\n", __func__, value);
                value = value | 0x04000000;
                *(volatile unsigned int *)(0xbfef0720) = backup_avsync_status;
            } else if (p_pb_internal->chip_type == CHIPTYPE_SONATA) {
            } else if (p_pb_internal->chip_type == CHIPTYPE_CONCERTO) {
            }

#endif
            mtos_free(p_pb_internal);
            p_pb_internal = NULL;
        }
    }
#ifdef ES_END_IN_TASK
    p_file_seq->check_task_state = CHECK_ES_EXIT;

    while (p_file_seq->check_task_state !=  CHECK_ES_FALSE) {
        mtos_task_sleep(100);
    }

    if (p_file_seq->p_checkesstack) {
        mtos_free(p_file_seq->p_checkesstack);
    }

    mtos_task_delete(p_file_seq->task_checkes_prio);
#endif
    //
    x_unmap_es_buffer(p_file_seq);
    //
    /*destroy all the url paths*/
    int i = 0;

    for (i = 0; i < MAX_PATH_NUM; i++) {
        if (p_file_seq->m_path[i]) {
            mtos_free(p_file_seq->m_path[i]);
            p_file_seq->m_path[i] = NULL;
        }
    }

    if (p_file_seq->loadMedaiOK) {
        x_unLoadMedia(p_file_seq);
    }

    if (p_file_seq->p_sub_fifo_handle) {
		void * p_start_pos = NULL;
		p_start_pos = ((fifo_type_t *)(p_file_seq->p_sub_fifo_handle))->start_pos;
		if(p_start_pos)
		{
			#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        	file_seq_mem_free(p_file_seq, p_start_pos);
			#else
        	mtos_free(p_start_pos);
			#endif
			((fifo_type_t *)(p_file_seq->p_sub_fifo_handle))->start_pos = 0;			
		}		
        deinit_fifo_kw(p_file_seq->p_sub_fifo_handle);
        p_file_seq->p_sub_fifo_handle = NULL;
    }
    
#ifdef USE_PB_FIFO	    
     if (p_file_seq->p_pb_fifo_handle) {
        deinit_fifo_kw(p_file_seq->p_pb_fifo_handle);
        p_file_seq->p_pb_fifo_handle = NULL;
    }
#endif

     if (p_file_seq->p_pb_fifo_handle) {
        deinit_fifo_kw(p_file_seq->p_pb_fifo_handle);
        p_file_seq->p_pb_fifo_handle = NULL;
    }

    /*destroy internal command fifo*/
    if (p_file_seq->cmd_fifo) {
        mtos_free(p_file_seq->cmd_fifo);
        p_file_seq->cmd_fifo = NULL;
    }

    if (p_file_seq->p_buffering) {
        mtos_free(p_file_seq->p_buffering);
        p_file_seq->p_buffering = NULL;
    }

    if (p_file_seq->p_loadstack) {
        mtos_free(p_file_seq->p_loadstack);
        p_file_seq->p_loadstack = NULL;
    }

#if defined(WITH_TCPIP_PROTOCOL)

    if (p_file_seq->pstack) {
        mtos_free(p_file_seq->pstack);
    }

    p_file_seq->pstack = NULL;
#endif

    if (p_file_seq->p_preloadstack) {
        mtos_free(p_file_seq->p_preloadstack);
        p_file_seq->p_preloadstack = NULL;
    }

#ifdef USE_PB_FIFO	
    if (p_file_seq->p_pbfifostack) {
        mtos_free(p_file_seq->p_pbfifostack);
        p_file_seq->p_pbfifostack = NULL;
    }
#endif    
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
    file_seq_mem_release(p_file_seq);

    if (p_file_seq->p_ext_heap_hdl) {
        mtos_free(p_file_seq->p_ext_heap_hdl);
        p_file_seq->p_ext_heap_hdl = NULL;
    }

#endif
    mtos_sem_destroy(&(p_file_seq->lock), 0);
    mtos_sem_destroy(&(p_file_seq->sub_fifo_mutex), 0);

#ifdef USE_PB_FIFO	    
    mtos_sem_destroy(&(p_file_seq->pb_fifo_mutex), 0);
#endif    

    //audioBuffering_destroy_list(p_file_seq->p_audio_buffering_packet_list);
    free_preload_buffer((void*)p_file_seq);
    p_file_seq->preload_video_buffer.buffer_start = NULL;
    p_file_seq->preload_audio_buffer.buffer_start = NULL;

    if (p_file_seq) {
        mtos_free(p_file_seq);
        p_file_seq = NULL;
    }

    g_player_handle = NULL;
    FILE_SEQ_LOG("[%s] stop stop ...\n", __func__);
}


/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
void file_seq_set_fw_mem(unsigned int p_vdec_start,
                         unsigned int  vdec_mem_len, unsigned int p_audio_mem_start,
                         unsigned int audio_mem_len)
{
    g_pFwMemCfg.p_vdec_start = p_vdec_start;
    g_pFwMemCfg.vdec_mem_size = vdec_mem_len;
    g_pFwMemCfg.p_audio_start = p_audio_mem_start;
    g_pFwMemCfg.audio_mem_size = audio_mem_len;
    return;
}
/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
void file_seq_clear_fw_mem()
{
    memset(&g_pFwMemCfg, 0, sizeof(FW_MEM_CFG_T));
}


/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
static s8 file_seq_parser_speed(void * p_handle, s8 speed)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    int ret_speed = 0;

    switch (speed) {
        case -32:
            ret_speed = TS_SEQ_REV_FAST_PLAY_32X;

            if (ret_speed != p_file_seq->last_speed) {
                vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_FREV, 2);
            }

            break;
        case -16:
            ret_speed = TS_SEQ_REV_FAST_PLAY_16X;

            if (ret_speed != p_file_seq->last_speed) {
                vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_FREV, 2);
            }

            break;
        case -8:
            ret_speed = TS_SEQ_REV_FAST_PLAY_8X;

            if (ret_speed != p_file_seq->last_speed) {
                vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_FREV, 2);
            }

            break;
        case -4:
            ret_speed = TS_SEQ_REV_FAST_PLAY_4X;

            if (ret_speed != p_file_seq->last_speed) {
                vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_FREV, 2);
            }

            break;
        case -2:
            ret_speed = TS_SEQ_REV_FAST_PLAY_2X;

            if (ret_speed != p_file_seq->last_speed) {
                vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_FREV, 2);
            }

            break;
        case 0:
        case 1:
            ret_speed = TS_SEQ_NORMAL_PLAY;

            if (ret_speed != p_file_seq->last_speed) {
                vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_NORMAL, 1);
            }

            break;
        case 2:
            ret_speed = TS_SEQ_FAST_PLAY_2X;

            if (ret_speed != p_file_seq->last_speed) {
                vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_FFWD, 2);
            }

            break;
        case 4:
            ret_speed = TS_SEQ_FAST_PLAY_4X;

            if (ret_speed != p_file_seq->last_speed) {
                vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_FFWD, 4);
            }

            break;
        case 8:
            ret_speed = TS_SEQ_FAST_PLAY_8X;

            if (ret_speed != p_file_seq->last_speed) {
                vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_FFWD, 8);
            }

            break;
        case 16:
            ret_speed = TS_SEQ_FAST_PLAY_16X;

            if (ret_speed != p_file_seq->last_speed) {
                vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_FFWD, 16);
            }

            break;
        case 32:
            ret_speed = TS_SEQ_FAST_PLAY_32X;

            if (ret_speed != p_file_seq->last_speed) {
                vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_FFWD, 32);
            }

            break;
        default:
            ret_speed = -1;
            break;
    }

    return ret_speed;
}
/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
void file_seq_check_trickplay(void * p_handle)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    s8 ret = 0;
    int in_size = 0;
    //OS_PRINTF("%s \n",__func__);
    p_file_seq->unable_trickplay = 0;

    if (0 == ((demuxer_t *)(p_file_seq->p_demuxer))->seekable) {
        p_file_seq->unable_trickplay = 1;
        OS_PRINTF("%s trickplay unsupport \n", __func__);
        return ;
    }

    //only h264 ts file for check
    if (p_file_seq->is_ts && p_file_seq->m_video_codec_type == 1) {
        demux_seek(p_file_seq->p_demuxer, 0, 0, 1) ;
        in_size = ds_get_packet(p_file_seq->p_cur_ds_video, &(p_file_seq->p_v_pkt_start));

        if (in_size < 128) {
            p_file_seq->unable_trickplay = 1;
            OS_PRINTF("%s trickplay unsupport \n", __func__);
        }

        demux_seek(p_file_seq->p_demuxer, 0, 0, 1) ;
    }
}

/*
 *
 *
 *
 *
 *
 *
 *
 *
 */
BOOL file_seq_get_load_err_info(void *p_handle, LOAD_ERR_INFO *p_load_err_info)
{
	 FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
	 LOAD_ERR_INFO err_info;
	 FILE_SEQ_LOG("[%s] start start ...\n", __func__);
	 if(!p_handle || !p_load_err_info){
	 	FILE_SEQ_LOG("[%s] input par. error, return\n", __func__);
	 	return FALSE;
 	}
 	
 	mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
 	memset(&err_info,0,sizeof(LOAD_ERR_INFO));
	err_info.err_type = g_load_err;
	strcpy(err_info.url,p_file_seq->m_path[0]);
	memcpy(p_load_err_info, &err_info, sizeof(LOAD_ERR_INFO));
	mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
	
	 FILE_SEQ_LOG("[%s] end end ...\n", __func__);
	 return TRUE;

}