#include "ui_common.h"
#include "ui_ca_public.h"
#include "ui_ca_feed.h"


enum ca_feed_id
{
	IDC_CA_FEED_MESSAGE = 1,
	IDC_CA_FEED_BUTTON_START,
	IDC_CA_FEED_BUTTON_END,
};

static u8  ca_feed_state  = FALSE;
static u16 ca_feed_string = FALSE;


u16 ca_feed_cont_keymap(u16 key);
RET_CODE ca_feed_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);
u16 ca_feed_button_keymap(u16 key);
RET_CODE ca_feed_button_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);
static RET_CODE on_ca_feed_message_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


void send_message_ca_feed(u32 message_id)
{
	control_t *p_cont = NULL;

	DEBUG(CAS,INFO,"message_id = %d \n",message_id);
	if(fw_get_focus_id() == ROOT_ID_CA_FEED)
	{
		p_cont = ui_comm_right_root_get_root(ROOT_ID_CA_FEED);
		on_ca_feed_message_update(p_cont, 0, 0, message_id);
	}
}

void update_ca_feed_message(control_t *p_cont, u16 str_id)
{
	control_t *p_message = NULL;

	p_message = ctrl_get_child_by_id(p_cont, IDC_CA_FEED_MESSAGE);
	text_set_content_by_strid(p_message,str_id);
	ctrl_paint_ctrl(p_message, TRUE);
}

RET_CODE open_ca_feed(u32 para1, u32 para2)
{
    control_t *p_cont = NULL;
	control_t *p_ctrl = NULL;


    p_cont = ui_comm_right_root_create(ROOT_ID_CA_FEED, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
    if(p_cont == NULL)
    {
        return ERR_FAILURE;
    }
	ctrl_set_keymap(p_cont, ca_feed_cont_keymap);
    ctrl_set_proc(p_cont, ca_feed_cont_proc);

	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_FEED_MESSAGE,
					  CA_FEED_MESSAGE_X,CA_FEED_MESSAGE_Y,
					  CA_FEED_MESSAGE_W,CA_FEED_MESSAGE_H,
					  p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_ctrl, STL_CENTER| STL_VCENTER);
	text_set_font_style(p_ctrl,FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_SUB_MOVE_CARD);
	ca_feed_string = IDS_SUB_MOVE_CARD;

	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_FEED_BUTTON_START,
						  CA_FEED_BUTTON_START_X,
						  CA_FEED_BUTTON_START_Y,
						  CA_FEED_BUTTON_START_W,
						  CA_FEED_BUTTON_START_H,
						  p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_SELECT_F, RSI_PBACK);
	ctrl_set_keymap(p_ctrl, ca_feed_button_keymap);
    ctrl_set_proc(p_ctrl, ca_feed_button_proc);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_font_style(p_ctrl,FSI_WHITE_24, FSI_DLG_BTN_HL, FSI_WHITE_24);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_START);

	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_CA_FEED_BUTTON_END,
						  CA_FEED_BUTTON_END_X,
						  CA_FEED_BUTTON_END_Y,
						  CA_FEED_BUTTON_END_W,
						  CA_FEED_BUTTON_END_H,
						  p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_SELECT_F, RSI_PBACK);
	ctrl_set_keymap(p_ctrl, ca_feed_button_keymap);
    ctrl_set_proc(p_ctrl, ca_feed_button_proc);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_font_style(p_ctrl,FSI_WHITE_24, FSI_DLG_BTN_HL, FSI_WHITE_24);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_CANCEL);

	ctrl_default_proc(ctrl_get_child_by_id(p_cont, IDC_CA_FEED_BUTTON_START), MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

	fw_tmr_create(ROOT_ID_CA_FEED,MSG_CANCEL,2000,TRUE);
	return SUCCESS;
}

static RET_CODE on_ca_feed_move(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_cont = NULL;
	control_t *p_active_ctrl = NULL;
	u16 ctrl_id = 0;

	p_cont = ctrl_get_parent(p_ctrl);
	ctrl_id = ctrl_get_ctrl_id(p_ctrl);
	if(ctrl_id == IDC_CA_FEED_BUTTON_START)
	{
		p_active_ctrl = ctrl_get_child_by_id(p_cont,IDC_CA_FEED_BUTTON_END);
	}
	else
	{
		p_active_ctrl = ctrl_get_child_by_id(p_cont,IDC_CA_FEED_BUTTON_START);
	}
	
	ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
	ctrl_process_msg(p_active_ctrl, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(p_cont, TRUE);
	return SUCCESS;
}

static RET_CODE on_ca_feed_button(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u16 ctrl_id = 0;
	control_t *p_cont = NULL;

	ctrl_id = ctrl_get_ctrl_id(p_ctrl);
	p_cont  = ctrl_get_parent(p_ctrl);
	
	if((ctrl_id == IDC_CA_FEED_BUTTON_START)&&(ca_feed_state == FALSE))
	{
		ca_feed_state = TRUE;
		ui_ca_do_cmd(CAS_CMD_MON_CHILD_FEED, (u32)&ca_feed_state, 0);
	}
	else if((ctrl_id == IDC_CA_FEED_BUTTON_END)&&(ca_feed_state == TRUE))
	{
		if(ui_get_smart_card_state())
		{
			update_ca_feed_message(p_cont, IDS_CANCEL_FEED_BEFORE_EXITING);
		}
		else
		{
			ca_feed_state = FALSE;
			ui_ca_do_cmd(CAS_CMD_MON_CHILD_FEED, (u32)&ca_feed_state, 0);
		}
	}
	
	return SUCCESS;
}


static RET_CODE on_ca_feed_mon_child_feed(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	RET_CODE ret = (RET_CODE)para1;

	if(ca_feed_state == TRUE)
	{
		if(ret != SUCCESS)
		{
			ca_feed_string = IDS_SUB_CARD_FEEDING_FAILURE_START;
			update_ca_feed_message(p_ctrl, IDS_SUB_CARD_FEEDING_FAILURE_START);
		}
	}
	else
	{
		if(ret == SUCCESS)
		{
			ca_feed_string = IDS_SUB_CARD_FEEDING_END;
			update_ca_feed_message(p_ctrl, IDS_SUB_CARD_FEEDING_END);
		}
		else
		{
			ca_feed_string = IDS_SUB_CARD_FEEDING_FAILURE_END;
			update_ca_feed_message(p_ctrl, IDS_SUB_CARD_FEEDING_FAILURE_END);
		}
	}

	return SUCCESS;
}

static RET_CODE on_ca_feed_message_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u32 event_id = para2;

	DEBUG(CAS,INFO,"event_id[0x%x] \n", event_id);
	switch(event_id)
	{	
		case CAS_S_CLEAR_DISPLAY:
			update_ca_feed_message(p_ctrl, 0);
		case CAS_S_MOTHER_CARD_REGU_INSERT:
			ca_feed_string = IDS_CAS_S_MOTHER_CARD_REGU_INSERT2;
			update_ca_feed_message(p_ctrl, IDS_CAS_S_MOTHER_CARD_REGU_INSERT2);
			break;
		case CAS_S_MOTHER_CARD_INVALID:
			ca_feed_string = IDS_CAS_S_MOTHER_CARD_INVALID;
			update_ca_feed_message(p_ctrl, IDS_CAS_S_MOTHER_CARD_INVALID);
			break;
		case CAS_S_SON_CARD_REGU_INSERT:
			ca_feed_string = IDS_CAS_S_SON_CARD_REGU_INSERT2;
			update_ca_feed_message(p_ctrl, IDS_CAS_S_SON_CARD_REGU_INSERT2);
			break;  

		case CAS_S_SON_CARD_INVALID:
			ca_feed_string = IDS_CAS_S_SON_CARD_INVALID;
			update_ca_feed_message(p_ctrl, IDS_CAS_S_SON_CARD_INVALID);
			break;
			
		case CAS_S_MOTHER_SON_CARD_NOT_PAIR:
			ca_feed_string = IDS_CAS_S_MOTHER_SON_CARD_NOT_PAIR;
			update_ca_feed_message(p_ctrl, IDS_CAS_S_MOTHER_SON_CARD_NOT_PAIR);
			break;
			
		case CAS_S_MOTHER_CARD_COMM_FAIL:
			ca_feed_string = IDS_CAS_S_MOTHER_CARD_COMM_FAIL;
			update_ca_feed_message(p_ctrl, IDS_CAS_S_MOTHER_CARD_COMM_FAIL);
			break;
			
		case CAS_S_SON_CARD_COMM_FAIL:
			ca_feed_string = IDS_CAS_S_SON_CARD_COMM_FAIL;
			update_ca_feed_message(p_ctrl, IDS_CAS_S_SON_CARD_COMM_FAIL);
			break;
			
		case CAS_S_CARD_NOT_IN_ACTTIME:
			ca_feed_string = IDS_CAS_S_CARD_NOT_IN_ACTTIME;
			update_ca_feed_message(p_ctrl, IDS_CAS_S_CARD_NOT_IN_ACTTIME);
			break;
			
			
		case CAS_S_CARD_INSERT_NEXT_CARD:
			ca_feed_string = IDS_CA_MATCH_SUC_OTHER_CHID_CARD;
			update_ca_feed_message(p_ctrl, IDS_CA_MATCH_SUC_OTHER_CHID_CARD);
			break;
			
		case CAS_S_CARD_STOP_ACTIVE:
			ca_feed_string = IDS_SUB_CARD_FEEDING_INTERRUPTION;
			update_ca_feed_message(p_ctrl, IDS_SUB_CARD_FEEDING_INTERRUPTION);
			break;

		default:
			break;
	}
	return SUCCESS;
}

static RET_CODE on_ca_feed_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	if(ca_feed_state)
	{
		if(ui_get_smart_card_state())
		{
			update_ca_feed_message(p_ctrl, IDS_UNPLUG_CARD_BEFORE_EXITING);
			return SUCCESS;
		}
		ca_feed_state = FALSE;
		ui_ca_do_cmd(CAS_CMD_MON_CHILD_FEED, (u32)&ca_feed_state, 0);
	}

	fw_tmr_destroy(ROOT_ID_CA_FEED,MSG_CANCEL);
	manage_close_menu(ROOT_ID_CA_FEED, 0, 0);
	if(ROOT_ID_XSYS_SET==fw_get_focus_id())
  		swtich_to_sys_set(ROOT_ID_CA_FEED, 0);
	return SUCCESS;
}

static RET_CODE on_ca_feed_update_message(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	update_ca_feed_message(p_ctrl, ca_feed_string);

	return SUCCESS;
}


BEGIN_KEYMAP(ca_feed_cont_keymap, ui_comm_root_keymap)
END_KEYMAP(ca_feed_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_feed_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_ca_feed_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_ca_feed_exit)
  ON_COMMAND(MSG_CA_MON_CHILD_FEED, on_ca_feed_mon_child_feed)
  ON_COMMAND(MSG_CANCEL, on_ca_feed_update_message)
END_MSGPROC(ca_feed_cont_proc, ui_comm_root_proc)


BEGIN_KEYMAP(ca_feed_button_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, 	MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ca_feed_button_keymap, NULL)

BEGIN_MSGPROC(ca_feed_button_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_ca_feed_button)
  ON_COMMAND(MSG_FOCUS_LEFT, on_ca_feed_move)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_ca_feed_move)
END_MSGPROC(ca_feed_button_proc, text_class_proc)


