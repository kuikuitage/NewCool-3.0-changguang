/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_EMAIL_MESS_MG__
#define __UI_EMAIL_MESS_MG__

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

#define EMAIL_MAX_NUMB         	(50)

extern RET_CODE asc_to_dec(const u8 *p_ascstr, u32 *p_dec_number, u32 max_len);

RET_CODE open_email_mess_MG(u32 para1, u32 para2);

#endif




