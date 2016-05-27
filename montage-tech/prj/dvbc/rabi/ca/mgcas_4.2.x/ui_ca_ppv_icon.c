/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "ui_ca_public.h"
#include "ui_ca_ppv_icon.h"

enum ppv_icon_ctrl_id
{
  IDC_PPV_ICON_CONT1 = 1,
  
};

RET_CODE ca_ppv_icon_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_ca_ppv_icon(u32 para1, u32 para2)
{
	control_t *p_cont = NULL,*p_ctrl = NULL;

	if(!ui_is_fullscreen_menu(fw_get_focus_id()))
		return ERR_FAILURE;
	if(fw_find_root_by_id(ROOT_ID_CA_PPV_ICON) != NULL)
		return ERR_FAILURE;

	p_cont = fw_create_mainwin(ROOT_ID_CA_PPV_ICON,
	                          PPV_ICON_CONT_X, PPV_ICON_CONT_Y, 
	                          PPV_ICON_CONT_W, PPV_ICON_CONT_H,
	                          0, 0,
	                          OBJ_ATTR_INACTIVE, 0);
	ctrl_set_proc(p_cont, ca_ppv_icon_cont_proc);
	ctrl_set_rstyle(p_cont, RSI_PPV_ICON_TXT, RSI_PPV_ICON_TXT, RSI_PPV_ICON_TXT);
	if (p_cont == NULL)
	{
	return FALSE;
	}
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_PPV_ICON_CONT1, 
	                                PPV_ICON_TXT_X, PPV_ICON_TXT_Y, 
	                                PPV_ICON_TXT_W, PPV_ICON_TXT_H, 
	                                p_cont, 0);
	text_set_font_style(p_ctrl, FSI_PPV_ICON_TXT, FSI_PPV_ICON_TXT, FSI_PPV_ICON_TXT);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_by_strid(p_ctrl, IDS_CA_PPV_PROG);

	fw_tmr_create(ROOT_ID_CA_PPV_ICON, MSG_CA_CANCEL_PPV_ICON, PPV_ICON_AUTOCLOSE_MS, FALSE);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
	return SUCCESS;
}

void close_ca_ppv_icon(void)
{
  fw_destroy_mainwin_by_id(ROOT_ID_CA_PPV_ICON);
}

static RET_CODE on_ppv_icon_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  close_ca_ppv_icon();
  fw_tmr_destroy(ROOT_ID_CA_PPV_ICON, MSG_CA_CANCEL_PPV_ICON);

  return SUCCESS;
}

BEGIN_MSGPROC(ca_ppv_icon_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_CA_CANCEL_PPV_ICON, on_ppv_icon_exit)
END_MSGPROC(ca_ppv_icon_cont_proc, cont_class_proc)


