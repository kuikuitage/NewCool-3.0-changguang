/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_volume.h"
#include "ui_volume_usb.h"
#ifdef ENABLE_CA
//ca
#include "cas_manager.h"
#endif

static BOOL g_is_mute = FALSE;

BOOL ui_is_mute(void)
{
  return g_is_mute;
}


void ui_set_mute(BOOL is_mute)
{
  g_is_mute = is_mute;
  if(g_is_mute)
  {
    u8 focus = fw_get_focus_id();

    /* try to close volume bar */
    if(fw_find_root_by_id(ROOT_ID_VOLUME) != NULL)
    {
      UI_PRINTF("set_mute: close volume\n");
      close_volume();
    }
	
    if(fw_find_root_by_id(ROOT_ID_VOLUME_USB) != NULL)
    {
      UI_PRINTF("set_mute: close usb volume\n");
      close_volume_usb();
    }

    if((ui_get_ttx_flag() == FALSE) &&
        (ui_is_fullscreen_menu(focus)
         || focus == ROOT_ID_SMALL_LIST))
    {
      open_mute(0, 0);
    }
    else
    {
      if(focus == ROOT_ID_PROG_LIST
        || focus == ROOT_ID_USB_MUSIC
        || focus == ROOT_ID_EPG
        || focus == ROOT_ID_VEPG
        || focus == ROOT_ID_TIMESHIFT_BAR
        || focus == ROOT_ID_RECORD_MANAGER 
        || focus == ROOT_ID_PVR_PLAY_BAR
        || focus == ROOT_ID_PVR_REC_BAR
        || focus == ROOT_ID_NETWORK_MUSIC
#if ENABLE_FILE_PLAY  				
        || focus == ROOT_ID_USB_FILEPLAY
        || focus == ROOT_ID_FILEPLAY_BAR
        || focus == ROOT_ID_USB_MUSIC_FULLSCREEN
#endif
#if ENABLE_NETWORK
        || focus == ROOT_ID_VIDEO_PLAYER
        || focus == ROOT_ID_LIVE_TV
        || focus == ROOT_ID_IPTV_PLAYER
#endif
          )
      {
        open_mute(0, 0);
      }
    }
  }
  else
  {
    close_mute();
  }

  /* do something to set mute */
  UI_PRINTF("AP: set_mute = %d\n", g_is_mute);
  avc_set_mute_1(class_get_handle_by_id(AVC_CLASS_ID),
               g_is_mute, TRUE);
}


RET_CODE open_mute(u32 para1, u32 para2)
{
  control_t *p_cont, *p_icon;
  u16 cont_x, cont_y, cont_w, cont_h;
  u16 icon_x, icon_y, icon_w, icon_h;
  u16 icon_id;
  u16 left, top, width, height;
  u8 focus = fw_get_focus_id();
  BOOL is_fullscr = TRUE;
  BOOL is_media = FALSE;
  BOOL is_video_fullscr = FALSE;
  // check for close
  if(fw_find_root_by_id(ROOT_ID_MUTE) != NULL)
  {
    close_mute();
  }

  if((ROOT_ID_USB_MUSIC == focus) || (ROOT_ID_USB_PICTURE == focus) || (ROOT_ID_USB_FILEPLAY == focus))
  {
    is_media = TRUE;
  }

#if ENABLE_FILE_PLAY  
  if(ROOT_ID_FILEPLAY_BAR== focus)
  {
    is_video_fullscr = TRUE;
  }
#endif
  if(manage_get_preview_rect(fw_get_focus_id(),
                             &left, &top,
                             &width, &height))
  {
    is_fullscr = FALSE;
  }

  if(is_media)
  {
    cont_x = USB_MUSIC_MUTE_CONT_X;
    cont_y = USB_MUSIC_MUTE_CONT_Y;
    cont_w = MUTE_CONT_LIST_W;
    cont_h = MUTE_CONT_LIST_H;
    
    icon_x = MUTE_ICON_LIST_X, icon_y = MUTE_ICON_LIST_Y;
    icon_w = MUTE_ICON_LIST_W, icon_h = MUTE_ICON_LIST_H;
    icon_id = IM_ICON_MUTE;
  }
  else if((is_fullscr)||(is_video_fullscr))  // list mode
  {
    cont_x = MUTE_CONT_FULL_X, cont_y = MUTE_CONT_FULL_Y;
    cont_w = MUTE_CONT_FULL_W, cont_h = MUTE_CONT_FULL_H;
    icon_x = MUTE_ICON_FULL_X, icon_y = MUTE_ICON_FULL_Y;
    icon_w = MUTE_ICON_FULL_W, icon_h = MUTE_ICON_FULL_H;
    icon_id = IM_ICON_MUTE;
  }
  else
  {
    cont_x = left + width - MUTE_CONT_LIST_W - MUTE_CONT_PREVIEW_OX;
    cont_y = top + MUTE_CONT_PREVIEW_OY;
    cont_w = MUTE_CONT_LIST_W;
    cont_h = MUTE_CONT_LIST_H;
    
    icon_x = MUTE_ICON_LIST_X, icon_y = MUTE_ICON_LIST_Y;
    icon_w = MUTE_ICON_LIST_W, icon_h = MUTE_ICON_LIST_H;
    icon_id = IM_ICON_MUTE;
  }
  p_cont = fw_create_mainwin(ROOT_ID_MUTE,
                             cont_x, cont_y, cont_w, cont_h,
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_INACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);

  p_icon = ctrl_create_ctrl(CTRL_BMAP, 0,
                            icon_x, icon_y, icon_w, icon_h,
                            p_cont, 0);
  bmap_set_content_by_id(p_icon, icon_id);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
#ifdef ENABLE_CA
  cas_manage_finger_repaint();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
#endif
  return SUCCESS;
}


void close_mute(void)
{
  //manage_close_menu(ROOT_ID_MUTE, 0, 0);
  fw_destroy_mainwin_by_id(ROOT_ID_MUTE);
}
