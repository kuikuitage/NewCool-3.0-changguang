/******************************************************************************/

/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "lib_util.h"

#include "string.h"

#include "mtos_misc.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "hal_dma.h"


#include "common.h"
#include "drv_dev.h"
#include "lib_rect.h"
#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "nim.h"
#include "scart.h"
#include "rf.h"
#include "avsync.h"
#include "drv_misc.h"

#include "uio.h"

#include "class_factory.h"
#include "mdl.h"
#include "dvb_protocol.h"
#include "service.h"
#include "dvb_svc.h"
#include "mosaic.h"
#include "pmt.h"
#include "sdt.h"
#include "nit.h"
#include "cat.h"

#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "avctrl1.h"
#include "subtitle_api.h"
#include "vbi_api.h"
#include "data_manager.h"
#include "data_base.h"
#include "db_dvbs.h"

#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "ss_ctrl.h"
#include "dmx.h"


#include "ap_framework.h"
#include "ap_playback.h"
#include "ap_playback_i.h"
#include "sys_status.h"


#include "ui_dbase_api.h"
#include "ui_common.h"

typedef struct
{
  u32 cur_pg_id;
  u32 s_nit_ver;
} pb_data_t;

extern void stc_set_pg_id_1(handle_t handle, u16 pg_id);

static dvbs_prog_node_t  g_cur_pg = {0}; 

static void send_evt_to_ui(pb_evt_t evt_t, u32 data1, u32 data2)
{
  event_t evt;
  evt.id = evt_t;
  evt.data1 = (u32)data1;
  evt.data2 = (u32)data2;
  ap_frm_send_evt_to_ui(APP_PLAYBACK, &evt);
}


static void _play(void *p_data, play_param_t *p_play_param)
{
  pb_data_t *p_pb_data = (pb_data_t *)p_data;
  p_pb_data->cur_pg_id = p_play_param->pg_info.context1;
#if ENABLE_TTX_SUBTITLE
   {
      class_handle_t handle = NULL;
      handle = class_get_handle_by_id(VBI_SUBT_CTRL_CLASS_ID);
      stc_set_pg_id_1(handle, (u16)p_play_param->pg_info.context1);
    }
#endif
  db_dvbs_get_pg_by_id((u16)p_pb_data->cur_pg_id, &g_cur_pg);
}


static void _check_hd_prog(void *p_data)
{
  pb_data_t *p_pb_data = (pb_data_t *)p_data;
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  vdec_info_t v_info = {0};
  void *p_video_dev = dev_find_identifier(NULL,
    DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);

  vdec_get_info(p_video_dev,&v_info);

  if(v_info.width > 720)
  {
    ret = db_dvbs_get_pg_by_id((u16)p_pb_data->cur_pg_id, &pg);
    MT_ASSERT(DB_DVBS_OK == ret);
    if(pg.service_type != 0x11)
    {
      if(pg.video_pid != 0)
      {
        pg.service_type = 0x11;
        db_dvbs_edit_program(&pg);
        db_dvbs_save_pg_edit(&pg);
      }
    }
  }
}


static void _check(play_param_t play_param)
{
  BOOL is_display = FALSE;
  vdec_info_t v_info;
  static u8 cnt = 0;
  static u16 last_pmt_pid = 0;
  static u8 sup_cnt = 0;

  void *p_disp_dev = dev_find_identifier(NULL,
    DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  void *p_video_dev = dev_find_identifier(NULL,
    DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
//  void *p_dmx_dev = dev_find_identifier(NULL,
//    DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

  if(0 == play_param.pg_info.v_pid || 0x1FFF == play_param.pg_info.v_pid)
  {
    //radio return
    return;
  }

  vdec_get_info(p_video_dev,&v_info);


   if(last_pmt_pid != play_param.pg_info.pmt_pid)
  {
    last_pmt_pid = play_param.pg_info.pmt_pid;
    sup_cnt = 0;
  }
  if(3 == sup_cnt || 0 == sup_cnt)
  {
    if(v_info.is_format != TRUE)
    {
     // OS_PRINTF("------------->>applayback not support video!\n");
     // send_evt_to_ui(PB_EVT_NOT_SUPPORT_VIDEO, sup_cnt == 0?FALSE:TRUE , TRUE);
    }
    else
    {
      //send_evt_to_ui(PB_EVT_NOT_SUPPORT_VIDEO, sup_cnt == 0?FALSE:TRUE , FALSE);
    }
  }
  if(sup_cnt <= 3)
  {
    sup_cnt ++;
  }
  else
  {
    sup_cnt = 0;
  }


  is_display = disp_layer_is_show(p_disp_dev,DISP_LAYER_ID_VIDEO_SD);

  if(v_info.err == VDEC_ERROR_NONE)
  {
    if(!is_display)
    {
      if(TRUE == v_info.is_stable)
      {
        OS_PRINTF("------------->> open video\n");
        disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);
        disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_HD, TRUE);
        send_evt_to_ui(PB_EVT_NOTIFY_VIDEO_FORMAT, 
          (u32)v_info.vid_format, (u32) play_param.pg_info.s_id);
      }

    }
    if(play_param.pg_info.is_scrambled &&  (++cnt > 1))
    {
      send_evt_to_ui(PB_DESCRAMBL_SUCCESS, 0, 0);
      cnt = 0;
    }
  }
  else
  {
    cnt ++;

    if((cnt > 3 && v_info.err == VDEC_ERROR_THIRSTY) // no data input
      || (cnt > 4 && v_info.err == VDEC_ERROR_SIZE)) // error data input
    {
      cnt = 0;
//      OS_PRINTF("------------->> close video\n");
      //dmx_av_reset(p_dmx_dev);
      disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);
      disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_HD, FALSE);
      if(play_param.pg_info.is_scrambled)
      {
        send_evt_to_ui(PB_DESCRAMBL_FAILED, 0, 0);
      }
    }
  }
  //OS_PRINTF("vdec info err %d \n",v_info.err);
}




static BOOL _process_pmt_info(void *p_data, pmt_t *p_pmt,
              u16 *p_v_pid, u16 *p_pcr_pid, u16 *p_a_pid, u16 *p_a_type)
{
  //pb_data_t *p_pb_data = (pb_data_t *)p_data;
  //db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};
  dvbs_prog_node_t pg_temp = {0};
  BOOL data_changed = FALSE;
  BOOL audio_pid_changed = FALSE;
  u16 audio_count = 0;
  u16 i = 0;
  u16 old_a_pid = 0;
  u16 old_a_type = 0;
  u8 view_id = ui_dbase_get_pg_view_id();
  u16 view_count = db_dvbs_get_count(view_id);
  u16 pg_id;
  BOOL find_pg = FALSE;
  
  for(i=0; i<view_count; i++)
  {
    pg_id = db_dvbs_get_id_by_view_pos(view_id, i);
    db_dvbs_get_pg_by_id(pg_id, &pg_temp);
    if(pg_temp.s_id == p_pmt->prog_num && pg_temp.pmt_pid == p_pmt->pmt_pid)
    {
      memcpy(&pg, &pg_temp, sizeof(dvbs_prog_node_t));
      find_pg = TRUE;
      break;
    }
  }
  if(find_pg == FALSE)
  {
    return FALSE;
  }
#if 0
  ret = db_dvbs_get_pg_by_id((u16)p_pb_data->cur_pg_id, &pg);
  if(DB_DVBS_OK != ret)
  {
    return FALSE;
  }
#endif
  //param check
  if(p_pmt->audio_count > DB_DVBS_MAX_AUDIO_CHANNEL)
  {
    audio_count = DB_DVBS_MAX_AUDIO_CHANNEL;
  }
  else
  {
    audio_count = p_pmt->audio_count;
  }

  //video pid
  if(p_pmt->video_pid)
  {
    *p_v_pid = p_pmt->video_pid;
    if(pg.video_pid != p_pmt->video_pid
      || pg.video_type != p_pmt->video_type)
    {
      OS_PRINTF(" old video pid %d type  %d\n",
        pg.video_pid, pg.video_type);
      pg.video_pid = p_pmt->video_pid;
      pg.video_type = p_pmt->video_type;
      OS_PRINTF(" process pmt info  save new video pid %d type  %d\n",
        pg.video_pid, pg.video_type);
      data_changed = TRUE;
    }
  }

  //prc pid
  *p_pcr_pid = p_pmt->pcr_pid;
  if(pg.pcr_pid != p_pmt->pcr_pid)
  {
    pg.pcr_pid = p_pmt->pcr_pid;
    data_changed = TRUE;
  }

  //audio pid
  old_a_pid = pg.audio[pg.audio_channel].p_id;
  old_a_type = pg.audio[pg.audio_channel].type;

  for(i = 0; i < audio_count; i++)
  {
    if((pg.audio[i].p_id != p_pmt->audio[i].p_id)
      || (pg.audio[i].type != p_pmt->audio[i].type))
    {
      audio_pid_changed = TRUE;
      OS_PRINTF(" audio_pid_changed to %d\n",p_pmt->audio[i].p_id);
      break;
    }
  }

  if(pg.audio_ch_num != audio_count || audio_pid_changed)
  {
    for(i = 0; i < audio_count; i++)
    {
      pg.audio[i] = p_pmt->audio[i];
      OS_PRINTF("audio[%d] = [%d,%d,%d]\n",i,pg.audio[i].p_id,
        pg.audio[i].type,pg.audio[i].language_index);
    }
    pg.audio_ch_num = audio_count;
    pg.audio_channel = 0;
    audio_pid_changed = TRUE;
    data_changed = TRUE;
  }

  if(audio_pid_changed)
  {
    BOOL has_sim_pid = FALSE;

    //find cur audio pid
    for(i = 0; i < audio_count; i++)
    {
      if((old_a_pid == pg.audio[i].p_id)
        && (old_a_type == pg.audio[i].type))
      {
        has_sim_pid = TRUE;
        pg.audio_channel = i;
        break;
      }
    }

    //find a new pid
    if(!has_sim_pid)
    {
      pg.audio_channel = get_audio_channel(&pg);
      OS_PRINTF("find a new pid,audio_channel=%d,pg.audio_ch_num=%d\n",
        pg.audio_channel,pg.audio_ch_num);
      if(pg.audio_ch_num)
        MT_ASSERT(pg.audio_channel < pg.audio_ch_num);
    }
  }

  *p_a_pid = pg.audio[pg.audio_channel].p_id;
  *p_a_type = pg.audio[pg.audio_channel].type;

  //save new db info
  if(data_changed)
  {
    OS_PRINTF("data changed,PID:%d/%d/%d\n",
      pg.video_pid, pg.audio[pg.audio_channel].p_id,pg.pcr_pid);
    db_dvbs_edit_program(&pg);
    return TRUE;
  }

  return FALSE;
}

static BOOL _process_nit_info(void *p_data, nit_t *p_nit )
{
    BOOL ret = TRUE;
    u8 nit_ver_num = p_nit->version_num;
    u32 old_nit_ver_num = 0;
    static u32 cur_nit_ver = 0;
    BOOL nit_recv;
    u16 prog_id;
    dvbc_lock_t main_tp = {0};
    dvbs_prog_node_t pg = {0};
    dvbs_tp_node_t tp = {0};


    prog_id = sys_status_get_curn_group_curn_prog_id();
    db_dvbs_get_pg_by_id(prog_id, &pg);
    db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);

    sys_status_get_main_tp1(&main_tp);
    
    sys_status_get_nit_version(&old_nit_ver_num);
    sys_status_get_status(BS_NIT_RECIEVED, &nit_recv);
    DEBUG(DBG,INFO,"_process_nit_info old_ver_num:%d, nit_ver_num:%d\n",old_nit_ver_num, nit_ver_num);
    if(nit_recv == FALSE)
    {
        sys_status_set_status(BS_NIT_RECIEVED, TRUE);
        sys_status_set_nit_version(nit_ver_num);    
    }
    else if((old_nit_ver_num != nit_ver_num)&&(cur_nit_ver != nit_ver_num))
    { 
        DEBUG(DBG,INFO,"_process_nit_info old_ver_num:%d, nit_ver_num:%d\n",old_nit_ver_num, nit_ver_num);
		switch(CUSTOMER_ID)
		{
			case CUSTOMER_MAIKE_HUNAN_LINLI:
			case CUSTOMER_DTMB_DESAI_JIMO:
			case CUSTOMER_DTMB_SHANGSHUIXIAN:
				break;
			default:
				if(main_tp.tp_freq == tp.freq)
				{
				    	send_evt_to_ui(PB_EVT_UPDATE_VERSION, (u32)nit_ver_num, 0);
					cur_nit_ver = nit_ver_num;
				}
				break;
		}
    }

#ifdef NIT_SOFTWARE_UPDATE
    if(p_data != NULL)
    {
        parse_nit_for_upgrade((u8*)p_nit->p_origion_data);
    }
#endif    
    return ret;
}

static BOOL _process_sdt_info(void *p_data, sdt_t *p_sdt)
{
  pb_data_t *p_pb_data = (pb_data_t *)p_data;
  db_dvbs_ret_t ret = DB_DVBS_OK;
  dvbs_prog_node_t pg = {0};

  ret = db_dvbs_get_pg_by_id((u16)p_pb_data->cur_pg_id, &pg);
  if(DB_DVBS_OK != ret)
  {
    return FALSE;
  }

  return FALSE;
}

void _init_play(play_param_t *p_play_param)
{
  p_play_param->is_do_sdt = FALSE;
  p_play_param->is_do_nit = TRUE;
  p_play_param->is_do_dynamic_pid = TRUE;
  p_play_param->start_mode = PB_START_STABLE;
}

pb_policy_t *construct_pb_policy(void)
{
#if ENABLE_TTX_SUBTITLE
  extern const u8 wstfont2_bits_pal_vsb[];
  extern const u8 wstfont2_bits_ntsc_vsb[];
  extern const u8 wstfont2_bits_small_vsb[];
  extern const u8 wstfont2_bits_hd_vsb[];

  ttx_font_src_t ttx_font;
#endif

  pb_policy_t *p_pb_pol = mtos_malloc(sizeof(pb_policy_t));
  MT_ASSERT(p_pb_pol != NULL);
  memset(p_pb_pol, 0, sizeof(pb_policy_t));

  //Alloc private data
  p_pb_pol->p_data = mtos_malloc(sizeof(pb_data_t));
  MT_ASSERT(p_pb_pol->p_data != NULL);
  memset(p_pb_pol->p_data, 0, sizeof(pb_data_t));

  p_pb_pol->init_play = _init_play;
  p_pb_pol->on_play = _play;
  p_pb_pol->process_pmt_info = _process_pmt_info;
  p_pb_pol->check_video = _check;
  p_pb_pol->process_sdt_info = _process_sdt_info;
  p_pb_pol->process_nit_info = _process_nit_info;
  p_pb_pol->check_hd_prog = _check_hd_prog;
  p_pb_pol->is_subtitle_on = FALSE;
#if ENABLE_TTX_SUBTITLE
  ttx_font.p_ntsl_font = (u8 *)wstfont2_bits_ntsc_vsb;
  ttx_font.p_pal_font = (u8 *)wstfont2_bits_pal_vsb;
  ttx_font.p_small_font = (u8 *)wstfont2_bits_small_vsb;
  ttx_font.p_hd_font = (u8 *)wstfont2_bits_hd_vsb;

  {
    RET_CODE ret = ERR_FAILURE;
    ret = subt_init_vsb(MDL_SUBT_TASK_PRIORITY, MDL_SUBT_TASK_STKSIZE,
                                  DISP_LAYER_ID_SUBTITL,mdl_broadcast_msg);
    MT_ASSERT(ret == SUCCESS);
    ret = vbi_init_vsb(MDL_VBI_TASK_PRIORITY, MDL_VBI_TASK_STKSIZE,mdl_broadcast_msg);
    MT_ASSERT(ret == SUCCESS);
  }
  vbi_set_font_src(&ttx_font);
   p_pb_pol->is_subtitle_on = TRUE;
  //vbi_ttx_start(TELTEXT_MAX_PAGE_NUM);
#endif

  return p_pb_pol;
}


void destruct_pb_policy(pb_policy_t *p_pb_policy)
{
  //Free private data
  mtos_free(p_pb_policy->p_data);

  //Free playback policy
  mtos_free(p_pb_policy);
}

