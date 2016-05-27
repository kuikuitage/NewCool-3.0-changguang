/****************************************************************************

****************************************************************************/
#ifndef __UI_PLAY_HIST_H__
#define __UI_PLAY_HIST_H__

/* coordinate */
//title cont
#define PLAYHIST_TITLE_CONTX        (90)//(COMM_ITEM_OX_IN_ROOT)
#define PLAYHIST_TITLE_CONTY        (90)
#define PLAYHIST_TITLE_CONTW        (COMM_BG_W - 2 * PLAYHIST_TITLE_CONTX)
#define PLAYHIST_TITLE_CONTH        (TITLE_H)
//icon
#define PLAYHIST_TITLE_ICON_X        (35)
#define PLAYHIST_TITLE_ICON_Y        ((PLAYHIST_TITLE_CONTH - PLAYHIST_TITLE_ICON_H) >> 1)
#define PLAYHIST_TITLE_ICON_W        (50)
#define PLAYHIST_TITLE_ICON_H        (50)
//name
#define PLAYHIST_TITLE_NAME_X        (PLAYHIST_TITLE_ICON_X + PLAYHIST_TITLE_ICON_W + 10)
#define PLAYHIST_TITLE_NAME_Y        (0)
#define PLAYHIST_TITLE_NAME_W        (116)
#define PLAYHIST_TITLE_NAME_H        (PLAYHIST_TITLE_CONTH)
//video number
#define PLAYHIST_VDO_NUM_X        (PLAYHIST_TITLE_CONTW - PLAYHIST_TITLE_ICON_X - PLAYHIST_VDO_NUM_W)//(770)
#define PLAYHIST_VDO_NUM_Y        (0)
#define PLAYHIST_VDO_NUM_W        (200)
#define PLAYHIST_VDO_NUM_H        (PLAYHIST_TITLE_NAME_H)
//button
#define PLAYHIST_BUTTON_X        (PLAYHIST_VDO_NUM_X - PLAYHIST_BUTTON_W)
#define PLAYHIST_BUTTON_Y        ((PLAYHIST_TITLE_CONTH - PLAYHIST_BUTTON_H) >> 1)
#define PLAYHIST_BUTTON_W        100
#define PLAYHIST_BUTTON_H        COMM_CTRL_H

//list cont
#define PLAYHIST_LIST_CONTX        (90)
#define PLAYHIST_LIST_CONTY        (PLAYHIST_TITLE_CONTY + PLAYHIST_TITLE_CONTH)
#define PLAYHIST_LIST_CONTW        (PLAYHIST_TITLE_CONTW)
#define PLAYHIST_LIST_CONTH        (520)
#define PLAYHIST_LIST_BORDER_X  (20)
#define PLAYHIST_LIST_BORDER_Y  (15)
//list
#define PLAYHIST_LIST_X        (PLAYHIST_LIST_BORDER_X)
#define PLAYHIST_LIST_Y        (PLAYHIST_LIST_BORDER_Y)
#define PLAYHIST_LIST_W       (PLAYHIST_LIST_CONTW - PLAYHIST_LIST_BORDER_X - PLAYHIST_LIST_BORDER_X)
#define PLAYHIST_LIST_H       (PLAYHIST_LIST_CONTH - (PLAYHIST_LIST_BORDER_Y + PLAYHIST_LIST_BORDER_Y + PLAYHIST_BUTTON_H + PLAYHIST_LIST_BORDER_Y))
#define PLAYHIST_LIST_ITEM_VGAP    (10)
//sbar
#define PLAYHIST_SBAR_X           (PLAYHIST_LIST_X + PLAYHIST_LIST_W + 9)
#define PLAYHIST_SBAR_Y           (PLAYHIST_LIST_Y)
#define PLAYHIST_SBAR_W      (6)
#define PLAYHIST_SBAR_H    (PLAYHIST_LIST_H)


/* rect style */
#define RSI_PLAYHIST_LIST       RSI_PBACK
/* font style */

/* others */
#define PLAYHIST_LIST_PAGE    10

#endif
