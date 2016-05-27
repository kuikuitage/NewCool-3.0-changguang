/****************************************************************************

****************************************************************************/
#ifndef __UI_MUSIC_FULL_SCREEN_H__
#define __UI_MUSIC_FULL_SCREEN_H__

/* coordinate */
#define MUSIC_FULL_SCREEN_CONT_X        ((SCREEN_WIDTH - MUSIC_FULL_SCREEN_CONT_W) / 2)
#define MUSIC_FULL_SCREEN_CONT_Y        COMM_BG_Y
#define MUSIC_FULL_SCREEN_CONT_W        COMM_BG_W
#define MUSIC_FULL_SCREEN_CONT_H        420

#define MUSIC_FULL_PICTURE_X        (COMM_BG_W-(MUSIC_FULL_PICTURE_W+MUSIC_FULL_LRC_W))/2//50
#define MUSIC_FULL_PICTURE_Y        150//70
#define MUSIC_FULL_PICTURE_W        400
#define MUSIC_FULL_PICTURE_H        450

#define MUSIC_FULL_LRC_X        (MUSIC_FULL_PICTURE_X + MUSIC_FULL_PICTURE_W)
#define MUSIC_FULL_LRC_Y        150//70
#define MUSIC_FULL_LRC_W        400
#define MUSIC_FULL_LRC_H        450


#define MUSIC_MOD_PICTURE_X        250
#define MUSIC_MOD_PICTURE_Y        50//70
#define MUSIC_MOD_PICTURE_W        550
#define MUSIC_MOD_PICTURE_H        450

#define MUSIC_MOD_LRC_X        800
#define MUSIC_MOD_LRC_Y        100//70
#define MUSIC_MOD_LRC_W        480
#define MUSIC_MOD_LRC_H        350

#define MUSIC_FULL_BACKGROUP_X        50
#define MUSIC_FULL_BACKGROUP_Y        70
#define MUSIC_FULL_BACKGROUP_W        855
#define MUSIC_FULL_BACKGROUP_H        450

#define MUSIC_FULL_SCREEN_LIST_X        COMM_ITEM_OX_IN_ROOT
#define MUSIC_FULL_SCREEN_LIST_Y        COMM_ITEM_OY_IN_ROOT
#define MUSIC_FULL_SCREEN_LIST_W       (COMM_ITEM_MAX_WIDTH)
#define MUSIC_FULL_SCREEN_LIST_H       360

#define MUSIC_FULL_SCREEN_LIST_MID_L     4
#define MUSIC_FULL_SCREEN_LIST_MID_T     4
#define MUSIC_FULL_SCREEN_LIST_MID_W   (MUSIC_FULL_SCREEN_LIST_W - 2 * MUSIC_FULL_SCREEN_LIST_MID_L)
#define MUSIC_FULL_SCREEN_LIST_MID_H    ((MUSIC_FULL_SCREEN_LIST_ITEM_H + MUSIC_FULL_SCREEN_LIST_ITEM_VGAP) * MUSIC_FULL_SCREEN_LIST_CNT - MUSIC_FULL_SCREEN_LIST_ITEM_VGAP)

#define MUSIC_FULL_SCREEN_LIST_ITEM_H        COMM_ITEM_H
#define MUSIC_FULL_SCREEN_LIST_ITEM_VGAP    4

/* rect style */
#define RSI_MUSIC_FULL_SCREEN_FRM       RSI_WINDOW_1
#define RSI_MUSIC_FULL_SCREEN_LIST      RSI_PBACK
/* font style */

/* others */
#define MUSIC_FULL_SCREEN_LIST_CNT      4
#define MUSIC_FULL_SCREEN_LIST_FIELD   3

/**/
//preview logo
#define MUSIC_FULL_LOGO_X  100
#define MUSIC_FULL_LOGO_Y  120
#define MUSIC_FULL_LOGO_W  250
#define MUSIC_FULL_LOGO_H  150

RET_CODE open_usb_music_fullscreen(u32 para1, u32 para2);

BOOL ui_music_is_fullscreen_play();
void ui_music_set_fullscreen_play(BOOL fullscreen_play);

#endif


