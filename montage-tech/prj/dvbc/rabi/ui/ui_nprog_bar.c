/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_nprog_bar.h"
#include "ui_pause.h"
#include "ui_live_tv_api.h"
#include "ui_video.h"
#include "file_play_api.h"
#include "Ui_signal.h"
#include "ui_fun_help.h"
#include "ui_small_list_v2.h"
#ifdef ENABLE_CA
//ca
#include "cas_manager.h"
#endif

disp_sys_t vid = VID_SYS_MAX;
extern iconv_t g_cd_utf8_to_utf16le;

enum control_id
{
  IDC_FRAME = 1,
};

enum title_sub_control_id
{
  IDC_NUMBER = 1,
  IDC_NAME,
  IDC_DATE,
  IDC_TIME,
};

enum frame_sub_control_id
{
  IDC_EPG_P = 1,
  IDC_EPG_F,
  IDC_EPG_P_DURATION,
  IDC_EPG_F_DURATION,
  IDC_ICON_AUDIO,
//  IDC_ICON_MONEY,
  IDC_ICON_FAV,
  IDC_ICON_LOCK,
//  IDC_ICON_SKIP,
//  IDC_INFO_EPG,
  IDC_INFO_SUBTT,
  IDC_INFO_TELTEXT,
  IDC_GROUP,
  IDC_CA_INFO,
  IDC_TP_INFO,
//   IDC_SIGNAL_QUALITY,
//  IDC_SIGNAL_INTENSITY,
  /*
  IDC_BUTTON_TV,
  IDC_BUTTON_H,
  IDC_BUTTON_MPEG,*/
  IDC_TEXT_TV,
  IDC_TEXT_H,
  IDC_TEXT_MPEG,
  IDC_ENCRYPT1,
  IDC_ENCRYPT2,
  IDC_ENCRYPT3,
  IDC_ENCRYPT4,
  IDC_ENCRYPT5,
  IDC_ENCRYPT6,
  IDC_ENCRYPT7,
  IDC_ENCRYPT8,
  IDC_ENCRYPT9,
};

/*
static u8 encrypt_icon_idc[] =
{
	IDC_ENCRYPT1,
	IDC_ENCRYPT2,
	IDC_ENCRYPT3,
	IDC_ENCRYPT4,
	IDC_ENCRYPT5,
	IDC_ENCRYPT6,
	IDC_ENCRYPT7,
	IDC_ENCRYPT8,
	IDC_ENCRYPT9
};
*/

enum local_msg
{
  MSG_RECALL = MSG_LOCAL_BEGIN + 600,
  MSG_INFO,
  MSG_CHANGE_ASPECT,
  MSG_PROGBAR_PLAY_PROGRAM,
};

#ifdef ENABLE_ADS
u16 ads_nprogbar_move = 300;
#endif
u16 nprog_bar_cont_keymap(u16 key);
RET_CODE nprog_bar_cont_proc(control_t *cont, u16 msg,u32 para1, u32 para2);
BOOL time_get_if_got_time_flag(void);


static void small_nprog_bar_open_dlg(u16 str_id, u32 time_out)
{
  comm_dlg_data_t dlg_data =
  {
    0,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
  };
  dlg_data.content = str_id;
  dlg_data.dlg_tmout = time_out;
  ui_comm_dlg_close();
  if(fw_find_root_by_id(ROOT_ID_POPUP) == NULL)
  {
    ui_comm_dlg_open(&dlg_data);
  }
}

static BOOL fill_epg_info(dvbs_prog_node_t *p_pg, u16 pos,
  u16 *p_str, u8 *p_time_str, u32 len)
{
  #ifndef IMPL_NEW_EPG
  epg_filter_t epg_info = {0};
  evt_node_t *p_evt_node;
  u32 item_num = 0;
  #else
  epg_prog_info_t prog_info = {0};
  event_node_t   *p_evt_node = NULL;
  event_node_t   *p_present_evt = NULL;
  event_node_t   *p_follow_evt = NULL;
  #endif
  utc_time_t startime = {0};
  utc_time_t endtime = {0};
  //u8 str = " ";

#ifndef IMPL_NEW_EPG
  epg_info.service_id = (u16)p_pg->s_id;
  epg_info.stream_id = (u16)p_pg->ts_id;
  epg_info.orig_network_id = (u16)p_pg->orig_net_id;
  epg_info.cont_level = 0;

   memset(p_str, 0, len);
   memset(p_time_str, 0, len);

  p_evt_node = epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID),
    &epg_info, &item_num);

  if ((pos == 2) && (p_evt_node != NULL))
  {
    p_evt_node = p_evt_node->p_next_evt_node;
  }
#else
  prog_info.network_id = (u16)p_pg->orig_net_id;
  prog_info.ts_id      = (u16)p_pg->ts_id;
  prog_info.svc_id     = (u16)p_pg->s_id;
       
  mul_epg_get_pf_event(&prog_info, &p_present_evt, &p_follow_evt);
  if((pos == 1) && (p_present_evt != NULL))
  {
    p_evt_node = p_present_evt;
  }
  else
    if((pos == 2) && (p_follow_evt != NULL))
    {
      p_evt_node = p_follow_evt;
    }
#endif

  if (p_evt_node != NULL)
  {
    time_to_local(&p_evt_node->start_time, &startime);
    memcpy(&endtime, &startime, sizeof(utc_time_t));
  time_add(&endtime, &(p_evt_node->drt_time));
    uni_strncpy(p_str, p_evt_node->event_name, len);
    sprintf(p_time_str, "%.2d:%.2d - %.2d:%.2d",startime.hour,startime.minute,\
		 endtime.hour,endtime.minute);
    return TRUE;
  }
  else
  {
    gui_get_string(IDS_MSG_NO_CONTENT, p_str, (u16)len);
    return FALSE;
  }

}
/*
static RET_CODE on_pbar_signal_update(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  struct signal_data *data = (struct signal_data *)(para1);
  control_t *p_frm, *p_quality, *p_intensity;

  p_frm = ctrl_get_child_by_id(p_ctrl, IDC_FRAME);
  p_quality = ctrl_get_child_by_id(p_frm, IDC_SIGNAL_QUALITY);
  p_intensity = ctrl_get_child_by_id(p_frm, IDC_SIGNAL_INTENSITY);

  pbar_set_rstyle(p_quality, data->lock ? RSI_ITEM_6_HL : RSI_ITEM_7_HL, RSI_IGNORE, INVALID_RSTYLE_IDX);
  pbar_set_rstyle(p_intensity, data->lock ? RSI_ITEM_6_HL : RSI_ITEM_7_HL, RSI_IGNORE, INVALID_RSTYLE_IDX);

  pbar_set_current(p_quality, data->quality);

  pbar_set_current(p_intensity, data->intensity);

  ctrl_paint_ctrl(p_quality, TRUE);
  ctrl_paint_ctrl(p_intensity, TRUE);

  return SUCCESS;
}
*/
#if 0
static RET_CODE on_pbar_net_prg_update(control_t *p_cont)
{
  control_t *p_frm;
  control_t *p_group, *p_ca_info, *p_tp, 
    *p_singal_quality, *p_singal_intensity;
  control_t *p_epg_p, *p_epg_f;
  control_t *p_epg_p_duration, *p_epg_f_duration;
  u16 i = 0;
  control_t *p_icon;
  u8 asc_str[16]={0};
  u16 cur_prg_source_index = 0;
  iptv_url_msg_info_t p_cur_channel_info = {0};
  
  p_frm = ctrl_get_child_by_id(p_cont, IDC_FRAME);
  p_epg_p = ctrl_get_child_by_id(p_frm, IDC_EPG_P);
  p_epg_f = ctrl_get_child_by_id(p_frm, IDC_EPG_F);
  p_epg_p_duration = ctrl_get_child_by_id(p_frm, IDC_EPG_P_DURATION);
  p_epg_f_duration = ctrl_get_child_by_id(p_frm, IDC_EPG_F_DURATION);
  
  p_group = ctrl_get_child_by_id(p_frm, IDC_GROUP);
  p_ca_info = ctrl_get_child_by_id(p_frm, IDC_CA_INFO);
  p_tp = ctrl_get_child_by_id(p_frm, IDC_TP_INFO);
  p_singal_quality = ctrl_get_child_by_id(p_frm, IDC_SIGNAL_QUALITY);
  p_singal_intensity = ctrl_get_child_by_id(p_frm, IDC_SIGNAL_INTENSITY);

  for(i = 0; i < NPROG_BAR_ENCRYPT_ICON_CNT; i ++)
  {
    p_icon = ctrl_get_child_by_id(p_frm, IDC_ENCRYPT1 + i);
    ctrl_set_sts(p_icon, OBJ_STS_HIDE);
  }

 for(i = 0; i < NPROG_BAR_INFO_ICON_CNT; i ++)
 {
    p_icon = ctrl_get_child_by_id(p_frm, IDC_INFO_EPG + i);
    ctrl_set_sts(p_icon, OBJ_STS_HIDE);
 }

for(i = 0; i < NPROG_BAR_AV_ICON_CNT; i ++)
 {
    p_icon = ctrl_get_child_by_id(p_frm, IDC_TEXT_TV + i);
    ctrl_set_sts(p_icon, OBJ_STS_HIDE);
 }

 for(i = 0; i < NPROG_BAR_MARK_ICON_CNT; i ++)
 {
    p_icon = ctrl_get_child_by_id(p_frm, IDC_ICON_MONEY + i);
    ctrl_set_sts(p_icon, OBJ_STS_HIDE);
 }
 
  ctrl_set_sts(p_group, OBJ_STS_HIDE);
  ctrl_set_sts(p_tp, OBJ_STS_HIDE);
  ctrl_set_sts(p_singal_quality, OBJ_STS_HIDE);
  ctrl_set_sts(p_singal_intensity, OBJ_STS_HIDE);

 text_set_content_by_ascstr(p_epg_p, "No content");
 text_set_content_by_ascstr(p_epg_f, "No content");
 text_set_content_by_ascstr(p_epg_p_duration, "  -  -  -  -");
 text_set_content_by_ascstr(p_epg_p_duration, "  -  -  -  -");

  cur_prg_source_index = ui_iptv_get_cur_prg_source_index(ui_iptv_get_cur_prg_pos());
  ui_iptv_get_cur_channel_info(&p_cur_channel_info);
  if(p_cur_channel_info.url_count <= 0)
    p_cur_channel_info.url_count = 1;
  sprintf(asc_str, "%s: %d/%d", "Source", cur_prg_source_index + 1, p_cur_channel_info.url_count);
  text_set_content_by_ascstr(p_ca_info, asc_str);

  ctrl_paint_ctrl(p_frm, TRUE);
  return SUCCESS;
}
#endif
/*
static u16 get_audio_format_icon_id(u16 type)
{
  u16 icon_id;
  switch(type)
  {
    case AUDIO_AC3_VSB:
        icon_id = IM_INFO_ICON_AC3;
        break;
     case AUDIO_EAC3:
        icon_id = IM_INFO_ICON_EAC3;
        break;
     case AUDIO_AAC:
      icon_id = IM_INFO_ICON_AAC;
      break;
     default:
      icon_id = IM_INFO_ICON_MPEG;
      break;

  }
  return icon_id;
}

static u16 get_video_format_icon_id(u32 type)
{
  u16 icon_id;
  switch(type)
  {
    case VIDEO_H264:
    icon_id = IM_INFO_ICON_H264;
    break;
    case VIDEO_AVS:
    case VIDEO_AVS_ES:
    icon_id = IM_INFO_ICON_AVS;
    break;
    default :
    icon_id = IM_INFO_ICON_MPEG;
    break;
  }
  return icon_id;
}
*/

static void get_vid_format_content(disp_sys_t vid, u8 *content)
{

	switch (vid)
	{
		case VID_SYS_NTSC_J:
		case VID_SYS_NTSC_M:
		case VID_SYS_NTSC_443:
			sprintf(content, "480I");
			break;
		case VID_SYS_480P:
			sprintf(content, "480P");
			break;
		case VID_SYS_PAL:
		case VID_SYS_PAL_N:
		case VID_SYS_PAL_NC:
		case VID_SYS_PAL_M:
			sprintf(content, "576I");
		break;
		case VID_SYS_576P_50HZ:
			sprintf(content, "576P");
			break;
		case VID_SYS_720P:
		case VID_SYS_720P_24HZ:
		case VID_SYS_720P_25HZ:
		case VID_SYS_720P_30HZ:
		case VID_SYS_720P_50HZ:
			sprintf(content, "720P");
			break;
		case VID_SYS_1080I:
		case VID_SYS_1080I_50HZ:
			sprintf(content, "1080I");
			break;
		case VID_SYS_1080P:
		case VID_SYS_1080P_24HZ:
		case VID_SYS_1080P_25HZ:
		case VID_SYS_1080P_30HZ:
		case VID_SYS_1080P_50HZ:
			sprintf(content, "1080P");
			break;
		default :
			sprintf(content, "unknown");
			break;
	}
}

static u16 convert_nim_modulate_to_str_id(u32 nim_modulate)
{
  u16 str_id;
  switch(nim_modulate)
  {
    case NIM_MODULA_AUTO:
      str_id = IDS_AUTO;
      break;

    case NIM_MODULA_BPSK:
      str_id = IDS_BPSK;
      break;

    case NIM_MODULA_QPSK:
      str_id =IDS_QPSK;
      break;

    case NIM_MODULA_8PSK:
      str_id = IDS_8PSK;
      break;

    case NIM_MODULA_QAM16:
      str_id = IDS_QAM16;
      break;

    case NIM_MODULA_QAM32:
      str_id = IDS_QAM32;
      break;

    case NIM_MODULA_QAM64:
      str_id = IDS_QAM64;
      break;

    case NIM_MODULA_QAM128:
      str_id = IDS_QAM128;
      break;

    case NIM_MODULA_QAM256:
      str_id = IDS_QAM256;
      break;
    default:
      str_id = IDS_QAM64;
      break;
  }

  return str_id;
}

void fill_nprog_info(control_t *cont, u16 prog_id)
{
  control_t *p_ctrl, *p_title, *p_subctrl;
  dvbs_prog_node_t pg;
  dvbs_tp_node_t tp;
  u8 asc_str[32] = { 0 };
  BOOL is_age_lock = FALSE;

  sat_node_t sat;
#if 0
  u8 is_encrypt[9] = { 0 };
  u16 icon_id;
  u16 encrypt_Invalid_icon_rid[] =
  {
	  IM_INFO_ICON_B_2,  IM_INFO_ICON_I_2, IM_INFO_ICON_S_2,IM_INFO_ICON_V_2,IM_INFO_ICON_N_2,IM_INFO_ICON_CW_2,IM_INFO_ICON_ND_2,IM_INFO_ICON_CO_2,IM_INFO_ICON_QUESMARK_2
  };
  u16 encrypt_Selected_icon_rid[] =
  {
	  IM_INFO_ICON_B_1,  IM_INFO_ICON_I_1, IM_INFO_ICON_S_1,IM_INFO_ICON_V_1,IM_INFO_ICON_N_1,IM_INFO_ICON_CW_1,IM_INFO_ICON_ND_1,IM_INFO_ICON_CO_1,IM_INFO_ICON_QUESMARK
  };
  u32 i;
#endif

  u16 uni_buf[32] = {0};
  u8 time_str[32];
  u16 prog_pos;
  BOOL is_mark, is_epg_get = FALSE;
//  iptv_channel_info_t p_net_pg = {0};
  u32 tuner_id = 0;
  u8 nim_asc_str[32] = { 0 };


//  BOOL is_tv = sys_status_get_curn_prog_mode() == CURN_MODE_TV;
  u8 view_id = ui_dbase_get_pg_view_id();
  tuner_id = ui_get_cur_prg_tuner_id();

  if ((db_dvbs_get_pg_by_id(prog_id, &pg) != DB_DVBS_OK) &&
    (ui_get_play_prg_type() == SAT_PRG_TYPE))
  {
    UI_PRINTF("SAT PROGBAR: can NOT get pg!\n");
    return;
  }
/*
   if ((iptv_get_channel_info_is_ready() == FALSE) &&
    (ui_get_play_prg_type() == NET_PRG_TYPE))
  {
    UI_PRINTF("NET PROGBAR: can NOT get pg!\n");
    return;
  }
*/
  prog_pos = db_dvbs_get_view_pos_by_id(view_id, prog_id);

  p_ctrl = ctrl_get_child_by_id(cont, IDC_FRAME);
  p_title = ctrl_get_child_by_id(p_ctrl, IDC_COMM_TITLE_CONT);

  // set tv radio icon
//  p_subctrl = ctrl_get_child_by_id(p_title, IDC_NICON);
//  bmap_set_content_by_id(p_subctrl, is_tv ? IM_INFOR_TV : IM_INFOR_TV);//IM_TITLEICON_RADIO

  // set number
  if(ui_get_play_prg_type() == NET_PRG_TYPE)
  {
    p_subctrl = ctrl_get_child_by_id(p_title, IDC_NUMBER);
//    prog_pos = ui_iptv_get_cur_prg_pos();
    sprintf(asc_str, "%.4d", (int)(prog_pos + 1));
  }else
  {
    p_subctrl = ctrl_get_child_by_id(p_title, IDC_NUMBER);
    sprintf(asc_str, "%.4d", (int)(prog_pos + 1));
  }
  text_set_content_by_ascstr(p_subctrl, asc_str);

  // set name
    p_subctrl = ctrl_get_child_by_id(p_title, IDC_NAME);
   if(ui_get_play_prg_type() == NET_PRG_TYPE)
   {
     /* prog_pos = ui_iptv_get_cur_prg_pos();
      iptv_get_pg_by_pos(&p_net_pg, (u16)(prog_pos));
      nprog_bar_convert_str_asc2unistr(p_net_pg.channel_name, uni_buf, sizeof(p_net_pg.channel_name));
      text_set_content_by_unistr(p_subctrl, uni_buf);
      memset(uni_buf, 0, sizeof(uni_buf));*/
   }else
   {
	 ui_dbase_get_full_prog_name(&pg, uni_buf, 31);
      text_set_content_by_unistr(p_subctrl, uni_buf);
   }

  // set epg pf
  if(ui_epg_is_initialized() && fill_epg_info(&pg, 1, uni_buf,time_str, 31))
  {
    is_epg_get = TRUE;
	p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_EPG_P_DURATION);
  text_set_content_by_ascstr(p_subctrl, time_str);
  }
  else
  {
  	p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_EPG_P_DURATION);
       text_set_content_by_ascstr(p_subctrl, "  -  -  -  -");
  }
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_EPG_P);
  text_set_content_by_unistr(p_subctrl, uni_buf);


  if(ui_epg_is_initialized() && fill_epg_info(&pg, 2, uni_buf,time_str, 31))
  {
    is_epg_get = TRUE;
	p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_EPG_F_DURATION);
  text_set_content_by_ascstr(p_subctrl, time_str);
  }
  else
  {
  	p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_EPG_F_DURATION);
       text_set_content_by_ascstr(p_subctrl, "  -  -  -  -");
  }
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_EPG_F);
  text_set_content_by_unistr(p_subctrl, uni_buf);

  // set mark icon
  /*
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_ICON_MONEY);
  is_mark = (BOOL)(pg.is_scrambled);
bmap_set_content_by_id(p_subctrl,
   (u16)(is_mark ? IM_INFO_ICON_MONEY : IM_INFO_ICON_MONEY_2));
*/
  p_subctrl = ctrl_get_child_by_id(p_title, IDC_ICON_FAV);
  is_mark = ui_dbase_pg_is_fav(view_id, prog_pos);
  if(is_mark)
 	  ctrl_set_sts(p_subctrl, OBJ_STS_SHOW);
  else
	  ctrl_set_sts(p_subctrl, OBJ_STS_HIDE);

  p_subctrl = ctrl_get_child_by_id(p_title, IDC_ICON_LOCK);
	is_mark = db_dvbs_get_mark_status(view_id, prog_pos, DB_DVBS_MARK_LCK, 0);
  is_age_lock =do_chk_age_limit(prog_id);
  if(is_mark)
 	  ctrl_set_sts(p_subctrl, OBJ_STS_SHOW);
  else
	  ctrl_set_sts(p_subctrl, OBJ_STS_HIDE);
/*
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_ICON_SKIP);
  is_mark = db_dvbs_get_mark_status(view_id, prog_pos, DB_DVBS_MARK_SKP, 0);
 bmap_set_content_by_id(p_subctrl, (u16)(is_mark ? IM_INFO_ICON_SKIP: IM_INFO_ICON_SKIP_2));
*/
  // set info icon
#if ENABLE_TTX_SUBTITLE
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_INFO_SUBTT);
  is_mark = ui_is_subt_data_ready(prog_id);
  bmap_set_content_by_id(p_subctrl,
   (u16)(is_mark ? IM_INFO_ICON_SUBTITLE: IM_INFO_ICON_SUBTITLE_2));

  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_INFO_TELTEXT);
  is_mark = ui_is_ttx_data_ready(prog_id);
 bmap_set_content_by_id(p_subctrl,
   (u16)(is_mark ? IM_INFO_ICON_TTX: IM_INFO_ICON_TTX_2));
#endif
/*
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_INFO_EPG);
  is_mark = is_epg_get;
 bmap_set_content_by_id(p_subctrl,
  (u16)(is_mark ? IM_INFO_ICON_EPG: IM_INFO_ICON_EPG_2));
*/
	if(CUSTOMER_ID != CUSTOMER_DTMB_DESAI_JIMO)
	{
		db_dvbs_get_sat_by_id((u16)pg.sat_id, &sat);
		//group info
		p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_GROUP);
		text_set_content_by_unistr(p_subctrl, sat.name);
		ui_enable_signal_monitor_by_tuner(TRUE,0);
		// ca info
		p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_CA_INFO);
		db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
		if(sat.scan_mode == DVBT_SCAN)
		{
			sprintf(asc_str, "Fre:%d KHz", (int)tp.freq);
		}
		else if(sat.scan_mode == DVBC_SCAN)
		{
			gui_get_string(convert_nim_modulate_to_str_id(tp.nim_modulate), uni_buf, 32);
			str_uni2asc((u8 *)nim_asc_str,(u16 *)uni_buf);
			sprintf(asc_str, "%d/%s/%d", (int)tp.freq, nim_asc_str, (int)tp.sym);
		}
		text_set_content_by_ascstr(p_subctrl, asc_str);
	}
#if 0
  //tp info
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_TP_INFO);
  db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);

   sprintf(asc_str, "PID:%d/%d/%d/%d", pg.video_pid, pg.audio[pg.audio_channel].p_id,
	  pg.pcr_pid,pg.s_id);
  text_set_content_by_ascstr(p_subctrl, asc_str);

    // SET Audio,Vdec,video formats

  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_TEXT_TV);
  icon_id = get_vid_format_icon_id(vid);
  bmap_set_content_by_id(p_subctrl, icon_id);
  if(!is_tv)
  {
    ctrl_set_sts(p_subctrl, OBJ_STS_HIDE);
  }
  else
  {
    ctrl_set_attr(p_subctrl, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_subctrl, OBJ_STS_SHOW);
  }
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_TEXT_H);
  icon_id = get_video_format_icon_id(pg.video_type);
  bmap_set_content_by_id(p_subctrl, icon_id);
  if(!is_tv)
  {
    ctrl_set_sts(p_subctrl, OBJ_STS_HIDE);
  }
  else
  {
    ctrl_set_attr(p_subctrl, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_subctrl, OBJ_STS_SHOW);
  }
  p_subctrl = ctrl_get_child_by_id(p_ctrl, IDC_TEXT_MPEG);
  icon_id = get_audio_format_icon_id(pg.audio[pg.audio_channel].type);
 bmap_set_content_by_id(p_subctrl, icon_id);


  //SET ENCRYPT SYS
   is_mark = (BOOL)(pg.is_scrambled);
  if(is_mark == 0)
  	{		for(i = 0;i < 9;i++)
		{
			p_subctrl = ctrl_get_child_by_id(p_ctrl, encrypt_icon_idc[i]);
			bmap_set_content_by_id(p_subctrl, encrypt_Invalid_icon_rid[i]);
		}
  	}
  else
  {
    if(!pg.ecm_num)
    {
    	  if(pg.ca_system_id == 0x2600)
    	  {
    		  is_encrypt[0] = 1;
    	  }
    	  else if(pg.ca_system_id >= 0x0600 && pg.ca_system_id <= 0x06ff)
    	  {
    		  is_encrypt[1] = 1;
    	  }
    	  else if(pg.ca_system_id >= 0x0100 && pg.ca_system_id <= 0x01ff)
    	  {
    		  is_encrypt[2] = 1;
    	  }
    	  else if(pg.ca_system_id >= 0x0500 && pg.ca_system_id <= 0x05ff)
    	  {
    		  is_encrypt[3] = 1;
    	  }
    	  else if(pg.ca_system_id >= 0x1800 && pg.ca_system_id <= 0x18ff)
    	  {
    		  is_encrypt[4] = 1;
    	  }
    	  else if(pg.ca_system_id >= 0x0d00 && pg.ca_system_id <= 0x0dff)
    	  {
    		  is_encrypt[5] = 1;
    	  }
    	  else if(pg.ca_system_id >= 0x0900 && pg.ca_system_id <= 0x09ff)
    	  {
    		  is_encrypt[6] = 1;
    	  }
    	   else if(pg.ca_system_id >= 0x0b00 && pg.ca_system_id <= 0x0bff)
    	  {
    		  is_encrypt[7] = 1;
    	  }
    	  else
    	  {
    	    is_encrypt[8] = 1;
    	  }
	  }
	  else
	  {
#ifdef ECM_DB_SUPPORT
	      for(i = 0; i < pg.ecm_num; i++)
	      {
	        if(pg.cas_ecm[i].ca_sys_id == 0x2600)  //Biss
        	  {
        		  is_encrypt[0] = 1;
        	  }
        	  else if(pg.cas_ecm[i].ca_sys_id >= 0x0600 && pg.cas_ecm[i].ca_sys_id <= 0x06ff)  // Irdeto
        	  {
        		  is_encrypt[1] = 1;
        	  }
        	  else if(pg.cas_ecm[i].ca_sys_id >= 0x0100 && pg.cas_ecm[i].ca_sys_id <= 0x01ff) //Seca
        	  {
        		  is_encrypt[2] = 1;
        	  }
        	  else if(pg.cas_ecm[i].ca_sys_id >= 0x0500 && pg.cas_ecm[i].ca_sys_id <= 0x05ff)// Viaccess
        	  {
        		  is_encrypt[3] = 1;
        	  }
        	  else if(pg.cas_ecm[i].ca_sys_id >= 0x1800 && pg.cas_ecm[i].ca_sys_id <= 0x18ff) //Nagravison
        	  {
        		  is_encrypt[4] = 1;
        	  }
        	  else if(pg.cas_ecm[i].ca_sys_id >= 0x0d00 && pg.cas_ecm[i].ca_sys_id <= 0x0dff)  //Crypto Work
        	  {
        		  is_encrypt[5] = 1;
        	  }
        	  else if(pg.cas_ecm[i].ca_sys_id >= 0x0900 && pg.cas_ecm[i].ca_sys_id <= 0x09ff)//NDS
        	  {
        		  is_encrypt[6] = 1;
        	  }
        	   else if(pg.cas_ecm[i].ca_sys_id >= 0x0b00 && pg.cas_ecm[i].ca_sys_id <= 0x0bff) // Connax
        	  {
        		  is_encrypt[7] = 1;
        	  }
        	  else
        	  {
        	    is_encrypt[8] = 1;
        	  }
	      }
#else
	      for(i = 0; i < pg.ecm_num; i++)
	      {
	        if(pg.cas_ecm[i] == 0x2600)  //Biss
        	  {
        		  is_encrypt[0] = 1;
        	  }
        	  else if(pg.cas_ecm[i] >= 0x0600 && pg.cas_ecm[i] <= 0x06ff)  // Irdeto
        	  {
        		  is_encrypt[1] = 1;
        	  }
        	  else if(pg.cas_ecm[i] >= 0x0100 && pg.cas_ecm[i] <= 0x01ff) //Seca
        	  {
        		  is_encrypt[2] = 1;
        	  }
        	  else if(pg.cas_ecm[i] >= 0x0500 && pg.cas_ecm[i] <= 0x05ff)// Viaccess
        	  {
        		  is_encrypt[3] = 1;
        	  }
        	  else if(pg.cas_ecm[i] >= 0x1800 && pg.cas_ecm[i] <= 0x18ff) //Nagravison
        	  {
        		  is_encrypt[4] = 1;
        	  }
        	  else if(pg.cas_ecm[i] >= 0x0d00 && pg.cas_ecm[i] <= 0x0dff)  //Crypto Work
        	  {
        		  is_encrypt[5] = 1;
        	  }
        	  else if(pg.cas_ecm[i] >= 0x0900 && pg.cas_ecm[i] <= 0x09ff)//NDS
        	  {
        		  is_encrypt[6] = 1;
        	  }
        	   else if(pg.cas_ecm[i] >= 0x0b00 && pg.cas_ecm[i] <= 0x0bff) // Connax
        	  {
        		  is_encrypt[7] = 1;
        	  }
        	  else
        	  {
        	    is_encrypt[8] = 1;
        	  }
	      }
#endif
	  }
	  for(i = 0;i < 9;i++)
	  {
		  p_subctrl = ctrl_get_child_by_id(p_ctrl, encrypt_icon_idc[i]);
		  bmap_set_content_by_id(p_subctrl, is_encrypt[i]?encrypt_Selected_icon_rid[i]: encrypt_Invalid_icon_rid[i]);
	  }
  }
  // update view
//  if(ui_get_play_prg_type() == NET_PRG_TYPE)
   // on_pbar_net_prg_update(cont);
#endif
  ctrl_paint_ctrl(cont, TRUE);
}

void fill_ntime_info(control_t *cont, BOOL is_redraw)
{
  utc_time_t time = {0};
  utc_time_t gmt_time = {0};
  time_set_t p_set={{0}};
  control_t *p_frm = NULL, *p_title, *p_ctrl = NULL;
  u8 time_str[32];

  if(FALSE == time_get_if_got_time_flag())
	return;
  
  p_frm = ctrl_get_child_by_id(cont, IDC_FRAME);
  MT_ASSERT(p_frm != NULL);
  p_title = ctrl_get_child_by_id(p_frm, IDC_COMM_TITLE_CONT);
  MT_ASSERT(p_title != NULL);

  sys_status_get_time(&p_set);

  time_get(&time, p_set.gmt_usage);
  if( p_set.gmt_usage)
   {
    time_to_local(&time, &gmt_time);
    memcpy(&time,&gmt_time,sizeof(gmt_time));
   }
  sprintf(time_str, "%.4d/%.2d/%.2d", time.year, time.month, time.day);
  p_ctrl = ctrl_get_child_by_id(p_title, IDC_DATE);
  MT_ASSERT(p_ctrl != NULL);
  text_set_content_by_ascstr(p_ctrl, time_str);

  sprintf(time_str, "%.2d:%.2d", time.hour, time.minute);
  p_ctrl = ctrl_get_child_by_id(p_title, IDC_TIME);
  MT_ASSERT(p_ctrl != NULL);
  text_set_content_by_ascstr(p_ctrl, time_str);
  if (is_redraw)
  {
    ctrl_paint_ctrl(ctrl_get_parent(p_ctrl), TRUE);
  }
}

control_t *ui_nprog_title_create(control_t *parent, u16 icon_id, u8 rstyle)
{
  control_t *p_cont, *p_first;
  MT_ASSERT(parent != NULL);

  p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_COMM_TITLE_CONT,
                            NPROG_BAR_TITLE_X, 0,
                            NPROG_BAR_TITLE_W, NPROG_BAR_TITLE_H,
                            NULL, 0);
  ctrl_set_rstyle(p_cont, rstyle, rstyle, rstyle);
/*
  //creat bmp icon
  p_icon = ctrl_create_ctrl(CTRL_BMAP, IDC_NICON,
                          NPROG_BAR_TITLE_ICON_X, NPROG_BAR_TITLE_ICON_Y,
                          NPROG_BAR_TITLE_ICON_W, NPROG_BAR_TITLE_ICON_H,
                          p_cont, 0);
  bmap_set_content_by_id(p_icon, icon_id);
*/
  // insert to parent
  p_first = parent->p_child;

  if(p_first != NULL)
  {
    p_first->p_prev = p_cont;
    p_cont->p_next = p_first;
  }
  parent->p_child = p_cont;
  p_cont->p_parent = parent;

  return p_cont;
}

static RET_CODE on_prog_bar_play_net_prg(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{
#if 0
    iptv_channel_info_t p_pg = {0};
  
   if(ui_get_play_prg_type() == NET_PRG_TYPE)
   {
      iptv_get_pg_by_pos(&p_pg,ui_iptv_get_cur_prg_pos());
      ui_iptv_request_url_info(p_pg.channel_id);
   }
    #endif
   return SUCCESS;
}

static void nprog_bar_change_pg_with_satip_client(u16 pg_id)
{
    dvbs_prog_node_t client_pg = {0};
    dvbs_prog_node_t curr_pg = {0};
    dvbs_tp_node_t client_tp = {0};
    sat_node_t client_sat = {0}; 
    comm_dlg_data_t dlg_data =
    {
        0,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        IDS_CHANGING_PG_WITH_CLINET,
        3000,
    };
    u16 client_pg_id;
    u16 curr_pg_id;
    OS_PRINTF("####%s, line[%d], pg_id == [%d]####\n", __FUNCTION__, __LINE__, pg_id);
    client_pg_id = (u16)pg_id;
    curr_pg_id = sys_status_get_curn_group_curn_prog_id();
    db_dvbs_get_pg_by_id(client_pg_id, &client_pg);
    db_dvbs_get_pg_by_id(curr_pg_id, &curr_pg);
    if(client_pg.tp_id != curr_pg.tp_id)
    {
        db_dvbs_get_tp_by_id(client_pg.tp_id, &client_tp);
        OS_PRINTF("##tp id==[%d],sate_id=[%d],freq=[%d],sym=[%d],polarity=[%d],is_on22k=[%d]##\n", client_tp.id, 
        client_tp.sat_id,client_tp.freq,client_tp.sym,client_tp.polarity,client_tp.is_on22k);
        db_dvbs_get_sat_by_id(client_tp.sat_id, &client_sat);
       // ui_set_antenna(&client_sat);
       // ui_set_transpond(&client_tp);
        OS_PRINTF("####%s, line[%d], clinet tp id != curr tp id, will lock clinet tp####\n", __FUNCTION__, __LINE__);
    }
    else
    {
        OS_PRINTF("####%s, line[%d], clinet tp id == curr tp id####\n", __FUNCTION__, __LINE__);
    }
    //if(client_pg_id != curr_pg_id)
    {
        //ui_comm_cfmdlg_open(NULL, IDS_CHANGING_PG_WITH_CLINET, NULL, 3000);
        ui_comm_dlg_open(&dlg_data);
		DEBUG(MAIN, INFO, "\n");
            ui_play_prog(client_pg_id, FALSE);
        }
    }

RET_CODE open_nprog_bar(u32 para1, u32 para2)
{
  control_t *p_cont;
#ifndef SPT_SUPPORT
  control_t *p_ctrl, *p_title, *p_subctrl;
  u16 i, x, y;
#endif
  s16 shift_prg = 0;
  u32 tuner_id = 0;
  BOOL is_satip_client = FALSE;
  u8 view_id = ui_dbase_get_pg_view_id();
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();
  u16 prog_pos = db_dvbs_get_view_pos_by_id(view_id, prog_id);
  BOOL is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);
    DEBUG(DBG, INFO, "view_id:%d, prog_id:%d, prog_pos:%d\n", view_id, prog_id, prog_pos);
  if(CURN_MODE_NONE==sys_status_get_curn_prog_mode()
  	|| db_dvbs_get_count( view_id) == 0)
  {
	  OS_PRINTF("!!!no pg!!!\n");
	  manage_open_menu(ROOT_ID_MAINMENU, 0, 0) ;
	  return ERR_FAILURE;
  }
  if(ui_video_c_init_state() == FALSE)
  {
    if(ui_get_play_prg_type() == NET_PRG_TYPE)
    {
      ui_video_c_create(VIDEO_PLAY_FUNC_ONLINE);
    }
    else if(ui_get_play_prg_type() == CAMERA_PRG_TYPE)
    {
      ui_video_c_create(VIDEO_PLAY_FUNC_USB);
    }
  }

  // shift prog.
  switch (para1)
  {
    case V_KEY_UP:
      shift_prg = 1;
      vid = VID_SYS_MAX;
      break;
    case V_KEY_DOWN:
      shift_prg = -1;
      vid = VID_SYS_MAX;
      break;
    case V_KEY_PAGE_UP:
      shift_prg = -(SMALL_LIST_PAGE);
      vid = VID_SYS_MAX;
      break;
    case V_KEY_PAGE_DOWN:
      shift_prg = SMALL_LIST_PAGE;
      vid = VID_SYS_MAX;
      break;
    case V_KEY_RECALL:
    case V_KEY_BACK:
      if((ui_get_play_prg_type() == NET_PRG_TYPE)
       ||(ui_get_play_prg_type() == CAMERA_PRG_TYPE))
      {
     //    prog_id = ui_iptv_get_prev_prg_pos();
   //      ui_iptv_set_prev_prg_pos(ui_iptv_get_cur_prg_pos());
   //      ui_iptv_set_cur_prg_pos(prog_id);
      }else
      {
          ui_recall(FALSE, &prog_id);
          vid = VID_SYS_MAX;
      }
      break;
    case V_KEY_TVRADIO:
      if((ui_get_play_prg_type() == NET_PRG_TYPE)
       ||(ui_get_play_prg_type() == CAMERA_PRG_TYPE))
          return SUCCESS;
      
      if(!ui_tvradio_switch(FALSE, &prog_id))
      {
        u16 content = (u16)(is_tv ? IDS_MSG_NO_RADIO_PROG : IDS_MSG_NO_TV_PROG);
        ui_comm_cfmdlg_open(NULL, content, NULL, 0);
        return ERR_FAILURE;
      }
      vid = VID_SYS_MAX;
      break;
      case V_KEY_UNKNOW:
        {
            u8 view_id = 0;
            ui_comm_pwdlg_close();
            manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
            OS_PRINTF("####%s, V_KEY_UNKNOW for satip, prog_id = [%d],prog_pos=[%d]####\n", __FUNCTION__, 
            para2,prog_pos);
            prog_id = para2;
            is_satip_client = TRUE;
            sys_status_set_curn_group(0);//set curr group is all group  
            sys_status_set_curn_prog_mode(CURN_MODE_TV);
            view_id = ui_dbase_create_view(DB_DVBS_ALL_TV, 0, NULL);
            ui_dbase_set_pg_view_id(view_id);
            sys_status_save();
        }
        break;
    default:
      break;
  }

  if(shift_prg != 0)
  {
    if(ui_get_play_prg_type() == NET_PRG_TYPE)
    {
      //ui_shift_net_prog(shift_prg);
    }else if(ui_get_play_prg_type() == SAT_PRG_TYPE)
    {
      ui_shift_prog(shift_prg, FALSE, &prog_id);
    }
  }

  if(prog_pos == INVALIDID)
  {
    prog_id = db_dvbs_get_id_by_view_pos(view_id, 0);
  }
  
  // check, if no prog
  if(ui_get_play_prg_type() == SAT_PRG_TYPE)
  {
    if(prog_id == INVALIDID)
    {
      UI_PRINTF("PROGBAR: ERROR, no prog in view!\n");
      return ERR_FAILURE;
    }
  }
  // ui_play_prog(prog_id, FALSE);
   manage_close_menu(ROOT_ID_SLEEP_HOTKEY, 0, 0) ;
  // reset
  is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);

  ui_epg_init();
#ifndef IMPL_NEW_EPG
  ui_epg_start();
#else
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#endif

  #ifdef ENABLE_ADS
  if(ui_adv_pic_play(ADS_AD_TYPE_CHBAR, ROOT_ID_PROG_BAR))
  {
    ads_nprogbar_move = 200;
  }
  else
    ads_nprogbar_move = 0;
  #endif

#ifndef SPT_SUPPORT
  /* create */
  p_cont = fw_create_mainwin(ROOT_ID_PROG_BAR,
                          NPROG_BAR_CONT_X, NPROG_BAR_CONT_Y,
                          NPROG_BAR_CONT_W, NPROG_BAR_CONT_H,
                          ROOT_ID_INVALID, 0,
                          OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  ctrl_set_rstyle(p_cont, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_keymap(p_cont, nprog_bar_cont_keymap);
  ctrl_set_proc(p_cont, nprog_bar_cont_proc);

  // frame part
  p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_FRAME,
                          NPROG_BAR_FRM_X, NPROG_BAR_FRM_Y,
	                      NPROG_BAR_FRM_W + NPROG_BAR_ADS_MOVE, NPROG_BAR_FRM_H,
	                      p_cont, 0);
  ctrl_set_rstyle(p_ctrl, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);

  // title
  p_title = ui_nprog_title_create(p_ctrl, 0, RSI_PBACK);

  //programe number
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_NUMBER,
                              NPROG_BAR_NUMER_X, NPROG_BAR_NUMER_Y,
                              NPROG_BAR_NUMER_W, NPROG_BAR_NUMER_H,
                              p_title, 0);
  text_set_align_type(p_subctrl, STL_LEFT |STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_NPROG_BAR_NAME, FSI_NPROG_BAR_NAME, FSI_NPROG_BAR_NAME);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  //program name
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_NAME,
                              NPROG_BAR_TITLE_NAME_X, NPROG_BAR_TITLE_NAME_Y,
                              NPROG_BAR_TITLE_NAME_W, NPROG_BAR_TITLE_NAME_H,
                              p_title, 0);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_NPROG_BAR_NAME, FSI_NPROG_BAR_NAME, FSI_NPROG_BAR_NAME);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  // date & time
  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, 0,
                              NPROG_BAR_DATE_ICON_X, NPROG_BAR_DATE_ICON_Y,
                              NPROG_BAR_DATE_ICON_W, NPROG_BAR_DATE_ICON_H,
                              p_title, 0);

  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_DATE,
                              NPROG_BAR_DATE_TXT_X, NPROG_BAR_DATE_TXT_Y,
                              NPROG_BAR_DATE_TXT_W, NPROG_BAR_DATE_TXT_H,
                              p_title, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT |STL_BOTTOM);
  text_set_font_style(p_subctrl, FSI_NPROG_BAR_NORMAL, FSI_NPROG_BAR_NORMAL, FSI_NPROG_BAR_NORMAL);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  p_subctrl = ctrl_create_ctrl(CTRL_BMAP, 0,
                              NPROG_BAR_TIME_ICON_X, NPROG_BAR_TIME_ICON_Y,
                              NPROG_BAR_TIME_ICON_W, NPROG_BAR_TIME_ICON_H,
                              p_title, 0);
  if(TRUE == time_get_if_got_time_flag())
	  bmap_set_content_by_id(p_subctrl, IM_IC_SET_TIME);

  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TIME,
                              NPROG_BAR_TIME_TXT_X, NPROG_BAR_TIME_TXT_Y,
                              NPROG_BAR_TIME_TXT_W, NPROG_BAR_TIME_TXT_H,
                              p_title, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  text_set_align_type(p_subctrl, STL_LEFT |STL_BOTTOM);
  text_set_font_style(p_subctrl, FSI_NPROG_BAR_NORMAL, FSI_NPROG_BAR_NORMAL,FSI_NPROG_BAR_NORMAL);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  // epg pf
  y = NPROG_BAR_PF_INFO_Y;
  for (i = 0; i < NPROG_BAR_PF_INFO_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_EPG_P + i),
                                NPROG_BAR_PF_INFO_X + NPROG_BAR_PF_INFO_DUR_W, y,
                                NPROG_BAR_PF_INFO_PRO_W, NPROG_BAR_PF_INFO_H,
                                p_ctrl, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_NPROG_BAR_NORMAL, FSI_NPROG_BAR_NORMAL, FSI_NPROG_BAR_NORMAL);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
    y += NPROG_BAR_PF_INFO_H + NPROG_BAR_PF_INFO_V_GAP;
  }
  y = NPROG_BAR_PF_INFO_Y;
  for(i = 0; i < NPROG_BAR_PF_INFO_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_EPG_P_DURATION + i),
                                NPROG_BAR_PF_INFO_X , y,
                                NPROG_BAR_PF_INFO_DUR_W, NPROG_BAR_PF_INFO_H,
                                p_ctrl, 0);
    ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_subctrl, FSI_NPROG_BAR_NORMAL, FSI_NPROG_BAR_NORMAL, FSI_NPROG_BAR_NORMAL);
    text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
    y += NPROG_BAR_PF_INFO_H + NPROG_BAR_PF_INFO_V_GAP;
  }

  // group
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_GROUP,
                              NPROG_BAR_GROUP_X, NPROG_BAR_GROUP_Y,
                              NPROG_BAR_GROUP_W, NPROG_BAR_GROUP_H,
                              p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_CENTER);
  text_set_font_style(p_subctrl, FSI_NPROG_BAR_NORMAL, FSI_NPROG_BAR_NORMAL, FSI_NPROG_BAR_NORMAL);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  // ca info
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_CA_INFO,
                              NPROG_BAR_CA_X, NPROG_BAR_CA_Y,
                              NPROG_BAR_CA_W,  NPROG_BAR_CA_H,
                              p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_subctrl, FSI_NPROG_BAR_NORMAL, FSI_NPROG_BAR_NORMAL, FSI_NPROG_BAR_NORMAL);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);

  //tp info
  p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TP_INFO,
                              NPROG_BAR_TP_X, NPROG_BAR_TP_Y,
                              NPROG_BAR_TP_W, NPROG_BAR_TP_H,
                              p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_subctrl, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
  text_set_font_style(p_subctrl, FSI_NPROG_BAR_NORMAL, FSI_NPROG_BAR_NORMAL, FSI_NPROG_BAR_NORMAL);

  // mark icon    MONEY  FAV LOCK SKIP
  x = NPROG_BAR_MARK_ICON_X;
  for (i = 0; i < NPROG_BAR_MARK_ICON_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl(CTRL_BMAP, (u8)(IDC_ICON_FAV+ i),
                                x, NPROG_BAR_MARK_ICON_Y,
                                NPROG_BAR_MARK_ICON_W, NPROG_BAR_MARK_ICON_H,
                                p_title, 0);
    switch(IDC_ICON_FAV+i)
    {
    case IDC_ICON_FAV:
    	bmap_set_content_by_id(p_subctrl, IM_XTV_FAV);
    	break;
	case IDC_ICON_LOCK:
    	bmap_set_content_by_id(p_subctrl, IM_IC_SET_LOCK);
    	break;
    }
    ctrl_set_sts(p_subctrl, OBJ_STS_HIDE);
    x += NPROG_BAR_MARK_ICON_W + NPROG_BAR_MARK_ICON_H_GAP;
  }

  // info icon  EPG  SubT  TTX
  x = NPROG_BAR_INFO_ICON_X;
  for (i = 0; i < NPROG_BAR_INFO_ICON_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl(CTRL_BMAP, (u8)(IDC_INFO_SUBTT+ i),
                                x, NPROG_BAR_INFO_ICON_Y,
                                NPROG_BAR_INFO_ICON_W , NPROG_BAR_INFO_ICON_H,
                                p_ctrl, 0);
    x += NPROG_BAR_INFO_ICON_W + NPROG_BAR_INFO_ICON_H_GAP;
  }

  //ENCRYPT SYS
  x = NPROG_BAR_ENCRYPT_ICON_X;
  for (i = 0; i < NPROG_BAR_ENCRYPT_ICON_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_ENCRYPT1+i,
                                x, NPROG_BAR_ENCRYPT_ICON_Y,
                                NPROG_BAR_ENCRYPT_ICON_W, NPROG_BAR_ENCRYPT_ICON_H,
                                p_ctrl, 0);
    x += NPROG_BAR_ENCRYPT_ICON_W + NPROG_BAR_ENCRYPT_ICON_H_GAP;
  }

  // button AV 10801 H.264 MPEGI
  x = NPROG_BAR_AV_ICON_X;
  for (i = 0; i < NPROG_BAR_AV_ICON_CNT; i++)
  {
    p_subctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_TEXT_TV+i,
                                x, NPROG_BAR_AV_ICON_Y,
                                NPROG_BAR_AV_ICON_W, NPROG_BAR_AV_ICON_H,
                                p_ctrl, 0);
	ctrl_set_rstyle(p_subctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
	
	text_set_align_type(p_subctrl, STL_CENTER |STL_VCENTER);
	text_set_font_style(p_subctrl, FSI_NPROG_BAR_NORMAL, FSI_NPROG_BAR_NORMAL,FSI_NPROG_BAR_NORMAL);
	text_set_content_type(p_subctrl, TEXT_STRTYPE_UNICODE);
	 x += NPROG_BAR_AV_ICON_W + NPROG_BAR_AV_ICON_H_GAP;
  }
/*
  //signal bar
  p_subctrl = ctrl_create_ctrl(CTRL_PBAR, IDC_SIGNAL_INTENSITY,
                              NPROG_BAR_SIGNAL_X, NPROG_BAR_SIGNAL_Y,
                              NPROG_BAR_SIGNAL_W, NPROG_BAR_SIGNAL_H,
                              p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_ITEM_10_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
  ctrl_set_mrect(p_subctrl, 2, 2, NPROG_BAR_SIGNAL_W - 2, NPROG_BAR_SIGNAL_H - 2 );
  pbar_set_direction(p_subctrl, 0);
  pbar_set_count(p_subctrl, 0, 100, 100);
  pbar_set_current(p_subctrl, 0);

  p_subctrl = ctrl_create_ctrl(CTRL_PBAR, IDC_SIGNAL_QUALITY,
                              NPROG_BAR_SIGNAL_X+NPROG_BAR_SIGNAL_W+NPROG_BAR_SIGNAL_H_GAP, NPROG_BAR_SIGNAL_Y,
                              NPROG_BAR_SIGNAL_W, NPROG_BAR_SIGNAL_H,
                              p_ctrl, 0);
  ctrl_set_rstyle(p_subctrl, RSI_ITEM_10_SH, RSI_ITEM_10_SH, RSI_ITEM_10_SH);
  ctrl_set_mrect(p_subctrl, 2, 2, NPROG_BAR_SIGNAL_W - 2, NPROG_BAR_SIGNAL_H - 2);
  pbar_set_direction(p_subctrl, 0);
  pbar_set_count(p_subctrl, 0, 100, 100);
  pbar_set_current(p_subctrl, 0);
  */
#endif
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  fill_ntime_info(p_cont, TRUE);
  fill_nprog_info(p_cont, prog_id);
  
#ifndef IMPL_NEW_EPG
  ui_set_epg_dy();
#endif
  tuner_id = ui_get_cur_prg_tuner_id();
  if(tuner_id != INVALIDID)
  {
      ui_enable_signal_monitor_by_tuner(TRUE, 0);
  }
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  manage_open_menu(ROOT_ID_FUN_HELP, 0, 0);

  if(ui_get_play_prg_type() == SAT_PRG_TYPE)
  {
    update_signal();
    if(is_satip_client == FALSE)
    {
    if(ui_is_playing() == FALSE)
       ui_stop_play(STOP_PLAY_BLACK, TRUE);
    ui_play_prog(prog_id, FALSE);
    }
    else
    {
      nprog_bar_change_pg_with_satip_client(prog_id);
    }
  }
  else if(ui_get_play_prg_type() == NET_PRG_TYPE)
  {
    if(is_satip_client == FALSE)
    {
      //on_pbar_net_prg_update(p_cont);
    if((ui_video_c_get_play_state() != MUL_PLAY_STATE_PLAY)
      || (para1 == V_KEY_UP) || (para1 == V_KEY_DOWN) ||(para1 == V_KEY_RECALL)||(para1 == V_KEY_BACK))
    {
      small_nprog_bar_open_dlg(IDS_LOAD_MEDIA, 0);
      if(ui_video_c_get_play_state() == MUL_PLAY_STATE_NONE)
        on_prog_bar_play_net_prg(NULL, 0, 0, 0);
      else
        ui_video_c_stop();
    }
  }
    else
    {
      OS_PRINTF("####%s, will close iptv before play satip####\n", __FUNCTION__);
      ui_set_play_prg_type(SAT_PRG_TYPE);
      ui_video_c_stop();
      update_signal();
      fill_nprog_info(p_cont, prog_id);
      nprog_bar_change_pg_with_satip_client(prog_id);
    }
  }
#ifndef IMPL_NEW_EPG
  ui_set_epg_dy();
#endif
  //ui_init_play_timer(ROOT_ID_PROG_BAR, MSG_MEDIA_PLAY_TMROUT, 200);
  return SUCCESS;
}

static BOOL time_inited = FALSE;
static BOOL key_cycle_finish = FALSE;

static u32 last_play_ticks;

static RET_CODE shift_prog_in_bar(control_t *p_ctrl, s16 offset)
{
  u16 prog_id;
  u32 cur_play_ticks = 0;

  // change prog
  if(ui_get_play_prg_type() == NET_PRG_TYPE)
  {
    //ui_shift_net_prog(offset);
    small_nprog_bar_open_dlg(IDS_LOAD_MEDIA, 0);
    if(ui_video_c_get_play_state() == MUL_PLAY_STATE_NONE)
      on_prog_bar_play_net_prg(NULL, 0, 0, 0);
    else
      ui_video_c_stop();
  }
  else
  {
  	cur_play_ticks = mtos_ticks_get();
  	if(key_cycle_finish == 0 && cur_play_ticks - last_play_ticks > 40)
  	{
	  	ui_shift_prog(offset, TRUE, &prog_id);
		if(time_inited == TRUE)
			fw_tmr_stop(ROOT_ID_PROG_BAR, MSG_PROGBAR_PLAY_PROGRAM);
		last_play_ticks = cur_play_ticks;
  	}
	else
	{
		ui_shift_prog(offset, FALSE, &prog_id);
		if(time_inited == FALSE)
		{
			fw_tmr_create(ROOT_ID_PROG_BAR, MSG_PROGBAR_PLAY_PROGRAM, 500, TRUE);
			time_inited = TRUE;
		}
		else
		{
			fw_tmr_start(ROOT_ID_PROG_BAR, MSG_PROGBAR_PLAY_PROGRAM);
			fw_tmr_reset(ROOT_ID_PROG_BAR, MSG_PROGBAR_PLAY_PROGRAM, 500);
		}
		key_cycle_finish = 1;
		last_play_ticks = cur_play_ticks;
	}
  }
  
  vid = VID_SYS_MAX;
  // update view
  fill_nprog_info(p_ctrl, prog_id);

  #ifdef ENABLE_ADS
  DEBUG(ADS,INFO,"send MSG_REFRESH_ADS_PIC to prog bar\n");
  fw_notify_root(p_ctrl, NOTIFY_T_MSG, TRUE, MSG_REFRESH_ADS_PIC, 0, 0);
  #endif  
  
  return SUCCESS;
}

static RET_CODE on_prog_bar_play_program(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	u16 prog_id;

	prog_id = sys_status_get_curn_group_curn_prog_id();
	ui_play_prog(prog_id, TRUE);

	fw_tmr_stop(ROOT_ID_PROG_BAR, MSG_PROGBAR_PLAY_PROGRAM);
	key_cycle_finish = 0;
	return SUCCESS;
}


static RET_CODE on_focus_up(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  // change prog
  return shift_prog_in_bar(p_ctrl, 1);
}


static RET_CODE on_focus_down(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  // change prog
  return shift_prog_in_bar(p_ctrl, -1);
}

static RET_CODE on_page_up(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  // change prog
  return shift_prog_in_bar(p_ctrl, -(SMALL_LIST_PAGE));
}

static RET_CODE on_page_down(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  // change prog
  return shift_prog_in_bar(p_ctrl, SMALL_LIST_PAGE);
}

static RET_CODE on_time_update(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
#ifdef IMPL_NEW_EPG
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();
  fill_nprog_info(p_ctrl,  prog_id);
#endif
  fill_ntime_info(p_ctrl, TRUE);
  return SUCCESS;
}

static RET_CODE on_recall(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u16 prog_id;

  if((ui_get_play_prg_type() == NET_PRG_TYPE)
    ||(ui_get_play_prg_type() == CAMERA_PRG_TYPE))
  {
    if((fw_get_focus_id() == ROOT_ID_BACKGROUND) ||
      (fw_get_focus_id() == ROOT_ID_PROG_BAR))
    {
//        prog_id = ui_iptv_get_prev_prg_pos();
 //       ui_iptv_set_prev_prg_pos(ui_iptv_get_cur_prg_pos());
 //       ui_iptv_set_cur_prg_pos(prog_id);
        fill_nprog_info(p_ctrl, prog_id);
        small_nprog_bar_open_dlg(IDS_LOAD_MEDIA, 0);
        ui_video_c_stop();
    }
    return SUCCESS;
  }

   /* do recall */
  if (ui_recall(TRUE, &prog_id))
  {
    UI_PRINTF("PROGBAR: recall is ok\n");
    // update view
    fill_nprog_info(p_ctrl, prog_id);
  }
  else
  {
    UI_PRINTF("PROGBAR: recall is failed\n");
  }
  return SUCCESS;
}

static RET_CODE on_tvradio(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u16 prog_id;

  if((ui_get_play_prg_type() == NET_PRG_TYPE)
    ||(ui_get_play_prg_type() == CAMERA_PRG_TYPE))
    return SUCCESS;
  
  /* do tvradio switch */
  if (ui_tvradio_switch(TRUE, &prog_id))
  {
    // update view
    fill_nprog_info(p_ctrl, prog_id);
  }
  else
  {
    BOOL is_tv = (BOOL)(sys_status_get_curn_prog_mode() == CURN_MODE_TV);
    u16 content = (u16)(is_tv ? IDS_MSG_NO_RADIO_PROG : IDS_MSG_NO_TV_PROG);
    ui_comm_cfmdlg_open(NULL, content, NULL, 0);
  }
  return SUCCESS;
}

static RET_CODE on_subt_ready(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_frm, *p_subctrl;
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();

  UI_PRINTF("on_subt_ready: prog id = %d, curn pg =%d \n", para1, prog_id);

  if (prog_id == (u16)para1)
  {
    p_frm = ctrl_get_child_by_id(p_ctrl, IDC_FRAME);
    p_subctrl = ctrl_get_child_by_id(p_frm , IDC_INFO_SUBTT);

    bmap_set_content_by_id(p_subctrl, IM_INFO_ICON_SUBTITLE);
    ctrl_paint_ctrl(p_subctrl, TRUE);
  }

  return SUCCESS;
}


static RET_CODE on_ttx_ready(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_frm, *p_subctrl;
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();

  UI_PRINTF("on_ttx_ready: prog id = %d, curn pg =%d \n", para1, prog_id);

  if (prog_id == (u16)para1)
  {
    p_frm = ctrl_get_child_by_id(p_ctrl, IDC_FRAME);
    p_subctrl = ctrl_get_child_by_id(p_frm , IDC_INFO_TELTEXT);

    bmap_set_content_by_id(p_subctrl, IM_INFO_ICON_TTX);
    ctrl_paint_ctrl(p_subctrl, TRUE);
  }

  return SUCCESS;
}

static RET_CODE on_pbar_destory(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  u32 tuner_id = 0;
  tuner_id = ui_get_cur_prg_tuner_id();
  if(tuner_id != INVALIDID)
  {
      ui_enable_signal_monitor_by_tuner(FALSE, 0);
  }
  sys_status_save();
  if ( time_inited)
    fw_tmr_destroy(ROOT_ID_PROG_BAR, MSG_PROGBAR_PLAY_PROGRAM);
  time_inited= FALSE;
  //ui_deinit_play_timer();
#ifdef ENABLE_CA
	cas_manage_super_osd_repaint();
	OS_PRINTF("function :%s ,line:%d,repaint_super_osd_again\n",__FUNCTION__,__LINE__);
#endif
  return ERR_NOFEATURE;
}

static RET_CODE on_prog_bar_detail(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{

  RET_CODE ret = SUCCESS;
  dvbs_prog_node_t pg;
  #ifndef IMPL_NEW_EPG
  epg_filter_t epg_info = {0};
  evt_node_t evt_node = {0};
  evt_node_t *p_evt_node = &evt_node;
  u32 item_num = 0;
  #else
  epg_prog_info_t prog_info = {0};
  event_node_t   *p_present_evt = NULL;
  event_node_t   *p_follow_evt = NULL;
  #endif
  u16 pg_id;

  pg_id = sys_status_get_curn_group_curn_prog_id();

   if (db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
  {
    UI_PRINTF("PROGBAR: can NOT get pg!\n");
    return 0;
  }
#ifndef IMPL_NEW_EPG
  epg_info.service_id = (u16)pg.s_id;
  epg_info.stream_id = (u16)pg.ts_id;
  epg_info.orig_network_id = (u16)pg.orig_net_id;
  epg_info.cont_level = 0;

   p_evt_node = epg_data_get_evt(class_get_handle_by_id(EPG_CLASS_ID),
    &epg_info, &item_num);

  if(p_evt_node != NULL)
  {
    ret = manage_open_menu(ROOT_ID_PROG_DETAIL, (u32)p_evt_node, pg_id);
  }
#else
  prog_info.network_id = (u16)pg.orig_net_id;
  prog_info.ts_id      = (u16)pg.ts_id;
  prog_info.svc_id     = (u16)pg.s_id;

  mul_epg_get_pf_event(&prog_info, &p_present_evt, &p_follow_evt);
  if(p_present_evt != NULL)
  {
    ret = manage_open_menu(ROOT_ID_PROG_DETAIL, (u32)p_present_evt, pg_id);
  }
#endif
  return ret;
}

static RET_CODE on_prog_bar_video_format(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{
	control_t *p_video_format;
	u8 asc_buf[16];
	vid  = (disp_sys_t)para1;
	get_vid_format_content(vid, asc_buf);
	p_video_format = ctrl_get_child_by_id(ctrl_get_child_by_id(p_ctrl,IDC_FRAME), IDC_TEXT_TV);
	text_set_content_by_ascstr(p_video_format, asc_buf);
	ctrl_paint_ctrl(p_video_format, TRUE);
	return SUCCESS;
}



  //ui_video_c_load_media_from_net(p_url_info->p_url[cur_prg_source_index], 0);

static RET_CODE on_prog_bar_load_media_success(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{
  ui_comm_dlg_close();

  return SUCCESS;
}

  


static RET_CODE on_prog_bar_load_media_error(control_t *p_ctrl, u16 msg,
                                    u32 para1, u32 para2)
{
#if 0
  iptv_url_msg_info_t p_cur_channel_info = {0};
  u8 cur_prg_source_index = 0;

  ui_iptv_get_cur_channel_info(&p_cur_channel_info);
  cur_prg_source_index = ui_iptv_get_cur_prg_source_index(ui_iptv_get_cur_prg_pos());
  cur_prg_source_index ++;
  if(cur_prg_source_index >= p_cur_channel_info.url_count)
  {
    cur_prg_source_index = 0;
    ui_iptv_set_cur_prg_source_index(ui_iptv_get_cur_prg_pos(), cur_prg_source_index);
    small_nprog_bar_open_dlg(IDS_PLAY_NEXT_NETWORK_PRG, 0);
    on_prog_bar_net_play_next_prg(p_ctrl);
    return SUCCESS;
  }

  ui_iptv_set_cur_prg_source_index(ui_iptv_get_cur_prg_pos(), cur_prg_source_index);

  if(p_cur_channel_info.p_url[cur_prg_source_index] != NULL)
  {
    ui_video_c_stop();
    small_nprog_bar_open_dlg(IDS_SWITCH_MEDIA_SOURCE, 0);
    return SUCCESS;
  }

  small_nprog_bar_open_dlg(IDS_LOAD_MEDIA_ERROR, 0);
  #endif
  return SUCCESS;
}

static RET_CODE on_prog_bar_change_aspect(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  av_set_t av_set;
  class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
  sys_status_get_av_set(&av_set);

  if(1 == av_set.tv_ratio)
  {
    av_set.tv_ratio = 2;
  }
  else
  {
    av_set.tv_ratio = 1;
  } 
  avc_set_video_aspect_mode_1(avc_handle, sys_status_get_video_aspect(av_set.tv_ratio));

  sys_status_set_av_set(&av_set);
  sys_status_save();
  return SUCCESS;
}

#ifdef IMPL_NEW_EPG
static RET_CODE on_pbar_update_event(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  u16 prog_id = sys_status_get_curn_group_curn_prog_id();
  fill_nprog_info(p_cont, prog_id);

  return SUCCESS;
}
#endif

#ifdef ENABLE_ADS
static RET_CODE on_item_refresh_ads(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  DEBUG(ADS,INFO,"on_item_refresh_ads\n");
  #ifdef ADS_DIVI_SUPPORT
  if(FALSE == ui_get_ads_osd_roll_over())
  {
	  divi_ads_update_osd_show(FALSE);
  }
  #endif
  return SUCCESS;
}

static RET_CODE on_picture_draw_end(control_t *p_cont, u16 msg,u32 para1, u32 para2)
{
  return SUCCESS;
}
#endif

static RET_CODE on_nprog_bar_exit(control_t *p_cont, u16 msg,u32 para1, u32 para2)
{
	#ifdef ENABLE_ADS
	pic_adv_stop();
	#endif
	manage_close_menu(ROOT_ID_PROG_BAR, 0, 0);
	return SUCCESS;
}



BEGIN_KEYMAP(nprog_bar_cont_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_RECALL, MSG_RECALL)
  ON_EVENT(V_KEY_BACK, MSG_RECALL)
  ON_EVENT(V_KEY_TVRADIO, MSG_TVRADIO)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_YELLOW, MSG_CHANGE_ASPECT)
END_KEYMAP(nprog_bar_cont_keymap, NULL)

BEGIN_MSGPROC(nprog_bar_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT, on_nprog_bar_exit)
  ON_COMMAND(MSG_FOCUS_UP, on_focus_up)
  ON_COMMAND(MSG_FOCUS_DOWN, on_focus_down)
  ON_COMMAND(MSG_PAGE_UP, on_page_up)
  ON_COMMAND(MSG_PAGE_DOWN, on_page_down)
  ON_COMMAND(MSG_TIME_UPDATE, on_time_update)
  ON_COMMAND(MSG_RECALL, on_recall)
  ON_COMMAND(MSG_TVRADIO, on_tvradio)
  ON_COMMAND(MSG_TTX_READY, on_ttx_ready)
  ON_COMMAND(MSG_SUBT_READY, on_subt_ready)
//  ON_COMMAND(MSG_SIGNAL_UPDATE, on_pbar_signal_update)
  ON_COMMAND(MSG_DESTROY, on_pbar_destory)
  ON_COMMAND(MSG_INFO, on_prog_bar_detail)
  ON_COMMAND(MSG_NOTIFY_VIDEO_FORMAT, on_prog_bar_video_format)
  ON_COMMAND(MSG_VIDEO_EVENT_STOP_CFM, on_prog_bar_play_net_prg)
  //ON_COMMAND(MSG_IPTV_URL_GET_SUCCESS, on_nprog_bar_get_url_success)
  ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, on_prog_bar_load_media_success)
  ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR, on_prog_bar_load_media_error)
  ON_COMMAND(MSG_CHANGE_ASPECT, on_prog_bar_change_aspect)
  ON_COMMAND(MSG_PROGBAR_PLAY_PROGRAM, on_prog_bar_play_program)
#ifdef IMPL_NEW_EPG
  ON_COMMAND(MSG_PF_READY, on_pbar_update_event)
#endif

#ifdef ENABLE_ADS
  ON_COMMAND(MSG_REFRESH_ADS_PIC, on_item_refresh_ads)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_picture_draw_end)
#endif
END_MSGPROC(nprog_bar_cont_proc, ui_comm_root_proc)
