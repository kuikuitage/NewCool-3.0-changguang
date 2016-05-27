/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_signal.h"
#include "ui_pause.h"
#include "ui_mute.h"
#include "ap_framework.h"
#include "ui_live_tv_api.h"
#include "ui_small_list_v2.h"
//#include "ap_twin_port.h"
#ifdef ENABLE_CA
#include "cas_manager.h"
#include "config_cas.h"
#include "ui_ca_public.h"
#endif



struct
{
  u8 type;
  BOOL is_chkpwd;
  u16  passchk_pg;
} g_chkpwd_info = { LOCK_TYPE_NONE, TRUE, INVALIDID};


static play_info_t g_play_info = { 0, INVALIDID, CURN_MODE_NONE, FALSE };
static BOOL g_add_to_history = TRUE;
#if ENABLE_TTX_SUBTITLE
static u8 g_national_charset_code = 0;
#endif

static play_info_t g_play_cnxt[MAX_PLAY_CNXT_COUNT] =
{
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE },
  { 0, INVALIDID, CURN_MODE_NONE, FALSE }
};
static u8 g_play_cnxt_count = 0;

static u8 g_play_mode = CURN_MODE_NONE;

static BOOL g_is_playing = FALSE;
static BOOL g_is_blkscr = TRUE;
static BOOL g_is_encrypt = FALSE;
static BOOL g_have_pass_pwd = FALSE;
static BOOL g_is_playback_active = FALSE;

static u8 g_play_timer_state = 0; /* 0: stopped, 1: running */
static u16 g_play_timer_owner = 0;
static u16 g_play_timer_notify = 0;
static u32 g_play_timer_tmout = 300;
static play_prg_type_t g_play_prg_type = SAT_PRG_TYPE;



enum plist_ctrl_prog_id
{
  IDC_SLIST_CONT = 1,  
  IDC_PLIST_LIST_CONT = 2,
  IDC_PLCONT_LIST = 8,
};

extern u8 get_menu_state_by_id(u8 id);

u16 ui_playback_evtmap(u32 event);

u16 pwdlg_keymap_in_scrn(u16 key);

u16 pwdlg_keymap_in_list(u16 key);

play_param_t g_pb_info = {0};


RET_CODE pwdlg_proc_in_scrn(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);

RET_CODE pwdlg_proc_in_list(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);

extern BOOL get_subt_description_1(class_handle_t handle,
        pb_subt_info_t *p_info, u16 pg_id);
extern RET_CODE aud_get_dolby_status(void *p_dev, aud_dolby_status_vsb_t *status);

void ui_set_play_prg_type(play_prg_type_t prg_type)
{
  g_play_prg_type = prg_type;
}

play_prg_type_t ui_get_play_prg_type(void)
{
  return g_play_prg_type;
}

#if ENABLE_TTX_SUBTITLE
void ui_enable_vbi_insert(BOOL is_enable)
{
  cmd_t cmd;

  cmd.id = is_enable ? PB_CMD_START_VBI_INSERTER : PB_CMD_STOP_VBI_INSERTER;
  cmd.data1 = 0;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
  OS_PRINTF("UI: %s vbi insert.........\n", is_enable ? "enable" : "disable");
}

BOOL ui_is_ttx_data_ready(u16 prog_id)
{
  pb_ttx_info_t ttx_info;
  class_handle_t handle = class_get_handle_by_id(VBI_SUBT_CTRL_CLASS_ID);

  if(!get_ttx_description_1(handle, &ttx_info, prog_id))
//  if(!pb_get_ttx_description(&ttx_info, prog_id))
  {
    OS_PRINTF("ui_is_ttx_data_ready: -----1\n");
    return FALSE;
  }

  if(ttx_info.service_cnt == 0)
  {
    OS_PRINTF("ui_is_ttx_data_ready: -----2\n");
    return FALSE;
  }
  else
  {
    s32 i = ttx_info.service_cnt;
    for (; i > 0; i --)
    {
        OS_PRINTF("Type[%d], i[%d]\n", ttx_info.service[i-1].type, i);
        if (ttx_info.service[i-1].type == TTX_INITIAL_PAGE
            || ttx_info.service[i-1].type == TTX_ADDITIONAL_INFO_PAGE
            || ttx_info.service[i-1].type == TTX_PROG_SCHEDULE_PAGE)
            return TRUE;
    }

    OS_PRINTF("ui_is_ttx_data_ready: -----3\n");
    return FALSE;
  }
}


BOOL ui_is_subt_data_ready(u16 prog_id)
{
  pb_subt_info_t subt_info;
  class_handle_t handle = class_get_handle_by_id(VBI_SUBT_CTRL_CLASS_ID);

  if (!get_subt_description_1(handle, &subt_info, prog_id))
//  if (!pb_get_subt_description(&subt_info, prog_id))
  {
    return FALSE;
  }

  if (subt_info.service_cnt == 0)
  {
    return FALSE;
  }
  else
  {
    s32 i = subt_info.service_cnt;
    for (; i > 0; i --)
    {
        if (subt_info.service[i-1].type != SUBT_TYPE_UNDEF)
        {
          return TRUE;
        }
    }
  }
  return FALSE;
}

void ui_set_ttx_curn_sub(u8 index)
{
  g_national_charset_code = index;
}

u8 ui_get_ttx_curn_sub()
{
  return g_national_charset_code;
}
// ETSI EN 300 706 V1.2.1 (2003-04) p103
u8 ui_set_ttx_lang_cb_fun(u8 charset_code, u8 national, BOOL charset_flag, BOOL national_flag)
{
  if(!charset_flag)
  {
    //OS_PRINTF("g_national_charset_code = [0x%x]", g_national_charset_code);
    charset_code = g_national_charset_code; //set charset 
  }

  if(!national_flag)
  {
    national = 0x07; //set default language as Turket
  }
  
  return ((charset_code & ~7) + national);
}

void ui_enable_ttx(BOOL is_enable)
{
#if 1//portingUI temporary
  cmd_t cmd;

  cmd.id = is_enable ? PB_CMD_START_TTX : PB_CMD_STOP_TTX;
  cmd.data1 = (TELTEXT_MAX_PAGE_NUM_1 << 16) | TELTEXT_MAX_PAGE_NUM_2;
  cmd.data2 = (u32)(&ui_set_ttx_lang_cb_fun);

  ap_frm_do_command(APP_PLAYBACK, &cmd);

  OS_PRINTF("UI: %s ttx.........\n", is_enable ? "enable" : "disable");
#endif
}

void ui_post_ttx_key(u32 key)
{
  cmd_t cmd;

  cmd.id    = PB_CMD_POST_TTX_KEY;
  cmd.data1 = key;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);

  OS_PRINTF("UI: post ttx key ......... [%d]\n", key);
}



void ui_set_ttx_display(BOOL is_display, u32 regino_handle, s32 magzine,
                        s32 page)
{
  cmd_t cmd;

  if (is_display)
  {
    cmd.id    = PB_CMD_SHOW_TTX;
    cmd.data1 = regino_handle;

    if (magzine < 0 || page < 0)
    {
      cmd.data2 = INVALID_TTX_PAGE;
    }
    else
    {
      cmd.data2 = magzine * 256 + page;
    }
  }
  else
  {
	cmd.id = PB_CMD_HIDE_TTX;
  }

  ap_frm_do_command(APP_PLAYBACK, &cmd);

  OS_PRINTF("UI: %s ttx display.........\n", is_display ? "enable" : "disable");
}


void ui_set_ttx_language(char *lang)
{
  cmd_t cmd;
  u16 b_index = iso_639_2_desc_to_idx(lang);
  u16 t_index = iso_639_2_desc_to_idx(lang);

  MT_ASSERT(strlen(lang) == 3);

  cmd.id = PB_CMD_SET_TTX_LANG;
  cmd.data1 = MT_MAKE_DWORD(b_index, t_index);
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);

  OS_PRINTF("UI: set ttx language [%s].........\n", lang);
}


static BOOL is_enable_subt_on_setting(void)
{
  osd_set_t osd_set;
  sys_status_get_osd_set(&osd_set);

  return (BOOL)(osd_set.enable_subtitle);
}


BOOL is_enable_vbi_on_setting(void)
{
  osd_set_t osd_set;
  sys_status_get_osd_set(&osd_set);

  return (BOOL)(osd_set.enable_vbinserter);
}
#endif

BOOL is_ap_playback_active(void)
{
  return g_is_playback_active;
}

void ui_enable_playback(BOOL is_enable)
{
  cmd_t cmd;

  cmd.data1 = APP_PLAYBACK;
  cmd.data2 = 0;

  if (is_enable)
  {
    OS_PRINTF("------------->>active ap playback\n");
    cmd.id = AP_FRM_CMD_ACTIVATE_APP;

		ap_frm_do_command(APP_FRAMEWORK, &cmd);

    fw_register_ap_evtmap(APP_PLAYBACK, ui_playback_evtmap);
    fw_register_ap_msghost(APP_PLAYBACK, ROOT_ID_PROG_BAR);
    fw_register_ap_msghost(APP_PLAYBACK, ROOT_ID_PROG_LIST);
    fw_register_ap_msghost(APP_PLAYBACK, ROOT_ID_BACKGROUND);
    fw_register_ap_msghost(APP_PLAYBACK, ROOT_ID_XEXTEND);
    fw_register_ap_msghost(APP_PLAYBACK, ROOT_ID_FACTORY_TEST);
  	g_is_playback_active = TRUE;
    // enable ca
#ifndef WIN32
  #ifdef ENABLE_CA
	
		OS_PRINTF("@@@@ Dexin init ca when leave search !!!CAS_ID = %d	\n", CAS_ID);
	  
		if(CAS_ID  == CONFIG_CAS_ID_DS)
		{
		  OS_PRINTF("@@@@ Dexin init ca when leave search !!! \n");
		  ui_init_ca();
		}
  #endif
#endif
#if ENABLE_BISS_KEY
   {
    cmd_t cmd = {0};
    cmd.id = AP_FRM_CMD_ACTIVATE_APP;
    cmd.data1 = APP_TWIN_PORT;
    ap_frm_do_command(APP_FRAMEWORK, &cmd);
    }
#endif
#if ENABLE_MUSIC_PICTURE
  //ui_timeshift_init();
#endif
  }
  else
  {
#if ENABLE_MUSIC_PICTURE
  //ui_timeshift_release();
#endif
    cmd.id = AP_FRM_CMD_DEACTIVATE_APP;

		ap_frm_do_command(APP_FRAMEWORK, &cmd);

    fw_unregister_ap_evtmap(APP_PLAYBACK);
    fw_unregister_ap_msghost(APP_PLAYBACK, ROOT_ID_PROG_BAR);
    fw_unregister_ap_msghost(APP_PLAYBACK, ROOT_ID_PROG_LIST);
    fw_unregister_ap_msghost(APP_PLAYBACK, ROOT_ID_BACKGROUND);
    fw_unregister_ap_msghost(APP_PLAYBACK, ROOT_ID_XEXTEND);
    fw_unregister_ap_msghost(APP_PLAYBACK, ROOT_ID_FACTORY_TEST);
    g_is_playback_active = FALSE;
    // disable ca
#ifndef WIN32
  #ifdef ENABLE_CA
		if(CAS_ID  == CONFIG_CAS_ID_DS)
		{
		  OS_PRINTF("@@@@ Dexin release ca when do auto search !!! \n");
		  ui_release_ca();
		}
		else
		{
		  cas_manager_stop_ca_play();
		}
  #endif
#endif
#if ENABLE_BISS_KEY
   {
   cmd_t cmd = {0};
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_TWIN_PORT;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);
  }
 #endif
  }

  OS_PRINTF("UI: %s playback.........\n", is_enable ? "active" : "deactive");
}


void ui_pause_play(void)
{
  cmd_t cmd;

  cmd.id = PB_CMD_PAUSE;
  cmd.data1 = 0;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}


void ui_resume_play(void)
{
  cmd_t cmd;

  cmd.id = PB_CMD_RESUME;
  cmd.data1 = 0;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}


void ui_stop_play(stop_mode_t type, BOOL is_sync)
{
  cmd_t cmd;
#ifdef ENABLE_CA
#ifdef EXTERN_CA_PVR
	if(type != STOP_PLAY_FREEZE)
	  cas_manager_stop_ca_play();
#else
	cas_manager_stop_ca_play();
#endif
#endif
  cmd.id = is_sync ? PB_CMD_STOP_SYNC : PB_CMD_STOP;
  cmd.data1 = type;
  cmd.data2 = 0;
  OS_PRINTF("stop type[%d]\n", type);
  ap_frm_do_command(APP_PLAYBACK, &cmd);
  // set flag
  g_is_playing = FALSE;
  g_is_blkscr = (BOOL)(type == STOP_PLAY_BLACK);

#if ENABLE_TTX_SUBTITLE
  // subt display off
  OS_PRINTF("Before ui_set_subt_display\n");
  ui_set_subt_display(FALSE);
  OS_PRINTF("After ui_set_subt_display\n");
  ui_enable_subt(FALSE);
  OS_PRINTF("Leave ui_enable_subt\n");
  ui_enable_ttx(FALSE);
  // stop insert
  if (is_enable_vbi_on_setting())
  {
    ui_enable_vbi_insert(FALSE);
  }
  g_is_playing = FALSE;
#endif
}

void ui_auto_switch_mode_stop_play(BOOL is_sync)
{
	play_set_t play_set;
	stop_mode_t stop_mode = STOP_PLAY_NONE;

	sys_status_get_play_set(&play_set);
	stop_mode = play_set.mode ? STOP_PLAY_BLACK : STOP_PLAY_FREEZE;
	ui_stop_play(stop_mode, is_sync);
}

#if ENABLE_TTX_SUBTITLE
void ui_enable_subt(BOOL is_enable)
{
  //if (is_enable_subt_on_setting())
  {
    cmd_t cmd;

    cmd.id = is_enable ? PB_CMD_START_SUBT : PB_CMD_STOP_SUBT_SYNC;
    cmd.data1 = 0;
    cmd.data2 = 0;

    ap_frm_do_command(APP_PLAYBACK, &cmd);

    OS_PRINTF("UI: %s subt.........\n", is_enable ? "enable" : "disable");
  }
}

void ui_set_subt_display(BOOL is_display)
{
  if (is_enable_subt_on_setting())
  {
    cmd_t cmd;

    cmd.id    = is_display ? PB_CMD_SHOW_SUBT : PB_CMD_HIDE_SUBT;
    cmd.data1 = 0;
    cmd.data2 = 0;

    ap_frm_do_command(APP_PLAYBACK, &cmd);

    OS_PRINTF("UI: %s subt display.........\n",
              is_display ? "enable" : "disable");
  }
}


void ui_set_subt_service(u8 lang_idx, u32 type)
{
  //if (is_enable_subt_on_setting())
  {
    cmd_t cmd;
    char **p_lang_code_list_b = sys_status_get_lang_code(TRUE);
    char **p_lang_code_list_t = sys_status_get_lang_code(FALSE);
    char *p_cur_lang_code_b = p_lang_code_list_b[lang_idx];
    char *p_cur_lang_code_t = p_lang_code_list_t[lang_idx];
    u16 b_index = iso_639_2_desc_to_idx(p_cur_lang_code_b);
    u16 t_index = iso_639_2_desc_to_idx(p_cur_lang_code_t);

    cmd.id = PB_CMD_SET_SUBT_SERVICE;
    cmd.data1 = MT_MAKE_DWORD(b_index, t_index);
    cmd.data2 = type;

    ap_frm_do_command(APP_PLAYBACK, &cmd);

  }
}


void ui_set_subt_service_1(char *lang_code, u32 type)
{
  //if (is_enable_subt_on_setting())
  {
    cmd_t cmd;

    cmd.id = PB_CMD_SET_SUBT_LANG_CODE;
    cmd.data1 = (u32)(lang_code[2] << 16 |lang_code[1] << 8 | lang_code[0]);
    cmd.data2 = type;

    ap_frm_do_command(APP_PLAYBACK, &cmd);

  }
}
#endif




BOOL ui_is_playing(void)
{
  return g_is_playing;
}


BOOL ui_is_blkscr(void)
{
  return g_is_blkscr;
}

BOOL ui_is_have_pass_pwd(void)
{
  return g_have_pass_pwd;
}

void ui_set_have_pass_pwd(BOOL flag)
{
  g_have_pass_pwd = flag;
}

static void play_pg_with_pwd(u32 prog_id)
{
  comm_pwdlg_data_t pwdlg_data =
  {
    ROOT_ID_INVALID,
    PWD_DLG_FOR_PLAY_X,
    PWD_DLG_FOR_PLAY_Y - 25, //-25 for fix bug 18201
    IDS_MSG_INPUT_PASSWORD,
    NULL,
    NULL
  };

  u8 root_id = fw_get_focus_id();

  if(root_id == ROOT_ID_NUM_PLAY)
  {
//    manage_close_menu(ROOT_ID_NUM_PLAY, 0, 0);
    if(fw_find_root_by_id(ROOT_ID_NUM_PLAY))
    {
      fw_notify_root(fw_find_root_by_id(ROOT_ID_NUM_PLAY), NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
    }
  }
  if(fw_find_root_by_id(ROOT_ID_POPUP) != NULL)
  {
    ui_comm_dlg_close();
  }

  //if (ui_sys_get_curn_prog_in_curn_view() == prog_id)
  if (sys_status_get_curn_group_curn_prog_id() == prog_id)
  {
    if (root_id == ROOT_ID_PROG_LIST
        || root_id == ROOT_ID_EPG
        || root_id == ROOT_ID_VEPG
        || root_id == ROOT_ID_SMALL_LIST)
    {
      pwdlg_data.keymap = pwdlg_keymap_in_list;
      pwdlg_data.proc = pwdlg_proc_in_list;
    }
    else
    {
      pwdlg_data.keymap = pwdlg_keymap_in_scrn;
      pwdlg_data.proc = pwdlg_proc_in_scrn;
    }
	if(NULL==fw_find_root_by_id(ROOT_ID_XEXTEND)&&NULL==fw_find_root_by_id(ROOT_ID_MAINMENU))
    	ui_comm_pwdlg_open(&pwdlg_data);
  }
}

#if ENABLE_TTX_SUBTITLE
/*
* Just display subtitle on fullscreen
*/
static void fullscreen_subtitle_display(void)
{
  if(is_enable_subt_on_setting())
  {
    if(ui_is_fullscreen_menu(fw_get_focus_id()))
    {
      ui_set_subt_display(TRUE);
    }
    else
    {
      ui_set_subt_display(FALSE);
    }
  }
}
#endif

void load_desc_paramter_by_pgid(cas_sid_t *p_info, u16 pg_id)
{
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};

  ret = db_dvbs_get_pg_by_id(pg_id, &pg);
  MT_ASSERT(DB_DVBS_OK == ret);
  ret = db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
  MT_ASSERT(DB_DVBS_OK == ret);
  p_info->ecm_num = (u8)(pg.ecm_num < 8 ? pg.ecm_num : 8);
  p_info->emm_num = (u8)(tp.emm_num < 8 ? tp.emm_num : 8);

  memcpy(p_info->ecm_info, pg.cas_ecm, p_info->ecm_num * sizeof(cas_desc_t));

  memcpy(p_info->emm_info, tp.emm_info, p_info->emm_num * sizeof(ca_desc_t));

  if ((CAS_ID == CONFIG_CAS_ID_DS) && (p_info->ecm_num == 0))
  {
    OS_PRINTF("@@@ set unscrambled pg ecm pid 0x1FFF !!!\n");
    p_info->ecm_num = 1;
    p_info->ecm_info[0].ecm_pid= 0x1FFF;
  }
}

static void do_play_pg(u16 pg_id)
{
  cmd_t cmd;
  language_set_t lang_set;
  aud_dolby_status_vsb_t dolby_status = AUDIO_DOLBY_NOT_SUPPORTED_BOTH;
  struct audio_device *p_dev = NULL;
  av_set_t av_set = {0};
#if ENABLE_TTX_SUBTITLE
  char **p_lang_code = sys_status_get_lang_code(FALSE);
#endif

  // check pause status
  if (ui_is_pause())
  {
    UI_PRINTF("DO_PLAY_PG: ui_set_pause(FALSE)\n");
    ui_set_pause(FALSE);
  }

  // show scramble mark or not
  update_signal();
  UI_PRINTF("DO_PLAY_PG:  id =%d \n", pg_id);
  load_play_paramter_by_pgid(&g_pb_info, pg_id);

  #if ENABLE_GLOBAL_VOLUME
  g_pb_info.pg_info.audio_volume = (u16)sys_status_get_global_volume();
  #endif

  //if set DIGITAL_OUTPUT as BS out,  change audio type
  sys_status_get_av_set(&av_set);

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
    aud_get_dolby_status(p_dev, &dolby_status);
    OS_PRINTF("dolby_status %d\n", dolby_status);
  if((1 == av_set.digital_audio_output)
    && ((AUDIO_AC3_VSB == g_pb_info.pg_info.audio_type)
    ||(AUDIO_EAC3 == g_pb_info.pg_info.audio_type))) 
  {
  	g_pb_info.pg_info.audio_type = AUDIO_EAC3;
  }

  // to play
  cmd.id = PB_CMD_PLAY;
  cmd.data1 = (u32)&g_pb_info;
  cmd.data2 = 0;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
  #if ENABLE_BISS_KEY
    if(g_pb_info.pg_info.is_scrambled)
   {
     sat_node_t sat_node = {0};
     dvbs_tp_node_t tp_node = {0};

    dvbs_prog_node_t pg_node = {0};


     db_dvbs_ret_t db_ret = DB_DVBS_OK;
     twin_port_info_t twin_port = {0};
    db_ret =  db_dvbs_get_pg_by_id(pg_id, &pg_node);
    MT_ASSERT(db_ret == DB_DVBS_OK);
    db_ret = db_dvbs_get_sat_by_id(pg_node.sat_id, &sat_node);
    MT_ASSERT(db_ret == DB_DVBS_OK);

    db_ret = db_dvbs_get_tp_by_id(pg_node.tp_id, &tp_node);
    MT_ASSERT(db_ret == DB_DVBS_OK);

     twin_port.svc_id = g_pb_info.pg_info.s_id;
    twin_port.pg_id = pg_id;
    twin_port.cur_audio_pid = pg_node.audio[pg_node.audio_channel].p_id;
    twin_port.cur_video_pid = pg_node.video_pid;
    twin_port.freq = tp_node.freq;
    twin_port.sat_degree = db_dvbs_get_sat_degree(&sat_node);
    ui_set_twin_port(&twin_port);
    }
#endif

#ifdef IMPL_NEW_EPG
  ui_epg_set_program_info(pg_id);
#endif

  sys_status_get_lang_set(&lang_set);
#if ENABLE_TTX_SUBTITLE
  // set teletext language
  ui_enable_ttx(TRUE);
  ui_set_ttx_language(p_lang_code[lang_set.tel_text]);
  // subt display on
  ui_enable_subt(TRUE);
  ui_set_subt_service(lang_set.sub_title, SUBT_TYPE_NORMAL);
  fullscreen_subtitle_display();
  // start insert
  if (is_enable_vbi_on_setting())
  {
    ui_enable_vbi_insert(TRUE);
  }
#endif
#ifndef IMPL_NEW_EPG
  ui_set_epg_dy();
#endif

  g_is_playing = TRUE;
}


void ui_reset_tvmode(u32 av_mode)
{
  cmd_t cmd;

  cmd.id = PB_CMD_RESET_TVMODE;
  cmd.data1 = av_mode;
  cmd.data2 = 0;

  g_pb_info.pg_info.tv_mode = (u16)av_mode;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}


void ui_reset_video_aspect_mode(u32 aspect)
{
  cmd_t cmd;

  cmd.id = PB_CMD_RESET_VIDEO_ASPECT_MODE;
  cmd.data1 = aspect;
  cmd.data2 = 0;

  g_pb_info.pg_info.aspect_ratio = aspect;

  ap_frm_do_command(APP_PLAYBACK, &cmd);
}


void do_lock_pg(u16 pg_id)
{
  dvbs_prog_node_t pg;
  dvbs_tp_node_t tp;
  sat_node_t sat;

  if (db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
    return;
  }

  if (db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
    return;
  }

  if (db_dvbs_get_sat_by_id((u16)tp.sat_id, &sat) != DB_DVBS_OK)
  {
    MT_ASSERT(0);
    return;
  }

  //ui_set_antenna(&sat);
}


#ifndef IMPL_NEW_EPG
static u8 g_parental_rating = 0xff;
static BOOL sg_b_check_agelimit_flag = FALSE;
BOOL get_check_agelimit_flag()
{
  return sg_b_check_agelimit_flag;
}
void ui_age_limit_recheck(u32 para1, u32 para2)
{
  dvbs_prog_node_t pg = {0};
  u32 group_context = 0;
  u16 curn_group = 0;
  u16 curn_mode = 0;
  u16 pg_id = 0;
  u16 svc_id, stream_id;
  u16 net_id;
  u8 parental_rating = 0;
  epg_event_para_t *p_para = NULL;
  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();
  sys_status_get_curn_prog_in_group(curn_group, (u8)curn_mode,
                                    &pg_id, &group_context);
  db_dvbs_get_pg_by_id(pg_id, &pg);
  p_para = (epg_event_para_t *)para1;
  stream_id = p_para->ts_id;
  svc_id = p_para->svc_id;
  net_id = p_para->orig_network_id;

  OS_PRINTF("@@@pg.orig_net_id = %d, net_id = %d\n", pg.orig_net_id, net_id);
  OS_PRINTF("@@@pg.ts_id = %d, stream_id = %d\n", pg.ts_id, stream_id);
  OS_PRINTF("@@@pg.s_id = %d, svc_id = %d\n", pg.s_id, svc_id);

  //current playing program change parent rating
  if((pg.orig_net_id == net_id)
    &&(pg.ts_id == stream_id)
    &&(pg.s_id == svc_id)
    )
  {
    parental_rating = p_para->parental_rating;
    OS_PRINTF("%s: g_parental_rating = %d, parental_rating = %d\n", __FUNCTION__, g_parental_rating, parental_rating);
    if((g_is_playing == TRUE) && (g_parental_rating != parental_rating))// && (g_parental_rating != INVALID_PARENTAL_RATING)
    {
      if(do_chk_age_limit(pg_id))
      {
        g_chkpwd_info.type |= LOCK_TYPE_AGE_LOCK;
        ui_stop_play(STOP_PLAY_BLACK, TRUE);
        play_pg_with_pwd(pg_id);
        sg_b_check_agelimit_flag = TRUE;
      }
    }
  }
}
#endif

BOOL do_chk_age_limit(u16 prog_id)
{
  BOOL is_age_limit = FALSE;
#ifndef IMPL_NEW_EPG
  u16 age_limit;
  u8 parental_rating = 0;
  u8 pro_age = 0;

  sys_status_get_age_limit(&age_limit);

  if(age_limit == AGE_LIMIT_VIEW_ALL)
  {
    OS_PRINTF("\n do_chk_age_limit \n");
    return FALSE;
  }

  //if(age_limit == AGE_LIMIT_LOCK_ALL)
  //{
   // return TRUE;
  //}

  OS_PRINTF("\n age_limit[%d] \n", age_limit);
  parental_rating = ui_get_current_parental_rating();
  if (parental_rating < 0x01 || parental_rating > 0x0F)
  {
    parental_rating = INVALID_PARENTAL_RATING;
  }
  pro_age = parental_rating + 3;
  g_parental_rating = parental_rating;

  OS_PRINTF("do check age limit\n");
  OS_PRINTF("parental_rating[%d]\n", parental_rating);

  if(parental_rating != INVALID_PARENTAL_RATING)
  {
    switch (age_limit)
    {
      case AGE_LIMIT_7:
        if(pro_age >= 7)
        {
          is_age_limit = TRUE;
        }
        break;
      case AGE_LIMIT_12:
        if(pro_age >= 12)
        {
          is_age_limit = TRUE;
        }
        break;
      case AGE_LIMIT_15:
        if(pro_age >= 15)
        {
          is_age_limit = TRUE;
        }
        break;
      case AGE_LIMIT_18:
        if(pro_age >= 18)
        {
          is_age_limit = TRUE;
        }
        break;
      case AGE_LIMIT_LOCK_ALL:
        is_age_limit = TRUE;
        break;
      default:
        break;
    }
  }
#endif
  OS_PRINTF("\n is_age_limit[%d] \n", is_age_limit);

  return is_age_limit;
}

static BOOL do_chk_pg(u16 prog_id)
{
  u8 view_id;
  u16 pos;
  BOOL is_ss_chk;
  BOOL is_age_lock = FALSE;
  BOOL is_chnl_lock = FALSE;

  view_id = ui_dbase_get_pg_view_id();
  pos = db_dvbs_get_view_pos_by_id(view_id, prog_id);

  g_chkpwd_info.type = LOCK_TYPE_NONE;
  g_chkpwd_info.passchk_pg = INVALIDID;

  if (do_chk_age_limit(prog_id))
  {
    if (ui_is_prog_locked(prog_id))
    {
      is_age_lock = TRUE;
    }
  }

  sys_status_get_status(BS_PROG_LOCK, &is_ss_chk);
  if (is_ss_chk)
  {
    if (db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, 0))
    {
      if (ui_is_prog_locked(prog_id))
      {
        is_chnl_lock = TRUE;
      }
    }
  }

  if (is_age_lock || is_chnl_lock)
  {
    if (is_age_lock)
    {
      g_chkpwd_info.type |= LOCK_TYPE_AGE_LOCK;
    }
    if (is_chnl_lock)
    {
      g_chkpwd_info.type |= LOCK_TYPE_CHNL_LOCK;
    }

    if (fw_get_focus_id() != ROOT_ID_BACKGROUND)
    {
      play_pg_with_pwd(prog_id);
    }
    // TODO: fw_post_msg(MSG_CHK_PWD, prog_id, 0);
    return FALSE;
  }

  if (g_chkpwd_info.passchk_pg != prog_id)
  {
    g_chkpwd_info.passchk_pg = prog_id;
  }
  return TRUE;
}

void trans_lock_tp_info(dvbc_lock_info_t *p_lock_info, dvbs_tp_node_t *p_tp)
{
  p_lock_info->tp_freq = p_tp->freq;
  p_lock_info->tp_sym = p_tp->sym;
  p_lock_info->nim_modulate = p_tp->nim_modulate;
}


void trans_sat_info(sat_rcv_para_t *p_sat_rcv, sat_node_t *p_sat)
{
  p_sat_rcv->lnb_high = p_sat->lnb_high;
  p_sat_rcv->lnb_low = p_sat->lnb_low;
  p_sat_rcv->lnb_power = p_sat->lnb_power;
  p_sat_rcv->lnb_type = p_sat->lnb_type;
  p_sat_rcv->k22 = p_sat->k22;
  p_sat_rcv->v12 = p_sat->v12;
  p_sat_rcv->band_freq = p_sat->band_freq;
  p_sat_rcv->user_band = p_sat->user_band;
  p_sat_rcv->tuner_index = p_sat->tuner_index;
}

void trans_tp_info(tp_rcv_para_t *p_tp_rcv, dvbs_tp_node_t *p_tp)
{
  p_tp_rcv->sym = p_tp->sym;
  p_tp_rcv->freq = p_tp->freq;
  p_tp_rcv->nim_modulate = p_tp->nim_modulate;
  p_tp_rcv->nim_type = p_tp->nim_type;
}

void trans_pg_info(dvbs_program_t *p_pg_info, dvbs_prog_node_t *p_pg)
{
  av_set_t av_set = {0};
  u32 cur_audio_ch, new_audio_ch = 0;
  u16 audio_pid = 0;

  u16 audio_type = AUDIO_MP2;

  sys_status_get_av_set(&av_set);
  new_audio_ch = get_audio_channel(p_pg);


  OS_PRINTF("[AUDIO_EAC3] new_audio_ch = %d \n",new_audio_ch);
  if((p_pg->audio_channel != new_audio_ch) && !p_pg->is_audio_channel_modify)
  {
    p_pg->audio_channel = new_audio_ch;
    db_dvbs_edit_program(p_pg);
  }
  cur_audio_ch = p_pg->audio_channel;

  audio_pid = (u16)p_pg->audio[cur_audio_ch].p_id;
  audio_type = (u16)p_pg->audio[cur_audio_ch].type;

  p_pg_info->s_id = (u16)p_pg->s_id;
  p_pg_info->is_scrambled = p_pg->is_scrambled;
  p_pg_info->tv_mode = sys_status_get_sd_mode(av_set.tv_mode);

  p_pg_info->v_pid = (u16)p_pg->video_pid;
  p_pg_info->video_type = (u8)p_pg->video_type;
  p_pg_info->pcr_pid = (u16)p_pg->pcr_pid;
  p_pg_info->a_pid = audio_pid;
  p_pg_info->audio_type = audio_type;
  p_pg_info->audio_track = (u16)p_pg->audio_track;
  p_pg_info->audio_volume = (u16)p_pg->volume;
  p_pg_info->pmt_pid = (u16)p_pg->pmt_pid;
  p_pg_info->aspect_ratio = sys_status_get_video_aspect(av_set.tv_ratio);
  p_pg_info->context1 = p_pg->id;

}

#ifdef DTMB_PROJECT
void trans_dtmb_tp_info(dvbt_lock_info_t *p_lock_info, dvbs_tp_node_t *p_tp)
{
  p_lock_info->tp_freq = p_tp->freq;
  p_lock_info->band_width = p_tp->sym;
  p_lock_info->nim_type = NIM_DTMB;
}
#endif

void load_play_paramter_by_pgid(play_param_t *p_info, u16 pg_id)
{
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  dvbs_tp_node_t tp = {0};
  sat_node_t sat = {0};

  memset(p_info, 0, sizeof(play_param_t));

  ret = db_dvbs_get_pg_by_id(pg_id, &pg);
  MT_ASSERT(DB_DVBS_OK == ret);
  ret = db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
  MT_ASSERT(DB_DVBS_OK == ret);
  ret = db_dvbs_get_sat_by_id((u16)tp.sat_id, &sat);
  MT_ASSERT(DB_DVBS_OK == ret);

#ifdef DTMB_PROJECT
  p_info->lock_mode = SYS_DTMB;
  trans_dtmb_tp_info(&p_info->inner.dvbt_lock_info, &tp);
#else
  p_info->lock_mode = SYS_DVBC;
  trans_lock_tp_info(&p_info->inner.dvbc_lock_info, &tp);
#endif

  trans_pg_info(&p_info->pg_info, &pg);
}

//recall
BOOL ui_recall(BOOL is_play, u16 *p_curn)
{
  u16 view_type;
  u32 context;
  dvbs_prog_node_t last_pg;
  play_info_t *p_last_info = &g_play_cnxt[0];
  u8 view_id = 0;
  u16 total = 0;
  u8 curn_mode;
  u16 curn_group;
  //g_last_info
  // init
  *p_curn = g_play_info.prog_id;
  if (db_dvbs_get_pg_by_id(p_last_info->prog_id, &last_pg) == DB_DVBS_FAILED)
  {
    return FALSE;
  }

  // recreate view for recall
  if ((p_last_info->group != g_play_info.group)
    || (p_last_info->mode != g_play_info.mode))
  {
    curn_mode = sys_status_get_curn_prog_mode();
    curn_group = sys_status_get_curn_group();
    sys_status_set_curn_prog_mode(p_last_info->mode);
    sys_status_set_curn_group(p_last_info->group);
    sys_status_save();

    sys_status_get_curn_view_info(&view_type, &context);
    view_id = ui_dbase_create_view(view_type, context, NULL);
    total = db_dvbs_get_count(view_id);
    if(total != 0)
    {
      ui_dbase_set_pg_view_id(view_id);
    }
    else
    {
      sys_status_set_curn_prog_mode(curn_mode);
      sys_status_set_curn_group(curn_group);
      sys_status_save();
      mtos_printk("now it is not has pg at curn view\n");
      return FALSE;
    }
  }

  *p_curn = p_last_info->prog_id;
  if (is_play)
  {
    return ui_play_prog(*p_curn, FALSE);
  }
  return TRUE;
}

//switch between tv and radio
BOOL ui_tvradio_switch(BOOL is_play, u16 *p_curn)
{
  u16 org_group, pos_in_set;
  u8 curn_mode, org_mode, group_type;
  u8 view_id;
  u16 pg_id,total;
  u32 context;
  u16 view_type;
  dvbs_prog_node_t  p_pg;
  // init
  *p_curn = g_play_info.prog_id;
  org_mode = sys_status_get_curn_prog_mode();
  org_group = sys_status_get_curn_group();

  switch (org_mode)
  {
    case CURN_MODE_TV:
      curn_mode = CURN_MODE_RADIO;
      break;
    case CURN_MODE_RADIO:
      curn_mode = CURN_MODE_TV;
      break;
    default:
      return FALSE;
  }

  sys_status_get_group_info(org_group, &group_type, &pos_in_set, &context);
  switch (group_type)
  {
    case GROUP_T_ALL:
      if (!sys_status_get_group_all_info(curn_mode, &pg_id))
      {
        pg_id = INVALIDID;
      }
      break;
    case GROUP_T_FAV:
      if (!sys_status_get_fav_group_info(pos_in_set, curn_mode, &pg_id))
      {
        pg_id = INVALIDID;
      }
      break;
    case GROUP_T_SAT:
      if (!sys_status_get_sat_group_info(pos_in_set, curn_mode, &pg_id))
      {
        pg_id = INVALIDID;
      }
      break;
    default:
      MT_ASSERT(0);
      return FALSE;
  }

  if(pg_id == INVALIDID)
  {
    return FALSE;
  }

  sys_status_set_curn_prog_mode(curn_mode);
  sys_status_save();
  db_dvbs_get_pg_by_id(pg_id, &p_pg);

  sys_status_get_curn_view_info(&view_type, &context);
  view_id = ui_dbase_create_view(view_type, context, NULL);
  ui_dbase_set_pg_view_id(view_id);
  total = db_dvbs_get_count(view_id);
  if(total == 0)
   {
    switch (curn_mode)
     {
      case CURN_MODE_TV:
        org_mode = CURN_MODE_RADIO;
        break;
      case CURN_MODE_RADIO:
        org_mode = CURN_MODE_TV;
        break;
     default:
       return FALSE;
     }
      sys_status_set_curn_prog_mode(org_mode);
      sys_status_save();
      sys_status_get_curn_view_info(&view_type, &context);
      view_id = ui_dbase_create_view(view_type, context, NULL);
      ui_dbase_set_pg_view_id(view_id);
      return FALSE;
   }
  else if(p_pg.hide_flag)
  {
    mtos_printk("curn pg is hide,so we need choose the first pg\n");
    pg_id = db_dvbs_get_id_by_view_pos(view_id,0);
    sys_status_set_curn_group_info(pg_id,0);
    sys_status_save();    
   }

  //close pause menu.
  ui_set_pause(FALSE);
  *p_curn = pg_id;
  if (is_play)
  {
    return ui_play_prog(*p_curn, FALSE);
  }

  return TRUE;
}


 #ifdef SMSX_CA  
  BOOL ui_dvbc_change_view(dvbs_view_t view_type, BOOL is_play)
  {
	  u16 org_group, pos_in_set;
	  u8 curn_mode, org_mode, group_type;
	  u8 view_id;
	  u16 pg_id = INVALIDID;
	  u16 pg_pos = INVALIDPOS;
	  u32 context;
	  BOOL ret = FALSE;
	  dvbs_prog_node_t prog = {0};
  
	  // init
	  org_mode = sys_status_get_curn_prog_mode();
	  org_group = sys_status_get_curn_group();
  
	  if(org_group != GROUP_T_ALL)
	  {
	  MT_ASSERT(0);
	  }
  
	  switch(view_type)
	  {
	  case DB_DVBS_ALL_TV:
		curn_mode = CURN_MODE_TV;
		if(org_mode == CURN_MODE_TV)
		{
		  //return TRUE;
		}
		break;
  
	  case DB_DVBS_ALL_RADIO:
		curn_mode = CURN_MODE_RADIO;
		if(org_mode == CURN_MODE_RADIO)
		{
		  //return TRUE;
		}
		break;
  
	  case DB_DVBS_ALL_PG:
	  case DB_DVBS_ALL_PG_IGNORE_SKIP_FLAG:
	  case DB_DVBS_TV_RADIO:
	  case DB_DVBS_TV_RADIO_IGNORE_SKIP_FLAG:
		curn_mode = org_mode;
		break;
  
	  case DB_DVBS_FAV_ALL:
		curn_mode = org_mode;
		break;
  
	  default:
		curn_mode = org_mode;
		break;
	  }
  
	  sys_status_get_group_info(org_group, &group_type, &pos_in_set, &context);
  
	  switch (group_type)
	  {
	  case GROUP_T_ALL:
		if (!sys_status_get_group_all_info(curn_mode, &pg_id))
		{
		  pg_id = INVALIDID, pg_pos = INVALIDPOS;
		}
		break;
  
	  default:
		MT_ASSERT(0);
		return FALSE;
	  }
  
	  if (pg_id == INVALIDID || pg_pos == INVALIDPOS)
	  {
	  view_id = ui_dbase_create_view(view_type, 0, NULL);
	  ui_dbase_set_pg_view_id(view_id);
  
	  if(db_dvbs_get_count(view_id)>0)
	  {
		pg_id = db_dvbs_get_id_by_view_pos(view_id, 0);
		pg_pos = 0;
		
		db_dvbs_get_pg_by_id(pg_id, &prog);
		
		if((BOOL)prog.tv_flag)
		{
		  curn_mode = CURN_MODE_TV;
		}
		else
		{
		  curn_mode = CURN_MODE_RADIO;
		}
  
		sys_status_set_curn_prog_mode(curn_mode);
		sys_status_set_curn_group_info(pg_id, pg_pos);
		//sys_status_save();
  
		if(is_play)
		{
		  ui_play_prog(pg_id, FALSE);
		}
  
		ret = TRUE;
	  }
	  else
	  {
		return ret;
	  }
	  }
	  else
	  {
	  view_id = ui_dbase_create_view(view_type, 0, NULL);
	  ui_dbase_set_pg_view_id(view_id);
  
	  //set sys info
	  pg_pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);
  
	  if(pg_id == INVALIDID || pg_pos == INVALIDPOS) 
	  {
		if(db_dvbs_get_count(view_id)>0)
		{
		  pg_id = db_dvbs_get_id_by_view_pos(view_id, 0);
		  pg_pos = 0;
		  
		  db_dvbs_get_pg_by_id(pg_id, &prog);
		  
		  if((BOOL)prog.tv_flag)
		  {
			curn_mode = CURN_MODE_TV;
		  }
		  else
		  {
			curn_mode = CURN_MODE_RADIO;
		  }
		}
		else
		{
		  return ret;
		}
	  }
  
	  sys_status_set_curn_prog_mode(curn_mode);
	  sys_status_set_curn_group_info(pg_id, pg_pos);
	  //sys_status_save();
	  view_id = ui_dbase_create_view(view_type, context, NULL);
	  ui_dbase_set_pg_view_id(view_id);
  
	  if(is_play)
	  {
		ui_play_prog(pg_id, is_play);
	  }
  
	  ret = TRUE;
	  }  
  
	  //if((view_type != DB_DVBS_ALL_PG) && (view_type != DB_DVBS_ALL_PG))
	  //{
	  //  sys_status_save();
	  //}
  
	  return ret;
  }
#endif



void ui_shift_prog(s16 offset, BOOL is_play, u16 *p_curn)
{
  play_set_t play_set;
  dvbs_prog_node_t pg;
  u16 prog_id, prev_prog, total_prog;
  u16 curn_prog;
  s32 dividend = 0;
  u8 curn_view = ui_dbase_get_pg_view_id();
	BOOL is_force_skip = FALSE;

  // init
  *p_curn = g_play_info.prog_id;

  sys_status_get_play_set(&play_set);

  total_prog = db_dvbs_get_count(curn_view);
  prog_id = sys_status_get_curn_group_curn_prog_id();
  curn_prog = prev_prog = db_dvbs_get_view_pos_by_id(
                curn_view, prog_id);

  do
  {
    dividend = curn_prog + offset;

 #if 0 //fix bug 15865
    while(dividend < 0)
    {
      dividend += total_prog;
    }
    curn_prog = (u16)(dividend) % total_prog;
  #else
    if(dividend >= total_prog)
    {
      if(curn_prog == (total_prog - 1))
      {
        curn_prog = 0;
      }
      else
      {
        curn_prog = total_prog - 1;
      }
    }
    else
    {
      if(dividend <= 0)
      {
        if(curn_prog == 0)
        {
          curn_prog = total_prog - 1;
        }
        else
        {
          curn_prog = 0;
        }
      }
      else
      {
        curn_prog = (u16)(dividend) % total_prog;
      }
    }
  #endif

    /* pos 2 id */
    prog_id = db_dvbs_get_id_by_view_pos(curn_view, curn_prog);
    MT_ASSERT(prog_id != INVALIDID);
    if (prev_prog == curn_prog) /* all is skip*/
    {
      return;
    }

    // check play type
    db_dvbs_get_pg_by_id(prog_id, &pg);
		OS_PRINTF("pg is scramble = %d, play type = %d\n",
			pg.is_scrambled, play_set.type);

    switch(play_set.type)
    {
      case 1: // only play free pg
      is_force_skip = (BOOL)(pg.is_scrambled == 1);
      break;

      case 2: // only play scramble pg
      is_force_skip = (BOOL)(pg.is_scrambled == 0);
      break;

      default:
        ;
    }
  } while (is_force_skip || db_dvbs_get_mark_status(curn_view, curn_prog,
                                   DB_DVBS_MARK_SKP, FALSE));

  *p_curn = prog_id;
	sys_status_set_curn_group_info(prog_id, curn_prog);
  if(is_play)
  {
    ui_play_prog(*p_curn, FALSE);
  }
}


void ui_set_frontpanel_by_curn_pg(void)
{
  u16 pos;

  if (g_play_info.prog_id != INVALIDID)
  {
    pos = db_dvbs_get_view_pos_by_id(
      ui_dbase_get_pg_view_id(), g_play_info.prog_id);
    ui_set_front_panel_by_num(pos + 1);
  }
  else
  {
    ui_set_front_panel_by_str("----");
  }
}

static RET_CODE on_pwdlg_correct_in_list(control_t *p_ctrl, u16 msg,
                                  u32 para1, u32 para2)
{
  /* if get correct pwd then don't check again in list
     and enable it when leave list */
  control_t *p_list;
  control_t *p_cont, *p_list_cont = NULL;
  u32 pos = 0;
  ui_enable_chk_pwd(FALSE);
  if (IS_MASKED(g_chkpwd_info.type, LOCK_TYPE_CHNL_LOCK))
  {
    g_chkpwd_info.type &= ~LOCK_TYPE_CHNL_LOCK;
  }
  if (IS_MASKED(g_chkpwd_info.type, LOCK_TYPE_AGE_LOCK))
  {
    g_chkpwd_info.type &= ~LOCK_TYPE_AGE_LOCK;
  }
  g_chkpwd_info.passchk_pg = g_play_info.prog_id;
  /* to play */
  do_play_pg(g_chkpwd_info.passchk_pg);
  ui_comm_pwdlg_close();
  update_signal();
  if((p_cont = fw_find_root_by_id(ROOT_ID_SMALL_LIST)) != NULL)
  {
    p_list_cont = ui_comm_root_get_ctrl(ROOT_ID_SMALL_LIST, IDC_SLIST_CONT);
    if(p_list_cont)
    {
        p_list = ctrl_get_child_by_id(p_list_cont, IDC_SMALL_LIST_LIST);
        if(p_list)
        {
            pos = list_get_focus_pos(p_list);
            list_set_focus_pos(p_list, pos);     
            ctrl_paint_ctrl(p_list->p_parent, TRUE);
        }
    }
  }
  ui_set_have_pass_pwd(TRUE);
  return SUCCESS;
}

static RET_CODE on_pwdlg_correct_in_scrn(control_t *p_ctrl, u16 msg,
                                  u32 para1, u32 para2)
{
  control_t *p_cont = NULL, *p_list = NULL, *p_list_cont = NULL;
  u32 pos = 0;
  
  if (IS_MASKED(g_chkpwd_info.type, LOCK_TYPE_CHNL_LOCK))
  {
    g_chkpwd_info.type &= ~LOCK_TYPE_CHNL_LOCK;
  }
  if (IS_MASKED(g_chkpwd_info.type, LOCK_TYPE_AGE_LOCK))
  {
    g_chkpwd_info.type &= ~LOCK_TYPE_AGE_LOCK;
  }
  g_chkpwd_info.passchk_pg = g_play_info.prog_id;

  /* to play */
  do_play_pg(g_chkpwd_info.passchk_pg);

  ui_comm_pwdlg_close();
  update_signal();
  ui_set_have_pass_pwd(TRUE);

  if((p_cont = fw_find_root_by_id(ROOT_ID_SMALL_LIST)) != NULL)
  {
    p_list_cont = ui_comm_root_get_ctrl(ROOT_ID_SMALL_LIST, IDC_SLIST_CONT);
    if(p_list_cont)
    {
        p_list = ctrl_get_child_by_id(p_list_cont, IDC_SMALL_LIST_LIST);
        if(p_list)
        {
            pos = list_get_focus_pos(p_list);
            list_set_focus_pos(p_list, pos);     
            ctrl_paint_ctrl(p_list->p_parent, TRUE);
        }
    }
  }

  
  return SUCCESS;
}


static RET_CODE on_pwdlg_exit(control_t *p_ctrl, u16 msg,
                                  u32 para1, u32 para2)
{
  control_t *p_list;
  control_t *p_cont;

  /* only lock */
  do_lock_pg(g_play_info.prog_id);
  ui_comm_pwdlg_close();
  update_signal();
  if((p_cont = fw_find_root_by_id(ROOT_ID_PROG_LIST)) != NULL)
  {
    p_list = ctrl_get_child_by_id(ui_comm_root_get_ctrl(ROOT_ID_PROG_LIST, IDC_PLIST_LIST_CONT), IDC_PLCONT_LIST);
    ctrl_paint_ctrl(p_list->p_parent, TRUE);
  }
  return SUCCESS;
}

static RET_CODE on_pwdlg_save(control_t *p_ctrl, u16 msg,
                                  u32 para1, u32 para2)
{
  /* only lock */
  do_lock_pg(g_play_info.prog_id);
  update_signal();
  return SUCCESS;
}
static RET_CODE on_pwdlg_hot_key(control_t *p_pwdlg, u16 msg,
															u32 para1, u32 para2)
{
	control_t *p_ctrl;
	if(MSG_SELECT == msg)
	{
		if(NULL==fw_find_root_by_id(ROOT_ID_SMALL_LIST))
		{			
			ui_comm_pwdlg_close();
			manage_notify_root(ROOT_ID_BACKGROUND, MSG_OPEN_MENU_IN_TAB | ROOT_ID_SMALL_LIST, V_KEY_OK, 0);
		}
	}
	else if(MSG_EXTEND_MENU == msg)
	{
		ui_comm_pwdlg_close();
		if(NULL==fw_find_root_by_id(ROOT_ID_SMALL_LIST))
		{
			manage_notify_root(ROOT_ID_BACKGROUND, MSG_OPEN_MENU_IN_TAB |ROOT_ID_XEXTEND, 0, 0);
		}
		else
		{
			p_ctrl = get_small_list_ctrl();
			ctrl_process_msg(p_ctrl, MSG_EXTEND_MENU, 0, 0);
		}
	}
	return SUCCESS;
}

static void pass_key_to_parent(u16 msg)
{
  u16 key;

  // pass the key to parent
  switch(msg)
  {
    case MSG_FOCUS_UP:
      key = V_KEY_UP;
      break;
    case MSG_FOCUS_DOWN:
      key = V_KEY_DOWN;
      break;
    case MSG_FOCUS_LEFT:
      key = V_KEY_LEFT;
      break;
    case MSG_FOCUS_RIGHT:
      key = V_KEY_RIGHT;
      break;
    case MSG_PAGE_UP:
      key = V_KEY_PAGE_UP;
      break;
    case MSG_PAGE_DOWN:
      key = V_KEY_PAGE_DOWN;
      break;
    default:
      key = V_KEY_INVALID;
  }
  fw_notify_parent(ROOT_ID_PASSWORD, NOTIFY_T_KEY, FALSE, key, 0, 0);
}

static RET_CODE on_pwdlg_cancel_in_list(control_t *p_ctrl, u16 msg,
                                  u32 para1, u32 para2)
{
  /* only lock */
//  do_lock_pg(g_chkpwd_info.passchk_pg);

  pass_key_to_parent(msg);
  ui_comm_pwdlg_close();
  update_signal();

  return SUCCESS;
}

static RET_CODE on_pwdlg_cancel_in_scrn(control_t *p_ctrl, u16 msg,
                                  u32 para1, u32 para2)
{
  /* only lock */
//  do_lock_pg(g_chkpwd_info.passchk_pg);

  pass_key_to_parent(msg);
  ui_comm_pwdlg_close();
  update_signal();

  return SUCCESS;
}


//play current channel in current mode
BOOL ui_play_curn_pg(void)
{
  u16 curn_group;
  u16 prog_id;
  u8 curn_mode,view_id;
  u16 view_type;
  u32 context,total_prog;

  curn_mode = sys_status_get_curn_prog_mode();
  curn_group = sys_status_get_curn_group();

  if (curn_mode == CURN_MODE_NONE)
  {
    sys_status_check_group();
    curn_mode = sys_status_get_curn_prog_mode();
    if (curn_mode == CURN_MODE_NONE)
    {
      //ui_stop_play(STOP_PLAY_BLACK, TRUE);
      ui_set_front_panel_by_str("----");
      return FALSE;
    }
  }

  sys_status_get_view_info(curn_group, curn_mode, &view_type, &context);
  view_id = ui_dbase_create_view(view_type, context, NULL);
  total_prog = db_dvbs_get_count(view_id);

  if(total_prog == 0)
  {
    ui_set_front_panel_by_str("----");
    return SUCCESS;
  }

//  ui_play_prog(ui_sys_get_curn_prog(view), FALSE);
  sys_status_get_curn_prog_in_group(curn_group, curn_mode, &prog_id, &context);

  if(ui_get_play_prg_type() == SAT_PRG_TYPE)
    ui_play_prog(prog_id, FALSE);
  return TRUE;
}

BOOL ui_is_prog_encrypt(void)
{
  return g_is_encrypt;
}


static BOOL ui_fast_play(u16 pg_id, stop_mode_t stop_mode)
{
  cmd_t cmd;
  language_set_t lang_set;
  av_set_t av_set = {0};
  aud_dolby_status_vsb_t dolby_status = AUDIO_DOLBY_NOT_SUPPORTED_BOTH;
  struct audio_device *p_dev = NULL;
#if ENABLE_TTX_SUBTITLE
  char **p_lang_code = sys_status_get_lang_code(FALSE);
#endif

  // check pause status
  if(ui_is_pause())
  {
    ui_set_pause(FALSE);
  }

  // show scramble mark or not
  update_signal();

  load_play_paramter_by_pgid(&g_pb_info, pg_id);

#if ENABLE_GLOBAL_VOLUME
  g_pb_info.pg_info.audio_volume = (u16)sys_status_get_global_volume();
#endif

  //if set DIGITAL_OUTPUT as BS out,  change audio type
  sys_status_get_av_set(&av_set);

  // 1 means BS out
    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
    aud_get_dolby_status(p_dev, &dolby_status);
    OS_PRINTF("dolby_status %d, audio_type = %d\n", dolby_status,g_pb_info.pg_info.audio_type);
  if((1 == av_set.digital_audio_output)
    && ((AUDIO_AC3_VSB == g_pb_info.pg_info.audio_type)
    ||(AUDIO_EAC3 == g_pb_info.pg_info.audio_type)))
  {
    g_pb_info.pg_info.audio_type = AUDIO_EAC3;
  }

  // to play
  cmd.id = PB_CMD_SWITCH_PG;
  cmd.data1 = (u32) & g_pb_info;
  cmd.data2 = stop_mode;
  ap_frm_do_command(APP_PLAYBACK, &cmd);
#ifdef IMPL_NEW_EPG
  ui_epg_set_program_info(pg_id);
#endif

  sys_status_get_lang_set(&lang_set);


#if ENABLE_TTX_SUBTITLE
  // set teletext language
  ui_enable_ttx(TRUE);
  ui_set_ttx_language(p_lang_code[lang_set.tel_text]);
  // subt display on
  ui_enable_subt(TRUE);
  ui_set_subt_service(lang_set.sub_title, SUBT_TYPE_NORMAL);
  fullscreen_subtitle_display();
  // start insert
  if(is_enable_vbi_on_setting())
  {
    ui_enable_vbi_insert(TRUE);
  }
#endif

#ifndef IMPL_NEW_EPG
  ui_set_epg_dy();
#endif
  return TRUE;
}

extern void ui_check_satip_client_is_exist_while_change_pg(u16 curr_pg_id);
BOOL ui_play_prog(u16 prog_id, BOOL is_force)
{
  dvbs_prog_node_t pg;
  BOOL is_switch = FALSE;
  u8 curn_mode,old_mode;
  u16 prog_pos;
  play_set_t play_set;
  BOOL is_playing = ui_is_playing();
  u8 i;
  BOOL b_find = FALSE;
  BOOL is_sync = FALSE, is_show_logo = FALSE;
  stop_mode_t stop_mode = STOP_PLAY_NONE;
  u16  group;

  if (prog_id >= INVALIDID)
  {
    UI_PRINTF("AP PLAY: INVALID pg = 0x%.4x\n", prog_id);
    return FALSE;
  }

  if(ui_get_play_prg_type() == NET_PRG_TYPE)
  {
    UI_PRINTF("AP PLAY FAIL: now is net prg, not this play\n");
    return FALSE;
  }

  group = sys_status_get_curn_group();

  // check for play
  if (g_play_info.prog_id == prog_id
      && !is_force
      && ui_is_playing()
      && !ui_is_prog_locked(prog_id))
  {
    DEBUG(MAIN, NOTICE, "AP PLAY: ERR same pg = 0x%.4x, group = %d\n", prog_id, group);
    if (g_play_info.group != group)
    {
      g_play_info.group = group;
    }
    #if ENABLE_TTX_SUBTITLE
    fullscreen_subtitle_display();
    #endif
    return FALSE;
  }

//  clean_pg_scramble();

  if (db_dvbs_get_pg_by_id(prog_id, &pg) != DB_DVBS_OK)
  {
    UI_PRINTF("AP PLAY: can not find pg = 0x%.4x\n", prog_id);
    return FALSE;
  }

  if(pg.hide_flag)
  {
    if(fw_find_root_by_id(ROOT_ID_PROG_LIST) == NULL)
    {
      mtos_printk("AP PLAY: can not play pg = 0x%.4x,becaus it is hide pg\n", prog_id);
      return FALSE;
    }
      mtos_printk("AP PLAY: can play pg = 0x%.4x,becaus it is hide pg at prog_list\n", prog_id);
  }
  // set sys info
  curn_mode = sys_status_get_curn_prog_mode();
  old_mode = g_play_mode;
  OS_PRINTF("\n g_play_mode[%d] curn_mode[%d]\n", g_play_mode, curn_mode);
  if (curn_mode != g_play_mode)
  {
    is_switch = TRUE;
    g_play_mode = curn_mode;
  }


  // just stop before play
  sys_status_get_play_set(&play_set);
  if (is_playing)
  {
    if(is_switch)
    {
      if (g_play_mode == CURN_MODE_RADIO)
      {
        stop_mode = STOP_PLAY_BLACK;
        is_sync = TRUE;
      }
      else
      {
        stop_mode = play_set.mode ? STOP_PLAY_BLACK : STOP_PLAY_FREEZE;
        is_sync = TRUE;
      }
    //  g_play_mode = curn_mode;
    }
   else
    {
      if(curn_mode == CURN_MODE_RADIO)
      {
        stop_mode = STOP_PLAY_NONE;
        is_sync = FALSE;
      }
      else
      {
        stop_mode = play_set.mode ? STOP_PLAY_BLACK : STOP_PLAY_FREEZE;
        is_sync = FALSE;
      }
    }
  }
  else
  {
    stop_mode = play_set.mode ? STOP_PLAY_BLACK : STOP_PLAY_FREEZE;
    is_sync = TRUE;
  }

  //disable for temp ,waiting for Rick upadte  //fixed
  if (curn_mode == CURN_MODE_RADIO)
  {
    if(is_switch
      || !is_playing)
    {
      is_show_logo = TRUE;
    }
  }

  // set play info
  if (g_play_info.prog_id != prog_id)
  {
    //memcpy(&g_last_info, &g_play_info, sizeof(play_info_t));
    dvbs_prog_node_t old_pg;
    if ((g_play_info.prog_id != INVALIDID) && g_add_to_history
      && (db_dvbs_get_pg_by_id(g_play_info.prog_id, &old_pg) == DB_DVBS_OK)) //add to history
    {
      if (g_play_cnxt_count > 0)
      {
        for (i = 0; i < g_play_cnxt_count; i++)
        {
          if (g_play_cnxt[i].prog_id == g_play_info.prog_id)
          {
            b_find = TRUE;
            break;
          }
        }
        if (i > 0)
        {
          i = (i < MAX_PLAY_CNXT_COUNT) ? i : (MAX_PLAY_CNXT_COUNT - 1);
          memmove((void *) &g_play_cnxt[1], (void *) &g_play_cnxt[0], sizeof(play_info_t) * i);
        }
      }
      memcpy((void *) &g_play_cnxt[0], (void *) &g_play_info, sizeof(play_info_t));
      if (!b_find)
      {
        g_play_cnxt_count++;
        if (g_play_cnxt_count > MAX_PLAY_CNXT_COUNT)
        {
          g_play_cnxt_count = MAX_PLAY_CNXT_COUNT;
        }
      }
    }
  }

  // update scrambe info
  g_play_info.is_scramble = (u8)pg.is_scrambled;
  g_is_encrypt = (BOOL)pg.is_scrambled;
  OS_PRINTF("[%s] %d g_is_encrypt = %d\n",__FUNCTION__,__LINE__,g_is_encrypt);

  // set play info
  g_play_info.group = group;
  g_play_info.mode = curn_mode;
  g_play_info.prog_id = prog_id;
  g_add_to_history = TRUE;

  //set sys info
  prog_pos = db_dvbs_get_view_pos_by_id(ui_dbase_get_pg_view_id(), g_play_info.prog_id);
  sys_status_set_curn_group_info(g_play_info.prog_id,prog_pos);
  sys_status_save();

  // set frontpanel
  ui_set_frontpanel_by_curn_pg();

  // try to close pwd dlg
  ui_comm_pwdlg_close();

  if(is_show_logo)
  {
    OS_PRINTF("show logo, normal play!!!\n");
    ui_stop_play(stop_mode, is_sync);

    if(is_show_logo)
    {
      ui_show_logo(LOGO_BLOCK_ID_M1);
    }

    if(do_chk_pg(g_play_info.prog_id))
    {
      do_play_pg(g_play_info.prog_id);
    }
  }
  else
  {
    if(do_chk_pg(g_play_info.prog_id))
    {
      OS_PRINTF("default, fast play!!!\n");
#ifdef ENABLE_CA
  	  cas_manager_stop_ca_play();
#endif
      if(old_mode == CURN_MODE_RADIO)  //fixed bug 32554
      {
       ui_stop_play(stop_mode, is_sync);
      }
      
	  DEBUG(CAS, INFO, "is_scramble:%d\n",g_play_info.is_scramble);
      switch(CAS_ID)
      {
 		case CONFIG_CAS_ID_ADT_MG:
 		case CONFIG_CAS_ID_DS:
			ui_fast_play(g_play_info.prog_id, stop_mode);
			g_is_playing = TRUE;
			if(!ui_is_playpg_scrambled())
 				ctrl_process_msg(fw_find_root_by_id(ROOT_ID_BACKGROUND), MSG_CA_EVT_NOTIFY, 0, CAS_S_CLEAR_DISPLAY);
 			break;
 
 		case CONFIG_CAS_ID_DVBCA:
 		case CONFIG_CAS_ID_TR:
			ui_fast_play(g_play_info.prog_id, stop_mode);
			g_is_playing = TRUE;
 			DEBUG(CAS, INFO, "update_signal\n");
 			update_signal();	
 			break;
 
 		case CONFIG_CAS_ID_DIVI:
		#ifdef SHUSHITONG_CA
			if(ui_is_need_clear_display())
				update_ca_message(RSC_INVALID_ID);
			else
				update_signal();
		
			if(CUSTOMER_DTMB_CHANGSHA_HHT == CUSTOMER_ID)
			{
				dvbs_tp_node_t tp = {0};
				db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);

				if(freq_is_valid(tp.freq))
				{
					DEBUG(CAS, INFO, "ui_fast_play\n");
					ui_fast_play(g_play_info.prog_id, stop_mode);
					g_is_playing = TRUE;
				}
				else 
				{
					if(get_have_valid_freq_flag())
					{
						if(get_program_entitlement_from_ca())
						{
							DEBUG(CAS, INFO, "ui_fast_play\n");
							
							if(get_message_strid() == IDS_HHT_TURN_ENC_PROG)
								update_ca_message(RSC_INVALID_ID);
							ui_fast_play(g_play_info.prog_id, stop_mode);
							g_is_playing = TRUE;
						}
						else
						{
							DEBUG(CAS, INFO, "ui_stop_play\n");
							ui_stop_play(stop_mode, is_sync);
							g_is_playing = FALSE;
							if(ui_is_smart_card_insert())
								update_ca_message(IDS_HHT_ENTITLE_EXPIRED);
							else
								update_ca_message(IDS_CA_CARD_REMOVE);
							break;
						}
					}
					else
					{
						DEBUG(CAS, INFO, "ui_stop_play\n");
						ui_stop_play(stop_mode, is_sync);
						g_is_playing = FALSE;
						update_ca_message(IDS_HHT_TURN_ENC_PROG);
						break;
					}
				}
			}
			else
			{
				DEBUG(CAS, INFO, "ui_fast_play\n");
				ui_fast_play(g_play_info.prog_id, stop_mode);
				g_is_playing = TRUE;
			}
 			
 			DEBUG(CAS, INFO, "update_signal:%d ticks = %d\n",ui_is_need_clear_display(),mtos_ticks_get());
 		#endif
 			break;
 
 		default:
			ui_fast_play(g_play_info.prog_id, stop_mode);
			g_is_playing = TRUE;
 		    DEBUG(CAS, INFO, "update_ca_message:RSC_INVALID_ID%d\n", RSC_INVALID_ID);
			if(!ui_is_playpg_scrambled())
         		update_ca_message(RSC_INVALID_ID);
 			break;
 	 }
    }
    else
    {
      OS_PRINTF("pwd check, normal play!!!\n");

      ui_stop_play(stop_mode, is_sync);
	  g_is_playing = FALSE;
      update_signal();
    }
  }
#if 0
  if(g_play_mode == CURN_MODE_RADIO)
  {
    update_encrypt_message_radio();
  }
#endif
  // set flag
  return TRUE;
}

void check_play_history()
{
  play_info_t play_cnxt[MAX_PLAY_CNXT_COUNT];
  //dvbs_prog_node_t pg;
  u16 curn_group;
  u8  curn_mode;
  u8 view_id;
  u8 i, j;

  for (i = 0; i < MAX_PLAY_CNXT_COUNT; i++)
  {
      play_cnxt[i].group = 0;
      play_cnxt[i].prog_id = INVALIDID;
      play_cnxt[i].mode = CURN_MODE_NONE;
      play_cnxt[i].is_scramble = FALSE;
  }

  curn_group = sys_status_get_curn_group();
  curn_mode = sys_status_get_curn_prog_mode();
  view_id = ui_dbase_get_pg_view_id();

  for (i = 0, j = 0; i < g_play_cnxt_count; i++)
  {
    if (g_play_cnxt[i].group != curn_group
      || g_play_cnxt[i].mode != curn_mode
      || db_dvbs_get_view_pos_by_id(view_id, g_play_cnxt[i].prog_id) != INVALIDID)
    //if (db_dvbs_get_pg_by_id(g_play_cnxt[i].prog_id, &pg) == DB_DVBS_OK)
    {
      play_cnxt[j] = g_play_cnxt[i];
      j++;
    }
  }

  if (g_play_info.group == curn_group
    && g_play_info.mode == curn_mode
    && db_dvbs_get_view_pos_by_id(view_id, g_play_info.prog_id) == INVALIDID)
  {
    g_add_to_history = FALSE;
  }

  memcpy((void *) g_play_cnxt, (void *) play_cnxt, sizeof(g_play_cnxt));
  g_play_cnxt_count = j;
}

//clear last prog(for recall) and passwordchecked prog(for password dialog)
void ui_clear_play_history(void)
{
  u8 i;
  for (i = 0; i < g_play_cnxt_count; i++)
  {
    g_play_cnxt[i].group = 0;//g_last_info
    g_play_cnxt[i].mode = CURN_MODE_NONE;
    g_play_cnxt[i].prog_id = INVALIDID;
  }
  g_play_cnxt_count = 0;

  g_chkpwd_info.is_chkpwd = TRUE;
  g_chkpwd_info.passchk_pg = INVALIDID;
}

void ui_reset_chkpwd(void)
{
  g_chkpwd_info.passchk_pg = INVALIDID;
}

void ui_enable_chk_pwd(BOOL is_enable)
{
  g_chkpwd_info.is_chkpwd = is_enable;
}

BOOL ui_is_prog_locked(u16 prog_id)
{
  if (g_chkpwd_info.is_chkpwd == FALSE)
  {
    return FALSE;
  }

  if (g_chkpwd_info.passchk_pg == prog_id)
  {
    return FALSE;
  }

  return TRUE;
}

u8 ui_get_lock_type(void)
{
  return g_chkpwd_info.type;
}

u32 ui_get_cur_prg_tuner_id(void)
{
   return  0;
}
BOOL ui_is_tvradio_switch(u16 pgid)
{
  dvbs_prog_node_t pg;
  u8 curn_mode = sys_status_get_curn_prog_mode();

  db_dvbs_get_pg_by_id(pgid, &pg);

  if(((curn_mode == CURN_MODE_TV) && (pg.video_pid == 0))
    ||((curn_mode == CURN_MODE_RADIO)  && (pg.video_pid != 0)))
  {
    return TRUE;
  }

  return FALSE;
}

BOOL ui_is_playpg_scrambled(void)
{
  return g_play_info.is_scramble;
}

BOOL ui_get_curpg_scramble_flag(void)
{
  dvbs_prog_node_t pg;
  if(g_play_info.prog_id != INVALIDID)
  {
    db_dvbs_get_pg_by_id(g_play_info.prog_id, &pg);
    return pg.is_scrambled;
  }
  return FALSE;
}


void ui_set_playpg_scrambled(BOOL is_scramble)
{
  if (g_play_info.prog_id != INVALIDID)
  {
    g_play_info.is_scramble = is_scramble;
  }
}

void ui_book_play(u16 pgid)
{
  u16 prog_id;
  u8 mode, view_id;
  BOOL ret;

  if(sys_status_get_curn_group_curn_prog_id() != pgid)
  {
    //need tv/radio switch?
    if(ui_is_tvradio_switch(pgid))
    {
      ret = ui_tvradio_switch(FALSE, &prog_id);
      MT_ASSERT(ret == TRUE);
    }

    //need change group? if current group isn't GROUP_ALL, change group.
    if(sys_status_get_curn_group() != 0)
    {
      //set the group as group all.
      sys_status_set_curn_group(0);

      //create new view.
      mode = sys_status_get_curn_prog_mode();
      view_id = ui_dbase_create_view(
        (mode == CURN_MODE_TV)?DB_DVBS_ALL_HIDE_TV:DB_DVBS_ALL_HIDE_RADIO, 0, NULL);

      ui_dbase_set_pg_view_id(view_id);
    }
  }

  ui_play_prog(pgid, FALSE);
}

RET_CODE switch_to_play_prog(void)
{
	u8 focus_root_id;
    focus_root_id = fw_get_focus_id();
	if(PS_PLAY==get_menu_state_by_id(focus_root_id))
    {
    	ui_play_curn_pg();
    	DEBUG(MAIN, INFO, "play curn pg. focus_root_id:%d\n",focus_root_id);
    	return SUCCESS;
    }
    DEBUG(MAIN, NOTICE, "The focus menu state is not PS_PLAY. focus_root_id:%d\n", focus_root_id);
    return ERR_FAILURE;
}

play_info_t ui_get_curplayinfo(void)
{
  return g_play_info;
}

void ui_get_play_cnxt_copy(void *pbuf)
{
  if (pbuf)
  {
    memcpy(pbuf, g_play_cnxt, sizeof(g_play_cnxt));
  }
}

u8 ui_get_play_cnxt_count()
{
  return g_play_cnxt_count;
}

void ui_init_play_timer(u16 owner, u16 notify, u32 tmout)
{
  g_play_timer_state = 0;
  g_play_timer_owner = owner;
  g_play_timer_notify = notify;
  g_play_timer_tmout = tmout;
}

void ui_deinit_play_timer(void)
{
  if(g_play_timer_state == 1)
  {
    fw_tmr_destroy(g_play_timer_owner, g_play_timer_notify);
	g_play_timer_state = 0;
  }
}

void ui_play_timer_start(void)
{
  if(g_play_timer_state == 0)
  {
    fw_tmr_create(g_play_timer_owner, g_play_timer_notify, g_play_timer_tmout, FALSE);
    g_play_timer_state = 1;
  }
  else
  {
    fw_tmr_reset(g_play_timer_owner, g_play_timer_notify, g_play_timer_tmout);
  }
}

void ui_play_timer_set_state(u8 state)
{
  g_play_timer_state = state;
}

void ui_play_set_prog_ca_info(void)
{
#ifdef ENABLE_CA
	OS_PRINTF("[debug]%s, %d call in\n", __FUNCTION__, __LINE__);
	if(ui_is_playing())
	{
		cas_manager_set_sid(g_play_info.prog_id);
		cas_manager_dvb_monitor_start(g_play_info.prog_id);
		cas_manager_set_pvr_forbid(FALSE);
	}
#endif
}


BEGIN_MSGPROC(pwdlg_proc_in_scrn, cont_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_pwdlg_cancel_in_scrn)
  ON_COMMAND(MSG_FOCUS_DOWN, on_pwdlg_cancel_in_scrn)
  ON_COMMAND(MSG_PAGE_UP, on_pwdlg_cancel_in_scrn)
  ON_COMMAND(MSG_PAGE_DOWN, on_pwdlg_cancel_in_scrn)
  ON_COMMAND(MSG_CORRECT_PWD, on_pwdlg_correct_in_scrn)
  ON_COMMAND(MSG_EXIT, on_pwdlg_exit)
  ON_COMMAND(MSG_SAVE, on_pwdlg_save)
  ON_COMMAND(MSG_SELECT, on_pwdlg_hot_key)
  ON_COMMAND(MSG_EXTEND_MENU, on_pwdlg_hot_key)
END_MSGPROC(pwdlg_proc_in_scrn, cont_class_proc)

BEGIN_MSGPROC(pwdlg_proc_in_list, cont_class_proc)
ON_COMMAND(MSG_FOCUS_UP, on_pwdlg_cancel_in_list)
ON_COMMAND(MSG_FOCUS_DOWN, on_pwdlg_cancel_in_list)
ON_COMMAND(MSG_PAGE_UP, on_pwdlg_cancel_in_list)
ON_COMMAND(MSG_PAGE_DOWN, on_pwdlg_cancel_in_list)
ON_COMMAND(MSG_FOCUS_LEFT, on_pwdlg_cancel_in_list)
ON_COMMAND(MSG_FOCUS_RIGHT, on_pwdlg_cancel_in_list)
ON_COMMAND(MSG_CORRECT_PWD, on_pwdlg_correct_in_list)
ON_COMMAND(MSG_EXIT, on_pwdlg_exit)
ON_COMMAND(MSG_SAVE, on_pwdlg_save)
ON_COMMAND(MSG_EXTEND_MENU, on_pwdlg_hot_key)
END_MSGPROC(pwdlg_proc_in_list, cont_class_proc)


BEGIN_KEYMAP(pwdlg_keymap_in_scrn, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_HOT_XEXTEND, MSG_EXTEND_MENU)
END_KEYMAP(pwdlg_keymap_in_scrn, NULL)

BEGIN_KEYMAP(pwdlg_keymap_in_list, NULL)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
ON_EVENT(V_KEY_MENU, MSG_EXIT)
ON_EVENT(V_KEY_HOT_XEXTEND, MSG_EXTEND_MENU)
END_KEYMAP(pwdlg_keymap_in_list, NULL)

BEGIN_AP_EVTMAP(ui_playback_evtmap)
  CONVERT_EVENT(PB_EVT_TTX_READY, MSG_TTX_READY)
  CONVERT_EVENT(PB_EVT_SUB_READY, MSG_SUBT_READY)
  CONVERT_EVENT(PB_EVT_DYNAMIC_PID, MSG_DYNC_PID_UPDATE)
  CONVERT_EVENT(PB_DESCRAMBL_SUCCESS, MSG_DESCRAMBLE_SUCCESS)
  CONVERT_EVENT(PB_DESCRAMBL_FAILED, MSG_DESCRAMBLE_FAILED)
  CONVERT_EVENT(PB_EVT_SCART_VCR_DETECTED, MSG_SCART_VCR_DETECTED)
  CONVERT_EVENT(PB_EVT_NOTIFY_VIDEO_FORMAT, MSG_NOTIFY_VIDEO_FORMAT)
  CONVERT_EVENT(PB_EVT_LOCK_RSL, MSG_PLAY_TUNER_LOCKED)
  CONVERT_EVENT(PB_EVT_UPDATE_VERSION, MSG_PB_NIT_FOUND)
END_AP_EVTMAP(ui_playback_evtmap)

