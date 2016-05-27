#ifndef _SDK_H_
#define _SDK_H_

#define   TASK_HEAP_MEM_LEN  (15*1024*1024)

#define   PULL_DATA_THREAD_STACK_LEN  (128*1024)
#define   PRELOAD_DATA_THREAD_STACK_LEN  (16)
#define   PRELOAD_AUDIO_BUF_SIZE    (16)
#define   PRELOAD_VIDEO_BUF_SIZE     (16)


typedef enum {
    PLAY_STOP   = 0,
    PLAY_PLAY   = 1,
    PLAY_PAUSE  = 2,
    PLAY_FF     = 3,
    PLAY_STEP   = 4,
    PLAY_FB     = 5,
    PLAY_SLOW   = 6,
    PLAY_REVSLOW = 7,
    PLAY_JUMP = 8,
    PLAY_PREV = 9,
    PLAY_NEXT = 10,
    PLAY_TOTAL = 11,
} T_PLAYSTATE;

typedef enum {
    MPG_EVENT_BufferingUpdateListener = 0x0,
    MPG_EVENT_CompletionListener,
    MPG_EVENT_ErrorListener,
    MPG_EVENT_PreparedListener,
    MPG_EVENT_SeekCompletionListener,
    MPG_EVENT_VideoSizeListener,
} MPG_Msg_t;

typedef enum {
    MPLAYER_ERROR_EVENT_LOAD_MEDIA_ERROR = 0x0,
	MPLAYER_ERROR_EVENT_UNSUPPORT_AUDIO,
	MPLAYER_ERROR_EVENT_UNSUPPORT_VIDEO,
	MPLAYER_ERROR_EVENT_UNSUPPORT_MEMORY,
	MPLAYER_ERROR_EVENT_UNSUPPORT_SEEK,
} Error_Event_t;
	
typedef enum {
	/*!
	  load err unknow, this is the init value
	  */
	MP_ERR_LOAD_UNKNOW,
	/*!
	  play path null
	  */
	MP_ERR_LOAD_NAME_NULL,
	/*!
	  last loading is still running
	  */
	MP_ERR_LOAD_LAST_LOAD_RUNNING,
	/*!
	  dns error, couldn't get the ip addr
	  */
	MP_ERR_LOAD_RESLV_HOSTNAME_ERR,
	/*!
	  tcp connect error
	  */
	MP_ERR_LOAD_CONNECT_ERR,
	/*!
	  open stream error, this is a general reason
	  */
	MP_ERR_LOAD_OPEN_STREAM_ERR,
	/*!
	  open demux error, this is a general reason
	  */
	MP_ERR_LOAD_DEMUX_ERR,
	/*!
	  unsupport av codec type
	  */
	MP_ERR_LOAD_AV_UNSUPPORT,
 

} MP_ERR_LOAD;

typedef enum {
        /*!
          xxxxxxxx
          */
        MPLAYER_VDEC_STOP = 0,
        /*!
          xxxxxxxx
          */
        MPLAYER_VDEC_FREEZE_STOP = 1,
        /*!
          xxxxxxxx
          */
        MPLAYER_VDEC_SWITCH_CH_STOP = 2,

    } mplayer_vdec_stop_type_t;

typedef struct {
	/*!
	  xxxxxxxx
	  */
	MP_ERR_LOAD err_type;
	/*!
	  xxxxxxxx
	  */
	char url[4096];
} MP_LOAD_ERR_INFO;

/*!
  xxxxxxxx
  */
typedef struct {
	/*!
	xxxxxxxx
	*/
	void* cfg;
	unsigned int video_fw_cfg_addr;
	unsigned int video_fw_cfg_size;
	unsigned int audio_fw_cfg_addr;
	unsigned int audio_fw_cfg_size; 
} MPLAYER_PARAM_T;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

void mplayer_setOnBufferingUpdateListener(void(*callback)(int percent));
void mplayer_setOnCompletionListener(void(*callback)(int event));
void mplayer_setOnErrorListener(void(*callback)(int type, int extraCode));
void mplayer_setOnPreparedListener(void(*callback)());
void mplayer_setOnSeekCompletionListener(void(*callback)());
void mplayer_setOnVideoSizeChangeListener(void(*callback)(int width, int height));
void mplayer_setPlayStateListener(void(*callback)(int event, int param));
void mplayer_setPerSecondListener(void(*callback)());


void mplayer_init(MPLAYER_PARAM_T * p_mplayer_param);
void mplayer_release();
void mplayer_setDataResource(char * Path);
void mplayer_prepare(void);
void mplayer_start(int jumptotime, unsigned int vdec_stop_type);
void mplayer_stop(void);
void mplayer_seekTo(int jumptotime);
void mplayer_pause(void);
int mplayer_getCurrentPosition(void);
int mplayer_getDuration(void);
int mplayer_getVideoWidth(void);
int mplayer_getVideoHeight(void);
bool mplayer_isPlaying(void);
bool mplayer_get_load_err_info(MP_LOAD_ERR_INFO *p_load_err_info);
    
#ifdef __cplusplus
}
#endif // __cplusplus


#endif
