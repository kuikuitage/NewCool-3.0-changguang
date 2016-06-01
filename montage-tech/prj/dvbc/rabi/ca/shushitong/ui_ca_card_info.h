/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_CARD_INFO_H__
#define __UI_CA_CARD_INFO_H__

#define CA_CARD_INFO_CNT 8

#define ACCEPT_INFO_CA_INFO_ITEM_X_DIVI        (COMM_ITEM_OY_IN_ROOT/2)
#define ACCEPT_INFO_CA_INFO_ITEM_Y_DIVI        (COMM_ITEM_OY_IN_ROOT/2)

#define ACCEPT_INFO_CA_INFO_ITEM_LW_DIVI       (COMM_ITEM_LW)
#define ACCEPT_INFO_CA_INFO_ITEM_RW_DIVI       (RIGHT_ROOT_W - COMM_ITEM_LW - COMM_ITEM_OX_IN_ROOT)
//#define ACCEPT_INFO_CA_INFO_ITEM_H         COMM_CTRL_H


#define ACCEPT_CA_INFO_ITEM_V_GAP_DIVI    0


//MG
#define ACCEPT_INFO_CARD_INFO_ITEM_CNT      10

#define ACCEPT_INFO_CA_INFO_ITEM_X_MG        	(COMM_ITEM_OX_IN_ROOT/2)
#define ACCEPT_INFO_CA_INFO_ITEM_Y_MG        	10
#define ACCEPT_INFO_CA_INFO_ITEM_LW_MG       	300
#define ACCEPT_INFO_CA_INFO_ITEM_RW_MG       	500
#define ACCEPT_CA_INFO_ITEM_V_GAP    		5

#define ACCEPT_INFO_CA_CODE_ITEM_X        	(ACCEPT_INFO_CA_INFO_ITEM_X_MG + ACCEPT_INFO_CA_INFO_ITEM_LW_MG)
#define ACCEPT_INFO_CA_CODE_ITEM_W       	450
#define ACCEPT_INFO_CA_CODE_ITEM_H       	80


//MG
RET_CODE open_ca_card_info(u32 para1, u32 para2);

#endif
