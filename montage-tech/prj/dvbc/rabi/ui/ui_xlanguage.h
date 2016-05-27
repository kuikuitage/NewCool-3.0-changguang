/****************************************************************************

 ****************************************************************************/
#ifndef __UI_XLANGUAGE_H__
#define __UI_XLANGUAGE_H__

/* coordinate */
#define LANGUAGE_CONT_X			((SCREEN_WIDTH-LANGUAGE_CONT_W)/2)
#define LANGUAGE_CONT_H			440

#if ENABLE_TTX_SUBTITLE
#define LANGUAGE_ITEM_CNT		8//5
#else
#define LANGUAGE_ITEM_CNT		6
#endif

#define LANGUAGE_ITEM_X			COMM_ITEM_OX_IN_ROOT
#define LANGUAGE_ITEM_Y			COMM_ITEM_OY_IN_ROOT
#define LANGUAGE_ITEM_LW		290//COMM_ITEM_LW
#define LANGUAGE_ITEM_RW		(COMM_ITEM_MAX_WIDTH - LANGUAGE_ITEM_LW)
#define LANGUAGE_ITEM_H			COMM_ITEM_H
#define LANGUAGE_ITEM_V_GAP		8

#define LANGUAGE_X			SYS_LEFT_CONT_W
#define LANGUAGE_Y			SYS_LEFT_CONT_Y
#define LANGUAGE_W				SYS_RIGHT_CONT_W
#define LANGUAGE_H				SYS_LEFT_CONT_H
#define UI_LANGUAGE_SET_X        60
#define UI_LANGUAGE_SET_W        (LANGUAGE_W-2*UI_LANGUAGE_SET_X)
#define UI_LANGUAGE_SET_W_L	(UI_LANGUAGE_SET_W/3)
#define UI_LANGUAGE_SET_W_R	(UI_LANGUAGE_SET_W/3*2)

#ifdef LANGUAGE_SIMPLIFIED_CHINESE
#define LANGUAGE_MAX_CNT    12
#else
#define LANGUAGE_MAX_CNT    11
#endif
#define RSI_LANGUAGE_FRM		RSI_WINDOW_1

RET_CODE open_xlanguage_set(u32 para1, u32 para2);
#endif

