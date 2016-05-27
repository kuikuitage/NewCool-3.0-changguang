/****************************************************************************

 ****************************************************************************/


#ifndef __SYS_STAUTS_H__
#define __SYS_STAUTS_H__

//temp.....
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
  u8 lnb_power_enable;
  resolution_ratio reso_ratio;
  tv_system tv_system_mode;
  u8 tuner_clock; //0-demo 1-other
}hw_cfg_t;

#endif

