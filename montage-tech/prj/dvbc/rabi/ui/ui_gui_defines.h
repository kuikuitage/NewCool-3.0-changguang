/****************************************************************************

 ****************************************************************************/
#ifndef __UI_GUI_DEFINES_H__
#define __UI_GUI_DEFINES_H__

#define C_TRANS           0x00000000
#define C_KEY             0xff00feff

#define C_TRANS_8BIT          0xFE
#define C_KEY_8BIT             0xFF


#define C_BLACK          0xFF000000
#define C_WHITE           0xFFFFFFFF
#define C_MAIN_LOGO_BG 0xFF1d8bed

#define C_BLUE_BACK 0xFF193a61
#define C_YELLOW_BACK 0xFFe08c02

#define C_RED             C_YELLOW_BACK//0xffff0000
#define C_GREEN           0xff00ff00
#define C_BLUE            0xff0000ff
#define C_GRAY            0xffbcb9c0//0xDEF8  // for font
#define C_YELLOW          0xffffff00//0xF6E0
#define C_VERMILION       0xffca5f02//0xE560
#define C_BG_GRAY         0xff64666b//0xB18D  // for submenu background
#define C_INDIGO          0xff1784e1//0x8553
#define C_DARK_GRAY       0xff505050//0x98C4
#define C_DARKER_GRAY     0xff222222//0x8842
#define C_TRANS_GRAY      0x00bcb9c0//0xaabcb9c0//0xDEF8  // for font
#define C_DEEP_BLUE       0xff003d7f//0xFF281101
#define C_LIGHT_BLUE      0xff4badf7//0xFF6A3A19

#define C_EPG_BLUE_SH       0xFF011128  //RGBA for EPG
#define C_EPG_BLUE_HL       0xFFD3992E  //RGBA for EPG

#define C_PROGLIST_BRIEF  C_BLACK //0xFFB97B6E
#define C_PROGBAR_BOTTOM  0xFFC6741E
#define C_FONT_GRAY   0xFFb9b9b9
#define C_GOLD            0xFFF8C000//0xff00

#define C_IPTV_PIC_YELLOW  0xFFFBCC00
#define C_IPTV_PIC_BLUE  0xFF223854

#define C_IPTV_SELECT_YELLOW  0xFFFBCC00
#define C_CORNFLOWER_BLUE   0xFF5C99CB
#define C_OSD_FONT       0xFF0000FC
#define C_ROLL_OSD_FONT       0xFF0000FC
#define C_ROLL_OSD_BACKGROUND       0xFF0000F9


enum rect_style_id
{
  RSI_IGNORE = 0,       //because default color is 0 in lib, this color style means will not be drawn
  RSI_TRANSPARENT = 1,
  RSI_MAIN_BG = 2,
  RSI_MAINMENU_SH = 3,
  RSI_SUBMENU_DETAIL_BG = 4,
  RSI_PBACK = 5,
  RSI_COMMON_TITLE = 6,
  RSI_BOX_1 = 7,
  RSI_SCROLL_BAR_BG = 8,
  RSI_SCROLL_BAR_MID = 9,
  RSI_BLACK_FRM = 10,
  RSI_MAIN_BG1,
  RSI_POPUP_BG,
  RSI_ITEM_1_SH,  //15
  RSI_ITEM_1_HL,
  RSI_ITEM_2_SH,
  RSI_ITEM_2_HL,
  RSI_ITEM_4_SH,  //20
  RSI_ITEM_4_HL,
  RSI_ITEM_5_SH,
  RSI_LEFT_RIGHT_SH,
  RSI_LEFT_RIGHT_HL,
  RSI_LEFT_RIGHT_GRAY,  //25
  RSI_ITEM_3_SH,
  RSI_GAME_BOX,
  RSI_BOX_2,
  RSI_PROGRESS_BAR_BG,
  RSI_PROGRESS_BAR_MID_BLUE, //30
  RSI_PROGRESS_BAR_MID_RED,
  RSI_PROGRESS_BAR_MID_YELLOW,
  RSI_PROGRESS_BAR_MID_SKY_BLUE,
  RSI_PROGRESS_BAR_VOLUME_MID,
  RSI_NOTIFY_SH, //35
  RSI_TV,
  RSI_KEYBOARD_BAR_V2,
  RSI_KEYBOARD_BUTTON_SH,
  RSI_KEYBOARD_BUTTON_HL,
 
  RSI_ITEM_6,  //40
  RSI_ITEM_10_SH,

  RSI_ITEM_8_SH,
  RSI_ITEM_8_HL,

  RSI_MOVLIST_BG,
  RSI_ITEM_8_BG,

  RSI_ITEM_11_SH,  //50
  RSI_ITEM_12_SH,
  RSI_ITEM_12_HL,

  RSI_ITEM_12_SH_TRANS,
  RSI_ITEM_12_HL_TRANS,
  RSI_ITEM_13_SH,  //55
  RSI_WINDOW_1,
  RSI_SMALL_LIST_ARROW,
  RSI_SMALL_LIST_ARROW_SELECT,
  RSI_WHITE_FRM,   //60
  RSI_GRAY_FRM,
  
 // RSI_ONMOV_PLAY_BG_HL,
//  RSI_ONMOV_PLAY_BG_SH,

  RSI_ITEM_13_HL,
//  RSI_NETWORK_SELECT_HL,
  RSI_COMMAN_BG,
  RSI_ITEM_11_HL,     //71

  RSI_PLAY_BUTTON_SH,
  RSI_PLAY_BUTTON_HL,

  RSI_MV_POSTER_FRM_SH, //76
  RSI_MV_POSTER_FRM_HL,

  RSI_OTT_FRM_1_SH,   
  RSI_OTT_FRM_1_HL,

  RSI_LIST_ITEM_1,
  RSI_OTT_IM_SEARCH,
  RSI_OTT_PROGRESS_BAR_BG,   
  RSI_OTT_PROGRESS_BAR_MID,
  RSI_OTT_FRM_1_RED_HL,
  RSI_NM_CLASSIC_FRM_HL,
  RSI_NM_CLASSIC_FRM_SH,

  RSI_OTT_KEYBOARD_KEY_SH,
  RSI_OTT_KEYBOARD_KEY_HL,
  RSI_MAIN_LOGO_BG,

  /*------------------wanghm add start--------------------*/
  RSI_YELLOW ,
  RSI_BLUE,
  RSI_MMENU_TV,
  RSI_MMENU_TV_F,
  RSI_MMENU_VOD,
  RSI_MMENU_VOD_F,
  RSI_MMENU_MPLAY,
  RSI_MMENU_MPLAY_F,
  RSI_MMENU_SET,
  RSI_MMENU_SET_F,
  RSI_MAINMENU_BG,
  RSI_LEFT_CONT_BG,
  RSI_OPT_BTN,
  RSI_RIGHT_CONT_BG,
  RSI_TITLE_BAR,
  RSI_OPT_BTN_H,
  RSI_SELECT_F,
  RSI_GRAY,
  RSI_AZ_LIST_F,
  RSI_ROLL_OSD,
  /*-------------------wanghm add end-------------------*/
  /****************************************************/
  /******添加风格，防止上传时冲突************/
  /******************************************************/
  /*-------------------langyy add start---------------------*/
  RSI_MEDIO_LEFT_BG,
  RSI_MEDIO_LEFT_LIST_BG,
  RSI_MEDIO_LEFT_LIST_HL,
  RSI_MEDIO_LEFT_LIST_SH,
  RSI_MEDIO_LEFT_LINE_BG,
  RSI_MEDIO_BTN_SH,
  RSI_MEDIO_BTN_HL,
  RSI_MEDIO_PLAY_BG,
  RSI_VOD_LIST_BG,
  RSI_VOD_LIST_N,
  RSI_VOD_LIST_F,
  RSI_VOD_LIST_S,
  RSI_VOD_LIST2_N,
  RSI_VOD_LIST2_F,
  RSI_VOD_LIST2_S,
  RSI_VOD_LEFT,
  RSI_VOD_RIGHT,
  RSI_VOD_LINE,
  RSI_VOD_VDOLIST_HL,
  RSI_BOOT_PAGE3_BOX_SH,
  RSI_BOOT_PAGE3_BOX_HL,
  RSI_KEY_BUTTON_BG,
  RSI_KEY_BUTTON_F_BG,
  /*-------------------langyy add end----------------------*/

  
  //not used, for compile successfully
  RSI_LIST_FIELD_LEFT_HL ,
  RSI_LIST_FIELD_LEFT_SH,
  RSI_LIST_FIELD_MID_HL,
  RSI_LIST_FIELD_MID_SH,
  RSI_LIST_FIELD_RIGHT_HL ,
  RSI_LIST_FIELD_RIGHT_SH ,
  
  
  RSTYLE_CNT,
};

enum font_style_id
{
  FSI_BLACK = 0,        //black string
  FSI_BLACK_28,
  FSI_BLACK_30,
  FSI_WHITE,
  FSI_RED,
  FSI_GREEN,
  FSI_BLUE,             //blue string
  FSI_GRAY,
  FSI_GRAY_26,
  FSI_GRAY_28,
  FSI_GRAY_20,
  FSI_GRAY_18,
  FSI_DARKER_GRAY,
  FSI_VERMILION,
  FSI_INDIGO,
  FSI_PROGLIST_BRIEF,
  FSI_PROGBAR_BOTTOM,

  FSI_NUMBER,
  FSI_BLACK_LARGE,
  FSI_WHITE_LARGE,
  FSI_GRAY_LARGE,

  FSI_HUGE,
  FSI_WHITE_16,
  FSI_WHITE_18,
  FSI_WHITE_36,
  FSI_WHITE_20,
  FSI_WHITE_28,
  FSI_WHITE_30,
  FSI_BLACK_20,
  FSI_BLACK_16,
  FSI_YELLOW_32,
  FSI_RED_32,
  
  FSI_WHITE_22,
  FSI_YELLOW_26,
  FSI_FONT_GRAY,
  FSI_BLUE_NEWS,
  FSI_WHITE_INFO,
  FSI_WHITE_S,
  FSI_CORNFLWR_BLUE_24,
  FSI_YELLOW_18,
  FSI_CORNFLWR_BLUE_16,
  FSI_BLACK_18,
  FSI_WHITE_26,
  FSI_WHITE_45,
  FSI_WHITE_24,
  FSI_WHITE_56,
  FSI_ROLL_OSD,
  FSI_OSD1,
  FSI_OSD2,
  FSI_OSD3,  
  FSI_OSD4,
  FSI_OSD5,
  FSI_OSD6,  
  FSI_OSD7,  
  FSI_OSD8,
  FSI_OSD9,  
  FSI_OSD10,  
  FSI_OSD11,
  FSI_OSD12,  
  FSI_OSD13,
  FSI_ROLL_OSD1,
  FSI_ROLL_OSD2,
  FSI_ROLL_OSD3,  
  FSI_ROLL_OSD4,
  FSI_ROLL_OSD5,
  FSI_ROLL_OSD6,  
  FSI_ROLL_OSD7,  
  FSI_ROLL_OSD8,
  FSI_ROLL_OSD9,  
  FSI_ROLL_OSD10,  
  FSI_ROLL_OSD11,
  FSI_ROLL_OSD12,  
  FSI_ROLL_OSD13,
  
  FSTYLE_CNT,
};

enum font_lib_id
{
  FONT_ID_1 = 1,
  
};

#define MAX_RSTYLE_CNT        200
#define MAX_FSTYLE_CNT        80//30

extern rsc_config_t g_rsc_config_music;
extern rsc_config_t g_rsc_config;

#endif
