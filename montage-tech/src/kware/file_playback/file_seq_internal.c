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
//#include "audio.h"     ////not used, please aud_vsb.h instead


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
#define MT_ASSERT
#else
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_printk.h"
#endif
#include "../ts_seq/fifo_kw.h"
#include "lib_memp.h"
#include "ts_sequence.h"
#include "libmpdemux/stheader.h"

#include "stheader.h"

#include "file_playback_sequence.h"
#include "demux_mp.h"
#include  "file_seq_internal.h"
#include  "file_seq_misc.h"
#define MUL_SEEK 1
#define TEST_VIDEO  1
#define TEST_AUDIO  1
#define USE_MPLAYER_DEMUXER
#define DUMP_AUDIO 1
#define AUTO_SEND_VPTS
#define AUTO_SEND_VIDEO_INFO
#define SWITCH_DEBUG
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
#include  "file_seq_internal.h"
#include "download_api.h"

extern ERR_LOAD g_load_err;
extern int rand(void);
extern unsigned short   *  Convert_Utf8_To_Unicode(unsigned char * putf8, unsigned short * out) ;
//#define    DUMP_AUDIO_ES_WITH_HEADER
//#define    DUMP_AUDIO_ES
//#define    DUMP_VIDEO_ES


#if  defined(DUMP_AUDIO_ES) || defined(DUMP_AUDIO_ES_WITH_HEADER)
#ifdef __LINUX__
static FILE * p_dump_audio_fp = NULL;
#else
static ufs_file_t  * p_dump_audio_fp = NULL;
#endif
#endif

#ifdef  DUMP_VIDEO_ES
#ifdef __LINUX__
static FILE  * p_dump_video_fp = NULL;
#else
static ufs_file_t  * p_dump_video_fp = NULL;
#endif
#endif



static int m_load_task_lock = 1;
FW_MEM_CFG_T  g_pFwMemCfg ;
int loop_count_ves;
extern int io_isnetworkstream ;
extern int bsf_vcodec_flag;
extern FILE_SEQ_T * x_get_cur_instance();
extern int g_hls_playmode;
extern int pbfifo_task_state;
extern fifo_fill_buff(demuxer_t *demux, demux_stream_t *ds);
#if  1//def     DBG_FILE_PLAYBACK_SEQ
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
unsigned int video_codec_id;
unsigned int audio_codec_id;
unsigned int last_vpts_upload;
#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))

extern int flag_debug ;
extern int while_cnt ;
extern int debug_tmp ;
extern int total_tcp_recv_size ;
extern int compute_Bps ;
extern int a_ticks ;
extern int v_ticks ;
extern int ves_left ;
extern int aes_left ;

static int buffering_finish_cnt = 0;
#define MAX_VPACKET_SIZE (2*1024*1024)
#define  MAX_BUFFERING_AUDIO_SIZE (1*1024*1024)
double first_vpts = -2;


static int show_ticks = 0;
///

RET_CODE vdec_dec_push_vitual_es(FILE_SEQ_T * p_file_seq, u32 src_addr, u32 size, u32 vpts)
{
    RET_CODE ret = SUCCESS;
    PLAYBACK_INTERNAL_T * p_pb_internal =  p_file_seq->pb_internal;

    //mtos_printk("\n%s %d %d\n",__func__,__LINE__,size);
    if (((u32)p_pb_internal->vitual_video_es_pos + size + 2 * sizeof(u32)) >= ((u32)p_pb_internal->vitual_video_es_buf + p_pb_internal->vitual_video_es_max_size)) {
        p_pb_internal->vitual_video_es_pos = p_pb_internal->vitual_video_es_buf;
        //mtos_printk("\n%s %d vitul vdec es full\n",__func__,__LINE__);
        return ret;
    }

    memcpy(p_pb_internal->vitual_video_es_pos, &size, sizeof(u32));
    p_pb_internal->vitual_video_es_pos = (void *)((u32)p_pb_internal->vitual_video_es_pos + sizeof(u32));
    memcpy(p_pb_internal->vitual_video_es_pos, src_addr, size);
    p_pb_internal->vitual_video_es_pos = (void *)((u32)p_pb_internal->vitual_video_es_pos + size);
    memcpy(p_pb_internal->vitual_video_es_pos, (void *)&vpts, sizeof(u32));
    p_pb_internal->vitual_video_es_pos = (void *)((u32)p_pb_internal->vitual_video_es_pos + sizeof(u32));
    //mtos_printk("\n%s %d \n",__func__,__LINE__);
    return ret;
}
RET_CODE vdec_get_vitual_es_buf_space(FILE_SEQ_T * p_file_seq, u32 * p_size)

{
    RET_CODE ret = SUCCESS;
    //return ret;
    PLAYBACK_INTERNAL_T * p_pb_internal =  p_file_seq->pb_internal;
    *p_size = (p_pb_internal->vitual_video_es_buf + p_pb_internal->vitual_video_es_max_size - p_pb_internal->vitual_video_es_pos) / 1024;
    //mtos_printk("\n%s %d  size:%d\n",__func__,__LINE__,*p_size);
    return ret;
}

RET_CODE aud_file_push_vitual_esbuffer_vsb(FILE_SEQ_T * p_file_seq, u32 src_addr, u32 size, u32 apts)
{
    PLAYBACK_INTERNAL_T * p_pb_internal =  p_file_seq->pb_internal;
    RET_CODE ret = SUCCESS;
    int vitual_size = 0;

    //mtos_printk("\n%s %d \n",__func__,__LINE__);
    if (size <= (56 - p_pb_internal->vitual_audio_size % 64)) {
        p_pb_internal->vitual_audio_size += size;
    } else {
        vitual_size = size - (56 - p_pb_internal->vitual_audio_size % 64);
        p_pb_internal->vitual_audio_size = (p_pb_internal->vitual_audio_size / 64 + 1) * 64 + (vitual_size / 56) * 64 + vitual_size % 56;
    }

    if (p_pb_internal->vitual_audio_size >= p_pb_internal->vitual_audio_es_max_size) {
        //mtos_printk("\n%s %d vitul aud es full\n",__func__,__LINE__);
        return ret;
    }

    //mtos_printk("\n%s %d  pos %d  size:%d\n",__func__,__LINE__,size,p_pb_internal->vitual_audio_es_pos);
    memcpy(p_pb_internal->vitual_audio_es_pos, &size, sizeof(u32));
    //mtos_printk("\n%s %d \n",__func__,__LINE__);
    p_pb_internal->vitual_audio_es_pos = (void *)((u32)p_pb_internal->vitual_audio_es_pos + sizeof(u32));
    //mtos_printk("\n%s %d \n",__func__,__LINE__);
    memcpy(p_pb_internal->vitual_audio_es_pos, src_addr, size);
    //mtos_printk("\n%s %d \n",__func__,__LINE__);
    p_pb_internal->vitual_audio_es_pos = (void *)((u32)p_pb_internal->vitual_audio_es_pos + size);
    //mtos_printk("\n%s %d \n",__func__,__LINE__);
    memcpy(p_pb_internal->vitual_audio_es_pos, (void *)&apts, sizeof(u32));
    //mtos_printk("\n%s %d \n",__func__,__LINE__);
    p_pb_internal->vitual_audio_es_pos = (void *)((u32)p_pb_internal->vitual_audio_es_pos + sizeof(u32));
    //mtos_printk("\n%s %d \n",__func__,__LINE__);
    return ret;
}
RET_CODE aud_file_getleftes_vitual_buffer(FILE_SEQ_T * p_file_seq, u32 * p_size)
{
    PLAYBACK_INTERNAL_T * p_pb_internal =  p_file_seq->pb_internal;
    RET_CODE ret = SUCCESS;
    //mtos_printk("\n%s %d \n",__func__,__LINE__);
    *p_size = p_pb_internal->vitual_audio_es_max_size - p_pb_internal->vitual_audio_size;
    return ret;
}
RET_CODE move_data_from_vitual_es(FILE_SEQ_T * p_file_seq)

{
    RET_CODE ret = SUCCESS;
    PLAYBACK_INTERNAL_T * p_pb_internal =  p_file_seq->pb_internal;
    u32 size = 0;
    u32 pts = 0;
    // mtos_printk("\n%s %d  size:%d\n",__func__,__LINE__,size);
    //video
    p_pb_internal->vitual_video_es_pos = p_pb_internal->vitual_video_es_buf;
    memcpy(&size, p_pb_internal->vitual_video_es_pos, sizeof(u32));

    while (size != 0) {
        //mtos_printk("\n%s %d  size:%d\n",__func__,__LINE__,size);
        memcpy(&pts, p_pb_internal->vitual_video_es_pos + size + sizeof(u32), sizeof(u32));
        vdec_dec_push_es(p_file_seq->p_vdec_dev, (u32)p_pb_internal->vitual_video_es_pos + sizeof(u32), size, pts);
        p_pb_internal->vitual_video_es_pos += (size + 2 * sizeof(u32));
        memcpy(&size, p_pb_internal->vitual_video_es_pos, sizeof(u32));
    }

    p_pb_internal->vitual_video_es_pos  = p_pb_internal->vitual_video_es_buf ;
    memset(p_pb_internal->vitual_video_es_buf, 0, p_pb_internal->vitual_video_es_max_size);
#if 1
    //audio
    p_pb_internal->vitual_audio_es_pos = p_pb_internal->vitual_audio_es_buf;
    memcpy(&size, p_pb_internal->vitual_audio_es_pos, sizeof(u32));

    //mtos_printk("\n%s %d \n",__func__,__LINE__);
    while (size != 0) {
        //mtos_printk("\n%s %d  size:%d\n",__func__,__LINE__,size);
        memcpy(&pts, p_pb_internal->vitual_audio_es_pos + size + sizeof(u32), sizeof(u32));
        adec_push_audio_es((u32)p_pb_internal->vitual_audio_es_pos + sizeof(u32), size, pts);
        p_pb_internal->vitual_audio_es_pos += (size + 2 * sizeof(u32));
        memcpy(&size, p_pb_internal->vitual_audio_es_pos, sizeof(u32));
    }

    p_pb_internal->vitual_audio_es_pos  = p_pb_internal->vitual_audio_es_buf ;
    memset(p_pb_internal->vitual_audio_es_buf, 0, p_pb_internal->vitual_audio_es_max_size);
    p_pb_internal->vitual_audio_size = 0;
#endif
    return ret;
}


int  av_decoder_init(void * p_handle)
{
    void (* p_fill_es_task)(void * p_param) = NULL;
    int ret       = 0;
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    {
        if (p_file_seq->init_av_dev == FALSE) {
            p_file_seq->init_av_dev = TRUE;
            x_init_av_device(p_file_seq);
        }

        if (p_file_seq->p_audio_dev) {
            aud_stop_vsb(p_file_seq->p_audio_dev);
        }

        if (p_file_seq->p_vdec_dev) {
			if(((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->is_timeshift==1)
            vdec_freeze_stop(p_file_seq->p_vdec_dev);
			else
				      vdec_stop(p_file_seq->p_vdec_dev);
        }

        mtos_task_sleep(20);

        if (p_file_seq->is_ts) {
            vdec_set_avsync_mode(p_file_seq->p_vdec_dev, VDEC_AVSYNC_FILEPLAYTS);
        } else if (io_isnetworkstream) {
            vdec_set_avsync_mode(p_file_seq->p_vdec_dev, 4);
        } else {
            vdec_set_avsync_mode(p_file_seq->p_vdec_dev, VDEC_AVSYNC_FILEPLAY);
        }

        vdec_set_file_playback_framerate(p_file_seq->p_vdec_dev, p_file_seq->video_fps);
#if (defined(CONFIG_YINHE_WARRIOS_SDK))
        //add by ybc for yinhe project
        vdec_set_avsync(NULL, TRUE);
#endif
        x_start_av_decoder(p_file_seq);
        //ret = x_map_es_buffer(p_file_seq);
        // yliu  rm
        /*
                if (ret != 0) {
                    FILE_SEQ_ERROR("[%s][ERROR] fail to map es buffer ...!!!\n", __func__);
                    FILE_SEQ_ERROR("[%s][ERROR] end end 22 ...\n", __func__);
                    mtos_task_sleep(300);
                    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
                    return 0;
                }
        */
        BUFFERING_PARAM_T * p_buf_param = NULL;
        p_buf_param = (BUFFERING_PARAM_T *)(p_file_seq->p_buffering);
        p_buf_param->start_show_loadmedia_time = FALSE;//peacer add
    }
    return 0;
}
///

/*
 *
 *    NOTICE:  please modify the function 'x_push_user_cmd' carefully
 *
 *                  you shouldn't modify the variable 'rd_pos' !!!!!!!!!!!!
 *
 */
int  x_push_user_cmd(void * hdl, FP_USER_CMD_T * p_cmd)
{
    FILE_SEQ_DEBUG("[%s] start start ...\n ", __func__);
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    int tmp = 0;

    if (p_file_seq->is_cmd_buf_full) {
        FILE_SEQ_ERROR("[%s]CMD BUFFER IS FULL...!!!!!\n ", __func__);
        return -1;
    }

    /*cmd ring buffer is not full*/
    if (p_file_seq->rd_pos <=  p_file_seq->wr_pos) {
        memcpy(p_file_seq->cmd_fifo + p_file_seq->wr_pos, p_cmd, sizeof(FP_USER_CMD_T));
        tmp = (p_file_seq->wr_pos + 1) % USER_CMD_FIFO_LEN;

        /*rd_pos == 0; and wr_pos wrap back to 0*/
        if (tmp == p_file_seq->rd_pos) {
            p_file_seq->is_cmd_buf_full = 1;
        }
    } else { //rd_pos > wr_pos
        memcpy(p_file_seq->cmd_fifo + p_file_seq->wr_pos, p_cmd, sizeof(FP_USER_CMD_T));
        tmp = p_file_seq->wr_pos + 1 ;//wr

        if (tmp == p_file_seq->rd_pos) {
            p_file_seq->is_cmd_buf_full = 1;
        }
    }

    p_file_seq->wr_pos = tmp;
    return 0;
}
/*
 *
 *    NOTICE:  please modify the function 'x_pop_user_cmd' carefully
 *
 *                  you shouldn't modify the variable 'wr_pos' !!!!!!!!!!!!
 *
 */
int    x_pop_user_cmd(void * hdl,  FP_USER_CMD_T * p_out_cmd)
{
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    int tmp = 0;

    if (p_file_seq->rd_pos  !=  p_file_seq->wr_pos  || p_file_seq->is_cmd_buf_full) {
        memcpy(p_out_cmd, p_file_seq->cmd_fifo + p_file_seq->rd_pos, sizeof(FP_USER_CMD_T));
        tmp = (p_file_seq->rd_pos + 1) % USER_CMD_FIFO_LEN;
        p_file_seq->rd_pos = tmp;
        p_file_seq->is_cmd_buf_full = 0;
        return 0;
    }

    return -1;
}

/*
 *
 *    NOTICE:  please modify the function 'x_pop_user_cmd' carefully
 *
 *                  you shouldn't modify the variable 'wr_pos' !!!!!!!!!!!!
 *
 */
void x_clear_cmd_fifo(void * hdl)
{
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    p_file_seq->is_cmd_buf_full = 0;
    p_file_seq->wr_pos = p_file_seq->rd_pos;
    return;
}
/*
 *
 *    NOTICE:  please modify the function 'x_pop_user_cmd' carefully
 *
 *                  you shouldn't modify the variable 'wr_pos' !!!!!!!!!!!!
 *
 */
void x_reset_cmd_fifo()
{
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    p_file_seq->is_cmd_buf_full = 0;
    p_file_seq->wr_pos = 0;
    p_file_seq->rd_pos = 0;
    memset(p_file_seq->cmd_fifo, 0, USER_CMD_FIFO_LEN * sizeof(FP_USER_CMD_T));
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
 */
extern  int audio_id, video_id, dvdsub_id;
BOOL x_loadMedia(void * p_handle)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    float fps = 25.0;
    BOOL ret = FALSE;
    char * file_path = p_file_seq->m_path[p_file_seq->cur_play_index];
    FILE_SEQ_DEBUG("[%s] start start...\n", __func__);
    //u32 tick_start = 0;
    //u32 tick_end =0;
    //tick_start = mtos_ticks_get();

    if (p_file_seq->loadMedaiOK) {
        x_unLoadMedia(p_file_seq);
    }

    if (!p_file_seq->loadMedaiOK) {
        int file_format = 0;
        //yliu add:
        p_file_seq->stream_type = STREAM_LIVE;
        // select stream to dump
        p_file_seq->p_stream  = open_stream(file_path, 0, &file_format);

        if (p_file_seq->p_stream == NULL) {
            FILE_SEQ_DEBUG("[%s] fail to open_stream!!!!!\n", __func__);
            FILE_SEQ_DEBUG("[%s] m_path:%s!!!!!\n", __func__, file_path);
            if (g_load_err == ERR_LOAD_UNKNOW &&  !is_file_seq_exit()){
				g_load_err = ERR_LOAD_OPEN_STREAM_ERR;
		}
            return FALSE;
        }
	 //mtos_printk("[%s] \n~~~~~~~~~~~~open_stream ok!!!!!\n\n", __func__);
        FILE_SEQ_DEBUG("\n mpctx->stream:%x,m_path:%s\n", p_file_seq->p_stream, file_path);
        p_file_seq->p_demuxer = demux_mp_open((stream_t *)(p_file_seq->p_stream), file_format, audio_id, video_id, dvdsub_id, file_path);

        if (p_file_seq->p_demuxer == NULL) {
            FILE_SEQ_DEBUG("[%s] fail to demux_open!!!!!\n", __func__);
            FILE_SEQ_DEBUG("[%s] m_path:%s!!!!!\n", __func__, p_file_seq->m_path[p_file_seq->cur_play_index]);

	       if (!is_file_seq_exit())
		    	g_load_err = ERR_LOAD_DEMUX_ERR;


            if (p_file_seq->p_stream) {
                free_stream(p_file_seq->p_stream);
                p_file_seq->p_stream = NULL;
            }

            return FALSE;
        }

        //tick_end = mtos_ticks_get();
        // FILE_SEQ_DEBUG("[%s] consume:%ld\n",__func__,tick_end-tick_start);
        //FILE_SEQ_DEBUG("\ndemux open %d %d\n", ((demuxer_t *)(p_file_seq->p_demuxer))->audio->id, ((demuxer_t *)(p_file_seq->p_demuxer))->video->id);
        p_file_seq->p_ds_audio = NULL;
        p_file_seq->p_ds_video = NULL;
        p_file_seq->p_ds_sub = NULL;
        p_file_seq->exsubtitle = 0;
        p_file_seq->subt_id = -1;
        p_file_seq->p_ds_audio = (void *)(((demuxer_t *)(p_file_seq->p_demuxer))->audio);
        p_file_seq->p_ds_video = (void *)(((demuxer_t *)(p_file_seq->p_demuxer))->video);
        p_file_seq->p_ds_sub = (void *)(((demuxer_t *)(p_file_seq->p_demuxer))->sub);

        //p_file_seq->exsubtitle = load_subtitles_mp(p_file_seq->m_path, fps);
        if (p_file_seq->m_play_state == FILE_SEQ_EXIT) {
            return TRUE;
        }

        if (DEMUXER_TYPE_MPEG_TS == ((demuxer_t *)(p_file_seq->p_demuxer))->type) {
            p_file_seq ->ts_priv = ds_ts_prog(p_file_seq->p_demuxer);
        }

        ds_get_vfilter_type(p_file_seq->p_ds_video);
        set_demux_media_info(p_file_seq);
        FILE_SEQ_DEBUG("[%s] ---------v_bps:%d  a_bps:%d duration:%d \n", __func__, \
                       p_file_seq->video_bps, p_file_seq->audio_bps, p_file_seq->duration);
        p_file_seq->loadMedaiOK = TRUE;
#ifdef MUL_SEEK
        p_file_seq->p_cur_demuxer = p_file_seq->p_demuxer;
#endif
    }

    FILE_SEQ_DEBUG("[%s] end end...\n", __func__);
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
 */
BOOL  x_check_av_codec_type(void * p_handle)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_handle;
    int video_codec  = 0xffff;
    int audio_codec  = 0xffff;
    BOOL isSupportVideoType = FALSE;
    BOOL isSupportAudioType = FALSE;

    if (p_file_seq->m_play_state == FILE_SEQ_EXIT) {
        FILE_SEQ_LOG("[%s] FILE_SEQ_EXIT...\n", __func__);
        return FALSE;
    }
    FILE_SEQ_LOG("[%s] p_file_seq->only_audio_mode: %d\n", __func__,p_file_seq->only_audio_mode);
    if (p_file_seq->only_audio_mode) {
        p_file_seq->audio_bps = ds_get_audio_bps(p_file_seq->p_ds_audio);

        if (p_file_seq->audio_bps <= 0 || p_file_seq->audio_bps >= AUDIO_MAX_BPS) {
            p_file_seq->audio_bps = AUDIO_DEFAULT_BPS;
            FILE_SEQ_ERROR("[%s] NOT FIND AUDIO BPS !!!!\n", __func__);
        }

  if(fp_is_timeshift_file())
  	{
     int   ts_apid = ts_get_audio_track_pid(p_file_seq->ts_priv, p_file_seq->audio_track_id);
	 mtos_printk("\n%s %d ts_pid :%d\n",__func__,__LINE__,ts_apid);
            demuxer_switch_audio(p_file_seq->p_demuxer, ts_apid);
  	}
        FILE_SEQ_DEBUG("[%s] audio_bps[%d]\n", __func__, p_file_seq->audio_bps);
        ds_get_audio_codec_type(p_file_seq->p_ds_audio, &p_file_seq->m_audio_codec_type,
                                &p_file_seq->audio_pid, p_file_seq->audio_output_mode);
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
                break;
            default:
                break;
        }

        if (isSupportAudioType == FALSE) {
            if (NULL != p_file_seq->event_cb) {
                p_file_seq->event_cb(FILE_PLAYBACK_UNSUPPORT_AUDIO, audio_codec_id);
            }

            return FALSE;
        } else {
            p_file_seq->is_av_codec_support = p_file_seq->is_av_codec_support | 0x1;
            p_file_seq->checkAvTypeOK = TRUE;
        }

        FILE_SEQ_DEBUG("[%s] this container has only audio stream !!!!\n", __func__);
        return TRUE;
    }

    p_file_seq->audio_bps = ds_get_audio_bps(p_file_seq->p_ds_audio);
    FILE_SEQ_DEBUG("[%s] type[%d]\n",
                   __func__, ((demuxer_t *)(p_file_seq->p_demuxer))->type);

    if (isNetworkStream(p_file_seq) == FALSE) {
        p_file_seq->video_bps = ds_get_video_bps(p_file_seq->p_ds_video);
    }

    ds_get_video_codec_type(p_file_seq->p_ds_video, &p_file_seq->m_video_codec_type,
                            &p_file_seq->video_pid, &p_file_seq->pcr_pid);
    ds_get_audio_codec_type(p_file_seq->p_ds_audio, &p_file_seq->m_audio_codec_type,
                            &p_file_seq->audio_pid, p_file_seq->audio_output_mode);
    FILE_SEQ_DEBUG("[%s] -----p_file_seq: m_video_codec_type=%d, video_pid=%d, pcr_pid=%d\n", __func__, \
                   p_file_seq->m_video_codec_type, p_file_seq->video_pid, p_file_seq->pcr_pid);
    FILE_SEQ_DEBUG("[%s] -----p_file_seq: m_audio_codec_type=%d, audio_pid=%d\n", __func__, \
                   p_file_seq->m_audio_codec_type, p_file_seq->audio_pid);
    FILE_SEQ_DEBUG("[%s] -----p_file_seq: acodec id=%x, vcodec id=%x\n", __func__, \
                   audio_codec_id, video_codec_id);
    video_codec = p_file_seq->m_video_codec_type;

    switch (video_codec) {
#ifdef DRV_DEFAULT
        case  9:
        case  10:
        case  11:
#endif
        case 12:
        case  3:
        case  1:
        case  0:
            isSupportVideoType = TRUE;
            p_file_seq->is_av_codec_support |= 0x2;
            break;
        default:
            break;
    }

    FILE_SEQ_DEBUG("[%s] isSupportVideoType=%d, video_codec=%d\n", __func__, isSupportVideoType, video_codec);
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
            p_file_seq->is_av_codec_support = p_file_seq->is_av_codec_support | 0x1;
            break;
        default:
            break;
    }

#if 0//add by doreen for rtsp test bbjx.mp4

    if (p_file_seq->rtsp_play_mode)
#endif
        FILE_SEQ_DEBUG("[%s] isSupportAudioType=%d, audio_codec=%d\n", __func__, isSupportAudioType, audio_codec);

    if (isSupportVideoType) {
        p_file_seq->checkAvTypeOK = TRUE;
        return TRUE;
    } else {
	    if((video_codec == 99) && (isSupportAudioType)) // linda zhu add, we support the audio file with MJPG cover.
	    {
	    	p_file_seq->only_audio_mode = 1;
		    p_file_seq->checkAvTypeOK = TRUE;
		    return TRUE;
	    }    
        if (NULL != p_file_seq->event_cb) {
            p_file_seq->event_cb(FILE_PLAYBACK_UNSUPPORT_VIDEO, video_codec_id);  //vince for bug 30923
        }

        FILE_SEQ_DEBUG("[%s] fail to check av codec type !!!\n", __func__);
        FILE_SEQ_DEBUG("[%s] video_codec[%d]  audio_codec[%d] !!!\n", __func__, video_codec, audio_codec);
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
/*
  cal the path count to jump and the adjust time
  yliu
*/
int multipath_seek_time_cal(float * seek_seconds, FILE_SEQ_T * p_file_seq)
{
    int i = 0;
    int time_accumulate = 0;
    int accum_direction = *seek_seconds > 0 ? 1 : -1;
    int m_seek_cnt = 0;
    double m_seek_adj = 0;
    double see_sec_modify = 0;
    double current_pts = 0;
    current_pts = ((demux_stream_t *)(p_file_seq->p_ds_video))->pts;
    mtos_printk("\n%s %d    seek sec %d  \n", __func__, __LINE__, (int)*seek_seconds);

    if (*seek_seconds > 0) {
        see_sec_modify = *seek_seconds * accum_direction + current_pts;
    } else {
        see_sec_modify = *seek_seconds * accum_direction + p_file_seq->p_m_duration[p_file_seq->cur_play_index] - current_pts;
    }

    see_sec_modify = *seek_seconds;
    //seek_seconds = seek_seconds * accum_direction;

    for (i = 0; i < p_file_seq->total_path; i ++) {
        time_accumulate += p_file_seq->p_m_duration[i];

        if (see_sec_modify - time_accumulate < 0) {
            break;
        }

        mtos_printk("\n%s %d  seek-cnt:%d  %d  i %d   secon:%d\n", __func__, __LINE__, time_accumulate, p_file_seq->p_m_duration[i], i, (int)see_sec_modify);
    }

    mtos_printk("\n%s %d    i %d   path:%d\n", __func__, __LINE__, i , p_file_seq->total_path);

    if (i >= p_file_seq->total_path) {
        m_seek_cnt = p_file_seq->total_path - 1;
        m_seek_adj = p_file_seq->p_m_duration[p_file_seq->total_path - 1];
    } else {
        m_seek_cnt = i;

        if (m_seek_cnt) {
            m_seek_adj = see_sec_modify - time_accumulate + p_file_seq->p_m_duration[m_seek_cnt];
        } else {
            m_seek_adj = *seek_seconds;
        }
    }

    p_file_seq->m_seek_adj = m_seek_adj;
    p_file_seq->m_seek_cnt = m_seek_cnt;
    mtos_printk("\n%s %d seek-cnt:%d  adj %d   asdf %d %d  %d\n", __func__, __LINE__, m_seek_cnt, (int)m_seek_adj, (int)see_sec_modify, (int)time_accumulate, (int)p_file_seq->p_m_duration[i]);
    return TRUE;
}

/**
* Function : Just set pcm audio codec audio paramter for audio decoder
*/
void set_audio_param_to_vsb(void)
{
    u32 size = 0;
    u32 align = 0;
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();

    // aud_get_buf_requirement_vsb(p_file_seq->p_audio_dev, &size, &align);
    // aud_set_buf_vsb(p_file_seq->p_audio_dev,(g_pFwMemCfg.p_audio_start) |0xa0000000, size);
    if (AUDIO_PCM == p_file_seq->m_audio_codec_type) {
        aud_pcm_param_vsb_t audio_param;
        int is_big_endian = 0;
        int bits = 16;
        int channels = 2;
        int sample_rate = 44100;

        if (p_file_seq->is_ts) {
            ts_get_pcm_info(p_file_seq->p_demuxer, &is_big_endian, &bits, &channels, &sample_rate);
        } else {
            //set default
            sample_rate = p_file_seq->audio_samplerate;
            channels = p_file_seq->audio_channels;
        }

        audio_param.is_big_endian = is_big_endian;
        audio_param.is_stereo = channels >= 2;
        audio_param.bits = bits;

        switch(sample_rate)
        {
          case 8000:
            audio_param.sample = AUDIO_SAMPLE_8_VSB;
            break;
          case 11025:
            audio_param.sample = AUDIO_SAMPLE_11_VSB;
            break;
          case 12000:
            audio_param.sample = AUDIO_SAMPLE_12_VSB;
            break;
          case 16000:
            audio_param.sample = AUDIO_SAMPLE_16_VSB;
            break;
          case 22050:
            audio_param.sample = AUDIO_SAMPLE_22_VSB;
            break;
          case 24000:
            audio_param.sample = AUDIO_SAMPLE_24_VSB;
            break;
          case 32000:
            audio_param.sample = AUDIO_SAMPLE_32_VSB;
            break;
          case 44100:
            audio_param.sample = AUDIO_SAMPLE_44_VSB;
            break;
          case 48000:
            audio_param.sample = AUDIO_SAMPLE_48_VSB;
            break;
          case 64000:
            audio_param.sample = AUDIO_SAMPLE_64_VSB;
            break;
          case 88200:
            audio_param.sample = AUDIO_SAMPLE_88_VSB;
            break;
          case 96000:
            audio_param.sample = AUDIO_SAMPLE_96_VSB;
            break;
          default:
            audio_param.sample = AUDIO_SAMPLE_48_VSB;
            break;
      
        }

/*
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
*/
        FILE_SEQ_DEBUG("pcm_header: %d channels, %d bits per sample, %d Hz\n", channels, bits, sample_rate);
        aud_start_vsb(p_file_seq->p_audio_dev, AUDIO_PCM, AUDIO_PCM_FILE);
        aud_set_pcm_param_vsb(p_file_seq->p_audio_dev, &audio_param);
    } else {
        aud_start_vsb(p_file_seq->p_audio_dev, p_file_seq->m_audio_codec_type, AUDIO_ES_FILE);
    }
}

/*
 *
 *
 *
 */
 float trick_pts = 0.0;
static int seek_buffer = 0; //linda add, fix bug 50332, there is something wrong if do seek when buffering, END BUFFERING after seek
void do_playAtTime()
{
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    demuxer_t * p_demuxer = (demuxer_t *)p_file_seq->p_demuxer;
    float real_seek_time_adj = 0;
    FILE_SEQ_DEBUG("[%s] ===do play at time222!!!!!\n", __func__);

    if (p_file_seq->total_path > 1) {
        while (m_load_task_lock <= 0) {
            mtos_task_sleep(50);
        }

        m_load_task_lock--;

        if ((p_file_seq->total_path > 1) && ((p_file_seq->p_audio_buf_tail == NULL) || (p_file_seq->p_video_buf_tail == NULL))) {
            ClearFlag(p_file_seq->m_user_cmd, CMD_PLAY_AT_TIME);
            m_load_task_lock++;
            return;
        }

#if 0

        if ((((p_file_seq->total_path > 1) && p_file_seq->p_audio_buf_tail->eof == 1) && (p_file_seq->p_video_buf_tail->eof == 1))) {
            mtos_printk("\n%s %d\n", __func__, __LINE__);
            ClearFlag(p_file_seq->m_user_cmd, CMD_PLAY_AT_TIME);
            m_load_task_lock++;
            return;
        }

#endif
    }

    FILE_SEQ_DEBUG("[%s] ===111111111111!!!!\n", __func__);
    p_demuxer = (demuxer_t *)p_file_seq->p_demuxer;
    //FILE_SEQ_DEBUG("[%s] %d %d %x %d %d\n", __func__,__LINE__,p_file_seq->total_path,p_demuxer,p_file_seq->p_audio_buf_tail->eof,p_file_seq->p_video_buf_tail->eof);
#ifdef MUL_SEEK
#if 0

    while (NULL == p_demuxer) {
        {
            ClearFlag(p_file_seq->m_user_cmd, CMD_PLAY_AT_TIME);
            m_load_task_lock++;
            //p_file_seq->m_play_state = FILE_SEQ_PLAY;
            return;
        }
        mtos_task_sleep(100);
        p_demuxer = (demuxer_t *)p_file_seq->p_demuxer;

        if (p_file_seq->is_preload_task_alive == FALSE) {
            ClearFlag(p_file_seq->m_user_cmd, CMD_PLAY_AT_TIME);
            m_load_task_lock++;
            //p_file_seq->m_play_state = FILE_SEQ_PLAY;
            return;
        }

        OS_PRINTF("\n%s %d\n", __FUNCTION__, __LINE__);
    }

#endif
#endif

    if (!p_demuxer->seekable) {
        ClearFlag(p_file_seq->m_user_cmd, CMD_PLAY_AT_TIME);

        if (NULL != p_file_seq->event_cb) {
            p_file_seq->event_cb(FILE_PLAYBACK_UNSUPPORT_SEEK, 0);
        }

        if (p_file_seq->total_path > 1) {
            m_load_task_lock++;
        }

        FILE_SEQ_LOG("---FILE_PLAYBACK_UNSUPPORT_SEEK---\n");
        return;
    }

    if (p_file_seq->vpts_upload > TIME_BASE) {
        last_vpts_upload = p_file_seq->vpts_upload;
    }

    if ((p_file_seq->is_play_at_time == 1) && (((p_file_seq->seek_seconds + 1) > 0) || ((p_file_seq->seek_seconds + 1) > 0 && (p_file_seq->total_path > 1)))) { //&& d_test==0
        // vdec_freeze(p_file_seq->p_vdec_dev);
        vdec_stop(p_file_seq->p_vdec_dev);

        if (p_file_seq->m_audio_codec_type != 0xffff) {
            aud_stop_vsb(p_file_seq->p_audio_dev);
        }

        /*vdec_file_clearesbuffer(p_file_seq->p_vdec_dev);
          vdec_get_es_buf_space(p_file_seq->p_vdec_dev, &freesize);
          OS_PRINTF("[%s] ------- playat time vdec es free: 0x%x\n",__func__,freesize);*/
        if (((demuxer_t *)(p_file_seq->p_cur_demuxer))->subt_info.cnt > 0) {
            clear_sub_fifo_kw(p_file_seq->p_sub_fifo_handle);    //clear fifo when do seek
        }

        if (p_file_seq->is_ts) {
            real_seek_time_adj = p_file_seq->orig_vpts - last_vpts_upload / TIME_BASE - p_file_seq->first_vpts ;
        } else {
            real_seek_time_adj = p_file_seq->orig_vpts - last_vpts_upload / TIME_BASE;
        }

        //    OS_PRINTF("\n%s %d %d %d  %d\n",__func__,__LINE__,(int)((p_file_seq->orig_vpts)),(int)((p_file_seq->vpts_upload / TIME_BASE)),p_file_seq->m_loaded_pts );
        FILE_SEQ_DEBUG("[%s] ------ real seek time: %d ms, real_seek_time_adj = %d\n", __func__, (int)(p_file_seq->seek_seconds * 1000), (int) real_seek_time_adj);
#if 0
        {
            float seek_sec;

            if ((real_seek_time_adj < 0) && (p_file_seq->seek_seconds > 0) && (p_file_seq->total_path > 1)) {
                seek_sec = 0;
            } else {
                seek_sec = p_file_seq->seek_seconds - (real_seek_time_adj / 1000);

                if ((seek_sec < 0) && (p_file_seq->seek_seconds > 0) && (p_file_seq->total_path > 1)) {
                    seek_sec = 0;
                } else {
                    p_file_seq->seek_seconds =  seek_sec;
                    OS_PRINTF("\n%s %d %x\n", __func__, __LINE__, p_file_seq->p_cur_demuxer);
                    demux_seek(p_file_seq->p_cur_demuxer, seek_sec, 0, 0) ;
                }
            }
        }
#else

        if (p_file_seq->total_path > 1) {
            multipath_seek_time_cal(&p_file_seq->seek_seconds, p_file_seq);
        } else {
            // ((demux_stream_t *) p_file_seq->p_cur_ds_video)->pts = 0;
            //      p_file_seq->seek_seconds = p_file_seq->seek_seconds - (real_seek_time_adj / 1000);
            trick_pts = p_file_seq->seek_seconds;
            if (p_file_seq->only_audio_mode) {
                p_file_seq->seek_seconds = p_file_seq->seek_seconds - ((demuxer_t *)p_file_seq->p_cur_demuxer)->audio->pts + first_vpts;
            } else {
                p_file_seq->seek_seconds = p_file_seq->seek_seconds - ((demuxer_t *)p_file_seq->p_cur_demuxer)->video->pts + first_vpts;
            }
        }

        if (p_file_seq->total_path > 1) {
            //demux_seek(p_file_seq->p_cur_demuxer, p_file_seq->seek_seconds , 0, 0) ;
        } else {
            demux_seek(p_file_seq->p_cur_demuxer, p_file_seq->seek_seconds , 0, 0) ;
        }

        if (p_file_seq->total_path > 1) {
            p_file_seq->orig_apts = 0;
            p_file_seq->orig_vpts = 0;
            p_file_seq->orig_spts = 0;
            ((demux_stream_t *) p_file_seq->p_cur_ds_audio)->pts = 0;
            ((demux_stream_t *) p_file_seq->p_cur_ds_video)->pts = 0;
            p_file_seq->m_get_apts =  p_file_seq->m_get_vpts = 0;
            p_file_seq->p_ds_audio = ((demuxer_t *)p_file_seq->p_cur_demuxer)->audio;
            p_file_seq->p_ds_video = ((demuxer_t *)p_file_seq->p_cur_demuxer)->video;
            p_file_seq->p_video_buf = NULL;
            p_file_seq->p_audio_buf = NULL;
            p_file_seq->preload_video_buffer.read_pos = 0;
            p_file_seq->preload_video_buffer.write_pos = 0;
            p_file_seq->preload_audio_buffer.read_pos = 0;
            p_file_seq->preload_audio_buffer.write_pos = 0;
            p_file_seq->isAudioEsEnd = FALSE;
            p_file_seq->isVideoEsEnd = FALSE;
            ((demux_stream_t *) p_file_seq->p_cur_ds_audio)->eof = 0;
            ((demux_stream_t *) p_file_seq->p_cur_ds_video)->eof = 0;
            OS_PRINTF("\n%s %d %d %d \n", __func__, __LINE__, (int)(((demuxer_t *)p_file_seq->p_cur_demuxer)->audio->pts), (int)(((demuxer_t *)p_file_seq->p_cur_demuxer)->video->pts));
            OS_PRINTF("\n%s %d \n", __func__, __LINE__);
        }

#endif
        OS_PRINTF("[%s] ------- call demux_seek\n", __func__);

        //vdec_set_avsync(p_file_seq->p_vdec_dev,1);
        //OS_PRINTF("[%s] ------- call vdec_set_avsync\n",__func__);
        if (p_file_seq->only_audio_mode == FALSE) {
            vdec_set_avsync_mode(p_file_seq->p_vdec_dev, VDEC_AVSYNC_FILEPLAY);
            vdec_start(p_file_seq->p_vdec_dev, p_file_seq->m_video_codec_type, 2);
            OS_PRINTF("[%s] ------- call vdec_start\n", __func__);
        }

        if (p_file_seq->m_audio_codec_type != 0xffff) {
            set_audio_param_to_vsb();
        }

        FILE_SEQ_DEBUG("[%s] ------- call aud_start_vsb\n", __func__);
        p_file_seq->needNewAudioData = TRUE;
        p_file_seq->needNewVideoData = TRUE;
        p_file_seq->m_tmp_ves_buf_pos = 0;
        p_file_seq->isAudioBufferFull = FALSE;
        p_file_seq->isVideoBufferFull = FALSE;
#if 1
        p_file_seq->isAudioEsEnd = FALSE;
        p_file_seq->isVideoEsEnd = FALSE;
        ((demux_stream_t *) p_file_seq->p_cur_ds_audio)->eof = 0;
        ((demux_stream_t *) p_file_seq->p_cur_ds_video)->eof = 0;
#endif
        p_file_seq->vpts_upload = 1;
        p_file_seq->apts_upload = 0;
        FILE_SEQ_DEBUG("@@ p_file_seq->vpts_upload = %d\n", p_file_seq->vpts_upload);//for bug27058

        if (g_hls_playmode==1) {
            demux_stream_t * ds_a = NULL;
            demux_stream_t * ds_v = NULL;
            demux_stream_t * ds_sub = NULL;
            ds_a = p_file_seq->p_cur_ds_audio;
            ds_v = p_file_seq->p_cur_ds_video;
            ds_sub = p_file_seq->p_cur_ds_sub;
            int pack_num = 0;
            int cur_audio_packet_size = 0;
            int cur_video_packet_size = 0;
            unsigned char * p_data;

            if (p_file_seq->only_audio_mode == FALSE) {
                vdec_set_avsync_mode(p_file_seq->p_vdec_dev, VDEC_AVSYNC_FILEPLAYTS);
            }

            cur_video_packet_size = ds_get_packet(ds_v, &(p_file_seq->p_v_pkt_start));

            if (ds_v->pts < 0.0) {
                ds_v->pts = 0.0;
            }

            p_data = p_file_seq->p_v_pkt_start;
            p_file_seq->orig_vpts =  ds_v->pts * 1000.0;
            p_file_seq->left_v_pkt_bytes = cur_video_packet_size;
            p_file_seq->sys_vpts = (u32)((p_file_seq->orig_vpts * PTS_BASE)) | VALID_PTS_MASK;
            p_file_seq->m_tmp_ves_buf_pos = 0;

            if (p_file_seq->m_audio_codec_type != 0xffff) {
                do {
                    if (!ds_a->eof) {
                        p_file_seq->extra_audio_size = 0;
                        p_file_seq->p_extra_aud_buf = NULL;
                        cur_audio_packet_size = ds_get_buffer_pkt_audio(ds_a, &(p_file_seq->p_a_pkt_start), &(p_file_seq->p_extra_aud_buf), &(p_file_seq->extra_audio_size));

                        if (cur_audio_packet_size <= 0 || p_file_seq->p_a_pkt_start == NULL) {
                            FILE_SEQ_LOG("[%s][error]  >>>> NO audio strream!!<<<<\n", __func__);
                            break;
                        }

                        p_file_seq->needNewVideoData = FALSE;
                        p_file_seq->left_a_pkt_bytes = cur_audio_packet_size;
                    } else {
                        FILE_SEQ_LOG("[%s][ok]  >>>> Audio Stream Demuxer Is Finished !!<<<<\n", __func__);
                        p_file_seq->isAudioEsEnd = TRUE;
                        break;
                    }

                    if (ds_a->pts < 0.0) {
                        ds_a->pts = 0.0;
                    }

                    p_file_seq->orig_apts =  ds_a->pts * 1000.0;

                    if (p_file_seq->orig_apts > p_file_seq->max_audio_pts) {
                        p_file_seq->max_audio_pts = p_file_seq->orig_apts;
                    }

                    {
                        if (p_file_seq->orig_apts - p_file_seq->orig_vpts > 1500) {
                            p_file_seq->needNewAudioData = FALSE;
                            p_file_seq->sys_apts = (u32)((p_file_seq->orig_apts * PTS_BASE)) | VALID_PTS_MASK;
                            FILE_SEQ_DEBUG("[%s]throw3[%d]packet,apts[%lld]ms,vpts[%lld]ms\n", __func__,
                                           pack_num, (int64_t)p_file_seq->orig_apts, (int64_t)p_file_seq->orig_vpts);
                            break;
                        }
                    }

                    if (p_file_seq->p_extra_aud_buf) {
                        free(p_file_seq->p_extra_aud_buf);
                        p_file_seq->p_extra_aud_buf = NULL;
                    }
                } while (pack_num++ < 50);

                if (p_file_seq->needNewAudioData) {
                    if (p_file_seq->p_extra_aud_buf) {
                        free(p_file_seq->p_extra_aud_buf);
                        p_file_seq->p_extra_aud_buf = NULL;
                    }
                }

                FILE_SEQ_DEBUG("[%s]throw4[%d]packet,apts[%lld]ms,vpts[%lld]ms\n", __func__,
                               pack_num, (int64_t)p_file_seq->orig_apts, (int64_t)p_file_seq->orig_vpts);
            }
        }

        p_file_seq->is_play_at_time = 0;

        if (p_file_seq->total_path <= 1) {
            p_file_seq->seek_seconds = -1;
        } else {
            // p_file_seq->seek_seconds = -1;
        }

        seek_buffer = 1;
#ifdef FILE_PLAYBACK_DEBUG
        flag_debug = 1;
#endif
    }

    ClearFlag(p_file_seq->m_user_cmd, CMD_PLAY_AT_TIME);

    if (p_file_seq->total_path > 1) {
        m_load_task_lock++;
    }
}
/*
 *
 *
 *
 */
void do_changeAudioTrack()
{
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
 *
 *
 *
 */
static int start_init = 0;
void mutipath_preload_thread(void * p_param)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_param;
    MT_ASSERT(p_file_seq != NULL);
    p_file_seq->is_preload_task_alive = TRUE;
    double audio_pts_base = 0.0;
    double video_pts_base = 0.0;
    double audio_pts_last = 0.0;
    double video_pts_last = 0.0;
    preload_buffer_t * p_video_buffer = &p_file_seq->preload_video_buffer;
    preload_buffer_t * p_audio_buffer = &p_file_seq->preload_audio_buffer;
    demux_stream_t * ds_a = NULL;
    demux_stream_t * ds_v = NULL;
    ds_a = p_file_seq->p_ds_audio;
    ds_v = p_file_seq->p_ds_video;
    u8 ** pp_file_address = p_file_seq->m_path;
    u32 file_addr_num = p_file_seq->total_path;
    u32 i = 0;
    u32 video_total_size = 0;
    u32 audio_total_size = 0;
    //double audio_pts = 0.0;
    //double video_pts = 0.0;
    p_file_seq->m_get_apts = 0;
    p_file_seq->m_get_vpts = 0;
    int video_pkt_size = 0, audio_pkt_size = 0;
    u8 * pktv_start = NULL;
    u8 * pkta_start = NULL;
    es_pkt_data_t * p_node = NULL;
    u8 * extra = NULL;
    u32 extra_len = 0;
    int get_video_buffer_ret = -2 ;
    float seek_adj = 0;
    int get_audio_buffer_ret = -2 ;
    OS_PRINTF("preload thread start\n");

    while (((p_file_seq->m_play_state != FILE_SEQ_PLAY) && (p_file_seq->m_preload_state != FILE_SEQ_EXIT)) || (p_file_seq->is_task_alive == FALSE)) {
        mtos_task_sleep(100);
    }

    while ((p_file_seq->m_preload_state != FILE_SEQ_STOP) && (p_file_seq->m_preload_state != FILE_SEQ_EXIT) && (p_file_seq->is_task_alive)) {
        //yliu add :add fake eof for pts distance too larger
        double pts_threahold = 30;
        static int fake_eof_cnt = 0;
        mtos_task_sleep(1);

        if (p_file_seq->start_seconds > 0) {
            if (start_init == 0) {
                float start_seconds = p_file_seq->start_seconds;
                multipath_seek_time_cal(&start_seconds, p_file_seq);
                start_init = 1;
            }

            //p_file_seq->start_seconds = 0;
        }

        if (((p_file_seq->m_get_vpts - p_file_seq->m_get_apts) > pts_threahold) || ((p_file_seq->m_get_apts - p_file_seq->m_get_vpts) > pts_threahold)) {
            fake_eof_cnt++;

            if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state == FILE_SEQ_BUFFERING) {
                fake_eof_cnt = 0;
            }

            if (fake_eof_cnt > 30) {
                OS_PRINTF("\n%s %d\n", __func__, __LINE__);
                ds_a->eof = ds_v->eof = 1;
                fake_eof_cnt = 0;
            }
        }

        if (((p_file_seq->m_get_vpts - p_file_seq->m_get_apts) > 0 && ds_a->eof == 1) || ((p_file_seq->m_get_apts  - p_file_seq->m_get_vpts) > 0 && ds_v->eof == 1)) {
            OS_PRINTF("\n%s %d\n", __func__, __LINE__);
            ds_a->eof = ds_v->eof = 1;
        }

        if (p_file_seq->m_seek_cnt == p_file_seq->cur_play_index && p_file_seq->m_seek_adj > -1) {
            while (m_load_task_lock <= 0) {
                mtos_task_sleep(50);
            }

            m_load_task_lock--;
            ds_a = p_file_seq->p_ds_audio;
            ds_v = p_file_seq->p_ds_video;
            {
                unsigned char * start_a;
                unsigned char * start_v;
                //OS_PRINTF("\n%s %d %d\n",__func__,__LINE__,(int)ds_v->pts);
                ds_get_packet(ds_v, &start_v);
                ds_get_packet(ds_a, &start_a);
                //OS_PRINTF("\n%s %d %d\n",__func__,__LINE__,(int)ds_v->pts);
                //OS_PRINTF("\n%s %d %d\n",__func__,__LINE__,(int)p_file_seq->m_seek_adj);
                OS_PRINTF("\n%s %d %d %d\n", __func__, __LINE__, (int)p_file_seq->m_seek_adj, (int)p_file_seq->duration);

                if (p_file_seq->m_seek_adj >= 0) {
                    double inner_pts = ds_v->pts;
                    inner_pts = min(ds_v->pts, p_file_seq->duration -  30);
                    // mtos_printk("\n%s %d  pts  %d  dura %d  adj:%d\n",__func__,__LINE__,(int)ds_v->pts,(int)p_file_seq->duration,(int)p_file_seq->m_seek_adj);
                    // p_file_seq->m_seek_adj = min(inner_pts, p_file_seq->m_seek_adj);
                    //   mtos_printk("\n%s %d  inner  %d  adj %d\n",__func__,__LINE__,(int)inner_pts,(int) p_file_seq->m_seek_adj);
                    {
                        seek_adj = p_file_seq->m_seek_adj - ds_v->pts;
                        seek_adj = min(seek_adj, p_file_seq->duration -  30 - ds_v->pts);
                        demux_seek(p_file_seq->p_cur_demuxer, seek_adj , 0, 0) ;
                    }
                }

                OS_PRINTF("\n%s %d %d %d\n", __func__, __LINE__, (int)p_file_seq->m_seek_adj, (int)p_file_seq->duration);
                p_file_seq->m_get_vpts += seek_adj;
                p_file_seq->m_get_apts += seek_adj;
                p_file_seq->m_seek_adj = -1;
                p_file_seq->m_seek_cnt = -1;
                seek_adj = 0;

                if ((p_file_seq->seek_seconds) < -0.5) {
                    if (NULL != p_file_seq->event_cb) {
                        FILE_SEQ_LOG("[%s] drv send FILE_PLAYBACK_SEEK_FINISHED!!\n", __func__);
                        p_file_seq->event_cb(FILE_PLAYBACK_SEEK_FINISHED, 0);
                    }
                }

                //OS_PRINTF("\n%s %d %d\n",__func__,__LINE__,(int)ds_v->pts);
            }
            //audio_pts_base +=p_file_seq->m_seek_adj;
            //video_pts_base +=p_file_seq->m_seek_adj;
            //p_file_seq->m_get_apts = p_file_seq->m_get_vpts = video_pts_base;
            m_load_task_lock++;
        }

        if (((ds_a->eof && ds_v->eof) || (p_file_seq->m_seek_cnt > -1 && p_file_seq->m_seek_cnt != p_file_seq->cur_play_index))) {
            char * cur_play_addr;
            int i;
            int  seeking = 0;
            OS_PRINTF("next file\n");

            while (m_load_task_lock <= 0) {
                mtos_task_sleep(50);
            }

            m_load_task_lock--;
        mul_next_file:
            get_video_buffer_ret = -2 ;
            get_audio_buffer_ret = -2 ;

            if (p_file_seq->m_seek_cnt > -1 && p_file_seq->m_seek_cnt != p_file_seq->cur_play_index) {
                seeking = 1;
                p_file_seq->cur_play_index = p_file_seq->m_seek_cnt;
            } else {
                p_file_seq->cur_play_index++;
            }

            file_addr_num--;
            cur_play_addr = (char *)(p_file_seq->m_path + p_file_seq->cur_play_index);

            if (/**cur_play_addr == NULL || */p_file_seq->total_path <= p_file_seq->cur_play_index) {
                OS_PRINTF("All program data are buffered, now exit preload thread\n");
                printf("####%d %d\n", file_addr_num, p_file_seq->total_path);

                if (p_file_seq->p_audio_buf_tail == NULL || p_file_seq->p_video_buf_tail == NULL) {
                    p_file_seq->isAudioEsEnd = TRUE;
                    p_file_seq->isVideoEsEnd = TRUE;
                } else {
                    p_file_seq->p_audio_buf_tail->eof = 1;
                    p_file_seq->p_video_buf_tail->eof = 1;
                    p_file_seq->p_audio_buf_tail->next_pkt = NULL;
                    p_file_seq->p_video_buf_tail->next_pkt = NULL;

                    if (p_file_seq->p_audio_buf == NULL) {
                        p_file_seq->p_audio_buf = p_file_seq->p_audio_buf_tail;
                    }

                    if (p_file_seq->p_video_buf == NULL) {
                        p_file_seq->p_video_buf = p_file_seq->p_video_buf_tail;
                    }
                }

                p_file_seq->m_seek_cnt = -1;
                p_file_seq->m_seek_adj = -1;
                p_file_seq->cur_play_index = p_file_seq->total_path - 1;

                if (seeking)
                    if (NULL != p_file_seq->event_cb) {
                        FILE_SEQ_LOG("[%s] drv send FILE_PLAYBACK_SEEK_FINISHED!!\n", __func__);
                        p_file_seq->event_cb(FILE_PLAYBACK_SEEK_FINISHED, 0);
                    }

                m_load_task_lock++;
                continue;
            }

            // OS_PRINTF("\n%s %d %d %d\n",__func__,__LINE__,p_file_seq->m_seek_cnt,(int)p_file_seq->m_loaded_pts);
            if (p_file_seq->m_seek_cnt == 0) {
                p_file_seq->m_loaded_pts = 0;
            } else {
                int kk = 0;
                double load_pts = 0;

                for (kk = 0; kk < p_file_seq->cur_play_index; kk++) {
                    load_pts += p_file_seq->p_m_duration[kk];
                }

                p_file_seq->m_loaded_pts = load_pts;
            }

            OS_PRINTF("\n%s %d %d %d\n", __func__, __LINE__, p_file_seq->m_seek_cnt, (int)p_file_seq->m_loaded_pts);
            p_file_seq->m_seek_cnt = -1;
            audio_pts_base = audio_pts_last;
            video_pts_base = video_pts_last;
            p_file_seq->p_ds_audio = NULL;
            p_file_seq->p_ds_video = NULL;
            audio_pts_base = video_pts_base = p_file_seq->m_loaded_pts ;
            x_unLoadMedia(p_file_seq);

            // mtos_sem_destroy(&(p_file_seq->sub_fifo_mutex), 0);
            for (i = 0; i < 4; i++) {
                if (x_loadMedia(p_file_seq) == TRUE) {
                    OS_PRINTF("Preload thread load media OK\n");
                    break;
                } else {
                    OS_PRINTF("Preload thread load media fail %d\n", i);
                }
            }

            if (i == 4) {
                //
                //
                OS_PRINTF("preload tried 4 times, failed, exit\n");
                p_file_seq->m_seek_adj = -1;
                goto mul_next_file;
            }

            p_file_seq->p_m_duration[p_file_seq->cur_play_index] = ceil(p_file_seq->duration);
            ds_a = p_file_seq->p_ds_audio;
            ds_v = p_file_seq->p_ds_video;
            {
                unsigned char * start_a;
                unsigned char * start_v;
                //OS_PRINTF("\n%s %d %d\n",__func__,__LINE__,(int)ds_v->pts);
                //OS_PRINTF("\n%s %d %d\n",__func__,__LINE__,(int)ds_v->pts);
                //OS_PRINTF("\n%s %d %d\n",__func__,__LINE__,(int)p_file_seq->m_seek_adj);
                seek_adj = 0;

                if (p_file_seq->m_seek_adj >= 0) {
                    p_file_seq->m_seek_adj = min((p_file_seq->duration - 30), p_file_seq->m_seek_adj);
                    OS_PRINTF("\n%s %d %d %d\n", __func__, __LINE__, (int)(p_file_seq->m_seek_adj * 1000), (int)(ds_v->pts * 1000));
                    ds_get_packet(ds_v, &start_v);
                    ds_get_packet(ds_a, &start_a);
                    OS_PRINTF("\n%s %d %d %d\n", __func__, __LINE__, (int)(p_file_seq->m_seek_adj * 1000), (int)(ds_v->pts * 1000));
                    seek_adj = min((p_file_seq->duration - 30), p_file_seq->m_seek_adj - ds_v->pts);
                    OS_PRINTF("\n%s %d %d %d\n", __func__, __LINE__, (int)(p_file_seq->m_seek_adj * 1000), (int)(ds_v->pts * 1000));
                    demux_seek(p_file_seq->p_cur_demuxer, (float)seek_adj, 0, 0) ;
                }

                //OS_PRINTF("\n%s %d %d\n",__func__,__LINE__,(int)ds_v->pts);
            }
            //audio_pts_base +=p_file_seq->m_seek_adj;
            //video_pts_base +=p_file_seq->m_seek_adj;
            p_file_seq->m_get_apts = p_file_seq->m_get_vpts = video_pts_base;
            p_file_seq->m_get_vpts += seek_adj;
            p_file_seq->m_get_apts += seek_adj;
            p_file_seq->m_seek_adj = -1;
            seek_adj = 0;
            OS_PRINTF("\n%s %d \n", __func__, __LINE__);

            if (seeking && (p_file_seq->seek_seconds) < -0.5)
                if (NULL != p_file_seq->event_cb) {
                    FILE_SEQ_LOG("[%s] drv send FILE_PLAYBACK_SEEK_FINISHED!!\n", __func__);
                    get_video_buffer_ret = -2 ;
                    get_audio_buffer_ret = -2 ;
                    p_file_seq->event_cb(FILE_PLAYBACK_SEEK_FINISHED, 0);
                }

            OS_PRINTF("\n%s %d \n", __func__, __LINE__);
            m_load_task_lock++;
            //OS_PRINTF("ds_a->eof=%d, ds_v->eof=%d, ds_a->pts=%lf, ds_v->pts=%lf\n",
            //      ds_a->eof, ds_v->eof, ds_a->pts, ds_v->pts);
            //p_file_seq->p_ds_audio = (void *)(((demuxer_t *)(p_file_seq->p_demuxer))->audio);
            //p_file_seq->p_ds_video = (void *)(((demuxer_t *)(p_file_seq->p_demuxer))->video);
        } else {
            if ((p_file_seq->m_get_vpts <= p_file_seq->m_get_apts && (!ds_v->eof)) || (ds_a->eof && ds_v->eof == 0) || ((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state) {
                if (!ds_v->eof) {
                    if (get_video_buffer_ret != -1) {
                        // OS_PRINTF("111\n");
                        video_pkt_size = ds_get_packet_video(ds_v, &pktv_start, p_file_seq->cur_speed);

                        //           mtos_printk("\n%s %d vpts:%d ds_v pts:%d  apts:%d\n",__func__,__LINE__,(int)p_file_seq->m_get_vpts,(int)ds_v->pts,(int)p_file_seq->m_get_apts);
                        if (video_pkt_size <= 0 || pktv_start == NULL) {
                            OS_PRINTF("[%s][ERROR] video packet size [%d]!!\n", __func__, video_pkt_size);
                            continue;
                        }

                        video_total_size = (UPPER_SIZE(sizeof(es_pkt_data_t)) + UPPER_SIZE(video_pkt_size));
                    }

                    //      mtos_printk("\n%s %d vpts:%d ds_v pts:%d  apts:%d\n",__func__,__LINE__,(int)p_file_seq->m_get_vpts,(int)ds_v->pts,(int)p_file_seq->m_get_apts);
                    get_video_buffer_ret = get_preload_buffer(p_video_buffer, video_total_size, (u8 **) &p_node);

                    if (get_video_buffer_ret == -1) {
                        continue;
                    }

                    // OS_PRINTF("%x\n", p_node);
                    if (p_file_seq->m_preload_state == FILE_SEQ_STOP) {
                        break;
                    }

                    if (p_node == NULL) {
                        continue;
                    }

                    memset(p_node, 0x00, sizeof(es_pkt_data_t));
                    p_node->data = (u8 *)p_node + UPPER_SIZE(sizeof(es_pkt_data_t));
                    p_node->data_len = video_pkt_size;
                    p_node->pts = p_file_seq->m_get_vpts = ds_v->pts + video_pts_base;
                    video_pts_last = ds_v->pts;
                    // OS_PRINTF("v %d \n", (u32)(p_node->pts*1000));
                    memcpy(p_node->data, pktv_start, video_pkt_size);

                    if (p_file_seq->p_video_buf == NULL) {
                        p_file_seq->p_video_buf = p_file_seq->p_video_buf_tail = p_node;
                    } else {
                        p_file_seq->p_video_buf_tail->next_pkt = p_node;
                        p_node->prev_pkt = p_file_seq->p_video_buf_tail;
                        p_file_seq->p_video_buf_tail = p_node;
                    }
                }
            }

            if (((p_file_seq->m_get_vpts - p_file_seq->m_get_apts > 0.0001) && (!ds_a->eof)) || (ds_v->eof && ds_a->eof == 0)) {
                if (!ds_a->eof) {
                    // OS_PRINTF("222\n");
                    if (get_audio_buffer_ret != -1) {
                        extra = NULL;
                        extra_len = 0;
                        audio_pkt_size = ds_get_packet_audio(ds_a, &pkta_start,
                                                             &extra, (uint8_t **)&extra_len);

                        if (audio_pkt_size <= 0 || pkta_start == NULL) {
                            OS_PRINTF("[%s][ERROR] audio packet size [%d]!!\n", __func__, audio_pkt_size);
                            continue;
                        }

                        audio_total_size = (UPPER_SIZE(sizeof(es_pkt_data_t)) + UPPER_SIZE(audio_pkt_size) +
                                            UPPER_SIZE(extra_len));
                    }

                    get_audio_buffer_ret = get_preload_buffer(p_audio_buffer, audio_total_size, (u8 **)&p_node);

                    if (p_file_seq->m_preload_state == FILE_SEQ_STOP) {
                        break;
                    }

                    if (get_audio_buffer_ret == -1) {
                        continue;
                    }

                    if (p_node == NULL) {
                        continue;
                    }

                    memset(p_node, 0x00, sizeof(es_pkt_data_t));
                    p_node->data = (u8 *)p_node + UPPER_SIZE(sizeof(es_pkt_data_t));
                    MT_ASSERT(p_node->data != NULL);
                    p_node->extra_data = p_node->data + UPPER_SIZE(audio_pkt_size);
                    p_node->data_len = audio_pkt_size;
                    p_node->pts = p_file_seq->m_get_apts = ds_a->pts + audio_pts_base;
                    audio_pts_last = ds_a->pts;
                    // OS_PRINTF("a %d \n", (u32)(p_node->pts*1000));
                    p_node->extra_data_len = extra_len;
                    memcpy(p_node->data, pkta_start, audio_pkt_size);

                    if (extra_len) {
                        memcpy(p_node->extra_data, extra, extra_len);
                        free(extra);
                    }

                    if (p_file_seq->p_audio_buf == NULL) {
                        p_file_seq->p_audio_buf = p_file_seq->p_audio_buf_tail = p_node;
                    } else {
                        p_file_seq->p_audio_buf_tail->next_pkt = p_node;
                        p_node->prev_pkt = p_file_seq->p_audio_buf_tail;
                        p_file_seq->p_audio_buf_tail = p_node;
                    }
                }
            }
        }
    }

    x_unLoadMedia(p_file_seq);
    p_file_seq->is_preload_task_alive = FALSE;
#if 0

    if (p_file_seq->total_path > 0) { //peacer add
        for (i = 0; i < p_file_seq->total_path; i++) {
            mtos_free(p_file_seq->m_path[i]);
            p_file_seq->m_path[i] = NULL;
        }

        //p_file_seq->total_path = 0;
    }

#endif
    FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);

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
 */
BOOL  x_get_av_dec_cap(void * pHandle)
{
    FILE_SEQ_T * p_file_seq = pHandle;
    int ves_buf_size = 0;
    int aes_buf_size = 0;
    MT_ASSERT(p_file_seq != NULL);
    MT_ASSERT(p_file_seq->p_vdec_dev != NULL);
    MT_ASSERT(p_file_seq->p_audio_dev != NULL);
    vdec_get_es_buf_size(p_file_seq->p_vdec_dev, (u32 *)&ves_buf_size);
    p_file_seq->dec_cap.max_ves_num = ves_buf_size << 10;
    aud_file_gettotalesbuffer_vsb(p_file_seq->p_audio_dev, (u32 *)&aes_buf_size);
    p_file_seq->dec_cap.max_aes_num = aes_buf_size;
    FILE_SEQ_LOG("[%s] ves %d aes %d...\n", __func__,
                 p_file_seq->dec_cap.max_ves_num, p_file_seq->dec_cap.max_aes_num);
    return TRUE;
}

/*
 *
 *    map ves/aes  tmp buf which was allocated in linux kernel to user space
 *    and on other os, do nothing except allocate necessary physical memory
 *
 */
int x_map_es_buffer(void * pHandle)
{
    FILE_SEQ_T * p_file_seq = pHandle;
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);
#ifdef __LINUX__
    audio_dec_init_es_buff(p_file_seq->p_audio_dev);
    vdec_dec_init_es_buff(p_file_seq->p_vdec_dev);
    p_file_seq->p_tmp_aes_buf = audio_get_es_tmp_buf(p_file_seq->p_audio_dev);
    //p_file_seq->p_tmp_ves_buf = vdec_map_es_tmp_buf(p_file_seq->p_vdec_dev);
    //p_file_seq->p_current_sys_vpts = (u32 *)(vdec_map_share_info_buf(p_file_seq->p_vdec_dev));
#else
    p_file_seq->p_tmp_aes_buf = (char *)mtos_malloc(AES_TMP_BUF_LEN) ;
    //p_file_seq->p_tmp_ves_buf = (char *)mtos_malloc(VES_TMP_BUF_LEN) ;

    if (p_file_seq->p_tmp_aes_buf == NULL) {
        FILE_SEQ_ERROR("[%s][ERROR] fail to malloc es buffer ...\n", __func__);
        return -1;
    }

#endif
    FILE_SEQ_DEBUG("[%s] p_tmp_aes_buf[0x%x] ...\n", __func__, p_file_seq->p_tmp_aes_buf);
    //FILE_SEQ_DEBUG("[%s] p_tmp_ves_buf[0x%x] ...\n", __func__, p_file_seq->p_tmp_ves_buf);
    //FILE_SEQ_DEBUG("[%s] p_current_sys_vpts[0x%x] ...\n", __func__, p_file_seq->p_current_sys_vpts);
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
 *    unmap ves/aes  tmp buf which was allocated in linux kernel
 *
 */
void x_unmap_es_buffer(void * pHandle)
{
    FILE_SEQ_T * p_file_seq = pHandle;
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);
#ifdef __LINUX__
    audio_free_es_tmp_buf(p_file_seq->p_audio_dev);
    vdec_unmap_es_tmp_buf(p_file_seq->p_vdec_dev);
    audio_dec_deinit_es_buff(p_file_seq->p_audio_dev);
    vdec_dec_deinit_es_buff(p_file_seq->p_vdec_dev);
#else// for uc os

    if (p_file_seq->p_tmp_aes_buf) {
        mtos_free(p_file_seq->p_tmp_aes_buf);
        p_file_seq->p_tmp_aes_buf = NULL;
    }
#if 0
    if (p_file_seq->p_tmp_ves_buf) {
        mtos_free(p_file_seq->p_tmp_ves_buf);
        p_file_seq->p_tmp_ves_buf = NULL;
    }
#endif
#endif
    FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);
}

/*
 *
 *    unmap ves/aes  tmp buf which was allocated in linux kernel
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
void x_clear_pti_es_counter(void * pHandle)
{
    FILE_SEQ_T * p_file_seq = pHandle;
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);
#ifdef __LINUX__
    // vdec_clear_es_counter(p_file_seq->p_vdec_dev);
#else
#define MREAD(A) (*((volatile unsigned int *)(A)))
#define REG_WRITE(A,V)        ((*((volatile u32 *)(A))) = (V))
    u32 value;
    {
        PLAYBACK_INTERNAL_T * p_pb_internal =  p_file_seq->pb_internal;

        if ((p_pb_internal->chip_type == CHIPTYPE_WARRIORS) || \
            (p_pb_internal->chip_type == CHIPTYPE_SONATA)) {
            while (((MREAD(0xbfef0758) >> 16) & 0x7fff) != 0) {
                value = MREAD(0xbfef0714);
                REG_WRITE(0xbfef0714, (value | (1 << 20)));
            }

            while ((MREAD(0Xbfef0758) & 0xffff) != 0) {
                value = MREAD(0xbfef0714);
                REG_WRITE(0xbfef0714, (value | (1 << 16)));
            }

            u32 led_count = (MREAD(0xbfef0758) >> 16) & 0x7fff;
            FILE_SEQ_DEBUG("[%s] 0xbfef0758:0x%x\n", __func__, MREAD(0xbfef0758));
            //    FILE_SEQ_DEBUG("[%s] left audio es num: %d\n", __func__, led_count);
            FILE_SEQ_DEBUG("[%s] 0xbfef0714:0x%x\n", __func__, MREAD(0xbfef0714));
        }else if (p_pb_internal->chip_type == CHIPTYPE_CONCERTO) {
          dmx_av_reset( p_file_seq->p_pti_dev);
        }
    }
#endif
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
 *
 */
void x_start_av_decoder(void * pHandle)
{
    FILE_SEQ_T * p_file_seq = pHandle;
    int ret = 0;
    int size = 0;
    int align = 0;
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);
    x_clear_pti_es_counter(p_file_seq);
    last_vpts_upload = 0;

    if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
        if (p_file_seq->p_vdec_dev) {
            vdec_set_data_input(p_file_seq->p_vdec_dev, 1);
        }
    }

    if (g_pFwMemCfg.audio_mem_size == 0
        || g_pFwMemCfg.vdec_mem_size == 0
        || g_pFwMemCfg.p_vdec_start == 0
        || g_pFwMemCfg.p_audio_start == 0) {
        FILE_SEQ_DEBUG("[%s] ap should do memcfg fp seq\n", __func__);
        MT_ASSERT(0);
    }

    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);
    /*
     *   require necessary resource for audio firmware
     */
    aud_get_buf_requirement_vsb(p_file_seq->p_audio_dev, (u32 *)&size, (u32 *)&align);
    MT_ASSERT(g_pFwMemCfg.audio_mem_size >= size);
    FILE_SEQ_DEBUG("[%s] audio fw require memory resource:  %ld bytes\n", __func__, size);
    FILE_SEQ_DEBUG("[%s] system allocate memory for audio fw:  %ld bytes\n", __func__, g_pFwMemCfg.audio_mem_size);
    aud_set_buf_vsb(p_file_seq->p_audio_dev, (g_pFwMemCfg.p_audio_start), size);
    FILE_SEQ_DEBUG("[%s] audio fw start buffer: 0x%x\n", __func__, (g_pFwMemCfg.p_audio_start));

    if (p_file_seq->m_audio_codec_type != 0xffff) {
        set_audio_param_to_vsb();
    }

    p_file_seq->isAudMute = FALSE;
    FILE_SEQ_DEBUG("[%s] p_file_seq->m_audio_codec_type[%d]\n", __func__, p_file_seq->m_audio_codec_type);
    /*
     *   require necessary resource for video firmware
     */

    if (p_file_seq->only_audio_mode == FALSE) {
        if (p_file_seq->vdec_policy == 0) {
            vdec_get_buf_requirement(p_file_seq->p_vdec_dev, VDEC_BUFFER_AD, &size, &align);
        } else {
            vdec_get_buf_requirement(p_file_seq->p_vdec_dev, p_file_seq->vdec_policy, &size, &align);
        }

        MT_ASSERT(g_pFwMemCfg.vdec_mem_size >= size);
        FILE_SEQ_DEBUG("[%s] vdec fw require memory resource:  %ld bytes\n", __func__, size);
        FILE_SEQ_DEBUG("[%s] system allocate memory for vdec fw:  %ld bytes\n", __func__, g_pFwMemCfg.vdec_mem_size);

#ifndef CHIP_CONCERTO      
	if (p_file_seq->vdec_policy == 0) {
		vdec_set_buf(p_file_seq->p_vdec_dev, VDEC_BUFFER_AD, g_pFwMemCfg.p_vdec_start);
	} else {
		vdec_set_buf(p_file_seq->p_vdec_dev, p_file_seq->vdec_policy, g_pFwMemCfg.p_vdec_start);
	}
#else
	vdec_set_buf(p_file_seq->p_vdec_dev, p_file_seq->vdec_policy, g_pFwMemCfg.p_vdec_start);
#endif

	FILE_SEQ_DEBUG("[%s] video fw start buffer: 0x%x\n", __func__, g_pFwMemCfg.p_vdec_start);
        ret = vdec_start(p_file_seq->p_vdec_dev, p_file_seq->m_video_codec_type, 2);
        FILE_SEQ_DEBUG("[%s] p_file_seq->m_video_codec_type:%d\n", __func__, p_file_seq->m_video_codec_type);
        MT_ASSERT(SUCCESS == ret);
        //mtos_task_delay_ms(10);///???? fix me
    }

    FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);
}


/*
 *
 *
 *
 *
 *
 */
void x_init_av_device(void * pHandle)
{
    FILE_SEQ_T * p_file_seq = pHandle;
    int ret = 0;
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);
    static int inited = 0;

    if (inited) {
        p_file_seq->p_pti_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
        p_file_seq->p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
        p_file_seq->p_vdec_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
        p_file_seq->p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
        return;
    }

    inited = 1;
    /* pti */
    {
        p_file_seq->p_pti_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

        MT_ASSERT(NULL != p_file_seq->p_pti_dev);
        FILE_SEQ_DEBUG("Init pti success\n");
    }
    
    /* display */
    {
        p_file_seq->p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
        MT_ASSERT(NULL != p_file_seq->p_disp_dev);
        FILE_SEQ_DEBUG("Init disply success\n");
    }
    /* vdec */
    {
        p_file_seq->p_vdec_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
        MT_ASSERT(NULL != p_file_seq->p_vdec_dev);
        ret = dev_open(p_file_seq->p_vdec_dev, NULL);
        MT_ASSERT(SUCCESS == ret);
        FILE_SEQ_DEBUG("Init vdec success\n");
#if 0
        if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
            if (p_file_seq->p_vdec_dev) {
                vdec_set_data_input(p_file_seq->p_vdec_dev, 1);
            }
        }
#endif
    }
    /* audio */
    {
        p_file_seq->p_audio_dev = dev_find_identifier(NULL,
                                  DEV_IDT_TYPE,
                                  SYS_DEV_TYPE_AUDIO);
        MT_ASSERT(NULL != p_file_seq->p_audio_dev);
        ret = dev_open(p_file_seq->p_audio_dev, NULL);
        MT_ASSERT(SUCCESS == ret);
        FILE_SEQ_DEBUG("Init audio  success\n");
    }
    FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);
}
/*
 *
 *
 *
 *
 *
 */
void x_deinit_av_dev(void * pHandle)
{
    // TODO:
}


/********************************************
*
*RETURN:
*
*1  MUTE
*0  NOT MUTE
*
*
*********************************************/

#define  REG_AUD_VOL_WARRIORS  0xbfd40008
#define  REG_AUD_VOL_SONATA       0xbfd40008
#define  REG_AUD_VOL_CONCERTO   0xbf490008

int file_get_audio_mute_onoff()
{
    int ret = 0;
    PLAYBACK_INTERNAL_T * p_pb_internal = NULL;
    FILE_SEQ_CHIPTYPE chip_type;
    unsigned int volumn = 0;
    
#ifndef __LINUX__
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();

    if(p_file_seq && p_file_seq->pb_internal)
    {
         p_pb_internal = (PLAYBACK_INTERNAL_T *)p_file_seq->pb_internal;

         chip_type = p_pb_internal->chip_type;

         if(chip_type == CHIPTYPE_WARRIORS)
         {
             volumn = *(volatile unsigned int *)REG_AUD_VOL_WARRIORS;
             //OS_PRINTF("%s chip %d volumn %x \n",__func__,chip_type,volumn);
             volumn = volumn & 0x0000ffff;

             ret = volumn ? 0 : 1;             
         }
         else if(chip_type == CHIPTYPE_SONATA)
         {
              volumn = *(volatile unsigned int *)REG_AUD_VOL_WARRIORS;
             //OS_PRINTF("%s chip %d volumn %x \n",__func__,chip_type,volumn);
             volumn = volumn & 0x0000ffff;

             ret = volumn ? 0 : 1;         
         }
         else if(chip_type == CHIPTYPE_CONCERTO)
         {
             volumn = *(volatile unsigned int *)REG_AUD_VOL_WARRIORS;
             //OS_PRINTF("%s chip %d volumn %x \n",__func__,chip_type,volumn);
             volumn = volumn & 0x0000ffff;

             ret = volumn ? 0 : 1;          
         }
         else
         {
             FILE_SEQ_LOG("%s not supprot chiptype %d \n",__func__,chip_type);
             ret = 0;       
         }
    }
    
#else
    //to do
    //printf("")
#endif

    return ret;  
    
}

extern int play_to_recpos;

//extern unsigned int video_codec_id;
void   handle_pending_event()
{
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
#if  defined(NOT_PUSH_ES_TO_DECODER)
    return;
#endif
    //OS_PRINTF("[%s] -------p_file_seq->internal_event = %x\n",__func__,p_file_seq->internal_event);
#ifdef  AUTO_SEND_VPTS

    if (IS_SET(p_file_seq->internal_event, AUTO_DELIVER_VPTS)) {
        if (p_file_seq->is_ts != 1  && p_file_seq->only_audio_mode == 0 &&
            DEMUXER_TYPE_MPEG_PS != ((demuxer_t *)(p_file_seq->p_demuxer))->type) {
            unsigned int vpts = 0 ;
            unsigned int tmp = 0 ;
            unsigned long first_uploadpts = 0;
#if 0//delete by doreen, 2013-06-24 
            //#ifdef __LINUX__
            //          vpts = *(p_file_seq->p_current_sys_vpts);
            //#else
            vpts = vdec_dec_get_vpts(p_file_seq->p_vdec_dev);
            //#endif
#else
            vdec_info_t vstate;
            vstate.is_stable = 0;
            vstate.vid_format = 0;
            disp_sys_t  tmp_tv_fmt = 0;
            vdec_get_info(p_file_seq->p_vdec_dev, &vstate);
            vpts = vstate.pts;

            if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
                vpts = vpts & (0x7fffffff);
            }

            p_file_seq->is_stable = vstate.is_stable;

            if (vstate.image_info_enable == TRUE && vstate.sd_image_flag == FALSE
                && p_file_seq->vdec_policy == VDEC_SDINPUT_ONLY &&
                p_file_seq->checkDefinitionOK == FALSE) {
                p_file_seq->event_cb(FILE_PLAYBACK_HD_UNSUPPORT, video_codec_id);
                p_file_seq->checkDefinitionOK = TRUE;
            }

            if (vstate.no_enough_buffer) {
                p_file_seq->checkAvTypeOK = FALSE;
                if (p_file_seq->vdec_policy == VDEC_SDINPUT_ONLY)
                    p_file_seq->event_cb(FILE_PLAYBACK_HD_UNSUPPORT, video_codec_id);
                else
                    p_file_seq->event_cb(FILE_PLAYBACK_UNSUPPORT_MEMORY, video_codec_id);
                p_file_seq->is_play_to_end = 1;
                FILE_SEQ_LOG("[%s] -------vstate.UNSUPPORT,nobuff[%d],sd_img[%d],[%d],[%d]\n", 
                    __func__,vstate.no_enough_buffer,vstate.sd_image_flag,p_file_seq->video_disp_w,p_file_seq->video_disp_h);
            }

            //peacer del
#if 1

            if (p_file_seq->tv_sys_auto_set) {
                if (vstate.is_stable) {
                    tmp_tv_fmt = vstate.vid_format;
                    //FILE_SEQ_DEBUG("fmt[%d]max[%d]@@\n", tmp_tv_fmt, VID_SYS_MAX);

                    if ((tmp_tv_fmt != p_file_seq->cur_tv_hd_fmt) && (vstate.vid_format != VID_SYS_MAX)) {
                        p_file_seq->cur_tv_hd_fmt = tmp_tv_fmt;
                        file_seq_switch_tv_sys(p_file_seq);
                    }
                }
            }

#endif
#endif
            //vpts = vdec_dec_get_vpts(p_file_seq->p_vdec_dev);
            //last_send_orig_vpts = p_file_seq->orig_vpts;
            //The first vpts is error, discard it!
            //OS_PRINTF("[%s] ------ vpts = %ld  , vpts_upload = %ld \n",__func__,vpts,p_file_seq->vpts_upload);

            if (p_file_seq->vpts_upload == 0) {
#if 0//delete by doreen, 2013-06-24         
                vdec_info_t vstate;
                vdec_get_info(p_file_seq->p_vdec_dev, &vstate);
#endif

                //OS_PRINTF("[%s] ------ vstate.decoder_frame_count = %d \n",__func__,vstate.decoder_frame_count);
                if (vstate.decoder_frame_count < 5) {
                    mtos_task_sleep(1);
                    //return ;
                } else {
                    FILE_SEQ_DEBUG("[%s] ------ frame_count = %d, vpts_upload = %d\n", __func__, vstate.decoder_frame_count, p_file_seq->vpts_upload);
                    p_file_seq->vpts_upload = 1;
                    //return;
                }
            } else {
                if (vpts != 0) {
                    if (p_file_seq->cur_speed >= TS_SEQ_REV_FAST_PLAY_2X &&  p_file_seq->cur_speed <= TS_SEQ_REV_FAST_PLAY_32X) {
                        //tirck play, fast back
                        //FILE_SEQ_DEBUG("[%s] ------ cur_speed %d ! vpts = %ld ms , vpts_upload = %ld ms, orig vpts =  %ld ms\n", __func__, p_file_seq->cur_speed, vpts/45, (p_file_seq->vpts_upload)/45, (int)(p_file_seq->orig_vpts));
                        //vpts = (int)(p_file_seq->orig_vpts) * TIME_BASE;
                        //tmp = p_file_seq->vpts_upload - DELIVER_VPTS_INTERVAL;
                        tmp = p_file_seq->vpts_upload - DELIVER_VPTS_INTERVAL;

                        //OS_PRINTF("[%s] ------ vpts = %x , vpts_upload = %x, tmp = %x\n",__func__,vpts,p_file_seq->vpts_upload,tmp);
                        if (vpts <= tmp) {
                            p_file_seq->vpts_upload = vpts;

                            if (NULL != p_file_seq->event_cb) {
                                first_uploadpts = ((unsigned long)p_file_seq->first_vpts) * TIME_BASE;
                                trick_pts = (float)p_file_seq->vpts_upload / (1000 * TIME_BASE);

                                //mtos_printk_f("start pts %f \n",p_file_seq->first_vpts);
                                //OS_PRINTF("first upload pts %d ,upload pts %d\n",first_uploadpts,p_file_seq->vpts_upload);
                                if (first_uploadpts > p_file_seq->vpts_upload) {
                                    p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS, 0 / TIME_BASE);
                                } else {
                                    p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS, (p_file_seq->vpts_upload - first_uploadpts) / TIME_BASE);
                                }
                            }
                        }
                    } else {
                        //normal play or trick paly(fast forward)
                        if (vpts < p_file_seq->vpts_upload && p_file_seq->seek_seconds < 0) {
                            FILE_SEQ_DEBUG("[%s] ------ seek back %d ms! vpts = %ld , vpts_upload = %ld\n", __func__, (int)(p_file_seq->seek_seconds * 1000), vpts, p_file_seq->vpts_upload);
                            p_file_seq->vpts_upload = vpts;
                            p_file_seq->seek_seconds = -1;
                        }

                        tmp = p_file_seq->vpts_upload + DELIVER_VPTS_INTERVAL;

                        //OS_PRINTF("[%s] ------ vpts = %x , vpts_upload = %x, tmp = %x\n",__func__,vpts,p_file_seq->vpts_upload,tmp);
                        if (vpts > tmp) {
                            p_file_seq->vpts_upload = vpts;

                            if (NULL != p_file_seq->event_cb) {
                                first_uploadpts = ((unsigned long)p_file_seq->first_vpts) * TIME_BASE;
                                trick_pts = (float)p_file_seq->vpts_upload / (1000 * TIME_BASE);

                                //mtos_printk_f("start pts %f \n",p_file_seq->first_vpts);
                                //OS_PRINTF("first upload pts %d ,upload pts %d\n",first_uploadpts,p_file_seq->vpts_upload);
                                if (first_uploadpts > p_file_seq->vpts_upload) {
                                    p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS, 0 / TIME_BASE);
                                } else {
                                    p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS, (p_file_seq->vpts_upload - first_uploadpts) / TIME_BASE);
                                }
                            }

#if 0//only for internal debug !!!!!!
                            unsigned int  cur_hour = ((vpts_upload / TIME_BASE) / 1000) / 3600;
                            unsigned int  cur_min = (((vpts_upload / TIME_BASE) / 1000) - (cur_hour * 3600)) / 60;
                            unsigned int  cur_sec = (((vpts_upload / TIME_BASE) / 1000) - (cur_hour * 3600)) - cur_min * 60;
                            FILE_SEQ_DEBUG("vpts_upload [%ld:%ld:%ld],  %ld ms\n", cur_hour, cur_min, cur_sec, (vpts_upload / TIME_BASE));
#endif
                        }
                    }
                }
            }
        } else if (p_file_seq->only_audio_mode) {
            int  cur_aud_freespace = 0;
            float   aes_buf_consume_ms = 0.0;
#ifdef ES_END_IN_TASK

            if (p_file_seq->init_buffering_finish != 1) {
                aud_file_getleftes_vitual_buffer(p_file_seq, (u32 *)&cur_aud_freespace);
            } else
#endif
                aud_file_getleftesbuffer_vsb(p_file_seq->p_audio_dev, (u32 *)&cur_aud_freespace);

            aes_buf_consume_ms = (((float)(p_file_seq->dec_cap.max_aes_num - cur_aud_freespace) * 1.0) / (p_file_seq->audio_bps) * 1.0) * 1000.0;
            //FILE_SEQ_DEBUG("[%s] max_aes_num[%ld]!!!\n", __func__,p_file_seq->dec_cap.max_aes_num);
            //FILE_SEQ_DEBUG("[%s]  cur_aud_freespace[%d]!!!\n", __func__,(u32)cur_aud_freespace);
            //FILE_SEQ_DEBUG("[%s] aes_buf_consume_ms[%d]!!!\n", __func__,(u32)aes_buf_consume_ms);
            // FILE_SEQ_DEBUG("[%s] orig_apts[%d]!!!\n", __func__,(u32)(p_file_seq->orig_apts));
            //FILE_SEQ_DEBUG("[%s] apts_upload[%ld]!!!\n", __func__,p_file_seq->apts_upload);

            if (p_file_seq->orig_apts -  aes_buf_consume_ms  > (float)(p_file_seq->apts_upload) + 1000.0) {
                p_file_seq->apts_upload = (u32)(p_file_seq->orig_apts -  aes_buf_consume_ms);

                if (NULL != p_file_seq->event_cb) {
                    if(aes_buf_consume_ms>0){
                    p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS, p_file_seq->apts_upload);
                    FILE_SEQ_DEBUG("send apts %ld,%u,%u\n",p_file_seq->apts_upload,(u32)(p_file_seq->orig_apts),(u32)(aes_buf_consume_ms));
                    }
                }
            }
        } else if (p_file_seq->is_ts || DEMUXER_TYPE_MPEG_PS == ((demuxer_t *)(p_file_seq->p_demuxer))->type) {
            unsigned int vpts = 0 ;
            unsigned int tmp = 0 ;
            vdec_info_t vstate;
            vstate.is_stable = 0;
            vstate.vid_format = 0;
            disp_sys_t  tmp_tv_fmt = 0;
            vdec_get_info(p_file_seq->p_vdec_dev, &vstate);
            vpts = vstate.pts;
#if 0   //linda zhu removed to get the correct vpts.
            if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
                vpts = vpts & (0x7fffffff);
            }
#endif
            p_file_seq->is_stable = vstate.is_stable;


            if (vstate.no_enough_buffer) {
                p_file_seq->checkAvTypeOK = FALSE;
                if (p_file_seq->vdec_policy == VDEC_SDINPUT_ONLY)
                    p_file_seq->event_cb(FILE_PLAYBACK_HD_UNSUPPORT, video_codec_id);
                else
                    p_file_seq->event_cb(FILE_PLAYBACK_UNSUPPORT_MEMORY, video_codec_id);
                p_file_seq->is_play_to_end = 1;
                FILE_SEQ_LOG("[%s] -------vstate.UNSUPPORT,nobuff[%d],sd_img[%d],[%d],[%d]\n", 
                    __func__,vstate.no_enough_buffer,vstate.sd_image_flag,p_file_seq->video_disp_w,p_file_seq->video_disp_h);
            }

            if (p_file_seq->tv_sys_auto_set) {
                if (vstate.is_stable) {
                    tmp_tv_fmt = vstate.vid_format;
                    //FILE_SEQ_DEBUG("fmt[%d]max[%d]@@\n", tmp_tv_fmt, VID_SYS_MAX);

                    if ((tmp_tv_fmt != p_file_seq->cur_tv_hd_fmt) && (vstate.vid_format != VID_SYS_MAX)) {
                        p_file_seq->cur_tv_hd_fmt = tmp_tv_fmt;
                        file_seq_switch_tv_sys(p_file_seq);
                    }
                }
            }

            if (p_file_seq->vpts_upload == 0) {
                //OS_PRINTF("[%s] ------ vstate.decoder_frame_count = %d \n",__func__,vstate.decoder_frame_count);
                if (vstate.decoder_frame_count < 5) {
                    mtos_task_sleep(1);
                    //return ;
                } else {
                    FILE_SEQ_DEBUG("[%s] ------ frame_count = %d, vpts_upload = %d\n", __func__, vstate.decoder_frame_count, p_file_seq->vpts_upload);
                    p_file_seq->vpts_upload = 1;
                    //return;
                }
            } else {
                if (p_file_seq->cur_speed >= TS_SEQ_REV_FAST_PLAY_2X &&  p_file_seq->cur_speed <= TS_SEQ_REV_FAST_PLAY_32X) {
                    //tirck play, fast back
                    //FILE_SEQ_DEBUG("[%s] ------ cur_speed %d ! vpts = %ld ms , vpts_upload = %ld ms, orig vpts =  %ld ms\n", __func__, p_file_seq->cur_speed, vpts/45, (p_file_seq->vpts_upload)/45, (int)(p_file_seq->orig_vpts));
                    vpts = (int)(p_file_seq->orig_vpts - p_file_seq->first_vpts) * TIME_BASE;
                    tmp = p_file_seq->vpts_upload - DELIVER_VPTS_INTERVAL;
                    // tmp = p_file_seq->vpts_upload;

                    //OS_PRINTF("[%s] ------ vpts = %x , vpts_upload = %x, tmp = %x\n",__func__,vpts,p_file_seq->vpts_upload,tmp);
                    if (vpts <= tmp) {
                        p_file_seq->vpts_upload = vpts;

                        if (NULL != p_file_seq->event_cb) {
                            trick_pts = p_file_seq->first_vpts / 1000 + (float)p_file_seq->vpts_upload / (1000 * TIME_BASE);
                            p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS, p_file_seq->vpts_upload / TIME_BASE);
                        }
                    }
                } else {
                    if (vpts != 0) {
                        if (p_file_seq->orig_vpts < p_file_seq->first_vpts) {
                            p_file_seq->first_vpts = p_file_seq->orig_vpts;
                            vpts = 0;
                        }

                        if (vpts > p_file_seq->first_vpts * TIME_BASE) {
                            vpts -= p_file_seq->first_vpts * TIME_BASE;
                        } else {
                            vpts = 0;
                        }

                        //vpts +=p_file_seq->apts_upload;
                        //normal play or trick paly(fast forward)
                        if (vpts < p_file_seq->vpts_upload && p_file_seq->seek_seconds < 0) {
                            FILE_SEQ_DEBUG("[%s] ------ seek back %d ms! vpts = %ld , vpts_upload = %ld\n", __func__, (int)(p_file_seq->seek_seconds * 1000), vpts, p_file_seq->vpts_upload);
                            p_file_seq->vpts_upload = vpts;
                            p_file_seq->seek_seconds = -1;
                        }

                        tmp = p_file_seq->vpts_upload + DELIVER_VPTS_INTERVAL;

                        //OS_PRINTF("[%s] ------ vpts = %x , vpts_upload = %x, tmp = %x\n",__func__,vpts,p_file_seq->vpts_upload,tmp);
                        if (vpts > tmp) {
                            p_file_seq->vpts_upload = vpts;
                            trick_pts = p_file_seq->first_vpts / 1000 + (float)p_file_seq->vpts_upload / (1000 * TIME_BASE);

                            if (NULL != p_file_seq->event_cb) {
                                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_GET_VPTS, p_file_seq->vpts_upload / TIME_BASE);
                            }

#if 0//only for internal debug !!!!!!
                            unsigned int  cur_hour = ((p_file_seq->vpts_upload / TIME_BASE) / 1000) / 3600;
                            unsigned int  cur_min = (((p_file_seq->vpts_upload / TIME_BASE) / 1000) - (cur_hour * 3600)) / 60;
                            unsigned int  cur_sec = (((p_file_seq->vpts_upload / TIME_BASE) / 1000) - (cur_hour * 3600)) - cur_min * 60;
                            FILE_SEQ_DEBUG("vpts_upload [%ld:%ld:%ld],  %ld ms\n", cur_hour, cur_min, cur_sec, (p_file_seq->vpts_upload / TIME_BASE));
#endif
                        }
                    }
                }
            }
        }
    }

#endif

    if (IS_SET(p_file_seq->internal_event, CHECK_TRICKPLAY)) {
        //printf("################TRICK PLAY EVENT\n");
        if (p_file_seq->cur_speed >= TS_SEQ_REV_FAST_PLAY_2X) {
            //back to start and play normal
            //printf("vpts %f,first vpts %f\n",p_file_seq->orig_vpts,p_file_seq->first_vpts);
            //mtos_printk_f("vpts_upload %f \n",(double)vstate.pts / (1000 * TIME_BASE));
            int speed_index = 0;

            switch (p_file_seq->cur_speed) {
                case TS_SEQ_REV_FAST_PLAY_2X:
                    speed_index = 2;
                    break;
                case TS_SEQ_REV_FAST_PLAY_4X:
                    speed_index = 4;
                    break;
                case TS_SEQ_REV_FAST_PLAY_8X:
                    speed_index = 8;
                    break;
                case TS_SEQ_REV_FAST_PLAY_16X:
                    speed_index = 25;
                    break;
                case TS_SEQ_REV_FAST_PLAY_32X:
                    speed_index = 50;
                    break;
                default:
                    speed_index = 16;
                    break;
            }

            speed_index = 16 >= speed_index ? 16 : speed_index;

            if (p_file_seq->orig_vpts - p_file_seq->first_vpts < 0.5 * 1000 &&
                (p_file_seq->vpts_upload / (1000 * TIME_BASE) < speed_index)) {
                FILE_SEQ_DEBUG("****** back to head play normal \n");
                trick_pts = 0.0;
                p_file_seq->vpts_upload = 3;

		vdec_freeze_stop(p_file_seq->p_vdec_dev);             
		vdec_file_clearesbuffer(p_file_seq->p_vdec_dev); //linda zhu, fix bug 54642, write_pointer of es_buffer is not reset in vdec_file_clearesbuffer of CHIPTYPE_CONCERTO, vdec need to be restarted.
		vdec_set_avsync_mode(p_file_seq->p_vdec_dev, VDEC_AVSYNC_FILEPLAY);
		vdec_start(p_file_seq->p_vdec_dev, p_file_seq->m_video_codec_type, 2);
		FILE_SEQ_LOG("[%s] p_file_seq->m_video_codec_type:%d\n", __func__, p_file_seq->m_video_codec_type);
		vdec_resume(p_file_seq->p_vdec_dev);
		vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_NORMAL, 1);
		 
                p_file_seq->cur_speed = TS_SEQ_NORMAL_PLAY;
                p_file_seq->last_speed = TS_SEQ_NORMAL_PLAY;
                p_file_seq->tmp_speed = 0;
                //aud_mute_onoff_vsb(p_file_seq->p_audio_dev, FALSE);
                //mute set by up layer
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
                }

                //mtos_task_delay_ms(50);
                p_file_seq->last_speed = p_file_seq->cur_speed;
                p_file_seq->needNewAudioData = TRUE;
                p_file_seq->needNewVideoData = TRUE;
                //p_file_seq->m_tmp_ves_buf_pos = 0;
                p_file_seq->isAudioBufferFull = FALSE;
                p_file_seq->isVideoBufferFull = FALSE;
		if (NULL != p_file_seq->event_cb) { 
			p_file_seq->event_cb(FILE_PLAYBACK_SEQ_INVALID, 0); 
			FILE_SEQ_DEBUG("send event playback to head\n");
		}
                if (NULL != p_file_seq->event_cb) {
                    p_file_seq->event_cb(FILE_PLAYBACK_CHECK_TRICKPLAY, 0);
                    FILE_SEQ_DEBUG("send event trickmode\n");
                }

                ClearFlag(p_file_seq->internal_event, CHECK_TRICKPLAY);
		if(p_file_seq->isAudMute==0)
		{
			aud_mute_onoff_vsb(p_file_seq->p_audio_dev, FALSE);
		}			
				
            }
        } else if(((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->is_timeshift==1 ){
            //back to start and play normal
            //printf("vpts %f,first vpts %f\n",p_file_seq->orig_vpts,p_file_seq->first_vpts);
            //mtos_printk_f("vpts_upload %f \n",(double)vstate.pts / (1000 * TIME_BASE));
      
   //  mtos_printk("\n%s %d\n",__func__,__LINE__);
   
            if (play_to_recpos) {
                FILE_SEQ_DEBUG("****** back to head play normal \n");
				     //mtos_printk("\n%s %d\n",__func__,__LINE__);
                //trick_pts = 0.0;
                //p_file_seq->vpts_upload = 1;

                    if ( p_file_seq->cur_speed != TS_SEQ_FAST_PLAY_2X) {
		vdec_freeze_stop(p_file_seq->p_vdec_dev);             
		vdec_file_clearesbuffer(p_file_seq->p_vdec_dev); //linda zhu, fix bug 54642, write_pointer of es_buffer is not reset in vdec_file_clearesbuffer of CHIPTYPE_CONCERTO, vdec need to be restarted.
		vdec_set_avsync_mode(p_file_seq->p_vdec_dev, VDEC_AVSYNC_FILEPLAY);
		vdec_start(p_file_seq->p_vdec_dev, p_file_seq->m_video_codec_type, 2);
		FILE_SEQ_LOG("[%s] p_file_seq->m_video_codec_type:%d\n", __func__, p_file_seq->m_video_codec_type);
		vdec_resume(p_file_seq->p_vdec_dev);
		vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_NORMAL, 1);
                


                if (p_file_seq->m_audio_codec_type != 0xffff) {
                    u32 size = 0;
                    u32 align = 0;
                    aud_get_buf_requirement_vsb(p_file_seq->p_audio_dev, &size, &align);
                    aud_set_buf_vsb(p_file_seq->p_audio_dev,
                                    (g_pFwMemCfg.p_audio_start) | 0xa0000000,
                                    size);
                    set_audio_param_to_vsb();
                }
                    	}
else
{
          vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_NORMAL, 1);
}

									p_file_seq->cur_speed = TS_SEQ_NORMAL_PLAY;
                p_file_seq->last_speed = TS_SEQ_NORMAL_PLAY;
                p_file_seq->tmp_speed = 0;
                //aud_mute_onoff_vsb(p_file_seq->p_audio_dev, FALSE);
                //mute set by up layer
                //send to es open
                //audio start
                p_file_seq->isNormalPlay = TRUE;
                p_file_seq->isTrickPlay = FALSE;
                p_file_seq->isTrickToNormal = TRUE;
                //mtos_task_delay_ms(50);
                p_file_seq->last_speed = p_file_seq->cur_speed;
                p_file_seq->needNewAudioData = TRUE;
                p_file_seq->needNewVideoData = TRUE;
                //p_file_seq->m_tmp_ves_buf_pos = 0;
                p_file_seq->isAudioBufferFull = FALSE;
                p_file_seq->isVideoBufferFull = FALSE;

                if (NULL != p_file_seq->event_cb) {
                    p_file_seq->event_cb(FILE_PLAYBACK_CHECK_TRICKPLAY, 0);
                    FILE_SEQ_DEBUG("send event trickmode\n");
                }

                ClearFlag(p_file_seq->internal_event, CHECK_TRICKPLAY);
				     mtos_printk("\n%s %d\n",__func__,__LINE__);
		if(p_file_seq->isAudMute==0)
		{
		    // mtos_printk("\n%s %d\n",__func__,__LINE__);
			aud_mute_onoff_vsb(p_file_seq->p_audio_dev, FALSE);
		}	
		if (NULL != p_file_seq->event_cb) { 
			p_file_seq->event_cb(FILE_PLAYBACK_SEQ_ES_TASK_DEAD, 0); 
			FILE_SEQ_DEBUG("send event playback to head\n");
		}
			
			play_to_recpos = 0;
				
            }
		
        }
        	
        else{
            ClearFlag(p_file_seq->internal_event, CHECK_TRICKPLAY);
        }
    }

    if (IS_SET(p_file_seq->internal_event, CLEAR_AUD_ES_BUF)) {
        ClearFlag(p_file_seq->internal_event, CLEAR_AUD_ES_BUF);
        return ;
    }

    if (IS_SET(p_file_seq->internal_event, CLEAR_VIDEO_ES_BUF)) {
        ClearFlag(p_file_seq->internal_event, CLEAR_VIDEO_ES_BUF);
        return ;
    }

    if (IS_SET(p_file_seq->internal_event, GET_TS_MEDIA_INFO) || IS_SET(p_file_seq->internal_event, GET_ES_VIDEO_W_H)) {
        //Check if the video info can be received
        vdec_info_t * p_vdec_info;
        VIDEO_W_H_FPS * video_whfps;
        int ret = -1;
        //p_vdec_info = (vdec_info_t *)malloc(sizeof(vdec_info_t ));
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        p_vdec_info = (vdec_info_t *)file_seq_mem_alloc(p_file_seq, sizeof(vdec_info_t));
#else
        p_vdec_info = (vdec_info_t *)mtos_malloc(sizeof(vdec_info_t));
#endif
        memset(p_vdec_info, 0, sizeof(vdec_info_t));
        ret = vdec_get_info(p_file_seq->p_vdec_dev, p_vdec_info);

        if (!ret && p_vdec_info->decoder_frame_count > 4) { //ret success
            p_file_seq->video_disp_w = p_vdec_info->width;
            p_file_seq->video_disp_h = p_vdec_info->heigth;
            p_file_seq->video_fps = p_vdec_info->frame_rate;
            //OS_PRINTF("[%s] -------- vdec_get_info: %dx%d (w x h), %d fps\n",__func__,\
            p_file_seq->video_disp_w, p_file_seq->video_disp_h, p_file_seq->video_fps);
#ifdef AUTO_SEND_VIDEO_INFO
            //video_whfps = malloc(sizeof(VIDEO_W_H_FPS));
            p_file_seq->video_whfps.video_disp_w = p_file_seq->video_disp_w;
            p_file_seq->video_whfps.video_disp_h = p_file_seq->video_disp_h;
            p_file_seq->video_whfps.video_fps = p_file_seq->video_fps;
#endif

            if (p_file_seq->video_disp_w != 0 && p_file_seq->video_disp_h != 0) { //&& p_file_seq->video_fps!=0
            OS_PRINTF("[%s] ------ Get video info from vdec_get_info: %dx%d (w x h), %d fps\n", __func__, \
                      p_file_seq->video_disp_w, p_file_seq->video_disp_h, p_file_seq->video_fps);
#ifdef  AUTO_SEND_VIDEO_INFO

                if (NULL != p_file_seq->event_cb) {
                    p_file_seq->event_cb(FILE_PLAYBACK_SEQ_RECEIVE_VIDEO_INFO, (u32) & (p_file_seq->video_whfps));
                }

#endif

                if (IS_SET(p_file_seq->internal_event, GET_TS_MEDIA_INFO)) {
                    ClearFlag(p_file_seq->internal_event, GET_TS_MEDIA_INFO);
                } else {
                    ClearFlag(p_file_seq->internal_event, GET_ES_VIDEO_W_H);
                }
            }
        }

#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        file_seq_mem_free(p_file_seq, p_vdec_info);
#else
        mtos_free(p_vdec_info);
#endif
        return ;
    }

    if (IS_SET(p_file_seq->internal_event, CHECK_SYSTEM_MEM)) {
        BOOL isFlushCacheMem = TRUE;
#ifdef __LINUX__
        run_memory(isFlushCacheMem);
#endif
        ClearFlag(p_file_seq->internal_event, CHECK_SYSTEM_MEM);
        return ;
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
 *
 */
BOOL adec_push_audio_es(u8 * start_a, int in_size_a, u32 apts)

{
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    int audioFrameLen = in_size_a, curSpaceSize = 0;
    int curRdAudNum = 0, totalRdAesCnt = 0;
    int ret = 0;

    while (audioFrameLen > 0) {
        if ((p_file_seq->m_aes_left + audioFrameLen) >= 56) {
            curRdAudNum = (56 - p_file_seq->m_aes_left);
            memcpy(p_file_seq->p_tmp_aes_buf + p_file_seq->m_aes_left,
                   start_a + totalRdAesCnt, (56 - p_file_seq->m_aes_left));
            totalRdAesCnt += (56 - p_file_seq->m_aes_left);
        } else {
            memcpy(p_file_seq->p_tmp_aes_buf + p_file_seq->m_aes_left,
                   start_a + totalRdAesCnt, audioFrameLen);
            p_file_seq->m_aes_left += audioFrameLen;
            audioFrameLen = 0;
            continue;
        }

        p_file_seq->m_aes_left = 0;

        while (1) {
            // TODO:   it seems that it is not nessceray to check space of aes ring buffer !!!
#ifdef ES_END_IN_TASK
            if (p_file_seq->init_buffering_finish != 1) {
                aud_file_getleftes_vitual_buffer(p_file_seq, (u32 *)&curSpaceSize);
            } else
#endif
                aud_file_getleftesbuffer_vsb(p_file_seq->p_audio_dev, (u32 *)&curSpaceSize);

            if (curSpaceSize > 64) {
                curSpaceSize = 0;
#ifdef ES_END_IN_TASK

                if (p_file_seq->init_buffering_finish != 1)
                    aud_file_push_vitual_esbuffer_vsb(p_file_seq, (u32)p_file_seq->p_tmp_aes_buf,
                                                      56, apts);
                else
#endif
                    ret = aud_file_pushesbuffer_vsb(p_file_seq->p_audio_dev, (u32)p_file_seq->p_tmp_aes_buf, 56, apts);

                if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
                    if (ret != ERR_TIMEOUT) {
                        audioFrameLen -= curRdAudNum;
                    }
                } else {
                    audioFrameLen -= curRdAudNum;
                }

                break;
            } else if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
                mtos_task_delay_ms(10);
            }
        }
    }

    return TRUE;
}

/*******************************************************************************************
 *
 *
 *   NOTICE1:  <1> don't change 'p_file_seq->m_play_state'  in update_BPS()   //peacer add 20140222
 *
 *
 *
 *
 *
 *
 *
 *
 *************************************************************************************************/


void  update_BPS()
{
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    BUFFERING_PARAM_T  *  p_buffering_param = NULL;// = p_file_seq->p_buffering;
    u32   videoFreeSpaceSize = 0;
    u32   cur_BPS = 0;
    u32   cur_percentage = 0;
    u32   total_water = 0;
    int    cur_water = 0;
    float  tmp_percent = 0.0;
    u32  bps_percent_val = 0;
    int diff_ves_num = 0;

    if (p_file_seq) {
        p_buffering_param = p_file_seq->p_buffering;

        if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state == FILE_SEQ_BUFFERING) {
            p_buffering_param->cur_tick = mtos_ticks_get();

            if (p_buffering_param->cur_tick > p_buffering_param->old_tick + 100) {
#ifdef ES_END_IN_TASK

                if (p_file_seq->init_buffering_finish != 1) {
                    vdec_get_vitual_es_buf_space(p_file_seq, (u32 *)&videoFreeSpaceSize);
                } else
#endif
                    vdec_get_es_buf_space(p_file_seq->p_vdec_dev, &videoFreeSpaceSize);

                p_buffering_param->cur_ves_num = p_file_seq->max_ves_size - videoFreeSpaceSize;
                diff_ves_num = 0;
                diff_ves_num = p_buffering_param->cur_ves_num - p_buffering_param->old_ves_num;

                if (diff_ves_num < 0) {
                    diff_ves_num = 0;
                }

                if (p_buffering_param->cur_tick - p_buffering_param->old_tick) {
                    cur_BPS = (diff_ves_num) / (((p_buffering_param->cur_tick - p_buffering_param->old_tick) * 10) / 1000);
                }

                p_buffering_param->old_tick = p_buffering_param->cur_tick;
                p_buffering_param->old_ves_num = p_buffering_param->cur_ves_num;
                cur_percentage = 0;
                total_water = p_buffering_param->end_buffering_wl - p_buffering_param->start_buffering_wl;
                cur_water = p_buffering_param->cur_ves_num - p_buffering_param->start_buffering_wl;

                if (cur_water < 0) {
                    cur_water = 0;
                }

                if (total_water) {
                    tmp_percent = (cur_water * 1.0) / (total_water * 1.0);
                }

                cur_percentage = (u32)((tmp_percent) * 100.0);

                if (cur_percentage > 100) {
                    cur_percentage = 100;
                }

                bps_percent_val = (cur_BPS & 0x0000ffff) | (cur_percentage << 16);
                FILE_SEQ_LOG(">>>11 VES [%ld KB]  [%ld KB/sec]  [%d]%!!!<<<\n",
                             p_buffering_param->cur_ves_num, cur_BPS, cur_percentage);
                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_UPDATE_BPS, bps_percent_val);

                //if duration of buffering is more than 8 seconds, we will suggest
                // app change to a new source
                if (p_buffering_param->cur_tick > p_buffering_param->start_tick + 8 * 100) {
                    p_file_seq->event_cb(FILE_PLAYBACK_SEQ_REQUEST_CHANGE_SRC, cur_BPS);
                }
            }
        } else if (p_file_seq->m_play_state == FILE_SEQ_PLAY) {
            videoFreeSpaceSize = 0;
#ifdef ES_END_IN_TASK

            if (p_file_seq->init_buffering_finish != 1) {
                vdec_get_vitual_es_buf_space(p_file_seq, (u32 *)&videoFreeSpaceSize);
            } else
#endif
                vdec_get_es_buf_space(p_file_seq->p_vdec_dev, &videoFreeSpaceSize);

            /*ves is  underflow*/
            if (videoFreeSpaceSize >
                p_file_seq->max_ves_size - p_buffering_param->start_buffering_wl && p_file_seq->isVideoEsEnd != TRUE && p_file_seq->isAudioEsEnd != TRUE) {
                p_buffering_param->cur_tick = mtos_ticks_get();

                if (p_buffering_param->start_show_buffering_in_playstate == FALSE) {
                    p_buffering_param->old_tick = p_buffering_param->cur_tick;
                    p_buffering_param->start_tick = p_buffering_param->cur_tick;
                    p_buffering_param->cur_ves_num = p_file_seq->max_ves_size - videoFreeSpaceSize;
                    p_buffering_param->old_ves_num = p_buffering_param->cur_ves_num;
                    p_buffering_param->start_show_buffering_in_playstate = TRUE;
                }

                if (p_buffering_param->cur_tick > p_buffering_param->old_tick + 100) {
                    p_buffering_param->cur_ves_num = p_file_seq->max_ves_size - videoFreeSpaceSize;
                    diff_ves_num = 0;
                    diff_ves_num = p_buffering_param->cur_ves_num - p_buffering_param->old_ves_num;

                    if (diff_ves_num < 0) {
                        diff_ves_num = 0;
                    }

                    if (p_buffering_param->cur_tick - p_buffering_param->old_tick) {
                        cur_BPS = diff_ves_num / (((p_buffering_param->cur_tick - p_buffering_param->old_tick) * 10) / 1000);
                    }

                    p_buffering_param->old_tick = p_buffering_param->cur_tick;
                    p_buffering_param->old_ves_num = p_buffering_param->cur_ves_num;
                    cur_percentage = 0;
                    total_water = p_buffering_param->end_buffering_wl - p_buffering_param->start_buffering_wl;
                    cur_water = p_buffering_param->cur_ves_num - p_buffering_param->start_buffering_wl;

                    if (cur_water < 0) {
                        cur_water = 0;
                    }

                    if (total_water) {
                        tmp_percent = (cur_water * 1.0) / (total_water * 1.0);
                    }

                    cur_percentage = (u32)((tmp_percent) * 100.0);

                    if (cur_percentage > 100) {
                        cur_percentage = 100;
                    }

                    bps_percent_val = (cur_BPS & 0x0000ffff) | (cur_percentage << 16);
                    FILE_SEQ_LOG(">>>22 VES [%ld KB]  [%ld KB/sec]  [%d]%!!!<<<\n", p_buffering_param->cur_ves_num, cur_BPS, cur_percentage);
                    p_file_seq->event_cb(FILE_PLAYBACK_SEQ_UPDATE_BPS, bps_percent_val);

                    //if duration of buffering is more than 8 seconds, we will suggest
                    // app change to a new source
                    if (p_buffering_param->cur_tick > p_buffering_param->start_tick + 8 * 100) {
                        p_file_seq->event_cb(FILE_PLAYBACK_SEQ_REQUEST_CHANGE_SRC, cur_BPS);
                    }
                }
            } else {
                if (p_buffering_param->start_show_buffering_in_playstate) {
                    p_file_seq->event_cb(FILE_PLAYBACK_FINISH_UPDATE_BPS, 0);
                }

                p_buffering_param->start_show_buffering_in_playstate = FALSE;
            }
        } else if (p_file_seq->m_play_state == FILE_SEQ_LOADMEDIA) {
            p_buffering_param->cur_tick = mtos_ticks_get();

            if (p_buffering_param->start_show_loadmedia_time == FALSE) {
                p_buffering_param->old_tick = p_buffering_param->cur_tick;
                p_buffering_param->start_tick = p_buffering_param->cur_tick;
                p_buffering_param->start_show_loadmedia_time = TRUE;
            }

            if (p_buffering_param->cur_tick > p_buffering_param->old_tick + 100) {
                u32 seconds = 0;
                seconds = (p_buffering_param->cur_tick - p_buffering_param->start_tick) * 10 / 1000;
                p_buffering_param->old_tick = p_buffering_param->cur_tick;
                p_file_seq->event_cb(FILE_PLAYBACK_SEQ_LOAD_MEDIA_TIME, seconds);
                FILE_SEQ_LOG("@@@@loadmedia has consumed %d seconds!!!!!!!\n", seconds);
            }
        }
    }
}
/*
 *
 *
 *
 *     The audio packet maybe come from  demuxer's buffer of ffmepg
 *                          preload buffer or audio buffering buffer
 *
 *
 *
 *
 *
 *
 */
int ds_get_buffer_pkt_audio(demux_stream_t * ds, unsigned char ** start, uint8_t ** extra_buf, uint8_t * extra_size)
{
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    MT_ASSERT(p_file_seq != NULL);
    es_pkt_data_t * p_node =  NULL;
    preload_buffer_t * p_audio_buffer = &p_file_seq->preload_audio_buffer;
    *start = NULL;
    *extra_buf = NULL;
    *extra_size = 0;
#if 0//del audio buffering
    AUDIO_PACKET_T  * p_audio_packet = NULL;//PEACER ADD
    /*
    *
    *
    *  At first , we fetch audio packets from audio buffering list
    *
    *
    */
    p_audio_packet = audioBuffering_get_packet(p_file_seq->p_audio_buffering_packet_list);

    if (p_audio_packet && p_audio_packet->data) {
        ds->pts = p_audio_packet->pts;
        *start = p_audio_packet->data;
        *extra_buf = p_audio_packet->header;
        *extra_size = p_audio_packet->header_len;
        p_file_seq->is_audio_buffering_packet = TRUE;
        p_file_seq->p_buffering_audio_packet = p_audio_packet;
        //FILE_SEQ_LOG("[%s] get audio packet from buffering audio list !!!!!!!\n",__func__);
        return p_audio_packet->size;
    }

#endif
    p_file_seq->is_audio_buffering_packet = FALSE;

    /*
    *
    *
    *  Secondly,if path total is only one,
    *               we fetch audio packet from audio buffer of ffmpeg's demuxer
    *
    */

    if (p_file_seq->total_path == 1) {
        return ds_get_packet_audio(ds, start, extra_buf, (uint8_t **)extra_size);
    }

    if (ds->eof) {
        *start = NULL;
        return 0;
    }

    while (p_file_seq->p_audio_buf == NULL) {
        return 0;
    }

    /*
    *
    *
    *  At last,if path total is more than one,
    *               we fetch audio packet from internal buffer of audio preloader.
    *
    */
    p_node = p_file_seq->p_audio_buf;

    while (p_node->next_pkt == NULL) {
        if (p_node->eof) {
            ds->eof = 1;
            OS_PRINTF("get pkt, audio is eof\n");
            break;
        } else { //wait data come
            return 0;
        }
    }

    ds->pts = p_node->pts;
    *start = p_node->data;
    //OS_PRINTF("get a %d\n", (u32)(ds->pts*1000));
    *extra_size = p_node->extra_data_len;

    //OS_PRINTF("\n%s %d %d\n",__func__,__LINE__,p_node->extra_data_len);
    if (p_node->extra_data_len > 0) {
        *extra_buf = malloc(p_node->extra_data_len);
        memcpy(*extra_buf, p_node->extra_data, *extra_size);
    }

    // OS_PRINTF("\n%s %d %d\n",__func__,__LINE__,p_node->extra_data_len);
    p_audio_buffer->read_pos = (u8 *)p_node - p_audio_buffer->buffer_start;
    p_file_seq->p_audio_buf = p_file_seq->p_audio_buf->next_pkt;
    return p_node->data_len;
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
static int ds_get_buffer_pkt_video(demux_stream_t * ds, unsigned char ** start, int8_t speed)
{
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    MT_ASSERT(p_file_seq != NULL);
    es_pkt_data_t * p_node =  NULL;
    preload_buffer_t * p_video_buffer = &p_file_seq->preload_video_buffer;
    *start = NULL;

    if (p_file_seq->total_path == 1) {
        return ds_get_packet_video(ds, start, speed);
    }

    if (ds->eof) {
        *start = NULL;
        return 0;
    }

    while (p_file_seq->p_video_buf == NULL) {
        return 0;
    }

    p_node = p_file_seq->p_video_buf;

    while (p_node->next_pkt == NULL) {
        if (p_node->eof) {
            ds->eof = 1;
            OS_PRINTF("get pkt, video is eof\n");
            break;
        } else { //wait data come
            return 0;
        }
    }

    ds->pts = p_node->pts;
    *start = p_node->data;
    // OS_PRINTF("get v %d\n", (u32)(ds->pts*1000));
    p_video_buffer->read_pos = (u32)((u8 *)p_node - p_video_buffer->buffer_start);
    p_file_seq->p_video_buf = p_file_seq->p_video_buf->next_pkt;
    return p_node->data_len;
}


/************************************************************
*
*
*        add one audio packet to audiobuffering list
*
*
*
*********************************************************************/
void  audioBuffering_add_packet(AUDIO_PACKET_T * list,
                                u32 size, u8 * data,
                                u8 * header, u32 headerLen, double pts)
{
    AUDIO_PACKET_T * p_packet_list = list;
    AUDIO_PACKET_T * p_tail = NULL;

    //FILE_SEQ_LOG("[%s] ==start start \n",__func__);

    if (list == NULL) {
        FILE_SEQ_ERROR("[%s] ==do nothing [%d] \n", __func__, __LINE__);
        return;
    }

    /*find the tail node*/
    while (p_packet_list) {
        p_tail = p_packet_list;
        p_packet_list = p_packet_list->p_next;
    }

    /*add new node to tail of list*/
    if (p_tail && data) {
        //FILE_SEQ_LOG("[%s] size[%ld] headerLen[%ld] \n",__func__,size,headerLen);
        AUDIO_PACKET_T * p_node = mtos_malloc(sizeof(AUDIO_PACKET_T));

        if (p_node && size > 0) {
            memset(p_node, 0, sizeof(AUDIO_PACKET_T));
            p_node->data = (char *)mtos_malloc(size + 1);

            /*audio packet size is alwarys little than 128*1024*/
            if (p_node->data && size < 128 * 1024) {
                memset(p_node->data, 0, size + 1);
                memcpy(p_node->data, data, size);
                p_node->size = size;

                if (header && headerLen) {
                    p_node->header = (char *)mtos_malloc(headerLen + 1);

                    if (p_node->header) {
                        memset(p_node->header, 0, headerLen + 1);
                        memcpy(p_node->header, header, headerLen);
                        p_node->header_len = headerLen;
                    }
                }

                p_node->pts = pts;
                p_node->p_next = NULL;
                p_tail->p_next = p_node;
            }
        }
    }
}

/*******************************************************************
*
*
*        get first packet from audio packet list
*
*
*
****************************************************************************/

AUDIO_PACKET_T  * audioBuffering_get_packet(AUDIO_PACKET_T * list)
{
    AUDIO_PACKET_T * p_packet_list = list;
    AUDIO_PACKET_T * p_tail = NULL;
    AUDIO_PACKET_T * p_head = NULL;
    AUDIO_PACKET_T * p_cur_node = NULL;

    if (list == NULL) {
        FILE_SEQ_ERROR("[%s] ==do nothing [%d] \n", __func__, __LINE__);
        return NULL;
    }

    p_head = list->p_next;

    if (p_head) {
        //FILE_SEQ_LOG("[%s] ==111 \n",__func__);
        list->p_next = p_head->p_next;
        return p_head;
    }

    return NULL;
}



/*************************************************************************
*
*
*        destroy all the audio packet node ,but don't destroy the first node for the first node
*               does't contain any valid audio data
*
*
*
**********************************************************************/
void  audioBuffering_clear_list(AUDIO_PACKET_T * p_List)
{
    FILE_SEQ_LOG("[%s] start start ... \n", __func__);
    AUDIO_PACKET_T * p_packet_list = p_List;
    AUDIO_PACKET_T * p_tail = NULL;
    AUDIO_PACKET_T * p_head = NULL;
    AUDIO_PACKET_T * p_cur_node = NULL;
    AUDIO_PACKET_T * p_tmp = NULL;

    if (p_List == NULL) {
        FILE_SEQ_ERROR("[%s] ==do nothing [%d] \n", __func__, __LINE__);
        return;
    }

    p_head = p_List->p_next;
    p_cur_node = p_head;

    while (p_cur_node) {
        if (p_cur_node->data) {
            mtos_free(p_cur_node->data);
            p_cur_node->data = NULL;
        }

        if (p_cur_node->header) {
            mtos_free(p_cur_node->header);
            p_cur_node->header = NULL;
        }

        p_tmp = p_cur_node;
        p_cur_node = p_cur_node->p_next;
        mtos_free(p_tmp);
    }

    p_List->p_next = NULL;
    //mtos_free(p_List);
    FILE_SEQ_LOG("[%s] end end .... \n", __func__);
}
/*******************************************************
*
*
*        destroy one specified audio packet
*
*
*
*
*
*
*
*******/
void  audioBuffering_destroy_packet(AUDIO_PACKET_T * p_packet)
{
    if (p_packet) {
        if (p_packet->data) {
            mtos_free(p_packet->data);
            p_packet->data = NULL;
        }

        if (p_packet->header) {
            mtos_free(p_packet->header);
            p_packet->header = NULL;
        }

        mtos_free(p_packet);
    }
}

/*****************************************************************
*
*
*        create audio buffering  packet list
*
*              NOTICE: this first valid node is the secode node !!!!!!!!!!!!!!!!!
*
**************************************************************************/
AUDIO_PACKET_T  * audioBuffering_create_list()
{
    FILE_SEQ_LOG("[%s] ==start start \n", __func__);
    AUDIO_PACKET_T * p_list = (AUDIO_PACKET_T *)mtos_malloc(sizeof(AUDIO_PACKET_T));
    memset(p_list, 0, sizeof(AUDIO_PACKET_T));
    FILE_SEQ_LOG("[%s] ==end end \n", __func__);
    return p_list;
}

/****************************************************************
*
*
*        destroy the entire audio buffering list ,including the first one
*
*
*
********************************************************************/
void  audioBuffering_destroy_list(AUDIO_PACKET_T * p_List)
{
    FILE_SEQ_LOG("[%s] start start ... \n", __func__);
    AUDIO_PACKET_T * p_packet_list = p_List;
    AUDIO_PACKET_T * p_tail = NULL;
    AUDIO_PACKET_T * p_head = NULL;
    AUDIO_PACKET_T * p_cur_node = NULL;
    AUDIO_PACKET_T * p_tmp = NULL;

    if (p_List == NULL) {
        return;
    }

    p_head = p_List->p_next;
    p_cur_node = p_head;

    while (p_cur_node) {
        if (p_cur_node->data) {
            mtos_free(p_cur_node->data);
            p_cur_node->data = NULL;
        }

        if (p_cur_node->header) {
            mtos_free(p_cur_node->header);
            p_cur_node->header = NULL;
        }

        p_tmp = p_cur_node;
        p_cur_node = p_cur_node->p_next;
        mtos_free(p_tmp);
    }

    mtos_free(p_List);
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
 *    fetch and push es
 */
extern  ufs_file_t  * p_dump_video_fp;
void handle_video_es(FILE_SEQ_T * p_file_seq)
{
    demux_stream_t * ds_a = NULL;
    demux_stream_t * ds_v = NULL;
    demux_stream_t * ds_sub = NULL;
    ds_a = p_file_seq->p_cur_ds_audio;
    ds_v = p_file_seq->p_cur_ds_video;
    ds_sub = p_file_seq->p_cur_ds_sub;
    int cur_video_packet_size = 0;
    int cur_v_ticks = 0;
    int delta_ticks = 0;
    RET_CODE ret = SUCCESS;

    //OS_PRINTF("[%s]----start\n",__func__);
    /*
     *    get video packet by  ffmpeg demuxer
     */
    if (p_file_seq->needNewVideoData || (p_file_seq->m_play_state == FILE_SEQ_EXIT)) {
        if (!ds_v->eof) {
            cur_video_packet_size = ds_get_buffer_pkt_video(ds_v, &(p_file_seq->p_v_pkt_start), p_file_seq->cur_speed);
            //cur_v_ticks = mtos_ticks_get();
            //FILE_SEQ_LOG("[%s] ----------%d: ticks[%dms] v \n", __func__,while_cnt,cur_v_ticks*10);
            /*delta_ticks = (cur_v_ticks - v_ticks)*10;//ms
              if(delta_ticks > 50 || cur_video_packet_size > 4096)
              FILE_SEQ_LOG("[%s] ----------%d: vpts [%d], size[%d], ves_left[%d], delta_t[%dms] V  \n", __func__,while_cnt,(int)(ds_v->pts*1000),cur_video_packet_size,ves_left, delta_ticks);
              v_ticks = cur_v_ticks;*/
            //FILE_SEQ_LOG("[%s] ---------- call ds_get_packet_video(), packet_size=%d\n", __func__,cur_video_packet_size);
            /*if(flag_debug==1)
              {
              flag_debug=0;
              FILE_SEQ_LOG("[%s] ---------- call ds_get_packet_video(), packet_size=%d\n", __func__,cur_video_packet_size);
              FILE_SEQ_LOG("[%s] ---------- vpts = %d, while_cnt = %d\n", __func__,(int)(ds_v->pts*1000),while_cnt);
              }*/
            //if (p_file_seq->exsubtitle == 1) {
            //  update_subtitles_mp(ds_v, ds_sub);
            //}

            if (cur_video_packet_size <= 0
                || p_file_seq->p_v_pkt_start == NULL
                || cur_video_packet_size >= MAX_VPACKET_SIZE) {
                //FILE_SEQ_ERROR("[%s][ERROR] cur_video_packet_size [%d]!!\n", __func__, cur_video_packet_size);
                return;
            }

#if  0//def  DUMP_VIDEO_ES
#ifdef __LINUX__

            if (p_dump_video_fp && p_file_seq->p_v_pkt_start && cur_video_packet_size > 0) {
                fwrite(p_file_seq->p_v_pkt_start, cur_video_packet_size, 1, p_dump_video_fp);
                FILE_SEQ_ERROR("[%s]------write data to file!, size[%d]!!\n", __func__, cur_video_packet_size);
            }

#else
            int r = 0;

            if (p_dump_video_fp && p_file_seq->p_v_pkt_start && cur_video_packet_size > 0) {
                if (ufs_write(p_dump_video_fp, p_file_seq->p_v_pkt_start, cur_video_packet_size, &r) != FR_OK) {
                    return -1;
                }
            }

#endif
#endif
        } else {
            p_file_seq->isVideoEsEnd = TRUE;
            p_file_seq->isTrickPlay = FALSE;
            FILE_SEQ_LOG("[%s] >>>Video Stream Demuxer Is Finished !!<<<\n", __func__);
            return;
        }

        if ((first_vpts) < -1) {
            first_vpts = ds_v->pts;
        }

        if (ds_a->pts < 0.0) {
            ds_a->pts = 0.0;
        }

        if (ds_v->pts < 0.0) {
            ds_v->pts = 0.0;
        }

        if (ds_sub->pts < 0.0) {
            ds_sub->pts = 0.0;
        }

        //p_file_seq->orig_apts =  ds_a->pts * 1000.0;
        //p_file_seq->orig_vpts =  ds_v->pts * 1000.0;
        //p_file_seq->orig_spts = ds_sub->pts * 1000.0;
        p_file_seq->orig_apts =  ds_a->pts * 1000.0;

        if (p_file_seq->orig_apts * PTS_BASE >= 0x80000000 - 1) {
            p_file_seq->orig_apts = ((u32)p_file_seq->orig_apts) & 0x04ffffff;
        }

        p_file_seq->orig_vpts =  ds_v->pts * 1000.0;

        if (p_file_seq->orig_vpts * PTS_BASE >= 0x80000000 - 1) {
            p_file_seq->orig_vpts = ((u32)p_file_seq->orig_vpts) & 0x04ffffff;
        }

        p_file_seq->orig_spts = ds_sub->pts * 1000.0;

        if (p_file_seq->orig_spts * PTS_BASE >= 0x80000000 - 1) {
            p_file_seq->orig_spts = ((u32)p_file_seq->orig_spts) & 0x04ffffff;
        }

        if (p_file_seq->is_ts) {
            p_file_seq->sys_apts = ds_a->pts31bit | VALID_PTS_MASK;
            p_file_seq->sys_vpts = ds_v->pts31bit | VALID_PTS_MASK;
        } else {
            p_file_seq->sys_apts = (u32)((p_file_seq->orig_apts * PTS_BASE)) | VALID_PTS_MASK;
            p_file_seq->sys_vpts = (u32)((p_file_seq->orig_vpts * PTS_BASE)) | VALID_PTS_MASK;
        }

        if (p_file_seq->m_video_codec_type == 9) { //9 vp8
            //for vp8, add pts to
            if (p_file_seq->p_v_pkt_start[0] == 0x44
                && p_file_seq->p_v_pkt_start[1] == 0x4B
                && p_file_seq->p_v_pkt_start[2] == 0x49) {
                FILE_SEQ_LOG("[%s] vp8 ivf head[%.4s] \n", __func__, p_file_seq->p_v_pkt_start);
                *(p_file_seq->p_v_pkt_start + 36) = (u8)(p_file_seq->sys_vpts);
                *(p_file_seq->p_v_pkt_start + 37) = (u8)(p_file_seq->sys_vpts >> 8);
                *(p_file_seq->p_v_pkt_start + 38) = (u8)(p_file_seq->sys_vpts >> 16);
                *(p_file_seq->p_v_pkt_start + 39) = (u8)(p_file_seq->sys_vpts >> 24);
                *((uint32_t *)(p_file_seq->p_v_pkt_start + 40)) = 0;
            } else {
                *(p_file_seq->p_v_pkt_start + 4) = (u8)(p_file_seq->sys_vpts);
                *(p_file_seq->p_v_pkt_start + 5) = (u8)(p_file_seq->sys_vpts >> 8);
                *(p_file_seq->p_v_pkt_start + 6) = (u8)(p_file_seq->sys_vpts >> 16);
                *(p_file_seq->p_v_pkt_start + 7) = (u8)(p_file_seq->sys_vpts >> 24);
                *((uint32_t *)(p_file_seq->p_v_pkt_start + 8)) = 0;
            }
        }

        if (p_file_seq->orig_vpts > p_file_seq->max_video_pts) {
            p_file_seq->max_video_pts = p_file_seq->orig_vpts;

            /*
             *  from trcik play mode to normal play mode
             */
            if (p_file_seq->isTrickToNormal) {
                p_file_seq->ref_first_video_pts = p_file_seq->max_video_pts;
                p_file_seq->totalVesNum = 0;
            }
        }

        /*
         *  if  cur_video_packet_size is less than 1016 , zero padding should be filled at seg_buf
         *  the fields of vpts/dts ocuppy 8 bytes
         */
        if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type != CHIPTYPE_CONCERTO && 
        	(cur_video_packet_size < VES_SEG_LEN - 8) && (!io_isnetworkstream) &&
            (DEMUXER_TYPE_MPEG_PS != ((demuxer_t *)(p_file_seq->p_demuxer))->type)) {
            memset(p_file_seq->ves_seg_buf, 0, VES_SEG_LEN);
            memcpy(p_file_seq->ves_seg_buf, p_file_seq->p_v_pkt_start, cur_video_packet_size);
            p_file_seq->p_v_pkt_start = p_file_seq->ves_seg_buf;
            p_file_seq->left_v_pkt_bytes = (VES_SEG_LEN - 8);
        } else {
            p_file_seq->left_v_pkt_bytes = cur_video_packet_size;
        }

        p_file_seq->m_tmp_ves_buf_pos = 0;
        p_file_seq->needNewVideoData = FALSE;
    }

    /*
     *
     *  push ves to dma ring buffer
     *
     */
    unsigned int videoFreeSpaceSize = 0;
    unsigned int push_ves_size = 0;

    if (p_file_seq->m_video_codec_type == 0xffff) {
        p_file_seq->left_v_pkt_bytes = 0;
        p_file_seq->isVideoBufferFull = FALSE;
        p_file_seq->needNewVideoData = TRUE;
    } else {
        do {
            /*
             *   length  of video es to be pushed should not be
             */
            if (p_file_seq->left_v_pkt_bytes > 0 && p_file_seq->left_v_pkt_bytes <= VIDEO_TMP_BUF_LEN) {
                push_ves_size = p_file_seq->left_v_pkt_bytes;
            } else {
                push_ves_size = VIDEO_TMP_BUF_LEN;
            }

#ifdef ES_END_IN_TASK

            if (p_file_seq->init_buffering_finish != 1) {
                vdec_get_vitual_es_buf_space(p_file_seq, (u32 *)&videoFreeSpaceSize);
            } else
#endif
                vdec_get_es_buf_space(p_file_seq->p_vdec_dev, (u32 *)&videoFreeSpaceSize);

            // OS_PRINTF("\n%s %  finsh:%dd\n",__func__,__LINE__,p_file_seq->init_buffering_finish);
            if (videoFreeSpaceSize > (VIDEO_ES_BUF_OVERFLOW_THRESHOLD + push_ves_size / VES_SEG_LEN + 1)) {
                //memcpy(p_file_seq->p_tmp_ves_buf, p_file_seq->p_v_pkt_start + (p_file_seq->m_tmp_ves_buf_pos), push_ves_size);
                //OS_PRINTF("\n%s %  finsh:%dd\n",__func__,__LINE__,p_file_seq->init_buffering_finish);
#ifdef ES_END_IN_TASK
                if (p_file_seq->init_buffering_finish != 1) {
                    ret = vdec_dec_push_vitual_es(p_file_seq, (u32)p_file_seq->p_tmp_ves_buf, push_ves_size, p_file_seq->sys_vpts);
                } else
#endif
                    ret = vdec_dec_push_es(p_file_seq->p_vdec_dev, (u32)(p_file_seq->p_v_pkt_start + (p_file_seq->m_tmp_ves_buf_pos)), push_ves_size, p_file_seq->sys_vpts);

                //  OS_PRINTF("\n%s %  finsh:%dd\n",__func__,__LINE__,p_file_seq->init_buffering_finish);
#ifdef DUMP_VIDEO_ES
                u8 * p1 = p_file_seq->p_v_pkt_start;
                unsigned int len1 = cur_video_packet_size;
#ifdef __LINUX__

                if (p_dump_video_fp && p1 && len1 > 0) {
                    fwrite(p1, len1, 1, p_dump_video_fp);
                    FILE_SEQ_ERROR("[%s]------write data to file!, size[%d]!!\n", __func__, len1);
                }

#else
                int tet = 0;

                if (p_dump_video_fp && p1 && len1 > 0) {
                    if (ufs_write(p_dump_video_fp, p1, len1, &tet) != FR_OK) {
                        return -1;
                    }
                }

#endif
#endif
        if((ret != SUCCESS) && (p_file_seq->m_play_state != FILE_SEQ_PAUSE))
        {

             if(((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO)
             {

                    if(p_file_seq->m_audio_codec_type != 0xffff)
                        aud_stop_vsb(p_file_seq->p_audio_dev);
                    vdec_freeze_stop(p_file_seq->p_vdec_dev);
                    vdec_file_clearesbuffer(p_file_seq->p_vdec_dev);
                    FILE_SEQ_ERROR("[%s]-ret[%d],state[%d],speed[%d],push ves error,reset decoder\n", 
                        __func__,ret,p_file_seq->m_play_state,p_file_seq->cur_speed);
                    if ((p_file_seq->cur_speed == TS_SEQ_NORMAL_PLAY || 
                        p_file_seq->cur_speed == TS_SEQ_FAST_PLAY_2X) &&
                        p_file_seq->m_audio_codec_type != 0xffff) {
                        u32 size = 0;
                        u32 align = 0;
                        
                        aud_get_buf_requirement_vsb(p_file_seq->p_audio_dev, &size, &align);
                        aud_set_buf_vsb(p_file_seq->p_audio_dev,
                                        (g_pFwMemCfg.p_audio_start) | 0xa0000000,
                                        size);
                        set_audio_param_to_vsb();
                    }
                    if(p_file_seq->is_ts)
                       vdec_set_avsync_mode(p_file_seq->p_vdec_dev, VDEC_AVSYNC_FILEPLAYTS); 
                    else
                       vdec_set_avsync_mode(p_file_seq->p_vdec_dev, VDEC_AVSYNC_FILEPLAY);
                    vdec_set_file_playback_framerate(p_file_seq->p_vdec_dev, p_file_seq->video_fps);
                    vdec_start(p_file_seq->p_vdec_dev,p_file_seq->m_video_codec_type,2);
                    vdec_resume(p_file_seq->p_vdec_dev);
                    if(p_file_seq->cur_speed != TS_SEQ_NORMAL_PLAY)
                    {
                        FILE_SEQ_LOG("[%s]-reset trick mode last[%d],speed[%d]\n", 
                            __func__,p_file_seq->last_speed,p_file_seq->cur_speed);
                        if(p_file_seq->cur_speed >= TS_SEQ_FAST_PLAY_2X && 
                            p_file_seq->cur_speed <= TS_SEQ_FAST_PLAY_32X)
                            vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_FFWD, 2);
                        else if(p_file_seq->cur_speed >= TS_SEQ_REV_FAST_PLAY_2X && 
                            p_file_seq->cur_speed <= TS_SEQ_REV_FAST_PLAY_32X)
                            vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_FREV, 2);
                    }
                        
                    p_file_seq->last_speed = p_file_seq->cur_speed;
                    p_file_seq->needNewAudioData = TRUE;
                    p_file_seq->needNewVideoData = TRUE;
                    p_file_seq->isAudioBufferFull = FALSE;
                    p_file_seq->isVideoBufferFull = FALSE;
                    p_file_seq->totalVesNum = 0;
                    p_file_seq->left_v_pkt_bytes = 0;
                    p_file_seq->m_tmp_ves_buf_pos = 0;
                    p_file_seq->vpts_upload = 1;
                    p_file_seq->apts_upload = 0;
                    
                    break;
             }
        }

                p_file_seq->totalVesNum += push_ves_size;
                p_file_seq->left_v_pkt_bytes -= push_ves_size;
                p_file_seq->m_tmp_ves_buf_pos += push_ves_size;
                //if(flag_debug==1)
                //  FILE_SEQ_LOG("[%s] ---------- call vdec_dec_push_es(), push_ves_size=%d\n", __func__,push_ves_size);

                if (p_file_seq->left_v_pkt_bytes == 0) {
                    p_file_seq->isVideoBufferFull = FALSE;
                    p_file_seq->needNewVideoData = TRUE;
                    videoFreeSpaceSize = (videoFreeSpaceSize * VES_SEG_LEN) - push_ves_size;
                    p_file_seq->available_ves_bytes = p_file_seq->dec_cap.max_ves_num - videoFreeSpaceSize;
                    p_file_seq->m_tmp_ves_buf_pos = 0;
#if  0

                    if (p_file_seq->is_debug) {
#define MREAD(A) (*((volatile unsigned int *)(A)))
                        FILE_SEQ_DEBUG("send ves ok\n");
                        FILE_SEQ_DEBUG("758[0x%x]\n", MREAD(0xbfef0758));
                        FILE_SEQ_DEBUG("720[0x%x]\n", MREAD(0xbfef0720));
                        FILE_SEQ_DEBUG("c4[0x%x]\n", MREAD(0xbfd300c4));
                        FILE_SEQ_DEBUG("b4[0x%x]\n", MREAD(0xbfd300b4));
                    }

#endif
                    break;
                }
            } else {
                p_file_seq->isVideoBufferFull = TRUE;
      if(p_file_seq->total_path == 1 && io_isnetworkstream && !ds_v->eof &&
        p_file_seq->orig_vpts <= p_file_seq->orig_apts && (ds_a->bytes + ds_v->bytes <= 1.5*1024*1024))
     {
        fifo_fill_buff(p_file_seq->p_demuxer,ds_v);
        //FILE_SEQ_DEBUG("[%s]line[%d],databufferd[%d]\n",__func__,__LINE__,(ds_a->bytes + ds_v->bytes)/1024);
     }
	 		if(io_isnetworkstream == 0)
				mtos_task_sleep(200);
              //FILE_SEQ_DEBUG("Ves Buf Full [%d]\n",videoFreeSpaceSize);
                break;
            }
        } while (p_file_seq->left_v_pkt_bytes > 0);
    }
}
/*
 *
 * pre_sync av es
 **
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
void handle_audio_es_buffering(FILE_SEQ_T * p_file_seq)
{
    demux_stream_t * ds_a = NULL;
    demux_stream_t * ds_v = NULL;
    demux_stream_t * ds_sub = NULL;
    ds_a = p_file_seq->p_cur_ds_audio;
    ds_v = p_file_seq->p_cur_ds_video;
    ds_sub = p_file_seq->p_cur_ds_sub;
    int pack_num = 0;
    int cur_audio_packet_size = 0;

    /*
     *  get audio paket from  ffmpeg demuxer
     */
    do {
        if (!ds_a->eof) {
            p_file_seq->extra_audio_size = 0;
            p_file_seq->p_extra_aud_buf = NULL;
            cur_audio_packet_size = ds_get_buffer_pkt_audio(ds_a, &(p_file_seq->p_a_pkt_start), &(p_file_seq->p_extra_aud_buf), &(p_file_seq->extra_audio_size));

            if (cur_audio_packet_size <= 0 || p_file_seq->p_a_pkt_start == NULL) {
                return;
            }

            p_file_seq->left_a_pkt_bytes = cur_audio_packet_size;
        } else {
            FILE_SEQ_LOG("[%s][ok]  >>>> Audio Stream Demuxer Is Finished !!<<<<\n", __func__);
            p_file_seq->isAudioEsEnd = TRUE;
            return;
        }

        if (ds_a->pts < 0.0) {
            ds_a->pts = 0.0;
        }

        p_file_seq->orig_apts =  ds_a->pts * 1000.0;

        if (p_file_seq->orig_apts > p_file_seq->max_audio_pts) {
            p_file_seq->max_audio_pts = p_file_seq->orig_apts;
        }

        if (p_file_seq->orig_apts < p_file_seq->first_vpts && p_file_seq->orig_apts >= 0) {
            if (p_file_seq->first_vpts - p_file_seq->orig_apts < 150 ||
                p_file_seq->first_vpts - p_file_seq->orig_apts > 10000) {
                p_file_seq->needNewAudioData = FALSE;
                p_file_seq->sys_apts = ds_a->pts31bit | VALID_PTS_MASK;
                FILE_SEQ_DEBUG("[%s]throw1[%d]packet,apts[%lld]ms,vpts[%lld]ms\n", __func__,
                               pack_num, (int64_t)p_file_seq->orig_apts, (int64_t)p_file_seq->orig_vpts);
                return;
            }
        }

        if (p_file_seq->p_extra_aud_buf) {
            free(p_file_seq->p_extra_aud_buf);
            p_file_seq->p_extra_aud_buf = NULL;
        }
    } while (pack_num++ < 50 && p_file_seq->orig_apts < p_file_seq->first_vpts);

    p_file_seq->sys_apts = ds_a->pts31bit | VALID_PTS_MASK;

    if (p_file_seq->orig_apts >= p_file_seq->first_vpts) {
        p_file_seq->needNewAudioData = FALSE;
    }

    if (p_file_seq->needNewAudioData && p_file_seq->p_extra_aud_buf) {
        free(p_file_seq->p_extra_aud_buf);
        p_file_seq->p_extra_aud_buf = NULL;
    }

    FILE_SEQ_DEBUG("[%s]throw2[%d]packet,apts[%lld]ms,vpts[%lld]ms\n", __func__,
                   pack_num, (int64_t)p_file_seq->orig_apts, (int64_t)p_file_seq->orig_vpts);
    return;
}
/*
 *
 * handle audio es
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

void handle_audio_es(FILE_SEQ_T * p_file_seq)
{
    demux_stream_t * ds_a = NULL;
    demux_stream_t * ds_v = NULL;
    demux_stream_t * ds_sub = NULL;
    ds_a = p_file_seq->p_cur_ds_audio;
    ds_v = p_file_seq->p_cur_ds_video;
    ds_sub = p_file_seq->p_cur_ds_sub;
    int cur_audio_packet_size = 0;
    int cur_a_ticks = 0;
    int delta_ticks = 0;

    /*
     *  get audio paket from  ffmpeg demuxer
     */

    if (p_file_seq->needNewAudioData || (p_file_seq->m_play_state == FILE_SEQ_EXIT)) {
        if (!ds_a->eof) {
            if (p_file_seq->isTrickPlay) {
                if ((!ds_a->eof)  && ((ds_a->pts - ds_v->pts) > 0.05)) {
                    return;
                    //ds_get_packet(ds_a, &(p_file_seq->p_a_pkt_start));
                }

                while ((!ds_a->eof)  && ((ds_v->pts - ds_a->pts) > 0.0)) {
                    //printf("1@\n");
                    ds_get_packet(ds_a, &(p_file_seq->p_a_pkt_start));
                }
            }

            /*why do that?
              if (p_file_seq->isTrickToNormal) {
              while ((!ds_a->eof)  && ((ds_v->pts - ds_a->pts) > 0.0)) {
              ds_get_packet(ds_a, &(p_file_seq->p_a_pkt_start));
              }
              }
             */
            if (ds_a->eof) {
                FILE_SEQ_LOG("[%s] end audio es !!! !!!\n", __func__);
                return;
            }

            p_file_seq->extra_audio_size = 0;
            p_file_seq->p_extra_aud_buf = NULL;
            cur_audio_packet_size = ds_get_buffer_pkt_audio(ds_a, &(p_file_seq->p_a_pkt_start),
                                    &(p_file_seq->p_extra_aud_buf), &(p_file_seq->extra_audio_size));

            if (cur_audio_packet_size <= 0 || p_file_seq->p_a_pkt_start == NULL) {
                //FILE_SEQ_ERROR("[%s]audio packet size [%d]!!!!\n", __func__, cur_audio_packet_size);
                return;
            }

            p_file_seq->left_a_pkt_bytes = cur_audio_packet_size;
        } else {
            FILE_SEQ_LOG("[%s][ok]  >>>> Audio Stream Demuxer Is Finished !!<<<<\n", __func__);
            p_file_seq->isAudioEsEnd = TRUE;
            return;
        }

        if (ds_a->pts < 0.0) {
            ds_a->pts = 0.0;
        }

        if (ds_v->pts < 0.0) {
            ds_v->pts = 0.0;
        }

        if (ds_sub->pts < 0.0) {
            ds_sub->pts = 0.0;
        }

        p_file_seq->orig_apts =  ds_a->pts * 1000.0;

        if (p_file_seq->orig_apts * PTS_BASE >= 0x80000000 - 1) {
            p_file_seq->orig_apts = ((u32)p_file_seq->orig_apts) & 0x04ffffff;
        }

        p_file_seq->orig_vpts =  ds_v->pts * 1000.0;

        if (p_file_seq->orig_vpts * PTS_BASE >= 0x80000000 - 1) {
            p_file_seq->orig_vpts = ((u32)p_file_seq->orig_vpts) & 0x04ffffff;
        }

        p_file_seq->orig_spts = ds_sub->pts * 1000.0;

        if (p_file_seq->orig_spts * PTS_BASE >= 0x80000000 - 1) {
            p_file_seq->orig_spts = ((u32)p_file_seq->orig_spts) & 0x04ffffff;
        }

        if (p_file_seq->is_ts) {
            p_file_seq->sys_apts = ds_a->pts31bit | VALID_PTS_MASK;
            p_file_seq->sys_vpts = ds_v->pts31bit | VALID_PTS_MASK;
        } else {
            p_file_seq->sys_apts = (u32)((p_file_seq->orig_apts * PTS_BASE)) | VALID_PTS_MASK;
            p_file_seq->sys_vpts = (u32)((p_file_seq->orig_vpts * PTS_BASE)) | VALID_PTS_MASK;
        }

        //FILE_SEQ_LOG("[%s]orig[%d],[%x]\n", __func__,(u32)p_file_seq->orig_apts,p_file_seq->sys_apts);

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

            if (p_file_seq->p_extra_aud_buf && p_file_seq->extra_audio_size) {
                free(p_file_seq->p_extra_aud_buf);
                p_file_seq->p_extra_aud_buf = NULL;
                p_file_seq->totalAesNum += (cur_audio_packet_size + p_file_seq->extra_audio_size);
            }

            p_file_seq->isAudioBufferFull = FALSE;
            return;
        }
    }

    /*
     *      push aes to dma ring buffer
     */
    unsigned int audFreeSpaceSize = 0;

    if (p_file_seq->isNormalPlay) {
        if (p_file_seq->m_audio_codec_type == 0xffff) {
            p_file_seq->left_a_pkt_bytes = 0;
            p_file_seq->isAudioBufferFull = FALSE;
            p_file_seq->needNewAudioData = TRUE;
        } else {
#ifdef ES_END_IN_TASK

            if (p_file_seq->init_buffering_finish != 1) {
                aud_file_getleftes_vitual_buffer(p_file_seq, (u32 *)&audFreeSpaceSize);
            } else
#endif
                aud_file_getleftesbuffer_vsb(p_file_seq->p_audio_dev, (u32 *)&audFreeSpaceSize);

            if (audFreeSpaceSize >= (p_file_seq->left_a_pkt_bytes + p_file_seq->extra_audio_size)
                + AUDIO_ES_BUF_OVERFLOW_THRESHOLD) {
                if (p_file_seq->p_extra_aud_buf && p_file_seq->extra_audio_size) {
                    memcpy(p_file_seq->p_aud_assemble_buf,
                           p_file_seq->p_extra_aud_buf, p_file_seq->extra_audio_size);

                    /*packet from audio buffering is not necessary to destroy*/
                    if (p_file_seq->is_audio_buffering_packet == FALSE) {
                        free(p_file_seq->p_extra_aud_buf);
                        p_file_seq->p_extra_aud_buf = NULL;
                    }
                }

                if ((p_file_seq->extra_audio_size + p_file_seq->left_a_pkt_bytes) > AUD_ASSEMBLE_BUF_LEN) {
                    p_file_seq->left_a_pkt_bytes = 0;
                    p_file_seq->isAudioBufferFull = FALSE;
                    p_file_seq->needNewAudioData = TRUE;
                    FILE_SEQ_ERROR("[%s][ERROR] audio packet data will overflow \n", __func__);
                    return;
                }

                if ((void *)(p_file_seq->p_a_pkt_start) == NULL) {
                    return -1;
                }

                memcpy(p_file_seq->p_aud_assemble_buf + p_file_seq->extra_audio_size,
                       (void *)(p_file_seq->p_a_pkt_start), p_file_seq->left_a_pkt_bytes);
                adec_push_audio_es(p_file_seq->p_aud_assemble_buf,
                                   p_file_seq->left_a_pkt_bytes + p_file_seq->extra_audio_size, p_file_seq->sys_apts);
#ifdef DUMP_AUDIO_ES_WITH_HEADER
                u8 * p1 = p_file_seq->p_aud_assemble_buf;
                unsigned int len1 = p_file_seq->left_a_pkt_bytes + p_file_seq->extra_audio_size;
#ifdef __LINUX__

                if (p_dump_audio_fp && p1 && len1 > 0) {
                    fwrite(p1, len1, 1, p_dump_audio_fp);
                    FILE_SEQ_ERROR("[%s]------write data to file!, size[%d]!!\n", __func__, len1);
                }

#else
                int r = 0;

                if (p_dump_audio_fp && p1 && len1 > 0) {
                    if (ufs_write(p_dump_audio_fp, p1, len1, &r) != FR_OK) {
                        return -1;
                    }

                    FILE_SEQ_LOG("*******write audio es 00,len = %d\n", len1);
                }

#endif
#else
#ifdef DUMP_AUDIO_ES
                u8 * p1 = p_file_seq->p_aud_assemble_buf + p_file_seq->extra_audio_size;
                unsigned int len1 = p_file_seq->left_a_pkt_bytes;
#ifdef __LINUX__

                if (p_dump_audio_fp && p1 && len1 > 0) {
                    fwrite(p1, len1, 1, p_dump_audio_fp);
                    FILE_SEQ_ERROR("[%s]------write data to file!, size[%d]!!\n", __func__, len1);
                }

#else
                int r = 0;

                if (p_dump_audio_fp && p1 && len1 > 0) {
                    if (ufs_write(p_dump_audio_fp, p1, len1, &r) != FR_OK) {
                        return -1;
                    }

                    FILE_SEQ_LOG("*******write audio es 00,len = %d\n", len1);
                }

#endif
#endif
#endif
#if 0//del audio buffering

                /*if this audio packet come from audio buffering ,we should free relative memory*/
                /* peacer add 20140220*/
                if (p_file_seq->is_audio_buffering_packet) {
                    if (p_file_seq->p_buffering_audio_packet) {
                        audioBuffering_destroy_packet(p_file_seq->p_buffering_audio_packet);
                        p_file_seq->p_buffering_audio_packet = NULL;
                    }

                    p_file_seq->is_audio_buffering_packet = FALSE;
                }

#endif
                p_file_seq->totalAesNum += p_file_seq->left_a_pkt_bytes;
                p_file_seq->available_aes_bytes = (p_file_seq->dec_cap.max_aes_num - audFreeSpaceSize)
                                                  + (p_file_seq->left_a_pkt_bytes + p_file_seq->extra_audio_size);
                p_file_seq->left_a_pkt_bytes = 0;
                p_file_seq->isAudioBufferFull = FALSE;
                p_file_seq->needNewAudioData = TRUE;
            } else {
        static int g_exit_errhandle = 0;
           if(p_file_seq->isVideoEsEnd)
           {
                if(((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO)
                {
                    g_exit_errhandle ++;
                    FILE_SEQ_LOG("[%s] video is end and repeat %d times and exit\n",__func__,g_exit_errhandle);
                    if(g_exit_errhandle >= 5)
                    {
                         g_exit_errhandle = 0;
                         FILE_SEQ_LOG("[%s][ok]  >>>> Audio Stream Demuxer Is Finished !!<<<<\n", __func__);
                         p_file_seq->isAudioEsEnd = TRUE;
                         return;
                    }
                }
          }

                p_file_seq->isAudioBufferFull = TRUE;
                p_file_seq->needNewAudioData = FALSE;
                
                if(p_file_seq->total_path == 1 && io_isnetworkstream && !ds_a->eof &&
                     p_file_seq->orig_apts <= p_file_seq->orig_vpts && (ds_a->bytes + ds_v->bytes <= 1.5*1024*1024))
                {
                     fifo_fill_buff(p_file_seq->p_demuxer,ds_a);
                     //FILE_SEQ_DEBUG("[%s]line[%d],databufferd[%d]\n",__func__,__LINE__,(ds_a->bytes + ds_v->bytes)/1024);
                }
                if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state == FILE_SEQ_BUFFERING) {
                    p_file_seq->isAudioBufferFull = FALSE;
                    p_file_seq->needNewAudioData = FALSE;
#if 0//del audio buffering

                    /*
                    *
                    *  whether this packet come from internal demuxer's buffer or audio buffering list
                    *      add it to auido buffering packet list
                    */
                    if (p_file_seq->p_a_pkt_start) {
                        if (p_file_seq->buffering_audio_data_size > MAX_BUFFERING_AUDIO_SIZE) {
                            audioBuffering_clear_list(p_file_seq->p_audio_buffering_packet_list);
                            p_file_seq->buffering_audio_data_size = 0;
                        }

                        audioBuffering_add_packet(p_file_seq->p_audio_buffering_packet_list,
                                                  cur_audio_packet_size,
                                                  p_file_seq->p_a_pkt_start,
                                                  p_file_seq->p_extra_aud_buf,
                                                  p_file_seq->extra_audio_size,
                                                  ds_a->pts);
                        p_file_seq->buffering_audio_data_size += cur_audio_packet_size;
                        p_file_seq->buffering_audio_data_size += p_file_seq->extra_audio_size;
                        p_file_seq->buffering_audio_data_size += sizeof(AUDIO_PACKET_T);

                        if (p_file_seq->p_extra_aud_buf) {
                            free(p_file_seq->p_extra_aud_buf);
                            p_file_seq->p_extra_aud_buf = NULL;
                        }

                        /*
                        *
                        *  if this packet come from audio buffer
                        *      we should destroy this node
                        *
                        */
                        if (p_file_seq->is_audio_buffering_packet) {
                            if (p_file_seq->p_buffering_audio_packet) {
                                audioBuffering_destroy_packet(p_file_seq->p_buffering_audio_packet);
                                p_file_seq->p_buffering_audio_packet = NULL;
                            }

                            p_file_seq->is_audio_buffering_packet = FALSE;
                        }
                    }

#endif
                }

                //OS_PRINTF("[%s]----------aud free size [f:%ld] paket size[%d] apts[%d] while_cnt=%d\n",__func__,audFreeSpaceSize,cur_audio_packet_size,(int)(ds_a->pts*1000),while_cnt);
            }
        }
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
 *
 *
 *
 */
void handle_subtitle_data(FILE_SEQ_T * p_file_seq)
{
    demux_stream_t * ds_a = NULL;
    demux_stream_t * ds_v = NULL;
    demux_stream_t * ds_sub = NULL;
    ds_a = p_file_seq->p_cur_ds_audio;
    ds_v = p_file_seq->p_cur_ds_video;
    ds_sub = p_file_seq->p_cur_ds_sub;
    int cur_sub_packet_size = 0;
    //  int index;
    //char temp[1024];
    char sub_text_head[200] = {0};
    demux_subtitle * p_subtitle = ((demuxer_t *)(p_file_seq->p_demuxer))->subt_info.subtitle;
    int i = 0;
    BOOL add_sub_header = FALSE;

    if (!ds_v->eof) {
        ds_sub->id = p_file_seq->subt_id;
        cur_sub_packet_size = ds_get_packet_subt(ds_sub, &(p_file_seq->p_sub_pkt_start), &(p_file_seq->orig_spts));
        //   FILE_SEQ_DEBUG("ds_sub->id [%d] \n", ds_sub->id);

        if (cur_sub_packet_size > 0) {
            // FILE_SEQ_DEBUG("sub header [%s]  [%d]  [%s]  [%d] \n", __func__,__LINE__,(((demuxer_t *)(p_file_seq->p_demuxer))->subt_info.subtitle[ds_sub_id].code),ds_sub->id);
            //FILE_SEQ_DEBUG("sub header [%s]  [%d]  [%d] \n", __func__,__LINE__,strcmp((((demuxer_t *)(p_file_seq->p_demuxer))->subt_info.subtitle[ds_sub_id].code), "text") );
            //FILE_SEQ_DEBUG("sub her [%s]  [%d]  [%d] \n", __func__,__LINE__,strcmp("text","text"));
            //   FILE_SEQ_DEBUG("sub id [%d] \n", ds_sub->id);
            for (i = 0; i < DEMUX_SUBTITLE_LEN; i++) {
                if (ds_sub->id == p_subtitle[i].id) {
                    //  FILE_SEQ_DEBUG("sub id i:[%d] code[%s] \n", i, p_subtitle[i].code);
                    if (0 == strcmp(p_subtitle[i].code, "text")) {
                        add_sub_header = TRUE;
                        int sh, sm, ss;
                        int sc = (int)(10 * p_file_seq->orig_spts);
                        int eh, em, es;
                        int ec = (int)((10 * p_file_seq->orig_spts) + 15);
                        sh = sc / 36000;
                        sc -= 36000 * sh;
                        sm = sc /  600;
                        sc -=   600 * sm;
                        ss = sc /   10;
                        sc -=    10 * ss;
                        sc *= 100;
                        eh = ec / 36000;
                        ec -= 36000 * eh;
                        em = ec /  600;
                        ec -=   600 * em;
                        es = ec /   10;
                        ec -=    10 * es;
                        ec *= 100;
                        sprintf(sub_text_head, "%02d:%02d:%02d,%03d --> %02d:%02d:%02d,%03d\r\n",
                                sh, sm, ss, sc, eh, em, es, ec);
                        cur_sub_packet_size += strlen(sub_text_head);
                        // FILE_SEQ_DEBUG("sub header111 [%s] \n", sub_text_head);
                    }

                    break;
                }
            }

#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
            SUBT_DATA * subt_data = (SUBT_DATA *) file_seq_mem_alloc(p_file_seq, sizeof(SUBT_DATA));
            subt_data->data = file_seq_mem_alloc(p_file_seq, (cur_sub_packet_size + 8));
#else
            SUBT_DATA * subt_data = (SUBT_DATA *) mtos_malloc(sizeof(SUBT_DATA));
            subt_data->data = mtos_malloc(cur_sub_packet_size + 8);
#endif
            subt_data->pts = p_file_seq->orig_spts;
            subt_data->size = cur_sub_packet_size;
            subt_data->data[0] = ((subt_data->size) & 0xff000000) >> 24;
            subt_data->data[1] = ((subt_data->size) & 0xff0000) >> 16;
            subt_data->data[2] = ((subt_data->size) & 0xff00) >> 8;
            subt_data->data[3] = ((subt_data->size)) & 0xff;
            subt_data->data[4] = ((subt_data->pts) & 0xff000000) >> 24;
            subt_data->data[5] = ((subt_data->pts) & 0xff0000) >> 16;
            subt_data->data[6] = ((subt_data->pts) & 0xff00) >> 8;
            subt_data->data[7] = ((subt_data->pts)) & 0xff;

            if (add_sub_header) {
                strcpy(subt_data->data + 8, sub_text_head);
                memcpy(subt_data->data + 8 + strlen(sub_text_head), p_file_seq->p_sub_pkt_start, cur_sub_packet_size - strlen(sub_text_head));
                // mtos_printk_f("\n\n  pts %f  \n\n",p_file_seq->orig_spts);
            } else {
                memcpy(subt_data->data + 8, p_file_seq->p_sub_pkt_start, cur_sub_packet_size);
            }

#if 0
            memcpy(temp, subt_data->data + 8, cur_sub_packet_size);
            temp[cur_sub_packet_size] = "\0";
            //  FILE_SEQ_LOG("%c\n",temp);
            // FILE_SEQ_DEBUG("sub header 1111[%s] \n", __func__);

            for (index = 0; index < cur_sub_packet_size; index++) {
                FILE_SEQ_LOG("%c", subt_data->data[8 + index]);
            }

            FILE_SEQ_LOG("\n");
            FILE_SEQ_DEBUG("sub heade2222r [%s] \n", __func__);
#endif
            mtos_sem_take((os_sem_t *)(&(p_file_seq->sub_fifo_mutex)), 0);
            write_sub_fifo_kw(p_file_seq->p_sub_fifo_handle, subt_data->data, cur_sub_packet_size + 8);
            mtos_sem_give((os_sem_t *)(&(p_file_seq->sub_fifo_mutex)));

            if (NULL != p_file_seq->event_cb) {
                p_file_seq->event_cb(FILE_PLAYBACK_NEW_SUB_DATA_RECEIVE, 0);
            }

#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
            file_seq_mem_free(p_file_seq, subt_data->data);
            file_seq_mem_free(p_file_seq, subt_data);
#else
            mtos_free(subt_data->data);
            mtos_free(subt_data);
#endif
        }
    }

    if (ds_a->pts < 0.0) {
        ds_a->pts = 0.0;
    }

    if (ds_v->pts < 0.0) {
        ds_v->pts = 0.0;
    }

    if (ds_sub->pts < 0.0) {
        ds_sub->pts = 0.0;
    }

    p_file_seq->orig_apts =  ds_a->pts * 1000;
    //p_file_seq->sys_apts = (p_file_seq->orig_apts * PTS_BASE) | VALID_PTS_MASK;
    p_file_seq->orig_vpts =  ds_v->pts * 1000;
    //p_file_seq->sys_vpts =    (p_file_seq->orig_vpts * PTS_BASE) | VALID_PTS_MASK;
    p_file_seq->orig_spts = ds_sub->pts * 1000;
    //p_file_seq->sys_spts =  (p_file_seq->orig_spts * PTS_BASE) | VALID_PTS_MASK;
}
/*******************************************************************************************
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
 *************************************************************************************************/

static  void check_ves_water_level()
{
    FILE_SEQ_T * p_file_seq = x_get_cur_instance();
    BUFFERING_PARAM_T  *  p_buffering_param = p_file_seq->p_buffering;
    u32 videoFreeSpaceSize = 0;
    u32 cur_ves_wl = 0;
if(((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->task_idle == 0)
{
    if (loop_count_ves++ > 32) {
        loop_count_ves = 0;
        mtos_task_sleep(20); //for lower prio task
    }
}
    if (!io_isnetworkstream) {
        return;
    }

    if (p_file_seq->cur_speed != TS_SEQ_NORMAL_PLAY &&
        ((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state != FILE_SEQ_BUFFERING) {
        return;
    }

    if (p_file_seq->isVideoEsEnd  ||  p_file_seq->isAudioEsEnd) {
        if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state == FILE_SEQ_BUFFERING) {
#ifdef ES_END_IN_TASK

            if (p_file_seq->init_buffering_finish == 1)
#endif
                if (p_file_seq->m_play_state == FILE_SEQ_PLAY) {
                    vdec_resume(p_file_seq->p_vdec_dev);
                    aud_resume_vsb(p_file_seq->p_audio_dev);
                }

            ((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state = FILE_SEQ_BUFFERING_END;
            p_file_seq->event_cb(FILE_PLAYBACK_SEQ_FINISH_BUFFERING, 0);
        }

        return;
    }

    if ((p_file_seq->vpts_upload < 1
         || p_file_seq->video_disp_h <= 0
         || p_file_seq->video_disp_w <= 0
         || p_file_seq->is_stable == FALSE)
        && ((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state != FILE_SEQ_BUFFERING
        && seek_buffer != 1) {
        //  mtos_printk("\n%s %d vpt:%d h:%d w %d ta:%d\n",__func__,__LINE__,p_file_seq->vpts_upload,p_file_seq->video_disp_h,
        // p_file_seq->video_disp_w, p_file_seq->is_stable );
        return;
    } else {
#ifdef ES_END_IN_TASK

        if (p_file_seq->init_buffering_finish != 1) {
            vdec_get_vitual_es_buf_space(p_file_seq, (u32 *)&videoFreeSpaceSize);
        } else
#endif
            vdec_get_es_buf_space(p_file_seq->p_vdec_dev, &videoFreeSpaceSize);

        cur_ves_wl = p_file_seq->max_ves_size - videoFreeSpaceSize;

        if (p_file_seq->init_buffering_finish == 1 && cur_ves_wl < 10) {
            if (p_file_seq->video_disp_h < 720) {
                p_file_seq->video_bps = DEFAULT_SD_BPS * 1024;
                p_buffering_param->start_buffering_wl = 100;
                p_buffering_param->end_buffering_wl =
                    p_buffering_param->start_buffering_wl + DEFAULT_SD_BPS * DEFAULT_BUFFERING_SECOND;
            } else {
                /*case  1080P*/
                if (p_file_seq->video_disp_w >= 1920 && p_file_seq->video_disp_h >= 720) {
                    p_file_seq->video_bps = DEF_SUPER_HD_BPS * 1024;
                    p_buffering_param->start_buffering_wl = 300;
                    p_buffering_param->end_buffering_wl =
                        p_buffering_param->start_buffering_wl + DEF_SUPER_HD_BPS;
                } else { /*case  720P*/
                    p_file_seq->video_bps = DEFAULT_HD_BPS * 1024;
                    p_buffering_param->start_buffering_wl = 100;
                    p_buffering_param->end_buffering_wl =
                        p_buffering_param->start_buffering_wl + DEFAULT_HD_BPS * DEFAULT_BUFFERING_SECOND;
                }
            }
        }
    }

    //mtos_printk("\n%s %d wl:%d wl_st:%d\n",__func__,__LINE__,cur_ves_wl,p_buffering_param->start_buffering_wl);
    if (cur_ves_wl >= p_buffering_param->end_buffering_wl || seek_buffer) {
        seek_buffer = 0;

        if (p_file_seq->init_buffering_finish == 0) {
            while (p_file_seq->check_task_state == CHECK_ES_RUNNING) {
                mtos_task_sleep(300);
            }

#ifdef ES_END_IN_TASK
            av_decoder_init(p_file_seq);
            move_data_from_vitual_es(p_file_seq);
#endif
        }

        p_file_seq->init_buffering_finish = 1;

        if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state == FILE_SEQ_BUFFERING) {
            if (p_file_seq->m_play_state == FILE_SEQ_PLAY) {
                vdec_resume(p_file_seq->p_vdec_dev);
                aud_resume_vsb(p_file_seq->p_audio_dev);
            }

            ((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state = FILE_SEQ_BUFFERING_END;
            p_file_seq->event_cb(FILE_PLAYBACK_SEQ_FINISH_BUFFERING, 0);
            FILE_SEQ_LOG("[%s] @@@@current ves count [%ld  KBytes]!!!!\n",
                         __func__, cur_ves_wl);
            FILE_SEQ_LOG("[%s] >>>>>>>>>@@@ END BUFFERING <<<<<<<<<\n", __func__);
        }

        return;
    } else if ((cur_ves_wl <=  p_buffering_param->start_buffering_wl)) {
        if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state == FILE_SEQ_BUFFERING_END) {
            p_file_seq->m_play_state_backup = p_file_seq->m_play_state;
            ((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state = FILE_SEQ_BUFFERING;
#ifdef ES_END_IN_TASK

            if (p_file_seq->init_buffering_finish == 1)
#endif
            {
                vdec_pause(p_file_seq->p_vdec_dev);
                aud_pause_vsb(p_file_seq->p_audio_dev);
            }

            p_buffering_param->cur_tick = mtos_ticks_get();
            p_buffering_param->old_tick = p_buffering_param->cur_tick;
            p_buffering_param->start_tick = p_buffering_param->cur_tick;
#ifdef ES_END_IN_TASK

            if (p_file_seq->init_buffering_finish != 1) {
                vdec_get_vitual_es_buf_space(p_file_seq, (u32 *)&videoFreeSpaceSize);
            } else
#endif
                vdec_get_es_buf_space(p_file_seq->p_vdec_dev, &videoFreeSpaceSize);

            p_buffering_param->cur_ves_num = p_file_seq->max_ves_size - videoFreeSpaceSize;
            p_buffering_param->old_ves_num = p_buffering_param->cur_ves_num;
            p_buffering_param->start_show_buffering_in_playstate = FALSE;
            p_buffering_param->start_show_loadmedia_time = FALSE;
            FILE_SEQ_LOG("[%s] >>>>>>>>>START   BUFFERING <<<<<<<<<<<\n", __func__);
            FILE_SEQ_LOG("[%s] @@@@current ves count [%ld  KBytes]!!!!\n", __func__, p_buffering_param->cur_ves_num);
            p_file_seq->event_cb(FILE_PLAYBACK_SEQ_START_BUFFERING, 0);
        }

        return;
    }

#if   defined(SDK_2_0)
    else {
        update_BPS();
    }

#endif
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
//#define SWITCH_DEBUG 1
#ifdef SWITCH_DEBUG
int exit_ticks = 0;
                 int switch_mode_flag = 0;
#endif
                                        int demux_state = 0;
                                                //#define CLIENT_SORT_TEST
                                                extern  int encry_delay;
                                                void  fill_es_task(void * p_param)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_param;
    int freespace = 0;
    int threhold = 0;
    int es_task_init = 0;
    int preload_cnt = 0;
    trick_pts = 0.0;
    int ticks_in, tick_out;
    int cur_ticks;
    int ves_buf_size = 0;
    loop_count_ves = 0;
    switch_mode_flag = 0;
    MT_ASSERT(p_file_seq != NULL);
    p_file_seq->init_buffering_finish = 0;
    start_init = 0;
    first_vpts = -2;
    //
    int buffer_tick = 0;
    BOOL drop_data = FALSE;
    int next_sleep_tick = 0;
    int start_tick ;
    //
    FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);
#if defined(DUMP_AUDIO_ES) || defined(DUMP_AUDIO_ES_WITH_HEADER)
    u8  dump_audio_file[32] = "dump.audio.es";
#ifdef __LINUX__
    p_dump_audio_fp = fopen(dump_audio_file, "wb");

    if (!p_dump_audio_fp) {
        FILE_SEQ_DEBUG("[%s] ---------open audio dump file error\n", __func__);
    }

#else
    p_dump_audio_fp = mtos_malloc(sizeof(ufs_file_t));
    memset(p_dump_audio_fp, 0, sizeof(ufs_file_t));
    unsigned short  path_tmp[256] = {0};
    u16 * p_name1 = Convert_Utf8_To_Unicode(dump_audio_file, path_tmp);

    if (ufs_open(p_dump_audio_fp, p_name1, UFS_WRITE | UFS_CREATE_NEW_COVER) != FR_OK) {
        FILE_SEQ_DEBUG("[%s] ERROR fail open dump.audio.es...\n", __func__);
    } else {
        FILE_SEQ_DEBUG("[%s] open :%s ok...\n", __func__, dump_audio_file);
    }

#endif
#endif
#ifdef  DUMP_VIDEO_ES
    u8  dump_video_file[32] = "dump.video.es";
#ifdef __LINUX__
    p_dump_video_fp = fopen(dump_video_file, "wb");

    if (!p_dump_video_fp) {
        FILE_SEQ_DEBUG("[%s] ---------open video dump file error\n", __func__);
    }

#else
    unsigned short  path_tmp2[256] = {0};
    u16 * p_name2 = Convert_Utf8_To_Unicode(dump_video_file, path_tmp2);
    p_dump_video_fp = mtos_malloc(sizeof(ufs_file_t));
    memset(p_dump_video_fp, 0, sizeof(ufs_file_t));

    if (ufs_open(p_dump_video_fp, p_name2, UFS_WRITE | UFS_CREATE_NEW_COVER) != FR_OK) {
        FILE_SEQ_DEBUG("[%s] ERROR fail open dump.video.es...\n", __func__);
    } else {
        FILE_SEQ_DEBUG("[%s] open :%s ok...\n", __func__, dump_video_file);
    }

#endif
#endif
    //
    //
    p_file_seq->needNewVideoData = TRUE;
    p_file_seq->needNewAudioData = TRUE;
    p_file_seq->isTrickToNormal = FALSE;
    p_file_seq->isNormalPlay = TRUE;
    p_file_seq->isTrickPlay = FALSE;
    p_file_seq->ref_first_audio_pts = 0;
    p_file_seq->ref_first_video_pts = 0;
    p_file_seq->isAudMute = FALSE;
    p_file_seq->first_vpts = 0;
    p_file_seq->orig_vpts = 0;
    p_file_seq->orig_apts = 0;
    p_file_seq->orig_spts = 0;
    p_file_seq->m_tmp_ves_buf_pos = 0;
    p_file_seq->is_play_to_end = 0;
    demux_state = 1;
    vdec_get_es_buf_size(p_file_seq->p_vdec_dev, (u32 *)&ves_buf_size);
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
    p_file_seq->p_aud_assemble_buf = file_seq_mem_alloc(p_file_seq, AUD_ASSEMBLE_BUF_LEN);
    p_file_seq->ves_seg_buf = file_seq_mem_alloc(p_file_seq, VES_SEG_LEN);
#else
    p_file_seq->p_aud_assemble_buf = mtos_malloc(AUD_ASSEMBLE_BUF_LEN);//fix me
    p_file_seq->ves_seg_buf = mtos_malloc(VES_SEG_LEN);
#endif

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

    p_file_seq->vpts_upload = 0;
#ifdef FILE_PLAYBACK_DEBUG
    while_cnt = 0;
    flag_debug = 0;
#endif

    if (p_file_seq->total_path <= 1) {
        p_file_seq->is_task_alive = TRUE;
    }
//mtos_printk("\n%s %d\n",__func__,__LINE__);
if(fp_is_timeshift_file() == 0)
    file_seq_check_trickplay(p_file_seq);
	//mtos_printk("\n%s %d\n",__func__,__LINE__);
#ifndef ES_END_IN_TASK
    av_decoder_init(p_file_seq);
#endif

    //extern void test_udp_recv();
    //test_udp_recv();
    if (p_file_seq->start_seconds > 0) {
        //get the first packet of audio & video

        //yliu add
        if (p_file_seq->total_path > 1) {
#if 0

            while (m_load_task_lock <= 0) {
                mtos_task_sleep(50);
            }

            m_load_task_lock--;
            //   mtos_printk("\n @@@@@%s %d\n",__func__,__LINE__);
#if 0

            if (p_file_seq->start_seconds >= p_file_seq->file_duration) {
                p_file_seq->m_seek_cnt = p_file_seq->total_path - 1 ;
                p_file_seq->m_seek_adj = 0;
            } else {
                p_file_seq->m_seek_cnt = p_file_seq->start_seconds / p_file_seq->max_duration;
                p_file_seq->m_seek_adj = p_file_seq->start_seconds - p_file_seq->m_seek_cnt * p_file_seq->max_duration;
            }

#endif
            //if(p_file_seq->m_seek_cnt == 0)
            {
                float start_seconds = p_file_seq->start_seconds;
                multipath_seek_time_cal(&start_seconds, p_file_seq);
                p_file_seq->start_seconds = start_seconds;
                OS_PRINTF("\n%s %d cnt %d adj %dseek:%d\n", __func__, __LINE__, p_file_seq->m_seek_cnt, (int)p_file_seq->m_seek_adj, (int)p_file_seq->start_seconds);
                //if(p_file_seq->m_seek_adj>0)
                //p_file_seq->m_seek_adj = 0;
            }
            m_load_task_lock++;
#endif
        } else {
            demux_stream_t * ds_a = p_file_seq->p_cur_ds_audio;
            demux_stream_t * ds_v = p_file_seq->p_cur_ds_video;

            if (p_file_seq->is_ts) {
                ds_get_packet(ds_v, &(p_file_seq->p_v_pkt_start));
                ds_get_packet(ds_a, &(p_file_seq->p_a_pkt_start));

                if (ds_a->pts < 0.0) {
                    ds_a->pts = 0.0;
                }

                if (ds_v->pts < 0.0) {
                    ds_v->pts = 0.0;
                }

                if (es_task_init == 0) {
                    p_file_seq->first_vpts = p_file_seq->orig_vpts = ds_v->pts * 1000;
                    p_file_seq->orig_apts = ds_a->pts * 1000;
                    es_task_init = 1;

                    if (p_file_seq->first_vpts > 1) {
                    } else if (p_file_seq->orig_apts > 1) {
                        p_file_seq->first_vpts = p_file_seq->orig_apts;
                    }
                }
            } else {
                if (p_file_seq->only_audio_mode == FALSE && g_hls_playmode==1) {
                    vdec_set_avsync_mode(p_file_seq->p_vdec_dev, VDEC_AVSYNC_FILEPLAYTS);
                }

                handle_video_es(p_file_seq);
                handle_audio_es(p_file_seq);

                if (es_task_init == 0) {
                    p_file_seq->first_vpts = p_file_seq->orig_vpts;
                    es_task_init = 1;

                    if (p_file_seq->first_vpts > 1) {
                    } else if (p_file_seq->orig_apts > 1) {
                        p_file_seq->first_vpts = p_file_seq->orig_apts;
                    }
                }
            }

            //start to seek
            demux_seek(p_file_seq->p_demuxer, p_file_seq->start_seconds, 0, 0) ;
        }
    }

    //p_file_seq->start_seconds = 0;
    if (p_file_seq->total_path > 1) {
        p_file_seq->is_task_alive = TRUE;
    }

    if (p_file_seq->is_ts) {
        int pre_sync_audio = 0;
		int sleep_cnt;
        vdec_pause(p_file_seq->p_vdec_dev);

        if (p_file_seq->m_audio_codec_type != 0xffff) {
            aud_pause_vsb(p_file_seq->p_audio_dev);
        }

        while (preload_cnt++ < 50) {
            vdec_get_es_buf_space(p_file_seq->p_vdec_dev, (u32 *)&freespace);

            if (freespace < 1500) {
                break;
            }
		if(fp_is_timeshift_file() == 1)
			{
			sleep_cnt++;
			if(sleep_cnt>5)
				{
            mtos_task_sleep(10);
			sleep_cnt = 0;
				}
			}
            handle_video_es(p_file_seq);

            if (es_task_init == 0) {
                p_file_seq->first_vpts = p_file_seq->orig_vpts;

                if (p_file_seq->first_vpts > 1) {
                    es_task_init = 1;
                }
            }

            if (es_task_init && !pre_sync_audio && p_file_seq->m_audio_codec_type != 0xffff) {
                handle_audio_es_buffering(p_file_seq);
                pre_sync_audio++;
            }

            if (pre_sync_audio) {
#ifdef ES_END_IN_TASK

                if (p_file_seq->init_buffering_finish != 1) {
                    aud_file_getleftes_vitual_buffer(p_file_seq, (u32 *)&freespace);
                } else
#endif
                    aud_file_getleftesbuffer_vsb(p_file_seq->p_audio_dev, (u32 *)&freespace);

                if (freespace < 60 * 1024) {
                    break;
                }

                handle_audio_es(p_file_seq);
            }
        }

        vdec_resume(p_file_seq->p_vdec_dev);

        if (p_file_seq->m_audio_codec_type != 0xffff) {
            aud_resume_vsb(p_file_seq->p_audio_dev);
        }
    }

    FILE_SEQ_DEBUG("[%s] @@@ START MAIN LOOP @@@ !![%d],es[%d]\n", __func__, preload_cnt, freespace);
#ifdef FILE_PLAYBACK_DEBUG
    ticks_in = mtos_ticks_get();
    FILE_SEQ_DEBUG("\n[%s] --------@@@@@@-------- ticks_in = %d\n", __func__, ticks_in);
#endif
    u32  ves_buf_max_size = ves_buf_size;
    BUFFERING_PARAM_T  *  p_buffering_param = p_file_seq->p_buffering;

    if (io_isnetworkstream) {
        p_buffering_param->start_buffering_wl = 20;
        p_buffering_param->end_buffering_wl =   p_buffering_param->start_buffering_wl + 180;
    }

    FILE_SEQ_DEBUG("ves_buf_max_size:%ld\n", ves_buf_max_size);
    FILE_SEQ_DEBUG("start_buffering_wl:%ld\n", p_buffering_param->start_buffering_wl);
    FILE_SEQ_DEBUG("end_buffering_wl:%ld\n", p_buffering_param->end_buffering_wl);

    if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state == FILE_SEQ_BUFFERING_END && io_isnetworkstream) {
        int videoFreeSpaceSize;
        p_file_seq->m_play_state_backup = p_file_seq->m_play_state;
        ((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state = FILE_SEQ_BUFFERING;
#ifndef ES_END_IN_TASK
        vdec_pause(p_file_seq->p_vdec_dev);

        if (p_file_seq->m_audio_codec_type != 0xffff) {
            aud_pause_vsb(p_file_seq->p_audio_dev);
        }

#endif
        p_buffering_param->cur_tick = mtos_ticks_get();
        p_buffering_param->old_tick = p_buffering_param->cur_tick;
        p_buffering_param->start_tick = p_buffering_param->cur_tick;
#ifdef ES_END_IN_TASK

        if (p_file_seq->init_buffering_finish != 1) {
            vdec_get_vitual_es_buf_space(p_file_seq, (u32 *)&videoFreeSpaceSize);
        } else
#endif
            vdec_get_es_buf_space(p_file_seq->p_vdec_dev, (u32 *)&videoFreeSpaceSize);

        p_buffering_param->cur_ves_num = p_file_seq->max_ves_size - videoFreeSpaceSize;
        p_buffering_param->old_ves_num = p_buffering_param->cur_ves_num;
        p_buffering_param->start_show_buffering_in_playstate = FALSE;
        p_buffering_param->start_show_loadmedia_time = FALSE;
        FILE_SEQ_LOG("[%s] >>>>>>>>>START   BUFFERING <<<<<<<<<<<\n", __func__);
        FILE_SEQ_LOG("[%s] @@@@current ves count [%ld  KBytes]!!!!\n", __func__, p_buffering_param->cur_ves_num);
        p_file_seq->event_cb(FILE_PLAYBACK_SEQ_START_BUFFERING, 0);
    }

#ifdef SWITCH_DEBUG
    exit_ticks = 0;// mtos_ticks_get();
#endif
    start_tick = mtos_ticks_get();

    while (1) {
        //cur_ticks = mtos_ticks_get();
        //FILE_SEQ_LOG("[%s] ----------%d: ticks[%dms] 1 \n", __func__,while_cnt,cur_ticks*10);
        /*
         *   excute user's command
         */
#ifdef WITH_OTT_DATA
        if (encry_delay) {
            int play_ticks = mtos_ticks_get() - start_tick;
            int tick_round = 5 * 60 * 100; //5 min

            if (drop_data == FALSE) {
                if (buffer_tick == 0) {
                    next_sleep_tick = max(rand() % 4000, 1000);
                } else {
                    next_sleep_tick = (rand() % tick_round) + buffer_tick * tick_round;
                }

                drop_data = TRUE;
            }

            if (play_ticks > next_sleep_tick && (drop_data == TRUE) && (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state != FILE_SEQ_BUFFERING)) {
                int drop_cnt = 30;
                mtos_printk("\n%s %d ticks:%d %d buffer_tick:%d\n",
                            __func__, __LINE__, play_ticks, next_sleep_tick, buffer_tick);

                while ((drop_cnt--) > 0) {
                    demux_stream_t    *   ds_v = p_file_seq->p_cur_ds_video;
                    ds_get_buffer_pkt_video(ds_v, &(p_file_seq->p_v_pkt_start), p_file_seq->cur_speed);
                }

                drop_data = FALSE;
                buffer_tick++;
            }
        }

#endif
        do_user_cmd();

        /*
        *
        *
        */
        check_ves_water_level();
        //#ifndef SWITCH_DEBUG
#ifdef CLIENT_SORT_TEST
        // #if 0
        {
            int play_ticks = mtos_ticks_get() - start_tick;

            //mtos_printk("\n %s %d  tick:%d  %d\n",__func__,__LINE__,play_ticks,p_file_seq->m_play_state);
            if (play_ticks > 4000) {
                p_file_seq->isVideoEsEnd  = 1;
                p_file_seq->isAudioEsEnd = 1;
            }
        }
#endif

        if (p_file_seq->m_play_state == FILE_SEQ_EXIT && (p_file_seq->ott_playmode != OTT_LIVE_MODE || p_file_seq->force_stopped == 1)) {
            OS_PRINTF("[%s]----p_file_seq->m_play_state == FILE_SEQ_EXIT,break!", __func__);

            if (p_file_seq->total_path > 1) {
                p_file_seq->m_preload_state = FILE_SEQ_EXIT;
            }

            break;
        }

        //#endif

        if (p_file_seq->m_play_state == FILE_SEQ_PAUSE) {
            mtos_task_sleep(30);
            continue;
        }

        if (p_file_seq->is_play_to_end == 1) {
            OS_PRINTF("[%s]----p_file_seq->is_play_to_end == 1,break!", __func__);
            break;
        }

        /*
         *   identify whether file sequencer can exit ?
         */
        if (check_task_finish(p_file_seq)) {
            OS_PRINTF("[%s]----check_task_finish true,break!", __func__);
            break;
        }

        /*
         *   fetch ves and try to push them to ves buffer
         */
        freespace = 0;
        threhold = ves_buf_size - 150;
#ifdef ES_END_IN_TASK

        if (p_file_seq->init_buffering_finish != 1) {
            vdec_get_vitual_es_buf_space(p_file_seq, (u32 *)&freespace);
        } else
#endif
            vdec_get_es_buf_space(p_file_seq->p_vdec_dev, (u32 *)&freespace);

        ves_left = freespace;

        //cur_ticks = mtos_ticks_get();
        //FILE_SEQ_LOG("[%s] ----------%d: ticks[%dms] 2 \n", __func__,while_cnt,cur_ticks*10);
		
        if (((p_file_seq->orig_vpts <= p_file_seq->orig_apts)
             || (p_file_seq->orig_vpts <= p_file_seq->orig_spts)
             || (p_file_seq->isAudioEsEnd)
             || (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state == FILE_SEQ_BUFFERING)
             || (freespace >= threhold && ds_check_packet_buffer_overflow(p_file_seq->p_ds_audio) == 0))
        && (p_file_seq->isVideoEsEnd == FALSE)) {
            handle_video_es(p_file_seq);
        }

        //aes_left = freespace;
        /*
         *   fetch aes and try to push them to aes buffer
         */
        if (((p_file_seq->orig_apts < p_file_seq->orig_vpts)
             || (p_file_seq->orig_apts < p_file_seq->orig_spts)
             || p_file_seq->isVideoEsEnd)
        && p_file_seq->isAudioEsEnd == FALSE) {
                handle_audio_es(p_file_seq);
        }

        /*
         *   fetch subtile data and try to push them to middleware
         */
        if (((demuxer_t *)(p_file_seq->p_cur_demuxer))->subt_info.cnt > 0 && p_file_seq->subt_id >= 0) {
            if (!IS_SET(p_file_seq->m_user_cmd, CMD_PLAY_AT_TIME)) {
                //won't get subt when do seek
                if (((p_file_seq->orig_vpts >= p_file_seq->orig_spts)
                     || (p_file_seq->orig_apts >= p_file_seq->orig_spts))
                    && ((ds_check_packet_buffer_overflow(p_file_seq->p_ds_audio) == 0)
                && (ds_check_packet_buffer_overflow(p_file_seq->p_ds_video) == 0))) {
                    handle_subtitle_data(p_file_seq);
                }
            }
        }
		
        SetFlag(p_file_seq->internal_event, AUTO_DELIVER_VPTS);
		        /*
         *   excute user's command
         */
        handle_pending_event();
#ifdef FILE_PLAYBACK_DEBUG
        while_cnt++;
        //OS_PRINTF("[%s]---------while_cnt=%d, vpts[%d], apts[%d], spts[%d] \n",__func__,while_cnt,\
        //(int)(p_file_seq->orig_vpts),(int)(p_file_seq->orig_apts),(int)(p_file_seq->orig_spts));
#endif

        if (es_task_init == 0) {
            if (p_file_seq->start_seconds > 0) {
                p_file_seq->first_vpts = p_file_seq->orig_vpts -
                                         (double)p_file_seq->start_seconds * 1000;

                if (p_file_seq->total_path > 1) {
                    p_file_seq->first_vpts = 2;
                }
            } else {
                p_file_seq->first_vpts = p_file_seq->orig_vpts;
            }

            if (p_file_seq->first_vpts > 1) {
                es_task_init = 1;
            }

            //printf("****** first pts %f \n",first_pts);
        }

        //cur_ticks = mtos_ticks_get();
        //FILE_SEQ_LOG("[%s] ----------%d: ticks[%dms] 3 \n", __func__,while_cnt,cur_ticks*10);
    }

    FILE_SEQ_LOG("[%s]  @@@ END MAIN LOOP @@@@\n", __func__);
#ifdef FILE_PLAYBACK_DEBUG
    tick_out = mtos_ticks_get();
    compute_Bps = (int)(total_tcp_recv_size / ((tick_out - ticks_in) / 100.0));
    FILE_SEQ_DEBUG("\n[%s] --------@@@@@@-------- tick_out = %d, total_tcp_recv_size: %d \n delta ticks[%d] [%d]Bps\n", __func__, tick_out, total_tcp_recv_size, \
                   (tick_out - ticks_in), compute_Bps);
    total_tcp_recv_size = 0;
#endif
EXIT_FILL_ES_TASK:

    //#ifndef SWITCH_DEBUG
    if (!switch_mode_flag) {
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
    }

    //#endif

    /*
     *     change to normal play
     */
    if (p_file_seq->cur_speed != TS_SEQ_NORMAL_PLAY) {
        p_file_seq->isAudMute = FALSE;
        p_file_seq->isTrickPlay = FALSE;
        p_file_seq->isNormalPlay = TRUE;
        p_file_seq->isTrickToNormal = TRUE;
        p_file_seq->totalVesNum = 0;
        p_file_seq->totalAesNum = 0;
        p_file_seq->ref_first_video_pts = p_file_seq->max_video_pts;
        aud_mute_onoff_vsb(p_file_seq->p_audio_dev, FALSE);
        vdec_set_trick_mode(p_file_seq->p_vdec_dev, VDEC_TM_NORMAL, 1);
        p_file_seq->cur_speed = TS_SEQ_NORMAL_PLAY;
        p_file_seq->last_speed = TS_SEQ_NORMAL_PLAY;
        p_file_seq->tmp_speed = 0;
        //trickmode set to normal?
    }

    if (p_file_seq->isAudMute) {
        aud_mute_onoff_vsb(p_file_seq->p_audio_dev, FALSE);
        p_file_seq->isAudMute = FALSE;
    }

#ifdef  DUMP_VIDEO_ES

    if (p_dump_video_fp) {
#ifdef __LINUX__
        fclose(p_dump_video_fp);
        FILE_SEQ_DEBUG("[%s] fclose p_dump_video_fp\n", __func__);
#else
        ufs_close(p_dump_video_fp);
        mtos_free(p_dump_video_fp);
#endif
        p_dump_video_fp = NULL;
    }

#endif
#if  defined(DUMP_AUDIO_ES) || defined(DUMP_AUDIO_ES_WITH_HEADER)
#ifdef __LINUX__
    fclose(p_dump_audio_fp);
    FILE_SEQ_DEBUG("[%s] fclose p_dump_audio_fp\n", __func__);
#else

    if (p_dump_audio_fp) {
        ufs_close(p_dump_audio_fp);
        mtos_free(p_dump_audio_fp);
        p_dump_audio_fp = NULL;
    }

#endif
#endif

    if (p_file_seq->total_path > 1) {
        if (p_file_seq->m_preload_state != FILE_SEQ_EXIT) {
            p_file_seq->m_preload_state = FILE_SEQ_EXIT;
        }

        while (1) {
            if (p_file_seq->is_preload_task_alive && !switch_mode_flag) {
                mtos_task_sleep(30);
                FILE_SEQ_DEBUG("[%s] wait 30ms...\n", __func__);
            } else {
                break;
            }
        }
    }

    if (p_file_seq->pb_internal &&
    ((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state ==  FILE_SEQ_BUFFERING) {
        ((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->internal_state = FILE_SEQ_BUFFERING_END;
        p_file_seq->event_cb(FILE_PLAYBACK_SEQ_FINISH_BUFFERING, 0);
    }

    handle_exit_fill_es_task();
    FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);
}



#ifdef USE_PB_FIFO

static void playback_fifo_update_m3u8ts_status(void *p_seq, int http_status)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_seq;

    if(!p_file_seq)
    {
        return ;
    }

    if(!p_file_seq->hls_parser.enable)
    {
        return ;
    }

    if(http_status < 0)
    {
        p_file_seq->hls_parser.ts_load_status = HLS_STATUS_LOAD_FINISHED;
    }
    else
    {
        p_file_seq->hls_parser.ts_load_status = HLS_STATUS_LOAD_SUCCESS;
    }
    return ;
}
void * playback_fifo_load_m3u8ts(void *p_seq)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_seq;
    char * ts_url;
    int j = 0;
    int cur_seq_num = 0;
    int start_seq_num = 0;
    int total_seq_num = 0;
    HttpDownloadHeader header;
    void *handle = NULL;

    if(!p_file_seq)
    {
        return NULL;
    }

    if(!p_file_seq->hls_parser.enable)
    {
        OS_PRINTF("%s enable is %d \n",__func__,p_file_seq->hls_parser.enable);
    
        return NULL;
    }
    OS_PRINTF("%s ts_load_status %d \n",__func__,p_file_seq->hls_parser.ts_load_status);
    
    if(p_file_seq->hls_parser.ts_load_status == HLS_STATUS_NEED_TO_LOAD)
    {
        cur_seq_num = p_file_seq->hls_parser.cur_seq_num;
        start_seq_num = p_file_seq->hls_parser.start_seq_num;
        total_seq_num = p_file_seq->hls_parser.url_num;

        if(cur_seq_num < start_seq_num)
        {
           OS_PRINTF("%s hls load error cur_seq_num %d start_seq_num %d\n",
                      __func__,cur_seq_num,start_seq_num);           
           cur_seq_num = start_seq_num;
           p_file_seq->hls_parser.cur_seq_num = start_seq_num;
        }


        if(cur_seq_num - start_seq_num >= total_seq_num)
        {
           if(p_file_seq->hls_parser.is_live)
           {
               //this is live and need new m3u8
               //to be continue:
               //wait hls new it's list file
               OS_PRINTF("%s hls live is not support yet cur_seq_num %d total_seq %d\n",
                                     __func__,cur_seq_num,total_seq_num);
               return NULL;  //for tmp;
           }
           else
           {
               //this is vod and file is end
               OS_PRINTF("%s hls vod is end cur_seq_num %d total_seq %d\n",
                                     __func__,cur_seq_num,total_seq_num);
               return NULL;
           }
        }
        
        ts_url = p_file_seq->hls_parser.ts_url[cur_seq_num - start_seq_num].url;
	 if(strncmp("fifo:http://", ts_url, 12) == 0)
        {
		ts_url = ts_url + 5;
	 }
	 else if(strncmp("mp:http://", ts_url, 10) == 0)
        {
		ts_url = ts_url + 3;
	  }
        else if(strncmp("mp:fifo:http://", ts_url, 15) == 0)
        {
		ts_url = ts_url + 8;
	  }

        OS_PRINTF("%s new ts %s cur_seq_num %d\n",
                             __func__,ts_url,cur_seq_num);
    
    	 for(j=0; j<1; j++)
        {
            handle = Nw_Http_Download_Start(ts_url, FALSE, NULL, 0, 5, &header,NULL);
            if(handle)
                break;
            //mtos_task_sleep(500);
        }
        if(!handle) 
            return NULL;
        p_file_seq->hls_parser.cur_seq_num ++;

        return handle;
     
    }

    return NULL;
    
}
//#define DEBUG_READ_FIFO_ONE_TADK
//#define DEBUG_PLAY_FILE_USB
void playback_fifo_thread(void * p_param)
{
	FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_param;
    	MT_ASSERT(p_file_seq != NULL);
    	p_file_seq->is_pbfifo_task_alive = TRUE;
	FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);

	char *file_path;
	int r = 0;
	int read_len = HTTP_RECV_LEN;
	int rd_num = 0;
	int is_file_end=0;
	int loop_cnt=0;
	int free_space = 0;
	int filled_space = 0;
	int flag_buf = 0;
	char read_data_buffer[20*1024] = {0};
	char read_data_buffer2[2*1024] = {0};
	void *p_fifo = NULL;
	fifo_type_t *p_pb_fifo_handle;

       HttpDownloadResult ret;
       HttpDownloadHeader header;
       int total_Len = 0, i, j, res = 0;
       void *handle = NULL;
       mem_user_dbg_info_t dbg_info;

#if 0//def DEBUG_READ_FIFO_ONE_TADK//debug: read date from fifo and write to file
#ifdef __LINUX__
       FILE  * p_file_w = NULL;
      
       p_file_w = fopen("fifo_dump.dat", "wb");
   
       if (!p_file_w) {
           FILE_SEQ_DEBUG("[%s] ---------open fifo_dump.dat error\n", __func__);
       }
   
   #else
       static  ufs_file_t  * p_file_w = NULL;
       unsigned short  path_tmp[256] = {0};
       u16 * p_name22 = Convert_Utf8_To_Unicode("fifo_dump.dat",path_tmp);
       p_file_w = mtos_malloc(sizeof(ufs_file_t));
       memset(p_file_w, 0, sizeof(ufs_file_t));
   
       if (ufs_open(p_file_w, p_name22, UFS_WRITE | UFS_CREATE_NEW_COVER) != FR_OK) {
           FILE_SEQ_DEBUG("[%s] ERROR fail open fifo_dump.dat...\n", __func__);
       } else {
           FILE_SEQ_DEBUG("[%s] open :fifo_dump.dat ok...\n", __func__);
       }

#endif
#endif


	//init playback fifo

	p_fifo = mtos_malloc(PB_FIFO_LEN+1); 
        if (!p_fifo) {
            FILE_SEQ_DEBUG("[%s] malloc for fifo failed!\n", __func__);
            return;
        }
        memset(p_fifo, 0, PB_FIFO_LEN+1);

        if (p_file_seq->p_pb_fifo_handle) {
            deinit_fifo_kw(p_file_seq->p_pb_fifo_handle);
            p_file_seq->p_pb_fifo_handle = NULL;
            FILE_SEQ_DEBUG("[%s] deinit fifo ok!\n", __func__);
        }
    
        /*if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
            p_pb_fifo_handle = init_fifo_kw(0, PB_FIFO_LEN+1);
        } else */
            p_pb_fifo_handle = init_fifo_kw(p_fifo, PB_FIFO_LEN+1);
         p_file_seq->p_pb_fifo_handle = p_pb_fifo_handle;
	  FILE_SEQ_DEBUG("[%s] init fifo ok\n", __func__);
         
	
	/*open usb file or download http data, then write to fifo*/
	file_path = p_file_seq->m_path[0];
	if(strncmp("fifo:http://", p_file_seq->m_path[0], 12) == 0){
		FILE_SEQ_LOG("[%s] file path: %s\n", __func__, p_file_seq->m_path[0]);
		file_path = p_file_seq->m_path[0]+5;
	}
	FILE_SEQ_LOG("[%s] connect URL: %s\n", __func__, file_path);
	

#if 0//def    DEBUG_PLAY_FILE_USB //debug playing file in usb	
	//open usb file
#ifdef __LINUX__
       FILE  * p_file_pb = NULL;
      
       p_file_pb = fopen(file_path, "rb");
   
       if (!p_file_pb) {
           FILE_SEQ_DEBUG("[%s] ---------open %s error\n", __func__,file_path);
       }
   
   #else
       static  ufs_file_t  * p_file_pb = NULL;
       unsigned short  path_tmp2[256] = {0};
       u16 * p_name2 = Convert_Utf8_To_Unicode(file_path,path_tmp2);
       p_file_pb = mtos_malloc(sizeof(ufs_file_t));
       memset(p_file_pb, 0, sizeof(ufs_file_t));
   
       if (ufs_open(p_file_pb, p_name2, UFS_READ) != FR_OK) {
           FILE_SEQ_DEBUG("[%s] ERROR fail open %s...\n", __func__, file_path);
           	p_file_seq->is_pbfifo_task_alive = FALSE;
		FILE_SEQ_DEBUG("[%s] task exit ...\n", __func__);
#ifndef __LINUX__
   		mtos_task_exit();
#endif	
           
       } else {
           FILE_SEQ_DEBUG("[%s] open :%s ok...\n", __func__, file_path);
       }

#endif

#else
	//dowload data by http
	 for(j=0; j<10; j++)
        {
            handle = Nw_Http_Download_Start(file_path, FALSE, NULL, 0, 10, &header,NULL);
            if(handle)
                break;
            mtos_task_sleep(500);
        }

	if(!handle){
		OS_PRINTF("[%s]  Nw_Http_Download_Start Failed!\n",__func__);
		p_file_seq->is_pbfifo_task_alive = FALSE;
		FILE_SEQ_DEBUG("[%s] task exit ...\n", __func__);
		
		deinit_fifo_kw(p_file_seq->p_pb_fifo_handle);
              p_file_seq->p_pb_fifo_handle = NULL;
              if(p_fifo)
              	mtos_free(p_fifo);
              FILE_SEQ_DEBUG("[%s] deinit fifo ok!\n", __func__);
#ifndef __LINUX__
   		mtos_task_exit();
#endif	
	}
	else
		OS_PRINTF("[%s] SUCCESS Nw_Http_Download_Start\n",__func__);
		
        

#endif
	
	do{

#if 0//def    DEBUG_PLAY_FILE_USB
	//read data from file
#ifdef __LINUX__
	  r = fread(p_file_pb,read_data_buffer,read_len);
#else
	  r = 0;  
	  ufs_read(p_file_pb, read_data_buffer, read_len, &r);
#endif
	  // We are certain this is EOF, do not retry
	  if (read_len && r == 0) {
		FILE_SEQ_DEBUG("[%s] end of file!\n", __func__);
		is_file_end = 1;
		break;
	  }
	 // FILE_SEQ_DEBUG("[%s] read %d data from file!\n", __func__,r);

#else
	   ret = Nw_Http_Download_Recv(handle, read_data_buffer, read_len);
          //OS_PRINTF("[%s] ret.readLen = %d\n",__func__, ret.readLen);
          //OS_PRINTF("[%s] ret.status = %d\n",__func__, ret.status);
          r = ret.readLen;
  	   if(ret.status <0){
  	   		Nw_Http_Download_Stop(handle);
  	   		OS_PRINTF("[%s] ret.status[%d] <0, call Nw_Http_Download_Stop, break\n",__func__,ret.status );
                    break;
            }
             #if 0

     	      if(loop_cnt == 5000){
     	  	FILE_SEQ_DEBUG("[%s] loop_cnt:%d, break!\n", __func__,loop_cnt);
     	  	break;
       	}
     		#endif  	
#endif
	  //write to playback fifo
	  if(r>0){

	  	do{
			filled_space = tell_pb_fifo_kw(p_pb_fifo_handle);
			free_space = p_pb_fifo_handle->len - filled_space;
			if(free_space>r || p_file_seq->m_play_state == FILE_SEQ_STOP)
				break;

			FILE_SEQ_DEBUG("[%s] pb fifo free size: %d < write len:%d,  wait~~~!\n", __func__,free_space,r);
			FILE_SEQ_DEBUG("[%s] rd_pos:%lx, wr_pos:%lx, fifo len:%d, total_Len:%d\n", __func__,p_pb_fifo_handle->rd_pos,p_pb_fifo_handle->wr_pos,p_pb_fifo_handle->len,total_Len);

			mtos_task_sleep(100);

		}while(1);
		mtos_sem_take((os_sem_t *)(&(p_file_seq->pb_fifo_mutex)), 0);
		write_pb_fifo_kw(p_pb_fifo_handle, read_data_buffer, r);
		mtos_sem_give((os_sem_t *)(&(p_file_seq->pb_fifo_mutex)));
		//FILE_SEQ_DEBUG("[%s] write fifo %d !  r[%lx], w[%lx]  \n", __func__,r,p_pb_fifo_handle->rd_pos,p_pb_fifo_handle->wr_pos);
		total_Len += r;
#if 0//def DEBUG_READ_FIFO_ONE_TADK 
		//test read fifo in this task
		mtos_sem_take((os_sem_t *)(&(p_file_seq->pb_fifo_mutex)), 0);
		rd_num = read_pb_fifo_kw(p_pb_fifo_handle,read_data_buffer2, r);
		mtos_sem_give((os_sem_t *)(&(p_file_seq->pb_fifo_mutex)));
		FILE_SEQ_DEBUG("[%s] read %d data from fifo!\n", __func__,rd_num);

		//write data to usb
		if(rd_num>0){
#ifdef __LINUX__

                            if (p_file_w) {
                                fwrite(read_data_buffer2, rd_num, 1, p_file_w);
                                FILE_SEQ_DEBUG("[%s]------write data to file!, size[%d]!!\n", __func__, rd_num);
                            }

#else
                            int rr = 0;

                            if (p_file_w) {
                                if (ufs_write(p_file_w, read_data_buffer2, rd_num, (u32 *) &rr) != FR_OK) {
                                    FILE_SEQ_ERROR("[%s][error]------ufs_write error!!\n", __func__);
                                } else {
                                    FILE_SEQ_DEBUG("[%s]------write data to file!, size[%d]!!\n", __func__, rd_num);
                                }
                            }

#endif
		}

#endif
		
	  }


	if(p_file_seq->m_play_state == FILE_SEQ_STOP){
		Nw_Http_Download_Stop(handle);
		break;
	}

	if(!flag_buf && total_Len > T_PB_FIFO_START){
		FILE_SEQ_DEBUG("[%s] buffered %d data, sleep\n", __func__,total_Len);
		flag_buf = 1;
	   	mtos_task_sleep(100);
	}
		
	loop_cnt++;

	}while(1);

       //out of while@@@@@@@@@
#if 0//def DEBUG_READ_FIFO_ONE_TADK

    	if (p_file_w) {
#ifdef __LINUX__
	        fclose(p_file_w);
	        FILE_SEQ_DEBUG("[%s] fclose p_file_w\n", __func__);
#else
	        ufs_close(p_file_w);
	        mtos_free(p_file_w);
	        FILE_SEQ_DEBUG("[%s] fclose p_file_w\n", __func__);
#endif
	        p_file_w = NULL;
    	}
#endif

#if 0//def    DEBUG_PLAY_FILE_USB    
    	if (p_file_pb) {
#ifdef __LINUX__
	        fclose(p_file_pb);
	        FILE_SEQ_DEBUG("[%s] fclose p_file_pb\n", __func__);
#else
	        ufs_close(p_file_pb);
	        mtos_free(p_file_pb);
	        FILE_SEQ_DEBUG("[%s] fclose p_file_pb\n", __func__);
#endif
	        p_file_pb = NULL;
    	}
#endif

	while(p_file_seq->m_play_state != FILE_SEQ_STOP)
       {    
		mtos_task_sleep(200);
       }
	deinit_fifo_kw(p_file_seq->p_pb_fifo_handle);
       p_file_seq->p_pb_fifo_handle = NULL;
        if(p_fifo)
              	mtos_free(p_fifo);
       FILE_SEQ_DEBUG("[%s] deinit fifo ok!\n", __func__);
       
	p_file_seq->is_pbfifo_task_alive = FALSE;
	FILE_SEQ_DEBUG("[%s] end end ...\n", __func__);
#ifndef __LINUX__
	mtos_mem_user_debug(&dbg_info);
       mtos_printk("[%s]-start men -- alloced[%d], alloced_peak[%d], rest[%d]\n",__func__,dbg_info.alloced, dbg_info.alloced_peak,dbg_info.rest_size);

   	mtos_task_exit();
#endif	

}


void playback_fifo_thread_lite(void * p_param)
{
	FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_param;
    	MT_ASSERT(p_file_seq != NULL);
    	p_file_seq->is_pbfifo_task_alive = TRUE;
	FILE_SEQ_DEBUG("[%s] start start ...\n", __func__);

	char *file_path;
	int r = 0;
	int read_len = HTTP_RECV_LEN;
	int rd_num = 0;
	int is_file_end=0;
	int loop_cnt=0;
	int free_space = 0;
	int filled_space = 0;
	int write_len = HTTP_RECV_LEN;
	unsigned long write_end;
	void *p_fifo = NULL;
	fifo_type_t *p_pb_fifo_handle;

       HttpDownloadResult ret;
       HttpDownloadHeader header;
       unsigned long total_Len = 0, i, j, res = 0;
       int flag_buf = 0;
       void *handle = NULL;
       mem_user_dbg_info_t dbg_info;

       u32  start_tick = 0;
       u32  cur_tick = 0;
       int reconnect_recv_times = 0;
       int reconnect_recv_len = 0;
       char extra_header[128]={0};
       char *tmp="\r\n";

#if 0//def DEBUG_READ_FIFO_ONE_TADK//debug: read date from fifo and write to file
#ifdef __LINUX__
       FILE  * p_file_w = NULL;
      
       p_file_w = fopen("fifo_dump.dat", "wb");
   
       if (!p_file_w) {
           FILE_SEQ_DEBUG("[%s] ---------open fifo_dump.dat error\n", __func__);
       }
   
   #else
       static  ufs_file_t  * p_file_w = NULL;
       unsigned short  path_tmp[256] = {0};
       u16 * p_name22 = Convert_Utf8_To_Unicode("fifo_dump.dat",path_tmp);
       p_file_w = mtos_malloc(sizeof(ufs_file_t));
       memset(p_file_w, 0, sizeof(ufs_file_t));
   
       if (ufs_open(p_file_w, p_name22, UFS_WRITE | UFS_CREATE_NEW_COVER) != FR_OK) {
           FILE_SEQ_DEBUG("[%s] ERROR fail open fifo_dump.dat...\n", __func__);
       } else {
           FILE_SEQ_DEBUG("[%s] open :fifo_dump.dat ok...\n", __func__);
       }

#endif
#endif
       while (pbfifo_task_state != 1) {
	        mtos_task_sleep(100);
        }


	//init playback fifo
	p_fifo = mtos_malloc(PB_FIFO_LEN+1);
        if (!p_fifo) {
            FILE_SEQ_LOG("[%s] malloc for fifo failed!\n", __func__);
            return;
        }
        memset(p_fifo, 0, PB_FIFO_LEN+1);

        if (p_file_seq->p_pb_fifo_handle) {
            deinit_fifo_kw(p_file_seq->p_pb_fifo_handle);
            p_file_seq->p_pb_fifo_handle = NULL;
            FILE_SEQ_LOG("[%s] 1 deinit fifo ok!\n", __func__);
        }
    
        /*if (((PLAYBACK_INTERNAL_T *)(p_file_seq->pb_internal))->chip_type == CHIPTYPE_CONCERTO) {
            p_pb_fifo_handle = init_fifo_kw(0, PB_FIFO_LEN+1);
        } else */
            p_pb_fifo_handle = init_fifo_kw(p_fifo, PB_FIFO_LEN+1);
         p_file_seq->p_pb_fifo_handle = p_pb_fifo_handle;
	  FILE_SEQ_LOG("[%s] init fifo ok\n", __func__);
         
	//set play path
	file_path = p_file_seq->m_path[0];
	if(strncmp("fifo:http", p_file_seq->m_path[0], 9) == 0){
		FILE_SEQ_LOG("[%s] file path: %s\n", __func__, p_file_seq->m_path[0]);
		file_path = p_file_seq->m_path[0]+5;
	}
	
#ifndef CHIP_CONCERTO
#ifdef WITH_TCPIP_PROTOCOL

    if (strstr(p_file_seq->m_path[0], "vdn.live.cntv.cn") || strstr(p_file_seq->m_path[0], "web-play.pptv.com")
        || strstr(p_file_seq->m_path[0], "giniko.com") || strstr(p_file_seq->m_path[0], "malimar.tv") || strstr(p_file_seq->m_path[0], "filmon.com")
        || strstr(p_file_seq->m_path[0], "dailymotion.com/live/video/")
    || strstr(p_file_seq->m_path[0], "youtube.com")) {
        replace_real_play_path(file_path);
        FILE_SEQ_LOG("[%s]  replace_real_play_path done\n", __func__);
    }


#endif
#endif	
	FILE_SEQ_LOG("[%s] connect URL: %s\n", __func__, file_path);

	//start to connect by http
	p_file_seq->pbfifo_http_state = HTTP_CONNECT;
	 for(j=0; j<2; j++)
        {

            handle = Nw_Http_Download_Start(file_path, FALSE, NULL, 0, 5, &header,NULL);
            if(handle)
            {
                memset(p_file_seq->hls_parser.m3u8_ur.url,0,4096);
                if(header.redirect_url)
                {
                    memcpy(p_file_seq->hls_parser.m3u8_ur.url,header.redirect_url,strlen(header.redirect_url)); 
                    OS_PRINTF("%s redirect url %s \n",__func__,p_file_seq->hls_parser.m3u8_ur.url);
                }
                
                break;
            }
            if (p_file_seq->m_play_state == FILE_SEQ_STOP) {
   	        FILE_SEQ_LOG("[%s] m_play_state:%d, don't reconnect, exit ...\n", __func__,p_file_seq->m_play_state);
   	        break;
   	    }
            mtos_task_sleep(200);
        }

	if(!handle){
		FILE_SEQ_LOG("[%s]  Nw_Http_Download_Start Failed!\n",__func__);
		
		deinit_fifo_kw(p_file_seq->p_pb_fifo_handle);
              p_file_seq->p_pb_fifo_handle = NULL;
              if(p_fifo)
              	mtos_free(p_fifo);
              FILE_SEQ_LOG("[%s] 2 deinit fifo ok!\n", __func__);

              p_file_seq->pbfifo_http_state = HTTP_FAILED;
   	       p_file_seq->pbfifo_buffering_ok = FALSE;
   	       p_file_seq->pbfifo_http_need_reconnect = FALSE;
   	       p_file_seq->pbfifo_connect_off = 0;
   	       p_file_seq->pbfifo_stream_pos = 0;
   	       p_file_seq->pbfifo_content_len = 0;    
		p_file_seq->is_pbfifo_task_alive = FALSE;
		FILE_SEQ_LOG("[%s] task exit ...\n", __func__);
#ifndef __LINUX__
   		mtos_task_exit();
#endif	
	}
	else{
		
		FILE_SEQ_LOG("[%s] SUCCESS Nw_Http_Download_Start Content_length: %d\n",__func__,header.Content_length);
		if(header.Content_length > 0)
			p_file_seq->pbfifo_content_len = header.Content_length;
		p_file_seq->pbfifo_http_state = HTTP_RECV;
	}
		
        start_tick = mtos_ticks_get();

HTTP_RECEIVE_LOOP:
	//receive data and push it to FIFO
	do{
	
           if(p_file_seq->m_play_state == FILE_SEQ_STOP){
	   		Nw_Http_Download_Stop(handle);
	   		handle = NULL;
	   		p_file_seq->pbfifo_http_state = HTTP_STOP;
	   		break;
   		}

   	 if (p_file_seq->m_play_state == FILE_SEQ_LOADED && p_file_seq->loadMedaiOK == FALSE) {
			Nw_Http_Download_Stop(handle);
	   		handle = NULL;
	   		p_file_seq->pbfifo_http_state = HTTP_STOP;
        	        FILE_SEQ_LOG("[%s] load media error, break ...\n", __func__);
        	        break;
        	    }
     	    

	    if(!flag_buf){
   	    	cur_tick = mtos_ticks_get();
   	    	if(total_Len >= T_PB_FIFO_START){
			FILE_SEQ_LOG("[%s] buffered %d data, sleep\n", __func__,total_Len);
			flag_buf = 1;
			p_file_seq->pbfifo_buffering_ok = TRUE;
		   	mtos_task_sleep(100);
   	    	}
   	    	else{
	   		 if ((cur_tick - start_tick) * 10  > T_PB_FIFO_BUFFING_TIME) { //ms
	                     FILE_SEQ_LOG("[%s] buffered more than %d ms, data len:%d, sleep\n", __func__,T_PB_FIFO_BUFFING_TIME,total_Len);
				flag_buf = 1;
				p_file_seq->pbfifo_buffering_ok = TRUE;
			   	mtos_task_sleep(100);
                    }
                }
		}
		
		//check if need to reconnect
		if(p_file_seq->pbfifo_http_need_reconnect){

		       FILE_SEQ_DEBUG("[%s]  start to reconnect!\n",__func__);
		       
			//stop old connection if necessary
			if(handle){
				FILE_SEQ_DEBUG("[%s]  firstly, stop old connection \n",__func__);
				Nw_Http_Download_Stop(handle);
				handle = NULL;
			}

			FILE_SEQ_LOG("[%s]  reconnectting ... off:%ld\n",__func__,p_file_seq->pbfifo_connect_off);

			memset(extra_header, 0, strlen(extra_header));
			sprintf(extra_header,"Range: bytes=%lu-", p_file_seq->pbfifo_connect_off);
			//strcat(extra_header, tmp);
			FILE_SEQ_DEBUG("[%s]  extra_header: %s\n",__func__,extra_header);
			//reconnect at offset
			 for(j=0; j<2; j++)
		        {
		            if (p_file_seq->m_play_state == FILE_SEQ_STOP) {
   		   	        FILE_SEQ_LOG("[%s] not reconnect 2, exit ...\n", __func__);
   		   	        break;
   		   	    }
   		   	    
		            handle = Nw_Http_Download_Start(file_path, FALSE, NULL, 0, 5, &header, extra_header);
		            if(handle)
		                break;
		            mtos_task_sleep(100);
		        }
			if(!handle){
				FILE_SEQ_LOG("[%s]  reconnect Failed! break\n",__func__);
				p_file_seq->pbfifo_http_state = HTTP_FAILED;
				p_file_seq->pbfifo_http_need_reconnect = FALSE;
				break;
			}
			else{
				FILE_SEQ_LOG("[%s]  reconnect ok! Content_length: %d\n",__func__,header.Content_length);
				p_file_seq->pbfifo_http_state = HTTP_RECV;
				clear_pb_fifo_kw(p_file_seq->p_pb_fifo_handle);
				p_file_seq->pbfifo_stream_pos = p_file_seq->pbfifo_connect_off;
				total_Len = p_file_seq->pbfifo_stream_pos;
			}
			//flag_buf = 0;
			//start_tick = mtos_ticks_get();
			reconnect_recv_times = 5;
			reconnect_recv_len = 0;
			
			p_file_seq->pbfifo_http_need_reconnect = FALSE;
		}
#if 0
		//test for low download speed
		if(total_Len>0){
			p_file_seq->pbfifo_connect_off = 2048;
			memset(extra_header, 0, strlen(extra_header));
			sprintf(extra_header,"Range: bytes=%ld", p_file_seq->pbfifo_connect_off);
			FILE_SEQ_LOG("[%s]  extra_header: %s\n",__func__,extra_header);
			sprintf(extra_header,"Range: bytes=%lu", p_file_seq->pbfifo_connect_off);
			strcat(extra_header, tmp);
			FILE_SEQ_LOG("[%s]  extra_header: %s\n",__func__,extra_header);

			
			FILE_SEQ_DEBUG("[%s] test for low download speed! continue, wait: %dms\n", __func__, (cur_tick - start_tick)*10);
			mtos_task_sleep(100);
			continue;
		}
#endif	

               if(p_file_seq->pbfifo_http_state == HTTP_RECV || p_file_seq->hls_parser.enable){
			   //check the free size
			   filled_space = tell_pb_fifo_kw_lite(p_pb_fifo_handle);
			   free_space = p_pb_fifo_handle->len - filled_space;
			   if(free_space <= 1){
			   	FILE_SEQ_LOG("[%s] pb fifo full!(free_size:%d)  wait~~~!\n", __func__,free_space);
			   	mtos_task_sleep(100);
			   	continue;
			   }
		   	   else{ 

				//compute the len of receive
		   	       if(p_pb_fifo_handle->rd_pos <= p_pb_fifo_handle->wr_pos)
					write_end = p_pb_fifo_handle->end_pos;
				else
					write_end = p_pb_fifo_handle->rd_pos;
				write_len = write_end - p_pb_fifo_handle->wr_pos;
		   	   	if(write_len > HTTP_RECV_LEN)
		   	   		write_len = HTTP_RECV_LEN;
		   	   	if(write_len <=0){
		   	   		FILE_SEQ_LOG("[%s]  write_len( %d) <=0 , r[%lx], w[%lx] \n", __func__,write_len,p_pb_fifo_handle->rd_pos, p_pb_fifo_handle->wr_pos);
                    mtos_task_sleep(100);
		   	   		continue;
			   	}
		   	   		/*
		   	   	while((p_pb_fifo_handle->wr_pos+write_len == p_pb_fifo_handle->end_pos) && p_pb_fifo_handle->rd_pos==p_pb_fifo_handle->start_pos){
					FILE_SEQ_DEBUG("[%s] pb fifo will full! wait===!\n", __func__,free_space);
		   	   		mtos_task_sleep(100);
		   		}*/

				//receive data to FIFO
				if(free_space > write_len){
                                 if(!handle && p_file_seq->pbfifo_http_state == HTTP_STOP)
                                 {
                                    handle = playback_fifo_load_m3u8ts(p_file_seq);
             
                                    if(!handle)
                                    {
                                       mtos_task_sleep(100);
                                       continue;
                                    }
                                    else{
                                        p_file_seq->pbfifo_http_state = HTTP_RECV;
                                    }
                                 }
			   	   	ret = Nw_Http_Download_Recv(handle, p_pb_fifo_handle->wr_pos, write_len);
			   	   	//FILE_SEQ_DEBUG("[%s] http receive %d(/%d) data, w[%lx], end[%lx] \n", __func__,ret.readLen,write_len,p_pb_fifo_handle->wr_pos,p_pb_fifo_handle->end_pos);
  			  	   	if(ret.status <0){
  			  	   		Nw_Http_Download_Stop(handle);
  			  	   		handle = NULL;
  			  	   		p_file_seq->pbfifo_http_state = HTTP_STOP;
              	  	   		FILE_SEQ_LOG("[%s] ret.status[%d] <0, total_Len[%lu], handle %x call Nw_Http_Download_Stop, break\n",__func__,ret.status,total_Len,handle);
              
                                        //break;
                                        //not break;
                                        p_file_seq->pbfifo_buffering_ok = TRUE;
                                        playback_fifo_update_m3u8ts_status(p_file_seq,ret.status);
                                        //mtos_task_sleep(100);
                                        //continue;
						break;
  			            }
			            else{
			            		//update the write pos of FIFO
						p_pb_fifo_handle->wr_pos += ret.readLen;
						if(p_pb_fifo_handle->wr_pos == p_pb_fifo_handle->end_pos)
							p_pb_fifo_handle->wr_pos = p_pb_fifo_handle->start_pos;
						//FILE_SEQ_DEBUG("[%s] http receive %d(/%d) data, w[%lx], end[%lx] \n", __func__,ret.readLen,write_len,p_pb_fifo_handle->wr_pos,p_pb_fifo_handle->end_pos);
						total_Len += ret.readLen;
						p_file_seq->pbfifo_stream_pos += ret.readLen;

                         //after push fifo
                         playback_fifo_update_m3u8ts_status(p_file_seq,ret.status);

						if(reconnect_recv_times != 0){
							//FILE_SEQ_LOG("[%s] after reconnection: %d received %d data \n", __func__,reconnect_recv_times,ret.readLen);
							reconnect_recv_len += ret.readLen;
							reconnect_recv_times--;
							if(reconnect_recv_times == 0){
								FILE_SEQ_LOG("[%s] received %d data after reconnection, p_file_seq->pbfifo_stream_pos(0x%x), sleep\n", __func__,reconnect_recv_len,p_file_seq->pbfifo_stream_pos);
								reconnect_recv_times = 0;
			   					mtos_task_sleep(100);
		   					}

						}
							
			            }
				     
			     }
			     else{
			     		mtos_task_sleep(100);
					FILE_SEQ_DEBUG("[%s] pb fifo will full! free_space: %d, wait===!\n", __func__,free_space);
			     }
			     
			     	  
		   	}
   	}

	loop_cnt++;
	//OS_PRINTF("[%s] loop_cnt:%d, total_Len: %d\n",__func__,loop_cnt, total_Len);

	}while(1);

       //out of while@@@@@@@@@
#if 0//def DEBUG_READ_FIFO_ONE_TADK

    	if (p_file_w) {
#ifdef __LINUX__
	        fclose(p_file_w);
	        FILE_SEQ_DEBUG("[%s] fclose p_file_w\n", __func__);
#else
	        ufs_close(p_file_w);
	        mtos_free(p_file_w);
	        FILE_SEQ_DEBUG("[%s] fclose p_file_w\n", __func__);
#endif
	        p_file_w = NULL;
    	}
#endif
	while(p_file_seq->m_play_state != FILE_SEQ_STOP)
       {    
       
       	 if (p_file_seq->m_play_state == FILE_SEQ_LOADED && p_file_seq->loadMedaiOK == FALSE) {
        	        FILE_SEQ_LOG("[%s] load error, exit ...\n", __func__);
        	        break;
        	    }
     	    
		FILE_SEQ_LOG("[%s] wait in loop ...m_play_state:%d\n", __func__, p_file_seq->m_play_state);
		mtos_task_sleep(200);
		if(p_file_seq->pbfifo_http_need_reconnect || p_file_seq->hls_parser.enable)
			goto HTTP_RECEIVE_LOOP;
       }
            
	deinit_fifo_kw(p_file_seq->p_pb_fifo_handle);
       p_file_seq->p_pb_fifo_handle = NULL;
        if(p_fifo)
              	mtos_free(p_fifo);
       FILE_SEQ_LOG("[%s] 3 deinit fifo ok!\n", __func__);
       
       p_file_seq->pbfifo_buffering_ok = FALSE;
       p_file_seq->pbfifo_http_need_reconnect = FALSE;
       p_file_seq->pbfifo_connect_off = 0;
       p_file_seq->pbfifo_stream_pos = 0;
       p_file_seq->pbfifo_http_state = HTTP_INT;
       p_file_seq->pbfifo_content_len = 0;  
	p_file_seq->is_pbfifo_task_alive = FALSE;
	FILE_SEQ_LOG("[%s] end end ...\n", __func__);

	mtos_mem_user_debug(&dbg_info);
       FILE_SEQ_DEBUG("[%s]-start men -- alloced[%d], alloced_peak[%d], rest[%d]\n",__func__,dbg_info.alloced, dbg_info.alloced_peak,dbg_info.rest_size);
#ifndef __LINUX__
   	mtos_task_exit();
#endif	

}
#endif


