/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_xsys_set.h"
#include "ui_keyboard_v2.h"
#include "ui_video.h"
#include "ui_xstb_name.h"
#include "cdlna.h"

enum control_id
{
  IDC_XINVALID = 0,
  IDC_XSTB_NAME,
  IDC_XSTB_HELP_CONT
};
/*
static comm_help_data_t xstbname_help_data = 
{
  3, 3,
  { IDS_MOVE, 
    IDS_OK,
    IDS_EXIT},
  { IM_HELP_ARROW,
    IM_HELP_OK,
    IM_HELP_MENU}
};
*/

static u8 g_xstb_name[128];
static u8 g_temp_xstb_name[128];
static char g_xori_uni_content[128];

static u16 xstbname_cont_keymap(u16 key);
static RET_CODE xstbname_select_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE xstbname_change_name_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static control_t *create_xstbname_menu(obj_attr_t obj_attr)
{
  control_t *p_cont = NULL;
  control_t *p_xstb_name = NULL;
//  control_t *p_name_help = NULL;
  char xstb_name[MAX_XDEVICE_NAME_LENGTH];
  
  /*create the main window*/
  if(NULL!=fw_find_root_by_id(ROOT_ID_XSTB_NAME))
		manage_close_menu(ROOT_ID_XSTB_NAME, 0, 0);
  p_cont = fw_create_mainwin(ROOT_ID_XSTB_NAME,
							   XSTB_NAME_X, XSTB_NAME_Y,
							   XSTB_NAME_W, XSTB_NAME_H,
                             ROOT_ID_XSYS_SET, 0,obj_attr, 0);
  ctrl_set_rstyle(p_cont,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG);
  ctrl_set_keymap(p_cont, xstbname_cont_keymap);
  ctrl_set_proc(p_cont, xstbname_select_proc);
  
  /*create name*/  
  sys_status_get_dlna_device_name(xstb_name);
  p_xstb_name = ui_comm_t9_v2_edit_create(p_cont, IDC_XSTB_NAME,
  					XSTB_NAME_ITEM_X, XSTB_NAME_ITEM_Y,
					 XSTB_NAME_ITEM_LW, XSTB_NAME_ITEM_RW, ROOT_ID_DLNA_DMR);
  ui_comm_t9_v2_edit_set_static_txt(p_xstb_name, IDS_STB_NAME);
  ui_comm_t9_v2_edit_set_content_by_ascstr(p_xstb_name, xstb_name);
  ui_comm_ctrl_set_proc(p_xstb_name, xstbname_change_name_proc);

  /*help bar */
  /*p_name_help = ctrl_create_ctrl(CTRL_CONT, IDC_XSTB_HELP_CONT,
  	                                  XSTB_NAME_ITEM_X, XSTB_NAME_H - 150,
  	                                  XSTB_NAME_ITEM_LW + XSTB_NAME_ITEM_RW, 80,
  	                                  p_cont, 0);
  ctrl_set_rstyle(p_name_help, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  ui_comm_help_create(&xstbname_help_data, p_name_help);*/
  
  /*set the name focus*/
  if(obj_attr != OBJ_ATTR_INACTIVE)
	  ctrl_default_proc(p_xstb_name, MSG_GETFOCUS, 0, 0); /* focus on xstb_name */
  return p_cont;
}


/*the exit function*/
static RET_CODE on_xstbname_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_XSTB_NAME, 0, 0);
  return swtich_to_sys_set(ROOT_ID_XSTB_NAME, 0);
}


static void save_device_name(void)
{
    control_t *p_edit_cont = NULL;
	int len = strlen(g_temp_xstb_name);

	p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_XSTB_NAME, IDC_XSTB_NAME);
	MT_ASSERT(p_edit_cont != NULL);
    if(len == 0 || len > 20)
    {
        OS_PRINTF("####%s, stb name cant be null####\n", __FUNCTION__);
        ui_comm_cfmdlg_open(NULL, IDS_NOT_AVAILABLE, NULL, 2000);
        ui_comm_t9_v2_edit_set_content_by_ascstr(p_edit_cont, g_xori_uni_content);
	ctrl_paint_ctrl(p_edit_cont, TRUE);
        return ;
    }
    strcpy(g_xstb_name, g_temp_xstb_name); 
	sys_status_set_dlna_device_name(g_xstb_name);
    ui_comm_t9_v2_edit_set_content_by_ascstr(p_edit_cont, g_xstb_name);
    ctrl_paint_ctrl(p_edit_cont, TRUE);
	#ifndef WIN32
    cg_dlna_rename(g_xstb_name, NULL, NULL);
	#endif
}

static void unsave_device_name(void)
{
	control_t *p_edit_cont;
	p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_XSTB_NAME, IDC_XSTB_NAME);
	MT_ASSERT(p_edit_cont != NULL);
	ui_comm_t9_v2_edit_set_content_by_ascstr(p_edit_cont, g_xori_uni_content);
	ctrl_paint_ctrl(p_edit_cont, TRUE);
}


RET_CODE edit_xstb_name_update(u16 *p_unistr)
{
  rect_t dlg_rc =
  {
  	RESTORE_ASK_L,
  	RESTORE_ASK_T,
  	RESTORE_ASK_R,
  	RESTORE_ASK_B,
  };

  str_uni2asc(g_temp_xstb_name, p_unistr);
 
  ui_comm_ask_for_savdlg_open(&dlg_rc, IDS_SAVE_NAME,(do_func_t)save_device_name, (do_func_t)unsave_device_name, 0);
  return SUCCESS;
}


static RET_CODE on_edit_xstb_name(control_t *p_ctrl,
                             u16 msg,
                             u32 para1,
                             u32 para2)
{
    kb_param_t param;  
    u8 device_name[64];

    param.uni_str = ebox_get_content(p_ctrl);
    str_uni2asc(device_name, param.uni_str);
    strcpy(g_xori_uni_content, device_name);
    param.type = KB_INPUT_TYPE_ENGLISH;
    param.max_len = MAX_XDEVICE_NAME_LENGTH;
    param.cb = edit_xstb_name_update;
    manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
    return SUCCESS;
}

RET_CODE open_xstbname(u32 para1, u32 para2)
{
  control_t *p_menu = NULL;

  obj_attr_t obj_attr = (obj_attr_t)para2;
  p_menu = create_xstbname_menu(obj_attr);
  MT_ASSERT(p_menu != NULL);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);

  return SUCCESS;
}

BEGIN_KEYMAP(xstbname_cont_keymap, NULL)
  ON_EVENT(V_KEY_MENU,MSG_EXIT)
  ON_EVENT(V_KEY_BACK,MSG_EXIT)
END_KEYMAP(xstbname_cont_keymap, NULL)

BEGIN_MSGPROC(xstbname_select_proc, ui_comm_root_proc)
ON_COMMAND(MSG_EXIT,on_xstbname_exit)
END_MSGPROC(xstbname_select_proc, ui_comm_root_proc)


BEGIN_MSGPROC(xstbname_change_name_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_edit_xstb_name)
  ON_COMMAND(MSG_NUMBER, on_edit_xstb_name)
END_MSGPROC(xstbname_change_name_proc, ui_comm_edit_proc)


