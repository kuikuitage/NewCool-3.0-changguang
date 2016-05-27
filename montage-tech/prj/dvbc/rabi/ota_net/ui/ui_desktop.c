/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "pti.h"
#include "dvb_svc.h"
#include "ap_ota.h"

#include "ethernet.h"
#include "pnp_service.h"
#include "net_svc.h"
#include "ui_net_upg_api.h"

#include "commonData.h"
#include "download_api.h"
#include "hotplug.h"
#include "fcrc.h"

ip_address_t primarydns_addr = {0,};
ip_address_t alternatedns_addr = {0,};


static ethernet_device_t* p_wifi_devv = NULL;
static wifi_info_t p_wifi_info = {{0},{0}};
static ethernet_cfg_t   wifi_ethcfg = {0}; 

static net_conn_stats_t g_net_connt_stats = {FALSE, FALSE, FALSE, FALSE};


extern void on_ping_wifi_ok(void);
extern void on_wifi_list_ext_update(void);
extern void paint_wifi_status(BOOL is_conn, BOOL is_paint);


net_conn_stats_t ui_get_net_connect_status(void)
{
  return g_net_connt_stats;
}
void ui_set_net_connect_status( net_conn_stats_t net_conn_stats)
{
  g_net_connt_stats = net_conn_stats;
}


RET_CODE ui_desktop_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 pnp_evtmap(u32 event);


void ui_desktop_init(void)
{
	fw_config_t g_desktop_config =
	{
	  { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT },

	  DST_IDLE_TMOUT,
	  RECEIVE_MSG_TMOUT,
	  POST_MSG_TMOUT,

	  ROOT_ID_BACKGROUND,
	  MAX_ROOT_CONT_CNT,
	  MAX_MESSAGE_CNT,
	  MAX_HOST_CNT,
	  MAX_TMR_CNT,
	  RSI_TRANSPARENT,
	};
	
  fw_init(&g_desktop_config,
               NULL,
               ui_desktop_proc,
               ui_menu_manage);

  fw_register_ap_evtmap(APP_FRAMEWORK, pnp_evtmap);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_BACKGROUND);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_SUBMENU);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_WIFI);
  fw_register_ap_msghost(APP_FRAMEWORK, ROOT_ID_WIFI_LINK_INFO);


}

void ui_desktop_main(void)
{
  fw_default_mainwin_loop(ROOT_ID_BACKGROUND);
}



static void init_ethernet_ethcfg(ethernet_cfg_t *ethcfg)
{
  u8 i;
  //mac address
  ethcfg->hwaddr[0] = sys_status_get_mac_by_index(0);
  ethcfg->hwaddr[1] = sys_status_get_mac_by_index(1);
  ethcfg->hwaddr[2] = sys_status_get_mac_by_index(2);
  ethcfg->hwaddr[3] = sys_status_get_mac_by_index(3);
  ethcfg->hwaddr[4] = sys_status_get_mac_by_index(4);
  ethcfg->hwaddr[5] = sys_status_get_mac_by_index(5);

  for(i=0;i<6;i++)
  {
    OS_PRINTF("%s ethcfg->hwaddr[%d]==%d\n",__FUNCTION__,i,(u8)(ethcfg->hwaddr[i]));
  }

  ethcfg->tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
  ethcfg->is_enabledhcp = 1;
}

extern void * ui_get_net_svc_instance(void);

void do_cmd_connect_network(ethernet_cfg_t *ethcfg, ethernet_device_t * eth_dev)
{
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;

  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  memset(&net_svc_para, 0 , sizeof(net_svc_para));
  net_svc_para.p_eth_dev = eth_dev;
  memcpy(&net_svc_para.net_cfg, ethcfg, sizeof(ethernet_cfg_t));
  p_server = (service_t *)ui_get_net_svc_instance();
  OS_PRINTF("%s(), %d\n", __FUNCTION__, __LINE__);
  p_server->do_cmd(p_server, NET_DO_CONFIG_TCPIP, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
}


/**************************************
    auto connect wifi
    
 **************************************/
void init_desktop_wifi_ethcfg()
{
  ip_address_set_t ss_ip = {{0}};
  net_config_t wifi_config = {0};

  sys_status_get_net_config_info(&wifi_config);
  wifi_ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
  if(wifi_config.config_mode == DHCP)
  {
    wifi_ethcfg.is_enabledhcp = 1;
  }
  else if(wifi_config.config_mode == STATIC_IP)
  {
    wifi_ethcfg.is_enabledhcp = 0;
  } 
  
  sys_status_get_ipaddress(&ss_ip);
   //ipaddress
  wifi_ethcfg.ipaddr[0] = ss_ip.sys_ipaddress.s_a4;
  wifi_ethcfg.ipaddr[1] = ss_ip.sys_ipaddress.s_a3;
  wifi_ethcfg.ipaddr[2] = ss_ip.sys_ipaddress.s_a2;
  wifi_ethcfg.ipaddr[3] = ss_ip.sys_ipaddress.s_a1;
  
  //netmask
  wifi_ethcfg.netmask[0] = ss_ip.sys_netmask.s_a4;
  wifi_ethcfg.netmask[1] = ss_ip.sys_netmask.s_a3;
  wifi_ethcfg.netmask[2] = ss_ip.sys_netmask.s_a2;
  wifi_ethcfg.netmask[3] = ss_ip.sys_netmask.s_a1;
  
  //gateway
  wifi_ethcfg.gateway[0] = ss_ip.sys_gateway.s_a4;
  wifi_ethcfg.gateway[1] = ss_ip.sys_gateway.s_a3;
  wifi_ethcfg.gateway[2] = ss_ip.sys_gateway.s_a2;
  wifi_ethcfg.gateway[3] = ss_ip.sys_gateway.s_a1;
  
  //dns server
  wifi_ethcfg.primaryDNS[0] = ss_ip.sys_dnsserver.s_a4;
  wifi_ethcfg.primaryDNS[1] = ss_ip.sys_dnsserver.s_a3;
  wifi_ethcfg.primaryDNS[2] = ss_ip.sys_dnsserver.s_a2;
  wifi_ethcfg.primaryDNS[3] = ss_ip.sys_dnsserver.s_a1;

  //dns server
  wifi_ethcfg.alternateDNS[0] = ss_ip.sys_dnsserver2.s_a4;
  wifi_ethcfg.alternateDNS[1] = ss_ip.sys_dnsserver2.s_a3;
  wifi_ethcfg.alternateDNS[2] = ss_ip.sys_dnsserver2.s_a2;
  wifi_ethcfg.alternateDNS[3] = ss_ip.sys_dnsserver2.s_a1;
  
  //mac address
  dev_io_ctrl(p_wifi_devv, GET_ETH_HW_MAC_ADDRESS, (u32)&wifi_ethcfg.hwaddr[0]);
  OS_PRINTF("#######init_wifi_ethcfg#######MAC= %02x-%02x-%02x-%02x-%02x-%02x##############\n",
                          wifi_ethcfg.hwaddr[0], wifi_ethcfg.hwaddr[1],wifi_ethcfg.hwaddr[2],
                          wifi_ethcfg.hwaddr[3], wifi_ethcfg.hwaddr[4], wifi_ethcfg.hwaddr[5]);

}

void do_cmd_to_conn_desktop_wifi()
{
  service_t *p_server = NULL;
  net_svc_cmd_para_t net_svc_para;
  net_svc_t *p_net_svc = NULL;

  sys_status_get_wifi_info(&p_wifi_info);
  if(strlen(p_wifi_info.essid) == 0)
  {
    return;
  }
  p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
  p_net_svc->net_svc_clear_msg(p_net_svc);
  net_svc_para.p_eth_dev = p_wifi_devv;
  memcpy(&net_svc_para.net_cfg, &wifi_ethcfg, sizeof(ethernet_cfg_t));
  strcpy(net_svc_para.net_cfg.wifi_para.key, p_wifi_info.key);
  strcpy(net_svc_para.net_cfg.wifi_para.ssid, p_wifi_info.essid);
  if(strlen(p_wifi_info.key) == 0)
  {
    net_svc_para.net_cfg.wifi_para.is_enable_encrypt = 0;
  }
  else
  {
    net_svc_para.net_cfg.wifi_para.is_enable_encrypt = 1;
  }
  net_svc_para.net_cfg.wifi_para.encrypt_type = p_wifi_info.encrypt_type;
  OS_PRINTF("#####################do_cmd_to_conn_desktop_wifi essid == %s###################\n",net_svc_para.net_cfg.wifi_para.ssid);
  p_server = (service_t *)ui_get_net_svc_instance();
  
  p_server->do_cmd(p_server, NET_DO_WIFI_CONNECT, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));

}

void auto_connect_wifi()
{
  p_wifi_devv = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_WIFI);
  MT_ASSERT(p_wifi_devv != NULL);
  
  init_desktop_wifi_ethcfg();
 // do_cmd_to_search_wifi();
  do_cmd_to_conn_desktop_wifi();
}
/////////////////////////////////////////////////////////////////////////////////////////////////


void auto_connect_ethernet()
{

  ethernet_cfg_t ethcfg = {0};
  ethernet_device_t * eth_dev = NULL;

  eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
  MT_ASSERT(eth_dev != NULL);
  
  init_ethernet_ethcfg(&ethcfg);
  do_cmd_connect_network(&ethcfg, eth_dev);
}
static RET_CODE on_ethernet_plug_in(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  OS_PRINTF("%s(), %d\n", __FUNCTION__, __LINE__);
  auto_connect_ethernet();


//      auto_connect_wifi();
//
  return SUCCESS;
}



static RET_CODE on_ethernet_plug_out(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  //ui_comm_cfmdlg_open_ex(NULL, IDS_NET_CABLE_PLUG_OUT, NULL, 3000);
  control_t *root = fw_find_root_by_id(ROOT_ID_SUBMENU);

  if(root)
  {
    OS_PRINTF("###at network config, keep###\n");
    return ERR_FAILURE;
  }
  OS_PRINTF("############desktop--->>ethernet plug out###########\n");
  g_net_connt_stats = ui_get_net_connect_status();
  g_net_connt_stats.is_eth_insert = FALSE;
  g_net_connt_stats.is_eth_conn = FALSE;
  ui_set_net_connect_status(g_net_connt_stats);

  //if( g_net_connt_stats.is_wifi_conn)
  //{
    ui_comm_cfmdlg_open_ex(NULL, IDS_NET_CABLE_PLUG_OUT, NULL, 2000);
    
   // return SUCCESS;
  //}
  
  //if(!g_net_connt_stats.is_wifi_conn)
  //{
  //}
  return SUCCESS;
}



//static BOOL sg_b_upg_start = FALSE;


//static BOOL sg_b_wifi_conn = FALSE;
extern void paint_connect_status(BOOL is_conn, BOOL is_paint);

static RET_CODE on_network_connected(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)

{
  net_conn_stats_t net_connt_stats = {0};
  net_config_t net_config = {0,};
  control_t *p_root = NULL;

  net_connt_stats = ui_get_net_connect_status();
  

  //OS_PRINTF("##%s,msg == %d##\n", __FUNCTION__, msg);
  switch(msg)
  {
    case MSG_NETWORK_CONNECTED:
      sys_status_get_net_config_info(&net_config);
      if(net_connt_stats.is_eth_conn&&(net_config.link_type == LINK_TYPE_WIFI))
      {
        net_connt_stats.is_eth_conn = FALSE;
        ui_set_net_connect_status(net_connt_stats);
        break;
      }
      if(net_connt_stats.is_wifi_conn&&(net_config.link_type == LINK_TYPE_LAN)) 
      {
        net_connt_stats.is_wifi_conn = FALSE;
        ui_set_net_connect_status(net_connt_stats);
        break;
      }
      
  // OS_PRINTF("\n##%s,msg == MSG_NETWORK_CONNECTED net_connt_stats.is_eth_conn==%d net_connt_stats.is_wifi_conn==%d ##\n", __FUNCTION__,net_connt_stats.is_eth_conn,net_connt_stats.is_wifi_conn);
      if(!net_connt_stats.is_eth_conn && !net_connt_stats.is_wifi_conn)
      {
        if(fw_find_root_by_id(ROOT_ID_SUBMENU))
        {
          paint_connect_status(TRUE, TRUE);
        }
        sys_status_get_net_config_info(&net_config);
        if(net_config.link_type == LINK_TYPE_WIFI)
        {
          net_connt_stats.is_wifi_conn = TRUE;
          p_root = fw_find_root_by_id(ROOT_ID_WIFI);
          if(p_root != NULL)
          {
            on_ping_wifi_ok();
            on_wifi_list_ext_update();
          }
        }
        else
        {
          net_connt_stats.is_eth_conn = TRUE;
        }
        ui_set_net_connect_status(net_connt_stats);
      }
      break;
    case MSG_NETWORK_UNCONNECTED:
      //OS_PRINTF("\n##%s,msg == MSG_NETWORK_UNCONNECTED net_connt_stats.is_eth_conn==%d net_connt_stats.is_wifi_conn==%d ##\n", __FUNCTION__,net_connt_stats.is_eth_conn,net_connt_stats.is_wifi_conn);
      if(fw_find_root_by_id(ROOT_ID_SUBMENU))
      {
        paint_connect_status(FALSE, TRUE);
      }
      if(net_connt_stats.is_eth_conn || net_connt_stats.is_wifi_conn)
      {
        net_connt_stats.is_eth_conn = FALSE;
        net_connt_stats.is_wifi_conn = FALSE;
        ui_set_net_connect_status(net_connt_stats);
        sys_status_get_net_config_info(&net_config);
     //   OS_PRINTF("\n\n net_config.link_type ==%d\n\n",net_config.link_type);
        if(net_config.link_type == LINK_TYPE_WIFI)
       {
        p_root = fw_find_root_by_id(ROOT_ID_WIFI);
        if(p_root != NULL)
        {
         on_wifi_list_ext_update();
         paint_wifi_status(net_connt_stats.is_wifi_conn, TRUE);
        }
       }
      }
      break;
     default:
      MT_ASSERT(0);
      break;
  }
 
  return SUCCESS;
}





static net_upg_api_item_t sg_net_upg_info_main = {0};


static u8* sg_p_save_buf = NULL;
u8* get_net_upg_save_buf(void)
{
  return sg_p_save_buf;
}

void ui_start_upg(net_upg_api_item_t* p_upg_info)
{
  net_upg_api_up_cfg_t cfg = {0};

  cfg.flash_addr_offset = DM_HDR_START_ADDR;
  cfg.flash_burn_size = CHARSTO_SIZE - DM_HDR_START_ADDR;
  cfg.main_code_offset_addr = DM_HDR_START_ADDR;

  cfg.sw = p_upg_info->sw;
  sg_p_save_buf = (u8 *)mtos_malloc(CHARSTO_SIZE);
  MT_ASSERT(sg_p_save_buf!=NULL);
  cfg.p_flash_buf = sg_p_save_buf;
  cfg.flash_buf_size = CHARSTO_SIZE;
  ui_net_upg_upgrade(&cfg);
}

static RET_CODE on_net_upg_check_done(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  net_upg_api_info_t* p_upg_info = ui_net_upg_get_upg_info();
  u16 cnt = p_upg_info->item_cnt;
  u16 i = 0;
  s64 max_sw_ver = -1;
  OS_PRINTF("%s(), %d\n", __FUNCTION__, __LINE__);

  for(i = 0; i < cnt; i++)
  {
    mtos_printk("%s(),%d,%d, sw:%d\n", __FUNCTION__,__LINE__,i, p_upg_info->item[i].sw);
    mtos_printk("%s(),%d,%d, name:%s\n",__FUNCTION__,__LINE__, i, p_upg_info->item[i].url);
  }

  for(i = 0; i < cnt; i++)
  {
    if(0 == p_upg_info->item[i].type && p_upg_info->item[i].sw > max_sw_ver)
    {
      max_sw_ver = p_upg_info->item[i].sw;
      sg_net_upg_info_main = p_upg_info->item[i];
    }
  }


//  manage_open_menu(ROOT_ID_NETWORK_UPGRADE, 1, (u32)&sg_net_upg_info_main);

//  ui_start_upg(&sg_net_upg_info_main);

  
  return SUCCESS;
}

static RET_CODE on_net_upg_url_info(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  int len;
  u8* p;
  u8 post_flag;
  u8* p_url = (u8*)para1;
  mtos_printk("%s(), %d, upg url: %s\n", __FUNCTION__, __LINE__, p_url);

  len = strlen(p_url);
  p = p_url + len - 4;
  post_flag = strcmp(".xml", p);
  ui_net_upg_init(TRUE);
  if( post_flag )
  {
    ui_net_upg_start_check(HTTP, post_flag, p_url, "", "", "");
  }
  else
  {
    while(*p != '/')
      p--;
    *p = '\0';
    ui_net_upg_start_check(HTTP, post_flag, p_url, p+1, "", "");
    mtos_printk("p_addr: %s, p_xml: %s\n", p_url, p+1);
  }

  return SUCCESS;
}


extern void get_addr_param(u8 *p_buffer, ip_address_t *p_addr);
extern void on_config_ip_update(ethernet_device_t * p_dev);
extern u16 get_wifi_select_focus();
extern u16 get_wifi_connecting_focus();

static BOOL is_addr_valued(u8 *addr_str)
{
  ip_address_t addr = {0,};
  
  get_addr_param(addr_str, (ip_address_t *)&addr);
  if((addr.s_b1 == 0)&&(addr.s_b2 == 0)&&(addr.s_b3 == 0)&&(addr.s_b4 == 0))
  {
    return FALSE;
  }
  return TRUE;
}


#if 1
static RET_CODE on_save_config_ip_to_ssdata(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
#if 1
  ip_address_t addr = {0,};
  ethernet_device_t *eth_dev = NULL;
  ip_address_set_t ss_ip = {{0}};

  u8 ipaddr[20] = {0};
  u8 netmask[20] = {0};
  u8 gw[20] = {0};
  u8 primarydns[20] = {0};
  u8 alternatedns[20] = {0};
  u8 ipaddress[32] = {0};
  control_t *root = NULL;
  control_t *root_2 = NULL;
  u32 is_connect = para1;
  net_config_t net_config = {0};
  OS_PRINTF("####destop after auto connect network the para1 is_connect == %d###\n", para1);
  
 // g_net_connt_stats = ui_get_net_connect_status();//at for network config ui 

  sys_status_get_net_config_info(&net_config);
  sys_status_get_ipaddress(&ss_ip);

  root = fw_find_root_by_id(ROOT_ID_WIFI_LINK_INFO);
  root_2 = fw_find_root_by_id(ROOT_ID_WIFI);

  OS_PRINTF("###on_save_config_ip_to_ssdata msg ==%d  is_connect==%d###\n",msg,is_connect);


   if(msg == MSG_WIFI_AP_CONNECT)
   {
     if(root||root_2)
     {
       OS_PRINTF("###at wifi ui, should return from destop connect ok###\n");
       return ERR_FAILURE;
     }
   }
   else 
   {
     if(net_config.link_type == LINK_TYPE_WIFI) 
     {
       #ifndef WIN32
         eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
         if(eth_dev == NULL)
         {
           OS_PRINTF("#######eth_dev == NULL, no need to disconnect eth#####\n");
           return ERR_FAILURE;
         }
         OS_PRINTF("#######disconnect eth for abnormality connect#####\n");
         lwip_netif_link_down(eth_dev);
         ui_comm_dlg_close_ex();
       #endif
       return ERR_FAILURE;
     }
   }
   
  if(net_config.link_type == LINK_TYPE_LAN) 
  {
    if(is_connect)
    {
      OS_PRINTF("###########lwip_init_tcpip return failed, desktop connect failed######\n ");
      ui_comm_dlg_close_ex();
      return ERR_FAILURE;
    }
    eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
    MT_ASSERT(eth_dev != NULL);
#ifndef WIN32
    get_net_device_addr_info(eth_dev, ipaddr, netmask, gw, primarydns, alternatedns);

    get_addr_param(ipaddr, (ip_address_t *)&addr);
    memcpy(&(ss_ip.sys_ipaddress), &addr, sizeof(ip_addr_t));
    sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    OS_PRINTF("###ethernet auto connect  get dhcp ip address value is:%s####\n",ipaddress);
    get_addr_param(netmask, (ip_address_t *)&addr);
    memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
    sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    OS_PRINTF("###ethernet auto connect  get dhcp netmask value is:%s####\n",ipaddress);
    get_addr_param(gw, (ip_address_t *)&addr);
    memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
    sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    OS_PRINTF("###ethernet auto connect  get dhcp gateway  value is:%s####\n",ipaddress);
    if(TRUE == is_addr_valued(primarydns))
    {
      get_addr_param(primarydns, (ip_address_t *)&addr);
      memcpy(&primarydns_addr, &addr, sizeof(ip_addr_t));
      sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    }
    else
    {
      memcpy(&primarydns_addr, &(ss_ip.sys_dnsserver), sizeof(ip_address_t));
      sprintf(ipaddress, "%d.%d.%d.%d",primarydns_addr.s_b1,primarydns_addr.s_b2,primarydns_addr.s_b3,primarydns_addr.s_b4);
    }
    OS_PRINTF("###ethernet auto connect  get dhcp dns_1 value is:%s####\n",ipaddress);
    if(TRUE == is_addr_valued(alternatedns))
    {
      get_addr_param(alternatedns, (ip_address_t *)&addr);
      memcpy(&alternatedns_addr, &addr, sizeof(ip_addr_t));
      sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    }
    else
    {
      memcpy(&alternatedns_addr, &(ss_ip.sys_dnsserver2), sizeof(ip_address_t));
      sprintf(ipaddress, "%d.%d.%d.%d",alternatedns_addr.s_b1,alternatedns_addr.s_b2,alternatedns_addr.s_b3,alternatedns_addr.s_b4);
    }
    OS_PRINTF("###ethernet auto connect  get dhcp dns_2 value is:%s####\n",ipaddress);
#endif
    sys_status_set_ipaddress(&ss_ip);
    sys_status_save();
    OS_PRINTF("@@@!auto connect ethernet successfully@@@\n");
//    g_net_connt_stats.is_eth_conn = TRUE;
    root = fw_find_root_by_id(ROOT_ID_SUBMENU);

    if(root)
    {
      OS_PRINTF("###will update ethernet dhcp and paint network config connect status at network config menu###\n");
      on_config_ip_update(eth_dev);//update ethernet dhcp ipaddress
      //close dlg
      ui_comm_dlg_close_ex();
//      paint_connect_status(g_net_connt_stats.is_eth_conn, TRUE);
    }
  }
  else if(net_config.link_type == LINK_TYPE_WIFI) 
  {
#ifndef WIN32
    p_wifi_devv = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_WIFI);//add for wifi ui get wifi connected msg
    MT_ASSERT(p_wifi_devv != NULL);
    get_net_device_addr_info(p_wifi_devv, ipaddr, netmask, gw, primarydns, alternatedns);

    get_addr_param(ipaddr, (ip_address_t *)&addr);
    memcpy(&(ss_ip.sys_ipaddress), &addr, sizeof(ip_addr_t));
    sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    OS_PRINTF("###wifi auto connect  get dhcp ip address value is:%s####\n",ipaddress);
    get_addr_param(netmask, (ip_address_t *)&addr);
    memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
    sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    OS_PRINTF("###wifi auto connect  get dhcp netmask value is:%s####\n",ipaddress); 
    if(TRUE == is_addr_valued(primarydns))
    {
      get_addr_param(primarydns, (ip_address_t *)&addr);
      memcpy(&primarydns_addr, &addr, sizeof(ip_addr_t));
      sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    }
    else
    {
      memcpy(&primarydns_addr, &(ss_ip.sys_dnsserver), sizeof(ip_address_t));
      sprintf(ipaddress, "%d.%d.%d.%d",primarydns_addr.s_b1,primarydns_addr.s_b2,primarydns_addr.s_b3,primarydns_addr.s_b4);
    }
    OS_PRINTF("###ethernet auto connect  get dhcp dns_1 value is:%s####\n",ipaddress);
    if(TRUE == is_addr_valued(alternatedns))
    {
      get_addr_param(alternatedns, (ip_address_t *)&addr);
      memcpy(&alternatedns_addr, &addr, sizeof(ip_addr_t));
      sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
    }
    else
    {
      memcpy(&alternatedns_addr, &(ss_ip.sys_dnsserver2), sizeof(ip_address_t));
      sprintf(ipaddress, "%d.%d.%d.%d",alternatedns_addr.s_b1,alternatedns_addr.s_b2,alternatedns_addr.s_b3,alternatedns_addr.s_b4);
    }
    OS_PRINTF("###ethernet auto connect  get dhcp dns_2 value is:%s####\n",ipaddress);
#endif
    sys_status_set_ipaddress(&ss_ip);
    sys_status_save();
   // g_net_connt_stats.is_wifi_conn = TRUE;
    OS_PRINTF("@@@!auto connect wifi successfully@@@\n");
    root = fw_find_root_by_id(ROOT_ID_SUBMENU);

    if(root)
    {
      OS_PRINTF("###will update wifi dhcp and paint network config connect status at network config menu###\n");
      on_config_ip_update(p_wifi_devv);//update wifi dhcp ipaddress
      ui_comm_dlg_close_ex();
 //     paint_connect_status(g_net_connt_stats.is_wifi_conn, TRUE);
    }

    root = fw_find_root_by_id(ROOT_ID_WIFI);

    if(root)
    {
      u16 focus = get_wifi_select_focus();
      if(focus == INVALID_ID)
      {
        focus = get_wifi_connecting_focus();
      }
      OS_PRINTF("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$focus======%d$$$$$$$$$$$$$$$$$$\n",focus);
    //  paint_list_field_is_connect(focus, TRUE, TRUE);
    }
    
  }
  OS_PRINTF("###save ip addr to ssdata successfully###\n");
#endif
#if 0 
  OS_PRINTF("##start call ui_uitil_get_city_init##\n");
  ui_util_get_city_init();  //for get city to get weather
  OS_PRINTF("##%s, end call ui_uitil_get_city_init##\n");
#endif  
  return SUCCESS;
}
#endif





static RET_CODE on_usb_dev_plug_in(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  u32 usb_dev_type = para2;
  u16 stringid = 0;
  net_config_t g_net_config = {0,};


  if(usb_dev_type == HP_WIFI)
  {
  //  is_usb_wifi = TRUE;
    stringid = IDS_WIFI_PLUG_IN;
    g_net_connt_stats.is_wifi_insert = TRUE;
    sys_status_get_net_config_info(&g_net_config);
    if(g_net_config.link_type == LINK_TYPE_WIFI)
    {
      ;//auto_connect_wifi();
    }
    ui_comm_cfmdlg_open_ex(NULL, IDS_WIFI_PLUG_IN, NULL, 2000);

  }
  return SUCCESS;
}



BEGIN_AP_EVTMAP(pnp_evtmap)
CONVERT_EVENT(PNP_SVC_USB_MASS_PLUG_IN, MSG_PLUG_IN)
CONVERT_EVENT(PNP_SVC_USB_DEV_PLUG_OUT, MSG_PLUG_OUT)
CONVERT_EVENT(PNP_SVC_USB_DEV_PLUG_IN, MSG_USB_DEV_PLUG_IN)
CONVERT_EVENT(PNP_SVC_VFS_MOUNT_SUCCESS, MSG_NETWORK_MOUNT_SUCCESS)
CONVERT_EVENT(PNP_SVC_VFS_MOUNT_FAIL, MSG_NETWORK_MOUNT_FAIL)
CONVERT_EVENT(PNP_SVC_INTERNET_PLUG_IN, MSG_INTERNET_PLUG_IN)
CONVERT_EVENT(PNP_SVC_INTERNET_PLUG_OUT, MSG_INTERNET_PLUG_OUT)
CONVERT_EVENT(PNP_SVC_WIFI_CONNECTING_AP, MSG_WIFI_AP_CONNECTING)
CONVERT_EVENT(PNP_SVC_WIFI_CONNECT_AP, MSG_WIFI_AP_CONNECT)
CONVERT_EVENT(PNP_SVC_WIFI_DIS_AP, MSG_WIFI_AP_DISCONNECT)
CONVERT_EVENT(NET_SVC_CONFIG_IP, MSG_CONFIG_IP)
CONVERT_EVENT(NET_SVC_PING_TEST, MSG_PING_TEST)
CONVERT_EVENT(NET_SVC_WIFI_AP_CNT, MSG_GET_WIFI_AP_CNT)
CONVERT_EVENT(NET_SVC_CHAIN_CONNECTED, MSG_NETWORK_CONNECTED)
CONVERT_EVENT(NET_SVC_CHAIN_UNCONNECTED, MSG_NETWORK_UNCONNECTED)
END_AP_EVTMAP(pnp_evtmap)


BEGIN_MSGPROC(ui_desktop_proc, cont_class_proc)
  ON_COMMAND(MSG_INTERNET_PLUG_IN, on_ethernet_plug_in)
  ON_COMMAND(MSG_INTERNET_PLUG_OUT, on_ethernet_plug_out)
  ON_COMMAND(MSG_NETWORK_CONNECTED, on_network_connected)
  ON_COMMAND(MSG_NETWORK_UNCONNECTED, on_network_connected)
  ON_COMMAND(MSG_NET_UPG_EVT_CHECK_DONE, on_net_upg_check_done)
  ON_COMMAND(MSG_UPG_URL_INFO, on_net_upg_url_info)
  ON_COMMAND(MSG_CONFIG_IP, on_save_config_ip_to_ssdata)
  ON_COMMAND(MSG_USB_DEV_PLUG_IN, on_usb_dev_plug_in)
END_MSGPROC(ui_desktop_proc, cont_class_proc);

