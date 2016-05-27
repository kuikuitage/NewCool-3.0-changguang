/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
//#include "sowell.h"
#include "ui_ca_card_info.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_SMALL_BG,
  IDC_CA_CARD_NUMBER,
  IDC_CA_CARD_TYPE,
  IDC_CA_CARD_STATE,
  IDC_CA_CARD_AREA,
  IDC_CA_CARD_WORK_TIME,
  IDC_CA_CARD_WATCH_LEVEL,
  IDC_CA_VER,
  IDC_CA_MOTHER_CARD_NUMBER,
};

static u8 asc_str[32] = {0};
static u16 uni_str[32] = {0};
static u8 g_is_child_card = 0;

RET_CODE ca_card_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_ca_card_info(u32 para1, u32 para2)
{
  control_t *p_cont = NULL;
  control_t *p_ctrl = NULL;
  u8 i;
  u16 y;
  u16 str[] = 
  {
    IDS_CA_SMART_CARD_NUMBER,
    IDS_CA_TYPE,
    IDS_CA_CARD_STATE,
    IDS_CA_CARD_ZONE_CODE,
    IDS_CA_WORK_TIME,
    IDS_CA_WATCH_LEVEL,
    IDS_CA_VERSION,
    IDS_CA_MASTER_CARD_ID,
  };

  p_cont = ui_comm_right_root_create(ROOT_ID_CA_CARD_INFO, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ca_card_info_proc);

  y = ACCEPT_INFO_CA_INFO_ITEM_Y;
  for(i=0; i < CA_CARD_INFO_CNT; i++)
  {

    switch(i)
    {
      case 0:
        p_ctrl = ui_comm_static_create(p_cont,(u8)(IDC_CA_CARD_NUMBER+ i),
                                                              ACCEPT_INFO_CA_INFO_ITEM_X,
                                                              y,
                                                              ACCEPT_INFO_CA_INFO_ITEM_LW,
                                                              ACCEPT_INFO_CA_INFO_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl, str[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        break;
      case 1:
        p_ctrl = ui_comm_static_create(p_cont,(u8)(IDC_CA_CARD_NUMBER+ i),
                                                              ACCEPT_INFO_CA_INFO_ITEM_X,
                                                              y,
                                                              ACCEPT_INFO_CA_INFO_ITEM_LW,
                                                              ACCEPT_INFO_CA_INFO_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl, str[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_STRID);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        break;
      case 2: 
         p_ctrl = ui_comm_static_create(p_cont,(u8)(IDC_CA_CARD_NUMBER+ i),
                                                              ACCEPT_INFO_CA_INFO_ITEM_X,
                                                              y,
                                                              ACCEPT_INFO_CA_INFO_ITEM_LW,
                                                              ACCEPT_INFO_CA_INFO_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl, str[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_STRID);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        break;
      case 3:
        p_ctrl = ui_comm_static_create(p_cont,(u8)(IDC_CA_CARD_NUMBER+ i),
                                                            ACCEPT_INFO_CA_INFO_ITEM_X,
                                                            y,
                                                            ACCEPT_INFO_CA_INFO_ITEM_LW,
                                                            ACCEPT_INFO_CA_INFO_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl, str[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        break;
      case 4:
        p_ctrl = ui_comm_static_create(p_cont,(u8)(IDC_CA_CARD_NUMBER+ i),
                                                              ACCEPT_INFO_CA_INFO_ITEM_X,
                                                              y,
                                                              ACCEPT_INFO_CA_INFO_ITEM_LW,
                                                              ACCEPT_INFO_CA_INFO_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl, str[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        break;
      case 5:
        p_ctrl = ui_comm_static_create(p_cont,(u8)(IDC_CA_CARD_NUMBER+ i),
                                                              ACCEPT_INFO_CA_INFO_ITEM_X,
                                                              y,
                                                              ACCEPT_INFO_CA_INFO_ITEM_LW,
                                                              ACCEPT_INFO_CA_INFO_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl, str[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_STRID);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        break;
      case 6:
        p_ctrl = ui_comm_static_create(p_cont,(u8)(IDC_CA_CARD_NUMBER+ i),
                                                            ACCEPT_INFO_CA_INFO_ITEM_X,
                                                            y,
                                                            ACCEPT_INFO_CA_INFO_ITEM_LW,
                                                            ACCEPT_INFO_CA_INFO_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl, str[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        break;
      case 7:
        p_ctrl = ui_comm_static_create(p_cont,(u8)(IDC_CA_CARD_NUMBER+ i),
                                                            ACCEPT_INFO_CA_INFO_ITEM_X,
                                                            y,
                                                            ACCEPT_INFO_CA_INFO_ITEM_LW,
                                                            ACCEPT_INFO_CA_INFO_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl, str[i]);
        ui_comm_static_set_param(p_ctrl, TEXT_STRTYPE_UNICODE);
        ui_comm_static_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        ctrl_set_sts(p_ctrl,OBJ_STS_HIDE);
        break;
      default:
        break;
    }
    
    ctrl_set_related_id(p_ctrl,
                        0,                                     /* left */
                        (u8)((i - 1 +
                              CA_CARD_INFO_CNT) %
                             CA_CARD_INFO_CNT + IDC_CA_CARD_NUMBER),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % CA_CARD_INFO_CNT + IDC_CA_CARD_NUMBER));/* down */

    y += COMM_CTRL_H + ACCEPT_CA_INFO_ITEM_V_GAP;
  }

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  //send command to get operator info.
#ifndef WIN32 
  ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0, 0);
  ui_ca_do_cmd(CAS_CMD_MON_CHILD_STATUS_GET, 0, 0);
#endif
  return SUCCESS;
}

static RET_CODE on_ca_card_info(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	control_t *p_ctrl = NULL;
	cas_card_info_t *p_card_info = NULL;
	chlid_card_status_info *child_card_status = NULL;

	if(FALSE == ui_is_smart_card_insert())
	{
		return SUCCESS;
	}
	if(msg == MSG_CA_CARD_INFO)
	{
		p_card_info = (cas_card_info_t *)para2;

		OS_PRINTF("\r\n****on_ca_card_info para2[0x%x]***\n",para2);

		if(p_card_info == NULL)
		{

		}
		else
		{
			//card num
			p_ctrl = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_NUMBER);
			sprintf((char *)asc_str, "%02X%02X%02X%02X%02X%02X%02X%02X", p_card_info->sn[0], p_card_info->sn[1], p_card_info->sn[2], p_card_info->sn[3],
			p_card_info->sn[4], p_card_info->sn[5], p_card_info->sn[6], p_card_info->sn[7]);
			str_asc2uni(asc_str, uni_str);
			ui_comm_static_set_content_by_unistr(p_ctrl, uni_str);

			//card type
			p_ctrl = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_TYPE);
			if(p_card_info->card_type == 0)
			{
				ui_comm_static_set_content_by_strid(p_ctrl, IDS_CA_MOTHER_CARD);
				g_is_child_card = 0;
			}
			else if(p_card_info->card_type == 1)
			{
				ui_comm_static_set_content_by_strid(p_ctrl, IDS_CA_CHILD_CARD);
				g_is_child_card = 1;
			}

			//card state
			p_ctrl = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_STATE);
			if(p_card_info->paterner_stat == 0)
			{
				ui_comm_static_set_content_by_strid(p_ctrl, IDS_OPERATOR_FAILED);
			}
			else if(p_card_info->paterner_stat == 1)
			{
				ui_comm_static_set_content_by_strid(p_ctrl, IDS_CA_STB_PAIR);
			}
			else if(p_card_info->paterner_stat == 2)
			{
				ui_comm_static_set_content_by_strid(p_ctrl, IDS_CA_NEED_SMC_STB_PAIR);
			}

			//card area code
			p_ctrl = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_AREA);
			sprintf((char *)asc_str, "%02d%02d%02d%02d",p_card_info->area_code[0], p_card_info->area_code[1],
			p_card_info->area_code[2], p_card_info->area_code[3]);
			str_asc2uni(asc_str, uni_str);
			ui_comm_static_set_content_by_unistr(p_ctrl, uni_str);

			//work time
			p_ctrl = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_WORK_TIME);
			sprintf((char *)asc_str, "%02d:%02d:%02d ~ %02d:%02d:%02d ", p_card_info->work_time.start_hour,p_card_info->work_time.start_minute,p_card_info->work_time.start_second, 
									p_card_info->work_time.end_hour,p_card_info->work_time.end_minute,p_card_info->work_time.end_second); 
			str_asc2uni(asc_str, uni_str);
			ui_comm_static_set_content_by_unistr(p_ctrl, uni_str);

			//watch level
			p_ctrl = ctrl_get_child_by_id(p_cont, IDC_CA_CARD_WATCH_LEVEL);
			if(p_card_info->cas_rating == 0)
			{
				ui_comm_static_set_content_by_strid(p_ctrl, IDS_FOUR_YEARS_UP);
			}
			else if(p_card_info->cas_rating == 1)
			{
				ui_comm_static_set_content_by_strid(p_ctrl, IDS_EIGHT_YEARS_UP);
			}
			else if(p_card_info->cas_rating == 2)
			{
				ui_comm_static_set_content_by_strid(p_ctrl, IDS_TWELVE_YEARS_UP);
			}
			else if(p_card_info->cas_rating == 3)
			{
				ui_comm_static_set_content_by_strid(p_ctrl, IDS_SIXTEEN_YEARS_UP);
			}
			else 
			{
				ui_comm_static_set_content_by_strid(p_ctrl, IDS_CA_RECEIVED_DATA_FAIL);
			}

			//pin state

			//ca ver
			p_ctrl = ctrl_get_child_by_id(p_cont, IDC_CA_VER);
			sprintf((char *)asc_str, "V3.1");
			str_asc2uni(asc_str, uni_str);
			ui_comm_static_set_content_by_unistr(p_ctrl, uni_str);

		}
		ctrl_paint_ctrl(p_cont, TRUE);
	}
	else
	{  
		p_ctrl = ctrl_get_child_by_id(p_cont, IDC_CA_MOTHER_CARD_NUMBER);
		child_card_status = (chlid_card_status_info *)para2;

		if(g_is_child_card)
		{
			sprintf((char *)asc_str, "%02X%02X%02X%02X%02X%02X%02X%02Xs", 
			child_card_status->parent_card_sn[0], child_card_status->parent_card_sn[1],
			child_card_status->parent_card_sn[2], child_card_status->parent_card_sn[3],
			child_card_status->parent_card_sn[4], child_card_status->parent_card_sn[5],
			child_card_status->parent_card_sn[6], child_card_status->parent_card_sn[7]);
			str_asc2uni(asc_str, uni_str);
			ui_comm_static_set_content_by_unistr(p_ctrl, uni_str);
			ctrl_set_sts(p_ctrl,OBJ_STS_SHOW);
		}
		else
		{
			ctrl_set_sts(p_ctrl,OBJ_STS_HIDE); 
		}
		ctrl_paint_ctrl(p_cont, TRUE);
	}

	return SUCCESS;
}


BEGIN_MSGPROC(ca_card_info_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CA_CARD_INFO, on_ca_card_info)
  ON_COMMAND(MSG_CA_MON_CHILD_INFO, on_ca_card_info)
  //ON_COMMAND(MSG_EXIT, on_exit_ca_card_info)
END_MSGPROC(ca_card_info_proc, ui_comm_root_proc)

 
