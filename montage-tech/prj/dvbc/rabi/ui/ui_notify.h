/****************************************************************************

 ****************************************************************************/
#ifndef __UI_NOTIFY_H__
#define __UI_NOTIFY_H__

/* coordinate */
#define NOTIFY_CONT_X     ((SCREEN_WIDTH - NOTIFY_CONT_W) / 2)//440
#define NOTIFY_CONT_Y     10
#define NOTIFY_CONT_W     200//180
#define NOTIFY_CONT_H     50//32

/* rect style */
#define RSI_NOTIFY_TXT    RSI_BOX_1//RSI_NOTIFY_SH

/* font style */
#define FSI_NOTIFY_TXT    FSI_WHITE

#define NOTIFY_AUTOCLOSE_MS 3000

enum notify_type
{
  NOTIFY_TYPE_STRID = 0,
  NOTIFY_TYPE_ASC,
  NOTIFY_TYPE_UNI,
};

void ui_set_notify_during(u32 during);

void ui_set_notify(rect_t *p_rect, u32 type, u32 content);

BOOL ui_is_notify(void);

RET_CODE open_notify(u32 para1, u32 para2);

void close_notify(void);

void  ui_set_notify_autoclose(BOOL is_autoclose);
#endif


