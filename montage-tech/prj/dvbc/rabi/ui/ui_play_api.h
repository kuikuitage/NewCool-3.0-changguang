/****************************************************************************

 ****************************************************************************/
#ifndef __AP_PLAY_API_H__
#define __AP_PLAY_API_H__

#define PWD_DLG_FOR_PLAY_X      ((SCREEN_WIDTH-PWDLG_W)/2)
#define PWD_DLG_FOR_PLAY_Y    ((SCREEN_HEIGHT-PWDLG_H)/2)
#define MAX_PLAY_CNXT_COUNT   (8)

enum playback_msg
{
  MSG_TTX_READY = MSG_EXTERN_BEGIN + 450,
  MSG_SUBT_READY,
  MSG_DESCRAMBLE_SUCCESS,
  MSG_DESCRAMBLE_FAILED,
  MSG_DYNC_PID_UPDATE,
  MSG_SCART_VCR_DETECTED,
  MSG_LOCK_RSL,
  MSG_PB_NIT_FOUND,
  MSG_NOTIFY_VIDEO_FORMAT,
  MSG_PLAY_TUNER_LOCKED,
};

typedef enum
{
  LOCK_TYPE_NONE = 0,
  LOCK_TYPE_CHNL_LOCK = 0x1,
  LOCK_TYPE_AGE_LOCK = (0x1 << 1),
  
  LOCK_TYPE_MAX
} lock_type_t;

typedef enum
{
  SAT_PRG_TYPE = 0,
  NET_PRG_TYPE = 1,
  CAMERA_PRG_TYPE = 2,
} play_prg_type_t;

typedef struct
{
  u16  group;
  u16 prog_id;
  u8  mode;
  u8  is_scramble;
} play_info_t;

BOOL ui_recall(BOOL is_play, u16 *p_curn);
#ifdef SMSX_CA
  BOOL ui_dvbc_change_view(dvbs_view_t view_type, BOOL is_play);
#endif
void ui_shift_prog(s16 offset, BOOL is_play, u16 *p_curn);

BOOL ui_tvradio_switch(BOOL is_play, u16 *p_curn);

BOOL ui_play_prog(u16 prog_id, BOOL is_force);

void ui_stop_play(stop_mode_t type, BOOL is_sync);
void ui_auto_switch_mode_stop_play(BOOL is_sync);

void ui_set_audio_channel(u8 channel);

void ui_pause_play(void);

void ui_resume_play(void);

BOOL is_ap_playback_active(void);
void ui_enable_playback(BOOL is_enable);

void check_play_history();

void ui_clear_play_history(void);

void ui_reset_chkpwd(void);

void ui_enable_chk_pwd(BOOL is_enable);

BOOL ui_is_prog_locked(u16 prog_id);

u8 ui_get_lock_type(void);

u32 ui_get_cur_prg_tuner_id(void);
BOOL ui_play_curn_pg(void);

void ui_set_frontpanel_by_curn_pg(void);

BOOL ui_is_playing(void);

BOOL ui_is_blkscr(void);

BOOL ui_is_have_pass_pwd(void);

void ui_set_have_pass_pwd(BOOL flag);

// teletext operation
void ui_enable_vbi_insert(BOOL is_enable);

BOOL ui_is_ttx_data_ready(u16 prog_id);

void ui_set_ttx_curn_sub(u8 index);
void ui_enable_ttx(BOOL is_enable);

void ui_post_ttx_key(u32 key);

void ui_set_ttx_language(char *lang);

void ui_set_ttx_display(BOOL is_on, u32 regino_handle, s32 magzine, s32 page);

// subtitle operation
void ui_enable_subt(BOOL is_enable);

void ui_set_subt_service(u8 lang_idx, u32 type);

void ui_set_subt_service_1(char *lang_code, u32 type);

void ui_set_subt_display(BOOL is_display);

BOOL ui_is_subt_data_ready(u16 prog_id);

BOOL ui_is_tvradio_switch(u16 pgid);

BOOL ui_is_playpg_scrambled(void);

BOOL ui_get_curpg_scramble_flag(void);

void ui_set_playpg_scrambled(BOOL is_scramble);

void ui_book_play(u16 pgid);

void trans_lock_tp_info(dvbc_lock_info_t *p_lock_info, dvbs_tp_node_t *p_tp);

void trans_sat_info(sat_rcv_para_t *p_sat_rcv, sat_node_t *p_sat);

void trans_tp_info(tp_rcv_para_t *p_tp_rcv, dvbs_tp_node_t *p_tp);

void trans_pg_info(dvbs_program_t *p_pg_info, dvbs_prog_node_t *p_pg);

void load_play_paramter_by_pgid(play_param_t *p_info, u16 pg_id);

void do_lock_pg(u16 pg_id);

void ui_reset_video_aspect_mode(u32 aspect);

void ui_reset_tvmode(u32 av_mode);

BOOL is_enable_vbi_on_setting(void);

void ui_age_limit_recheck(u32 para1, u32 para2);

BOOL ui_is_prog_encrypt(void);

play_info_t ui_get_curplayinfo(void);

void ui_get_play_cnxt_copy(void *pbuf);

u8 ui_get_play_cnxt_count();

void ui_init_play_timer(u16 owner, u16 notify, u32 tmout);

void ui_deinit_play_timer(void);

void ui_play_timer_start(void);

void ui_play_timer_set_state(u8 state);

void ui_set_play_prg_type(play_prg_type_t prg_type);

play_prg_type_t ui_get_play_prg_type(void);
BOOL get_check_agelimit_flag();
BOOL do_chk_age_limit(u16 prog_id);

void ui_play_set_prog_ca_info(void);
#endif

