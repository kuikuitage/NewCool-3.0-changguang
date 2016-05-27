/****************************************************************************

****************************************************************************/
#ifndef __UI_NPROG_BAR_H__
#define __UI_NPROG_BAR_H__

/* coordinate */
#define NPROG_BAR_CONT_GAP			 80
#define NPROG_BAR_COMM_GAP			 10
#define NPROG_BAR_CONT_X             (0)
#define NPROG_BAR_CONT_Y             (SCREEN_HEIGHT-NPROG_BAR_CONT_H)
#define NPROG_BAR_CONT_W             (SCREEN_WIDTH)
#define NPROG_BAR_CONT_H             (200)//260

#define NPROG_BAR_FRM_X              (0)
#define NPROG_BAR_FRM_Y              (0)
#define NPROG_BAR_FRM_W              (NPROG_BAR_CONT_W - NPROG_BAR_ADS_MOVE)
#define NPROG_BAR_FRM_H              NPROG_BAR_CONT_H

#define NPROG_BAR_TITLE_X			 0
#define NPROG_BAR_TITLE_W            NPROG_BAR_FRM_W
#define NPROG_BAR_TITLE_H            (60)

#define NPROG_BAR_NUMER_X            (NPROG_BAR_CONT_GAP)
#define NPROG_BAR_NUMER_Y            (20)
#define NPROG_BAR_NUMER_W            (75)
#define NPROG_BAR_NUMER_H            (36)

#define NPROG_BAR_TITLE_NAME_X       (NPROG_BAR_NUMER_X + NPROG_BAR_NUMER_W + NPROG_BAR_COMM_GAP)
#define NPROG_BAR_TITLE_NAME_Y       (20)
#define NPROG_BAR_TITLE_NAME_W       (400)
#define NPROG_BAR_TITLE_NAME_H       (NPROG_BAR_NUMER_H)

#define NPROG_BAR_DATE_ICON_X        (NPROG_BAR_DATE_TXT_X-NPROG_BAR_DATE_ICON_W)
#define NPROG_BAR_DATE_ICON_Y        (20)
#define NPROG_BAR_DATE_ICON_W        (30)
#define NPROG_BAR_DATE_ICON_H        (30)

#define NPROG_BAR_DATE_TXT_X         (NPROG_BAR_TIME_ICON_X-NPROG_BAR_DATE_TXT_W)
#define NPROG_BAR_DATE_TXT_Y         NPROG_BAR_DATE_ICON_Y
#define NPROG_BAR_DATE_TXT_W         (150)
#define NPROG_BAR_DATE_TXT_H         (NPROG_BAR_NUMER_H)

#define NPROG_BAR_TIME_ICON_X        (NPROG_BAR_TIME_TXT_X-NPROG_BAR_TIME_ICON_W)
#define NPROG_BAR_TIME_ICON_Y        NPROG_BAR_DATE_ICON_Y
#define NPROG_BAR_TIME_ICON_W        (30)
#define NPROG_BAR_TIME_ICON_H        (NPROG_BAR_NUMER_H)

#define NPROG_BAR_TIME_TXT_X         (NPROG_BAR_TITLE_W-NPROG_BAR_TIME_TXT_W-NPROG_BAR_CONT_GAP)
#define NPROG_BAR_TIME_TXT_Y         NPROG_BAR_DATE_ICON_Y
#define NPROG_BAR_TIME_TXT_W         (80)
#define NPROG_BAR_TIME_TXT_H         (NPROG_BAR_NUMER_H)

#define NPROG_BAR_PF_INFO_CNT        (2)
#define NPROG_BAR_PF_INFO_X          (NPROG_BAR_NUMER_X)//39
#define NPROG_BAR_PF_INFO_Y          (85)//88
#define NPROG_BAR_PF_INFO_PRO_W      (320)
#define NPROG_BAR_PF_INFO_DUR_W      (180)
#define NPROG_BAR_PF_INFO_H          (36)
#define NPROG_BAR_PF_INFO_V_GAP      (4)

#define NPROG_BAR_GROUP_X            (NPROG_BAR_FRM_W-NPROG_BAR_GROUP_W-NPROG_BAR_CONT_GAP)
#define NPROG_BAR_GROUP_Y            (68)
#define NPROG_BAR_GROUP_W            (280)
#define NPROG_BAR_GROUP_H            (36)

#define NPROG_BAR_CA_X               NPROG_BAR_GROUP_X
#define NPROG_BAR_CA_Y               (NPROG_BAR_AV_ICON_Y+NPROG_BAR_AV_ICON_H)//114
#define NPROG_BAR_CA_W               (NPROG_BAR_GROUP_W)
#define NPROG_BAR_CA_H               NPROG_BAR_GROUP_H

#define NPROG_BAR_TP_X               NPROG_BAR_GROUP_X
#define NPROG_BAR_TP_Y               (NPROG_BAR_TITLE_H)
#define NPROG_BAR_TP_W               NPROG_BAR_GROUP_W
#define NPROG_BAR_TP_H               NPROG_BAR_GROUP_H

#define NPROG_BAR_SIGNAL_X           (840)//850
#define NPROG_BAR_SIGNAL_Y           (90)//115
#define NPROG_BAR_SIGNAL_W           (12)//12
#define NPROG_BAR_SIGNAL_H           (70)//70
#define NPROG_BAR_SIGNAL_H_GAP       (10)

#define NPROG_BAR_MARK_ICON_CNT      (2)/*  MONEY FAV LOCK SKIP*/
#define NPROG_BAR_MARK_ICON_X        (NPROG_BAR_TITLE_NAME_X+NPROG_BAR_TITLE_NAME_W+NPROG_BAR_COMM_GAP)//621
#define NPROG_BAR_MARK_ICON_Y        ((NPROG_BAR_TITLE_H-NPROG_BAR_MARK_ICON_H)/2)//120
#define NPROG_BAR_MARK_ICON_W        (32)
#define NPROG_BAR_MARK_ICON_H        (30)
#define NPROG_BAR_MARK_ICON_H_GAP    (5)

#define NPROG_BAR_INFO_ICON_CNT      (2) /* SUBTT TELTEXT EPG */
#define NPROG_BAR_INFO_ICON_X        (NPROG_BAR_MARK_ICON_X+NPROG_BAR_MARK_ICON_W*NPROG_BAR_MARK_ICON_CNT)//720
#define NPROG_BAR_INFO_ICON_Y        (NPROG_BAR_MARK_ICON_Y)
#define NPROG_BAR_INFO_ICON_W        (35)
#define NPROG_BAR_INFO_ICON_H        (30)
#define NPROG_BAR_INFO_ICON_H_GAP    (5)

#define NPROG_BAR_ENCRYPT_ICON_CNT   (9) /* ENCRYPT SYS */
#define NPROG_BAR_ENCRYPT_ICON_X     (40)// 410//720
#define NPROG_BAR_ENCRYPT_ICON_Y     ((NPROG_BAR_TITLE_H-NPROG_BAR_ENCRYPT_ICON_H)/2 )
#define NPROG_BAR_ENCRYPT_ICON_W     (25)
#define NPROG_BAR_ENCRYPT_ICON_H     (30)
#define NPROG_BAR_ENCRYPT_ICON_H_GAP (5)

#define NPROG_BAR_AV_ICON_CNT        (1) //10801 H.264 MPEGI
#define NPROG_BAR_AV_ICON_X          (NPROG_BAR_GROUP_X)
#define NPROG_BAR_AV_ICON_Y          (NPROG_BAR_TITLE_H+36)
#define NPROG_BAR_AV_ICON_W          (NPROG_BAR_CA_W)//80 
#define NPROG_BAR_AV_ICON_H          (30)
#define NPROG_BAR_AV_ICON_H_GAP      (5)

#ifdef ENABLE_ADS
extern u16 ads_nprogbar_move;
#define	NPROG_BAR_ADS_MOVE			(ads_nprogbar_move)
#else
#define	NPROG_BAR_ADS_MOVE			(0)
#endif


/* rect style */
#define RSI_NPROG_BAR_FRM            RSI_POPUP_BG//RSI_SUBMENU_DETAIL_BG

/* font style */
#ifdef BIGFONT_PROJECT
#define FSI_NPROG_BAR_NAME           FSI_WHITE_30
#define FSI_NPROG_BAR_NORMAL         FSI_WHITE_28
#else
#define FSI_NPROG_BAR_NAME           FSI_WHITE_28
#define FSI_NPROG_BAR_NORMAL         FSI_WHITE_26
#endif
#define FSI_NPROG_BAR_PF             FSI_BLUE
#define FSI_NPROG_BAR_GROUP          FSI_PROGBAR_BOTTOM

#define RSI_TP_LIST_SBAR_BG          RSI_SCROLL_BAR_BG
/* rect style */
#define RSI_TP_LIST_FRM              RSI_WINDOW_1
#define RSI_TP_LIST                  RSI_PBACK
#define RSI_TP_LIST_SBAR_BG          RSI_SCROLL_BAR_BG
#define RSI_TP_LIST_SBAR_MID         RSI_SCROLL_BAR_MID
#define RSI_TP_LIST_PBAR_BG          RSI_PROGRESS_BAR_BG
#define RSI_TP_LIST_PBAR_MID         RSI_PROGRESS_BAR_MID_ORANGE
#define RSI_TP_LIST_SAT              RSI_ITEM_1_HL

RET_CODE open_nprog_bar(u32 para1, u32 para2);
void fill_nprog_info(control_t *cont, u16 prog_id);
#endif

