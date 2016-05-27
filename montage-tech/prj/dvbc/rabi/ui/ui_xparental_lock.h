/****************************************************************************

****************************************************************************/
#ifndef __UI_XPARENTAL_LOCK_H__
#define __UI_XPARENTAL_LOCK_H__

/* coordinate */
#define XPARENTAL_LOCK_CONT_X        ((SCREEN_WIDTH - XPARENTAL_LOCK_CONT_W) / 2)
#define XPARENTAL_LOCK_CONT_H        420

#define XPARENTAL_LOCK_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define XPARENTAL_LOCK_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define XPARENTAL_LOCK_ITEM_LW       COMM_ITEM_LW
#define XPARENTAL_LOCK_ITEM_RW       (COMM_ITEM_MAX_WIDTH - XPARENTAL_LOCK_ITEM_LW)
#define XPARENTAL_LOCK_ITEM_H        COMM_ITEM_H
#define XPARENTAL_LOCK_ITEM_V_GAP    16

#define LOCK_X			SYS_LEFT_CONT_W
#define LOCK_Y			SYS_LEFT_CONT_Y
#define LOCK_W				SYS_RIGHT_CONT_W
#define LOCK_H				SYS_LEFT_CONT_H
#define UI_LOCK_SET_X        60
#define UI_LOCK_SET_W        (LOCK_W-2*UI_LOCK_SET_X)
#define UI_LOCK_SET_W_L	(UI_LOCK_SET_W/3)
#define UI_LOCK_SET_W_R	(UI_LOCK_SET_W/3*2)


/* rect style */
#define RSI_XPARENTAL_LOCK_FRM       RSI_WINDOW_1
/* font style */

/* others */

RET_CODE preopen_xparental_lock(u32 para1, u32 para2);

RET_CODE open_xparental_lock(u32 para1, u32 para2);

#endif


