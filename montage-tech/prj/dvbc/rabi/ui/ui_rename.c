/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_rename.h"
#include "ui_keyboard_v2.h"


enum control_id
{
  IDC_RENAME_TITLE = 1,
  IDC_RENAME_EBOX,
  IDC_RENAME_CAPS_BMP,
  IDC_RENAME_CAPS_TXT,
};

enum rename_local_msg
{
  MSG_SWITCH_CAPS = MSG_LOCAL_BEGIN + 700,
  MSG_CONFIRM,
};

static comm_help_data_t help_data = //help bar data
{
  3,                                    //select  + scroll page + exit
  3,
  {  IDS_INPUT,//,IDS_OK,
      IDS_SAVE,
     IDS_CANCEL },
  {
     IM_HELP_OK,
     IM_HELP_YELLOW,
     IM_HELP_BLUE },
};

static u8 g_is_upper = FALSE;
rename_param_t rename_param;
static u16 new_name[MAX_FAV_NAME_LEN];

u16 rename_cont_keymap(u16 key);
RET_CODE rename_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 rename_name_keymap(u16 key);
RET_CODE rename_name_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

/*
static RET_CODE on_rename_switch_caps(control_t *p_cont, u16 msg, u32 para1,
                            u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_ctrl;

  g_is_upper = !g_is_upper;
  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_RENAME_EBOX);
  ui_comm_t9edit_set_upper(p_ctrl, g_is_upper);

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_RENAME_CAPS_TXT);
  text_set_content_by_ascstr(p_ctrl, g_is_upper ? "ABC" : "abc");
  ctrl_paint_ctrl(p_ctrl, TRUE);

  return ret;
}
*/

static RET_CODE on_rename_confirm(control_t *p_cont, u16 msg, u32 para1,
                            u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_ctrl;
  u16 *unistr;

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_RENAME_EBOX);
  unistr = ui_comm_t9edit_get_content(p_ctrl);
  if(uni_strlen(unistr) != 0)
  {
    uni_strcpy(new_name, unistr);

    if(SUCCESS == fw_notify_parent(
      ROOT_ID_RENAME, NOTIFY_T_MSG, TRUE,
      MSG_RENAME_CHECK, (u32)new_name, 0))//check to invalidation of the new string.
    {
      if (uni_strcmp(new_name, rename_param.uni_str))
      {
        //rename_param.cb(new_name);
        fw_notify_parent(ROOT_ID_RENAME, NOTIFY_T_MSG, FALSE,
          MSG_RENAME_UPDATE, (u32)new_name, 0);
      }
      ret = manage_close_menu(ROOT_ID_RENAME, 0, 0);
    }
    else
    {
      ui_comm_cfmdlg_open(NULL, IDS_MSG_INVALID_NAME, NULL, 0);
    }
  }
  else
  {
    ui_comm_cfmdlg_open(NULL, IDS_MSG_INVALID_NAME, NULL, 0);
  }

  return ret;
}
/*
static RET_CODE on_rename_backspace(control_t *p_cont, u16 msg, u32 para1,
                            u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_ctrl;

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_RENAME_EBOX);
  ui_comm_t9edit_backspace(p_ctrl);
  ui_comm_ctrl_paint_ctrl(p_ctrl, FALSE);

  return ret;
}
*/

static RET_CODE on_rename_cancle(control_t *p_cont, u16 msg, u32 para1,
                            u32 para2)
{

 fw_notify_parent(
      ROOT_ID_RENAME, NOTIFY_T_MSG, TRUE,
      MSG_RENAME_CHECK, (u32)new_name, 0);
  return manage_close_menu(ROOT_ID_RENAME, 0, 0);
}

RET_CODE open_rename(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ebox, *p_title; //*p_caps_bmp;

  g_is_upper = FALSE;
  memcpy((void *)&rename_param, (void *)para1, sizeof(rename_param_t));

//#ifndef SPT_SUPPORT
  p_cont = fw_create_mainwin(ROOT_ID_RENAME, RENAME_CONT_X,
                                  RENAME_CONT_Y, RENAME_CONT_W, RENAME_CONT_H,
                                  (u8)para2,
                                  0, OBJ_ATTR_ACTIVE,
                                  0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);//RSI_RENAME_FRM
  ctrl_set_keymap(p_cont, rename_cont_keymap);
  ctrl_set_proc(p_cont, rename_cont_proc);

  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_RENAME_TITLE,
                             RENAME_TITLE_X, RENAME_TITLE_Y, RENAME_TITLE_W,
                             RENAME_TITLE_H, p_cont, 0);
  text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_RENAME);
/*
  p_caps_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_RENAME_CAPS_BMP,
                                RENAME_CAPS_BMAPX, RENAME_CAPS_BMAPY,
                                RENAME_CAPS_BMAPW,
                                RENAME_CAPS_BMAPH, p_cont,
                                0);
  bmap_set_content_type(p_caps_bmp, FALSE);
  bmap_set_content_by_id(p_caps_bmp, IM_ZOOM);*/
  //Im_Zoom ---IM_RENAME_PENCIL temp picture
/*
  p_caps_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_RENAME_CAPS_TXT,
                                RENAME_CAPS_TXTX, RENAME_CAPS_TXTY,
                                RENAME_CAPS_TXTW,
                                RENAME_CAPS_TXTH, p_cont,
                                0);;
  ctrl_set_rstyle(p_caps_txt, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_caps_txt, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_caps_txt, FSI_RENAME_SH, FSI_RENAME_SH, FSI_RENAME_SH);
  text_set_content_type(p_caps_txt, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_caps_txt, g_is_upper ? "ABC" : "abc");*/

  p_ebox = ui_comm_t9edit_create(p_cont, IDC_RENAME_EBOX,
                                   RENAME_EBOX_X, RENAME_EBOX_Y,
                                   0, RENAME_EBOX_W,
                                   ROOT_ID_RENAME);

  ctrl_set_rstyle(p_ebox, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  //ui_comm_ctrl_set_txt_font(p_ebox, FSI_BLACK, FSI_BLACK, FSI_BLACK);
  //ui_comm_t9edit_set_font(p_ebox, FSI_BLACK, FSI_BLACK, FSI_BLACK);
  ui_comm_ctrl_set_keymap(p_ebox, rename_name_keymap);
  ui_comm_ctrl_set_proc(p_ebox, rename_name_proc);

  ui_comm_t9edit_set_static_txt(p_ebox, IDS_NAME);
  ui_comm_t9edit_set_param(p_ebox, rename_param.max_len);
  ui_comm_t9edit_set_content_by_unistr(p_ebox, rename_param.uni_str);


//#else
//#endif
  // add help bar
  ui_comm_help_create_for_pop_dlg(&help_data, p_cont);

//#ifdef SPT_DUMP_DATA
//  spt_dump_menu_data(p_cont);
//#endif

  ctrl_default_proc(p_ebox, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_cont, FALSE);

  return SUCCESS;
}


//void prog_name_update(void)
RET_CODE prog_name_update(u16* p_unistr)
{
  control_t *p_edit_cont;
  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_RENAME, IDC_RENAME_EBOX);

  ctrl_paint_ctrl(p_edit_cont, TRUE);
  
  return SUCCESS;
}

static RET_CODE on_edit_name(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
  kb_param_t param;
  control_t *p_ebox;

  p_ebox = ctrl_get_parent(p_ctrl);
  param.uni_str = ui_comm_t9edit_get_content(p_ebox);
  param.type = rename_param.type;
  if(NULL != fw_find_root_by_id(ROOT_ID_FAVORITE))
  {
    param.max_len = (MAX_FAV_NAME_LEN - 8);
  }
  else
  {
    param.max_len = (DB_DVBS_MAX_NAME_LENGTH - 1);
  }
  param.cb = prog_name_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  return SUCCESS;
}


BEGIN_KEYMAP(rename_cont_keymap, NULL)
 // ON_EVENT(V_KEY_RED, MSG_SWITCH_CAPS)
  ON_EVENT(V_KEY_YELLOW, MSG_CONFIRM)
  ON_EVENT(V_KEY_BLUE, MSG_CANCEL)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
END_KEYMAP(rename_cont_keymap, NULL)

BEGIN_MSGPROC(rename_cont_proc, cont_class_proc)
 // ON_COMMAND(MSG_SWITCH_CAPS, on_rename_switch_caps)
  ON_COMMAND(MSG_CANCEL, on_rename_cancle)
  //ON_COMMAND(MSG_BACKSPACE, on_rename_backspace)
  ON_COMMAND(MSG_CONFIRM, on_rename_confirm)
END_MSGPROC(rename_cont_proc, cont_class_proc)

BEGIN_KEYMAP(rename_name_keymap, ui_comm_t9_keymap)
ON_EVENT(V_KEY_OK, MSG_SELECT)
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
END_KEYMAP(rename_name_keymap, ui_comm_t9_keymap)

BEGIN_MSGPROC(rename_name_proc, ui_comm_t9_proc)
ON_COMMAND(MSG_SELECT, on_edit_name)
ON_COMMAND(MSG_NUMBER, on_edit_name)
END_MSGPROC(rename_name_proc, ui_comm_t9_proc)

