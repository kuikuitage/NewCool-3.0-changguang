/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_ts_record.h"
#include "vfs.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_TS_RECORD_FREQ,
#ifndef DTMB_PROJECT
  IDC_TS_RECORD_SYM,
  IDC_TS_RECORD_DEMOD,
#endif
  IDC_TS_RECORD_START,
};

static u32 g_cur_total_time = 0;
static u32 g_cur_rec_time = 0;
static u32 g_time_out = 5;
static u32 g_cur_rec_index = 0;
static u8 g_bar_state = 0;
static utc_time_t total_rec_time = {0,0,0,2,0,0,0};
static u32 g_uTotal_rec_time = 0;
static u16 g_capacity = 0;
static const u8 g_invalid_char[] = {
  0x22,//'"'
  0x2a,//'*'
  0x2f,//'/'
  0x3a,//':'
  0x3c,//'<'
  0x3e,//'>'
  0x3f,//'?'
  0x5c,//'\'
  0x7c,//'|'
  0x7f
};

static u16 ts_record_cont_keymap(u16 key);
static u16 ts_record_time_keymap(u16 key);

#ifndef DTMB_PROJECT
static RET_CODE ts_record_cbox_proc(control_t *p_cbox, u16 msg, u32 para1,u32 para2);
#endif
static RET_CODE ts_record_cont_proc(control_t *cont, u16 msg, u32 para1,u32 para2);
static RET_CODE ts_record_nbox_proc(control_t *p_cbox, u16 msg, u32 para1,u32 para2);
static RET_CODE ts_record_time_proc(control_t *p_cbox, u16 msg, u32 para1,u32 para2);


static void ts_record_set_transpond(control_t *p_cont)
{
#ifdef DTMB_PROJECT
	control_t *p_ctrl;
	dvbs_tp_node_t tp = {0};
	sat_node_t sat_node = {0};

	p_ctrl = ctrl_get_child_by_id(p_cont, IDC_TS_RECORD_FREQ);
	tp.freq = ui_comm_numedit_get_num(p_ctrl);
	tp.sym = DTMB_HN_SYMBOL_DEFAULT;
	tp.sat_id = sat_node.id;
	tp.nim_type = NIM_DVBT;
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

	p_ctrl = ctrl_get_child_by_id(p_cont, IDC_TS_RECORD_FREQ);
	freq = ui_comm_numedit_get_num(p_ctrl);

	p_ctrl = ctrl_get_child_by_id(p_cont, IDC_TS_RECORD_SYM);
	sym = ui_comm_numedit_get_num(p_ctrl);

	p_ctrl = ctrl_get_child_by_id(p_cont, IDC_TS_RECORD_DEMOD);
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

RET_CODE open_ts_record_tool(u32 para1, u32 para2)
{
	control_t *p_cont, *p_ctrl[TS_RECORD_ITEM_CNT];
	u8 i, j;

#ifdef DTMB_PROJECT
	u16 stxt[TS_RECORD_ITEM_CNT] = {IDS_FREQUENCY_MHZ, IDS_START};
#else
	u16 stxt[TS_RECORD_ITEM_CNT] = {IDS_FREQUECY, IDS_SYMBOL, IDS_MODULATION, IDS_START};
#endif

	u16 y;
	u8 separator[TBOX_MAX_ITEM_NUM] = {'-', '-', ' ', ':', ':', ' '};
	dvbc_lock_t tp = {0};

	total_rec_time.hour = 2;
	total_rec_time.minute = 0;
	total_rec_time.second= 0;

	sys_status_get_main_tp1(&tp);

	//stop monitor service
	{
		m_svc_cmd_p_t param = {0};
		dvb_monitor_do_cmd(class_get_handle_by_id(M_SVC_CLASS_ID), M_SVC_STOP_CMD, &param);
	}

	p_cont = ui_comm_right_root_create_with_signbar(ROOT_ID_TS_RECORD, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
	if (p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	ctrl_set_keymap(p_cont, ts_record_cont_keymap);
	ctrl_set_proc(p_cont, ts_record_cont_proc);

	y = TS_RECORD_ITEM_Y;
	for (i = 0; i < TS_RECORD_ITEM_CNT; i++)
	{
#ifdef DTMB_PROJECT
		switch (i)
		{
			case 0:
				p_ctrl[i] = ui_comm_numedit_create(p_cont, (u8)(IDC_TS_RECORD_FREQ + i),
									TS_RECORD_ITEM_X, y, TS_RECORD_ITEM_LW, TS_RECORD_ITEM_RW);
				ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
				ui_comm_ctrl_set_proc(p_ctrl[i], ts_record_nbox_proc);
				ui_comm_numedit_set_param(p_ctrl[i], NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID,
										SEARCH_FREQ_MIN, SEARCH_FREQ_MAX, SEARCH_FREQ_BIT, SEARCH_FREQ_BIT);
				if(tp.tp_freq < SEARCH_FREQ_MIN)
					tp.tp_freq = SEARCH_FREQ_MIN;
				if(tp.tp_freq > SEARCH_FREQ_MAX)
					tp.tp_freq = SEARCH_FREQ_MAX;
				ui_comm_numedit_set_num(p_ctrl[i], tp.tp_freq);
				break;
			case 1:
				p_ctrl[i] = ui_comm_timedit_create(p_cont, (u8)(IDC_TS_RECORD_FREQ + i), 
				                        TS_RECORD_ITEM_X, y, TS_RECORD_ITEM_LW, TS_RECORD_ITEM_RW);
				ui_comm_ctrl_set_keymap(p_ctrl[i], ts_record_time_keymap);
				ui_comm_ctrl_set_proc(p_ctrl[i], ts_record_time_proc);
				ui_comm_timedit_set_static_txt(p_ctrl[i], stxt[i]);
				ui_comm_timedit_set_time(p_ctrl[i], &total_rec_time);
				ui_comm_timedit_set_param(p_ctrl[i], 0, TBOX_ITEM_HOUR, TBOX_HOUR | TBOX_MIN | TBOX_SECOND, 
				                  TBOX_SEPARATOR_TYPE_UNICODE, 18);
				for(j = 0; j < TBOX_MAX_ITEM_NUM; j++)
				{
					ui_comm_timedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, separator[j]);
				}

				break;
			default:
				MT_ASSERT(0);
				break;
		}
#else
		switch (i)
		{
			case 0:
			case 1:
				p_ctrl[i] = ui_comm_numedit_create(p_cont, (u8)(IDC_TS_RECORD_FREQ + i),
				TS_RECORD_ITEM_X, y, TS_RECORD_ITEM_LW, TS_RECORD_ITEM_RW);
				ui_comm_numedit_set_static_txt(p_ctrl[i], stxt[i]);
				ui_comm_ctrl_set_proc(p_ctrl[i], ts_record_nbox_proc);
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
				p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_TS_RECORD_FREQ + i),
										TS_RECORD_ITEM_X, y, TS_RECORD_ITEM_LW, TS_RECORD_ITEM_RW);
				ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
				ui_comm_ctrl_set_proc(p_ctrl[i], ts_record_cbox_proc);
				ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,9, CBOX_ITEM_STRTYPE_STRID, NULL);
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
				//duration
				p_ctrl[i] = ui_comm_timedit_create(p_cont, (u8)(IDC_TS_RECORD_FREQ + i), 
				                        TS_RECORD_ITEM_X, y, TS_RECORD_ITEM_LW, TS_RECORD_ITEM_RW);
				ui_comm_ctrl_set_keymap(p_ctrl[i], ts_record_time_keymap);
				ui_comm_ctrl_set_proc(p_ctrl[i], ts_record_time_proc);
				ui_comm_timedit_set_static_txt(p_ctrl[i], stxt[i]);
				ui_comm_timedit_set_time(p_ctrl[i], &total_rec_time);
				ui_comm_timedit_set_param(p_ctrl[i], 0, TBOX_ITEM_HOUR, TBOX_HOUR | TBOX_MIN | TBOX_SECOND, 
				                  TBOX_SEPARATOR_TYPE_UNICODE, 18);
				for(j = 0; j < TBOX_MAX_ITEM_NUM; j++)
				{
					ui_comm_timedit_set_separator_by_ascchar(p_ctrl[i], (u8)j, separator[j]);
				}

				break;
			default:
				MT_ASSERT(0);
				break;
		}
#endif

		ctrl_set_related_id(p_ctrl[i],
		                    0,(u8)((i - 1 +TS_RECORD_ITEM_CNT) %TS_RECORD_ITEM_CNT + 1),            /* up */
		                    0,(u8)((i + 1) % TS_RECORD_ITEM_CNT + 1));
		y += TS_RECORD_ITEM_H + TS_RECORD_ITEM_V_GAP;
	}

	ts_record_set_transpond(p_cont);
	ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
	return SUCCESS;
}

static RET_CODE on_pvr_rec_stopped(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	OS_PRINTF("*******receive MUL_PVR_EVENT_REC_RESV,g_cur_rec_index=%d\n",para1);
	g_cur_rec_index = para1;
	g_cur_total_time += g_cur_rec_time;
	ui_recorder_up_devinfo();
	g_cur_rec_time = 0;
	return SUCCESS;
}
static RET_CODE on_pvr_rec_mem_not_enough(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	control_t *p_start = NULL;
	if(ui_recorder_isrecording())
	{
		ui_recorder_end();
		ui_recorder_stop();

		MT_ASSERT(ui_recorder_release() == SUCCESS);
	}
	p_start = ctrl_get_child_by_id(p_ctrl, IDC_TS_RECORD_START);
	ui_comm_timedit_set_static_txt(p_start, IDS_START);
	ctrl_paint_ctrl(p_start, TRUE);
	ui_comm_cfmdlg_open(NULL, IDS_NO_STORAGE_FOR_RECORD, NULL, 0);

	return SUCCESS;
}
static RET_CODE on_pvr_rec_write_error(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	if(ui_recorder_isrecording())
	{
		ui_recorder_end();
		ui_recorder_stop();

		MT_ASSERT(ui_recorder_release() == SUCCESS);
	}

	return SUCCESS;
}
static RET_CODE on_pvr_rec_no_signal(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	return SUCCESS;
}
static RET_CODE on_pvr_rec_updated(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	control_t *p_start = NULL;
	utc_time_t rec_time = {0};
	u32 cur_rec_time = 0;
	g_cur_rec_time = para1 / 1000;
	cur_rec_time = para1 / 1000;
	rec_time.hour = (u8)(cur_rec_time / 3600);
	rec_time.minute = (u8)((cur_rec_time % 3600) / 60);
	rec_time.second = (u8)(cur_rec_time % 60);    
	p_start = ctrl_get_child_by_id(p_ctrl, IDC_TS_RECORD_START);

	if(g_cur_rec_time + g_cur_total_time >= g_uTotal_rec_time)
	{
		if(ui_recorder_isrecording())
		{
			ui_recorder_end();
			ui_recorder_stop();
			MT_ASSERT(ui_recorder_release() == SUCCESS);
		}

		if(p_start)
		{
			ui_comm_timedit_set_time(p_start, &rec_time);
			ui_comm_timedit_set_static_txt(p_start, IDS_START);
			ctrl_paint_ctrl(p_start, TRUE);
		}
		ui_comm_cfmdlg_open(NULL, IDS_RECORD_FINISH, NULL, 2000);
	}
	else
	{
		if(p_start == NULL)
		{
			return ERR_FAILURE;
		}
		ui_comm_timedit_set_time(p_start, &rec_time);
		ctrl_paint_ctrl(p_start, TRUE);
	}
	return SUCCESS;
}


static RET_CODE on_ts_record_exit_edit_sts(control_t *p_text, u16 msg, u32 para1,u32 para2)
{
	control_t  *p_time = NULL;

	p_time = ctrl_get_parent(p_text);
	if((p_time) && (ui_comm_timedit_get_is_on_edit(p_time)))
	{
		tbox_exit_edit(p_text);
	}  
	else
	{
		ctrl_process_msg(ctrl_get_parent(ctrl_get_parent(p_text)), MSG_EXIT, 0, 0);
	}

	return SUCCESS;
}                             	   
//start record
static RET_CODE on_ts_record_select(control_t *p_text, u16 msg, u32 para1,u32 para2)
{
	u8 file_name[DB_DVBS_MAX_NAME_LENGTH + 1] = {0};
	u16 filename[DB_DVBS_MAX_NAME_LENGTH + 1] = {0};
	u8 asc_tmp[64] = {0};
	u16 uni_tmp[64] = {0};
	osd_set_t osd_set = {0};  
	utc_time_t gmt_time = {0};
	partition_t *p_partition = NULL;
	hfile_t file = NULL;
	mul_pvr_rec_attr_t rec_attr = {{0}};

	control_t *p_cont = NULL, *p_freq = NULL; 
	#ifndef DTMB_PROJECT
	control_t *p_sym = NULL, *p_demod = NULL;
	#endif
	control_t  *p_time = NULL;

	p_time = ctrl_get_parent(p_text);
	if((p_time) && (ui_comm_timedit_get_is_on_edit(p_time)))
	{
		tbox_exit_edit(p_text);
	}
  
    //if recording stop, else update string.
	if(ui_recorder_isrecording())
	{
		ui_recorder_end();
		ui_recorder_stop();
		MT_ASSERT(ui_recorder_release() == SUCCESS);

		ui_comm_timedit_set_static_txt(ctrl_get_parent(p_text), IDS_START);
		ctrl_paint_ctrl(ctrl_get_parent(p_text), TRUE);
		return SUCCESS;
	}
	else
	{
		if(!ui_signal_is_lock())
		{
			ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_SIGNAL, NULL, 2000);
			return SUCCESS;
		}
		ui_comm_timedit_set_static_txt(ctrl_get_parent(p_text), IDS_STOP);
		ctrl_paint_ctrl(ctrl_get_parent(p_text), TRUE);
	}

	p_cont = ctrl_get_parent(ctrl_get_parent(p_text));  
	p_freq = ctrl_get_child_by_id(p_cont, IDC_TS_RECORD_FREQ);
	#ifndef DTMB_PROJECT
	p_sym = ctrl_get_child_by_id(p_cont, IDC_TS_RECORD_SYM);
	p_demod = ctrl_get_child_by_id(p_cont, IDC_TS_RECORD_DEMOD);
	#endif

	#ifdef DTMB_PROJECT
	sprintf(file_name, "%ld", ui_comm_numedit_get_num(p_freq));
	#else
	sprintf(file_name, "%ld-%ld", ui_comm_numedit_get_num(p_freq), ui_comm_numedit_get_num(p_sym));
	#endif

	//check if can record
	DEBUG(MAIN,INFO,"file_name=%s para2=%d \n",file_name,para2);
	if(para2 != 0)
	{
		memcpy(&total_rec_time, (u32 *)para1, sizeof(utc_time_t));
	}
	else
	{
		control_t *p_start = NULL;
		memset(&total_rec_time, 0, sizeof(utc_time_t));

		p_start = ctrl_get_child_by_id(p_cont, IDC_TS_RECORD_START);
		ui_comm_timedit_get_time(p_start, &total_rec_time);

		total_rec_time.hour = 2;
		total_rec_time.minute = 0;
		total_rec_time.second = 0;
	}
	g_bar_state = 1;
	g_cur_rec_time = 0;
	g_cur_total_time = 0;
	g_cur_rec_index = 0;
	sys_status_get_osd_set(&osd_set);
	g_time_out = osd_set.timeout;

	memset(&rec_attr, 0, sizeof(mul_pvr_rec_attr_t));
	rec_attr.media_info.v_pid = 0;
	rec_attr.b_clear_stream = TRUE;
	rec_attr.media_info.extern_num = 0;

	g_uTotal_rec_time = time_conver(&total_rec_time);
	DEBUG(MAIN,INFO,"g_uTotal_rec_time=%d \n",g_uTotal_rec_time);

	time_get(&gmt_time, TRUE);
	time_to_local(&gmt_time, &(rec_attr.start));
    
	if (file_list_get_partition(&p_partition) > 0)
	{
		memset(rec_attr.file_name, 0, sizeof(u16) * PVR_MAX_FILENAME_LEN);

		uni_strcat(rec_attr.file_name,p_partition[sys_status_get_usb_work_partition()].letter, PVR_MAX_FILENAME_LEN);

		str_asc2uni("\\", uni_tmp);
		uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);


		str_asc2uni(PVR_DIR, uni_tmp);
		uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);

		str_asc2uni("\\", uni_tmp);
		uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);    

		str_asc2uni(file_name, filename);
		uni_strcat(rec_attr.file_name, filename, PVR_MAX_FILENAME_LEN);    

		sprintf(asc_tmp, "-%.4d-%.2d-%.2d %.2d-%.2d-%.2d.ts",
				rec_attr.start.year,
				rec_attr.start.month,
				rec_attr.start.day,
				rec_attr.start.hour,
				rec_attr.start.minute,
				rec_attr.start.second);

		str_asc2uni(asc_tmp, uni_tmp);
		uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);

		ui_recorder_init();
		g_capacity = ui_pvr_get_capacity();
		file = vfs_open(rec_attr.file_name, VFS_NEW);
		if(file == NULL)
		{
			memset(rec_attr.file_name, 0, sizeof(u16) * PVR_MAX_FILENAME_LEN);
			uni_strcat(rec_attr.file_name, p_partition[sys_status_get_usb_work_partition()].letter, PVR_MAX_FILENAME_LEN);

			str_asc2uni("\\", uni_tmp);
			uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);

			str_asc2uni(PVR_DIR, uni_tmp);
			uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);

			str_asc2uni("\\", uni_tmp);
			uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);

			str_asc2uni("No_Name", uni_tmp);

			uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);    

			sprintf(asc_tmp, "-%.4d-%.2d-%.2d %.2d-%.2d-%.2d.ts",
				  rec_attr.start.year,
				  rec_attr.start.month,
				  rec_attr.start.day,
				  rec_attr.start.hour,
				  rec_attr.start.minute,
				  rec_attr.start.second);

			str_asc2uni(asc_tmp, uni_tmp);
			uni_strcat(rec_attr.file_name, uni_tmp, PVR_MAX_FILENAME_LEN);
		}
		else
		{
			vfs_close(file);
			file = NULL;
		}

		rec_attr.file_name_len = uni_strlen(rec_attr.file_name);
		rec_attr.max_file_size = (u64)p_partition->free_size * (u64)KBYTES;
		DEBUG(MAIN,INFO,"free_size = %d max_file_size = %d\n",p_partition->free_size,rec_attr.max_file_size);
		if(p_partition->free_size/KBYTES < 100)
		{
			ui_comm_timedit_set_static_txt(ctrl_get_parent(p_text), IDS_START);
			ctrl_paint_ctrl(ctrl_get_parent(p_text), TRUE);
			ui_comm_cfmdlg_open(NULL, IDS_NO_STORAGE_FOR_RECORD, NULL, 0);
			return SUCCESS;
		}
	}
	else
	{
		//no available storage found
		ui_comm_timedit_set_static_txt(ctrl_get_parent(p_text), IDS_START);
		ctrl_paint_ctrl(ctrl_get_parent(p_text), TRUE);
		ui_comm_cfmdlg_open(NULL, IDS_NO_STORAGE_FOR_RECORD, NULL, 0);
		return SUCCESS;
	}

	ui_epg_stop();
	ui_epg_release();

	rec_attr.stream_type = MUL_PVR_STREAM_TYPE_ALL_TS;
	if(ui_recorder_start(&rec_attr) == ERR_FAILURE)
	{
		ui_recorder_end();
		ui_recorder_stop();
		ui_recorder_release();
		ui_epg_init();
		#ifndef IMPL_NEW_EPG
		ui_epg_start();
		#else
		ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
		#endif
		return ERR_FAILURE;
	}

	return SUCCESS;
}

//combobox
#ifndef DTMB_PROJECT
static RET_CODE on_ts_record_cbox_changed(control_t *p_cbox, u16 msg, u32 para1,u32 para2)
{
	RET_CODE ret = SUCCESS;

	ret = cbox_class_proc(p_cbox, msg, para1, para2);
	ts_record_set_transpond(ctrl_get_parent(ctrl_get_parent(p_cbox)));

	return SUCCESS;
}
#endif

//2014-04-16 modify for ui_close_all_menus() when book pg arrived.
static RET_CODE on_ts_record_destroy(control_t *cont, u16 msg, u32 para1,u32 para2)
{
	if(ui_recorder_isrecording())
	{
		ui_recorder_end();
		ui_recorder_stop();

		MT_ASSERT(ui_recorder_release() == SUCCESS);
	}

	return ERR_NOFEATURE;
}

static RET_CODE on_ts_rec_plug_out(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	partition_t *p_partition = NULL;
	file_list_get_partition(&p_partition);

	if(ui_usb_get_cur_used(p_partition[sys_status_get_usb_work_partition()].letter[0]) != ui_usb_get_plug_partition_sev())
	{
		return SUCCESS;
	}

	if(ui_recorder_isrecording())
	{
		ui_recorder_end();
		ui_recorder_stop();

		MT_ASSERT(ui_recorder_release() == SUCCESS);
	}

	manage_close_menu(ROOT_ID_TS_RECORD, 0, 0);

	return SUCCESS;
}							

static RET_CODE on_ts_record_change_freq_symbol(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	ui_comm_num_proc(p_ctrl, msg, para1, para2);
	ts_record_set_transpond(p_ctrl->p_parent->p_parent);
	return SUCCESS;
}

static RET_CODE on_ts_record_signal_update(control_t *p_ctrl,u16 msg,u32 para1,u32 para2)
{
	control_t *p_start;
	struct signal_data *data;

	p_start = ctrl_get_child_by_id(p_ctrl, IDC_TS_RECORD_START);
	data = (struct signal_data *)(para1);

	if(!data->lock && ui_recorder_isrecording())
	{
		ui_comm_cfmdlg_open(NULL, IDS_MSG_NO_SIGNAL, NULL, 2000);

		ui_recorder_end();
		ui_recorder_stop();
		MT_ASSERT(ui_recorder_release() == SUCCESS);

		if(p_start != NULL)
		{
			ui_comm_timedit_set_static_txt(p_start, IDS_START);
			ctrl_paint_ctrl(p_start, TRUE);
		}
	}

	return SUCCESS;
}

BEGIN_KEYMAP(ts_record_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(ts_record_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(ts_record_cont_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_DESTROY, on_ts_record_destroy)

	ON_COMMAND(MSG_PVR_REC_STOPPED, on_pvr_rec_stopped)
	ON_COMMAND(MSG_PVR_REC_MEM_NOT_ENOUGH, on_pvr_rec_mem_not_enough)
	ON_COMMAND(MSG_WRITE_ERROR, on_pvr_rec_write_error)
	ON_COMMAND(MSG_PVR_REC_NO_SIGNAL, on_pvr_rec_no_signal)
	ON_COMMAND(MSG_PVR_REC_UPDATED, on_pvr_rec_updated)	
	ON_COMMAND(MSG_PLUG_OUT, on_ts_rec_plug_out)
	ON_COMMAND(MSG_SIGNAL_UPDATE, on_ts_record_signal_update)
END_MSGPROC(ts_record_cont_proc, ui_comm_root_proc)

#ifndef DTMB_PROJECT
BEGIN_MSGPROC(ts_record_cbox_proc, cbox_class_proc)
	ON_COMMAND(MSG_CHANGED, on_ts_record_cbox_changed)
END_MSGPROC(ts_record_cbox_proc, cbox_class_proc)
#endif

BEGIN_MSGPROC(ts_record_nbox_proc, nbox_class_proc)
	ON_COMMAND(MSG_NUMBER, on_ts_record_change_freq_symbol)
END_MSGPROC(ts_record_nbox_proc, nbox_class_proc)


BEGIN_KEYMAP(ts_record_time_keymap, ui_comm_tbox_keymap)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(ts_record_time_keymap, ui_comm_tbox_keymap)

BEGIN_MSGPROC(ts_record_time_proc, ui_comm_time_proc)
	ON_COMMAND(MSG_SELECT, on_ts_record_select)
	ON_COMMAND(MSG_EXIT, on_ts_record_exit_edit_sts)
END_MSGPROC(ts_record_time_proc, ui_comm_time_proc)

