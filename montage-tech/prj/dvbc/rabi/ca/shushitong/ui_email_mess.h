/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_EMAIL_MESS__
#define __UI_EMAIL_MESS__

/* coordinate */
#define EMAIL_MENU_X            SYS_RIGHT_CONT_X 
#define EMAIL_MENU_Y            SYS_RIGHT_CONT_Y
#define EMAIL_MENU_W            SYS_RIGHT_CONT_W 
#define EMAIL_MENU_H            SYS_RIGHT_CONT_H 

//items
#define EMAIL_MBOX_X       (40)
#define EMAIL_MBOX_Y       (5)
#define EMAIL_MBOX_W       (SYS_RIGHT_CONT_W-2*EMAIL_MBOX_X)
#define EMAIL_MBOX_H        COMM_ITEM_H


//plist
#define EMAIL_LIST_X       (30)
#define EMAIL_LIST_Y       (EMAIL_MBOX_Y+EMAIL_MBOX_H)
#define EMAIL_LIST_W       (SYS_RIGHT_CONT_W - 90)
#define EMAIL_LIST_H       (EMAIL_RECEIVED_HEAD_Y - EMAIL_LIST_Y)

//plist rect
#define EMAIL_LIST_MIDL         10
#define EMAIL_LIST_MIDT         10
#define EMAIL_LIST_MIDW        (EMAIL_LIST_W - EMAIL_LIST_MIDL * 3)
#define EMAIL_LIST_MIDH         (EMAIL_LIST_H - EMAIL_LIST_MIDT * 2)
#define EMAIL_LIST_VGAP         10

//list scroll bar
#define EMAIL_SBAR_X       (EMAIL_LIST_X + EMAIL_LIST_W - EMAIL_LIST_VGAP - EMAIL_SBAR_W)
#define EMAIL_SBAR_Y       (EMAIL_LIST_Y + EMAIL_LIST_MIDT)
#define EMAIL_SBAR_W       8
#define EMAIL_SBAR_H       EMAIL_LIST_MIDH

//received head
#define EMAIL_RECEIVED_HEAD_X    (EMAIL_LIST_X + 20)
#define EMAIL_RECEIVED_HEAD_Y    (570)
#define EMAIL_RECEIVED_HEAD_W    (80)
#define EMAIL_RECEIVED_HEAD_H    (30)

//received
#define EMAIL_RECEIVED_X        (EMAIL_RECEIVED_HEAD_X + EMAIL_RECEIVED_HEAD_W)
#define EMAIL_RECEIVED_Y        EMAIL_RECEIVED_HEAD_Y
#define EMAIL_RECEIVED_W        100
#define EMAIL_RECEIVED_H        EMAIL_RECEIVED_HEAD_H

//reset head
#define EMAIL_RESET_HEAD_X      (EMAIL_RECEIVED_X + EMAIL_RECEIVED_W)
#define EMAIL_RESET_HEAD_Y      EMAIL_RECEIVED_HEAD_Y
#define EMAIL_RESET_HEAD_W      120
#define EMAIL_RESET_HEAD_H      EMAIL_RECEIVED_HEAD_H

//reset
#define EMAIL_RESET_X          (EMAIL_RESET_HEAD_X+EMAIL_RESET_HEAD_W)
#define EMAIL_RESET_Y          EMAIL_RECEIVED_HEAD_Y
#define EMAIL_RESET_W          100
#define EMAIL_RESET_H          EMAIL_RECEIVED_HEAD_H
#define EMAIL_LIST_PAGE         11
#define EMAIL_ITEM_OFFSET     5

//del one
#define EMAIL_DEL_ICON_X      (EMAIL_RESET_X + EMAIL_RESET_W + 30)
#define EMAIL_DEL_ICON_Y      (EMAIL_RECEIVED_HEAD_Y)
#define EMAIL_DEL_ICON_W      (50)
#define EMAIL_DEL_ICON_H      (EMAIL_RECEIVED_HEAD_H)

#define EMAIL_DEL_TEXT_X      (EMAIL_DEL_ICON_X + EMAIL_DEL_ICON_W + 20)
#define EMAIL_DEL_TEXT_Y      (EMAIL_RECEIVED_HEAD_Y)
#define EMAIL_DEL_TEXT_W      (100)
#define EMAIL_DEL_TEXT_H      (EMAIL_RECEIVED_HEAD_H)

//del all
#define EMAIL_DEL_ALL_ICON_X      (EMAIL_DEL_TEXT_X + EMAIL_DEL_TEXT_W + 40)
#define EMAIL_DEL_ALL_ICON_Y      (EMAIL_RECEIVED_HEAD_Y)
#define EMAIL_DEL_ALL_ICON_W      (50)
#define EMAIL_DEL_ALL_ICON_H      (EMAIL_RECEIVED_HEAD_H)

#define EMAIL_DEL_ALL_TEXT_X      (EMAIL_DEL_ALL_ICON_X + EMAIL_DEL_ALL_ICON_W + 20)
#define EMAIL_DEL_ALL_TEXT_Y      (EMAIL_RECEIVED_HEAD_Y)
#define EMAIL_DEL_ALL_TEXT_W      (200)
#define EMAIL_DEL_ALL_TEXT_H      (EMAIL_RECEIVED_HEAD_H)


//others

#define EMAIL_LIST_FIELD        4

#define EMAIL_ITME_COUNT      4

#define EMAIL_MAX_NUMB         10

RET_CODE open_email_mess(u32 para1, u32 para2);
RET_CODE asc_to_dec(const u8 *p_ascstr, u32 *p_dec_number, u32 max_len);

#endif




