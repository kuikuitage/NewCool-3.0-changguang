/****************************************************************************

****************************************************************************/
#ifndef __UI_RANGE_SEARCH_H__
#define __UI_RANGE_SEARCH_H__

/* coordinate */
#define RANGE_SEARCH_ITEM_CNT      5
#define RANGE_SEARCH_ITEM_X        COMM_ITEM_OX_IN_ROOT
#define RANGE_SEARCH_ITEM_Y        COMM_ITEM_OY_IN_ROOT
#define RANGE_SEARCH_ITEM_LW       COMM_ITEM_LW
#define RANGE_SEARCH_ITEM_RW       (COMM_ITEM_MAX_WIDTH - RANGE_SEARCH_ITEM_LW)
#define RANGE_SEARCH_ITEM_H        COMM_ITEM_H
#define RANGE_SEARCH_ITEM_V_GAP    10

/* rect style */

/* font style */

RET_CODE open_range_search(u32 para1, u32 para2);

#endif