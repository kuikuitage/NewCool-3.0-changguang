/****************************************************************************

 ****************************************************************************/
#ifndef __UI_GUI_DEFINES_H__
#define __UI_GUI_DEFINES_H__

#define C_TRANS           0x00000
#define C_KEY             0xFF00FEFF

#define C_RED             0xffff0000

#define C_GREEN           0xff00ff00

#define C_BLUE            0xFF1E6498
#define C_YELLOW        0xFFCC9900

#define C_BLACK           0xFF000000
#define C_WHITE           0xFFFFFFFF
#define C_GRAY             0xFF64666B //0xB18D
#define C_BG_GRAY         0xff64666b//0xB18D  // for submenu background
#define C_VERMILION     0xffca5f02


enum rect_style_id
{
  RSI_IGNORE = 0,       //because default color is 0 in lib, this color style means will not be drawn
  RSI_TRANSPARENT,
  RSI_BLUE,
  RSI_YELLOW,
  RSI_WHITE,
  RSI_BLACK,
  RSI_DLG_FRM,
  RSI_GRAY,
  RSI_PBACK,
  RSI_LIST_ITEM_1,
  RSI_LIST_ITEM_2,
  RSI_ITEM_1_SH,
  RSI_ITEM_1_HL,
  RSI_BOX2,
  RSI_SCROLL_BAR_BG,
  RSI_SCROLL_BAR_MID,
  RSI_SUBMENU_DETAIL_BG,
  RSI_MAIN_BG,
  RSI_BOX3_TITLE,
  RSI_OTT_PROGRESS_BAR_BG,
  RSI_OTT_PROGRESS_BAR_MID,
  RSI_OTT_IM_BUTTON_SL,

  RSI_BOX3,
  RSI_OTT_SCROLL_BAR_BG,
  RSI_OTT_SCROLL_BAR,
  RSI_BOX1,

  RSI_OTT_IM_SEARCH,
  RSI_OTT_KEYBOARD_KEY_HL,
  RSI_OTT_KEYBOARD_KEY_SH,
  RSI_POPUP_BG,

  
  RSTYLE_CNT,
};



enum font_style_id
{
  FSI_BLACK = 0,        //black string
  FSI_WHITE,
  FSI_GREEN,
  FSI_RED,
  FSI_GRAY,
  FSI_VERMILION,
  FSTYLE_CNT,
};

#define   FSI_WHITE_16    FSI_WHITE
#define   FSI_WHITE_18    FSI_WHITE
#define   FSI_GREEN_18    FSI_GREEN
#define   FSI_WHITE_16    FSI_WHITE
#define   FSI_WHITE_LARGE  FSI_WHITE
#define   FSI_GRAY_16    FSI_GRAY
#define   FSI_RED_18   FSI_RED
#define   FSI_WHITE_20   FSI_WHITE

enum font_lib_id
{
  FONT_ID_1 = 1,
};

#define MAX_RSTYLE_CNT        50
#define MAX_FSTYLE_CNT        10


extern rsc_config_t g_rsc_config;

#endif

