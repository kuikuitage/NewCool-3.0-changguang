/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_ENTITLE_INFO_H__
#define __UI_CA_ENTITLE_INFO_H__

//mbox
#define CA_MBOX_X           (40)
#define CA_MBOX_Y           (5)
#define CA_MBOX_W           (SYS_RIGHT_CONT_W-2*CA_MBOX_X)
#define CA_MBOX_H           (COMM_ITEM_H)
#define CA_MBOX_MID_X   	(20)
#define CA_MBOX_MID_W  		(CA_MBOX_W)
#define CA_MBOX_MID_H   	(CA_MBOX_H)


//plist
#define CA_PLIST_BG_X  		(CA_MBOX_X)
#define CA_PLIST_BG_Y  		(CA_MBOX_Y + CA_MBOX_H + 5)
#define CA_PLIST_BG_W  		(CA_MBOX_W)
#define CA_PLIST_BG_H  		(SYS_RIGHT_CONT_H - CA_PLIST_BG_Y - COMM_ITEM_OY_IN_ROOT/2)


//plist rect
#define CA_LIST_BG_MIDL         4
#define CA_LIST_BG_MIDT         10
#define CA_LIST_BG_MIDW        (CA_PLIST_BG_W-20)
#define CA_LIST_BG_MIDH        (CA_PLIST_BG_H-20)
#define CA_LIST_BG_VGAP         2
//other
#define CA_INFO_LIST_PAGE      (10)
#define CA_INFO_ENTITLE_FIELD  4

#define CA_ITME_COUNT           4



/* rect style */
RET_CODE open_ca_entitle_info(u32 para1, u32 para2);

#endif
