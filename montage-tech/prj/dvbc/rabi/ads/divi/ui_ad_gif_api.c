#include "ui_common.h"
#include "ads_api_divi.h"
#include "ui_ad_gif_api.h"
#include "ui_ads_display.h"

#define IDN_OSD_BUF_NUM    					(6)
#define IDN_OSD_BUF_HEIGH					(40)
#define IDN_OSD_BUF_WIDTH					(4)
#define IDN_OSD_BUF_SCREEN_WIDTH			(1280)

static void *p_disp = NULL;
static void *p_gpe = NULL;
static void *osd_region = NULL;
static u32 decode_data_size = 0;
static u32 src_height = 0;
static u32 src_width = 0;
static u8 *p_output_osd= NULL;
static u8  gif_osd_data[63*1024] ;

static u32 frameNub = 0;
static u32 delayTime= 0;
static u8 p_osd_buf[IDN_OSD_BUF_NUM][1280*40*4];
static u8 sec_num = 0;
static u32 divi_osd_offset = 0xffffffff;
static u32 divi_osd_offset_save = 0;

static u8 divi_osd_show_pos = 0xff;

static u32 s_divi_ads_pic_lock = 0;
static u8  s_divi_ads_pic_rolling = FALSE;

void divi_ads_pic_osd_lock(void)
{
    mtos_sem_take((os_sem_t *)&s_divi_ads_pic_lock, 0);
}

void divi_ads_pic_osd_unlock(void)
{
    mtos_sem_give((os_sem_t *)&s_divi_ads_pic_lock);
}

void divi_ads_pic_osd_set_rolling(u8 state)
{
	divi_ads_pic_osd_lock();
	s_divi_ads_pic_rolling = state;
	divi_ads_pic_osd_unlock();
}

void divi_ads_pic_osd_get_rolling(u8 *state)
{
	divi_ads_pic_osd_lock();
	*state = s_divi_ads_pic_rolling;
	divi_ads_pic_osd_unlock();
}

static void divi_draw_pic(rect_t rgn_rect, rect_t fill_rect, u8 *data, u32 pitch, u32 size)
{
	RET_CODE ret = SUCCESS;
	gpe_param_vsb_t param = {0, 0, 0, 0, 0, 0};

	param.enable_colorkey = TRUE;
	param.colorkey = 0x4d00feff; //colorkey alpha 0x4d

	gpe_draw_image_vsb(p_gpe, osd_region, &rgn_rect,
	                    data, NULL, 0, pitch,
	                    size, PIX_FMT_ARGB8888, 
	                    &param, &fill_rect);

	ret = disp_layer_update_region(p_disp, osd_region, NULL);
}


static void divi_decode_pic(u8 *osd_data, u32 osd_size)
{
	pic_info_t pic_info = {PIC_FRAME_MODE,};
	pic_param_t pic_param = {PIC_FRAME_MODE,};
	gif_output_t gif_dst = {0,};
	drv_dev_t *p_pic_dev = NULL;
	RET_CODE ret = SUCCESS;
	pdec_ins_t pic_ins_osd = {IMAGE_FORMAT_UNKNOWN,};
  
	p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);
	MT_ASSERT(p_pic_dev != NULL); 

	memset(&pic_ins_osd, 0, sizeof(pic_ins_osd));
	ret = pdec_getinfo(p_pic_dev, osd_data, osd_size, &pic_info, &pic_ins_osd);
	MT_ASSERT(ret == SUCCESS);

	OS_PRINTK("pic width:%d, height:%d, format:%d, bbp:%d frame_num:%d delaytime:%d\n", pic_info.src_width, pic_info.src_height, pic_info.image_format, pic_info.bbp,pic_info.frame_nums,pic_info.daleytime);
	if(pic_info.image_format != IMAGE_FORMAT_BMP)
	{
		decode_data_size = (pic_info.src_width) * (pic_info.src_height) * 4; 
		src_height = pic_info.src_height;
		src_width = pic_info.src_width;
		frameNub = pic_info.frame_nums;
		delayTime = pic_info.daleytime;

		if(p_output_osd != NULL)
		{
			mtos_align_free(p_output_osd);
		}
		p_output_osd= (u8 *)mtos_align_malloc(decode_data_size, 8);
		//p_output_osd = (u8 *)mtos_malloc(decode_data_size);
		MT_ASSERT(p_output_osd!= NULL);
		memset(p_output_osd  ,0, decode_data_size);

		pic_param.dec_mode = DEC_FRAME_MODE;
		pic_param.output_format = PIX_FMT_ARGB8888;
		pic_param.scale_w_num = 1;
		pic_param.scale_w_demo = 1;
		pic_param.scale_h_num = 1;
		pic_param.scale_h_demo = 1;
		pic_param.disp_width = pic_info.src_width;
		pic_param.disp_height = pic_info.src_height;      
		pic_param.p_src_buf = osd_data;
		pic_param.p_dst_buf = &gif_dst;
		pic_param.flip = PIC_NO_F_NO_R;
		pic_param.src_size = osd_size;

		gif_dst.pic_out_mode = PIC_FRAME_MODE;
		gif_dst.dec_mode= DEC_FRAME_MODE;
		gif_dst.frmnum = 1;
		gif_dst.frmsize = decode_data_size;
		gif_dst.biHeight = pic_info.src_height; 
		gif_dst.biWidth = pic_info.src_width;
		gif_dst.oncedecnum = 1;
		gif_dst.frmdata[0] = p_output_osd;

		OS_PRINTF("pdec_setinfo\n");
		ret = pdec_setinfo(p_pic_dev, &pic_param, &pic_ins_osd);
		MT_ASSERT(ret == SUCCESS);

		OS_PRINTF("pdec_start\n");
		ret = pdec_start(p_pic_dev, &pic_ins_osd);
		MT_ASSERT(ret == SUCCESS);

		OS_PRINTF("pdec_stop\n");
		ret = pdec_stop(p_pic_dev, &pic_ins_osd);
		MT_ASSERT(ret == SUCCESS);	

	}
}

extern void send_event_to_ui_from_ads(u32 event, u32 param);
static void gif_move_Time(u32 param)
{
	rect_t rgn_rect = {0};
	rect_t fill_rect = {0};
	rect_t rgn_rect_2 = {0};
	rect_t fill_rect_2 = {0};

	divi_osd_offset += 2;

	if ((sec_num + 1)*1280 <= divi_osd_offset)
	{
		divi_ads_pic_osd_set_rolling(FALSE);
		send_event_to_ui_from_ads(CAS_C_ADS_MESSAGE, IDN_ADS_PIC_ROLLING_OVER);
		return;
	}

	if (divi_osd_offset <= 1280)
	{
		rgn_rect.top = 0;
		rgn_rect.bottom = rgn_rect.top + src_height;
		rgn_rect.left = 1280 - divi_osd_offset;
		rgn_rect.right = 1280;

		fill_rect.top = 0;
		fill_rect.bottom = fill_rect.top + src_height;
		fill_rect.left = 0;
		fill_rect.right = divi_osd_offset;

		divi_draw_pic(rgn_rect, fill_rect, p_osd_buf[0], 1280*4, 1280*src_height*4);
	}
	else
	{
		if ((divi_osd_offset % 1280) != 0)
		{
			rgn_rect.top = 0;
			rgn_rect.bottom = rgn_rect.top + src_height;
			rgn_rect.left = 0;
			rgn_rect.right = 1280 - (divi_osd_offset%1280);

			fill_rect.top = 0;
			fill_rect.bottom = fill_rect.top + src_height;
			fill_rect.left = divi_osd_offset%1280;
			fill_rect.right = 1280;

			divi_draw_pic(rgn_rect, fill_rect, p_osd_buf[divi_osd_offset/1280 -1], 1280*4, 1280*src_height*4);
		}
    
		rgn_rect_2.top = 0;
		rgn_rect_2.bottom = rgn_rect_2.top + src_height;
		rgn_rect_2.left = rgn_rect.right;
		rgn_rect_2.right = 1280;

		fill_rect_2.top = 0;
		fill_rect_2.bottom = src_height;
		fill_rect_2.left = 0;
		if ((divi_osd_offset % 1280) == 0)
			fill_rect_2.right = 1280;
		else
			fill_rect_2.right = divi_osd_offset%1280;

		divi_draw_pic(rgn_rect_2, fill_rect_2, p_osd_buf[(divi_osd_offset/1280)], 1280*4, 1280*src_height*4);
	}
}

static void mem_copy(void)
{ 
	u8 i = 0;
	u8 j = 0;
	u32 line_len = 1280*4;
	u32 line_len_2 = 0;

	if ((src_width%1280) == 0)
		sec_num = src_width/1280;
	else
		sec_num = src_width/1280 + 1;

	line_len_2 = (1280 - (sec_num*1280-src_width))*4;

	DEBUG(ADS,INFO,"sec_num=%d src_height=%d \n",sec_num,src_height);
	memset(p_osd_buf, 0, IDN_OSD_BUF_NUM*1280*40*4);
	if(sec_num > IDN_OSD_BUF_NUM)
		sec_num = IDN_OSD_BUF_NUM;

	if(src_height>IDN_OSD_BUF_HEIGH)
		src_height = IDN_OSD_BUF_HEIGH;

	for (i = 0; i < src_height; i++)
	{
		for (j = 0; j < sec_num; j++)
		{
			if ((j == (sec_num-1)) && ((src_width%1280) != 0))
				memcpy(p_osd_buf[j]+line_len*i, p_output_osd+src_width*4*i+line_len*j, line_len_2);
			else
				memcpy(p_osd_buf[j]+line_len*i, p_output_osd+src_width*4*i+line_len*j, line_len);           
		}
	}
}

static void divi_ads_pic_osd_creat(u8 show_pos)
{
	RET_CODE ret = SUCCESS;
	point_t pos = {0};
	rect_size_t rect_size = {0};

	switch(show_pos)
	{
		case 2:
			pos.x = 350;
			pos.y = 40;
			break;
		case 1:
			pos.x = 0;
			pos.y = 300;
			break;
		case 0:
			pos.x = 0;
			if(ROOT_ID_PROG_BAR== fw_get_focus_id())
				pos.y = 470;
			else
				pos.y = 640;
			break;
		default:
			break;
	}

	rect_size.w = (OSD_ADS_WIDTH - pos.x);
	rect_size.h = OSD_ADS_HEIGHT;

	osd_region = region_create(&rect_size, PIX_FMT_ARGB8888);
	MT_ASSERT(NULL != osd_region);

	ret = disp_layer_add_region(p_disp, DISP_LAYER_ID_SUBTITL, osd_region, &pos, NULL);
	if(SUCCESS != ret)
	{
		DEBUG(ADS,INFO,"\n");
		return;
	}

	region_show(osd_region, TRUE);
	disp_layer_show(p_disp, DISP_LAYER_ID_SUBTITL, TRUE);

}

static void divi_ads_pic_osd_close(void)
{
	RET_CODE ret = SUCCESS;

	if(p_disp != 0)
	{
		if(NULL != osd_region)
		{
			ret = disp_layer_remove_region(p_disp, DISP_LAYER_ID_SUBTITL, osd_region);
			if(SUCCESS != ret)
			{
				DEBUG(ADS,INFO,"\n");
				return;
			}


			ret = region_delete(osd_region);
			if(SUCCESS != ret)
			{
				DEBUG(ADS,INFO,"\n");
				return;
			}

			osd_region = NULL;
		}
	}
}

#if 0
static void divi_gif_osd_stop(void)
{
	gpe_draw_rectangle_vsb(p_gpe, osd_region, NULL, 0x00000000);
	disp_layer_update_region(p_disp, osd_region, NULL);
	return;
}
#endif


BOOL divi_ads_osd_over(void)
{
	if(NULL == osd_region)
		return FALSE;

	divi_ads_pic_osd_close();
	divi_osd_show_pos = 0xff;
	return TRUE;
}

RET_CODE divi_gif_osd_start(void)
{
	u8 *p_data = gif_osd_data;
	u32  filesize =0;
	u8 is_show=0;
	u8 show_time=0;
	u8 show_pos=0;
	u8 file_type=0;

	OS_PRINTF("[%s:%d]OSD ADS start!\n", __FUNCTION__, __LINE__);
	memset(p_data, 0, 63 * KBYTES);
	filesize = divi_ads_get_data(IDN_ADS_PIC_ROLLING , p_data, &is_show, &show_time, &show_pos, &file_type);
	if(filesize==0 || is_show==0)
	{
		return ERR_FAILURE;
	}

	OS_PRINTF("[%s:is_show:%d  show_time:%d show_pos:%d filetype:%d \n", __FUNCTION__,is_show,show_time,show_pos,file_type);

	divi_osd_show_pos = show_pos%3;
	divi_ads_pic_osd_creat(show_pos%3);

	divi_decode_pic(p_data,filesize);
	mem_copy();
	divi_osd_offset = 0;
	divi_ads_pic_osd_set_rolling(TRUE);

	return SUCCESS;
}

static void divi_ads_rolling_monitor(void *p_data)
{
	u8 divi_osd_rolling = FALSE;

	while(1)
	{
		divi_ads_pic_osd_get_rolling(&divi_osd_rolling);

		if(divi_osd_rolling)
		{
			gif_move_Time(0);
			mtos_task_sleep(40);
		}
		else
		{
			mtos_task_sleep(600);
		}
	}
}

void divi_ads_pic_osd_init(void)
{
	static u8 osdinit =0;
	u32 *p_stack = NULL;

	if(osdinit == 1)
	{
		return ;
	}

	osdinit =1;  
	p_disp = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
	MT_ASSERT(NULL != p_disp);

	p_gpe = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
	MT_ASSERT(NULL != p_gpe);

	if(p_stack == NULL)
	{
		p_stack = (u32 *)mtos_malloc(16 * 1024);
		MT_ASSERT(p_stack != NULL);
	}

    mtos_task_create((u8 *)"GIF osd Display",
            divi_ads_rolling_monitor,
            (void *)0,
            AD_PIC_ROLL_TASK,
            p_stack,
            16 * 1024);	

	if(FALSE == mtos_sem_create((os_sem_t *)&s_divi_ads_pic_lock,1))
	{
		OS_PRINTF("ads_adt_divi_attach, create sem error ! \n");
	}

}


void divi_ads_update_pic_osd_show(BOOL direction)
{
	RET_CODE ret = SUCCESS;
	point_t pos = {0};

	if((FALSE == ui_get_ads_osd_roll_over()) && (0 == divi_osd_show_pos))
	{
		if(direction)
		{
			if(p_disp != 0 && NULL != osd_region)
			{
				pos.x = 0;
				pos.y = 640;
				DEBUG(ADS,INFO,"\n");
			}
			else
			{
				DEBUG(ADS,INFO,"\n");
				return;
			}
		}
		else
		{
			if(p_disp != 0 && NULL != osd_region)
			{
				pos.x = 0;
				pos.y = 470;
				DEBUG(ADS,INFO,"\n");
			}
			else
			{
				DEBUG(ADS,INFO,"\n");
				return;
			}
		}
		ret = disp_layer_move_region(p_disp, osd_region, &pos);
		if(SUCCESS != ret)
		{
			DEBUG(ADS,INFO,"\n");
			return;
		}
		region_show(osd_region, TRUE);
		DEBUG(ADS,INFO,"\n");
	}
	DEBUG(ADS,INFO,"\n");
}

void divi_ads_pic_rolling_hide(void)
{
	rect_t rgn_rect = {0};
	rect_t fill_rect = {0};
	rect_t rgn_rect_2 = {0};
	rect_t fill_rect_2 = {0};

	divi_ads_pic_osd_set_rolling(FALSE);

	divi_osd_offset_save = divi_osd_offset;
	divi_osd_offset = (sec_num + 1)*1280;

	if (divi_osd_offset <= 1280)
	{
		rgn_rect.top = 0;
		rgn_rect.bottom = rgn_rect.top + src_height;
		rgn_rect.left = 1280 - divi_osd_offset;
		rgn_rect.right = 1280;

		fill_rect.top = 0;
		fill_rect.bottom = fill_rect.top + src_height;
		fill_rect.left = 0;
		fill_rect.right = divi_osd_offset;

		divi_draw_pic(rgn_rect, fill_rect, p_osd_buf[0], 1280*4, 1280*src_height*4);
	}
	else
	{
		if ((divi_osd_offset % 1280) != 0)
		{
			rgn_rect.top = 0;
			rgn_rect.bottom = rgn_rect.top + src_height;
			rgn_rect.left = 0;
			rgn_rect.right = 1280 - (divi_osd_offset%1280);

			fill_rect.top = 0;
			fill_rect.bottom = fill_rect.top + src_height;
			fill_rect.left = divi_osd_offset%1280;
			fill_rect.right = 1280;

			divi_draw_pic(rgn_rect, fill_rect, p_osd_buf[divi_osd_offset/1280 -1], 1280*4, 1280*src_height*4);
		}

		rgn_rect_2.top = 0;
		rgn_rect_2.bottom = rgn_rect_2.top + src_height;
		rgn_rect_2.left = rgn_rect.right;
		rgn_rect_2.right = 1280;

		fill_rect_2.top = 0;
		fill_rect_2.bottom = src_height;
		fill_rect_2.left = 0;
		if ((divi_osd_offset % 1280) == 0)
			fill_rect_2.right = 1280;
		else
			fill_rect_2.right = divi_osd_offset%1280;

		divi_draw_pic(rgn_rect_2, fill_rect_2, p_osd_buf[(divi_osd_offset/1280)], 1280*4, 1280*src_height*4);
	}
	divi_ads_pic_osd_close();

}

void divi_ads_pic_rolling_show(void)
{
	divi_ads_pic_osd_creat(divi_osd_show_pos);
	divi_osd_offset = divi_osd_offset_save;
	divi_osd_offset_save = 0;
	gif_move_Time(0);

	divi_ads_pic_osd_set_rolling(TRUE);
}




