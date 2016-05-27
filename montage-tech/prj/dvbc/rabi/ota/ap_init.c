/******************************************************************************/
/******************************************************************************/

// standard library
#include <stdlib.h>
#include <string.h>

// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_concerto.h"
#include "sys_devs.h"
#include "sys_cfg.h"
#include "driver.h"

// os
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
#include "mtos_event.h"

// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "char_map.h"
#include "gb2312.h"
#include "fcrc.h"

// driver
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_misc.h"
#include "hal_uart.h"

#include "common.h"
#include "drv_dev.h"
#include "drv_misc.h"

#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"


#include "display.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "gpe_vsb.h"

#include "nim.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"
#include "sys_types.h"
#include "smc_op.h"
#include "hdmi.h"
#include "region.h"
#include "display.h"
#include "pdec.h"
// mdl
#include "mdl.h"

#include "data_manager.h"
#include "data_manager_v2.h"
#include "data_base.h"
#include "data_base16v2.h"

#include "service.h"
#include "smc_ctrl.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "nit.h"
#include "pat.h"
#include "mosaic.h"
#include "pmt.h"
#include "sdt.h"
#include "cat.h"
#include "emm.h"
#include "ecm.h"
#include "bat.h"
#include "video_packet.h"
#include "eit.h"
#include "dsmcc.h"
//#include "dsmcc_galaxy.h"
//#include "cas_ware.h"
#include "monitor_service.h"
#include "dmx.h"

#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "drv_svc.h"

#include "dvbc_util.h"
#include "ss_ctrl.h"
#include "mt_time.h"

#include "avctrl1.h"

#include "surface.h"


//eva
#include "interface.h"
#include "eva.h"
// ap
#include "ap_framework.h"

#include "ap_uio.h"
#include "ap_playback.h"
#include "ap_scan.h"
#include "ap_signal_monitor.h"
#include "ap_satcodx.h"
//#include "ap_dvbs_ota.h"
#include "ap_upgrade.h"
#include "ap_time.h"
//#include "ap_cas.h"
//#include "ap_ota.h"
//#include "ap_twin_port.h"

#include "ota_public_def.h"
//eva
#include "interface.h"
#include "eva.h"
#include "media_format_define.h"
#include "ipin.h"
#include "ifilter.h"
#include "chain.h"
#include "controller.h"
#include "eva_filter_factory.h"
#include "common_filter.h"
#include "demux_filter.h"
#include "flash_sink_filter.h"
#include "ota_filter.h"
#include "lpower.h"

#include "db_dvbs.h"
#include "sys_status.h"
#include "spi.h"
#include "mem_cfg.h"
#include "customer_config.h"
char *even_mem = 0;
char *odd_mem = 0;

hw_cfg_t hw_cfg = {0};
extern app_t *construct_virtual_nim_filter(app_id_t app_id,u8 tuner_id);

extern u8 ui_get_com_num(void);

#define PTI_PARALLEL

/*!
    The device id of GigaDevice
  */
#define FLASH_MF_GD     0xC8
/*!
    The device id of WINBOND
  */
#define FLASH_MF_WINB   0xEF

#define FLASH_MF_ISSI    0x9D

charsto_protect_info_t e_char_prot_t[] = 
{
   //GD
   {FLASH_MF_GD, {PRT_UNPROT_ALL,  0x00000000, 0x407C, 2}},//0
   {FLASH_MF_GD, {PRT_PROT_ALL,    0x0000001C, 0x407C, 2}},//1
   {FLASH_MF_GD, {PRT_UPPER_1_64,  0x00000004, 0x407C, 2}},//2
   {FLASH_MF_GD, {PRT_UPPER_1_32,  0x00000008, 0x407C, 2}},//3
   {FLASH_MF_GD, {PRT_UPPER_1_16,  0x0000000c, 0x407C, 2}},//4
   {FLASH_MF_GD, {PRT_UPPER_1_8,   0x00000010, 0x407C, 2}},//5
   {FLASH_MF_GD, {PRT_UPPER_1_4,   0x00000014, 0x407C, 2}},//6
   {FLASH_MF_GD, {PRT_UPPER_1_2,   0x00000018, 0x407C, 2}},//7
   {FLASH_MF_GD, {PRT_LOWER_1_64,  0x00000024, 0x407C, 2}},//8
   {FLASH_MF_GD, {PRT_LOWER_1_32,  0x00000028, 0x407C, 2}},//9
   {FLASH_MF_GD, {PRT_LOWER_1_16,  0x0000002C, 0x407C, 2}},//10
   {FLASH_MF_GD, {PRT_LOWER_1_8,   0x00000030, 0x407C, 2}},//11
   {FLASH_MF_GD, {PRT_LOWER_1_4,   0x00000034, 0x407C, 2}},//12
   {FLASH_MF_GD, {PRT_LOWER_1_2,   0x00000038, 0x407C, 2}},//13
   {FLASH_MF_GD, {PRT_LOWER_3_4,   0x00004014, 0x407C, 2}},//14
   {FLASH_MF_GD, {PRT_LOWER_7_8,   0x00004010, 0x407C, 2}},//15
   {FLASH_MF_GD, {PRT_LOWER_15_16, 0x0000400c, 0x407C, 2}},//16
   {FLASH_MF_GD, {PRT_LOWER_31_32, 0x00004008, 0x407C, 2}},//17
   {FLASH_MF_GD, {PRT_LOWER_63_64, 0x00004004, 0x407C, 2}},//18
    //winbond
   {FLASH_MF_WINB, {PRT_UNPROT_ALL,  0x00000000, 0x407C, 2}},//0
   {FLASH_MF_WINB, {PRT_PROT_ALL,    0x0000001C, 0x407C, 2}},//1
   {FLASH_MF_WINB, {PRT_UPPER_1_64,  0x00000004, 0x407C, 2}},//2
   {FLASH_MF_WINB, {PRT_UPPER_1_32,  0x00000008, 0x407C, 2}},//3
   {FLASH_MF_WINB, {PRT_UPPER_1_16,  0x0000000c, 0x407C, 2}},//4
   {FLASH_MF_WINB, {PRT_UPPER_1_8,   0x00000010, 0x407C, 2}},//5
   {FLASH_MF_WINB, {PRT_UPPER_1_4,   0x00000014, 0x407C, 2}},//6
   {FLASH_MF_WINB, {PRT_UPPER_1_2,   0x00000018, 0x407C, 2}},//7
   {FLASH_MF_WINB, {PRT_LOWER_1_64,  0x00000024, 0x407C, 2}},//8
   {FLASH_MF_WINB, {PRT_LOWER_1_32,  0x00000028, 0x407C, 2}},//9
   {FLASH_MF_WINB, {PRT_LOWER_1_16,  0x0000002C, 0x407C, 2}},//10
   {FLASH_MF_WINB, {PRT_LOWER_1_8,   0x00000030, 0x407C, 2}},//11
   {FLASH_MF_WINB, {PRT_LOWER_1_4,   0x00000034, 0x407C, 2}},//12
   {FLASH_MF_WINB, {PRT_LOWER_1_2,   0x00000038, 0x407C, 2}},//13
   {FLASH_MF_WINB, {PRT_LOWER_3_4,   0x00004014, 0x407C, 2}},//14
   {FLASH_MF_WINB, {PRT_LOWER_7_8,   0x00004010, 0x407C, 2}},//15
   {FLASH_MF_WINB, {PRT_LOWER_15_16, 0x0000400c, 0x407C, 2}},//16
   {FLASH_MF_WINB, {PRT_LOWER_31_32, 0x00004008, 0x407C, 2}},//17
   {FLASH_MF_WINB, {PRT_LOWER_63_64, 0x00004004, 0x407C, 2}},//18
    //ISSI
   {FLASH_MF_ISSI, {PRT_UNPROT_ALL,  0x00000000, 0x3C, 1}},//1
   {FLASH_MF_ISSI, {PRT_PROT_ALL,    0x00000028, 0x3C, 1}},//2
   {FLASH_MF_ISSI, {PRT_UPPER_1_64,  0x00000004, 0x3C, 1}},//3
   {FLASH_MF_ISSI, {PRT_UPPER_1_32,  0x00000008, 0x3C, 1}},//4
   {FLASH_MF_ISSI, {PRT_UPPER_1_16,  0x0000000c, 0x3C, 1}},//5
   {FLASH_MF_ISSI, {PRT_UPPER_1_8,   0x00000010, 0x3C, 1}},//6
   {FLASH_MF_ISSI, {PRT_UPPER_1_4,   0x00000014, 0x3C, 1}},//7
   {FLASH_MF_ISSI, {PRT_UPPER_1_2,   0x00000018, 0x3C, 1}},//8
   {FLASH_MF_ISSI, {PRT_LOWER_1_64,  0x0000001C, 0x3C, 1}},//9
   {FLASH_MF_ISSI, {PRT_LOWER_1_32,  0x00000020, 0x3C, 1}},//10
   {FLASH_MF_ISSI, {PRT_LOWER_1_16,  0x00000024, 0x3C, 1}},//11
};

static RET_CODE find_protect_cmd(u8 m_id, charsto_prot_status_t *pro_st)
{
  u32 i = 0;
  RET_CODE ret = ERR_FAILURE;
  
  for(i = 0; i < sizeof(e_char_prot_t) / sizeof(charsto_protect_info_t); i++)
  {
    if((e_char_prot_t[i].m_id == m_id) && (e_char_prot_t[i].prot_st.prt_t == pro_st->prt_t))
    {
      pro_st->st = e_char_prot_t[i].prot_st.st;
      pro_st->len = e_char_prot_t[i].prot_st.len;
      pro_st->st_mask = e_char_prot_t[i].prot_st.st_mask;
      ret = SUCCESS;
    }
  }
  return ret;
}

RET_CODE charsto_set_protect(spi_prot_block_type_t p_t)
{
  RET_CODE ret = ERR_FAILURE;
  u32 mid = 0;
  charsto_prot_status_t pro_st;
  void  * p_char_dev = (struct charsto_device *)dev_find_identifier(NULL, DEV_IDT_TYPE, 
                                                              SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(p_char_dev != NULL);
  
  /*Get manufactor id to search the mach cmd for set protect type*/
#ifndef WIN32
  ret = dev_io_ctrl(p_char_dev, CHARSTO_IOCTRL_GET_DEV_ID, (u32)&mid);
  MT_ASSERT(ret == SUCCESS);
  
  mid = (mid & 0xFF);
  pro_st.prt_t = p_t;
  ret = find_protect_cmd(mid, &pro_st);

  if(ret != SUCCESS)
  {
    OS_PRINTF("flash mid[%x] not in prot_info_list\n",mid);
  }
  
  ret = dev_io_ctrl(p_char_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&pro_st);
  if(ret != SUCCESS)
  {
     OS_PRINTF("set mid[%x] protect status[%x] fail!\n",mid, pro_st.st);
  }
  OS_PRINTF("set mid[%x] protect status[%x] type[%d]Success\n",mid, pro_st.st, p_t);
#endif
  return ret;
}

u8 ui_get_com_num(void)
{
  return hw_cfg.led_num;
}

#ifdef WIN32
u32 get_flash_addr(void)
{
  extern u8 *p_flash_buf;
  return (u32)p_flash_buf;
}

 inline RET_CODE gpe_vsb_sim_attach_warriors(char *name)
{
    return gpe_soft_attach(name);
}
#endif

static BOOL init_display_cfg(disp_cfg_t *p_cfg)
{
  u32 p_addr = 0;

  //init sub buffer
  p_addr = mem_mgr_require_block(BLOCK_SUB_32BIT_BUFFER, SYS_MODULE_GDI);
  MT_ASSERT(p_addr != 0);

  mem_mgr_release_block(BLOCK_SUB_32BIT_BUFFER);

#ifndef WIN32
  p_cfg->p_sub_cfg->odd_mem_start = p_addr;
  p_cfg->p_sub_cfg->odd_mem_end = p_addr + SUB_32BIT_BUFFER_SIZE;
  p_cfg->p_sub_cfg->even_mem_start = 0;
  p_cfg->p_sub_cfg->even_mem_end = 0;
#else
  p_cfg->p_sub_cfg->odd_mem_start = p_addr;
  p_cfg->p_sub_cfg->odd_mem_end = p_addr + SUB_32BIT_BUFFER_SIZE/2;
  p_cfg->p_sub_cfg->even_mem_start = p_addr + SUB_32BIT_BUFFER_SIZE/2;
  p_cfg->p_sub_cfg->even_mem_end = p_addr + SUB_32BIT_BUFFER_SIZE;
#endif

  // init osd0 frame buffer
  p_addr = mem_mgr_require_block(BLOCK_OSD1_32BIT_BUFFER, SYS_MODULE_GDI);
  MT_ASSERT(p_addr != 0);

  mem_mgr_release_block(BLOCK_OSD1_32BIT_BUFFER);

#ifndef WIN32
  p_cfg->p_osd0_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd0_cfg->odd_mem_end = p_addr +  OSD1_32BIT_BUFFER_SIZE;
  p_cfg->p_osd0_cfg->even_mem_start = 0;
  p_cfg->p_osd0_cfg->even_mem_end = 0;
#else
  p_cfg->p_osd0_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd0_cfg->odd_mem_end = p_addr +  OSD1_32BIT_BUFFER_SIZE/2;
  p_cfg->p_osd0_cfg->even_mem_start = p_addr +  OSD1_32BIT_BUFFER_SIZE/2;
  p_cfg->p_osd0_cfg->even_mem_end = p_addr +  OSD1_32BIT_BUFFER_SIZE;
#endif

  even_mem = (char *)(p_cfg->p_osd0_cfg->even_mem_start + 0 * 1024);
  odd_mem = (char *)(p_cfg->p_osd0_cfg->odd_mem_start + 2 *1024);


  // init osd1 frame buffer
  p_addr = mem_mgr_require_block(BLOCK_OSD0_32BIT_BUFFER, SYS_MODULE_GDI);
  MT_ASSERT(p_addr != 0);

  mem_mgr_release_block(BLOCK_OSD0_32BIT_BUFFER);

#ifndef WIN32
  p_cfg->p_osd1_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd1_cfg->odd_mem_end = p_addr +  OSD0_32BIT_BUFFER_SIZE;
  p_cfg->p_osd1_cfg->even_mem_start = 0;
  p_cfg->p_osd1_cfg->even_mem_end = 0;
#else
  p_cfg->p_osd1_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd1_cfg->odd_mem_end = p_addr +  OSD0_32BIT_BUFFER_SIZE/2;
  p_cfg->p_osd1_cfg->even_mem_start = p_addr +  OSD0_32BIT_BUFFER_SIZE/2;
  p_cfg->p_osd1_cfg->even_mem_end = p_addr +  OSD0_32BIT_BUFFER_SIZE;
#endif

  return TRUE;
}


void uio_init()
{
  RET_CODE ret;
  void *p_dev = NULL;
  uio_cfg_t uiocfg = {0};
  fp_cfg_t fpcfg = {0};
  irda_cfg_t irdacfg = {0};
  u32 read_len = 0;
#ifndef WIN32
  u32 reg= 0 , val = 0;
  void *p_dev_i2cfp = NULL;
#endif

  OS_PRINTF("\r\n[OTA]uio attach");
  //uio including irda and frontpanel dev....
   ret = uio_attach_concerto("concerto");
  MT_ASSERT(ret == SUCCESS);
 
  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                     HW_CFG_BLOCK_ID, 0, 0,
                     sizeof(hw_cfg_t),
                     (u8 *)&hw_cfg);
  MT_ASSERT(read_len == sizeof(hw_cfg_t));

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  MT_ASSERT(NULL != p_dev);

  fpcfg.p_info = &(hw_cfg.pan_info);
  fpcfg.p_map = hw_cfg.led_bitmap;
  fpcfg.map_size = hw_cfg.map_size;
  fpcfg.fp_type = hw_cfg.fp_type;

  if(fpcfg.fp_type != HAL_GPIO)
  {
    fpcfg.p_map = NULL; //650D/1635 use default LED bitmap
  }
  #ifndef WIN32
  // PINMUX
  switch (fpcfg.fp_type)
  {
   case HAL_CT1642:
   case HAL_LM8168:
     reg = R_IE3_SEL;
     val = *((volatile u32 *)reg);
     val &= ~(0x7 << 27);
     val |= 0x5 << 27;
     *((volatile u32 *)reg) = val;
  
     reg = R_PIN8_SEL;
     val = *((volatile u32 *)reg);
     val &= ~(0xFFF << 16);
     val |= 0x444 << 16;
     *((volatile u32 *)reg) = val;
     break;
  
   case HAL_FD650:
     reg = R_IE3_SEL;
     val = *((volatile u32 *)reg);
     val |= 0x3 << 27;
     *((volatile u32 *)reg) = val;
  
     reg = R_PIN8_SEL;
     val = *((volatile u32 *)reg);
     val &= ~(0xFF << 16);
     val |= 0x33 << 16;
     *((volatile u32 *)reg) = val;
  
     p_dev_i2cfp = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_FP");
     MT_ASSERT(NULL != p_dev_i2cfp);
     break;
  
   default:
     break;
  }
  if (NULL != p_dev_i2cfp)
  {
    fpcfg.p_bus_dev = p_dev_i2cfp;
  }
#endif
  irdacfg.protocol = IRDA_NEC;
  irdacfg.code_mode = OUR_DEF_MODE;
  irdacfg.irda_repeat_time = 300;

{
  u8 i;
  u8 power_keys[2];
  u8 tmp;
  u16 user_code;
  u8 block_ids[2] = {IRKEY1_BLOCK_ID, IRKEY2_BLOCK_ID};


  irdacfg.irda_wfilt_channel = 2;

  for (i=0; i<2; i++)
  {
    dm_read(class_get_handle_by_id(DM_CLASS_ID),
          block_ids[i], 0, 1,
          1,
          (u8 *)&tmp);
    user_code = tmp;
    dm_read(class_get_handle_by_id(DM_CLASS_ID),
          block_ids[i], 0, 0,
          1,
          (u8 *)&tmp);
    user_code |= ((u16)tmp) << 8;
    
    dm_read(class_get_handle_by_id(DM_CLASS_ID),
          block_ids[i], 0, 2,
          1,
          (u8 *)&power_keys[i]);
    
    irdacfg.irda_wfilt_channel_cfg[i].wfilt_code |= ((u32)user_code) << 16;
    irdacfg.irda_wfilt_channel_cfg[i].protocol = IRDA_NEC;
    irdacfg.irda_wfilt_channel_cfg[i].addr_len = 32;
    tmp = ~power_keys[i];
    irdacfg.irda_wfilt_channel_cfg[i].wfilt_code |= tmp;
    irdacfg.irda_wfilt_channel_cfg[i].wfilt_code |= ((u16)power_keys[i]) << 8;
    
    OS_PRINTF("wavefilter code 0x%x\n", irdacfg.irda_wfilt_channel_cfg[i].wfilt_code);
  }
}

  uiocfg.p_ircfg = &irdacfg;
  uiocfg.p_fpcfg = &fpcfg;
  ret = dev_open(p_dev, &uiocfg);
  MT_ASSERT(SUCCESS == ret);

  if(hw_cfg.led_num == 4)
  {
    uio_display(p_dev, " ON ", 4);
  }
  else if(hw_cfg.led_num == 3)
  {
    uio_display(p_dev, "ON ", 3);
  }
  else if(hw_cfg.led_num == 2)
  {
    uio_display(p_dev, "ON ", 2);
  }
  else if(hw_cfg.led_num == 1)
  {
    uio_display(p_dev, "O ", 1);
  }

  dev_io_ctrl(p_dev, UIO_FP_SET_POWER_LBD, TRUE);

    switch (fpcfg.fp_type)
  {
    case HAL_CT1642:
      standby_cpu_attach_fw_ct1642_concerto();
      break;
    case HAL_FD650:
      standby_cpu_attach_fw_fd650_concerto();
      break;
    default :
      standby_cpu_attach_fw_fd650_concerto();
      break;
  }

}

#if 1
static void disp_set_hdmi(hdmi_video_config_t *p_hdmi_vcfg, disp_sys_t fmt)
{
    switch(fmt)
    {
      case VID_SYS_PAL:
      case VID_SYS_PAL_N:
      case VID_SYS_PAL_NC:
      case VID_SYS_PAL_M:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_2_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_576I;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_PAL;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_NTSC_J:
      case VID_SYS_NTSC_M:
      case VID_SYS_NTSC_443:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_2_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_480I;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_NTSC;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_576P_50HZ:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_576P;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_PAL;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_480P:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_480P;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_NTSC;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_720P:
      case VID_SYS_720P_30HZ:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_720P;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_NTSC;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_720P_50HZ:
      case VID_SYS_720P_24HZ:
      case VID_SYS_720P_25HZ:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_720P;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_PAL;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_1080I:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_1080I;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_NTSC;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_1080I_50HZ:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_1080I;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_PAL;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_1080P:
      case VID_SYS_1080P_30HZ:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_1080P;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_NTSC;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      case VID_SYS_1080P_50HZ:
      case VID_SYS_1080P_25HZ:
      case VID_SYS_1080P_24HZ:
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_1080P;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_PAL;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
        break;
      default: //1080i50
        p_hdmi_vcfg->input_v_cfg.fmt = RGB_YCBCR444_SYNCS;
        p_hdmi_vcfg->input_v_cfg.csc = HDMI_COLOR_YUV444;
        p_hdmi_vcfg->input_v_cfg.ddr_edge = DDR_EDGE_FALLING;
        p_hdmi_vcfg->input_v_cfg.pixel_rpt = PIXEL_RPT_1_TIMES;
        p_hdmi_vcfg->output_v_cfg.resolution = HDMI_1080I;
        p_hdmi_vcfg->output_v_cfg.shape = HDMI_SHAPE_16X9;
        p_hdmi_vcfg->output_v_cfg.standard = HDMI_PAL;
        p_hdmi_vcfg->output_v_cfg.color_space = HDMI_COLOR_AUTO;
    }
}

static void hdmi_driver_attach(void* p_drvsvc)
{
  RET_CODE ret;
  u32 reg= 0, val = 0;
  hdmi_cfg_t hdmi_cfg = {0};
  void * p_i2c_master = NULL;
  drv_dev_t *p_hdmi_dev = NULL;

  ret = hdmi_anx8560_attach("hdmi");
  MT_ASSERT(ret == SUCCESS);

  p_hdmi_dev = dev_find_identifier(NULL,
                              DEV_IDT_TYPE,
                              SYS_DEV_TYPE_HDMI);
  MT_ASSERT(p_hdmi_dev != NULL);

  // PINMUX
  reg = R_PIN3_SEL;
  val = *((volatile u32 *)reg);
  val &= ~(0xFF << 16);
  *((volatile u32 *)reg) = val;

  if (SUCCESS != hal_module_clk_get(HAL_HD_VIDEO, &val))
  {
    OS_PRINTF("HAL_HD_VIDEO mclk: 74.25MHz\n");
    hal_module_clk_set(HAL_HD_VIDEO, 74250000);
  }

  p_i2c_master = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_HDMI");
  MT_ASSERT(NULL != p_i2c_master);

  memset(&hdmi_cfg, 0x0, sizeof(hdmi_cfg_t));
  if(NULL == p_drvsvc)
  {
  hdmi_cfg.task_prio = DRV_HDMI_TASK_PRIORITY;
  hdmi_cfg.stack_size = (8 * KBYTES);
  }
  else
  {
    hdmi_cfg.p_drvsvc = p_drvsvc;
  }
  hdmi_cfg.i2c_master = p_i2c_master;
  hdmi_cfg.is_initialized = 0;
  hdmi_cfg.is_fastlogo_mode = 0;
//#ifdef CHIP
  ret = dev_open(p_hdmi_dev, &hdmi_cfg);
  MT_ASSERT(SUCCESS == ret);
}
#endif

void spiflash_cfg( )
{
    RET_CODE ret;
    void *p_dev = NULL;
    void *p_spi = NULL; 
    charsto_cfg_t charsto_cfg = {0};
    spi_cfg_t spiCfg;

    spiCfg.bus_clk_mhz   = 50;//10;
    spiCfg.bus_clk_delay = 12;//8;
    spiCfg.io_num        = 1;
    spiCfg.lock_mode     = OS_MUTEX_LOCK;
    spiCfg.op_mode       = 0;
    spiCfg.pins_cfg[0].miso1_src  = 0;
    spiCfg.pins_cfg[0].miso0_src  = 1;
    spiCfg.pins_cfg[0].spiio1_src = 0;
    spiCfg.pins_cfg[0].spiio0_src = 0;
    spiCfg.pins_dir[0].spiio1_dir = 3;
    spiCfg.pins_dir[0].spiio0_dir = 3;
    spiCfg.spi_id = 0;
    
    ret = spi_concerto_attach("spi_concerto_0");
    MT_ASSERT(SUCCESS == ret);
    p_spi = dev_find_identifier(NULL,DEV_IDT_NAME, (u32)"spi_concerto_0");
    spiCfg.spi_id = 0;
    ret = dev_open(p_spi, &spiCfg);
    MT_ASSERT(SUCCESS == ret);

    spiflash_jazz_attach("charsto_concerto");
    OS_PRINTF("drv --charsto 1\n");

    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    MT_ASSERT(NULL != p_dev);
    OS_PRINTF("drv --charsto 2\n");
 
    charsto_cfg.rd_mode = SPI_FR_MODE;
    charsto_cfg.p_bus_handle = p_spi;
    charsto_cfg.size = 4*1024*1024;
    ret = dev_open(p_dev, &charsto_cfg);
    MT_ASSERT(SUCCESS == ret);
		
}


static void disp_vdac_config_ota(void * p_disp)
{
  vdac_info_t v_dac_info = hw_cfg.vdac_info;
  
  OS_PRINTF("[disp_vdac_config_ota] v_dav_info = %d \n",v_dac_info);
  
  switch(v_dac_info)
  {
    case VDAC_CVBS_RGB:
    {
      OS_PRINTF("\nVDAC_CVBS_RGB\n");
      disp_cvbs_onoff(p_disp, CVBS_GRP0, FALSE);
      disp_component_set_type(p_disp, COMPONENT_GRP0,COLOR_CVBS_RGB);
      disp_component_onoff(p_disp, COMPONENT_GRP0, TRUE);
    }
    break;
    
    case VDAC_SIGN_CVBS:
    {
      OS_PRINTF("\nVDAC_SIGN_CVBS\n");
      disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      disp_component_set_type(p_disp, COMPONENT_GRP1,COLOR_YUV);
      disp_component_onoff(p_disp, COMPONENT_GRP1, TRUE);
      disp_dac_onoff(p_disp, DAC_1,  FALSE);
      disp_dac_onoff(p_disp, DAC_2,  FALSE);

    }
    break;
    
    case VDAC_SIGN_CVBS_LOW_POWER:
    {
      OS_PRINTF("\nVDAC_SIGN_CVBS_LOW_POWER\n");
      disp_cvbs_onoff(p_disp, CVBS_GRP3, TRUE);
      disp_dac_onoff(p_disp, DAC_0,  FALSE);
      disp_dac_onoff(p_disp, DAC_1,  FALSE);
      disp_dac_onoff(p_disp, DAC_2,  FALSE);
    }
    break;
    
    case VDAC_DUAL_CVBS:
    {
      OS_PRINTF("\nVDAC_DUAL_CVBS\n");
      disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      disp_cvbs_onoff(p_disp, CVBS_GRP1, FALSE);
      disp_cvbs_onoff(p_disp, CVBS_GRP2, FALSE);
      disp_cvbs_onoff(p_disp, CVBS_GRP3, TRUE);
    }
    break;
    
    case VDAC_CVBS_SVIDEO:
    {
      OS_PRINTF("\nVDAC_CVBS_SVIDEO\n");
      disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      disp_svideo_onoff(p_disp, SVIDEO_GRP0, TRUE);
      disp_cvbs_onoff(p_disp, CVBS_GRP2, TRUE);
      disp_cvbs_onoff(p_disp, CVBS_GRP3, TRUE);
    }
    break;
    
    case VDAC_CVBS_YPBPR_SD:
    {
      OS_PRINTF("\nVDAC_CVBS_YPBPR_SD\n");
      disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      disp_component_set_type(p_disp, COMPONENT_GRP0, COLOR_YUV);
      disp_component_onoff(p_disp, COMPONENT_GRP0, TRUE);
    }
    break;
    
    case VDAC_CVBS_YPBPR_HD:
    {
      OS_PRINTF("\nVDAC_CVBS_YPBPR_HD\n");
      disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      disp_component_set_type(p_disp, COMPONENT_GRP1,COLOR_YUV);
      disp_component_onoff(p_disp, COMPONENT_GRP1, TRUE);
    }
    break;
    
    default:
    {
      OS_PRINTF("\nVDAC setting default, CVBS + HD COMPONENT \n");
      disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      disp_component_set_type(p_disp, COMPONENT_GRP1,COLOR_YUV);
      disp_component_onoff(p_disp, COMPONENT_GRP1, TRUE);
    }
    break;
  }
}


extern u32 get_flash_addr(void);

static void drv_init_concerto(void)
{
  RET_CODE ret;
  void *p_dev = NULL;
  void *p_dev_i2c0 = NULL;
  void *p_dev_i2c1 = NULL;
  void *p_dev_i2cfp = NULL;
  void *p_dev_i2chdmi = NULL;
  void *p_dev_i2cqam = NULL;
  drvsvc_handle_t *p_public_drvsvc = NULL;
  u32 *p_buf = NULL;
  dm_v2_init_para_t dm_para = { 0 };

  //this service will be shared by HDMI, Audio and Display driver
  p_buf = (u32 *)mtos_malloc(DRV_HDMI_TASK_STKSIZE);
  MT_ASSERT(NULL != p_buf);
  p_public_drvsvc = drvsvc_create(DRV_HDMI_TASK_PRIORITY,
    p_buf, DRV_HDMI_TASK_STKSIZE, 8);
  MT_ASSERT(NULL != p_public_drvsvc);


#if 1
  /* GPE */
  OS_PRINTF("-------------- GPE_USE_HW  \n");
  void *p_gpe_c = NULL;
  ret = cct_gpe_attach("cct_gpe");
  p_gpe_c = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  ret = dev_open(p_gpe_c, NULL);
  MT_ASSERT(TRUE == ret);
#else // soft
  /* GPE */
  void *p_gpe_c = NULL;
  ret = gpe_soft_attach("gpe_soft");
  p_gpe_c = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  ret = dev_open(p_gpe_c, NULL);
  MT_ASSERT(SUCCESS == ret);
#endif

  {
    /* I2C 0 */
#if 1
    i2c_cfg_t i2c_cfg = {0};
    ret = i2c_concerto_attach("i2c0");
    MT_ASSERT(SUCCESS == ret);

    p_dev_i2c0 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c0");
    MT_ASSERT(NULL != p_dev_i2c0);
    i2c_cfg.i2c_id = 0;
    i2c_cfg.bus_clk_khz = 100;
    i2c_cfg.lock_mode = OS_MUTEX_LOCK;
    ret = dev_open(p_dev_i2c0, &i2c_cfg);
    MT_ASSERT(SUCCESS == ret);
#endif
  }
  {
    /* I2C 1 */
#if 1
    i2c_cfg_t i2c_cfg = {0};
    ret = i2c_concerto_attach("i2c1");
    MT_ASSERT(SUCCESS == ret);

    p_dev_i2c1 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c1");
    MT_ASSERT(NULL != p_dev_i2c1);
    i2c_cfg.i2c_id = 1;
    i2c_cfg.bus_clk_khz = 100;
    i2c_cfg.lock_mode = OS_MUTEX_LOCK;
    ret = dev_open(p_dev_i2c1, &i2c_cfg);
    MT_ASSERT(SUCCESS == ret);
#endif
  }

  {
    /* I2C FP */
#if 1
    i2c_cfg_t i2c_cfg = {0};
    ret = i2c_concerto_attach("i2c_FP");
    MT_ASSERT(SUCCESS == ret);

    p_dev_i2cfp = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_FP");
    MT_ASSERT(NULL != p_dev_i2cfp);
    i2c_cfg.i2c_id = 2;
    i2c_cfg.bus_clk_khz = 100;
    i2c_cfg.lock_mode = OS_MUTEX_LOCK;
    ret = dev_open(p_dev_i2cfp, &i2c_cfg);
    MT_ASSERT(SUCCESS == ret);
#endif
  }

  {
    /* I2C HDMI */
#if 1
    i2c_cfg_t i2c_cfg = {0};
    ret = i2c_concerto_attach("i2c_HDMI");
    MT_ASSERT(SUCCESS == ret);

    p_dev_i2chdmi = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_HDMI");
    MT_ASSERT(NULL != p_dev_i2chdmi);
    i2c_cfg.i2c_id = 3;
    i2c_cfg.bus_clk_khz = 100;
    i2c_cfg.lock_mode = OS_MUTEX_LOCK;
    ret = dev_open(p_dev_i2chdmi, &i2c_cfg);
    MT_ASSERT(SUCCESS == ret);
#endif
  }

  {
    /* I2C QAM */
#if 1
    i2c_cfg_t i2c_cfg = {0};
    ret = i2c_concerto_attach("i2c_QAM");
    MT_ASSERT(SUCCESS == ret);

    p_dev_i2cqam = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_QAM");
    MT_ASSERT(NULL != p_dev_i2cqam);
    i2c_cfg.i2c_id = 4;
    i2c_cfg.bus_clk_khz = 400;
    i2c_cfg.lock_mode = OS_MUTEX_LOCK;
    ret = dev_open(p_dev_i2cqam, &i2c_cfg);
    MT_ASSERT(SUCCESS == ret);
#endif
  }
#if 1
#ifdef DTMB_PROJECT
  extern RET_CODE nim_m88dd3k_attach(char *name);
  
  nim_config_t nim_cfg = {0};
  ret = nim_m88dd3k_attach("nim_dd3k");
  MT_ASSERT(ret == SUCCESS);
  
  p_dev = dev_find_identifier(p_dev, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
  MT_ASSERT(NULL != p_dev);
  nim_cfg.p_dem_bus = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c0");
  MT_ASSERT(NULL != nim_cfg.p_dem_bus);
  nim_cfg.ts_mode = NIM_TS_INTF_SERIAL;
  nim_cfg.tn_version = TC3800;
  
  gpio_io_enable(65, TRUE);
  gpio_set_dir(65, GPIO_DIR_OUTPUT);
  gpio_set_value(65, 0);
  mtos_task_delay_ms(20);
  gpio_set_value(65, 1);
  mtos_task_delay_ms(10);
  
  ret = dev_open(p_dev, &nim_cfg);
  
  MT_ASSERT(SUCCESS == ret);
  
  dev_io_ctrl(p_dev, DEV_IOCTRL_POWER, DEV_POWER_FULLSPEED);
#else
  extern void tuner_attach_tc2800();

  nim_config_t nim_cfg = {0};
  ret = nim_attach_concerto("nim");
  MT_ASSERT(ret == SUCCESS);
  tuner_attach_tc2800();
  
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
  MT_ASSERT(NULL != p_dev);
  nim_cfg.p_dem_bus = p_dev_i2cqam;
  nim_cfg.lock_mode = OS_LOCK_DEF;
#ifndef PTI_PARALLEL
  nim_cfg.dem_addr = 0x3a;
  nim_cfg.tun_addr = 0xc2;
  nim_cfg.ts_mode = NIM_TS_INTF_SERIAL;
#else
  nim_cfg.dem_addr = 0x38;
  nim_cfg.tun_addr = 0xc2;
  nim_cfg.ts_mode = NIM_TS_INTF_PARALLEL;
#endif

  nim_cfg.dem_ver = DEM_VER_3;

  ret = dev_open(p_dev, &nim_cfg);

  MT_ASSERT(SUCCESS == ret);

  dev_io_ctrl(p_dev, NIM_IOCTRL_SET_TUNER_WAKEUP, 0);
#endif  

#endif


  spiflash_cfg( );

  charsto_set_protect(PRT_LOWER_7_8);
  
  /* init data manager */
  dm_para.flash_base_addr = get_flash_addr();
  dm_para.max_blk_num = DM_MAX_BLOCK_NUM;
  dm_para.task_prio = MDL_DM_MONITOR_TASK_PRIORITY;
  dm_para.task_stack_size = MDL_DM_MONITOR_TASK_STKSIZE;
  dm_para.open_monitor = TRUE;
  dm_para.test_mode = FALSE;
  dm_para.para_size = sizeof(dm_v2_init_para_t);


  dm_init_v2(&dm_para);
  OS_PRINTF("[OTA]set header [0x%x]\n",DM_BOOTER_START_ADDR);
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), DM_BOOTER_START_ADDR);

  /* open uio */
  uio_init();

    /* HDMI */
  hdmi_driver_attach((void *)p_public_drvsvc);

{
  *(volatile unsigned long *)0xbf156014 = 0xa0000;
#ifdef DTMB_PROJECT
  *(volatile unsigned long *)0xbf15601c = 0x111100;
  *(volatile unsigned long *)0xbf156308 = 0xfff80000;
#else
  *(volatile unsigned long *)0xbf15601c = 0x1111a5;
  *(volatile unsigned long *)0xbf156308 = 0xfcf80000;
#endif

    /*DMX PTI*/
#if 1
    void *p_pti0 = NULL;
    void *p_pti1 = NULL;
    dmx_config_t dmx_cfg0 = {0};
    dmx_config_t dmx_cfg1 = {0};
    /*************PTI0 attatch************/
    dmx_concerto_attach("concerto_pti0");
    p_pti0 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_pti0");
    MT_ASSERT(NULL != p_pti0);
    
#ifdef DTMB_PROJECT
    dmx_cfg0.input_port_used[1] = TRUE;
    dmx_cfg0.ts_input_cfg[1].input_way = 0;
    dmx_cfg0.ts_input_cfg[1].local_sel_edge = 1;
    dmx_cfg0.ts_input_cfg[1].error_indicator = 0;
    dmx_cfg0.ts_input_cfg[1].start_byte_mask = 0;
#else
    dmx_cfg0.input_port_used[0] = TRUE;
    dmx_cfg0.ts_input_cfg[0].input_way = 1;
    dmx_cfg0.ts_input_cfg[0].local_sel_edge = 1;
    dmx_cfg0.ts_input_cfg[0].error_indicator = 0;
    dmx_cfg0.ts_input_cfg[0].start_byte_mask = 0;
#endif
#ifdef AV_SYNC_FLAG_ON
	 dmx_cfg0.av_sync = TRUE;
#else
	 dmx_cfg0.av_sync = FALSE;
#endif
      dmx_cfg0.pool_size = 512*188;
      dmx_cfg0.pool_mode = 1;
      dmx_cfg0.pti_id = 0;
      ret = dev_open(p_pti0, &dmx_cfg0);
      MT_ASSERT(SUCCESS == ret);
      /*************PTI1 attatch************/
      dmx_concerto_attach("concerto_pti1");
      p_pti1 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_pti1");
      MT_ASSERT(NULL != p_pti1);

      dmx_cfg1.input_port_used[0] = TRUE;
      dmx_cfg1.ts_input_cfg[0].input_way = 1;
      dmx_cfg1.ts_input_cfg[0].local_sel_edge = 1;
      dmx_cfg1.ts_input_cfg[0].error_indicator = 0;
      dmx_cfg1.ts_input_cfg[0].start_byte_mask = 0;

#ifdef AV_SYNC_FLAG_ON
     dmx_cfg1.av_sync = TRUE;
#else
     dmx_cfg1.av_sync = FALSE;
#endif

    dmx_cfg1.pool_size = 512*188;
    dmx_cfg1.pool_mode = 1;
    dmx_cfg1.pti_id = 1;
    ret = dev_open(p_pti1, &dmx_cfg1);
    MT_ASSERT(SUCCESS == ret);
#endif
  }


    /*disp*/
    void *p_disp = NULL;
    disp_cfg_t disp_cfg = {0};
    layer_cfg_t osd0_vs_cfg = {0};
    layer_cfg_t osd1_vs_cfg = {0};
    layer_cfg_t sub_cfg = {0};
    rect_size_t disp_rect;
    //layer_cfg_t sub_vs_cfg = {0};
    //display driver attach
    ret = disp_concerto_attach("disp_concerto");
    MT_ASSERT(ret == SUCCESS);
    p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    MT_ASSERT(NULL != p_disp);
    //adjust these in future

    OS_PRINTF("\ndisp_concerto_attach ok\n");
    disp_cfg.p_sub_cfg = &sub_cfg;
    disp_cfg.p_osd0_cfg = &osd0_vs_cfg;
    disp_cfg.p_osd1_cfg = &osd1_vs_cfg;
    disp_cfg.p_still_sd_cfg = NULL;

    disp_cfg.b_osd_vscle = TRUE;
    disp_cfg.b_osd_hscle = TRUE;
    disp_cfg.b_di_en = TRUE;
    disp_cfg.b_vscale = FALSE;
    disp_cfg.misc_buf_cfg.sd_wb_addr = VID_SD_WR_BACK_ADDR;
    disp_cfg.misc_buf_cfg.sd_wb_size = VID_SD_WR_BACK_SIZE;
    disp_cfg.misc_buf_cfg.sd_wb_field_no = VID_SD_WR_BACK_FIELD_NO;
    memset((void *)(VID_SD_WR_BACK_ADDR | 0xa0000000), 0, VID_SD_WR_BACK_SIZE);

    if(TRUE == disp_cfg.b_di_en)
    {
        disp_cfg.misc_buf_cfg.di_addr = VID_DI_CFG_ADDR;
        disp_cfg.misc_buf_cfg.di_size = VID_DI_CFG_SIZE;
    }
    disp_cfg.stack_size = DISP_HDMI_NOTIFY_TASK_STK_SIZE;
    disp_cfg.task_prio = DISP_HDMI_NOTIFY_TASK_PRIORITY;

    OS_PRINTF("\ndisp_open begin\n");

    if(init_display_cfg(&disp_cfg))
    {
    	ret = dev_open(p_disp, &disp_cfg);
    }
    else
    {
    ret = dev_open(p_disp, NULL);
    }
    MT_ASSERT(SUCCESS == ret);

    OS_PRINTF("\ndisp_open ok\n");
    
    //minnan add cfg vdac
    disp_vdac_config_ota(p_disp);
    
    /*
    disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
    disp_component_set_type(p_disp, COMPONENT_GRP1,COLOR_YUV);
    disp_component_onoff(p_disp, COMPONENT_GRP1, TRUE);
    */
    
    disp_rect.w = 1280;
    disp_rect.h = 720;
    disp_set_graphic_size(p_disp,&disp_rect);
    OS_PRINTF("disp_set_graphic_size w %d h %d\n",disp_rect.w,disp_rect.h);
    disp_layer_show(p_disp, DISP_LAYER_ID_VIDEO_SD, FALSE);
    //disp_layer_show(p_disp, DISP_LAYER_ID_VIDEO_HD, FALSE);

    {
      hdmi_video_config_t hdmi_vcfg;

      disp_set_hdmi(&hdmi_vcfg, VID_SYS_1080I_50HZ);
      hdmi_video_config(dev_find_identifier(NULL,
                              DEV_IDT_TYPE,
                              SYS_DEV_TYPE_HDMI),
                              &hdmi_vcfg);
    }

}

 void middleware_init(void)
{
  eva_init_para_t eva_para = {0};
  mdl_init();
  eva_para.debug_level = E_PRINT_ERROR;
  //eva_para.eva_sys_task_prio = EVA_SYS_TASK_PRIORITY;
  eva_para.eva_sys_task_prio_start = EVA_SYS_TASK_PRIORITY_START;
  eva_para.eva_sys_task_prio_end = EVA_SYS_TASK_PRIORITY_END;
  eva_init(&eva_para);

  eva_filter_factory_init();
  eva_register_filter(DEMUX_FILTER, dmx_filter_create);
  eva_register_filter(OTA_FILTER, ota_filter_create);
  eva_register_filter(FLASH_SINK_FILTER,flash_sink_filter_create);

  {
    nc_svc_cfg_t nc_cfg = {0};
    nc_cfg.b_sync_lock = FALSE;
#ifdef DTMB_PROJECT
    nc_cfg.lock_mode = SYS_DTMB;
#else
    nc_cfg.lock_mode = SYS_DVBC;
#endif
    nc_cfg.priority = MDL_NIM_CTRL_TASK_PRIORITY;
    nc_cfg.stack_size = MDL_NIM_CTRL_TASK_STKSIZE;
    nc_cfg.auto_iq = TRUE;
    nc_svc_init(&nc_cfg, NULL);

  }

}

#define INIT_APP_INFO(info, id, name, instance, task_prio, task_stack) \
  { \
    info.app_id = id; \
    info.p_name = name; \
    info.p_instance = instance; \
    info.priority = task_prio; \
    info.stack_size = task_stack; \
  }

extern ap_frm_policy_t *construct_ap_frm_policy(void);
extern ap_uio_policy_t *construct_ap_uio_policy(void);

//extern ota_policy_t *construct_ota_policy(void);

 void app_init(void)
{
  ap_frm_init_param_t param = {0};
  handle_t ap_frm_handle = NULL;
  u32 ap_frm_msgq_id = 0;

  app_info_t app_array[] =
  {
  //  {APP_OTA, construct_ap_ota(construct_ota_policy()), "app ota", AP_OTA_TASK_PRIORITY, AP_OTA_TASK_STKSIZE},
    {APP_OTA, construct_virtual_nim_filter(APP_OTA,0), "app ota", AP_OTA_TASK_PRIORITY, AP_OTA_TASK_STKSIZE},
    {APP_UIO, construct_ap_uio(construct_ap_uio_policy()), "app uio", AP_UIO_TASK_PRIORITY, AP_UIO_TASK_STKSIZE},
  };

  param.p_implement_policy = construct_ap_frm_policy();
  param.p_app_array = app_array;
  param.num_of_apps = sizeof(app_array)/ sizeof(app_info_t);
  param.stack_size = AP_FRM_TASK_STKSIZE;
  param.priority = AP_FRM_TASK_PRIORITY;
  param.ui_priority = UI_FRM_TASK_PRIORITY;
  param.ap_highest_priority = AP_HIGHEST_TASK_PRIORITY;
  //param.print_level = AP_FRM_PRINT_ALL;
  ap_frm_handle = ap_frm_init(&param, &ap_frm_msgq_id);

  mdl_set_msgq(class_get_handle_by_id(MDL_CLASS_ID), ap_frm_msgq_id);
}

 void util_init(void)
{
  simple_queue_init();
}

extern void ui_init(void);

void ap_init(void)
{

  mem_cfg(MEMCFG_T_NORMAL);
  OS_PRINTF("\r\n[OTA] mem cfg ok ");

  util_init();

  drv_init_concerto();
  middleware_init();

  app_init();
  OS_PRINTF("\r\n[OTA] app init ok ");

  ui_init();
}

