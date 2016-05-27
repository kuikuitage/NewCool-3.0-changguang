/****************************************************************************

****************************************************************************/
#ifndef __UI_SLEEP_TIMER_H__
#define __UI_SLEEP_TIMER_H__

/* coordinate */
#define SLEEP_TIMER_CONT_H        50
#define SLEEP_TIMER_CONT_W        300

#define SLEEP_TIMER_ITEM_X        0
#define SLEEP_TIMER_ITEM_Y        0
#define SLEEP_TIMER_ITEM_LW       (SLEEP_TIMER_CONT_W/2)
#define SLEEP_TIMER_ITEM_RW       (SLEEP_TIMER_CONT_W/2)
#define SLEEP_TIMER_ITEM_H        COMM_ITEM_H
#define SLEEP_TIMER_ITEM_V_GAP    0

/* rect style */
#define RSI_SLEEP_TIMER_FRM         RSI_LEFT_CONT_BG

/* font style */

/* others */

RET_CODE open_sleep_timer(u32 para1, u32 para2);

#endif

