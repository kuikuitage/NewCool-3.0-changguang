/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_range_search.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_RANGE_SEARCH_FREQ_START,
  IDC_RANGE_SEARCH_FREQ_END,
  IDC_RANGE_SEARCH_SYM,
  IDC_RANGE_SEARCH_DEMOD,
  IDC_RANGE_SEARCH_START,
};

s32 g_DVBCFullFreqTable_Chi[] =
{
  115000,
  123000,
  131000,
  139000,
  147000,
  155000,
  163000,
  171000,
  179000,
  187000,
  195000,
  203000,
  211000,
  219000,
  227000,
  235000,
  243000,
  251000,
  259000,
  267000,
  275000,
  283000,
  291000,
  299000,
  307000,
  315000,
  323000,
  331000,
  339000,
  347000,
  355000,
  363000,
  371000,
  379000,
  387000,
  395000,
  403000,
  411000,
  419000,
  427000,
  435000,
  443000,
  451000,
  459000,
  467000,
  474000,
  482000,
  490000,
  498000,
  506000,
  514000,
  522000,
  530000,
  538000,
  546000,
  554000,
  562000,
  570000,
  578000,
  586000,
  594000,
  602000,
  610000,
  618000,
  626000,
  634000,
  642000,
  650000,
  658000,
  666000,
  674000,
  682000,
  690000,
  698000,
  706000,
  714000,
  722000,
  730000,
  738000,
  746000,
  754000,
  762000,
  770000,
  778000,
  786000,
  794000,
  802000,
  810000,
  818000,
  826000,
  834000,
  842000,
  850000,
  858000,
};

s32 g_DVBCFullFreqTable_Ind[] =
{    
  50000,
  58000,
  66000,
  74000,
  82000,
  90000,
  98000,
  106000,
  114000,
  122000,
  130000,
  138000,
  146000,
  154000,
  162000,
  170000,
  178000,
  186000,
  194000,
  202000,
  210000,
  218000,
  226000,
  234000,
  242000,
  250000,
  258000,
  266000,
  274000,
  282000,
  290000,
  298000,
  306000,
  314000,
  322000,
  330000,
  338000,
  346000,
  354000,
  362000,
  370000,
  378000,
  386000,
  394000,
  402000,
  410000,
  418000,
  426000,
  434000,
  442000,
  450000,
  458000,
  466000,
  474000,
  482000,
  490000,
  498000,
  506000,
  514000,
  522000,
  530000,
  538000,
  546000,
  554000,
  562000,
  570000,
  578000,
  586000,
  594000,
  602000,
  610000,
  618000,
  626000,
  634000,
  642000,
  650000,
  658000,
  666000,
  674000,
  682000,
  690000,
  698000,
  706000,
  714000,
  722000,
  730000,
  738000,
  746000,
  754000,
  762000,
  770000,
  778000,
  786000,
  794000,
  802000,
  810000,
  818000,
  826000,
  834000,
  842000,
  850000,
  858000,
};

static u8 full_tp_count = 0;
s32 *p_FreqTable = NULL;

static u16 range_search_cont_keymap(u16 key);



static RET_CODE range_search_cont_proc(control_t *cont, u16 msg, u32 para1,
                                            u32 para2);

static RET_CODE range_search_cbox_proc(control_t *p_cbox, u16 msg, u32 para1,
                                            u32 para2);
static RET_CODE range_search_nbox_proc(control_t *p_cbox, u16 msg, u32 para1,
                                            u32 para2);

u16 range_search_select_keymap(u16 key);

RET_CODE range_search_select_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

static void range_search_set_transpond(control_t *p_cont)
{
  dvbs_tp_node_t tp = {0};
  control_t *p_ctrl;
  u32 freq;
  u32 sym;
  u8 demod;
  ui_scan_param_t scan_param;
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;

  memset(&scan_param, 0, sizeof(ui_scan_param_t));
  
  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_RANGE_SEARCH_FREQ_START);
  freq = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_RANGE_SEARCH_SYM);
  sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_RANGE_SEARCH_DEMOD);
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

  ui_set_transpond(&tp);
}

static RET_CODE start_range_scan(control_t *cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_ctrl;
  u32 sym;
  u8 demod;
  ui_scan_param_t scan_param;
  sat_node_t sat_node = {0};
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;
  u32 freq_start, freq_end;
  u8 i;

  memset(&scan_param, 0, sizeof(ui_scan_param_t));
  
  p_ctrl = ctrl_get_child_by_id(cont, IDC_RANGE_SEARCH_FREQ_START);
  freq_start = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_RANGE_SEARCH_FREQ_END);
  freq_end = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_RANGE_SEARCH_SYM);
  sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_RANGE_SEARCH_DEMOD);
  demod = (u8)ui_comm_select_get_focus(p_ctrl);

  scan_param.tp.freq = freq_start;
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
  scan_param.nit_type = NIT_SCAN_ONCE;

  if(freq_start < freq_end)
  {
    ui_scan_param_init();
    ui_scan_add_dvbc_sat(&sat_node);
    ui_scan_param_add_sat(&sat_node);
    for(i =0; i< full_tp_count; i++)
    {
      if(p_FreqTable[i] == freq_start)
        break;
    }
    if(i == full_tp_count)
    {
      scan_param.tp.freq = freq_start;
      ui_scan_param_add_tp(&scan_param.tp);
    }
    for(i =0; i< full_tp_count; i++)
    {
      if( p_FreqTable[i] >= freq_start && p_FreqTable[i] <= freq_end)
      {
        scan_param.tp.freq = p_FreqTable[i];
        ui_scan_param_add_tp(&scan_param.tp);
      }
    }
    ui_scan_param_set_type(USC_DVBC_FULL_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);
    manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DVBC_RANGE, 0);
  }
  else if(freq_start == freq_end)
  {
    ui_scan_param_init();
    
    scan_param.tp.freq = freq_start;
    ui_scan_param_add_tp(&scan_param.tp);
    ui_scan_param_set_type(USC_DVBC_FULL_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);
    manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DVBC_RANGE, 0);
  }
  else
  {
    ui_comm_cfmdlg_open(NULL, IDS_FREQUENCY_RANGE_ERROR, NULL, 2000);
  }

  return SUCCESS;
}

RET_CODE open_range_search(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[RANGE_SEARCH_ITEM_CNT];
  u8 i = 0;
  u16 stxt [RANGE_SEARCH_ITEM_CNT] = {IDS_FREQUENCY_START, IDS_FREQUENCY_END, IDS_SYMBOL, IDS_MODULATION, IDS_SEARCH};
  u16 y;
  u8 country = COUNTRY_CHINA;
  dvbc_lock_t tp = {0};
  sys_status_get_main_tp1(&tp);

  switch(country)
  {
  case COUNTRY_INDIA:
    p_FreqTable = g_DVBCFullFreqTable_Ind;
    full_tp_count = sizeof(g_DVBCFullFreqTable_Ind)/sizeof(s32);
    break;
    
  case COUNTRY_CHINA:
    p_FreqTable = g_DVBCFullFreqTable_Chi;
    full_tp_count = sizeof(g_DVBCFullFreqTable_Chi)/sizeof(s32);
    break;
    
  default:
    p_FreqTable = g_DVBCFullFreqTable_Chi;
    full_tp_count = sizeof(g_DVBCFullFreqTable_Chi)/sizeof(s32);
    break;
  }

  p_cont = ui_comm_root_create_with_signbar(ROOT_ID_RANGE_SEARCH, 0,
                                  COMM_BG_X, COMM_BG_Y,
                                  COMM_BG_W, COMM_BG_H,
                                  0, IDS_RANGE_SEARCH, FALSE);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, range_search_cont_keymap);
  ctrl_set_proc(p_cont, range_search_cont_proc);

  y = RANGE_SEARCH_ITEM_Y;
  for (i = 0; i < RANGE_SEARCH_ITEM_CNT; i++)
  {
    switch (i)
    {
      case 0:
      case 1:
      case 2:
        p_ctrl[i] = ui_comm_numedit_create(p_cont, (u8)(IDC_RANGE_SEARCH_FREQ_START + i),
          RANGE_SEARCH_ITEM_X, y, RANGE_SEARCH_ITEM_LW, RANGE_SEARCH_ITEM_RW);
        ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], range_search_nbox_proc);
        if(i == 0)
        {
          ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID,
            SEARCH_FREQ_MIN, SEARCH_FREQ_MAX, SEARCH_FREQ_BIT, (SEARCH_FREQ_BIT - 1));
          ui_comm_numedit_set_postfix(p_ctrl[i], IDS_UNIT_MHZ);
          ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
          ui_comm_numedit_set_num(p_ctrl[i], p_FreqTable[0]);
        }
        else if(i == 1)
        {
          ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID,
            SEARCH_FREQ_MIN, SEARCH_FREQ_MAX, SEARCH_FREQ_BIT, (SEARCH_FREQ_BIT - 1));
          ui_comm_numedit_set_postfix(p_ctrl[i], IDS_UNIT_MHZ);
          ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
          ui_comm_numedit_set_num(p_ctrl[i], p_FreqTable[full_tp_count - 1]);
        }
        else if(i == 2)
        {
          ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID,
            SEARCH_SYM_MIN, SEARCH_SYM_MAX, SEARCH_SYM_BIT, (SEARCH_FREQ_BIT - 1));
          ui_comm_numedit_set_postfix(p_ctrl[i], IDS_UNIT_MBAUD);
          ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
          ui_comm_numedit_set_num(p_ctrl[i], tp.tp_sym);
        }
        break;
      case 3:
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_RANGE_SEARCH_FREQ_START + i),
          RANGE_SEARCH_ITEM_X, y, RANGE_SEARCH_ITEM_LW, RANGE_SEARCH_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], range_search_cbox_proc);
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
      case 4:
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_RANGE_SEARCH_FREQ_START + i), 
                                        RANGE_SEARCH_ITEM_X, y,
                                        RANGE_SEARCH_ITEM_LW + RANGE_SEARCH_ITEM_RW, RANGE_SEARCH_ITEM_H,
                                        p_cont, 0);
        ctrl_set_keymap(p_ctrl[i], range_search_select_keymap);
        ctrl_set_proc(p_ctrl[i], range_search_select_proc);
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
                              RANGE_SEARCH_ITEM_CNT) %
                             RANGE_SEARCH_ITEM_CNT + 1),            /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % RANGE_SEARCH_ITEM_CNT + 1)); /* down */

    y += RANGE_SEARCH_ITEM_H + RANGE_SEARCH_ITEM_V_GAP;
  }

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); 
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

//combobox
static RET_CODE on_range_search_cbox_changed(control_t *p_cbox, u16 msg, u32 para1,
                             	   u32 para2)
{
  cbox_class_proc(p_cbox, msg, para1, para2);
  range_search_set_transpond(ctrl_get_parent(ctrl_get_parent(p_cbox)));

  return SUCCESS;
}

static RET_CODE on_range_search_change_freq_symbol(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_num_proc(p_ctrl, msg, para1, para2);
  range_search_set_transpond(p_ctrl->p_parent->p_parent);
  return SUCCESS;
}

static RET_CODE on_select_msg_start_search(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  start_range_scan(p_ctrl->p_parent, msg, para1, para2);
  return SUCCESS;
}
static RET_CODE on_range_search_destroy(control_t *cont, u16 msg, u32 para1,
                            u32 para2)
{
   return ERR_NOFEATURE;
}


BEGIN_KEYMAP(range_search_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(range_search_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(range_search_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, on_range_search_destroy)
END_MSGPROC(range_search_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(range_search_cbox_proc, cbox_class_proc)
	ON_COMMAND(MSG_CHANGED, on_range_search_cbox_changed)
END_MSGPROC(range_search_cbox_proc, cbox_class_proc)

BEGIN_MSGPROC(range_search_nbox_proc, ui_comm_num_proc)
  ON_COMMAND(MSG_NUMBER, on_range_search_change_freq_symbol)
END_MSGPROC(range_search_nbox_proc, ui_comm_num_proc)


BEGIN_KEYMAP(range_search_select_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(range_search_select_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(range_search_select_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_select_msg_start_search)
END_MSGPROC(range_search_select_proc, text_class_proc)

