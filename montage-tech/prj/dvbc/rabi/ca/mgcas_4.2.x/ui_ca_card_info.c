/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ca_card_info.h"
#include "ui_ca_public.h"
#include "ui_xsys_set.h"

enum control_id
{
	IDC_INVALID = 0,
	IDC_FRM_BUTTONS,
};


enum prog_info_cont_ctrl_id
{
	IDC_CA_SP_ID = 1,
	IDC_CA_CAS_VER,
	IDC_CA_SMART_CARD_NUMBER,
	IDC_CA_CA_STATUS,
	IDC_CA_CARD_AREA_CODE,
	IDC_CA_CARD_LIMIT_CODE,
	IDC_CA_SLOT_TOTAL,
	IDC_CA_USER_USED,
	IDC_CA_ENTITLE_EXPIRED,
	IDC_CHARACTER_CODE,
	IDC_CHARACTER_NUMB,
};

#define CARD_SN_MAX_LEN             16
#define CARD_EIGEN_VALU_MAX_LEN     64

u16 card_info_root_keymap(u16 key);
RET_CODE card_info_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_ca_card_info(u32 para1, u32 para2)
{
	control_t *p_cont = NULL, *p_ctrl = NULL;
	u16 stxt_card_info[]=
	{
		IDS_CA_PROVIDER,
		IDS_CA_CARD_VER,
		IDS_CA_SERIAL_NUMBER,
		IDS_CA_STB_PAIR_STATUS,
		IDS_CA_ZONECODE_VALUUE,
		IDS_CA_CARD_LIMIT_CODE,
		IDS_CA_WALLET_TOTAL,
		IDS_CA_USER_USED,
		IDS_CA_ENTITLE_EXPIRED,
		IDS_CA_CONDTION_CODE,
	};
	u16 y=0, i=0;

	p_cont = ui_comm_right_root_create(ROOT_ID_CA_CARD_INFO, ROOT_ID_XSYS_SET,OBJ_ATTR_ACTIVE, 0);
	if(NULL == p_cont)
	{
		return ERR_FAILURE;
	}
	ctrl_set_keymap(p_cont, card_info_root_keymap);
	ctrl_set_proc(p_cont, card_info_root_proc);

	y = ACCEPT_INFO_CA_INFO_ITEM_Y;
	for(i=0; i < ACCEPT_INFO_CARD_INFO_ITEM_CNT-1; i++)
	{
		p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_CA_SP_ID + i),
		                                   ACCEPT_INFO_CA_INFO_ITEM_X, y,
		                                   ACCEPT_INFO_CA_INFO_ITEM_LW,
		                                   ACCEPT_INFO_CA_INFO_ITEM_RW);
		ui_comm_static_set_static_txt(p_ctrl, stxt_card_info[i]);
		ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
		ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

		ctrl_set_related_id(p_ctrl,
		                0,                                     /* left */
		                (u8)((i - 1 +
		                      ACCEPT_INFO_CARD_INFO_ITEM_CNT) %
		                     ACCEPT_INFO_CARD_INFO_ITEM_CNT + IDC_CA_SP_ID),           /* up */
		                0,                                     /* right */
		                (u8)((i + 1) % ACCEPT_INFO_CARD_INFO_ITEM_CNT + IDC_CA_SP_ID));/* down */

		y += COMM_CTRL_H + ACCEPT_CA_INFO_ITEM_V_GAP;
	}

	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CHARACTER_CODE),
	                    ACCEPT_INFO_CA_INFO_ITEM_X+COMM_CTRL_OX,
	                    y,
	                    ACCEPT_INFO_CA_INFO_ITEM_LW,
	                    COMM_CTRL_H,p_cont, 0);

	ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	text_set_font_style(p_ctrl, FSI_COMM_CTRL_SH, FSI_COMM_CTRL_SH, FSI_COMM_CTRL_SH);
	text_set_align_type(p_ctrl, STL_LEFT| STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_ctrl, stxt_card_info[i]);

	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_CHARACTER_NUMB),
	                    ACCEPT_INFO_CA_CODE_ITEM_X+20,
	                    y,
	                    ACCEPT_INFO_CA_INFO_ITEM_RW,
	                    ACCEPT_INFO_CA_CODE_ITEM_H,p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
	text_set_font_style(p_ctrl, FSI_COMM_CTRL_SH, FSI_COMM_CTRL_SH, FSI_COMM_CTRL_SH);
	text_set_align_type(p_ctrl, STL_LEFT| STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);

	ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

	#ifdef WIN32
	on_conditional_accept_info_update(p_cont, 0, 0, (cas_card_info_t *)para1);
	#else
	ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0 ,0);
	ui_ca_do_cmd((u32)CAS_CMD_BURSE_INFO_GET,0,0);
	#endif
	return SUCCESS;
}

static void accept_ca_info_set_content(control_t *p_cont, cas_card_info_t *p_card_info)
{
	control_t *p_frm = p_cont;
	control_t *p_cas_sp_id = ctrl_get_child_by_id(p_frm, IDC_CA_SP_ID);
	control_t *p_cas_ver = ctrl_get_child_by_id(p_frm, IDC_CA_CAS_VER);
	control_t *p_card_id = ctrl_get_child_by_id(p_frm, IDC_CA_SMART_CARD_NUMBER);
	control_t *p_cas_satue = ctrl_get_child_by_id(p_frm, IDC_CA_CA_STATUS);
	control_t *p_arear_code = ctrl_get_child_by_id(p_frm, IDC_CA_CARD_AREA_CODE);
	control_t *p_limit_code = ctrl_get_child_by_id(p_frm, IDC_CA_CARD_LIMIT_CODE);
	control_t *p_entitle_expired = ctrl_get_child_by_id(p_frm, IDC_CA_ENTITLE_EXPIRED);
	control_t *p_eigen_code = ctrl_get_child_by_id(p_cont, IDC_CHARACTER_NUMB);

	u8 asc_str[128];
	s8 asc_str1[128];
	#ifdef WIN32
	u8 card_state = 2;
	#else
	u8 card_state = p_card_info->paterner_stat;
	#endif
	u8 i =0, j = 0;
	u16 uni_num[65];
	rect_t dlg_rc =
	{
		RESTORE_ASK_L,
		RESTORE_ASK_T,
		RESTORE_ASK_R,
		RESTORE_ASK_B,
	};

	#ifndef WIN32
	OS_PRINTF(" conditional_accept_info_set_content ########V%d.%d.%d\n", p_card_info->cas_ver[0],
	                  p_card_info->cas_ver[1] >>4, p_card_info->cas_ver[1] & 0xf);

	OS_PRINTF("p_card_info->card_state : %d\n", p_card_info->paterner_stat);

	OS_PRINTF("%s", p_card_info->sn);
	OS_PRINTF("p_card_info->card_zone_code : %d\n", p_card_info->card_zone_code);
	OS_PRINTF("p_card_info->card_ctrl_code : %d\n", p_card_info->card_ctrl_code);
	OS_PRINTF("p_card_info->expire_date : %d\n", ((p_card_info->expire_date[0] << 8) | (p_card_info->expire_date[1])));
	#endif

	if(p_card_info == NULL || ui_is_smart_card_insert() == FALSE)
	{
		ui_comm_cfmdlg_open(&dlg_rc, IDS_MG_CARD_NOT_READY, NULL, 2000);
	}
	else
	{
		//card sp
		sprintf((char *)asc_str, "ÉÏº£ÈýÖÞÑ¸³Û");
		gb2312_to_unicode(asc_str,sizeof(asc_str),uni_num,56);
		ui_comm_static_set_content_by_unistr(p_cas_sp_id, uni_num);

		//card version
		sprintf((char *)asc_str, "V%d.%d.%d", p_card_info->cas_ver[0],
		              p_card_info->cas_ver[1] >>4, p_card_info->cas_ver[1] & 0xf);
		ui_comm_static_set_content_by_ascstr(p_cas_ver, asc_str);

		if(p_card_info->card_state == FALSE)
		{
			OS_PRINTF("[%s]card not ready \n",__FUNCTION__);
			return;
		}

		ui_comm_static_set_content_by_ascstr(p_card_id, p_card_info->sn);

		ui_comm_static_set_param(p_cas_satue, TEXT_STRTYPE_STRID);
		//card status
		if(card_state == CAS_PATERNER)
		{
			ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_STB_PAIR);
		}
		else if(card_state == CAS_NOT_PATERNER)
		{
			ui_comm_static_set_content_by_strid(p_cas_satue, IDS_CA_NEED_SMC_STB_PAIR);
		}

		//area code
		sprintf((char *)asc_str, "%d", p_card_info->card_zone_code);
		ui_comm_static_set_content_by_ascstr(p_arear_code, asc_str);

		//limit code
		sprintf((char *)asc_str, "%d", p_card_info->card_ctrl_code);
		ui_comm_static_set_content_by_ascstr(p_limit_code, asc_str);

		//expire date
		sprintf((char *)asc_str, "%d", ((p_card_info->expire_date[0] << 8) | (p_card_info->expire_date[1])));
		ui_comm_static_set_content_by_ascstr(p_entitle_expired, asc_str);

		for(i = 0, j = 0; i < CARD_EIGEN_VALU_MAX_LEN; i += 2, j++)
		{
			sprintf(&asc_str1[i], "%02x", p_card_info->card_eigen_valu[j]);
		}
		text_set_content_by_ascstr(p_eigen_code,(u8*)asc_str1);
		DEBUG(CAS,INFO,"card_eigen_valu = %s\n",asc_str1);
	}

	//for test
	#ifdef WIN32
	sprintf((char *)asc_str, "8047499004002961");
	ui_comm_static_set_content_by_ascstr(p_card_id, asc_str);
	#endif
}

static RET_CODE on_conditional_accept_info_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	OS_PRINTF("on_conditional_accept_info_update\n");
	accept_ca_info_set_content(p_cont, (cas_card_info_t *)para2);
	ctrl_paint_ctrl(p_cont, TRUE);

	return SUCCESS;
}
RET_CODE on_exit_card_info(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	fw_destroy_mainwin_by_id(ROOT_ID_CA_CARD_INFO);
	return SUCCESS;
}

static void accept_ca_burse_info(control_t *p_cont, burses_info_t *p_burse)
{
	control_t *p_frm =  p_cont;
	control_t *p_slot_total = ctrl_get_child_by_id(p_frm, IDC_CA_SLOT_TOTAL);
	control_t *p_user_used = ctrl_get_child_by_id(p_frm, IDC_CA_USER_USED);

	u8 asc_str[128];

	if(p_burse == NULL || ui_is_smart_card_insert() == FALSE)
	{
		ui_comm_static_set_content_by_ascstr(p_slot_total,(u8 *) " ");
		ui_comm_static_set_content_by_ascstr(p_user_used,(u8 *) " ");
	}
	else
	{
		if(p_burse->p_burse_info[p_burse->index].cash_value >= 0)
		{
			sprintf((char *)asc_str, "%ld", p_burse->p_burse_info[p_burse->index].cash_value);
			ui_comm_static_set_content_by_ascstr(p_slot_total, asc_str);
		}
		else
		{
			sprintf((char *)asc_str, "-%ld", p_burse->p_burse_info[p_burse->index].cash_value);
			ui_comm_static_set_content_by_ascstr(p_slot_total, asc_str);
		}
		sprintf((char *)asc_str, "%ld", p_burse->p_burse_info[p_burse->index].balance);
		ui_comm_static_set_content_by_ascstr(p_user_used, asc_str);
	}
	ctrl_paint_ctrl(p_slot_total, TRUE);
	ctrl_paint_ctrl(p_user_used, TRUE);
}


static RET_CODE on_ca_accept_burse_update(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	accept_ca_burse_info(p_cont, (burses_info_t *)para2);

	return SUCCESS;
}

BEGIN_KEYMAP(card_info_root_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
	ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(card_info_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(card_info_root_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_CA_CARD_INFO, on_conditional_accept_info_update)
	ON_COMMAND(MSG_CA_BUR_INFO, on_ca_accept_burse_update)
	ON_COMMAND(MSG_EXIT, on_exit_card_info)
END_MSGPROC(card_info_root_proc, ui_comm_root_proc)


