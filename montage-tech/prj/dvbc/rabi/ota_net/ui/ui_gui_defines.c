/****************************************************************************

****************************************************************************/
#include "ui_common.h"

rsc_rstyle_t c_tab[MAX_RSTYLE_CNT] =
{
  // RSI_IGNORE
  { 
    {R_IGNORE}, 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  // RSI_TRANSPARENT
  { 
    {R_IGNORE}, 
    {R_IGNORE}, 
    {R_IGNORE}, 
    {R_IGNORE}, 
    {R_FILL_RECT_TO_BORDER, C_TRANS}, 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}, 
    {R_IGNORE}
  },

  //RSI_BLUE
  { 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_BLUE},
    {R_IGNORE},
    {R_IGNORE}, 
    {R_IGNORE},
    {R_IGNORE}
  },

  //RSI_YELLOW
  { 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_YELLOW},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  //RSI_WHITE
  { 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_WHITE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  //RSI_BLACK
  { 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_BLACK},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  //RSI_DLG_FRM
  { 
    {R_LINE2, C_WHITE}, 
    {R_LINE2, C_WHITE}, 
    {R_LINE2, C_WHITE}, 
    {R_LINE2, C_WHITE}, 
    {R_FILL_RECT_TO_BORDER, C_BLUE}, 
    {R_IGNORE}, 
    {R_IGNORE}, 
    {R_IGNORE}, 
    {R_IGNORE}
  },

  //RSI_GRAY
  { 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_RECT_TO_BORDER, C_GRAY},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  // RSI_PBACK
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_COPY_BG}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },

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
  
  // RSI_LIST_ITEM_2
  { 
    {R_ICON, IM_SYSTEM_INPUTBOX_L},
    {R_IGNORE},
    {R_ICON, IM_SYSTEM_INPUTBOX_R},
    {R_IGNORE},
    {R_FILL_ICON_TO_BORDER, IM_SYSTEM_INPUTBOX_M}, 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
  },

  // RSI_ITEM_1_SH
  { {R_ICON, IM_BUTTON_1_L}, {R_IGNORE}, {R_ICON, IM_BUTTON_1_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_BUTTON_1_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },
  // RSI_ITEM_1_HL
  { {R_ICON, IM_BUTTON_1_SELECT_L}, {R_IGNORE}, {R_ICON, IM_BUTTON_1_SELECT_R}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER, IM_BUTTON_1_SELECT_M}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },
  // RSI_BOX2
  { 
    {R_ICON, IM_BOX2_ML},
    {R_ICON, IM_BOX2_MT},
    {R_ICON, IM_BOX2_MR},
    {R_ICON, IM_BOX2_MD}, 
    {R_FILL_ICON_TO_BORDER, IM_BOX2_MM}, 
    {R_ICON, IM_BOX2_LT},
    {R_ICON, IM_BOX2_RT},
    {R_ICON, IM_BOX2_RD},
    {R_ICON, IM_BOX2_LD},
  },


  // RSI_SCROLL_BAR_BG
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_SCROLL_BAR_BG}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },
  // RSI_SCROLL_BAR_MID
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_ICON_TO_BORDER , IM_SCROLL_BAR}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },
  // RSI_SUBMENU_DETAIL_BG
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_RECT_TO_BORDER, C_BG_GRAY}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },
  // RSI_MAIN_BG
  { {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_FILL_RECT_TO_BORDER, C_BLACK}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE}, {R_IGNORE} },
  // RSI_BOX3_TITLE
  { 
    {R_ICON, IM_BOX3_LT},
    {R_IGNORE},
    {R_ICON, IM_BOX3_RT},
    {R_IGNORE},
    {R_FILL_ICON_TO_BORDER, IM_BOX3_MT}, 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
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
  { {R_ICON, IM_PROGRESS_L}, 
    {R_IGNORE},
    {R_ICON, IM_PROGRESS_R1},
    {R_IGNORE},
    {R_FILL_ICON_TO_BORDER , IM_PROGRESS_M},
    {R_IGNORE},
    {R_IGNORE},     
    {R_IGNORE}, 
    {R_IGNORE} 
  },

	// RSI_OTT_IM_BUTTON_SL
  { {R_ICON, IM_BUTTON_A_L}, 
     {R_IGNORE}, 
     {R_ICON, IM_BUTTON_A_R}, 
     {R_IGNORE}, 
     {R_FILL_ICON_TO_BORDER, IM_BUTTON_A_M}, 
     {R_IGNORE}, 
     {R_IGNORE}, 
     {R_IGNORE}, 
     {R_IGNORE} 
  },

  // RSI_BOX3
  { 
    {R_ICON, IM_BOX3_ML},
    {R_ICON, IM_BOX3_MT_WITH_BOTTOM_LINE},
    {R_ICON, IM_BOX3_MR},
    {R_ICON, IM_BOX3_MD}, 
    {R_FILL_ICON_TO_BORDER, IM_BOX3_MM}, 
    {R_ICON, IM_BOX3_LT_WITH_BOTTOM_LINE},
    {R_ICON, IM_BOX3_RT_WITH_BOTTOM_LINE},
    {R_ICON, IM_BOX3_RD},
    {R_ICON, IM_BOX3_LD}, 
  },
  // RSI_OTT_SCROLL_BAR_BG
  { 
    {R_IGNORE},
    {R_ICON, IM_SCROLL_BAR_BG_TOP},
    {R_IGNORE},
    {R_ICON, IM_SCROLL_BAR_BG_BOTTOM},
    {R_FILL_ICON_TO_BORDER, IM_SCROLL_BAR_BG}, 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
  },

  // RSI_OTT_SCROLL_BAR
  { 
    {R_IGNORE},
    {R_ICON, IM_SCROLL_SCROLL_TOP},
    {R_IGNORE},
    {R_ICON, IM_SCROLL_SCROLL_BOTTOM},
    {R_FILL_ICON_TO_BORDER, IM_SCROLL_SCROLL_MIDDLE}, 
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
  },

  // RSI_BOX1
  { 
    {R_ICON, IM_BOX1_ML},
    {R_ICON, IM_BOX1_MT},
    {R_ICON, IM_BOX1_MR},
    {R_ICON, IM_BOX1_MD}, 
    {R_FILL_ICON_TO_BORDER, IM_BOX1_MM}, 
    {R_ICON, IM_BOX1_LT},
    {R_ICON, IM_BOX1_RT},
    {R_ICON, IM_BOX1_RD},
    {R_ICON, IM_BOX1_LD},
  },
		// RSI_OTT_IM_SEARCH
	  { {R_ICON, IM_SEARCHBOX_L}, 
		 {R_IGNORE}, 
		 {R_ICON, IM_SEARCHBOX_R}, 
		 {R_IGNORE}, 
		 {R_FILL_ICON_TO_BORDER, IM_SEARCHBOX_M}, 
		 {R_IGNORE}, 
		 {R_IGNORE}, 
		 {R_IGNORE}, 
		 {R_IGNORE} 
		},
  // RSI_OTT_KEYBOARD_KEY_HL
  {
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_ICON_TO_BORDER , IM_KEYBOARD_2},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  // RSI_OTT_KEYBOARD_KEY_SH
  {
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_FILL_ICON_TO_BORDER , IM_KEYBOARD_1},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE},
    {R_IGNORE}
  },

  // RSI_POPUP_BG
  { {R_ICON , IM_POPUPBOX_SIDE}, {R_ICON , IM_POPUPBOX_SIDE}, {R_ICON , IM_POPUPBOX_SIDE}, {R_ICON , IM_POPUPBOX_SIDE}, {R_FILL_ICON_TO_VERTEX , IM_POPUPBOX_IN}, {R_ICON , IM_POPUPBOX_SIDE}, {R_ICON , IM_POPUPBOX_SIDE}, {R_ICON , IM_POPUPBOX_SIDE}, {R_ICON , IM_POPUPBOX_SIDE} },

};


font_map_t f_map_tab[] =
{
  { FONT_ID_1, FONT_CHINESE, FONT_BUFFER_COMMON, 0, NULL },
  { FONT_ID_1, FONT_ENGLISH, FONT_BUFFER_LOCAL, 0, NULL },
//  { FONT_ID_2, FONT_NUMBER, FONT_BUFFER_LOCAL, 0, NULL },
};

#define FONTMAP_CNT    sizeof(f_map_tab) / sizeof(font_map_t)


rsc_fstyle_t f_tab[MAX_FSTYLE_CNT] =
{
  //FSI_BLACK
  { FONT_ID_1, 0, C_BLACK     },
  //FSI_WHITE
  { FONT_ID_1, 0, C_WHITE     },
  //FSI_GREEN,
  { FONT_ID_1, 0, C_GREEN     },
  //FSI_RED
  { FONT_ID_1, 0, C_RED     },
  //FSI_GRAY,
  { FONT_ID_1, 0, C_GRAY     },
  //FSI_VERMILION,
  { FONT_ID_1, 0, C_VERMILION     },
};


rsc_config_t g_rsc_config =
{
  512,         // common data buf size
  0,//80 * 336,    //max bitmap size,every pixel cost 2 Byte
  0,           //max language size,every char cast 2 Bytes
  256 * 4,     //palette size,every color cost 2 Bytes
  0,           //must bigger than latin font size
  0,           //must bigger than GB2312 font size
  0,           // script buffer size
  36 * 36 * 4,     //cache buffer size,every pixel cost 2 Byte
  10,          //cache buffer length
  0,           //offset for block
  0,           //flash base addr
  RSTYLE_CNT,
  c_tab,
  FSTYLE_CNT,
  f_tab,
  FONTMAP_CNT,
  f_map_tab,
  0,
  TRUE,
};
