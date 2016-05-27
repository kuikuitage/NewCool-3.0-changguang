#ifndef __UI_VIDEO_API_H__
#define __UI_VIDEO_API_H__
#include "file_playback_sequence.h"
/*!
 * Debug on/off
 */
#define UI_VIDEO_PRINF OS_PRINTF

#ifdef WIN32
#define STRCMPI strcmpi
#else
#define STRCMPI strcasecmp
#endif
/*!
 *Preview x/y
 */
#define VIDEO_V_PREV_X  0
#define VIDEO_V_PREV_Y  0
#define VIDEO_V_PREV_W  1280
#define VIDEO_V_PREV_H  720

/*!
 *Preview offset
 */
#define VIDEO_V_PREV_OFFSET_X  160
#define VIDEO_V_PREV_OFFSET_Y  64


typedef enum
{
    /*!
     Play end event
     */
    MSG_VIDEO_EVENT_EOS = MSG_EXTERN_BEGIN + 1200,
    /*!
     Update play time
     */
    MSG_VIDEO_EVENT_UP_TIME,
    /*!
     Update video resolution
     */
    MSG_VIDEO_EVENT_UP_RESOLUTION,
    /*!
     File play bar close
     */
    MSG_VIDEO_EVENT_FULLSCREEN_CLOSE,
    /*!
     File play unsupport video event
     */
    MSG_VIDEO_EVENT_UNSUPPORTED_VIDEO,
    /*!
     File play trick to begin
     */
    MSG_VIDEO_EVENT_TRICK_TO_BEGIN,
    /*!
     File play unsupport video memory
     */
    MSG_VIDEO_EVENT_UNSUPPORTED_MEMORY,
    /*!
     File play unsupport seek operation
     */
    MSG_VIDEO_EVENT_UNSUPPORT_SEEK,
    /*!
     File play load media user exit
     */
    MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT,
    /*!
     File play load media error
     */
    MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR,
    /*!
     File play load media success
     */
    MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS,
    /*!
     File es task exit
     */
    MSG_VIDEO_EVENT_STOP_CFM,
    /*!
     File play update network speed and loading percent, (X)Byte/s, (Y)%
     */
    MSG_VIDEO_EVENT_UPDATE_BPS,
    /*!
     File play network speed is low, request to change url src
     */
    MSG_VIDEO_EVENT_REQUEST_CHANGE_SRC,
    /*!
     File play set path failed
     */
    MSG_VIDEO_EVENT_SET_PATH_FAIL,
    /*!
     File play update loading media cost time  X(s)
     */
    MSG_VIDEO_EVENT_LOAD_MEDIA_TIME,
    /*!
     File play finish show loading bps
     */
    MSG_VIDEO_EVENT_FINISH_BUFFERING,
    /*!
    * notify upper layer that player has already finish updating bps
    */
    MSG_VIDEO_EVENT_FINISH_UPDATE_BPS,
    /*!
     Event max
     */
    MSG_VIDEO_EVENT_MAX,
}ui_video_event_t;

typedef enum
{
    VIDEO_PLAY_FUNC_INVALID = 0,
    VIDEO_PLAY_FUNC_USB,
    VIDEO_PLAY_FUNC_ONLINE,
    VIDEO_PLAY_FUNC_ONLINE_IPTV
}ui_video_play_func_t;

typedef enum
{
    VDO_PLAY_MODE_VOD = OTT_VOD_MODE,
    VDO_PLAY_MODE_LIVE = OTT_LIVE_MODE,
    VDO_PLAY_MODE_INVALID = OTT_INVALID_MODE,
} ui_play_mode_t;
/*!
 *Struct for play time
 */
typedef struct
{
    /*!
     *Hours
     */
    u8 hour;
    /*!
     *minitues
     */
    u8 min;
    /*!
     *Seconds
     */
    u8 sec;
}video_play_time_t;
/*!
 *Struct for resolution
 */
typedef struct
{
    /*!
     * file play resolution pixel x
     */
    int pixel_x;
    /*!
     * file play resolution pixel y
     */
    int pixel_y;
}video_play_pixel_t;

RET_CODE ui_video_v_open(u32 para1, u32 para2);
void _ui_video_v_set_full_stop_states(BOOL states);
RET_CODE ui_video_subt_open(u32 para1, u32 para2);
void ui_video_subt_bg_init(void);
void ui_video_subt_show(void);
/*!
 * Ui video control function
 */
RET_CODE ui_video_c_create(u8 play_func);
RET_CODE ui_video_c_play_by_file(u16 *p_file_name, u32 seek_play_time);
RET_CODE ui_video_c_play_by_url_ex(u8 **pp_url_list, u32 url_cnt, u32 seek_play_time, ui_play_mode_t play_mode);
RET_CODE ui_video_c_play_by_url(u8 *p_net_url, u32 seek_play_time, ui_play_mode_t play_mode);
RET_CODE ui_video_c_stop(void);
RET_CODE ui_video_c_destroy(void);
RET_CODE ui_video_c_pause_resume(void);
RET_CODE ui_video_c_set_speed(void);
RET_CODE ui_video_c_sub_on(void *p_attr);
RET_CODE ui_video_c_sub_off(void);
RET_CODE ui_video_c_sub_hide(void);
RET_CODE ui_video_c_sub_show(void);
RET_CODE ui_video_c_fast_play(void);
RET_CODE ui_video_c_fast_back(void);
void ui_video_c_play_end(void);
void ui_video_c_set_playing_insert_sub_id(u8 id);
u8 ui_video_c_get_playing_insert_sub_id(void);

int ui_video_c_get_audio_track_num(void);
RET_CODE ui_video_c_change_audio_track(int idx);
RET_CODE ui_video_c_load_all_audio_track(void);
void ui_video_c_free_audio_track(void);
RET_CODE ui_video_c_get_audio_track_lang_by_idx(int idx, u16 *p_buf, u16 len);

void ui_video_c_get_resolution(rect_size_t *p_rect);
int ui_video_c_get_play_speed(void);
u32 ui_video_c_get_play_time(void);
void ui_video_c_get_total_time(video_play_time_t *p_time);
int ui_video_c_get_play_state(void);
void ui_video_c_switch_time(video_play_time_t *p_time, u32 sec);
BOOL ui_video_c_get_usr_stop_flag(void);
u32 ui_video_c_get_total_time_by_sec(void);
void ui_video_c_play_normal(void);
void ui_video_c_set_charset(str_fmt_t charset);
void ui_video_c_enable_tv_mode_change(BOOL enable);
void ui_video_c_reset_speed(void);
void ui_video_c_set_usr_stop_flag(BOOL flag);

//subtitle control
u16 ui_video_c_get_insert_sub_cnt(void);
void ui_video_c_get_insert_sub_by_idx(u16 idx, u16 *p_buf, u16 str_len);
RET_CODE ui_video_c_set_insert_sub_id_by_idx(int idx);
/*!
 * Ui video model function
 */
RET_CODE ui_video_m_load(u32 partition);
RET_CODE ui_video_m_net_load(u16*ip_address_path);

u32 ui_video_m_get_total(void);
u16 ui_video_m_get_partition_cnt(void);
void ui_video_m_get_name_by_idx(u32 idx, u16 *p_file_name, u16 str_len);
void ui_video_m_get_size_by_idx(u32 idx, u32 *p_size);
u16 * ui_video_m_get_path_by_idx(u32 idx);
file_type_t ui_video_m_get_type_by_idx(u32 idx);
RET_CODE ui_video_m_go_to_dir(u16 idx);
RET_CODE ui_video_m_back_to_parent_dir(void);
u16 * ui_video_m_get_cur_path(void);
void ui_video_m_set_cur_playing_file_by_idx(u32 idx);
void ui_video_m_set_support_flag_by_idx(u32 idx, BOOL support);
u16 *ui_video_m_get_cur_playing_file(void);
RET_CODE ui_video_m_get_next_file_idx(u16 *p_idx);
RET_CODE ui_video_m_get_prev_file_idx(u16 *p_idx);
u16 ui_video_m_get_cur_playing_idx(void);
u16 *ui_video_m_get_cur_playing_file_path(void);
BOOL ui_video_m_check_cur_playing_file_support(void);

u16 ui_video_m_get_cur_partition(void);
void ui_video_m_reset(void);
void ui_video_m_set_subt_focus(u16 focus);
u16 ui_video_m_get_subt_focus(void);
u16 * ui_video_m_get_name_ptr_by_idx(u32 idx);
u16 ui_video_m_get_video_icon(u16 *p_filename);
RET_CODE ui_video_m_find_idx_by_cur_play_file(u32 *p_idx);
//subtitle interface
u16 * ui_video_m_get_cur_subt_path(u16 idx);
void ui_video_m_get_subt_by_idx(u16 idx, u16 *p_file_name, u16 str_len);
u16 ui_video_m_get_subt_cnt(void);
RET_CODE ui_video_m_load_subt(u16 *p_name);


void ui_video_m_save_total_time(video_play_time_t *time);
void ui_video_m_get_saved_total_time(video_play_time_t *time);
void ui_video_m_save_resolution(rect_size_t *rect);
void ui_video_m_get_saved_resolution(rect_size_t *rect);
BOOL ui_video_file_is_enter_dir(void);
handle_t get_rsc_handle();
void ui_video_m_rsc_init();
void ui_video_m_rsc_release();
BOOL ui_video_c_init_state(void);
u8 ui_video_c_get_play_func();
///////////////////////
RET_CODE ui_video_c_seek(s32 time_sec);
#endif
