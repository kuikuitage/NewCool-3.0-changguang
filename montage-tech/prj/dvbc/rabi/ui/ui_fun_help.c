#include "ui_common.h"
#include "ui_fun_help.h"

#define FUN_HELP_RIGHT  (40)
#define FUN_HELP_X  (COMM_BG_W - FUN_HELP_W)
#define FUN_HELP_Y  450
#define FUN_HELP_W  (150 + FUN_HELP_RIGHT)
#define FUN_HELP_H  60
#define FUN_HELP_BMP_X  5
#define FUN_HELP_BMP_Y  ((FUN_HELP_H-FUN_HELP_BMP_H)/2)
#define FUN_HELP_BMP_W  50
#define FUN_HELP_BMP_H  50

#define FUN_HELP_TXT_X  (FUN_HELP_BMP_X+FUN_HELP_BMP_W)
#define FUN_HELP_TXT_Y  ((FUN_HELP_H-FUN_HELP_TXT_H)/2)
#define FUN_HELP_TXT_W  (FUN_HELP_W-FUN_HELP_TXT_X)
#define FUN_HELP_TXT_H  FUN_HELP_BMP_H

#define FUN_HELP_TIME_OUT		5000

enum{
	IDC_FUN_HELP_CONT,
	IDC_FUN_HELP_TXT,
	IDC_FUN_HELP_BMP,
};

RET_CODE fun_help_cont_proc(control_t *p_ctrl, u16 msg, 
                            u32 para1, u32 para2);

RET_CODE open_fun_help(u32 para1, u32 para2)
{
	control_t *p_cont, *p_bottom_help_con, *p_bottom_help_ic, *p_bottom_help_txt;

       p_cont = fw_find_root_by_id(ROOT_ID_FUN_HELP);
	if(NULL != p_cont)
	{
		fw_tmr_destroy(ROOT_ID_FUN_HELP, MSG_CLOSE_MENU);
              ctrl_paint_ctrl(p_cont, TRUE);
		return SUCCESS;
	}
	p_cont = fw_create_mainwin(ROOT_ID_FUN_HELP,
                                    FUN_HELP_X, FUN_HELP_Y,
                                    FUN_HELP_W, FUN_HELP_H,
                                    ROOT_ID_INVALID, 0, 
                                    OBJ_ATTR_INACTIVE, 0);
    if (p_cont == NULL)
    {
      return ERR_FAILURE;
    }
    ctrl_set_rstyle(p_cont, RSI_TRANSPARENT,RSI_TRANSPARENT, RSI_TRANSPARENT);
	ctrl_set_proc(p_cont, fun_help_cont_proc);
	p_bottom_help_con = ctrl_create_ctrl(CTRL_CONT, IDC_FUN_HELP_CONT,
					 0, 0, FUN_HELP_W,
					 FUN_HELP_H, p_cont, 0);
	ctrl_set_rstyle(p_bottom_help_con, RSI_MEDIO_LEFT_BG, RSI_MEDIO_LEFT_BG, RSI_MEDIO_LEFT_BG);
	p_bottom_help_ic= ctrl_create_ctrl(CTRL_BMAP, IDC_FUN_HELP_BMP,
					 FUN_HELP_BMP_X, FUN_HELP_BMP_Y, FUN_HELP_BMP_W,
					 FUN_HELP_BMP_H, p_bottom_help_con, 0);
	bmap_set_align_type(p_bottom_help_ic, STL_CENTER|STL_VCENTER);
	bmap_set_content_by_id(p_bottom_help_ic, IM_XKEY_FN);
	p_bottom_help_txt= ctrl_create_ctrl(CTRL_TEXT, IDC_FUN_HELP_TXT,
					 FUN_HELP_TXT_X, FUN_HELP_TXT_Y, FUN_HELP_TXT_W,
					 FUN_HELP_TXT_H, p_bottom_help_con, 0);
	ctrl_set_rstyle(p_bottom_help_txt, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_bottom_help_txt, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	text_set_align_type(p_bottom_help_txt, STL_LEFT | STL_VCENTER);
	text_set_content_type(p_bottom_help_txt, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_bottom_help_txt, IDS_FN);

	ctrl_paint_ctrl(p_cont, FALSE);
	return SUCCESS;
}

RET_CODE close_fun_help_later(void)
{
	fw_tmr_create(ROOT_ID_FUN_HELP, MSG_CLOSE_MENU, FUN_HELP_TIME_OUT, FALSE);
	return SUCCESS;
}

RET_CODE timer_close_fun_help(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	if(NULL!=fw_find_root_by_id(ROOT_ID_PROG_BAR)
			||NULL!=fw_find_root_by_id(ROOT_ID_SMALL_LIST))
	{
		fw_tmr_destroy(ROOT_ID_FUN_HELP, MSG_CLOSE_MENU);
		return SUCCESS;
	}
	fw_tmr_destroy(ROOT_ID_FUN_HELP, MSG_CLOSE_MENU);
//	fw_destroy_mainwin_by_id(ROOT_ID_FUN_HELP);
	manage_close_menu(ROOT_ID_FUN_HELP, 0, 0);
	return SUCCESS;
}

BEGIN_MSGPROC(fun_help_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CLOSE_MENU, timer_close_fun_help)
END_MSGPROC(fun_help_cont_proc, ui_comm_root_proc)

