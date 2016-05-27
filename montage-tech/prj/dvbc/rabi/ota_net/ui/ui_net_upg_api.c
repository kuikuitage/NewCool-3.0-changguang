/****************************************************************************

 ****************************************************************************/


#include "ui_common.h"
#include "ui_net_upg_api.h"
#include "commonData.h"


extern u8 *ui_get_customer(void);

static net_upg_api_info_t s_net_upg_get;
static BOOL s_net_upg_started = FALSE;

u16 net_upg_evtmap(u32 event);

static void net_upg_callback(net_upg_api_evt_t event, void *p_data, u16 context)
{
  event_t evt = {0};
  
  switch (event)
  {
  case NET_UPG_EVT_CHECK_DONE:
    
    evt.id = UI_NET_UPG_EVT_CHECK_DONE;
    evt.data1 = (u32)context;
    evt.data2 = (u32)p_data;
    OS_PRINTF("\n##net_upg_callback NET_UPG_EVT_CHECK_DONE[%d]!\n",
      ((net_upg_api_info_t *)p_data)->item_cnt);
    memcpy(&s_net_upg_get, (void *)p_data, sizeof(net_upg_api_info_t));
    break;
  case NET_UPG_EVT_PROGRESS:
    {
      static u32 s_progress;
      if( s_progress == *((u32 *)p_data) )
        return;
      s_progress = *((u32 *)p_data);
    if (context == 0)
    {
      evt.id = UI_NET_UPG_EVT_DOWN_PROGRESS;
    }
    else
    {
      evt.id = UI_NET_UPG_EVT_BURN_PROGRESS;
    }
    
    evt.data1 = (u32)context;
    evt.data2 = *((u32 *)p_data);
    }
    break;
   case NET_UPG_EVT_FAIL:
    evt.id = UI_NET_UPG_EVT_FAIL;
    evt.data1 = 0;
    evt.data2 = 0;
    OS_PRINTF("\n##net_upg_callback NET_UPG_EVT_FAIL!\n");
    break;
  default:
    break;
  }
  ap_frm_send_evt_to_ui(APP_USB_UPG, &evt);
}

net_upg_api_info_t *ui_net_upg_get_upg_info(void)
{
  return (net_upg_api_info_t *)&s_net_upg_get;
}

RET_CODE ui_net_upg_init(BOOL b_from_bg)
{
  mul_net_upg_attr_t net_upg_attr;

  if (!s_net_upg_started)
  {
    memset(&s_net_upg_get, 0, sizeof(mul_net_upg_attr_t));
    memset(&net_upg_attr, 0, sizeof(mul_net_upg_attr_t));

    net_upg_attr.stk_size = (32 * KBYTES);
    net_upg_attr.cb = (net_upg_call_back)net_upg_callback;

    fw_register_ap_evtmap(APP_USB_UPG, net_upg_evtmap);
    if(TRUE == b_from_bg)
      fw_register_ap_msghost(APP_USB_UPG, ROOT_ID_BACKGROUND); 
    else
      fw_register_ap_msghost(APP_USB_UPG, ROOT_ID_NETWORK_UPGRADE); 
    s_net_upg_started = TRUE;
    return mul_net_upg_chn_create(&net_upg_attr);
  }
  else
  {
    return SUCCESS;
  }
}


#define UPG_POST_DATA_FMT \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\
<request method=\"getUpdateUrl\">\n\
<parameters>\n\
<mac>%s</mac>\n\
<ID>%llu</ID>\n\
</parameters>\n\
</request>\n"

RET_CODE ui_net_upg_start_check(protocol_t pt, u8 post_flag, u8 *p_addr, u8* p_xml, u8 *p_usr_asc, u8 *p_pwd_asc)
{
  net_upg_config_t cfg;

  if (!s_net_upg_started)
  {
    return ERR_FAILURE;
  }
  memset(&cfg, 0, sizeof(net_upg_config_t));

  cfg.customer_id = sys_status_get_customer_id();
  cfg.serial_num = sys_status_get_serial_num();
  cfg.hw = sys_status_get_hw_ver();

  cfg.sw = sys_status_get_sw_version();
  cfg.type = pt;
  strcpy(cfg.customer, CUSTOMER_STRING);
  strcpy(cfg.user_name, p_usr_asc);
  strcpy(cfg.pwd, p_pwd_asc);
  strcpy(cfg.ftp, p_addr);
  strcpy(cfg.xml, p_xml);
  #if 0
  cfg.post_check = post_flag;
  if(cfg.post_check)
  {
    u32 low, high;
    u64 hight_64;
    u64 chip_id;
    u8   str_mac[32];
    u8 i;
#ifndef WIN32
    hal_get_chip_unique_numb(&low, &high);
#endif
    hight_64 = high;
    chip_id = low | (hight_64<<32);
    memset(str_mac, 0, sizeof(str_mac));
    for(i = 0; i < 6; i++)
    {
      u8 c;
      c = sys_status_get_mac_by_index(i);
      sprintf(str_mac+i*2, "%02x", c);
    }
    cfg.post_size= sprintf(cfg.post_data, UPG_POST_DATA_FMT, str_mac,chip_id);
    mtos_printk("post_data:\n%s\n", cfg.post_data);
  }
  else
  {
    ;
  }
	#endif
  //OS_PRINTK("\n##mul_net_upg_start_check [%d][%s][%s, %s]\n", pt, cfg.ftp, cfg.user_name, cfg.pwd);
  mul_net_upg_start_check(&cfg);
  return SUCCESS;
}

RET_CODE ui_net_upg_upgrade(net_upg_api_up_cfg_t *p_cfg)
{
  if (!s_net_upg_started)
  {
    return ERR_FAILURE;
  }
  OS_PRINTF("\n##ui_net_upg_upgrade sw[%lu]\n", p_cfg->sw);
  return mul_net_upg_start_upgrade(p_cfg);
}

RET_CODE ui_net_upg_burn(void)
{
  if (!s_net_upg_started)
  {
    return ERR_FAILURE;
  }
  return mul_net_upg_start_burn_flash();
}

RET_CODE ui_net_upg_stop(void)
{
  OS_PRINTF("\n##ui_net_upg_stop !\n");
  if (s_net_upg_started)
  {
    s_net_upg_started = FALSE;
    fw_unregister_ap_evtmap(APP_USB_UPG);
    fw_unregister_ap_msghost(APP_USB_UPG, ROOT_ID_NETWORK_UPGRADE);
    fw_unregister_ap_msghost(APP_USB_UPG, ROOT_ID_BACKGROUND);
   mul_net_upg_stop();
    mul_net_upg_chn_destroy(0);
  }
  return SUCCESS;
}


static u8* p_sg_buf = NULL;

static int upgrade_url_event_call_back(UPGRADE_INFO_EVENT_E event, u32 msg)
{
  OS_PRINTF("[%s] event=%d,msg = %d\n",__func__,event,msg);
  control_t* p_ctrl;                     
  switch (event) 
  {
    case UPGRADE_INFO_PARSER_SUCCESS:
	if((p_ctrl = fw_find_root_by_id(ROOT_ID_NETWORK_UPGRADE)))
         fw_notify_root(p_ctrl, NOTIFY_T_MSG, FALSE, MSG_UPG_URL_INFO, msg, 0);
       else if((p_ctrl = fw_find_root_by_id(ROOT_ID_BACKGROUND)))
         fw_notify_root(p_ctrl, NOTIFY_T_MSG, FALSE, MSG_UPG_URL_INFO, msg, 0);
    break;
    case UPGRADE_INFO_PARSER_FAIL:
      OS_PRINTF("[%s] parser fail ! msg = %d\n",__func__,msg);
    break;
    case UPGRADE_INFO_PARSERING:
      OS_PRINTF("[%s] parser ing  ! msg = %d\n",__func__,msg);
     break;
   case UPGRADE_INFO_PARSER_INVALID:
     OS_PRINTF("[%s] parser UPGRADE_INFO_PARSER_INVALID ! msg = %d\n",__func__,msg);
     break;   
   default:
     OS_PRINTF("[%s] enter default ! msg = %d\n",__func__,msg);
    break;
  }
  mtos_free(p_sg_buf);
  return 0;
}


void get_upgrade_url(void)
{
  UPGRADE_INFO_TASK_CONFIG_T task_param ; 

  memset(&task_param, 0x00, sizeof(UPGRADE_INFO_TASK_CONFIG_T));
                
  task_param.priority= AP_OTA_TASK_PRIORITY;
  task_param.http_priority= NW_DOWNLOAD_PRIO;
  task_param.stack_size = 512*1024;
  p_sg_buf = (char *)mtos_malloc(task_param.stack_size);
  MT_ASSERT(p_sg_buf != NULL);
  memset(p_sg_buf, 0, task_param.stack_size);
  task_param.p_mem_start = p_sg_buf;
                
  task_param.dp = BSW_IPTV_DP;
  task_param.sub_dp= SUB_DP_ID;
  task_param.mac=NULL;
  task_param.id=NULL;

  task_param.proc_cmd_callback = upgrade_url_event_call_back;
#ifndef WIN32               
  get_upgrade_url_hdl(&task_param);
#endif
}


BEGIN_AP_EVTMAP(net_upg_evtmap)
  CONVERT_EVENT(UI_NET_UPG_EVT_CHECK_DONE, MSG_NET_UPG_EVT_CHECK_DONE)
  CONVERT_EVENT(UI_NET_UPG_EVT_DOWN_PROGRESS, MSG_NET_UPG_EVT_DOWN_PROGRESS)
  CONVERT_EVENT(UI_NET_UPG_EVT_BURN_PROGRESS, MSG_NET_UPG_EVT_BURN_PROGRESS)
  CONVERT_EVENT(UI_NET_UPG_EVT_FAIL, MSG_NET_UPG_EVT_FAIL)
END_AP_EVTMAP(net_upg_evtmap)
