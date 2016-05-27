/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#include "ui_common.h"
#include "ui_ca_prompt.h"
#include "ui_new_mail.h"
#include "ui_ca_public.h"
enum ca_prompt_cont_id
{
	IDC_MESSAGE_FRM = 1,
};

enum ca_prompt_ctrl_id
{
	IDC_MESSAGE_TITLE = 1,
	IDC_MESSAGE_CONTENT,  

	IDC_MESSAGE_DETAIL_INFO_BG,

	IDC_MESSAGE_RECEIVE_TIME_TITLE,
	IDC_MESSAGE_RECEIVE_TIME_CONTENT,

	IDC_MESSAGE_NEW_TITLE,
	IDC_MESSAGE_NEW_CONTENT,

	IDC_MESSAGE_IMPORT_TITLE,
	IDC_MESSAGE_IMPORT_CONTENT,
};

extern RET_CODE asc_to_dec(const u8 *p_ascstr, u32 *p_dec_number, u32 max_len);
static prompt_type_t prompt_type;
static cas_mail_headers_t mail_header;
u16 uni_str[CAS_MAIL_BODY_MAX_LEN + 1] = {0};

u16 ca_prompt_root_keymap(u16 key);
RET_CODE ca_prompt_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE email_body_text_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 email_body_text_keymap(u16 key);

static RET_CODE ca_prompt_email_set_content(control_t *p_cont, u16 index, cas_mail_headers_t *p_mail_header, 
  														cas_mail_body_t *p_cas_mail_body)
{
	control_t *p_frm =  ctrl_get_child_by_id(p_cont, IDC_MESSAGE_FRM);
	control_t *p_message_content = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_CONTENT);
	u32 new_mail = 0;
	u32 i;
  
	for(i = 0; i < p_mail_header->max_num; i++)
	{
		if(p_mail_header->p_mail_head[i].new_email&& i != index)
			new_mail++;
	}

	if(new_mail == 0)
	{
		if(ui_is_new_mail())
		{
			close_new_mail();
			ui_set_new_mail(FALSE);
		}
	}
	if(strlen((const char *)p_cas_mail_body->data) == 0)
	{
		OS_PRINTF("err mail body \n");
		return ERR_FAILURE;
	}
	if(p_cas_mail_body)
	{
		gb2312_to_unicode((u8*)p_cas_mail_body->data, strlen((const char *)p_cas_mail_body->data), uni_str, (sizeof(uni_str)/sizeof(uni_str[0]) - 1));
		OS_PRINTF("data[20]:%d, data[21]:%d, data[22]:%d, data[23]:%d, data[24]:%d, data[25]:%d, data[26]:%d, data[27]:%d, data[28]:%d, data[29]:%d, data[30]:%d, data[31]:%d\n",  \
		               p_cas_mail_body->data[20], p_cas_mail_body->data[21],   \
		              p_cas_mail_body->data[22], p_cas_mail_body->data[23],p_cas_mail_body->data[24],p_cas_mail_body->data[25],   \
		               p_cas_mail_body->data[26], p_cas_mail_body->data[27],  \
		              p_cas_mail_body->data[28], p_cas_mail_body->data[29],p_cas_mail_body->data[30],p_cas_mail_body->data[31]);
		text_set_content_by_extstr(p_message_content, uni_str);
	}
	text_reset_param(p_message_content);

	return SUCCESS;
}

static RET_CODE on_ca_mail_update_body(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	OS_PRINTF("on_ca_mail_update_body p_prompt_type->index=%d p_mail_header->max_num=%d para1= %d \n",prompt_type.index, mail_header.max_num,para1);
	if(para1 != SUCCESS)
		return ERR_FAILURE;
	ca_prompt_email_set_content(p_cont, (u16)prompt_type.index,  &mail_header, (cas_mail_body_t *)para2);
	ctrl_paint_ctrl(p_cont, TRUE);
	ui_ca_do_cmd(CAS_CMD_MAIL_CHANGE_STATUS, mail_header.p_mail_head[prompt_type.index].m_id, 0);
	return SUCCESS;
}
  
RET_CODE open_ca_prompt(u32 para1, u32 para2)
{
	control_t *p_cont,*p_ctrl,*p_frm;
	#if 0
	utc_time_t	s_ca_time = {0};
	u32 tmp_value = 0;
	u8 asc_str[64];
	#endif
	u16 uni_str[CAS_MAIL_SUBJECT_MAX_LEN + 1] = {0};

	// check for close
	if(fw_find_root_by_id(ROOT_ID_CA_PROMPT) != NULL)
	{
		fw_destroy_mainwin_by_id(ROOT_ID_CA_PROMPT);
	}
 
	memcpy(&prompt_type, (prompt_type_t *)para1, sizeof(prompt_type_t));
	OS_PRINTF("open_ca_prompt index=%d, type=%d\n",prompt_type.index, prompt_type.message_type);

	p_cont = ui_comm_right_root_create(ROOT_ID_CA_PROMPT, 0, OBJ_ATTR_ACTIVE, 0);
	MT_ASSERT(NULL!=p_cont);
	ctrl_set_keymap(p_cont, ca_prompt_root_keymap);
	ctrl_set_proc(p_cont, ca_prompt_root_proc);

	//frm
	p_frm = ctrl_create_ctrl(CTRL_CONT, (u8)(IDC_MESSAGE_FRM),
	                        CA_PROMPT_FRM_X, CA_PROMPT_FRM_Y, CA_PROMPT_FRM_W, CA_PROMPT_FRM_H, p_cont, 0);

	//message title
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_MESSAGE_TITLE),
	                        CA_PROMPT_TITLE_X, CA_PROMPT_TITLE_Y, CA_PROMPT_TITLE_W, CA_PROMPT_TITLE_H, p_frm, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

	//message content
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_MESSAGE_CONTENT),
	                        CA_PROMPT_CONTERNT_X, CA_PROMPT_CONTERNT_Y, CA_PROMPT_CONTERNT_W, CA_PROMPT_CONTERNT_H, p_frm, 0);
	ctrl_set_keymap(p_ctrl, email_body_text_keymap); 
	ctrl_set_proc(p_ctrl, text_class_proc);

	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_align_type(p_ctrl, STL_LEFT | STL_TOP);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_EXTSTR);
	text_set_offset(p_ctrl, 10, 0);
	text_enable_page(p_ctrl, TRUE);

	//message time
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_MESSAGE_RECEIVE_TIME_TITLE),
	                        CA_PROMPT_PROMPT_SENTER_TITLE_X, CA_PROMPT_PROMPT_Y, CA_PROMPT_PROMPT_SENTER_TITLE_W, CA_PROMPT_PROMPT_H, p_frm, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_CA_SENDER);

	//message sender
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_MESSAGE_RECEIVE_TIME_CONTENT),
	                        CA_PROMPT_PROMPT_SENTER_X, CA_PROMPT_PROMPT_Y, CA_PROMPT_PROMPT_SENTER_W, CA_PROMPT_PROMPT_H, p_frm, 0);
	MT_ASSERT(NULL!=p_ctrl);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

	//message new title
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_MESSAGE_NEW_TITLE),
	                        CA_PROMPT_PROMPT_NEW_TITLE_X, CA_PROMPT_PROMPT_Y, CA_PROMPT_PROMPT_NEW_TITLE_W, CA_PROMPT_PROMPT_H, p_frm, 0);
	MT_ASSERT(NULL!=p_ctrl);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_CA_NEW_NOT);

	//message new content
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_MESSAGE_NEW_CONTENT),
	                        CA_PROMPT_PROMPT_NEW_X, CA_PROMPT_PROMPT_Y, CA_PROMPT_PROMPT_NEW_W, CA_PROMPT_PROMPT_H, p_frm, 0);
	MT_ASSERT(NULL!=p_ctrl);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);

	//message import title
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_MESSAGE_IMPORT_TITLE),
	                        CA_PROMPT_PROMPT_IMPORT_TITLE_X, CA_PROMPT_PROMPT_Y, CA_PROMPT_PROMPT_IMPORT_TITLE_W, CA_PROMPT_PROMPT_H, p_frm, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_CA_IMPORT);

	//message import content
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_MESSAGE_IMPORT_CONTENT),
	                        CA_PROMPT_PROMPT_IMPORT_X, CA_PROMPT_PROMPT_Y, CA_PROMPT_PROMPT_IMPORT_W, CA_PROMPT_PROMPT_H, p_frm, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);

	ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);

#ifndef WIN32
	//set content
	memcpy(&mail_header, (cas_mail_headers_t *)para2, sizeof(cas_mail_headers_t));

	p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_TITLE);
	MT_ASSERT(NULL!=p_ctrl);
	gb2312_to_unicode((u8*)mail_header.p_mail_head[prompt_type.index].subject, CAS_MAIL_SUBJECT_MAX_LEN, 
	                                                                uni_str, CAS_MAIL_SUBJECT_MAX_LEN +  1);
	text_set_content_by_unistr(p_ctrl, uni_str);

	p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_RECEIVE_TIME_CONTENT);
	MT_ASSERT(NULL!=p_ctrl);
	#if 0
	sprintf((char *)asc_str,"%02x", mail_header.p_mail_head[prompt_type.index].creat_date[0]);
	asc_to_dec(asc_str, &tmp_value, 2);
	s_ca_time.year = (u8)tmp_value;
	sprintf((char *)asc_str,"%02x", mail_header.p_mail_head[prompt_type.index].creat_date[1]);
	asc_to_dec(asc_str, &tmp_value, 2);
	s_ca_time.year = s_ca_time.year*100 + (u8)tmp_value;
	
	sprintf((char *)asc_str,"%02x", mail_header.p_mail_head[prompt_type.index].creat_date[2]);
	asc_to_dec(asc_str, &tmp_value, 2);
	s_ca_time.month = (u8)tmp_value;
	sprintf((char *)asc_str,"%02x", mail_header.p_mail_head[prompt_type.index].creat_date[3]);
	asc_to_dec(asc_str, &tmp_value, 2);
	s_ca_time.day = (u8)tmp_value;
	
	sprintf((char *)asc_str,"%02x", mail_header.p_mail_head[prompt_type.index].creat_date[4]);
	asc_to_dec(asc_str, &tmp_value, 2);
	s_ca_time.hour = (u8)tmp_value;
	sprintf((char *)asc_str,"%02x", mail_header.p_mail_head[prompt_type.index].creat_date[5]);
	asc_to_dec(asc_str, &tmp_value, 2);
	s_ca_time.minute = (u8)tmp_value;

	sprintf((char *)asc_str,"%04d/%02d/%02d %02d:%02d", 
			  s_ca_time.year, s_ca_time.month, 
			  s_ca_time.day, s_ca_time.hour, s_ca_time.minute);
	str_asc2uni(asc_str, uni_str);
	#endif
	gb2312_to_unicode(mail_header.p_mail_head[prompt_type.index].sender, 80, uni_str, 40);
	text_set_content_by_unistr(p_ctrl, uni_str);

	p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_NEW_CONTENT);
	MT_ASSERT(NULL!=p_ctrl);

	if(mail_header.p_mail_head[prompt_type.index].new_email)
		text_set_content_by_strid(p_ctrl, IDS_YES);
	else
		text_set_content_by_strid(p_ctrl, IDS_NO);

	p_ctrl = ctrl_get_child_by_id(p_frm, IDC_MESSAGE_IMPORT_CONTENT);
	MT_ASSERT(NULL!=p_ctrl);

	if(mail_header.p_mail_head[prompt_type.index].priority)
		text_set_content_by_strid(p_ctrl, IDS_YES);
	else
		text_set_content_by_strid(p_ctrl, IDS_NO);

	ui_ca_do_cmd(CAS_CMD_MAIL_BODY_GET, mail_header.p_mail_head[prompt_type.index].m_id, 0);
#endif

	ctrl_default_proc(ctrl_get_child_by_id(p_frm, IDC_MESSAGE_CONTENT), MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(p_cont, FALSE);
	return SUCCESS;
}

static RET_CODE on_ca_prompt_exit(control_t *p_cont,u16 msg,u32 para1,u32 para2)
{
	ui_ca_do_cmd(CAS_CMD_MAIL_HEADER_GET, 0, 0);
	return ERR_NOFEATURE;
}

BEGIN_KEYMAP(ca_prompt_root_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_INCREASE)    
	ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
	ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
	ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
END_KEYMAP(ca_prompt_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_prompt_root_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_EXIT, on_ca_prompt_exit)
	ON_COMMAND(MSG_EXIT_ALL, on_ca_prompt_exit)
	ON_COMMAND(MSG_CA_MAIL_BODY_INFO, on_ca_mail_update_body)
END_MSGPROC(ca_prompt_root_proc, ui_comm_root_proc)

BEGIN_KEYMAP(email_body_text_keymap, NULL)
	ON_EVENT(V_KEY_UP, MSG_INCREASE)    
	ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
	ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
	ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
END_KEYMAP(email_body_text_keymap, NULL)


