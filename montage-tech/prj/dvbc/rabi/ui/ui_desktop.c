/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_time_api.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_signal.h"
#include "ui_notify.h"
#include "ui_do_search.h"
#include "ui_nprog_bar.h"
#include "ui_xupgrade_by_rs232.h"
#include "ethernet.h"
#include "pnp_service.h"
#include "net_svc.h"
#include "ui_ca_api.h"
#include "ui_xupgrade_by_usb.h"
#include "ui_dump_by_usb.h"
#include "ui_sleep_hotkey.h"
//#include "ui_dvr_config.h"
#include "ui_book_api.h"
#include "ui_hdd_info.h"
//#include "ui_timeshift_bar.h"
//#include "ui_record_manager.h"
#include "ui_timer.h"
//#include "hotplug.h"
#include "wifi.h"
#include "modem.h"
#include "ui_xlanguage.h"
//#include "ui_dlna_api.h"
//#include "ui_satip.h"
#include "network_monitor.h"
#include "ui_video.h"

#include "ui_video.h"
#include "NetMediaDataProvider.h"
#include "ui_live_tv_api.h"

#include "IIPTVDataProvider.h"
#include "ui_iptv_api.h"
#include "SM_StateMachine.h"
#include "States_Desktop.h"
#include "plug_monitor.h"
#include "network_monitor.h"
#include "cas_manager.h"

#include "authorization.h"

#include "iqiyiTV_C.h"
#include "stdbool.h"
#include "iqy_porting_function.h"
#include "vfs.h"

#if ENABLE_TEST_SPEED
#include "commondata.h"
#endif

#define ReturnCheck(Ret_In, Ret_Out, Stand)\
{\
  if(Ret_In == Stand)return Ret_Out;\
}

void auto_connect_ethernet();
void do_cmd_disconnect_g3();
u16 sys_evtmap(u32 event);

u16 ui_desktop_keymap_on_normal(u16 key);

u16 ui_desktop_keymap_on_vcr(u16 key);

u16 ui_desktop_keymap_on_standby(u16 key);

RET_CODE ui_desktop_proc_on_normal(control_t *p_ctrl, u16 msg,
								   u32 para1, u32 para2);

#if ENABLE_TTX_SUBTITLE
u16 ui_desktop_keymap_on_ttx(u16 key);

RET_CODE ui_desktop_proc_on_ttx(control_t *p_ctrl, u16 msg,
								u32 para1, u32 para2);
#endif

extern RET_CODE ui_get_ip(char *ipaddr);
extern void paint_connect_status(BOOL is_conn, BOOL is_paint);
extern void paint_wifi_connect_status(BOOL is_conn, BOOL is_paint);
extern void on_config_ip_update(ethernet_device_t * p_dev);
extern void on_config_ip_update_wifi(ethernet_device_t * p_dev);
extern void paint_connect_wifi_status(BOOL is_conn, BOOL is_paint);
extern u32 get_3g_status();

extern void set_temp_wifi_info(wifi_info_t temp);
extern u16 get_wifi_select_focus();
extern void paint_list_field_is_connect(u16 index, BOOL is_connect, BOOL is_paint);
extern void paint_wifi_status(BOOL is_conn, BOOL is_paint);
//extern void al_netmedia_register_msg(void);

extern s32 get_music_is_usb();
extern s32 get_picture_is_usb();
extern s32 get_video_is_usb();
extern u16 get_wifi_connecting_focus();
extern void set_3g_connect_status(u32 conn_stats, u32 conn_signal, u8 *operator);
extern void set_gprs_connect_status(u32 conn_stats);
extern void paint_wifi_list_field_not_connect(control_t *p_list);
static void auto_connect_wifi();
static void close_all_network_menus();
void do_cmd_connect_g3(g3_conn_info_t *p_g3_info);

static BOOL g_vcr_input_is_tv = FALSE;
#ifndef WIN32
static BOOL g_usb_status = FALSE; //FALSE: USB OUT; TRUE:USB IN
#else
static BOOL g_usb_status = TRUE; //FALSE: USB OUT; TRUE:USB IN
#endif
static net_conn_stats_t g_net_connt_stats = {FALSE, FALSE, FALSE, FALSE};

static BOOL g_deskto_init = FALSE;
static BOOL g_deskto_first_enter = FALSE;


#ifdef SLEEP_TIMER
//is the sleep timer has one minute left.
static BOOL g_is_one_min_left = FALSE;
static u16 usb_focus = 0;
static ethernet_cfg_t   wifi_ethcfg = {0};
static ethernet_device_t* p_wifi_devv = NULL;
static wifi_info_t p_wifi_info = {{0},{0}};
static net_conn_info_t g_net_conn_info = {0,0,0,MODEM_DEVICE_NO_SIM_CARD};
static BOOL g_is_book_node_dlg_opened = FALSE;
static BOOL is_start_ttx = FALSE; //just for satip check
//static BOOL is_usb_wifi = FALSE; //add for check plug out device is wifi
static desk_sts_mch_t g_desktop = {0};
static   void *file_live = NULL;
static   vfs_file_info_t file_info;


BOOL ui_get_ttx_flag()
{
    return is_start_ttx;
}

void ui_set_ttx_flag(BOOL flag)
{
    is_start_ttx = flag;
}

net_conn_info_t ui_get_conn_info()
{
    return g_net_conn_info;
}

void ui_set_conn_info(net_conn_info_t net_conn_info)
{
    g_net_conn_info = net_conn_info;
}

u16 ui_get_usb_focus(void)
{
  return usb_focus;
}


void ui_set_usb_focus(u16 focus)
{
  usb_focus = focus;
}

void ui_sleep_timer_create(void)
{
  u32 auto_sleep = 0;
  u32 tm_out;
  utc_time_t sleep_time = {0};

  ui_sleep_timer_destory();

  auto_sleep = sys_status_get_auto_sleep();

  if(auto_sleep != 0)
  {
	sys_status_get_sleep_time(&sleep_time);
	tm_out = (((sleep_time.hour * 60) + sleep_time.minute) * 60000);
	DEBUG(DBG, INFO, "tm out %d, %02d:%02d\n", tm_out,sleep_time.hour, sleep_time.minute);


    fw_tmr_create(ROOT_ID_BACKGROUND, MSG_SLEEP_TMROUT, tm_out, FALSE);
  }
}


void ui_sleep_timer_destory(void)
{

	fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_SLEEP_TMROUT);
}


void ui_sleep_timer_reset(void)
{
	utc_time_t sleep_time;
	u32 tm_out;
	BOOL is_sleep_on;

	sys_status_get_status(BS_SLEEP_LOCK, &is_sleep_on);

	if(is_sleep_on)
	{
		sys_status_get_sleep_time(&sleep_time);

		printf_time(&sleep_time, "sleep timer create");

		tm_out = (((sleep_time.hour * 60) + sleep_time.minute) * 60000);

		fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_SLEEP_TMROUT, tm_out);
	}
}


void uio_reset_sleep_timer(void)
{
	ui_set_notify_autoclose(TRUE);
	if(g_is_one_min_left)
	{
		close_notify();
		//destory one minute timer.
		fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_POWER_OFF);

		//create sleep timer by sleep time.
		ui_sleep_timer_create();
	}
	else
	{
		ui_sleep_timer_reset();
	}

	g_is_one_min_left = FALSE;
}
#endif

#if ENABLE_TTX_SUBTITLE
extern u8 ui_get_ttx_curn_sub();
extern void ui_set_ttx_curn_sub(u8 index);
static u8 sg_sub_lan_backup;
void start_ttx(void)
{
  language_set_t lang_set;
  gdi_set_enable(FALSE);

  manage_enable_autoswitch(FALSE);
  fw_destroy_all_mainwin(TRUE);
  manage_enable_autoswitch(TRUE);

  sg_sub_lan_backup = ui_get_ttx_curn_sub();
  sys_status_get_lang_set(&lang_set);
  if( lang_set.osd_text == LANGUAGE_MAX_CNT-2 || lang_set.osd_text == LANGUAGE_MAX_CNT-1)
    ui_set_ttx_curn_sub(1<<6); //arab or persian
  else
    ui_set_ttx_curn_sub(0); //latin

  ui_config_ttx_osd();
  gdi_set_enable(TRUE);

  vbi_set_font_size_vsb(TTX_FONT_HD);
  ui_set_ttx_display(TRUE, (u32)gdi_get_screen_handle(FALSE), -1, -1);
  ui_set_ttx_flag(TRUE);
}


static void end_ttx(void)
{
  ui_set_ttx_curn_sub(sg_sub_lan_backup);//restore subtitle language
  gdi_set_enable(FALSE);
  manage_enable_autoswitch(TRUE);
  // stop ttx
  ui_set_ttx_display(FALSE, 0, -1, -1);
  gdi_clear_screen();
  ui_config_normal_osd();
  gdi_set_enable(TRUE);
  ui_set_ttx_flag(FALSE);
}
#endif

static void enter_standby(u32 tm_out)
{
	cmd_t cmd;

	cmd.id = AP_FRM_CMD_STANDBY;
	cmd.data1 = tm_out;
	cmd.data2 = 0;

	ap_frm_do_command(APP_FRAMEWORK, &cmd);
}


static void reset_screen(BOOL is_ntsc)
{
	rect_t orc;
	s16 x, y;
	//BOOL is_9pic;
	u8 prv_root_id = ui_get_preview_menu();

	gdi_set_enable(FALSE);

	if (prv_root_id != ROOT_ID_INVALID)
	{
		if(manage_find_preview(prv_root_id, &orc) != INVALID_IDX)
		{
			avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
				orc.left, orc.top, orc.right, orc.bottom);
		}
	}

	//is_9pic = (BOOL)(fw_find_root_by_id(ROOT_ID_MULTIVIEW) != NULL);
	//if (is_9pic)
	//{
	//  x = is_ntsc ? SCREEN_9PIC_POS_NTSC_L : SCREEN_9PIC_POS_PAL_L;
	//  y = is_ntsc ? SCREEN_9PIC_POS_NTSC_T : SCREEN_9PIC_POS_PAL_T;
	//}
	//else
	//{
	x = is_ntsc ? SCREEN_POS_NTSC_L : SCREEN_POS_PAL_L;
	y = is_ntsc ? SCREEN_POS_NTSC_T : SCREEN_POS_PAL_T;
	//}

	gdi_move_screen(x, y);
	gdi_set_enable(TRUE);

	//just for warriors.
	avc_update_region(class_get_handle_by_id(AVC_CLASS_ID),
		(void *)gdi_get_screen_handle(FALSE), NULL);
}

 #if ENABLE_TEST_SPEED
 #else
static BOOL menu_open_conflict(control_t *p_curn, u8 new_root)
{
  if (fw_find_root_by_id(new_root) != NULL)  //this menu has been opened
  {
    return FALSE;
  }

  if ((new_root == ROOT_ID_RECALL_LIST )&& ui_get_play_cnxt_count() == 0)
  {
    return FALSE;
  }

  if(ctrl_get_ctrl_id(p_curn) == ROOT_ID_BACKGROUND)
  {
    return TRUE;
  }

  if (ui_is_fullscreen_menu(ctrl_get_ctrl_id(p_curn)))
  {
    if (ctrl_process_msg(p_curn, MSG_EXIT, 0, 0) != SUCCESS)
    {
      fw_destroy_mainwin_by_root(p_curn);
    }
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
#endif

#ifdef SLEEP_TIMER
static RET_CODE on_sleep_tmrout(control_t *p_ctrl, u16 msg,
                                u32 para1, u32 para2)
{
  utc_time_t curn_time, one_minute = {0};
  utc_time_t sleep_time = {0};
  utc_time_t tmp_s, tmp_e;
//  osd_set_t osd_set = {0};

/*  comm_dlg_data_t sleep_notify = //book prog play,popup msg
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    (COMM_DLG_W + 100), COMM_DLG_W,//COMM_DLG_WITHTEXT_H
    IDS_AUTO_SLEEP,//IDS_BOOKPG_END
    10000,
    0,
  };*/

   rect_t sleep_notify_new =
  {
    420, 10, 800, 70,
  };

 // dlg_ret_t dlg_ret = DLG_RET_NULL;

  book_pg_t tmp_book;
  BOOL ret = FALSE;
  u8 i;

  DEBUG(DBG, INFO, "\n");

  //Destory sleep timer.
  ui_sleep_timer_destory();

  //To Check if any booked pg is playing.
  time_get(&curn_time, FALSE);
  one_minute.minute = 1;
  time_add(&curn_time, &one_minute);

  for(i = 0; i < MAX_BOOK_PG; i++)
  {
    sys_status_get_book_node(i, &tmp_book);

    memcpy(&tmp_s, &(tmp_book.start_time), sizeof(utc_time_t));
    memcpy(&tmp_e, &(tmp_book.start_time), sizeof(utc_time_t));
    time_add(&tmp_e, &(tmp_book.drt_time));

    if((time_cmp(&curn_time, &tmp_s, FALSE) >= 0)
      && (time_cmp(&curn_time, &tmp_e, FALSE) < 0)
      && tmp_book.pgid != INVALIDID)
    {
      printf_time(&tmp_s, "tmp start");
      printf_time(&tmp_e, "tmp end");
      OS_PRINTF("pgid %d, i %d\n", tmp_book.pgid, i);
      ret = TRUE;
    }
  }

  OS_PRINTF("ret %d\n", ret);

  //No booked pg is playing.
  if(!ret)
  {
    g_is_one_min_left = TRUE;

   // ui_set_notify_autoclose(FALSE);

    //Create a one minute timer for power off.
  //  g_is_one_min_left = TRUE;

    //Set notify for on minute left.
    ui_set_notify(&sleep_notify_new, NOTIFY_TYPE_STRID, IDS_ONE_MIN_LEFT);

    //Create a one minute timer for power off.
    OS_PRINTF("create one min timer\n");
	sys_status_set_sleep_time(&sleep_time);
    fw_tmr_create(ROOT_ID_BACKGROUND, MSG_POWER_OFF, 1000, FALSE);
  }

  return SUCCESS;
}
#endif

static RET_CODE on_timer_open(control_t *p_ctrl, u16 msg,
                                  u32 para1, u32 para2)
{
  u8 focus_id = fw_get_focus_id();
  if(focus_id == ROOT_ID_PROG_BAR)
  {
    manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
  }
  ui_stop_play(STOP_PLAY_BLACK, TRUE);
  manage_open_menu(ROOT_ID_BOOK_LIST, 0, FROM_TIMER_SETMENU);
  return SUCCESS;
}

#if 0
static RET_CODE on_start_record(control_t *p_ctrl, u16 msg,
								u32 para1, u32 para2)
{
  u8 focus_id = fw_get_focus_id();
#if ENABLE_MUSIC_PICTURE
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();

  if(ui_recorder_isrecording())
  {
    return SUCCESS;
  }

  if(!ui_get_usb_status())
  {
    ui_comm_cfmdlg_open(NULL, IDS_USB_DISCONNECT, NULL, 2000);
    return SUCCESS;
  }

  if(prog_id == INVALIDID)
  {
    return ERR_FAILURE;
  }

	if(ui_is_prog_locked(prog_id))
	{
      ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
      return ERR_FAILURE;
	}

#endif
  if((focus_id != ROOT_ID_BACKGROUND)
     && (focus_id != ROOT_ID_PROG_BAR))
     return SUCCESS;

  if(!ui_signal_is_lock() || !ui_is_playing()
    || ui_is_playpg_scrambled() || (sys_status_get_curn_prog_mode() != CURN_MODE_TV))
  {
    ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
    return SUCCESS;
  }

  {
    partition_t *p_partition = NULL;
    if (file_list_get_partition(&p_partition) > 0)
    {
      // start record ,rec free size should > 100M
      OS_PRINTF("free size=%ld\n",p_partition->free_size);
      if(p_partition->free_size/KBYTES < 100)
      {
        ui_comm_cfmdlg_open(NULL, IDS_NO_STORAGE_FOR_RECORD, NULL, 0);
      }
      else
      {
        manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
        if(ui_is_pause())
          ui_set_pause(FALSE);
        manage_open_menu(ROOT_ID_PVR_REC_BAR, para1, para2);
      }
    }
  }

  return SUCCESS;
}
#endif

#ifdef SUPPORT_GPIO5_OUTOUT
void fp_set_gpio_lock_state(BOOL flag);
#endif
static RET_CODE on_power_off(control_t *p_ctrl, u16 msg,
							 u32 para1, u32 para2)
{
	u8 index = MAX_BOOK_PG;
	u32 tm_out = 0;
	book_pg_t node;
	utc_time_t loc_time = {0};
   u32 auto_sleep = 0;
   	utc_time_t curn_time = {0};

	//  u8 vid;
	//  u16 pos;
	//  dvbs_prog_node_t pg;
	OS_PRINTF("%s : %d \n", __FUNCTION__, __LINE__);
	switch(fw_get_focus_id())
	{
	case ROOT_ID_DO_SEARCH:
		/* ask for enter standby on prog scan */
		if (!do_search_is_finish())
		{
			comm_dlg_data_t dlg_data = {0};
			dlg_ret_t ret;

			// pause at first
			ui_pause_scan();

			// opend dlg
			dlg_data.x = COMM_DLG_X, dlg_data.y = COMM_DLG_Y;
			dlg_data.w = COMM_DLG_W, dlg_data.h = COMM_DLG_H;
			dlg_data.style = DLG_FOR_ASK | DLG_STR_MODE_STATIC;
			dlg_data.content = IDS_MSG_ASK_FOR_STANDBY;

			ret = ui_comm_dlg_open2(&dlg_data);

			if (ret == DLG_RET_NO)
			{
				ui_resume_scan();
				return SUCCESS;
			}
         else
          {
           ui_stop_scan();
          }

			gdi_set_enable(FALSE);
		}
		break;
	case ROOT_ID_UPGRADE_BY_RS232:
		break;
	case ROOT_ID_POPUP:
	case ROOT_ID_POPUP2:
		if(UI_USB_DUMP_IDEL != ui_usb_dumping_sts())
		{
			OS_PRINTF("[USB] ui release dump\r\n");
			ui_release_dump();
		}
    	if (!do_search_is_finish())
		{
		  ui_stop_scan();
		}
		break;
    case ROOT_ID_TS_RECORD:
#if 0		
        if(ui_recorder_isrecording())
        {
          ui_recorder_end();
          ui_recorder_stop();

          MT_ASSERT(ui_recorder_release() == SUCCESS);
        }
#endif		
      break;
    case ROOT_ID_PVR_REC_BAR:
#if 0				
      {
        ui_recorder_stop();
        fw_destroy_all_mainwin(FALSE);
        mtos_task_delay_ms(100);
      }
#endif
      break;
	default:
		ui_stop_play(STOP_PLAY_BLACK, TRUE);
		gdi_set_enable(FALSE);
		if(ui_is_playing())
		{
			ui_stop_play(STOP_PLAY_BLACK, TRUE);
		}
		break;
	}


#ifdef WIN32
	/* pre standby */
	gdi_set_enable(FALSE);

	/* change keymap */
	fw_set_keymap(ui_desktop_keymap_on_standby);

	/* close all menu */
	manage_enable_autoswitch(FALSE);
	fw_destroy_all_mainwin(TRUE);
	manage_enable_autoswitch(TRUE);

#if ENABLE_TTX_SUBTITLE
	/*close ttx*/
	end_ttx();
#endif

	/* kill autosleep tmr */
	//kill_auto_slp();

	/* release apps */
	ui_stop_play(STOP_PLAY_BLACK, TRUE);
	ui_release_signal();

	/* clear history */
	ui_clear_play_history();

	//to get wake up time.
	index = book_get_latest_index();
	if(index != MAX_BOOK_PG)
	{
		book_get_book_node(index, &node);

		time_get(&loc_time, FALSE);
		if(time_cmp(&loc_time, &(node.start_time), FALSE) >= 0)
		{
			book_overdue_node_handler(index);
			index = book_get_latest_index();
			if(index != MAX_BOOK_PG)
			{
				book_get_book_node(index, &node);

				time_get(&loc_time, FALSE);
				loc_time.second = 0;
				OS_PRINTF("index %d\n", index);
				printf_time(&loc_time, "loc time");
				printf_time(&(node.start_time), "start time");
				tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
			}
		}
		else
		{
			time_get(&loc_time, FALSE);
			loc_time.second = 0;
			OS_PRINTF("index %d\n", index);
			printf_time(&loc_time, "loc time");
			printf_time(&(node.start_time), "start time");
			tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
		}
	}
#else

	/* do nothing but send cmd to ap to enter standby */

	//to get wake up time.
	index = book_get_latest_index();
	if(index != MAX_BOOK_PG)
	{
		book_get_book_node(index, &node);

		time_get(&loc_time, FALSE);
		if(time_cmp(&loc_time, &(node.start_time), FALSE) >= 0)
      {
        book_overdue_node_handler(index);
        index = book_get_latest_index();
        if(index != MAX_BOOK_PG)
        {
          book_get_book_node(index, &node);

          time_get(&loc_time, FALSE);
          loc_time.second = 0;
          OS_PRINTF("index %d\n", index);
          printf_time(&loc_time, "loc time");
          printf_time(&(node.start_time), "start time");
          if(node.record_enable)
          {
            tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
            if(tm_out > RECORD_WAKE_UP_TIME)
            {
              tm_out -= RECORD_WAKE_UP_TIME;
            }
            else if(tm_out > RECORD_TIME_OUT_MS)
            {
              tm_out -= RECORD_TIME_OUT_MS;
            }
          }
          else
          {
            tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
          }
          sys_status_set_standby_time_out(tm_out);
        }
      }
    else
    {
      time_get(&loc_time, FALSE);
      loc_time.second = 0;
      OS_PRINTF("index %d\n", index);
      printf_time(&loc_time, "loc time");
      printf_time(&(node.start_time), "start time");
      if(node.record_enable)
      {
        tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
        if(tm_out > RECORD_WAKE_UP_TIME)
        {
          tm_out -= RECORD_WAKE_UP_TIME;
        }
        sys_status_set_standby_time_out(tm_out);
      }
      else
      {
        tm_out = (time_dec(&(node.start_time), &loc_time)) * SECOND;
        sys_status_set_standby_time_out(tm_out);
      }
    }
  }
  else
  {
    sys_status_set_standby_time_out(0);
  }
   auto_sleep = sys_status_get_auto_sleep();

   if(auto_sleep != 0)
      auto_sleep = 0;

   time_get(&curn_time, TRUE);

   sys_status_set_auto_sleep(auto_sleep);
   sys_status_set_utc_time(&curn_time);
   sys_status_save();

	#ifdef SUPPORT_GPIO5_OUTOUT
	fp_set_gpio_lock_state(FALSE);
	#endif
	/* enter standby */
	OS_PRINTF("on power off time out : %d\n", tm_out);
	enter_standby(tm_out);
#endif
	return SUCCESS;
}


static RET_CODE on_power_on(control_t *p_ctrl, u16 msg,
							u32 para1, u32 para2)
{
#if ((!defined WIN32) ) /* jump to start address */
	hal_pm_reset();
#else

	fw_set_keymap(ui_desktop_keymap_on_normal);

	ui_init_signal();

	manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
	if (ui_is_mute())
	{
		ui_set_mute(FALSE);
	}
	if (ui_is_pause())
	{
		ui_set_pause(FALSE);
	}
	ui_play_curn_pg();

	mtos_task_delay_ms(1 * SECOND);
	gdi_set_enable(TRUE);
#endif
	/* set front panel */

	return SUCCESS;
}


static void update_fp_on_scart_switch(BOOL input_is_tv, BOOL is_detected)
{
	if (input_is_tv)
	{
		ui_set_front_panel_by_str(" tu ");
	}
	else
	{
		if (is_detected)
		{
			ui_set_front_panel_by_str("VCR");
		}
		else
		{
			// set frontpanel
			ui_set_frontpanel_by_curn_pg();
		}
	}
}



static RET_CODE on_open_twin_port(control_t *p_ctrl, u16 msg,
								  u32 para1, u32 para2)
{
	if(fw_get_focus_id() ==  ROOT_ID_MAINMENU)
		manage_open_menu(ROOT_ID_TWIN_PORT, 0,0);
	return SUCCESS;
}

static RET_CODE on_scart_switch(control_t *p_ctrl, u16 msg,
								u32 para1, u32 para2)
{
	static control_t *p_orig = NULL;
	static control_t *p_root = NULL;

	BOOL is_detected = avc_detect_scart_vcr_1(class_get_handle_by_id(AVC_CLASS_ID));

	g_vcr_input_is_tv = !g_vcr_input_is_tv;

	if (!is_detected)
	{
		avc_cfg_scart_select_tv_master_1(class_get_handle_by_id(AVC_CLASS_ID),
			g_vcr_input_is_tv ? SCART_TERM_VCR : SCART_TERM_STB);
	}

	avc_cfg_scart_vcr_input_1(class_get_handle_by_id(AVC_CLASS_ID),
		g_vcr_input_is_tv ? SCART_TERM_TV : SCART_TERM_STB);
	update_fp_on_scart_switch(g_vcr_input_is_tv, is_detected);

	p_root = fw_get_root();
	if(g_vcr_input_is_tv)
	{
		OS_PRINTF("vcr keymap mode\n");
		p_orig = fw_get_focus();
		if (p_root != p_orig)
		{
			manage_enable_autoswitch(FALSE);
			fw_set_focus(p_root);
		}
		fw_set_keymap(ui_desktop_keymap_on_vcr);
	}
	else
	{
		OS_PRINTF("normal keymap mode\n");
		if (p_root != p_orig)
		{
			fw_set_focus(p_orig);
			manage_enable_autoswitch(TRUE);
		}
		fw_set_keymap(ui_desktop_keymap_on_normal);
	}

	return SUCCESS;
}


static RET_CODE on_scart_vcr_detected(control_t *p_ctrl, u16 msg,
									  u32 para1, u32 para2)
{
	BOOL is_detected = (BOOL)(para1);
	class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
	/* video adc */
	void *display_dev = NULL;
	display_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);

	disp_cvbs_onoff(display_dev, 1,  !is_detected);
	disp_component_onoff(display_dev, 1, !is_detected);

	if (is_detected)
	{
		avc_cfg_scart_select_tv_master_1(avc_handle, SCART_TERM_VCR);
		avc_cfg_scart_vcr_input_1(avc_handle, SCART_TERM_TV);
		// force to CVBS mode
		avc_cfg_scart_format_1(avc_handle, SCART_VID_CVBS);
		fw_set_keymap(ui_desktop_keymap_on_vcr);
	}
	else
	{
		av_set_t av_set;
		sys_status_get_av_set(&av_set);
		avc_cfg_scart_select_tv_master_1(avc_handle, SCART_TERM_STB);
		avc_cfg_scart_vcr_input_1(avc_handle, SCART_TERM_STB);

		// restore by setting
		avc_cfg_scart_format_1(avc_handle,
			sys_status_get_scart_out_cfg(av_set.video_output));
		fw_set_keymap(ui_desktop_keymap_on_normal);
	}

	update_fp_on_scart_switch(FALSE, is_detected);
	return SUCCESS;
}

#if 0
static RET_CODE on_revise_eid(control_t *p_ctrl, u16 msg,
							  u32 para1, u32 para2)
{
	if (ui_is_playing())
	{
		ui_set_epg_priority(sys_status_get_curn_group_curn_prog_id());
	}
	return SUCCESS;
}
#endif


#ifdef PATCH_ENABLE
static RET_CODE on_edit_ucas(control_t *p_ctrl, u16 msg,
							 u32 para1, u32 para2)
{
	if(fw_get_focus_id() == ROOT_ID_MAINMENU)
	{
		manage_open_menu(ROOT_ID_EDIT_UCAS, 0, 0);
	}

	return SUCCESS;
}
#endif


static RET_CODE on_switch_pn(control_t *p_ctrl, u16 msg,
							 u32 para1, u32 para2)
{
	class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
	av_set_t av_set = {0};
	u8 org_mode;
	u8 mode_str[5][8] = {"PAL", "NTSC", "Auto"};

	rect_t pn_notify =
	{
		NOTIFY_CONT_X, NOTIFY_CONT_Y,
		NOTIFY_CONT_X + NOTIFY_CONT_W,
		NOTIFY_CONT_Y + NOTIFY_CONT_H,
	};

	if(fw_get_focus_id() == ROOT_ID_BACKGROUND)
	{
		sys_status_get_av_set(&av_set);
		org_mode = av_set.tv_mode;

		av_set.tv_mode++;

		if(av_set.tv_mode > AVC_VIDEO_MODE_PAL_1)
		{
			av_set.tv_mode = AVC_VIDEO_MODE_AUTO_1;
		}

		if(av_set.tv_mode != org_mode)
		{
			ui_reset_tvmode(av_set.tv_mode);
		}

		avc_switch_video_mode_1(avc_handle, sys_status_get_hd_mode(av_set.tv_resolution));

		ui_set_notify(&pn_notify, NOTIFY_TYPE_ASC, (u32)mode_str[av_set.tv_mode]);
		sys_status_set_av_set(&av_set);
		sys_status_save();
	}
	return SUCCESS;
}

static RET_CODE on_sleep(control_t *p_ctrl, u16 msg,
						 u32 para1, u32 para2)
{
  u32 auto_sleep = 0;
  manage_close_menu(ROOT_ID_PROG_DETAIL, 0, 0);
  manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
  manage_close_menu(ROOT_ID_VOLUME, 0, 0);
  DEBUG(DBG, INFO, "\n");
  if(fw_get_focus_id() == ROOT_ID_BACKGROUND)
  {
    auto_sleep = sys_status_get_auto_sleep();

		MT_ASSERT(auto_sleep < SLEEP_CNT);

		if(fw_find_root_by_id(ROOT_ID_SLEEP_HOTKEY) != NULL)
		{
			auto_sleep++;

			if(auto_sleep >= SLEEP_CNT)
			{
				auto_sleep = 0;
			}

			sys_status_set_auto_sleep(auto_sleep);
			sys_status_save();
		}

    sleep_hotkey_set_content((u8)auto_sleep);

		ui_sleep_timer_create();

	}
	return SUCCESS;
}

static RET_CODE on_switch_video_mode(control_t *p_ctrl, u16 msg,
									 u32 para1, u32 para2)
{
	class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
	disp_sys_t video_std = VID_SYS_AUTO;
	av_set_t av_set;
	static u8 *video_resolution_hd_str_50hz[] = {"576i", "576p", "720p", "1080i", "1080p"};
	static u8 *video_resolution_hd_str_60hz[] = {"480i", "480p", "720p", "1080i", "1080p"};
	u32 content = 0;
	rect_t pn_notify =
	{
		NOTIFY_CONT_X, NOTIFY_CONT_Y,
		NOTIFY_CONT_X + NOTIFY_CONT_W,
		NOTIFY_CONT_Y + NOTIFY_CONT_H,
	};

	if(fw_get_focus_id() == ROOT_ID_BACKGROUND)
	{
		sys_status_get_av_set(&av_set);

		if (fw_find_root_by_id(ROOT_ID_NOTIFY) != NULL)
		{
			av_set.tv_resolution++;
		}

		avc_video_switch_chann(avc_handle, DISP_CHANNEL_SD);

		video_std = avc_get_video_mode_1(avc_handle);

		switch (video_std)
		{
		case VID_SYS_NTSC_J:
		case VID_SYS_NTSC_M:
		case VID_SYS_NTSC_443:
		case VID_SYS_PAL_M:
			av_set.tv_resolution %= (sizeof(video_resolution_hd_str_60hz)/sizeof(u8*));

			content = (u32)video_resolution_hd_str_60hz[av_set.tv_resolution];
			break;

		case VID_SYS_PAL:
		case VID_SYS_PAL_N:
		case VID_SYS_PAL_NC:
			av_set.tv_resolution %= (sizeof(video_resolution_hd_str_50hz)/sizeof(u8*));

			content = (u32)video_resolution_hd_str_50hz[av_set.tv_resolution];
			break;

		default:
			MT_ASSERT(0);
			break;
		}

		avc_switch_video_mode_1(avc_handle, sys_status_get_hd_mode(av_set.tv_resolution));
		ui_set_notify(&pn_notify, NOTIFY_TYPE_ASC, content);
		sys_status_set_av_set(&av_set);
		sys_status_save();
	}
	return SUCCESS;
}

#if ENABLE_TTX_SUBTITLE
static RET_CODE on_switch_subt(control_t *p_ctrl, u16 msg,
							   u32 para1, u32 para2)
{
  if((ui_get_play_prg_type() == NET_PRG_TYPE)
    ||(ui_get_play_prg_type() == CAMERA_PRG_TYPE))
  {
    return SUCCESS;
  }


    manage_close_menu(ROOT_ID_PROG_DETAIL, 0, 0);
    manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
    //manage_close_menu(ROOT_ID_VOLUME, 0, 0);
	if(fw_get_focus_id() != ROOT_ID_BACKGROUND)
	{
		return ERR_FAILURE;
	}
	manage_open_menu(ROOT_ID_SUBT_LANGUAGE,
		0, 0);

	return SUCCESS;
}
#endif

static RET_CODE on_update_descramble(control_t *p_ctrl, u16 msg,
									 u32 para1, u32 para2)
{
	ui_set_playpg_scrambled(msg == MSG_DESCRAMBLE_FAILED);
//	update_encrypt_message();
	return SUCCESS;
}

static RET_CODE on_mute(control_t *p_ctrl, u16 msg,
						u32 para1, u32 para2)
{
  u8 index;
    //ui_release_ca();
    index = fw_get_focus_id();
    if(index != ROOT_ID_DO_SEARCH
      && index != ROOT_ID_MAINMENU
      && do_search_is_finish())
    {
      ui_set_mute(!ui_is_mute());
    }
  return SUCCESS;
}


static RET_CODE on_pause(control_t *p_ctrl, u16 msg,
						 u32 para1, u32 para2)
{
  u8 index;
  control_t *p_root = NULL;
  BOOL time_shift = FALSE;
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();

#if 0
{
  dmx_device_t *p_dmx_dev = NULL;
  dmx_chanid_t p_channel_a = 0,p_channel_v = 0;
    dvbs_prog_node_t pg_node = {0};
  u8 audio_key_odd[8] = {0x6e, 0x29, 0x05, 0x9c, 0xb2, 0x22, 0xf2, 0xc6};
  u8 audio_key_even[8]= {0xd3, 0x21, 0xbc, 0xb0, 0x38, 0x91, 0x8f, 0x58};
  p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  MT_ASSERT(p_dmx_dev != NULL);

  db_dvbs_get_pg_by_id(prog_id, &pg_node);

  dmx_get_chanid_bypid(p_dmx_dev, pg_node.audio[pg_node.audio_channel].p_id,&p_channel_a);
  dmx_descrambler_onoff(p_dmx_dev, p_channel_a, FALSE);
  dmx_descrambler_onoff(p_dmx_dev, p_channel_a, TRUE);
  dmx_descrambler_set_odd_keys(p_dmx_dev,p_channel_a, audio_key_odd, 8);
  dmx_descrambler_set_even_keys(p_dmx_dev,p_channel_a, audio_key_even, 8);

  dmx_get_chanid_bypid(p_dmx_dev, pg_node.video_pid, &p_channel_v);
  dmx_descrambler_onoff(p_dmx_dev, p_channel_v, FALSE);
  dmx_descrambler_onoff(p_dmx_dev, p_channel_v, TRUE);
  dmx_descrambler_set_even_keys(p_dmx_dev,p_channel_v, audio_key_even, 8);
  dmx_descrambler_set_odd_keys(p_dmx_dev,p_channel_v, audio_key_odd, 8);
}
#endif

 if((ui_get_play_prg_type() == NET_PRG_TYPE))
 {
    OS_PRINTF(" it is net play,so it is can not support pause\n");
    return SUCCESS;
 }

  if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
  {
    if((fw_get_focus_id() == ROOT_ID_BACKGROUND) ||
      (fw_get_focus_id() == ROOT_ID_PROG_BAR))
    {
        ui_video_c_pause_resume();
        ui_set_pause(!ui_is_pause());
    }
    return SUCCESS;
  }

  p_root = fw_find_root_by_id(ROOT_ID_PVR_REC_BAR);
  if(NULL != p_root)
  {
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_RECORD_PLAY_PAUSE, 0, 0);
    return SUCCESS;
  }
  {
    index = fw_get_focus_id();
    time_shift = sys_status_get_timeshift_switch();
    if((index != ROOT_ID_NUM_PLAY
       && ui_is_fullscreen_menu(index))
      || (index == ROOT_ID_PROG_LIST)
      || (index == ROOT_ID_ZOOM)
      || (index == ROOT_ID_EPG)
      || (index == ROOT_ID_VEPG)
      ||(index == ROOT_ID_XEXTEND))
    {
      // only respond the key on tv mode
      if(sys_status_get_curn_prog_mode() == CURN_MODE_TV)
      {
          //sys_status_get_status(BS_DVR_ON, &time_shift);
        if((time_shift == TRUE) && ui_is_fullscreen_menu(index) && ui_get_usb_status())
        {
        	if(ui_is_prog_locked(prog_id))
        	{
	            ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
	            return ERR_FAILURE;
        	}

          manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
          manage_open_menu(ROOT_ID_TIMESHIFT_BAR, 0, 0);
        }
        else if ((!ui_is_playpg_scrambled()) && ui_signal_is_lock() && ui_is_playing())
        {
          ui_set_pause(!ui_is_pause());
        }
      }
      else if(sys_status_get_curn_prog_mode() == CURN_MODE_RADIO)
      {
        if(time_shift == TRUE)
        {
          ui_comm_cfmdlg_open(NULL, IDS_TIMESHIFT_RADIO_PRO, NULL, 2000);
          return ERR_NOFEATURE;
        }
      }
      else
      {
        return ERR_NOFEATURE;
      }
    }
  }
  return SUCCESS;
}

static RET_CODE on_play(control_t *p_ctrl, u16 msg,
                         u32 para1, u32 para2)
{
  u8 index;
  BOOL time_shift = FALSE;

  if((ui_get_play_prg_type() == NET_PRG_TYPE)
    ||(ui_get_play_prg_type() == CAMERA_PRG_TYPE))
  {
    return SUCCESS;
  }

  {
    index = fw_get_focus_id();
    time_shift = sys_status_get_timeshift_switch();
    if((index != ROOT_ID_NUM_PLAY
       && ui_is_fullscreen_menu(index))
      || (index == ROOT_ID_PROG_LIST)
      || (index == ROOT_ID_ZOOM)
      || (index == ROOT_ID_EPG)
      || (index == ROOT_ID_VEPG))
    {
      // only respond the key on tv mode
      if(sys_status_get_curn_prog_mode() == CURN_MODE_TV)
      {
        if((time_shift == TRUE) && ui_is_fullscreen_menu(index))
        {
          return ERR_FAILURE;
        }
        else
        {
          ui_set_pause(!ui_is_pause());
        }
      }
      else if(sys_status_get_curn_prog_mode() == CURN_MODE_RADIO)
      {
        return ERR_NOFEATURE;
      }
    }
  }
  return SUCCESS;
}
#if ENABLE_TTX_SUBTITLE
static RET_CODE on_start_ttx(control_t *p_ctrl, u16 msg,
							 u32 para1, u32 para2)
{
	u8 index;
  if((ui_get_play_prg_type() == NET_PRG_TYPE)
    ||(ui_get_play_prg_type() == CAMERA_PRG_TYPE))
  {
    return SUCCESS;
  }


  index = fw_get_focus_id();
  if(ui_is_fullscreen_menu(index))
  {
    if(ui_is_ttx_data_ready(sys_status_get_curn_group_curn_prog_id()))
    {
			start_ttx();

			fw_set_keymap(ui_desktop_keymap_on_ttx);
			fw_set_proc(ui_desktop_proc_on_ttx);
		}
		else
		{
			ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_TELETEXT, NULL, 2000);
		}
	}

	return SUCCESS;
}


static RET_CODE on_stop_ttx(control_t *p_ctrl, u16 msg,
							u32 para1, u32 para2)
{
	end_ttx();

	// restore keymap
	fw_set_keymap(ui_desktop_keymap_on_normal);
	fw_set_proc(ui_desktop_proc_on_normal);

	// open prog bar
	if(ui_is_mute())
	{
		open_mute(0, 0);
	}

	if(ui_is_pause())
	{
		open_pause(0, 0);
	}

	if(ui_is_notify())
	{
		open_notify(0, 0);
	}

	update_signal();
	manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);

	return SUCCESS;
}


static RET_CODE on_ttx_key(control_t *p_ctrl, u16 msg,
						   u32 para1, u32 para2)
{
	ui_post_ttx_key(msg & MSG_DATA_MASK);
	return SUCCESS;
}
#endif

static RET_CODE on_open_in_tab(control_t *p_ctrl, u16 msg,
							   u32 para1, u32 para2)
{
  #if ENABLE_TEST_SPEED
  	manage_open_menu(ROOT_ID_UPGRADE_BY_USB, 0, 0);
  #else
  u32 vkey = para1;
  u8 new_root = (u8)(msg & MSG_DATA_MASK);

  if(menu_open_conflict(fw_get_focus(), new_root))
  {
  #if ENABLE_MUSIC_PICTURE
      return manage_open_menu(new_root, vkey, 0);
  #else
     return manage_open_menu(new_root, vkey, 0);
  #endif
  }
  #endif

	return ERR_NOFEATURE;
}


static RET_CODE on_close_menu(control_t *p_ctrl, u16 msg,
							  u32 para1, u32 para2)
{
	//OS_PRINTF("------------------------------------ on close menu\n");
	manage_autoclose();
	return SUCCESS;
}

static void ui_desktop_close_special_menus_before_play_satip_prog()
{
    control_t *p_root = NULL;

    p_root = ui_comm_root_get_root(ROOT_ID_VIDEO_PLAYER);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to netplaybar##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_REDTUBE);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to redtube##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_USB_MUSIC);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to music##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_USB_MUSIC_FULLSCREEN);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to music fullscreen##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_USB_PICTURE);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to picture##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_NETWORK_MUSIC);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to net music##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_VEPG);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  epg##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_TETRIS);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  game tetris##\n", __FUNCTION__, __LINE__);
        manage_close_menu(ROOT_ID_TETRIS, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_YOUPORN);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  youporn##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_UPGRADE_BY_USB);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to USB upgrade ##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_UPGRADE_BY_RS232);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to rs232 upgrade ##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_DUMP_BY_USB);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  USB dump##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_USB_MUSIC_FULLSCREEN);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_USB_MUSIC_FULLSCREEN##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_USB_MUSIC);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_USB_MUSIC##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_ONMOV_DESCRIPTION);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_ONMOV_DESCRIPTION##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_NETWORK_PLAY);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_NETWORK_PLAY##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_ONMOV_WEBSITES);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_ONMOV_WEBSITES##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_OTA);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_OTA##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_LIVE_TV);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_LIVE_TV##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    p_root = ui_comm_root_get_root(ROOT_ID_IPTV_PLAYER);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to  ROOT_ID_LIVE_TV##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT_ALL, 0, 0);
    }

    OS_PRINTF("#######%s, line[%d]######\n", __FUNCTION__, __LINE__);
}

static RET_CODE timer_end_to_play(void)
{
  u16 pg_id;
  dlg_ret_t dlg_ret = DLG_RET_NULL;
  comm_dlg_data_t *popup_msg = NULL;
  BOOL p_time_out = FALSE;

  comm_dlg_data_t dlg_data_end = //book prog play,popup msg
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    (COMM_DLG_W + 100), COMM_DLG_H,
    IDS_BOOKPG_END,
    15000,
    0,
  };

  comm_dlg_data_t dlg_data_jump_pg = //book prog play,popup msg
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    (COMM_DLG_W + 100), COMM_DLG_H,
    IDS_RECALL_PG,
    15000,
    0,
  };

  sys_status_get_status(BS_IS_TIME_OUT_WAKE_UP, &p_time_out);
//  p_time_out = FALSE;

  if(g_is_book_node_dlg_opened)
  {
    return SUCCESS;
  }

  if(p_time_out)//dlg_data_jump_pg
  {
    //means we need to standby.
    popup_msg = &dlg_data_end;
  }
  else
  {
    //means whether to play curn pg.
    popup_msg = &dlg_data_jump_pg;
  }
  dlg_ret = ui_comm_dlg_open2(popup_msg);

  g_is_book_node_dlg_opened = FALSE;
  switch(dlg_ret)
  {
    case DLG_RET_YES:
  	  if(p_time_out)//standby.
  	  {
  	    fw_tmr_create(ROOT_ID_BACKGROUND, MSG_POWER_OFF, 1000, FALSE);
  	  }
  	  else
  	  {
  	    ui_desktop_close_special_menus_before_play_satip_prog();
  	    ui_close_all_mennus();
  	    ui_recall(TRUE,&pg_id);
  	  }
      break;

    case DLG_RET_NO:
      break;

    default:
      break;
  }

  return SUCCESS;
}

static RET_CODE on_book_prog_check(void)
{
  book_pg_t node;
  //dvbs_prog_node_t prog = NULL;
  u32 tm_out;
  u8 node_state = LATEST_NODE_WAIT;
  u8 latest_index = MAX_BOOK_PG;
  control_t *p_root = NULL;
  //BOOL p_time_out = FALSE;
  //u16 pg_id = 0;
  //u8 ucSkip = 0;
  comm_dlg_data_t dlg_data = //book prog play,popup msg
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    (COMM_DLG_W + 100), COMM_DLG_H,
    IDS_BOOKPG_START,
    15000,
    0,
  };


  dlg_ret_t dlg_ret = DLG_RET_NULL;

  book_pg_t book_node;

  book_delete_overdue_node();

  latest_index = book_get_latest_index();
  book_get_book_node(latest_index, &node);
  node_state = book_check_latest_node(latest_index, TRUE);

  if(ui_get_book_flag())
  {
    if(node_state == LATEST_NODE_START)
    {

      if(g_is_book_node_dlg_opened)
      {
        return SUCCESS;
      }
      //ui_close_all_mennus();
      //ui_play_curn_pg();
      dlg_ret = ui_comm_dlg_open2(&dlg_data);
	  g_is_book_node_dlg_opened = TRUE;
      if(dlg_ret == DLG_RET_YES)
      {
             if(ui_get_play_prg_type() != SAT_PRG_TYPE)
             {
               ui_video_c_stop();
               ui_video_c_destroy();
               ui_comm_dlg_close();
               ui_set_play_prg_type(SAT_PRG_TYPE);
             }

             ui_desktop_close_special_menus_before_play_satip_prog();

             ui_close_all_mennus();

             g_is_book_node_dlg_opened = FALSE;
             if(!ui_get_book_flag())
            {
              return SUCCESS;
            }

            tm_out = (node.drt_time.hour * 60 + node.drt_time.minute) * 60 * SECOND;

            if(tm_out == 0)
            {
              book_overdue_node_handler(latest_index);
            }

            if(node.record_enable)
            {
                ui_book_play(node.pgid);
                memcpy(&book_node,&node,sizeof(book_pg_t));

                if(tm_out != 0)
                {
                 // on_start_record(NULL,0,(u32)&(book_node.drt_time),(u32)node.record_enable);
                }
            }
            else
            {
                p_root = fw_find_root_by_id(ROOT_ID_PROG_BAR);
                if(p_root == NULL)
                {
                   manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
                }

                ui_book_play(node.pgid);
                fill_nprog_info(fw_find_root_by_id(ROOT_ID_PROG_BAR), node.pgid);

                if(ui_is_mute()&& (fw_find_root_by_id(ROOT_ID_MUTE) == NULL))
                {
                  open_mute(0, 0);
                }
            }
        }
        else if(dlg_ret == DLG_RET_NO)
        {
           g_is_book_node_dlg_opened = FALSE;
          book_overdue_node_handler(latest_index);
          return SUCCESS;
        }
    }
    else if(node_state == LATEST_NODE_END)
    {
      if(g_is_book_node_dlg_opened)
      {
        return SUCCESS;
      }

      book_overdue_node_handler(latest_index);
      latest_index = book_get_latest_index();
      node_state = book_check_latest_node(latest_index, TRUE);
      if(node_state == LATEST_NODE_START)
      {

        dlg_ret = ui_comm_dlg_open2(&dlg_data);
        g_is_book_node_dlg_opened = TRUE;
        if(dlg_ret == DLG_RET_YES)
        {
          if(ui_get_play_prg_type() != SAT_PRG_TYPE)
          {
            ui_video_c_stop();
            ui_video_c_destroy();
            ui_comm_dlg_close();
            ui_set_play_prg_type(SAT_PRG_TYPE);
          }
          g_is_book_node_dlg_opened = FALSE;
          book_get_book_node(latest_index, &node);
          ui_desktop_close_special_menus_before_play_satip_prog();
          ui_close_all_mennus();
          tm_out = (node.drt_time.hour * 60 + node.drt_time.minute) * 60 * SECOND;
          if(tm_out == 0)
          {
            book_overdue_node_handler(latest_index);
          }
          if(node.record_enable)
          {
            ui_book_play(node.pgid);
            memcpy(&book_node,&node,sizeof(book_pg_t));
            if(tm_out != 0)
            {
            //on_start_record(NULL,0,(u32)&(book_node.drt_time),(u32)node.record_enable);
            }
          }
          else
          {
              p_root = fw_find_root_by_id(ROOT_ID_PROG_BAR);

              if(p_root == NULL)
              {
                manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
              }

              fill_nprog_info(fw_find_root_by_id(ROOT_ID_PROG_BAR), node.pgid);

              ui_book_play(node.pgid);
              if(ui_is_mute() && (fw_find_root_by_id(ROOT_ID_MUTE) == NULL))
              {
                open_mute(0, 0);
              }
    	    }

        }
        else if(dlg_ret == DLG_RET_NO)
        {
          g_is_book_node_dlg_opened = FALSE;
          book_delete_node(latest_index);
          return SUCCESS;
        }
      }
      else
      {
            timer_end_to_play();

      }
    }
  }
  return SUCCESS;
}

#ifdef IMPL_NEW_EPG
static u16 time_update_desktop_times = 0;
static void on_time_update_epg_info(void)
{
    dvbs_prog_node_t pg = {0};
    epg_prog_info_t prog_info = {0};
    utc_time_t g_time_st = {0};
    utc_time_t g_time_end = {0};
    db_dvbs_ret_t ret = DB_DVBS_OK;
    u16 pg_id = 0;

   if((fw_get_focus_id() == ROOT_ID_BACKGROUND) && ui_epg_is_initialized())
  {
     time_update_desktop_times ++;
     if(time_update_desktop_times >10)
      {
        time_update_desktop_times = 0;
        time_get(&g_time_st, FALSE);
        memcpy(&g_time_end, &g_time_st, sizeof(utc_time_t));
        time_up(&g_time_end, 24 * 60 * 60);
        pg_id = sys_status_get_curn_group_curn_prog_id();
        if(INVALIDID == pg_id)
        {
          return;
        }
        ret = db_dvbs_get_pg_by_id(pg_id, &pg);
        MT_ASSERT(DB_DVBS_OK == ret);
        prog_info.network_id = (u16)pg.orig_net_id;
        prog_info.ts_id      = (u16)pg.ts_id;
        prog_info.svc_id     = (u16)pg.s_id;
        memcpy(&prog_info.start_time, &g_time_st, sizeof(utc_time_t));
        memcpy(&prog_info.end_time, &g_time_end, sizeof(utc_time_t));
        mul_epg_set_db_info(&prog_info);

      }
  }
  else
  {
    time_update_desktop_times = 0;
  }
  return;
}
#endif

static RET_CODE on_time_update(control_t *p_ctrl, u16 msg,
							   u32 para1, u32 para2)
{
  utc_time_t curn_time = {0};
  //utc_time_t ss_time = {0};
  time_set_t t_set;

  time_get(&curn_time, TRUE);
  {
    sys_status_get_time(&t_set);
    memcpy(&(t_set.sys_time), &curn_time, sizeof(utc_time_t));
    sys_status_set_time(&t_set);
  }

#ifdef IMPL_NEW_EPG
  on_time_update_epg_info();
#endif

  (void)on_book_prog_check();

  return SUCCESS;
}


static RET_CODE on_check_signal(control_t *p_ctrl, u16 msg,
								u32 para1, u32 para2)
{
	ui_signal_check(para1, para2);
	return SUCCESS;
}

void ui_set_usb_status(BOOL b)
{
	g_usb_status = b;
}

BOOL ui_get_usb_status(void)
{
	return g_usb_status;
}

void ui_set_net_connect_status( net_conn_stats_t net_conn_stats)
{
  g_net_connt_stats = net_conn_stats;
}

net_conn_stats_t ui_get_net_connect_status(void)
{
  return g_net_connt_stats;
}

#if 0
static RET_CODE on_epg(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

	if(fw_find_root_by_id(ROOT_ID_VEPG) == NULL)
	{
		manage_open_menu(ui_comm_get_epg_root(), 0, 0);
	}
	return SUCCESS;
}
#endif
static RET_CODE ui_exit_usb_upg(u8 root_id)
{
	control_t *p_cont = NULL;
	u32 tmp_status = 0;

	switch(root_id)
	{
	case ROOT_ID_XUPGRADE_BY_USB:
		tmp_status = ui_xusb_upgade_sts();
		if((UI_XUSB_UPG_IDEL != tmp_status)
			&&(UI_XUSB_UPG_LOAD_END != tmp_status))
		{
			return ERR_FAILURE;
		}
		break;
	case ROOT_ID_DUMP_BY_USB:
		tmp_status = ui_usb_dumping_sts();
		if((UI_USB_DUMP_IDEL != tmp_status)
			&&(UI_USB_DUMP_PRE_READ != tmp_status))
		{
			return ERR_FAILURE;
		}
		break;
	default:
		return ERR_FAILURE;
	}

	p_cont = ui_comm_root_get_root(root_id);
	if(NULL != p_cont)
	{
		ctrl_process_msg(p_cont, MSG_UPG_QUIT, 0, 0);
	}
	return SUCCESS;
}

static void ui_jump_to_usbupg_menu(void)
{
	//para2 ==1 for open_upgrade_by_usb highlight on Start menu
	manage_open_menu(ROOT_ID_UPGRADE_BY_USB, 0, 1);
}

static RET_CODE on_usb_dev_plug_in(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  u32 usb_dev_type = para2;
  u32 bus_speed = (u16)para1;
  u16 stringid = 0;
  net_config_t g_net_config = {0,};
  pnp_svc_t * p_this = NULL;
  control_t *root = NULL;

  if(bus_speed == HP_BUS_FULL_SPEED)
  {
    ui_comm_cfmdlg_open(NULL, IDS_FULL_SPEED_USB_DEVICE_CONNECT, NULL, 2000);
    //update_signal();
    //return SUCCESS;
  }
  if(bus_speed == HP_BUS_LOW_SPEED)
  {
    ui_comm_cfmdlg_open(NULL, IDS_LOW_SPEED_USB_DEVICE_CONNECT, NULL, 2000);
    update_signal();
    return SUCCESS;
  }

  if(usb_dev_type == HP_STORAGE)
  {
    return SUCCESS;
  }
  else if(usb_dev_type == HP_HUB)
  {
    stringid = IDS_USB_HUB_CONNECT;
  }
  else if(usb_dev_type == HP_WIFI)
  {
  //  is_usb_wifi = TRUE;
    p_this = (pnp_svc_t *)class_get_handle_by_id(PNP_SVC_CLASS_ID);
    #ifndef WIN32
    p_this->p_wifi_dev = get_wifi_handle();
    #endif
    stringid = IDS_WIFI_PLUG_IN;
    g_net_connt_stats.is_wifi_insert = TRUE;
    sys_status_get_net_config_info(&g_net_config);
    if(g_net_config.link_type == LINK_TYPE_WIFI)
    {
      auto_connect_wifi();
    }

    root = fw_find_root_by_id(ROOT_ID_SUBNETWORK_CONFIG);
    if(root)
    {
        fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_UI_REFRESH, 0, 0);
    }
  }
  else if(usb_dev_type == HP_MODEM)
  {
    stringid = IDS_3G_PLUG_IN;
    if(g_net_connt_stats.is_3g_insert == TRUE)
    {
      return ERR_FAILURE;
    }
    g_net_connt_stats.is_3g_insert = TRUE;
    sys_status_get_net_config_info(&g_net_config);
    if(g_net_config.link_type == LINK_TYPE_3G)
    {
     fw_tmr_create(ROOT_ID_BACKGROUND, MSG_G3_READY, 1000, TRUE);
    }

    root = fw_find_root_by_id(ROOT_ID_SUBNETWORK_CONFIG);
    if(root)
    {
        fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_UI_REFRESH, 0, 0);
    }
  }
  else if(usb_dev_type == HP_ETHERNET)
  {
    stringid = IDS_USB_ETH_DEV_CONNECT;
  }
  else
  {
    update_signal();
    return SUCCESS;
  }

  ui_comm_cfmdlg_open(NULL, stringid, NULL, 2000);
  update_signal();
  return SUCCESS;
}


BOOL ui_find_usb_onlive_file(void)
{
    u16 g_ffilter_all[32] = {0};
    u32 g_partition_cnt = 0;
    file_list_t g_list = {0};
    flist_dir_t g_flist_dir = NULL;
    partition_t *p_partition = NULL;
    u32 count = 0;
    u16 file_name[MAX_FILE_PATH] = {0};
    str_asc2uni("|txt|TXT", g_ffilter_all);
    g_partition_cnt = 0;
    p_partition = NULL;
    g_partition_cnt = file_list_get_partition(&p_partition);
    if(g_partition_cnt > 0)
    {
        g_flist_dir = file_list_enter_dir(g_ffilter_all, MAX_FILE_COUNT, p_partition[sys_status_get_usb_work_partition()].letter);
        if (g_flist_dir != NULL)
        {
            file_list_get(g_flist_dir, FLIST_UNIT_FIRST, &g_list);
            count = (u16)g_list.file_count;
            file_name[0] = p_partition[sys_status_get_usb_work_partition()].letter[0];
            str_asc2uni(":\\custom_url.txt", file_name + 1);
            file_live = vfs_open( file_name, VFS_READ);
            if (file_live == NULL)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}
BOOL check_custom_def_in_usb(hfile_t* pp_file_hdl)
{
    u16 g_ffilter_all[32] = {0};
    u32 g_partition_cnt = 0;
    file_list_t g_list = {0};
    flist_dir_t g_flist_dir = NULL;
    partition_t *p_partition = NULL;
    u32 count = 0;
    u16 file_name[MAX_FILE_PATH] = {0};
    str_asc2uni("|bin|", g_ffilter_all);
    g_partition_cnt = 0;
    p_partition = NULL;
    g_partition_cnt = file_list_get_partition(&p_partition);
    if(g_partition_cnt > 0)
    {
        g_flist_dir = file_list_enter_dir(g_ffilter_all, MAX_FILE_COUNT, p_partition[0].letter);
        if (g_flist_dir != NULL)
        {
            file_list_get(g_flist_dir, FLIST_UNIT_FIRST, &g_list);
            count = (u16)g_list.file_count;
            file_name[0] = p_partition[0].letter[0];
            str_asc2uni(":\\custom_def.bin", file_name + 1);
            *pp_file_hdl = vfs_open( file_name, VFS_READ);
            if (*pp_file_hdl  == NULL)
            {
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}
static void start_load_customer_prog()
{
  u8 *p_buffer = NULL;
  u32 file_size = 0;
  if(file_live == NULL)
  {
    return ;
  }

    if(ui_get_usb_status() == FALSE)
    {
       if(file_live)
      {
        vfs_close(file_live);
        file_live = NULL;
      }
       return;
    }

    ui_evt_disable_ir();
    OS_PRINTF("#########%s, line[%d]########\n", __FUNCTION__, __LINE__);
    vfs_get_file_info(file_live, &file_info);
    p_buffer = mtos_malloc(file_info.file_size + 2* KBYTES);
    MT_ASSERT(NULL != p_buffer);
    file_size = vfs_read(p_buffer, file_info.file_size, 1, file_live);
    MT_ASSERT(file_size == file_info.file_size);
    save_custom_url(p_buffer,file_info.file_size);
    ui_evt_enable_ir();
    mtos_free(p_buffer);
    vfs_close(file_live);
    file_live = NULL;
}
static void cancel_load_customer_prog()
{
  if(file_live)
  {
    vfs_close(file_live);
    file_live = NULL;
  }
}
static RET_CODE on_plug_in(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  BOOL is_find = FALSE;
  rect_t upgrade_dlg_rc =
  {
    ((SCREEN_WIDTH - 400) / 2),
    ((SCREEN_HEIGHT - 200) / 2),
    ((SCREEN_WIDTH - 400) / 2) + 400,
    ((SCREEN_HEIGHT - 200) / 2) + 200,
  };
  //u32 usb_dev_type = para2;
  //u32 bus_speed = (u16)para1;
  u16 stringid = 0;

  //OS_PRINTF("on_plug_in, dev_type[%d]\n", usb_dev_type);
  ui_set_usb_status(TRUE);

  ui_usb_up_partition_sev();


  //if (fw_get_focus_id() != ROOT_ID_DO_SEARCH)
  //{
    //is_find = ui_find_usb_upg_file();
  //}
  if(is_find)
  {
    ui_comm_ask_for_dodlg_open(&upgrade_dlg_rc, IDS_USB_UPG_FILE_EXIST,
                               ui_jump_to_usbupg_menu,
                               NULL, 0);
    update_signal();
    return SUCCESS;
  }
#if ENABLE_CUSTOM_URL
  if(fw_get_focus_id() != ROOT_ID_LIVE_TV)
  {
     is_find = ui_find_usb_onlive_file();
  }
#endif
  if(is_find)
  {
    ui_comm_ask_for_dodlg_open(NULL, IDS_IMPORT_CUSTOM_PGS_OR_NOT,
                                                      start_load_customer_prog, cancel_load_customer_prog ,0);
  }
  else
  {
    stringid = IDS_USB_STORAGE_CONNECT;
    if(NULL == fw_find_root_by_id(ROOT_ID_PASSWORD) )
    {
      ui_comm_cfmdlg_open(NULL, stringid, NULL, 2000);
    }
  }

  update_signal();
  return SUCCESS;
}

static RET_CODE on_plug_out(control_t *p_ctrl, u16 msg,
							u32 para1, u32 para2)
{
    u32 usb_dev_type = para2;
    u8 root_id = fw_get_focus_id();
    net_conn_stats_t eth_conn_sts = ui_get_net_connect_status();
    net_config_t net_config = {0,};
    control_t *root = NULL;

    sys_status_get_net_config_info(&net_config);

    DEBUG(MAIN, INFO, "#########usb type is == %d####\n",usb_dev_type);
    if(usb_dev_type == HP_WIFI)
    {
#ifndef WIN32
      root = fw_find_root_by_id(ROOT_ID_KEYBOARD_V2);
      if(root != NULL)
      {
        OS_PRINTF("###wifi device plug out, will close ROOT_ID_KEYBOARD_V2 menu###\n");
        manage_close_menu(ROOT_ID_KEYBOARD_V2, 0, 0);
      }
	   root = fw_find_root_by_id(ROOT_ID_WIFI_LINK_INFO);
      ui_comm_cfmdlg_open(NULL, IDS_WIFI_PLUG_OUT, NULL, 2000);

      if(root != NULL)
      {
        OS_PRINTF("###wifi device plug out, will close wifi_link_info menu###\n");
        //manage_close_menu(ROOT_ID_WIFI_LINK_INFO, 0, 0);
        fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
      }

      root = fw_find_root_by_id(ROOT_ID_WIFI);
      if(root != NULL)
      {
        OS_PRINTF("###wifi device plug out, will close wifi_link_info menu###\n");
       // manage_close_menu(ROOT_ID_WIFI, 0, 0);
        fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
      }


    //  OS_PRINTF("#########is_usb_wifi is == %d####\n",usb_dev_type);
      g_net_connt_stats.is_wifi_insert = FALSE;
      g_net_connt_stats.is_wifi_conn = FALSE;
  //    is_usb_wifi = FALSE;

     g_net_conn_info.wifi_conn_info = WIFI_DIS_CONNECT;


      root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_WIFI);
      if(root)
      {
        OS_PRINTF("###wifi device plug out, will close network_config_wifi menu###\n");
        fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
      }
      root = fw_find_root_by_id(ROOT_ID_SUBNETWORK_CONFIG);
      if(root)
      {
        fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_UI_REFRESH, 0, 0);
      }

      if((!g_net_connt_stats.is_eth_conn)&&(!g_net_connt_stats.is_usb_eth_conn)&&(!g_net_connt_stats.is_gprs_conn)&&(!g_net_connt_stats.is_3g_conn))
      {
        close_all_network_menus();
      }
      if((eth_conn_sts.is_eth_conn == FALSE) && (eth_conn_sts.is_usb_eth_conn == FALSE) && (eth_conn_sts.is_3g_conn == FALSE))
      {
        fw_notify_root(g_desktop.p_desktop, NOTIFY_T_MSG, FALSE, MSG_DESKTOP_NET_UNCONNTECED, 0, 0);
      }
#endif
    }
    else if(usb_dev_type == HP_MODEM)
    {
#ifndef WIN32
       if((g_net_connt_stats.is_eth_conn == FALSE) && (g_net_connt_stats.is_usb_eth_conn == FALSE) && (g_net_connt_stats.is_wifi_conn == FALSE))
       {
            fw_notify_root(g_desktop.p_desktop, NOTIFY_T_MSG, FALSE, MSG_DESKTOP_NET_UNCONNTECED, 0, 0);
       }
       fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_G3_READY);

       g_net_connt_stats.is_3g_insert = FALSE;
       g_net_connt_stats.is_3g_conn = FALSE;

      // paint_3g_conn_status(FALSE);


       root = fw_find_root_by_id(ROOT_ID_SUBNETWORK_CONFIG);
       if(root)
       {
           fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_UI_REFRESH, 0, 0);
       }

       ui_comm_cfmdlg_open(NULL, IDS_3G_PLUG_OUT, NULL, 2000);

      if((!g_net_connt_stats.is_eth_conn)&&(!g_net_connt_stats.is_usb_eth_conn)&&(!g_net_connt_stats.is_gprs_conn)&&(!g_net_connt_stats.is_wifi_conn))
       {
          close_all_network_menus();
       }
#endif
    }
    else if(usb_dev_type == HP_ETHERNET)
    {
      ui_comm_cfmdlg_open(NULL, IDS_USB_DISCONNECT, NULL, 2000);
    }
    else
    {
   	   ui_usb_remove_partition_sev();
      ui_set_usb_status((ui_usb_get_dev_num() == 0)? FALSE : TRUE);
        switch(root_id)
        {
          case ROOT_ID_SUBMENU:
            ctrl_paint_ctrl(fw_get_focus(), TRUE);
            break;
          case ROOT_ID_POPUP:
            if((UI_XUSB_UPG_IDEL != ui_xusb_upgade_sts())
              || (UI_USB_DUMP_IDEL != ui_usb_dumping_sts()))
            {
              manage_close_menu(ROOT_ID_POPUP, 0, 0);
              fw_notify_root(fw_find_root_by_id(ROOT_ID_BACKGROUND), NOTIFY_T_MSG,
                             FALSE, MSG_PLUG_OUT, 0, 0);
              update_signal();
              return SUCCESS;
            }
            break;
          case ROOT_ID_POPUP2:
            if((UI_XUSB_UPG_IDEL != ui_xusb_upgade_sts())
              || (UI_USB_DUMP_IDEL != ui_usb_dumping_sts()))
            {
              manage_close_menu(ROOT_ID_POPUP2, 0, 0);
              fw_notify_root(fw_find_root_by_id(ROOT_ID_BACKGROUND), NOTIFY_T_MSG,
                             FALSE, MSG_PLUG_OUT, 0, 0);
              update_signal();
              return SUCCESS;
            }
            break;
          case ROOT_ID_XUPGRADE_BY_USB:
          case ROOT_ID_DUMP_BY_USB:
            ui_exit_usb_upg(root_id);
            break;
          case ROOT_ID_TS_RECORD:
            fw_notify_root(fw_find_root_by_id(ROOT_ID_TS_RECORD), NOTIFY_T_MSG,
                             FALSE, MSG_PLUG_OUT, 0, 0);
            break;
           case ROOT_ID_MAINMENU:
            break;
        }
      ui_comm_cfmdlg_open(NULL, IDS_USB_DISCONNECT, NULL, 2000);

    }

    update_signal();

	return SUCCESS;
}


/*************************************************************

        add functions to  auto connect network

*************************************************************/
static ethernet_device_t* get_cur_connected_wifi(wifi_info_t *curWifi)
{
	wifi_status_info_t	wif_sts_info = {{0}};
	ethernet_device_t* p_wifi_dev = NULL;
	RET_CODE ret;
	
	p_wifi_dev = (ethernet_device_t *)get_wifi_handle();
	if(p_wifi_dev == NULL)
		return NULL;
	
	ret = dev_io_ctrl(p_wifi_dev, ETH_WIFI_LINK_INFO, (u32)&wif_sts_info);
	if(SUCCESS == ret)
	{
		switch(wif_sts_info.connect_sts)
		{
			case WIFI_CONNECTED:
				if(wif_sts_info.essid != NULL)
				{
					strcpy(curWifi->essid, wif_sts_info.essid);
					curWifi->encrypt_type = wif_sts_info.encrypt_type;
					DEBUG(MAIN, INFO, "essid = %s encrypt_type = %d\n",wif_sts_info.essid,wif_sts_info.encrypt_type);
				}
				else
				{
					DEBUG(MAIN, ERR, "essid == NULL\n");
				}
				break;
			case WIFI_CONNECT_FAILED:
				DEBUG(MAIN, INFO, "WIFI_CONNECT_FAILED \n");
				break;
			case NO_WIFI_DEVICE:
				DEBUG(MAIN, INFO, "NO_WIFI_DEVICE \n");
				break;
			case WIFI_DIS_CONNECT:
				DEBUG(MAIN, INFO, "WIFI_DIS_CONNECT \n");
				break;
			case WIFI_CONNECTING:
				DEBUG(MAIN, INFO, "WIFI_CONNECTING \n");
				break;
			default:
				break;
		}
	}
	else
	{
		DEBUG(MAIN, ERR, "get wifi states fail ret = %d\n",ret);
	}
	return p_wifi_dev;
}

u16 get_connected_wifi_focus(void)
{
#define WIFI_MAC_COUNT  (100)
	u8 i;
	u16 connected_focus = INVALIDID;
	wifi_info_t curWifi = {{0},{0}};
	wifi_ap_info_t ap_info = {{0}};
	ethernet_device_t* p_wifi_dev = NULL;

	p_wifi_dev = get_cur_connected_wifi(&curWifi);
	
	if(p_wifi_dev == NULL)
	{
		DEBUG(MAIN,INFO,"\n");
		return connected_focus;
	}

	if(strcmp("", curWifi.essid) == 0)
	{
		DEBUG(MAIN,INFO,"\n");
		return connected_focus;
	}
	
	for(i=0; i<WIFI_MAC_COUNT; i++)
	{
		memset(&ap_info, 0, sizeof(wifi_ap_info_t));
		wifi_ap_info_get(p_wifi_dev,i,&ap_info);
		if(strcmp("", ap_info.essid) == 0)
		{
			DEBUG(MAIN,INFO,"\n");
			break;
		}

		DEBUG(MAIN,INFO,"ap_info.essid = %s curWifi.essid = %s \n",ap_info.essid,curWifi.essid);
		if((strcmp(curWifi.essid, ap_info.essid) == 0) 
			&& (strlen(curWifi.essid) == strlen(ap_info.essid)))
		{
			connected_focus =  i;
			break;
		}
	}

	DEBUG(MAIN,INFO,"connected_focus = %d i=%d \n",connected_focus,i);
	return connected_focus;
}

extern BOOL check_is_connect_ok(ethernet_device_t * p_eth_dev);
void init_ethernet_ethcfg(ethernet_cfg_t *ethcfg)
{
  ip_address_set_t ss_ip = {{0}};
  net_config_t lan_config = {0};

  sys_status_get_net_config_info(&lan_config);
  ethcfg->tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
  if(lan_config.config_mode == DHCP)
  {
    ethcfg->is_enabledhcp = 1;
  }
  else if(lan_config.config_mode == STATIC_IP)
  {
    ethcfg->is_enabledhcp = 0;
  }

  sys_status_get_ipaddress(&ss_ip);
  //ipaddress
  ethcfg->ipaddr[0] = ss_ip.sys_ipaddress.s_a4;
  ethcfg->ipaddr[1] = ss_ip.sys_ipaddress.s_a3;
  ethcfg->ipaddr[2] = ss_ip.sys_ipaddress.s_a2;
  ethcfg->ipaddr[3] = ss_ip.sys_ipaddress.s_a1;

  //netmask
  ethcfg->netmask[0] = ss_ip.sys_netmask.s_a4;
  ethcfg->netmask[1] = ss_ip.sys_netmask.s_a3;
  ethcfg->netmask[2] = ss_ip.sys_netmask.s_a2;
  ethcfg->netmask[3] = ss_ip.sys_netmask.s_a1;

  //gateway
  ethcfg->gateway[0] = ss_ip.sys_gateway.s_a4;
  ethcfg->gateway[1] = ss_ip.sys_gateway.s_a3;
  ethcfg->gateway[2] = ss_ip.sys_gateway.s_a2;
  ethcfg->gateway[3] = ss_ip.sys_gateway.s_a1;

  //dns server
  ethcfg->primaryDNS[0] = ss_ip.sys_dnsserver.s_a4;
  ethcfg->primaryDNS[1] = ss_ip.sys_dnsserver.s_a3;
  ethcfg->primaryDNS[2] = ss_ip.sys_dnsserver.s_a2;
  ethcfg->primaryDNS[3] = ss_ip.sys_dnsserver.s_a1;
}

void do_cmd_connect_network(ethernet_cfg_t *ethcfg, ethernet_device_t * eth_dev)
{
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;

  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  memset(&net_svc_para, 0 , sizeof(net_svc_para));
  net_svc_para.p_eth_dev = eth_dev;
  memcpy(&net_svc_para.net_cfg, ethcfg, sizeof(ethernet_cfg_t));
  p_server = (service_t *)ui_get_net_svc_instance();
  p_server->do_cmd(p_server, NET_DO_CONFIG_TCPIP, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
}
void auto_connect_ethernet()
{

  ethernet_cfg_t ethcfg = {0};
  ethernet_device_t * eth_dev = NULL;

  if(g_net_connt_stats.is_eth_insert)
  {
    eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
    //mac address
    ethcfg.hwaddr[0] = sys_status_get_mac_by_index(0);
    ethcfg.hwaddr[1] = sys_status_get_mac_by_index(1);
    ethcfg.hwaddr[2] = sys_status_get_mac_by_index(2);
    ethcfg.hwaddr[3] = sys_status_get_mac_by_index(3);
    ethcfg.hwaddr[4] = sys_status_get_mac_by_index(4);
    ethcfg.hwaddr[5] = sys_status_get_mac_by_index(5);
  }
  else
  {
    eth_dev = (ethernet_device_t *)ui_get_usb_eth_dev();
    dev_io_ctrl(eth_dev, GET_ETH_HW_MAC_ADDRESS, (u32)&ethcfg.hwaddr[0]);
  }
  MT_ASSERT(eth_dev != NULL);
  OS_PRINTF("MAC= %02x-%02x-%02x-%02x-%02x-%02x\n",
              ethcfg.hwaddr[0], ethcfg.hwaddr[1],ethcfg.hwaddr[2],
              ethcfg.hwaddr[3], ethcfg.hwaddr[4], ethcfg.hwaddr[5]);

  init_ethernet_ethcfg(&ethcfg);
  do_cmd_connect_network(&ethcfg, eth_dev);
}

void do_cmd_connect_gprs(gprs_info_t *p_gprs_info)
{
#if 0
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;
  void *p_dev = NULL;

  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"neo_m660");
  MT_ASSERT(p_dev != NULL);

  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  memset(&net_svc_para, 0 , sizeof(net_svc_para));
  net_svc_para.p_eth_dev = p_dev;

  net_svc_para.net_cfg.gprs_para.tcpip_task_prio = ETH_NET_TASK_PRIORITY;
  strcpy(net_svc_para.net_cfg.gprs_para.apn, p_gprs_info->apn);
  strcpy(net_svc_para.net_cfg.gprs_para.dial_num, p_gprs_info->dial_num);
  strcpy(net_svc_para.net_cfg.gprs_para.usrname, p_gprs_info->usr_name);
  strcpy(net_svc_para.net_cfg.gprs_para.passwd, p_gprs_info->passwd);

  p_server = (service_t *)ui_get_net_svc_instance();
  if(p_server != NULL)
  {
    p_server->do_cmd(p_server, NET_DO_GPRS_CONNECT,
                     (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
  }
#endif
}

void do_cmd_disconnect_gprs()
{
#if 0
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;
  void *p_dev = NULL;

  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"neo_m660");
  MT_ASSERT(p_dev != NULL);

  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  memset(&net_svc_para, 0 , sizeof(net_svc_para));
  net_svc_para.p_eth_dev = p_dev;

  p_server = (service_t *)ui_get_net_svc_instance();
  if(p_server != NULL)
  {
    p_server->do_cmd(p_server, NET_DO_GPRS_DISCONNECT, (u32)&net_svc_para,
                     sizeof(net_svc_cmd_para_t));
  }
#endif
}


void do_cmd_connect_g3(g3_conn_info_t *p_g3_info)
{
#if 0
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;
  void *p_dev = NULL;

  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"usb_serial_modem");
  MT_ASSERT(p_dev != NULL);

  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  memset(&net_svc_para, 0 , sizeof(net_svc_para));
  net_svc_para.p_eth_dev = p_dev;

  if(p_g3_info != NULL)
  {
      strcpy(net_svc_para.net_cfg.g3_para.apn, p_g3_info->apn);
      strcpy(net_svc_para.net_cfg.g3_para.dial_num, p_g3_info->dial_num);
      strcpy(net_svc_para.net_cfg.g3_para.usrname, p_g3_info->usr_name);
      strcpy(net_svc_para.net_cfg.g3_para.passwd, p_g3_info->passwd);
  }

  p_server = (service_t *)ui_get_net_svc_instance();
  if(p_server != NULL)
  {
    p_server->do_cmd(p_server, NET_DO_3G_CONNECT,
                     (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
  }
#endif
}

void do_cmd_disconnect_g3()
{
#if 0
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;
  void *p_dev = NULL;

  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"usb_serial_modem");
  MT_ASSERT(p_dev != NULL);

  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  memset(&net_svc_para, 0 , sizeof(net_svc_para));
  net_svc_para.p_eth_dev = p_dev;

  p_server = (service_t *)ui_get_net_svc_instance();
  if(p_server != NULL)
  {
    p_server->do_cmd(p_server, NET_DO_3G_DISCONNECT, (u32)&net_svc_para,
                     sizeof(net_svc_cmd_para_t));
  }
#endif
}

void do_cmd_get_g3_info()
{
#if 0
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;
  void *p_dev = NULL;

  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"usb_serial_modem");
  MT_ASSERT(p_dev != NULL);

  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  memset(&net_svc_para, 0 , sizeof(net_svc_para));
  net_svc_para.p_eth_dev = p_dev;

  p_server = (service_t *)ui_get_net_svc_instance();
  if(p_server != NULL)
  {
    p_server->do_cmd(p_server, NET_DO_GET_3G_INFO,
                     (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
  }
#endif
}

extern wifi_info_t get_connecting_wifi_info(void);
static void save_wifi_info_ssdata()
{
	wifi_info_t wifi_info;

	get_cur_connected_wifi(&p_wifi_info);

	wifi_info = get_connecting_wifi_info();
	//sys_status_get_wifi_info(&wifi_info,p_wifi_info.essid);
	if(strcmp(wifi_info.essid, p_wifi_info.essid) == 0)
	{
		p_wifi_info = wifi_info;
	}
	
	set_temp_wifi_info(p_wifi_info);
	sys_status_set_wifi_info(&p_wifi_info);
	sys_status_save();
}

extern void get_addr_param(u8 *p_buffer, ip_address_t *p_addr);

void ui_notify_desktop_to_get_livetv_info()
{
    fw_notify_root(g_desktop.p_desktop, NOTIFY_T_MSG, FALSE, MSG_DESKTOP_NET_CONNTECED, 0, 0);
}

static RET_CODE on_save_config_ip_to_ssdata(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	ip_address_t addr = {0,};
	ethernet_device_t *eth_dev = NULL;
	ip_address_set_t ss_ip = {{0}};

	u8 ipaddr[20] = {0};
	u8 netmask[20] = {0};
	u8 gw[20] = {0};
	u8 primarydns[20] = {0};
	u8 alternatedns[20] = {0};
	u8 ipaddress[32] = {0};
	control_t *root = NULL;
	u32 is_connect = para1;

	OS_PRINTF("####destop after auto connect network the para1 is_connect == %d  msg = %x MSG_CONFIG_IP[%x] ###\n", para1, msg,MSG_CONFIG_IP);



	if(is_connect)
	{
		OS_PRINTF("###########lwip_init_tcpip return failed, desktop connect failed######\n ");
		return ERR_FAILURE;
	}
	if(g_net_connt_stats.is_eth_insert)
	{
		eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
	}
	else
	{
		eth_dev = (ethernet_device_t *)ui_get_usb_eth_dev();
	}
	MT_ASSERT(eth_dev != NULL);
	
	#ifndef WIN32
	get_net_device_addr_info(eth_dev, ipaddr, netmask, gw, primarydns, alternatedns);

	get_addr_param(ipaddr, (ip_address_t *)&addr);
	memcpy(&(ss_ip.sys_ipaddress), &addr, sizeof(ip_addr_t));
	sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
	OS_PRINTF("###ethernet auto connect  get dhcp ip address value is:%s####\n",ipaddress);
	get_addr_param(netmask, (ip_address_t *)&addr);
	memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
	sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
	OS_PRINTF("###ethernet auto connect  get dhcp netmask value is:%s####\n",ipaddress);
	get_addr_param(gw, (ip_address_t *)&addr);
	memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
	sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
	OS_PRINTF("###ethernet auto connect  get dhcp gateway  value is:%s####\n",ipaddress);
	get_addr_param(primarydns, (ip_address_t *)&addr);
	memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
	sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
	OS_PRINTF("###ethernet auto connect  get dhcp dns value is:%s####\n",ipaddress);
	#endif
	
	sys_status_set_ipaddress(&ss_ip);
	sys_status_save();

	OS_PRINTF("@@@!auto connect ethernet successfully@@@\n");
	if(g_net_connt_stats.is_eth_insert)
	{
		g_net_connt_stats.is_eth_conn = TRUE;
		g_net_connt_stats.is_usb_eth_conn = FALSE;
	}
	else
	{
		g_net_connt_stats.is_usb_eth_conn = TRUE;
		g_net_connt_stats.is_eth_conn = FALSE;
	}

	g_net_connt_stats.is_gprs_conn = FALSE;
	g_net_connt_stats.is_3g_conn = FALSE;
	g_net_connt_stats.is_wifi_conn = FALSE;

	root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_LAN);

	if(root)
	{
		OS_PRINTF("###will update ethernet dhcp and paint network config connect status at network config menu###\n");
		on_config_ip_update(eth_dev);//update ethernet dhcp ipaddress
		paint_connect_status((g_net_connt_stats.is_eth_conn || g_net_connt_stats.is_usb_eth_conn), TRUE);
	}

	return SUCCESS;
}

static RET_CODE on_save_config_wifi_to_ssdata(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	ip_address_t addr = {0,};
	ip_address_set_t ss_ip = {{0}};
	u8 ipaddr[20] = {0};
	u8 netmask[20] = {0};
	u8 gw[20] = {0};
	u8 primarydns[20] = {0};
	u8 alternatedns[20] = {0};
	u8 ipaddress[32] = {0};
	control_t *root = NULL;
	OS_PRINTF("####destop after auto connect network the para1 is_connect == %d  msg = %x MSG_CONFIG_IP[%x] ###\n", para1, msg,MSG_CONFIG_IP);

	root = fw_find_root_by_id(ROOT_ID_WIFI_LINK_INFO);
	if(root)
	{
		OS_PRINTF("###at wifi ui, should return from destop connect ok###\n");
		return ERR_FAILURE;
	}

	g_net_connt_stats = ui_get_net_connect_status();//at for network config ui

	#ifndef WIN32
	if(g_net_connt_stats.is_wifi_insert == FALSE)
	{
		return SUCCESS;
	}
	
	#ifndef WIN32
	p_wifi_devv = (ethernet_device_t *)get_wifi_handle();
	#endif
	
	if(p_wifi_devv == NULL)
	{
		OS_PRINTF("cannot get wifi handle maybe plug out.........\n");
		return ERR_FAILURE;
	}
	get_net_device_addr_info(p_wifi_devv, ipaddr, netmask, gw, primarydns, alternatedns);

	get_addr_param(ipaddr, (ip_address_t *)&addr);
	memcpy(&(ss_ip.sys_ipaddress), &addr, sizeof(ip_addr_t));
	sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
	OS_PRINTF("###wifi auto connect  get dhcp ip address value is:%s####\n",ipaddress);
	get_addr_param(netmask, (ip_address_t *)&addr);
	memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
	sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
	OS_PRINTF("###wifi auto connect  get dhcp netmask value is:%s####\n",ipaddress);
	get_addr_param(gw, (ip_address_t *)&addr);
	memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
	sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
	OS_PRINTF("###wifi auto connect  get dhcp gateway  value is:%s####\n",ipaddress);
	get_addr_param(primarydns, (ip_address_t *)&addr);
	memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
	sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
	OS_PRINTF("###wifi auto connect  get dhcp dns value is:%s####\n",ipaddress);
	#endif
	
	save_wifi_info_ssdata();
	sys_status_set_ipaddress(&ss_ip);
	sys_status_save();
	g_net_connt_stats.is_wifi_conn = TRUE;

	g_net_connt_stats.is_gprs_conn = FALSE;
	g_net_connt_stats.is_3g_conn = FALSE;
	g_net_connt_stats.is_eth_conn = FALSE;
	g_net_connt_stats.is_usb_eth_conn = FALSE;

	g_net_conn_info.wifi_conn_info = WIFI_CONNECTED;

	OS_PRINTF("@@@!auto connect wifi successfully@@@\n");
	root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_WIFI);
	if(root)
	{
		OS_PRINTF("###will update wifi dhcp and paint network config connect status at network config menu###\n");
		on_config_ip_update_wifi(p_wifi_devv);//update wifi dhcp ipaddress
		paint_connect_wifi_status(g_net_connt_stats.is_wifi_conn, TRUE);
	}
	root = fw_find_root_by_id(ROOT_ID_WIFI);
	if(root)
	{
		u16 focus = get_connected_wifi_focus();
		OS_PRINTF("$$$$$$$$$focus======%d$$$$$$$$$$$\n",focus);
		paint_wifi_status(TRUE, TRUE);
		paint_list_field_is_connect(focus, TRUE, TRUE);
	}

	return SUCCESS;
}

static RET_CODE on_desktop_gprs_status_update(control_t *p_ctrl, u16 msg,
                                                          u32 para1, u32 para2)
{
#if 0
   u32 status = para1;

   if(status == MODEM_DEVICE_NETWORK_CONNECTED)
   {
      on_save_config_ip_to_ssdata(p_ctrl, msg, para1, para2);
      paint_gprs_conn_status(TRUE);
   }
   else if(status == MODEM_DEVICE_NETWORK_UNCONNECTED)
   {
     //
     paint_gprs_conn_status(FALSE);
   }

   g_net_conn_info.gprs_conn_info = para1;

   set_gprs_connect_status(status);
#endif
   return SUCCESS;
}

static RET_CODE on_desktop_g3_status_update(control_t *p_ctrl, u16 msg,
                                                          u32 para1, u32 para2)
{
#if 0
   g3_status_info_t *p_g3_info = NULL;

   p_g3_info = (g3_status_info_t *)para1;
   if(p_g3_info->status == MODEM_DEVICE_NETWORK_CONNECTED)
   {
      on_save_config_ip_to_ssdata(p_ctrl, msg, para1, para2);
   }

   g_net_conn_info.g3_conn_info = p_g3_info->status;

   set_3g_connect_status(p_g3_info->status, p_g3_info->strength, p_g3_info->operator);
#endif
   return SUCCESS;
}


static RET_CODE on_3g_dev_ready(control_t *p_ctrl, u16 msg,
                                                          u32 para1, u32 para2)
{
#if 0
   g3_conn_info_t p_g3_info = {{0}};

  // extern u32 get_3g_status();
   if((g_net_conn_info.g3_conn_info != MODEM_DEVICE_NO_SIM_CARD) && (g_net_conn_info.g3_conn_info != MODEM_DEVICE_NO_SIGNAL))
   {
        fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_G3_READY);
        memcpy(&p_g3_info, sys_status_get_3g_info(), sizeof(g3_conn_info_t));
        do_cmd_connect_g3(&p_g3_info);
   }
#endif
   return SUCCESS;
}


static RET_CODE on_desktop_wifi_connecting(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
    g_net_conn_info.wifi_conn_info = WIFI_CONNECTING;

    return SUCCESS;

}

static RET_CODE on_desktop_connect_wifi_failed(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  control_t *root = NULL;
  net_config_t net_config = {0};
  net_conn_stats_t wifi_conn_sts = {0};
  root = fw_find_root_by_id(ROOT_ID_WIFI_LINK_INFO);

  g_net_conn_info.wifi_conn_info = WIFI_DIS_CONNECT;

  if(root)
  {
    OS_PRINTF("###at wifi ui, should return from destop connect failed###\n");
    return ERR_FAILURE;
  }
  wifi_conn_sts = ui_get_net_connect_status();
/*  if(!wifi_conn_sts.is_wifi_conn)
  {
    OS_PRINTF("####wifi status is disconnect, should ignore this disconnect msg###\n");
    return ERR_FAILURE;
  }*/
  sys_status_get_net_config_info(&net_config);
  if(net_config.link_type == LINK_TYPE_LAN)
  {
    OS_PRINTF("###link type is ethernet, wifi connect failed will return###\n");
    return ERR_FAILURE;
  }
  else if(net_config.link_type == LINK_TYPE_WIFI)
  {
    wifi_conn_sts.is_wifi_conn = FALSE;
    ui_set_net_connect_status(wifi_conn_sts);
    root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_WIFI);
    if(root)
    {
      paint_wifi_connect_status(wifi_conn_sts.is_wifi_conn, TRUE);
    }
    root = fw_find_root_by_id(ROOT_ID_WIFI);
    if(root)
    {
      control_t *p_list = NULL;
      p_list = ui_comm_root_get_ctrl(ROOT_ID_WIFI, 8);//IDC_WIFI_LIST
      paint_wifi_list_field_not_connect(p_list);
      paint_wifi_status(FALSE, TRUE);
    }
  }

  OS_PRINTF("###at  destop, connect wifi failed###\n");
  return SUCCESS;
}

static void close_all_samba_menu()
{
  control_t *root = NULL;
  root = ui_comm_root_get_root(ROOT_ID_USB_MUSIC);
  if(root && !get_music_is_usb())
  {
    control_t *full_screen_root = NULL;
    full_screen_root = ui_comm_root_get_root(ROOT_ID_USB_MUSIC_FULLSCREEN);
    if(full_screen_root)
    {
      ctrl_process_msg(full_screen_root, MSG_EXIT, 0, 0);
    }
    ctrl_process_msg(root, MSG_EXIT, 0, 0);
  }
  root = ui_comm_root_get_root(ROOT_ID_USB_PICTURE);
  if(root && !get_picture_is_usb())
  {
    control_t *full_screen_root = NULL;
    full_screen_root = ui_comm_root_get_root(ROOT_ID_PICTURE);
    if(full_screen_root)
    {
      ctrl_process_msg(full_screen_root, MSG_EXIT, 0, 0);
    }
    ctrl_process_msg(root, MSG_EXIT, 0, 0);
  }
  root = ui_comm_root_get_root(ROOT_ID_USB_FILEPLAY);

    if(root && !get_video_is_usb())
  {
    control_t *full_screen_root = NULL;
    full_screen_root = ui_comm_root_get_root(ROOT_ID_FILEPLAY_BAR);
    if(full_screen_root)
    {
      ctrl_process_msg(full_screen_root, MSG_EXIT, 0, 0);
    }
    ctrl_process_msg(root, MSG_EXIT, 0, 0);
  }
}

static void close_common_menus()
{
  if(NULL != fw_find_root_by_id(ROOT_ID_POPUP))
  {
    manage_close_menu(ROOT_ID_POPUP, 0, 0);
  }
  if(NULL != fw_find_root_by_id(ROOT_ID_KEYBOARD_V2))
  {
    manage_close_menu(ROOT_ID_KEYBOARD_V2, 0, 0);
  }
}

static void close_all_network_menus()
{
  control_t *root = NULL;

  root = fw_find_root_by_id(ROOT_ID_ONMOV_WEBSITES);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
  }

  root = fw_find_root_by_id(ROOT_ID_NETWORK_PLAY);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
  }

  root = fw_find_root_by_id(ROOT_ID_ONMOV_DESCRIPTION);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
  }

  root = fw_find_root_by_id(ROOT_ID_VIDEO_PLAYER);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
  }

  root = fw_find_root_by_id(ROOT_ID_IPTV_PLAYER);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT_ALL, 0, 0);
  }

  root = fw_find_root_by_id(ROOT_ID_YOUTUBE);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
  }

  root = ui_comm_root_get_root(ROOT_ID_GOOGLE_MAP);
  if(root)
  {
    ctrl_process_msg(root, MSG_EXIT, 0, 0);
  }

  root = ui_comm_root_get_root(ROOT_ID_WEATHER_FORECAST);
  if(root)
  {
    ctrl_process_msg(root, MSG_EXIT, 0, 0);
  }

  root = ui_comm_root_get_root(ROOT_ID_LIVE_TV);
  if(root)
  {
    ctrl_process_msg(root, MSG_EXIT_ALL, 0, 0);
  }

  close_all_samba_menu();
}

#if 0
/*********************************************************/
/*                         if lan or wifi disconnect, will close satip and dlna        */
 /*********************************************************/
static void close_all_dlna_switch_after_disconnect_lan_or_wifi()
{
    control_t *p_root = NULL;

    OS_PRINTF("####%s####\n", __FUNCTION__);

    p_root = ui_comm_root_get_root(ROOT_ID_DLNA_DMR);
    if(p_root)
    {
        OS_PRINTF("##%s, line[%d], send MSG_EXIT to dlna##\n", __FUNCTION__, __LINE__);
        ctrl_process_msg(p_root, MSG_EXIT, 0, 0);
    }

    manage_close_menu(ROOT_ID_SATIP, 0, 0);
    if(ui_get_dlna_status(SATIP) == DLNA_ON)
    {
        ui_comm_showdlg_open(NULL, IDS_CLOSING_SATIP, NULL, 2000);
        ui_cg_dlna_server_stop();
        ui_stop_mini_httpd();
        ui_set_dlna_status(SATIP, DLNA_OFF);
    }
}
#endif

#if ENABLE_ETH
static RET_CODE on_ethernet_plug_in(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  net_config_t g_net_config = {0,};
  control_t *root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_LAN);
  comm_dlg_data_t dlg_data =
  {
    0,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  };

  g_net_connt_stats.is_eth_insert = TRUE;

  if(root)
  {
    OS_PRINTF("###at network config, should return from destop plug in###\n");
    return ERR_FAILURE;
  }


  root = fw_find_root_by_id(ROOT_ID_SUBNETWORK_CONFIG);
  if(root)
  {
      fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_UI_REFRESH, 0, 0);
  }


  sys_status_get_net_config_info(&g_net_config);
  if (g_net_config.link_type == LINK_TYPE_LAN)
  {
    if((g_net_connt_stats.is_eth_conn == FALSE)&&(g_net_connt_stats.is_usb_eth_conn == FALSE))
    {
      auto_connect_ethernet();
    }
  }
  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
  {
      ui_comm_dlg_close();
  }

  dlg_data.content = IDS_NET_CABLE_PLUG_IN;
  dlg_data.dlg_tmout = 2000;
  ui_comm_dlg_open(&dlg_data);
  return SUCCESS;
}

static RET_CODE on_ethernet_plug_out(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  comm_dlg_data_t dlg_data =
  {
    0,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  };

  //ui_comm_cfmdlg_open(NULL, IDS_NET_CABLE_PLUG_OUT, NULL, 3000);
  control_t *root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_LAN);
  BOOL flag = FALSE;
  dlg_data.content = IDS_NET_CABLE_PLUG_OUT;
  dlg_data.dlg_tmout = 2000;

  if((root != NULL) && g_net_connt_stats.is_eth_conn)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
  }
  //if(ROOT_ID_MAINMENU == fw_get_focus_id())
  OS_PRINTF("############desktop--->>ethernet plug out###########\n");
  g_net_connt_stats.is_eth_insert = FALSE;
  g_net_connt_stats.is_eth_conn = FALSE;

 if(ROOT_ID_SUBNETWORK_CONFIG == fw_get_focus_id())
  {
    control_t *p_root = fw_find_root_by_id(ROOT_ID_SUBNETWORK_CONFIG);
    if(p_root)
    {
      fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UI_REFRESH, 0, 0);
      flag = TRUE;
    }
  }
  close_common_menus();

  if(root)
  {
    OS_PRINTF("###at network config, should return from destop plug out###\n");
    return ERR_FAILURE;
  }

  g_net_connt_stats = ui_get_net_connect_status();
  if((!g_net_connt_stats.is_wifi_conn)&&(!g_net_connt_stats.is_usb_eth_conn)&&(!g_net_connt_stats.is_gprs_conn)&&(!g_net_connt_stats.is_3g_conn))
  {
    close_all_network_menus();
  }

  if((g_net_connt_stats.is_usb_eth_conn == FALSE) && (g_net_connt_stats.is_wifi_conn == FALSE) && (g_net_connt_stats.is_3g_conn == FALSE))
  {
    fw_notify_root(g_desktop.p_desktop, NOTIFY_T_MSG, FALSE, MSG_DESKTOP_NET_UNCONNTECED, 0, 0);
  }
  if((fw_get_focus_id() == ROOT_ID_SMALL_LIST) ||
    (fw_get_focus_id() == ROOT_ID_PROG_BAR) ||
    (fw_get_focus_id() == ROOT_ID_BACKGROUND))
  {
      ui_video_c_stop();
      dlg_data.content = IDS_NET_CABLE_PLUG_OUT;
      if(ui_get_play_prg_type() == NET_PRG_TYPE)
        dlg_data.dlg_tmout = 0;
      else
        dlg_data.dlg_tmout = 2000;
      ui_comm_dlg_close();
      if(fw_find_root_by_id(ROOT_ID_POPUP) == NULL)
      {
        ui_comm_dlg_open(&dlg_data);
      }
  }

  ui_comm_dlg_open(&dlg_data);

  return SUCCESS;
}
#endif

static RET_CODE on_usb_ethernet_plug_in(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  net_config_t g_net_config = {0,};
  control_t *root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_LAN);
  comm_dlg_data_t dlg_data =
  {
    0,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  };

  g_net_connt_stats.is_usb_eth_insert = TRUE;

  if(root)
  {
    OS_PRINTF("###at network config, should return from destop plug in###\n");
    return ERR_FAILURE;
  }


  root = fw_find_root_by_id(ROOT_ID_SUBNETWORK_CONFIG);
  if(root)
  {
      fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_UI_REFRESH, 0, 0);
  }


  sys_status_get_net_config_info(&g_net_config);
  if (g_net_config.link_type == LINK_TYPE_LAN)
  {
    if((g_net_connt_stats.is_eth_insert== FALSE)&&(g_net_connt_stats.is_usb_eth_conn == FALSE))
    {
      auto_connect_ethernet();
    }
  }
  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
  {
      ui_comm_dlg_close();
  }

  dlg_data.content = IDS_NET_CABLE_PLUG_IN;
  dlg_data.dlg_tmout = 2000;
  ui_comm_dlg_open(&dlg_data);
  return SUCCESS;
}

static RET_CODE on_usb_ethernet_plug_out(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  comm_dlg_data_t dlg_data =
  {
    0,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  };

  control_t *root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_LAN);
  BOOL flag = FALSE;
  dlg_data.content = IDS_NET_CABLE_PLUG_OUT;
  dlg_data.dlg_tmout = 2000;

  if((root != NULL) && g_net_connt_stats.is_usb_eth_conn)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
  }
  g_net_connt_stats.is_usb_eth_insert = FALSE;
  g_net_connt_stats.is_usb_eth_conn = FALSE;

  if(ROOT_ID_SUBNETWORK_CONFIG == fw_get_focus_id())
  {
    control_t *p_root = fw_find_root_by_id(ROOT_ID_SUBNETWORK_CONFIG);
    if(p_root)
    {
      fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UI_REFRESH, 0, 0);
      flag = TRUE;
    }
  }
  close_common_menus();

  if(root)
  {
    OS_PRINTF("###at network config, should return from destop plug out###\n");
    return ERR_FAILURE;
  }

  g_net_connt_stats = ui_get_net_connect_status();
  if((!g_net_connt_stats.is_eth_conn)&&(!g_net_connt_stats.is_wifi_conn)&&(!g_net_connt_stats.is_gprs_conn)&&(!g_net_connt_stats.is_3g_conn))
  {
    close_all_network_menus();
  }

  if((g_net_connt_stats.is_eth_conn == FALSE) && (g_net_connt_stats.is_wifi_conn == FALSE) && (g_net_connt_stats.is_3g_conn == FALSE))
  {
    fw_notify_root(g_desktop.p_desktop, NOTIFY_T_MSG, FALSE, MSG_DESKTOP_NET_UNCONNTECED, 0, 0);
  }
  if((fw_get_focus_id() == ROOT_ID_SMALL_LIST) ||
    (fw_get_focus_id() == ROOT_ID_PROG_BAR) ||
    (fw_get_focus_id() == ROOT_ID_BACKGROUND))
  {
      ui_video_c_stop();
      dlg_data.content = IDS_NET_CABLE_PLUG_OUT;
      if(ui_get_play_prg_type() == NET_PRG_TYPE)
        dlg_data.dlg_tmout = 0;
      else
        dlg_data.dlg_tmout = 2000;
      ui_comm_dlg_close();
      if(fw_find_root_by_id(ROOT_ID_POPUP) == NULL)
      {
        ui_comm_dlg_open(&dlg_data);
      }
  }

  ui_comm_dlg_open(&dlg_data);

  return SUCCESS;
}
/**************************************
    auto connect wifi

 **************************************/
void init_desktop_wifi_ethcfg()
{
//   ip_address_set_t ss_ip = {{0}};

   wifi_ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
   wifi_ethcfg.is_enabledhcp = 1;

   /*sys_status_get_ipaddress(&ss_ip);
   //ipaddress
  wifi_ethcfg.ipaddr[0] = ss_ip.sys_ipaddress.s_a4;
  wifi_ethcfg.ipaddr[1] = ss_ip.sys_ipaddress.s_a3;
  wifi_ethcfg.ipaddr[2] = ss_ip.sys_ipaddress.s_a2;
  wifi_ethcfg.ipaddr[3] = ss_ip.sys_ipaddress.s_a1;

  //netmask
  wifi_ethcfg.netmask[0] = ss_ip.sys_netmask.s_a4;
  wifi_ethcfg.netmask[1] = ss_ip.sys_netmask.s_a3;
  wifi_ethcfg.netmask[2] = ss_ip.sys_netmask.s_a2;
  wifi_ethcfg.netmask[3] = ss_ip.sys_netmask.s_a1;

  //gateway
  wifi_ethcfg.gateway[0] = ss_ip.sys_gateway.s_a4;
  wifi_ethcfg.gateway[1] = ss_ip.sys_gateway.s_a3;
  wifi_ethcfg.gateway[2] = ss_ip.sys_gateway.s_a2;
  wifi_ethcfg.gateway[3] = ss_ip.sys_gateway.s_a1;

  //dns server
  wifi_ethcfg.primaryDNS[0] = ss_ip.sys_dnsserver.s_a4;
  wifi_ethcfg.primaryDNS[1] = ss_ip.sys_dnsserver.s_a3;
  wifi_ethcfg.primaryDNS[2] = ss_ip.sys_dnsserver.s_a2;
  wifi_ethcfg.primaryDNS[3] = ss_ip.sys_dnsserver.s_a1;*/

  //mac address
  dev_io_ctrl(p_wifi_devv, GET_ETH_HW_MAC_ADDRESS, (u32)&wifi_ethcfg.hwaddr[0]);
  OS_PRINTF("#######init_wifi_ethcfg#######MAC= %02x-%02x-%02x-%02x-%02x-%02x##############\n",
                          wifi_ethcfg.hwaddr[0], wifi_ethcfg.hwaddr[1],wifi_ethcfg.hwaddr[2],
                          wifi_ethcfg.hwaddr[3], wifi_ethcfg.hwaddr[4], wifi_ethcfg.hwaddr[5]);

}

#if 0
void do_cmd_to_search_wifi()
{
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;

  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  if(p_net_svc->net_svc_is_busy(p_net_svc) == TRUE)
  {
    ui_comm_cfmdlg_open(NULL, IDS_NETWORK_BUSY, NULL, 2000);
    OS_PRINTF("###########net service is busy ,will be return#########\n");
    return ;
  }
  net_svc_para.p_eth_dev = p_wifi_devv;
  p_server = (service_t *)ui_get_net_svc_instance();

  p_server->do_cmd(p_server, NET_DO_WIFI_SERACH, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
}
#endif
void do_cmd_to_conn_desktop_wifi()
{
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;

  sys_status_get_wifi_info(&p_wifi_info,NULL);
  if(strlen(p_wifi_info.essid) == 0)
  {
    return;
  }

  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  net_svc_para.p_eth_dev = p_wifi_devv;
  memcpy(&net_svc_para.net_cfg, &wifi_ethcfg, sizeof(ethernet_cfg_t));
  strcpy(net_svc_para.net_cfg.wifi_para.key, p_wifi_info.key);
  strcpy(net_svc_para.net_cfg.wifi_para.ssid, p_wifi_info.essid);
  net_svc_para.net_cfg.wifi_para.is_enable_encrypt = 1;
  net_svc_para.net_cfg.wifi_para.encrypt_type = p_wifi_info.encrypt_type;
  OS_PRINTF("#####################do_cmd_to_conn_desktop_wifi essid == %s###################\n",net_svc_para.net_cfg.wifi_para.ssid);
  p_server = (service_t *)ui_get_net_svc_instance();

  p_server->do_cmd(p_server, NET_DO_WIFI_CONNECT, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));

}

#if 0
static RET_CODE on_desktop_search_wifi_over(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
 {
#ifndef WIN32
  u32 wifi_ap_cnt = para1;
  u16 i;
  wifi_ap_info_t ap_info;
  control_t *root = fw_find_root_by_id(ROOT_ID_WIFI);
  if(root)
  {
    OS_PRINTF("###after search wifi, get ap count == %d###\n",wifi_ap_cnt);
    OS_PRINTF("###at wifi ui, should return from destop wifi search###\n");
    return ERR_FAILURE;
  }
  sys_status_get_wifi_info(&p_wifi_info);
  if(wifi_ap_cnt < 0 || wifi_ap_cnt == 0 || strcmp("", p_wifi_info.essid) == 0)
  {
    OS_PRINTF("@@@@wifi count <= 0, or ssdata wifi essid is NULL, so return@@\n");
    return ERR_FAILURE;
  }
  for(i = 0; i < wifi_ap_cnt; i++)
  {
    wifi_ap_info_get(p_wifi_devv,(u32)i, &ap_info);
    if(strcmp(p_wifi_info.essid, ap_info.essid) == 0)
    {
      OS_PRINTF("###desktop search wifi equal ssdata wifi, is %s, will auto connect  wifi####\n", ap_info.essid);
      do_cmd_to_conn_desktop_wifi();
    }
  }

  OS_PRINTF("@@@@wifi count == %d@@\n",wifi_ap_cnt);
  OS_PRINTF("@@@on_wifi_search_ok@@@\n");
#endif
  return SUCCESS;
}
#endif
#if 0
static RET_CODE on_desktop_wifi_connect_ap(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
 {
#ifndef WIN32
  ethernet_device_t * p_wifi_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE,
              SYS_DEV_TYPE_USB_WIFI);
  ethernet_cfg_t ethcfg = {0};
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;

  ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
  ethcfg.is_enabledhcp = 1;

  //mac address
  dev_io_ctrl(p_wifi_dev, GET_ETH_HW_MAC_ADDRESS, (u32)&ethcfg.hwaddr[0]);
  OS_PRINTF("#######init_wifi_ethcfg#######MAC= %02x-%02x-%02x-%02x-%02x-%02x##############\n",
                    ethcfg.hwaddr[0], ethcfg.hwaddr[1],ethcfg.hwaddr[2],
                    ethcfg.hwaddr[3], ethcfg.hwaddr[4], ethcfg.hwaddr[5]);


  net_svc_para.p_eth_dev = p_wifi_dev;
  p_server = (service_t *)ui_get_net_svc_instance();
  memcpy(&net_svc_para.net_cfg, &ethcfg, sizeof(ethernet_cfg_t));
  p_server->do_cmd(p_server, NET_DO_CONFIG_TCPIP, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
  OS_PRINTF("set wifi tcpip\n");
#endif
  return SUCCESS;
}
#endif
static void auto_connect_wifi()
{
  #ifndef WIN32
  p_wifi_devv = (ethernet_device_t *)get_wifi_handle();
  //p_wifi_devv = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_WIFI);
  if(p_wifi_devv == NULL)
  {
    OS_PRINTF("Wifi has been plug out\n\n");
    return;
  }
  #endif

  init_desktop_wifi_ethcfg();
 // do_cmd_to_search_wifi();
  do_cmd_to_conn_desktop_wifi();
}

#ifndef IMPL_NEW_EPG
static RET_CODE on_pf_change(control_t *p_ctrl, u16 msg,
							 u32 para1, u32 para2)
{
	OS_PRINTF("\n ui_desktop do age limit recheck \n");
	ui_age_limit_recheck(para1, para2);

	return SUCCESS;
}
#endif
#ifdef IMPL_NEW_EPG
static RET_CODE on_epg_request_table_policy_switch(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
   if(fw_get_focus_id() != ROOT_ID_VEPG)
    {
      OS_PRINTF("on_epg_request_table_policy_switch\n");
      ui_epg_init();
      ui_epg_start(EPG_TABLE_SELECTE_PF_ALL_SCH_ACTUAL);
    }
    return SUCCESS;
}
#endif


extern void gui_rolling(void);
void ui_desktop_rhythm(void)
{
	mtos_printk("%s, %d\n", __FUNCTION__, __LINE__);
	gui_rolling();
}

#if ENABLE_TEST_SPEED
#define DO_ACTIVE_STB_TASK_STACK_SIZE (64*KBYTES)

void  ui_do_active_stb_init(void)
{
    char *p_stack_buf = NULL;
    DO_ACTIVE_TASK_CONFIG_T param = {0};
    param.priority = VOD_DP_PRIORITY;
    param.http_priority = ONMOV_PRIORITY;
    param.stack_size = DO_ACTIVE_STB_TASK_STACK_SIZE;

    p_stack_buf = (char *)mtos_malloc(param.stack_size);
    MT_ASSERT(p_stack_buf != NULL);
    memset(p_stack_buf, 0, param.stack_size);

    param.p_mem_start = p_stack_buf;
    param.dp = BSW_IPTV_DP;
    param.sub_dp = SUB_DP_ID;
    param.id = mtos_malloc(MAC_CHIPID_LEN);
    param.encrypt_flag = 0;
    param.mac = mtos_malloc(MAC_CHIPID_LEN);

    ui_get_stb_mac_addr_chip_id(param.mac, param.id);

#ifndef WIN32
    active_the_stb_box(&param);
#endif

    mtos_free(param.id);
    mtos_free(param.mac);

    return;
}
#endif

static void close_fast_logo(void)
{
  static BOOL closed = FALSE;

  OS_PRINTF("%s\n",__FUNCTION__);
  if(!closed)
  {
    void  *p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
      SYS_DEV_TYPE_DISPLAY);

    disp_layer_show(p_dev, DISP_LAYER_ID_STILL_HD, FALSE);
    closed = TRUE;
  }
}

void ui_desktop_init(void)
{
	fw_config_t config =
	{
		{ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT },                /* background */

		DST_IDLE_TMOUT,                                       /* timeout of idle */
		RECEIVE_MSG_TMOUT,                                    /* timeout of recieving msg */
		POST_MSG_TMOUT,                                       /* timeout of sending msg */

		ROOT_ID_BACKGROUND,                                   /* background root_id */
		MAX_ROOT_CONT_CNT,
		MAX_MESSAGE_CNT,
		MAX_HOST_CNT,
		MAX_TMR_CNT,

		RSI_TRANSPARENT,
	};

	fw_init(&config,
		ui_desktop_keymap_on_normal,
		ui_desktop_proc_on_normal,
		ui_menu_manage);

  //al_netmedia_register_msg();
	//fw_set_rhythm(ui_desktop_rhythm);

	manage_init();

	// set callback func
	avc_install_screen_notification_1(class_get_handle_by_id(AVC_CLASS_ID),
		reset_screen);

    fw_register_ap_evtmap(APP_FRAMEWORK, sys_evtmap);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_JUMP);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_DVR_CONFIG);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_STORAGE_FORMAT);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_HDD_INFO);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_USB_SETTING);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_PIC_PLAY_MODE_SET);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_PICTURE);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_PVR_REC_BAR);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_PVR_PLAY_BAR);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_RECORD_MANAGER);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_USB_MUSIC_FULLSCREEN);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_USB_PICTURE);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_USB_MUSIC);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_USB_FILEPLAY);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_FILEPLAY_BAR);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_FILEPLAY_SUBT);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_TIMESHIFT_BAR);
    //fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_SUBMENU);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_MAINMENU);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_BACKGROUND);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_PING_TEST);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_NETWORK_CONFIG_LAN);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_NETWORK_CONFIG_WIFI);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_NETWORK_CONFIG_GPRS);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_NETWORK_CONFIG_3G);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_WIFI);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_WIFI_LINK_INFO);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_PHOTO_SHOW);
    fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_FACTORY_TEST);
    g_desktop.p_desktop = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	
    Desktop_OpenStateTree();

    #if ENABLE_TEST_SPEED
    ui_video_c_create(VIDEO_PLAY_FUNC_ONLINE);

    ui_do_active_stb_init();
    #endif
}


void ui_desktop_release(void)
{
	fw_release();
	// set callback func
	avc_install_screen_notification_1(class_get_handle_by_id(AVC_CLASS_ID), NULL);
}


void ui_desktop_main(void)
{
	fw_default_mainwin_loop(ROOT_ID_BACKGROUND);
}

void ui_desktop_search(void)
{
#ifdef  SUPPORT_POWER_FULL_SEARCH
	ui_do_search_full_search();
#else
#ifdef DTMB_PROJECT
	manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DTMB_AUTO, 0);
#else
	ui_scan_param_t scan_param;
	dvbc_lock_t tp = {0};

	memset(&scan_param, 0, sizeof(ui_scan_param_t));
	sys_status_get_main_tp1(&tp);

	scan_param.tp.freq = tp.tp_freq;
	scan_param.tp.sym = tp.tp_sym;
	switch(tp.nim_modulate)
	{
		case 0:
			scan_param.tp.nim_modulate =NIM_MODULA_AUTO;
			break;
		case 1:
			scan_param.tp.nim_modulate = NIM_MODULA_BPSK;      
			break;
		case 2:
			scan_param.tp.nim_modulate = NIM_MODULA_QPSK;      
			break;
		case 3:
			scan_param.tp.nim_modulate = NIM_MODULA_8PSK;      
			break;
		case 4:
			scan_param.tp.nim_modulate = NIM_MODULA_QAM16;
			break;
		case 5:
			scan_param.tp.nim_modulate = NIM_MODULA_QAM32;
			break;
		case 6:
			scan_param.tp.nim_modulate = NIM_MODULA_QAM64;      
			break;
		case 7:
			scan_param.tp.nim_modulate = NIM_MODULA_QAM128;
			break;
		case 8:
			scan_param.tp.nim_modulate = NIM_MODULA_QAM256;
			break;
		default:
			scan_param.tp.nim_modulate = NIM_MODULA_QAM64;
			break;
	}
	#ifdef AISET_BOUQUET_SUPPORT
	scan_param.nit_type = NIT_SCAN_ALL_TP;
	#else
	scan_param.nit_type = NIT_SCAN_ONCE;
	#endif
	manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DVBC_AUTO, (u32)&scan_param);
#endif
#endif
}

extern RET_CODE vdec_set_freeze_mem(void *p_dev, u32 addr, u32 size);
 void ui_desktop_start(void)
{
  u8 type = 0;
  u16 pos = 0;
  u32 context = 0;
  u8 curn_mode = CURN_MODE_NONE, vid = DB_DVBS_INVALID_VIEW;
  u8 curn_type = 3;
  u16 curn_group = GROUP_T_ALL;
  u16 view_count;
  gprs_info_t gprs_info = {{0}, };
  net_config_t net_config = {0,};

  sys_status_check_group();
  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();
  curn_type = sys_status_get_group_curn_type();

  sys_status_get_group_info(curn_group, &type, &pos, &context);

  if(curn_mode != CURN_MODE_NONE)
  {
    switch(type)
    {
      case GROUP_T_ALL:
        if(0 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_ALL_TV_FTA : DB_DVBS_ALL_RADIO_FTA, context, NULL);
        }
        else if(1 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_ALL_TV_CAS : DB_DVBS_ALL_RADIO_CAS, context, NULL);
        }
        else if(2 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_ALL_TV_HD : DB_DVBS_ALL_RADIO_HD, context, NULL);
        }
        else if(3 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_ALL_HIDE_TV : DB_DVBS_ALL_HIDE_RADIO, context, NULL);
        }
      break;
      case GROUP_T_FAV:
        if(0 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_FAV_TV_FTA : DB_DVBS_FAV_RADIO_FTA, context, NULL);
        }
        else if(1 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_FAV_TV_CAS : DB_DVBS_FAV_RADIO_CAS, context, NULL);
        }
        else if(2 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_FAV_TV_HD : DB_DVBS_FAV_RADIO_HD, context, NULL);
        }
        else if(3 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_FAV_HIDE_TV : DB_DVBS_FAV_HIDE_RADIO, context, NULL);
        }
      break;
      case GROUP_T_SAT:
        if(0 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_SAT_ALL_TV_FTA : DB_DVBS_SAT_ALL_RADIO_FTA, context, NULL);
        }
        else if(1 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_SAT_ALL_TV_CAS : DB_DVBS_SAT_ALL_RADIO_CAS, context, NULL);
        }
        else if(2 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_SAT_ALL_TV_HD : DB_DVBS_SAT_ALL_RADIO_HD, context, NULL);
        }
        else if(3 == curn_type)
        {
          vid = ui_dbase_create_view((curn_mode == CURN_MODE_TV) ? DB_DVBS_SAT_ALL_HIDE_TV : DB_DVBS_SAT_ALL_HIDE_RADIO, context, NULL);
        }
      break;
      default:
        MT_ASSERT(0);
    }
    ui_dbase_set_pg_view_id(vid);
  }
#ifdef SLEEP_TIMER
  //create sleep timer.
  ui_sleep_timer_create();
#endif

  {
   // ui_iptv_init();
   // ui_iptv_request_server_info();


    if(ui_get_play_prg_type() == NET_PRG_TYPE)
    {
      ui_video_c_create(VIDEO_PLAY_FUNC_ONLINE);
    }
    else if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
    {
      ui_video_c_create(VIDEO_PLAY_FUNC_USB);
    }

    view_count = db_dvbs_get_count(vid);

    #if ENABLE_TEST_SPEED
    #else
     {
       u32 size = 0;
       u32 align = 0;
	void *p_video = (void *)dev_find_identifier(NULL
                           , DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
       MT_ASSERT(NULL != p_video);
       vdec_stop(p_video);

     #ifdef MIN_AV_64M
     #ifdef MIN_AV_SDONLY
       vdec_get_buf_requirement(p_video, VDEC_SDINPUT_ONLY, &size, &align);
       OS_PRINTF("VDEC_SDINPUT_ONLY vdec buffer size is : 0x%x\n",size);
       MT_ASSERT(VIDEO_FW_CFG_SIZE >= size);
       OS_PRINTF("VDEC_SDINPUT_ONLY :0x%x\n",VIDEO_FW_CFG_ADDR);
       vdec_set_buf(p_video, VDEC_SDINPUT_ONLY, VIDEO_FW_CFG_ADDR);
     #else
       vdec_get_buf_requirement(p_video, VDEC_BUFFER_AD_UNUSEPRESCALE, &size, &align);
       OS_PRINTF("VDEC_BUFFER_AD_UNUSEPRESCALE vdec buffer size is : 0x%x\n",size);
       MT_ASSERT(VIDEO_FW_CFG_SIZE >= size);
       OS_PRINTF("VDEC_BUFFER_AD_UNUSEPRESCALE :0x%x\n",VIDEO_FW_CFG_ADDR);
       vdec_set_buf(p_video, VDEC_BUFFER_AD_UNUSEPRESCALE, VIDEO_FW_CFG_ADDR);
     #endif
     #else
       vdec_get_buf_requirement(p_video, VDEC_QAULITY_AD_128M, &size, &align);
       OS_PRINTF("VDEC_QAULITY_AD_128M vdec buffer size is : 0x%x\n",size);
       MT_ASSERT(VIDEO_FW_CFG_SIZE >= size);
       OS_PRINTF("VIDEO_FW_CFG_ADDR :0x%x\n",VIDEO_FW_CFG_ADDR);
       vdec_set_buf(p_video, VDEC_QAULITY_AD_128M, VIDEO_FW_CFG_ADDR);
     #endif
	 
	 #ifndef WIN32
       vdec_do_avsync_cmd(p_video,AVSYNC_NO_PAUSE_SYNC_CMD,0);
	 #endif
    }
    if((curn_mode == CURN_MODE_NONE ) ||(view_count == 0) || (view_count == 0xFFFF))
    {
      if(FALSE == g_deskto_first_enter)
      {
		  ui_desktop_search();
		  g_deskto_first_enter = TRUE;
	  }
	  else
	  {
		manage_open_menu(ROOT_ID_MAINMENU, 0, 0);
	  }
    }
    else
    {
      g_deskto_first_enter = TRUE;
      if(LATEST_NODE_START != book_check_latest_node(book_get_latest_index(), FALSE))
      {
        manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
      }
	  #ifdef SHUSHITONG_CA
	  if(CUSTOMER_ID == CUSTOMER_DTMB_CHANGSHA_HHT)
	  {
		update_have_valid_freq_flag();
		fw_tmr_create(ROOT_ID_BACKGROUND, MSG_TIMER_CARD_OVERDUE, 1000, TRUE);
	  }
	  #endif
    }
    #endif
  }

  //auto connect gprs
  sys_status_get_net_config_info(&net_config);


  if(net_config.link_type == LINK_TYPE_GPRS)
  {
     memcpy(&gprs_info, sys_status_get_gprs_info(), sizeof(gprs_info_t));
     do_cmd_connect_gprs(&gprs_info);
  }

#ifdef   NC_AUTH_EN
{
    u8 mac[6] = {0x0C,0xF0,0xB4,0x06,0x74,0xE5};
    NC_AuthInit("87079770", mac, 6);
    NC_AuthStart();
}
#endif
  g_deskto_init = TRUE;
 {
	extern int ui_aiqiyi_entry();
  	ui_aiqiyi_entry();
 }

#ifdef ENABLE_CA
  #ifndef DESAI_52_CA
  ui_ca_do_cmd(CAS_CMD_MAIL_HEADER_GET, 0 ,0);
  #endif
#endif

#ifdef ADS_DIVI_SUPPORT
	fw_tmr_create(ROOT_ID_BACKGROUND, MSG_ADS_TIME, 1000, TRUE);
#endif

}


void getChannelList()
{
    mtos_printk("IQY_ChannelList size = %lu \r\n", sizeof(IQY_ChannelList));

    ////////////////////////////////////////
    //get all the channel
    ////////////////////////////////////////
    IQY_ChannelList* pChnlLst = mtos_malloc(sizeof(IQY_ChannelList));
    if(pChnlLst != NULL)
    {
        memset(pChnlLst, 0, sizeof(IQY_ChannelList));
        char version[20] = "1.0.1.0";
        iqy_getChannelList(version, 1, 5, pChnlLst);

        IQY_ChannelLabelList* pLabelList = mtos_malloc(sizeof(IQY_ChannelLabelList));
        memset(pLabelList, 0, sizeof(IQY_ChannelLabelList));
        iqy_getChannelLabelList(0, pChnlLst->data[0].recTag, pLabelList);

        IQY_PlayList* pAlbumList = mtos_malloc(sizeof(IQY_AlbumList));

        memset(pAlbumList, 0, sizeof(IQY_AlbumList));
        iqy_getPlayListById(0, pLabelList->labelList[0].plid, TYPE_PLAYLIST, pAlbumList);

        memset(pAlbumList, 0, sizeof(IQY_AlbumList));
        iqy_getPlayListById(0, pChnlLst->data[0].focus, TYPE_RECOMMAND, pAlbumList);


        mtos_free(pChnlLst);
        mtos_free(pAlbumList);
        mtos_free(pLabelList);

    }
}

void getAlbumList()
{
    mtos_printk("IQY_ChannelList size = %lu \r\n", sizeof(IQY_ChannelList));

    IQY_ChannelList* pChnlLst = mtos_malloc(sizeof(IQY_ChannelList));
    memset(pChnlLst, 0, sizeof(IQY_ChannelList));
    char version[20] = "1.0.1.0";
    iqy_getChannelList(version, 1, 5, pChnlLst);


    ////////////////////////////////////////
    //put all the channel's 10 album list
    ////////////////////////////////////////
    IQY_AlbumList* pAlbumList = mtos_malloc(sizeof(IQY_AlbumList));
    //char tagvalues[50] = { 0 };
    int pageNo = 1;
    int pageSize = 5;
    int i = 1;
//    for( i = 0; i < pChnlLst->size; i++ )
    {
        memset(pAlbumList, 0, sizeof(IQY_AlbumList));
        //mtos_printk(" **************** subtag is %s \r\n", pChnlLst->data[i].tags[1].subtag[2].value);
        mtos_printk(" **************** subtag is %s \r\n", pChnlLst->data[i].tags[4].subtag[2].value);
        iqy_getAlbumList(pChnlLst->data[i].channelId, -1, pChnlLst->data[i].tags[4].subtag[2].value, pageNo, pageSize, pAlbumList);

    #if 0//OUTPUT_ALBUM_INFO
        ////////////////////////////////////////
        //get all the channel's album's info
        ////////////////////////////////////////
        IQY_AlbumInfo* pAlbumInfo = mtos_malloc(sizeof(IQY_AlbumInfo));
        int j = 0;
        for( j = 0; j < pAlbumList->size; j++ )
        {
            memset(pAlbumInfo, 0, sizeof(IQY_AlbumInfo));
            mtos_printk("\r\n*****************************************\r\n");
            mtos_printk("pAlbumList->data[j].qpId = %s\r\n", pAlbumList->data[j].qpId);
            iqy_getAlbumInfo(pAlbumList->data[j].qpId, pAlbumInfo);
        }
        if(pAlbumInfo != NULL)
        {
            mtos_free(pAlbumInfo);
        }
    #endif // OUTPUT_ALBUM_INFO
    }

    if(pChnlLst != NULL)
    {
        mtos_free(pChnlLst);
    }

    if(pAlbumList != NULL)
    {
        mtos_free(pAlbumList);
    }
}


void realtimeSearch()
{
    mtos_printk("IQY_SearchWord size = %lu \r\n", sizeof(IQY_SearchWord));

    ////////////////////////////////////////
    //search real time word
    ////////////////////////////////////////
    char keyword[20] = "sdyjq";
    IQY_SearchWord* pSearchWord = mtos_malloc(sizeof(IQY_SearchWord));
    memset(pSearchWord, 0, sizeof(IQY_SearchWord));
    iqy_getRealTimeSearchWord(keyword, pSearchWord);

    if(pSearchWord != NULL)
    {
        mtos_free(pSearchWord);
    }
}

#if 0
void searchHotwords()
{
    mtos_printk("IQY_SearchWord size = %lu \r\n", sizeof(IQY_SearchWord));

    ////////////////////////////////////////
    //get search hot word
    ////////////////////////////////////////
    IQY_SearchWord* pSearchWord = mtos_malloc(sizeof(IQY_SearchWord));
    memset(pSearchWord, 0, sizeof(IQY_SearchWord));
    iqy_getSearchHotwords(pSearchWord);

 #if 1//OUTPUT_SEARCH_ALBUM
    ////////////////////////////////////////
    //search hot word
    ////////////////////////////////////////
    int pageNo = 1;
    int pageSize = 5;
    IQY_AlbumList* pAlbumList = mtos_malloc(sizeof(IQY_AlbumList));
    memset(pAlbumList, 0, sizeof(IQY_AlbumList));
    // 0: all channel
    iqy_albumSearch(pSearchWord->wordList[0].word, 0, pageNo, pageSize, pAlbumList);
    if(pAlbumList != NULL)
    {
        mtos_free(pAlbumList);
    }
 #endif // OUTPUT_SEARCH_ALBUM

    if(pSearchWord != NULL)
    {
        mtos_free(pSearchWord);
    }
}
#endif

void albumVideo()
{
    IQY_AlbumList* pAlbumList = mtos_malloc(sizeof(IQY_AlbumList));
    memset(pAlbumList, 0, sizeof(IQY_AlbumList));

    int pageNo = 1;
    int pageSize = 5;

    IQY_SearchWord* pSearchWord = mtos_malloc(sizeof(IQY_SearchWord));
    memset(pSearchWord, 0, sizeof(IQY_SearchWord));

    IQY_EpisodeList* pEpisodeList = mtos_malloc(sizeof(IQY_EpisodeList));
    memset(pEpisodeList, 0, sizeof(IQY_EpisodeList));

    ////////////////////////////////////////
    //get album video
    ////////////////////////////////////////

    if(0)
    {
        char keyword[20] = "sbtj";
        iqy_getRealTimeSearchWord(keyword, pSearchWord);

        iqy_albumSearch(pSearchWord->wordList[3].word, 0, pageNo, pageSize, pAlbumList);

        int videoType = 0;

        iqy_getAlbumVideo(pAlbumList->data[0].qpId, pAlbumList->data[0].sourceCode, videoType, pageNo, pageSize, pEpisodeList);
    }
    else if(1)
    {
        char keyword[20] = "bxjg";
        iqy_getRealTimeSearchWord(keyword, pSearchWord);

        int i = 0;
        for( i = 0; i < pSearchWord->wordSize; i++ )
        {
            char* word = pSearchWord->wordList[i].word;

            mtos_printk("+++++++++chinese word is %s\r\n", word);

        }
        iqy_albumSearch(pSearchWord->wordList[0].word, 1, pageNo, pageSize, pAlbumList);

//        int videoType = 0;

//        iqy_getAlbumVideo(pAlbumList->data[0].qpId, pAlbumList->data[0].sourceCode, videoType, pageNo, pageSize, pEpisodeList);

    }

    if(pAlbumList != NULL)
    {
        mtos_free(pAlbumList);
    }

    if(pSearchWord != NULL)
    {
        mtos_free(pSearchWord);
    }

    if(pEpisodeList != NULL)
    {
        mtos_free(pEpisodeList);
    }
}


void getAllChannelAlbumList()
{
    IQY_AlbumList* pAlbumList = mtos_malloc(sizeof(IQY_AlbumList));
    memset(pAlbumList, 0, sizeof(IQY_AlbumList));
    char tagvalues[50] = { 0 };
    int pageNo = 1;
    int pageSize = 5;

    ////////////////////////////////////////
    //get all the channel's 10 album list
    ////////////////////////////////////////
    iqy_getAlbumList(1, 1, tagvalues, 0, 0, pAlbumList);
    int total = pAlbumList->total;
    for( ; pageNo * pageSize < total; pageNo++ )
    {
        iqy_getAlbumList(1, 1, tagvalues, pageNo, pageSize, pAlbumList);
    }

    if(pAlbumList != NULL)
    {
        mtos_free(pAlbumList);
    }
}

#if 0
void getDataRcarousel()
{
    IQY_AlbumList* pAlbumList = mtos_malloc(sizeof(IQY_AlbumList));
    memset(pAlbumList, 0, sizeof(IQY_AlbumList));

    iqy_getDataRcarousel(1, pAlbumList);

    if(pAlbumList != NULL)
    {
        mtos_free(pAlbumList);
    }
}
#endif

void errorCallback(int errorCode)
{
     mtos_printk("!!!!!!errorCode %d\r\n", errorCode);
}

void testPlay()
{

    ////////////////////////////////////////
    //put all the channel's 10 album list
    ////////////////////////////////////////
    IQY_AlbumList* pAlbumList = mtos_malloc(sizeof(IQY_AlbumList));
    char tagvalues[50] = { 0 };
    int pageNo = 1;
    int pageSize = 10;

    memset(pAlbumList, 0, sizeof(IQY_AlbumList));
    iqy_getAlbumList(1, -1, tagvalues, pageNo, pageSize, pAlbumList);

mtos_printk("+++++++++pAlbumList->data[0].tvQid is %s\r\n", pAlbumList->data[1].tvQid);
mtos_printk("+++++++++pAlbumList->data[0].vid is %s\r\n", pAlbumList->data[1].vid);


    IQY_PlaySetup* setup = mtos_malloc(sizeof(IQY_PlaySetup));
    strcpy(setup->tvQid, pAlbumList->data[ pAlbumList->size - 1 ].tvQid);
    strcpy(setup->vid, pAlbumList->data[ pAlbumList->size - 1 ].vid);
    setup->error = (void*)errorCallback;

    mtos_printk("sleep 1 \r\n");
    mtos_task_sleep(1000);

    iqy_start(setup);

    mtos_printk("sleep 5 \r\n");
    mtos_task_sleep(50000);

    iqy_pause();

    mtos_printk("sleep 5 \r\n");
    mtos_task_sleep(5000);

    iqy_stop();

    mtos_printk("sleep 5 \r\n");
    mtos_task_sleep(5000);


    if(pAlbumList != NULL)
    {
        mtos_free(pAlbumList);
    }

    if(setup != NULL)
    {
        mtos_free(setup);
    }
}

void init()
{
#define   PULL_DATA_THREAD_STACK_LEN  (128*1024)
#define   PRELOAD_DATA_THREAD_STACK_LEN  (16)
#define   PRELOAD_AUDIO_BUF_SIZE    (16)
#define   PRELOAD_VIDEO_BUF_SIZE     (16)
    int ret = 0;

	static IQY_Init 				    init;
	static PB_SEQ_PARAM_T 	            PB_cfg;

    PB_cfg.pb_seq_mem_size  		    = 0;
    PB_cfg.pb_seq_mem_start 		    = 0;
    PB_cfg.stack_size       			= PULL_DATA_THREAD_STACK_LEN;
    PB_cfg.task_priority    			= FILE_PLAYBACK_PRIORITY;
    PB_cfg.stack_preload_size       	= PRELOAD_DATA_THREAD_STACK_LEN;
    PB_cfg.preload_audio_buffer_size 	= PRELOAD_AUDIO_BUF_SIZE;
    PB_cfg.preload_video_buffer_size 	= PRELOAD_VIDEO_BUF_SIZE;

	init.mode = 0;
	init.playerInit.size = sizeof(PB_cfg);
	init.playerInit.cfg = (void*)&PB_cfg;
	ret = iqy_libInit(&init);
	if (ret != IQY_NORMAL){
		OS_PRINTF(" iqy lib init failure ret = %d\n",ret);
		return;
	}

}
extern BOOL xmian_check_network_stat(void);
unsigned int porting_gen_rand(void);
unsigned long porting_gen_systime_ms(void);
void show_tasks_stk_info(void);
extern  int iqy_set_system_time(void);
extern int iqy_lib_init(void);
unsigned long porting_gen_systime_utc_time(void);
void time_set_have_got_time_from_net_flag(void);

#ifdef DVB_CA_SET_ECM_TIME
void time_set_have_got_time_from_ca_flag(void);
BOOL time_have_got_time_from_other_flag(void);
BOOL time_get_if_got_time_flag(void);
BOOL ca_set_system_time(void);
#endif

static void aiqiyi(void *param)
{
	int index = 0;
	mem_user_dbg_info_t	 m;
	int mem_debug_div = 60;
	int net_time_set_div = 60;
	#ifdef DVB_CA_SET_ECM_TIME
	int ca_time_set_div = 60;
	#endif
	int get_chip_id = 0;

	//  unsigned int rand_v = 0;
	//   unsigned long ms = 0;
#ifdef SUPPORT_TIME_REBOOT
	utc_time_t * loc_time = NULL;
	loc_time = (utc_time_t*)mtos_malloc(sizeof(utc_time_t));
	BOOL bret = FALSE;
#endif
	while(1)
	{
		//show_tasks_stk_info();
		if (get_chip_id)
		{
			u32 h,l;
			hal_get_chip_unique_numb(&h, &l);
			OS_PRINTF("chip id[h:0x%08x,L0x%08x]\n",h,l);
		}
		if (index%mem_debug_div == 0)
		{
			mtos_mem_user_debug(&m);
			OS_PRINTF("run aiqiyi!!alloced[%#x], alloced_peak[%#x],rest_size[%#x]\r\n",m.alloced,m.alloced_peak,m.rest_size);
		}
		if(index%net_time_set_div==0  && xmian_check_network_stat() == TRUE)
		{
			static u8 have_inited=0;
			if (have_inited == 0)
				iqy_lib_init();
			have_inited = 1;
			if (iqy_set_system_time() == 0)
				time_set_have_got_time_from_net_flag();
			
			net_time_set_div = 60*60;
			OS_PRINTF("[%s] %d  iqy_set_system_time\n",__FUNCTION__,__LINE__);
		}

		#ifdef DVB_CA_SET_ECM_TIME
		if(index%ca_time_set_div==0  && (time_have_got_time_from_other_flag() == FALSE))
		{
			if(ca_set_system_time())
				time_set_have_got_time_from_ca_flag();
		}
		#endif

		#ifdef SUPPORT_TIME_REBOOT
		extern BOOL hal_pm_reset(void);
		memset(loc_time,0x00,sizeof(utc_time_t));
		if((index)%30==0 && index != 0)
		{
			bret = time_get(loc_time, FALSE);
			if(bret != FALSE)
			{
				if((loc_time->hour) == 2 && loc_time->minute == 0)
				{
					hal_pm_reset();
				}
			}
		}	
		#endif
		index++;
		mtos_task_sleep(1000);
	}
}


int ui_aiqiyi_entry()
{
	#define AIQIYI_STATIC  (64*1024)
	u32 * stack  = mtos_malloc(AIQIYI_STATIC);

	// init
	mtos_task_create("aiqiyimain",
			aiqiyi,
			NULL,
			TASK_AIQIYI_MODULE,
			stack,
			AIQIYI_STATIC);

	return 0;	
}



BOOL is_desktop_init(void)
{
	return g_deskto_init;
}

static RET_CODE on_ota_info_find(control_t *cont, u16 msg, u32 para1, u32 para2)
{
	u8 focus_root_id = 0;

	OS_PRINTF("\r\n[OTA]%s",__FUNCTION__);

	focus_root_id = fw_get_focus_id();

	if((ROOT_ID_DO_SEARCH == focus_root_id)
		||(ROOT_ID_TP_SEARCH == focus_root_id)
		||(! do_search_is_finish())
		||(ROOT_ID_OTA_SEARCH == focus_root_id)
		||(ROOT_ID_OTA == focus_root_id)
		||(ROOT_ID_DUMP_BY_USB == focus_root_id)
		||(ROOT_ID_PVR_REC_BAR == focus_root_id)
		||(ROOT_ID_USB_MUSIC == focus_root_id))
	{
		OS_PRINTF("\r\n[OTA] desktop don't process, focus_root_id[%d]",focus_root_id);
		return ERR_FAILURE;
	}

	ui_comm_cfmdlg_open(NULL, IDS_MSG_ASK_FOR_UPGRADE, NULL, 10000);

	OS_PRINTF("\r\n[OTA]%s: reboot stb for upgrade! ",__FUNCTION__);
#ifndef WIN32
	hal_pm_reset();
#endif
	return SUCCESS;
}

 static RET_CODE on_ota_timeout_start_play(control_t *p_ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  OS_PRINTF("###debug ota timeout,it will play\n");
#ifdef NIT_SOFTWARE_UPDATE
  if(g_nit_ota_check == TRUE)
  {
    comm_dlg_data_t timeout_data =
    {
      ROOT_ID_INVALID,
      DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
      COMM_DLG_X, COMM_DLG_Y,
      COMM_DLG_W, COMM_DLG_H,
      IDS_OTA_TIMEOUT,
      0,
    };
    ui_comm_dlg_open(&timeout_data);
    g_nit_ota_check = FALSE;
  }
#endif
  if(g_deskto_init == TRUE)
  {
    return SUCCESS;
  }
  DEBUG(DBG, INFO, "usb_monitor_attach\n");
  usb_monitor_attach();
#if ENABLE_NETWORK
#if ENABLE_ETH
  eth_monitor_attach();
#endif
#ifndef WIN32
#if ENABLE_USB_ETH
  usb_eth_monitor_attach();
#endif
#endif
  wifi_monitor_attach();
  gprs_monitor_attach();
  g3_monitor_attach();
#endif

  ui_enable_playback(TRUE);

  fw_set_keymap(ui_desktop_keymap_on_normal);

  /*close logo*/
  close_fast_logo();
  // restore the setting
  sys_status_reload_environment_setting();
#ifdef TEMP_SUPPORT_DS_AD
  ui_close_ad_logo();
#endif
  if(fw_get_focus_id() != ROOT_ID_OTA_SEARCH)
  {
      OS_PRINTF("###debug ota timeout,it will play2\n");
      ui_ota_api_stop();
#ifdef ENABLE_CA
#ifndef WIN32
      if(SUCCESS != cas_manager_ca_area_limit_check_start())
#endif
#endif
      {
        ui_desktop_start();
      }
    }

  return SUCCESS;
}

static void ui_ota_upg_reset(void)
{
#ifndef WIN32
    OS_PRINTF("###debug ota restar\n");
    ui_ota_api_stop();
    mtos_task_delay_ms(100);
    hal_pm_reset();
#endif
}

#if 0
static void ui_ota_upg_cancel(void)
{
  ota_bl_info_t bl_info = {0};

  mul_ota_dm_api_read_bootload_info(&bl_info);

  OS_PRINTF("\r\n[OTA]%s:upg ota_tri[%d] ",__FUNCTION__, bl_info.ota_status);

  bl_info.ota_status = OTA_TRI_MODE_NONE;
  mul_ota_dm_api_save_bootload_info(&bl_info);

  ui_ota_api_auto_check_cancel();
}
#endif
static RET_CODE on_ota_reboot_into_ota(control_t *p_ctrl, u16 msg,
                              u32 para1, u32 para2)
{
  OS_PRINTF("###debug ota restart %d\n", fw_get_focus_id());
#ifdef NIT_SOFTWARE_UPDATE
  if(g_nit_ota_check == TRUE)
  {
    #ifdef WIN32
    ui_ota_api_stop();
    OS_PRINTF("OTA check over!please run ota app\n");
    MT_ASSERT(0);
    #else
    ui_ota_api_manual_save_ota_info();
    ui_ota_upg_reset();
    #endif
    g_nit_ota_check = FALSE;
  }
#endif
  if(g_deskto_init == TRUE)
  {
    return SUCCESS;
  }
  ui_enable_uio(TRUE);
  if(fw_get_focus_id() != ROOT_ID_OTA_SEARCH)
  {
    if(TRUE == ui_ota_api_get_maincode_ota_flag())
      {
        ui_ota_api_maincode_ota_start();
      }
    else
      {
          #ifdef WIN32
          ui_ota_api_stop();
          //on_ota_timeout_start_play(p_ctrl,msg,para1,para2);
          OS_PRINTF("OTA check over!please run ota app\n");
          MT_ASSERT(0);
          #else
          ui_ota_upg_reset();
          #endif
      }
  }

  return SUCCESS;
}

static RET_CODE on_ota_in_maincode_finsh(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
   if(fw_get_focus_id() != ROOT_ID_OTA_SEARCH)
  {
    #ifdef WIN32
    on_ota_timeout_start_play(p_ctrl,msg,para1,para2);
    #else
    ui_ota_upg_reset();
    #endif
  }
   return SUCCESS;
}

static RET_CODE on_heart_beat(control_t *p_ctrl, u16 msg,
                              u32 para1, u32 para2)
{
//if(ui_get_usb_status())
  {
	//mtos_printk("%s, %d\n", __FUNCTION__, __LINE__);
     gui_rolling();
  }
  return SUCCESS;
}

static RET_CODE on_net_ip_mount(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  u32 is_mont_success = para1;
  char ipaddress[128] = "";

  ui_get_ip(ipaddress);

  if(is_mont_success == 1)
  {
    OS_PRINTF("lou it  connect ip success\n");
    manage_open_menu(ROOT_ID_USB_FILEPLAY,1 ,(u32)(ipaddress));
  }
 else
  {
     ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_CONTENT, NULL, 0);
  }

  return SUCCESS;
}

static RET_CODE on_net_mount_success(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  char ipaddress[128] = {0};

  ui_get_ip(ipaddress);

  OS_PRINTF("lou it  connect ip success\n");
  manage_open_menu(ROOT_ID_USB_FILEPLAY,1 ,(u32)(ipaddress));
  return SUCCESS;
}

static RET_CODE on_net_mount_fail(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_CONTENT, NULL, 0);
  return SUCCESS;
}

#if 0
static RET_CODE on_desktop_recieve_satip_client_msg(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
    BOOL ret = FALSE;
    dvbs_prog_node_t pg_node = {0,};
    u16 pg_id = para1;
    OS_PRINTF("##%s, line[%d], prog_id ==[%d]##\n", __FUNCTION__, __LINE__,pg_id);
    if(ui_get_dlna_status(SATIP) != DLNA_ON)//to fixed bug 51380
    {
        OS_PRINTF("##%s, line[%d], dlna is not closing ok##\n", __FUNCTION__, __LINE__);
        return SUCCESS;
    }

    ret = SatIp_Client_Req_Pg_Is_Exist(pg_id);

    ReturnCheck(ret, SUCCESS, FALSE);

    if(pg_id == sys_status_get_curn_group_curn_prog_id() && ui_is_playing())
    {
        db_dvbs_get_pg_by_id(pg_id, &pg_node);
        OS_PRINTF("##%s, line[%d], is_scrambled ==[%d]##\n", __FUNCTION__, __LINE__,pg_node.is_scrambled);
        ret = pg_node.is_scrambled;
        ReturnCheck(ret, SUCCESS, FALSE);
    }

    if(SatIp_Client_Check_ReqTp_And_PlayTp(pg_id) == TRUE)
    {
        db_dvbs_get_pg_by_id(pg_id, &pg_node);
        OS_PRINTF("##%s, line[%d], is_scrambled ==[%d]##\n", __FUNCTION__, __LINE__,pg_node.is_scrambled);
        ret = pg_node.is_scrambled;
        ReturnCheck(ret, SUCCESS, FALSE);
    }

    ret = SatIp_Client_Post_Change_Pg();
    OS_PRINTF("At [%s] in Line %d\n", __FUNCTION__, __LINE__);
    ReturnCheck(ret, SUCCESS, FALSE);

    ui_desktop_close_special_menus_before_play_satip_prog();
    ui_close_all_mennus();

    if(NULL != fw_find_root_by_id(ROOT_ID_PROG_BAR))
    {
        manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
    }

    OS_PRINTF("At [%s] in Line %d\n", __FUNCTION__, __LINE__);
    manage_open_menu(ROOT_ID_PROG_BAR, V_KEY_UNKNOW, pg_id);

    return SUCCESS;
}

#endif
void ui_evt_disable_ir()
{
  cmd_t cmd = {0};
  OS_PRINTF("%s\n", __FUNCTION__);
  cmd.id = AP_UIO_CMD_DISABLE_IR;
  cmd.data1 = 0;
  cmd.data2 = 0;
  ap_frm_do_command(APP_UIO, &cmd);
}
void ui_evt_enable_ir()
{
  cmd_t cmd = {0};
  OS_PRINTF("%s\n", __FUNCTION__);
  cmd.id = AP_UIO_CMD_ENABLE_IR;
  cmd.data1 = 0;
  cmd.data2 = 0;
  ap_frm_do_command(APP_UIO, &cmd);
}
void ui_clean_all_menus_do_nothing(void)
{
    /* close all menu */
    manage_enable_autoswitch(FALSE);
    fw_destroy_all_mainwin(TRUE);
    manage_enable_autoswitch(TRUE);
}


static RET_CODE on_desktop_load_media_success(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{
  u8 focus = fw_get_focus_id();
  if(ROOT_ID_BACKGROUND!= focus)
    {
      return SUCCESS;
    }
 if( fw_find_root_by_id(ROOT_ID_SMALL_LIST) != NULL)
  return SUCCESS;
 if( fw_find_root_by_id(ROOT_ID_PROG_BAR) != NULL)
  return SUCCESS;
  ui_comm_dlg_close();

  return SUCCESS;
}

static RET_CODE on_desktop_load_media_error(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{
   return SUCCESS;
}

static RET_CODE on_desktop_load_media_exit(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{
   return SUCCESS;
}

void ui_get_stb_mac_addr_chip_id(u8 *p_mac, u8 *p_id)
{
  u8 temp_mac[32] = {0};
  u8 temp_id[32] = {0};
  unsigned char hwaddr[6];
  ethernet_device_t * eth_dev = NULL;
  net_conn_stats_t eth_connt_stats = ui_get_net_connect_status();

  if(eth_connt_stats.is_usb_eth_conn)
  {
    eth_dev = (ethernet_device_t *)ui_get_usb_eth_dev();
	if(NULL == eth_dev)
		return;
    dev_io_ctrl(eth_dev, GET_ETH_HW_MAC_ADDRESS, (u32)&hwaddr[0]);
    sprintf((char *)temp_mac,"%.2x%.2x%.2x%.2x%.2x%.2x", hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);
  }
#ifndef WIN32
  else if(eth_connt_stats.is_wifi_conn)
  {
    eth_dev = (ethernet_device_t *)get_wifi_handle();
	if(NULL == eth_dev)
		return;
    dev_io_ctrl(eth_dev, GET_ETH_HW_MAC_ADDRESS, (u32)&hwaddr[0]);
    sprintf((char *)temp_mac,"%.2x%.2x%.2x%.2x%.2x%.2x", hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);
  }
#endif
  else
  {
      memcpy(temp_mac,sys_status_get_mac_string(),12);
  }

  temp_mac[12] = '\0';
  sprintf(temp_id, "%08d", (int)0);
  memcpy(p_mac, temp_mac, sizeof(temp_mac) - 1);
  memcpy(p_id, temp_id, sizeof(temp_id) - 1);
  OS_PRINTF("##%s, mac addr = [%s], sn num = [%s]##\n", __FUNCTION__, p_mac, p_id);
}

static void ui_desktop_notify_msg_to_win(u8 root_id, u16 msg)
{
    control_t *p_root = NULL;
    p_root = fw_find_root_by_id(root_id);
    if(p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, msg, 0, 0);
    }
}

BOOL ui_get_livetv_db_flag(void)
{
    return g_desktop.write_db_info_done;
}
void ui_set_livetv_db_flag(BOOL is_done)
{
    g_desktop.write_db_info_done = is_done;
}
u8 ui_get_init_livetv_status(void)
{
    return g_desktop.livetv_init;
}
void ui_set_init_livetv_status(u8 status)
{
    g_desktop.livetv_init = status;
}

static RET_CODE ui_desktop_on_state_process(control_t *p_ctrl, u16 msg, u32
para1, u32 para2)
{
    RET_CODE ret = ERR_NOFEATURE;
    ret = Desktop_DispatchMsg(p_ctrl, msg, para1, para2);
    if (ret != SUCCESS)
    {
        OS_PRINTF("####desktop unaccepted msg, id=0x%04x####\n", msg);
    }
    return ret;
}
static void SenDesktop_Disconnected(void)
{
    OS_PRINTF("####func:[%s]####\n", __FUNCTION__);
}
static void SexDesktop_Disconnected(void)
{
    OS_PRINTF("####func:[%s]####\n", __FUNCTION__);
}
static void SenDesktop_Connected(void)
{
    OS_PRINTF("####func:[%s]####\n", __FUNCTION__);
}
static void SexDesktop_Connected(void)
{
    OS_PRINTF("####func:[%s]####\n", __FUNCTION__);
}
static STATEID StaDesktop_desktop_on_connect_network(control_t *ctrl, u16 msg
, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_CONNECTED;
}
static STATEID StaDesktop_desktop_on_disconnect_network(control_t *ctrl, u16
msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s]####\n", __FUNCTION__);
    return SID_DESKTOP_DISCONNECTED;
}
static void SenDesktop_Initialize(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static void SexDesktop_Initialize(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}

static void SenDesktop_LivetvInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static void SexDesktop_LivetvInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static void SenDesktop_LivetvDpInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    if(ui_get_init_livetv_status() != LIVETV_INIT_SUCCESS &&
ui_get_init_livetv_status() != LIVETV_INITING)
    {
        ui_live_tv_init();
        ui_set_init_livetv_status(LIVETV_INITING);
    }
    else
    {
        fw_notify_root(g_desktop.p_desktop, NOTIFY_T_MSG, FALSE,
MSG_DESKTOP_JUMP_TO_NEXT_STATUS, 0, 0);
    }
}
static STATEID StaDesktop_livetv_init_on_init_success(control_t *ctrl, u16 msg
, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    ui_set_init_livetv_status(LIVETV_INIT_SUCCESS);
    return SID_DESKTOP_LIVETV_CATGRY_INIT;
}
static STATEID StaDesktop_livetv_init_on_init_fail(control_t *ctrl, u16 msg,
u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    ui_set_init_livetv_status(LIVETV_INIT_FAILED);
    return SID_DESKTOP_IPTV_INIT;
}
static STATEID StaDesktop_jump_to_desktop_livetv_categry_init(control_t *ctrl
, u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_LIVETV_CATGRY_INIT;
}
static void SexDesktop_LivetvDpInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static void SenDesktop_LivetvCatgryInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    if(g_desktop.category_init == FALSE)
    {
        ui_live_tv_get_categorylist();
    }
    else
    {
        if(g_desktop.up_livetv_timing == TRUE)
        {
            ui_live_tv_get_categorylist();
        }
        else
        {
            fw_notify_root(g_desktop.p_desktop, NOTIFY_T_MSG, FALSE,
MSG_DESKTOP_JUMP_TO_NEXT_STATUS, 0, 0);
        }
    }
}
static STATEID StaDesktop_livetv_init_on_catgry_arrived(control_t *ctrl, u16
msg, u32 para1, u32 para2)
{
    LIVE_TV_CATEGORY_LIST_T *p_group  = (LIVE_TV_CATEGORY_LIST_T *)para1;
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    if(p_group && p_group->status == TVLIVE_DATA_SUCCESS)
    {
        ui_live_tv_api_group_arrived(para1);
        g_desktop.category_init = TRUE;
        return SID_DESKTOP_LIVETV_CHAN_ZIP_INIT;
    }
    else
    {
        g_desktop.category_init = FALSE;
        g_desktop.up_livetv_timing = FALSE;
        return SID_DESKTOP_IPTV_INIT;
    }
}

static STATEID StaDesktop_jump_to_desktop_chan_zip_init(control_t *ctrl, u16
msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_LIVETV_CHAN_ZIP_INIT;
}

static void SexDesktop_LivetvCatgryInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static void SenDesktop_LivetvChanZipInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    if(g_desktop.chan_zip_init == FALSE)
    {
        ui_live_tv_api_all_chan_zip_load();
    }
    else
    {
        if(g_desktop.up_livetv_timing == TRUE)
        {
            ui_live_tv_api_all_chan_zip_load();
        }
        else
        {
            fw_notify_root(g_desktop.p_desktop, NOTIFY_T_MSG, FALSE,
MSG_DESKTOP_JUMP_TO_NEXT_STATUS, 0, 0);
        }
    }
}

static STATEID StaDesktop_livetv_init_on_chan_zip_arrived(control_t *ctrl,
u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    g_desktop.p_chan_zip = (LIVE_TV_CHANNEL_ZIP_T *)para1;
    if(g_desktop.p_chan_zip && ((LIVE_TV_CHANNEL_ZIP_T *)(g_desktop.p_chan_zip
))->status == TVLIVE_DATA_SUCCESS)
    {
        g_desktop.chan_zip_init = TRUE;
        return SID_DESKTOP_LIVETV_CHAN_LIST_INIT;
    }
    else
    {
        g_desktop.chan_zip_init = FALSE;
        g_desktop.up_livetv_timing = FALSE;
        return SID_DESKTOP_IPTV_INIT;
    }
}

static STATEID StaDesktop_jump_to_desktop_chan_list_init(control_t *ctrl, u16
msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_LIVETV_CHAN_LIST_INIT;
}

static void SexDesktop_LivetvChanZipInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}

static void SenDesktop_LivetvChanListInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    if(ui_get_livetv_db_flag() == FALSE)
    {
    #ifndef WIN32
        LiveTV_getChannelList(get_live_tv_handle(),\
            ((LIVE_TV_CHANNEL_ZIP_T *)(g_desktop.p_chan_zip))->zip_start_addr,\
                ((LIVE_TV_CHANNEL_ZIP_T *)(g_desktop.p_chan_zip))->zip_length);
    #endif
    }
    else
    {
        if(g_desktop.up_livetv_timing == TRUE)
        {
            #ifndef WIN32
            LiveTV_getChannelList(get_live_tv_handle(),\
                ((LIVE_TV_CHANNEL_ZIP_T *)(g_desktop.p_chan_zip))->zip_start_addr,\
                    ((LIVE_TV_CHANNEL_ZIP_T *)(g_desktop.p_chan_zip))->zip_length);
            #endif
        }
        else
        {
            fw_notify_root(g_desktop.p_desktop, NOTIFY_T_MSG, FALSE,
MSG_DESKTOP_JUMP_TO_NEXT_STATUS, 0, 0);
        }
    }
}
static STATEID StaDesktop_livetv_init_on_livetv_all_chan_arrived(control_t *
ctrl, u16 msg, u32 para1, u32 para2)
{
    LIVE_TV_CHANNEL_LIST_T *p_all_chan = NULL;
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    p_all_chan = (LIVE_TV_CHANNEL_LIST_T *)para1;
    if(p_all_chan && p_all_chan->status == TVLIVE_DATA_SUCCESS)
    {
        OS_PRINTF("###p_all_chan->num == %d##\n",p_all_chan->num);
        ui_live_tv_api_all_chan_arrived(p_all_chan);
        ui_set_livetv_db_flag(TRUE);
        ui_comm_dlg_close();

        if(fw_get_focus_id() == ROOT_ID_MAINMENU)
        {
            ui_desktop_notify_msg_to_win(ROOT_ID_MAINMENU, MSG_OPEN_LIVETV_MENU);
        }
        if(g_desktop.up_livetv_timing == FALSE)
        {
            ui_desktop_notify_msg_to_win(ROOT_ID_LIVE_TV, MSG_GET_LIVE_TV_GET_ZIP);
        }
        else
        {
            ui_desktop_notify_msg_to_win(ROOT_ID_LIVE_TV, MSG_LIVETV_UPDATE_TIMING);
        }
    }
    else
    {
        ui_set_livetv_db_flag(FALSE);
    }
    g_desktop.up_livetv_timing = FALSE;
    return SID_DESKTOP_IPTV_INIT;
}
static STATEID StaDesktop_jump_to_desktop_iptv_init(control_t *ctrl, u16 msg,
u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_IPTV_INIT;
}
static void SexDesktop_LivetvChanListInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static void SenDesktop_IptvInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    #ifndef WIN32
    {
        fw_notify_root(g_desktop.p_desktop, NOTIFY_T_MSG, FALSE,
MSG_DESKTOP_JUMP_TO_NEXT_STATUS, 0, 0);
    }
    #endif
}
static void SexDesktop_IptvInit(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static STATEID StaDesktop_iptv_init_on_init_success(control_t *ctrl, u16 msg,
u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    ui_iptv_set_dp_state(IPTV_DP_STATE_INIT_SUCC);
    return SID_DESKTOP_IDLE;
}
static STATEID StaDesktop_iptv_init_on_init_fail(control_t *ctrl, u16 msg,
u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    ui_iptv_set_dp_state(IPTV_DP_STATE_INIT_FAIL);
    return SID_DESKTOP_IDLE;
}
static STATEID StaDesktop_jump_to_desktop_idle_status(control_t *ctrl, u16 msg
, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    return SID_DESKTOP_IDLE;
}
static void SenDesktop_Idle(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}
static STATEID StaDesktop_livetv_init_on_init_success_idle(control_t *ctrl,
u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    ui_set_init_livetv_status(LIVETV_INIT_SUCCESS);
    if(ui_get_livetv_db_flag() == TRUE)
    {
        g_desktop.up_livetv_timing = TRUE;
    }
    return SID_DESKTOP_LIVETV_CATGRY_INIT;
}
static STATEID StaDesktop_livetv_init_on_init_failed_idle(control_t *ctrl,
u16 msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    ui_set_init_livetv_status(LIVETV_INIT_FAILED);
    return SID_DESKTOP_IDLE;
}
static STATEID StaDesktop_iptv_init_on_init_success_idle(control_t *ctrl, u16
msg, u32 para1, u32 para2)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
    ui_iptv_set_dp_state(IPTV_DP_STATE_INIT_SUCC);
    return SID_DESKTOP_IDLE;
}

static void SexDesktop_Idle(void)
{
    OS_PRINTF("####func:[%s], line[%d]####\n", __FUNCTION__,__LINE__);
}

static RET_CODE on_ca_vol_auth(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	rect_t rec;
	u8 focus_id = 0;

	focus_id = fw_get_focus_id();
	if((ui_is_vod_menu(focus_id)))
	{
		rec.left = SYS_DLG_FOR_CHK_X;
		rec.right = SYS_DLG_FOR_CHK_X+PWDLG_W+60;
		rec.top = SYS_DLG_FOR_CHK_Y;
		rec.bottom = SYS_DLG_FOR_CHK_Y+PWDLG_H;

		ui_comm_cfmdlg_open(&rec, IDS_NC_AUTH_BEAT_FAILURE, NULL, 2000);
	}
	OS_PRINTF("[%s] get msg MSG_CA_VOD_AUTH \n",__FUNCTION__);
    return SUCCESS;
}

static RET_CODE respond_play_tuner_locked(control_t *p_cont, u16 msg, u32 para1, u32 para2)  
{
	OS_PRINTF("%s, %d call in\n", __FUNCTION__, __LINE__);
	ui_play_set_prog_ca_info();
	return SUCCESS;
}

static RET_CODE on_update_search(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	dlg_ret_t dlg_ret = DLG_RET_NULL;
	u8 index = 0;
	u32 pre_nit_ver = 0;
	u32 new_nit_ver = 0;

	comm_dlg_data_t dlg_ack_data =
	{
		ROOT_ID_INVALID,
		DLG_FOR_ASK | DLG_STR_MODE_STATIC,
		COMM_DLG_X, COMM_DLG_Y,
		COMM_DLG_W, COMM_DLG_H,
		RSC_INVALID_ID,
		10000,
	};

	new_nit_ver = para1;
	sys_status_get_nit_version(&pre_nit_ver);
	OS_PRINTF("\r\n*** %s:new_nit_ver[0x%x], pre_nit_ver [0x%x]  ", __FUNCTION__, new_nit_ver, pre_nit_ver );
	if(new_nit_ver == pre_nit_ver)
	{
		OS_PRINTF("@@@ %s %d version same \n", __FUNCTION__,__LINE__);
		return SUCCESS;
	}
	else if(pre_nit_ver == 0xFFFFFFFF)
	{
		OS_PRINTF("@@@ %s %d pre_nit_ver[0xFFFFFFFF] \n", __FUNCTION__,__LINE__);
		sys_status_set_nit_version(new_nit_ver);
		return SUCCESS;
	}

	index = fw_get_focus_id();

	if((ROOT_ID_DO_SEARCH== index)
			||(ROOT_ID_AUTO_SEARCH== index)
			||(! do_search_is_finish())
			||(ui_is_vod_menu(index)))
	{
		OS_PRINTF("ERR %s %d cur searing \n", __FUNCTION__,__LINE__);
		return ERR_FAILURE;
	}

	switch(CUSTOMER_ID)
	{
		case CUSTOMER_XINNEW_GAIZHOU:
			index = fw_get_focus_id();
			if(index != ROOT_ID_DO_SEARCH)
			{
				ui_clean_all_menus_do_nothing();
				manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_AUTO, 0);
			}
			break;
		default:
			dlg_ack_data.content = (u32)IDS_DVB_UPDATE_SEARCH_TITLE;
			dlg_ret = ui_comm_dlg_open(&dlg_ack_data);
			if(dlg_ret == DLG_RET_YES)
			{
				index = fw_get_focus_id();
				if(index != ROOT_ID_DO_SEARCH)
				{
					ui_clean_all_menus_do_nothing();
					manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_AUTO, 0);
				}
			}				
			break;
	}

  return SUCCESS;
}

#define FACTORY_TEST_ENTER_PASSWORD  (0x3636)
static RET_CODE on_enter_factory_test(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	static u16 password;
	u8 inputNum = 0;

	if(msg == MSG_ENTER_FACTORY_TEST_SAV)
	{
		inputNum = 3;
	}
	else if(msg == MSG_ENTER_FACTORY_TEST_DEL)
	{
		inputNum = 6;
	}

	password = (password<<4)|inputNum;
	if(password == FACTORY_TEST_ENTER_PASSWORD)
	{
		manage_open_menu(ROOT_ID_FACTORY_TEST, 0, 0);
		password = 0;
	}
	return SUCCESS;
}


#ifdef ENABLE_ADS
static RET_CODE on_picture_draw_end(control_t *p_cont, u16 msg,u32 para1, u32 para2)
{
	return SUCCESS;
}

static RET_CODE on_picture_refresh_ads(control_t *p_cont, u16 msg,u32 para1, u32 para2)
{
	DEBUG(ADS,INFO,"on_picture_refresh_ads\n");
	ui_adv_pic_play(ADS_AD_TYPE_BANNER, ROOT_ID_BACKGROUND);
	return SUCCESS;
}
#endif

BEGIN_AP_EVTMAP(sys_evtmap)
CONVERT_EVENT(PNP_SVC_USB_MASS_PLUG_IN, MSG_PLUG_IN)
CONVERT_EVENT(PNP_SVC_USB_DEV_PLUG_OUT, MSG_PLUG_OUT)
CONVERT_EVENT(PNP_SVC_USB_DEV_PLUG_IN, MSG_USB_DEV_PLUG_IN)
CONVERT_EVENT(PNP_SVC_VFS_MOUNT_SUCCESS, MSG_NETWORK_MOUNT_SUCCESS)
CONVERT_EVENT(PNP_SVC_VFS_MOUNT_FAIL, MSG_NETWORK_MOUNT_FAIL)
//CONVERT_EVENT(PNP_SVC_CONFIG_TCPIP_RESULT, MSG_CONFIG_TCPIP)
#if ENABLE_ETH
CONVERT_EVENT(PNP_SVC_INTERNET_PLUG_IN, MSG_INTERNET_PLUG_IN)
CONVERT_EVENT(PNP_SVC_INTERNET_PLUG_OUT, MSG_INTERNET_PLUG_OUT)
#endif
CONVERT_EVENT(PNP_SVC_WIFI_CONNECTING_AP, MSG_WIFI_AP_CONNECTING)
CONVERT_EVENT(PNP_SVC_WIFI_CONNECT_AP, MSG_WIFI_AP_CONNECT)
CONVERT_EVENT(PNP_SVC_WIFI_DIS_AP, MSG_WIFI_AP_DISCONNECT)
  CONVERT_EVENT(PNP_SVC_WIFI_CONNECT_FAIL_AP, MSG_WIFI_AP_DISCONNECT)
CONVERT_EVENT(NET_SVC_CONFIG_IP, MSG_CONFIG_IP)
CONVERT_EVENT(NET_SVC_PING_TEST, MSG_PING_TEST)
CONVERT_EVENT(NET_SVC_WIFI_AP_CNT, MSG_GET_WIFI_AP_CNT)
CONVERT_EVENT(NET_SVC_3G_INFO_GET, MSG_GET_3G_INFO)
CONVERT_EVENT(PNP_SVC_GPRS_STATUS_UPDATE, MSG_GPRS_STATUS_UPDATE)
CONVERT_EVENT(PNP_SVC_3G_STATUS_UPDATE, MSG_G3_STATUS_UPDATE)
#if ENABLE_USB_ETH
CONVERT_EVENT(PNP_SVC_USB_INTERNET_PLUG_IN, MSG_USB_INTERNET_PLUG_IN)
CONVERT_EVENT(PNP_SVC_USB_INTERNET_PLUG_OUT, MSG_USB_INTERNET_PLUG_OUT)
#endif
END_AP_EVTMAP(sys_evtmap)


// start message mapping
#ifdef ENABLE_CA
BEGIN_MSGPROC(ui_desktop_proc_on_normal, cas_manage_proc_on_normal_cas)
#else
BEGIN_MSGPROC(ui_desktop_proc_on_normal, cont_class_proc)
#endif

ON_COMMAND(MSG_ENTER_FACTORY_TEST_SAV, on_enter_factory_test)
ON_COMMAND(MSG_ENTER_FACTORY_TEST_DEL, on_enter_factory_test)

ON_COMMAND(MSG_PB_NIT_FOUND, on_update_search)
  ON_COMMAND(MSG_PLAY_TUNER_LOCKED, respond_play_tuner_locked)
ON_COMMAND(MSG_HEART_BEAT, on_heart_beat)
ON_COMMAND(MSG_PLUG_IN, on_plug_in)
ON_COMMAND(MSG_PLUG_OUT, on_plug_out)
ON_COMMAND(MSG_USB_DEV_PLUG_IN, on_usb_dev_plug_in)
ON_COMMAND(MSG_SIGNAL_CHECK, on_check_signal)
ON_COMMAND(MSG_TIME_UPDATE, on_time_update)
ON_COMMAND(MSG_DESCRAMBLE_SUCCESS, on_update_descramble)
ON_COMMAND(MSG_DESCRAMBLE_FAILED, on_update_descramble)
ON_COMMAND(MSG_CLOSE_MENU, on_close_menu)
ON_COMMAND(MSG_OPEN_MENU_IN_TAB, on_open_in_tab)
ON_COMMAND(MSG_MUTE, on_mute)
ON_COMMAND(MSG_PAUSE, on_pause)
ON_COMMAND(MSG_PLAY, on_play)
#if ENABLE_TTX_SUBTITLE
ON_COMMAND(MSG_START_TTX, on_start_ttx)
ON_COMMAND(MSG_STOP_TTX, on_stop_ttx)
ON_COMMAND(MSG_TTX_KEY, on_ttx_key)
#endif
ON_COMMAND(MSG_POWER_OFF, on_power_off)
ON_COMMAND(MSG_POWER_ON, on_power_on)
ON_COMMAND(MSG_SWITCH_SCART, on_scart_switch)
ON_COMMAND(MSG_SCART_VCR_DETECTED, on_scart_vcr_detected)
#ifdef IMPL_NEW_EPG
ON_COMMAND(MSG_EPG_CHANG_TABLE_POLICY,on_epg_request_table_policy_switch)
#else
ON_COMMAND(MSG_PF_CHANGE, on_pf_change)
#endif
#if ENABLE_ETH
ON_COMMAND(MSG_INTERNET_PLUG_IN, on_ethernet_plug_in)
ON_COMMAND(MSG_INTERNET_PLUG_OUT, on_ethernet_plug_out)
#endif
ON_COMMAND(MSG_USB_INTERNET_PLUG_IN, on_usb_ethernet_plug_in)
ON_COMMAND(MSG_USB_INTERNET_PLUG_OUT, on_usb_ethernet_plug_out)
ON_COMMAND(MSG_CONFIG_IP, on_save_config_ip_to_ssdata)
//ON_COMMAND(MSG_GET_WIFI_AP_CNT, on_desktop_search_wifi_over)
ON_COMMAND(MSG_WIFI_AP_CONNECTING, on_desktop_wifi_connecting)
ON_COMMAND(MSG_WIFI_AP_CONNECT, on_save_config_wifi_to_ssdata)
ON_COMMAND(MSG_WIFI_AP_DISCONNECT, on_desktop_connect_wifi_failed)
ON_COMMAND(MSG_GPRS_STATUS_UPDATE, on_desktop_gprs_status_update)
ON_COMMAND(MSG_G3_STATUS_UPDATE, on_desktop_g3_status_update)
ON_COMMAND(MSG_G3_READY, on_3g_dev_ready)


  ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, on_desktop_load_media_success)
  ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR, on_desktop_load_media_error)
  ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT, on_desktop_load_media_exit)
    ON_COMMAND(MSG_DESKTOP_NET_CONNTECED, ui_desktop_on_state_process)
    ON_COMMAND(MSG_DESKTOP_NET_UNCONNTECED, ui_desktop_on_state_process)
    ON_COMMAND(MSG_INIT_LIVETV_SUCCESS, ui_desktop_on_state_process)
    ON_COMMAND(MSG_INIT_LIVETV_FAILED, ui_desktop_on_state_process)
    ON_COMMAND(MSG_GET_LIVE_TV_CATEGORY_ARRIVAL, ui_desktop_on_state_process)
    ON_COMMAND(MSG_GET_LIVE_TV_GET_ZIP, ui_desktop_on_state_process)
    ON_COMMAND(MSG_GET_LIVE_TV_CHANNELLIST, ui_desktop_on_state_process)
    ON_COMMAND(MSG_DESKTOP_JUMP_TO_NEXT_STATUS, ui_desktop_on_state_process)
//ON_COMMAND(MSG_REVISE_EID, on_revise_eid)

//#ifdef PATCH_ENABLE
//  ON_COMMAND(MSG_EDIT_UCAS, on_edit_ucas)
//#endif
ON_COMMAND(MSG_EDIT_TWIN_PORT, on_open_twin_port)


ON_COMMAND(MSG_SWITCH_PN, on_switch_pn)
#if ENABLE_TTX_SUBTITLE
ON_COMMAND(MSG_SWITCH_SUBT, on_switch_subt)
#endif
ON_COMMAND(MSG_SWITCH_VIDEO_MODE, on_switch_video_mode)  //disable v.format for temp ,wait for 144pin
#ifdef SLEEP_TIMER
ON_COMMAND(MSG_SLEEP, on_sleep)
ON_COMMAND(MSG_SLEEP_TMROUT, on_sleep_tmrout)
#endif
//ON_COMMAND(MSG_START_RECORD, on_start_record)
ON_COMMAND(MSG_OTA_FOUND, on_ota_info_find)
ON_COMMAND(MSG_TIMER_OPEN, on_timer_open)
ON_COMMAND(MSG_NET_DEV_MOUNT, on_net_ip_mount)
ON_COMMAND(MSG_NETWORK_MOUNT_SUCCESS, on_net_mount_success)
ON_COMMAND(MSG_NETWORK_MOUNT_FAIL, on_net_mount_fail)

//ON_COMMAND(MSG_SATIP_CLIENT_REQ_CHANGE_PG, on_desktop_recieve_satip_client_msg)
ON_COMMAND(MSG_OTA_TMOUT, on_ota_timeout_start_play)
ON_COMMAND(MSG_OTA_TRIGGER_RESET, on_ota_reboot_into_ota)
ON_COMMAND(MSG_OTA_FINISH,on_ota_in_maincode_finsh)

	ON_COMMAND(MSG_CA_VOD_AUTH, on_ca_vol_auth)


#ifdef ENABLE_ADS
	ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_picture_draw_end)
	ON_COMMAND(MSG_REFRESH_ADS_PIC, on_picture_refresh_ads) 
	//ON_COMMAND(MSG_ADS_TO_UI, on_ads_process)
	ON_COMMAND(MSG_ADS_MESSAGE, on_ads_process)

#ifdef ADS_DIVI_SUPPORT
	ON_COMMAND(MSG_ADS_TIME, on_ads_time)
#endif
#endif

#ifdef ENABLE_CA
	END_MSGPROC(ui_desktop_proc_on_normal, cas_manage_proc_on_normal_cas)
#else
	END_MSGPROC(ui_desktop_proc_on_normal, cont_class_proc)
#endif

#if ENABLE_TTX_SUBTITLE
BEGIN_MSGPROC(ui_desktop_proc_on_ttx, cont_class_proc)
ON_COMMAND(MSG_TIME_UPDATE, on_time_update)
ON_COMMAND(MSG_CLOSE_MENU, on_close_menu)
ON_COMMAND(MSG_STOP_TTX, on_stop_ttx)
ON_COMMAND(MSG_TTX_KEY, on_ttx_key)
ON_COMMAND(MSG_POWER_OFF, on_power_off)
ON_COMMAND(MSG_MUTE, on_mute)
#ifdef SLEEP_TIMER
//ON_COMMAND(MSG_SLEEP_TMROUT, on_sleep_tmrout)
#endif
END_MSGPROC(ui_desktop_proc_on_ttx, cont_class_proc);
#endif

// start key mapping on normal
#ifdef ENABLE_CA
BEGIN_KEYMAP(ui_desktop_keymap_on_normal, cas_manage_kepmap_on_normal_cas)
#else
BEGIN_KEYMAP(ui_desktop_keymap_on_normal, NULL)
#endif

ON_EVENT(V_KEY_HOT_XSAVE, MSG_ENTER_FACTORY_TEST_SAV)
ON_EVENT(V_KEY_HOT_XDEL, MSG_ENTER_FACTORY_TEST_DEL)

ON_EVENT(V_KEY_MUTE, MSG_MUTE)
ON_EVENT(V_KEY_PAUSE, MSG_PAUSE)
//ON_EVENT(V_KEY_SCAN, MSG_PAUSE)//timeshift for customer jingli
ON_EVENT(V_KEY_PLAY, MSG_PLAY)
ON_EVENT(V_KEY_MENU, MSG_OPEN_MENU_IN_TAB | ROOT_ID_MAINMENU)
ON_EVENT(V_KEY_UP, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
ON_EVENT(V_KEY_DOWN, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
ON_EVENT(V_KEY_PAGE_UP, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
ON_EVENT(V_KEY_RECALL, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
ON_EVENT(V_KEY_BACK, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)
  ON_EVENT(V_KEY_UCAS, MSG_OPEN_MENU_IN_TAB | ROOT_ID_RECALL_LIST)
ON_EVENT(V_KEY_TVRADIO, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR)

//ON_EVENT(V_KEY_0, MSG_OPEN_MENU_IN_TAB | ROOT_ID_TTX_HELP)
ON_EVENT(V_KEY_1, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_2, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_3, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_4, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_5, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_6, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_7, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_8, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)
ON_EVENT(V_KEY_9, MSG_OPEN_MENU_IN_TAB | ROOT_ID_NUM_PLAY)

ON_EVENT(V_KEY_LEFT, MSG_OPEN_MENU_IN_TAB | ROOT_ID_VOLUME)
ON_EVENT(V_KEY_RIGHT, MSG_OPEN_MENU_IN_TAB | ROOT_ID_VOLUME)

ON_EVENT(V_KEY_AUDIO, MSG_OPEN_MENU_IN_TAB | ROOT_ID_AUDIO_SET)
// ON_EVENT(V_KEY_SUBT, MSG_OPEN_MENU_IN_TAB | ROOT_ID_SUBT_LANGUAGE)
// ON_EVENT(V_KEY_ZOOM, MSG_OPEN_MENU_IN_TAB | ROOT_ID_ZOOM)
//ON_EVENT(V_KEY_BLUE, MSG_OPEN_MENU_IN_TAB | ROOT_ID_MULTIVIEW)
//ON_EVENT(V_KEY_PLAY, MSG_PAUSE)
//ON_EVENT(V_KEY_BLUE, MSG_START_RECORD)
ON_EVENT(V_KEY_VDOWN, MSG_OPEN_MENU_IN_TAB | ROOT_ID_VOLUME)
ON_EVENT(V_KEY_VUP, MSG_OPEN_MENU_IN_TAB | ROOT_ID_VOLUME)

ON_EVENT(V_KEY_OK, MSG_OPEN_MENU_IN_TAB | ROOT_ID_SMALL_LIST)
ON_EVENT(V_KEY_PROGLIST, MSG_OPEN_MENU_IN_TAB | ROOT_ID_SMALL_LIST)
//ON_EVENT(V_KEY_FAV, MSG_OPEN_MENU_IN_TAB | ROOT_ID_FAV_LIST)
ON_EVENT(V_KEY_FIND, MSG_OPEN_MENU_IN_TAB | ROOT_ID_FIND)
//ON_EVENT(V_KEY_SAT, MSG_OPEN_MENU_IN_TAB | ROOT_ID_HOT_SLIST)
ON_EVENT(V_KEY_CTRL0, MSG_OPEN_MENU_IN_TAB | ROOT_ID_WEATHER)
ON_EVENT(V_KEY_SLEEP, MSG_SLEEP)
//ON_EVENT(V_KEY_BLUE, MSG_OPEN_MENU_IN_TAB |ROOT_ID_BOOK_LIST)  //MSG_OPEN_MENU_IN_TAB | ROOT_ID_SLEEP_HOTKEY)
//ON_EVENT(V_KEY_EPG, MSG_EPG)
//ON_EVENT(V_KEY_EPG, MSG_OPEN_MENU_IN_TAB | ROOT_ID_VEPG)
#if ENABLE_MUSIC_PICTURE
ON_EVENT(V_KEY_REC, MSG_START_RECORD)

#endif

ON_EVENT(V_KEY_HOT_XEXTEND, MSG_OPEN_MENU_IN_TAB |ROOT_ID_XEXTEND)

//#ifdef PATCH_ENABLE
// ON_EVENT(V_KEY_UCAS, MSG_EDIT_UCAS)
//#endif

ON_EVENT(V_KEY_BISS_KEY, MSG_EDIT_TWIN_PORT)

#if ENABLE_TTX_SUBTITLE
ON_EVENT(V_KEY_TTX, MSG_START_TTX)
#endif
ON_EVENT(V_KEY_TVSAT, MSG_SWITCH_SCART)
ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
ON_EVENT(V_KEY_PN, MSG_SWITCH_PN)
ON_EVENT(V_KEY_SUBT, MSG_SWITCH_SUBT)
ON_EVENT(V_KEY_VIDEO_MODE, MSG_SWITCH_VIDEO_MODE)
#ifdef ENABLE_CA
	END_KEYMAP(ui_desktop_keymap_on_normal, cas_manage_kepmap_on_normal_cas)
#else
	END_KEYMAP(ui_desktop_keymap_on_normal, NULL)
#endif

// start key mapping on standby
BEGIN_KEYMAP(ui_desktop_keymap_on_standby, NULL)
ON_EVENT(V_KEY_POWER, MSG_POWER_ON)
END_KEYMAP(ui_desktop_keymap_on_standby, NULL)

// start key mapping on vcr
BEGIN_KEYMAP(ui_desktop_keymap_on_vcr, NULL)
ON_EVENT(V_KEY_TVSAT, MSG_SWITCH_SCART)
ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
END_KEYMAP(ui_desktop_keymap_on_vcr, NULL)

// start key mapping on teletext
#if ENABLE_TTX_SUBTITLE
BEGIN_KEYMAP(ui_desktop_keymap_on_ttx, NULL)
ON_EVENT(V_KEY_TTX, MSG_STOP_TTX)
ON_EVENT(V_KEY_MENU, MSG_STOP_TTX)
ON_EVENT(V_KEY_CANCEL, MSG_STOP_TTX)
ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
ON_EVENT(V_KEY_MUTE, MSG_MUTE)

ON_EVENT(V_KEY_0, MSG_TTX_KEY | TTX_KEY_0)
ON_EVENT(V_KEY_1, MSG_TTX_KEY | TTX_KEY_1)
ON_EVENT(V_KEY_2, MSG_TTX_KEY | TTX_KEY_2)
ON_EVENT(V_KEY_3, MSG_TTX_KEY | TTX_KEY_3)
ON_EVENT(V_KEY_4, MSG_TTX_KEY | TTX_KEY_4)
ON_EVENT(V_KEY_5, MSG_TTX_KEY | TTX_KEY_5)
ON_EVENT(V_KEY_6, MSG_TTX_KEY | TTX_KEY_6)
ON_EVENT(V_KEY_7, MSG_TTX_KEY | TTX_KEY_7)
ON_EVENT(V_KEY_8, MSG_TTX_KEY | TTX_KEY_8)
ON_EVENT(V_KEY_9, MSG_TTX_KEY | TTX_KEY_9)

ON_EVENT(V_KEY_OK, MSG_TTX_KEY | TTX_KEY_TRANSPARENT)
ON_EVENT(V_KEY_CANCEL, MSG_TTX_KEY | TTX_KEY_CANCEL)

ON_EVENT(V_KEY_UP, MSG_TTX_KEY | TTX_KEY_UP)
ON_EVENT(V_KEY_DOWN, MSG_TTX_KEY | TTX_KEY_DOWN)
ON_EVENT(V_KEY_LEFT, MSG_TTX_KEY | TTX_KEY_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_TTX_KEY | TTX_KEY_RIGHT)
ON_EVENT(V_KEY_PAGE_UP, MSG_TTX_KEY | TTX_KEY_PAGE_UP)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_TTX_KEY | TTX_KEY_PAGE_DOWN)


ON_EVENT(V_KEY_RED, MSG_TTX_KEY | TTX_KEY_RED)
ON_EVENT(V_KEY_GREEN, MSG_TTX_KEY | TTX_KEY_GREEN)
ON_EVENT(V_KEY_YELLOW, MSG_TTX_KEY | TTX_KEY_YELLOW)
ON_EVENT(V_KEY_BLUE, MSG_TTX_KEY | TTX_KEY_CYAN)
END_KEYMAP(ui_desktop_keymap_on_ttx, NULL)
#endif

