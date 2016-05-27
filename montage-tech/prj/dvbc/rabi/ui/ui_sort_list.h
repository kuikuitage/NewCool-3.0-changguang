/****************************************************************************

****************************************************************************/
#ifndef _UI_SORT_LIST_H_
#define _UI_SORT_LIST_H_
/*!
 \file ui_fav_set.h

   This file defined the coordinates, rectangle style, font style and some other
   constants used in fav list menu.
   And interfaces for open the fav list menu.

   Development policy:
   If you want to change the menu style, you can just modified the macro
   definitions.
 */

//coordinates
#define SORT_LIST_X               672//112  //180
#define SORT_LIST_Y               405//166 //80
#define SORT_LIST_W               200 //160
#define SORT_LIST_H               (SORT_LIST_ITEM_PAGE * SORT_LIST_ITEM_HEIGHT + 2 * SORT_LIST_LIST_Y)

#define SORT_LIST_LIST_X          0
#define SORT_LIST_LIST_Y          0
#define SORT_LIST_LIST_W          (SORT_LIST_W - 2 * SORT_LIST_LIST_X - \
                                   SORT_LIST_SBARW)
#define SORT_LIST_LIST_H          (SORT_LIST_H - 2 * SORT_LIST_LIST_Y)

#define SORT_LIST_LIST_MIDL       15
#define SORT_LIST_LIST_MIDT       4
#define SORT_LIST_LIST_MIDW       (SORT_LIST_LIST_W - 2*SORT_LIST_LIST_MIDL)
#define SORT_LIST_LIST_MIDH       SORT_LIST_LIST_H-4

#define SORT_LIST_SBARX           (SORT_LIST_LIST_X + SORT_LIST_LIST_W)
#define SORT_LIST_SBARY           SORT_LIST_LIST_Y
#define SORT_LIST_SBARW           6
#define SORT_LIST_SBARH           SORT_LIST_LIST_H

#define SORT_LIST_LIST_VGAP       2

//rstyles
#define RSI_SORT_LIST_MENU        RSI_WINDOW_1
#define RSI_SORT_LIST_SBAR        RSI_SCROLL_BAR_BG
#define RSI_SORT_LIST_SBAR_MID    RSI_SCROLL_BAR_MID

//fstyles


//others
#define SORT_LIST_ITEM_TOL     6//5
#define SORT_LIST_ITEM_PAGE    SORT_LIST_ITEM_TOL
#define SORT_LIST_ITEM_HEIGHT  36
#define SORT_LIST_FIELD_NUM    1

RET_CODE open_sort_list(u32 para1, u32 para2);

#endif
