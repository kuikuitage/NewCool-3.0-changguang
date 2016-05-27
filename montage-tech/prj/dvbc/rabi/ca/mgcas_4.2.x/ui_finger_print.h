/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_FINGER_PRINT_H__
#define __UI_FINGER_PRINT_H__

#define FINGER_PRINT_X   980
#define FINGER_PRINT_Y   100
#define FINGER_PRINT_W   250
#define FINGER_PRINT_H   40

//finger
#define FINGER_PRINT_TEXT_X  0
#define FINGER_PRINT_TEXT_Y  0
#define FINGER_PRINT_TEXT_W  220
#define FINGER_PRINT_TEXT_H  30


RET_CODE open_finger_print (u32 para1, u32 para2);

RET_CODE ui_close_finger_menu(void);

void ui_finger_print_repaint(void);

void ui_close_finger_mainwin(void);

BOOL ui_finger_is_exsit(void);

BOOL ui_is_finger_show(void);

BOOL ui_finger_is_repaint(void);
#endif

