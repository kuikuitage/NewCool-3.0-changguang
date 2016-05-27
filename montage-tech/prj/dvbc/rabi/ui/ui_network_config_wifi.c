/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_network_config_wifi.h"
#include "ui_network_config_lan.h"
#include "ethernet.h"
#include "ui_wifi.h"
#include "wifi.h"
#include "ui_keyboard_v2.h"

#ifndef WIN32
 #include "ppp.h"
 #include "modem.h"
#endif

enum ip_ctrl_id
{
  IDC_CONNECT_TYPE = 1,
  IDC_CNFIG_MODE,
  IDC_IP_ADDRESS,
  IDC_NETMASK,
  IDC_GATEWAY,
  IDC_DNS_SERVER,
  IDC_MAC_ADDRESS,
  IDC_PING_TEST,
  //IDC_STORAGE_PATH,
  IDC_CONNECT_NETWORK,
  
  IDC_CONNECT_STATUS_TXT,
  IDC_CONNECT_STATUS,
  
  IDC_GPRS_APN,
  IDC_GPRS_DIAL,
  IDC_GPRS_IP,
  IDC_GPRS_MASK,
  IDC_GPRS_DNS_PREFER,
  IDC_GPRS_DNS_ALTER,
  IDC_GPRS_SIGNAL,
  IDC_GPRS_PING_TEST,
  IDC_GPRS_CONNECT,    
};

u16 network_wifi_config_cont_keymap(u16 key);
u16 ping_wifi_test_keymap(u16 key);
u16 connect_wifi_network_keymap(u16 key);
u16 link_type_keymap(u16 key);
u16 gprs_ping_test_keymap(u16 key);
u16 gprs_connect_keymap(u16 key);

RET_CODE network_wifi_config_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE ping_wifi_test_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE network_wifi_mode_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE connect_wifi_network_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE gprs_apn_set_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE gprs_dial_set_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE gprs_ping_test_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE gprs_connect_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

extern void init_ethernet_ethcfg(ethernet_cfg_t *ethcfg);
extern void auto_connect_ethernet();
extern void do_cmd_connect_gprs();
extern void do_cmd_disconnect_gprs();

void do_cmd_connect_network(ethernet_cfg_t *ethcfg, ethernet_device_t * eth_dev);

static u8 ip_separator[IPBOX_MAX_ITEM_NUM] = {'.', '.', '.', ' '};
//static partition_t *p_partition = NULL;
//static u32 g_partition_count = 0;

static ethernet_cfg_t      ethcfg = {0};
static ethernet_cfg_t      static_wifi_ethcfg = {0};
static ip_address_set_t ss_ip = {{0}};
//static link_type_t link_type = LINK_TYPE_LAN;
static net_config_t g_net_config;
static net_conn_stats_t eth_connt_stats;


static  void ssdata_ip_address_get()
{
  sys_status_get_ipaddress(&ss_ip);
}

static void ssdata_ip_address_set()
{
  sys_status_set_ipaddress(&ss_ip);
  sys_status_save();
}
/*
static RET_CODE fill_partition_name(control_t *p_cbox, u16 focus, u16 *p_str,
                              u16 max_length)
{
  uni_strncpy(p_str, p_partition[focus].name, max_length);
  return SUCCESS;
}
*/

static void get_mac_address(void)
{
#ifndef WIN32
  ethernet_device_t* p_wifi_dev = (ethernet_device_t *)get_wifi_handle();

  dev_io_ctrl(p_wifi_dev, GET_ETH_HW_MAC_ADDRESS, (u32)&ethcfg.hwaddr[0]);
#endif
  OS_PRINTF("MAC= %02x-%02x-%02x-%02x-%02x-%02x\n",	
              ethcfg.hwaddr[0], ethcfg.hwaddr[1],ethcfg.hwaddr[2],
              ethcfg.hwaddr[3], ethcfg.hwaddr[4], ethcfg.hwaddr[5]);
}

static control_t *create_network_config_menu()
{
	control_t *p_menu = NULL;

	p_menu = ui_comm_right_root_create(ROOT_ID_NETWORK_CONFIG_WIFI, 
											ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);

	ctrl_set_keymap(p_menu, network_wifi_config_cont_keymap);
	ctrl_set_proc(p_menu, network_wifi_config_cont_proc);

	return p_menu;
}

static void create_config_ctrl_items(control_t *p_menu)
{
  control_t *p_ctrl[WIFI_IP_CTRL_CNT] = { NULL };
  u16 network_str[LAN_CONFIG_MODE_TYPE_CNT + 1] = { IDS_DHCP, IDS_STATIC_IP, IDS_PPPOE };
  u16 network_str_wifi[CONFIG_MODE_TYPE_CNT - 1] = { IDS_DHCP, IDS_STATIC_IP};
  //u16 type_str[3] = { IDS_LAN, IDS_WIFI, IDS_GPRS};
  u16 ctrl_str[WIFI_IP_CTRL_CNT] =
  {
    IDS_LINK_TYPE,IDS_CONFIG_MODE,IDS_IP_ADDRESS,IDS_NETMASK,IDS_GATEWAY,
    IDS_DNS_SREVER, IDS_DHCP_SERVER,IDS_PING_TEST,/*IDS_PREFERRED_STORAGE,*/0
  };
  ip_address_t temp_address = {0};
  u8 macaddr[128] = {0};
  u16 i, j, y;

  sys_status_get_net_config_info(&g_net_config);
  y = NETWORK_ITEM_Y;
  for(i = 0;i < WIFI_IP_CTRL_CNT;i++)
  {
     switch(i)
      {
        case 0: 
    			p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW+NETWORK_ITEM_RW, COMM_CTRL_H, p_menu, 0);
    			text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
    			text_set_font_style(p_ctrl[i], FSI_WHITE, FSI_WHITE, FSI_WHITE);
    			text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);  
    			text_set_content_by_strid(p_ctrl[i], IDS_LINK_TYPE_WIFI);		
			  break;		
        case 1:
          p_ctrl[i] = ui_comm_select_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_select_set_static_txt(p_ctrl[i], ctrl_str[i]);
          if(g_net_config.link_type == LINK_TYPE_WIFI)
          {
            ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC, CONFIG_MODE_TYPE_CNT - 1, CBOX_ITEM_STRTYPE_STRID, NULL);
            for(j = 0;j < CONFIG_MODE_TYPE_CNT - 1;j++)
            {
               ui_comm_select_set_content(p_ctrl[i], j, network_str_wifi[j]);
            }
            if(g_net_config.config_mode == DHCP)
            {
              ui_comm_select_set_focus(p_ctrl[i], 0);
            }
            else if(g_net_config.config_mode == STATIC_IP)
            {
              ui_comm_select_set_focus(p_ctrl[i], 1);
            }
            
            ui_comm_ctrl_set_proc(p_ctrl[i],network_wifi_mode_proc);
          }
          else
          {
            ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC, LAN_CONFIG_MODE_TYPE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
            for(j = 0;j < LAN_CONFIG_MODE_TYPE_CNT;j++)
            {
               ui_comm_select_set_content(p_ctrl[i], j, network_str[j]);
            }
            OS_PRINTF("####open func   is_enable_dhcp == %d########\n", g_net_config.config_mode);
            if(g_net_config.config_mode == DHCP)
            {
              ui_comm_select_set_focus(p_ctrl[i], 0);
            }
            else if(g_net_config.config_mode == STATIC_IP)
            {
              ui_comm_select_set_focus(p_ctrl[i], 1);
            }
            else 
            {
              ui_comm_select_set_focus(p_ctrl[i], 2);
            }
            ui_comm_ctrl_set_proc(p_ctrl[i],network_wifi_mode_proc);
            
          }
          ui_comm_select_create_droplist(p_ctrl[i], LAN_CONFIG_MODE_TYPE_CNT);
			  break;		
        case 2:
          p_ctrl[i] = ui_comm_ipedit_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_ipaddress), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
          
		      ctrl_set_rstyle(p_ctrl[i],
            						  RSI_COMM_CONT_SH,
            						  RSI_SELECT_F,
            						  RSI_COMM_CONT_GRAY);
          ui_comm_ctrl_set_keymap(p_ctrl[i], ui_comm_ipbox_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], ui_comm_ipbox_proc);
          for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
          {
            ui_comm_ipedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, 
            ip_separator[j]);
          }
          if(g_net_config.config_mode == STATIC_IP)
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], TRUE); 
          }
          else
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
          }
        break;
        case 3:
          p_ctrl[i] = ui_comm_ipedit_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_netmask), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
          ui_comm_ctrl_set_keymap(p_ctrl[i], ui_comm_ipbox_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], ui_comm_ipbox_proc);
           for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
          {
            ui_comm_ipedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, 
            ip_separator[j]);
          }
          if(g_net_config.config_mode == STATIC_IP)
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], TRUE); 
          }
          else
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
          }
        break;
        case 4:
          p_ctrl[i] = ui_comm_ipedit_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_gateway), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
          ui_comm_ctrl_set_keymap(p_ctrl[i], ui_comm_ipbox_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], ui_comm_ipbox_proc);
           for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
          {
            ui_comm_ipedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, 
            ip_separator[j]);
          }
          if(g_net_config.config_mode == STATIC_IP)
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], TRUE); 
          }
          else
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
          }
        break;
        case 5:
          p_ctrl[i] = ui_comm_ipedit_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_dnsserver), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
          ui_comm_ctrl_set_keymap(p_ctrl[i], ui_comm_ipbox_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], ui_comm_ipbox_proc);
           for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
          {
            ui_comm_ipedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, 
            ip_separator[j]);
          }
          if(g_net_config.config_mode == STATIC_IP)
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], TRUE); 
          }
          else
          {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
          }
        break;
        case 6:
          p_ctrl[i] = ui_comm_static_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE);
          ui_comm_static_set_static_txt(p_ctrl[i], IDS_MAC_ADDR);//MAC ADDRESS
          sprintf((char *)macaddr,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
                    ethcfg.hwaddr[0], ethcfg.hwaddr[1], ethcfg.hwaddr[2],
                    ethcfg.hwaddr[3], ethcfg.hwaddr[4], ethcfg.hwaddr[5]);

          macaddr[17]='\0';
          ui_comm_static_set_content_by_ascstr(p_ctrl[i], macaddr);
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
        break;
        case 7:
          p_ctrl[i] = ui_comm_static_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_static_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ctrl_set_keymap(p_ctrl[i], ping_wifi_test_keymap);
          ctrl_set_proc(p_ctrl[i], ping_wifi_test_proc);
        break;
        case 8:
          p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW+NETWORK_ITEM_RW, COMM_CTRL_H, p_menu, 0);
          ctrl_set_keymap(p_ctrl[i], connect_wifi_network_keymap);
          ctrl_set_proc(p_ctrl[i], connect_wifi_network_proc);
          ctrl_set_rstyle(p_ctrl[i], RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
          text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
          text_set_font_style(p_ctrl[i], FSI_WHITE, FSI_BLACK, FSI_WHITE);
          text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
          text_set_content_by_strid(p_ctrl[i], IDS_SEARCH);
        break;
       
      }
      y += NETWORK_ITEM_H + NETWORK_ITEM_VGAP;

      if(i != 0)
      {
          if(i == 1)
          {
              ctrl_set_related_id(p_ctrl[i], 0, WIFI_IP_CTRL_CNT, 0, 3);
          }
          else if(i == (WIFI_IP_CTRL_CNT-1))
          {
               ctrl_set_related_id(p_ctrl[i], 0, (WIFI_IP_CTRL_CNT-1), 0, 2);
          }
          else
          {
              ctrl_set_related_id(p_ctrl[i], 0, i + 1 - 1, 0, i + 1 + 1);
          }
  
       }
      
      
  }

  ctrl_default_proc(p_ctrl[1], MSG_GETFOCUS, 0, 0);
  
}

void paint_connect_wifi_status(BOOL is_conn, BOOL is_paint)
{
  control_t *p_conn_status = NULL;
  control_t *root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_WIFI);
  
  if(root == NULL)
  {
    DEBUG(MAIN, INFO, "########not need to paint connect status , is_conn == %d####\n", is_conn);
    return ;
  }
  OS_PRINTF("########need to paint connect status , is_conn == %d####\n", is_conn);
  p_conn_status = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_CONNECT_STATUS);

  if(is_conn)
  {
    text_set_font_style(p_conn_status, FSI_GREEN, FSI_GREEN, FSI_GREEN);
    text_set_content_by_strid(p_conn_status, IDS_CONNECT);
  }
  else
  {
    text_set_font_style(p_conn_status, FSI_RED, FSI_RED, FSI_RED);
    text_set_content_by_strid(p_conn_status, IDS_DISCONNECT);
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_conn_status, TRUE);
  }
  
}

static void create_connect_status_ctrl(control_t *p_menu)
{
  control_t *p_status_txt = NULL, *p_status = NULL;

  p_status_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_CONNECT_STATUS_TXT, NETWORK_CONNECT_STATUS_TEXTX, 
  NETWORK_CONNECT_STATUS_TEXTY, NETWORK_CONNECT_STATUS_TEXTW, NETWORK_CONNECT_STATUS_TEXTH, p_menu, 0);

  ctrl_set_rstyle(p_status_txt, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_status_txt, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_status_txt, FSI_WHITE, FSI_BLACK, FSI_WHITE);
  text_set_content_type(p_status_txt, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_status_txt, IDS_NETWORK_STATUS);

  p_status = ctrl_create_ctrl(CTRL_TEXT, IDC_CONNECT_STATUS, NETWORK_CONNECT_STATUSX, 
  NETWORK_CONNECT_STATUSY, NETWORK_CONNECT_STATUSW, NETWORK_CONNECT_STATUSH, p_menu, 0);

  ctrl_set_rstyle(p_status, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_status, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_status, TEXT_STRTYPE_STRID);


  eth_connt_stats = ui_get_net_connect_status();
  OS_PRINTF("################print connect status, %d,%d,%d,%d############", 
                     eth_connt_stats.is_eth_conn,eth_connt_stats.is_eth_insert,eth_connt_stats.is_wifi_conn,
                     eth_connt_stats.is_wifi_insert);
  if(eth_connt_stats.is_wifi_conn)
  {
    paint_connect_wifi_status(TRUE, FALSE);
  }
  else
  {
    paint_connect_wifi_status(FALSE, FALSE);
  }

}


RET_CODE open_network_config_wifi(u32 para1, u32 para2)
{
	control_t *p_menu = NULL;
	ssdata_ip_address_get();

  get_mac_address();
 
  //create main menu
  p_menu = create_network_config_menu();

	//create config items
	create_config_ctrl_items(p_menu);

	//create network connect status
	create_connect_status_ctrl(p_menu);

	ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

	return SUCCESS;

}


static RET_CODE on_config_test_select(control_t *p_sub, u16 msg,
  u32 para1, u32 para2)
{
  manage_open_menu(ROOT_ID_PING_TEST, 0, 0);
  
  return SUCCESS;
}

static void static_ip_to_dhcp()
{
  ip_address_t temp_address = {0};
  control_t *p_ctrl[4] = {NULL};
  u16 i;
  
  ssdata_ip_address_get();
  for(i = 0;i < 4;i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_IP_ADDRESS);
        memcpy(&temp_address, &(ss_ip.sys_ipaddress), sizeof(ip_address_t));
        ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
       break;
      case 1:
        p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_NETMASK);
        memcpy(&temp_address, &(ss_ip.sys_netmask), sizeof(ip_address_t));
        ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
       break;
      case 2:
        p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_GATEWAY);
        memcpy(&temp_address, &(ss_ip.sys_gateway), sizeof(ip_address_t));
        ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
       break;
      case 3:
        p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI,IDC_DNS_SERVER);
        memcpy(&temp_address, &(ss_ip.sys_dnsserver), sizeof(ip_address_t));
        ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
       break;   
     default:
        break;
    }
  }
  
}

static void update_ipbox_attr(BOOL is_enable)
{
  u16 i = 0;
  control_t *p_ctrl[4] = {NULL};
  for(i = 0;i < 4;i++)
  {
    p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_IP_ADDRESS + i);
    ui_comm_ctrl_update_attr(p_ctrl[i], is_enable); 
    ctrl_paint_ctrl(p_ctrl[i], TRUE);
  }
}

static RET_CODE on_network_mode_change(control_t *p_cbox, u16 msg,
  u32 para1, u32 para2)
{
  u16 focus = 0;
  control_t *p_ctrl = NULL;
  focus = ui_comm_select_get_focus(p_cbox->p_parent);
  p_ctrl = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_CONNECT_NETWORK);
  switch(focus)
  {
    case 0:
      g_net_config.config_mode = DHCP; 
      static_ip_to_dhcp();
      update_ipbox_attr(FALSE);
      if(g_net_config.link_type == LINK_TYPE_WIFI)
      {
        text_set_content_by_strid(p_ctrl, IDS_SEARCH);
      }
      else
      {
        text_set_content_by_strid(p_ctrl, IDS_CONNECT);
      }
      break;
    case 1:
      g_net_config.config_mode = STATIC_IP; 
      update_ipbox_attr(TRUE);
      break;
    case 2:
    if(g_net_config.link_type == LINK_TYPE_WIFI)
      return ERR_FAILURE;
      g_net_config.config_mode = PPPOE; 
      update_ipbox_attr(FALSE);
      if(g_net_config.link_type == LINK_TYPE_LAN)
      {
        text_set_content_by_strid(p_ctrl, IDS_PPPOE_CONN);
      }
      break;
    default:
      break;
  }
  ctrl_paint_ctrl(p_ctrl, TRUE);
  sys_status_set_net_config_info(&g_net_config);
  sys_status_save();
  
  return SUCCESS;
}


void get_addr_param_wifi(u8 *p_buffer, ip_address_t *p_addr)
{

  p_addr->s_b1 = p_buffer[0];
  p_addr->s_b2 = p_buffer[1];
  p_addr->s_b3 = p_buffer[2];
  p_addr->s_b4 = p_buffer[3];
}

void on_config_ip_update_wifi(ethernet_device_t * p_dev)
{
  ip_address_t addr = {0,};
  control_t *p_ctrl[4] = {NULL};
  u8 ipaddress[32] = {0};
  u8 i;

#ifndef WIN32
  u8 ipaddr[20] = {0};
  u8 netmask[20] = {0};
  u8 gw[20] = {0};
  u8 primarydns[20] = {0};
  u8 alternatedns[20] = {0};
  
  get_net_device_addr_info(p_dev, ipaddr, netmask, gw, primarydns, alternatedns);

#endif

  if(g_net_config.link_type == LINK_TYPE_GPRS)
  {
      for(i = 0;i < 4;i++)
      {
        switch(i)
        {
          case 0:
            p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_GPRS_IP);
            #ifndef WIN32
            get_addr_param_wifi(ipaddr, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_ipaddress), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp ip address value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;
          case 1:
            p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_GPRS_MASK);
            #ifndef WIN32
            get_addr_param_wifi(netmask, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp netmask value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;
          case 2:
            p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_GPRS_DNS_PREFER);
            #ifndef WIN32
            get_addr_param_wifi(gw, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp gateway value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;
          case 3:
            p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI,IDC_GPRS_DNS_ALTER);
            #ifndef WIN32
            get_addr_param_wifi(primarydns, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp dns server value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;   
         default:
            break;
        }
      }
  }
  else
  {
      for(i = 0;i < 4;i++)
      {
        switch(i)
        {
          case 0:
            p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_IP_ADDRESS);
            #ifndef WIN32
            get_addr_param_wifi(ipaddr, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_ipaddress), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp ip address value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;
          case 1:
            p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_NETMASK);
            #ifndef WIN32
            get_addr_param_wifi(netmask, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp netmask value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;
          case 2:
            p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_GATEWAY);
            #ifndef WIN32
            get_addr_param_wifi(gw, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp gateway value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;
          case 3:
            p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI,IDC_DNS_SERVER);
            #ifndef WIN32
            get_addr_param_wifi(primarydns, (ip_address_t *)&addr);
            #endif
            ui_comm_ipedit_set_address(p_ctrl[i], &addr);
            memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
            sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
            OS_PRINTF("###update dhcp dns server value is:%s####\n",ipaddress);
            ctrl_paint_ctrl(p_ctrl[i], TRUE);
           break;   
         default:
            break;
        }
      }
  }

  ssdata_ip_address_set();
  
}

static void config_network_wifi_ethcfg()
{
  control_t *p_ctrl[4] = { NULL };
  u8 ipaddress[32] = {0};
  ip_address_t addr;
  u16 i;
  
  ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
  if(g_net_config.config_mode == DHCP)
  {
    ethcfg.is_enabledhcp = TRUE;
  }
  else
  {
    ethcfg.is_enabledhcp = FALSE;
  }
  OS_PRINTF("@@@config_network_wifi_ethcfg, ethcfg.is_enabledhcp = %d@@@@\n",g_net_config.config_mode);

  for(i = 0;i < 4;i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_IP_ADDRESS);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_ipaddress), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
      
        ethcfg.ipaddr[0] = addr.s_b4;
        ethcfg.ipaddr[1] = addr.s_b3;
        ethcfg.ipaddr[2] = addr.s_b2;
        ethcfg.ipaddr[3] = addr.s_b1;
        OS_PRINTF("###ip address  is:%s####\n",ipaddress);
       break;
      case 1:
        p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_NETMASK);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
       
        ethcfg.netmask[0] = addr.s_b4;
        ethcfg.netmask[1] = addr.s_b3;
        ethcfg.netmask[2] = addr.s_b2;
        ethcfg.netmask[3] = addr.s_b1;
        OS_PRINTF("###netmask  is:%s####\n",ipaddress);
       break;
      case 2:
        p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_GATEWAY);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
       
        ethcfg.gateway[0] = addr.s_b4;
        ethcfg.gateway[1] = addr.s_b3;
        ethcfg.gateway[2] = addr.s_b2;
        ethcfg.gateway[3] = addr.s_b1;
        OS_PRINTF("###gateway  is:%s####\n",ipaddress);
       break;
      case 3:
        p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_DNS_SERVER);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
       
        ethcfg.primaryDNS[0] = addr.s_b4;
        ethcfg.primaryDNS[1] = addr.s_b3;
        ethcfg.primaryDNS[2] = addr.s_b2;
        ethcfg.primaryDNS[3] = addr.s_b1;
        OS_PRINTF("###dns server is:%s####\n",ipaddress);
       break;
     default:
        break;
    }
  }
  
}

ethernet_cfg_t get_static_wifi_ethcfg()
{
  return static_wifi_ethcfg;
}
static void set_static_wifi_ethcfg()
{
  control_t *p_ctrl[4] = { NULL };
  u8 ipaddress[32] = {0};
  ip_address_t addr;
  u16 i;
  static_wifi_ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
  static_wifi_ethcfg.is_enabledhcp = FALSE;
  for(i = 0;i < 4;i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_IP_ADDRESS);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
        static_wifi_ethcfg.ipaddr[0] = addr.s_b4;
        static_wifi_ethcfg.ipaddr[1] = addr.s_b3;
        static_wifi_ethcfg.ipaddr[2] = addr.s_b2;
        static_wifi_ethcfg.ipaddr[3] = addr.s_b1;
        OS_PRINTF("###static wifi ip address  is:%s####\n",ipaddress);
       break;
      case 1:
        p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_NETMASK);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
        static_wifi_ethcfg.netmask[0] = addr.s_b4;
        static_wifi_ethcfg.netmask[1] = addr.s_b3;
        static_wifi_ethcfg.netmask[2] = addr.s_b2;
        static_wifi_ethcfg.netmask[3] = addr.s_b1;
        OS_PRINTF("###static wifi netmask  is:%s####\n",ipaddress);
       break;
      case 2:
        p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_GATEWAY);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
        static_wifi_ethcfg.gateway[0] = addr.s_b4;
        static_wifi_ethcfg.gateway[1] = addr.s_b3;
        static_wifi_ethcfg.gateway[2] = addr.s_b2;
        static_wifi_ethcfg.gateway[3] = addr.s_b1;
        OS_PRINTF("###static wifi gateway  is:%s####\n",ipaddress);
       break;
      case 3:
        p_ctrl[i] = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_DNS_SERVER);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
        static_wifi_ethcfg.primaryDNS[0] = addr.s_b4;
        static_wifi_ethcfg.primaryDNS[1] = addr.s_b3;
        static_wifi_ethcfg.primaryDNS[2] = addr.s_b2;
        static_wifi_ethcfg.primaryDNS[3] = addr.s_b1;
        OS_PRINTF("###static wifi dns server is:%s####\n",ipaddress);
       break;
     default:
        break;
    }
  }
}
static RET_CODE on_search_network(control_t *p_sub, u16 msg,
  u32 para1, u32 para2)
{
  control_t *root = NULL;
    
  eth_connt_stats = ui_get_net_connect_status();
  if(g_net_config.link_type == LINK_TYPE_WIFI)
  {
    if(!eth_connt_stats.is_wifi_insert)
    {
      OS_PRINTF("@@@wifi connect not ok@@@\n");
      ui_comm_cfmdlg_open(NULL, IDS_WIFI_DEV_NOT_EXIST, NULL, 2000);
      return ERR_FAILURE;
    }
    if(g_net_config.config_mode == STATIC_IP)
    {
      set_static_wifi_ethcfg();
    }

    root = ui_comm_right_root_get_root(ROOT_ID_WIFI);
    if(root)
    {
      ctrl_process_msg(root, MSG_EXIT, 0, 0);
    }
  
    manage_open_menu(ROOT_ID_WIFI, para1, para2);
    manage_close_menu(ROOT_ID_NETWORK_CONFIG_WIFI, 0, 0);//fix 47420
    return SUCCESS;
  }
  return ERR_FAILURE;
}


static RET_CODE on_network_config_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
   control_t *root;

   if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
   {
      OS_PRINTF("###disconnecting not exit will return from on_network_config_exit####\n");
      return ERR_FAILURE;
   }
  config_network_wifi_ethcfg();
  ssdata_ip_address_set();
  sys_status_set_net_config_info(&g_net_config);
  sys_status_save();

  root = ui_comm_right_root_get_root(ROOT_ID_WIFI);
  if(root)
  {
    ctrl_process_msg(root, MSG_EXIT, 0, 0);
  }
  
  manage_open_menu(ROOT_ID_WIFI, para1, para2);
  manage_close_menu(ROOT_ID_NETWORK_CONFIG_WIFI, 0, 0);//fix 47420

  OS_PRINTF("@@@@@@@@@@@on_network_config_exit @@@@@@@@@\n");
  return SUCCESS;
}

static RET_CODE on_network_config_plug_in(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  net_config_t net_config = {0,};
  eth_connt_stats.is_eth_insert = TRUE;
  ui_set_net_connect_status(eth_connt_stats);
  sys_status_get_net_config_info(&net_config);
  if((net_config.link_type == LINK_TYPE_LAN) && !eth_connt_stats.is_usb_eth_conn)
  {
    auto_connect_ethernet();
  }
  
  OS_PRINTF("@@@@@@@@@@@on_network_config_plug_in @@@@@@@@@\n");
  
  return SUCCESS;
}

static RET_CODE on_network_config_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  net_conn_stats_t net_connt_stats;
  net_connt_stats = ui_get_net_connect_status();
  
  if (!net_connt_stats.is_wifi_conn)
  {
    paint_connect_wifi_status((net_connt_stats.is_eth_conn || net_connt_stats.is_usb_eth_conn), TRUE);
  }
  OS_PRINTF("@@@@@@@@@@@on_network_config_plug_out is_eth_conn = %d,is_eth_insert = %d@@@@@@@@@\n",net_connt_stats.is_eth_conn, net_connt_stats.is_eth_insert);
  
  return SUCCESS;
}

static RET_CODE on_network_config_usb_plug_in(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  net_config_t net_config = {0,};
  eth_connt_stats.is_usb_eth_insert = TRUE;
  ui_set_net_connect_status(eth_connt_stats);
  sys_status_get_net_config_info(&net_config);
  if(net_config.link_type == LINK_TYPE_LAN && !eth_connt_stats.is_eth_conn)
  {
    auto_connect_ethernet();
  }
  
  OS_PRINTF("@@@@@@@@@@@on_network_config_plug_in @@@@@@@@@\n");
  
  return SUCCESS;
}

void paint_wifi_connect_status(BOOL is_conn, BOOL is_paint)
{
  control_t *p_conn_status = NULL;
  control_t *root = fw_find_root_by_id(ROOT_ID_NETWORK_CONFIG_WIFI);
  
  if(root == NULL)
  {
    OS_PRINTF("########wifi not need to paint connect status , is_conn == %d####\n", is_conn);
    return ;
  }
  OS_PRINTF("########wifi need to paint connect status , is_conn == %d####\n", is_conn);
  p_conn_status = ui_comm_right_root_get_ctrl(ROOT_ID_NETWORK_CONFIG_WIFI, IDC_CONNECT_STATUS);

  if(is_conn)
  {
    text_set_font_style(p_conn_status, FSI_GREEN, FSI_GREEN, FSI_GREEN);
    text_set_content_by_strid(p_conn_status, IDS_CONNECT);
  }
  else
  {
    text_set_font_style(p_conn_status, FSI_RED, FSI_RED, FSI_RED);
    text_set_content_by_strid(p_conn_status, IDS_DISCONNECT);
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_conn_status, TRUE);
  }
  
}



void dis_conn_wifi(comm_dlg_data_t *p_wifi_dlg_data)
{
  ethernet_device_t* wifi_dev = NULL;
  net_conn_stats_t wifi_connt_stats = {0};
 
  wifi_connt_stats = ui_get_net_connect_status();
  if(!wifi_connt_stats.is_wifi_insert || !wifi_connt_stats.is_wifi_conn)
  {
    OS_PRINTF("#######WIFI not insert or not connected, no need to disconnect wifi#####\n");
    return ;
  }
    #ifndef WIN32
 wifi_dev = (ethernet_device_t *)get_wifi_handle();
	#endif
  //wifi_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_WIFI);
  if(wifi_dev == NULL)
  {
    OS_PRINTF("#######wifi_dev == NULL, no need to disconnect wifi#####\n");
    return ;
  }
  
  #ifndef WIN32
  set_wifi_disconnect(wifi_dev);
  ui_comm_dlg_open(p_wifi_dlg_data);
  wifi_connt_stats.is_wifi_conn = FALSE;
  ui_set_net_connect_status(wifi_connt_stats);

  paint_wifi_connect_status(wifi_connt_stats.is_wifi_conn, TRUE);
  #endif
}


BEGIN_KEYMAP(network_wifi_config_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
END_KEYMAP(network_wifi_config_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(network_wifi_config_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_network_config_exit)
  ON_COMMAND(MSG_INTERNET_PLUG_IN, on_network_config_plug_in)
  ON_COMMAND(MSG_INTERNET_PLUG_OUT, on_network_config_plug_out)
  ON_COMMAND(MSG_USB_INTERNET_PLUG_IN, on_network_config_usb_plug_in)
  ON_COMMAND(MSG_USB_INTERNET_PLUG_OUT, on_network_config_plug_out)

//  ON_COMMAND(MSG_WIFI_AP_DISCONNECT, on_disconn_wifi_over)
END_MSGPROC(network_wifi_config_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ping_wifi_test_keymap, ui_comm_static_keymap)
  
END_KEYMAP(ping_wifi_test_keymap, ui_comm_static_keymap)

BEGIN_MSGPROC(ping_wifi_test_proc, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_config_test_select)
END_MSGPROC(ping_wifi_test_proc, cont_class_proc)

BEGIN_MSGPROC(network_wifi_mode_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_network_mode_change)
END_MSGPROC(network_wifi_mode_proc, cbox_class_proc)

BEGIN_KEYMAP(connect_wifi_network_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(connect_wifi_network_keymap, NULL)

BEGIN_MSGPROC(connect_wifi_network_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_search_network)
END_MSGPROC(connect_wifi_network_proc, text_class_proc)



