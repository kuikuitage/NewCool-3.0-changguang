#if 0
#include "sys_types.h"
#include "ui_common.h"
#include "pti.h"
#include "service.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "pmt.h"
#include "ui_ota_user_input.h"
#include "nim.h"
#include "ap_ota.h"
#include "ui_ota_upgrade.h"
#include "ui_ota_api.h"

enum ota_user_input_ctrl_id
{
  IDC_OTA_USER_INPUT_TPFREQ_T = 1,
  IDC_OTA_USER_INPUT_TPFREQ,
  IDC_OTA_USER_INPUT_SYM_T,
  IDC_OTA_USER_INPUT_SYM,
  IDC_OTA_USER_INPUT_POL_T,
  IDC_OTA_USER_INPUT_POL,
  IDC_OTA_USER_INPUT_LNBFREQ_T,
  IDC_OTA_USER_INPUT_LNBFREQ,
  IDC_OTA_USER_INPUT_DPID_T,
  IDC_OTA_USER_INPUT_DPID, 
  IDC_OTA_USER_INPUT_DISEQ10_T,
  IDC_OTA_USER_INPUT_DISEQ10, 
  IDC_OTA_USER_INPUT_22K_T,
  IDC_OTA_USER_INPUT_22K,   
  IDC_OTA_USER_INPUT_START,
  IDC_OTA_USER_INPUT_HELP,
  IDC_OTA_USER_INPUT_TTL,
};

u16 ota_user_input_keymap(u16 key);
RET_CODE ota_user_input_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 ota_user_input_num_keymap(u16 key);

u16 ota_user_input_cbox_keymap(u16 key);
RET_CODE ota_user_input_cbox_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

u16 ota_user_input_start_keymap(u16 key);
RET_CODE ota_user_input_start_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

ota_info_t upgrade;

static u8 freq_str[LNB_FREQ_TYPE_CNT][24] = {"5150", "5750", "5950", "9750", "10000", 
  "10050", "10450", "10600", "10700", "10750", 
  "11250", "11300", "5150-5750", "5750-5150", "Universal(9750-10600)"};

s32 open_ota_user_input(u32 para1, u32 para2)
{
  struct control *p_cont, *p_title;
  struct control *p_item[OTA_USER_INPUT_ITEM_CNT];

  u8 stxt[OTA_USER_INPUT_ITEM_CNT] =
  {
    IDS_FREQUECY, 0, IDS_SYMBOL_RATE, 0, 
    IDS_POLARITY, 0, IDS_LNB_TYPE, 0,
    IDS_DOWNLOAD_PID, 0, IDS_DISEQC10, 0,
    IDS_22K, 0,
    IDS_START, 0
  };
  u16 y;
  u8 i, j;

  p_cont = fw_create_mainwin(ROOT_ID_OTA_USER_INPUT, 
    OTA_USER_INPUT_CONTX, OTA_USER_INPUT_CONTY, 
    OTA_USER_INPUT_CONTW, OTA_USER_INPUT_CONTH, 
    0, 0, OBJ_ATTR_ACTIVE, 0);

  MT_ASSERT(p_cont != NULL);

  ctrl_set_rstyle(p_cont, RSI_COMM_TXT_N, RSI_COMM_TXT_N, RSI_COMM_TXT_N);
  ctrl_set_proc(p_cont, ota_user_input_proc);
  ctrl_set_keymap(p_cont, ota_user_input_keymap);  

  y = OTA_USER_INPUT_ITEMY;
  
  for(i = 0; i < OTA_USER_INPUT_ITEM_CNT; i++)
  {
    switch(i + 1)
    {
      case IDC_OTA_USER_INPUT_TPFREQ_T:
      case IDC_OTA_USER_INPUT_SYM_T:
      case IDC_OTA_USER_INPUT_LNBFREQ_T:
      case IDC_OTA_USER_INPUT_DPID_T:
      case IDC_OTA_USER_INPUT_POL_T:
      case IDC_OTA_USER_INPUT_DISEQ10_T:
      case IDC_OTA_USER_INPUT_22K_T:
        p_item[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_OTA_USER_INPUT_TPFREQ_T + i),
          OTA_USER_INPUT_ITEMLX, y, 
          OTA_USER_INPUT_ITEMRW, OTA_USER_INPUT_ITEMH,
          p_cont, 0);
        ctrl_set_rstyle(p_item[i], RSI_COMM_TXT_N, RSI_COMM_TXT_HL, RSI_COMM_TXT_N);
        text_set_font_style(p_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
        text_set_align_type(p_item[i], STL_LEFT | STL_VCENTER);
        text_set_content_type(p_item[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_item[i], stxt[i]);
        break;
        
      case IDC_OTA_USER_INPUT_TPFREQ:
      case IDC_OTA_USER_INPUT_SYM:
      case IDC_OTA_USER_INPUT_DPID:
        p_item[i] = ctrl_create_ctrl(CTRL_NBOX, (u8)(IDC_OTA_USER_INPUT_TPFREQ_T + i),
          OTA_USER_INPUT_ITEMRX, y, 
          OTA_USER_INPUT_ITEMRW, OTA_USER_INPUT_ITEMH,
          p_cont, 0);
        ctrl_set_keymap(p_item[i], ota_user_input_num_keymap);
        ctrl_set_rstyle(p_item[i], RSI_COMM_TXT_N, RSI_COMM_TXT_HL, RSI_COMM_TXT_N);
        nbox_set_font_style(p_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
        nbox_set_align_type(p_item[i], STL_CENTER | STL_VCENTER);
        nbox_set_num_type(p_item[i], NBOX_NUMTYPE_DEC);
        switch(i + 1)
        {
          case IDC_OTA_USER_INPUT_TPFREQ:
            nbox_set_range(p_item[i], OTA_TPFREQ_MIN, OTA_TPFREQ_MAX, OTA_TPFREQ_BIT);
            nbox_set_focus(p_item[i], (OTA_TPFREQ_BIT - 1));
            nbox_set_postfix_type(p_item[i], NBOX_ITEM_PREFIX_TYPE_STRID);
            nbox_set_postfix_by_strid(p_item[i], IDS_UNIT_MHZ);
            nbox_set_num_by_dec(p_item[i], 4100);
            break;
          case IDC_OTA_USER_INPUT_SYM:
            nbox_set_range(p_item[i], OTA_SYM_MIN, OTA_SYM_MAX, OTA_SYM_BIT);
            nbox_set_focus(p_item[i], (OTA_SYM_BIT - 1));
            nbox_set_postfix_type(p_item[i], NBOX_ITEM_PREFIX_TYPE_STRID);
            nbox_set_postfix_by_strid(p_item[i], IDS_UNIT_KSS);
            nbox_set_num_by_dec(p_item[i], 30000);
            break;
          case IDC_OTA_USER_INPUT_DPID:
            nbox_set_range(p_item[i], OTA_DPID_MIN, OTA_DPID_MAX, OTA_DPID_BIT);
            nbox_set_focus(p_item[i], (OTA_DPID_BIT - 1));
            nbox_set_num_by_dec(p_item[i], 8160);//0x1FE0            
            break;
          default:
            MT_ASSERT(0);
            break;
        }

        y += (OTA_USER_INPUT_ITEMH + OTA_USER_INPUT_ITEM_VGAP);
        break;

      case IDC_OTA_USER_INPUT_POL:
        p_item[i] = ctrl_create_ctrl(CTRL_CBOX, (u8)(IDC_OTA_USER_INPUT_TPFREQ_T + i),
          OTA_USER_INPUT_ITEMRX, y, 
          OTA_USER_INPUT_ITEMRW, OTA_USER_INPUT_ITEMH,
          p_cont, 0);
        ctrl_set_rstyle(p_item[i], RSI_COMM_TXT_N, RSI_COMM_TXT_HL, RSI_COMM_TXT_N);
        ctrl_set_keymap(p_item[i], ota_user_input_cbox_keymap);
        ctrl_set_proc(p_item[i], ota_user_input_cbox_proc);
        cbox_enable_cycle_mode(p_item[i], TRUE);
        cbox_set_work_mode(p_item[i], CBOX_WORKMODE_STATIC);
        cbox_set_align_style(p_item[i], STL_CENTER | STL_VCENTER);
        cbox_set_font_style(p_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
        cbox_static_set_count(p_item[i], 2);
        cbox_static_set_content_type(p_item[i], CBOX_ITEM_STRTYPE_STRID);
        cbox_static_set_content_by_strid(p_item[i], 0, IDS_HORI);
        cbox_static_set_content_by_strid(p_item[i], 1, IDS_VERT);
        cbox_static_set_focus(p_item[i], 0);     
        y += (OTA_USER_INPUT_ITEMH + OTA_USER_INPUT_ITEM_VGAP);
        break;

      case IDC_OTA_USER_INPUT_22K:
        p_item[i] = ctrl_create_ctrl(CTRL_CBOX, (u8)(IDC_OTA_USER_INPUT_TPFREQ_T + i),
          OTA_USER_INPUT_ITEMRX, y, 
          OTA_USER_INPUT_ITEMRW, OTA_USER_INPUT_ITEMH,
          p_cont, 0);
        ctrl_set_rstyle(p_item[i], RSI_COMM_TXT_N, RSI_COMM_TXT_HL, RSI_COMM_TXT_N);
        ctrl_set_keymap(p_item[i], ota_user_input_cbox_keymap);
        ctrl_set_proc(p_item[i], ota_user_input_cbox_proc);
        cbox_enable_cycle_mode(p_item[i], TRUE);
        cbox_set_work_mode(p_item[i], CBOX_WORKMODE_STATIC);
        cbox_set_align_style(p_item[i], STL_CENTER | STL_VCENTER);
        cbox_set_font_style(p_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
        cbox_static_set_count(p_item[i], 2);
        cbox_static_set_content_type(p_item[i], CBOX_ITEM_STRTYPE_STRID);
        cbox_static_set_content_by_strid(p_item[i], 0, IDS_OFF);
        cbox_static_set_content_by_strid(p_item[i], 1, IDS_ON);
        cbox_static_set_focus(p_item[i], 0);     
        y += (OTA_USER_INPUT_ITEMH + OTA_USER_INPUT_ITEM_VGAP);
        break;

      case IDC_OTA_USER_INPUT_DISEQ10:
        p_item[i] = ctrl_create_ctrl(CTRL_CBOX, (u8)(IDC_OTA_USER_INPUT_TPFREQ_T + i),
          OTA_USER_INPUT_ITEMRX, y, 
          OTA_USER_INPUT_ITEMRW, OTA_USER_INPUT_ITEMH,
          p_cont, 0);
        ctrl_set_rstyle(p_item[i], RSI_COMM_TXT_N, RSI_COMM_TXT_HL, RSI_COMM_TXT_N);
        ctrl_set_keymap(p_item[i], ota_user_input_cbox_keymap);
        ctrl_set_proc(p_item[i], ota_user_input_cbox_proc);
        cbox_enable_cycle_mode(p_item[i], TRUE);
        cbox_set_work_mode(p_item[i], CBOX_WORKMODE_STATIC);
        cbox_set_align_style(p_item[i], STL_CENTER | STL_VCENTER);
        cbox_set_font_style(p_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
        cbox_static_set_count(p_item[i], 5);
        cbox_static_set_content_type(p_item[i], CBOX_ITEM_STRTYPE_STRID);
        cbox_static_set_content_by_strid(p_item[i], 0, IDS_OFF);
        cbox_static_set_content_by_strid(p_item[i], 1, IDS_PORT1);
        cbox_static_set_content_by_strid(p_item[i], 2, IDS_PORT2);
        cbox_static_set_content_by_strid(p_item[i], 3, IDS_PORT3);
        cbox_static_set_content_by_strid(p_item[i], 4, IDS_PORT4);        
        cbox_static_set_focus(p_item[i], 0);     
        y += (OTA_USER_INPUT_ITEMH + OTA_USER_INPUT_ITEM_VGAP);
        break;
        
      case IDC_OTA_USER_INPUT_LNBFREQ:
        p_item[i] = ctrl_create_ctrl(CTRL_CBOX, (u8)(IDC_OTA_USER_INPUT_TPFREQ_T + i),
          OTA_USER_INPUT_ITEMRX, y, 
          OTA_USER_INPUT_ITEMRW, OTA_USER_INPUT_ITEMH,
          p_cont, 0);
        ctrl_set_rstyle(p_item[i], RSI_COMM_TXT_N, RSI_COMM_TXT_HL, RSI_COMM_TXT_N);
        ctrl_set_keymap(p_item[i], ota_user_input_cbox_keymap);
        ctrl_set_proc(p_item[i], ota_user_input_cbox_proc);
        cbox_enable_cycle_mode(p_item[i], TRUE);
        cbox_set_work_mode(p_item[i], CBOX_WORKMODE_STATIC);
        cbox_set_align_style(p_item[i], STL_CENTER | STL_VCENTER);
        cbox_set_font_style(p_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
        cbox_static_set_count(p_item[i], LNB_FREQ_TYPE_CNT);
        cbox_static_set_content_type(p_item[i], CBOX_ITEM_STRTYPE_UNICODE);
        for(j = 0; j < LNB_FREQ_TYPE_CNT; j++)
        {
          cbox_static_set_content_by_ascstr(p_item[i], j, freq_str[j]);
        }

        cbox_static_set_focus(p_item[i], 0);     
        y += (OTA_USER_INPUT_ITEMH + OTA_USER_INPUT_ITEM_VGAP);
        break;
        
      case IDC_OTA_USER_INPUT_START:
        p_item[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_OTA_USER_INPUT_TPFREQ_T + i),
          OTA_USER_INPUT_ITEMLX, y, 
          (OTA_USER_INPUT_ITEMRW + OTA_USER_INPUT_ITEMLW), 
          OTA_USER_INPUT_ITEMH, p_cont, 0);
        ctrl_set_rstyle(p_item[i], RSI_COMM_TXT_N, RSI_COMM_TXT_HL, RSI_COMM_TXT_N);
        text_set_font_style(p_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
        text_set_align_type(p_item[i], STL_LEFT | STL_VCENTER);
        text_set_content_type(p_item[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_item[i], stxt[i]);
        ctrl_set_keymap(p_item[i], ota_user_input_start_keymap);
        ctrl_set_proc(p_item[i], ota_user_input_start_proc);     
        y += (OTA_USER_INPUT_ITEMH + 2*OTA_USER_INPUT_ITEM_VGAP);        
        break;
      case IDC_OTA_USER_INPUT_HELP:
        p_item[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_OTA_USER_INPUT_TPFREQ_T + i),
          OTA_USER_INPUT_ITEMLX, y, 
          (OTA_USER_INPUT_ITEMRW + OTA_USER_INPUT_ITEMLW), 
          OTA_USER_INPUT_ITEMH, p_cont, 0);
        ctrl_set_rstyle(p_item[i], RSI_COMM_TXT_N, RSI_COMM_TXT_HL, RSI_COMM_TXT_N);
        text_set_font_style(p_item[i], FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
        text_set_align_type(p_item[i], STL_LEFT | STL_VCENTER);
        text_set_content_type(p_item[i], TEXT_STRTYPE_STRID);
        text_set_content_by_strid(p_item[i], IDS_PRESS_EXIT_TO_EXIT_UPGRADE_AND_RESTART);
        y += (OTA_USER_INPUT_ITEMH + OTA_USER_INPUT_ITEM_VGAP);        
        break;
      default:
        MT_ASSERT(0);
        break;
    }
  }
  ctrl_set_related_id(p_item[1],  0, 15, 0, 4);//tp freq
  ctrl_set_related_id(p_item[3],  0,  2, 0, 6);//sym
  ctrl_set_related_id(p_item[5],  0,  4, 0, 8);//pol
  ctrl_set_related_id(p_item[7],  0,  6, 0, 10);//lnb freq
  ctrl_set_related_id(p_item[9],  0,  8, 0, 12);//dpid
  ctrl_set_related_id(p_item[11], 0, 10, 0, 14);//dpid
  ctrl_set_related_id(p_item[13], 0, 12, 0, 15);//dpid
  ctrl_set_related_id(p_item[14], 0, 14, 0, 2);//dpid

  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_OTA_USER_INPUT_TTL,
    OTA_USER_INPUT_TTLX, OTA_USER_INPUT_TTLY, 
    OTA_USER_INPUT_TTLW, OTA_USER_INPUT_TTLH,
    p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  text_set_font_style(p_title, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);  
  text_set_content_by_strid(p_title, IDS_OTA_USER_INPUT);

  ctrl_default_proc(p_item[1], MSG_GETFOCUS, 0, 0);//focus on tp freq
  
  ctrl_paint_ctrl(p_cont, FALSE);
  return SUCCESS;
}

static RET_CODE on_ota_user_input_exit(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  cmd_t cmd;

  //OS_PRINTF("exit key pressed\n");
  cmd.id = OTA_CMD_STOP;
  cmd.data1 = 0;
  cmd.data2 = 0;
  
  ap_frm_do_command(APP_OTA, &cmd);   	
  return SUCCESS;
}

static RET_CODE on_ota_user_input_cbox_changed(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  cbox_class_proc(p_ctrl, msg, 0, 0);
	return SUCCESS;
}

static RET_CODE on_ota_user_input_start(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  struct control *p_cont;
  struct control *p_tpfreq, *p_lnbfreq, *p_symb, *p_lnbtype;
  struct control *p_dpid, *p_pola, *p_22k, *p_disc10;
  u32 lnb_type, disc10;
  u16 standard_val[12] = {5150, 5750, 5950, 9750, 10000, 10050,
    10450, 10600, 10700, 10750, 11250, 11300};
  u16 user_val[2][2] = {{5150, 5750}, {5750, 5150}};
  u16 univer_val[2] = {9750, 10600};
  
  p_cont = ctrl_get_parent(p_ctrl);
  p_tpfreq = ctrl_get_child_by_id(p_cont, IDC_OTA_USER_INPUT_TPFREQ);
  p_lnbfreq = ctrl_get_child_by_id(p_cont, IDC_OTA_USER_INPUT_LNBFREQ);
  p_symb = ctrl_get_child_by_id(p_cont, IDC_OTA_USER_INPUT_SYM);
  p_dpid = ctrl_get_child_by_id(p_cont, IDC_OTA_USER_INPUT_DPID);
  p_pola = ctrl_get_child_by_id(p_cont, IDC_OTA_USER_INPUT_POL);
  p_lnbtype = ctrl_get_child_by_id(p_cont, IDC_OTA_USER_INPUT_LNBFREQ);
  p_disc10 = ctrl_get_child_by_id(p_cont, IDC_OTA_USER_INPUT_DISEQ10);
  p_22k = ctrl_get_child_by_id(p_cont, IDC_OTA_USER_INPUT_22K);
  
  //to be continued.
  upgrade.sys_mode = SYS_DVBS;
  
  upgrade.locks.tp_rcv.polarity = cbox_static_get_focus(p_pola);
  upgrade.locks.tp_rcv.sym = nbox_get_num(p_symb);
  upgrade.locks.tp_rcv.freq = nbox_get_num(p_tpfreq);
  upgrade.locks.tp_rcv.is_on22k = cbox_static_get_focus(p_22k);
  OS_PRINTF("lou 1111111  upgrade.locks.tp_rcv.is_on22k ==%d\n",upgrade.locks.tp_rcv.is_on22k );

  upgrade.locks.sat_rcv.k22 = cbox_static_get_focus(p_22k);

  lnb_type = cbox_static_get_focus(p_lnbtype);
  if(lnb_type < 12)
  {
    upgrade.locks.sat_rcv.lnb_type = 0;
    upgrade.locks.sat_rcv.lnb_low = standard_val[lnb_type];
    upgrade.locks.sat_rcv.lnb_high = standard_val[lnb_type];
  }
  else if(lnb_type < 14)
  {
    upgrade.locks.sat_rcv.lnb_type = 1;
    upgrade.locks.sat_rcv.lnb_low = user_val[lnb_type - 12][0];
    upgrade.locks.sat_rcv.lnb_high = user_val[lnb_type - 12][1];  
  }
  else
  {
    upgrade.locks.sat_rcv.lnb_type = 2;
    upgrade.locks.sat_rcv.lnb_low = univer_val[0];
    upgrade.locks.sat_rcv.lnb_high = univer_val[1];  
  }

  upgrade.locks.sat_rcv.v12 = 0;
  upgrade.locks.sat_rcv.lnb_power = 0;

  disc10 = cbox_static_get_focus(p_disc10);
  OS_PRINTF("lou 2222  disc10 ==%d\n",disc10);
  if(disc10)
  {
    upgrade.locks.disepc_rcv.diseqc_port_1_0 = disc10-1;
    upgrade.locks.disepc_rcv.diseqc_type_1_0 = 1;
  }
  else
  {
    upgrade.locks.disepc_rcv.diseqc_port_1_0 = 0;
    upgrade.locks.disepc_rcv.diseqc_type_1_0 = 0;
  }
  upgrade.locks.disepc_rcv.diseqc_1_1_mode = 0;
  upgrade.locks.disepc_rcv.diseqc_port_1_1 = 0;
  upgrade.locks.disepc_rcv.diseqc_type_1_1 = 0;
  upgrade.locks.disepc_rcv.d_local_latitude = 0;
  upgrade.locks.disepc_rcv.d_local_longitude = 0;
  upgrade.locks.disepc_rcv.d_sat_longitude = 0;
  upgrade.locks.disepc_rcv.is_fixed = TRUE;
  upgrade.locks.disepc_rcv.position = 0;
  upgrade.locks.disepc_rcv.position_type = 0;
  upgrade.locks.disepc_rcv.used_DiSEqC12 = FALSE;

  upgrade.download_data_pid = nbox_get_num(p_dpid);

  ui_ota_relock(&upgrade);

  if(fw_find_root_by_id(ROOT_ID_OTA_UPGRADE) == NULL)
  {
	  manage_open_menu(ROOT_ID_OTA_UPGRADE, 0, 0);
  }  

	return SUCCESS;
}

BEGIN_KEYMAP(ota_user_input_keymap, NULL)
	ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ota_user_input_keymap, NULL)

BEGIN_MSGPROC(ota_user_input_proc, cont_class_proc)
  ON_COMMAND(MSG_EXIT, on_ota_user_input_exit)    
END_MSGPROC(ota_user_input_proc, cont_class_proc);

BEGIN_KEYMAP(ota_user_input_cbox_keymap, NULL)
	ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
	ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
END_KEYMAP(ota_user_input_cbox_keymap, NULL)

BEGIN_MSGPROC(ota_user_input_cbox_proc, cbox_class_proc)
  ON_COMMAND(MSG_DECREASE, on_ota_user_input_cbox_changed)
  ON_COMMAND(MSG_INCREASE, on_ota_user_input_cbox_changed)
END_MSGPROC(ota_user_input_cbox_proc, cbox_class_proc);

BEGIN_KEYMAP(ota_user_input_start_keymap, NULL)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(ota_user_input_start_keymap, NULL)

BEGIN_MSGPROC(ota_user_input_start_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_ota_user_input_start)
END_MSGPROC(ota_user_input_start_proc, text_class_proc);

BEGIN_KEYMAP(ota_user_input_num_keymap, NULL)
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

  ON_EVENT(V_KEY_UP, MSG_INCREASE)
  ON_EVENT(V_KEY_DOWN, MSG_DECREASE)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)

  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_UNSELECT)
  ON_EVENT(V_KEY_MENU, MSG_UNSELECT)
END_KEYMAP(ota_user_input_num_keymap, NULL)	
#endif
