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
#define EMAIL_TITLE_Y   	(5)
#define EMAIL_TITLE_H   	(COMM_ITEM_H)

#define EMAIL_TITLE1_W   	(140)//(100)
#define EMAIL_TITLE2_W   	(140)//(100)
#define EMAIL_TITLE3_W   	(275)//(210)
#define EMAIL_TITLE4_W   	(275)//(210)
#define EMAIL_TITLE5_W   	(210)

#define EMAIL_TITLE1_X   	(30)
#define EMAIL_TITLE2_X   	(EMAIL_TITLE1_X + EMAIL_TITLE1_W)
#define EMAIL_TITLE3_X   	(EMAIL_TITLE2_X + EMAIL_TITLE2_W)
#define EMAIL_TITLE4_X   	(EMAIL_TITLE3_X + EMAIL_TITLE3_W)
#define EMAIL_TITLE5_X   	(EMAIL_TITLE4_X + EMAIL_TITLE4_W)


//plist
#define EMAIL_LIST_X       (0)
#define EMAIL_LIST_Y       (EMAIL_TITLE_Y+EMAIL_TITLE_H)
#define EMAIL_LIST_W       (SYS_RIGHT_CONT_W - 80)
#define EMAIL_LIST_H       (EMAIL_RECEIVED_HEAD_Y - EMAIL_LIST_Y)

//plist rect
#define EMAIL_LIST_VGAP         10

//list scroll bar
#define EMAIL_SBAR_X       (EMAIL_LIST_X + EMAIL_LIST_W + EMAIL_LIST_VGAP)
#define EMAIL_SBAR_Y       (EMAIL_LIST_Y)
#define EMAIL_SBAR_W       (8)
#define EMAIL_SBAR_H       (EMAIL_LIST_H)

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

#define EMAIL_LIST_FIELD        (4)//(5)

#define EMAIL_ITME_COUNT      	(4)//(5)

#define EMAIL_MAX_NUMB         	(50)

RET_CODE open_email_mess(u32 para1, u32 para2);

#endif




