/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_CARD_INFO_H__
#define __UI_CA_CARD_INFO_H__

//CA info items
#define ACCEPT_INFO_CARD_INFO_ITEM_CNT      10

#define ACCEPT_INFO_CA_INFO_ITEM_X        	(COMM_ITEM_OX_IN_ROOT/2)
#define ACCEPT_INFO_CA_INFO_ITEM_Y        	10
#define ACCEPT_INFO_CA_INFO_ITEM_LW       	300
#define ACCEPT_INFO_CA_INFO_ITEM_RW       	500
#define ACCEPT_INFO_CA_INFO_ITEM_H        	COMM_CTRL_H

#define ACCEPT_INFO_CA_CODE_ITEM_X        	(ACCEPT_INFO_CA_INFO_ITEM_X + ACCEPT_INFO_CA_INFO_ITEM_LW)
#define ACCEPT_INFO_CA_CODE_ITEM_W       	450
#define ACCEPT_INFO_CA_CODE_ITEM_H       	80

#define ACCEPT_CA_INFO_ITEM_V_GAP    		5


RET_CODE open_ca_card_info(u32 para1, u32 para2);

#endif


