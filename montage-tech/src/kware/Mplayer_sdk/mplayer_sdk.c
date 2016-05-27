/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys_types.h"
//#include "sys_define.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
//#include "mtos_sem.h"
#include "common.h"
#include "vdec.h"
#include "stdbool.h"
#include "mplayer_sdk.h"
#include "file_playback_sequence.h"

#define PVR_LOG(format, args...)               mtos_printk(format, ##args)
#define PVR_WARNING(format, args...)       mtos_printk(format, ##args)
#define PVR_DEBUG(format, args...)           mtos_printk(format, ##args)
#define PVR_ERROR(format, args...)           mtos_printk(format, ##args)

static PB_SEQ_PARAM_T        pb_seq_param = {0};
static unsigned int vpts = 0;
static int load_status = -1;
#define PATH_SIZE (4096)
#define    TIME_BASE (45)

typedef enum {
    /*!
      xxxxxxxx
      */
    PB_FALSE        = 0,
    /*!
    xxxxxxxx
    */
    PB_TRUE,
    /*!
      xxxxxxxx
      */
    PB_RUNNING,
    /*!
      xxxxxxxx
      */
    PB_IDLE,
    /*!
    xxxxxxxx
    */
    PB_EXIT,
    PB_STOP,
} FP_BP_STATE;
typedef struct {
    FP_BP_STATE state;
    int inited;
    char path[PATH_SIZE];
    char  path_in_use;
    unsigned int lock;
} fp_pb_handle_t;

static T_PLAYSTATE fplay_state = PLAY_STOP;


void (*OnBufferingUpdateCallback) (int percent);
void (*OnCompletionCallback) (int event);
void (*OnErrorCallback) (int type, int extraCode);
void (*OnPreparedCallback) ();
void (*OnSeekCompletionCallback) ();
void (*OnVideoSizeCallback) (int width, int height);
void (*PlayStateCallback)(int event, int param);
void (*PerSecondCallback)();

void mplayer_setOnBufferingUpdateListener(void(*callback)(int percent))
{
    OnBufferingUpdateCallback = callback;
}

void mplayer_setOnCompletionListener(void(*callback)(int event))
{
    OnCompletionCallback = callback;
}

void mplayer_setOnErrorListener(void(*callback)(int type, int extraCode))
{
    OnErrorCallback = callback;
}

void mplayer_setOnPreparedListener(void(*callback)())
{
    OnPreparedCallback = callback;
}

void mplayer_setOnSeekCompletionListener(void(*callback)())
{
    OnSeekCompletionCallback = callback;
}

void mplayer_setOnVideoSizeChangeListener(void(*callback)(int width, int height))
{
    OnVideoSizeCallback = callback;
}

void mplayer_setPlayStateListener(void(*callback)(int event, int param))
{
	PlayStateCallback = callback;
}

void mplayer_setPerSecondListener(void(*callback)())
{
	PerSecondCallback = callback;
}

void caller(void(*fnp)())
{
	fnp();
}

void caller_param1(void(*fnp)(), int p1)
{
	fnp(p1);
}

void caller_param2(void(*fnp)(), int p1, int p2)
{
	fnp(p1, p2);
}

int fp_file_seq_event_callback(FILE_PLAY_EVENT_E event, u32 param)
{
    unsigned int  cur_hour = 0;
    unsigned int  cur_min  = 0;
    unsigned int  cur_sec = 0;

    PVR_DEBUG("%s %d event=%d\n", __func__, __LINE__, event);

	//caller_param2(PlayStateCallback, event, param);

    switch (event) {
        case FILE_PLAYBACK_SEQ_GET_VPTS:
            vpts  =  param;   //millisecond
            cur_hour = (vpts / 1000) / 3600;
            cur_min = ((vpts / 1000) - (cur_hour * 3600)) / 60;
            cur_sec = (vpts / 1000) - (cur_hour * 3600) - cur_min * 60;
            PVR_DEBUG("[%ld:%ld:%ld],  %ld ms\n", cur_hour, cur_min, cur_sec, (vpts));
			caller(PerSecondCallback);
            break;

        case FILE_PLAYBACK_SEQ_INVALID:

    		//cbInfo.onErrorcallback(errorcode);

            PVR_DEBUG("receive fastbackward to head message ! \n");
            break;

        case FILE_PLAYBACK_NEW_SUB_DATA_RECEIVE:
            PVR_DEBUG("receive new subtitle packet message ! \n");
            break;

        case FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR:
            load_status = FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR;

			caller_param2(OnErrorCallback, (int)MPLAYER_ERROR_EVENT_LOAD_MEDIA_ERROR, 0);

            PVR_DEBUG("\n recv  FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR!!\n", __func__, __LINE__);
            break;

        case FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT:
            load_status = FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT;
            PVR_DEBUG("\n recv  FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT!!\n", __func__, __LINE__);
            break;

        case FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS:;
            load_status = FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS;
			PVR_DEBUG("\n recv  FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS!!\n", __func__, __LINE__);

			caller(OnPreparedCallback);

           	PVR_DEBUG("\n recv  FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS!!\n", __func__, __LINE__);
            break;

        case FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT:
            load_status = FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT ;
            PVR_DEBUG("\n recv  FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT!!!\n", __func__);
            break;

        case FILE_PLAYBACK_SEQ_STOP:
            load_status = FILE_PLAYBACK_SEQ_STOP;
            caller_param1(OnCompletionCallback,(int)FILE_PLAYBACK_SEQ_STOP);
            PVR_DEBUG("\n recv  FILE_PLAYBACK_SEQ_STOP [EOF]!!!\n", __func__);
            break;

        case FILE_PLAYBACK_UNSUPPORT_AUDIO:
			caller_param2(OnErrorCallback, (int)MPLAYER_ERROR_EVENT_UNSUPPORT_AUDIO, 0);
            PVR_DEBUG("\n recv  FILE_PLAYBACK_UNSUPPORT_AUDIO!!!\n", __func__);
            break;

        case FILE_PLAYBACK_UNSUPPORT_VIDEO:
			caller_param2(OnErrorCallback, (int)MPLAYER_ERROR_EVENT_UNSUPPORT_VIDEO, 0);
			PVR_DEBUG("\n recv  FILE_PLAYBACK_UNSUPPORT_VIDEO !!!\n", __func__);
            break;

        case FILE_PLAYBACK_CHECK_TRICKPLAY:
            PVR_DEBUG("\n recv  FILE_PLAYBACK_CHECK_TRICKPLAY !!!\n", __func__);
            break;

        case FILE_PLAYBACK_UNSUPPORT_MEMORY:
			caller_param2(OnErrorCallback, (int)MPLAYER_ERROR_EVENT_UNSUPPORT_MEMORY, 0);
            PVR_DEBUG("\n recv  FILE_PLAYBACK_UNSUPPORT_MEMORY !!!\n", __func__);
            break;

        case FILE_PLAYBACK_UNSUPPORT_SEEK:
			caller_param2(OnErrorCallback, (int)MPLAYER_ERROR_EVENT_UNSUPPORT_SEEK, 0);
            PVR_DEBUG("\n recv  FILE_PLAYBACK_UNSUPPORT_SEEK !!!\n", __func__);
            break;

        default :
            break;
    }

    if (event != FILE_PLAYBACK_SEQ_STOP)
    	  caller_param2(PlayStateCallback, event, param);

    return 0;
}
#if 1
void mplayer_init(MPLAYER_PARAM_T * p_mplayer_param)
{
    FILE_SEQ_T * p_file_seq =  file_seq_get_instance();
	PB_SEQ_PARAM_T * mplayer_cfg = (PB_SEQ_PARAM_T *)(p_mplayer_param->cfg);
    PVR_LOG("%s %d start\n", __func__, __LINE__);

    if (p_file_seq == NULL && mplayer_cfg) {

        memset(&pb_seq_param, 0x00, sizeof(pb_seq_param));
        PVR_LOG("%s %d start mplayer_cfg->pb_seq_mem_size=%d\n", __func__, __LINE__, mplayer_cfg->pb_seq_mem_size);
        pb_seq_param.pb_seq_mem_size  = mplayer_cfg->pb_seq_mem_size;
        pb_seq_param.pb_seq_mem_start = mplayer_cfg->pb_seq_mem_start;
        pb_seq_param.stack_size       =     mplayer_cfg->stack_size;
        pb_seq_param.task_priority    =     mplayer_cfg->task_priority;
        pb_seq_param.stack_preload_size       =     mplayer_cfg->stack_preload_size;
        pb_seq_param.preload_audio_buffer_size = mplayer_cfg->preload_audio_buffer_size;
        pb_seq_param.preload_video_buffer_size = mplayer_cfg->preload_video_buffer_size;
        pb_seq_param.preload_audio_buffer = mtos_malloc(mplayer_cfg->preload_audio_buffer_size);
        pb_seq_param.preload_video_buffer = mtos_malloc(mplayer_cfg->preload_video_buffer_size);
        pb_seq_param.vdec_policy = VDEC_QAULITY_AD_128M;

		p_file_seq = file_seq_create(&pb_seq_param);//TODO
        file_seq_set_fw_mem(p_mplayer_param->video_fw_cfg_addr, p_mplayer_param->video_fw_cfg_size, p_mplayer_param->audio_fw_cfg_addr,  p_mplayer_param->audio_fw_cfg_size);
        PVR_LOG("%s %d start\n", __func__, __LINE__);
        p_file_seq->register_event_cb(p_file_seq, fp_file_seq_event_callback);
    }

    PVR_LOG("%s %d end\n", __func__, __LINE__);
    return;
}
#else

void mplayer_init()
{
    FILE_SEQ_T * p_file_seq =  file_seq_get_instance();

    PVR_LOG("%s %d start\n", __func__, __LINE__);

    if (p_file_seq == NULL) {

 		memset(&pb_seq_param, 0x00, sizeof(pb_seq_param));
        pb_seq_param.pb_seq_mem_size  = 0;
        pb_seq_param.pb_seq_mem_start = 0;
        pb_seq_param.stack_size       =     PULL_DATA_THREAD_STACK_LEN;
        pb_seq_param.task_priority    =     FILE_PLAYBACK_PRIORITY;
        pb_seq_param.stack_preload_size       =     PRELOAD_DATA_THREAD_STACK_LEN;
        pb_seq_param.task_preload_priority    =     PRELOAD_DATA_THREAD_PRIORITY;
        pb_seq_param.preload_audio_buffer_size = PRELOAD_AUDIO_BUF_SIZE;
        pb_seq_param.preload_video_buffer_size = PRELOAD_VIDEO_BUF_SIZE;
        pb_seq_param.preload_audio_buffer = mtos_malloc(PRELOAD_AUDIO_BUF_SIZE);
        pb_seq_param.preload_video_buffer = mtos_malloc(PRELOAD_VIDEO_BUF_SIZE);
        pb_seq_param.vdec_policy = VDEC_BUFFER_AD;

		p_file_seq = file_seq_create(&pb_seq_param);//TODO
        file_seq_set_fw_mem(VIDEO_FW_CFG_ADDR, VIDEO_FW_CFG_SIZE, AUDIO_FW_CFG_ADDR, AUDIO_FW_CFG_SIZE);
        PVR_LOG("%s %d start\n", __func__, __LINE__);
        p_file_seq->register_event_cb(p_file_seq, fp_file_seq_event_callback);
    }

    PVR_LOG("%s %d end\n", __func__, __LINE__);
    return;
}
#endif
void mplayer_release()
{
    PVR_LOG("%s %d start\n", __func__, __LINE__);
    file_seq_destroy();
    mtos_free(pb_seq_param.preload_audio_buffer);
    mtos_free(pb_seq_param.preload_video_buffer);
	pb_seq_param.preload_audio_buffer = NULL;
    pb_seq_param.preload_video_buffer = NULL;
    vpts = 0;
    PVR_LOG("%s %d end\n", __func__, __LINE__);
    return;
}

void mplayer_setDataResource(char * Path)
{
    FILE_SEQ_T * p_file_seq =  file_seq_get_instance();
    PVR_LOG("%s %d start path=%s\n", __func__, __LINE__, Path);

    if (p_file_seq) {
        if (Path == NULL) {
            PVR_ERROR("[%s %d] parameter is error !\n", __func__, __LINE__);
        }

        //set path
        p_file_seq->set_file_path_ex(p_file_seq, Path, 0);
    }

    PVR_LOG("%s %d end\n", __func__, __LINE__);
    return;
}

void mplayer_prepare(void)
{
    FILE_SEQ_T * p_file_seq =  file_seq_get_instance();
    PVR_LOG("%s %d start\n", __func__, __LINE__);

    if (p_file_seq) {
        //loadmedia
        load_status = -1;
        p_file_seq->loadmedia_task(p_file_seq);
    }

    PVR_LOG("%s %d end\n", __func__, __LINE__);
    return;
}

void mplayer_start(int jumptotime, unsigned int vdec_stop_type) //jumptotime 5%N;:ACk
{
    PVR_LOG("%s %d start\n", __func__, __LINE__);

    if (load_status != FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS) {
        PVR_ERROR("[%s %d] load media is error !\n", __func__, __LINE__);
        return;
    }

    FILE_SEQ_T * p_file_seq =  file_seq_get_instance();
    PVR_LOG("%s %d fplay_state=%d\n", __func__, __LINE__, fplay_state);
    
    if (p_file_seq) {
        if (fplay_state == PLAY_STOP) {
            fplay_state = PLAY_PLAY;
			if(vdec_stop_type == MPLAYER_VDEC_SWITCH_CH_STOP)
			{
				PVR_LOG("%s %d\n", __func__, __LINE__);
            	p_file_seq->set_vdec_stop_type(p_file_seq, MPLAYER_VDEC_SWITCH_CH_STOP);
			}
			else
			{
				PVR_LOG("%s %d\n", __func__, __LINE__);
				p_file_seq->set_vdec_stop_type(p_file_seq, MPLAYER_VDEC_STOP);
			}
			p_file_seq->start(p_file_seq, jumptotime/1000);
        } else if (fplay_state == PLAY_PAUSE) {
            fplay_state = PLAY_PLAY;
            p_file_seq->resume((void *)p_file_seq);
        } else {
            return;
        }
    }

    PVR_LOG("%s %d end\n", __func__, __LINE__);
    return;
}

void mplayer_stop(void)
{
    int ret = -1;
    FILE_SEQ_T * p_file_seq =  file_seq_get_instance();
    PVR_LOG("%s %d start\n", __func__, __LINE__);

    if (p_file_seq && (fplay_state != PLAY_STOP)) {
        load_status = -1;
        fplay_state = PLAY_STOP;
        ret = p_file_seq->stop((void *)p_file_seq);

        if (ret < 0) {
            PVR_ERROR("%s %d\n", __func__, __LINE__);
            return;
        }

        while (load_status != FILE_PLAYBACK_SEQ_STOP
               && load_status != FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT
               && load_status != FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR
               && load_status != FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT
              ) {
            mtos_task_sleep(10);
        }
	 PVR_LOG("%s path: %s\n", __func__, p_file_seq->m_path[0]);
        p_file_seq = NULL;
        
        PVR_LOG("%s %d end\n", __func__, __LINE__);
    }

    return;
}


void mplayer_seekTo(int jumptotime) //jumptotime 5%N;:ACk
{
    FILE_SEQ_T * p_file_seq =  file_seq_get_instance();
    PVR_LOG("%s %d start jumptotime=%d\n", __func__, __LINE__, jumptotime);

    if (p_file_seq) {
        p_file_seq->play_at_time((void *)p_file_seq, jumptotime / 1000);
    }

    PVR_LOG("%s %d end\n", __func__, __LINE__);
    return;
}

void mplayer_pause(void)
{
    FILE_SEQ_T * p_file_seq =  file_seq_get_instance();
    PVR_LOG("%s %d start\n", __func__, __LINE__);

    if (p_file_seq) {
        fplay_state = PLAY_PAUSE;
        p_file_seq->pause((void *)p_file_seq);
    }

    PVR_LOG("%s %d end\n", __func__, __LINE__);
    return;
}

int mplayer_getCurrentPosition(void)
{
    int MovieCurTime = 0;
    FILE_SEQ_T * p_file_seq =  file_seq_get_instance();
	PVR_LOG("%s %d start\n", __func__, __LINE__);
    if (p_file_seq) {
        if (p_file_seq->only_audio_mode) {
            MovieCurTime = p_file_seq->apts_upload;
        } else {
            MovieCurTime = p_file_seq->vpts_upload / (TIME_BASE);
        }
    }
	PVR_LOG("%s %d end MovieCurTime=%d\n", __func__, __LINE__, MovieCurTime);
    return MovieCurTime;         // MovieCurTime 5%N;:ACk
}

int mplayer_getDuration(void)
{
    int ret = -1;
    FILM_INFO_T film_info;
    FILE_SEQ_T * p_file_seq =  file_seq_get_instance();
	PVR_LOG("%s %d start\n", __func__, __LINE__);
    if (p_file_seq) {
        ret = p_file_seq->get_film_info(p_file_seq, &film_info);

        if (ret < 0) {
            PVR_ERROR("%s %d\n", __func__, __LINE__);
            return -1;
        }

        if (film_info.film_duration == 0) {
			PVR_ERROR("%s %d\n", __func__, __LINE__);
            return -1;
        }
		PVR_LOG("%s %d end film_info.film_duration=%d\n", __func__, __LINE__, film_info.film_duration);
        return film_info.film_duration*1000;     //film_info.film_duration 5%N;Ck
    }
	PVR_ERROR("%s %d\n", __func__, __LINE__);
    return -1;
}

int mplayer_getVideoWidth(void)
{
    int ret = -1;
    FILM_INFO_T film_info;
    FILE_SEQ_T * p_file_seq =  file_seq_get_instance();
	PVR_LOG("%s %d start\n", __func__, __LINE__);
    if (p_file_seq) {
        ret = p_file_seq->get_film_info(p_file_seq, &film_info);

        if (ret < 0) {
            PVR_ERROR("%s %d\n", __func__, __LINE__);
            return 0;
        }
		PVR_LOG("%s %d end film_info.video_disp_w=%d\n", __func__, __LINE__, film_info.video_disp_w);
        return film_info.video_disp_w;
    }
	PVR_ERROR("%s %d\n", __func__, __LINE__);
    return 0;
}

int mplayer_getVideoHeight(void)
{
    int ret = -1;
    FILM_INFO_T film_info;
    FILE_SEQ_T * p_file_seq =  file_seq_get_instance();
	PVR_LOG("%s %d start\n", __func__, __LINE__);
    if (p_file_seq) {
        ret = p_file_seq->get_film_info(p_file_seq, &film_info);

        if (ret < 0) {
            PVR_ERROR("%s %d\n", __func__, __LINE__);
            return 0;
        }
		PVR_LOG("%s %d end film_info.video_disp_h=%d\n", __func__, __LINE__, film_info.video_disp_h);
        return film_info.video_disp_h;
    }
	PVR_ERROR("%s %d\n", __func__, __LINE__);
    return 0;
}

bool mplayer_isPlaying(void)
{
	if (fplay_state == PLAY_PLAY) {
		return true;
	}
	else
	{
		return false;
	}
}

bool mplayer_get_load_err_info(MP_LOAD_ERR_INFO *p_load_err_info)
{
	bool ret;
	FILE_SEQ_T * p_file_seq =  file_seq_get_instance();
	if(!p_load_err_info)
		return 0;
		
	ret = file_seq_get_load_err_info(p_file_seq, (LOAD_ERR_INFO *)p_load_err_info);
	return ret;
	

}