/****************************************************************************

 ****************************************************************************/
#ifndef __UI_XSTB_NAME_H__
#define __UI_XSTB_NAME_H__

/* coordinate */

#define XSTB_NAME_X			SYS_LEFT_CONT_W
#define XSTB_NAME_Y			SYS_LEFT_CONT_Y
#define XSTB_NAME_W				SYS_RIGHT_CONT_W
#define XSTB_NAME_H				SYS_LEFT_CONT_H
#define UI_XSTB_NAME_SET_X        60
#define UI_XSTB_NAME_SET_W        (XSTB_NAME_W-2*UI_XSTB_NAME_SET_X)
#define UI_XSTB_NAME_SET_W_L	(UI_XSTB_NAME_SET_W/3)
#define UI_XSTB_NAME_SET_W_R	(UI_XSTB_NAME_SET_W/3*2)


#define XSTB_NAME_ITEM_CNT		1
#define XSTB_NAME_ITEM_X			UI_XSTB_NAME_SET_X
#define XSTB_NAME_ITEM_Y			XSTB_NAME_Y
#define XSTB_NAME_ITEM_LW		UI_XSTB_NAME_SET_W_L
#define XSTB_NAME_ITEM_RW		UI_XSTB_NAME_SET_W_R
#define XSTB_NAME_ITEM_H			COMM_ITEM_H
#define XSTB_NAME_ITEM_V_GAP		16

/* rect style */
#define XSTB_NAME_OTHERS_FRM		RSI_WINDOW_1
/* font style */

#define MAX_XDEVICE_NAME_LENGTH 64

RET_CODE open_xstbname (u32 para1, u32 para2);

#endif

