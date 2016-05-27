/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_BURSE_CHARGE_H__
#define __UI_CA_BURSE_CHARGE_H__

#define BURSE_CONT_FULL_X  ((SCREEN_WIDTH-BURSE_CONT_FULL_W)/2)
#define BURSE_CONT_FULL_Y  ((SCREEN_HEIGHT-BURSE_CONT_FULL_H)/2 -20)
#define BURSE_CONT_FULL_W  (420)
#define BURSE_CONT_FULL_H  (140)

#define BURSE_TITLE_FULL_X  0
#define BURSE_TITLE_FULL_Y  0
#define BURSE_TITLE_FULL_W  BURSE_CONT_FULL_W
#define BURSE_TITLE_FULL_H  40

#define BURSE_CONTENT_FULL_X  10
#define BURSE_CONTENT_FULL_Y  (BURSE_TITLE_FULL_Y+BURSE_TITLE_FULL_H+5)
#define BURSE_CONTENT_FULL_W  (BURSE_CONT_FULL_W - 2*BURSE_CONTENT_FULL_X)
#define BURSE_CONTENT_FULL_H  50

#define BURSE_TIME_OUT 6000
RET_CODE open_ca_burse_dlg(u32 para1, u32 para2);
void close_ca_burse_dlg(void);

#endif


