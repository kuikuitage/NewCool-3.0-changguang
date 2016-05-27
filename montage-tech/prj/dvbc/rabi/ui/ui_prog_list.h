/****************************************************************************

****************************************************************************/
#ifndef __UI_PROG_LIST_H__
#define __UI_PROG_LIST_H__

#include "ui_common.h"

#define PLIST_MENU_X            0//60
#define PLIST_MENU_Y            0//80//60
#define PLIST_MENU_W            (COMM_BG_W - PLIST_MENU_X)
#define PLIST_MENU_H            (COMM_BG_H - PLIST_MENU_Y)

//group container
#define PLIST_GROUP_CONTX 120
#define PLIST_GROUP_CONTY 100//20
#define PLIST_GROUP_CONTW 510
#define PLIST_GROUP_CONTH 50


#define PLIST_LIST_CONTX        120
#define PLIST_LIST_CONTY        160//80
#define PLIST_LIST_CONTW        510//502//552
#define PLIST_LIST_CONTH        450

//top line
#define PLIST_TOP_LINE_X  0
#define PLIST_TOP_LINE_Y  6
#define PLIST_TOP_LINE_W  PLIST_LIST_CONTW
#define PLIST_TOP_LINE_H  18

//bottom line
#define PLIST_BOTTOM_LINE_X  PLIST_TOP_LINE_X
#define PLIST_BOTTOM_LINE_Y  100
#define PLIST_BOTTOM_LINE_W  PLIST_TOP_LINE_W
#define PLIST_BOTTOM_LINE_H  18

//group
#define PLIST_CONT_GROUPX       (PLIST_LIST_CONTW - PLIST_CONT_GROUPW) / 2
#define PLIST_CONT_GROUPY       (PLIST_GROUP_CONTH-PLIST_CONT_GROUPH)/2//0//20
#define PLIST_CONT_GROUPW       350//190//240
#define PLIST_CONT_GROUPH        36


//group arrow left
#define PLIST_GROUP_ARROWL_X       (PLIST_CONT_GROUPX - PLIST_GROUP_ARROWL_W)
#define PLIST_GROUP_ARROWL_Y       PLIST_CONT_GROUPY
#define PLIST_GROUP_ARROWL_W       36
#define PLIST_GROUP_ARROWL_H       36

//group arrow right
#define PLIST_GROUP_ARROWR_X       (PLIST_CONT_GROUPX + PLIST_CONT_GROUPW)
#define PLIST_GROUP_ARROWR_Y       PLIST_CONT_GROUPY
#define PLIST_GROUP_ARROWR_W       36
#define PLIST_GROUP_ARROWR_H       36


//color mbox
#define PLIST_CONT_MBOXX        120//(PLIST_LIST_CONTW - PLIST_CONT_MBOXW) / 2
#define PLIST_CONT_MBOXY        620//540//60
#define PLIST_CONT_MBOXW       1040//400//502//552
#define PLIST_CONT_MBOXH        50//44

//prog list
#define PLIST_LIST_X            10//0
#define PLIST_LIST_Y            10//120
#define PLIST_LIST_W            490//510//480//530
#define PLIST_LIST_H            400//360

#define PLIST_LIST_MIDL  0
#define PLIST_LIST_MIDT  0
#define PLIST_LIST_MIDW  (PLIST_LIST_W - 2 * PLIST_LIST_MIDL)
#define PLIST_LIST_MIDH (PLIST_LIST_H - 2 * PLIST_LIST_MIDT)

#define PLIST_LIST_VGAP         0

#define PLIST_BAR_X             640//394//490//540
#define PLIST_BAR_Y             (PLIST_LIST_Y + PLIST_LIST_CONTY + PLIST_LIST_MIDT)
#define PLIST_BAR_W             6
#define PLIST_BAR_H             PLIST_LIST_MIDH

//preview
#define PLIST_PREV_X            672//655//672//408//415//520//570
#define PLIST_PREV_Y            100//20 //10//30
#define PLIST_PREV_W            488//320 //270
#define PLIST_PREV_H            300//204

#define PLIST_BRIEF_ICON_X          20//435// 510//560
#define PLIST_BRIEF_ICON_Y          20//310// 315//260
#define PLIST_BRIEF_ICON_W           20//60
#define PLIST_BRIEF_ICON_H           20//60

#define PLIST_BRIEF_X           672//655//672//505// 616
#define PLIST_BRIEF_Y           410//330//315//260
#define PLIST_BRIEF_W           488//250//206
#define PLIST_BRIEF_H           200//202

#define PLIST_BRIFE_ITEM_CNT     5

#define PLIST_BRIEF_ITEMAX       60//0
#define PLIST_BRIEF_ITEMAW       (PLIST_BRIEF_W - 2 * PLIST_BRIEF_ITEMAX)

#define PLIST_BRIEF_ITEMBX       60//0 //4
#define PLIST_BRIEF_ITEMBW       (PLIST_BRIEF_W - 2 * PLIST_BRIEF_ITEMBX)

#define PLIST_BRIEF_ITEMY       20//5//12
#define PLIST_BRIEF_ITEM_GAP    5//10
#define PLIST_BRIEF_ITEMH      32// 26

#define PLIST_ICON_X            0
#define PLIST_ICON_Y            0
#define PLIST_ICON_W            80
#define PLIST_ICON_H            80

#define PLIST_PWD_X             COMM_DLG_X
#define PLIST_PWD_Y             COMM_DLG_Y
#define PLIST_PWD_W             COMM_DLG_W
#define PLIST_PWD_H             COMM_DLG_H

/*rstyle*/

#define RSI_PLIST_MENU          RSI_SUBMENU_DETAIL_BG//RSI_PBACK
#define RSI_COMMAN_BG           RSI_SUBMENU_DETAIL_BG//add
//#define RSI_PLIST_LIST_CONT     RSI_WINDOW_2
#define RSI_PLIST_TITLE         RSI_ITEM_1_HL
//#define RSI_PLIST_GROUP         RSI_LEFT_RIGHT
#define RSI_PLIST_MBOX          RSI_BOX_1//RSI_PBACK
//#define RSI_PLIST_LIST          RSI_WINDOW_2
#define RSI_PLIST_SBAR          RSI_SCROLL_BAR_BG
#define RSI_PLIST_SBAR_MID      RSI_SCROLL_BAR_MID
#define RSI_PLIST_PREV          RSI_TV
#define RSI_PLIST_BRIEF         RSI_BOX_1//RSI_PBACK
#define RSI_PLIST_BRIEF_ITEM    RSI_BOX_1//RSI_COMMAN_MID_BG//RSI_PBACK

/*fstyle*/
#define FSI_PLIST_MBOX          FSI_BLACK
#define FSI_PLIST_CBOX          FSI_PROGBAR_BOTTOM
#define FSI_PLIST_TITLE         FSI_WHITE
#define FSI_PLIST_BRIEF        FSI_PROGLIST_BRIEF

/*others*/
#define PLIST_LIST_PAGE         10
#define PLIST_LIST_FIELD        7//6
#define PLIST_MBOX_TOL          6//5
#define PLIST_MBOX_COL          6
#define PLIST_MBOX_ROW          1
#define PLIST_MBOX_HGAP         6
#define PLIST_MBOX_VGAP         0
#define PLIST_CBOX_PAGE         4

#define PLIST_MOVE_HELP_X       15
#define PLIST_MOVE_HELP_Y       (PLIST_LIST_Y + PLIST_LIST_H + 10)
#define PLIST_MOVE_HELP_W       (PLIST_LIST_W - PLIST_MOVE_HELP_X)
#define PLIST_MOVE_HELP_H       30

RET_CODE open_prog_list(u32 para1, u32 para2);

void plist_set_modify_state(BOOL state);

#endif

