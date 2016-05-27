/****************************************************************************

****************************************************************************/
#ifndef __UI_XTIME_SET_H__
#define __UI_XTIME_SET_H__


#define XTIME_SET_X			SYS_LEFT_CONT_W
#define XTIME_SET_Y			SYS_LEFT_CONT_Y
#define XTIME_SET_W				SYS_RIGHT_CONT_W
#define XTIME_SET_H				SYS_LEFT_CONT_H
#define UI_XTIME_SET_SET_X        60
#define UI_XTIME_SET_SET_W        (XTIME_SET_W-2*UI_XTIME_SET_SET_X)
#define UI_XTIME_SET_SET_W_L	(UI_XTIME_SET_SET_W/3)
#define UI_XTIME_SET_SET_W_R	(UI_XTIME_SET_SET_W/3*2)

/* coordinate */

#define XTIME_SET_ITEM_CNT      5
#define XTIME_SET_ITEM_X        UI_XTIME_SET_SET_X
#define XTIME_SET_ITEM_Y        50
#define XTIME_SET_ITEM_LW       UI_XTIME_SET_SET_W_L
#define XTIME_SET_ITEM_RW       UI_XTIME_SET_SET_W_R
#define XTIME_SET_ITEM_H        COMM_ITEM_H
#define XTIME_SET_ITEM_V_GAP    15

/* rect style */
#define RSI_XTIME_SET_FRM       RSI_WINDOW_1
/* font style */

#define  RSI_XTIME_SET_INFO       RSI_BOX_1


/* others */
#define XTIME_SET_GMT_OFFSET_NUM 48
#define XTIME_SET_GMT_OFFSET_CITY_LEN 200

RET_CODE open_xtime_set(u32 para1, u32 para2);

#endif

