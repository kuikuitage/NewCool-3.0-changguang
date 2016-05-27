/****************************************************************************

 ****************************************************************************/
#ifndef _UI_USB_SETTING_H_
#define _UI_USB_SETTING_H_
/*!
 \file ui_usb_setting.h

   This file defined the coordinates, rectangle style, font style and some other
   constants used in fav list menu.
   And interfaces for open the fav list menu.

   Development policy:
   If you want to change the menu style, you can just modified the macro
   definitions.
 */
//#include "ui_common.h"
//#include "ui_fav_set.h"

//coordinates
#define USB_SET_X  (SYS_LEFT_CONT_W+(SYS_RIGHT_CONT_W-USB_SET_W)/2)
#define USB_SET_Y  ((SCREEN_HEIGHT-USB_SET_H)/2) //160//236 
#define USB_SET_W  280//160
#define USB_SET_H  200


#define USB_SET_LIST_MIDL FAV_SET_LIST_MIDL  
#define USB_SET_LIST_MIDT FAV_SET_LIST_MIDT  
#define USB_SET_LIST_MIDW FAV_SET_LIST_MIDW  
#define USB_SET_LIST_MIDH  190

RET_CODE open_usb_setting(u32 para1, u32 para2);

#endif

