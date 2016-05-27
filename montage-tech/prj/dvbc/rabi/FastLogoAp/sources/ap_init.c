#include <string.h>
#include "sys_types.h"
#include "../../includes/sys_devs.h"
#include "sys_define.h"
#include "sys_regs_concerto.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_event.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
#include "sys_cfg.h"

// driver
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_misc.h"
#include "hal_uart.h"
#include "hal.h"
#include "ipc.h"
#include "common.h"
#include "drv_dev.h"
#include "drv_misc.h"
#include "drv_svc.h"
#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "class_factory.h"
#include "data_manager.h"
#include "data_manager_v2.h"
#include "fcrc.h"
#include "mtos_misc.h"
#include "hal_base.h"
#include "mem_cfg.h"
#include "mem_manager.h"
#include "lib_rect.h"
#include "common.h"
#include "display.h"
#include "gpe_vsb.h"
#include "hdmi.h"
#include "driver.h"
#include "region.h"
#include "lib_memp.h"
#include "hal_gpio.h"
#include "uio.h"
#include "pdec.h"
#include "mdl.h"
#include "nim.h"
#include "dmx.h"
#include "service.h"
#include "nim_ctrl_svc.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "lib_util.h"
#include "ss_ctrl.h"
#include "drv_svc.h"
#include "../../includes/customer_config.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "scart.h"
#include "rf.h"
#include "avctrl1.h"
#include "fast_logo.h"
#include "fastlogo_display.h"
#include "ap_framework.h"
#include "ap_uio.h"
#include "lpower.h"
#include "showlogo.h"
#include "fcrc.h"

#include "ota_public_def.h"
#include "ota_dm_api.h"
#include "spi.h"
#include "LzmaIf.h"
#include "hal_watchdog.h"
#include "showlogo.h"

#define LOGO_SIZE           (1280 * 720 * 4 + 0x100000)
#define STILL_BUFF_ADDR     (GUI_RESOURCE_BUFFER_ADDR -  LOGO_SIZE)
#define PDEC_OUT_ADDR       (0x80000000 | STILL_BUFF_ADDR)
#define HW_STILL_BUFF_ADDR  (STILL_BUFF_ADDR/8)

#define FP_TABLE_SIZE sizeof(fp_bitmap)/sizeof(led_bitmap_t)
#define DM_ADDRESS_OFFSET   (0x10000)
#define MAINCODE_SIZE	(0x500000)
#define MAINCODE_OUT_ZIE (0xF00000)
#define MAINCODE_RUN_ADDR	 (0x80008000)
#define UBOOT_ENV_ADDR  (0xa0000)
#define UBOOT_ENV_SIZE   (0x10000)
#define JUMP(dest) {__asm__ __volatile__ ("j %0" : : "d" (dest));  __asm__("nop");}

static dm_dmh_info_t dmh_info;
 #if 0
static u8 *g_p_logo_buf = NULL;
static u32 g_logo_size = 0;
static s32 load_logo(void);
#endif
//static void show_iframe(u8 * p_input, u32 input_size);


#define START_LOGO_BLOCK_ID          0x93

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

static RET_CODE charsto_set_protect(spi_prot_block_type_t p_t)
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

static RET_CODE charsto_cancel_protect(void)
{
  RET_CODE ret  = SUCCESS;
  charsto_prot_status_t st = {0};
  void  * p_char_dev = (struct charsto_device *)dev_find_identifier(NULL, DEV_IDT_TYPE, 
                                                              SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(p_char_dev != NULL);

  //unprotect
  st.prt_t = PRT_UNPROT_ALL;
  ret  = dev_io_ctrl(p_char_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st);
  
  return ret;
}

static void show_startup_iFram()
{
  RET_CODE ret = SUCCESS;

  void * p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_VDEC_VSB);
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_DISPLAY);

  u32 size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID),START_LOGO_BLOCK_ID);
  u8 *addr = (u8 *)(dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), START_LOGO_BLOCK_ID));

  OS_PRINTF("$$$$$$$$$$ %s,%d,addr==%0x,size==%d\n\n",__FUNCTION__,__LINE__,addr,size);

  dmx_av_reset(dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI));

  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);
  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_HD, FALSE);

  vdec_set_data_input(p_video_dev,1);
  ret = vdec_start(p_video_dev, VIDEO_MPEG, VID_UNBLANK_STABLE);
  ret = vdec_dec_one_frame(p_video_dev, (u8 *)addr, size);
  MT_ASSERT(SUCCESS == ret);

  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, TRUE);
  disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_HD, TRUE);
  vdec_set_data_input(p_video_dev,0);

}

#if 0
static void uio_init(void)
{
  RET_CODE ret;
  hw_cfg_t hw_cfg = {0};
  void *p_dev = NULL;
  uio_cfg_t uiocfg = {0};
  u32 read_len = 0;
  fp_cfg_t fpcfg = {0};
  irda_cfg_t irdacfg = {0};
#ifndef WIN32
  u32 reg= 0 , val = 0;
  void *p_dev_i2cfp = NULL;
#endif

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
  //dev_io_ctrl(p_dev, UIO_FP_SET_POWER_LBD, TRUE);
}
#endif

scart_v_aspect_t get_aspect_mode(u8 focus)
{
  scart_v_aspect_t mode = SCART_ASPECT_16_9;
  switch(focus)
    {
      case 0:
      case 1:
      case 2:
        mode = SCART_ASPECT_4_3;
        break;
      default :
        break;
    }
  return mode;
}

typedef unsigned long (*p_entry)(int, char * const[]);
extern unsigned long _do_go_exec (unsigned long (*entry)(int, char * const []), int argc, char * const argv[]);


#define BARCODE_RUN_ADDR	 (0x81500000)
#define BARCODE_SIZE	(0x80000)
#define BARCODE_OUT_ZIE (0x80000)

static void jump_barcode_app(u8 block_id)
{
  u8 *p_data_buffer = NULL;
  dmh_block_info_t block_dm_info = {0};
  u8 *p_data_buffer2 = NULL;
  u8 *p_zip_sys = NULL;
  u32 out_size = 0;


  dm_get_block_header(class_get_handle_by_id(DM_CLASS_ID),block_id,&block_dm_info) ;
  OS_PRINTF("block_id %d size 0x%x \n",block_id,block_dm_info.size);

  //dm_get_block_header(class_get_handle_by_id(DM_CLASS_ID),0x86,&block_dm_info) ;
  //OS_PRINTF("%s size 0x%x \n",__FUNCTION__,block_dm_info.size);

  p_data_buffer = (u8 *)mtos_malloc(BARCODE_SIZE);
  if(p_data_buffer == NULL)
  {
  	OS_PRINTF("p_data_buffer malloc fail \n");
	return;
  }
  memset(p_data_buffer,0,BARCODE_SIZE);
  p_zip_sys = (u8 *)mtos_malloc(320 * KBYTES);
  if(p_zip_sys == NULL)
  {
  	OS_PRINTF("p_zip_sys malloc fail \n");
	return;
  }

  charsto_cancel_protect();
  
  memset(p_zip_sys,0,320 * KBYTES);
  p_data_buffer2 = (u8 *)(BARCODE_RUN_ADDR);
  memset(p_data_buffer2,0,BARCODE_SIZE);
  out_size = BARCODE_OUT_ZIE;

  dm_read(class_get_handle_by_id(DM_CLASS_ID), block_id, 0, 0, block_dm_info.size, p_data_buffer);

  OS_PRINTF("\nblock_id :%d data :\n",block_id);

  init_fake_mem_lzma(p_zip_sys, 320 * KBYTES);
  lzma_decompress((void *)p_data_buffer2, &out_size, p_data_buffer, block_dm_info.size);
  OS_PRINTF("\nblock_id  decompres data :\n");
  OS_PRINTF("0x%x 0x%x 0x%x 0x%x 0x%x \n\n",p_data_buffer2[0],p_data_buffer2[1],p_data_buffer2[2],p_data_buffer2[3],p_data_buffer2[4]);

  hal_dcache_flush((void *)p_data_buffer2, out_size);

  _do_go_exec((p_entry)BARCODE_RUN_ADDR,0,0);

}
#define   SN_CLIRNT_SIG   "JASNJASNJASNJASN"
#define   SN_CLIRNT_RES   0xA5
#define   SN_TIMEOUT      3
static RET_CODE barcode_client_process(void)
{
  u32 ret = 0, i = 0;
  u8 ch = 0;
  #if 0
  u16 code = 0xFFFF;
  u8 hkey = 0xFF;
  void * p_dev = dev_find_identifier(NULL,
                            DEV_IDT_TYPE,
                            SYS_DEV_TYPE_UIO);
  dm_read(class_get_handle_by_id(DM_CLASS_ID), FPKEY_BLOCK_ID, 0, 5, sizeof(u8), &hkey);
  #endif
  
  uart_flush(0);
  mtos_printk(SN_CLIRNT_SIG);

  for(i = 0; i < SN_TIMEOUT; i++)
  {
    #if 0
    uio_get_code(p_dev, &code);
    if(hkey != 0xFF)
    {
      if((code & 0x100) && (hkey == (code & 0xFF)))
      {
        jump_to_usbupgrade();
        return ERR_FAILURE;
      }
    }
    #endif
    ret = uart_read_byte(0, &ch, 100);
    if((ret == 0) && (ch == SN_CLIRNT_RES))
    {
      jump_barcode_app(UPG_TOOL_BLOCK_ID);
      return SUCCESS;
    }
  }
  return ERR_FAILURE;
}


void jump_app(u8 block_id)
{
  u8 *p_data_buffer = NULL;
  dmh_block_info_t block_dm_info = {0};
  u8 *p_data_buffer2 = NULL;
  u8 *p_zip_sys = NULL;
  u32 out_size = 0;


  dm_get_block_header(class_get_handle_by_id(DM_CLASS_ID),block_id,&block_dm_info) ;
  OS_PRINTF("block_id %d size 0x%x \n",block_id,block_dm_info.size);

  //dm_get_block_header(class_get_handle_by_id(DM_CLASS_ID),0x86,&block_dm_info) ;
  //OS_PRINTF("%s size 0x%x \n",__FUNCTION__,block_dm_info.size);

  p_data_buffer = (u8 *)mtos_malloc(MAINCODE_SIZE);
  if(p_data_buffer == NULL)
  {
  	OS_PRINTF("p_data_buffer malloc fail \n");
	return;
  }
  memset(p_data_buffer,0,MAINCODE_SIZE);
  p_zip_sys = (u8 *)mtos_malloc(320 * KBYTES);
  if(p_zip_sys == NULL)
  {
  	OS_PRINTF("p_zip_sys malloc fail \n");
	return;
  }
  memset(p_zip_sys,0,320 * KBYTES);
  p_data_buffer2 = (u8 *)(MAINCODE_RUN_ADDR);
  memset(p_data_buffer2,0,MAINCODE_SIZE);
  out_size = MAINCODE_OUT_ZIE;

  dm_read(class_get_handle_by_id(DM_CLASS_ID), block_id, 0, 0, block_dm_info.size, p_data_buffer);

  OS_PRINTF("\nblock_id :%d data :\n",block_id);
  OS_PRINTF("0x%x 0x%x 0x%x 0x%x 0x%x \n\n",p_data_buffer[0],p_data_buffer[1],p_data_buffer[2],p_data_buffer[3],p_data_buffer[4]);

  if((p_data_buffer[0] == 0x5d) && (p_data_buffer[1] == 0x00) && (p_data_buffer[2] == 0x00)  && (p_data_buffer[3] == 0x80) )
  {
    init_fake_mem_lzma(p_zip_sys, 320 * KBYTES);
    lzma_decompress((void *)p_data_buffer2, &out_size, p_data_buffer, block_dm_info.size);
    OS_PRINTF("\nblock_id  decompres data :\n");
    OS_PRINTF("0x%x 0x%x 0x%x 0x%x 0x%x \n\n",p_data_buffer2[0],p_data_buffer2[1],p_data_buffer2[2],p_data_buffer2[3],p_data_buffer2[4]);
  }
  else
  {
    memcpy((void *)p_data_buffer2,(void *)p_data_buffer,block_dm_info.size);
  }

  hal_dcache_flush((void *)p_data_buffer2, out_size);

  _do_go_exec((p_entry)MAINCODE_RUN_ADDR,0,0);
  //JUMP(MAINCODE_RUN_ADDR);

}
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
    charsto_cfg.size = CHARSTO_SIZE;
    ret = dev_open(p_dev, &charsto_cfg);
    MT_ASSERT(SUCCESS == ret);
}
#if 1
static BOOL init_display_cfg(disp_cfg_t *p_cfg)
{
  u32 p_addr = 0;

  //init sub buffer
  p_addr = SUB_BUFFER_ADDR;
  if(SUB_BUFFER_SIZE == 0)
    p_addr = 0;

  p_cfg->p_sub_cfg->odd_mem_start = p_addr;
  p_cfg->p_sub_cfg->odd_mem_end = p_addr + SUB_BUFFER_SIZE;
  p_cfg->p_sub_cfg->even_mem_start = 0;
  p_cfg->p_sub_cfg->even_mem_end = 0;

  // init osd0 frame buffer
  p_addr = OSD1_BUFFER_ADDR;
  if(OSD1_BUFFER_SIZE == 0)
    p_addr = 0;

  p_cfg->p_osd1_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd1_cfg->odd_mem_end = p_addr + OSD1_BUFFER_SIZE;
  p_cfg->p_osd1_cfg->even_mem_start = 0;
  p_cfg->p_osd1_cfg->even_mem_end = 0;

  // init osd1 frame buffer
  p_addr = OSD0_BUFFER_ADDR;
  if(OSD0_BUFFER_SIZE == 0)
    p_addr = 0;

  p_cfg->p_osd0_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd0_cfg->odd_mem_end = p_addr + OSD0_BUFFER_SIZE;
  p_cfg->p_osd0_cfg->even_mem_start = 0;
  p_cfg->p_osd0_cfg->even_mem_end = 0;

  p_cfg->lock_type = OS_MUTEX_LOCK;
  return TRUE;
}
#endif

#if 0
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
#endif

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

  ret = dev_open(p_hdmi_dev, &hdmi_cfg);
  MT_ASSERT(SUCCESS == ret);
}


static const drvsvc_handle_t* drv_public_svr_init(void)
{
  drvsvc_handle_t *p_public_drvsvc = NULL;
  u32 *p_buf = NULL;

  //this service will be shared by HDMI, Audio and Display driver
  p_buf = (u32 *)mtos_malloc(DRV_HDMI_TASK_STKSIZE);
  MT_ASSERT(NULL != p_buf);
  p_public_drvsvc = drvsvc_create(DRV_HDMI_TASK_PRIORITY,
    p_buf, DRV_HDMI_TASK_STKSIZE, 8);
  MT_ASSERT(NULL != p_public_drvsvc);

  return p_public_drvsvc;
}

static int drv_dm_init(void)
{
  dm_v2_init_para_t dm_para = {0};
  u32 dm_app_addr = 0;
  RET_CODE ret;

  ret = ATTACH_DRIVER(CHARSTO, concerto, default, default);
  MT_ASSERT(ret == SUCCESS);

  spiflash_cfg( );

  charsto_set_protect(PRT_LOWER_7_8);
  
  //init data manager....
  dm_para.flash_base_addr = get_flash_addr();
  dm_para.max_blk_num = DM_MAX_BLOCK_NUM;
  dm_para.task_prio = MDL_DM_MONITOR_TASK_PRIORITY;
  OS_PRINTF("%s %d dm_para.task_prio = %d\n",__FUNCTION__,__LINE__, dm_para.task_prio);
  dm_para.task_stack_size = MDL_DM_MONITOR_TASK_STKSIZE;
  dm_para.open_monitor = TRUE;
  dm_para.para_size = sizeof(dm_v2_init_para_t);
  dm_para.use_mutex = TRUE;
  dm_para.mutex_prio = 1;
  dm_para.test_mode = FALSE;
  OS_PRINTK("dm_para.flash_base_addr  :0x%x \n",dm_para.flash_base_addr );

  dm_init_v2(&dm_para);
  OS_PRINTF("set header [0x%08x]\n",DM_BOOTER_START_ADDR);
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), DM_BOOTER_START_ADDR);
  OS_PRINTF("set header [0x%08x]\n",DM_HDR_START_ADDR);
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), DM_HDR_START_ADDR);
  dm_app_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), DM_APP_BLOCK_ID)
                           - get_flash_addr();
  OS_PRINTF("set header [0x%08x]\n",dm_app_addr);
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID),dm_app_addr);

  //load customer hw config
  //dm_load_customer_hw_cfg();
  return SUCCESS;
}


static int drv_i2c_init(void)
{
  RET_CODE ret = ERR_FAILURE;
  i2c_cfg_t i2c_cfg = {0};
  void *p_dev_i2c0 = NULL;
  void *p_dev_i2c1 = NULL;
  void *p_dev_i2cfp = NULL;
  void *p_dev_i2chdmi = NULL;
  void *p_dev_i2cqam = NULL;

  ret = i2c_concerto_attach("i2c0");
  MT_ASSERT(SUCCESS == ret);

  p_dev_i2c0 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c0");
  MT_ASSERT(NULL != p_dev_i2c0);
  memset(&i2c_cfg, 0, sizeof(i2c_cfg));
  i2c_cfg.i2c_id = 0;
  i2c_cfg.bus_clk_khz = 100;
  i2c_cfg.lock_mode = OS_MUTEX_LOCK;
  ret = dev_open(p_dev_i2c0, &i2c_cfg);
  MT_ASSERT(SUCCESS == ret);
  /* I2C 1 */
  ret = i2c_concerto_attach("i2c1");
  MT_ASSERT(SUCCESS == ret);

  p_dev_i2c1 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c1");
  MT_ASSERT(NULL != p_dev_i2c1);
  memset(&i2c_cfg, 0, sizeof(i2c_cfg));
  i2c_cfg.i2c_id = 1;
  i2c_cfg.bus_clk_khz = 100;
  i2c_cfg.lock_mode = OS_MUTEX_LOCK;
  ret = dev_open(p_dev_i2c1, &i2c_cfg);
  MT_ASSERT(SUCCESS == ret);
  /* I2C FP */
  ret = i2c_concerto_attach("i2c_FP");
  MT_ASSERT(SUCCESS == ret);

  p_dev_i2cfp = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_FP");
  MT_ASSERT(NULL != p_dev_i2cfp);
  memset(&i2c_cfg, 0, sizeof(i2c_cfg));
  i2c_cfg.i2c_id = 2;
  i2c_cfg.bus_clk_khz = 100;
  i2c_cfg.lock_mode = OS_MUTEX_LOCK;
  ret = dev_open(p_dev_i2cfp, &i2c_cfg);
  MT_ASSERT(SUCCESS == ret);
  /* I2C HDMI */
  ret = i2c_concerto_attach("i2c_HDMI");
  MT_ASSERT(SUCCESS == ret);

  p_dev_i2chdmi = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_HDMI");
  MT_ASSERT(NULL != p_dev_i2chdmi);
  memset(&i2c_cfg, 0, sizeof(i2c_cfg));
  i2c_cfg.i2c_id = 3;
  i2c_cfg.bus_clk_khz = 100;
  i2c_cfg.lock_mode = OS_MUTEX_LOCK;
  ret = dev_open(p_dev_i2chdmi, &i2c_cfg);
  MT_ASSERT(SUCCESS == ret);
  /* I2C QAM */
  ret = i2c_concerto_attach("i2c_QAM");
  MT_ASSERT(SUCCESS == ret);

  p_dev_i2cqam = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_QAM");
  MT_ASSERT(NULL != p_dev_i2cqam);
  memset(&i2c_cfg, 0, sizeof(i2c_cfg));
  i2c_cfg.i2c_id = 4;
  i2c_cfg.bus_clk_khz = 400;
  i2c_cfg.lock_mode = OS_MUTEX_LOCK;
  ret = dev_open(p_dev_i2cqam, &i2c_cfg);
  MT_ASSERT(SUCCESS == ret);

  return SUCCESS;
}

static int drv_hdmi_init(const drvsvc_handle_t* p_public_drvsvc)
{
  hdmi_driver_attach((void *)p_public_drvsvc);

  return SUCCESS;
}

#define AV_SYNC_FLAG_ON

static int drv_dmx_init(void)
{
  RET_CODE ret;
  void *p_pti0 = NULL;
  void *p_pti1 = NULL;
  dmx_config_t dmx_cfg0 = {0};
  dmx_config_t dmx_cfg1 = {0};

  *(volatile unsigned long *)0xbf156014 = 0xa0000;
#ifdef DTMB_PROJECT
  *(volatile unsigned long *)0xbf15601c = 0x111100;
  *(volatile unsigned long *)0xbf156308 = 0xfff80000;
#else
  *(volatile unsigned long *)0xbf15601c = 0x1111a5;
  *(volatile unsigned long *)0xbf156308 = 0xfcf80000;
#endif
  /*************PTI0 attatch************/
  ret = dmx_concerto_attach("concerto_pti0");
  MT_ASSERT(ret == SUCCESS);
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
  dmx_cfg0.pool_mode = 0;
  dmx_cfg0.pti_id = 0;
  ret = dev_open(p_pti0, &dmx_cfg0);
  MT_ASSERT(SUCCESS == ret);
  
  /*************PTI1 attatch************/
  dmx_concerto_attach("concerto_pti1");
  p_pti1 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_pti1");
  MT_ASSERT(NULL != p_pti1);
  
  dmx_cfg1.input_port_used[1] = TRUE;
  dmx_cfg1.ts_input_cfg[1].input_way = 0;
  dmx_cfg1.ts_input_cfg[1].local_sel_edge = 1;
  dmx_cfg1.ts_input_cfg[1].error_indicator = 0;
  dmx_cfg1.ts_input_cfg[1].start_byte_mask = 0;
  
#ifdef AV_SYNC_FLAG_ON
  dmx_cfg1.av_sync = TRUE;
#else
  dmx_cfg1.av_sync = FALSE;
#endif
  
  dmx_cfg1.pool_size = 512*188;
  dmx_cfg1.pool_mode = 0;
  dmx_cfg1.pti_id = 1;
  ret = dev_open(p_pti1, &dmx_cfg1);
  MT_ASSERT(SUCCESS == ret);

  return SUCCESS;
}

static int drv_video_init(void)
{
  RET_CODE ret;
  vdec_cfg_t vdec_cfg = {0};
  void *p_video = NULL;
  u32 size = 0;
  u32 align = 0;

  ret = ATTACH_DRIVER(VDEC, concerto, default, default);
  MT_ASSERT(ret == SUCCESS);

  p_video = (void *)dev_find_identifier(NULL
   , DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
  MT_ASSERT(NULL != p_video);

  vdec_cfg.is_autotest = FALSE;
  ret = dev_open(p_video, &vdec_cfg);
  MT_ASSERT(SUCCESS == ret);
  OS_PRINTF("Init Vdec success\n");

  vdec_stop(p_video);

#ifdef MIN_AV_64M
#ifdef MIN_AV_SDONLY
  vdec_get_buf_requirement(p_video, VDEC_SDINPUT_ONLY, &size, &align);
  OS_PRINTF("VDEC_SDINPUT_ONLY vdec buffer size is : 0x%x\n",size);
  MT_ASSERT(VIDEO_FW_CFG_SIZE >= size);
  OS_PRINTF("VDEC_SDINPUT_ONLY :0x%x\n",VIDEO_FW_CFG_ADDR);
  vdec_set_buf(p_video, VDEC_SDINPUT_ONLY, VIDEO_FW_CFG_ADDR);
#else
  vdec_get_buf_requirement(p_video, VDEC_BUFFER_AD_UNUSEPRESCALE, &size, &align);
  OS_PRINTF("VDEC_BUFFER_AD_UNUSEPRESCALE vdec buffer size is : 0x%x\n",size);
  MT_ASSERT(VIDEO_FW_CFG_SIZE >= size);
  OS_PRINTF("VDEC_BUFFER_AD_UNUSEPRESCALE :0x%x\n",VIDEO_FW_CFG_ADDR);
  vdec_set_buf(p_video, VDEC_BUFFER_AD_UNUSEPRESCALE, VIDEO_FW_CFG_ADDR);
#endif
#else
  vdec_get_buf_requirement(p_video, VDEC_QAULITY_AD_128M, &size, &align);
  OS_PRINTF("VDEC_QAULITY_AD_128M vdec buffer size is : 0x%x\n",size);
  MT_ASSERT(VIDEO_FW_CFG_SIZE >= size);
  OS_PRINTF("VIDEO_FW_CFG_ADDR :0x%x\n",VIDEO_FW_CFG_ADDR);
  vdec_set_buf(p_video, VDEC_QAULITY_AD_128M, VIDEO_FW_CFG_ADDR);
#endif

#ifndef WIN32
  vdec_do_avsync_cmd(p_video,AVSYNC_NO_PAUSE_SYNC_CMD,0);
#endif
  return ret;
}


static int drv_display_init(void)
{
  void *p_disp = NULL;
  disp_cfg_t disp_cfg = {0};
  layer_cfg_t osd0_vs_cfg = {0};
  layer_cfg_t osd1_vs_cfg = {0};
  layer_cfg_t sub_vs_cfg = {0};
  layer_cfg_t osd0_cfg = {0};
  layer_cfg_t osd1_cfg = {0};
  layer_cfg_t sub_cfg = {0};
  rect_size_t disp_rect;
  drv_dev_t *p_hdmi_dev = NULL;
  RET_CODE ret;

  //display driver attach
  ret = ATTACH_DRIVER(DISP, concerto, default, default);
  MT_ASSERT(ret == SUCCESS);
  p_disp = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
  MT_ASSERT(NULL != p_disp);
  //adjust these in future
  
  OS_PRINTF("\ndisp_concerto_attach ok\n");
  disp_cfg.p_sub_cfg = &sub_cfg;
  disp_cfg.p_osd0_cfg = &osd0_cfg;
  disp_cfg.p_osd1_cfg = &osd1_cfg;
  disp_cfg.p_still_sd_cfg = NULL;
  
  disp_cfg.b_osd_vscle = TRUE;
  disp_cfg.b_osd_hscle = TRUE;
  disp_cfg.b_di_en = TRUE;
  disp_cfg.b_vscale = TRUE;
  disp_cfg.b_vscale_osd0 = TRUE;
  disp_cfg.b_vscale_osd1 = TRUE;
  disp_cfg.b_vscale_sub = TRUE;
  #if (defined(MIN_AV_64M))
  disp_cfg.b_wrback_422 = TRUE;
  disp_cfg.b_unuse_prescale = TRUE;
  #else
  disp_cfg.b_wrback_422 = FALSE;
  disp_cfg.b_unuse_prescale = FALSE;
  #endif    
  disp_cfg.misc_buf_cfg.sd_wb_addr = VID_SD_WR_BACK_ADDR;
  disp_cfg.misc_buf_cfg.sd_wb_size = VID_SD_WR_BACK_SIZE;
  disp_cfg.misc_buf_cfg.sd_wb_field_no = VID_SD_WR_BACK_FIELD_NO;
  //memset((void *)(VID_SD_WR_BACK_ADDR | 0xa0000000), 0, VID_SD_WR_BACK_SIZE);
  disp_cfg.av_ap_shared_mem = AP_AV_SHARE_MEM_ADDR;
  disp_cfg.shared_mem_size = AP_AV_SHARE_MEM_SIZE;
  
  if(TRUE == disp_cfg.b_vscale)
  {
      if(disp_cfg.b_vscale_osd1 == TRUE)
      {
          osd1_vs_cfg.odd_mem_start = OSD1_VSCALER_BUF_ADDR;
          osd1_vs_cfg.odd_mem_end = OSD1_VSCALER_BUF_ADDR + OSD1_VSCALER_BUF_SIZE;
      }
      if(disp_cfg.b_vscale_osd0 == TRUE)
      {
          osd0_vs_cfg.odd_mem_start = OSD0_VSCALER_BUF_ADDR;
          osd0_vs_cfg.odd_mem_end = OSD0_VSCALER_BUF_ADDR + OSD0_VSCALER_BUF_SIZE;
      }
      if(disp_cfg.b_vscale_sub == TRUE)
      {
        sub_vs_cfg.odd_mem_start = SUB_VSCALER_BUF_ADDR;
        sub_vs_cfg.odd_mem_end = SUB_VSCALER_BUF_ADDR + SUB_VSCALER_BUF_SIZE;
      }
      disp_cfg.p_osd0_vscale_cfg = &osd0_vs_cfg;
      disp_cfg.p_osd1_vscale_cfg = &osd1_vs_cfg;
      disp_cfg.p_sub_vscale_cfg = &sub_vs_cfg;
    }
  if(TRUE == disp_cfg.b_di_en)
  {
    disp_cfg.misc_buf_cfg.di_addr = VID_DI_CFG_ADDR;
    disp_cfg.misc_buf_cfg.di_size = VID_DI_CFG_SIZE;
  }
  disp_cfg.stack_size = DISP_HDMI_NOTIFY_TASK_STK_SIZE;
  disp_cfg.task_prio = DISP_HDMI_NOTIFY_TASK_PRIORITY;
  disp_cfg.lock_type = OS_MUTEX_LOCK;
  
  disp_cfg.b_uboot_uninit = TRUE;
  
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
  
//  disp_set_tv_sys(p_disp, DISP_CHANNEL_HD, VID_SYS_1080I_50HZ);
//  disp_set_tv_sys(p_disp, DISP_CHANNEL_SD, VID_SYS_PAL);
  p_hdmi_dev = dev_find_identifier(NULL,
                        DEV_IDT_TYPE,
                        SYS_DEV_TYPE_HDMI);
  if(p_hdmi_dev != NULL)
  {
    ret = dev_io_ctrl(p_hdmi_dev, HDMI_START_MAIN_SERVICE, 0);
  }
  if(ret != SUCCESS)
  {
    OS_PRINTF("Can not start main service ret %d\n",ret);
  }
  
    {
      OS_PRINTF("\nVDAC_CVBS_YPBPR_HD\n");
      disp_set_dacmode(p_disp,DISP_DAC_CVBS_YPBPR_HD);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      //disp_component_set_type(p_disp, COMPONENT_GRP1,COLOR_YUV);
      //disp_component_onoff(p_disp, COMPONENT_GRP1, TRUE);
    }
  
  disp_rect.w = 1280;
  disp_rect.h = 720;
  disp_set_graphic_size(p_disp,&disp_rect);
  OS_PRINTK("disp_set_graphic_size w %d h %d\n",disp_rect.w,disp_rect.h);

/*
  disp_set_hdmi(&hdmi_vcfg, VID_SYS_1080I_50HZ);
  hdmi_video_config(dev_find_identifier(NULL,
                          DEV_IDT_TYPE,
                          SYS_DEV_TYPE_HDMI), 
                          &hdmi_vcfg);
*/
  return ret;
}

#if 0
static int drv_gpe_init(void)
{
  RET_CODE ret = ERR_FAILURE;
  void *p_gpe_c = NULL;

  /* GPE */
  OS_PRINTK("GPE_USE_HW\n");
  ret = ATTACH_DRIVER(GPE_VSB, concerto, default, default);
  MT_ASSERT(SUCCESS == ret);

  p_gpe_c = (void *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  ret = dev_open(p_gpe_c, NULL);
  MT_ASSERT(TRUE == ret);

  return SUCCESS;
}

static int drv_jpeg_init(void)
{
  RET_CODE ret = ERR_FAILURE;
  void *p_dev = NULL;

  /*JPEG*/
  ret = ATTACH_DRIVER(PDEC, concerto, default, default);
  MT_ASSERT(ret == SUCCESS);
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);
  MT_ASSERT(NULL != p_dev);

  ret = dev_open(p_dev, NULL);
  MT_ASSERT(SUCCESS == ret);

  return ret;
}
#endif

static void uio_init(void)
{
  RET_CODE ret;
  hw_cfg_t hw_cfg = {0};
  void *p_dev = NULL;
  uio_cfg_t uiocfg = {0};
  u32 read_len = 0;
  fp_cfg_t fpcfg = {0};
  irda_cfg_t irdacfg = {0};
#ifndef WIN32
  u32 reg= 0 , val = 0;
  void *p_dev_i2cfp = NULL;
#endif

  char led_buffer[12] = {0};
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
  uiocfg.p_ircfg = &irdacfg;
  uiocfg.p_fpcfg = &fpcfg;
  ret = dev_open(p_dev, &uiocfg);
  MT_ASSERT(SUCCESS == ret);

  if(hw_cfg.led_num == 4)
  {
    strcpy(led_buffer, "boo.t");
  }
  else if(hw_cfg.led_num == 3)
  {
    strcpy(led_buffer, "ON .");
  }
  else if(hw_cfg.led_num == 2)
  {
    strcpy(led_buffer, "ON .");
  }
  else if(hw_cfg.led_num == 1)
  {
    strcpy(led_buffer, "O  .");
  }
  uio_display(p_dev, led_buffer, strlen(led_buffer));
  //dev_io_ctrl(p_dev, UIO_FP_SET_POWER_LBD, TRUE);
}

extern void hal_otp_init(void);


extern RET_CODE vdec_set_freeze_mem(void *p_dev, u32 addr, u32 size);
void drv_init_concerto(void)
{
  RET_CODE ret;
  const drvsvc_handle_t *p_public_drvsvc;

#ifndef WIN32
  hal_otp_init();
#endif

  /* DMA */
  ret = hal_dma_init();
  MT_ASSERT(ret == SUCCESS);

  p_public_drvsvc = drv_public_svr_init();
  MT_ASSERT(p_public_drvsvc != NULL);

  ret = drv_dm_init();
  MT_ASSERT(ret == SUCCESS);

  ret = drv_i2c_init();
  MT_ASSERT(ret == SUCCESS);
  
  ret = uio_attach_concerto("concerto");
  MT_ASSERT(ret == SUCCESS);
  
  uio_init();
  
  ret = drv_hdmi_init(p_public_drvsvc);
  MT_ASSERT(ret == SUCCESS);

  ret = drv_dmx_init();
  MT_ASSERT(ret == SUCCESS);

  ret = drv_video_init();
  MT_ASSERT(ret == SUCCESS);

  ret = drv_display_init();
  MT_ASSERT(ret == SUCCESS);

  OS_PRINTF("LOGO is NOT picture, assuming IFRAME.\n");
  show_startup_iFram();
 
#if 0
  ret = drv_gpe_init();
  MT_ASSERT(ret == SUCCESS);
  ret = drv_jpeg_init();
  MT_ASSERT(ret == SUCCESS);
#endif


}

BOOL  check_key(void)
{
  return FALSE;
}

static int check_block_exist_from_file_dm(dm_dmh_info_t *p_dmh_info,unsigned char block_id)
{
	unsigned char i = 0,j = 0;
	if(p_dmh_info->header_num > 0)
	{
		for(i = 0; i< p_dmh_info->header_num;i ++)
			{
			   for(j = 0;j < p_dmh_info->header_group[i].block_num ; j ++)
			   	{
			   	   if(p_dmh_info->header_group[i].dm_block[j].id == block_id)
			   	   	{
			   	   	  return TRUE;
			   	   	}
			   	}
			}
	}
  return FALSE;
}

void ota_dm_api_init(void)
{
  ota_dm_config_t p_cfg = {0};
  dm_dmh_info_t *p_all_dmh_info = NULL;
  #if 0
  p_cfg.ota_dm_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID),
                                     STATIC_SAVE_DATA_BLOCK_ID) - get_flash_addr();
  p_cfg.ota_dm_backup_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID),
                                     STATIC_SAVE_DATA_RESERVE_BLOCK_ID) - get_flash_addr();

  OS_PRINTF("#### debug dm addr:0x%x,dm backup addr:0x%x\n",p_cfg.ota_dm_addr,p_cfg.ota_dm_backup_addr);
  #endif
  p_cfg.is_use_mutex = TRUE;
  p_cfg.mutex_prio = 1;
  p_cfg.is_protect = TRUE;
  p_cfg.disable_backup_block = FALSE;
  p_cfg.debug_level = OTA_DM_DEBUG_ALL;
  p_cfg.ota_dm_api_printf = NULL;
  p_cfg.align_size = PICE_MAX_ALIGN_SIZE;
  p_cfg.flash_size = CHARSTO_SIZE;
  p_cfg.pp_dm_info = &p_all_dmh_info;
  p_cfg.ota_dm_block_id = STATIC_SAVE_DATA_BLOCK_ID;
  p_cfg.ota_dm_backup_block_id =STATIC_SAVE_DATA_RESERVE_BLOCK_ID;

  OS_PRINTF("#####debug ui ota dm api  init\n");
  mul_ota_dm_api_init(&p_cfg);

  /****test use ,please don't use in project,but factory upg*****/
 //mul_ota_dm_api_reset_data_block();   
}
void ota_dm_api_bootload_info_init(void)
{
    ota_bl_info_t bl_info = {0};
    
    bl_info.ota_number = 1;
    bl_info.ota_curr_id = OTA_BLOCK_ID;
    bl_info.ota_backup_id = OTA1_BLOCK_ID;
    
    bl_info.load_block_id = MAINCODE_BLOCK_ID;

    bl_info.destroy_flag = FALSE;
    bl_info.ota_status = OTA_TRI_MODE_NONE;
    bl_info.fail_times = 0;
    bl_info.medium_type = OTA_MEDIUM_BY_TUNER;
    mul_ota_dm_api_init_bootload_info(&bl_info);
}
void ota_dm_init(void)
{
  ota_dm_api_init();
  if(mul_ota_dm_api_check_intact_picec(OTA_DM_BLOCK_PIECE_OTA_BLINFO_ID) == FALSE)
  {
    ota_dm_api_bootload_info_init();
  }
  memset(&dmh_info,0,sizeof(dm_dmh_info_t));
  mul_ota_dm_api_find_dmh_info_from_flash(0,4 * MBYTES,&dmh_info);
}

BOOL is_force_key_press()
{
  void *p_dev = NULL;
  u16 code = 0xFFFF;
  u8 hkey = 0xFF;
  u32 i = 4;

  p_dev = dev_find_identifier(NULL,
                            DEV_IDT_TYPE,
                            SYS_DEV_TYPE_UIO);

  //get ok hkey from fp.bin.
  dm_read(class_get_handle_by_id(DM_CLASS_ID), FPKEY_BLOCK_ID, 0, 5, sizeof(u8), &hkey);
  while(i > 1)
  {
    uio_get_code(p_dev, &code);
    OS_PRINTF("is_force_key_press hkey %x , code %x \n", hkey, code);
  //make sure it's fp key.
    if((code & 0x100) && (hkey == (code & 0xFF)))
    {
      return TRUE;
    }
    i --;
    mtos_task_delay_ms(100);
  }

  return FALSE;
}

void load_jump_policy(void)
{
  u8 jump_block_id = MAINCODE_BLOCK_ID;
  ota_bl_info_t bl_info = {0};
  
  mul_ota_dm_api_read_bootload_info(&bl_info);
  OS_PRINTF("**************load info***************\n ");
  OS_PRINTF("ota_number:0x%x,  curr_ota_id:0x%x,  back_ota_id:0x%x,\n",bl_info.ota_number,bl_info.ota_curr_id,bl_info.ota_backup_id);
  OS_PRINTF("load_block_id:0x%x,ota_tri:%d\n", bl_info.load_block_id,bl_info.ota_status);
  OS_PRINTF("destroy flag:0x%x,   destroy_block:0x%x\n",bl_info.destroy_flag,bl_info.destroy_block_id);
  OS_PRINTF("**************load info end***************\n");
  /* flash burning is not finished, force ota*/
  if( bl_info.destroy_flag == TRUE)
  {
    if(bl_info.ota_status != OTA_TRI_MODE_FORC)
    {
     bl_info.ota_status = OTA_TRI_MODE_FORC;
      mul_ota_dm_api_save_bootload_info(&bl_info);
    }
   if(((bl_info.destroy_block_id != OTA_BLOCK_ID)
         || (bl_info.destroy_block_id != OTA1_BLOCK_ID))
       && (bl_info.load_block_id == MAINCODE_BLOCK_ID))
      {
         bl_info.load_block_id = bl_info.ota_curr_id;
         mul_ota_dm_api_save_bootload_info(&bl_info);
      }
    else if(((bl_info.destroy_block_id == OTA_BLOCK_ID))
         && (bl_info.load_block_id != MAINCODE_BLOCK_ID)
         && (bl_info.ota_number < 2))
      {
         bl_info.load_block_id = MAINCODE_BLOCK_ID;
         mul_ota_dm_api_save_bootload_info(&bl_info);
      }
    else if((bl_info.ota_number >= 2)
               && (bl_info.destroy_block_id == bl_info.load_block_id))
      {
        if(bl_info.load_block_id == OTA_BLOCK_ID)
          {
            bl_info.load_block_id = OTA1_BLOCK_ID;
          }
        else
          {
            bl_info.load_block_id = OTA_BLOCK_ID;
          }
          mul_ota_dm_api_save_bootload_info(&bl_info);
      }

    OS_PRINTF("it will forc ota by dstroy flag\n");
  }


  if((bl_info.load_block_id == MAINCODE_BLOCK_ID)
     && (check_block_exist_from_file_dm(&dmh_info,MAINCODE_BLOCK_ID) == TRUE))
  {
     jump_block_id = MAINCODE_BLOCK_ID;
  }
  else
  {
     if((bl_info.load_block_id == OTA_BLOCK_ID)
     && (check_block_exist_from_file_dm(&dmh_info,OTA_BLOCK_ID) == TRUE))
      {
        jump_block_id = OTA_BLOCK_ID;
      }
     else if((bl_info.ota_number >=2)
                && (bl_info.load_block_id == OTA1_BLOCK_ID)
                && (check_block_exist_from_file_dm(&dmh_info,OTA1_BLOCK_ID) == TRUE))
      {
         jump_block_id = OTA1_BLOCK_ID;
      }
     else
      {
        jump_block_id = USB_TOOL_BLOCK_ID;
      }
  }

  if(bl_info.load_block_id != jump_block_id)
  {
     if((jump_block_id == OTA_BLOCK_ID)
        ||(jump_block_id == OTA1_BLOCK_ID))
      {
         if(bl_info.ota_status != OTA_TRI_MODE_FORC)
        {
          mul_ota_dm_api_read_bootload_info(&bl_info);
          bl_info.ota_status = OTA_TRI_MODE_FORC;
          bl_info.load_block_id = jump_block_id;
          if(bl_info.ota_number >=2)
            {
              if(bl_info.load_block_id == OTA_BLOCK_ID)
                {
                 bl_info.ota_curr_id = OTA_BLOCK_ID;
                 bl_info.ota_backup_id = OTA1_BLOCK_ID;
                }
              else
                {
                 bl_info.ota_curr_id = OTA1_BLOCK_ID;
                 bl_info.ota_backup_id = OTA_BLOCK_ID;
                }
            }
          else
            {
              bl_info.ota_curr_id = OTA_BLOCK_ID;
              bl_info.ota_backup_id = OTA_BLOCK_ID;
            }
          
          mul_ota_dm_api_save_bootload_info(&bl_info);
        }
      }
  }
  OS_PRINTF("fast load block id:0x%x\n",jump_block_id);

  if((jump_block_id != OTA_BLOCK_ID) && is_force_key_press())
  {
    jump_block_id = OTA_BLOCK_ID;
    bl_info.ota_status = OTA_TRI_MODE_KEY;
    bl_info.load_block_id = jump_block_id;
    mul_ota_dm_api_save_bootload_info(&bl_info);
  }
  
  jump_app(jump_block_id);
}


#define   SN_CLIRNT_SIG   "JASNJASNJASNJASN"
#define   SN_CLIRNT_RES   0xA5
#define   SN_TIMEOUT      3
#if 0
static void jump_to_usbupgrade()
{
  u8 * env_buffer = NULL;
  charsto_device_t *p_charsto = NULL;
  charsto_prot_status_t st_set = {0};

  env_buffer = mtos_malloc(0x10000);
  if(env_buffer == NULL)
  {
    OS_PRINTF("env_buffer fail \n");
    return;
  }
  p_charsto = (charsto_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, \
                                                      SYS_DEV_TYPE_CHARSTO);
  if(NULL == p_charsto)
  {
    return;
  }
  charsto_read(p_charsto,UBOOT_ENV_ADDR,env_buffer,UBOOT_ENV_SIZE);

  OS_PRINTF("env end  0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n",
    env_buffer[UBOOT_ENV_SIZE - 8],env_buffer[UBOOT_ENV_SIZE - 7],
    env_buffer[UBOOT_ENV_SIZE - 6],
    env_buffer[UBOOT_ENV_SIZE - 5],env_buffer[UBOOT_ENV_SIZE - 4],
    env_buffer[UBOOT_ENV_SIZE - 3],env_buffer[UBOOT_ENV_SIZE - 2],
    env_buffer[UBOOT_ENV_SIZE - 1]);

  env_buffer[UBOOT_ENV_SIZE - 1] = 0;
  env_buffer[UBOOT_ENV_SIZE - 2] = 0;
  env_buffer[UBOOT_ENV_SIZE - 3] = 0;
  env_buffer[UBOOT_ENV_SIZE - 4] = 0;
  env_buffer[UBOOT_ENV_SIZE - 5] = 1;

  st_set.prt_t = PRT_UNPROT_ALL;
  dev_io_ctrl(p_charsto, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);
  charsto_erase(p_charsto, UBOOT_ENV_ADDR,1);
  
  charsto_writeonly(p_charsto, UBOOT_ENV_ADDR, env_buffer, UBOOT_ENV_SIZE);

  mtos_free(env_buffer);

  mtos_task_delay_ms(200);
  hal_watchdog_enable();
  hal_pm_reset();

}
RET_CODE sn_client_process(void)
{
  u32 ret = 0, i = 0;
  u8 ch = 0;
  u16 code = 0xFFFF;
  u8 hkey = 0xFF;
  void * p_dev = dev_find_identifier(NULL,
                            DEV_IDT_TYPE,
                            SYS_DEV_TYPE_UIO);
  dm_read(class_get_handle_by_id(DM_CLASS_ID), FPKEY_BLOCK_ID, 0, 5, sizeof(u8), &hkey);

  uart_flush(0);
  OS_PRINTF(SN_CLIRNT_SIG);
  for(i = 0; i < SN_TIMEOUT; i++)
  {
    uio_get_code(p_dev, &code);
    if(hkey != 0xFF)
    {
      if((code & 0x100) && (hkey == (code & 0xFF)))
      {
        jump_to_usbupgrade();
        return ERR_FAILURE;
      }
    }
    ret = uart_read_byte(0, &ch, 100);
    if((ret == 0) && (ch == SN_CLIRNT_RES))
    {
      return SUCCESS;  
    }
  }
  return ERR_FAILURE;
}
#endif

#define FF_BSS __attribute__((section(".av_bss")))
extern FF_BSS ipc_fw_fun_set_t g_ipcfw_f;
extern u32 attach_ipcfw_fun_set_concerto(ipc_fw_fun_set_t * p_funset);

#ifndef WIN32
static inline void __delay(unsigned int loops)
{
  __asm__ __volatile__ (
  "       .set    noreorder                               \n"
  "       .align  3                                       \n"
  "1:     bnez    %0, 1b                                  \n"
  "       subu    %0, 1                                   \n"
  "       .set    reorder                                 \n"
  : "=r" (loops)
  : "0" (loops));
}
#endif

int ap_init(void)
{
  RET_CODE ret = ERR_FAILURE;
    __delay(0x10000);
  hal_dcache_invalidate((void *)&g_ipcfw_f,sizeof(ipc_fw_fun_set_t));

  attach_ipcfw_fun_set_concerto(&g_ipcfw_f);

  ap_ipc_init(32);   // ap ipc fifo create

  mem_cfg(MEMCFG_T_NORMAL);
  drv_init_concerto();

  OS_PRINTF("$$$ %s,%d\n\n",__FUNCTION__,__LINE__);

  ret = barcode_client_process();
  if(ret == SUCCESS)
  {
      jump_barcode_app(UPG_TOOL_BLOCK_ID);
  }

#if 0
  ret = sn_client_process();
  if(ret == SUCCESS)
  {
      jump_app(UPG_TOOL_BLOCK_ID);
  }
  else
#endif
  {
    #if 0
    drv_dev_t * p_pic_dev;
    pic_info_t pic_info = {0};
    pdec_ins_t picdec_ins = {0};

    load_logo();
    mtos_printk("PDEC_OUT_ADDR 0x%x\n",PDEC_OUT_ADDR);

    p_pic_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);
    MT_ASSERT(p_pic_dev != NULL);
    if (pdec_getinfo(p_pic_dev, g_p_logo_buf, g_logo_size, &pic_info, &picdec_ins) == SUCCESS
        && pic_info.image_format != IMAGE_FORMAT_UNKNOWN)
    {
      OS_PRINTF("LOGO is picture, pic size [%d,%d] ,format %d\n", 
          pic_info.src_width, pic_info.src_height, pic_info.image_format);
      show_logo(PDEC_OUT_ADDR, g_p_logo_buf, g_logo_size, DRAW_ON_OSD);
    }
    else
   #endif

    ota_dm_init();
    load_jump_policy();
  }

  return 0;
}

BOOL get_dm_block_real_file_info(u8 block_id,dmh_block_info_t *block_info)
{
  void *p_dev = NULL;
  u32 block_addr = 0;
  u32 block_size = 0;
  u32 wr_addr = 0;
  dmh_block_info_t real_dm_info = {0};
  RET_CODE ret = ERR_NOFEATURE;
  dmh_block_info_t block_dm_info = {0};
  if(dm_get_block_header(class_get_handle_by_id(DM_CLASS_ID),block_id,&block_dm_info) == DM_FAIL)
  {
    return -1;
  }
  if(block_dm_info.type != BLOCK_TYPE_RO)
  {
    memcpy(block_info,&block_dm_info,sizeof(dmh_block_info_t));
    return 0;
  }
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(NULL != p_dev);
  block_addr = dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), block_id);
  if(block_addr == 0)
    {
       return -1;
    }
  block_addr = block_addr - get_flash_addr();
  block_size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID),block_id);
  wr_addr = block_addr + block_size - sizeof(dmh_block_info_t);
  ret = charsto_read(p_dev,wr_addr,(u8 *)&real_dm_info,sizeof(dmh_block_info_t));
  if((ret == SUCCESS) && (block_id == real_dm_info.id))
    {
        memcpy(block_info,&real_dm_info,sizeof(dmh_block_info_t));
        return 0;
    }
  return -1;
}

u32 get_dm_block_real_file_size(u8 block_id)
{
    dmh_block_info_t real_dm_info = {0};
    BOOL ret = 0;
    ret = get_dm_block_real_file_info(block_id,&real_dm_info);
    if(ret == 0)
    {   
        return real_dm_info.size;
    }
    return 0; 
}


 #if 0
static s32 load_logo(void)
{
  OS_PRINTK("\n###fast logo logo buf :0x%x flash addr 0x%x\n",
    dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), START_LOGO_BLOCK_ID)),get_flash_addr();
  
   g_p_logo_buf = (u8 *)(dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), START_LOGO_BLOCK_ID));
   
  g_logo_size = get_dm_block_real_file_size(START_LOGO_BLOCK_ID);//dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID),START_LOGO_BLOCK_ID);
  
  OS_PRINTK("\n###fast logo logo buf :0x%x,size:0x%x\n",*((u32 *)g_p_logo_buf),g_logo_size);
  return SUCCESS;
}
#endif


