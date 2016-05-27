/****************************************************************************

****************************************************************************/
#ifndef __UI_OTA_H__
#define __UI_OTA_H__

/* coordinate */
#ifdef DTMB_PROJECT
#define OTA_ITEM_CNT        3
#else
#define OTA_ITEM_CNT        5
#endif
#define OTA_ITEM_X            COMM_ITEM_OX_IN_ROOT
#define OTA_ITEM_Y            COMM_ITEM_OY_IN_ROOT
#define OTA_ITEM_LW         COMM_ITEM_LW
#define OTA_ITEM_RW         (COMM_ITEM_MAX_WIDTH - OTA_ITEM_LW)
#define OTA_ITEM_H            COMM_ITEM_H
#define OTA_ITEM_V_GAP    10

/* rect style */

/* font style */

/* others */

#define OTA_PID_MIN 0
#define OTA_PID_MAX 17777//0x1FFF
#define OTA_PID_BIT 4

RET_CODE open_ota(u32 para1, u32 para2);

#endif
