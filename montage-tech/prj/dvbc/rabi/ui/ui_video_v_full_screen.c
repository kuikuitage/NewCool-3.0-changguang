/****************************************************************************
****************************************************************************/
#include "ui_common.h"

#include "ui_video_v_full_screen.h"
#include "ui_pause.h"
#include "pvr_api.h"
#include "ui_video.h"
#include "file_list.h"
#include "pnp_service.h"
#include "ui_do_search.h"
#include "ui_xupgrade_by_rs232.h"
#include "ui_mute.h"
#include "ui_jump.h"
#include "lib_char.h"
#include "ui_signal.h"
#include "file_play_api.h"
#include "ui_video_goto.h"
#include "ui_text_encode.h"
#include "ui_fp_audio_set.h"
#include "Ui_fun_help.h"

#define M_VIDEO_UNSUPPORT_SHOW_TIME  3000
#define M_VIDEO_UNSUPPORT_SEEK_TIME  1000

u8 mbox_index=0;

typedef enum 
{
    IDC_FILEPLAY_BAR_CONT = 1,
    IDC_FILEPLAY_BAR_SPEED_PLAY_CONT,
  
}control_id;

typedef enum 
{
    IDC_FILEPLAY_ICON = 1,
#ifdef ENABLE_TRICK_PLAY
    IDC_FILEPLAY_FB_STAT_ICON, 
#endif
    IDC_FILEPLAY_PAUSE_STAT_ICON,
    IDC_FILEPLAY_PLAY_STAT_ICON,
    IDC_FILEPLAY_PREV_STAT_ICON,           
    IDC_FILEPLAY_NEXT_STAT_ICON,
#ifdef ENABLE_TRICK_PLAY    
    IDC_FILEPLAY_FF_STAT_ICON,
#endif
    IDC_FILEPLAY_TITLE,
    IDC_FILEPLAY_NAME,
    IDC_FILEPLAY_PLAYING_TIME,
    IDC_FILEPLAY_F_TIME,
    IDC_FILEPLAY_TOTAL_TIME,
    IDC_FILEPLAY_PLAY_PROGRESS,
    IDC_FILEPLAY_SEEK,
    IDC_FILEPLAY_SPECIAL_PLAY_IMG_FRAME,
    IDC_FILEPLAY_SPECIAL_PLAY_TXT_FRAME,

    IDC_FILEPLAY_BPS_CONT_CONT,
    IDC_FILEPLAY_BPS_CONT,
    IDC_FILEPLAY_BPS,
    IDC_FILEPLAY_BUF_PERCENT,
    IDC_FILEPLAY_LOADING_MEDIA_TIME_CONT_CONT,
    IDC_FILEPLAY_LOADING_MEDIA_TIME_CONT,
    IDC_FILEPLAY_LOADING_MEDIA_TIME,
    IDC_FILEPLAY_SORT_LIST,
}sub_control_id;

typedef struct
{
    BOOL enable_seek;
    BOOL do_change_movie;
    s32  seek_sec;
}ui_video_v_full_screen_t;

static list_xstyle_t fileplay_list_field_rstyle =
{
  RSI_MEDIO_LEFT_LIST_BG,
  RSI_MEDIO_LEFT_LIST_BG,
  RSI_MEDIO_LEFT_LIST_HL,
  RSI_MEDIO_LEFT_LIST_SH,
  RSI_MEDIO_LEFT_LIST_HL,
};
static list_xstyle_t fileplay_list_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};
static list_field_attr_t fileplay_sort_list_field_attr[FILEPLAY_SORT_LIST_FIELD_NUM] =
{
  { LISTFIELD_TYPE_STRID | STL_LEFT | STL_VCENTER,
    FILEPLAY_SORT_LIST_MIDW - 10, 10, 0, &fileplay_list_field_rstyle, &fileplay_list_field_fstyle },
};

static u16 play_letter = 0;
static ui_video_v_full_screen_t g_video_full_screen = {0};
static RET_CODE on_fileplay_cacel_show_loading_media_time(void);
u16 fileplay_bar_mainwin_keymap(u16 key);
static RET_CODE _ui_video_v_fscreen_play_next(void);
extern u32 get_video_is_usb();
extern void set_play_focus(u16 index);
RET_CODE fileplay_bar_mainwin_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE on_fileplay_cacel_bps(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 _ui_video_f_txt_keymap(u16 key);
RET_CODE _ui_video_f_txt_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 video_f_sort_list_keymap(u16 key);
RET_CODE video_f_sort_list_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

static void _ui_video_v_fscreen_set_pn_change(BOOL enable)
{   
    av_set_t           av_set  = {0};
    avc_video_mode_1_t tv_mode = AVC_VIDEO_MODE_AUTO_1;

    sys_status_get_av_set(&av_set);
    
    tv_mode = sys_status_get_sd_mode(av_set.tv_mode);  

    /*!
     * tv mode is p or n, don't enable auto change mode
     * tv mode is auto, enable or disable auto change mode
     */
    if((AVC_VIDEO_MODE_AUTO_1 == tv_mode)&& (enable))
    {
        ui_video_c_enable_tv_mode_change(TRUE);
    }
    else
    {
        ui_video_c_enable_tv_mode_change(FALSE);
    }
}

static control_t *_ui_video_v_fscreen_get_bar_cont(void)
{
    control_t *p_mainwin  = NULL;
    control_t *p_bar_cont = NULL;
    
    p_mainwin = fw_find_root_by_id(ROOT_ID_FILEPLAY_BAR);
    MT_ASSERT(p_mainwin != NULL);

    p_bar_cont = ctrl_get_child_by_id(p_mainwin, IDC_FILEPLAY_BAR_CONT);
    MT_ASSERT(p_bar_cont != NULL);

    return  p_bar_cont;
}
static RET_CODE _ui_video_v_fscreen_up_speed_play_cont(void)
{
    int        speed             = 0;
    control_t *p_mainwin         = NULL;
    control_t *p_speed_play_cont = NULL;
    control_t *p_speed_txt       = NULL;
    control_t *p_speed_bmp       = NULL;
    control_t *p_bar_cont = NULL,*p_mbox;
    
    p_mainwin = fw_find_root_by_id(ROOT_ID_FILEPLAY_BAR);
    MT_ASSERT(p_mainwin != NULL);

    p_speed_play_cont = ctrl_get_child_by_id(p_mainwin, IDC_FILEPLAY_BAR_SPEED_PLAY_CONT);

    p_speed_bmp = ctrl_get_child_by_id(p_speed_play_cont, IDC_FILEPLAY_SPECIAL_PLAY_IMG_FRAME);
    p_speed_txt = ctrl_get_child_by_id(p_speed_play_cont, IDC_FILEPLAY_SPECIAL_PLAY_TXT_FRAME);
    
    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    p_mbox = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_FB_STAT_ICON);
    
    
    speed = ui_video_c_get_play_speed();

    if(speed > MUL_PLAY_NORMAL_PLAY)
    {
      bmap_set_content_by_id(p_speed_bmp, IM_INFORMATION_FF_2);
    }
    else if(speed < MUL_PLAY_NORMAL_PLAY)
    {
      bmap_set_content_by_id(p_speed_bmp, IM_INFORMATION_FB_2);
    }
    else
    {
      bmap_set_content_by_id(p_speed_bmp, 0);
    }
    switch(speed)
    {
      case MUL_PLAY_BACKWORD_32X:
      case MUL_PLAY_FORWORD_32X:
        {
          text_set_content_by_ascstr(p_speed_txt, "x 32");
          mbox_set_content_by_icon(p_mbox, 2, IM_XPLAY_CTRL_PLAY_F,IM_XPLAY_CTRL_PLAY);
          break;
        }
        
      case MUL_PLAY_BACKWORD_16X:
      case MUL_PLAY_FORWORD_16X:
        {
          text_set_content_by_ascstr(p_speed_txt, "x 16");
          mbox_set_content_by_icon(p_mbox, 2, IM_XPLAY_CTRL_PLAY_F,IM_XPLAY_CTRL_PLAY);
          break;
        }
        
      case MUL_PLAY_BACKWORD_8X:
      case MUL_PLAY_FORWORD_8X:
        {
          text_set_content_by_ascstr(p_speed_txt, "x 8");
          mbox_set_content_by_icon(p_mbox, 2, IM_XPLAY_CTRL_PLAY_F,IM_XPLAY_CTRL_PLAY);
          break;
        }
      
      case MUL_PLAY_BACKWORD_4X:
      case MUL_PLAY_FORWORD_4X:
        {
          text_set_content_by_ascstr(p_speed_txt, "x 4");
          mbox_set_content_by_icon(p_mbox, 2, IM_XPLAY_CTRL_PLAY_F,IM_XPLAY_CTRL_PLAY);
          break;
        }
      
      case MUL_PLAY_BACKWORD_2X:
      case MUL_PLAY_FORWORD_2X:
        {
          text_set_content_by_ascstr(p_speed_txt, "x 2");
          mbox_set_content_by_icon(p_mbox, 2, IM_XPLAY_CTRL_PLAY_F,IM_XPLAY_CTRL_PLAY);
          break;
        }
      
      case MUL_PLAY_NORMAL_PLAY:
        {
          text_set_content_by_ascstr(p_speed_txt, "");
          mbox_set_content_by_icon(p_mbox, 2, IM_XPLAY_CTRL_PUESH_F,IM_XPLAY_CTRL_PUESH);
          break;
        }
      
      default:
        break;   
    }

    ctrl_paint_ctrl(p_speed_bmp,  TRUE);
    ctrl_paint_ctrl(p_speed_txt,  TRUE);
        ctrl_paint_ctrl(p_mbox,TRUE);
    
    return SUCCESS;
}

static void _ui_video_v_fscreen_up_ctrl(control_t *p_ctrl)
{
    control_t *p_bar_cont = NULL;

    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    
    if(OBJ_STS_HIDE == ctrl_get_sts(p_bar_cont))
    {
      return;
    }  

    ctrl_paint_ctrl(p_ctrl, TRUE);
}

static void _ui_video_v_fscreen_up_unsupported_video(void)
{
    if(g_video_full_screen.do_change_movie)
    {
      //ui_comm_dlg_open(NULL, IDS_UNSUPPORTED, NULL, 0);
      ui_comm_cfmdlg_open(NULL, IDS_UNSUPPORTED, NULL, 2000);
    }
    else
    {
      //ui_comm_dlg_open(NULL, IDS_UNSUPPORTED, NULL, 3000);
      ui_comm_cfmdlg_open(NULL, IDS_UNSUPPORTED, NULL, 0);//M_VIDEO_UNSUPPORT_SHOW_TIME);

      /*Set timer for play next video*/
     /* fw_tmr_create(ROOT_ID_FILEPLAY_BAR, 
                    MSG_FP_DISAPPEAR_UNSUPPORT, 
                    M_VIDEO_UNSUPPORT_SHOW_TIME,
                    FALSE);*/
    }
}

static void _ui_video_v_fscreen_up_name(void)
{
    control_t *p_bar_cont = NULL;
    control_t *p_ctrl     = NULL;
    u16       *p_name_uni = NULL;
    
    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    p_ctrl = ctrl_get_child_by_id(ctrl_get_child_by_id(p_bar_cont,IDC_FILEPLAY_TITLE), IDC_FILEPLAY_NAME);
    p_name_uni = ui_video_m_get_cur_playing_file();
    
    text_set_content_by_unistr(p_ctrl, p_name_uni);
    _ui_video_v_fscreen_up_ctrl(p_ctrl);
}

static void _ui_video_v_fscreen_up_process_bar(void)
{  
    u32 pbar_curn  = 0;
    u32 play_time  = 0;
    u32 total_time = 0;
    control_t  *p_bar_cont     = NULL;
    control_t *p_play_progress = NULL;
    control_t *p_seek     = NULL;
    s16        steps      = 0;
    rect_t rect       = {0};
    s16    center     = 0;

    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    p_seek = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_SEEK);
  
    ctrl_empty_invrgn(p_seek);

    ctrl_get_frame(p_seek, &rect); 
    
    center = (rect.left + rect.right ) / 2; 
    p_play_progress = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_PLAY_PROGRESS);
  
    play_time = ui_video_c_get_play_time();
    total_time = ui_video_c_get_total_time_by_sec();
    
    if(0 != total_time)
    {
      pbar_curn = (play_time * FILEPLAY_BAR_PROGRESS_STEP / total_time);
    }
    steps = pbar_curn - center + FILEPLAY_TRICK_X;
     ctrl_move_ctrl(p_seek, steps, 0);
    pbar_set_current(p_play_progress, (u16)pbar_curn);
    ctrl_paint_ctrl(p_bar_cont, TRUE);
    _ui_video_v_fscreen_up_ctrl(p_play_progress);
}

static void _ui_video_v_fscreen_up_cur_time(void)
{   
    control_t         *p_bar_cont  = NULL;
    control_t         *p_ctrl      = NULL;
    video_play_time_t play_time    = {0};
    u8                time_str[32] = {0};

    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    p_ctrl = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_PLAYING_TIME);
    
    ui_video_c_switch_time(&play_time, ui_video_c_get_play_time());
    
    sprintf(time_str, "%.2d:%.2d:%.2d", play_time.hour, play_time.min, play_time.sec);
    
    text_set_content_by_ascstr(p_ctrl, time_str);

    _ui_video_v_fscreen_up_ctrl(p_ctrl);
    
    _ui_video_v_fscreen_up_process_bar();
}

static void _ui_video_v_fscreen_up_total_time(void)
{
    control_t         *p_bar_cont   = NULL;
    control_t         *p_ctrl       = NULL;
    video_play_time_t total_time    = {0};
    u8                time_str[32]  = {0};

    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    
    p_ctrl = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_TOTAL_TIME);
    
    ui_video_c_get_total_time(&total_time);
    
    sprintf(time_str, "%.2d:%.2d:%.2d", total_time.hour, total_time.min, total_time.sec);
    
    text_set_content_by_ascstr(p_ctrl, time_str);
    _ui_video_v_fscreen_up_ctrl(p_ctrl);
}

static s16 _ui_video_v_fscreen_get_seek_steps(control_t *p_trick)
{
    rect_t rect       = {0};
    s16    center     = 0;
    s16    new_center = 0;
    u32    total_time = 0;
    
    ctrl_get_frame(p_trick, &rect); 
    
    center = (rect.left + rect.right ) / 2;      

    total_time = ui_video_c_get_total_time_by_sec();
    
    if(0 != total_time)
    {
      new_center = (s16)((FILEPLAY_BAR_PROGRESS_W * g_video_full_screen.seek_sec) / total_time);
    }
    
    new_center += FILEPLAY_BAR_PROGRESS_X;

    return (new_center - center);
    
}

static void _ui_video_v_fscreen_show_seek_icon(void)
{
    control_t *p_bar_cont = NULL;
    control_t *p_seek     = NULL;
    s16        steps      = 0;

    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    p_seek = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_SEEK);
  
    ctrl_empty_invrgn(p_seek);

    steps = _ui_video_v_fscreen_get_seek_steps(p_seek);
    ctrl_move_ctrl(p_seek, steps, 0);
    
    ctrl_set_sts(p_seek, OBJ_STS_SHOW);
    
    ctrl_paint_ctrl(p_bar_cont, TRUE);
}

/*static void _ui_video_v_fscreen_paint_icon(control_t *p_ctrl, u8 idc, u16 bmp_id)
{
    control_t *p_subctrl = NULL;    
    
    p_subctrl = ctrl_get_child_by_id(p_ctrl,  idc);
    
    bmap_set_content_by_id(p_subctrl,  bmp_id);
    ctrl_paint_ctrl(p_subctrl,  TRUE);
}

static void _ui_video_v_fscreen_up_normal_icon()
{
    control_t *p_bar_cont = NULL;
    
    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    
    _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_PREV_STAT_ICON, IM_MP3_ICON_FB);
    _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_PLAY_STAT_ICON, IM_MP3_ICON_PLAY_2);
    _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_PAUSE_STAT_ICON, IM_MP3_ICON_PAUSE);
    _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_NEXT_STAT_ICON, IM_MP3_ICON_FF);
#ifdef ENABLE_TRICK_PLAY    
    _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_FB_STAT_ICON, IM_MP3_ICON_FB_V2);
    _ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_FF_STAT_ICON, IM_MP3_ICON_FF_V2);
#endif
}
*/
//only for PLAY PAUSE FF FB, PREV and NEXT don't need use this func 
static void _ui_video_v_fscreen_up_select_icon(void)
{
    control_t *p_bar_cont = NULL,*p_mbox;
    mul_fp_play_state_t state = (mul_fp_play_state_t)ui_video_c_get_play_state();
    
    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();
    p_mbox = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_FB_STAT_ICON);
    switch(state)
    {
      case MUL_PLAY_STATE_PLAY:
      case MUL_PLAY_STATE_LOAD_MEDIA:
      {
        mbox_set_content_by_icon(p_mbox, 2, IM_XPLAY_CTRL_PUESH_F, IM_XPLAY_CTRL_PUESH);
        break;
      }
      case MUL_PLAY_STATE_PAUSE:
      case MUL_PLAY_STATE_STOP:
      case MUL_PLAY_STATE_SPEED_PLAY:
      {
        mbox_set_content_by_icon(p_mbox, 2, IM_XPLAY_CTRL_PLAY_F,IM_XPLAY_CTRL_PLAY);
        break;
      }
      default:
      {
        break;
      }
    }
    ctrl_paint_ctrl(p_mbox,TRUE);
}

static void _ui_video_v_fscreen_create_timer(BOOL create)
{
    osd_set_t osd_set = {0};
    sys_status_get_osd_set(&osd_set);

    if(FALSE == create)
    {
      fw_tmr_reset(ROOT_ID_FILEPLAY_BAR, MSG_FP_HIDE_BAR, (osd_set.timeout * 1000));
    }
    else
    {
      fw_tmr_create(ROOT_ID_FILEPLAY_BAR, MSG_FP_HIDE_BAR, (osd_set.timeout * 1000), FALSE);
    }
}
static void _ui_video_v_fscreen_show_bar(void)
{
    control_t *p_bar_cont = NULL;

    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();

    if(OBJ_STS_SHOW != ctrl_get_sts(p_bar_cont))
    {
      ctrl_set_sts(p_bar_cont, OBJ_STS_SHOW);
        manage_open_menu(ROOT_ID_FUN_HELP, 0, 0);
      ctrl_paint_ctrl(p_bar_cont, TRUE);
      _ui_video_v_fscreen_create_timer(TRUE);
    }
    else
    {
      _ui_video_v_fscreen_create_timer(FALSE);
    }
    ui_video_c_sub_hide();

    //_ui_video_v_fscreen_up_clear_seek_icon(); 
}

static void _ui_video_v_fscreen_up_all_icon(void)
{
    _ui_video_v_fscreen_show_bar();
    //_ui_video_v_fscreen_up_normal_icon();
    _ui_video_v_fscreen_up_select_icon();
    //_ui_video_v_fscreen_up_clear_seek_icon();   
}

static void _ui_video_v_fscreen_up_all(void)
{   
    _ui_video_v_fscreen_up_all_icon();
    _ui_video_v_fscreen_up_name();
    _ui_video_v_fscreen_up_cur_time();
    _ui_video_v_fscreen_up_total_time();
    _ui_video_v_fscreen_up_speed_play_cont();
}

static void _ui_video_v_fscreen_close(void)
{
    control_t *p_ctrl = NULL;
  
    ui_video_c_sub_hide();
  
    g_video_full_screen.enable_seek = FALSE;
    g_video_full_screen.do_change_movie = FALSE;
    g_video_full_screen.seek_sec = 0;
    
    if(ui_is_mute())
    {
      close_mute();
    }

    fw_tmr_destroy(ROOT_ID_FILEPLAY_BAR, MSG_FP_HIDE_BAR);
    fw_tmr_destroy(ROOT_ID_FILEPLAY_BAR, MSG_FP_DISAPPEAR_UNSUPPORT);
    
    /*!
     * Preview needn't auto change tv mode
     */
    _ui_video_v_fscreen_set_pn_change(FALSE);
      
    /*!
     Send event to ui_video_v to update current playing file
     */
    p_ctrl = fw_find_root_by_id(ROOT_ID_USB_FILEPLAY);
  
    if(NULL != p_ctrl)
    {
      fw_notify_root(p_ctrl,
                     NOTIFY_T_MSG,
                     TRUE,
                     MSG_VIDEO_EVENT_FULLSCREEN_CLOSE,
                     0,
                     0);
    }
    manage_close_menu(ROOT_ID_FILEPLAY_BAR, 0,0);
    manage_open_menu(ROOT_ID_FUN_HELP, 0, 0);
}
static void _ui_video_v_fscreen_destroy(void)
{
    mul_fp_play_state_t state = (mul_fp_play_state_t)ui_video_c_get_play_state();

    if(fw_find_root_by_id(ROOT_ID_POPUP))
    {
      ui_comm_dlg_close();
    }

    if(MUL_PLAY_STATE_SPEED_PLAY == state)
    {
      ui_video_c_play_normal();
      ui_set_mute(ui_is_mute());
    }
    
    _ui_video_v_fscreen_close();
}

static RET_CODE _ui_video_v_fscreen_exit(control_t *p_ctrl, 
                                         u16 msg,
                                         u32 para1, 
                                         u32 para2)
{
    control_t *p_bar_cont = NULL;
    
    if(para1 == V_KEY_YELLOW)
    {
        p_bar_cont = ui_comm_root_get_ctrl(ROOT_ID_FILEPLAY_BAR, IDC_FILEPLAY_BAR_CONT);
        ctrl_set_sts(p_bar_cont, OBJ_STS_HIDE);
        ctrl_erase_ctrl(p_bar_cont);
    }
    _ui_video_v_fscreen_destroy();
    return SUCCESS;
}
static RET_CODE _ui_video_v_fscreen_plug_out(control_t *p_ctrl, 
                                         u16 msg,
                                         u32 para1, 
                                         u32 para2)
{
    u32 usb_dev_type = para2;
    if(usb_dev_type == HP_WIFI || !get_video_is_usb())
    {
      OS_PRINTF("####wifi device  plug out or samba plug out usb#####\n");
      return ERR_FAILURE;
    }

   if(ui_usb_get_cur_used(play_letter) != ui_usb_get_plug_partition_sev())
   {
	    return SUCCESS;
   }
    _ui_video_v_fscreen_destroy();
    
    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_on_show_bar(control_t *p_ctrl, 
                                                u16 msg,
                                                u32 para1, 
                                                u32 para2)
{
    _ui_video_v_fscreen_show_bar();
    return SUCCESS;
}
static void _ui_video_v_fscreen_hide_bar(control_t *p_bar_cont)
{
      fw_tmr_destroy(ROOT_ID_FILEPLAY_BAR, MSG_FP_HIDE_BAR);
      ctrl_set_sts(p_bar_cont, OBJ_STS_HIDE);
      ctrl_erase_ctrl(p_bar_cont);
}
static RET_CODE _ui_video_v_fscreen_on_hide_bar(control_t *p_ctrl, 
                                                u16 msg,
                                                u32 para1, 
                                                u32 para2)
{
    control_t *p_bar_cont = NULL;
  
    p_bar_cont = ctrl_get_child_by_id(p_ctrl, IDC_FILEPLAY_BAR_CONT);
    
    if(OBJ_STS_SHOW == ctrl_get_sts(p_bar_cont))
    {
      _ui_video_v_fscreen_hide_bar(p_bar_cont);
      close_fun_help_later();
      ui_video_subt_show();
    }
    else
    {
      _ui_video_v_fscreen_destroy();
    }
    
    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_play_pause(control_t *p_ctrl, 
                                               u16 msg,
                                               u32 para1, 
                                               u32 para2)
{
    mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;
    state = (mul_fp_play_state_t)ui_video_c_get_play_state();

    if (state == MUL_PLAY_STATE_LOAD_MEDIA)
    {
      return SUCCESS;
    }

    on_fileplay_cacel_bps(p_ctrl, msg, para1, para2);//for bug 49489

    ui_video_c_pause_resume();

    state = (mul_fp_play_state_t)ui_video_c_get_play_state();
    
    if(MUL_PLAY_STATE_PLAY == state)
    {
      OS_PRINTF("[%s] mute = %d",  __FUNCTION__, ui_is_mute());
      ui_set_mute(ui_is_mute());
    }
    
    _ui_video_v_fscreen_up_speed_play_cont();
    _ui_video_v_fscreen_up_all_icon();

    return SUCCESS;
}
static RET_CODE _ui_video_v_fscreen_adjust_volume(control_t *p_ctrl, 
                                               u16 msg,
                                               u32 para1, 
                                               u32 para2)
{
    return manage_open_menu(ROOT_ID_VOLUME_USB, 
                            ROOT_ID_FILEPLAY_BAR, 
                            (msg == MSG_INCREASE) ? V_KEY_LEFT : V_KEY_RIGHT);
}


static RET_CODE _ui_video_v_fscreen_left_right(control_t *p_bar_cont, 
                                               u16 msg,
                                               u32 para1, 
                                               u32 para2)
{   
    s32 jump_time  = 0;
    s32 play_time  = 0;
    s32 total_time = 0;
    control_t *p_active_cont = NULL;
    mul_fp_play_state_t cur_state = MUL_PLAY_STATE_NONE;

    cur_state = (mul_fp_play_state_t)ui_video_c_get_play_state();
    if (cur_state == MUL_PLAY_STATE_LOAD_MEDIA)
    {
        return SUCCESS;
    }

    if(OBJ_STS_HIDE == ctrl_get_sts(p_bar_cont))
    {
          _ui_video_v_fscreen_show_bar();
      	  return SUCCESS;
    }  

    p_active_cont = ctrl_get_active_ctrl(p_bar_cont);
    if(IDC_FILEPLAY_FB_STAT_ICON == ctrl_get_ctrl_id(p_active_cont))
    {
  	switch(msg)
  	{
  		case MSG_FOCUS_LEFT:
  			mbox_index = (mbox_index - 1 + FILEPLAY_BAR_ICON) % FILEPLAY_BAR_ICON;
  			break;
  		case MSG_FOCUS_RIGHT:
  			mbox_index = (mbox_index + 1 + FILEPLAY_BAR_ICON) % FILEPLAY_BAR_ICON;
  			break;
  		default:
  			return ERR_FAILURE;
  	}
  	mbox_set_focus(p_active_cont, mbox_index);
  	ctrl_paint_ctrl(p_bar_cont, TRUE);
  	ctrl_paint_ctrl(p_active_cont,TRUE);
  	return SUCCESS;
    }

    jump_time = (s32)ui_jump_para_get();

    if(MUL_PLAY_STATE_PLAY == cur_state)
    {
      ui_video_c_pause_resume();
    }
    else if(MUL_PLAY_STATE_PAUSE != cur_state)
    {
      return SUCCESS;
    }

    UI_VIDEO_PRINF("[%s]: seek:%d\n", __FUNCTION__, g_video_full_screen.enable_seek);
    
    /*!
      * First time enable seek and set seek time as play time
      */
    if(FALSE == g_video_full_screen.enable_seek)
    {
      play_time = (s32)ui_video_c_get_play_time();
      g_video_full_screen.seek_sec = play_time;
      g_video_full_screen.enable_seek = TRUE;

      _ui_video_v_fscreen_up_all_icon();
    }
    
    if(msg == MSG_FOCUS_LEFT)
    {
      g_video_full_screen.seek_sec -= jump_time;

      if(g_video_full_screen.seek_sec < 0)
      {
        g_video_full_screen.seek_sec = 0;
      }
    }
    else if(msg == MSG_FOCUS_RIGHT)
    {
      g_video_full_screen.seek_sec += jump_time;

      total_time = (s32)ui_video_c_get_total_time_by_sec();

      if(g_video_full_screen.seek_sec > total_time)
      {
        g_video_full_screen.seek_sec = total_time;
      }
    }

    _ui_video_v_fscreen_show_seek_icon();

    return SUCCESS;
}                                               

static RET_CODE _ui_video_v_fscreen_confirm(control_t *p_ctrl, 
                                            u16 msg,
                                            u32 para1, 
                                            u32 para2)
{
   s32 play_time  = 0;
  
    play_time = (s32)ui_video_c_get_play_time();

    if(g_video_full_screen.enable_seek)
    {
      //g_video_full_screen.seek_sec -= play_time;
      
      ui_video_c_seek(g_video_full_screen.seek_sec);
      
      g_video_full_screen.seek_sec = 0;
      
      /*Disable seek, because in up all icon function, need hide seek icon*/
      g_video_full_screen.enable_seek = FALSE;

      _ui_video_v_fscreen_up_all_icon();
    }
    else
    {
      _ui_video_v_fscreen_show_bar();  
    }
   
    return SUCCESS;
}
static RET_CODE _ui_video_v_fscreen_stop(control_t *p_ctrl, 
                                         u16 msg,
                                         u32 para1, 
                                         u32 para2)

{
    ui_video_c_stop();

    _ui_video_v_fscreen_destroy();

    return SUCCESS;
}

#ifdef ENABLE_TRICK_PLAY
static RET_CODE _ui_video_v_fscreen_trick_play(control_t *p_ctrl, 
                                               u16 msg,
                                               u32 para1, 
                                               u32 para2)
{
    if(MSG_FP_PLAY_FF == msg)
    {
        ui_video_c_fast_play();
    }
    else
    {
        ui_video_c_fast_back();
    }

    _ui_video_v_fscreen_up_speed_play_cont();
    _ui_video_v_fscreen_up_all_icon();

    return SUCCESS;
}
#endif

static RET_CODE _ui_video_v_fscreen_play_by_focus(u16 focus)
{
    u16               *p_path = NULL;
    control_t *p_root = NULL;
  
    p_path = ui_video_m_get_path_by_idx(focus);
    if(NULL == p_path)
    {
      return ERR_FAILURE;
    }

    if(fw_find_root_by_id(ROOT_ID_POPUP))
    {
      ui_comm_dlg_close();
    }
    p_root = fw_get_focus();
    if (p_root && p_root->id != ROOT_ID_FILEPLAY_BAR)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
    }

    UI_VIDEO_PRINF("@@@_ui_video_v_fscreen_play_by_focus() idx=%d\n", focus);

    //ui_video_c_load_media_from_local(p_path, FALSE);
    ui_video_c_play_by_file(p_path, 0);

    set_play_focus(focus);
    ui_video_m_set_cur_playing_file_by_idx(focus);
    return SUCCESS;
}   

static RET_CODE _ui_video_v_fscreen_play_next(void)
{
    RET_CODE ret = ERR_FAILURE;
    u16      idx = 0;
    control_t *p_bar_cont = NULL;
    p_bar_cont = _ui_video_v_fscreen_get_bar_cont();

    _ui_video_v_fscreen_show_bar();
    //_ui_video_v_fscreen_up_normal_icon();
    //_ui_video_v_fscreen_paint_icon(p_bar_cont, IDC_FILEPLAY_NEXT_STAT_ICON, IM_MP3_ICON_FF_SELECT);
    ui_video_m_get_next_file_idx(&idx);
    _ui_video_v_fscreen_play_by_focus(idx);

    return ret;
}

static RET_CODE _ui_video_v_fscreen_process_next(control_t *p_ctrl, 
                                                 u16 msg,
                                                 u32 para1, 
                                                 u32 para2)
{
    ui_fp_audio_set_init_audio_ch_num();
    g_video_full_screen.do_change_movie = TRUE;
    _ui_video_v_fscreen_play_next();

    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_process_prev(control_t *p_ctrl, 
                                                 u16 msg,
                                                 u32 para1, 
                                                 u32 para2)
{
    u16       idx         = 0;
    control_t *p_bar_cont = NULL;
    
    ui_fp_audio_set_init_audio_ch_num();
    g_video_full_screen.do_change_movie = TRUE;
    
    p_bar_cont = ctrl_get_child_by_id(p_ctrl, IDC_FILEPLAY_BAR_CONT);

    _ui_video_v_fscreen_show_bar();
    ui_video_m_get_prev_file_idx(&idx);
    _ui_video_v_fscreen_play_by_focus(idx);

    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_on_mute(control_t *p_ctrl, 
                                            u16 msg,
                                            u32 para1, 
                                            u32 para2)
{
    ui_set_mute(!ui_is_mute());
  
    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_open_text_encode(control_t *p_ctrl, 
                                                     u16 msg,
                                                     u32 para1, 
                                                     u32 para2)
{
    manage_open_menu(ROOT_ID_TEXT_ENCODE, 0, 0);
    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_show_goto(control_t *p_ctrl, 
                                              u16 msg,
                                              u32 para1, 
                                              u32 para2)
{
    mul_fp_play_state_t cur_state = (mul_fp_play_state_t)ui_video_c_get_play_state();

    if (cur_state == MUL_PLAY_STATE_LOAD_MEDIA)
    {
      return SUCCESS;
    }
      
    if((MUL_PLAY_STATE_PAUSE == cur_state) || (MUL_PLAY_STATE_SPEED_PLAY == cur_state))
    {
      ui_video_c_pause_resume();
      _ui_video_v_fscreen_up_speed_play_cont();
      _ui_video_v_fscreen_up_all_icon();
    }
    
    _ui_video_v_fscreen_show_bar();
  
    manage_open_menu(ROOT_ID_VIDEO_GOTO, 0, 0);
  
    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_show_subt(control_t *p_ctrl, 
                                              u16 msg,
                                              u32 para1, 
                                              u32 para2)
{
    control_t *p_bar_cont = NULL;
    mul_fp_play_state_t cur_state = (mul_fp_play_state_t)ui_video_c_get_play_state();
    if (cur_state == MUL_PLAY_STATE_LOAD_MEDIA)
    {
      return SUCCESS;
    }
    p_bar_cont = ctrl_get_child_by_id(p_ctrl, IDC_FILEPLAY_BAR_CONT);
    if(OBJ_STS_SHOW == ctrl_get_sts(p_bar_cont))
    {
      _ui_video_v_fscreen_hide_bar(p_bar_cont);
    }
    manage_open_menu(ROOT_ID_FILEPLAY_SUBT, 0, 0);
    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_show_audio_set(control_t *p_ctrl, 
                                                   u16 msg,
                                                   u32 para1, 
                                                   u32 para2)
{
    control_t *p_bar_cont = NULL;
    
    p_bar_cont = ctrl_get_child_by_id(p_ctrl, IDC_FILEPLAY_BAR_CONT);
  
    if(OBJ_STS_SHOW == ctrl_get_sts(p_bar_cont))
    {
      _ui_video_v_fscreen_hide_bar(p_bar_cont);
    }
    
    return manage_open_menu(ROOT_ID_FP_AUDIO_SET, para1, 0);
}

/*static RET_CODE _ui_video_v_fscreen_on_stop_cfm(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  UI_PRINTF("@@@_ui_video_v_fscreen_on_stop_cfm()\n");
  return SUCCESS;
}*/

static RET_CODE _ui_video_v_fscreen_on_load_media_success(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    rect_size_t        rect   = {0};
    video_play_time_t  time   = {0};
    u16 focus;

    UI_PRINTF("@@@_ui_video_v_fscreen_on_load_media_success()\n");

    on_fileplay_cacel_show_loading_media_time();
    if ( ROOT_ID_FILEPLAY_BAR != fw_get_focus_id() )
    {
      return SUCCESS;
    }

    focus = ui_video_m_get_cur_playing_idx();
    ui_video_m_set_support_flag_by_idx(focus, TRUE);

    /*---get total time and resolution,then save them ----------*/
    ui_video_c_get_total_time(&time);
    ui_video_m_save_total_time(&time);
    ui_video_c_get_resolution(&rect);
    ui_video_m_save_resolution(&rect);

    _ui_video_v_fscreen_up_all();

    return SUCCESS;
}
static RET_CODE _ui_video_v_fscreen_on_load_media_error(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    u16 focus;

    UI_PRINTF("@@@_ui_video_v_fscreen_on_load_media_error()\n");

    on_fileplay_cacel_show_loading_media_time();
    if ( ROOT_ID_FILEPLAY_BAR != fw_get_focus_id() )
    {
      return SUCCESS;
    }

    focus = ui_video_m_get_cur_playing_idx();
    ui_video_m_set_support_flag_by_idx(focus, FALSE);

    _ui_video_v_fscreen_up_unsupported_video();

    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_on_eos(control_t *p_ctrl, 
                                           u16 msg,
                                           u32 para1, 
                                           u32 para2)
{
    UI_VIDEO_PRINF("[%s]: \n", __FUNCTION__);

   /*!
    * Need to clear when play is end
    */
    ui_video_c_play_end();
      
    /*This case is that playing next file in MSG_FP_DISAPPEAR_UNSUPPORT msg arrived*/
    if(NULL != fw_find_root_by_id(ROOT_ID_POPUP))
    { 
        UI_VIDEO_PRINF("[%s]: popup exist\n", __FUNCTION__);
        return SUCCESS;
    }
    
    g_video_full_screen.do_change_movie = FALSE;
    
    _ui_video_v_fscreen_play_next();

    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_up_time(control_t *p_ctrl, 
                                            u16 msg,
                                            u32 para1, 
                                            u32 para2)
{
    _ui_video_v_fscreen_up_cur_time();

    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_unsupported_video(control_t *p_ctrl, 
                                                      u16 msg,
                                                      u32 para1, 
                                                      u32 para2)
{
    u16 focus;
    UI_VIDEO_PRINF("@@@_ui_video_v_fscreen_unsupported_video\n");
    
    /**Load media failure maybe send this event to ui, so show popup once*/
    if(NULL == fw_find_root_by_id(ROOT_ID_POPUP))
    {
        UI_VIDEO_PRINF("[%s]: popup not exist\n", __FUNCTION__);
        focus = ui_video_m_get_cur_playing_idx();
        ui_video_m_set_support_flag_by_idx(focus, FALSE);
        _ui_video_v_fscreen_up_unsupported_video();
    }

    return SUCCESS;
}
/*
static RET_CODE _ui_video_v_fscreen_unsupport_seek(control_t *p_ctrl, 
                                                   u16 msg,
                                                   u32 para1, 
                                                   u32 para2)
{
    ui_comm_showdlg_open(NULL, IDS_UNSUPPORTED_SEEK, NULL, M_VIDEO_UNSUPPORT_SEEK_TIME);  
    return SUCCESS;
}
*/
static RET_CODE _ui_video_v_fscreen_trick_to_begin(control_t *p_ctrl, 
                                                   u16 msg,
                                                   u32 para1, 
                                                   u32 para2)
{
    ui_set_mute(ui_is_mute());
    ui_video_c_reset_speed();
    _ui_video_v_fscreen_up_speed_play_cont();
    _ui_video_v_fscreen_up_all_icon();
    
    return SUCCESS;
}

static RET_CODE _ui_video_v_fscreen_unsupported_timeout(control_t *p_ctrl, 
                                                        u16 msg,
                                                        u32 para1, 
                                                        u32 para2)
{
    UI_VIDEO_PRINF("[%s]: unsupport\n", __FUNCTION__);
    
    fw_tmr_destroy(ROOT_ID_FILEPLAY_BAR, MSG_FP_DISAPPEAR_UNSUPPORT);
    _ui_video_v_fscreen_play_next();
    
    return SUCCESS;
}

static control_t * _ui_video_v_fscreen_create_main(void)
{
    control_t *p_mainwin = NULL;  

    p_mainwin = fw_create_mainwin(ROOT_ID_FILEPLAY_BAR,
                                  FILEPLAY_BAR_CONT_X, FILEPLAY_BAR_CONT_Y,
                                  FILEPLAY_BAR_CONT_W, FILEPLAY_BAR_CONT_H,
                                  ROOT_ID_INVALID, 0, OBJ_ATTR_ACTIVE, 0);
    MT_ASSERT(p_mainwin != NULL);
    
    ctrl_set_rstyle(p_mainwin, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
    ctrl_set_keymap(p_mainwin, fileplay_bar_mainwin_keymap);
    ctrl_set_proc(p_mainwin, fileplay_bar_mainwin_proc);

    return p_mainwin;
}

static void _ui_video_v_fscreen_create_icon(control_t *p_bar_cont)
{
//  control_t *p_pvr_img = NULL;
u8 i;
control_t *p_txt;
  u16 play_icon_hl[FILEPLAY_BAR_ICON] =
    {
    #ifdef ENABLE_TRICK_PLAY  
    IM_XPLAY_CTRL_BBF_F,
    #endif
      IM_XPLAY_CTRL_BF_F,
      IM_XPLAY_CTRL_PLAY_F,
      IM_XPLAY_CTRL_STOP_F,
      IM_XPLAY_CTRL_BB_F,
      #ifdef ENABLE_TRICK_PLAY  
      IM_XPLAY_CTRL_BBB_F,
      #endif
    };
  u16 play_icon_sh[FILEPLAY_BAR_ICON] =
    {
    #ifdef ENABLE_TRICK_PLAY  
    IM_XPLAY_CTRL_BBF,
    #endif
      IM_XPLAY_CTRL_BF,
      IM_XPLAY_CTRL_PLAY,
      IM_XPLAY_CTRL_STOP,
      IM_XPLAY_CTRL_BB,
      #ifdef ENABLE_TRICK_PLAY  
      IM_XPLAY_CTRL_BBB,
      #endif
    };
#ifdef ENABLE_TRICK_PLAY  
  /*//FB icon
  ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_FB_STAT_ICON,
                   FILEPLAY_BAR_PLAY_STAT_ICON_X, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
                   FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, 
                   p_bar_cont, 0);

    //FF
  ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_FF_STAT_ICON,
                   FILEPLAY_BAR_PLAY_STAT_ICON_X+350, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
                   FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, 
                   p_bar_cont, 0);*/
#endif
/*
    for(i=0;i<FILEPLAY_BAR_ICON;i++)
    {
#ifdef ENABLE_TRICK_PLAY 
        p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_FB_STAT_ICON + i,
                       FILEPLAY_BAR_PLAY_STAT_ICON_X + 70 * i, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
                       FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, 
                       p_bar_cont, 0);
#else
        p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_PAUSE_STAT_ICON + i,
                       FILEPLAY_BAR_PLAY_STAT_ICON_X + 70 * i, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
                       FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, 
                       p_bar_cont, 0);
#endif
        ctrl_set_rstyle(p_txt, IM_MP3_ICON_PLAY_2, IM_MP3_ICON_PLAY_2, RSI_LEFT_CONT_BG);
        ctrl_set_keymap(p_txt, _ui_video_f_txt_keymap);
        ctrl_set_proc(p_txt, _ui_video_f_txt_proc);
        ctrl_set_related_id(p_txt,
                        ( i -1 + FILEPLAY_BAR_ICON )%FILEPLAY_BAR_ICON + 2,                      
                         0,                 
                        ( i + 1 + FILEPLAY_BAR_ICON )%FILEPLAY_BAR_ICON + 2,                  
    }*/
  p_txt = ctrl_create_ctrl(CTRL_MBOX, IDC_FILEPLAY_FB_STAT_ICON,
    FILEPLAY_BAR_PLAY_STAT_ICON_X, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
    FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, p_bar_cont, 0);
ctrl_set_rstyle(p_txt, RSI_MEDIO_PLAY_BG, RSI_MEDIO_PLAY_BG, RSI_MEDIO_PLAY_BG);
  ctrl_set_mrect(p_txt, 0, 0, FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H);
  mbox_enable_icon_mode(p_txt, TRUE);
  mbox_set_count(p_txt, FILEPLAY_BAR_ICON,
    FILEPLAY_BAR_ICON, 1);
  mbox_set_item_interval(p_txt, 0, 0);
  mbox_set_item_rstyle(p_txt, RSI_IGNORE,
    RSI_IGNORE, RSI_IGNORE);
  mbox_set_icon_align_type(p_txt, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_txt, MBOX_STRTYPE_STRID);
    for(i=0; i<FILEPLAY_BAR_ICON; i++)
  {
    mbox_set_content_by_icon(p_txt, (u8)i, play_icon_hl[i], play_icon_sh[i]);
  }


    
/*    //Prev File
  ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_PREV_STAT_ICON,
                   FILEPLAY_BAR_PLAY_STAT_ICON_X+70, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
                   FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, 
                   p_bar_cont, 0);
    
    //play icon
  ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_PLAY_STAT_ICON,
                   FILEPLAY_BAR_PLAY_STAT_ICON_X+140, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
                   FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, 
                   p_bar_cont, 0);
    
    //pause icon
  ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_PAUSE_STAT_ICON,
                    FILEPLAY_BAR_PLAY_STAT_ICON_X + 210, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
                    FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, 
                    p_bar_cont, 0);
                                    
    //Next File
  ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_NEXT_STAT_ICON,
                   FILEPLAY_BAR_PLAY_STAT_ICON_X+280, FILEPLAY_BAR_PLAY_STAT_ICON_Y,
                   FILEPLAY_BAR_PLAY_STAT_ICON_W, FILEPLAY_BAR_PLAY_STAT_ICON_H, 
                   p_bar_cont, 0);
    
    //PVR icon
    p_pvr_img = ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_ICON,
                                 FILEPLAY_BAR_ICON_X, FILEPLAY_BAR_ICON_Y,
                                 FILEPLAY_BAR_ICON_W, FILEPLAY_BAR_ICON_H, 
                                 ctrl_get_child_by_id(p_bar_cont,IDC_FILEPLAY_TITLE), 0);
    bmap_set_content_by_id(p_pvr_img, IM_INFOR_PVR);*/
}

static void create_fileplay_show_bps(control_t *p_mainwin)
{
  control_t *p_cont_cont = NULL;
  control_t *p_cont = NULL;
  control_t *p_bps = NULL;
  control_t *p_process = NULL;
  
  
  p_cont_cont = ctrl_create_ctrl(CTRL_CONT, IDC_FILEPLAY_BPS_CONT_CONT,
                          FILEPLAY_BPS_X, FILEPLAY_BPS_Y, 
                          FILEPLAY_BPS_W, FILEPLAY_BPS_H, 
                          p_mainwin, 0);
  ctrl_set_rstyle(p_cont_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_FILEPLAY_BPS_CONT,
                          0, 0, 
                          FILEPLAY_BPS_W, FILEPLAY_BPS_H, 
                          p_cont_cont, 0);
  ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
  ctrl_set_sts(p_cont, OBJ_STS_HIDE);
  p_bps = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_BPS,
                          0, 0, FILEPLAY_BPS_W/2, FILEPLAY_BPS_H, 
                          p_cont, 0);
  ctrl_set_rstyle(p_bps, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_bps, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_bps, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
  text_set_content_type(p_bps, TEXT_STRTYPE_UNICODE);

  p_process = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_BUF_PERCENT,
                          FILEPLAY_BPS_W/2, 0, FILEPLAY_BPS_W/2, FILEPLAY_BPS_H, 
                          p_cont, 0);
  ctrl_set_rstyle(p_process, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_process, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_process, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
  text_set_content_type(p_process, TEXT_STRTYPE_UNICODE);
  //fileplay_update_bps(p_bps,p_process, 123, 35);
}

static void create_fileplay_show_loading_media_time(control_t *p_mainwin)
{
  control_t *p_cont_cont = NULL;
  control_t *p_cont = NULL;
  control_t *p_ctrl = NULL;

  p_cont_cont = ctrl_create_ctrl(CTRL_CONT, IDC_FILEPLAY_LOADING_MEDIA_TIME_CONT_CONT,
                          FILEPLAY_BPS_X, FILEPLAY_BPS_Y, 
                          FILEPLAY_BPS_W, FILEPLAY_BPS_H, 
                          p_mainwin, 0);  
  ctrl_set_rstyle(p_cont_cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_FILEPLAY_LOADING_MEDIA_TIME_CONT,
                          0, 0, 
                          FILEPLAY_BPS_W, FILEPLAY_BPS_H, 
                          p_cont_cont, 0);  
  ctrl_set_rstyle(p_cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_sts(p_cont, OBJ_STS_HIDE);
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_LOADING_MEDIA_TIME,
                          0, 0, 
                          FILEPLAY_BPS_W, FILEPLAY_BPS_H, 
                          p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_ctrl, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

  
 // fileplay_update_loading_media_time(p_ctrl, 2);
}
static RET_CODE fileplay_sort_list_update(control_t* p_list, u16 start, u16 size,
                                 u32 context)
{
  u16 i;
  u16 cnt = list_get_count(p_list);
  u16 strid [FILEPLAY_SORT_LIST_ITEM_TOL] = {
    IDS_TEXT_ENCODE, IDS_GOTO,
  };

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      list_set_field_content_by_strid(p_list, (u16)(start + i), 0,
                                                            strid[start + i]);
    }
  }
  return SUCCESS;
}
RET_CODE ui_video_v_fscreen_open(u32 para1, u32 para2)
{
    control_t *p_mainwin;
    control_t *p_bar_cont;
    control_t *p_speed_play_cont;
    control_t *p_speed_bmp;
    control_t *p_speed_txt;
    control_t *p_play_time;
    control_t *p_total_time;
    control_t *p_bar_title;
    control_t *p_file_name;
    control_t *p_play_progress;
    control_t *p_trick,*p_sort_list;
    u8 i;

    //open menu.

    p_mainwin = _ui_video_v_fscreen_create_main();
    
    //create fileplaybar bps
    create_fileplay_show_bps(p_mainwin);

    //create fileplaybar show loading media time
    create_fileplay_show_loading_media_time(p_mainwin);
  
    //frame part
    p_bar_cont = ctrl_create_ctrl(CTRL_CONT, IDC_FILEPLAY_BAR_CONT,
                                         FILEPLAY_BAR_FRM_X, FILEPLAY_BAR_FRM_Y,
                                         FILEPLAY_BAR_FRM_W,FILEPLAY_BAR_FRM_H, 
                                         p_mainwin, 0);
    
    ctrl_set_rstyle(p_bar_cont, RSI_MEDIO_PLAY_BG, RSI_MEDIO_PLAY_BG, RSI_MEDIO_PLAY_BG);
    ctrl_set_keymap(p_bar_cont, _ui_video_f_txt_keymap);
    ctrl_set_proc(p_bar_cont, _ui_video_f_txt_proc);

    //speed play part
    p_speed_play_cont = ctrl_create_ctrl(CTRL_CONT, IDC_FILEPLAY_BAR_SPEED_PLAY_CONT,
                                         FILEPLAY_SPEED_PLAY_FRM_X, FILEPLAY_SPEED_PLAY_FRM_Y,
                                         FILEPLAY_SPEED_PLAY_FRM_W,FILEPLAY_SPEED_PLAY_FRM_H, 
                                         p_mainwin, 0);

    //Top-right remind special play img
    p_speed_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_SPECIAL_PLAY_IMG_FRAME,
                                 FILEPLAY_BAR_SPECIAL_PLAY_IMG_X, FILEPLAY_BAR_SPECIAL_PLAY_IMG_Y,
                                 FILEPLAY_BAR_SPECIAL_PLAY_IMG_W,FILEPLAY_BAR_SPECIAL_PLAY_IMG_H,
                                 p_speed_play_cont, 0);
    ctrl_set_rstyle(p_speed_bmp, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

    //special play txt
    p_speed_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_SPECIAL_PLAY_TXT_FRAME,
                                  FILEPLAY_BAR_SPECIAL_PLAY_TXT_X, FILEPLAY_BAR_SPECIAL_PLAY_TXT_Y,
                                  FILEPLAY_BAR_SPECIAL_PLAY_TXT_W,FILEPLAY_BAR_SPECIAL_PLAY_TXT_H,
                                  p_speed_play_cont, 0);
    ctrl_set_rstyle(p_speed_txt, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_align_type(p_speed_txt, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_speed_txt, FSI_FILEPLAY_SPECIAL_PLAY_TEXT,
                                    FSI_FILEPLAY_SPECIAL_PLAY_TEXT, FSI_FILEPLAY_SPECIAL_PLAY_TEXT);
    text_set_content_type(p_speed_txt, TEXT_STRTYPE_UNICODE);

    //title
    p_bar_title = ctrl_create_ctrl(CTRL_CONT, IDC_FILEPLAY_TITLE,
                            FILEPLAY_BAR_TITLE_X, FILEPLAY_BAR_TITLE_Y, 
                            FILEPLAY_BAR_TITLE_W, FILEPLAY_BAR_TITLE_H,
                            p_bar_cont, 0);
  ctrl_set_rstyle(p_bar_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  
  //create icons
    _ui_video_v_fscreen_create_icon(p_bar_cont);
  
    //name
    p_file_name = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_NAME,
                                   FILEPLAY_BAR_NAME_TXT_X, FILEPLAY_BAR_NAME_TXT_Y,
                                   FILEPLAY_BAR_NAME_TXT_W, FILEPLAY_BAR_NAME_TXT_H,
                                   p_bar_title, 0);
      ctrl_set_rstyle(p_file_name, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_file_name, FSI_WHITE_30, FSI_WHITE_30, FSI_WHITE_30);
  text_set_align_type(p_file_name, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_file_name, TEXT_STRTYPE_UNICODE);
    text_set_content_by_ascstr(p_file_name, "_______");
    
    // Playing time
    p_play_time = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_PLAYING_TIME,
                                   FILEPLAY_BAR_PLAY_CUR_TIME_X, FILEPLAY_BAR_PLAY_CUR_TIME_Y,
                                   FILEPLAY_BAR_PLAY_CUR_TIME_W, FILEPLAY_BAR_PLAY_CUR_TIME_H,
                                   p_bar_cont, 0);
    ctrl_set_rstyle(p_play_time, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_play_time, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_play_time, FSI_FILEPLAY_BAR_INFO,
                        FSI_FILEPLAY_BAR_INFO, FSI_FILEPLAY_BAR_INFO);
    text_set_content_type(p_play_time, TEXT_STRTYPE_UNICODE);
    p_play_time = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_F_TIME,
                                   FILEPLAY_BAR_PLAY_CUR_TIME_X + FILEPLAY_BAR_PLAY_CUR_TIME_W, 
                                   FILEPLAY_BAR_PLAY_CUR_TIME_Y,
                                   8,
                                   FILEPLAY_BAR_PLAY_CUR_TIME_H,
                                   p_bar_cont, 0);
    ctrl_set_rstyle(p_play_time, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_play_time, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_play_time, FSI_FILEPLAY_BAR_INFO,
                        FSI_FILEPLAY_BAR_INFO, FSI_FILEPLAY_BAR_INFO);
    text_set_content_type(p_play_time, TEXT_STRTYPE_UNICODE);
    text_set_content_by_ascstr(p_play_time, "/");
    // Total time
    p_total_time = ctrl_create_ctrl(CTRL_TEXT, IDC_FILEPLAY_TOTAL_TIME,
                                    FILEPLAY_BAR_PLAY_CUR_TIME_X + 8 + FILEPLAY_BAR_PLAY_CUR_TIME_W, 
                                    FILEPLAY_BAR_PLAY_CUR_TIME_Y,
                                    FILEPLAY_BAR_PLAY_CUR_TIME_W, FILEPLAY_BAR_PLAY_CUR_TIME_H,
                                    p_bar_cont, 0);
    ctrl_set_rstyle(p_total_time, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_total_time, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_total_time, FSI_FILEPLAY_BAR_INFO,
                        FSI_FILEPLAY_BAR_INFO, FSI_FILEPLAY_BAR_INFO);
    text_set_content_type(p_total_time, TEXT_STRTYPE_UNICODE);

    //playing progress
    p_play_progress = ctrl_create_ctrl(CTRL_PBAR, IDC_FILEPLAY_PLAY_PROGRESS,
                                       FILEPLAY_BAR_PROGRESS_X, FILEPLAY_BAR_PROGRESS_Y,
                                       FILEPLAY_BAR_PROGRESS_W, FILEPLAY_BAR_PROGRESS_H,
                                       p_bar_cont, 0);
    ctrl_set_rstyle(p_play_progress, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    ctrl_set_mrect(p_play_progress,
                   FILEPLAY_BAR_PROGRESS_MIDX, FILEPLAY_BAR_PROGRESS_MIDY,
                   FILEPLAY_BAR_PROGRESS_MIDW, FILEPLAY_BAR_PROGRESS_MIDH);
    
    pbar_set_rstyle(p_play_progress, FILEPLAY_BAR_PROGRESS_MIN, FILEPLAY_BAR_PROGRESS_MAX, FILEPLAY_BAR_PROGRESS_MID);
    pbar_set_count(p_play_progress, 0, FILEPLAY_BAR_PROGRESS_STEP, FILEPLAY_BAR_PROGRESS_STEP);
    pbar_set_direction(p_play_progress, 1);
    pbar_set_workmode(p_play_progress, TRUE, 0);
    pbar_set_current(p_play_progress, 0);

    //seek point
    p_trick = ctrl_create_ctrl(CTRL_BMAP, IDC_FILEPLAY_SEEK,
                               FILEPLAY_TRICK_X, FILEPLAY_TRICK_Y, 
                               FILEPLAY_TRICK_W, FILEPLAY_TRICK_H, 
                               p_bar_cont, 0);
    
    //ctrl_set_sts(p_trick, OBJ_STS_HIDE);
    bmap_set_content_by_id(p_trick, IM_ICONS_PAUSE2);

   p_sort_list = ctrl_create_ctrl(CTRL_LIST, IDC_FILEPLAY_SORT_LIST,
                            FILEPLAY_SORT_LIST_X, FILEPLAY_SORT_LIST_Y,
                            FILEPLAY_SORT_LIST_W, FILEPLAY_SORT_LIST_H,
                            p_mainwin,
                            0);
  ctrl_set_rstyle(p_sort_list, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG);//RSI_COMMAN_BG
  ctrl_set_keymap(p_sort_list, video_f_sort_list_keymap);
  ctrl_set_proc(p_sort_list, video_f_sort_list_proc);
  ctrl_set_mrect(p_sort_list, FILEPLAY_SORT_LIST_MIDL, FILEPLAY_SORT_LIST_MIDT,
                    FILEPLAY_SORT_LIST_MIDW+FILEPLAY_SORT_LIST_MIDL, FILEPLAY_SORT_LIST_MIDH+FILEPLAY_SORT_LIST_MIDT);
  list_set_item_interval(p_sort_list, 0);
  list_set_item_rstyle(p_sort_list, &fileplay_list_field_rstyle);
  list_set_count(p_sort_list, FILEPLAY_SORT_LIST_ITEM_TOL, FILEPLAY_SORT_LIST_ITEM_PAGE);
  list_set_field_count(p_sort_list, FILEPLAY_SORT_LIST_FIELD_NUM, FILEPLAY_SORT_LIST_ITEM_PAGE);
  list_set_focus_pos(p_sort_list, 0);
  list_set_update(p_sort_list, fileplay_sort_list_update, 0);

  for (i = 0; i < FILEPLAY_SORT_LIST_FIELD_NUM; i++)
  {
    list_set_field_attr(p_sort_list, (u8)i,
                        (u32)(fileplay_sort_list_field_attr[i].attr),
                        (u16)(fileplay_sort_list_field_attr[i].width),
                        (u16)(fileplay_sort_list_field_attr[i].left),
                        (u8)(fileplay_sort_list_field_attr[i].top));
   // list_set_field_rect_style(p_sort_list, (u8)i, fileplay_sort_list_field_attr[i].rstyle);
    list_set_field_font_style(p_sort_list, (u8)i, fileplay_sort_list_field_attr[i].fstyle);
  }
  fileplay_sort_list_update(p_sort_list, list_get_valid_pos(p_sort_list), FILEPLAY_SORT_LIST_ITEM_PAGE, 0);
  //ctrl_set_attr(p_sort_list, OBJ_ATTR_HIDDEN);
  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);

    //help
    //ui_comm_help_create(&help_data, p_bar_cont);
    //ui_comm_help_move_pos(p_bar_cont, 850, -20, 300, 20, 40);


    ctrl_process_msg(p_play_progress, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_mainwin), FALSE);

    //ctrl_paint_ctrl(p_mainwin, FALSE);

    //update all the ctrls
    _ui_video_v_fscreen_up_all();

    _ui_video_v_fscreen_create_timer(TRUE);

    _ui_video_v_fscreen_set_pn_change(TRUE);


    manage_open_menu(ROOT_ID_FUN_HELP, 0, 0);
    play_letter = para2;
    return SUCCESS;
}

/************************************************
                        show bps and downloading percent
************************************************/
void ui_fileplay_get_bps_infos(u32 param, u16 *percent, u16 *bps)
{
  *percent = param >> 16;  //high 16bit saved percent
  *bps = param & 0xffff;  //low 16 bit saved bps
}

static void fileplay_update_bps(control_t* p_bps, control_t *p_per, u16 cur_bps, u16 load_percent)
{
  u8 bps_asc[32] = {"Downloading Speed:"};
  u8 bps_string[8] = {"KB/s"};
  u8 load_asc[32] = {"Loading Percent:"};
  
  sprintf(bps_asc, "%s%d%s", bps_asc, cur_bps, bps_string);
  text_set_content_by_ascstr(p_bps, bps_asc);

  sprintf(load_asc, "%s%d%s", load_asc, load_percent, "\%");
  text_set_content_by_ascstr(p_per, load_asc);
  OS_PRINTF("##%s, bps = %s, load percent = %s##\n", __FUNCTION__, bps_asc, load_asc);
}

static RET_CODE on_fileplay_show_bps(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u16 net_bps = 0;
  u16 percent = 0;
  control_t *p_bps_cont_cont = NULL;
  control_t *p_bps_cont = NULL;
  control_t *p_bps = NULL;
  control_t *p_percent = NULL;
  u8 play_func = 0;

  play_func = ui_video_c_get_play_func();

  if((ROOT_ID_FILEPLAY_BAR != fw_get_focus_id()) || (play_func == VIDEO_PLAY_FUNC_USB))
  {
    return SUCCESS;
  }

  p_bps_cont_cont = ui_comm_root_get_ctrl(ROOT_ID_FILEPLAY_BAR, IDC_FILEPLAY_BPS_CONT_CONT);
  p_bps_cont = ctrl_get_child_by_id(p_bps_cont_cont, IDC_FILEPLAY_BPS_CONT);
  p_bps = ctrl_get_child_by_id(p_bps_cont, IDC_FILEPLAY_BPS);
  p_percent = ctrl_get_child_by_id(p_bps_cont, IDC_FILEPLAY_BUF_PERCENT);
  ui_fileplay_get_bps_infos(para1, &percent, &net_bps);
  OS_PRINTF("##%s, para1 = %d, net_bps = %d, load percent = %d##\n", __FUNCTION__, para1, net_bps, percent);

  if(ctrl_get_sts(p_bps_cont) == OBJ_STS_HIDE)
  {
    ctrl_set_sts(p_bps_cont, OBJ_STS_SHOW);
    fileplay_update_bps(p_bps, p_percent, net_bps, percent);
    ctrl_paint_ctrl(p_bps_cont, TRUE);
  }
  else
  {
    fileplay_update_bps(p_bps, p_percent, net_bps, percent);
    ctrl_paint_ctrl(p_bps_cont, TRUE);
  }
  return SUCCESS;
}

static RET_CODE on_fileplay_cacel_bps(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_bps_cont_cont = NULL;
  control_t *p_bps_cont = NULL;
 OS_PRINTF("@@@@@@@@@##%s##@@@@@@@@@@@##\n", __FUNCTION__);
  p_bps_cont_cont = ui_comm_root_get_ctrl(ROOT_ID_FILEPLAY_BAR, IDC_FILEPLAY_BPS_CONT_CONT);
  p_bps_cont = ctrl_get_child_by_id(p_bps_cont_cont, IDC_FILEPLAY_BPS_CONT);
  ctrl_set_sts(p_bps_cont, OBJ_STS_HIDE);
  ctrl_paint_ctrl(p_bps_cont_cont, TRUE);

  
  return SUCCESS;
}

/************************************************
                        show time while loading media
                        
************************************************/
static void fileplay_update_loading_media_time(control_t* p_ctrl, u32 second)
{
  u8 *str_text[2] = 
  {
    "Loading time: ",
    "Hard Loading, please wait, Loading time: ",
  };
  u8 asc_str[128] = {0};

  OS_PRINTF("##%s, second = %d##\n", __FUNCTION__, second);
  if(second <= 5)
  {
    sprintf(asc_str, "%s %d %s", str_text[0], (int)second, " s");
  }
  else if(second > 5)
  {
    sprintf(asc_str, "%s %d %s", str_text[1], (int)second, " s");
  }  
  text_set_content_by_ascstr(p_ctrl, asc_str);
}

static RET_CODE on_fileplay_show_loading_media_time(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont_cont = NULL;
  control_t *p_conts = NULL;
  control_t *p_ctrl = NULL;
  u8 play_func = 0;
 
  play_func = ui_video_c_get_play_func();
 
  if((ROOT_ID_FILEPLAY_BAR != fw_get_focus_id()) || (play_func == VIDEO_PLAY_FUNC_USB))
  {
    return SUCCESS;
  }
  
  p_cont_cont = ui_comm_root_get_ctrl(ROOT_ID_FILEPLAY_BAR, IDC_FILEPLAY_LOADING_MEDIA_TIME_CONT_CONT);
  p_conts = ctrl_get_child_by_id(p_cont_cont, IDC_FILEPLAY_LOADING_MEDIA_TIME_CONT);
  p_ctrl =ctrl_get_child_by_id(p_conts, IDC_FILEPLAY_LOADING_MEDIA_TIME);
  if(ctrl_get_sts(p_ctrl) == OBJ_STS_HIDE)
  {
    ctrl_set_sts(p_ctrl, OBJ_STS_SHOW);
    fileplay_update_loading_media_time(p_ctrl, para1);
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
  else
  {
    fileplay_update_loading_media_time(p_ctrl, para1);
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
  return SUCCESS;
}

static RET_CODE on_fileplay_cacel_show_loading_media_time(void)
{
  control_t *p_cont_cont = NULL;  
  control_t *p_conts = NULL;
  control_t *p_ctrl = NULL;
  
  p_cont_cont = ui_comm_root_get_ctrl(ROOT_ID_FILEPLAY_BAR, IDC_FILEPLAY_LOADING_MEDIA_TIME_CONT_CONT);
  p_conts = ctrl_get_child_by_id(p_cont_cont, IDC_FILEPLAY_LOADING_MEDIA_TIME_CONT);
  p_ctrl =ctrl_get_child_by_id(p_conts, IDC_FILEPLAY_LOADING_MEDIA_TIME);
  ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
  ctrl_paint_ctrl(p_conts, TRUE);
  return SUCCESS;
}

static RET_CODE _ui_video_f_list_change_focus(control_t *p_bar_cont, 
                                               u16 msg,
                                               u32 para1, 
                                               u32 para2)
{ 
    control_t  *p_old,*p_next;
    control_t *p_seek= NULL,  *p_active = NULL;

    if(OBJ_STS_HIDE == ctrl_get_sts(p_bar_cont))
    {
        _ui_video_v_fscreen_show_bar();
        return SUCCESS;
    } 

    p_active = ctrl_get_active_ctrl(p_bar_cont);
    if(ctrl_get_ctrl_id(p_active)==IDC_FILEPLAY_PLAY_PROGRESS)
    {
	    if(g_video_full_screen.enable_seek)
	    {

		mul_fp_play_state_t cur_state = MUL_PLAY_STATE_NONE;

		cur_state = (mul_fp_play_state_t)ui_video_c_get_play_state();
		if(MUL_PLAY_STATE_PAUSE == cur_state)
		{
			ui_video_c_pause_resume();
		}

		g_video_full_screen.seek_sec = 0;
		g_video_full_screen.enable_seek = FALSE;
		_ui_video_v_fscreen_up_all_icon();
	    }
	
            p_seek = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_SEEK);
            ctrl_set_sts(p_seek, OBJ_STS_HIDE);
            p_old = ctrl_get_child_by_id(p_bar_cont,IDC_FILEPLAY_PLAY_PROGRESS);
            p_next = ctrl_get_child_by_id(p_bar_cont,IDC_FILEPLAY_FB_STAT_ICON);
            pbar_set_rstyle(p_old, RSI_PROGRESS_BAR_MID_YELLOW, FILEPLAY_BAR_PROGRESS_MAX, FILEPLAY_BAR_PROGRESS_MID);
            mbox_get_focus(p_next);
            ctrl_process_msg(p_old, MSG_LOSTFOCUS, 0, 0); 
            ctrl_process_msg(p_next, MSG_GETFOCUS, 0, 0); 
            mbox_set_focus(p_next, mbox_index);
    }
    else
    {
            p_seek = ctrl_get_child_by_id(p_bar_cont, IDC_FILEPLAY_SEEK);
            _ui_video_v_fscreen_up_process_bar();
            ctrl_set_sts(p_seek, OBJ_STS_SHOW);
            p_next = ctrl_get_child_by_id(p_bar_cont,IDC_FILEPLAY_PLAY_PROGRESS);
            p_old =  ctrl_get_child_by_id(p_bar_cont,IDC_FILEPLAY_FB_STAT_ICON);
            pbar_set_rstyle(p_next, RSI_PROGRESS_BAR_MID_SKY_BLUE, FILEPLAY_BAR_PROGRESS_MAX, FILEPLAY_BAR_PROGRESS_MID);
              ctrl_process_msg(p_old, MSG_LOSTFOCUS, 0, 0); 
            ctrl_process_msg(p_next, MSG_GETFOCUS, 0, 0); 
    }

    ctrl_paint_ctrl(p_next->p_parent, TRUE);
    ctrl_paint_ctrl(p_old,TRUE);
    ctrl_paint_ctrl(p_next,TRUE);
    return SUCCESS;
}
static RET_CODE _ui_video_f_change(control_t *p_bar_cont, 
                                               u16 msg,
                                               u32 para1, 
                                               u32 para2)
{
    control_t  *p_active = NULL ;
	
    if(OBJ_STS_SHOW != ctrl_get_sts(p_bar_cont))
    {
        _ui_video_v_fscreen_show_bar();
        return SUCCESS;
    }

    p_active = ctrl_get_active_ctrl(p_bar_cont);
    if(IDC_FILEPLAY_FB_STAT_ICON == ctrl_get_ctrl_id(p_active))
    {
         switch(mbox_get_focus(p_active))
         {
             case 0:
                 ui_video_c_fast_back();
                 _ui_video_v_fscreen_up_speed_play_cont();
             break;
             case 3:
                 ui_video_c_stop();
                 _ui_video_v_set_full_stop_states(TRUE);
                 _ui_video_v_fscreen_up_all_icon();
                 _ui_video_v_fscreen_destroy();
             break;
             case 2:
                 _ui_video_v_fscreen_play_pause(p_bar_cont,msg,para1,para2);
             break;
             case 1:    
                 _ui_video_v_fscreen_process_prev(p_bar_cont,msg,para1,para2);
                 _ui_video_v_fscreen_up_select_icon();
             break;
             case 4:
                 _ui_video_v_fscreen_process_next(p_bar_cont,msg,para1,para2);
                 _ui_video_v_fscreen_up_select_icon();
             break;
     #ifdef ENABLE_TRICK_PLAY    
             case 5:
                 ui_video_c_fast_play();
                 _ui_video_v_fscreen_up_speed_play_cont();
             break;
     #endif
             default:
             break;
         }
         return SUCCESS;
    }
    _ui_video_v_fscreen_confirm(p_bar_cont, msg,0,0);
    return SUCCESS;
}

static RET_CODE on_f_sort_list_select(control_t *p_sort_list, u16 msg, u32 para1,
                               u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_cont;
  control_t *p_con_bar;
  control_t *p_speed_play;
  u16 pos;

  p_cont = p_sort_list->p_parent;
  p_con_bar = ctrl_get_child_by_id(p_cont, IDC_FILEPLAY_BAR_CONT);
  p_speed_play= ctrl_get_child_by_id(p_con_bar, IDC_FILEPLAY_PLAY_PROGRESS );
  ctrl_process_msg(p_sort_list, MSG_LOSTFOCUS, para1, para2);

  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);

  ctrl_paint_ctrl(p_cont, TRUE);

  pos = list_get_focus_pos(p_sort_list);
  switch(pos)
  {
    case 1:
        _ui_video_v_fscreen_show_goto(p_con_bar,0,0,0);
        ctrl_process_msg(p_speed_play, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_speed_play, TRUE);
        return SUCCESS;
    case 0:
        _ui_video_v_fscreen_open_text_encode(p_con_bar,0,0,0);
        ctrl_process_msg(p_speed_play, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_speed_play, TRUE);
        return SUCCESS;
    default:
        break;
  }
  return ret;
}

static RET_CODE on_video_f_sort_list_exit(control_t *p_sort_list, u16 msg, u32 para1,
                               u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_cont;
  control_t *p_con_bar;
  control_t *p_speed_play;

  p_cont = p_sort_list->p_parent;
  p_con_bar = ctrl_get_child_by_id(p_cont, IDC_FILEPLAY_BAR_CONT);
  p_speed_play= ctrl_get_child_by_id(p_con_bar, IDC_FILEPLAY_PLAY_PROGRESS );

  ctrl_process_msg(p_sort_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_speed_play, MSG_GETFOCUS, 0, 0);

  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);
  ctrl_paint_ctrl(p_cont, TRUE);

  return ret;
}

static RET_CODE on_video_f_list_sort(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  rect_t p_frame;
  control_t *p_sort_list;
  s16 pos=0;
  
  pos = 4;
  p_sort_list = ctrl_get_child_by_id(p_list, IDC_FILEPLAY_SORT_LIST);
  ctrl_set_attr(p_sort_list, OBJ_ATTR_ACTIVE);
  p_frame.top = pos * 50;
  p_frame.bottom= pos * 50 + FILEPLAY_SORT_LIST_H;
  p_frame.left= FILEPLAY_SORT_LIST_X;
  p_frame.right= FILEPLAY_SORT_LIST_X + FILEPLAY_SORT_LIST_W;
  ctrl_resize(p_sort_list, &p_frame);
  ctrl_set_sts(p_sort_list, OBJ_STS_SHOW);

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_sort_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_sort_list, TRUE);

  return ret;
}


BEGIN_KEYMAP(_ui_video_f_txt_keymap, NULL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(_ui_video_f_txt_keymap, NULL)
BEGIN_MSGPROC(_ui_video_f_txt_proc, cont_class_proc)
    ON_COMMAND(MSG_FOCUS_LEFT, _ui_video_v_fscreen_left_right)
    ON_COMMAND(MSG_FOCUS_RIGHT, _ui_video_v_fscreen_left_right)
    ON_COMMAND(MSG_FOCUS_UP, _ui_video_f_list_change_focus)
    ON_COMMAND(MSG_FOCUS_DOWN, _ui_video_f_list_change_focus)
    ON_COMMAND(MSG_SELECT, _ui_video_f_change)
END_MSGPROC(_ui_video_f_txt_proc, cont_class_proc)

BEGIN_KEYMAP(fileplay_bar_mainwin_keymap, NULL)
  ON_EVENT(V_KEY_VUP, MSG_INCREASE)
  ON_EVENT(V_KEY_VDOWN, MSG_DECREASE)
  ON_EVENT(V_KEY_INFO, MSG_FP_INFO)
  ON_EVENT(V_KEY_OK, MSG_FP_OK)
  //ON_EVENT(V_KEY_GREEN, MSG_GREEN)
  ON_EVENT(V_KEY_AUDIO, MSG_FP_AUDIO_SET)
  ON_EVENT(V_KEY_HOT_XEXTEND, MSG_SHOW_GOTO)
  ON_EVENT(V_KEY_SUBT, MSG_FP_PLAY_SUBT)
  ON_EVENT(V_KEY_MUTE, MSG_MUTE)  
    
  //ON_EVENT(V_KEY_PAUSE, MSG_FP_PLAY_PAUSE)
  //ON_EVENT(V_KEY_PLAY, MSG_FP_PLAY_PAUSE)
  //ON_EVENT(V_KEY_NEXT, MSG_FP_PLAY_NEXT)
  //ON_EVENT(V_KEY_PREV, MSG_FP_PLAY_PREV)  
//#ifdef ENABLE_TRICK_PLAY
 // ON_EVENT(V_KEY_FORW, MSG_FP_PLAY_FF)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_FP_PLAY_NEXT)
  ON_EVENT(V_KEY_PAGE_UP, MSG_FP_PLAY_PREV)  
  ON_EVENT(V_KEY_BACK, MSG_FP_EXIT)
//#endif
  ON_EVENT(V_KEY_STOP, MSG_FP_PLAY_STOP)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)  

  ON_EVENT(V_KEY_CANCEL, MSG_FP_HIDE_BAR)
  ON_EVENT(V_KEY_MENU, MSG_FP_EXIT)
  //ON_EVENT(V_KEY_YELLOW, MSG_FP_EXIT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(fileplay_bar_mainwin_keymap, NULL)

BEGIN_MSGPROC(fileplay_bar_mainwin_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DECREASE, _ui_video_v_fscreen_adjust_volume)
  ON_COMMAND(MSG_INCREASE, _ui_video_v_fscreen_adjust_volume)
  ON_COMMAND(MSG_MUTE, _ui_video_v_fscreen_on_mute)
  //ON_COMMAND(MSG_GREEN, _ui_video_v_fscreen_open_text_encode)
  ON_COMMAND(MSG_FP_INFO, _ui_video_v_fscreen_on_show_bar)
  ON_COMMAND(MSG_SHOW_GOTO, on_video_f_list_sort)
  ON_COMMAND(MSG_FP_HIDE_BAR, _ui_video_v_fscreen_on_hide_bar)
  ON_COMMAND(MSG_FP_EXIT, _ui_video_v_fscreen_exit)
  ON_COMMAND(MSG_FP_PLAY_PAUSE, _ui_video_v_fscreen_play_pause)
  ON_COMMAND(MSG_FP_PLAY_PREV, _ui_video_v_fscreen_process_prev)
  ON_COMMAND(MSG_FP_PLAY_NEXT, _ui_video_v_fscreen_process_next)
#ifdef ENABLE_TRICK_PLAY
  ON_COMMAND(MSG_FP_PLAY_FF, _ui_video_v_fscreen_trick_play)
  ON_COMMAND(MSG_FP_PLAY_FB, _ui_video_v_fscreen_trick_play)
#endif
  ON_COMMAND(MSG_FP_PLAY_STOP, _ui_video_v_fscreen_stop)
  ON_COMMAND(MSG_FP_OK, _ui_video_v_fscreen_confirm)
  ON_COMMAND(MSG_FP_PLAY_SUBT, _ui_video_v_fscreen_show_subt)
  ON_COMMAND(MSG_FP_AUDIO_SET, _ui_video_v_fscreen_show_audio_set)
  
  /*------Update Msg------*/
  ON_COMMAND(MSG_PLUG_OUT, _ui_video_v_fscreen_plug_out)
  ON_COMMAND(MSG_VIDEO_EVENT_UP_TIME, _ui_video_v_fscreen_up_time)
  //ON_COMMAND(MSG_VIDEO_EVENT_STOP_CFM, _ui_video_v_fscreen_on_stop_cfm)
  ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, _ui_video_v_fscreen_on_load_media_success)
  ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR, _ui_video_v_fscreen_on_load_media_error)
  ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT, _ui_video_v_fscreen_on_load_media_error)
  ON_COMMAND(MSG_VIDEO_EVENT_EOS, _ui_video_v_fscreen_on_eos)
  ON_COMMAND(MSG_VIDEO_EVENT_UNSUPPORTED_MEMORY, _ui_video_v_fscreen_unsupported_video)
  ON_COMMAND(MSG_FP_DISAPPEAR_UNSUPPORT, _ui_video_v_fscreen_unsupported_timeout)
  ON_COMMAND(MSG_VIDEO_EVENT_TRICK_TO_BEGIN, _ui_video_v_fscreen_trick_to_begin)
  ON_COMMAND(MSG_VIDEO_EVENT_UPDATE_BPS, on_fileplay_show_bps)
  ON_COMMAND(MSG_VIDEO_EVENT_FINISH_BUFFERING, on_fileplay_cacel_bps)
  ON_COMMAND(MSG_VIDEO_EVENT_FINISH_UPDATE_BPS, on_fileplay_cacel_bps)
  ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_TIME, on_fileplay_show_loading_media_time)
  //ON_COMMAND(MSG_VIDEO_EVENT_UNSUPPORT_SEEK, _ui_video_v_fscreen_unsupport_seek)
END_MSGPROC(fileplay_bar_mainwin_proc, ui_comm_root_proc)

BEGIN_KEYMAP(video_f_sort_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_HOT_XEXTEND, MSG_EXIT)
END_KEYMAP(video_f_sort_list_keymap, NULL)

BEGIN_MSGPROC(video_f_sort_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_f_sort_list_select)
  ON_COMMAND(MSG_EXIT, on_video_f_sort_list_exit)
END_MSGPROC(video_f_sort_list_proc, list_class_proc)
