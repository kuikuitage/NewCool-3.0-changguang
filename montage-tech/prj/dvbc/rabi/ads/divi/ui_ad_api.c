/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifdef ENABLE_ADS
#include "mtos_mutex.h"
#include "ui_common.h"
#include "ui_config.h"
#include "pic_api.h"
#include "ui_usb_music.h"
#include "ui_pic_play_mode_set.h"
#include "ads_ware.h"
#include "ads_api_divi.h" 
#include "ui_ad_api.h"

#include "ui_ad_gif_api.h"
#include "ui_small_list_v2.h"


static handle_t pic_adv_handle = 0;
static void* pic_adv_region = NULL;

static u8* g_adv_pic_addr = NULL;
static u32 g_pic_size = 0;
static rect_t g_rect = {0};
static u32 g_adm_id;

static u8 *g_ad_data_buf = NULL;
u8 PIC_DATA_BUF[64*1024] = {0,};
static u8 show_time = 0;
static u8 show_pos = 0;
static u8 file_type = 0;

extern u16 ui_pic_evtmap(u32 event);
extern void pic_api_callback(handle_t pic_handle, u32 content, u32 para1, u32 para2);

void ui_adv_set_adm_id(u32 adm_id)
{
    g_adm_id = adm_id;
}

u32 ui_adv_get_adm_id(void)
{
    return g_adm_id;
}

/*ADV Region  create */
static RET_CODE ui_adv_pic_region_init(const rect_t *rect)
{
	rect_size_t rect_size = {0};
	point_t pos;  
	RET_CODE ret = ERR_FAILURE;
	void *p_region_buffer = NULL;
	u32 region_block_size = 0;
	u32 align = 0;
	u32 region_size = 0;
	void *p_disp_dev;

	p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
	//set coordinate
	pos.x = rect->left;
	pos.y = rect->top;
	rect_size.w = ((u16)((rect->right - rect->left) + 7) >> 3) << 3; //must be multiple of 8
	rect_size.h =  (u16)(rect->bottom - rect->top);
	if(pos.x+rect_size.w > SCREEN_WIDTH)
	{
		pos.x = SCREEN_WIDTH-rect_size.w;
	}
	if(pos.y+rect_size.h > SCREEN_HEIGHT)
	{
		pos.y = SCREEN_HEIGHT-rect_size.h;
	}
  
	OS_PRINTF("@@@region x=%d, y = %d, w = %d, h = %d \n",pos.x,pos.y,rect_size.w,rect_size.h);

	region_block_size = SUB_BUFFER_SIZE;
	MT_ASSERT(region_block_size != 0);

	pic_adv_region = region_create(&rect_size, PIX_FMT_ARGB8888);
	MT_ASSERT(NULL != pic_adv_region);

	ret = disp_calc_region_size(p_disp_dev, DISP_LAYER_ID_SUBTITL, pic_adv_region, &align, &region_size);
	OS_PRINTF("region_size=0x%x,region_block_size=0x%x\n",region_size,region_block_size);
	if (ret != SUCCESS)
	{
		OS_PRINTF("disp_calc_region_size Failure!\n");
		pic_adv_region = NULL;
		return ERR_FAILURE;
	}
	MT_ASSERT(region_block_size >= region_size);

	ret = disp_layer_add_region(p_disp_dev, DISP_LAYER_ID_SUBTITL, 
	                                          pic_adv_region, &pos, p_region_buffer);
	if(ret != SUCCESS)
	{
		pic_adv_region = NULL;
		OS_PRINTF("ui_adv_pic_region_init  add region fail, LINE : %d!!!\n",__LINE__);
		return ERR_FAILURE;
	}
  
	region_show(pic_adv_region, TRUE);
	disp_layer_show(p_disp_dev, DISP_LAYER_ID_SUBTITL, TRUE);
	return SUCCESS;
}

//adv region release
static void ui_adv_pic_region_release(void)
{
	RET_CODE ret = ERR_FAILURE;
	void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);

	if(NULL != pic_adv_region)
	{
		ret = disp_layer_remove_region(p_disp_dev, DISP_LAYER_ID_SUBTITL, pic_adv_region);
		MT_ASSERT(SUCCESS == ret);

		ret = region_delete(pic_adv_region);
		MT_ASSERT(SUCCESS == ret);
		pic_adv_region = NULL;
	}
}

void pic_adv_start(mul_pic_param_t *p_pic_param)
{
	MT_ASSERT(p_pic_param != NULL);

	if(pic_adv_handle != 0)
	{
		mul_pic_start(pic_adv_handle, p_pic_param);
	}
}

void pic_adv_stop(void)
{
	#ifdef MULTI_PIC_ADV
	ui_adv_stop_mul_pic();
	#endif

	if(pic_adv_handle != 0)
	{
		mul_pic_stop(pic_adv_handle);

		#ifndef WIN32
		//release region of current adv
		ui_adv_pic_region_release();
		#endif
	}
}

void ui_adv_pic_init(pic_source_t src)
{
	mul_pic_chan_t chan = {0};
	RET_CODE ret = ERR_FAILURE;

	if (g_ad_data_buf == NULL)
	{
		g_ad_data_buf = (u8 *)mtos_malloc(64 * KBYTES);;
		if (g_ad_data_buf == NULL){
			DEBUG(ADS,ERR,"\n");
			return;
		}
	}

	chan.task_stk_size = 16 * KBYTES;
	chan.anim_pri = JPEG_CHAIN_ANIM_TASK_PRIORITY;
	chan.anim_stk_size = 4 * KBYTES;
  
	if (pic_adv_handle != NULL)
	{
		DEBUG(ADS,INFO,"\n");
		return;
	}

	if( src == PIC_SOURCE_BUF )
	{
		ret = mul_pic_create_buffer_chain(&pic_adv_handle, &chan);
	}
  
	if(SUCCESS != ret)
	{
		pic_adv_handle = 0;
		DEBUG(ADS,ERR,"\n");
		return;
	}
	OS_PRINTF("ads pic chain created, 0x%x\n", pic_adv_handle);  
	mul_pic_set_dec_mode(pic_adv_handle, DEC_FRAME_MODE); 
	mul_pic_register_evt(pic_adv_handle, pic_api_callback);

	fw_register_ap_evtmap(APP_PICTURE, ui_pic_evtmap);
	fw_register_ap_msghost(APP_PICTURE, ROOT_ID_MAINMENU); 
	fw_register_ap_msghost(APP_PICTURE, ROOT_ID_PROG_BAR); 
	fw_register_ap_msghost(APP_PICTURE, ROOT_ID_PROG_LIST); 
	fw_register_ap_msghost(APP_PICTURE, ROOT_ID_BACKGROUND);
	fw_register_ap_msghost(APP_PICTURE, ROOT_ID_VOLUME);
	fw_register_ap_msghost(APP_PICTURE, ROOT_ID_SMALL_LIST); 
	fw_register_ap_msghost(APP_PICTURE, ROOT_ID_BACKGROUND);
}

void ui_adv_pic_release(void)
{
	OS_PRINTF("pic chain destoryed, 0x%x\n", pic_adv_handle);

	if(pic_adv_handle != 0) 
	{
		mul_pic_destroy_chain(pic_adv_handle);
		pic_adv_handle = 0;
	}
#ifndef WIN32
	ui_adv_pic_region_release();
#endif
	fw_unregister_ap_evtmap(APP_PICTURE);
	fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_MAINMENU); 
	fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_PROG_BAR); 
	fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_PROG_LIST); 
	fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_BACKGROUND);
	fw_unregister_ap_msghost(APP_PICTURE, ROOT_ID_VOLUME);
	fw_register_ap_msghost(APP_PICTURE, ROOT_ID_SMALL_LIST); 
	fw_register_ap_msghost(APP_PICTURE, ROOT_ID_BACKGROUND);  
}

static u8* get_adv_pic_buf_addr(u32* p_len)
{
	*p_len = g_pic_size;
	return g_adv_pic_addr;
}

static void ui_adv_set_pic_param(u8 *pic_add,u32 size)
{
	OS_PRINTF("#@@@ set pic addr = 0x%x, size = 0x%x \n",pic_add, size);
	g_adv_pic_addr = pic_add;
	g_pic_size =  size;
}

RET_CODE ui_adv_set_pic(u32 size, u8 ads_type, u8 *pic_add)
{
	mul_pic_param_t pic_param = {{0},0};
	RET_CODE ret = ERR_FAILURE;

	OS_PRINTF("ADV ui_adv_set_pic length[0x%x]\n",size);
	if(pic_add == NULL || size == 0)
	{
		OS_PRINTF("ADV p_data = NULL");
		return ERR_FAILURE;
	}

	#ifndef WIN32
	ret = ui_adv_pic_region_init(&g_rect);
	OS_PRINTF("[%s:%d]ret=%d\n", __FUNCTION__, __LINE__, ret);
	#else
	pic_adv_region = (void *)gdi_get_screen_handle(FALSE);
	#endif

	if(ret != SUCCESS)
	{
		OS_PRINTF("ui_adv_set_pic ui_adv_pic_region_init FAIL , LINE = %d\n", __LINE__);
		return ERR_FAILURE;
	}
	mul_pic_set_rend_rgn(pic_adv_handle, pic_adv_region); 

	ui_adv_set_pic_param(pic_add, size);

	OS_PRINTF("ADV ui_adv_set_pic g_rect.x[%d], t[%d], r[%d], b[%d]\n",g_rect.left,g_rect.top,g_rect.right,g_rect.bottom);

	pic_param.win_rect.left 	= 0;
	pic_param.win_rect.top 		= 0;
	pic_param.win_rect.right 	= (g_rect.right - g_rect.left);
	pic_param.win_rect.bottom 	= (g_rect.bottom - g_rect.top);

	pic_param.file_size 	= 0;
	pic_param.is_file 		= FALSE;  
	pic_param.buf_get 		= (u32)get_adv_pic_buf_addr;

	pic_param.style 	= REND_STYLE_STRECH;
	pic_param.flip 		= PIC_NO_F_NO_R;
	pic_param.anim 		= REND_ANIM_NONE;

	if(ads_type == 0x07)
	{
		pic_param.style = REND_STYLE_CENTER;
	}

	pic_adv_start(&pic_param);
	return SUCCESS;
}

BOOL ui_adv_set_pic_by_type(ads_ad_type_t type)
{
	RET_CODE ret = SUCCESS;
	#ifndef WIN32
	#ifdef ENABLE_ADS
	u32 filesize = 0;
	ads_ad_type_t ads_divi_type = 0;
	u8 is_hide = 0;
 
	switch(type)
	{
		case ADS_AD_TYPE_MENU_UP:
			g_rect.left = 478;
			g_rect.right = g_rect.left + 325;
			g_rect.top = 135;
			g_rect.bottom = g_rect.top + 245;
			ads_divi_type = 0x5;
			break;

		case ADS_AD_TYPE_CHLIST:
			g_rect.left = (SMALL_EXTERN_LEFT + (SMALL_LIST_MENU_WIDTH - 235)/2);
			g_rect.right = g_rect.left + 235;
			g_rect.top = SCREEN_HEIGHT - 120;
			g_rect.bottom = g_rect.top + 80;
			ads_divi_type = 0x6;
			break;

		case ADS_AD_TYPE_CHBAR:
			g_rect.left = 1020;
			g_rect.right = g_rect.left + 200;
			g_rect.top = 540;
			g_rect.bottom = g_rect.top + 150;
			ads_divi_type = 0x2;
			break;

		case ADS_AD_TYPE_MENU_DOWN:
			g_rect.left = 407;
			g_rect.right = g_rect.left+280;
			g_rect.top = 333;
			g_rect.bottom = g_rect.top+140;
			break;

		case ADS_AD_TYPE_MAIN_MENU:
			g_rect.left = 12;
			g_rect.right = 12 +265;
			g_rect.top = 15;
			g_rect.bottom = 15+220;
			break;

		case ADS_AD_TYPE_BANNER:
			g_rect.left = 117;
			g_rect.right = g_rect.left+80;
			g_rect.top = 250;
			g_rect.bottom = g_rect.top+80;
			ads_divi_type = 0x4;
			break;

		case ADS_AD_TYPE_OTHER_GIF:
			g_rect.left = 42;
			g_rect.right = g_rect.left+280;
			g_rect.top = 275;
			g_rect.bottom = g_rect.top+140;
			break;

		case ADS_AD_TYPE_VOLBAR:
			g_rect.left = 950;//984;
			g_rect.right = g_rect.left + 150;
			g_rect.top = 480;
			g_rect.bottom = g_rect.top + 150;
			ads_divi_type = 0x3;
			break;

		case ADS_AD_TYPE_OSD:
			g_rect.left = 0;
			g_rect.right = g_rect.left + 1200;
			g_rect.top = 100;
			g_rect.bottom = g_rect.top + 100;
			ads_divi_type = 0x7;
			return FALSE;

		default:
			g_rect.left = 0;
			g_rect.right = SCREEN_WIDTH - g_rect.left;
			g_rect.top = 0;
			g_rect.bottom = SCREEN_HEIGHT - g_rect.top;
			break;
	}
  
	OS_PRINTF("[%s:%d]ads_divi_type=%d\n", __FUNCTION__, __LINE__, ads_divi_type);
	memset(g_ad_data_buf, 0, 64*KBYTES);
	filesize = divi_ads_get_data(ads_divi_type , g_ad_data_buf, &is_hide, &show_time, &show_pos, &file_type);
	OS_PRINTF("start show picture, filesize=%d\n", filesize);
	if(filesize != 0)
	{
		if (!is_hide)
		{
			OS_PRINTF("\n pic show is hide \n");
			return FALSE;
		}
		ret = ui_adv_set_pic(filesize, ads_divi_type,g_ad_data_buf);
		DEBUG(ADS,INFO,"ret = %d\n",ret);
	}
	else
	{		
	return FALSE;
	}
	#endif
	#endif

	OS_PRINTF("ui_adv_set_pic_by_type ret = %d\n", ret); 
	return (ret==SUCCESS)?TRUE : FALSE;
}

BOOL ui_adv_get_pic_rect(rect_t * rect_region)
{
	BOOL ret = FALSE;

	if(pic_adv_region != NULL)
	{
		rect_region->top = g_rect.top;
		rect_region->bottom = g_rect.bottom;
		rect_region->left = g_rect.left;
		rect_region->right = g_rect.right;
		ret =  TRUE;
	}
	return ret;
}

BOOL ui_adv_pic_play(ads_ad_type_t type, u8 root_id)
{
	u8 ads_osd_state;
	
	DEBUG(ADS,INFO,"ADV ui_pic_play type[%d] root_id[0x%x]\n",type,root_id);
	ads_osd_state = ui_get_ads_update_osd_state();
	switch(type)
	{
		case ADS_AD_TYPE_VOLBAR:
			if(IDN_ADS_PIC_ROLLING == ads_osd_state)
			{
				DEBUG(ADS,INFO,"pic rolling\n");
				return FALSE;
			}
			break;
		case ADS_AD_TYPE_CHBAR:
			if(IDN_ADS_PIC_ROLLING == ads_osd_state)
			{
				divi_ads_update_osd_show(FALSE);
				return FALSE;
			}
			else if(IDN_ADS_TEXT_ROLLING == ads_osd_state)
			{
				divi_ads_update_osd_show(FALSE);
			}
			break;
		case ADS_AD_TYPE_BANNER:
			if(IDN_ADS_PIC_ROLLING == ads_osd_state)
			{
				divi_ads_update_osd_show(TRUE);
				return FALSE;
			}
			else if(IDN_ADS_TEXT_ROLLING == ads_osd_state)
			{
				divi_ads_update_osd_show(TRUE);
			}
			break;
		case ADS_AD_TYPE_CHLIST:
		case ADS_AD_TYPE_MENU_UP:
			break;
		default:
			break;
	}

	pic_adv_stop();
	if(ui_adv_set_pic_by_type(type))
	{
		return TRUE;
	}

	return FALSE;
}

void divi_show_logo(void)
{
	u32 filesize = 0;
	u8 is_show = 0;
	u8 show_time = 0;
	u8 show_pos = 0;
	u8 file_type = 0;
	u8 ads_divi_type = 1;

	memset(PIC_DATA_BUF, 0, 64*KBYTES);
	filesize = divi_ads_get_data(ads_divi_type , PIC_DATA_BUF, &is_show, &show_time, &show_pos, &file_type);

	OS_PRINTF("pic_addr=%d,filesize=%d AD_TYPE = %d\n", PIC_DATA_BUF, filesize, ads_divi_type);

	if(filesize == 0 || file_type != 1)
	{
		OS_PRINTF("ads_show_logo no ad logo or logo is not mpeg\n");
		return ;
	}
	else 
	{      
		OS_PRINTF("Get OpenLogo success\n");
		RET_CODE ret = SUCCESS;
		void * p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
		void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);

		vdec_stop(p_video_dev);
		disp_layer_show(p_disp_dev, DISP_LAYER_ID_SUBTITL, FALSE);
		vdec_set_data_input(p_video_dev,1);

		ret = vdec_start(p_video_dev, VIDEO_MPEG, VID_UNBLANK_STABLE);  
		MT_ASSERT(SUCCESS == ret);
		ret = vdec_dec_one_frame(p_video_dev, (u8 *)PIC_DATA_BUF, filesize);
		MT_ASSERT(SUCCESS == ret);
		ret = disp_layer_show(p_disp_dev, DISP_LAYER_ID_SUBTITL, TRUE);
		MT_ASSERT(SUCCESS == ret); 
		mtos_task_sleep(1000);
	}
}

#endif

