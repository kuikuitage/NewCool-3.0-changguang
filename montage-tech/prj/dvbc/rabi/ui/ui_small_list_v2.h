/****************************************************************************

****************************************************************************/
#ifndef _UI_SMALL_LIST_V2_H_
#define _UI_SMALL_LIST_V2_H_
/*!
 \file ui_small_list_v2.h

   This file defined the coordinates, rectangle style, font style and some other
   constants used in SMALLor list menu.
   And interfaces for open the SMALLor list menu.

   Development policy:
   If you want to change the menu style, you can just modified the macro
   definitions.
 */

/*coordinate*/
#define SMALL_EXTERN_LEFT             (40)
#define SMALL_MENU_W				  (SMALL_LIST_MENU_WIDTH+2*SMALL_LIST_MENU_CONT_X+SMALL_LIST_AZ_W+SMALL_LIST_MENU_CONT_H_GAP + SMALL_EXTERN_LEFT)
#define SMALL_LIST_MENU_CONT_X        (0)
#define SMALL_LIST_MENU_CONT_Y        (SMALL_LIST_TITLE_CONT_H)
#ifdef BIGFONT_PROJECT
#define SMALL_LIST_MENU_WIDTH         (340)
#else
#define SMALL_LIST_MENU_WIDTH         (305)
#endif
#define SMALL_LIST_MENU_HEIGHT        (SCREEN_HEIGHT - SMALL_LIST_MENU_CONT_Y)

#define SMALL_LIST_MENU_CONT_H_GAP    4
#define SMALL_LIST_MENU_CONT_V_GAP    4

#define SMALL_LIST_TITLE_CONT_H		(SMALL_LIST_SAT_H+SMALL_LIST_SAT_Y)

//group
#define SMALL_LIST_SAT_X              (SLIST_ARROWL_X + SLIST_ARROWL_W)
#define SMALL_LIST_SAT_Y              10
#define SMALL_LIST_SAT_W              (SMALL_LIST_MENU_WIDTH - 2 * SMALL_LIST_SAT_X + SMALL_EXTERN_LEFT) 
#ifdef BIGFONT_PROJECT
#define SMALL_LIST_SAT_H              (COMM_CTRL_H + 10)//30
#else
#define SMALL_LIST_SAT_H              COMM_CTRL_H//30
#endif

//group arrow left
#define SLIST_ARROWL_X              (30 + SMALL_EXTERN_LEFT)
#define SLIST_ARROWL_Y              SMALL_LIST_SAT_Y
#define SLIST_ARROWL_W              24
#define SLIST_ARROWL_H              SMALL_LIST_SAT_H

//group arrow right
#define SLIST_ARROWR_X             (SMALL_LIST_SAT_X + SMALL_LIST_SAT_W)
#define SLIST_ARROWR_Y             SMALL_LIST_SAT_Y
#define SLIST_ARROWR_W             36
#define SLIST_ARROWR_H             SMALL_LIST_SAT_H

//list
#define SMALL_LIST_LIST_X             25
#define SMALL_LIST_LIST_Y             (SMALL_LIST_SAT_Y + SMALL_LIST_SAT_H)
#define SMALL_LIST_LIST_W             (SMALL_LIST_MENU_WIDTH - SMALL_LIST_LIST_X-5)
#ifdef BIGFONT_PROJECT
#define SMALL_LIST_LIST_H             (50*SMALL_LIST_PAGE)
#else
#define SMALL_LIST_LIST_H             (40*SMALL_LIST_PAGE)
#endif

#define SMALL_LIST_ITEM_V_GAP         2

#define SMALL_LIST_MID_L              (0)
#define SMALL_LIST_MID_T              (0)
#define SMALL_LIST_MID_W              (SMALL_LIST_LIST_W - 2 * SMALL_LIST_MID_L)
#define SMALL_LIST_MID_H              (SMALL_LIST_LIST_H - 2 * SMALL_LIST_MID_T)

#define SMALL_LIST_SBAR_X             (SMALL_LIST_LIST_X + SMALL_LIST_LIST_W + SMALL_LIST_MENU_CONT_H_GAP) 
#define SMALL_LIST_SBAR_Y             (SMALL_LIST_LIST_Y)
#define SMALL_LIST_SBAR_W             (6)
#define SMALL_LIST_SBAR_H             SMALL_LIST_LIST_H
#define SMALL_LIST_SBAR_VERTEX_GAP    (12)

#define SMALL_LIST_AZ_X               (SMALL_LIST_MENU_CONT_X + SMALL_LIST_MENU_WIDTH + SMALL_LIST_MENU_CONT_H_GAP)
#define SMALL_LIST_AZ_Y               (SMALL_LIST_LIST_Y)
#define SMALL_LIST_AZ_W               32
#define SMALL_LIST_AZ_H               ((SMALL_LIST_AZ_W+SMALL_LIST_ITEM_V_GAP)*SLIST_AZ_PAGE)

#define SMALL_LIST_AZ_ITEM_L          0
#define SMALL_LIST_AZ_ITEM_T          0
#define SMALL_LIST_AZ_ITEM_W          (SMALL_LIST_AZ_W - 2 * SMALL_LIST_AZ_ITEM_L)
#define SMALL_LIST_AZ_ITEM_H          (SMALL_LIST_AZ_H - 2 * SMALL_LIST_AZ_ITEM_T)

#define SMALL_LIST_AZ_ITEM_V_GAP      SMALL_LIST_ITEM_V_GAP

#define SLIST_SORT_STATUS_X           (SMALL_LIST_MENU_WIDTH - SMALL_LIST_MENU_CONT_H_GAP - SLIST_SORT_STATUS_W)//400
#define SLIST_SORT_STATUS_Y           (SMALL_LIST_LIST_H+SMALL_LIST_MENU_CONT_H_GAP)//417
#define SLIST_SORT_STATUS_W           (100)//100
#define SLIST_SORT_STATUS_H           (30)

//help
#define SMALL_LIST_HELP_CONT_X        120
#define SMALL_LIST_HELP_CONT_Y        580
#define SMALL_LIST_HELP_WIDTH         1024
#define SMALL_LIST_HELP_HEIGHT        90

#define SLIST_HELP_X           120
#define SLIST_HELP_Y           580
#define SLIST_HELP_W           1024
#define SLIST_HELP_H           90

#define SLIST_HELP_ITEM_L           20
#define SLIST_HELP_ITEM_T           10
#define SLIST_HELP_ITEM_R           (SLIST_HELP_W - SLIST_HELP_ITEM_L) 
#define SLIST_HELP_ITEM_B           (SLIST_HELP_H - SLIST_HELP_ITEM_T)

#define SLIST_HELP_HGAP     20
#define SLIST_HELP_VGAP     4

/*rstyle*/
#define RSI_SLIST_LIST                RSI_PBACK          //rect style of the list
#define RSI_SLIST_SBAR                RSI_SCROLL_BAR_BG  //rect style of scroll bar
#define RSI_SLIST_SBAR_MID            RSI_SCROLL_BAR_MID //rect style of scroll bar middle rectangle
#define RSI_SLIST_SAT_CBOX            RSI_LEFT_RIGHT_SH
#define RSI_SLIST_HELP                RSI_BOX_2

/*fstyle*/
#define FSI_SLIST_INFO_EDIT           FSI_WHITE      //font styel of single satellite information edit
#ifdef BIGFONT_PROJECT
#define FSI_SLIST_TEXT                FSI_WHITE_30
#else
#define FSI_SLIST_TEXT                FSI_WHITE
#endif
#define FSI_SLIST_HELP_CONT           FSI_WHITE
#define FSI_SLIST_SORT_STATUS         FSI_WHITE

/*others*/
#ifdef BIGFONT_PROJECT
#define SMALL_LIST_PAGE               12
#else
#ifdef ENABLE_ADS
#define SMALL_LIST_PAGE               12
#else
#define SMALL_LIST_PAGE               15
#endif
#endif
#define SMALL_LIST_FIELD_NUM          2
#define SLIST_HELP_TOL                8
#define SLIST_HELP_COL                4
#define SLIST_HELP_ROW                2

#define SLIST_AZ_PAGE        18
#define SLIST_AZ_TOTAL                  37

typedef enum
{
  MSG_GROUP_UP = MSG_LOCAL_BEGIN + 850,
  MSG_GROUP_DOWN,
  MSG_CHANGE_LIST,
  MSG_NEWS,
  MSG_CHANGE_TO_FAV,
  MSG_SMALL_LIST_BULE,
  MSG_SMALL_LIST_INFO,
  MSG_SOURCE_UP,
  MSG_SOURCE_DOWN,
  MSG_EXTEND_MENU,
  MSG_SMALLIST_UPDATE,
  MSG_PLAY_IN_SLIST,
  MSG_DEL_PG_FROM_VIEW,
  MSG_DELALL_PG_FROM_VIEW,
  MSG_REMOVE_FROM_FAV,
  MSG_SLIST_SAVE,
  MSG_SLIST_PWDLG_EXIT,
  MSG_TIMER_ON,
  MSG_XEXTEND_RESET_TIMER,
  MSG_XEXTEND_STOP_TIMER,
  MSG_XEXTEND_START_TIMER,
}local_msg_t;

typedef enum{
	PLAY_IN_SLIST_FORCE,	//play force if pg already play
	PLAY_IN_SLIST_EXIT,		//exit menu if pg already play
	PLAY_IN_SLIST_NOTHING, //nothing to do if pg already play
}play_in_slist_type_t;

typedef enum
{
  SLIST_PG_TV = 0,
  SLIST_PG_HD,
  SLIST_PG_RADIO,
  SLIST_PG_FAV ,

  SLIST_PG_TYPE_CNT,
}slist_pg_type_t;

typedef enum
{
  IDC_SMALL_LIST_LIST = 1,
  //IDC_SMALL_LIST_SBAR,
  IDC_SMALL_LIST_SAT,
  IDC_SMALL_LIST_STATUS,
  IDC_SMALL_LIST_ARROWL,
  IDC_SMALL_LIST_ARROWR,
}small_list_ctrl_id_t;

slist_pg_type_t get_slist_prog_type(void);
control_t *get_small_list_ctrl(void);
RET_CODE open_small_list_v2(u32 para1, u32 para2);

#endif

