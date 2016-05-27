/****************************************************************************

****************************************************************************/
#ifndef __UI_NUM_PLAY_H__
#define __UI_NUM_PLAY_H__

/* coordinate */
#define NUM_PLAY_CONT_X       (850)
#define NUM_PLAY_CONT_Y       (42)
#define NUM_PLAY_CONT_W       (210)
#define NUM_PLAY_CONT_H       (64)

#define NUM_PLAY_FRM_X       (NUM_PLAY_CONT_W-NUM_PLAY_FRM_W)
#define NUM_PLAY_FRM_Y       (0)
#define NUM_PLAY_FRM_W       (NUM_PLAY_CONT_W)//(191)
#define NUM_PLAY_FRM_H       (NUM_PLAY_CONT_H)

#define NUM_PLAY_TXT_X       0
#define NUM_PLAY_TXT_Y       0
#define NUM_PLAY_TXT_W       (NUM_PLAY_FRM_W-2*NUM_PLAY_TXT_X)
#define NUM_PLAY_TXT_H       (NUM_PLAY_FRM_H-2*NUM_PLAY_TXT_Y)

//for ui_proglist
#define NUM_PLAY_CONT_LISTX       850
#define NUM_PLAY_CONT_LISTY       120
#define NUM_PLAY_CONT_LISTW       (NUM_PLAY_CONT_W)
#define NUM_PLAY_CONT_LISTH       (NUM_PLAY_CONT_H)

#define NUM_PLAY_FRM_LISTX       (NUM_PLAY_CONT_LISTW - NUM_PLAY_FRM_LISTW)
#define NUM_PLAY_FRM_LISTY       0
#define NUM_PLAY_FRM_LISTW       (NUM_PLAY_CONT_LISTW)
#define NUM_PLAY_FRM_LISTH       NUM_PLAY_CONT_H

#define NUM_PLAY_TXT_LISTX       0
#define NUM_PLAY_TXT_LISTY       0
#define NUM_PLAY_TXT_LISTW       (NUM_PLAY_FRM_LISTW - 2*NUM_PLAY_TXT_LISTX)
#define NUM_PLAY_TXT_LISTH       (NUM_PLAY_FRM_LISTH - 2*NUM_PLAY_TXT_LISTY)

#define NUM_PLAY_CNT        4

/* rect style */
#define RSI_NUM_PLAY_CONT     	RSI_BOX_1//RSI_TRANSPARENT
#define RSI_NUM_PLAY_TXT     	RSI_BOX_1//RSI_TRANSPARENT

/* font style */
#define FSI_NUM_PLAY_TXT      FSI_WHITE_56

/* others */

RET_CODE open_num_play(u32 para1, u32 para2);

#endif

