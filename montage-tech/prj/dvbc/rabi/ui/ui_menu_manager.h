/****************************************************************************

 ****************************************************************************/
#ifndef __UI_MENU_MANAGER_H__
#define __UI_MENU_MANAGER_H__

enum ui_root_id
{
  ROOT_ID_BACKGROUND = 1,
  ROOT_ID_MAINMENU = 2,
  ROOT_ID_SUBMENU = 3,
  ROOT_ID_PROG_LIST = 4,
  ROOT_ID_DELETE_ALL = 5,
  ROOT_ID_MOTOR_ANTENNA = 6,
  ROOT_ID_MOTOR_SET = 7,
  ROOT_ID_LIMIT_SET = 8,
  ROOT_ID_SAT_LIST = 9,
  ROOT_ID_TP_LIST = 10,
  ROOT_ID_EDIT_TP = 11,
  ROOT_ID_EDIT_SAT = 12,
  ROOT_ID_TP_SEARCH = 13,
  ROOT_ID_SET_SEARCH = 14,
  ROOT_ID_DO_SEARCH = 15,
  ROOT_ID_GOTO_X = 16,
  ROOT_ID_LANGUAGE = 17,
  ROOT_ID_TVSYS_SET = 18,
  ROOT_ID_TIME_SET = 19,
  ROOT_ID_BOOK_LIST = 20,
  ROOT_ID_PARENTAL_LOCK = 21,
  ROOT_ID_OSD_SET = 22,
  ROOT_ID_OTHERS = 23,
  ROOT_ID_INFO = 24,
  ROOT_ID_FACTORY_SET = 25,
  ROOT_ID_SATCODX = 26,
  ROOT_ID_OTA = 27,
  ROOT_ID_TETRIS = 28,
  ROOT_ID_OTHELLO = 29,
  ROOT_ID_POPUP = 30,
  ROOT_ID_PASSWORD = 31,
  ROOT_ID_VOLUME = 32,
  ROOT_ID_SIGNAL = 33,
  ROOT_ID_MUTE = 34,
  ROOT_ID_PAUSE = 35,
  ROOT_ID_AUDIO_SET = 36,
  ROOT_ID_PROG_BAR = 37,
  ROOT_ID_NUM_PLAY = 38,
  ROOT_ID_NOTIFY = 39,
  ROOT_ID_ZOOM = 40,

  ROOT_ID_FIND = 41,
  ROOT_ID_EPG = 42,
  ROOT_ID_EPG_DETAIL = 43,
  ROOT_ID_MULTIVIEW = 44,
  ROOT_ID_FAV_SET = 45,
  ROOT_ID_SORT_LIST = 46,
  ROOT_ID_RENAME = 47,
  ROOT_ID_TIMER = 48,
  ROOT_ID_SMALL_LIST = 49,
  ROOT_ID_FAV_LIST = 50,
  ROOT_ID_HOT_SLIST = 51,
  ROOT_ID_OTA_SEARCH = 52,
  ROOT_ID_EDIT_UCAS = 53,
  ROOT_ID_EDIT_KEY = 54,
  ROOT_ID_TEST_MENU = 55,
  ROOT_ID_TEST_FKEY = 56,
  ROOT_ID_TEST_TIMER = 57,  
  ROOT_ID_SLEEP_TIMER = 58,
  ROOT_ID_KEYBOARD = 59,
  ROOT_ID_UPGRADE_BY_USB = 60,

  ROOT_ID_DUMP_BY_USB = 61,
  ROOT_ID_REMOVE_USB = 62,
  ROOT_ID_USB_MUSIC = 63,
  ROOT_ID_RECORD_MANAGER = 64,
  ROOT_ID_HDD_INFO = 65,
  ROOT_ID_STORAGE_FORMAT = 66,
  ROOT_ID_DVR_CONFIG = 67,
  ROOT_ID_JUMP = 68,
  ROOT_ID_PVR_REC_BAR = 69,
  ROOT_ID_PVR_PLAY_BAR = 70,
  ROOT_ID_VEPG = 71,
  ROOT_ID_PICTURE = 72,
  ROOT_ID_PIC_PLAY_MODE_SET = 73,
  ROOT_ID_UPGRADE_BY_RS232 = 74,
  ROOT_ID_TIMESHIFT_BAR = 75,
  ROOT_ID_WEATHER = 76,
  ROOT_ID_WEATHER_WORLD = 77,
  ROOT_ID_SLEEP_HOTKEY = 78,
  ROOT_ID_V_RESOLUTION = 79,
  ROOT_ID_PROG_DETAIL = 80,

  ROOT_ID_KEYBOARD_V2 = 81,
  ROOT_ID_USB_PICTURE = 82,
  ROOT_ID_INSTALLATION_LIST = 83,
  ROOT_ID_MOTOR_SETLOCATION = 84,
  ROOT_ID_SAT_SEARCH = 85,
  ROOT_ID_MOTOR_SET_INSTALLTION_V2 = 86,
  ROOT_ID_USB_MUSIC_FULLSCREEN = 87,
  ROOT_ID_EDIT_SAT_V2 = 88,
  ROOT_ID_USB_SETTING =89,
  ROOT_ID_TWIN_PORT = 90,
  ROOT_ID_CA_BC = 91,
  ROOT_ID_SUBT_LANGUAGE = 92,
  ROOT_ID_RECALL_LIST = 93,
  ROOT_ID_CA_SETTING = 94,
  ROOT_ID_CA_INFORMATION = 95,
  ROOT_ID_RS232_PRODOCOL = 96,
  ROOT_ID_TS_DELAY_SETTING = 97,
  ROOT_ID_CA_SETTING_KEY_EDIT = 98,
  ROOT_ID_VOLUME_USB = 99,
  ROOT_ID_USB_FILEPLAY = 100,

  ROOT_ID_FILEPLAY_BAR = 101,
  ROOT_ID_FP_AUDIO_SET = 102,
  ROOT_ID_FILEPLAY_SUBT = 103,
  ROOT_ID_VIDEO_GOTO = 104,
  ROOT_ID_GOOGLE_MAP = 105,
  ROOT_ID_WEATHER_FORECAST = 106,
  ROOT_ID_NETWORK_PLAY = 107,
  ROOT_ID_NETWORK_CONFIG = 108,
  ROOT_ID_PING_TEST = 109,
  ROOT_ID_ONMOV_DESCRIPTION = 110,
  ROOT_ID_NETWORK_PLACES = 111,
  ROOT_ID_SUBMENU_USB = 112,
  ROOT_ID_VIDEO_PLAYER = 113,
  ROOT_ID_USB_STORAGE = 115,
  ROOT_ID_EDIT_USR_PWD = 116,
  ROOT_ID_SUBMENU_NETWORK = 117,
  ROOT_ID_YOUTUBE = 118,
  ROOT_ID_TEXT_ENCODE = 119,
  ROOT_ID_WIFI = 120,

  ROOT_ID_WIFI_LINK_INFO = 121,
  ROOT_ID_EDIT_IP_PATH = 122,
  ROOT_ID_EDIT_CITY = 123,
  ROOT_ID_ONMOV_WEBSITES = 124,
  ROOT_ID_PPPOE_CONNECT = 125,
  ROOT_ID_MOVIE_SEARCH = 126,
  ROOT_ID_YOUPORN = 127,
  ROOT_ID_AUDIO_SET_RECORD = 128,
  ROOT_ID_UPGRADE_BY_NETWORK = 129,
  ROOT_ID_FLICKR = 130,
  ROOT_ID_FLICKR_FULLSCRN = 131,
  ROOT_ID_TS_RECORD = 132,
  ROOT_ID_DLNA_DMR = 133,  
  ROOT_ID_YAHOO_NEWS = 137,
  ROOT_ID_NEWS_TITLE = 138,
  ROOT_ID_NEWS_INFO = 139,
  ROOT_ID_ALBUMS = 140,

  ROOT_ID_PHOTOS = 141,
  ROOT_ID_PHOTO_SHOW = 142,
  ROOT_ID_PHOTO_INFO = 143,
  ROOT_ID_NETWORK_MUSIC = 144,
  ROOT_ID_NETWORK_MUSIC_SEARCH = 145,
  ROOT_ID_REDTUBE = 146,
  ROOT_ID_SUBNETWORK_CONFIG = 147,

  ROOT_ID_NETWORK_CONFIG_LAN = 148,
  ROOT_ID_NETWORK_CONFIG_WIFI = 149,
  ROOT_ID_NETWORK_CONFIG_3G = 150,
  ROOT_ID_NETWORK_CONFIG_GPRS = 151,
  ROOT_ID_POPUP2 = 152,
  ROOT_ID_STB_NAME = 153,
  ROOT_ID_MANUAL_SEARCH = 154,
  ROOT_ID_RANGE_SEARCH = 155,
  ROOT_ID_SATIP = 156,
  ROOT_ID_NETMEDIA = 157,
  ROOT_ID_NM_SIMPLE = ROOT_ID_NETMEDIA,
  ROOT_ID_NM_CLASSIC = ROOT_ID_NETMEDIA,
  ROOT_ID_NM_COUNTRY = ROOT_ID_NETMEDIA,
  ROOT_ID_SUBMENU_NM = 158,
  ROOT_ID_LIVE_TV = 159,
  ROOT_ID_IPTV = 160,
  ROOT_ID_IPTV_DESCRIPTION = 161,  
  ROOT_ID_IPTV_SEARCH = 162,
  ROOT_ID_VDO_FAVORITE = 163,
  ROOT_ID_PLAY_HIST = 164,
  ROOT_ID_IPTV_PLAYER = 165,
  ROOT_ID_FAVORITE = 166,

  ROOT_ID_AUTO_SEARCH = 167,
  ROOT_ID_FREQ_SET = 168,
  ROOT_ID_NET_UPG = 169,
  ROOT_ID_TERRESTRIAL = 170,
  ROOT_ID_TERRESTRIAL_MANUAL_SEARCH = 171,
  ROOT_ID_TERRESTRIAL_FULL_SEARCH = 172,

//wanghm add start
  ROOT_ID_OLD_MAINMENU,
  ROOT_ID_XSYS_SET,
  ROOT_ID_XSEARCH,
  ROOT_ID_XEXTEND,
  ROOT_ID_XSMALL_INFO,
  ROOT_ID_FUN_HELP,
  ROOT_ID_POWER_TIMER,
  ROOT_ID_AUTH_SERVER_SETTING,
//wanghm add end
//langyy add start
  ROOT_ID_XLANGUAGE,
  ROOT_ID_XPARENTAL_LOCK,
  ROOT_ID_XTVSYS_SET,
  ROOT_ID_XTIME_SET,
  ROOT_ID_XUPGRADE_BY_USB,
  ROOT_ID_XFREQ_SET,
  ROOT_ID_XINFO,
  ROOT_ID_XSTB_NAME,
  ROOT_ID_XBOOT_WIZARD,
  ROOT_ID_XUPGRADE_BY_RS232,
  ROOT_ID_XSTB_HELP,
//langyy add end
//fuxl add
  ROOT_ID_NET_STATE,
  ROOT_ID_FACTORY_TEST,

  ROOT_ID_CA_START,
};


#define PS_KEEP                 0 // keep status
#define PS_PLAY                 1 // play full screen
#define PS_PREV                 2 // preview play with logo
#define PS_LOGO                 3 // stop and show logo
#define PS_STOP                 4 // stop
#define PS_TS                   5 // ts play

#define OFF                     0
#define ON                      1

#define SM_OFF                  0 // needn't signal message
#define SM_LOCK                 1 // need message when lock status changed
#define SM_BAR                  2 // need message always

#define MENU_TYPE_FULLSCREEN    0
#define MENU_TYPE_PREVIEW       1
#define MENU_TYPE_NORMAL        2
#define MENU_TYPE_POPUP         3
#define MENU_TYPE_MUSIC         4
#define MENU_TYPE_PICTURE       5
#if ENABLE_FILE_PLAY
#define MENU_TYPE_VIDEO         6
#endif
#define MENU_MAX_CNT   200

typedef  RET_CODE (*open_menu_t)(u32 para1, u32 para2);

typedef struct
{
  u8 root_id;
  u8 play_state;                //PS_
  u8 auto_close;                //OFF,ON
  u8 signal_msg;                //SM_
  open_menu_t open_func;      // open function
}menu_attr_t;

typedef struct
{
  u8 root_id;
  u32 logo_id;
}logo_attr_t;

typedef struct
{
  u8 root_id;
  rect_t position;
}preview_attr_t;

#define INVALID_IDX 0xFF

void manage_init(void);

RET_CODE ui_menu_manage(u32 event, u32 para1, u32 para2);

RET_CODE manage_open_menu(u8 root_id, u32 para1, u32 para2);

RET_CODE manage_close_menu(u8 root_id, u32 para1, u32 para2);

RET_CODE manage_back_to_menu(u8 root_id, u32 para1, u32 para2);

RET_CODE manage_notify_root(u8 root_id, u16 msg, u32 para1, u32 para2);

void manage_autoclose(void);

menu_attr_t *manage_get_curn_menu_attr(void);

BOOL manage_get_preview_rect(u8 root_id,
                             u16 *left,
                             u16 *top,
                             u16 *width,
                             u16 *height);

void manage_logo2preview(u8 root_id);

void manage_enable_autoswitch(BOOL is_enable);

u8 manage_find_preview(u8 root_id, rect_t *orc);

void ui_close_all_mennus(void);

BOOL ui_is_preview_menu(u8 root_id);

BOOL ui_is_vod_menu(u8 root_id);

u8 ui_get_preview_menu(void);

BOOL ui_is_fullscreen_menu(u8 root_id);

BOOL ui_is_popup_menu(u8 root_id);

void manage_auto_switch(void);
RET_CODE manage_tmr_reset(void);
BOOL ui_ca_get_roll_status(void);

void ui_ca_set_roll_status(BOOL is_enable);


#endif
