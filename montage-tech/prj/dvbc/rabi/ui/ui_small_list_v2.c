#include "ui_common.h"
#include "ui_small_list_v2.h"
#include "ui_rename.h"
#include "ui_favorite.h"
#include "ui_live_tv_api.h"
#include "ui_video.h"
#include "file_play_api.h"
#include "ui_fun_help.h"
#include "cas_manager.h"

typedef enum
{
  IDC_SLIST_CONT = 1,
  IDC_SLIST_AZ,
  IDC_TITLE_CONT,
  //IDC_SLIST_HELP,
}slist_ctrl_id_t;


typedef enum
{
  MS_NOR = 0, 
  MS_FAV,
  MS_AZS,
}menu_state_t;

typedef enum
{
  FAV_FIRST = 0,
  FAV_SECOND,
}fav_state_t;

#define SLIST_T9_MAX_KEY        10
#define SLIST_T9_MAX_OPT        5
#define SLIST_T9_TMOUT          2000

extern iconv_t g_cd_utf8_to_utf16le;
static u16 slist_t9_curn_key = 0xFFFF;
static u8 slist_t9_curn_bit = 0;

static u8 slist_mode;   
static u16 slist_group;
static BOOL is_tv = FALSE;
static BOOL is_modified = FALSE;
static u8 slist_hkey = V_KEY_OK;
static menu_state_t ms = MS_NOR;
static slist_pg_type_t pg_type = SLIST_PG_TV;
static u8 parent_vid = 0xFF;
static u8 sub_vid = 0xFF;
fav_state_t fav_state = FAV_FIRST;
static u16 g_group_num = 0;
static BOOL b_is_exit = FALSE;

extern BOOL ui_is_pause(void);
extern void ui_set_pause(BOOL is_pause);
static void slist_reset_list(control_t *p_list);
static RET_CODE slist_update(control_t *p_list, u16 start, u16 size, u32 context);
//static void slist_set_list_item_mark(control_t *p_list, u16 pos, u16 pg_id);
//static void slist_set_list_fav_item_mark(control_t *p_list, u16 pos, u16 pg_id);
static void play_in_small_list(u16 focus);
static void play_focus_pg_in_slist(control_t *p_list, play_in_slist_type_t type);
static RET_CODE on_slist_change_type(control_t *p_list, u16 msg, u32 para1, u32 para2);
static RET_CODE on_slist_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2);


u16 small_list_keymap_v2(u16 key);
RET_CODE small_list_proc_v2(control_t *p_list, u16 msg, u32 para1, u32 para2);

u16 slist_az_keymap(u16 key);
RET_CODE slist_az_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u8 slist_create_view(u8 group_type, slist_pg_type_t pg_type, BOOL is_tv, u32 context)
{
	u8 view_id = 0;
	u8 tmp_group_type = GROUP_T_ALL;

	switch(pg_type)
	{
		case SLIST_PG_TV:
			view_id = ui_dbase_create_view(DB_DVBS_ALL_TV , context, NULL);
			tmp_group_type = GROUP_T_ALL;
			break;
		case SLIST_PG_RADIO:
			view_id = ui_dbase_create_view(DB_DVBS_ALL_RADIO , context, NULL);
			tmp_group_type = GROUP_T_ALL;
			break;
		case SLIST_PG_HD:
			view_id = ui_dbase_create_view(DB_DVBS_ALL_TV_HD , context, NULL);
			tmp_group_type = GROUP_T_ALL;
			break;
		case SLIST_PG_FAV:
			view_id = ui_dbase_create_view(DB_DVBS_FAV_ALL , context, NULL);
			tmp_group_type = GROUP_T_FAV;
			break;

		default:
			break;
	}

	ui_dbase_set_pg_view_id(view_id);
	if(tmp_group_type!=slist_group)
	{
//		slist_group = tmp_group_type;
 //   	sys_status_set_curn_group(slist_group);
    }
    sys_status_save();
	return view_id;
}

static RET_CODE on_small_list_exit(void)
{
	u8 view_id;
	u8 group_type = 0;
	u16 pos_in_set = 0;
	u32 context = 0;
	u16 pg_id;
	dvbs_prog_node_t pg;
    #ifdef ENABLE_ADS
	pic_adv_stop();	
    #endif
    view_id = ui_dbase_get_pg_view_id();
	pg_id = sys_status_get_curn_group_curn_prog_id();
	sys_status_get_group_info(sys_status_get_curn_group(), &group_type, &pos_in_set, &context);
	if(db_dvbs_get_count(view_id)>0)
	{
    	db_dvbs_get_pg_by_id(pg_id, &pg);  
    	if((0==pg.video_pid)&&((CURN_MODE_TV == sys_status_get_curn_prog_mode())))
			sys_status_set_curn_prog_mode(CURN_MODE_RADIO);
		else if((0!=pg.video_pid)&&((CURN_MODE_RADIO == sys_status_get_curn_prog_mode())))
			sys_status_set_curn_prog_mode(CURN_MODE_TV);
    }
	else
	{
		if(CURN_MODE_TV == sys_status_get_curn_prog_mode())
		{
			sys_status_set_curn_prog_mode(CURN_MODE_RADIO);
			slist_create_view(GROUP_T_ALL, SLIST_PG_TV, TRUE, context);
		}
		else if(CURN_MODE_RADIO == sys_status_get_curn_prog_mode())
		{
			sys_status_set_curn_prog_mode(CURN_MODE_TV);
			slist_create_view(GROUP_T_ALL, SLIST_PG_RADIO, TRUE, context);
		}
	}
	if(CURN_MODE_TV == sys_status_get_curn_prog_mode())
	{
		ui_dbase_create_view(DB_DVBS_ALL_HIDE_TV, context, NULL);
	}
	else if(CURN_MODE_RADIO == sys_status_get_curn_prog_mode())
	{
		ui_dbase_create_view(DB_DVBS_ALL_HIDE_RADIO, context, NULL);
	}
	ui_dbase_set_pg_view_id(view_id);
    sys_status_set_curn_group(GROUP_T_ALL);
    sys_status_save();
    close_fun_help_later();
	return manage_close_menu(ROOT_ID_SMALL_LIST, 0, 0);
}

control_t *get_small_list_ctrl(void)
{
	control_t *p_ctrl = NULL;
	p_ctrl = fw_find_root_by_id(ROOT_ID_SMALL_LIST);
	if(NULL == p_ctrl)
		return NULL;
	p_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_SLIST_CONT);
	return ctrl_get_child_by_id(p_ctrl, IDC_SMALL_LIST_LIST);
}

static slist_pg_type_t increase_prog_type(void)
{
	pg_type++;
	pg_type %= SLIST_PG_TYPE_CNT;
	return pg_type;
}
static slist_pg_type_t decrease_prog_type(void)
{
	if(SLIST_PG_TV == pg_type)
		pg_type = SLIST_PG_TYPE_CNT - 1;
	else
		pg_type --;
	return pg_type;
}

static void set_slist_prog_type(slist_pg_type_t type)
{
	pg_type = type;
}
slist_pg_type_t get_slist_prog_type(void)
{
	return pg_type;
}

static void slist_set_modify_state(BOOL state)
{
  is_modified = state;
}

static BOOL slist_get_modify_state(void)
{
  return is_modified;
}
#if 0
static void small_list_open_dlg(u16 str_id, u32 time_out)
{
  comm_dlg_data_t dlg_data =
  {
    0,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  };
  dlg_data.content = str_id;
  dlg_data.dlg_tmout = time_out;
  ui_comm_dlg_close();
  if(fw_find_root_by_id(ROOT_ID_POPUP) == NULL)
  {
    ui_comm_dlg_open(&dlg_data);
  }
}
#endif
#if 0
static BOOL small_list_check_network_state(void)
{
  net_conn_stats_t net_stat;
  net_stat = ui_get_net_connect_status();
  return (net_stat.is_eth_insert && net_stat.is_eth_conn)
  	      || (net_stat.is_usb_eth_insert && net_stat.is_usb_eth_conn)  	      
  	      || (net_stat.is_wifi_insert && net_stat.is_wifi_conn)  	      
          || (net_stat.is_3g_insert && net_stat.is_3g_conn)
  	      || (net_stat.is_gprs_conn);
}
#endif
static void slist_do_save_all(void)
{
  u16 view_type;
  u32 group_context;
  u16 i, count;
  u8 view_id;
  u16 curn_group;
  u16 prog_id;
  u8 curn_mode;
  u32 context;
    u16 prog_pos;


  view_id = ui_dbase_get_pg_view_id();
  count = db_dvbs_get_count(view_id);
  //to check if current view is favorit view
  sys_status_get_curn_view_info(&view_type, &group_context);

  if((view_type == DB_DVBS_FAV_HIDE_TV)
    || (view_type == DB_DVBS_FAV_HIDE_RADIO)
    ||(view_type == DB_DVBS_FAV_ALL))
  {
    for(i = 0; i < count; i++)
    {
      if(db_dvbs_get_mark_status(view_id, i, DB_DVBS_DEL_FLAG, 0) == TRUE)
      {
        db_dvbs_change_mark_status(view_id, i, DB_DVBS_DEL_FLAG, 0);
        db_dvbs_change_mark_status(view_id, i, DB_DVBS_FAV_GRP, (u16)group_context);
      }
    }
  }

  for(i = 0; i < count; i++)
  {
    if(db_dvbs_get_mark_status(view_id, i, DB_DVBS_SEL_FLAG, 0) == TRUE)
    {
      db_dvbs_change_mark_status(view_id, i, DB_DVBS_SEL_FLAG, 0);
    }
  }
  //save your modifications.
  db_dvbs_save(ui_dbase_get_pg_view_id());
  count = db_dvbs_get_count(view_id);
  if(0==count)
  {
  	slist_reset_list(get_small_list_ctrl());
	DEBUG(DBG, INFO, "view_id:%d, count:%d, \n", view_id, count);
  	return;
  }

  ui_dbase_set_pg_view_id(ui_dbase_create_view(view_type, group_context, NULL));
  check_play_history();

  sys_status_check_group();

  //recreate current view to remove all flags.
  sys_status_get_curn_view_info(&view_type, &group_context);

  //if curn view is invalid, that means no pg saved now, so we should
  //do nothing about it.
  if(view_type != DB_DVBS_INVALID_VIEW)
  {
    ui_dbase_set_pg_view_id(ui_dbase_create_view(view_type, group_context, NULL));
  }

  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();
//  slist_group = curn_group;

  sys_status_get_curn_prog_in_group(curn_group, curn_mode, &prog_id, &context);

  prog_pos = db_dvbs_get_view_pos_by_id(ui_dbase_get_pg_view_id(), prog_id);

  sys_status_set_curn_group_info(prog_id,prog_pos);
  sys_status_save();
  book_check_node_on_delete();
}
static RET_CODE save_slist_data_from_extend(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	slist_do_save_all();
	return SUCCESS;
}

static void slist_undo_save_all(void)
{
  u16 view_type;
  u32 group_context;

  //undo modification about view
  db_dvbs_undo(ui_dbase_get_pg_view_id());

  //recreate current view to remove all flags.
  sys_status_get_curn_view_info(&view_type, &group_context);
  ui_dbase_set_pg_view_id(
    ui_dbase_create_view(view_type, group_context, NULL));
}


static BOOL slist_save_data(void)
{
  dlg_ret_t dlg_ret;
  u8 org_mode, curn_mode;
  comm_dlg_data_t slist_exit_data = //popup dialog data
  {
    ROOT_ID_INVALID,
    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
    IDS_MSG_ASK_FOR_SAV,
    0,
  };    

  org_mode = sys_status_get_curn_prog_mode();

  if(slist_get_modify_state())
  {
    slist_exit_data.parent_root = ROOT_ID_SMALL_LIST;

    dlg_ret = ui_comm_dlg_open2(&slist_exit_data);

    if(dlg_ret == DLG_RET_YES)
    {
      slist_do_save_all();
    }
    else
    {
      slist_undo_save_all();
    }

    ui_set_frontpanel_by_curn_pg();
    slist_set_modify_state(FALSE);
  }

  ui_enable_chk_pwd(TRUE);

  curn_mode = sys_status_get_curn_prog_mode();

  return (org_mode == curn_mode) ? TRUE : FALSE;
}

static RET_CODE slist_az_update(control_t* ctrl, u16 start,u16 size,
                                u32 context)
{
  u16 i;
  u8 str[2];
  str[1] = '\0';
  
  for (i = 0; i < SLIST_AZ_TOTAL; i++)
  {
    if(i == 0)
    {
      str[0] = '*';
      list_set_field_content_by_ascstr(ctrl, i, 0, str);
    }
    else if(i < 27)
    {
      str[0] = 'A'+ (u8)i - 1;
      list_set_field_content_by_ascstr(ctrl, i, 0, str);
    }
    else
    {
      str[0] = '0' + (u8)i - 27;
      list_set_field_content_by_ascstr(ctrl, i, 0, str); 
    }
  }
  return SUCCESS;
}

static void slist_az_find(control_t *p_az_list, u16 *p_unistr, BOOL is_paint)
{
  control_t *p_cont, *p_list_cont, *p_list = NULL;
  u8 vid = 0;
  u16 total;
  char *string_ask= "*\0";
  
  p_cont = ctrl_get_parent(p_az_list);
  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_SLIST_CONT);  
  p_list = ctrl_get_child_by_id(p_list_cont, IDC_SMALL_LIST_LIST);  
  
  MT_ASSERT(p_unistr != NULL);

  db_dvbs_destroy_view(sub_vid);
  
  if(uni_strlen(p_unistr) != 0)
  {
   if(strcmp(string_ask,(char *)p_unistr))
    {
     db_dvbs_set_no_part_az(TRUE);
     db_dvbs_find(parent_vid, p_unistr, &sub_vid);
     ui_dbase_set_pg_view_id(sub_vid);
     db_dvbs_set_no_part_az(FALSE);
    }
   else
     ui_dbase_set_pg_view_id(parent_vid);
  }

  vid = ui_dbase_get_pg_view_id();
  total = db_dvbs_get_count(vid);

  list_set_count(p_list, total, SMALL_LIST_PAGE);

  if(total > 0)
  {
    list_set_focus_pos(p_list, 0);
    slist_update(p_list, list_get_valid_pos(p_list), SMALL_LIST_PAGE, 0);
  }    

  if(is_paint)
  {
    ctrl_paint_ctrl(p_list, TRUE);
  }
}

static void slist_reset_pg_type(control_t *p_text)
{
  u16 str_status[SLIST_PG_TYPE_CNT] = {IDS_TV, IDS_HD, IDS_RADIO, IDS_FAV};
  u16 uni_str[32] = {0}, focus = 0, uni_str_cat[4] = {0};
  u8 asc_str[4] = {0};
  control_t *p_cont, *p_list_cont, *p_az_list = NULL;

  gui_get_string(str_status[get_slist_prog_type()], uni_str, 31);

  p_list_cont = ctrl_get_parent(p_text);  
  p_cont = ctrl_get_parent(p_list_cont);
  p_az_list = ctrl_get_child_by_id(p_cont, IDC_SLIST_AZ);  

  focus = list_get_focus_pos(p_az_list);

  if(focus != 0)
  {
    asc_str[0] = '+';
    if(focus < 27)
    {
      asc_str[1] = 'A' + focus - 1;
    }
    else
    {
      asc_str[1] = '0' + focus - 27;
    }

    str_asc2uni(asc_str, uni_str_cat);

    uni_strcat(uni_str, uni_str_cat, 31);
  }
  
  text_set_content_by_unistr(p_text, uni_str);
}

static void slist_reset_az(control_t *p_az)
{
  list_set_focus_pos(p_az, 0);

  slist_az_update(p_az, list_get_valid_pos(p_az), SLIST_AZ_PAGE, 0);
}
/*
static void slist_reset_help(control_t *p_mbox)
{
  u16 bmpid_nor[SLIST_HELP_TOL] = 
    { IM_HELP_RED, IM_HELP_YELLOW, IM_HELP_BLUE, 0, IM_HELP_MENU, IM_HELP_OK, IM_HELP_EXIT, 0 };
  u16 strid_nor[SLIST_HELP_TOL] = 
    { IDS_TYPE, IDS_FAVOR, IDS_A_Z, 0, IDS_BACK, IDS_CONFIRM, IDS_CLOSE, 0};
    
  u16 bmpid_fav[SLIST_HELP_TOL] = 
    { IM_HELP_RED, IM_HELP_GREEN, IM_HELP_YELLOW, IM_HELP_BLUE, IM_HELP_MENU, IM_HELP_OK, IM_HELP_EXIT, IM_HELP_INFOR};
  u16 strid_fav[SLIST_HELP_TOL] =
    { 0, 0, 0, 0, IDS_BACK, IDS_CONFIRM, IDS_CLOSE, IDS_SWITCH};

  u16 bmpid_az[SLIST_HELP_TOL] = 
    { IM_HELP_SHORTCUT, 0, 0, 0, IM_HELP_MENU, IM_HELP_OK, IM_HELP_EXIT, 0};
  u16 strid_az[SLIST_HELP_TOL] =
    { IDS_SHORT_CUT, 0, 0, 0, IDS_BACK, IDS_CONFIRM, IDS_CLOSE, 0};
 
    
  u16 uni_str[MAX_FAV_NAME_LEN];
  
  u16 i = 0;
  
  switch(ms)
  {
    case MS_NOR:
      for(i = 0; i < SLIST_HELP_TOL; i++)
      {
        if(strid_nor[i] != 0)
        {
          gui_get_string(strid_nor[i], uni_str, MAX_FAV_NAME_LEN);
        }
        else
        {
          str_asc2uni(" ", uni_str);
        }
        mbox_set_content_by_unistr(p_mbox, i, uni_str);
        mbox_set_content_by_icon(p_mbox, i, bmpid_nor[i], bmpid_nor[i]);
      }
      break;

    case MS_FAV:
      for(i = 0; i < SLIST_HELP_TOL; i++)
      {
        if(i < 4)
        {
          if(fav_state == FAV_FIRST)
          {
            sys_status_get_fav_name((u8)i, uni_str);
          }
          else if(fav_state == FAV_SECOND)
          {
            sys_status_get_fav_name((u8)(i + 4), uni_str);
          }
        }
        else
        {
          if(strid_fav[i] != 0)
          {
            gui_get_string(strid_fav[i], uni_str, MAX_FAV_NAME_LEN);
          }
          else
          {
            str_asc2uni(" ", uni_str);
          }
        }
        mbox_set_content_by_icon(p_mbox, (u16)i, bmpid_fav[i], bmpid_fav[i]);
        mbox_set_content_by_unistr(p_mbox, (u16)i, uni_str);
      }
      break;
      
    case MS_AZS:
      for(i = 0; i < SLIST_HELP_TOL; i++)
      {
        if(strid_az[i] != 0)
        {
          gui_get_string(strid_az[i], uni_str, MAX_FAV_NAME_LEN);
        }
        else
        {
          str_asc2uni(" ", uni_str);
        }
        mbox_set_content_by_icon(p_mbox, (u16)i, bmpid_az[i], bmpid_az[i]);
        mbox_set_content_by_unistr(p_mbox, (u16)i, uni_str);
      }    
    
      break;

    default:
      break;
  }
}
*/
static void slist_reset_list(control_t *p_list)
{
  u8 vid = 0;
  //u16 new_group;
  u16 list_cnt = 0;
  u16 curn_focus = 0;
  u16 curn_pgid = 0;
  control_t *p_ctrl, *p_title_cont;

  curn_pgid = sys_status_get_curn_group_curn_prog_id();

  //new_group = sys_status_get_curn_group();

  vid = ui_dbase_get_pg_view_id();

  list_cnt = db_dvbs_get_count(vid);

  curn_focus = db_dvbs_get_view_pos_by_id(vid, curn_pgid);

  //reset list count & focus.
  list_set_count(p_list, list_cnt, SMALL_LIST_PAGE);

  if((curn_focus == INVALIDID) && (list_cnt))
  {
    curn_focus = 0;
  }

  list_set_focus_pos(p_list, curn_focus);

  if(list_get_item_status(p_list, curn_focus) != LIST_ITEM_SELECTED)
  {
    list_class_proc(p_list, MSG_SELECT, 0, 0);
    slist_update(p_list, list_get_valid_pos(p_list), SMALL_LIST_PAGE, 0);
    ctrl_paint_ctrl(p_list, TRUE);
    play_in_small_list(curn_focus);
  }
  else
  {
    slist_update(p_list, list_get_valid_pos(p_list), SMALL_LIST_PAGE, 0);
    ctrl_paint_ctrl(p_list, TRUE);
  }

  ui_set_frontpanel_by_curn_pg();
  p_title_cont = ctrl_get_child_by_id(ctrl_get_root(p_list), IDC_TITLE_CONT);
  p_ctrl = ctrl_get_child_by_id(p_title_cont, IDC_SMALL_LIST_SAT);
  
  cbox_static_set_focus(p_ctrl, (u16)(get_slist_prog_type()));
  ctrl_paint_ctrl(p_title_cont, TRUE);

}

static RET_CODE slist_camera_cbox_update(control_t *p_cbox, u16 focus, u16 *p_str,
                                      u16 max_length)
{
   gui_get_string(IDS_CAMERA, p_str, 31);
   return SUCCESS;
}



static RET_CODE slist_net_cbox_update(control_t *p_cbox, u16 focus, u16 *p_str,
                                      u16 max_length)
{
   gui_get_string(IDS_IPTV, p_str, 31);
   return SUCCESS;
}

/*
static void slist_set_net_list_item_mark(control_t *p_list, u16 pos)
{
  u16 i, img[4];

  img[0] = 0;
  img[1] = 0;
  img[2] = 0;
  img[3] = 0;

  for(i = 0; i < 4; i++)
  {
    list_set_field_content_by_icon(p_list, pos, (u8)(i + 2), img[i]);
  }
}



static void slist_set_list_item_mark(control_t *p_list, u16 pos, u16 pg_id)
{
  u16 i, img[4];
  u8 view_id = ui_dbase_get_pg_view_id();

  img[0] = ui_dbase_pg_is_fav(view_id, pos) ? IM_TV_FAV : 0;
  img[1] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, 0) ? IM_TV_LOCK : 0;
  img[2] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_SKP, FALSE)? IM_TV_SKIP: 0;
  img[3] = 0;

  for(i = 0; i < 4; i++)
  {
    list_set_field_content_by_icon(p_list, pos, (u8)(i + 2), img[i]);
  }
}

static void slist_set_list_fav_item_mark(control_t *p_list, u16 pos, u16 pg_id)
{
  u16 i, img[4] = {0};
  u8 view_id = ui_dbase_get_pg_view_id();

  switch(ms)
  {
    case MS_FAV:
      if(fav_state == FAV_FIRST)
      {
        img[0] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, 0) ? IM_TV_FAV1 : 0;
        img[1] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, 1) ? IM_TV_FAV2 : 0;
        img[2] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, 2) ? IM_TV_FAV3 : 0;
        img[3] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, 3) ? IM_TV_FAV4 : 0;
      }
      else if(fav_state == FAV_SECOND)
      {
        img[0] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, 4) ? IM_TV_FAV5 : 0;
        img[1] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, 5) ? IM_TV_FAV6 : 0;
        img[2] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, 6) ? IM_TV_FAV7 : 0;
        img[3] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, 7) ? IM_TV_FAV8 : 0;
      }
      break;

    default:
      break;
      
  }

  for(i = 0; i < 4; i++)
  {
    list_set_field_content_by_icon(p_list, pos, (u8)(i + 2), img[i]);
  }
}
*/
static void convert_str_asc2unistr(u8 *asc_str, u16 *uni_str, int uni_str_len)
{
  int src_len = 0;
  char* outbuf = NULL;
  int dest_len = 0;

  src_len = strlen(asc_str) + 1;
  outbuf = (char*)uni_str;
  dest_len = uni_str_len;
  iconv(g_cd_utf8_to_utf16le, (char**)&asc_str, &src_len, &outbuf, &dest_len);
}

static RET_CODE slist_camera_update(control_t *p_list, u16 start, u16 size,
                             u32 context)
{
  u16 uni_str[32] = {0};
  u8 asc_str[32] = {0};
  u16 i = 0, cnt = 0;

  cnt = list_get_count(p_list);
  for(i = 0; i < size; i++)
  {
    if(i + start < cnt)
    {
      memset(asc_str, 0, sizeof(asc_str));
      sprintf(asc_str, "%.4d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);

      memset(uni_str, 0, sizeof(uni_str));
      memset(asc_str, 0, sizeof(asc_str));
      sprintf(asc_str, "%s %d", "Channel", start + i);

      convert_str_asc2unistr(asc_str, uni_str, sizeof(uni_str));
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, uni_str); //pg name

//      slist_set_net_list_item_mark(p_list, (u16)(start + i));
    }
  }
  return SUCCESS;
}

#if 0
static RET_CODE slist_net_update(control_t *p_list, u16 start, u16 size,
                             u32 context)
{
  u16 uni_str[32] = {0};
  u8 asc_str[8] = {0};
  u16 i = 0, cnt = 0;
  iptv_channel_info_t p_pg = {0};

  cnt = list_get_count(p_list);
  for(i = 0; i < size; i++)
  {
    if(i + start < cnt)
    {
      memset(asc_str, 0, sizeof(asc_str));
      sprintf(asc_str, "%.4d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);

      memset(uni_str, 0, sizeof(uni_str));
      iptv_get_pg_by_pos(&p_pg, (u16)(start + i));
      convert_str_asc2unistr(p_pg.channel_name, uni_str, sizeof(p_pg.channel_name));
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, uni_str); //pg name

      slist_set_net_list_item_mark(p_list, (u16)(start + i));
        
    }
  }
  return SUCCESS;
}
#endif
/*
static RET_CODE slist_update_net_help_bar(control_t *p_list)
{
  control_t *p_list_cont = NULL;
  control_t *p_cont = NULL;
  control_t *p_help = NULL;
  
  p_list_cont = ctrl_get_parent(p_list);
  p_cont = ctrl_get_parent(p_list_cont);
  p_help = ctrl_get_child_by_id(p_cont, IDC_SLIST_HELP);

  mbox_set_content_by_ascstr(p_help, 0, "Prev Source");
  mbox_set_content_by_icon(p_help,0, IM_HELP_RED, IM_HELP_RED);

  mbox_set_content_by_ascstr(p_help, 1, "Next Source");
  mbox_set_content_by_icon(p_help,1, IM_HELP_GREEN, IM_HELP_GREEN);

  mbox_set_content_by_ascstr(p_help, 2, "");
  mbox_set_content_by_icon(p_help,2, 0, 0);

  return SUCCESS;
}
*//*
static RET_CODE slist_update_camera_help_bar(control_t *p_list)
{
  control_t *p_list_cont = NULL;
  control_t *p_cont = NULL;
  control_t *p_help = NULL;
  
  p_list_cont = ctrl_get_parent(p_list);
  p_cont = ctrl_get_parent(p_list_cont);
  p_help = ctrl_get_child_by_id(p_cont, IDC_SLIST_HELP);

  mbox_set_content_by_ascstr(p_help, 0, "");
  mbox_set_content_by_icon(p_help,0, 0, 0);

  mbox_set_content_by_ascstr(p_help, 1, "");
  mbox_set_content_by_icon(p_help,1, 0, 0);

  mbox_set_content_by_ascstr(p_help, 2, "");
  mbox_set_content_by_icon(p_help,2, 0, 0);

  return SUCCESS;
}
*/
#if 0
static void slist_reset_net_source_type(control_t *p_text)
{
  u8 asc_str[16]={0};
  u16 cur_prg_source_index = 0;
  iptv_url_msg_info_t p_cur_channel_info = {0};
  
  cur_prg_source_index = ui_iptv_get_cur_prg_source_index(ui_iptv_get_cur_prg_pos());
  ui_iptv_get_cur_channel_info(&p_cur_channel_info);
  if(p_cur_channel_info.url_count <= 0)
    p_cur_channel_info.url_count = 1;

  sprintf(asc_str, "%s: %d/%d", "Source", cur_prg_source_index + 1, p_cur_channel_info.url_count);
  text_set_content_by_ascstr(p_text, asc_str);
}
#endif
#if 0
static RET_CODE slist_camera_play(u16 cur_prg_pos)
{
    u8 str[64] = "C:/hbjy.flv";
    u16 p_path[32] = {0};
    
    convert_str_asc2unistr(str, p_path, sizeof(p_path));
    //ui_video_c_load_media_from_local(p_path, 0);
    ui_video_c_play_by_file(p_path, 0);

    return SUCCESS;
}
#endif

//------------------ iptv 
#if 0
static RET_CODE on_list_iptv_get_url_success(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  iptv_url_msg_info_t *p_url_info = (iptv_url_msg_info_t *)para1;
  u16 cur_prg_source_index = 0;
  control_t *p_list_cont = NULL;
  control_t *p_type = NULL;
  control_t *p_cbox;
  u16 group_pos = 0;

  p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_SMALL_LIST_SAT);
  group_pos = cbox_dync_get_focus(p_cbox);
  p_list_cont = ctrl_get_parent(p_list);
  p_type = ctrl_get_child_by_id(p_list_cont, IDC_SMALL_LIST_STATUS); 

  if(p_url_info == NULL)
    return ERR_FAILURE;

  cur_prg_source_index = ui_iptv_get_cur_prg_source_index(ui_iptv_get_cur_prg_pos());
  if(p_url_info->p_url[cur_prg_source_index] == NULL)
    return SUCCESS;

  if(group_pos == g_group_num - 2)
  {
  slist_reset_net_source_type(p_type);
  ctrl_paint_ctrl(p_type, TRUE);
  }
  ui_video_c_play_by_url(p_url_info->p_url[cur_prg_source_index], 0, VDO_PLAY_MODE_LIVE);
  return SUCCESS;
}

static RET_CODE on_list_play_iptv_after_exit_msg(control_t *p_list, u16 start, u16 size,
                             u32 context)
{
  u16 cur_prg_pos = 0;
  iptv_channel_info_t p_pg = {0};
  control_t *p_cbox;
  u16 group_pos = 0;

  p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_SMALL_LIST_SAT);
  group_pos = cbox_dync_get_focus(p_cbox);
  cur_prg_pos = list_get_focus_pos(p_list);
   if(group_pos != g_group_num - 2)
   {
     ui_comm_dlg_close();
   }
  
  if(ui_get_play_prg_type() == NET_PRG_TYPE)
  {
    if(small_list_check_network_state() == FALSE)
       return SUCCESS;

    iptv_get_pg_by_pos(&p_pg, ui_iptv_get_cur_prg_pos());
    ui_iptv_request_url_info(p_pg.channel_id);
  }
  
   if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
  {
    slist_camera_play(cur_prg_pos);
  }
  return SUCCESS;
}

static RET_CODE on_list_net_media_exit_msg(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  if(b_is_exit == TRUE)
  {
    b_is_exit = FALSE;
    return SUCCESS;
  }
  on_list_play_iptv_after_exit_msg(p_list, msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_list_net_load_media_success(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  if(small_list_check_network_state() == FALSE)
       return SUCCESS;
  
  ui_comm_dlg_close();

  return SUCCESS;
}

static RET_CODE on_list_net_play_next_prg(control_t *p_list)
{
  control_t *p_list_cont = NULL;
  control_t *p_cbox = NULL;
  u16 prg_id = 0;
  u16 group_pos = 0;
  p_list_cont = ctrl_get_parent(p_list);
  
  p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_SMALL_LIST_SAT);
  group_pos = cbox_dync_get_focus(p_cbox);
  prg_id = ui_iptv_get_cur_prg_pos();
  prg_id ++;
  if(ui_iptv_get_channel_count() <= 0)
    return SUCCESS;
  if(prg_id >= ui_iptv_get_channel_count())
    prg_id = 0;

  ui_iptv_set_prev_prg_pos(ui_iptv_get_cur_prg_pos());
  ui_iptv_set_cur_prg_pos(prg_id);
  if(group_pos == g_group_num - 2)
  {
  list_set_focus_pos(p_list, prg_id);
  list_select_item(p_list, prg_id);
  ctrl_paint_ctrl(p_list, TRUE);
  }
  if(ui_video_c_get_play_state() == MUL_PLAY_STATE_NONE)
  {
      on_list_play_iptv_after_exit_msg(p_list, 0, 0, 0);
  }else
  {
      b_is_exit = FALSE;
      ui_video_c_stop();
  }
  return SUCCESS;
}

static RET_CODE on_list_net_load_media_error(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  iptv_url_msg_info_t p_cur_channel_info = {0};
  u8 cur_prg_source_index = 0;
  control_t *p_list_cont = NULL;
  control_t *p_type = NULL;
  control_t *p_cbox;
  u16 group_pos = 0;
  p_list_cont = ctrl_get_parent(p_list);
  p_type = ctrl_get_child_by_id(p_list_cont, IDC_SMALL_LIST_STATUS); 
  p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_SMALL_LIST_SAT);
  group_pos = cbox_dync_get_focus(p_cbox);
  
  ui_iptv_get_cur_channel_info(&p_cur_channel_info);
  cur_prg_source_index = ui_iptv_get_cur_prg_source_index(ui_iptv_get_cur_prg_pos());
  cur_prg_source_index ++;
  if(cur_prg_source_index >= p_cur_channel_info.url_count)
  {
    cur_prg_source_index = 0;
    ui_iptv_set_cur_prg_source_index(ui_iptv_get_cur_prg_pos(), cur_prg_source_index);
    if(group_pos == g_group_num - 2)
    small_list_open_dlg(IDS_PLAY_NEXT_NETWORK_PRG, 0);
    on_list_net_play_next_prg(p_list);
    return SUCCESS;
  }

  ui_iptv_set_cur_prg_source_index(ui_iptv_get_cur_prg_pos(), cur_prg_source_index);

  if(p_cur_channel_info.p_url[cur_prg_source_index] != NULL)
  {
   if(ui_video_c_get_play_state() == MUL_PLAY_STATE_NONE)
    {
        on_list_play_iptv_after_exit_msg(p_list, 0, 0, 0);
    }else
    {
        b_is_exit = FALSE;
        ui_video_c_stop();
    }
    if(group_pos == g_group_num - 2)
    {
    small_list_open_dlg(IDS_SWITCH_MEDIA_SOURCE, 0);
       slist_reset_net_source_type(p_type);
       ctrl_paint_ctrl(p_type, TRUE);
    }
    return SUCCESS;
  }
  if(group_pos == g_group_num - 2)
  small_list_open_dlg(IDS_LOAD_MEDIA_ERROR, 0);
  return SUCCESS;
}


static RET_CODE on_net_manual_switch_source(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  iptv_url_msg_info_t p_cur_channel_info = {0};
  s8 cur_prg_source_index = 0;
  control_t *p_list_cont = NULL;
  control_t *p_type = NULL;
  control_t *p_cbox;
  u16 group_pos = 0;
  
  p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_SMALL_LIST_SAT);
  group_pos = cbox_dync_get_focus(p_cbox);
  if(ui_get_play_prg_type() != NET_PRG_TYPE)
    return SUCCESS;

  p_list_cont = ctrl_get_parent(p_list);
  p_type = ctrl_get_child_by_id(p_list_cont, IDC_SMALL_LIST_STATUS); 
  
  ui_iptv_get_cur_channel_info(&p_cur_channel_info);
  cur_prg_source_index = ui_iptv_get_cur_prg_source_index(ui_iptv_get_cur_prg_pos());

  if(msg == MSG_CHANGE_LIST)
    cur_prg_source_index --;
  else if(msg == MSG_NEWS)
    cur_prg_source_index ++;

  if(cur_prg_source_index < 0)
    cur_prg_source_index = p_cur_channel_info.url_count - 1;
  if(cur_prg_source_index >= p_cur_channel_info.url_count)
    cur_prg_source_index = 0;

  ui_iptv_set_cur_prg_source_index(ui_iptv_get_cur_prg_pos(), cur_prg_source_index);

  if(p_cur_channel_info.p_url[cur_prg_source_index] != NULL)
  {
    if(ui_video_c_get_play_state() == MUL_PLAY_STATE_NONE)
    {
        on_list_play_iptv_after_exit_msg(p_list, 0, 0, 0);
    }else
    {
        b_is_exit = FALSE;
        ui_video_c_stop();
    }
    if(group_pos == g_group_num - 2)
    {
    small_list_open_dlg(IDS_SWITCH_MEDIA_SOURCE, 0);
      slist_reset_net_source_type(p_type);
      ctrl_paint_ctrl(p_type, TRUE);
    }
    return SUCCESS;
  }
  return SUCCESS;
}

#endif
//load data from database, will be modified later
static RET_CODE slist_update(control_t *p_list, u16 start, u16 size,
                             u32 context)
{
  dvbs_prog_node_t curn_prog;
  u16 i;
  u16 uni_str[32];
  u8 asc_str[8];
  u16 pg_id, cnt = list_get_count(p_list);

  for(i = 0; i < size; i++)
  {
    if(i + start < cnt)
    {
      pg_id = db_dvbs_get_id_by_view_pos(
        ui_dbase_get_pg_view_id(),
        (u16)(i + start));
      db_dvbs_get_pg_by_id(pg_id, &curn_prog);

      // NO.
      sprintf(asc_str, "%.4d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0,
                                       asc_str);  //program number in list

      // NAME
      ui_dbase_get_full_prog_name(&curn_prog, uni_str, 31);
      list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, uni_str); //pg name
/*
      //set marks
      if(MS_FAV == ms)
      {
        slist_set_list_fav_item_mark(p_list, (u16)(start + i), pg_id);
      }
      else
      {
        slist_set_list_item_mark(p_list, (u16)(start + i), pg_id);
      }
*/
      //set selected state.
      if(context == 0xFEFE)
      {
        if(pg_id == sys_status_get_curn_group_curn_prog_id())
        {
          list_select_item(p_list, start + i);
          ui_set_front_panel_by_num(start + i+1);
        }
      }
    }
  }
  return SUCCESS;
}

static void play_in_small_list(u16 focus)
{
  u16 rid;

  if((rid =
        db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(),
                                   focus)) != INVALIDID)
  {
    if(ui_is_playing() == FALSE)
    {
	ui_auto_switch_mode_stop_play(TRUE);
    }
    ui_play_prog(rid, FALSE);
  }
}

static void slist_update_group_name(control_t *p_cbox)
{
  u16 all_num = 0;
  u16 cbox_focus = 0xFFFF;
  switch(slist_hkey)
  {
    case V_KEY_OK:
      all_num = sys_status_get_all_group_num() + 2;
      cbox_focus = sys_status_get_pos_by_group(slist_group);
      break;
    case V_KEY_SAT:
      all_num = sys_status_get_sat_group_num() + 3;
      if(slist_group == 0)
      {
        cbox_focus = 0;
      }
      else
      {
        cbox_focus = sys_status_get_sat_group_pos(slist_group);
      }
      break;
    case V_KEY_FAV:
      all_num = sys_status_get_fav_group_num() + 3;
      if(slist_group == 0)
      {
        cbox_focus = 0;
      }
      else
      {
        cbox_focus = sys_status_get_fav_group_pos(slist_group);
      }
      break;
    default:
      all_num = sys_status_get_all_group_num() + 2;
      cbox_focus = sys_status_get_pos_by_group(slist_group);
      break;
  }

  g_group_num = all_num;
  cbox_dync_set_count(p_cbox, all_num);
  cbox_dync_set_focus(p_cbox, cbox_focus);
}
static void play_focus_pg_in_slist(control_t *p_list, play_in_slist_type_t type)
{
	u16 cur_prg_pos = 0;
	u8 view_id;
	u16 pg_id;
	u16 view_pg_pos;

	pg_id = sys_status_get_curn_group_curn_prog_id();
	view_id = ui_dbase_get_pg_view_id();
	view_pg_pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);
	cur_prg_pos = list_get_focus_pos(p_list);
	if(view_pg_pos==cur_prg_pos)
	{
		switch(type)
		{
		case PLAY_IN_SLIST_EXIT:
			on_small_list_exit();
			return;
			break;
		case PLAY_IN_SLIST_NOTHING:
			return;
			break;
		case PLAY_IN_SLIST_FORCE:
			break;
		}
	}
	ui_auto_switch_mode_stop_play(TRUE);
	play_in_small_list(cur_prg_pos);
	list_select_item(p_list, cur_prg_pos);
	ctrl_paint_ctrl(p_list, TRUE);

	ui_play_timer_start();
#ifdef ENABLE_CA
	cas_manage_finger_repaint();
	OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
#endif
}
RET_CODE play_in_slist_from_extend(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	play_focus_pg_in_slist(p_list, (play_in_slist_type_t)para1);
	return SUCCESS;
}

static RET_CODE slist_update_from_extend(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	u16 pos;
	u8 view_id;
	u16 total;
	if((MSG_DEL_PG_FROM_VIEW==para1)||
		((MSG_REMOVE_FROM_FAV==para1)&&(SLIST_PG_FAV==get_slist_prog_type())))
	{
		pos = list_get_focus_pos(p_list);
		view_id = ui_dbase_get_pg_view_id();
		total = db_dvbs_get_count(view_id);
		list_set_count(p_list, total, SMALL_LIST_PAGE);
		if(pos>0)
			pos -= 1;
		else
			pos = 0;
		list_set_focus_pos(p_list, pos);

		#ifdef SHUSHITONG_CA
		if((CUSTOMER_ID == CUSTOMER_DTMB_CHANGSHA_HHT)&&(MSG_DEL_PG_FROM_VIEW==para1))
		{
			update_have_valid_freq_flag();
		}
		#endif
	}

    slist_update(p_list, list_get_valid_pos(p_list), SMALL_LIST_PAGE, 0);
	play_focus_pg_in_slist(p_list, PLAY_IN_SLIST_FORCE);
    ctrl_paint_ctrl(p_list, TRUE);
    return SUCCESS;
}

RET_CODE on_slist_select_play_process(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    control_t *p_cbox, *p_ctrl;
    control_t *p_list_cont;
    static play_prg_type_t prev_play_prg_type = SAT_PRG_TYPE;
  
   p_ctrl = ctrl_get_child_by_id(ctrl_get_root(p_list), IDC_TITLE_CONT);
   p_cbox = ctrl_get_child_by_id(p_ctrl, IDC_SMALL_LIST_SAT);
   p_list_cont = ctrl_get_parent(p_list);
   ui_comm_dlg_close();
   
	prev_play_prg_type = ui_get_play_prg_type();
	if(prev_play_prg_type != SAT_PRG_TYPE)
	{
	  b_is_exit = TRUE;
	  ui_video_c_stop();
	  ui_video_c_destroy();
   
	  ui_set_play_prg_type(SAT_PRG_TYPE);
	}

	if(ui_is_pause())
	{
		ui_set_pause(FALSE);
	}
	play_focus_pg_in_slist(p_list, PLAY_IN_SLIST_EXIT);

  return SUCCESS;
}

RET_CODE on_slist_state_process(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_list_cont, *p_cont, *p_az_list, *p_cbox, *p_ctrl;
  control_t *p_sort_status;
  u16 focus, total;
  u16 org_group = 0;
  u16 *p_unistr = NULL;
  u8 view_id;
  u16 param = 0;
  u8 group_type = 0;
  u16 pos_in_set = 0;
  u32 context = 0;
  menu_state_t old_ms = MS_NOR;
  slist_pg_type_t old_type = SLIST_PG_TV;
  fav_state_t old_fav_state = FAV_FIRST;

  view_id = ui_dbase_get_pg_view_id();
  total = db_dvbs_get_count(view_id);
  focus = list_get_focus_pos(p_list);
  p_list_cont = ctrl_get_parent(p_list);
  p_cont = ctrl_get_parent(p_list_cont);
  p_sort_status = ctrl_get_child_by_id(p_list_cont, IDC_SMALL_LIST_STATUS);
//  p_help = ctrl_get_child_by_id(p_cont, IDC_SLIST_HELP);
  p_az_list = ctrl_get_child_by_id(p_cont, IDC_SLIST_AZ);
  
  is_tv = ((CURN_MODE_TV == sys_status_get_curn_prog_mode())?TRUE:FALSE);

  old_ms = ms;
  old_type = get_slist_prog_type();
  old_fav_state = fav_state;

  switch(msg)
  {
    case MSG_EXIT:
    case MSG_EXIT_ALL:
      if(ui_get_play_prg_type() == NET_PRG_TYPE)
      {
        if(fw_get_first_active_id() == ROOT_ID_MAINMENU)
        {
          b_is_exit = TRUE;
          ui_video_c_stop();
          ui_video_c_destroy();
        }
        ui_comm_dlg_close();
		on_small_list_exit();
        return SUCCESS;
      }
      switch(old_ms)
      {
        case MS_NOR:
          slist_save_data();
          
          if(db_dvbs_get_count(view_id) == 0)
          {
            set_slist_prog_type(SLIST_PG_TV);
            sys_status_get_group_info(slist_group, &group_type, &pos_in_set, &context);
            view_id = slist_create_view(group_type, get_slist_prog_type(), is_tv, context);
//            ui_dbase_set_pg_view_id(view_id);
			if (db_dvbs_get_count(view_id) == 0)
			{
			  view_id = slist_create_view(group_type, SLIST_PG_TV, is_tv, context);
			  on_small_list_exit();
			  return SUCCESS;
			}
			//ui_dbase_set_pg_view_id(view_id);            
          }
          else
          {
            sys_status_get_group_info(slist_group, &group_type, &pos_in_set, &context);

            view_id = slist_create_view(group_type, SLIST_PG_TV, is_tv, context);

//            ui_dbase_set_pg_view_id(view_id);
            ui_comm_dlg_close();
			on_small_list_exit();
          }
          break;

        case MS_FAV:
          org_group = sys_status_get_curn_group();

          slist_save_data();
          
          sys_status_get_group_info(slist_group, &group_type, &pos_in_set, &context);
          view_id = slist_create_view(group_type, get_slist_prog_type(), is_tv, context);
//          ui_dbase_set_pg_view_id(view_id);

          if (slist_group != org_group)
          {
			p_ctrl = ctrl_get_child_by_id(ctrl_get_root(p_list), IDC_TITLE_CONT);
			p_cbox = ctrl_get_child_by_id(p_ctrl, IDC_SMALL_LIST_SAT);
            slist_update_group_name(p_cbox);
            ctrl_paint_ctrl(p_cbox, TRUE);
          }

          slist_reset_az(p_az_list);
          slist_reset_pg_type(p_sort_status);
          ctrl_paint_ctrl(p_sort_status, TRUE);

          ms = MS_NOR;
          break;

        case MS_AZS:
          slist_save_data();
          
          sys_status_get_group_info(slist_group, &group_type, &pos_in_set, &context);

          view_id = slist_create_view(group_type, get_slist_prog_type(), is_tv, context);
          
//          ui_dbase_set_pg_view_id(view_id);

          ctrl_process_msg(p_az_list, MSG_LOSTFOCUS, 0, 0);
          
          ctrl_set_sts(p_az_list, OBJ_STS_HIDE);
		  ctrl_set_sts(p_sort_status, OBJ_STS_HIDE);
          ctrl_erase_ctrl(p_az_list);
          
          ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);

          slist_reset_az(p_az_list);
          slist_reset_pg_type(p_sort_status);
          ctrl_paint_ctrl(p_sort_status, TRUE);
    
          ms = MS_NOR;
          break;          

        default:
          break;
      }
      break;

    case MSG_SELECT:
      switch(old_ms)
      {
        case MS_AZS:
          //play_in_small_list(0);
         
          ctrl_process_msg(p_az_list, MSG_LOSTFOCUS, 0, 0);
          
          ctrl_set_sts(p_az_list, OBJ_STS_HIDE);
		  ctrl_set_sts(p_sort_status, OBJ_STS_HIDE);
          ctrl_erase_ctrl(p_az_list);
          
          ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);

          ms = MS_NOR;
          break; 
          
        case MS_FAV:
        case MS_NOR:
          slist_save_data();
          
          sys_status_get_group_info(slist_group, &group_type, &pos_in_set, &context);

          view_id = slist_create_view(group_type, SLIST_PG_TV, is_tv, context);
          
//          ui_dbase_set_pg_view_id(view_id);
          
          ui_close_all_mennus();          
          break;
          
        default:
          break;
      }    
      break;

    case MSG_GROUP_UP:
    case MSG_GROUP_DOWN:
      slist_save_data();
      
	  set_slist_prog_type(SLIST_PG_TV);
      slist_reset_az(p_az_list);
      
      ms = MS_NOR;
      break;

    case MSG_SMALL_LIST_BULE:
      if(ui_get_play_prg_type() == NET_PRG_TYPE)
        return SUCCESS;
      if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
        return SUCCESS;
      switch(old_ms)
      {
        case MS_NOR:
          sys_status_get_group_info(slist_group, &group_type, &pos_in_set, &context);

          view_id = slist_create_view(group_type, get_slist_prog_type(), is_tv, context);
          
          parent_vid = view_id;
          
      //    sub_vid = 0xFF;

          ctrl_process_msg(p_list, MSG_LOSTFOCUS, 0, 0);
          
          ctrl_set_sts(p_az_list, OBJ_STS_SHOW);
		  ctrl_set_sts(p_sort_status, OBJ_STS_SHOW);
          ctrl_process_msg(p_az_list, MSG_GETFOCUS, 0, 0);

          focus = list_get_focus_pos(p_az_list);
          p_unistr = (u16 *)list_get_field_content(p_az_list, focus, 0);
          MT_ASSERT(p_unistr != NULL);
          
          slist_az_find(p_az_list, p_unistr, TRUE);

          ctrl_paint_ctrl(p_az_list, TRUE);

//          slist_reset_help(p_help);
 //         ctrl_paint_ctrl(p_help, TRUE);
          
          ms = MS_AZS;
          
          return SUCCESS;

        case MS_FAV:
          if(ui_get_play_prg_type() == NET_PRG_TYPE)
            return SUCCESS;
          if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
            return SUCCESS;
          if(focus == INVALIDID)
          {
            return ERR_FAILURE;//fix bug 28233
          }
          if(fav_state == FAV_FIRST)
          {
            if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_FAV_GRP, 3) == TRUE)
            {
              param = (3 & (~DB_DVBS_PARAM_ACTIVE_FLAG));
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 5, 0);
            }
            else
            {
              param = (3 | DB_DVBS_PARAM_ACTIVE_FLAG);
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 5, IM_TV_FAV4);
            }
          }
          else if(fav_state == FAV_SECOND)
          {
            if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_FAV_GRP, 7) == TRUE)
            {
              param = (7 & (~DB_DVBS_PARAM_ACTIVE_FLAG));
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 5, 0);
            }
            else
            {
              param = (7 | DB_DVBS_PARAM_ACTIVE_FLAG);
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 5, IM_TV_FAV8);
            }
          }
          list_draw_field_ext(p_list, focus, 5, TRUE);
          slist_set_modify_state(TRUE);
          break;
          
        default:
          break;
      }
      break;      

    case MSG_CHANGE_LIST:
      if(ui_get_play_prg_type() == NET_PRG_TYPE)
      {
//        on_net_manual_switch_source(p_list, msg, para1, para2);
        return SUCCESS;
      }
      if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
        return SUCCESS;
      
      switch(old_ms)
      {
        case MS_NOR:
		  increase_prog_type();
  //        if(SLIST_PG_FAV == get_slist_prog_type())
//          	slist_group = 1 + DB_DVBS_MAX_SAT;
    //      else
			  slist_group = sys_status_get_curn_group();
          sys_status_get_group_info(slist_group, &group_type, &pos_in_set, &context);

          view_id = slist_create_view(group_type, get_slist_prog_type(), is_tv, context);

//          ui_dbase_set_pg_view_id(view_id);

          slist_reset_az(p_az_list);
          break;

        case MS_FAV:
          if(focus == INVALIDID)
          {
            return ERR_FAILURE;//fix bug 28233
          }
          if(fav_state == FAV_FIRST)
          {
            if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_FAV_GRP, 0) == TRUE)
            {
              param = (0 & (~DB_DVBS_PARAM_ACTIVE_FLAG));
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 2, 0);
            }
            else
            {
              param = (0 | DB_DVBS_PARAM_ACTIVE_FLAG);
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 2, IM_TV_FAV1);
            }
          }
          else if(fav_state == FAV_SECOND)
          {
            if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_FAV_GRP, 4) == TRUE)
            {
              param = (4 & (~DB_DVBS_PARAM_ACTIVE_FLAG));
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 2, 0);
            }
            else
            {
              param = (4 | DB_DVBS_PARAM_ACTIVE_FLAG);
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 2, IM_TV_FAV5);
            }
          }
          
          list_draw_field_ext(p_list, focus, 2, TRUE);
          
          slist_set_modify_state(TRUE);
          break;

       default:
          break;
      }
      break;

    case MSG_NEWS:
      if(ui_get_play_prg_type() == NET_PRG_TYPE)
      {
//         on_net_manual_switch_source(p_list, msg, para1, para2);
        return SUCCESS;
      }
      if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
        return SUCCESS;
      
      switch(old_ms)
      {
        case MS_FAV:
          if(focus == INVALIDID)
          {
            return ERR_FAILURE;//fix bug 28233
          }
          if(fav_state == FAV_FIRST)
          {
            if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_FAV_GRP, 1) == TRUE)
            {
              param = (1 & (~DB_DVBS_PARAM_ACTIVE_FLAG));
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 3, 0);
            }
            else
            {
              param = (1 | DB_DVBS_PARAM_ACTIVE_FLAG);
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 3, IM_TV_FAV2);
            }
          }
          else if(fav_state == FAV_SECOND)     
          {
            if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_FAV_GRP, 5) == TRUE)
            {
              param = (5 & (~DB_DVBS_PARAM_ACTIVE_FLAG));
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 3, 0);
            }
            else
            {
              param = (5 | DB_DVBS_PARAM_ACTIVE_FLAG);
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 3, IM_TV_FAV6);
            }
          }
          list_draw_field_ext(p_list, focus, 3, TRUE);
          
          slist_set_modify_state(TRUE);
          break;
         
        default:
          break;
      }
      break;

    case MSG_CHANGE_TO_FAV:
      if(ui_get_play_prg_type() == NET_PRG_TYPE)
        return SUCCESS;
      if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
        return SUCCESS;
      switch(old_ms)
      {
        case MS_NOR:

          ms = MS_FAV;
          break;

        case MS_FAV:
          if(focus == INVALIDID)
          {
            return ERR_FAILURE;//fix bug 28233
          }
          if(fav_state == FAV_FIRST)
          {
            if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_FAV_GRP, 2) == TRUE)
            {
              param = (2 & (~DB_DVBS_PARAM_ACTIVE_FLAG));
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 4, 0);
            }
            else
            {
              param = (2 | DB_DVBS_PARAM_ACTIVE_FLAG);
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 4, IM_TV_FAV3);
            }
          }
          else if(fav_state == FAV_SECOND)
          {
            if(db_dvbs_get_mark_status(view_id, focus, DB_DVBS_FAV_GRP, 6) == TRUE)
            {
              param = (6 & (~DB_DVBS_PARAM_ACTIVE_FLAG));
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 4, 0);
            }
            else
            {
              param = (6 | DB_DVBS_PARAM_ACTIVE_FLAG);
              db_dvbs_change_mark_status(view_id, focus, DB_DVBS_FAV_GRP, param);
              list_set_field_content_by_icon(p_list, focus, 4, IM_TV_FAV7);
            }
          }
          
          list_draw_field_ext(p_list, focus, 4, TRUE);
          
          slist_set_modify_state(TRUE);
          break;
          
        default:
          break;
      }
      break;

    case MSG_SMALL_LIST_INFO:
      if(ui_get_play_prg_type() == NET_PRG_TYPE)
        return SUCCESS;
      if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
        return SUCCESS;
      switch(old_ms)
      {
        case MS_FAV:
          if(fav_state == FAV_FIRST)
          {
            fav_state = FAV_SECOND;
          }
          else if(fav_state == FAV_SECOND)
          {
            fav_state = FAV_FIRST;
          }
          break;
          
        default:
          break;
      }    
      break;

    default:
      break;
  }

  //update type status.
  if(old_type != get_slist_prog_type())
  {
    slist_reset_pg_type(p_sort_status);
    ctrl_paint_ctrl(p_sort_status, TRUE);
  }

  //update list.
  if((old_ms != ms) || (old_type != get_slist_prog_type()) || (old_fav_state != fav_state))
  {
    slist_reset_list(p_list);
  }
/*
  //update help.
  if((old_ms != ms) || (old_fav_state != fav_state))
  {
    slist_reset_help(p_help);
    ctrl_paint_ctrl(p_help, TRUE);
  }
*/
  return SUCCESS;
}

static RET_CODE on_slist_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret;

  ret = list_class_proc(p_list, msg, 0, 0);

#ifdef PROGRAM_LIST_FAST_CUT
  u16 cur_prg_pos = 0;

  cur_prg_pos = list_get_focus_pos(p_list);
  list_select_item(p_list, cur_prg_pos);
  ctrl_paint_ctrl(p_list, TRUE);
  
  ui_auto_switch_mode_stop_play(TRUE);
  play_in_small_list(cur_prg_pos);

  
  ui_play_timer_start();
#endif
  
  return SUCCESS;
}

static RET_CODE on_slist_play_tmout(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  play_in_small_list(list_get_focus_pos(p_list));
  ui_play_timer_set_state(0);
  return SUCCESS;
}

static RET_CODE on_slist_change_type(control_t *p_list,
                                      u16 msg, u32 para1, u32 para2)
{
	u8 group_type = 0;
	u16 pos_in_set = 0;
	u32 context = 0;
	u8 view_id;
	BOOL slist_is_tv;
	control_t *p_az_list;
	u16 view_count;
	u16 pg_id;

	switch(msg)
	{
	case MSG_FOCUS_LEFT:
		decrease_prog_type();
		break;
	case MSG_FOCUS_RIGHT:
		increase_prog_type();
		break;
	default:
		return ERR_FAILURE;
		break;
	}
	if((SLIST_PG_RADIO == get_slist_prog_type())&&sys_status_get_group_all_info(CURN_MODE_RADIO, &pg_id))
		sys_status_set_curn_prog_mode(CURN_MODE_RADIO);
	else if(((SLIST_PG_TV == get_slist_prog_type())||(SLIST_PG_HD == get_slist_prog_type()))
					&&sys_status_get_group_all_info(CURN_MODE_TV, &pg_id))
		sys_status_set_curn_prog_mode(CURN_MODE_TV);
	if(CURN_MODE_TV == sys_status_get_curn_prog_mode())
		slist_is_tv = TRUE;
	else
		slist_is_tv = FALSE;
	sys_status_get_group_info(sys_status_get_curn_group(), &group_type, &pos_in_set, &context);
	
	view_id = slist_create_view(group_type, get_slist_prog_type(), slist_is_tv, context);
//	ui_dbase_set_pg_view_id(view_id);
	view_count = db_dvbs_get_count(view_id);
	p_az_list = ctrl_get_child_by_id(ctrl_get_root(p_list), IDC_SLIST_AZ);
	slist_reset_az(p_az_list);
    slist_reset_list(p_list);
#ifdef ENABLE_CA
    cas_manage_finger_repaint();
    OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
#endif
	return SUCCESS;
}

static RET_CODE on_slist_destroy(control_t *p_list,
                                      u16 msg, u32 para1, u32 para2)
{
  ui_time_enable_heart_beat(FALSE);
  ui_deinit_play_timer();
  return ERR_NOFEATURE;
}

static RET_CODE on_slist_az_change_focus(control_t *p_az_list, u16 msg, u32 para1, u32 para2)
{
  u16 focus;
  u16 *p_unistr = NULL;
  control_t *p_cont, *p_list_cont, *p_type = NULL;
  
  p_cont = ctrl_get_parent(p_az_list);
  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_SLIST_CONT);  
  p_type = ctrl_get_child_by_id(p_list_cont, IDC_SMALL_LIST_STATUS);  
  
  list_class_proc(p_az_list, msg, 0, 0);
  
  focus = list_get_focus_pos(p_az_list);
  p_unistr = (u16 *)list_get_field_content(p_az_list, focus, 0);

  
  MT_ASSERT(p_unistr != NULL);
  
   slist_az_find(p_az_list, p_unistr, TRUE);
   
   slist_reset_pg_type(p_type);

  ctrl_paint_ctrl(p_type, TRUE);

  return SUCCESS;
}

static RET_CODE on_slist_az_select(control_t *p_az_list, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_list_cont, *p_list = NULL;
  
  p_cont = ctrl_get_parent(p_az_list);
  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_SLIST_CONT);  
  p_list = ctrl_get_child_by_id(p_list_cont, IDC_SMALL_LIST_LIST);  
  
  if(db_dvbs_get_count(ui_dbase_get_pg_view_id()))
  {
    on_slist_state_process(p_list, MSG_SELECT, 0, 0);
  }

  return  SUCCESS;
}

static RET_CODE on_slist_az_cancle(control_t *p_az_list, u16 msg, u32 para1,
                            u32 para2)
{
  control_t *p_cont, *p_list_cont, *p_list = NULL;
  
  p_cont = ctrl_get_parent(p_az_list);
  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_SLIST_CONT);  
  p_list = ctrl_get_child_by_id(p_list_cont, IDC_SMALL_LIST_LIST);  
  
  on_slist_state_process(p_list, MSG_EXIT, 0, 0);
  
  return  SUCCESS;
}

static u16 slist_t9_get_focus(u16 key)
{
  u8 slist_t9_opt[SLIST_T9_MAX_KEY][SLIST_T9_MAX_OPT] =
  {
    {'0'},
    {'1'},
    {'a', 'b', 'c', '2', },
    {'d', 'e', 'f', '3', },
    {'g', 'h', 'i', '4', },
    {'j', 'k', 'l', '5', },
    {'m', 'n', 'o', '6', },
    {'p', 'q', 'r', 's', '7'},
    {'t', 'u', 'v', '8', },
    {'w', 'x', 'y', 'z', '9'}
  };
  u8 slist_t9_bit_cnt[SLIST_T9_MAX_KEY] =
  {
    1,
    1,
    4,
    4,
    4,
    4,
    4,
    5,
    4,
    5,
  };
  u8 ch;
  u16 focus = 0;

  slist_t9_curn_bit %= slist_t9_bit_cnt[key];

  ch = slist_t9_opt[key][slist_t9_curn_bit];

  if(ch >= '0' && ch <= '9')
  {
    focus = 27 + ch - '0';
  }
  else
  {
    focus = ch - 'a' + 1;
  }

  return focus;
}


static RET_CODE on_slist_t9_input(control_t *p_az_list, u16 msg, u32 para1, u32 para2)
{
  u16 *p_unistr = NULL;
  u16 focus = 0;
  u8 key;
  control_t *p_cont, *p_list_cont, *p_type = NULL;

  key = msg & MSG_DATA_MASK;

  if(key == slist_t9_curn_key)
  {
    slist_t9_curn_bit++;
  }
  else
  {
    slist_t9_curn_bit = 0;
  }

  slist_t9_curn_key = key;

  //jump
  focus = slist_t9_get_focus(key);

  list_set_focus_pos(p_az_list, focus);

  slist_az_update(p_az_list, list_get_valid_pos(p_az_list), SLIST_AZ_PAGE, 0);

  ctrl_paint_ctrl(p_az_list, TRUE);
  
  p_unistr = (u16 *)list_get_field_content(p_az_list, focus, 0);
  
  MT_ASSERT(p_unistr != NULL);
  
  slist_az_find(p_az_list, p_unistr, TRUE);  

  
  p_cont = ctrl_get_parent(p_az_list);
  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_SLIST_CONT);  
  p_type = ctrl_get_child_by_id(p_list_cont, IDC_SMALL_LIST_STATUS);  
 
  slist_reset_pg_type(p_type);

  ctrl_paint_ctrl(p_type, TRUE);


  return SUCCESS;
}

RET_CODE open_small_list_v2(u32 para1, u32 para2)
{
  control_t *p_cont, *p_list, *p_sat, *p_ctrl, *p_title_cont;
  control_t *p_list_cont;
  control_t *p_az_list;
  control_t *p_sort_status;
  u16 i, total = 0;
  u16 list_focus = 0;
  u16 all_num = 0;
  u16 pg_id, cbox_focus = 0xFFFF, pos_in_set;
  u32 context;
  u8 view_id = 0xFF;
  u8 group_type;
  dvbs_view_t view_name;
  list_xstyle_t slist_item_rstyle =
	{
	  RSI_PBACK,
	  RSI_PBACK,
	  RSI_OPT_BTN_H,
	  RSI_ITEM_8_BG, //for bug 37665
	  RSI_OPT_BTN_H,
	};
#ifdef BIGFONT_PROJECT
  list_xstyle_t slist_field_fstyle =
  {
    FSI_GRAY_28,
    FSI_WHITE_28,
    FSI_WHITE_28,
    FSI_BLACK_28,
    FSI_WHITE_28,
  };
#else
  list_xstyle_t slist_field_fstyle =
  {
    FSI_GRAY,
    FSI_WHITE,
    FSI_WHITE,
    FSI_BLACK,
    FSI_WHITE,
  };
#endif

  list_xstyle_t slist_field_rstyle_left =
  {
    RSI_PBACK,
    RSI_PBACK,
    RSI_PBACK,
    RSI_PBACK,
    RSI_PBACK,
  };

  list_xstyle_t slist_field_rstyle_mid =
  {
    RSI_PBACK,
    RSI_PBACK,
    RSI_PBACK,
    RSI_PBACK,
    RSI_PBACK,
  };

  list_xstyle_t slist_az_item_rstyle =
  {
    RSI_PBACK,
    RSI_LEFT_CONT_BG,
    RSI_AZ_LIST_F,
    RSI_AZ_LIST_F,
    RSI_AZ_LIST_F,
  };

  list_xstyle_t slist_az_item_fstyle =
  {
    FSI_GRAY,
    FSI_WHITE,
    FSI_WHITE,
    FSI_BLACK,
    FSI_WHITE,
  };  

#ifdef BIGFONT_PROJECT	
  list_field_attr_t slist_attr[SMALL_LIST_FIELD_NUM] =
  {
    {LISTFIELD_TYPE_UNISTR,
     70, 10, 0, &slist_field_rstyle_left, &slist_field_fstyle},
    {LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
     225, 85, 0, &slist_field_rstyle_mid, &slist_field_fstyle},
  };
#else
list_field_attr_t slist_attr[SMALL_LIST_FIELD_NUM] =
{
  {LISTFIELD_TYPE_UNISTR,
   60, 10, 0, &slist_field_rstyle_left, &slist_field_fstyle},
  {LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
   200, 75, 0, &slist_field_rstyle_mid, &slist_field_fstyle},
};
#endif
  u16 pg_type_ids[SLIST_PG_TYPE_CNT] = {IDS_TV, IDS_HD, IDS_RADIO, IDS_FAV};

  para1 = V_KEY_OK;
  ms = MS_NOR;
  if(ui_get_play_prg_type() == NET_PRG_TYPE)
  {
    if(ui_video_c_init_state() == FALSE)
    {
      ui_video_c_create(VIDEO_PLAY_FUNC_ONLINE);
    }
  }
  else if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
  {
    if(ui_video_c_init_state() == FALSE)
    {
      ui_video_c_create(VIDEO_PLAY_FUNC_USB);
    }
  }
  else if(ui_get_play_prg_type() == SAT_PRG_TYPE)
  {
    b_is_exit = TRUE;
    ui_video_c_stop();
    ui_video_c_destroy();
  }
  
  ui_time_enable_heart_beat(TRUE); 
  slist_mode = sys_status_get_curn_prog_mode();
  slist_group = sys_status_get_curn_group();
  sys_status_get_group_info(slist_group, &group_type, &pos_in_set, &context);
  view_id = ui_dbase_get_pg_view_id();

  if(CURN_MODE_TV == slist_mode)
  {
    is_tv = TRUE;
	view_id = ui_dbase_create_view(DB_DVBS_ALL_TV, context, NULL);
  }
  else if(CURN_MODE_RADIO == slist_mode)
  {
    is_tv = FALSE;
	view_id = ui_dbase_create_view(DB_DVBS_ALL_RADIO, context, NULL);
  }
  else if(ui_get_play_prg_type() == NET_PRG_TYPE)
  {
  }
  else
  {
    return ERR_FAILURE;
  }

  if(para1 == V_KEY_OK)
  {
    slist_hkey = V_KEY_OK;

	view_name = db_dvbs_get_view_name_by_view_id(view_id);
	switch(view_name)
	{
	case DB_DVBS_FAV_ALL:
		set_slist_prog_type(SLIST_PG_FAV);
		break;
	case DB_DVBS_ALL_TV_HD:
		set_slist_prog_type(SLIST_PG_HD);
		break;	
	case DB_DVBS_ALL_HIDE_TV:
	case DB_DVBS_ALL_TV:
		set_slist_prog_type(SLIST_PG_TV);
		break;
	case DB_DVBS_ALL_HIDE_RADIO:
	case DB_DVBS_ALL_RADIO:
		set_slist_prog_type(SLIST_PG_RADIO);
		break;
	default:
		set_slist_prog_type(SLIST_PG_TV);
		break;
	}
    all_num = sys_status_get_all_group_num() + 3;
  }
  else 
  {
	return ERR_FAILURE;
  }

  g_group_num = all_num;
  sys_status_get_curn_prog_in_group(slist_group, slist_mode, &pg_id, &context);
  sys_status_set_curn_group(slist_group);
  sys_status_set_curn_prog_mode(slist_mode);
  sys_status_save();

  ui_dbase_set_pg_view_id(view_id);

  if(NULL!=fw_find_root_by_id(ROOT_ID_PROG_BAR))
  	manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
  p_cont = fw_create_mainwin(ROOT_ID_SMALL_LIST,
                             COMM_BG_X ,
                             COMM_BG_Y,
                            SMALL_MENU_W,
                             COMM_BG_H,
                             0,
                             0,
                             OBJ_ATTR_ACTIVE, 0);

  MT_ASSERT(p_cont != NULL);

  ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);

  p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_SLIST_CONT,
                                  SMALL_LIST_MENU_CONT_X, SMALL_LIST_MENU_CONT_Y,
                                  (SMALL_LIST_MENU_WIDTH + SMALL_EXTERN_LEFT), SMALL_LIST_MENU_HEIGHT,
                                  p_cont, 0);

  MT_ASSERT(p_list_cont != NULL);

  ctrl_set_rstyle(p_list_cont, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);
  ctrl_set_keymap(p_list_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_list_cont, ui_comm_root_proc);

  //create program list
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_SMALL_LIST_LIST,
                            (SMALL_LIST_LIST_X + SMALL_EXTERN_LEFT), 0,
                            SMALL_LIST_LIST_W, SMALL_LIST_LIST_H,
                            p_list_cont, 0);
  MT_ASSERT(p_list != NULL);
  ctrl_set_rstyle(p_list, RSI_SLIST_LIST, RSI_SLIST_LIST, RSI_SLIST_LIST);
  ctrl_set_keymap(p_list, small_list_keymap_v2);
  ctrl_set_proc(p_list, small_list_proc_v2);

  ctrl_set_mrect(p_list, SMALL_LIST_MID_L, SMALL_LIST_MID_T,
                    SMALL_LIST_MID_W, SMALL_LIST_MID_H);
  list_set_item_interval(p_list, SMALL_LIST_ITEM_V_GAP);
  list_set_item_rstyle(p_list, &slist_item_rstyle);
  list_enable_select_mode(p_list, TRUE);

   if(ui_get_play_prg_type() == NET_PRG_TYPE)
   {
    //  total = ui_iptv_get_channel_count();
    //  list_focus = ui_iptv_get_cur_prg_pos();
    //  list_set_update(p_list, slist_net_update, 0);
   }
   else if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
   {
      total = 4;
      list_focus = 0;
      list_set_update(p_list, slist_camera_update, 0);
   }
   else
   {
     total = db_dvbs_get_count(view_id);
     if(total > 0)
     {
      list_focus = db_dvbs_get_view_pos_by_id(view_id, pg_id);
     }
     list_set_update(p_list, slist_update, 0);
   }

  list_set_count(p_list, total, SMALL_LIST_PAGE);
  list_set_field_count(p_list, SMALL_LIST_FIELD_NUM, SMALL_LIST_PAGE);
  list_set_focus_pos(p_list, list_focus);
  list_select_item(p_list, list_focus);
//  list_enable_page_mode(p_list, TRUE);
  for(i = 0; i < SMALL_LIST_FIELD_NUM; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(slist_attr[i].attr), (u16)(slist_attr[i].width),
                        (u16)(slist_attr[i].left), (u8)(slist_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, slist_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, slist_attr[i].fstyle);
  }

 // if(ui_get_play_prg_type() == NET_PRG_TYPE)
//    slist_net_update(p_list, list_get_valid_pos(p_list), SMALL_LIST_PAGE, 0);
   if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
    slist_camera_update(p_list, list_get_valid_pos(p_list), SMALL_LIST_PAGE, 0);
  else
    slist_update(p_list, list_get_valid_pos(p_list), SMALL_LIST_PAGE, 0);

	p_title_cont = ctrl_create_ctrl(CTRL_CONT, IDC_TITLE_CONT, 0, 0, 
							(SMALL_LIST_MENU_WIDTH + SMALL_EXTERN_LEFT), SMALL_LIST_TITLE_CONT_H, 
							p_cont, 0);
	ctrl_set_rstyle(p_title_cont, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);
  //group arrow
  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_SMALL_LIST_ARROWL,
                             SLIST_ARROWL_X, SLIST_ARROWL_Y,
                             SLIST_ARROWL_W, SLIST_ARROWL_H,
                             p_title_cont, 0);
  bmap_set_content_by_id(p_ctrl, IM_ARROW1_L);

  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_SMALL_LIST_ARROWR,
                             SLIST_ARROWR_X, SLIST_ARROWR_Y,
                             SLIST_ARROWR_W, SLIST_ARROWR_H,
                             p_title_cont, 0);
  bmap_set_content_by_id(p_ctrl, IM_ARROW1_R);

  //sat
  p_sat = ctrl_create_ctrl(CTRL_CBOX, IDC_SMALL_LIST_SAT,
                           SMALL_LIST_SAT_X, SMALL_LIST_SAT_Y,
                           SMALL_LIST_SAT_W, SMALL_LIST_SAT_H,
                           p_title_cont, 0);
  MT_ASSERT(p_sat != NULL);
  ctrl_set_rstyle(p_sat, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  cbox_set_font_style(p_sat, FSI_SLIST_TEXT, FSI_SLIST_TEXT, FSI_SLIST_TEXT);
  cbox_enable_cycle_mode(p_sat, TRUE);
  if((ui_get_play_prg_type() == NET_PRG_TYPE)||(ui_get_play_prg_type() == CAMERA_PRG_TYPE))
  {
    cbox_set_work_mode(p_sat, CBOX_WORKMODE_DYNAMIC);
  	cbox_dync_set_count(p_sat, all_num);
  }
  else
  {
 	 cbox_set_work_mode(p_sat, CBOX_WORKMODE_STATIC);
 	 cbox_static_set_content_type(p_sat, CBOX_ITEM_STRTYPE_STRID);
	 cbox_static_set_count(p_sat, SLIST_PG_TYPE_CNT);
	  for(i = 0; i<SLIST_PG_TYPE_CNT; i++)
	  {
	 	 cbox_static_set_content_by_strid(p_sat, i, pg_type_ids[i]);
	  }
  }

  if(ui_get_play_prg_type() == NET_PRG_TYPE)
    cbox_dync_set_update(p_sat, slist_net_cbox_update);
  else if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
    cbox_dync_set_update(p_sat, slist_camera_cbox_update);
  sys_status_get_group_info(slist_group, &group_type, &pos_in_set, &context);
  switch(slist_hkey)
  {
    case V_KEY_OK:
		cbox_focus = (u16)(get_slist_prog_type());
      break;
    case V_KEY_SAT:
      cbox_focus = sys_status_get_sat_group_pos(slist_group);
      break;
    case V_KEY_FAV:
		cbox_focus = sys_status_get_fav_group_pos(slist_group);
      break;
    default:
      if(ui_get_play_prg_type() == SAT_PRG_TYPE)
        MT_ASSERT(0);
      break;
  }

  if(ui_get_play_prg_type() == NET_PRG_TYPE)
    cbox_dync_set_focus(p_sat, g_group_num - 2);
  else if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
    cbox_dync_set_focus(p_sat, g_group_num - 1);
  else
    cbox_static_set_focus(p_sat, cbox_focus);

  //sort status
  p_sort_status = ctrl_create_ctrl(CTRL_TEXT,
                                   IDC_SMALL_LIST_STATUS,
                                   SLIST_SORT_STATUS_X,
                                   SLIST_SORT_STATUS_Y,
                                   SLIST_SORT_STATUS_W,
                                   SLIST_SORT_STATUS_H,
                                   p_list_cont,
                                   0);
  MT_ASSERT(p_sort_status != NULL);
  ctrl_set_rstyle(p_sort_status, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_sort_status, STL_RIGHT | STL_TOP);
  text_set_content_type(p_sort_status, TEXT_STRTYPE_UNICODE);
  text_set_font_style(p_sort_status, FSI_SLIST_SORT_STATUS, FSI_SLIST_SORT_STATUS,
                      FSI_SLIST_SORT_STATUS);
  ctrl_set_sts(p_sort_status, OBJ_STS_HIDE);

  //a~z list.
  p_az_list = ctrl_create_ctrl(CTRL_LIST, IDC_SLIST_AZ, 
                        SMALL_LIST_AZ_X, SMALL_LIST_AZ_Y,
                        SMALL_LIST_AZ_W, SMALL_LIST_AZ_H, 
                        p_cont, 0);
  ctrl_set_rstyle(p_az_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_az_list, slist_az_keymap);
  ctrl_set_proc(p_az_list, slist_az_proc);
  ctrl_set_mrect(p_az_list, 
    SMALL_LIST_AZ_ITEM_L, SMALL_LIST_AZ_ITEM_T,
    SMALL_LIST_AZ_ITEM_W, SMALL_LIST_AZ_ITEM_H);
  list_set_item_interval(p_az_list, SMALL_LIST_ITEM_V_GAP);
  list_set_item_rstyle(p_az_list, &slist_az_item_rstyle);
  list_enable_select_mode(p_az_list, TRUE);
  list_set_count(p_az_list, SLIST_AZ_TOTAL, SLIST_AZ_PAGE);
  list_set_select_mode(p_az_list, LIST_SINGLE_SELECT);
  list_set_field_count(p_az_list, 1, SLIST_AZ_PAGE);
  list_set_field_font_style(p_az_list, 0, &slist_az_item_fstyle);
  list_set_focus_pos(p_az_list, 0);
  list_set_update(p_az_list, slist_az_update, 0);
  list_set_field_attr(p_az_list, 0, LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER, 30,
                        0, 0);
  list_set_field_rect_style(p_az_list, 0, &slist_az_item_rstyle);
  slist_az_update(p_az_list, 0, 0, 0);
  ctrl_set_sts(p_az_list, OBJ_STS_HIDE);

  if(ui_get_play_prg_type() == NET_PRG_TYPE)
  {
  }
     // slist_reset_net_source_type(p_sort_status);
  else
    slist_reset_pg_type(p_sort_status);

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  ui_init_play_timer(ROOT_ID_SMALL_LIST, MSG_MEDIA_PLAY_TMROUT, 200);
  manage_open_menu(ROOT_ID_FUN_HELP, 0, 0);
  
  if(ui_get_play_prg_type() == SAT_PRG_TYPE)
  {
    b_is_exit = TRUE;
    ui_video_c_stop();
    play_in_small_list(list_focus);
  }
  else if(ui_get_play_prg_type() == NET_PRG_TYPE)
  {
   /* if(ui_video_c_get_play_state() != MUL_PLAY_STATE_PLAY)
    {
      ui_stop_play(STOP_PLAY_BLACK, TRUE);
      small_list_open_dlg(IDS_LOAD_MEDIA, 0);
      if(ui_video_c_get_play_state() == MUL_PLAY_STATE_NONE)
      //  on_list_play_iptv_after_exit_msg(p_list, 0, 0, 0);
      else
      {
        b_is_exit = FALSE;
        ui_video_c_stop();
      }
    }*/
  }
#ifdef ENABLE_CA
  cas_manage_finger_repaint();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
#endif

  #ifdef ENABLE_ADS
  ui_adv_pic_play(ADS_AD_TYPE_CHLIST, ROOT_ID_SMALL_LIST);
  #endif
  return SUCCESS;
}

static RET_CODE on_slist_extend(control_t *p_list, u16 msg, u32 para1,u32 para2)
{
	u16 focus;
	u16 y;
	rect_t rect;
	RET_CODE ret;

	play_focus_pg_in_slist(p_list, PLAY_IN_SLIST_NOTHING);
	focus = list_get_focus_pos(p_list);
	list_get_item_rect(p_list, focus, &rect);
	y = SMALL_LIST_LIST_Y+rect.top;
	ret = manage_open_menu(ROOT_ID_XEXTEND, (u32)y, 0);
	ctrl_paint_ctrl(ctrl_get_root(p_list), TRUE);
	return ret;
}


BEGIN_KEYMAP(small_list_keymap_v2, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_HOT_XEXTEND, MSG_EXTEND_MENU)
//  ON_EVENT(V_KEY_GREEN, MSG_NEWS)
//  ON_EVENT(V_KEY_YELLOW, MSG_CHANGE_TO_FAV)
//  ON_EVENT(V_KEY_BLUE, MSG_SMALL_LIST_BULE)
//  ON_EVENT(V_KEY_INFO, MSG_SMALL_LIST_INFO)
  ON_EVENT(V_KEY_TVRADIO, MSG_TVRADIO)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(small_list_keymap_v2, NULL)

BEGIN_MSGPROC(small_list_proc_v2, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_slist_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_slist_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_slist_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_slist_change_focus)
  ON_COMMAND(MSG_FOCUS_LEFT, on_slist_change_type)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_slist_change_type)
 // ON_COMMAND(MSG_CHANGE_LIST, on_slist_state_process)
 // ON_COMMAND(MSG_CHANGE_TO_FAV, on_slist_state_process)
//  ON_COMMAND(MSG_NEWS, on_slist_state_process)
  ON_COMMAND(MSG_SMALL_LIST_BULE, on_slist_state_process)
//  ON_COMMAND(MSG_SMALL_LIST_INFO, on_slist_state_process)
  ON_COMMAND(MSG_SELECT, on_slist_select_play_process)  
  ON_COMMAND(MSG_EXIT, on_slist_state_process)
  ON_COMMAND(MSG_EXIT_ALL, on_slist_state_process)
  ON_COMMAND(MSG_DESTROY, on_slist_destroy)
  ON_COMMAND(MSG_MEDIA_PLAY_TMROUT, on_slist_play_tmout)
  ON_COMMAND(MSG_EXTEND_MENU, on_slist_extend)
  ON_COMMAND(MSG_SMALLIST_UPDATE, slist_update_from_extend)
  ON_COMMAND(MSG_PLAY_IN_SLIST, play_in_slist_from_extend)
  ON_COMMAND(MSG_SLIST_SAVE, save_slist_data_from_extend)
END_MSGPROC(small_list_proc_v2, list_class_proc)

BEGIN_KEYMAP(slist_az_keymap, ui_comm_t9_keymap)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_YES)
END_KEYMAP(slist_az_keymap, ui_comm_t9_keymap)

BEGIN_MSGPROC(slist_az_proc, list_class_proc)
  ON_COMMAND(MSG_CANCEL, on_slist_az_cancle)
  ON_COMMAND(MSG_FOCUS_UP, on_slist_az_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_slist_az_change_focus)
  ON_COMMAND(MSG_YES, on_slist_az_select)
  ON_COMMAND(MSG_NUMBER, on_slist_t9_input)
END_MSGPROC(slist_az_proc, list_class_proc)

