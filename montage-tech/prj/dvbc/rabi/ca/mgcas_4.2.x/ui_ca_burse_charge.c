/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ca_public.h"
#include "ui_ca_burse_charge.h"

enum email_ctrl_id
{
	IDC_INVALID = 0,
	IDC_CONT,
	IDC_TITLE,
	IDC_CONTENT,
};

static u16 ca_burse_keymap(u16 key);
static RET_CODE ca_burse_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static void get_burse_str(u16 * uni_str, u16 str_id1,u16 str_id2, u32 content)
{
	u16 str[10] = {0}; 
	u16 len = 0;

	gui_get_string(str_id1, uni_str, 64);
	convert_i_to_dec_str(str, (s32)content); 
	uni_strcat(uni_str, str,64);

	len = (u16)uni_strlen(uni_str);
	gui_get_string(str_id2, str, 64 - len); 
	uni_strcat(uni_str, str, 64); 

	return;
}

RET_CODE open_ca_burse_dlg(u32 para1, u32 para2)
{
	control_t *p_content, *p_title,*p_cont = NULL;
	u16 uni_str[64] = {0};
	cas_burse_charge_t *charge = (cas_burse_charge_t*)para2;

	if((!ui_is_fullscreen_menu(fw_get_focus_id()) )|| (fw_find_root_by_id(ROOT_ID_CA_BURSE_CHARGE) != NULL))
	{
		return ERR_FAILURE;
	}

	p_cont=  fw_create_mainwin(ROOT_ID_CA_BURSE_CHARGE, 
	                           BURSE_CONT_FULL_X, BURSE_CONT_FULL_Y,
	                           BURSE_CONT_FULL_W, BURSE_CONT_FULL_H,
	                           ROOT_ID_INVALID, 0,OBJ_ATTR_ACTIVE, 0);

	if (p_cont == NULL)
	{
		return ERR_FAILURE;
	}  
  
	ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	ctrl_set_keymap(p_cont, ca_burse_keymap);
	ctrl_set_proc(p_cont, ca_burse_proc);

	p_cont = ctrl_create_ctrl(CTRL_TEXT, IDC_CONT,
	                     0, 0,
	                     BURSE_CONT_FULL_W,BURSE_CONT_FULL_H,
	                     p_cont, 0);
	ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);

	
	//title
	p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_TITLE,
	                     BURSE_TITLE_FULL_X, BURSE_TITLE_FULL_Y,
	                     BURSE_TITLE_FULL_W,BURSE_TITLE_FULL_H,
	                     p_cont, 0);
	ctrl_set_rstyle(p_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_content_type(p_title, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_title, IDS_CA_SLOT_INFO);


	//content
	p_content = ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
	                   BURSE_CONTENT_FULL_X, BURSE_CONTENT_FULL_Y,
	                   BURSE_CONTENT_FULL_W, BURSE_CONTENT_FULL_H,
	                   p_cont, 0);
	ctrl_set_rstyle(p_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_content, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_align_type(p_content, STL_CENTER | STL_VCENTER); 
	text_set_content_type(p_content, TEXT_STRTYPE_UNICODE);
	get_burse_str(uni_str,IDS_CA_E_CHARGE, IDS_CA_POINT,charge->charge_value);
	text_set_content_by_unistr(p_content, uni_str);


	fw_tmr_create(ROOT_ID_CA_BURSE_CHARGE, MSG_TIMER_EXPIRED, BURSE_TIME_OUT, FALSE);
	#ifdef SPT_DUMP_DATA
	spt_dump_menu_data(p_cont);
	#endif
	if(p_cont != NULL)
	{
		ctrl_paint_ctrl(p_cont, FALSE);
	}
	return SUCCESS;
}

void close_ca_burse_dlg(void)
{
	fw_destroy_mainwin_by_id(ROOT_ID_CA_BURSE_CHARGE);
	fw_tmr_destroy(ROOT_ID_CA_BURSE_CHARGE, MSG_TIMER_EXPIRED);
}

static RET_CODE on_ca_burse_timeout(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	close_ca_burse_dlg();
	return SUCCESS;
}

static RET_CODE on_ca_burse_change_channel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u16 prog_id = 0;
	switch (msg)
	{
		case MSG_FOCUS_UP:
			ui_shift_prog(1, TRUE, &prog_id);
			break;
		case MSG_FOCUS_DOWN:
			ui_shift_prog(-1, TRUE, &prog_id);
			break;
		default:
			break;
	}

	close_ca_burse_dlg();

	return SUCCESS;
}

BEGIN_KEYMAP(ca_burse_keymap, NULL)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
	ON_EVENT(V_KEY_EXIT, MSG_EXIT)
	ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ca_burse_keymap, NULL)

BEGIN_MSGPROC(ca_burse_proc, cont_class_proc)
	ON_COMMAND(MSG_TIMER_EXPIRED, on_ca_burse_timeout)
	ON_COMMAND(MSG_EXIT, on_ca_burse_timeout)
	ON_COMMAND(MSG_FOCUS_UP,on_ca_burse_change_channel)
	ON_COMMAND(MSG_FOCUS_DOWN,on_ca_burse_change_channel)
END_MSGPROC(ca_burse_proc, cont_class_proc)



