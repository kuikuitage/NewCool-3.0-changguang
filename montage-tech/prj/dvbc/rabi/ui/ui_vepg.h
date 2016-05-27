/****************************************************************************

 ****************************************************************************/
#ifndef _UI_VEPG_H_
#define _UI_VEPG_H_

#include "ui_common.h"

/*coordinates*/

#define VEPG_MENU_GARY	2
#define VEPG_MENU_H		(VEPG_LIST_CONTH+VEPG_MENU_GARY+VEPG_DETAIL_PGW)
#define VEPG_MENU_HGAY	30

//detial container
#define VEPG_DETAIL_X  (120)
#define VEPG_DETAIL_Y  (100)
#define VEPG_DETAIL_W  (542)
#define VEPG_DETAIL_H  (300)

#define VEPG_DETAIL_PGX  COMM_BOX1_BORDER
#define VEPG_DETAIL_PGY  COMM_BOX1_BORDER
#define VEPG_DETAIL_PGW  160
#define VEPG_DETAIL_PGH  (30)

#define VEPG_DETAIL_EVTNX  VEPG_DETAIL_PGX
#define VEPG_DETAIL_EVTNY  (VEPG_DETAIL_PGY + VEPG_DETAIL_PGH + VEPG_DETAIL_VGAP)
#define VEPG_DETAIL_EVTNW  (VEPG_DETAIL_W - (VEPG_DETAIL_EVTNX + VEPG_DETAIL_EVTNX))
#define VEPG_DETAIL_EVTNH  VEPG_DETAIL_PGH

#define VEPG_DETAIL_EVTTX  VEPG_DETAIL_PGX
#define VEPG_DETAIL_EVTTY  (VEPG_DETAIL_EVTNY + VEPG_DETAIL_EVTNH + VEPG_DETAIL_VGAP)
#define VEPG_DETAIL_EVTTW  (VEPG_DETAIL_W - (VEPG_DETAIL_EVTTX + VEPG_DETAIL_EVTTX) - 4 * VEPG_DETAIL_TYPEW)
#define VEPG_DETAIL_EVTTH  VEPG_DETAIL_PGH

#define VEPG_DETAIL_SHORTX  VEPG_DETAIL_PGX
#define VEPG_DETAIL_SHORTY  (VEPG_DETAIL_EVTTY + VEPG_DETAIL_EVTTH + VEPG_DETAIL_VGAP)
#define VEPG_DETAIL_SHORTW  (VEPG_DETAIL_W - (VEPG_DETAIL_SHORTX + VEPG_DETAIL_SHORTX))
#define VEPG_DETAIL_SHORTH  (4 * (VEPG_DETAIL_PGH + VEPG_DETAIL_VGAP))

//
#define VEPG_DETAIL_TYPEX  (VEPG_DETAIL_EVTTX + VEPG_DETAIL_EVTTW)
#define VEPG_DETAIL_TYPEY  VEPG_DETAIL_EVTTY
#define VEPG_DETAIL_TYPEW  (70)
#define VEPG_DETAIL_TYPEH  VEPG_DETAIL_PGH

#define VEPG_DETAIL_SCRAMBLEX  (VEPG_DETAIL_TYPEX + VEPG_DETAIL_TYPEW)
#define VEPG_DETAIL_SCRAMBLEY  VEPG_DETAIL_TYPEY
#define VEPG_DETAIL_SCRAMBLEW  VEPG_DETAIL_TYPEW
#define VEPG_DETAIL_SCRAMBLEH  VEPG_DETAIL_TYPEH

#define VEPG_DETAIL_AGEX  (VEPG_DETAIL_TYPEX + 2 * VEPG_DETAIL_TYPEW)
#define VEPG_DETAIL_AGEY  VEPG_DETAIL_TYPEY
#define VEPG_DETAIL_AGEW  VEPG_DETAIL_TYPEW
#define VEPG_DETAIL_AGEH  VEPG_DETAIL_TYPEH

#define VEPG_DETAIL_RESERVEX  (VEPG_DETAIL_TYPEX + 3 * VEPG_DETAIL_TYPEW)
#define VEPG_DETAIL_RESERVEY  VEPG_DETAIL_TYPEY
#define VEPG_DETAIL_RESERVEW  VEPG_DETAIL_TYPEW
#define VEPG_DETAIL_RESERVEH  VEPG_DETAIL_TYPEH


#define VEPG_DETAIL_VGAP  (4)

#define VEPG_PREV_X  (672)
#define VEPG_PREV_Y  (100)
#define VEPG_PREV_W  (488)
#define VEPG_PREV_H  (300)

#define VEPG_TIME_X  740
#define VEPG_TIME_Y  30
#define VEPG_TIME_W  170
#define VEPG_TIME_H  36

#define VEPG_LIST_CONTX  (0)
#define VEPG_LIST_CONTY  (0)
#define VEPG_LIST_CONTW  (694)
#define VEPG_LIST_CONTH  (260)
#define VEPG_LIST_HGAP  (2)
#define VEPG_LIST_VGAP  (2)

#define VEPG_LIST_PGX  (0)
#define VEPG_LIST_PGY  (0)
#define VEPG_LIST_PGW  (320)//(240)//160
#define VEPG_LIST_PGH  (210)

#define VEPG_LIST_DATEX  0
#define VEPG_LIST_DATEY  0
#define VEPG_LIST_DATEW  (140)//160//148
#define VEPG_LIST_DATEH  VEPG_LIST_CONTH

#define VEPG_LIST_LIST_GAP		4
#define VEPG_LIST_EVTX  		(VEPG_LIST_DATEX + VEPG_LIST_DATEW+VEPG_LIST_LIST_GAP)
#define VEPG_LIST_EVTY  		VEPG_LIST_DATEY
#define VEPG_LIST_EVTW  		400//470//400
#define VEPG_LIST_EVTH 			 VEPG_LIST_DATEH

//help
#define VEPG_HELP_X  (200)
#define VEPG_HELP_Y  (620)//530
#define VEPG_HELP_W  (880)//600
#define VEPG_HELP_H  (50)//28

#define VEPG_LIST_SBARX  (VEPG_LIST_CONTX + VEPG_LIST_CONTW + 5)
#define VEPG_LIST_SBARY  (VEPG_LIST_CONTY + VEPG_LIST_VGAP)
#define VEPG_LIST_SBARW  (6)
#define VEPG_LIST_SBARH  (VEPG_LIST_PGH - ((VEPG_LIST_PGY + VEPG_LIST_VGAP) << 1))

#define VEPG_LIST_SBAR_MIDL  0
#define VEPG_LIST_SBAR_MIDT  12
#define VEPG_LIST_SBAR_MIDR  12
#define VEPG_LIST_SBAR_MIDB  (VEPG_LIST_SBARH - VEPG_LIST_SBAR_MIDT)

/*rstyle*/
#define RSI_VEPG_MENU    RSI_IGNORE
#define RSI_VEPG_PREV  RSI_TV
#define RSI_VEPG_DETAIL  RSI_BOX_1
#define RSI_VEPG_LIST_CONT RSI_BLACK_FRM
#define RSI_VEPG_LIST  RSI_PBACK
#define RSI_VEPG_TIME  RSI_PBACK
#define RSI_VEPG_SBAR RSI_SCROLL_BAR_BG
#define RSI_VEPG_SBAR_MID RSI_SCROLL_BAR_MID

/*others*/
#define VEPG_GROUP_LIST_FIELD  1
#define VEPG_LIST_PAGE  7//5
#define VEPG_LIST_PAGE_TOTAL  7
#define VEPG_LIST_FIELD  2

#define VEPG_LCONT_MBOX_TOL  4
#define VEPG_LCONT_MBOX_COL  4
#define VEPG_LCONT_MBOX_ROW  1
#define VEPG_LCONT_MBOX_HGAP  0
#define VEPG_LCONT_MBOX_VGAP  0

#define VEPG_INFO_MBOX_TOL  2
#define VEPG_INFO_MBOX_COL  2
#define VEPG_INFO_MBOX_ROW  1
#define VEPG_INFO_MBOX_HGAP  4
#define VEPG_INFO_MBOX_VGAP  0

/*fstyle*/
#define FSI_VEPG_INFO_MBOX FSI_WHITE

BOOL is_epg_have_focus(void);
control_t *get_vepg_date_list(void);
RET_CODE open_vepg(u32 para1, u32 para2);

#endif
