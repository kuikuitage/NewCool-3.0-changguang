/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_volume.h"
#include "ui_mute.h"
#ifdef ENABLE_CA
//ca
#include "cas_manager.h"
#endif

enum local_msg
{
  MSG_AUDIO = MSG_LOCAL_BEGIN + 900,
  MSG_PARENT,
};
enum control_id
{
  IDC_INVALID = 0,
  IDC_ICON,
  IDC_BAR,
};

static dvbs_prog_node_t curn_prog;
BOOL is_display_pg = TRUE;
BOOL is_no_audio = FALSE;

u16 volume_keymap(u16 key);
RET_CODE volume_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 volume_bar_keymap(u16 key);
RET_CODE volume_bar_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);


void set_volume(u8 volume)
{    
  avc_setvolume_1(class_get_handle_by_id(AVC_CLASS_ID), volume);
  curn_prog.volume = volume;
  #if 0
  if(is_display_pg == TRUE)
  {
    curn_prog.volume = volume;
    #if ENABLE_GLOBAL_VOLUME
    sys_status_set_global_volume(curn_prog.volume);
    sys_status_save();
    #else
    db_dvbs_edit_program(&curn_prog);
    db_dvbs_save_pg_edit(&curn_prog);
    #endif
  }
  else
  {
    sys_status_set_global_volume(volume);
    sys_status_save();
  }
  #endif
}


RET_CODE open_volume(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_frm;
  control_t *p_bar = NULL, *p_icon = NULL;
  u8 volume;
if(is_display_pg == TRUE)
{
#if ENABLE_GLOBAL_VOLUME
    curn_prog.volume = sys_status_get_global_volume( ); 
#else
  u16 prog_id;
  prog_id = sys_status_get_curn_group_curn_prog_id();
  if (db_dvbs_get_pg_by_id(prog_id, &curn_prog) != DB_DVBS_OK)
  {
    return ERR_FAILURE;
  }
#endif
}
is_no_audio =(BOOL) para2;

  if(ui_is_mute())
  {
    ui_set_mute(FALSE);
  }

  /* get volume */
  if(is_display_pg == TRUE)
  {
    volume = (u8)curn_prog.volume;
  }
  else
  {
    volume = sys_status_get_global_volume();
  }
  
  switch(para1)
  {
    case V_KEY_LEFT:
      if(volume > 0)
      {
        volume--;
        /* set volume */
        set_volume(volume);
      }
      break;
    case V_KEY_RIGHT:
      if(volume < VOLUME_MAX)
      {
        volume++;
        /* set volume */
        set_volume(volume);
      }
    default:
      /* do nothing */;
  }

  p_cont = fw_create_mainwin(ROOT_ID_VOLUME,
                             VOLUME_L, VOLUME_T,
                             VOLUME_W, VOLUME_H,
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_VOLUME_CONT,
                  RSI_VOLUME_CONT, RSI_VOLUME_CONT);
  ctrl_set_proc(p_cont, volume_proc);
  ctrl_set_keymap(p_cont, volume_keymap);

  p_frm = ctrl_create_ctrl(CTRL_CONT, 0,
                           VOLUME_BAR_CONT_X, VOLUME_BAR_CONT_Y,
                           VOLUME_BAR_CONT_W, VOLUME_BAR_CONT_H,
                           p_cont, 0);
  ctrl_set_rstyle(p_frm,
                  RSI_POPUP_BG,
                  RSI_POPUP_BG,
                  RSI_POPUP_BG);

  p_icon = ctrl_create_ctrl(CTRL_BMAP, IDC_ICON,
                           VOLUME_BAR_ICON_X, VOLUME_BAR_ICON_Y,
                           VOLUME_BAR_ICON_W, VOLUME_BAR_ICON_H,
                           p_frm, 0);
  bmap_set_content_by_id(p_icon, IM_ICON_SOUND);                     

  p_bar = ctrl_create_ctrl(CTRL_PBAR, IDC_BAR,
                           VOLUME_BAR_X, VOLUME_BAR_Y,
                           VOLUME_BAR_W, VOLUME_BAR_H,
                           p_frm, 0);
  ctrl_set_rstyle(p_bar, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_mrect(p_bar, 0, 0, VOLUME_BAR_W, VOLUME_BAR_H);
  pbar_set_rstyle(p_bar, RSI_PROGRESS_BAR_VOLUME_MID, RSI_PBACK, RSI_PROGRESS_BAR_VOLUME_MID);
  pbar_set_count(p_bar, 0, VOLUME_MAX, VOLUME_MAX);
  pbar_set_direction(p_bar, 1);
  pbar_set_workmode(p_bar, 0, 4);
  pbar_set_current(p_bar, volume);
  ctrl_set_proc(p_bar, volume_bar_proc);
  ctrl_set_keymap(p_bar, volume_bar_keymap);

  ctrl_default_proc(p_bar, MSG_GETFOCUS, 0, 0);
  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  
  #ifdef ENABLE_CA
  cas_manage_finger_repaint();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif

  #ifdef ENABLE_ADS
  ui_adv_pic_play(ADS_AD_TYPE_VOLBAR, ROOT_ID_VOLUME);
  #endif
  
  return SUCCESS;
}


RET_CODE close_volume(void)
{
 if(is_display_pg == TRUE)
 {
 #ifdef ENABLE_GLOBAL_VOLUME
   sys_status_set_global_volume(curn_prog.volume);
   sys_status_save();
 #else
   db_dvbs_edit_program(&curn_prog);
   db_dvbs_save_pg_edit(&curn_prog);
 #endif

    #ifdef ENABLE_ADS
	pic_adv_stop();
    #endif
 }

    manage_close_menu(ROOT_ID_VOLUME, 0, 0);

  return SUCCESS;
}


static RET_CODE on_exit_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  close_volume();
  return SUCCESS;
}

static RET_CODE on_change_volume(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 volume = 0;
  if(is_display_pg == TRUE)
  {
    volume = (u8)curn_prog.volume;
  }
  else
  {
    volume = sys_status_get_global_volume();
  }
  
  if(msg == MSG_INCREASE)
  {
    if(volume < VOLUME_MAX)
    {
      volume++;
    }
  }
  else
  {
    if(volume > 0)
    {
      volume--;
    }
  }
  /* set volume */
  set_volume(volume);
  pbar_class_proc(p_ctrl, msg, para1, para2);
  ctrl_paint_ctrl(p_ctrl,TRUE);
#ifdef ENABLE_CA
  cas_manage_finger_repaint();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
#endif

  return SUCCESS;
}

static RET_CODE on_audio(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u32 vkey = para1;
   if(!is_no_audio)
  {
      manage_open_menu(ROOT_ID_AUDIO_SET, vkey, 0);
  }
  return SUCCESS;
}

static RET_CODE on_exit_volume_soon(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  fw_notify_parent(ROOT_ID_VOLUME, NOTIFY_T_KEY, FALSE, para1, 0, 0);
  close_volume();
  return SUCCESS;
}

BEGIN_KEYMAP(volume_keymap, NULL)
//ON_EVENT(V_KEY_MENU, MSG_EXIT)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
ON_EVENT(V_KEY_EXIT, MSG_EXIT)
ON_EVENT(V_KEY_AUDIO,MSG_AUDIO)
ON_EVENT(V_KEY_REC,MSG_PARENT)
ON_EVENT(V_KEY_BLUE, MSG_PARENT)
//ON_EVENT(V_KEY_INFO,MSG_EXIT)
END_KEYMAP(volume_keymap, NULL)

BEGIN_MSGPROC(volume_proc, cont_class_proc)
ON_COMMAND(MSG_EXIT, on_exit_volume)
ON_COMMAND(MSG_AUDIO, on_audio)
ON_COMMAND(MSG_PARENT, on_exit_volume_soon)
END_MSGPROC(volume_proc, cont_class_proc)

BEGIN_KEYMAP(volume_bar_keymap, NULL)
ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
ON_EVENT(V_KEY_VDOWN, MSG_DECREASE)
ON_EVENT(V_KEY_VUP, MSG_INCREASE)
END_KEYMAP(volume_bar_keymap, NULL)

BEGIN_MSGPROC(volume_bar_proc, pbar_class_proc)
ON_COMMAND(MSG_INCREASE, on_change_volume)
ON_COMMAND(MSG_DECREASE, on_change_volume)
END_MSGPROC(volume_bar_proc, pbar_class_proc)
