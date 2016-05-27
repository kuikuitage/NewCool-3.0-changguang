/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "ui_common.h"
#include "mtos_mem.h"
#include "ui_ads_display.h"
#include "ui_ad_api.h"
#include "ui_config.h"
#include "pdec.h"
#include "ads_api_divi.h"
#include "ui_ca_public.h"
#include "ui_ad_gif_api.h"

typedef struct{
	BOOL text_roll_over;
	BOOL text_roll_hide;
	BOOL text_roll_wait;

	BOOL pic_roll_over;
	BOOL pic_roll_hide;
	BOOL pic_roll_wait;

	BOOL close_ads_pic_show;
}divi_ads_rolling_state_t;
static divi_ads_rolling_state_t divi_ads_rolling_state;

static u8 g_divi_osd_data[63*1024] = {0,};
static control_t *g_divi_osd_ctrl = NULL;


static u32 g_divi_ads_pg_filter[80] = {0,};
static u8 g_ads_filter_cnt = 0;

extern BOOL get_ca_msg_rolling_status(void);
extern BOOL get_ca_osd_hide_state(void);
extern RET_CODE on_ca_rolling_show(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE ui_ads_roll_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

void ui_ads_osd_init_param(void)
{
	divi_ads_rolling_state.pic_roll_hide = TRUE;
	divi_ads_rolling_state.pic_roll_over = TRUE;
	divi_ads_rolling_state.pic_roll_wait = FALSE;

	divi_ads_rolling_state.text_roll_hide = TRUE;
	divi_ads_rolling_state.text_roll_over = TRUE;
	divi_ads_rolling_state.text_roll_wait = FALSE;

	divi_ads_rolling_state.close_ads_pic_show = FALSE;
}

BOOL ui_get_ads_osd_roll_over(void)
{
	DEBUG(ADS,INFO,"!!!!!!!\n");
	if(divi_ads_rolling_state.pic_roll_over && divi_ads_rolling_state.text_roll_over)
		return TRUE;

	return FALSE;
}

static BOOL ui_get_ads_osd_hide(void)
{
	DEBUG(ADS,INFO,"!!!!!!!\n");
	if(divi_ads_rolling_state.pic_roll_hide && divi_ads_rolling_state.text_roll_hide)
		return TRUE;

	return FALSE;
}

static BOOL ui_ads_osd_whether_noshow(u8 ad_type)
{
	if(get_ca_osd_hide_state() && (ROOT_ID_BACKGROUND == fw_get_focus_id()))
	{
		if(TRUE == divi_ads_rolling_state.pic_roll_over &&
			TRUE == divi_ads_rolling_state.text_roll_over)
		{
			return FALSE;
		}
	}
	return TRUE;
}

RET_CODE ui_ads_text_rolling_show(void)
{
	control_t * p_ctrl_top = NULL;
	control_t * p_ctrl_bottom = NULL;
	control_t * p_ctrl_middle = NULL;
	control_t *p_background = NULL;

	p_background = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	p_ctrl_top = ctrl_get_child_by_id(p_background, IDC_ADS_ROLL_TOP);
	p_ctrl_middle = ctrl_get_child_by_id(p_background, IDC_ADS_ROLL_MIDDLE);
	p_ctrl_bottom = ctrl_get_child_by_id(p_background, IDC_ADS_ROLL_BOTTOM);
  
	if((NULL != p_ctrl_top) && (ctrl_is_rolling(p_ctrl_top))&&(divi_ads_rolling_state.text_roll_hide))
	{
		ctrl_set_sts(p_ctrl_top,OBJ_STS_SHOW);
		gui_resume_roll(p_ctrl_top);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(TRUE);
	}
	if((NULL != p_ctrl_middle) && (ctrl_is_rolling(p_ctrl_middle))&&(divi_ads_rolling_state.text_roll_hide))
	{
		ctrl_set_sts(p_ctrl_middle,OBJ_STS_SHOW);
		gui_resume_roll(p_ctrl_middle);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(TRUE);
	}  
	if((NULL != p_ctrl_bottom) && (ctrl_is_rolling(p_ctrl_bottom))&&(divi_ads_rolling_state.text_roll_hide))
	{
		ctrl_set_sts(p_ctrl_bottom,OBJ_STS_SHOW);
		gui_resume_roll(p_ctrl_bottom);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(TRUE);
	}

	DEBUG(ADS,INFO,"!!!!!!!\n");
	return SUCCESS;
}

RET_CODE ui_ads_text_rolling_hide(void)
{
	control_t * p_ctrl_top = NULL;
	control_t * p_ctrl_middle = NULL;  
	control_t * p_ctrl_bottom = NULL;
	control_t *p_background = NULL;

	p_background = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	p_ctrl_top = ctrl_get_child_by_id(p_background, IDC_ADS_ROLL_TOP);
	p_ctrl_middle = ctrl_get_child_by_id(p_background, IDC_ADS_ROLL_MIDDLE);
	p_ctrl_bottom = ctrl_get_child_by_id(p_background, IDC_ADS_ROLL_BOTTOM);
 
	if((NULL != p_ctrl_top) && (ctrl_is_rolling(p_ctrl_top)))
	{
		ctrl_set_sts(p_ctrl_top,OBJ_STS_HIDE);
		gui_pause_roll(p_ctrl_top);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(FALSE);
	}
	if((NULL != p_ctrl_middle) && (ctrl_is_rolling(p_ctrl_middle)))
	{
		ctrl_set_sts(p_ctrl_middle,OBJ_STS_HIDE);
		gui_pause_roll(p_ctrl_middle);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(FALSE);
	}  
	if((NULL != p_ctrl_bottom) && (ctrl_is_rolling(p_ctrl_bottom)))
	{
		ctrl_set_sts(p_ctrl_bottom,OBJ_STS_HIDE);
		gui_pause_roll(p_ctrl_bottom);
		ctrl_paint_ctrl(p_background, TRUE);
		ui_time_enable_heart_beat(FALSE);
	}

	DEBUG(ADS,INFO,"!!!!!!!\n");
	return SUCCESS;
}

static RET_CODE on_ads_rolling_stop(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	rect_t frame = {0};
	control_t *p_parent = NULL;

	MT_ASSERT(p_ctrl != NULL);
	DEBUG(ADS,INFO,"[DIVI]Rolling ads stopped!\n");

	ctrl_set_sts(p_ctrl,OBJ_STS_HIDE);
	ctrl_get_frame(p_ctrl, &frame);

	gui_stop_roll(p_ctrl);
	p_parent = ctrl_get_parent(p_ctrl);
	MT_ASSERT(p_parent != NULL);

	ctrl_add_rect_to_invrgn(p_parent, &frame);
	ctrl_paint_ctrl(p_parent, FALSE);
	DEBUG(ADS,INFO,"[DIVI]Rolling ads over!\n");

	divi_ads_rolling_state.text_roll_over = TRUE;
	g_divi_osd_ctrl = NULL;
	ui_time_enable_heart_beat(FALSE);
	return SUCCESS;
}

extern rsc_rstyle_t c_tab[MAX_RSTYLE_CNT];
RET_CODE create_ads_rolling_ctrl(void)
{
#define ADS_ROLLING_XMOVE		(400)
#define ADS_ROLLING_X			(0)
#define ADS_ROLLING_W			(SCREEN_WIDTH - ADS_ROLLING_X)
#define ADS_ROLLING_H			(60)
#define ADS_ROLLING_TOP_Y		(38)
#define ADS_ROLLING_MIDDLE_Y	(330)
#define ADS_ROLLING_BUTTON_Y	(SCREEN_HEIGHT - ADS_ROLLING_H - 20)
	control_t *p_cont = NULL;
	control_t *p_txt_top = NULL;
	control_t *p_txt_middle = NULL;
	control_t *p_txt_bottom = NULL;

	p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);

	if (p_cont == NULL)
	{
		DEBUG(ADS,ERR,"fw_find_root_by_id ROOT_ID_BACKGROUND fail \n");
		return ERR_FAILURE;
	}

  	c_tab[RSI_ROLL_OSD].bg.value = 0x00000000;
	if(ctrl_get_child_by_id(p_cont,IDC_ADS_ROLL_TOP) == NULL)
	{
		p_txt_top = ctrl_create_ctrl(CTRL_TEXT, IDC_ADS_ROLL_TOP,
				          (ADS_ROLLING_XMOVE + ADS_ROLLING_X), ADS_ROLLING_TOP_Y,
				          (ADS_ROLLING_W - ADS_ROLLING_XMOVE), ADS_ROLLING_H,p_cont, 0);
		ctrl_set_proc(p_txt_top, ui_ads_roll_proc);
		ctrl_set_sts(p_txt_top, OBJ_STS_HIDE);
		ctrl_set_rstyle(p_txt_top,RSI_TRANSPARENT,RSI_TRANSPARENT,RSI_TRANSPARENT);
		text_set_font_style(p_txt_top, FSI_WHITE_36, FSI_WHITE_36, FSI_WHITE_36);
		text_set_align_type(p_txt_top,STL_LEFT|STL_VCENTER);
		text_set_content_type(p_txt_top, TEXT_STRTYPE_UNICODE);
	}
	
	if(ctrl_get_child_by_id(p_cont,IDC_ADS_ROLL_BOTTOM) == NULL)
	{
		p_txt_bottom= ctrl_create_ctrl(CTRL_TEXT, IDC_ADS_ROLL_BOTTOM,
		                  ADS_ROLLING_X, ADS_ROLLING_BUTTON_Y,
		                  ADS_ROLLING_W, ADS_ROLLING_H,p_cont, 0);
		ctrl_set_proc(p_txt_bottom, ui_ads_roll_proc);
		ctrl_set_sts(p_txt_bottom,OBJ_STS_HIDE);
		ctrl_set_rstyle(p_txt_bottom, RSI_ROLL_OSD, RSI_ROLL_OSD, RSI_ROLL_OSD);
		text_set_font_style(p_txt_bottom, FSI_WHITE_36, FSI_WHITE_36, FSI_WHITE_36);
		text_set_align_type(p_txt_bottom,STL_LEFT|STL_VCENTER);
		text_set_content_type(p_txt_bottom, TEXT_STRTYPE_UNICODE);
	}

	if(ctrl_get_child_by_id(p_cont,IDC_ADS_ROLL_MIDDLE) == NULL)
	{
		p_txt_middle= ctrl_create_ctrl(CTRL_TEXT, IDC_ADS_ROLL_MIDDLE,
					                  ADS_ROLLING_X, ADS_ROLLING_MIDDLE_Y,
					                  ADS_ROLLING_W, ADS_ROLLING_H,p_cont, 0);
		ctrl_set_proc(p_txt_middle, ui_ads_roll_proc);
		ctrl_set_sts(p_txt_middle,OBJ_STS_HIDE);
		ctrl_set_rstyle(p_txt_middle, RSI_ROLL_OSD, RSI_ROLL_OSD, RSI_ROLL_OSD);
		text_set_font_style(p_txt_middle, FSI_WHITE_36, FSI_WHITE_36, FSI_WHITE_36);
		text_set_align_type(p_txt_middle,STL_LEFT|STL_VCENTER);
		text_set_content_type(p_txt_middle, TEXT_STRTYPE_UNICODE);
	}
	DEBUG(ADS,ERR,"creat success\n");
	return SUCCESS;
}

void divi_ads_update_text_osd_show(BOOL direction)
{
	control_t * p_ctrl_bottom = NULL;
	control_t *p_background = NULL;
	rect_t  rect = {0};

	p_background = fw_find_root_by_id(ROOT_ID_BACKGROUND);
	p_ctrl_bottom = ctrl_get_child_by_id(p_background, IDC_ADS_ROLL_BOTTOM);
  	ctrl_get_frame(p_ctrl_bottom, &rect);
		
	if((NULL != p_ctrl_bottom) && (ctrl_is_rolling(p_ctrl_bottom)))
	{
		DEBUG(ADS,INFO,"p_ctrl_bottom top = %d button = %d\n",rect.top, rect.bottom);
		if(direction)
		{
			if(rect.top < 600)
				ctrl_move_ctrl(p_ctrl_bottom,0,170);
		}
		else
		{
			if(rect.top > 600)
				ctrl_move_ctrl(p_ctrl_bottom,0,-170);
		}
		ctrl_paint_ctrl(p_ctrl_bottom,TRUE);
		ctrl_get_frame(p_ctrl_bottom, &rect);
	}

}


void on_ads_start_roll_osd(u8 pos, u8 *p_data)
{
	control_t *p_cont= NULL;
	control_t *p_ctrl = NULL;
	roll_param_t p_param ;
	u16 *uni_str = NULL;

	uni_str = (u16 *)mtos_malloc(1024);
	MT_ASSERT(uni_str != NULL);
	memset(uni_str, 0, 1024);
	gb2312_to_unicode(p_data, 512, uni_str, 512);
  
	p_cont = fw_find_root_by_id(ROOT_ID_BACKGROUND);

	if (pos == 0)
	{
		p_ctrl = ctrl_get_child_by_id(p_cont, IDC_ADS_ROLL_BOTTOM);
	}
	else if (pos == 1)
	{
		p_ctrl = ctrl_get_child_by_id(p_cont, IDC_ADS_ROLL_MIDDLE);
	}
	else
	{
		p_ctrl = ctrl_get_child_by_id(p_cont, IDC_ADS_ROLL_TOP);
	} 

	if (p_ctrl != NULL)
	{
		g_divi_osd_ctrl = p_ctrl;
	}

	gui_stop_roll(p_ctrl);
	ctrl_set_sts(p_ctrl, OBJ_STS_SHOW);
	text_set_content_by_unistr(p_ctrl, uni_str);

	memset(&p_param, 0, sizeof(roll_param_t));
	p_param.pace = ROLL_SINGLE;
	p_param.style = ROLL_LR;
	p_param.is_force = TRUE;
	p_param.repeats= 1;
	gui_start_roll(p_ctrl, &p_param);
	ctrl_paint_ctrl(p_cont,TRUE);

	mtos_free(uni_str);
	uni_str = NULL;
	divi_ads_rolling_state.text_roll_over = FALSE;
	divi_ads_rolling_state.text_roll_hide = FALSE;
}


/*priority show ads to hide cas osd*/
void divi_show_text_osd(void)
{
	u8 is_show = 0;
	u8 show_time = 0;
	u8 show_pos = 0;
	u8 file_type = 0;
	u32 filesize = 0;
	u8 *p_data = NULL;
	u16 pg_id = 0;
	dvbs_prog_node_t pg = {0};

	if(ui_ads_osd_whether_noshow(IDN_ADS_TEXT_ROLLING))
	{
		divi_ads_rolling_state.text_roll_wait = TRUE;
		return;
	}
	divi_ads_rolling_state.text_roll_wait = FALSE;

	if((pg_id = sys_status_get_curn_group_curn_prog_id()) == INVALIDID)
		return;
	if (db_dvbs_get_pg_by_id(pg_id, &pg) != DB_DVBS_OK)
		return;

	DEBUG(ADS,INFO,"pg_id=%d, s_id=%d\n", pg_id, pg.s_id);

	if (TRUE == divi_ads_filter_for_pg(pg.s_id))
	{
		DEBUG(ADS,INFO,"Current pg can't show ads!\n");
		return;
	}

	p_data = g_divi_osd_data;
	memset(p_data, 0, 63 * KBYTES);
	filesize = divi_ads_get_data(IDN_ADS_TEXT_ROLLING , p_data, &is_show, &show_time, &show_pos, &file_type);
	DEBUG(ADS,INFO,"divi_ads_get_data:filesize=%d,is_show=%d,show_time=%d,show_pos=%d,file_type=%d\n", 
	    filesize,is_show,show_time,show_pos,file_type);

	if((is_show == 0) || (filesize == 0))
	{
		DEBUG(ADS,INFO,"Cancel osd display, is_show=%d\n", is_show);
		return;
	}

	on_ads_start_roll_osd(show_pos%3, p_data);
	if(ROOT_ID_PROG_BAR== fw_get_focus_id())
		divi_ads_update_osd_show(TRUE);
	ui_time_enable_heart_beat(TRUE);
	p_data = NULL;
}

RET_CODE ui_ads_pic_rolling_show(void)
{
	if((divi_ads_rolling_state.pic_roll_over == FALSE)&&
		(divi_ads_rolling_state.pic_roll_hide == TRUE))
	{
		divi_ads_pic_rolling_show();
		divi_ads_rolling_state.pic_roll_hide = FALSE;
		DEBUG(ADS,INFO,"!!!!!!!\n");
	}

	DEBUG(ADS,INFO,"!!!!!!!\n");
	return SUCCESS;
}

RET_CODE ui_ads_pic_rolling_hide(void)
{
	if((divi_ads_rolling_state.pic_roll_over == FALSE)&&
		(divi_ads_rolling_state.pic_roll_hide == FALSE))
	{
		divi_ads_pic_rolling_hide();
		divi_ads_rolling_state.pic_roll_hide = TRUE;
		DEBUG(ADS,INFO,"!!!!!!!\n");
	}

	DEBUG(ADS,INFO,"!!!!!!!\n");
	return SUCCESS;
}

void divi_show_pic_osd(void)
{
	if(ui_ads_osd_whether_noshow(IDN_ADS_PIC_ROLLING))
	{
		divi_ads_rolling_state.pic_roll_wait = TRUE;
		DEBUG(ADS,INFO,"!!!!!!!\n");
		return;
	}

	DEBUG(ADS,INFO,"!!!!!!!\n");
	pic_adv_stop();
 	if(SUCCESS == divi_gif_osd_start())
 	{
		divi_ads_rolling_state.pic_roll_wait = FALSE;
		divi_ads_rolling_state.pic_roll_over = FALSE;
		divi_ads_rolling_state.pic_roll_hide = FALSE;
	}
	divi_ads_rolling_state.close_ads_pic_show = TRUE;

}

static void ui_ads_osd_rolling_hide(void)
{
	DEBUG(ADS,INFO,"!!!!!!!\n");
	if(divi_ads_rolling_state.pic_roll_over == FALSE)
	{
		ui_ads_pic_rolling_hide();
		divi_ads_rolling_state.pic_roll_hide = TRUE;
	}
	else if(divi_ads_rolling_state.text_roll_over == FALSE)
	{
		ui_ads_text_rolling_hide();
		divi_ads_rolling_state.text_roll_hide = TRUE;
	}
}

static void ui_ads_osd_rolling_show(void)
{
	DEBUG(ADS,INFO,"!!!!!!!\n");
	if(divi_ads_rolling_state.pic_roll_over == FALSE)
	{
		ui_ads_pic_rolling_show();
		divi_ads_rolling_state.pic_roll_hide = FALSE;
	}
	else if(divi_ads_rolling_state.text_roll_over == FALSE)
	{
		ui_ads_text_rolling_show();
		divi_ads_rolling_state.text_roll_hide = FALSE;
	}
}

void divi_ads_update_osd_show(BOOL direction)
{
	DEBUG(ADS,INFO,"\n");
	if(divi_ads_rolling_state.pic_roll_over == FALSE)
	{
		DEBUG(ADS,INFO,"\n");
		divi_ads_update_pic_osd_show(direction);
	}
	else if(divi_ads_rolling_state.text_roll_over == FALSE)
	{
		DEBUG(ADS,INFO,"\n");
		divi_ads_update_text_osd_show(direction);
	}
}

u8 ui_get_ads_update_osd_state(void)
{
	DEBUG(ADS,INFO,"!!!!!!!\n");
	if(divi_ads_rolling_state.pic_roll_over == FALSE)
		return IDN_ADS_PIC_ROLLING;

	if(divi_ads_rolling_state.text_roll_over == FALSE)
		return IDN_ADS_TEXT_ROLLING;

	return IDN_ADS_INVALID;
}

void ui_set_osd_rolling_state(BOOL state)
{
	static BOOL ads_hide = FALSE;

	if(state){
	  	if(TRUE == ads_hide)
	  	{
			ui_ads_osd_rolling_show();
		}
		else
		{
			on_ca_rolling_show(NULL,0,0,0);
		}

	}else{
		if(FALSE == get_ca_osd_hide_state())
	  	{
			on_ca_rolling_hide(NULL,0,0,0);
			ads_hide = FALSE;
		}
		else if(FALSE == ui_get_ads_osd_hide())
		{
			ui_ads_osd_rolling_hide();
			ads_hide = TRUE;
		}
	}
}

void divi_ads_show_fs(void)
{
	if (ROOT_ID_BACKGROUND == fw_get_focus_id() && ui_get_ads_osd_roll_over())
	{
		DEBUG(ADS,INFO,"focus in ROOT_ID_BACKGROUND \n");
		ui_adv_pic_play(ADS_AD_TYPE_BANNER, ROOT_ID_BACKGROUND);
	}
	else
	{
		if ((ROOT_ID_PROG_BAR == fw_get_focus_id()) ||
					(ROOT_ID_SMALL_LIST == fw_get_focus_id()) ||
					(ROOT_ID_MAINMENU == fw_get_focus_id()) ||
					(ROOT_ID_VOLUME == fw_get_focus_id()))
		{
			DEBUG(ADS,INFO,"focus in fulmenu \n");
		}
		else
		{
			DEBUG(ADS,INFO,"focus in other window\n");
			pic_adv_stop();
		}
	}
}

BOOL divi_ads_filter_for_pg(u16 s_id)
{
	u8 i = 0;

	for (i = 0; i < 80; i++)
	{
		if (s_id == (u16)g_divi_ads_pg_filter[i])
			return TRUE;
	}
	return FALSE;
}

void divi_ad_filter_data_process(void)
{
	u8 i =0;
	u8 is_show = 0;
	u8 show_time = 0;
	u8 show_pos = 0;
	u8 file_type = 0;
	u32 filesize = 0;
	u8 fiter_data[320] = {0};

	filesize = divi_ads_get_data(IDN_ADS_PROGR_FILTER, fiter_data, &is_show, &show_time, &show_pos, &file_type);
	DEBUG(ADS,INFO,"divi_ads_get_data:filesize=%d,is_show=%d,show_time=%d,show_pos=%d,file_type=%d\n", 
	        filesize,is_show,show_time,show_pos,file_type);

	g_ads_filter_cnt = filesize/4;
	DEBUG(ADS,INFO,"ADS filter info,cnt=%d **********************\n", g_ads_filter_cnt);
	for (i = 0; i < g_ads_filter_cnt; i++)
	{
		g_divi_ads_pg_filter[i] = (fiter_data[i*4]-0x30)*1000 + (fiter_data[i*4+1]-0x30)*100 + 
		                          (fiter_data[i*4+2]-0x30)*10 + (fiter_data[i*4+3]-0x30);
		DEBUG(ADS,INFO,"g_divi_ads_pg_filter[%d]=%d\n", i, g_divi_ads_pg_filter[i]);
	}
	DEBUG(ADS,INFO,"****************************************\n");
}

RET_CODE on_ads_time(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	if(divi_ads_rolling_state.pic_roll_wait)
	{
		divi_ads_rolling_state.pic_roll_wait = FALSE;
		divi_show_pic_osd();
	}
	else if(divi_ads_rolling_state.text_roll_wait)
	{
		divi_ads_rolling_state.text_roll_wait = FALSE;
		divi_show_text_osd();
	}
	else if(divi_ads_rolling_state.pic_roll_over && divi_ads_rolling_state.text_roll_over)
	{
		if((FALSE == get_ca_msg_rolling_status())&&get_ca_osd_hide_state())
			on_ca_rolling_show(NULL,0,0,0);
	
		if(divi_ads_rolling_state.close_ads_pic_show)
		{
			divi_ads_rolling_state.close_ads_pic_show = FALSE;
			divi_ads_show_fs();
		}
	}
	return SUCCESS;
}

RET_CODE on_ads_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	u32 ad_type = para2;
	DEBUG(ADS,INFO,"[on_ads_process]ad_type=%d\n", ad_type);
	switch (ad_type)
	{
		case IDN_ADS_CORNER:
			divi_ads_show_fs();
			break;
		case IDN_ADS_TEXT_ROLLING:
			divi_show_text_osd();
			break;
		case IDN_ADS_PIC_ROLLING:
			divi_show_pic_osd();
			break;	
		case IDN_ADS_PROGR_FILTER:
			divi_ad_filter_data_process();
			break;
		case IDN_ADS_PIC_ROLLING_OVER:
			divi_ads_osd_over();
			divi_ads_rolling_state.pic_roll_over = TRUE;
		default:
			break;
	}

	return SUCCESS;
}

void divi_ads_show_init(void)
{
	divi_ads_pic_osd_init();
	ui_ads_osd_init_param();
	create_ads_rolling_ctrl();
}


BEGIN_MSGPROC(ui_ads_roll_proc, text_class_proc)
	ON_COMMAND(MSG_ROLL_STOPPED, on_ads_rolling_stop)
END_MSGPROC(ui_ads_roll_proc, text_class_proc)
 
