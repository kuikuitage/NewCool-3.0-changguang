/****************************************************************************

****************************************************************************/

#include "ui_common.h"
#include "ui_live_tv.h"
#include "ui_live_tv_api.h"
#include "ui_video.h"
#include "ui_volume_usb.h"
#include "ui_mute.h"
#include "file_play_api.h"
#include "db_preset.h"
#include "Ui_dbase_api.h"
#include "ui_pause.h"

#define ENABLE_IPTV_EPG 0

enum live_tv_local_msg
{
	MSG_CHANGE_GROUP = MSG_LOCAL_BEGIN + 200,
	MSG_CHANGE_URL_SRC,
	MSG_HIDE_ALL_LIST,
	MSG_TIME_OUT_CHANGE_URL_SRC,
	MSG_SHOW_EPG_OR_SEEK_BAR,
	MSG_HIDE_EPG,
	MSG_ADD_FAV_PG,
	MSG_RECALL,
	MSG_CACEL_EXIT_WINDOW,
	MSG_SHOW_TODAY_EPG,
	MSG_ADD_URL_SRC,
	MSG_REDUCE_URL_SRC,
	MSG_HIDE_TODAY_EPG,
	MSG_SELECT_GROUP,
};

enum live_tv_ctrl_id
{
	IDC_LIVE_TV_LIST_CONT = 1,
	IDC_LIVE_TV_GROUP_CONT,
	IDC_LIVE_TV_GROUP_LEFT,
	IDC_LIVE_TV_GROUP_RIGHT,
	IDC_LIVE_TV_GROUP,
	IDC_LIVE_TV_LIST_BAR,
	IDC_LIVE_TV_LIST,
	IDC_LIVE_TV_NUM_FRM,
	IDC_LIVE_TV_NUM_TXT,
	IDC_LIVE_TV_CHANGE_URL_SRC_PROG_NAME = 10,
	IDC_LIVE_TV_CHANGE_URL_SRC_INDEX,
	IDC_LIVE_TV_HELP_BAR,
	IDC_LIVE_TV_HELP_BMP,
	IDC_LIVE_TV_HELP_TEXT,
	IDC_LIVE_TV_HELP_BMP2,
	IDC_LIVE_TV_HELP_TEXT2,
	IDC_LIVE_TV_EXIT_CONT,
	IDC_LIVE_TV_EXIT_TEXT,
	IDC_LIVE_TV_BPS_CONT_CONT,
	IDC_LIVE_TV_BPS_CONT,
	IDC_LIVE_TV_BPS = 20,
	IDC_LIVE_TV_BUF_PERCENT,
	IDC_LIVE_TV_LOADING_MEDIA_TIME_CONT,
	IDC_LIVE_TV_LOADING_MEDIA_TIME,
};


enum live_tv_epg_info_ctrl_id
{
	IDC_LIVE_TV_EPG_CONT = 24,
	IDC_EPG_CURR_P,
	IDC_EPG_CURR_P_NAME,
	IDC_EPG_CURR_TIME,
	IDC_EPG_CURR_TIME_TEXT,
	IDC_EPG_NET_CONN,
	IDC_EPG_NET_CONN_TEXT,
	IDC_EPG_CURR_PROG,
	IDC_EPG_CURR_PROG_NAME,
	IDC_EPG_URL_SRC,
	IDC_EPG_URL_SRC_NUM,
	IDC_EPG_NEXT_PROG,
	IDC_EPG_NEXT_PROG_NAME,
	IDC_EPG_CHANGE_PROG = 37,

};

enum live_tv_today_epg_ctrl_id
{
	IDC_LIVE_TV_TODAY_EPG_CONT = 38,
	IDC_TODAY_EPG_CURR_P,
	IDC_TODAY_EPG_CURR_P_NAME,
	IDC_TODAY_EPG_EVT_LIST,
	IDC_TODAY_EPG_EVT_BAR = 42,
};

enum live_tv_group_list_ctrl_id

{
    IDC_LIVE_TV_GP_LIST_CONT = 43,
    IDC_LIVE_TV_GP_GROUP_TITLE_CONT,
    IDC_LIVE_TV_GP_GROUP_LEFT,
    IDC_LIVE_TV_GP_GROUP_RIGHT,
    IDC_LIVE_TV_GP_LIST_BAR,
    IDC_LIVE_TV_GP_LIST,
    IDC_LIVE_TV_GP_HELP_BAR,
    IDC_LIVE_TV_GP_HELP_BMP,
    IDC_LIVE_TV_GP_HELP_TEXT,
    IDC_LIVE_TV_GP_HELP_BMP2,
    IDC_LIVE_TV_GP_HELP_TEXT2,

};

static list_xstyle_t live_tv_list_item_rstyle =
{
	RSI_PBACK,
	RSI_ITEM_2_SH,
	RSI_ITEM_2_HL,
	RSI_ITEM_8_BG,
	RSI_ITEM_2_HL,
};

static list_xstyle_t live_tv_list_field_rstyle =
{
	RSI_PBACK,
	RSI_PBACK,
	RSI_PBACK,
	RSI_PBACK,
	RSI_PBACK,
};

static list_xstyle_t live_tv_list_field_fstyle =
{
	FSI_GRAY,
	FSI_WHITE,
	FSI_WHITE,
	FSI_BLACK,
	FSI_WHITE,
};

static list_xstyle_t live_tv_epg_list_item_rstyle =
{
	RSI_PBACK,
	RSI_ITEM_2_SH,
	RSI_ITEM_2_HL,
	RSI_ITEM_8_BG, 
	RSI_ITEM_2_HL,
};

static list_xstyle_t live_tv_epg_list_field_rstyle =
{
	RSI_PBACK,
	RSI_PBACK,
	RSI_PBACK,
	RSI_PBACK,
	RSI_PBACK,
};

static list_xstyle_t live_tv_epg_list_field_fstyle =
{
	FSI_WHITE,
	FSI_WHITE,
	FSI_WHITE,
	FSI_BLACK,
	FSI_WHITE,
};

static list_field_attr_t live_tv_play_list_attr[LIVE_TV_LIST_FIELD] =
{
	{ LISTFIELD_TYPE_UNISTR,
	55, 15, 0, &live_tv_list_field_rstyle,  &live_tv_list_field_fstyle},
	{ LISTFIELD_TYPE_UNISTR|STL_LEFT,
	190, 75, 0, &live_tv_list_field_rstyle,  &live_tv_list_field_fstyle},
	{ LISTFIELD_TYPE_ICON | STL_LEFT,
	17, 270, 0, &live_tv_list_field_rstyle,  &live_tv_list_field_fstyle}
};


static list_field_attr_t live_tv_epg_list_attr[LIVE_TV_EPG_LIST_FIELD] =
{
	{ LISTFIELD_TYPE_UNISTR,
	45, 10, 0, &live_tv_epg_list_field_rstyle,  &live_tv_epg_list_field_fstyle},
	{ LISTFIELD_TYPE_UNISTR|STL_LEFT,
	232, 60, 0, &live_tv_epg_list_field_rstyle,  &live_tv_epg_list_field_fstyle},
};

typedef struct
{
	u8 all_grp;
	u8 fav_grp;
	u8 cus_grp;
}ui_livetv_grp_index;

typedef struct
{
    control_t *p_menu;
    control_t *p_list_cont;
    control_t *p_list;  //prog list
    control_t *p_grp; 
    control_t *p_gp_list_cont;
    control_t *p_gp_list;  //prog list
    control_t *p_gp_grp; 
    control_t *p_epg_frm;
    control_t *p_exit_info_cont;
    control_t *p_num_play;
    control_t *p_help;
    control_t *p_gp_help;
    control_t *p_today_epg_cont;
    control_t *p_today_epg;
    control_t *p_bps_cont_cont;
    control_t *p_bps_cont;
    control_t *p_bps;
    control_t *p_percent;
    control_t *p_load_time_cont;
    control_t *p_load_time;

    u8 livetv_grp_cnt;
    u8 is_live_exit_state;
    u8 g_num_play_bit;
    u8 g_circle_index;//for change url src one circle
    u16 g_vcircle_index;//for change prog one circle
    u16 g_livetv_all_pg_cnt;
    u16 g_playing_focus;
    u16 max_net_pg_id;
    u16 cus_url_total;
    BOOL g_is_play_stoped;
    BOOL is_exit_play;
    BOOL is_left;//for manually change url src, record left right key
    BOOL is_db_done;//if true, network channel get done
    BOOL is_loading_epg;
    BOOL is_auto_change; //if TRUE means auto change url, else means manually change url src
    ui_livetv_grp_index grp_index; 
    ui_epg_chan_info_t *p_epg; //curr epg info

    custom_url_t *p_url_array;
    custom_url_t *cus_url_tbl;
    u8 *p_url_buf;
    today_epg_info_t g_t_epg; //today epg
    ui_live_tv_time_t days[LIVE_TV_LOOK_BACK_DAY];
    live_db_prg_t g_prev_pg;
    live_db_prg_t g_curr_pg;
}ui_livetv_t;

static ui_livetv_t g_livetv = {0};

static u16 g_cur_group = INVALIDID;

u16 live_tv_mainwin_keymap(u16 key);
u16 live_tv_list_keymap(u16 key);
u16 live_tv_num_play_cont_keymap(u16 key);
u16 live_tv_epg_list_keymap(u16 key);
u16 live_tv_group_list_keymap(u16 key);

RET_CODE live_tv_mainwin_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE live_tv_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE live_tv_group_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

RET_CODE live_tv_num_play_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE live_tv_epg_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static RET_CODE live_tv_today_epg_list_update(control_t* p_list, u16 start, u16 size, u32 context);

#if ENABLE_CUSTOM_URL
extern u16 load_custom_url(custom_url_t** pp_url_array,u8 *buf,custom_url_t *customs_url_tbl);
#endif
static void show_epg_bar_while_change_prog();
static void live_tv_jump_to_prog(control_t *p_cont);
static RET_CODE on_live_tv_list_change_focus_or_program(control_t *p_ctrl, 
														u16 msg,u32 para1, u32 para2);
static RET_CODE on_live_tv_show_epg_info_bar(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE on_live_tv_hide_all_list(control_t *p_list, u16 msg, u32 para1, u32 para2);
static void live_tv_update_loading_media_time(control_t* p_ctrl, u32 second);
static RET_CODE on_live_tv_load_media_error(control_t *p_cont, u16 msg, u32 
											para1, u32 para2);
static void clear_show_bps_dlg();
static void clear_show_loading_media_time_dlg();
static void ui_livetv_get_utc_time();
static void ui_livetv_hide_bps_dlg();
static void ui_livetv_hide_loading_time_dlg();


static void ui_livetv_start_timer(u16 msg, u32 time_out)
{
	ui_start_timer(ROOT_ID_LIVE_TV, msg, time_out);
}

static void ui_livetv_stop_timer(u16 msg)
{
	ui_stop_timer(ROOT_ID_LIVE_TV, msg);
}

static void ui_livetv_reset_hide_all_list()
{
	ui_livetv_start_timer(MSG_HIDE_ALL_LIST, 10000);
}

live_db_prg_t *get_livetv_curr_pg()
{
	return &g_livetv.g_curr_pg;
}

void set_livetv_curr_pg(live_db_prg_t *pg)
{
	memcpy(&g_livetv.g_curr_pg, pg, sizeof(live_db_prg_t));
}

void ui_live_tv_set_exit_state(BOOL is_exit)
{
	g_livetv.is_exit_play = is_exit;
}

void ui_live_tv_get_exit_state(BOOL *is_exit)
{
	*is_exit = g_livetv.is_exit_play;
}

static RET_CODE live_tv_group_update(control_t *p_cbox, u16 focus, u16 *p_str,
									 u16 max_length)
{
	live_db_group_t p_group = {0};

	OS_PRINTF("####g_group_index == %d#\n",focus);
	if(focus == g_livetv.grp_index.all_grp)
	{
		u8 asc_str[16] = "All";
		convert_gb2312_chinese_asc2unistr(asc_str, p_str, sizeof(asc_str));
	}
	else if(focus == g_livetv.grp_index.fav_grp)
	{
		u8 asc_str[16] = "Fav";
		convert_gb2312_chinese_asc2unistr(asc_str, p_str, sizeof(asc_str));
	}
	else if(focus == g_livetv.grp_index.cus_grp)
	{
		u8 asc_str[16] = "Custom";
		convert_gb2312_chinese_asc2unistr(asc_str, p_str, sizeof(asc_str));
	}
	else //comm network group
	{
		livetv_get_group_info(&p_group, focus - 1);
		OS_PRINTF("####this group == %s#\n",p_group.name);
		convert_utf8_chinese_asc2unistr(p_group.name, p_str, sizeof(p_group.name));
	}

	return SUCCESS;
}

u16 get_cur_group_index(void)
{
	return g_cur_group;
}

static BOOL ui_livetv_check_net()
{
	net_conn_stats_t eth_connt_stats = {0};

	eth_connt_stats = ui_get_net_connect_status();
	if((eth_connt_stats.is_eth_conn == FALSE) &&
       (eth_connt_stats.is_usb_eth_conn == FALSE) &&
       (eth_connt_stats.is_wifi_conn == FALSE) &&
       (eth_connt_stats.is_3g_conn == FALSE))
	{
		return FALSE;
	}
	return TRUE;
}

static void live_tv_open_dlg(u16 str_id, u32 time_out)
{
	comm_dlg_data_t dlg_data =
	{
		0,
		DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
		COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
	};
	dlg_data.content = str_id;
	dlg_data.dlg_tmout = time_out;

	if(fw_find_root_by_id(ROOT_ID_POPUP) == NULL)
	{
		ui_comm_dlg_open(&dlg_data);
	}
}

static void ui_livetv_play_net_prog(u8 *p_net_url)
{
	live_tv_open_dlg(IDS_TO_PLAY_WAIT, 0);
      ui_livetv_hide_loading_time_dlg();
      ui_livetv_hide_bps_dlg();
	if(p_net_url != NULL)
	{
		if(strlen(p_net_url) && g_livetv.g_curr_pg.url_cnt)
		{
			OS_PRINTF("##%s, url == %s##\n", __FUNCTION__, p_net_url);
			ui_video_c_play_by_url(p_net_url, 0, OTT_LIVE_MODE);
		}
		else
		{
			on_live_tv_load_media_error(NULL, 0, 0, 0);
		}
	}
	else
	{
		on_live_tv_load_media_error(NULL, 0, 0, 0);
	}
}

void  set_cur_group_index(u16 group_index)
{
	g_cur_group = group_index;
}

void set_livetv_cur_playing_focus(u16 focus)
{
	g_livetv.g_playing_focus = focus;
}

static void live_tv_update_bps(control_t* p_bps, control_t *p_per, u16 cur_bps, u16 load_percent)
{
    u8 bps_asc[32] = {"Speed:"};
    u8 bps_string[8] = {"KB/s"};
    u8 load_asc[32] = {"Loading Percent:"};

    sprintf(bps_asc, "%s%d%s", bps_asc, cur_bps, bps_string);
    text_set_content_by_ascstr(p_bps, bps_asc);

    sprintf(load_asc, "%s%d%s", load_asc, load_percent, "\%");
    text_set_content_by_ascstr(p_per, load_asc);
    OS_PRINTF("##%s, bps = %s, load percent = %s##\n", __FUNCTION__, bps_asc, load_asc);
}

static control_t *create_mainwindow(void)
{
	control_t *p_mainwin = NULL;  

	p_mainwin = fw_create_mainwin(ROOT_ID_LIVE_TV,
		LIVE_TV_CONT_X, LIVE_TV_CONT_Y,
		LIVE_TV_CONT_W, LIVE_TV_CONT_H,
		ROOT_ID_INVALID, 0, OBJ_ATTR_ACTIVE, 0);
	MT_ASSERT(p_mainwin != NULL);

	ctrl_set_rstyle(p_mainwin, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
	ctrl_set_keymap(p_mainwin, live_tv_mainwin_keymap);
	ctrl_set_proc(p_mainwin, live_tv_mainwin_proc);
	g_livetv.p_menu = p_mainwin;

	return p_mainwin;
}

/*=================================================================
                                        create livetv program group list
===================================================================*/

static control_t *create_live_tv_prog_list_cont(control_t *p_mainwin)
{
	control_t *p_list_cont = NULL;

	p_list_cont = ctrl_create_ctrl(CTRL_CONT,
		IDC_LIVE_TV_LIST_CONT, LIVE_TV_LIST_CONT_X,
		LIVE_TV_LIST_CONT_Y, LIVE_TV_LIST_CONT_W,
		LIVE_TV_LIST_CONT_H, p_mainwin, 0);

	ctrl_set_rstyle(p_list_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	ui_livetv_reset_hide_all_list();
	g_livetv.p_list_cont = p_list_cont;
	
	return p_list_cont;
}

static void create_live_tv_prog_list_group(control_t *p_list_cont)
{
	control_t *p_group_cont = NULL,  *p_group = NULL;
	control_t *p_ctrl = NULL;
	u16 group_index = 0;
	u32 fav_num = 0;

	//group container
	p_group_cont =
		ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_GROUP_CONT, LIVE_TV_GROUP_CONT_X,
		LIVE_TV_GROUP_CONT_Y, LIVE_TV_GROUP_CONT_W,LIVE_TV_GROUP_CONT_H, p_list_cont, 0);

	ctrl_set_rstyle(p_group_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	
	//left arrow
	p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_LIVE_TV_GROUP_LEFT,
		LIVE_TV_GROUPL_X, LIVE_TV_GROUPL_Y,
		LIVE_TV_GROUPL_W, LIVE_TV_GROUPL_H,
		p_group_cont, 0);
	bmap_set_content_by_id(p_ctrl, IM_ARROW1_L);

	p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_LIVE_TV_GROUP_RIGHT,
		LIVE_TV_GROUPR_X, LIVE_TV_GROUPR_Y,
		LIVE_TV_GROUPR_W, LIVE_TV_GROUPR_H,
		p_group_cont, 0);
	bmap_set_content_by_id(p_ctrl, IM_ARROW1_R);

	//group
	p_group = ctrl_create_ctrl(CTRL_CBOX, IDC_LIVE_TV_GROUP, LIVE_TV_GROUP_X,
		LIVE_TV_GROUP_Y, LIVE_TV_GROUP_W,
		LIVE_TV_GROUP_H, p_group_cont, 0);
	ctrl_set_rstyle(p_group, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	cbox_set_font_style(p_group, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	cbox_set_align_style(p_group, STL_CENTER | STL_VCENTER);
	cbox_set_work_mode(p_group, CBOX_WORKMODE_DYNAMIC);
	cbox_enable_cycle_mode(p_group, TRUE);

	cbox_dync_set_count(p_group, g_livetv.grp_index.cus_grp + 1);//all grp cnt
	cbox_dync_set_update(p_group, live_tv_group_update);
	g_livetv.p_grp = p_group;
	group_index = get_cur_group_index();
	// OS_PRINTF("##%s, group index = [%d]##\n", __FUNCTION__, group_index);

	if(get_cur_group_index() == g_livetv.grp_index.fav_grp)
	{
		sys_status_get_fav_livetv_total_num(&fav_num);
		OS_PRINTF("##%s, fav num = [%d]##\n", __FUNCTION__, fav_num);
		if(fav_num == 0)
		{
			OS_PRINTF("##%s, line = [%d]##\n", __FUNCTION__, __LINE__);
			group_index = g_livetv.grp_index.all_grp;
			livetv_db_get_pos_by_id(&g_livetv.g_playing_focus, g_livetv.g_curr_pg.id);
			OS_PRINTF("##%s, line = [%d], playing fav id ==[%d], \
					  playing focus = [%d]##\n", __FUNCTION__, \
					  __LINE__,g_livetv.g_curr_pg.id, g_livetv.g_playing_focus);
		}
	}

	if(group_index == INVALIDID)
	{
		g_livetv.g_playing_focus = INVALIDID;
		//if(g_livetv.is_db_done && ui_livetv_check_net())
		{
			group_index = g_livetv.grp_index.all_grp;//first init net focus all group
		}
	}
	
	set_cur_group_index(group_index);
	cbox_dync_set_focus(p_group, group_index);  
	OS_PRINTF("##%s, group index = %d##\n", __FUNCTION__, group_index);
}

static RET_CODE live_tv_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
	u16 uni_str[128] = {0};
	u8 asc_str[32] = {0};
	live_tv_fav_info fav_livetv_info = {0};
	u16 i, j, cnt, group_focus;
	u32 fav_num = 0;
	live_db_prg_t p_pg = {0};

	cnt = list_get_count(p_list);
	group_focus = cbox_dync_get_focus(g_livetv.p_grp);

#ifndef WIN32
	for (i = start; i < start + size && i < cnt; i++)
	{
		/* icon. */
		list_set_field_content_by_icon(p_list, i, 2, 0);
		/* NAME */
		{
			if(group_focus < g_livetv.grp_index.fav_grp)
			{
				if(group_focus == 0)
				{
					livetv_get_pg_by_group_pos(&p_pg, i, INVALID_GRP);
				}
				else
				{
					livetv_get_pg_by_group_pos(&p_pg, i, group_focus - 1);
				}
				sys_status_get_fav_livetv_total_num(&fav_num);

				if(fav_num > 0)
				{
					for(j = 0; j < fav_num;j++)
					{
						sys_status_get_fav_livetv_info(j, &fav_livetv_info);
						if(fav_livetv_info.live_fav_id == p_pg.id)
						{
							list_set_field_content_by_icon(p_list, i, 2, IM_TV_FAV);
						}
					}
				}
				/* NO. */
				sprintf(asc_str, "%04d", p_pg.id);
				list_set_field_content_by_ascstr(p_list, i, 0, asc_str);
				convert_utf8_chinese_asc2unistr(p_pg.name, uni_str, sizeof(p_pg.name));
			}
			else if(group_focus == g_livetv.grp_index.fav_grp)//fav group
			{
				list_set_field_content_by_icon(p_list, i, 2, IM_TV_FAV);
				sys_status_get_fav_livetv_info(i, &fav_livetv_info);
				convert_utf8_chinese_asc2unistr(fav_livetv_info.live_fav_name, uni_str, sizeof(fav_livetv_info.live_fav_name));
				sprintf(asc_str, "%04d", (u16)fav_livetv_info.live_fav_id);
				list_set_field_content_by_ascstr(p_list, i, 0, asc_str);
			}
			else if(group_focus == g_livetv.grp_index.cus_grp)//customer group
			{
				if(g_livetv.cus_url_total > 0)
				{
					// sprintf(asc_str, "%04d", 1 + i + g_livetv.max_net_pg_id);
					sprintf(asc_str, "%04d", 1 + i);
					list_set_field_content_by_ascstr(p_list, i, 0, asc_str);
					if(g_livetv.p_url_array[i].name != NULL)
					{
						convert_gb2312_chinese_asc2unistr(g_livetv.p_url_array[i].name, \
							uni_str, sizeof(g_livetv.p_url_array[i].name));
					}
				}
			}
			
			list_set_field_content_by_unistr(p_list, i, 1, uni_str); 
		}
	}
#endif

	return SUCCESS;
}

static void create_live_tv_prog_list(control_t *p_list_cont)
{
	control_t  *p_list = NULL;
	control_t *p_list_bar = NULL;
	u16 i;  
	u32 count;
	u8 group_index;

	//list
	p_list = ctrl_create_ctrl(CTRL_LIST, IDC_LIVE_TV_LIST, LIVE_TV_LIST_X,
		LIVE_TV_LIST_Y, LIVE_TV_LIST_W, LIVE_TV_LIST_H, p_list_cont, 0);
	ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	ctrl_set_keymap(p_list, live_tv_list_keymap);
	ctrl_set_proc(p_list, live_tv_list_proc);

	ctrl_set_mrect(p_list, LIVE_TV_LIST_MIDL, LIVE_TV_LIST_MIDT,
		LIVE_TV_LIST_MIDW+LIVE_TV_LIST_MIDL, LIVE_TV_LIST_MIDH+LIVE_TV_LIST_MIDT);
	list_set_item_interval(p_list,LIVE_TV_LCONT_LIST_VGAP);
	list_set_item_rstyle(p_list, &live_tv_list_item_rstyle);
	list_enable_select_mode(p_list, TRUE);
	list_set_select_mode(p_list, LIST_SINGLE_SELECT);
	list_enable_page_mode(p_list, TRUE);
	g_livetv.p_list = p_list;
	group_index = get_cur_group_index();

	if(group_index < g_livetv.grp_index.fav_grp)
	{
		if(group_index == 0)
		{
			count = g_livetv.g_livetv_all_pg_cnt;
		}
		else
		{
			count = livetv_get_cnt_by_group(group_index - 1);
		}
	}
	else if(group_index == g_livetv.grp_index.fav_grp)//fav
	{
		sys_status_get_fav_livetv_total_num(&count);
	}
	else if(group_index == g_livetv.grp_index.cus_grp)
	{
		count = g_livetv.cus_url_total;
		OS_PRINTF("###customer total number == %d##\n",count);
	}
	
	list_set_count(p_list, count, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
	list_set_field_count(p_list, LIVE_TV_LIST_FIELD, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
	if(g_livetv.g_playing_focus == INVALIDID)
	{
		g_livetv.g_playing_focus = 0;
	}
	
	
	list_set_focus_pos(p_list, g_livetv.g_playing_focus);
	list_select_item(p_list, g_livetv.g_playing_focus);
	list_set_update(p_list, live_tv_list_update, 0);

	for (i = 0; i < LIVE_TV_LIST_FIELD; i++)
	{
		list_set_field_attr(p_list, (u8)i, (u32)(live_tv_play_list_attr[i].attr), (u16)(live_tv_play_list_attr[i].width),
			(u16)(live_tv_play_list_attr[i].left), (u8)(live_tv_play_list_attr[i].top));
	//	list_set_field_rect_style(p_list, (u8)i, live_tv_play_list_attr[i].rstyle);
		list_set_field_font_style(p_list, (u8)i, live_tv_play_list_attr[i].fstyle);
	}
	//scrollbar
	p_list_bar = ctrl_create_ctrl(CTRL_SBAR, IDC_LIVE_TV_LIST_BAR, LIVE_TV_LIST_BAR_X,
		LIVE_TV_LIST_BAR_Y, LIVE_TV_LIST_BAR_W, LIVE_TV_LIST_BAR_H, p_list_cont, 0);
	ctrl_set_rstyle(p_list_bar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
	sbar_set_autosize_mode(p_list_bar, 1);
	sbar_set_direction(p_list_bar, 0);
	sbar_set_mid_rstyle(p_list_bar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
	list_set_scrollbar(p_list, p_list_bar);

	live_tv_list_update(p_list, list_get_valid_pos(p_list), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);
	ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0); 

}

static void create_live_tv_prog_list_helpbar(control_t *p_mainwin)
{
	control_t *p_cont = NULL;
	control_t *p_bmp = NULL;
	control_t *p_text = NULL;
      u8 asc_str1[32]= {"[Fav]:Add fav program"};
      u8 asc_str2[32]= {"[Yellow]:Select group"};
      
	p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_HELP_BAR,
		LIVE_TV_HELP_FRM_X, LIVE_TV_HELP_FRM_Y,
		LIVE_TV_HELP_FRM_W, LIVE_TV_HELP_FRM_H,
		p_mainwin, 0);

	ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	g_livetv.p_help = p_cont;
	ctrl_set_sts(p_cont, OBJ_STS_SHOW);

      //fav bmp
	p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_LIVE_TV_HELP_BMP, 20, 0, 
		26, LIVE_TV_HELP_FRM_H/2, p_cont, 0);
	ctrl_set_rstyle(p_bmp, RSI_PBACK, RSI_PBACK, RSI_PBACK);
      bmap_set_align_type(p_bmp, STL_CENTER|STL_VCENTER);
	bmap_set_content_by_id(p_bmp, IM_TV_FAV);

        //fav text
	p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_HELP_TEXT,
		56, 0,
		LIVE_TV_HELP_FRM_W-66, LIVE_TV_HELP_FRM_H/2,
		p_cont, 0);
	ctrl_set_rstyle(p_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_text, STL_LEFT| STL_VCENTER);
	text_set_content_type(p_text, TEXT_STRTYPE_UNICODE);
	text_set_font_style(p_text, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
	text_set_content_by_ascstr(p_text, asc_str1);

        //select group bmp(yellow key)
	p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_LIVE_TV_HELP_BMP2, 20, 
	LIVE_TV_HELP_FRM_H/2+5, 
		25, 25, p_cont, 0);
	ctrl_set_rstyle(p_bmp, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	bmap_set_content_by_id(p_bmp, IM_EPG_COLORBUTTON_YELLOW);
      bmap_set_align_type(p_bmp, STL_CENTER|STL_VCENTER);

        //select group text
	p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_HELP_TEXT2,
		56, LIVE_TV_HELP_FRM_H/2,
		LIVE_TV_HELP_FRM_W-66, LIVE_TV_HELP_FRM_H/2,
		p_cont, 0);
	ctrl_set_rstyle(p_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_text, STL_LEFT| STL_VCENTER);
	text_set_content_type(p_text, TEXT_STRTYPE_UNICODE);
	text_set_font_style(p_text, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
	text_set_content_by_ascstr(p_text, asc_str2);
    
}


static void ui_livetv_create_prog_list(control_t *p_mainwin)
{
    control_t *p_list_cont = NULL;
    
    //create list cont
    p_list_cont = create_live_tv_prog_list_cont(p_mainwin);

    //create live_tv group
    create_live_tv_prog_list_group(p_list_cont);

    //create live_tv list
    create_live_tv_prog_list(p_list_cont);

    //create help bar
    create_live_tv_prog_list_helpbar(p_mainwin);
}



/*=================================================================
                                        create livetv jump group list
===================================================================*/
static control_t *create_live_tv_group_list_cont(control_t *p_mainwin)
{
    control_t *p_list_cont = NULL;

    p_list_cont = ctrl_create_ctrl(CTRL_CONT,
        IDC_LIVE_TV_GP_LIST_CONT, LIVE_TV_LIST_CONT_X,
        LIVE_TV_LIST_CONT_Y, LIVE_TV_LIST_CONT_W,
        LIVE_TV_LIST_CONT_H, p_mainwin, 0);

    ctrl_set_rstyle(p_list_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
    g_livetv.p_gp_list_cont = p_list_cont;
    ctrl_set_sts(p_list_cont, OBJ_STS_HIDE);
    
    return p_list_cont;
}

static void create_live_tv_group_list_curr_group_title(control_t *p_list_cont)
{
    control_t *p_title_cont = NULL;
    control_t *p_left = NULL;
    control_t *p_right = NULL;
    u8 asc_str[32] = {"Current Group:"};
    
    //group container
    p_title_cont = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_GP_GROUP_TITLE_CONT, LIVE_TV_GROUP_CONT_X,
        LIVE_TV_GROUP_CONT_Y, LIVE_TV_GROUP_CONT_W,LIVE_TV_GROUP_CONT_H, p_list_cont, 0);
    ctrl_set_rstyle(p_title_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
    
    //left text
    p_left = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_GP_GROUP_LEFT, 5,
        0, LIVE_TV_GROUP_CONT_W/2+25,LIVE_TV_GROUP_CONT_H, p_title_cont, 0);
    ctrl_set_rstyle(p_left, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_left, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_left, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_content_type(p_left, TEXT_STRTYPE_UNICODE);
    text_set_content_by_ascstr(p_left, asc_str);

    //right curr group name
    p_right = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_GP_GROUP_RIGHT, 
    LIVE_TV_GROUP_CONT_W/2+30,
        0, LIVE_TV_GROUP_CONT_W/2-30,LIVE_TV_GROUP_CONT_H, p_title_cont, 0);
    ctrl_set_rstyle(p_right, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    
    text_set_align_type(p_right, STL_LEFT| STL_VCENTER);
    text_set_font_style(p_right, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_content_type(p_right, TEXT_STRTYPE_UNICODE);
    g_livetv.p_gp_grp = p_right;
}

static RET_CODE live_tv_group_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
    u16 uni_str[128] = {0};
    u8 asc_str[32] = {0};
    u16 i,  cnt;
    live_db_group_t group = {0};

    cnt = list_get_count(p_list);

    for (i = start; i < start + size && i < cnt; i++)
    {
        /* NO. */
        sprintf(asc_str, "%03d", i + 1);
        list_set_field_content_by_ascstr(p_list, i, 0, asc_str);

        /* NAME */
        if(i == 0)
        {
            u8 ascstr[16] = "Fav";
            convert_gb2312_chinese_asc2unistr(ascstr, uni_str, sizeof(ascstr));
        }
        else if(i == 1)
        {
            u8 ascstr[16] = "Custom";
            convert_gb2312_chinese_asc2unistr(ascstr, uni_str, sizeof(ascstr));
        }      
        else if(i == 2)
        {
            u8 ascstr[16] = "All";
            convert_gb2312_chinese_asc2unistr(ascstr, uni_str, sizeof(ascstr));
        }
        else
        {
            livetv_get_group_info(&group, i - 3);
            OS_PRINTF("####this group == %s#\n",group.name);
            convert_utf8_chinese_asc2unistr(group.name, uni_str, sizeof(group.name));
        }
        list_set_field_content_by_unistr(p_list, i, 1, uni_str); 
        list_set_field_content_by_icon(p_list, i, 2, 0);
    }

    return SUCCESS;
}

static void create_live_tv_group_list(control_t *p_list_cont)
{
	control_t  *p_list = NULL;
	control_t *p_list_bar = NULL;
	u16 i;  
    
	//list
	p_list = ctrl_create_ctrl(CTRL_LIST, IDC_LIVE_TV_GP_LIST, LIVE_TV_LIST_X,
		LIVE_TV_LIST_Y, LIVE_TV_LIST_W, LIVE_TV_LIST_H, p_list_cont, 0);
	ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	ctrl_set_keymap(p_list, live_tv_group_list_keymap);
	ctrl_set_proc(p_list, live_tv_group_list_proc);

	ctrl_set_mrect(p_list, LIVE_TV_LIST_MIDL, LIVE_TV_LIST_MIDT,
		LIVE_TV_LIST_MIDW+LIVE_TV_LIST_MIDL, LIVE_TV_LIST_MIDH+LIVE_TV_LIST_MIDT);
	list_set_item_interval(p_list,LIVE_TV_LCONT_LIST_VGAP);
	list_set_item_rstyle(p_list, &live_tv_list_item_rstyle);
	list_enable_select_mode(p_list, TRUE);
	list_set_select_mode(p_list, LIST_SINGLE_SELECT);
	list_enable_page_mode(p_list, TRUE);
	g_livetv.p_gp_list = p_list;
	
	list_set_count(p_list, g_livetv.livetv_grp_cnt, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
	list_set_field_count(p_list, LIVE_TV_LIST_FIELD, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
	
	list_set_focus_pos(p_list, 0);
	list_set_update(p_list, live_tv_group_list_update, 0);

	for (i = 0; i < LIVE_TV_LIST_FIELD; i++)
	{
		list_set_field_attr(p_list, (u8)i, (u32)(live_tv_play_list_attr[i].attr), (u16)(live_tv_play_list_attr[i].width),
			(u16)(live_tv_play_list_attr[i].left), (u8)(live_tv_play_list_attr[i].top));
	//	list_set_field_rect_style(p_list, (u8)i, live_tv_play_list_attr[i].rstyle);
		list_set_field_font_style(p_list, (u8)i, live_tv_play_list_attr[i].fstyle);
	}
	//scrollbar
	p_list_bar = ctrl_create_ctrl(CTRL_SBAR, IDC_LIVE_TV_GP_LIST_BAR, LIVE_TV_LIST_BAR_X,
		LIVE_TV_LIST_BAR_Y, LIVE_TV_LIST_BAR_W, LIVE_TV_LIST_BAR_H, p_list_cont, 0);
	ctrl_set_rstyle(p_list_bar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
	sbar_set_autosize_mode(p_list_bar, 1);
	sbar_set_direction(p_list_bar, 0);
	sbar_set_mid_rstyle(p_list_bar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
	list_set_scrollbar(p_list, p_list_bar);

	live_tv_list_update(p_list, list_get_valid_pos(p_list), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);

}

static void create_live_tv_group_list_helpbar(control_t *p_mainwin)
{
	control_t *p_cont = NULL;
	control_t *p_bmp = NULL;
	control_t *p_text = NULL;
      u8 asc_str[32] = {"[OK]:Entry group"};
      
	p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_GP_HELP_BAR,
		LIVE_TV_HELP_FRM_X, LIVE_TV_HELP_FRM_Y,
		LIVE_TV_HELP_FRM_W, LIVE_TV_HELP_FRM_H,
		p_mainwin, 0);

	ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	g_livetv.p_gp_help = p_cont;
      ctrl_set_sts(p_cont, OBJ_STS_HIDE);
	p_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_LIVE_TV_GP_HELP_BMP, 20, 0, 
		38, LIVE_TV_HELP_FRM_H, p_cont, 0);
	ctrl_set_rstyle(p_bmp, RSI_PBACK, RSI_PBACK, RSI_PBACK);
       bmap_set_align_type(p_bmp, STL_CENTER|STL_VCENTER);
	bmap_set_content_by_id(p_bmp, IM_HELP_OK);

	p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_GP_HELP_TEXT,
		56, 0,
		LIVE_TV_HELP_FRM_W-66, LIVE_TV_HELP_FRM_H,
		p_cont, 0);
	ctrl_set_rstyle(p_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_text, STL_CENTER| STL_VCENTER);
	text_set_content_type(p_text, TEXT_STRTYPE_UNICODE);
	text_set_font_style(p_text, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
	text_set_content_by_ascstr(p_text, asc_str);
	
    
}

static void ui_livetv_create_select_group_list(control_t *p_mainwin)
{
    control_t *p_list_cont = NULL;
    
    //create list cont
    p_list_cont = create_live_tv_group_list_cont(p_mainwin);

    //create live_tv group
    create_live_tv_group_list_curr_group_title(p_list_cont);

    //create live_tv list
    create_live_tv_group_list(p_list_cont);

    //create help bar
    create_live_tv_group_list_helpbar(p_mainwin);
}


static void create_num_play(control_t *p_mainwin)
{
	control_t *p_frm = NULL;
	control_t *p_subctrl = NULL;

	// frm
	p_frm = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_NUM_FRM,
		LIVE_TV_NUM_PLAY_FRM_X, LIVE_TV_NUM_PLAY_FRM_Y,
		LIVE_TV_NUM_PLAY_FRM_W, LIVE_TV_NUM_PLAY_FRM_H,
		p_mainwin, 0);
	ctrl_set_rstyle(p_frm, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	ctrl_set_keymap(p_frm, live_tv_num_play_cont_keymap);
	ctrl_set_proc(p_frm, live_tv_num_play_cont_proc);
	ctrl_set_sts(p_frm, OBJ_STS_HIDE);
	g_livetv.p_num_play = p_frm;

	p_subctrl = ctrl_create_ctrl(CTRL_NBOX, IDC_LIVE_TV_NUM_TXT,
		LIVE_TV_NUM_PLAY_TXT_X, LIVE_TV_NUM_PLAY_TXT_Y,
		LIVE_TV_NUM_PLAY_TXT_W, LIVE_TV_NUM_PLAY_TXT_H,
		p_frm, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	nbox_set_align_type(p_subctrl, STL_CENTER | STL_VCENTER);
	nbox_set_font_style(p_subctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
	nbox_set_num_type(p_subctrl, NBOX_NUMTYPE_DEC);
	nbox_set_range(p_subctrl, 0, 9999, LIVE_TV_NUM_PLAY_CNT);

}

static void create_live_tv_epg_bar(control_t *p_mainwin)
{
	control_t *p_frm = NULL;
	control_t *p_subctrl = NULL;

	p_frm = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_EPG_CONT,
		LIVE_TV_EPG_FRM_X, LIVE_TV_EPG_FRM_Y,
		LIVE_TV_EPG_FRM_W, LIVE_TV_EPG_FRM_H,
		p_mainwin, 0);
	ctrl_set_rstyle(p_frm, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	ctrl_set_sts(p_frm, OBJ_STS_HIDE);
	g_livetv.p_epg_frm = p_frm;

	//first line µ±Ç°ÆµµÀ
	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_CURR_P,
		LIVE_TV_EPG_CURR_P_X, LIVE_TV_EPG_CURR_P_Y,
		LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_H,
		p_frm, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
	text_set_font_style(p_subctrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_subctrl, IDS_LIVETV_CURR_PG);


	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_CURR_P_NAME,
		LIVE_TV_EPG_CURR_P_X+LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_Y,
		2*LIVE_TV_EPG_CURR_P_W, LIVE_TV_EPG_CURR_P_H,
		p_frm, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
	text_set_font_style(p_subctrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);

	//time controls
	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_CURR_TIME,
		LIVE_TV_EPG_CURR_P_X+3*LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_Y,
		LIVE_TV_EPG_CURR_P_W+30, LIVE_TV_EPG_CURR_P_H,
		p_frm, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
	text_set_font_style(p_subctrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_subctrl, IDS_LIVETV_CURR_TIME);

	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_CURR_TIME_TEXT,
		LIVE_TV_EPG_CURR_P_X+4*LIVE_TV_EPG_CURR_P_W+30+50, LIVE_TV_EPG_CURR_P_Y,
		2*LIVE_TV_EPG_CURR_P_W, LIVE_TV_EPG_CURR_P_H,
		p_frm, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
	text_set_font_style(p_subctrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);

	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_NET_CONN,
		LIVE_TV_EPG_CURR_P_X+6*LIVE_TV_EPG_CURR_P_W+30+50, LIVE_TV_EPG_CURR_P_Y,
		1*LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_H,
		p_frm, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
	text_set_font_style(p_subctrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
	text_set_content_by_strid(p_subctrl, IDS_CON_STATUS);

	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_NET_CONN_TEXT,
		LIVE_TV_EPG_CURR_P_X+7*LIVE_TV_EPG_CURR_P_W+30+50+50, LIVE_TV_EPG_CURR_P_Y,
		LIVE_TV_EPG_CURR_P_W, LIVE_TV_EPG_CURR_P_H,
		p_frm, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
	text_set_font_style(p_subctrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);

	//second line current program
	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_CURR_PROG,
		LIVE_TV_EPG_CURR_P_X, LIVE_TV_EPG_CURR_P_Y+LIVE_TV_EPG_CURR_P_H,
		LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_H,
		p_frm, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
	text_set_font_style(p_subctrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_subctrl, IDS_LIVETV_CURR_PROG);

	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_CURR_PROG_NAME,
		LIVE_TV_EPG_CURR_P_X+LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_Y+LIVE_TV_EPG_CURR_P_H,
		2*LIVE_TV_EPG_CURR_P_W, LIVE_TV_EPG_CURR_P_H,
		p_frm, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
	text_set_font_style(p_subctrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);

	//change url src info 
	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_URL_SRC,
		4*LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_Y+LIVE_TV_EPG_CURR_P_H,
		2*LIVE_TV_EPG_CURR_P_W + 50, LIVE_TV_EPG_CURR_P_H,
		p_frm, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
	text_set_font_style(p_subctrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_subctrl, IDS_LIVETV_CHANGE_SRC);

	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_URL_SRC_NUM,
		6*LIVE_TV_EPG_CURR_P_W + 50+50, LIVE_TV_EPG_CURR_P_Y+LIVE_TV_EPG_CURR_P_H,
		2 *LIVE_TV_EPG_CURR_P_W, LIVE_TV_EPG_CURR_P_H,
		p_frm, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
	text_set_font_style(p_subctrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);

	//third line next pg
	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_NEXT_PROG,
		LIVE_TV_EPG_CURR_P_X, LIVE_TV_EPG_CURR_P_Y+2*LIVE_TV_EPG_CURR_P_H,
		LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_H,
		p_frm, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
	text_set_font_style(p_subctrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_subctrl, IDS_LIVETV_NEXT_PROG);

	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_NEXT_PROG_NAME,
		LIVE_TV_EPG_CURR_P_X+LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_Y+2*LIVE_TV_EPG_CURR_P_H,
		2*LIVE_TV_EPG_CURR_P_W, LIVE_TV_EPG_CURR_P_H,
		p_frm, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
	text_set_font_style(p_subctrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);

	p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_EPG_CHANGE_PROG,
		4*LIVE_TV_EPG_CURR_P_W+50, LIVE_TV_EPG_CURR_P_Y+2*LIVE_TV_EPG_CURR_P_H,
		3*LIVE_TV_EPG_CURR_P_W, LIVE_TV_EPG_CURR_P_H,
		p_frm, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_subctrl, STL_LEFT| STL_VCENTER);
	text_set_font_style(p_subctrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_subctrl, IDS_LIVETV_CHANGE_PG);
	
}

static void create_live_tv_exit_info(control_t *p_mainwin)
{
	control_t *p_cont = NULL;
	control_t *p_txt = NULL;

	p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_EXIT_CONT,
		COMM_DLG_X, COMM_DLG_Y,COMM_DLG_W, COMM_DLG_H,
		p_mainwin, 0);
	ctrl_set_rstyle(p_cont,RSI_POPUP_BG,RSI_POPUP_BG,RSI_POPUP_BG);
	ctrl_set_sts(p_cont, OBJ_STS_HIDE);
	g_livetv.p_exit_info_cont = p_cont;
	p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_EXIT_TEXT,
		DLG_CONTENT_GAP, DLG_CONTENT_GAP,
		(u16)(COMM_DLG_X - 2 * DLG_CONTENT_GAP),
		(u16)(COMM_DLG_H - 2 * DLG_CONTENT_GAP),
		p_cont, 0);

	text_set_font_style(p_txt, FSI_DLG_CONTENT, FSI_DLG_CONTENT, FSI_DLG_CONTENT);
	text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_txt,IDS_EXIT_PLAY_AFTER_EXIT );
}

static void create_live_tv_today_epg_list(control_t *p_mainwin)
{
	control_t *p_frm = NULL;
	control_t  *p_list = NULL;
	control_t  *p_list_bar = NULL;
	u16 i;

	//cont
	p_frm = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_TODAY_EPG_CONT,
		LIVE_TV_TODAY_EPG_FRM_X, LIVE_TV_TODAY_EPG_FRM_Y,
		LIVE_TV_TODAY_EPG_FRM_W, LIVE_TV_TODAY_EPG_FRM_H,
		p_mainwin, 0);
	ctrl_set_rstyle(p_frm, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	ctrl_set_sts(p_frm, OBJ_STS_HIDE);
	g_livetv.p_today_epg_cont = p_frm;

	//today evt list
	p_list = ctrl_create_ctrl(CTRL_LIST, IDC_TODAY_EPG_EVT_LIST,
		LIVE_TV_TODAY_EPG_LIST_X, LIVE_TV_TODAY_EPG_LIST_Y,
		LIVE_TV_TODAY_EPG_LIST_W, LIVE_TV_TODAY_EPG_LIST_H, 
		p_frm, 0);
	ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	ctrl_set_keymap(p_list, live_tv_epg_list_keymap);
	ctrl_set_proc(p_list, live_tv_epg_list_proc);
	g_livetv.p_today_epg = p_list;
	ctrl_set_mrect(p_list, LIVE_TV_EPG_LIST_MIDL, LIVE_TV_EPG_LIST_MIDT,
		LIVE_TV_EPG_LIST_MIDL + LIVE_TV_EPG_LIST_MIDW, 
		LIVE_TV_EPG_LIST_MIDT + LIVE_TV_EPG_LIST_MIDH);

	list_set_item_interval(p_list, LIVE_TV_TODAY_EPG_EVT_LIST_VGAP);
	list_set_item_rstyle(p_list, &live_tv_epg_list_item_rstyle);
	list_enable_select_mode(p_list, TRUE);
	list_set_select_mode(p_list, LIST_SINGLE_SELECT);

	list_set_count(p_list, 0, LIVE_TV_TODAY_EPG_EVT_LIST_ONE_PAGE);

	list_set_field_count(p_list, LIVE_TV_EPG_LIST_FIELD, LIVE_TV_TODAY_EPG_EVT_LIST_ONE_PAGE);
	list_set_update(p_list, live_tv_today_epg_list_update, 0);

	for (i = 0; i < LIVE_TV_EPG_LIST_FIELD; i++)
	{
		list_set_field_attr(p_list, (u8)i, (u32)(live_tv_epg_list_attr[i].attr), (u16)(live_tv_epg_list_attr[i].width),
			(u16)(live_tv_epg_list_attr[i].left), (u8)(live_tv_epg_list_attr[i].top));
	//	list_set_field_rect_style(p_list, (u8)i, live_tv_epg_list_attr[i].rstyle);
		list_set_field_font_style(p_list, (u8)i, live_tv_epg_list_attr[i].fstyle);
	}

	//scrollbar
	p_list_bar = ctrl_create_ctrl(CTRL_SBAR, IDC_TODAY_EPG_EVT_BAR,
		LIVE_TV_EPG_LIST_BAR_X, LIVE_TV_EPG_LIST_BAR_Y,
		LIVE_TV_EPG_LIST_BAR_W, LIVE_TV_EPG_LIST_BAR_H,
		p_frm, 0);
	ctrl_set_rstyle(p_list_bar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
	sbar_set_autosize_mode(p_list_bar, 1);
	sbar_set_direction(p_list_bar, 0);
	sbar_set_mid_rstyle(p_list_bar, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID, RSI_SCROLL_BAR_MID);
	list_set_scrollbar(p_list, p_list_bar);

}


static void create_live_tv_show_bps(control_t *p_mainwin)
{
	control_t *p_cont_cont = NULL;
	control_t *p_cont = NULL;
	control_t *p_bps = NULL;
	control_t *p_process = NULL;

	p_cont_cont = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_BPS_CONT_CONT,
		LIVE_TV_BPS_X, LIVE_TV_BPS_Y, 
		LIVE_TV_BPS_W, LIVE_TV_BPS_H, 
		p_mainwin, 0);
	ctrl_set_rstyle(p_cont_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	g_livetv.p_bps_cont_cont = p_cont_cont;
	p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_LIVE_TV_BPS_CONT,
		0, 0, 
		LIVE_TV_BPS_W, LIVE_TV_BPS_H, 
		p_cont_cont, 0);
	ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	ctrl_set_sts(p_cont, OBJ_STS_HIDE);
	g_livetv.p_bps_cont = p_cont;
	p_bps = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_BPS,
		0, 0, LIVE_TV_BPS_W/2+30, LIVE_TV_BPS_H, 
		p_cont, 0);
	ctrl_set_rstyle(p_bps, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_bps, STL_CENTER | STL_VCENTER);
	text_set_font_style(p_bps, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_content_type(p_bps, TEXT_STRTYPE_UNICODE);
	g_livetv.p_bps = p_bps;
	p_process = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_BUF_PERCENT,
		LIVE_TV_BPS_W/2, 0, LIVE_TV_BPS_W/2-30, LIVE_TV_BPS_H, 
		p_cont, 0);
	ctrl_set_rstyle(p_process, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	text_set_align_type(p_process, STL_CENTER | STL_VCENTER);
	text_set_font_style(p_process, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_content_type(p_process, TEXT_STRTYPE_UNICODE);
	g_livetv.p_percent = p_process;
	//live_tv_update_bps(p_bps,p_process, 123, 35);
}

static void create_live_tv_show_loading_media_time(control_t *p_mainwin)
{
	control_t *p_cont = NULL;
	control_t *p_ctrl = NULL;

	p_cont = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_LOADING_MEDIA_TIME_CONT,
		LIVE_TV_BPS_X, LIVE_TV_BPS_Y, 
		LIVE_TV_BPS_W, LIVE_TV_BPS_H, 
		p_mainwin, 0);
	ctrl_set_rstyle(p_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	g_livetv.p_load_time_cont = p_cont;
	p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_LIVE_TV_LOADING_MEDIA_TIME,
		0, 0, 
		LIVE_TV_BPS_W, LIVE_TV_BPS_H, 
		p_cont, 0);
	ctrl_set_rstyle(p_ctrl, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
	ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
	text_set_align_type(p_ctrl, STL_CENTER | STL_VCENTER);
	text_set_font_style(p_ctrl, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_content_type(p_ctrl, TEXT_STRTYPE_UNICODE);
	g_livetv.p_load_time = p_ctrl;

	// live_tv_update_loading_media_time(p_ctrl, 8);
}

static void ui_livetv_play_first_prog()
{
	live_db_prg_t *curn_pg = NULL;
  	live_db_prg_t p_pg = {0};

	curn_pg = get_livetv_curr_pg();

	if(strlen(curn_pg->name) == 0)
	{
		livetv_get_pg_by_group_pos(&p_pg, 0, INVALID_GRP);
		set_livetv_curr_pg(&p_pg);
		set_cur_group_index(0);//play all grp
		ui_livetv_play_net_prog(p_pg.url[p_pg.src_idx]);
		OS_PRINTF("##%s,line:%d url_name = %s##\n", __FUNCTION__, __LINE__, p_pg.url[p_pg.src_idx]);
		g_livetv.g_circle_index = g_livetv.g_curr_pg.src_idx;
		g_livetv.g_playing_focus = 0;
		OS_PRINTF("##%s, line[%d], url_circle_index = %d##\n", __FUNCTION__, __LINE__, g_livetv.g_circle_index);
	}
	else
	{
		ui_livetv_play_net_prog(curn_pg->url[curn_pg->src_idx]);
		OS_PRINTF("##%s, line[%d], url_circle_index = %d##\n", __FUNCTION__, __LINE__, g_livetv.g_circle_index);
	}
	
}

static void init_livetv_net_grp()
{
	live_db_prg_t temp_pg = {0};
	u8 group_cnt;
	u16 all_pg_cnt;

	if(ui_get_livetv_db_flag())
	{
		g_livetv.is_db_done = TRUE;
		livetv_get_group_cnt(&group_cnt);
		all_pg_cnt = livetv_get_cnt_by_group(INVALID_GRP);
		livetv_get_pg_by_group_pos(&temp_pg, all_pg_cnt -1, INVALID_GRP);
		g_livetv.max_net_pg_id = temp_pg.id;
	}
	else
	{
		all_pg_cnt = 0;
		group_cnt = 0;
		g_livetv.is_db_done = FALSE;
	} 
	OS_PRINTF("##%s, all pg cnt == %d, group_cnt = %d##\n", __FUNCTION__, all_pg_cnt, group_cnt);
	//1.all grp  grp 2.fav grp 3. customer grp
	g_livetv.g_livetv_all_pg_cnt = all_pg_cnt;
	g_livetv.grp_index.all_grp = 0;
	g_livetv.grp_index.fav_grp = group_cnt + 1;
	g_livetv.grp_index.cus_grp = group_cnt + 2;
    g_livetv.livetv_grp_cnt = group_cnt + 3;
	g_livetv.g_vcircle_index = INVALIDID;
}


static void init_livetv_customer_url()
{
	g_livetv.p_url_buf = (u8*)mtos_malloc(CUSTOM_URL_BLOCK_SIZE);
	MT_ASSERT(g_livetv.p_url_buf != NULL);
	memset(g_livetv.p_url_buf, 0, CUSTOM_URL_BLOCK_SIZE);
	g_livetv.cus_url_tbl  = (custom_url_t*) mtos_malloc(sizeof(custom_url_t)*MAX_CUSTOM_URL);
	MT_ASSERT(g_livetv.cus_url_tbl != NULL);
	memset(g_livetv.cus_url_tbl, 0, sizeof(custom_url_t)*MAX_CUSTOM_URL);
#if ENABLE_CUSTOM_URL
	g_livetv.cus_url_total = load_custom_url(&g_livetv.p_url_array, g_livetv.p_url_buf,g_livetv.cus_url_tbl);
#endif
}

static void ui_livetv_get_utc_time()
{
	get_several_days_time(g_livetv.days, MAX_LOOK_BACK_INFO_DAY_NUM);
}


RET_CODE open_live_tv(u32 para1, u32 para2)
{
    control_t *p_mainwin = NULL;

	OS_PRINTF("##%s, para1 = %d##\n", __FUNCTION__, para1);

    ui_video_c_create(VIDEO_PLAY_FUNC_ONLINE);
	
	ui_livetv_get_utc_time();

	ui_live_tv_set_exit_state(FALSE);

	//init livetv net prog group info
	init_livetv_net_grp();

	//init customer url
	init_livetv_customer_url();

	//create mainwindow
	p_mainwin = create_mainwindow();

    //ctrl_paint_ctrl(ctrl_get_root(p_mainwin), FALSE);//first paint, for livetv can't show quickly

    //create livetv prog list
    ui_livetv_create_prog_list(p_mainwin);

     //create livetv group list
    ui_livetv_create_select_group_list(p_mainwin);

    //create number play
    create_num_play(p_mainwin);

	//create epg bar
	create_live_tv_epg_bar(p_mainwin);

	//creat epg cont
	create_live_tv_today_epg_list(p_mainwin);

	//create livetv bps
	create_live_tv_show_bps(p_mainwin);

	//create livetv show loading media time
	create_live_tv_show_loading_media_time(p_mainwin);

	//create exit text
	create_live_tv_exit_info(p_mainwin);

	ctrl_paint_ctrl(ctrl_get_root(p_mainwin), FALSE);

	if(ui_is_mute())
	{
		open_mute(0,0);
	}

       //play the last prog once entry livetv
	ui_livetv_play_first_prog();

#if ENABLE_IPTV_EPG
	//load epg info
    init_livetv_one_pg_one_day_epg_param();
#endif

	return SUCCESS;
}

static void close_epg_bar_while_show_pg_list()
{
	if(ctrl_get_sts(g_livetv.p_epg_frm) == OBJ_STS_SHOW)
	{
		ui_livetv_stop_timer(MSG_HIDE_EPG);
		ctrl_set_sts(g_livetv.p_epg_frm, OBJ_STS_HIDE);
		ctrl_paint_ctrl(g_livetv.p_epg_frm, TRUE);
	}
}

static void close_today_epg_list_while_change_prog()
{
       control_t *p_active = NULL;
       
       p_active = ctrl_get_active_ctrl(g_livetv.p_menu);
	if(ctrl_get_sts(g_livetv.p_today_epg_cont) == OBJ_STS_SHOW)
	{
		ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
            if(ctrl_get_sts(g_livetv.p_gp_list_cont) == OBJ_STS_HIDE)
            {
                ctrl_process_msg(g_livetv.p_list, MSG_GETFOCUS, 0, 0);
            }
            else
            {
                ctrl_process_msg(g_livetv.p_gp_list, MSG_GETFOCUS, 0, 0);
            }
		ctrl_set_sts(g_livetv.p_today_epg_cont, OBJ_STS_HIDE);
		ui_livetv_start_timer(MSG_HIDE_EPG, 2000);
		ui_livetv_stop_timer(MSG_HIDE_TODAY_EPG);
		ctrl_paint_ctrl(g_livetv.p_menu, TRUE);
		if(ui_is_mute())
		{
			open_mute(0, 0);
		}
	}
}

static RET_CODE on_live_tv_list_select_to_show_list(BOOL is_jump)
{
	control_t *p_list = NULL;
	u8 group_index;
	u32 count;

	p_list = g_livetv.p_list;
	close_epg_bar_while_show_pg_list();
	ui_livetv_reset_hide_all_list();

	ctrl_set_sts(g_livetv.p_help, OBJ_STS_SHOW);
	ctrl_set_sts(g_livetv.p_list_cont, OBJ_STS_SHOW);
	OS_PRINTF("###show list prog id == %d##\n", g_livetv.g_curr_pg.id);
	OS_PRINTF("##%s, line[%d]##\n", __FUNCTION__, __LINE__);
       if(!is_jump)
       {
	    group_index = get_cur_group_index();
       }
       else
       {
            group_index = cbox_dync_get_focus(g_livetv.p_grp);
       }

	if(group_index < g_livetv.grp_index.fav_grp) //comm network prog
	{
		if(group_index == 0)
		{
			count = g_livetv.g_livetv_all_pg_cnt;
		}
		else
		{
			count = livetv_get_cnt_by_group(group_index - 1);
		}
		list_set_count(p_list, count, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
	}
	else if(group_index == g_livetv.grp_index.cus_grp)
	{
		count = g_livetv.cus_url_total;
             ctrl_set_sts(g_livetv.p_help, OBJ_STS_HIDE);
	}
	
	else if(group_index == g_livetv.grp_index.fav_grp)//fav
	{
		sys_status_get_fav_livetv_total_num(&count);
             if(!is_jump)
            {
        		if(count == 0)
        		{
        			count = g_livetv.g_livetv_all_pg_cnt;
        			group_index = g_livetv.grp_index.all_grp;
        			set_cur_group_index(group_index); 
        			livetv_db_get_pos_by_id(&g_livetv.g_playing_focus, g_livetv.g_curr_pg.id);
        			OS_PRINTF("##%s, line = [%d], playing fav id ==[%d], \
        					  playing focus = [%d]##\n", __FUNCTION__, \
        					  __LINE__,g_livetv.g_curr_pg.id, g_livetv.g_playing_focus);
        		}
            }
	}
	OS_PRINTF("##%s, this group count == %d##\n", __FUNCTION__, count);
	list_set_count(p_list, count, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
        if(!is_jump)
        {
            list_set_focus_pos(p_list, g_livetv.g_playing_focus);
            list_select_item(p_list, g_livetv.g_playing_focus);
        }
        else
        {
            list_set_focus_pos(p_list, 0);
        }
	cbox_dync_set_focus(g_livetv.p_grp, group_index);
	cbox_dync_set_update(g_livetv.p_grp, live_tv_group_update);
	live_tv_list_update(p_list, list_get_valid_pos(p_list), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);
	ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(g_livetv.p_menu, TRUE);
	OS_PRINTF("##live tv list hide, should show list##\n");
	return SUCCESS;
}

static RET_CODE on_live_tv_list_select_to_play()
{
	livetv_url channel_url = {0};
	control_t *p_list = NULL;
	live_tv_fav_info fav_livetv_info = {0};
	live_db_prg_t p_pg = {0};
	u8 group_focus;
	u16 list_focus;
	u16 name_len = 0;
	u16 list_cnt;
	mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;

	p_list = g_livetv.p_list;
	list_cnt = list_get_count(p_list);
	g_livetv.g_vcircle_index = INVALIDID;
	g_livetv.is_auto_change = FALSE;
	if(list_cnt == 0)
	{
		OS_PRINTF("##%s, list count == 0, will return##\n", __FUNCTION__);
		return ERR_FAILURE;
	}

	//fix epg list can't auto close
	//close_today_epg_list_while_change_prog();


	list_focus = list_get_focus_pos(p_list);
	group_focus = cbox_dync_get_focus(g_livetv.p_grp);
	
	g_livetv.g_playing_focus = list_focus;
	
	OS_PRINTF("###%s,%d,  change group focus == %d##\n",__FUNCTION__,__LINE__,group_focus);

	if(get_cur_group_index() != group_focus)
	{
		g_livetv.is_live_exit_state = 0;
	}
	state = ui_video_c_get_play_state();

	if(group_focus < g_livetv.grp_index.fav_grp) //comm network prog
	{
		if(group_focus == 0)
		{
			livetv_get_pg_by_group_pos(&p_pg, list_focus, INVALID_GRP);
		}
		else
		{
			livetv_get_pg_by_group_pos(&p_pg, list_focus, group_focus - 1);
		}

		//check curr pg is playing, if is playing, will return
		OS_PRINTF("##%s, play status == %d##\n", __FUNCTION__, state);
		if(MUL_PLAY_STATE_PLAY ==  state && (g_livetv.g_curr_pg.id == p_pg.id))
		{
			OS_PRINTF("##play the same prog, will return##\n");
			list_select_item(p_list, list_focus);
			ctrl_paint_ctrl(g_livetv.p_list_cont, TRUE);
			set_cur_group_index(group_focus);
			return ERR_FAILURE;
		}

		channel_url.url = p_pg.url[p_pg.src_idx];
		// OS_PRINTF("##play prog name == %s, url ==  %s##\n",p_pg.name, p_pg.url[p_pg.src_idx]);
		memcpy(&g_livetv.g_prev_pg, &g_livetv.g_curr_pg, sizeof(live_db_prg_t));
		memcpy(&g_livetv.g_curr_pg, &p_pg, sizeof(live_db_prg_t));
		g_livetv.g_circle_index = g_livetv.g_curr_pg.src_idx;
		OS_PRINTF("###%d, prev_pg name == %s##\n", __LINE__, g_livetv.g_prev_pg.name);
		OS_PRINTF("###%d, curr_pg name == %s##\n", __LINE__, g_livetv.g_curr_pg.name);

	}
	else if(group_focus == g_livetv.grp_index.fav_grp)//fav group
	{
		sys_status_get_fav_livetv_info(list_focus, &fav_livetv_info);
		OS_PRINTF("##%s, fav id == %d##\n", __FUNCTION__,  fav_livetv_info.live_fav_id);
		livetv_get_pg_by_id(&p_pg, fav_livetv_info.live_fav_id);
		//check curr pg is playing, if is playing, will return
		OS_PRINTF("##%s, play status == %d##\n", __FUNCTION__, state);
		if(MUL_PLAY_STATE_PLAY == state && (g_livetv.g_curr_pg.id == p_pg.id))
		{
			OS_PRINTF("##play the same fav prog, will return##\n");
			list_select_item(p_list, list_focus);
			ctrl_paint_ctrl(g_livetv.p_list_cont, TRUE);
			set_cur_group_index(group_focus);
			return ERR_FAILURE;
		}

		channel_url.url = p_pg.url[p_pg.src_idx];
		//OS_PRINTF("##fav play prog name == %s, url ==  %s##\n",p_pg.name,p_pg.url[p_pg.src_idx]);
		memcpy(&g_livetv.g_prev_pg, &g_livetv.g_curr_pg, sizeof(live_db_prg_t));
		memcpy(&g_livetv.g_curr_pg, &p_pg, sizeof(live_db_prg_t));
		g_livetv.g_circle_index = g_livetv.g_curr_pg.src_idx;

	}
	else if(group_focus == g_livetv.grp_index.cus_grp)//customer group
	{
		list_focus = list_get_focus_pos(p_list);
		channel_url.url = g_livetv.p_url_array[list_focus].url;

		OS_PRINTF("##customer list play prog name == %s, url ==  \
				  %s##\n", g_livetv.p_url_array[list_focus].name, g_livetv.p_url_array[list_focus].url);
		//check curr pg is playing, if is playing, will return
		{
			OS_PRINTF("##%d, play status == %d##\n", __LINE__, state);
			if(MUL_PLAY_STATE_PLAY == state && g_livetv.g_curr_pg.id == (list_focus + g_livetv.max_net_pg_id + 1))
			{
				OS_PRINTF("##play the customer prog, will return##\n");
				list_select_item(p_list, list_focus);
				ctrl_paint_ctrl(g_livetv.p_list_cont, TRUE);
				set_cur_group_index(group_focus);
				return ERR_FAILURE;
			}
		}
		memcpy(&g_livetv.g_prev_pg, &g_livetv.g_curr_pg, sizeof(live_db_prg_t));
		g_livetv.g_curr_pg.url[0] = g_livetv.p_url_array[list_focus].url;
		name_len = strlen(g_livetv.p_url_array[list_focus].name);
		name_len = (name_len >= MAX_LIVE_NAME_LEN) ? (MAX_LIVE_NAME_LEN - 1) : name_len;
		strncpy(g_livetv.g_curr_pg.name, g_livetv.p_url_array[list_focus].name, name_len);
		g_livetv.g_curr_pg.name[name_len] = '\0';
		g_livetv.g_curr_pg.id = list_focus + g_livetv.max_net_pg_id + 1;
		g_livetv.g_curr_pg.sate_id = 0;  
		g_livetv.g_curr_pg.url_cnt = 1;
	}

	set_cur_group_index(group_focus);//select to play, set current playing grp index
	list_select_item(p_list, list_focus);
	ctrl_paint_ctrl(g_livetv.p_list_cont, TRUE);
	ui_livetv_play_net_prog(channel_url.url);
	
	return SUCCESS;

}

static RET_CODE on_live_tv_list_select_to_play_or_show_list(control_t *p_list, u16 msg,
															u32 para1, u32 para2)
{
	if(OBJ_STS_HIDE == ctrl_get_sts(g_livetv.p_list_cont))
	{
		on_live_tv_list_select_to_show_list(FALSE);
	}
	else
	{
		on_live_tv_list_select_to_play();
	}

	return SUCCESS;
}

/************************************************
change url src functions

************************************************/
static RET_CODE auto_change_url_src()
{
	livetv_url channel_url = {0};

	OS_PRINTF("###%s, g_livetv.is_left == %d#\n", __FUNCTION__, g_livetv.is_left);


	if(!ui_livetv_check_net())
	{
		//jump_to_sat_group_first_prog();    
		g_livetv.is_auto_change = TRUE;
		return SUCCESS;
	}

	if(g_livetv.is_left == FALSE)//MSG_INCREASE , increase url index
	{
		if(g_livetv.g_curr_pg.src_idx + 1 < g_livetv.g_curr_pg.url_cnt)
		{
			channel_url.url = g_livetv.g_curr_pg.url[g_livetv.g_curr_pg.src_idx+1];
			g_livetv.g_curr_pg.src_idx++;
		}
		else
		{
			g_livetv.g_curr_pg.src_idx = 0;
			channel_url.url = g_livetv.g_curr_pg.url[g_livetv.g_curr_pg.src_idx];
		}
	}
	else //msg == MSG_DECREASE
	{
		if(g_livetv.g_curr_pg.src_idx == 0)
		{
			g_livetv.g_curr_pg.src_idx = g_livetv.g_curr_pg.url_cnt - 1;
			channel_url.url = g_livetv.g_curr_pg.url[g_livetv.g_curr_pg.src_idx];
		}
		else
		{
			channel_url.url = g_livetv.g_curr_pg.url[g_livetv.g_curr_pg.src_idx - 1];
			g_livetv.g_curr_pg.src_idx --;
		}
	}

	if(g_livetv.g_curr_pg.src_idx == g_livetv.g_circle_index || g_livetv.g_curr_pg.url_cnt == 1)
	{
		live_tv_open_dlg(IDS_PLAY_ERROR, 2000);
		livetv_set_pg_cur_source(g_livetv.g_curr_pg.id, g_livetv.g_curr_pg.src_idx);
		on_live_tv_list_change_focus_or_program(NULL, MSG_FOCUS_UP, 0, 1);
		g_livetv.is_auto_change = TRUE;
		return SUCCESS;
	}

	livetv_set_pg_cur_source(g_livetv.g_curr_pg.id, g_livetv.g_curr_pg.src_idx);
	// OS_PRINTF("##change src play prog name == %s, url == %s, src index == %d##\n",g_livetv.g_curr_pg.name, 
	// g_livetv.g_curr_pg.url[g_livetv.g_curr_pg.src_idx], g_livetv.g_curr_pg.src_idx);
	ui_livetv_play_net_prog(channel_url.url);
	live_tv_open_dlg(IDS_STARTING_CHANGE_URL, 0);
	if(ctrl_get_sts(g_livetv.p_list_cont) == OBJ_STS_HIDE && ctrl_get_sts(g_livetv.p_gp_list_cont) == OBJ_STS_HIDE)
	{
		show_epg_bar_while_change_prog();
	}
	g_livetv.is_auto_change = TRUE;

	return SUCCESS;

}

static RET_CODE on_live_tv_change_url_src(control_t *p_ctrl, u16 msg,
										  u32 para1, u32 para2)
{
	livetv_url channel_url = {0};
	u8 group_index;

	//fix epg list can't auto close
	//close_today_epg_list_while_change_prog();

	group_index = get_cur_group_index();
	if(group_index == g_livetv.grp_index.cus_grp)//except lookback and customer group
	{
		OS_PRINTF("########look back list or customer group, should not change url src###\n");
		return ERR_FAILURE;
	}

	OS_PRINTF("####%s, para1 == %d##\n", __FUNCTION__, para1);

	if(!ui_livetv_check_net())
	{
		g_livetv.is_auto_change = FALSE;
		return SUCCESS;
	}

	/*********don't change url src index if url cnt == 1********/
	if(g_livetv.g_curr_pg.url_cnt == 1)
	{
		OS_PRINTF("##%s, only has one url, will not change url src##\n", __FUNCTION__);
             if(ctrl_get_sts(g_livetv.p_epg_frm) == OBJ_STS_HIDE)
		{
			show_epg_bar_while_change_prog();
		}
		g_livetv.is_auto_change = FALSE;
		return SUCCESS;
	}

	if(MSG_ADD_URL_SRC == msg)
	{
		g_livetv.is_left = FALSE;
		if(g_livetv.g_curr_pg.src_idx + 1 < g_livetv.g_curr_pg.url_cnt)
		{
			channel_url.url = g_livetv.g_curr_pg.url[g_livetv.g_curr_pg.src_idx+1];
			g_livetv.g_curr_pg.src_idx++;
		}
		else
		{
			g_livetv.g_curr_pg.src_idx = 0;
			channel_url.url = g_livetv.g_curr_pg.url[g_livetv.g_curr_pg.src_idx];
		}
		OS_PRINTF("g_livetv.g_circle_index == %d, src-index = %d##\n", g_livetv.g_circle_index,g_livetv.g_curr_pg.src_idx);
	}
	else //msg == MSG_REDUCE_URL_SRC
	{
		g_livetv.is_left = TRUE;
		if(g_livetv.g_curr_pg.src_idx == 0)
		{
			g_livetv.g_curr_pg.src_idx = g_livetv.g_curr_pg.url_cnt - 1;
			channel_url.url = g_livetv.g_curr_pg.url[g_livetv.g_curr_pg.src_idx];
		}
		else
		{
			channel_url.url = g_livetv.g_curr_pg.url[g_livetv.g_curr_pg.src_idx - 1];
			g_livetv.g_curr_pg.src_idx --;
		}
	}
	g_livetv.g_circle_index = g_livetv.g_curr_pg.src_idx;
	livetv_set_pg_cur_source(g_livetv.g_curr_pg.id, g_livetv.g_curr_pg.src_idx);
	// OS_PRINTF("##change src play prog name == %s, url == %s, src index == %d##\n",g_livetv.g_curr_pg.name, 
	// g_livetv.g_curr_pg.url[g_livetv.g_curr_pg.src_idx], g_livetv.g_curr_pg.src_idx);
	g_livetv.is_auto_change = FALSE;
	ui_livetv_play_net_prog(channel_url.url);
	show_epg_bar_while_change_prog();

	return SUCCESS;
}

/************************************************
liveTV after callback functions

************************************************/

static RET_CODE on_live_tv_get_epg_lookback_params(control_t *p_cont, u16 msg,
												   u32 para1, u32 para2)
{
	OS_PRINTF("##%s##\n", __FUNCTION__);
	if(!ui_livetv_check_net())
	{
		return ERR_FAILURE;
	}

	switch (msg)
	{
	case MSG_GET_LIVE_TV_ONE_PG_ONE_DAY_LOOKBACK:
		OS_PRINTF("##%s, %d##\n", __FUNCTION__, __LINE__);
		break;  
	case MSG_GET_LIVE_TV_LOOKBACK_NAMES:
		OS_PRINTF("##%s, %d##\n", __FUNCTION__, __LINE__);
		
		break;
	case MSG_GET_LIVE_TV_ONE_PG_ONE_DAY_EPG:
		OS_PRINTF("##%s, %d##\n", __FUNCTION__, __LINE__);
		ui_live_tv_api_one_pg_one_day_epg_arrived(para1);
		g_livetv.is_loading_epg = FALSE;
		if((LIVETV_EPG_INFO_T *)para1 == NULL)
		{
			OS_PRINTF("##not match epg info1111##\n");
			g_livetv.p_epg = NULL;
		}
		else
		{
			g_livetv.p_epg = livetv_api_get_one_pg_one_day_epg();
		}
		if(g_livetv.p_epg != NULL)
		{
			OS_PRINTF("####epg name = [%s], curr pg name = [%s]##\n", g_livetv.p_epg->title,g_livetv.g_curr_pg.name);
			if(strcmp(g_livetv.g_curr_pg.name, g_livetv.p_epg->title) == 0)
			{
				on_live_tv_show_epg_info_bar(NULL, 0, 0, 0);
			}
			else
			{
				show_epg_bar_while_change_prog();
			}
		}
		else
		{
			on_live_tv_show_epg_info_bar(NULL, 0, 0, 0);
		}

		break;
	}

	return SUCCESS;
}

static void live_tv_all_list_hide()
{
    control_t *p_active = NULL;

    ctrl_set_sts(g_livetv.p_help, OBJ_STS_HIDE);
    ctrl_set_sts(g_livetv.p_list_cont, OBJ_STS_HIDE);
    ctrl_set_sts(g_livetv.p_gp_help, OBJ_STS_HIDE);
    ctrl_set_sts(g_livetv.p_gp_list_cont, OBJ_STS_HIDE);
    p_active = ctrl_get_active_ctrl(g_livetv.p_menu);
    OS_PRINTF("##%s, active id:%d##\n", __FUNCTION__, p_active->id);
    if(p_active->id != IDC_LIVE_TV_NUM_FRM && p_active->id != IDC_LIVE_TV_TODAY_EPG_CONT \
    && p_active->id != IDC_TODAY_EPG_EVT_LIST)
    {
        ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
        ctrl_process_msg(g_livetv.p_list, MSG_GETFOCUS, 0, 0);
    }

    ctrl_paint_ctrl(g_livetv.p_menu, TRUE);
}

static RET_CODE on_live_tv_hide_all_list(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	live_tv_all_list_hide();               
	ui_livetv_stop_timer(MSG_HIDE_ALL_LIST);

	return SUCCESS;
}

static void live_tv_exit_info_show()
{
	if(ctrl_get_sts(g_livetv.p_exit_info_cont) == OBJ_STS_HIDE)
	{
		ctrl_set_sts(g_livetv.p_exit_info_cont, OBJ_STS_SHOW);
	}

	ctrl_paint_ctrl(g_livetv.p_exit_info_cont, TRUE);
}

static void live_tv_exit_info_hide()
{
	if(ctrl_get_sts(g_livetv.p_exit_info_cont) == OBJ_STS_SHOW)
	{
		ctrl_set_sts(g_livetv.p_exit_info_cont, OBJ_STS_HIDE);
	}

	ctrl_paint_ctrl(g_livetv.p_menu, TRUE);
}

static void stop_all_timer_before_exit_livetv()
{
	ui_livetv_stop_timer(MSG_CACEL_EXIT_WINDOW);
	ui_livetv_stop_timer(MSG_HIDE_EPG);
	ui_livetv_stop_timer(MSG_HIDE_TODAY_EPG);
	ui_livetv_stop_timer(MSG_HIDE_ALL_LIST);
	ui_livetv_stop_timer(MSG_SELECT);
}

static RET_CODE on_live_tv_list_exit(control_t *p_list, u16 msg,
									 u32 para1, u32 para2)
{
	BOOL is_exit = FALSE;
      mul_fp_play_state_t _state;

      _state = ui_video_c_get_play_state();
	if(OBJ_STS_HIDE == ctrl_get_sts(g_livetv.p_list_cont))
	{
		ui_live_tv_get_exit_state(&is_exit);
		if(!is_exit)
		{
			ui_comm_dlg_close();
			live_tv_exit_info_show();
			ui_livetv_start_timer(MSG_CACEL_EXIT_WINDOW, 2000);
			ui_live_tv_set_exit_state(TRUE);
		}
		else
		{
		      g_livetv.is_live_exit_state = 1;
                   ui_comm_dlg_close();
                   ui_live_tv_set_exit_state(FALSE);
		      if(_state == MUL_PLAY_STATE_NONE || _state == MUL_PLAY_STATE_STOP)
                  {
                        stop_all_timer_before_exit_livetv();
                        ui_comm_dlg_close();
                        manage_close_menu(ROOT_ID_LIVE_TV, 0, 0);
                  }
                   else
                  {
                        ui_video_c_stop();
                  }
		}
	}
	else
	{
		live_tv_all_list_hide();
	}
	return SUCCESS;
}

 static RET_CODE on_live_tv_list_driect_exit(control_t *p_list, u16 msg,
									 u32 para1, u32 para2)
{
      mul_fp_play_state_t _state;
       _state = ui_video_c_get_play_state();
       g_livetv.is_live_exit_state = 1;
       ui_comm_dlg_close();
       ui_live_tv_set_exit_state(FALSE);
       if(_state == MUL_PLAY_STATE_NONE || _state == MUL_PLAY_STATE_STOP)
      {
            stop_all_timer_before_exit_livetv();
            ui_comm_dlg_close();
            manage_close_menu(ROOT_ID_LIVE_TV, 0, 0);
      }
       else
      {
            ui_video_c_stop();
      }

   	return SUCCESS;
}

static RET_CODE on_live_tv_exit_window(control_t *p_list, u16 msg,
									   u32 para1, u32 para2)
{
	ui_livetv_stop_timer(MSG_CACEL_EXIT_WINDOW);
	ui_live_tv_set_exit_state(FALSE);
	live_tv_exit_info_hide();
	
	return SUCCESS;
}

static void live_tv_change_program(u16 list_focus, u8 group_index)
{
	livetv_url channel_url = {NULL};
	live_tv_fav_info fav_livetv_info = {0};
	live_db_prg_t p_pg = {0};
	u16 name_len;

	g_livetv.is_left = FALSE;
	if(group_index < g_livetv.grp_index.fav_grp)
	{
		if(group_index == g_livetv.grp_index.all_grp)
		{
			livetv_get_pg_by_group_pos(&p_pg, list_focus, INVALID_GRP);
		}  
		else
		{
			livetv_get_pg_by_group_pos(&p_pg, list_focus, group_index - 1);
		}
	}
	else if(group_index == g_livetv.grp_index.fav_grp)//fav
	{
		sys_status_get_fav_livetv_info(list_focus, &fav_livetv_info);
		livetv_get_pg_by_id(&p_pg, fav_livetv_info.live_fav_id);
	}
	else if(group_index == g_livetv.grp_index.cus_grp)
	{
		p_pg.url[0] = g_livetv.p_url_array[list_focus].url;
		name_len = strlen(g_livetv.p_url_array[list_focus].name);
		name_len = (name_len >= MAX_LIVE_NAME_LEN) ? (MAX_LIVE_NAME_LEN - 1) : name_len;
		strncpy(p_pg.name, g_livetv.p_url_array[list_focus].name, name_len);
		p_pg.name[name_len] = '\0';
		p_pg.src_idx = 0;
		p_pg.id = list_focus + g_livetv.max_net_pg_id + 1;
		p_pg.sate_id = 0; 
		p_pg.url_cnt = 1;
	}

	channel_url.url = p_pg.url[p_pg.src_idx];
	//OS_PRINTF("##play prog name == %s, url == %s##\n",p_pg.name, p_pg.url[p_pg.src_idx]);
	memcpy(&g_livetv.g_prev_pg, &g_livetv.g_curr_pg, sizeof(live_db_prg_t));
	memcpy(&g_livetv.g_curr_pg, &p_pg, sizeof(live_db_prg_t));
	g_livetv.g_circle_index = g_livetv.g_curr_pg.src_idx;
	if(g_livetv.g_vcircle_index == INVALIDID)
	{
		g_livetv.g_vcircle_index = g_livetv.g_playing_focus;
		// OS_PRINTF("####will change prog at this group, vcircle index == [%d]####\n", g_livetv.g_vcircle_index);
	}
	else
	{
		if(g_livetv.g_playing_focus == g_livetv.g_vcircle_index)
		{
			if(ctrl_get_sts(g_livetv.p_list_cont) == OBJ_STS_SHOW)
			{
				ctrl_set_sts(g_livetv.p_help, OBJ_STS_HIDE);
				ctrl_set_sts(g_livetv.p_list_cont, OBJ_STS_HIDE);
				ctrl_paint_ctrl(g_livetv.p_menu, TRUE);
			}
			g_livetv.g_vcircle_index = INVALIDID;
			mtos_printk("####livetv changed prog at this group one circle, stopped!!!####\n");
			return ;
		}
		else
		{
			if(ctrl_get_sts(g_livetv.p_list_cont) == OBJ_STS_SHOW)
			{
				if(get_cur_group_index() == cbox_dync_get_focus(g_livetv.p_grp))
				{
					list_set_focus_pos(g_livetv.p_list, list_focus);
					list_select_item(g_livetv.p_list, list_focus);
					ctrl_paint_ctrl(g_livetv.p_list_cont, TRUE);
				}
			}
			OS_PRINTF("####changing prog at this group, vcircle index == [%d]####\n", g_livetv.g_vcircle_index);
		}
	}

	
	ui_livetv_play_net_prog(channel_url.url);

	if(ctrl_get_sts(g_livetv.p_list_cont) == OBJ_STS_HIDE && ctrl_get_sts(g_livetv.p_gp_list_cont) == OBJ_STS_HIDE)
	{
		show_epg_bar_while_change_prog();
	}
	else
	{
		if(get_cur_group_index() == cbox_dync_get_focus(g_livetv.p_grp))
		{
			list_set_focus_pos(g_livetv.p_list, list_focus);
			list_select_item(g_livetv.p_list, list_focus);
			ctrl_paint_ctrl(g_livetv.p_list_cont, TRUE);
		}
	}
}
static RET_CODE on_live_tv_list_change_focus_or_program(control_t *p_ctrl, u16 msg,
														u32 para1, u32 para2)
{
	s16 list_focus = 0;
	u8 group_index;
	u32 cur_grp_cnt;

	OS_PRINTF("##%s, para2 = %d##\n", __FUNCTION__, para2);
	if(OBJ_STS_SHOW == ctrl_get_sts(g_livetv.p_list_cont) && para2 != 1)
	{
		ui_livetv_reset_hide_all_list();
		list_class_proc(g_livetv.p_list, msg, para1, para2);
		OS_PRINTF("###will return from %s##\n", __FUNCTION__);
		return ERR_FAILURE;
	}

	group_index = get_cur_group_index();
	
	if(!ui_livetv_check_net())
	{
		//jump_to_sat_group_first_prog();
		return SUCCESS;
	}

	if(ui_get_livetv_db_flag() == FALSE)
	{
		return SUCCESS;
	}

	if(group_index < g_livetv.grp_index.fav_grp)
	{
		if(group_index == g_livetv.grp_index.all_grp)
		{
			cur_grp_cnt = g_livetv.g_livetv_all_pg_cnt;
		}  
		else
		{
			cur_grp_cnt = livetv_get_cnt_by_group(group_index - 1);
		}
	}
	else if(group_index == g_livetv.grp_index.fav_grp)//fav
	{
		sys_status_get_fav_livetv_total_num(&cur_grp_cnt);
		if(cur_grp_cnt == 0)
		{
			OS_PRINTF("##%s, line = [%d]##\n", __FUNCTION__, __LINE__);
			group_index = g_livetv.grp_index.all_grp;
			cur_grp_cnt = g_livetv.g_livetv_all_pg_cnt;
			livetv_db_get_pos_by_id(&g_livetv.g_playing_focus, g_livetv.g_curr_pg.id);
			OS_PRINTF("##%s, line = [%d], playing fav id ==[%d], \
					  playing focus = [%d]##\n", __FUNCTION__, \
					  __LINE__,g_livetv.g_curr_pg.id, g_livetv.g_playing_focus);
		}
	}
	else if(group_index == g_livetv.grp_index.cus_grp)
	{
		cur_grp_cnt = g_livetv.cus_url_total;
	}

	list_focus = g_livetv.g_playing_focus;
	// OS_PRINTF("####line[%d], change prog focus index is :%d###\n", __LINE__, list_focus);  
	switch(msg)
	{
	case MSG_FOCUS_UP:
		list_focus ++;
		break;
	case MSG_FOCUS_DOWN:
		list_focus --;
		break;
	case MSG_PAGE_UP:
		list_focus -= LIVE_TV_LIST_ITEM_NUM_ONE_PAGE;
		break;
	case MSG_PAGE_DOWN:
		list_focus += LIVE_TV_LIST_ITEM_NUM_ONE_PAGE;
		break;  
	default:
		break;
	}

	if(list_focus < 0)
	{
		list_focus = cur_grp_cnt - 1;
	}
	if(list_focus > cur_grp_cnt - 1)
	{
		list_focus = 0;
	}

	g_livetv.g_playing_focus = list_focus;
	if(para2 != 1)//para2 == 1, means auto change prog
	{
		g_livetv.g_vcircle_index = INVALIDID;
	}
	live_tv_change_program(g_livetv.g_playing_focus, group_index);

	return SUCCESS;



}

static BOOL ui_livetv_check_focus_root()
{
	u8 focus_root;

	focus_root = fw_get_focus_id();

	if(ROOT_ID_LIVE_TV != focus_root && ROOT_ID_VOLUME_USB != focus_root)
	{
		return FALSE;
	}
	return TRUE;
}

static RET_CODE on_live_tv_load_media_success(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
      ui_livetv_hide_loading_time_dlg();
      ui_comm_dlg_close();
	if(!ui_livetv_check_focus_root())
	{
		return SUCCESS;
	}
	OS_PRINTF("###########on_live_tv_load_media_success#########\n");
	
	g_livetv.g_is_play_stoped = FALSE;
	
	
	return SUCCESS;
}


static void ui_livetv_print_load_media_error_msg(u16 msg)
{
	if(msg == MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT)
	{
		mtos_printk("##load media error msg is[MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT]##\n");
	}
	else if(msg == MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR)
	{
		mtos_printk("##load media error msg is[MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR]##\n");
	}
	else if(msg == MSG_VIDEO_EVENT_SET_PATH_FAIL)
	{
		mtos_printk("##load media error msg is[MSG_VIDEO_EVENT_SET_PATH_FAIL]##\n");
	}
	else
	{
		mtos_printk("##load media error msg is[UNKNOWN]##\n");
	}
}

static RET_CODE on_live_tv_load_media_error(control_t *p_cont, u16 msg, u32 
											para1, u32 para2)
{
	u8 group_index;
    
      ui_livetv_hide_loading_time_dlg();
      ui_comm_dlg_close();
	if(!ui_livetv_check_focus_root())
	{
		return SUCCESS;
	}

	OS_PRINTF("##%s, line[%d]##\n", __FUNCTION__, __LINE__);
	group_index = get_cur_group_index();

	ui_livetv_print_load_media_error_msg(msg);
	
	g_livetv.g_is_play_stoped = TRUE;

	if(!ui_livetv_check_net())
	{
		return ERR_FAILURE;
	}
			
	if(group_index == g_livetv.grp_index.cus_grp)
	{
		live_tv_open_dlg(IDS_PLAY_ERROR, 2000);
		on_live_tv_list_change_focus_or_program(NULL, MSG_FOCUS_UP, 0, 1);
	}
	else
	{
		OS_PRINTF("##%s, line[%d]##\n", __FUNCTION__, __LINE__);
		auto_change_url_src();
	}

	return SUCCESS;
}

static RET_CODE on_live_tv_request_to_change_url_src(control_t *p_cont, u16 msg, u32 
													 para1, u32 para2)
{
	OS_PRINTF("##%s##\n", __FUNCTION__);

	if(!ui_livetv_check_focus_root())
	{
		return SUCCESS;
	}

       if(g_livetv.g_curr_pg.url_cnt > 1)
       {
            live_tv_open_dlg(IDS_IF_CHANGE_URL, 2000);
       }
       else
       {
            live_tv_open_dlg(IDS_IF_CHANGE_CHAL, 2000);
       }

	return SUCCESS;
}


static RET_CODE on_live_tv_net_play_stop_cfm(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
	OS_PRINTF("##%s, line[%d]##\n", __FUNCTION__, __LINE__);

	if(1 == g_livetv.is_live_exit_state)
	{
		ui_comm_dlg_close();
		stop_all_timer_before_exit_livetv();
		manage_close_menu(ROOT_ID_LIVE_TV, 0, 0);
	}
	
	g_livetv.is_live_exit_state = 0;
	return SUCCESS;
	
}

static RET_CODE on_live_tv_net_play_load_media_stop_cfm(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
	OS_PRINTF("##%s, line[%d]##\n", __FUNCTION__, __LINE__);

	if(1 == g_livetv.is_live_exit_state)
	{
	      OS_PRINTF("##########load media stop cfm, will exit livetv####\n");
		ui_comm_dlg_close();
		stop_all_timer_before_exit_livetv();
		manage_close_menu(ROOT_ID_LIVE_TV, 0, 0);
             g_livetv.is_live_exit_state = 0;
	}
	else
	{
	       OS_PRINTF("##########load media stop cfm, load media error now####\n");
		on_live_tv_load_media_error(NULL, msg, 0, 0);
	}
	
	return SUCCESS;
	
}


static livetv_fav_sts ui_livetv_get_fav_status(live_db_prg_t *p_pg)
{
	live_tv_fav_info fav_info = {0};
	live_tv_fav_info fav_livetv_info = {0};
	u16 name_len = 0;
	u16 i;
	u32 g_fav_prog_num;

	sys_status_get_fav_livetv_total_num(&g_fav_prog_num);

	for(i = 0; i < g_fav_prog_num; i ++)
	{
		sys_status_get_fav_livetv_info(i, &fav_livetv_info);
		if(fav_livetv_info.live_fav_id == p_pg->id)
		{
			sys_status_del_fav_livetv_info(i);

			sys_status_save();
			OS_PRINTF("###this is fav prog, will delete fav##\n");
			return DEL_FAV;
		}
	}

	if(g_fav_prog_num >= FAV_LIVETV_NUM)
	{
		OS_PRINTF("#####fav num is max number##\n");
		return FULL_FAV;
	} 

	fav_info.live_fav_id = p_pg->id;
	name_len = strlen(p_pg->name);
	name_len = (name_len >= MAX_LIVE_NAME_LEN) ? (MAX_LIVE_NAME_LEN - 1) : name_len;
	memcpy(fav_info.live_fav_name, p_pg->name, name_len);
	fav_info.live_fav_name[name_len] = '\0';

	sys_status_set_fav_livetv_info(g_fav_prog_num, &fav_info);
	g_fav_prog_num++;
	sys_status_set_fav_livetv_total_num(g_fav_prog_num);
	sys_status_save();

	return ADD_FAV;

}

static RET_CODE on_live_tv_add_fav_prog(control_t *p_list, u16 msg, u32 
										para1, u32 para2)
{
	live_db_prg_t p_pg = {0};
	livetv_fav_sts fav_status;
	u8 group_focus;
	u8 group_index;
	u16 list_focus;

	group_index = get_cur_group_index();
	group_focus = cbox_dync_get_focus(g_livetv.p_grp);

	OS_PRINTF("###########on_live_tv_add_fav_prog#########\n");
	ui_livetv_reset_hide_all_list();
	if(OBJ_STS_HIDE == ctrl_get_sts(g_livetv.p_list_cont))
	{
		if(group_index != g_livetv.grp_index.cus_grp)//except customer
		{
			fav_status = ui_livetv_get_fav_status(&g_livetv.g_curr_pg);
			if(fav_status == ADD_FAV)
			{
				OS_PRINTF("##add curr to fav##\n");
				live_tv_open_dlg(IDS_LIVETV_ADD_ONE_FAV, 500);
			}
			else if(fav_status == DEL_FAV)
			{
				OS_PRINTF("##del curr from fav##\n");
				live_tv_open_dlg(IDS_LIVETV_DEL_ONE_FAV, 500);
			}
			else if(fav_status == FULL_FAV)
			{
				OS_PRINTF("##fav is full##\n");
				live_tv_open_dlg(IDS_FAV_PROG_IS_FULL, 500);
			}
		}
		OS_PRINTF("###list hide will return from add fav##\n");
		return ERR_FAILURE;
	}
	list_focus = list_get_focus_pos(p_list);
	if(group_focus < g_livetv.grp_index.fav_grp)
	{
		if(group_focus == 0)
		{
			livetv_get_pg_by_group_pos(&p_pg, list_focus, INVALID_GRP);
		}
		else
		{
			livetv_get_pg_by_group_pos(&p_pg, list_focus, group_focus - 1);
		}
		fav_status = ui_livetv_get_fav_status(&p_pg);
		if(fav_status == ADD_FAV)
		{
			OS_PRINTF("##add fav success##\n");
			list_set_field_content_by_icon(p_list,list_focus, 2, IM_TV_FAV);
			live_tv_open_dlg(IDS_LIVETV_ADD_ONE_FAV, 500);
		}
		else if(fav_status == DEL_FAV)
		{
			OS_PRINTF("##del a fav##\n");
			list_set_field_content_by_icon(p_list,list_focus, 2, 0);
			live_tv_open_dlg(IDS_LIVETV_DEL_ONE_FAV, 500);
		}
		else if(fav_status == FULL_FAV)
		{
			OS_PRINTF("##fav is full##\n");
			live_tv_open_dlg(IDS_FAV_PROG_IS_FULL, 500);
			return ERR_FAILURE;
		}
		live_tv_list_update(p_list, list_get_valid_pos(p_list), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);
		list_class_proc(p_list, MSG_FOCUS_DOWN, 0, 0);

		ctrl_paint_ctrl(g_livetv.p_list_cont, TRUE);
	}
	else if(group_focus == g_livetv.grp_index.fav_grp)//fav grp
	{
		u32 cur_fav_cnt;
		sys_status_get_fav_livetv_total_num(&cur_fav_cnt);
		if(cur_fav_cnt == 0)
		{
			OS_PRINTF("###fav number is 0, can not cancel fav##\n");
			list_set_count(p_list, cur_fav_cnt, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
			live_tv_list_update(p_list, list_get_valid_pos(p_list), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);
			if(get_cur_group_index() == g_livetv.grp_index.fav_grp)
			{
				group_index = g_livetv.grp_index.all_grp;
				livetv_db_get_pos_by_id(&g_livetv.g_playing_focus, g_livetv.g_curr_pg.id);
				OS_PRINTF("##%s, line = [%d], playing fav id ==[%d], \
							  playing focus = [%d]##\n", __FUNCTION__, \
							  __LINE__,g_livetv.g_curr_pg.id, g_livetv.g_playing_focus);
			}
			return ERR_FAILURE;
		}
		sys_status_del_fav_livetv_info(list_focus);
		sys_status_save();
		list_set_field_content_by_icon(p_list,list_focus, 2, 0);
		live_tv_open_dlg(IDS_LIVETV_DEL_ONE_FAV, 500);
		sys_status_get_fav_livetv_total_num(&cur_fav_cnt);
		if(cur_fav_cnt != 0)
		{
			list_set_focus_pos(p_list, 0);
		}
		list_set_count(p_list, cur_fav_cnt, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
		live_tv_list_update(p_list, list_get_valid_pos(p_list), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);
		ctrl_paint_ctrl(g_livetv.p_list_cont, TRUE);
	}


	return SUCCESS;
}

static RET_CODE on_live_tv_list_change_group(u16 msg)
{
	u16 group_focus = 0;
	u32 count = 0;
	u16 list_focus;
	u8 group_index;

	
	OS_PRINTF("##get_livetv_init_status() == %d##\n", ui_get_init_livetv_status());
	if(ui_get_init_livetv_status() == LIVETV_INIT_FAILED)
	{
		ui_live_tv_init();
		ui_set_init_livetv_status(LIVETV_INITING);//LIVETV_NOT_INIT
		live_tv_open_dlg(IDS_DOWNLOAD_TO_WAIT, 5000);
	}
	cbox_class_proc(g_livetv.p_grp, msg, 0, 0);

	group_focus = cbox_dync_get_focus(g_livetv.p_grp);
	OS_PRINTF("###%s,%d,  change group focus == %d##\n",__FUNCTION__,__LINE__,group_focus);
	ctrl_set_sts(g_livetv.p_help, OBJ_STS_SHOW);

	if(group_focus < g_livetv.grp_index.fav_grp)
	{
		if(group_focus == 0)
		{
			count = g_livetv.g_livetv_all_pg_cnt;
		}
		else
		{
			count = livetv_get_cnt_by_group(group_focus - 1);
		}
		if(count == 0 || count > g_livetv.g_livetv_all_pg_cnt)
		{
			fw_notify_root(g_livetv.p_list, NOTIFY_T_MSG, FALSE, msg, 0, 0);
			return SUCCESS;
		}
	}
	else if(group_focus == g_livetv.grp_index.fav_grp)//fav
	{
		sys_status_get_fav_livetv_total_num(&count);
		OS_PRINTF("##%s, line[%d], fav count = %d##\n", __FUNCTION__, __LINE__, count);
	}
	else if(group_focus == g_livetv.grp_index.cus_grp)
	{
		count = g_livetv.cus_url_total;
             ctrl_set_sts(g_livetv.p_help, OBJ_STS_HIDE);
		OS_PRINTF("###customer total number == %d##\n",count);
	}
	
	OS_PRINTF("##%s, this group count == %d##\n", __FUNCTION__, count);
	list_set_count(g_livetv.p_list, count, LIVE_TV_LIST_ITEM_NUM_ONE_PAGE);
	group_index = get_cur_group_index();
	if(group_focus == group_index)
	{
		list_set_focus_pos(g_livetv.p_list, g_livetv.g_playing_focus);
		list_select_item(g_livetv.p_list, g_livetv.g_playing_focus);
	}
	else
	{
		if(group_focus == g_livetv.grp_index.all_grp)
		{
			if(group_index == g_livetv.grp_index.cus_grp)
			{
				list_set_focus_pos(g_livetv.p_list, 0);
			}
			else
			{
				livetv_db_get_pos_by_id(&list_focus, g_livetv.g_curr_pg.id);
				list_set_focus_pos(g_livetv.p_list, list_focus);
				list_select_item(g_livetv.p_list, list_focus);
			}
		}
		else
		{
			if(count > 0)
			{
				list_set_focus_pos(g_livetv.p_list, 0);
			}
			else
			{
				list_set_focus_pos(g_livetv.p_list, INVALID);
			}
		}
	}

	list_set_update(g_livetv.p_list, live_tv_list_update, 0);
	live_tv_list_update(g_livetv.p_list, list_get_valid_pos(g_livetv.p_list), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);
	ctrl_paint_ctrl(g_livetv.p_menu, TRUE);

	return SUCCESS;
}

static RET_CODE on_live_tv_list_change_group_or_volume(control_t *p_ctrl, u16 msg,
														u32 para1, u32 para2)
{
	if(OBJ_STS_HIDE == ctrl_get_sts(g_livetv.p_list_cont))
	{
	      if(OBJ_STS_HIDE == ctrl_get_sts(g_livetv.p_gp_list_cont))
            {   
                manage_open_menu(ROOT_ID_VOLUME_USB, ROOT_ID_LIVE_TV, para1);
                OS_PRINTF("##live tv change volume! %s##\n", __FUNCTION__);
            }
            else
            {
                live_tv_all_list_hide();                
            }
	}
	else
	{
		ui_livetv_reset_hide_all_list();
		on_live_tv_list_change_group(msg);
	}

	return SUCCESS;
}

/************************************************
number play functions

************************************************/
static BOOL live_tv_input_num(control_t *parent, u8 num, BOOL is_update)
{
	control_t *p_subctrl;
	u32 curn;

	if (g_livetv.g_num_play_bit < LIVE_TV_NUM_PLAY_CNT)
	{
		p_subctrl = ctrl_get_child_by_id(g_livetv.p_num_play, IDC_LIVE_TV_NUM_TXT);

		curn = nbox_get_num(p_subctrl);
		curn = curn * 10 + num;
		nbox_set_num_by_dec(p_subctrl, curn);

		g_livetv.g_num_play_bit++;
		if (is_update)
		{
			ctrl_paint_ctrl(p_subctrl, TRUE);
		}

		return TRUE;
	}

	return FALSE;
}

static RET_CODE on_live_tv_show_number_play(control_t *p_ctrl, u16 msg,
											u32 para1, u32 para2)
{
        control_t *p_active = NULL;
	u8 group_focus;
	u8 group_index;
      
       
       p_active = ctrl_get_active_ctrl(g_livetv.p_menu);
	//fix epg list can't auto close
	close_today_epg_list_while_change_prog();

	group_index = get_cur_group_index();
	if(group_index == g_livetv.grp_index.cus_grp)
	{
		OS_PRINTF("##%s, lookback prog playing, will return##\n", __FUNCTION__);
		return ERR_FAILURE;
	}

	group_focus = cbox_dync_get_focus(g_livetv.p_grp);

	if(OBJ_STS_HIDE == ctrl_get_sts(g_livetv.p_num_play))
	{
		ctrl_set_sts(g_livetv.p_num_play, OBJ_STS_SHOW);
		ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
		ctrl_process_msg(g_livetv.p_num_play, MSG_GETFOCUS, 0, 0);
	}
	g_livetv.g_num_play_bit = 0;
	live_tv_input_num(g_livetv.p_num_play, (u8)(para1 - V_KEY_0), FALSE);
	ctrl_paint_ctrl(g_livetv.p_menu, TRUE);

	/* create tmr for jump */
	ui_livetv_start_timer(MSG_SELECT, 3000);
	return SUCCESS;
}


static void jump_to_comm_network_prog(u16 curn, control_t *p_cont)
{
	livetv_url channel_url = {0};
	live_db_prg_t p_pg = {0};
	comm_dlg_data_t dlg_data = //popup dialog data
	{
		ROOT_ID_INVALID,
		DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
		COMM_DLG_X, COMM_DLG_Y,
		COMM_DLG_W, COMM_DLG_H,
		IDS_MSG_INVALID_NUMBER,
		0,
	};

	if(!livetv_get_pg_by_id(&p_pg, curn))
	{
		ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
		ui_comm_dlg_open(&dlg_data);
		return;
	}  
	set_cur_group_index(g_livetv.grp_index.all_grp);//set curr playing grp is all grp
	channel_url.url = p_pg.url[p_pg.src_idx];
	//  OS_PRINTF("##play num index == %d, name == %s, url ==  %s##\n",curn, p_pg.name, p_pg.url[p_pg.src_idx]);
	memcpy(&g_livetv.g_prev_pg, &g_livetv.g_curr_pg, sizeof(live_db_prg_t));
	memcpy(&g_livetv.g_curr_pg, &p_pg, sizeof(live_db_prg_t));
	g_livetv.g_circle_index = g_livetv.g_curr_pg.src_idx;

	livetv_db_get_pos_by_id(&g_livetv.g_playing_focus, curn);
	OS_PRINTF("####jump comm prog g_is_playin_focus  == %d##\n", g_livetv.g_playing_focus);
	if(ctrl_get_sts(g_livetv.p_list_cont) == OBJ_STS_HIDE && ctrl_get_sts(g_livetv.p_gp_list_cont) == OBJ_STS_HIDE)
	{
		show_epg_bar_while_change_prog();
	}
	else
	{
		list_set_focus_pos(g_livetv.p_list, g_livetv.g_playing_focus);
		list_select_item(g_livetv.p_list, g_livetv.g_playing_focus);
		ctrl_paint_ctrl(g_livetv.p_list_cont, TRUE);
	}
	
	ui_livetv_play_net_prog(channel_url.url);
	
}

static void jump_to_customer_net_prog(u16 curn, control_t *p_cont)
{
	livetv_url channel_url = {0};
	live_db_prg_t p_pg = {0};
	u16 name_len;
	comm_dlg_data_t dlg_data = //popup dialog data
	{
		ROOT_ID_INVALID,
		DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
		COMM_DLG_X, COMM_DLG_Y,
		COMM_DLG_W, COMM_DLG_H,
		IDS_MSG_INVALID_NUMBER,
		0,
	};

	{
		ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
		ui_comm_dlg_open(&dlg_data);
		return ;
	}

	if(curn > g_livetv.max_net_pg_id + g_livetv.cus_url_total)
	{
		ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
		ui_comm_dlg_open(&dlg_data);
		return;
	}  
	set_cur_group_index(g_livetv.grp_index.cus_grp);//set curr playing grp is customer grp
	g_livetv.g_playing_focus = curn - g_livetv.max_net_pg_id - 1;
	OS_PRINTF("####jump comm prog g_is_playin_focus  == %d##\n", g_livetv.g_playing_focus);
	p_pg.url[0] = g_livetv.p_url_array[g_livetv.g_playing_focus].url;
	name_len = strlen(g_livetv.p_url_array[g_livetv.g_playing_focus].name);
	name_len = (name_len >= MAX_LIVE_NAME_LEN) ? (MAX_LIVE_NAME_LEN - 1) : name_len;
	strncpy(p_pg.name, g_livetv.p_url_array[g_livetv.g_playing_focus].name, name_len);
	p_pg.name[name_len] = '\0';
	p_pg.src_idx = 0;
	p_pg.id = curn;
	channel_url.url = p_pg.url[p_pg.src_idx];
	p_pg.sate_id = 0; 
	p_pg.url_cnt = 1;
	// OS_PRINTF("##play num index == %d, name == %s, url ==  %s##\n",curn, p_pg.name, p_pg.url[p_pg.src_idx]);
	memcpy(&g_livetv.g_prev_pg, &g_livetv.g_curr_pg, sizeof(live_db_prg_t));
	memcpy(&g_livetv.g_curr_pg, &p_pg, sizeof(live_db_prg_t));
	ui_livetv_play_net_prog(channel_url.url);
	if(ctrl_get_sts(g_livetv.p_list_cont) == OBJ_STS_HIDE && ctrl_get_sts(g_livetv.p_gp_list_cont) == OBJ_STS_HIDE)
	{
		show_epg_bar_while_change_prog();
	}
	else
	{
		live_tv_all_list_hide();
	}
}

static void live_tv_jump_to_prog(control_t *p_cont)
{
	control_t *p_subctrl = NULL;
	u16 curn;
	u8 group_index;

	group_index = get_cur_group_index();
	
	g_livetv.is_auto_change = FALSE;
	p_subctrl = ctrl_get_child_by_id(g_livetv.p_num_play, IDC_LIVE_TV_NUM_TXT);
	curn = (u16)nbox_get_num(p_subctrl);

	/* jump to prog */
	if(ui_livetv_check_net())
	{
		if(curn <= g_livetv.max_net_pg_id)
		{
			jump_to_comm_network_prog(curn, p_cont);
		}
		else
		{
			jump_to_customer_net_prog(curn, p_cont);
		}
	}
	ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);

	return;
}


static RET_CODE on_live_tv_input_num(control_t *p_ctrl, u16 msg, 
									 u32 para1, u32 para2)
{
	u8 num = (u8)(MSG_DATA_MASK & msg);
	live_tv_input_num(p_ctrl, num, TRUE);

	return SUCCESS;
}

static RET_CODE on_live_tv_num_select(control_t *p_ctrl, u16 msg, 
									  u32 para1, u32 para2)
{
	g_livetv.g_vcircle_index = INVALIDID;
	live_tv_jump_to_prog(p_ctrl);
	return SUCCESS;
}

static RET_CODE on_live_tv_exit_num_play(control_t *p_ctrl, u16 msg, 
										 u32 para1, u32 para2)
{
	control_t *p_num_txt = NULL;
      control_t *p_active = NULL;
      
       p_active = ctrl_get_active_ctrl(g_livetv.p_menu);
	p_num_txt = ctrl_get_child_by_id(p_ctrl, IDC_LIVE_TV_NUM_TXT);
	ctrl_set_sts(p_ctrl, OBJ_STS_HIDE);
	ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
       if(ctrl_get_sts(g_livetv.p_gp_list_cont) == OBJ_STS_HIDE)
       {
	    ctrl_process_msg(g_livetv.p_list, MSG_GETFOCUS, 0, 0);
       }
       else
       {
           ctrl_process_msg(g_livetv.p_gp_list, MSG_GETFOCUS, 0, 0);
       }
	ctrl_paint_ctrl(p_ctrl->p_parent, TRUE);
	nbox_set_num_by_dec(p_num_txt, 0);
	ui_livetv_stop_timer(MSG_SELECT);
	return ERR_NOFEATURE;
}

/******************************update epg list details***********************/

static u16 get_curr_prog_curr_pg_epg_index(u16 current_sys_time_min)
{
	BOOL find = FALSE;
	u16 i;

	OS_PRINTF("###%s, current_sys_time_min == %d##\n", __FUNCTION__, current_sys_time_min);
	if(g_livetv.p_epg == NULL)
	{
		mtos_printk("##%s, line[%d],g_livetv.p_epg is null##\n", __FUNCTION__, __LINE__);
		return LIVE_TV_INVALID_ID;
	}

	for(i = 0; i < g_livetv.p_epg->pgms_num; i ++)
	{
		if(g_livetv.p_epg->pgms[i].start_min >= current_sys_time_min)
		{
			OS_PRINTF("###next epg index == %d##\n", i);
			find = TRUE;
			break;
		}
	}
	if(find == TRUE)
	{
		if(i == 0)
		{
			return i;
		}

		return (i - 1);
	}
	else
	{
		return LIVE_TV_INVALID_ID;
	}

}

static RET_CODE live_tv_today_epg_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
	u8 asc_str[256] = {0};
	u16 uni_str[256] = {0};
	u16 i, cnt;

	if(g_livetv.p_epg == NULL)
	{
		return ERR_FAILURE;
	}

	cnt = list_get_count(p_list);
	for (i = start; (i < (start + size)) && (i < cnt); i++)
	{
		sprintf(asc_str, "%.3d ", (u16)(i + 1));
		list_set_field_content_by_ascstr(p_list, i, 0, asc_str);
		sprintf(asc_str, "%s %s", \
			g_livetv.p_epg->pgms[g_livetv.g_t_epg.epg_detail_index+i].st,\
			g_livetv.p_epg->pgms[g_livetv.g_t_epg.epg_detail_index+i].na);
		convert_utf8_chinese_asc2unistr(asc_str, uni_str, sizeof(asc_str));
		list_set_field_content_by_unistr(p_list, i, 1, uni_str); 

	}
	return SUCCESS;
}
static RET_CODE on_live_tv_show_today_epg(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	control_t *p_active = NULL;
	u16 cnt;

	if(g_livetv.p_epg == NULL || g_livetv.g_t_epg.epg_detail_index == LIVE_TV_INVALID_ID)
	{
		OS_PRINTF("###this prog no epg, not need show epg list##\n");
		ui_livetv_start_timer(MSG_HIDE_EPG, 2000);
		return ERR_FAILURE;
	}

	if(fw_find_root_by_id(ROOT_ID_MUTE) != NULL)
	{
		close_mute();
	}
	cnt = g_livetv.p_epg->pgms_num - g_livetv.g_t_epg.epg_detail_index;
	p_active = ctrl_get_active_ctrl(g_livetv.p_menu);
	list_set_count(g_livetv.p_today_epg, cnt, LIVE_TV_TODAY_EPG_EVT_LIST_ONE_PAGE);
	list_set_focus_pos(g_livetv.p_today_epg, 0);
	ctrl_set_sts(g_livetv.p_today_epg_cont, OBJ_STS_SHOW);
	ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
	ctrl_process_msg(g_livetv.p_today_epg, MSG_GETFOCUS, 0, 0);
	live_tv_today_epg_list_update(g_livetv.p_today_epg, list_get_valid_pos(g_livetv.p_today_epg), 
		LIVE_TV_TODAY_EPG_EVT_LIST_ONE_PAGE, 0);
	ctrl_paint_ctrl(g_livetv.p_menu, TRUE);
	ui_livetv_start_timer(MSG_HIDE_TODAY_EPG, 5000);

	return SUCCESS;
}

static RET_CODE on_live_tv_epg_list_exit(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
       control_t *p_active = NULL;
      
       p_active = ctrl_get_active_ctrl(g_livetv.p_menu);
	ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
	if(ctrl_get_sts(g_livetv.p_gp_list_cont) == OBJ_STS_HIDE)
       {
	    ctrl_process_msg(g_livetv.p_list, MSG_GETFOCUS, 0, 0);
       }
       else
       {
           ctrl_process_msg(g_livetv.p_gp_list, MSG_GETFOCUS, 0, 0);
       }
	ctrl_set_sts(g_livetv.p_today_epg_cont, OBJ_STS_HIDE);
	ui_livetv_start_timer(MSG_HIDE_EPG, 2000);
	ui_livetv_stop_timer(MSG_HIDE_TODAY_EPG);
	ctrl_paint_ctrl(g_livetv.p_menu, TRUE);
	if(ui_is_mute())
	{
		open_mute(0, 0);
	}
	return SUCCESS;
}

static RET_CODE on_live_tv_epg_list_up_down(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	ui_livetv_start_timer(MSG_HIDE_TODAY_EPG, 5000);
	list_class_proc(p_list, msg, para1, para2);

	return ERR_FAILURE;
}

static void update_epg_network_sts()
{
	u16 net_sts[4] = 
	{
		IDS_ETHERNET,
		IDS_WIFI,
		IDS_3G,
		IDS_NONE
	};
	control_t *p_net = NULL;
	net_conn_stats_t sts = {0};

	sts = ui_get_net_connect_status();
	p_net = ctrl_get_child_by_id(g_livetv.p_epg_frm, IDC_EPG_NET_CONN_TEXT);
	if(sts.is_eth_conn || sts.is_usb_eth_conn)
	{
		text_set_content_by_strid(p_net, net_sts[0]);
	}
	else if(sts.is_wifi_conn)
	{
		text_set_content_by_strid(p_net, net_sts[1]);
	}
	else if(sts.is_3g_conn)
	{
		text_set_content_by_strid(p_net, net_sts[2]);
	}
	else
	{
		text_set_content_by_strid(p_net, net_sts[3]);
	}
}

static void update_epg_info_bar(control_t *p_epg_frm, BOOL is_show)
{
	control_t *p_sub = NULL;
	u16 uni_str[128] = {0};
	u8 asc_str[128] = {0};
	u8 time_str[32] = {0};
       utc_time_t time = {0};
       utc_time_t gmt_time = {0};
        time_set_t p_set={{0}};
	u8 group_index;
	u16 curr_epg_pg_index = LIVE_TV_INVALID_ID;

	group_index = get_cur_group_index();
	//program name
	p_sub = ctrl_get_child_by_id(p_epg_frm, IDC_EPG_CURR_P_NAME);

	if(group_index == g_livetv.grp_index.cus_grp)//customer grp
	{
		sprintf(asc_str, "%d %s", g_livetv.g_curr_pg.id - g_livetv.max_net_pg_id, g_livetv.g_curr_pg.name);
		convert_gb2312_chinese_asc2unistr(asc_str, uni_str, sizeof(asc_str));
	} 
	else
	{
		sprintf(asc_str, "%d %s", g_livetv.g_curr_pg.id, g_livetv.g_curr_pg.name);
		convert_utf8_chinese_asc2unistr(asc_str, uni_str, sizeof(asc_str));
	}
	OS_PRINTF("###epg bar epg name == %s##\n", asc_str);
	text_set_content_by_unistr(p_sub, uni_str);

	//current time
        sys_status_get_time(&p_set);
        time_get(&time, p_set.gmt_usage);
        if( p_set.gmt_usage)
        {
            time_to_local(&time, &gmt_time);
            memcpy(&time,&gmt_time,sizeof(gmt_time));
        }
	p_sub = ctrl_get_child_by_id(p_epg_frm, IDC_EPG_CURR_TIME_TEXT);
	sprintf(time_str, "%.4d-%.2d-%.2d  %.2d:%.2d", time.year, time.month, time.day, time.hour, time.minute);
	text_set_content_by_ascstr(p_sub, time_str);

	p_sub = ctrl_get_child_by_id(p_epg_frm, IDC_EPG_URL_SRC);
	text_set_content_by_strid(p_sub, IDS_LIVETV_CHANGE_SRC);

	//current network connect status
	update_epg_network_sts();

	//src url number
	p_sub = ctrl_get_child_by_id(p_epg_frm, IDC_EPG_URL_SRC_NUM);
	if(get_cur_group_index() == g_livetv.grp_index.cus_grp)//customer grp
	{
		sprintf(asc_str,"%s", "(1/1)");
	}
	else
	{
		sprintf(asc_str,"(%d/%d)", g_livetv.g_curr_pg.src_idx + 1, g_livetv.g_curr_pg.url_cnt);
	}
	text_set_content_by_ascstr(p_sub, asc_str);

	//current prog name 
	p_sub = ctrl_get_child_by_id(p_epg_frm, IDC_EPG_CURR_PROG_NAME);
	if(g_livetv.p_epg == NULL || get_cur_group_index() == g_livetv.grp_index.cus_grp || (!is_show))
	{
		u8 temp_asc[16] = "None";
		convert_gb2312_chinese_asc2unistr(temp_asc, uni_str, sizeof(temp_asc));
		g_livetv.g_t_epg.epg_detail_index = LIVE_TV_INVALID_ID;
	}
	else
	{
		curr_epg_pg_index = get_curr_prog_curr_pg_epg_index(60*(time.hour)+time.minute);
		OS_PRINTF("###curr_epg_pg_index == %d##\n", curr_epg_pg_index);
		g_livetv.g_t_epg.epg_detail_index = curr_epg_pg_index;
		if(curr_epg_pg_index != LIVE_TV_INVALID_ID)
		{
			OS_PRINTF("##curr epg prog name == %s##\n",g_livetv.p_epg->pgms[curr_epg_pg_index].na);
			sprintf(asc_str, "%s%s", g_livetv.p_epg->pgms[curr_epg_pg_index].st, g_livetv.p_epg->pgms[curr_epg_pg_index].na);
			convert_utf8_chinese_asc2unistr(asc_str, uni_str, sizeof(asc_str));
		}
		else
		{
			OS_PRINTF("##curr epg prog name == null##\n");
		}
	}
	text_set_content_by_unistr(p_sub, uni_str);

	//next prog name 
	p_sub = ctrl_get_child_by_id(p_epg_frm, IDC_EPG_NEXT_PROG_NAME);
	if(g_livetv.p_epg == NULL || get_cur_group_index() == g_livetv.grp_index.cus_grp || 
		curr_epg_pg_index == LIVE_TV_INVALID_ID || (!is_show))
	{
		u8 temp_asc[16] = "None";
		convert_gb2312_chinese_asc2unistr(temp_asc, uni_str, sizeof(temp_asc));
	}
	else
	{
		OS_PRINTF("##next epg prog name == %s##\n",g_livetv.p_epg->pgms[curr_epg_pg_index+1].na);
		sprintf(asc_str, "%s%s", g_livetv.p_epg->pgms[curr_epg_pg_index+1].st, g_livetv.p_epg->pgms[curr_epg_pg_index+1].na);
		convert_utf8_chinese_asc2unistr(asc_str, uni_str, sizeof(asc_str));
	}
	text_set_content_by_unistr(p_sub, uni_str);

}

static void show_epg_bar_while_change_prog()
{
	if(ctrl_get_sts(g_livetv.p_epg_frm) == OBJ_STS_SHOW)
	{
		ui_livetv_stop_timer(MSG_HIDE_EPG);
		ctrl_set_sts(g_livetv.p_epg_frm, OBJ_STS_HIDE);
	}
	fw_notify_root(g_livetv.p_menu, NOTIFY_T_MSG, TRUE, MSG_SHOW_EPG_OR_SEEK_BAR, 0, 0);
}

static RET_CODE on_live_tv_start_to_load_curr_pg_epg_or_show_seek_bar(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	control_t *p_cont = NULL;
       control_t *p_active = NULL;
      
       p_active = ctrl_get_active_ctrl(g_livetv.p_menu);
	OS_PRINTF("##%s##\n", __FUNCTION__);  
	p_cont = g_livetv.p_menu;

	if(ctrl_get_sts(g_livetv.p_list_cont) == OBJ_STS_SHOW)
	{
		ctrl_set_sts(g_livetv.p_help, OBJ_STS_HIDE);
		ctrl_set_sts(g_livetv.p_list_cont, OBJ_STS_HIDE);
		ctrl_paint_ctrl(p_cont, TRUE);
	}
    
       if(ctrl_get_sts(g_livetv.p_gp_list_cont) == OBJ_STS_SHOW)
	{
		ctrl_set_sts(g_livetv.p_gp_help, OBJ_STS_HIDE);
		ctrl_set_sts(g_livetv.p_gp_list_cont, OBJ_STS_HIDE);
		ctrl_paint_ctrl(p_cont, TRUE);
	}
       
#if ENABLE_IPTV_EPG
	if(get_cur_group_index() == g_livetv.grp_index.cus_grp)//except customer
#endif
	{
		OS_PRINTF("###playing customer prog, not need to load epg####\n");
		on_live_tv_show_epg_info_bar(NULL, 0, 0, 0);
		return ERR_FAILURE;
	}

	if(ctrl_get_sts(g_livetv.p_epg_frm) == OBJ_STS_SHOW)
	{
		ui_livetv_stop_timer(MSG_HIDE_EPG);
		ctrl_process_msg(p_cont, MSG_SHOW_TODAY_EPG, 0, 0);
		ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
		ctrl_process_msg(g_livetv.p_today_epg, MSG_GETFOCUS, 0, 0);
	}
	else
	{
		if(ui_livetv_check_net())
		{
			if(g_livetv.is_loading_epg == FALSE)
			{
				if(g_livetv.days[0].year == 2009)
				{
					get_several_days_time(g_livetv.days, MAX_LOOK_BACK_INFO_DAY_NUM);
				}
				ui_live_tv_api_one_pg_one_day_epg_load(&g_livetv.days[0], g_livetv.g_curr_pg.id, g_livetv.g_curr_pg.name);
				g_livetv.is_loading_epg = TRUE;
			}
			ui_livetv_start_timer(MSG_HIDE_EPG, 2000);
			update_epg_info_bar(g_livetv.p_epg_frm, FALSE);
			ctrl_set_sts(g_livetv.p_epg_frm, OBJ_STS_SHOW);
			ctrl_paint_ctrl(p_cont, TRUE);
		}
	}
	return SUCCESS;
}

static RET_CODE on_live_tv_show_epg_info_bar(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_cont = NULL;

	p_cont = g_livetv.p_menu;
	if(ctrl_get_sts(g_livetv.p_list_cont) == OBJ_STS_SHOW || ctrl_get_sts(g_livetv.p_gp_list_cont) == OBJ_STS_SHOW)
	{
		OS_PRINTF("##on_live_tv_show_epg_info_bar, ok list is show, will return##\n");
		return ERR_FAILURE;
	}

	ctrl_set_sts(g_livetv.p_epg_frm, OBJ_STS_SHOW);
	ui_livetv_start_timer(MSG_HIDE_EPG, 2000);

	update_epg_info_bar(g_livetv.p_epg_frm, TRUE);

	ctrl_paint_ctrl(p_cont, TRUE);

	return SUCCESS;
}

static RET_CODE on_live_tv_hide_epg_info_bar(control_t *p_ctrl, u16 msg,
											 u32 para1, u32 para2)
{
	ctrl_set_sts(g_livetv.p_epg_frm, OBJ_STS_HIDE);
	ui_livetv_stop_timer(MSG_HIDE_EPG);
	if(ctrl_get_sts(g_livetv.p_today_epg_cont) == OBJ_STS_HIDE)
	{
            if(ctrl_get_sts(g_livetv.p_gp_list_cont) == OBJ_STS_HIDE)
            {
                ctrl_process_msg(g_livetv.p_list, MSG_GETFOCUS, 0, 0);
            }
            else
            {
                ctrl_process_msg(g_livetv.p_gp_list, MSG_GETFOCUS, 0, 0);
            }
	}
	ctrl_paint_ctrl(g_livetv.p_menu, TRUE);

	return SUCCESS;
}

static RET_CODE on_live_tv_recall(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{ 
	livetv_url channel_url = {0};
	live_db_prg_t temp_pg = {0};
	u8 group_index;

	group_index = get_cur_group_index();
	g_livetv.g_vcircle_index = INVALIDID;
	g_livetv.is_auto_change = FALSE;
	if(group_index == g_livetv.grp_index.cus_grp)
	{
		OS_PRINTF("##playing lookbk or sat or cus prog, can not recall##\n");
		return ERR_FAILURE;
	}
	ctrl_set_sts(g_livetv.p_help, OBJ_STS_HIDE);
	ctrl_set_sts(g_livetv.p_list_cont, OBJ_STS_HIDE);
	ctrl_paint_ctrl(g_livetv.p_list_cont, TRUE);
	if(g_livetv.g_prev_pg.name != NULL)
	{
		if(strlen(g_livetv.g_prev_pg.name) != 0)
		{
			OS_PRINTF("##curr pg id :%d, max pg id = %d##\n", g_livetv.g_prev_pg.id, g_livetv.max_net_pg_id);

			if(g_livetv.g_prev_pg.id <= g_livetv.max_net_pg_id)
			{
				memcpy(&temp_pg, &g_livetv.g_curr_pg, sizeof(live_db_prg_t));
				memcpy(&g_livetv.g_curr_pg, &g_livetv.g_prev_pg, sizeof(live_db_prg_t));
				memcpy(&g_livetv.g_prev_pg, &temp_pg, sizeof(live_db_prg_t));
			}
			set_cur_group_index(g_livetv.grp_index.all_grp);
			g_livetv.g_circle_index = g_livetv.g_curr_pg.src_idx;
			livetv_db_get_pos_by_id(&g_livetv.g_playing_focus, g_livetv.g_curr_pg.id);
			OS_PRINTF("##%s, line[%d], playing focus = %d##\n", __FUNCTION__, __LINE__, g_livetv.g_playing_focus);
			channel_url.url = g_livetv.g_curr_pg.url[g_livetv.g_curr_pg.src_idx];
			//   OS_PRINTF("##will recall prev prog name =%s, url=%s##\n", g_livetv.g_curr_pg.name, 
			//   g_livetv.g_curr_pg.url[g_livetv.g_curr_pg.src_idx]);

			if(ctrl_get_sts(g_livetv.p_list_cont) == OBJ_STS_HIDE && ctrl_get_sts(g_livetv.p_gp_list_cont) == OBJ_STS_HIDE)
			{
				show_epg_bar_while_change_prog();
			}
			
			ui_livetv_play_net_prog(channel_url.url);
		}
		else
		{
			channel_url.url = g_livetv.g_curr_pg.url[g_livetv.g_curr_pg.src_idx];
			//     OS_PRINTF("##will recall prev prog name =%s, url=%s##\n", g_livetv.g_curr_pg.name, 
			//     g_livetv.g_curr_pg.url[g_livetv.g_curr_pg.src_idx]);

			if(ctrl_get_sts(g_livetv.p_list_cont) == OBJ_STS_HIDE && ctrl_get_sts(g_livetv.p_gp_list_cont) == OBJ_STS_HIDE)
			{
				show_epg_bar_while_change_prog();
			}
			
			ui_livetv_play_net_prog(channel_url.url);
		}
	}

	return SUCCESS;
}

static RET_CODE ui_livetv_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	if(g_livetv.p_url_buf != NULL)
	{
		OS_PRINTF("@@@ui_livetv_on_destory\n");
		mtos_free(g_livetv.p_url_buf);
		g_livetv.p_url_buf = NULL;
	}
	if(g_livetv.cus_url_tbl != NULL)
	{
		mtos_free(g_livetv.cus_url_tbl);
		g_livetv.cus_url_tbl = NULL;
	}

#if ENABLE_IPTV_EPG
	deinit_livetv_one_pg_one_day_epg_param();
#endif
	g_livetv.p_epg = NULL;
	g_livetv.is_loading_epg = FALSE;
	
    ui_video_c_destroy();

	return ERR_NOFEATURE;
}

static RET_CODE on_live_tv_mute(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	if(ctrl_get_sts(g_livetv.p_today_epg_cont) == OBJ_STS_SHOW)
	{
		OS_PRINTF("##epg list is exist, first hide epg list##\n");
		on_live_tv_epg_list_exit(NULL, 0, 0, 0);
	}
	OS_PRINTF("##%s##\n", __FUNCTION__);
	ui_set_mute(!ui_is_mute());

	return SUCCESS;
}


/************************************************
show bps and downloading percent

************************************************/
void ui_livetv_get_bps_infos(u32 param, u16 *percent, u16 *bps)
{
	*percent = param >> 16;  //high 16bit saved percent
	*bps = param & 0xffff;  //low 16 bit saved bps
}

static void clear_show_bps_dlg(BOOL is_destory)
{
	if(ctrl_get_sts(g_livetv.p_bps_cont) == OBJ_STS_SHOW)
	{
		ctrl_set_sts(g_livetv.p_bps_cont, OBJ_STS_HIDE);
		ctrl_paint_ctrl(g_livetv.p_bps_cont_cont, TRUE);
	}
}

static RET_CODE on_live_tv_show_bps(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	u16 net_bps = 0;
	u16 percent = 0;

	ui_livetv_get_bps_infos(para1, &percent, &net_bps);
	OS_PRINTF("##%s, para1 = %d, net_bps = %d, load percent = %d##\n", __FUNCTION__, para1, net_bps, percent);
	ui_comm_dlg_close();
	if(!ui_livetv_check_focus_root())
	{
		clear_show_bps_dlg(FALSE);
		return SUCCESS;
	}

	ctrl_set_sts(g_livetv.p_bps_cont, OBJ_STS_SHOW);
	live_tv_update_bps(g_livetv.p_bps, g_livetv.p_percent, net_bps, percent);
	ctrl_paint_ctrl(g_livetv.p_bps_cont, TRUE);
    
	return SUCCESS;
}

static void ui_livetv_hide_bps_dlg()
{
    OS_PRINTF("########%s###########\n", __FUNCTION__);
    ctrl_set_sts(g_livetv.p_bps_cont, OBJ_STS_HIDE);
    ctrl_paint_ctrl(g_livetv.p_bps_cont_cont, TRUE);
}

static RET_CODE on_live_tv_cacel_bps(control_t *p_cont, u16 msg,
									 u32 para1, u32 para2)
{
    ui_livetv_hide_bps_dlg();
    ui_comm_dlg_close();
    
    return SUCCESS;
}

/************************************************
show time while loading media

************************************************/
static void live_tv_update_loading_media_time(control_t* p_ctrl, u32 second)
{
    u8 *str_text[2] = 
    {
        "Loading time: ",
        "Hard Loading, please wait, Loading time: ",
    };
    u8 asc_str[128] = {0};

    OS_PRINTF("##%s, second = %d##\n", __FUNCTION__, second);
    if(second <= 5)
    {
        sprintf(asc_str, "%s %d %s", str_text[0], (int)second, " s");
    }
    else if(second > 5)
    {
        sprintf(asc_str, "%s %d %s", str_text[1], (int)second, " s");
    }  
    text_set_content_by_ascstr(p_ctrl, asc_str);
}

static void clear_show_loading_media_time_dlg(BOOL is_destory)
{
	if(ctrl_get_sts(g_livetv.p_load_time) == OBJ_STS_SHOW)
	{
		ctrl_set_sts(g_livetv.p_load_time, OBJ_STS_HIDE);
		ctrl_paint_ctrl(g_livetv.p_load_time_cont, TRUE);
	}
}
static RET_CODE on_live_tv_show_loading_media_time(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	ui_comm_dlg_close();
	if(!ui_livetv_check_focus_root())
	{
		clear_show_loading_media_time_dlg(FALSE);
		return SUCCESS;
	}

	if(para1 <= LIVETV_LOAD_MEDIA_MAX_TIME)
	{
		ctrl_set_sts(g_livetv.p_load_time, OBJ_STS_SHOW);
		live_tv_update_loading_media_time(g_livetv.p_load_time, para1);
		ctrl_paint_ctrl(g_livetv.p_load_time, TRUE);
	}
	else
	{
             
		if(g_livetv.is_auto_change == FALSE)// manaully change url
		{
                ui_livetv_hide_loading_time_dlg();
                if(g_livetv.g_curr_pg.url_cnt > 1)
                {
                    live_tv_open_dlg(IDS_IF_CHANGE_URL, 0);
                }
                else
                {
                    live_tv_open_dlg(IDS_IF_CHANGE_CHAL, 0);
                }
		}
		else
		{
			mtos_printk("####%s, auto change url over 8 sec, will change next url or prog###\n", __FUNCTION__);
			auto_change_url_src();
		}
	}
	
	return SUCCESS;
}

static void ui_livetv_hide_loading_time_dlg()
{
    OS_PRINTF("########%s###########\n", __FUNCTION__);
    ctrl_set_sts(g_livetv.p_load_time, OBJ_STS_HIDE);
    ctrl_paint_ctrl(g_livetv.p_load_time_cont, TRUE);
}


static RET_CODE on_live_tv_open_select_group_list(control_t *p_list, u16 msg,
									 u32 para1, u32 para2)
{
    control_t *p_active = NULL;
    u16 *p_uni_str = NULL;
    u8 group_index;
    u16 list_focus;
    
    OS_PRINTF("########%s#####\n", __FUNCTION__);
    if(ctrl_get_sts(g_livetv.p_gp_list_cont) == OBJ_STS_HIDE)
    {
        group_index = get_cur_group_index();
        live_tv_all_list_hide();
        close_epg_bar_while_show_pg_list();

        ctrl_set_sts(g_livetv.p_gp_help, OBJ_STS_SHOW);
        ctrl_set_sts(g_livetv.p_gp_list_cont, OBJ_STS_SHOW);
        
        if(group_index == g_livetv.grp_index.fav_grp)
        {
            list_focus = 0;
        }
        else if(group_index == g_livetv.grp_index.cus_grp)
        {
            list_focus = 1;
        }
        else if(group_index == g_livetv.grp_index.all_grp)
        {
            list_focus = 2;
        }
        else
        {
            list_focus = group_index + 2;
        }
            
        list_set_focus_pos(g_livetv.p_gp_list, list_focus);        
        list_set_update(g_livetv.p_gp_list, live_tv_group_list_update, 0);
        live_tv_group_list_update(g_livetv.p_gp_list, list_get_valid_pos(g_livetv.p_gp_list), LIVE_TV_LIST_ITEM_NUM_ONE_PAGE, 0);
        p_active = ctrl_get_active_ctrl(g_livetv.p_menu);
        ctrl_process_msg(p_active, MSG_LOSTFOCUS, 0, 0);
        ctrl_process_msg(g_livetv.p_gp_list, MSG_GETFOCUS, 0, 0);
        p_uni_str = (u16 *)list_get_field_content(g_livetv.p_gp_list, list_focus, 1);
        text_set_content_by_unistr(g_livetv.p_gp_grp, p_uni_str);
        ctrl_paint_ctrl(g_livetv.p_menu, TRUE);
    }
    ui_livetv_reset_hide_all_list();

    return SUCCESS;
}

static RET_CODE on_live_tv_group_list_exit(control_t *p_list, u16 msg,
									 u32 para1, u32 para2)
{
    OS_PRINTF("########%s#####\n", __FUNCTION__);
    live_tv_all_list_hide();
    return SUCCESS;
}

static RET_CODE on_live_tv_list_select_group(control_t *p_list, u16 msg,
															u32 para1, u32 para2)
{
    u8 group_index;
    u16 list_focus;
    
    OS_PRINTF("########%s#####\n", __FUNCTION__);
    list_focus = list_get_focus_pos(g_livetv.p_gp_list);

    if(list_focus == 0)
    {
        group_index = g_livetv.grp_index.fav_grp;
    }
    else if(list_focus == 1)
    {
        group_index = g_livetv.grp_index.cus_grp;
    }
    else if(list_focus == 2)
    {
        group_index = g_livetv.grp_index.all_grp;
    }
    else
    {
        group_index = list_focus - 2;
    }
        
   cbox_dync_set_focus(g_livetv.p_grp, group_index);  
        
    live_tv_all_list_hide();

    on_live_tv_list_select_to_show_list(TRUE);

    return SUCCESS;
}

static RET_CODE on_live_tv_group_list_change_focus(control_t *p_ctrl, u16 msg,
														u32 para1, u32 para2)
{
    OS_PRINTF("#####%s######\n", __FUNCTION__);
    ui_livetv_reset_hide_all_list();
    list_class_proc(g_livetv.p_gp_list, msg, para1, para2);

    return SUCCESS;
}


/************************************************
All keymap and proc

************************************************/
BEGIN_KEYMAP(live_tv_mainwin_keymap, NULL)
ON_EVENT(V_KEY_INFO, MSG_SHOW_EPG_OR_SEEK_BAR)
ON_EVENT(V_KEY_0, MSG_NUMBER)
ON_EVENT(V_KEY_1, MSG_NUMBER)
ON_EVENT(V_KEY_2, MSG_NUMBER)
ON_EVENT(V_KEY_3, MSG_NUMBER)
ON_EVENT(V_KEY_4, MSG_NUMBER)
ON_EVENT(V_KEY_5, MSG_NUMBER)
ON_EVENT(V_KEY_6, MSG_NUMBER)
ON_EVENT(V_KEY_7, MSG_NUMBER)
ON_EVENT(V_KEY_8, MSG_NUMBER)
ON_EVENT(V_KEY_9, MSG_NUMBER)
ON_EVENT(V_KEY_RECALL, MSG_RECALL)
ON_EVENT(V_KEY_RED, MSG_REDUCE_URL_SRC)
ON_EVENT(V_KEY_GREEN, MSG_ADD_URL_SRC)
ON_EVENT(V_KEY_YELLOW, MSG_SELECT_GROUP)

ON_EVENT(V_KEY_MUTE, MSG_MUTE)
END_KEYMAP(live_tv_mainwin_keymap, NULL)

BEGIN_MSGPROC(live_tv_mainwin_proc, ui_comm_root_proc)
ON_COMMAND(MSG_NUMBER, on_live_tv_show_number_play)
ON_COMMAND(MSG_SHOW_EPG_OR_SEEK_BAR, on_live_tv_start_to_load_curr_pg_epg_or_show_seek_bar)
ON_COMMAND(MSG_HIDE_TODAY_EPG, on_live_tv_epg_list_exit)
ON_COMMAND(MSG_HIDE_EPG, on_live_tv_hide_epg_info_bar)
ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, on_live_tv_load_media_success)
ON_COMMAND(MSG_VIDEO_EVENT_STOP_CFM, on_live_tv_net_play_stop_cfm)
ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT, on_live_tv_net_play_load_media_stop_cfm)
ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR, on_live_tv_load_media_error)
ON_COMMAND(MSG_VIDEO_EVENT_REQUEST_CHANGE_SRC, on_live_tv_request_to_change_url_src)
ON_COMMAND(MSG_VIDEO_EVENT_SET_PATH_FAIL, on_live_tv_load_media_error)
ON_COMMAND(MSG_DESTROY, ui_livetv_on_destory)
ON_COMMAND(MSG_EXIT_ALL, on_live_tv_list_driect_exit)
ON_COMMAND(MSG_RECALL, on_live_tv_recall)
ON_COMMAND(MSG_CACEL_EXIT_WINDOW, on_live_tv_exit_window)
ON_COMMAND(MSG_SHOW_TODAY_EPG, on_live_tv_show_today_epg)
ON_COMMAND(MSG_ADD_URL_SRC, on_live_tv_change_url_src)
ON_COMMAND(MSG_REDUCE_URL_SRC, on_live_tv_change_url_src)
ON_COMMAND(MSG_MUTE, on_live_tv_mute)
ON_COMMAND(MSG_GET_LIVE_TV_ONE_PG_ONE_DAY_EPG, on_live_tv_get_epg_lookback_params)
ON_COMMAND(MSG_GET_LIVE_TV_LOOKBACK_NAMES, on_live_tv_get_epg_lookback_params)
ON_COMMAND(MSG_GET_LIVE_TV_ONE_PG_ONE_DAY_LOOKBACK, on_live_tv_get_epg_lookback_params)
ON_COMMAND(MSG_HIDE_ALL_LIST, on_live_tv_hide_all_list)
ON_COMMAND(MSG_VIDEO_EVENT_EOS, on_live_tv_load_media_error)
ON_COMMAND(MSG_VIDEO_EVENT_UPDATE_BPS, on_live_tv_show_bps)
ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_TIME, on_live_tv_show_loading_media_time)
ON_COMMAND(MSG_VIDEO_EVENT_FINISH_BUFFERING, on_live_tv_cacel_bps)
ON_COMMAND(MSG_VIDEO_EVENT_FINISH_UPDATE_BPS, on_live_tv_cacel_bps)
ON_COMMAND(MSG_SELECT_GROUP, on_live_tv_open_select_group_list)

END_MSGPROC(live_tv_mainwin_proc, ui_comm_root_proc)

BEGIN_KEYMAP(live_tv_list_keymap, NULL)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
ON_EVENT(V_KEY_LEFT,MSG_DECREASE)
ON_EVENT(V_KEY_RIGHT,MSG_INCREASE)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_FAV, MSG_ADD_FAV_PG)
END_KEYMAP(live_tv_list_keymap, NULL)

BEGIN_MSGPROC(live_tv_list_proc, list_class_proc)
ON_COMMAND(MSG_DECREASE, on_live_tv_list_change_group_or_volume)
ON_COMMAND(MSG_INCREASE, on_live_tv_list_change_group_or_volume)
ON_COMMAND(MSG_FOCUS_UP, on_live_tv_list_change_focus_or_program)
ON_COMMAND(MSG_FOCUS_DOWN, on_live_tv_list_change_focus_or_program)
ON_COMMAND(MSG_PAGE_UP, on_live_tv_list_change_focus_or_program)
ON_COMMAND(MSG_PAGE_DOWN, on_live_tv_list_change_focus_or_program)
ON_COMMAND(MSG_EXIT, on_live_tv_list_exit)
ON_COMMAND(MSG_SELECT, on_live_tv_list_select_to_play_or_show_list)
ON_COMMAND(MSG_ADD_FAV_PG, on_live_tv_add_fav_prog)
END_MSGPROC(live_tv_list_proc, list_class_proc)

BEGIN_KEYMAP(live_tv_num_play_cont_keymap, NULL)
ON_EVENT(V_KEY_0, MSG_NUMBER | 0)
ON_EVENT(V_KEY_1, MSG_NUMBER | 1)
ON_EVENT(V_KEY_2, MSG_NUMBER | 2)
ON_EVENT(V_KEY_3, MSG_NUMBER | 3)
ON_EVENT(V_KEY_4, MSG_NUMBER | 4)
ON_EVENT(V_KEY_5, MSG_NUMBER | 5)
ON_EVENT(V_KEY_6, MSG_NUMBER | 6)
ON_EVENT(V_KEY_7, MSG_NUMBER | 7)
ON_EVENT(V_KEY_8, MSG_NUMBER | 8)
ON_EVENT(V_KEY_9, MSG_NUMBER | 9)
ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(live_tv_num_play_cont_keymap, NULL)

BEGIN_MSGPROC(live_tv_num_play_cont_proc, cont_class_proc)
ON_COMMAND(MSG_NUMBER, on_live_tv_input_num)
ON_COMMAND(MSG_SELECT, on_live_tv_num_select)
ON_COMMAND(MSG_EXIT, on_live_tv_exit_num_play)
END_MSGPROC(live_tv_num_play_cont_proc, cont_class_proc)

BEGIN_KEYMAP(live_tv_epg_list_keymap, NULL)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_OK, MSG_EXIT)
ON_EVENT(V_KEY_INFO, MSG_EXIT)
END_KEYMAP(live_tv_epg_list_keymap, NULL)

BEGIN_MSGPROC(live_tv_epg_list_proc, list_class_proc)
ON_COMMAND(MSG_FOCUS_UP, on_live_tv_epg_list_up_down)
ON_COMMAND(MSG_FOCUS_DOWN, on_live_tv_epg_list_up_down)
ON_COMMAND(MSG_PAGE_UP, on_live_tv_epg_list_up_down)
ON_COMMAND(MSG_PAGE_DOWN, on_live_tv_epg_list_up_down)
ON_COMMAND(MSG_EXIT, on_live_tv_epg_list_exit)
END_MSGPROC(live_tv_epg_list_proc, list_class_proc)


BEGIN_KEYMAP(live_tv_group_list_keymap, NULL)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
ON_EVENT(V_KEY_OK, MSG_SELECT)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
END_KEYMAP(live_tv_group_list_keymap, NULL)

BEGIN_MSGPROC(live_tv_group_list_proc, list_class_proc)
ON_COMMAND(MSG_EXIT, on_live_tv_group_list_exit)
ON_COMMAND(MSG_SELECT, on_live_tv_list_select_group)
ON_COMMAND(MSG_FOCUS_UP, on_live_tv_group_list_change_focus)
ON_COMMAND(MSG_FOCUS_DOWN, on_live_tv_group_list_change_focus)
ON_COMMAND(MSG_PAGE_UP, on_live_tv_group_list_change_focus)
ON_COMMAND(MSG_PAGE_DOWN, on_live_tv_group_list_change_focus)

END_MSGPROC(live_tv_group_list_proc, list_class_proc)

