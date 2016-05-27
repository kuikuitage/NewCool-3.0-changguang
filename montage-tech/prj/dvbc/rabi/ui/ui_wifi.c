/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_wifi.h"
#include "wifi.h"
#include "ui_network_config_wifi.h"
#include "net_svc.h"
#include "ui_comm_root.h"

enum wifi_local_msg
{
  MSG_REFRESH = MSG_LOCAL_BEGIN + 200,
  MSG_ADD_WIFI,
  MSG_REFRESH_WIFI_SIGNAL,
  MSG_CONFIG_WIFI,
};
 
enum wifi_ctrl_id
{
  IDC_WIFI_TITLE  = 1,
  IDC_CONNECT_STATUS_CONT,
  IDC_CONNECT_STATUS_LEFT,
  IDC_CONNECT_STATUS_RIGHT,
  IDC_CONNECT_STATUS,
  IDC_WIFI_LIST_CONT,
  IDC_WIFI_LIST_BAR,
  IDC_WIFI_LIST,
    
};

static list_xstyle_t wifi_list_item_rstyle =
{
  RSI_PBACK,
  RSI_COMM_CONT_SH,
  RSI_SELECT_F,
  RSI_SELECT_F,
  RSI_SELECT_F,
};

static list_xstyle_t wifi_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_xstyle_t wifi_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLACK,
  FSI_WHITE,
};

static list_field_attr_t wifi_play_list_attr[WIFI_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR,
    45, 0, 0, &wifi_list_field_rstyle,  &wifi_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR |STL_CENTER,
    500, 50, 0, &wifi_list_field_rstyle,  &wifi_list_field_fstyle},
  { LISTFIELD_TYPE_ICON | STL_CENTER,
    50, 550, 0, &wifi_list_field_rstyle,  &wifi_list_field_fstyle},
  { LISTFIELD_TYPE_ICON | STL_CENTER,
    50, 600, 0, &wifi_list_field_rstyle,  &wifi_list_field_fstyle},
  { LISTFIELD_TYPE_STRID| STL_CENTER,
    100, 700, 0, &wifi_list_field_rstyle,  &wifi_list_field_fstyle},
};

ethernet_device_t* p_wifi_dev = NULL;
static u32 wifi_ap_count = 0;
static wifi_info_t p_wifi_inf = {{0},{0}};
static net_conn_stats_t wifi_connt_stats;
static u16 g_conn_focus = INVALIDID;
static u16 g_select_focus = INVALIDID;
static u16 g_connecting_focus = INVALIDID;
//static u8 retry_cnt = 0;
static BOOL g_search_ok = FALSE;
static BOOL g_first_wifi_search = FALSE;
static u8 p_cur_wifi_essid[32+1] = {0};

u16 wifi_cont_keymap(u16 key);
u16 wifi_list_keymap(u16 key);

RET_CODE wifi_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE wifi_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
extern wifi_info_t get_temp_wifi_info();
extern void disconnect_curn_wifi(ethernet_device_t * p_dev);

void set_wifi_select_essid(u8 *essid)
{
	memset(p_cur_wifi_essid, 0, sizeof(p_cur_wifi_essid));
	strcpy(p_cur_wifi_essid, essid);
}

u16 get_wifi_select_focus()
{
  return g_select_focus;
}

u16 get_wifi_conn_focus()
{
  return g_conn_focus;
}

void set_wifi_conn_focus(u16 focus)
{
  g_conn_focus = focus;
}

u16 get_wifi_connecting_focus()
{
  return g_connecting_focus;
}

static control_t *create_wifi_menu(void)
{
  control_t *p_menu = NULL;
  
  p_menu = ui_comm_right_root_create(ROOT_ID_WIFI, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  ctrl_set_keymap(p_menu, wifi_cont_keymap);
  ctrl_set_proc(p_menu, wifi_cont_proc);

  return p_menu;
}

void paint_wifi_status(BOOL is_conn, BOOL is_paint)
{
  control_t *p_status = NULL;
  control_t *root = fw_find_root_by_id(ROOT_ID_WIFI);
  
  if(root == NULL)
  {
    return ;
  }
  
  p_status = ctrl_get_child_by_id(ui_comm_right_root_get_ctrl(ROOT_ID_WIFI, IDC_CONNECT_STATUS_CONT), IDC_CONNECT_STATUS);
  if(is_conn)
  {
    text_set_font_style(p_status, FSI_GREEN, FSI_GREEN, FSI_GREEN);
    text_set_content_by_strid(p_status, IDS_WIFI_DEV_ON_CONN);
  }
  else
  {
    text_set_font_style(p_status, FSI_RED, FSI_RED, FSI_RED);
    text_set_content_by_strid(p_status, IDS_WIFI_DEV_ON_NOT_CONN);
  }
  
  if(is_paint)
  {
    ctrl_paint_ctrl(p_status, TRUE);
  }
}

static void create_wifi_status(control_t *p_menu)
{
  control_t *p_status_cont = NULL, *p_ctrl = NULL, *p_status = NULL;
  
  //status container
  p_status_cont =
			ctrl_create_ctrl(CTRL_CONT, IDC_CONNECT_STATUS_CONT, WIFI_STATUS_CONT_X,
			WIFI_STATUS_CONT_Y, WIFI_STATUS_CONT_W,WIFI_STATUS_CONT_H, p_menu, 0);
  ctrl_set_rstyle(p_status_cont, RSI_COMM_CONT_SH, RSI_SELECT_F, RSI_COMM_CONT_GRAY);

  //left arrow
  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_CONNECT_STATUS_LEFT,
                             WIFI_STATUSL_X, WIFI_STATUSL_Y,
                             WIFI_STATUSL_W, WIFI_STATUSL_H,
                             p_status_cont, 0);
  //bmap_set_content_by_id(p_ctrl, IM_ARROW_L);

  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_CONNECT_STATUS_RIGHT,
                             WIFI_STATUSR_X, WIFI_STATUSR_Y,
                             WIFI_STATUSR_W, WIFI_STATUSR_H,
                             p_status_cont, 0);
  //bmap_set_content_by_id(p_ctrl, IM_ARROW_R);

  //status
  p_status = ctrl_create_ctrl(CTRL_TEXT, IDC_CONNECT_STATUS, WIFI_STATUS_X,
                             WIFI_STATUS_Y, WIFI_STATUS_W,
                             WIFI_STATUS_H, p_status_cont, 0);
  ctrl_set_rstyle(p_status, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_status, FSI_VERMILION, FSI_VERMILION, FSI_VERMILION);
  text_set_align_type(p_status, STL_CENTER | STL_VCENTER);

  wifi_connt_stats = ui_get_net_connect_status();
  paint_wifi_status(wifi_connt_stats.is_wifi_conn, FALSE);
  
}

void paint_list_field_is_connect(u16 index, BOOL is_connect, BOOL is_paint)
{
  control_t *p_list = NULL;
  control_t *root = fw_find_root_by_id(ROOT_ID_WIFI);
  
  if(root == NULL || index == INVALIDID)
  {
    return ;
  }
  
  p_list = ui_comm_right_root_get_ctrl(ROOT_ID_WIFI, IDC_WIFI_LIST);
  if(is_connect)
  {
    list_set_field_content_by_strid(p_list, index, 4, IDS_CONNECT);
  }
  else
  {
    list_set_field_content_by_strid(p_list, index, 4, 0);
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_list, TRUE);
  }
}

static RET_CODE wifi_list_update(control_t* p_list, u16 start, u16 size,
                                u32 context)
{
  u16 i = 0;
  u8 asc_str[32] = {0};
  u16 uni_str[64];
  u16 temp = 0;
  wifi_ap_info_t ap_info = {{0}};
  u16 cnt = 0;
  wifi_info_t wifi_inf = get_temp_wifi_info();

  sys_status_get_wifi_info(&p_wifi_inf,wifi_inf.essid);
  wifi_connt_stats = ui_get_net_connect_status();
  cnt = list_get_count(p_list);
  DEBUG(MAIN, INFO, "wifi list update cnt == %d ####  wifi_inf.essid = %s p_wifi_inf.essid=%s \n", cnt, wifi_inf.essid,p_wifi_inf.essid);
  if(cnt == INVALIDID || cnt == 0)
  {
    OS_PRINTF("####cnt is 65535 or 0, is invalidid list not update ,will return###\n");
    return ERR_FAILURE;
  }
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      sprintf(asc_str, "%.3d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);
      /* NAME */
      #ifndef WIN32
      wifi_ap_info_get(p_wifi_dev,(u16)(start + i),&ap_info);
      #endif
      OS_PRINTF(" ####ui list update, Qual_level %3d,   Essid %s ####\n", ap_info.qual_level,ap_info.essid);
	  memset(uni_str, 0, sizeof(uni_str)); 
	  ui_wifi_asc2uni(ap_info.essid, uni_str, sizeof(ap_info.essid));
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, uni_str);
      /* LOCK ICON */
      if(ap_info.EncrypType != IW_ENC_NONE)
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 2, IM_XIC_SET_PER_LOCK);
   //     OS_PRINTF("@@the %s wifi sigal encrypttype is %d###\n",ap_info.essid, ap_info.EncrypType);
      }
      else
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 2, 0);
      }
      /*connect status*/
      if((strcmp(p_wifi_inf.essid, ap_info.essid) == 0 )&& (strcmp("", p_wifi_inf.essid) != 0)&&(strlen(p_wifi_inf.essid) == strlen(ap_info.essid)))
      {
        g_connecting_focus = (u16)(start + i);
        if(wifi_connt_stats.is_wifi_conn)
        {
          g_conn_focus = (u16)(start + i);
          paint_list_field_is_connect(g_conn_focus, 1, FALSE);
          OS_PRINTF("########ssdata essid is%s , and search wifi ap essid  is:%s#####",p_wifi_inf.essid,ap_info.essid);
        }
      }
      else
      {
        paint_list_field_is_connect((u16)(start + i), 0, FALSE);
      }
      
       /*SIGNAL  ICON */
       temp = ap_info.qual_level;
       temp = temp/10;
      
       switch(temp)
       {
          case 1:
          case 2:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_1);
            break;
          case 3:
          case 4:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_2);
            break; 
          case 5:
          case 6:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3,IM_SIGNAL_3);
            break;  
          case 7:
          case 8:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_4);
            break;
          case 9:
          case 10:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_5);
            break;
           default:
            break;
          
       }
      
    }

  }

  return SUCCESS;
}

static void create_wifi_list(control_t *p_menu)
{
  control_t *p_list_sbar = NULL, *p_list = NULL;
  u16 i;  
  
  //scrollbar
  p_list_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_WIFI_LIST_BAR, WIFI_LIST_BAR_X,
    WIFI_LIST_BAR_Y, WIFI_LIST_BAR_W, WIFI_LIST_BAR_H, p_menu, 0);
  ctrl_set_rstyle(p_list_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
  sbar_set_autosize_mode(p_list_sbar, 1);
  sbar_set_direction(p_list_sbar, 0);
  sbar_set_mid_rstyle(p_list_sbar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID,
                     RSI_SCROLL_BAR_MID);

  //list
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_WIFI_LIST, WIFI_LIST_X,
    WIFI_LIST_Y, WIFI_LIST_W, WIFI_LIST_H, p_menu, 0);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_list, wifi_list_keymap);
  ctrl_set_proc(p_list, wifi_list_proc);
 
  ctrl_set_mrect(p_list, WIFI_LIST_MIDL, WIFI_LIST_MIDT,
                    WIFI_LIST_MIDW+WIFI_LIST_MIDL, WIFI_LIST_MIDH+WIFI_LIST_MIDT);
  list_set_item_interval(p_list,WIFI_LCONT_LIST_VGAP);
  list_set_item_rstyle(p_list, &wifi_list_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_count(p_list, (u16)0, WIFI_LIST_ITEM_NUM_ONE_PAGE);
  list_set_field_count(p_list, WIFI_LIST_FIELD, WIFI_LIST_ITEM_NUM_ONE_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, wifi_list_update, 0);
  ctrl_set_style(p_list, STL_EX_ALWAYS_HL);

  for (i = 0; i < WIFI_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(wifi_play_list_attr[i].attr), (u16)(wifi_play_list_attr[i].width),
                        (u16)(wifi_play_list_attr[i].left), (u8)(wifi_play_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, wifi_play_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, wifi_play_list_attr[i].fstyle);
  }
  list_set_scrollbar(p_list, p_list_sbar);
  //wifi_list_update(p_list, list_get_valid_pos(p_list), WIFI_LIST_ITEM_NUM_ONE_PAGE, 0);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0); 
}

static void create_wifi_helpbar(control_t *p_menu)
{

  comm_help_data_t help_data =
  {
    3,3,
    {IDS_REFRESH,0, IDS_CONFIG_MODE},
    {IM_KEY_IC_REFRESH,0, IM_KEY_IC_FN},
  };

  ui_comm_help_create(&help_data, p_menu);
  ui_comm_help_move_pos(p_menu, 200, -30, 600, 20, 42);
  
}
ethernet_device_t *get_wifi_dev_handle()
{
  return p_wifi_dev;
}

static void ui_wifi_info_get(void)
{
  #ifndef WIN32
   p_wifi_dev = (ethernet_device_t *)get_wifi_handle();
  #endif
  //p_wifi_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_WIFI);
  if(p_wifi_dev == NULL)
  {
    OS_PRINTF("@@@p_wifi_dev == NULL@@@\n");
    return ;
  }
  {
    service_t *p_server = NULL;
    net_svc_cmd_para_t net_svc_para;
    net_svc_t *p_net_svc = NULL;

    p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
    if(p_net_svc->net_svc_is_busy(p_net_svc) == TRUE)
    {
      ui_comm_cfmdlg_open(NULL, IDS_NETWORK_BUSY, NULL, 2000);
      OS_PRINTF("###########net service is busy ,will be return#########\n");
      return ;
    }
    net_svc_para.p_eth_dev = p_wifi_dev;
    p_server = (service_t *)ui_get_net_svc_instance();
    
    p_server->do_cmd(p_server, NET_DO_WIFI_SERACH, (u32)&net_svc_para, sizeof(net_svc_cmd_para_t));
  }

}

void ui_wifi_info_search(void)
{
	g_search_ok = FALSE; 
	ui_wifi_info_get();
}

static void popup_searching_win()
{
  comm_dlg_data_t search_dlg_data =
  {
    0,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    RIGHT_ROOT_X+(WIFI_CONT_W-COMM_DLG_W)/2, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    IDS_SEARCHING_WIFI,
    0,
  };
  ui_comm_dlg_open(&search_dlg_data);
}

static RET_CODE active_wifi(void)
{
	rect_t rec;
	net_config_t g_net_config;
	net_conn_stats_t connt_stats;
	connt_stats = ui_get_net_connect_status();	
	sys_status_get_net_config_info(&g_net_config);	
	if(!connt_stats.is_wifi_insert)
	{
		rec.left = RIGHT_ROOT_X+(WIFI_CONT_W-COMM_DLG_W)/2;
		rec.right = RIGHT_ROOT_X+(WIFI_CONT_W+COMM_DLG_W)/2;
		rec.top = COMM_DLG_Y;
		rec.bottom = COMM_DLG_Y+COMM_DLG_H;
		ui_comm_cfmdlg_open(&rec, IDS_WIFI_DEV_NOT_EXIST, NULL, 2000);		
		return ERR_FAILURE;
	}

	if(LINK_TYPE_WIFI!=g_net_config.link_type)
	{
		g_net_config.link_type = LINK_TYPE_WIFI;
		sys_status_set_net_config_info(&g_net_config);
		sys_status_save();	
	}
	//ui_comm_cfmdlg_open(NULL, IDS_WIFI_CONN_SUC, NULL, 2000);
	return SUCCESS;
}


RET_CODE ui_open_wifi(u32 para1, u32 para2)
{
  control_t *p_menu = NULL;
#ifndef WIN32
 if(SUCCESS!=active_wifi())
 	return ERR_FAILURE;
#endif
  //wifi info get
  ui_wifi_info_get();
 g_search_ok = FALSE; 
 g_first_wifi_search = FALSE;
  //create menu
  p_menu = create_wifi_menu();
  
  //create connect status control
  create_wifi_status(p_menu);

  //create wifi list
  create_wifi_list(p_menu);

  //create help bar
  create_wifi_helpbar(p_menu);

  fw_tmr_create(ROOT_ID_WIFI, MSG_REFRESH_WIFI_SIGNAL, 30 * SECOND, TRUE);
  
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  popup_searching_win();
  
  return SUCCESS;
}

static RET_CODE on_wifi_exit(control_t *p_cont,
                              u16 msg, u32 para1, u32 para2)
{
	if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) 
	{
		ui_comm_dlg_close();
	}
	// retry_cnt = 0;
	fw_tmr_destroy(ROOT_ID_WIFI, MSG_REFRESH_WIFI_SIGNAL);
	return ERR_NOFEATURE;
}

static RET_CODE on_wifi_destory(control_t *p_cont,
                              u16 msg, u32 para1, u32 para2)
{
	g_conn_focus = INVALIDID;
	g_select_focus = INVALIDID;
	g_connecting_focus = INVALIDID;

	return ERR_NOFEATURE;
}


static RET_CODE on_wifi_search_ok(control_t *p_list, u16 msg,u32 para1, u32 para2)
{

	if(g_first_wifi_search == FALSE)
	{
		ui_comm_dlg_close();
		wifi_ap_count = para1;

		DEBUG(MAIN, INFO, "wifi ui, after search get ap count == %d@@\n", wifi_ap_count);
		list_set_count(p_list, wifi_ap_count, WIFI_LIST_ITEM_NUM_ONE_PAGE);
		list_set_update(p_list, wifi_list_update, 0);
		list_set_focus_pos(p_list, 0);
		wifi_list_update(p_list, list_get_valid_pos(p_list), WIFI_LIST_ITEM_NUM_ONE_PAGE, 0);

		//ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
		DEBUG(MAIN, INFO, "@@@on_wifi_search_ok@@@\n");
		ctrl_paint_ctrl(p_list->p_parent, TRUE);
		g_search_ok = TRUE;
		g_first_wifi_search = TRUE;
	}
	else
	{
		u16 i;
		wifi_ap_info_t ap_info = {{0}};
		wifi_ap_count = para1;

		wifi_connt_stats = ui_get_net_connect_status();
		for(i=0; i<wifi_ap_count; i++)
		{
			#ifndef WIN32
			wifi_ap_info_get(p_wifi_dev,i,&ap_info);
			#endif
			if(strlen(p_cur_wifi_essid) != strlen(ap_info.essid))
				continue;
			if(strcmp(p_cur_wifi_essid, ap_info.essid) == 0 && strcmp("", p_cur_wifi_essid) != 0)
			{
				g_select_focus =  i;
				break;
			}
		}

		if(i < wifi_ap_count)
		{
			list_set_focus_pos(p_list, g_select_focus);
		}
		else
		{
			if(wifi_connt_stats.is_wifi_conn)
				disconnect_curn_wifi(p_wifi_dev);
			list_set_focus_pos(p_list, 0);
		}
		
		DEBUG(MAIN, INFO, "wifi ui, after search get ap count == %d@@\n", wifi_ap_count);
		list_set_count(p_list, wifi_ap_count, WIFI_LIST_ITEM_NUM_ONE_PAGE);
		wifi_list_update(p_list,list_get_valid_pos(p_list), WIFI_LIST_ITEM_NUM_ONE_PAGE, 0);
		DEBUG(MAIN, INFO, "@@@on_wifi_search_ok@@@\n");
		ctrl_paint_ctrl(p_list->p_parent, TRUE);
		g_search_ok = TRUE;
		g_first_wifi_search = TRUE;
	}

	return SUCCESS;
}

static RET_CODE on_wifi_list_select(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  u16 focus = 0;
  if(!g_search_ok)
  {
    return ERR_FAILURE;
  }
  if(wifi_ap_count < 0 || wifi_ap_count == 0)
  {
    OS_PRINTF("#######wifi ap count <=0, will return####\n");
    return ERR_FAILURE;
  }
  focus = list_get_focus_pos(p_list);
  if(0xffff == focus)
    {      
      list_set_focus_pos(p_list, 0);
      wifi_list_update(p_list, list_get_valid_pos(p_list), WIFI_LIST_ITEM_NUM_ONE_PAGE, 0);
      focus = 0;
    }
  g_select_focus = focus;
  manage_open_menu(ROOT_ID_WIFI_LINK_INFO, 0, (u32)focus);//para1 means connect wifi
  return SUCCESS;
}

static RET_CODE on_wifi_list_add(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  	manage_open_menu(ROOT_ID_WIFI_LINK_INFO, 1, 0);//para1 ==1 means  add wifi
  	return SUCCESS;
}

static RET_CODE on_wifi_config(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
    control_t *p_root = NULL;
    p_root = ui_comm_right_root_get_root(ROOT_ID_NETWORK_CONFIG_WIFI);
    if(p_root)
    {
      ctrl_process_msg(p_root, MSG_EXIT, 0, 0);
    }  
	manage_open_menu(ROOT_ID_NETWORK_CONFIG_WIFI, 1, 0);//para1 ==1 means  add wifi
	ui_comm_dlg_close();
	manage_close_menu(ROOT_ID_WIFI, 0, 0);//fix 47420
	return SUCCESS;
}

static RET_CODE on_wifi_list_refresh(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  popup_searching_win();
  ui_wifi_info_get();
  g_search_ok = FALSE;
  g_first_wifi_search = FALSE;
  list_set_focus_pos(p_list, 0);
  list_set_count(p_list, wifi_ap_count, WIFI_LIST_ITEM_NUM_ONE_PAGE);
  list_set_update(p_list, wifi_list_update, 0);
  wifi_list_update(p_list, list_get_valid_pos(p_list), WIFI_LIST_ITEM_NUM_ONE_PAGE, 0);
  OS_PRINTF("@@@on_wifi_list_refresh@@@\n");
  ctrl_paint_ctrl(p_list->p_parent, TRUE);
  return SUCCESS;
}

void paint_wifi_list_field_not_connect(control_t *p_list)
{
  u16 i;
  u16 cnt;
  u16 start;
  
  start = list_get_valid_pos(p_list);
  cnt = list_get_count(p_list);
  for (i = 0; i < WIFI_LIST_ITEM_NUM_ONE_PAGE; i++)
  {
    if (i + start < cnt)
    {
      paint_list_field_is_connect((u16)(start + i), FALSE, TRUE);
    }
  }
  ctrl_paint_ctrl(p_list, TRUE);
}

static void paint_wifi_signal(control_t *p_list)
{
  u16 i;
  u16 temp;
  wifi_ap_info_t ap_info;
  u16 cnt;
  u16 start;
  
  start = list_get_valid_pos(p_list);
  cnt = list_get_count(p_list);
  for (i = 0; i < WIFI_LIST_ITEM_NUM_ONE_PAGE; i++)
  {
    if (i + start < cnt)
    {
     
      #ifndef WIN32
      wifi_ap_info_get(p_wifi_dev,(u16)(start + i),&ap_info);
      #endif
       temp = ap_info.qual_level;
       temp = temp/10;
      
       switch(temp)
       {
          case 1:
          case 2:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_1);
            break;
          case 3:
          case 4:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_2);
            break; 
          case 5:
          case 6:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3,IM_SIGNAL_3);
            break;  
          case 7:
          case 8:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_4);
            break;
          case 9:
          case 10:
            list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_SIGNAL_5);
            break;
           default:
            break;
          
       }
      
    }

  }
  OS_PRINTF("@@@@###########paint_wifi_signal###########@@@\n");
  ctrl_paint_ctrl(p_list, TRUE);
  
}


static RET_CODE on_wifi_list_signal_update(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  DEBUG(MAIN, INFO, "wifi list update \n");
  paint_wifi_signal(p_list);
  
  return SUCCESS;
}

static BOOL ui_wifi_is_utf8(char *rawtext,int len)
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

static void ui_wifi_c_utf82uni(u8 *in, u16 *out, u16 out_size)
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

void ui_wifi_asc2uni(u8 *asc_str, u16 *uni_str, int uni_str_len)
{
	if(ui_wifi_is_utf8((char *)asc_str, uni_str_len))
	{
		DEBUG(MAIN,ERR,"UTF8\n");
		ui_wifi_c_utf82uni(asc_str, uni_str, uni_str_len);
	}
	else
	{
		DEBUG(MAIN,ERR,"gb2312\n");
		convert_gb2312_chinese_asc2unistr(asc_str, uni_str, uni_str_len);
	}
}


BEGIN_KEYMAP(wifi_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  
END_KEYMAP(wifi_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(wifi_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_wifi_exit)
  ON_COMMAND(MSG_EXIT_ALL, on_wifi_exit)
  ON_COMMAND(MSG_DESTROY, on_wifi_destory)
END_MSGPROC(wifi_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(wifi_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_HOT_XREFRESH,MSG_REFRESH)
  //ON_EVENT(V_KEY_GREEN,MSG_ADD_WIFI)
	ON_EVENT(V_KEY_HOT_XEXTEND,MSG_CONFIG_WIFI)
END_KEYMAP(wifi_list_keymap, NULL)

BEGIN_MSGPROC(wifi_list_proc, list_class_proc)
  ON_COMMAND(MSG_GET_WIFI_AP_CNT, on_wifi_search_ok)
  ON_COMMAND(MSG_SELECT, on_wifi_list_select)
  ON_COMMAND(MSG_REFRESH, on_wifi_list_refresh)
  ON_COMMAND(MSG_ADD_WIFI, on_wifi_list_add)
  ON_COMMAND(MSG_REFRESH_WIFI_SIGNAL, on_wifi_list_signal_update)
  ON_COMMAND(MSG_CONFIG_WIFI, on_wifi_config)
END_MSGPROC(wifi_list_proc, list_class_proc)
