/****************************************************************************

****************************************************************************/
#include "ui_common.h"


#include "ui_sub_menu.h"

#include "ethernet.h"
//#include "ui_wifi.h"
#include "wifi.h"
#include "network_monitor.h"
#include "sys_status.h"


enum play_cont_id
{
  IDC_VIDEO_MODE = 1,
  IDC_VIDEO_RESOLUTION,    
  IDC_ASPECT_MODE,
};

enum ip_ctrl_id
{
  IDC_CONNECT_TYPE = 1,
  IDC_CNFIG_MODE,
  IDC_IP_ADDRESS,
  IDC_NETMASK,
  IDC_GATEWAY,
  IDC_DNS_SERVER,
  IDC_DNS_SERVER2,
  IDC_MAC_ADDRESS,
  //IDC_PING_TEST,
  IDC_CONNECT_NETWORK,
  
  IDC_CONNECT_STATUS_TXT,
  IDC_CONNECT_STATUS,
};

enum search_local_msg
{
  MSG_SEARCH_MSG = MSG_LOCAL_BEGIN + 220,
  MSG_TIME_OUT,  
};

enum search_cont_id
{
  IDC_SEARCH_TP = 1,
  IDC_SIGNAL_STRENGTH,
  IDC_SIGNAL_QUALITY,
  IDC_SIGNAL_START,
  IDC_SEARCH_MSG,
  IDC_SEARCH_PBAR,
};


static  u16 item_strid[sub_mneu_item_count] = 
{
  IDS_NET_UPGRADE,
};

static  u16 main_mbox_strid[main_item_count] = 
{
  IDS_SYS_RESTORE,
  IDS_NET_CONFIG,
  IDS_UPG_CONFIG
};

static u8 main_mbox_bmpid_select[main_item_count] = 
{
  IM_SYSTEM_ICON_INFO_2,
  //IM_SYSTEM_ICON_TV_2,
  IM_SYSTEM_ICON_WEB_2,
  IM_SYSTEM_ICON_UPGRADE_2,
  //IM_SYSTEM_ICON_TV_2,
};

static u8 main_mbox_bmpid[main_item_count] = 
{
  IM_SYSTEM_ICON_INFO_1,
  //IM_SYSTEM_ICON_TV_1,
  IM_SYSTEM_ICON_WEB_1,
  IM_SYSTEM_ICON_UPGRADE_1,
  //IM_SYSTEM_ICON_TV_1,
};





/*
static list_xstyle_t slist_item_rstyle =
{
  RSI_PBACK,
  RSI_ITEM_2_SH,
  RSI_ITEM_2_HL,
  RSI_ITEM_2_HL,
  RSI_ITEM_2_HL,
};
*/

u16 up_cont_mbox_keymap(u16 key);

RET_CODE up_cont_mbox_proc(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);



u16 main_mbox_keymap(u16 key);
RET_CODE main_mbox_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 sub_menu_cont_keymap(u16 key);
RET_CODE sub_menu_cont_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

u16 factory_pwdlg_keymap(u16 key);
RET_CODE factory_pwdlg_proc(control_t *ctrl, u16 msg, u32 para1, u32 para2);

//net cont keymap and proc--------------------------------------------------------
u16 network_config_cont_keymap(u16 key);
u16 ping_test_keymap(u16 key);
u16 connect_network_keymap(u16 key);
u16 link_type_keymap(u16 key);
u16 ui_ipbox_keymap(u16 key);

static RET_CODE on_sub_menu_auto_connect(control_t *ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE network_config_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE ping_test_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE network_mode_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE connect_network_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE link_type_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE ui_ipbox_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
extern void init_ethernet_ethcfg(ethernet_cfg_t *ethcfg);
extern void auto_connect_ethernet();
extern void auto_connect_wifi();
extern void meset_livetv_curr_pg();
extern void ui_unrestore_active_stb_result();






static u8 ip_separator[IPBOX_MAX_ITEM_NUM] = {'.', '.', '.', ' '};
//static BOOL is_enable_dhcp = TRUE; 
static ethernet_cfg_t      ethcfg = {0};
static ethernet_cfg_t      static_wifi_ethcfg = {0};
static ip_address_set_t ss_ip = {{0}};
//static link_type_t link_type = LINK_TYPE_LAN;
static net_config_t g_net_config;
net_conn_stats_t eth_connt_stats;
extern ip_address_t primarydns_addr;
extern ip_address_t alternatedns_addr;
static BOOL is_eth_plug_inflag = FALSE;
BOOL is_link_type_changing_flag = FALSE;
//----------------------------------------------------------------------------

extern void do_cmd_connect_network(ethernet_cfg_t *ethcfg, ethernet_device_t * eth_dev);



extern net_conn_stats_t ui_get_net_connect_status(void);

extern void ui_set_net_connect_status( net_conn_stats_t net_conn_stats);

//sys cont code--------------------------------------------------------------


//---------------------------------------------------------------------------



static BOOL sg_b_reso_confirmed = FALSE;
void unset_video_res(void)
{
  sg_b_reso_confirmed = FALSE;
}

void set_video_res(void)
{
  sg_b_reso_confirmed = TRUE;
}



//net cont code--------------------------------------------------------------
static  void ssdata_ip_address_get()
{
  sys_status_get_ipaddress(&ss_ip);
}

static void ssdata_ip_address_set()
{
  sys_status_set_ipaddress(&ss_ip);
}

static void pop_up_connecting_dlg(u8 str_id)
{
  comm_dlg_data_t conn_dlg_data =
  {
    ROOT_ID_SUBMENU,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    str_id,
    0,
  };

  ui_comm_dlg_open_ex(&conn_dlg_data);
}

static void create_config_ctrl_items(control_t *p_menu)
{
  u8 ipaddress[32] = {0};
  control_t *p_ctrl[IP_CTRL_CNT] = { NULL };
  u16 network_str[CONFIG_MODE_TYPE_CNT] = { IDS_DHCP, IDS_STATIC_IP, IDS_VIRTUAL_DIAL};
  u16 network_str_wifi[CONFIG_MODE_TYPE_CNT - 1] = { IDS_DHCP, IDS_STATIC_IP};
  u16 type_str[2] = { IDS_WIRED, IDS_WIRELESS};
  u16 ctrl_str[IP_CTRL_CNT] =
  {
    IDS_CONNECTION ,IDS_CONNECTION_MODE,IDS_IP_ADDRESS,IDS_SUBNET_MASK,IDS_GATEWAY,
    IDS_DNS, IDS_DNS2, IDS_MAC_ADDRESS,0
  };
  ip_address_t temp_address = {0};
  u8 macaddr[128] = {0};
  u8 mac[6][2] = {{0},};
  u16 i, j, y;

  sys_status_get_net_config_info(&g_net_config);
  y = NETWORK_ITEM_Y;
  for(i = 0;i < IP_CTRL_CNT;i++)
  {
     switch(i)
      {
        case 0: 
          p_ctrl[i] = ui_comm_select_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_select_set_font(p_ctrl[i], FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
          ui_comm_ctrl_set_proc(p_ctrl[i], link_type_proc);
          ui_comm_select_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,2, CBOX_ITEM_STRTYPE_STRID, NULL);
          for(j = 0;j < 2;j++)
          {
             ui_comm_select_set_content(p_ctrl[i],j,type_str[j]);
          }
          if(g_net_config.link_type == LINK_TYPE_LAN)
          {
            ui_comm_select_set_focus(p_ctrl[i], 0);
          }
          else if(g_net_config.link_type == LINK_TYPE_WIFI)
          {
            ui_comm_select_set_focus(p_ctrl[i], 1);
          }
          ui_comm_select_create_droplist(p_ctrl[i], 2);
			break;		
        case 1:
          p_ctrl[i] = ui_comm_select_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_select_set_font(p_ctrl[i], FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
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
            
            ui_comm_ctrl_set_proc(p_ctrl[i],network_mode_proc);
     //       ui_comm_select_create_droplist(p_ctrl[i], CONFIG_MODE_TYPE_CNT - 1);
          }
          else
          {
            ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC, CONFIG_MODE_TYPE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
            for(j = 0;j < CONFIG_MODE_TYPE_CNT;j++)
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
            ui_comm_ctrl_set_proc(p_ctrl[i],network_mode_proc);
            
          }
          ui_comm_select_create_droplist(p_ctrl[i], CONFIG_MODE_TYPE_CNT);
			break;		
        case 2:
          p_ctrl[i] = ui_comm_ipedit_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_ipedit_set_font(p_ctrl[i], FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_ipaddress), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
          
          ui_comm_ctrl_set_keymap(p_ctrl[i], ui_ipbox_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], ui_ipbox_proc);
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
          ui_comm_ipedit_set_font(p_ctrl[i], FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_netmask), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
          ui_comm_ctrl_set_keymap(p_ctrl[i], ui_ipbox_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], ui_ipbox_proc);
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
          ui_comm_ipedit_set_font(p_ctrl[i], FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          memcpy(&temp_address, &(ss_ip.sys_gateway), sizeof(ip_address_t));
          ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
          ui_comm_ctrl_set_keymap(p_ctrl[i], ui_ipbox_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], ui_ipbox_proc);
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
          ui_comm_ipedit_set_font(p_ctrl[i], FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          sprintf(ipaddress, "%d.%d.%d.%d",primarydns_addr.s_b1,primarydns_addr.s_b2,primarydns_addr.s_b3,primarydns_addr.s_b4);
          OS_PRINTF("###ethernet auto connect  get dhcp dns_1 value is:%s####\n",ipaddress);
          if((0 == primarydns_addr.s_b1) && (0 == primarydns_addr.s_b2) && (0 == primarydns_addr.s_b3) && (0 == primarydns_addr.s_b1)) 
          {
            OS_PRINTF("\nss_ip.sys_dnsserver.s_a1 = [%d]\n", ss_ip.sys_dnsserver.s_a1);
            memcpy(&primarydns_addr, &(ss_ip.sys_dnsserver), sizeof(ip_address_t));
          }
          sprintf(ipaddress, "%d.%d.%d.%d",primarydns_addr.s_b1,primarydns_addr.s_b2,primarydns_addr.s_b3,primarydns_addr.s_b4);
          OS_PRINTF("###ethernet auto connect  get dhcp dns_1 value is:%s####\n",ipaddress);
          ui_comm_ipedit_set_address(p_ctrl[i], &primarydns_addr);
          ui_comm_ctrl_set_keymap(p_ctrl[i], ui_ipbox_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], ui_ipbox_proc);
          for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
          {
            ui_comm_ipedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, 
            ip_separator[j]);
          }
          ui_comm_ctrl_update_attr(p_ctrl[i], TRUE); 
        break;
        case 6:
          p_ctrl[i] = ui_comm_ipedit_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_ipedit_set_font(p_ctrl[i], FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
          ui_comm_ipedit_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ui_comm_ipedit_set_param(p_ctrl[i], 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
          sprintf(ipaddress, "%d.%d.%d.%d",alternatedns_addr.s_b1,alternatedns_addr.s_b2,alternatedns_addr.s_b3,alternatedns_addr.s_b4);
          OS_PRINTF("###ethernet auto connect  get dhcp dns_2 value is:%s####\n",ipaddress);
          if((0 == alternatedns_addr.s_b1) && (0 == alternatedns_addr.s_b2) && (0 == alternatedns_addr.s_b3) && (0 == alternatedns_addr.s_b4)) 
          {
            OS_PRINTF("\nss_ip.sys_dnsserver2.s_a1 = [%d]\n", ss_ip.sys_dnsserver2.s_a1);
            memcpy(&alternatedns_addr, &(ss_ip.sys_dnsserver2), sizeof(ip_address_t));
          }
          sprintf(ipaddress, "%d.%d.%d.%d",alternatedns_addr.s_b1,alternatedns_addr.s_b2,alternatedns_addr.s_b3,alternatedns_addr.s_b4);
          OS_PRINTF("###ethernet auto connect  get dhcp dns_2 value is:%s####\n",ipaddress);
          ui_comm_ipedit_set_address(p_ctrl[i], &alternatedns_addr);
          ui_comm_ctrl_set_keymap(p_ctrl[i], ui_ipbox_keymap);
          ui_comm_ctrl_set_proc(p_ctrl[i], ui_ipbox_proc);
          for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
          {
            ui_comm_ipedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, 
            ip_separator[j]);
          }
          ui_comm_ctrl_update_attr(p_ctrl[i], TRUE); 

        break;
        case 7:
          p_ctrl[i] = ui_comm_static_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_static_set_font(p_ctrl[i],FSI_WHITE_16,FSI_WHITE_16,FSI_WHITE_16);
          ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE);
          ui_comm_static_set_static_txt(p_ctrl[i], IDS_MAC_ADDRESS);//MAC ADDRESS
          #ifndef WIN32
          ethcfg.hwaddr[0] = sys_status_get_mac_by_index(0);
          ethcfg.hwaddr[1] = sys_status_get_mac_by_index(1);
          ethcfg.hwaddr[2] = sys_status_get_mac_by_index(2);
          ethcfg.hwaddr[3] = sys_status_get_mac_by_index(3);
          ethcfg.hwaddr[4] = sys_status_get_mac_by_index(4);
          ethcfg.hwaddr[5] = sys_status_get_mac_by_index(5);
/*            
          ethcfg.hwaddr[0] = 0xD8;
          ethcfg.hwaddr[1] = 0xA5;
          ethcfg.hwaddr[2] = 0xB7;
          ethcfg.hwaddr[3] = 0x80;
          ethcfg.hwaddr[4] = 0x01;
          ethcfg.hwaddr[5] = 0x00;
*/          
          OS_PRINTF("lou 9999999999999999999   %x:%x:%x:%x:%x:%x\n ",
                    ethcfg.hwaddr[0] ,ethcfg.hwaddr[1],
                    ethcfg.hwaddr[2] ,ethcfg.hwaddr[3],
                    ethcfg.hwaddr[4] ,ethcfg.hwaddr[5]);
//MT_ASSERT(0);
          for(j = 0; j < 6; j ++)  
          {
           sys_status_get_mac(j, mac[j]);
           strcat(macaddr,mac[j]);
           if(j != 5)
           {
           strcat(macaddr,":");
           }
          }
          macaddr[17]='\0';
         // strcpy(macaddr, "D8:A5:B7:80:01:01");
         #endif
          ui_comm_static_set_content_by_ascstr(p_ctrl[i], macaddr);
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 
        break;
		/*
        case 8:
          p_ctrl[i] = ui_comm_static_create(p_menu, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW, NETWORK_ITEM_RW);
          ui_comm_static_set_static_txt(p_ctrl[i], ctrl_str[i]);
          ctrl_set_keymap(p_ctrl[i], ping_test_keymap);
          ctrl_set_proc(p_ctrl[i], ping_test_proc);
        break;
        */
       case 8:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, IDC_CONNECT_TYPE + i, NETWORK_ITEM_X, y, NETWORK_ITEM_LW+NETWORK_ITEM_RW, COMM_CTRL_H, p_menu, 0);
        ctrl_set_keymap(p_ctrl[i], connect_network_keymap);
        ctrl_set_proc(p_ctrl[i], connect_network_proc);
        ctrl_set_rstyle(p_ctrl[i], RSI_PBACK, RSI_LIST_ITEM_1, RSI_PBACK);
        text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
        text_set_font_style(p_ctrl[i], FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        if(g_net_config.link_type == LINK_TYPE_LAN && g_net_config.config_mode != PPPOE)
        {
          text_set_content_by_strid(p_ctrl[i], IDS_CONNECT);
        }
		  else if(g_net_config.link_type == LINK_TYPE_LAN && g_net_config.config_mode == PPPOE)
        {
          text_set_content_by_strid(p_ctrl[i], IDS_PPPOE_CONN);
        }
        else if(g_net_config.link_type == LINK_TYPE_WIFI)
        {
          text_set_content_by_strid(p_ctrl[i], IDS_SEARCH);
        }
        
        break;
       
      }
      y += NETWORK_ITEM_H + NETWORK_ITEM_VGAP;
      ctrl_set_related_id(p_ctrl[i],
                        0,                       /* left */
                        (u8)((i - 1 + IP_CTRL_CNT)
                             % IP_CTRL_CNT + 1),    /* up */
                        0,                       /* right */
                        (u8)((i + 1) % IP_CTRL_CNT + 1));    /* down */
  }

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0);
  
}

void paint_connect_status(BOOL is_conn, BOOL is_paint)
{
  control_t *p_conn_status = NULL;
  control_t *p_net_cont;
  control_t *root = fw_find_root_by_id(ROOT_ID_SUBMENU);
  
  p_net_cont = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU, IDC_NET_CONT);
  
  if(root == NULL)
  {
    OS_PRINTF("########not need to paint connect status , is_conn == %d####\n", is_conn);
    return ;
  }
 // OS_PRINTF("########need to paint connect status , is_conn == %d####\n", is_conn);
  
  p_conn_status = ctrl_get_child_by_id(p_net_cont, IDC_CONNECT_STATUS);

  if(is_conn)
  {
    text_set_font_style(p_conn_status, FSI_GREEN_18, FSI_GREEN_18, FSI_GREEN_18);
    text_set_content_by_strid(p_conn_status, IDS_CONNECT);
  }
  else
  {
    text_set_font_style(p_conn_status, FSI_RED_18, FSI_RED_18, FSI_RED_18);
    text_set_content_by_strid(p_conn_status, IDS_NO_CONNECT);
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
  text_set_font_style(p_status_txt, FSI_WHITE_18, FSI_WHITE_18, FSI_WHITE_18);
  text_set_content_type(p_status_txt, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_status_txt, IDS_NETWORK_STATUS);

  p_status = ctrl_create_ctrl(CTRL_TEXT, IDC_CONNECT_STATUS, NETWORK_CONNECT_STATUSX, 
  NETWORK_CONNECT_STATUSY, NETWORK_CONNECT_STATUSW, NETWORK_CONNECT_STATUSH, p_menu, 0);
  
  ctrl_set_rstyle(p_status, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_status, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_status, TEXT_STRTYPE_STRID);

  eth_connt_stats = ui_get_net_connect_status();
  OS_PRINTF("################print connect status, %d,%d,%d,%d############", eth_connt_stats.is_eth_conn,eth_connt_stats.is_eth_insert,eth_connt_stats.is_wifi_conn,eth_connt_stats.is_wifi_insert);
  if(!eth_connt_stats.is_eth_conn && !eth_connt_stats.is_wifi_conn)
  {
    paint_connect_status(FALSE, FALSE);
  }
  else
  {
    paint_connect_status(TRUE, FALSE);
  }
  
}

static RET_CODE on_config_test_select(control_t *p_sub, u16 msg,
  u32 para1, u32 para2)
{
//  manage_open_menu(ROOT_ID_PING_TEST, 0, 0);
  
  return SUCCESS;
}

static void static_ip_to_dhcp()
{
  control_t *p_net_cont;
  ip_address_t temp_address = {0};
  control_t *p_ctrl[4] = {NULL};
  u16 i;

  p_net_cont = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU, IDC_NET_CONT);

  ssdata_ip_address_get();
  for(i = 0;i < 4;i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_IP_ADDRESS);
        memcpy(&temp_address, &(ss_ip.sys_ipaddress), sizeof(ip_address_t));
        ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
       break;
      case 1:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_NETMASK);
        memcpy(&temp_address, &(ss_ip.sys_netmask), sizeof(ip_address_t));
        ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
       break;
      case 2:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_GATEWAY);
        memcpy(&temp_address, &(ss_ip.sys_gateway), sizeof(ip_address_t));
        ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
       break;
/*       
      case 3:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_DNS_SERVER);
        memcpy(&temp_address, &(ss_ip.sys_dnsserver), sizeof(ip_address_t));
        ui_comm_ipedit_set_address(p_ctrl[i], &temp_address);
       break;   
*/       
     default:
        break;
    }
  }
  
}

static void update_ipbox_attr(BOOL is_enable)
{
  u16 i = 0;
  control_t *p_ctrl[3] = {NULL};
  control_t *p_net_cont;
  
  p_net_cont = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU, IDC_NET_CONT);

  for(i = 0;i < 3;i++)
  {
    p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, (u8)(IDC_IP_ADDRESS + i));
    ui_comm_ctrl_update_attr(p_ctrl[i], is_enable); 
    ctrl_paint_ctrl(p_ctrl[i], TRUE);
  }
}

static RET_CODE on_network_mode_change(control_t *p_cbox, u16 msg,
  u32 para1, u32 para2)
{
  u16 focus = 0;
  control_t *p_ctrl = NULL;
  control_t *p_net_cont;
  
  p_net_cont = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU, IDC_NET_CONT);
  
  focus = ui_comm_select_get_focus(p_cbox->p_parent);
  p_ctrl = ctrl_get_child_by_id(p_net_cont, IDC_CONNECT_NETWORK);

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


void get_addr_param(u8 *p_buffer, ip_address_t *p_addr)
{

  p_addr->s_b1 = p_buffer[0];
  p_addr->s_b2 = p_buffer[1];
  p_addr->s_b3 = p_buffer[2];
  p_addr->s_b4 = p_buffer[3];
}

void on_config_ip_update(ethernet_device_t * p_dev)
{
  ip_address_t addr = {0,};
  control_t *p_ctrl[4] = {NULL};
  control_t *p_net_cont;
  u8 ipaddress[32] = {0};
  u8 i;

  p_net_cont = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU, IDC_NET_CONT);

#ifndef WIN32
  u8 ipaddr[20] = {0};
  u8 netmask[20] = {0};
  u8 gw[20] = {0};
  u8 primarydns[20] = {0};
  u8 alternatedns[20] = {0};
  
  get_net_device_addr_info(p_dev, ipaddr, netmask, gw, primarydns, alternatedns);

#endif

  for(i = 0;i < 5;i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_IP_ADDRESS);
        #ifndef WIN32
        get_addr_param(ipaddr, (ip_address_t *)&addr);
        #endif
        ui_comm_ipedit_set_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_ipaddress), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
        OS_PRINTF("\n\n\n\n###update dhcp ip address value is:%s####\n\n\n\n\n",ipaddress);
        //ctrl_paint_ctrl(p_ctrl[i], TRUE);
       break;
      case 1:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_NETMASK);
        #ifndef WIN32
        get_addr_param(netmask, (ip_address_t *)&addr);
        #endif
        ui_comm_ipedit_set_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_netmask), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
        OS_PRINTF("###update dhcp netmask value is:%s####\n",ipaddress);
        //ctrl_paint_ctrl(p_ctrl[i], TRUE);
       break;
      case 2:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_GATEWAY);
        #ifndef WIN32
        get_addr_param(gw, (ip_address_t *)&addr);
        #endif
        ui_comm_ipedit_set_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
        OS_PRINTF("###update dhcp gateway value is:%s####\n",ipaddress);
        //ctrl_paint_ctrl(p_ctrl[i], TRUE);
       break;
      case 3:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_DNS_SERVER);
        ui_comm_ipedit_set_address(p_ctrl[i], &primarydns_addr);
        sprintf(ipaddress, "%d.%d.%d.%d",primarydns_addr.s_b1,primarydns_addr.s_b2,primarydns_addr.s_b3,primarydns_addr.s_b4);
        OS_PRINTF("###update dhcp dns server value is:%s####\n",ipaddress);
        //ctrl_paint_ctrl(p_ctrl[i], TRUE);
       break;   
      case 4:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_DNS_SERVER2);
        ui_comm_ipedit_set_address(p_ctrl[i], &alternatedns_addr);
        sprintf(ipaddress, "%d.%d.%d.%d",alternatedns_addr.s_b1,alternatedns_addr.s_b2,alternatedns_addr.s_b3,alternatedns_addr.s_b4);
        OS_PRINTF("###update dhcp dns server2 value is:%s####\n",ipaddress);
        //ctrl_paint_ctrl(p_ctrl[i], TRUE);
       break;   
     default:
        break;
    }
  }

  ctrl_paint_ctrl(ctrl_get_parent(p_net_cont), TRUE);
  ssdata_ip_address_set();
  
}


BOOL check_is_connect_ok(ethernet_device_t *p_eth_dev)
{
  u8 ipaddr[20] = {0};
  u8 netmask[20] = {0};
  u8 gw[20] = {0};
  u8 primarydns[20] = {0};
  u8 alternatedns[20] = {0};

  MT_ASSERT(p_eth_dev != NULL);
  #ifndef WIN32
  get_net_device_addr_info(p_eth_dev,ipaddr,netmask,gw,primarydns,alternatedns);
  #endif
  OS_PRINTF("###ipaddr %d.%d.%d.%d###\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
 // if(strcmp(ipaddr, "0.0.0.0") == 0)
  if((0 == ipaddr[0]) && (0 == ipaddr[1]) && (0 == ipaddr[2]) && (0 == ipaddr[3]))
  {
    OS_PRINTF("####get dhcp ipadress failed###\n");
    return FALSE;
  }
  else
  {
    OS_PRINTF("####get dhcp ipadress successfully###\n");
    return TRUE;
  }
  
}

static void config_network_ethcfg()
{
  control_t *p_net_cont;
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
  OS_PRINTF("@@@config_network_ethcfg, ethcfg.is_enabledhcp = %d@@@@\n",g_net_config.config_mode);

  p_net_cont = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU, IDC_NET_CONT);

  for(i = 0;i < 4;i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_IP_ADDRESS);
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
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_NETMASK);
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
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_GATEWAY);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
       
        ethcfg.gateway[0] = addr.s_b4;
        ethcfg.gateway[1] = addr.s_b3;
        ethcfg.gateway[2] = addr.s_b2;
        ethcfg.gateway[3] = addr.s_b1;
        OS_PRINTF("###gateway  is:%s####\n",ipaddress);
       break;
/*
      case 3:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_DNS_SERVER);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
       
        ethcfg.primaryDNS[0] = addr.s_b4;
        ethcfg.primaryDNS[1] = addr.s_b3;
        ethcfg.primaryDNS[2] = addr.s_b2;
        ethcfg.primaryDNS[3] = addr.s_b1;
        OS_PRINTF("###dns server is:%s####\n",ipaddress);
       break;
*/       
     default:
        break;
    }
  }
  
}

ethernet_cfg_t get_static_wifi_ethcfg()
{
  return static_wifi_ethcfg;
}

static void only_set_dns_server()
{
  control_t *p_ctrl[4] = { NULL };
  u8 ipaddress[32] = {0};
  ip_address_t addr;
  u16 i;
  control_t *p_net_cont;
  
  p_net_cont = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU, IDC_NET_CONT);

  for(i = 0;i < 2;i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_DNS_SERVER);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
        memcpy(&primarydns_addr, &(ss_ip.sys_dnsserver), sizeof(ip_address_t));
        OS_PRINTF("###static wifi dns server is:%s####\n",ipaddress);
      break;

      case 1:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_DNS_SERVER2);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_dnsserver2), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
        memcpy(&alternatedns_addr, &(ss_ip.sys_dnsserver2), sizeof(ip_address_t));
        OS_PRINTF("###static wifi dns server2 is:%s####\n",ipaddress);
      break;

      default:
        break;
    }
  }
}

static void set_static_wifi_ethcfg()
{
  control_t *p_ctrl[4] = { NULL };
  u8 ipaddress[32] = {0};
  ip_address_t addr;
  u16 i;
  control_t *p_net_cont;
  
  p_net_cont = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU, IDC_NET_CONT);
  
  static_wifi_ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
  static_wifi_ethcfg.is_enabledhcp = FALSE;

  for(i = 0;i < 4;i++)
  {
    switch(i)
    {
      case 0:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_IP_ADDRESS);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
      
        static_wifi_ethcfg.ipaddr[0] = addr.s_b4;
        static_wifi_ethcfg.ipaddr[1] = addr.s_b3;
        static_wifi_ethcfg.ipaddr[2] = addr.s_b2;
        static_wifi_ethcfg.ipaddr[3] = addr.s_b1;
        OS_PRINTF("###static wifi ip address  is:%s####\n",ipaddress);
       break;
      case 1:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_NETMASK);
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
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_GATEWAY);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_gateway), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
       
        static_wifi_ethcfg.gateway[0] = addr.s_b4;
        static_wifi_ethcfg.gateway[1] = addr.s_b3;
        static_wifi_ethcfg.gateway[2] = addr.s_b2;
        static_wifi_ethcfg.gateway[3] = addr.s_b1;
        OS_PRINTF("###static wifi gateway  is:%s####\n",ipaddress);
       break;
/*
      case 3:
        p_ctrl[i] = ctrl_get_child_by_id(p_net_cont, IDC_DNS_SERVER);
        ui_comm_ipedit_get_address(p_ctrl[i], &addr);
        memcpy(&(ss_ip.sys_dnsserver), &addr, sizeof(ip_addr_t));
        sprintf(ipaddress, "%d.%d.%d.%d",addr.s_b1,addr.s_b2,addr.s_b3,addr.s_b4);
       
        static_wifi_ethcfg.primaryDNS[0] = addr.s_b4;
        static_wifi_ethcfg.primaryDNS[1] = addr.s_b3;
        static_wifi_ethcfg.primaryDNS[2] = addr.s_b2;
        static_wifi_ethcfg.primaryDNS[3] = addr.s_b1;
        OS_PRINTF("###static wifi dns server is:%s####\n",ipaddress);
       break;
*/
     default:
        break;
    }
  }
  
}

static RET_CODE on_connect_network(control_t *p_sub, u16 msg,
  u32 para1, u32 para2)
{
  ethernet_device_t *eth_dev = NULL;
  
  eth_connt_stats = ui_get_net_connect_status();
  if(g_net_config.link_type == LINK_TYPE_WIFI)
  {
    if(!eth_connt_stats.is_wifi_insert)
    {
      OS_PRINTF("@@@wifi connect not ok@@@\n");
      ui_comm_cfmdlg_open_ex(NULL, IDS_WIFI_DEV_NOT_EXIST, NULL, 2000);
      return ERR_FAILURE;
    }
    if(g_net_config.config_mode == STATIC_IP)
    {
      set_static_wifi_ethcfg();
    }
    manage_open_menu(ROOT_ID_WIFI, para1, para2);
    return SUCCESS;
  }
  
  eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
  if(eth_dev == NULL)
  {
    OS_PRINTF("#####eth_dev  == NULL######\n");
    return ERR_FAILURE;
  }
  
  if(!eth_connt_stats.is_eth_insert)
  {
    OS_PRINTF("@@@ethernet connect not ok@@@\n");
    ui_comm_cfmdlg_open_ex(NULL, IDS_NET_NOT_CONNECT, NULL, 2000);
    return ERR_FAILURE;
  }
  if(g_net_config.config_mode == PPPOE)
  {
      //manage_open_menu(ROOT_ID_PPPOE_CONNECT, para1, para2);
      //OS_PRINTF("####will open PPPoE connect window####\n");
      return SUCCESS;
  }
//show dlg
  config_network_ethcfg();
  OS_PRINTF("@@@begin  do_cmd_connect_network@@@\n");
  do_cmd_connect_network(&ethcfg, eth_dev);
  ssdata_ip_address_set();
  
  pop_up_connecting_dlg(IDS_CONNECTING_ETH);

  is_link_type_changing_flag = FALSE;
  
  return SUCCESS;
  
}

static void dis_conn_ethernet()
{
  //net_conn_stats_t eth_connt_stats = {0};
  ethernet_device_t* eth_dev = NULL;
  comm_dlg_data_t net_eth_dlg_data =
  {
    ROOT_ID_SUBMENU,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    IDS_DISCONNECTING_ETH,
    4000,
  };

  /*eth_connt_stats = ui_get_net_connect_status();            // for quick switch
  if(eth_connt_stats.is_eth_conn == FALSE)
  {
    OS_PRINTF("##return from %s##\n", __FUNCTION__);
    return;
  }*/
  ui_comm_dlg_open_ex(&net_eth_dlg_data);
#ifndef WIN32
  eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
  if(eth_dev == NULL)
  {
    OS_PRINTF("#######eth_dev == NULL, no need to disconnect eth#####\n");
    return ;
  }
  lwip_netif_link_down(eth_dev);
#endif
  OS_PRINTF("##%s##\n", __FUNCTION__);
}

 void dis_conn_wifi(u32 time_out)
{
  ethernet_device_t* wifi_dev = NULL;
  net_conn_stats_t wifi_connt_stats = {0};

  comm_dlg_data_t net_wifi_dlg_data =
  {
    ROOT_ID_SUBMENU,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    IDS_DISCONNECTING_WIFI,
    time_out,
  };
  
  wifi_connt_stats = ui_get_net_connect_status();
  if(!wifi_connt_stats.is_wifi_insert /*|| !wifi_connt_stats.is_wifi_conn*/)
  {
    OS_PRINTF("#######WIFI not insert or not connected, no need to disconnect wifi#####\n");
    return ;
  }
  wifi_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_WIFI);
  if(wifi_dev == NULL)
  {
    OS_PRINTF("#######wifi_dev == NULL, no need to disconnect wifi#####\n");
    return ;
  }
  
  #ifndef WIN32
  set_wifi_disconnect(wifi_dev);
  ui_comm_dlg_open_ex(&net_wifi_dlg_data);
  wifi_connt_stats.is_wifi_conn = FALSE;
  ui_set_net_connect_status(wifi_connt_stats);
 // mtos_task_sleep(4000);
 // paint_connect_status(wifi_connt_stats.is_wifi_conn, TRUE);
  #endif
}

static RET_CODE on_link_type_changed(control_t *p_cbox, u16 msg,u32 para1, u32 para2)
{
  control_t *p_link_type = NULL;
  control_t *p_connect = NULL;
  control_t *p_config_mode = NULL;
  control_t *p_cont = NULL;
  u16 network_str[CONFIG_MODE_TYPE_CNT] = { IDS_DHCP, IDS_STATIC_IP, IDS_VIRTUAL_DIAL };
  u16 network_str_wifi[CONFIG_MODE_TYPE_CNT - 1] = { IDS_DHCP, IDS_STATIC_IP};
  u16 focus = 0;
  u16 j;

  p_link_type = ctrl_get_parent(p_cbox);
  focus = ui_comm_select_get_focus(p_link_type);
  p_connect = ctrl_get_child_by_id(p_link_type->p_parent, IDC_CONNECT_NETWORK);
  p_config_mode = ctrl_get_child_by_id(p_link_type->p_parent, IDC_CNFIG_MODE);
  p_cont = ctrl_get_parent(p_link_type->p_parent);
  
  if(p_connect == NULL || p_config_mode == NULL)
  {
    return ERR_FAILURE;
  }
   
  if(1 == focus)
  {
    if(LINK_TYPE_WIFI == g_net_config.link_type)
    {
      return SUCCESS;
    }
    OS_PRINTF("\n\n 111111 zhuzhu is_link_type_changing_flag");
    text_set_content_by_strid(p_connect, IDS_SEARCH);
    g_net_config.link_type = LINK_TYPE_WIFI;
    ui_comm_select_set_param(p_config_mode, TRUE, CBOX_WORKMODE_STATIC, CONFIG_MODE_TYPE_CNT - 1, CBOX_ITEM_STRTYPE_STRID, NULL);
    for(j = 0;j < CONFIG_MODE_TYPE_CNT - 1;j++)
    {
       ui_comm_select_set_content(p_config_mode, j, network_str_wifi[j]);
    }
    if(g_net_config.config_mode == STATIC_IP)
    {
      ui_comm_select_set_focus(p_config_mode, 1);
    }
    else
    {
      ui_comm_select_set_focus(p_config_mode, 0);
    }
    if(!is_link_type_changing_flag)
    {
      is_link_type_changing_flag = TRUE;
      dis_conn_ethernet();
      is_link_type_changing_flag = FALSE;
      on_sub_menu_auto_connect(p_cont, MSG_TIME_OUT, 0, 0);
    } 
   // ui_comm_select_create_droplist(p_config_mode, CONFIG_MODE_TYPE_CNT - 1);
  }
  else
  {
    if(LINK_TYPE_LAN == g_net_config.link_type)
    {
      return SUCCESS;
    }
    text_set_content_by_strid(p_connect, IDS_CONNECT);
    g_net_config.link_type = LINK_TYPE_LAN;
    ui_comm_select_set_param(p_config_mode, TRUE, CBOX_WORKMODE_STATIC, CONFIG_MODE_TYPE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
    for(j = 0;j < CONFIG_MODE_TYPE_CNT;j++)
    {
       ui_comm_select_set_content(p_config_mode, j, network_str[j]);
    }
    if(g_net_config.config_mode == DHCP)
    {
      ui_comm_select_set_focus(p_config_mode, 0);
    }
    else if(g_net_config.config_mode == STATIC_IP)
    {
      ui_comm_select_set_focus(p_config_mode, 1);
    }
    else 
    {
      ui_comm_select_set_focus(p_config_mode, 2);
    }
    if(!is_link_type_changing_flag)
    {
      is_link_type_changing_flag = TRUE;
      //if(is_wifi_disconnect())
      {
        is_link_type_changing_flag = FALSE;
        ui_comm_dlg_close_ex();
        on_sub_menu_auto_connect(p_cont, MSG_TIME_OUT, 0, 0);
      }
      //else
      //{
        //dis_conn_wifi(0);
      //}
    }
   // ui_comm_select_create_droplist(p_config_mode, CONFIG_MODE_TYPE_CNT);
  }
  ctrl_paint_ctrl(p_connect, TRUE);
  ctrl_paint_ctrl(p_config_mode, TRUE);
  sys_status_set_net_config_info(&g_net_config);
  sys_status_save();
  return SUCCESS;
}

static RET_CODE on_ipbox_select(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  if(ipbox_is_on_edit(p_ctrl))
  {
    if(!ipbox_is_invalid_address(p_ctrl))
    {
      only_set_dns_server();
      ssdata_ip_address_set();
      sys_status_save();
    }
  }
  return ERR_NOFEATURE;
}
static RET_CODE on_ipbox_unselect(control_t *p_ctrl,
                                 u16 msg,
                                 u32 para1,
                                 u32 para2)
{
  RET_CODE ret = SUCCESS;

  if(ipbox_is_on_edit(p_ctrl))
  {
    if(ipbox_is_invalid_address(p_ctrl))
    {
      ret = ctrl_process_msg(p_ctrl, MSG_SELECT, para1, para2);
    }
    else
    {
      ret = ctrl_process_msg(p_ctrl, MSG_SELECT, para1, para2);
    }
  }
  else
  {
    ret = ERR_NOFEATURE;
  }

  return ret;
}


static RET_CODE on_ipaddress_out_range(control_t *p_ctrl,
                                  u16 msg,
                                  u32 para1,
                                  u32 para2)
{
  ui_comm_cfmdlg_open_ex(NULL, IDS_MSG_OUT_OF_RANGE, NULL, 0);
  return SUCCESS;
}

static RET_CODE on_active_ctrl_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  u8 ctrl_id = 0;
  control_t *p_main_mbox;

  p_main_mbox = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU, IDC_MAIN_MBOX);

  ctrl_id = ctrl_get_ctrl_id(ctrl_get_parent(p_cont));

  if(is_eth_plug_inflag)
  {
    return ERR_NOFEATURE;
  }

  OS_PRINTF("\n\n\n\n ctrl_id = [%d] \n\n\n", ctrl_id);
  if((IDC_CONNECT_TYPE == ctrl_id)||(IDC_CNFIG_MODE == ctrl_id))
  {
    if(MSG_INCREASE == msg)
    {
      return SUCCESS;
    }

    if(cbox_droplist_is_opened(p_cont))
    {
      return SUCCESS;
    }
  }
  
  if((ctrl_id > IDC_CNFIG_MODE)&&(ctrl_id < IDC_MAC_ADDRESS))
  {
    if(ipbox_is_on_edit(p_cont))
    {
      return ERR_NOFEATURE;
    }

    if(MSG_FOCUS_RIGHT == msg)
    {
      return SUCCESS;
    }
  }
  
  ctrl_process_msg(ctrl_get_parent(p_cont), MSG_LOSTFOCUS, 0, 0);
  ctrl_process_msg(p_main_mbox, MSG_GETFOCUS, 0, 0);
  
  config_network_ethcfg();

  ssdata_ip_address_set();
  sys_status_set_net_config_info(&g_net_config);
  sys_status_save();
  
  ctrl_paint_ctrl(ctrl_get_parent(p_cont), TRUE);
  ctrl_paint_ctrl(p_main_mbox, TRUE);
  
  OS_PRINTF("@@@@@@@@@@@ network_cont_hide @@@@@@@@@\n");
  
  return SUCCESS;
}

static RET_CODE on_network_config_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
   u8 ctrl_id;
   control_t *p_main_mbox;
   control_t *p_select_cont, *p_active;

   p_main_mbox = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU, IDC_MAIN_MBOX);
   
   if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
   {
      OS_PRINTF("###disconnecting not exit will return from on_network_config_exit####\n");
      return ERR_FAILURE;
   }
  config_network_ethcfg();
  ssdata_ip_address_set();
  sys_status_set_net_config_info(&g_net_config);
  sys_status_save();
  p_select_cont = ctrl_get_active_ctrl(p_cont);
  ctrl_id = ctrl_get_ctrl_id(p_select_cont);
  p_active = ctrl_get_active_ctrl(p_select_cont);
  if((IDC_CONNECT_TYPE == ctrl_id)||(IDC_CNFIG_MODE == ctrl_id))
  {
    if(cbox_droplist_is_opened(p_active))
    {
      cbox_close_droplist(p_active, TRUE);
    }
  }
  else if((ctrl_id > IDC_CNFIG_MODE)&&(ctrl_id < IDC_MAC_ADDRESS))
  {
    ipbox_exit_edit(p_active);
  }
  
  //manage_close_menu(ROOT_ID_NETWORK_CONFIG, 0, 0);
  ctrl_process_msg(p_cont, MSG_LOSTFOCUS, 0, 0);
  ctrl_process_msg(p_main_mbox, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_cont, TRUE);
  ctrl_paint_ctrl(p_main_mbox, TRUE);
  
  OS_PRINTF("@@@@@@@@@@@ network_cont_hide @@@@@@@@@\n");
  
  return SUCCESS;
}

static void  ui_submenu_set_linktype(control_t *p_cont,u8 type)
{
  control_t *p_net_cont,*p_net_cont_ctrl;
  p_net_cont = ctrl_get_child_by_id(p_cont, IDC_NET_CONT);
  p_net_cont_ctrl = ctrl_get_child_by_id(p_net_cont, IDC_CONNECT_TYPE);
 // on_link_type_changed(p_net_cont_ctrl,0,0,0);
  is_eth_plug_inflag = TRUE;
  link_type_proc(ctrl_get_child_by_id(p_net_cont_ctrl, IDC_COMM_CTRL), MSG_INCREASE, 0,0);
  mtos_printk("\n\nlou 333333333 \n\n");
  is_eth_plug_inflag = FALSE;
}


static RET_CODE on_network_config_plug_in(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  OS_PRINTF("@@@@@@@@@@@on_network_config_plug_in start @@@@@@@@@\n");
  ui_comm_dlg_close_ex();
  if(NULL != fw_find_root_by_id(ROOT_ID_WIFI_LINK_INFO))
  {
    manage_close_menu(ROOT_ID_WIFI_LINK_INFO, 0, 0);
  }
  if(NULL != fw_find_root_by_id(ROOT_ID_WIFI))
  {
    manage_close_menu(ROOT_ID_WIFI, 0, 0);
  }
  eth_connt_stats = ui_get_net_connect_status();
  net_config_t net_config = {0,};
  eth_connt_stats.is_eth_insert = TRUE;
  ui_set_net_connect_status(eth_connt_stats);
  sys_status_get_net_config_info(&net_config);
  if(net_config.link_type == LINK_TYPE_LAN)
  {
    auto_connect_ethernet();
  }
  else
  {
     ui_submenu_set_linktype(p_cont,LINK_TYPE_LAN);
      //auto_connect_ethernet();
  }
  
  OS_PRINTF("@@@@@@@@@@@on_network_config_plug_in @@@@@@@@@\n");
  
  return SUCCESS;
}

static RET_CODE on_network_config_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  net_conn_stats_t net_connt_stats;
  net_connt_stats = ui_get_net_connect_status();
  net_connt_stats.is_eth_conn = FALSE;
  net_connt_stats.is_eth_insert = FALSE;

  eth_connt_stats.is_eth_insert = FALSE;
  eth_connt_stats.is_eth_conn = FALSE;
  if (!net_connt_stats.is_wifi_conn)
  {
    paint_connect_status(net_connt_stats.is_eth_conn, TRUE);
  }
  ui_set_net_connect_status(net_connt_stats);
  OS_PRINTF("@@@@@@@@@@@on_network_config_plug_out is_eth_conn = %d,is_eth_insert = %d@@@@@@@@@\n",net_connt_stats.is_eth_conn, net_connt_stats.is_eth_insert);
  
  return SUCCESS;
}
#if 0
static RET_CODE on_disconn_wifi_over(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  net_conn_stats_t wifi_stats = {0};
  control_t *root = NULL;
  
  OS_PRINTF("###before change link type from wifi to ethernet, disconnect  over###\n");
  ui_comm_dlg_close_ex();
  root = fw_find_root_by_id(ROOT_ID_WIFI_LINK_INFO);
  if(root)
  {
    OS_PRINTF("###WIFI link info ui exist,should ignore the disconnect msg at networkconfig UI###\n");
    return ERR_FAILURE;
  }
  wifi_stats = ui_get_net_connect_status();
  if(!wifi_stats.is_wifi_conn)
  {
    OS_PRINTF("###WIFI status is disconnect,should ignore the disconnect msg at networkconfig UI###\n");
    return ERR_FAILURE;
  }
  
  wifi_stats.is_wifi_conn = FALSE;
  ui_set_net_connect_status(wifi_stats);
  paint_connect_status(wifi_stats.is_wifi_conn, TRUE);
  return SUCCESS;
}
#endif
//---------------------------------------------------------------------------


//upgrade cont code----------------------------------------------------------
static RET_CODE on_item_select(control_t *ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  u16 focus;
  u16 item_sting_id;
  /*
  comm_pwdlg_data_t factory_pwdlg_data =
  {
    ROOT_ID_SUBMENU,
    PWD_DLG_FOR_CHK_X,
    PWD_DLG_FOR_CHK_Y,
    IDS_PASSWORD,
    factory_pwdlg_keymap,
    factory_pwdlg_proc,
  };
*/
  focus = mbox_get_focus(ctrl);

  item_sting_id = item_strid[focus];
  switch (item_sting_id)
  {/*
    case IDS_USB_UPGRADE:
	//if(ui_get_usb_status())
	if(0)
	{
        //manage_open_menu(ROOT_ID_UPGRADE_BY_USB, 0, 0);
	}
	else
        mbox_set_item_status(ctrl, 1 , MBOX_ITEM_DISABLED);    
      break;*/
    case IDS_NET_UPGRADE:
      manage_open_menu(ROOT_ID_NETWORK_UPGRADE, 0, 0);
	    break;    
   // case IDS_RESTORE_FACTORY_SETTINGS:
      //ui_comm_pwdlg_open(&factory_pwdlg_data);
	//    break;    
    default:
	    break;
  }

  return SUCCESS;
}



static RET_CODE on_item_change_focus(control_t *ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  mbox_class_proc(ctrl, msg, para1, para2);
  return SUCCESS;
}


static RET_CODE on_item_paint(control_t *ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  // check item attr before painting
  // means pass the msg to base
  return ERR_NOFEATURE;
}

static RET_CODE on_item_exit(control_t *ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  control_t *p_main_mbox;

  p_main_mbox = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU, IDC_MAIN_MBOX);

  ctrl_process_msg(ctrl, MSG_LOSTFOCUS, 0, 0);
  ctrl_process_msg(p_main_mbox, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl, TRUE);
  ctrl_paint_ctrl(p_main_mbox, TRUE);
  
  return SUCCESS;
}


//---------------------------------------------------------------------------




/*
static void delete_all_prog(void)
{
  ui_dbase_delete_all_pg();
  sys_status_check_group();
  sys_status_save();

  book_check_node_on_delete();
}
*/


RET_CODE open_sub_menu(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_sys_cont;
  control_t *p_net_cont;
  control_t *p_upgrade_cont, *p_upgrade_mbox;
  control_t *p_main_mbox;
  control_t *p_title1, *p_title2;
  u16 i;

  ssdata_ip_address_get();
  // create container
  p_cont = fw_create_mainwin(ROOT_ID_SUBMENU,
                                  SUBMENU_CONT_X, SUBMENU_CONT_Y,
                                  SUBMENU_CONT_W, SUBMENU_CONT_H,
                                  ROOT_ID_INVALID, 0,
                                  OBJ_ATTR_ACTIVE, 0);
 OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);
 if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_MAIN_BG, RSI_MAIN_BG, RSI_MAIN_BG);
  ctrl_set_keymap(p_cont, sub_menu_cont_keymap);
  ctrl_set_proc(p_cont, sub_menu_cont_proc);
  ctrl_set_style(p_cont, STL_EX_WHOLE_HL);
OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);

  //creat main title
  p_title1 = ctrl_create_ctrl(CTRL_TEXT, IDC_MAIN_TITLE_TXT,
                               MAIN_TITLE_TXT_X, MAIN_TITLE_TXT_Y, 
                               MAIN_TITLE_TXT_W, MAIN_TITLE_TXT_H,
                               p_cont, 0);
  
  ctrl_set_rstyle(p_title1, RSI_BOX3_TITLE, RSI_BOX3_TITLE, RSI_BOX3_TITLE);
  text_set_align_type(p_title1, STL_CENTER| STL_VCENTER);
  text_set_font_style(p_title1, FSI_WHITE_LARGE, FSI_WHITE_LARGE, FSI_WHITE_LARGE);
  text_set_content_type(p_title1, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title1, IDS_SYS_RESTORE);
OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);

  //creat title
  p_title2 = ctrl_create_ctrl(CTRL_TEXT, IDC_TITLE_TXT,
                               SUB_TITLE_TXT_X, SUB_TITLE_TXT_Y, 
                               SUB_TITLE_TXT_W, SUB_TITLE_TXT_H,
                               p_cont, 0);
  ctrl_set_rstyle(p_title2, RSI_BOX3_TITLE, RSI_BOX3_TITLE, RSI_BOX3_TITLE);
  text_set_align_type(p_title2, STL_CENTER| STL_VCENTER);
  text_set_font_style(p_title2, FSI_WHITE_18, FSI_WHITE_18, FSI_WHITE_18);
  text_set_content_type(p_title2, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title2, main_mbox_strid[0]);

OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);
  
  //creat main mbox
  p_main_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_MAIN_MBOX,
                                  MAIN_MBOX_X, MAIN_MBOX_Y, 
                                  MAIN_MBOX_W, MAIN_MBOX_H, 
                                  p_cont, 0);
 OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);
 
  ctrl_set_rstyle(p_main_mbox, RSI_BOX2, RSI_BOX2, RSI_BOX2);
 OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);
 
  ctrl_set_keymap(p_main_mbox, main_mbox_keymap);
  ctrl_set_proc(p_main_mbox, main_mbox_proc);
  
  ctrl_set_mrect(p_main_mbox, SUBMENU_ITEM_X, SUBMENU_ITEM_Y, (SUBMENU_ITEM_W + SUBMENU_ITEM_X),
    (SUBMENU_ITEM_H + SUBMENU_ITEM_Y));
  
  mbox_set_item_interval(p_main_mbox, SUBMENU_MBOX_HGAP, SUBMENU_MBOX_VGAP);
  
  mbox_set_item_rstyle(p_main_mbox, RSI_SUBMENU_ITEM_HL,
                      RSI_SUBMENU_ITEM_SH, RSI_SUBMENU_ITEM_GRAY);
OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);
  
  mbox_set_string_fstyle(p_main_mbox, FSI_WHITE_18, FSI_WHITE_18, FSI_WHITE_18);
  mbox_enable_icon_mode(p_main_mbox, TRUE);
  mbox_enable_string_mode(p_main_mbox, TRUE);

  mbox_set_icon_offset(p_main_mbox, SUBMENU_MBOX_OX_ICON, 0);
  mbox_set_string_offset(p_main_mbox, SUBMENU_MBOX_OX_STR, 0);
  mbox_set_icon_align_type(p_main_mbox, STL_LEFT | STL_VCENTER);
  mbox_set_string_align_type(p_main_mbox, STL_RIGHT| STL_VCENTER);
  mbox_set_content_strtype(p_main_mbox, MBOX_STRTYPE_STRID);

  mbox_set_count(p_main_mbox, main_item_count, 1, main_item_count);
  mbox_set_focus(p_main_mbox, para1);
  ctrl_set_style(p_main_mbox, STL_EX_ALWAYS_HL);
OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);

  for (i = 0; i < main_item_count; i++)
  {
    mbox_set_content_by_strid(p_main_mbox, i, main_mbox_strid[i]);
    mbox_set_content_by_icon(p_main_mbox, i, main_mbox_bmpid_select[i], main_mbox_bmpid[i]);
  }
OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);

  //creat cont1  --> sys cont-----------------------------------------
  p_sys_cont =
    ctrl_create_ctrl(CTRL_CONT, IDC_SYS_CONT, 
    PLAY_CONT_X, PLAY_CONT_Y, PLAY_CONT_W, PLAY_CONT_H, p_cont, 0);
  
  if( p_sys_cont )
  {
    control_t* p_txt;
    u8 m;
    u16 tips[] =
      {
        IDS_UPG_TIP_0,
        IDS_UPG_TIP_1,
        IDS_UPG_TIP_2,
        IDS_UPG_TIP_3,
        IDS_UPG_TIP_4,
        IDS_UPG_TIP_5,
      };

    for( m = 0; m<sizeof(tips)/sizeof(tips[0]); m++ )
    {
      p_txt = ctrl_create_ctrl(CTRL_TEXT,m,30,30+m*50, 600, 40, p_sys_cont, 0);
      ctrl_set_rstyle(p_txt, RSI_PBACK, RSI_PBACK, RSI_PBACK);
      text_set_align_type(p_txt, STL_LEFT| STL_VCENTER);
      text_set_font_style(p_txt, FSI_WHITE_LARGE, FSI_WHITE_LARGE, FSI_WHITE_LARGE);
      text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
      text_set_content_by_strid(p_txt, tips[m]);
    }

    ctrl_set_rstyle(p_sys_cont, RSI_BOX2, RSI_BOX2, RSI_BOX2);
    ctrl_set_attr(p_sys_cont, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_sys_cont, OBJ_STS_HIDE);


    OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);
  }
  else
  {
    return ERR_FAILURE;
  }
  

  //creat cont3  --> net cont---------------------------------------------
  p_net_cont = ctrl_create_ctrl(CTRL_CONT, IDC_NET_CONT, 
                                 PLAY_CONT_X, PLAY_CONT_Y, 
                                 PLAY_CONT_W, PLAY_CONT_H, 
                                 p_cont, 0);

  if(p_net_cont == NULL)
  {
    return ERR_FAILURE;
  }
OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);

  ctrl_set_rstyle(p_net_cont, RSI_BOX2, RSI_BOX2, RSI_BOX2);
  
  ctrl_set_keymap(p_net_cont, network_config_cont_keymap);
  ctrl_set_proc(p_net_cont, network_config_cont_proc);
  
  //create config items
  create_config_ctrl_items(p_net_cont);
  
  //create network connect status
  create_connect_status_ctrl(p_net_cont);

  ctrl_set_attr(p_net_cont, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_net_cont, OBJ_STS_HIDE);
  //----------------------------------------------------------------------

  //creat cont4  --> up cont----------------------------------------
  p_upgrade_cont = ctrl_create_ctrl(CTRL_CONT, IDC_UPGRADE_CONT, 
                                     PLAY_CONT_X, PLAY_CONT_Y, 
                                     PLAY_CONT_W, PLAY_CONT_H, 
                                     p_cont, 0);
  if(p_upgrade_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_upgrade_cont, RSI_BOX2, RSI_BOX2, RSI_BOX2);
  
OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);
  // create mbox for items
  p_upgrade_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_UPGRADE_MBOX,
    SUBMENU_MBOX_X, SUBMENU_MBOX_Y, SUBMENU_MBOX_W, SUBMENU_MBOX_H, p_upgrade_cont, 0);
  
  ctrl_set_rstyle(p_upgrade_mbox, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  
  ctrl_set_keymap(p_upgrade_mbox, up_cont_mbox_keymap);
  ctrl_set_proc(p_upgrade_mbox, up_cont_mbox_proc);
  
  //ctrl_set_mrect(p_upgrade_mbox, SUBMENU_ITEM_X, SUBMENU_ITEM_Y, (SUBMENU_ITEM_W + SUBMENU_ITEM_X),
    //((SUBMENU_ITEM_H*sub_mneu_item_count+SUBMENU_MBOX_VGAP*(sub_mneu_item_count-1)) + SUBMENU_ITEM_Y));
  
  mbox_set_item_interval(p_upgrade_mbox, SUBMENU_MBOX_HGAP, SUBMENU_MBOX_VGAP);
  mbox_set_item_rstyle(p_upgrade_mbox, RSI_LIST_ITEM_1,
                                         RSI_PBACK, RSI_PBACK);
  mbox_set_string_fstyle(p_upgrade_mbox, FSI_WHITE_16, FSI_WHITE_16, FSI_GRAY_16);
  mbox_enable_icon_mode(p_upgrade_mbox, FALSE);
  mbox_enable_string_mode(p_upgrade_mbox, TRUE);
  mbox_set_string_offset(p_upgrade_mbox, SUBMENU_MBOX_OX_STR, 0);
  mbox_set_string_align_type(p_upgrade_mbox, STL_CENTER | STL_VCENTER);
  mbox_set_content_strtype(p_upgrade_mbox, MBOX_STRTYPE_STRID);

  mbox_set_count(p_upgrade_mbox, up_cont_item_count, 1, up_cont_item_count);
  mbox_set_focus(p_upgrade_mbox, para1);
  //ctrl_set_style(p_upgrade_mbox, STL_EX_ALWAYS_HL);
  for (i = 0; i < up_cont_item_count; i++)
  {
    mbox_set_content_by_strid(p_upgrade_mbox, (u8)i, item_strid[i]);
  }
  
  //if(FALSE == ui_get_usb_status())
  if(1)
  {
    mbox_set_item_status(p_upgrade_mbox, 1 , MBOX_ITEM_DISABLED);    
  }
  
  ctrl_set_attr(p_upgrade_cont, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_upgrade_cont, OBJ_STS_HIDE);

OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);

OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);

  //-------------------------------------------------------------------
  
  if(0 == para1)
    ctrl_set_sts(p_sys_cont, OBJ_STS_SHOW);
  else if(1 == para1)
    ctrl_set_sts(p_net_cont, OBJ_STS_SHOW);
  else if(2 == para1)
    ctrl_set_sts(p_upgrade_cont, OBJ_STS_SHOW);
  else
    ctrl_set_sts(p_sys_cont, OBJ_STS_SHOW);
  

OS_PRINTF("%s(), %d\n", __FUNCTION__,__LINE__);
  ctrl_default_proc(p_main_mbox, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
  return SUCCESS;
}

static RET_CODE on_main_mbox_select(control_t *ctrl, u16 msg,
                                     u32 para1, u32 para2)
{
  u16 focus;
  control_t *p_cont;
  control_t *p_sys_cont;
  control_t *p_net_cont, *p_net_cont_ctrl;
  control_t *p_upgrade_cont, *p_upgrade_mbox;
  //control_t *p_main_mbox;
  //av_set_t av_set;
  
  focus = mbox_get_focus(ctrl);
  p_cont = ctrl_get_parent(ctrl);
  p_sys_cont = ctrl_get_child_by_id(p_cont, IDC_SYS_CONT);
  p_net_cont = ctrl_get_child_by_id(p_cont, IDC_NET_CONT);
  p_upgrade_cont = ctrl_get_child_by_id(p_cont, IDC_UPGRADE_CONT);

  p_net_cont_ctrl = ctrl_get_child_by_id(p_net_cont, IDC_CONNECT_TYPE);
  p_upgrade_mbox = ctrl_get_child_by_id(p_upgrade_cont, IDC_UPGRADE_MBOX);
  

  //sys_status_get_av_set(&av_set);

  ctrl_process_msg(ctrl, MSG_LOSTFOCUS, 0, 0);
  switch (focus)
  {
    case 0:
      break;
    case 1:
      ctrl_process_msg(p_net_cont_ctrl, MSG_GETFOCUS, 0, 0);
      //ui_comm_select_set_focus(p_play_cont_ctrl[0], av_set.tv_mode);
      //ui_comm_select_set_focus(p_play_cont_ctrl[1], av_set.tv_resolution);
      //ui_comm_select_set_focus(p_play_cont_ctrl[2], av_set.tv_ratio); 
	    break;    
    case 2:
      ctrl_process_msg(p_upgrade_mbox, MSG_GETFOCUS, 0, 0);
      mbox_set_focus(p_upgrade_mbox, 0);
	    break;
    default:
	    break;
  }

  ctrl_paint_ctrl(ctrl_get_child_by_id(p_cont, (u8)(IDC_SYS_CONT + focus)), TRUE);
  
  return SUCCESS;
}

static RET_CODE on_main_mbox_change_focus(control_t *ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  u16 focus;
  u16 i;
  control_t *p_cont;
  control_t *p_pre_cont, *p_crn_cont;
  control_t *p_title;
   
  p_cont = ctrl_get_parent(ctrl);
  p_title = ctrl_get_child_by_id(p_cont, IDC_TITLE_TXT);

  for(i = 0; i < main_item_count; i++)
  {
    p_pre_cont = ctrl_get_child_by_id(p_cont, (u8)(IDC_SYS_CONT + i));
    if(OBJ_STS_SHOW == ctrl_get_sts(p_pre_cont))
    {
      ctrl_set_sts(p_pre_cont, OBJ_STS_HIDE);
    }
  }   
  mbox_class_proc(ctrl, msg, para1, para2);
  focus = mbox_get_focus(ctrl);

  p_crn_cont = ctrl_get_child_by_id(p_cont, (u8)(IDC_SYS_CONT + focus));
  ctrl_set_sts(p_crn_cont, OBJ_STS_SHOW);


  text_set_content_by_strid(p_title, main_mbox_strid[focus]);

  ctrl_paint_ctrl(p_cont, TRUE);
  return SUCCESS;
}

static RET_CODE on_main_mbox_paint(control_t *ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  // check item attr before painting
  // means pass the msg to base
  return ERR_NOFEATURE;
}

static RET_CODE on_main_mbox_exit(control_t *ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  return SUCCESS;
}

static RET_CODE on_paint_sub_menu(control_t *ctrl, u16 msg,
                               u32 para1, u32 para2)
{
  cont_class_proc(ctrl, msg, para1, para2);
  return SUCCESS;
}

//static RET_CODE on_sub_menu_plug_out(control_t *ctrl, u16 msg,
 //                              u32 para1, u32 para2)
//{
//  if(g_entry->title_strid == IDS_MEDIA_CENTER)
//  {
 //   manage_close_menu(ROOT_ID_SUBMENU, 0, 0);
 // }
 //ctrl_paint_ctrl(ctrl, TRUE);
//  return SUCCESS;
//}

static RET_CODE on_sub_menu_destroy(control_t *ctrl, u16 msg, 
                               u32 para1, u32 para2)
{
  return ERR_NOFEATURE;
}

static RET_CODE on_sub_menu_auto_connect(control_t *ctrl, u16 msg, 
                               u32 para1, u32 para2)
{
  wifi_info_t p_wifi_info = {{0},{0}};
  ethernet_device_t *eth_dev = NULL;
  eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);

  eth_connt_stats = ui_get_net_connect_status();

  
  if(eth_dev == NULL)
  {
    OS_PRINTF("#####eth_dev  == NULL######\n");
    return ERR_FAILURE;
  }
  
  if(g_net_config.config_mode == PPPOE)
  {
      return SUCCESS;
  }

  if(g_net_config.link_type == LINK_TYPE_LAN)
  {
    if(!eth_connt_stats.is_eth_insert)
    {
      OS_PRINTF("@@@ethernet connect not ok@@@\n");
      ui_comm_cfmdlg_open_ex(NULL, IDS_NET_NOT_CONNECT, NULL, 2000);
      return ERR_FAILURE;
    }
    else
    {
      config_network_ethcfg();
      OS_PRINTF("@@@begin  do_cmd_connect_network@@@\n");
      do_cmd_connect_network(&ethcfg, eth_dev);
      ssdata_ip_address_set();
      pop_up_connecting_dlg(IDS_CONNECTING_ETH);
    }
  }
  else
  {
    if(!eth_connt_stats.is_wifi_insert)
    {
      OS_PRINTF("@@@wifi connect not ok@@@\n");
      ui_comm_cfmdlg_open_ex(NULL, IDS_WIFI_DEV_NOT_EXIST, NULL, 2000);
      return ERR_FAILURE;
    }
    if(g_net_config.config_mode == STATIC_IP)
    {
      set_static_wifi_ethcfg();
      if(NULL == fw_find_root_by_id(ROOT_ID_WIFI))
      {
       manage_open_menu(ROOT_ID_WIFI, para1, para2);
      }
      return SUCCESS;
    }
    
    sys_status_get_wifi_info(&p_wifi_info);
    
    if(strlen(p_wifi_info.essid) == 0)
    {
      if(NULL == fw_find_root_by_id(ROOT_ID_WIFI))
      {
       manage_open_menu(ROOT_ID_WIFI, para1, para2);
      }
    }
    else
    {
      if(NULL == fw_find_root_by_id(ROOT_ID_WIFI))
      {
        pop_up_connecting_dlg(IDS_CONNECTING_WIFI);
        auto_connect_wifi();
        manage_open_menu(ROOT_ID_WIFI, para1, para2);
      }
      }
  }
 
  return SUCCESS;
}

static RET_CODE on_connect_wifi_failed(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  if(is_link_type_changing_flag)
  {
    is_link_type_changing_flag = FALSE;
  }
  else
  {
    return ERR_FAILURE;
  }
  
  ui_comm_dlg_close_ex();

  on_sub_menu_auto_connect(p_cont, MSG_TIME_OUT, 0, 0);

  return SUCCESS;
}


BEGIN_KEYMAP(sub_menu_cont_keymap, ui_comm_root_keymap)
END_KEYMAP(sub_menu_cont_keymap, ui_comm_root_keymap)
BEGIN_MSGPROC(sub_menu_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_PAINT, on_paint_sub_menu)
  //ON_COMMAND(MSG_PLUG_OUT, on_sub_menu_plug_out)
  ON_COMMAND(MSG_DESTROY, on_sub_menu_destroy)
  ON_COMMAND(MSG_INTERNET_PLUG_IN, on_network_config_plug_in)
  ON_COMMAND(MSG_INTERNET_PLUG_OUT, on_network_config_plug_out)
//  ON_COMMAND(MSG_SIGNAL_UPDATE, on_search_update_signal)
//  ON_COMMAND(MSG_SEARCH_MSG, on_search_update_msg)
  ON_COMMAND(MSG_TIME_OUT, on_sub_menu_auto_connect)
  ON_COMMAND(MSG_WIFI_AP_DISCONNECT, on_connect_wifi_failed)
END_MSGPROC(sub_menu_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(main_mbox_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_RIGHT, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(main_mbox_keymap, NULL)

BEGIN_MSGPROC(main_mbox_proc, mbox_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_main_mbox_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_main_mbox_change_focus)
  ON_COMMAND(MSG_SELECT, on_main_mbox_select)
  ON_COMMAND(MSG_PAINT, on_main_mbox_paint)
  ON_COMMAND(MSG_EXIT, on_main_mbox_exit)
END_MSGPROC(main_mbox_proc, mbox_class_proc)


BEGIN_KEYMAP(up_cont_mbox_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  //ON_EVENT(V_KEY_RIGHT, MSG_SELECT)
  ON_EVENT(V_KEY_LEFT, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(up_cont_mbox_keymap, NULL)

BEGIN_MSGPROC(up_cont_mbox_proc, mbox_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_item_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_item_change_focus)
  ON_COMMAND(MSG_SELECT, on_item_select)
  ON_COMMAND(MSG_PAINT, on_item_paint)
  ON_COMMAND(MSG_EXIT, on_item_exit)
END_MSGPROC(up_cont_mbox_proc, mbox_class_proc)

  //ON_EVENT(V_KEY_LEFT, MSG_EXIT)



//--------
BEGIN_KEYMAP(network_config_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_LEFT, MSG_EXIT)
END_KEYMAP(network_config_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(network_config_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_network_config_exit)
END_MSGPROC(network_config_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(ping_test_keymap, ui_comm_static_keymap)
  
END_KEYMAP(ping_test_keymap, ui_comm_static_keymap)

BEGIN_MSGPROC(ping_test_proc, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_config_test_select)
END_MSGPROC(ping_test_proc, cont_class_proc)

BEGIN_MSGPROC(network_mode_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_network_mode_change)
  ON_COMMAND(MSG_DECREASE, on_active_ctrl_exit)
  ON_COMMAND(MSG_INCREASE, on_active_ctrl_exit)
END_MSGPROC(network_mode_proc, cbox_class_proc)

BEGIN_KEYMAP(connect_network_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(connect_network_keymap, NULL)

BEGIN_MSGPROC(connect_network_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_connect_network)
END_MSGPROC(connect_network_proc, text_class_proc)

BEGIN_KEYMAP(link_type_keymap, NULL)
  ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
END_KEYMAP(link_type_keymap, NULL)

BEGIN_MSGPROC(link_type_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_link_type_changed)
  ON_COMMAND(MSG_DECREASE, on_active_ctrl_exit)
  ON_COMMAND(MSG_INCREASE, on_active_ctrl_exit)
END_MSGPROC(link_type_proc, cbox_class_proc)

BEGIN_KEYMAP(ui_ipbox_keymap, ui_comm_t9_keymap)
  ON_EVENT(V_KEY_UP, MSG_INCREASE)
  ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_UNSELECT)
  ON_EVENT(V_KEY_MENU, MSG_UNSELECT)
END_KEYMAP(ui_ipbox_keymap, ui_comm_t9_keymap)

BEGIN_MSGPROC(ui_ipbox_proc, ipbox_class_proc)
  ON_COMMAND(MSG_FOCUS_LEFT, on_active_ctrl_exit)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_active_ctrl_exit)
  ON_COMMAND(MSG_SELECT, on_ipbox_select)
  ON_COMMAND(MSG_UNSELECT, on_ipbox_unselect)
  ON_COMMAND(MSG_OUTRANGE, on_ipaddress_out_range)
END_MSGPROC(ui_ipbox_proc, ipbox_class_proc)
