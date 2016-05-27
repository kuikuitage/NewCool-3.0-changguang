/******************************************************************************/

/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "string.h"
#include "lib_util.h"
#include "lib_char.h"
#include "lib_unicode.h"
#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "class_factory.h"
#include "mdl.h"
#include "class_factory.h"
#include "mdl.h"

#include "service.h"
#include "drv_dev.h"
#include "nim.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "data_manager.h"
#include "data_base.h"
#include "ss_ctrl.h"
#include "uio.h"
#include "service.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "dvb_protocol.h"
#include "cat.h"
#include "pmt.h"
#include "db_dvbs.h"
#include "db_preset.h"
#include "data_manager.h"
#include "data_base.h"
#include "data_base16v2.h"
#include "mem_manager.h"
#include "iso_639_2.h"
#include "ui_common.h"

#define  DB_DBVS_DBG

#ifdef   DB_DBVS_DBG
#define  DVBS_DBG    OS_PRINTF("BORDEN %s , %d lines :", __FUNCTION__, __LINE__);OS_PRINTF
#else
#define  DVBS_DBG    DUMMY_PRINTF
#endif


#define TABLE_BLOCK_ID    (IW_TABLE_BLOCK_ID)
#define VIEW_BLOCK_ID     (IW_VIEW_BLOCK_ID)
#define PG_NODE_VALID_CACHE_DATA_LEN (MAX_TABLE_CACHE_SIZE)//(12)

#define DATA_CACHE_UNIT_SIZE (40)
#define VIEW_CACHE_UNIT_SIZE (sizeof(item_type_t))

static u8 g_sat_table_id = 0;
static u8 g_tp_table_id = 0;
static u8 g_pg_table_id = 0;
static BOOL g_is_part_az_AZ = 0;

static item_type_t g_all_sat_view_buf[DB_DVBS_MAX_SAT];
static item_type_t g_sel_sat_view_buf[DB_DVBS_MAX_SAT];
static item_type_t g_all_tp_view_buf[DB_DVBS_MAX_TP];
static item_type_t g_sat_tp_view_buf[DB_DVBS_MAX_TP];
static item_type_t g_pg_view_buf[DB_DVBS_MAX_PRO];

static u8 *g_find_view_buf;
static db_filter_proc_t filter_array[DB_DVBS_INVALID_VIEW];
static db_filter_proc_new_t filter_new_array[DB_DVBS_INVALID_VIEW];

static pro_sort_t *p_sort_buf = NULL;
static pro_sort_id_t *p_sort_id_buf = NULL;
static pro_view_id_t g_name_view;
static pro_view_id_t g_free_view;
static pro_view_id_t g_lock_view;
static pro_view_id_t g_hd_view;
static pro_view_id_t g_default_view;

static u32 default_order = 0;
static dvbs_view_t g_vv_id_name_map[MAX_VIRTUAL_VIEW_NUM];

typedef struct
{
  u32 diseqc_type         : 2;
  u32 diseqc_port         : 4;
  u32 diseqc11_type       : 3;
  u32 diseqc11_port       : 4;
  u32 lnb_power           : 2;
  u32 lnb_type            : 3;
  u32 position            : 8;
  u32 k22                 : 1;
  u32 name_length         : 5;

  u32 lnb_high            : 16;
  u32 lnb_low             : 16;

  u32 positioner_type     : 2;
  u32 v12                 : 1;
  u32 toneburst           : 2;
  u32 p_is_unicable       : 1;
  u32 longitude           : 16;
  u32 reserved            : 10;

  u32 user_band           : 4;
  u32 band_freq           : 12;
  u32 unicable_type       : 1;
  u32 motor_type          : 2;
  u32 polarity            : 2;
  u32 tuner_index           :1;
  u32 reserved2           : 10;
  u32 scan_mode;
} sat_data_t;

typedef struct
{
  /*!Each bit of fav_grp_flag represents a single favorite group*/
  u32 fav_grp_flag      :32;

  u32 tv_flag                 : 1;
  u32 lck_flag                : 1;
  u32 skp_flag                : 1;
  /*! audio volume */
  u32 volume                  : 5;
  /*! audio channel option, 0: channel 1 \sa audio_pid1, 1: channel 2
     \sa audio_pid2 */
  u32 audio_channel           : 5;
  /*! scrambled flag 0: not scrambled, 1: scrambled */
  u32 is_scrambled            : 1;
  u32 audio_ch_num            : 5;
  /*! video pid */
  u32 video_pid               : 13;

  /*! pcr pid */
  u32 pcr_pid                 : 13;
  /*! audio track option, 0: stereo, 1: left, 2: right */
  u32 audio_track             : 2;
  u32 hide_flag                : 1;
  u32 service_type            : 8;
  u32 video_type              : 8;
  u32 sat_id                  : 16;
  u32 tp_id                   : 16;
  /*
     ---------------------------------------------------------
     above into data cache
    */
  u32 default_order   : 16;
  u32 orig_net_id     : 16;
  u32 ca_system_id            : 16;
  u32 is_audio_channel_modify  : 1;
  u32 name_length     : 5;
  u32 reserved_1        : 10;

  u32 s_id            : 16;
  u32 ts_id           : 16;

//TKGS_SUPPORT START
  u32 reserved_3        :10;
//used by aiset bouquet protocol
  u32 operator_flag       :1;
  u32 age_limit       :5;
  u32 logical_num     : 16;
//TKGS_SUPPORT END

  u32 pmt_pid         : 16;
  u32 ecm_num         : 6;
  u32 reserved_2      : 10;
  cas_desc_t *p_cas_ecm;  //[MAX_ECM_DESC_NUM];
} pg_data_t;

typedef BOOL (*db_dvbs_sort_func)(u8 *prev_node, u8 *cur_node,
                                  dvbs_sort_node_type_t node_type);

typedef BOOL (*db_dvbs_qsort_func)(pro_sort_t *prev_node, pro_sort_t *cur_node);

static db_dvbs_ret_t translate_return_value(db_ret_t db_value)
{
  if(db_value == DB_FAIL)
  {
    return DB_DVBS_FAILED;
  }
  if(db_value == DB_FULL)
  {
    return DB_DVBS_FULL;
  }
  if(db_value == DB_NOT_FOUND)
  {
    return DB_DVBS_NOT_FOUND;
  }
  return DB_DVBS_OK;
}

static BOOL sat_new_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{

  return FALSE;
}
/*
static BOOL sat_t2_new_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  sat_data_t *sat_data = (sat_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 54);
  return (sat_data->scan_mode == DVBT_SCAN);
}
*/
static BOOL sat_selected_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  return (bit_mask & (1 << (DB_DVBS_MARK_SEL))) == 1;
}


static u32 sat_selected_new_filter(u16 bit_mask, u32 context,
                                   u8 block_id, u16 node_id)
{
  return (bit_mask & (1 << (DB_DVBS_MARK_SEL))) == 1;
}


static BOOL all_radio_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return pg_data->tv_flag == 0;
}

static BOOL all_hide_radio_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 0)&&(pg_data->hide_flag== 0);
}


static u32 all_radio_new_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return pg_data->tv_flag == 0;
}


static u32 all_hide_radio_new_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 0)&&(pg_data->hide_flag== 0);
}


static BOOL all_radio_fta_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 0)
         && (pg_data->is_scrambled == 0)&&(pg_data->hide_flag== 0);
}



static u32 all_radio_fta_new_filter(u16 bit_mask, u32 context,
                                    u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);

  return (pg_data->tv_flag == 0)
         && (pg_data->is_scrambled == 0);
}


static BOOL all_radio_cas_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 0)
         && (pg_data->is_scrambled == 1)&&(pg_data->hide_flag== 0);
}


static u32 all_radio_cas_new_filter(u16 bit_mask, u32 context,
                                    u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);

  return (pg_data->tv_flag == 0)
         && (pg_data->is_scrambled == 1);
}


static BOOL all_radio_hd_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return (pg_data->tv_flag == 0)
         && (pg_data->service_type == 0x11)&&(pg_data->hide_flag== 0);
}


static u32 all_radio_hd_new_filter(u16 bit_mask, u32 context,
                                   u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 0)
         && (pg_data->service_type == 0x11);
}


static BOOL all_tv_filter(u16 bit_mask, u32 context,
                          u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 1);
}

static BOOL all_hide_tv_filter(u16 bit_mask, u32 context,
                          u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 1)&&(pg_data->hide_flag== 0);
}


/*!
	hide
*/
static BOOL all_tv_hide_filter(u16 bit_mask, u32 context,
                          u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 1);
}



static u32 all_tv_new_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);

  return (pg_data->tv_flag == 1);
}

static u32 all_hide_tv_new_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);

  return (pg_data->tv_flag == 1) &&(pg_data->hide_flag == 0);
}



static BOOL all_tv_fta_filter(u16 bit_mask, u32 context,
                              u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 1)
         && (pg_data->is_scrambled == 0)&&(pg_data->hide_flag== 0);
}


static u32 all_tv_fta_new_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);

  return (pg_data->tv_flag == 1)
         && (pg_data->is_scrambled == 0);
}


static BOOL all_tv_cas_filter(u16 bit_mask, u32 context,
                              u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 1)
         && (pg_data->is_scrambled == 1)&&(pg_data->hide_flag== 0);
}


static u32 all_tv_cas_new_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);

  return (pg_data->tv_flag == 1)
         && (pg_data->is_scrambled == 1)&&(pg_data->hide_flag== 0);
}


static BOOL all_tv_hd_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return (pg_data->tv_flag == 1)
         && (pg_data->service_type == 0x11)&&(pg_data->hide_flag== 0);
}


static u32 all_tv_hd_new_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->tv_flag == 1)
         && (pg_data->service_type == 0x11)&&(pg_data->hide_flag== 0);
}


static BOOL fav_radio_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & (1 << context)) != 0)
         && (pg_data->tv_flag == 0);
}

static BOOL fav_hide_radio_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & (1 << context)) != 0)
         && (pg_data->tv_flag == 0)&&(pg_data->hide_flag== 0);
}


static u32 fav_radio_new_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & (1 << i)) != 0)
      && (pg_data->tv_flag == 0))
    {
      ret |= 1 << i;
    }
  }
  return ret;
  //return (((pg_data->fav_grp_flag & (1<<context))!= 0)&&( pg_data->tv_flag == 0));
}

static u32 fav_hide_radio_new_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & (1 << i)) != 0)
      && (pg_data->tv_flag == 0)&&(pg_data->hide_flag== 0))
    {
      ret |= 1 << i;
    }
  }
  return ret;
  //return (((pg_data->fav_grp_flag & (1<<context))!= 0)&&( pg_data->tv_flag == 0));
}


static BOOL fav_radio_fta_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & (1 << context)) != 0)
         && (pg_data->tv_flag == 0)
         && (pg_data->is_scrambled == 0)&&(pg_data->hide_flag== 0);
}


static u32 fav_radio_fta_new_filter(u16 bit_mask, u32 context,
                                    u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & (1 << i)) != 0)
      && (pg_data->tv_flag == 0)
      && (pg_data->is_scrambled == 0)
      &&(pg_data->hide_flag== 0))
    {
      ret |= 1 << i;
    }
  }
  return ret;
}


static BOOL fav_radio_cas_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & (1 << context)) != 0)
         && (pg_data->tv_flag == 0)
         && (pg_data->is_scrambled == 1)&&(pg_data->hide_flag== 0);
}


static u32 fav_radio_cas_new_filter(u16 bit_mask, u32 context,
                                    u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & (1 << i)) != 0)
      && (pg_data->tv_flag == 0)
      && (pg_data->is_scrambled == 1)
      &&(pg_data->hide_flag== 0))
    {
      ret |= 1 << i;
    }
  }
  return ret;
}


static BOOL fav_radio_hd_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & (1 << context)) != 0)
         && (pg_data->tv_flag == 0)
         && (pg_data->service_type == 0x11)&&(pg_data->hide_flag== 0);
}


static u32 fav_radio_hd_new_filter(u16 bit_mask, u32 context,
                                   u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & (1 << i)) != 0)
      && (pg_data->tv_flag == 0)
      && (pg_data->service_type == 0x11)
      &&(pg_data->hide_flag== 0))
    {
      ret |= 1 << i;
    }
  }
  return ret;
}


static BOOL fav_tv_filter(u16 bit_mask, u32 context,
                          u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & (1 << context)) != 0)
         && (pg_data->tv_flag == 1) ;
}

static BOOL fav_hide_tv_filter(u16 bit_mask, u32 context,
                          u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & (1 << context)) != 0)
         && (pg_data->tv_flag == 1)&&(pg_data->hide_flag== 0) ;
}


static u32 fav_tv_new_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & (1 << i)) != 0)
      && (pg_data->tv_flag == 1))
    {
      ret |= 1 << i;
    }
  }
  return ret;
}

static u32 fav_hide_tv_new_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & (1 << i)) != 0)
      && (pg_data->tv_flag == 1)&&(pg_data->hide_flag== 0))
    {
      ret |= 1 << i;
    }
  }
  return ret;
}



static BOOL fav_tv_fta_filter(u16 bit_mask, u32 context,
                              u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & (1 << context)) != 0)
         && (pg_data->tv_flag == 1)
         && (pg_data->is_scrambled == 0)&&(pg_data->hide_flag== 0);
}


static u32 fav_tv_fta_new_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & (1 << i)) != 0)
      && (pg_data->tv_flag == 1)
      && (pg_data->is_scrambled == 0)
      &&(pg_data->hide_flag== 0))
    {
      ret |= 1 << i;
    }
  }
  return ret;
}


static BOOL fav_tv_cas_filter(u16 bit_mask, u32 context,
                              u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & (1 << context)) != 0)
         && (pg_data->tv_flag == 1)
         && (pg_data->is_scrambled == 1)&&(pg_data->hide_flag== 0);
}


static u32 fav_tv_cas_new_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & (1 << i)) != 0)
      && (pg_data->tv_flag == 1)
      && (pg_data->is_scrambled == 1)
      &&(pg_data->hide_flag== 0))
    {
      ret |= 1 << i;
    }
  }
  return ret;
}


static BOOL fav_tv_hd_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return ((pg_data->fav_grp_flag & (1 << context)) != 0)
         && (pg_data->tv_flag == 1)
         && (pg_data->service_type == 0x11)&&(pg_data->hide_flag== 0);
}


static u32 fav_tv_hd_new_filter(u16 bit_mask, u32 context,
                                u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if(((pg_data->fav_grp_flag & (1 << i)) != 0)
      && (pg_data->tv_flag == 1)
      && (pg_data->service_type == 0x11)
      &&(pg_data->hide_flag== 0))
    {
      ret |= 1 << i;
    }
  }
  return ret;
}

static BOOL fav_all_tv_filter(u16 bit_mask, u32 context,
                           u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->fav_grp_flag) && (pg_data->tv_flag == 1);
}

static u32 fav_all_tv_new_filter(u16 bit_mask, u32 context,
                              u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->fav_grp_flag) && (pg_data->tv_flag == 1);
}

static BOOL fav_all_filter(u16 bit_mask, u32 context,
                           u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  return (pg_data->fav_grp_flag & (1 << context)) != 0;
}


static u32 fav_all_new_filter(u16 bit_mask, u32 context,
                              u8 block_id, u16 node_id)
{
  u32 i = 0;
  u32 ret = 0;
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_TABLE_CACHE_SIZE);
  for(i = 0; i < context; i++)
  {
    if((pg_data->fav_grp_flag & (1 << i)) != 0)
    {
      ret |= 1 << i;
    }
  }
  return ret;
  //return((pg_data->fav_grp_flag & (1<<context)) != 0);
}


static BOOL sat_pg_filter(u16 bit_mask, u32 context,
                          u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *p = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return p->sat_id == context;
}


static u32 sat_pg_new_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return pg_data->sat_id == context;
}


static BOOL sat_all_tv_filter(u16 bit_mask, u32 context,
                              u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *p = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return (p->sat_id == context)
         && (p->tv_flag == 1);
}

static BOOL sat_all_hide_tv_filter(u16 bit_mask, u32 context,
                              u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *p = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return (p->sat_id == context)
         && (p->tv_flag == 1)&&(p->hide_flag== 0);
}


static BOOL sat_all_tv_fta_filter(u16 bit_mask, u32 context,
                                  u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *p = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return (p->sat_id == context)
         && (p->tv_flag == 1) && (p->hide_flag == 0)
         && (p->is_scrambled == 0);
}


static BOOL sat_all_tv_cas_filter(u16 bit_mask, u32 context,
                                  u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *p = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return (p->sat_id == context)
         && (p->tv_flag == 1)
         && (p->is_scrambled == 1)&&(p->hide_flag== 0);
}


static BOOL sat_all_tv_hd_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *p = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return (p->sat_id == context)
         && (p->tv_flag == 1)
         &&(p->hide_flag== 0)
         && (p->service_type == 0x11);
}


static u32 sat_all_tv_new_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return (pg_data->sat_id == context)
         && (pg_data->tv_flag == 1) ;
}

static u32 sat_all_hide_tv_new_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return (pg_data->sat_id == context)
         && (pg_data->tv_flag == 1) &&(pg_data->hide_flag== 0);
}


static u32 sat_all_tv_fta_new_filter(u16 bit_mask, u32 context,
                                     u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return (pg_data->sat_id == context)
         && (pg_data->is_scrambled == 0)&&(pg_data->hide_flag== 0);
}


static u32 sat_all_tv_cas_new_filter(u16 bit_mask, u32 context,
                                     u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return (pg_data->sat_id == context)
         && (pg_data->is_scrambled == 1)&&(pg_data->hide_flag== 0);
}


static u32 sat_all_tv_hd_new_filter(u16 bit_mask, u32 context,
                                    u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return (pg_data->sat_id == context)
         && (pg_data->service_type == 0x11)&&(pg_data->hide_flag== 0);
}


static BOOL sat_all_radio_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *p = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return ((p->sat_id == context)
         && (p->tv_flag == 0)) == 1;
}

static BOOL sat_all_hide_radio_filter(u16 bit_mask, u32 context,
                                 u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *p = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return ((p->sat_id == context)
         && ((p->tv_flag == 0)) == 1)&&(p->hide_flag== 0);
}


static u32 sat_all_radio_new_filter(u16 bit_mask, u32 context,
                                    u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return ((pg_data->sat_id == context)
         && (pg_data->tv_flag == 0)) == 1;
}

static u32 sat_all_hide_radio_new_filter(u16 bit_mask, u32 context,
                                    u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return ((pg_data->sat_id == context)
         && ((pg_data->tv_flag == 0)) == 1)&& (pg_data->hide_flag== 0);
}

static BOOL sat_all_radio_fta_filter(u16 bit_mask, u32 context,
                                     u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *p = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return ((p->sat_id == context)
         && (p->tv_flag == 0)
         &&(p->hide_flag== 0)
         && (p->is_scrambled == 0)) == 1;
}


static u32 sat_all_radio_fta_new_filter(u16 bit_mask, u32 context,
                                        u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return ((pg_data->sat_id == context)
         && (pg_data->tv_flag == 0)
         &&(pg_data->hide_flag== 0)
         && (pg_data->is_scrambled == 0)) == 1;
}


static BOOL sat_all_radio_cas_filter(u16 bit_mask, u32 context,
                                     u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *p = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return ((p->sat_id == context)
         && (p->tv_flag == 0)
         &&(p->hide_flag== 0)
         && (p->is_scrambled == 1)) == 1;
}


static u32 sat_all_radio_cas_new_filter(u16 bit_mask, u32 context,
                                        u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return ((pg_data->sat_id == context)
         && (pg_data->tv_flag == 0)
         &&(pg_data->hide_flag== 0)
         && (pg_data->is_scrambled == 1)) == 1;
}


static BOOL sat_all_radio_hd_filter(u16 bit_mask, u32 context,
                                    u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *p = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return ((p->sat_id == context)
         && (p->tv_flag == 0)
         &&(p->hide_flag== 0)
         && (p->service_type == 0x11)) == 1;
}


static u32 sat_all_radio_hd_new_filter(u16 bit_mask, u32 context,
                                       u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return ((pg_data->sat_id == context)
         && (pg_data->tv_flag == 0)
         &&(pg_data->hide_flag== 0)
         && (pg_data->service_type == 0x11)) == 1;
}


static BOOL tp_pg_filter(u16 bit_mask, u32 context,
                         u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *p = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return p->tp_id == context;
}


static u32 tp_pg_new_filter(u16 bit_mask, u32 context,
                            u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(pg_data_t));
  return pg_data->tp_id == context;
}


static BOOL sat_tp_filter(u16 bit_mask, u32 context,
                          u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  dvbs_tp_node_t *p = (dvbs_tp_node_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(dvbs_tp_node_t));
  return p->sat_id == context;
}


static u32 sat_tp_new_filter(u16 bit_mask, u32 context,
                             u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 sizeof(dvbs_tp_node_t));
  return pg_data->sat_id == context;
}

static BOOL match_str_v2(const u16 *s,
                  const u16 *m,
                  BOOL enable)
{
  s16 i = 0;
  s16 j = 0;
  s16 nMoff = 0;
  s16 nSoff = 0;
  BOOL bIsM = TRUE;
  u16 szS[32 + 2] = {0};
  u16 szM[32 + 2] = {0};
  u16 sclen = 0;
  
  if(s == NULL
    || m == NULL)
  {
    return FALSE;
  }

  if(m[0] == 0)
  {
    if(s[0] == 0)
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }


  if(enable == TRUE)
  {
    uni_strcpy(szS, s);
  }
  else
  {
    //Change source string from lower case to upper case
    i = 0;
    while(s[i])
    {
      if(s[i] >= 'A' && s[i] <= 'Z')
      {
        szS[i] = s[i] - 'A' + 'a';
      }
      else
      {
        szS[i] = s[i];
      }
      i++;
    }
    szS[i] = 0;
  }

  i = j = 0;
  while(m[i])
  {
    szM[j++] = (!enable) ?
                     //Lowercase lpszMatch[i] to szMatcher[j]
                     ((m[i] >= 'A' && m[i] <= 'Z') ?
                      m[i] - 'A' + 'a' :
                      m[i]) : m[i];
    //Merge '?'
    if(m[i] == '?')
    {
      while(m[++i] == '?')
      {
        ;
      }
    }
    else
    {
      i++;
    }
  }
  szM[j] = 0; //Add 0 to the end of a string

  //special handler for match string didn't start with '?'
  i = 0;
  sclen = 0;

  if(szM[0] != '?')
  {
    while(1)
    {
      if((szM[i] != 0) && (szM[i] != '?'))
      {
        sclen++;
      }
      else
      {
        break;
      }
      
      i++;
    }
    if(uni_strncmp(szM, szS, sclen) != 0)
    {
      return FALSE;
    }
  }

  nMoff = nSoff = sclen;
  while(1)
  {
    if(szM[nMoff] == 0)
    {
      bIsM = TRUE;
      break;
    }

    if(szS[nSoff] == 0)
    {
      bIsM = FALSE;
      break;
    }
  
    if(szM[nMoff] == '?')
    {
      //just skip one charater to match '?'
      nMoff++;
      nSoff++;
    }
    else
    {
      sclen = 0;
      i = nMoff;
      
      while(1)
      {
        if((szM[i] != 0) && (szM[i] != '?'))
        {
          sclen++;
        }
        else
        {
          break;
        }
        
        i++;
      }
      
      while((szM[nMoff] != 0) &&
        (szM[nMoff] != '?') &&
        (szS[nSoff] != 0))
      {
        if(uni_strncmp(szM + nMoff, szS + nSoff, sclen) == 0)
        {
          nMoff += sclen;
          nSoff += sclen;
          
          break;
        }

        nSoff++;
      }
    }
  }

  return bIsM;
}

static BOOL string_find_filter(u16 bit_mask, u32 context,
                               u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *p_pg_node = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  u8 *p_pg_name = NULL;
  const u16 *string_to_be_matched = NULL;

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0,
                 MAX_NODE_LEN);

  p_pg_name = (u8 *)p_pg_node + sizeof(pg_data_t)
              + p_pg_node->audio_ch_num * sizeof(audio_t);

  string_to_be_matched = (u16 *)context;

  if(db_dvbs_get_no_part_az())
  {
    if(p_pg_name[0] >= 'a' && p_pg_name[0] <= 'z')
    {
      p_pg_name[0] = p_pg_name[0] - 'a' + 'A';
    }
  }

  MT_ASSERT(context != 0);
  //Case is not sensitive
  return match_str_v2((u16 *)p_pg_name, string_to_be_matched, TRUE) == TRUE;
}


/*!
   \param[in] prev_node: previous node to be sorted
   \param[in] cur_node:  current node to be sorted
   \param[in] node type: node type of input ( No meaning in this api)
   return : TRUE >exchange pre_node with current node
           FALSE >continue
  */
static BOOL fta_first_sort(u8 *prev_node, u8 *cur_node, dvbs_sort_node_type_t node_type)
{
  BOOL ret = FALSE;

  switch(node_type)
  {
    case DB_DVBS_PG_NODE:
    {
      dvbs_prog_node_t *p_pre_pg_node = (dvbs_prog_node_t *)prev_node;
      dvbs_prog_node_t *p_cur_pg_node = (dvbs_prog_node_t *)cur_node;

      /*!Fix me about is scrambled and found scrambled*/
      ret = (p_pre_pg_node->is_scrambled == FALSE
            && p_cur_pg_node->is_scrambled == TRUE);
    }
    break;
    case DB_DVBS_SAT_NODE:
      MT_ASSERT(0);
      break;
    case DB_DVBS_TP_NODE:
    {
      dvbs_tp_node_t *p_pre_tp_node = (dvbs_tp_node_t *)prev_node;
      dvbs_tp_node_t *p_cur_tp_node = (dvbs_tp_node_t *)cur_node;

      /*!Fix me about is scrambled and found scrambled*/
      ret = (p_pre_tp_node->is_scramble == FALSE
            && p_cur_tp_node->is_scramble == TRUE);
    }
    break;
  }

  return ret;
}


static BOOL name_a_z_sort(u8 *prev_node, u8 *cur_node, dvbs_sort_node_type_t node_type)
{
  BOOL ret = FALSE;

  switch(node_type)
  {
    case DB_DVBS_PG_NODE:
    {
      dvbs_prog_node_t *p_cur_pg_node = (dvbs_prog_node_t *)cur_node;
      dvbs_prog_node_t *p_pre_pg_node = (dvbs_prog_node_t *)prev_node;
      if(p_cur_pg_node->name[0] == 0
        || p_pre_pg_node->name[0] == 0)
      {
        ret = TRUE;
      }
      else
      {
        // ret = uni_strcmp(p_cur_pg_node->name, p_pre_pg_node->name) > 0 ;
        ret = uni_strcmp(p_pre_pg_node->name, p_cur_pg_node->name) > 0;
      }
    }
    break;
    case  DB_DVBS_SAT_NODE:
    {
      sat_node_t *p_cur_sat_node = (sat_node_t *)cur_node;
      sat_node_t *p_pre_sat_node = (sat_node_t *)prev_node;
      if(p_cur_sat_node->name[0] == 0
        || p_pre_sat_node->name[0] == 0)
      {
        ret = TRUE;
      }
      else
      {
        ret = uni_strcmp(p_pre_sat_node->name, p_cur_sat_node->name) > 0;
      }
    }
    break;
    case  DB_DVBS_TP_NODE:
      MT_ASSERT(0); //No tp name existing
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  return ret;
}


static BOOL name_z_a_sort(u8 *prev_node, u8 *cur_node,
                          dvbs_sort_node_type_t node_type)
{
  BOOL ret = FALSE;

  switch(node_type)
  {
    case DB_DVBS_PG_NODE:
    {
      dvbs_prog_node_t *p_cur_pg_node = (dvbs_prog_node_t *)cur_node;
      dvbs_prog_node_t *p_pre_pg_node = (dvbs_prog_node_t *)prev_node;
      ret = uni_strcmp(p_pre_pg_node->name, p_cur_pg_node->name) < 0;
    }
    break;
    case  DB_DVBS_SAT_NODE:
    {
      sat_node_t *p_cur_sat_node = (sat_node_t *)cur_node;
      sat_node_t *p_pre_sat_node = (sat_node_t *)prev_node;
      ret = uni_strcmp(p_pre_sat_node->name, p_cur_sat_node->name) < 0;
    }
    break;
    case  DB_DVBS_TP_NODE:
      MT_ASSERT(0); //No tp name existing
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  return ret;
}


static BOOL lock_first_sort(u8 *prev_node, u8 *cur_node,
                            dvbs_sort_node_type_t node_type)
{
  dvbs_prog_node_t *p_cur_pg_node = (dvbs_prog_node_t *)cur_node;
  dvbs_prog_node_t *p_prev_pg_node = (dvbs_prog_node_t *)prev_node;

  return p_cur_pg_node->lck_flag == 1
         && p_prev_pg_node->lck_flag == 0;
}

static BOOL default_order_sort(u8 *prev_node, u8 *cur_node,
                               dvbs_sort_node_type_t node_type)
{
  dvbs_prog_node_t *p_cur_pg_node = (dvbs_prog_node_t *)cur_node;
  dvbs_prog_node_t *p_pre_pg_node = (dvbs_prog_node_t *)prev_node;

  return p_cur_pg_node->default_order > p_pre_pg_node->default_order;
}

static void sp_proc_tv(check_group_info_t *p, u16 node_id)
{
    if(p->tv_id == INVALIDID) //low priority
    {
      p->tv_id = node_id;
      p->tv_pos = 0;
    }
    if(p->orig_tv == node_id)  //found the orig id // first priority
    {
      p->tv_found = TRUE;
      p->tv_pos = p->tv_cur;
      p->tv_id = node_id;
    }
    if((p->orig_tv_pos == p->tv_cur) && (!p->tv_found))//found pos // sec priority
    {
      p->tv_found = TRUE;
      p->tv_pos = p->tv_cur;
      p->tv_id = node_id;
    }
    p->tv_cur++;
}

static void sp_proc_rd(check_group_info_t *p, u16 node_id)
{
    if(p->rd_id == INVALIDID) //low priority
    {
      p->rd_id = node_id;
      p->rd_pos = 0;
    }
    
    if(p->orig_rd == node_id)  //found the orig id // first priority
    {
      p->rd_found = TRUE;
      p->rd_pos = p->rd_cur;
      p->rd_id = node_id;
    }

    if((p->orig_rd_pos == p->rd_cur) && (!p->rd_found))//found pos // sec priority
    {
      p->rd_found = TRUE;
      p->rd_pos = p->rd_cur;
      p->rd_id = node_id;
    }
    p->rd_cur++;
}

static void special_check_sat_group(spec_check_group_t *p_group, u16 node_id, pg_data_t *pg_info)
{
  u16 i = 0, total_sat = 0;
  check_group_info_t *p = NULL;

  for(i = 0; i < DB_DVBS_MAX_SAT; i++)
  {
    p = p_group->sat_group + i;
    //found the sat
    if(p->context == pg_info->sat_id)
    {
      if(pg_info->video_pid != 0) //tv
      {
        sp_proc_tv(p, node_id);
      }
      else  //radio
      {
        sp_proc_rd(p, node_id);
      }
      return;
    }

    if(p_group->sat_group[i].context != INVALIDID)
    {
      total_sat++;
    }
  }

  if(total_sat < DB_DVBS_MAX_SAT)
  {
    p = p_group->sat_group + total_sat;
    p->context = pg_info->sat_id;

    if(pg_info->video_pid != 0) //tv
    {
      sp_proc_tv(p, node_id);
    }
    else  //radio
    {
      sp_proc_rd(p, node_id);
    }
  }
}


static void special_check_all_group(spec_check_group_t *p_group, u16 node_id, pg_data_t *pg_info)
{
  check_group_info_t *p = &p_group->all_group;

  if(pg_info->video_pid != 0) //tv
  {
    sp_proc_tv(p, node_id);
  }
  else  //radio
  {
    sp_proc_rd(p, node_id);
  }
}


static void special_check_fav_group(spec_check_group_t *p_group, u16 node_id, pg_data_t *pg_info)
{
  u16 i = 0;
  check_group_info_t *p = NULL;
  
  for(i = 0; i < MAX_FAV_GRP; i++)
  {
    p = p_group->fav_group + i;
    
    if(pg_info->fav_grp_flag & (1<<p->context))
    {
      if(pg_info->video_pid != 0) //tv
      {
        sp_proc_tv(p, node_id);
      }
      else  //radio
      {
        sp_proc_rd(p, node_id);
      }
    }
  }
}


static BOOL special_check_group_filter(u16 bit_mask, u32 context,
                                       u8 block_id, u16 node_id)
{
  u8 element_buf[MAX_NODE_LEN];
  pg_data_t *pg_data = (pg_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  db_get_element(p_db_handle, block_id, node_id, element_buf, 0, sizeof(pg_data_t));

  special_check_fav_group((spec_check_group_t *)context, node_id, pg_data);
  special_check_sat_group((spec_check_group_t *)context, node_id, pg_data);
  special_check_all_group((spec_check_group_t *)context, node_id, pg_data);

  //FALSE represents need save to buffer.
  return FALSE;
}

static void sp_group_init(check_group_info_t *p)
{
  p->tv_found = FALSE;
  p->rd_found = FALSE;
  p->tv_id = INVALIDID;
  p->rd_id = INVALIDID;
  p->tv_pos = INVALIDPOS;
  p->rd_pos = INVALIDPOS;
  p->tv_cur = 0;
  p->rd_cur = 0;
}

void special_check_group(spec_check_group_t *p_group, u8 *buf_ex)
{
  int i = 0;
  u8 vv_id;
  item_type_t *p_view_buf = g_pg_view_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  //initialize parameter
  for(i = 0; i < DB_DVBS_MAX_SAT; i++)
  {
    sp_group_init(p_group->sat_group + i);
  }

  for(i = 0; i < MAX_FAV_GRP; i++)
  {
    sp_group_init(p_group->fav_group + i);
  }

  sp_group_init(&p_group->all_group);

  if(buf_ex != NULL)
  {
    p_view_buf = (item_type_t *)buf_ex;
  }
  else
  {
    p_view_buf = g_pg_view_buf;
  }

  db_create_virtual_view(p_db_handle, g_pg_table_id, special_check_group_filter, FALSE,
                         (u8 *)p_view_buf, (u32)p_group, &vv_id);
}


u8 db_dvbs_create_view(dvbs_view_t name, u32 context, u8 *buf_ex)
{
  u8 vv_id;
  BOOL bit_only = FALSE;
  u8 table_id = g_pg_table_id;
  item_type_t *p_view_buf = g_pg_view_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  switch(name)
  {
    case DB_DVBS_ALL_RADIO:
    case DB_DVBS_ALL_RADIO_FTA:
    case DB_DVBS_ALL_RADIO_CAS:
    case DB_DVBS_ALL_RADIO_HD:
    case DB_DVBS_ALL_TV:
    case DB_DVBS_ALL_TV_FTA:
    case DB_DVBS_ALL_TV_CAS:
    case DB_DVBS_ALL_TV_HD:
    case DB_DVBS_FAV_RADIO:
    case DB_DVBS_FAV_RADIO_FTA:
    case DB_DVBS_FAV_RADIO_CAS:
    case DB_DVBS_FAV_RADIO_HD:
    case DB_DVBS_FAV_TV:
    case DB_DVBS_FAV_ALL_TV:    
    case DB_DVBS_FAV_TV_FTA:
    case DB_DVBS_FAV_TV_CAS:
    case DB_DVBS_FAV_TV_HD:
    case DB_DVBS_ALL_PG:
    case DB_DVBS_FAV_ALL:
    case DB_DVBS_HIDE_TV: //add hide
    case DB_DVBS_ALL_HIDE_TV:
    case DB_DVBS_ALL_HIDE_RADIO:
    case DB_DVBS_FAV_HIDE_TV:
    case DB_DVBS_FAV_HIDE_RADIO:
      break;
    case DB_DVBS_SAT_ALL_RADIO:
    case DB_DVBS_SAT_ALL_RADIO_FTA:
    case DB_DVBS_SAT_ALL_RADIO_CAS:
    case DB_DVBS_SAT_ALL_RADIO_HD:
    case DB_DVBS_SAT_ALL_TV:
    case  DB_DVBS_SAT_ALL_TV_FTA:
    case DB_DVBS_SAT_ALL_TV_CAS:
    case DB_DVBS_SAT_ALL_TV_HD:
    case DB_DVBS_SAT_PG:
    case DB_DVBS_TP_PG:
    case DB_DVBS_SAT_ALL_HIDE_RADIO:
    case DB_DVBS_SAT_ALL_HIDE_TV:
      bit_only = FALSE;
      break;
    case DB_DVBS_SAT_TP:
      bit_only = FALSE;
      table_id = g_tp_table_id;
      p_view_buf = g_sat_tp_view_buf;
      break;
    case DB_DVBS_ALL_TP:
      table_id = g_tp_table_id;
      p_view_buf = g_all_tp_view_buf;
      break;
    case DB_DVBS_SAT:
    case DB_DVBS_T2_SAT:
    case DB_DVBS_CABLE_SAT:
      table_id = g_sat_table_id;
      p_view_buf = g_all_sat_view_buf;
      break;
    case DB_DVBS_SELECTED_SAT:
      bit_only = TRUE;
      table_id = g_sat_table_id;
      p_view_buf = g_sel_sat_view_buf;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  if(buf_ex != NULL)
  {
    p_view_buf = (item_type_t *)buf_ex;
  }

  db_create_virtual_view(p_db_handle, table_id, filter_array[name], bit_only,
                         (u8 *)p_view_buf, context, &vv_id);
  g_vv_id_name_map[vv_id] = name;

  return vv_id;
}

dvbs_view_t db_dvbs_get_view_name_by_view_id(u8 view_id)
{
	return g_vv_id_name_map[view_id];
}

u8 db_dvbs_create_view_all(dvbs_view_t name, u16 *p_context, u16 fav_count, u8 *buf_ex)
{
  u8 vv_id;
  BOOL bit_only = FALSE;
  u8 table_id = g_pg_table_id;
  item_type_t *p_view_buf = g_pg_view_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  switch(name)
  {
    case DB_DVBS_ALL_RADIO:
    case DB_DVBS_ALL_RADIO_FTA:
    case DB_DVBS_ALL_RADIO_CAS:
    case DB_DVBS_ALL_RADIO_HD:
    case DB_DVBS_ALL_TV:
    case DB_DVBS_ALL_TV_FTA:
    case DB_DVBS_ALL_TV_CAS:
    case DB_DVBS_ALL_TV_HD:
    case DB_DVBS_FAV_RADIO:
    case DB_DVBS_FAV_RADIO_FTA:
    case DB_DVBS_FAV_RADIO_CAS:
    case DB_DVBS_FAV_RADIO_HD:
    case DB_DVBS_FAV_TV:
    case DB_DVBS_FAV_ALL_TV:    
    case DB_DVBS_FAV_TV_FTA:
    case DB_DVBS_FAV_TV_CAS:
    case DB_DVBS_FAV_TV_HD:
    case DB_DVBS_ALL_PG:
    case DB_DVBS_FAV_ALL:
    case DB_DVBS_ALL_HIDE_TV:
    case DB_DVBS_ALL_HIDE_RADIO:
    case DB_DVBS_FAV_HIDE_TV:
    case DB_DVBS_FAV_HIDE_RADIO:
      break;
    case DB_DVBS_SAT_ALL_RADIO:
    case DB_DVBS_SAT_ALL_RADIO_FTA:
    case DB_DVBS_SAT_ALL_RADIO_CAS:
    case DB_DVBS_SAT_ALL_RADIO_HD:
    case DB_DVBS_SAT_ALL_TV:
    case DB_DVBS_SAT_ALL_TV_FTA:
    case DB_DVBS_SAT_ALL_TV_CAS:
    case DB_DVBS_SAT_ALL_TV_HD:
    case DB_DVBS_SAT_PG:
    case DB_DVBS_TP_PG:
    case DB_DVBS_SAT_ALL_HIDE_TV:
    case DB_DVBS_SAT_ALL_HIDE_RADIO:
      bit_only = FALSE;
      break;
    case DB_DVBS_SAT_TP:
      bit_only = FALSE;
      table_id = g_tp_table_id;
      p_view_buf = g_sat_tp_view_buf;
      break;
    case DB_DVBS_ALL_TP:
      table_id = g_tp_table_id;
      p_view_buf = g_all_tp_view_buf;
      break;
    case DB_DVBS_SAT:
      table_id = g_sat_table_id;
      p_view_buf = g_all_sat_view_buf;
      break;
    case DB_DVBS_SELECTED_SAT:
      bit_only = TRUE;
      table_id = g_sat_table_id;
      p_view_buf = g_sel_sat_view_buf;
      break;
    default:
      MT_ASSERT(0);
      break;
  }
  if(buf_ex != NULL)
  {
    p_view_buf = (item_type_t *)buf_ex;
  }

  db_create_virtual_view_all(p_db_handle, table_id, filter_new_array[name], bit_only,
                             (u8 *)p_view_buf, p_context, fav_count, &vv_id);
  g_vv_id_name_map[vv_id] = name;

  return vv_id;
}


void db_dvbs_destroy_view(u8 view_id)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_destroy_virtual_view(p_db_handle, view_id);
}


BOOL db_dvbs_get_view_del_flag(u8 view_id)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  return db_get_view_del_flag(p_db_handle, view_id);
}

#ifdef SMSX_CA
dvbs_view_t db_dvbs_get_view_type(u8 vv_id)
{
  return g_vv_id_name_map[vv_id];
}
#endif

db_dvbs_ret_t db_dvbs_init(void)
{
  static BOOL is_initialized = FALSE;
  u8 vv_cnt = 0;
  void *p_db_handle = NULL;

  if(is_initialized == FALSE)
  {

    //Initialize database 16v2
    db_init_database_16v2();

    //Get database handle
    p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
    MT_ASSERT(p_db_handle != NULL);

    g_sat_table_id = db_create_table(p_db_handle,
                                     TABLE_BLOCK_ID, DB_DVBS_MAX_SAT);

    db_create_view(p_db_handle, VIEW_BLOCK_ID, g_sat_table_id);

    g_tp_table_id = db_create_table(p_db_handle, TABLE_BLOCK_ID,
                                    DB_DVBS_MAX_TP);

    db_create_view(p_db_handle, VIEW_BLOCK_ID, g_tp_table_id);
    g_pg_table_id = db_create_table(p_db_handle, TABLE_BLOCK_ID,
                                    DB_DVBS_MAX_PRO);

    db_create_view(p_db_handle, VIEW_BLOCK_ID, g_pg_table_id);

    //Initaialize find view buffer
    g_find_view_buf = mtos_malloc(MAX_VIEW_BUFFER_SIZE);
    MT_ASSERT(g_find_view_buf != NULL);
    memset(g_find_view_buf, 0, MAX_VIEW_BUFFER_SIZE);

    //Initialize virtual view filter
    filter_array[DB_DVBS_ALL_RADIO] = all_radio_filter;
    filter_array[DB_DVBS_ALL_RADIO_FTA] = all_radio_fta_filter;
    filter_array[DB_DVBS_ALL_RADIO_CAS] = all_radio_cas_filter;
    filter_array[DB_DVBS_ALL_RADIO_HD] = all_radio_hd_filter;
    filter_array[DB_DVBS_ALL_TV] = all_tv_filter;
    filter_array[DB_DVBS_ALL_TV_FTA] = all_tv_fta_filter;
    filter_array[DB_DVBS_ALL_TV_CAS] = all_tv_cas_filter;
    filter_array[DB_DVBS_ALL_TV_HD] = all_tv_hd_filter;
    filter_array[DB_DVBS_FAV_RADIO] = fav_radio_filter;
    filter_array[DB_DVBS_FAV_RADIO_FTA] = fav_radio_fta_filter;
    filter_array[DB_DVBS_FAV_RADIO_CAS] = fav_radio_cas_filter;
    filter_array[DB_DVBS_FAV_RADIO_HD] = fav_radio_hd_filter;
    filter_array[DB_DVBS_FAV_TV] = fav_tv_filter;
    filter_array[DB_DVBS_FAV_ALL_TV] = fav_all_tv_filter;
    filter_array[DB_DVBS_FAV_TV_FTA] = fav_tv_fta_filter;
    filter_array[DB_DVBS_FAV_TV_CAS] = fav_tv_cas_filter;
    filter_array[DB_DVBS_FAV_TV_HD] = fav_tv_hd_filter;
    filter_array[DB_DVBS_FAV_ALL] = fav_all_filter;
    filter_array[DB_DVBS_SAT_TP] = sat_tp_filter;
    filter_array[DB_DVBS_SAT_PG] = sat_pg_filter;
    filter_array[DB_DVBS_TP_PG] = tp_pg_filter;
    filter_array[DB_DVBS_SAT_ALL_TV] = sat_all_tv_filter;
    filter_array[DB_DVBS_SAT_ALL_TV_FTA] = sat_all_tv_fta_filter;
    filter_array[DB_DVBS_SAT_ALL_TV_CAS] = sat_all_tv_cas_filter;
    filter_array[DB_DVBS_SAT_ALL_TV_HD] = sat_all_tv_hd_filter;
    filter_array[DB_DVBS_SAT_ALL_RADIO] = sat_all_radio_filter;
    filter_array[DB_DVBS_SAT_ALL_RADIO_FTA] = sat_all_radio_fta_filter;
    filter_array[DB_DVBS_SAT_ALL_RADIO_CAS] = sat_all_radio_cas_filter;
    filter_array[DB_DVBS_SAT_ALL_RADIO_HD] = sat_all_radio_hd_filter;
    filter_array[DB_DVBS_ALL_PG] = NULL;
    filter_array[DB_DVBS_ALL_TP] = NULL;
    filter_array[DB_DVBS_SAT] = sat_new_filter;
    filter_array[DB_DVBS_SELECTED_SAT] = sat_selected_filter;
    filter_array[DB_DVBS_HIDE_TV] = all_tv_hide_filter;
    filter_array[DB_DVBS_ALL_HIDE_TV] = all_hide_tv_filter;
    filter_array[DB_DVBS_ALL_HIDE_RADIO] = all_hide_radio_filter;
    filter_array[DB_DVBS_FAV_HIDE_TV] = fav_hide_tv_filter;
    filter_array[DB_DVBS_FAV_HIDE_RADIO] = fav_hide_radio_filter;
    filter_array[DB_DVBS_SAT_ALL_HIDE_TV] = sat_all_hide_tv_filter;
    filter_array[DB_DVBS_SAT_ALL_HIDE_RADIO] = sat_all_hide_radio_filter;
    filter_array[DB_DVBS_T2_SAT] = NULL;
    filter_array[DB_DVBS_CABLE_SAT] = NULL;


    filter_new_array[DB_DVBS_ALL_RADIO] = all_radio_new_filter;
    filter_new_array[DB_DVBS_ALL_RADIO_FTA] = all_radio_fta_new_filter;
    filter_new_array[DB_DVBS_ALL_RADIO_CAS] = all_radio_cas_new_filter;
    filter_new_array[DB_DVBS_ALL_RADIO_HD] = all_radio_hd_new_filter;
    filter_new_array[DB_DVBS_ALL_TV] = all_tv_new_filter;
    filter_new_array[DB_DVBS_ALL_TV_FTA] = all_tv_fta_new_filter;
    filter_new_array[DB_DVBS_ALL_TV_CAS] = all_tv_cas_new_filter;
    filter_new_array[DB_DVBS_ALL_TV_HD] = all_tv_hd_new_filter;
    filter_new_array[DB_DVBS_FAV_RADIO] = fav_radio_new_filter;
    filter_new_array[DB_DVBS_FAV_RADIO_FTA] = fav_radio_fta_new_filter;
    filter_new_array[DB_DVBS_FAV_RADIO_CAS] = fav_radio_cas_new_filter;
    filter_new_array[DB_DVBS_FAV_RADIO_HD] = fav_radio_hd_new_filter;
    filter_new_array[DB_DVBS_FAV_TV] = fav_tv_new_filter;
    filter_new_array[DB_DVBS_FAV_ALL_TV] = fav_all_tv_new_filter;
    filter_new_array[DB_DVBS_FAV_TV_FTA] = fav_tv_fta_new_filter;
    filter_new_array[DB_DVBS_FAV_TV_CAS] = fav_tv_cas_new_filter;
    filter_new_array[DB_DVBS_FAV_TV_HD] = fav_tv_hd_new_filter;
    filter_new_array[DB_DVBS_FAV_ALL] = fav_all_new_filter;
    filter_new_array[DB_DVBS_SAT_TP] = sat_tp_new_filter;
    filter_new_array[DB_DVBS_SAT_PG] = sat_pg_new_filter;
    filter_new_array[DB_DVBS_TP_PG] = tp_pg_new_filter;
    filter_new_array[DB_DVBS_SAT_ALL_TV] = sat_all_tv_new_filter;
    filter_new_array[DB_DVBS_SAT_ALL_TV_FTA] = sat_all_tv_fta_new_filter;
    filter_new_array[DB_DVBS_SAT_ALL_TV_CAS] = sat_all_tv_cas_new_filter;
    filter_new_array[DB_DVBS_SAT_ALL_TV_HD] = sat_all_tv_hd_new_filter;
    filter_new_array[DB_DVBS_SAT_ALL_RADIO] = sat_all_radio_new_filter;
    filter_new_array[DB_DVBS_SAT_ALL_RADIO_FTA] = sat_all_radio_fta_new_filter;
    filter_new_array[DB_DVBS_SAT_ALL_RADIO_CAS] = sat_all_radio_cas_new_filter;
    filter_new_array[DB_DVBS_SAT_ALL_RADIO_HD] = sat_all_radio_hd_new_filter;
    filter_new_array[DB_DVBS_ALL_PG] = NULL;
    filter_new_array[DB_DVBS_ALL_TP] = NULL;
    filter_new_array[DB_DVBS_SAT] = NULL;
    filter_new_array[DB_DVBS_SELECTED_SAT] = sat_selected_new_filter;
    filter_new_array[DB_DVBS_ALL_HIDE_TV] = all_hide_tv_new_filter;
    filter_new_array[DB_DVBS_ALL_HIDE_RADIO] = all_hide_radio_new_filter;
    filter_new_array[DB_DVBS_FAV_HIDE_TV] = fav_hide_tv_new_filter;
    filter_new_array[DB_DVBS_FAV_HIDE_RADIO] = fav_hide_radio_new_filter;
    filter_new_array[DB_DVBS_SAT_ALL_HIDE_TV] = sat_all_hide_tv_new_filter;
    filter_new_array[DB_DVBS_SAT_ALL_HIDE_RADIO] = sat_all_hide_radio_new_filter;
    



    //initialize virtual view id and name map
    for(vv_cnt = 0; vv_cnt < MAX_VIRTUAL_VIEW_NUM; vv_cnt++)
    {
      g_vv_id_name_map[vv_cnt] = DB_DVBS_INVALID_VIEW;
    }

    is_initialized = TRUE;
  }
  return DB_DVBS_OK;
}


void db_dvbs_del_tp(u8 view_id, u16 pos)
{
  u8 pg_view_id = 0;
  u16 pg_total = 0;
  u16 i = 0;
  u16 tp_id = 0XFFFF;
  u16 param = DB_DVBS_PARAM_ACTIVE_FLAG;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  tp_id = db_get_element_id_by_pos(p_db_handle, view_id, pos);
  pg_view_id = db_dvbs_create_view(DB_DVBS_TP_PG, tp_id, NULL);
  pg_total = db_dvbs_get_count(pg_view_id);

  for(i = 0; i < pg_total; i++)
  {
    db_dvbs_change_mark_status(pg_view_id, i, DB_DVBS_DEL_FLAG, param);
    //db_dvbs_view_mark(pg_view_id, i, DB_DVBS_MARK_DEL, TRUE);
  }

  //System stores modifications of element to flash
  db_update_element(p_db_handle, pg_view_id);
  //System stores modifications of view to flash
  db_update_view(p_db_handle, pg_view_id);

  db_del_view_item(p_db_handle, view_id, pos);
}


void db_dvbs_del_satellite(u8 view_id, u16 pos)
{
  u8 tp_view_id = 0;
  u16 i = 0;
  u16 tp_total = 0;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  u16 sat_id = 0;
  u16 param = DB_DVBS_PARAM_ACTIVE_FLAG;
  u8 pg_view_id = 0;
  u16 pg_total = 0;

#if 0
  MT_ASSERT(p_db_handle != NULL);
  sat_id = db_get_element_id_by_pos(p_db_handle, view_id, pos);

  tp_view_id = db_dvbs_create_view(DB_DVBS_SAT_TP, sat_id, NULL);
  tp_total = db_dvbs_get_count(tp_view_id);

  for(i = 0; i < tp_total; i++)
  {
    db_dvbs_del_tp(tp_view_id, i);
  }

  //System stores modifications of element to flash
  db_update_element(p_db_handle, tp_view_id);
  //System stores modifications of view to flash
  db_update_view(p_db_handle, tp_view_id);

  db_del_view_item(p_db_handle, view_id, pos);
#else
  MT_ASSERT(p_db_handle != NULL);
  sat_id = db_get_element_id_by_pos(p_db_handle, view_id, pos);

  tp_view_id = db_dvbs_create_view(DB_DVBS_SAT_TP, sat_id, NULL);
  tp_total = db_dvbs_get_count(tp_view_id);

  for(i = 0; i < tp_total; i++)
  {
    db_dvbs_change_mark_status(tp_view_id, i, DB_DVBS_DEL_FLAG, param);
    //db_dvbs_view_mark(pg_view_id, i, DB_DVBS_MARK_DEL, TRUE);
  }

  //System stores modifications of element to flash
  db_update_element(p_db_handle, tp_view_id);
  //System stores modifications of view to flash
  db_update_view(p_db_handle, tp_view_id);


  pg_view_id = db_dvbs_create_view(DB_DVBS_SAT_PG, sat_id, NULL);
  pg_total = db_dvbs_get_count(pg_view_id);

  for(i = 0; i < pg_total; i++)
  {
    db_dvbs_change_mark_status(pg_view_id, i, DB_DVBS_DEL_FLAG, param);
    //db_dvbs_view_mark(pg_view_id, i, DB_DVBS_MARK_DEL, TRUE);
  }

  //System stores modifications of element to flash
  db_update_element(p_db_handle, pg_view_id);
  //System stores modifications of view to flash
  db_update_view(p_db_handle, pg_view_id);

  db_del_view_item(p_db_handle, view_id, pos);
#endif
}


db_dvbs_ret_t db_dvbs_edit_program(dvbs_prog_node_t *p_node)
{
  u16 i, len;
  db_ret_t ret;
  u8 element_buf[MAX_NODE_LEN] = {0};
  pg_data_t *p_pg   = (pg_data_t *)element_buf;
  audio_t *p_audio  = (audio_t *)(p_pg + 1);
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  u16 *p_pg_name = NULL;
    
  MT_ASSERT(p_db_handle != NULL);
  
  OS_PRINTF("edit id %d, name %s\n", p_node->id, p_node->name);
  p_pg->tv_flag = p_node->tv_flag;
  p_pg->lck_flag = p_node->lck_flag;
  p_pg->hide_flag= p_node->hide_flag;
  p_pg->skp_flag = p_node->skp_flag;

  p_pg->default_order = p_node->default_order;
  p_pg->fav_grp_flag = p_node->fav_grp_flag;
  p_pg->audio_channel = p_node->audio_channel;
  p_pg->is_audio_channel_modify = p_node->is_audio_channel_modify;

  p_pg->audio_ch_num = p_node->audio_ch_num;
  p_pg->audio_track = p_node->audio_track;

  p_pg->is_scrambled = p_node->is_scrambled;
  p_pg->ca_system_id = p_node->ca_system_id;
  p_pg->pcr_pid = p_node->pcr_pid;
  p_pg->service_type = p_node->service_type;
  p_pg->sat_id = p_node->sat_id;
  p_pg->orig_net_id = p_node->orig_net_id;
  p_pg->s_id = p_node->s_id;
  p_pg->tp_id = p_node->tp_id;
  p_pg->pmt_pid = p_node->pmt_pid;
  p_pg->ecm_num = p_node->ecm_num;
  if(p_pg->ecm_num > MAX_ECM_DESC_NUM)
  {
    p_pg->ecm_num = MAX_ECM_DESC_NUM;
  }
  p_pg->video_pid = p_node->video_pid;
  p_pg->video_type = p_node->video_type;
//  OS_PRINTF("video type  : %d  %d line %d\n", p_pg->video_type, p_node->video_type, __LINE__);
  p_pg->volume = p_node->volume;
  p_pg->ts_id = p_node->ts_id;

  p_pg->age_limit= p_node->age_limit;
  p_pg->logical_num = p_node->logical_num;
  p_pg->operator_flag = p_node->operator_flag;

  len = (u16)uni_strlen(p_node->name);
  p_pg->name_length = len > DB_DVBS_MAX_NAME_LENGTH
                      ? DB_DVBS_MAX_NAME_LENGTH : len;

  /*!Modify audio type*/
  for(i = 0; i < p_pg->audio_ch_num; i++, p_audio++)
  {
    memcpy(p_audio, p_node->audio + i, sizeof(audio_t));
  }

  p_pg_name = (u16 *)p_audio;
  if(p_pg->name_length > 0)
  {
    uni_strncpy(p_pg_name, p_node->name, p_pg->name_length);
    p_pg_name += p_pg->name_length + 1;
  }

  len = (u16)(sizeof(pg_data_t) + (p_pg->name_length + 1) * sizeof(u16)
              + p_pg->audio_ch_num * sizeof(audio_t));

#if 1 //ecm_num support
  if(p_pg->ecm_num)
  {
    p_pg->p_cas_ecm = (cas_desc_t *)p_pg_name;
    memcpy(p_pg->p_cas_ecm, p_node->cas_ecm, p_pg->ecm_num * sizeof(cas_desc_t));
    len += (u16)p_pg->ecm_num * sizeof(cas_desc_t);
  }
#endif

  ret = db_edit_element(p_db_handle, g_pg_table_id, (u16)p_node->id, element_buf, len);

  return translate_return_value(ret);
}


void db_dvbs_save_pg_edit(dvbs_prog_node_t *p_pg_node)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_save_element_edit(p_db_handle, g_pg_table_id, (u16)p_pg_node->id);
}


void db_dvbs_undo_pg_edit(dvbs_prog_node_t *p_pg_node)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_undo_element_edit(p_db_handle, g_pg_table_id, (u16)p_pg_node->id);
}


db_dvbs_ret_t db_dvbs_edit_satellite(sat_node_t *p_node)
{
  u16 len;
  db_ret_t ret;
  u8 element_buf[MAX_NODE_LEN] = {0};  
  sat_data_t *p_sat = (sat_data_t *)element_buf;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  p_sat->diseqc_port = p_node->diseqc_port;
  p_sat->diseqc_type = p_node->diseqc_type;
  p_sat->diseqc11_port = p_node->diseqc11_port;
  p_sat->diseqc11_type = p_node->diseqc11_type;

  p_sat->k22 = p_node->k22;
  p_sat->v12 = p_node->v12;

  p_sat->position = p_node->position;
  p_sat->positioner_type = p_node->positioner_type;
  p_sat->toneburst = p_node->toneburst;
  p_sat->longitude = p_node->longitude;

  p_sat->lnb_high = p_node->lnb_high;
  p_sat->lnb_low = p_node->lnb_low;
  p_sat->lnb_power = p_node->lnb_power;
  p_sat->tuner_index = p_node->tuner_index;
  //p_sat->polarity = p_node->polarity;
  p_sat->lnb_type = p_node->lnb_type;

  p_sat->user_band = p_node->user_band;
  p_sat->band_freq = p_node->band_freq;
  p_sat->motor_type = p_node->motor_type;
  p_sat->p_is_unicable = p_node->p_is_unicable;
  p_sat->unicable_type = p_node->unicable_type;
  p_sat->scan_mode = p_node->scan_mode;

  len = (u16)uni_strlen(p_node->name);
  p_sat->name_length = len > DB_DVBS_MAX_NAME_LENGTH
                       ? DB_DVBS_MAX_NAME_LENGTH : len;

  if(p_sat->name_length > 0)
  {
    u8 *p_name = element_buf + sizeof(sat_data_t);
    uni_strncpy((u16 *)p_name, p_node->name, p_sat->name_length);
  }

  len = (u16)(sizeof(sat_data_t) + (p_sat->name_length + 1) * sizeof(u16));
  ret = db_edit_element(p_db_handle, g_sat_table_id, (u16)p_node->id, element_buf, len);
  
  return translate_return_value(ret);
}


void db_dvbs_save_sat_edit(sat_node_t *p_sat_node)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_save_element_edit(p_db_handle, g_sat_table_id, (u16)p_sat_node->id);
}


void db_dvbs_undo_sat_edit(sat_node_t *p_sat_node)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_undo_element_edit(p_db_handle, g_sat_table_id, (u16)p_sat_node->id);
}


db_dvbs_ret_t db_dvbs_edit_tp(dvbs_tp_node_t *p_node)
{
  db_ret_t ret = DB_FAIL;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  db_edit_element(p_db_handle, g_tp_table_id, (u16)p_node->id, (u8 *)p_node,
                  sizeof(dvbs_tp_node_t));

  return translate_return_value(ret);
}


void db_dvbs_save_tp_edit(dvbs_tp_node_t *p_tp_node)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  db_save_element_edit(p_db_handle, g_tp_table_id, (u16)p_tp_node->id);
}


void db_dvbs_undo_tp_edit(dvbs_tp_node_t *p_tp_node)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  db_undo_element_edit(p_db_handle, g_tp_table_id, (u16)p_tp_node->id);
}


u16 db_dvbs_get_count(u8 view_id)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  return db_get_virtual_view_count(p_db_handle, view_id);
}


db_dvbs_ret_t db_dvbs_restore_to_factory(u8 blk_id, u8 ext_mem_blk_id)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  void *p_sc_handle = class_get_handle_by_id(SC_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  MT_ASSERT(p_sc_handle != NULL);

  mtos_task_lock();
  //ss_set_status(SS_CHECKOUT, 0xFF); //write it to invalid
  ss_ctrl_clr_checksum(p_sc_handle);
  OS_PRINTF("load default start...\n");
  db_dvbs_init();
  db_clean(p_db_handle);

  #if 0//#ifdef CONFIG_CUSTOMER
  db_dvbs_load_preset(blk_id, PRESET_PG_TYPE_ALL, PRESET_PG_AREA_ALL);
  #endif
  //ss_set_status(SS_CHECKOUT, SS_CHECKOUT_VALUE); //write it back
  ss_ctrl_set_checksum(p_sc_handle);
  OS_PRINTF("load default finish.\n");
  //stk_info_rep();

  mtos_task_unlock();
  return DB_DVBS_OK;
}

db_dvbs_ret_t db_dvbs_get_pg_by_id(u16 id, dvbs_prog_node_t *p_pg)
{
  u16 i = 0;
  audio_t *p_audio = NULL;
  u8 element_buf[MAX_NODE_LEN] = {0};  
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  u16 *p_pg_name = NULL;

  MT_ASSERT(p_db_handle != NULL);

  if(db_get_element(p_db_handle, g_pg_table_id, id, element_buf, 0,
                    sizeof(pg_data_t)
                    + (DB_DVBS_MAX_NAME_LENGTH + 1) * sizeof(u16)
                    + sizeof(audio_t) * DB_DVBS_MAX_AUDIO_CHANNEL))
  {
    pg_data_t *p_node = (pg_data_t *)element_buf;
    p_audio = (audio_t *)(p_node + 1);

    memset(p_pg, 0x00, sizeof(dvbs_prog_node_t));
    p_pg->id = id;
    p_pg->audio_channel = p_node->audio_channel;
    p_pg->is_audio_channel_modify = p_node->is_audio_channel_modify;
    p_pg->audio_track = p_node->audio_track;
    p_pg->is_scrambled = p_node->is_scrambled;
    p_pg->ca_system_id = p_node->ca_system_id;
    p_pg->pcr_pid = p_node->pcr_pid;
    p_pg->service_type = p_node->service_type;
    p_pg->sat_id = p_node->sat_id;
    p_pg->s_id = p_node->s_id;
    p_pg->tp_id = p_node->tp_id;
    p_pg->video_pid = p_node->video_pid;
    p_pg->video_type = p_node->video_type;
    p_pg->volume = p_node->volume;
    p_pg->audio_ch_num = (p_node->audio_ch_num < DB_DVBS_MAX_AUDIO_CHANNEL)
                         ? p_node->audio_ch_num : DB_DVBS_MAX_AUDIO_CHANNEL;
    p_pg->ts_id = p_node->ts_id;
    p_pg->orig_net_id = p_node->orig_net_id;
    p_pg->pmt_pid = p_node->pmt_pid;
    p_pg->ecm_num = p_node->ecm_num;
    if(p_pg->ecm_num > MAX_ECM_DESC_NUM)
    {
      p_pg->ecm_num = MAX_ECM_DESC_NUM;
    }
    p_pg->lck_flag = p_node->lck_flag;
    p_pg->hide_flag= p_node->hide_flag;
    p_pg->skp_flag = p_node->skp_flag;
    p_pg->tv_flag = p_node->tv_flag;


    p_pg->age_limit= p_node->age_limit;
    p_pg->logical_num = p_node->logical_num;
    p_pg->operator_flag = p_node->operator_flag;

    p_pg->fav_grp_flag = p_node->fav_grp_flag;
    p_pg->default_order = p_node->default_order;
    for(i = 0; i < p_pg->audio_ch_num; i++)
    {
      memcpy((p_pg->audio + i), p_audio, sizeof(audio_t));
      p_audio++;
    }
#if 0
    if(p_node->name_length > 0)
    {
      memset(p_pg->name, 0, DB_DVBS_MAX_NAME_LENGTH);
      strncpy((char *)p_pg->name, (char *)p_audio, p_node->name_length);
      if(p_node->name_length >= DB_DVBS_MAX_NAME_LENGTH)
      {
        p_pg->name[p_node->name_length - 1] = 0;
      }
      else
      {
        p_pg->name[p_node->name_length] = 0;
      }
    }
    else
    {
      strcpy((char *)p_pg->name, "noname");
    }
#else
    p_pg_name = (u16 *)p_audio;
    if(p_node->name_length > 0)
    {
      uni_strncpy(p_pg->name, p_pg_name, DB_DVBS_MAX_NAME_LENGTH);
      p_pg_name += p_node->name_length + 1;
    }
    else
    {
      //strcpy((char*)p_pg->name, "noname");
      str_nasc2uni("noname", p_pg->name, DB_DVBS_MAX_NAME_LENGTH);
    }
#endif

#if 1 //ecm_num support
    if(p_pg->ecm_num)
    {
      p_node->p_cas_ecm = (cas_desc_t *)p_pg_name;
      memcpy(p_pg->cas_ecm, p_node->p_cas_ecm, p_pg->ecm_num * sizeof(cas_desc_t));
    }
#endif
    return DB_DVBS_OK;
  }
  return DB_DVBS_FAILED;
}


db_dvbs_ret_t db_dvbs_get_tp_by_id(u16 id, dvbs_tp_node_t *p_tp)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  if(db_get_element(p_db_handle, g_tp_table_id, id, (u8 *)p_tp, 0, sizeof(dvbs_tp_node_t)))
  {
    p_tp->id = id;
    return DB_DVBS_OK;
  }
  return DB_DVBS_FAILED;
}


db_dvbs_ret_t db_dvbs_get_sat_by_id(u16 id, sat_node_t *p_sat)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);  
  u8 element_buf[MAX_NODE_LEN] = {0};  
  u16 sat_data_len = sizeof(sat_data_t) + (DB_DVBS_MAX_NAME_LENGTH + 1) * sizeof(u16);

  MT_ASSERT(p_db_handle != NULL);
  
  if(db_get_element(p_db_handle, g_sat_table_id, id, element_buf, 0, sat_data_len))
  {
    sat_data_t *p_data = (sat_data_t *)element_buf;
    p_sat->id = id;
    p_sat->diseqc_port = p_data->diseqc_port;
    p_sat->diseqc_type = p_data->diseqc_type;
    p_sat->diseqc11_port = p_data->diseqc11_port;
    p_sat->diseqc11_type = p_data->diseqc11_type;

    p_sat->k22 = p_data->k22;
    p_sat->v12 = p_data->v12;

    p_sat->position = p_data->position;
    p_sat->positioner_type = p_data->positioner_type;
    p_sat->toneburst = p_data->toneburst;
    p_sat->longitude = p_data->longitude;

    p_sat->lnb_high = p_data->lnb_high;
    p_sat->lnb_power = p_data->lnb_power;
    p_sat->tuner_index = p_data->tuner_index;
    p_sat->lnb_low = p_data->lnb_low;
    //p_sat->polarity = p_data->polarity;
    p_sat->lnb_type = p_data->lnb_type;

    p_sat->motor_type = p_data->motor_type;
    p_sat->band_freq = p_data->band_freq;
    p_sat->user_band = p_data->user_band;
    p_sat->p_is_unicable = p_data->p_is_unicable;
    p_sat->unicable_type = p_data->unicable_type;
    p_sat->scan_mode = p_data->scan_mode;

#if 0
    if(p_data->name_length > 0)
    {
      memset(p_sat->name, 0, DB_DVBS_MAX_NAME_LENGTH);
      strncpy((char *)p_sat->name,
              (char *)&g_p_data_buf[sizeof(sat_data_t)],
              p_data->name_length);
    }
    else
    {
      strcpy((char *)p_sat->name, "no name");
    }
#else
    if(p_data->name_length > 0)
    {
      uni_strncpy(p_sat->name,
                  (u16 *)&element_buf[sizeof(sat_data_t)],
                  DB_DVBS_MAX_NAME_LENGTH);
    }
    else
    {
      //strcpy((char*)p_sat->name, "no name");
      str_nasc2uni("no name", p_sat->name, DB_DVBS_MAX_NAME_LENGTH);
    }
#endif
    // for motor mode, use uniform setting of sat

    return DB_DVBS_OK;
  }
  return DB_DVBS_FAILED;
}


u16 db_dvbs_get_view_pos_by_id(u8 view_id, u16 id)
{
  u16 i = 0;
  u16 ret_id = 0;
  u16 count = 0;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  count = db_get_virtual_view_count(p_db_handle, view_id);

  for(i = 0; i < count; i++)
  {
    ret_id = db_get_element_id_by_pos(p_db_handle, view_id, i);
    if(ret_id == id)
    {
      return i;
    }
  }
  return INVALIDID;
}


u16 db_dvbs_get_id_by_view_pos(u8 view_id, u16 pos)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  return db_get_element_id_by_pos(p_db_handle, view_id, pos);
}


db_dvbs_ret_t db_dvbs_move_item_in_view(u8 view_id, u16 pos_src, u16 pos_des)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_move_view_item(p_db_handle, view_id, pos_src, pos_des);
  return DB_DVBS_OK;
}


db_dvbs_ret_t db_dvbs_exchange_item_in_view(u8 view_id, u16 pos_src,
                                            u16 pos_des)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_exchange_view_item(p_db_handle, view_id, pos_src, pos_des);
  return DB_DVBS_OK;
}


db_dvbs_ret_t db_dvbs_dump_item_in_view(u8 view_id, void *p_mem, u16 len)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_dump_view_item(p_db_handle, view_id, p_mem, len);
  return DB_DVBS_OK;
}


void db_dvbs_save(u8 view_id)
{
  dvbs_view_t view_name = g_vv_id_name_map[view_id];
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  switch(view_name)
  {
    case DB_DVBS_ALL_RADIO:
    case DB_DVBS_ALL_TV:
    case DB_DVBS_ALL_TV_FTA:
    case DB_DVBS_ALL_TV_CAS:
    case DB_DVBS_ALL_TV_HD:
    case DB_DVBS_FAV_RADIO:
    case DB_DVBS_FAV_TV:
    case DB_DVBS_FAV_ALL_TV:
    case DB_DVBS_FAV_ALL:
    case  DB_DVBS_FAV_TV_FTA:
    case  DB_DVBS_FAV_TV_CAS:
    case  DB_DVBS_FAV_TV_HD:
    case DB_DVBS_ALL_PG:
    case DB_DVBS_HIDE_TV:
    case DB_DVBS_ALL_HIDE_TV:
    case DB_DVBS_ALL_HIDE_RADIO:
    case DB_DVBS_FAV_HIDE_TV:
    case DB_DVBS_FAV_HIDE_RADIO:
    case DB_DVBS_SAT_PG:
    case DB_DVBS_SAT_ALL_RADIO:
    case DB_DVBS_SAT_ALL_TV:
    case DB_DVBS_SAT_ALL_TV_FTA:
    case DB_DVBS_SAT_ALL_TV_CAS:
    case DB_DVBS_SAT_ALL_TV_HD:
    case DB_DVBS_SAT_ALL_HIDE_RADIO:
    case DB_DVBS_SAT_ALL_HIDE_TV:
      break;
    case DB_DVBS_SAT:
    case DB_DVBS_SELECTED_SAT:
    {
      u16 sat_cnt = 0;
      u16 total_sat_cnt = 0;
      total_sat_cnt = db_dvbs_get_count(view_id);

      for(sat_cnt = 0; sat_cnt < total_sat_cnt; sat_cnt++)
      {
        if(db_dvbs_get_mark_status(view_id, sat_cnt, DB_DVBS_DEL_FLAG, 0) == 1)
        {
          db_dvbs_del_satellite(view_id, sat_cnt);
        }
      }
    }
    break;
    case DB_DVBS_ALL_TP:
    case DB_DVBS_TP_PG:
    case DB_DVBS_SAT_TP:
    {
      u16 tp_cnt = 0;
      u16 total_tp_cnt = db_dvbs_get_count(view_id);
      for(tp_cnt = 0; tp_cnt < total_tp_cnt; tp_cnt++)
      {
        if(db_dvbs_get_mark_status(view_id, tp_cnt, DB_DVBS_DEL_FLAG, 0) == 1)
        {
          db_dvbs_del_tp(view_id, tp_cnt);
        }
      }
    }
    break;
    default:
      //MT_ASSERT(0);
      break;
  }

  //System stores modifications of element to flash
  db_update_element(p_db_handle, view_id);
  //System stores modifications of view to flash
  db_update_view(p_db_handle, view_id);
}


void db_dvbs_undo(u8 view_id)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  db_undo_view(p_db_handle, view_id);
  db_undo_element(p_db_handle, view_id);
}


BOOL is_tp_full(void)
{
  u8 view_id = 0;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  view_id = db_dvbs_create_view(DB_DVBS_ALL_TP, 0, NULL);
  if(db_get_real_view_count(p_db_handle, view_id) >= DB_DVBS_MAX_TP)
  {
    return TRUE;
  }
  return FALSE;
}


BOOL is_pg_full(void)
{
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  u8 view_id = 0;

  MT_ASSERT(p_db_handle != NULL);
  view_id = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, NULL);
  if(db_get_real_view_count(p_db_handle, view_id) >= DB_DVBS_MAX_PRO)
  {
    return TRUE;
  }
  return FALSE;
}


db_dvbs_ret_t db_dvbs_add_program(u8 view_id, dvbs_prog_node_t *p_node)
{
  u16 pos = 0;
  u16 i = 0;
  u16 len = 0;
  u16 *p_pg_name = NULL;
  db_ret_t ret = DB_SUC;
  pg_data_t *p_pg = NULL;
  audio_t *p_audio = NULL;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);
  u8 element_buf[MAX_NODE_LEN] = {0};  

  MT_ASSERT(p_db_handle != NULL);

  p_pg = (pg_data_t *)element_buf;
  p_audio = (audio_t *)(p_pg + 1);
  
  default_order++;
  p_pg->fav_grp_flag = p_node->fav_grp_flag;
  p_pg->audio_channel = p_node->audio_channel;
  p_pg->is_audio_channel_modify = p_node->is_audio_channel_modify;
  p_pg->audio_ch_num = p_node->audio_ch_num;
  p_pg->audio_track = p_node->audio_track;
  p_pg->is_scrambled = p_node->is_scrambled;
  p_pg->ca_system_id = p_node->ca_system_id;
  p_pg->pcr_pid = p_node->pcr_pid;
  p_pg->service_type = p_node->service_type;
  p_pg->sat_id = p_node->sat_id;
  p_pg->s_id = p_node->s_id;
  p_pg->tp_id = p_node->tp_id;
  p_pg->video_pid = p_node->video_pid;
  p_pg->video_type = p_node->video_type;
  OS_PRINTF("video type2 : %d  %d\n", p_pg->video_type, p_node->video_type);
  p_pg->volume = p_node->volume;
  p_pg->ts_id = p_node->ts_id;
  p_pg->orig_net_id = p_node->orig_net_id;
  p_pg->pmt_pid = p_node->pmt_pid;
  p_pg->ecm_num = p_node->ecm_num;
  if(p_pg->ecm_num > MAX_ECM_DESC_NUM)
  {
    p_pg->ecm_num = MAX_ECM_DESC_NUM;
  }

//  p_pg->default_order = default_order;
  p_pg->default_order = p_node->s_id;
  len = (u16)uni_strlen(p_node->name);
  p_pg->name_length = len > DB_DVBS_MAX_NAME_LENGTH
                      ? DB_DVBS_MAX_NAME_LENGTH : len;

  p_pg->tv_flag = p_node->tv_flag;
  p_pg->lck_flag = p_node->lck_flag;
  p_pg->hide_flag= p_node->hide_flag;
  p_pg->skp_flag = p_node->skp_flag;
  p_pg->age_limit= p_node->age_limit;
  p_pg->logical_num = p_node->logical_num;
  p_pg->operator_flag = p_node->operator_flag;

  if(p_pg->audio_ch_num > DB_DVBS_MAX_AUDIO_CHANNEL)
  {
    p_pg->audio_ch_num = DB_DVBS_MAX_AUDIO_CHANNEL;
  }
  for(i = 0; i < p_pg->audio_ch_num; i++)
  {
    memcpy(p_audio, (p_node->audio + i), sizeof(audio_t));
    //*p_audio++ = p_node->audio_pid[i];
    p_audio++;
  }

  p_pg_name = (u16 *)p_audio;
  if(p_pg->name_length > 0)
  {
    uni_strncpy((u16 *)p_pg_name, p_node->name, p_pg->name_length);
  }
  else
  {
    u8 p_no_name[LEN_OF_NO_NAME] =
    {
      'n', 'o', ' ', 'n', 'a', 'm', 'e', 0
    };
    //Add the unicode of noname

    str_nasc2uni(p_no_name, p_pg_name, LEN_OF_NO_NAME);

    p_pg->name_length = uni_strlen(p_pg_name);
  }

  len = (u16)(sizeof(pg_data_t) + (p_pg->name_length + 1) * sizeof(u16)
              + p_pg->audio_ch_num * sizeof(audio_t));

#if 1 //ecm_num support
  if(p_pg->ecm_num)
  {
    p_pg->p_cas_ecm = (cas_desc_t *)(p_pg_name + p_pg->name_length + 1);
    memcpy(p_pg->p_cas_ecm, p_node->cas_ecm, p_pg->ecm_num * sizeof(cas_desc_t));
    len += (u16)p_pg->ecm_num * sizeof(cas_desc_t);
  }
#endif

  OS_PRINTF("@@@view_id: %d, len: %d\n", view_id, len);
  ret = db_add_view_item(p_db_handle, view_id, (u8 *)p_pg, len, &pos);
  OS_PRINTF("@@@ret: %d, pos: %d\n", ret, pos);

  p_node->id = db_get_element_id_by_pos(p_db_handle, view_id, pos);
  OS_PRINTF("@@@p_node->id: %d\n", p_node->id);
  return translate_return_value(ret);
}


db_dvbs_ret_t db_dvbs_add_satellite(u8 view_id, sat_node_t *p_node)
{
  u16 pos;
  u16 len;
  db_ret_t ret;
  sat_data_t *p_sat = NULL;  
  u8 element_buf[MAX_NODE_LEN] = {0};     
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);
  
  p_sat = (sat_data_t *)element_buf;

  p_sat->diseqc_port = p_node->diseqc_port;
  p_sat->diseqc_type = p_node->diseqc_type;
  p_sat->diseqc11_port = p_node->diseqc11_port;
  p_sat->diseqc11_type = p_node->diseqc11_type;

  p_sat->k22 = p_node->k22;
  p_sat->v12 = p_node->v12;

  p_sat->position = p_node->position;
  p_sat->positioner_type = p_node->positioner_type;
  p_sat->toneburst = p_node->toneburst;
  p_sat->longitude = p_node->longitude;

  p_sat->lnb_high = p_node->lnb_high;
  p_sat->lnb_low = p_node->lnb_low;
  p_sat->lnb_power = p_node->lnb_power;
  p_sat->tuner_index = p_node->tuner_index;
  //p_sat->polarity = p_node->polarity;
  p_sat->lnb_type = p_node->lnb_type;
  p_sat->user_band = p_node->user_band;
  p_sat->band_freq = p_node->band_freq;
  p_sat->scan_mode = p_node->scan_mode;

  len = (u16)uni_strlen(p_node->name);
  p_sat->name_length = len > DB_DVBS_MAX_NAME_LENGTH
                       ? DB_DVBS_MAX_NAME_LENGTH : len;
  if(p_sat->name_length > 0)
  {
    u8 *p_name = element_buf + sizeof(sat_data_t);
    uni_strncpy((u16 *)p_name, p_node->name, p_sat->name_length);
  }

  len = (u16)(sizeof(sat_data_t) + (p_sat->name_length + 1) * sizeof(u16));
  ret = db_add_view_item(p_db_handle, view_id, (u8 *)p_sat, len, &pos);
  p_node->id = db_get_element_id_by_pos(p_db_handle, view_id, pos);

  return translate_return_value(ret);
}


db_dvbs_ret_t db_dvbs_add_tp(u8 view_id, dvbs_tp_node_t *p_node)
{
  db_ret_t ret = {DB_FAIL};
  u16 pos = 0;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  ret = db_add_view_item(p_db_handle, view_id, (u8 *)p_node,
                         sizeof(dvbs_tp_node_t), &pos);
  p_node->id = db_get_element_id_by_pos(p_db_handle, view_id, pos);
  return translate_return_value(ret);
}


BOOL db_dvbs_get_special_pg(u16 ts_id, u16 s_id, dvbs_prog_node_t *p_pg)
{
  BOOL ret = FALSE;
  u16 count = 0;
  u16 pg_id = 0;
  u16 i = 0;
  u8 view_id = 0;
  item_type_t *p_array = NULL;

  MT_ASSERT(p_pg != NULL);

  p_array = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(item_type_t));
  MT_ASSERT(p_array != NULL);
  memset(p_array, 0, DB_DVBS_MAX_PRO * sizeof(item_type_t));

  view_id = db_dvbs_create_view(DB_DVBS_ALL_PG, 0, (u8 *)p_array);
  count = db_dvbs_get_count(view_id);
  for(i = 0; i < count; i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, p_pg);
    if((p_pg->ts_id == ts_id)
      && (p_pg->s_id == s_id))
    {
      ret = TRUE;
      break;
    }
  }
  db_dvbs_destroy_view(view_id);
  mtos_free(p_array);
  return ret;
}


void db_dvbs_change_mark_status(u8 view_id, u16 item_pos,
                                dvbs_element_mark_digit dig, u16 param)
{
  BOOL is_set = ((param & 0x8000) != 0) ? TRUE : FALSE;
  // Operation can be completed based on view id
  BOOL view_only = FALSE;
  BOOL is_del_operation = FALSE;
  // Parameter for view operation
  u8 mask_shift = 0;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  switch(dig)
  {
    case DB_DVBS_MARK_TV:
      break;
    case DB_DVBS_MARK_LCK:
      break;
    case DB_DVBS_MARK_HIDE:
  //  case DB_DVBS_HIDE_TV:
      break;
    case DB_DVBS_MARK_SKP:
      break;
    case DB_DVBS_FAV_GRP:
      mask_shift = param & 0x7FFF;
      break;
    case DB_DVBS_DEL_FLAG:
      view_only = TRUE;
      is_del_operation = TRUE;
      mask_shift = DB_DVBS_MARK_DEL;
      break;
    case DB_DVBS_SEL_FLAG:
      view_only = TRUE;
      mask_shift = DB_DVBS_MARK_SEL;
      break;



  }

  if(view_only == TRUE)
  {
    if(is_del_operation == TRUE)
    {
      if(is_set == TRUE)
      {
        db_del_view_item(p_db_handle, view_id, item_pos);
      }
      else
      {
        db_active_view_item(p_db_handle, view_id, item_pos);
      }
    }
    else
    {
      db_set_mask_view_item(p_db_handle, view_id, item_pos, mask_shift,
                            is_set);
    }
    //db_update_view(view_id);
  } //View operation
  else
  {
    //PG only
    pg_data_t pg_data = { 0 };
    u16 element_id = db_get_element_id_by_pos(p_db_handle, view_id, item_pos);
    u16 cache_data_len = PG_NODE_VALID_CACHE_DATA_LEN;
    BOOL b_ret = FALSE;
    
    b_ret = db_get_element(p_db_handle, g_pg_table_id, element_id, (u8 *)&pg_data, 0, sizeof(pg_data_t));
    MT_ASSERT(b_ret == TRUE);

    switch(dig)
    {
      case DB_DVBS_MARK_TV:
        pg_data.tv_flag = is_set;
        break;
      case DB_DVBS_MARK_LCK:
        pg_data.lck_flag = is_set;
        break;
      case DB_DVBS_MARK_HIDE:
	  case DB_DVBS_HIDE_TV:
        pg_data.hide_flag= is_set;
        break;
      case DB_DVBS_MARK_SKP:
        pg_data.skp_flag = is_set;
        break;
      case DB_DVBS_FAV_GRP:
        if(is_set == TRUE)
        {
          pg_data.fav_grp_flag |= 1 << mask_shift;
        }
        else
        {
          pg_data.fav_grp_flag &= ~(1 << mask_shift);
        }
        break;
      default:
        MT_ASSERT(0);
        break;
    }

    db_edit_element(p_db_handle, g_pg_table_id, element_id, (u8 *)&pg_data,
                    cache_data_len);
  }
}


BOOL db_dvbs_get_mark_status(u8 view_id, u16 item_pos,
                             dvbs_element_mark_digit dig, u16 param)
{
  // Operation can be completed based on view id
  BOOL view_only = FALSE;
  BOOL is_del_operation = FALSE;
  u8 mask_shift = 0;  // Parameter for view operation
  BOOL ret = FALSE;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  //Differenciate view only and not
  switch(dig)
  {
    case DB_DVBS_MARK_TV:
      break;
    case DB_DVBS_MARK_LCK:
      break;
    case DB_DVBS_MARK_HIDE:
//    case DB_DVBS_HIDE_TV:
      break;
    case DB_DVBS_MARK_SKP:
      break;
    case DB_DVBS_FAV_GRP:
      mask_shift = param & 0x7FFF;
      break;
    case DB_DVBS_DEL_FLAG:
      view_only = TRUE;
      is_del_operation = TRUE;
      mask_shift = DB_DVBS_MARK_DEL;
      break;
    case DB_DVBS_SEL_FLAG:
      view_only = TRUE;
      mask_shift = DB_DVBS_MARK_SEL;
      break;
  }

  if(view_only == TRUE)
  {
    //View only
    if(dig == DB_DVBS_DEL_FLAG)
    {
      ret = !(db_get_mask_view_item(p_db_handle, view_id, item_pos,
                                    mask_shift));
    }
    else
    {
      ret = db_get_mask_view_item(p_db_handle, view_id, item_pos, mask_shift);
    }
  }
  else
  {
    //PG only
    pg_data_t pg_data = { 0 };
    u16 element_id = db_get_element_id_by_pos(p_db_handle, view_id, item_pos);

    db_get_element(p_db_handle, g_pg_table_id, element_id, (u8 *)&pg_data, 0, sizeof(pg_data_t));

    switch(dig)
    {
      case DB_DVBS_MARK_TV:
        ret = pg_data.tv_flag;
        break;
      case DB_DVBS_MARK_LCK:
        ret = pg_data.lck_flag;
        break;
	case DB_DVBS_MARK_HIDE:
	case DB_DVBS_HIDE_TV:
        ret = pg_data.hide_flag;
        break;
      case DB_DVBS_MARK_SKP:
        ret = pg_data.skp_flag;
        break;
      case DB_DVBS_FAV_GRP:
        ret = (pg_data.fav_grp_flag & (1 << mask_shift)) ? TRUE : FALSE;
        break;
      default:
        MT_ASSERT(0);
        break;
    }
  }

  return ret;
}


/*!
   \param[in]parent_view_id: id of the view to be searched for the input
      string
   \param[in]p_string_in: string to be searched for in given view
   \param[in/out]p_sub_view_id: the sub-view id
   return    TRUE:  input string existing
            FALSE: input string not found
  */
BOOL db_dvbs_find(u8 parent_view_id, const u16 *p_string_in,
                  u8 *p_sub_view_id)
{
  u16 view_item_cnt = 0;
  void *p_db_handle = class_get_handle_by_id(DB_CLASS_ID);

  MT_ASSERT(p_db_handle != NULL);

  // str_uni2asc(asc_str, (const u16 *) p_string_in);
  memset(g_find_view_buf, 0, MAX_VIEW_BUFFER_SIZE);
  view_item_cnt = db_create_sub_virtual_view(p_db_handle,
                                             string_find_filter,
                                             FALSE,
                                             g_find_view_buf,
                                             (u32)p_string_in,
                                             p_sub_view_id,
                                             parent_view_id);

  if(view_item_cnt == 0)
  {
    //Invalid sub-view is found
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}


void db_dvbs_pg_sort_init(u8 view_id)
{
  s16 view_cnt = 0;
  s16 outer_cnt = 0;
  u16 prv_element_id = 0;
  dvbs_prog_node_t prv_prog_node = {0};
  db_dvbs_ret_t ret = DB_DVBS_OK;
  u32 pos_off = 0;
  u16 *p_mem_tmp = NULL;
  pro_sort_t *p_pro_item = NULL;
  s16 i = 0, j = 0;
  u16 count = 0;
  dvbs_sort_type_t sort_type;

  p_sort_buf = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(pro_sort_t));
  MT_ASSERT(p_sort_buf != NULL);
  memset(p_sort_buf, 0, DB_DVBS_MAX_PRO * sizeof(pro_sort_t));

  p_sort_id_buf = mtos_malloc(33 * sizeof(pro_sort_id_t));
  MT_ASSERT(p_sort_id_buf != NULL);
  memset(p_sort_id_buf, 0, 33 * sizeof(pro_sort_id_t));

  g_name_view.sort_type = DB_DVBS_A_Z_MODE;
  g_name_view.view_init = 0;
  g_name_view.view_count = 0;
  g_name_view.p_mem = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(g_name_view.p_mem != NULL);
  memset(g_name_view.p_mem, 0x0, DB_DVBS_MAX_PRO * sizeof(u16));

  g_lock_view.sort_type = DB_DVBS_LOCK_FIRST_MODE;
  g_lock_view.view_init = 0;
  g_lock_view.view_count = 0;
  g_lock_view.p_mem = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(g_lock_view.p_mem != NULL);
  memset(g_lock_view.p_mem, 0x0, DB_DVBS_MAX_PRO * sizeof(u16));

  g_free_view.sort_type = DB_DVBS_FTA_FIRST_MODE;
  g_free_view.view_init = 0;
  g_free_view.view_count = 0;
  g_free_view.p_mem = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(g_free_view.p_mem != NULL);
  memset(g_free_view.p_mem, 0x0, DB_DVBS_MAX_PRO * sizeof(u16));
  g_hd_view.sort_type = DB_DVBS_HD_FIRST_MODE;
  g_hd_view.view_init = 0;
  g_hd_view.view_count = 0;
  g_hd_view.p_mem = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(g_hd_view.p_mem != NULL);
  memset(g_hd_view.p_mem, 0x0, DB_DVBS_MAX_PRO * sizeof(u16));

  g_default_view.sort_type = DB_DVBS_DEFAULT_ORDER_MODE;
  g_default_view.view_init = 0;
  g_default_view.view_count = 0;
  g_default_view.p_mem = mtos_malloc(DB_DVBS_MAX_PRO * sizeof(u16));
  MT_ASSERT(g_default_view.p_mem != NULL);
  memset(g_default_view.p_mem, 0x0, DB_DVBS_MAX_PRO * sizeof(u16));

  view_cnt = db_dvbs_get_count(view_id);
  p_pro_item = (pro_sort_t *)p_sort_buf;

  pos_off = 0;
  count = 0;
  sort_type = DB_DVBS_A_Z_MODE;

  for(outer_cnt = 0; outer_cnt < view_cnt; outer_cnt++)
  {
    prv_element_id = db_dvbs_get_id_by_view_pos(view_id, outer_cnt);
    ret = db_dvbs_get_pg_by_id(prv_element_id, &prv_prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);

    //a-z
    for(i = 0; i < 26; i++)
    {
      if(prv_prog_node.name[0] == (0x41 + i)
        || prv_prog_node.name[0] == (0x61 + i))
      {
        if(p_sort_id_buf[i].id_init == 0)
        {
          p_sort_id_buf[i].id_init = 1;
          p_sort_id_buf[i].id_count = 1;
          p_sort_id_buf[i].id_offset = 0;
          p_sort_id_buf[i].id_start = 0;
          break;
        }
        else
        {
          p_sort_id_buf[i].id_count++;
          break;
        }
      }
    }

    //other char
    if(i == 26)
    {
      if(p_sort_id_buf[i].id_init == 0)
      {
        p_sort_id_buf[i].id_init = 1;
        p_sort_id_buf[i].id_count = 1;
        p_sort_id_buf[i].id_offset = 0;
        p_sort_id_buf[i].id_start = 0;
      }
      else
      {
        p_sort_id_buf[i].id_count++;
      }
    }

    //fta
    if(prv_prog_node.is_scrambled == FALSE)
    {
      if(p_sort_id_buf[27].id_init == 0)
      {
        p_sort_id_buf[27].id_init = 1;
        p_sort_id_buf[27].id_count = 1;
        p_sort_id_buf[27].id_offset = 0;
        p_sort_id_buf[27].id_start = 0;
      }
      else
      {
        p_sort_id_buf[27].id_count++;
      }
    }
    else
    {
      if(p_sort_id_buf[28].id_init == 0)
      {
        p_sort_id_buf[28].id_init = 1;
        p_sort_id_buf[28].id_count = 1;
        p_sort_id_buf[28].id_offset = 0;
        p_sort_id_buf[28].id_start = 0;
      }
      else
      {
        p_sort_id_buf[28].id_count++;
      }
    }

    //lock
    if(prv_prog_node.lck_flag == FALSE)
    {
      if(p_sort_id_buf[29].id_init == 0)
      {
        p_sort_id_buf[29].id_init = 1;
        p_sort_id_buf[29].id_count = 1;
        p_sort_id_buf[29].id_offset = 0;
        p_sort_id_buf[29].id_start = 0;
      }
      else
      {
        p_sort_id_buf[29].id_count++;
      }
    }
    else
    {
      if(p_sort_id_buf[30].id_init == 0)
      {
        p_sort_id_buf[30].id_init = 1;
        p_sort_id_buf[30].id_count = 1;
        p_sort_id_buf[30].id_offset = 0;
        p_sort_id_buf[30].id_start = 0;
      }
      else
      {
        p_sort_id_buf[30].id_count++;
      }
    }

    if(prv_prog_node.service_type == 0x11)
    {
      if(p_sort_id_buf[31].id_init == 0)
      {
        p_sort_id_buf[31].id_init = 1;
        p_sort_id_buf[31].id_count = 1;
        p_sort_id_buf[31].id_offset = 0;
        p_sort_id_buf[31].id_start = 0;
      }
      else
      {
        p_sort_id_buf[31].id_count++;
      }
    }
    else
    {
      if(p_sort_id_buf[32].id_init == 0)
      {
        p_sort_id_buf[32].id_init = 1;
        p_sort_id_buf[32].id_count = 1;
        p_sort_id_buf[32].id_offset = 0;
        p_sort_id_buf[32].id_start = 0;
      }
      else
      {
        p_sort_id_buf[32].id_count++;
      }
    }
    p_pro_item++;
    pos_off += sizeof(pro_sort_t);
  }

  //(a-z) + other char.
  for(i = 0; i < 27; i++)
  {
    count = 0;
    for(j = 0; j < i; j++)
    {
      if(p_sort_id_buf[j].id_init == 1)
      {
        count += p_sort_id_buf[j].id_count;
      }
    }

    if(p_sort_id_buf[i].id_init == 1)
    {
      p_sort_id_buf[i].id_start = count;
    }
  }

  //fta and locked and letters separate
  if(p_sort_id_buf[27].id_init == 1)
  {
    p_sort_id_buf[27].id_start = 0;
  }

  if(p_sort_id_buf[28].id_init == 1)
  {
    p_sort_id_buf[28].id_start += p_sort_id_buf[27].id_count;
  }

  if(p_sort_id_buf[29].id_init == 1)
  {
    p_sort_id_buf[29].id_start = 0;
  }

  if(p_sort_id_buf[30].id_init == 1)
  {
    p_sort_id_buf[30].id_start += p_sort_id_buf[29].id_count;
  }

  if(p_sort_id_buf[31].id_init == 1)
  {
    p_sort_id_buf[31].id_start = 0;
  }
  if(p_sort_id_buf[32].id_init == 1)
  {
    p_sort_id_buf[32].id_start += p_sort_id_buf[31].id_count;
  }
  pos_off = 0;
  for(outer_cnt = 0; outer_cnt < view_cnt; outer_cnt++)
  {
    prv_element_id = db_dvbs_get_id_by_view_pos(view_id, outer_cnt);
    ret = db_dvbs_get_pg_by_id(prv_element_id, &prv_prog_node);
    MT_ASSERT(ret == DB_DVBS_OK);

    for(i = 0; i < 26; i++)
    {
      if(prv_prog_node.name[0] == (0x41 + i)
        || prv_prog_node.name[0] == (0x61 + i))
      {
        if(p_sort_id_buf[i].id_init == 1)
        {
          p_pro_item = p_sort_buf + p_sort_id_buf[i].id_start + p_sort_id_buf[i].id_offset;
          p_pro_item->node_id = prv_element_id;
          memcpy(p_pro_item->name, prv_prog_node.name, DB_DVBS_MAX_NAME_LENGTH + 1);
          p_pro_item->is_scrambled = prv_prog_node.is_scrambled;
          p_pro_item->lck_flag = prv_prog_node.lck_flag;
          p_pro_item->service_type = prv_prog_node.service_type;
          p_pro_item->default_order = prv_prog_node.default_order;

          p_sort_id_buf[i].id_offset++;
        }
        break;
      }
    }

    if(i == 26)
    {
      if(p_sort_id_buf[i].id_init == 1)
      {
        p_pro_item = p_sort_buf + p_sort_id_buf[i].id_start + p_sort_id_buf[i].id_offset;
        p_pro_item->node_id = prv_element_id;
        memcpy(p_pro_item->name, prv_prog_node.name, DB_DVBS_MAX_NAME_LENGTH + 1);
        p_pro_item->is_scrambled = prv_prog_node.is_scrambled;
        p_pro_item->lck_flag = prv_prog_node.lck_flag;
        p_pro_item->service_type = prv_prog_node.service_type;
        p_pro_item->default_order = prv_prog_node.default_order;

        p_sort_id_buf[i].id_offset++;
      }
    }

    if(prv_prog_node.is_scrambled == FALSE)
    {
      if(p_sort_id_buf[27].id_init == 1)
      {
        p_mem_tmp = g_free_view.p_mem + p_sort_id_buf[27].id_start + p_sort_id_buf[27].id_offset;
        *p_mem_tmp = prv_element_id | 0x8000;

        p_sort_id_buf[27].id_offset++;
      }
    }
    else
    {
      if(p_sort_id_buf[28].id_init == 1)
      {
        p_mem_tmp = g_free_view.p_mem + p_sort_id_buf[28].id_start + p_sort_id_buf[28].id_offset;
        *p_mem_tmp = prv_element_id | 0x8000;

        p_sort_id_buf[28].id_offset++;
      }
    }

    if(prv_prog_node.lck_flag == FALSE)
    {
      if(p_sort_id_buf[29].id_init == 1)
      {
        p_mem_tmp = g_lock_view.p_mem + p_sort_id_buf[29].id_start + p_sort_id_buf[29].id_offset;
        *p_mem_tmp = prv_element_id | 0x8000;

        p_sort_id_buf[29].id_offset++;
      }
    }
    else
    {
      if(p_sort_id_buf[30].id_init == 1)
      {
        p_mem_tmp = g_lock_view.p_mem + p_sort_id_buf[30].id_start + p_sort_id_buf[30].id_offset;
        *p_mem_tmp = prv_element_id | 0x8000;

        p_sort_id_buf[30].id_offset++;
      }
    }
    if(prv_prog_node.service_type == 0x11)
    {
      if(p_sort_id_buf[31].id_init == 1)
      {
        p_mem_tmp = g_hd_view.p_mem + p_sort_id_buf[31].id_start + p_sort_id_buf[31].id_offset;
        *p_mem_tmp = prv_element_id | 0x8000;
        p_sort_id_buf[31].id_offset++;
      }
    }
    else
    {
      if(p_sort_id_buf[32].id_init == 1)
      {
        p_mem_tmp = g_hd_view.p_mem + p_sort_id_buf[32].id_start + p_sort_id_buf[32].id_offset;
        *p_mem_tmp = prv_element_id | 0x8000;
        p_sort_id_buf[32].id_offset++;
      }
    }

    pos_off += sizeof(pro_sort_t);
  }

  //OS_PRINTF("#####init delta tick = %d \n", mtos_ticks_get()-ticks);
}


static BOOL str_cmp_a_z(pro_sort_t *prev_node, pro_sort_t *cur_node)
{
  return uni_strcmp(prev_node->name, cur_node->name);
}


static BOOL str_cmp_z_a(pro_sort_t *prev_node, pro_sort_t *cur_node)
{
  return uni_strcmp(cur_node->name, prev_node->name);
}


static BOOL default_order_cmp(pro_sort_t *prev_node, pro_sort_t *cur_node)
{
  return prev_node->default_order < cur_node->default_order;
}


void pro_str_change(pro_sort_t *p_1, pro_sort_t *p_2)
{
  pro_sort_t temp;
  int size = sizeof(pro_sort_t);

  memcpy(&temp, p_1, size);
  memcpy(p_1, p_2, size);
  memcpy(p_2, &temp, size);
}


void shellsort(pro_sort_t *p_sort, u32 count, dvbs_sort_type_t sort_type)
{
  u32 i, j, k;
  pro_sort_t temp_prog = {0};
  db_dvbs_qsort_func sort_func = NULL;
  u16 size = sizeof(pro_sort_t);

  switch(sort_type)
  {
    case DB_DVBS_A_Z_MODE:
      sort_func = str_cmp_a_z;
      break;

    case DB_DVBS_Z_A_MODE:
      sort_func = str_cmp_z_a;
      break;

    case DB_DVBS_FTA_FIRST_MODE:
    case DB_DVBS_LOCK_FIRST_MODE:
    case DB_DVBS_DEFAULT_ORDER_MODE:
      sort_func = default_order_cmp;
      break;

    default:
      MT_ASSERT(0);
      break;
  }

  for(i = 0; i < (count - 1); i++)
  {
    k = i;
    for(j = i + 1; j< count; j++)
    {
        if(sort_func(&p_sort[j], &p_sort[k]) > 0)
            k = j;
    }

    if(k != i)
    {
        memcpy(&temp_prog, &p_sort[i], size);
        memcpy(&p_sort[i], &p_sort[k], size);
        memcpy(&p_sort[k], &temp_prog, size);
    }
  }
}


/*!
   \param[in]parent_view_id: view to be sorted
   \param[in]sort_type:			sort type to be selected
   \param[in]p_sub_view_id:	the sub-view created based on sort result
   \param[in]view:						view to be sorted
  */
void db_dvbs_pg_sort(u8 view_id, dvbs_sort_type_t sort_type)
{
  s16 view_cnt = 0, i = 0, j = 0;
  u32 pos_off = 0;
  pro_sort_t *p_pro_item = NULL;
  u16 n = 0, count = 0, temp = 0, group_num = 0, *p_mem = NULL, *p_mem1 = NULL;
  pro_sort_id_t all = {0};

  //Set sort function
  switch(sort_type)
  {
    case DB_DVBS_A_Z_MODE:
    case DB_DVBS_Z_A_MODE:
      n = 27;
      break;

    case DB_DVBS_FTA_FIRST_MODE:
    case DB_DVBS_LOCK_FIRST_MODE:
    case DB_DVBS_HD_FIRST_MODE:
    case DB_DVBS_DEFAULT_ORDER_MODE:
      break;

    default:
      OS_PRINTF("%s, sort_type : %d\n", __FUNCTION__, sort_type);
      MT_ASSERT(0);
      break;
  }

  p_pro_item = (pro_sort_t *)p_sort_buf;
  pos_off = 0;
  count = 0;
  view_cnt = db_dvbs_get_count(view_id);

  p_mem = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_mem != NULL);
  memset(p_mem, 0, view_cnt * sizeof(u16));
  p_mem1 = mtos_malloc(view_cnt * sizeof(u16));
  MT_ASSERT(p_mem1 != NULL);
  memset(p_mem1, 0, view_cnt * sizeof(u16));

  if(sort_type == DB_DVBS_A_Z_MODE  || sort_type == DB_DVBS_Z_A_MODE)
  {
    if(g_name_view.view_init == 0)
    {
      count = 0;
      for(i = 0; i < n; i++)
      {
        if(p_sort_id_buf[i].id_init == 1)
        {
          shellsort(p_sort_buf + p_sort_id_buf[i].id_start, p_sort_id_buf[i].id_count, DB_DVBS_A_Z_MODE);
        }
      }

      for(i = 0; i < view_cnt; i++)
      {
        *(p_mem + i) = p_sort_buf[i].node_id | 0x8000;
      }

      if(sort_type != DB_DVBS_A_Z_MODE)
      {
        for(i = 0; i < view_cnt / 2; i++)
        {
          temp = *(p_mem + i);
          *(p_mem + i) = *(p_mem + (view_cnt - 1 - i));
          *(p_mem + (view_cnt - 1 - i)) = temp;
        }
      }

      g_name_view.view_init = 1;
      g_name_view.view_count = view_cnt;
      g_name_view.sort_type = sort_type;

      for(i = 0; i < view_cnt; i++)
      {
        *(g_name_view.p_mem + i) = *(p_mem + i);
      }
    }
    else
    {
      if(g_name_view.sort_type == sort_type)
      {
        for(i = 0; i < g_name_view.view_count; i++)
        {
          *(p_mem + i) = *(g_name_view.p_mem + i);
        }
      }
      else
      {
        for(i = 0; i < g_name_view.view_count; i++)
        {
          *(p_mem + i) = *(g_name_view.p_mem + i);
        }
        for(i = 0; i < g_name_view.view_count / 2; i++)
        {
          temp = *(p_mem + i);
          *(p_mem + i) = *(p_mem + (g_name_view.view_count - 1 - i));
          *(p_mem + (g_name_view.view_count - 1 - i)) = temp;
        }
      }
    }
  }
  else if(sort_type == DB_DVBS_FTA_FIRST_MODE)
  {
    if(g_free_view.view_init == 0)
    {
      count = 0;

      g_free_view.view_init = 1;
      g_free_view.view_count = view_cnt;
      g_free_view.sort_type = sort_type;

      for(i = 0; i < g_free_view.view_count; i++)
      {
        *(p_mem + i) = *(g_free_view.p_mem + i);
      }
    }
    else
    {
      if(g_free_view.sort_type == sort_type)
      {
        for(i = 0; i < g_free_view.view_count; i++)
        {
          *(p_mem + i) = *(g_free_view.p_mem + i);
        }
        for(i = 0; i < g_free_view.view_count / 2; i++)
        {
          temp = *(p_mem + i);
          *(p_mem + i) = *(p_mem + (g_free_view.view_count - 1 - i));
          *(p_mem + (g_free_view.view_count - 1 - i)) = temp;
        }
        for(i = 0; i < view_cnt; i++)
        {
          *(g_free_view.p_mem + i) = *(p_mem + i);
        }
      }
      count = g_free_view.view_count;
    }
  }
  else if(sort_type == DB_DVBS_LOCK_FIRST_MODE)
  {
    if(g_lock_view.view_init == 0)
    {
      count = 0;

      g_lock_view.view_init = 1;
      g_lock_view.view_count = view_cnt;
      g_lock_view.sort_type = sort_type;

      for(i = 0; i < g_lock_view.view_count; i++)
      {
        *(p_mem + i) = *(g_lock_view.p_mem + i);
      }
    }
    else
    {
      if(g_lock_view.sort_type == sort_type)
      {
        for(i = 0; i < g_lock_view.view_count; i++)
        {
          *(p_mem + i) = *(g_lock_view.p_mem + i);
        }
        for(i = 0; i < g_lock_view.view_count / 2; i++)
        {
          temp = *(p_mem + i);
          *(p_mem + i) = *(p_mem + (g_lock_view.view_count - 1 - i));
          *(p_mem + (g_lock_view.view_count - 1 - i)) = temp;
        }
        for(i = 0; i < view_cnt; i++)
        {
          *(g_lock_view.p_mem + i) = *(p_mem + i);
        }
      }
      count = g_lock_view.view_count;
    }
  }
  else if(sort_type == DB_DVBS_HD_FIRST_MODE)
  {
    if(g_hd_view.view_init == 0)
    {
      count = 0;
      g_hd_view.view_init = 1;
      g_hd_view.view_count = view_cnt;
      g_hd_view.sort_type = sort_type;
      for(i = 0; i < g_hd_view.view_count; i++)
      {
        *(p_mem + i) = *(g_hd_view.p_mem + i);
      }
    }
    else
    {
      if(g_hd_view.sort_type == sort_type)
      {
        for(i = 0; i < g_hd_view.view_count; i++)
        {
          *(p_mem + i) = *(g_hd_view.p_mem + i);
        }
        for(i = 0; i < g_hd_view.view_count / 2; i++)
        {
          temp = *(p_mem + i);
          *(p_mem + i) = *(p_mem + (g_hd_view.view_count - 1 - i));
          *(p_mem + (g_hd_view.view_count - 1 - i)) = temp;
        }
        for(i = 0; i < view_cnt; i++)
        {
          *(g_hd_view.p_mem + i) = *(p_mem + i);
        }
      }
      count = g_hd_view.view_count;
    }
  }

  if(sort_type == DB_DVBS_DEFAULT_ORDER_MODE)
  {
    if(g_default_view.view_init == 0)
    {
      count = 0;
      all.id_count = view_cnt;
      all.id_offset = view_cnt;
      all.id_start = 0;
      all.id_init = 1;
      group_num = sys_status_get_all_group_num();

      if(all.id_init == 1)
      {
        shellsort(p_sort_buf + all.id_start, all.id_count, sort_type);
      }

      for(i = 0; i < view_cnt; i++)
      {
        *(p_mem1 + i) = p_sort_buf[i].node_id | 0x8000;
      }

      for(j = 0; j < group_num; j++)
      {
        for(i = j; i < view_cnt; i = i + group_num)
        {
          *(p_mem + i) = *(p_mem1 + i);
        }
      }

      g_default_view.view_init = 1;
      g_default_view.view_count = view_cnt;
      g_default_view.sort_type = sort_type;

      for(i = 0; i < view_cnt; i++)
      {
        *(g_default_view.p_mem + i) = *(p_mem + i);
      }
    }
    else
    {
      if(g_default_view.sort_type == sort_type)
      {
        for(i = 0; i < g_default_view.view_count; i++)
        {
          *(p_mem + i) = *(g_default_view.p_mem + i);
        }
      }
    }
  }

  db_dvbs_dump_item_in_view(view_id, p_mem, view_cnt);

  mtos_free(p_mem);
  mtos_free(p_mem1);
}


void db_dvbs_pg_sort_deinit()
{
  mtos_free(p_sort_buf);
  mtos_free(p_sort_id_buf);

  g_name_view.sort_type = DB_DVBS_A_Z_MODE;
  g_name_view.view_init = 0;
  g_name_view.view_count = 0;
  mtos_free(g_name_view.p_mem);
  g_name_view.p_mem = NULL;

  g_free_view.sort_type = DB_DVBS_FTA_FIRST_MODE;
  g_free_view.view_init = 0;
  g_free_view.view_count = 0;
  mtos_free(g_free_view.p_mem);
  g_free_view.p_mem = NULL;

  g_lock_view.sort_type = DB_DVBS_LOCK_FIRST_MODE;
  g_lock_view.view_init = 0;
  g_lock_view.view_count = 0;
  mtos_free(g_lock_view.p_mem);
  g_lock_view.p_mem = NULL;

  g_hd_view.sort_type = DB_DVBS_HD_FIRST_MODE;
  g_hd_view.view_init = 0;
  g_hd_view.view_count = 0;
  mtos_free(g_hd_view.p_mem);
  g_hd_view.p_mem = NULL;
  g_default_view.sort_type = DB_DVBS_DEFAULT_ORDER_MODE;
  g_default_view.view_init = 0;
  g_default_view.view_count = 0;
  mtos_free(g_default_view.p_mem);
  g_default_view.p_mem = NULL;
}


/*!
   \param[in]parent_view_id: view to be sorted
   \param[in]sort_type:			sort type to be selected
   \param[in]p_sub_view_id:	the sub-view created based on sort result
   \param[in]view:						view to be sorted
  */
void db_dvbs_sat_sort(u8 view_id, dvbs_sort_type_t sort_type)
{
  db_dvbs_sort_func sort_func = NULL;
  u16 view_cnt = 0;

  u16 inner_cnt = 0;
  u16 outer_cnt = 0;

  u16 cur_element_id = 0;
  u16 prv_element_id = 0;

  sat_node_t cur_sat_node = {0};
  sat_node_t prv_sat_node = {0};
  db_dvbs_ret_t ret = DB_DVBS_OK;

  //Set sort function
  switch(sort_type)
  {
    case DB_DVBS_A_Z_MODE:
      sort_func = name_a_z_sort;
      break;
    case DB_DVBS_Z_A_MODE:
      sort_func = name_z_a_sort;
      break;
    case DB_DVBS_FTA_FIRST_MODE:
      sort_func = fta_first_sort;
      break;
    case DB_DVBS_LOCK_FIRST_MODE:
      sort_func = lock_first_sort;
      break;
    case DB_DVBS_DEFAULT_ORDER_MODE:
      sort_func = default_order_sort;
      break;
    default:
      break;
  }

  view_cnt = db_dvbs_get_count(view_id);

  for(outer_cnt = view_cnt; outer_cnt >= 0; outer_cnt--)
  {
    for(inner_cnt = 0; inner_cnt < outer_cnt; inner_cnt++)
    {
      if(inner_cnt == 0)
      {
        prv_element_id = db_dvbs_get_id_by_view_pos(view_id, inner_cnt);
        ret = db_dvbs_get_sat_by_id(prv_element_id, &prv_sat_node);
        MT_ASSERT(ret == DB_DVBS_OK);
      }

      cur_element_id = db_dvbs_get_id_by_view_pos(view_id, inner_cnt + 1);
      ret = db_dvbs_get_sat_by_id(cur_element_id, &cur_sat_node);
      MT_ASSERT(ret == DB_DVBS_OK);

      if(sort_func((u8 *)&cur_sat_node, (u8 *)&prv_sat_node, DB_DVBS_PG_NODE)
         == TRUE)
      {
        //Remain current node status unchanged
        db_dvbs_exchange_item_in_view(view_id, outer_cnt, inner_cnt);
      }
      else
      {
        //Set current node to previous
        memcpy(&prv_sat_node, &cur_sat_node, sizeof(dvbs_prog_node_t));
      }
    }
  }
}


/*!
   \param[in]parent_view_id: view to be sorted
   \param[in]sort_type:			sort type to be selected
   \param[in]p_sub_view_id:	the sub-view created based on sort result
   \param[in]view:						view to be sorted
  */
void db_dvbs_tp_sort(u8 view_id, dvbs_sort_type_t sort_type)
{
  db_dvbs_sort_func sort_func = NULL;
  u16 view_cnt = 0;

  u16 inner_cnt = 0;
  u16 outer_cnt = 0;

  u16 cur_element_id = 0;
  u16 prv_element_id = 0;

  dvbs_tp_node_t cur_tp_node = {0};
  dvbs_tp_node_t prv_tp_node = {0};
  db_dvbs_ret_t ret = DB_DVBS_OK;

  //Set sort function
  switch(sort_type)
  {
    case DB_DVBS_A_Z_MODE:
      sort_func = name_a_z_sort;
      break;
    case DB_DVBS_Z_A_MODE:
      sort_func = name_z_a_sort;
      break;
    case DB_DVBS_FTA_FIRST_MODE:
      sort_func = fta_first_sort;
      break;
    case DB_DVBS_LOCK_FIRST_MODE:
      sort_func = lock_first_sort;
      break;
    case DB_DVBS_DEFAULT_ORDER_MODE:
      sort_func = default_order_sort;
      break;
    default:
      break;
  }

  //Create view
  view_cnt = db_dvbs_get_count(view_id);

  for(outer_cnt = view_cnt; outer_cnt >= 0; outer_cnt--)
  {
    for(inner_cnt = 0; inner_cnt < outer_cnt; inner_cnt++)
    {
      if(inner_cnt == 0)
      {
        prv_element_id = db_dvbs_get_id_by_view_pos(view_id, inner_cnt);
        ret = db_dvbs_get_tp_by_id(prv_element_id, &prv_tp_node);
        MT_ASSERT(ret == DB_DVBS_OK);
      }

      cur_element_id = db_dvbs_get_id_by_view_pos(view_id, inner_cnt + 1);
      ret = db_dvbs_get_tp_by_id(cur_element_id, &cur_tp_node);
      MT_ASSERT(ret == DB_DVBS_OK);

      if(sort_func((u8 *)&cur_tp_node, (u8 *)&prv_tp_node, DB_DVBS_PG_NODE)
         == TRUE)
      {
        //Remain current node status unchanged
        db_dvbs_exchange_item_in_view(view_id, outer_cnt, inner_cnt);
      }
      else
      {
        //Set current node to previous
        memcpy(&prv_tp_node, &cur_tp_node, sizeof(dvbs_prog_node_t));
      }
    }
  }

  //db_dvbs_destroy_view(view_id);
}


u32 get_audio_channel(dvbs_prog_node_t *p_pg)
{
  language_set_t lang_set;
  char **p_lang_code_list_b = sys_status_get_lang_code(TRUE);
  char **p_lang_code_list_t = sys_status_get_lang_code(FALSE);
  char *p_cur_lang_code_b = NULL;
  char *p_cur_lang_code_t = NULL;
  u16 b_type = 0;
  u16 t_type = 0;
  BOOL found_trace = FALSE;
  u16 pg_lang = 0;
  u32 i = 0;

  //get current language code
  sys_status_get_lang_set(&lang_set);

  //search first audio trace
  p_cur_lang_code_b = p_lang_code_list_b[lang_set.first_audio];
  p_cur_lang_code_t = p_lang_code_list_t[lang_set.first_audio];
  b_type = iso_639_2_idx_to_type(iso_639_2_desc_to_idx(p_cur_lang_code_b));
  t_type = iso_639_2_idx_to_type(iso_639_2_desc_to_idx(p_cur_lang_code_t));

  for(i = 0; i < p_pg->audio_ch_num; i++)
  {
    //match language code
    //if((memcmp(p_cur_lang_code_b, p_pg->audio[i].language, LANGUAGE_LEN) == 0)
    //  ||(memcmp(p_cur_lang_code_t, p_pg->audio[i].language, LANGUAGE_LEN) == 0))
    pg_lang = iso_639_2_idx_to_type(p_pg->audio[i].language_index);

    if((pg_lang == b_type)
      || (pg_lang == t_type))
    {
      found_trace = TRUE;
      break;
    }
  }

  //search second audio trace
  if(!found_trace)
  {
    p_cur_lang_code_b = p_lang_code_list_b[lang_set.second_audio];
    p_cur_lang_code_t = p_lang_code_list_t[lang_set.second_audio];
    b_type = iso_639_2_idx_to_type(iso_639_2_desc_to_idx(p_cur_lang_code_b));
    t_type = iso_639_2_idx_to_type(iso_639_2_desc_to_idx(p_cur_lang_code_t));

    for(i = 0; i < p_pg->audio_ch_num; i++)
    {
      //match language code
      //if((memcmp(p_cur_lang_code_b, p_pg->audio[i].language, LANGUAGE_LEN) == 0)
      //  ||(memcmp(p_cur_lang_code_t, p_pg->audio[i].language, LANGUAGE_LEN) == 0))
      pg_lang = iso_639_2_idx_to_type(p_pg->audio[i].language_index);

      if((pg_lang == b_type)
        || (pg_lang == t_type))
      {
        found_trace = TRUE;
        break;
      }
    }
  }
  return found_trace ? i : 0;
}


u16 db_dvbs_get_sat_degree(sat_node_t *p_sat)
{
  u16 sat_degree = 0;
  u16 sat_longitude = p_sat->longitude;

  if(sat_longitude > 0x8000)
  {
    sat_longitude -= 0x8000;

    sat_degree = 3600 - sat_longitude / 10;
  }
  else
  {
    sat_degree = sat_longitude / 10;
  }
  return sat_degree;
}

void  db_dvbs_set_no_part_az(BOOL is_part_az_AZ)
{
  g_is_part_az_AZ = is_part_az_AZ;
}

BOOL  db_dvbs_get_no_part_az(void)
{
  return g_is_part_az_AZ;
}
