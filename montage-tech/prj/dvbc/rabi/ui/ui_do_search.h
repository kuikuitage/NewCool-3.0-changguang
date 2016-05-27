/****************************************************************************

****************************************************************************/
#ifndef __UI_DO_SEARCH_H__
#define __UI_DO_SEARCH_H__

/* coordinate */


//prog info container
#define DO_SEARCH_FRM_X                 ((COMM_BG_W - ((DO_SEARCH_TITLE_BG_W + DO_SEARCH_TITLE_AND_TITLE_HGAP) * DO_SEARCH_TITLE_CNT - DO_SEARCH_TITLE_AND_TITLE_HGAP)) >> 1)
#define DO_SEARCH_FRM_Y                 (100)
#define DO_SEARCH_FRM_W                 ((DO_SEARCH_TITLE_BG_W + DO_SEARCH_TITLE_AND_TITLE_HGAP) * DO_SEARCH_TITLE_CNT - DO_SEARCH_TITLE_AND_TITLE_HGAP)
#define DO_SEARCH_FRM_H                 (DO_SEARCH_TITLE_BG_H + DO_SEARCH_LIST_CONT_H + DO_SEARCH_TITLE_AND_LIST_VGAP)

//tv radio title
#define DO_SEARCH_TITLE_CNT             (2)
#define DO_SEARCH_TITLE_AND_TITLE_HGAP  (20)

#define DO_SEARCH_TITLE_BG_X            (0)
#define DO_SEARCH_TITLE_BG_Y            (0)
#define DO_SEARCH_TITLE_BG_W            (510)
#define DO_SEARCH_TITLE_BG_H            (50)

#define DO_SEARCH_TITLE_NAME_X          (0)
#define DO_SEARCH_TITLE_NAME_Y          DO_SEARCH_TITLE_BG_Y
#define DO_SEARCH_TITLE_NAME_W          ((DO_SEARCH_TITLE_BG_W >> 1) - 30)
#define DO_SEARCH_TITLE_NAME_H          DO_SEARCH_TITLE_BG_H

#define DO_SEARCH_TITLE_NUM_X           ((DO_SEARCH_TITLE_BG_W >> 1) + 30)
#define DO_SEARCH_TITLE_NUM_Y           DO_SEARCH_TITLE_BG_Y
#define DO_SEARCH_TITLE_NUM_W           DO_SEARCH_TITLE_NAME_W
#define DO_SEARCH_TITLE_NUM_H           DO_SEARCH_TITLE_BG_H

#define DO_SEARCH_TITLE_AND_LIST_VGAP   (10)

//search list
#define DO_SEARCH_LIST_CNT              (2)
#define DO_SEARCH_LIST_PAGE             (7)

#define DO_SEARCH_LIST_FIELD_CNT        (1)
#define DO_SEARCH_LIST_ITEM_H           (36)
#define DO_SEARCH_LIST_ITEM_V_GAP       (0)

#define DO_SEARCH_LIST_CONT_X           (0)
#define DO_SEARCH_LIST_CONT_Y           (DO_SEARCH_TITLE_BG_H + DO_SEARCH_TITLE_AND_LIST_VGAP)
#define DO_SEARCH_LIST_CONT_W           (DO_SEARCH_TITLE_BG_W)
#define DO_SEARCH_LIST_CONT_H           (330)

#define DO_SEARCH_LIST_X                (COMM_BOX1_BORDER)
#define DO_SEARCH_LIST_Y                (COMM_BOX1_BORDER)
#define DO_SEARCH_LIST_W                (DO_SEARCH_LIST_CONT_W - COMM_BOX1_BORDER - COMM_BOX1_BORDER)
#define DO_SEARCH_LIST_H                (DO_SEARCH_LIST_CONT_H - COMM_BOX1_BORDER - COMM_BOX1_BORDER)

//tp list
#define DO_SEARCH_TP_LIST_FRM_X         DO_SEARCH_FRM_X
#define DO_SEARCH_TP_LIST_FRM_Y         (DO_SEARCH_FRM_Y + DO_SEARCH_FRM_H + 10)
#define DO_SEARCH_TP_LIST_FRM_W         DO_SEARCH_FRM_W
#define DO_SEARCH_TP_LIST_FRM_H         (130)

#define DO_SEARCH_TP_LIST_PAGE          (3)
#define DO_SEARCH_TP_LIST_FIELD_CNT     (3)
#define DO_SEARCH_TP_LIST_ITEM_H        (36)
#define DO_SEARCH_TP_LIST_ITEM_V_GAP    (0)

#define DO_SEARCH_TP_LIST_X             (COMM_BOX1_BORDER)
#define DO_SEARCH_TP_LIST_Y             (COMM_BOX1_BORDER)
#define DO_SEARCH_TP_LIST_W             (800)
#define DO_SEARCH_TP_LIST_H             (DO_SEARCH_TP_LIST_FRM_H - COMM_BOX1_BORDER - COMM_BOX1_BORDER)

#define DO_SEARCH_TP_LIST_NO_W          (75)
#define DO_SEARCH_TP_LIST_SAT_W         (250)
#define DO_SEARCH_TP_LIST_INFO_W        (DO_SEARCH_TP_LIST_W - DO_SEARCH_TP_LIST_NO_W - DO_SEARCH_TP_LIST_SAT_W)


#define DO_SEARCH_ANIMATION_CNT         (4)
#define DO_SEARCH_ANIMATION_X           (DO_SEARCH_TP_LIST_FRM_W - DO_SEARCH_ANIMATION_W - 30)
#define DO_SEARCH_ANIMATION_Y           (5)
#define DO_SEARCH_ANIMATION_W           (130)
#define DO_SEARCH_ANIMATION_H           (120)

#define DO_SEARCH_PBAR_X                DO_SEARCH_FRM_X
#define DO_SEARCH_PBAR_Y                (DO_SEARCH_TP_LIST_FRM_Y + DO_SEARCH_TP_LIST_FRM_H + 20)
#define DO_SEARCH_PBAR_W                (DO_SEARCH_FRM_W - DO_SEARCH_PBAR_PERCENT_W)
#define DO_SEARCH_PBAR_H                (16)

#define DO_SEARCH_PBAR_PERCENT_X        (DO_SEARCH_PBAR_X + DO_SEARCH_PBAR_W)
#define DO_SEARCH_PBAR_PERCENT_Y        (DO_SEARCH_PBAR_Y - ((DO_SEARCH_PBAR_PERCENT_H - DO_SEARCH_PBAR_H) >> 1))
#define DO_SEARCH_PBAR_PERCENT_W        (70)
#define DO_SEARCH_PBAR_PERCENT_H        (30)

/* rect style */
#define RSI_DO_SEARCH_LIST              RSI_BOX_2
#define RSI_DO_SEARCH_ITEM              RSI_IGNORE
#define RSI_DO_SEARCH_PBAR_BG           RSI_PROGRESS_BAR_BG
#define RSI_DO_SEARCH_PBAR_MID          RSI_PROGRESS_BAR_MID_YELLOW
#define RSI_DO_SEARCH_FRM         RSI_POPUP_BG

/* font style */
#define FSI_DO_SEARCH_LIST_ITEM         FSI_WHITE

/* others */

RET_CODE open_do_search(u32 para1, u32 para2);

BOOL do_search_is_finish(void);

#ifdef SEARCH_SUPPORT_FILTER
#define  RSN_SEARCH_FILTER_DEFAULT		(1)
u32 ui_do_search_get_search_filter_type(void);
void ui_do_search_set_search_filter_type(u32 type);
#endif
void ui_do_search_full_search(void);

#endif


