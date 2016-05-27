/****************************************************************************

****************************************************************************/
#ifndef __UI_XMAINMENU_H__
#define __UI_XMAINMENU_H__
//mainmenu
#define MMENU_CONT_X          ((SCREEN_WIDTH - MMENU_CONT_W)/2)
#define MMENU_CONT_Y          (SCREEN_HEIGHT - MMENU_CONT_H-MAINMENU_FONT_H-20)
#define MMENU_CONT_W         SCREEN_WIDTH
#define MMENU_CONT_H          150

//btn
#define MAINMENU_BTN_CNT			 4
#define MAINMENU_BTN_H			 MMENU_CONT_H
#define MAINMENU_BTN_W			 200
#define MAINMENU_TV_X			 ((MMENU_CONT_W-(MAINMENU_BTN_CNT*(MAINMENU_BTN_W+SPACE_W))-SPACE_W)/2)

#define SPACE_W		3

#ifdef BIGFONT_PROJECT
#define MAINMENU_FONT_H		70
#else
#define MAINMENU_FONT_H		55
#endif

RET_CODE open_main_menu(u32 para1, u32 para2);

#endif

