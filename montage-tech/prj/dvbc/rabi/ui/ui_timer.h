/****************************************************************************

****************************************************************************/
#ifndef __UI_TIMER_H__
#define __UI_TIMER_H__

/* coordinate */
#define TIMER_CONT_X        (SYS_LEFT_CONT_W+(RIGHT_ROOT_W - TIMER_CONT_W) / 2)
#define TIMER_CONT_Y        ((SCREEN_HEIGHT- TIMER_CONT_H) / 2)
#define TIMER_CONT_W        650//550//500
#define TIMER_CONT_H        (2 * TIMER_TITLE_Y + TIMER_TITLE_H + TIMER_ITEM_CNT * (TIMER_ITEM_H + TIMER_ITEM_V_GAP) + TIMER_BTN_H + 2*TIMER_ITEM_V_GAP)

#define TIMER_TITLE_X 4
#define TIMER_TITLE_Y 15// 4
#define TIMER_TITLE_W (TIMER_CONT_W - 2 * TIMER_TITLE_X)
#define TIMER_TITLE_H 30

#define TIMER_ITEM_CNT      7
#define TIMER_ITEM_X        20//10
#define TIMER_ITEM_Y        2*TIMER_TITLE_Y+TIMER_TITLE_H
#define TIMER_ITEM_LW       235//192
#define TIMER_ITEM_RW      (TIMER_CONT_W - TIMER_ITEM_X - 2*TIMER_ITEM_V_GAP - TIMER_ITEM_LW)
#define TIMER_ITEM_H        COMM_ITEM_H-10
#define TIMER_ITEM_V_GAP    15//4

#define TIMER_BTN_W 100//64//100
#define TIMER_BTN_X 150//50//70
#define TIMER_BTN_H 30//50
#define TIMER_BTN_HGAP (TIMER_CONT_W - 2 * TIMER_BTN_X - 2 * TIMER_BTN_W)
/* rect style */
#define RSI_TIMER_FRM       RSI_SUBMENU_DETAIL_BG
//#define RSI_TIMER_DLIST     RSI_SUBMENU_DETAIL_BG   //RSI_WINDOW_1
//#define RSI_TIMER_DLIST_SBAR     RSI_SCROLL_BAR_BG
#define RSI_TIMER_DLIST_SBAR_MID    RSI_SCROLL_BAR_MID

/* font style */

/* others */
#define TIMER_GMT_OFFSET_NUM 48
#ifndef IMPL_NEW_EPG
typedef struct
{
  evt_node_t *p_node;
  u16 focus;
} timer_param_epg_t;
#endif
typedef  RET_CODE (*OBJ_TIMER_CB)(void);

typedef struct
{
  OBJ_TIMER_CB cb;
  u16 focus;
} timer_param_list_t;

enum local_msg
{
  MSG_TIMER_UPDATE = MSG_LOCAL_BEGIN + 950,
  MSG_TIMER_CLOSE,
};

enum timer_from_menu
{
  FROM_EPG_MENU,
  FROM_VEPG_MENU,
  FROM_SCHEDULE_MENU,
  FROM_TIMER_SETMENU,
};

RET_CODE open_timer(u32 para1, u32 para2);

#endif

