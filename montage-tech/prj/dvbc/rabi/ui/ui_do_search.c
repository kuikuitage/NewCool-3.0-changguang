/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_set_search.h"
#include "ui_do_search.h"
#ifdef ENABLE_CA
#include "config_cas.h"
#include "ui_ca_public.h"
#endif

static sat_node_t curn_sat_node;

enum local_msg
{
  MSG_STOP_SCAN = MSG_LOCAL_BEGIN + 150,
  MSG_CLOSE_ALL,
  MSG_ANIMATION,
};

enum control_id
{
  IDC_PROG_INFO_FRM = 1,
  IDC_DO_SEARCH_TOP_LINE,
  IDC_DO_SEARCH_BOTTOM_LINE,
  IDC_TP_INFO_FRM,
  IDC_PBAR,
};

enum prog_sub_control_id
{
  IDC_TITLE_TV = 1,
  IDC_TITLE_RADIO,
  IDC_NUM,
  IDC_LIST_TV,
  IDC_LIST_RADIO,
};

enum tp_sub_control_id
{
  IDC_TP_LIST = 1,
  IDC_ANIMATION,
};

#define PROG_NAME_STRLEN    DB_DVBS_MAX_NAME_LENGTH

static u16 prog_name_str[DO_SEARCH_LIST_CNT][DO_SEARCH_LIST_PAGE][
  PROG_NAME_STRLEN + 1];
static u16 *prog_name_str_addr[DO_SEARCH_LIST_CNT][DO_SEARCH_LIST_PAGE];
static u16 prog_curn[DO_SEARCH_LIST_CNT];

#define TP_NO_STRLEN      4 /* 999 */
#define TP_INFO_STRLEN    64

static u16 tp_no_str[DO_SEARCH_TP_LIST_PAGE][TP_NO_STRLEN + 1];
static u16 tp_sat_str[DO_SEARCH_TP_LIST_PAGE][DB_DVBS_MAX_NAME_LENGTH + 1];
static u16 tp_info_str[DO_SEARCH_TP_LIST_PAGE][TP_INFO_STRLEN + 1];
static u16 *tp_no_str_addr[DO_SEARCH_TP_LIST_PAGE];
static u16 *tp_sat_str_addr[DO_SEARCH_TP_LIST_PAGE];
static u16 *tp_info_str_addr[DO_SEARCH_TP_LIST_PAGE];
static u16 tp_curn;

static BOOL is_db_empty = FALSE;
static BOOL is_stop = FALSE;
static BOOL is_ask_for_cancel = FALSE;
static BOOL is_finished = TRUE;
static sat_node_t curn_sat_node;

static u16 animation_icon[DO_SEARCH_ANIMATION_CNT] =
{
  IM_SATELLITE_LINE_1, IM_SATELLITE_LINE_2,
  IM_SATELLITE_LINE_3, IM_SATELLITE_LINE_4
};
static u16 animation_focus = 0;

u16 do_search_cont_keymap(u16 key);
RET_CODE do_search_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
extern void categories_mem_reset(void);
extern BOOL have_logic_number();


#ifdef SEARCH_SUPPORT_FILTER
static  u32 filter_type = RSN_SEARCH_FILTER_DEFAULT;

u32 ui_do_search_get_search_filter_type(void)
{
	DEBUG(MAIN,INFO,"filter_type = %d \n",filter_type);
	return filter_type;
}

void ui_do_search_set_search_filter_type(u32 type)
{
	filter_type = type;
	DEBUG(MAIN,INFO,"filter_type = %d \n",filter_type);
}

void ui_do_search_clear_search_filter_type(void)
{
	filter_type = RSN_SEARCH_FILTER_DEFAULT;
	DEBUG(MAIN,INFO,"filter_type = %d \n",filter_type);
}
#endif



static void do_search_reset_pg_and_bouquet(void)
{
  BOOL is_scart = FALSE;
  u8 view_id;
  
  //set default use common buffer as ext buffer
  db_dvbs_restore_to_factory(PRESET_BLOCK_ID, BLOCK_REC_BUFFER);
  sys_status_get_status(BS_IS_SCART_OUT, &is_scart);
  sys_status_set_categories_count(0);
  categories_mem_reset();
  if(is_scart)
 {
 	sys_status_set_status(BS_IS_SCART_OUT, TRUE);
 }
  sys_status_check_group();
  sys_status_save();

  view_id = ui_dbase_create_view(DB_DVBS_ALL_TV, 0, NULL);
  ui_dbase_set_pg_view_id(view_id);

  //clear history 
  ui_clear_play_history();

  //delete all book imformation
  book_delete_all_node();
  #ifdef SHUSHITONG_CA
  if(CUSTOMER_ID == CUSTOMER_DTMB_CHANGSHA_HHT)
  {
    set_have_valid_freq_flag(FALSE);
  }
  #endif
 }
 
static void do_cancel(void)
{
  if(!is_stop)
  {
    UI_PRINTF("DO_SEARCH: cancel scan!\n");

    OS_PRINTF("*************************press ok : %d\n", mtos_ticks_get());
    is_stop = TRUE;
    ui_stop_scan();
  }
}


static void undo_cancel(void)
{
  UI_PRINTF("DO_SEARCH: resume scan!\n");
  ui_resume_scan();
}


static void init_static_data(void)
{
  u16 i, j, *p_str;

  for(i = 0; i < DO_SEARCH_LIST_CNT; i++)
  {
    for(j = 0; j < DO_SEARCH_LIST_PAGE; j++)
    {
      p_str = prog_name_str[i][j];
      prog_name_str_addr[i][j] = p_str;
      p_str[0] = '\0';
    }
  }
  memset(prog_curn, 0, sizeof(prog_curn));

  for(i = 0; i < DO_SEARCH_TP_LIST_PAGE; i++)
  {
    p_str = tp_no_str[i];
    tp_no_str_addr[i] = p_str;
    p_str[0] = '\0';

    p_str = tp_sat_str[i];
    tp_sat_str_addr[i] = p_str;
    p_str[0] = '\0';

    p_str = tp_info_str[i];
    tp_info_str_addr[i] = p_str;
    p_str[0] = '\0';
  }
  tp_curn = 0;
}


static void init_prog_list_content(control_t *list)
{
  u8 i, list_idx = ctrl_get_ctrl_id(list) == IDC_LIST_TV ? 0 : 1;

  for(i = 0; i < DO_SEARCH_LIST_PAGE; i++)
  {
    list_set_field_content_by_extstr(list, i, 0,
                                     prog_name_str_addr[list_idx][i]);
  }
}


static void init_tp_list_content(control_t *list)
{
  u8 i;
  if(CUSTOMER_ID == CUSTOMER_DTMB_DESAI_JIMO)
  {
	return;
  }
  for(i = 0; i < DO_SEARCH_TP_LIST_PAGE; i++)
  {
    list_set_field_content_by_extstr(list, i, 0, tp_no_str_addr[i]);
    list_set_field_content_by_extstr(list, i, 1, tp_sat_str_addr[i]);
    list_set_field_content_by_extstr(list, i, 2, tp_info_str_addr[i]);
  }
}


static void add_name_to_data(u8 list, u16 *p_str)
{
  u16 i, curn = prog_curn[list] % DO_SEARCH_LIST_PAGE;

  prog_name_str[list][curn][0] = '\0';
  uni_strcat(prog_name_str[list][curn], p_str, PROG_NAME_STRLEN);
  prog_name_str_addr[list][curn] = prog_name_str[list][curn];

  if(prog_curn[list] >= DO_SEARCH_LIST_PAGE)
  {
    for(i = 0; i < DO_SEARCH_LIST_PAGE; i++)
    {
      prog_name_str_addr[list][i] =
        prog_name_str[list][(prog_curn[list] + i + 1) % DO_SEARCH_LIST_PAGE];
    }
  }

  prog_curn[list]++;
}


static void add_info_to_data(u16 *p_str)
{
  u8 ascstr[TP_NO_STRLEN + 1];
  u16 i, curn = tp_curn % DO_SEARCH_TP_LIST_PAGE;

  sprintf(ascstr, "%.4d", tp_curn + 1);
  str_asc2uni(ascstr, tp_no_str[curn]);
  tp_no_str_addr[curn] = tp_no_str[curn];

  tp_sat_str[curn][0] = '\0';
  uni_strcat(tp_sat_str[curn], curn_sat_node.name, DB_DVBS_MAX_NAME_LENGTH);
  tp_sat_str_addr[curn] = tp_sat_str[curn];

  tp_info_str[curn][0] = '\0';
  uni_strcat(tp_info_str[curn], p_str, TP_INFO_STRLEN);
  tp_info_str_addr[curn] = tp_info_str[curn];

  if(tp_curn >= DO_SEARCH_TP_LIST_PAGE)
  {
    for(i = 0; i < DO_SEARCH_TP_LIST_PAGE; i++)
    {
      tp_no_str_addr[i] =
        tp_no_str[(tp_curn + i + 1) % DO_SEARCH_TP_LIST_PAGE];
      tp_sat_str_addr[i] =
        tp_sat_str[(tp_curn + i + 1) % DO_SEARCH_TP_LIST_PAGE];
      tp_info_str_addr[i] =
        tp_info_str[(tp_curn + i + 1) % DO_SEARCH_TP_LIST_PAGE];
    }
  }

  tp_curn++;
}


static void add_prog_to_list(control_t *cont, dvbs_prog_node_t *p_pg_info)
{
  control_t *frm, *title, *icon, *list;
  dvbs_prog_node_t pg;
  u8 idx;
  u8 icon_num[10];
  u16 content[PROG_NAME_STRLEN + 1];

  UI_PRINTF("ADD PG -> name[%s]\n", p_pg_info->name);

/*
   if (db_dvbs_get_pg_by_id(rid, &pg) != DB_DVBS_OK)
   {
    UI_PRINTF("GET PG failed\n", rid);
    return;
   }
  */
  memcpy(&pg, p_pg_info, sizeof(dvbs_prog_node_t));

  idx = pg.video_pid != 0 ? 0 /* tv */ : 1 /* radio */;

  ui_dbase_get_full_prog_name(&pg, content, PROG_NAME_STRLEN);
  add_name_to_data(idx, content);

  frm = ctrl_get_child_by_id(cont, IDC_PROG_INFO_FRM);
  title = ctrl_get_child_by_id(frm, IDC_TITLE_TV + idx);
  icon = ctrl_get_child_by_id(title, IDC_NUM);
  sprintf(icon_num, "[%d]", prog_curn[idx]);
  text_set_content_by_ascstr(icon, icon_num);
  ctrl_paint_ctrl(icon, TRUE);

  list = ctrl_get_child_by_id(frm, IDC_LIST_TV + idx);
  init_prog_list_content(list);
  ctrl_paint_ctrl(list, TRUE);
  UI_PRINTF("END ADD PG -> name[%s]\n", p_pg_info->name);
}

static void add_tp_to_list(control_t *cont, dvbs_tp_node_t *p_tp_info)
{
  control_t *frm, *list;
  dvbs_tp_node_t tp;
  u16 str_id;
  u8 asc_buf[TP_INFO_STRLEN + 1];
  u16 uni_buf[TP_INFO_STRLEN + 1];
  u8 ascstr[32];
  u16 unistr[64];
  u8 scan_type = ui_scan_param_get_type();

  UI_PRINTF("ADD TP -> freq[%d]\n", p_tp_info->freq);
  memcpy(&tp, p_tp_info, sizeof(dvbs_tp_node_t));

  asc_buf[0] = '\0';
  uni_buf[0] = '\0';

  //--ui_dbase_name2str(curn_sat_node.name, uni_buf, TP_INFO_STRLEN);
  //uni_strncpy(uni_buf, curn_sat_node.name, TP_INFO_STRLEN);
  //len = (u16)uni_strlen(uni_buf);
  if(scan_type == USC_DTMB_SCAN)
  {
    sprintf(asc_buf, " %.5d %.5d...", (int)tp.freq, (int)tp.sym);
  }
  else
  {
    switch(tp.nim_modulate)
    {
      case 0:
        str_id = IDS_AUTO;
        break;
  
      case 1:
        str_id =IDS_QPSK;
        break;
  
      case 2:
        str_id = IDS_BPSK;
        break;
  
      case 3:
        str_id = IDS_8PSK;
        break;
  
      case 4:
        str_id = IDS_QAM16;
        break;
  
      case 5:
        str_id = IDS_QAM32;
        break;
  
      case 6:
        str_id = IDS_QAM64;
        break;
  
      case 7:
        str_id = IDS_QAM128;
        break;
  
      case 8:
        str_id = IDS_QAM256;
        break;
      default:
        str_id = IDS_QAM64;
        break;
    }
    gui_get_string(str_id, unistr, 64);
    str_uni2asc((u8 *)ascstr,(u16 *)unistr);
    sprintf(asc_buf, "%.6d %s %.4d...", (int)tp.freq,
           (char *)ascstr, (int)tp.sym);
  }
  str_nasc2uni(asc_buf, uni_buf, TP_INFO_STRLEN);

  add_info_to_data(uni_buf);

  frm = ctrl_get_child_by_id(cont, IDC_TP_INFO_FRM);
  list = ctrl_get_child_by_id(frm, IDC_TP_LIST);
  init_tp_list_content(list);
  ctrl_paint_ctrl(list, TRUE);
  UI_PRINTF("END ADD TP ->freq[%d]\n", p_tp_info->freq);
}


static void update_progress(control_t *cont, u16 progress)
{
  control_t *bar = ctrl_get_child_by_id(cont, IDC_PBAR);

  if(ctrl_get_sts(bar) != OBJ_STS_HIDE)
  {
    UI_PRINTF("UPDATE PROGRESS -> %d\n", progress);
    ui_comm_bar_update(bar, progress, TRUE);
    ui_comm_bar_paint(bar, TRUE);
  }
}

static void set_animation_icon(control_t *cont)
{
  control_t *frm, *icon;

  frm = ctrl_get_child_by_id(cont, IDC_TP_INFO_FRM);
  icon = ctrl_get_child_by_id(frm, IDC_ANIMATION);

  animation_focus = (animation_focus + 1) % DO_SEARCH_ANIMATION_CNT;
  bmap_set_content_by_id(icon, animation_icon[animation_focus]);
  ctrl_paint_ctrl(icon, TRUE);
}

static void get_finish_str(u16 *str, u16 max_len)
{
  u16 uni_str[10], len;

  if(prog_curn[0] > 0
    || prog_curn[1] > 0)
  {
    len = 0, str[0] = '\0';
    gui_get_string(IDS_MSG_SEARCH_IS_END, str, max_len);

    convert_i_to_dec_str(uni_str, prog_curn[0]);
    uni_strcat(str, uni_str, max_len);

    len = (u16)uni_strlen(str);
    gui_get_string(IDS_MSG_N_TV, &str[len], (u16)(max_len - len));

    convert_i_to_dec_str(uni_str, prog_curn[1]);
    uni_strcat(str, uni_str, max_len);

    len = (u16)uni_strlen(str);
    gui_get_string(IDS_MSG_N_RADIO, &str[len], (u16)(max_len - len));
  }
  else
  {
    gui_get_string(IDS_MSG_NO_PROG_FOUND, str, max_len);
  }
}


static void process_finish(void)
{
  comm_dlg_data_t dlg_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_CONFIRM | DLG_STR_MODE_EXTSTR,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W, COMM_DLG_H,
    0,
    0,
  };

  u16 content[64 + 1];

  get_finish_str(content, 64);
  dlg_data.content = (u32)content;

  is_stop = TRUE;

  // already popup a dlg for ask, force close it
  if(is_ask_for_cancel)
  {
    // resume scan
    ui_resume_scan();
  }

  switch(CUSTOMER_ID)
  {
    case CUSTOMER_DTMB_DESAI_JIMO:
	case CUSTOMER_XINNEW_GAIZHOU:
		break;
	default:
		ui_comm_dlg_open(&dlg_data);
		break;
		
  }	
}


static void do_search_pre_open(void)
{
  OS_PRINTF("do scan pre open\n");
  ui_set_book_flag(FALSE);
  OS_PRINTF("set book flag\n");

#if ENABLE_TTX_SUBTITLE
  ui_enable_subt(FALSE);
#endif

  ui_enable_playback(FALSE);

  ui_time_release();

  ui_set_front_panel_by_str("----");
  OS_PRINTF("set front panel\n");
//  video_layer_display(FALSE);

  ui_epg_stop();
  ui_epg_release();
  /* stop epg */
  init_static_data();
  OS_PRINTF("init static data\n");

  /* set callback func */
//  dvbs_install_notification(scan_do_notify, 0);

  /* set flag */
  is_stop = is_finished = FALSE;

  if(sys_status_get_curn_group_curn_prog_id() == INVALIDID)
  {
    is_db_empty = TRUE;
  }
  else
  {
    is_db_empty = FALSE;
  }

//  ui_simon_stop();
//  ui_simon_release();

  ui_init_scan();
  OS_PRINTF("init scan\n");

  // animation
  animation_focus = 0;
  fw_tmr_create(ROOT_ID_DO_SEARCH, MSG_ANIMATION, 1 * SECOND, TRUE);
  OS_PRINTF("create timer for animation\n");
}


static void do_search_post_close(void)
{
  comm_dlg_data_t saving_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W, COMM_DLG_H,
    IDS_MSG_SAVING,
    0,
  };

  if((prog_curn[0] > 0)
    || (prog_curn[1] > 0))
  {
    switch(CUSTOMER_ID)
    {
      case CUSTOMER_DTMB_DESAI_JIMO:
  	  case CUSTOMER_XINNEW_GAIZHOU:
  		break;
  	  default:
  		ui_comm_dlg_open(&saving_data);
  		break;
    }		
  }

  /* set callback func */
  UI_PRINTF("do_search_post_close: reset the desktop notify func\n");

  //to enable book remind.
  ui_set_book_flag(TRUE);

  fw_tmr_destroy(ROOT_ID_DO_SEARCH, MSG_ANIMATION);

  /* fix bug 18050 */
  ui_signal_set_lock(TRUE);

  /* delete the local view */

  /* chk, if need sort on autoscan mode */

  /* recheck group info*/

  ui_dbase_reset_last_prog(is_db_empty);

  ui_set_front_panel_by_str("----");

  /* restart epg in ui_play_channel */
  ui_release_scan();

  // restart epg after finish scan
  ui_epg_init();

#ifndef IMPL_NEW_EPG
  ui_epg_start();
#else
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#endif
  /* restart monitor */
//  ui_simon_init();
//  ui_simon_start();

  // disable pb
  ui_enable_playback(TRUE);

  ui_time_init();
#if ENABLE_TTX_SUBTITLE
  ui_enable_subt(TRUE);
#endif

  is_stop = is_finished = TRUE;

  if((prog_curn[0] > 0)
    || (prog_curn[1] > 0))
  {
    ui_comm_dlg_close();
  }

}


BOOL do_search_is_finish(void)
{
  return is_finished;
}

static void do_search_get_main_tp(ui_scan_param_t *scan_param)
{
  nim_modulation_t nim_modul = NIM_MODULA_AUTO;
  dvbc_lock_t tp = {0};
  sys_status_get_main_tp1(&tp);

  scan_param->nit_type = NIT_SCAN_ONCE;
  scan_param->tp.freq = tp.tp_freq;
  scan_param->tp.sym = tp.tp_sym;

  switch(tp.nim_modulate)
  {
    case 0:
      nim_modul = NIM_MODULA_AUTO;
      break;

    case 1:
      nim_modul = NIM_MODULA_QPSK;
      break;

    case 2:
      nim_modul = NIM_MODULA_BPSK;
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

  scan_param->tp.nim_modulate = nim_modul;
}

#ifdef DTMB_PROJECT
static void ui_do_search_dtmb_full_search(void)
{
	dvbs_tp_node_t tp[FULL_SCAN_CNT];
	sat_node_t sat_node = {0};
	u16 i = 0;
	u8 scan_cnt = 0 ;
	BOOL lcn_enable = FALSE;
	
	memset(tp, 0, sizeof(dvbs_tp_node_t) * FULL_SCAN_CNT);
	ui_scan_param_init();
	ui_scan_add_dvbt_sat(FALSE, &sat_node);
	ui_scan_param_add_sat(&sat_node);

	if(CUSTOMER_ID == CUSTOMER_DTMB_CHANGSHA_HHT)
	{
		tp[0].freq = 690000;
		scan_cnt = 7;
	}
	else
	{
		tp[0].freq = 474000;
		scan_cnt = FULL_SCAN_CNT;
	}
	for(i = 0; i < scan_cnt; i++)
	{
		tp[i].freq 	   = tp[0].freq + (i * 8000);
		tp[i].sym      = DTMB_HN_SYMBOL_DEFAULT;
		tp[i].sat_id   = sat_node.id;
		tp[i].nim_type = NIM_DVBT;
		ui_scan_param_add_tp(&tp[i]);
	}

	sys_status_get_status(BS_LCN_RECEIVED, &lcn_enable);
	#ifdef SEARCH_SUPPORT_FILTER
	if(TRUE == lcn_enable)
	{
		ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, filter_type, NIT_SCAN_ALL_TP);
	}
	else
	{
		ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, filter_type, NIT_SCAN_WITHOUT);
	}	
	#else
	if(TRUE == lcn_enable)
	{
		ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, FALSE, NIT_SCAN_ALL_TP);
	}
	else
	{
		ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);
	}
	#endif
	manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DTMB_FULL, 0);
}
#else
extern s32 g_DVBCFullFreqTable_Chi[] ;
static void ui_do_search_dvbc_full_search(void)
{
    ui_scan_param_t scan_param;
    sat_node_t sat_node = {0};
    u8 i;

	memset(&scan_param, 0, sizeof(ui_scan_param_t));

	scan_param.tp.sym = 6875;
	scan_param.tp.nim_modulate = NIM_MODULA_QAM64;
	scan_param.nit_type = NIT_SCAN_ONCE;

	ui_scan_param_init();
	ui_scan_add_dvbc_sat(&sat_node);
	ui_scan_param_add_sat(&sat_node);

	for(i =0; i< 94 ; i++)
	{
		scan_param.tp.freq = g_DVBCFullFreqTable_Chi[i];
		ui_scan_param_add_tp(&scan_param.tp);
	}
	#ifdef SEARCH_SUPPORT_FILTER
	ui_scan_param_set_type(USC_DVBC_FULL_SCAN, CHAN_ALL, filter_type, NIT_SCAN_WITHOUT);
	#else
	ui_scan_param_set_type(USC_DVBC_FULL_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);
	#endif
	manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DVBC_RANGE, 0);
}
#endif

void ui_do_search_full_search(void)
{
	#ifdef DTMB_PROJECT
	ui_do_search_dtmb_full_search();
	#else
	ui_do_search_dvbc_full_search();
	#endif
}

static BOOL ui_auto_dtmb_search_parameter_init()
{  
  dvbs_tp_node_t tp = {0};
  scan_param_t scan_param = {0};
  sat_node_t sat_node = {0};
  u16 rid = 0;
  dvbc_lock_t main_tp = {0};

  /* init, use video buffer */
  ui_scan_param_init();
  ui_scan_add_dvbt_sat(FALSE, &sat_node);
  ui_scan_param_add_sat(&sat_node);
  /* add preset tp */
  sys_status_get_scan_param(&scan_param);
  sys_status_get_main_tp1(&main_tp);
  
  if(main_tp.tp_freq < SEARCH_FREQ_MIN)
    main_tp.tp_freq = SEARCH_FREQ_MIN;
  if(main_tp.tp_freq > SEARCH_FREQ_MAX)
    main_tp.tp_freq = SEARCH_FREQ_MAX;
  tp.freq = main_tp.tp_freq;
  tp.sym = DTMB_HN_SYMBOL_DEFAULT;
  tp.sat_id = rid;
  tp.nim_type = NIM_DTMB;
  ui_scan_param_add_tp(&tp);
  
  /* init scan param */
  #ifdef SEARCH_SUPPORT_FILTER
  ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, filter_type,NIT_SCAN_ONCE);
  #else
  ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, FALSE,NIT_SCAN_ONCE);
  #endif

  return TRUE;
}

static BOOL ui_regular_dtmb_get_search_freq(u32 *freq, u8 *freq_num)
{
	switch(CUSTOMER_ID)
	{
		case CUSTOMER_MAIKE_HUNAN_LINLI:
			{
			u32 cur_freq[] = {746000, 754000, 770000, 778000, 786000, 794000};
			*freq_num = sizeof(cur_freq)/sizeof(cur_freq[0]);
			memcpy(freq, cur_freq,*freq_num*sizeof(cur_freq[0]));
			}
			break;
		case CUSTOMER_DTMB_SHANGSHUIXIAN:
			{
			u32 cur_freq[] = {706000, 730000, 746000, 762000};
			*freq_num = sizeof(cur_freq)/sizeof(cur_freq[0]);
			memcpy(freq, cur_freq,*freq_num*sizeof(cur_freq[0]));
			}
			break;
		case CUSTOMER_DTMB_DESAI_JIMO:
			{
			u32 cur_freq[] = {714000, 522000, 610000};
			*freq_num = sizeof(cur_freq)/sizeof(cur_freq[0]);
			memcpy(freq, cur_freq,*freq_num*sizeof(cur_freq[0]));
			}			
			break;
		case CUSTOMER_DTMB_PCBA_GUOMAO:
		case CUSTOMER_DTMB_SZXC_GUOMAO:
			{
			u32 cur_freq[] = {522000, 530000, 666000,674000,690000,706000};
			*freq_num = sizeof(cur_freq)/sizeof(cur_freq[0]);
			memcpy(freq, cur_freq,*freq_num*sizeof(cur_freq[0]));
			}			
			break;
		case CUSTOMER_DTMB_CHANGSHA_HHT:
			{
			u32 cur_freq[] = {690000, 698000, 706000,714000,738000};
			*freq_num = sizeof(cur_freq)/sizeof(cur_freq[0]);
			memcpy(freq, cur_freq,*freq_num*sizeof(cur_freq[0]));
			}			
			break;

		default:
			return FALSE;
	}
	return TRUE;
}

static BOOL ui_regular_dtmb_search_parameter_init(void)
{  
  dvbs_tp_node_t tp[FULL_SCAN_CNT];
  sat_node_t sat_node = {0};
  u8 i;
  u8 freq_num = 0;
  u32 freq[FULL_SCAN_CNT] = {0};

  if(ui_regular_dtmb_get_search_freq(freq, &freq_num) == FALSE)
  	return FALSE;

  memset(tp, 0, sizeof(dvbs_tp_node_t) * FULL_SCAN_CNT);
  /* init, use video buffer */
  ui_scan_param_init();
  ui_scan_add_dvbt_sat(FALSE, &sat_node);
  ui_scan_param_add_sat(&sat_node);
  /* add preset tp */
  
  for(i=0; i< freq_num; i++)
  {
      tp[i].freq = freq[i];
      tp[i].sym = DTMB_HN_SYMBOL_DEFAULT;
      tp[i].sat_id = sat_node.id;
      ui_scan_param_add_tp(&tp[i]);
  }

  /* init scan param */
  switch(CUSTOMER_ID)
  {
	case CUSTOMER_DTMB_DESAI_JIMO:
		ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, FALSE, NIT_SCAN_ONCE);
		break;
	default:
		ui_scan_param_set_type(USC_DTMB_SCAN, CHAN_ALL, FALSE, NIT_SCAN_WITHOUT);
		break;
  }
  return TRUE;
}

RET_CODE open_do_search(u32 para1, u32 para2)
{
  control_t *p_cont, *p_title, *p_list, *p_bar;
  ui_scan_param_t scan_param;
  control_t *p_ctrl, *p_subctrl;
  list_xstyle_t rstyle =
  {
    RSI_PBACK, RSI_PBACK,
    RSI_PBACK, RSI_PBACK,
    RSI_PBACK
  };

  u16 title_txt[DO_SEARCH_TITLE_CNT] = {IDS_TV, IDS_RADIO};

  list_xstyle_t fstyle =
  {
    FSI_DO_SEARCH_LIST_ITEM, FSI_DO_SEARCH_LIST_ITEM,
    FSI_DO_SEARCH_LIST_ITEM, FSI_DO_SEARCH_LIST_ITEM,
    FSI_DO_SEARCH_LIST_ITEM
  };
  u16 i;
  u16 title_strid = RSC_INVALID_ID;
  dvbs_tp_node_t tp;
  sat_node_t sat_node = {0};

  title_strid = IDS_MANUAL_SEARCH;

  //stop monitor service
  {
    m_svc_cmd_p_t param = {0};

    dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_STOP_CMD, &param);
  }
  if(ui_is_playing())
    {
        DEBUG(DBG, INFO, "stop play\n");
        ui_stop_play(STOP_PLAY_BLACK, TRUE);
    }
  switch((u8)para1)
  {
    case SCAN_TYPE_DVBC_RANGE:
      do_search_reset_pg_and_bouquet();
	  ui_scan_add_dvbc_sat(&sat_node);
      ui_scan_param_add_sat(&sat_node);
      title_strid = IDS_RANGE_SEARCH;
      break;
    case SCAN_TYPE_DVBC_MANUAL:
      title_strid = IDS_MANUAL_SEARCH;
      memcpy((void *)&scan_param, (void *)para2, sizeof(ui_scan_param_t));
      if(scan_param.nit_type != NIT_SCAN_WITHOUT)
      {
        do_search_reset_pg_and_bouquet();
      }
      ui_scan_param_init();
      ui_scan_add_dvbc_sat(&sat_node);
      ui_scan_param_add_sat(&sat_node);
      memset(&tp, 0, sizeof(dvbs_tp_node_t));
      ui_scan_param_add_tp(&scan_param.tp);
	  #ifdef SEARCH_SUPPORT_FILTER
	  ui_scan_param_set_type(USC_DVBC_MANUAL_SCAN, CHAN_ALL, filter_type, scan_param.nit_type);
	  #else
      ui_scan_param_set_type(USC_DVBC_MANUAL_SCAN, CHAN_ALL, FALSE, scan_param.nit_type);
	  #endif
      break;
    case SCAN_TYPE_DVBC_AUTO:
      title_strid = IDS_AUTOL_SEARCH;

      do_search_reset_pg_and_bouquet();
      if((void *)para2 != NULL)
      {
        memcpy((void *)&scan_param, (void *)para2, sizeof(ui_scan_param_t));
      }
      else
      {
        do_search_get_main_tp(&scan_param);
      }
      ui_scan_param_init();
      ui_scan_add_dvbc_sat(&sat_node);
      ui_scan_param_add_sat(&sat_node);
      memset(&tp, 0, sizeof(dvbs_tp_node_t));
      ui_scan_param_add_tp(&scan_param.tp);
	  #ifdef SEARCH_SUPPORT_FILTER
	  ui_scan_param_set_type(USC_DVBC_MANUAL_SCAN, CHAN_ALL, filter_type, scan_param.nit_type);
	  #else
      ui_scan_param_set_type(USC_DVBC_MANUAL_SCAN, CHAN_ALL, FALSE, scan_param.nit_type);
	  #endif
      break;
    case SCAN_TYPE_DTMB_AUTO:
      title_strid = IDS_AUTOL_SEARCH;

      do_search_reset_pg_and_bouquet();

	  switch(CUSTOMER_ID)
	  {
		case CUSTOMER_DTMB_SHANGSHUIXIAN:
		case CUSTOMER_MAIKE_HUNAN_LINLI:
		case CUSTOMER_DTMB_PCBA_GUOMAO:
		case CUSTOMER_DTMB_SZXC_GUOMAO:
		case CUSTOMER_DTMB_DESAI_JIMO:
		case CUSTOMER_DTMB_CHANGSHA_HHT:
			ui_regular_dtmb_search_parameter_init();
			break;
		default:
			ui_auto_dtmb_search_parameter_init();
			break;
	  }
      break;
    case SCAN_TYPE_DTMB_MANUAL:
      title_strid = IDS_MANUAL_SEARCH;
      break;
    case SCAN_TYPE_DTMB_FULL:
	  do_search_reset_pg_and_bouquet();
  	  ui_scan_add_dvbt_sat(FALSE, &sat_node);
  	  ui_scan_param_add_sat(&sat_node);
      title_strid = IDS_FULL_SCAN;
      break;
    default:
      MT_ASSERT(0);
      return ERR_FAILURE;
  }

  do_search_pre_open();

#ifndef SPT_SUPPORT
  p_cont = ui_comm_root_cont_create(ROOT_ID_DO_SEARCH, 0,
                            COMM_BG_X, COMM_BG_Y,
                            COMM_BG_W, COMM_BG_H,
                            OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_keymap(p_cont, do_search_cont_keymap);
  ctrl_set_proc(p_cont, do_search_cont_proc);

  // prog part
  p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_PROG_INFO_FRM,
                            DO_SEARCH_FRM_X, DO_SEARCH_FRM_Y,
                            DO_SEARCH_FRM_W, DO_SEARCH_FRM_H,
                            p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  for(i = 0; i < DO_SEARCH_TITLE_CNT; i++)
  {
    p_title = ctrl_create_ctrl(CTRL_CONT, (u8) (IDC_TITLE_TV + i),
                                 DO_SEARCH_TITLE_BG_X + i * (DO_SEARCH_TITLE_AND_TITLE_HGAP + DO_SEARCH_TITLE_BG_W),
                                 DO_SEARCH_TITLE_BG_Y,
                                 DO_SEARCH_TITLE_BG_W,
                                 DO_SEARCH_TITLE_BG_H,
                                 p_ctrl,
                                 0);
    ctrl_set_rstyle(p_title, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);

    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) 0,
                                 DO_SEARCH_TITLE_NAME_X,
                                 DO_SEARCH_TITLE_NAME_Y,
                                 DO_SEARCH_TITLE_NAME_W,
                                 DO_SEARCH_TITLE_NAME_H,
                                 p_title, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_RIGHT | STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_VERMILION, FSI_VERMILION, FSI_VERMILION);
    text_set_content_by_strid(p_subctrl, title_txt[i]);

    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, (u8) IDC_NUM,
                                 DO_SEARCH_TITLE_NUM_X,
                                 DO_SEARCH_TITLE_NUM_Y,
                                 DO_SEARCH_TITLE_NUM_W,
                                 DO_SEARCH_TITLE_NUM_H,
                                 p_title, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_VERMILION, FSI_VERMILION, FSI_VERMILION);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
  }

  for(i = 0; i < DO_SEARCH_LIST_CNT; i++)
  {
    p_list = ctrl_create_ctrl(CTRL_LIST, (u8) (IDC_LIST_TV + i),
                              DO_SEARCH_LIST_CONT_X + i * (DO_SEARCH_TITLE_AND_TITLE_HGAP + DO_SEARCH_LIST_CONT_W),
                              DO_SEARCH_LIST_CONT_Y,
                              DO_SEARCH_LIST_CONT_W,
                              DO_SEARCH_LIST_CONT_H,
                              p_ctrl, 0);

    ctrl_set_rstyle(p_list, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);
    
    list_set_count(p_list,
                   DO_SEARCH_LIST_PAGE,
                   DO_SEARCH_LIST_PAGE);
    ctrl_set_mrect(p_list, DO_SEARCH_LIST_X, DO_SEARCH_LIST_Y,
                      DO_SEARCH_LIST_W+DO_SEARCH_LIST_X,
                      DO_SEARCH_LIST_H+DO_SEARCH_LIST_Y);
    list_set_item_interval(p_list, DO_SEARCH_LIST_ITEM_V_GAP);
    list_set_item_rstyle(p_list, &rstyle);

    list_set_field_count(p_list, DO_SEARCH_LIST_FIELD_CNT,
                   DO_SEARCH_LIST_PAGE);
    list_set_field_attr(p_list, 0,
                        STL_LEFT | STL_VCENTER | LISTFIELD_TYPE_EXTSTR,
                        DO_SEARCH_LIST_W - 30, 30, 0);
    list_set_field_font_style(p_list, 0, &fstyle);

    init_prog_list_content(p_list);
  }

  // tp part
  p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_TP_INFO_FRM, 
                            DO_SEARCH_TP_LIST_FRM_X, DO_SEARCH_TP_LIST_FRM_Y, 
                            DO_SEARCH_TP_LIST_FRM_W, DO_SEARCH_TP_LIST_FRM_H, 
                            p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);

  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_TP_LIST,
                            DO_SEARCH_TP_LIST_X, DO_SEARCH_TP_LIST_Y,
                            DO_SEARCH_TP_LIST_W, DO_SEARCH_TP_LIST_H,
                            p_ctrl, 0);
  
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  list_set_count(p_list,
                 DO_SEARCH_TP_LIST_PAGE,
                 DO_SEARCH_TP_LIST_PAGE);
  ctrl_set_mrect(p_list, 0, 0,
                    DO_SEARCH_TP_LIST_W,
                    DO_SEARCH_TP_LIST_H);
  list_set_item_interval(p_list,DO_SEARCH_TP_LIST_ITEM_V_GAP);
  list_set_item_rstyle(p_list, &rstyle);

  list_set_field_count(p_list, DO_SEARCH_TP_LIST_FIELD_CNT,
                  DO_SEARCH_TP_LIST_PAGE);
  list_set_field_attr(p_list, 0,
                      STL_LEFT | STL_VCENTER | LISTFIELD_TYPE_EXTSTR,
                      DO_SEARCH_TP_LIST_NO_W, 0, 0);
  list_set_field_font_style(p_list, 0, &fstyle);
  list_set_field_attr(p_list, 1,
                      STL_LEFT | STL_VCENTER | LISTFIELD_TYPE_EXTSTR,
                      DO_SEARCH_TP_LIST_SAT_W, DO_SEARCH_TP_LIST_NO_W, 0);
  list_set_field_font_style(p_list, 1, &fstyle);
  list_set_field_attr(p_list, 2,
                      STL_LEFT | STL_VCENTER | LISTFIELD_TYPE_EXTSTR,
                      DO_SEARCH_TP_LIST_INFO_W, DO_SEARCH_TP_LIST_NO_W + DO_SEARCH_TP_LIST_SAT_W, 0);
  list_set_field_font_style(p_list, 2, &fstyle);
  init_tp_list_content(p_list);

  // search animation
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_ANIMATION,
                                DO_SEARCH_ANIMATION_X, DO_SEARCH_ANIMATION_Y, 
                                DO_SEARCH_ANIMATION_W, DO_SEARCH_ANIMATION_H, 
                                p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_DO_SEARCH_ITEM, RSI_DO_SEARCH_ITEM, RSI_DO_SEARCH_ITEM);
  bmap_set_content_by_id(p_subctrl, animation_icon[animation_focus]);
  
  // progress bar  
  p_bar = ui_comm_bar_create(p_cont, IDC_PBAR,
                               DO_SEARCH_PBAR_X, DO_SEARCH_PBAR_Y,
                               DO_SEARCH_PBAR_W, DO_SEARCH_PBAR_H,
                               0, 0, 0, 0,
                               DO_SEARCH_PBAR_PERCENT_X,
                               DO_SEARCH_PBAR_PERCENT_Y,
                               DO_SEARCH_PBAR_PERCENT_W,
                               DO_SEARCH_PBAR_PERCENT_H);
  ui_comm_bar_set_param(p_bar, RSC_INVALID_ID, 0, 100, 100);

  ui_comm_bar_set_style(p_bar,
                               RSI_DO_SEARCH_PBAR_BG, RSI_DO_SEARCH_PBAR_MID,
                               RSI_IGNORE, FSI_WHITE,
                               RSI_PBACK, FSI_WHITE);
  ui_comm_bar_update(p_bar, 0, TRUE);
  ctrl_set_sts(p_bar, OBJ_STS_SHOW);
#else
#endif

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

//  nc_enable_monitor(class_get_handle_by_id(NC_CLASS_ID), TUNER0, FALSE);
  // start scan
  ui_start_scan();
  
  return SUCCESS;
}

static RET_CODE on_pg_found(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  UI_PRINTF("PROCESS -> add prog\n");
  add_prog_to_list(cont, (dvbs_prog_node_t *)para1);
  return SUCCESS;
}

static RET_CODE on_tp_found(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  UI_PRINTF("PROCESS -> add/change tp\n");
  add_tp_to_list(cont, (dvbs_tp_node_t *)para1);
  return SUCCESS;
}

static RET_CODE on_nit_found(control_t *cont, u16 msg,
                                   u32 para1, u32 para2)
{
  u32 old_nit_ver = 0;
  sys_status_get_nit_version(&old_nit_ver);
  if(old_nit_ver != para2)
  {
    sys_status_set_nit_version(para2);
  }
  return SUCCESS;
}

static RET_CODE on_update_progress(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  UI_PRINTF("PROCESS -> update progress\n");
  update_progress(cont, (u16)para1);
  return SUCCESS;
}

static RET_CODE on_update_animation(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  set_animation_icon(cont);
  return SUCCESS;
}

static RET_CODE on_db_is_full(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  UI_PRINTF("PROCESS -> dbase full\n");
  // pause scanning, at first
  UI_PRINTF("DO_SEARCH: pause scan!\n");
  ui_pause_scan();
  
  ui_comm_cfmdlg_open(NULL, IDS_MSG_SPACE_IS_FULL, do_cancel, 0);
  return SUCCESS;
}


static RET_CODE on_finished(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  if(!is_finished)
  {
    UI_PRINTF("PROCESS -> finish\n");
    OS_PRINTF("*************************on finished : %d\n", mtos_ticks_get());
    is_finished = TRUE;
    process_finish();
    OS_PRINTF("*************************process finished : %d\n", mtos_ticks_get());
    ui_close_all_mennus();
    OS_PRINTF("*************************close all menus : %d\n", mtos_ticks_get());

    if(prog_curn[0] == 0|| prog_curn[1] == 0)
    {
  		nc_enable_monitor(class_get_handle_by_id(NC_CLASS_ID), TUNER0, TRUE);
    }
  }
  #ifdef SEARCH_SUPPORT_FILTER
  ui_do_search_clear_search_filter_type();
  #endif
  return SUCCESS;
}

static RET_CODE on_stop_scan(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  if (!is_stop)
  {
    // pause scanning, at first
    UI_PRINTF("DO_SEARCH: pause scan!\n");
    ui_pause_scan();
    is_ask_for_cancel = TRUE;
    // ask for cancel
    ui_comm_ask_for_dodlg_open(NULL, IDS_MSG_ASK_FOR_EXIT_SCAN, 
                               do_cancel, undo_cancel, 0);
    is_ask_for_cancel = FALSE;
  }
  return SUCCESS;
}

static RET_CODE on_save_do_search(control_t *cont, u16 msg, 
                                   u32 para1, u32 para2)
{
  UI_PRINTF("DO_SEARCH: before MSG_SAVE!\n");
  if (!is_stop) /* cancel check, dvbs_cancel_scan don't clear it */
  {
    UI_PRINTF("DO_SEARCH: cancel scan!\n");
    ui_stop_scan();
  }
  
  ui_dbase_pg_sort(DB_DVBS_ALL_PG);
  
  OS_PRINTF("*************************do_search_post_close 1 : %d\n", mtos_ticks_get());
  do_search_post_close();
  OS_PRINTF("*************************do_search_post_close 2 : %d\n", mtos_ticks_get());

  #ifdef SHUSHITONG_CA
  if(CUSTOMER_ID == CUSTOMER_DTMB_CHANGSHA_HHT)
  {
    update_have_valid_freq_flag();
  }
  #endif

  return SUCCESS;
}


BEGIN_KEYMAP(do_search_cont_keymap, NULL)
  ON_EVENT(V_KEY_CANCEL, MSG_STOP_SCAN)
  ON_EVENT(V_KEY_MENU, MSG_STOP_SCAN)
  ON_EVENT(V_KEY_BACK, MSG_STOP_SCAN)
END_KEYMAP(do_search_cont_keymap, NULL)

BEGIN_MSGPROC(do_search_cont_proc, cont_class_proc)
  ON_COMMAND(MSG_SCAN_PG_FOUND, on_pg_found)
  ON_COMMAND(MSG_SCAN_TP_FOUND, on_tp_found)
  ON_COMMAND(MSG_SCAN_PROGRESS, on_update_progress)
  ON_COMMAND(MSG_ANIMATION, on_update_animation)
  ON_COMMAND(MSG_SCAN_FINISHED, on_finished)
  ON_COMMAND(MSG_STOP_SCAN, on_stop_scan)
  ON_COMMAND(MSG_SAVE, on_save_do_search)
  ON_COMMAND(MSG_SCAN_DB_FULL, on_db_is_full)
ON_COMMAND(MSG_SCAN_NIT_FOUND, on_nit_found)
END_MSGPROC(do_search_cont_proc, cont_class_proc)



