/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

// param package
struct
{
  u8 type;
  BOOL is_paused;
  scan_input_param_t *p_param;
}g_scan_param = {0, FALSE, NULL};

u16 ui_scan_evtmap(u32 event);

void ui_scan_param_init(void)
{
#if 1
  // use common block
  ap_scan_set_attach_block(TRUE, BLOCK_AV_BUFFER);

  g_scan_param.p_param = ap_scan_param_buffer();
  MT_ASSERT(g_scan_param.p_param != NULL);

  memset(g_scan_param.p_param, 0, sizeof(scan_input_param_t));
#endif
}

void ui_scan_param_set_type(u8 type, u8 chan_type, u8 free_only, nit_scan_type_t nit_type)
{
  scan_input_param_t *p_param = g_scan_param.p_param;
  
  MT_ASSERT(p_param != NULL);
  
  g_scan_param.type = type;
  
  p_param->scan_data.bat_scan_type = BAT_SCAN_ALL_TP;
  p_param->scan_data.scramble_scan_origin = SCAN_SCRAMBLE_FROM_PMT;
  p_param->scan_data.nit_scan_type = nit_type;
  p_param->chan_type = chan_type;
  p_param->is_free_only = free_only;
  //p_param->scan_data.scramble_scan_origin = scram_org;

}


u8 ui_scan_param_get_type(void)
{
  return g_scan_param.type;
}

nit_scan_type_t ui_scan_param_nit_type(void)
{
  return g_scan_param.p_param->scan_data.nit_scan_type;
}

u16 ui_scan_param_get_sat_num(void)
{
  return g_scan_param.p_param->scan_data.total_sat;
}

BOOL ui_scan_param_add_sat(sat_node_t *p_sat_info)
{
  scan_preset_data_t *p_scan_buf = &g_scan_param.p_param->scan_data;
  u16 cur_sat_num = p_scan_buf->total_sat;
  scan_sat_info_t *p_buf_sat = &p_scan_buf->sat_list[cur_sat_num];

  MT_ASSERT(p_sat_info != NULL);
  if(p_scan_buf->total_sat >= MAX_SAT_NUM_SUPPORTED)
  {
    return FALSE;
  }

  p_buf_sat->sat_id = p_sat_info->id;
//  trans_diseqc_info(&p_buf_sat->diseqc_info, p_sat_info);
  trans_sat_info(&p_buf_sat->sat_info, p_sat_info);
  p_buf_sat->tuner_id = p_sat_info->tuner_index;
  memcpy(p_buf_sat->name, p_sat_info->name, sizeof(p_sat_info->name));
  
  //Update current satellite information
  p_scan_buf->total_sat++;
  return TRUE;
}

BOOL ui_scan_param_add_tp(dvbs_tp_node_t *p_tp_info)
{
  scan_preset_data_t *p_scan_buf = &g_scan_param.p_param->scan_data;
  u16 cur_tp_num = p_scan_buf->total_tp;
  scan_tp_info_i_t *p_buf_tp = &p_scan_buf->tp_list[cur_tp_num];

  MT_ASSERT(p_scan_buf != NULL);
  if(p_scan_buf->total_tp >= MAX_TP_NUM_SUPPORTED)
  {
    return FALSE;
  }

  trans_tp_info(&p_buf_tp->tp_info, p_tp_info);
  //Set tp info
  p_buf_tp->id = p_tp_info->id;
  p_buf_tp->sat_id = p_tp_info->sat_id;

  //Update current satellite information
  p_scan_buf->total_tp++;
  return TRUE;
}

void ui_scan_param_set_pid(u16 v_pid, u16 a_pid, u16 pcr_pid)
{
  scan_input_param_t *p_param = g_scan_param.p_param;
  MT_ASSERT(p_param != NULL);

  p_param->pid_scan_enable = TRUE;
  
  p_param->pid_parm.video_pid = v_pid;
  p_param->pid_parm.audio_pid = a_pid;
  p_param->pid_parm.pcr_pid = pcr_pid;
}

//only one satellite when dvbt2
u16 ui_scan_add_dvbt_sat(BOOL isRebuild, sat_node_t *p_sat_node)
{
  u8 sat_view_id = 0;
  u16 i = 0, cnt = 0, sat_id = 0;
  sat_node_t sat_node = {0};
  db_dvbs_ret_t ret = DB_DVBS_OK;
  BOOL t2_sat_exist = 0;

  if(p_sat_node == NULL)
    p_sat_node = &sat_node;
  
  sat_view_id = ui_dbase_create_view(DB_DVBS_T2_SAT, 0, NULL);
  cnt = db_dvbs_get_count(sat_view_id);
  for(i = 0; i < cnt; i++)
  {
       sat_id = ui_dbase_get_sat_by_pos(sat_view_id, &sat_node, i);
       if(sat_node.scan_mode == DVBT_SCAN)
       {
           t2_sat_exist = 1;
           break;
       }
  }
  
  if(t2_sat_exist)
  {
      memcpy(p_sat_node, &sat_node, sizeof(sat_node_t));
      db_dvbs_destroy_view(sat_view_id);
      return (u16)sat_node.id;
  }
  /* add sat */
  memset(p_sat_node, 0, sizeof(sat_node_t));
  p_sat_node->id = INVALIDID;
  p_sat_node->lnb_high = p_sat_node->lnb_low = 5150;
  p_sat_node->longitude = 0;
  p_sat_node->scan_mode = DVBT_SCAN;
  p_sat_node->tuner_index = TUNER0;
  
  str_nasc2uni("DTMB", p_sat_node->name, DB_DVBS_MAX_NAME_LENGTH);
  ret = db_dvbs_add_satellite(sat_view_id, p_sat_node);

  switch(ret)
  {
    case DB_DVBS_OK:
      UI_PRINTF("ui_antenna_set: add sat is OK!\n");
      db_dvbs_save(sat_view_id);
      db_dvbs_destroy_view(sat_view_id);
      break;

    default:
      UI_PRINTF("ui_antenna_set: add sat is FAILED!\n");
      MT_ASSERT(0);
      return INVALIDID;
  }

  return (u16)p_sat_node->id;
}

u16 ui_scan_add_dvbc_sat(sat_node_t *p_sat_node)
{
  u8 sat_view_id = 0;
  u16 i = 0, cnt = 0, sat_id = 0;
  sat_node_t sat_node = {0};
  db_dvbs_ret_t ret = DB_DVBS_OK;
  BOOL cable_sat_exist = 0;

  if(p_sat_node == NULL)
    p_sat_node = &sat_node;
  
  sat_view_id = ui_dbase_create_view(DB_DVBS_CABLE_SAT, 0, NULL);
  cnt = db_dvbs_get_count(sat_view_id);
  for(i = 0; i < cnt; i++)
  {
       sat_id = ui_dbase_get_sat_by_pos(sat_view_id, &sat_node, i);

       if(sat_node.scan_mode == DVBC_SCAN)
       {
           cable_sat_exist = 1;
           break;
       }
  }

  if(cable_sat_exist)
  {
      p_sat_node->id = (u16)sat_node.id;
      str_nasc2uni("Cable", p_sat_node->name, DB_DVBS_MAX_NAME_LENGTH);
      return (u16)sat_node.id;
  }
  /* add sat */
  memset(p_sat_node, 0, sizeof(sat_node_t));
  p_sat_node->id = INVALIDID;
  p_sat_node->scan_mode = DVBC_SCAN;
  p_sat_node->tuner_index = TUNER0;
  
  str_nasc2uni("Cable", p_sat_node->name, DB_DVBS_MAX_NAME_LENGTH);

  ret = db_dvbs_add_satellite(sat_view_id, p_sat_node);

  switch(ret)
  {
    case DB_DVBS_OK:
      UI_PRINTF("ui_scan_add_dvbc_sat: add sat is OK!\n");
      db_dvbs_save(sat_view_id);
      db_dvbs_destroy_view(sat_view_id);
      break;

    default:
      UI_PRINTF("ui_scan_add_dvbc_sat: add sat is FAILED!\n");
      MT_ASSERT(0);
      return INVALIDID;
  }
  return (u16)p_sat_node->id;
}

BOOL ui_scan_param_set_dvbc_autosearch()
{
  scan_preset_data_t *p_pst = &g_scan_param.p_param->scan_data;
  p_pst->sat_list[0].sat_id = ui_scan_add_dvbc_sat(NULL);
  return TRUE;
}

void ui_init_scan(void)
{
#if 1
  cmd_t cmd = {0};
  
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_SCAN;
  
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_register_ap_evtmap(APP_SCAN, ui_scan_evtmap);
  fw_register_ap_msghost(APP_SCAN, ROOT_ID_DO_SEARCH);
  fw_register_ap_msghost(APP_SCAN, ROOT_ID_FACTORY_TEST);
#endif
}


void ui_release_scan(void)
{
#if 1
 cmd_t cmd = {0};
  
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_SCAN;
  
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  fw_unregister_ap_msghost(APP_SCAN, ROOT_ID_DO_SEARCH);
  fw_unregister_ap_msghost(APP_SCAN, ROOT_ID_FACTORY_TEST);
  fw_unregister_ap_evtmap(APP_SCAN);
#endif
}

BOOL ui_start_scan(void)
{
#if 1
  cmd_t cmd = {SCAN_CMD_START_SCAN};
  class_handle_t nc_handle = class_get_handle_by_id(NC_CLASS_ID);    
  switch(g_scan_param.type)
  {
    case USC_BLIND_SCAN:
      g_scan_param.p_param->scan_mode = BLIND_SCAN;
      nc_set_lock_mode(nc_handle, TUNER0, SYS_DVBS);
      break;
    case USC_PID_SCAN:
    case USC_TP_SCAN:
    case USC_PRESET_SCAN:
      g_scan_param.p_param->scan_mode = TP_SCAN;
      nc_set_lock_mode(nc_handle, TUNER0, SYS_DVBS);
      break;
    case USC_DVBC_FULL_SCAN:
    case USC_DVBC_MANUAL_SCAN:
      g_scan_param.p_param->scan_mode = DVBC_SCAN;
      nc_set_lock_mode(nc_handle, TUNER0, SYS_DVBC);
      break;
    case USC_DTMB_SCAN:
      g_scan_param.p_param->scan_mode = DTMB_SCAN;
      nc_set_lock_mode(nc_handle, TUNER0, SYS_DTMB);
      break;
    default:
      MT_ASSERT(0);
      return FALSE;
  }

  cmd.data1 = (u32)ap_scan_param_apply();
  cmd.data2 = 1;
  ap_frm_do_command(APP_SCAN, &cmd);

  g_scan_param.is_paused = FALSE;
#endif
 
  return TRUE;
}

void ui_stop_scan(void)
{
#if 1
 cmd_t cmd = {SCAN_CMD_CANCEL_SCAN};

  if(g_scan_param.is_paused)
  {
    ui_resume_scan();
  }

  ap_frm_do_command(APP_SCAN, &cmd);
#endif
}


void ui_pause_scan(void)
{
#if 1
 cmd_t cmd;

  if(g_scan_param.is_paused)
  {
    OS_PRINTF("ui_pause_scan: already paused, ERROR!\n");
    return;
  }

  cmd.id = SCAN_CMD_PAUSE_SCAN;
  cmd.data1 = 0;
  cmd.data2 = 0;  
  ap_frm_do_command(APP_SCAN, &cmd);

  cmd.id = AP_FRM_CMD_PAUSE_APP;
  cmd.data1 = APP_SCAN;
  cmd.data2 = 0;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  g_scan_param.is_paused = TRUE;
#endif
}



void ui_resume_scan(void)
{
#if 1
 cmd_t cmd;

  if(!g_scan_param.is_paused)
  {
    OS_PRINTF("ui_resume_scan: not paused, ERROR!\n");
    return;
  }

  cmd.id = AP_FRM_CMD_RESUME_APP;
  cmd.data1 = APP_SCAN;
  cmd.data2 = 0;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  cmd.id = SCAN_CMD_RESUME_SCAN;
  cmd.data1 = 0;
  cmd.data2 = 0;  
  ap_frm_do_command(APP_SCAN, &cmd);

  g_scan_param.is_paused = FALSE;
#endif
}

BEGIN_AP_EVTMAP(ui_scan_evtmap)
  CONVERT_EVENT(SCAN_EVT_PG_FOUND,  MSG_SCAN_PG_FOUND)
  CONVERT_EVENT(SCAN_EVT_TP_FOUND,  MSG_SCAN_TP_FOUND)
  CONVERT_EVENT(SCAN_EVT_PROGRESS,  MSG_SCAN_PROGRESS)
  CONVERT_EVENT(SCAN_EVT_SAT_SWITCH, MSG_SCAN_SAT_FOUND)
  CONVERT_EVENT(SCAN_EVT_NO_MEMORY,   MSG_SCAN_DB_FULL)
  CONVERT_EVENT(SCAN_EVT_FINISHED,  MSG_SCAN_FINISHED)
  CONVERT_EVENT(SCAN_EVT_NIT_FOUND,  MSG_SCAN_NIT_FOUND)
END_AP_EVTMAP(ui_scan_evtmap);


