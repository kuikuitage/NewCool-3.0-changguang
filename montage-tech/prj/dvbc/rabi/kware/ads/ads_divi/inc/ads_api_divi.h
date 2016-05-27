/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __DIVI_AD_DEF_H
#define __DIVI_AD_DEF_H

#ifdef __cplusplus
extern "C"
{
#endif


/*-------------------------1????¨²2??¡§¨°?---------------------------------*/
#define DIVI_ADS_MONITOR_TASK_PRIO  AD_TASK_PRIO_END
#define ADS_NUM               8
#define OSD_OFFSET            6*64*KBYTES
#define FILTER_DATA_OFFSET    7*64*KBYTES
#define FLASH_OFFSET          64*KBYTES
#define ADS_SIZE              64*KBYTES
#define PACKET_SIZE           160
#define MAX_OSD_TEXT_SIZE     (256)


typedef enum AD_TYPE_E
{
  AD_OPEN_LOGO = 0x01,
  AD_PF_INFO,
  AD_VOLUME,
  AD_FS_RIGHT,
  AD_MAIN_MENU,
  AD_CH_LIST,
  AD_BACKUP,
  AD_OSD_START = 0x7,
  AD_OSD_END = 0xb,
  AD_OTA = 0x99,
}AD_TYPE_T;

typedef enum FILE_TYPE_E
{
  FILE_MPEG = 0x01,
  FILE_BMP,
  FILE_JPEG,
  FILE_GIF,
  FILE_OTA,
}FILE_TYPE_T;

typedef enum PACKET_FLAG_E
{
  PACKET_NO_REV,
  PACKET_REV,
}PACKET_FLAG_T;

typedef struct divi_ads_sdram_s
{
  u8  ads_type;
  u8  ads_file_type;
  u8  ads_is_show;
  u8  ads_is_finish;
  u8  ads_show_time;
  u8  ads_show_pos;
  u16 ads_length;
  u16 cur_packet;
  u16 total_packet;
  u8  *packet_flag;
  u8  *ads_content;
  u32 ads_ver;
  struct divi_ads_sdram_s *next;
}divi_ads_sdram_t;


typedef struct divi_ads_flash_s
{
  u8  ads_type;
  u8  ads_file_type;
  u8  ads_is_show;
  u8  ads_is_finish;
  u8  ads_show_time;
  u8  ads_show_pos;
  u16 total_packet;
  u16 ads_len;
  u32 ads_ver;
  u32 ads_flash_offset;
}divi_ads_flash_t;

typedef struct divi_ads_data_s
{
  u8  ads_type;
  u8  ads_file_type;
  u8  ads_is_show;
  u8  ads_is_finish;
  u8  ads_show_time;
  u8  ads_show_pos;
  u16 ads_length;
  u16 cur_packet;
  u16 total_packet;
  u8  packet_flag[500];
  u8  ads_content[63*KBYTES];
  u32 ads_ver;
}divi_ads_data_t;

typedef struct DIVI_COM_TIME_T
{
    u32 year    : 12;
    u32 month   : 4;
    u32 day     : 5;
    u32 hour    : 5;
    u32 minute  : 6;
    u32 second;
} DIVI_AD_TIME_T;

typedef struct DIVI_ROLL_Text_AD_DATA
{
	u8       m_chAdContent[MAX_OSD_TEXT_SIZE];
	u8       m_bigOsdLen;
	DIVI_AD_TIME_T    font_end_time;
	u16    start_x;
	u16  start_y;
	u16  width;
	u16	bk_width;
	u16	bk_height;
} S_DIVI_OSD_DATA;

u8 divi_ads_drv_init(void);
void divi_ads_read_data_from_flash(void);
void divi_ads_add_data(u8 *p_got_data,u32 got_data_len);
void divi_ads_set_ads_ver(u8 ads_type);
u32 divi_ads_get_data(u8 ads_type, u8 *p_display, u8 *is_show, u8 *show_time, u8 *show_pos, u8 *file_type);
u8 divi_check_roll(void);
/* C++ support */
#ifdef __cplusplus
}
#endif




#endif

