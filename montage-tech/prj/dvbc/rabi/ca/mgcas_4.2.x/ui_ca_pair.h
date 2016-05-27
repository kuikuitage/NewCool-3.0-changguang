/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_PAIR_H__
#define __UI_CA_PAIR_H__

#define PAIR_CONT_FULL_X  	((SCREEN_WIDTH-PAIR_CONT_FULL_W)/2)
#define PAIR_CONT_FULL_Y  	((SCREEN_HEIGHT-PAIR_CONT_FULL_H)/2 -20)
#define PAIR_CONT_FULL_W  	(420)
#define PAIR_CONT_FULL_H  	(140)

#define PAIR_TITLE_FULL_X  0
#define PAIR_TITLE_FULL_Y  0
#define PAIR_TITLE_FULL_W  PAIR_CONT_FULL_W
#define PAIR_TITLE_FULL_H  40

#define PAIR_CONTENT_FULL_X  	(10)
#define PAIR_CONTENT_FULL_Y  	(PAIR_TITLE_FULL_Y+PAIR_TITLE_FULL_H+5)
#define PAIR_CONTENT_FULL_W  	(PAIR_CONT_FULL_W - 2*PAIR_CONTENT_FULL_X)
#define PAIR_CONTENT_FULL_H  	(PAIR_CONT_FULL_H - PAIR_CONTENT_FULL_Y)


#define PAIR_ERROR_AUTOCLOSE_MS 20000
RET_CODE open_ca_pair_dlg(u32 para1, u32 para2);
void close_ca_pair_dlg(void);

#endif


