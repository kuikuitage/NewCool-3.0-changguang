/****************************************************************************

 ****************************************************************************/
#ifndef _UI_FAV_LIST_H_
#define _UI_FAV_LIST_H_
/*!
 \file ui_fav_list.h

   This file defined the coordinates, rectangle style, font style and some other
   constants used in SMALLor list menu.
   And interfaces for open the SMALLor list menu.

   Development policy:
   If you want to change the menu style, you can just modified the macro
   definitions.
 */
#include "ui_common.h"

/*coordinate*/

#define FAV_LIST_ITEM_V_GAP  6         
#define FAV_LIST_ITEM_H_GAP  4

#define FAV_LIST_CONT_GARY		30
#define FAV_LIST_MENU_WIDTH  (FAV_LIST_LIST_W + FAV_LIST_SBAR_W + 2*FAV_LIST_LIST_X)
#define FAV_LIST_MENU_HEIGHT (FAV_LIST_LIST_H+2*FAV_LIST_LIST_Y)
#define FAV_LIST_MENU_CONT_X 100
#define FAV_LIST_MENU_CONT_Y ((SCREEN_HEIGHT - FAV_LIST_MENU_HEIGHT)/2)
/*
#define FAV_LIST_TTL_X 50
#define FAV_LIST_TTL_Y 20//60
#define FAV_LIST_TTL_W FAV_LIST_LIST_W
#define FAV_LIST_TTL_H 50 
*/
#define FAV_LIST_LIST_X  10
#define FAV_LIST_LIST_Y  10
#define FAV_LIST_LIST_W  310
#define FAV_LIST_LIST_H  380

#define FAV_LIST_MID_L 4
#define FAV_LIST_MID_T 4
#define FAV_LIST_MID_W (FAV_LIST_LIST_W - 2 * FAV_LIST_MID_L)
#define FAV_LIST_MID_H (FAV_LIST_LIST_H - 2 * FAV_LIST_MID_T)

#define FAV_LIST_SBAR_X  (FAV_LIST_LIST_X + FAV_LIST_LIST_W)
#define FAV_LIST_SBAR_Y  (FAV_LIST_LIST_Y + FAV_LIST_MID_T)
#define FAV_LIST_SBAR_W  6
#define FAV_LIST_SBAR_H FAV_LIST_MID_H

#define FAV_LIST_SBAR_VERTEX_GAP         12

/*rstyle*/
#define RSI_FLIST_CONT              RSI_WINDOW_1//rect style of program list window
#define RSI_FLIST_LIST                RSI_PBACK//rect style of the list
#define RSI_FLIST_SBAR              RSI_SCROLL_BAR_BG //rect style of scroll bar
#define RSI_FLIST_SBAR_MID       RSI_SCROLL_BAR_MID//rect style of scroll bar middle rectangle
#define RSI_FLIST_TITLE       RSI_PBACK

/*fstyle*/
#define FSI_FLIST_TEXT                      FSI_WHITE     

/*others*/
#define FAV_LIST_PAGE                      9
#define FAV_LIST_FIELD_NUM            2

enum fav_list_control_id
{
  IDC_FAV_LIST_LIST = 1,
  IDC_FAV_LIST_SBAR,
  IDC_FAV_LIST_TTL,
  
  IDC_FAV_LIST_CONT,
};

control_t *get_fav_list_cont_ctrl(void);
RET_CODE open_fav_list(u32 para1, u32 para2);

#endif
