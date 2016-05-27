#include "ui_common.h"
#include "ui_xsys_set.h"
#include "ui_pause.h"
#include "ui_signal.h"
#include "ui_notify.h"
#include "ui_network_config_wifi.h"
#include "ui_mute.h"
#include "ui_video.h"


enum{
	IDC_INVALID = 0,
	IDC_SYS_SET_TITLE,
	IDCSYS_SET_ICO,
	IDCSYS_SET_TEXT,
	IDC_SYS_SET_TITLE_BAR,
 	IDC_RIGHT_CONT,
	IDC_BTN_ICON,

	IDC_CNT,
};
enum{
	IDC_SEARCH_TYPE = IDC_CNT,
	IDC_NET_TYPE,
	IDC_AUDIO_TYPE,
#if defined DVB_CA || defined DESAI_52_CA|| defined TR_CA ||defined SHUSHITONG_CA ||defined TF_CA ||defined SMSX_CA
	IDC_USB,
	IDC_SYSTEM_TYPE,
#else
	IDC_SYSTEM_TYPE,
	IDC_USB,
	IDC_MORE_TYPE,
#endif
	IDC_TYPE_CNT,
};

enum{
 	IDC_LEFT_CONT1 = IDC_TYPE_CNT,
 	IDC_LEFT_CONT2,
 	
 	IDC_CONT_CNT,
};

enum{
	IDC_AUTO_SEARCH = IDC_CONT_CNT,
	IDC_MANUAL_SEARCH, 
	IDC_RANGE_SEARCH,
	IDC_FREQUENCY,
	
	IDC_WIFI,
	IDC_LAN,
	
#if defined DVB_CA || defined DESAI_52_CA ||defined TF_CA
	IDC_CAINFO, 
      IDC_CA_PWD_MODIFY,
      IDC_CA_WOR_DURATION_SET,
      IDC_CA_LEVEL_CONTROL_SET,
      IDC_CA_PAIR_STATE,
	IDC_CA_OPERATOR,
	IDC_CA_EMAIL,
	//IDC_CA_ENTITLE_INFO,
	IDC_MORE,
#elif TR_CA
	IDC_CAINFO, 
	IDC_CA_LEVEL_CONTROL_SET,
	IDC_CA_PWD_PIN,
	IDC_CA_OPERATOR,
	IDC_CA_FEED,
	IDC_CA_ALTER_MESS,
	IDC_CA_EMAIL,
	IDC_MORE,
#elif SZXC_CA
	IDC_CAINFO,
	IDC_CA_MOTHER,
	IDC_CA_EMAIL,

	IDC_LOCK,
	IDC_DISPLAY,
	IDC_SYSINFO,
	IDC_MORE,
#elif SHUSHITONG_CA
	IDC_CAINFO, 
	IDC_CA_LEVEL_CONTROL_SET,
	IDC_CA_WOR_DURATION_SET,
	IDC_CA_PWD_MODIFY,
	IDC_CA_FEED,
	IDC_CA_ENTITLE_INFO,
	IDC_CA_EMAIL,
	IDC_MORE,
#elif SMSX_CA
	IDC_CAINFO, 
	//IDC_CA_ENTITLE_INFO,
	//IDC_CA_PWD_MODIFY,
	//IDC_CA_LEVEL_CONTROL_SET,
	//IDC_CA_WOR_DURATION_SET,
	//IDC_CA_FEED,
	//IDC_CA_EMAIL,
	IDC_MORE,
#else
	IDC_CAINFO, 
	IDC_CA_EMAIL,
	IDC_CA_ENTITLE_INFO,
	
	IDC_LOCK,
	IDC_DISPLAY,
	IDC_SYSINFO,
	IDC_MORE,
#endif
	
	IDC_UPDATE,
	IDC_USB_DUMP,
	IDC_USB_HDD_INFO,
	IDC_USB_FORMAT,
	//IDC_USB_DVR_CFG,
	IDC_USB_JUMP,
	
#if defined DVB_CA || defined DESAI_52_CA ||defined TR_CA ||defined SHUSHITONG_CA ||defined TF_CA ||defined SMSX_CA     
	IDC_LOCK,
	IDC_DISPLAY,
	IDC_SYSINFO,
#endif
#ifdef AUTH_SERVER_SETTING
	IDC_AUTH_SETTING,
#else
	#ifdef AUPPORT_TS_RECORD
	IDC_RECORD_MANAGER,
	#else
	IDC_STB_NAME_SET,
	#endif
#endif
	IDC_TIME,
	IDC_TIMER_SET,
	IDC_FACTORY_SET,
	IDC_LANGUAGE, 
	IDC_UPGRADE_BY_NET,
	IDC_BACK,
	IDC_BTN_CNT,
};

typedef struct{
	u8 btn_id;
	u16 btn_str_id;
	u16 btn_bmp_id;
	u16 root_id;
	u32 para;
}btn_attr_with_root_t;

#define SYS_SET_ERR_IDEX		0xFFFF
static btn_attr_with_root_t btn_attr_with_root[IDC_BTN_CNT-IDC_CONT_CNT] = {
/*	  btn_id						btn_str_id				btn_bmp_id					root_id								para*/
// page1
#ifdef DTMB_PROJECT
	#ifdef  SUPPORT_POWER_FULL_SEARCH
		#ifdef SHUSHITONG_CGHHT
		{IDC_AUTO_SEARCH,	IDS_AUTOL_SEARCH,		IM_XIC_SET_SEARCH_AUTO, 	ROOT_ID_DO_SEARCH,					SCAN_TYPE_AUTO},
		#else
		{IDC_AUTO_SEARCH,	IDS_AUTOL_SEARCH,		IM_XIC_SET_SEARCH_AUTO, 	ROOT_ID_TERRESTRIAL_FULL_SEARCH,	SCAN_TYPE_RANGE},
		#endif
	#else
		#ifdef SEARCH_SUPPORT_FILTER
		{IDC_AUTO_SEARCH,	IDS_AUTOL_SEARCH,		IM_XIC_SET_SEARCH_AUTO, 	ROOT_ID_TERRESTRIAL_MANUAL_SEARCH,	SCAN_TYPE_AUTO},
		#else
		{IDC_AUTO_SEARCH,	IDS_AUTOL_SEARCH,		IM_XIC_SET_SEARCH_AUTO, 	ROOT_ID_DO_SEARCH,					SCAN_TYPE_AUTO},
		#endif
	#endif
	{IDC_MANUAL_SEARCH, IDS_MANUAL_SEARCH,		IM_XIC_SET_SEARCH_MANUAL,	ROOT_ID_TERRESTRIAL_MANUAL_SEARCH,	SCAN_TYPE_MANUAL},
	{IDC_RANGE_SEARCH,	IDS_RANGE_SEARCH,		IM_XIC_SET_SEARCH_ALL,		ROOT_ID_TERRESTRIAL_FULL_SEARCH,	SCAN_TYPE_RANGE},
#else
	#ifdef  SUPPORT_POWER_FULL_SEARCH
	{IDC_AUTO_SEARCH,	IDS_AUTOL_SEARCH, 		IM_XIC_SET_SEARCH_AUTO, 	ROOT_ID_XSEARCH, 			SCAN_TYPE_RANGE},
	#else
	{IDC_AUTO_SEARCH,	IDS_AUTOL_SEARCH, 		IM_XIC_SET_SEARCH_AUTO, 	ROOT_ID_XSEARCH, 			SCAN_TYPE_AUTO},
	#endif
	{IDC_MANUAL_SEARCH,	IDS_MANUAL_SEARCH, 		IM_XIC_SET_SEARCH_MANUAL, 	ROOT_ID_XSEARCH, 			SCAN_TYPE_MANUAL},
	{IDC_RANGE_SEARCH,	IDS_RANGE_SEARCH, 		IM_XIC_SET_SEARCH_ALL, 		ROOT_ID_XSEARCH, 			SCAN_TYPE_RANGE},
#endif
	{IDC_FREQUENCY,		IDS_FREQUENCY_SETTING, 	IM_XIC_SET_SYS_FREQUENCY,	ROOT_ID_FREQ_SET, 			0},
	{IDC_WIFI,			IDS_WIFI, 				IM_XIC_SET_NET_WIFI, 		ROOT_ID_WIFI, 				0},
	{IDC_LAN,			IDS_LAN, 				IM_XIC_SET_NET_LINE, 		ROOT_ID_NETWORK_CONFIG_LAN, 0},
#ifdef DVB_CA
	{IDC_CAINFO,				IDS_CA_INFORMATION, 	IM_XIC_SET_PER_CA, 			ROOT_ID_CA_CARD_INFO, 					0}, 
	{IDC_CA_PWD_MODIFY,			IDS_CA_PWD_MODIFY, 		IM_XIC_SET_PER_LOCK, 		ROOT_ID_CONDITIONAL_ACCEPT_PIN, 		0}, 
      {IDC_CA_WOR_DURATION_SET,	IDS_CA_WOR_DURATION_SET,IM_IC_SET_TIME, 			ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, 	0}, 
      {IDC_CA_LEVEL_CONTROL_SET,	IDS_CA_LEVEL_CONTROL_SET,IM_XIC_SET_SYS_FREQUENCY, 	ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, 		0}, 
      {IDC_CA_PAIR_STATE,			IDS_CA_STB_PAIR, 		IM_XIC_SET_PER_CA, 			ROOT_ID_CONDITIONAL_ACCEPT_PAIR, 		0}, 
      {IDC_CA_OPERATOR,			IDS_3G_OPERATOR, 		IM_XIC_SET_SEARCH_MANUAL, 	ROOT_ID_SP_LIST2, 						0}, 
	{IDC_CA_EMAIL,				IDS_CA_EMAIL, 			IM_IC_SET_EMAIL, 			ROOT_ID_EMAIL_MESS, 					0}, 
	{IDC_MORE,					IDS_MORE, 				IM_IC_SET_MORE, 			ROOT_ID_INVALID, 						0},
#elif SZXC_CA
	{IDC_CAINFO,				IDS_CA_INFORMATION, 	IM_XIC_SET_PER_CA, 			ROOT_ID_CA_CARD_INFO, 					0}, 
	{IDC_CA_MOTHER,				IDS_CARD_MANAGE, 		IM_XIC_SET_PER_CA, 		ROOT_ID_CA_MOTHER, 						0}, 
    {IDC_CA_EMAIL,				IDS_CA_EMAIL,			IM_IC_SET_EMAIL, 			ROOT_ID_EMAIL_MESS, 					0},
    {IDC_LOCK,					IDS_PARENTAL_LOCK, 		IM_XIC_SET_PER_LOCK, 		ROOT_ID_XPARENTAL_LOCK, 				0},
	{IDC_DISPLAY,				IDS_AV_SETTING, 		IM_XIC_SET_DEV_DIS, 		ROOT_ID_XTVSYS_SET, 					0},
	{IDC_SYSINFO,				IDS_INFO, 				IM_XIC_SET_SYS_INFO, 		ROOT_ID_XINFO, 							0},
    {IDC_MORE,					IDS_MORE, 				IM_IC_SET_MORE, 			ROOT_ID_INVALID, 						0},
#elif DESAI_52_CA
	{IDC_CAINFO,				IDS_CA_INFORMATION, 	IM_XIC_SET_PER_CA, 			ROOT_ID_CA_CARD_INFO, 					0}, 	
	{IDC_CA_PWD_MODIFY,			IDS_CA_PWD_MODIFY, 		IM_XIC_SET_PER_LOCK, 		ROOT_ID_CONDITIONAL_ACCEPT_PIN, 		0}, 
	{IDC_CA_WOR_DURATION_SET,	IDS_CA_WOR_DURATION_SET,IM_IC_SET_TIME, 			ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME, 	0}, 
	{IDC_CA_LEVEL_CONTROL_SET,	IDS_CA_LEVEL_CONTROL_SET,IM_XIC_SET_SYS_FREQUENCY, 	ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, 		0}, 
      {IDC_CA_PAIR_STATE,		IDS_CA_SLOT_INFO, 		IM_XIC_SET_PER_CA, 			ROOT_ID_CA_ANTIAUTH_INFO, 		0}, 
	{IDC_CA_OPERATOR,			IDS_3G_OPERATOR, 		IM_XIC_SET_SEARCH_MANUAL, 	ROOT_ID_CA_OPERATOR_INFO, 						0}, 
	{IDC_CA_EMAIL,				IDS_CA_EMAIL, 			IM_IC_SET_EMAIL, 			ROOT_ID_EMAIL_MESS, 					0}, 
	{IDC_MORE,					IDS_MORE, 				IM_IC_SET_MORE, 			ROOT_ID_INVALID, 						0},
#elif TF_CA
	{IDC_CAINFO,				IDS_CA_INFORMATION, 	IM_XIC_SET_PER_CA,			ROOT_ID_CA_CARD_INFO,					0}, 	
	{IDC_CA_PWD_MODIFY, 		IDS_CA_PWD_MODIFY,		IM_XIC_SET_PER_LOCK,		ROOT_ID_CONDITIONAL_ACCEPT_PIN, 		0}, 
	{IDC_CA_WOR_DURATION_SET,	IDS_CA_WOR_DURATION_SET,IM_IC_SET_TIME, 			ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME,	0}, 
	{IDC_CA_LEVEL_CONTROL_SET,	IDS_CA_LEVEL_CONTROL_SET,IM_XIC_SET_SYS_FREQUENCY,	ROOT_ID_CONDITIONAL_ACCEPT_LEVEL,		0}, 
	{IDC_CA_PAIR_STATE,			IDS_CARD_MANAGE,		IM_XIC_SET_PER_CA,			ROOT_ID_CONDITIONAL_ACCEPT_MOTHER,		0}, 
	{IDC_CA_OPERATOR,			IDS_3G_OPERATOR,		IM_XIC_SET_SEARCH_MANUAL,	ROOT_ID_CA_OPE,						0}, 
	{IDC_CA_EMAIL,				IDS_CA_EMAIL,			IM_IC_SET_EMAIL,			ROOT_ID_EMAIL_MESS, 					0}, 
	{IDC_MORE,					IDS_MORE,				IM_IC_SET_MORE, 			ROOT_ID_INVALID,						0},	
#elif TR_CA
	{IDC_CAINFO,					IDS_CA_INFORMATION, 			IM_XIC_SET_PER_CA, 			ROOT_ID_CA_CARD_INFO, 					0}, 	
	{IDC_CA_LEVEL_CONTROL_SET,	IDS_CA_LEVEL_CONTROL_SET, 	IM_XIC_SET_PER_LOCK, 		ROOT_ID_TR_LEVEL, 		0}, 
	{IDC_CA_PWD_PIN,			IDS_CA_PWD_MODIFY,			IM_IC_SET_TIME, 				ROOT_ID_TR_PIN, 	0}, 
	{IDC_CA_OPERATOR,			IDS_3G_OPERATOR,			IM_XIC_SET_SYS_FREQUENCY, 	ROOT_ID_CA_OPE, 		0}, 
      {IDC_CA_FEED,				IDS_CARD_MANAGE, 			IM_XIC_SET_PER_CA, 			ROOT_ID_TR_FEED, 		0}, 
	{IDC_CA_ALTER_MESS,			IDS_ALERT_MESSAGE, 			IM_XIC_SET_SEARCH_MANUAL, 	ROOT_ID_ALERT_MESS, 						0}, 
	{IDC_CA_EMAIL,				IDS_CA_EMAIL, 				IM_IC_SET_EMAIL, 				ROOT_ID_EMAIL_MESS, 					0}, 
	{IDC_MORE,					IDS_MORE, 					IM_IC_SET_MORE, 				ROOT_ID_INVALID, 						0},
#elif SHUSHITONG_CA
	{IDC_CAINFO,				IDS_CA_INFORMATION, 		IM_XIC_SET_PER_CA, 			ROOT_ID_CA_CARD_INFO, 					0},
	{IDC_CA_LEVEL_CONTROL_SET,	IDS_CA_LEVEL_CONTROL_SET,	IM_XIC_SET_SYS_FREQUENCY,  	ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, 	  0}, 
	{IDC_CA_WOR_DURATION_SET, 	IDS_CA_WOR_DURATION_SET,	IM_IC_SET_TIME,			  	ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME,	  0}, 
	{IDC_CA_PWD_MODIFY,			IDS_CA_PWD_MODIFY, 			IM_XIC_SET_PER_LOCK, 		ROOT_ID_CONDITIONAL_ACCEPT_PIN, 		0},
	{IDC_CA_FEED,				IDS_SUB_CARD_FEEDING, 		IM_XIC_SET_PER_CA, 			ROOT_ID_CA_FEED, 		0},
	{IDC_CA_ENTITLE_INFO,		IDS_CA_ACCREDIT_PRODUCK_INFO,IM_XIC_SET_SEARCH_MANUAL,			ROOT_ID_CA_ENTITLE_INFO,		0}, 
	{IDC_CA_EMAIL,				IDS_CA_EMAIL,				IM_IC_SET_EMAIL,			ROOT_ID_EMAIL_MESS, 					0}, 
	{IDC_MORE,					IDS_MORE, 					IM_IC_SET_MORE, 			ROOT_ID_INVALID, 						0},	
#elif SMSX_CA
	{IDC_CAINFO,				IDS_CA_INFORMATION, 		IM_XIC_SET_PER_CA, 			ROOT_ID_CONDITIONAL_ACCEPT, 					0},
	//{IDC_CAINFO,				IDS_CA_INFORMATION, 		IM_XIC_SET_PER_CA, 			ROOT_ID_CA_CARD_INFO, 					0},
	//{IDC_CA_ENTITLE_INFO,		IDS_CA_ACCREDIT_PRODUCK_INFO,IM_XIC_SET_SEARCH_MANUAL,			ROOT_ID_CA_OPE,		0}, 
	//{IDC_CA_PWD_MODIFY,			IDS_CA_PWD_MODIFY, 			IM_XIC_SET_PER_LOCK, 		ROOT_ID_CONDITIONAL_ACCEPT_PIN, 		0},
	//{IDC_CA_LEVEL_CONTROL_SET,	IDS_CA_LEVEL_CONTROL_SET,	IM_XIC_SET_SYS_FREQUENCY,  	ROOT_ID_CONDITIONAL_ACCEPT_LEVEL, 	  0},
	//{IDC_CA_WOR_DURATION_SET, 	IDS_CA_WOR_DURATION_SET,	IM_IC_SET_TIME,			  	ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME,	  0},
	//{IDC_CA_FEED,				IDS_CA_CARD_PAIR, 		IM_XIC_SET_PER_CA, 			ROOT_ID_CONDITIONAL_ACCEPT_FEED, 		0},
	//{IDC_CA_EMAIL,				IDS_CA_EMAIL,				IM_IC_SET_EMAIL,			ROOT_ID_EMAIL_MESS, 					0}, 
	{IDC_MORE,					IDS_MORE, 					IM_IC_SET_MORE, 			ROOT_ID_INVALID, 						0},	
#else	
	{IDC_CAINFO,				IDS_CA_INFORMATION, 	IM_XIC_SET_PER_CA,			ROOT_ID_CA_CARD_INFO,					0}, 
	{IDC_CA_EMAIL,				IDS_CA_EMAIL, 			IM_IC_SET_EMAIL, 			ROOT_ID_EMAIL_MESS, 					0}, 
	#ifdef QUANZHI_CA
	{IDC_CA_ENTITLE_INFO,IDS_CARD_MANAGE, IM_XIC_SET_PER_CA,	ROOT_ID_CONDITIONAL_ACCEPT_FEED,0}, 
	#else
	{IDC_CA_ENTITLE_INFO,IDS_CA_ACCREDIT_PRODUCK_INFO,IM_XIC_SET_PER_CA,	ROOT_ID_CA_ENTITLE_INFO,		0}, 
	#endif
	{IDC_LOCK,					IDS_PARENTAL_LOCK, 		IM_XIC_SET_PER_LOCK, 		ROOT_ID_XPARENTAL_LOCK, 				0},
	{IDC_DISPLAY,				IDS_AV_SETTING, 		IM_XIC_SET_DEV_DIS, 		ROOT_ID_XTVSYS_SET, 					0},
	{IDC_SYSINFO,				IDS_INFO, 				IM_XIC_SET_SYS_INFO, 		ROOT_ID_XINFO, 							0},
	{IDC_MORE,					IDS_MORE,				IM_IC_SET_MORE, 			ROOT_ID_INVALID,						0},
#endif

//page2
	{IDC_UPDATE,				IDS_UPGRADE_BY_USB, 	IM_XIC_SET_SYS_UPDATE, 		ROOT_ID_XUPGRADE_BY_USB, 				0},
	{IDC_USB_DUMP,				IDS_DUMP_BY_USB, 		IM_IC_SET_MORE_DUMP, 		ROOT_ID_DUMP_BY_USB, 					0},
	{IDC_USB_HDD_INFO,			IDS_HDD_INFO, 			IM_USB_INFO, 				ROOT_ID_HDD_INFO,						0},
	{IDC_USB_FORMAT,			IDS_STORAGE_FORMAT, 	IM_USB_MODEL, 				ROOT_ID_STORAGE_FORMAT,					0},
	//{IDC_USB_DVR_CFG,	IDS_DVR_CONFIG, 		IM_USB_SET, 				ROOT_ID_DVR_CONFIG,			0},
	{IDC_USB_JUMP,				IDS_JUMP, 				IM_USB_JUMP, 				ROOT_ID_JUMP,							0},
	
#if defined DVB_CA || defined DESAI_52_CA || defined TR_CA ||defined SHUSHITONG_CA ||defined TF_CA ||defined SMSX_CA
	{IDC_LOCK,					IDS_PARENTAL_LOCK, 		IM_XIC_SET_PER_LOCK, 		ROOT_ID_XPARENTAL_LOCK, 				0},
	{IDC_DISPLAY,				IDS_AV_SETTING, 		IM_XIC_SET_DEV_DIS, 		ROOT_ID_XTVSYS_SET, 					0},
	{IDC_SYSINFO,				IDS_INFO, 				IM_XIC_SET_SYS_INFO, 		ROOT_ID_XINFO, 							0},
#endif
#ifdef AUTH_SERVER_SETTING
	{IDC_AUTH_SETTING,			IDS_AUTH_SETTING, 		IM_IC_SET_MORE_NAME, 		ROOT_ID_AUTH_SERVER_SETTING,	 		0},
#else
	#ifdef AUPPORT_TS_RECORD
	{IDC_RECORD_MANAGER,		IDS_RECORD_MANAGER, 	IM_IC_SET_MORE_RECORD, 		ROOT_ID_TS_RECORD, 						0},
	#else
	{IDC_STB_NAME_SET,			IDS_STB_NAME_SET, 		IM_IC_SET_MORE_NAME, 		ROOT_ID_XSTB_NAME, 						0},
	#endif
#endif
	{IDC_TIME,					IDS_TIME_SET, 			IM_XIC_SET_SYS_TIME, 		ROOT_ID_XTIME_SET, 						0},
	{IDC_TIMER_SET, 			IDS_TIMER_SET, 			IM_XIC_SET_SYS_TIME, 		ROOT_ID_BOOK_LIST, 						0},
	{IDC_FACTORY_SET,			IDS_FACTORY_SET, 		IM_IC_SET_MORE_FAC, 		ROOT_ID_INVALID, 						0},
	{IDC_LANGUAGE,				IDS_LANGUAGE, 			IM_XIC_SET_PER_LANGUAGE, 	ROOT_ID_XLANGUAGE, 						0}, 
	{IDC_UPGRADE_BY_NET,		IDS_UPGRADE_BY_NET,		IM_XIC_SET_SYS_UPDATE,	    ROOT_ID_NET_UPG,						0},
	{IDC_BACK,					IDS_BACK, 				IM_IC_SET_MORE_BACK, 		ROOT_ID_INVALID,						0},
};

extern u16 g_ts_jump_select;

u16 ui_sys_set_btn_cont_keymap(u16 key);
RET_CODE ui_sys_set_btn_cont_proc(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);
u16 ui_sys_set_cont_keymap(u16 key);
RET_CODE ui_sys_set_cont_proc(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);

u16 sys_pwdlg_keymap(u16 key);
RET_CODE sys_pwdlg_proc(control_t *ctrl,u16 msg,u32 para1,u32 para2);

static BOOL get_idex_by_btn_id(u8 btn_id, u16 *idex)
{
	u16 i;
	for(i = 0; i<(IDC_BTN_CNT-IDC_CONT_CNT); i++)
	{
		if(btn_id==btn_attr_with_root[i].btn_id)
		{
			*idex = i;
			return TRUE;
		}
	}

	return FALSE;
}
/*
static BOOL get_idex_by_root_id(u16 root_id, u16 *idex, u32 para1)
{
	u16 i;
	for(i = 0; i<(IDC_BTN_CNT-IDC_CONT_CNT); i++)
	{
		if(root_id==btn_attr_with_root[i].root_id)
		{
			if(ROOT_ID_XSEARCH==root_id)
			{
				if(para1==btn_attr_with_root[i].para)
				{
					*idex = i;
					return TRUE;
				}
				else
					continue;
			}
			*idex = i;
			return TRUE;
		}
	}

	return FALSE;
}
*/
void sys_set_paint_left_cont(void)
{
	control_t *p_ctrl, *p_cont, *p_title;
	p_ctrl = fw_find_root_by_id(ROOT_ID_XSYS_SET);
	if(NULL==p_ctrl)
		return;
	p_cont = ctrl_get_child_by_id(p_ctrl, IDC_LEFT_CONT1);
	if(OBJ_STS_SHOW!=ctrl_get_sts(p_cont))
		p_cont = ctrl_get_child_by_id(p_ctrl, IDC_LEFT_CONT2);

	p_title = ctrl_get_child_by_id(p_ctrl, IDC_SYS_SET_TITLE);
	ctrl_paint_ctrl(p_cont, TRUE);
	ctrl_paint_ctrl(p_title, TRUE);
}

RET_CODE open_xsys_set(u32 para1, u32 para2)
{
	control_t *p_cont, *p_ctrl, *p_left_cont1, *p_left_cont2, *p_left_cont;
	control_t *p_title_bmap, *p_title_text;
	u16 i,y;
	u16 btn_idex;
#if defined DVB_CA || defined DESAI_52_CA || defined TR_CA || SHUSHITONG_CA  ||defined TF_CA //||defined SMSX_CA
	u16 type_btn_cnt[IDC_TYPE_CNT-IDC_CNT] = {4,2,8,5,10};
#elif SMSX_CA
	u16 type_btn_cnt[IDC_TYPE_CNT-IDC_CNT] = {4,2,2,5,10};
#else
	u16 type_btn_cnt[IDC_TYPE_CNT-IDC_CNT] = {4,2,3,4,5,7};
#endif
	u16 type_y = 0;
	u16 next_id, prev_id;
	u16 type_ids_item[IDC_TYPE_CNT-IDC_CNT] = {
#if defined DVB_CA || defined DESAI_52_CA || defined TR_CA ||defined SHUSHITONG_CA ||defined TF_CA ||defined SMSX_CA
		IDS_SEARCH,IDS_NETWORK,IDS_CA,IDS_USB_SETTING,IDS_SYSTEM,
#else
		IDS_SEARCH,IDS_NETWORK,IDS_CA,IDS_SYSTEM,IDS_USB_SETTING,IDS_MORE,
#endif
	};
	
	p_cont = fw_create_mainwin(ROOT_ID_XSYS_SET,
							   SYS_SET_X, SYS_SET_Y,
							   SYS_SET_W, SYS_SET_H,
							   ROOT_ID_INVALID, 0,
							   OBJ_ATTR_ACTIVE, 0);
	if(p_cont == NULL)
	{
	  return ERR_FAILURE;
	}	
	ctrl_set_keymap(p_cont, ui_sys_set_cont_keymap);
	ctrl_set_proc(p_cont, ui_sys_set_cont_proc);
	ctrl_set_style(p_cont, STL_EX_WHOLE_HL);

	//title
	p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_SYS_SET_TITLE,
											0, 0,
											SYS_SET_W, SYS_TITLE_H,
											p_cont, 0);
	ctrl_set_rstyle(p_ctrl,RSI_LEFT_CONT_BG,RSI_LEFT_CONT_BG,RSI_LEFT_CONT_BG);
	
	p_title_bmap = ctrl_create_ctrl(CTRL_BMAP, IDCSYS_SET_ICO,
											SYS_SET_ICO_X, SYS_TITLE_H-SYS_SET_ICO_H,
											SYS_SET_ICO_W, SYS_SET_ICO_H,
											p_ctrl, 0);
	bmap_set_content_by_id(p_title_bmap, IM_IC_SET);


	p_title_text= ctrl_create_ctrl(CTRL_TEXT, IDCSYS_SET_TEXT,SYS_SET_TEXT_X, SYS_TITLE_H - SYS_SET_TEXT_H,
										SYS_SET_TEXT_W, SYS_SET_TEXT_H,
										p_ctrl, 0);
	MT_ASSERT(p_title_text != NULL);
	ctrl_set_rstyle(p_title_text,RSI_LEFT_CONT_BG,RSI_LEFT_CONT_BG,RSI_LEFT_CONT_BG);
	text_set_font_style(p_title_text, FSI_WHITE_36, FSI_WHITE_36, FSI_WHITE_36);
	text_set_align_type(p_title_text, STL_LEFT | STL_VCENTER);
	text_set_content_type(p_title_text, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_title_text, IDS_SETUP);
	
	//title bar
	p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_SYS_SET_TITLE_BAR,
											0, SYS_TITLE_H,
											SYS_SET_W, SYS_BAR_H,
											p_cont, 0);
	ctrl_set_rstyle(p_ctrl,RSI_TITLE_BAR,RSI_TITLE_BAR,RSI_TITLE_BAR);

	//left cont page1 bg
	p_left_cont1 = ctrl_create_ctrl(CTRL_CONT, IDC_LEFT_CONT1,
											0, SYS_LEFT_CONT_Y,
											SYS_SET_W-SYS_RIGHT_CONT_W, SYS_LEFT_CONT_H,
											p_cont, 0);
	ctrl_set_rstyle(p_left_cont1,RSI_LEFT_CONT_BG,RSI_LEFT_CONT_BG,RSI_LEFT_CONT_BG);
	//left cont page2 bg
	p_left_cont2 = ctrl_create_ctrl(CTRL_CONT, IDC_LEFT_CONT2,
											0, SYS_LEFT_CONT_Y,
											SYS_SET_W-SYS_RIGHT_CONT_W, SYS_LEFT_CONT_H,
											p_cont, 0);
	ctrl_set_rstyle(p_left_cont2,RSI_LEFT_CONT_BG,RSI_LEFT_CONT_BG,RSI_LEFT_CONT_BG);
	ctrl_set_sts(p_left_cont2, OBJ_STS_HIDE);

	//left option
	btn_idex = 0;
	for(y = 0; y<(IDC_TYPE_CNT-IDC_CNT);y++)
	{
		if(y<(IDC_USB-IDC_CNT))
		{
			type_y = btn_idex*SYS_OPT_TEXT_H+y*SYS_SET_TYPE_H;
			p_left_cont = p_left_cont1;
		}
		else
		{
			type_y = (btn_idex-SYS_SET_BTN_ONEPAGE)*SYS_OPT_TEXT_H+(y-(IDC_USB-IDC_CNT))*SYS_SET_TYPE_H;
			p_left_cont = p_left_cont2;
		}
		p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_SEARCH_TYPE+y,
									SYS_SET_TYPE_X, type_y,
									SYS_SET_TYPE_W, SYS_SET_TYPE_H,
									p_left_cont, 
									0);
		MT_ASSERT(p_ctrl != NULL);
		ctrl_set_rstyle(p_ctrl,RSI_OPT_BTN,RSI_OPT_BTN_H,RSI_OPT_BTN);
		ctrl_set_keymap(p_ctrl, ui_sys_set_btn_cont_keymap);
		ctrl_set_proc(p_ctrl, ui_sys_set_btn_cont_proc);
		text_set_font_style(p_ctrl, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
		text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
		text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_ctrl, type_ids_item[y]);
		text_set_offset(p_ctrl, 35,0);
		for(i = 0; i<type_btn_cnt[y];i++)
		{
		        OS_PRINTF("[debug] type_btn_cnt[%d]  i[%d]\n",type_btn_cnt[y],i);
			p_ctrl = ctrl_create_ctrl(CTRL_TEXT, btn_attr_with_root[btn_idex].btn_id,
												(SYS_LEFT_CONT_W-SYS_OPT_TEXT_W)/2, type_y+SYS_SET_TYPE_H+i*SYS_OPT_TEXT_H,
												SYS_OPT_TEXT_W, SYS_OPT_TEXT_H,
												p_left_cont, 0);
			MT_ASSERT(p_ctrl != NULL);
			ctrl_set_rstyle(p_ctrl,RSI_OPT_BTN,RSI_OPT_BTN_H,RSI_ITEM_8_BG);
			ctrl_set_keymap(p_ctrl, ui_sys_set_btn_cont_keymap);
			ctrl_set_proc(p_ctrl, ui_sys_set_btn_cont_proc);
			text_set_font_style(p_ctrl, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
			text_set_align_type(p_ctrl, STL_LEFT | STL_VCENTER);
			text_set_content_type(p_ctrl, TEXT_STRTYPE_STRID);
			text_set_content_by_strid(p_ctrl, btn_attr_with_root[btn_idex].btn_str_id);
			text_set_offset(p_ctrl, SYS_BTN_ICO_W+60,0);
			if(0==btn_idex)
				prev_id = btn_attr_with_root[SYS_SET_BTN_ONEPAGE-1].btn_id;
			else if(SYS_SET_BTN_ONEPAGE==btn_idex)
				prev_id = btn_attr_with_root[IDC_BTN_CNT-IDC_CONT_CNT-1].btn_id;
			else
				prev_id = btn_attr_with_root[btn_idex-1].btn_id;
			if((SYS_SET_BTN_ONEPAGE-1)==btn_idex)
				next_id = btn_attr_with_root[0].btn_id;
			else if((IDC_BTN_CNT-IDC_CONT_CNT-1)==btn_idex)
				next_id = btn_attr_with_root[SYS_SET_BTN_ONEPAGE].btn_id;
			else
				next_id = btn_attr_with_root[btn_idex+1].btn_id;
			
			ctrl_set_related_id(p_ctrl, 0, prev_id, 0, next_id);
			p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_BTN_ICON,
												40, 0,
												SYS_BTN_ICO_W, SYS_OPT_TEXT_H,
												p_ctrl, 0);
			bmap_set_content_by_id(p_ctrl, btn_attr_with_root[btn_idex].btn_bmp_id);
			btn_idex++;
		}
	}
	p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_RIGHT_CONT,
							   SYS_LEFT_CONT_W, SYS_LEFT_CONT_Y,
							   RIGHT_ROOT_W, RIGHT_ROOT_H,
							   p_cont, 0);
	MT_ASSERT(p_ctrl != NULL);
	ctrl_set_rstyle(p_ctrl,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG);
	ctrl_default_proc(ctrl_get_child_by_id(p_left_cont1, IDC_AUTO_SEARCH), MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(p_cont,FALSE);
    manage_open_menu(ROOT_ID_XSTB_HELP,0,OBJ_ATTR_INACTIVE);
	return SUCCESS;
}

static RET_CODE sys_set_focus_change(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	
	control_t *p_parent, *p_next;

	p_parent = ctrl_get_parent(p_ctrl);
	switch(msg)
	{
		case MSG_FOCUS_UP:
			p_next = ctrl_get_child_by_id(p_parent, p_ctrl->up);
			break;
		case MSG_FOCUS_DOWN:
			p_next = ctrl_get_child_by_id(p_parent, p_ctrl->down);
			break;
		default:
			return ERR_FAILURE;
			break;
	}	
	ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0); 
	ctrl_process_msg(p_next, MSG_GETFOCUS, 0, 0); 
	ctrl_paint_ctrl(p_ctrl,FALSE);
	ctrl_paint_ctrl(p_next,FALSE);
	return SUCCESS;
}
static control_t *get_sys_set_active_btn(void)
{
	control_t *p_cont, *p_ctrl;
	control_t *p_btn = NULL;
	
	p_cont = fw_find_root_by_id(ROOT_ID_XSYS_SET);
	if(NULL==p_cont)
	  return NULL;
	p_ctrl = ctrl_get_child_by_id(p_cont, IDC_LEFT_CONT1);
	if(OBJ_STS_SHOW==ctrl_get_sts(p_ctrl))
	{
	  p_btn = ctrl_get_active_ctrl(p_ctrl);
	}
	else
	{
	  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_LEFT_CONT2);
	  p_btn = ctrl_get_active_ctrl(p_ctrl);
	}
	return p_btn;
}
RET_CODE swtich_to_sys_set(u16 old_root_id, u32 para)
{
	control_t *p_btn;
	control_t *p_ctrl;
	p_ctrl = fw_find_root_by_id(ROOT_ID_XSYS_SET);
	if(NULL==p_ctrl)
		return ERR_FAILURE;
	p_btn = get_sys_set_active_btn();
	if(NULL==p_btn)
		return ERR_FAILURE;
	ctrl_set_attr(p_btn, OBJ_ATTR_HL);
	ctrl_paint_ctrl(p_btn, TRUE);
	return SUCCESS;
}

static BOOL set_check_usb_stat(void)
{
  return (BOOL)(ui_get_usb_status());
}

static RET_CODE show_dlg(s16 x,s16 y,s16 w,s16 h,s16 time,u16 text)
{
	rect_t rec;
	rec.left = x;
	rec.right = x+w;
	rec.top = y;
	rec.bottom = y+h;
	ui_comm_cfmdlg_open(&rec, text, NULL, time);	
	return SUCCESS;
}

static void _restore_to_factory(void)
{
  u16 view_id;
  utc_time_t  p_time;
  ip_address_set_t ip_addr = {{0}};
  net_conn_stats_t wifi_connt_stats = {0};

  comm_dlg_data_t wifi_dlg_data  =
  {
    ROOT_ID_XSYS_SET,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    WIFI_DLG_X, WIFI_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    IDS_DISCONNECTING_WIFI,
    5000,
  };   

  ui_stop_play(STOP_PLAY_BLACK, TRUE);
  //reset timeshift
  g_ts_jump_select = 0;
  // set front panel
  ui_set_front_panel_by_str("----");

  //discon wifi
  wifi_connt_stats = ui_get_net_connect_status();
  if(wifi_connt_stats.is_wifi_conn)
  {
    dis_conn_wifi(&wifi_dlg_data);
  }
  
  //set default use common buffer as ext buffer
  db_dvbs_restore_to_factory(PRESET_BLOCK_ID, BLOCK_EPG_BUFFER);

  sys_status_set_ipaddress(&ip_addr);  //fix bug 38656
  sys_status_load();
  sys_status_set_ipaddress(&ip_addr); 
  
  sys_status_check_group();
  sys_status_save();

  sys_status_resume_ca_key();


  // set environment according ss_data
  sys_status_reload_environment_setting();

  sys_status_get_utc_time( &p_time);
  time_set(&p_time);
  if (ui_is_mute())
  {
    ui_set_mute(FALSE);
  }
  if (ui_is_pause())
  {
    ui_set_pause(FALSE);
  }
  if (ui_is_notify())
  {
    ui_set_notify(NULL, NOTIFY_TYPE_STRID, RSC_INVALID_ID);
  }

  //create a new view after load default, and save the new view id.
  view_id = ui_dbase_create_view(DB_DVBS_ALL_TV, 0, NULL);
  ui_dbase_set_pg_view_id((u8)view_id);

  /* clear history */
  ui_clear_play_history();
  #ifdef SHUSHITONG_CA
  if(CUSTOMER_ID == CUSTOMER_DTMB_CHANGSHA_HHT)
  {
    set_have_valid_freq_flag(FALSE);
  }
  #endif

}

static void restore_to_factory(void)
{
	control_t *p_ctrl;
	_restore_to_factory();
	
	p_ctrl = fw_find_root_by_id(ROOT_ID_XSYS_SET);
	ctrl_paint_ctrl(p_ctrl, TRUE);
}

static RET_CODE on_open_child_menu(control_t *p_btn,u32 para1, u32 para2)
{
	u16 idex;
	RET_CODE re = ERR_FAILURE;
	
	rect_t restore_dlg_rc =
	{
		RESTORE_ASK_L,
		RESTORE_ASK_T,
		RESTORE_ASK_R,
		RESTORE_ASK_B,
	};
	
	if(!get_idex_by_btn_id(p_btn->id, &idex))
		return ERR_FAILURE;
	if((!set_check_usb_stat())
			&&(IDC_UPDATE==p_btn->id
				||IDC_USB_DUMP==p_btn->id
				||IDC_USB_HDD_INFO==p_btn->id
				||IDC_USB_FORMAT==p_btn->id
				||IDC_USB_JUMP==p_btn->id))
		{
			return show_dlg(SYS_DLG_FOR_CHK_X,SYS_DLG_FOR_CHK_Y,PWDLG_W,PWDLG_H,2000,IDS_USB_DISCONNECT);
		}
	switch(p_btn->id)
	{
		case IDC_UPDATE:
			if(ROOT_ID_INVALID!=btn_attr_with_root[idex].root_id)
			{
				re = manage_open_menu(btn_attr_with_root[idex].root_id, 
										btn_attr_with_root[idex].para, 0);
			}
			break;
		case IDC_FACTORY_SET:
			ui_comm_ask_for_savdlg_open(&restore_dlg_rc,
										IDS_MSG_ASK_FOR_RESTORE_TO_FACTORY,
										restore_to_factory, NULL, 0);
			break;
            case IDC_RANGE_SEARCH:
                   if(CUSTOMER_ID == CUSTOMER_MAIKE_HUNAN_LINLI ||
				   	CUSTOMER_ID == CUSTOMER_DTMB_SHANGSHUIXIAN ||
				   	CUSTOMER_ID == CUSTOMER_DTMB_SZXC_GUOMAO ||
					CUSTOMER_ID == CUSTOMER_DTMB_PCBA_GUOMAO)
                   {
                        re = manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_AUTO, 0);
                   }
                   else
                   {
           			re = manage_open_menu(btn_attr_with_root[idex].root_id, btn_attr_with_root[idex].para, 0);
                   }
                   break;
		default:
			if(ROOT_ID_INVALID!=btn_attr_with_root[idex].root_id)
			{
				re = manage_open_menu(btn_attr_with_root[idex].root_id, 
										btn_attr_with_root[idex].para, 0);
			}
			break;
	}
	return re;
}
static RET_CODE on_sys_pwdlg_correct(control_t *p_pwdlg, u16 msg,u32 para1,u32 para2)
{
	control_t *p_btn;
	RET_CODE re = ERR_FAILURE;

	ui_comm_pwdlg_close();
	p_btn = get_sys_set_active_btn();
	if(NULL==p_btn)
		return ERR_FAILURE;
	switch(p_btn->id)
	{
		default:
			re = on_open_child_menu(p_btn, 0, 0);
			break;
	}
	return re;
}

static RET_CODE sys_set_select(control_t *p_btn, u16 msg,u32 para1, u32 para2)
{
	BOOL is_chk = FALSE;
	control_t *p_ctrl;
	comm_pwdlg_data_t sys_pwdlg_data =
	{
		ROOT_ID_XSYS_SET,
		RESTORE_ASK_L,
		RESTORE_ASK_T,
		IDS_MSG_INPUT_PASSWORD,
		sys_pwdlg_keymap,
		sys_pwdlg_proc,
	};
	RET_CODE re = SUCCESS;
	
	if(IDC_MORE!=p_btn->id&&IDC_BACK!=p_btn->id)
	{
		ctrl_set_attr(p_btn, OBJ_ATTR_INACTIVE);
		ctrl_paint_ctrl(p_btn, TRUE);
	}
	sys_status_get_status(BS_MENU_LOCK, &is_chk);
	if(is_chk  && (p_btn->id != IDC_MORE) && (p_btn->id != IDC_BACK) )
	{
		ui_comm_pwdlg_open(&sys_pwdlg_data);
		return SUCCESS;
	}
	switch(p_btn->id)
	{
	case IDC_FACTORY_SET:
	case IDC_LOCK:
		if(CUSTOMER_ID == CUSTOMER_DTMB_DESAI_JIMO)
		{
			re = on_open_child_menu(p_btn, 0, 0);
		}
		else
		{
			ui_comm_pwdlg_open(&sys_pwdlg_data);
		}
		break;
	case IDC_RANGE_SEARCH:
	case IDC_MANUAL_SEARCH:
	case IDC_FREQUENCY:
		if(CUSTOMER_ID == CUSTOMER_DTMB_DESAI_JIMO)
		{
			sys_pwdlg_data.password_type = PWDLG_T_CUSTMER;
			ui_comm_pwdlg_open(&sys_pwdlg_data);
		}
		else
		{
			on_open_child_menu(p_btn, 0, 0);
		}
		break;
	case IDC_MORE:
	case IDC_BACK:
		p_ctrl = ctrl_get_child_by_id(ctrl_get_root(p_btn), 
							(IDC_MORE==p_btn->id)?IDC_LEFT_CONT2:IDC_LEFT_CONT1);
		ctrl_set_sts(p_btn->p_parent, OBJ_STS_HIDE);
		ctrl_set_sts(p_ctrl, OBJ_STS_SHOW);
		p_ctrl = ctrl_get_child_by_id(p_ctrl, 
			(IDC_MORE==p_btn->id)?(btn_attr_with_root[SYS_SET_BTN_ONEPAGE].btn_id):(btn_attr_with_root[0].btn_id));
		ctrl_process_msg(p_btn, MSG_LOSTFOCUS, 0, 0);
		ctrl_process_msg(p_ctrl, MSG_GETFOCUS, 0, 0);
		ctrl_paint_ctrl(p_btn->p_parent, TRUE);
		ctrl_paint_ctrl(p_ctrl->p_parent, TRUE);
		break;
	default:
		re = on_open_child_menu(p_btn, 0, 0);
		break;
	}

	if(ERR_FAILURE==re)
	{
		ctrl_set_attr(p_btn, OBJ_ATTR_HL);
		ctrl_paint_ctrl(p_btn, TRUE);
	}
	return re;
}

BEGIN_KEYMAP(ui_sys_set_cont_keymap, ui_comm_root_keymap)
END_KEYMAP(ui_sys_set_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ui_sys_set_cont_proc, ui_comm_root_proc)
END_MSGPROC(ui_sys_set_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ui_sys_set_btn_cont_keymap, NULL)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ui_sys_set_btn_cont_keymap, NULL)

BEGIN_MSGPROC(ui_sys_set_btn_cont_proc, text_class_proc)
	ON_COMMAND(MSG_SELECT, sys_set_select)
	ON_COMMAND(MSG_FOCUS_UP, sys_set_focus_change)
	ON_COMMAND(MSG_FOCUS_DOWN, sys_set_focus_change)
END_MSGPROC(ui_sys_set_btn_cont_proc, text_class_proc)

BEGIN_KEYMAP(sys_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(sys_pwdlg_keymap, NULL)
BEGIN_MSGPROC(sys_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_CORRECT_PWD, on_sys_pwdlg_correct)
END_MSGPROC(sys_pwdlg_proc, cont_class_proc)

