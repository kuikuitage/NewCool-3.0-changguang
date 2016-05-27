#include "ui_common.h"
#include "ui_email_mess.h"
#include "ui_ca_public.h"

static cas_mail_headers_t *p_mail_data_buf = NULL;
static u8 g_del_mail = 0; 
enum control_id
{
	IDC_INVALID = 0,
	IDC_EMAIL_ITEM1,
	IDC_EMAIL_ITEM2,
	IDC_EMAIL_ITEM3,
	IDC_EMAIL_ITEM4,
	IDC_EMAIL_ITEM5,
	IDC_EMAIL_PRO,
	IDC_EMAIL_BAR,
	IDC_EMAIL_NOTIC,
	IDC_EMAIL_BODY_CONT,
	IDC_EMAIL_TEXT_BAR,
	IDC_EMAIL_TEXT,
	IDC_EMAIL_RECEIVED_HEAD,
	IDC_EMAIL_RECEIVED,
	IDC_EMAIL_RESET_HEAD,
	IDC_EMAIL_RESET,
	IDC_EMAIL_DEL_ICON,
	IDC_EMAIL_DEL_TEXT,
	IDC_EMAIL_DEL_ALL_ICON,
	IDC_EMAIL_DEL_ALL_TEXT,
};

enum nvod_email_mess_local_msg
{
	MSG_DELETE_ONE = MSG_LOCAL_BEGIN + 150,
	MSG_DELETE_ALL,
};

enum read_email_id
{
  READY_TO_READ_MAIL = 0,
};

list_xstyle_t email_item_rstyle =
{
	RSI_PBACK,
	RSI_COMM_CONT_SH,
	RSI_SELECT_F,
	RSI_SELECT_F,
	RSI_SELECT_F,
};

list_xstyle_t emial_plist_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};
list_xstyle_t email_plist_field_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

list_field_attr_t email_plist_attr[EMAIL_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
	  EMAIL_TITLE1_W, EMAIL_TITLE1_X, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },

  { LISTFIELD_TYPE_STRID | STL_CENTER| STL_VCENTER,
	  EMAIL_TITLE2_W, EMAIL_TITLE2_X, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle},

  { LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
	  EMAIL_TITLE3_W, EMAIL_TITLE3_X, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle },

  { LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER ,
	  EMAIL_TITLE4_W, EMAIL_TITLE4_X, 0, &email_plist_field_rstyle,  &emial_plist_field_fstyle},

};


u16 email_plist_list_keymap(u16 key);
RET_CODE email_plist_list_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE asc_to_dec(const u8 *p_ascstr, u32 *p_dec_number, u32 max_len)
{
	u32 i = 0;
	u32 ascStrLen = 0;
	u32 tmp_value = 0;

	if((NULL == p_ascstr) || (NULL == p_dec_number) || (max_len > 8))
	{
		return ERR_FAILURE;
	}

	ascStrLen = strlen((char *)p_ascstr);
	*p_dec_number = 0;

	if((0 == ascStrLen) || (ascStrLen > max_len))
	{
		return ERR_FAILURE;
	}

	for(i = 0; i < ascStrLen; i++)
	{
		if((p_ascstr[i] >= '0') && (p_ascstr[i] <= '9'))
		{
			tmp_value = p_ascstr[i] - '0';
			*p_dec_number = (*p_dec_number) * 10 + tmp_value;
		}
		else
		{
			break;
		}
	}

	return SUCCESS;
}

static RET_CODE email_plist_update(control_t* ctrl, u16 start, u16 size, u32 context)
{
	u16 i, cnt = list_get_count(ctrl);
	u8 asc_str[32];
	u16 uni_str[64];
	control_t *p_ctrl= NULL;
	utc_time_t	s_ca_time = {0};
	u32 tmp_value = 0;

	for (i = 0; i < size; i++)
	{
		if (i + start < cnt)
		{
			/* NO. */
			OS_PRINTF("[debug] %s %d i=%d\n", __FUNCTION__, __LINE__,(start + i));
			sprintf((char *)asc_str, "%d ", i + start + 1);
			list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 0, asc_str); 

			/* Is new */
			if(p_mail_data_buf->p_mail_head[i + start].new_email)
			{
				list_set_field_content_by_strid(ctrl, (u16)(start + i), 1, IDS_YES);
			}
			else
			{
				list_set_field_content_by_strid(ctrl, (u16)(start + i), 1, IDS_NO);
			}

			/* NAME */
			gb2312_to_unicode(p_mail_data_buf->p_mail_head[i + start].subject, 80, uni_str, 40);
			list_set_field_content_by_unistr(ctrl, (u16)(start + i), 2, (u16 *)uni_str);

			#if 0
			//sender
			gb2312_to_unicode(p_mail_data_buf->p_mail_head[i + start].sender, 80, uni_str, 40);
			list_set_field_content_by_unistr(ctrl, (u16)(start + i), 3, (u16 *)uni_str);
			#endif

			//data time
			sprintf((char *)asc_str,"%02x", p_mail_data_buf->p_mail_head[i + start].creat_date[0]);
			asc_to_dec(asc_str, &tmp_value, 2);
			s_ca_time.year = (u8)tmp_value;
			sprintf((char *)asc_str,"%02x", p_mail_data_buf->p_mail_head[i + start].creat_date[1]);
			asc_to_dec(asc_str, &tmp_value, 2);
			s_ca_time.year = s_ca_time.year*100 + (u8)tmp_value;

			sprintf((char *)asc_str,"%02x", p_mail_data_buf->p_mail_head[i + start].creat_date[2]);
			asc_to_dec(asc_str, &tmp_value, 2);
			s_ca_time.month = (u8)tmp_value;
			sprintf((char *)asc_str,"%02x", p_mail_data_buf->p_mail_head[i + start].creat_date[3]);
			asc_to_dec(asc_str, &tmp_value, 2);
			s_ca_time.day = (u8)tmp_value;

			sprintf((char *)asc_str,"%02x", p_mail_data_buf->p_mail_head[i + start].creat_date[4]);
			asc_to_dec(asc_str, &tmp_value, 2);
			s_ca_time.hour = (u8)tmp_value;
			sprintf((char *)asc_str,"%02x", p_mail_data_buf->p_mail_head[i + start].creat_date[5]);
			asc_to_dec(asc_str, &tmp_value, 2);
			s_ca_time.minute = (u8)tmp_value;
			OS_PRINTF("%04d/%02d/%02d %02d:%02d \n", s_ca_time.year, s_ca_time.month, s_ca_time.day, s_ca_time.hour, s_ca_time.minute);
			sprintf((char *)asc_str,"%04d/%02d/%02d %02d:%02d", 
					s_ca_time.year, s_ca_time.month, 
					s_ca_time.day, s_ca_time.hour, s_ca_time.minute);
			str_asc2uni(asc_str, uni_str);

			list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 3, asc_str);

		}
	}

	if(0xFFFF != p_mail_data_buf->max_num)
	{
		p_ctrl = ctrl_get_child_by_id(ctrl->p_parent, IDC_EMAIL_RECEIVED);
		sprintf((char*)asc_str,"%d",p_mail_data_buf->max_num);
		text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
		p_ctrl = ctrl_get_child_by_id(ctrl->p_parent, IDC_EMAIL_RESET);
		sprintf((char*)asc_str,"%d",EMAIL_MAX_NUMB - p_mail_data_buf->max_num);
		text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);
	}
	return SUCCESS;
}

static RET_CODE plist_update_email(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u16 size = EMAIL_LIST_PAGE;
	u16 index;
	p_mail_data_buf = (cas_mail_headers_t*)para2;

	DEBUG(CAS,INFO,"\n");
	list_set_count(p_ctrl, p_mail_data_buf->max_num, EMAIL_LIST_PAGE);  
	email_plist_update(p_ctrl, list_get_valid_pos(p_ctrl), size, 0);

	index = list_get_focus_pos(p_ctrl);
	if(index > p_mail_data_buf->max_num)
		list_set_focus_pos(p_ctrl, 0);
	else
		list_set_focus_pos(p_ctrl, list_get_focus_pos(p_ctrl));

	ctrl_paint_ctrl(p_ctrl->p_parent, TRUE);
	return SUCCESS;

}

#define EMAIL_CORE(x,y)  x#y

RET_CODE open_email_mess(u32 para1, u32 para2)
{
	control_t *p_cont = NULL;
	control_t *p_mbox = NULL;
	control_t *p_sbar = NULL;
	control_t *p_list = NULL;
	control_t *p_ctrl = NULL;
	u16 i = 0;
	u8 asc_str[8];

	u16 itemmenu_btn_str[EMAIL_ITME_COUNT] =
	{
		IDS_CA_NUMBER, IDS_CA_NEW_NOT, IDS_CA_TITLE,IDS_CA_EMAIL_RECEIVE_TIME,  
	};

	p_cont = ui_comm_right_root_create(ROOT_ID_EMAIL_MESS, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
	//mbox item title

	for (i = 0; i < EMAIL_ITME_COUNT; i++)
	{
		switch(i)
		{
			case 0:
				p_mbox = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_EMAIL_ITEM1,
				                EMAIL_TITLE1_X, EMAIL_TITLE_Y, 
				                EMAIL_TITLE1_W, EMAIL_TITLE_H,
				                p_cont, 0);
				break;
			case 1:
				p_mbox = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_EMAIL_ITEM2,
				                EMAIL_TITLE2_X, EMAIL_TITLE_Y, 
				                EMAIL_TITLE2_W, EMAIL_TITLE_H,
				                p_cont, 0);
				break;				
			case 2:
				p_mbox = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_EMAIL_ITEM3,
				                EMAIL_TITLE3_X, EMAIL_TITLE_Y, 
				                EMAIL_TITLE3_W, EMAIL_TITLE_H,
				                p_cont, 0);
				break;				
			case 3:
				p_mbox = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_EMAIL_ITEM4,
				                EMAIL_TITLE4_X, EMAIL_TITLE_Y, 
				                EMAIL_TITLE4_W, EMAIL_TITLE_H,
				                p_cont, 0);
				break;				
			case 4:
				p_mbox = ctrl_create_ctrl(CTRL_TEXT, (u8)IDC_EMAIL_ITEM5,
				                EMAIL_TITLE5_X, EMAIL_TITLE_Y, 
				                EMAIL_TITLE5_W, EMAIL_TITLE_H,
				                p_cont, 0);
				break;
			default:
				break;
		}
		ctrl_set_rstyle(p_mbox, RSI_PBACK,RSI_PBACK, RSI_PBACK);
		text_set_font_style(p_mbox, FSI_WHITE, FSI_WHITE, FSI_WHITE);
		text_set_align_type(p_mbox, STL_CENTER| STL_VCENTER);
		text_set_content_by_strid(p_mbox,itemmenu_btn_str[i]);
	}
  
	//email list
	p_list = ctrl_create_ctrl(CTRL_LIST, IDC_EMAIL_PRO,
	                  		 EMAIL_LIST_X, EMAIL_LIST_Y, EMAIL_LIST_W,EMAIL_LIST_H, p_cont, 0);
	ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	ctrl_set_keymap(p_list, email_plist_list_keymap);
	ctrl_set_proc(p_list, email_plist_list_proc);
	list_set_item_rstyle(p_list, &email_item_rstyle);
	list_set_count(p_list, 0, EMAIL_LIST_PAGE);

	list_set_field_count(p_list, EMAIL_LIST_FIELD, EMAIL_LIST_PAGE);
	list_set_focus_pos(p_list, 0);
	list_set_update(p_list, email_plist_update, 0);

	for (i = 0; i < EMAIL_LIST_FIELD; i++)
	{
		list_set_field_attr(p_list, (u8)i, (u32)(email_plist_attr[i].attr), (u16)(email_plist_attr[i].width),
		                (u16)(email_plist_attr[i].left), (u8)(email_plist_attr[i].top));
		list_set_field_rect_style(p_list, (u8)i, email_plist_attr[i].rstyle);
		list_set_field_font_style(p_list, (u8)i, email_plist_attr[i].fstyle);
	}
  
	//list scroll bar
	p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_EMAIL_BAR, 
	                          EMAIL_SBAR_X, EMAIL_SBAR_Y, EMAIL_SBAR_W, EMAIL_SBAR_H, 
	                          p_cont, 0);
	ctrl_set_rstyle(p_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
	sbar_set_autosize_mode(p_sbar, 1);
	sbar_set_direction(p_sbar, 0);
	sbar_set_mid_rstyle(p_sbar,  RSI_GRAY,  RSI_GRAY,  RSI_GRAY);
	ctrl_set_mrect(p_sbar, 0, 0, EMAIL_SBAR_W,  EMAIL_SBAR_H);
	list_set_scrollbar(p_list, p_sbar);
  
	//received head
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EMAIL_RECEIVED_HEAD,
	                       EMAIL_RECEIVED_HEAD_X, EMAIL_RECEIVED_HEAD_Y,
	                       EMAIL_RECEIVED_HEAD_W,EMAIL_RECEIVED_HEAD_H,
	                       p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_RECEIVED);

	//received
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EMAIL_RECEIVED,
	                       EMAIL_RECEIVED_X, EMAIL_RECEIVED_Y,
	                       EMAIL_RECEIVED_W,EMAIL_RECEIVED_H,
	                       p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
	text_set_content_by_ascstr(p_ctrl, (u8*)"0");

	//rest space head
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EMAIL_RESET_HEAD,
	                       EMAIL_RESET_HEAD_X, EMAIL_RESET_HEAD_Y,
	                       EMAIL_RESET_HEAD_W, EMAIL_RESET_HEAD_H,
	                       p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_REST_SPACE);

	//rest space
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EMAIL_RESET,
	                       EMAIL_RESET_X, EMAIL_RESET_Y,
	                       EMAIL_RESET_W, EMAIL_RESET_H,
	                       p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
	sprintf((char*)asc_str,"%d",EMAIL_MAX_NUMB);
	text_set_content_by_ascstr(p_ctrl, (u8*)asc_str);

	//del one
	p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_EMAIL_DEL_ICON,
							EMAIL_DEL_ICON_X, EMAIL_DEL_ICON_Y,
							EMAIL_DEL_ICON_W, EMAIL_DEL_ICON_H,
							p_cont, 0);
	bmap_set_content_by_id(p_ctrl, IM_KEY_IC_DEL);

	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EMAIL_DEL_TEXT,
	                       EMAIL_DEL_TEXT_X, EMAIL_DEL_TEXT_Y,
	                       EMAIL_DEL_TEXT_W, EMAIL_DEL_TEXT_H,
	                       p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_DELETE);

	//del all
	p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_EMAIL_DEL_ALL_ICON,
							EMAIL_DEL_ALL_ICON_X, EMAIL_DEL_ALL_ICON_Y,
							EMAIL_DEL_ALL_ICON_W, EMAIL_DEL_ALL_ICON_H,
							p_cont, 0);
	bmap_set_content_by_id(p_ctrl, IM_KEY_IC_FN);

	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EMAIL_DEL_ALL_TEXT,
	                       EMAIL_DEL_ALL_TEXT_X, EMAIL_DEL_ALL_TEXT_Y,
	                       EMAIL_DEL_ALL_TEXT_W, EMAIL_DEL_ALL_TEXT_H,
	                       p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_DELETE_ALL);

	ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(p_cont, FALSE);

	ui_ca_do_cmd(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
	return SUCCESS;
}

static RET_CODE on_email_list_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
	RET_CODE ret = SUCCESS;
	u16 old_focus, new_focus;
	old_focus = list_get_focus_pos(p_list);
	ret = list_class_proc(p_list, msg, para1, para2);
	new_focus = list_get_focus_pos(p_list);

	OS_PRINTF("m_id %d\n",p_mail_data_buf->p_mail_head[new_focus].m_id);
	OS_PRINTF("sender %d\n",p_mail_data_buf->p_mail_head[new_focus].sender);
	OS_PRINTF("subject %d\n",p_mail_data_buf->p_mail_head[new_focus].subject);
	OS_PRINTF("on_email_list_change_focus : old focus : %d,  new focus : %d\n",old_focus, new_focus);
	return SUCCESS;
}

static RET_CODE on_email_del_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)  
{
	u16 new_focus;
	control_t *p_ctrl = NULL;

	p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	new_focus = list_get_focus_pos(p_list);
	OS_PRINTF("--------on_email_del_mail   :focus :%d\n",new_focus);

	switch(msg)
	{
		case MSG_DELETE_ONE:
			ui_ca_do_cmd(CAS_CMD_MAIL_DEL_ONE, p_mail_data_buf->p_mail_head[new_focus].m_id,0); 
			break;
		case MSG_DELETE_ALL:
			ui_ca_do_cmd(CAS_CMD_MAIL_DEL_ALL, 0 ,0);  
			break;
		default:
			return ERR_FAILURE;
	}
	ui_ca_do_cmd(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
	return SUCCESS;

}

void on_select_del(void)
{
	g_del_mail = 1;
}

void on_cancel_select_del(void)
{
	g_del_mail = 0;
}

static RET_CODE on_dlg_email_del_one_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	if(p_mail_data_buf->max_num)
	{
		ui_comm_ask_for_dodlg_open(NULL, IDS_CA_DEL_CURRENT_MAIL, on_select_del, on_cancel_select_del, 0);
		if(g_del_mail == 1)
			on_email_del_mail(p_list,msg,para1,para2);

		if(list_get_focus_pos(p_list) == (p_mail_data_buf->max_num - 1))
			list_set_focus_pos(p_list, (p_mail_data_buf->max_num - 2));
	}

	return SUCCESS;
}

static RET_CODE on_dlg_email_del_all_mail(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	if(p_mail_data_buf->max_num)
	{
		ui_comm_ask_for_dodlg_open(NULL, IDS_CA_DEL_ALL_MAIL, 
		                         on_select_del, on_cancel_select_del, 0);
		if(g_del_mail == 1)
		on_email_del_mail(p_list,msg,para1,para2);
	}
	return SUCCESS;
}

static RET_CODE on_email_list_select(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	u32 ret = READY_TO_READ_MAIL;
	u16 index = 0;
	prompt_type_t prompt_type;
	index = list_get_focus_pos(p_list);

	prompt_type.index = index;
	prompt_type.message_type = 0;

	OS_PRINTF("@@@@p_mail_data_buf.total_email_num = %d, index = %d\n",p_mail_data_buf->total_email_num,index);

	switch(ret)
	{
		case READY_TO_READ_MAIL:
			if((0 != p_mail_data_buf->max_num) && (0xFFFF != p_mail_data_buf->max_num))
			{
				manage_open_menu(ROOT_ID_CA_PROMPT, (u32)&prompt_type, (u32)p_mail_data_buf);
			}
			break;
		case CAS_E_MAIL_CAN_NOT_READ:
			ui_comm_cfmdlg_open(NULL,IDS_CA,NULL,0);
			OS_PRINTF("ret  = CAS_E_MAIL_CAN_NOT_READ\n");
			break;
		default:
			ui_comm_cfmdlg_open(NULL,IDS_CA2, NULL,0);
			break;
	}
	return SUCCESS;
}

static RET_CODE on_email_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	ui_ca_do_cmd(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
	fw_destroy_mainwin_by_id(ROOT_ID_EMAIL_MESS);
	swtich_to_sys_set(ROOT_ID_INVALID, 0);
	return SUCCESS;
}

static RET_CODE on_email_exit_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	ui_close_all_mennus();
	return SUCCESS;
}

BEGIN_KEYMAP(email_plist_list_keymap, NULL)
	ON_EVENT(V_KEY_F1, MSG_DELETE_ONE)
	ON_EVENT(V_KEY_RED, MSG_DELETE_ONE)
	ON_EVENT(V_KEY_HOT_XDEL, MSG_DELETE_ONE)
	ON_EVENT(V_KEY_F2, MSG_DELETE_ALL)
	ON_EVENT(V_KEY_HOT_XEXTEND, MSG_DELETE_ALL)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)    
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
	ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
	ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
	ON_EVENT(V_KEY_MAIL, MSG_EXIT_ALL)
END_KEYMAP(email_plist_list_keymap, NULL)

BEGIN_MSGPROC(email_plist_list_proc, list_class_proc)
	ON_COMMAND(MSG_FOCUS_UP, on_email_list_change_focus)
	ON_COMMAND(MSG_FOCUS_DOWN, on_email_list_change_focus)
	ON_COMMAND(MSG_PAGE_UP, on_email_list_change_focus)
	ON_COMMAND(MSG_PAGE_DOWN, on_email_list_change_focus)
	ON_COMMAND(MSG_CA_MAIL_HEADER_INFO, plist_update_email)
	ON_COMMAND(MSG_SELECT, on_email_list_select)
	ON_COMMAND(MSG_DELETE_ONE, on_dlg_email_del_one_mail)
	ON_COMMAND(MSG_DELETE_ALL, on_dlg_email_del_all_mail)
	ON_COMMAND(MSG_EXIT, on_email_exit)
	ON_COMMAND(MSG_EXIT_ALL, on_email_exit_all)
END_MSGPROC(email_plist_list_proc, list_class_proc)

