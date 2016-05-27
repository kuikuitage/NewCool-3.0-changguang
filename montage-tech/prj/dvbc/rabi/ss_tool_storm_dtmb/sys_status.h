#ifndef __SYS_STAUTS_H__
#define __SYS_STAUTS_H__

#define MAX_GROUP_CNT        (DB_DVBS_MAX_SAT + MAX_FAV_CNT + 0x1 /*all group*/)
#define MAX_FAV_CNT          (MAX_FAV_GRP)

#define MAX_IP_CNT           (8)
#define MAX_CITY_CNT         (10)

#define EVENT_NAME_LENGTH    (16)         // TO BE CHECKED
#define MAX_BOOK_PG          20
#define MAX_FAV_NAME_LEN	16
#define INFO_STRING_LEN  16
#define MAX_CHANGESET_LEN 20
#define UCAS_KEY_SIZE   256
#define CRYPTO_KEY_SIZE 50
#define ALBUMS_ID_LENHGT 32
#define MAX_FRIENDS_NUM 10
#define FAV_LIVETV_NUM    (100)
#define FAV_IPTV_NUM       (100)
#define MAX_CONN_PLAY_CNT (100)
#define MAX_PLAY_HIST_CNT (20)
#define MAX_PG_NAME_LEN	(32)
#define MAX_KEY_LEN	(64)
#define MAX_WIFI_NUM 32

typedef enum 
{
  BS_LNB_POWER = 0,
  BS_BEEPER,
  BS_ANTENNA_CONNECT,
  BS_MENU_LOCK,
  BS_PROG_LOCK,
  BS_SLEEP_LOCK,
  BS_DVR_ON,
  BS_UNFIRST_UNPOWER_DOWN,
  BS_TWIN_PORT,
  BS_BISS_TYPE,
  BS_IS_TIME_OUT_WAKE_UP,
  BS_LCN_RECEIVED,
  BS_MAX_CNT
}bit_status_t;

typedef enum 
{
  AGE_LIMIT_VIEW_ALL,
  AGE_LIMIT_7,
  AGE_LIMIT_12,
  AGE_LIMIT_15,
  AGE_LIMIT_18,
  AGE_LIMIT_LOCK_ALL,
  
  AGE_LIMIT_MAX_CNT
}age_limit_t;

typedef enum 
{
  BOOK_TMR_OFF = 0,
  BOOK_TMR_ONCE,
  BOOK_TMR_DAILY,
  BOOK_TMR_WEEKLY,
}book_tmr_mode_t;

/*!
 * network link type includes ethernet lan, wifi
 */
typedef enum
{
  LINK_TYPE_LAN,
  LINK_TYPE_WIFI,
  LINK_TYPE_GPRS,
  LINK_TYPE_3G,
}link_type_t;

typedef enum
{
  DHCP,
  STATIC_IP,
  PPPOE,
  
}config_mode_t;
/*!
 * network config  information
 */

typedef struct
{
  config_mode_t config_mode;
  link_type_t link_type;
  
}net_config_t;

/*!
 * book pg information
 */
typedef struct
{
  u16 pgid;
  u8 record_enable : 4;
  u8 book_mode;
  utc_time_t start_time;
  utc_time_t drt_time;
} book_pg_t;

/*!
 * book information
 */
typedef struct
{
  book_pg_t pg_info[MAX_BOOK_PG];
} book_info_t;

/*!
* favorite group
*/
typedef struct
{
	u16 fav_name[MAX_FAV_NAME_LEN];
} fav_group_t;

typedef struct
{
	u16 ip_name[32];
	u8 ip_account[32];
	u8 ip_password[32];
} ip_group_t;

typedef struct
{
	u16 city_name[32];
} city_group_t;

typedef struct
{
	fav_group_t fav_group[MAX_FAV_CNT];
} fav_set_t;

typedef struct
{
	ip_group_t ip_group[MAX_IP_CNT];
} ip_set_t;

typedef struct
{
	city_group_t city_group[MAX_CITY_CNT];
} city_set_t;

/*!
 * wifi ap info
 */
typedef struct
{
  u8 essid[32+1];
  u8 key[32+1];
  u8 encrypt_type;
  u8 latest;
}wifi_info_t;

/*!
 * gprs connect info
 */
typedef struct
{
  u8 name[64];
  u8 apn[64];
  u8 dial_num[64];
  u8 usr_name[64];
  u8 passwd[64];
}gprs_info_t;

/*!
 * 3g connect info
 */
typedef struct
{
  u8 name[64];
  u8 apn[64];
  u8 dial_num[64];
  u8 usr_name[64];
  u8 passwd[64];
}g3_conn_info_t;

/*!
 * pppoe connect info
 */
typedef struct
{
  u8 account[32+1];
  u8 pwd[32+1];
}pppoe_info_t;

/*!
 * youtube config info
 */
typedef struct
{
  u16  region_id;
  u8  time_mode;
  u8  resolution;
}youtube_config_t;

/*!
 * nm simple config info
 */
typedef struct
{
  u8  resolution;
}nm_simple_config_t;

/*!
 * group type
 */
enum
{
  GROUP_T_ALL = 0,
  GROUP_T_SAT,
  GROUP_T_FAV
};

/*!
 * scan mode
 */
enum
{
  SCAN_MODE_ALL = 0,
  SCAN_MODE_FREE,
};

/*!
 * curn mode
 */
enum
{
  CURN_MODE_NONE = 0,
  CURN_MODE_TV,
  CURN_MODE_RADIO,

  CURN_MODE_CNT,
};


/*last sat*/
typedef struct
{
  u8 sat_id;
} last_sat_t;

/*!
 * language setting
 */
typedef struct
{
  u8 osd_text;
  u8 first_audio;
  u8 second_audio;
  u8 tel_text;
  u8 sub_title;
  u8 text_encode_video;
  u8 text_encode_music;
} language_set_t;

/*!
 * audio/vidoe setting
 */
typedef struct
{
  u8 tv_mode;
  u8 tv_resolution;
  u8 tv_ratio;
  u8 video_output;
  u8 digital_audio_output;
  u8 rf_system;
  u8 rf_channel;
 // u8 transcoding;
  u8 video_effects;
} av_set_t;

/*!
 * osd setting
 */
typedef struct
{
  u8 palette;
  u8 transparent;
  u8 timeout;
  u8 enable_teltext;
  u8 enable_subtitle;
//  u8 enable_clock;
  u8 enable_vbinserter;
} osd_set_t;

/*!
 * password setting
 */
typedef struct
{
  u32 normal;
  u32 super;
} pwd_set_t;

/*!
 program group infor
 */
typedef struct
{
  u16 curn_tv;
  u16 curn_radio;
  u16 tv_pos;
  u16 rd_pos;
  u32 context;
}curn_info_t;

/*!
 program group setting
 */
typedef struct
{
  u8          curn_mode;
  u16         curn_group;
  u8          curn_type;  
  curn_info_t group_info[MAX_GROUP_CNT];
} group_set_t;


#define W_LONGITUDE_MASK 0x8000

#define IS_W_LONGITUDE(x) (BOOL)(x & W_LONGITUDE_MASK)
#define SET_LONGITUDE(x, w) (x = w ? (x | W_LONGITUDE_MASK) : (x & (~W_LONGITUDE_MASK)))
#define LONGITUDE_NORMALIZE(x) (x &= (~W_LONGITUDE_MASK))

#define S_LATITUDE_MASK 0x8000

#define IS_S_LATITUDE(x) (BOOL)(x & S_LATITUDE_MASK)
#define SET_LATITUDE(x, s) (x = s ? (x | S_LATITUDE_MASK) : (x & (~S_LATITUDE_MASK)))
#define LATITUDE_NORMALIZE(x) (x &= (~S_LATITUDE_MASK))

/*!
 * local setting
 */
typedef struct
{
  u8   positioner_type; // 0 DiSEqC1.2 1 USALS
  u8   lnb_type;
  
  u16 longitude;
  u16 latitude;
  
  u16 lnb_h;
  u16 lnb_l;
} local_set_t;

/*!
 * time setting
 */
typedef struct
{
  utc_time_t sys_time;
  u8 gmt_usage  : 1;
  u8 gmt_offset :  6;
  u8 summer_time :1;
} time_set_t;

/*! 
   Format ip address definition
  */
typedef struct
{
  /*!
    s_a1
    */
  u16 s_a1;
  /*!
    s_a2
    */
  u16 s_a2;
  /*!
    s_a3
    */
  u16 s_a3;
  /*!
    s_a4
    */
  u16 s_a4;

} ip_addr_t;

/*!
 *	ipaddress setting
 */
typedef struct
{
  ip_addr_t sys_ipaddress;
  ip_addr_t sys_netmask;
  ip_addr_t sys_gateway;
  ip_addr_t sys_dnsserver;
}	ip_address_set_t;

/*!
 *	mac setting
 */
typedef struct
{
  char mac_one[2];
  char mac_two[2];
  char mac_three[2];
  char mac_four[2];
  char mac_five[2];
  char mac_six[2];
}	mac_set_t;

/*!
 * play type
 */
enum
{
  PLAY_T_ALL = 0,
  PLAY_T_FREE,
  PLAY_T_SCRAMBLE,
};

/*!
 * channel change mode
 */
enum
{
  CHCHANGE_M_FREEZE = 0,
  CHCHANGE_M_BLACKSCR,
};

/*!
 * channel play setting
 */
typedef struct
{
  /* all free scramble */
  u8 type;
  /* black freeze */
  u8 mode;
} play_set_t;

/*!
 * preset setting
 */
typedef struct
{
  u8 enable;
  u8 tv_track;
  u8 radio_track;
} preset_set_t;

/*version info*/
typedef struct
{
  u16 bl_ver[INFO_STRING_LEN];
  u16 bl_time[INFO_STRING_LEN];
  u16 main_ver[INFO_STRING_LEN];
  u16 main_time[INFO_STRING_LEN];
  u16 rback_ver[INFO_STRING_LEN];
  u16 rback_time[INFO_STRING_LEN];
  u16 vinfo_ver[INFO_STRING_LEN];
  u16 vinfo_time[INFO_STRING_LEN];
  u16 ddb_ver[INFO_STRING_LEN];
  u16 ddb_time[INFO_STRING_LEN];
  u16 udb_ver[INFO_STRING_LEN];
  u16 udb_time[INFO_STRING_LEN];
}sw_info_t;

/*!
  DVBC lock info
  */
typedef struct
{
  /*!
    freq
    */
  u32 tp_freq;
  /*!
    sym
    */
  u32 tp_sym;
  /*!
    nim modulate
    */
  u8 nim_modulate;

  u8 reserve1;
  u8 reserve2;
  u8 reserve3;
} dvbc_lock_t;

/*version info*/
typedef struct
{
  u8 scan_mode;
  u8 fta_only;
  u8 scan_channel;
  u8 network_search;
  
  u8 tp_scan_mode;
  u8 reserved;
  u16 vpid;
  
  u16 apid;
  u16 ppid;
}scan_param_t;

/*!
 * picture show size
 */
enum
{
  PIC_SHOW_SIZE_FULL = 0,
  PIC_SHOW_SIZE_REAL,
};
#define BISS_KEY_LENGTH 8
#define CRYPTO_KEY_LENGTH 16

typedef struct
{
  u8 data_bc[16];
  u8 data_bc_len;
}bc_key_info;


typedef struct
{
   u32 reserved;
   u16 pg_freq;
   u16 pg_s_id;
   u8 biss_key[BISS_KEY_LENGTH];

}biss_key_t;

typedef struct
{
   u32 reserved[2];
   u32 key_length;  
   u16 pg_freq;
   u16 pg_s_id;
   u8 crypto_key[CRYPTO_KEY_LENGTH];
}crypto_key_t;

/*picture play mode*/
typedef struct
{
	u8 slide_show;
	u8 slide_time;
	u8 slide_repeat;
	u8 special_effect;
}pic_showmode_t;


//#define UCAS_KEY_SIZE (32*KBYTES)
#define UCAS_KEY_SYNC_HDR_SIZE 4

/*!
 * system setting
 */
typedef struct
{
  char debug_info;
  char sig_str_ratio;
  char sig_str_max;
  char sig_qua_max;
  char str_l_qua;
  unsigned int str_0_gain;
  char bs_times;
  char spi_clk;
  char rd_mode;
  char standby_mode;
  char watchdog_mode;    			
  char str_poweron[10];
  char str_standby[10];
  char str_sw_version[20]; 
  char str_last_upgrade_date[20];
  //ota_tdi_t ota_tdi;
}misc_options_t;

/*!
  * epg lang setting
  */
typedef struct
{
  u8 first_lang;

  u8 second_lang;

  u8 default_lang;
  
}epg_lang_t;


typedef struct key_info{
  unsigned int     Sync;//always is 0x87654321
  unsigned short   Version;//150 is 1.50
  unsigned short   ViaKeyNr,IrdKeyNr,SecaKeyNr,NagKeyNr,
	               BissKeyNr,ConaxKeyNr,Nag2KeyNr,Via2KeyNr,
				   NagRsaKeyNr,Nag2RsaKeyNr,
				   CryptoWorksKeyNr,Seca2KeyNr,CryptoWorkshalfKeyNr,VideoGuardkeyNr,VideoGuard2keyNr,Conax2KeyNr,
				   Irdeto2Nr,ProviderNameOffset,reserve1,reserve2,reserve3,reserve4,reserve5,reserve6;
}key_head_info;

#define  IRDETO_MAX_KEYINFOS      80
#define  VIACCESS_MAX_KEYINFOS    150
#define  SECA_MAX_KEYINFOS        100
#define  NAGRA_MAX_KEYINFOS       80
#define  BISS_MAX_KEYINFOS        200
#define  CONAX_MAX_KEYINFOS       8
#define  VIACCESS2_MAX_KEYINFOS   200//1200
#define  NAGRA2_MAX_KEYINFOS      32
#define  CRYPTOWORKS_MAX_KEYINFOS    80
#define  IRDETO2_MAX_KEYINFOS     56
#define  SECA2_MAX_KEYINFOS        50
#define  CONAX2_MAX_KEYINFOS       8
#define  CRYPTOWORKSHALF_MAX_KEYINFOS    40
#define  VIDEOGUARD_MAX_KEYINFOS    80
#define  VIDEOGUARD2_MAX_KEYINFOS    80


//ca key types
#define TYPE_VIACCESS2   0x04
#define TYPE_VIACCESS    0x05
#define TYPE_IRDETO      0x06
#define TYPE_SECA        0x01
#define TYPE_SECA2       0x02
#define TYPE_CONAX       0x0B
#define TYPE_CONAX2       0x0C
#define TYPE_CRYPTOWORKS 0x0d
#define TYPE_CRYPTOWORKSHALF 0x0e
#define TYPE_NAGRA       0x18
#define TYPE_NAGRA2      0x19
#define TYPE_BISS        0x26
#define TYPE_IRDETO2	 0x2a
#define TYPE_VIDEOGUARD     0x2b
#define TYPE_VIDEOGUARD2       0x2c

typedef struct key0{
  unsigned int id;  //ident
  unsigned char type;  //  ca system type
  unsigned char keynr; // key number
  unsigned char key[8]; //8 bytes key
}keyinf0;

//op key version2 for viaccess2,nagra2
typedef struct key2{
  unsigned int id;  //ident
  unsigned char type;  //  ca system type
  unsigned char keynr; // key number
  unsigned char key[16]; //16 bytes key
}keyinf2;

typedef struct keybiss{
  unsigned int id;  //ident
  unsigned char type;  //  ca system type
  unsigned char keynr; // key number
  unsigned int servid; // servid
  unsigned char key[8]; //8 bytes key
}keyinf0biss;

typedef struct key_ca{
 key_head_info g_keyheaderinfos;
 keyinf0    g_ViaccessKey[VIACCESS_MAX_KEYINFOS];
 keyinf0    g_IrdetoKey[IRDETO_MAX_KEYINFOS];
 keyinf0    g_SecaKey[SECA_MAX_KEYINFOS];
 keyinf0    g_NagraKey[NAGRA_MAX_KEYINFOS];
 keyinf0  g_ConaxKey[CONAX_MAX_KEYINFOS];
 keyinf0  g_CryptoWorkshalfKey[CRYPTOWORKSHALF_MAX_KEYINFOS];
 keyinf0  g_VideoGuardKey[VIDEOGUARD_MAX_KEYINFOS];
 keyinf0biss    g_BissKey[BISS_MAX_KEYINFOS];
 keyinf2   g_Viaccess2Key[VIACCESS2_MAX_KEYINFOS];
 keyinf2   g_Nagra2Key[NAGRA2_MAX_KEYINFOS];
 keyinf2   g_CryptoWorksKey[CRYPTOWORKS_MAX_KEYINFOS];
 keyinf2   g_Irdeto2Key[IRDETO2_MAX_KEYINFOS];
 keyinf2    g_Seca2Key[SECA2_MAX_KEYINFOS]; 
 keyinf2  g_Conax2Key[CONAX2_MAX_KEYINFOS];
 keyinf2  g_VideoGuard2Key[VIDEOGUARD2_MAX_KEYINFOS];
}key_ca_description;

/*!
 * ts delay setting
 */
typedef struct
{
  u8 delay_enable;
  u16 time_ms;
  u8 reserved;
} delay_set_t;

/*!
  albums_site_t
  */

typedef struct
{
  u8 user_id[ALBUMS_ID_LENHGT];
  
  u8 site;

  BOOL is_used;
}albums_friend_t;

typedef struct
{
  u8 friends_num;

  albums_friend_t albums_friend[MAX_FRIENDS_NUM];
}albums_friends_t;

typedef struct
{
  u32 live_fav_id;
  u8 live_fav_name[32];
}live_tv_fav_info;

typedef struct
{
  u32 live_tv_total_num;
  live_tv_fav_info live_fav_pg[FAV_LIVETV_NUM];
}live_tv_fav;

typedef struct
{
  u8  type;
  u8  qpId[32];
  u8  tvQid[32];
  u8  sourceCode[65];
}iptv_vdo_id_t;

typedef struct
{
  u32 iptv_fav_category_id;
  u16 iptv_fav_name[32];
  iptv_vdo_id_t iptv_fav_vdo_id;
}iptv_fav_info;

typedef struct
{
  u32 iptv_total_num;
  iptv_fav_info iptv_fav_pg[FAV_IPTV_NUM];
}iptv_fav;

typedef struct
{
  u8  is_used;
  u16 episode_num;

  u32 timestamp;
  u32 play_time;

  u16 key[MAX_KEY_LEN];
} db_conn_play_item;

typedef struct
{
  u8  is_used;

  u32 timestamp;
  u32 res_id;

  u16 pg_name[MAX_PG_NAME_LEN];
  iptv_vdo_id_t vdo_id;
} db_play_hist_item;

/*t2 search info */
typedef struct
{
  u8 country_style;
  u8 antenna_state;
  u8 lcn_state;
}t2_search_info;

/*!
 * system setting
 */
typedef struct
{
  /* change set.no */
  u8 changeset[MAX_CHANGESET_LEN];

  /*version number*/
  u32 sw_version;
 
  /*last sat*/
  last_sat_t     last_sat;

  /* language settting */
  language_set_t lang_set;

  /* av settting */
  av_set_t       av_set;
  
  /* play settting */
  play_set_t play_set;

  /* osd settting */
  osd_set_t      osd_set;

  /* password settting */
  pwd_set_t      pwd_set;

  /* preset setting */
  preset_set_t   preset_set;

  /* time settting */
  time_set_t     time_set;

  /* group settting */
  group_set_t    group_set;

  /* book infor */
  book_info_t book_info;

  /* local setting */
  local_set_t local_set;
  
  /*fav group*/
  fav_set_t fav_set;

  /*information*/
  sw_info_t ver_info;

  /*scan parameter*/
  scan_param_t scan_param;
  
  /* bit status */
  u32 bit_status;

  /*sleep time*/
  utc_time_t sleep_time;

  /*global volume*/  
  u8 global_volume;

  /*brightness*/  
  u8 brightness;

  /*contrast*/  
  u8 contrast;

  /*saturation*/  
  u8 saturation;

  /*dvr jump time*/  
  u32 dvr_jump_time;

  /* picture play mode */
  pic_showmode_t pic_showmode;
  pic_showmode_t pic_showmode_net;
  
  /* ota info */
  ota_info_t ota_info;

  /* force key */
  u16 force_key;

  /*age limit*/
  u16 age_limit;  
                        
  /* ucas size */
   u16 ucas_size;

   /* ucas size */
   u16 crypto_size;

      /* ucas size */
   u8 usb_work_partition;

   /* timeshift on/off */
   u8 timeshift_switch;
   
   /* ucas size */
   u16 reserved2;

  /*auto_sleep hotkey*/
  u32 auto_sleep;

   /*ipaddress setting*/
   ip_address_set_t ipaddress_set;

   delay_set_t delay_set;
	/*time out*/
   u32 time_out;   

  /*ip path*/
  ip_set_t ip_path_set;

  u8 ip_path_cnt;

    /*city path*/

  city_set_t city_path_set;

  u8 city_cnt;

   
  /*ipaddress setting*/
  mac_set_t mac_set;

  /*wifi ap info setting*/
  wifi_info_t wifi_set[MAX_WIFI_NUM];

  /*network config setting*/
  net_config_t net_config_set;

  /*pppoe connect setting*/
  pppoe_info_t pppoe_set;

  youtube_config_t youtube_config_set;

  nm_simple_config_t nm_simple_config_set;
  // add friends
  albums_friends_t albums_friends;

  //dlna device name
  char dlna_device_name[64];

  live_tv_fav fav_live_pg ;

  iptv_fav fav_iptv_pg ;

  db_conn_play_item conn_play_table[MAX_CONN_PLAY_CNT];

  db_play_hist_item play_hist_table[MAX_PLAY_HIST_CNT];

  /*gprs info setting*/
  gprs_info_t gprs_info;

  g3_conn_info_t g3_conn_info;

  t2_search_info t2_search;

  epg_lang_t e_lang_set;	

  /*mian TP 1*/
  dvbc_lock_t main_tp1;
  
  /* nit version */
  u8 nit_version;
} sys_status_t;

typedef enum
{
  VDAC_CVBS_RGB = 0,
  VDAC_DUAL_CVBS,
  VDAC_CVBS_YPBPR,
  VDAC_CVBS_SVIDEO,
  VDAC_SIGN_CVBS,
}vdac_info_t;


typedef enum
{
  SPDIF_ON = 0,
  SPDIF_OFF,    
}spdif_info_t;

typedef enum
{
  LNB_LOAD_ON = 0,
  LNB_LOAD_OFF,    
}lnb_load_t;

typedef enum
{
  FP_GPIO = 0,
  FP_MCU
}fp_type_t;

#define MAX_LED_MAP_NUM 128
#define MAX_LANG_NUM 9

typedef struct
{
  BOOL scart_enable;
  u8 pin_tvav;
  u8 pin_tvmode;
  u8 pin_tvratio;
  BOOL sog_enable;
}scart_info_t;

typedef enum
{
     DISP_TIME,
     DISP_OFF,
     DISP_NONE
}fp_disp_t;


void sys_status_init(void);

void sys_status_load(void);

void sys_status_save(void);

sys_status_t *sys_status_get(void);

void sys_status_get_key_info(key_ca_description *key_info);

u32 sys_status_get_sw_version(void);

void sys_status_set_sw_version(u32 new_version);

void  sys_status_add_book_node(u8 index, book_pg_t *node);

void sys_status_delete_book_node(u8 index);

void sys_status_get_book_node(u8 index, book_pg_t *node);

BOOL sys_status_get_status(u8 type, BOOL *p_status);

BOOL sys_status_set_status(u8 type, BOOL enable);

void sys_status_set_pwd_set(pwd_set_t *p_set);

void sys_status_get_pwd_set(pwd_set_t *p_set);

void sys_status_set_time(time_set_t *p_set);

void sys_status_get_time(time_set_t *p_set);
void sys_status_set_ipaddress(ip_address_set_t *p_set);

void sys_status_get_ipaddress(ip_address_set_t *p_set);

void sys_status_set_wifi_info(wifi_info_t *p_set);

void sys_status_get_wifi_info(wifi_info_t *p_set);

void sys_status_set_net_config_info(net_config_t *p_set);

void sys_status_get_net_config_info(net_config_t *p_set);

void sys_status_set_pppoe_config_info(pppoe_info_t *p_set);

void sys_status_get_pppoe_config_info(pppoe_info_t *p_set);

void sys_status_set_youtube_config_info(youtube_config_t *p_set);

void sys_status_get_youtube_config_info(youtube_config_t *p_set);

void sys_status_set_play_set(play_set_t *p_set);

void sys_status_get_play_set(play_set_t *p_set);

void sys_status_set_av_set(av_set_t *p_set);

void sys_status_get_av_set(av_set_t *p_set);

void sys_status_set_osd_set(osd_set_t *p_set);

void sys_status_get_osd_set(osd_set_t *p_set);

void sys_status_set_lang_set(language_set_t *p_set);

void sys_status_get_lang_set(language_set_t *p_set);

BOOL sys_status_get_fav_name(u8 index, u16 *name);

void sys_status_set_local_set(local_set_t *p_set);

BOOL sys_status_set_fav_name(u8 index, u16 *name);

void sys_status_get_local_set(local_set_t *p_set);

void sys_status_set_scan_param(scan_param_t *p_set);

void sys_status_get_scan_param(scan_param_t *p_set);

void sys_status_get_sw_info(sw_info_t *p_sw);

void sys_status_set_sw_info(sw_info_t *p_sw);

void sys_status_set_time_zone(void);

void sys_status_get_utc_time(utc_time_t *p_time);

void sys_status_set_utc_time(utc_time_t *p_time);

void sys_status_get_sleep_time(utc_time_t *p_time);

void sys_status_set_sleep_time(utc_time_t *p_time);

void sys_status_get_pic_showmode(pic_showmode_t *p_showmode);

void sys_status_set_pic_showmode(pic_showmode_t *p_showmode);

void sys_status_load_ucaskey(void);

/*
RET_CODE sys_status_get_key(u8 block_id,u16 pg_freq,\
     u16 pg_s_id,u8 *data,u8 *len);

RET_CODE sys_status_set_key(u8 block_id,u16 pg_freq,u16 pg_sid,\
                           u8 *p_bc_data,u8 p_data_len);

*/
char **sys_status_get_text_encode(void);
char **sys_status_get_lang_code(BOOL is_2_b);

/*!
   convert unit of longitude from unsigned int to double.

   \param[in] u_longitude west longitude with W_LONGITUDE_MASK
   \return longitude 0 to 180 east, -180 to 0 west.
  */
double sys_status_convert_longitude(u32 u_longitude);

/*!
   convert unit of latitude from unsigned int to double.

   \param[in] u_longitude sorth latitude with S_LATITUDE_MASK
   \return longitude 0 to 90 north, -90 to 0 sorth.
  */
double sys_status_convert_latitude(u32 u_latitude);

u16 sys_status_get_force_key(void);

ota_info_t *sys_status_get_ota_info(void);

void sys_status_set_ota_info(ota_info_t *p_otai);

BOOL sys_status_get_age_limit(u16 *p_agelimit);

BOOL sys_status_set_age_limit(u16 agelimit);

u8 sys_status_get_usb_work_partition(void);


#endif

