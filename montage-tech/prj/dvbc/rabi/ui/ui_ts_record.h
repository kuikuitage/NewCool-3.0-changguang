/****************************************************************************

****************************************************************************/
#ifndef __UI_TS_RECORD_TOOL_H__
#define __UI_TS_RECORD_TOOL_H__

/* coordinate */
#ifdef DTMB_PROJECT
#define TS_RECORD_ITEM_CNT      2
#else
#define TS_RECORD_ITEM_CNT      4
#endif

#define TS_RECORD_ITEM_X        (COMM_ITEM_OX_IN_ROOT/2)
#define TS_RECORD_ITEM_Y        (COMM_ITEM_OY_IN_ROOT/2)
#define TS_RECORD_ITEM_LW       (400)
#define TS_RECORD_ITEM_RW       (400)
#define TS_RECORD_ITEM_H        (COMM_ITEM_H)
#define TS_RECORD_ITEM_V_GAP    (10)

/* rect style */

/* font style */


RET_CODE open_ts_record_tool(u32 para1, u32 para2);

#endif
