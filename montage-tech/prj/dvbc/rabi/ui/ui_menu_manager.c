/****************************************************************************

 ****************************************************************************/

#include "ui_common.h"

#include "ui_xmainmenu.h"
#include "ui_volume.h"
#include "ui_volume_usb.h"
#include "ui_mute.h"
#include "ui_pause.h"
#include "ui_nprog_bar.h"
#include "ui_num_play.h"
#include "ui_audio_set.h"
#include "ui_signal.h"
#include "ui_do_search.h"
#include "ui_prog_list.h"
#include "ui_osd_set.h"
//#include "ui_game_tetris.h"
//#include "ui_game_othello.h"
#include "ui_find.h"
#include "ui_small_list_v2.h"
#include "ui_notify.h"
#include "ui_zoom.h"
#include "ui_set_search.h"
//#include "ui_fav_set.h"
#include "ui_favorite.h"
#include "ui_timer.h"
//#include "ui_epg_detail.h"
#include "ui_prog_detail.h"
#include "ui_book_list.h"
//#include "ui_fav_list.h"
#include "ui_ota.h"
#include "ui_ota_search.h"
#include "ui_sort_list.h"
#include "ui_rename.h"
#include "ui_usb_setting.h"
#include "ui_sleep_timer.h"
#include "ui_vepg.h"
#include "ui_xupgrade_by_usb.h"
#include "ui_dump_by_usb.h"
#include "ui_jump.h"
#include "ui_hdd_info.h"
#include "ui_storage_format.h"
//#include "ui_dvr_config.h"
//#include "ui_record_manager.h"
#include "ui_picture.h"
#include "ui_pic_play_mode_set.h"
#include "ui_xupgrade_by_rs232.h"
//#include "ui_timeshift_bar.h"
#include "ui_sleep_hotkey.h"
#include "ui_keyboard_v2.h"
#include "ui_usb_picture.h"
#include "ui_usb_music.h"
//#include "ui_twin_port.h"
//#include "ui_ca_bc.h"
#include "ui_recall_list.h"
#include "ui_subt_language.h"
//#include "ui_edit_city.h"
//#include "ui_networkplaces.h"
//#include "ui_onmov_websites.h"
#include "ui_video_player_gprot.h"
#if 0 //def WIN32
#include "ui_weather.h"
#include "ui_weather_world.h"
#include "ui_test.h"
#endif

#if ENABLE_FILE_PLAY
#include "ui_video.h"
#include "ui_video_v_full_screen.h"
#include "ui_fp_audio_set.h"
#include "ui_video_goto.h"
#include "ui_text_encode.h"
#endif

#ifdef GOBY_PLUS
#include "ui_upgrade_by_network.h"
#include "ui_network_upgrade.h"
#endif

#if ENABLE_NETWORK
#include "ui_network_config_lan.h"
#include "ui_network_config_wifi.h"
#include "ui_ping_test.h"
#include "ui_edit_usr_pwd.h"
#include "ui_wifi.h"
#include "ui_wifi_link_info.h"
#include "ui_edit_ip_path.h"
#include "ui_net_upg.h"

#if ENABLE_NETMEDIA
#include "ui_nm_gprot.h"
#endif
#if ENABLE_FLICKR
#include "ui_flickr.h"
#include "ui_flickr_fullscrn.h"
#endif
#endif

#include "ui_manual_search.h"
#include "ui_range_search.h"
//#include "ui_satip.h"
#include "ui_live_tv.h"
#include "ui_iptv_gprot.h"
#include "ui_vdo_favorite.h"

#include "ui_auto_search.h"
#include "ui_terrestrial.h"
#include "ui_terrestrial_full_search.h"
#include "ui_terrestrial_manual_search.h"
#include "ui_xsearch.h"
#include "ui_xsys_set.h"
#include "ui_xlanguage.h"
#include "ui_xparental_lock.h"
#include "ui_xtvsys_set.h"
#include "ui_xtime_set.h"
#include "ui_xupgrade_by_usb.h"
#include "ui_freq_set.h"
#include "ui_xinfo.h"
#include "ui_xstb_name.h"
#include "ui_xupgrade_by_rs232.h"
#include "ui_xextend.h"
#include "ui_small_info.h"
#include "ui_fun_help.h"

#include "ui_xbootWizard.h"

#include "ui_xstb_help.h"
#ifdef ENABLE_CA
//ca
#include "cas_manager.h"
#include "ui_ca_public.h"
#include "ui_new_mail.h"
#endif

#ifdef WIN32
#include "ui_ca_card_info.h"
#include "ui_email_mess.h"
#include "ui_ca_prompt.h"
#include "ui_ca_entitle_info.h"
#endif
#ifdef AUTH_SERVER_SETTING
#include "ui_auth_server_setting.h"
#endif
#include "ui_factory.h"
#ifdef AUPPORT_TS_RECORD
#include "ui_ts_record.h"
#endif
// the information of focus change
typedef struct
{
  // the focus is changed or not
  BOOL is_changed;

  // the focus is changed from...
  u8 from_id;

  // the focus will change to...
  u8 to_id;
}focus_change_info_t;

// the information of menu management
typedef struct
{
  // the attribute of current menu
  menu_attr_t curn_menu_attr;

  // the root which will be automatic close by sys tmr
  u8 pre_autoclose_root;

  // enable or not what process the event about focus changed
  u8 enable_autoswitch;

  // the focus change info between 2 menus
  focus_change_info_t pre_focus_change;
}menu_manage_info_t;

static menu_manage_info_t g_menu_manage =
{
  // curn menu attr
  {ROOT_ID_BACKGROUND, PS_PLAY, OFF, SM_LOCK, NULL},
  // enable autoswtich
  ROOT_ID_INVALID, TRUE,
  // focus changed info
  {FALSE, ROOT_ID_INVALID, ROOT_ID_INVALID}
};

#define PRE_AUTOCLOSE_ROOT (g_menu_manage.pre_autoclose_root)

#define CURN_MENU_ROOT_ID (g_menu_manage.curn_menu_attr.root_id)
#define CURN_MENU_PLAY_STATE (g_menu_manage.curn_menu_attr.play_state)
#define CURN_MENU_SIGN_TYPE (g_menu_manage.curn_menu_attr.signal_msg)
#define CURN_MENU_CLOSE_TYPE (g_menu_manage.curn_menu_attr.auto_close)
static menu_attr_t all_menu_attr[MENU_MAX_CNT];
static u32 totle_menu_cnk = 0; 

#if 0
static RET_CODE ui_open_netmedia(u32 para1, u32 para2);
#endif

static const menu_attr_t public_menu_attr[] =
{
//  root_id,            play_state,   auto_close, signal_msg,   open
	{ROOT_ID_BACKGROUND, PS_PLAY, ON, SM_OFF, NULL},
//wanghm add start
	{ROOT_ID_XSEARCH, PS_STOP, OFF, SM_BAR, open_ui_search},
	{ROOT_ID_XSYS_SET, PS_STOP, OFF, SM_BAR, open_xsys_set},
	{ROOT_ID_XEXTEND, PS_PLAY, OFF, SM_BAR, open_extend},
	{ROOT_ID_XSMALL_INFO, PS_PLAY, OFF, SM_BAR, open_small_info},
	{ROOT_ID_FUN_HELP, PS_KEEP, OFF, SM_OFF, open_fun_help},
#ifdef AUTH_SERVER_SETTING
	{ROOT_ID_AUTH_SERVER_SETTING, PS_KEEP, OFF, SM_OFF, open_auth_server_setting},
#endif
//wanghm add end
/********************************/
//langyy add start
{ROOT_ID_XLANGUAGE, PS_KEEP, OFF, SM_OFF, open_xlanguage_set},
{ROOT_ID_XPARENTAL_LOCK, PS_KEEP, OFF, SM_OFF, open_xparental_lock},
{ROOT_ID_XTVSYS_SET, PS_KEEP, OFF, SM_OFF, open_xtvsys_set},
{ROOT_ID_XTIME_SET, PS_KEEP, OFF, SM_OFF, open_xtime_set},
{ROOT_ID_XUPGRADE_BY_USB, PS_STOP, OFF, SM_OFF, open_xupgrade_by_usb},
{ROOT_ID_FREQ_SET, PS_STOP, OFF, SM_OFF, open_freq_set},
{ROOT_ID_XINFO, PS_KEEP, OFF, SM_OFF, open_xinfo},
{ROOT_ID_XSTB_NAME, PS_KEEP, OFF, SM_BAR, open_xstbname},
{ROOT_ID_XUPGRADE_BY_RS232, PS_KEEP, OFF, SM_OFF, open_xupgrade_by_rs232},
{ROOT_ID_XBOOT_WIZARD, PS_STOP, OFF, SM_OFF, open_xbootWizard},
{ROOT_ID_XSTB_HELP, PS_KEEP, OFF, SM_OFF, open_xstbhelp},
//langyy add end
  {ROOT_ID_MAINMENU, PS_PLAY, OFF, SM_OFF, open_main_menu},
  {ROOT_ID_DUMP_BY_USB, PS_KEEP, OFF, SM_OFF, open_dump_by_usb},
//  {ROOT_ID_DLNA_DMR, PS_KEEP, OFF, SM_OFF, open_dlna_dmr},
  {ROOT_ID_OTA, PS_KEEP, OFF, SM_BAR, open_ota},
  {ROOT_ID_OTA_SEARCH, PS_KEEP, OFF, SM_OFF, open_ota_search},
#if ENABLE_TTX_SUBTITLE
  {ROOT_ID_SUBT_LANGUAGE, PS_KEEP, OFF, SM_OFF, open_subt_language},
#endif
  {ROOT_ID_BOOK_LIST, PS_KEEP, OFF, SM_OFF, open_book_list},
  {ROOT_ID_OSD_SET, PS_KEEP, OFF, SM_OFF, open_osd_set},
  {ROOT_ID_SLEEP_TIMER, PS_KEEP, OFF, SM_OFF, open_sleep_timer},
  {ROOT_ID_TIMER, PS_KEEP, OFF, SM_OFF, open_timer},



  {ROOT_ID_SORT_LIST, PS_KEEP, OFF, SM_OFF, open_sort_list},
  {ROOT_ID_RENAME, PS_KEEP, OFF, SM_OFF, open_rename},
  {ROOT_ID_AUDIO_SET, PS_PLAY, ON, SM_OFF, open_audio_set},
  {ROOT_ID_AUDIO_SET_RECORD, PS_TS, ON, SM_OFF, open_audio_set},
  {ROOT_ID_SLEEP_HOTKEY, PS_KEEP, OFF, SM_OFF, open_sleep_hotkey},


  {ROOT_ID_HDD_INFO, PS_KEEP, OFF, SM_OFF, open_hdd_info},
  {ROOT_ID_STORAGE_FORMAT, PS_KEEP, OFF, SM_OFF, open_storage_format},
//  {ROOT_ID_DVR_CONFIG, PS_KEEP, OFF, SM_OFF, open_dvr_config},
  {ROOT_ID_JUMP, PS_KEEP, OFF, SM_OFF, open_jump},
//  {ROOT_ID_USB_SETTING, PS_KEEP, OFF, SM_OFF, open_usb_setting},



  {ROOT_ID_POPUP, PS_KEEP, OFF, SM_OFF, NULL},
  {ROOT_ID_PASSWORD, PS_KEEP, OFF, SM_OFF, NULL},
  {ROOT_ID_VOLUME, PS_PLAY, ON, SM_OFF, open_volume},
  {ROOT_ID_VOLUME_USB, PS_KEEP, ON, SM_OFF, open_volume_usb},
  {ROOT_ID_AUTO_SEARCH, PS_STOP, OFF, SM_BAR, open_auto_search},
  {ROOT_ID_KEYBOARD_V2, PS_KEEP, OFF, SM_OFF, open_keyboard_v2},
  {ROOT_ID_DO_SEARCH, PS_KEEP, OFF, SM_BAR, open_do_search},
  {ROOT_ID_VEPG, PS_PLAY, OFF, SM_OFF, open_vepg},
  {ROOT_ID_SMALL_LIST, PS_PLAY, OFF, SM_OFF, open_small_list_v2},
  {ROOT_ID_FIND, PS_KEEP, OFF, SM_OFF, open_find},
  {ROOT_ID_RECALL_LIST, PS_KEEP, OFF, SM_OFF, open_recall_list},


  {ROOT_ID_PROG_BAR, PS_PLAY, ON, SM_BAR, open_nprog_bar},
  {ROOT_ID_PROG_DETAIL, PS_KEEP, OFF, SM_OFF, open_prog_detail},
  {ROOT_ID_NUM_PLAY, PS_KEEP, OFF, SM_OFF, open_num_play},

  {ROOT_ID_FACTORY_TEST, PS_KEEP, OFF, SM_BAR, open_factory_test},
 // {ROOT_ID_ZOOM, PS_KEEP, OFF, SM_OFF, open_zoom},

  #if ENABLE_MUSIC_PICTURE
  {ROOT_ID_USB_MUSIC, PS_TS, OFF, SM_OFF, open_usb_music},
  {ROOT_ID_USB_MUSIC_FULLSCREEN, PS_KEEP, OFF, SM_OFF, open_usb_music_fullscreen},
  {ROOT_ID_USB_PICTURE, PS_TS, OFF, SM_OFF, open_usb_picture},
  {ROOT_ID_PIC_PLAY_MODE_SET, PS_KEEP, OFF, SM_OFF, open_pic_play_mode_set},
//  {ROOT_ID_TIMESHIFT_BAR, PS_KEEP, OFF, SM_OFF, open_timeshift_bar},
  {ROOT_ID_PICTURE, PS_KEEP, OFF, SM_OFF, open_picture},
//  {ROOT_ID_RECORD_MANAGER, PS_PREV, OFF, SM_OFF, open_record_manager},
//  {ROOT_ID_NETWORK_PLACES, PS_KEEP, OFF, SM_OFF, open_networkplaces},


//    {ROOT_ID_PVR_PLAY_BAR, PS_TS, OFF, SM_OFF, open_pvr_play_bar},
//    {ROOT_ID_PVR_REC_BAR, PS_PLAY, OFF, SM_OFF, open_pvr_rec_bar},
  #endif

  #if ENABLE_FILE_PLAY
  {ROOT_ID_USB_FILEPLAY, PS_TS, OFF, SM_OFF, ui_video_v_open},
  {ROOT_ID_FILEPLAY_BAR, PS_TS, OFF, SM_OFF, ui_video_v_fscreen_open},
  {ROOT_ID_FILEPLAY_SUBT, PS_KEEP, OFF, SM_OFF, ui_video_subt_open},
  {ROOT_ID_FP_AUDIO_SET, PS_TS, ON, SM_OFF, open_fp_audio_set},
  {ROOT_ID_VIDEO_GOTO, PS_KEEP, OFF, SM_OFF, open_video_goto},
  {ROOT_ID_TEXT_ENCODE, PS_KEEP, OFF, SM_OFF, ui_text_encode_open},
#endif

#if ENABLE_NETWORK
  //{ROOT_ID_WEATHER_FORECAST, PS_KEEP, OFF, SM_OFF, open_weather},
  {ROOT_ID_NETWORK_CONFIG_LAN, PS_KEEP, OFF, SM_OFF, open_network_config_lan},
  {ROOT_ID_NETWORK_CONFIG_WIFI, PS_KEEP, OFF, SM_OFF, open_network_config_wifi},
//  {ROOT_ID_NETWORK_CONFIG_GPRS, PS_KEEP, OFF, SM_OFF, open_network_config_gprs},
//  {ROOT_ID_NETWORK_CONFIG_3G, PS_KEEP, OFF, SM_OFF, open_network_config_3g},
  {ROOT_ID_EDIT_IP_PATH, PS_KEEP, OFF, SM_OFF, open_edit_ip_path},
//#ifndef WIN32
  {ROOT_ID_PING_TEST, PS_KEEP, OFF, SM_OFF, open_ping_test},
//#endif
 // {ROOT_ID_GOOGLE_MAP, PS_KEEP, OFF, SM_OFF, open_google_map},
#if 0
  {ROOT_ID_ONMOV_DESCRIPTION, PS_KEEP, OFF, SM_OFF, ui_open_online_movie_description},
#endif
  {ROOT_ID_VIDEO_PLAYER, PS_TS, OFF, SM_OFF, open_video_player},
  //{ROOT_ID_SUBMENU_NETWORK, PS_KEEP, OFF, SM_OFF, open_submenu_network},
#if 0
  {ROOT_ID_SUBMENU_NM, PS_KEEP, OFF, SM_OFF, open_submenu_online_movie},
#endif
//  {ROOT_ID_SATIP, PS_KEEP, OFF, SM_OFF, open_satip},
  {ROOT_ID_EDIT_USR_PWD, PS_KEEP, OFF, SM_OFF, open_edit_usr_pwd},
//  {ROOT_ID_EDIT_CITY, PS_KEEP, OFF, SM_OFF, open_edit_city},
  {ROOT_ID_WIFI, PS_KEEP, OFF, SM_OFF, ui_open_wifi},
  {ROOT_ID_WIFI_LINK_INFO, PS_KEEP, OFF, SM_OFF, open_wifi_link_info},
#if 0
#if ENABLE_NET_PLAY  
//  {ROOT_ID_NETWORK_PLAY, PS_KEEP, OFF, SM_OFF, ui_open_online_movie},
  {ROOT_ID_ONMOV_WEBSITES, PS_KEEP, OFF, SM_OFF, open_onmov_websites},
  {ROOT_ID_MOVIE_SEARCH, PS_KEEP, OFF, SM_OFF, open_movie_search},
#endif
#endif

#if ENABLE_NETMEDIA
  {ROOT_ID_NETMEDIA, PS_KEEP, OFF, SM_OFF, ui_open_netmedia},
#endif
#if ENABLE_FLICKR
#endif

  {ROOT_ID_PPPOE_CONNECT, PS_KEEP, OFF, SM_OFF, open_pppoe_connect},
#ifdef GOBY_PLUS
  {ROOT_ID_UPGRADE_BY_NETWORK, PS_KEEP, OFF, SM_OFF, open_upgrade_by_network},
#endif

 #ifdef AUPPORT_TS_RECORD
 {ROOT_ID_TS_RECORD, PS_STOP, OFF, SM_BAR, open_ts_record_tool},  
 #endif
 //{ROOT_ID_YAHOO_NEWS, PS_TS, OFF, SM_OFF, open_news},
// {ROOT_ID_NEWS_TITLE, PS_TS, OFF, SM_OFF, open_news_title},
 // {ROOT_ID_NEWS_INFO, PS_TS, OFF, SM_OFF, open_news_info},

//{ROOT_ID_PHOTO_SHOW, PS_STOP, OFF, SM_OFF, open_photo_show},
//{ROOT_ID_PHOTO_INFO, PS_STOP, OFF, SM_OFF, open_photo_info},


//{ROOT_ID_NETWORK_MUSIC, PS_KEEP, OFF, SM_OFF, open_network_music},
//{ROOT_ID_NETWORK_MUSIC_SEARCH,PS_KEEP, OFF, SM_OFF,ui_net_music_search_open},
//{ROOT_ID_SUBNETWORK_CONFIG,PS_KEEP, OFF, SM_OFF,open_submenu_networkconfig},
#endif

{ROOT_ID_LIVE_TV, PS_PLAY, OFF, SM_OFF, open_live_tv},
#if ENABLE_NETWORK
  {ROOT_ID_IPTV, PS_STOP, OFF, SM_OFF, ui_open_iptv},
  {ROOT_ID_IPTV_DESCRIPTION, PS_STOP, OFF, SM_OFF, open_iptv_description},
  {ROOT_ID_IPTV_SEARCH, PS_STOP, OFF, SM_OFF, ui_open_iptv_search},
  #ifndef WIN32
  {ROOT_ID_VDO_FAVORITE, PS_KEEP, OFF, SM_OFF, open_vdo_favorite},
  {ROOT_ID_PLAY_HIST, PS_KEEP, OFF, SM_OFF, open_play_hist},
  {ROOT_ID_IPTV_PLAYER, PS_TS, OFF, SM_OFF, ui_open_iptv_player},
  #endif

  {ROOT_ID_NET_UPG, PS_KEEP, OFF, SM_OFF, open_net_upg},

#endif
#ifdef DTMB_PROJECT  
  {ROOT_ID_TERRESTRIAL, PS_STOP, OFF, SM_OFF, open_terrestrial},
  {ROOT_ID_TERRESTRIAL_MANUAL_SEARCH, PS_STOP, OFF, SM_BAR, open_terrestrial_manual_search},
  {ROOT_ID_TERRESTRIAL_FULL_SEARCH, PS_STOP, OFF, SM_OFF, open_terrestrial_full_search},
#else
  {ROOT_ID_RANGE_SEARCH, PS_STOP, OFF, SM_BAR, open_range_search},
  {ROOT_ID_MANUAL_SEARCH, PS_STOP, OFF, SM_BAR, open_manual_search},
#endif  
#ifdef WIN32
  {ROOT_ID_CA_CARD_INFO, PS_KEEP, OFF, SM_OFF, open_ca_card_info},
  {ROOT_ID_EMAIL_MESS, PS_KEEP, OFF, SM_OFF, open_email_mess},
  {ROOT_ID_CA_PROMPT, PS_KEEP, OFF, SM_OFF, open_ca_prompt},
  {ROOT_ID_CA_ENTITLE_INFO, PS_KEEP, OFF, SM_OFF, open_ca_entitle_info},
#endif
};

#define MENU_CNT    (sizeof(all_menu_attr) / sizeof(menu_attr_t))

#define MENU_GET_ROOT_ID(idx) (all_menu_attr[idx].root_id)
#define MENU_GET_PLAY_STATE(idx) (all_menu_attr[idx].play_state)
#define MENU_GET_SIGN_TYPE(idx) (all_menu_attr[idx].signal_msg)
#define MENU_GET_CLOSE_TYPE(idx) (all_menu_attr[idx].auto_close)
#define MENU_GET_OPEN_FUNC(idx) (all_menu_attr[idx].open_func)

static const logo_attr_t all_logo_attr[] =
{
  {ROOT_ID_MAINMENU, LOGO_BLOCK_ID_M0},
  //{ROOT_ID_SUBMENU, LOGO_BLOCK_ID_M0},
};
#define LOGO_CNT    (sizeof(all_logo_attr) / sizeof(logo_attr_t))

#define LOGO_GET_ROOT_ID(idx) (all_logo_attr[idx].root_id)
#define LOGO_GET_LOGO_ID(idx) ((u8)all_logo_attr[idx].logo_id)
static preview_attr_t all_preview_attr[MENU_MAX_CNT];
static u32 totle_preview_cnk = 0; 

static const preview_attr_t public_preview_attr[] =
{
  {
    ROOT_ID_PROG_LIST,
    {
      (PREV_OFFSET_X + PLIST_MENU_X + PLIST_PREV_X),
      (PREV_OFFSET_Y + PLIST_MENU_Y + PLIST_PREV_Y),
      (PREV_OFFSET_X + PLIST_MENU_X + PLIST_PREV_X + PLIST_PREV_W),
      (PREV_OFFSET_Y + PLIST_MENU_Y + PLIST_PREV_Y + PLIST_PREV_H)
    },
  },

  {
    ROOT_ID_VEPG,
    {
      (PREV_OFFSET_X + VEPG_PREV_X),
      (PREV_OFFSET_Y + VEPG_PREV_Y),
      (PREV_OFFSET_X + VEPG_PREV_X + VEPG_PREV_W),
      (PREV_OFFSET_Y + VEPG_PREV_Y + VEPG_PREV_H)
    },
  },
#if 0

  {
    ROOT_ID_EPG_FIND,
    {
      (PREV_OFFSET_X + EPG_FIND_PREV_X),
      (PREV_OFFSET_Y + EPG_FIND_PREV_Y),
      (PREV_OFFSET_X + EPG_FIND_PREV_X + EPG_FIND_PREV_W),
      (PREV_OFFSET_Y + EPG_FIND_PREV_Y + EPG_FIND_PREV_H)
    },
  },

  {
    ROOT_ID_SCHEDULE,
    {
      (PREV_OFFSET_X + SCHEDULE_PREV_X),
      (PREV_OFFSET_Y + SCHEDULE_PREV_Y),
      (PREV_OFFSET_X + SCHEDULE_PREV_X + SCHEDULE_PREV_W),
      (PREV_OFFSET_Y + SCHEDULE_PREV_Y + SCHEDULE_PREV_H)
    },
  },
  {
    ROOT_ID_RECORD_MANAGER,
    {
      (REC_MANAGER_PREV_X + REC_TV_STYLE_SIDE),
      (REC_MANAGER_PREV_Y + REC_TV_STYLE_SIDE),
      (REC_MANAGER_PREV_X + REC_MANAGER_PREV_W - 2 * REC_TV_STYLE_SIDE),
      (REC_MANAGER_PREV_Y + REC_MANAGER_PREV_H - 2 * REC_TV_STYLE_SIDE)
    },
  },
      {
        ROOT_ID_USB_MUSIC,
        {
          (REC_MANAGER_MUSIC_PREV_X + REC_TV_STYLE_SIDE),
          (REC_MANAGER_MUSIC_PREV_Y + REC_TV_STYLE_SIDE),
          (REC_MANAGER_MUSIC_PREV_X + REC_MANAGER_MUSIC_PREV_W - 2 * REC_TV_STYLE_SIDE),
          (REC_MANAGER_MUSIC_PREV_Y + REC_MANAGER_MUSIC_PREV_H - 2 * REC_TV_STYLE_SIDE)
        },
      },
      
        {
          ROOT_ID_USB_PICTURE,
          {
            (REC_MANAGER_PREV_X + REC_TV_STYLE_SIDE),
            (REC_MANAGER_PREV_Y + REC_TV_STYLE_SIDE),
            (REC_MANAGER_PREV_X + REC_MANAGER_PREV_W - 2 * REC_TV_STYLE_SIDE),
            (REC_MANAGER_PREV_Y + REC_MANAGER_PREV_H - 2 * REC_TV_STYLE_SIDE)
          },
        },
        
#if ENABLE_FILE_PLAY
  {
    ROOT_ID_USB_FILEPLAY,
    {
      (REC_MANAGER_PREV_X ),
      (REC_MANAGER_PREV_Y ),
      (REC_MANAGER_PREV_X + REC_MANAGER_PREV_W),
      (REC_MANAGER_PREV_Y + REC_MANAGER_PREV_H)
    },
  },
#endif
  #endif

};

//#define totle_preview_cnk    (sizeof(all_preview_attr) / sizeof(preview_attr_t))

static u8 fullscreen_root[MENU_MAX_CNT] = {0};
static u32 totle_fullscreen_cnk = 0; 

static const u8 public_fullscreen_root[] =
{
  ROOT_ID_BACKGROUND,
  ROOT_ID_VOLUME,
  ROOT_ID_PROG_BAR,
  ROOT_ID_AUDIO_SET,
  ROOT_ID_MUTE,
  ROOT_ID_PAUSE,
  ROOT_ID_SIGNAL,
  ROOT_ID_NOTIFY,
//  ROOT_ID_PVR_REC_BAR,
//  ROOT_ID_PVR_PLAY_BAR
//  ROOT_ID_NUM_PLAY,
 // ROOT_ID_VOLUME_USB,
  ROOT_ID_FUN_HELP,
};

#define media_root_cnk    (sizeof(media_root) / sizeof(u8))
static const u8 media_root[] =
{
ROOT_ID_USB_MUSIC,
ROOT_ID_USB_MUSIC_FULLSCREEN,
ROOT_ID_USB_PICTURE,
ROOT_ID_USB_FILEPLAY,
ROOT_ID_FILEPLAY_BAR,
};

#define vod_root_cnk    (sizeof(vod_root) / sizeof(u8))
static const u8 vod_root[] =
{
	ROOT_ID_IPTV,
	ROOT_ID_VDO_FAVORITE,
	ROOT_ID_PLAY_HIST,
	ROOT_ID_IPTV_DESCRIPTION,
	ROOT_ID_IPTV_PLAYER,
};

//#define totle_fullscreen_cnk    (sizeof(fullscreen_root) / sizeof(u8))

static u8 popup_root[] =
{
  ROOT_ID_POPUP,
  ROOT_ID_PASSWORD,
  ROOT_ID_SORT_LIST,
  ROOT_ID_FAV_SET,
};

#define POPUP_CNT    (sizeof(popup_root) / sizeof(u8))

BOOL ui_is_media_menu(u8 root_id)
{
	u8 i;
	
	for(i = 0; i < media_root_cnk; i++)
	{
	  if(media_root[i] == root_id)
	  {
		return TRUE;
	  }
	}
	return FALSE;
}

BOOL ui_is_vod_menu(u8 root_id)
{
	u8 i;
	
	for(i = 0; i < vod_root_cnk; i++)
	{
	  if(vod_root[i] == root_id)
	  {
		return TRUE;
	  }
	}
	return FALSE;
}

BOOL ui_is_preview_menu(u8 root_id)
{
  u8 i;

  for(i = 0; i < totle_preview_cnk; i++)
  {
    if(all_preview_attr[i].root_id == root_id)
    {
      return TRUE;
    }
  }
  return FALSE;
}

BOOL ui_is_music_menu(u8 root_id)
{
  if(ROOT_ID_USB_MUSIC == root_id)
  {
    return TRUE;
  }
  return FALSE;
}

BOOL ui_is_picture_menu(u8 root_id)
{
  if(ROOT_ID_USB_PICTURE== root_id)
  {
    return TRUE;
  }
  return FALSE;
}

#if ENABLE_FILE_PLAY
BOOL ui_is_video_menu(u8 root_id)
{
  if(ROOT_ID_FILEPLAY_BAR == root_id)
  {
    return TRUE;
  }
  return FALSE;
}
#endif
u8 ui_get_preview_menu(void)
{
  u8 i, root_id;

  for(i = 0; i < totle_preview_cnk; i++)
  {
    root_id = all_preview_attr[i].root_id;
    if(fw_find_root_by_id(root_id) != NULL)
    {
      return root_id;
    }
  }
  return ROOT_ID_INVALID;
}


//Can show mute /signal window at the same time
//Other windows can be opened
BOOL ui_is_fullscreen_menu(u8 root_id)
{
  u8 i;

  for(i = 0; i < totle_fullscreen_cnk; i++)
  {
    if(fullscreen_root[i] == root_id)
    {
      return TRUE;
    }
  }
  return FALSE;
}


BOOL ui_is_popup_menu(u8 root_id)
{
  u8 i;

  for(i = 0; i < POPUP_CNT; i++)
  {
    if(popup_root[i] == root_id)
    {
      return TRUE;
    }
  }
  return FALSE;
}


static RET_CODE manage_tmr_create(void)
{
  osd_set_t osd_set;

  sys_status_get_osd_set(&osd_set);

  //OS_PRINTF("-====================manage create timer, %d\n", osd_set.timeout);

  return fw_tmr_create(ROOT_ID_BACKGROUND, MSG_CLOSE_MENU,
                       osd_set.timeout * 1000, FALSE);
}


static RET_CODE manage_tmr_destroy(void)
{
  return fw_tmr_destroy(ROOT_ID_BACKGROUND, MSG_CLOSE_MENU);
}


RET_CODE manage_tmr_reset(void)
{
#if 0
  manage_tmr_destroy();
  return manage_tmr_create();
#else
  osd_set_t osd_set;

  sys_status_get_osd_set(&osd_set);

  if(fw_tmr_reset(ROOT_ID_BACKGROUND, MSG_CLOSE_MENU,
                  osd_set.timeout * 1000) != SUCCESS)
  {
    return manage_tmr_create();
  }


  return SUCCESS;
#endif
}
static void ui_add_to_all_fullscreen_menu(void)
{
#ifdef ENABLE_CA
  cas_manager_policy_t *handle = NULL;
  memset(fullscreen_root,0,sizeof(fullscreen_root));
  memcpy(fullscreen_root,public_fullscreen_root,sizeof(public_fullscreen_root));

  handle = get_cas_manager_policy_handle();
  if(NULL == handle)
  {
    totle_fullscreen_cnk = sizeof(public_fullscreen_root) / sizeof(u8) ;
    MT_ASSERT(totle_fullscreen_cnk <= MENU_MAX_CNT);
  }
  else
  {
    totle_fullscreen_cnk = sizeof(public_fullscreen_root) / sizeof(u8) 
                                        + handle->ca_fullscreen_root_number;
    MT_ASSERT(totle_fullscreen_cnk <= MENU_MAX_CNT);
    memcpy((u8 *)fullscreen_root+sizeof(public_fullscreen_root),
                handle->ca_fullscreen_root,sizeof(u8) * handle->ca_fullscreen_root_number);
  }
#else
  memset(fullscreen_root,0,sizeof(fullscreen_root));
  memcpy(fullscreen_root,public_fullscreen_root,sizeof(public_fullscreen_root));
  totle_fullscreen_cnk = sizeof(public_fullscreen_root) / sizeof(u8) ;
  MT_ASSERT(totle_fullscreen_cnk <= MENU_MAX_CNT);
#endif
}

static void ui_add_to_all_menu(void)
{
#ifdef ENABLE_CA
  cas_manager_policy_t *handle = NULL;

  memset(all_menu_attr,0,sizeof(all_menu_attr));
  memcpy(all_menu_attr,public_menu_attr,sizeof(public_menu_attr));

  handle = get_cas_manager_policy_handle();
  if(NULL == handle)
  {
    totle_menu_cnk = sizeof(public_menu_attr) / sizeof(menu_attr_t) ;
    MT_ASSERT(totle_menu_cnk <= MENU_MAX_CNT);
  }
  else
  {
    totle_menu_cnk = sizeof(public_menu_attr) / sizeof(menu_attr_t) + handle->attr_number;
    MT_ASSERT(totle_menu_cnk <= MENU_MAX_CNT);
    memcpy((u8 *)all_menu_attr+sizeof(public_menu_attr),
    handle->ca_menu_attr,sizeof(menu_attr_t) * handle->attr_number);
  }
#else
  memset(all_menu_attr,0,sizeof(all_menu_attr));
  memcpy(all_menu_attr,public_menu_attr,sizeof(public_menu_attr));
  totle_menu_cnk = sizeof(public_menu_attr) / sizeof(menu_attr_t) ;
  MT_ASSERT(totle_menu_cnk <= MENU_MAX_CNT);
#endif
}

static void ui_add_all_preview(void)
{
#ifdef ENABLE_CA
  cas_manager_policy_t *handle = NULL;

  memset(all_preview_attr,0,sizeof(all_preview_attr));
  memcpy(all_preview_attr,public_preview_attr,sizeof(public_preview_attr));

  handle = get_cas_manager_policy_handle();
  if(NULL == handle)
  {
    totle_preview_cnk = sizeof(public_preview_attr) / sizeof(preview_attr_t) ;
    MT_ASSERT(totle_preview_cnk <= MENU_MAX_CNT);
  }
  else
  {
    totle_preview_cnk = sizeof(public_preview_attr) / sizeof(preview_attr_t) + handle->preview_attr_number;
    MT_ASSERT(totle_preview_cnk <= MENU_MAX_CNT);
    memcpy((u8 *)all_preview_attr+sizeof(public_preview_attr),
    handle->ca_preview_attr,sizeof(preview_attr_t) * handle->preview_attr_number);
  }

#else 
  memset(all_preview_attr,0,sizeof(all_preview_attr));
  memcpy(all_preview_attr,public_preview_attr,sizeof(public_preview_attr));
  totle_preview_cnk = sizeof(public_preview_attr) / sizeof(preview_attr_t) ;
  MT_ASSERT(totle_preview_cnk <= MENU_MAX_CNT);
#endif
}


static u8 manage_find_menu(u8 root_id)
{
  u8 i;

  for(i = 0; i < MENU_CNT; i++)
  {
    if(MENU_GET_ROOT_ID(i) == root_id)
    {
      return i;
    }
  }
  return INVALID_IDX;
}


static u8 manage_find_logo(u8 root_id)
{
  u8 i;

  for(i = 0; i < LOGO_CNT; i++)
  {
    if(LOGO_GET_ROOT_ID(i) == root_id)
    {
      return i;
    }
  }
  return INVALID_IDX;
}


u8 manage_find_preview(u8 root_id, rect_t *orc)
{
  u8 i;
//  s16 x = 0, y = 0;
  disp_sys_t std;

  empty_rect(orc);
  avc_video_switch_chann(class_get_handle_by_id(AVC_CLASS_ID), DISP_CHANNEL_HD);
  std = avc_get_video_mode_1(class_get_handle_by_id(AVC_CLASS_ID));
  OS_PRINTF("UI: swithced to PREV........std=%d\n", std);

  for(i = 0; i < totle_preview_cnk; i++)
  {
    if(all_preview_attr[i].root_id == root_id)
    {
      copy_rect(orc, &all_preview_attr[i].position);
      /* convert coordinate to screen */
     /* switch(std)
      {
        case VID_SYS_NTSC_J:
        case VID_SYS_NTSC_M:
        case VID_SYS_NTSC_443:
        case VID_SYS_PAL_M:
          x = SCREEN_POS_NTSC_L, y = SCREEN_POS_NTSC_T;
          break;
        case VID_SYS_PAL_N:
        case VID_SYS_PAL:
        case VID_SYS_PAL_NC:
          x = SCREEN_POS_PAL_L, y = SCREEN_POS_PAL_T;
          break;
        default:

          break;
      }
      offset_rect(orc, x, y);*/

   #ifndef WIN32
      switch(std)
      {
        case VID_SYS_1080I:
        case VID_SYS_1080I_50HZ:
        case VID_SYS_1080P:
        case VID_SYS_1080P_24HZ:
        case VID_SYS_1080P_25HZ:
        case VID_SYS_1080P_30HZ:
        case VID_SYS_1080P_50HZ:
          //to scale video
          orc->left = orc->left * 1920 / 1280;
          orc->right = orc->right * 1920 / 1280;
          orc->top = orc->top * 1080 / 720;
          orc->bottom = orc->bottom * 1080 / 720;
          break;

        case VID_SYS_NTSC_J:
        case VID_SYS_NTSC_M:
        case VID_SYS_NTSC_443:
        case VID_SYS_PAL_M:
        case VID_SYS_480P:
          orc->left = orc->left * 720 / 1280;
          orc->right = orc->right * 720 / 1280;
          orc->top = orc->top * 480 / 720;
          orc->bottom = orc->bottom * 480 / 720;
          break;

        case VID_SYS_PAL_N:
        case VID_SYS_PAL:
        case VID_SYS_PAL_NC:
        case VID_SYS_576P_50HZ:
          orc->left = orc->left * 720 / 1280;
          orc->right = orc->right * 720 / 1280;
          orc->top = orc->top * 576 / 720;
          orc->bottom = orc->bottom * 576 / 720;
          break;

        default:
          break;
      }
  #endif
      return i;
    }
  }
  return INVALID_IDX;
}


BOOL manage_get_preview_rect(u8 root_id,
                             u16 *left,
                             u16 *top,
                             u16 *width,
                             u16 *height)
{
  u8 i;

  for(i = 0; i < totle_preview_cnk; i++)
  {
    if(all_preview_attr[i].root_id == root_id)
    {
      *left = all_preview_attr[i].position.left;
      *top = all_preview_attr[i].position.top;
      *width = RECTW(all_preview_attr[i].position);
      *height = RECTH(all_preview_attr[i].position);
      return TRUE;
    }
  }
  return FALSE;
}


//we should make all background feature work well after register manager
void manage_init(void)
{
  manage_tmr_create();
}


//special : for change the timing of show logo and show OSD
void manage_logo2preview(u8 root_id)
{
  u8 idx;
  rect_t orc;

  idx = manage_find_logo(root_id);
  if(idx != INVALID_IDX)
  {
    ui_show_logo(LOGO_GET_LOGO_ID(idx));
  }
  idx = manage_find_preview(root_id, &orc);
  if(idx != INVALID_IDX)
  {
    avc_video_switch_chann(class_get_handle_by_id(AVC_CLASS_ID), DISP_CHANNEL_HD);
    avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                        orc.left, orc.top, orc.right, orc.bottom);
    avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                         orc.left, orc.top, orc.right, orc.bottom);
  }
//  av_video_output_enable();
}


static u8 _get_menu_type(u8 root_id)
{
  if(ui_is_fullscreen_menu(root_id))
  {
    return MENU_TYPE_FULLSCREEN;
  }
  else if(ui_is_preview_menu(root_id))
  {
    return MENU_TYPE_PREVIEW;
  }
  else if(ui_is_popup_menu(root_id))
  {
    return MENU_TYPE_POPUP;
  }
   else if(ui_is_music_menu(root_id))
  {
    return MENU_TYPE_MUSIC;
  }
  else if(ui_is_picture_menu(root_id))
  {
    return MENU_TYPE_PICTURE;
  }
  #if ENABLE_FILE_PLAY
  else if(ui_is_video_menu(root_id))
  {
    return MENU_TYPE_VIDEO;
  }
  #endif
  else
  {
    return MENU_TYPE_NORMAL;
  }
}


void manage_enable_autoswitch(BOOL is_enable)
{
  g_menu_manage.enable_autoswitch = is_enable;
}


static void _switch_playstatus(u8 from_id, u8 to_id)
{
  u8 idx, new_ps;
  rect_t orc;
  menu_attr_t *p_curn_attr;

  p_curn_attr = &g_menu_manage.curn_menu_attr;

  idx = manage_find_menu(to_id);
  if(idx == INVALID_IDX)
  {
    return;
  }
  else
  {
    new_ps = MENU_GET_PLAY_STATE(idx);
  }
  if(new_ps == PS_KEEP)
  {
    return;
  }

  if(new_ps == CURN_MENU_PLAY_STATE)
  {
    if(new_ps == PS_LOGO)
    {
      ui_set_front_panel_by_str("----");
    }
    return;
  }

  if(from_id == ROOT_ID_USB_MUSIC
  	&&to_id == ROOT_ID_USB_MUSIC_FULLSCREEN)
  {//work around for bug 30144
    CURN_MENU_PLAY_STATE = new_ps;
    return;
  }
  //we should auto switch logo
  switch(new_ps)
  {
    case PS_PREV:
      idx = manage_find_logo(to_id);
      switch(CURN_MENU_PLAY_STATE)
      {
        case PS_LOGO:
          ui_stop_play(STOP_PLAY_BLACK, TRUE);
          idx = manage_find_preview(to_id, &orc);
          if(idx != INVALID_IDX)
          {
            OS_PRINTF("UI: swithced to PREV........PS_LOGO\n");
            avc_video_switch_chann(class_get_handle_by_id(AVC_CLASS_ID), DISP_CHANNEL_HD);
            avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                                orc.left, orc.top, orc.right, orc.bottom);
            avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                                 orc.left, orc.top, orc.right, orc.bottom);

            OS_PRINTF("UI: swithced to PREV........\n");
          }
          break;
        default:
          idx = manage_find_preview(to_id, &orc);
          OS_PRINTF("avc-------x = %d\n-------y = %d\n-------w = %d\n-------h = %d\n",
                    orc.left, orc.top, orc.right, orc.bottom);
          if(idx != INVALID_IDX)
          {
            avc_video_switch_chann(class_get_handle_by_id(AVC_CLASS_ID), DISP_CHANNEL_HD);
            avc_enter_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                                orc.left, orc.top, orc.right, orc.bottom);
            avc_config_preview_1(class_get_handle_by_id(AVC_CLASS_ID),
                                 orc.left, orc.top, orc.right, orc.bottom);
            OS_PRINTF("UI: swithced to PREV........\n");
          }
      }
      break;
    case PS_PLAY:
      OS_PRINTF("UI: switch to PLAY.......... to_id:%d, CURN_MENU_PLAY_STATE:%d\n", 
      												to_id, CURN_MENU_PLAY_STATE);
      switch(CURN_MENU_PLAY_STATE)
      {
        case PS_PREV:
          avc_leave_preview_1(class_get_handle_by_id(AVC_CLASS_ID));
          ui_play_curn_pg();
          break;
        case PS_LOGO:
          if((to_id == ROOT_ID_PROG_LIST)
            && (sys_status_get_curn_prog_mode() == CURN_MODE_RADIO))
          {
            //ui_show_logo(LOGO_BLOCK_ID_M1);
            ui_show_logo(LOGO_BLOCK_ID_M0); //LOGO_BLOCK_ID_M2
          }
          else
          {
            ui_play_curn_pg();
          }

          break;
        default:
            if((to_id != ROOT_ID_LIVE_TV))
            {
				ui_play_curn_pg();
            }
            else
            {
                OS_PRINTF("#############switch to TS PLAY..livetv.....##########\n");
            }
          break;
      }
      break;
    case PS_TS:
      DEBUG(MAIN, INFO, "UI: switch to TS PLAY.......... CURN_MENU_PLAY_STATE:%d\n", CURN_MENU_PLAY_STATE);
      switch(CURN_MENU_PLAY_STATE)
      {
        case PS_PREV:
          avc_leave_preview_1(class_get_handle_by_id(AVC_CLASS_ID));
          break;
        default:
          break;
      }
      break;
    case PS_LOGO:
      ui_set_front_panel_by_str("----");
      OS_PRINTF("UI: switch to LOGO..........\n");
      idx = manage_find_logo(to_id);
      switch(CURN_MENU_PLAY_STATE)
      {
        case PS_PREV:
          ui_stop_play(STOP_PLAY_BLACK, TRUE);
          avc_leave_preview_1(class_get_handle_by_id(AVC_CLASS_ID));

          if(idx != INVALID_IDX)
          {
            ui_show_logo(LOGO_GET_LOGO_ID(idx));
          }
          break;
        case PS_PLAY:
          ui_stop_play(STOP_PLAY_BLACK, TRUE);
          if(idx != INVALID_IDX)
          {
            ui_show_logo(LOGO_GET_LOGO_ID(idx));
          }
          break;
        default:
          if(idx != INVALID_IDX)
          {
            ui_show_logo(LOGO_GET_LOGO_ID(idx));
          }
      }
      break;
    case PS_STOP:
      ui_set_front_panel_by_str("----");
      /* just stop playing */
      OS_PRINTF("UI: switch to STOP..........\n");
      
      if(ui_get_play_prg_type() == SAT_PRG_TYPE)
      {
        ui_stop_play(STOP_PLAY_BLACK, TRUE);
      }
      else if(ui_get_play_prg_type() == NET_PRG_TYPE)
      {
          ui_video_c_stop();
      }
      
      if(PS_PREV == CURN_MENU_PLAY_STATE)
      {
        avc_leave_preview_1(class_get_handle_by_id(AVC_CLASS_ID));
      }

      /* stop monitor */
//      dvbs_stop_monitor();
      break;

    default:
      /* do nothing */;
  }

  CURN_MENU_PLAY_STATE = new_ps;
}


static void _switch_signalcheck(u8 from_id, u8 to_id)
{
  u8 idx, new_sm;

  idx = manage_find_menu(to_id);
  if(idx == INVALID_IDX)
  {
    new_sm = SM_OFF;
  }
  else
  {
    new_sm = MENU_GET_SIGN_TYPE(idx);
  }

  CURN_MENU_SIGN_TYPE = new_sm;
}

BOOL _is_find_media_menu(void)
{
	u8 i;
	for(i = 0; i < media_root_cnk; i++)
	{
	  if(NULL!=fw_find_root_by_id(media_root[i]))
	  {
		return TRUE;
	  }
	}
	return FALSE;
}
BOOL _is_find_vod_menu(void)
{
	u8 i;
	for(i = 0; i < vod_root_cnk; i++)
	{
	  if(NULL!=fw_find_root_by_id(vod_root[i]))
	  {
		return TRUE;
	  }
	}
	return FALSE;
}

static void _switch_autoopen(u8 from_id, u8 to_id)
{
  u8 old_status, new_status;

  BOOL is_chk_mute = FALSE;
  BOOL is_chk_pause = FALSE;
  BOOL is_chk_notify = FALSE;
  BOOL is_chk_signal = FALSE;
  BOOL b_time_shift = FALSE;
  BOOL is_need_help = TRUE;
  BOOL is_chk_email = FALSE;
  old_status = _get_menu_type(from_id);
  new_status = _get_menu_type(to_id);

  if(new_status != old_status
    && old_status != MENU_TYPE_POPUP)
  {
    switch(new_status)
    {
      case MENU_TYPE_FULLSCREEN:
        is_chk_mute = is_chk_pause = is_chk_notify = is_chk_signal = is_chk_email = TRUE;
        OS_PRINTF("@@@MENU_TYPE_FULLSCREEN\n");
        break;
      case MENU_TYPE_PREVIEW:
#if ENABLE_FILE_PLAY
        if(ROOT_ID_USB_FILEPLAY == to_id)
        {
          is_chk_mute = TRUE;
        }
        else
#endif
        {
          is_chk_mute = is_chk_pause = TRUE;
        }
		if(ROOT_ID_VEPG!=to_id
          &&ROOT_ID_USB_FILEPLAY!=to_id
          &&ROOT_ID_USB_MUSIC!=to_id
          &&ROOT_ID_USB_PICTURE!=to_id
          &&ROOT_ID_FILEPLAY_BAR!=to_id)
      	        is_need_help = FALSE;
        OS_PRINTF("@@@MENU_TYPE_PREVIEW\n");
        break;
#if ENABLE_FILE_PLAY
      case MENU_TYPE_VIDEO:
      case MENU_TYPE_MUSIC:
      case MENU_TYPE_PICTURE:
                    is_chk_mute = TRUE;
			is_need_help = TRUE;
            OS_PRINTF("@@@MENU_TYPE_VIDEO  MENU_TYPE_MUSIC MENU_TYPE_PICTURE \n");
            break;
#endif
      default:
      	if(ROOT_ID_PROG_BAR!=to_id
      				&&ROOT_ID_SMALL_LIST!=to_id
					&&ROOT_ID_XEXTEND!=to_id
      				&&ROOT_ID_VOLUME!=to_id
      				&&ROOT_ID_XSMALL_INFO!=to_id
      				&&ROOT_ID_USB_MUSIC_FULLSCREEN != to_id
      				&&ROOT_ID_FILEPLAY_BAR != to_id
      				&&ROOT_ID_PICTURE != to_id)
      	{
			is_need_help = FALSE;
      	}
        OS_PRINTF("@@@default \n");
        break;
    }
  }

  if(ui_is_mute()
    && is_chk_mute)
  {
    open_mute(0, 0);
  }

  if(ui_is_pause()
    && is_chk_pause)
  {
    open_pause(0, 0);
  }

  if(ui_is_notify()
    && is_chk_notify)
  {
    open_notify(0, 0);
  }
  if(!is_need_help)
  {
    if(NULL!=fw_find_root_by_id(ROOT_ID_FUN_HELP))
    {
        manage_close_menu(ROOT_ID_FUN_HELP, 0,0);
        OS_PRINTF("@@@ to_id = %d\n",to_id);
        OS_PRINTF("@@@%s\n", __FUNCTION__);
        OS_PRINTF("@@@\n");
    }
  }

  if((from_id == ROOT_ID_PASSWORD && to_id == ROOT_ID_PROG_BAR)
	|| (from_id == ROOT_ID_PASSWORD && to_id == ROOT_ID_BACKGROUND))//for bug 36851
  {
    is_chk_signal = TRUE;
  }
  if(_is_find_media_menu()||_is_find_vod_menu())
  {
	is_chk_signal = FALSE;
  }
  if(is_chk_signal)
  {
  	DEBUG(DBG, INFO, "from_id:%d, to_id:%d\n", from_id, to_id);
    update_signal();
  }

  if((to_id == ROOT_ID_BACKGROUND)||(to_id == ROOT_ID_PROG_BAR))
  {
  }
  else if(to_id == ROOT_ID_MAINMENU && from_id != ROOT_ID_MAINMENU)
  {
  }
  
#ifdef ENABLE_CA
	if(is_chk_email)
	{
	  cas_manage_process_menu(CA_OPEN_EMAIL, NULL, 0, 0);
        cas_manage_process_menu(CA_SHOW_ROLL_ON_DESKTOP, NULL, 0, 0);
	  cas_manage_process_menu(CA_OPEN_CURTAIN, NULL, 0, 0);
	}
#endif
  if((to_id != ROOT_ID_POPUP2)
  	&&(from_id != ROOT_ID_XEXTEND)
    &&(to_id == ROOT_ID_BACKGROUND)
    && (old_status != MENU_TYPE_FULLSCREEN)
    && (fw_get_focus_id() == ROOT_ID_BACKGROUND))
  {
    sys_status_get_status(BS_DVR_ON, &b_time_shift);
    if(!b_time_shift)
    {
      manage_open_menu(ROOT_ID_PROG_BAR, 0, 0);
    }
  }
}


static void _switch_autoclose(u8 from_id, u8 to_id)
{
  u8 idx, new_auto;
  u8 old_status, new_status;

  old_status = _get_menu_type(from_id);
  new_status = _get_menu_type(to_id);

  if((old_status == MENU_TYPE_FULLSCREEN
     || old_status == MENU_TYPE_PREVIEW
     || old_status == MENU_TYPE_MUSIC
     || old_status == MENU_TYPE_PICTURE
)
    && (old_status != new_status) /*preview or normal*/
    && new_status != MENU_TYPE_POPUP)
  {
    if(ui_is_mute() && (to_id != ROOT_ID_PVR_REC_BAR))
    {
      close_mute();
    }
    if(ui_is_pause())
    {
      close_pause();
    }
    if(ui_is_notify())
    {
      close_notify();
    }

#ifdef ENABLE_CA
    cas_manage_process_menu(CA_CLOSE_EMAIL, NULL, 0, 0);
	if(to_id != ROOT_ID_DO_SEARCH)
	  cas_manage_process_menu(CA_HIDE_ROLL_ON_DESKTOP, NULL, 0, 0);
#endif
    /* try to close */
    close_signal();
  }

  idx = manage_find_menu(to_id);
  if(idx == INVALID_IDX)
  {
    new_auto = OFF;
  }
  else
  {
    new_auto = MENU_GET_CLOSE_TYPE(idx);
  }

  UI_PRINTF("switch_autoclose: from %d to %d \n", from_id, to_id);

  if(CURN_MENU_CLOSE_TYPE == OFF)
  {
    if(new_auto == OFF)
    {
      PRE_AUTOCLOSE_ROOT = INVALID_IDX;
      return;
    }
    else
    {
      PRE_AUTOCLOSE_ROOT = to_id;
      UI_PRINTF("switch_autoclose: create tmr \n");
      manage_tmr_create();
    }
  }
  else
  {
    if(new_auto == OFF)
    {
      PRE_AUTOCLOSE_ROOT = INVALID_IDX;
      UI_PRINTF("switch_autoclose: destroy tmr \n");
      manage_tmr_destroy();
    }
    else
    {
      PRE_AUTOCLOSE_ROOT = to_id;
      UI_PRINTF("switch_autoclose: reset tmr \n");
      manage_tmr_reset();
    }
  }

  CURN_MENU_CLOSE_TYPE = new_auto;
}


void manage_auto_switch(void)
{
  focus_change_info_t *p_info = &g_menu_manage.pre_focus_change;

  if(p_info->is_changed)
  {
    p_info->is_changed = FALSE;
/* move to ui_menu_manger_proc
    _switch_playstatus(p_info->from_id, p_info->to_id);
    _switch_autoclose(p_info->from_id, p_info->to_id);*/
    _switch_autoopen(p_info->from_id, p_info->to_id);
    _switch_signalcheck(p_info->from_id, p_info->to_id);
  }
}


RET_CODE manage_open_menu(u8 root_id, u32 para1, u32 para2)
{
  open_menu_t open_func;
  u8 idx;
  RET_CODE ret;

  idx = manage_find_menu(root_id);
  if(idx == INVALID_IDX)
  {
    return ERR_NOFEATURE;
  }

  open_func = MENU_GET_OPEN_FUNC(idx);
  if((open_func == NULL))
  {
    return ERR_NOFEATURE;
  }

  ret = (*open_func)(para1, para2);
  if(ret == SUCCESS)
  {
    manage_auto_switch();
  }
  return ret;
}


RET_CODE manage_close_menu(u8 root_id, u32 para1, u32 para2)
{
  if(!fw_destroy_mainwin_by_id(root_id))
  {
    return ERR_FAILURE;
  }

  OS_PRINTF("@@@@@@###%s:root_id = %d ",__FUNCTION__,root_id);
  manage_auto_switch();
  return SUCCESS;
}


RET_CODE manage_back_to_menu(u8 root_id, u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;

  if(fw_find_root_by_id(root_id) != NULL)
  {
    while(fw_get_focus_id() != root_id)
    {
      if(!fw_destroy_mainwin_by_id(fw_get_focus_id()))
      {
        ret = ERR_FAILURE;
        break;
      }
    }

    manage_enable_autoswitch(FALSE);
    fw_destroy_mainwin_by_id(root_id);
    manage_enable_autoswitch(TRUE);
    manage_open_menu(root_id, para1, para2);
    //ctrl_paint_ctrl(fw_get_focus(), TRUE);

    ret = SUCCESS;
  }
  else
  {
    manage_open_menu(root_id, para1, para2);
    ret = SUCCESS;
  }

  return ret;
}


RET_CODE manage_notify_root(u8 root_id, u16 msg, u32 para1, u32 para2)
{
  control_t *p_root;

  p_root = fw_find_root_by_id(root_id);
  if(p_root != NULL)
  {
    return ctrl_process_msg(p_root, msg, para1, para2);
  }

  return ERR_NOFEATURE;
}


void manage_autoclose(void)
{
  u8 root_id;
  control_t *p_obj;

  root_id = PRE_AUTOCLOSE_ROOT;

  p_obj = fw_find_root_by_id(root_id);
  if(NULL == p_obj)
  {
    return;
  }

  /* add patch to keep prog bar on radio mode. */
  if(root_id == ROOT_ID_PROG_BAR
    && sys_status_get_curn_prog_mode() == CURN_MODE_RADIO)
  {
    return;
  }

  if(ctrl_process_msg(p_obj, MSG_EXIT, 0, 0) != SUCCESS)
  {
    fw_destroy_mainwin_by_root(p_obj);
  }
  if(ROOT_ID_PROG_BAR==root_id)
  	close_fun_help_later();
}


menu_attr_t *manage_get_curn_menu_attr(void)
{
  return &g_menu_manage.curn_menu_attr;
}


static void _pre_enter_preview(u8 root_id, u32 para1, u32 para2)
{
  u8 curn_mode = sys_status_get_curn_prog_mode();

  switch(root_id)
  {
    case ROOT_ID_PROG_LIST:
      if(((curn_mode == CURN_MODE_TV)
         && (para1 == 1))                                  //enter tv list on radio mode.
        || ((curn_mode == CURN_MODE_RADIO)
           && (para1 == 0)))                               //enter radio list on tv mode.
      {
        ui_enable_video_display(FALSE);
      }
      break;
    default:
      /* do nothing */;
  }
}


static void _before_open_menu(u8 old_root, u8 new_root, u32 para1, u32 para2)
{
  u8 old_status, new_status;

  old_status = _get_menu_type(old_root);
  new_status = _get_menu_type(new_root);

  if(new_status == MENU_TYPE_PREVIEW)
  {
    _pre_enter_preview(new_root, para1, para2);
  }
}


static void _before_close_menu(u8 root_id)
{
  /*
     u8 status = _get_menu_type(root_id);

     if(status == MENU_TYPE_PREVIEW)
     {
     close_signal();
     }
    */
}


void ui_close_all_mennus(void)
{
  u8 to = ROOT_ID_INVALID, from = fw_get_focus_id();

  manage_enable_autoswitch(FALSE);
  to = fw_destroy_all_mainwin(FALSE);
  manage_enable_autoswitch(TRUE);

  ui_menu_manage(WINDOW_MANAGE_FOCUS_CHANGED, from, to);
  fw_paint_all_mainwin();

#ifdef ENABLE_ADS
  pic_adv_stop();
#endif

  manage_auto_switch();
#ifdef ENABLE_CA
  cas_manager_reroll_after_close_all_menu();
#endif

}

#if 0
static RET_CODE ui_open_netmedia(u32 para1, u32 para2)
{
    RET_CODE ret = ERR_FAILURE;
    NETMEDIA_DP_TYPE type;
    type = (NETMEDIA_DP_TYPE)para1;

    switch (type)
    {
        case NETMEDIA_DP_VIMEO:
            ret = ui_open_nm_simple(para1, para2);
            break;
        case NETMEDIA_DP_ONLINE_MOVIES:

            break;
        case NETMEDIA_DP_DAILYMOTION:
            ret = ui_open_nm_simple(para1, para2);
            break;
        case NETMEDIA_DP_ALJAZEER:
            ret = ui_open_nm_simple(para1, para2);
            break;
        default:
            break;
    }
    return ret;
}
#endif


static RET_CODE on_initialize(u32 event, u32 para1, u32 para2)
{
  // start app framework
  ui_add_to_all_menu();
  ui_add_to_all_fullscreen_menu();
  ui_add_all_preview();
#ifdef ENABLE_CA
	if(CAS_ID  != CONFIG_CAS_ID_DS)
	{
	  ui_init_ca();
	}
#endif
  OS_PRINTF("menu manager inital\n");
  ap_frm_set_ui_msgq_id((s32)para1);

  OS_PRINTF("menu manager inital 1\n");


  ap_frm_run();
  OS_PRINTF("menu manager inital 2\n");

  ui_enable_uio(TRUE);
  ui_enable_playback(TRUE);

  ui_time_init();

  ui_epg_init();
  /* activate EPG */
#ifndef IMPL_NEW_EPG
  ui_epg_start();
#else
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#endif
  // open signal monitor
  ui_init_signal();
  return SUCCESS;
}


static RET_CODE on_pre_open(u32 event, u32 para1, u32 para2)
{
  u8 new_root = (u8)(para1);

  _before_open_menu(CURN_MENU_ROOT_ID, new_root, para1, para2);

  return SUCCESS;
}


static RET_CODE on_pre_close(u32 event, u32 para1, u32 para2)
{
  u8 old_root = (u8)(para1);

  _before_close_menu(old_root);

  return SUCCESS;
}

#ifdef ENABLE_ADS
static void manage_ad_focus_changed(u8 old_root, u8 new_root)
{
	control_t *p_root = NULL;

	if(old_root != new_root)
	{
		switch(old_root)
		{
			case ROOT_ID_BACKGROUND:
				if(!ui_is_fullscreen_menu(new_root))
				{
					pic_adv_stop();
				}
					
				break;
			case ROOT_ID_MAINMENU:
				pic_adv_stop();
				break;
			default:
				break;
		}
		switch(new_root)
		{
			case ROOT_ID_BACKGROUND:
			case ROOT_ID_MAINMENU:
				p_root = fw_find_root_by_id(new_root);
				if(NULL !=	p_root)
				{
					fw_notify_root(p_root, NOTIFY_T_MSG, TRUE, MSG_REFRESH_ADS_PIC, 0, 0);
				}
				break;
			default:
				break;
		}
	}
}
#endif

static RET_CODE on_focus_changed(u32 event, u32 para1, u32 para2)
{
	focus_change_info_t *p_info;
	u8 old_root, new_root;
	BOOL  g_is_lock = TRUE;

	old_root = (u8)(para1);
	new_root = (u8)(para2);

	if(g_menu_manage.enable_autoswitch)
	{
		p_info = &g_menu_manage.pre_focus_change;

		if(p_info->is_changed)
		{
			/* just update to */
			p_info->to_id = new_root;
		}
		else
		{
			/* start to save */
			p_info->is_changed = TRUE;
			p_info->from_id = old_root;
			p_info->to_id = new_root;
		}
		if(p_info->to_id == ROOT_ID_INSTALLATION_LIST)
		{
			ui_enable_signal_monitor_by_tuner(TRUE, 0);

			ui_set_frontpanel_by_curn_pg();

			g_is_lock =  ui_signal_is_lock();

			fw_tmr_create(ROOT_ID_INSTALLATION_LIST, MSG_BEEPER, g_is_lock ? BEEPER_LOCKD_TMOUT :
			BEEPER_UNLKD_TMOUT, TRUE);
		}
		/* because paint menu is too slow, so move it from auto_switch */
		_switch_playstatus(p_info->from_id, p_info->to_id);
		_switch_autoclose(p_info->from_id, p_info->to_id);


		//auto open adv when switch to mainmenu
#ifdef ENABLE_ADS
		manage_ad_focus_changed(old_root, new_root);
#endif


#ifdef ENABLE_CA
		cas_manage_finger_repaint();
		OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
#endif
	}

	CURN_MENU_ROOT_ID = new_root;
	return SUCCESS;
}


static RET_CODE on_uio_event(u32 event, u32 para1, u32 para2)
{
  if(CURN_MENU_CLOSE_TYPE == ON)
  {
    manage_tmr_reset();
  }

//#ifdef SLEEP_TIMER
 // uio_reset_sleep_timer();
//#endif

  return SUCCESS;
}
static BOOL g_ca_roll_status = FALSE;

u8 get_menu_state_by_id(u8 id)
{
	u8 idx;
	idx = manage_find_menu(id);
	
	return MENU_GET_PLAY_STATE(idx);
}

BOOL ui_ca_get_roll_status(void)
{
  return g_ca_roll_status;
}

void ui_ca_set_roll_status(BOOL is_enable)
{
  g_ca_roll_status = is_enable;
}


BEGIN_WINDOW_MANAGE(ui_menu_manage)
ON_MENU_EVENT(WINDOW_MANAGE_ON_UIO_EVENT, on_uio_event)
ON_MENU_EVENT(WINDOW_MANAGE_FOCUS_CHANGED, on_focus_changed)
ON_MENU_EVENT(WINDOW_MANAGE_PRE_OPEN, on_pre_open)
ON_MENU_EVENT(WINDOW_MANAGE_PRE_CLOSE, on_pre_close)
ON_MENU_EVENT(WINDOW_MANAGE_INIT, on_initialize)
END_WINDOW_MANAGE(ui_menu_manage)
