/****************************************************************************

****************************************************************************/
#ifndef __UI_INFO_H__
#define __UI_INFO_H__

#define  INFO_SYSTEM_VERSION	(1)

/* coordinate */
#define INFO_TITLE_X      120
#define INFO_TITLE_Y      100
#define INFO_TITLE_W      1040
#define INFO_TITLE_H      50


#define INFO_LIST_X        60
#define INFO_LIST_Y        60
#define INFO_LIST_W       (RIGHT_ROOT_W-2*INFO_LIST_X)
#define INFO_LIST_H       500

#define INFO_LIST_MID_L     20
#define INFO_LIST_MID_T     20
#define INFO_LIST_MID_W   (INFO_LIST_W - 2 * INFO_LIST_MID_L)
#define INFO_LIST_MID_H    ((INFO_LIST_ITEM_H + INFO_LIST_ITEM_VGAP) * INFO_LIST_CNT - INFO_LIST_ITEM_VGAP)

#define INFO_LIST_ITEM_H        30
#define INFO_LIST_ITEM_VGAP    8

/* rect style */
//#define RSI_INFO_FRM       RSI_WINDOW_1
#define RSI_INFO_LIST      RSI_BOX_1
#define INFO_TITLE_TXT           FSI_VERMILION
#define INFO_TITLE_STYLE RSI_ITEM_1_SH
/* font style */

/* others */
#if INFO_SYSTEM_VERSION
#define INFO_LIST_CNT      (7)
#else
#define INFO_LIST_CNT      (6)

#endif
#define INFO_LIST_FIELD   2

RET_CODE open_xinfo(u32 para1, u32 para2);

#endif


