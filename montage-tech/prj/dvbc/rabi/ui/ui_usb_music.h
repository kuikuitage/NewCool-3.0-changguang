/****************************************************************************

 ****************************************************************************/
#ifndef _UI_USB_MUSIC_H_
#define _UI_USB_MUSIC_H_

#include "ui_common.h"

/*coordinates*/
#define MP_MENU_X  (SCREEN_WIDTH - MP_MENU_W)/2
#define MP_MENU_Y  (SCREEN_HEIGHT - MP_MENU_H)/2
#define MP_MENU_W  634//SCREEN_WIDTH
#define MP_MENU_H  420//388//SCREEN_HEIGHT

//title icons
#define MP_TITLE_X  180
#define MP_TITLE_Y  64
#define MP_TITLE_W  640
#define MP_TITLE_H  78

//title1 text
#define MP_TITLE_TEXT1_X 0
#define MP_TITLE_TEXT1_Y 10
#define MP_TITLE_TEXT1_W 150
#define MP_TITLE_TEXT1_H 105

//group container
#define MP_GROUP_CONTX 70
#define MP_GROUP_CONTY 0
#define MP_GROUP_CONTW 300
#define MP_GROUP_CONTH 50

//group
#define MP_GROUP_X  0
#define MP_GROUP_Y  7
#define MP_GROUP_W 300
#define MP_GROUP_H  36

//group arrow left
#define MP_GROUP_ARROWL_X       44
#define MP_GROUP_ARROWL_Y       7
#define MP_GROUP_ARROWL_W      36
#define MP_GROUP_ARROWL_H       36

//group arrow right
#define MP_GROUP_ARROWR_X       480
#define MP_GROUP_ARROWR_Y       7
#define MP_GROUP_ARROWR_W      36
#define MP_GROUP_ARROWR_H       36

//preview
#define MUSIC_PREV_X  0
#define MUSIC_PREV_Y  0
#define MUSIC_PREV_W  1280
#define MUSIC_PREV_H  720

//preview logo
#define MUSIC_PREV_LOGO_X  ((MUSIC_PREV_W + MUSIC_LIST_SBAR_X-MUSIC_PREV_LOGO_W)/2)
#define MUSIC_PREV_LOGO_Y  ((MUSIC_PREV_H - 150 - MUSIC_PREV_LOGO_H)/2)
#define MUSIC_PREV_LOGO_W  250
#define MUSIC_PREV_LOGO_H  150

//list scroll bar
#define MUSIC_LIST_SBAR_X  370
#define MUSIC_LIST_SBAR_Y  50
#define MP_LIST_SBARW  6
#define MP_LIST_SBARH  660

#define MP_LIST_SBAR_MIDL  0
#define MP_LIST_SBAR_MIDT  12
#define MP_LIST_SBAR_MIDR  12
#define MP_LIST_SBAR_MIDB  (MP_LIST_SBARH - MP_LIST_SBAR_MIDT)

//plist list container
#define MP_LIST_LIST_CONTX   150
#define MP_LIST_LIST_CONTY   10
#define MP_LIST_LIST_CONTW  300
#define MP_LIST_LIST_CONTH   700

//list
#define MUSIC_LIST_X  5
#define MUSIC_LIST_Y  5
#define MP_LISTW  290
#define MP_LISTH  660

#define MP_LIST_MIDL  4
#define MP_LIST_MIDT  4
#define MP_LIST_MIDW  (MP_LISTW - 2 * MP_LIST_MIDL)
#define MP_LIST_MIDH  (MP_LISTH - 2 * MP_LIST_MIDT)

#define MP_LCONT_LIST_VGAP 4


//bottom infor
#define MUSIC_BOTTOM_INFO_X  500
#define MUSIC_BOTTOM_INFO_Y  0
#define MUSIC_BOTTOM_INFO_W  300
#define MUSIC_BOTTOM_INFO_H  50

//bottom help container
#define MUSIC_BOTTOM_HELP_X  1125
#define MUSIC_BOTTOM_HELP_Y  455
#define MUSIC_BOTTOM_HELP_W  155
#define MUSIC_BOTTOM_HELP_H  60

#define MUSIC_BOTTOM_HELP_BMP_X  5
#define MUSIC_BOTTOM_HELP_BMP_Y  5
#define MUSIC_BOTTOM_HELP_BMP_W  50
#define MUSIC_BOTTOM_HELP_BMP_H  50

#define MUSIC_BOTTOM_HELP_TXT_X  55
#define MUSIC_BOTTOM_HELP_TXT_Y  5
#define MUSIC_BOTTOM_HELP_TXT_W  100
#define MUSIC_BOTTOM_HELP_TXT_H  50

//detail container
//#define MP_DETAIL_CONT_X  672
//#define MP_DETAIL_CONT_Y  410
//#define MP_DETAIL_CONT_W  488
//#define MP_DETAIL_CONT_H  150

#define MP_DETAIL_FILENAME_X  40
#define MP_DETAIL_FILENAME_Y  0
#define MP_DETAIL_FILENAME_W  450
#define MP_DETAIL_FILENAME_H  50


#define MP_LEFT_BG_X 0
#define MP_LEFT_BG_Y 0
#define MP_LEFT_BG_W 460
#define MP_LEFT_BG_H  720

#define MP_LEFT_HD_X 0
#define MP_LEFT_HD_Y 10
#define MP_LEFT_HD_W 150
#define MP_LEFT_HD_H  700

#define MP_DETAIL_VGAP 0

//play container
#define MP_PLAY_CONT_X  465
#define MP_PLAY_CONT_Y  520
#define MP_PLAY_CONT_W  815
#define MP_PLAY_CONT_H  200

#define MP_PLAY_BOTTOM_LINE_X  0
#define MP_PLAY_BOTTOM_LINE_Y  102
#define MP_PLAY_BOTTOM_LINE_W  438
#define MP_PLAY_BOTTOM_LINE_H  18

#define MP_PLAY_MODE_X  710
#define MP_PLAY_MODE_Y  72
#define MP_PLAY_MODE_W  28
#define MP_PLAY_MODE_H  28

#define MP_PLAY_PROGRESS_X  70
#define MP_PLAY_PROGRESS_Y  75
#define MP_PLAY_PROGRESS_W  570
#define MP_PLAY_PROGRESS_H  16
#define MP_PLAY_PROGRESS_MIDX 0
#define MP_PLAY_PROGRESS_MIDY 0
#define MP_PLAY_PROGRESS_MIDW MP_PLAY_PROGRESS_W
#define MP_PLAY_PROGRESS_MIDH MP_PLAY_PROGRESS_H
#define MP_PLAY_PROGRESS_MIN   RSI_PROGRESS_BAR_MID_BLUE//RSI_PLAY_PBAR_FRONT
#define MP_PLAY_PROGRESS_MAX RSI_PROGRESS_BAR_BG//RSI_PLAY_PBAR_BACK
#define MP_PLAY_PROGRESS_MID INVALID_RSTYLE_IDX


#define MP_PLAY_CURTM_X  500
#define MP_PLAY_CURTM_Y  40
#define MP_PLAY_CURTM_W  70
#define MP_PLAY_CURTM_H  30

#define MP_PLAY_TOLTM_X  540//338
#define MP_PLAY_TOLTM_Y  106
#define MP_PLAY_TOLTM_W  120
#define MP_PLAY_TOLTM_H  30

#define MP_PLAY_CTRL_ICON_X  40
#define MP_PLAY_CTRL_ICON_Y  110
#define MP_PLAY_CTRL_ICON_W  650
#define MP_PLAY_CTRL_ICON_H  72

//play list container
#define MP_PLAY_LIST_CONT_X  950//123
#define MP_PLAY_LIST_CONT_Y  20//40
#define MP_PLAY_LIST_CONT_W 300
#define MP_PLAY_LIST_CONT_H  400

//play list title
#define MP_PLAY_LIST_TITLE_X  ((MP_PLAY_LIST_CONT_W - MP_PLAY_LIST_TITLE_W)/2)
#define MP_PLAY_LIST_TITLE_Y  5
#define MP_PLAY_LIST_TITLE_W  180
#define MP_PLAY_LIST_TITLE_H  30

//play list scroll bar
#define MP_PLAY_LIST_SBARX  0
#define MP_PLAY_LIST_SBARY  0
#define MP_PLAY_LIST_SBARW  6
#define MP_PLAY_LIST_SBARH  300

#define MP_PLAY_LIST_SBAR_MIDL  0
#define MP_PLAY_LIST_SBAR_MIDT  12
#define MP_PLAY_LIST_SBAR_MIDR  12
#define MP_PLAY_LIST_SBAR_MIDB  (MP_PLAY_LIST_SBARH - MP_PLAY_LIST_SBAR_MIDT)

//play list
#define MP_PLAY_LIST_LIST_X  5
#define MP_PLAY_LIST_LIST_Y  (MP_PLAY_LIST_TITLE_Y + MP_PLAY_LIST_TITLE_H + 5)
#define MP_PLAY_LIST_LIST_W  (MP_PLAY_LIST_CONT_W - MP_PLAY_LIST_LIST_X*2)
#define MP_PLAY_LIST_LIST_H  (MP_PLAY_LIST_CONT_H - MP_PLAY_LIST_LIST_Y - 5)

#define MP_PLAY_LIST_LIST_MIDL  4
#define MP_PLAY_LIST_LIST_MIDT  4
#define MP_PLAY_LIST_LIST_MIDW  (MP_PLAY_LIST_LIST_W - 2 * MP_PLAY_LIST_LIST_MIDL)
#define MP_PLAY_LIST_LIST_MIDH  (MP_PLAY_LIST_LIST_H - 2 * MP_PLAY_LIST_LIST_MIDT)

#define MP_PLAY_LIST_LIST_VGAP 0

//play list help
#define MP_PLAY_LIST_HELP_X  10
#define MP_PLAY_LIST_HELP_Y  (MP_PLAY_LIST_CONT_H - MP_PLAY_LIST_HELP_H - 10)
#define MP_PLAY_LIST_HELP_W  (MP_PLAY_LIST_CONT_W - MP_PLAY_LIST_HELP_X*2)
#define MP_PLAY_LIST_HELP_H  70

//sort list
#define MUSIC_SORT_LIST_X  465 
#define MUSIC_SORT_LIST_Y  50
#define MUSIC_SORT_LIST_W  250
#define MUSIC_SORT_LIST_H  200

#define MUSIC_SORT_LIST_MIDL       5
#define MUSIC_SORT_LIST_MIDT       5
#define MUSIC_SORT_LIST_MIDW       (MUSIC_SORT_LIST_W - 2*MUSIC_SORT_LIST_MIDL)
#define MUSIC_SORT_LIST_MIDH       (MUSIC_SORT_LIST_H - 2*MUSIC_SORT_LIST_MIDT)

#define MP_SORT_LIST_VGAP       0

//help
#define MP_HELP_X  100 
#define MP_HELP_Y  505
#define MP_HELP_W  800
#define MP_HELP_H  28

//save or no
#define MP_SAVE_R  840
#define MP_SAVE_L  440
#define MP_SAVE_B  400
#define MP_SAVE_T  280

/*rstyle*/
#define RSI_MP_MENU    RSI_IGNORE
#define RSI_MP_PREV  RSI_TV
#define RSI_MP_DETAIL  RSI_BOX_1//RSI_PBACK
#define RSI_MP_TITLE RSI_PBACK
#define RSI_MP_LIST_CONT RSI_PBACK
#define RSI_MP_HELP_CONT RSI_PBACK
#define RSI_MP_SBAR RSI_SCROLL_BAR_BG
#define RSI_MP_SBAR_MID RSI_SCROLL_BAR_MID
#define RSI_MP_LCONT_LIST  RSI_PBACK
#define RSI_MP_LCONT_MP_CONT  RSI_PBACK
#define RSI_MP_LCONT_MBOX  RSI_BOX_2//RSI_LINE_03_RIGHT
#define RSI_MP_LCONT_DATE    RSI_BOX_1 //RSI_LINE_03_LEFT
#define RSI_MP_LCONT_MBOX_ITEM   RSI_BOX_1//RSI_LINE_03_MID
#define RSI_MP_TPBAR RSI_PBACK
#define RSI_MP_TPBAR_MIN RSI_PBACK
#define RSI_MP_TPBAR_MAX RSI_PBACK
#define RSI_MP_TPBAR_MID    RSI_DETAIL_SBAR_MID//RSI_PROGRESS_BAR_MID_TIME

/*others*/
#define MUSIC_LIST_ITEM_NUM_ONE_PAGE  16
#define MP_LIST_FIELD 4

#define MUSIC_FAV_LIST_MAX_ITEM_NUME 12
#define MP_PLAY_LIST_LIST_FIELD 4

#define MP_PLAY_MBOX_TOL  6
#define MP_PLAY_MBOX_COL  6
#define MP_PLAY_MBOX_ROW  1
#define MP_PLAY_MBOX_HGAP  0
#define MP_PLAY_MBOX_VGAP  0

//
#define MP_PLAY_LIST_HELP_MBOX_TOL  4
#define MP_PLAY_LIST_HELP_MBOX_COL  2
#define MP_PLAY_LIST_HELP_MBOX_ROW  2
#define MP_PLAY_LIST_HELP_MBOX_HGAP  0
#define MP_PLAY_LIST_HELP_MBOX_VGAP  0

//
#define MP_HELP_MBOX_TOL  5
#define MP_HELP_MBOX_COL  5
#define MP_HELP_MBOX_ROW  1
#define MP_HELP_MBOX_HGAP  0
#define MP_HELP_MBOX_VGAP  0

//sort list
#define MUSIC_SORT_LIST_ITEM_TOL     6
#define MUSIC_SORT_LIST_ITEM_PAGE    MUSIC_SORT_LIST_ITEM_TOL
#define MUSIC_SORT_LIST_ITEM_HEIGHT  20
#define MP_SORT_LIST_FIELD_NUM    1


#define MP_TIME_PBAR_MIN 0
#define MP_TIME_PBAR_MAX 66

#define MP_TIME_TOT_HOURS 2

/*fstyle*/
#define FSI_MP_LCONT_MBOX FSI_WHITE
#define FSI_MP_INFO_MBOX FSI_WHITE

/////lrc
#define LRC_MULTI_LEFT   480
#define LRC_MULTI_TOP    100
#define LRC_SING_LEFT    716//480
#define LRC_SING_TOP     360//330
#define LRC_SING_HEIGHT   20
#define LYC_WIDTH        400
#define LYC_HEIGHT       360//280

#define LRC_SHOW_CNT     8


RET_CODE open_usb_music(u32 para1, u32 para2);

void ui_usb_music_exit();

void ui_music_reset_curn(u16 *p_path);
void music_reset_time_info();
#endif
