/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_num_play.h"
#include "ui_live_tv_api.h"
#include "ui_video.h"

enum control_id
{
  IDC_NUM_ICON = 1,
  IDC_NUM_FRM,
};

enum sub_control_id
{
  IDC_NUM_TXT = 1,
};

static comm_dlg_data_t dlg_data = //popup dialog data
{
  ROOT_ID_INVALID,
  DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
  COMM_DLG_X, COMM_DLG_Y,
  COMM_DLG_W, COMM_DLG_H,
  IDS_MSG_INVALID_NUMBER,
  2000,
};

static u8 g_num_play_bit;
static u16 g_prog_cnt;
BOOL is_fullsrc = FALSE;

u16 num_play_cont_keymap(u16 key);
RET_CODE num_play_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);



static void jump_to_prog(control_t *p_cont)
{
  control_t *p_ctrl, *p_subctrl;
  u16 curn;
  u16 prog_id;

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_NUM_FRM);
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_NUM_TXT);

  curn = (u16)nbox_get_num(p_subctrl);

  /* jump to prog */
  if (curn > g_prog_cnt || curn == 0)
  {
    ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
    ui_comm_dlg_open(&dlg_data);
    return;
  }
  
  if(is_fullsrc)
  {
    prog_id = db_dvbs_get_id_by_view_pos(ui_dbase_get_pg_view_id(), curn - 1 /* base on 1*/);
    ui_play_prog(prog_id, FALSE);
  }
  else
  {
    fw_notify_parent(ROOT_ID_NUM_PLAY, NOTIFY_T_MSG, 
      FALSE, MSG_NUM_SELECT, (curn - 1), 0);
  }
  
  /* close */
  ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
  return;
}

static BOOL input_num(control_t *parent, u8 num, BOOL is_update)
{
  control_t *p_ctrl, *p_subctrl;
  u32 curn;

  if (g_num_play_bit < NUM_PLAY_CNT)
  {
    p_ctrl = ctrl_get_child_by_id(parent, IDC_NUM_FRM);
    p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_NUM_TXT);

    curn = nbox_get_num(p_subctrl);
    curn = curn * 10 + num;
    nbox_set_num_by_dec(p_subctrl, curn);

    g_num_play_bit++;
    if (is_update)
    {
      ctrl_paint_ctrl(p_subctrl, TRUE);
    }

    return TRUE;
  }

  return FALSE;
}

RET_CODE open_num_play(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_ctrl, *p_subctrl;

  g_num_play_bit = 0;
  if ((g_prog_cnt = db_dvbs_get_count(ui_dbase_get_pg_view_id())) == 0)
  {
    return ERR_FAILURE;
  }

  is_fullsrc = ui_is_fullscreen_menu(fw_get_focus_id());
  DEBUG(MAIN,INFO,"is_fullsrc = %d \n",is_fullsrc);
  
  if(is_fullsrc)
  {
  	DEBUG(MAIN,INFO,"fullsrc\n");
    p_cont = fw_create_mainwin(ROOT_ID_NUM_PLAY,
                                    NUM_PLAY_CONT_X, NUM_PLAY_CONT_Y,
                                    NUM_PLAY_CONT_W, NUM_PLAY_CONT_H,
                                    ROOT_ID_INVALID, 0, 
                                    OBJ_ATTR_ACTIVE, 0);
    if (p_cont == NULL)
    {
      return ERR_FAILURE;
    }
    ctrl_set_rstyle(p_cont, RSI_IGNORE,
                    RSI_IGNORE, RSI_IGNORE);
    ctrl_set_keymap(p_cont, num_play_cont_keymap);
    ctrl_set_proc(p_cont, num_play_cont_proc);

    // frm
    p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_NUM_FRM,
                              NUM_PLAY_FRM_X, NUM_PLAY_FRM_Y,
                              NUM_PLAY_FRM_W, NUM_PLAY_FRM_H,
                              p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_NUM_PLAY_CONT,RSI_NUM_PLAY_CONT, RSI_NUM_PLAY_CONT);

    p_subctrl = ctrl_create_ctrl(CTRL_NBOX, IDC_NUM_TXT,
                                 NUM_PLAY_TXT_X, NUM_PLAY_TXT_Y,
                                 NUM_PLAY_TXT_W, NUM_PLAY_TXT_H,
                                 p_ctrl, 0);
    ctrl_set_rstyle(p_subctrl, RSI_NUM_PLAY_TXT, RSI_NUM_PLAY_TXT, RSI_NUM_PLAY_TXT);
    nbox_set_align_type(p_subctrl, STL_CENTER | STL_VCENTER);
    nbox_set_font_style(p_subctrl, FSI_NUM_PLAY_TXT,FSI_NUM_PLAY_TXT, FSI_NUM_PLAY_TXT);
    nbox_set_num_type(p_subctrl, NBOX_NUMTYPE_DEC);
    nbox_set_range(p_subctrl, 0, 9999, NUM_PLAY_CNT);
  }
  else
  {
  	DEBUG(MAIN,INFO,"NO fullsrc\n");
    p_cont = fw_create_mainwin(ROOT_ID_NUM_PLAY,
                                    NUM_PLAY_CONT_LISTX, NUM_PLAY_CONT_LISTY,
                                    NUM_PLAY_CONT_LISTW, NUM_PLAY_CONT_LISTH,
                                    ROOT_ID_PROG_LIST, 0, 
                                    OBJ_ATTR_ACTIVE, 0);
    if (p_cont == NULL)
    {
      return ERR_FAILURE;
    }
    ctrl_set_rstyle(p_cont, RSI_IGNORE,RSI_IGNORE, RSI_IGNORE);
    ctrl_set_keymap(p_cont, num_play_cont_keymap);
    ctrl_set_proc(p_cont, num_play_cont_proc);

    // frm
    p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_NUM_FRM,
                              NUM_PLAY_FRM_LISTX, NUM_PLAY_FRM_LISTY,
                              NUM_PLAY_FRM_LISTW, NUM_PLAY_FRM_LISTH,
                              p_cont, 0);
    ctrl_set_rstyle(p_ctrl, RSI_NUM_PLAY_CONT, RSI_NUM_PLAY_CONT, RSI_NUM_PLAY_CONT);

    p_subctrl = ctrl_create_ctrl(CTRL_NBOX, IDC_NUM_TXT,
                                 NUM_PLAY_TXT_LISTX, NUM_PLAY_TXT_LISTY,
                                 NUM_PLAY_TXT_LISTW, NUM_PLAY_TXT_LISTH,
                                 p_ctrl, 0);
    ctrl_set_rstyle(p_subctrl, RSI_NUM_PLAY_TXT, RSI_NUM_PLAY_TXT, RSI_NUM_PLAY_TXT);
    nbox_set_align_type(p_subctrl, STL_CENTER | STL_VCENTER);
    nbox_set_font_style(p_subctrl, FSI_WHITE,FSI_WHITE, FSI_WHITE);
    nbox_set_num_type(p_subctrl, NBOX_NUMTYPE_DEC);
    nbox_set_range(p_subctrl, 0, 9999, NUM_PLAY_CNT);
  }
  input_num(p_cont, (u8)(para1 - V_KEY_0), FALSE);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  /* create tmr for jump */
  fw_tmr_create(ROOT_ID_NUM_PLAY, MSG_SELECT, 3000, FALSE);

  return SUCCESS;
}

static RET_CODE on_input_num(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u8 num = (u8)(MSG_DATA_MASK & msg);
	input_num(p_ctrl, num, TRUE);
	return SUCCESS;
}

static RET_CODE on_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	jump_to_prog(p_ctrl);
	return SUCCESS;
}


static RET_CODE on_exit_num_play(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	fw_tmr_destroy(ROOT_ID_NUM_PLAY, MSG_SELECT);
	return ERR_NOFEATURE;
}

BEGIN_KEYMAP(num_play_cont_keymap, ui_comm_root_keymap)
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
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(num_play_cont_keymap, ui_comm_root_keymap)


BEGIN_MSGPROC(num_play_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_NUMBER, on_input_num)
  ON_COMMAND(MSG_SELECT, on_select)
  ON_COMMAND(MSG_EXIT, on_exit_num_play)
END_MSGPROC(num_play_cont_proc, ui_comm_root_proc)


