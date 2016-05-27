/****************************************************************************

****************************************************************************/
#ifndef __UI_SUMENU_NETWORK_H__
#define __UI_SUMENU_NETWORK_H__

//left arrow 
#define UI_SUBMENU_NET_LEFT_ARROW_X 60
#define UI_SUBMENU_NET_LEFT_ARROW_Y (SCREEN_HEIGHT - UI_SUBMENU_NET_LEFT_ARROW_H)/2
#define UI_SUBMENU_NET_LEFT_ARROW_W 17
#define UI_SUBMENU_NET_LEFT_ARROW_H 50
//right arrow
#define UI_SUBMENU_NET_RIGHT_ARROW_X UI_SUBMENU_NET_CTRL_X + \
  UI_SUBMENU_NET_CTRL_W +(UI_SUBMENU_NET_CTRL_X - UI_SUBMENU_NET_LEFT_ARROW_X - UI_SUBMENU_NET_LEFT_ARROW_W)
#define UI_SUBMENU_NET_RIGHT_ARROW_Y UI_SUBMENU_NET_LEFT_ARROW_Y
#define UI_SUBMENU_NET_RIGHT_ARROW_W UI_SUBMENU_NET_LEFT_ARROW_W
#define UI_SUBMENU_NET_RIGHT_ARROW_H UI_SUBMENU_NET_LEFT_ARROW_H

//icon box 
#define UI_SUBMENU_NET_CTRL_X 120
#define UI_SUBMENU_NET_CTRL_Y 190
#define UI_SUBMENU_NET_CTRL_W 1040
#define UI_SUBMENU_NET_CTRL_H 280 + UI_SUBMENU_TEXT_CTL + UI_SUBMENU_TEXT_VGAP
#define UI_SUBMENU_TEXT_CTL 50
#define UI_SUBMENU_TEXT_VGAP 10

//bottom help container
#define UI_NETWORK_BOTTOM_HELP_X  0
#define UI_NETWORK_BOTTOM_HELP_Y  630
#define UI_NETWORK_BOTTOM_HELP_W  1040
#define UI_NETWORK_BOTTOM_HELP_H  80

//youporn pwdlg 
#define YOUPORN_PWD_DLG_FOR_CHK_X      ((SCREEN_WIDTH-PWDLG_W)/2)
#define YOUPORN_PWD_DLG_FOR_CHK_Y      ((SCREEN_HEIGHT-PWDLG_H)/2)

#define NETWORK_SUBMENU_ITEM 4


RET_CODE open_submenu_network(u32 para1, u32 para2);

#endif


