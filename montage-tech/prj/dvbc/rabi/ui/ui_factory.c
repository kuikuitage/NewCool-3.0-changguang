#include "ui_common.h"
#include "ui_factory.h"
#include "data_manager.h"
#include "data_manager_v2.h"
#include "smc_op.h"
#include "net_svc.h"
#include "ethernet.h"
#include "wifi.h"
#include "modem.h"
#include "ui_desktop.h"
#include "ui_pause.h"
#include "ui_signal.h"
#include "ui_notify.h"
#include "ui_network_config_wifi.h"
#include "ui_mute.h"
#include "ui_video.h"
#include "smc_pro.h"
#include "ui_ca_public.h"


#ifdef FACTORY_TEST_DEBUG
	#define FACTORY_TEST_PRINTF   mtos_printk
#else
	#define FACTORY_TEST_PRINTF   OS_PRINTF
#endif

enum net_ping_test
{
	NET_PING_INVALID = 0,
	LAN_PING_TEST,
	WIFI_PING_TEST,
};

typedef struct {
	control_t *p_info_list;
	control_t *p_search_list;
	control_t *p_net_list;
	control_t *p_frontpanel_mbox;

	u16 prog_num;
	u16 ping_test;
	s16 lan_ok_times;
	s16 lan_bad_times;
	ethernet_cfg_t ethcfg;

	u16	partition_cnt;
	u32 file_count;
	partition_t *p_partition;

	BOOL scard_insert_state;
	BOOL lan_insert_state;
	BOOL wifi_insert_state;
	BOOL wifi_search_state;
	BOOL read_card;
	BOOL searched_flag;
	u8 wifi_search_count;
	ethernet_device_t* p_wifi_dev;
	
}factory_test_t;

enum factory_test_state
{
	IDN_STATE_INVALID =0,
	IDN_STATE_TITLE,
	IDN_STATE_CONTENT,
	IDN_STATE_OK ,
	IDN_STATE_NO,
};

enum factory_test_field
{
	IDN_FIELD_TITLE =0,
	IDN_FIELD_CONTENT =1,
	IDN_FIELD_STATE =2,
};

enum factory_test
{
	IDC_INFO_CONT = 1,
	IDC_INFO_LIST,
	IDC_SEARCH_CONT,
	IDC_SEARCH_LIST,
	IDC_NET_STATE_CONT,
	IDC_NET_STATE_LIST,
	
	IDC_REMOTE_CONT,
	IDC_REMOTE_TITLE,
	IDC_REMOTE_MBOX,

	IDC_FACTORY_BUTTON_CONT,
	IDC_FACTORY_BUTTON_TEXT,
};

enum factory_test_info
{
	IDN_INFO_SERIAL_NUMBER =0,
	IDN_INFO_MAC_ADDR =1,
	IDN_INFO_OUI_ID =2,
	IDN_INFO_HARD_VERSION =3,
	IDN_INFO_SOFT_VERSION =4,
	IDN_INFO_CA_CARD_STATE =5,
	IDN_INFO_CA_CARD_ID =6,
};

enum factory_test_search
{
	IDN_SEARCH_MAIN_FRE =0,
	IDN_SEARCH_TUNER_SATTE =1,
	IDN_SEARCH_PROGRAM_NUM =2,
	IDN_SEARCH_PROGRESS =3,
};

enum factory_test_net
{
	IDN_NET_LAN_IP_ADDR =0,
	IDN_NET_LAN_CONNECT_STATE =1,
	IDN_NET_LAN_PING_TEST =2,
	IDN_NET_WIFI_SEARCH_NUM =3,
	IDN_NET_USB_STATE =4,
	IDN_NET_USB_PARTITION_1 =5,
	IDN_NET_USB_PARTITION_2 =6,
};

enum factory_test_frontspanel
{
#ifdef FACTORY_TEST_FRONTSPANEL_ALL
	IDN_KEY_POWER = 0,
	IDN_KEY_UP =1,
	IDN_KEY_DOWN =2,
	IDN_KEY_LEFT =3,
	IDN_KEY_RIGHT =4,
	IDN_KEY_OK =5,
	IDN_KEY_MENU =6,
	IDN_KEY_VUP =7,
	IDN_KEY_VDOWN =8,
#else
	IDN_KEY_MENU =0,
	IDN_KEY_OK =1,
	IDN_KEY_DOWN =2,
	IDN_KEY_UP =3,
	IDN_KEY_LEFT =4,
	IDN_KEY_RIGHT =5,
#endif
};

static list_xstyle_t factory_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_xstyle_t factory_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLACK,
  FSI_WHITE,
};

static list_xstyle_t factory_list_field_state_fstyle =
{
  FSI_GRAY,
  FSI_GREEN,
  FSI_GREEN,
  FSI_BLACK,
  FSI_GREEN,
};

static list_field_attr_t factory_list_attr[UI_FACTORY_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR|STL_LEFT,
    170, 10, 0, &factory_list_field_rstyle,  &factory_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR |STL_LEFT,
    220, 180, 0, &factory_list_field_rstyle,  &factory_list_field_fstyle},
  { LISTFIELD_TYPE_ICON|STL_CENTER|STL_VCENTER,
    50, 400, 0, &factory_list_field_rstyle,  &factory_list_field_state_fstyle},
};

extern BOOL ui_is_smart_card_insert(void);
extern BOOL ui_get_smart_card_state(void);
extern void categories_mem_reset(void);
extern BOOL get_ca_card_inset_state(void);
extern BOOL dvbcas_get_sc_insert_status(void);
extern void get_addr_param(u8 *p_buffer, ip_address_t *p_addr);
extern void do_cmd_connect_network(ethernet_cfg_t *ethcfg, ethernet_device_t * eth_dev);
RET_CODE  ui_factory_cont_proc(struct control *p_ctrl, u16 msg,u32 para1, u32 para2);
u16 ui_factory_cont_msg(u16 key);
static u32 factory_test_get_card_id(void);

static void factory_test_set_wifi_info_search(void);
RET_CODE  ui_factory_button_cont_proc(struct control *p_ctrl, u16 msg,u32 para1, u32 para2);
u16 ui_factory_button_cont_msg(u16 key);


static factory_test_t  factory_test;

static void set_text_content_by_ascstr(control_t *p_ctrl, u8 *asc_str)
{
	u16 uni_str[64] = {0};

	convert_gb2312_chinese_asc2unistr(asc_str, uni_str, sizeof(uni_str));
	text_set_content_by_unistr(p_ctrl, uni_str);
}

static void set_mbox_content_by_ascstr(control_t *p_ctrl, u16 item_idx, u8 *asc_str)
{
	u16 uni_str[64] = {0};

	convert_gb2312_chinese_asc2unistr(asc_str, uni_str, sizeof(uni_str));
	mbox_set_content_by_unistr(p_ctrl, item_idx, uni_str);
}

static void set_list_field_content(control_t *p_list, u8 *asc_str, u16 item_idx,u8 state)
{
	u16 uni_str[64] = {0};

	switch(state)
	{
		case IDN_STATE_TITLE:
			convert_gb2312_chinese_asc2unistr(asc_str, uni_str, sizeof(uni_str));
			list_set_field_content_by_unistr(p_list, item_idx, IDN_FIELD_TITLE, uni_str);
			break;
		case IDN_STATE_CONTENT:
			convert_gb2312_chinese_asc2unistr(asc_str, uni_str, sizeof(uni_str));
			list_set_field_content_by_unistr(p_list, item_idx, IDN_FIELD_CONTENT, uni_str);
			break;
		case IDN_STATE_OK:
			convert_gb2312_chinese_asc2unistr(asc_str, uni_str, sizeof(uni_str));
			list_set_field_content_by_unistr(p_list, item_idx, IDN_FIELD_CONTENT, uni_str);

			list_set_field_content_by_icon(p_list, item_idx, IDN_FIELD_STATE, IM_CONFIGCONNECT_2);
			break;
		case IDN_STATE_NO:
			convert_gb2312_chinese_asc2unistr(asc_str, uni_str, sizeof(uni_str));
			list_set_field_content_by_unistr(p_list, item_idx, IDN_FIELD_CONTENT, uni_str);

			list_set_field_content_by_icon(p_list, item_idx, IDN_FIELD_STATE, IM_CONFIGCONNECT_1);
			break;
		default:
			break;
	}
}

control_t *create_factory_info_cont(control_t *p_cont)
{
	control_t *p_info_cont = NULL, *p_info_list = NULL;
	u8 i = 0;
	u16 *p_unstr = NULL;
	u16 uni_str[32] = {0};
	
	u16 strId[] = {
		    IDS_CUR_STBID,
		    IDS_MAC_ADDR,
		    IDS_MANUFACTURE_ID,
		    IDS_HARDWARE_VER,
		    IDS_SOFTWARE_VER,
		    0,
		    IDS_CA_SMART_CARD_NUMBER,
	};

	p_info_cont = ctrl_create_ctrl(CTRL_CONT,  IDC_INFO_CONT, 
		UI_FACTORY_INFO_CONT_X,UI_FACTORY_INFO_CONT_Y,
		UI_FACTORY_INFO_CONT_W,UI_FACTORY_INFO_CONT_H,
		p_cont,(u32)0);
	ctrl_set_rstyle(p_info_cont, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);


	p_info_list = ctrl_create_ctrl(CTRL_LIST, IDC_INFO_LIST,
		UI_FACTORY_INFO_LIST_X,UI_FACTORY_INFO_LIST_Y,
		UI_FACTORY_INFO_LIST_W,UI_FACTORY_INFO_LIST_H,
		p_info_cont,0);
	ctrl_set_rstyle(p_info_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	list_set_count(p_info_list, (u16)UI_FACTORY_INFO_LIST_COUNT, UI_FACTORY_INFO_LIST_COUNT);
	list_set_field_count(p_info_list, UI_FACTORY_LIST_FIELD, UI_FACTORY_INFO_LIST_COUNT);

	for (i = 0; i < UI_FACTORY_LIST_FIELD; i++)
	{
		list_set_field_attr(p_info_list, (u8)i, (u32)(factory_list_attr[i].attr), (u16)(factory_list_attr[i].width),
				(u16)(factory_list_attr[i].left), (u16)(factory_list_attr[i].top));
		list_set_field_rect_style(p_info_list, (u8)i, factory_list_attr[i].rstyle);
		list_set_field_font_style(p_info_list, (u8)i, factory_list_attr[i].fstyle);
	}

	for(i =0; i<UI_FACTORY_INFO_LIST_COUNT; i++)
	{
		switch(i)
		{
			case IDN_INFO_MAC_ADDR:
				set_list_field_content(p_info_list, "MAC地址:", i, IDN_STATE_TITLE);
				break;
			case IDN_INFO_CA_CARD_STATE:
				set_list_field_content(p_info_list, "智能卡状态:", i, IDN_STATE_TITLE);
				break;
			case IDN_INFO_CA_CARD_ID:
				set_list_field_content(p_info_list, "智能卡卡号:", i, IDN_STATE_TITLE);
				break;
			default:
				if(i > sizeof(strId)/sizeof(strId[0]))
					break;
				p_unstr = (u16 *)gui_get_string_addr(strId[i]);
				uni_strcpy(uni_str, p_unstr);
				list_set_field_content_by_unistr(p_info_list, i, 0, uni_str);
				break;
		}
	}

	factory_test.p_info_list = p_info_list;
	return p_info_cont;
}


control_t *create_factory_search_cont(control_t *p_cont)
{
	control_t *p_search_cont = NULL, *p_search_list = NULL;
	u8 i = 0;
	u8 asc_str[][64] = {
		{"主频点:"},{ "TUNER状态:"},{"节目数:"},{"搜索进度:"}
	};

	p_search_cont = ctrl_create_ctrl(CTRL_CONT,  IDC_SEARCH_CONT, 
		UI_FACTORY_SEARCH_CONT_X,UI_FACTORY_SEARCH_CONT_Y,
		UI_FACTORY_SEARCH_CONT_W,UI_FACTORY_SEARCH_CONT_H,
		p_cont,(u32)0);
	ctrl_set_rstyle(p_search_cont, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);


	p_search_list = ctrl_create_ctrl(CTRL_LIST, IDC_SEARCH_LIST,
		UI_FACTORY_SEARCH_LIST_X,UI_FACTORY_SEARCH_LIST_Y,
		UI_FACTORY_SEARCH_LIST_W,UI_FACTORY_SEARCH_LIST_H,
		p_search_cont,0);
	ctrl_set_rstyle(p_search_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	list_set_count(p_search_list, (u16)UI_FACTORY_SEARCH_LIST_COUNT, UI_FACTORY_SEARCH_LIST_COUNT);
	list_set_field_count(p_search_list, UI_FACTORY_LIST_FIELD, UI_FACTORY_SEARCH_LIST_COUNT);

	for (i = 0; i < UI_FACTORY_LIST_FIELD; i++)
	{
		list_set_field_attr(p_search_list, (u8)i, (u32)(factory_list_attr[i].attr), (u16)(factory_list_attr[i].width),
				(u16)(factory_list_attr[i].left), (u16)(factory_list_attr[i].top));
		list_set_field_rect_style(p_search_list, (u8)i, factory_list_attr[i].rstyle);
		list_set_field_font_style(p_search_list, (u8)i, factory_list_attr[i].fstyle);
	}

	for(i =0; i<UI_FACTORY_SEARCH_LIST_COUNT; i++)
	{
		set_list_field_content(p_search_list,asc_str[i], i, IDN_STATE_TITLE);
	}

	factory_test.p_search_list = p_search_list;
	return p_search_cont;
}


control_t *create_factory_net_cont(control_t *p_cont)
{
	control_t *p_net_cont = NULL, *p_net_list = NULL;
	u8 i = 0;
	u8 asc_str[][64] = {
			{"网口IP地址:"},{"网口连接状态:"},{"网口连接测试:"},{"wifi搜索数量:"},
			{"U盘连接数量:"},{"U盘1名称:"},{"U盘2名称:"}};
			
	p_net_cont = ctrl_create_ctrl(CTRL_CONT,  IDC_NET_STATE_CONT, 
		UI_FACTORY_NET_CONT_X,UI_FACTORY_NET_CONT_Y,
		UI_FACTORY_NET_CONT_W,UI_FACTORY_NET_CONT_H,
		p_cont,(u32)0);
	ctrl_set_rstyle(p_net_cont, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);


	p_net_list = ctrl_create_ctrl(CTRL_LIST, IDC_NET_STATE_LIST,
		UI_FACTORY_NET_LIST_X,UI_FACTORY_NET_LIST_Y,
		UI_FACTORY_NET_LIST_W,UI_FACTORY_NET_LIST_H,
		p_net_cont,0);
	ctrl_set_rstyle(p_net_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	list_set_count(p_net_list, (u16)UI_FACTORY_NET_LIST_COUNT, UI_FACTORY_NET_LIST_COUNT);
	list_set_field_count(p_net_list, UI_FACTORY_LIST_FIELD, UI_FACTORY_NET_LIST_COUNT);

	for (i = 0; i < UI_FACTORY_LIST_FIELD; i++)
	{
		list_set_field_attr(p_net_list, (u8)i, (u32)(factory_list_attr[i].attr), (u16)(factory_list_attr[i].width),
				(u16)(factory_list_attr[i].left), (u16)(factory_list_attr[i].top));
		list_set_field_rect_style(p_net_list, (u8)i, factory_list_attr[i].rstyle);
		list_set_field_font_style(p_net_list, (u8)i, factory_list_attr[i].fstyle);
	}

	for(i =0; i<UI_FACTORY_NET_LIST_COUNT; i++)
	{
		set_list_field_content(p_net_list,asc_str[i], i, IDN_STATE_TITLE);
	}

	factory_test.p_net_list = p_net_list;
	return p_net_cont;
}

control_t *create_factory_remote_cont(control_t *p_cont)
{
	control_t *p_remote_cont = NULL,*p_remote = NULL,*p_remote_title = NULL;
	u16 i = 0;
	#ifdef FACTORY_TEST_FRONTSPANEL_ALL
	u8 asc_str[][64] = {{"待机"},{"频道+"},{"频道-"},{"音量-"},{"音量+"},{"确认"},{"菜单"}};
	#else
	u8 asc_str[][64] = {{"菜单"},{"确认"},{"频道-"},{"频道+"},{"音量-"},{"音量+"}};
	#endif

	p_remote_cont = ctrl_create_ctrl(CTRL_CONT,  IDC_REMOTE_CONT, 
							UI_FACTORY_FRONTPANEL_CONT_X,UI_FACTORY_FRONTPANEL_CONT_Y,
							UI_FACTORY_FRONTPANEL_CONT_W,UI_FACTORY_FRONTPANEL_CONT_H,
							p_cont,(u32)0);
	ctrl_set_rstyle(p_remote_cont, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);

	p_remote_title = ctrl_create_ctrl(CTRL_TEXT, IDC_REMOTE_TITLE,
						UI_FACTORY_FRONTPANEL_TITLE_X,UI_FACTORY_FRONTPANEL_TITLE_Y,
						UI_FACTORY_FRONTPANEL_TITLE_W,UI_FACTORY_FRONTPANEL_TITLE_H,p_remote_cont,0);
	ctrl_set_rstyle(p_remote_title,RSI_PBACK,RSI_COMM_TXT_HL,RSI_PBACK);
	text_set_content_type(p_remote_title, TEXT_STRTYPE_UNICODE);
	text_set_font_style(p_remote_title,FSI_WHITE,FSI_WHITE,FSI_WHITE);
	text_set_align_type(p_remote_title, STL_LEFT | STL_VCENTER);
	set_text_content_by_ascstr(p_remote_title, "前面板测试");

	p_remote = ctrl_create_ctrl(CTRL_MBOX, IDC_REMOTE_MBOX,
							  UI_FACTORY_FRONTPANEL_MBOX_X, UI_FACTORY_FRONTPANEL_MBOX_Y, 
							  UI_FACTORY_FRONTPANEL_MBOX_W, UI_FACTORY_FRONTPANEL_MBOX_H,
							  p_remote_cont, 0);
	ctrl_set_rstyle(p_remote, RSI_PBACK,RSI_PBACK, RSI_PBACK);
	mbox_enable_string_mode(p_remote, TRUE);
	mbox_set_content_strtype(p_remote, MBOX_STRTYPE_STRID);
	mbox_set_count(p_remote, UI_FACTORY_FRONTPANEL_MBOX_TOTAL, UI_FACTORY_FRONTPANEL_MBOX_COL, UI_FACTORY_FRONTPANEL_MBOX_ROW);
	mbox_set_string_fstyle(p_remote, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	mbox_set_string_align_type(p_remote, STL_CENTER| STL_VCENTER);
	mbox_set_content_strtype(p_remote, MBOX_STRTYPE_UNICODE);

	for(i=0; i<UI_FACTORY_FRONTPANEL_MBOX_COL; i++)
		set_mbox_content_by_ascstr(p_remote, i, asc_str[i]);

	for(i=UI_FACTORY_FRONTPANEL_MBOX_COL; i<UI_FACTORY_FRONTPANEL_MBOX_TOTAL; i++)
		mbox_set_content_by_icon(p_remote, (u16)i, IM_CONFIGCONNECT_1, IM_CONFIGCONNECT_1);

	factory_test.p_frontpanel_mbox= p_remote;
	return p_remote_cont;
}

control_t *create_factory_button_cont(control_t *p_cont)
{
	control_t *p_button;
	control_t *p_text;
	u16 uni_str[32] = {0};

	p_button = ctrl_create_ctrl(CTRL_CONT, IDC_FACTORY_BUTTON_CONT,
		UI_FACTORY_BUTTON_CONT_X, UI_FACTORY_BUTTON_CONT_Y,
		UI_FACTORY_BUTTON_CONT_W, UI_FACTORY_BUTTON_CONT_H, p_cont, 0);

	ctrl_set_rstyle(p_button,RSI_COMM_CONT_SH,RSI_SELECT_F,RSI_COMM_CONT_GRAY);
	ctrl_set_proc(p_button, ui_factory_button_cont_proc);
	ctrl_set_keymap(p_button, ui_factory_button_cont_msg);

	p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_FACTORY_BUTTON_TEXT,
		UI_FACTORY_BUTTON_TEXT_X,UI_FACTORY_BUTTON_TEXT_Y,
		UI_FACTORY_BUTTON_TEXT_W,UI_FACTORY_BUTTON_TEXT_H,p_button,0);
	ctrl_set_rstyle(p_text,RSI_PBACK,RSI_COMM_TXT_HL,RSI_PBACK);
	text_set_content_type(p_text, TEXT_STRTYPE_UNICODE);
	text_set_font_style(p_text,FSI_WHITE,FSI_WHITE,FSI_WHITE);
	text_set_align_type(p_text, STL_LEFT | STL_VCENTER);
	text_set_offset(p_text, 10, 0);
	convert_gb2312_chinese_asc2unistr("恢复出厂设置", uni_str, sizeof(uni_str));
	text_set_content_by_unistr(p_text, uni_str);

	return p_button;
}

static void factory_test_get_info()
{
	misc_options_t misc;
	u32 soft_version = 0;

	u8 *p_asc = NULL;
	u8 asc_str[64];

	memset(asc_str, 0, sizeof(asc_str));
	sys_get_serial_num(asc_str, sizeof(asc_str));
	FACTORY_TEST_PRINTF("[debug] %s \n",asc_str);
	if((asc_str[0] != 0))
	{
		set_list_field_content(factory_test.p_info_list,asc_str, IDN_INFO_SERIAL_NUMBER, IDN_STATE_OK);
	}
	else
	{
		set_list_field_content(factory_test.p_info_list,"", IDN_INFO_SERIAL_NUMBER, IDN_STATE_NO);	
	}

	if((p_asc = sys_status_get_mac_string()) != NULL)
	{
		memset(asc_str, 0, sizeof(asc_str));
		memcpy(asc_str,p_asc,12);
		set_list_field_content(factory_test.p_info_list,asc_str, IDN_INFO_MAC_ADDR, IDN_STATE_OK);
	}
	else
	{
		set_list_field_content(factory_test.p_info_list,"", IDN_INFO_MAC_ADDR, IDN_STATE_NO);
	}

	
	dm_read(class_get_handle_by_id(DM_CLASS_ID),
	                   MISC_OPTION_BLOCK_ID, 0, 0,
	                   sizeof(misc_options_t), (u8 *)&misc);
	if(misc.ota_tdi.oui != 0)
	{
		memset(asc_str, 0, sizeof(asc_str));
		sprintf(asc_str,"%lx",misc.ota_tdi.oui);
		set_list_field_content(factory_test.p_info_list,asc_str, IDN_INFO_OUI_ID, IDN_STATE_OK);
	}
	else
	{
		set_list_field_content(factory_test.p_info_list,"", IDN_INFO_OUI_ID, IDN_STATE_NO);
	}		

	if(misc.ota_tdi.hw_version != 0)
	{
		memset(asc_str, 0, sizeof(asc_str));
		sprintf(asc_str,"%x",misc.ota_tdi.hw_version);
		set_list_field_content(factory_test.p_info_list,asc_str, IDN_INFO_HARD_VERSION, IDN_STATE_OK);
	}
	else
	{
		set_list_field_content(factory_test.p_info_list,"", IDN_INFO_HARD_VERSION, IDN_STATE_NO);
	}

	soft_version = ui_ota_api_get_upg_check_version();
	if(soft_version != 0)
	{
		memset(asc_str, 0, sizeof(asc_str));
		sprintf(asc_str,"%lx",soft_version);
		set_list_field_content(factory_test.p_info_list,asc_str, IDN_INFO_SOFT_VERSION, IDN_STATE_OK);
	}
	else
	{
		set_list_field_content(factory_test.p_info_list,"", IDN_INFO_SOFT_VERSION, IDN_STATE_NO);
	}

	#ifdef FACTORY_TEST_WITH_DIFF_SMARTCARD
	set_list_field_content(factory_test.p_info_list,"", IDN_INFO_CA_CARD_STATE, IDN_STATE_NO);
	set_list_field_content(factory_test.p_info_list,"", IDN_INFO_CA_CARD_ID, IDN_STATE_NO);
	ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0 ,0);
	#endif
	#ifdef FACTORY_TEST_WITH_ALONE_SMARTCARD
	set_list_field_content(factory_test.p_info_list,"", IDN_INFO_CA_CARD_STATE, IDN_STATE_NO);
	set_list_field_content(factory_test.p_info_list,"", IDN_INFO_CA_CARD_ID, IDN_STATE_NO);
	factory_test.scard_insert_state = ui_get_smart_card_state();
	#endif
	ctrl_paint_ctrl(factory_test.p_info_list, TRUE);
}

static void factory_test_scan_search_over(void)
{
	u16 rid;
	
	ui_dbase_pg_sort(DB_DVBS_ALL_PG);
	ui_set_book_flag(TRUE);
	ui_dbase_reset_last_prog(TRUE);
	ui_set_front_panel_by_str("----");
	ui_release_scan();
	ui_epg_init();
	#ifndef IMPL_NEW_EPG
	ui_epg_start();
	#else
	ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
	#endif
	ui_enable_playback(TRUE);
	ui_time_init();

	if((rid =db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(),0)) != INVALIDID)
	{
		if(ui_is_playing() == FALSE)
		{
			ui_auto_switch_mode_stop_play(TRUE);
		}
		ui_play_prog(rid, FALSE);
	}
}

static void factory_test_scan_search_start(void)
{
	scan_param_t scan_param;
	sat_node_t sat_node = {0};
	dvbs_tp_node_t tp = {0};
	dvbc_lock_t main_tp = {0};
	u16 rid = 0;

	sys_status_get_main_tp1(&main_tp);
	/* init, use video buffer */
	ui_scan_param_init();
	ui_scan_add_dvbt_sat(FALSE, &sat_node);
	ui_scan_param_add_sat(&sat_node);
	/* add preset tp */
	sys_status_get_scan_param(&scan_param);

	tp.freq = main_tp.tp_freq;
	tp.sym = DTMB_HN_SYMBOL_DEFAULT;
	tp.sat_id = rid;
	ui_scan_param_add_tp(&tp);

	/* init scan param */
	ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, FALSE,NIT_SCAN_ONCE);

	FACTORY_TEST_PRINTF("[debug] %s %d \n",__FUNCTION__,__LINE__);

	ui_set_book_flag(FALSE);
	ui_enable_playback(FALSE);
	ui_time_release();
	ui_epg_stop();
	ui_epg_release();
	ui_init_scan();
	ui_start_scan();
	FACTORY_TEST_PRINTF("init scan\n");
}

static void factory_test_scan_lock_tuner()
{
	dvbs_tp_node_t tp = {0};
	dvbc_lock_t main_tp = {0};

	sys_status_get_main_tp1(&main_tp);
	tp.freq = main_tp.tp_freq;
#ifdef  DTMB_PROJECT
	tp.sym = DTMB_HN_SYMBOL_DEFAULT;
#else
	nim_modulation_t nim_modul = NIM_MODULA_AUTO;
	tp.sym = main_tp.tp_sym;
	switch(main_tp.nim_modulate)
	{
		case 0:
			nim_modul =NIM_MODULA_AUTO;
			break;

		case 1:
			nim_modul = NIM_MODULA_BPSK;      
			break;

		case 2:
			nim_modul = NIM_MODULA_QPSK;      
			break;

		case 3:
			nim_modul = NIM_MODULA_8PSK;      
			break;

		case 4:
			nim_modul = NIM_MODULA_QAM16;
			break;

		case 5:
			nim_modul = NIM_MODULA_QAM32;
			break;

		case 6:
			nim_modul = NIM_MODULA_QAM64;      
			break;

		case 7:
			nim_modul = NIM_MODULA_QAM128;
			break;

		case 8:
			nim_modul = NIM_MODULA_QAM256;
			break;

		default:
			nim_modul = NIM_MODULA_QAM64;
			break;
	}
	tp.nim_modulate = nim_modul;
#endif
	ui_set_transpond(&tp);
}

#ifdef FACTORY_TEST_WITH_SEARCH
static void do_search_reset_pg_and_bouquet(void)
{
	BOOL is_scart = FALSE;
	//set default use common buffer as ext buffer
	db_dvbs_restore_to_factory(PRESET_BLOCK_ID, BLOCK_REC_BUFFER);
	sys_status_get_status(BS_IS_SCART_OUT, &is_scart);
	sys_status_set_categories_count(0);
	categories_mem_reset();
	if(is_scart)
	{
		sys_status_set_status(BS_IS_SCART_OUT, TRUE);
	}
	sys_status_check_group();
	sys_status_save();

	//clear history 
	ui_clear_play_history();

	//delete all book imformation
	book_delete_all_node();
 }
#endif

static void factory_test_get_search_param(void)
{
	dvbc_lock_t main_tp = {0};
	u8 asc_str[64];
	
	sys_status_get_main_tp1(&main_tp);
	if(main_tp.tp_freq > 0)
	{
		memset(asc_str, 0, sizeof(asc_str));
		sprintf(asc_str,"%ld",main_tp.tp_freq);
		set_list_field_content(factory_test.p_search_list,asc_str, IDN_SEARCH_MAIN_FRE, IDN_STATE_OK);
	}
	else
	{
		set_list_field_content(factory_test.p_search_list,"", IDN_SEARCH_MAIN_FRE, IDN_STATE_OK);
	}
	FACTORY_TEST_PRINTF("[debug] %s %d \n",__FUNCTION__,__LINE__);
	ctrl_paint_ctrl(factory_test.p_search_list, TRUE);
#ifdef FACTORY_TEST_WITH_SEARCH
	do_search_reset_pg_and_bouquet();
	factory_test_scan_search_start();
#else
	factory_test_scan_lock_tuner();
#endif
}

static void factory_test_search_exit(void)
{
#ifdef FACTORY_TEST_WITH_SEARCH
	if(factory_test.searched_flag == FALSE)
	{
		FACTORY_TEST_PRINTF("[debug] %s %d\n",__FUNCTION__,__LINE__);
		ui_stop_scan();
		ui_release_scan();

		ui_set_book_flag(TRUE);
		ui_epg_init();
#ifndef IMPL_NEW_EPG
		ui_epg_start();
#else
		ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#endif
		ui_enable_playback(TRUE);
		ui_time_init();
	}
	factory_test_restore_to_factory();
#endif
	return;
}

static void factory_test_cmd_to_ping_test(ip_address_t *addr)
{
	service_t *p_server = NULL;
	net_svc_cmd_para_t net_svc_para;
	net_svc_t *p_net_svc = NULL;

	p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
	if(p_net_svc->net_svc_is_busy(p_net_svc) == TRUE)
	{
		FACTORY_TEST_PRINTF("###########net service is busy ,return %d#########\n",p_net_svc->net_svc_is_busy(p_net_svc));
		return ;
	}
	memset(&net_svc_para, 0 , sizeof(net_svc_para));
	net_svc_para.p_eth_dev = NULL;
	net_svc_para.ping_para.count = 20;
	net_svc_para.ping_para.size = 64;
	net_svc_para.ping_para.ipaddr[0] = addr->s_b1;
	net_svc_para.ping_para.ipaddr[1] = addr->s_b2;
	net_svc_para.ping_para.ipaddr[2] = addr->s_b3;
	net_svc_para.ping_para.ipaddr[3] = addr->s_b4;
	memcpy(net_svc_para.ping_para.url, "www.baidu.com", sizeof(net_svc_para.ping_para.url));
	FACTORY_TEST_PRINTF("####do_cmd_to_ping_test url is %s#####\n",net_svc_para.ping_para.url);
	p_server = (service_t *)ui_get_net_svc_instance();

	p_server->do_cmd(p_server, NET_DO_PING_TEST, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
}

static void factory_test_set_network_lan_connect(void)
{
	net_conn_info_t net_conn_info = {0};  
	ethernet_device_t *eth_dev = NULL;
	net_conn_stats_t eth_connt_stats;

	net_conn_info = ui_get_conn_info();

	eth_connt_stats = ui_get_net_connect_status();

	if(eth_connt_stats.is_eth_insert)
	{
		eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
	}
	else
	{
		eth_dev = (ethernet_device_t *)ui_get_usb_eth_dev();
	}

	set_list_field_content(factory_test.p_net_list, "000.000.000.000", IDN_NET_LAN_IP_ADDR, IDN_STATE_NO);
	if(eth_dev == NULL)
	{
		factory_test.lan_insert_state = FALSE;
		set_list_field_content(factory_test.p_net_list, "没有插网线", IDN_NET_LAN_CONNECT_STATE, IDN_STATE_NO);
		ctrl_paint_ctrl(factory_test.p_net_list, TRUE);
		return;
	}
	else
	{
		factory_test.lan_insert_state = TRUE;
		set_list_field_content(factory_test.p_net_list, "正在连接网络...", IDN_NET_LAN_CONNECT_STATE, IDN_STATE_NO);
		ctrl_paint_ctrl(factory_test.p_net_list, TRUE);
	}

	factory_test.ethcfg.hwaddr[0] = sys_status_get_mac_by_index(0);
	factory_test.ethcfg.hwaddr[1] = sys_status_get_mac_by_index(1);
	factory_test.ethcfg.hwaddr[2] = sys_status_get_mac_by_index(2);
	factory_test.ethcfg.hwaddr[3] = sys_status_get_mac_by_index(3);
	factory_test.ethcfg.hwaddr[4] = sys_status_get_mac_by_index(4);
	factory_test.ethcfg.hwaddr[5] = sys_status_get_mac_by_index(5);
	factory_test.ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
	factory_test.ethcfg.is_enabledhcp = TRUE;

	do_cmd_connect_network(&factory_test.ethcfg, eth_dev);
}

static void factory_test_set_wifi_info_search(void)
{
	service_t *p_server = NULL;
	net_svc_cmd_para_t net_svc_para;
	net_svc_t *p_net_svc = NULL;

	if(factory_test.wifi_search_state == TRUE)
		return;
	
	factory_test.p_wifi_dev = (ethernet_device_t *)get_wifi_handle();

	if(factory_test.p_wifi_dev == NULL)
	{
		set_list_field_content(factory_test.p_net_list, "没有wifi设备", IDN_NET_WIFI_SEARCH_NUM, IDN_STATE_NO);
		ctrl_paint_ctrl(factory_test.p_net_list,TRUE);
		factory_test.wifi_insert_state = FALSE;
		FACTORY_TEST_PRINTF("[debug] %s %d \n",__FUNCTION__,__LINE__);
		return ;
	}

	p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
	if(p_net_svc->net_svc_is_busy(p_net_svc) == TRUE)
	{
		set_list_field_content(factory_test.p_net_list,"wifi设备忙", IDN_NET_WIFI_SEARCH_NUM, IDN_STATE_NO);
		ctrl_paint_ctrl(factory_test.p_net_list,TRUE);
		FACTORY_TEST_PRINTF("###########net service is busy ,will be return#########\n");
		return ;
	}
	net_svc_para.p_eth_dev = factory_test.p_wifi_dev;
	p_server = (service_t *)ui_get_net_svc_instance();

	FACTORY_TEST_PRINTF("[debug] %s %d \n",__FUNCTION__,__LINE__);
	factory_test.wifi_search_state = TRUE;
	factory_test.wifi_insert_state = TRUE;
	p_server->do_cmd(p_server, NET_DO_WIFI_SERACH, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
}

static void factory_test_do_cmd_to_conn_wifi()
{
	wifi_encrypt_type_t wifi_encrypt_type = 0;
	service_t *p_server = NULL;
	net_svc_cmd_para_t net_svc_para;
	net_svc_t *p_net_svc = NULL;
	ethernet_cfg_t ethcfg = {0};

	ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
	ethcfg.is_enabledhcp = 1;
	dev_io_ctrl(factory_test.p_wifi_dev, GET_ETH_HW_MAC_ADDRESS, (u32)&ethcfg.hwaddr[0]);

	wifi_encrypt_type = IW_ENC_WPA2;
	p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
	p_net_svc->net_svc_clear_msg(p_net_svc);
	memset(&net_svc_para, 0 , sizeof(net_svc_para));
	net_svc_para.p_eth_dev = factory_test.p_wifi_dev;

	memcpy(&net_svc_para.net_cfg, &ethcfg, sizeof(ethernet_cfg_t));
	if(wifi_encrypt_type != IW_ENC_NONE)
	{
		memcpy(net_svc_para.net_cfg.wifi_para.key, "123456778", MAX_PASS_WORD_LEN);
	}
	memcpy(net_svc_para.net_cfg.wifi_para.ssid, "ott_dianxin", MAX_SSID_LEN);

	net_svc_para.net_cfg.wifi_para.is_enable_encrypt = wifi_encrypt_type;
	net_svc_para.net_cfg.wifi_para.encrypt_type = wifi_encrypt_type;

	p_server = (service_t *)ui_get_net_svc_instance();
	p_server->do_cmd(p_server, NET_DO_WIFI_CONNECT, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
}

static void factory_test_get_first_partition_info()
{
	flist_dir_t flist_dir = NULL;
	file_list_t flist = {0};

	factory_test.partition_cnt = (u16)file_list_get_partition(&factory_test.p_partition);
	
	if(factory_test.partition_cnt > 0)
	{
		ui_evt_disable_ir();
		flist_dir = file_list_enter_dir(NULL, MAX_FILE_COUNT, factory_test.p_partition[0].letter);
		if(NULL == flist_dir)
		{
			factory_test.file_count= 0;
		}
		else
		{
			file_list_get(flist_dir, FLIST_UNIT_FIRST, &flist);
			factory_test.file_count = (u16)flist.file_count;
		}
		ui_evt_enable_ir();
	}
}

static void factory_test_get_net_info(void)
{
	net_conn_stats_t net_stat;
	net_stat = ui_get_net_connect_status();

	if(net_stat.is_eth_insert || net_stat.is_usb_eth_insert)
	{
		if(factory_test.lan_insert_state == FALSE)
		{
			factory_test_set_network_lan_connect();
		}
	}
	else
	{
		set_list_field_content(factory_test.p_net_list, "", IDN_NET_LAN_IP_ADDR, IDN_STATE_NO);
		set_list_field_content(factory_test.p_net_list, "网线没有连接", IDN_NET_LAN_CONNECT_STATE, IDN_STATE_NO);
		set_list_field_content(factory_test.p_net_list, "", IDN_NET_LAN_PING_TEST, IDN_STATE_NO);
		ctrl_paint_ctrl(factory_test.p_net_list, TRUE);
		factory_test.lan_insert_state = FALSE;
	}
	
	if(net_stat.is_wifi_insert || net_stat.is_3g_insert)
	{
		if(factory_test.wifi_insert_state == FALSE)
		{
			factory_test.wifi_search_state = FALSE;
			factory_test_set_wifi_info_search();
		}
	}
	else
	{
		factory_test.wifi_search_state = FALSE;
		factory_test.wifi_insert_state  = FALSE;
		set_list_field_content(factory_test.p_net_list, "没有wifi设备", IDN_NET_WIFI_SEARCH_NUM, IDN_STATE_NO);
		ctrl_paint_ctrl(factory_test.p_net_list, TRUE);			
	}
}

static void factory_test_get_usb_info(void)
{
	u8	asc_str[8] = {0};
	
	if(ui_get_usb_status() == FALSE)
	{
		set_list_field_content(factory_test.p_net_list, "没有U盘", IDN_NET_USB_STATE, IDN_STATE_NO);
		set_list_field_content(factory_test.p_net_list, "", IDN_NET_USB_PARTITION_1, IDN_STATE_NO);
		set_list_field_content(factory_test.p_net_list, "", IDN_NET_USB_PARTITION_2, IDN_STATE_NO);
		ctrl_paint_ctrl(factory_test.p_net_list, TRUE);
		return;
	}

	factory_test.partition_cnt = (u16)file_list_get_partition(&factory_test.p_partition);
	if(factory_test.partition_cnt == 1)
	{
		sprintf(asc_str, "%d 个U盘",factory_test.partition_cnt);
		set_list_field_content(factory_test.p_net_list, asc_str, IDN_NET_USB_STATE, IDN_STATE_OK);

		if(factory_test.p_partition[0].name[0] == 0x43)
		{
			list_set_field_content_by_unistr(factory_test.p_net_list, IDN_NET_USB_PARTITION_1, IDN_FIELD_CONTENT, factory_test.p_partition[0].name);
			list_set_field_content_by_icon(factory_test.p_net_list, IDN_NET_USB_PARTITION_1, IDN_FIELD_STATE, IM_CONFIGCONNECT_2);
			set_list_field_content(factory_test.p_net_list, "", IDN_NET_USB_PARTITION_2, IDN_STATE_NO);
		}
		else
		{
			list_set_field_content_by_unistr(factory_test.p_net_list, IDN_NET_USB_PARTITION_2, IDN_FIELD_CONTENT, factory_test.p_partition[0].name);
			list_set_field_content_by_icon(factory_test.p_net_list, IDN_NET_USB_PARTITION_2, IDN_FIELD_STATE, IM_CONFIGCONNECT_2);
			set_list_field_content(factory_test.p_net_list, "", IDN_NET_USB_PARTITION_1, IDN_STATE_NO);
		}
	}
	else
	{
		sprintf(asc_str, "%d 个U盘",factory_test.partition_cnt);
		set_list_field_content(factory_test.p_net_list, asc_str, IDN_NET_USB_STATE, IDN_STATE_OK);

		FACTORY_TEST_PRINTF("[debug] %x %x %x %x \n",factory_test.p_partition[0].name[0],factory_test.p_partition[0].name[1],factory_test.p_partition[0].name[2],factory_test.p_partition[0].name[3]);
		FACTORY_TEST_PRINTF("[debug] %x %x %x %x \n",factory_test.p_partition[1].name[0],factory_test.p_partition[1].name[1],factory_test.p_partition[1].name[2],factory_test.p_partition[1].name[3]);
		list_set_field_content_by_unistr(factory_test.p_net_list, IDN_NET_USB_PARTITION_1, IDN_FIELD_CONTENT, factory_test.p_partition[0].name);
		list_set_field_content_by_icon(factory_test.p_net_list, IDN_NET_USB_PARTITION_1, IDN_FIELD_STATE, IM_CONFIGCONNECT_2);
		list_set_field_content_by_unistr(factory_test.p_net_list, IDN_NET_USB_PARTITION_2, IDN_FIELD_CONTENT, factory_test.p_partition[1].name);
		list_set_field_content_by_icon(factory_test.p_net_list, IDN_NET_USB_PARTITION_2, IDN_FIELD_STATE, IM_CONFIGCONNECT_2);		
	}

	ctrl_paint_ctrl(factory_test.p_net_list, TRUE);
}

static void factory_test_tp_lock_info_update(struct signal_data *data)
{
	if(data->lock)
	{
		set_list_field_content(factory_test.p_search_list, "TUNER锁定", IDN_SEARCH_TUNER_SATTE, IDN_STATE_OK);
	}
	else
	{
		set_list_field_content(factory_test.p_search_list, "TUNER未锁定", IDN_SEARCH_TUNER_SATTE, IDN_STATE_NO);
	}
	ctrl_paint_ctrl(factory_test.p_search_list, TRUE);
}

static BOOL factory_test_drv_smc_reset()
{
	u32 err = 0;
	scard_atr_desc_t s_atr = {0};
	scard_device_t *p_smc_dev = NULL;

	p_smc_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
	if(p_smc_dev == NULL)
		return FALSE;

	err = scard_active(p_smc_dev, &s_atr);
	if(0 != err)
	{
		FACTORY_TEST_PRINTF("%s : %s : %d : error_code = 0x%08x \n", __FILE__, __FUNCTION__, __LINE__, err);
		return FALSE;
	}
	scard_pro_register_op((scard_device_t *)p_smc_dev);

	return TRUE;
}

static int factory_test_card_cmd_run(u8 *pucCommand,u16 usCommandLen,u8 *pucReply,u16 *pusReplyLen)
{
	scard_opt_desc_t opt_desc = {0};
	s8 retry_times = 3;
	int ret;

	opt_desc.p_buf_out = pucCommand;
	opt_desc.bytes_out = usCommandLen;
	opt_desc.p_buf_in = pucReply;
	opt_desc.bytes_in_actual = 0;

	mtos_task_lock();
	do
	{
		ret = scard_pro_rw_transaction(&opt_desc);
		if(ret == SUCCESS)
		{
			break;
		}
		retry_times--;
	}while (retry_times > 0);
	mtos_task_unlock();

	if((ret == SUCCESS)&&(opt_desc.bytes_in_actual >= 2))
	{
		*pusReplyLen = opt_desc.bytes_in_actual;
		return SUCCESS;
	}
	return ERR_FAILURE;
}

static u32 factory_test_get_card_id(void)
{
	u8 aucCardIDCmd[] = 	{0x5A, 0x0c, 0x00, 0x00, 0x08};
	u8 pucReply[64] = {0};
	u16 usReplyLen = 0;
	u32 uiCardID = 0;
	int ret;

	ret = factory_test_card_cmd_run(aucCardIDCmd, 5, pucReply, &usReplyLen);
	if(ret == SUCCESS)
	{
		if(pucReply[usReplyLen - 2] == 0x90 && pucReply[usReplyLen - 1] == 0x00)
		{
			uiCardID = (pucReply[4] << 24) | (pucReply[5] << 16) | (pucReply[6] << 8) |pucReply[7];
			uiCardID = (uiCardID ^ 0x57b87c2d) - 0X5F1D6B;
			//FACTORY_TEST_PRINTF("[debug] uiCardID %ld %x \n",uiCardID,uiCardID);
			return uiCardID;
		}
	}
	return 0;
}

static void factory_test_ca_info_update(void)
{
	u32 uiCardID = 0;
	u8 asc_str[32] = {0};

	if((factory_test.scard_insert_state == FALSE)|| (factory_test.p_info_list == NULL)||(factory_test.read_card == TRUE))
	{
		return;
	}

	if(factory_test_drv_smc_reset() == FALSE)
	{
		set_list_field_content(factory_test.p_info_list, "", IDN_INFO_CA_CARD_ID, IDN_STATE_NO);
		set_list_field_content(factory_test.p_info_list, "智能卡复位失败", IDN_INFO_CA_CARD_STATE, IDN_STATE_NO);
		ctrl_paint_ctrl(factory_test.p_info_list, TRUE);
		return;
	}

	uiCardID = factory_test_get_card_id();
	if(uiCardID > 0)
	{
		sprintf(asc_str, "%lx",uiCardID);
		factory_test.read_card = TRUE;
		set_list_field_content(factory_test.p_info_list, asc_str, IDN_INFO_CA_CARD_ID, IDN_STATE_OK);
		set_list_field_content(factory_test.p_info_list, "智能卡复位成功", IDN_INFO_CA_CARD_STATE, IDN_STATE_OK);
	}
	else
	{
		factory_test.read_card = FALSE;
		set_list_field_content(factory_test.p_info_list, "", IDN_INFO_CA_CARD_ID, IDN_STATE_NO);
		set_list_field_content(factory_test.p_info_list, "无法识别卡", IDN_INFO_CA_CARD_STATE, IDN_STATE_NO);
	}
	ctrl_paint_ctrl(factory_test.p_info_list, TRUE);

	FACTORY_TEST_PRINTF("[debug] %s %d \n",__FUNCTION__,__LINE__);
	return ;
}


void ui_factory_test_monitor(control_t *p_cont)
{
	factory_test_get_info();
	factory_test_get_search_param();
	factory_test_set_network_lan_connect();
	factory_test_get_usb_info();
	factory_test_set_wifi_info_search();
}

RET_CODE open_factory_test(u32 para1, u32 para2)
{
	control_t *p_cont;
	control_t *p_button;
	(void)factory_test_do_cmd_to_conn_wifi;
	(void)factory_test_get_first_partition_info;
	#ifdef FACTORY_TEST_WITH_SEARCH
	(void)factory_test_scan_lock_tuner;
	#else
	(void)factory_test_scan_search_start;
	#endif
	
	memset((char *)&factory_test, 0, sizeof(factory_test_t));
	factory_test.searched_flag = FALSE;
	
	p_cont = fw_create_mainwin(ROOT_ID_FACTORY_TEST,
	                UI_FACTORY_CONT_X, UI_FACTORY_CONT_Y,
	                UI_FACTORY_CONT_W, UI_FACTORY_CONT_H,
	                0,0,OBJ_ATTR_ACTIVE, 0);
	if(p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	ctrl_set_rstyle(p_cont, RSI_TRANSPARENT,RSI_TRANSPARENT, RSI_TRANSPARENT);
	ctrl_set_proc(p_cont, ui_factory_cont_proc);
	ctrl_set_keymap(p_cont, ui_factory_cont_msg);

	create_factory_info_cont(p_cont);
	create_factory_search_cont(p_cont);
	create_factory_net_cont(p_cont);
	create_factory_remote_cont(p_cont);
	p_button = create_factory_button_cont(p_cont);

	ctrl_default_proc(p_button, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

	ui_factory_test_monitor(p_cont);

	return SUCCESS;
}


//ca info
static RET_CODE on_factory_test_ca_message_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u32 event_id = para2;

	FACTORY_TEST_PRINTF("\r\n*** on_factory_test_ca_message_update event_id[0x%x]  CAS_S_ADPT_CARD_INSERT = %x\r\n",para2,CAS_S_ADPT_CARD_INSERT);
	switch(event_id)
	{
		case CAS_S_ADPT_CARD_REMOVE:
			factory_test.scard_insert_state = FALSE;
			factory_test.read_card = FALSE;
			set_list_field_content(factory_test.p_info_list, "", IDN_INFO_CA_CARD_ID, IDN_STATE_NO);
			set_list_field_content(factory_test.p_info_list, "请插入智能卡", IDN_INFO_CA_CARD_STATE, IDN_STATE_NO);
			break;
		case CAS_S_ADPT_CARD_INSERT:
			factory_test.scard_insert_state = TRUE;
			factory_test.read_card = FALSE;
			set_list_field_content(factory_test.p_info_list, "", IDN_INFO_CA_CARD_ID, IDN_STATE_NO);
			set_list_field_content(factory_test.p_info_list, "智能卡复位中", IDN_INFO_CA_CARD_STATE, IDN_STATE_NO);
			break;		
		default:
			break;
	}
	ctrl_paint_ctrl(factory_test.p_info_list, TRUE);
	return SUCCESS;
}

static RET_CODE on_factory_test_ca_init_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	FACTORY_TEST_PRINTF("[%s] %d \n",__FUNCTION__,__LINE__);
	#ifdef FACTORY_TEST_WITH_DIFF_SMARTCARD
	ui_ca_do_cmd(CAS_CMD_CARD_INFO_GET, 0 ,0);
	#endif
	return SUCCESS;
}

static RET_CODE on_factory_test_ca_info_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	cas_card_info_t *p_card_info = (cas_card_info_t *)para2;
	u8 asc_str[64] = {0};

	FACTORY_TEST_PRINTF("[%s] %d \n",__FUNCTION__,__LINE__);
	if(strlen(p_card_info->sn) != 0)
	{
		FACTORY_TEST_PRINTF("[%s] %d  sn[%s]\n",__FUNCTION__,__LINE__,p_card_info->sn);
		sprintf((char *)asc_str, "%s", p_card_info->sn);
		set_list_field_content(factory_test.p_info_list, asc_str, IDN_INFO_CA_CARD_ID, IDN_STATE_OK);
		set_list_field_content(factory_test.p_info_list, "智能卡复位成功", IDN_INFO_CA_CARD_STATE, IDN_STATE_OK);
		ctrl_paint_ctrl(factory_test.p_info_list,TRUE);
	}
	return SUCCESS;
}

//lock
static RET_CODE on_factory_test_update_signal(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
#ifdef FACTORY_TEST_WITH_ALONE_SMARTCARD
	factory_test_ca_info_update();
#else
	(void)factory_test_ca_info_update;
#endif
	factory_test_tp_lock_info_update((struct signal_data *)(para1));
	factory_test_get_usb_info();
	factory_test_get_net_info();

	//FACTORY_TEST_PRINTF("[debug] %s %d \n",__FUNCTION__,__LINE__);
	return SUCCESS;
}


//search
static RET_CODE on_factory_test_update_progress(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u8 asc_str[64];

	FACTORY_TEST_PRINTF("[debug] %s %d  para1=%d\n",__FUNCTION__,__LINE__,para1);
	memset(asc_str, 0, sizeof(asc_str));
	sprintf(asc_str,"%d%%",(u16)para1);
	set_list_field_content(factory_test.p_search_list, asc_str, IDN_SEARCH_PROGRESS, IDN_STATE_CONTENT);
	ctrl_paint_ctrl(factory_test.p_search_list, TRUE);
	return SUCCESS;
}

static RET_CODE on_factory_test_update_program_num(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u8 asc_str[64] = {0};

	factory_test.prog_num++;
	FACTORY_TEST_PRINTF("[debug] %s %d  factory_test.prog_num = %d\n",__FUNCTION__,__LINE__,factory_test.prog_num);
	sprintf(asc_str,"%d",factory_test.prog_num);
	set_list_field_content(factory_test.p_search_list, asc_str, IDN_SEARCH_PROGRAM_NUM, IDN_STATE_CONTENT);
	ctrl_paint_ctrl(factory_test.p_search_list, TRUE);
	return SUCCESS;
}

static RET_CODE on_factory_test_update_search_finished(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u8 asc_str[64] = {0};

	FACTORY_TEST_PRINTF("[debug] %s %d para1 = %d\n",__FUNCTION__,__LINE__,para1);
	factory_test.searched_flag = TRUE;
	if(factory_test.prog_num > 0)
	{

		sprintf(asc_str,"%d",factory_test.prog_num);
		set_list_field_content(factory_test.p_search_list, asc_str, IDN_SEARCH_PROGRAM_NUM, IDN_STATE_OK);
		set_list_field_content(factory_test.p_search_list, "100%", IDN_SEARCH_PROGRESS, IDN_STATE_OK);
	}
	else
	{
		set_list_field_content(factory_test.p_search_list, "0", IDN_SEARCH_PROGRAM_NUM, IDN_STATE_NO);
		set_list_field_content(factory_test.p_search_list, "100%", IDN_SEARCH_PROGRESS, IDN_STATE_NO);
	}
	
	ctrl_paint_ctrl(factory_test.p_search_list, TRUE);
	factory_test_scan_search_over();

	return SUCCESS;
}

static RET_CODE on_factory_test_lan_connect(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	ip_address_t addr = {0,};
	ethernet_device_t *eth_dev = NULL;

	u8 ipaddr[20] = {0};
	u8 netmask[20] = {0};
	u8 gw[20] = {0};
	u8 primarydns[20] = {0};
	u8 alternatedns[20] = {0};
	u8 ipaddress[32] = {0};
	net_conn_stats_t eth_connt_stats;
	
	if(para1)
	{
		FACTORY_TEST_PRINTF("###########lwip_init_tcpip return failed, desktop connect failed######\n ");
		return ERR_FAILURE;
	}

	eth_connt_stats = ui_get_net_connect_status();
	if(eth_connt_stats.is_eth_insert)
	{
		eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
	}
	else
	{
		eth_dev = (ethernet_device_t *)ui_get_usb_eth_dev();
	}
	MT_ASSERT(eth_dev != NULL);
	
	get_net_device_addr_info(eth_dev, ipaddr, netmask, gw, primarydns, alternatedns);

	factory_test.ping_test = LAN_PING_TEST;
	factory_test.lan_ok_times = 0;
	factory_test.lan_bad_times = 0;
	get_addr_param(ipaddr, (ip_address_t *)&addr);
	factory_test_cmd_to_ping_test(&addr);
	sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);

	set_list_field_content(factory_test.p_net_list, ipaddress, IDN_NET_LAN_IP_ADDR, IDN_STATE_OK);
	set_list_field_content(factory_test.p_net_list, "连接正常", IDN_NET_LAN_CONNECT_STATE, IDN_STATE_OK);
	ctrl_paint_ctrl(factory_test.p_net_list, TRUE);

	return SUCCESS;
}

static RET_CODE on_factory_test_ping_test_update(control_t *p_sub, u16 msg,u32 para1, u32 para2)
{
  	u8 asc_str[64] = {0};
	
	if(para1 == 1)
	{
		factory_test.lan_ok_times += 1;
	}
	else
	{
		factory_test.lan_bad_times += 1;
	}

	sprintf(asc_str, "%d/20",factory_test.lan_ok_times);

	if(factory_test.ping_test == LAN_PING_TEST)
	{
		set_list_field_content(factory_test.p_net_list, asc_str, IDN_NET_LAN_PING_TEST, IDN_STATE_CONTENT);
	}

	if((factory_test.lan_ok_times + factory_test.lan_bad_times) == 20)
	{
		if(factory_test.lan_ok_times > 15)
			set_list_field_content(factory_test.p_net_list, asc_str, IDN_NET_LAN_PING_TEST, IDN_STATE_OK);
		else
			set_list_field_content(factory_test.p_net_list, asc_str, IDN_NET_LAN_PING_TEST, IDN_STATE_NO);
	}
	ctrl_paint_ctrl(factory_test.p_net_list, TRUE);
	FACTORY_TEST_PRINTF("#####after ping test the ok times = %d, bad times is %d######\n", factory_test.lan_ok_times, factory_test.lan_bad_times);

	return SUCCESS;
}


static RET_CODE on_factory_test_wifi_search_ok(control_t *p_list, u16 msg,u32 para1, u32 para2)
{
	u8 asc_str[64] = {0};

	sprintf(asc_str, "%ld",para1);
	if(para1 > 0)
	{
		set_list_field_content(factory_test.p_net_list, asc_str, IDN_NET_WIFI_SEARCH_NUM, IDN_STATE_OK);
	}
	else
	{
		set_list_field_content(factory_test.p_net_list, asc_str, IDN_NET_WIFI_SEARCH_NUM, IDN_STATE_NO);
		if(factory_test.wifi_search_count < 3)
		{
			factory_test.wifi_search_count++;
			factory_test.wifi_search_state = FALSE;
			factory_test_set_wifi_info_search();
		}
	}
	ctrl_paint_ctrl(factory_test.p_net_list, TRUE);

	FACTORY_TEST_PRINTF("[debug] %s %d \n",__FUNCTION__,__LINE__);
	return SUCCESS;
}

static void  factory_test_restore_to_factory(void)
{
	u16 view_id;
	utc_time_t  p_time;
	ip_address_set_t ip_addr = {{0}};
	FACTORY_TEST_PRINTF("[debug] %s %d\n",__FUNCTION__,__LINE__);

	ui_stop_play(STOP_PLAY_BLACK, TRUE);

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
}

static RET_CODE on_exit_and_restore_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u16 v_key = para1;
	key_type_t type = para2;
	FACTORY_TEST_PRINTF("[debug] para1 = %x para2 = %x \n", para1, para2);	
	
	if(type == FP_KEY)
	{
		switch(v_key)
		{
			case V_KEY_OK:
				mbox_set_content_by_icon(factory_test.p_frontpanel_mbox, 
					(u16)(UI_FACTORY_FRONTPANEL_MBOX_COL + IDN_KEY_OK),IM_CONFIGCONNECT_2, IM_CONFIGCONNECT_2);
				break;
			case V_KEY_UP:
				mbox_set_content_by_icon(factory_test.p_frontpanel_mbox, 
					(u16)(UI_FACTORY_FRONTPANEL_MBOX_COL + IDN_KEY_UP),IM_CONFIGCONNECT_2, IM_CONFIGCONNECT_2);
				break;
			case V_KEY_DOWN:
				mbox_set_content_by_icon(factory_test.p_frontpanel_mbox, 
					(u16)(UI_FACTORY_FRONTPANEL_MBOX_COL + IDN_KEY_DOWN),IM_CONFIGCONNECT_2, IM_CONFIGCONNECT_2);
				break;
			case V_KEY_LEFT:
				mbox_set_content_by_icon(factory_test.p_frontpanel_mbox, 
					(u16)(UI_FACTORY_FRONTPANEL_MBOX_COL + IDN_KEY_LEFT),IM_CONFIGCONNECT_2, IM_CONFIGCONNECT_2);
				break;
			case V_KEY_RIGHT:
				mbox_set_content_by_icon(factory_test.p_frontpanel_mbox, 
					(u16)(UI_FACTORY_FRONTPANEL_MBOX_COL + IDN_KEY_RIGHT),IM_CONFIGCONNECT_2, IM_CONFIGCONNECT_2);
				break;
			case V_KEY_MENU:
				mbox_set_content_by_icon(factory_test.p_frontpanel_mbox, 
					(u16)(UI_FACTORY_FRONTPANEL_MBOX_COL + IDN_KEY_MENU),IM_CONFIGCONNECT_2, IM_CONFIGCONNECT_2);
				break;
			#ifdef FACTORY_TEST_FRONTSPANEL_ALL
			case V_KEY_POWER:
				mbox_set_content_by_icon(factory_test.p_frontpanel_mbox, 
					(u16)(UI_FACTORY_FRONTPANEL_MBOX_COL + IDN_KEY_POWER),IM_CONFIGCONNECT_2, IM_CONFIGCONNECT_2);
				break;
			#endif
			default:
				break;
		}
		ctrl_paint_ctrl(factory_test.p_frontpanel_mbox,TRUE);
	}
	else if(msg == MSG_SELECT)
	{
		factory_test_search_exit();
		#ifndef FACTORY_TEST_WITH_SEARCH
		factory_test_restore_to_factory();
		#endif
		ui_close_all_mennus();
	}
	else if(msg == MSG_EXIT)
	{
		factory_test_search_exit();
		manage_close_menu(ROOT_ID_FACTORY_TEST, 0, 0);
	}
	else
	{
		return ERR_FAILURE;
	}

	return SUCCESS;
}

static RET_CODE on_exit_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	factory_test_search_exit();
	manage_close_menu(ROOT_ID_FACTORY_TEST, 0, 0);
	return SUCCESS;
}

BEGIN_KEYMAP(ui_factory_cont_msg, NULL)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_EXIT, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
	ON_EVENT(V_KEY_POWER, MSG_EXIT)
END_KEYMAP(ui_factory_cont_msg, NULL)

BEGIN_MSGPROC(ui_factory_cont_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_EXIT, on_exit_all)
	ON_COMMAND(MSG_CA_CARD_INFO, on_factory_test_ca_info_update)
	ON_COMMAND(MSG_CA_INIT_OK, on_factory_test_ca_init_ok)
	ON_COMMAND(MSG_CA_EVT_NOTIFY, on_factory_test_ca_message_update)
	ON_COMMAND(MSG_SIGNAL_UPDATE, on_factory_test_update_signal)
	ON_COMMAND(MSG_SCAN_PROGRESS, on_factory_test_update_progress)	//search
	ON_COMMAND(MSG_SCAN_PG_FOUND, on_factory_test_update_program_num)
	ON_COMMAND(MSG_SCAN_FINISHED, on_factory_test_update_search_finished)
	ON_COMMAND(MSG_CONFIG_IP, on_factory_test_lan_connect)   //net
	ON_COMMAND(MSG_PING_TEST, on_factory_test_ping_test_update)
	ON_COMMAND(MSG_GET_WIFI_AP_CNT, on_factory_test_wifi_search_ok)
END_MSGPROC(ui_factory_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ui_factory_button_cont_msg, NULL)
	ON_EVENT(V_KEY_OK, 		MSG_SELECT)
	ON_EVENT(V_KEY_UP, 		MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, 	MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_LEFT, 	MSG_FOCUS_LEFT)
	ON_EVENT(V_KEY_RIGHT, 	MSG_FOCUS_RIGHT)
	ON_EVENT(V_KEY_MENU, 	MSG_EXIT)
	ON_EVENT(V_KEY_POWER, 	MSG_EXIT)
END_KEYMAP(ui_factory_button_cont_msg, NULL)

BEGIN_MSGPROC(ui_factory_button_cont_proc, cont_class_proc)
	ON_COMMAND(MSG_SELECT, on_exit_and_restore_all)
	ON_COMMAND(MSG_FOCUS_UP, on_exit_and_restore_all)
	ON_COMMAND(MSG_FOCUS_DOWN, on_exit_and_restore_all)
	ON_COMMAND(MSG_FOCUS_LEFT, on_exit_and_restore_all)
	ON_COMMAND(MSG_FOCUS_RIGHT, on_exit_and_restore_all)
	ON_COMMAND(MSG_EXIT, on_exit_and_restore_all)
END_MSGPROC(ui_factory_button_cont_proc, cont_class_proc)


