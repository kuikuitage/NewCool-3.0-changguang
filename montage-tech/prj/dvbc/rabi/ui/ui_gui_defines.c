/****************************************************************************

****************************************************************************/
#include "ui_common.h"

rsc_rstyle_t c_tab[MAX_RSTYLE_CNT] =
{
  // RSI_IGNORE = 0
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_TRANSPARENT = 1
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_RECT_TO_BORDER , C_TRANS}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_MAIN_BG = 2
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_RECT_TO_BORDER, C_BLACK}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_MAINMENU_SH =3
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_RECT_TO_BORDER, C_BLACK}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_SUBMENU_DETAIL_BG 4
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_RECT_TO_BORDER, C_BLUE_BACK}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_PBACK 5
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_COPY_BG/*R_FILL_RECT_TO_BORDER, C_BLACK*/}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },
  //{ {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_RECT_TO_BORDER, C_BLACK}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },
  // RSI_COMMON_TITLE 6
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_INPAGE_BANNER_BG_2}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_BOX_1 7
  { {R_ICON,IM_BOX_1_ML}, {R_ICON,IM_BOX_1_MT}, {R_ICON,IM_BOX_1_MR}, {R_ICON,IM_BOX_1_MD}, {R_FILL_ICON_TO_VERTEX,IM_BOX_1_MM}, {R_ICON,IM_BOX_1_LT}, {R_ICON,IM_BOX_1_RT}, {R_ICON,IM_BOX_1_RD}, {R_ICON,IM_BOX_1_LD}},

  // RSI_SCROLL_BAR_BG 8
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_SCROLL_BAR_BG}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_SCROLL_BAR_MID 9
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_SCROLL_BAR}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_BLACK_FRM 10
  {{R_LINE2, C_BLACK}, {R_LINE2, C_BLACK}, {R_LINE2, C_BLACK}, {R_LINE2, C_BLACK}, {R_FILL_RECT_TO_BORDER , C_TRANS}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},

  // RSI_MAIN_BG1 11
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_RECT_TO_BORDER, C_DARKER_GRAY}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_POPUP_BG 12
  { {R_ICON , IM_SET_DL_LM}, {R_ICON , IM_SET_DL_MT}, {R_ICON , IM_SET_DL_RM}, {R_ICON , IM_SET_DL_MB}, {R_FILL_ICON_TO_VERTEX , IM_SET_DL_MM}, {R_ICON , IM_SET_DL_LT}, {R_ICON , IM_SET_DL_RT}, {R_ICON , IM_SET_DL_RB}, {R_ICON , IM_SET_DL_LB} },

  // RSI_ITEM_1_SH 15
  { {R_ICON, IM_BUTTON_1_L}, {R_IGNORE}, {R_ICON, IM_BUTTON_1_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_BUTTON_1_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_ITEM_1_HL 16
  { {R_ICON, IM_BUTTON_1_SELECT_L}, {R_IGNORE}, {R_ICON, IM_BUTTON_1_SELECT_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_BUTTON_1_SELECT_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_ITEM_2_SH 17
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_BUTTON_1_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_ITEM_2_HL 18
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_BUTTON_1_SELECT_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_ITEM_4_SH 20
  { {R_ICON, IM_BUTTON_4_L}, {R_IGNORE}, {R_ICON, IM_BUTTON_4_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_BUTTON_4_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_ITEM_4_HL 21
  { {R_ICON, IM_BUTTON_4_SELECT_L}, {R_IGNORE}, {R_ICON, IM_BUTTON_4_SELECT_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_BUTTON_4_SELECT_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_ITEM_5_SH 22
  { {R_ICON, IM_BUTTON_5_L}, {R_IGNORE}, {R_ICON, IM_BUTTON_5_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_BUTTON_5_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_LEFT_RIGHT_SH 23
  { {R_ICON, IM_ARROW2_L}, {R_IGNORE}, {R_ICON, IM_ARROW2_R}, {R_IGNORE}, {R_COPY_BG}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_LEFT_RIGHT_HL 24
  { {R_ICON, IM_ARROW_L_SELECT}, {R_IGNORE}, {R_ICON, IM_ARROW_R_SELECT}, {R_IGNORE}, {R_COPY_BG}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_LEFT_RIGHT_GRAY 25
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_COPY_BG}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_ITEM_3_SH 26
  { {R_ICON, IM_BUTTON_2_L}, {R_IGNORE}, {R_ICON, IM_BUTTON_2_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_BUTTON_2_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_GAME_BOX 27
  { {R_ICON,IM_BOX_1_ML}, {R_ICON,IM_BOX_1_MT}, {R_ICON,IM_BOX_1_MR}, {R_ICON,IM_BOX_1_MD}, {R_FILL_ICON_TO_VERTEX,IM_BOX_2_MM}, {R_ICON,IM_BOX_1_LT}, {R_ICON,IM_BOX_1_RT}, {R_ICON,IM_BOX_1_RD}, {R_ICON,IM_BOX_1_LD}},

  // RSI_BOX_2 28
  { {R_ICON,IM_BOX_2_ML}, {R_ICON,IM_BOX_2_MT}, {R_ICON,IM_BOX_2_MR}, {R_ICON,IM_BOX_2_MD}, {R_FILL_ICON_TO_VERTEX,IM_BOX_2_MM}, {R_ICON,IM_BOX_2_LT}, {R_ICON,IM_BOX_2_RT}, {R_ICON,IM_BOX_2_RD}, {R_ICON,IM_BOX_2_LD}},

  // RSI_PROGRESS_BAR_BG 29
  { {R_ICON, IM_SIGNALBG_L}, {R_IGNORE}, {R_ICON, IM_SIGNALBG_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_SIGNALBG_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_PROGRESS_BAR_MID_BLUE 30
  { {R_ICON, IM_SIGNAL_BLUE_L}, {R_IGNORE}, {R_ICON, IM_SIGNAL_BLUE_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_SIGNAL_BLUE_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_PROGRESS_BAR_MID_RED 31
  { {R_ICON, IM_SIGNAL_RED_L}, {R_IGNORE}, {R_ICON, IM_SIGNAL_RED_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_SIGNAL_RED_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_PROGRESS_BAR_MID_YELLOW 32
  { {R_ICON, IM_SIGNAL_YELLOW_L}, {R_IGNORE}, {R_ICON, IM_SIGNAL_YELLOW_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_SIGNAL_YELLOW_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_PROGRESS_BAR_MID_SKY_BLUE 33
  { {R_ICON, IM_SIGNALSEARCH_BLUE_L}, {R_IGNORE}, {R_ICON, IM_SIGNALSEARCH_BLUE_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_SIGNALSEARCH_BLUE_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_PROGRESS_BAR_VOLUME_MID 34
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_ICON_SOUND_SCROLL}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_NOTIFY_SH 35
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_BUTTON_5_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_TV
  { {R_FILL_ICON_TO_BORDER , IM_TVBOX_M}, {R_FILL_ICON_TO_BORDER , IM_TVBOX_M}, {R_FILL_ICON_TO_BORDER , IM_TVBOX_M}, {R_FILL_ICON_TO_BORDER , IM_TVBOX_M}, {R_FILL_RECT_TO_BORDER , C_TRANS}, {R_ICON ,IM_TVBOX_LT}, {R_ICON , IM_TVBOX_RT}, {R_ICON , IM_TVBOX_RD}, {R_ICON , IM_TVBOX_LD} },

  // RSI_KEYBOARD_BAR_V2
  { {R_ICON, IM_KEYBOARD_INPUTBOX_L}, {R_IGNORE}, {R_ICON, IM_KEYBOARD_INPUTBOX_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_KEYBOARD_INPUTBOX_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  //RSI_KEYBOARD_BUTTON_SH
  { {R_ICON , IM_KEYBOARD_NUMBUTTOM_L}, {R_IGNORE}, {R_ICON , IM_KEYBOARD_NUMBUTTOM_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_KEYBOARD_NUMBUTTOM_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  //RSI_KEYBOARD_BUTTON_HL
  { {R_ICON , IM_KEYBOARD_NUMBUTTOM_SELECT_L}, {R_IGNORE}, {R_ICON , IM_KEYBOARD_NUMBUTTOM_SELECT_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_KEYBOARD_NUMBUTTOM_SELECT_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },
 //RSI_ITEM_6 40
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_BUTTON_6_BG}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_ITEM_10_SH
  { {R_LINE2,C_GRAY}, {R_LINE2,C_GRAY}, {R_LINE2,C_GRAY}, {R_LINE2,C_GRAY}, {R_COPY_BG}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},

    // RSI_ITEM_8_SH
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_BUTTON_7_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_ITEM_8_HL
  { {R_ICON, IM_BUTTON_7_SELECT_L}, {R_IGNORE}, {R_ICON, IM_BUTTON_7_SELECT_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_BUTTON_7_SELECT_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_MOVLIST_BG
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_RECT_TO_BORDER, C_DARK_GRAY}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_ITEM_8_BG
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_BUTTON_7_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_ITEM_11_SH 50
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_RECT_TO_BORDER , C_DARKER_GRAY}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},
#if 1
  // RSI_ITEM_12_SH
  //{ {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {/*R_FILL_RECT_TO_BORDER, C_BG_GRAY*/R_FILL_RECT_TO_BORDER, C_TRANS}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},
  { {R_ICON, IM_PIC_YOUTUBE_BG}, {R_ICON, IM_PIC_YOUTUBE_BG}, {R_ICON, IM_PIC_YOUTUBE_BG}, {R_ICON, IM_PIC_YOUTUBE_BG}, {/*R_FILL_RECT_TO_BORDER , C_TRANS*//*R_IGNORE*/R_COPY_BG}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},

  //RSI_ITEM_12_HL = 100
  { {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_COPY_BG}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},

  //RSI_ITEM_12_SH_TRANS  = 101
  { {R_ICON, IM_PIC_YOUTUBE_BG}, {R_ICON, IM_PIC_YOUTUBE_BG}, {R_ICON, IM_PIC_YOUTUBE_BG}, {R_ICON, IM_PIC_YOUTUBE_BG}, {R_FILL_RECT_TO_BORDER , C_TRANS/*R_IGNORE*/}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},

  //RSI_ITEM_12_HL_TRANS = 102
  { {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_FILL_RECT_TO_BORDER , C_TRANS}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},
#endif
/*  // RSI_ITEM_12_SH
	{ {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_RECT_TO_BORDER, C_BG_GRAY}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},
  
	//RSI_ITEM_12_HL = 100
	{ {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_COPY_BG}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},
  
	//RSI_ITEM_12_SH_TRANS	= 101
	{ {R_ICON, IM_PIC_YOUTUBE_BG}, {R_ICON, IM_PIC_YOUTUBE_BG}, {R_ICON, IM_PIC_YOUTUBE_BG}, {R_ICON, IM_PIC_YOUTUBE_BG}, {R_FILL_RECT_TO_BORDER , C_TRANS}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},
  
	//RSI_ITEM_12_HL_TRANS = 102
	{ {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON, IM_PIC_YOUTUBE_BG_SELECT}, {R_FILL_RECT_TO_BORDER , C_TRANS}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},
*/

  // RSI_ITEM_13_SH 55
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_RECT_TO_BORDER , C_TRANS_GRAY}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},

  // RSI_WINDOW_1 = 2
  { {R_ICON , IM_MMENUWIN01_L}, {R_IGNORE}, {R_ICON , IM_MMENUWIN01_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_MMENUWIN01_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  //RSI_SMALL_LIST_ARROW = 56,
  { {R_ICON | R_CENTER, IM_SLISTARROW01_L}, {R_IGNORE}, {R_ICON | R_CENTER , IM_SLISTARROW01_R}, {R_IGNORE}, {R_COPY_BG}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  //RSI_SMALL_LIST_ARROW_SELECT = 88,
  { {R_ICON | R_CENTER , IM_SLISTARROW02_L}, {R_IGNORE}, {R_ICON | R_CENTER, IM_SLISTARROW02_R}, {R_IGNORE}, {R_COPY_BG}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  // RSI_WHITE_FRM = 35 60
  {{R_LINE2, C_WHITE}, {R_LINE2, C_WHITE}, {R_LINE2, C_WHITE}, {R_LINE2, C_WHITE}, {R_FILL_RECT_TO_BORDER , C_TRANS}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},

  // RSI_GRAY_FRM = 35 60
  {{R_LINE2, C_WHITE}, {R_LINE2, C_WHITE}, {R_LINE2, C_WHITE}, {R_LINE2, C_WHITE}, {R_FILL_RECT_TO_BORDER , C_DARKER_GRAY}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},
  
  // RSI_ITEM_13_HL
  { {R_ICON , IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON , IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON , IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON , IM_PIC_YOUTUBE_BG_SELECT}, {R_FILL_RECT_TO_BORDER , C_BLACK}, {R_ICON ,IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON , IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON , IM_PIC_YOUTUBE_BG_SELECT}, {R_ICON , IM_PIC_YOUTUBE_BG_SELECT} },

  // RSI_NETWORK_SELECT_HL = 97
 // { {R_ICON | R_CENTER, IM_ICON_NETWORK_ARROW_SELECT_L}, {R_IGNORE}, {R_ICON | R_CENTER, IM_ICON_NETWORK_ARROW_SELECT_R}, {R_IGNORE}, {R_COPY_BG}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

  //RSI_COMMAN_BG 
  { {R_ICON , IM_COMM_MID_LEFT}, {R_ICON , IM_COMM_TOP_MID}, {R_ICON , IM_COMM_MID_RIGHT}, {R_ICON , IM_COMM_BOTTOM_MID}, {R_FILL_ICON_TO_VERTEX , IM_COMM_CENTER}, {R_ICON , IM_COMM_TOP_LEFT}, {R_ICON , IM_COMM_TOP_RIGHT}, {R_ICON , IM_COMM_BOTTOM_RIGHT}, {R_ICON , IM_COMM_BOTTOM_LEFT} },

  //RSI_ITEM_11_HL = 93  70
  //{ {R_LINE4, C_GOLD}, {R_LINE4, C_GOLD}, {R_LINE4, C_GOLD}, {R_LINE4, C_GOLD}, {R_FILL_RECT_TO_BORDER , C_TRANS}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},
  { {R_LINE4, C_RED}, {R_LINE4, C_RED}, {R_LINE4, C_RED}, {R_LINE4, C_RED}, {R_FILL_RECT_TO_BORDER , C_DARKER_GRAY}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}},

  //RSI_PLAY_BUTTON_SH,
  { 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_ICON_TO_VERTEX, IM_COLORBUTTON_BLUE_SELECT},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },
 
  //RSI_PLAY_BUTTON_HL, 74
  { 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_ICON_TO_VERTEX, IM_COLORBUTTON_RED_SELECT},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },
  //RSI_MV_POSTER_FRM_SH, 75
  { 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_DARKER_GRAY},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },
  //RSI_MV_POSTER_FRM_HL,
  { 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_RED},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

	// RSI_OTT_FRM_1_SH
	{
	  {R_ICON, IM_BG_NORMAL_1},
	  {R_ICON, IM_BG_NORMAL_1},
	  {R_ICON, IM_BG_NORMAL_1},
	  {R_ICON, IM_BG_NORMAL_1},
	  {R_FILL_RECT_TO_BORDER , C_TRANS},
	  {R_IGNORE},
	  {R_IGNORE},
	  {R_IGNORE},
	  {R_IGNORE}
	},
	// RSI_OTT_FRM_1_HL
	{
	  {R_ICON, IM_BG_FOCUS_1},
	  {R_ICON, IM_BG_FOCUS_1},
	  {R_ICON, IM_BG_FOCUS_1},
	  {R_ICON, IM_BG_FOCUS_1},
	  {R_FILL_RECT_TO_BORDER , C_TRANS},
	  {R_IGNORE},
	  {R_IGNORE},
	  {R_IGNORE},
	  {R_IGNORE}
	},
	
	// RSI_LIST_ITEM_1
	{ 
		{R_IGNORE},
		{R_IGNORE},
		{R_IGNORE},
		{R_IGNORE},
		{R_FILL_ICON_TO_BORDER, IM_SYSTEM_INFOR_BG_M}, 
		{R_IGNORE},
		{R_IGNORE},
		{R_IGNORE},
		{R_IGNORE},
	},
	
	// RSI_OTT_IM_SEARCH
	{ 
		{R_ICON, IM_SEARCHBOX_L}, 
		{R_IGNORE}, 
	    {R_ICON, IM_SEARCHBOX_R}, 
        {R_IGNORE}, 
		{R_FILL_ICON_TO_BORDER, IM_SEARCHBOX_M}, 
		{R_IGNORE}, 
		{R_IGNORE}, 
		{R_IGNORE}, 
		{R_IGNORE} 
	},
	
	// RSI_OTT_PROGRESS_BAR_BG
	{
	    {R_ICON, IM_PROGRESS_BG_L},
	    {R_IGNORE},
		{R_ICON, IM_PROGRESS_BG_R}, 
		{R_IGNORE}, 
		{R_FILL_ICON_TO_BORDER , IM_PROGRESS_BG_M},
		{R_IGNORE}, 
		{R_IGNORE}, 
		{R_IGNORE}, 
		{R_IGNORE} 
	},
	// RSI_OTT_PROGRESS_BAR_MID
	{ 
		{R_ICON, IM_PROGRESS_L}, 
		{R_IGNORE},
		{R_ICON, IM_PROGRESS_R1},
		{R_IGNORE},
		{R_FILL_ICON_TO_BORDER , IM_PROGRESS_M},
		{R_IGNORE},
		{R_IGNORE},	  
		{R_IGNORE}, 
		{R_IGNORE} 
	},
		// RSI_OTT_FRM_1_RED_HL
	{
	  {R_LINE5, C_RED},
	  {R_LINE5, C_RED},
	  {R_LINE5, C_RED},
	  {R_LINE5, C_RED},
	  {R_FILL_RECT_TO_BORDER , C_TRANS},
	  {R_IGNORE},
	  {R_IGNORE},
	  {R_IGNORE},
	  {R_IGNORE}
	},

  	  //RSI_NM_CLASSIC_FRM_HL
	{
	  {R_ICON, IM_BG_FOCUS_1},
	  {R_ICON, IM_BG_FOCUS_1},
	  {R_ICON, IM_BG_FOCUS_1},
	  {R_ICON, IM_BG_FOCUS_2},
	  {R_FILL_RECT_TO_BORDER , C_TRANS},
	  {R_ICON, IM_BG_FOCUS_1},
	  {R_ICON, IM_BG_FOCUS_1},
	  {R_ICON, IM_BG_FOCUS_2},
	  {R_ICON, IM_BG_FOCUS_2},
	},
   //RSI_NM_CLASSIC_FRM_SH
	{
	  {R_ICON, IM_BG_NORMAL_1},
	  {R_ICON, IM_BG_NORMAL_1},
	  {R_ICON, IM_BG_NORMAL_1},
	  {R_ICON, IM_BG_NORMAL_2},
	  {R_FILL_RECT_TO_BORDER , C_TRANS},
	  {R_ICON, IM_BG_NORMAL_1},
	  {R_ICON, IM_BG_NORMAL_1},
	  {R_ICON, IM_BG_NORMAL_2},
	  {R_ICON, IM_BG_NORMAL_2},
	},
    // RSI_OTT_KEYBOARD_KEY_SH
    {
      {R_ICON, IM_KEYBOARD_1_L}, 
      {R_IGNORE},
      {R_ICON, IM_KEYBOARD_1_R}, 
      {R_IGNORE},
      {R_FILL_ICON_TO_BORDER , IM_KEYBOARD_1_M},
      {R_IGNORE},
      {R_IGNORE},
      {R_IGNORE},
      {R_IGNORE}
    },
    // RSI_OTT_KEYBOARD_KEY_HL
    {
      {R_ICON, IM_KEYBOARD_2_L}, 
      {R_IGNORE},
      {R_ICON, IM_KEYBOARD_2_R}, 
      {R_IGNORE},
      {R_FILL_ICON_TO_BORDER , IM_KEYBOARD_2_M},
      {R_IGNORE},
      {R_IGNORE},
      {R_IGNORE},
      {R_IGNORE}
    },
    // RSI_MAIN_LOGO_BG
    {
        {R_IGNORE}, 
        {R_IGNORE}, 
        {R_IGNORE}, 
        {R_IGNORE},
        {R_FILL_RECT_TO_BORDER, C_MAIN_LOGO_BG}, 
        {R_IGNORE}, 
        {R_IGNORE},
        {R_IGNORE}, 
        {R_IGNORE}
    },
    
	/*------------------wanghm add start--------------------*/
	//RSI_YELLOW 
	{
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_FILL_RECT_TO_BORDER , C_YELLOW}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}
	},

	//RSI_BLUE
	{
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_FILL_RECT_TO_BORDER , C_BLUE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}
	},

	
	//RSI_MMENU_TV
    {
        {R_IGNORE}, {R_IGNORE}, 
        {R_IGNORE}, {R_IGNORE},
        {R_ICON, IM_XBTN_TV}, {R_IGNORE}, 
        {R_IGNORE},{R_IGNORE}, 
        {R_IGNORE}
    },
	//RSI_MMENU_TV_F
    {
        {R_IGNORE}, {R_IGNORE}, 
        {R_IGNORE}, {R_IGNORE},
        {R_ICON, IM_XBTN_TV_F}, {R_IGNORE}, 
        {R_IGNORE},{R_IGNORE}, 
        {R_IGNORE}
    },
	//RSI_MMENU_VOD
    {
        {R_IGNORE}, {R_IGNORE}, 
        {R_IGNORE}, {R_IGNORE},
        {R_ICON, IM_XBTN_VOD}, {R_IGNORE}, 
        {R_IGNORE},{R_IGNORE}, 
        {R_IGNORE}
    },
	//RSI_MMENU_VOD_F
    {
        {R_IGNORE}, {R_IGNORE}, 
        {R_IGNORE}, {R_IGNORE},
        {R_ICON, IM_XBTN_VOD_F}, {R_IGNORE}, 
        {R_IGNORE},{R_IGNORE}, 
        {R_IGNORE}
    },
	//RSI_MMENU_MPLAY
    {
        {R_IGNORE}, {R_IGNORE}, 
        {R_IGNORE}, {R_IGNORE},
        {R_ICON, IM_XBTN_PLAY}, {R_IGNORE}, 
        {R_IGNORE},{R_IGNORE}, 
        {R_IGNORE}
    },
	//RSI_MMENU_MPLAY_F
    {
        {R_IGNORE}, {R_IGNORE}, 
        {R_IGNORE}, {R_IGNORE},
        {R_ICON, IM_XBTN_PLAY_F}, {R_IGNORE}, 
        {R_IGNORE},{R_IGNORE}, 
        {R_IGNORE}
    },
	//RSI_MMENU_SET
    {
        {R_IGNORE}, {R_IGNORE}, 
        {R_IGNORE}, {R_IGNORE},
        {R_ICON, IM_XBTN_SET}, {R_IGNORE}, 
        {R_IGNORE},{R_IGNORE}, 
        {R_IGNORE}
    },
	//RSI_MMENU_SET_F
    {
        {R_IGNORE}, {R_IGNORE}, 
        {R_IGNORE}, {R_IGNORE},
        {R_ICON, IM_XBTN_SET_F}, {R_IGNORE}, 
        {R_IGNORE},{R_IGNORE}, 
        {R_IGNORE}
    },
    //RSI_MAINMENU_BG
    {
        {R_IGNORE}, {R_IGNORE}, 
        {R_IGNORE}, {R_IGNORE},
        {R_ICON, IM_XMAIN_BG}, {R_IGNORE}, 
        {R_IGNORE},{R_IGNORE}, 
        {R_IGNORE}
    },
	//RSI_LEFT_CONT_BG
	{
        {R_IGNORE}, {R_IGNORE}, 
        {R_IGNORE}, {R_IGNORE},
        {R_FILL_ICON_TO_BORDER, IM_SET_BG_SIMPLE}, {R_IGNORE}, 
        {R_IGNORE},{R_IGNORE}, 
        {R_IGNORE}
    },
	//RSI_OPT_BTN
	{
        {R_IGNORE}, {R_IGNORE}, 
        {R_IGNORE}, {R_IGNORE},
        {R_FILL_ICON_TO_VERTEX , IM_SET_BG_SIMPLE}, {R_IGNORE}, 
        {R_IGNORE},{R_IGNORE}, 
        {R_IGNORE}
    },
	//RSI_RIGHT_CONT_BG
	{
        {R_IGNORE}, {R_IGNORE}, 
        {R_IGNORE}, {R_IGNORE},
        {R_FILL_ICON_TO_BORDER, IM_SET_BG_DARK}, {R_IGNORE}, 
        {R_IGNORE},{R_IGNORE}, 
        {R_IGNORE}
    },
    //RSI_TITLE_BAR
    {
        {R_IGNORE}, {R_IGNORE}, 
        {R_IGNORE}, {R_IGNORE},
        {R_FILL_ICON_TO_VERTEX, IM_SET_LONG_LINE}, {R_IGNORE}, 
        {R_IGNORE},{R_IGNORE}, 
        {R_IGNORE}
    },
    //RSI_OPT_BTN_H
    {
		{R_ICON, IM_SET_BTN_LEFT_F_LM}, {R_ICON, IM_SET_BTN_LEFT_F_MT}, 
		{R_ICON, IM_SET_BTN_LEFT_F_RM}, {R_ICON, IM_SET_BTN_LEFT_F_MB}, 
		{R_FILL_ICON_TO_VERTEX, IM_SET_BTN_LEFT_F_MM}, {R_ICON, IM_SET_BTN_LEFT_F_LT}, 
		{R_ICON, IM_SET_BTN_LEFT_F_RT}, {R_ICON, IM_SET_BTN_LEFT_F_RB}, 
		{R_ICON, IM_SET_BTN_LEFT_F_LB}
    },
	//RSI_SELECT_F
	{
		{R_ICON, IM_SET_BTN_RIGHT_F_LM}, {R_ICON, IM_SET_BTN_RIGHT_F_MT}, 
		{R_ICON, IM_SET_BTN_RIGHT_F_RM}, {R_ICON, IM_SET_BTN_RIGHT_F_MB}, 
		{R_FILL_ICON_TO_VERTEX, IM_SET_BTN_RIGHT_F_MM}, {R_ICON, IM_SET_BTN_RIGHT_F_LT}, 
		{R_ICON, IM_SET_BTN_RIGHT_F_RT}, {R_ICON, IM_SET_BTN_RIGHT_F_RB}, 
		{R_ICON, IM_SET_BTN_RIGHT_F_LB}
    },
	// RSI_GRAY
	{ 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_FILL_RECT_TO_BORDER, C_GRAY}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
	},
	//RSI_AZ_LIST_F
	{
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
	},
	// RSI_ROLL_OSD
	{ 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_FILL_RECT_TO_BORDER, C_ROLL_OSD_BACKGROUND}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}
	},
	/*-------------------wanghm add end-------------------*/
	/****************************************************/
	/******添加风格，防止上传时冲突************/
	/******************************************************/
	/*-------------------langyy add start---------------------*/

  //RSI_MEDIO_LEFT_BG
  { 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_ICON, IM_XMEDIO_LEFT_BG}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
   },
  //RSI_MEDIO_LEFT_LIST_BG
    { 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_ICON, IM_XMEDIO_LEFT_LIST_BG}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
   },
  //RSI_MEDIO_LEFT_LIST_HL
    { 
        {R_ICON, IM_XMEDIO_LIST_BLUE_LM}, {R_ICON, IM_XMEDIO_LIST_BLUE_MT}, 
		{R_ICON, IM_XMEDIO_LIST_BLUE_RM}, {R_ICON, IM_XMEDIO_LIST_BLUE_MB}, 
		{R_FILL_ICON_TO_VERTEX, IM_XMEDIO_LIST_BLUE_MM}, {R_ICON, IM_XMEDIO_LIST_BLUE_LT}, 
		{R_ICON, IM_XMEDIO_LIST_BLUE_RT}, {R_ICON, IM_XMEDIO_LIST_BLUE_RB}, 
		{R_ICON, IM_XMEDIO_LIST_BLUE_LB}
   },
  //RSI_MEDIO_LEFT_LIST_SH
      { 
        {R_ICON, IM_XMEDIO_LIST_YW_LM}, {R_ICON, IM_XMEDIO_LIST_YW_MT}, 
		{R_ICON, IM_XMEDIO_LIST_YW_RM}, {R_ICON, IM_XMEDIO_LIST_YW_MB}, 
		{R_FILL_ICON_TO_VERTEX, IM_XMEDIO_LIST_YW_MM}, {R_ICON, IM_XMEDIO_LIST_YW_LT}, 
		{R_ICON, IM_XMEDIO_LIST_YW_RT}, {R_ICON, IM_XMEDIO_LIST_YW_RB}, 
		{R_ICON, IM_XMEDIO_LIST_YW_LB}
   },
  //RSI_MEDIO_LEFT_LINE_BG
      { 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_ICON, IM_XMEDIO_LEFT_DH_BG}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
   },
  //RSI_MEDIO_BTN_SH
      { 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_ICON, IM_XMEDIO_LEFT_BTN}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
   },
  //RSI_MEDIO_BTN_HL
        { 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_ICON, IM_XMEDIO_LEFT_BTN_F}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
   },
 // RSI_MEDIO_PLAY_BG
    { 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_ICON, IM_XPLAY_BG}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
   },
 // RSI_VOD_LIST_BG
     { 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_ICON, IM_VOD_TITLE_1_M}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
   },
 // RSI_VOD_LIST_N
       { 
        {R_ICON, IM_VOD_TITLE_1_M}, {R_IGNORE}, 
		{R_ICON, IM_VOD_TITLE_1_R}, {R_IGNORE}, 
		{R_FILL_ICON_TO_VERTEX, IM_VOD_TITLE_1_M}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}
   },

 // RSI_VOD_LIST_F
        { 
        {R_ICON, IM_VOD_TITLE_1_L}, {R_IGNORE}, 
		{R_ICON, IM_VOD_TITLE_1_R_F}, {R_IGNORE}, 
		{R_FILL_ICON_TO_VERTEX, IM_VOD_TITLE_1_M_F}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}
   },
 // RSI_VOD_LIST_S
       { 
        {R_ICON, IM_VOD_TITLE_1_L_S}, {R_IGNORE}, 
		{R_ICON, IM_VOD_TITLE_1_R_S}, {R_IGNORE}, 
		{R_FILL_ICON_TO_VERTEX, IM_VOD_TITLE_1_M_S}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}
   },
 // RSI_VOD_LIST2_N
        { 
        {R_ICON, IM_VOD_TITLE_M}, {R_IGNORE}, 
		{R_ICON, IM_VOD_TITLE_R}, {R_IGNORE}, 
		{R_FILL_ICON_TO_VERTEX, IM_VOD_TITLE_M}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}
   },
   //  RSI_VOD_LIST2_F
          { 
        {R_ICON, IM_VOD_TITLE_L}, {R_IGNORE}, 
		{R_ICON, IM_VOD_TITLE_R_F}, {R_IGNORE}, 
		{R_FILL_ICON_TO_VERTEX, IM_VOD_TITLE_M_F}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}
   },
  //RSI_VOD_LIST2_S
         { 
        {R_ICON, IM_VOD_TITLE_L_S}, {R_IGNORE}, 
		{R_ICON, IM_VOD_TITLE_R_S}, {R_IGNORE}, 
		{R_FILL_ICON_TO_VERTEX, IM_VOD_TITLE_M_S}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}
   },
 // RSI_VOD_LEFT
      { 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_ICON, IM_VOD_LEFT}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
   },
 // RSI_VOD_RIGHT
      { 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_ICON, IM_VOD_RIGHT}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
   },
 // RSI_VOD_LINE
 { 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_ICON, IM_VOD_LINE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
   },
   //RSI_VOD_VDOLIST_HL
    {
           {R_ICON, IM_XMEDIO_LIST_BLUE_LM}, {R_ICON, IM_XMEDIO_LIST_BLUE_MT}, 
		{R_ICON, IM_XMEDIO_LIST_BLUE_RM}, {R_ICON, IM_XMEDIO_LIST_BLUE_MB}, 
		{R_FILL_RECT_TO_BORDER , C_TRANS}, {R_ICON, IM_XMEDIO_LIST_BLUE_LT}, 
		{R_ICON, IM_XMEDIO_LIST_BLUE_RT}, {R_ICON, IM_XMEDIO_LIST_BLUE_RB}, 
		{R_ICON, IM_XMEDIO_LIST_BLUE_LB}
    },
    //RSI_BOOT_PAGE3_BOX_SH,
         { 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_ICON, IM_OPENDVB_CTRLBOX_SH}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
   },
    //RSI_BOOT_PAGE3_BOX_HL,
             { 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_ICON, IM_OPENDVB_CTRLBOX_HL}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
   },
//      RSI_KEY_BUTTON_BG
{ 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_ICON, IM_KEY_BUTTON}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
   },
//  RSI_KEY_BUTTON_F_BG
{ 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_ICON, IM_KEY_BUTTON_F}, {R_IGNORE}, 
		{R_IGNORE}, {R_IGNORE}, 
		{R_IGNORE} 
   },
	/*-------------------langyy add end----------------------*/
	
};

rsc_fstyle_t f_tab[MAX_FSTYLE_CNT] =
{
  //FSI_BLACK
  { FONT_ID_1, C_BLACK, 0, 24, 24},
  //FSI_BLACK_28
  { FONT_ID_1, C_BLACK, 0, 28, 28},
  //FSI_BLACK_30
  { FONT_ID_1, C_BLACK, 0, 30, 30},
  //FSI_WHITE
  { FONT_ID_1, C_WHITE, 0, 24, 24},
  //FSI_RED
  { FONT_ID_1, C_RED, 0, 24, 24},
  //FSI_GREEN
  { FONT_ID_1, C_GREEN, 0, 24, 24},
  //FSI_BLUE
  { FONT_ID_1, C_BLUE, 0, 24, 24},
  //FSI_GRAY
  { FONT_ID_1, C_GRAY, 0, 24, 24},
  //FSI_GRAY_26
  { FONT_ID_1, C_GRAY, 0, 26, 26},
  //FSI_GRAY_28
  { FONT_ID_1, C_GRAY, 0, 28, 28},
  //FSI_GRAY_20
  { FONT_ID_1, C_GRAY, 0, 20, 20},
  //FSI_GRAY_18
  { FONT_ID_1, C_GRAY, 0, 18, 18},
  //FSI_DARKER_GRAY
  { FONT_ID_1, C_DARKER_GRAY, 0, 24, 24},
  //FSI_VERMILION
  { FONT_ID_1, C_RED, 0, 24, 24},
  //FSI_INDIGO
  { FONT_ID_1, C_INDIGO, 0, 24, 24},
  //FSI_PROGLIST_BRIEF
  { FONT_ID_1, C_WHITE, 0, 24, 24},
  //FSI_PROGBAR_BOTTOM
  { FONT_ID_1, C_PROGBAR_BOTTOM, 0, 24, 24},

  //FSI_NUMBER
  { FONT_ID_1, C_WHITE, 0, 24, 24},

  //FSI_BLACK_LARGE
  { FONT_ID_1, C_BLACK, 0, 24, 24},

  //FSI_WHITE_LARGE
  { FONT_ID_1, C_WHITE, 0, 24, 24},

  //FSI_GRAY_LARGE
  { FONT_ID_1, C_GRAY, 0, 24, 24},

  //FSI_HUGE
  { FONT_ID_1, C_YELLOW, 0, 72, 72},
  //FSI_WHITE_16 = 17,
  { FONT_ID_1, C_WHITE, 0, 16, 16},
  //FSI_WHITE_18 = 18,
  { FONT_ID_1, C_WHITE, 0, 18, 18},
  //FSI_WHITE_36 = 19,
  { FONT_ID_1, C_WHITE, 0, 36, 36},
  //FSI_WHITE_20,
  { FONT_ID_1, C_WHITE, 0, 20, 20},
  //FSI_WHITE_28 = 10
  { FONT_ID_1, C_WHITE, 0, 28, 28},
  //FSI_WHITE_30 = 10
  { FONT_ID_1, C_WHITE, 0, 30, 30},
  //FSI_BLACK_20,
  { FONT_ID_1, C_BLACK, 0, 20, 20},
  //FSI_BLACK_16
  { FONT_ID_1, C_BLACK, 0, 16, 16},
  //FSI_YELLOW_32
  { FONT_ID_1, C_YELLOW, 0, 32, 32},
  //FSI_YELLOW_32
  { FONT_ID_1, C_RED, 0, 32, 32},
  //FSI_WHITE_22
  { FONT_ID_1, C_WHITE, 0, 22, 22},  
   //FSI_YELLOW_26
  { FONT_ID_1, C_YELLOW, 0, 26, 26},
  //FSI_FONT_GRAY
  { FONT_ID_1, C_FONT_GRAY , 0, 18, 18},
  //FSI_BLUE_NEWS
  { FONT_ID_1, C_BLUE, 0, 20, 20},  
   //FSI_WHITE_INFO
  { FONT_ID_1, C_WHITE, 0, 20, 20},
  //FSI_WHITE_S
  { FONT_ID_1, C_WHITE, 0, 15, 15,},  
  
  //FSI_CORNFLWR_BLUE_24 = 31
  { FONT_ID_1, C_CORNFLOWER_BLUE, 0, 24, 24},
  
  //FSI_YELLOW_18
  { FONT_ID_1, C_YELLOW, 0, 18, 18},
  
  //FSI_CORNFLWR_BLUE_16
  { FONT_ID_1, C_CORNFLOWER_BLUE, 0, 16, 16},
  
  //FSI_BLACK_18
  { FONT_ID_1, C_BLACK, 0, 18, 18},
  //FSI_WHITE_26
  { FONT_ID_1, C_WHITE, 0, 26, 26}, 
  //FSI_WHITE_45
  { FONT_ID_1, C_WHITE, 0, 45, 45},
  //FSI_WHITE_24
  { FONT_ID_1, C_WHITE, 0, 24, 24},
  //FSI_WHITE_56
  #ifndef SMSX_CA
  { FONT_ID_1, C_WHITE, 0, 56, 56}, 
  #else
  { FONT_ID_1, C_WHITE, 0, 32, 32}, 
  #endif
  //FSI_ROLL_OSD
  { FONT_ARIAL, C_WHITE, 0, 52, 52},
  //FSI_OSD1
  { FONT_ARIAL, C_OSD_FONT, 0, 18, 18},
  //FSI_OSD2
  { FONT_ARIAL, C_OSD_FONT, 0, 20, 20},
  //FSI_OSD3
  { FONT_ARIAL, C_OSD_FONT, 0, 22, 22},
  //FSI_OSD4
  { FONT_ARIAL, C_OSD_FONT, 0, 24, 24},
  //FSI_OSD5
  { FONT_ARIAL, C_OSD_FONT, 0, 26, 26},
  //FSI_OSD6
  { FONT_ARIAL, C_OSD_FONT, 0, 32, 32},
  //FSI_OSD7
  { FONT_ARIAL, C_OSD_FONT, 0, 34, 34},
  //FSI_OSD8
  { FONT_ARIAL, C_OSD_FONT, 0, 28, 28},
  //FSI_OSD9
  { FONT_ARIAL, C_OSD_FONT, 0, 36, 36},
  //FSI_OSD10
  { FONT_ARIAL, C_OSD_FONT, 0, 48, 48},
  //FSI_OSD11
  { FONT_ARIAL, C_OSD_FONT, 0, 8, 8},
  //FSI_OSD12
  { FONT_ARIAL, C_OSD_FONT, 0, 12, 12},
  //FSI_OSD13
  { FONT_ARIAL, C_OSD_FONT, 0, 16, 16},
  //FSI_ROLL_OSD1
  { FONT_ARIAL, C_ROLL_OSD_FONT, 0, 18, 18},
  //FSI_ROLL_OSD2
  { FONT_ARIAL, C_ROLL_OSD_FONT, 0, 20, 20},
  //FSI_ROLL_OSD3
  { FONT_ARIAL, C_ROLL_OSD_FONT, 0, 22, 22},
  //FSI_ROLL_OSD4
  { FONT_ARIAL, C_ROLL_OSD_FONT, 0, 24, 24},
  //FSI_ROLL_OSD5
  { FONT_ARIAL, C_ROLL_OSD_FONT, 0, 26, 26},
  //FSI_ROLL_OSD6
  { FONT_ARIAL, C_ROLL_OSD_FONT, 0, 32, 32},
  //FSI_ROLL_OSD7
  { FONT_ARIAL, C_ROLL_OSD_FONT, 0, 34, 34},
  //FSI_ROLL_OSD8
  { FONT_ARIAL, C_ROLL_OSD_FONT, 0, 28, 28},
  //FSI_ROLL_OSD9
  { FONT_ARIAL, C_ROLL_OSD_FONT, 0, 36, 36},
  //FSI_ROLL_OSD10
  { FONT_ARIAL, C_ROLL_OSD_FONT, 0, 48, 48},
  //FSI_ROLL_OSD11
  { FONT_ARIAL, C_ROLL_OSD_FONT, 0, 8, 8},
  //FSI_ROLL_OSD12
  { FONT_ARIAL, C_ROLL_OSD_FONT, 0, 12, 12},
  //FSI_ROLL_OSD13
  { FONT_ARIAL, C_ROLL_OSD_FONT, 0, 16, 16},
};

rsc_config_t g_rsc_config =
{
  512,         // common data buf size
  ((128 *2) * KBYTES),    //max bitmap size,every pixel cost 2 Byte
  0,           //max language size,every char cast 2 Bytes
  256 * 4,     //palette size,every color cost 2 Bytes
    0,                //must bigger than latin font size
    0,                //script buffer size
    0,                //offset for block
    0,                //flash base addr
  RSTYLE_CNT,
  c_tab,
  FSTYLE_CNT,
  f_tab,
};
rsc_config_t g_rsc_config_music =
{
  512,         // common data buf size
  0,    //max bitmap size,every pixel cost 2 Byte
  0,           //max language size,every char cast 2 Bytes
  256 * 4,     //palette size,every color cost 2 Bytes
    0,                //must bigger than latin font size
    0,                //script buffer size
    0,                //offset for block
    0,                //flash base addr
  RSTYLE_CNT,
  c_tab,
  FSTYLE_CNT,
  f_tab,
};
