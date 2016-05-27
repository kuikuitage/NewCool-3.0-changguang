/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_PPV_ICON_H__
#define __UI_CA_PPV_ICON_H__
#include "ui_finger_print.h"

/* coordinate */
#define PPV_ICON_CONT_X     (980)
#define PPV_ICON_CONT_Y     (FINGER_PRINT_Y + FINGER_PRINT_H)
#define PPV_ICON_CONT_W     (150)
#define PPV_ICON_CONT_H     (40)

#define PPV_ICON_TXT_X    	(2)
#define PPV_ICON_TXT_Y     	(0)
#define PPV_ICON_TXT_W     	(PPV_ICON_CONT_W - PPV_ICON_TXT_X * 2)
#define PPV_ICON_TXT_H     	(PPV_ICON_CONT_H)

/* rect style */
#define RSI_PPV_ICON_TXT   RSI_POPUP_BG

/* font style */
#define FSI_PPV_ICON_TXT    FSI_WHITE

#define PPV_ICON_AUTOCLOSE_MS 10000

RET_CODE open_ca_ppv_icon(u32 para1, u32 para2);

void close_ca_ppv_icon(void);

#endif


