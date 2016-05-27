/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_submenu_network.h"

typedef enum
{
  FIRST_PAGE = 0,
  SECOND_PAGE,
}network_page_t;

enum control_id
{
	IDC_INVALID = 0,
	IDC_NETWORK_ICON_BOX,
	IDC_TEXT,
	IDC_HELP = 10,
	IDC_NETWORK_BTM_LINE,
	IDC_NETWORK_LEFT_ARROW,
	IDC_NETWORK_RIGHT_ARROW,

};

u16 icon_mbox_keymap(u16 key);
RET_CODE icon_mbox_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 youporn_pwdlg_keymap(u16 key);
RET_CODE youporn_pwdlg_proc(control_t *ctrl,u16 msg,u32 para1,u32 para2);

static void on_switch_network();

static network_page_t network_page = FIRST_PAGE;

static control_t *create_net_sub_menu()
{
  control_t *p_menu = NULL;
  
   //main menu
  p_menu = ui_comm_root_create(ROOT_ID_SUBMENU_NETWORK,
    0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,  COMM_BG_H, IM_INDEX_NETWORK_BANNER, IDS_NETWORK);
  
  ctrl_set_proc(p_menu, ui_comm_root_proc);
  ctrl_set_rstyle(p_menu, RSI_MAIN_BG, RSI_MAIN_BG, RSI_MAIN_BG);
  
  return p_menu;
}

static void create_net_sub_arrows(control_t *p_menu)
{
  control_t *p_ctrl = NULL;
  
  //left arrow
  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_NETWORK_LEFT_ARROW,
                             UI_SUBMENU_NET_LEFT_ARROW_X, UI_SUBMENU_NET_LEFT_ARROW_Y,
                             UI_SUBMENU_NET_LEFT_ARROW_W, UI_SUBMENU_NET_LEFT_ARROW_H,
                             p_menu, 0);
  
  if(network_page == FIRST_PAGE)
  {
    bmap_set_content_by_id(p_ctrl, IM_ICON_NETWORK_ARROW_L);
  }
  else if(network_page == SECOND_PAGE)
  {
    bmap_set_content_by_id(p_ctrl, IM_ICON_NETWORK_ARROW_SELECT_L);
  }  
  
  //right arrow
  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_NETWORK_RIGHT_ARROW,
                             UI_SUBMENU_NET_RIGHT_ARROW_X, UI_SUBMENU_NET_RIGHT_ARROW_Y,
                             UI_SUBMENU_NET_RIGHT_ARROW_W, UI_SUBMENU_NET_RIGHT_ARROW_H,
                             p_menu, 0);
  
  if(network_page == FIRST_PAGE)
  {
    bmap_set_content_by_id(p_ctrl, IM_ICON_NETWORK_ARROW_SELECT_R);
  }
  else if(network_page == SECOND_PAGE)
  {
    bmap_set_content_by_id(p_ctrl, IM_ICON_NETWORK_ARROW_R);
  }  
  
}

static void change_left_right_arrows()
{
  control_t *p_left_arrow = NULL;
  control_t *p_right_arrow = NULL;

  p_left_arrow = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU_NETWORK, IDC_NETWORK_LEFT_ARROW);
  p_right_arrow = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU_NETWORK, IDC_NETWORK_RIGHT_ARROW);

  if(network_page == FIRST_PAGE)
  {
    bmap_set_content_by_id(p_left_arrow, IM_ICON_NETWORK_ARROW_SELECT_L);
    bmap_set_content_by_id(p_right_arrow, IM_ICON_NETWORK_ARROW_R);
  }
  else if(network_page == SECOND_PAGE)
  {
    bmap_set_content_by_id(p_left_arrow, IM_ICON_NETWORK_ARROW_L);
    bmap_set_content_by_id(p_right_arrow, IM_ICON_NETWORK_ARROW_SELECT_R);
  }  
}
static void create_net_sub_details_one_page(control_t *p_mbox)
{
  u16 i = 0;
#if ENABLE_NET_PLAY
  u16 net_image_hl[NETWORK_SUBMENU_ITEM] = 
  {IM_INDEX_NETWORKDETAIL_ETHERNETCONFIG_2,IM_INDEX_NETWORKDETAIL_ONLINEMOVIE_2,
  IM_INDEX_NETWORKDETAIL_YOUTUBE_2,IM_INDEX_NETWORKDETAIL_YOUPORN_2};
  u16 net_image_sh[NETWORK_SUBMENU_ITEM] = 
  {IM_INDEX_NETWORKDETAIL_ETHERNETCONFIG_1,IM_INDEX_NETWORKDETAIL_ONLINEMOVIE_1,
  IM_INDEX_NETWORKDETAIL_YOUTUBE_1,IM_INDEX_NETWORKDETAIL_YOUPORN_1};
  u16 text_str[NETWORK_SUBMENU_ITEM] =
  {IDS_NETWORK_CONFIG, IDS_NETWORK_PLAY, IDS_YOUTUBE, IDS_YOUPORN};
#else
  u16 net_image_hl[NETWORK_SUBMENU_ITEM] = 
  {IM_INDEX_NETWORKDETAIL_ETHERNETCONFIG_2,IM_INDEX_NETWORKDETAIL_YOUTUBE_2,
  IM_INDEX_NETWORKDETAIL_YOUPORN_2,IM_INDEX_NETWORKDETAIL_WEATHER_2};
  u16 net_image_sh[NETWORK_SUBMENU_ITEM] = 
  {IM_INDEX_NETWORKDETAIL_ETHERNETCONFIG_1,IM_INDEX_NETWORKDETAIL_YOUTUBE_1,
  IM_INDEX_NETWORKDETAIL_YOUPORN_1,IM_INDEX_NETWORKDETAIL_WEATHER_1};
  u16 text_str[NETWORK_SUBMENU_ITEM] =
  {IDS_NETWORK_CONFIG, IDS_YOUTUBE, IDS_YOUPORN, IDS_WEATHER_FORECAST};
#endif

  for(i = 0; i < NETWORK_SUBMENU_ITEM; i++)
  {
    mbox_set_content_by_icon(p_mbox, i, net_image_hl[i], net_image_sh[i]);
    mbox_set_content_by_strid(p_mbox, i, text_str[i]);
  }

}

static void create_net_sub_details_second_page(control_t *p_mbox)
{
  u16 i = 0;
#if ENABLE_NET_PLAY  
  #if ENABLE_FLICKR
  u16 net_image_hl[NETWORK_SUBMENU_ITEM] = 
  {IM_INDEX_NETWORKDETAIL_WEATHER_2,IM_INDEX_NETWORKDETAIL_GOOGLE_MAP_2,IM_INDEX_NETWORKDETAIL_FLICKR_2,0};
  u16 net_image_sh[NETWORK_SUBMENU_ITEM] = 
  {IM_INDEX_NETWORKDETAIL_WEATHER_1,IM_INDEX_NETWORKDETAIL_GOOGLE_MAP_1,IM_INDEX_NETWORKDETAIL_FLICKR_1,0};
  u16 text_str[NETWORK_SUBMENU_ITEM] =
  {IDS_WEATHER_FORECAST,IDS_GOOGLE_MAP, IDS_FLICKR, 0};
  #else
  u16 net_image_hl[NETWORK_SUBMENU_ITEM] = 
  {IM_INDEX_NETWORKDETAIL_WEATHER_2,IM_INDEX_NETWORKDETAIL_GOOGLE_MAP_2,0,0};
  u16 net_image_sh[NETWORK_SUBMENU_ITEM] = 
  {IM_INDEX_NETWORKDETAIL_WEATHER_1,IM_INDEX_NETWORKDETAIL_GOOGLE_MAP_1,0,0};
  u16 text_str[NETWORK_SUBMENU_ITEM] =
  {IDS_WEATHER_FORECAST,IDS_GOOGLE_MAP, 0, 0};
  #endif
#else
  #if ENABLE_FLICKR
  u16 net_image_hl[NETWORK_SUBMENU_ITEM] = 
  {IM_INDEX_NETWORKDETAIL_GOOGLE_MAP_2,IM_INDEX_NETWORKDETAIL_FLICKR_2,0,0};
  u16 net_image_sh[NETWORK_SUBMENU_ITEM] = 
  {IM_INDEX_NETWORKDETAIL_GOOGLE_MAP_1,IM_INDEX_NETWORKDETAIL_FLICKR_1,0,0};
  u16 text_str[NETWORK_SUBMENU_ITEM] =
  {IDS_GOOGLE_MAP, IDS_FLICKR, 0, 0};
  #else
  u16 net_image_hl[NETWORK_SUBMENU_ITEM] = 
  {IM_INDEX_NETWORKDETAIL_GOOGLE_MAP_2,0,0,0};
  u16 net_image_sh[NETWORK_SUBMENU_ITEM] = 
  {IM_INDEX_NETWORKDETAIL_GOOGLE_MAP_1,0,0,0};
  u16 text_str[NETWORK_SUBMENU_ITEM] =
  {IDS_GOOGLE_MAP, 0, 0, 0};
  #endif
#endif

  for(i = 0; i < NETWORK_SUBMENU_ITEM; i++)
  {
    mbox_set_content_by_icon(p_mbox, i, net_image_hl[i], net_image_sh[i]);
    mbox_set_content_by_strid(p_mbox, i, text_str[i]);
  }

}

static void create_net_sub_mbox(control_t *p_menu)
{
  control_t *p_mbox_icon = NULL;
   //icon box
  p_mbox_icon = ctrl_create_ctrl(CTRL_MBOX, IDC_NETWORK_ICON_BOX,
                                 UI_SUBMENU_NET_CTRL_X, UI_SUBMENU_NET_CTRL_Y,
                                 UI_SUBMENU_NET_CTRL_W, UI_SUBMENU_NET_CTRL_H,
                                 p_menu, 0);
  ctrl_set_rstyle(p_mbox_icon, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_mbox_icon, icon_mbox_keymap);
  ctrl_set_proc(p_mbox_icon,icon_mbox_proc);
  mbox_enable_icon_mode(p_mbox_icon, TRUE);
  mbox_enable_string_mode(p_mbox_icon, TRUE);
  mbox_set_count(p_mbox_icon, NETWORK_SUBMENU_ITEM, NETWORK_SUBMENU_ITEM, 1);
  mbox_set_item_interval(p_mbox_icon, 0, 0);
  mbox_set_item_rstyle(p_mbox_icon, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_set_string_fstyle(p_mbox_icon, FSI_YELLOW_32, FSI_WHITE, FSI_WHITE);
  mbox_set_content_strtype(p_mbox_icon, MBOX_STRTYPE_STRID);
  mbox_set_icon_align_type(p_mbox_icon, STL_CENTER | STL_TOP);
  mbox_set_string_offset(p_mbox_icon, 0, 0);
  mbox_set_string_align_type(p_mbox_icon, STL_CENTER | STL_BOTTOM);
  if(network_page == FIRST_PAGE)
  {
    create_net_sub_details_one_page(p_mbox_icon);
  }
  else if(network_page == SECOND_PAGE)
  {
    create_net_sub_details_second_page(p_mbox_icon);
  }

  mbox_set_focus(p_mbox_icon, 0);
  ctrl_set_style(p_mbox_icon, STL_EX_ALWAYS_HL);
  ctrl_default_proc(p_mbox_icon, MSG_GETFOCUS, 0, 0);
  
}

static void create_net_sub_bottom_line(control_t *p_menu)
{
  control_t *p_line = NULL;

   //bottom line
  p_line = ctrl_create_ctrl(CTRL_CONT, IDC_NETWORK_BTM_LINE, 0, 
  UI_NETWORK_BOTTOM_HELP_Y-60, COMM_BG_W, 60,p_menu, 0);
  ctrl_set_rstyle(p_line, RSI_INFO_BAR_TITLE, RSI_INFO_BAR_TITLE, RSI_INFO_BAR_TITLE);
  
}

static void create_net_sub_help_bar(control_t *p_menu)
{
  control_t *p_help_bar = NULL;
  comm_help_data_t help_data = 
  {
    3,3,
    {IDS_MOVE,IDS_OK,IDS_MENU},
    {IM_HELP_ARROW,IM_HELP_OK,IM_HELP_MENU}
  };

  p_help_bar = ctrl_create_ctrl(CTRL_TEXT, IDC_HELP,UI_NETWORK_BOTTOM_HELP_X, 
                                            UI_NETWORK_BOTTOM_HELP_Y, UI_NETWORK_BOTTOM_HELP_W,
                                            UI_NETWORK_BOTTOM_HELP_H, p_menu, 0);
  ui_comm_help_create_ext(300, 0, UI_NETWORK_BOTTOM_HELP_W-300, UI_NETWORK_BOTTOM_HELP_H, \
                                            &help_data,  p_help_bar);
}

RET_CODE open_submenu_network(u32 para1, u32 para2)
{
  control_t *p_menu = NULL;

  //create main menu
  p_menu = create_net_sub_menu();

  //create left right arrows
  create_net_sub_arrows(p_menu);

  //create icon box
  create_net_sub_mbox(p_menu);

  //create bottom line
  create_net_sub_bottom_line(p_menu);

  //create help bar
  create_net_sub_help_bar(p_menu);
 
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  return SUCCESS;
}

static RET_CODE on_icon_box_change_focus(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{ 
  u16 focus;
  
  focus = mbox_get_focus(p_ctrl);
  if(network_page == FIRST_PAGE && focus == 3 && msg == MSG_FOCUS_RIGHT)
  {
    on_switch_network();
  }
  if(network_page == FIRST_PAGE && focus == 0 && msg == MSG_FOCUS_LEFT)
  {
    return ERR_FAILURE;
  }
  else if(network_page == SECOND_PAGE && focus == 0 && msg == MSG_FOCUS_LEFT)
  {
    on_switch_network();
  }
#if ENABLE_NET_PLAY
  #if ENABLE_FLICKR
  else if(network_page == SECOND_PAGE && focus == 2 && msg == MSG_FOCUS_RIGHT)
  {
    return ERR_FAILURE;
  }
  else if(network_page == SECOND_PAGE && focus == 3)
  {
    return ERR_FAILURE;
  }
  #else
  else if(network_page == SECOND_PAGE && focus == 1 && msg == MSG_FOCUS_RIGHT)
  {
    return ERR_FAILURE;
  }
  else if(network_page == SECOND_PAGE && (focus == 2 || focus == 3))
  {
    return ERR_FAILURE;
  }
  #endif
#else
  else if(network_page == SECOND_PAGE && msg == MSG_FOCUS_RIGHT)
  {
    return ERR_FAILURE;
  }	
#endif
  else
  {
    mbox_class_proc(p_ctrl, msg, para1, para2);
  }
  OS_PRINTF("@@@@on_icon_box_change_focus@@@@\n");
  
  return SUCCESS;
}

static RET_CODE check_network_conn_stats()
{
  net_conn_stats_t eth_connt_stats = {0};
  RET_CODE ret = SUCCESS;
#ifndef WIN32  
  eth_connt_stats = ui_get_net_connect_status();
  if((eth_connt_stats.is_eth_conn == FALSE) && (eth_connt_stats.is_wifi_conn == FALSE))
  {
     ret = ERR_FAILURE;  
     ui_comm_cfmdlg_open_ex(NULL, IDS_NET_CABLE_NOT_CONNECT, NULL, 2000);
  }
#endif
  return ret;
}

static RET_CODE on_icon_box_select(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  u16 focus = 0;
  u16 focus_bmp = 0;
  comm_pwdlg_data_t youporn_pwdlg_data =
  {
	ROOT_ID_SUBMENU_NETWORK,
	YOUPORN_PWD_DLG_FOR_CHK_X,
	YOUPORN_PWD_DLG_FOR_CHK_Y,
	IDS_MSG_INPUT_PASSWORD,
	youporn_pwdlg_keymap,
	youporn_pwdlg_proc,
  };
  focus = mbox_get_focus(p_ctrl);
  focus_bmp = mbox_get_focus_icon(p_ctrl, focus);
  
  switch(focus_bmp)
  {
    case IM_INDEX_NETWORKDETAIL_ETHERNETCONFIG_2:
      manage_open_menu(ROOT_ID_NETWORK_CONFIG, 0, 0);
      break;
#if ENABLE_NET_PLAY
    case IM_INDEX_NETWORKDETAIL_ONLINEMOVIE_2:
      if(ERR_FAILURE == check_network_conn_stats())
      {
        return ERR_FAILURE;
      }
      manage_open_menu(ROOT_ID_ONMOV_WEBSITES, 0, 0);
    break;
#endif    
    case IM_INDEX_NETWORKDETAIL_YOUTUBE_2:
      if(ERR_FAILURE == check_network_conn_stats())
      {
        return ERR_FAILURE;
      }
      manage_open_menu(ROOT_ID_YOUTUBE, 0, 0);
    break;
    case IM_INDEX_NETWORKDETAIL_GOOGLE_MAP_2:
      if(ERR_FAILURE == check_network_conn_stats())
      {
        return ERR_FAILURE;
      }
      manage_open_menu(ROOT_ID_GOOGLE_MAP, 0, 0);
    break;
    case IM_INDEX_NETWORKDETAIL_WEATHER_2:
      if(ERR_FAILURE == check_network_conn_stats())
      {
        return ERR_FAILURE;
      }
      manage_open_menu(ROOT_ID_WEATHER_FORECAST, 0, 0);
    break;
	case IM_INDEX_NETWORKDETAIL_YOUPORN_2:
      if(ERR_FAILURE == check_network_conn_stats())
      {
        return ERR_FAILURE;
      }
      ui_comm_pwdlg_open(&youporn_pwdlg_data);
    break;
#if ENABLE_FLICKR
   case IM_INDEX_NETWORKDETAIL_FLICKR_2:
      if(ERR_FAILURE == check_network_conn_stats())
      {
        return ERR_FAILURE;
      }
      manage_open_menu(ROOT_ID_FLICKR, 0, 0);
    break;
#endif
  }
  OS_PRINTF("@@@@on_icon_box_select@@@@\n");
  
  return SUCCESS;
}

static void on_switch_network()
{
  control_t *p_icon_box = NULL;

  change_left_right_arrows();
  p_icon_box = ui_comm_root_get_ctrl(ROOT_ID_SUBMENU_NETWORK, IDC_NETWORK_ICON_BOX);
  if(network_page == FIRST_PAGE)
  {
    network_page = SECOND_PAGE;
    create_net_sub_details_second_page(p_icon_box);
    mbox_set_focus(p_icon_box, 0);
  }
  else if(network_page == SECOND_PAGE)
  {
    network_page = FIRST_PAGE;
    create_net_sub_details_one_page(p_icon_box);
    mbox_set_focus(p_icon_box, 3);
  }
  
  ctrl_paint_ctrl(ctrl_get_parent(p_icon_box), TRUE);
  
}

static RET_CODE on_youporn_pwdlg_cancel(control_t *p_ctrl,
												u16 msg,
							                                u32 para1,
							                                u32 para2)
{
  u16 key;
  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, key, 0, 0);

  ui_comm_pwdlg_close();
  return SUCCESS;
}


static RET_CODE on_youporn_pwdlg_correct(control_t *p_ctrl,
							                                 u16 msg,
							                                 u32 para1,
							                                 u32 para2)
{
  ui_comm_pwdlg_close();
  enter_youporn();
  ctrl_get_proc(fw_get_focus())(fw_get_focus(), MSG_PAINT, TRUE, 0);

  return SUCCESS;
}

static RET_CODE on_youporn_pwdlg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_pwdlg_close();
  return SUCCESS;
}

BEGIN_KEYMAP(icon_mbox_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(icon_mbox_keymap, NULL)

BEGIN_MSGPROC(icon_mbox_proc, mbox_class_proc)
  ON_COMMAND(MSG_FOCUS_LEFT, on_icon_box_change_focus)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_icon_box_change_focus)
  ON_COMMAND(MSG_SELECT, on_icon_box_select)
END_MSGPROC(icon_mbox_proc, mbox_class_proc)

BEGIN_KEYMAP(youporn_pwdlg_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(youporn_pwdlg_keymap, NULL)

BEGIN_MSGPROC(youporn_pwdlg_proc, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_youporn_pwdlg_cancel)
  ON_COMMAND(MSG_FOCUS_DOWN, on_youporn_pwdlg_cancel)
  ON_COMMAND(MSG_CORRECT_PWD, on_youporn_pwdlg_correct)
  ON_COMMAND(MSG_EXIT, on_youporn_pwdlg_exit)
END_MSGPROC(youporn_pwdlg_proc, cont_class_proc)

