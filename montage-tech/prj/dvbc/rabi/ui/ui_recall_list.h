/****************************************************************************

****************************************************************************/
#ifndef _UI_RECALL_LIST_H_
#define _UI_RECALL_LIST_H_
/*!
 \file ui_small_list.h

   This file defined the coordinates, rectangle style, font style and some other
   constants used in RECALLor list menu.
   And interfaces for open the RECALLor list menu.

   Development policy:
   If you want to change the menu style, you can just modified the macro
   definitions.
 */

/*coordinate*/

#define RECALL_LIST_ITEM_V_GAP         6
#define RECALL_LIST_ITEM_H_GAP         40

#define RECALL_LIST_MENU_CONT_X        120
#define RECALL_LIST_MENU_CONT_Y       110
#define RECALL_LIST_MENU_WIDTH       450
#define RECALL_LIST_MENU_HEIGHT        490

//title
#define RECALL_LIST_TITLE_X              (20)
#define RECALL_LIST_TITLE_Y              (20)
#define RECALL_LIST_TITLE_W             (RECALL_LIST_MENU_WIDTH - (RECALL_LIST_TITLE_X + RECALL_LIST_TITLE_X))
#define RECALL_LIST_TITLE_H              (40)//28

//top line
#define RECALL_LIST_TOP_LINE_X  40
#define RECALL_LIST_TOP_LINE_Y  80
#define RECALL_LIST_TOP_LINE_W  RECALL_LIST_LIST_W + 60
#define RECALL_LIST_TOP_LINE_H  18

//bottom line
#define RECALL_LIST_BOTTOM_LINE_X  RECALL_LIST_TOP_LINE_X
#define RECALL_LIST_BOTTOM_LINE_Y  (RECALL_LIST_LIST_Y+RECALL_LIST_LIST_H)
#define RECALL_LIST_BOTTOM_LINE_W  RECALL_LIST_TOP_LINE_W
#define RECALL_LIST_BOTTOM_LINE_H  18

//list
#define RECALL_LIST_LIST_X             20
#define RECALL_LIST_LIST_Y           60
#define RECALL_LIST_LIST_W             400
#define RECALL_LIST_LIST_H             380

#define RECALL_LIST_MID_L              4
#define RECALL_LIST_MID_T              4
#define RECALL_LIST_MID_W              (RECALL_LIST_LIST_W - 2 * RECALL_LIST_MID_L)
#define RECALL_LIST_MID_H              (RECALL_LIST_LIST_H - 2 * RECALL_LIST_MID_T)

#define RECALL_LIST_SBAR_X           424 //(RECALL_LIST_MID_W + 2 * RECALL_LIST_ITEM_H_GAP)
#define RECALL_LIST_SBAR_Y             60
#define RECALL_LIST_SBAR_W             6
#define RECALL_LIST_SBAR_H             380

//help
#define RECALL_LIST_HELP_X             20
#define RECALL_LIST_HELP_Y            450
#define RECALL_LIST_HELP_W             410
#define RECALL_LIST_HELP_H             30

#define RECALL_LIST_SBAR_VERTEX_GAP    12

/*rstyle*/
#define RSI_RLIST_CONT                RSI_WINDOW_1       //rect style of program list window
#define RSI_RLIST_LIST                RSI_PBACK          //rect style of the list
#define RSI_RLIST_SBAR                RSI_SCROLL_BAR_BG  //rect style of scroll bar
#define RSI_RLIST_SBAR_MID            RSI_SCROLL_BAR_MID //rect style of scroll bar middle rectangle
#define RSI_RLIST_SAT_CBOX            RSI_PBACK

/*fstyle*/
#define FSI_RLIST_INFO_EDIT           FSI_WHITE      //font styel of single satellite information edit
#define FSI_RLIST_TEXT                FSI_WHITE

/*others*/
#define RECALL_LIST_PAGE               (8)
#define RECALL_LIST_FIELD_NUM          5
#define RLIST_SAT_EDIT_NUM            5

RET_CODE open_recall_list(u32 para1, u32 para2);

#endif

