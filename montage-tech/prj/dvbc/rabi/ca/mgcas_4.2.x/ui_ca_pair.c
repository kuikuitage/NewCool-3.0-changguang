/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ca_public.h"
#include "ui_ca_pair.h"

enum email_ctrl_id
{
	IDC_INVALID = 0,
	IDC_CONT,
	IDC_TITLE,
	IDC_CONTENT,
};

static void get_pair_str(u16 * uni_str, u16 str_id1,u16 str_id2, u32 content)
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

RET_CODE open_ca_pair_dlg(u32 para1, u32 para2)
{
	control_t *p_content, *p_title, *p_cont = NULL;
	u16 uni_str[64] = {0};
	#ifndef WIN32
	cas_pin_err_t *pair_info = (cas_pin_err_t *)para2;
	#else
	u32 charge = 2;
	u8 error_code = 3;
	cas_pin_err_t *pair_info;
	pair_info->err_code = error_code;
	pair_info->err_cnt = charge;
	#endif
	
	if(FALSE == ui_is_fullscreen_menu(fw_get_focus_id()) )
	{
		return ERR_FAILURE;
	}  
	if(!ui_is_playpg_scrambled()) 
		return ERR_FAILURE;

	if(fw_find_root_by_id(ROOT_ID_CA_PAIR) != NULL)
	{
		fw_destroy_mainwin_by_id(ROOT_ID_CA_PAIR);
	}

	p_cont = fw_create_mainwin(ROOT_ID_CA_PAIR,
	                       PAIR_CONT_FULL_X, PAIR_CONT_FULL_Y,
	                       PAIR_CONT_FULL_W, PAIR_CONT_FULL_H,
	                       ROOT_ID_INVALID, 0,
	                       OBJ_ATTR_ACTIVE, 0);
	if(p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);

	p_cont = ctrl_create_ctrl(CTRL_TEXT, IDC_CONT,
	                     0, 0,PAIR_CONT_FULL_W,PAIR_CONT_FULL_H,p_cont, 0);
	ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);

	p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_TITLE,
	                     PAIR_TITLE_FULL_X, PAIR_TITLE_FULL_Y,
	                     PAIR_TITLE_FULL_W,PAIR_TITLE_FULL_H,
	                     p_cont, 0);
	ctrl_set_rstyle(p_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_content_type(p_title, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_title, IDS_CA_STB_PAIR_STATUS);

	//content
	p_content = ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
	                   PAIR_CONTENT_FULL_X, PAIR_CONTENT_FULL_Y,
	                   PAIR_CONTENT_FULL_W, PAIR_CONTENT_FULL_H,
	                   p_cont, 0);
	ctrl_set_rstyle(p_content, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_content, FSI_WHITE, FSI_BLUE, FSI_WHITE);
	text_set_align_type(p_content, STL_CENTER | STL_VCENTER);

	switch(pair_info->err_code)
	{
		case CAS_STB_PATERNERED:
			OS_PRINTF("CAS_STB_PATERNERED return pair error code!!!!\n");
			text_set_content_type(p_content, TEXT_STRTYPE_STRID);
			text_set_content_by_strid(p_content, IDS_CA_SMAR_CARD_UNPAIRD);
			break;
		case CAS_CARD_PATERNERED:
			OS_PRINTF("CAS_CARD_PATERNERED return pair error code!!!!\n");
			text_set_content_type(p_content, TEXT_STRTYPE_STRID);
			text_set_content_by_strid(p_content, IDS_CA_SMART_CARD_PAIRD);
			break;
		case CAS_CARD_NONE_PATERNERED:
			OS_PRINTF("CAS_CARD_NONE_PATERNERED return pair error code!!!!\n");
			text_set_content_type(p_content, TEXT_STRTYPE_UNICODE);
			get_pair_str(uni_str, IDS_CA_INSERT_TIME, IDS_CA_TIME_LOCK, pair_info->err_cnt);
			text_set_content_by_unistr(p_content, uni_str);
			break;
		default :
			OS_PRINTF("err return pair error code!!!!\n");
	}

	#ifdef SPT_DUMP_DATA
	spt_dump_menu_data(p_cont);
	#endif
	if(p_cont != NULL)
	{
		ctrl_paint_ctrl(p_cont, FALSE);
	}
	return SUCCESS;
}

void close_ca_pair_dlg(void)
{
	fw_destroy_mainwin_by_id(ROOT_ID_CA_PAIR);
}



