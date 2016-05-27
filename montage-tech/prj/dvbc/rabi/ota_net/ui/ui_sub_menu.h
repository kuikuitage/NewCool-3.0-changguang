/****************************************************************************

****************************************************************************/
#ifndef __UI_SUBMENU_H__
#define __UI_SUBMENU_H__


/*********************************************************************
 * COMMON TITLE
 *********************************************************************/
/* coordinate */
#define TITLE_W                 1280
#define TITLE_H                 60

#define TITLE_ICON_X             40
#define TITLE_ICON_Y             0
#define TITLE_ICON_W             70
#define TITLE_ICON_H             TITLE_H

#define TITLE_TXT_X             140
#define TITLE_TXT_Y             30
#define TITLE_TXT_W             300
#define TITLE_TXT_H             30
//--------------------------------------------


#define MAINMENU_CONT_X          ((SCREEN_WIDTH - MAINMENU_CONT_W)/2)
#define MAINMENU_CONT_Y          ((SCREEN_HEIGHT - MAINMENU_CONT_H)/2)
#define MAINMENU_CONT_W         1280
#define MAINMENU_CONT_H          720

/* coordinate */
#define SUBMENU_CONT_X           MAINMENU_CONT_X
#define SUBMENU_CONT_Y           MAINMENU_CONT_Y
#define SUBMENU_CONT_W          MAINMENU_CONT_W
#define SUBMENU_CONT_H           MAINMENU_CONT_H

#define SUBMENU_ICON_X           100
#define SUBMENU_ICON_Y           240
#define SUBMENU_ICON_W           220
#define SUBMENU_ICON_H           360

#define SUBMENU_ITEM_X           10
#define SUBMENU_ITEM_Y           10
#define SUBMENU_ITEM_W           180
#define SUBMENU_ITEM_H           140

#define SUBMENU_MBOX_VGAP        20
#define SUBMENU_MBOX_HGAP        0
#define SUBMENU_MBOX_OX_ICON     35
#define SUBMENU_MBOX_OX_STR      40
#define SUBMENU_MBOX_SINGLE_H    32

#define SUBMENU_DEL_ALL_X           (SUBMENU_CONT_X+(SUBMENU_CONT_W-SUBMENU_DEL_ALL_W)/2)
#define SUBMENU_DEL_ALL_Y           COMM_DLG_Y
#define SUBMENU_DEL_ALL_W           COMM_DLG_W
#define SUBMENU_DEL_ALL_H           COMM_DLG_H

#define SUBMENU_RESTORE_X           (SUBMENU_CONT_X+(SUBMENU_CONT_W-SUBMENU_RESTORE_W)/2)
#define SUBMENU_RESTORE_Y           (SUBMENU_CONT_Y+(SUBMENU_CONT_H+TITLE_H-SUBMENU_RESTORE_H)/2)
#define SUBMENU_RESTORE_W           400
#define SUBMENU_RESTORE_H           200

//main title
#define MAIN_TITLE_TXT_X             160
#define MAIN_TITLE_TXT_Y             60
#define MAIN_TITLE_TXT_W             200
#define MAIN_TITLE_TXT_H             TITLE_H

// sub title
#define SUB_TITLE_TXT_X              370
#define SUB_TITLE_TXT_Y              60
#define SUB_TITLE_TXT_W              750
#define SUB_TITLE_TXT_H              TITLE_H

// main mbox
#define  MAIN_MBOX_X                 160
#define  MAIN_MBOX_Y                 120
#define  MAIN_MBOX_W                 200
#define  MAIN_MBOX_H                 160

//sys cont--------------------------------------
#define  SYS_CONT_LIST_X             20
#define  SYS_CONT_LIST_Y             20
#define  SYS_CONT_LIST_W             710
#define  SYS_CONT_LIST_H             130

#define  INFO_LIST_ITEM_VGAP         20
#define  INFO_LIST_CNT               3
#define  INFO_LIST_FIELD             2
//-----------------------------------------------

//play cont--------------------------------------
#define  PLAY_CONT_X                 370
#define  PLAY_CONT_Y                 120
#define  PLAY_CONT_W                 750
#define  PLAY_CONT_H                 540

#define TVSYS_SET_ITEM_CNT           3
#define TVSYS_SET_ITEM_X             20//COMM_ITEM_OX_IN_ROOT
#define TVSYS_SET_ITEM_Y             20//COMM_ITEM_OY_IN_ROOT
#define TVSYS_SET_ITEM_LW            300//COMM_ITEM_LW
#define TVSYS_SET_ITEM_RW            410//(COMM_ITEM_MAX_WIDTH - TVSYS_SET_ITEM_LW)
#define TVSYS_SET_ITEM_H             30
#define TVSYS_SET_ITEM_V_GAP         20
//-------------------------------------------------

//net cont-----------------------------------------
//weather  area cont
#define NETWORK_ITEM_X         20//120
#define NETWORK_ITEM_Y         20//90//100
#define NETWORK_ITEM_W         710
#define NETWORK_ITEM_LW        300
#define NETWORK_ITEM_RW        NETWORK_ITEM_W-NETWORK_ITEM_LW
#define PING_ITEM_X            175
#define PING_ITEM_LW           300
#define PING_ITEM_RW           410
#define NETWORK_ITEM_H         30
#define NETWORK_ITEM_VGAP      15

//connect status text
#define NETWORK_CONNECT_STATUS_TEXTX  450
#define NETWORK_CONNECT_STATUS_TEXTY  500
#define NETWORK_CONNECT_STATUS_TEXTW  150
#define NETWORK_CONNECT_STATUS_TEXTH  NETWORK_ITEM_H

//connect status
#define NETWORK_CONNECT_STATUSX     NETWORK_CONNECT_STATUS_TEXTX+NETWORK_CONNECT_STATUS_TEXTW
#define NETWORK_CONNECT_STATUSY     NETWORK_CONNECT_STATUS_TEXTY
#define NETWORK_CONNECT_STATUSW     NETWORK_CONNECT_STATUS_TEXTW
#define NETWORK_CONNECT_STATUSH     NETWORK_CONNECT_STATUS_TEXTH


#define IP_CTRL_CNT 9
#define CONFIG_MODE_TYPE_CNT 3
//-------------------------------------------------

//upgrade cont--------------------------------------
#define SUBMENU_MBOX_X           20
#define SUBMENU_MBOX_Y           20
#define SUBMENU_MBOX_W           710
#define SUBMENU_MBOX_H           40

#define up_cont_item_count         1
//--------------------------------------------------

//search cont--------------------------------------
#define SEARCH_ITEM_COUNT         3

#define SEARCH_SET_ITEM_X             50
#define SEARCH_SET_ITEM_Y             20
#define SEARCH_SET_ITEM_LW            200
#define SEARCH_SET_ITEM_RW            410
#define SEARCH_SET_ITEM_H             30

//signal pbar
#define SIGNAL_PBAR_X             110
#define SIGNAL_PBAR_Y             157
#define SIGNAL_PBAR_W            550
#define SIGNAL_PBAR_H            12

#define SIGNAL_PBAR_TXT_X            30
#define SIGNAL_PBAR_TXT_Y             150
#define SIGNAL_PBAR_TXT_W            80
#define SIGNAL_PBAR_TXT_H            30

#define SIGNAL_PBAR_PERCENT_X             660
#define SIGNAL_PBAR_PERCENT_Y             150
#define SIGNAL_PBAR_PERCENT_W            60
#define SIGNAL_PBAR_PERCENT_H            30

#define SIGNAL_START_TXT_X             315
#define SIGNAL_START_TXT_Y             300
#define SIGNAL_START_TXT_W            120
#define SIGNAL_START_TXT_H            35

#define SEARCH_MSG_TXT_X             150
#define SEARCH_MSG_TXT_Y             250
#define SEARCH_MSG_TXT_W            400
#define SEARCH_MSG_TXT_H            100

#define SEARCH_PBAR_X              110
#define SEARCH_PBAR_Y              407
#define SEARCH_PBAR_W              550
#define SEARCH_PBAR_H              12
#define SEARCH_TXT_X             30
#define SEARCH_TXT_Y             400
#define SEARCH_TXT_W             80
#define SEARCH_TXT_H             30
#define SEARCH_PER_X             660
#define SEARCH_PER_Y             400
#define SEARCH_PER_W             60
#define SEARCH_PER_H             30
/*! signal pbar style */
#define RSI_SIGNAL_PBAR_BG     RSI_OTT_PROGRESS_BAR_BG
#define RSI_SIGNAL_PBAR_MID    RSI_OTT_PROGRESS_BAR_MID


/* rect style */
#define RSI_SUBMENU_FRM           RSI_MAIN_BG
#define RSI_SUBMENU_MBOX          RSI_MAIN_BG
#define RSI_SUBMENU_ITEM_SH      RSI_PBACK//RSI_ITEM_3_SH
#define RSI_SUBMENU_ITEM_HL      RSI_LIST_ITEM_1//RSI_ITEM_3_HL
#define RSI_SUBMENU_ITEM_GRAY      RSI_PBACK//RSI_ITEM_3_HL

#define PWD_DLG_FOR_CHK_X      ((SCREEN_WIDTH-PWDLG_W)/2)
#define PWD_DLG_FOR_CHK_Y      ((SCREEN_HEIGHT-PWDLG_H)/2)

#define main_item_count  3//5
#define sub_mneu_item_count 1 // 2

/* font style */

/* others */

enum sub_menu_control_id
{
  IDC_INVALID = 0,
  IDC_MAIN_TITLE_TXT,
  IDC_TITLE_TXT,
  IDC_MAIN_MBOX,
  IDC_SYS_CONT,
  IDC_NET_CONT,
  IDC_UPGRADE_CONT,
  IDC_UPGRADE_MBOX,
};
RET_CODE open_sub_menu(u32 para1, u32 para2);

#endif

