/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_signal.h"

static BOOL g_is_lock = TRUE;
static struct signal_data g_sign_data = {0, 0,0, TRUE};
static dvbc_lock_info_t g_tuner_param = {0};
static dvbt_lock_info_t g_dvbt_param = {0};

u16 ui_signal_evtmap(u32 event);

void ui_init_signal(void)
{
  cmd_t cmd = {0};

  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_SIGNAL_MONITOR;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  g_tuner_param.tp_freq = 4125;
  g_tuner_param.tp_sym = 1000;
  g_tuner_param.nim_modulate = NIM_MODULA_AUTO;

  fw_register_ap_evtmap(APP_SIGNAL_MONITOR, ui_signal_evtmap);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_BACKGROUND);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_TS_RECORD);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_MAINMENU);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_MANUAL_SEARCH);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_AUTO_SEARCH);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_RANGE_SEARCH);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_XSEARCH);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_XSYS_SET);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_XSMALL_INFO);
  fw_register_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_OTA);
}

/*
void ui_enable_signal_monitor(BOOL is_enable)
{
  cmd_t cmd = {0};
  
  cmd.id = SIG_MON_MONITOR_PERF;
  cmd.data1 = is_enable;
  //cmd.data2 = sat.tuner_index;
  ap_frm_do_command(APP_SIGNAL_MONITOR, &cmd);
}*/

void ui_enable_signal_monitor_by_tuner(BOOL is_enable,u8 tuner_index)
{
  cmd_t cmd = {0};
  
  cmd.id = SIG_MON_MONITOR_PERF;
  cmd.data1 = is_enable;
  cmd.data2 = tuner_index;
  ap_frm_do_command(APP_SIGNAL_MONITOR, &cmd);
}

void ui_release_signal(void)
{
  cmd_t cmd = {0};
  
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_SIGNAL_MONITOR;
  
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_AUTO_SEARCH);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_MANUAL_SEARCH);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_MAINMENU);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_BACKGROUND);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_RANGE_SEARCH);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_TS_RECORD);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_XSEARCH);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_XSYS_SET);
  fw_unregister_ap_msghost(APP_SIGNAL_MONITOR, ROOT_ID_XSMALL_INFO);
  fw_unregister_ap_evtmap(APP_SIGNAL_MONITOR);
}


void ui_set_transpond(dvbs_tp_node_t *p_tp)
{
  cmd_t cmd = {0};

  trans_lock_tp_info(&g_tuner_param, p_tp);

  cmd.id = SIG_MON_LOCK_TP;
  cmd.data1 = (u32)(&g_tuner_param);
  cmd.data2 = SYS_DVBC;

  ap_frm_do_command(APP_SIGNAL_MONITOR, &cmd);

  UI_PRINTF("----------------------------------\n");
  UI_PRINTF("- set transpond id = %d\n", p_tp->id);
  UI_PRINTF("---freq = %d\n", p_tp->freq);
  UI_PRINTF("---symbol = %d\n", p_tp->sym);
  UI_PRINTF("---modulate = %d\n", p_tp->nim_modulate);
  UI_PRINTF("----------------------------------\n");
}

//T2
void ui_set_antenna_and_transpond(sat_node_t *p_sat, dvbs_tp_node_t *p_tp)
{
  cmd_t cmd = {0};
  u32  sys_mode = 0;

  if(nim_get_tuner_count() > 1)
  {
    sys_mode =  (TUNER1 << 16) | SYS_DTMB;
  }
  else
  {
     sys_mode =  (TUNER0 << 16) | SYS_DTMB;
  }

  g_dvbt_param.tp_freq = p_tp->freq;
  g_dvbt_param.band_width = p_tp->sym;
  g_dvbt_param.nim_type = p_tp->nim_type;

  cmd.id = SIG_MON_LOCK_TP;
  cmd.data1 = (u32)(&g_dvbt_param);
  cmd.data2 = (u32)sys_mode;

  ap_frm_do_command(APP_SIGNAL_MONITOR, &cmd);

  UI_PRINTF("--------------ui_set_antenna_and_transpond--------------------\n");
  UI_PRINTF("---nim type = %d\n", g_dvbt_param.nim_type);
  UI_PRINTF("---freq = %d\n", p_tp->freq);
  UI_PRINTF("---symbol = %d\n", p_tp->sym);
  UI_PRINTF("----------------------------------\n");

}


BOOL ui_signal_is_lock(void)
{
  return g_is_lock;
}

void ui_signal_set_lock(BOOL is_lock)
{
  g_is_lock = is_lock;
}

void ui_signal_check(u32 para1, u32 para2)
{
  menu_attr_t *p_attr;
  control_t *p_root;

  u8 curn_mode;
  sig_mon_info_t *signal = (sig_mon_info_t *)para1;

  g_sign_data.intensity = signal->strength;
  g_sign_data.quality = signal->snr;
  g_sign_data.ber = signal->ber;
  if((((para2 >> 8) & 0XFF) == 1) || ((para2 & 0X00FF) == 1))
  {
    g_sign_data.lock = TRUE;    
  }
  else
  {
    g_sign_data.lock = FALSE;        
  }

  //OS_PRINTF("intensity %d, quality %d\n", g_sign_data.intensity, g_sign_data.quality);
  p_attr = manage_get_curn_menu_attr();
  p_root = fw_find_root_by_id(p_attr->root_id);

  curn_mode = sys_status_get_curn_prog_mode();

  //OS_PRINTF("signal check g_sign_data.lock[%d], g_is_lock[%d]\n", g_sign_data.lock, g_is_lock);
  if(g_sign_data.lock != g_is_lock)
  {
    g_is_lock = g_sign_data.lock;

    //show black screen when no signal in TV mode
    if((!g_is_lock) && (curn_mode == CURN_MODE_TV) && ui_is_playing())
    {
      void *p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
      
      disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);
      disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_HD, FALSE);
    }

    update_signal();
    if (p_attr->signal_msg == SM_LOCK)
    {
      if (p_root != NULL)
      {
        ctrl_process_msg(p_root, 
          (u16)(g_is_lock ? MSG_SIGNAL_LOCK : MSG_SIGNAL_UNLOCK),
          0, 0);
      }
    }
  }

  if (p_attr->signal_msg == SM_BAR)
  {
    if (p_root != NULL)
    {
        ctrl_process_msg(p_root, MSG_SIGNAL_UPDATE,
              (u32) & g_sign_data, 0);
    }
  }
}

BEGIN_AP_EVTMAP(ui_signal_evtmap)
CONVERT_EVENT(SIG_MON_SIGNAL_INFO, MSG_SIGNAL_CHECK)
END_AP_EVTMAP(ui_signal_evtmap)



