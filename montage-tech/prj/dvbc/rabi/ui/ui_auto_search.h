/****************************************************************************

****************************************************************************/
#ifndef __UI_AUTO_SEARCH_H__
#define __UI_AUTO_SEARCH_H__

/* coordinate */
#define AUTO_SEARCH_ITEM_CNT      4
#define AUTO_SEARCH_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define AUTO_SEARCH_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define AUTO_SEARCH_ITEM_LW       COMM_ITEM_LW
#define AUTO_SEARCH_ITEM_RW       (COMM_ITEM_MAX_WIDTH - AUTO_SEARCH_ITEM_LW)
#define AUTO_SEARCH_ITEM_H        COMM_ITEM_H
#define AUTO_SEARCH_ITEM_V_GAP    10

/* rect style */

/* font style */

RET_CODE open_auto_search(u32 para1, u32 para2);

#endif