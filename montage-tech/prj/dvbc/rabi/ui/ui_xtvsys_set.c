/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_xsys_set.h"
#include "ui_xtvsys_set.h"

enum control_id
{
  IDC_XVIDEO_MODE = 1,
  IDC_XVIDEO_RESOLUTION,    
  IDC_XASPECT_MODE,
  IDC_XDIGITAL_OUTPUT,
  IDC_XVIDEO_EFFECTS,
  //IDC_XINVALID ,
#if ENABLE_TTX_SUBTITLE
  IDC_XSUBTITLE,
  //IDC_XINSERTER,
#endif
  IDC_XTIMEOUT,
  IDC_XTRANSPARENCY,
#ifdef TV_BIRTHNESS_EN
  IDC_XBRIGHTNESS,
#endif
 // IDC_XTRANSCODING
};


extern RET_CODE aud_get_dolby_status(void *p_dev, aud_dolby_status_vsb_t *status);
static RET_CODE xosd_set_select_proc(control_t *ctrl, u16 msg, u32 para1,u32 para2);
static RET_CODE xbright_set_select_proc(control_t *ctrl, u16 msg, u32 para1,u32 para2);
static u16 xtvsys_set_cont_keymap(u16 key);
static RET_CODE xtvsys_set_select_proc(control_t *p_ctrl,u16 msg,u32 para1,u32 para2);
static RET_CODE xtvsys_set_cont_proc(control_t *p_ctrl,u16 msg,u32 para1,u32 para2);
/*
static void transcoding_set(control_t *p_cont, BOOL is_paint)
{
	control_t *p_transcoding = NULL , *p_audio_out = NULL;
	u16 focus;
	p_audio_out = ctrl_get_child_by_id(p_cont, IDC_XDIGITAL_OUTPUT);
	p_transcoding = ctrl_get_child_by_id(p_cont, IDC_XTRANSCODING);

	focus = ui_comm_select_get_focus(p_audio_out);

	ui_comm_ctrl_update_attr(p_transcoding, (BOOL)focus ? TRUE:FALSE );

	if(is_paint)
		ctrl_paint_ctrl(p_transcoding, TRUE);
}
*/

static RET_CODE xtvsys_set_comm_fill_content(control_t *p_ctrl,
                                            u16 max_length)
{
	static u16 video_resolution_sd_str[] = {IDS_PAL, IDS_NTSC, IDS_AUTO};
	static u16 video_resolution_hd_str_50hz[] = {IDS_576I, IDS_576P, IDS_720P, IDS_1080I, IDS_1080P};
	static u16 video_resolution_hd_str_60hz[] = {IDS_480I, IDS_480P, IDS_720P, IDS_1080I, IDS_1080P};  
	static u16 aspect_mode_str[]  = {IDS_AUTO, IDS_4_3LETTERBOX, IDS_4_3PANSCAN, IDS_16_9};
	static u16 audio_output_str[] = {IDS_LPCM_OUT, IDS_BS_OUT};
	static u16 video_effects_str[] = {IDS_SHARPENING, IDS_ADAPTIVE, IDS_SOFT,IDS_STANDARD};
	av_set_t av_set;
	class_handle_t avc_handle = 0;
	disp_sys_t video_std = VID_SYS_AUTO;
	u16 j=0;

	avc_handle = class_get_handle_by_id(AVC_CLASS_ID);

	sys_status_get_av_set(&av_set);
	#ifdef WIN32
	avc_video_switch_chann(avc_handle, DISP_CHANNEL_HD);
	#else
	avc_video_switch_chann(avc_handle, DISP_CHANNEL_SD);
	#endif
	video_std = avc_get_video_mode_1(avc_handle);

     for(j = 0; j < max_length; j++)
	{

    

		switch(ctrl_get_ctrl_id(p_ctrl))
		{
			case IDC_XVIDEO_MODE:
				ui_comm_select_set_content(p_ctrl, j, video_resolution_sd_str[j]);
				break;

			case IDC_XVIDEO_RESOLUTION:
				switch (video_std)
				{ 
					case VID_SYS_NTSC_J:
					case VID_SYS_NTSC_M:
					case VID_SYS_NTSC_443:
					case VID_SYS_PAL_M:
						ui_comm_select_set_content(p_ctrl, j, video_resolution_hd_str_60hz[j]);
						break;

					case VID_SYS_PAL:
					case VID_SYS_PAL_N:
					case VID_SYS_PAL_NC:
					case VID_SYS_1080I_50HZ:
					case VID_SYS_1080P_50HZ:
					case VID_SYS_720P_50HZ:
						ui_comm_select_set_content(p_ctrl, j, video_resolution_hd_str_50hz[j]);
						break;

					default:
						MT_ASSERT(0);
						break;
				}
				break;

			case IDC_XASPECT_MODE:
				ui_comm_select_set_content(p_ctrl, j, aspect_mode_str[j]);
				break;

			case IDC_XDIGITAL_OUTPUT:
				ui_comm_select_set_content(p_ctrl, j, audio_output_str[j]);
				break;
			/*
			case IDC_XTRANSCODING:
			content = audio_transcoding_str[focus];
			break;
			*/

			case IDC_XVIDEO_EFFECTS:
				ui_comm_select_set_content(p_ctrl, j, video_effects_str[j]);
				break;
			default:
				MT_ASSERT(0);
			// return ERR_FAILURE;
		}
	}
	return SUCCESS;
}


RET_CODE open_xtvsys_set(u32 para1, u32 para2)
{
	control_t *p_cont, *p_ctrl[XTVSYS_SET_ITEM_CNT+XOSD_SET_ITEM_CNT];
	u8 i;
	u8 brightness;
	av_set_t av_set;
	osd_set_t osd_set;

#ifndef SPT_SUPPORT
#if ENABLE_TTX_SUBTITLE
	u8 j;
	u16 stxt[XTVSYS_SET_ITEM_CNT+XOSD_SET_ITEM_CNT + ITEM_ECTERN_CNT] =
	{
		IDS_TV_SYSTEM, IDS_VIDEO_RESOLUTION, IDS_ASPECT_MODE, IDS_DIGITAL_AUDIO_OUT, IDS_VIDEO_EFFECTS,
		IDS_SUBTITLE_DISPLAY, /*IDS_VBI_INSERTER,*/
		IDS_OSD_TIMEOUT, IDS_OSD_TRANSPARENCY,IDS_XTVSYS_BIRTHNESS
	};
	u16 opt_data[XOSD_SET_ITEM_CNT + ITEM_ECTERN_CNT][2] = {{ IDS_OFF, IDS_ON },/*{ IDS_OFF, IDS_ON },*/{0 },{0},};
    u8 opt_cnt[XTVSYS_SET_ITEM_CNT+XOSD_SET_ITEM_CNT + ITEM_ECTERN_CNT] = {3, 5, 4, 2, 4, 2, 0, 0,0};
	u16 y;
	u16 droplist_page[XTVSYS_SET_ITEM_CNT+XOSD_SET_ITEM_CNT + ITEM_ECTERN_CNT] = {3, 5, 4, 2, 4, 0, 5, 5,5};
#else
	u16 stxt[XTVSYS_SET_ITEM_CNT+XOSD_SET_ITEM_CNT + ITEM_ECTERN_CNT] =
	{
		IDS_TV_SYSTEM, IDS_VIDEO_RESOLUTION, IDS_ASPECT_MODE, IDS_DIGITAL_AUDIO_OUT, IDS_VIDEO_EFFECTS,
		IDS_OSD_TIMEOUT, IDS_OSD_TRANSPARENCY,IDS_XTVSYS_BIRTHNESS,
	};
	u8 opt_cnt[XTVSYS_SET_ITEM_CNT+XOSD_SET_ITEM_CNT + ITEM_ECTERN_CNT] = {3, 5, 4, 2, 4, 0, 0, 0};
	u16 y;

	u16 droplist_page[XTVSYS_SET_ITEM_CNT+XOSD_SET_ITEM_CNT + ITEM_ECTERN_CNT] = {3, 5, 4, 2, 4, 5, 5, 5};
#endif
#endif
	/* set focus according to current info */
	brightness = sys_status_get_brightness();
	sys_status_get_av_set(&av_set);
	if(sys_status_get_scart_out_cfg(av_set.video_output) == SCART_VID_RGB)
	{
		opt_cnt[0] = 3;
	}
#ifndef SPT_SUPPORT

	p_cont =fw_create_mainwin(ROOT_ID_XTVSYS_SET,
							   XTVSYS_SET_X, XTVSYS_SET_Y,
							   XTVSYS_SET_W, XTVSYS_SET_H,
	                         ROOT_ID_XSYS_SET, 0, OBJ_ATTR_ACTIVE, 0);
	ctrl_set_rstyle(p_cont,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG);
	if(p_cont == NULL)
	{
		return ERR_FAILURE;
	}
	ctrl_set_keymap(p_cont, xtvsys_set_cont_keymap);
	ctrl_set_proc(p_cont, xtvsys_set_cont_proc);

	y = XTVSYS_SET_ITEM_Y;
	for(i = 0; i < XOSD_SET_ITEM_CNT + XTVSYS_SET_ITEM_CNT; i++)
	{
		p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_XVIDEO_MODE + i),
		                      XTVSYS_SET_ITEM_X, y,
		                      XTVSYS_SET_ITEM_LW,
		                      XTVSYS_SET_ITEM_RW);
		ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
	#if ENABLE_TTX_SUBTITLE    

		switch (i)
		{
			case 5:
				ui_comm_select_set_param(p_ctrl[i], TRUE,
				                   CBOX_WORKMODE_STATIC, opt_cnt[i],
				                   CBOX_ITEM_STRTYPE_STRID, NULL);
				for (j = 0; j < opt_cnt[i]; j++)
				{
					ui_comm_select_set_content(p_ctrl[i], j, opt_data[i][j]);
				}
				break;
			case 6:
				ui_comm_ctrl_set_proc(p_ctrl[i], xosd_set_select_proc);
				ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_NUMBER,
				                   opt_cnt[i], CBOX_ITEM_STRTYPE_DEC, NULL);
				ui_comm_select_set_num_range(p_ctrl[i], 3, 10, 1, 0, NULL);
				break;
			case 7:
				ui_comm_ctrl_set_proc(p_ctrl[i], xosd_set_select_proc);
				ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_NUMBER,
				                   opt_cnt[i], CBOX_ITEM_STRTYPE_DEC, NULL);
				ui_comm_select_set_num_range(p_ctrl[i], 0, 40, 10, 0, NULL);
				break;
			case 8:
				ui_comm_ctrl_set_proc(p_ctrl[i], xbright_set_select_proc);
				ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_NUMBER,
				                   opt_cnt[i], CBOX_ITEM_STRTYPE_DEC, NULL);
				ui_comm_select_set_num_range(p_ctrl[i], 0, 100, 5, 0, NULL);
				break;					
			default:
				ui_comm_ctrl_set_proc(p_ctrl[i], xtvsys_set_select_proc);
				/*ui_comm_select_set_param(p_ctrl[i], TRUE,
				             CBOX_WORKMODE_DYNAMIC, opt_cnt[i],
				             0, xtvsys_set_comm_fill_content);*/
				ui_comm_select_set_param(p_ctrl[i], TRUE,
				CBOX_WORKMODE_STATIC, opt_cnt[i],
				CBOX_ITEM_STRTYPE_STRID, NULL);
				xtvsys_set_comm_fill_content(p_ctrl[i],opt_cnt[i]);
				break;
		}

	#else
		switch (i)
		{
			case 5:
				ui_comm_ctrl_set_proc(p_ctrl[i], xosd_set_select_proc);
				ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_NUMBER,
				                   opt_cnt[i], CBOX_ITEM_STRTYPE_DEC, NULL);
				ui_comm_select_set_num_range(p_ctrl[i], 3, 10, 1, 0, NULL);
				break;
			case 6:
				ui_comm_ctrl_set_proc(p_ctrl[i], xosd_set_select_proc);
				ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_NUMBER,
				                   opt_cnt[i], CBOX_ITEM_STRTYPE_DEC, NULL);
				ui_comm_select_set_num_range(p_ctrl[i], 0, 40, 10, 0, NULL);
				break;
			case 7:
				ui_comm_ctrl_set_proc(p_ctrl[i], xbright_set_select_proc);
				ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_NUMBER,
				                   opt_cnt[i], CBOX_ITEM_STRTYPE_DEC, NULL);
				ui_comm_select_set_num_range(p_ctrl[i], 0, 100, 5, 0, NULL);
				break;		
			default:
				ui_comm_ctrl_set_proc(p_ctrl[i], xtvsys_set_select_proc);
				  	/*ui_comm_select_set_param(p_ctrl[i], TRUE,
				             CBOX_WORKMODE_DYNAMIC, opt_cnt[i],
				             0, xtvsys_set_comm_fill_content);*/
				ui_comm_select_set_param(p_ctrl[i], TRUE,CBOX_WORKMODE_STATIC, opt_cnt[i],
				CBOX_ITEM_STRTYPE_STRID, NULL);
				xtvsys_set_comm_fill_content(p_ctrl[i],opt_cnt[i]);
				break;
		}
	#endif

		ui_comm_select_create_droplist(p_ctrl[i], droplist_page[i]);
		ctrl_set_related_id(p_ctrl[i],
		    0,                                                            /* left */
		    (u8)((i - 1 + 
		          XTVSYS_SET_ITEM_CNT +XOSD_SET_ITEM_CNT) %
		         (XTVSYS_SET_ITEM_CNT + XOSD_SET_ITEM_CNT) + 1),                  /* up */
		    0,                                                            /* right */
		    (u8)((i + 1) %( XTVSYS_SET_ITEM_CNT + XOSD_SET_ITEM_CNT) + 1)); /* down */
		y += XTVSYS_SET_ITEM_H + XTVSYS_SET_ITEM_V_GAP;
	}
#else
#endif
	/* set focus according to current info */
	sys_status_get_osd_set(&osd_set);
	//ui_comm_ctrl_update_attr(p_ctrl[1], FALSE);//demo on 256pin,disable for temp.need to open on 144pin

	ui_comm_select_set_focus(p_ctrl[0], av_set.tv_mode);
	ui_comm_select_set_focus(p_ctrl[1], av_set.tv_resolution);
	ui_comm_select_set_focus(p_ctrl[2], av_set.tv_ratio);
	ui_comm_select_set_focus(p_ctrl[3], av_set.digital_audio_output);
	//  ui_comm_select_set_focus(p_ctrl[4],av_set.transcoding);    // to do
	ui_comm_select_set_focus(p_ctrl[4],av_set.video_effects);  

#if ENABLE_TTX_SUBTITLE
	ui_comm_select_set_focus(p_ctrl[5], osd_set.enable_subtitle);
	// ui_comm_select_set_focus(p_ctrl[1], osd_set.enable_vbinserter);
	ui_comm_select_set_num(p_ctrl[6], osd_set.timeout);
	ui_comm_select_set_num(p_ctrl[7], osd_set.transparent);
#ifdef TV_BIRTHNESS_EN
	ui_comm_select_set_num(p_ctrl[8], brightness);
#endif	
#else
	ui_comm_select_set_num(p_ctrl[5], osd_set.timeout);
	ui_comm_select_set_num(p_ctrl[6], osd_set.transparent);
#ifdef TV_BIRTHNESS_EN
	ui_comm_select_set_num(p_ctrl[7], brightness);
#endif
#endif

	#if 0
	struct audio_device *p_dev = NULL;
	aud_dolby_status_vsb_t dolby_status = AUDIO_DOLBY_NOT_SUPPORTED_BOTH;
	p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
	aud_get_dolby_status(p_dev, &dolby_status);
	OS_PRINTF("dolby_status %d\n", dolby_status); 
	if(dolby_status == 3)//temp modify
	{
		ui_comm_ctrl_update_attr(p_ctrl[3], FALSE);
	}
	#endif

	#ifdef SPT_DUMP_DATA
	spt_dump_menu_data(p_cont);
	#endif

	ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
	ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

	return SUCCESS;
}


static RET_CODE on_xtvsys_set_select_change(control_t *p_ctrl,u16 msg,u32 para1,u32 para2)
{
	u8 to, from;
	av_set_t av_set;
	class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);
	RET_CODE ret = SUCCESS;
	control_t *p_video_resolution = NULL;
	void *p_disp = NULL;
	// BOOL is_onfocus = ctrl_is_onfocus(p_ctrl);
	//  control_t *p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));

	sys_status_get_av_set(&av_set);

	from = (u8)(para1), to = (u8)(para2);

	switch(ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
	{
		case IDC_XVIDEO_MODE:
			if (!ctrl_is_onfocus(p_ctrl) 
			|| av_set.tv_mode == to)
			{
				return SUCCESS;
			}
			av_set.tv_mode = to;
			#if ENABLE_TTX_SUBTITLE
			if (is_enable_vbi_on_setting())
			{
				ui_enable_vbi_insert(FALSE);
			}
			#endif
			avc_switch_video_mode_1(avc_handle, sys_status_get_sd_mode(av_set.tv_mode));

			avc_video_switch_chann(avc_handle, DISP_CHANNEL_SD);

			avc_switch_video_mode_1(avc_handle, sys_status_get_hd_mode(av_set.tv_resolution));
			#if ENABLE_TTX_SUBTITLE
			if (is_enable_vbi_on_setting())
			{
				ui_enable_vbi_insert(TRUE);
			}
			#endif
			if((av_set.tv_resolution == 0) //PAL->576i NTSC->480i
						||(av_set.tv_resolution == 1)) //PAL->576p NTSC->480p
			{
				p_video_resolution = ui_comm_root_get_ctrl(ROOT_ID_XTVSYS_SET, IDC_XVIDEO_RESOLUTION);
				ui_comm_select_set_param(p_video_resolution, TRUE,
												CBOX_WORKMODE_STATIC, 5,
												CBOX_ITEM_STRTYPE_STRID, NULL);
				xtvsys_set_comm_fill_content(p_video_resolution,5);
				ui_comm_select_set_focus(p_video_resolution, av_set.tv_resolution);
				ctrl_paint_ctrl(p_video_resolution, TRUE);  
			}
			break;

		case IDC_XVIDEO_RESOLUTION:
			if (!ctrl_is_onfocus(p_ctrl) || av_set.tv_resolution == to)
			{
				return SUCCESS;
			}      

			av_set.tv_resolution = to;
			avc_switch_video_mode_1(avc_handle, sys_status_get_hd_mode(av_set.tv_resolution));
			break;


		case IDC_XASPECT_MODE:
			if (!ctrl_is_onfocus(p_ctrl) || av_set.tv_ratio == to)
			{
				return SUCCESS;
			}
			av_set.tv_ratio = to;

			ui_reset_video_aspect_mode(sys_status_get_video_aspect(av_set.tv_ratio));
			avc_set_video_aspect_mode_1(avc_handle, sys_status_get_video_aspect(av_set.tv_ratio));
			avc_cfg_scart_aspect_1(avc_handle,sys_status_get_scart_aspect(av_set.tv_ratio));
			break;

		case IDC_XDIGITAL_OUTPUT:

			if (!ctrl_is_onfocus(p_ctrl)
			|| av_set.digital_audio_output == to)
			{
				return SUCCESS;
			}
			av_set.digital_audio_output = to;
			//transcoding_set(p_cont,is_onfocus);  
			break;
		/*
		case IDC_XTRANSCODING:
		if(!ctrl_is_onfocus(p_ctrl) || av_set.transcoding == to)
		return SUCCESS;
		av_set.transcoding = to;
		break;
		*/
		case IDC_XVIDEO_EFFECTS:
			if (!ctrl_is_onfocus(p_ctrl)|| av_set.video_effects == to)
			{
				return SUCCESS;
			}
			av_set.video_effects = to;

			p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
			MT_ASSERT(p_disp != NULL);
			disp_set_postprocess_mode(p_disp, (disp_pp_mode_t)to);

			break;

		default:
			MT_ASSERT(0);
			return ERR_FAILURE;
	}

	sys_status_set_av_set(&av_set);
	sys_status_save();

	return ret;
}

static RET_CODE on_xosd_set_select_change(control_t *ctrl, u16 msg, u32 para1,u32 para2)
{
	u8 focus = (u8)(para2);
	osd_set_t osd_set;
	u8 ctrl_id = ctrl_get_ctrl_id(ctrl_get_parent(ctrl));

	// check for initialize
	if(!ctrl_is_onfocus(ctrl))
	{
		return SUCCESS;
	}

	sys_status_get_osd_set(&osd_set);
	switch (ctrl_id)
	{
#if ENABLE_TTX_SUBTITLE
		case IDC_XSUBTITLE:
			osd_set.enable_subtitle = focus;
			break;
		#if 0
		case IDC_XINSERTER:
		osd_set.enable_vbinserter = focus;
		ui_enable_vbi_insert((BOOL)focus);
		break;
		#endif
#endif
		case IDC_XTIMEOUT:
			osd_set.timeout = focus;
			break;
		case IDC_XTRANSPARENCY:
			osd_set.transparent = focus;
			gdi_set_global_alpha((100 - focus) * 255 / 100);        
			break;
		default:
			MT_ASSERT(0);
			return ERR_FAILURE;
	}

	sys_status_set_osd_set(&osd_set);
	sys_status_save();

	if(ctrl_id == IDC_XTRANSPARENCY)
	{
		ctrl_paint_ctrl(ctrl_get_root(ctrl), TRUE);
	}

	return SUCCESS;
}

static RET_CODE on_xbright_set_select_change(control_t *ctrl, u16 msg, u32 para1,u32 para2)
{
	u8 brightness = (u8)(para2);
	class_handle_t avc_handle = class_get_handle_by_id(AVC_CLASS_ID);

	DEBUG(MAIN,INFO,"\n");
	if(!ctrl_is_onfocus(ctrl))
	{
		return SUCCESS;
	}

	DEBUG(MAIN,INFO,"brightness = %d \n",brightness);
	avc_set_hd_video_bright(avc_handle, brightness);
	sys_status_set_brightness(brightness);
	sys_status_save();
	return SUCCESS;
}


BEGIN_MSGPROC(xtvsys_set_cont_proc, ui_comm_root_proc)
END_MSGPROC(xtvsys_set_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(xtvsys_set_cont_keymap, ui_comm_root_keymap)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(xtvsys_set_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(xtvsys_set_select_proc, cbox_class_proc)
ON_COMMAND(MSG_CHANGED, on_xtvsys_set_select_change)
END_MSGPROC(xtvsys_set_select_proc, cbox_class_proc)

BEGIN_MSGPROC(xosd_set_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_xosd_set_select_change)
END_MSGPROC(xosd_set_select_proc, cbox_class_proc)

BEGIN_MSGPROC(xbright_set_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_xbright_set_select_change)
END_MSGPROC(xbright_set_select_proc, cbox_class_proc) 


