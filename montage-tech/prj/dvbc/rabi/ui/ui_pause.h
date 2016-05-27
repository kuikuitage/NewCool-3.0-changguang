/****************************************************************************

 ****************************************************************************/
#ifndef __UI_PAUSE_H__
#define __UI_PAUSE_H__

#define PAUSE_CONT_FULL_X			1170
#define PAUSE_CONT_FULL_Y			60
#define PAUSE_CONT_FULL_W			PAUSE_ICON_FULL_W
#define PAUSE_CONT_FULL_H			PAUSE_ICON_FULL_H

#define PAUSE_ICON_FULL_X			0
#define PAUSE_ICON_FULL_Y			0
#define PAUSE_ICON_FULL_W			36
#define PAUSE_ICON_FULL_H			36

#define PAUSE_CONT_LIST_W			PAUSE_ICON_LIST_W
#define PAUSE_CONT_LIST_H			PAUSE_ICON_LIST_H
#define PAUSE_CONT_PREVIEW_OX    50
#define PAUSE_CONT_PREVIEW_OY    20

#define PAUSE_ICON_LIST_X			0
#define PAUSE_ICON_LIST_Y			0
#define PAUSE_ICON_LIST_W			36
#define PAUSE_ICON_LIST_H			36

RET_CODE open_pause (u32 para1, u32 para2);

BOOL ui_is_pause (void);

void ui_set_pause (BOOL is_pause);

void close_pause(void);

#endif

