/****************************************************************************

 ****************************************************************************/
#ifndef __UI_TERRESTRIAL_H__
#define __UI_TERRESTRIAL_H__

/* coordinate */

#define TERRESTRIAL_ITEM_CNT		3
#define TERRESTRIAL_ITEM_X			60
#define TERRESTRIAL_ITEM_Y			60
#define TERRESTRIAL_ITEM_LW		((RIGHT_ROOT_W-2*TERRESTRIAL_ITEM_X)/3)
#define TERRESTRIAL_ITEM_RW		((RIGHT_ROOT_W-2*TERRESTRIAL_ITEM_X)/3*2)
#define TERRESTRIAL_ITEM_H			COMM_ITEM_H
#define TERRESTRIAL_ITEM_V_GAP		16

#define TERRESTRIAL_MAX_CNT    16
#define ANTENNA_POWER_CNT          2
#define TERRESTRIAL_LCN_CNT     2
#define RSI_TERRESTRIAL_BTN_SH    RSI_PBACK
#define RSI_TERRESTRIAL_BTN_HL    RSI_SELECT_F
/* rect style */
#define RSI_LANGUAGE_FRM		RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_terrestrial (u32 para1, u32 para2);

#endif

