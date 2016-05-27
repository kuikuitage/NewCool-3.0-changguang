/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_LEVEL_H__
#define __UI_CONDITIONAL_ACCEPT_LEVEL_H__

/* coordinate */

//level control items
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_CNT      3
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_X        (COMM_ITEM_OY_IN_ROOT/2)
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_Y        (COMM_ITEM_OY_IN_ROOT/2)
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_LW       400
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_RW       400
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_H        40
#define CONDITIONAL_ACCEPT_LEVEL_LEVEL_CONTROL_ITEM_V_GAP    20

#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_X        140
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_Y        130
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_W        60
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_OK_H        35

//change level result
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_X        50
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_Y        250
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_W       260
#define CONDITIONAL_ACCEPT_LEVEL_CHANGE_RESULT_H        90

//change pin warn
#define CONDITIONAL_ACCEPT_WARN_X        10
#define CONDITIONAL_ACCEPT_WARN_Y        310
#define CONDITIONAL_ACCEPT_WARN_W        310
#define CONDITIONAL_ACCEPT_WARN_H        35


/* rect style */
#define RSI_CONDITIONAL_ACCEPT_LEVEL_FRM       RSI_WINDOW_1
/* font style */

/* others */
#define CONDITIONAL_ACCEPT_WATCH_LEVEL_TOTAL 4

RET_CODE open_conditional_accept_level(u32 para1, u32 para2);

#endif


