/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_freq_set.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_FREQ_SET_FREQ,
#ifndef DTMB_PROJECT
  IDC_FREQ_SET_SYM,
  IDC_FREQ_SET_DEMOD,
#endif
  IDC_FREQ_SET_OK,
};

static u16 freq_set_cont_keymap(u16 key);



static RET_CODE freq_set_cont_proc(control_t *cont, u16 msg, u32 para1,
                                            u32 para2);

#ifndef DTMB_PROJECT
static RET_CODE freq_set_cbox_proc(control_t *p_cbox, u16 msg, u32 para1,
                                            u32 para2);
#endif
static RET_CODE freq_set_nbox_proc(control_t *p_cbox, u16 msg, u32 para1,
                                            u32 para2);

u16 freq_set_select_keymap(u16 key);

RET_CODE freq_set_select_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

RET_CODE open_freq_set(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[FREQ_SET_ITEM_CNT];
  u8 i = 0;
#ifdef DTMB_PROJECT
  u16 stxt [FREQ_SET_ITEM_CNT] = {IDS_FREQUECY, IDS_OK};
#else
  u16 stxt [FREQ_SET_ITEM_CNT] = {IDS_FREQUECY, IDS_SYMBOL, IDS_MODULATION, IDS_OK};
#endif
  u16 y;

  dvbc_lock_t tp = {0};
  sys_status_get_main_tp1(&tp);

   p_cont = fw_create_mainwin(ROOT_ID_FREQ_SET,
							   FREQ_SET_X, FREQ_SET_Y,
							   FREQ_SET_W, FREQ_SET_H,
                             ROOT_ID_XSYS_SET, 0, OBJ_ATTR_ACTIVE, 0);
  ctrl_set_rstyle(p_cont,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, freq_set_cont_keymap);
  ctrl_set_proc(p_cont, freq_set_cont_proc);

  OS_PRINTF("iten count[%d]\n",FREQ_SET_ITEM_CNT);
  OS_PRINTF("__line__%d\n",__LINE__);
  
  y = FREQ_SET_ITEM_Y;
  for (i = 0; i < FREQ_SET_ITEM_CNT; i++)
  {
#ifdef DTMB_PROJECT
  OS_PRINTF("__line__%d\n",__LINE__);
    switch (i)
    {
      case 0:
        p_ctrl[i] = ui_comm_numedit_create(p_cont, (u8)(IDC_FREQ_SET_FREQ + i),
          FREQ_SET_ITEM_X, y, FREQ_SET_ITEM_LW, FREQ_SET_ITEM_RW);
        ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], freq_set_nbox_proc);
        ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID,
        SEARCH_FREQ_MIN, SEARCH_FREQ_MAX, SEARCH_FREQ_BIT, SEARCH_FREQ_BIT);
        if(tp.tp_freq < SEARCH_FREQ_MIN)
          tp.tp_freq = SEARCH_FREQ_MIN;
        if(tp.tp_freq > SEARCH_FREQ_MAX)
          tp.tp_freq = SEARCH_FREQ_MAX;
		ui_comm_numedit_set_postfix(p_ctrl[i], IDS_UNIT_MHZ);
		ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
        ui_comm_numedit_set_num(p_ctrl[i], tp.tp_freq);
        break;
      case 1:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_FREQ_SET_FREQ + i), 
                                        FREQ_SET_ITEM_X, y,
                                        FREQ_SET_ITEM_LW + FREQ_SET_ITEM_RW, FREQ_SET_ITEM_H,
                                        p_cont, 0);
        ctrl_set_keymap(p_ctrl[i], freq_set_select_keymap);
        ctrl_set_proc(p_ctrl[i], freq_set_select_proc);
        ctrl_set_rstyle(p_ctrl[i],RSI_PBACK,RSI_SELECT_F,RSI_PBACK);
        text_set_font_style(p_ctrl[i],FSI_COMM_CTRL_SH,FSI_COMM_CTRL_HL,FSI_COMM_CTRL_GRAY);
        text_set_align_type(p_ctrl[i], STL_LEFT | STL_VCENTER);
        text_set_offset(p_ctrl[i], COMM_CTRL_OX, 0);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl[i], stxt[i]);
        break;
      default:
        MT_ASSERT(0);
        break;
    }
#else
  OS_PRINTF("__line__%d\n",__LINE__);

    switch (i)
    {
      case 0:
      case 1:
        p_ctrl[i] = ui_comm_numedit_create(p_cont, (u8)(IDC_FREQ_SET_FREQ + i),
          FREQ_SET_ITEM_X, y, FREQ_SET_ITEM_LW, FREQ_SET_ITEM_RW);
        ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], freq_set_nbox_proc);
        if(i == 0)
        {
          ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID,
            SEARCH_FREQ_MIN, SEARCH_FREQ_MAX, SEARCH_FREQ_BIT, (SEARCH_FREQ_BIT - 1));
          ui_comm_numedit_set_postfix(p_ctrl[i], IDS_UNIT_MHZ);
          ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
          ui_comm_numedit_set_num(p_ctrl[i], tp.tp_freq);
        }
        else if(i == 1)
        {
          ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID,
            SEARCH_SYM_MIN, SEARCH_SYM_MAX, SEARCH_SYM_BIT, (SEARCH_FREQ_BIT - 1));
          ui_comm_numedit_set_postfix(p_ctrl[i], IDS_UNIT_MBAUD);
          ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
          ui_comm_numedit_set_num(p_ctrl[i], tp.tp_sym);
        }
        break;
      case 2:
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_FREQ_SET_FREQ + i),
          FREQ_SET_ITEM_X, y, FREQ_SET_ITEM_LW, FREQ_SET_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], freq_set_cbox_proc);
          ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
            9, CBOX_ITEM_STRTYPE_STRID, NULL);
        ui_comm_select_set_content(p_ctrl[i], 0, IDS_AUTO);
        ui_comm_select_set_content(p_ctrl[i], 1, IDS_BPSK);
        ui_comm_select_set_content(p_ctrl[i], 2, IDS_QPSK);
        ui_comm_select_set_content(p_ctrl[i], 3, IDS_8PSK);
        ui_comm_select_set_content(p_ctrl[i], 4, IDS_QAM16);
        ui_comm_select_set_content(p_ctrl[i], 5, IDS_QAM32);
        ui_comm_select_set_content(p_ctrl[i], 6, IDS_QAM64);
        ui_comm_select_set_content(p_ctrl[i], 7, IDS_QAM128);
        ui_comm_select_set_content(p_ctrl[i], 8, IDS_QAM256);
        ui_comm_select_set_focus(p_ctrl[i], tp.nim_modulate);
        break;
      case 3:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_FREQ_SET_FREQ + i), 
                                        FREQ_SET_ITEM_X, y,
                                        FREQ_SET_ITEM_LW + FREQ_SET_ITEM_RW, FREQ_SET_ITEM_H,
                                        p_cont, 0);
        ctrl_set_keymap(p_ctrl[i], freq_set_select_keymap);
        ctrl_set_proc(p_ctrl[i], freq_set_select_proc);
        ctrl_set_rstyle(p_ctrl[i],RSI_PBACK,RSI_SELECT_F,RSI_PBACK);
        text_set_font_style(p_ctrl[i],FSI_COMM_CTRL_SH,FSI_COMM_CTRL_HL,FSI_COMM_CTRL_GRAY);
        text_set_align_type(p_ctrl[i], STL_LEFT | STL_VCENTER);
        text_set_offset(p_ctrl[i], COMM_CTRL_OX, 0);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl[i], stxt[i]);
        break;
      default:
        MT_ASSERT(0);
        break;
    }
#endif

    ctrl_set_related_id(p_ctrl[i],
                        0,                                     /* left */
                        (u8)((i - 1 +
                              FREQ_SET_ITEM_CNT) %
                             FREQ_SET_ITEM_CNT + 1),            /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % FREQ_SET_ITEM_CNT + 1)); /* down */

    y += FREQ_SET_ITEM_H + FREQ_SET_ITEM_V_GAP;
  }
  OS_PRINTF("__line__%d\n",__LINE__);

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); 
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  OS_PRINTF("__line__%d\n",__LINE__);

  return SUCCESS;
}

#ifndef DTMB_PROJECT
//combobox
static RET_CODE on_freq_set_cbox_changed(control_t *p_cbox, u16 msg, u32 para1,
                             	   u32 para2)
{
  cbox_class_proc(p_cbox, msg, para1, para2);

  return SUCCESS;
}
#endif

static RET_CODE on_freq_set_change_freq_symbol(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_num_proc(p_ctrl, msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_freq_set_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  dvbc_lock_t tp1 = {0};

  p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_FREQ_SET_FREQ);
  tp1.tp_freq = ui_comm_numedit_get_num(p_ctrl);

#ifndef DTMB_PROJECT
  p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_FREQ_SET_SYM);
  tp1.tp_sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_FREQ_SET_DEMOD);
  tp1.nim_modulate = (u8)ui_comm_select_get_focus(p_ctrl);
#endif

  sys_status_set_main_tp1(&tp1);

  sys_status_save();
  manage_close_menu(ROOT_ID_FREQ_SET, 0, 0);
  
  return ERR_NOFEATURE;
}

static RET_CODE on_freq_set_destroy(control_t *cont, u16 msg, u32 para1,
                            u32 para2)
{
   return ERR_NOFEATURE;
}


BEGIN_KEYMAP(freq_set_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(freq_set_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(freq_set_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, on_freq_set_destroy)
END_MSGPROC(freq_set_cont_proc, ui_comm_root_proc)

#ifndef DTMB_PROJECT
BEGIN_MSGPROC(freq_set_cbox_proc, cbox_class_proc)
	ON_COMMAND(MSG_CHANGED, on_freq_set_cbox_changed)
END_MSGPROC(freq_set_cbox_proc, cbox_class_proc)
#endif

BEGIN_MSGPROC(freq_set_nbox_proc, ui_comm_num_proc)
  ON_COMMAND(MSG_NUMBER, on_freq_set_change_freq_symbol)
END_MSGPROC(freq_set_nbox_proc, ui_comm_num_proc)


BEGIN_KEYMAP(freq_set_select_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(freq_set_select_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(freq_set_select_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_freq_set_ok)
END_MSGPROC(freq_set_select_proc, text_class_proc)

