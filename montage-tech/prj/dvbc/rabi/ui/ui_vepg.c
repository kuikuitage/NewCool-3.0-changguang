#include "ui_common.h"
#include "ui_vepg.h"
#include "ui_timer.h"
#include "ui_small_list_v2.h"
#include "ui_xextend.h"

#define IDC_VEPG_CONT_ID IDC_COMM_ROOT_CONT

enum vepg_local_msg
{
  MSG_RED = MSG_LOCAL_BEGIN + 250,
  MSG_GREEN,
  MSG_YELLOW,
  MSG_BLUE,
  MSG_INFO,
  MSG_GROUP,
  MSG_RESET,
};


enum focus_style
{
  FOCUS_NONE = 0,
  FOCUS_FIRST,
  FOCUS_LAST,
  FOCUS_NEXT,
  FOCUS_PREV,
};

enum vepg_menu_ctrl_id
{
  IDC_VEPG_ICON = 1,
  IDC_VEPG_TOP_LINE,
  IDC_VEPG_BOTTOM_LINE,
//  IDC_VEPG_PREVIEW,
  IDC_VEPG_DETAIL,
//  IDC_VEPG_TIME,
  IDC_VEPG_LIST_CONT,
//  IDC_VEPG_HELP,
//  IDC_VEPG_LIST_SBAR,

  IDC_VEPR_CONT,
};

enum vepg_list_cont_id
{
  IDC_VEPG_LIST_GROUP = 1,
//  IDC_VEPG_LIST_PG,
  IDC_VEPG_LIST_DATE,
  IDC_VEPG_LIST_EVT,
};

enum vepg_list_detail_id
{
  IDC_VEPG_DETAIL_TTL = 1,
//  IDC_VEPG_DETAIL_PG,
  IDC_VEPG_DETAIL_SAT,
//  IDC_VEPG_DETAIL_EVTNAME,
//  IDC_VEPG_DETAIL_EVTTIME,
  IDC_VEPG_DETAIL_SHORT_EVT,
};

list_xstyle_t vepg_list_item_rstyle =
{
	  RSI_PBACK,
	  RSI_PBACK,
	  RSI_OPT_BTN_H,
	  RSI_ITEM_8_BG, //for bug 37665
	  RSI_OPT_BTN_H,
	};


list_xstyle_t vepg_list_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLACK,
  FSI_WHITE,
};

list_xstyle_t vepg_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

list_field_attr_t vepg_pg_attr[VEPG_LIST_FIELD] =
{
  {LISTFIELD_TYPE_UNISTR | STL_LEFT,
   70, 4, 0, &vepg_list_field_rstyle, &vepg_list_field_fstyle},
  {LISTFIELD_TYPE_UNISTR | STL_LEFT| LISTFIELD_SCROLL,
   148, 74, 0, &vepg_list_field_rstyle, &vepg_list_field_fstyle},
};
list_field_attr_t vepg_date_attr[VEPG_LIST_FIELD] =
{
  {LISTFIELD_TYPE_STRID | STL_LEFT,
   55, 5, 0, &vepg_list_field_rstyle, &vepg_list_field_fstyle},
  {LISTFIELD_TYPE_UNISTR | STL_LEFT,
   70, 60, 0, &vepg_list_field_rstyle, &vepg_list_field_fstyle},
};
list_field_attr_t vepg_evt_attr[VEPG_LIST_FIELD] =
{
  {LISTFIELD_TYPE_UNISTR | STL_LEFT,
   140, 5, 0, &vepg_list_field_rstyle, &vepg_list_field_fstyle},
  {LISTFIELD_TYPE_UNISTR | STL_LEFT,
   200, 145, 0, &vepg_list_field_rstyle, &vepg_list_field_fstyle},
};


static u16 vepg_pg_id;
book_pg_t book_node;

static RET_CODE vepg_list_date_update(control_t *p_list, u16 start, u16 size, u32 context);
static RET_CODE vepg_list_evt_update(control_t *p_list, u16 start, u16 size, u32 context);

u16 vepg_cont_keymap(u16 key);
RET_CODE vepg_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 vepg_listcont_keymap(u16 key);
RET_CODE vepg_listcont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 vepg_item_keymap(u16 key);
RET_CODE vepg_item_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 vepg_list_date_keymap(u16 key);
RET_CODE vepg_list_date_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

u16 vepg_list_evt_keymap(u16 key);
RET_CODE vepg_list_evt_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);


u16 vepg_step_keymap(u16 key);
RET_CODE vepg_step_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static u16 vepg_get_weekday_str(utc_time_t *p_time)
{
  u8 week_day = 0;

  week_day = date_to_weekday(p_time);
/*
   switch(week_day)
   {
    case 0:
      memcpy(p_str, "Mon", 4);
      break;
    case 1:
      memcpy(p_str, "Tus", 4);
      break;
    case 2:
      memcpy(p_str, "Wed", 4);
      break;
    case 3:
      memcpy(p_str, "Thu", 4);
      break;
    case 4:
      memcpy(p_str, "Fri", 4);
      break;
    case 5:
      memcpy(p_str, "Sat", 4);
      break;
    case 6:
      memcpy(p_str, "Sun", 4);
      break;
    default:
      MT_ASSERT(0);
      break;
   }*/

  return IDS_MON + week_day;
}

static void set_vepg_pg_id(u16 id)
{
	vepg_pg_id = id;
}

static u16 get_vepg_pg_id(void)
{
	return vepg_pg_id;
}
/*
static void ui_pg_list_start_roll(control_t *p_cont)
{
  roll_param_t p_param = {0};
  MT_ASSERT(p_cont != NULL);

  gui_stop_roll(p_cont);

  p_param.pace = ROLL_SINGLE;
  p_param.style = ROLL_LR;
  p_param.repeats = 0;
  p_param.is_force = FALSE;

  gui_start_roll(p_cont, &p_param);

}
*/
static void ui_vepg_list_start_roll(control_t *p_cont)
{
	roll_param_t p_param = {0};
	MT_ASSERT(p_cont != NULL);

	gui_stop_roll(p_cont);

	p_param.pace = ROLL_SINGLE;
	p_param.style = ROLL_LR;
	p_param.repeats = 0;
	p_param.is_force = FALSE;

	gui_start_roll(p_cont, &p_param);

}

static RET_CODE vepg_evt_list_reset(control_t *p_list_cont, u16 pg_id)
{
  control_t *p_list_evt, *p_list_date;
#ifndef IMPL_NEW_EPG
  epg_filter_t item_info;
#else
  epg_prog_info_t prog_info = {0};
#endif
  dvbs_prog_node_t pg;
  utc_time_t s_time = {0};
  utc_time_t e_time = {0};
  utc_time_t t_time = {0};
  u16 date_focus, evt_focus;
  u32 total;

  p_list_evt = ctrl_get_child_by_id(p_list_cont, IDC_VEPG_LIST_EVT);
  p_list_date = ctrl_get_child_by_id(p_list_cont, IDC_VEPG_LIST_DATE);

  date_focus = list_get_focus_pos(p_list_date);
  evt_focus = list_get_focus_pos(p_list_evt);

  time_get(&s_time, FALSE);

  if(date_focus != 0)
  {
    s_time.hour = 0;
    s_time.minute = 0;
    s_time.minute = 0;
    t_time.day = (u8)date_focus;

    time_add(&s_time, &t_time);
  }

  memcpy(&e_time, &s_time, sizeof(utc_time_t));
  e_time.hour = 23;
  e_time.minute = 59;
  e_time.second = 59;

  db_dvbs_get_pg_by_id(pg_id, &pg);
#ifndef IMPL_NEW_EPG
  item_info.service_id = (u16)pg.s_id;
  item_info.stream_id = (u16)pg.ts_id;
  item_info.orig_network_id = (u16)pg.orig_net_id;
  item_info.cont_level = 0;
  time_to_gmt(&s_time, &(item_info.start_time));
  time_to_gmt(&e_time, &(item_info.end_time));

  epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID), &item_info, &total);
#else
  prog_info.network_id = (u16)pg.orig_net_id;
  prog_info.ts_id      = (u16)pg.ts_id;
  prog_info.svc_id     = (u16)pg.s_id;
  time_to_gmt(&s_time, &(prog_info.start_time));
  time_to_gmt(&e_time, &(prog_info.end_time));

  mul_epg_set_db_info(&prog_info);
  mul_epg_get_sch_event(&prog_info, (u16*)&total);
#endif

  if(evt_focus >= total)
  {
    evt_focus = 0;
  }

  list_set_count(p_list_evt, (u16)total, VEPG_LIST_PAGE);
  list_set_focus_pos(p_list_evt, evt_focus);
  list_select_item(p_list_evt, evt_focus);
  OS_PRINTF("select item %d\n", evt_focus);
  return SUCCESS;
}


#ifndef IMPL_NEW_EPG
static evt_node_t *vepg_get_curn_node(control_t *p_list_cont)
#else
static event_node_t *vepg_get_curn_node(control_t *p_list_cont)
#endif
{
  control_t *p_list_evt, *p_list_date;
#ifndef IMPL_NEW_EPG
  epg_filter_t item_info;
  evt_node_t *p_evt_node;
  u32 item_num;
  u16 i = 0;
#else
  epg_prog_info_t prog_info = {0};
  event_node_t   *p_evt_node = NULL;
  u16 total_event  = 0;
#endif
  dvbs_prog_node_t pg;
  utc_time_t s_time = {0};
  utc_time_t e_time = {0};
  utc_time_t t_time = {0};
  u16 date_focus, evt_focus;
  u16 pg_id;

  p_list_evt = ctrl_get_child_by_id(p_list_cont, IDC_VEPG_LIST_EVT);
  p_list_date = ctrl_get_child_by_id(p_list_cont, IDC_VEPG_LIST_DATE);

  if(list_get_count(p_list_evt) == 0)
  {
    return NULL;
  }

  date_focus = list_get_focus_pos(p_list_date);
  evt_focus = list_get_focus_pos(p_list_evt);

  time_get(&s_time, FALSE);

  if(date_focus != 0)
  {
    s_time.hour = 0;
    s_time.minute = 0;
    s_time.second = 0;
    t_time.day = (u8)date_focus;

    time_add(&s_time, &t_time);
  }

  memcpy(&e_time, &s_time, sizeof(utc_time_t));
  e_time.hour = 23;
  e_time.minute = 59;
  e_time.second = 59;

  pg_id = get_vepg_pg_id();
  db_dvbs_get_pg_by_id(pg_id, &pg);
#ifndef IMPL_NEW_EPG
  item_info.service_id = (u16)pg.s_id;
  item_info.stream_id = (u16)pg.ts_id;
  item_info.orig_network_id = (u16)pg.orig_net_id;
  time_to_gmt(&s_time, &(item_info.start_time));
  time_to_gmt(&e_time, &(item_info.end_time));
  item_info.cont_level = 0;

  p_evt_node = epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID),
                                &item_info, &item_num);

  for(i = 0; i < evt_focus; i++)
  {
    if(p_evt_node != NULL)
    {
      p_evt_node = epg_data_get_next_evt(class_get_handle_by_id(
                                           EPG_CLASS_ID), p_evt_node, &item_info);
    }
  }
#else
  prog_info.network_id = (u16)pg.orig_net_id;
  prog_info.ts_id      = (u16)pg.ts_id;
  prog_info.svc_id     = (u16)pg.s_id;
  time_to_gmt(&s_time, &(prog_info.start_time));
  time_to_gmt(&e_time, &(prog_info.end_time));
  mul_epg_set_db_info(&prog_info);
  p_evt_node = mul_epg_get_sch_event(&prog_info, &total_event);
  
  if(evt_focus > 0)
  {
    p_evt_node = mul_epg_get_sch_event_by_pos(&prog_info,
                                             p_evt_node,
                                             evt_focus);
  }
#endif

  return p_evt_node;
}


static void vepg_set_detail(control_t *p_detail, BOOL is_paint)
{
  control_t *p_list_cont ;
  control_t  *p_short;
  dvbs_prog_node_t pg;
#ifndef IMPL_NEW_EPG
  evt_node_t evt_node = {0};
  evt_node_t *p_evt_node = &evt_node;
#else
  event_node_t *p_evt_node = NULL;
#endif
  utc_time_t startime = {0};
  utc_time_t endtime = {0};
  u16 pg_id;
  u8 asc_str[32];


  OS_PRINTF("epg set detail\n");
  p_list_cont = ctrl_get_child_by_id(ctrl_get_parent(p_detail), IDC_VEPG_LIST_CONT);
  
  //db_dvbs_get_pg_by_id(sys_status_get_curn_group_curn_prog_id(), &pg);
  pg_id = get_vepg_pg_id();

  MT_ASSERT(pg_id != INVALIDID);
  if(db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
  }

   p_short = ctrl_get_child_by_id(p_detail, IDC_VEPG_DETAIL_SHORT_EVT);
 
  p_evt_node = vepg_get_curn_node(p_list_cont);

  if(p_evt_node != NULL)
  {
    //set event name
 
    time_to_local(&p_evt_node->start_time, &startime);

    memcpy(&endtime, &startime, sizeof(utc_time_t));
    time_add(&endtime, &(p_evt_node->drt_time));

    //set event time
    sprintf(asc_str, "%.2d:%.2d-%.2d:%.2d",
            startime.hour, startime.minute, endtime.hour, endtime.minute);
 
#ifndef IMPL_NEW_EPG
    if(p_evt_node->p_sht_txt != NULL)
    {
      text_set_content_by_unistr(p_short, p_evt_node->p_sht_txt);
    }
#else
    if(p_evt_node->p_sht_text != NULL)
    {
      text_set_content_by_unistr(p_short, p_evt_node->p_sht_text);
    }
#endif
    else
    {
      text_set_content_by_ascstr(p_short, " ");
    }
  }
  else
  {
    //no event, set as null
     text_set_content_by_ascstr(p_short, " ");
  }


  if(is_paint)
  {
     ctrl_paint_ctrl(p_short, TRUE);
  }
}



void vepg_list_update_time(control_t *p_info, BOOL is_paint)
{
  utc_time_t time;
  u8 asc_str[10];

  time_get(&time, FALSE);

//  printf_time(&time, "epg_list_update_time");

  sprintf(asc_str, "%.2d/%.2d", time.month, time.day);
  mbox_set_content_by_ascstr(p_info, 0, asc_str);

  sprintf(asc_str, "%.2d:%.2d", time.hour, time.minute);
  mbox_set_content_by_ascstr(p_info, 1, asc_str);

  if(is_paint)
  {
    ctrl_paint_ctrl(p_info, TRUE);
  }
}
  
static RET_CODE vepg_list_date_update(control_t *p_list, u16 start, u16 size,
                                      u32 context)
{
  u16 i;
  u8 asc_str[8];
  u16 cnt = list_get_count(p_list);
  utc_time_t loc_time = {0};
  utc_time_t shw_time = {0};
  utc_time_t add_time = {0};

  time_get(&loc_time, FALSE);

  for(i = 0; i < size; i++)
  {
    if(i + start < cnt)
    {
      memset(&add_time, 0, sizeof(utc_time_t));
      add_time.day = (start + i);
      memcpy(&shw_time, &loc_time, sizeof(utc_time_t));
      time_add(&shw_time, &add_time);

      /* WEEK DAY */

      list_set_field_content_by_strid(p_list, (u16)(start + i), 0, vepg_get_weekday_str(&shw_time));

      /* DATE */
      sprintf(asc_str, "%.2d/%.2d", shw_time.month, shw_time.day);
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 1, asc_str);
    }
  }
  return SUCCESS;
}


static RET_CODE vepg_list_evt_update(control_t *p_list, u16 start, u16 size,
                                     u32 context)
{
  u16 pg_id;
  u16 cnt = list_get_count(p_list);
  dvbs_prog_node_t pg;
  control_t *p_list_cont , *p_list_date;
  u16 date_focus = 0;
  u16 i = 0;
  u8 asc_str[16];
  utc_time_t s_time_temp = {0};
#ifndef IMPL_NEW_EPG
  epg_filter_t item_info;
  evt_node_t *p_evt_node = NULL;
  u32 item_num;
#else
  epg_prog_info_t prog_info = {0};
  event_node_t   *p_evt_node = NULL;
  u16           total_evt  = 0;
#endif
  utc_time_t s_time = {0};
  utc_time_t e_time = {0};
  utc_time_t a_time = {0};

  p_list_cont = ctrl_get_parent(p_list);
   p_list_date = ctrl_get_child_by_id(p_list_cont, IDC_VEPG_LIST_DATE);
   date_focus = list_get_focus_pos(p_list_date);

  pg_id = get_vepg_pg_id();
  db_dvbs_get_pg_by_id(pg_id, &pg);

  time_get(&s_time, FALSE);
  if(date_focus != 0)
  {
    s_time.hour = 0;
    s_time.minute = 0;
    s_time.second = 0;

    a_time.day = (u8)date_focus;

    time_add(&s_time, &a_time);
  }

  memcpy(&e_time, &s_time, sizeof(utc_time_t));
  e_time.hour = 23;
  e_time.minute = 59;
  e_time.second = 59;

#ifndef IMPL_NEW_EPG
  item_info.service_id = (u16)pg.s_id;
  item_info.stream_id = (u16)pg.ts_id;
  item_info.orig_network_id = (u16)pg.orig_net_id;
  item_info.cont_level = 0;
  time_to_gmt(&s_time, &(item_info.start_time));
  time_to_gmt(&e_time, &(item_info.end_time));
  ui_set_epg_policy_period(&item_info.start_time, 48);
  p_evt_node = epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID),
                                &item_info, &item_num);

  i = 0;
  while(p_evt_node)
  {
    if(i < start)
    {
      p_evt_node = epg_data_get_next_evt(class_get_handle_by_id(
                                           EPG_CLASS_ID), p_evt_node, &item_info);
      i++;
    }
    else
    {
      break;
    }
  }
#else
  prog_info.network_id = (u16)pg.orig_net_id;
  prog_info.ts_id      = (u16)pg.ts_id;
  prog_info.svc_id     = (u16)pg.s_id;
  time_to_gmt(&s_time, &(prog_info.start_time));
  time_to_gmt(&e_time, &(prog_info.end_time));

  mul_epg_set_db_info(&prog_info);
  p_evt_node = mul_epg_get_sch_event(&prog_info, &total_evt);
  if(p_evt_node == NULL)
  {
    return ERR_FAILURE;
  }
 /*!
  found start event node.
  */ 
  if(start > 0)
  {
    p_evt_node = mul_epg_get_sch_event_by_pos(&prog_info, p_evt_node, start);
  }
#endif

  for(i = 0; i < size; i++)
  {
  #ifndef IMPL_NEW_EPG
    if(i + start < cnt)
  #else
    if((i + start < cnt) && (i + start < total_evt))
  #endif
    {
      //item_info.item_pos = (u16)(start + i + 1);
      //p_evt_node = epg_data_get2(class_get_handle_by_id(EPG_CLASS_ID), &item_info);

      if(p_evt_node != NULL)
      {
        /* NO. */
        time_to_local(&(p_evt_node->start_time), &s_time);
        if(i == 0)          //check first evt wether overnight
          {
            memcpy(&e_time, &s_time, sizeof(utc_time_t));
            memcpy(&s_time_temp, &s_time, sizeof(utc_time_t));
            time_add(&e_time, &p_evt_node->drt_time);
            e_time.hour = 0;
            e_time.minute = 0;
            e_time.second = 0;
            s_time_temp.hour = 0;
            s_time_temp.minute = 0;
            s_time_temp.second = 0;
            if(time_cmp(&s_time_temp, &e_time, FALSE) < 0)
              {
                s_time.day = e_time.day;
                s_time.hour = 0;
                s_time.minute = 0;
              }
          }
        }
	 	memcpy(&e_time, &s_time, sizeof(utc_time_t));
	  	time_add(&e_time, &(p_evt_node->drt_time));
    	sprintf(asc_str, "%.2d:%.2d-%.2d:%.2d",
            s_time.hour, s_time.minute, e_time.hour, e_time.minute);        
        list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);

        /* NAME */
        list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, p_evt_node->event_name);
      }
      else
      {
        /* NO. */
        list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, "     ");

        /* NAME */
        list_set_field_content_by_ascstr(p_list, (u16)(start + i), 1, "     ");
      }

#ifndef IMPL_NEW_EPG
      p_evt_node = epg_data_get_next_evt(class_get_handle_by_id(
                                           EPG_CLASS_ID), p_evt_node, &item_info);
#else
    if((i + start + 1 < cnt) && (i + start + 1 < total_evt))
      p_evt_node = mul_epg_get_sch_event_by_pos(&prog_info, p_evt_node, 1);
#endif
    }

  if(cnt == 0)
  {
    if(ctrl_get_active_ctrl(p_list_cont) == p_list)
    {
      ctrl_default_proc(p_list, MSG_LOSTFOCUS, 0, 0);

      ctrl_default_proc(p_list_date, MSG_GETFOCUS, 0, 0);
      ctrl_paint_ctrl(p_list_date, TRUE);
    }
  }
  ui_vepg_list_start_roll(p_list);
  return SUCCESS;
}

control_t *get_vepg_date_list(void)
{
	control_t *p_ctrl;

	p_ctrl = fw_find_root_by_id(ROOT_ID_VEPG);
	if(NULL == p_ctrl)
		return NULL;
	p_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_VEPR_CONT);			
	p_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_VEPG_LIST_CONT);
	p_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_VEPG_LIST_DATE);

	return p_ctrl;
}

RET_CODE open_vepg(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_detail ;
  control_t *p_list_cont;
  control_t *p_short;
  control_t *p_list_date, *p_list_evt;// *p_group_list,
   u16 i, curn_group, curn_mode, pg_id, group_num;
  u16 x, y;
  btn_rec_t *btn_rec = (btn_rec_t*)para1;
  utc_time_t s_time = {0};
  dvbs_prog_node_t pg;
#ifndef IMPL_NEW_EPG
  epg_filter_t item_info;
#else
  epg_prog_info_t prog_info = {0};
#endif
  u16 total_pg = 0;
  u32 total_evt = 0;

  //initialize global variables.
  time_get(&s_time, FALSE);
  
  ui_epg_init();
#ifndef IMPL_NEW_EPG
  ui_epg_start();
#else
  ui_epg_start(EPG_TABLE_SELECTE_SCH_ALL);
#endif

  ui_time_enable_heart_beat(TRUE); 
  group_num = sys_status_get_all_group_num();

  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();
  pg_id = (u16)para2;
  set_vepg_pg_id(pg_id);
  db_dvbs_get_pg_by_id(pg_id, &pg);
  total_pg = db_dvbs_get_count(ui_dbase_get_pg_view_id());

#ifndef IMPL_NEW_EPG
  item_info.service_id = (u16)pg.s_id;
  item_info.stream_id = (u16)pg.ts_id;
  item_info.orig_network_id = (u16)pg.orig_net_id;
  item_info.cont_level = 0;
  time_to_gmt(&s_time, &(item_info.start_time));
  s_time.hour = 23;
  s_time.minute = 59;
  s_time.second = 59;
  time_to_gmt(&s_time, &(item_info.end_time));

  epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID), &item_info, &total_evt);
#else
  prog_info.network_id = (u16)pg.orig_net_id;
  prog_info.ts_id      = (u16)pg.ts_id;
  prog_info.svc_id     = (u16)pg.s_id;
  time_to_gmt(&s_time, &(prog_info.start_time));
  s_time.hour = 23;
  s_time.minute = 59;
  s_time.second = 59;
  time_to_gmt(&s_time, &(prog_info.end_time));
  mul_epg_set_db_info(&prog_info);
  mul_epg_get_sch_event(&prog_info, (u16*)&total_evt);
#endif
  OS_PRINTF("total pg %d, total evt %d\n", total_pg, total_evt);
  if(0 == total_pg )
  {
      return   ERR_FAILURE;
  }
  //Create Menu
  if(0 == para1)
  	return ERR_FAILURE;
  x = btn_rec->x+btn_rec->w+2;
  if(btn_rec->y<(VEPG_MENU_H/2+VEPG_MENU_HGAY))
	  y = VEPG_MENU_HGAY;
  else 
	  y = btn_rec->y - VEPG_MENU_H/2;
  if((y+VEPG_MENU_H+VEPG_MENU_HGAY)>SCREEN_HEIGHT)
	  y = SCREEN_HEIGHT - VEPG_MENU_H-VEPG_MENU_HGAY;
  p_cont = fw_create_mainwin(ROOT_ID_VEPG, x, y, SCREEN_WIDTH-x, 
  						VEPG_MENU_H, ROOT_ID_XEXTEND, 0, OBJ_ATTR_ACTIVE, 0);
  ctrl_set_style(p_cont, STL_EX_WHOLE_HL);
  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_VEPR_CONT, 0, 0, SCREEN_WIDTH-x,
  									VEPG_MENU_H, p_cont, 0);
  ctrl_set_rstyle(p_cont, RSI_IGNORE,RSI_IGNORE, RSI_IGNORE);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_proc(p_cont, vepg_cont_proc);
  ctrl_set_keymap(p_cont, vepg_cont_keymap);

  //detail information
  p_detail = ctrl_create_ctrl(CTRL_CONT, IDC_VEPG_DETAIL, 
                             VEPG_LIST_CONTX, VEPG_LIST_CONTH+VEPG_MENU_GARY, 
                              VEPG_LIST_EVTX+VEPG_LIST_EVTW, VEPG_DETAIL_PGW, 
                              p_cont, 0);
  ctrl_set_rstyle(p_detail, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);

  p_short = ctrl_create_ctrl(CTRL_TEXT, IDC_VEPG_DETAIL_SHORT_EVT,
                             0, 0,
                             VEPG_LIST_EVTX+VEPG_LIST_EVTW, VEPG_DETAIL_PGW,
                             p_detail, 0);
  ctrl_set_rstyle(p_short, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_short, FSI_VEPG_INFO_MBOX, FSI_VEPG_INFO_MBOX, FSI_VEPG_INFO_MBOX);
  text_set_align_type(p_short, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_short, TEXT_STRTYPE_UNICODE);
  text_set_line_gap(p_short, VEPG_DETAIL_VGAP);
 
  //vepg list container.
  p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_VEPG_LIST_CONT, 
                                VEPG_LIST_CONTX, VEPG_LIST_CONTY, 
                                VEPG_LIST_EVTX+VEPG_LIST_EVTW, VEPG_LIST_CONTH, 
                                p_cont, 0);
  ctrl_set_rstyle(p_list_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_proc(p_list_cont, vepg_listcont_proc);
  ctrl_set_keymap(p_list_cont, vepg_listcont_keymap);
 
  //date list
  p_list_date = ctrl_create_ctrl(CTRL_LIST, IDC_VEPG_LIST_DATE, 
                                VEPG_LIST_DATEX, VEPG_LIST_DATEY, 
                                VEPG_LIST_DATEW, VEPG_LIST_DATEH, 
                                p_list_cont, 0);
  ctrl_set_rstyle(p_list_date, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);
  ctrl_set_keymap(p_list_date, vepg_list_date_keymap);
  ctrl_set_proc(p_list_date, vepg_list_date_proc);
  ctrl_set_mrect(p_list_date, 
                VEPG_LIST_HGAP, VEPG_LIST_VGAP, 
                (VEPG_LIST_DATEW - VEPG_LIST_HGAP), (VEPG_LIST_DATEH-VEPG_LIST_VGAP));
  list_set_item_interval(p_list_date, VEPG_LIST_VGAP);
  list_set_item_rstyle(p_list_date, &vepg_list_item_rstyle);
  list_enable_select_mode(p_list_date, TRUE);
  list_set_select_mode(p_list_date, LIST_SINGLE_SELECT);
  list_set_count(p_list_date, VEPG_LIST_PAGE_TOTAL, VEPG_LIST_PAGE);
  list_set_field_count(p_list_date, VEPG_LIST_FIELD, VEPG_LIST_PAGE);
  list_set_focus_pos(p_list_date, 0);
  list_select_item(p_list_date, 0);
  list_set_update(p_list_date, vepg_list_date_update, 0);

  for(i = 0; i < VEPG_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list_date, (u8)i, (u32)(vepg_date_attr[i].attr),
                        (u16)(vepg_date_attr[i].width),
                        (u16)(vepg_date_attr[i].left), (u8)(vepg_date_attr[i].top));
    list_set_field_rect_style(p_list_date, (u8)i, vepg_date_attr[i].rstyle);
    list_set_field_font_style(p_list_date, (u8)i, vepg_date_attr[i].fstyle);
  }
  vepg_list_date_update(p_list_date, list_get_valid_pos(p_list_date), VEPG_LIST_PAGE, 0);
 
  //evt list
  p_list_evt = ctrl_create_ctrl(CTRL_LIST, IDC_VEPG_LIST_EVT, 
                                VEPG_LIST_EVTX, VEPG_LIST_EVTY, 
                                VEPG_LIST_EVTW, VEPG_LIST_EVTH, 
                                p_list_cont, 0);
  ctrl_set_rstyle(p_list_evt, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);
  ctrl_set_keymap(p_list_evt, vepg_list_evt_keymap);
  ctrl_set_proc(p_list_evt, vepg_list_evt_proc);
  ctrl_set_mrect(p_list_evt, VEPG_LIST_HGAP, VEPG_LIST_VGAP, 
	  			(VEPG_LIST_EVTW - VEPG_LIST_HGAP), (VEPG_LIST_EVTH-VEPG_LIST_VGAP));
  list_set_item_interval(p_list_evt, VEPG_LIST_VGAP);
  list_set_item_rstyle(p_list_evt, &vepg_list_item_rstyle);
  list_enable_select_mode(p_list_evt, TRUE);
  list_set_select_mode(p_list_evt, LIST_SINGLE_SELECT);
  list_set_count(p_list_evt, (u16)total_evt, VEPG_LIST_PAGE);
  list_set_field_count(p_list_evt, VEPG_LIST_FIELD, VEPG_LIST_PAGE);
  list_set_focus_pos(p_list_evt, 0);
  list_select_item(p_list_evt, 0);
  list_set_update(p_list_evt, vepg_list_evt_update, 0);

  for(i = 0; i < VEPG_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list_evt, (u8)i, (u32)(vepg_evt_attr[i].attr),
                        (u16)(vepg_evt_attr[i].width),
                        (u16)(vepg_evt_attr[i].left), (u8)(vepg_evt_attr[i].top));
    list_set_field_rect_style(p_list_evt, (u8)i, vepg_evt_attr[i].rstyle);
    list_set_field_font_style(p_list_evt, (u8)i, vepg_evt_attr[i].fstyle);
  }
  vepg_list_evt_update(p_list_evt, list_get_valid_pos(p_list_evt), VEPG_LIST_PAGE, 0);
 
   vepg_set_detail(p_detail, FALSE);  
  
  ctrl_default_proc(p_list_date, MSG_GETFOCUS, 0, 0);
  ctrl_set_attr(p_list_evt, OBJ_ATTR_INACTIVE);
  ctrl_set_attr(p_list_date, OBJ_ATTR_INACTIVE);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
   
  return SUCCESS;
}

static RET_CODE on_vepg_ready(control_t *p_cont, u16 msg,
                              u32 para1, u32 para2)
{
  control_t *p_list_cont, *p_list_evt;
  dvbs_prog_node_t pg;
  u16 pg_id;

  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_VEPG_LIST_CONT);
  p_list_evt = ctrl_get_child_by_id(p_list_cont, IDC_VEPG_LIST_EVT);

  pg_id = get_vepg_pg_id();
  db_dvbs_get_pg_by_id(pg_id, &pg);

  #ifdef IMPL_NEW_EPG
  if(para1 == EVENT_ATTR_CURRENT_PROG)
  #else
  if((para1 == pg.s_id)
    && (para2 == pg.ts_id))
  #endif
  {
    vepg_evt_list_reset(p_list_cont, pg_id);
    vepg_list_evt_update(p_list_evt, list_get_valid_pos(p_list_evt), VEPG_LIST_PAGE, 0);
    ctrl_paint_ctrl(p_list_evt, TRUE);

    vepg_set_detail(ctrl_get_child_by_id(p_cont, IDC_VEPG_DETAIL), TRUE);
  }

  return SUCCESS;
}
 
static RET_CODE on_vepg_destory(control_t *p_cont, u16 msg,
                                u32 para1, u32 para2)
{
  ui_enable_chk_pwd(TRUE);

  return ERR_NOFEATURE;
}
/*
static RET_CODE on_vepg_pg_list_jump_date(control_t *p_list, u16 msg,
                                          u32 para1, u32 para2)
{
  control_t *p_list_cont, *p_list_date;

  p_list_cont = ctrl_get_parent(p_list);
  p_list_date = ctrl_get_child_by_id(p_list_cont, IDC_VEPG_LIST_DATE);
  //ctrl_default_proc(p_list, MSG_LOSTFOCUS, 0, 0);
  //list_draw_item_ext(p_list, list_get_focus_pos(p_list), TRUE);

  //ctrl_default_proc(p_list_date, MSG_GETFOCUS, 0, 0);
  //list_draw_item_ext(p_list_date, list_get_focus_pos(p_list_date), TRUE);
  ctrl_default_proc(p_list, MSG_LOSTFOCUS, 0, 0);
  ctrl_paint_ctrl(p_list, TRUE);
  ctrl_default_proc(p_list_date, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_list_date, TRUE);

//  gui_stop_roll(p_list_pg);

  return SUCCESS;
}
*/

static RET_CODE on_vepg_list_change_date(control_t *p_list, u16 msg,
                                         u32 para1, u32 para2)
{
  control_t *p_list_cont, *p_list_evt;
  u16 old_focus, new_focus = 0;

  p_list_cont = ctrl_get_parent(p_list);
  p_list_evt = ctrl_get_child_by_id(p_list_cont, IDC_VEPG_LIST_EVT);

  old_focus = list_get_focus_pos(p_list);
  list_unselect_item(p_list, old_focus);
  list_class_proc(p_list, msg, 0, 0);
  new_focus = list_get_focus_pos(p_list);
  list_select_item(p_list, new_focus);
  //list_draw_item_ext(p_list, old_focus, TRUE);
  //list_draw_item_ext(p_list, new_focus, TRUE);
  ctrl_paint_ctrl(p_list, TRUE);

  vepg_evt_list_reset(p_list_cont, sys_status_get_curn_group_curn_prog_id());
  list_set_focus_pos(p_list_evt, 0);
  list_select_item(p_list_evt, 0);
  vepg_list_evt_update(p_list_evt, list_get_valid_pos(p_list_evt), VEPG_LIST_PAGE, 0);
  ctrl_paint_ctrl(p_list_evt, TRUE);

  vepg_set_detail(ctrl_get_child_by_id(ctrl_get_parent(p_list_cont), IDC_VEPG_DETAIL), TRUE);
  return SUCCESS;
}

static RET_CODE on_vepg_date_list_jump_evt(control_t *p_list, u16 msg,
                                           u32 para1, u32 para2)
{
	control_t *p_list_cont, *p_list_evt;
	//  control_t *p_temp = NULL;

	p_list_cont = ctrl_get_parent(p_list);
	p_list_evt = ctrl_get_child_by_id(p_list_cont, IDC_VEPG_LIST_EVT);
	if(list_get_focus_pos(p_list_evt)>list_get_count(p_list_evt))
	{
		list_set_focus_pos(p_list_evt, list_get_current_pos(p_list_evt));
		list_select_item(p_list_evt, list_get_current_pos(p_list_evt));
		vepg_list_evt_update(p_list_evt, list_get_valid_pos(p_list_evt), VEPG_LIST_PAGE, 0);
	}
	// p_temp = ctrl_get_child_by_id(p_list_cont,IDC_VEPG_LIST_SBAR);
	if(list_get_count(p_list_evt) != 0)
	{
		ctrl_default_proc(p_list, MSG_LOSTFOCUS, 0, 0);
		ctrl_paint_ctrl(p_list, TRUE);

		ctrl_default_proc(p_list_evt, MSG_GETFOCUS, 0, 0);
		///	 list_set_scrollbar(p_list_evt, p_temp);
		ctrl_paint_ctrl(p_list_evt, TRUE);

	}

	return SUCCESS;
}


static RET_CODE on_vepg_list_change_evt(control_t *p_list, u16 msg,
                                        u32 para1, u32 para2)
{
  u16 old_focus, new_focus = 0;

  old_focus = list_get_focus_pos(p_list);
  list_unselect_item(p_list, old_focus);
  list_class_proc(p_list, msg, 0, 0);
  new_focus = list_get_focus_pos(p_list);
  list_select_item(p_list, new_focus);
  //list_draw_item_ext(p_list, old_focus, TRUE);
  //list_draw_item_ext(p_list, new_focus, TRUE);
  ctrl_paint_ctrl(p_list, TRUE);
  
  vepg_set_detail(ctrl_get_child_by_id(ctrl_get_parent(ctrl_get_parent(p_list)),
                                       IDC_VEPG_DETAIL), TRUE);

  return SUCCESS;
}


static RET_CODE on_vepg_evt_list_jump_date(control_t *p_list, u16 msg,
                                           u32 para1, u32 para2)
{
  control_t *p_list_cont, *p_list_date;

  p_list_cont = ctrl_get_parent(p_list);
  p_list_date = ctrl_get_child_by_id(p_list_cont, IDC_VEPG_LIST_DATE);

  ctrl_set_style(p_list, 0);
  ctrl_default_proc(p_list, MSG_LOSTFOCUS, 0, 0);
  ctrl_paint_ctrl(p_list, TRUE);

  ctrl_default_proc(p_list_date, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_list_date, TRUE);

  return SUCCESS;
}

static RET_CODE on_vepg_list_book(control_t *p_list, u16 msg,
                                  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_list_cont;
#ifndef IMPL_NEW_EPG
  evt_node_t evt_node = {0};
  evt_node_t *p_evt_node = &evt_node;
#else
  event_node_t *p_evt_node = NULL;
#endif
  utc_time_t one_min = {0};
  utc_time_t l_time = {0};
  utc_time_t temp_time = {0};

  p_list_cont = ctrl_get_parent(p_list);
  p_evt_node = vepg_get_curn_node(p_list_cont);

  if(p_evt_node != NULL)
  {
    book_node.pgid = sys_status_get_curn_group_curn_prog_id();
    book_node.book_mode = BOOK_TMR_ONCE;

    time_to_local(&(p_evt_node->start_time), &(book_node.start_time));
    memcpy(&(book_node.drt_time), &(p_evt_node->drt_time), sizeof(utc_time_t));

    one_min.minute = 1;

    time_get(&l_time, FALSE);
    memcpy(&temp_time, &(book_node.start_time), sizeof(utc_time_t));
    if(time_cmp(&l_time, &temp_time, FALSE) > 0)
    {
      time_add(&temp_time, &(book_node.drt_time));
      //if(time_cmp(&l_time, &temp_time, FALSE) < 0)
      //{
       // ui_close_all_mennus();
       // return SUCCESS;
      //}
    }

    if(time_cmp(&one_min, &book_node.drt_time, FALSE) > 0)
    {
      //less than one-min, can't be booked.
      ret = ERR_FAILURE;
    }
    else
    {
      ctrl_set_style(p_list, STL_EX_ALWAYS_HL);
      book_node.start_time.second = 0;
      book_node.drt_time.second = 0;
      ret = manage_open_menu(ROOT_ID_TIMER, (u32)(&book_node), FROM_VEPG_MENU);
    }
  }
  else
  {
    ui_close_all_mennus();
    return SUCCESS;
  }

  return ret;
}
/*
static RET_CODE on_vepg_list_select(control_t *p_list, u16 msg,
                                    u32 para1, u32 para2)
{
  gui_stop_roll(p_list);
  
  ui_time_enable_heart_beat(FALSE); 
  
  ui_close_all_mennus();
	//vepg_play_pg(select_focus);
	return SUCCESS;
}
*/
static RET_CODE on_vepg_reset(control_t *p_list_cont, u16 msg,
                              u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_list_date, *p_list_evt;
  utc_time_t g_time = {0};

  //utc_time_t t_time = {0};

  time_get(&g_time, TRUE);

   p_list_date = ctrl_get_child_by_id(p_list_cont, IDC_VEPG_LIST_DATE);
  p_list_evt = ctrl_get_child_by_id(p_list_cont, IDC_VEPG_LIST_EVT);

  list_set_focus_pos(p_list_date, 0);
  list_select_item(p_list_date, 0);
  vepg_list_date_update(p_list_date, list_get_valid_pos(p_list_date), VEPG_LIST_PAGE, 0);
  ctrl_paint_ctrl(p_list_date, TRUE);

  vepg_evt_list_reset(p_list_cont, sys_status_get_curn_group_curn_prog_id());
  list_set_focus_pos(p_list_evt, 0);
  list_select_item(p_list_evt, 0);
  vepg_list_evt_update(p_list_evt, list_get_valid_pos(p_list_evt), VEPG_LIST_PAGE, 0);
  ctrl_paint_ctrl(p_list_evt, TRUE);

  vepg_set_detail(ctrl_get_child_by_id(ctrl_get_parent(p_list_cont), IDC_VEPG_DETAIL), TRUE);

  return ret;
}

#ifndef IMPL_NEW_EPG
static RET_CODE on_pf_change(control_t *p_ctrl, u16 msg, 
                                  u32 para1, u32 para2)
{
  OS_PRINTF("\n ui_vepg do age limit recheck \n");
  ui_age_limit_recheck(para1, para2);

  return SUCCESS;
}
#endif
static RET_CODE vepg_get_focus(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	control_t *p_date_list, *p_evt_list;
	p_date_list = ctrl_get_child_by_id(p_cont, IDC_VEPG_LIST_DATE);
	p_evt_list = ctrl_get_child_by_id(p_cont, IDC_VEPG_LIST_EVT);
	ctrl_set_attr(p_date_list, OBJ_ATTR_ACTIVE);
	ctrl_set_attr(p_evt_list, OBJ_ATTR_ACTIVE);
    ctrl_default_proc(p_date_list, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(p_cont, TRUE);
	return SUCCESS;
}

static RET_CODE send_msg_to_extend_epg_btn(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	control_t *p_ctrl, *p_date_list, *p_evt_list;
	
	p_date_list = ctrl_get_child_by_id(p_cont, IDC_VEPG_LIST_DATE);
	p_evt_list = ctrl_get_child_by_id(p_cont, IDC_VEPG_LIST_EVT);
	if(OBJ_ATTR_INACTIVE == ctrl_get_attr(p_date_list))
	{
		p_ctrl = get_extend_cont1_ctrl();
		return ctrl_process_msg(p_ctrl, msg, 0, 0);
	}

	return ERR_FAILURE;
}
static RET_CODE on_vepg_date_list_jump_xextend(control_t *p_ctrl, u16 msg,
                                          u32 para1, u32 para2)
{
	control_t *p_evt_list, *p_date_list, *p_tmp;
	p_tmp = fw_find_root_by_id(ROOT_ID_VEPG);
	if(NULL == p_tmp)
		return ERR_FAILURE;
	p_tmp = ctrl_get_child_by_id(p_tmp, IDC_VEPR_CONT);
	p_tmp = ctrl_get_child_by_id(p_tmp, IDC_VEPG_LIST_CONT);
	p_date_list = ctrl_get_child_by_id(p_tmp, IDC_VEPG_LIST_DATE);
	p_evt_list = ctrl_get_child_by_id(p_tmp, IDC_VEPG_LIST_EVT);
	
	if((OBJ_ATTR_INACTIVE==ctrl_get_attr(p_evt_list))
					&&(OBJ_ATTR_INACTIVE==ctrl_get_attr(p_date_list)))
	{
		send_msg_to_extend_epg_btn(p_tmp, msg, 0,0 );
	}
	ctrl_set_attr(p_evt_list, OBJ_ATTR_INACTIVE);
	ctrl_set_attr(p_date_list, OBJ_ATTR_INACTIVE);
	ctrl_paint_ctrl(p_tmp, TRUE);
  return SUCCESS;
}
BOOL is_epg_have_focus(void)
{
	control_t *p_evt_list, *p_date_list, *p_tmp;
	p_tmp = fw_find_root_by_id(ROOT_ID_VEPG);
	if(NULL == p_tmp)
		return FALSE;
	p_tmp = ctrl_get_child_by_id(p_tmp, IDC_VEPR_CONT);
	p_tmp = ctrl_get_child_by_id(p_tmp, IDC_VEPG_LIST_CONT);
	p_date_list = ctrl_get_child_by_id(p_tmp, IDC_VEPG_LIST_DATE);
	p_evt_list = ctrl_get_child_by_id(p_tmp, IDC_VEPG_LIST_EVT);
	
	if((OBJ_ATTR_INACTIVE!=ctrl_get_attr(p_evt_list))
					||(OBJ_ATTR_INACTIVE!=ctrl_get_attr(p_date_list)))
	{
		return TRUE;
	}
	return FALSE;
}
/*
static RET_CODE on_vepg_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{   
  ui_close_all_mennus();

#ifdef IMPL_NEW_EPG
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#endif

  return SUCCESS;
}
*/

BEGIN_KEYMAP(vepg_cont_keymap, ui_comm_root_keymap)
ON_EVENT(V_KEY_EPG, MSG_EXIT)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(vepg_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(vepg_cont_proc, ui_comm_root_proc)
ON_COMMAND(MSG_EXIT, on_vepg_date_list_jump_xextend)
#ifdef IMPL_NEW_EPG
  ON_COMMAND(MSG_SCHE_READY, on_vepg_ready)
#else
  ON_COMMAND(MSG_EPG_READY, on_vepg_ready)
#endif
//ON_COMMAND(MSG_TIME_UPDATE, on_vepg_time_update)
ON_COMMAND(MSG_DESTROY, on_vepg_destory)
#ifndef IMPL_NEW_EPG
ON_COMMAND(MSG_PF_CHANGE, on_pf_change)
#endif
END_MSGPROC(vepg_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(vepg_listcont_keymap, NULL)
ON_EVENT(V_KEY_RECALL, MSG_RESET)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(vepg_listcont_keymap, NULL)

BEGIN_MSGPROC(vepg_listcont_proc, ui_comm_root_proc)
ON_COMMAND(MSG_RESET, on_vepg_reset)
ON_COMMAND(MSG_FOCUS_UP, send_msg_to_extend_epg_btn)
ON_COMMAND(MSG_FOCUS_DOWN, send_msg_to_extend_epg_btn)
ON_COMMAND(MSG_FOCUS_LEFT, send_msg_to_extend_epg_btn)
ON_COMMAND(MSG_SELECT, vepg_get_focus)
ON_COMMAND(MSG_FOCUS_RIGHT, vepg_get_focus)
END_MSGPROC(vepg_listcont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(vepg_list_date_keymap, NULL)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(vepg_list_date_keymap, NULL)

BEGIN_MSGPROC(vepg_list_date_proc, list_class_proc)
ON_COMMAND(MSG_FOCUS_UP, on_vepg_list_change_date)
ON_COMMAND(MSG_FOCUS_DOWN, on_vepg_list_change_date)
ON_COMMAND(MSG_PAGE_UP, on_vepg_list_change_date)
ON_COMMAND(MSG_PAGE_DOWN, on_vepg_list_change_date)
ON_COMMAND(MSG_FOCUS_LEFT, on_vepg_date_list_jump_xextend)
ON_COMMAND(MSG_FOCUS_RIGHT, on_vepg_date_list_jump_evt)
END_MSGPROC(vepg_list_date_proc, list_class_proc)

BEGIN_KEYMAP(vepg_list_evt_keymap, NULL)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_PAGE_UP,MSG_PAGE_DOWN )
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_UP)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(vepg_list_evt_keymap, NULL)

BEGIN_MSGPROC(vepg_list_evt_proc, list_class_proc)
ON_COMMAND(MSG_FOCUS_UP, on_vepg_list_change_evt)
ON_COMMAND(MSG_FOCUS_DOWN, on_vepg_list_change_evt)
ON_COMMAND(MSG_PAGE_UP, on_vepg_list_change_evt)
ON_COMMAND(MSG_PAGE_DOWN, on_vepg_list_change_evt)
ON_COMMAND(MSG_FOCUS_LEFT, on_vepg_evt_list_jump_date)
ON_COMMAND(MSG_SELECT, on_vepg_list_book)
END_MSGPROC(vepg_list_evt_proc, list_class_proc)
