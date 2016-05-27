/****************************************************************************

 ****************************************************************************/
#ifndef __UI_VOLUME_H__
#define __UI_VOLUME_H__

#define VOLUME_L			((SCREEN_WIDTH-VOLUME_W)/2)
#define VOLUME_T			580
#define VOLUME_W		    600
#define VOLUME_H			50

#define VOLUME_BAR_CONT_X	0
#define VOLUME_BAR_CONT_Y	0
#define VOLUME_BAR_CONT_W	VOLUME_W
#define VOLUME_BAR_CONT_H	VOLUME_H

#define VOLUME_BAR_ICON_X	    24
#define VOLUME_BAR_ICON_Y	    ((VOLUME_H - VOLUME_BAR_ICON_H)/2)
#define VOLUME_BAR_ICON_W	 36
#define VOLUME_BAR_ICON_H	    36

#define VOLUME_BAR_X	    (2 * VOLUME_BAR_ICON_X + VOLUME_BAR_ICON_W)
#define VOLUME_BAR_Y	    ((VOLUME_H - VOLUME_BAR_H)/2)
#define VOLUME_BAR_W	 430
#define VOLUME_BAR_H	    16

#define VOLUME_MAX    AP_VOLUME_MAX

/* rect style */
#define RSI_VOLUME_CONT     RSI_IGNORE
#define RSI_VOLUME_BAR_FRAME RSI_BOX_2

void set_volume(u8 volume);

RET_CODE open_volume (u32 para1, u32 para2);

RET_CODE close_volume(void);

extern BOOL is_display_pg;

#endif


