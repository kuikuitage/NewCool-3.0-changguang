/****************************************************************************

****************************************************************************/


#ifndef __SYS_STAUTS_H__
#define __SYS_STAUTS_H__

#define MAX_GROUP_CNT        (64 + MAX_FAV_CNT + 0x1 /*all group*/)
#define MAX_FAV_CNT          8//(8)

#define EVENT_NAME_LENGTH    (16)         // TO BE CHECKED
#define MAX_BOOK_PG          20
#define MAX_FAV_NAME_LEN	16
#define INFO_STRING_LEN  16
#define MAX_CHANGESET_LEN 20
#define UCAS_KEY_SIZE   256
#define CRYPTO_KEY_SIZE 50


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
 * book pg information
 */
typedef struct
{
  u16 pgid;
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
  fav_group_t fav_group[MAX_FAV_CNT];
} fav_set_t;

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
  //u8 transcoding;
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
  u8   positioner_type; // 0 none,1 DiSEqC1.2 2 USALS
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
  u8 show_size;
  u8 slide_time;
  u8 slide_repeat;
  u8 reserved;
}pic_showmode_t;


//#define UCAS_KEY_SIZE (32*KBYTES)
#define UCAS_KEY_SYNC_HDR_SIZE 4

/*!
 * system setting
 */

typedef struct key_info{
  unsigned int     Sync;//always is 0x87654321
  unsigned short   Version;//150 is 1.50
  unsigned short   ViaKeyNr,IrdKeyNr,SecaKeyNr,NagKeyNr,
	               BissKeyNr,ConaxKeyNr,Nag2KeyNr,Via2KeyNr,
				   NagRsaKeyNr,Nag2RsaKeyNr,Nag2EMMKeyNr,
				   CryptoWorksKeyNr,direct_cw_keyNr,N2_SmartcardNr,ProviderNameOffset,ProviderNameNr,
				   tps_v3Nr,Irdeto2Nr,reserve1,reserve2,reserve3,reserve4,reserve5,reserve6;
}key_head_info;

#define  IRDETO_MAX_KEYINFOS      80
#define  VIACCESS_MAX_KEYINFOS    150
#define  SECA_MAX_KEYINFOS        100
#define  NAGRA_MAX_KEYINFOS       80
#define  BISS_MAX_KEYINFOS        200
#define  CONAX_MAX_KEYINFOS       8
#define  VIACCESS2_MAX_KEYINFOS   200//1200
#define  NAGRA2_MAX_KEYINFOS      32
#define  MAX_KEYINFOS_NAG_RSA     40
#define  MAX_KEYINFOS_NAG2_RSA    20
#define  MAX_KEYINFOS_NAG2_EMM    16
#define  CRYPTOWORKS_MAX_KEYINFOS    80
#define  DIRECT_CW_MAX_KEYINFOS      100
#define  N2_SMARTCARD_MAX_KEYINFOS    16
#define  TPS_V3_MAX_KEYINFOS    	200
#define  IRDETO2_MAX_KEYINFOS     56


//ca key types
#define TYPE_VIACCESS2   0x04
#define TYPE_VIACCESS    0x05
#define TYPE_IRDETO      0x06
#define TYPE_SECA        0x01
#define TYPE_CONAX       0x0B
#define TYPE_CRYPTOWORKS 0x0d
#define TYPE_NAGRA       0x18
#define TYPE_NAGRA2      0x19
#define TYPE_IRDETO2	 0x2a
#define TYPE_BISS        0x26

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

typedef struct keynax{      
  unsigned int id;  //ident
  unsigned char type;  //  ca system type
  unsigned char keynr; // key number
  unsigned char exp[64],mod[64];
}keyinfnax;

typedef struct key_ca{
 key_head_info g_keyheaderinfos;
 keyinf0    g_ViaccessKey[VIACCESS_MAX_KEYINFOS];
 keyinf0    g_IrdetoKey[IRDETO_MAX_KEYINFOS];
 keyinf0    g_SecaKey[SECA_MAX_KEYINFOS];
 keyinf0    g_NagraKey[NAGRA_MAX_KEYINFOS];
 keyinf0    g_BissKey[BISS_MAX_KEYINFOS];
 keyinf2   g_Viaccess2Key[VIACCESS2_MAX_KEYINFOS];
 keyinf2   g_Nagra2Key[NAGRA2_MAX_KEYINFOS];
 keyinf2   g_CryptoWorksKey[CRYPTOWORKS_MAX_KEYINFOS];
 keyinf2   g_Irdeto2Key[IRDETO2_MAX_KEYINFOS];
 keyinfnax   g_ConaxKey[CONAX_MAX_KEYINFOS];
}key_ca_description;

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

    /* ucas size */
   u8 reserved1;
   
   /* ucas size */
   u16 reserved2;
   
   /*auto_sleep hotkey*/
  u32 auto_sleep;
   
  /*biss  key */
  key_ca_description ucas_key;
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

typedef struct
{
  fp_type_t fp_type;
  pan_hw_info_t pan_info;
  led_bitmap_t led_bitmap[MAX_LED_MAP_NUM];
  u16 led_num;  
  vdac_info_t vdac_info;
  spdif_info_t spdif_info;
  lnb_load_t lnb_load;
  scart_info_t scart_info;
  u8 brightness;
  fp_disp_t fp_disp;
  u8 standby_mode;
  u8 lang[MAX_LANG_NUM];
  u8 lang_def; 
  u8 loop_through;
  u16 power_wave[63];
  u8 biss_enable;
}hw_cfg_t;


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
}mac_set_t;





/*!
 * wifi ap info
 */
typedef struct
{
  u8 essid[32+1];
  u8 key[32+1];
  u8 encrypt_type;
}wifi_info_t;


typedef struct
{
  BOOL is_eth_insert; //TRUE means ethernet cable insert
  BOOL is_eth_conn; //TRUE means ethernet connect successfully
  BOOL is_wifi_insert; //TRUE means wifi device insert
  BOOL is_wifi_conn; //TRUE means wifi connect successfully
  
}net_conn_stats_t;



/*!
 * network link type includes ethernet lan, wifi
 */
typedef enum
{
  LINK_TYPE_LAN,
  LINK_TYPE_WIFI,
  
}link_type_t;

typedef enum
{
  DHCP,
  STATIC_IP,
  PPPOE,
  
}config_mode_t;

/*!
 * active stb result
 */
typedef enum
{
  ACTIVE_STB_NONE = 0,
  ACTIVE_STB_SUCCESS,
  ACTIVE_STB_FAILED,
}active_stb_t;

/*!
 * network config  information
 */

typedef struct
{
  config_mode_t config_mode;
  link_type_t link_type;
  
}net_config_t;



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
  ip_addr_t sys_dnsserver2;
}	ip_address_set_t;

void sys_status_init(void);

u32 sys_status_get_sw_version(void);


void sys_status_get_ipaddress(ip_address_set_t *p_set);

void sys_status_set_ipaddress(ip_address_set_t *p_set);


void sys_status_get_net_config_info(net_config_t *p_set);
char  sys_status_get_mac_by_index(u8 index);

BOOL sys_status_get_mac(u8 index, char *name);

void sys_status_set_net_config_info(net_config_t *p_set);


void sys_status_get_wifi_info(wifi_info_t *p_set);



u32 sys_status_get_customer_id();
void sys_status_set_customer_id(u32 cid);

u32 sys_status_get_serial_num();
void  sys_status_set_serial_num(u32 sn);

u32 sys_status_get_hw_ver();
void sys_status_set_hw_ver(u32 hw);

void sys_status_save(void);

void sys_status_set_wifi_info(wifi_info_t *p_set);
void ui_set_net_connect_status( net_conn_stats_t);


char  sys_status_get_mac_by_index(u8 index);
BOOL sys_status_set_mac(u8 index, char *name);
BOOL sys_status_get_mac(u8 index, char *name);

#endif

