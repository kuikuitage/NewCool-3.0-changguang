/****************************************************************************

 ****************************************************************************/
#ifndef _UI_PICTURE_H_
#define _UI_PICTURE_H_

#include "ui_common.h"

/* coordinate */
#define PICTURE_CONT_X             ((SCREEN_WIDTH - PICTURE_CONT_W) / 2)
#define PICTURE_CONT_Y             ((SCREEN_HEIGHT - PICTURE_CONT_H) / 2)
#define PICTURE_CONT_W             SCREEN_WIDTH
#define PICTURE_CONT_H             SCREEN_HEIGHT

//title icons
#define PICTURE_PAUSE_X  (PICTURE_CONT_W - (PICTURE_PAUSE_W * 2))
#define PICTURE_PAUSE_Y  40
#define PICTURE_PAUSE_W  40
#define PICTURE_PAUSE_H  40

typedef  RET_CODE (*OBJ_PICTURE_CB)();

typedef struct
{
  BOOL is_fav_pic;
  OBJ_PICTURE_CB cb;
} picture_param_t;


RET_CODE open_picture(u32 para1, u32 para2);

#endif
