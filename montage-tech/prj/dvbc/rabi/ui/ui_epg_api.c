#include "ui_common.h"
#include "ui_epg_api.h"
#include "ui_config.h"

#ifdef IMPL_NEW_EPG
//eva
#include "media_format_define.h"
#include "interface.h"
#include "eva.h"
#include "ipin.h"
#include "ifilter.h"
#include "eva_filter_factory.h"
#include "demux_interface.h"
#include "demux_filter.h"
#include "epg_type.h"

u16 ui_epg_evtmap(u32 event);
static void get_current_epg_program_info(epg_prog_info_t *prog_info);


static BOOL g_epg_start = FALSE;
static BOOL g_epg_init = FALSE;
static epg_table_selecte_policy run_policy = EPG_TABLE_SELECTE_UNKOWN;

static u8 *p_epg_new_buf =  NULL;
static BOOL g_epg_buf_new = FALSE;

void epg_api_evt_process(u32 content, u32 para1, u32 para2)
{
    event_t evt = {0};
    BOOL  send_flag = TRUE;
    
    evt.id = content;
    evt.data1 = para1;
    evt.data2 = para2;
   
    switch(content)
    {
      case EPG_API_SCHE_EVENT:
      if(para1 == EVENT_ATTR_OTHERS_PROG)
      {
        return;
      }
//      OS_PRINTF("epg_api_evt_process status:EPG_API_SCHE_EVENT\n"); 
      break;
      case EPG_API_DEL_EVENT:
//      OS_PRINTF("epg_api_evt_process status:EPG_API_DEL_EVENT\n"); 
      break;
      case EPG_API_PF_EVENT:
       if(para1 == EVENT_ATTR_OTHERS_PROG)
      {
        return;
      }
//      OS_PRINTF("epg_api_evt_process status:EPG_API_PF_EVENT\n");  
      break;
      case EPG_API_PF_SECTION_FINISH:
//      OS_PRINTF("epg_api_evt_process status:EPG_API_PF_SECTION_FINISH\n");  
      break;
      case EPG_API_PF_SECTION_TIMEOUT:
//       OS_PRINTF("epg_api_evt_process status:EPG_API_PF_SECTION_TIMEOUT\n");  
      break;
      case EPG_API_EIT_TIMEOUT:
      // OS_PRINTF("epg_api_evt_process status:EPG_API_EIT_TIMEOUT\n");  
       break;
      default :
       send_flag = FALSE;    
      break;
    }
    if(TRUE == send_flag)
    {
      //OS_PRINTF("epg_api_evt_process content[%d]\n", content);  
      ap_frm_send_evt_to_ui(APP_EPG, &evt);
    }
}

void epg_api_init(void)
{
  epg_init_para_t para;
  epg_config_t  epg_config;
  
  memset(&para,0,sizeof(epg_init_para_t));
  para.task_priority = AP_EPG_TASK_PRIORITY;
  para.task_stk_size = AP_EPG_TASK_STKSIZE;
  para.evt_process = epg_api_evt_process;
   mul_epg_init(&para);

  memset(&epg_config,0,sizeof(epg_config_t));
  #ifndef WIN32 
    epg_config.p_attach_buf = (u8 *)mem_mgr_require_block(BLOCK_EPG_BUFFER, SYS_MODULE_EPG);
    MT_ASSERT(epg_config.p_attach_buf != NULL);
    epg_config.buffer_size = mem_mgr_get_block_size(BLOCK_EPG_BUFFER);
    mem_mgr_release_block(BLOCK_EPG_BUFFER);
    OS_PRINTF("%s epg addr 0x%x\n",__FUNCTION__,epg_config.p_attach_buf);
    p_epg_new_buf =  NULL;
    g_epg_buf_new = FALSE;
    epg_config.config_open = FALSE;
  #else
  epg_config.buffer_size = 1 * KBYTES * KBYTES;                                                                             
  epg_config.p_attach_buf = (u8 *)mtos_malloc(epg_config.buffer_size);
  MT_ASSERT(epg_config.p_attach_buf != NULL);
    p_epg_new_buf =  epg_config.p_attach_buf;
    g_epg_buf_new = TRUE;
  #endif

  epg_db_set_lang_code("eng","eng","eng");

  mul_epg_config(&epg_config);                                                                          
      
}

void ui_epg_init(void)
{
  if (g_epg_init)
  {
    OS_PRINTF("ui epg already init!\n");
    return;
  }
  epg_api_init();
  fw_register_ap_evtmap(APP_EPG, ui_epg_evtmap);
  fw_register_ap_msghost(APP_EPG, ROOT_ID_VEPG); 
  fw_register_ap_msghost(APP_EPG, ROOT_ID_BACKGROUND); 
  fw_register_ap_msghost(APP_EPG, ROOT_ID_PROG_LIST); 
  fw_register_ap_msghost(APP_EPG, ROOT_ID_PROG_BAR); 
  fw_register_ap_msghost(APP_EPG, ROOT_ID_FAV_LIST); 

  g_epg_init = TRUE;
}
void ui_epg_api_release(void)
{
   ui_epg_stop();
   mul_epg_deinit();
  /******free  epg api config buffer******/
  if ((p_epg_new_buf != NULL) && (g_epg_buf_new == TRUE))
  {
     mtos_free(p_epg_new_buf);
     p_epg_new_buf = NULL;
  } 
}

void ui_epg_release(void)
{
  if (! g_epg_init)
  {
    OS_PRINTF("ui epg already released!\n");
    return;
  }
  
  ui_epg_api_release();
  
  fw_unregister_ap_evtmap(APP_EPG);
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_VEPG); 
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_BACKGROUND); 
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_PROG_LIST); 
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_PROG_BAR); 
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_FAV_LIST);

  g_epg_init = FALSE;
}

void ui_epg_start(epg_table_selecte_policy selecte)
{
  epg_filter_ply_t config;
#if 0
  void *epg_buffer = NULL;
  u32 epg_buffer_size = 0;
#endif

  if((TRUE == g_epg_start ) && (run_policy == selecte))
  {
    return;
  }
  ui_epg_stop(); 
  #if 0
  epg_buffer = (void *)mem_mgr_require_block(BLOCK_REC_BUFFER, SYS_MODULE_EPG);
  MT_ASSERT(epg_buffer!= NULL);
  epg_buffer_size = mem_mgr_get_block_size(BLOCK_REC_BUFFER);
  mem_mgr_release_block(BLOCK_REC_BUFFER);
  //memset((void *)(epg_buffer),0,epg_buffer_size);
    mtos_printk("%s epg addr 0x%x\n",__FUNCTION__,epg_buffer);
  #endif
  if(FALSE == g_epg_start)
  {   
      run_policy = selecte;
      memset(&config,0,sizeof(epg_filter_ply_t));
      config.epg_selecte = selecte;
      config.pf_timeout = 10000;/**ms**/  
      config.eit_timeout = 10000; /**ms***/ 
      config.pf_policy  =  PF_EVENT_OF_NETWORK_ID;//PF_EVENT_OF_NETWORK_ID;
      config.sch_policy = SCH_EVENT_OF_TS_ID;
      config.net_policy = NETWORK_ID_NORMAL;
      switch(selecte)
      {
          case EPG_TABLE_SELECTE_PF_ALL:
          config.evt_fixed_able = TRUE;
          break;
          case EPG_TABLE_SELECTE_PF_ACTUAL:
          config.evt_fixed_able = TRUE;  
          break;
          case EPG_TABLE_SELECTE_PF_OTHER:
          config.evt_fixed_able = TRUE;
          break;
          case EPG_TABLE_SELECTE_SCH_ALL:
          config.evt_fixed_able = FALSE;  
          break;
          case EPG_TABLE_SELECTE_SCH_ACTUAL:
          config.evt_fixed_able = FALSE;  
          break;
          case EPG_TABLE_SELECTE_SCH_OTHER:
          config.evt_fixed_able = FALSE;  
          break;
          case EPG_TABLE_SELECTE_PF_ALL_SCH_ACTUAL:
           config.evt_fixed_able = TRUE; 
          break;
          case EPG_TABLE_SELECTE_FROM_CONFIG:
           config.evt_fixed_able = TRUE; 
          break;
          default:
          break;
      }
      get_current_epg_program_info(&config.prog_info);   
      mul_epg_filter_policy(&config);  
      mul_epg_start();              
      g_epg_start = TRUE;
  }
}

void ui_epg_stop(void)
{   
    if(TRUE == g_epg_start )
    {
        g_epg_start = FALSE;
        mul_epg_stop();
    } 
 }
   
static void get_current_epg_program_info(epg_prog_info_t *prog_info)
{
    dvbs_prog_node_t pg = {0};
    utc_time_t g_time_st = {0};
    utc_time_t g_time_end = {0};
    db_dvbs_ret_t ret = DB_DVBS_OK;
    u16 pg_id = 0;
   
    time_get(&g_time_st, FALSE);
    memcpy(&g_time_end, &g_time_st, sizeof(utc_time_t));
    time_up(&g_time_end, 24 * 60 * 60);
    pg_id = sys_status_get_curn_group_curn_prog_id();
    if(INVALIDID == pg_id)
    {
      prog_info->network_id = EPG_IGNORE_ID;
      prog_info->ts_id       = EPG_IGNORE_ID;
      prog_info->svc_id     = EPG_IGNORE_ID;
    }
    ret = db_dvbs_get_pg_by_id(pg_id, &pg);
    if(DB_DVBS_OK == ret)
    {
      prog_info->network_id = (u16)pg.orig_net_id;
      prog_info->ts_id      = (u16)pg.ts_id;
      prog_info->svc_id     = (u16)pg.s_id;
    }
    else
    {
      prog_info->network_id = EPG_IGNORE_ID;
      prog_info->ts_id       = EPG_IGNORE_ID;
      prog_info->svc_id     = EPG_IGNORE_ID;
    }  
    memcpy(&prog_info->start_time, &g_time_st, sizeof(utc_time_t));
    memcpy(&prog_info->end_time, &g_time_end, sizeof(utc_time_t));
 }

void ui_epg_set_program_info(u16 pg_id)
{
    db_dvbs_ret_t ret = DB_DVBS_OK;
    dvbs_prog_node_t pg = {0};
    epg_prog_info_t prog_info = {0};
    utc_time_t g_time_st = {0};
    utc_time_t g_time_end = {0};
    
     time_get(&g_time_st, FALSE);
     memcpy(&g_time_end, &g_time_st, sizeof(utc_time_t));
     time_up(&g_time_end, 24 * 60 * 60);
     if(INVALIDID != pg_id)
    {
      ret = db_dvbs_get_pg_by_id(pg_id, &pg);
      MT_ASSERT(DB_DVBS_OK == ret);
      prog_info.network_id = (u16)pg.orig_net_id;
      prog_info.ts_id      = (u16)pg.ts_id;
      prog_info.svc_id     = (u16)pg.s_id;
    }
    else
    {
      prog_info.network_id = EPG_IGNORE_ID;
      prog_info.ts_id       = EPG_IGNORE_ID;
      prog_info.svc_id     = EPG_IGNORE_ID;
    }
    memcpy(&prog_info.start_time, &g_time_st, sizeof(utc_time_t));
    memcpy(&prog_info.end_time, &g_time_end, sizeof(utc_time_t));
    OS_PRINTF("epg api set filter info: net_id:0x%x,ts_id:0x%x,s_id:0x%x\n",
      prog_info.network_id,prog_info.ts_id,prog_info.svc_id );
    if(g_epg_init)
    {
      mul_epg_set_filter_info(&prog_info);
    }
}

BOOL ui_epg_is_initialized(void)
{
  return g_epg_init;
}

#else
#include "epg_data4.h"
u16 ui_epg_evtmap(u32 event);

static BOOL sg_ap_epg_active_flag = FALSE;
static BOOL sg_ap_epg_start_flag = FALSE;

void ui_epg_set_lang_code(u8 *first_code, u8 *second_code, u8 *default_code)
{
  void * epg_handle = NULL;

  if(NULL == first_code || NULL == second_code || NULL == default_code)
  {
    OS_PRINTF("Epg lang code set error!\n");
    return;
  }
  
  epg_handle = class_get_handle_by_id(EPG_CLASS_ID);

  OS_PRINTF("AT [%s] in Line %d:\n f_code = %s, s_code = %s, d_code = %s\n", 
            __FUNCTION__, __LINE__, first_code, second_code, default_code);

  epg_data_set_lang_code(epg_handle, first_code, second_code, default_code);
}
void ui_epg_init(void)
{
  cmd_t cmd = {0};
  if( TRUE == sg_ap_epg_active_flag )
    return;
  sg_ap_epg_active_flag = TRUE;
  
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_EPG;
  OS_PRINTF("\nACTIVATE EPG\n");
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_register_ap_evtmap(APP_EPG, ui_epg_evtmap);
  fw_register_ap_msghost(APP_EPG, ROOT_ID_VEPG); 
  fw_register_ap_msghost(APP_EPG, ROOT_ID_BACKGROUND); 
}

void ui_epg_release(void)
{
  cmd_t cmd = {0};

  if( FALSE == sg_ap_epg_active_flag )
    return;
  sg_ap_epg_active_flag = FALSE;

  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_EPG;
  
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  
  fw_unregister_ap_evtmap(APP_EPG);
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_VEPG); 
  fw_unregister_ap_msghost(APP_EPG, ROOT_ID_BACKGROUND); 
}

void ui_epg_start(void)
{
  cmd_t cmd = {0};

  if( FALSE == sg_ap_epg_active_flag )
    return; //ap_epg deactived, do not send cmd to it

  if(TRUE == sg_ap_epg_start_flag)
    return; // epg already started
  sg_ap_epg_start_flag = TRUE;
  
  cmd.id = EPG_CMD_START_SCAN;
  cmd.data1 = DMX_INPUT_EXTERN0;
  cmd.data2 = BLOCK_EPG_BUFFER;

  ap_frm_do_command(APP_EPG, &cmd);  
}

void ui_epg_stop(void)
{
  cmd_t cmd = {0};

  if( FALSE == sg_ap_epg_active_flag )
    return; //ap_epg deactived, do not send cmd to it

  if(FALSE == sg_ap_epg_start_flag)
    return; // epg already stopped
  sg_ap_epg_start_flag = FALSE;

  cmd.id = EPG_CMD_STOP_SCAN;
  cmd.data1 = APP_EPG;
  cmd.data2 = BLOCK_EPG_BUFFER;

  ap_frm_do_command(APP_EPG, &cmd);    
}

void ui_set_epg_dy_by_offset(u16 offset)
{
  dvbs_prog_node_t pg;
  u16 curn_group;
  u16 curn_mode;
  u16 pg_id;
  u32 group_context;
  u8 view_id=0;
  u16 total=0;
  epg_dy_policy_t *p_dy_policy = NULL;
  u16 i;
  utc_time_t start_time = {0};
  utc_time_t end_time = {0};
  utc_time_t add_time = {0};

  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, &pg_id, &group_context);
  view_id = ui_dbase_get_pg_view_id();  
  total = db_dvbs_get_count(view_id);
  p_dy_policy = epg_data_get_dy_policy(class_get_handle_by_id(EPG_CLASS_ID));
  if (p_dy_policy == NULL)
  {
    OS_PRINTF("\n##debug: epg_data_get_dy_policy NULL!\n");
    return ;
  }

#if 1
  time_get(&start_time, FALSE);
  start_time.hour = 0;
  start_time.minute = 0;
  start_time.second = 0;
  memcpy(&end_time, &start_time, sizeof(utc_time_t));
  add_time.day = DY_DAY_NUM;
  time_add(&end_time, &add_time);
  time_to_gmt(&start_time, &p_dy_policy->start);
  time_to_gmt(&end_time, &p_dy_policy->end);   
#endif

  
  for (i = 0; i < DY_SVC_NUM ; i++)  //left side num 
  {
    if(offset + i >= total)
    {
      break;
    }
    pg_id = db_dvbs_get_id_by_view_pos(view_id, (offset + i));
    
    //pg_id = db_dvbs_get_id_by_view_pos(view_id,i+pg_pos-offset);
    db_dvbs_get_pg_by_id(pg_id, &pg);  
    p_dy_policy[i].cur_net_id = (u16)pg.orig_net_id;
    p_dy_policy[i].cur_ts_id = (u16)pg.ts_id;
    p_dy_policy[i].cur_svc_id = (u16)pg.s_id;
#if 0
    OS_PRINTF("ui_set_epg_dy_offset: net:%d ts:%d svc:%d\n",  
    p_dy_policy[i].cur_net_id, p_dy_policy[i].cur_ts_id, p_dy_policy[i].cur_svc_id);
#endif
  }
  epg_data_set_dy_policy(class_get_handle_by_id(EPG_CLASS_ID), p_dy_policy);
}

void ui_set_epg_dy(void)
{
  dvbs_prog_node_t pg;
  u16 curn_group;
  u16 curn_mode;
  u16 pg_pos;
  u16 pg_id;
  u32 group_context;
  u8 view_id=0;
  u16 i;
  u16 page;
  u16 total;
  u16 offset;
  u16 mubiao;
  epg_dy_policy_t *p_dy_policy = NULL;
  utc_time_t g_time = {0};
  utc_time_t add_time = {0};
 
  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, 
                                    &pg_id, &group_context);  
  view_id = ui_dbase_get_pg_view_id();  
  pg_pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);
  total = db_dvbs_get_count(view_id);

  page = DY_SVC_NUM;

  if(pg_pos < page)
  {
    offset = pg_pos;
  }
  else if(total-pg_pos <= page)
  {
    offset = pg_pos-(total-page);
  }
  else
  {
    offset = 0;
  }

  p_dy_policy = epg_data_get_dy_policy(class_get_handle_by_id(EPG_CLASS_ID));
  if (p_dy_policy == NULL)
  {
    OS_PRINTF("\n##debug: epg_data_get_dy_policy NULL!\n");
    return ;
  }
  
#if 1
    time_get(&g_time, TRUE);
    memcpy(&p_dy_policy->start, &g_time, sizeof(utc_time_t));
    p_dy_policy->start.hour = 0;
    p_dy_policy->start.minute = 0;
    p_dy_policy->start.second = 0;
    add_time.day = DY_DAY_NUM;    

    memcpy(&p_dy_policy->end, &p_dy_policy->start, sizeof(utc_time_t));  
    time_add(&p_dy_policy->end, &add_time);
#endif

  for (i = 0; i < DY_SVC_NUM ; i++)
  {        
    mubiao = i + pg_pos - offset;
    if(mubiao >= total)
    {
      break;
    }
    pg_id = db_dvbs_get_id_by_view_pos(view_id, mubiao);
    db_dvbs_get_pg_by_id(pg_id, &pg);  
    p_dy_policy[i].cur_net_id = (u16)pg.orig_net_id;
    p_dy_policy[i].cur_ts_id = (u16)pg.ts_id;
    p_dy_policy[i].cur_svc_id = (u16)pg.s_id;
  }
  epg_data_set_dy_policy(class_get_handle_by_id(EPG_CLASS_ID), p_dy_policy);
}

void ui_set_epg_day_offset(u8 offset)
{
  u16 curn_group;
  u16 curn_mode;
  u16 pg_id;
  u32 group_context;
  u8 view_id=0;
  u16 total;
  epg_dy_policy_t *p_dy_policy = NULL;
  utc_time_t g_time = {0};
  utc_time_t add_time = {0};
  //cmd_t cmd = {0};
  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode, 
                                    (u16 *)&pg_id, (u32 *)&group_context);
  view_id = ui_dbase_get_pg_view_id();  
  total = db_dvbs_get_count(view_id);

  p_dy_policy = epg_data_get_dy_policy(class_get_handle_by_id(EPG_CLASS_ID));
  if (p_dy_policy == NULL)
  {
    OS_PRINTF("!!!!!!!!!!p_dy_policy = NULL\n");
    return ;

  }

  time_get(&g_time, TRUE);
  add_time.day = offset;
  time_add(&g_time, &add_time);
  memcpy(&p_dy_policy->start, &g_time, sizeof(utc_time_t));    
  add_time.day = 7;
  time_add(&g_time, &add_time);
  memcpy(&p_dy_policy->end, &g_time, sizeof(utc_time_t));  
  epg_data_set_dy_policy(class_get_handle_by_id(EPG_CLASS_ID), p_dy_policy);  
}

void ui_set_epg_policy_period(utc_time_t *p_start, u32 hour_num)
{
  epg_dy_policy_t *p_dy_policy = NULL;
  utc_time_t add_time = {0};
  utc_time_t end_time = {0};

  p_dy_policy = epg_data_get_dy_policy(class_get_handle_by_id(EPG_CLASS_ID));
  if (p_dy_policy == NULL)
  {
    OS_PRINTF("!!!!!!!!!!p_dy_policy = NULL\n");
    return ;
  }
  add_time.hour = hour_num;
  memcpy(&end_time, p_start, sizeof(utc_time_t));
  time_add(&end_time, &add_time);
    
  memcpy(&p_dy_policy->start, p_start, sizeof(utc_time_t));
  memcpy(&p_dy_policy->end, &end_time, sizeof(utc_time_t));  
  epg_data_set_dy_policy(class_get_handle_by_id(EPG_CLASS_ID), p_dy_policy);  
}



u8 ui_get_current_parental_rating(void)
{
  dvbs_prog_node_t pg = {0};
  u16 curn_group = 0;
  u16 curn_mode = 0;
  u16 pg_id = 0;
  u32 group_context = 0;
  epg_filter_t filter = {0};
  evt_node_t *p_evt_node = NULL;  

  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode,
                                    &pg_id, &group_context);
  db_dvbs_get_pg_by_id(pg_id, &pg);
  filter.orig_network_id = pg.orig_net_id;
  filter.stream_id = pg.ts_id;
  filter.service_id = pg.s_id;
  p_evt_node = epg_data_get_pf(class_get_handle_by_id(EPG_CLASS_ID),
                                &filter);
  if(p_evt_node == NULL)
  {
    return INVALID_PARENTAL_RATING;
  }
#if 0
  time_get(&cur_time, TRUE);
  memcpy(&end_time, &p_evt_node->start_time, sizeof(utc_time_t));
  time_add(&end_time, &p_evt_node->drt_time);
  if(time_cmp(&cur_time, &p_evt_node->start_time, FALSE) < 0
    || time_cmp(&cur_time, &end_time, FALSE) > 0)
  {
    return INVALID_PARENTAL_RATING;
  }
#endif

  return p_evt_node->parental_rating;
}
#endif

BEGIN_AP_EVTMAP(ui_epg_evtmap)
#ifndef IMPL_NEW_EPG
  CONVERT_EVENT(EPG_EVT_EVT_DELETED,    MSG_EPG_READY)
  CONVERT_EVENT(EPG_EVT_TAB_READY,      MSG_EPG_READY)
  CONVERT_EVENT(EPG_EVT_EVT_READY,      MSG_EPG_READY)  
  CONVERT_EVENT(EPG_EVT_PF_READY,       MSG_PF_READY) 
  CONVERT_EVENT(EPG_EVT_NEW_SEC_FOUND,    MSG_EPG_READY)
  CONVERT_EVENT(EPG_EVT_STOP,           MSG_STOP_EPG)
  CONVERT_EVENT(EPG_EVT_NEW_PF_VER_FOUND, MSG_PF_CHANGE)
  #else
  CONVERT_EVENT(EPG_API_SCHE_EVENT, MSG_SCHE_READY)    
  CONVERT_EVENT(EPG_API_PF_EVENT,   MSG_PF_READY)
  CONVERT_EVENT(EPG_API_DEL_EVENT,  MSG_EPG_DEL)
  CONVERT_EVENT(EPG_API_PF_SECTION_FINISH,  MSG_EPG_CHANG_TABLE_POLICY)
  CONVERT_EVENT(EPG_API_PF_SECTION_TIMEOUT,  MSG_EPG_CHANG_TABLE_POLICY)
  #endif
END_AP_EVTMAP(ui_epg_evtmap)


