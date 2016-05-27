/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_sleep_hotkey.h"

#include "ui_sleep_timer.h"

#include "ui_pause.h"

enum control_id
{
	IDC_INVALID = 0,
	IDC_SLEEP_HKEY_SWITCH,
};

u16 hkey_sleep_keymap(u16 key);
static  RET_CODE hkey_sleep_proc(control_t *p_ctrl, u16 msg,
																 u32 para1, u32 para2);


void sleep_hotkey_set_content(u8 focus)
{
	u16 strid[SLEEP_CNT]=
	{
		IDS_OFF, IDS_10MIN, IDS_30MIN,
		IDS_60MIN, IDS_90MIN, IDS_120MIN
	};
	control_t *p_cont = NULL, *p_switch = NULL;
	osd_set_t osd_set = {0};

	MT_ASSERT(focus < SLEEP_CNT);

	sys_status_get_osd_set(&osd_set);

	p_cont = fw_find_root_by_id(ROOT_ID_SLEEP_HOTKEY);

	if(p_cont != NULL)
	{
		p_switch = ctrl_get_child_by_id(p_cont, IDC_SLEEP_HKEY_SWITCH);

		text_set_content_type(p_switch, TEXT_STRTYPE_UNICODE);  
		text_set_content_by_strid(p_switch, strid[focus]);
		ctrl_paint_ctrl(p_switch, TRUE);

		sys_status_get_osd_set(&osd_set);

		fw_tmr_reset(ROOT_ID_SLEEP_HOTKEY, MSG_EXIT, osd_set.timeout * 2000);
	}
	else
	{
		open_sleep_hotkey(focus, 0);

		fw_tmr_create(ROOT_ID_SLEEP_HOTKEY, MSG_EXIT, osd_set.timeout * 2000, FALSE);

	}
}

RET_CODE open_sleep_hotkey(u32 para1, u32 para2)
{
	control_t *p_cont= NULL, *p_switch = NULL;
	BOOL is_sleep_on = FALSE;
	sys_status_t *sys_data = NULL;
	u8 i;
	u16 strid[SLEEP_CNT]=
	{
		IDS_OFF, IDS_10MIN, IDS_30MIN,
		IDS_60MIN, IDS_90MIN, IDS_120MIN
	};

	sys_data = sys_status_get();

	/* pre-create */
	p_cont = fw_create_mainwin(ROOT_ID_SLEEP_HOTKEY, 
		SLEEP_HOTKEY_CONT_X, SLEEP_HOTKEY_CONT_Y, 
		SLEEP_HOTKEY_CONT_W, SLEEP_HOTKEY_CONT_H,
		0, 0, OBJ_ATTR_ACTIVE, 0);
	if (p_cont == NULL)
	{
		return ERR_FAILURE;
	}

	ctrl_set_rstyle(p_cont, RSI_SLEEP_HOTKEY_FRM, RSI_SLEEP_HOTKEY_FRM, RSI_SLEEP_HOTKEY_FRM);
	//ctrl_set_keymap(p_cont, hkey_sleep_keymap);

	/* switch */
	p_switch = ui_comm_select_create(p_cont, IDC_SLEEP_HKEY_SWITCH,
		SLEEP_HOTKEY_ITEM_X, SLEEP_HOTKEY_ITEM_Y, SLEEP_HOTKEY_ITEM_LW, SLEEP_HOTKEY_ITEM_LW);

	ui_comm_static_set_static_txt(p_switch, IDS_AUTO_SLEEP);
	ui_comm_select_set_param(p_switch, TRUE,
		CBOX_WORKMODE_STATIC, SLEEP_CNT,
		CBOX_ITEM_STRTYPE_STRID, NULL);
	for( i = 0; i < SLEEP_CNT; i++ )
	{
		ui_comm_select_set_content(p_switch, i, strid[i]);
	}

	ui_comm_ctrl_set_keymap(p_switch, hkey_sleep_keymap);
	ui_comm_ctrl_set_proc(p_switch, hkey_sleep_proc);

	sys_status_get_status(BS_SLEEP_LOCK, &is_sleep_on);
	if(!is_sleep_on)
	{
		ui_comm_select_set_focus(p_switch, 0);
	}
	else
	{
		if(sys_data->auto_sleep <= 6 && sys_data->auto_sleep > 0)
		{
			ui_comm_select_set_focus(p_switch, sys_data->auto_sleep);
		}
		else
		{
			ui_comm_select_set_focus(p_switch, 0);
		}
	}

	// MT_ASSERT(para1 < SLEEP_CNT);
	// text_set_content_by_strid(p_switch, strid[para1]);  

	ctrl_default_proc(p_switch, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

	return SUCCESS;
}

static RET_CODE on_hkey_sleep_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

	fw_destroy_mainwin_by_id(ROOT_ID_SLEEP_HOTKEY);
//	fw_tmr_destroy(ROOT_ID_SLEEP_HOTKEY, MSG_CANCEL);

   if(ui_is_pause())
   {
     open_pause(0, 0);
   }
	return SUCCESS;
}

static RET_CODE on_hkey_sleep_change_focus(control_t *p_ctrl, u16 msg, 
																					 u32 para1, u32 para2)
{
	control_t *p_cont,  *p_switch;
	u16 focus;
	sys_status_t *sys_data = NULL;
  osd_set_t osd_set = {0};

  sys_status_get_osd_set(&osd_set);
	cbox_class_proc(p_ctrl, msg, 0, 0);
	p_switch = ctrl_get_parent(p_ctrl);
	p_cont = ctrl_get_parent(p_switch);
	focus = cbox_static_get_focus(p_ctrl);
	sys_data = sys_status_get();
	sys_data->auto_sleep = focus;
	if(focus == 0)
	{
		sys_status_set_status(BS_SLEEP_LOCK, FALSE);
		ui_sleep_timer_destory();
	}
	else
	{
		sys_status_set_status(BS_SLEEP_LOCK, TRUE);
		ui_sleep_timer_create();
	}

	sys_status_save();
	ctrl_paint_ctrl(p_switch, TRUE);
  fw_tmr_reset(ROOT_ID_SLEEP_HOTKEY, MSG_EXIT, osd_set.timeout * 2000);

	return SUCCESS;
}


BEGIN_KEYMAP(hkey_sleep_keymap, NULL)
ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
ON_EVENT(V_KEY_SLEEP,MSG_CANCEL)
ON_EVENT(V_KEY_MENU, MSG_CANCEL)
END_KEYMAP(hkey_sleep_keymap, NULL)

BEGIN_MSGPROC(hkey_sleep_proc, cbox_class_proc)
ON_COMMAND(MSG_INCREASE, on_hkey_sleep_change_focus)
ON_COMMAND(MSG_DECREASE, on_hkey_sleep_change_focus)
ON_COMMAND(MSG_CANCEL, on_hkey_sleep_exit)
END_MSGPROC(hkey_sleep_proc, cbox_class_proc)

