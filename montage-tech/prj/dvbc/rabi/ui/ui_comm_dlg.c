/****************************************************************************

****************************************************************************/

#include "ui_common.h"
#include "ui_mute.h"
#include "fw_common.h"

enum comm_dlg_idc
{
  IDC_BTN_YES = 1,
  IDC_BTN_NO,
  IDC_CONTENT
};

enum comm_dlg_msg
{
    MSG_DLG_TIME_OUT = MSG_LOCAL_BEGIN + 550,
};

static dlg_ret_t g_dlg_ret;
extern RET_CODE mainmenu_paint_btn(u16 id);
extern BOOL is_desktop_init(void);

u16 comm_dlg_keymap(u16 key);

RET_CODE comm_dlg_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 comm_dlg_keymap_2(u16 key);

RET_CODE comm_dlg_proc_2(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

#define DO_DLG_FUNC(func) \
  if (func != NULL) (*func)();

#define DO_DLG_FUNC_WITHPARA(func, para1, para2) \
  if (func != NULL) (*func)(para1, para2);

void ui_comm_dlg_reset_cur_loop_win(u16 root_id)
{
	FW_RESET_STATE(root_id);
}

u16 ui_comm_dlg_get_cur_loop_win(void)
{
	fw_main_t *p_info = NULL;

	p_info = (fw_main_t *)class_get_handle_by_id(UIFRM_CLASS_ID);
	MT_ASSERT(p_info != NULL);  

	return GET_QS_ROOT(p_info->infor.msg_q_state);
}

void ui_comm_dlg_set_default_mainwin_loop(u16 mainwin_id)
{
    u16 root_id;
	
	root_id = ui_comm_dlg_get_cur_loop_win();
	DEBUG(MAIN, INFO, "root_id = %d mainwin_id = %d\n",root_id,mainwin_id);
    fw_default_mainwin_loop(mainwin_id);
	DEBUG(MAIN, INFO, "\n");

	if(root_id != mainwin_id)
		ui_comm_dlg_reset_cur_loop_win(root_id);
}

extern iconv_t g_cd_gb2312_to_utf16le;
void gb2312_2_utf16le(char* inbuf, int in_len, char* outbuf, int out_len)
{
  char** p_inbuf = &inbuf;
  char** p_outbuf = &outbuf;
  iconv(g_cd_gb2312_to_utf16le, p_inbuf, &in_len, p_outbuf, &out_len);
}
dlg_ret_t ui_comm_dlg_open(comm_dlg_data_t *p_data)
{
  control_t *p_cont;
  control_t *p_btn, *p_txt;
  u16 i, cnt, x, y;
  u8 type, mode;
  u16 btn[2] = { IDS_YES, IDS_NO };
  dlg_ret_t  ret;
  dlg_ret_t  last_ret = DLG_RET_NULL;

  if(ui_is_vod_menu(fw_get_first_active_id()))
  {
        btn[0] = IDS_HD_YES;
        btn[1] = IDS_HD_NO;
  }

  if(!is_desktop_init())
  {
	DEBUG(DBG, ERR, "desktop not init!!!\n");
	return last_ret;
  }
  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) // already opened
  {
    UI_PRINTF("UI: already open a dialog, force close it! \n");
    ui_comm_dlg_close();
    last_ret = g_dlg_ret;
  }

  p_cont = p_btn = p_txt = NULL;
  x = y = cnt = 0;

  type = p_data->style & 0x0F;
  mode = p_data->style & 0xF0;

  // create root at first
  p_cont = ctrl_create_ctrl(CTRL_CONT, ROOT_ID_POPUP,
                            p_data->x, p_data->y, p_data->w, p_data->h,
                            NULL, 0);
  ctrl_set_attr(p_cont, (u8)(type == \
                             DLG_FOR_SHOW ? OBJ_ATTR_INACTIVE : OBJ_ATTR_ACTIVE));
  ctrl_set_rstyle(p_cont,
                  RSI_POPUP_BG,
                  RSI_POPUP_BG,
                  RSI_POPUP_BG);

  ctrl_set_keymap(p_cont, comm_dlg_keymap);
  ctrl_set_proc(p_cont, comm_dlg_proc);

  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
                           DLG_CONTENT_GAP, DLG_CONTENT_GAP,
                           (u16)(p_data->w - 2 * DLG_CONTENT_GAP),
                           (u16)(p_data->h - 2 * DLG_CONTENT_GAP - DLG_BTN_H),
                           p_cont, 0);

  text_set_font_style(p_txt, FSI_DLG_BTN_HL, FSI_DLG_BTN_HL, FSI_DLG_BTN_HL);
  if (mode == DLG_STR_MODE_STATIC)
  {
    text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_txt, (u16)p_data->content);
  }
  else
  {
    text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
    OS_PRINTF("%s(), %d\n", __FUNCTION__, __LINE__);
    text_set_content_by_unistr(p_txt, (u16*)p_data->content);
    OS_PRINTF("%s(), %d\n", __FUNCTION__, __LINE__);
  }

  switch (type)
  {
    case DLG_FOR_ASK:
      x = p_data->w / 3 - DLG_BTN_W / 2;
      y = p_data->h - DLG_BTN_H - DLG_CONTENT_GAP;
      cnt = 2;
      break;
    case DLG_FOR_CONFIRM:
      x = (p_data->w - DLG_BTN_W) / 2;
      y = p_data->h - DLG_BTN_H - DLG_CONTENT_GAP;
      cnt = 1;
      break;
    case DLG_FOR_SHOW:
      cnt = 0;
      break;
    default:
      MT_ASSERT(0);
  }

  for (i = 0; i < cnt; i++)
  {
    p_btn = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_BTN_YES + i),
                             x, y, DLG_BTN_W, DLG_BTN_H, p_cont, 0);
    ctrl_set_rstyle(p_btn,
                    RSI_PBACK, RSI_ITEM_4_HL, RSI_ITEM_4_SH);
    text_set_font_style(p_btn,
                        FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
    text_set_content_type(p_btn, TEXT_STRTYPE_STRID);

    if(p_data->specify_bnt == TRUE)
    {
      text_set_content_by_strid(p_btn, p_data->specify_bnt_cont[i]);
    }
    else
    {
      text_set_content_by_strid(p_btn,
                                (u16)(cnt == 1 ? IDS_OK : btn[i]));
    }

    ctrl_set_related_id(p_btn,
                        (u8)((i - 1 + cnt) % cnt + 1), /* left */
                        0,                             /* up */
                        (u8)((i + 1) % cnt + 1),       /* right */
                        0);                            /* down */

    x += p_data->w / 3;
  }

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  // attach the root onto mainwin
  OS_PRINTF("p_cont[0x%x], root[%d]\n", p_cont, p_data->parent_root);
  fw_attach_root(p_cont, p_data->parent_root);

  // paint
  if (cnt > 0)
  {
    ctrl_default_proc(p_btn, MSG_GETFOCUS, 0, 0);
  }
  ctrl_paint_ctrl(p_cont, FALSE);

  if(p_data->dlg_tmout != 0)
  {
    ret = fw_tmr_create(ROOT_ID_POPUP, MSG_CANCEL, p_data->dlg_tmout, FALSE);

    MT_ASSERT(ret == SUCCESS);
  }

  // start loop to get msg
  if (type == DLG_FOR_SHOW)
  {
    ret = DLG_RET_NULL;
  }
  else
  {
	ui_comm_dlg_set_default_mainwin_loop(ctrl_get_ctrl_id(p_cont));
    ret = g_dlg_ret;
    g_dlg_ret = last_ret;
  }

  return ret;
}


void ui_comm_dlg_close(void)
{
  u8 index;
  BOOL is_check_mute = FALSE;

  fw_tmr_destroy(ROOT_ID_POPUP, MSG_CANCEL);
  manage_close_menu(ROOT_ID_POPUP, 0, 0);

  index = fw_get_focus_id();
  if(index == ROOT_ID_PROG_LIST
    || index == ROOT_ID_VEPG
    || ui_is_fullscreen_menu(index))
  {
    is_check_mute = TRUE; //fix bug 18847
  }
  if(ROOT_ID_MAINMENU==index)
  {
	  mainmenu_paint_btn(3);
  }

  if(ui_is_mute() && is_check_mute)
  {
    open_mute(0, 0);
  }
	
	if(ROOT_ID_XSYS_SET==index)
		  swtich_to_sys_set(ROOT_ID_INVALID, 0);
}

dlg_ret_t ui_comm_dlg_open2(comm_dlg_data_t *p_data)
{
  control_t *p_cont;
  control_t *p_btn, *p_txt;
  u16 i, cnt, x, y;
  u8 type, mode;
  u16 btn[2] = { IDS_YES, IDS_NO };
  dlg_ret_t ret;

  if(fw_find_root_by_id(ROOT_ID_POPUP2) != NULL) // already opened
  {
    UI_PRINTF("UI: already open a dialog2, force close it! \n");
    ui_comm_dlg_close2();
  }

  if(ui_is_vod_menu(fw_get_first_active_id()))
  {
        btn[0] = IDS_HD_YES;
        btn[1] = IDS_HD_NO;
  }

  p_cont = p_btn = p_txt = NULL;
  x = y = cnt = 0;

  type = p_data->style & 0x0F;
  mode = p_data->style & 0xF0;

  // create root at first
  p_cont = ctrl_create_ctrl(CTRL_CONT, ROOT_ID_POPUP2,
                            p_data->x, p_data->y, p_data->w, p_data->h,
                            NULL, 0);
  ctrl_set_attr(p_cont, (u8)(type == \
                             DLG_FOR_SHOW ? OBJ_ATTR_INACTIVE : OBJ_ATTR_ACTIVE));
  ctrl_set_rstyle(p_cont,
                  RSI_POPUP_BG,
                  RSI_POPUP_BG,
                  RSI_POPUP_BG);

  ctrl_set_keymap(p_cont, comm_dlg_keymap_2);
  ctrl_set_proc(p_cont, comm_dlg_proc_2);

  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
                           DLG_CONTENT_GAP, DLG_CONTENT_GAP,
                           (u16)(p_data->w - 2 * DLG_CONTENT_GAP),
                           (u16)(p_data->h - 2 * DLG_CONTENT_GAP - DLG_BTN_H),
                           p_cont, 0);

  text_set_font_style(p_txt, FSI_DLG_CONTENT, FSI_DLG_CONTENT, FSI_DLG_CONTENT);
  if (mode == DLG_STR_MODE_STATIC)
  {
    text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_txt, (u16)p_data->content);
  }
  else
  {
    text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_txt, (u16*)p_data->content);
  }

  switch (type)
  {
    case DLG_FOR_ASK:
      x = p_data->w / 3 - DLG_BTN_W / 2;
      y = p_data->h - DLG_BTN_H - DLG_CONTENT_GAP;
      cnt = 2;
      break;
    case DLG_FOR_CONFIRM:
      x = (p_data->w - DLG_BTN_W) / 2;
      y = p_data->h - DLG_BTN_H - DLG_CONTENT_GAP;
      cnt = 1;
      break;
    case DLG_FOR_SHOW:
      cnt = 0;
      break;
    default:
      MT_ASSERT(0);
  }

  for (i = 0; i < cnt; i++)
  {
    p_btn = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_BTN_YES + i),
                             x, y, DLG_BTN_W, DLG_BTN_H, p_cont, 0);
    ctrl_set_rstyle(p_btn,
                    RSI_ITEM_4_SH, RSI_ITEM_4_HL, RSI_ITEM_4_SH);
    text_set_font_style(p_btn,
                        FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
    text_set_content_type(p_btn, TEXT_STRTYPE_STRID);

    if(p_data->specify_bnt == TRUE)
    {
      text_set_content_by_strid(p_btn, p_data->specify_bnt_cont[i]);
    }
    else
    {
      text_set_content_by_strid(p_btn,
                                (u16)(cnt == 1 ? IDS_OK : btn[i]));
    }

    ctrl_set_related_id(p_btn,
                        (u8)((i - 1 + cnt) % cnt + 1), /* left */
                        0,                             /* up */
                        (u8)((i + 1) % cnt + 1),       /* right */
                        0);                            /* down */

    x += p_data->w / 3;
  }

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  // attach the root onto mainwin
  OS_PRINTF("p_cont[0x%x], root[%d]\n", p_cont, p_data->parent_root);
  fw_attach_root(p_cont, p_data->parent_root);

  // paint
  if (cnt > 0)
  {
    if((p_data->content == IDS_BOOKPG_START) ||
        (p_data->content == IDS_BOOKPG_END) ||
        (p_data->content == IDS_RECALL_PG))
    {
      p_btn = ctrl_get_child_by_id(p_cont,IDC_BTN_YES);
    }
    else
    {
      p_btn = ctrl_get_child_by_id(p_cont,IDC_BTN_NO);
    }
    ctrl_default_proc(p_btn, MSG_GETFOCUS, 0, 0);
  }
  ctrl_paint_ctrl(p_cont, FALSE);

  if(p_data->dlg_tmout != 0)
  {
    ret = fw_tmr_create(ROOT_ID_POPUP2, MSG_DLG_TIME_OUT, p_data->dlg_tmout, FALSE);

    MT_ASSERT(ret == SUCCESS);
  }

  // start loop to get msg
  if (type == DLG_FOR_SHOW)
  {
    //ret = DLG_RET_NULL;
    ret = DLG_RET_NULL;
    //ret = g_dlg_ret;
  }
  else
  {
 	ui_comm_dlg_set_default_mainwin_loop(ctrl_get_ctrl_id(p_cont));
    ret = g_dlg_ret;
  }

  return ret;
}

dlg_ret_t ui_comm_dlg_open3(comm_dlg_data_t2*p_data)
{
  control_t *p_cont, *p_btn = NULL;
  control_t *p_title, *p_txt;
  u16 i,x,cnt, y;
  u8 type[2], mode[2];
  dlg_ret_t ret,last_ret;
  u8 asc_str[255] = {0};
  u16 uni_str[255+1] = {0};
  u16 btn[2] = { IDS_YES, IDS_NO };

  last_ret = g_dlg_ret;
  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL) // already opened
  {
    UI_PRINTF("UI: already open a dialog, force close it! \n");
    ui_comm_dlg_close();
  }
  
  p_cont = p_title = p_txt = NULL;
  x = y = cnt =0;

  type[0] = p_data->title_style& 0x0F;
  mode[0] = p_data->title_style& 0xF0;
  type[1] = p_data->text_style& 0x0F;
  mode[1] = p_data->text_style& 0xF0;

  // create root at first
  p_cont = ctrl_create_ctrl(CTRL_CONT, ROOT_ID_POPUP,
                            p_data->x, p_data->y, p_data->w, p_data->h,
                            NULL, 0);
  ctrl_set_attr(p_cont, (u8)(type[0] == \
                             DLG_FOR_SHOW ? OBJ_ATTR_INACTIVE : OBJ_ATTR_ACTIVE));
  ctrl_set_rstyle(p_cont,
                  RSI_POPUP_BG,
                  RSI_POPUP_BG,
                  RSI_POPUP_BG);

  ctrl_set_keymap(p_cont, comm_dlg_keymap);
  ctrl_set_proc(p_cont, comm_dlg_proc);

  p_title= ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
                           DLG_CONTENT_GAP, DLG_CONTENT_GAP,
                           (u16)(p_data->w - 2 * DLG_CONTENT_GAP),
                           (u16)(DLG_BTN_H),
                           p_cont, 0);

  text_set_font_style(p_title, FSI_DLG_CONTENT, FSI_DLG_CONTENT, FSI_DLG_CONTENT);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  if (mode[0] == DLG_STR_MODE_STATIC)
  {
    text_set_content_type(p_title, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_title, (u16)p_data->title_strID);
  }
  else
  {
	  text_set_content_type(p_title, TEXT_STRTYPE_UNICODE);
	  sprintf((char *)asc_str, "%s", (char *)p_data->title_content);
	  gb2312_to_unicode(asc_str, sizeof(asc_str), uni_str, sizeof(asc_str));
	  text_set_content_by_unistr(p_title, uni_str);
  }

  p_txt = ctrl_create_ctrl(CTRL_TEXT, (IDC_CONTENT+1),
                           DLG_CONTENT_GAP, (DLG_CONTENT_GAP + DLG_BTN_H),
                           (u16)(p_data->w - 2 * DLG_CONTENT_GAP),
                           (u16)(p_data->h - 2 * DLG_CONTENT_GAP - DLG_BTN_H*2),
                           p_cont, 0);

  text_set_font_style(p_txt, FSI_DLG_CONTENT, FSI_DLG_CONTENT, FSI_DLG_CONTENT);
  if (mode[1] == DLG_STR_MODE_STATIC)
  {
    text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_txt, (u16)p_data->text_strID);
  }
  else
  {
		text_set_content_type(p_txt, TEXT_STRTYPE_UNICODE);
		memset(asc_str,0,sizeof(asc_str));
		memset(uni_str,0,sizeof(uni_str));
		sprintf((char *)asc_str, "%s", (char *)p_data->text_content);
		gb2312_to_unicode(asc_str, sizeof(asc_str), uni_str, sizeof(asc_str));
		text_set_content_by_unistr(p_txt, uni_str);
  }

  switch (type[0])
  {
    case DLG_FOR_ASK:
      x = p_data->w / 3 - DLG_BTN_W / 2;
      y = p_data->h - DLG_BTN_H - DLG_CONTENT_GAP;
      cnt = 2;
      break;
    case DLG_FOR_CONFIRM:
      x = (p_data->w - DLG_BTN_W) / 2;
      y = p_data->h - DLG_BTN_H - DLG_CONTENT_GAP;
      cnt = 1;
      break;
    case DLG_FOR_SHOW:
      cnt = 0;
      break;
    default:
      MT_ASSERT(0);
  }

  for (i = 0; i < cnt; i++)
  {
    p_btn = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_BTN_YES + i),
                             x, y, DLG_BTN_W, DLG_BTN_H, p_cont, 0);
    ctrl_set_rstyle(p_btn,
                    RSI_ITEM_1_SH, RSI_ITEM_1_HL, RSI_ITEM_1_SH);
    text_set_font_style(p_btn,
                        FSI_DLG_BTN_SH, FSI_DLG_BTN_HL, FSI_DLG_BTN_SH);
    text_set_content_type(p_btn, TEXT_STRTYPE_STRID);

    text_set_content_by_strid(p_btn,
                                (u16)(cnt == 1 ? IDS_OK : btn[i]));

    ctrl_set_related_id(p_btn,
                        (u8)((i - 1 + cnt) % cnt + 1), /* left */
                        0,                             /* up */
                        (u8)((i + 1) % cnt + 1),       /* right */
                        0);                            /* down */

    x += p_data->w / 3;
  }


  if (cnt > 0)
  {
    ctrl_default_proc(p_btn, MSG_GETFOCUS, 0, 0);
  }
  ctrl_paint_ctrl(p_cont, FALSE);


  // attach the root onto mainwin
  OS_PRINTF("p_cont[0x%x], root[%d]\n", p_cont, p_data->parent_root);
  fw_attach_root(p_cont, p_data->parent_root);

  if(p_data->dlg_tmout != 0)
  {
    ret = fw_tmr_create(ROOT_ID_POPUP, MSG_CANCEL, p_data->dlg_tmout, FALSE);

    MT_ASSERT(ret == SUCCESS);
  }

  // start loop to get msg
  if (type[0] == DLG_FOR_SHOW)
  {
    ret = DLG_RET_NULL;
  }
  else
  {
	ui_comm_dlg_set_default_mainwin_loop(ctrl_get_ctrl_id(p_cont));
    ret = g_dlg_ret;
  }
  g_dlg_ret = last_ret;
  return ret;
}

void ui_comm_dlg_close2(void)
{
  u8 index;
  BOOL is_check_mute = FALSE;

  fw_tmr_destroy(ROOT_ID_POPUP2, MSG_DLG_TIME_OUT);
  manage_close_menu(ROOT_ID_POPUP2, 0, 0);

  index = fw_get_focus_id();
  if(index == ROOT_ID_PROG_LIST
    || index == ROOT_ID_VEPG
    || ui_is_fullscreen_menu(index))
  {
    is_check_mute = TRUE; //fix bug 18847
  }

  if(ui_is_mute() && is_check_mute)
  {
    open_mute(0, 0);
  }
  if(ROOT_ID_XSYS_SET==fw_get_focus_id())
		swtich_to_sys_set(ROOT_ID_INVALID, 0);

}

static void fill_dlg_data(comm_dlg_data_t *p_data, u8 style, u32 content,
                          rect_t *p_dlg_rc, u32 tm_out)
{
  memset(p_data, 0, sizeof(comm_dlg_data_t));

  p_data->parent_root = 0;
  p_data->style = style;
  p_data->content = content;
  p_data->dlg_tmout = tm_out;

  if (p_dlg_rc != NULL)
  {
    p_data->x = p_dlg_rc->left;
    p_data->y = p_dlg_rc->top;
    p_data->w = RECTWP(p_dlg_rc);
    p_data->h = RECTHP(p_dlg_rc);
  }
  else
  {
    p_data->x = COMM_DLG_X;
    p_data->y = COMM_DLG_Y;
    p_data->w = COMM_DLG_W;
    p_data->h = COMM_DLG_H;
  }
}


void ui_comm_cfmdlg_open(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out)
{
  comm_dlg_data_t dlg_data;

  if(!is_desktop_init())
  {
	DEBUG(DBG, ERR, "desktop not init!!!\n");
	return;
  }
  fill_dlg_data(&dlg_data,
                DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
                strid, p_dlg_rc, tm_out);

  ui_comm_dlg_open(&dlg_data);

  DO_DLG_FUNC(do_cmf);
}

void ui_comm_cfmdlg_open_unistr(rect_t *p_dlg_rc, u16* p_unistr, do_func_t do_cmf, u32 tm_out)
{
  comm_dlg_data_t dlg_data;

  fill_dlg_data(&dlg_data,
                DLG_FOR_CONFIRM | DLG_STR_MODE_EXTSTR,
                (u32)p_unistr, p_dlg_rc, tm_out);
  ui_comm_dlg_open(&dlg_data);
  DO_DLG_FUNC(do_cmf);
}
void ui_comm_cfmdlg_open_gb(rect_t *p_dlg_rc, u8* p_gb, do_func_t do_cmf, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  u16 uni_str[128] = {0};
  gb2312_2_utf16le(p_gb, strlen(p_gb), (char*)uni_str, sizeof(uni_str)-1);
  fill_dlg_data(&dlg_data,
                DLG_FOR_CONFIRM | DLG_STR_MODE_EXTSTR,
                (u32)uni_str, p_dlg_rc, tm_out);

  ui_comm_dlg_open(&dlg_data);

  DO_DLG_FUNC(do_cmf);
}


void ui_comm_ask_for_dodlg_open_xxx(rect_t *p_dlg_rc, u16 strid,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  dlg_ret_t ret;

  fill_dlg_data(&dlg_data,
                DLG_FOR_ASK | DLG_STR_MODE_STATIC,
                (u32)strid, p_dlg_rc, tm_out);

  ret = ui_comm_dlg_open(&dlg_data);

  if (ret == DLG_RET_YES)
  {
    DO_DLG_FUNC(do_func);
  }
  else if(ret == DLG_RET_NO)
  {
    DO_DLG_FUNC(undo_func);
  }
}


void ui_comm_ask_for_dodlg_open_unistr(rect_t *p_dlg_rc, u16 *p_unistr,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  dlg_ret_t ret;

  fill_dlg_data(&dlg_data,
                DLG_FOR_ASK | DLG_STR_MODE_EXTSTR,
                (u32)p_unistr, p_dlg_rc, tm_out);

  ret = ui_comm_dlg_open(&dlg_data);

  if (ret == DLG_RET_YES)
  {
    DO_DLG_FUNC(do_func);
  }
  else if (ret == DLG_RET_NO)
  {
    DO_DLG_FUNC(undo_func);
  }
}


void ui_comm_savdlg_open(rect_t *p_dlg_rc, do_func_t func, u32 tm_out)
{
  comm_dlg_data_t dlg_data;

  fill_dlg_data(&dlg_data,
                DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
                IDS_MSG_SAVING, p_dlg_rc, tm_out);

  ui_comm_dlg_open(&dlg_data);

  DO_DLG_FUNC(func);

  ui_comm_dlg_close();
}

void ui_comm_ask_for_dodlg_open(rect_t *p_dlg_rc, u16 strid,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  dlg_ret_t ret;

  if(!is_desktop_init())
  {
	DEBUG(DBG, ERR, "desktop not init!!!\n");
	return;
  }

  fill_dlg_data(&dlg_data,
                DLG_FOR_ASK | DLG_STR_MODE_STATIC,
                strid, p_dlg_rc, tm_out);

  ret = ui_comm_dlg_open2(&dlg_data);

  if (ret == DLG_RET_YES)
  {
    DO_DLG_FUNC(do_func);
  }
  else
  {
    DO_DLG_FUNC(undo_func);
  }
}

void ui_comm_ask_for_dodlg_open_ex(rect_t *p_dlg_rc, u16 strid,
  do_func_withpara_t do_func, u32 para1, u32 para2, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  dlg_ret_t ret;

  fill_dlg_data(&dlg_data,
                DLG_FOR_ASK | DLG_STR_MODE_STATIC,
                strid, p_dlg_rc, tm_out);

  ret = ui_comm_dlg_open(&dlg_data);

  if (ret == DLG_RET_YES)
  {
    DO_DLG_FUNC_WITHPARA(do_func,para1,para2);
  }
}



BOOL ui_comm_ask_for_savdlg_open(rect_t *p_dlg_rc,
                                   u16 strid,
                                   do_func_t do_save,
                                   do_func_t undo_save, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  dlg_ret_t ret;

  fill_dlg_data(&dlg_data,
                DLG_FOR_ASK | DLG_STR_MODE_STATIC,
                strid, p_dlg_rc, tm_out);

  ret = ui_comm_dlg_open2(&dlg_data);
  if (ret == DLG_RET_YES)
  {
    ui_comm_savdlg_open(p_dlg_rc, do_save, 0);
    return TRUE;
  }
  else
  {
    DO_DLG_FUNC(undo_save);
    return FALSE;

  }
}

void ui_comm_showdlg_open(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out)
{
  comm_dlg_data_t dlg_data;
  dlg_ret_t ret;
  
  fill_dlg_data(&dlg_data, DLG_FOR_SHOW | DLG_STR_MODE_STATIC, strid, p_dlg_rc, tm_out);
  ret = ui_comm_dlg_open2(&dlg_data);
  
  //DO_DLG_FUNC(do_cmf);
  //ui_comm_dlg_close();
}




static RET_CODE on_dlg_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_dlg_close();
  return SUCCESS;
}


static RET_CODE on_dlg_cancel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
#if 0
  control_t *p_btn;

  p_btn = ctrl_get_child_by_id(p_ctrl, IDC_BTN_NO);

  if(p_btn != NULL)
  {
    ctrl_set_active_ctrl(p_ctrl, p_btn);
  }
#else
  ctrl_set_active_ctrl(p_ctrl, NULL);
#endif
  ui_comm_dlg_close();
  return SUCCESS;
}

static RET_CODE on_dlg_destroy(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_btn;

  p_btn = ctrl_get_active_ctrl(p_ctrl);
  if (p_btn != NULL)
  {
    g_dlg_ret = ctrl_get_ctrl_id(p_btn) == IDC_BTN_YES ? \
      DLG_RET_YES : DLG_RET_NO;
  }
  else
  {
    g_dlg_ret = DLG_RET_NULL;
  }

  fw_tmr_destroy(ROOT_ID_POPUP, MSG_CANCEL);

  return ERR_NOFEATURE;
}

/*
  dlg2
*/
static RET_CODE on_dlg_2_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_dlg_close2();
  return SUCCESS;
}


static RET_CODE on_dlg_2_cancel(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_btn;

  p_btn = ctrl_get_child_by_id(p_ctrl, IDC_BTN_NO);

  if(p_btn != NULL)
  {
    ctrl_set_active_ctrl(p_ctrl, p_btn);
  }
  if(NULL!=fw_find_root_by_id(ROOT_ID_XSYS_SET))
  		swtich_to_sys_set(ROOT_ID_INVALID, 0);

  ui_comm_dlg_close2();
  return SUCCESS;
}

static RET_CODE on_dlg_2_timeout(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
   //ctrl_set_active_ctrl(p_ctrl, NULL);  
  ui_comm_dlg_close2();
  return SUCCESS;
}

static RET_CODE on_dlg_2_destroy(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_btn;

  p_btn = ctrl_get_active_ctrl(p_ctrl);
  if (p_btn != NULL)
  {
    g_dlg_ret = ctrl_get_ctrl_id(p_btn) == IDC_BTN_YES ? \
      DLG_RET_YES : DLG_RET_NO;
  }
  else
  {
    g_dlg_ret = DLG_RET_NULL;
  }

  fw_tmr_destroy(ROOT_ID_POPUP2, MSG_DLG_TIME_OUT);

  return ERR_NOFEATURE;
}

BEGIN_KEYMAP(comm_dlg_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
  ON_EVENT(V_KEY_BACK, MSG_CANCEL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
END_KEYMAP(comm_dlg_keymap, NULL)


BEGIN_MSGPROC(comm_dlg_proc, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_dlg_select)
  ON_COMMAND(MSG_CANCEL, on_dlg_cancel)
  ON_COMMAND(MSG_DESTROY, on_dlg_destroy)
END_MSGPROC(comm_dlg_proc, cont_class_proc)

BEGIN_KEYMAP(comm_dlg_keymap_2, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_CANCEL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_BACK, MSG_CANCEL)
  ON_EVENT(V_KEY_MENU, MSG_CANCEL)
END_KEYMAP(comm_dlg_keymap_2, NULL)

BEGIN_MSGPROC(comm_dlg_proc_2, cont_class_proc)
  ON_COMMAND(MSG_SELECT, on_dlg_2_select)
  ON_COMMAND(MSG_CANCEL, on_dlg_2_cancel)
  ON_COMMAND(MSG_DESTROY, on_dlg_2_destroy)
  ON_COMMAND(MSG_DLG_TIME_OUT, on_dlg_2_timeout)
END_MSGPROC(comm_dlg_proc_2, cont_class_proc)


