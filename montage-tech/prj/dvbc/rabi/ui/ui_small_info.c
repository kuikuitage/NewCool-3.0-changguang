#include "ui_common.h"
#include "ui_small_info.h"
#include "ui_xextend.h"


enum{
	IDC_CONT = 1,
	IDC_PROGRAM_NAME, 
	IDC_PROGRAM_NAME_CONTENT, 

	IDC_BAR_CN,
	IDC_BAR_BER,
	IDC_BAR_RFLEVEL,

	IDC_SERVICE_ID , 
	IDC_TS_ID, 
	IDC_FREQUECY, 
	IDC_A_PID, 
	IDC_SYMBOLS, 
	IDC_V_PID, 
	IDC_MODULATION,
	IDC_PCR_PID,
	
	IDC_SERVICE_ID_CONTENT,
	IDC_TS_ID_CONTENT, 
	IDC_FREQUECY_CONTENT, 
	IDC_A_PID_CONTENT, 
	IDC_SYMBOLS_CONTENT, 
	IDC_V_PID_CONTENT, 
	IDC_MODULATION_CONTENT,
	IDC_PCR_PID_CONTENT,
	
};

u16 small_info_cont_keymap(u16 key);
RET_CODE small_info_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static control_t *create_menucont(btn_rec_t left_btn_rec)
{
	control_t *p_ctrl;
	u16 y;

	y = left_btn_rec.y-SMALL_INFO_CONT_H/2;
	if(left_btn_rec.y<(SMALL_INFO_CONT_H/2+SMALL_INFO_CONT_GARY))
		y = SMALL_INFO_CONT_GARY;
	else 
		y = left_btn_rec.y-SMALL_INFO_CONT_H/2;
	if((y+SMALL_INFO_CONT_H+SMALL_INFO_CONT_GARY)>SCREEN_HEIGHT)
		y = SCREEN_HEIGHT - SMALL_INFO_CONT_H-SMALL_INFO_CONT_GARY;
	p_ctrl = fw_create_mainwin(ROOT_ID_XSMALL_INFO, left_btn_rec.x+left_btn_rec.w+EXTEND_AND_MENU_GAP,
								y, SMALL_INFO_CONT_W, SMALL_INFO_CONT_H, 
								ROOT_ID_XEXTEND, 0, OBJ_ATTR_ACTIVE, 0);
	ctrl_set_proc(p_ctrl, small_info_cont_proc);
	p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_CONT, 0, 0, 
				SMALL_INFO_CONT_W, SMALL_INFO_CONT_H, p_ctrl, 0);
	ctrl_set_rstyle(p_ctrl, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);
	ctrl_set_keymap(p_ctrl, small_info_cont_keymap);
	ctrl_set_proc(p_ctrl, small_info_cont_proc);
	return p_ctrl;
}

RET_CODE open_small_info(u32 para1, u32 para2)
{
#define OPT_X					20
#define OPT_Y					20
#define OPT_H					40
#define SIGN_BAR_PERCENT_W		110
#define SIGN_BAR_STXT_W			110
#define SIGN_BAR_STXT_H			36
#define SIGNE_BAR_H				16
#define SIGN_BAR_W				(SMALL_INFO_CONT_W-2*OPT_X-SIGN_BAR_PERCENT_W-SIGN_BAR_STXT_W)
#define OPT_CONTENT_X			120
#define OPT_CNT					8
#define LEFT_SHORT_OPT_W		(SMALL_INFO_CONT_W/2+2*OPT_X)
#define RIGHT_SHORT_OPT_W		(SMALL_INFO_CONT_W-LEFT_SHORT_OPT_W)
	control_t *p_cont, *p_opt, *p_content, *p_ctrl;
	u8 i;
	u8 opt_cnt = 0;
	u16 y;
	u16 pg_id;
	dvbs_prog_node_t pg;
	dvbs_tp_node_t tp;
	u16 bar_y, percent_y;
	u8 str_buf[64];
	u16 modu_ids_item[9] = 
	{
		IDS_AUTO,IDS_BPSK,IDS_QPSK,IDS_8PSK,
		IDS_QAM16,IDS_QAM32,IDS_QAM64,IDS_QAM128,
		IDS_QAM256
	};
	u16 stxt_ids[] ={IDS_CN, IDS_BER, IDS_RFLEVEL};
	u16 ids_item[] = {IDS_SERVICE_ID, IDS_TS_ID, IDS_FREQUECY2, IDS_A_PID2, 
					IDS_SYMBOLS2, IDS_V_PID2, IDS_MODULATION2,IDS_PCR_PID2};
	
	btn_rec_t *left_btn_rec = (btn_rec_t*)para1;
	pg_id = sys_status_get_curn_group_curn_prog_id();
	if (db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
	{
		DEBUG(MAIN, ERR, "can NOT get pg!\n");
		return ERR_FAILURE;
	}
	db_dvbs_get_tp_by_id((u16)pg.tp_id, &tp);
	
	p_cont = create_menucont(*left_btn_rec);
	
	p_opt = ctrl_create_ctrl(CTRL_TEXT, IDC_PROGRAM_NAME, OPT_X, OPT_Y, 
					SMALL_INFO_CONT_W-2*OPT_X, OPT_H, p_cont, 0);
	text_set_font_style(p_opt, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_align_type(p_opt, STL_LEFT | STL_VCENTER);
	text_set_content_type(p_opt, TEXT_STRTYPE_STRID);
	text_set_content_by_strid(p_opt, IDS_PROGRAM_NAME);
	p_content = ctrl_create_ctrl(CTRL_TEXT, IDC_PROGRAM_NAME_CONTENT, OPT_CONTENT_X, 0, 
					SMALL_INFO_CONT_W-2*OPT_X-OPT_CONTENT_X, OPT_H, p_opt, 0);
	text_set_font_style(p_content, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
	text_set_align_type(p_content, STL_LEFT | STL_VCENTER);
	text_set_content_type(p_content, TEXT_STRTYPE_UNICODE);
	text_set_content_by_unistr(p_content, pg.name);
	
	y = OPT_Y+OPT_H;
	if(CUSTOMER_ID == CUSTOMER_DTMB_DESAI_JIMO)
		opt_cnt = 2;
	else
		opt_cnt = OPT_CNT;

	for(i = 0; i<opt_cnt; i++)
	{
		p_opt = ctrl_create_ctrl(CTRL_TEXT, IDC_SERVICE_ID+i,
						((i%2)==0?OPT_X:(LEFT_SHORT_OPT_W)), y, 
						((i%2)==0?(LEFT_SHORT_OPT_W):(RIGHT_SHORT_OPT_W)), OPT_H, p_cont, 0);
		text_set_font_style(p_opt, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
		text_set_align_type(p_opt, STL_LEFT | STL_VCENTER);
		text_set_content_type(p_opt, TEXT_STRTYPE_STRID);
		text_set_content_by_strid(p_opt, ids_item[i]);
		p_content = ctrl_create_ctrl(CTRL_TEXT, IDC_SERVICE_ID+i+OPT_CNT, OPT_CONTENT_X, 0, 
									((i%2)==0?(LEFT_SHORT_OPT_W):(RIGHT_SHORT_OPT_W))-OPT_CONTENT_X, 
									OPT_H, p_opt, 0);
		text_set_font_style(p_content, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
		text_set_align_type(p_content, STL_LEFT | STL_VCENTER);
		switch(IDC_SERVICE_ID+i+OPT_CNT)
		{
		case IDC_SERVICE_ID_CONTENT:
			text_set_content_type(p_content, TEXT_STRTYPE_DEC);
			text_set_content_by_dec(p_content, pg.s_id);
			break;
		case IDC_TS_ID_CONTENT:
			text_set_content_type(p_content, TEXT_STRTYPE_DEC);
			text_set_content_by_dec(p_content, pg.ts_id);
			break;
		case IDC_FREQUECY_CONTENT: 
			text_set_content_type(p_content, TEXT_STRTYPE_UNICODE);
			sprintf(str_buf, "%ld MBaud", tp.freq/1000);
			text_set_content_by_ascstr(p_content, str_buf);
			break;
		case IDC_A_PID_CONTENT:
			text_set_content_type(p_content, TEXT_STRTYPE_DEC);
			text_set_content_by_dec(p_content, pg.audio[pg.audio_channel].p_id);
			break;
		case IDC_SYMBOLS_CONTENT: 
			text_set_content_type(p_content, TEXT_STRTYPE_UNICODE);
			sprintf(str_buf, "%01ld.%03ld MHZ", tp.sym/1000, tp.sym%1000);
			text_set_content_by_ascstr(p_content, str_buf);
			break;
		case IDC_V_PID_CONTENT:
			text_set_content_type(p_content, TEXT_STRTYPE_DEC);
			text_set_content_by_dec(p_content, pg.video_pid);
			break;
		case IDC_MODULATION_CONTENT:
			text_set_content_type(p_content, TEXT_STRTYPE_STRID);
			text_set_content_by_strid(p_content, modu_ids_item[tp.nim_modulate]);
			break;
		case IDC_PCR_PID_CONTENT:
			text_set_content_type(p_content, TEXT_STRTYPE_DEC);
			text_set_content_by_dec(p_content, pg.pcr_pid);
			break;
		}
		if(1==(i%2))
			y +=OPT_H;
	}

	percent_y = SMALL_INFO_CONT_H - 3*SIGN_BAR_STXT_H-20;
	bar_y = percent_y+(SIGN_BAR_STXT_H - SIGNE_BAR_H)/2;
	for(i = 0; i < 3; i++)
	{
		p_ctrl = ui_comm_bar_create(p_cont, (u8)(IDC_BAR_CN + i),
		                            OPT_X+SIGN_BAR_STXT_W+SIGN_BAR_PERCENT_W, bar_y,
		                            SIGN_BAR_W,
		                            SIGNE_BAR_H,
		                            OPT_X, percent_y,
		                            SIGN_BAR_STXT_W,
		                            SIGN_BAR_STXT_H,
		                            OPT_X+SIGN_BAR_STXT_W, percent_y,
		                            SIGN_BAR_PERCENT_W,
		                            SIGN_BAR_STXT_H);
		ui_comm_bar_set_style(p_ctrl,
		                      RSI_ROOT_SIGN_BAR,
		                      RSI_ROOT_SIGN_BAR_MID_1,
		                      RSI_ROOT_SIGN_BAR_STXT,
		                      FSI_ROOT_SIGN_BAR_STXT,
		                      RSI_ROOT_SIGN_BAR_PERCENT,
		                      FSI_ROOT_SIGN_BAR_PERCENT);
		ui_comm_bar_set_param(p_ctrl, stxt_ids[i], 0, 100, 100);
		if(i == 0)
			ui_comm_tp_bar_update(p_ctrl, 0, TRUE, (u8*)"dB");
		else if(i == 1)
			ui_comm_tp_bar_update(p_ctrl, 0, TRUE, (u8*)"E-6");
		else if(i == 2)
			ui_comm_tp_bar_update(p_ctrl, 0, TRUE, (u8*)"dBuv");

		bar_y += SIGN_BAR_STXT_H;
		percent_y += SIGN_BAR_STXT_H;
	}

	ui_enable_signal_monitor_by_tuner(TRUE, 0);

	ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(p_cont, FALSE);

	stop_extend_timer();
	return SUCCESS;
}

static RET_CODE small_info_signal_update(control_t *p_root,u16 msg,
                                 						u32 para1,u32 para2)
{
	struct signal_data *data = (struct signal_data *)(para1);
	control_t *p_ctrl, *p_cont;

	p_cont = ctrl_get_child_by_id(p_root, IDC_CONT);
	p_ctrl = ctrl_get_child_by_id(p_cont, IDC_BAR_CN);
#ifdef DTMB_PROJECT
	update_comm_signbar(p_ctrl, data->intensity, data->lock);
#else
	update_comm_tp_signbar(p_ctrl, data->intensity, data->lock, (u8*)"dB");
#endif
	p_ctrl = ctrl_get_child_by_id(p_cont, IDC_BAR_BER);
#ifdef DTMB_PROJECT
	update_comm_signbar(p_ctrl, data->ber, data->lock);
#else
	update_comm_tp_signbar(p_ctrl, data->ber, data->lock, (u8*)"E-6");
#endif
	p_ctrl = ctrl_get_child_by_id(p_cont, IDC_BAR_RFLEVEL);
#ifdef DTMB_PROJECT
	update_comm_signbar(p_ctrl, data->quality, data->lock);
#else
	update_comm_tp_signbar(p_ctrl, data->quality, data->lock, (u8*)"dBuv");
#endif
	return SUCCESS;
}

static RET_CODE small_info_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	control_t *p_ctrl;
	p_ctrl = get_extend_cont1_ctrl();
	start_extend_timer();
	ctrl_process_msg(p_ctrl, msg, 0, 0);//exit and change xextend btn focus
	return SUCCESS;
}

static RET_CODE send_to_reset_extend_timer(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	return reset_extend_timer();
}

BEGIN_KEYMAP(small_info_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
	ON_EVENT(V_KEY_EXIT, MSG_EXIT_ALL)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_HOT_XEXTEND, MSG_EXIT)
	//ON_EVENT(V_KEY_OK, MSG_XEXTEND_RESET_TIMER)
	//ON_EVENT(V_KEY_LEFT, MSG_XEXTEND_RESET_TIMER)
	//ON_EVENT(V_KEY_RIGHT, MSG_XEXTEND_RESET_TIMER)
END_KEYMAP(small_info_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(small_info_cont_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_FOCUS_UP, small_info_exit)
	ON_COMMAND(MSG_FOCUS_DOWN, small_info_exit)
	ON_COMMAND(MSG_EXIT, small_info_exit)
	ON_COMMAND(MSG_EXIT_ALL, small_info_exit)
	ON_COMMAND(MSG_SIGNAL_UPDATE, small_info_signal_update)
	ON_COMMAND(MSG_XEXTEND_RESET_TIMER, send_to_reset_extend_timer)
END_MSGPROC(small_info_cont_proc, ui_comm_root_proc)


