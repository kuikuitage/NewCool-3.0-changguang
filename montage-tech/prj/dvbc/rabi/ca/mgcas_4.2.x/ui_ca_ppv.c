/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ca_public.h"
#include "ui_ca_ppv.h"
#include "ui_notify.h"
#include "ui_ca_ppv_icon.h"

enum ppv_ctrl_id
{
	IDC_PPV_CONT1 = 1,
	IDC_TEXT_BIT_YES,
	IDC_TEXT_BIT_NO,  
	IDC_PPV_CONT2,
	IDC_TEXT_TITLE1,
	IDC_TEXT_TITLE2,
	IDC_PPV_TITLE1 ,
	IDC_PPV_TITLE2 ,
	IDC_PPV_TITLE3 ,
  
};

enum local_msg
{
	MSG_PPV_ORDE = MSG_LOCAL_BEGIN + 720,
};

static u16 ca_ppv_info_keymap(u16 key);
RET_CODE ca_ppv_info_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);
static u16 ca_ppv_cont1_keymap(u16 key);
RET_CODE ca_ppv_cont1_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

static ipp_buy_info_t *p_buy_info = NULL;
static burses_info_t *p_burse = NULL;
static ipps_info_t *p_ipp_info = NULL;

void ui_ppv_dlg_close(void)
{
	fw_destroy_mainwin_by_id(ROOT_ID_CA_IPPV_PPT_DLG);
}

static void get_ppv_str(u16 *uni_str, u16 str_id, u32 content)
{
	u16 str[10] = {0}; 
	u16 len = 0;

	convert_i_to_dec_str(str, (s32)content);
	uni_strcat(uni_str, str,64);
	len = (u16)uni_strlen(uni_str);
	gui_get_string(str_id, str, 64 - len); 
	uni_strcat(uni_str, str, 64); 

	return;
}

static RET_CODE on_ppv_ca_orde_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_btn;
	p_btn = ctrl_get_active_ctrl(p_ctrl);
	if (p_btn != NULL)
	{
		if(ctrl_get_ctrl_id(p_btn) == IDC_TEXT_BIT_YES)
		{
			p_buy_info->state_flag = 1;
			ui_ca_do_cmd((u32)CAS_CMD_IPP_BUY_SET, (u32)p_buy_info,0);
		}
		else
		{
			p_buy_info->state_flag = 0;
			ui_ca_do_cmd((u32)CAS_CMD_IPP_BUY_SET, (u32)p_buy_info,0);
		}
	}
	ui_ppv_dlg_close();
	return SUCCESS;
}

static RET_CODE on_ppv_ca_burse_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_attr, *p_text;
	u16 uni_str[32]={0};

	p_burse = (burses_info_t*)para2;
	p_attr = ctrl_get_child_by_id(p_ctrl, IDC_PPV_CONT1);
	p_text = ctrl_get_child_by_id(p_attr, IDC_PPV_TITLE3);
	if(p_burse->p_burse_info[p_burse->index].cash_value >= 0)
	{
		#if 0
		hight_vale = p_burse->p_burse_info[p_burse->index].cash_value/10000;
		low_vale = (p_burse->p_burse_info[p_burse->index].cash_value - hight_vale*10000)/100;
		limt_hight[0] = (u8)(hight_vale/1000);
		limt_hight[1] = (u8)((hight_vale%1000)/100);
		limt_hight[2] = (u8)((hight_vale%1000)%100/10);
		limt_hight[3] = (u8)((hight_vale%1000)%100%10);

		sprintf((char *)asc_str, "%d.%02d", (limt_hight[0]*1000 + limt_hight[1]*100 + limt_hight[2]*10 + limt_hight[3]),(u8)low_vale);
		#endif
		get_ppv_str(uni_str, IDS_CA_POINT, ((u32)(p_burse->p_burse_info[p_burse->index].cash_value) - (p_burse->p_burse_info[p_burse->index].balance)));
		ui_comm_static_set_content_by_unistr(p_text, uni_str);
	}
	else
	{
		#if 0
		p_burse->p_burse_info[p_burse->index].cash_value*=(-1);
		hight_vale = p_burse->p_burse_info[p_burse->index].cash_value/10000;
		low_vale = (p_burse->p_burse_info[p_burse->index].cash_value - hight_vale*10000)/100;
		#endif
		get_ppv_str(uni_str, IDS_CA_POINT,((u32)(p_burse->p_burse_info[p_burse->index].cash_value) - (p_burse->p_burse_info[p_burse->index].balance)));
		ui_comm_static_set_content_by_unistr(p_text, uni_str);
	}
	OS_PRINTF("p_burse->p_burse_info[p_burse->index].cash_value:%d \n \
	                p_burse->p_burse_info[p_burse->index].balance:%d\n",
	                 p_burse->p_burse_info[p_burse->index].cash_value, p_burse->p_burse_info[p_burse->index].balance);
	ctrl_paint_ctrl(p_ctrl,TRUE);

	return SUCCESS;
}

static RET_CODE on_ppv_ca_ppv_bug_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_attr, *p_text;
	u16 uni_str[64] = {0,};

	p_buy_info = (ipp_buy_info_t *)para2;
	p_attr = ctrl_get_child_by_id(p_ctrl, IDC_PPV_CONT1);
	p_text = ctrl_get_child_by_id(p_attr, IDC_PPV_TITLE2);
	OS_PRINTF("p_buy_info->ipp_charge ====== : %d\n",p_buy_info->ipp_charge);
	memset(uni_str, 0, 64 * sizeof(u16));
	get_ppv_str(uni_str, IDS_CA_PPV_PRICE, p_buy_info->ipp_charge);
	ui_comm_static_set_content_by_unistr(p_text, uni_str);
	ctrl_paint_ctrl(p_ctrl,TRUE);
	return SUCCESS;
}

static RET_CODE on_ppv_ca_ppv_show_status(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_attr, *p_text;
  #ifndef WIN32
  p_ipp_info = (ipps_info_t *)para2;
  #endif
  p_attr = ctrl_get_child_by_id(p_ctrl, IDC_PPV_CONT1);
  p_text = ctrl_get_child_by_id(p_attr, IDC_PPV_TITLE1);
  OS_PRINTF("p_ipp_info->p_ipp_info[0].book_state_flag ====== : %d\n",p_ipp_info->p_ipp_info[0].book_state_flag);

  if(p_ipp_info->p_ipp_info[0].book_state_flag)
  {
    ui_comm_static_set_content_by_strid(p_text, IDS_HAS_ORRDERED);
  }
  else
  {
    ui_comm_static_set_content_by_strid(p_text, IDS_NOT_ORDER);
  }
  ctrl_paint_ctrl(p_ctrl, TRUE);
  return SUCCESS;
}


static RET_CODE on_ppv_change_channel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
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
  
  ui_ppv_dlg_close();
  close_ca_ppv_icon();
  return SUCCESS;
}


RET_CODE open_ca_ippv_ppt_dlg(u32 para1, u32 para2)
{
	control_t *p_cont = NULL,*p_title = NULL,*p_bit,*p_attr1,*p_ctrl = NULL;
	u16 i = 0, y =0;
	u16 str_id[PPV_INFO_ITEM_CNT] = 
	{
		IDS_CA_SELECTED_STATUS_1,
		IDS_CA_PROGRAM_PRICE,
		IDS_CA_CARD_BALANCE,
		IDS_IPPV_ORDER,
		IDS_IPPV_RESCING
	};

	if((p_cont = fw_find_root_by_id(ROOT_ID_CA_IPPV_PPT_DLG)) != NULL)
	{
		return SUCCESS;
	}
	
	if(!ui_is_fullscreen_menu(fw_get_focus_id()))
	{
		return SUCCESS;
	}

	if(NULL != fw_find_root_by_id(ROOT_ID_PROG_BAR))
	{
		manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
	}

	#ifdef WIN32
	ui_set_ppv_info_test();
	#endif
	open_ca_ppv_icon(0, 0);

	p_cont = fw_create_mainwin(ROOT_ID_CA_IPPV_PPT_DLG,
	                              PPV_INFO_CONT_X, PPV_INFO_CONT_Y, 
	                              PPV_INFO_CONT_W, PPV_INFO_CONT_H,
	                              0, 0,
	                              OBJ_ATTR_ACTIVE, STL_EX_TOPMOST);
	if (p_cont == NULL)
	{
		return FALSE;
	}
	ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
	ctrl_set_keymap(p_cont, ca_ppv_info_keymap);
	ctrl_set_proc(p_cont, ca_ppv_info_proc);
  

	p_attr1 = ctrl_create_ctrl(CTRL_CONT, (u8)(IDC_PPV_CONT1),
	                    PPV_CONT_TITLE_X,
	                    PPV_CONT_TITLE_Y,
	                    PPV_CONT_TITLE_W, PPV_CONT_TITLE_H,
	                    p_cont, 0);
	ctrl_set_rstyle(p_attr1, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	ctrl_set_keymap(p_attr1, ca_ppv_cont1_keymap);
	ctrl_set_proc(p_attr1, ca_ppv_cont1_proc);
	ctrl_set_attr(p_attr1, OBJ_ATTR_ACTIVE);

	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_TEXT_TITLE1),
	                    PPV_TEXT_TITLE_X,
	                    PPV_TEXT_TITLE_Y,
	                    PPV_TEXT_TITLE_W, PPV_TEXT_TITLE_H,
	                    p_attr1, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_CA_PPV_SH, FSI_CA_PPV_SH, FSI_CA_PPV_SH);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, IDS_CA_PPV_PROG);

	y = PPV_ACCEPT_CA_INFO_ITEM_Y;
	for (i = 0; i<PPV_ACCEPT_CA_INFO_ITEM_CNT; i++)
	{
	    switch(i)
	    {
			case 0:
				p_title = ui_comm_static_create(p_attr1, (u8)(IDC_PPV_TITLE1 + i),
				                              PPV_ACCEPT_CA_INFO_ITEM_X,y,
				                              PPV_ACCEPT_CA_INFO_ITEM_LW,
				                              PPV_ACCEPT_CA_INFO_ITEM_RW);
				ui_comm_static_set_static_txt(p_title, str_id[i]);
				ui_comm_static_set_param(p_title, TEXT_STRTYPE_STRID);
				ui_comm_static_set_rstyle(p_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
				break;
			case 1:
			case 2:
				p_title = ui_comm_static_create(p_attr1, (u8)(IDC_PPV_TITLE1 + i),
				                               PPV_ACCEPT_CA_INFO_ITEM_X, y,
				                               PPV_ACCEPT_CA_INFO_ITEM_LW,
				                               PPV_ACCEPT_CA_INFO_ITEM_RW);
				ui_comm_static_set_static_txt(p_title, str_id[i]);
				ui_comm_static_set_param(p_title, TEXT_STRTYPE_UNICODE);
				ui_comm_static_set_rstyle(p_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
				break;
			default:
				break;
		}
		ui_comm_static_set_rstyle(p_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
		ui_comm_ctrl_set_cont_rstyle(p_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
		y += PPV_ACCEPT_CA_INFO_ITEM_H + PPV_ACCEPT_CA_INFO_ITEM_V_GAP;  
        
	}
  
	for(i=0; i<PPV_SET_BTN_CNT; i++)
	{
		p_bit = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_TEXT_BIT_YES+i),
		                    (u16)(PPV_TEXT_BIT_X+(PPV_TEXT_BIT_W + PPV_TEXT_BIT_X)*i),
		                    PPV_TEXT_BIT_Y,
		                    PPV_TEXT_BIT_W, PPV_TEXT_BIT_H,
		                    p_attr1, 0);
		ctrl_set_rstyle(p_bit, RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH); 
		text_set_font_style(p_bit, FSI_CA_PPV_N, FSI_CA_PPV_HL, FSI_CA_PPV_SH);
		text_set_align_type(p_bit, STL_CENTER| STL_VCENTER);
		text_set_content_type(p_bit, TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_bit, str_id[3+i]);
		ctrl_set_related_id(p_bit,
		                    (u8)((i - 1 + PPV_SET_BTN_CNT) % PPV_SET_BTN_CNT + IDC_TEXT_BIT_YES), /* left */
		                    0,                             /* up */
		                    (u8)((i + 1) % PPV_SET_BTN_CNT + IDC_TEXT_BIT_YES),       /* right */
		                    0);                            /* down */
	}

	p_ctrl = ctrl_get_child_by_id(p_attr1, IDC_TEXT_BIT_YES);
	ctrl_default_proc(p_ctrl, MSG_GETFOCUS, 0, 0); 
	ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

	#ifdef WIN32
	ctrl_process_msg(fw_find_root_by_id(ROOT_ID_CA_IPPV_PPT_DLG), MSG_CA_BUR_INFO,0,0);
	#else
	ui_ca_do_cmd((u32)CAS_CMD_GET_IPP_BUY_INFO, 0,0);
	ui_ca_do_cmd((u32)CAS_CMD_BURSE_INFO_GET, 0,0);
	ui_ca_do_cmd((u32)CAS_CMD_IPPV_INFO_GET, 0,0);
	#endif
  return SUCCESS;
}

BEGIN_KEYMAP(ca_ppv_info_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ca_ppv_info_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_ppv_info_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_CA_BUR_INFO,on_ppv_ca_burse_info)
	ON_COMMAND(MSG_CA_IPPV_BUY_INFO,on_ppv_ca_ppv_bug_info)
	ON_COMMAND(MSG_CA_IPV_INFO,on_ppv_ca_ppv_show_status)
END_MSGPROC(ca_ppv_info_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ca_ppv_cont1_keymap, NULL)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
	ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
	ON_EVENT(V_KEY_OK, MSG_PPV_ORDE)
END_KEYMAP(ca_ppv_cont1_keymap, NULL)

BEGIN_MSGPROC(ca_ppv_cont1_proc, cont_class_proc)
	ON_COMMAND(MSG_PPV_ORDE,on_ppv_ca_orde_select)
	ON_COMMAND(MSG_FOCUS_UP,on_ppv_change_channel)
	ON_COMMAND(MSG_FOCUS_DOWN,on_ppv_change_channel)
END_MSGPROC(ca_ppv_cont1_proc, cont_class_proc)


