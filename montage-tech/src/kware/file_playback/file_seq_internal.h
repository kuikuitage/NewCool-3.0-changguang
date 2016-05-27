/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FILE_SEQ_INTERNAL_H__
#define __FILE_SEQ_INTERNAL_H__
//#define ES_END_IN_TASK 1

#define  USER_CMD_FIFO_LEN   (16)
/*
 *
 *   all types of command supported by file playback sequence
 *
 */
#define CMD_STOP        (1L << 1)
#define CMD_START        (1L << 2)
#define CMD_PAUSE        (1L << 3)
#define CMD_RESUME    (1L << 4)
#define CMD_FF2X_PLAY           (1L << 5)
#define CMD_FF4X_PLAY           (1L << 6)
#define CMD_FF8X_PLAY           (1L << 7)
#define CMD_FF16X_PLAY           (1L << 8)
#define CMD_FF32X_PLAY           (1L << 8)
#define CMD_FF64X_PLAY           (1L << 9)
#define CMD_CHECK_MEM           (1L << 10)
#define CMD_CHANGE_SPEED     (1L << 11)
#define CMD_PLAY_AT_TIME     (1L << 12)
#define CMD_CHANGE_AUDIO_TRACK     (1L << 13)
#define CMD_CHANGE_SUBT_ID    (1L << 14)
#define MP_NOPTS_VALUE        (-1LL<<63)


#define    CHECK_AV_ES_WL_BASE  (32)
#define    DEFAULT_ES_WL_VAL  (4) //sec
#define    FILE_PATH_MAX_LEN  (256)
#define   DEFAULT_VIDEO_NAME  "stream.dump"
#define   VES_TMP_BUF_LEN    (1024*128)
#define   DEFAULT_AUDIO_NAME "stream.dump.audio2"
#define   AES_TMP_BUF_LEN    (1024 * 32)
#define   TS_FILE_BUFFER_SIZE   (2*512 * 188)

#define   PACKET_HEADER_LEN (8)
#define   FRAME_SIZE_FIELD_LEN (4)
#define   PTS_FIELD_LEN  (4)
#define   VIDEO_TMP_BUF_LEN   (128*1024)
#define   VIDEO_ES_BUF_OVERFLOW_THRESHOLD  (100) // k bytes
#define   AUDIO_ES_BUF_OVERFLOW_THRESHOLD  (4*1024)
#define   DEFAULT_VIDEO_BPS   (16*1024)
#define   SUB_FIFO_LEN             (128*1024)

#define   PB_FIFO_LEN             (5*1024*1024)
#define   T_PB_FIFO_START     (1*1024*1024)
#define   HTTP_RECV_LEN		20*1024
#define   T_PB_FIFO_BUFFING_TIME  3000//ms


#define     VALID_PTS_MASK     (0x80000000)
#define     MAX_PATH_LEN                 (256*3)
#define     AUD_ASSEMBLE_BUF_LEN  (16*1024)
#define     DELIVER_VPTS_INTERVAL  (45000)//2          //1 seconds     
#define     AUDIO_MAX_BPS                (500*1000)
#define     AUDIO_DEFAULT_BPS            (15*1000)

/*
 *
 *   all types of event supported by file playback sequence
 *
 */
#define     CHECK_SYSTEM_MEM   (0x00000001)
#define     CLEAR_AUD_ES_BUF    (0x00000002)
#define     CLEAR_VIDEO_ES_BUF  (0x0000004)
#define     GET_TS_MEDIA_INFO   (0x00000008)
#define     AUTO_DELIVER_VPTS        (0x00000010)
#define   GET_ES_VIDEO_W_H (0x00000020)
#define   CHECK_TRICKPLAY  (0x00000040)
#define     MAX_EVENT                  (0x80000000)


#define    DEFAULT_SD_BPS    (100) //Kbytes/sec
#define    DEFAULT_HD_BPS    (200)//KBytes/sec
#define    DEF_SUPER_HD_BPS    (2*1024)//KBytes/sec
#define    DEFAULT_BUFFERING_SECOND   (2)//default buffer 2 seconds es data






#define    PTS_BASE    (22.50)//(90 >> 2)
#define    TIME_BASE (45)

#define BLANK_LEN  (0)
#define UPPER_SIZE(x)  ((((x) + 7) & (~7)) + BLANK_LEN)
#define IS_SET(x,y)        ((x) & (y))
#define ClearFlag(x,y)    ((x) &= ~(y))
#define SetFlag(x,y)    ((x) |= (y))



typedef  struct {

    u32 cur_tick;
    u32 old_tick;
    u32 cur_ves_num;
    u32 old_ves_num;
    u32  start_tick;
    u32  start_buffering_wl;
    u32  end_buffering_wl;
    u8   request_change_src_interval;

    BOOL  buffering_thread_islive;
    BOOL  buffering_first_flag;
    BOOL   start_show_buffering_in_playstate;
    BOOL   start_show_loadmedia_time;

} BUFFERING_PARAM_T;


typedef struct {

    int size;
    u8  * data;
    u8  * header;
    u32   header_len;
    double        pts;
    void  * p_next;

} AUDIO_PACKET_T;

/*!
  xxxxxxxx
  */
typedef enum {
    /*!
    *    player is buffering data
    **/
    FILE_SEQ_BUFFERING_END,
    /*!
    *    player is buffering data
    **/
    FILE_SEQ_BUFFERING,


} FILE_SEQ__INTERNAL_STATUS;
/*!
  xxxxxxxx
  */
typedef enum {
    /*!
    *    player is buffering data
    **/
    CHIPTYPE_WARRIORS,
    /*!
    *    player is buffering data
    **/
    CHIPTYPE_SONATA,
    /*!
    *    player is buffering data
    **/
    CHIPTYPE_CONCERTO,


} FILE_SEQ_CHIPTYPE;

typedef struct {


    BOOL es_task_deleting;
    BOOL load_task_deleting;
    BOOL vitual_es_enable;
    int  vitual_video_es_max_size;
    int  vitual_video_es_cur_size;
    void * vitual_video_es_pos;
    void * vitual_video_es_buf;
    int  vitual_audio_es_max_size;
    int  vitual_audio_es_cur_size;
    void * vitual_audio_es_buf;
    void * vitual_audio_es_pos;
    int vitual_audio_size;
    int  task_idle;
    int is_timeshift;
    FILE_SEQ_CHIPTYPE chip_type;
    FILE_SEQ_STATUS internal_state;
    int change_subt_got_pkt;
    int cur_sub_packet_size;
    



} PLAYBACK_INTERNAL_T;

int    x_push_user_cmd(void * hdl, FP_USER_CMD_T * p_cmd);
int     x_pop_user_cmd(void * hdl,  FP_USER_CMD_T * p_out_cmd) ;
void   x_clear_cmd_fifo(void * hdl);
void   x_reset_cmd_fifo();

void do_changeAudioTrack();
void do_playAtTime();

void mutipath_preload_thread(void * p_param);

void x_unLoadMedia(void * p_handle);
BOOL x_loadMedia(void * p_handle);

int get_preload_buffer(preload_buffer_t * buffer_start, u32 size, u8 ** buffer);

BOOL  x_check_av_codec_type(void * p_handle);
BOOL x_loadMedia(void * p_handle);
BOOL  x_get_av_dec_cap(void * pHandle);
int x_map_es_buffer(void * pHandle);
void x_unmap_es_buffer(void * pHandle);
void x_clear_pti_es_counter(void * pHandle);
void x_start_av_decoder(void * pHandle);
void x_init_av_device(void * pHandle);
void x_deinit_av_dev(void * pHandle);
void x_stop_av_decoder(void * pHandle);
void   handle_pending_event();
void  fill_es_task(void * p_param);
void * file_seq_mem_alloc(void * p_handle, u32 size);
void file_seq_mem_free(void * p_handle, void * p_buf);
BOOL isEsWaterLevelEnough(int sec);
void handle_audio_es(FILE_SEQ_T * p_file_seq);
void handle_video_es(FILE_SEQ_T * p_file_seq);
BOOL isNetworkStream(void * p_handle);
int do_user_cmd();
void    handle_exit_fill_es_task();
void handle_subtitle_data(FILE_SEQ_T * p_file_seq);
void handle_audio_es_buffering(FILE_SEQ_T * p_file_seq);
BOOL set_demux_media_info(void * p_handle);
BOOL adec_push_audio_es(u8 * start_a, int in_size_a, u32 apts);











AUDIO_PACKET_T  * audioBuffering_create_list();

void  audioBuffering_destroy_list(AUDIO_PACKET_T * p_List);

AUDIO_PACKET_T  * audioBuffering_get_packet(AUDIO_PACKET_T * list);

void  audioBuffering_add_packet(AUDIO_PACKET_T * list,
                                u32 size, u8 * data,
                                u8 * header, u32 headerLen, double pts);

void  audioBuffering_clear_list(AUDIO_PACKET_T * p_List);

void  audioBuffering_destroy_packet(AUDIO_PACKET_T * p_packet);
void playback_fifo_thread(void * p_param);
void playback_fifo_thread_lite(void * p_param);
void check_buffering_thread(void * p_param);


#endif
