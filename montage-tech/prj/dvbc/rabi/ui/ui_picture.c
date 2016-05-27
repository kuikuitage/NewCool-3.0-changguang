/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_picture.h"
#include "file_list.h"
#include "ui_pic_play_mode_set.h"
#include "ui_usb_picture.h"
#include "ui_usb_music.h"
#include "ui_volume_usb.h"

enum picture_local_msg
{
  MSG_RED = MSG_LOCAL_BEGIN + 1050,
  MSG_GREEN,
  MSG_BLUE,
  MSG_YELLOW,
  MSG_PIC_PLAY,
  MSG_PIC_PAUSE,
  MSG_PREV_PIC,
  MSG_NEXT_PIC,
  MSG_CLOCK_WISE,
  MSG_ANTI_CLOCK,
  MSG_PIC_TIMER,
  MSG_MUSIC_NEXT,
  MSG_MUSIC_PRE,
  MSG_VOLUME_DOWN,
  MSG_VOLUME_UP,
};

enum picture_menu_ctrl_id
{
  IDC_PAUSE_CONT = 1,
};

enum picture_pause_cont_ctrl_id
{
  IDC_PAUSE = 1,
};

static rect_t g_rect = {0, PICTURE_PAUSE_H, SCREEN_WIDTH, SCREEN_HEIGHT};  //bug fixed move pic down
static BOOL g_slide_pause = FALSE;
static u32 g_is_fullscreen_pic_usb = 1;

typedef enum
{ 
    Play_By_StorageDev,
    Play_By_Url,
    Play_By_Dlna,
}play_mode_t;

static play_mode_t Mode = Play_By_StorageDev;
static u8 is_pic_loading = FALSE;
//static char p_url[512];

extern s32 get_picture_is_usb();
u16 picture_cont_keymap(u16 key);

RET_CODE picture_cont_proc(control_t *p_cont, u16 msg,
  u32 para1, u32 para2);

static void picture_tmr_create(void)
{
  BOOL is_slide_show = pic_play_get_slide_show_sts();
  
  u8 interval = pic_play_get_slide_interval();
  
  if(is_slide_show && interval)
  {
    fw_tmr_create(ROOT_ID_PICTURE, MSG_PIC_TIMER, interval * 1000, FALSE);
  }
}

static void picture_tmr_destory(void)
{
  fw_tmr_destroy(ROOT_ID_PICTURE, MSG_PIC_TIMER);
}

static void picture_rotate(control_t *p_cont, BOOL is_clock)
{
  control_t *p_pause = NULL, *p_pause_cont = NULL;

  p_pause_cont = ctrl_get_child_by_id(p_cont, IDC_PAUSE_CONT);
  p_pause = ctrl_get_child_by_id(p_pause_cont, IDC_PAUSE);

  if(ui_pic_is_rotate())
  {
    bmap_set_content_by_id(p_pause, is_clock ? IM_SWORD_R: IM_SWORD_L);
  }
  else
  {
    bmap_set_content_by_id(p_pause, IM_ICON_PAUSE);
  }
  g_slide_pause = TRUE;
  ctrl_set_attr(p_pause, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_pause, OBJ_STS_SHOW);
  ctrl_paint_ctrl(p_pause_cont, TRUE);
}

static void picture_pause(control_t *p_cont, BOOL is_pause)
{
  control_t *p_pause = NULL, *p_pause_cont = NULL;

  p_pause_cont = ctrl_get_child_by_id(p_cont, IDC_PAUSE_CONT);
  p_pause = ctrl_get_child_by_id(p_pause_cont, IDC_PAUSE);

  bmap_set_content_by_id(p_pause, IM_ICON_PAUSE);

  if(is_pause )
  {
    g_slide_pause = TRUE;
    ctrl_set_attr(p_pause, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_pause, OBJ_STS_SHOW);
    ctrl_paint_ctrl(p_pause_cont, TRUE);
  }
  else
  {
    g_slide_pause = FALSE;
    ctrl_set_sts(p_pause, OBJ_STS_HIDE);
    ctrl_paint_ctrl(p_pause_cont, TRUE);
  }
}


RET_CODE open_picture(u32 para1, u32 para2)
{
  control_t *p_cont = NULL, *p_pause_cont = NULL, *p_pause = NULL;
  media_fav_t *p_media = NULL;
  u8 root_id = ROOT_ID_USB_PICTURE;
  
  //pic_stop();
    
  ui_pic_clear(0);
  g_slide_pause = FALSE;
  Mode = (play_mode_t)para1;
  g_is_fullscreen_pic_usb = 1;

  if(fw_find_root_by_id(ROOT_ID_PICTURE))
  {
    is_pic_loading = TRUE;
    ui_comm_showdlg_open(NULL, IDS_LOADING_WITH_WAIT, NULL, 0);
    return SUCCESS;
  }
  /*Create Menu*/
  p_cont = fw_create_mainwin(ROOT_ID_PICTURE,
           PICTURE_CONT_X, PICTURE_CONT_Y,
           PICTURE_CONT_W, PICTURE_CONT_H,
           root_id, 0,
           OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, picture_cont_keymap);
  ctrl_set_proc(p_cont, picture_cont_proc);

  p_pause_cont = ctrl_create_ctrl(CTRL_BMAP, IDC_PAUSE_CONT,
    PICTURE_PAUSE_X, PICTURE_PAUSE_Y, PICTURE_PAUSE_W, PICTURE_PAUSE_H, p_cont, 0);
  ctrl_set_rstyle(p_pause_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

  p_pause = ctrl_create_ctrl(CTRL_BMAP, IDC_PAUSE,
    0, 0, PICTURE_PAUSE_W, PICTURE_PAUSE_H, p_pause_cont, 0);
  //bmap_set_content_by_id(p_pause, IM_PAUSE);
  ctrl_set_sts(p_pause, OBJ_STS_HIDE);

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  ui_pic_play_curn(&p_media, &g_rect);

  return SUCCESS;
}

static RET_CODE on_picture_pause(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{

  if(g_slide_pause)
  {
    picture_tmr_destory();
    picture_pause(p_cont, FALSE);
    pic_resume();
    picture_tmr_create();
  }
  else
  {
    picture_tmr_destory();
    picture_pause(p_cont, TRUE);
    pic_pause();
  }

  return SUCCESS;
}

static RET_CODE on_picture_resume(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
    picture_tmr_destory();
    picture_pause(p_cont, FALSE);
    pic_resume();

    return SUCCESS;
}

static RET_CODE on_picture_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  pic_stop();
  ui_pic_clear(0);//clear picture buffer
  manage_open_menu(ROOT_ID_FUN_HELP, 0, 0);
  return ERR_NOFEATURE;
}

static RET_CODE on_picture_destory(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_root = NULL;
  
  pic_reset_flip();

  p_root = fw_find_root_by_id(ROOT_ID_USB_PICTURE);
  ui_pic_clear(0);
  if(p_root != NULL)
  {
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
  }
  return ERR_NOFEATURE;
}

static RET_CODE on_picture_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  u32 usb_dev_type = para2;
  if(usb_dev_type == HP_WIFI || !get_picture_is_usb() || g_is_fullscreen_pic_usb)
  {
    OS_PRINTF("####wifi device  plug out or samba plug out usb#####\n");
    return ERR_FAILURE;
  }
  ui_pic_clear(0);
  manage_close_menu(ROOT_ID_PICTURE, 0, 0);

  return SUCCESS;
}

static RET_CODE on_picture_prev_pic(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;
  if(g_slide_pause)
  {
    return SUCCESS;
  }

  picture_tmr_destory();
  pic_stop();
  ui_pic_play_prev(&p_media, &g_rect);

  return ret;
}

static RET_CODE on_picture_next_pic(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;
  if(g_slide_pause)
  {
    return SUCCESS;
  }
  
  picture_tmr_destory();
  pic_stop();
  if(!ui_pic_play_next(&p_media, &g_rect))
  {
    ui_pic_clear(0);
    manage_close_menu(ROOT_ID_PICTURE, 0, 0);
  }
  return ret;
}

static RET_CODE on_picture_clock_wise(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;

  picture_tmr_destory();
  //picture_pause(p_cont, TRUE);
  pic_stop();
  ui_pic_play_clock(&p_media, &g_rect, TRUE);
  picture_rotate(p_cont, TRUE);
  return ret;
}

static RET_CODE on_picture_anti_clock(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;
  picture_tmr_destory();
  //picture_pause(p_cont, TRUE);
  pic_stop();
  ui_pic_play_clock(&p_media, &g_rect, FALSE);
  picture_rotate(p_cont, FALSE);

  return ret;
}


static RET_CODE on_picture_draw_end(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  //media_fav_t *p_media = NULL;
  u8 interval = pic_play_get_slide_interval();

  pic_stop();

  if(g_slide_pause)
  {
    return SUCCESS;
  }
  
  if(interval != 0)
  {
    picture_tmr_create();
  }
  else
  {
    //if(!ui_pic_play_next(&p_media, &g_rect))
    //{
    //  manage_close_menu(ROOT_ID_PICTURE, 0, 0);
    //}
  }
  return ret;
}

static RET_CODE on_picture_timer(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;

  picture_tmr_destory();
  if(!ui_pic_play_next(&p_media, &g_rect))
  {
    ui_pic_clear(0);
    manage_close_menu(ROOT_ID_PICTURE, 0, 0);
  }

  return ret;
}

static RET_CODE on_picture_too_large(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;

  
  pic_stop();
  picture_tmr_destory();
  if(!ui_pic_play_next(&p_media, &g_rect))
  {
    ui_pic_clear(0);
    manage_close_menu(ROOT_ID_PICTURE, 0, 0);
  }

  return ret;
}

static RET_CODE on_picture_unsupport(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;

  pic_stop();

  picture_tmr_destory();
  if(!ui_pic_play_next(&p_media, &g_rect))
  {
    ui_pic_clear(0);
    manage_close_menu(ROOT_ID_PICTURE, 0, 0);
  }

  return ret;
}

static RET_CODE on_picture_fsceen_data_error(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  media_fav_t *p_media = NULL;

   pic_stop();
  picture_tmr_destory();
 
  if(!ui_pic_play_next(&p_media, &g_rect))
  {
    ui_pic_clear(0);
    manage_close_menu(ROOT_ID_PICTURE, 0, 0);
  }

  return ret;
}

static RET_CODE on_picture_next_music(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  media_fav_t *p_media = NULL;

  if(ui_music_get_play_status() == MUSIC_STAT_PLAY)
  {
    if(ui_music_pre_play_next(&p_media))
    {
      music_player_next(p_media->path);
      ui_music_reset_curn(p_media->path);
    }
  }

  return SUCCESS;
}

static RET_CODE on_picture_prev_music(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  media_fav_t *p_media = NULL;
  if(ui_music_get_play_status() == MUSIC_STAT_PLAY)
  {
    if(ui_music_pre_play_pre(&p_media))
    {
      music_player_next(p_media->path);
      ui_music_reset_curn(p_media->path);
    }
  }

  return SUCCESS;
}

static RET_CODE on_pic_list_volume(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
    open_volume_usb(ROOT_ID_PICTURE, para1); 
    return SUCCESS;
}


BEGIN_KEYMAP(picture_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_PREV_PIC)
  ON_EVENT(V_KEY_DOWN, MSG_NEXT_PIC)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PREV_PIC)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_NEXT_PIC)
  ON_EVENT(V_KEY_LEFT, MSG_CLOCK_WISE)
  ON_EVENT(V_KEY_RIGHT, MSG_ANTI_CLOCK)
  //  ON_EVENT(V_KEY_LEFT, MSG_PREV_PIC)
  //ON_EVENT(V_KEY_RIGHT, MSG_NEXT_PIC)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_OK, MSG_PIC_PLAY)
  ON_EVENT(V_KEY_PLAY, MSG_PIC_PLAY)
  ON_EVENT(V_KEY_PAUSE, MSG_PIC_PAUSE)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_NEXT, MSG_MUSIC_NEXT)
  ON_EVENT(V_KEY_PREV, MSG_MUSIC_PRE)
  ON_EVENT(V_KEY_VUP, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DOWN)
END_KEYMAP(picture_cont_keymap, NULL)

BEGIN_MSGPROC(picture_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_picture_exit)
  ON_COMMAND(MSG_DESTROY, on_picture_destory)
  ON_COMMAND(MSG_CLOCK_WISE, on_picture_clock_wise)
  ON_COMMAND(MSG_ANTI_CLOCK, on_picture_anti_clock)
  ON_COMMAND(MSG_PREV_PIC, on_picture_prev_pic)
  ON_COMMAND(MSG_NEXT_PIC, on_picture_next_pic)
  ON_COMMAND(MSG_PIC_PLAY, on_picture_resume)
  ON_COMMAND(MSG_PIC_PAUSE, on_picture_pause)
  ON_COMMAND(MSG_PLUG_OUT, on_picture_plug_out)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_picture_draw_end)
  ON_COMMAND(MSG_PIC_TIMER, on_picture_timer)
  ON_COMMAND(MSG_MUSIC_NEXT, on_picture_next_music)
  ON_COMMAND(MSG_MUSIC_PRE, on_picture_prev_music)
  ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, on_picture_too_large)
  ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, on_picture_unsupport)  
  ON_COMMAND(MSG_PIC_EVT_DATA_ERR, on_picture_fsceen_data_error)
  ON_COMMAND(MSG_VOLUME_UP, on_pic_list_volume)
  ON_COMMAND(MSG_VOLUME_DOWN, on_pic_list_volume)
END_MSGPROC(picture_cont_proc, ui_comm_root_proc)


