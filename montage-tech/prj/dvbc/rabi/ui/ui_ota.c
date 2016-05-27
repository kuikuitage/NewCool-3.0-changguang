/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_ota.h"
#include "ui_ota_search.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_OTA_FREQ,
#ifndef DTMB_PROJECT
  IDC_OTA_SYM,
  IDC_OTA_DEMOD,
#endif
  IDC_OTA_PID,
  IDC_OTA_START,
#ifdef DTMB_PROJECT
  IDC_HELP_CONT
#endif
};

static u16 ota_cont_keymap(u16 key);



static RET_CODE ota_cont_proc(control_t *cont, u16 msg, u32 para1,
                                            u32 para2);

#ifndef DTMB_PROJECT
static RET_CODE ota_cbox_proc(control_t *p_cbox, u16 msg, u32 para1,
                                            u32 para2);
#endif
static RET_CODE ota_nbox_proc(control_t *p_cbox, u16 msg, u32 para1,
                                            u32 para2);

u16 ota_select_keymap(u16 key);

RET_CODE ota_select_proc(control_t *cont, u16 msg, u32 para1, u32 para2);

static void ota_set_transpond(control_t *p_cont)
{
#ifdef DTMB_PROJECT
  control_t *p_ctrl;
  dvbs_tp_node_t tp = {0};
  sat_node_t sat_node = {0};

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_OTA_FREQ);
  tp.freq = ui_comm_numedit_get_num(p_ctrl);
  tp.sym = DTMB_HN_SYMBOL_DEFAULT;
  tp.sat_id = sat_node.id;
  tp.nim_type = NIM_DTMB;
  ui_set_antenna_and_transpond(&sat_node, &tp);
#else
  dvbs_tp_node_t tp = {0};
  control_t *p_ctrl;
  u32 freq;
  u32 sym;
  u8 demod;
  ui_scan_param_t scan_param;
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;

  memset(&scan_param, 0, sizeof(ui_scan_param_t));
  
  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_OTA_FREQ);
  freq = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_OTA_SYM);
  sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(p_cont, IDC_OTA_DEMOD);
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
#endif
}

static RET_CODE start_ota_search(control_t *cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_ctrl;
  ota_info_t upgrade;
#ifndef DTMB_PROJECT
  u8 demod;
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;
#endif

  memcpy((u8 *)&upgrade, (u8 *)sys_status_get_ota_info(), sizeof(ota_info_t));
#ifdef DTMB_PROJECT
  upgrade.sys_mode = SYS_DTMB;
#else
  upgrade.sys_mode = SYS_DVBC;
#endif
  upgrade.ota_tri = OTA_TRI_NONE;
  
  p_ctrl = ctrl_get_child_by_id(cont, IDC_OTA_FREQ);
#ifdef DTMB_PROJECT
  upgrade.lockt.tp_freq = ui_comm_numedit_get_num(p_ctrl);
#else
  upgrade.lockc.tp_freq = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_OTA_SYM);
  upgrade.lockc.tp_sym = ui_comm_numedit_get_num(p_ctrl);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_OTA_DEMOD);
  demod = (u8)ui_comm_select_get_focus(p_ctrl);
#endif

  p_ctrl = ctrl_get_child_by_id(cont, IDC_OTA_PID);
  upgrade.download_data_pid = (u16)ui_comm_numedit_get_num(p_ctrl);

#ifndef DTMB_PROJECT
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

  upgrade.lockc.nim_modulate = nim_modul;
#endif

  manage_open_menu(ROOT_ID_OTA_SEARCH, 0, (u32)&upgrade);

  return SUCCESS;
}

RET_CODE open_ota(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[OTA_ITEM_CNT];
  u8 i = 0;
#ifdef DTMB_PROJECT
  t2_search_info search_info = {0};
  u8 country_id = 0, channel_min = 0, channel_max =0;
  u32 frequency = 0;
  u16 bandwidth = 0;
  u16 stxt [OTA_ITEM_CNT] = {IDS_FREQUENCY_MHZ, IDS_DOWNLOAD_PID, IDS_START};

  comm_help_data_t help =
  {
    2,
    2,
    {
      IDS_MENU,
      IDS_EXIT,
    },
    {
      IM_HELP_MENU,
      IM_HELP_EXIT,
    },
  };
#else
  u16 stxt [OTA_ITEM_CNT] = {IDS_FREQUECY, IDS_SYMBOL, IDS_MODULATION, IDS_DOWNLOAD_PID, IDS_START};
#endif
  u16 y;
  dvbc_lock_t tp = {0};
  sys_status_get_main_tp1(&tp);

#ifdef DTMB_PROJECT
  country_id = (u8)para2;
  /* todo: need update country */
  sys_status_get_country_set(&search_info);
  /* check receive id == saved infomation? */
  if(country_id != search_info.country_style)
  {
    return ERR_FAILURE;
  }
  
  channel_min = dvbt_get_min_rf_ch_num(search_info.country_style);
  channel_max = dvbt_get_max_rf_ch_num(search_info.country_style);
  dvbt_get_tp_setting(channel_min, search_info.country_style, &bandwidth, &frequency);
#endif

  p_cont = ui_comm_root_create_with_signbar(ROOT_ID_OTA, 0,
                                  COMM_BG_X, COMM_BG_Y,
                                  COMM_BG_W, COMM_BG_H,
                                  IM_ARROW1_L, IDS_UPGRADE_BY_OTA, FALSE);

  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, ota_cont_keymap);
  ctrl_set_proc(p_cont, ota_cont_proc);

  y = OTA_ITEM_Y;
  for (i = 0; i < OTA_ITEM_CNT; i++)
  {
    switch (i)
    {
      case 0:
#ifndef DTMB_PROJECT
      case 1:
#endif
        p_ctrl[i] = ui_comm_numedit_create(p_cont, (u8)(IDC_OTA_FREQ + i),
          OTA_ITEM_X, y, OTA_ITEM_LW, OTA_ITEM_RW);
        ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], ota_nbox_proc);
        if(i == 0)
        {
#ifdef DTMB_PROJECT
          ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC, 474000, 864000, 6, 0);
          ui_comm_numedit_set_num(p_ctrl[i], frequency);
#else
          ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID,
            SEARCH_FREQ_MIN, SEARCH_FREQ_MAX, SEARCH_FREQ_BIT, (SEARCH_FREQ_BIT - 1));
          ui_comm_numedit_set_postfix(p_ctrl[i], IDS_UNIT_MHZ);
          ui_comm_numedit_set_decimal_places(p_ctrl[i], 3);
          ui_comm_numedit_set_num(p_ctrl[i], tp.tp_freq);
#endif
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
#ifndef DTMB_PROJECT
      case 2:
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_OTA_FREQ + i),
          OTA_ITEM_X, y, OTA_ITEM_LW, OTA_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], ota_cbox_proc);
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
#endif
#ifdef DTMB_PROJECT
      case 1:
#else
      case 3:
#endif
        p_ctrl[i] = ui_comm_numedit_create(p_cont, (u8)(IDC_OTA_FREQ + i),
          OTA_ITEM_X, y, OTA_ITEM_LW, OTA_ITEM_RW);
        ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], ota_nbox_proc);
        ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID, OTA_PID_MIN, OTA_PID_MAX, OTA_PID_BIT, (OTA_PID_BIT-1));
        ui_comm_numedit_set_num(p_ctrl[i], 7000);
        break;
#ifdef DTMB_PROJECT
      case 2:
#else
      case 4:
#endif
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_OTA_FREQ + i), 
                                        OTA_ITEM_X, y,
                                        OTA_ITEM_LW + OTA_ITEM_RW, OTA_ITEM_H,
                                        p_cont, 0);
        ctrl_set_keymap(p_ctrl[i], ota_select_keymap);
        ctrl_set_proc(p_ctrl[i], ota_select_proc);
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
                              OTA_ITEM_CNT) %
                             OTA_ITEM_CNT + 1),            /* up */
                        0,                                     /* right */
                        (u8)((i + 1) % OTA_ITEM_CNT + 1)); /* down */

    y += OTA_ITEM_H + OTA_ITEM_V_GAP;
  }

#ifdef DTMB_PROJECT
  {
    control_t *p_help_cont = NULL;
    p_help_cont = ctrl_create_ctrl(CTRL_CONT, IDC_HELP_CONT, OTA_ITEM_X + 20, SCREEN_HEIGHT - 100, 
                          SCREEN_WIDTH - OTA_ITEM_X * 2, OTA_ITEM_H, p_cont, 0);
    if(p_help_cont)
    {
       ui_comm_help_create(&help, p_help_cont);
    }
  }
#endif

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); 
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}

//combobox
#ifndef DTMB_PROJECT
static RET_CODE on_ota_cbox_changed(control_t *p_cbox, u16 msg, u32 para1,
                             	   u32 para2)
{
  cbox_class_proc(p_cbox, msg, para1, para2);
  ota_set_transpond(ctrl_get_parent(ctrl_get_parent(p_cbox)));

  return SUCCESS;
}
#endif

static RET_CODE on_ota_change_freq_symbol(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_comm_num_proc(p_ctrl, msg, para1, para2);
  ota_set_transpond(p_ctrl->p_parent->p_parent);
  return SUCCESS;
}

static RET_CODE on_select_ota_start(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  start_ota_search(p_ctrl->p_parent, msg, para1, para2);
  return SUCCESS;
}
static RET_CODE on_ota_destroy(control_t *cont, u16 msg, u32 para1,
                            u32 para2)
{
   return ERR_NOFEATURE;
}


BEGIN_KEYMAP(ota_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ota_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ota_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_DESTROY, on_ota_destroy)
END_MSGPROC(ota_cont_proc, ui_comm_root_proc)

#ifndef DTMB_PROJECT
BEGIN_MSGPROC(ota_cbox_proc, cbox_class_proc)
	ON_COMMAND(MSG_CHANGED, on_ota_cbox_changed)
END_MSGPROC(ota_cbox_proc, cbox_class_proc)
#endif

BEGIN_MSGPROC(ota_nbox_proc, ui_comm_num_proc)
  ON_COMMAND(MSG_NUMBER, on_ota_change_freq_symbol)
END_MSGPROC(ota_nbox_proc, ui_comm_num_proc)

BEGIN_KEYMAP(ota_select_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ota_select_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ota_select_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_select_ota_start)
END_MSGPROC(ota_select_proc, text_class_proc)


