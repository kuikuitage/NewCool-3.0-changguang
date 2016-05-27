/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_xmainmenu.h"
#include "ui_pause.h"
#include "ui_signal.h"
#ifndef WIN32
#include "hal_gpio.h"
#endif
//ad
#include "ui_util_api.h"
//pic
#include "ui_picture_api.h"
#include "ui_picture_api.h"
#include "authorization.h"
//#include "tree.h"

enum main_menu_control_id
{
	IDC_INVALID = 0,
	IDC_TV = 1,
	IDC_VOD = 2,
	IDC_MPLAY = 3,
	IDC_SET = 4,
	IDC_TV_TEXT,
	IDC_VOD_TEXT,
	IDC_MPLAY_TEXT,
	IDC_SET_TEXT,
	IDC_SPACE1,
	IDC_SPACE2,
	IDC_SPACE3,
	IDC_SPACE4,
	IDC_SPACE5,
	IDC_TEXT_CONT,
	IDC_BTN_CONT,
};
static BOOL iptv_test_auth_timeout(void);
u16 xmain_menu_cont_keymap(u16 key);
RET_CODE xmain_menu_cont_proc(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);
RET_CODE xmain_menu_btn_proc(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);
u16 xmain_menu_btn_keymap(u16 key);

extern void ui_create_network_stat(void);
extern void ui_close_network_stat(void);

RET_CODE mainmenu_paint_btn(u16 id)
{
	control_t *p_ctrl;
	p_ctrl = fw_find_root_by_id(ROOT_ID_MAINMENU);
	if(NULL==p_ctrl)
		return ERR_FAILURE;
	p_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_BTN_CONT);
	p_ctrl = ctrl_get_child_by_id(p_ctrl, id);
	ctrl_paint_ctrl(p_ctrl, TRUE);
	return SUCCESS;
}

RET_CODE open_main_menu(u32 para1, u32 para2)
{
	control_t *p_cont, *p_btn, *p_btn_bg,*p_text, *p_text_bg, *p_space;
	u16 i;
	u16 ids_item[MAINMENU_BTN_CNT] = {IDS_TV,IDS_VOD, IDS_MULTI_MEDIA,IDS_SETUP};
	u32 rstyle_item[MAINMENU_BTN_CNT][3] = 
	{
		{RSI_MMENU_TV,RSI_MMENU_TV_F,RSI_MMENU_TV},
		{RSI_MMENU_VOD,RSI_MMENU_VOD_F,RSI_MMENU_VOD},
		{RSI_MMENU_MPLAY,RSI_MMENU_MPLAY_F,RSI_MMENU_MPLAY},
		{RSI_MMENU_SET,RSI_MMENU_SET_F,RSI_MMENU_SET},
	};
	p_cont = fw_create_mainwin(ROOT_ID_MAINMENU,
							   MMENU_CONT_X, MMENU_CONT_Y,
							   MMENU_CONT_W, MMENU_CONT_H+MAINMENU_FONT_H,
							   ROOT_ID_INVALID, 0,
							   OBJ_ATTR_ACTIVE, 0);
	if(p_cont == NULL)
	{
	  return ERR_FAILURE;
	}

	//option name bg
	p_text_bg = ctrl_create_ctrl(CTRL_CONT,IDC_TEXT_CONT,
										 0, 0,
										 MMENU_CONT_W, MAINMENU_FONT_H,
										 p_cont, 0);
	ctrl_set_rstyle(p_text_bg, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

	//option button bg
	p_btn_bg = ctrl_create_ctrl(CTRL_CONT,IDC_BTN_CONT,
										 0, MAINMENU_FONT_H,
										 MMENU_CONT_W, MMENU_CONT_H,
										 p_cont, 0);
	ctrl_set_rstyle(p_btn_bg, RSI_MAINMENU_BG, RSI_MAINMENU_BG, RSI_MAINMENU_BG);
	// create 
	for(i=0; i<MAINMENU_BTN_CNT; i++)
	{
		//option name
		p_text = ctrl_create_ctrl(CTRL_TEXT, (u16)(IDC_TV_TEXT+ i),
							  MAINMENU_TV_X+(MAINMENU_BTN_W+SPACE_W)*i, 0, 
							  MAINMENU_BTN_W,MAINMENU_FONT_H,
							  p_text_bg, 0);
		ctrl_set_rstyle(p_text, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
		#ifdef BIGFONT_PROJECT
		text_set_font_style(p_text, FSI_WHITE_56, FSI_WHITE_56, FSI_WHITE_56);
		#else
		text_set_font_style(p_text, FSI_WHITE_45, FSI_WHITE_45, FSI_WHITE_45);
		#endif
		text_set_align_type(p_text, STL_CENTER | STL_VCENTER);
		text_set_content_type(p_text, TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_text, ids_item[i]);
		ctrl_set_sts(p_text,((i==0)?(OBJ_STS_SHOW):(OBJ_STS_HIDE)));
		//option button
		p_btn = ctrl_create_ctrl(CTRL_CONT, (u16)(IDC_TV+ i),
							  MAINMENU_TV_X+(MAINMENU_BTN_W+SPACE_W)*i, 0, 
							  MAINMENU_BTN_W,MAINMENU_BTN_H,
							  p_btn_bg, 0);
		ctrl_set_rstyle(p_btn, rstyle_item[i][0], rstyle_item[i][1], rstyle_item[i][2]);
		ctrl_set_keymap(p_btn, xmain_menu_btn_keymap);
		ctrl_set_proc(p_btn, xmain_menu_btn_proc);
		ctrl_set_related_id(p_btn, ((0==i)?(IDC_TV+MAINMENU_BTN_CNT-1):(IDC_TV+ i-1)),
									0,(((MAINMENU_BTN_CNT-1)==i)?(IDC_TV):(IDC_TV+ i+1)),0);
	}

	//space
	for(i=0; i<MAINMENU_BTN_CNT+1; i++)
	{
		p_space = ctrl_create_ctrl(CTRL_TEXT, (u16)(IDC_SPACE1+ i),
							  MAINMENU_TV_X+(MAINMENU_BTN_W+SPACE_W)*i-SPACE_W, 0, 
							  SPACE_W,MAINMENU_BTN_H,
							  p_btn_bg, 0);
		ctrl_set_rstyle(p_space, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
	}
	
    #ifdef ENABLE_ADS
	ui_adv_pic_play(ADS_AD_TYPE_MENU_UP, ROOT_ID_MAINMENU);
    #endif
  
	ctrl_default_proc(ctrl_get_child_by_id(p_btn_bg,IDC_TV), MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(p_cont, TRUE);

	ui_create_network_stat();

	return SUCCESS;

}

static RET_CODE on_mainmenu_btn_change_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_parent, *p_next,*p_prev_text, *p_next_text;
	u16 prev_id,next_id;

	p_parent = ctrl_get_parent(p_ctrl);
	switch(msg)
	{
		case MSG_FOCUS_LEFT:
			p_next = ctrl_get_child_by_id(p_parent, p_ctrl->left);
			break;
		case MSG_FOCUS_RIGHT:
			p_next = ctrl_get_child_by_id(p_parent, p_ctrl->right);
			break;
		default:
			return ERR_FAILURE;
			break;
	}
	p_parent = ctrl_get_root(p_ctrl);
	prev_id = ctrl_get_ctrl_id(p_ctrl);
	next_id = ctrl_get_ctrl_id(p_next);
	p_parent = ctrl_get_child_by_id(p_parent, IDC_TEXT_CONT);
	p_prev_text = ctrl_get_child_by_id(p_parent,prev_id+MAINMENU_BTN_CNT);
	p_next_text = ctrl_get_child_by_id(p_parent,next_id+MAINMENU_BTN_CNT);
	ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0); 
	ctrl_process_msg(p_next, MSG_GETFOCUS, 0, 0); 
	ctrl_set_sts(p_prev_text, OBJ_STS_HIDE);
	ctrl_set_sts(p_next_text, OBJ_STS_SHOW);
	ctrl_paint_ctrl(p_parent,TRUE);
	ctrl_paint_ctrl(p_ctrl,FALSE);
	ctrl_paint_ctrl(p_next,FALSE);
	return SUCCESS;
}

static RET_CODE on_exit_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 view_id = 0;
  u16 count = 0;
  u8 mode =0 ;

  mode = sys_status_get_curn_prog_mode();
  switch(mode)
	{
	  case CURN_MODE_TV:
		  {
			 view_id = ui_dbase_get_pg_view_id();
			 count = db_dvbs_get_count(view_id);
			if(!count)
			 {
			 view_id = ui_dbase_create_view(DB_DVBS_ALL_HIDE_TV, 0, NULL);
			 count = db_dvbs_get_count(view_id);
			  if(!count)
			  {
			   view_id = ui_dbase_create_view(DB_DVBS_ALL_HIDE_RADIO, 0, NULL);
			   count = db_dvbs_get_count(view_id);
			   if(count)
			   {
				sys_status_set_curn_prog_mode(CURN_MODE_RADIO);
				ui_dbase_set_pg_view_id(view_id);
				sys_status_save();
			   }
			  }
			  else
				{
				   ui_dbase_set_pg_view_id(view_id);
				   sys_status_set_curn_group(0);
				   sys_status_save();
				}
			}
		  }
	  break;
	  case CURN_MODE_RADIO:
		{
			 view_id = ui_dbase_get_pg_view_id();
			 count = db_dvbs_get_count(view_id);
			if(!count)
			{
			 view_id = ui_dbase_create_view(DB_DVBS_ALL_HIDE_RADIO, 0, NULL);
			 count = db_dvbs_get_count(view_id);
			if(!count)
			{
			   view_id = ui_dbase_create_view(DB_DVBS_ALL_HIDE_TV, 0, NULL);
			   count = db_dvbs_get_count(view_id);
			   if(count)
			   {
				sys_status_set_curn_prog_mode(CURN_MODE_TV);
				ui_dbase_set_pg_view_id(view_id);
				sys_status_save();
			   }
			}
			else
			 {
			   ui_dbase_set_pg_view_id(view_id);
			   sys_status_set_curn_group(0);
			   sys_status_save();
			 }
		  }
		}
	  break;
	  default:
		break;
	}
  
  if((sys_status_get_curn_prog_mode() == CURN_MODE_NONE)||((sys_status_get_curn_prog_mode() != CURN_MODE_NONE)&&(count == 0)))
  {
	if(ui_get_play_prg_type() == SAT_PRG_TYPE)
	{
	  ui_set_front_panel_by_str("----");
	  ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_PROG, NULL, 0);
	  return SUCCESS;
	}
  }
  
  ui_close_network_stat();

  manage_close_menu(ROOT_ID_MAINMENU, 0, 0);
 // ui_close_logo_bg(); //for bug 33999
  return ERR_NOFEATURE;
}
static BOOL xmain_check_usb_stat(void)
{
  return (BOOL)(ui_get_usb_status());
}

static RET_CODE xmain_show_dlg(s16 x,s16 y,s16 w,s16 h,s16 time,u16 text)
{
	rect_t rec;
	rec.left = x;
	rec.right = x+w;
	rec.top = y;
	rec.bottom = y+h;
	ui_comm_cfmdlg_open(&rec, text, NULL, time);	
	return SUCCESS;
}
BOOL xmian_check_network_stat(void)
{
  net_conn_stats_t net_stat;
  net_stat = ui_get_net_connect_status();
  #ifdef WIN32
  return TRUE;
  #else
  return (net_stat.is_eth_insert && net_stat.is_eth_conn)
	  	  || (net_stat.is_usb_eth_insert && net_stat.is_usb_eth_conn)		  
  	      || (net_stat.is_wifi_insert && net_stat.is_wifi_conn)  	      
          || (net_stat.is_3g_insert && net_stat.is_3g_conn)
  	      || (net_stat.is_gprs_conn);

  #endif
}

/*char p_buff[2048];="<?xml version=\"1.0\" encoding=\"gb2312\" standalone=\"yes\" ?>\n\
<modifys num=\"2\">\n\
	<modify name=\"menu\">\n\
		<url>http://192.168.2.12/test/1.jpg</url>\n\
	</modify>\n\
	<modify name=\"setting\">\n\
		<url>http://192.168.2.12/test/1.jpg</url>\n\
	</modify>\n\
</modifys>";
static void get_image_httphead(char* url)
{
   // HTTP_header_t *p_hdr = NULL;
   // int fd = 0;
  //  int length = 0;
    //int format_length = 50;
   // char *p_buff = NULL;
       xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr type;
	RET_CODE ret;
    u8*aaa,*bbb;
    if((NULL == url)||(strstr(url, "http://") == NULL))
    {
        return ;
    }
    
    //fd = chunkhttp_download_start(url, &p_hdr);
   // if(fd >= 0)
 //   {
    //    p_buff = mtos_malloc(p_hdr->content_length);
    //}
   // if(fd >= 0)
  //  {
   ///        length =  chunkhttp_recv(fd, p_buff, p_hdr->content_length, p_hdr->isChunked);
   //         OS_PRINTF("$$@@@@%s():%d:   p_buff =xml(%s)   format_length =%d  length = %d\n", __FUNCTION__, __LINE__,p_buff,p_hdr->content_length,length);
    //        mthttp_http_free(p_hdr);
    //        chunkhttp_close(fd);
    //}

	sprintf(p_buff,"<?xml version=\"1.0\" encoding=\"gb2312\" standalone=\"yes\" ?>\n"
	"<modifys num=\"2\">\n"
	"<modify name=\"menu\">\n"
	"<url>http://192.168.2.12/test/1.jpg</url>\n"
	"</modify>\n"
	"<modify name=\"setting\">\n"
	"<url>http://192.168.2.12/test/1.jpg</url>\n"
	"</modify>\n"
	"</modifys>\n");
	if(NULL==p_buff)
	{
		return ERR_FAILURE;
	}
	doc = xmlParseMemory((char*)p_buff, sizeof(p_buff));
	if(NULL==doc)
	{
		return ERR_FAILURE;
	}
	root = doc->root;
	if(NULL==root)
	{
		xmlFreeDoc(doc);
	}
    bbb = root->properties->name;
       aaa = xmlNodeGetContent(root->properties->val);
	type = root->childs;
	//ret = ads_xml_type_parse(type, p_ads_pic_data, buf_len);
	xmlFreeDoc(doc);*/
/*    
mul_pic_param_t pic_param = {{0},};
    ui_pic_init(SOURCE_NET);
    pic_stop();
    ui_pic_set_url(url);
    pic_param.anim = REND_ANIM_NONE;
    pic_param.style = REND_STYLE_CENTER;
    pic_param.flip = PIC_NO_F_NO_R;
    pic_param.win_rect.left = 0;
    pic_param.win_rect.top = 0;
    pic_param.win_rect.right = (pic_param.win_rect.left + 500);
    pic_param.win_rect.bottom = (pic_param.win_rect.top + 500);
    pic_param.file_size = 0;
    pic_param.handle = ((2 << 16) | (12  + 1));
    pic_param.is_thumb = TRUE;
    pic_start(&pic_param);       
    return;
}
*/ 

static RET_CODE on_mainmenu_btn_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u8 ctrlID;
	RET_CODE ret = ERR_FAILURE;
	
	ctrlID = ctrl_get_ctrl_id(ctrl_get_active_ctrl(p_ctrl->p_parent));
	switch(ctrlID)
	{

		case IDC_TV:
			on_exit_all(p_ctrl, msg, para1, para2);
			break;
		case IDC_VOD:
			if(xmian_check_network_stat())
			{
				if(FALSE == iptv_test_auth_timeout())
				{
					xmain_show_dlg(SYS_DLG_FOR_CHK_X,SYS_DLG_FOR_CHK_Y,PWDLG_W,PWDLG_H,2000,IDS_NC_AUTH_NOT_REGISTERED);
					break;
				}
#ifdef   NC_AUTH_EN			
				NC_STATUS_t auth_state= NC_AuthGetState();
				if (NC_AUTH_UNKNOW == auth_state)
					ret = xmain_show_dlg(SYS_DLG_FOR_CHK_X,SYS_DLG_FOR_CHK_Y,PWDLG_W,PWDLG_H,2000,IDS_NC_AUTH_NOT_REGISTERED);
				else if (NC_AUTH_REGISTERED == auth_state)
					ret = xmain_show_dlg(SYS_DLG_FOR_CHK_X,SYS_DLG_FOR_CHK_Y,PWDLG_W,PWDLG_H,2000,IDS_NC_AUTH_NOT_LOGIN);
				else if (NC_AUTH_BEATING_FAILURE == auth_state)
					ret = xmain_show_dlg(SYS_DLG_FOR_CHK_X,SYS_DLG_FOR_CHK_Y,PWDLG_W,PWDLG_H,2000,IDS_NC_AUTH_BEAT_FAILURE);
				else{
					if(!ui_signal_is_lock()&& ((CUSTOMER_ID == CUSTOMER_MAIKE_HUNAN) || (CUSTOMER_ID == CUSTOMER_MAIKE_HUNAN_LINLI)))
						ret = xmain_show_dlg(SYS_DLG_FOR_CHK_X,SYS_DLG_FOR_CHK_Y,PWDLG_W,PWDLG_H,2000,IDS_MSG_NO_SIGNAL);
					else
						ret = manage_open_menu(ROOT_ID_IPTV, 0, 0);
				}
					
#else
                ret = manage_open_menu(ROOT_ID_IPTV, 0, 0);
#endif

				//  get_image_httphead("http://192.168.2.12/test/a.xml");
			}
            else
			{
				ret = xmain_show_dlg(SYS_DLG_FOR_CHK_X,SYS_DLG_FOR_CHK_Y,PWDLG_W,PWDLG_H,2000,IDS_NET_NOT_CONNECT);
			}
			xmian_check_network_stat();
        
			break;
		case IDC_MPLAY:
			//get_image_httphead("http://192.168.2.12/test/1.png");
			//xmain_check_usb_stat();
			if(xmain_check_usb_stat())
				ret = manage_open_menu(ROOT_ID_USB_FILEPLAY, 0, 0);
			else
				ret = xmain_show_dlg(SYS_DLG_FOR_CHK_X,SYS_DLG_FOR_CHK_Y,PWDLG_W,PWDLG_H,2000,IDS_USB_DISCONNECT);
			break;
			
		case IDC_SET:
				ret = manage_open_menu(ROOT_ID_XSYS_SET, 0, 0);
			break;
		default:
			break;
	}
	return SUCCESS;
}


/*iptv test during time*/

static u8 get_month_from_compile_time(char *time)
{
	u8 month = 0;
	
	switch(time[0])
	{
		case 'J':
			if(time[1] == 'a')
				month = 1;
			else if(time[2] == 'n')
				month = 6;
			else if(time[2] == 'l')
				month = 7;
			break;
		case 'F':
			month = 2;
			break;
		case 'M':
			if(time[2] == 'r')
				month = 3;
			else if(time[2] == 'y')
				month = 5;
			break;
		case 'A':
			if(time[1] == 'p')
				month = 4;
			else if(time[1] == 'u')
				month = 8;
			break;
		case 'S':
			month = 9;
			break;
		case 'O':
			month = 10;
			break;
		case 'N':
			month = 11;
			break;
		case 'D':
			month = 12;
			break;
		default:
			break;
	}

	DEBUG(MAIN,INFO,"month = %d \n",month);
	return month;
}

static u16 get_digital_from_compile_time(char *time)
{
	u8 i;
	u16 num;

	for(i=0,num=0; i<strlen(time); i++)
	{
		if(time[i]<0x3a && time[i]>0x2F)
			num = num*10 + (time[i] - 0x30);
	}

	DEBUG(MAIN,INFO,"num = %d \n",num);
	return num;	
}

//#define IPTV_TEST_AUTH_TIMEOUT
#define IPTV_TEST_AUTH_TIME		(3)

extern unsigned long _mktime (unsigned int year, unsigned int mon,unsigned int day, unsigned int hour,unsigned int min, unsigned int sec);

static BOOL iptv_test_auth_timeout(void)
{
#ifdef IPTV_TEST_AUTH_TIMEOUT
	u8 date[16] = {0};
	u8 time[3][5] = {{0},{0},{0}};
	u8 index, i, j;
	u32 curTime, expTime;
	utc_time_t utc_time = {0}, utc_expired = {0};
	time_set_t p_set={{0}};

	sprintf(date,"%s",__DATE__);
	DEBUG(MAIN,INFO,"%s (%d)\n",date,strlen(date));

	if(strlen(date) == 0)
		return FALSE;

	for(i=0, j=0, index=0; i<strlen(date);i++)
	{
		if(date[i] == 0x20)
		{
			time[index][j] = 0x00;
			j=0;
			index++;
			continue;
		}
		time[index][j] = date[i];
		j++;
	}

	utc_expired.month = get_month_from_compile_time(time[0]);
	if(utc_expired.month == 0 || utc_expired.month > 12)
		return FALSE;

	utc_expired.day  = (u8)get_digital_from_compile_time(time[1]);
	if(utc_expired.day > 31)
		return FALSE;
	
	utc_expired.year =     get_digital_from_compile_time(time[2]);

	if(utc_expired.month + IPTV_TEST_AUTH_TIME <= 12)
	{
		utc_expired.month += IPTV_TEST_AUTH_TIME;
	}
	else
	{
		utc_expired.month = utc_expired.month + IPTV_TEST_AUTH_TIME -12;
		utc_expired.year += 1;
	}

	sys_status_get_time(&p_set);
	time_get(&utc_time, p_set.gmt_usage);
	curTime = _mktime(utc_time.year,utc_time.month,utc_time.day,0,0,0);
	DEBUG(DBG,INFO, "begin_date :%d-%d-%d\n",utc_time.year,utc_time.month,utc_time.day);
	DEBUG(DBG,INFO, "expire_date:%d-%d-%d\n",utc_expired.year,utc_expired.month,utc_expired.day);
	
	expTime = _mktime(utc_expired.year,utc_expired.month,utc_expired.day,0,0,0);

	expTime += 173*24*60*60;
	DEBUG(MAIN,INFO,"expTime = %x  curTime = %x\n",expTime,curTime);

	if(expTime < curTime)
		return FALSE;
#else
	(void)get_digital_from_compile_time;
	(void)get_month_from_compile_time;
#endif
	return TRUE;
}

/*================================================================================================
                           xmian net state internel function
 ================================================================================================*/
enum net_state_control_id
{
	IDC_NET_STATE_ICON = 1,
};

enum net_connect_state
{
        NET_CONNECT_INVALID = 0,
        NET_CONNECT_LINE = 1,
        NET_CONNECT_WIFI = 2,
};

static RET_CODE network_stat_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

static u8 ui_get_network_stat(void)
{
	net_conn_stats_t net_stat;
	net_stat = ui_get_net_connect_status();

	if((net_stat.is_eth_insert && net_stat.is_eth_conn)|| (net_stat.is_usb_eth_insert && net_stat.is_usb_eth_conn))
		return NET_CONNECT_LINE;
	else if((net_stat.is_wifi_insert && net_stat.is_wifi_conn)|| (net_stat.is_3g_insert && net_stat.is_3g_conn)|| (net_stat.is_gprs_conn))
		return NET_CONNECT_WIFI;
	else
		return NET_CONNECT_INVALID;
}

void ui_close_network_stat(void)
{
	fw_tmr_destroy(ROOT_ID_NET_STATE, MSG_PIC_FLASH);
	fw_destroy_mainwin_by_id(ROOT_ID_NET_STATE);
}

static RET_CODE ui_update_network_stat(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	control_t *p_icon;
	u8 state;
	net_config_t net_config;
	
	state = ui_get_network_stat();

	if(p_cont != NULL)
	{
		p_icon = ctrl_get_child_by_id(p_cont, IDC_NET_STATE_ICON);

		switch(state)
		{
			case NET_CONNECT_LINE:
				bmap_set_content_by_id(p_icon,  IM_NET_LINE_CONNECTED);
				break;
			case NET_CONNECT_WIFI:
				bmap_set_content_by_id(p_icon,  IM_NET_WIFI_CONNECTED);
				break;
			case NET_CONNECT_INVALID:
				sys_status_get_net_config_info(&net_config);
				if(net_config.link_type == LINK_TYPE_LAN)
					bmap_set_content_by_id(p_icon,  IM_NET_DISCONNECTED);
				else
					bmap_set_content_by_id(p_icon,  IM_NET_WIFI_DISCONNECTED);
				break;
			default:
				break;
		}
		ctrl_paint_ctrl(p_cont, TRUE);
	}
	return SUCCESS;
}

void ui_create_network_stat(void)
{
#define NET_STATE_CONT_FULL_X			(SCREEN_WIDTH - NET_STATE_CONT_FULL_W*2)
#define NET_STATE_CONT_FULL_Y			(40)
#define NET_STATE_CONT_FULL_W			(60)
#define NET_STATE_CONT_FULL_H			(60)

#define NET_STATE_ICON_X			(0)
#define NET_STATE_ICON_Y			(0)
#define NET_STATE_ICON_W			(NET_STATE_CONT_FULL_W)
#define NET_STATE_ICON_H			(NET_STATE_CONT_FULL_H)

	control_t *p_cont, *p_icon;
	OS_PRINTF("[NET] %s %d \n",__FUNCTION__,__LINE__);
	// check for close
	if(fw_find_root_by_id(ROOT_ID_NET_STATE) != NULL)
	{
		ui_close_network_stat();
	}

	p_cont = fw_create_mainwin(ROOT_ID_NET_STATE,
					                     NET_STATE_CONT_FULL_X, 
					                     NET_STATE_CONT_FULL_Y, 
					                     NET_STATE_CONT_FULL_W, 
					                     NET_STATE_CONT_FULL_H,
					                     ROOT_ID_INVALID, 0,
					                     OBJ_ATTR_INACTIVE, 0);
	if(p_cont == NULL)
	{
		return ;
	}
	ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
	ctrl_set_proc(p_cont, network_stat_cont_proc);

	p_icon = ctrl_create_ctrl(CTRL_BMAP, IDC_NET_STATE_ICON,
	                    NET_STATE_ICON_X, 
	                    NET_STATE_ICON_Y,
	                    NET_STATE_ICON_W, 
	                    NET_STATE_ICON_H,
	                    p_cont, 0);
	ctrl_set_rstyle(p_icon, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	bmap_set_align_type(p_icon, STL_CENTER | STL_VCENTER);
	ui_update_network_stat(p_cont, 0,0,0);
	fw_tmr_create(ROOT_ID_NET_STATE, MSG_PIC_FLASH, 2000, TRUE);
	OS_PRINTF("[NET] %s %d \n",__FUNCTION__,__LINE__);
}

#ifdef ENABLE_ADS
static RET_CODE on_mainmenu_item_refresh_ads(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  DEBUG(ADS,INFO,"on_item_refresh_ads\n");
  ui_adv_pic_play(ADS_AD_TYPE_MENU_UP, ROOT_ID_MAINMENU);
  return SUCCESS;
}
#endif

BEGIN_MSGPROC(network_stat_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_PIC_FLASH, ui_update_network_stat)
END_MSGPROC(network_stat_cont_proc, ui_comm_root_proc)



BEGIN_KEYMAP(xmain_menu_btn_keymap, NULL)
	ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
	ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(xmain_menu_btn_keymap, NULL)

BEGIN_MSGPROC(xmain_menu_btn_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_FOCUS_LEFT, on_mainmenu_btn_change_focus)
	ON_COMMAND(MSG_FOCUS_RIGHT, on_mainmenu_btn_change_focus)
	ON_COMMAND(MSG_SELECT, on_mainmenu_btn_select)
	ON_COMMAND(MSG_EXIT_ALL, on_exit_all)
	ON_COMMAND(MSG_EXIT, on_exit_all)
#ifdef ENABLE_ADS
  ON_COMMAND(MSG_REFRESH_ADS_PIC, on_mainmenu_item_refresh_ads)	
#endif
END_MSGPROC(xmain_menu_btn_proc, ui_comm_root_proc)

