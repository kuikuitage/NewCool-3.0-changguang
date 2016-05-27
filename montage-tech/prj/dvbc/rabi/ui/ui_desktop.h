/****************************************************************************

 ****************************************************************************/ 
#ifndef __UI_DESKTOP_H__
#define __UI_DESKTOP_H__

#define MOV_DLG_TMOUT (20*SECOND)

#define OUTRANGE_DLG_TMOUT (5*SECOND)

#define UI_SEEK_TIME   60

#define MIN_SEEK_STEP   (60)
#define MAX_SEEK_STEP   (600)
#define SEEK_STEP_INCR  (60)
#define MAC_CHIPID_LEN (128)
enum desktop_local_msg
{
#if ENABLE_TTX_SUBTITLE
	MSG_START_TTX = MSG_LOCAL_BEGIN + 100,
	MSG_STOP_TTX,
	MSG_SWITCH_SCART,
#else
	MSG_SWITCH_SCART = MSG_LOCAL_BEGIN + 100,
#endif
	MSG_EDIT_UCAS,
	MSG_EDIT_TWIN_PORT,
	MSG_DURATION_TIMEOUT,
	MSG_SWITCH_PN,
	MSG_SWITCH_SUBT,
	MSG_SWITCH_VIDEO_MODE,
	MSG_EPG,
	MSG_START_RECORD,
	MSG_SLEEP,
    MSG_PLAY,
    MSG_DESKTOP_NET_CONNTECED,
    MSG_DESKTOP_NET_UNCONNTECED,
    MSG_DESKTOP_JUMP_TO_NEXT_STATUS,
    MSG_LIVETV_UPDATE_TIMING,
    MSG_OPEN_LIVETV_MENU,
    MSG_DLNA_START_PICTURE,
    MSG_DLNA_CLOSE_PICTURE,
    MSG_DLNA_START_VIDEO,
    MSG_DLNA_CLOSE_VIDEO,
    MSG_ENTER_FACTORY_TEST_SAV,
    MSG_ENTER_FACTORY_TEST_DEL,
    MSG_ENTER_TEST_REFRESH,
};
enum local_shfit_msg
{
	MSG_TTX_KEY = MSG_SHIFT_LOCAL_BEGIN + 0x0100,
};
typedef struct
{
  BOOL is_eth_insert; //TRUE means ethernet cable insert
  BOOL is_eth_conn; //TRUE means ethernet connect successfully
  BOOL is_usb_eth_insert; //TRUE means usb ethernet cable insert
  BOOL is_usb_eth_conn; //TRUE means usb ethernet connect successfully
  BOOL is_wifi_insert; //TRUE means wifi device insert
  BOOL is_wifi_conn; //TRUE means wifi connect successfully
  BOOL is_gprs_conn; //TRUE means gprs connect successfully
  BOOL is_3g_insert; //TRUE mens 3g insert
  BOOL is_3g_conn;   // TRUE means 3g connect succefully
}net_conn_stats_t;

typedef struct tag_net_conn_info
{
    u8 eth_conn_info;
    u8 wifi_conn_info;
    u8 gprs_conn_info;
    u8 g3_conn_info;

}net_conn_info_t;
typedef struct
{
  control_t *p_desktop;
  BOOL write_db_info_done;
  BOOL category_init;
  BOOL chan_zip_init;
  BOOL chan_list_init;
  BOOL up_livetv_timing;
  u8 livetv_init;
  void *p_chan_zip;  
}desk_sts_mch_t;   //desktop state machine 

void ui_desktop_init (void);

void ui_desktop_release(void);

void ui_desktop_main(void);

void ui_sleep_timer_create(void);

void ui_sleep_timer_destory(void);

void ui_sleep_timer_reset(void);

void uio_reset_sleep_timer(void);

void ui_set_usb_status(BOOL);
BOOL ui_get_usb_status(void);

u16 ui_get_usb_focus(void);
void ui_set_usb_focus(u16 focus);

void ui_set_net_connect_status( net_conn_stats_t);
net_conn_stats_t ui_get_net_connect_status(void);

void ui_evt_disable_ir();
void ui_evt_enable_ir();

net_conn_info_t ui_get_conn_info();

void ui_set_conn_info(net_conn_info_t net_conn_info);

BOOL ui_get_ttx_flag();
void ui_get_stb_mac_addr_chip_id(u8 *p_mac, u8 *p_id);
BOOL ui_get_livetv_db_flag(void);
void ui_set_livetv_db_flag(BOOL is_done);
u8 ui_get_init_livetv_status(void);
void ui_set_init_livetv_status(u8 status);

void ui_notify_desktop_to_get_livetv_info();

#endif

