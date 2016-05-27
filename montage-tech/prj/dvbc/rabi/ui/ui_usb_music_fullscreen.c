/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_xinfo.h"
#include "data_manager.h"
#include "data_manager_v2.h"
#include "ui_usb_music.h"
#include "ui_volume_usb.h"
#include "ui_text_encode.h"
#include "ui_mute.h"

#define RSI_MP_PREV  RSI_TV
#define IDC_MUSIC_LOGO  1

enum info_ctrl_id
{
  IDC_MUSIC_FULL_SCREEN_LIST = 1,
  IDC_MUSIC_FULL_SCREEN_PICTURE ,
  IDC_MUSIC_FULL_SCREEN_LRC ,
};

enum mp_local_msg
{
  MSG_REC_FULL_NONE = MSG_LOCAL_BEGIN + 100,
  MSG_VOLUME_UP,
  MSG_VOLUME_DOWN,
  MSG_GREEN,
  MSG_MUSIC_FULL_PAUSE,
  MSG_MUSIC_FULL_RESUME,
};

/*static comm_help_data_t help_data =
{
    1,1,
    {IDS_TEXT_ENCODE},
    {IM_HELP_GREEN}
};*/

static BOOL g_fullscreen_play = FALSE;
static u16 play_letter = 0;
extern s32 get_music_is_usb();
u16 music_fullscreen_keymap(u16 key);


RET_CODE music_fullscreen_cont_proc(control_t *p_cont, u16 msg,
  u32 para1, u32 para2);

static void music_exit_fullscreen()
{
/*  rect_t logo_rect =  {REC_MANAGER_MUSIC_PREV_X, 
                       REC_MANAGER_MUSIC_PREV_Y,
                       REC_MANAGER_MUSIC_PREV_W,
                       REC_MANAGER_MUSIC_PREV_H};


  ui_music_logo_show(TRUE,logo_rect);*/
  OS_PRINTF("@@@%s  line=%d  tick=%d\n", __FUNCTION__,__LINE__,mtos_ticks_get());
ui_music_mod_player();
OS_PRINTF("@@@%s  line=%d  tick=%d\n", __FUNCTION__,__LINE__,mtos_ticks_get());
}

static RET_CODE on_music_fullscreen_destroy(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t  *p_root = NULL;

  ui_music_logo_clear(0);
  
  music_exit_fullscreen();

  if(ui_is_mute())
  {
    open_mute(0, 0);
  }
  g_fullscreen_play = FALSE;

  p_root = fw_find_root_by_id(ROOT_ID_USB_MUSIC);
  if (p_root != NULL)
  {
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
  }

  return ERR_NOFEATURE;
}

static RET_CODE on_music_fullscreen_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_USB_MUSIC_FULLSCREEN, 0, 0);
  manage_open_menu(ROOT_ID_FUN_HELP, 0, 0);
  return SUCCESS;
}


RET_CODE open_usb_music_fullscreen(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_cont_picture, *p_cont_lrc;//, *p_prev_logo;
//  control_t *p_con_backgroup;
  language_set_t lang_set = {0};
  
  p_cont =
    ui_comm_root_create(ROOT_ID_USB_MUSIC_FULLSCREEN, 0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
    COMM_BG_H, 0,0 );

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  p_cont_picture = ctrl_create_ctrl(CTRL_CONT, IDC_MUSIC_FULL_SCREEN_PICTURE,
  MUSIC_FULL_PICTURE_X, MUSIC_FULL_PICTURE_Y, MUSIC_FULL_PICTURE_W, MUSIC_FULL_PICTURE_H, p_cont, 0);
  ctrl_set_rstyle(p_cont_picture, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

    //preview logo
/*  p_prev_logo = ctrl_create_ctrl(CTRL_BMAP, IDC_MUSIC_LOGO,
                            MUSIC_FULL_LOGO_X,
                            MUSIC_FULL_LOGO_Y,
                            MUSIC_FULL_LOGO_W,
                            MUSIC_FULL_LOGO_H,
                            p_cont_picture, 0);
  bmap_set_align_type(p_prev_logo, STL_CENTER|STL_VCENTER);
  bmap_set_content_by_id(p_prev_logo, IM_MP3_LOGO_MUSIC_SMALL);
*/
  p_cont_lrc = ctrl_create_ctrl(CTRL_CONT, IDC_MUSIC_FULL_SCREEN_LRC,
  MUSIC_FULL_LRC_X, MUSIC_FULL_LRC_Y, MUSIC_FULL_LRC_W, MUSIC_FULL_LRC_H, p_cont, 0);
  ctrl_set_rstyle(p_cont_lrc, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

//  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_MUSIC_FULL_SCREEN_LIST,
//  MUSIC_FULL_SCREEN_LIST_X, MUSIC_FULL_SCREEN_LIST_Y, MUSIC_FULL_SCREEN_LIST_W, MUSIC_FULL_SCREEN_LIST_H, p_cont, 0);
//  ctrl_set_rstyle(p_list, RSI_MUSIC_FULL_SCREEN_LIST, RSI_MUSIC_FULL_SCREEN_LIST, RSI_MUSIC_FULL_SCREEN_LIST);

  ctrl_set_keymap(p_cont, music_fullscreen_keymap);
  ctrl_set_proc(p_cont, music_fullscreen_cont_proc);

/*  ui_comm_help_create(&help_data, p_cont);

  ui_comm_help_move_pos(p_cont, 750, -20, 190, 60, 42);*/

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
  OS_PRINTF("####OPEN  ROOT_ID_FUN_HELP#####\n");
  manage_open_menu(ROOT_ID_FUN_HELP, 0, 0);
  g_fullscreen_play = TRUE;
  if(para2 != 0)
  {
     play_letter = para2;
  }
  sys_status_get_lang_set(&lang_set);
  ui_music_set_charset(lang_set.text_encode_music);
  return SUCCESS;
}


BOOL ui_music_is_fullscreen_play()
{
   return g_fullscreen_play;

}

void ui_music_set_fullscreen_play(BOOL fullscreen_play)
{
    g_fullscreen_play = fullscreen_play;
}

static RET_CODE on_usb_rec_none(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  return SUCCESS;
}

static RET_CODE on_music_fullscreen_change_volume(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  open_volume_usb(ROOT_ID_USB_MUSIC_FULLSCREEN, para1);
  return SUCCESS;
}

static RET_CODE on_usb_music_full_screen_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  u32 usb_dev_type = para2;
  if(usb_dev_type == HP_WIFI || !get_music_is_usb())
  {
    OS_PRINTF("####wifi device  plug out or samba plug out usb#####\n");
    return ERR_FAILURE;
  }

  if(ui_usb_get_cur_used(play_letter) != ui_usb_get_plug_partition_sev())
  {
    return SUCCESS;
  }
	
  manage_close_menu(ROOT_ID_USB_MUSIC_FULLSCREEN, 0, 0);
  #if 0
  music_exit_fullscreen();

  g_fullscreen_play = FALSE;

  fw_notify_parent(ROOT_ID_USB_MUSIC_FULLSCREEN, NOTIFY_T_MSG, FALSE,
          MSG_PLUG_OUT, 0, 0);
  #endif
  return SUCCESS;
}

static RET_CODE on_usb_music_full_screen_open_text_encode(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
    manage_open_menu(ROOT_ID_TEXT_ENCODE, 0, 0);
    return SUCCESS;
}

static RET_CODE on_usb_music_full_status(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
    music_play_state_t play_status;

    play_status = ui_music_get_play_status();
  
    if(play_status == MUSIC_STAT_PAUSE)
    {
      music_player_resume();
      ui_music_set_play_status(MUSIC_STAT_PLAY);
    }
    else if(play_status == MUSIC_STAT_PLAY)
    {
      music_player_pause();
      ui_music_set_play_status(MUSIC_STAT_PAUSE);
    }
    return SUCCESS;
}

static RET_CODE on_usb_music_full_play_music(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
    media_fav_t *p_media = NULL;
    OS_PRINTF("\n##debug: on_usb_music_full_play_music!\n");

    ui_set_mute(ui_is_mute());
    if(MSG_FOCUS_UP == msg){
        if(ui_music_get_play_status() == MUSIC_STAT_PLAY)
        {
            if(ui_music_pre_play_pre(&p_media))
            {
                music_player_next(p_media->path);
                ui_music_reset_curn(p_media->path);
            }
        }
    }else if(MSG_FOCUS_DOWN== msg){
        if(ui_music_get_play_status() == MUSIC_STAT_PLAY)
        {
            if(ui_music_pre_play_next(&p_media))
            {
                music_player_next(p_media->path);
                ui_music_reset_curn(p_media->path);
            }
        }
    }

    return SUCCESS;
}


BEGIN_KEYMAP(music_fullscreen_keymap, NULL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_REC,MSG_REC_FULL_NONE)
  ON_EVENT(V_KEY_LEFT, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_VUP, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_HOT_XEXTEND, MSG_GREEN)  
  ON_EVENT(V_KEY_PAUSE, MSG_MUSIC_FULL_PAUSE)  
  ON_EVENT(V_KEY_PLAY, MSG_MUSIC_FULL_RESUME)
  ON_EVENT(V_KEY_OK, MSG_MUSIC_FULL_PAUSE)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(music_fullscreen_keymap, NULL)

BEGIN_MSGPROC(music_fullscreen_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_VOLUME_DOWN, on_music_fullscreen_change_volume)
  ON_COMMAND(MSG_VOLUME_UP, on_music_fullscreen_change_volume)
  ON_COMMAND(MSG_EXIT, on_music_fullscreen_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_music_fullscreen_exit)
  ON_COMMAND(MSG_DESTROY, on_music_fullscreen_destroy)
  ON_COMMAND(MSG_REC_FULL_NONE,on_usb_rec_none)
  ON_COMMAND(MSG_PLUG_OUT, on_usb_music_full_screen_plug_out)
  ON_COMMAND(MSG_GREEN, on_usb_music_full_screen_open_text_encode)
  ON_COMMAND(MSG_MUSIC_FULL_PAUSE, on_usb_music_full_status)
  ON_COMMAND(MSG_MUSIC_FULL_RESUME, on_usb_music_full_status)
  ON_COMMAND(MSG_FOCUS_UP, on_usb_music_full_play_music)
  ON_COMMAND(MSG_FOCUS_DOWN, on_usb_music_full_play_music)
END_MSGPROC(music_fullscreen_cont_proc, ui_comm_root_proc)




