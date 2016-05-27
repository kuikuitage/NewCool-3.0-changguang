/****************************************************************************

****************************************************************************/
#ifndef __UI_XTVSYS_SET_H__
#define __UI_XTVSYS_SET_H__

/* coordinate */
#ifdef TV_BIRTHNESS_EN
#define BIRTHNESS_ITEM_CNT		(1)
#define IDS_XTVSYS_BIRTHNESS	(IDS_HD_BIRGHTNESS)
#else
#define BIRTHNESS_ITEM_CNT		(0)
#define IDS_XTVSYS_BIRTHNESS	(RSC_INVALID_ID)
#endif
#define ITEM_ECTERN_CNT			(1)

#if ENABLE_TTX_SUBTITLE
#define XOSD_SET_ITEM_CNT     (3 + BIRTHNESS_ITEM_CNT)
#else
#define XOSD_SET_ITEM_CNT     (2 + BIRTHNESS_ITEM_CNT)
#endif

#define XTVSYS_SET_X			SYS_LEFT_CONT_W
#define XTVSYS_SET_Y			SYS_LEFT_CONT_Y
#define XTVSYS_SET_W				SYS_RIGHT_CONT_W
#define XTVSYS_SET_H				SYS_LEFT_CONT_H
#define UI_XTVSYS_SET_SET_X        60
#define UI_XTVSYS_SET_SET_W        (XTVSYS_SET_W-2*UI_XTVSYS_SET_SET_X)
#define UI_XTVSYS_SET_SET_W_L	(UI_XTVSYS_SET_SET_W/3)
#define UI_XTVSYS_SET_SET_W_R	(UI_XTVSYS_SET_SET_W/3*2)

#define XTVSYS_SET_ITEM_CNT      5
#define XTVSYS_SET_ITEM_X        UI_XTVSYS_SET_SET_X
#define XTVSYS_SET_ITEM_Y        XTVSYS_SET_Y
#define XTVSYS_SET_ITEM_LW       UI_XTVSYS_SET_SET_W_L
#define XTVSYS_SET_ITEM_RW       UI_XTVSYS_SET_SET_W_R
#define XTVSYS_SET_ITEM_H        COMM_ITEM_H
#define XTVSYS_SET_ITEM_V_GAP    10

/* rect style */
#define RSI_XTVSYS_SET_FRM       RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_xtvsys_set(u32 para1, u32 para2);

#endif


