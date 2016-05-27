/******************************************************************************/
/******************************************************************************/
#include "string.h"
#include "stdio.h"

#include "sys_types.h"
#include "sys_define.h"

#include "lib_char.h"
#include "lib_unicode.h"
#include "lib_util.h"

#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_printk.h"

#include "drv_dev.h"
#include "nim.h"

#include "mdl.h"
#include "class_factory.h"
#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "cat.h"
#include "nit.h"
#include "pmt.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"

#include "data_base.h"
#include "db_dvbs.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "mem_manager.h"
#include "ap_framework.h"
#include "ap_scan.h"
#include "ap_dvbs_scan.h"
#include "ss_ctrl.h"
#include "sys_status.h"

/*!
    Max pg number supported in one tp
    */
#define MAX_PG_NUM_PER_SAT (512)

/*!
  Frequency deviation tolerance
  */
//#define TOLERANCE(x)  (((x) > 20000) ? (5) : ((x/5000)+1))
/*!
  Maximum symbol rate offset
  */
#define MAX_SYM_OFFSET(x)  ((x) << 3)/100

/*!
  default volume
  */
#define DEFAULT_VOLUME (16)

#define ALIGN4(x)    (((x) + 3) & (~3))

/*!
  Tp scan private data
  */
typedef struct
{
  /*!
    ALL pg view id
    */
  u8 pg_view_id;
  /*!
    All tp view id
    */
  u8 tp_view_id;
  /*!
    Current satelite id
    */
  u32 cur_sat_id;
  /*!
    Current satelite id
    */
  u32 cur_tp_id;
  /*!
    tp total in tp_list
    */
  u16 tp_depth;
  /*!
    pg total in tp_list
    */
  u16 pg_depth;
  /*!
    sat info
    */
  sat_node_t cur_sat_info;
  /*!
    program list
    */
  dvbs_prog_node_t pg_list[MAX_PG_NUM_PER_SAT];
  /*!
    tp list
    */
  dvbs_tp_node_t tp_list[MAX_TP_NUM_PER_SAT];
}scan_impl_data_t;

/*!
  Tp scan private data
  */
typedef struct
{
  BOOL using_attach_block;
  u32 block_id;
  void *p_block_addr;
  u32 total_size;
  u32 using_size;
}attach_block_info_t;

/*!
  Scan input parameter
  */
scan_input_param_t *g_p_input_para = NULL;
/*!
  Global variable for saving tp scan private data
  */
scan_impl_data_t *g_p_scan_priv = NULL;
/*!
  attach buff
  */
attach_block_info_t g_attach_block = {0};

static dvbs_tp_node_t cur_tp = {0};
/*!
  Create tp array
  */
static void create_tp_array(u32 sat_id)
{
  u16 view_cnt = 0;
  u8  view_id  = 0;
  u16 tp_cnt   = 0;
  u16 tp_id    = 0;
  dvbs_tp_node_t *p_tp_list = g_p_scan_priv->tp_list;
  db_dvbs_ret_t db_ret = DB_DVBS_OK;

  //Reset tp array
  memset(p_tp_list, 0xFF, sizeof(dvbs_tp_node_t)*MAX_TP_NUM_PER_SAT);
  view_id = db_dvbs_create_view(DB_DVBS_SAT_TP, sat_id, NULL);
  view_cnt = db_dvbs_get_count(view_id);
  for(tp_cnt = 0; tp_cnt < view_cnt; tp_cnt++)
  {
    tp_id = db_dvbs_get_id_by_view_pos(view_id, tp_cnt);
    MT_ASSERT(tp_id != DB_UNKNOWN_ID);
    db_ret = db_dvbs_get_tp_by_id(tp_id, p_tp_list + tp_cnt);
    MT_ASSERT(db_ret == DB_DVBS_OK);
  }
  g_p_scan_priv->tp_depth = view_cnt;
  g_p_scan_priv->tp_view_id = view_id;
}

/*!
  Create pg array
  */
static u32 create_pg_array(u32 tp_id)
{
  u16 view_cnt = 0;
  u8  view_id = 0;
  u16 pg_cnt = 0;
  u16 pg_id = 0;
  dvbs_prog_node_t *p_pg_list = g_p_scan_priv->pg_list;
  db_dvbs_ret_t db_ret = DB_DVBS_OK;
  
  //Memset pg array to 0xFF
  memset(p_pg_list, 0xFF, sizeof(dvbs_prog_node_t)*MAX_PG_NUM_PER_SAT);
  view_id = db_dvbs_create_view(DB_DVBS_TP_PG, tp_id, NULL);
  view_cnt = db_dvbs_get_count(view_id);
  
  if(view_cnt >= MAX_PG_NUM_PER_SAT)
   {
       return ERR_FAILURE;
   }  
  for(pg_cnt = 0; pg_cnt < view_cnt; pg_cnt++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, pg_cnt);
    MT_ASSERT(pg_id != DB_UNKNOWN_ID);
    db_ret = db_dvbs_get_pg_by_id(pg_id, p_pg_list + pg_cnt);
    MT_ASSERT(db_ret == DB_DVBS_OK);
  }
  g_p_scan_priv->pg_depth = view_cnt;
  g_p_scan_priv->pg_view_id = view_id;
  return SUCCESS;
}

static BOOL is_new_tp_dvbt(dvbs_tp_node_t *p_new_tp, u16 *p_sim_idx)
{
  u16 index = 0;
  dvbs_tp_node_t old_tp = {0};
  u16 cnt = 0;
  u16 pos = 0;

  cnt = db_dvbs_get_count(g_p_scan_priv->tp_view_id);

  for(index = 0; index < cnt; index++)
  {
    pos = db_dvbs_get_id_by_view_pos(g_p_scan_priv->tp_view_id, index);
    db_dvbs_get_tp_by_id(pos, &old_tp);
    if((old_tp.freq == p_new_tp->freq) &&
       (old_tp.sym == p_new_tp->sym))
    {
      *p_sim_idx = pos;
      return FALSE;
    }
  }
  return TRUE;
}

/*!
  Check whether input tp information is a new tp
  \param[in] p_tp_info input tp information
  */
static BOOL is_new_tp_dvbc(dvbs_tp_node_t *p_new_tp, u16 *p_sim_idx)
{
  u16 index = 0;
  dvbs_tp_node_t old_tp = {0};
  u16 cnt = 0;
  u16 pos = 0;

  cnt = db_dvbs_get_count(g_p_scan_priv->tp_view_id);

  for(index = 0; index < cnt; index++)
  {
    pos = db_dvbs_get_id_by_view_pos(g_p_scan_priv->tp_view_id, index);
    db_dvbs_get_tp_by_id(pos, &old_tp);
    if((old_tp.freq == p_new_tp->freq) &&
       (old_tp.nim_modulate == p_new_tp->nim_modulate) &&
       (old_tp.sym == p_new_tp->sym))
    {
      *p_sim_idx = pos;
      return FALSE;
    }
  }
  return TRUE;
}

static BOOL is_new_tp(dvbs_tp_node_t *p_new_tp, u16 *p_sim_idx)
{
  if(p_new_tp->nim_type == NIM_DVBC)
  {
    return is_new_tp_dvbc(p_new_tp, p_sim_idx);
  }
  else
  {
    return is_new_tp_dvbt(p_new_tp, p_sim_idx);
  }
}

/*!
  Check whether input pg is new program
  \param[in] program information
  */
static BOOL is_new_pg(scan_pg_info_t *p_pg_info, u16 *p_sim_idx)
{
  u16 index = 0;

  for(index = 0; index < g_p_scan_priv->pg_depth; index++)
  {
    dvbs_prog_node_t *p_pg = g_p_scan_priv->pg_list + index;
    if((p_pg->s_id == p_pg_info->s_id) &&
//      (p_pg->ts_id == p_pg_info->ts_id) &&
      (p_pg->tp_id == g_p_scan_priv->cur_tp_id ))
    {
      //ts_id from sdt,maybe lost because of poor signal
      *p_sim_idx = index;
      return FALSE;
    }
  }
  return TRUE;
}
u32 check_service_type(scan_pg_info_t *p_pg_info)
{
  //it's error flag, maybe the stream not has the info
  if((p_pg_info->service_type == 0) || (p_pg_info->service_type == SVC_TYPE_TV))
  {
    return (p_pg_info->video_pid == 0) ? SVC_TYPE_RADIO : SVC_TYPE_TV;
  }
  else if((p_pg_info->service_type == SVC_TYPE_DATA) || (p_pg_info->service_type == SVC_TYPE_DATA))
    {
         return  (p_pg_info->audio[0].p_id != 0) ? SVC_TYPE_RADIO : SVC_TYPE_TRESERVED;
    }
  else
  {
    return p_pg_info->service_type;
  }
}

/*!
  Add program information into database
  \param[in] scan_pg_info_t
  */
extern u16 find_logic_number_by_sid(u16 sID);
#ifdef AISET_BOUQUET_SUPPORT
extern u8 find_show_flag_by_sid(u16 sID);
#endif

#ifndef SUPPORT_NIGNORE_PRECODE
static BOOL str_isvalidchar(u8 src)
{
    if(src < 0x20)
        return FALSE;

    return TRUE;
}

static u32 str_uni_filter(u8 *p_dest, u8 *p_src)
{
    u32 i = 0, j = 0;
    MT_ASSERT(p_dest != NULL && p_src != NULL);

    while(p_src[i] != 0)
    {
        if(str_isvalidchar(p_src[i]))
        {
            p_dest[j] = p_src[i];
            j ++;
        }
        i++;
    }
    p_dest[j] = 0;

    return j;
}
#else
static BOOL impl_string_is_utf8(char *rawtext,int len)
{
  s32 score = 0;
  s32 i = 0, rawtextlen = 0;
  s32 goodbytes = 0, asciibytes = 0;
  // Maybe also use UTF8 Byte Order Mark: EF BB BF
  // Check to see if characters fit into acceptable ranges
  rawtextlen = len;
  for(i = 0; i < rawtextlen; i++)
  {
    if((rawtext[i] & 0x7F) == rawtext[i])
    {
      asciibytes++;  // Ignore ASCII, can throw off count
    }
    else if(-64 <= rawtext[i] && rawtext[i] <= -33
      &&i + 1 < rawtextlen && -128 <= rawtext[i + 1]
      && rawtext[i + 1] <= -65)
    {
      goodbytes += 2;
      i++;
    }
    else if(-32 <= rawtext[i]
      && rawtext[i] <= -17
      && // Three bytes
      i + 2 < rawtextlen && -128 <= rawtext[i + 1]
      && rawtext[i + 1] <= -65 && -128 <= rawtext[i + 2]
      && rawtext[i + 2] <= -65)
    {
      goodbytes += 3;
      i += 2;
    }
  }
  if(asciibytes == rawtextlen)
  {
    return FALSE;
  }
  score = 100 * goodbytes / (rawtextlen - asciibytes);
  // If not above 98, reduce to zero to prevent coincidental matches
  // Allows for some (few) bad formed sequences
  if(score > 98)
    return TRUE;
  else if (score > 95 && goodbytes > 30)
    return TRUE;
  else
    return FALSE;

}

static void impl_string_utf82uni(u8 *in, u16 *out, u16 out_size)
{
    u16 index = 0;
    
    while(*in && index <= out_size)
    {
        if (*in >= 0x00 && *in <= 0x7f)
        {
            *out = (u16) *in;
        }
        else if ((*in & (0xff << 5)) == 0xc0)
        {
            u16 unicode = 0;

            unicode = (*in & 0x1F) << 6; 
            in++;
            
            unicode |= (*in)  & 0x3F; 

            *out = unicode;
        }
        else if ((*in & (0xff << 4))== 0xe0)
        {
            u16 unicode = 0;

            unicode = ((*in) & 0x1F) << 12; 
            in++;

            unicode |= ((*in) & 0x3F) << 6; 
            in++;

            unicode |= ((*in) & 0x3F); 

            *out = unicode;
        }

        in++;
        out++;
        index ++;
    }

    out = 0;
}

#endif

static BOOL add_pg_into_db(scan_pg_info_t *p_pg_info)
{
  //Add program into database
  event_t evt = {0};
  db_dvbs_ret_t db_ret = DB_DVBS_OK;
  dvbs_prog_node_t *p_pg_node = g_p_scan_priv->pg_list
    + g_p_scan_priv->pg_depth;  //add into the end
  u8 str[16] = {0};
  u16 uni_str[32] = {0}, uni_str2[32] = {0};
  
  RET_CODE ret = SUCCESS;

  //check "HD" str
  strcpy(str, "HD");
  str_asc2uni(str, uni_str);
  #ifndef SUPPORT_NIGNORE_PRECODE
  {
  	u8 filter_name[MAX_SVC_NAME_SUPPORTED] = {0};
  	str_uni_filter(filter_name, p_pg_info->name);
  	memset(p_pg_info->name, 0, MAX_SVC_NAME_SUPPORTED);
  	memcpy(p_pg_info->name, filter_name, MAX_SVC_NAME_SUPPORTED);
  }
  #endif
  str_asc2uni(p_pg_info->name, uni_str2);
  if(match_string(uni_str2, uni_str, FALSE))
    p_pg_info->service_type = 0x11;
  
  memset(p_pg_node, 0, sizeof(dvbs_prog_node_t));
  p_pg_node->s_id = p_pg_info->s_id;
  p_pg_node->ts_id = p_pg_info->ts_id;
  p_pg_node->orig_net_id = p_pg_info->orig_net_id;
  p_pg_node->sat_id = g_p_scan_priv->cur_sat_id;
  p_pg_node->tp_id = g_p_scan_priv->cur_tp_id;
  p_pg_node->pcr_pid = p_pg_info->pcr_pid;
  p_pg_node->video_pid = p_pg_info->video_pid;
  p_pg_node->volume = DEFAULT_VOLUME;
  p_pg_node->is_scrambled = p_pg_info->is_scrambled;
//  p_pg_node->volume_compensate = p_pg_info->volume_compensate;
  p_pg_node->ca_system_id = p_pg_info->ca_system_id;
  p_pg_node->service_type = check_service_type(p_pg_info);
  p_pg_node->pmt_pid = p_pg_info->pmt_pid;
  p_pg_node->ecm_num = p_pg_info->ecm_num;
  p_pg_node->video_type = p_pg_info->video_type;
  //p_pg_node->aud_modify_flg = FALSE;
#ifdef LOGIC_NUM_SUPPORT
  //p_pg_node->logical_num = p_pg_info->logical_num;
  p_pg_info->logical_num = find_logic_number_by_sid(p_pg_info->s_id);
  if(p_pg_info->logical_num)
  {
    p_pg_node->logical_num = p_pg_info->logical_num;
  }
  else
  {
    u16 un_india_ts_logic_num = sys_status_get_default_logic_num();
    p_pg_info->logical_num = un_india_ts_logic_num;
    p_pg_node->logical_num = p_pg_info->logical_num;
    un_india_ts_logic_num++;
    sys_status_set_default_logic_num(un_india_ts_logic_num);
  }
#endif
  //p_pg_node->bouquet_id = p_pg_info->bouquet_id;

  //make a name for no name for some pgs
  if(strlen((char *)p_pg_info->name) == 0)
  {
    u8 p_name[MAX_SVC_NAME_SUPPORTED] = {0};
    u8 name_size = 0;
    if(p_pg_info->video_pid != 0)
    {
      sprintf((char *)p_name,"%s%d","TV CH",p_pg_info->s_id);
    }
    else
    {
      sprintf((char *)p_name,"%s%d","RADIO CH",p_pg_info->s_id);
    }
    name_size = strlen((char *)p_name);
    if(name_size > MAX_SVC_NAME_SUPPORTED)
      {
        name_size = MAX_SVC_NAME_SUPPORTED;
      }
    strncpy((char *)p_pg_info->name,(char *)p_name,name_size);
  }
  //Transfer name to unicode
  #ifdef SUPPORT_NIGNORE_PRECODE
  if(impl_string_is_utf8((char *)p_pg_info->name, strlen(p_pg_info->name)))
  {
  	impl_string_utf82uni((char *)p_pg_info->name, p_pg_node->name, strlen(p_pg_info->name));
  }
  else
  {
    ret = dvb_to_unicode(p_pg_info->name, 
  		sizeof(p_pg_info->name), p_pg_node->name, DB_DVBS_MAX_NAME_LENGTH+1);
  }
  #else
  ret = dvb_to_unicode(p_pg_info->name, 
        sizeof(p_pg_info->name), p_pg_node->name, DB_DVBS_MAX_NAME_LENGTH+1);
  #endif

#ifdef AISET_BOUQUET_SUPPORT
  switch(find_show_flag_by_sid(p_pg_info->s_id))
  {
    case 0x01: //the service is always showing on service list
      p_pg_node->hide_flag = 0;
      break;    
    case 0x02://the service is always hiding on service list
      p_pg_node->hide_flag = 1;
      break;    
    case 0x00://if service is not authorized by operator then the service is hided on service list
      p_pg_node->hide_flag = 0;
      p_pg_node->operator_flag = 1;
      break;    
    default:
      break;
  }
#else
  p_pg_node->hide_flag = 0;      
#endif
  p_pg_node->tv_flag = (p_pg_node->video_pid != 0) ? 1 : 0;
  p_pg_node->skp_flag = 0;
  p_pg_node->audio_ch_num = p_pg_info->audio_ch_num;
  if (p_pg_node->service_type != DVB_NVOD_REF_SVC)
  {
    memcpy(p_pg_node->audio, p_pg_info->audio,
      sizeof(audio_t)*p_pg_node->audio_ch_num);
  }

  p_pg_node->mosaic_flag = 0;
  if (p_pg_info->mosaic_flag)
  {
    if (p_pg_node->service_type == DVB_MOSAIC_SVC)
    {
      p_pg_node->mosaic_flag = p_pg_info->mosaic_flag;
      memcpy(&p_pg_node->mosaic, p_pg_info->p_mosaic, sizeof(mosaic_t));
    }

    p_pg_info->mosaic_flag = 0;
  }
  
  if (p_pg_node->ecm_num)
  {
    memcpy(&p_pg_node->cas_ecm, p_pg_info->ca_ecm, sizeof(cas_desc_t) * p_pg_node->ecm_num);
  }
  
  db_ret = db_dvbs_add_program(g_p_scan_priv->pg_view_id, p_pg_node);

  if(DB_DVBS_OK == db_ret)
  {
    evt.id = SCAN_EVT_PG_FOUND;
    evt.data1 = (u32)p_pg_node;
    g_p_scan_priv->pg_depth++;
  }
  else 
  {
    evt.id = SCAN_EVT_NO_MEMORY;
  }
  
  ap_frm_send_evt_to_ui(APP_SCAN, &evt);
  return (DB_DVBS_OK == db_ret);
}

/*!
  Add program information into database
  \param[in]
  */
static BOOL edit_pg_into_db(scan_pg_info_t *p_pg_info, u16 similar_idx)
{
  //Edit program into database
  event_t evt = {SCAN_EVT_PG_FOUND};
  db_dvbs_ret_t db_ret = DB_DVBS_OK;
  dvbs_prog_node_t *p_pg_node = g_p_scan_priv->pg_list + similar_idx;
  u16  pg_name_uni[DB_DVBS_MAX_NAME_LENGTH+1] = {0};
  
  
  //the below is read only for user
  MT_ASSERT(p_pg_node->s_id == p_pg_info->s_id);

  #ifndef SUPPORT_NIGNORE_PRECODE
  {
  	u8 filter_name[MAX_SVC_NAME_SUPPORTED] = {0};
  	str_uni_filter(filter_name, p_pg_info->name);
  	memset(p_pg_info->name, 0, MAX_SVC_NAME_SUPPORTED);
  	memcpy(p_pg_info->name, filter_name, MAX_SVC_NAME_SUPPORTED);
  }
  #endif
  
  //make a name for no name for some pgs
  if(strlen((char *)p_pg_info->name) == 0)
  {
    u8 p_name[MAX_SVC_NAME_SUPPORTED] = {0};
    u8 name_size = 0;
    if(p_pg_info->video_pid != 0)
    {
      sprintf((char *)p_name,"%s%d","TV CH",p_pg_info->s_id);
    }
    else
    {
      sprintf((char *)p_name,"%s%d","RADIO CH",p_pg_info->s_id);
    }
    name_size = strlen((char *)p_name);
    if(name_size > MAX_SVC_NAME_SUPPORTED)
    {
      name_size = MAX_SVC_NAME_SUPPORTED;
    }
    strncpy((char *)p_pg_info->name,(char *)p_name,name_size);
  }
  
  dvb_to_unicode(p_pg_info->name, MAX_SVC_NAME_SUPPORTED,
    pg_name_uni, DB_DVBS_MAX_NAME_LENGTH+1);
  
  if((p_pg_node->hide_flag == 1)
    ||(p_pg_node->video_pid != p_pg_info->video_pid)
    || (p_pg_node->audio_ch_num != p_pg_info->audio_ch_num)
    || (memcmp(p_pg_node->audio, p_pg_info->audio,
      sizeof(audio_t) * p_pg_info->audio_ch_num)) 
    || (p_pg_node->pcr_pid != p_pg_info->pcr_pid) 
    || (p_pg_node->skp_flag != 0)
    || (p_pg_node->is_scrambled != p_pg_info->is_scrambled) 
    || (uni_strcmp(p_pg_node->name, pg_name_uni) != 0)
//    || (p_pg_node->volume_compensate != p_pg_info->volume_compensate) 
#ifdef LOGIC_NUM_SUPPORT
    || (p_pg_node->logical_num != p_pg_info->logical_num && p_pg_info->logical_num != 0)
#endif
    //|| (p_pg_node->bouquet_id != p_pg_info->bouquet_id)
    )
  {
#ifdef AISET_BOUQUET_SUPPORT
    switch(find_show_flag_by_sid(p_pg_info->s_id))
    {
      case 0x01://the service is always showing on service list
        p_pg_node->hide_flag = 0;
        break;    
      case 0x02://the service is always hiding on service list
        p_pg_node->hide_flag = 1;
        break;    
      case 0x00://if service is not authorized by operator then the service is hided on service list
        p_pg_node->hide_flag = 0;
        p_pg_node->operator_flag = 1;
        break;    
      default:
        break;
    }
#else
    p_pg_node->hide_flag = 0;
#endif
    p_pg_node->pcr_pid = p_pg_info->pcr_pid;
    p_pg_node->video_pid = p_pg_info->video_pid;
    uni_strncpy(p_pg_node->name, pg_name_uni, DB_DVBS_MAX_NAME_LENGTH);
    p_pg_node->tv_flag = (p_pg_node->video_pid != 0) ? 1 : 0;
    p_pg_node->skp_flag = 0;
    p_pg_node->audio_ch_num = p_pg_info->audio_ch_num;
    p_pg_node->is_scrambled = p_pg_info->is_scrambled;
//    p_pg_node->volume_compensate = p_pg_info->volume_compensate;
    p_pg_node->ca_system_id = p_pg_info->ca_system_id;
    p_pg_node->service_type = check_service_type(p_pg_info);
    p_pg_node->video_type = p_pg_info->video_type;
#ifdef LOGIC_NUM_SUPPORT
    //p_pg_node->logical_num = p_pg_info->logical_num;
    p_pg_info->logical_num = find_logic_number_by_sid(p_pg_info->s_id);
    if(p_pg_info->logical_num)
    {
      p_pg_node->logical_num = p_pg_info->logical_num;
    }
    else
    {
      u16 un_india_ts_logic_num = sys_status_get_default_logic_num();
      p_pg_info->logical_num = un_india_ts_logic_num;
      p_pg_node->logical_num = p_pg_info->logical_num;
      un_india_ts_logic_num++;
      sys_status_set_default_logic_num(un_india_ts_logic_num);
    }
#endif
    //p_pg_node->bouquet_id = p_pg_info->bouquet_id;
    memcpy(p_pg_node->audio, p_pg_info->audio,
      sizeof(audio_t)*p_pg_node->audio_ch_num);
    db_ret = db_dvbs_edit_program(p_pg_node);
    MT_ASSERT(db_ret == DB_DVBS_OK);
  }
  
  evt.data1 = (u32)p_pg_node;
  ap_frm_send_evt_to_ui(APP_SCAN, &evt);
 
  return TRUE;
}

static BOOL match_pid(scan_pg_info_t *p_pg_info)
{
  u32 audio_cnt = 0; 
      
  if(g_p_input_para->pid_parm.pcr_pid == p_pg_info->pcr_pid
    && g_p_input_para->pid_parm.video_pid == p_pg_info->video_pid)
  {
    for(audio_cnt = 0; audio_cnt < p_pg_info->audio_ch_num; audio_cnt++)
    {
      if(p_pg_info->audio[audio_cnt].p_id == g_p_input_para->pid_parm.audio_pid)
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/*!
  Process tp information when one tp is done
  \param[in] sat_id satellite id
  \param[in] p_tp_list
  */
static RET_CODE impl_process_pg_list(scan_tp_info_i_t *p_tp)
{
  u32 pg_cnt = 0;
  scan_pg_info_t *p_pg = NULL;
  u16 similar_pg_idx = 0;
  BOOL db_done = TRUE;
  u32 loopi = 0;
  //Check if new pg
  for(loopi = 0; loopi < SCAN_MAX_PG_PER_TP; loopi++)
  {
    if (pg_cnt == p_tp->pg_num)
    {
      break;
    }
    p_pg = p_tp->p_pg_list + loopi;
    
    OS_PRINTF("\tFound PG: s_id[%d], v_pid[%d], a_pid_0[%d],"
      " a_pid_1[%d],a_pid_2[%d], trace num[%d], name[%s]\n",
      p_pg->s_id, p_pg->video_pid, p_pg->audio[0].p_id,p_pg->audio[1].p_id,
      p_pg->audio[2].p_id, p_pg->audio_ch_num, p_pg->name);

    if(0 == p_pg->video_pid && 0 == p_pg->audio_ch_num
      && p_pg->service_type != DVB_NVOD_REF_SVC)
    {
      continue; //it's dummy pg
    }
    pg_cnt ++;
    //Check scramble status
    if(g_p_input_para->is_free_only && p_pg->is_scrambled)
    {
      continue;
    }
    if(CHAN_TV == g_p_input_para->chan_type
      && 0 == p_pg->video_pid
      && p_pg->service_type != DVB_NVOD_REF_SVC)
    {
      continue;
    }
    if (p_pg->service_type == DVB_NVOD_TIME_SHIFTED_SVC)
    {
      continue;
    }
    else if(CHAN_RADIO == g_p_input_para->chan_type
      && p_pg->video_pid != 0)
    {
      continue;
    }
    
    //Check pid parameter
    if(g_p_input_para->pid_scan_enable && !match_pid(p_pg))
    {
      continue;
    }

    if(is_new_pg(p_pg, &similar_pg_idx))
    {
      db_done = add_pg_into_db(p_pg);
    }
    else
    {
      db_done = edit_pg_into_db(p_pg, similar_pg_idx);
    }

    if(!db_done)
    {
      break;
    }
  }

  //reset pg depth
  g_p_scan_priv->pg_depth = 0;
  db_dvbs_save(g_p_scan_priv->pg_view_id);
  db_dvbs_save(g_p_scan_priv->tp_view_id);

  return  db_done ? SUCCESS : ERR_NO_MEM;
}

static RET_CODE impl_process_tp(scan_tp_info_i_t *p_tp)
{
  u16 similar_tp_idx = 0;
  event_t evt = {0};
  db_dvbs_ret_t ret = DB_DVBS_OK;
  tp_rcv_para_t *p_tp_para = &p_tp->tp_info;
  u32 iRet = 0;
  cur_tp.freq = p_tp_para->freq;
  cur_tp.sym  = p_tp_para->sym;
  cur_tp.nim_modulate = p_tp_para->nim_modulate;
  cur_tp.nim_type = p_tp->tp_info.nim_type;

  //it's may be the new tp , need add it to 
  {
    if(is_new_tp(&cur_tp, &similar_tp_idx))
    {
      ret = db_dvbs_add_tp(g_p_scan_priv->tp_view_id, &cur_tp);
      //add to list
      
      p_tp->id = cur_tp.id;
    }
    else
    {
      p_tp->id = similar_tp_idx;
      cur_tp.id = similar_tp_idx;

      db_dvbs_edit_tp(&cur_tp);
      db_dvbs_save_tp_edit(&cur_tp);
    }
  }
  memcpy(g_p_scan_priv->tp_list + g_p_scan_priv->tp_depth,
        &cur_tp, sizeof(dvbs_tp_node_t));
    g_p_scan_priv->tp_depth++;
      
  cur_tp.id = p_tp->id;
  g_p_scan_priv->cur_tp_id = p_tp->id;
  //if(DB_DVBS_OK == ret)
  {
    if(p_tp->can_locked)
    {
      iRet = create_pg_array(p_tp->id);
    }
    if(iRet == SUCCESS)
    {
      evt.id = SCAN_EVT_TP_FOUND;
      evt.data1 = (u32)&cur_tp;
      evt.data2 = p_tp->can_locked;
    }
    else
    {
      evt.id = SCAN_EVT_NO_MEMORY;
    }
  }
  //else
  {
    //evt.id = SCAN_EVT_NO_MEMORY;
  }
  ap_frm_send_evt_to_ui(APP_SCAN, &evt);
  return SUCCESS;
}

static RET_CODE impl_on_new_sat(scan_sat_info_t *p_sat_info)
{
  sat_node_t *p_sat = &g_p_scan_priv->cur_sat_info;
  event_t evt = {SCAN_EVT_SAT_SWITCH, (u32)p_sat};

#ifdef SCAN_INFO
  if(g_p_scan_priv->tp_view_id)
  {
    print_scan_info();
  }
  memset(&g_scan_info,0,sizeof(scan_info_t));
#endif

   //for some unlock TP with parse NIT on last sat
  if(g_p_scan_priv->tp_view_id)
    db_dvbs_save(g_p_scan_priv->tp_view_id);

  g_p_scan_priv->cur_sat_id = p_sat_info->sat_id;
  p_sat->id = p_sat_info->sat_id;
  memcpy(p_sat->name, p_sat_info->name, sizeof(p_sat->name));

  //create TP list of current satellite
  create_tp_array(p_sat_info->sat_id);
  //Notify UI current satellite information
  ap_frm_send_evt_to_ui(APP_SCAN, &evt);
  return SUCCESS;
}

static dvbs_tp_node_t * impl_find_cur_tp(u32 cur_tp_id)
{
  u32 i = 0;
  dvbs_tp_node_t * p_cur = NULL;

  for(i = 0; i < g_p_scan_priv->tp_depth; i ++)
  {
    p_cur= g_p_scan_priv->tp_list + i;
    if(p_cur->id == cur_tp_id)
    {
      return p_cur;
    }
  }
  return NULL;
}

static RET_CODE impl_process_cat(void *p_cat)
{
  dvbs_tp_node_t *p_cur_tp = NULL;
  cat_cas_t *p_cat_info = (cat_cas_t *)p_cat;

  p_cur_tp = impl_find_cur_tp(g_p_scan_priv->cur_tp_id);

  if (NULL == p_cur_tp)
  {
    OS_PRINTF("tp error\n");
    return ERR_NOFEATURE;
  }
  OS_PRINTF("[ap_scan_impl] p_cat_info->emm_cnt",p_cat_info->emm_cnt);
  p_cur_tp->emm_num = p_cat_info->emm_cnt;
  memcpy(p_cur_tp->emm_info, p_cat_info->ca_desc, p_cur_tp->emm_num * sizeof(ca_desc_t));

  db_dvbs_edit_tp(p_cur_tp);
  db_dvbs_save_tp_edit(p_cur_tp);
  return SUCCESS;
}


static RET_CODE impl_on_start(void)
{
  //create TP list of current satellite
//  create_tp_array();
  return SUCCESS;
}

static u32 impl_get_switch_disq_time(scan_sat_info_t *p_sat_info)
{
  if (!p_sat_info->diseqc_info.is_fixed)
  {
    return (20 * 1000);  //20 s
  }
  else
  {
    return (1 * 1000);
  }
}

static RET_CODE impl_check_tp(nc_channel_info_t *p_channel)
{
#if 0
  u8 i = 0;
  dvbs_tp_node_t * p_cur = NULL;

  for(i = 0; i < g_p_scan_priv->tp_depth; i ++)
  {
    p_cur= g_p_scan_priv->tp_list + i;
    //OS_PRINTF("#@#@ freq %d %d\n",p_cur->freq , p_channel->channel_info.frequency);
    if(p_cur->freq == p_channel->channel_info.frequency)
    {
      return ERR_FAILURE;
    }
  }
#endif
  return SUCCESS;
}

static RET_CODE impl_free_resource(void)
{
  //for some unlock TP with parse NIT
  db_dvbs_save(g_p_scan_priv->tp_view_id);

  if(g_attach_block.using_attach_block)
  {
    g_p_input_para = NULL;
    g_p_scan_priv = NULL;
  }
  else
  {
    // workround since mtos_free/alloc's bug
    //mtos_free(g_p_input_para);
    //g_p_input_para = NULL;
    mtos_free(g_p_scan_priv);
    g_p_scan_priv = NULL;
  }
  return SUCCESS;
}

static void * impl_get_attach_buffer(u32 size)
{
  void *p_addr = NULL;

  if(g_attach_block.using_attach_block)
  {
    size = ALIGN4(size);
    OS_PRINTF("scan alloc mem %d\n", size);
    p_addr = (u8*)g_attach_block.p_block_addr;
    g_attach_block.p_block_addr = (u8*)g_attach_block.p_block_addr + size;
    g_attach_block.using_size += size;
    MT_ASSERT(g_attach_block.using_size <= g_attach_block.total_size);
  }
  return p_addr;
}

/*!
  Construct tp scan ploicy
  \param[in] block id
  */
void ap_scan_set_attach_block(BOOL use_attach_block, u32 block_id)
{
  g_attach_block.using_attach_block = use_attach_block;
  if(use_attach_block)
  {
    g_attach_block.block_id = block_id;
    g_attach_block.p_block_addr = (void *)
      mem_mgr_require_block(block_id, SYS_MODULE_SCAN);
    MT_ASSERT(g_attach_block.p_block_addr != NULL);
    MT_ASSERT(((u32)g_attach_block.p_block_addr % 4) == 0);
    g_attach_block.total_size = mem_mgr_get_block_size(block_id);
    mem_mgr_release_block(block_id);
    
    g_attach_block.using_size = 0;
  }
}

scan_input_param_t *ap_scan_param_buffer(void)
{
  u32 size = sizeof(scan_input_param_t);

  if(NULL == g_p_input_para)
  {
    if(g_attach_block.using_attach_block)
    {
      g_p_input_para = impl_get_attach_buffer(size);
    }
    else
    {
      g_p_input_para = mtos_malloc(size);
    }
    MT_ASSERT(g_p_input_para != NULL);
    //memset(g_p_input_para, 0, size);
  }
  return g_p_input_para;
}

static void load_sat_info_for_tp_scan(void)
{
  u16 tp_cnt = 0;
  u16 sat_cnt  = 0;
  u32 sat_id = 0;
  BOOL found_sat = FALSE;
  scan_preset_data_t *p_pst = &g_p_input_para->scan_data;
  sat_node_t db_sat = {0};
  BOOL is_fixed = FALSE;

  //MT_ASSERT(0 == p_pst->total_sat);

  sys_status_get_status(BS_ANTENNA_CONNECT, &is_fixed);

  for(tp_cnt = 0; tp_cnt < p_pst->total_tp; tp_cnt++)
  {
    found_sat = FALSE;
    sat_id = p_pst->tp_list[tp_cnt].sat_id;
    for(sat_cnt = 0; sat_cnt < p_pst->total_sat; sat_cnt++)
    {
      if(sat_id == p_pst->sat_list[sat_cnt].sat_id)
      {
        found_sat = TRUE;
        break;
      }
    }


    if(!found_sat)  //not found ,put it in
    {
      scan_sat_info_t *p_sat = p_pst->sat_list + p_pst->total_sat;
      nc_diseqc_info_t *p_diseqc = &p_sat->diseqc_info;
      sat_rcv_para_t *p_sat_info = &p_sat->sat_info;
      local_set_t local_set;

      db_dvbs_get_sat_by_id((u16)sat_id, &db_sat);

      //load diseqc info
      sys_status_get_local_set(&local_set);
      sys_status_get_status(BS_ANTENNA_CONNECT, &p_diseqc->is_fixed);

      p_diseqc->position_type = db_sat.positioner_type;
      p_diseqc->position = db_sat.position;
      p_diseqc->diseqc_type_1_0 = db_sat.diseqc_type;
      p_diseqc->diseqc_port_1_0 = db_sat.diseqc_port;
      p_diseqc->diseqc_type_1_1 = db_sat.diseqc11_type;
      if (db_sat.diseqc11_type)
      {
        p_diseqc->diseqc_type_1_1 = 2;
      }
      p_diseqc->diseqc_port_1_1 = db_sat.diseqc11_port;
      p_diseqc->diseqc_1_1_mode = 0;
      p_diseqc->is_fixed = (db_sat.motor_type == 0);
      p_diseqc->used_DiSEqC12 = (db_sat.motor_type == 1);
      p_diseqc->d_sat_longitude = sys_status_convert_longitude(db_sat.longitude);
      p_diseqc->d_local_longitude = sys_status_convert_longitude(local_set.longitude);
      p_diseqc->d_local_latitude = sys_status_convert_latitude(local_set.latitude);


      p_sat_info->lnb_high = db_sat.lnb_high;
      p_sat_info->lnb_low = db_sat.lnb_low;
      p_sat_info->lnb_power = db_sat.lnb_power;
      p_sat_info->lnb_type = db_sat.lnb_type;
      p_sat_info->k22 = db_sat.k22;
      p_sat_info->v12 = db_sat.v12;
      p_sat_info->band_freq = db_sat.band_freq;
      p_sat_info->unicable_type = db_sat.unicable_type;
      p_sat_info->user_band = db_sat.user_band;  
      
      p_sat->sat_id = db_sat.id;
      OS_PRINTF("load sat id %d\n", p_sat->sat_id);
      memcpy(p_sat->name, db_sat.name, sizeof(db_sat.name));

      //Update current satellite information
      p_pst->total_sat++;
    }
  }
}


policy_handle_t ap_scan_param_apply(void)
{
  u32 size = sizeof(scan_impl_data_t);
  scan_hook_t hook = {0};

  memset(&hook, 0, sizeof(scan_hook_t));
  hook.get_switch_disq_time = impl_get_switch_disq_time;
  hook.check_tp = impl_check_tp;
  hook.process_tp = impl_process_tp;
  hook.free_resource = impl_free_resource;
  hook.get_attach_buffer = impl_get_attach_buffer;
  hook.on_start = impl_on_start;
  hook.process_pg_list = impl_process_pg_list;
  hook.process_cat = impl_process_cat;
  hook.process_nit_info =NULL;
  hook.on_new_sat = impl_on_new_sat;
  
  MT_ASSERT(NULL == g_p_scan_priv);
  if(g_attach_block.using_attach_block)
  {
    g_p_scan_priv = impl_get_attach_buffer(size);
  }
  else
  {
    g_p_scan_priv = mtos_malloc(size);
  }
  MT_ASSERT(g_p_scan_priv != NULL);
  memset(g_p_scan_priv, 0, size);

  if(BLIND_SCAN == g_p_input_para->scan_mode)
  {
    MT_ASSERT(0);
    return NULL;
  }
  else if(TP_SCAN == g_p_input_para->scan_mode)
  {
    MT_ASSERT(0);
    return NULL;
  }
  else if(DVBC_SCAN == g_p_input_para->scan_mode)
  {
      return construct_dvbc_scan_policy(&hook, &g_p_input_para->scan_data);
  }
  else if(DTMB_SCAN == g_p_input_para->scan_mode)
  {
    load_sat_info_for_tp_scan();
    return construct_dtmb_scan_policy(&hook, &g_p_input_para->scan_data);
  }    
  else
  {
    MT_ASSERT(0);
    return NULL;
  }
}



