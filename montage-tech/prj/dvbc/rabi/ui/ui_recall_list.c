/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_recall_list.h"

enum recall_list_control_id
{
  IDC_RECALL_LIST_LIST = 1,
  IDC_RECALL_LIST_SBAR,
  IDC_RECALL_LIST_HELP,
};

static u8 slist_mode;   //to saved the current played pg mode, for restore view when exit the recall list.
static u16 slist_group; //to saved the current played pg group, for restore view when exit the recall list.
static u8 slist_hkey = 0;

static list_xstyle_t rlist_item_rstyle =
{
	RSI_PBACK,
	RSI_PBACK,
	RSI_OPT_BTN_H,
	RSI_ITEM_8_BG, //for bug 37665
	RSI_OPT_BTN_H,
};

static list_xstyle_t rlist_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLACK,
  FSI_WHITE,
};


static list_xstyle_t rlist_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t rlist_attr[RECALL_LIST_FIELD_NUM] =
{
  { LISTFIELD_TYPE_DEC,
    48, 0, 0, &rlist_field_rstyle,  &rlist_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    160, 48, 0, &rlist_field_rstyle,  &rlist_field_fstyle },
  { LISTFIELD_TYPE_ICON,
    28, 208, 0, &rlist_field_rstyle,  &rlist_field_fstyle},
  { LISTFIELD_TYPE_ICON,
    28, 236, 0, &rlist_field_rstyle,  &rlist_field_fstyle},
  { LISTFIELD_TYPE_ICON,
    28, 264, 0, &rlist_field_rstyle,  &rlist_field_fstyle},
};

static play_info_t recall_play_cnxt[MAX_PLAY_CNXT_COUNT] = 
{
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE }
};

static comm_help_data_t help_data = //help bar data
{
  2,
  2,
  {IDS_SELECT, 
   IDS_EXIT},
  {IM_HELP_OK,
   IM_HELP_EXIT},
};

u16 recall_list_keymap(u16 key);
RET_CODE recall_list_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

#if 0
static void slist_set_list_item_mark(control_t *p_list, u16 pos, u16 pg_id)
{
  u16 i, img[4];
  u8 view_id = ui_dbase_get_pg_view_id();

  img[0] = ui_dbase_pg_is_fav(view_id, pos) ? IM_TV_FAV : 0;
  img[1] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, 0)? IM_TV_LOCK : 0;

  img[2] = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_SKP, FALSE)? IM_TV_SKIP: 0;

  for (i = 0; i < 3; i++)
  {
    list_set_field_content_by_icon(p_list, pos, (u8)(i + 2), img[i]);
  }
}
#endif

static u16 ui_check_recall_prog_hide(u16 cnt)
{
  u16 pg_id;
  u16 i, j = 0;
  dvbs_prog_node_t curn_prog;
  play_info_t recall_play_cnxt_tmp[MAX_PLAY_CNXT_COUNT] = 
  {
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE }
  };
  
   for (i = 0; i < RECALL_LIST_PAGE && i < cnt; i++)
   {
     pg_id = recall_play_cnxt[i].prog_id;
     db_dvbs_get_pg_by_id(pg_id, &curn_prog);
     if(!curn_prog.hide_flag)
     {
      memcpy((void *)&recall_play_cnxt_tmp[j],(void *)&recall_play_cnxt[i],sizeof(play_info_t));
      j++;
     } 
   }
   memcpy((void *)&recall_play_cnxt[0],(void *)&recall_play_cnxt_tmp[0],sizeof(play_info_t)*MAX_PLAY_CNXT_COUNT);
   return j;
}

static u16 ui_get_recall_pg_cont(void)
{
  u16 pg_id = 0;
  u16 i, j = 0;
   for (i = 0; i < RECALL_LIST_PAGE; i++)
   {
     pg_id = recall_play_cnxt[i].prog_id;
     if(pg_id != INVALIDID)    
     {
       j++;
     }
   }
   return j;
}
//load data from database, will be modified later
static RET_CODE load_recall_data(control_t* p_list, u16 start, u16 size,
                                u32 context)
{
  dvbs_prog_node_t curn_prog;
  u16 i, j;
  u16 uni_str[32];
  u16 pg_id, cnt = list_get_count(p_list);
  u16 img[3];

  for (i = start; i < size && i < cnt; i++)
  {
    pg_id = recall_play_cnxt[i].prog_id;
    db_dvbs_get_pg_by_id(pg_id, &curn_prog);

    // NO.
    list_set_field_content_by_dec(p_list, i, 0, (u16)(i + 1)); //program number in list

    // NAME
    ui_dbase_get_full_prog_name(&curn_prog, uni_str, 31);
    list_set_field_content_by_unistr(p_list, i, 1, uni_str); //pg name

    //set marks
    //slist_set_list_item_mark(p_list, i, pg_id);
    img[0] = (curn_prog.fav_grp_flag & ((1 << MAX_FAV_GRP) - 1)) != 0 ? IM_TV_FAV : 0;
    img[1] = curn_prog.lck_flag == TRUE ? IM_TV_LOCK : 0;
    img[2] = curn_prog.skp_flag == TRUE ? IM_TV_SKIP : 0;
    for (j = 0; j < 3; j++)
    {
      list_set_field_content_by_icon(p_list, i, (u8)(j + 2), img[j]);
    }
  }
  return SUCCESS;
}

static void play_in_recall_list(u16 focus)
{
  u16 view_type;
  u32 context;
  play_info_t play_info = ui_get_curplayinfo();
  
  if (focus < ui_get_play_cnxt_count())
  {
    if ((recall_play_cnxt[focus].group != play_info.group)
      || (recall_play_cnxt[focus].mode != play_info.mode))
    {
      sys_status_set_curn_group(recall_play_cnxt[focus].group);
      sys_status_set_curn_prog_mode(recall_play_cnxt[focus].mode);
      sys_status_save();

      sys_status_get_curn_view_info(&view_type, &context);
      ui_dbase_set_pg_view_id(ui_dbase_create_view(view_type, context, NULL));
    }

    ui_play_prog(recall_play_cnxt[focus].prog_id, FALSE);
  }
}

static RET_CODE on_slist_select(control_t *p_list,
  u16 msg, u32 para1, u32 para2)
{
  ui_close_all_mennus();

  return SUCCESS;
}

static RET_CODE on_slist_change_focus(control_t *p_list,
  u16 msg, u32 para1, u32 para2)
{
  RET_CODE ret;

  ret = list_class_proc(p_list, msg, 0, 0);

  play_in_recall_list(list_get_focus_pos(p_list));

  return SUCCESS;
}

//  manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
//  manage_open_menu(ROOT_ID_RECALL_LIST, 0, 0);
RET_CODE open_recall_list(u32 para1, u32 para2)
{
  control_t *p_cont, *p_title, *p_list, *p_sbar;
  u16 i, total;
  u16 all_num = 0;
  u16 pg_id;
  u32 context;
  u8 view_id = 0xFF;
  u8 recall_pg_cont = 0;

  if((ui_get_play_prg_type() == NET_PRG_TYPE)
  ||(ui_get_play_prg_type() == CAMERA_PRG_TYPE))
    return SUCCESS;

  slist_mode = sys_status_get_curn_prog_mode();

  {
    slist_hkey = V_KEY_OK;
    view_id = ui_dbase_get_pg_view_id();
    slist_group = sys_status_get_curn_group();
    all_num = sys_status_get_all_group_num();
  }
  
  total = ui_get_play_cnxt_count();
  ui_get_play_cnxt_copy((void *) recall_play_cnxt);
  sys_status_get_curn_prog_in_group(slist_group, slist_mode, &pg_id, &context);

  total = ui_check_recall_prog_hide(total);

  recall_pg_cont =  ui_get_recall_pg_cont();

  if(recall_pg_cont == 0)
  {
    return ERR_FAILURE;
  }
#if 0
  if (0 == total)
  {
    ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_TV_PROG, NULL, 0);
    return ERR_FAILURE;
  }
#endif

  sys_status_set_curn_group(slist_group);
  sys_status_set_curn_prog_mode(slist_mode);
  sys_status_save();

  ui_dbase_set_pg_view_id(view_id);

  p_cont = fw_create_mainwin((u8)(ROOT_ID_RECALL_LIST),
                                  RECALL_LIST_MENU_CONT_X, RECALL_LIST_MENU_CONT_Y,
                                  RECALL_LIST_MENU_WIDTH, RECALL_LIST_MENU_HEIGHT,
                                  0, 0, OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  //IDS_RECALL_LIST

  ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);

  p_title = ctrl_create_ctrl(CTRL_TEXT, 0, 
                          RECALL_LIST_TITLE_X, RECALL_LIST_TITLE_Y, 
                          RECALL_LIST_TITLE_W, RECALL_LIST_TITLE_H,
                          p_cont, 0);
  text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_RECALL_LIST);//IDS_RECALL_LIST

  //create recall list
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_RECALL_LIST_LIST,
                        RECALL_LIST_LIST_X, RECALL_LIST_LIST_Y,
                        RECALL_LIST_LIST_W, RECALL_LIST_LIST_H, 
                        p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ctrl_set_keymap(p_list, recall_list_keymap);
  ctrl_set_proc(p_list, recall_list_proc);

  ctrl_set_mrect(p_list, RECALL_LIST_MID_L, RECALL_LIST_MID_T,
                    RECALL_LIST_MID_W+RECALL_LIST_MID_L, RECALL_LIST_MID_H+RECALL_LIST_MID_T);
  list_set_item_interval(p_list, RECALL_LIST_ITEM_V_GAP);
  list_set_item_rstyle(p_list, &rlist_item_rstyle);
  list_set_count(p_list, total, RECALL_LIST_PAGE);
  list_set_field_count(p_list, RECALL_LIST_FIELD_NUM, RECALL_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  list_set_update(p_list, load_recall_data, 0);

  for (i = 0; i < RECALL_LIST_FIELD_NUM; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(rlist_attr[i].attr), (u16)(rlist_attr[i].width),
                        (u16)(rlist_attr[i].left), (u8)(rlist_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, rlist_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, rlist_attr[i].fstyle);
  }

  //create scroll bar
  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_RECALL_LIST_SBAR,
                          RECALL_LIST_SBAR_X, RECALL_LIST_SBAR_Y,
                          RECALL_LIST_SBAR_W, RECALL_LIST_SBAR_H,
                          p_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_RLIST_SBAR, RSI_RLIST_SBAR, RSI_RLIST_SBAR);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_RLIST_SBAR_MID, RSI_RLIST_SBAR_MID,
                     RSI_RLIST_SBAR_MID);
  ctrl_set_mrect(p_sbar, 0, RECALL_LIST_SBAR_VERTEX_GAP,
                    RECALL_LIST_SBAR_W, RECALL_LIST_SBAR_H - RECALL_LIST_SBAR_VERTEX_GAP);
  list_set_scrollbar(p_list, p_sbar);

  load_recall_data(p_list, list_get_valid_pos(p_list), RECALL_LIST_PAGE, 0);

  //create help
  ui_comm_help_create_for_pop_dlg(&help_data, p_cont);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  play_in_recall_list(0);

  return SUCCESS;
 }

BEGIN_KEYMAP(recall_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
END_KEYMAP(recall_list_keymap, NULL)

BEGIN_MSGPROC(recall_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_slist_select)
  ON_COMMAND(MSG_FOCUS_UP, on_slist_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_slist_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_slist_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_slist_change_focus)
END_MSGPROC(recall_list_proc, list_class_proc)

