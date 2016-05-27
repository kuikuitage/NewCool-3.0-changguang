#ifndef __UI_LIVE_TV_API_H__
#define __UI_LIVE_TV_API_H__
#include "commonData.h"
#include "tvLiveDataProvider.h"

#define APP_LIVE_TV APP_TKGS
#define LIVE_TV_TASK_STACK_SIZE (512*KBYTES)
#define MAX_LOOK_BACK_INFO_DAY_NUM (7)
#define MAX_EPG_INFO_DAY_NUM (1)
#define MAX_LOOKBACK_URL_CNT (50)
#define MAX_EPG_NUM (200)
#define MAX_LOOKBACK_CHANNEL (100)
#define MAX_LOOKBACK_PGMS_NUM (50)
#define MAX_LOOKBACK_URL_LEN (512)
#define MAX_TITLE_LEN (64)
#define LIVE_TV_INVALID_ID      (0xffff)
#define LIVE_TV_INVALID_INDEX (0xff)

enum 
{
  MSG_GET_LIVE_TV_CATEGORY_ARRIVAL = MSG_EXTERN_BEGIN + 2900,
  MSG_GET_LIVE_TV_GET_ZIP,
  MSG_GET_LIVE_TV_LOOKBACK_NAMES,
  MSG_GET_LIVE_TV_ONE_PG_ONE_DAY_EPG,
  MSG_GET_LIVE_TV_ONE_PG_ONE_DAY_LOOKBACK,
  MSG_INIT_LIVETV_SUCCESS,
  MSG_INIT_LIVETV_FAILED,
  MSG_GET_LIVE_TV_CHANNELLIST,
};

typedef enum
{
  LIVETV_UNINIT,
  LIVETV_INITING,
  LIVETV_INIT_SUCCESS,
  LIVETV_INIT_FAILED,
}livetv_init_sts;

typedef enum
{
  COMMON_PLAY = 0,
  FULLSCREEN_PLAY = 65536,
  LOOKBACK_PLAY,  
}entry_livetv_flag;

typedef enum
{
  COMMON_MODE = 0,
  FAV_MODE,
  LOOKBACK_MODE,
  FULLSCREEN_MODE,
}livetv_play_mode;
/*********************************************************
**************all lookback names structs*************************
**********************************************************/
//look back all pg names structs
typedef struct
{
  char prog_title[MAX_TITLE_LEN];
}ui_look_back_name_t;

typedef struct
{
  int chan_num;
  ui_look_back_name_t channels[MAX_LOOKBACK_CHANNEL];
}ui_look_back_name_list_t;

/*********************************************************
***********lookback channels info structs*************************
**********************************************************/
//lookback channel info struct
typedef struct
{
  char na[MAX_TITLE_LEN];
  char time[MAX_TITLE_LEN];
  int chpcount;//lookback url piece count, one piece movie only 5mins
  char url[MAX_LOOKBACK_URL_CNT][MAX_LOOKBACK_URL_LEN];
}ui_look_back_prog_ms_t;

typedef struct
{
  char prog_title[MAX_TITLE_LEN];
  int  pgms_num;
  ui_look_back_prog_ms_t pgms[MAX_LOOKBACK_PGMS_NUM];
}ui_look_back_chan_info_t;

/*********************************************************
*************************epg structs*************************
**********************************************************/
//liveTV EPG info struct
typedef struct
{
  char na[MAX_TITLE_LEN];
  char st[MAX_TITLE_LEN];
  u16 start_min;
}ui_epg_pgms_t;

typedef struct
{
  int  cid;//channel id, match chan_id
  char title[MAX_TITLE_LEN];
  int  pgms_num;
  ui_epg_pgms_t pgms[MAX_EPG_NUM];
}ui_epg_chan_info_t;

/*********************************************************
*************************other structs***********************
**********************************************************/
typedef struct
{
  u16 year;
  u8 month;
  u8 day;
}ui_live_tv_time_t;

typedef struct
{ 
  u16 epg_chann_index;
  u16 epg_detail_index;
} today_epg_info_t;

typedef  struct
{
  u8 *name;
  u8 *url;
} livetv_url;

typedef enum
{
  ADD_FAV,
  DEL_FAV,
  FULL_FAV,
}livetv_fav_sts;

/*********************************************************
*************************functions *************************
**********************************************************/
void ui_live_tv_init();
void ui_live_tv_deinit(void);
LIVE_TV_DP_HDL_T *get_live_tv_handle();
void get_several_days_time(ui_live_tv_time_t *days, u8 day_cnt);

RET_CODE ui_live_tv_api_group_arrived(u32 param);
RET_CODE ui_live_tv_api_all_chan_arrived(LIVE_TV_CHANNEL_LIST_T * param);
RET_CODE ui_live_tv_api_all_chan_zip_load();
RET_CODE ui_live_tv_api_epg_arrived(u32 param);
RET_CODE ui_live_tv_api_look_back_arrived(u32 param);
RET_CODE ui_live_tv_api_one_pg_one_day_epg_load(ui_live_tv_time_t *day, u16 chan_id, u8 *pg_name);
RET_CODE ui_live_tv_api_one_pg_one_day_epg_arrived(u32 param);
ui_epg_chan_info_t *livetv_api_get_one_pg_one_day_epg();
void init_livetv_one_pg_one_day_epg_param();
void deinit_livetv_one_pg_one_day_epg_param();
RET_CODE ui_live_tv_api_all_lookbk_names_load(ui_live_tv_time_t *day);
ui_look_back_name_list_t *livetv_api_get_all_lookbk_names();
ui_look_back_chan_info_t *livetv_api_get_one_pg_one_day_lookbk_info();
RET_CODE ui_live_tv_api_look_back_names_arrived(u32 param);
RET_CODE ui_live_tv_api_one_pg_one_day_lookbk_arrived(u32 param);
RET_CODE ui_live_tv_api_one_pg_one_day_lookbk_load(ui_live_tv_time_t *day, ui_live_tv_time_t *today, u8 *pg_name);

void ui_live_tv_get_categorylist();
void ui_livetv_get_load_media_param(u32 param, u16 *pg_id, u16 *url_index);
void ui_livetv_set_load_media_param(u32 *param, u16 pg_id, u16 url_index);
void ui_live_tv_init_pg_pos();



////////////just for build
BOOL iptv_get_channel_info_is_ready(void);


#endif

