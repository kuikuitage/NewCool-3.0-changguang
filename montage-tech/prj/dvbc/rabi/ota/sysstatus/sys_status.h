/****************************************************************************

****************************************************************************/


#ifndef __SYS_STAUTS_H__
#define __SYS_STAUTS_H__

#define MAX_CATEGORY_CNT     (24) //use fav flag, use in MAX_FAV_CNT
#define MAX_PRESET_FAV_CNT   (24)
#define MAX_FAV_CNT          (MAX_CATEGORY_CNT + MAX_PRESET_FAV_CNT)
#define MAX_GROUP_CNT        (64 + MAX_FAV_CNT + 0x1 /*all group*/)

#define EVENT_NAME_LENGTH    (16)
#define MAX_BOOK_PG          20
#define MAX_FAV_NAME_LEN	 24
#define INFO_STRING_LEN  16
#define MAX_CHANGESET_LEN 20

typedef enum 
{
  BS_LNB_POWER = 0,
  BS_NIT_RECIEVED,
  BS_ANTENNA_CONNECT,
  BS_MENU_LOCK,
  BS_PROG_LOCK,
  BS_SLEEP_LOCK,
  BS_DVR_ON,
  BS_UNFIRST_UNPOWER_DOWN,
  BS_LCN_RECEIVED,
  BS_FIRST_ON,
  BS_IS_SCART_OUT,
  BS_IS_TIME_OUT_WAKE_UP,
  BS_FACTORY_ENTERED,
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
  u8 book_mode : 4;
  u8 record_enable : 4;
  u8 svc_type:4;
  u8 timer_mode:4;
  u16 event_name[EVENT_NAME_LENGTH + 1];
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
  u8 video_effects;
} av_set_t;

/*!
 * audio/vidoe setting
 */
typedef struct
{
  u8 is_global_volume;
  u8 global_volume;
  u8 is_global_track;
  u8 global_track;
} audio_set_t;

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

/*picture play mode*/
typedef struct
{
  u8 show_size;
  u8 slide_time;
  u8 slide_repeat;
  u8 reserved;
}pic_showmode_t;

/*!
  OTA TDI info definition
*/
typedef struct
{
  /*!
    oui
    */
  u32 oui;
  /*!
    manufacture id
    */
  u16 manufacture_id;
  /*!
    hardware module id
    */
  u16 hw_mod_id;
  /*!
    software module id
    */
  u16 sw_mod_id;
  /*!
    hardware version
    */
  u16 hw_version;
  /*!
    software version
    */
  u16 sw_version; 
  /*!
    reserved
    */
  u16 reserved;
}ota_tdi_t;
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
  ota_tdi_t ota_tdi;
}misc_options_t;


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

  /* audio settting */
  audio_set_t    audio_set;
  
  /* play settting */
  play_set_t play_set;

  /* osd settting */
  osd_set_t      osd_set;

  /* password settting */
  pwd_set_t      pwd_set;

  /* preset setting */
  preset_set_t   preset_set;
  u32 auto_sleep;

  /*advertisment TP*/
  dvbc_lock_t ad_tp;

  /*upgrade TP*/
  dvbc_lock_t upgrade_tp;

  /*mian TP 1*/
  dvbc_lock_t main_tp1;

  /*mian TP 2*/
  dvbc_lock_t main_tp2;

  /*NVOD TP*/
  dvbc_lock_t nvod_tp;

  /* bit status */
  u32 bit_status;

  /* nit version */
  u8 nit_version;
  
  /* nit tp number */
  u8 nit_tp_num;

  /*force key*/
  u16 force_key;

  /*global media volume*/  
  u8 global_media_volume;

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

  /*age limit*/
  u16 age_limit;  
  
  /* ucas size */
  u16 ucas_size;

  /* ucas size */
  u16 crypto_size;

  /* ucas size */
  u8 usb_work_partition;

  /* ucas size */
  u8 is_open_biss_key_flag;

  /* timeshift on/off */
  u8 timeshift_switch;

  /* ucas size */
  u8 reserved2;

  /*time out*/
  u32 time_out;
  
  /* categories count */
  u8 categories_count;

  /* default progame number */
  u16 default_logic_number;

  /* need show WaterMark */
  u8 b_WaterMark;

  /* WaterMark x position */
  u16 x_WaterMark;
  
  /* WaterMark y position */
  u16 y_WaterMark;
  
  /* bit status */
  u16 bouquet_id[MAX_FAV_CNT];

  /*scan parameter*/
  scan_param_t scan_param;
  
  /*information*/
  sw_info_t ver_info;

  /*sleep time*/
  utc_time_t sleep_time;

  /*card upg time*/
  utc_time_t card_upg_time;

  /*card upg state*/
  u32 card_upg_state;

  /* group settting */
  group_set_t    group_set;

  /* book infor */
  book_info_t book_info;

  /* local setting */
  local_set_t local_set;

  /*fav group*/
  fav_set_t fav_set;

  /* time settting */
  time_set_t     time_set;
  
  /* ota info */
  ota_info_t ota_info;
} sys_status_t;

typedef enum
{
  /*!
    CVBS (RGB) format for video out.
  */
  VDAC_CVBS_RGB = 0,
  /*!
  Signal CVBS video out, need enable dac 3 on concerto platform
  */
  VDAC_SIGN_CVBS,
    /*!
  Signal CVBS low power video out, use dac 3 for cvbs video out, need HW support
  */
  VDAC_SIGN_CVBS_LOW_POWER,
  /*!
  CVBS+CVBS format for video out.
  */
  VDAC_DUAL_CVBS,
    /*!
    SVDIEO format for  video out.
    */
  VDAC_SIGN_SVIDEO,
    /*!
   CVBS + S_VIDEO format for  video out.
    */
  VDAC_CVBS_SVIDEO,
    /*!
    YUV (SD) + CVBS format for  video out.
    */
  VDAC_CVBS_YPBPR_SD,
    /*!
    YUV (HD) + CVBS format for  video out.
    */
  VDAC_CVBS_YPBPR_HD,
    /*!
    reserved
    */
  VDAC_RESERVED1,
      /*!
    reserved
    */
  VDAC_RESERVED2,
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
#define MAX_LANG_NUM 13

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

typedef enum
{
  //ratio 480p
  RATIO_480I_576I,
  //ratio 576p
  RATIO_480P_576P,
  //ratio 720p
  RATIO_720_P,
  //ratio 1080i
  RATIO_1080_i,
  //ratio 1080p
  RATIO_1080_P
}resolution_ratio;

typedef enum
{
  //pal
  TV_SYSTEM_PAL,
  //ntsc
  TV_SYSTEM_NTSC,
  //auto
  TV_SYSTEM_AUTO,
}tv_system;

typedef struct
{
  unsigned long addr;  
  unsigned long data; 
}reg_data_t;

typedef struct
{
  u8 num;
  reg_data_t array_data[10];  
}reg_cfg_t;

typedef struct
{
  hal_fp_type_t fp_type;
  pan_hw_info_t pan_info;
  led_bitmap_t led_bitmap[MAX_LED_MAP_NUM];
  u16 map_size;  
  vdac_info_t vdac_info;
  spdif_info_t spdif_info;
  lnb_load_t lnb_load;
  scart_info_t scart_info;
  fp_disp_t fp_disp;
  u8 standby_mode;
  u8 loop_through;
  u8 tuner_clock; //0-demo 1-other
  u8 convention;  
  u8 detect_pin_pol;
  u8 vcc_enable_pol;
  BOOL enable_mute_pin;
  u8 mute_pin;
  BOOL mute_gpio_level;
  reg_cfg_t reg;
  u8 led_num;
  u32 reserve1; 
  u32 reserve2; 
  u32 reserve3; 
}hw_cfg_t;

void sys_status_init(void);

void sys_status_load(void);

void sys_status_save(void);

sys_status_t *sys_status_get(void);

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

u32 sys_status_get_standby_time_out(void);

void sys_status_set_standby_time_out(u32 time_out);

char **sys_status_get_lang_code(BOOL is_2_b);

u16 sys_status_get_force_key(void);

ota_info_t *sys_status_get_ota_info(void);

void sys_status_set_ota_info(ota_info_t *p_otai);

BOOL sys_status_get_age_limit(u16 *p_agelimit);

BOOL sys_status_set_age_limit(u16 agelimit);

u32 sys_status_get_auto_sleep(void);

void sys_status_set_auto_sleep(u32 auto_sleep);

void sys_status_set_usb_work_partition(u8 partition);

u8 sys_status_get_usb_work_partition(void);

void sys_status_set_timeshift_switch(BOOL is_on);

BOOL sys_status_get_timeshift_switch(void);
BOOL sys_status_get_ota_preset(void *p_nim_param);

#endif

