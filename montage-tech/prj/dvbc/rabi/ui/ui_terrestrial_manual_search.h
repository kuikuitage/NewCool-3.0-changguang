/****************************************************************************

 ****************************************************************************/
#ifndef __UI_TERRESTRIAL_MANUAL_SEARCH_H__
#define __UI_TERRESTRIAL_MANUAL_SEARCH_H__

/* coordinate */

#ifdef SEARCH_SUPPORT_FILTER
#define _MANUAL_SEARCH_ITEM_CNT		3
#else
#define _MANUAL_SEARCH_ITEM_CNT		2
#endif
#define _MANUAL_SEARCH_ITEM_X			60
#define _MANUAL_SEARCH_ITEM_Y			60
#define _MANUAL_SEARCH_ITEM_LW		  ((RIGHT_ROOT_W-2*_MANUAL_SEARCH_ITEM_X)/3)
#define _MANUAL_SEARCH_ITEM_RW		  ((RIGHT_ROOT_W-2*_MANUAL_SEARCH_ITEM_X)/3*2)
#define _MANUAL_SEARCH_ITEM_H			COMM_ITEM_H
#define _MANUAL_SEARCH_ITEM_V_GAP	16

#define _RSI_MANUAL_SEARCH_BTN_SH    RSI_PBACK
#define _RSI_MANUAL_SEARCH_BTN_HL    RSI_SELECT_F
/* rect style */
#define _RSI_LANGUAGE_FRM		RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_terrestrial_manual_search(u32 para1, u32 para2);

#endif

