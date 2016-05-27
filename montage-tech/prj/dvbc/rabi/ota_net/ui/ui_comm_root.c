/****************************************************************************

 ****************************************************************************/

#include "ui_common.h"

#define EN_TITLE_HELP


/*********************************************************************
 * COMMON ROOT
 *********************************************************************/

u16 comm_mask_keymap(u16 key);

RET_CODE comm_mask_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);


// ON COMM ROOT

static RET_CODE on_exit_menu(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u8 root_id;

  MT_ASSERT(p_ctrl != NULL);

  root_id = ctrl_get_ctrl_id(p_ctrl);
  if(root_id == IDC_COMM_ROOT_CONT)
  {
    root_id = ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl));
  }

  manage_close_menu(root_id, 0, 0);

  return SUCCESS;
}


// ON COMM MASK
static RET_CODE on_save(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *cont;

  cont = ctrl_get_child_by_id(p_ctrl, IDC_COMM_ROOT_CONT);
  if(cont != NULL)
  {
    ctrl_process_msg(cont, msg, para1, para2);
  }

  return SUCCESS;
}


static RET_CODE on_destory(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  return ERR_NOFEATURE;
}

void ui_comm_title_create(control_t *parent, u16 icon_id, u16 strid, u8 rstyle)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_txt, *p_first, *p_icon;
  MT_ASSERT(parent != NULL);

  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_TITLE_CONT,
                            0, 0, TITLE_W, TITLE_H,
                            NULL, 0);
  ctrl_set_rstyle(p_cont, rstyle, rstyle, rstyle);

  //creat bmp icon
  p_icon = ctrl_create_ctrl(CTRL_BMAP,
                              IDC_COMM_TITLE_IC,
                              TITLE_ICON_X,
                              TITLE_ICON_Y,
                              TITLE_ICON_W,
                              TITLE_ICON_H,
                              p_cont,
                              0);
  bmap_set_content_by_id(p_icon, icon_id);

  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_COMM_TITLE_TXT,
                           TITLE_TXT_X, TITLE_TXT_Y, TITLE_TXT_W, TITLE_TXT_H,
                           p_cont, 0);
  text_set_align_type(p_txt, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_txt, FSI_TITLE_TXT, FSI_TITLE_TXT, FSI_TITLE_TXT);
  text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_txt, strid);

  // insert to parent
  p_first = parent->p_child;

  if(p_first != NULL)
  {
    p_first->p_prev = p_cont;
    p_cont->p_next = p_first;
  }
  parent->p_child = p_cont;
  p_cont->p_parent = parent;
#endif
}



control_t *ui_comm_root_create(u8 root_id,
                               u8 parent_root,
                               u16 x,
                               u16 y,
                               u16 w,
                               u16 h,
                               u16 title_icon,
                               u16 title_strid)
{
  control_t *p_mask, *p_cont;

  p_mask = fw_create_mainwin(root_id,
                             0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                             parent_root, 0, OBJ_ATTR_ACTIVE, 0);
  MT_ASSERT(p_mask != NULL);
  ctrl_set_rstyle(p_mask, RSI_TRANSPARENT,
                  RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_proc(p_mask, comm_mask_proc);


  // create container
  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_ROOT_CONT,
                            x, y, w, h, p_mask, 0);
  ctrl_set_rstyle(p_cont, RSI_SUBMENU_DETAIL_BG, RSI_SUBMENU_DETAIL_BG, RSI_SUBMENU_DETAIL_BG);
  ctrl_set_keymap(p_cont, ui_comm_root_keymap);
  ctrl_set_proc(p_cont, ui_comm_root_proc);
  ui_comm_title_create(p_cont, title_icon, title_strid, RSI_MAIN_BG);

  return p_cont;
}





control_t *ui_comm_help_create_for_pop_dlg(comm_help_data_t *p_data, control_t *p_parent)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_mbox;
  u16 w, h;
  u8 i, row;

  MT_ASSERT(p_parent != NULL);
  MT_ASSERT(p_data != NULL);

  row = (p_data->item_cnt / p_data->col_cnt) +
        ((p_data->item_cnt % p_data->col_cnt) ? 1 : 0);
  w = ctrl_get_width(p_parent) - 2 * HELP_CONT_POP_DLG_H_GAP;
  h = (HELP_ITEM_H + HELP_ITEM_V_GAP) * row - HELP_ITEM_V_GAP;

  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_HELP_CONT,
                            HELP_CONT_POP_DLG_H_GAP,
                            (u16)(ctrl_get_height(p_parent) - h -
                                  HELP_CONT_POP_DLG_V_GAP),
                            w, h, p_parent, 0);

  ctrl_set_rstyle(p_cont, RSI_HELP_CNT, RSI_HELP_CNT, RSI_HELP_CNT);

  w = w - 2 * HELP_ITEM_H_GAP, h = h - 2 * HELP_ITEM_V_GAP;
  p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_COMM_HELP_MBOX,
                            HELP_ITEM_H_GAP, HELP_ITEM_V_GAP, w, h,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_enable_icon_mode(p_mbox, TRUE);
  mbox_enable_string_mode(p_mbox, TRUE);
  mbox_set_count(p_mbox, p_data->item_cnt, p_data->col_cnt, row);
  ctrl_set_mrect(p_mbox, 0, 0, w, h);
  mbox_set_item_interval(p_mbox,0, 0);
  mbox_set_item_rstyle(p_mbox,
                       RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_set_string_fstyle(p_mbox, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);

  mbox_set_icon_offset(p_mbox, HELP_ITEM_ICON_OX, HELP_ITEM_ICON_OY);
  mbox_set_icon_align_type(p_mbox, STL_LEFT | STL_VCENTER);

  mbox_set_string_offset(p_mbox, HELP_ITEM_TEXT_OX, HELP_ITEM_TEXT_OY);
  mbox_set_string_align_type(p_mbox, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);

  for(i = 0; i < p_data->item_cnt; i++)
  {
    mbox_set_content_by_icon(p_mbox, (u8)i, p_data->bmp_id[i],
                             p_data->bmp_id[i]);
    mbox_set_content_by_strid(p_mbox, (u8)i, p_data->str_id[i]);
  }

  return p_cont;
#else
  return NULL;
#endif
}


void ui_comm_help_move_pos(control_t *p_parent, u16 x, u16 offset_top,
                           u16 w, u16 offset_bottom, u16 str_offset)
{
  control_t *p_tmp = NULL;
  rect_t frm = {0};

  p_tmp = ctrl_get_child_by_id(p_parent,IDC_COMM_HELP_CONT);
  ctrl_get_frame(p_tmp, &frm);
  frm.top = frm.top +offset_top;
  frm.bottom = frm.bottom + offset_bottom;
  frm.left= x;
  frm.right= x + w;
  ctrl_set_frame(p_tmp, &frm);

  ctrl_add_rect_to_invrgn(p_tmp, NULL);

  p_tmp = ctrl_get_child_by_id(p_tmp,IDC_COMM_HELP_MBOX);
  ctrl_get_frame(p_tmp, &frm);
  frm.left= 0;
  frm.right= w;
  ctrl_set_frame(p_tmp, &frm);

  ctrl_add_rect_to_invrgn(p_tmp, NULL);

  ctrl_set_mrect(p_tmp, 0, 0, w, (frm.bottom-frm.top));
  mbox_set_item_interval(p_tmp,0, 0);
  mbox_set_string_offset(p_tmp, str_offset, 0);
}


void ui_comm_help_create(comm_help_data_t *p_data, control_t *p_parent)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_mbox;
  u16 w, h;
  u8 i, row;

  MT_ASSERT(p_parent != NULL);
  MT_ASSERT(p_data != NULL);

  row = (p_data->item_cnt / p_data->col_cnt) +
        ((p_data->item_cnt % p_data->col_cnt) ? 1 : 0);
  w = ctrl_get_width(p_parent) - 2 * HELP_CONT_H_GAP;
  h = (HELP_ITEM_H + HELP_ITEM_V_GAP) * row - HELP_ITEM_V_GAP;

  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_HELP_CONT,
                            HELP_CONT_H_GAP,
                            (u16)(ctrl_get_height(p_parent) - h -
                                  HELP_CONT_V_GAP),
                            w, h, p_parent, 0);

  ctrl_set_rstyle(p_cont, RSI_HELP_CNT, RSI_HELP_CNT, RSI_HELP_CNT);

  w = w - 2 * HELP_ITEM_H_GAP, h = h - 2 * HELP_ITEM_V_GAP;
  p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_COMM_HELP_MBOX,
                            HELP_ITEM_H_GAP, HELP_ITEM_V_GAP, w, h,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_enable_icon_mode(p_mbox, TRUE);
  mbox_enable_string_mode(p_mbox, TRUE);
  mbox_set_count(p_mbox, p_data->item_cnt, p_data->col_cnt, row);
  ctrl_set_mrect(p_mbox, 0, 0, w, h);
  mbox_set_item_interval(p_mbox, 0, 0);
  mbox_set_item_rstyle(p_mbox,
                       RSI_PBACK, RSI_PBACK, RSI_PBACK);
  mbox_set_string_fstyle(p_mbox, FSI_HELP_TEXT, FSI_HELP_TEXT, FSI_HELP_TEXT);

  mbox_set_icon_offset(p_mbox, HELP_ITEM_ICON_OX, HELP_ITEM_ICON_OY);
  mbox_set_icon_align_type(p_mbox, STL_LEFT | STL_VCENTER);

  mbox_set_string_offset(p_mbox, HELP_ITEM_TEXT_OX, HELP_ITEM_TEXT_OY);
  mbox_set_string_align_type(p_mbox, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);

  for(i = 0; i < p_data->item_cnt; i++)
  {
    mbox_set_content_by_icon(p_mbox, (u8)i, p_data->bmp_id[i],
                             p_data->bmp_id[i]);
    mbox_set_content_by_strid(p_mbox, (u8)i, p_data->str_id[i]);
  }
#endif
}


void ui_comm_help_create_ext(u16 x,
                               u16 y,
                               u16 w,
                               u16 h,
                               comm_help_data_t *p_data,
                               control_t *p_parent)
{
#ifdef EN_TITLE_HELP
  control_t *p_cont, *p_mbox;
  //u16 w, h;
  u8 i, row;

  MT_ASSERT(p_parent != NULL);
  MT_ASSERT(p_data != NULL);

  row = (p_data->item_cnt / p_data->col_cnt) +
        ((p_data->item_cnt % p_data->col_cnt) ? 1 : 0);
  //w = ctrl_get_width(p_parent) - 2 * HELP_CONT_H_GAP;
  //h = (HELP_ITEM_H + HELP_ITEM_V_GAP) * row - HELP_ITEM_V_GAP;

  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_HELP_CONT,
                            x, y,
                            w, h,
                            p_parent, 0);

  ctrl_set_rstyle(p_cont, RSI_HELP_CNT, RSI_HELP_CNT, RSI_HELP_CNT);

  w = w - 2 * HELP_ITEM_H_GAP;
  h = h - 2 * HELP_ITEM_V_GAP;
  p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_COMM_HELP_MBOX,
                            HELP_ITEM_H_GAP, HELP_ITEM_V_GAP, w, h,
                            p_cont, 0);
  ctrl_set_rstyle(p_mbox, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  mbox_enable_icon_mode(p_mbox, TRUE);
  mbox_enable_string_mode(p_mbox, TRUE);
  mbox_set_count(p_mbox, p_data->item_cnt, p_data->col_cnt, row);
  ctrl_set_mrect(p_mbox, 0, 0, w, h);
  mbox_set_item_interval(p_mbox, 0, 0);
  mbox_set_item_rstyle(p_mbox,
                       RSI_PBACK, RSI_PBACK, RSI_PBACK);
  mbox_set_string_fstyle(p_mbox, FSI_HELP_TEXT, FSI_HELP_TEXT, FSI_HELP_TEXT);

  mbox_set_icon_offset(p_mbox, HELP_ITEM_ICON_OX, HELP_ITEM_ICON_OY);
  mbox_set_icon_align_type(p_mbox, STL_LEFT | STL_VCENTER);

  mbox_set_string_offset(p_mbox, HELP_ITEM_TEXT_OX, HELP_ITEM_TEXT_OY);
  mbox_set_string_align_type(p_mbox, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_STRID);

  for(i = 0; i < p_data->item_cnt; i++)
  {
    mbox_set_content_by_icon(p_mbox, (u8)i, p_data->bmp_id[i],
                             p_data->bmp_id[i]);
    mbox_set_content_by_strid(p_mbox, (u8)i, p_data->str_id[i]);
  }
#endif
}

control_t *ui_comm_root_get_ctrl(u8 root_id, u8 ctrl_id)
{
  control_t *root = fw_find_root_by_id(root_id);

  if(root == NULL)
  {
    return NULL;
  }
  if(ctrl_get_ctrl_id(root->p_child) == IDC_COMM_ROOT_CONT)
  {
    root = root->p_child;
  }
  return ctrl_get_child_by_id(root, ctrl_id);
}


control_t *ui_comm_root_get_root(u8 root_id)
{
  control_t *root = fw_find_root_by_id(root_id);

  if(root == NULL)
  {
    return NULL;
  }
  if(ctrl_get_ctrl_id(root->p_child) == IDC_COMM_ROOT_CONT)
  {
    root = root->p_child;
  }

  return root;
}


BEGIN_KEYMAP(ui_comm_root_keymap, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(ui_comm_root_keymap, NULL)

BEGIN_MSGPROC(comm_mask_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SAVE, on_save)
  ON_COMMAND(MSG_DESTROY, on_destory)
END_MSGPROC(comm_mask_proc, ui_comm_root_proc)

BEGIN_MSGPROC(ui_comm_root_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_exit_menu)
  ON_COMMAND(MSG_EXIT_ALL, on_exit_menu)
END_MSGPROC(ui_comm_root_proc, cont_class_proc)

BEGIN_KEYMAP(comm_menu_mbox_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(comm_menu_mbox_keymap, NULL)

BEGIN_KEYMAP(comm_menu_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(comm_menu_cont_keymap, ui_comm_root_keymap)
