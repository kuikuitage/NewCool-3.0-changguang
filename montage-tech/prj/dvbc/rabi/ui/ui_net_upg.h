/****************************************************************************

 ****************************************************************************/
#ifndef _UI_NET_UPG_H_
#define _UI_NET_UPG_H_

#include "ui_common.h"

/*! coordinate */
#define XUPGRADE_BY_NET_X			SYS_LEFT_CONT_W
#define XUPGRADE_BY_NET_Y			SYS_LEFT_CONT_Y
#define XUPGRADE_BY_NET_W				SYS_RIGHT_CONT_W
#define XUPGRADE_BY_NET_H				SYS_LEFT_CONT_H
#define UI_XUPGRADE_BY_NET_SET_X        60
#define UI_XUPGRADE_BY_NET_SET_W        (XUPGRADE_BY_NET_W-2*UI_XUPGRADE_BY_NET_SET_X)
#define UI_XUPGRADE_BY_NET_SET_W_L		(UI_XUPGRADE_BY_NET_SET_W/3)
#define UI_XUPGRADE_BY_NET_SET_W_R		(UI_XUPGRADE_BY_NET_SET_W/3*2)

#define NET_UPG_ITEM_CNT      6
#define NET_UPG_ITEM_X        UI_XUPGRADE_BY_NET_SET_X
#define NET_UPG_ITEM_Y        XUPGRADE_BY_NET_Y
#define NET_UPG_ITEM_LW       UI_XUPGRADE_BY_NET_SET_W_L
#define NET_UPG_ITEM_RW       UI_XUPGRADE_BY_NET_SET_W_R
#define NET_UPG_ITEM_H        50
#define NET_UPG_ITEM_V_GAP    16

#define NET_UPG_TXT_X	NET_UPG_ITEM_X
#define NET_UPG_TXT_W	0//60
#define NET_UPG_TXT_H	30

#define NET_UPG_PBAR_X  (NET_UPG_TXT_X + NET_UPG_TXT_W)
#define NET_UPG_PBAR_W (UI_XUPGRADE_BY_NET_SET_W - NET_UPG_PER_W)
#define NET_UPG_PBAR_H  16

#define NET_UPG_PER_X (NET_UPG_PBAR_X + NET_UPG_PBAR_W)
#define NET_UPG_PER_W 60
#define NET_UPG_PER_H 30

//misc file list
#define NET_UPG_MISC_LIST_X    140
#define NET_UPG_MISC_LIST_Y    180
#define NET_UPG_MISC_LIST_W    300
#define NET_UPG_MISC_LIST_H    260

#define NET_UPG_MISC_LIST_MIDL  0
#define NET_UPG_MISC_LIST_MIDT  0
#define NET_UPG_MISC_LIST_MIDW  (NET_UPG_MISC_LIST_W - 2 * NET_UPG_MISC_LIST_MIDL)
#define NET_UPG_MISC_LIST_MIDH  (NET_UPG_MISC_LIST_H - 2 * NET_UPG_MISC_LIST_MIDT)

#define NET_UPG_MISC_LIST_VGAP  0

#define NET_UPG_MISC_LIST_PAGE     7
#define NET_UPG_MISC_LIST_FIELD    1

//file files list
#define NET_UPG_IMG_LIST_X     NET_UPG_MISC_LIST_X
#define NET_UPG_IMG_LIST_Y     NET_UPG_MISC_LIST_Y
#define NET_UPG_IMG_LIST_W     NET_UPG_MISC_LIST_W
#define NET_UPG_IMG_LIST_H     NET_UPG_MISC_LIST_H

#define NET_UPG_IMG_LIST_MIDL  0
#define NET_UPG_IMG_LIST_MIDT  0
#define NET_UPG_IMG_LIST_MIDW  (NET_UPG_IMG_LIST_W - 2 * NET_UPG_IMG_LIST_MIDL)
#define NET_UPG_IMG_LIST_MIDH  (NET_UPG_IMG_LIST_H - 2 * NET_UPG_IMG_LIST_MIDT)

#define NET_UPG_IMG_LIST_VGAP  0

#define NET_UPG_IMG_LIST_PAGE     7
#define NET_UPG_IMG_LIST_FIELD    1


#define NET_UPG_MODE_CNT 3

#define NET_UPG_MAX_MISC_FILES_CNT 7
#define NET_UPG_MAX_IMG_FILES_CNT 7


#define NET_UPG_STS_ITEM_CNT 1

/*! rect style */
#define RSI_NET_UPG_PBAR_BG     RSI_PROGRESS_BAR_BG
#define RSI_NET_UPG_PBAR_MID    RSI_PROGRESS_BAR_MID_YELLOW


#define STS_STR_DUMP_MAX_LEN (32+1)

#define MAX_URL_LENGTH (255)

/*! font style */

/*! others*/
typedef enum
{
    UI_NET_UPG_IDEL = 0,
    UI_NET_UPG_PRE_READ,
    UI_NET_UPG_DOWNLOADING,
    UI_NET_UPG_DOWNLOAD_END,
    UI_NET_UPG_WRITING    
}ui_net_upg_status_t;

RET_CODE open_net_upg(u32 para1, u32 para2);


#endif
