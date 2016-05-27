/****************************************************************************

****************************************************************************/
#ifndef __UI_OSD_SET_H__
#define __UI_OSD_SET_H__

/* coordinate */
#define OSD_SET_CONT_X        ((SCREEN_WIDTH - OSD_SET_CONT_W) / 2)
#define OSD_SET_CONT_H        420

#if ENABLE_TTX_SUBTITLE
#define OSD_SET_ITEM_CNT     3// 4
#else
#define OSD_SET_ITEM_CNT     2
#endif

#define OSD_SET_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define OSD_SET_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define OSD_SET_ITEM_LW       COMM_ITEM_LW
#define OSD_SET_ITEM_RW       (COMM_ITEM_MAX_WIDTH - OSD_SET_ITEM_LW)
#define OSD_SET_ITEM_H        COMM_ITEM_H
#define OSD_SET_ITEM_V_GAP    16

/* rect style */
#define RSI_OSD_SET_FRM       RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_osd_set(u32 para1, u32 para2);

#endif



