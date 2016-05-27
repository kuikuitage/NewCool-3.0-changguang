/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_xextend.h"
#include "ui_sleep_timer.h"

/* others */
enum control_id
{
  IDC_INVALID = 0,
//  IDC_SLEEP_TIMER_SWITCH,
  IDC_SLEEP_TIMER_TIME,
};

u16 sleep_timer_cont_keymap(u16 key);

RET_CODE sleep_timer_time_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

//RET_CODE sleep_timer_select_proc(control_t *p_ctrl, u16 msg,
//                                      u32 para1, u32 para2);

static BOOL sleep_time_is_invalid(control_t *p_time, utc_time_t *p_sleep_time)
{
  utc_time_t max_time = {0};
  utc_time_t min_time = {0};
  BOOL is_invalid = FALSE;
  
  tbox_get_time(p_time, p_sleep_time);
  max_time.hour = 24;
  //min_time.hour = 3;
  min_time.minute = 0;
  
  if(time_cmp(p_sleep_time, &max_time, TRUE) > 0)
  {
    p_sleep_time->hour = max_time.hour;
    p_sleep_time->minute = max_time.minute;
    is_invalid = TRUE;
  }
  else if(time_cmp(p_sleep_time, &min_time, TRUE) < 0)
  {
    p_sleep_time->hour = min_time.hour;
    p_sleep_time->minute = min_time.minute;
    is_invalid = TRUE;
  }
  else if(p_sleep_time->minute >= 60)
  {
    p_sleep_time->minute = 59;
    is_invalid = TRUE;
  }

  return is_invalid;
}


RET_CODE open_sleep_timer(u32 para1, u32 para2)
{
  control_t *p_cont, *p_time;
  utc_time_t sleep_time = {0};
  btn_rec_t *left_btn_rec = (btn_rec_t*)para1;
  /* pre-create */
#ifndef SPT_SUPPORT
  p_cont = ui_comm_root_cont_create(ROOT_ID_SLEEP_TIMER, ROOT_ID_XEXTEND, 
  				left_btn_rec->x+left_btn_rec->w+EXTEND_AND_MENU_GAP, 
  				left_btn_rec->y+SLEEP_TIMER_CONT_H, SLEEP_TIMER_CONT_W,
    			SLEEP_TIMER_CONT_H, OBJ_ATTR_ACTIVE, 0);
  ctrl_set_keymap(p_cont, sleep_timer_cont_keymap);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
/*
  p_switch = ui_comm_select_create(p_cont, IDC_SLEEP_TIMER_SWITCH, 
    0, 0, 
    SLEEP_TIMER_ITEM_LW, SLEEP_TIMER_ITEM_RW);
  ui_comm_select_set_static_txt(p_switch, IDS_TIMER_MODE);
  ui_comm_ctrl_set_proc(p_switch, sleep_timer_select_proc);
  ui_comm_select_set_param(p_switch, TRUE,
                             CBOX_WORKMODE_STATIC, 2,
                             CBOX_ITEM_STRTYPE_STRID, NULL);
  ui_comm_select_set_content(p_switch, 0, IDS_ON);
  ui_comm_select_set_content(p_switch, 1, IDS_OFF);

  sys_status_get_status(BS_SLEEP_LOCK, &is_sleep_on);
  ui_comm_select_set_focus(p_switch, !is_sleep_on);
*/
  p_time = ui_comm_timedit_create(p_cont, IDC_SLEEP_TIMER_TIME, 
    0, 
    0, 
    SLEEP_TIMER_ITEM_LW, SLEEP_TIMER_ITEM_RW);
  ui_comm_ctrl_set_proc(p_time, sleep_timer_time_proc);
  ui_comm_timedit_set_static_txt(p_time, IDS_SLEEP_TIME);

  sys_status_get_sleep_time(&sleep_time);
  ui_comm_timedit_set_time(p_time, &sleep_time);

  ui_comm_timedit_set_param(p_time, 0, TBOX_ITEM_HOUR, TBOX_HOUR | TBOX_MIN, 
    TBOX_SEPARATOR_TYPE_UNICODE, 18);

  ui_comm_timedit_set_separator_by_ascchar(p_time, 3, ':');

/*
  ctrl_set_related_id(p_switch, 0, IDC_SLEEP_TIMER_TIME, 
    0, IDC_SLEEP_TIMER_TIME);
  ctrl_set_related_id(p_time, 0, IDC_SLEEP_TIMER_SWITCH, 
    0, IDC_SLEEP_TIMER_SWITCH);*/
#else
#endif

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_time, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}
/*
RET_CODE on_sleep_timer_change_focus(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2)
{
  control_t *p_cont, *p_time, *p_switch;
  u16 focus;
  sys_status_t *sys_data = NULL;

  cbox_class_proc(p_ctrl, msg, 0, 0);

  p_switch = ctrl_get_parent(p_ctrl);
  p_cont = ctrl_get_parent(p_switch);
  p_time = ctrl_get_child_by_id(p_cont, IDC_SLEEP_TIMER_TIME);

  focus = cbox_static_get_focus(p_ctrl);

  sys_data = sys_status_get();

  sys_data->auto_sleep = focus;
  if(focus == 0)
  {
    ui_comm_ctrl_update_attr(p_time, FALSE);
    sys_status_set_status(BS_SLEEP_LOCK, FALSE);
    ui_sleep_timer_destory();
  }
  else
  {
    sys_status_set_status(BS_SLEEP_LOCK, TRUE);
    ui_comm_ctrl_update_attr(p_time, TRUE);
    
    ui_sleep_timer_create();
  }

  sys_status_save();

  ctrl_paint_ctrl(p_time, TRUE);

  return SUCCESS;
}
*/
static RET_CODE on_sleep_timer_time_select(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2)
{
  utc_time_t sleep_time = {0};
  MT_ASSERT(p_ctrl != NULL);

  if(!(p_ctrl->priv_attr & TBOX_HL_STATUS_MASK))
  {
    tbox_enter_edit(p_ctrl);
  }
  else
  {
    if(sleep_time_is_invalid(p_ctrl, &sleep_time))
    {
      tbox_set_time(p_ctrl, &sleep_time);
      ctrl_add_rect_to_invrgn(p_ctrl, NULL);
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
    else
    {
      p_ctrl->priv_attr &= (~TBOX_HL_STATUS_MASK);
      ctrl_add_rect_to_invrgn(p_ctrl, NULL);
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);

      ctrl_process_msg(p_ctrl, MSG_CHANGED, 0, 0);
    }
  }

  return SUCCESS;
}

RET_CODE on_sleep_timer_time_unselect(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  utc_time_t sleep_time = {0};
  
  if(tbox_is_on_edit(p_ctrl))
  {    
    if(sleep_time_is_invalid(p_ctrl, &sleep_time))
    {
      tbox_set_time(p_ctrl, &sleep_time);
      ctrl_add_rect_to_invrgn(p_ctrl, NULL);
      ctrl_process_msg(p_ctrl, MSG_PAINT, FALSE, 0);
    }
    else
    {
      ret = ERR_NOFEATURE;
    }
  }
  else
  {
    ret = ERR_NOFEATURE;
  }

  return ret;  
}

RET_CODE on_sleep_timer_sleep_time_changed(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2)
{
  utc_time_t sleep_time = {0};
  utc_time_t countdown_time = {0};
  sys_status_t *sys_data = NULL;

  tbox_get_time(p_ctrl, &sleep_time);

  ui_sleep_timer_destory();
  sys_data = sys_status_get();
  if(!memcmp(&sleep_time, &countdown_time, sizeof(utc_time_t)))
  {
	  sys_data->auto_sleep = 0;
	  sys_status_set_status(BS_SLEEP_LOCK, FALSE);
	 DEBUG(DBG, INFO, "close sleep timer\n");
	 return SUCCESS;
  }
  DEBUG(DBG, INFO, "sleep time:%04d-%02d-%02d %02d:%02d:%02d\n", sleep_time.year, 
  	sleep_time.month, sleep_time.day, sleep_time.hour, sleep_time.minute, sleep_time.second);
  sys_data->auto_sleep = 1;
  sys_status_set_status(BS_SLEEP_LOCK, TRUE);
  sys_status_set_sleep_time(&sleep_time);
  sys_status_save();
  memset(&sleep_time, 0, sizeof(utc_time_t));
  ui_sleep_timer_create();
  
  return SUCCESS;  
}
/*
static RET_CODE on_hkey_sleep_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  //save_data();
  
  fw_destroy_mainwin_by_id(ROOT_ID_SLEEP_HOTKEY);
  
  fw_tmr_destroy(ROOT_ID_SLEEP_HOTKEY, MSG_EXIT);
  return SUCCESS;
}
*/
BEGIN_KEYMAP(sleep_timer_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_SLEEP, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(sleep_timer_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(sleep_timer_time_proc, ui_comm_time_proc)
  ON_COMMAND(MSG_SELECT, on_sleep_timer_time_select)
  ON_COMMAND(MSG_UNSELECT, on_sleep_timer_time_unselect)
  ON_COMMAND(MSG_CHANGED, on_sleep_timer_sleep_time_changed)
 // ON_COMMAND(MSG_CANCEL, on_hkey_sleep_exit)
END_MSGPROC(sleep_timer_time_proc, ui_comm_time_proc)
/*
BEGIN_MSGPROC(sleep_timer_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_sleep_timer_change_focus)
  ON_COMMAND(MSG_DECREASE, on_sleep_timer_change_focus)
    ON_COMMAND(MSG_CANCEL, on_hkey_sleep_exit)
END_MSGPROC(sleep_timer_select_proc, cbox_class_proc)

*/





