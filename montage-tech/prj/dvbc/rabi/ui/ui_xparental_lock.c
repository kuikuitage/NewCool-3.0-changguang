/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_xsys_set.h"
#include "ui_xparental_lock.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_AGE_LIMIT,
  IDC_MENU_LOCK,
  IDC_CHAN_LOCK,
  IDC_NEW,
  IDC_CONFIRM,
  //IDC_PWD_MENU,
  IDC_ITEM_MAX
};

u16 xparental_lock_cont_keymap(u16 key);
RET_CODE xparental_lock_cont_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

RET_CODE xparental_lock_select_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

RET_CODE xparental_lock_pwdedit_proc(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2);

static u16 xparental_pwdlg_keymap(u16 key);
RET_CODE xparental_pwdlg_proc(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);

static RET_CODE on_xparental_lock_cont_focus_change(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_child;
  control_t *p_new;
  u8 ctrl_id;

  p_child = ctrl_get_active_ctrl(p_ctrl);

  ctrl_id = ctrl_get_ctrl_id(p_child);

  if(ctrl_id == IDC_CONFIRM)
  {
    ui_comm_pwdedit_empty_value(p_child);
    
    p_new = ctrl_get_child_by_id(p_ctrl, IDC_NEW);
    ui_comm_pwdedit_empty_value(p_new);

    ui_comm_ctrl_update_attr(p_child, FALSE);
  }

  return ERR_NOFEATURE;
}

static RET_CODE on_xparental_lock_select_change(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  u16 focus;

  /*before switch*/

  ret = cbox_class_proc(p_ctrl, msg, para1, para2);

  /*after switch*/
  switch (ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
  {
    case IDC_AGE_LIMIT:
      focus = cbox_static_get_focus(p_ctrl);
      sys_status_set_age_limit(focus);
      ui_reset_chkpwd();
      break;
    case IDC_MENU_LOCK:
      focus = cbox_static_get_focus(p_ctrl);
      sys_status_set_status(BS_MENU_LOCK, (BOOL)focus);
      break;
    case IDC_CHAN_LOCK:
      focus = cbox_static_get_focus(p_ctrl);
      sys_status_set_status(BS_PROG_LOCK, (BOOL)focus);    
      break;
    default:
      ;
  }
  sys_status_save();   

  return ret;
}

static RET_CODE on_xparental_lock_pwdedit_maxtext(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_cont;
  control_t *p_new, *p_confirm, *p_active;
  u8 ctrl_id;
  u32 confirm_pwd, new_pwd;
  pwd_set_t pwd_set;

  p_active = ctrl_get_parent(p_ctrl);
  p_cont = ctrl_get_parent(p_active);

  ctrl_id = ctrl_get_ctrl_id(p_active);
  
  if(ctrl_id == IDC_NEW)
  {
    p_confirm = ctrl_get_child_by_id(p_cont, IDC_CONFIRM);
    ui_comm_ctrl_update_attr(p_confirm, TRUE);
    ctrl_process_msg(p_cont, MSG_FOCUS_DOWN, 0, 0);
  }
  else//IDC_CONFIRM
  {
    p_new = ctrl_get_child_by_id(p_cont, IDC_NEW);
    p_confirm = ctrl_get_child_by_id(p_cont, IDC_CONFIRM);
    confirm_pwd = ui_comm_pwdedit_get_value(p_confirm);
    new_pwd = ui_comm_pwdedit_get_value(p_new);

    if(new_pwd == confirm_pwd)//success
    {
      sys_status_get_pwd_set(&pwd_set);
      pwd_set.normal = new_pwd;
      sys_status_set_pwd_set(&pwd_set);
      sys_status_save();
      
	  ctrl_paint_ctrl(p_confirm, TRUE);

      ui_comm_cfmdlg_open(NULL, IDS_PASSWORD_CHANGE_FINISH, NULL, 0);

      ret = manage_close_menu(ROOT_ID_XPARENTAL_LOCK, 0, 0);
    }
    else
    {
      ctrl_process_msg(p_ctrl, MSG_EMPTY, 0, 0);
      ctrl_paint_ctrl(p_ctrl, TRUE);

	  ui_comm_cfmdlg_open(NULL, IDS_MSG_REINPUT_PASSWORD, NULL, 0); 
    }
  }
  

  return SUCCESS;
}

static RET_CODE on_parental_pwdlg_correct(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();

  return open_xparental_lock(0, 0);
}

static RET_CODE on_parental_pwdlg_exit(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  
  return SUCCESS;
}

RET_CODE preopen_xparental_lock(u32 para1, u32 para2)
{
  comm_pwdlg_data_t pwdlg_data =
  {
    ROOT_ID_INVALID,
    PWD_DLG_FOR_PLAY_X,
    PWD_DLG_FOR_PLAY_Y,
    IDS_MSG_INPUT_PASSWORD,
    xparental_pwdlg_keymap,
    xparental_pwdlg_proc,
  };

  ui_comm_pwdlg_open(&pwdlg_data);

  return SUCCESS;
}

RET_CODE open_xparental_lock(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[IDC_ITEM_MAX];
  u8 i, j;
  u16 age_limit ; //= AGE_LIMIT_VIEW_ALL;
  BOOL is_lock;

#ifndef SPT_SUPPORT
  u16 stxt [IDC_ITEM_MAX] =
  {0, IDS_AGE_LIMIT, IDS_MENU_LOCK, IDS_CHANNEL_LOCK, 
    IDS_NEW_PASSWORD, IDS_CONFIRM_PASSWORD};
  u8 opt_cnt [IDC_ITEM_MAX] = {0, 6, 2, 2, 0, 0 };
  u16 opt_data[IDC_ITEM_MAX][6] = {
    { 0 },
    { IDS_VIEW_ALL, IDS_7, IDS_12, IDS_15, IDS_18, IDS_LOCK_ALL},
    { IDS_OFF, IDS_ON },
    { IDS_OFF, IDS_ON },
    { 0 },
    { 0 },
  };
  u16 y;
#endif
#ifndef SPT_SUPPORT
p_cont = fw_create_mainwin(ROOT_ID_XPARENTAL_LOCK,
							   LOCK_X, LOCK_Y,
							   LOCK_W, LOCK_H,
                             ROOT_ID_XSYS_SET, 0, OBJ_ATTR_ACTIVE, 0);
  ctrl_set_rstyle(p_cont,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, xparental_lock_cont_keymap);
  ctrl_set_proc(p_cont, xparental_lock_cont_proc);

  y = XPARENTAL_LOCK_ITEM_Y;
  for (i = IDC_AGE_LIMIT; i < IDC_ITEM_MAX; i++)
  {
    switch (i)
    {
      // edit
      case IDC_NEW:
      case IDC_CONFIRM:
        p_ctrl[i] = ui_comm_pwdedit_create(p_cont, i,
                                             UI_LOCK_SET_X, y,
                                             UI_LOCK_SET_W_L,
                                             UI_LOCK_SET_W_R);
        ui_comm_pwdedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_pwdedit_set_param(p_ctrl[i], 4);
        ui_comm_pwdedit_set_mask(p_ctrl[i], '-', '*');
        ui_comm_ctrl_set_proc(p_ctrl[i], xparental_lock_pwdedit_proc);
        break;

      // select
      default:
        p_ctrl[i] = ui_comm_select_create(p_cont, i,
                                            UI_LOCK_SET_X, y,
                                            UI_LOCK_SET_W_L,
                                            UI_LOCK_SET_W_R);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], xparental_lock_select_proc);

        ui_comm_select_set_param(p_ctrl[i], TRUE,
                                   CBOX_WORKMODE_STATIC, opt_cnt[i],
                                   CBOX_ITEM_STRTYPE_STRID, NULL);
        for (j = 0; j < opt_cnt[i]; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, opt_data[i][j]);
        }
    }

    ctrl_set_related_id(p_ctrl[i],
                        0,                                     /* left */
                        (u8)((i == 1)  ? (IDC_ITEM_MAX -1) : (i-1)),           /* up */
                        0,                                     /* right */
                        (u8)((i == (IDC_ITEM_MAX -1)) ? 1 : (i+1)));          /* down */

    y += XPARENTAL_LOCK_ITEM_H + XPARENTAL_LOCK_ITEM_V_GAP;
  }
#else
#endif
  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  sys_status_get_age_limit(&age_limit);
  if(AGE_LIMIT_MAX_CNT > age_limit)
  {
    ui_comm_select_set_focus(p_ctrl[IDC_AGE_LIMIT], age_limit);
  }
  else
  {
    ui_comm_select_set_focus(p_ctrl[IDC_AGE_LIMIT], AGE_LIMIT_VIEW_ALL);
  }
  
  sys_status_get_status(BS_MENU_LOCK, &is_lock);
  ui_comm_select_set_focus(p_ctrl[IDC_MENU_LOCK], is_lock);
  
  sys_status_get_status(BS_PROG_LOCK, &is_lock);
  ui_comm_select_set_focus(p_ctrl[IDC_CHAN_LOCK], is_lock);

  if(CUSTOMER_DTMB_CHANGSHA_HHT == CUSTOMER_ID)
  {
	ui_comm_ctrl_update_attr(p_ctrl[IDC_NEW], FALSE);
  }

  ui_comm_ctrl_update_attr(p_ctrl[IDC_CONFIRM], FALSE);

  ctrl_default_proc(p_ctrl[IDC_AGE_LIMIT], MSG_GETFOCUS, 0, 0); 
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

BEGIN_KEYMAP(xparental_lock_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(xparental_lock_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(xparental_lock_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_xparental_lock_cont_focus_change)
  ON_COMMAND(MSG_FOCUS_DOWN, on_xparental_lock_cont_focus_change)  
END_MSGPROC(xparental_lock_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(xparental_lock_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_xparental_lock_select_change)
  ON_COMMAND(MSG_DECREASE, on_xparental_lock_select_change)  
END_MSGPROC(xparental_lock_select_proc, cbox_class_proc)

BEGIN_MSGPROC(xparental_lock_pwdedit_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_MAXTEXT, on_xparental_lock_pwdedit_maxtext)
END_MSGPROC(xparental_lock_pwdedit_proc, ui_comm_edit_proc)

BEGIN_KEYMAP(xparental_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_EXIT)
  ON_EVENT(V_KEY_DOWN, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(xparental_pwdlg_keymap, NULL)

BEGIN_MSGPROC(xparental_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_CORRECT_PWD, on_parental_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_parental_pwdlg_exit)
END_MSGPROC(xparental_pwdlg_proc, cont_class_proc)

