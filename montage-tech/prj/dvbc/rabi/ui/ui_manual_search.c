/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_manual_search.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_MANUAL_SEARCH_FREQ,
  IDC_MANUAL_SEARCH_SYM,
  IDC_MANUAL_SEARCH_DEMOD,
  IDC_MANUAL_SEARCH_NIT,
  IDC_MANUAL_SEARCH_START,
};

static u16 manual_search_cont_keymap(u16 key);



static RET_CODE manual_search_cont_proc(control_t *cont, u16 msg, u32 para1,
                                            u32 para2);

static RET_CODE manual_search_cbox_proc(control_t *p_cbox, u16 msg, u32 para1,
                                            u32 para2);
static RET_CODE manual_search_nbox_proc(control_t *p_cbox, u16 msg, u32 para1,
                                            u32 para2);

u16 manual_search_select_keymap(u16 key);

RET_CODE manual_search_select_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

static void manual_search_set_transpond(control_t *p_cont)
{
  dvbs_tp_node_t tp = {0};
  control_t *p_ctrl;
  u32 freq;
  u32 sym;
  u8 demod;
  ui_scan_param_t scan_param;
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;

  memset(&scan_param, 0, sizeof(ui_scan_param_t));
  
  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_MANUAL_SEARCH_FREQ);
  freq = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_MANUAL_SEARCH_SYM);
  sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_MANUAL_SEARCH_DEMOD);
  demod = (u8)ui_comm_select_get_focus(p_ctrl);
  
  tp.freq = freq;
  tp.sym = sym;

  switch(demod)
  {
    case 0:
      nim_modul =NIM_MODULA_AUTO;
      break;

    case 1:
      nim_modul = NIM_MODULA_BPSK;      
      break;

    case 2:
      nim_modul = NIM_MODULA_QPSK;      
      break;

    case 3:
      nim_modul = NIM_MODULA_8PSK;      
      break;

    case 4:
      nim_modul = NIM_MODULA_QAM16;
      break;
      
    case 5:
      nim_modul = NIM_MODULA_QAM32;
      break;

    case 6:
      nim_modul = NIM_MODULA_QAM64;      
      break;

    case 7:
      nim_modul = NIM_MODULA_QAM128;
      break;

    case 8:
      nim_modul = NIM_MODULA_QAM256;
      break;
      
    default:
      nim_modul = NIM_MODULA_QAM64;
      break;
  }

  tp.nim_modulate = nim_modul;
  mtos_printk("!!!!!!!!!!!!! tp.freq = %d\n", tp.freq);
  mtos_printk("!!!!!!!!!!!!! tp.sym = %d\n", tp.sym);
  mtos_printk("!!!!!!!!!!!!! tp.nim_modulate = %d\n", tp.nim_modulate);

  ui_set_transpond(&tp);

}

static RET_CODE start_manual_scan(control_t *cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_ctrl;
  u32 freq;
  u32 sym;
  u8 demod;
  u8 nit;
  ui_scan_param_t scan_param;
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;

  memset(&scan_param, 0, sizeof(ui_scan_param_t));
  
  p_ctrl = ctrl_get_child_by_id(cont, IDC_MANUAL_SEARCH_FREQ);
  freq = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_MANUAL_SEARCH_SYM);
  sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_MANUAL_SEARCH_DEMOD);
  demod = (u8)ui_comm_select_get_focus(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_MANUAL_SEARCH_NIT);
  nit = (u8)ui_comm_select_get_focus(p_ctrl);

  scan_param.tp.freq = freq;
  scan_param.tp.sym = sym;
  
  switch(demod)
  {
    case 0:
      nim_modul =NIM_MODULA_AUTO;
      break;

    case 1:
      nim_modul = NIM_MODULA_BPSK;      
      break;

    case 2:
      nim_modul = NIM_MODULA_QPSK;      
      break;

    case 3:
      nim_modul = NIM_MODULA_8PSK;      
      break;

    case 4:
      nim_modul = NIM_MODULA_QAM16;
      break;
      
    case 5:
      nim_modul = NIM_MODULA_QAM32;
      break;

    case 6:
      nim_modul = NIM_MODULA_QAM64;      
      break;

    case 7:
      nim_modul = NIM_MODULA_QAM128;
      break;

    case 8:
      nim_modul = NIM_MODULA_QAM256;
      break;
      
    default:
      nim_modul = NIM_MODULA_QAM64;
      break;
  }

  scan_param.tp.nim_modulate = nim_modul;
  scan_param.tp.nim_type = NIM_DVBC;

  switch(nit)
  {
    case 0:
      scan_param.nit_type = NIT_SCAN_WITHOUT;
      break;
    case 1:
#ifdef AISET_BOUQUET_SUPPORT
      scan_param.nit_type = NIT_SCAN_ALL_TP;
#else
      scan_param.nit_type = NIT_SCAN_ONCE;
#endif
      break;
  }

  manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_MANUAL, (u32)&scan_param);

  return SUCCESS;
}

RET_CODE open_manual_search(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[MANUAL_SEARCH_ITEM_CNT];
  u8 i = 0;
  u16 stxt [MANUAL_SEARCH_ITEM_CNT] = {IDS_FREQUECY, IDS_SYMBOL, IDS_MODULATION, IDS_NIT_SEARCH, IDS_SEARCH};
  u16 y;
  dvbc_lock_t tp = {0};
  sys_status_get_main_tp1(&tp);

  p_cont = ui_comm_root_create_with_signbar(ROOT_ID_MANUAL_SEARCH, 0,
                                  COMM_BG_X, COMM_BG_Y,
                                  COMM_BG_W, COMM_BG_H,
                                  0, IDS_MANUAL_SEARCH, FALSE);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, manual_search_cont_keymap);
  ctrl_set_proc(p_cont, manual_search_cont_proc);

  y = MANUAL_SEARCH_ITEM_Y;
  for (i = 0; i < MANUAL_SEARCH_ITEM_CNT; i++)
  {
    switch (i)
    {
      case 0:
      case 1:
        p_ctrl[i] = ui_comm_numedit_create(p_cont, (u8)(IDC_MANUAL_SEARCH_FREQ + i),
          MANUAL_SEARCH_ITEM_X, y, MANUAL_SEARCH_ITEM_LW, MANUAL_SEARCH_ITEM_RW);
        ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], manual_search_nbox_proc);
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
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_MANUAL_SEARCH_FREQ + i),
          MANUAL_SEARCH_ITEM_X, y, MANUAL_SEARCH_ITEM_LW, MANUAL_SEARCH_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], manual_search_cbox_proc);
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
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_MANUAL_SEARCH_FREQ + i),
                                            MANUAL_SEARCH_ITEM_X, y,
                                            MANUAL_SEARCH_ITEM_LW,
                                            MANUAL_SEARCH_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE,
                                   CBOX_WORKMODE_STATIC, 2,
                                   CBOX_ITEM_STRTYPE_STRID,
                                   NULL);
        ui_comm_select_set_content(p_ctrl[i], 0, IDS_NO);
        ui_comm_select_set_content(p_ctrl[i], 1, IDS_YES);
        ui_comm_select_set_focus(p_ctrl[i], 0);
        break;
      case 4:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_MANUAL_SEARCH_FREQ + i), 
                                        MANUAL_SEARCH_ITEM_X, y,
                                        MANUAL_SEARCH_ITEM_LW + MANUAL_SEARCH_ITEM_RW, MANUAL_SEARCH_ITEM_H,
                                        p_cont, 0);
        ctrl_set_keymap(p_ctrl[i], manual_search_select_keymap);
        ctrl_set_proc(p_ctrl[i], manual_search_select_proc);
        ctrl_set_rstyle(p_ctrl[i],
                        RSI_COMM_CONT_SH,
                        RSI_COMM_CONT_HL,
                        RSI_COMM_CONT_GRAY);
        text_set_font_style(p_ctrl[i],
                            FSI_COMM_CTRL_SH,
                            FSI_COMM_CTRL_HL,
                            FSI_COMM_CTRL_GRAY);
        text_set_align_type(p_ctrl[i], STL_LEFT | STL_VCENTER);
        text_set_offset(p_ctrl[i], COMM_CTRL_OX, 0);
        text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_ctrl[i], stxt[i]);
        break;
      default:
        MT_ASSERT(0);
        break;
    }

    ctrl_set_related_id(p_ctrl[i],
                        0,                                     /* left */
                        (u8)((i - 1 +
                              MANUAL_SEARCH_ITEM_CNT) %
                             MANUAL_SEARCH_ITEM_CNT + 1),            /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % MANUAL_SEARCH_ITEM_CNT + 1)); /* down */

    y += MANUAL_SEARCH_ITEM_H + MANUAL_SEARCH_ITEM_V_GAP;
  }

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); 
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

//combobox
static RET_CODE on_manual_search_cbox_changed(control_t *p_cbox, u16 msg, u32 para1,
                             	   u32 para2)
{
  cbox_class_proc(p_cbox, msg, para1, para2);
  manual_search_set_transpond(ctrl_get_parent(ctrl_get_parent(p_cbox)));

  return SUCCESS;
}

static RET_CODE on_manual_search_change_freq_symbol(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_num_proc(p_ctrl, msg, para1, para2);
  manual_search_set_transpond(p_ctrl->p_parent->p_parent);
  return SUCCESS;
}

static RET_CODE on_select_msg_start_search(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  start_manual_scan(p_ctrl->p_parent, msg, para1, para2);
  return SUCCESS;
}
static RET_CODE on_manual_search_destroy(control_t *cont, u16 msg, u32 para1,
                            u32 para2)
{
   return ERR_NOFEATURE;
}


BEGIN_KEYMAP(manual_search_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(manual_search_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(manual_search_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, on_manual_search_destroy)
END_MSGPROC(manual_search_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(manual_search_cbox_proc, cbox_class_proc)
	ON_COMMAND(MSG_CHANGED, on_manual_search_cbox_changed)
END_MSGPROC(manual_search_cbox_proc, cbox_class_proc)

BEGIN_MSGPROC(manual_search_nbox_proc, ui_comm_num_proc)
  ON_COMMAND(MSG_NUMBER, on_manual_search_change_freq_symbol)
END_MSGPROC(manual_search_nbox_proc, ui_comm_num_proc)

BEGIN_KEYMAP(manual_search_select_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(manual_search_select_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(manual_search_select_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_select_msg_start_search)
END_MSGPROC(manual_search_select_proc, text_class_proc)

