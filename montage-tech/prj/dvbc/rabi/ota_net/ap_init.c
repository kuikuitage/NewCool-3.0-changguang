/******************************************************************************/

/******************************************************************************/

// standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_warriors.h"
//#include "sys_devs.h"
#include "sys_cfg.h"
#include "driver.h"


#include "drv_dev.h"


// os
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"

#include "ipc.h"

//
#include "ethernet.h"
#include "mtos_event.h"


#include "list.h"

#include "block.h"

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

#include "hal.h"


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

#include "lpower.h"
#include "spi.h"
#include "hid.h"
#include "drv_cfg.h"
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

#include "net_svc.h"

#include "vfs.h"

#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"

#include "dvbc_util.h"
#include "ss_ctrl.h"
#include "mt_time.h"

#include "avctrl1.h"
#include "vbi_vsb.h"
#include "surface.h"

#include "db_dvbs.h"
#include "mem_cfg.h"
//eva
#include "interface.h"
#include "eva.h"
#include "media_format_define.h"//
#include "ipin.h"//
#include "ifilter.h"//
#include "chain.h"//
#include "controller.h"//
#include "eva_filter_factory.h"//
#include "common_filter.h"

// ap
#include "ap_framework.h"

#include "ap_multi_pic.h"
#include "ap_uio.h"
#include "ap_playback.h"
#include "ap_scan.h"
#include "ap_signal_monitor.h"
#include "ap_satcodx.h"
//#include "ap_dvbs_ota.h"
#include "ap_upgrade.h"
#include "ap_time.h"
//#include "ap_cas.h"
#include "ap_ota.h"
//#include "ap_twin_port.h"

#include "db_dvbs.h"
#include "sys_status.h"
#include "mem_cfg.h"
#include "pnp_service.h"
#include "network_monitor.h"

#include "net_source_filter.h"
#include "net_upg_filter.h"
#include "flash_sink_filter.h"

#include "net_upg_api.h"

#include "flash_records.h"
#include "commonData.h"

#include "iconv_ext.h"

typedef struct wifi_config
{
  u8 wifi_send_task_prio;
  u8 wifi_recv_task_prio;
  u8 wifi_cmd_task_prio;
  u8 wifi_wpa_supplicant_task_prio;
}wifi_config_t;


char *even_mem = 0;
char *odd_mem = 0;

extern void ui_init(void);
hw_cfg_t hw_cfg;
static u32 is_fastlogo = 0;

extern  u8 mount_ramfs();


extern RET_CODE ethernet_sonata_attach(char *p_name);

//extern RET_CODE ethernet_sonata_attach(char *p_name);
extern RET_CODE nim_m88ds3000_attach(char *name);
extern RET_CODE nim_s2_sa_attach(char *name);
extern RET_CODE nim_m88rs6k_attach(char *name);

extern RET_CODE rtl8188eus_ethernet_attach(char *p_name);
extern RET_CODE spi_ethernet_attach(char *p_name);
extern RET_CODE usb_eth_asix_attach(char *p_name);
extern RET_CODE usb_eth_sr_attach(char *p_name);
#ifndef GPE_USE_HW
extern RET_CODE gpe_soft_attach(char *p_name);
#endif

extern RET_CODE  usb_mass_stor_attach(char *p_name);
extern RET_CODE sdcard_warriors_attach(char *p_name);


extern RET_CODE nandflash_attach_sonata(char *p_name);
extern RET_CODE spibus_attach_sonata(char *p_name); 



//gpio config for shadow, replace the FRONTPANEL module in chip
static const pan_hw_info_t pan_info =
{
  PAN_SCAN_SHADOW,  /*type_scan, */
  0,              /*type_lbd,  0: Stand-alone LBD, 1: LBD in shifter */
  0,              /*type_com, 0: Stand-alone COM, 1: COM in shifter*/
  0,              /*num_scan, Number of scan PIN, 0: no scan; <= 2 */
  4,              /*num_com,  Number of COM PIN, 0: no com; <= 8 */
  8,              /*pos_colon, Position of colon flag, 0 to 7, 8 no colon */
  8,              /*special control byte: bit 0: revert the data sequence or not
                     bit 1: HT1628 special cmd, bit 2: GPIO control lbd or not*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},   /*flatch*/
  {GPIO_LEVEL_HIGH, GPIO_DIR_OUTPUT, 63},  /*fclock*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},   /*fdata*/
  {{GPIO_LEVEL_LOW, GPIO_DIR_INPUT, 63},  /*scan[0]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63}}, /*scan[1]*/
  {{GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[0]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[1]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[2]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[3]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[4]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[5]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*com[6]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63}}, /*com[7]*/
  {{GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 55},  /*lbd[0]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*lbd[1]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63},  /*lbd[2]*/
   {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 63}}, /*lbd[3]*/
  30,       /* com_scan_intv, scan com digital interval in ms, */
  300,     /* repeat_intv, Repeat interval in ms, for rpt key only */
};


static const led_bitmap_t fp_bitmap[] =
{
{'.', 0x80},
{'0', 0x3F},  {'1', 0x06},  {'2', 0x5B},  {'3', 0x4F},
{'4', 0x66},  {'5', 0x6D},  {'6', 0x7D},  {'7', 0x07},
{'8', 0x7F},  {'9', 0x6F},  {'a', 0x77},  {'A', 0x77},
{'b', 0x7C},  {'B', 0x7C},  {'c', 0x39},  {'C', 0x39},
{'d', 0x5E},  {'D', 0x5E},  {'e', 0x79},  {'E', 0x79},
{'f', 0x71},  {'F', 0x71},  {'g', 0x6F},  {'G', 0x3D},
{'h', 0x76},  {'H', 0x76},  {'i', 0x04},  {'I', 0x30},
{'j', 0x0E},  {'J', 0x0E},  {'l', 0x38},  {'L', 0x38},
{'n', 0x54},  {'N', 0x37},  {'o', 0x5C},  {'O', 0x3F},
{'p', 0x73},  {'P', 0x73},  {'q', 0x67},  {'Q', 0x67},
{'r', 0x50},  {'R', 0x77},  {'s', 0x6D},  {'S', 0x6D},
{'t', 0x78},  {'T', 0x31},  {'u', 0x3E},  {'U', 0x3E},
{'y', 0x6E},  {'Y', 0x6E},  {'z', 0x5B},  {'Z', 0x5B},
{':', 0x80},  {'-', 0x40},  {'_', 0x08},  {' ', 0x00},
};
#define FP_TABLE_SIZE sizeof(fp_bitmap)/sizeof(led_bitmap_t)

//gpio map for scart
static const scart_hw_info_t gpio_scart_info =
{
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 9, SCART_VID_CVBS, SCART_VID_RGB},   /*outmode*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 5, SCART_ASPECT_4_3, SCART_ASPECT_16_9},  /*aspect*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 7, SCART_TERM_STB, SCART_TERM_TV},   /*vcr input*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 32, SCART_TERM_STB, SCART_TERM_VCR},   /*tv master*/
  {GPIO_LEVEL_LOW, GPIO_DIR_OUTPUT, 6, VCR_DETECTED, VCR_NOT_DETECTED},   /*vcr dectect*/
};

static void init_fp_cfg(fp_cfg_t *p_cfg)
{
  // use default in drv
  p_cfg->map_size = FP_TABLE_SIZE;
  p_cfg->p_map = fp_bitmap;
  p_cfg->p_info = (pan_hw_info_t *)&pan_info;
  p_cfg->display_off = 0;
}

static BOOL init_display_cfg(disp_cfg_t *p_cfg)
{
  u32 p_addr = 0;
  
  //init sub buffer
  p_addr = mem_mgr_require_block(BLOCK_SUB_32BIT_BUFFER, SYS_MODULE_GDI);
  MT_ASSERT(p_addr != 0);
  mem_mgr_release_block(BLOCK_SUB_32BIT_BUFFER);

  OS_PRINTF("KKK[%d]\n", __LINE__);
	
  p_cfg->p_sub_cfg->odd_mem_start = (p_addr | 0xa0000000) ;
  p_cfg->p_sub_cfg->odd_mem_end = (p_addr | 0xa0000000)  + SUB_32BIT_BUFFER_SIZE;
  p_cfg->p_sub_cfg->even_mem_start = 0;
  p_cfg->p_sub_cfg->even_mem_end = 0;

  OS_PRINTF("KKK[%d]\n", __LINE__);
	
  // init osd0 frame buffer
  p_addr = mem_mgr_require_block(BLOCK_OSD0_32BIT_BUFFER, SYS_MODULE_GDI);
  MT_ASSERT(p_addr != 0);
  mem_mgr_release_block(BLOCK_OSD0_32BIT_BUFFER);


	 OS_PRINTF("KKK[%d]\n", __LINE__);
  p_cfg->p_osd1_cfg->odd_mem_start = (p_addr | 0xa0000000) ;
  p_cfg->p_osd1_cfg->odd_mem_end = (p_addr | 0xa0000000)  + OSD0_32BIT_BUFFER_SIZE;
  p_cfg->p_osd1_cfg->even_mem_start = 0;
  p_cfg->p_osd1_cfg->even_mem_end = 0;

  // init osd1 frame buffer
  p_addr = mem_mgr_require_block(BLOCK_OSD1_32BIT_BUFFER, SYS_MODULE_GDI);
  MT_ASSERT(p_addr != 0);
  mem_mgr_release_block(BLOCK_OSD1_32BIT_BUFFER);
 OS_PRINTF("KKK[%d]\n", __LINE__);
  p_cfg->p_osd0_cfg->odd_mem_start = (p_addr | 0xa0000000) ;
  p_cfg->p_osd0_cfg->odd_mem_end = (p_addr | 0xa0000000)  + OSD1_32BIT_BUFFER_SIZE;
  p_cfg->p_osd0_cfg->even_mem_start = 0;
  p_cfg->p_osd0_cfg->even_mem_end = 0;

  p_cfg->lock_type = OS_MUTEX_LOCK;




  return TRUE;
}

hw_cfg_t hw_cfg;
extern RET_CODE uio_fp_stop_work(uio_device_t *p_dev);

void uio_init()
{
#if 0
  RET_CODE ret;
  void *p_dev = NULL;
  uio_cfg_t uiocfg = {0};
  fp_cfg_t fpcfg = {0};
  irda_cfg_t irdacfg = {0};

  OS_PRINTF("\r\n[OTA]uio attach");
  //uio including irda and frontpanel dev....
  ret = ATTACH_DRIVER(UIO, warriors, default, warriors_mt);
  MT_ASSERT(ret == SUCCESS);

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  MT_ASSERT(NULL != p_dev);

  irdacfg.protocol = IRDA_NEC;
  init_fp_cfg(&fpcfg);
  uiocfg.p_ircfg = &irdacfg;
  uiocfg.p_fpcfg = &fpcfg;
  uiocfg.lock_type = OS_MUTEX_LOCK;
  ret = dev_open(p_dev, &uiocfg);
  MT_ASSERT(SUCCESS == ret);
#else
  RET_CODE ret;
  void *p_dev = NULL;
  fp_cfg_t fpcfg = {0};
  irda_cfg_t irdacfg = {0};
  uio_cfg_t uiocfg = {0};

  OS_PRINTF("uio attach\n");
  //uio including irda and frontpanel dev....
#ifndef WIN32
  ret = ATTACH_DRIVER(UIO, warriors, default, warriors_mt);
#else
  ret = ATTACH_DRIVER(UIO, magic, default, gpio);
#endif
  MT_ASSERT(ret == SUCCESS);

  {
    u32 read_len = 0;
    read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                            HW_CFG_BLOCK_ID, 0, 0,
                            sizeof(hw_cfg_t),
                            (u8 *)&hw_cfg);
    MT_ASSERT(read_len == sizeof(hw_cfg_t));
  }

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  MT_ASSERT(NULL != p_dev);

  //fpcfg.p_info =  &(hw_cfg.pan_info);
  //fpcfg.p_map = hw_cfg.led_bitmap;
  //fpcfg.map_size = 4;
//  fpcfg.fp_type = FD650;
  fpcfg.p_info = &(hw_cfg.pan_info);
  fpcfg.p_map = hw_cfg.led_bitmap;
  fpcfg.map_size = hw_cfg.led_num;
  fpcfg.fp_type = hw_cfg.fp_type;
 // if(fpcfg.fp_type != FP_GPIO)
 // {
 //   fpcfg.p_map = NULL; //650D/1635 use default LED bitmap
//  }

  irdacfg.protocol = IRDA_NEC;
  irdacfg.code_mode = OUR_DEF_MODE;
  irdacfg.irda_repeat_time = 200;
 // init_fp_cfg(&fpcfg);
  uiocfg.p_ircfg = &irdacfg;
  uiocfg.p_fpcfg = &fpcfg;
  uiocfg.lock_type = OS_MUTEX_LOCK;
  ret = dev_open(p_dev, &uiocfg);
  MT_ASSERT(SUCCESS == ret);

  //ui_set_com_num(hw_cfg.pan_info.num_com);
#ifndef WIN32
  uio_fp_stop_work(p_dev);
#endif
#if 0
  if(hw_cfg.pan_info.num_com == 4)
  {
    uio_display(p_dev, " ON ", 4);
  }
  else if(hw_cfg.pan_info.num_com == 3)
  {
    uio_display(p_dev, "ON ", 3);
  }
  else if(hw_cfg.pan_info.num_com == 2)
  {
    uio_display(p_dev, "ON ", 2);
  }
  else if(hw_cfg.pan_info.num_com == 1)
  {
    uio_display(p_dev, "O ", 1);
  }
#endif
  mtos_printk("uio init end\n");

#endif
}

#if 0
static u32 is_hdmi_cfg(void)
{
  #define PARA_ADDRESS   (0xbfef0180)
  volatile u32* boot_para3 = NULL;//HDMI flag
  u8 ret = 0;

  boot_para3 = (volatile u32 *)(PARA_ADDRESS + 0x10);
  OS_PRINTF("*********hdmi config value=%x\n",*boot_para3);
  if(*boot_para3 == (0x12340000 | 0x0001))
  {
    OS_PRINTF("*********hdmi is config\n");
    *boot_para3 = 0x12340000;
    ret = 1;
  }
  return ret;
}
#endif
extern RET_CODE rtl8188eus_ethernet_attach(char *p_name);
extern RET_CODE ethernet_attach(char *p_name);
extern int lwip_init_tcpip(ethernet_device_t *p_dev, ethernet_cfg_t *p_cfg);
extern int lwip_stack_init(ethernet_cfg_t *p_cfg);
#define ETH_RST_PIN 58
void eth_init(void)
{
#if 0//ndef WIN32
  RET_CODE op = SUCCESS;
  BOOL ret = FALSE;
  ethernet_device_t *eth_dev = NULL;
  ethernet_cfg_t      ethcfg = {0};
#endif
#ifndef WIN32
  ethernet_device_t *eth_dev = NULL;
  ethernet_cfg_t      ethcfg = {0};
  /* reset demod chip before attach driver */
  gpio_ioctl(GPIO_CMD_IO_ENABLE, ETH_RST_PIN, TRUE);
  gpio_set_dir(ETH_RST_PIN, GPIO_DIR_OUTPUT);
  gpio_set_value(ETH_RST_PIN, 0);
  mtos_task_delay_ms(30);
  gpio_set_value(ETH_RST_PIN, 1);

  ethernet_attach("mt_eth");
    eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
  MT_ASSERT(eth_dev != NULL);
  dev_open((void *)eth_dev, (void *)&ethcfg);
  ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
  lwip_stack_init(&ethcfg);
  
#endif
#if 0//ndef WIN32
  eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
  MT_ASSERT(eth_dev != NULL);

  ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
  ethcfg.is_enabledhcp = 0;

  ethcfg.ipaddr[0] = 189;
  ethcfg.ipaddr[1] = 8;
  ethcfg.ipaddr[2] = 168;
  ethcfg.ipaddr[3] = 192;

  ethcfg.netmask[0] = 0;
  ethcfg.netmask[1] = 255;
  ethcfg.netmask[2] = 255;
  ethcfg.netmask[3] = 255;

  ethcfg.gateway[0] = 1;
  ethcfg.gateway[1] = 8;
  ethcfg.gateway[2] = 168;
  ethcfg.gateway[3] = 192;

  ethcfg.primaryDNS[0] = 5;
  ethcfg.primaryDNS[1] = 8;
  ethcfg.primaryDNS[2] = 168;
  ethcfg.primaryDNS[3] = 192;


  ethcfg.hwaddr[0] = 0xD8;
  ethcfg.hwaddr[1] = 0xa5;
  ethcfg.hwaddr[2] = 0xb7;
  ethcfg.hwaddr[3] = 0x80;
  ethcfg.hwaddr[4] = 0x01;
  ethcfg.hwaddr[5] = 0x00;

  op = dev_open((void *)eth_dev, (void *)&ethcfg);
  if(SUCCESS == op)
  {
    OS_PRINTF("tf init ethernet ok:\n");
    ret = TRUE;
  }
  else
  {
    OS_PRINTF("tf init ethernet failed:\n");
    ret = FALSE;
  }
  lwip_init_tcpip(eth_dev, &ethcfg);
#endif
}

void wifi_init(void)
{
#ifndef WIN32
  RET_CODE ret = ERR_FAILURE;
  ethernet_cfg_t ethcfg = {0};
  wifi_config_t cfg;

  cfg.wifi_send_task_prio = WIFI_TASK_LET_SEND;
  cfg.wifi_recv_task_prio = WIFI_TASK_LET_RECV;
  cfg.wifi_cmd_task_prio = WIFI_TASK_THREAD;
  cfg.wifi_wpa_supplicant_task_prio = WPA_SUPPLICANT_TASK;
  ethcfg.p_priv_confg = (u8*)&cfg;
  OS_PRINTF("attach wifi\n");
  void *p_dev = NULL;
  rtl8188eus_ethernet_attach("usb_wifi");
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_WIFI);
  MT_ASSERT(p_dev != NULL);
  ret = dev_open(p_dev, (void *)&ethcfg);

  OS_PRINTF("attach wifi success\n");
#endif
}

extern void register_http_stream();
//extern void register_http_stream_is();
//extern void register_rtsp_stream();
//extern void register_rtmp_stream();
//extern void register_rtmp_protocol(int type);


extern u32 get_flash_addr(void);

extern u8 ufs_dev_init();
extern void chip_usb_debug();

extern RET_CODE usb_mass_stor_attach(char *p_name);

static void load_stb_info(void)
{
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);
  u32 stb_info_addr = STB_INFO_ADDR;
  u8 stb_info_buf[48];
  u8 stb_info_str[48];
  u8 tmp[26];
  u8 i = 0;

  charsto_read(p_charsto_dev, stb_info_addr,stb_info_buf, sizeof(stb_info_buf));
  memset(tmp, 0xff, sizeof(tmp));
  if( 0 != memcmp(stb_info_buf, tmp, 3) )
  {
    for(i = 0; i < 3; i++)
    stb_info_str[i] = '0' + stb_info_buf[i];
    stb_info_str[i] = '\0';
    sys_status_set_customer_id(atoi(stb_info_str));
  }

  if( 0 != memcmp(stb_info_buf+3, tmp, 9) )
  {
    for(i = 3; i < 12; i++)
    stb_info_str[i-3] = '0' + stb_info_buf[i];
    stb_info_str[i-3] = '\0';
    sys_status_set_hw_ver(atoi(stb_info_str));
  }

  if( 0 != memcmp(stb_info_buf+12, tmp, 8) )
  {
    for(i=12; i<20; i++)
    stb_info_str[i-12] = '0' + stb_info_buf[i];
    stb_info_str[i-12] = '\0';
    sys_status_set_serial_num(atoi(stb_info_str));
  }

  mtos_printk("###############\n\nmac:    ");
  if( 0 != memcmp(stb_info_buf+20, tmp, 6) )
  {
    for(i=20; i<26;i++)
    {
      u8 hmac[3] = {0};
      mtos_printk("0x%02x:", stb_info_buf[i]);
      sprintf(hmac, "%02x", stb_info_buf[i]);
	 mtos_printk("hmac: %s\n", hmac);
      sys_status_set_mac(i-20, hmac);
    }
//MT_ASSERT(0);
  }
  else
  {
    u32 low, high;
    u64 hight_64;
    u64 chip_id;
    u32 crc;
    u8 hmac[3];
    char hwaddr = 0x0;
    char *pstr;
    char name[3] = {0};
    
    hal_get_chip_unique_numb(&high, &low);
    hight_64 = high;
    chip_id = (hight_64<<32) | low;
    OS_PRINTF("chip_id = %llx\n", chip_id);
    crc = crc_fast_calculate(CRC32_ARITHMETIC_CCITT, 0xFFFFFFFF, (u8*)&hight_64, sizeof(u64));
    sys_status_set_mac(0, "00");
    sys_status_set_mac(1, "1f");
    for(i = 2; i < 6; i++)
    {
      unsigned int m = crc<<((i-2)*8)>>24;
      sprintf(hmac, "%02x", m);
      sys_status_set_mac(i, hmac);
      memcpy(name, hmac, 2);
      name[2] = '\0';
      hwaddr = (char)strtol(name,&pstr,16);
      OS_PRINTF("%s mac[%d]==%s hwaddr==%d\n",__FUNCTION__,i,hmac,(u8)hwaddr);
    }
  }
  mtos_printk("\n\n##############\n");

}

#ifdef SONATA
static void drv_reset_eth(void)
{
   /* reset eth   this should modify accord to boards and needs*/
  u32 eth_reset = 3; 
 
  gpio_ioctl(GPIO_CMD_IO_ENABLE, eth_reset, TRUE);
  gpio_set_dir(eth_reset, GPIO_DIR_OUTPUT);
  gpio_set_value(eth_reset, 0);
  mtos_task_delay_ms(20);
  gpio_set_value(eth_reset, 1);
  mtos_task_delay_ms(10);
	
	return;
}
#endif

static u8 *_find_led_map(u8 *LED_MAP2,  fp_cfg_t *p_fpcfg)
{
  u8 bitmap_led[8] = {'.', '1', '3', '6', '7', '9', '-', '_'};
  u8 i = 0, j = 0, l_num = 0;

  l_num = 0;
  while(l_num < p_fpcfg->map_size)
  {
    if(p_fpcfg->p_map[l_num].ch == bitmap_led[i])
    {
      bitmap_led[i] = p_fpcfg->p_map[l_num].bitmap;
      i++;
    }
    l_num++;
  }
  LED_MAP2[0] = bitmap_led[4] ^ bitmap_led[1];
  LED_MAP2[1] = ~(bitmap_led[0] | bitmap_led[3]);
  LED_MAP2[2] = bitmap_led[1] ^ LED_MAP2[1];
  LED_MAP2[3] = bitmap_led[7];
  LED_MAP2[4] = ~(bitmap_led[0] | bitmap_led[5]);
  LED_MAP2[5] = bitmap_led[5] ^ bitmap_led[2];
  LED_MAP2[6] = bitmap_led[6];
  LED_MAP2[7] = bitmap_led[0];

  while(j < 8)
  {
    for(i = 0; i < 8; i++)
    {
      if(LED_MAP2[j] >> (i + 1) == 0)
      {
        LED_MAP2[j] = i;
        break;
      }
    }
    j++;
  }
  return LED_MAP2;
}

u32 get_otaback_off_addr(void)
{
  return DM_HDR_START_ADDR;
}

u32 get_bootload_off_addr(void)
{
  return DM_BOOTER_START_ADDR;
}

u32 get_maincode_off_addr(void)
{
  return DM_HDR_START_ADDR;
}

static void drv_init(void)
{
 drv_config_t drv_cfg = {0};
  fp_cfg_t fpcfg = {0};
  irda_cfg_t irdacfg = {0};
  layer_cfg_t osd0_cfg = {0};
  layer_cfg_t osd1_cfg = {0};

  layer_cfg_t sub_cfg = {0};

	/*
	config sharing driver service
	*/
	drv_cfg.sharing_drvsvc_prio = DRV_HDMI_TASK_PRIORITY;

	/*
	config test mode
	*/
	drv_cfg.if_for_test = 1;
#ifdef AUTOTEST
	drv_cfg.test_mode = TEST_AUTO_MODE;
#else
	drv_cfg.test_mode = TEST_MANUAL_MODE;
#endif
  drv_reset_eth();


	drv_cfg.if_use_ethernet = 1;
	drv_cfg.eth_cfg.drv_attach = ethernet_sonata_attach;
	drv_cfg.eth_cfg.p_dev_name = "mt_eth";
	drv_cfg.if_use_wifi = 1;
	drv_cfg.wifi_cfg.drv_attach = rtl8188eus_ethernet_attach;
	drv_cfg.wifi_cfg.p_dev_name = "usb_wifi";
	drv_cfg.wifi_cfg.send_task_prio = WIFI_TASK_LET_SEND;
	drv_cfg.wifi_cfg.recv_task_prio = WIFI_TASK_LET_RECV;
	drv_cfg.wifi_cfg.cmd_task_prio = WIFI_TASK_THREAD;
	drv_cfg.wifi_cfg.wpa_supplicant_task_prio = WPA_SUPPLICANT_TASK;
	/*
	config ethernet: use spi eth controller
	*/
	drv_cfg.if_use_spi_ethernet = 0;
	drv_cfg.spi_eth_cfg.drv_attach = spi_ethernet_attach;
	drv_cfg.spi_eth_cfg.p_dev_name = "spi_eth";
	/*
	config sd, default, no hardware config
	*/
	drv_cfg.if_use_sd = 0;

	/*
	config gpe, default, use hardware gpe
	*/
	drv_cfg.if_use_soft_gpe = 0;

	/*
	config ts input, default is one way on port 0
	*/
	drv_cfg.ts_input_port_num = 1;
	drv_cfg.ts_inut_cfg[0].port = 2;
	drv_cfg.ts_inut_cfg[0].mode = NIM_TS_INTF_PARALLEL;
	drv_cfg.ts_inut_cfg[0].edge = 1;
	drv_cfg.ts_inut_cfg[0].source = TS_SOURCE_DMD_0;


	/*
	config demux
	*/
#ifdef AV_SYNC_ON
	drv_cfg.dmx_cfg.if_disable_av_sync = FALSE;
#else
	drv_cfg.dmx_cfg.if_disable_av_sync = TRUE;
#endif

	/*
	config on-board storage, not include hotplug device like usb key and sd card
	*/
	drv_cfg.stor_cfg.dev_num = 1;
	drv_cfg.stor_cfg.dev_info[0].medium = NOR_FLASH;
	drv_cfg.stor_cfg.dev_info[0].type = CHAR_STOR;
	drv_cfg.stor_cfg.dev_info[0].bus = BUS_SPI;
	drv_cfg.stor_cfg.dev_info[0].size = 8 * 1024; //8M

	/*
	config uio
	*/
	drv_cfg.uio_cfg.mode = UIO_HW_CTRL;
	irdacfg.protocol = IRDA_NEC;
    irdacfg.irda_xtal = 27;  // irda's xtal is 27M( or 4M)
    irdacfg.irda_repeat_time = 300;
	init_fp_cfg(&fpcfg);
	drv_cfg.uio_cfg.soft_cfg.p_ircfg = &irdacfg;
	drv_cfg.uio_cfg.soft_cfg.p_fpcfg = &fpcfg;

    drv_cfg.if_use_nim = 1;

    drv_cfg.nim_cfg.if_dual_nim = 0;
    drv_cfg.nim_cfg.if_sel_nim = 0;
    drv_cfg.nim_cfg.dev_cfg[0].drv_attach = nim_s2_sa_attach;
    drv_cfg.nim_cfg.dev_cfg[0].p_dev_name = "nim_s2_sa";
    drv_cfg.nim_cfg.dev_cfg[0].task_prio = NIM_NOTIFY_TASK_PRIORITY_0;
    //tuner config
    drv_cfg.nim_cfg.dev_cfg[0].tn_cfg.chip_id = TS2022;
    drv_cfg.nim_cfg.dev_cfg[0].tn_cfg.access_type = TN_ACCESS_HW_I2C; //by demod i2c repeater
    drv_cfg.nim_cfg.dev_cfg[0].tn_cfg.if_support_loop_through = 0;
    drv_cfg.nim_cfg.dev_cfg[0].tn_cfg.clk_out_ctrl = 0;
    drv_cfg.nim_cfg.dev_cfg[0].tn_cfg.i2c_cfg.hw_i2c_id = 0;
    drv_cfg.nim_cfg.dev_cfg[0].tn_cfg.i2c_cfg.i2c_addr = 0xc0;

    //demod config
    drv_cfg.nim_cfg.dev_cfg[0].dmd_cfg.freq_offset_limit = 4000;
    //drv_cfg.nim_cfg.dev_cfg[0].dmd_cfg.i2c_cfg.i2c_addr = 0xd0;
    //pin config
    drv_cfg.nim_cfg.dev_cfg[0].pin_cfg.vsel_when_13v = 0;
    drv_cfg.nim_cfg.dev_cfg[0].pin_cfg.vsel_when_lnb_off = 1;
    drv_cfg.nim_cfg.dev_cfg[0].pin_cfg.diseqc_out_when_lnb_off = 0;
    drv_cfg.nim_cfg.dev_cfg[0].pin_cfg.lnb_enable = 1;
    drv_cfg.nim_cfg.dev_cfg[0].pin_cfg.lnb_enable_by_mcu = 1;
    drv_cfg.nim_cfg.dev_cfg[0].pin_cfg.lnb_prot_by_mcu = 0;
    drv_cfg.nim_cfg.dev_cfg[0].pin_cfg.lnb_vol_pin_by_mcu = 1;
      drv_cfg.nim_cfg.dev_cfg[0].pin_cfg.lnb_vol_pin = 7;
    drv_cfg.nim_cfg.dev_cfg[0].pin_cfg.lnb_enable_pin = 7;
  
	/*
	config video out
	*/
	//config display driver
   drv_cfg.if_use_vid = 1;
   drv_cfg.video_cfg.drv_attach = sonata_vdec_attach;
	drv_cfg.video_cfg.disp_cfg.b_osd_hscle = TRUE;
	drv_cfg.video_cfg.disp_cfg.b_osd_vscle = TRUE;
	drv_cfg.video_cfg.disp_cfg.b_di_en = TRUE;
	drv_cfg.video_cfg.disp_cfg.b_vscale = FALSE;
	
	drv_cfg.video_cfg.disp_cfg.misc_buf_cfg.sd_wb_addr = VID_SD_WR_BACK_ADDR;
	drv_cfg.video_cfg.disp_cfg.misc_buf_cfg.sd_wb_size = VID_SD_WR_BACK_SIZE;
	drv_cfg.video_cfg.disp_cfg.misc_buf_cfg.sd_wb_field_no = VID_SD_WR_BACK_FIELD_NO;
	
	///TODO:AE should remove this to pass test
	drv_cfg.video_cfg.disp_cfg.pp_cfg.pp_value = 0x808574;
	drv_cfg.video_cfg.disp_cfg.pp_cfg.shoot_value = 0x20101000;
	drv_cfg.video_cfg.disp_cfg.pp_cfg.pp_mode = PP_MODE_ADAPTIVE;
	drv_cfg.video_cfg.disp_cfg.lock_type = OS_MUTEX_LOCK;

	memset((void *)(VID_SD_WR_BACK_ADDR | 0xa0000000), 0, VID_SD_WR_BACK_SIZE);
	if(TRUE == drv_cfg.video_cfg.disp_cfg.b_di_en)
	{
		drv_cfg.video_cfg.disp_cfg.misc_buf_cfg.di_addr = VID_DI_CFG_ADDR;
		drv_cfg.video_cfg.disp_cfg.misc_buf_cfg.di_size = VID_DI_CFG_SIZE;
	}
	drv_cfg.video_cfg.disp_cfg.p_osd0_cfg = &osd0_cfg;
	drv_cfg.video_cfg.disp_cfg.p_osd1_cfg = &osd1_cfg;
	drv_cfg.video_cfg.disp_cfg.p_sub_cfg = &sub_cfg;

	init_display_cfg(&(drv_cfg.video_cfg.disp_cfg));

	//config on-board VDAC
	if(drv_cfg.platform == PLATFORM_CHIP)
	{
		drv_cfg.video_cfg.board_cfg.vdac_mode[3] = VDAC_CVBS;
		drv_cfg.video_cfg.board_cfg.vdac_mode[1] = VDAC_Y;
		drv_cfg.video_cfg.board_cfg.vdac_mode[0] = VDAC_PB;
		drv_cfg.video_cfg.board_cfg.vdac_mode[2] = VDAC_PR;
	}
	else
	{
		drv_cfg.video_cfg.board_cfg.vdac_mode[0] = VDAC_CVBS;
		drv_cfg.video_cfg.board_cfg.vdac_mode[1] = VDAC_Y;
		drv_cfg.video_cfg.board_cfg.vdac_mode[2] = VDAC_PB;
		drv_cfg.video_cfg.board_cfg.vdac_mode[3] = VDAC_PR;
	}

  /*

  config audio, todo:

  */
  drv_cfg.if_use_adu = 0;
  drv_cfg.audio_cfg.drv_attach = NULL;


#if  1
  /*
  config usb
  */

  drv_cfg.if_use_usb_ethernet = 1;
  drv_cfg.usb_cfg.usb_eth_support_cnt = 2;
  drv_cfg.usb_cfg.usb_eth_drv_attach[0] = usb_eth_sr_attach;
  drv_cfg.usb_cfg.p_usb_eth_drv_name[0] = "sr";
  drv_cfg.usb_cfg.usb_eth_task_prio[0] = USB_ETH0_RECV_TASK_PRIORITY;
  drv_cfg.usb_cfg.usb_eth_drv_attach[1] = usb_eth_asix_attach;
  drv_cfg.usb_cfg.p_usb_eth_drv_name[1] = "asix";
  drv_cfg.usb_cfg.usb_eth_task_prio[1] = USB_ETH1_RECV_TASK_PRIORITY;

  drv_cfg.usb_cfg.host_cnt = 1;
  drv_cfg.usb_cfg.select_usb_port_number = 0; // usb usb 1
  drv_cfg.usb_cfg.if_support_hub = 0;// 1 -disable usb
  drv_cfg.usb_cfg.drv_attach = usb_mass_stor_attach;
  drv_cfg.usb_cfg.bus0_task_prio = USB_BUS_TASK_PRIO;
  drv_cfg.usb_cfg.us0_task_prio = USB_MASS_STOR_TASK_PRIO;

  drv_cfg.usb_cfg.usb_hub_tt_task_prio = USB_HUB_TT_TASK_PRIO;
#endif


	/*
	config smc
	*/
	drv_cfg.if_use_smc = 1;
	drv_cfg.smc_cfg.if_support_vol_sel = 1;
	drv_cfg.smc_cfg.card_detect_pin_level = 0;
	drv_cfg.smc_cfg.vol_sel_gpio = 1;
	drv_cfg.smc_cfg.pin_level_5V = 0;

  {
	/*
	config standby
	*/
	u8 cfg_tmp[8] = {0x0a,0x1c,0x1};
	//u8 LED_MAP1[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	u8 LED_MAP2[8] = {0};
	u8 fp_type = CT1642_FLOUNDER_LOTUS; //mcu fp
 
	drv_cfg.lp_cfg.standby_cfg.pd_key0 = cfg_tmp[0]; /* T.B.D */
	drv_cfg.lp_cfg.standby_cfg.pd_key1 = cfg_tmp[1]; /* T.B.D */
	drv_cfg.lp_cfg.standby_cfg.pd_key_fp = cfg_tmp[2]; /* T.B.D */


	{
		drv_cfg.lp_cfg.standby_cfg.type = fp_type;
		drv_cfg.lp_cfg.standby_cfg.p_raw_map =
			(u8 *)_find_led_map(LED_MAP2, drv_cfg.uio_cfg.soft_cfg.p_fpcfg);
	}
	}
	/*
  config hdmi
  */
  drv_cfg.hdmi_cfg.is_initialized = 0;
  drv_cfg.hdmi_cfg.if_use_hdcp = 0;
  drv_cfg.hdmi_cfg.is_initialized = is_fastlogo;

	/*
	config gprs
	*/
	drv_cfg.if_use_gprs = 0;
	OS_PRINTF("drv_cfg.if_use_gprs = 1;\n");
	/*
	config usb-3g-dongle
	*/
  drv_cfg.if_use_usb_serial = 0;
  drv_cfg.if_use_usb_serial_modem = 0;

	  /* config spi bus for nand flash */
  drv_cfg.spi_nand_cfg.spi_cfg.bus_clk_mhz = 20;
  drv_cfg.spi_nand_cfg.spi_cfg.bus_clk_delay = 0;
  drv_cfg.spi_nand_cfg.spi_cfg.io_num = 1;
  drv_cfg.spi_nand_cfg.spi_cfg.lock_mode = OS_MUTEX_LOCK;
  drv_cfg.spi_nand_cfg.spi_cfg.op_mode = 0;
  drv_cfg.spi_nand_cfg.spi_cfg.pins_cfg[0].miso1_src = 0;
  drv_cfg.spi_nand_cfg.spi_cfg.pins_cfg[0].miso0_src = 1;
  drv_cfg.spi_nand_cfg.spi_cfg.pins_cfg[0].spiio1_src = 0;
  drv_cfg.spi_nand_cfg.spi_cfg.pins_cfg[0].spiio0_src = 0;
  drv_cfg.spi_nand_cfg.spi_cfg.pins_dir[0].spiio0_dir = 1;/*output*/
  drv_cfg.spi_nand_cfg.spi_cfg.pins_dir[0].spiio1_dir = 0;/*input*/
  drv_cfg.spi_nand_cfg.spi_cfg.spi_id = 0;


  drv_cfg.spi_nand_cfg.snand_cfg.rd_mode = SNAND_RD_1IO_MODE;
  drv_cfg.spi_nand_cfg.snand_cfg.wr_mode = SNAND_WR_1IO_MODE;
   
  drv_cfg.spi_nand_cfg.drv_attach_spi_bus = spibus_attach_sonata;
  drv_cfg.spi_nand_cfg.drv_attach_nand = nandflash_attach_sonata;
  drv_cfg.if_use_nand = 0;

  mount_ramfs();
  {
    ethernet_cfg_t      ethcfg = {0};
    ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
    lwip_stack_init(&ethcfg);
  }
//	g3_dongle_init();
	sonata_drv_init(&drv_cfg);

  {
  extern void register_http_stream();
  register_http_stream();
  }
  load_stb_info();
  //ap_charsto_init();
  //ap_charsto_set_protect(PRT_LOWER_7_8);

	{
	//   void *p_dm = NULL;
	dm_v2_init_para_t dm_para = {0};
	u32 read_len;

	//init data manager....
	dm_para.flash_base_addr = get_flash_addr();
	dm_para.max_blk_num = DM_MAX_BLOCK_NUM;
	dm_para.task_prio = MDL_DM_MONITOR_TASK_PRIORITY;
	dm_para.task_stack_size = MDL_DM_MONITOR_TASK_STKSIZE;
	dm_para.open_monitor = TRUE;
	dm_para.para_size = sizeof(dm_v2_init_para_t);
	dm_para.use_mutex = TRUE;
	dm_para.mutex_prio = 1;
	dm_para.test_mode = FALSE;

	dm_init_v2(&dm_para);

	OS_PRINTF("KKKK[%d]set header [0x%x]\n", __LINE__, get_bootload_off_addr());
	dm_set_header(class_get_handle_by_id(DM_CLASS_ID), get_bootload_off_addr());
	OS_PRINTF("KKKK[%d]set header [0x%x]\n", __LINE__, DM_HDR_START_ADDR);
	dm_set_header(class_get_handle_by_id(DM_CLASS_ID), get_maincode_off_addr());

	//load ui resource
	//load_ui_resource();

	//reset_standby_key();//do for 8 Remote Control.

	read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
	                 HW_CFG_BLOCK_ID, 0, 0,
	                 sizeof(hw_cfg_t),
	                 (u8 *)&hw_cfg);
	MT_ASSERT(read_len == sizeof(hw_cfg_t));


	return;
	}
}

static void *p_net_svc = NULL;
void ui_set_net_svc_instance(void *p_instance)
{
  p_net_svc = p_instance;
}

void * ui_get_net_svc_instance(void)
{
  return p_net_svc;
}


extern u8 mount_ramfs();

//#define NUM_ENCODINGS (2)
//iconv_t g_cd_gb2312_to_utf16le = (iconv_t)-1;

static void close_fast_logo(void)
{
  static BOOL closed = FALSE;
  OS_PRINTF("%s\n",__FUNCTION__);
  if(!closed)
  {
    void  *p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
      SYS_DEV_TYPE_DISPLAY);
    disp_layer_show(p_dev, DISP_LAYER_ID_STILL_HD, FALSE);
    closed = TRUE;
  }
}
static void middleware_init(void)
{
//  handle_t dm_handle = NULL;
//  dvb_t *p_dvb = NULL;
//  u32 size = 0;
  eva_init_para_t eva_para = {0};
//  u8 *p_buf = NULL;
  close_fast_logo();

  mdl_init();

  eva_para.debug_level = E_PRINT_NONE;
  eva_para.eva_sys_task_prio_start = EVA_SYS_TASK_PRIORITY;
  eva_para.eva_sys_task_prio_end = EVA_SYS_TASK_PRIORITY_END;
  eva_init(&eva_para);

  eva_filter_factory_init();
  eva_register_filter(NET_SOURCE_FILTER, net_src_filter_create);
  eva_register_filter(NET_UPG_FILTER, net_upg_filter_create);
  eva_register_filter(FLASH_SINK_FILTER, flash_sink_filter_create);


  mul_net_upg_init();


#ifndef WIN32
  {
    net_svc_init_para_t net_svc_cfg;
    net_svc_t *p_net_svc = NULL;
    service_t *p_net = NULL;

    net_svc_cfg.service_prio = MDL_NET_SVC_TASK_PRIORITY;
    net_svc_cfg.nstksize = DEFAULT_STKSIZE;
    net_svc_cfg.is_ping_url = TRUE;
    net_svc_cfg.enable_auto_ping = TRUE;
    net_svc_cfg.monitor_interval= 2000;
    strcpy(net_svc_cfg.param.ping_url, "www.baidu.com");
    //strcpy(net_svc_cfg.param.ping_url_2, "baidu.com");

    net_service_init(&net_svc_cfg);

    p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
    p_net = p_net_svc->get_svc_instance(p_net_svc, APP_FRAMEWORK);

    ui_set_net_svc_instance(p_net);
  }
#endif

  net_source_svc_init(NET_SOURCE_SVC_PRIO, 16 * KBYTES);

{
  pnp_svc_init_para_t pnp_param = {0};
  pnp_param.nstksize = DEFAULT_STKSIZE;
  pnp_param.service_prio = MDL_PNP_SVC_TASK_PRIORITY;
OS_PRINTF("%s(), %d\n", __FUNCTION__, __LINE__);
  pnp_service_init(&pnp_param);
   //add for usb, ethernet, wifi devices
OS_PRINTF("%s(), %d\n", __FUNCTION__, __LINE__);
  usb_monitor_attach();
OS_PRINTF("%s(), %d\n", __FUNCTION__, __LINE__);
  eth_monitor_attach();
  wifi_monitor_attach();
OS_PRINTF("%s(), %d\n", __FUNCTION__, __LINE__);

}

  vfs_uri_init();


  mount_ramfs();


  flash_records_init();
  if(1)
  {
    MAIN_URL_LIST_T url_info;
    u8 enc_url1_buf[256];
    u8 enc_url2_buf[256];
    u16 val_len;
    memset(&url_info, 0x0, sizeof(url_info));

    if( SUCCESS == record_get(SERVER_URL_1, enc_url1_buf, sizeof(enc_url1_buf), &val_len ))
    {
      url_info.array[url_info.count] = enc_url1_buf;
      url_info.encrypt_strlen[url_info.count] = val_len;
      url_info.count++;
    }
    if( SUCCESS == record_get(SERVER_URL_2, enc_url2_buf, sizeof(enc_url2_buf), &val_len ))
    {
      url_info.array[url_info.count] = enc_url2_buf;
      url_info.encrypt_strlen[url_info.count] = val_len;
      url_info.count++;
    }
    set_the_main_server_address(&url_info);
  }

#if 0
  if(1)
  {
    //initialize iconv
    MT_ASSERT(0 == iconv_init(NUM_ENCODINGS));
    MT_ASSERT(0 ==ICONV_ADD_ENC(ucs2le));  //utf-16 little endian
    MT_ASSERT(0 ==ICONV_ADD_ENC(euccn)); //gb2312
  
    g_cd_gb2312_to_utf16le = iconv_open("ucs2le", "euccn");
    MT_ASSERT((iconv_t)-1 != g_cd_gb2312_to_utf16le);
  }
#endif
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

extern ota_policy_t *construct_ota_policy(void);

static void app_init(void)
{
  ap_frm_init_param_t param = {0};
  handle_t ap_frm_handle = NULL;
  u32 ap_frm_msgq_id = 0;

  app_info_t app_array[] =
  {
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
OS_PRINTF("*******************************%s(), %d, ap_frm_msgq_id = %d\n", __FUNCTION__, __LINE__, ap_frm_msgq_id);
  mdl_set_msgq(class_get_handle_by_id(MDL_CLASS_ID), ap_frm_msgq_id);
}

static void util_init(void)
{
  simple_queue_init();
}

extern void ui_init(void);

extern u32 attach_ipcfw_fun_set_warriors(ipc_fw_fun_set_t * p_funset);


#if (!defined WIN32)
#define FF_BSS __attribute__((section(".av_bss")))
extern FF_BSS ipc_fw_fun_set_t g_ipcfw_f;
#endif

void ap_init(void)
{

//  memset((void *)(&g_ipcfw_f), 0, sizeof(ipc_fw_fun_set_t));
//  attach_ipcfw_fun_set_warriors(&g_ipcfw_f);


  //hal_dcache_flush_all();

//  ap_ipc_init(32);   // ap ipc fifo create


  mem_cfg(MEMCFG_T_NORMAL);
  mtos_printk("\r\n[OTA] mem cfg ok ");

  util_init();

  drv_init();

  middleware_init();

  app_init();
  mtos_printk("\r\n[OTA] app init ok ");

  ui_init();

}
