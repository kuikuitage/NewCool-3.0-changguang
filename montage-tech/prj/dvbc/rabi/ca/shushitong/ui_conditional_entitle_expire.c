/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 ****************************************************************************/
#include "ui_common.h"

#include "ui_conditional_entitle_expire.h"

static u16 comm_ca_entitle_expire_keymap(u16 key);
static RET_CODE comm_ca_entitle_expire_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

enum control_id
{
  IDC_INVALID = 0,
  IDC_TITLE,
  IDC_CONTENT,
  IDC_ICON,
};

enum local_msg
{
  MSG_CA_CLOSE_TIMER = MSG_LOCAL_BEGIN + 1160,
  MSG_CA_PARWAIT_TIMER,
};

static u8 TIMER_FLAG = 0;

static void get_entitle_str(u16 * uni_str, u16 str_id1,u16 str_id2, u32 content)
{
  u16 str[10] = {0}; 
  u16 len = 0;
  
  gui_get_string(str_id1, uni_str, 64);
  convert_i_to_dec_str(str, content); 
  uni_strcat(uni_str, str,64);
  
  len = (u16)uni_strlen(uni_str);
  gui_get_string(str_id2, str, 64 - len); 
  uni_strcat(uni_str, str, 64); 

  return;
}


RET_CODE open_ca_entitle_expire_dlg(u32 para1, u32 para2)
{
  control_t *p_content, *p_title, *p_icon, *p_cont = NULL;
  u16 uni_str[64] = {0};
  
  #ifndef WIN32
  u8 reminds = *(u8*)para2;
  #else
  u8 reminds = 10;
  #endif
  
  if((!ui_is_fullscreen_menu(fw_get_focus_id()) )|| (fw_find_root_by_id(ROOT_ID_CA_ENTITLE_EXP_DLG) != NULL))
  {
    return ERR_FAILURE;
  }
  p_cont = fw_create_mainwin(ROOT_ID_CA_ENTITLE_EXP_DLG,
                           ENTITLE_CONT_FULL_X, ENTITLE_CONT_FULL_Y,
                           ENTITLE_CONT_FULL_W, ENTITLE_CONT_FULL_H,
                           ROOT_ID_INVALID, 0,
                           OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
  return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_keymap(p_cont, comm_ca_entitle_expire_keymap);
  ctrl_set_proc(p_cont, comm_ca_entitle_expire_proc);
  //title
  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_TITLE,
                         ENTITLE_TITLE_FULL_X, ENTITLE_TITLE_FULL_Y,
                         ENTITLE_TITLE_FULL_W,ENTITLE_TITLE_FULL_H,
                         p_cont, 0);

  ctrl_set_rstyle(p_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_title, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);

  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_CA_ACCREDIT_INFO);
  //content
  p_content = ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
                       ENTITLE_CONTENT_FULL_X, ENTITLE_CONTENT_FULL_Y,
                       ENTITLE_CONTENT_FULL_W, ENTITLE_CONTENT_FULL_H,
                       p_cont, 0);
  ctrl_set_rstyle(p_content, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_content, FSI_WHITE_24,FSI_WHITE_24,FSI_WHITE_24);
  text_set_align_type(p_content, STL_CENTER| STL_VCENTER);
  text_set_content_type(p_content, TEXT_STRTYPE_UNICODE);
  get_entitle_str(uni_str,IDS_CA_ENTITLE_EXPIRED_DIVI, IDS_CA_RENEW, reminds);
  text_set_content_by_unistr(p_content, uni_str);
  
  //icon
  p_icon = ctrl_create_ctrl(CTRL_BMAP, IDC_ICON,
                   10, 0,
                   40, 40,
                   p_cont, 0);
  bmap_set_content_by_id(p_icon, IM_M_ICON_F4V);//yyf IM_ICON_WARNING);


#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif
  if(p_cont != NULL)
  {
    ctrl_paint_ctrl(p_cont, FALSE);
  }
  TIMER_FLAG = fw_tmr_create(ROOT_ID_CA_ENTITLE_EXP_DLG, MSG_CA_CLOSE_TIMER, 60000, FALSE);//wait 60s
  return SUCCESS;
}


void close_ca_entitle_expire_dlg(void)
{
  fw_destroy_mainwin_by_id(ROOT_ID_CA_ENTITLE_EXP_DLG);
}


static RET_CODE on_ca_entitle_expire_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if (TIMER_FLAG == SUCCESS) {
    fw_tmr_destroy(ROOT_ID_CA_ENTITLE_EXP_DLG, MSG_CA_CLOSE_TIMER);
  }
  close_ca_entitle_expire_dlg();
  return SUCCESS;  
}

BEGIN_KEYMAP(comm_ca_entitle_expire_keymap, NULL)
  ON_COMMAND(V_KEY_MENU, MSG_EXIT)
  ON_COMMAND(V_KEY_BACK, MSG_EXIT)
  ON_COMMAND(V_KEY_EXIT, MSG_EXIT)
  ON_COMMAND(V_KEY_CANCEL, MSG_EXIT)
END_KEYMAP(comm_ca_entitle_expire_keymap, NULL)

BEGIN_MSGPROC(comm_ca_entitle_expire_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_ca_entitle_expire_exit)
  ON_COMMAND(MSG_CA_CLOSE_TIMER, on_ca_entitle_expire_exit)
END_MSGPROC(comm_ca_entitle_expire_proc, cont_class_proc)

