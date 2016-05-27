/****************************************************************************

 ****************************************************************************/
#ifndef __UI_TERRESTRIAL_FULL_SEARCH_H__
#define __UI_TERRESTRIAL_FULL_SEARCH_H__

/* coordinate */

#ifdef SEARCH_SUPPORT_FILTER
#define FULL_SEARCH_ITEM_CNT		(4)
#else
#define FULL_SEARCH_ITEM_CNT		3
#endif
#define FULL_SEARCH_ITEM_X			60
#define FULL_SEARCH_ITEM_Y			60
#define FULL_SEARCH_ITEM_LW		((RIGHT_ROOT_W-2*FULL_SEARCH_ITEM_X)/3)
#define FULL_SEARCH_ITEM_RW		((RIGHT_ROOT_W-2*FULL_SEARCH_ITEM_X)/3*2)
#define FULL_SEARCH_ITEM_H			COMM_ITEM_H
#define FULL_SEARCH_ITEM_V_GAP		16

#define RSI_FULL_SEARCH_BTN_SH    RSI_PBACK
#define RSI_FULL_SEARCH_BTN_HL    RSI_SELECT_F
/* rect style */
#define RSI_LANGUAGE_FRM		RSI_WINDOW_1
/* font style */

/* others */
RET_CODE open_terrestrial_full_search(u32 para1, u32 para2);

#endif


#if 0
#define __UI_FULL_SEARCH_H__

/* coordinate */
#define FULL_SEARCH_CONT_W        480
#define FULL_SEARCH_CONT_H        350
#define FULL_SEARCH_CONT_X        ((SCREEN_WIDTH - FULL_SEARCH_CONT_W) / 2)
#define FULL_SEARCH_CONT_Y        ((SCREEN_HEIGHT- FULL_SEARCH_CONT_H) / 2)

#define FULL_SCAN_ITEM_CNT      2
#define FULL_SCAN_ITEM_X        20
#define FULL_SCAN_ITEM_Y        120
#define FULL_SCAN_ITEM_LW       220
#define FULL_SCAN_ITEM_RW       220
#define FULL_SCAN_ITEM_H        COMM_ITEM_H
#define FULL_SCAN_ITEM_V_GAP    20

#define FULL_SCAN_FREQ_MIN    474000
#define FULL_SCAN_FREQ_MAX    864000
#define FULL_SCAN_FREQ_BIT  6



RET_CODE open_full_search(u32 para1, u32 para2);

#endif


