#include "ui_common.h"
#include "ui_xextend.h"
#include "ui_audio_set.h"
//#include "ui_pvr_play_bar.h"
#include "ui_pause.h"
#include "ui_mute.h"
#include "ui_fav_list.h"
#include "ui_vepg.h"
#include "ui_do_search.h"


typedef enum{
	DESKTOP_EXTEND,
	PROG_BAR_EXTEND,
	PROG_LIST_EXTEND,
}menu_type_t;

enum{
	IDC_MAINCONT= 1,
	IDC_LIST_CONT1,
	IDC_LEFT1_BMP1,
	IDC_LEFT1_BMP2,
};

typedef enum{
	TEXT_BTN,
	SELECT_BTN,

	UNKNOWN_CTRL_TYPE,
}btn_type_t;

typedef struct
{
  dvbs_prog_node_t program;
//  pvr_audio_info_t record;
  ui_audio_set_type_e type;
}ui_audio_set_t;

static u16 ids_audio_mode[4] = {IDS_AUDIO_MODE_STEREO,IDS_AUDIO_MODE_LEFT, IDS_AUDIO_MODE_RIGHT, IDS_AUDIO_MODE_MONO};
static ui_audio_set_t _audio_set;
static int if_delete_program_all = 0;

//static BOOL love_mark_status;

 
extern control_t *get_sleep_timer_switch_ctrl(void);
u16 extend_pwd_keymap(u16 key);
RET_CODE extend_pwd_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 extend_cont1_keymp(u16 key);
RET_CODE extend_cont1_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE extend_comm_cbox_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE extend_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
u16 extend_root_cont_keymap(u16 key);
RET_CODE extend_root_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE extend_change_focus1_manage(control_t *p_old_btn, control_t *p_new_btn, u16 msg);
static RET_CODE exit_extend(control_t *p_cont1, u16 msg, u32 para1, u32 para2);


control_t *get_extend_cont1_ctrl(void)
{
	control_t *p_ctrl;
	
	p_ctrl = fw_find_root_by_id(ROOT_ID_XEXTEND);
	if(NULL==p_ctrl)
		return NULL;
	p_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_MAINCONT);			
	p_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_LIST_CONT1);

	return p_ctrl;
}

static RET_CODE create_timer_for_close(void)
{
	return fw_tmr_create(ROOT_ID_XEXTEND, MSG_TIMER_ON, 10000, FALSE);
}

static RET_CODE reset_timer_for_close(void)
{
	return fw_tmr_reset(ROOT_ID_XEXTEND, MSG_TIMER_ON, 10000);
}

static RET_CODE destroy_timer_for_close(void)
{
	return fw_tmr_destroy(ROOT_ID_XEXTEND, MSG_TIMER_ON);
}

static RET_CODE reset_timer_from_other(control_t *p_cont1, u16 msg, u32 para1, u32 para2)
{
	return reset_timer_for_close();
}

static RET_CODE stop_timer_from_other(control_t *p_cont1, u16 msg, u32 para1, u32 para2)
{
	return fw_tmr_stop(ROOT_ID_XEXTEND, MSG_TIMER_ON);
}

static RET_CODE start_timer_from_other(control_t *p_cont1, u16 msg, u32 para1, u32 para2)
{
	return fw_tmr_start(ROOT_ID_XEXTEND, MSG_TIMER_ON);
}

RET_CODE reset_extend_timer(void)
{
	control_t *p_ctrl;
	p_ctrl = get_extend_cont1_ctrl();
	ctrl_process_msg(p_ctrl, MSG_XEXTEND_RESET_TIMER, 0, 0);//reset extend timer
	return SUCCESS;
}

RET_CODE stop_extend_timer(void)
{
	control_t *p_ctrl;
	p_ctrl = get_extend_cont1_ctrl();
	ctrl_process_msg(p_ctrl, MSG_XEXTEND_STOP_TIMER, 0, 0);//reset extend timer
	return SUCCESS;
}

RET_CODE start_extend_timer(void)
{
	control_t *p_ctrl;
	p_ctrl = get_extend_cont1_ctrl();
	ctrl_process_msg(p_ctrl, MSG_XEXTEND_START_TIMER, 0, 0);//reset extend timer
	return SUCCESS;
}

static ui_audio_set_t *ui_audio_set_get_private_data()
{
  return &_audio_set;
}
#if 0
static BOOL ui_audio_set_record_is_same(pvr_audio_info_t *p_rec_info)
{
  ui_audio_set_t *private_data = ui_audio_set_get_private_data();
  if((private_data->type == AUDIO_SET_PLAY_PROGRAM)
    || (private_data->type == AUDIO_SET_PLAY_NULL))
  {
    return FALSE;
  }
  if(memcmp(private_data->record.program_name, p_rec_info->program_name, sizeof(u16) *
            PROGRAM_NAME_MAX) != 0)
  {
    return FALSE;
  }
  if((private_data->record.total_size != p_rec_info->total_size)
    || (private_data->record.total_time != p_rec_info->total_time))
  {
    return FALSE;
  }
  return TRUE;
}
#endif
static RET_CODE ui_audio_set_c_get_data(u32 para1, u32 para2)
{
    u16 prog_id;
    ui_audio_set_t *private_data = ui_audio_set_get_private_data();  
    if((prog_id = sys_status_get_curn_group_curn_prog_id()) == INVALIDID)
    {
    	DEBUG(MAIN, ERR, "((prog_id = sys_status_get_curn_group_curn_prog_id()) == INVALIDID)\n");
      return ERR_FAILURE;
    }
    if(db_dvbs_get_pg_by_id(prog_id, &(private_data->program)) != DB_DVBS_OK)
    {
    	DEBUG(MAIN, ERR, "(db_dvbs_get_pg_by_id(prog_id, &(private_data->program)) != DB_DVBS_OK)");
      return ERR_FAILURE;
    }
    private_data->type = AUDIO_SET_PLAY_PROGRAM;
  return SUCCESS;
}

#ifdef VOLUME_CHANNEL_ON
static RET_CODE ui_fill_track_info(control_t *ctrl,
                                                u16 focus,
                                                u16 *p_str,
                         u16 max_length)
{
  u8 asc_buf[20] = {0};
  u16 uni_str[12] = {0};
  const u8 *p_language = NULL;
  u16 curn_aid, audio_type, len = 0;
  ui_audio_set_t *private_data = ui_audio_set_get_private_data();
  #ifdef PLAY_RECORD_VOLUME_CHANNEL_EN
  if(private_data->type == AUDIO_SET_PLAY_RECORD)
  {
    p_language = iso_639_2_idx_to_desc(private_data->record.audio[focus].language_index);
    curn_aid = private_data->record.audio[focus].p_id;
    audio_type = private_data->record.audio[focus].type;
  }
  else
  #endif
  {
    p_language = iso_639_2_idx_to_desc(private_data->program.audio[focus].language_index);
    curn_aid = private_data->program.audio[focus].p_id;
    audio_type = private_data->program.audio[focus].type;
  }

  if(memcmp("udf", p_language, 3) == 0)
  {
    switch(audio_type)
    {
      case AUDIO_MP1:
	  	sprintf(asc_buf, "MP1");
        break;
      case AUDIO_MP2:
	  	sprintf(asc_buf, "MP2");
        break;
      case AUDIO_MP3:
	  	sprintf(asc_buf, "MP3");
		break;
      case AUDIO_AC3_VSB:
        sprintf(asc_buf, "AC3");
        break;
      case AUDIO_EAC3:
        sprintf(asc_buf, "AC3+");
        break;
	  case AUDIO_DRA:
		sprintf(asc_buf, "DRA");
		break; 	
      default:
        sprintf(asc_buf, "snd%d", focus + 1);
        break;
    }
    gui_get_string(IDS_AUDIO_TRACK_X, p_str, max_length);
    
    len = (u16)uni_strlen(p_str);
    MT_ASSERT(len < max_length - 2);
  }
  else
  {
    
    switch(audio_type)
    {
      case AUDIO_MP1:
	  	sprintf(asc_buf, "MPG1_");
		memcpy(&asc_buf[strlen(asc_buf)], p_language, LANGUAGE_LEN);
        break;
      case AUDIO_MP2:
	  	sprintf(asc_buf, "MPG2_");
		memcpy(&asc_buf[strlen(asc_buf)], p_language, LANGUAGE_LEN);
        break;
      case AUDIO_MP3:
	  	sprintf(asc_buf, "MPG3_");
		memcpy(&asc_buf[strlen(asc_buf)], p_language, LANGUAGE_LEN);
        break;		
      case AUDIO_AC3_VSB:
        sprintf(asc_buf, "AC3_");
		memcpy(&asc_buf[strlen(asc_buf)], p_language, LANGUAGE_LEN);
        break;
      case AUDIO_EAC3:
        sprintf(asc_buf, "AC3+_");
		memcpy(&asc_buf[strlen(asc_buf)], p_language, LANGUAGE_LEN);
        break;
	  case AUDIO_DRA:
        sprintf(asc_buf, "DRA_");
		memcpy(&asc_buf[strlen(asc_buf)], p_language, LANGUAGE_LEN);
        break;	  	
      default:
	  	memcpy(asc_buf, p_language, LANGUAGE_LEN);
        sprintf(&asc_buf[strlen(asc_buf)], "%d", focus + 1);
        break;
    }
  }
  
  gui_get_string(IDS_AUDIO_TRACK, p_str, max_length);
  str_asc2uni(asc_buf, uni_str);
  uni_strcat(p_str, uni_str, max_length);

  return SUCCESS;
}
#endif

static BOOL ui_audio_set_program_is_ac3_bsout(u8 channel)
{
  av_set_t av_set = {0};
  ui_audio_set_t *private_data = ui_audio_set_get_private_data();
  sys_status_get_av_set(&av_set);
  if((1 == av_set.digital_audio_output)
    && ((AUDIO_AC3_VSB == private_data->program.audio[channel].type)
       || (AUDIO_EAC3 == private_data->program.audio[channel].type)))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#ifdef VOLUME_CHANNEL_ON
static RET_CODE ui_audio_set_program_set_channel(u8 channel)
{
  cmd_t cmd;
  u16 pg_id;
  dvbs_prog_node_t pg_node = {0};
  av_set_t av_set = {0};

  pg_id = sys_status_get_curn_group_curn_prog_id();
  MT_ASSERT(pg_id != INVALIDID);

  db_dvbs_get_pg_by_id(pg_id, &pg_node);

  //if set DIGITAL_OUTPUT as BS out,  change audio type
  sys_status_get_av_set(&av_set);

  DEBUG(MAIN,INFO,"digital_audio_output=%d channel=%d type=%d\n",av_set.digital_audio_output,channel,pg_node.audio[channel].type);
  if((1 == av_set.digital_audio_output)
    && ((AUDIO_AC3_VSB == pg_node.audio[channel].type)
       || (AUDIO_EAC3 == pg_node.audio[channel].type)))
  {
    cmd.id = PB_CMD_SWITCH_AUDIO_CHANNEL;
    cmd.data1 = pg_node.audio[channel].p_id;
    cmd.data2 = AUDIO_EAC3;
    ap_frm_do_command(APP_PLAYBACK, &cmd);
  }
  else
  {
    cmd.id = PB_CMD_SWITCH_AUDIO_CHANNEL;
    cmd.data1 = pg_node.audio[channel].p_id;
    cmd.data2 = pg_node.audio[channel].type;
    ap_frm_do_command(APP_PLAYBACK, &cmd);
  }
  
  return SUCCESS;
}
#endif

static BOOL ui_audio_set_program_is_modify(control_t *p_cont1)
{
  dvbs_prog_node_t pg;
  BOOL is_modify;
  control_t *p_ctrl;
  ui_audio_set_t *private_data = ui_audio_set_get_private_data();

  memcpy(&pg, &(private_data->program), sizeof(dvbs_prog_node_t));

  #ifdef VOLUME_CHANNEL_ON
  p_ctrl = ctrl_get_child_by_id(p_cont1, IDC_XEXTEND_CHANNEL);
  pg.audio_channel = cbox_static_get_focus(p_ctrl);
  #endif

  p_ctrl = ctrl_get_child_by_id(p_cont1, IDC_XEXTEND_AUDIO_TRACK);
  pg.audio_track = cbox_static_get_focus(p_ctrl);

  is_modify = memcmp(&pg, &(private_data->program), sizeof(dvbs_prog_node_t));
  if (is_modify)
  {
    if(pg.audio_channel != private_data->program.audio_channel)
    {
	  	pg.is_audio_channel_modify = TRUE;
    }
  else
    {
	  	pg.is_audio_channel_modify = FALSE;
    }
    memcpy(&(private_data->program), &pg, sizeof(dvbs_prog_node_t));
  }

  return is_modify;
}

#if 0
static RET_CODE ui_audio_set_record_set_channel(u8 channel)
{
  u8 type = 0;
  u16 pid;
  ui_audio_set_t *private_data = ui_audio_set_get_private_data();
  pid = private_data->record.audio[channel].p_id;
  type = (u8)private_data->record.audio[channel].type;
  ts_player_change_audio(pid, type);
  return SUCCESS;
}
#endif

static BOOL ui_audio_set_program_save_data(control_t *p_cont1)
{
  ui_audio_set_t *private_data = ui_audio_set_get_private_data();
  if((private_data->type == AUDIO_SET_PLAY_PROGRAM)
    && ui_audio_set_program_is_modify(p_cont1))
  {
    db_dvbs_edit_program(&(private_data->program));
    db_dvbs_save_pg_edit(&(private_data->program));
    return TRUE;
  }

  return FALSE;
}

static BOOL ui_audio_set_record_exit(void)
{
  ui_audio_set_t *private_data = ui_audio_set_get_private_data();
  if(private_data->type == AUDIO_SET_PLAY_RECORD)
  {
    return TRUE;
  }
  return FALSE;
}

static RET_CODE ui_audio_set_key_left_right(control_t *p_btn, u16 msg, u32 para1, u32 para2)
{
	RET_CODE ret = SUCCESS;
	#ifdef VOLUME_CHANNEL_ON
	control_t *p_btn_track = NULL;
	u8 channel_change = 0;
	#endif
	ui_audio_set_t *private_data = ui_audio_set_get_private_data();

	if (cbox_static_get_count(p_btn) < 2)
	{
		return SUCCESS;
	}
	/* before switch */
	if (ui_is_mute())
	{
		ui_set_mute(FALSE);
	}
	if(MSG_FOCUS_LEFT==msg)
		msg = MSG_DECREASE;
	else if(MSG_FOCUS_RIGHT==msg)
		msg = MSG_INCREASE;
	ret = cbox_class_proc(p_btn, msg, para1, para2);

	if(!ui_is_playing()
		&& (private_data->type == AUDIO_SET_PLAY_PROGRAM))
	{
		/* just change value */
		return SUCCESS;
	}

	/* after switch */
	switch (ctrl_get_ctrl_id(p_btn))
	{
	  #ifdef VOLUME_CHANNEL_ON
	  case IDC_XEXTEND_CHANNEL:
		channel_change = (u8)cbox_static_get_focus(p_btn);
		p_btn_track = ctrl_get_child_by_id(ctrl_get_parent(p_btn),IDC_XEXTEND_AUDIO_TRACK);
		#ifdef PLAY_RECORD_VOLUME_CHANNEL_EN
		if(private_data->type == AUDIO_SET_PLAY_RECORD)
		{
			ui_audio_set_record_set_channel(channel_change);
			private_data->record.audio_channel = channel_change;
		}
		else
		#endif
		{
			ui_audio_set_program_set_channel(channel_change);
			if(ui_audio_set_program_is_ac3_bsout(channel_change))
			{
				ctrl_set_attr(p_btn_track, OBJ_ATTR_INACTIVE);
			}
			else
			{
				ctrl_set_attr(p_btn_track, OBJ_ATTR_ACTIVE);
			}
			ctrl_paint_ctrl(ctrl_get_parent(p_btn), TRUE);
		}
		break;
		#endif
	case IDC_XEXTEND_AUDIO_TRACK:
		avc_set_audio_mode_1(class_get_handle_by_id(AVC_CLASS_ID), cbox_static_get_focus(p_btn));
		break;
	default:
		/* do nothing */;
		break;
	}  

	return ret;
}

static void _set_mute_btn_state(control_t *p_btn)
{
	control_t *p_bmp;
	p_bmp = ctrl_get_child_by_id(p_btn, IDC_LEFT1_BMP2);
	if(ui_is_mute())
	{
		text_set_content_by_strid(p_btn, IDS_UNMUTE);
		bmap_set_content_by_id(p_bmp, IM_XMUTE_S);
	}
	else
	{
		text_set_content_by_strid(p_btn, IDS_MUTE);
		bmap_set_content_by_id(p_bmp, IM_XVOLUME_HORN);
	}

}

static void _set_puse_btn_state(control_t *p_btn)
{
	control_t *p_bmp;
	p_bmp = ctrl_get_child_by_id(p_btn, IDC_LEFT1_BMP2);
	if(ui_is_pause())
	{
		text_set_content_by_strid(p_btn, IDS_PLAY);
		bmap_set_content_by_id(p_bmp, IM_XIC_PLAY_2);
	}
	else
	{
		text_set_content_by_strid(p_btn, IDS_PAUSE);
		bmap_set_content_by_id(p_bmp, IM_XIC_PAUSE);
	}
}

static void _set_btn_content(control_t *p_btn, dvbs_element_mark_digit dig)
{
	u8 view_id;
	u16 pg_id;
	u16 pos;
	u16 normal_strid, cancel_strid;
	u16 normal_bmp, cancel_bmp;
	control_t *p_bmp;
	p_bmp = ctrl_get_child_by_id(p_btn, IDC_LEFT1_BMP2);
	pg_id = sys_status_get_curn_group_curn_prog_id();
	view_id = ui_dbase_get_pg_view_id();
	pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);
	switch(dig)
	{
	case DB_DVBS_FAV_GRP:
		normal_strid = IDS_ADD_TO_FAV_LIST;
		cancel_strid = IDS_DELE_FROM_FAV_LIST;
		normal_bmp = IM_XTV_FAV;
		cancel_bmp = 0;
		break;
	case DB_DVBS_MARK_SKP:
		normal_strid = IDS_SKIP;
		cancel_strid = IDS_UNSKIP;
		normal_bmp = IM_XTV_SKIP;
		cancel_bmp = 0;
		break;
	case DB_DVBS_MARK_LCK:
		normal_strid = IDS_LOCK;
		cancel_strid = IDS_UNLOCK;
		normal_bmp = IM_IC_SET_LOCK;
		cancel_bmp = IM_IC_SET_UNLOCK;
		break;
	default:
		return;
		break;
	}
	if( db_dvbs_get_mark_status(view_id, pos, dig, 0)== TRUE)
	{
		text_set_content_by_strid(p_btn, cancel_strid);
		bmap_set_content_by_id(p_bmp, cancel_bmp);
	}
	else
	{
		text_set_content_by_strid(p_btn, normal_strid);
		bmap_set_content_by_id(p_bmp, normal_bmp);
	}

}

static void set_btn_state(control_t *p_btn)
{
	if(NULL==p_btn)
		return;
	switch(p_btn->id)
	{
	case IDC_XEXTEND_PAUSE:
		_set_puse_btn_state(p_btn);
		break;
	case IDC_XEXTEND_MUTE:
		_set_mute_btn_state(p_btn);
		break;
	case IDC_XEXTEND_ADD_FAV_DESKTOP:
	case IDC_XEXTEND_ADD_TO_FAV:
		_set_btn_content(p_btn, DB_DVBS_FAV_GRP);
		break;
	case IDC_XEXTEND_LOCK:
		_set_btn_content(p_btn, DB_DVBS_MARK_LCK);
		break;
	case IDC_XEXTEND_SKIP:
		_set_btn_content(p_btn, DB_DVBS_MARK_SKP);
		break;
	default:
		break;
	}
	ctrl_paint_ctrl(p_btn, TRUE);
}
static RET_CODE extend_pwd_open(void)
{
	comm_pwdlg_data_t pwd =
	{
		ROOT_ID_XEXTEND,
		COMM_DLG_X,		COMM_DLG_Y,
		IDS_MSG_INPUT_PASSWORD,
		extend_pwd_keymap,
		extend_pwd_proc,
	};
	if(ui_comm_pwdlg_open(&pwd))
		return SUCCESS;
	else 
		return ERR_FAILURE;
}

static RET_CODE update_event_state(control_t *p_btn, dvbs_element_mark_digit dig)
{
	u8 view_id;
	u16 pg_id;
	u16 pos;
	control_t *p_bmp;
	control_t *p_small_list;

	p_bmp = ctrl_get_child_by_id(p_btn, IDC_LEFT1_BMP2);
	pg_id = sys_status_get_curn_group_curn_prog_id();
	view_id = ui_dbase_get_pg_view_id();
	pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);
	if (if_delete_program_all == 1 && dig == DB_DVBS_DEL_FLAG)
	{
		u16 total = db_dvbs_get_count(view_id);
		int i = 0;
		for(i = 0;i < total;i++)
		{
			OS_PRINTF("[test::pos = %d %s %d]\n",i,__func__,__LINE__);
			db_dvbs_change_mark_status(view_id, i, DB_DVBS_DEL_FLAG, DB_DVBS_PARAM_ACTIVE_FLAG);
		}
	}
	else
	{
		if( db_dvbs_get_mark_status(view_id, pos, dig, 0)== TRUE)
		{
			db_dvbs_change_mark_status(view_id, pos, dig, 0);
		}
		else
		{
			db_dvbs_change_mark_status(view_id, pos, dig, DB_DVBS_PARAM_ACTIVE_FLAG);
		}
	}
	p_small_list = get_small_list_ctrl();
	if(NULL!=p_small_list)
		ctrl_process_msg(p_small_list, MSG_SLIST_SAVE, 0, 0);//save modified 
	else
		db_dvbs_save(view_id);
	set_btn_state(p_btn);
	
	if(NULL==p_small_list)
		return ERR_FAILURE;

	switch(dig)
	{
		case DB_DVBS_MARK_LCK:
			ctrl_process_msg(p_small_list, MSG_PLAY_IN_SLIST, (u32)PLAY_IN_SLIST_FORCE, 0);//replay
			break;
		case DB_DVBS_FAV_GRP:
			if(SLIST_PG_FAV==get_slist_prog_type())
			{
				ctrl_process_msg(p_small_list, MSG_SMALLIST_UPDATE, MSG_REMOVE_FROM_FAV, 0);//update small list after delete program from fav list
				exit_extend(NULL, 0, 0, 0);
			}
			break;
		case DB_DVBS_DEL_FLAG:
			if (if_delete_program_all == 0)
				ctrl_process_msg(p_small_list, MSG_SMALLIST_UPDATE, MSG_DEL_PG_FROM_VIEW, 0);//update small list after delete program
			else
				ctrl_process_msg(p_small_list, MSG_SMALLIST_UPDATE, MSG_DELALL_PG_FROM_VIEW, 0);//update small list after delete program
			exit_extend(NULL, 0, 0, 0);
			
			break;
		default:
			break;
	}

	return SUCCESS;
}

static RET_CODE on_extend_pwd_close(control_t *p_pwd, u16 msg, u32 para1, u32 para2)
{
	control_t *p_ctrl;
	if(NULL!= fw_find_root_by_id(ROOT_ID_PASSWORD))
		ui_comm_pwdlg_close();

	p_ctrl = get_extend_cont1_ctrl();
	ctrl_paint_ctrl(p_ctrl, TRUE);
	return SUCCESS;
}

static RET_CODE extend_pwd_correct(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_btn, *p_cont;
	if_delete_program_all = 0;
	on_extend_pwd_close(p_ctrl, msg, para1, para2);
 	p_cont = get_extend_cont1_ctrl();
	p_btn = ctrl_get_active_ctrl(p_cont);
	if(NULL==p_btn)
		return ERR_FAILURE;

	switch(p_btn->id)
	{
		case IDC_XEXTEND_LOCK:
			return update_event_state(p_btn,DB_DVBS_MARK_LCK);
		case IDC_XEXTEND_DELETE:
			return update_event_state(p_btn,DB_DVBS_DEL_FLAG);
#ifdef XEXTEND_SMLIST_DELETE_ALL
		case IDC_XEXTEND_DELETE_ALL:
			if_delete_program_all = 1;
			return update_event_state(p_btn,DB_DVBS_DEL_FLAG);
#endif
		default:
			break;
	}	
	return SUCCESS;
}

static RET_CODE send_msg_to_desktop(control_t *p_btn, u16 msg, u32 para1, u32 para2)
{
	control_t *p_bmp;

	p_bmp = ctrl_get_child_by_id(p_btn, IDC_LEFT1_BMP2);
	manage_notify_root(ROOT_ID_BACKGROUND, msg, para1, para2);
	switch(msg)
	{
	case MSG_PAUSE:
	case MSG_MUTE:
		set_btn_state(p_btn);
		break;
	}
	return SUCCESS;
}

static RET_CODE send_msg_to_small_list(control_t *p_btn, u16 msg)
{
	control_t *p_ctrl;

	p_ctrl = get_small_list_ctrl();
	if(NULL!=p_ctrl)
		ctrl_process_msg(p_ctrl, msg, 0,0);
	if(NULL!=p_btn)
		ctrl_paint_ctrl(p_btn, FALSE);
	return SUCCESS;
}

static menu_type_t get_menu_type(void)
{
	control_t *p_bar_root = NULL;
	control_t *p_list_root = NULL;
	
	p_bar_root = fw_find_root_by_id(ROOT_ID_PROG_BAR);
	p_list_root = fw_find_root_by_id(ROOT_ID_SMALL_LIST);
	if((NULL==p_bar_root)&&(NULL==p_list_root))
		return DESKTOP_EXTEND;
	else if(NULL!=p_bar_root)
		return PROG_BAR_EXTEND;
	else 
		return PROG_LIST_EXTEND;
}
static RET_CODE exit_extend(control_t *p_cont1, u16 msg, u32 para1, u32 para2)
{
	ui_audio_set_t *private_data;
	u16 pg_id;
	u8 view_id;
	u16 pos;
	control_t *p_ctrl;
//	BOOL new_love_mark_status;

	if(NULL==p_cont1)
		p_cont1 = get_extend_cont1_ctrl();
	if(DESKTOP_EXTEND == get_menu_type())
	{
		private_data = ui_audio_set_get_private_data();
		if(private_data->type == AUDIO_SET_PLAY_PROGRAM)
		{
		  ui_audio_set_program_save_data(p_cont1);
		}
		else
		{
		  ui_audio_set_record_exit();
		}
	}
	if(NULL!=fw_find_root_by_id(ROOT_ID_VEPG))
		manage_close_menu(ROOT_ID_VEPG, 0,0);
	destroy_timer_for_close();
	manage_close_menu(ROOT_ID_XEXTEND, 0,0);

	if(NULL!=fw_find_root_by_id(ROOT_ID_SMALL_LIST))
	{
		pg_id = sys_status_get_curn_group_curn_prog_id();
		view_id = ui_dbase_get_pg_view_id();
		pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);
		p_ctrl = get_small_list_ctrl();
		if(NULL==p_ctrl)
			return ERR_FAILURE;
		if(TRUE==db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, 0))
		{
			if(ui_is_prog_locked(pg_id))
				ctrl_process_msg(p_ctrl, MSG_PLAY_IN_SLIST, PLAY_IN_SLIST_FORCE, 0);//if program lock, play program in small list 
		}
		send_msg_to_small_list(NULL, MSG_GETFOCUS);
		ctrl_paint_ctrl(p_ctrl, TRUE);/*
		p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_SMALL_LIST_SAT);
		ctrl_paint_ctrl(p_ctrl, TRUE);
		p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_SMALL_LIST_ARROWL);
		ctrl_paint_ctrl(p_ctrl, TRUE);
		p_ctrl = ctrl_get_child_by_id(p_ctrl->p_parent, IDC_SMALL_LIST_ARROWR);
		ctrl_paint_ctrl(p_ctrl, TRUE);*/
	}
	return SUCCESS;
}
static RET_CODE extend_open_epg(control_t *p_new_btn)
{
	control_t *p_ctrl;
	u16 pg_id;
	rect_t frame_rec;
	
	btn_rec_t btn_rec;
	btn_rec.w = BTN1_W;
	btn_rec.h = BTN1_H;
	p_ctrl = fw_find_root_by_id(ROOT_ID_XEXTEND);
	ctrl_get_frame(p_ctrl, &frame_rec);
	if(PROG_LIST_EXTEND==get_menu_type())
	{
		btn_rec.x = SMALL_MENU_W+COMM_BG_X-SMALL_LIST_AZ_W;
		btn_rec.y = frame_rec.top+(BTN1_H+BTN1_GARY)*(p_new_btn->id-XEXTEND_BTN1_CNT_DESKTOP);
	}
	else if(DESKTOP_EXTEND==get_menu_type())
	{
		btn_rec.y = frame_rec.top+(BTN1_H+BTN1_GARY)*(p_new_btn->id-1);
		btn_rec.x = EXTEND_GAP;
	}
	else
		return ERR_FAILURE;
	pg_id = sys_status_get_curn_group_curn_prog_id();
	return manage_open_menu(ROOT_ID_VEPG, (u32)&btn_rec, pg_id);
}

static control_t *create_maincont(menu_type_t type, u32 para1, u32 para2)
{
	control_t *p_ctrl;
	u16 btn_cnt;
	u16 x, y, w;

	switch(type)
	{
	case DESKTOP_EXTEND:
		btn_cnt = XEXTEND_BTN1_CNT_DESKTOP-1;
		x = DESKTOP_CONT1_X;
		y = DESKTOP_CONT1_Y;
		w = EXTEND_W+EXTEND_GAP;
		break;
	case PROG_LIST_EXTEND:
		btn_cnt = XEXTEND_BTN1_CNT_PROG_LIST - XEXTEND_BTN1_CNT_DESKTOP;
		x = SMALL_MENU_W+COMM_BG_X-SMALL_LIST_AZ_W;
		y = (u16)para1;
		if((y+EXTEND_H)>(SCREEN_HEIGHT-20))
			y = y-(BTN1_H+BTN1_GARY)*btn_cnt+(SMALL_LIST_LIST_H/SMALL_LIST_PAGE);
		w = EXTEND_W;
		break;
	case PROG_BAR_EXTEND:
		btn_cnt = XEXTEND_BTN1_CNT_DESKTOP-1;
		x = DESKTOP_CONT1_X;
		y = DESKTOP_CONT1_Y;
		w = EXTEND_W+EXTEND_GAP;
		break;
	default:
		DEBUG(MAIN, ERR, "ERROR MENU TYPE");
		return NULL;
		break;
	}
	
	p_ctrl = fw_create_mainwin(ROOT_ID_XEXTEND, x, y, w, (BTN1_H+BTN1_GARY)*btn_cnt+2*EXTEND_CONT_GAP, 
									ROOT_ID_INVALID, 0, OBJ_ATTR_ACTIVE, 0);
	ctrl_set_proc(p_ctrl, extend_root_proc);
	p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_MAINCONT, 0, 0, 
											w, (BTN1_H+BTN1_GARY)*btn_cnt+2*EXTEND_CONT_GAP, p_ctrl, 0);
	ctrl_set_rstyle(p_ctrl, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);
	ctrl_set_keymap(p_ctrl, extend_root_cont_keymap);
	ctrl_set_proc(p_ctrl, extend_root_cont_proc);

	return p_ctrl;
}

static void create_bmp_ctrl_for_btn(control_t *p_btn)
{
	control_t *p_bmp1, *p_bmp2;
	
	p_bmp1 = ctrl_create_ctrl(CTRL_BMAP, IDC_LEFT1_BMP1, BTN_BMP_GARY,0,
									BMP_W, BTN1_H, p_btn, 0);
	p_bmp2 = ctrl_create_ctrl(CTRL_BMAP, IDC_LEFT1_BMP2, BTN1_W-BMP_W-BTN_BMP_GARY,0,
									BMP_W, BTN1_H, p_btn, 0);
	#ifdef VOLUME_CHANNEL_ON
	if((IDC_XEXTEND_AUDIO_TRACK==p_btn->id)||(IDC_XEXTEND_CHANNEL==p_btn->id))
	#else
    if(IDC_XEXTEND_AUDIO_TRACK==p_btn->id)
	#endif
	{
		bmap_set_content_by_id(p_bmp1, IM_SLISTARROW02_L);
		bmap_set_content_by_id(p_bmp2, IM_SLISTARROW02_R);
	}
	else if(IDC_XEXTEND_PAUSE==p_btn->id)
	{
		if(ui_is_pause())
			bmap_set_content_by_id(p_bmp2, IM_XIC_PLAY_2);
		else
			bmap_set_content_by_id(p_bmp2, IM_XIC_PAUSE);
	}
	ctrl_set_sts(p_bmp1, OBJ_STS_HIDE);
	ctrl_set_sts(p_bmp2, OBJ_STS_HIDE);
}

static btn_type_t get_ctrl_type(u16 id, menu_type_t type)
{
	u16 size;
	u16 i;
	u16 desktop_idc_select_item[] = {
		IDC_XEXTEND_AUDIO_TRACK,
		#ifdef VOLUME_CHANNEL_ON
		IDC_XEXTEND_CHANNEL,
		#endif
	};

	
	if(DESKTOP_EXTEND==type)
	{
		size = sizeof(desktop_idc_select_item)/sizeof(u16);
		for(i = 0; i<size; i++)
		{
			if(id==desktop_idc_select_item[i])
				return SELECT_BTN;
		}
		return TEXT_BTN;
	}
	else if(PROG_BAR_EXTEND==type)
	{
		return TEXT_BTN;
	}
	else if(PROG_LIST_EXTEND==type)
	{
		return TEXT_BTN;
	}

	return UNKNOWN_CTRL_TYPE;
}
static control_t *_create_btn(control_t *p_btn_cont, menu_type_t type, u16 id, u16 num, u16 str_id)
{
	control_t *p_btn = NULL;
	btn_type_t ctrl_type;

	ctrl_type = get_ctrl_type(id, type);
	if(TEXT_BTN==ctrl_type)
	{
		p_btn = ctrl_create_ctrl(CTRL_TEXT, id, 0,(BTN1_H+BTN1_GARY)*num,
										BTN1_W, BTN1_H, p_btn_cont, 0);
		text_set_font_style(p_btn, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
		text_set_align_type(p_btn, STL_LEFT | STL_VCENTER);
		text_set_content_type(p_btn, TEXT_STRTYPE_STRID);
		text_set_offset(p_btn, BMP_W+BTN_BMP_GARY,0);
/*		if(IDC_XEXTEND_TVRADIO==id)
		{
			if(CURN_MODE_TV == sys_status_get_curn_prog_mode())
				text_set_content_by_strid(p_btn, IDS_RADIO);
			else if(CURN_MODE_RADIO == sys_status_get_curn_prog_mode())
				text_set_content_by_strid(p_btn, IDS_TV);
		}
		else
		{*/
			text_set_content_by_strid(p_btn, str_id);
//		}
	}
	else if(SELECT_BTN==ctrl_type)
	{
		p_btn = ctrl_create_ctrl(CTRL_CBOX, id, 0,(BTN1_H+BTN1_GARY)*num, 
											BTN1_W,BTN1_H, p_btn_cont, 0);
		cbox_set_font_style(p_btn, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
		cbox_set_align_style(p_btn, STL_LEFT | STL_VCENTER);
		cbox_set_offset(p_btn, BMP_W+BTN_BMP_GARY,0);
		cbox_enable_cycle_mode(p_btn, TRUE);
	}
	else
	{
		DEBUG(MAIN, ERR, "ERROR btn type!\n");
	}
	if(NULL!=p_btn)
	{
		ctrl_set_rstyle(p_btn, RSI_LEFT_CONT_BG, RSI_OPT_BTN_H, RSI_GRAY);
		create_bmp_ctrl_for_btn(p_btn);
		set_btn_state(p_btn);
	}

	return p_btn;
}

static void set_default_focus(control_t *p_btn_cont, menu_type_t type, u16* idc_item)
{
	control_t *p_btn, *p_bmp1, *p_bmp2;
	u8 i=0;
	if(DESKTOP_EXTEND==type)
	{
		p_btn = ctrl_get_child_by_id(p_btn_cont, idc_item[i]);
		while(NULL!=p_btn)
		{
			if(OBJ_ATTR_INACTIVE!=ctrl_get_attr(p_btn))
				break;
			p_btn = ctrl_get_child_by_id(p_btn_cont, idc_item[++i]);
		}
		if(NULL==p_btn)
			return;
		/*if(IDC_XEXTEND_ALL_EPG_DESKTOP==p_btn->id)
			extend_open_epg(p_btn);*/
		p_bmp1 = ctrl_get_child_by_id(p_btn, IDC_LEFT1_BMP1);
		p_bmp2 = ctrl_get_child_by_id(p_btn, IDC_LEFT1_BMP2);
		ctrl_set_sts(p_bmp1, OBJ_STS_SHOW);
		ctrl_set_sts(p_bmp2, OBJ_STS_SHOW);
		ctrl_default_proc(p_btn, MSG_GETFOCUS, 0, 0);
	}
	else if(PROG_LIST_EXTEND==type)
	{
		p_btn = ctrl_get_child_by_id(p_btn_cont, idc_item[0]);
		if(IDC_XEXTEND_ALL_EPG==p_btn->id)
			extend_open_epg(p_btn);
		ctrl_default_proc(p_btn, MSG_GETFOCUS, 0, 0);
	}
}

static void create_btn_list1(control_t *p_cont, menu_type_t type, u32 para1, u32 para2)
{
	control_t *p_btn_cont, *p_btn;
	u16 cnt;
	u16 i,j;
	u16 *ids_item1;
	#ifdef VOLUME_CHANNEL_ON
	u16 total=0;
	#endif
	u16 ids_desktop_item1[XEXTEND_BTN1_CNT_DESKTOP-1] = {
		IDS_MUTE, IDS_AUDIO_TRACK,/*IDS_AUDIO_MODE, IDS_ALL_EPG, */
		#ifdef VOLUME_CHANNEL_ON
		IDS_AUDIO_TRACK,
		#endif
		IDS_INFO,IDS_ADD_TO_FAV_LIST,IDS_PAUSE,IDS_A_KEY_SEARCH,
		/*IDS_TV, */IDS_DVB_TIME_OFF, IDS_SETTING
	};
	u16 idc_desktop[XEXTEND_BTN1_CNT_DESKTOP-1] = {
		IDC_XEXTEND_MUTE,
		IDC_XEXTEND_AUDIO_TRACK,
		#ifdef VOLUME_CHANNEL_ON
		IDC_XEXTEND_CHANNEL,
		#endif
		//IDC_XEXTEND_ALL_EPG_DESKTOP,
		IDC_XEXTEND_INFO,
		IDC_XEXTEND_ADD_FAV_DESKTOP,
		IDC_XEXTEND_PAUSE,
		//IDC_XEXTEND_TVRADIO,
		IDC_A_KEY_SEARCH,
		IDC_XEXTEND_SLEEP_TIMER,
		IDC_XEXTEND_SETTING
	};
	u16 ids_prog_list_item1[XEXTEND_BTN1_CNT_PROG_LIST - XEXTEND_BTN1_CNT_DESKTOP] = {
		 IDS_ADD_TO_FAV_LIST, IDS_ALL_EPG,IDS_LOCK, IDS_HIDE, IDS_DEL,
#ifdef XEXTEND_SMLIST_DELETE_ALL
		IDS_DELETE_ALL
#endif
	};
	u16 idc_prog_list[XEXTEND_BTN1_CNT_PROG_LIST - XEXTEND_BTN1_CNT_DESKTOP] = {
		IDC_XEXTEND_ADD_TO_FAV,
		IDC_XEXTEND_ALL_EPG,
		IDC_XEXTEND_LOCK,
		IDC_XEXTEND_SKIP,
		IDC_XEXTEND_DELETE,
#ifdef XEXTEND_SMLIST_DELETE_ALL
		IDC_XEXTEND_DELETE_ALL
#endif
	};
	u16 *idc_item;
	u16 pg_id;
	u8 view_id;
	u16 pos;
	ui_audio_set_t *private_data = ui_audio_set_get_private_data();
	
	pg_id = sys_status_get_curn_group_curn_prog_id();
	view_id = ui_dbase_get_pg_view_id();
	pos = db_dvbs_get_view_pos_by_id(view_id, pg_id);
//	love_mark_status = db_dvbs_get_mark_status(view_id, pos, DB_DVBS_FAV_GRP, 0);
	switch(type)
	{
	case DESKTOP_EXTEND:
		if(ui_audio_set_c_get_data(para1, para2) != SUCCESS)
			return;
		cnt = XEXTEND_BTN1_CNT_DESKTOP-1;
		ids_item1 = ids_desktop_item1;
		idc_item = idc_desktop;
		break;
	case PROG_BAR_EXTEND:
		cnt = 1;
		ids_item1 = ids_desktop_item1;
		idc_item = idc_desktop;
		break;
	case PROG_LIST_EXTEND:
		cnt = XEXTEND_BTN1_CNT_PROG_LIST - XEXTEND_BTN1_CNT_DESKTOP;
		ids_item1 = ids_prog_list_item1;
		idc_item = idc_prog_list;
		break;
	default:
		break;
	}
	
	p_btn_cont = ctrl_create_ctrl(CTRL_CONT, IDC_LIST_CONT1, 
								((DESKTOP_EXTEND==type)?EXTEND_GAP:0),EXTEND_CONT_GAP,
								BTN1_W, (BTN1_H+BTN1_GARY)*cnt, p_cont, 0);
	ctrl_set_rstyle(p_btn_cont, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);
	ctrl_set_keymap(p_btn_cont, extend_cont1_keymp);
	ctrl_set_proc(p_btn_cont, extend_cont1_proc);
	for(i = 0; i<cnt; i++)
	{
		p_btn = _create_btn(p_btn_cont, type, idc_item[i], i, ids_item1[i]);
		switch(idc_item[i])
		{
		case IDC_XEXTEND_AUDIO_TRACK:
			cbox_set_work_mode(p_btn, CBOX_WORKMODE_STATIC);
			cbox_static_set_content_type(p_btn, CBOX_ITEM_STRTYPE_STRID);
			for (j = 0; j < 4; j++)
			{
				cbox_static_set_content_by_strid(p_btn, j, ids_audio_mode[j]);
			}
			if(ui_audio_set_program_is_ac3_bsout((u8)private_data->program.audio_channel))
			{
				ctrl_set_attr(p_btn, OBJ_ATTR_INACTIVE);
			}
			cbox_static_set_count(p_btn, 4);
			for(j= 0;j<4;j++)
			{
				cbox_static_set_content_by_strid(p_btn, j, ids_audio_mode[j]);
			}
			cbox_static_set_focus(p_btn, (u16)private_data->program.audio_track);
			break;
		#ifdef VOLUME_CHANNEL_ON
		case IDC_XEXTEND_CHANNEL:
			cbox_set_work_mode(p_btn, CBOX_WORKMODE_DYNAMIC);
			#ifdef PLAY_RECORD_VOLUME_CHANNEL_EN
			if(private_data->type == AUDIO_SET_PLAY_RECORD)
			{
				total = private_data->record.audio_channel_total;
				cbox_dync_set_count(p_btn, (0==total?1:total));
				if(total<2)
					ctrl_set_attr(p_btn, OBJ_ATTR_INACTIVE);
				cbox_dync_set_focus(p_btn, (u16)private_data->record.audio_channel);
			}
			else
			#endif
			{
				total = private_data->program.audio_ch_num;
				cbox_dync_set_count(p_btn, (0==total?1:total));
				if(total<2)
					ctrl_set_attr(p_btn, OBJ_ATTR_INACTIVE);
				cbox_dync_set_focus(p_btn, (u16)private_data->program.audio_channel);
			}
			cbox_dync_set_update(p_btn, ui_fill_track_info);
			break;
			#endif
		default:
			break;
		}
		ctrl_set_related_id(p_btn, 0, idc_item[(0==i)?(cnt-1):(i-1)], 
										0, idc_item[(cnt-1==i)?0:(i+1)]);
	}

	set_default_focus(p_btn_cont, type, idc_item);
	ctrl_paint_ctrl(p_cont, FALSE);
	if((IDC_XEXTEND_ALL_EPG==idc_item[0]))
		extend_change_focus1_manage(NULL, ctrl_get_child_by_id(p_btn_cont, IDC_XEXTEND_ALL_EPG), 0);
	/*else if((IDC_XEXTEND_ALL_EPG_DESKTOP==idc_item[0]))
		extend_change_focus1_manage(NULL, ctrl_get_child_by_id(p_btn_cont, IDC_XEXTEND_ALL_EPG_DESKTOP), 0);*/
}

RET_CODE open_extend(u32 para1, u32 para2)
{
	control_t *p_cont;
	menu_type_t menu_type;

	if(db_dvbs_get_count(ui_dbase_get_pg_view_id()) > 0)
	{
		menu_type = get_menu_type();
		p_cont = create_maincont(menu_type, para1, para2);
		if(NULL==p_cont)
			return ERR_FAILURE;
		
		create_btn_list1(p_cont, menu_type, para1, para2);
		create_timer_for_close();
	}

	return SUCCESS;
}
static RET_CODE extend_change_focus1_manage(control_t *p_old_btn, control_t *p_new_btn, u16 msg)
{
	btn_rec_t btn_rec;
	
	switch(p_old_btn->id)
	{
	case IDC_XEXTEND_ALL_EPG:
	//case IDC_XEXTEND_ALL_EPG_DESKTOP:
		if(NULL!=fw_find_root_by_id(ROOT_ID_VEPG))
			manage_close_menu(ROOT_ID_VEPG, 0, 0);
		break;
	case IDC_XEXTEND_INFO:
		if(NULL!=fw_find_root_by_id(ROOT_ID_XSMALL_INFO))
			manage_close_menu(ROOT_ID_XSMALL_INFO, 0, 0);
		break;
	}

	btn_rec.w = BTN1_W;
	btn_rec.h = BTN1_H;
	btn_rec.x = EXTEND_GAP;
	btn_rec.y = DESKTOP_CONT1_Y+(BTN1_H+BTN1_GARY)*(p_new_btn->id-IDC_XEXTEND_AUDIO_TRACK);
	switch(p_new_btn->id)
	{
	case IDC_XEXTEND_INFO:
		return manage_open_menu(ROOT_ID_XSMALL_INFO, (u32)&btn_rec, 0);
		break;
	case IDC_XEXTEND_ADD_FAV_DESKTOP:
	case IDC_XEXTEND_ADD_TO_FAV:
		set_btn_state(p_new_btn);
		break;
	case IDC_XEXTEND_ALL_EPG:
//	case IDC_XEXTEND_ALL_EPG_DESKTOP:
		return extend_open_epg(p_new_btn);
		break;
	}
		
	return SUCCESS;
}

static RET_CODE extend_change_focus1(control_t *p_cont1, u16 msg, u32 para1, u32 para2)
{
	control_t *p_ctrl, *p_bmp_curr1, *p_bmp_next1,*p_bmp_curr2, *p_bmp_next2;
	control_t *p_next_btn = NULL;

	reset_timer_for_close();
	p_ctrl = ctrl_get_active_ctrl(p_cont1);
	p_bmp_curr1 = ctrl_get_child_by_id(p_ctrl, IDC_LEFT1_BMP1);
	p_bmp_curr2 = ctrl_get_child_by_id(p_ctrl, IDC_LEFT1_BMP2);
	switch(msg)
	{
	case MSG_FOCUS_UP:
		p_next_btn = ctrl_get_child_by_id(p_ctrl->p_parent, p_ctrl->up);
		while(NULL!=p_next_btn)
		{
			if(OBJ_ATTR_INACTIVE!=ctrl_get_attr(p_next_btn))
				break;
			p_next_btn = ctrl_get_child_by_id(p_next_btn->p_parent, p_next_btn->up);
		}
		break;
	case MSG_FOCUS_DOWN:
		p_next_btn = ctrl_get_child_by_id(p_ctrl->p_parent, p_ctrl->down);
		while(NULL!=p_next_btn)
		{
			if(OBJ_ATTR_INACTIVE!=ctrl_get_attr(p_next_btn))
				break;
			p_next_btn = ctrl_get_child_by_id(p_next_btn->p_parent, p_next_btn->down);
		}
		break;
	}
	if(NULL==p_next_btn)
		return ERR_FAILURE;
	p_bmp_next1 = ctrl_get_child_by_id(p_next_btn, IDC_LEFT1_BMP1);
	p_bmp_next2 = ctrl_get_child_by_id(p_next_btn, IDC_LEFT1_BMP2);

	ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0); 
	ctrl_process_msg(p_next_btn, MSG_GETFOCUS, 0, 0); 
	ctrl_set_sts(p_bmp_curr1, OBJ_STS_HIDE);
	ctrl_set_sts(p_bmp_curr2, OBJ_STS_HIDE);
	ctrl_set_sts(p_bmp_next1, OBJ_STS_SHOW);
	ctrl_set_sts(p_bmp_next2, OBJ_STS_SHOW);
	ctrl_paint_ctrl(p_ctrl, FALSE);
	ctrl_paint_ctrl(p_next_btn, FALSE);
	return extend_change_focus1_manage(p_ctrl, p_next_btn, msg);
}

static RET_CODE extend_change_cont1(control_t *p_cont1, u16 msg, u32 para1, u32 para2)
{
	control_t *p_btn;
	RET_CODE re = SUCCESS;
	
	reset_timer_for_close();
	if(PROG_LIST_EXTEND == get_menu_type())
	{
		if(MSG_FOCUS_LEFT == msg)
		{
			exit_extend(p_cont1, msg, para1, para2);
			return SUCCESS;
		}
	}
	p_btn = ctrl_get_active_ctrl(p_cont1);
	switch(p_btn->id)
	{
	case IDC_XEXTEND_AUDIO_TRACK:
	#ifdef VOLUME_CHANNEL_ON
	case IDC_XEXTEND_CHANNEL:
	#endif
		re = ui_audio_set_key_left_right(p_btn, msg, para1, para2);
		break;
	}

	if(PROG_LIST_EXTEND != get_menu_type())
	{
		_set_mute_btn_state(ctrl_get_child_by_id(p_cont1, IDC_XEXTEND_MUTE));
		ctrl_paint_ctrl(ctrl_get_child_by_id(p_cont1, IDC_XEXTEND_MUTE), TRUE);
	}
	return re;
}

#ifndef  SUPPORT_POWER_FULL_SEARCH
static void on_extend_copy_tp(dvbs_tp_node_t *tp1,dvbc_lock_t tp2)
{
        tp1->freq = tp2.tp_freq;
        tp1->sym = tp2.tp_sym;
        tp1->nim_modulate = tp2.nim_modulate;
}
#endif

static RET_CODE on_extend_auto_search(void)
{
#ifdef  SUPPORT_POWER_FULL_SEARCH
	ui_do_search_full_search();
#else
	ui_scan_param_t scan_param;
	dvbc_lock_t tp = {0};

	memset(&scan_param, 0, sizeof(ui_scan_param_t));
	sys_status_get_main_tp1(&tp);  
	on_extend_copy_tp(&(scan_param.tp),tp);
#ifdef AISET_BOUQUET_SUPPORT
	scan_param.nit_type = NIT_SCAN_ALL_TP;
#else
	scan_param.nit_type = NIT_SCAN_ONCE;
#endif
	manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_AUTO, (u32)&scan_param);
#endif
return SUCCESS;
}
static RET_CODE extend_cont1_select(control_t *p_cont1, u16 msg, u32 para1, u32 para2)
{
	control_t *p_btn;
	btn_rec_t btn_rec;
	p_btn = ctrl_get_active_ctrl(p_cont1);
	
	reset_timer_for_close();
	btn_rec.w = BTN1_W;
	btn_rec.h = BTN1_H;
	btn_rec.x = EXTEND_GAP;
	btn_rec.y = DESKTOP_CONT1_Y+(BTN1_H+BTN1_GARY)*(p_btn->id-IDC_XEXTEND_AUDIO_TRACK);
	switch(p_btn->id)
	{
	case IDC_XEXTEND_PAUSE:
		return send_msg_to_desktop(p_btn, MSG_PAUSE, 0, 0);
		break;
	case IDC_XEXTEND_MUTE:
		return send_msg_to_desktop(p_btn, MSG_MUTE, 0, 0);
		break;
	case IDC_XEXTEND_ADD_FAV_DESKTOP:
	case IDC_XEXTEND_ADD_TO_FAV:
		return update_event_state(p_btn, DB_DVBS_FAV_GRP);
		break;
	case IDC_XEXTEND_SETTING:
		exit_extend(NULL, MSG_EXIT, 0, 0);
		manage_open_menu(ROOT_ID_XSYS_SET, 0, 0);
		break;
	case IDC_XEXTEND_LOCK:
	case IDC_XEXTEND_DELETE:
#ifdef XEXTEND_SMLIST_DELETE_ALL
	case IDC_XEXTEND_DELETE_ALL:
#endif
		return extend_pwd_open();
		break;
	case IDC_XEXTEND_SKIP:
		return update_event_state(p_btn, DB_DVBS_MARK_SKP);
		break;
//	case IDC_XEXTEND_TVRADIO:
//		exit_extend(NULL, MSG_EXIT, 0, 0);
//		return send_msg_to_desktop(p_btn, MSG_OPEN_MENU_IN_TAB | ROOT_ID_PROG_BAR, (u32)V_KEY_TVRADIO, 0);
//		break;
	case IDC_XEXTEND_SLEEP_TIMER:
		manage_open_menu(ROOT_ID_SLEEP_TIMER, (u32)&btn_rec, 0);
		break;
    case IDC_A_KEY_SEARCH:
        exit_extend(NULL, MSG_EXIT, 0, 0);
        return on_extend_auto_search();
        break;
	}

	return ERR_FAILURE;
}
static RET_CODE extend_exit_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_cont;

	if(NULL!=fw_find_root_by_id(ROOT_ID_PASSWORD)
				||is_epg_have_focus()
				||NULL!=fw_find_root_by_id(ROOT_ID_TIMER)
				||ROOT_ID_SLEEP_TIMER==fw_get_focus_id())
	{
		reset_timer_for_close();
		return SUCCESS;
	}
	p_cont = get_extend_cont1_ctrl();
	if(NULL!=p_cont)
		exit_extend(p_cont, MSG_EXIT, 0, 0);
	return SUCCESS;
}

BEGIN_MSGPROC(extend_root_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_TIMER_ON, extend_exit_all)
END_MSGPROC(extend_root_proc, ui_comm_root_proc)

BEGIN_KEYMAP(extend_root_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_HOT_XEXTEND, MSG_EXIT)
	ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
END_KEYMAP(extend_root_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(extend_root_cont_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_EXIT_ALL, extend_exit_all)
END_MSGPROC(extend_root_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(extend_cont1_keymp, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
	ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_HOT_XEXTEND, MSG_EXIT)
END_KEYMAP(extend_cont1_keymp, ui_comm_root_keymap)

BEGIN_MSGPROC(extend_cont1_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_FOCUS_UP, extend_change_focus1)
	ON_COMMAND(MSG_FOCUS_DOWN, extend_change_focus1)
	ON_COMMAND(MSG_FOCUS_LEFT, extend_change_cont1)
	ON_COMMAND(MSG_FOCUS_RIGHT, extend_change_cont1)
	ON_COMMAND(MSG_SELECT, extend_cont1_select)
	ON_COMMAND(MSG_EXIT, exit_extend)
	ON_COMMAND(MSG_SLIST_PWDLG_EXIT, on_extend_pwd_close)
	ON_COMMAND(MSG_EXIT_ALL, extend_exit_all)
	ON_COMMAND(MSG_XEXTEND_RESET_TIMER, reset_timer_from_other)
	ON_COMMAND(MSG_XEXTEND_STOP_TIMER,  stop_timer_from_other)
	ON_COMMAND(MSG_XEXTEND_START_TIMER,  start_timer_from_other)
END_MSGPROC(extend_cont1_proc, ui_comm_root_proc)

BEGIN_KEYMAP(extend_pwd_keymap, NULL)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_LEFT, MSG_PAGE_UP)
	ON_EVENT(V_KEY_RIGHT, MSG_PAGE_DOWN)
	ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
	ON_EVENT(V_KEY_EXIT, MSG_EXIT)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
END_KEYMAP(extend_pwd_keymap, NULL)

BEGIN_MSGPROC(extend_pwd_proc, cont_class_proc)
	ON_COMMAND(MSG_FOCUS_UP, on_extend_pwd_close)
	ON_COMMAND(MSG_FOCUS_DOWN, on_extend_pwd_close)
	ON_COMMAND(MSG_EXIT, on_extend_pwd_close)
	ON_COMMAND(MSG_CORRECT_PWD, extend_pwd_correct)
	ON_COMMAND(MSG_EXIT_ALL, extend_exit_all)
END_MSGPROC(extend_pwd_proc, cont_class_proc)

