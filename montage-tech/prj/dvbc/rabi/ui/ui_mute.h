/****************************************************************************

 ****************************************************************************/
#ifndef __UI_MUTE_H__
#define __UI_MUTE_H__

/* coordinate */
#define MUTE_CONT_FULL_X			1140
#define MUTE_CONT_FULL_Y			60
#define MUTE_CONT_FULL_W			MUTE_ICON_FULL_W
#define MUTE_CONT_FULL_H			MUTE_ICON_FULL_H

#define MUTE_ICON_FULL_X			0
#define MUTE_ICON_FULL_Y			0
#define MUTE_ICON_FULL_W			36
#define MUTE_ICON_FULL_H			36

#define MUTE_CONT_LIST_W			MUTE_ICON_LIST_W
#define MUTE_CONT_LIST_H			MUTE_ICON_LIST_H
#define MUTE_CONT_PREVIEW_OX    50
#define MUTE_CONT_PREVIEW_OY    10

#define MUTE_ICON_LIST_X			0
#define MUTE_ICON_LIST_Y			0
#define MUTE_ICON_LIST_W			36
#define MUTE_ICON_LIST_H			36

#define USB_MUSIC_MUTE_CONT_X			1090
#define USB_MUSIC_MUTE_CONT_Y			450

RET_CODE open_mute (u32 para1, u32 para2);

BOOL ui_is_mute (void);

void ui_set_mute (BOOL is_mute);

void close_mute(void);

#endif


