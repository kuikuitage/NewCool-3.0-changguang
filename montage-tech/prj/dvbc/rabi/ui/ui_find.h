/****************************************************************************

****************************************************************************/
#ifndef __UI_FIND_H__
#define __UI_FIND_H__
//coordinate

#define FIND_ITEM_V_GAP  6         
#define FIND_ITEM_H_GAP  4

#define FIND_MENU_CONT_X        (0)
#define FIND_MENU_CONT_Y        (0)
#define FIND_MENU_CONT_WIDTH    SCREEN_WIDTH
#define FIND_MENU_CONT_HEIGHT   SCREEN_HEIGHT

// list cont
#define FIND_LIST_CONT_X        (80)
#define FIND_LIST_CONT_Y        (110)
#define FIND_LIST_CONT_W        (370)
#define FIND_LIST_CONT_H        (486)

//list
#define FIND_LIST_X             (20)
#define FIND_LIST_Y             (20)
#define FIND_LIST_W             (320)
#define FIND_LIST_H             (390)

#define FIND_LIST_MID_L         (0)
#define FIND_LIST_MID_T         (0)
#define FIND_LIST_MID_W         FIND_LIST_W
#define FIND_LIST_MID_H         FIND_LIST_H

#define FIND_SBAR_X             (FIND_LIST_X + FIND_LIST_W + 4)
#define FIND_SBAR_Y             FIND_LIST_Y
#define FIND_SBAR_W             (6)
#define FIND_SBAR_H             FIND_LIST_H

#define FIND_SBAR_VERTEX_GAP    (12)

//keyboard container
#define FIND_EDIT_CONTX         (460)//(FIND_MENU_CONT_WIDTH - FIND_EDIT_CONTW)
#define FIND_EDIT_CONTY         (110)//(FIND_LIST_CONT_Y + 12)
#define FIND_EDIT_CONTW         (424)
#define FIND_EDIT_CONTH         (486)

/*rstyle*/
#define RSI_FIND_CONT           RSI_TRANSPARENT//rect style of program list window
#define RSI_FIND_LIST_CONT      RSI_POPUP_BG//rect style of the list
#define RSI_FIND_LIST           RSI_PBACK//rect style of the list
#define RSI_FIND_SBAR           RSI_SCROLL_BAR_BG //rect style of scroll bar
#define RSI_FIND_SBAR_MID       RSI_SCROLL_BAR_MID//rect style of scroll bar middle rectangle

/*fstyle*/
#define FSI_FIND_TEXT           FSI_BLACK     //font styel of single satellite information edit
 

/*others*/
#define FIND_LIST_PAGE                      9
#define FIND_LIST_FIELD_NUM            2

RET_CODE open_find(u32 para1, u32 para2);
#endif


