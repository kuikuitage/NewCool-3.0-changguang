#include "ui_common.h"
#include "ui_config.h"
#include "commonData.h"
#include "utildataprovider.h"
#include "ui_live_tv_api.h"
#ifndef WIN32
#include "download_api.h"
#endif

#define ENABLE_LIVETV_DEBUG 0
enum
{
  LIVE_TV_EVT_CATEGORY_ARRIVAL = ((APP_LIVE_TV << 16) + 0),
  LIVE_TV_EVT_GET_ZIP,
  LIVE_TV_EVT_GET_ONE_PG_ONE_DAY_EPG,
  LIVE_TV_EVT_GET_LOOKBACK_NAMES,
  LIVE_TV_EVT_GET_ONE_PG_ONE_DAY_LOOKBACK,
  LIVE_TV_INIT_SUCCESS,
  LIVE_TV_INIT_FAILED,
  LIVE_TV_EVT_GET_CHANNELLIST,
};

static LIVE_TV_DP_HDL_T  *sg_live_tv_handle = NULL;
static ui_epg_chan_info_t *sg_1day_epg = NULL;
static ui_look_back_name_list_t *sg_all_lookbk_name = NULL;
static ui_look_back_chan_info_t *sg_lookbk_info = NULL;
u8 *p_livetv_stack_buf = NULL;
static BOOL is_init_epg = FALSE;

/************************************************************************
****************************livetv get/set load media param*********************
************************************************************************/
BOOL iptv_get_channel_info_is_ready(void)
{
    return FALSE;
}

void ui_livetv_get_load_media_param(u32 param, u16 *pg_id, u16 *url_index)
{
  *pg_id = param >> 16;  //high 16bit saved prog id
  *url_index = param & 0xffff;  //low 16 bit saved url index
}
void ui_livetv_set_load_media_param(u32 *param, u16 pg_id, u16 url_index)
{
  *param = pg_id << 16 | url_index;
}
RET_CODE ui_live_tv_api_all_chan_arrived(LIVE_TV_CHANNEL_LIST_T * param)
{
  u16 i = 0;
  u8 j = 0;
  live_db_prg_t chl_t = {0};
  u16 name_len = 0;
  LIVE_TV_CHANNEL_LIST_T *p_chl = (LIVE_TV_CHANNEL_LIST_T *)param;
  MT_ASSERT(NULL != p_chl);
  OS_PRINTF("##%s, totalcount == %d#\n", __FUNCTION__, p_chl->totalcount);
  for(i = 0; i < MIN(p_chl->totalcount, MAX_LIVE_PRG); i++)
  {
    chl_t.id          = p_chl->array[i].id;
    chl_t.sate_id = p_chl->array[i].satelliteId;
    #if ENABLE_LIVETV_DEBUG
    OS_PRINTF("##%s, i = [%d], satelliteId = [%d]##\n", __FUNCTION__, i, p_chl->array[i].satelliteId);
    #endif
    chl_t.sate_id = 0;
    if(chl_t.sate_id > 0)
{
      chl_t.sate_apid = p_chl->array[i].satellite_apid;
      chl_t.sate_vpid = p_chl->array[i].satellite_vpid;
      chl_t.sate_pcrpid = p_chl->array[i].satellite_pcrpid;
      chl_t.sate_freq = p_chl->array[i].satellite_freq;
      chl_t.sate_sym = p_chl->array[i].satellite_symbol_rate;
      chl_t.sate_polarity = p_chl->array[i].satellite_polarity;
      chl_t.sate_tv_flag = p_chl->array[i].satellite_type;
      #if ENABLE_LIVETV_DEBUG
      OS_PRINTF("##i=[%d],sate_id=[%d],sate_apid=[%d],sate_vpid=[%d],sate_pcrpid=[%d],sate_freq=[%d],sate_sym=[%d],sate_polarity=[%d],sate_tv_flag=[%d]##\n",\
        i, chl_t.sate_id,chl_t.sate_apid, chl_t.sate_vpid, chl_t.sate_pcrpid, chl_t.sate_freq,\
        chl_t.sate_sym,chl_t.sate_polarity,chl_t.sate_tv_flag);
      #endif
}
    livetv_db_get_group_view_id(&(chl_t.grp_view_id), p_chl->array[i].category_id);
    chl_t.url_cnt = MIN(p_chl->array[i].num, MAX_LIVE_SRC_PER_PG);
    if(p_chl->array[i].title != NULL)
    {
      name_len = strlen(p_chl->array[i].title);
      name_len = (name_len >= MAX_LIVE_NAME_LEN) ? (MAX_LIVE_NAME_LEN - 1) : name_len;
      strncpy(chl_t.name, p_chl->array[i].title, name_len); 
      chl_t.name[name_len] = '\0';
    }
    #if ENABLE_LIVETV_DEBUG
    OS_PRINTF("##index = %d, group id == %d,viewid= %d, channel id == %d, prog name == \
    %s##\n", i + 1, p_chl->array[i].category_id,chl_t.grp_view_id, chl_t.id,p_chl->array[i].title);
    #endif
    for(j = 0; j < chl_t.url_cnt; j++)
{
      if(p_chl->array[i].playurl[j] != NULL)
      {
        chl_t.url[j] = p_chl->array[i].playurl[j];
}
    }

    if(chl_t.url_cnt != 0)
{
      livetv_add_pg_info(&chl_t);
    }
    
  }
  livetv_db_create_view();
    return SUCCESS;
}

RET_CODE ui_live_tv_api_all_chan_zip_load()
{
  OS_PRINTF("##%s, start##\n", __FUNCTION__);
  #ifndef WIN32
  LiveTV_getLzmaChannelData(sg_live_tv_handle);
  #endif
  OS_PRINTF("##%s, end##\n", __FUNCTION__);
  return SUCCESS;
}
static void ui_live_tv_api_add_group(LIVE_TV_CATEGORY_LIST_T *p_group) 
{
  u8              i       = 0;
  live_db_group_t group_t = {0};
  u16 name_len = 0;
    
  livetv_clear_db();
  for(i = 0; i < MIN(p_group->num, MAX_LIVE_GROUP); i++)
  {
    group_t.id = p_group->array[i].id;
    if(p_group->array[i].name != NULL)
    {
      name_len = strlen(p_group->array[i].name);
      name_len = (name_len >= MAX_LIVE_NAME_LEN) ? (MAX_LIVE_NAME_LEN - 1) : name_len;
      strncpy(group_t.name, p_group->array[i].name, name_len);
      group_t.name[name_len] = '\0';
    }
    #if ENABLE_LIVETV_DEBUG
    OS_PRINTF("##i == %d, group id == %d, group name == %s, group name len = %d##\n", i+1, group_t.id, p_group->array[i].name, strlen(p_group->array[i].name));
    #endif
    livetv_write_group_info(&group_t);
  }  
}

RET_CODE ui_live_tv_api_group_arrived(u32 param)
{
  LIVE_TV_CATEGORY_LIST_T *p_group  = (LIVE_TV_CATEGORY_LIST_T *)param;

  if(NULL == p_group)
  {
    return ERR_FAILURE;
  }

  ui_live_tv_api_add_group(p_group);

  return SUCCESS;
}

/************************************************************************
***************************epg info function*********************************
************************************************************************/
void get_several_days_time(ui_live_tv_time_t *days, u8 day_cnt)
{
    utc_time_t time = {0};
    utc_time_t gmt_time = {0};
    time_set_t p_set={{0}};
    u8 i;
  
  ui_live_tv_time_t *day = (ui_live_tv_time_t *)days; 
    sys_status_get_time(&p_set);
    time_get(&time, p_set.gmt_usage);
    if( p_set.gmt_usage)
    {
        time_to_local(&time, &gmt_time);
        memcpy(&time,&gmt_time,sizeof(gmt_time));
    }
  day[0].year = time.year;
  day[0].month = time.month;
  day[0].day = time.day;

  for(i = 1; i < day_cnt; i++)
  {
    day[i].year = day[i-1].year;
    day[i].month = day[i-1].month;
    day[i].day = day[i-1].day - 1;
    if(day[i].day == 0)
    {
	  day[i].month = day[i-1].month - 1;
      switch(day[i-1].month)
      {
        
        case 1:
          day[i].year = day[i-1].year - 1;
          day[i].month = 12;
          day[i].day = 31;
          break;
        case 2:
        case 4:
        case 6:
        case 8:
        case 9:
        case 11:
          day[i].day = 31;
          break;
        case 3:
          if((day[i].year%4 == 0 && day[i].year%100 != 0) || (day[i].year%400 == 0))
            day[i].day = 29;
          else  
            day[i].day = 28;
          break;  
        case 5:
        case 7:
        case 10:
        case 12:
          day[i].day = 30;  
          break;
        default:
          break;
      }
    }
  }
}
ui_epg_chan_info_t *livetv_api_get_one_pg_one_day_epg()
{
  return sg_1day_epg;
}
static int get_epg_prog_start_time_min(u8 *time)
{
  int time_min;
  int hour, min;
    
  sscanf(time, "%d:%d",&hour,&min);  
  if(hour < 0)
  {
    hour = 24 - hour;
  }
  time_min = 60*hour + min;
  return time_min;
}

void init_livetv_one_pg_one_day_epg_param()
{
  if(!is_init_epg)
  {
    is_init_epg = TRUE;
    
    deinit_livetv_one_pg_one_day_epg_param();
    sg_1day_epg = (ui_epg_chan_info_t *)mtos_malloc(sizeof(ui_epg_chan_info_t));
    if(sg_1day_epg == NULL)
    {
        mtos_printk("##%s, can not malloc, line[%d]##\n", __FUNCTION__, __LINE__);
        return;
    }
    memset(sg_1day_epg, 0, sizeof(ui_epg_chan_info_t));
  }
}

void deinit_livetv_one_pg_one_day_epg_param()
{
  if(sg_1day_epg)
  {
    mtos_free(sg_1day_epg);
    sg_1day_epg = NULL;
  }
  
  is_init_epg = FALSE;
}

RET_CODE ui_live_tv_api_one_pg_one_day_epg_load(ui_live_tv_time_t *day, u16 chan_id, u8 *pg_name)
{
  LIVETV_EPG_CHANNEL_INFO_REQ req = {{0}};
  u16 name_len = 0;
  
  req.day.year = day->year;
  req.day.month = day->month;
  req.day.day = day->day;
  req.today.year = day->year;
  req.today.month = day->month;
  req.today.day = day->day;
  req.id = chan_id;
  if(pg_name != NULL)
  {
    name_len = strlen(pg_name);
    name_len = (name_len >= MAX_LIVE_NAME_LEN) ? (MAX_LIVE_NAME_LEN - 1) : name_len;
    strncpy(req.title, pg_name, name_len);
    req.title[name_len] = '\0';
  }
  OS_PRINTF("##%s, pg name = %s, data is:%4d-%2d-%2d##\n", __FUNCTION__,req.title,req.day.year,req.day.month,req.day.day);
  #ifndef WIN32
  if(sg_live_tv_handle == NULL)
    return SUCCESS;
  LiveTV_getChannelEpgInfo(sg_live_tv_handle, &req);
  #endif
  return SUCCESS;
}

RET_CODE ui_live_tv_api_one_pg_one_day_epg_arrived(u32 param)
{
  LIVETV_EPG_INFO_T *p_temp = NULL;
  EPG_CHANNEL_INFO_T *p_epg_info = NULL;
  u16 name_len = 0;
  u16 i;

  p_temp = (LIVETV_EPG_INFO_T *)param;
  if(p_temp == NULL)
  {
    return ERR_FAILURE;
  }
  p_epg_info = p_temp->channels;
  OS_PRINTF("###dddddddddd %x##\n",param);
  if(sg_1day_epg == NULL)
  {
    mtos_printk("##%s,line[%d], sg_1day_epg == NULL##\n", __FUNCTION__, __LINE__);
    return ERR_FAILURE;
  }
  memset(sg_1day_epg, 0, sizeof(ui_epg_chan_info_t));
  if(p_epg_info == NULL)
  {
    OS_PRINTF("###p_epg_info is null##\n");
    return ERR_FAILURE;
  }
  sg_1day_epg->cid = p_epg_info->cid;
  sg_1day_epg->pgms_num = MIN(p_epg_info->pgms_num, MAX_EPG_NUM);
  if(p_epg_info->title != NULL)
  {
    OS_PRINTF("##title addr:0x%x##\n", p_epg_info->title);
    name_len = strlen(p_epg_info->title);
    name_len = (name_len >= MAX_TITLE_LEN) ? (MAX_TITLE_LEN - 1) : name_len;
    strncpy(sg_1day_epg->title, p_epg_info->title, name_len);
    sg_1day_epg->title[name_len] = '\0';
  }
  else
  {
    OS_PRINTF("##title is null##\n");
    return ERR_FAILURE;
  }
  for(i = 0; i < sg_1day_epg->pgms_num; i++)
  {
    if(p_epg_info->pgms[i].na != NULL)
    {
      name_len = strlen(p_epg_info->pgms[i].na);
      name_len = (name_len >= MAX_TITLE_LEN) ? (MAX_TITLE_LEN - 1) : name_len;
      strncpy(sg_1day_epg->pgms[i].na, p_epg_info->pgms[i].na, name_len);
      sg_1day_epg->pgms[i].na[name_len] = '\0';
    }
    else
    {
      OS_PRINTF("##name[%d] is null##\n", i);
      return ERR_FAILURE;
    }
    if(p_epg_info->pgms[i].st != NULL)
    {
      name_len = strlen(p_epg_info->pgms[i].st);
      name_len = (name_len >= MAX_TITLE_LEN) ? (MAX_TITLE_LEN - 1) : name_len;
      strncpy(sg_1day_epg->pgms[i].st, p_epg_info->pgms[i].st, name_len);
      sg_1day_epg->pgms[i].st[name_len] = '\0';
      sg_1day_epg->pgms[i].start_min = get_epg_prog_start_time_min(sg_1day_epg->pgms[i].st); 
    }
    else
    {
      OS_PRINTF("##start_time[%d] is null##\n", i);
      sg_1day_epg->pgms[i].start_min = 0;
    }
  }

  p_temp->can_destroy = 1;
    
  return SUCCESS;
}

char * cut_out_string_from_mid(char *dst,char *src, int n,int m)
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) 
      n = len-m;    
    if(m<0) 
      m=0;  
    if(m>len) 
      return NULL;
    p += m;
    while(n--) 
      *(q++) = *(p++);
    *(q++)='\0'; 
    return dst;
}

char *convert_look_up_time(u8 *dst, u8 *start, u8 *end)
{
  u8 temp_time1[16];
  u8 temp_time2[16];
  u8 temp_time3[16];
  u8 temp_time4[16];
  u8 *dst_str;

  dst_str = (u8 *)dst;
  if(end == NULL)
  {
    if(start == NULL)
    {
      sprintf(dst_str,"%s-%s","00:00", "00:00");
    }
    else
    {
      sprintf(dst_str,"%s", start);
    }
    return dst;
  }
  cut_out_string_from_mid(temp_time1, start, 2, 8);
  cut_out_string_from_mid(temp_time2, start, 2, 10);
  sprintf(temp_time3, "%s:%s",temp_time1,temp_time2);
  cut_out_string_from_mid(temp_time1, end, 2, 8);
  cut_out_string_from_mid(temp_time2, end, 2, 10);
  sprintf(temp_time4, "%s:%s",temp_time1,temp_time2);
  sprintf(dst_str,"%s-%s",temp_time3, temp_time4);

  return dst;
}

ui_look_back_name_list_t *livetv_api_get_all_lookbk_names()
{
  return sg_all_lookbk_name;
}

ui_look_back_chan_info_t *livetv_api_get_one_pg_one_day_lookbk_info()
{
  return sg_lookbk_info;
}

RET_CODE ui_live_tv_api_all_lookbk_names_load(ui_live_tv_time_t *day)
{
  LIVETV_PB_NAMES_REQ req = {{0}};
  
  req.day.year = day->year;
  req.day.month = day->month;
  req.day.day = day->day;
  
  OS_PRINTF("##%s,data is:%4d-%2d-%2d##\n", __FUNCTION__, req.day.year,req.day.month,req.day.day);
  #ifndef WIN32
   LiveTV_getLookbackChannelNames(sg_live_tv_handle, &req);
  #endif
  return SUCCESS;
}

RET_CODE ui_live_tv_api_look_back_names_arrived(u32 param)
{
  LIVETV_PB_NAMES_LIST_T *p_name = NULL;
  u16 name_len = 0;
  int i;
  
  OS_PRINTF("##%s##\n", __FUNCTION__);
  p_name = (LIVETV_PB_NAMES_LIST_T *)param;
  if(p_name == NULL)
  {
    return ERR_FAILURE;
  }

  if(sg_all_lookbk_name == NULL)
  {
    mtos_printk("##%s, line[%d],sg_all_lookbk_name == NULL ##\n", __FUNCTION__, __LINE__);
    return ERR_FAILURE;
  }
  
  sg_all_lookbk_name->chan_num = MIN(p_name->channel_number, MAX_LOOKBACK_CHANNEL);
  for(i = 0; i < sg_all_lookbk_name->chan_num; i++)
  {
    if(p_name->channels[i].title != NULL)
    {
      if(p_name->channels[i].title != NULL)
      {
        name_len = strlen(p_name->channels[i].title);
        name_len = (name_len >= MAX_TITLE_LEN) ? (MAX_TITLE_LEN - 1) : name_len;
        strncpy(sg_all_lookbk_name->channels[i].prog_title, p_name->channels[i].title, name_len);
        sg_all_lookbk_name->channels[i].prog_title[name_len] = '\0';
      }
    }
#if ENABLE_LIVETV_DEBUG
    OS_PRINTF("##lookback title:%s##\n", sg_all_lookbk_name->channels[i].prog_title);
#endif
  }
  return SUCCESS;
}

RET_CODE ui_live_tv_api_one_pg_one_day_lookbk_load(ui_live_tv_time_t *day, ui_live_tv_time_t *today, u8 *pg_name)
{
  LIVETV_PB_CHANNEL_INFO_REQ req = {{0}};
  u16 name_len = 0;
  
  req.day.year = day->year;
  req.day.month = day->month;
  req.day.day = day->day;
  req.today.year = today->year;
  req.today.month = today->month;
  req.today.day = today->day;
  if(pg_name != NULL)
  {
    name_len = strlen(pg_name);
    name_len = (name_len >= MAX_TITLE_LEN) ? (MAX_TITLE_LEN - 1) : name_len;
    strncpy(req.title, pg_name, name_len);
    req.title[name_len] = '\0';
  }
  OS_PRINTF("##%s, line:%d##\n", __FUNCTION__, __LINE__);
  OS_PRINTF("##pg name = %s, data is:%4d-%2d-%2d###\n", req.title, req.day.year,req.day.month,req.day.day);
  #ifndef WIN32
  LiveTV_getLookbackChannelInfo(sg_live_tv_handle, &req);
  #endif
  
  return SUCCESS;
}

RET_CODE ui_live_tv_api_one_pg_one_day_lookbk_arrived(u32 param)
{
  PLAYBACK_CHANNEL_INFO_T *p_playback_info = NULL;
  u16 name_len = 0;
  u16 i, j;
  
  OS_PRINTF("##%s, %d##\n", __FUNCTION__, __LINE__);
  p_playback_info = (PLAYBACK_CHANNEL_INFO_T *)param;
  mtos_printk("##[%s], p_playback_info addr = 0x%x##\n", __FUNCTION__, p_playback_info);
  //MT_ASSERT(sg_lookbk_info != NULL);
  if(sg_lookbk_info == NULL)
  {
    mtos_printk("##%s, line[%d], sg_lookbk_info == NULL##\n", __FUNCTION__, __LINE__);
    return ERR_FAILURE;
  }
  
  memset(sg_lookbk_info, 0, sizeof(ui_look_back_chan_info_t));
  if(p_playback_info == NULL)
  {
    return ERR_FAILURE;
  }
  sg_lookbk_info->pgms_num = MIN(p_playback_info->pgms_num, MAX_LOOKBACK_PGMS_NUM);
  if(p_playback_info->title != NULL)
  {
    name_len = strlen(p_playback_info->title);
    name_len = (name_len >= MAX_TITLE_LEN) ? (MAX_TITLE_LEN - 1) : name_len;
    strncpy(sg_lookbk_info->prog_title, p_playback_info->title, name_len);
    sg_lookbk_info->prog_title[name_len] = '\0';
  }
  for(i = 0; i < sg_lookbk_info->pgms_num; i++)
  {
    if(p_playback_info->pgms[i].na != NULL)
    {
      name_len = strlen(p_playback_info->pgms[i].na);
      name_len = (name_len >= MAX_TITLE_LEN) ? (MAX_TITLE_LEN - 1) : name_len;
      strncpy(sg_lookbk_info->pgms[i].na, p_playback_info->pgms[i].na, name_len);
      sg_lookbk_info->pgms[i].na[name_len] = '\0';
    }
    convert_look_up_time(sg_lookbk_info->pgms[i].time,p_playback_info->pgms[i].st, p_playback_info->pgms[i].et);
    sg_lookbk_info->pgms[i].chpcount = MIN(MAX_LOOKBACK_URL_CNT, p_playback_info->pgms[i].chpcount);
#if ENABLE_LIVETV_DEBUG
    OS_PRINTF("##chpcount = %d, start time:%s, na:%s##\n", p_playback_info->pgms[i].chpcount,\
        p_playback_info->pgms[i].st,p_playback_info->pgms[i].na);
#endif
    for(j = 0; j < sg_lookbk_info->pgms[i].chpcount; j++)
    {
      if(p_playback_info->pgms[i].url[j] != NULL)
      {
        name_len = strlen(p_playback_info->pgms[i].url[j]);
        name_len = (name_len >= MAX_LOOKBACK_URL_LEN) ? (MAX_LOOKBACK_URL_LEN - 1) : name_len;
        strncpy(sg_lookbk_info->pgms[i].url[j], p_playback_info->pgms[i].url[j], name_len); 
        sg_lookbk_info->pgms[i].url[j][name_len] = '\0';
      }
#if ENABLE_LIVETV_DEBUG
      OS_PRINTF("##i = %d,  url[%d] = %s##\n", i, j, sg_lookbk_info->pgms[i].url[j]);
#endif
    }
  }
      
  return SUCCESS;
  
}

/************************************************************************
**************************livetv callback function******************************
************************************************************************/
u16 ui_live_tv_evtmap(u32 event);
static RET_CODE  live_tv_event_callback(TVLIVE_EVENT_TYPE event, u32 param) 
{
  event_t evt = {0};
  
  OS_PRINTF("%s start\n", __FUNCTION__);
  
  switch(event) 
  {
    case PB_CHANNELS_NAME_ARRIVAL:
      evt.id = LIVE_TV_EVT_GET_LOOKBACK_NAMES;
      evt.data1 = (u32)param;
      break;
    case PB_CHANNEL_INFO_ARRIVAL:
      evt.id = LIVE_TV_EVT_GET_ONE_PG_ONE_DAY_LOOKBACK;
      evt.data1 = (u32)param;
      break;
    case EPG_CHANNEL_INFO_ARRIVAL:
      evt.id = LIVE_TV_EVT_GET_ONE_PG_ONE_DAY_EPG;
      evt.data1 = (u32)param;
      break;
      
    case CATEGORY_ARRIVAL:
      evt.id = LIVE_TV_EVT_CATEGORY_ARRIVAL;
      evt.data1 = (u32)param;
      break;
      
    case PLAY_CHANNEL_ZIP_ARRIVAL:
      evt.id = LIVE_TV_EVT_GET_ZIP;
      evt.data1 = (u32)param;
      break;
      
    case DATA_PROVIDER_INIT_SUCCESS:
      evt.id = LIVE_TV_INIT_SUCCESS;
      break;
      
    case DATA_PROVIDER_INIT_FAIL:
      evt.id = LIVE_TV_INIT_FAILED;
      break;

    case ALL_PLAY_CHANNEL_ARRIVAL:
      evt.id = LIVE_TV_EVT_GET_CHANNELLIST;
      evt.data1 = (u32)param;
      break;      
      
    default:
      break;
  }
  
  ap_frm_send_evt_to_ui(APP_LIVE_TV, &evt);
  OS_PRINTF("%s end\n", __FUNCTION__);
  return SUCCESS;
}

/************************************************************************
**************************livetv init function*********************************
************************************************************************/
LIVE_TV_DP_HDL_T  *get_live_tv_handle()
{
  return sg_live_tv_handle;
}

void register_livetv_evtmap()
{
  fw_register_ap_evtmap(APP_LIVE_TV, ui_live_tv_evtmap);
  fw_register_ap_msghost(APP_LIVE_TV, ROOT_ID_LIVE_TV);  
  fw_register_ap_msghost(APP_LIVE_TV, ROOT_ID_BACKGROUND);  
}

void unregister_livetv_evtmap()
{
  fw_unregister_ap_evtmap(APP_LIVE_TV);
  fw_unregister_ap_msghost(APP_LIVE_TV, ROOT_ID_LIVE_TV); 
  fw_unregister_ap_msghost(APP_LIVE_TV, ROOT_ID_BACKGROUND); 
}

void ui_live_tv_init()
{
  DO_CMD_TASK_CONFIG_T task_param = {0,};

  memset(&task_param, 0x00, sizeof(DO_CMD_TASK_CONFIG_T));
  OS_PRINTF("##%s, start##\n", __FUNCTION__);
  #ifndef WIN32
  if(sg_live_tv_handle == NULL)
  {
    task_param.priority = LIVE_TV_PRIORITY;
    task_param.http_priority = LIVE_TV_HTTP_PRIORITY;
    task_param.misc_priotiry = MISC_PRIORITY;
    task_param.stack_size = LIVE_TV_TASK_STACK_SIZE;
    if(p_livetv_stack_buf)
    {
      mtos_free(p_livetv_stack_buf);
      p_livetv_stack_buf = NULL;
    }
    p_livetv_stack_buf = (char *)mtos_malloc(task_param.stack_size);
    MT_ASSERT(p_livetv_stack_buf != NULL);
    memset(p_livetv_stack_buf, 0, task_param.stack_size);
    task_param.p_mem_start = p_livetv_stack_buf;
    task_param.dp = BSW_IPTV_DP;
    task_param.sub_dp = SUB_DP_ID;
    task_param.mac = mtos_malloc(MAC_CHIPID_LEN);
    task_param.id = mtos_malloc(MAC_CHIPID_LEN);
    ui_get_stb_mac_addr_chip_id(task_param.mac, task_param.id);
    sg_live_tv_handle =  LiveTV_initDataProvider((void *)&task_param);
    if(sg_live_tv_handle != NULL)
    {
      sg_live_tv_handle->register_event_callback(sg_live_tv_handle, live_tv_event_callback);
    }
    else
    {
      OS_PRINTF("##%s, line[%d], init handle is null##\n", __FUNCTION__, __LINE__);
      ui_set_init_livetv_status(LIVETV_INIT_FAILED);
    }
    register_livetv_evtmap();
    mtos_free(task_param.id);
    mtos_free(task_param.mac);
  }
  else
  {
    OS_PRINTF("###%s, is_init = %d##\n", __FUNCTION__, sg_live_tv_handle->is_init);
    
    ui_live_tv_deinit();
    task_param.priority = LIVE_TV_PRIORITY;
    task_param.http_priority = LIVE_TV_HTTP_PRIORITY;
    task_param.misc_priotiry = MISC_PRIORITY;
    task_param.stack_size = LIVE_TV_TASK_STACK_SIZE;
    if(p_livetv_stack_buf)
    {
      mtos_free(p_livetv_stack_buf);
      p_livetv_stack_buf = NULL;
    }
    p_livetv_stack_buf = (char *)mtos_malloc(task_param.stack_size);
    MT_ASSERT(p_livetv_stack_buf != NULL);
    memset(p_livetv_stack_buf, 0, task_param.stack_size);
    task_param.p_mem_start = p_livetv_stack_buf;
    task_param.dp = BSW_IPTV_DP;
    task_param.sub_dp = SUB_DP_ID;
    task_param.mac = mtos_malloc(MAC_CHIPID_LEN);
    task_param.id =mtos_malloc(MAC_CHIPID_LEN);
    ui_get_stb_mac_addr_chip_id(task_param.mac, task_param.id);
    sg_live_tv_handle =  LiveTV_initDataProvider((void *)&task_param);
    if(sg_live_tv_handle != NULL)
    {
      sg_live_tv_handle->register_event_callback(sg_live_tv_handle, live_tv_event_callback);
    }
    else
    {
      OS_PRINTF("##%s, line[%d], init handle is null##\n", __FUNCTION__, __LINE__);
      ui_set_init_livetv_status(LIVETV_INIT_FAILED);
    }
    register_livetv_evtmap();
    mtos_free(task_param.id);
    mtos_free(task_param.mac);
  }
  OS_PRINTF("##%s, end##\n", __FUNCTION__);
#endif  

}

void ui_live_tv_get_categorylist()
{
  #ifndef WIN32
  LiveTV_getCategoryList(sg_live_tv_handle);
  #endif
}

void ui_live_tv_deinit(void)
{
#ifndef WIN32
  if (sg_live_tv_handle != NULL)
  {
    LiveTV_deinitDataProvider((void  *)sg_live_tv_handle);
    if(p_livetv_stack_buf)
    {
      mtos_free(p_livetv_stack_buf);
      p_livetv_stack_buf = NULL;
    }
    sg_live_tv_handle = NULL;
  }
  unregister_livetv_evtmap();
#endif
}

BEGIN_AP_EVTMAP(ui_live_tv_evtmap)
  CONVERT_EVENT(LIVE_TV_EVT_CATEGORY_ARRIVAL, MSG_GET_LIVE_TV_CATEGORY_ARRIVAL)
  CONVERT_EVENT(LIVE_TV_EVT_GET_ZIP, MSG_GET_LIVE_TV_GET_ZIP)
  CONVERT_EVENT(LIVE_TV_EVT_GET_ONE_PG_ONE_DAY_EPG, MSG_GET_LIVE_TV_ONE_PG_ONE_DAY_EPG)
  CONVERT_EVENT(LIVE_TV_EVT_GET_LOOKBACK_NAMES, MSG_GET_LIVE_TV_LOOKBACK_NAMES)
  CONVERT_EVENT(LIVE_TV_EVT_GET_ONE_PG_ONE_DAY_LOOKBACK, MSG_GET_LIVE_TV_ONE_PG_ONE_DAY_LOOKBACK)
  CONVERT_EVENT(LIVE_TV_INIT_SUCCESS, MSG_INIT_LIVETV_SUCCESS)
  CONVERT_EVENT(LIVE_TV_INIT_FAILED, MSG_INIT_LIVETV_FAILED)
  CONVERT_EVENT(LIVE_TV_EVT_GET_CHANNELLIST, MSG_GET_LIVE_TV_CHANNELLIST)
END_AP_EVTMAP(ui_live_tv_evtmap)



