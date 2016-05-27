/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_PRE_AUTH_H__
#define __UI_CA_PRE_AUTH_H__
#include "ui_ca_ppv_icon.h"
/* coordinate */
#define PRE_AUTH_CONT_X     (PPV_ICON_CONT_X)
#define PRE_AUTH_CONT_Y     (PPV_ICON_CONT_Y + PPV_ICON_CONT_H + 10)
#define PRE_AUTH_CONT_W     (PPV_ICON_CONT_W)
#define PRE_AUTH_CONT_H     (PPV_ICON_CONT_H)

#define PRE_AUTH_TXT_X     	(2)
#define PRE_AUTH_TXT_Y     	(0)
#define PRE_AUTH_TXT_W     	(PRE_AUTH_CONT_W - PRE_AUTH_TXT_X * 2)
#define PRE_AUTH_TXT_H     	(PRE_AUTH_CONT_H)

/* rect style */
#define RSI_PRE_AUTH_TXT   RSI_POPUP_BG

/* font style */
#define FSI_PRE_AUTH_TXT    FSI_WHITE

#define PRE_AUTH_AUTOCLOSE_MS 3000


RET_CODE open_ca_pre_auth(u32 para1, u32 para2);

void close_ca_pre_auth(void);

#endif


