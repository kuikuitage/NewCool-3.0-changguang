/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#include "ui_common.h"
#include "ui_time_api.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_signal.h"
#include "ui_notify.h"
#include "ui_do_search.h"
#include "ui_ca_public.h"
#include "ui_new_mail.h"
#include "cas_ware.h"
#include "config_cas.h"

//mg
#define MAX_ON_SCREEN_MSG_LENGTH 320
typedef struct
{
  BOOL is_msg_need_show;
  BOOL is_cur_msg_showing;
  u16 roll_uni_str[CAS_MAX_MSG_DATA_LEN + 1];
  u8 roll_mode;
  u8 display_times;
  u8 location;
}on_screen_msg_t;

enum auto_feed_local_msg
{
  MSG_INSERT_MON_CARD = MSG_LOCAL_BEGIN + 625,
  MSG_INSERT_CHILD_CARD
};

static u8 finger_print_flag = 0;
static finger_msg_t *g_p_finger_info = NULL;

#define MAX_MEMORY_ROLL_MSG  5
typedef struct {
	BOOL ca_rolled_flag;
	u8 ca_roll_mode;
	u8 ca_roll_value;
	u8 ca_roll_pos;
	u16 ca_roll_uni_str[512];
}ca_roll_msg_t;
static ca_roll_msg_t g_ca_roll_msg[MAX_MEMORY_ROLL_MSG];

typedef struct
{
  u8 root_id;
  u8 state;
  void (*close_func)(void);
}mgcas_notify_menu_attr_t;

extern void ui_ppv_dlg_close(void);
extern void close_ca_pair_dlg(void);
extern void close_ca_burse_dlg(void);
extern void close_ca_entitle_expire_dlg(void);
extern RET_CODE open_finger_print(u32 para1, u32 para2);
extern RET_CODE ui_close_finger_menu(void);
RET_CODE on_ca_show_osd_DIVI(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static void on_desktop_start_roll_DIVI(msg_info_t *ca_osd_msg);


static mgcas_notify_menu_attr_t mgcas_popup_root[] =
{
	{ROOT_ID_CA_BURSE_CHARGE,	FALSE,	ui_ppv_dlg_close},
	{ROOT_ID_CA_PAIR,			FALSE,	close_ca_pair_dlg},
	{ROOT_ID_CA_ENTITLE_EXP_DLG,FALSE,	close_ca_entitle_expire_dlg},
	{ROOT_ID_CA_IPPV_PPT_DLG,	FALSE,	close_ca_burse_dlg},
};

//
#define OSD_ROLL_BAK_COUNT     10

typedef struct osd_roll_info_s {
	u8 valid;
	u32 msg_id;
	msg_info_t msg;
}osd_roll_info_t;

typedef struct osd_roll_state_s {
	u8 is_busy;
	osd_roll_info_t info[OSD_ROLL_BAK_COUNT];
}osd_roll_state_t;
static ca_roll_msg_t g_ca_roll_msg[MAX_MEMORY_ROLL_MSG];

// just top and buttom
static osd_roll_state_t osd_rolling[2];


static cas_sid_t g_ca_sid;

static BOOL g_is_need_clear_display = FALSE;
static BOOL g_is_smart_card_status = FALSE;
static BOOL g_is_smart_card_insert = FALSE;
static BOOL g_is_force_channel = FALSE;
static BOOL g_ca_msg_rolling_over = TRUE;
static BOOL g_osd_stop_msg_unsend = FALSE;
static u8 g_is_roll_bottom_show = 0;
static u8 g_is_roll_top_show = 0;
extern rsc_rstyle_t c_tab[MAX_RSTYLE_CNT];
extern rsc_fstyle_t f_tab[FSTYLE_CNT];
extern msg_info_t divi_OsdMsg;

extern u32 g_cas_id_flag;

extern void ui_desktop_start(void);
RET_CODE ui_ca_rolling_proc(control_t *p_ctrl, u16 msg,u32 para1, u32 para2);

enum ca_btn_id
{
  IDC_BTN_CA_CARD_INFO = 1,
  IDC_BTN_CA_PIN_MODIFY,
  IDC_BTN_CA_CHILD_CARD_FEED,
  IDC_BTN_CA_WORK_DURATION,
  IDC_BTN_CA_LEVEL_CONTROL,
  IDC_BTN_CA_ENTTITLE_INFO,
  IDC_BTN_CA_EMAIL,
};


#define DIVI_CONDITIONAL_ACCEPT_BTN_CNT      7
#define MAX_ON_SCREEN_MSG_LENGTH 			320

BOOL ui_is_need_clear_display(void)
{
  return g_is_need_clear_display;
}

void ui_set_smart_card_insert(BOOL status)
{
    g_is_smart_card_insert = status;
    //return 0;
}

BOOL ui_is_smart_card_insert(void)
{
  return g_is_smart_card_insert;
}

void ui_set_smart_card_state(BOOL status)
{
	g_is_smart_card_status = status;
}

BOOL ui_get_smart_card_state(void)
{
	return g_is_smart_card_status;
}

void ui_ca_set_sid(cas_sid_t *p_ca_sid)
{
 cmd_t cmd = {CAS_CMD_PLAY_INFO_SET_ANSYNC};

  DEBUG(MAIN,INFO,"CA: set sid pgid: %d\n", p_ca_sid->pgid);

  memcpy(&g_ca_sid, p_ca_sid, sizeof(cas_sid_t));

  cmd.data1 = (u32)(&g_ca_sid);

  ap_frm_do_command(APP_CA, &cmd);
}

static void ui_release_ap(void)
{
  ui_time_release();
  ui_epg_stop();
  ui_epg_release();
  ui_release_signal();
  ui_enable_uio(FALSE);
}

static void ui_activate_ap(void)
{
  ui_time_init();
  ui_epg_init();
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
  ui_init_signal();
  ui_enable_uio(TRUE);
}
void on_ca_zone_check_start(u32 cmd_id, u32 para1, u32 para2)
{
  /*stop all ap task,wait ca zone check end*/

  cmd_t cmd = {0};
  dvbc_lock_t tp_set = {0};
  static nim_channel_info_t nim_channel_info = {0};

  sys_status_get_main_tp1(&tp_set);
 #ifdef  ZONE_CHECK_TP_FRE_291
  nim_channel_info.frequency = 291000;
 #else
  nim_channel_info.frequency = tp_set.tp_freq;
 #endif
  nim_channel_info.param.dvbc.symbol_rate = tp_set.tp_sym;
  nim_channel_info.param.dvbc.modulation = tp_set.nim_modulate;

  ui_release_ap();
  
  cmd.id = cmd_id;
  cmd.data1 = (u32)(&nim_channel_info);
  cmd.data2 = ((CAS_INVALID_SLOT << 16) | CAS_ID_DIVI);
  OS_PRINTF("\r\n***ui_ca_zone_check_start cmd id[0x%x],data1[0x%x],data2[0x%x]**** ",
                cmd.id,cmd.data1,cmd.data2);
  ap_frm_do_command(APP_CA, &cmd);
}

void on_ca_zone_check_end(void)
{
  /*restart the ap task*/
 ui_activate_ap();
 ui_desktop_start();
}
void on_ca_zone_check_stop(void)
{
  #if 0
    /**no zone check funtion,it will set msg self**/
    control_t *p_ctrl = NULL;
    p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND);
    if(NULL != p_ctrl)
    {
        /*simulator msg :ca zone check end*/
        ctrl_process_msg(p_ctrl, MSG_CA_EVT_NOTIFY, 0, CAS_S_ZONE_CHECK_END);
    }
  #endif
}
void ui_cas_factory_set(u32 cmd_id, u32 para1, u32 para2)
{
#if 0
  cmd_t cmd = {0};

  cmd.id = cmd_id;
  cmd.data1 = para1;
  cmd.data2 = ((CAS_INVALID_SLOT << 16) | AP_CAS_ID);
  OS_PRINTF("\r\n***ui_cas_factory_set cmd id[0x%x],data1[0x%x],data2[0x%x]**** ",
                    cmd.id,cmd.data1,cmd.data2);
  ap_frm_do_command(APP_CA, &cmd);
#endif
}

static BOOL haveVaildFreq = FALSE;

void update_have_valid_freq_flag(void)
{
	u8 view_id = DB_DVBS_INVALID_VIEW;

	u16 i=0;
	u16 prog_id;
	u16 pg_count = 0;
	dvbs_prog_node_t prog;
	dvbs_tp_node_t tp = {0};

	view_id = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
	pg_count = db_dvbs_get_count(view_id);

	DEBUG(MAIN, INFO,"pg_count=%d view_id=%d\n",pg_count,view_id);
	if((pg_count == 0) || (pg_count == 0xFFFF))
	{
		DEBUG(MAIN, INFO,"no program \n");
		haveVaildFreq = FALSE;
		return;
	}

	for(i=0; i<pg_count; i++)
	{
		prog_id = db_dvbs_get_id_by_view_pos(view_id, i);
		db_dvbs_get_pg_by_id(prog_id, &prog);
		db_dvbs_get_tp_by_id((u16)prog.tp_id, &tp);

		DEBUG(MAIN, INFO,"tp.freq=%d \n",tp.freq);
		if(freq_is_valid(tp.freq))
		{
			DEBUG(MAIN, INFO,"valid freq: tp.freq=%d \n",tp.freq);
			haveVaildFreq = TRUE;
			return;
		}
	}

	haveVaildFreq = FALSE;
	return;
}

void set_have_valid_freq_flag(BOOL flag)
{
	haveVaildFreq = flag;
}


BOOL get_have_valid_freq_flag(void)
{
	return haveVaildFreq;
}

BOOL freq_is_valid(u32 freq)
{
	DEBUG(CAS,INFO,"freq = %d\n",freq);
	if((freq >= 690000) && (freq <= 738000) && (freq != 722000))
		return TRUE;
	else
		return FALSE;
}

static BOOL program_is_valid(void)
{
	u16 prog_id;
	dvbs_tp_node_t tp = {0};
	dvbs_prog_node_t pg;
	
	prog_id = sys_status_get_curn_group_curn_prog_id();
	db_dvbs_get_pg_by_id(prog_id, &pg);
	db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);

	return freq_is_valid(tp.freq);
}

extern BOOL is_desktop_init(void);

//mg
static void ui_update_mgca_notify(u16 root_id,u32 para1, u32 para2)
{
	u8 root_cnt = sizeof(mgcas_popup_root)/sizeof(mgcas_popup_root[0]);
	u8 index;

	DEBUG(CAS,INFO,"\n");
	if(RSC_INVALID_ID == root_id)
	{
		for(index = 0;index<root_cnt; index++)
		{
			if(mgcas_popup_root[index].state)
			{
				mgcas_popup_root[index].close_func();
				mgcas_popup_root[index].state = FALSE;
			}
		}

	}
	else
	{
		for(index = 0;index<root_cnt; index++)
		{
			if(root_id == mgcas_popup_root[index].root_id)
			{
				mgcas_popup_root[index].state = TRUE;
				manage_open_menu(root_id, para1, para2);
				break;
			}
		}
	}
}

static RET_CODE on_show_mgcas_notify(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	ipps_info_t *p_ippv_info = (ipps_info_t *) para2;

	update_ca_message(RSC_INVALID_ID);
	switch(msg)
	{
		case MSG_CA_SHOW_BURSE_CHARGE:
			DEBUG(CAS,INFO,"MSG_CA_SHOW_BURSE_CHARGE\n");
			ui_update_mgca_notify(ROOT_ID_CA_BURSE_CHARGE,0,para2);
			break;

		case MSG_CA_SHOW_ERROR_PIN_CODE:
			DEBUG(CAS,INFO,"MSG_CA_SHOW_ERROR_PIN_CODE\n");		
			ui_update_mgca_notify(ROOT_ID_CA_PAIR,para1,para2);
			break;

		case MSG_CA_AUTHEN_EXPIRE_DAY:
			DEBUG(CAS,INFO,"MSG_CA_AUTHEN_EXPIRE_DAY\n");
			ui_update_mgca_notify(ROOT_ID_CA_ENTITLE_EXP_DLG,msg,para2);
			break;

		case MSG_CA_OPEN_IPPV_PPT:
			DEBUG(CAS,INFO,"MSG_CA_OPEN_IPPV_PPT\n");
			if(p_ippv_info->p_ipp_info[0].type == 1)
			{
				ui_update_mgca_notify(ROOT_ID_CA_IPPV_PPT_DLG,1,(u32)para2);
			}
			break;
		case MSG_CA_HIDE_IPPV_PPT:
			DEBUG(CAS,INFO,"\n");
			manage_close_menu(ROOT_ID_CA_IPPV_PPT_DLG,1,(u32)para2);
			break;
	}

	return SUCCESS;
}

static RET_CODE on_finger_print(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u16 i, j;
	s8 asc_num[32];

	g_p_finger_info = (finger_msg_t *)para2;
	for(i = 0, j = 0; i < 16; i += 2, j++)
	{
		sprintf(&asc_num[i], "%02x", g_p_finger_info->data[j]);
	}

	if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
	{
		fw_destroy_mainwin_by_id(ROOT_ID_PROG_BAR);
	}
	finger_print_flag = 1;

	open_finger_print(0, (u32)(u32)asc_num);

	return SUCCESS;
}


static RET_CODE on_ca_hide_finger(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	DEBUG(CAS,INFO,"@@@@@ on_ca_hide_finger\n");
	if(fw_find_root_by_id(ROOT_ID_FINGER_PRINT) != NULL)
	{
		ui_close_finger_menu();
	}
	return SUCCESS;
}

void on_desktop_start_roll_MG(u16 *p_uni_str, u32 roll_mode, u32 value,u32 dis_pos)
{
	control_t *p_cont = NULL;
	control_t *p_ctrl = NULL;
	roll_param_t p_param = {0};

	value = value?value:2;
	DEBUG(CAS,INFO,"***mg osd_display.location:%d*** value = %d\n",dis_pos,value);

	p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	if(OSD_SHOW_TOP == dis_pos)
	{
		p_ctrl = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_TOP);
	}
	else
	{
		p_ctrl = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_BOTTOM);
	}

	gui_stop_roll(p_ctrl);
	ctrl_set_attr(p_ctrl, OBJ_STS_SHOW);
	text_set_content_by_unistr(p_ctrl, p_uni_str);
	p_param.pace 	 = ROLL_SINGLE;
	p_param.style 	 = ROLL_LR;
	p_param.repeats	 = value;
	p_param.is_force = 1;
	gui_start_roll(p_ctrl, &p_param);
	ctrl_paint_ctrl(p_ctrl,TRUE);
	
	g_ca_msg_rolling_over = FALSE;

}

static RET_CODE on_ca_rolling_msg_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	if(g_cas_id_flag  == CAS_ID_ADT_MG)
	{
		msg_info_t *p_ca_msg_info = NULL;
		u16 uni_str[512] = {0};//0x673a;
		u8 i = 0;
		p_ca_msg_info = (msg_info_t *)para2;
		if(NULL == p_ca_msg_info)
			return ERR_FAILURE;

		DEBUG(CAS,INFO,"OSD STR =%s\n", p_ca_msg_info->data);
		DEBUG(CAS,INFO,"OSD TYPE =%d\n", p_ca_msg_info->type);
		DEBUG(CAS,INFO,"roll_mode =%d\n", p_ca_msg_info->osd_display.roll_mode);
		DEBUG(CAS,INFO,"roll_value =%d\n", p_ca_msg_info->osd_display.roll_value);
		DEBUG(CAS,INFO,"OSD location =%d\n", p_ca_msg_info->osd_display.location);
		if(p_ca_msg_info->type == 0)
		{
			gb2312_to_unicode(p_ca_msg_info->data, 511, uni_str, 511);
			if(g_ca_msg_rolling_over)
			{
				on_desktop_start_roll_MG(uni_str,p_ca_msg_info->osd_display.roll_mode,
		 							          p_ca_msg_info->osd_display.roll_value,
		 							          p_ca_msg_info->osd_display.location);
				if(ui_is_fullscreen_menu(fw_get_focus_id()))
					ui_time_enable_heart_beat(TRUE);
			}
			else
			{
				for(i=0;i<MAX_MEMORY_ROLL_MSG;i++)
				{
					if(g_ca_roll_msg[i].ca_rolled_flag == FALSE)
						break;
				}
				
				if(i == MAX_MEMORY_ROLL_MSG)
					i = i - 1;
				
				DEBUG(CAS,INFO,"i =%d\n", i);
				memcpy(g_ca_roll_msg[i].ca_roll_uni_str,uni_str,511);
				g_ca_roll_msg[i].ca_roll_mode 	= (u8)p_ca_msg_info->osd_display.roll_mode;
				g_ca_roll_msg[i].ca_roll_value 	= p_ca_msg_info->osd_display.roll_value;
				g_ca_roll_msg[i].ca_roll_pos 	= p_ca_msg_info->osd_display.location;
				g_ca_roll_msg[i].ca_rolled_flag = TRUE;
			}
		}
		else
		{
			DEBUG(CAS_OSD, INFO,"content = %s \n",p_ca_msg_info->data);
			comm_dlg_data_t2 overdue_data =
			{
				ROOT_ID_INVALID,
				DLG_FOR_CONFIRM| DLG_STR_MODE_EXTSTR,
				DLG_FOR_CONFIRM| DLG_STR_MODE_EXTSTR,
				((SCREEN_WIDTH - 450) / 2), ((SCREEN_HEIGHT - 280) / 2),
				450, 280,
				0,(u32)p_ca_msg_info->title,
				0,(u32)p_ca_msg_info->data,
				0,
			};

			ui_comm_dlg_open3(&overdue_data);
		}
	}
	else
	{
		return on_ca_show_osd_DIVI(p_ctrl,msg,para1,para2);
	}
	return SUCCESS;
}

void ui_ca_rolling_post_stop_MG(void)
{
	u8 i = 0;

	for(i=0;i<MAX_MEMORY_ROLL_MSG;i++)
	{
		if(g_ca_roll_msg[i].ca_rolled_flag)
		{
			on_desktop_start_roll_MG(g_ca_roll_msg[i].ca_roll_uni_str,
			                            g_ca_roll_msg[i].ca_roll_mode,
			                            g_ca_roll_msg[i].ca_roll_value,
			                            g_ca_roll_msg[i].ca_roll_pos);
			g_ca_roll_msg[i].ca_rolled_flag = FALSE;
			DEBUG(CAS,INFO,"start roll memory osd i =%d\n", i);
			break;
		}
	}

	DEBUG(CAS,INFO,"roll end line : %d\n", i, __LINE__);
	if(i == MAX_MEMORY_ROLL_MSG)
	{
		g_ca_msg_rolling_over = TRUE;
		ui_time_enable_heart_beat(FALSE);
	}

	if(do_search_is_finish())	
	{
		ui_ca_do_cmd(CAS_CMD_OSD_ROLL_OVER, 0, 0);
	}
}

RET_CODE hotkey_green_process(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	DEBUG(CAS,INFO,"\n");
	ui_ca_do_cmd(CAS_CMD_IPPV_INFO_SYNC_GET, 0,0);
	return SUCCESS;
}


//mg
RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
  if(g_cas_id_flag  == CAS_ID_ADT_MG)
  {
 	u32 event_id = para2;
	control_t *p_cont = NULL;
	rect_t notify ={(SCREEN_WIDTH-350)/2, (SCREEN_HEIGHT-140)/2 -20,(SCREEN_WIDTH-350)/2 + 350, (SCREEN_HEIGHT-140)/2 +120};

	switch(event_id)
	{
		case CAS_S_ADPT_CARD_REMOVE:
			DEBUG(CAS,INFO,"CAS_S_ADPT_CARD_REMOVE\n");
			if(TRUE == ui_is_smart_card_insert())
			{
				ui_set_smart_card_insert(FALSE);
				ui_set_smart_card_state(FALSE);
				ui_update_mgca_notify(RSC_INVALID_ID,para1,para2);
				update_ca_message(IDS_CAS_S_CARD_REMOVE);
			}
			break;
		case CAS_S_ADPT_CARD_INSERT:
			DEBUG(CAS,INFO,"CAS_S_ADPT_CARD_INSERT\n");
			ui_set_smart_card_state(TRUE);
			update_ca_message(RSC_INVALID_ID);
			break;


		case CAS_C_EMM_APP_STOP://���ܿ�ֹͣ����
			DEBUG(CAS,INFO,"CAS_C_EMM_APP_STOP\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID, IDS_CA_E_APP_STOP);
			break;
		case CAS_C_EMM_APP_RESUME://���ܿ��ָ�����
			DEBUG(CAS,INFO,"CAS_C_EMM_APP_RESUME\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_APP_RESUME);
			break;
		case CAS_C_EMM_PROG_UNAUTH://ȡ����Ŀ����
			DEBUG(CAS,INFO,"CAS_C_EMM_PROG_UNAUTH\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_PROG_UNAUTH);
			break;
		case CAS_C_EMM_PROG_AUTH://���ӽ�Ŀ����
			DEBUG(CAS,INFO,"CAS_C_EMM_PROG_AUTH\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_PROG_AUTH);
			break;
		case CAS_C_EMM_UPDATE_GNO://���ܿ���Ÿ���
			DEBUG(CAS,INFO,"CAS_C_EMM_UPDATE_GNO\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_UPDATE_GNO);
			break;
		case CAS_S_CARD_NEED_UPG://���ܿ�������β忨
			DEBUG(CAS,INFO,"CAS_S_CARD_NEED_UPG\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_MG_CA_E_UPDATE_GK);
			break;
		case CAS_S_MODIFY_ZONECODE://���������
			DEBUG(CAS,INFO,"CAS_S_MODIFY_ZONECODE\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_UPDATE_ZONECODE);
			break;
		case CAS_C_EMM_UPDATE_COUT://���������
			DEBUG(CAS,INFO,"CAS_C_EMM_UPDATE_COUT\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_UPDATE_COUT);
			break;			
		case CAS_C_NOTIFY_UPDATE_CTRLCODE://���ܿ��޲������
			DEBUG(CAS,INFO,"���ܿ��޲������!\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_MODIFY_CTRLCODE);
			break;
		case CAS_C_NOTIFY_UPDATE_CRT://��Ȩ����
			DEBUG(CAS,INFO,"��Ȩ����!\n");
			close_notify();
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_UPDATE_CRT);
			break;
		case CAS_C_EN_PARTNER://����ƥ��
			DEBUG(CAS,INFO,"CAS_C_EN_PARTNER\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_EN_PAPTNER);
			break;
		case CAS_C_DIS_PARTNER://ȡ������ƥ��
			DEBUG(CAS,INFO,"CAS_C_DIS_PARTNER\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_E_DIS_PARTNER);
			break;
		case CAS_C_SET_SON_CARD://������ĸ��
			DEBUG(CAS,INFO,"������ĸ��!\n");
			ui_set_notify(&notify, NOTIFY_TYPE_STRID,IDS_CA_SET_MON_SON_CARD);
			break;
		case CAS_C_SHOW_NEW_EMAIL:
			DEBUG(CAS,INFO,"CAS_C_SHOW_NEW_EMAIL\n");
			ui_set_new_mail(TRUE);
			update_email_picture(EMAIL_PIC_SHOW);
			break;
		case CAS_C_HIDE_NEW_EMAIL:
			DEBUG(CAS,INFO,"CAS_C_HIDE_NEW_EMAIL\n");
			ui_set_new_mail(FALSE);
			update_email_picture(EMAIL_PIC_HIDE);
			break;
		case CAS_S_CLEAR_DISPLAY:
			DEBUG(CAS,INFO,"CAS_S_CLEAR_DISPLAY\n");
			update_ca_message(RSC_INVALID_ID);
			{
				p_cont = NULL;
				if((p_cont = fw_find_root_by_id(ROOT_ID_CA_PAIR)) != NULL)
				{
					close_ca_pair_dlg();
				}
			}
			break;  
			
		case CAS_C_EMM_CHILD_UNLOCK:
			DEBUG(CAS,INFO,"CAS_C_EMM_CHILD_UNLOCK\n");
			update_ca_message(IDS_CA_CLEAR_PARENT_LOCK);
			break;

		/*ECM message*/
		
		case CAS_E_CADR_NOT_ACT://�뼤���ӿ�
			DEBUG(CAS,INFO,"CAS_E_CADR_NOT_ACT\n");
			update_ca_message(IDS_CA_E_SCARD_NOT_ACT);
			break;
		case CAS_C_AUTH_LOGO_DISP://��ĿԤ��Ȩ
			DEBUG(CAS,INFO,"��ĿԤ��Ȩ!\n");
			manage_open_menu(ROOT_ID_CA_PRE_AUTH, 0, 0);
			break;
		case CAS_C_UPDATE_BALANCE://IPPV��Ŀ
			DEBUG(CAS,INFO,"IPPV��Ŀ\n");
			manage_open_menu(ROOT_ID_CA_PPV_ICON, 0, 0);
			break;
		case CAS_E_ZONE_CODE_ERR:
			DEBUG(CAS,INFO,"CAS_E_ZONE_CODE_ERR\n");
			update_ca_message(IDS_CA_E_SCARD_DISABLED);
			break;
		case CAS_E_CADR_NO_MONEY: //���ܿ�����
			DEBUG(CAS,INFO,"CAS_E_CADR_NO_MONEY\n");
			update_ca_message(IDS_CA_E_NO_MONEY);
			break;
		case CAS_E_PROG_UNAUTH://Ƶ��δ��Ȩ
			DEBUG(CAS,INFO,"CAS_E_PROG_UNAUTH\n");
			update_ca_message(IDS_NO_AUTHORIZED);
			break;  
		case CAS_E_CARD_AUTH_FAILED:
			DEBUG(CAS,INFO,"���ܿ���Ȩ����\n");
			update_ca_message(IDS_CA_ILLEGAL_AUTH);
			break;
		case CAS_E_CARD_LOCKED://���ܿ�����
			DEBUG(CAS,INFO,"CAS_E_CARD_LOCKED\n");
			update_ca_message(IDS_CAS_E_CARD_LOCKED);
			break;
		case CAS_E_SERVICE_LOCKED://�����ѱ���
			DEBUG(CAS,INFO,"CAS_E_SERVICE_LOCKED\n");
			update_ca_message(IDS_CA_E_APP_LOCKED);
			break;
		case CAS_E_EXTER_AUTHEN://���ܿ���֤ʧ��
			DEBUG(CAS,INFO,"CAS_E_EXTER_AUTHEN\n");
			update_ca_message(IDS_CA_E_EXTER_AUTHEN);
			break;
		case CAS_C_IPPT_BUY_OR_NOT://IPP��Ϣ
			DEBUG(CAS,INFO,"CAS_C_IPPT_BUY_OR_NOT\n");
			update_ca_message(IDS_IPPV_PROG);
			break;
		case CAS_E_CARD_NOT_FOUND://������ƥ��
			DEBUG(CAS,INFO,"CAS_E_CARD_NOT_FOUND\n");
			update_ca_message(IDS_CA_E_CARD_NOTMATCH);
			break;
		case CAS_E_CARD_FAILED://���ܿ�����ʧ��
			DEBUG(CAS,INFO,"CAS_E_CARD_FAILED\n");
			update_ca_message(IDS_CA_E_UPDATE_GR);
			break;
		case CAS_E_NOT_REG_CARD://��������ܿ�
			DEBUG(CAS,INFO,"CAS_E_NOT_REG_CARD\n");
			update_ca_message(IDS_CA_CARD_REMOVE);
			break;
		case CAS_C_ENTITLE_CLOSED://��Ȩ�ر�
			update_ca_message(IDS_CA_AUTH_CLOSED);
			break;
		case CAS_S_CARD_AUTHEN_UPDATING://��Ȩˢ��
			update_ca_message(IDS_CA_AUTH_UPDATE);
			break;
		case CAS_E_CARD_RENEW_RESET_STB://���ܿ�״̬���£�������
			update_ca_message(IDS_CA_STATUS_RENEW);
			break;
		case CAS_E_CARD_AUTHEN_OVERDUE:
			DEBUG(CAS,INFO,"CAS_E_CARD_AUTHEN_OVERDUE\n");
			update_ca_message(IDS_CA_AUTH_EXPIRED);
			break;
		case CAS_E_CARD_DATA_GET:// ���ܿ����ݴ���
			DEBUG(CAS,INFO,"���ܿ����ݴ���!\n");      
			update_ca_message(IDS_CA_CARD_DATA_ERROR);
			break;
		case CAS_E_ILLEGAL_CARD://���ܿ�ͨ�Ŵ���
			DEBUG(CAS,INFO,"���ܿ�ͨ�Ŵ���!\n");
			g_is_smart_card_insert = FALSE;
			update_ca_message(IDS_CA_IC_CMD_FAIL);
			break;
		case CAS_C_USERCDN_LIMIT_CTRL_ENABLE://���û��������޲�
			DEBUG(CAS,INFO,"���û��������޲�!\n");
			update_ca_message(IDS_CA_CARD_CONDITIONAL_LIMIT);
			break;
		case CAS_E_CARD_OPERATE_FAILE://���ܿ�����ʧ��
			DEBUG(CAS,INFO,"���ܿ�����ʧ��!\n");
			update_ca_message(IDS_CA_OPERATE_CARD_FAIL);
			break;
		case CAS_E_AUTHEN_EXPIRED://�н�Ŀ��Ȩ�Ѿ����ڣ��뼰ʱ����
			DEBUG(CAS,INFO,"�н�Ŀ��Ȩ�Ѿ����ڣ��뼰ʱ����\n");
			if(ui_is_fullscreen_menu(fw_get_focus_id()))
			{
				manage_open_menu(ROOT_ID_CA_ENTITLE_EXP_DLG, CAS_E_AUTHEN_EXPIRED, 0);
			}
			break;
		default:
			DEBUG(CAS,INFO,"on_ca_message_update : ui_get_curpg_scramble_flag == 1  default : event_id = [%d]\n",event_id);
			break;
	}

	DEBUG(CAS,INFO,"print for pc ilnt = 0x%x\n",p_cont);
 
  }
  else
  {
    u32 event_id = para2;  
    rect_t notify ={(SCREEN_WIDTH-350)/2, (SCREEN_HEIGHT-140)/2 -20,(SCREEN_WIDTH-350)/2 + 350, (SCREEN_HEIGHT-140)/2 +120};
  
    OS_PRINTF("\r\n*** ca_message_update event_id[0x%x]\r\n", event_id);
    if(event_id == CAS_E_PROG_UNAUTH)
    {
    	g_is_need_clear_display = TRUE;
    }
    else
    {
    	g_is_need_clear_display = FALSE;
    }
  
    switch(event_id)
    {
      case CAS_S_ZONE_CHECK_END:
        OS_PRINTF("\r\n*** %s :CAS_S_ZONE_CHECK_END ",__FUNCTION__);
        #ifndef WIN32
          on_ca_zone_check_end();
        #endif
        break;
      case CAS_C_CARD_ZONE_CHECK_OK://���ι��ܿ��������
        update_ca_message(IDS_CAS_C_CARD_ZONE_CHECK_OK);
        break;
      case CAS_C_CARD_ZONE_INVALID://�������ܿ����ٷ�����
        update_ca_message(IDS_CAS_C_CARD_ZONE_INVALID);
        break;
      case CAS_C_SHOW_NEW_EMAIL:
        update_email_picture(EMAIL_PIC_SHOW);
        break;
      case CAS_C_HIDE_NEW_EMAIL:
        update_email_picture(EMAIL_PIC_HIDE);
        break;
      case CAS_C_SHOW_SPACE_EXHAUST:
        update_email_picture(EMAIL_PIC_FLASH);
        break;
      case CAS_S_ADPT_CARD_INSERT:
  	  ui_set_smart_card_state(TRUE);
        update_ca_message(RSC_INVALID_ID);
  
  	  DEBUG(CAS,INFO,"CAS_S_ADPT_CARD_INSERT = %d \n",CAS_S_ADPT_CARD_INSERT);
        if(CUSTOMER_DTMB_CHANGSHA_HHT == CUSTOMER_ID)
        {
  
  		if(program_is_valid())
  		{
  			DEBUG(CAS,INFO,"ui_get_curpg_scramble_flag = %d \n",ui_get_curpg_scramble_flag());
  			if(ui_get_curpg_scramble_flag())
  			{
  				update_ca_message(IDS_CA_CARD_INIT);
  			}
  		}
  		else
  		{
  			menu_attr_t *p_attr;
  
  			p_attr = manage_get_curn_menu_attr();
  			if((ui_is_fullscreen_menu(p_attr->root_id)
  				|| ui_is_preview_menu(p_attr->root_id)
  				|| ui_is_popup_menu(p_attr->root_id))
  				&&(is_desktop_init()))
  			{
  				ui_set_notify_during(5000);
  				ui_set_notify(&notify, NOTIFY_TYPE_STRID, IDS_CA_CARD_INIT);
  			}		
  		}
        }
        break;
      case CAS_S_ADPT_CARD_REMOVE:
        ui_set_smart_card_state(FALSE);
        ui_set_smart_card_insert(FALSE);
        update_ca_message(IDS_CA_CRYPTED_PROG_INSERT_SMC);
  	  if(CUSTOMER_DTMB_CHANGSHA_HHT == CUSTOMER_ID && !program_is_valid())
  	  {
  	  	  DEBUG(CAS,INFO,"ui_get_curpg_scramble_flag = %d \n",ui_get_curpg_scramble_flag());
  	  	  if(!ui_get_curpg_scramble_flag())
  	  	  {
  		  	update_ca_message(IDS_CA_CARD_REMOVE);
  	  	  }
  
  		  if(is_ap_playback_active()&& ui_is_playing())
  		  	ui_stop_play(STOP_PLAY_BLACK, TRUE);
  		  update_signal();
  	  }
        break;
      case CAS_E_PROG_UNAUTH: //���ܿ��û�����Ȩ
        update_ca_message(IDS_HHT_ENTITLE_EXPIRED);
        break;
      case CAS_E_CONDITION_DISPLAY:           //�����޲�
        update_ca_message(IDS_CAS_C_USERCDN_LIMIT_CTRL_ENABLE2);
        break;
      case CAS_E_CARD_INIT_INSTALL: //��ʼ����װ��������ʹ��
        update_ca_message(IDS_CAS_E_CARD_INIT_INSTALL);
        break;
  
      case CAS_E_CARD_INSTALLING: //���ڰ�װ�����ȴ�����
        update_ca_message(IDS_CAS_E_CARD_INSTALLING);
        break;
      case CAS_E_CADR_NO_MONEY:
        update_ca_message(IDS_CAS_E_CADR_NO_MONEY2);
        break;
      case CAS_E_CARD_REPAIR:          //ά��ͣ��
        update_ca_message(IDS_CAS_E_CARD_REPAIR);
        break;
      case CAS_E_CARD_STOP: //���뱨ͣͣ��
        update_ca_message(IDS_CAS_E_CARD_STOP);
        break;
      case CAS_E_CARD_LOGOUT: //ע��״̬
        update_ca_message(IDS_CAS_E_CARD_LOGOUT);
        break;
      case CAS_E_CARD_OUT_WORK_TIME:
        update_ca_message(IDS_CAS_E_CARD_OUT_WORK_TIME2);
        break;
      case CAS_E_PROG_LEVEL_LOWER:
        update_ca_message(IDS_CAS_E_PROG_LEVEL_LOWER);
        break;   
      case CAS_E_CARD_MONEY_LACK:
        update_ca_message(IDS_CAS_E_CARD_MONEY_LACK);
        break;  	
      case CAS_E_CADR_TEMP_ACT:
        update_ca_message(IDS_CAS_E_CADR_TEMP_ACT);
        break; 
      case CAS_E_CARD_EXPIRED:
        update_ca_message(IDS_SUB_EXPIRED_ACTIVATE);
        break; 
      case CAS_E_CARD_PARTNER_FAILED:
        update_ca_message(IDS_CAS_E_CARD_PARTNER_FAILED2);
        break;
      case CAS_E_ILLEGAL_CARD:
        update_ca_message(IDS_CAS_E_ILLEGAL_CARD2);
        break;
      case CAS_S_CARD_REMOVE: 	
        update_ca_message(IDS_CAS_S_CARD_REMOVE2);
        break;
      case CAS_S_CLEAR_DISPLAY:
        update_ca_message(RSC_INVALID_ID);
        break;
      default:
        break;
    }
  }
   return SUCCESS;
}

#define CA_ROLLING_TOP_X			(400)
#define CA_ROLLING_TOP_W			(SCREEN_WIDTH - CA_ROLLING_TOP_X)
#define CA_ROLLING_TOP_H			(56)
#define CA_ROLLING_TOP_Y			(38)
				 
#define CA_ROLLING_BUTTON_X			(0)
#define CA_ROLLING_BUTTON_W			(SCREEN_WIDTH - CA_ROLLING_BUTTON_X)
#define CA_ROLLING_BUTTON_H			(56)
#define CA_ROLLING_BUTTON_Y			(SCREEN_HEIGHT - CA_ROLLING_BUTTON_H - CA_ROLLING_TOP_Y)

#define CA_ROLLING_X			(400)
#define CA_ROLLING_W			(SCREEN_WIDTH - CA_ROLLING_X)
#define CA_ROLLING_H			(56)

RET_CODE open_ca_rolling_menu(u32 para1, u32 para2)
{
	if(g_cas_id_flag  == CAS_ID_ADT_MG)
	{
		

 
		control_t *p_cont = NULL;
		control_t *p_txt_top = NULL;
		control_t *p_txt_bottom = NULL;
	 
		p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	
		// top roll control
		p_txt_top = ctrl_create_ctrl(CTRL_TEXT, IDC_BG_MENU_CA_ROLL_TOP,
							CA_ROLLING_TOP_X, CA_ROLLING_TOP_Y,
							CA_ROLLING_TOP_W, CA_ROLLING_TOP_H,
							p_cont, 0);
		ctrl_set_proc(p_txt_top, ui_ca_rolling_proc);
		ctrl_set_sts(p_txt_top,OBJ_STS_HIDE);
		ctrl_set_rstyle(p_txt_top,RSI_TRANSPARENT,RSI_TRANSPARENT,RSI_TRANSPARENT);
		text_set_font_style(p_txt_top, FSI_WHITE_36, FSI_WHITE_36, FSI_WHITE_36);
		text_set_align_type(p_txt_top,STL_LEFT|STL_VCENTER);
		text_set_content_type(p_txt_top, TEXT_STRTYPE_UNICODE);
	
		//bottom roll control
		p_txt_bottom = ctrl_create_ctrl(CTRL_TEXT, IDC_BG_MENU_CA_ROLL_BOTTOM,
							CA_ROLLING_BUTTON_X, CA_ROLLING_BUTTON_Y,
							CA_ROLLING_BUTTON_W, CA_ROLLING_BUTTON_H,
							p_cont, 0);
		ctrl_set_proc(p_txt_bottom, ui_ca_rolling_proc);
		ctrl_set_sts(p_txt_bottom,OBJ_STS_HIDE);
		ctrl_set_rstyle(p_txt_bottom,RSI_TRANSPARENT,RSI_TRANSPARENT,RSI_TRANSPARENT);
		text_set_font_style(p_txt_bottom, FSI_WHITE_36, FSI_WHITE_36, FSI_WHITE_36);
		text_set_align_type(p_txt_bottom,STL_LEFT|STL_VCENTER);
		text_set_content_type(p_txt_bottom, TEXT_STRTYPE_UNICODE);

	}
	else
	{
  	  control_t *p_cont = NULL;
  	  control_t *p_txt_top = NULL;
  	  control_t *p_txt_bottom = NULL;
  	  
  	  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  	  
  	  // top roll control
  	  p_txt_top = ctrl_create_ctrl(CTRL_TEXT, IDC_BG_MENU_CA_ROLL_TOP,
  	                           CA_ROLLING_X, CA_ROLLING_TOP_Y,
  	                           CA_ROLLING_W,CA_ROLLING_H,
  	                           p_cont, 0);
  	  ctrl_set_proc(p_txt_top, ui_ca_rolling_proc);
  	  ctrl_set_sts(p_txt_top,OBJ_STS_HIDE);
  	  //ctrl_set_rstyle(p_txt_top, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  	  text_set_font_style(p_txt_top, FSI_ROLL_OSD, FSI_ROLL_OSD, FSI_ROLL_OSD);
  	  text_set_align_type(p_txt_top,STL_LEFT|STL_VCENTER);
  	  text_set_content_type(p_txt_top, TEXT_STRTYPE_UNICODE);
  
  	  //bottom roll control
  	  p_txt_bottom = ctrl_create_ctrl(CTRL_TEXT, IDC_BG_MENU_CA_ROLL_BOTTOM,
  	                           CA_ROLLING_X, CA_ROLLING_BUTTON_Y,
  	                           CA_ROLLING_W,CA_ROLLING_H,
  	                           p_cont, 0);
  	  ctrl_set_proc(p_txt_bottom, ui_ca_rolling_proc);
  	  ctrl_set_sts(p_txt_bottom,OBJ_STS_HIDE);
  	  //ctrl_set_rstyle(p_txt_bottom, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  	  text_set_font_style(p_txt_bottom, FSI_ROLL_OSD, FSI_ROLL_OSD, FSI_ROLL_OSD);
  	  text_set_align_type(p_txt_bottom,STL_LEFT|STL_VCENTER);
  	  text_set_content_type(p_txt_bottom, TEXT_STRTYPE_UNICODE);
  }
  return SUCCESS;
}
 
static RET_CODE on_ca_init_ok(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	if(g_cas_id_flag  == CAS_ID_ADT_MG)
	{
		DEBUG(CAS,INFO,"on_ca_init_ok\n");
		ui_set_smart_card_insert(TRUE);
		if(finger_print_flag)
		{
			if(fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
			{
			fw_destroy_mainwin_by_id(ROOT_ID_PROG_BAR);
			}
#ifndef WIN32
			on_finger_print(p_ctrl, 0, 0, (u32)g_p_finger_info);
#endif
			finger_print_flag = 0;
		}
	}
	else
	{
		DEBUG(MAIN,INFO,"on_ca_init_ok\n");
		ui_set_smart_card_insert(TRUE);

		if(CUSTOMER_DTMB_CHANGSHA_HHT == CUSTOMER_ID && is_ap_playback_active())
		{
			fw_tmr_create(ROOT_ID_BACKGROUND, MSG_TIMER_CARD_OVERDUE, 1000, TRUE);
		}
	}
	return SUCCESS;
}
static RET_CODE on_ca_play_cur_pg(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	static u8 count;

	DEBUG(MAIN,INFO,"delay to card init ok\n");
	if((is_ap_playback_active() && ui_is_playing())|| (count == 20))
	{
		update_signal();
		fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_TIMER_CARD_OVERDUE);
		count = 0;
	}
	else
	{
		if(get_card_reset_finsh())
			ui_play_curn_pg();
	}
	count ++;
	return SUCCESS;
}
 
static RET_CODE on_ca_clear_OSD(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
   DEBUG(CAS_OSD, INFO,"\n");
   ui_set_playpg_scrambled(FALSE);
   update_signal();
   return SUCCESS;
}
BOOL get_force_channel_status(void)
{
  return g_is_force_channel;
}
static RET_CODE on_ca_force_channel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret = ERR_FAILURE;
  u16 view_count = 0, pg_id = 0;
  u16 cur_pg_id = 0;
  static u16 pre_pg_id = INVALID_PG_ID;
  u8 view_id;
  u8 i = 0;
  u8 focus_id = 0;
  
  dvbs_prog_node_t pg = {0};
  cas_force_channel_t *p_force_channel_info = NULL;
  p_force_channel_info = (cas_force_channel_t *) para2;
  if(g_cas_id_flag  == CAS_ID_ADT_MG)
  {
 	rect_t notify ={(SCREEN_WIDTH-350)/2, (SCREEN_HEIGHT-140)/2 -20,(SCREEN_WIDTH-350)/2 + 350, (SCREEN_HEIGHT-140)/2 +120};
	if(MSG_CA_FORCE_CHANNEL_INFO == msg)
	{
		if(NULL == p_force_channel_info)
		{
			return ERR_FAILURE;
		}

		view_id = ui_dbase_get_pg_view_id();
		view_count = db_dvbs_get_count(view_id);
		cur_pg_id = sys_status_get_curn_group_curn_prog_id();

		for(i=0; i<view_count; i++)
		{
			pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
			if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
			{
				//mgca not notice ts_id and netwrok_id
				#if 0
				if((pg.s_id == p_force_channel_info->serv_id)
							&& (pg.ts_id == p_force_channel_info->ts_id)
							&& (pg.orig_net_id == p_force_channel_info->netwrok_id)
							&&(pg_id != cur_pg_id))
				#endif
				if((pg.s_id == p_force_channel_info->serv_id)&&(pg_id != cur_pg_id))							
				{
					focus_id = fw_get_focus_id();
					if(!ui_is_fullscreen_menu(focus_id))
					{
						ui_close_all_mennus();
					}
					//set_uio_status(FALSE);
					ui_play_prog(pg_id, FALSE);
					pre_pg_id = cur_pg_id;
					ret = SUCCESS;
					break;
				}
				else
				{
					pre_pg_id = INVALID_PG_ID;
				}
			}
		}
		ui_set_notify(&notify, NOTIFY_TYPE_STRID, IDS_CA_SERVICE_EMERGENCE);
	}
	else
	{
		//set_uio_status(TRUE);
		if(pre_pg_id != INVALID_PG_ID)
		{
			ui_play_prog(pre_pg_id, FALSE);
			pre_pg_id = INVALID_PG_ID;
		}
		ret = SUCCESS;
	}
 }

 else
 {
  
    switch(msg)
    {
      case MSG_CA_FORCE_CHANNEL_INFO:
      if(NULL == p_force_channel_info)
      {
        return ERR_FAILURE;
      }
     
      view_id = ui_dbase_get_pg_view_id();
      view_count = db_dvbs_get_count(view_id);
      cur_pg_id = sys_status_get_curn_group_curn_prog_id();
  
      for(i=0; i<view_count; i++)
      {
          pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
          if (db_dvbs_get_pg_by_id(pg_id, &pg) == DB_DVBS_OK)
         {
            if(pg.s_id == p_force_channel_info->serv_id
              && pg.ts_id == p_force_channel_info->ts_id
              && pg.orig_net_id == p_force_channel_info->netwrok_id)
            {
                focus_id = fw_get_focus_id();
                if(!ui_is_fullscreen_menu(focus_id))
                {
                  ui_close_all_mennus();
                }
                if(ui_is_mute())
                {
                  ui_set_mute(FALSE);
                }
                ui_play_prog(pg_id, FALSE);
                g_is_force_channel = TRUE;
                #ifdef TEMP_SUPPORT_DS_AD
  
                if(TRUE == ui_get_ads_osd_status())
                {
                  ui_ads_osd_roll_stop();
                }
                #endif
                //fill_prog_info(fw_find_root_by_id(ROOT_ID_PROG_BAR), TRUE, pg_id);
                DEBUG(MAIN,INFO,"pg_id [%d] , cur_pg_id [%d]\n", pg_id, cur_pg_id);
                if(pg_id != cur_pg_id)
                {
                  pre_pg_id = cur_pg_id;
                  manage_notify_root(ROOT_ID_BACKGROUND, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR, 0, 0);
                  //set_uio_key_status(TRUE);
                }
                else
                {
                  pre_pg_id = INVALID_PG_ID;
                  //set_uio_key_status(TRUE);
                }
                ret = SUCCESS;
                break;
            }
         }
      }
      break;
     case MSG_CA_UNFORCE_CHANNEL_INFO:
        g_is_force_channel = FALSE;
        //set_uio_key_status(FALSE);
        if(pre_pg_id != INVALID_PG_ID)
        {
          ui_play_prog(pre_pg_id, FALSE);
          pre_pg_id = INVALID_PG_ID;
         }
        break;
      default:
        break;
    }
  }
  return ret;
}

static RET_CODE on_new_mail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(g_cas_id_flag  == CAS_ID_ADT_MG)
  {
  	cas_mail_headers_t *p_mail_header = (cas_mail_headers_t*)para2;
	u32 new_mail = 0;
	s8 i =0;
	u8 index;
	index = fw_get_focus_id();
  
	if(index != ROOT_ID_DO_SEARCH&& do_search_is_finish())
	{
		DEBUG(MAIN,INFO,"max_num = %d\n",p_mail_header->max_num);
		if((0 != p_mail_header->max_num) && (0xFFFF != p_mail_header->max_num))
		{
			for(i = 0; i < p_mail_header->max_num; i++)
			{
				if(p_mail_header->p_mail_head[i].new_email)
					new_mail++;
			}

			DEBUG(MAIN,INFO,"new_mail = %d\n",new_mail);
			if(new_mail > 0)
			{
				ui_set_new_mail(TRUE);
				update_email_picture(EMAIL_PIC_SHOW);
			}
			else if(p_mail_header->max_num == 50)
			{
				ui_set_new_mail(TRUE);
				update_email_picture(EMAIL_PIC_FLASH);
			}
			else
			{
				ui_set_new_mail(FALSE);
				update_email_picture(EMAIL_PIC_HIDE);
			}
		}
		else
		{
			ui_set_new_mail(FALSE);
			update_email_picture(EMAIL_PIC_HIDE);
		}
	}
  }
  else
  {
    cas_mail_headers_t *p_mail_header = (cas_mail_headers_t*)para2;
    u32 new_mail = 0;
    s8 i =0;
    menu_attr_t *p_attr;
  
    p_attr = manage_get_curn_menu_attr();
  
    if((0 != p_mail_header->max_num) && (0xFFFF != p_mail_header->max_num))
    {
      for(i = 0; i < p_mail_header->max_num; i++)
      {
        if(p_mail_header->p_mail_head[i].new_email)
          new_mail++;
      }
      if(new_mail > 0)
      {
        ui_set_new_mail(TRUE);
        if(!ui_is_fullscreen_menu(p_attr->root_id)
        && !ui_is_popup_menu(p_attr->root_id))
        {
          return SUCCESS;
        }
        open_ui_new_mail(0, 0);
      }
    }
  }
  return SUCCESS;
}

BOOL osd_roll_stop_msg_unsend(void)
{
  return g_osd_stop_msg_unsend;
}

BOOL get_ca_osd_hide_state(void)
{
	control_t * p_ctrl_top = NULL;
	control_t * p_ctrl_bottom = NULL;
	control_t *p_background = NULL;

	DEBUG(CAS_OSD,INFO,"\n");
	p_background = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	p_ctrl_top = ctrl_get_child_by_id(p_background, IDC_BG_MENU_CA_ROLL_TOP);
	p_ctrl_bottom = ctrl_get_child_by_id(p_background, IDC_BG_MENU_CA_ROLL_BOTTOM);

	if(OBJ_STS_HIDE == ctrl_get_sts(p_ctrl_top) && OBJ_STS_HIDE == ctrl_get_sts(p_ctrl_bottom))
		return TRUE;

	return FALSE;
}

BOOL get_ca_msg_rolling_status(void)
{
  return g_ca_msg_rolling_over;
}

void set_ca_msg_rolling_status(BOOL is_over)
{
  g_ca_msg_rolling_over = is_over;
}

void ui_ca_send_rolling_over(u32 cmd_id, u32 para1, u32 para2)
{
  cmd_t cmd = {0};
  cmd.id = cmd_id;
  cmd.data1 = para1;
  cmd.data2 = ((CAS_INVALID_SLOT << 16) | AP_CAS_ID);
  ap_frm_do_command(APP_CA, &cmd);
  return;
}


static void on_desktop_roll_set_font_size_and_ctrl_h(u8 *font_size, u8 *display_pos)
{
  control_t *p_cont = NULL;
  control_t *p_ctrl_top = NULL;
  control_t *p_ctrl_bottom = NULL;
  rect_t frm = {0};
  s16 ctrl_h = 0;
  
  
  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  p_ctrl_top = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_TOP);
  p_ctrl_bottom = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_BOTTOM);
  switch(*font_size)
  {
    case 1:
      f_tab[FSI_ROLL_OSD1].width = 18;
      f_tab[FSI_ROLL_OSD1].height = 18;
      ctrl_h = 28;
      break;
    case 2:
      f_tab[FSI_ROLL_OSD1].width = 22;
      f_tab[FSI_ROLL_OSD1].height = 22;
      ctrl_h = 32;
      break;
    case 3:
      f_tab[FSI_ROLL_OSD1].width = 26;
      f_tab[FSI_ROLL_OSD1].height = 26;
      ctrl_h = 36;
      break;
    case 4:
      f_tab[FSI_ROLL_OSD1].width = 30;
      f_tab[FSI_ROLL_OSD1].height = 30;
      ctrl_h = 40;
      break;
    case 5:
      f_tab[FSI_ROLL_OSD1].width = 34;
      f_tab[FSI_ROLL_OSD1].height = 34;
      ctrl_h = 44;
      break;
    case 6:
      f_tab[FSI_ROLL_OSD1].width = 38;
      f_tab[FSI_ROLL_OSD1].height = 38;
      ctrl_h = 48;
      break;
    case 7:
      f_tab[FSI_ROLL_OSD1].width = 42;
      f_tab[FSI_ROLL_OSD1].height = 42;
      ctrl_h = 52;
      break;
    case 8:
      f_tab[FSI_ROLL_OSD1].width = 46;
      f_tab[FSI_ROLL_OSD1].height = 46;
      ctrl_h = 56;
      break;
    case 9:
      f_tab[FSI_ROLL_OSD1].width = 50;
      f_tab[FSI_ROLL_OSD1].height = 50;
      ctrl_h = 60;
      break;
    case 10:
      f_tab[FSI_ROLL_OSD1].width = 54;
      f_tab[FSI_ROLL_OSD1].height = 54;
      ctrl_h = 64;
      break;
    case 11:
      f_tab[FSI_ROLL_OSD1].width = 58;
      f_tab[FSI_ROLL_OSD1].height = 58;
      ctrl_h = 68;
        break;
    case 12:
      f_tab[FSI_ROLL_OSD1].width = 62;
      f_tab[FSI_ROLL_OSD1].height = 62;
      ctrl_h = 72;
      break;
    default:
      f_tab[FSI_ROLL_OSD1].width = 38;
      f_tab[FSI_ROLL_OSD1].height = 38;
      ctrl_h = 48;
      break;
  }

  switch(*display_pos)
  {
    case 0://top
      ctrl_get_frame(p_ctrl_top, &frm);
      frm.bottom = frm.top + ctrl_h;
      ctrl_set_frame(p_ctrl_top, &frm);
      ctrl_set_mrect(p_ctrl_top, 0, 0, (u16)(frm.right - frm.left), (u16)(frm.bottom - frm.top));
      break;
    case 1://bottom
      ctrl_get_frame(p_ctrl_bottom, &frm);
      frm.top = frm.bottom - ctrl_h;
      ctrl_set_frame(p_ctrl_bottom, &frm);
      ctrl_set_mrect(p_ctrl_bottom, 0, 0, (u16)(frm.right - frm.left), (u16)(frm.bottom - frm.top));
      break;
    case 2://top + bottom
      ctrl_get_frame(p_ctrl_top, &frm);
      frm.bottom = frm.top + ctrl_h;
      ctrl_set_frame(p_ctrl_top, &frm);
      ctrl_set_mrect(p_ctrl_top, 0, 0, (u16)(frm.right - frm.left), (u16)(frm.bottom - frm.top));
      
      ctrl_get_frame(p_ctrl_bottom, &frm);
      frm.top = frm.bottom - ctrl_h;
      ctrl_set_frame(p_ctrl_bottom, &frm);
      ctrl_set_mrect(p_ctrl_bottom, 0, 0, (u16)(frm.right - frm.left), (u16)(frm.bottom - frm.top));
      break;
    default:
      OS_PRINTF("error display_pos , do nothing!!!!!\n");
      break;
  }
}

void on_desktop_start_roll_position(u16 *uni_str, u8 *display_back_color, u8 *display_color, u8 *display_front_size, u8 *display_pos)
{
  control_t *p_cont = NULL;
  control_t *p_ctrl_top = NULL;
  control_t *p_ctrl_bottom = NULL;
  roll_param_t p_param;
  u32 color = 0, color_bg = 0;
  u8 i = 0;
  BOOL bcolor = FALSE;

  memset(&p_param, 0, sizeof(roll_param_t));
  
  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  for(i=0;i<4;i++)
  {
      if(display_back_color[i] != display_color[i])
      {
          bcolor = TRUE;
          break;
      }
  }
  color_bg = ((u32)(display_back_color[3]) <<24) + ((u32)(display_back_color[2]) << 16)
          + ((u32)(display_back_color[1]) << 8) + ((u32)(display_back_color[0]));  
  c_tab[RSI_ROLL_OSD].bg.value = color_bg;  

  if(!bcolor)
  {
      color = 0xffffffff;/*background color is the same as font color ;default font color is white*/
  }
  else
  {
      color = ((u32)(display_color[3]) <<24) +(u32)( (display_color[2]) << 16)
                + ((u32)(display_color[1]) << 8) + (u32)((display_color[0]));
  }

  f_tab[FSI_ROLL_OSD1].color = color;	
  
  on_desktop_roll_set_font_size_and_ctrl_h(display_front_size, display_pos);

  if(*display_pos == 0)
  {
    p_ctrl_top = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_TOP);
    text_set_font_style(p_ctrl_top, FSI_ROLL_OSD1, FSI_ROLL_OSD1, FSI_ROLL_OSD1);
    ctrl_set_rstyle(p_ctrl_top, RSI_ROLL_OSD, RSI_ROLL_OSD, RSI_ROLL_OSD);
    gui_stop_roll(p_ctrl_top);
    ctrl_set_sts(p_ctrl_top, OBJ_STS_SHOW);
    text_set_content_by_unistr(p_ctrl_top, uni_str);
    p_param.pace = ROLL_SINGLE;
    p_param.style = ROLL_LR;
    p_param.repeats= 1;
    p_param.is_force = 1;
    p_param.use_bg = TRUE;
    p_param.bg_color = color_bg;
    gui_start_roll(p_ctrl_top, &p_param);
    ctrl_paint_ctrl(p_ctrl_top,TRUE);
    g_is_roll_top_show = 1;
    g_ca_msg_rolling_over = FALSE;
  }
  else if(*display_pos == 1)
  {
    p_ctrl_bottom = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_BOTTOM);
    text_set_font_style(p_ctrl_bottom, FSI_ROLL_OSD1, FSI_ROLL_OSD1, FSI_ROLL_OSD1);
    ctrl_set_rstyle(p_ctrl_bottom, RSI_ROLL_OSD, RSI_ROLL_OSD, RSI_ROLL_OSD);
  
    gui_stop_roll(p_ctrl_bottom);
    ctrl_set_sts(p_ctrl_bottom, OBJ_STS_SHOW);
    text_set_content_by_unistr(p_ctrl_bottom, uni_str);
    p_param.pace = ROLL_SINGLE;
    p_param.style = ROLL_LR;
    p_param.repeats= 1;
    p_param.is_force = 1;
    p_param.use_bg = TRUE;
    p_param.bg_color = color_bg;
    gui_start_roll(p_ctrl_bottom, &p_param);
    ctrl_paint_ctrl(p_ctrl_bottom,TRUE);
    g_is_roll_bottom_show = 1;
    g_ca_msg_rolling_over = FALSE;
  }
  else if(*display_pos == 2)
  {
    p_ctrl_top = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_TOP);
    p_ctrl_bottom = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_BOTTOM);
    text_set_font_style(p_ctrl_top, FSI_ROLL_OSD1, FSI_ROLL_OSD1, FSI_ROLL_OSD1);
    text_set_font_style(p_ctrl_bottom, FSI_ROLL_OSD1, FSI_ROLL_OSD1, FSI_ROLL_OSD1); 
    ctrl_set_rstyle(p_ctrl_top, RSI_ROLL_OSD, RSI_ROLL_OSD, RSI_ROLL_OSD);
    ctrl_set_rstyle(p_ctrl_bottom, RSI_ROLL_OSD, RSI_ROLL_OSD, RSI_ROLL_OSD);
    gui_stop_roll(p_ctrl_top);
    gui_stop_roll(p_ctrl_bottom);
    ctrl_set_sts(p_ctrl_top, OBJ_STS_SHOW);
    ctrl_set_sts(p_ctrl_bottom, OBJ_STS_SHOW);
    text_set_content_by_unistr(p_ctrl_top, uni_str);
    text_set_content_by_unistr(p_ctrl_bottom, uni_str);

    p_param.pace = ROLL_SINGLE;
    p_param.style = ROLL_LR;
    p_param.repeats= 1;
    p_param.is_force = 1;
    p_param.use_bg = TRUE;
    p_param.bg_color = color_bg;
    gui_start_roll(p_ctrl_top, &p_param);
    gui_start_roll(p_ctrl_bottom, &p_param);
    ctrl_paint_ctrl(p_ctrl_top,TRUE);
    ctrl_paint_ctrl(p_ctrl_bottom,TRUE);

    g_is_roll_top_show = 1;
    g_is_roll_bottom_show = 1;
    g_ca_msg_rolling_over = FALSE;

  }
}

static void on_desktop_start_roll_DIVI(msg_info_t *ca_osd_msg)
{
  msg_info_t *p_ca_msg_info = NULL;
  u16 uni_str[MAX_ON_SCREEN_MSG_LENGTH] = {0};
  p_ca_msg_info = (msg_info_t *)ca_osd_msg;
 
 DEBUG(CAS_OSD, INFO, "display_pos:%d\n", p_ca_msg_info->display_pos);
 DEBUG(CAS_OSD, INFO, "background_area:%d\n", p_ca_msg_info->background_area);
 DEBUG(CAS_OSD, INFO, "background_color:%#x\n", p_ca_msg_info->background_color); 
 DEBUG(CAS_OSD, INFO, "color_type:%d\n", p_ca_msg_info->color_type);
 DEBUG(CAS_OSD, INFO, "content_len:%d\n", p_ca_msg_info->content_len);
 DEBUG(CAS_OSD, INFO, "data_len:%d\n", p_ca_msg_info->data_len);
 DEBUG(CAS_OSD, INFO, "data:%s\n", p_ca_msg_info->data);
 DEBUG(CAS_OSD, INFO, "display_back_color:%d %d %d %d\n", p_ca_msg_info->display_back_color[0],p_ca_msg_info->display_back_color[1],p_ca_msg_info->display_back_color[2],p_ca_msg_info->display_back_color[3]);
 DEBUG(CAS_OSD, INFO, "display_color:%#x\n", p_ca_msg_info->display_color);
 DEBUG(CAS_OSD, INFO, "display_front:%d\n", p_ca_msg_info->display_front);
 DEBUG(CAS_OSD, INFO, "display_front_size:%d\n", p_ca_msg_info->display_front_size);
 DEBUG(CAS_OSD, INFO, "display_status:%d\n", p_ca_msg_info->display_status);
 DEBUG(CAS_OSD, INFO, "font_color:%#x\n", p_ca_msg_info->font_color);
 DEBUG(CAS_OSD, INFO, "display_time:%#x\n", p_ca_msg_info->display_time);
 DEBUG(CAS_OSD, INFO, "period:%#x\n", p_ca_msg_info->period);

  gb2312_to_unicode(p_ca_msg_info->data,
                               MAX_ON_SCREEN_MSG_LENGTH -1, uni_str, MAX_ON_SCREEN_MSG_LENGTH - 1);

  if(CUSTOMER_XINNEW_GAIZHOU == CUSTOMER_ID)
  {
	p_ca_msg_info->display_pos = 1;
  }
  on_desktop_start_roll_position(uni_str, p_ca_msg_info->display_back_color,
                              p_ca_msg_info->display_color, &p_ca_msg_info->display_front_size, &p_ca_msg_info->display_pos);
}


RET_CODE on_ca_show_osd_DIVI(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	msg_info_t *p_ca_msg_info = NULL;
	static u32 msg_id = 1000;
	p_ca_msg_info = (msg_info_t *)para2;
	UI_PRINTF("\r\n*** rolling_msg_update  p_ca_msg_info addr[0x%x], msg:[%d]***\r\n", para2, msg);

	if(p_ca_msg_info == NULL)
	{
		return ERR_FAILURE;
	}
	p_ca_msg_info->display_pos = divi_OsdMsg.osd_display.location;
	p_ca_msg_info->display_front_size = 8;
	if((TRUE == g_ca_msg_rolling_over)
			&& (osd_rolling[p_ca_msg_info->display_pos].is_busy == 0))
	{
		OS_PRINTF("SHOW OSD \n");
		OS_PRINTF("display_pos: %d!\n",p_ca_msg_info->display_pos);
		OS_PRINTF("display_front: %d!\n",p_ca_msg_info->display_front);
		OS_PRINTF("display_front_size: %d!\n",p_ca_msg_info->display_front_size);
		OS_PRINTF("display_back_color: 0x%x-%x-%x-%x !\n",p_ca_msg_info->display_back_color[0],p_ca_msg_info->display_back_color[1],p_ca_msg_info->display_back_color[2],p_ca_msg_info->display_back_color[3]);
		OS_PRINTF("display_color: 0x%x-%x-%x-%x!\n",p_ca_msg_info->display_color[0],p_ca_msg_info->display_color[1],p_ca_msg_info->display_color[2],p_ca_msg_info->display_color[3]);
		OS_PRINTF("content_len: %d!\n",p_ca_msg_info->content_len);
		OS_PRINTF("OSD data len:%d\n",p_ca_msg_info->data_len);
		OS_PRINTF("OSD data:%s\n",p_ca_msg_info->data);
		on_desktop_start_roll_DIVI(p_ca_msg_info);
		#ifdef ADS_DIVI_SUPPORT
		if(ui_is_fullscreen_menu(fw_get_focus_id()) && ui_get_ads_osd_roll_over())
		#else
		if(ui_is_fullscreen_menu(fw_get_focus_id()))
		#endif
			ui_time_enable_heart_beat(TRUE);
		osd_rolling[p_ca_msg_info->display_pos].is_busy = 1;
		//reset store OSD bak msg_id
		msg_id = 1000;
	}
	else
	{
		u8 i;
		OS_PRINTF("*** busy bak:***\n");
		for(i=0;i<OSD_ROLL_BAK_COUNT;i++)
		{
			if (osd_rolling[p_ca_msg_info->display_pos].info[i].valid == 0)
				break;
		}
		if (i != OSD_ROLL_BAK_COUNT) 
		{
			memcpy(&(osd_rolling[p_ca_msg_info->display_pos].info[i].msg),p_ca_msg_info,sizeof(msg_info_t));
			osd_rolling[p_ca_msg_info->display_pos].info[i].valid = 1;
			osd_rolling[p_ca_msg_info->display_pos].info[i].msg_id = msg_id;
			msg_id++;
		}
		else 
		{
			OS_PRINTF("No bak buffer to store roll msg!!\n");
		}
		for (i=0;i<OSD_ROLL_BAK_COUNT;i++) 
		{
			OS_PRINTF("OSD_ROLL_BAK_COUNT----^^^^^^^^^^^^^^^^^^i= %d,	msg= %s,   msg index = %d,    msg_id = %d,	valid = %d^^^^^^^^^^^^^^^^^^^^^^\n",i,
									  osd_rolling[p_ca_msg_info->display_pos].info[i].msg.data,
									  osd_rolling[p_ca_msg_info->display_pos].info[i].msg.index,
									  osd_rolling[p_ca_msg_info->display_pos].info[i].msg_id,
									  osd_rolling[p_ca_msg_info->display_pos].info[i].valid);
		}
	}
	return SUCCESS;

}

static void ui_ca_rolling_post_stop_DIVI(control_t *p_ctrl)
{
	u8 which,i;
	u8 temp_index;
	u32 min_msg_id;
	control_t *tmp = NULL, *proot = NULL;
	//confirm which rolling test has stoped
	proot = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	tmp = ctrl_get_child_by_id(proot, IDC_BG_MENU_CA_ROLL_TOP);
	if (p_ctrl == tmp)
	{
		which = OSD_SHOW_TOP;
	}
	else
	{
		tmp = ctrl_get_child_by_id(proot, IDC_BG_MENU_CA_ROLL_BOTTOM);
		if (p_ctrl == tmp)
			which = OSD_SHOW_BOTTOM;
		else 
			which = OSD_SHOW_TOP;
	}
	
	// find need to show osd info on the same ops
	for (i=0;i<OSD_ROLL_BAK_COUNT;i++) 
	{
		if (osd_rolling[which].info[i].valid == 1)
		{
			min_msg_id = osd_rolling[which].info[i].msg_id;
			temp_index = i;
			//find the store OSD which msg id is min
			for(;i<OSD_ROLL_BAK_COUNT;i++)
			{
				if((osd_rolling[which].info[i].valid == 1)
						&&(min_msg_id > osd_rolling[which].info[i].msg_id))
				{
					min_msg_id = osd_rolling[which].info[i].msg_id;
					temp_index = i;
				}
			}
			i = temp_index;
			break;
		}
	}
	if(i < (OSD_ROLL_BAK_COUNT) 
			&& (osd_rolling[which].info[i].valid == 1))
	{
		OS_PRINTF("which:%d, msg_id:%d\n", which, osd_rolling[which].info[i].msg_id);
		//roll the OSD which msg id is min
		on_desktop_start_roll_DIVI(&(osd_rolling[which].info[i].msg));
		//roll over, reset msg_id
		osd_rolling[which].info[i].msg_id = 0;
		osd_rolling[which].info[i].valid = 0;
		osd_rolling[which].is_busy = 1;
		OS_PRINTF("osd reroll.valid:%d,  msg:%s\n", 
					osd_rolling[which].info[i].valid, osd_rolling[which].info[i].msg.data);
	}	
	else  // if not 
	{
		OS_PRINTF("which:%d, is_busy:%d\n", which, osd_rolling[which].is_busy);
		osd_rolling[which].is_busy = 0;
		// swap
		if (which == OSD_SHOW_TOP)
			which = OSD_SHOW_BOTTOM;
		else
			which = OSD_SHOW_TOP;
		// find the against pos store msg
		for (i=0;i<OSD_ROLL_BAK_COUNT;i++) 
		{
			if (osd_rolling[which].info[i].valid == 1)
			{
				min_msg_id = osd_rolling[which].info[i].msg_id;
				temp_index = i;
				//find the store OSD which msg id is min
				for(;i<OSD_ROLL_BAK_COUNT;i++)
				{
					if((osd_rolling[which].info[i].valid == 1)
							&&(min_msg_id > osd_rolling[which].info[i].msg_id))
					{
						min_msg_id = osd_rolling[which].info[i].msg_id;
						temp_index = i;
					}
				}
				i = temp_index;
				break;
			}
		}
		if(i < OSD_ROLL_BAK_COUNT 
				&& (osd_rolling[which].info[i].valid == 1))
		{
			OS_PRINTF("which:%d, msg_id:%d\n", which, osd_rolling[which].info[i].msg_id);
			//roll the OSD which msg id is min
			on_desktop_start_roll_DIVI(&(osd_rolling[which].info[i].msg));
			//roll over, reset msg_id
			osd_rolling[which].info[i].msg_id = 0;
			osd_rolling[which].info[i].valid = 0;
			osd_rolling[which].is_busy = 1;
			OS_PRINTF("osd reroll.valid:%d,  msg:%s\n", 
					osd_rolling[which].info[i].valid, osd_rolling[which].info[i].msg.data);
		}	
		else
		{
			if(do_search_is_finish())
			{
				ui_ca_do_cmd(CAS_CMD_OSD_ROLL_OVER, 0, 0);
				g_osd_stop_msg_unsend = FALSE;
			}
			else
			{
				g_osd_stop_msg_unsend = TRUE;
			}
			g_ca_msg_rolling_over = TRUE;
			ui_time_enable_heart_beat(FALSE);
		}
	}
}
void on_ca_roll_osd_hide(void)
{
	control_t *p_cont = NULL;
  control_t *p_ctrl_top = NULL;
  control_t *p_ctrl_bottom = NULL;
  rect_t frame = {0};
  
  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);
  
  if(g_is_roll_top_show)
  {
   p_ctrl_top = ctrl_get_child_by_id(p_cont, IDC_BG_MENU_CA_ROLL_TOP);
  ctrl_set_sts(p_ctrl_top ,OBJ_STS_HIDE);
  ctrl_get_frame(p_ctrl_top, &frame);

  gui_stop_roll(p_ctrl_top);

  ctrl_add_rect_to_invrgn(p_cont, &frame);
  ctrl_paint_ctrl(p_cont, FALSE);
    ui_ca_send_rolling_over(CAS_CMD_OSD_ROLL_OVER, 0, 0);
    g_is_roll_top_show = 0;
  }

  if(g_is_roll_bottom_show)
  {
   p_ctrl_bottom = ctrl_get_child_by_id(p_cont, IDC_BG_MENU_CA_ROLL_BOTTOM);
  ctrl_set_sts(p_ctrl_bottom ,OBJ_STS_HIDE);
  ctrl_get_frame(p_ctrl_bottom, &frame);

  gui_stop_roll(p_ctrl_bottom);

  ctrl_add_rect_to_invrgn(p_cont, &frame);
  ctrl_paint_ctrl(p_cont, FALSE);
    ui_ca_send_rolling_over(CAS_CMD_OSD_ROLL_OVER, 0, 0);
    g_is_roll_bottom_show = 0;
  }
}
static RET_CODE on_ca_rolling_stop(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  rect_t frame = {0};
  control_t *p_parent = NULL;

  MT_ASSERT(p_ctrl != NULL);
  ctrl_set_sts(p_ctrl,OBJ_STS_HIDE);
  ctrl_get_frame(p_ctrl, &frame);

  gui_stop_roll(p_ctrl);
  p_parent = ctrl_get_parent(p_ctrl);
  MT_ASSERT(p_parent != NULL);

  ctrl_add_rect_to_invrgn(p_parent, &frame);
  ctrl_paint_ctrl(p_parent, FALSE);
  if(g_cas_id_flag  == CAS_ID_ADT_MG)
  {
	ui_ca_rolling_post_stop_MG();
  }
  else
  {
  	ui_ca_rolling_post_stop_DIVI(p_ctrl);
  }
  return SUCCESS;
}

RET_CODE on_ca_rolling_show(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t * p_ctrl_top = NULL;
	control_t * p_ctrl_bottom = NULL;
	control_t *p_background = NULL;

	DEBUG(CAS_OSD,INFO," on_ca_rolling_show \n");
	p_background = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	p_ctrl_top = ctrl_get_child_by_id(p_background, IDC_BG_MENU_CA_ROLL_TOP);
	p_ctrl_bottom = ctrl_get_child_by_id(p_background, IDC_BG_MENU_CA_ROLL_BOTTOM);

	DEBUG(CAS_OSD,INFO," ctrl_is_rolling = %d \n",ctrl_is_rolling(p_ctrl_top));
	if((NULL != p_ctrl_top) && (ctrl_is_rolling(p_ctrl_top))&&(!g_ca_msg_rolling_over))
	{
		DEBUG(CAS_OSD,INFO," on_ca_rolling_show \n");
		ctrl_set_sts(p_ctrl_top,OBJ_STS_SHOW);
		gui_resume_roll(p_ctrl_top);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(TRUE);
	}
	if((NULL != p_ctrl_bottom) && (ctrl_is_rolling(p_ctrl_bottom))&&(!g_ca_msg_rolling_over))
	{
		DEBUG(CAS_OSD,INFO," on_ca_rolling_show \n");
		ctrl_set_sts(p_ctrl_bottom,OBJ_STS_SHOW);
		gui_resume_roll(p_ctrl_bottom);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(TRUE);
	}
	return SUCCESS;
}

RET_CODE on_ca_rolling_hide(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t * p_ctrl_top = NULL;
	control_t * p_ctrl_bottom = NULL;
	control_t *p_background = NULL;

	DEBUG(CAS_OSD,INFO," on_ca_rolling_hide \n");
	p_background = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	p_ctrl_top = ctrl_get_child_by_id(p_background, IDC_BG_MENU_CA_ROLL_TOP);
	p_ctrl_bottom = ctrl_get_child_by_id(p_background, IDC_BG_MENU_CA_ROLL_BOTTOM);
	if((NULL != p_ctrl_top) && (ctrl_is_rolling(p_ctrl_top)))
	{
		DEBUG(CAS_OSD,INFO," on_ca_rolling_hide \n");
		ctrl_set_sts(p_ctrl_top,OBJ_STS_HIDE);
		gui_pause_roll(p_ctrl_top);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(FALSE);
	}
	if((NULL != p_ctrl_bottom) && (ctrl_is_rolling(p_ctrl_bottom)))
	{
		DEBUG(CAS_OSD,INFO," on_ca_rolling_hide \n");
		ctrl_set_sts(p_ctrl_bottom,OBJ_STS_HIDE);
		gui_pause_roll(p_ctrl_bottom);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(FALSE);
	}
	return SUCCESS;
}

RET_CODE on_ca_hide_osd(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont = NULL;
  control_t *p_ctrl_top = NULL;
  control_t *p_ctrl_bottom = NULL;
  
  DEBUG(CAS_OSD,INFO,"msg = %d \n",msg);
  p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);

  if(ui_is_fullscreen_menu(fw_get_focus_id()))
  {
	  if(g_is_roll_top_show)
	  {
		g_is_roll_top_show = 0;
		p_ctrl_top = ctrl_get_child_by_id(p_cont, IDC_BG_MENU_CA_ROLL_TOP);
		gui_pause_roll(p_ctrl_top);
		ctrl_set_attr(p_ctrl_top ,OBJ_STS_HIDE);
		ctrl_paint_ctrl(p_cont, TRUE);
		ui_time_enable_heart_beat(FALSE);
	  }
	  
	  if(g_is_roll_bottom_show)
	  {
		g_is_roll_bottom_show =0;
		p_ctrl_bottom = ctrl_get_child_by_id(p_cont,IDC_BG_MENU_CA_ROLL_BOTTOM);
		gui_pause_roll(p_ctrl_bottom);
		ctrl_set_attr(p_ctrl_bottom ,OBJ_STS_HIDE);
		ctrl_paint_ctrl(p_cont, FALSE);
		ui_time_enable_heart_beat(FALSE);
	  }
  }

  g_ca_msg_rolling_over = TRUE;
  return SUCCESS;
}

static RET_CODE on_ca_stop_cur_pg(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
  u16 pg_id = 0;
  u32 index = 0;
  dvbs_prog_node_t pg = {0};
  u32 pid_param = 0; // (u32)*(u32*)para2;
  u16 ecm_pid = 0; 
  u16 service_id = 0;
  u16 temp = 0;

  if(para2 == 0)
  {
    DEBUG(MAIN,INFO,"@@ PARA2 error, return;");
    return SUCCESS;
  }
  pid_param = (u32)*(u32*)para2;
  ecm_pid = (u16)(pid_param >> 16);
  service_id = (u16)(pid_param & 0xFFFF);
  if(is_ap_playback_active())
  {
  	pg_id = sys_status_get_curn_group_curn_prog_id();
  	db_dvbs_get_pg_by_id(pg_id, &pg);
  
  //undescramble pg ecm_pid 0x1fff
  if((0x1FFF == ecm_pid) && (pg.s_id == service_id))
  {
    DEBUG(MAIN,INFO,"@@@@  current pg ecm_pid = %d ,ca_sys ecm_pid = %d ,current pg service_id = %d, cas service_id = %d \n",
                      temp, ecm_pid,pg.s_id,service_id);
    //ui_stop_play(STOP_PLAY_BLACK, TRUE);
    return SUCCESS;
  }
  
  //descramble pg
  for(index = 0; index < (u32)(int)pg.ecm_num; index++)
  {
   	temp = pg.cas_ecm[index].ecm_pid;
   	DEBUG(MAIN,INFO,"@@@@ index = %d, current pg ecm_pid = %d ,ca_sys ecm_pid = %d ,current pg service_id = %d, cas service_id = %d \n",
       index, temp, ecm_pid,pg.s_id,service_id);
       if(((pg.cas_ecm[index].ecm_pid == ecm_pid) && ( pg.s_id == service_id) ))
    	{
      		ui_stop_play(STOP_PLAY_BLACK, TRUE);
    	}
   }
 }
  return SUCCESS;
}


//----------------------------------------

BEGIN_MSGPROC(ui_desktop_proc_cas, cont_class_proc)
	ON_COMMAND(MSG_CA_INIT_OK, on_ca_init_ok)
	ON_COMMAND(MSG_CA_CLEAR_DISPLAY, on_ca_clear_OSD)
	ON_COMMAND(MSG_CA_EVT_NOTIFY, on_ca_message_update)
	ON_COMMAND(MSG_CA_FINGER_INFO, on_finger_print)
	ON_COMMAND(MSG_CA_HIDE_FINGER, on_ca_hide_finger)
	ON_COMMAND(MSG_CA_FORCE_CHANNEL_INFO, on_ca_force_channel)
	ON_COMMAND(MSG_CA_UNFORCE_CHANNEL_INFO, on_ca_force_channel) 

	ON_COMMAND(MSG_CA_MAIL_HEADER_INFO, on_new_mail)
	ON_COMMAND(MSG_CA_SHOW_OSD, on_ca_rolling_msg_update)

	ON_COMMAND(MSG_CA_SHOW_BURSE_CHARGE, 	on_show_mgcas_notify)
	ON_COMMAND(MSG_CA_SHOW_ERROR_PIN_CODE,	on_show_mgcas_notify)
	ON_COMMAND(MSG_CA_AUTHEN_EXPIRE_DAY, 	on_show_mgcas_notify)
	ON_COMMAND(MSG_CA_OPEN_IPPV_PPT, 		on_show_mgcas_notify)
	ON_COMMAND(MSG_CA_HIDE_IPPV_PPT, 		on_show_mgcas_notify)

	ON_COMMAND(MSG_HOTKEY_GREEN, hotkey_green_process)

	ON_COMMAND(MSG_CA_HIDE_OSD, on_ca_hide_osd)
	ON_COMMAND(MSG_CA_STOP_CUR_PG, on_ca_stop_cur_pg)
	
	ON_COMMAND(MSG_TIMER_CARD_OVERDUE, on_ca_play_cur_pg)

END_MSGPROC(ui_desktop_proc_cas, cont_class_proc)

BEGIN_KEYMAP(ui_desktop_keymap_cas, NULL)
	ON_EVENT(V_KEY_MAIL,MSG_OPEN_MENU_IN_TAB | ROOT_ID_EMAIL_MESS)
	ON_EVENT(V_KEY_HOT_XREFRESH,MSG_HOTKEY_GREEN)
END_KEYMAP(ui_desktop_keymap_cas, NULL)

BEGIN_MSGPROC(ui_ca_rolling_proc, text_class_proc)
	ON_COMMAND(MSG_ROLL_STOPPED, on_ca_rolling_stop)
END_MSGPROC(ui_ca_rolling_proc, text_class_proc)
//----------------------------------------
