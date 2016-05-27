/****************************************************************************

****************************************************************************/
#ifndef __UI_HDD_INFO_H__
#define __UI_HDD_INFO_H__

/* coordinate */

#define HDD_INFO_ITEM_CNT      7
#define HDD_INFO_ITEM_X        60
#define HDD_INFO_ITEM_Y        60
#define HDD_INFO_ITEM_LW       ((SYS_RIGHT_CONT_W-2*HDD_INFO_ITEM_X)/3)
#define HDD_INFO_ITEM_RW       ((SYS_RIGHT_CONT_W-2*HDD_INFO_ITEM_X)/3*2)
#define HDD_INFO_ITEM_H        COMM_ITEM_H
#define HDD_INFO_ITEM_V_GAP    16

#define HDD_INFO_HELP_X        COMM_ITEM_OX_IN_ROOT
#define HDD_INFO_HELP_W        (HDD_INFO_ITEM_LW + HDD_INFO_ITEM_RW)
#define HDD_INFO_HELP_H        COMM_ITEM_H

/* rect style */
#define RSI_HDD_INFO_FRM       RSI_WINDOW_1

/* others */

RET_CODE open_hdd_info(u32 para1, u32 para2);

#endif
