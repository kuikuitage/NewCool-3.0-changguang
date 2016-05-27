/****************************************************************************

****************************************************************************/
#ifndef __UI_SET_SEARCH_H__
#define __UI_SET_SEARCH_H__

/* coordinate */
#define SET_SEARCH_CONT_X        ((SCREEN_WIDTH - SET_SEARCH_CONT_W) / 2)
#define SET_SEARCH_CONT_Y        COMM_BG_Y
#define SET_SEARCH_CONT_W        COMM_BG_W

#define SET_SEARCH_ITEM_CNT      6
#define SET_SEARCH_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define SET_SEARCH_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define SET_SEARCH_ITEM_LW       COMM_ITEM_LW
#define SET_SEARCH_ITEM_RW       (COMM_ITEM_MAX_WIDTH - SET_SEARCH_ITEM_LW)
#define SET_SEARCH_ITEM_H        COMM_ITEM_H
#define SET_SEARCH_ITEM_V_GAP    16

#define SET_SEARCH_INFO_X        SET_SEARCH_ITEM_X
#define SET_SEARCH_INFO_Y        (COMM_BG_H - SET_SEARCH_INFO_H - 50)
#define SET_SEARCH_INFO_W        COMM_ITEM_MAX_WIDTH
#define SET_SEARCH_INFO_H        100

#define SET_SEARCH_LINE_H        18

/* rect style */
#define RSI_SET_SEARCH_FRM       RSI_WINDOW_1
#define RSI_SET_SEARCH_INFO     RSI_IGNORE
#define RSI_SET_SEARCH_BTN_SH  RSI_ITEM_1_SH
#define RSI_SET_SEARCH_BTN_HL  RSI_ITEM_1_HL

/* font style */
#define FSI_SET_SEARCH_INFO      FSI_WHITE
/* others */

RET_CODE open_set_search(u32 para1, u32 para2);

#endif

