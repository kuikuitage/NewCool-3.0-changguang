/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_PPV_H__
#define __UI_CA_PPV_H__

/* coordinate */
#define PPV_INFO_CONT_X        	((SCREEN_WIDTH - PPV_INFO_CONT_W) / 2)
#define PPV_INFO_CONT_Y        	((SCREEN_HEIGHT - PPV_INFO_CONT_H) / 2)
#define PPV_INFO_CONT_W        	(480)
#define PPV_INFO_CONT_H        	(220)

#define PPV_CONT_TITLE_X      	(0)
#define PPV_CONT_TITLE_Y      	(0)
#define PPV_CONT_TITLE_W      	(PPV_INFO_CONT_W - 2 * PPV_CONT_TITLE_X)
#define PPV_CONT_TITLE_H      	(220)

#define PPV_TEXT_TITLE_X      	(100)
#define PPV_TEXT_TITLE_Y      	(5)
#define PPV_TEXT_TITLE_W      	(PPV_CONT_TITLE_W - 2 * PPV_TEXT_TITLE_X)
#define PPV_TEXT_TITLE_H      	(35)


//PPV info items
#define PPV_ACCEPT_CA_INFO_ITEM_CNT      	(3)
#define PPV_ACCEPT_CA_INFO_ITEM_X        	(67)
#define PPV_ACCEPT_CA_INFO_ITEM_Y        	(PPV_TEXT_TITLE_H + PPV_TEXT_TITLE_Y)
#define PPV_ACCEPT_CA_INFO_ITEM_LW       	(150)
#define PPV_ACCEPT_CA_INFO_ITEM_RW       	(200)

#define PPV_ACCEPT_CA_INFO_ITEM_H        	(35)
#define PPV_ACCEPT_CA_INFO_ITEM_V_GAP    	(5)

#define PPV_TEXT_BIT_X      ((PPV_CONT_TITLE_W - PPV_SET_BTN_CNT*PPV_TEXT_BIT_W)/(PPV_SET_BTN_CNT + 1))
#define PPV_TEXT_BIT_Y      (PPV_CONT_TITLE_H - PPV_TEXT_BIT_H - 10)
#define PPV_TEXT_BIT_W      (100)
#define PPV_TEXT_BIT_H      (40)

#define PPV_INFO_ITEM_CNT    5
#define PPV_SET_BTN_CNT      2


/* rect style */
/* font style */
#define FSI_CA_PPV_SH        	FSI_BLACK
#define FSI_CA_PPV_N     	    FSI_WHITE
#define FSI_CA_PPV_HL        	FSI_WHITE
RET_CODE open_ca_ippv_ppt_dlg(u32 para1, u32 para2);
void ui_ppv_dlg_close(void);
#endif


