/****************************************************************************

****************************************************************************/
#ifndef __UI_BOOK_LIST_H__
#define __UI_BOOK_LIST_H__

/* coordinate */
#define BLIST_CONT_X          0
#define BLIST_CONT_Y          0
#define BLIST_CONT_W          RIGHT_ROOT_W
#define BLIST_CONT_H          RIGHT_ROOT_H

#define BLIST_LIST_X          60
#define BLIST_LIST_Y          40
#define BLIST_LIST_W          (BLIST_CONT_W - 2*BLIST_LIST_X-BLIST_SBAR_WIDTH)
#define BLIST_LIST_H          ((BLIST_ITEM_H + BLIST_ITEM_V_GAP) * BLIST_PAGE)
#define BLIST_PAGE            10
#define BLIST_FIELD_NUM       4

#define BLIST_ITEM_H          50//36
#define BLIST_ITEM_V_GAP      5//0

#define BLIST_MID_X           0
#define BLIST_MID_Y           0
#define BLIST_MID_W          BLIST_LIST_W
#define BLIST_MID_H           BLIST_LIST_H

#define BLIST_SBAR_H_GAP      8
#define BLIST_SBAR_V_GAP      0
#define BLIST_SBAR_VERTEX_GAP         12
#define BLIST_SBAR_WIDTH      6
#define BLIST_SBAR_HEIGHT     BLIST_LIST_H

/* rect style */
#define RSI_BLIST_FRM         RSI_WINDOW_1
#define RSI_BLIST_LIST        RSI_PBACK
#define RSI_BLIST_SBAR_BG     RSI_SCROLL_BAR_BG
#define RSI_BLIST_SBAR_MID    RSI_SCROLL_BAR_MID

/* font style */
/*fstyle*/
#define FSI_BLIST_SH          FSI_WHITE       //font styel of single satellite information edit
#define FSI_BLIST_HL          FSI_BLACK


/*others*/

RET_CODE open_book_list(u32 para1, u32 para2);

#endif


