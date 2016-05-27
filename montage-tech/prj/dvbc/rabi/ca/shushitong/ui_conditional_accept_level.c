/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_level.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_CA,
};

enum ca_level_control_id
{
  IDC_CA_LEVEL_CONTROL_PIN = 1,
  IDC_CA_LEVEL_CONTROL_LEVEL,
  IDC_CA_LEVEL_CONTROL_ENTER,
  IDC_CA_LEVEL_CHANGE_OK,
  IDC_CA_LEVEL_CHANGE_RESULT,
  IDC_CA_PIN_WARN,
};

static cas_rating_set_t rate_info;

u16 conditional_accept_level_cont_keymap(u16 key);
RET_CODE conditional_accept_level_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

RET_CODE conditional_accept_level_select_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 conditional_accept_level_pwdedit_keymap(u16 key);
RET_CODE conditional_accept_level_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 conditional_accept_level_enter_keymap(u16 key);
RET_CODE conditional_accept_level_enter_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static void conditional_accept_level_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
  control_t *p_level = ctrl_get_child_by_id(p_cont, IDC_CA_LEVEL_CONTROL_LEVEL);
  control_t *p_result = ctrl_get_child_by_id(p_cont, IDC_CA_LEVEL_CHANGE_RESULT);
  
  OS_PRINTF("LEVER : %d\n",p_card_info->card_work_level);
  
  //返回值：-1 操作失败；0 四岁以上；1 八岁以上；2 十二岁以上；3 十六岁以上
  if(p_card_info->cas_rating== 0XFF)
  {
    text_set_content_by_strid(p_result,IDS_CA_RECEIVED_DATA_FAIL);
  }
  else
  {
    ui_comm_select_set_focus(p_level, p_card_info->cas_rating);
    OS_PRINTF("set LEVER : %d\n",p_card_info->cas_rating);
  }
}

RET_CODE open_conditional_accept_level(u32 para1, u32 para2)
{
  control_t *p_cont = NULL;
  control_t *p_ctrl = NULL;
  u8 i;
  u16 y;
  u16 stxt_level_control[CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT] =
  {
    IDS_CA_INPUT_PIN, IDS_CA_SELECT_LEVEL,
  };

  p_cont = ui_comm_right_root_create(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, conditional_accept_level_cont_keymap);
  ctrl_set_proc(p_cont, conditional_accept_level_cont_proc);

  //level control
  y = CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_Y;
  for (i = 0; i<CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT; i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl = ui_comm_pwdedit_create(p_cont, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_X, y,
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_LW,
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_RW);
        ui_comm_pwdedit_set_static_txt(p_ctrl, stxt_level_control[i]);
        ui_comm_pwdedit_set_param(p_ctrl, 6);
        ui_comm_ctrl_set_keymap(p_ctrl, conditional_accept_level_pwdedit_keymap);
        ui_comm_ctrl_set_proc(p_ctrl, conditional_accept_level_pwdedit_proc);
        break;
      case 1:
        p_ctrl = ui_comm_select_create(p_cont, (u8)(IDC_CA_LEVEL_CONTROL_PIN + i),
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_X, y,
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_LW,
                                           CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl, stxt_level_control[i]);
        ui_comm_select_set_param(p_ctrl, TRUE,
                                   CBOX_WORKMODE_STATIC, CONDITIONAL_ACCEPT_WATCH_LEVEL_TOTAL,
                                   CBOX_ITEM_STRTYPE_STRID,
                                   NULL);
        
        //返回值：-1 操作失败；0 四岁以上；1 八岁以上；2 十二岁以上；3 十六岁以上
        ui_comm_select_set_content(p_ctrl, 0, IDS_FOUR_YEARS_UP);
        ui_comm_select_set_content(p_ctrl, 1, IDS_EIGHT_YEARS_UP);
        ui_comm_select_set_content(p_ctrl, 2, IDS_TWELVE_YEARS_UP);
        ui_comm_select_set_content(p_ctrl, 3, IDS_SIXTEEN_YEARS_UP);
		break;
	  case 2:
	  	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_CA_LEVEL_CONTROL_ENTER,
                              CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_X,y,
                              (CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_LW + CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_RW),
                              CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H
                              ,p_cont, 0);
		ctrl_set_keymap(p_ctrl, conditional_accept_level_enter_keymap);
        ctrl_set_proc(p_ctrl, conditional_accept_level_enter_proc);
		ctrl_set_rstyle(p_ctrl,RSI_PBACK,RSI_SELECT_F,RSI_PBACK);
		text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
		text_set_offset(p_ctrl, COMM_CTRL_OX, 0);
    	text_set_font_style(p_ctrl,FSI_WHITE_24,FSI_WHITE_24,FSI_WHITE_24);
		text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_ctrl,IDS_START);
        break;
      default:
        p_ctrl = NULL;
        break;
    }
    if(NULL  == p_ctrl)
    {
        break;
    }

    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT) %
                             CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_PIN),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT + IDC_CA_LEVEL_CONTROL_PIN));/* down */

    y += CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H + CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_V_GAP;
  }

  //change result
  p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_LEVEL_CHANGE_RESULT,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_X,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_Y,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_W,
                              CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_H,
                              p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_COMM_CONT_SH, RSI_COMM_CONT_HL, RSI_COMM_CONT_GRAY);
  text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
  text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);  
  ctrl_set_attr(p_ctrl,OBJ_ATTR_ACTIVE);
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  //conditional_accept_level_set_content(p_cont, p_card_info_t->cas_rating);

  ctrl_default_proc(ctrl_get_child_by_id(p_cont, IDC_CA_LEVEL_CONTROL_PIN), MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  //ui_ca_get_info(CAS_CMD_RATING_GET, 0 ,0);
#ifndef WIN32
  ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0 ,0);
#endif
  
  //ui_comm_change_window_focus(ROOT_ID_CONDITIONAL_ACCEPT_LEVEL,ROOT_ID_CONDITIONAL_ACCEPT);
  return SUCCESS;
}

static RET_CODE on_conditional_accept_level_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  conditional_accept_level_set_content(p_cont, (cas_card_info_t *)para2);

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_conditional_accept_level_set(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_result = NULL;
  p_result = ctrl_get_child_by_id(p_cont, IDC_CA_LEVEL_CHANGE_RESULT);

  //when set repeatly, clear previous show
  text_set_content_by_strid(p_result, RSC_INVALID_ID);
  if(para1 == 0)
  {
    text_set_content_by_strid(p_result, IDS_CA_SET_SUCCESS);
  }
  else if(para1 == CAS_E_PARAM_ERR)
  {
    text_set_content_by_strid(p_result, IDS_CA_PARAME_SET_ERROR);
  }
  else if(para1 == CAS_E_PIN_INVALID)
  {
    text_set_content_by_strid(p_result, IDS_CA_PASSWORD_PIN_ERROR);
  }
  else if(para1 == CAS_E_WATCHRATING_INVALID)
  {
    text_set_content_by_strid(p_result, IDS_CA_IC_CMD_FAIL);
  }
  else if(para1 == 9)
  {
    text_set_content_by_strid(p_result, IDS_CA_PIN_MODIFY_LOCK);
  }
  else if(para1 == CAS_E_PIN_LOCKED)
  {
    text_set_content_by_strid(p_result,IDS_CARD_LOCKED);
  }
  else
  {
    text_set_content_by_strid(p_result, IDS_CA_PASSWORD_ERROR);
  }
  ctrl_paint_ctrl(p_result,TRUE);

  return SUCCESS;
}


static RET_CODE on_conditional_accept_level_pwdedit_maxtext(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_active;
  u8 ctrl_id;

  p_active = ctrl_get_parent(p_ctrl);
  p_cont = ctrl_get_parent(p_active);

  ctrl_id = ctrl_get_ctrl_id(p_active);

  ctrl_process_msg(p_cont, MSG_FOCUS_DOWN, 0, 0);

  return SUCCESS;
}

static RET_CODE on_conditional_accept_level_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
#define	UI_MAX_PIN_LEN	6
  control_t *p_pin, *p_level = NULL, *p_result,*p_parent;
  u32 card_pin = 0;
  u8 focus = 0;
  BOOL pin_code_is_full;
  s8 i =0, pin = 0;

  p_parent = ctrl_get_parent(p_ctrl);
  p_result = ctrl_get_child_by_id(p_parent, IDC_CA_LEVEL_CHANGE_RESULT);
  
  p_pin = ctrl_get_child_by_id(p_parent, IDC_CA_LEVEL_CONTROL_PIN);
  p_level = ctrl_get_child_by_id(p_parent, IDC_CA_LEVEL_CONTROL_LEVEL);
  focus = ui_comm_select_get_focus(p_level);
  UI_PRINTF("level= %d \n",focus);

  if(FALSE == ui_is_smart_card_insert())
  {
    text_set_content_by_strid(p_result, IDS_CA_CARD_NOT_INSERT);
    ctrl_paint_ctrl(p_result, TRUE);
	return SUCCESS;
  }

  card_pin = ui_comm_pwdedit_get_realValue(p_pin);
  pin_code_is_full = ui_comm_pwdedit_is_full(p_pin);
  
  for (i = UI_MAX_PIN_LEN -1; i >= 0; i--)
  {
    pin = card_pin % 10;
    rate_info.pin[i] = pin;
    card_pin = card_pin / 10;
  }
  
  if(pin_code_is_full == FALSE)
  {
    OS_PRINTF("pin_code_is_full == FALSE\n");
    text_set_content_by_strid(p_result, IDS_DIGIT_NOT_ENOUGH);
    ctrl_paint_ctrl(p_result, TRUE);
  }
  else
  {
    rate_info.rate = (u8)focus;  
    UI_PRINTF("[ca_accept_level]rate_info.rate= %d \n",rate_info.rate);
    OS_PRINTF("PIN发出去rate_info的值是%s\n",rate_info.pin);
    ui_ca_do_cmd((u32)CAS_CMD_RATING_SET, (u32)&rate_info, 0);
  }
  ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_pin), MSG_EMPTY, 0, 0);

  ctrl_paint_ctrl(p_pin, TRUE);
  return SUCCESS;
}

BEGIN_KEYMAP(conditional_accept_level_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_level_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_level_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_CARD_INFO, on_conditional_accept_level_update)
  ON_COMMAND(MSG_CA_RATING_SET, on_conditional_accept_level_set)
END_MSGPROC(conditional_accept_level_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_level_pwdedit_keymap, ui_comm_edit_keymap)
    ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
    ON_EVENT(V_KEY_HOT_XDEL, MSG_BACKSPACE)
END_KEYMAP(conditional_accept_level_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(conditional_accept_level_pwdedit_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_MAXTEXT, on_conditional_accept_level_pwdedit_maxtext)
END_MSGPROC(conditional_accept_level_pwdedit_proc, ui_comm_edit_proc)

BEGIN_KEYMAP(conditional_accept_level_enter_keymap, NULL)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(conditional_accept_level_enter_keymap, NULL)

BEGIN_MSGPROC(conditional_accept_level_enter_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_conditional_accept_level_ca_frm_ok)
END_MSGPROC(conditional_accept_level_enter_proc, text_class_proc)


