/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_xsys_set.h"
#include "ui_xtime_set.h"

enum control_id
{
  IDC_XINVALID = 0,
  IDC_XGMT_USAGE,
  IDC_XGMT_OFFSET,
  IDC_XSUMMER_TIME,
  IDC_XDATE,
  IDC_XTIME,
  IDC_XINFO,
};

static u8 xgmt_info[XTIME_SET_GMT_OFFSET_NUM][XTIME_SET_GMT_OFFSET_CITY_LEN] = 
{
  " ",//-11:30
  
  " ",//-11:00

  " ",//-10:30
  
  "Honolulu",//-10:00
  
  " ",//-09:30
  
  "Anchorage",//-09:00
  
  " ",//-08:30
  
  "Vancouver, San Francisco, Seattle, Los Angeles",//-08:00
  
  " ",//-07:30
  
  "Phoenix, Edmonton, Denver",//-07:00
  
  " ",//-06:30
  
  "San Salvador, Mexico City, Winnipeg, Houston, Minneapolis, St. Paul, Guatemala, New Orleans, Chicago, Tegucigalpa, Montgomery, Managua",//-06:00
  
  " ",//-05:30
  
  "Lima, Kingston, Bogota, Indianapolis, Atlanta, Detroit, Havana, Miami, Toronto, Nassau, Washington DC, Ottawa, Philadelphia, New York, Montreal, Boston",//-05:00
  
  " ",//-04:30
  
  "Santiago, Santo Domingo, La Paz, Caracas, San Juan, Asunicion, Halifax",//-04:00
  
  " ",//-03:30
  
  "Buenos Aires, Montevideo, Brasilia, Sao Paulo, Rio de Janerio",//-03:00
  
  " ",//-02:30
  
  " ",//-02:00
  
  " ",//-01:30
  
  " ",//-01:00
  
  " ",//-00:30
  
  "Reykjavik, Casablanca, Lisbon, Dublin, London",//00:00

  " ",//00:30
  
  "Lagos, Algiers, Madrid, Barcelona, Paris, Brussels, Amsterdam, Geneva, Frankfurt, Oslo, Copenhagen, Rome, Berlin, Prague, Zagreb, Vienna, Stockholm, Cape Town, Budapest, Belgrade, Warsaw",//01:00
  
  " ",//01:30
  
  "Johannesburg, Harare, Sofia, Athens, Tallinn, Helsinki, Bucharest, Minsk, Istanbul, Kyiv, Odesa, Cairo, Khartoum, Ankara, Jerusalem, Beirut, Amman",//02:00

  " ",//02:30
  
  "Nairobi, Addis ababa, Aden, Riyadh, Antananarivo, Kuwait City, Moscow, Baghdad",//03:00

  "Tehran",//03:30

  " ",//04:00

  "Kabul",//04:30

  "Karachi, Tashkent, Islamabad, Lahore",//05:00

  "Mumbai, New Delhi, Kolkata",//05:30

  "Dhaka",//06:00

  "Yangon",//06:30

  "Bangkok, Hanoi, Jakarta",//07:00

  " ",//07:30

  "Kuala Lumpur, Singapore, Hong Kong, Perth, Beijing, Manila, Shanghai, Taipei",//08:00

  " ",//08:30

  "Seoul, Tokyo",//09:00

  "Darwin, Adelaide",//09:30

  "Melbourne, Canberra, Sydney, Brisbane, Vladivostok",//10:00

  " ",//10:30

  " ",//11:00

  " ",//11:30

  "Auckland, Suva, Kamchatka, Anadyr, Kiritimati, Stok",//12:00
}; 

static u16 xtime_set_cont_keymap(u16 key);
static RET_CODE xtime_set_cont_proc(control_t *p_ctrl, u16 msg, u32 para1,
                                     u32 para2);

static u16 xtime_set_select_keymap(u16 key);
static RET_CODE xtime_set_select_proc(control_t *p_ctrl, u16 msg, u32 para1,
                                     u32 para2);

static RET_CODE xtime_set_timedit_proc(control_t *p_ctrl, u16 msg, u32 para1,
                                     u32 para2);

static void xtime_set_gmt_usage(control_t *p_cont, BOOL is_paint)
{
  control_t *p_gmt_usage, *p_xgmt_offset;
  control_t *p_summer, *p_date, *p_time;
  u16 focus;

  p_gmt_usage = ctrl_get_child_by_id(p_cont, IDC_XGMT_USAGE);
  p_xgmt_offset = ctrl_get_child_by_id(p_cont, IDC_XGMT_OFFSET);
  p_summer = ctrl_get_child_by_id(p_cont, IDC_XSUMMER_TIME);
  p_date = ctrl_get_child_by_id(p_cont, IDC_XDATE);
  p_time = ctrl_get_child_by_id(p_cont, IDC_XTIME);

  focus = ui_comm_select_get_focus(p_gmt_usage);
  ui_comm_ctrl_update_attr(p_xgmt_offset, (BOOL)focus?TRUE:FALSE);
  ui_comm_ctrl_update_attr(p_summer, (BOOL)focus?TRUE:FALSE);
  ui_comm_ctrl_update_attr(p_date, (BOOL)focus?FALSE:TRUE);
  ui_comm_ctrl_update_attr(p_time, (BOOL)focus?FALSE:TRUE);

  if(is_paint)
  {
    ctrl_paint_ctrl(p_xgmt_offset, TRUE);
    ctrl_paint_ctrl(p_summer, TRUE);
    ctrl_paint_ctrl(p_date, TRUE);
    ctrl_paint_ctrl(p_time, TRUE);
  }
}

static RET_CODE xfill_date_time(BOOL is_paint)
{
  control_t *p_cont, *p_date, *p_time, *p_offset, *p_summer;
  utc_time_t gmt_time, local_time;
  //u8 asc_buf[32];
  
  p_cont = fw_find_root_by_id(ROOT_ID_XTIME_SET);
  //p_cont = ctrl_get_child_by_id(p_cont, IDC_COMM_ROOT_CONT);
  p_date = ctrl_get_child_by_id(p_cont, IDC_XDATE);
  p_time = ctrl_get_child_by_id(p_cont, IDC_XTIME);

  if(ui_comm_timedit_get_is_on_edit(p_date) || ui_comm_timedit_get_is_on_edit(p_time))
  {
      return ERR_FAILURE;
  }

  time_get(&gmt_time, TRUE);
  time_to_local(&gmt_time, &local_time);

  ui_comm_timedit_set_time(p_date, &local_time);
  ui_comm_timedit_set_time(p_time, &local_time);

  if(is_paint)
  {
    p_offset = ctrl_get_child_by_id(p_cont, IDC_XGMT_OFFSET);
    p_offset = ctrl_get_child_by_id(p_offset, IDC_COMM_CTRL);
    p_summer = ctrl_get_child_by_id(p_cont, IDC_XSUMMER_TIME);
    p_summer = ctrl_get_child_by_id(p_summer, IDC_COMM_CTRL);

    if(!(p_offset->priv_attr & CBOX_STATUS_LIST))
    {
      ui_comm_ctrl_paint_ctrl(p_time, TRUE);

      if(!(p_summer->priv_attr & CBOX_STATUS_LIST))
      {
        ui_comm_ctrl_paint_ctrl(p_date, TRUE);
      }
    }
  }
  return SUCCESS;
}

static RET_CODE fill_xgmt_offset(control_t *ctrl, u16 focus, u16 *p_str,
                        u16 max_length)
{
  u8 asc_buf[32];
  
  if(focus >= 23)//+
  {
    sprintf(asc_buf, "UTC + %.2d:%.2d", (focus - 23)/2, (focus - 23)%2*30);
  }
  else//-
  {
    sprintf(asc_buf, "UTC - %.2d:%.2d", (23 - focus)/2, (23 - focus)%2*30);

  }

  str_nasc2uni(asc_buf, p_str, max_length);

  return SUCCESS;
}

static RET_CODE fill_xgmt_info(control_t *p_ctrl, u8 *p_str, BOOL is_paint)
{
  OS_PRINTF("fill gmt info, is_paint %d \n", is_paint);
  text_set_content_by_ascstr(p_ctrl, p_str);

  if(is_paint)
  {
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
  return SUCCESS;
}

RET_CODE open_xtime_set(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[XTIME_SET_ITEM_CNT], *p_info;
  u8 i, j;
  time_set_t xtime_set;
//  utc_time_t time = {{0}};

#ifndef SPT_SUPPORT
  u16 stxt [XTIME_SET_ITEM_CNT] =
  { IDS_GMT_USAGE, IDS_GMT_OFFSET, IDS_SUMMER_TIME,
    IDS_DATE,      IDS_TIME };
  u8 opt_cnt [XTIME_SET_ITEM_CNT] = { 2, 10, 2, 0, 0 };
  u16 opt_data[XTIME_SET_ITEM_CNT][3] = {
    { IDS_OFF, IDS_ON },
    {     0 },
    { IDS_OFF, IDS_ON },
    {     0 },
    {     0 },
  };
  u8 droplist_page[XTIME_SET_ITEM_CNT] = {2, 5, 2, 0, 0};
  u16 y;
#endif

#ifndef SPT_SUPPORT
p_cont = fw_create_mainwin(ROOT_ID_XTIME_SET,
							   XTIME_SET_X, XTIME_SET_Y,
							   XTIME_SET_W, XTIME_SET_H,
                             ROOT_ID_XSYS_SET, 0, OBJ_ATTR_ACTIVE, 0);
  ctrl_set_rstyle(p_cont,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, xtime_set_cont_keymap);
  ctrl_set_proc(p_cont, xtime_set_cont_proc);

  y = XTIME_SET_ITEM_Y;
  for (i = 0; i < XTIME_SET_ITEM_CNT; i++)
  {
    switch (i + 1)
    {
      // edit
      case IDC_XDATE:
      case IDC_XTIME:
        p_ctrl[i] = ui_comm_timedit_create(p_cont, (u8)(IDC_XGMT_USAGE + i),
                                             XTIME_SET_ITEM_X, y,
                                             XTIME_SET_ITEM_LW,
                                             XTIME_SET_ITEM_RW);
        ui_comm_timedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], xtime_set_timedit_proc);
        if((i + 1) == IDC_XDATE)
        {
          ui_comm_timedit_set_param(p_ctrl[i], 0, TBOX_ITEM_YEAR, TBOX_YEAR | TBOX_MONTH | TBOX_DAY, 
            TBOX_SEPARATOR_TYPE_UNICODE, 18);
          ui_comm_timedit_set_separator_by_ascchar(p_ctrl[i], TBOX_ITEM_YEAR, '-');
          ui_comm_timedit_set_separator_by_ascchar(p_ctrl[i], TBOX_ITEM_MONTH, '-');		  
        }
        else
        {
          ui_comm_timedit_set_param(p_ctrl[i], 0, TBOX_ITEM_HOUR, TBOX_HOUR| TBOX_MIN, 
            TBOX_SEPARATOR_TYPE_UNICODE, 18);
          ui_comm_timedit_set_separator_by_ascchar(p_ctrl[i], TBOX_ITEM_HOUR, ':');
        }
        break;

        // select
      default:
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_XGMT_USAGE + i),
                                            XTIME_SET_ITEM_X, y,
                                            XTIME_SET_ITEM_LW,
                                            XTIME_SET_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_keymap(p_ctrl[i], xtime_set_select_keymap);
        ui_comm_ctrl_set_proc(p_ctrl[i], xtime_set_select_proc);
        if(i == 1)
        {
          ui_comm_select_set_param(p_ctrl[i], TRUE, 
            CBOX_WORKMODE_DYNAMIC, XTIME_SET_GMT_OFFSET_NUM, 0, fill_xgmt_offset);
        }
        else
        {
          ui_comm_select_set_param(p_ctrl[i], TRUE,
                                     CBOX_WORKMODE_STATIC, opt_cnt[i],
                                     CBOX_ITEM_STRTYPE_STRID, NULL);
          for (j = 0; j < opt_cnt[i]; j++)
          {
            ui_comm_select_set_content(p_ctrl[i], j, opt_data[i][j]);
          }
        }
        ui_comm_select_create_droplist(p_ctrl[i], droplist_page[i]);
        break;
    }

    ctrl_set_related_id(p_ctrl[i],
                        0,                                     /* left */
                        (u8)((i - 1 +
                              XTIME_SET_ITEM_CNT) %
                             XTIME_SET_ITEM_CNT + 1),           /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % XTIME_SET_ITEM_CNT + 1));/* down */

    y += XTIME_SET_ITEM_H + XTIME_SET_ITEM_V_GAP;
  }

  p_info = ctrl_create_ctrl(CTRL_TEXT, IDC_XINFO, 
    20, 
    (y+XTIME_SET_ITEM_H + XTIME_SET_ITEM_V_GAP) + 50, 
    XTIME_SET_W-40,
   ( XTIME_SET_H -(y+XTIME_SET_ITEM_H + XTIME_SET_ITEM_V_GAP))/2, p_cont, 0);
  text_set_content_type(p_info, TEXT_STRTYPE_UNICODE);
  text_set_align_type(p_info, STL_CENTER | STL_VCENTER);
  ctrl_set_rstyle(p_info, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
  text_set_font_style(p_info, FSI_COMM_TXT_SH, FSI_COMM_TXT_HL, FSI_COMM_TXT_GRAY);
  
  sys_status_get_time(&xtime_set);
    
  ui_comm_select_set_focus(p_ctrl[0], (u16)xtime_set.gmt_usage);//usage
  ui_comm_select_set_focus(p_ctrl[1], (u16)xtime_set.gmt_offset);//offset
  ui_comm_select_set_focus(p_ctrl[2], (u16)xtime_set.summer_time);//summer
  if(xtime_set.gmt_usage)
  {
    ctrl_set_sts(p_info, OBJ_STS_SHOW);
  }
  else
  {
    ctrl_set_sts(p_info, OBJ_STS_HIDE);
  }

#else
#endif
  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */

  xfill_date_time(TRUE);
  
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
  
  return SUCCESS;
}

static RET_CODE on_xtime_set_time_update(control_t *p_ctrl, u16 msg, u32 para1,
                                     u32 para2)
{
  xfill_date_time(TRUE);

  return SUCCESS;
}

static RET_CODE on_xtime_set_select_change(control_t *p_ctrl, u16 msg, u32 para1,
                                     u32 para2)
{
	RET_CODE ret = SUCCESS;
	time_set_t ss_time;
	u16 focus;
	control_t *p_cont;
  BOOL is_onfocus = ctrl_is_onfocus(p_ctrl);
  
	// before switch

  ret = cbox_class_proc(p_ctrl, msg, para1, para2);

	// after switch 
  p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));	
	sys_status_get_time(&ss_time);
	switch (ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
	{
  	case IDC_XGMT_USAGE:
  		focus = cbox_static_get_focus(p_ctrl);
  		ss_time.gmt_usage = (u8)focus;
  		xtime_set_gmt_usage(p_cont, is_onfocus);
      
              if(ss_time.gmt_usage)
              {
                ctrl_set_sts(ctrl_get_child_by_id(p_cont, IDC_XINFO), OBJ_STS_SHOW);
                fill_xgmt_info(ctrl_get_child_by_id(p_cont, IDC_XINFO), xgmt_info[ss_time.gmt_offset], is_onfocus);
        //        ui_time_req_tdt();
              }
              else
              {
                fill_xgmt_info(ctrl_get_child_by_id(p_cont, IDC_XINFO), " ", is_onfocus);
                ctrl_set_sts(ctrl_get_child_by_id(p_cont, IDC_XINFO), OBJ_STS_HIDE);
              }
              ctrl_paint_ctrl(p_cont, TRUE);
  		break;
  	case IDC_XGMT_OFFSET:
  		focus = cbox_dync_get_focus(p_ctrl);
  		ss_time.gmt_offset = (u8)focus;
      if(ss_time.gmt_usage)
      {
      ctrl_set_sts(p_cont, OBJ_STS_SHOW);
        fill_xgmt_info(ctrl_get_child_by_id(p_cont, IDC_XINFO), xgmt_info[focus], is_onfocus);
      }
  		break;
  	case IDC_XSUMMER_TIME:
  		focus = cbox_static_get_focus(p_ctrl);
  		ss_time.summer_time = (u8)focus;
  		break;
  	default:
  		MT_ASSERT(0);
  		return ERR_FAILURE;
	}

	sys_status_set_time(&ss_time);
	sys_status_save();      

  sys_status_set_time_zone();

  //local time had been changed, so check book node again.
  book_delete_overdue_node();

  xfill_date_time(is_onfocus);
  
  return ret;
}

static RET_CODE on_xtime_set_timedit_unselect(control_t *p_ctrl,
  u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont, *p_date, *p_time;
  utc_time_t loc_time = {0};
  utc_time_t gmt_time = {0};

  p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));
  p_date = ctrl_get_child_by_id(p_cont, IDC_XDATE);
  p_time = ctrl_get_child_by_id(p_cont, IDC_XTIME);

  ui_comm_timedit_get_time(p_date, &loc_time);
  ui_comm_timedit_get_time(p_time, &loc_time);

  time_to_gmt(&loc_time, &gmt_time);

  sys_status_set_utc_time(&gmt_time);
  time_set(&gmt_time);
  
  //local time had been changed, so check book node again.
  book_delete_overdue_node();
  
  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(xtime_set_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(xtime_set_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(xtime_set_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_TIME_UPDATE, on_xtime_set_time_update)
END_MSGPROC(xtime_set_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(xtime_set_select_keymap, ui_comm_select_keymap)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(xtime_set_select_keymap, ui_comm_select_keymap)

BEGIN_MSGPROC(xtime_set_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_xtime_set_select_change)
END_MSGPROC(xtime_set_select_proc, cbox_class_proc)

BEGIN_MSGPROC(xtime_set_timedit_proc, ui_comm_time_proc)
  ON_COMMAND(MSG_UNSELECT, on_xtime_set_timedit_unselect)
  ON_COMMAND(MSG_CHANGED, on_xtime_set_timedit_unselect)
END_MSGPROC(xtime_set_timedit_proc, ui_comm_time_proc)
