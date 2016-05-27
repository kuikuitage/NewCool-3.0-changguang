#ifndef __UI_XSYS_SET_H__
#define __UI_XSYS_SET_H__

//sys_set main menu
#define SYS_SET_X 					0
#define SYS_SET_Y					0
#define SYS_SET_W 					SCREEN_WIDTH
#define SYS_SET_H 					SCREEN_HEIGHT
#if defined DVB_CA || defined DESAI_52_CA || defined TR_CA || SHUSHITONG_CA ||defined TF_CA //||defined SMSX_CA
#define SYS_SET_BTN_ONEPAGE			14
#elif SMSX_CA
#define SYS_SET_BTN_ONEPAGE			8
#else
#define SYS_SET_BTN_ONEPAGE			13
#endif

//sys_set_titil
#define SYS_TITLE_H					66

//sys_set_bar
#define SYS_BAR_H					16

#define SYS_SET_CTRL_TYPE     		2 

//sys_set left option
#define SYS_LEFT_CONT_Y				(SYS_TITLE_H+SYS_BAR_H)
#define SYS_LEFT_CONT_W				300
#define SYS_LEFT_CONT_H				(SYS_SET_H-SYS_LEFT_CONT_Y)

//sys_set right
#define RIGHT_BTN_GAP				10
#define SYS_RIGHT_CONT_X				SYS_LEFT_CONT_W	
#define SYS_RIGHT_CONT_Y				SYS_LEFT_CONT_Y
#define SYS_RIGHT_CONT_W				(SYS_SET_W-SYS_LEFT_CONT_W)
#define SYS_RIGHT_CONT_H				SYS_LEFT_CONT_H

//sys_set_ico
#define SYS_SET_ICO_X				40	
#define SYS_SET_ICO_W				(65)	
#define SYS_SET_ICO_H				62		

//sys_set_ico
#define SYS_SET_TEXT_X				(SYS_SET_ICO_X + SYS_SET_ICO_W)	
#define SYS_SET_TEXT_W			    (120)	
#define SYS_SET_TEXT_H				(58)	

//text
#define SYS_OPT_TEXT_H				37
#define SYS_OPT_TEXT_W				260

//btn bmp ico
#define SYS_BTN_ICO_W				36

//type
#define SYS_SET_TYPE_PAGE1_CNT			4
#define SYS_SET_TYPE_X				20
#define SYS_SET_TYPE_W				200
#define SYS_SET_TYPE_H				28

//-----------pwd dlg xy-------------------------
#define SYS_DLG_FOR_CHK_X      ((SCREEN_WIDTH-PWDLG_W)/2)
#define SYS_DLG_FOR_CHK_Y      ((SCREEN_HEIGHT-PWDLG_H)/2)

//ask for savdlg 
#define RESTORE_ASK_L		   (SYS_LEFT_CONT_W+(SYS_RIGHT_CONT_W-PWDLG_W)/2)
#define RESTORE_ASK_T		   ((SCREEN_HEIGHT-PWDLG_H)/2)
#define RESTORE_ASK_R		   (RESTORE_ASK_L+PWDLG_W)
#define RESTORE_ASK_B		   (RESTORE_ASK_T+PWDLG_H)

//wifi dlg
#define WIFI_DLG_X			(SYS_LEFT_CONT_W+(SYS_RIGHT_CONT_W - COMM_DLG_W) / 2)
#define WIFI_DLG_Y			((SCREEN_HEIGHT - COMM_DLG_H) / 2)


enum{
	AUTO_SEARCH = 1,
	MANUAL_SEARCH,
	RANGE_SEARCH,
};

void sys_set_paint_left_cont(void);
RET_CODE swtich_to_sys_set(u16 old_root_id, u32 para);
RET_CODE open_xsys_set(u32 para1, u32 para2);

#endif

