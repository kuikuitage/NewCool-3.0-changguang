/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_accept_pin.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_PIN,
};

enum ca_pin_modify_id
{
  IDC_CA_PIN_MODIFY_PSW_OLD = 1,
  IDC_CA_PIN_MODIFY_PSW_NEW,
  IDC_CA_PIN_MODIFY_PSW_CONFIRM,
  IDC_CA_PIN_MODIFY_PSW_OK,
  IDC_CA_PIN_CHANGE_RESULT,
};


u16 conditional_accept_pin_cont_keymap(u16 key);
RET_CODE conditional_accept_pin_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 conditional_accept_pin_ca_frm_keymap(u16 key);
RET_CODE conditional_accept_pin_ca_frm_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

u16 conditional_accept_pin_pwdedit_keymap(u16 key);
RET_CODE conditional_accept_pin_pwdedit_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);



RET_CODE open_conditional_accept_pin(u32 para1, u32 para2)
{
    control_t *p_ctrl=NULL;
    control_t *p_cont;
    control_t *p_ca_frm;
    u8 i;
    u16 y;
    u16 stxt_pin[CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT] =
    { 
        IDS_CA_INPUT_OLD_PIN, IDS_CA_INPUT_NEW_PIN, IDS_CA_CONFIRM_NEW_PIN,
    };

    p_cont = ui_comm_right_root_create(ROOT_ID_CONDITIONAL_ACCEPT_PIN, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
    if(p_cont == NULL)
    {
        return ERR_FAILURE;
    }
    ctrl_set_keymap(p_cont, conditional_accept_pin_cont_keymap);
    ctrl_set_proc(p_cont, conditional_accept_pin_cont_proc);

    //CA frm
    p_ca_frm = ctrl_create_ctrl(CTRL_CONT, (u8)IDC_FRM_PIN,
                                              CONDITIONAL_ACCEPT_PIN_CA_FRM_X, CONDITIONAL_ACCEPT_PIN_CA_FRM_Y,
                                              CONDITIONAL_ACCEPT_PIN_CA_FRM_W, CONDITIONAL_ACCEPT_PIN_CA_FRM_H,
                                              p_cont, 0);
    ctrl_set_rstyle(p_ca_frm, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_ca_frm, conditional_accept_pin_ca_frm_keymap);
    ctrl_set_proc(p_ca_frm, conditional_accept_pin_ca_frm_proc);

    //pin modify
    y = CONDITIONAL_ACCEPT_PIN_PIN_ITEM_Y;
    for (i = 0; i<CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT; i++)
    {
        switch(i)
        {
            case 0:
            case 1:
            case 2:
                p_ctrl = ui_comm_pwdedit_create(p_ca_frm, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD + i),
                                                     CONDITIONAL_ACCEPT_PIN_PIN_ITEM_X, y+(CONDITIONAL_ACCEPT_PIN_PIN_ITEM_H + CONDITIONAL_ACCEPT_PIN_PIN_ITEM_V_GAP)*i,
                                                     CONDITIONAL_ACCEPT_PIN_PIN_ITEM_LW,
                                                     CONDITIONAL_ACCEPT_PIN_PIN_ITEM_RW);
                ui_comm_pwdedit_set_static_txt(p_ctrl, stxt_pin[i]);
                ui_comm_pwdedit_set_param(p_ctrl, 6);
                //ui_comm_pwdedit_set_mask(p_ctrl, '-', '*');
                ui_comm_ctrl_set_keymap(p_ctrl,conditional_accept_pin_pwdedit_keymap);
                ui_comm_ctrl_set_proc(p_ctrl, conditional_accept_pin_pwdedit_proc);
                break;
            case 3:
                p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD + i),
                                      CONDITIONAL_ACCEPT_PIN_PIN_ITEM_X,
                                      y+(CONDITIONAL_ACCEPT_PIN_PIN_ITEM_H + CONDITIONAL_ACCEPT_PIN_PIN_ITEM_V_GAP)*(3),
                                      (CONDITIONAL_ACCEPT_PIN_PIN_ITEM_LW + CONDITIONAL_ACCEPT_PIN_PIN_ITEM_RW),
                                      CONDITIONAL_ACCEPT_PIN_OK_ITEM_H,
                                      p_ca_frm, 0);
                ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_SELECT_F, RSI_PBACK);
				text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
				text_set_offset(p_ctrl, COMM_CTRL_OX, 0);
                text_set_font_style(p_ctrl,FSI_WHITE_24, FSI_DLG_BTN_HL, FSI_WHITE_24);
                text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
                text_set_content_by_strid(p_ctrl, IDS_START);

                break;

            default:  
                break;
        }
        
        ctrl_set_related_id(p_ctrl,
                          0,                                     /* left */
                          (u8)((i - 1 +
                                CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT) %
                               CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT + IDC_CA_PIN_MODIFY_PSW_OLD),           /* up */
                          0,                                     /* right */
                          (u8)((i + 1) % CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT + IDC_CA_PIN_MODIFY_PSW_OLD));/* down */
    }

    //change result
    p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_PIN_CHANGE_RESULT,
                              CONDITIONAL_ACCEPT_CHANGE_RESULT_X, 
                              CONDITIONAL_ACCEPT_CHANGE_RESULT_Y,
                              CONDITIONAL_ACCEPT_CHANGE_RESULT_W, 
                              CONDITIONAL_ACCEPT_CHANGE_RESULT_H,
                              p_ca_frm, 0);
    ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
    text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
    text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);  
  //text_set_content_by_strid(p_ctrl, IDS_SET_SUCCESS);
  
  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

    ctrl_default_proc(ctrl_get_child_by_id(p_ca_frm, IDC_CA_PIN_MODIFY_PSW_OLD), MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
    return SUCCESS;
}

static RET_CODE on_conditional_accept_pin_pwdedit_maxtext(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
    control_t *p_cont;
    control_t *p_active;

    p_active = ctrl_get_parent(p_ctrl);
    p_cont = ctrl_get_parent(p_active);
    ctrl_process_msg(p_cont, MSG_FOCUS_DOWN, 0, 0);
    ctrl_paint_ctrl(ui_comm_ctrl_get_ctrl(p_cont),TRUE);
    return SUCCESS;
}

static RET_CODE on_conditional_accept_pin_ca_frm_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
#define	UI_MAX_PIN_LEN	6
  u32 enable_psw_pinold, enable_psw_pinnew ,enable_psw_pinnew_confirm;
  control_t *p_result, *p_ctrl_temp;
  u32 pin_new_psw,pin_old_psw;
  u32 pin_new_psw_confirm;
  cas_pin_modify_t pin_modify;
  u8 pin = 0;
  s8 i = 0;

  //change result
  p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_CHANGE_RESULT);
  p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_MODIFY_PSW_OLD);
  pin_old_psw = ui_comm_pwdedit_get_realValue(p_ctrl_temp);
  enable_psw_pinold = ui_comm_pwdedit_is_full(p_ctrl_temp);

  p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_MODIFY_PSW_NEW);
  pin_new_psw = ui_comm_pwdedit_get_realValue(p_ctrl_temp);
  enable_psw_pinnew = ui_comm_pwdedit_is_full(p_ctrl_temp);

  p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_MODIFY_PSW_CONFIRM);
  pin_new_psw_confirm = ui_comm_pwdedit_get_realValue(p_ctrl_temp);
  enable_psw_pinnew_confirm = ui_comm_pwdedit_is_full(p_ctrl_temp);

  if(FALSE == ui_is_smart_card_insert())
  {
  	text_set_content_by_strid(p_result, IDS_CA_CARD_NOT_INSERT);
    ctrl_paint_ctrl(p_result, TRUE);
	return SUCCESS;
  }

  if(TRUE != enable_psw_pinold || TRUE != enable_psw_pinnew || TRUE != enable_psw_pinnew_confirm)
  {
	  text_set_content_by_strid(p_result, IDS_CA_PIN_INVALID);
	  ctrl_set_sts(p_result,OBJ_STS_SHOW);
	  ctrl_paint_ctrl(p_result, TRUE); 
  }
  else
  {
	  if(pin_new_psw == pin_new_psw_confirm)
	  {
		for(i = UI_MAX_PIN_LEN -1; i >= 0; i--)
		{
		  pin = pin_old_psw%10;
		  pin_modify.old_pin[i] = pin;
		  pin_old_psw = pin_old_psw/10;
		}
		
		for(i = UI_MAX_PIN_LEN - 1; i >= 0; i--)
		{
		  pin = pin_new_psw%10;
		  pin_modify.new_pin[i] = pin;
		  pin_new_psw = pin_new_psw/10;
		}
		ui_ca_do_cmd((u32)CAS_CMD_PIN_SET, (u32)&pin_modify, 0);
	  }
	  else
	  {
		text_set_content_by_strid(p_result, IDS_CA_PIN_INVALID);
		ctrl_paint_ctrl(p_result, TRUE);
	  }
  }

  for(i=0; i<3; i++)
  {
    p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, (u8)(IDC_CA_PIN_MODIFY_PSW_OLD+i));
    ctrl_process_msg(ui_comm_ctrl_get_ctrl(p_ctrl_temp), MSG_EMPTY, 0, 0);
    ctrl_paint_ctrl(ui_comm_ctrl_get_ctrl(p_ctrl_temp), TRUE);
  }
  
  return SUCCESS;
}

static RET_CODE on_conditional_accept_pin_ca_modify(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_result;
  RET_CODE ret;
  ret = (RET_CODE)para1;
  
  p_result = ctrl_get_child_by_id(p_ctrl, IDC_CA_PIN_CHANGE_RESULT);

  //when set repeatly, clear previous show
  text_set_content_by_strid(p_result, RSC_INVALID_ID);
  
  if(ret == SUCCESS)
  {
    text_set_content_by_strid(p_result, IDS_CA_SET_SUCCESS);
  }
  else
  {
    text_set_content_by_strid(p_result, IDS_CA_PASSWORD_ERROR);
  }
  ctrl_paint_ctrl(p_result, TRUE);
  
  return SUCCESS;
}


BEGIN_KEYMAP(conditional_accept_pin_cont_keymap, ui_comm_root_keymap)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(conditional_accept_pin_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(conditional_accept_pin_cont_proc, ui_comm_root_proc)

END_MSGPROC(conditional_accept_pin_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_pin_ca_frm_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(conditional_accept_pin_ca_frm_keymap, NULL)

BEGIN_MSGPROC(conditional_accept_pin_ca_frm_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_SELECT, on_conditional_accept_pin_ca_frm_ok)  
    ON_COMMAND(MSG_CA_PIN_SET_INFO, on_conditional_accept_pin_ca_modify)
END_MSGPROC(conditional_accept_pin_ca_frm_proc, ui_comm_root_proc)

BEGIN_KEYMAP(conditional_accept_pin_pwdedit_keymap, ui_comm_edit_keymap)
    ON_EVENT(V_KEY_LEFT, MSG_BACKSPACE)
    ON_EVENT(V_KEY_HOT_XDEL, MSG_BACKSPACE)
END_KEYMAP(conditional_accept_pin_pwdedit_keymap, ui_comm_edit_keymap)

BEGIN_MSGPROC(conditional_accept_pin_pwdedit_proc, ui_comm_edit_proc)
    ON_COMMAND(MSG_MAXTEXT, on_conditional_accept_pin_pwdedit_maxtext)
END_MSGPROC(conditional_accept_pin_pwdedit_proc, ui_comm_edit_proc)



