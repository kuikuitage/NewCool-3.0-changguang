/****************************************************************************

****************************************************************************/
#ifndef __UI_FREQ_SET_H__
#define __UI_FREQ_SET_H__

/* coordinate */

#ifdef DTMB_PROJECT
#define FREQ_SET_ITEM_CNT      2
#else
#define FREQ_SET_ITEM_CNT      4
#endif

#define FREQ_SET_X			SYS_LEFT_CONT_W
#define FREQ_SET_Y			SYS_LEFT_CONT_Y
#define FREQ_SET_W		SYS_RIGHT_CONT_W
#define FREQ_SET_H			SYS_LEFT_CONT_H

#define FREQ_SET_ITEM_X        (COMM_ITEM_OX_IN_ROOT/2)
#define FREQ_SET_ITEM_Y        (COMM_ITEM_OY_IN_ROOT/2)
#define FREQ_SET_ITEM_LW      (COMM_ITEM_LW)
#define FREQ_SET_ITEM_RW      ((FREQ_SET_W - 2*FREQ_SET_ITEM_X) - FREQ_SET_ITEM_LW)
#define FREQ_SET_ITEM_H        COMM_ITEM_H
#define FREQ_SET_ITEM_V_GAP    10

/* rect style */

/* font style */

RET_CODE open_freq_set(u32 para1, u32 para2);

#endif


