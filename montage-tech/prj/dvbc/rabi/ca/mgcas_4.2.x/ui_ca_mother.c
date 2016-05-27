/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ca_public.h"
#include "ui_ca_mother.h"
#include "ui_notify.h"
#include "ui_xsys_set.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FRM_BUTTONS,
};

enum card_manage_id
{
  IDC_TITLE1 = 1, 
  IDC_TITLE2,
  IDC_TITLE3,
};

#define PARENT_CARD_SN_MAX_LEN         8
static u16 ca_mother_cont_keymap(u16 key);
RET_CODE ca_mother_cont_proc(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);

RET_CODE open_ca_mother(u32 para1, u32 para2)
{
	control_t *p_cont, *p_ctrl = NULL;
	u8 i;
	u16 y;
	u16 str_id[ACCEPT_CA_MOTHER_TITLE_CNT] =
	{ 
		IDS_CARD_TYPE, 
		IDS_MOTHER_CARD_NUM,
		IDS_CA_CARD_ACTIVE_TIME,
	};

	p_cont = ui_comm_right_root_create(ROOT_ID_CA_MOTHER, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
	ctrl_set_keymap(p_cont, ca_mother_cont_keymap);
	ctrl_set_proc(p_cont, ca_mother_cont_proc);

	//card manage
	y = ACCEPT_CA_MOTHER_ITEM_Y;
	for (i = 0; i<ACCEPT_CA_MOTHER_TITLE_CNT; i++)
	{
		switch(i)
		{
			case 0:
				p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_TITLE1 + i),
				                              ACCEPT_CA_MOTHER_ITEM_X,y,
				                              ACCEPT_CA_MOTHER_ITEM_LW,
				                              ACCEPT_CA_MOTHER_ITEM_RW);
				ui_comm_static_set_static_txt(p_ctrl, str_id[i]);
				ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_STRID);
				break;

			case 1:
			case 2:
				p_ctrl = ui_comm_static_create(p_cont, (u8)(IDC_TITLE1 + i),
				                               ACCEPT_CA_MOTHER_ITEM_X, y,
				                               ACCEPT_CA_MOTHER_ITEM_LW,
				                               ACCEPT_CA_MOTHER_ITEM_RW);
				ui_comm_static_set_static_txt(p_ctrl, str_id[i]);
				ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
				ctrl_set_sts(p_ctrl,OBJ_STS_HIDE);
				break;
			default:
				break;    
		}
		ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
		ctrl_set_related_id(p_ctrl,
		            0,                                     /* left */
		            (u8)((i-1+ACCEPT_CA_MOTHER_TITLE_CNT)%ACCEPT_CA_MOTHER_TITLE_CNT + IDC_TITLE1),           /* up */
		            0,                                     /* right */
		           (u8)((i + 1) % ACCEPT_CA_MOTHER_TITLE_CNT + IDC_TITLE1));/* down */
		y += ACCEPT_CA_MOTHER_ITEM_H + ACCEPT_CA_MOTHER_ITEM_V_GAP;
	}

	ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
	ui_ca_do_cmd((u32)CAS_CMD_MON_CHILD_STATUS_GET,0,0);
	return SUCCESS;
}

static void set_ca_child_mother_info(control_t *p_cont, chlid_card_status_info *p_card)
{
	control_t *p_frm = p_cont;
	control_t *p_card_style = ctrl_get_child_by_id(p_frm, IDC_TITLE1);
	control_t *p_mon_code = ctrl_get_child_by_id(p_frm, IDC_TITLE2);
	control_t *p_over_time = ctrl_get_child_by_id(p_frm, IDC_TITLE3);
	u8 asc_str[64],i = 0,j = 0;
	rect_t dlg_rc =
	{
		RESTORE_ASK_L,
		RESTORE_ASK_T,
		RESTORE_ASK_R,
		RESTORE_ASK_B,
	};

	if(p_card == NULL || ui_is_smart_card_insert() == FALSE)
	{
		ui_comm_cfmdlg_open(&dlg_rc, IDS_MG_CARD_NOT_READY, NULL, 2000);
	}
	else
	{
		switch(p_card->is_child)
		{
			case 0:
				ui_comm_static_set_content_by_strid(p_card_style, IDS_CA_MOTHER_CARD);
				break;
			case 1:
				ui_comm_static_set_content_by_strid(p_card_style, IDS_CA_CHILD_CARD);

				for(i = 0, j = 0; i < PARENT_CARD_SN_MAX_LEN; i += 2, j++)
				{
					sprintf((char *)(&asc_str[i]), "%02x", p_card->parent_card_sn[j]);
					OS_PRINTF("%s,    %d, parent_card_sn: %02x\n",__FUNCTION__,__LINE__,p_card->parent_card_sn[j]);
				}
				OS_PRINTF("%s,    %d, parent_card_sn: %s\n",__FUNCTION__,__LINE__,asc_str);
				ui_comm_static_set_content_by_ascstr(p_mon_code, asc_str);
				ctrl_set_sts(p_mon_code,OBJ_STS_SHOW);

				sprintf((char *)asc_str, "%d", p_card->child_card_remain_time[0]);
				OS_PRINTF("child_card_remain_time : %d\n",p_card->child_card_remain_time[0]);
				ui_comm_static_set_content_by_ascstr(p_over_time, asc_str);
				ctrl_set_sts(p_over_time,OBJ_STS_SHOW);
				break;
			default:
				ui_comm_static_set_content_by_strid(p_card_style, RSC_INVALID_ID);
				break;
		}
	}
	return;
}

static RET_CODE on_ca_get_mon_child_info(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	set_ca_child_mother_info(p_cont, (chlid_card_status_info *)para2);
	ctrl_paint_ctrl(p_cont, TRUE);
	return SUCCESS;
}

static RET_CODE on_is_card_move(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	u32 event_id = para2;

	switch(event_id)
	{
		case CAS_S_ADPT_CARD_REMOVE:
			manage_close_menu(ROOT_ID_CA_MOTHER,0,0);
			break;
		default:
			break;
	}

	return SUCCESS;
}


BEGIN_KEYMAP(ca_mother_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ca_mother_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ca_mother_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_MON_CHILD_INFO, on_ca_get_mon_child_info)
  ON_COMMAND(MSG_CA_EVT_NOTIFY, on_is_card_move)
END_MSGPROC(ca_mother_cont_proc, ui_comm_root_proc)




