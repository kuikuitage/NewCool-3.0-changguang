/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ethernet.h"


u8 com_num;

#ifdef SHOW_LOGO_BG
static BOOL is_show_logo = FALSE;
#endif

static void *p_net_svc = NULL;

u8 g_led_buffer[10] = {0};
u8 g_led_index = 0;

static BOOL g_uio_enable = FALSE;
void ui_enable_uio(BOOL is_enable)
{
  cmd_t cmd;
  
  if(g_uio_enable && is_enable)
  {
    return;
  }
  else if(g_uio_enable == FALSE && is_enable == FALSE)
  {
    return;
  }
  
  if(is_enable == TRUE)
  {
    g_uio_enable = TRUE;
  }
  else
  {
    g_uio_enable = FALSE;
  }

  cmd.id = (u8)is_enable ?
           AP_FRM_CMD_ACTIVATE_APP : AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_UIO;
  cmd.data2 = 0;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);
}
void ui_set_com_num(u8 num)
{
  com_num = num;
}
#if 1
static void _set_fp_display(char *content)
{
  void *p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);

  switch(CUSTOMER_ID)
  {
  	  case CUSTOMER_XINNEW_DEMO:
	  case CUSTOMER_MAIKE_HUNAN_LINLI:
	  case CUSTOMER_DTMB_SZXC_GUOMAO:
	  case CUSTOMER_XINNEW_GAIZHOU:
	  case CUSTOMER_MAIKE_HUNAN:
	  case CUSTOMER_DTMB_CHANGSHA_HHT:
	  case CUSTOMER_DTMB_DESAI_JIMO:
	  case CUSTOMER_DTMB_SHANGSHUIXIAN:
		{
			u8  power_led_index = 3;

			if(g_led_buffer[com_num + 1] == 0)
			{
				memcpy(g_led_buffer,content,power_led_index);
				g_led_buffer[power_led_index] = '.';
				memcpy(&(g_led_buffer[power_led_index + 1]),content + power_led_index,com_num - power_led_index);

				//OS_PRINTF("0~~ led buffer :%s ,num :%d\n",g_led_buffer,com_num + 1);
				uio_display(p_dev, g_led_buffer, com_num + 1);
			}
			else
			{
				memcpy(g_led_buffer,content,g_led_index);
				g_led_buffer[g_led_index] = '.';

				memcpy(&(g_led_buffer[g_led_index + 1]),content + g_led_index,power_led_index - g_led_index);
				g_led_buffer[power_led_index + 1] = '.';

				memcpy(&(g_led_buffer[power_led_index + 2]),content + power_led_index,com_num - power_led_index);
				//OS_PRINTF("1~~ led buffer :%s ,num :%d\n",g_led_buffer,com_num + 2);
				uio_display(p_dev, g_led_buffer, com_num + 2);
			}
		}
	  	break;

	  default:
		  if(g_led_buffer[com_num] == 0)
		  {
			memcpy(g_led_buffer,content,com_num);
			//OS_PRINTF("0~~ led buffer :%s ,num :%d\n",g_led_buffer,com_num);
			uio_display(p_dev, g_led_buffer, com_num);
		  }
		  else
		  {
			memcpy(g_led_buffer,content,g_led_index);
			g_led_buffer[g_led_index] = '.';
			memcpy(&(g_led_buffer[g_led_index + 1]),content + g_led_index,com_num - g_led_index);
			//OS_PRINTF("1~~ led buffer :%s ,num :%d\n",g_led_buffer,com_num + 1);
			uio_display(p_dev, g_led_buffer, com_num + 1);
		  }

	  	break;
  }
}
#endif

void ui_set_front_panel_by_str_with_upg(const char *str_flag,u8 process)
{
   char str[5] = {0};
   static u8 i,j;
   void *p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
   memset(str,' ',5);
  if(process == 100)
  {
    i = 0;
  }
  if(i % com_num == 0)
  {
    memset(str,(int)'-',1);
    i++;
  }
  else if(i % com_num == 1)
  {
    memset(str+1,(int)'_',1);
    i++;
  }
  else if(i % com_num == 2)
  {
    i++;
    memset(str + 2,(int)'-',1);
  }
  else if(i % com_num == 3)
  {
    memset(str + 3,(int)'_',1);
    i++;
  }

  if(memcmp(str_flag,"W",1) == 0)
  {
    j = 1;
  }

 if(process == 100)
 {
     if((memcmp(str_flag,"B",1) == 0))
    {
       memcpy(str,"OFF ",4);
     }
     if(memcmp(str_flag,"R",1) == 0)
     {
        if(j == 1)
        {
          memcpy(str,"END ",3);
          j = 0;
        }
     }
  }

   if(memcmp(str_flag,"ERR",3) == 0)
   {
     memcpy(str,"E01 ",4);
   }
   uio_display(p_dev,str,com_num);
  //#endif
}


void ui_set_front_panel_by_str(const char * str)
{
  #if 1
  char content[5];

  if (strlen(str) > 4)
  {
    memcpy(content, str, 4);
    content[4] = '\0';
  }
  else
  {
    if (com_num == 4)
    sprintf(content, "%4s", str);
   else 
    sprintf(content, "%3s ", str); 
   }

  _set_fp_display(content);
  #endif
}

void ui_set_front_panel_by_signal_num(u16 num)
 {
  #if 1
  char content[5];

  if(com_num == 4)
  {
    sprintf(content, "P%.3d", num);
  }
  else if(com_num == 3)
  {
    sprintf(content, "P%.2d", num);
  }
  else if(com_num == 2)
  {
    sprintf(content, "P%.1d", num);
  }
  else if(com_num == 1)
  {
    sprintf(content, "%.1d", num);
  }
  else
  {
    return;
  }

  _set_fp_display(content);
  #endif
}

void ui_set_front_panel_by_num(u16 num)
{
  #if 1
  char content[5];

  if(com_num == 4)
  {
    sprintf(content, "%.4d", num);
  }
  else if(com_num == 3)
  {
    sprintf(content, "%.3d", num);
  }
  else if(com_num == 2)
  {
    sprintf(content, "%.2d", num);
  }
  else if(com_num == 1)
  {
    sprintf(content, "%.1d", num);
  }
  else
  {
    return;
  }
  
  _set_fp_display(content);
  #endif
}

void ui_show_logo(u8 block_id)
{
  RET_CODE ret = SUCCESS;
  static u32  s_size;
  u32 addr;
  static u8* s_p_addr;
  void * p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_VDEC_VSB);

  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,   
    SYS_DEV_TYPE_DISPLAY);

  if( NULL == s_p_addr )
  {
    addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), block_id);
    s_size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), block_id);
    s_size = ROUNDUP(s_size, 4);
    s_p_addr = (u8*)mtos_malloc(s_size);
    MT_ASSERT(s_p_addr != NULL);
    memcpy(s_p_addr, (void*)addr, s_size);
  }
  vdec_stop(p_video_dev);
  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);

  vdec_set_data_input(p_video_dev,1);
  OS_PRINTF("-----ui show logo addr 0x%x , size 0x%x\n",s_p_addr, s_size);
  //vdec_start(p_video_dev, VIDEO_MPEG_ES, 2);    //fix bug 16214
  vdec_start(p_video_dev, VIDEO_MPEG, VID_UNBLANK_STABLE);
  ret = vdec_dec_one_frame(p_video_dev, s_p_addr, s_size);
  MT_ASSERT(SUCCESS == ret);
}

#ifdef SHOW_LOGO_BG
u8* get_pic_buf_addr(u32* p_len)
{
  u32 addr;
  static u32 s_pic_size;
  static u8* s_p_addr;

  if( NULL == s_p_addr )
  {
    addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), 0x92);
    MT_ASSERT(addr != 0);
    s_pic_size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), 0x92);
    MT_ASSERT(s_pic_size != 0);
    *p_len = s_pic_size;
  
    s_pic_size = ROUNDUP(s_pic_size, 4);
    s_p_addr = mtos_malloc(s_pic_size);
    MT_ASSERT(s_p_addr != NULL);
    memcpy(s_p_addr, (void*)addr, *p_len);
  }
  else
  {
    *p_len = s_pic_size;
  }
  
  return s_p_addr;
}
#endif


void ui_enable_video_display(BOOL is_enable)
{
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,   
    SYS_DEV_TYPE_DISPLAY);
  
  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, is_enable);
}


static void _config_osd(rect_t *p_scr)
{
  u16 ss_value = 0;
	sys_status_t *p_systatus = NULL;
	rect_t flinger_rect =
	{
    0,
    0,
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
	};
	flinger_cfg_t bot = 
	{
	  SURFACE_OSD1,
	  1,
	  &flinger_rect,
  };
	rect_t screen_rect = 
	{
    SCREEN_POS_PAL_L,
    SCREEN_POS_PAL_T,
    SCREEN_POS_PAL_L + SCREEN_WIDTH,
    SCREEN_POS_PAL_T + SCREEN_HEIGHT
  };
  screen_reset_t reset_info = 
  {
    &screen_rect,
    NULL,
    &bot,
    COLORFORMAT_ARGB8888,         //osd color format.
    NULL,                         //palette.
    C_TRANS,                      //transparent color.
    C_KEY,                        //color key.
  };
  gdi_reset_screen(&reset_info);

  // get sys status
  p_systatus = sys_status_get();

  ss_value = p_systatus->osd_set.transparent;
  UI_PRINTF("set global alpha = %d\n", (100 - ss_value) * 255 / 100);
  gdi_set_global_alpha((100 - ss_value) * 255 / 100);
}



void ui_config_ttx_osd(void)
{

  s16 x = 0, y = 0;
  disp_sys_t video_std = 0;
  rsc_palette_t hdr_pal = {{0}};
  u8 *p_data = NULL;
  palette_t pal = {0};
  void *p_disp = NULL;
	rect_t flinger_rect =
	{
    0,
    0,
    1280,
    720,
	};
	flinger_cfg_t bot = 
	{
	  SURFACE_OSD1,
	  1,
	  &flinger_rect,
  };
	rect_t screen_rect = 
	{
    0,
    0,
    1280,
    720
  };
  screen_reset_t reset_info = 
  {
    &screen_rect,
    NULL,
    &bot,
    COLORFORMAT_RGB8BIT,                //osd color format.
    &pal,                               //palette.
    C_TRANS_8BIT,                       //transparent color.
    C_KEY_8BIT,                         //color key.
  };
  
  video_std = avc_get_video_mode_1(class_get_handle_by_id(AVC_CLASS_ID));
  switch (video_std)
  { 
    case VID_SYS_NTSC_J:
    case VID_SYS_NTSC_M:
    case VID_SYS_NTSC_443:
    case VID_SYS_PAL_M:
      x = SCREEN_POS_NTSC_L;
      y = SCREEN_POS_NTSC_T;
      break;
    default:
      x = SCREEN_POS_PAL_L;
      y = SCREEN_POS_PAL_T;
  }

  //offset_rect(&rc_scr, x, y);

  rsc_get_palette(gui_get_rsc_handle(), 1, &hdr_pal, (u8**)&p_data);
  pal.cnt = hdr_pal.color_num;
  pal.p_entry = (color_t *)p_data;
  pal.p_entry[C_TRANS_8BIT].a = 0;

  gdi_reset_screen(&reset_info);

  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_disp);
  disp_layer_color_key_onoff(p_disp, DISP_LAYER_ID_OSD1, FALSE);
}

void ui_config_normal_osd(void)
{
  rect_t rc_scr = 
    { 0, 0,
      SCREEN_WIDTH,
      SCREEN_HEIGHT };

  s16 x, y;
  disp_sys_t video_std = 0;
  video_std = avc_get_video_mode_1(class_get_handle_by_id(AVC_CLASS_ID));
  switch (video_std)
  { 
    case VID_SYS_NTSC_J:
    case VID_SYS_NTSC_M:
    case VID_SYS_NTSC_443:
    case VID_SYS_PAL_M:
      x = SCREEN_POS_NTSC_L;
      y = SCREEN_POS_NTSC_T;
      break;
    default:
      x = SCREEN_POS_PAL_L;
      y = SCREEN_POS_PAL_T;
  }

  offset_rect(&rc_scr, x, y);
  _config_osd(&rc_scr);
}

/*
the input size is Kbytes
*/
void ui_conver_file_size_unit_bytes(u32 bytes, u8 *p_str)
{
  u32 m_unit = 1024*1024*1024;
  u32 mod_value = 0;
  if(bytes<=0)
  {
    sprintf(p_str,"0 Bytes");
  }
  else if(bytes/1024/1024/1024 >0)
  {
    mod_value = bytes%m_unit;
    sprintf(p_str,"%ld.%ld%ldG Bytes", bytes/m_unit,mod_value*10/m_unit,(mod_value*10%m_unit)*10/m_unit);
  }
  else if(bytes/1024/1024 >0)
  {
    mod_value = bytes%(1024*1024);
    sprintf(p_str,"%ld.%ld%ldM Bytes", bytes/1024/1024,mod_value*10/1024/1024,(mod_value*10%(1024*1024))*10/1024/1024);
  }
  else if(bytes/1024 >0)
  {
    sprintf(p_str,"%ldK Bytes", bytes/1024);
  }
  else if(bytes >0)
  {
    sprintf(p_str,"%ld Bytes", bytes);
  }
}


/*
the input size is Kbytes
*/
void ui_conver_file_size_unit(u32 k_size, u8 *p_str)
{
  u32 m_unit = 1024*1024;
  u32 mod_value = 0;
  if(k_size<=0)
  {
    sprintf(p_str,"0M Bytes");
  }
  else if(k_size/1024/1024 >0)
  {
    mod_value = k_size%m_unit;
    sprintf(p_str,"%ld.%ld%ldG Bytes", k_size/m_unit,mod_value*10/m_unit,(mod_value*10%m_unit)*10/m_unit);
  }
  else if(k_size/1024 >0)
  {
    mod_value = k_size%1024;
    sprintf(p_str,"%ld.%ld%ldM Bytes", k_size/1024,mod_value*10/1024,(mod_value*10%1024)*10/1024);
  }
  else
  {
    sprintf(p_str,"%ldK Bytes", k_size);
  }
}



void ui_set_net_svc_instance(void *p_instance)
{
  p_net_svc = p_instance;
}

void * ui_get_net_svc_instance(void)
{
  return p_net_svc;
}

void * ui_get_usb_eth_dev(void)
{
  void *usb_eth_dev = NULL;
  void *p_usb_eth_dev = NULL;
  u8 link_state = 0;
  u8 result = 0;
  u8 usb_eth_support_cnt = USB_ETH_SUPPORT_CNT;

  while(usb_eth_support_cnt -- > 0)
  {
    usb_eth_dev = dev_find_identifier(usb_eth_dev, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_ETHERNET_DEV);
    if(usb_eth_dev != NULL)
    {
      dev_io_ctrl(usb_eth_dev, ETH_IOCTRL_IDENTIFY, (u32)(&result));
      if(result == 1)
      {
        p_usb_eth_dev = usb_eth_dev;
        dev_io_ctrl(usb_eth_dev, ETH_IOCTRL_LINK_STATS_CHECK, (u32)&link_state);
        if(link_state)
        {
          break;
        }
      }
    }
  }
  return p_usb_eth_dev;
}

#ifdef ENABLE_ADS
static u8 g_montage_ads_pic_id = 0;
u8* get_pic_buf_addr(u32* p_len)
{
  u32 pic_size = 0;
  u32 block_size = 0;
  u8* p_addr = NULL;
  ads_res_t ads_res = {0};
  
	 p_addr = (u8*)mem_mgr_require_block(BLOCK_REC_BUFFER, 0);
	 mem_mgr_release_block(BLOCK_REC_BUFFER);
	 block_size = mem_mgr_get_block_size(BLOCK_REC_BUFFER);
	 
	 memset(p_addr, 0, block_size);

  ads_res.id = g_montage_ads_pic_id;
  ads_io_ctrl(ADS_ID_ADT_MT, ADS_IOCMD_AD_RES_GET, &ads_res);
  
   if(ads_res.p_head)
   {
      UI_PRINTF("File size:%d\n", ads_res.p_head->dwFileSize);
		  *p_len = ads_res.p_head->dwFileSize;
		  pic_size = ads_res.p_head->dwFileSize;
		  
		  pic_size = ROUNDUP(pic_size, 4);
		  p_addr += (block_size - pic_size);
		  
		  memcpy(p_addr, ads_res.p_data, *p_len);
   }
   else
   	{
		  *p_len =0;
		  pic_size = 0;
		  
		  pic_size = ROUNDUP(pic_size, 4);
		  p_addr += (block_size - pic_size);
		  
		  //memset(p_addr, 0, *p_len);
   	}

  return p_addr;
}

//use it when show pic in no audio and video
void ui_show_logo_bg(ads_type_t ads_type , rect_t *rect)
{
  mul_pic_param_t pic_param;
  
//  u32 addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), block_id);
//  u32 size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), block_id);

  if(ads_type == PIC_TYPE_MAIN_MENU)
  {
    g_montage_ads_pic_id = 0xc1;
  }
  else if(ads_type == PIC_TYPE_SUB_MENU)
  {
    g_montage_ads_pic_id = 0xc2;
  }
  
  ui_pic_init(PIC_SOURCE_BUF);

  pic_param.anim = REND_ANIM_NONE;
  pic_param.file_size = 0;
  pic_param.is_file = FALSE;
  pic_param.flip = PIC_NO_F_NO_R;
  pic_param.win_rect.left = rect->left;
  pic_param.win_rect.top = rect->top;
  pic_param.win_rect.right = rect->right;
  pic_param.win_rect.bottom = rect->bottom;
  pic_param.style = REND_STYLE_CENTER;
  pic_param.buf_get = (u32)get_pic_buf_addr; 

  pic_start(&pic_param);
}

void ui_close_logo_bg(void)
{
  OS_PRINTF("enter %s\n", __FUNCTION__);
  pic_stop();
  ui_pic_release();
  OS_PRINTF("exit %s\n", __FUNCTION__);
}



#endif




