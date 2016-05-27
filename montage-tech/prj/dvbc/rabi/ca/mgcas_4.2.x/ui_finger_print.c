/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#include "ui_common.h"
#include "ui_finger_print.h"
#include "ui_ca_public.h"

enum control_id
{
	IDC_FINGER_PRINT = 1,
};

static BOOL g_finger_is_repaint;
static BOOL g_is_finger_show = FALSE;
static u32 g_ca_finger_content = 0;

RET_CODE open_finger_print(u32 para1, u32 para2)
{
	control_t *p_cont, *p_ctrl;
	u32 size = 0;
	
	if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) != NULL)
	{
		ui_close_finger_menu();
	}
	
	if(!ui_is_fullscreen_menu(fw_get_focus_id()))
	{
		return SUCCESS;
	}

	size = strlen((s8 *)para2) + 1;
	g_ca_finger_content = (u32)mtos_malloc(size);
	memset((void *)g_ca_finger_content, 0, size);
	memcpy((u8 *)g_ca_finger_content, (u8 *)para2, size);

	p_cont = fw_create_mainwin(ROOT_ID_FINGER_PRINT,
			                       FINGER_PRINT_X, FINGER_PRINT_Y,
			                       FINGER_PRINT_W,FINGER_PRINT_H,
			                       ROOT_ID_INVALID,0,OBJ_ATTR_INACTIVE,0);
	ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
	if (p_cont == NULL)
	{
		return ERR_FAILURE;
	}
  
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_FINGER_PRINT,
	                      FINGER_PRINT_TEXT_X, FINGER_PRINT_TEXT_Y,
	                      FINGER_PRINT_TEXT_W, FINGER_PRINT_TEXT_H,
	                      p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE); 
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
  
	OS_PRINTF("%s      :p_finger_info->data = %s\n",__FUNCTION__, para2);
	text_set_content_by_ascstr(p_ctrl, (u8*)g_ca_finger_content);

	ctrl_paint_ctrl(p_cont, FALSE);
	if(!g_finger_is_repaint)
	{
		fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CA_HIDE_FINGER, (u32)200000, FALSE);
		g_finger_is_repaint = TRUE;
	}

	return SUCCESS;
}

void ui_set_finger_show(BOOL is_show)
{
	g_is_finger_show = is_show;
}

BOOL ui_is_finger_show(void)
{
	return g_is_finger_show;
}

BOOL ui_finger_is_repaint(void)
{
	return g_finger_is_repaint;
}

RET_CODE ui_close_finger_menu(void)
{
	g_finger_is_repaint = FALSE;
	ui_set_finger_show(FALSE);
	
	OS_PRINTF("Truely close the finger\n");
	fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CA_HIDE_FINGER);
	fw_destroy_mainwin_by_id(ROOT_ID_FINGER_PRINT);
	return SUCCESS;
}

BOOL ui_finger_is_exsit(void)
{
	if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) != NULL)
	{
		return TRUE;
	}
	return FALSE;
}

void ui_close_finger_mainwin()
{
	OS_PRINTF("On the surface close the finger\n");
	fw_destroy_mainwin_by_id(ROOT_ID_FINGER_PRINT);
}

void ui_finger_print_repaint()
{
	if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) == NULL)//check it to paint finger
	{
		open_finger_print(0,(u32)g_ca_finger_content);
	}
}


