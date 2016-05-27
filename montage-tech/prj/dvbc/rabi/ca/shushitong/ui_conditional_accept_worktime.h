/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_WORKTIME_H__
#define __UI_CONDITIONAL_ACCEPT_WORKTIME_H__

/* coordinate */
#define CONDITIONAL_ACCEPT_WORKTIME_SCREEN_X		402
#define CONDITIONAL_ACCEPT_WORKTIME_SCREEN_Y		97
#define CONDITIONAL_ACCEPT_WORKTIME_SCREEN_W		808
#define CONDITIONAL_ACCEPT_WORKTIME_SCREEN_H		503

//work duration items
#define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_CNT    4
#define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_X        (COMM_ITEM_OY_IN_ROOT/2)
#define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_Y        (COMM_ITEM_OY_IN_ROOT/2)
#define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_LW       400
#define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_RW       400
#define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_H        40
#define CONDITIONAL_ACCEPT_WORKTIME_WORK_ITEM_V_GAP    20

#define CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_X        120
#define CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_Y        180
#define CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_W        60
#define CONDITIONAL_ACCEPT_WORKTIME_OK_ITEM_H        35

//change work time result
#define CONDITIONAL_ACCEPT_WORKTIME_CHANGE_RESULT_X        10
#define CONDITIONAL_ACCEPT_WORKTIME_CHANGE_RESULT_Y        300
#define CONDITIONAL_ACCEPT_WORKTIME_CHANGE_RESULT_W        260
#define CONDITIONAL_ACCEPT_WORKTIME_CHANGE_RESULT_H        35

/* rect style */
#define RSI_CONDITIONAL_ACCEPT_WORKTIME_FRM       RSI_WINDOW_1
/* font style */

/* others */

//RET_CODE open_conditional_accept_worktime(u32 para1, u32 para2);

#endif


