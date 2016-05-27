/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_book_list.h"
#include "ui_timer.h"
/* others */
enum control_id
{
  IDC_INVALID = 0,
  IDC_BLIST_LIST,
  IDC_BLIST_SBAR,
};


static list_xstyle_t list_item_rstyle =
{
  RSI_PBACK,
  RSI_COMM_CONT_SH,
  RSI_SELECT_F,
  RSI_SELECT_F,
  RSI_SELECT_F,
};

static list_xstyle_t list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLACK,
  FSI_WHITE,
};


static list_xstyle_t list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t list_field_attr[BLIST_FIELD_NUM] =
{
  { LISTFIELD_TYPE_DEC,
    40, 0, 0, &list_field_rstyle, &list_field_fstyle },//no.
  { LISTFIELD_TYPE_STRID | STL_LEFT | STL_VCENTER,
    80, 40, 0, &list_field_rstyle, &list_field_fstyle },//mode
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    180, 120, 0, &list_field_rstyle, &list_field_fstyle },//name
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    400, 300, 0, &list_field_rstyle, &list_field_fstyle },//time
};

//static BOOL g_is_modified;

RET_CODE book_list_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 book_list_keymap(u16 key);
RET_CODE book_list_proc(control_t * p_list, u16 msg, u32 para1, u32 para2);

static RET_CODE book_list_update(control_t* ctrl, u16 start, u16 size, 
                                      u32 context)
{
  u8 asc_str[32];
  u16 i;
  u16 uni_str[32];
  u16 pos, cnt = list_get_count(ctrl);
  book_pg_t node;
  dvbs_prog_node_t pg;  
  u16 timer_type [4] = {IDS_OFF, IDS_ONCE, IDS_DAILY, IDS_WEEKLY};
  utc_time_t tmp_time;

  for (i = 0; i < size; i++)
  {
    pos = i + start;
    if (pos < cnt)
    {
      //no.
      list_set_field_content_by_dec(ctrl, pos, 0,
                                    (u16)(pos + 1 ));

      sys_status_get_book_node((u8)pos, &node);
      OS_PRINTF("node.book_mode[%d], node.pgid[%d]\n", node.book_mode, node.pgid);
      
      //timer type
      list_set_field_content_by_strid(ctrl, pos, 1,
                                    timer_type[node.book_mode]);

      //service name
      if(node.book_mode)
      {
        db_dvbs_get_pg_by_id(node.pgid, &pg);
        ui_dbase_get_full_prog_name(&pg, uni_str, 31);
        list_set_field_content_by_unistr(ctrl, (u16)(start + i), 2, uni_str);
      }    
      else
      {
        list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 2, " "); 
      }
      
      //time
      if(node.book_mode)
      {
        //tmp_time.value = node.start_time.value;
        memcpy(&tmp_time, &(node.start_time), sizeof(utc_time_t));
        
        time_add(&tmp_time, &(node.drt_time));
        
        sprintf(asc_str, "%.2d/%.2d/%.4d %.2d:%.2d-%.2d:%.2d", 
          node.start_time.day, node.start_time.month, (node.start_time.year),
          node.start_time.hour, node.start_time.minute, tmp_time.hour, tmp_time.minute);
        
        list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 3, asc_str);   
      }
      else
      {
        list_set_field_content_by_ascstr(ctrl, (u16)(start + i), 3, " ");   
      }
    }
  }
  return SUCCESS;
}

static RET_CODE on_book_list_update(control_t *p_cont,
  u16 msg, u32 para1, u32 para2)
{  
  control_t *p_list;

  p_list = ctrl_get_child_by_id(p_cont, IDC_BLIST_LIST);
  MT_ASSERT(p_list != NULL);

  book_list_update(p_list, list_get_valid_pos(p_list), BLIST_PAGE, 0);
  //ctrl_paint_ctrl(p_list, TRUE);
  list_draw_item_ext(p_list, list_get_focus_pos(p_list), TRUE);

  return SUCCESS;
}

static RET_CODE on_book_list_select(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
  u16 focus = list_get_focus_pos(p_list);
  return manage_open_menu(ROOT_ID_TIMER, focus, FROM_TIMER_SETMENU);
}

static RET_CODE on_book_list_destroy(control_t *p_list, 
  u16 msg, u32 para1, u32 para2)
{
#if ENABLE_TTX_SUBTITLE
  osd_set_t osd_set = {0};
  sys_status_get_osd_set(&osd_set);			  
  if(osd_set.enable_subtitle)
  {
    	ui_set_subt_display(TRUE);
  }
#endif
   return ERR_NOFEATURE;
}
RET_CODE open_book_list(u32 para1, u32 para2)
{
  control_t *p_cont, *p_list, *p_sbar;
  u16 i;
  list_field_attr_t *p_attr = list_field_attr;
#if ENABLE_TTX_SUBTITLE
  osd_set_t osd_set = {0};
  sys_status_get_osd_set(&osd_set);			  
  if(osd_set.enable_subtitle)
  {
    	ui_set_subt_display(FALSE);
  }
#endif
  /* pre-create */

#ifndef SPT_SUPPORT
  p_cont =ui_comm_right_root_create(ROOT_ID_BOOK_LIST, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, book_list_cont_proc);

  //create program list
  p_list =
    ctrl_create_ctrl(CTRL_LIST, IDC_BLIST_LIST,
                     BLIST_LIST_X, BLIST_LIST_Y,
                     BLIST_LIST_W, BLIST_LIST_H,
                     p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_BLIST_LIST,
                  RSI_BLIST_LIST, RSI_BLIST_LIST);
  ctrl_set_keymap(p_list, book_list_keymap);
  ctrl_set_proc(p_list, book_list_proc);

  ctrl_set_mrect(p_list, BLIST_MID_X, BLIST_MID_Y,
                    BLIST_MID_W, BLIST_MID_H);
  list_set_item_interval(p_list, BLIST_ITEM_V_GAP);
  list_set_item_rstyle(p_list, &list_item_rstyle);
  list_set_count(p_list, MAX_BOOK_PG, BLIST_PAGE);
  list_set_field_count(p_list, BLIST_FIELD_NUM, BLIST_PAGE);
  list_set_update(p_list, book_list_update, 0);

  for (i = 0; i < BLIST_FIELD_NUM; i++)
  {
    list_set_field_attr(p_list, (u8)i, p_attr->attr,
                        p_attr->width, p_attr->left, p_attr->top);
    list_set_field_rect_style(p_list, (u8)i, p_attr->rstyle);
    list_set_field_font_style(p_list, (u8)i, p_attr->fstyle);

    p_attr++;
  }

  //create scroll bar
  p_sbar = ctrl_create_ctrl(
    CTRL_SBAR, IDC_BLIST_SBAR,
    BLIST_LIST_X + BLIST_LIST_W +
    BLIST_SBAR_H_GAP,
    BLIST_LIST_Y + BLIST_SBAR_V_GAP,
    BLIST_SBAR_WIDTH, BLIST_SBAR_HEIGHT,
    p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_BLIST_SBAR_BG,
                  RSI_BLIST_SBAR_BG, RSI_BLIST_SBAR_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_BLIST_SBAR_MID,
                     RSI_BLIST_SBAR_MID, RSI_BLIST_SBAR_MID);
  ctrl_set_mrect(p_sbar, 0, BLIST_SBAR_VERTEX_GAP,
                    BLIST_SBAR_WIDTH,
                    BLIST_SBAR_HEIGHT - BLIST_SBAR_VERTEX_GAP);
  list_set_scrollbar(p_list, p_sbar);

#else
#endif
  list_set_focus_pos(p_list, 0);
  book_list_update(p_list, list_get_valid_pos(p_list), BLIST_PAGE, 0);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

BEGIN_MSGPROC(book_list_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_TIMER_UPDATE, on_book_list_update)
END_MSGPROC(book_list_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(book_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)  
END_KEYMAP(book_list_keymap, NULL)

BEGIN_MSGPROC(book_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_book_list_select)
  ON_COMMAND(MSG_DESTROY, on_book_list_destroy)
END_MSGPROC(book_list_proc, list_class_proc)


