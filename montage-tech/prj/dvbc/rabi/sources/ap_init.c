#include "stdio.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"
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
#include "mtos_event.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
#include "mtos_int.h"
// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_unicode.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "char_map.h"
#include "gb2312.h"
#include "fcrc.h"
// driver
#include "hal_timer.h"
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_misc.h"
#include "hal_uart.h"
#include "hal_otp.h"
#include "hal.h"
#include "ipc.h"
#include "common.h"
#include "drv_dev.h"
#include "drv_misc.h"
#include "../../../src/drv/drvbase/drv_svc.h"
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
#include "dmx.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"
#include "sys_types.h"
#include "smc_op.h"
#include "mtos_event.h"
#include "list.h"
#include "block.h"
#ifdef WIN32
#include "fsioctl.h"
#include "ufs.h"
#endif
#include "vfs.h"
#include "hdmi.h"
#include "region.h"
#include "display.h"
#include "pdec.h"
#ifndef WIN32
#include "netif/ppp.h"
#endif
#include "lpower.h"
#include "spi.h"
#include "hid.h"
#include "drv_cfg.h"
#include "modem.h"
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
#include "mosaic.h"
#include "nit.h"
#include "pat.h"
#include "pmt.h"
#include "sdt.h"
#include "cat.h"
#include "emm.h"
#include "ecm.h"
#include "bat.h"
#include "video_packet.h"
#include "eit.h"
#ifndef IMPL_NEW_EPG
#include "epg_data4.h"
#endif
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#if ENABLE_NETWORK
#include "net_svc.h"
#endif
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "ss_ctrl.h"
#include "mt_time.h"
#include "vfs.h"
#include "avctrl1.h"
#ifndef WIN32
#include "vbi_api.h"
#endif
#include "hal_secure.h"
#include "sctrl.h"
#include "lib_rect.h"
#include "lib_memp.h"
#include "common.h"
#include "gpe_vsb.h"
#include "surface.h"
#include "lib_memf.h"
#include "flinger.h"
#include "gdi.h"
#include "ctrl_base.h"
#include "gui_resource.h"
#include "gui_paint.h"
#include "framework.h"
#include "surface.h"
#include "db_dvbs.h"
#include "mem_cfg.h"
#include "pnp_service.h"
#include "media_data.h"
#include "cas_ware.h"
#include "dsmcc.h"

#include "monitor_service.h"
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
#include "file_source_filter.h"
#include "ts_player_filter.h"
#include "av_render_filter.h"
#include "demux_filter.h"
#include "record_filter.h"
#include "file_sink_filter.h"
#include "pic_filter.h"
#include "pic_render_filter.h"
#include "mp3_player_filter.h"
#include "lrc_filter.h"
#include "str_filter.h"
#include "flash_sink_filter.h"
#include "ota_filter.h"
#include "file_list.h"
#include "dmx_manager.h"
#if ENABLE_NETWORK
#include "vod_public.h"
#include "google_map_filter.h"
#include "weather_filter.h"
#include "net_source_filter.h"
#include "net_extern_filter.h"
#include "vod_filter.h"
#include "albums_public.h"
#include "albums_filter.h"
#include "vod_api.h"
#include "albums_api.h"
#include "news_filter.h"
#include "news_api.h"
#include "net_upg_filter.h"
#include "flash_sink_filter.h"
#include "net_upg_api.h"
#include "albums_public.h"
#include "albums_api.h"
#include "net_music_filter.h"
#include "net_music_api.h"
#include "music_api.h"
#include "livetv_db.h"
#endif
#ifdef IMPL_NEW_EPG
#include "epg_type.h"
//#include "epg_filter.h"
#endif
// ap
#include "ap_framework.h"
#include "ap_uio.h"
#include "ap_playback.h"
#include "ap_scan.h"
#include "ap_signal_monitor.h"
#include "ap_satcodx.h"
#include "ap_upgrade.h"
#include "ap_time.h"
#include "ap_cas.h"
#ifndef IMPL_NEW_EPG
#include "ap_epg4.h"
#endif
#include "media_data.h"
#include "ap_usb_upgrade.h"
#include "pvr_api.h"
//customer define
#include "customer_config.h"
#include "lpower.h"
#include "ui_util_api.h"
#include "ui_usb_api.h"
#include "sys_status.h"
#include "ethernet.h"
#include "subt_station_filter.h"
#include "user_parse_table.h"
#ifdef NVOD_ENABLE
#include "nvod_data.h"
#include "ap_nvod.h"
#endif
#if defined DESAI_56_CA || defined DESAI_52_CA
#include "ap_nit.h"
#endif
#include "iconv_ext.h"
#include "plug_monitor.h"
#include "network_monitor.h"

#include "LzmaIf.h"

#include "spi.h"

#ifdef TENGRUI_ROLLTITLE
#include "ap_tr_subt.h"
#endif

#ifndef WIN32
#include "register_net_stream.h"
#endif

#include "commonData.h"

#include "db_conn_play.h"
#include "db_play_hist.h"
#include "wifi.h"
#include "sys_dbg.h"
#include "ap_charsto_impl.h"

#ifndef WIN32
  #define FF_BSS __attribute__((section(".av_bss")))
  extern FF_BSS ipc_fw_fun_set_t g_ipcfw_f;
  extern RET_CODE usb_mass_stor_attach(char *p_name);
#endif

#define DEMOD_RST_PIN 3
#define NUM_ENCODINGS (4)
#define PTI_PARALLEL

#define AV_SYNC_FLAG_ON
#define GPE_USE_HW (1)
typedef struct wifi_config
{
  u8 wifi_send_task_prio;
  u8 wifi_recv_task_prio;
  u8 wifi_cmd_task_prio;
  u8 wifi_wpa_supplicant_task_prio;
}wifi_config_t;
typedef struct
{
u8 task_let_0;
u8 task_let_1;
u8 task_let_2;
u8 task_let_3;
 u8 wpa_supplicant_task_prio;
}wifi_ra0_config_t;

#ifdef WIN32
extern RET_CODE disk_win32_attach(char *p_name);
#endif
extern u8 ufs_dev_init();
extern u32 get_flash_addr(void);
extern void load_ui_resource(void);
extern RET_CODE ethernet_attach(char *p_name);
extern int lwip_stack_init(ethernet_cfg_t *p_cfg);
extern void pppInit(void);
extern RET_CODE lpower_concerto_attach(char *p_name);
#ifndef IMPL_NEW_EPG
extern void epg_data_init4(void);
extern epg_policy_t *construct_epg_policy4(void);
#else
extern void epg_api_init(void);
extern ifilter_t *epg_filter_create(void *p_para);
#endif
extern ap_frm_policy_t *construct_ap_frm_policy(void);
extern ap_uio_policy_t *construct_ap_uio_policy(void);
extern sig_mon_policy_t *construct_sig_mon_policy(void);
extern pb_policy_t *construct_pb_policy(void);
extern cas_policy_t *construct_cas_policy(void);

//extern ota_policy_t *construct_dvbs_ota_policy(void);
extern time_policy_t *construct_time_policy(void);
extern upg_policy_t *construct_upg_policy(void);
extern usb_upg_policy_t *construct_usb_upg_policy(void);
//extern ota_policy_t *construct_ota_policy(void);
//extern ap_twin_port_policy_t *construct_twin_policy(void);
extern upg_policy_t *construct_upg_policy(void);
extern app_t *construct_virtual_nim_filter(app_id_t app_id,u8 tuner_id);
extern int add_av_task_cmd(void * hComp, void *entryPoint, int priority, int once);
extern void ui_init(void);

#ifdef TENGRUI_ROLLTITLE
extern subt_policy_t* construct_subt_policy(void);
extern app_t *construct_ap_tr_subt(subt_policy_t *p_policy);
#endif

extern RET_CODE smc_attach_concerto(char *name);


extern RET_CODE ethernet_concerto_attach(char *p_name);
extern RET_CODE rtl8188eus_ethernet_attach(char *p_name);
extern RET_CODE cmm_wifi_attach(char *devname);
extern RET_CODE usb_eth_asix_attach(char *p_name);
extern RET_CODE usb_eth_sr_attach(char *p_name);
extern app_t *construct_virtual_nim_filter(app_id_t app_id,u8 tuner_id);
extern void ui_set_front_panel_by_str(const char * str);

//static modem_apn_info_t g_account_gprs[]={};

static hw_cfg_t hw_cfg = {0};

#ifdef SHOW_MEM_SUPPORT
#define MEM_SHOW_MARK 0x1FFFFFFF
#define MEM_SHOW_MAX_NAME_SIZE 30
void show_single_memory_mapping(u32 flag, u32 start_addr, u32 size)
{
  u8 name[MEM_SHOW_MAX_NAME_SIZE] = {0};
  u32 s_addr = 0;
  u32 e_addr = 0;

  OS_PRINTF("S:,\n");
  OS_PRINTF("#Name,Start Addr,End Addr,Size,Note(KBytes),Total(KBytes),!\n");
  memset(name, 0, MEM_SHOW_MAX_NAME_SIZE);
  switch(flag)
  {
    case AUDIO_FW_CFG_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Audio_fw_cfg", strlen("Audio_fw_cfg"));
      break;
    case AV_POWER_UP_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Av_power_up", strlen("Av_power_up"));
      break;
    case VID_SD_WR_BACK_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Vid_sd_wr_back", strlen("Vid_sd_wr_back"));
      break;
    case VID_DI_CFG_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Vid_di_cfg", strlen("Vid_di_cfg"));
      break;
    case VIDEO_FW_CFG_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Video_fw_cfg", strlen("Video_fw_cfg"));
      break;
    case OSD0_VSCALER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Osd0_vscaler", strlen("Osd0_vscaler"));
      break;
    case OSD1_VSCALER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Osd1_vscaler", strlen("Osd1_vscaler"));
      break;
    case SUB_VSCALER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Sub_vscaler", strlen("Sub_vscaler"));
      break;
    case OSD0_LAYER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Osd0_layer", strlen("Osd0_layer"));
      break;
    case OSD1_LAYER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Osd1_layer", strlen("Osd1_layer"));
      break;
    case SUB_LAYER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Sub_layer", strlen("Sub_layer"));
      break;
    case REC_BUFFER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Rec_buffer", strlen("Rec_buffer"));
      break;
    case EPG_BUFFER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Epg_buffer", strlen("Epg_buffer"));
      break;
    case GUI_RESOURCE_BUFFER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Gui_resource_buffer", strlen("Gui_resource_buffer"));
      break;
    case AV_STACK_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Av_stack", strlen("Av_stack"));
      break;
    case CODE_SIZE_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      size = size & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Code_size", strlen("Code_size"));
      break;
    case SYSTEM_PARTITION_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "System_paritition", strlen("System_partition"));
      break;
    default:
      break;
  }
  OS_PRINTF("#%s,0x%x,0x%x,0x%x,%d!\n", name, s_addr, e_addr, size, size / 0x400);
  OS_PRINTF("E:,\n");
}
#endif

#if 0
static void board_config(void)
{
}
#endif

u8 nim_get_tuner_count(void)
{
  return 1;
}

static BOOL init_display_cfg(disp_cfg_t *p_cfg)
{
  u32 p_addr = 0;

  //init sub buffer
  p_addr = SUB_BUFFER_ADDR;
  if(SUB_BUFFER_SIZE == 0)
    p_addr = 0;

#ifndef WIN32
  p_cfg->p_sub_cfg->odd_mem_start = p_addr;
  p_cfg->p_sub_cfg->odd_mem_end = p_addr + SUB_BUFFER_SIZE;
  p_cfg->p_sub_cfg->even_mem_start = 0;
  p_cfg->p_sub_cfg->even_mem_end = 0;
#else
  p_addr &= (~0xF0000000);
  p_cfg->p_sub_cfg->odd_mem_start = p_addr;
  p_cfg->p_sub_cfg->odd_mem_end = p_addr + SUB_BUFFER_SIZE / 2;
  p_cfg->p_sub_cfg->even_mem_start = p_addr + SUB_BUFFER_SIZE / 2;
  p_cfg->p_sub_cfg->even_mem_end = p_addr + SUB_BUFFER_SIZE;
#endif

  // init osd0 frame buffer
  p_addr = OSD1_BUFFER_ADDR;
  if(OSD1_BUFFER_SIZE == 0)
    p_addr = 0;

#ifndef WIN32
  p_cfg->p_osd1_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd1_cfg->odd_mem_end = p_addr + OSD1_BUFFER_SIZE;
  p_cfg->p_osd1_cfg->even_mem_start = 0;
  p_cfg->p_osd1_cfg->even_mem_end = 0;
#else
  p_addr &= (~0xF0000000);
  p_cfg->p_osd1_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd1_cfg->odd_mem_end = p_addr + OSD1_BUFFER_SIZE / 2;
  p_cfg->p_osd1_cfg->even_mem_start = p_addr + OSD1_BUFFER_SIZE / 2;
  p_cfg->p_osd1_cfg->even_mem_end = p_addr + OSD1_BUFFER_SIZE;
#endif

  // init osd1 frame buffer
  p_addr = OSD0_BUFFER_ADDR;
  if(OSD0_BUFFER_SIZE == 0)
    p_addr = 0;

#ifndef WIN32
  p_cfg->p_osd0_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd0_cfg->odd_mem_end = p_addr + OSD0_BUFFER_SIZE;
  p_cfg->p_osd0_cfg->even_mem_start = 0;
  p_cfg->p_osd0_cfg->even_mem_end = 0;
#else
  p_addr &= (~0xF0000000);
  p_cfg->p_osd0_cfg->odd_mem_start = p_addr;
  p_cfg->p_osd0_cfg->odd_mem_end = p_addr + OSD0_BUFFER_SIZE / 2;
  p_cfg->p_osd0_cfg->even_mem_start = p_addr + OSD0_BUFFER_SIZE / 2;
  p_cfg->p_osd0_cfg->even_mem_end = p_addr + OSD0_BUFFER_SIZE;
#endif
#ifdef SHOW_MEM_SUPPORT
  show_single_memory_mapping(GUI_RESOURCE_BUFFER_FLAG,
                             GUI_RESOURCE_BUFFER_ADDR,
                             GUI_RESOURCE_BUFFER_SIZE);
  show_single_memory_mapping(EPG_BUFFER_FLAG, EPG_BUFFER_ADDR, EPG_BUFFER_SIZE);
  show_single_memory_mapping(REC_BUFFER_FLAG, REC_BUFFER_ADDR, REC_BUFFER_SIZE);
  show_single_memory_mapping(OSD0_LAYER_FLAG,
                             p_cfg->p_osd0_cfg->odd_mem_start,
                             p_cfg->p_osd0_cfg->odd_mem_end - p_cfg->p_osd0_cfg->odd_mem_start);
  show_single_memory_mapping(OSD1_LAYER_FLAG,
                             p_cfg->p_osd1_cfg->odd_mem_start,
                             p_cfg->p_osd1_cfg->odd_mem_end - p_cfg->p_osd1_cfg->odd_mem_start);
  show_single_memory_mapping(SUB_LAYER_FLAG,
                             p_cfg->p_sub_cfg->odd_mem_start,
                             p_cfg->p_sub_cfg->odd_mem_end - p_cfg->p_sub_cfg->odd_mem_start);
#endif

  p_cfg->lock_type = OS_MUTEX_LOCK;
  return TRUE;
}

iconv_t g_cd_utf16le_to_utf8 = NULL;
iconv_t g_cd_utf8_to_utf16le = NULL;
iconv_t g_cd_gb2312_to_utf16le = NULL;
static void util_init(void)
{
#ifdef CORE_DUMP_DEBUG
  static debug_mem_t d_mem = {0};

  d_mem.start_addr = (u8 *)mtos_malloc(10 * KBYTES);
  MT_ASSERT(d_mem.start_addr != NULL);
  memset(d_mem.start_addr, 0, 10 * KBYTES);

  d_mem.common_print_size = 2 * KBYTES;
  d_mem.msgq_print_size = 2 * KBYTES;
  d_mem.os_info_size = 4 * KBYTES;
  d_mem.stack_info_size = 1 * KBYTES;
  d_mem.mem_map_size = 1 * KBYTES;

  os_debug_init(&d_mem);
#endif
  simple_queue_init();

  //initialize iconv
  MT_ASSERT(0 == iconv_init(NUM_ENCODINGS));
  MT_ASSERT(0 ==ICONV_ADD_ENC(utf8));  //utf-8
  MT_ASSERT(0 ==ICONV_ADD_ENC(ucs2le));  //utf-16 little endian
  MT_ASSERT(0 ==ICONV_ADD_ENC(euccn)); //gb2312
  //MT_ASSERT(0 ==ICONV_ADD_ENC(iso8859_9)); //iso8859_9
  g_cd_utf8_to_utf16le  = iconv_open("ucs2le", "utf8");
  g_cd_gb2312_to_utf16le  = iconv_open("ucs2le", "euccn");
  g_cd_utf16le_to_utf8 = iconv_open("utf8", "ucs2le");
}

#ifndef WIN32

typedef struct  bin_board_cfg_st
{
  BOOL bin_flag;
  hal_board_config_t  board_cfg;
}bin_board_cfg;
static bin_board_cfg bin_board_cfg_info;

void set_board_config(void)
{
  u8 cfg_buff[2 * KBYTES] = {0};
  u32 read_len = 0;
  u32 size = 0;

  memset(&bin_board_cfg_info,0,sizeof(bin_board_cfg));
  bin_board_cfg_info.bin_flag = 0;
  size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), BOARD_CONFIG_BLOCK_ID);
  if(size >0)
  {

    if(size > 2 * KBYTES)
    {
      size = 2 * KBYTES;
    }
    read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
    BOARD_CONFIG_BLOCK_ID, 0, 0,
    size,cfg_buff);
    if(read_len >0)
    {
      bin_board_cfg_info.bin_flag = 1;
      memcpy(&bin_board_cfg_info.board_cfg,cfg_buff,sizeof(hal_board_config_t));
      hal_board_config(cfg_buff);
    }
  }
}
#endif

#ifdef WIN32
extern u8 ufs_dev_init();
static void s32_block_int(void)
{
  drv_dev_t *p_dev = NULL;
  block_device_config_t config = {0};
  tchar_t part_tab = 0;
  u8 value = 0;
  RET_CODE ret = SUCCESS;
  char *disk_win32_dev = "disk_win32";

  //assign_drives(1,5); //fix me andy.chen//what's happened
  disk_win32_attach(disk_win32_dev);

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_WIN32_FAKE_DEV);
  OS_PRINTF("p_dev[0x%x]\n",p_dev);

  /*config  block device*/
  ufs_dev_init();
  config.cmd = 1;  /*support tr transport*/
  config.tr_submit_prio = BLOCK_SERV_PRIORITY;   /*tr task prio*/
  dev_open(p_dev, &config);


  regist_device(p_dev);
  regist_logic_parttion(&part_tab,&value);
}
#endif

BOOL uio_get_lock_pol(void)
{
  return hw_cfg.pan_info.lbd[0].polar;
}


BOOL uio_get_power_pol(void)
{
  return hw_cfg.pan_info.lbd[1].polar;
}

u8 *find_led_map(u8 *LED_MAP2)
{
  u8 bitmap_led[8] = {'.', '1', '3', '6', '7', '9', '-', '_'};
  u8 i = 0, j = 0, l_num = 0;

  l_num = 0;
  while(l_num < hw_cfg.map_size)
  {
    if(hw_cfg.led_bitmap[l_num].ch == bitmap_led[i])
    {
      bitmap_led[i] = hw_cfg.led_bitmap[l_num].bitmap;
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
static void uio_init(void)
{
  RET_CODE ret;
#ifndef WIN32
  u32 reg= 0 , val = 0;
  void *p_dev_i2cfp = NULL;
#endif
  void *p_dev = NULL;
  uio_cfg_t uiocfg = {0};
  fp_cfg_t fpcfg = {0};
  irda_cfg_t irdacfg = {0};


  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
  MT_ASSERT(NULL != p_dev);

  fpcfg.p_info = &(hw_cfg.pan_info);
  fpcfg.fp_type = hw_cfg.fp_type;

  OS_PRINTF("===fpcfg.fp_type:%d, map_size %d ,com0:%d, com7:%d,reserve7:%d\n", fpcfg.fp_type, fpcfg.map_size,
    fpcfg.p_info->com[0].pos, fpcfg.p_info->com[7].pos,fpcfg.p_info->com[7].reserve);


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
#ifdef REMOTE_REPEAT_DELAY
  irdacfg.irda_repeat_time = 600;
#else
  irdacfg.irda_repeat_time = 300;
#endif
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

  ui_set_com_num((u8)(hw_cfg.led_num));

  switch(CUSTOMER_ID)
  {
  	case CUSTOMER_XINNEW_DEMO:
	case CUSTOMER_MAIKE_HUNAN_LINLI:
	case CUSTOMER_XINNEW_GAIZHOU:
	case CUSTOMER_DTMB_DESAI_JIMO:
	case CUSTOMER_DTMB_SZXC_GUOMAO:
	case CUSTOMER_MAIKE_HUNAN:
	case CUSTOMER_DTMB_CHANGSHA_HHT:
	case CUSTOMER_DTMB_SHANGSHUIXIAN:
		if(hw_cfg.led_num == 4)
		{
		  ui_set_front_panel_by_str(" ON ");
		}
		else if(hw_cfg.led_num == 3)
		{
		  ui_set_front_panel_by_str("ON ");
		}
		else if(hw_cfg.led_num == 2)
		{
		  ui_set_front_panel_by_str("ON ");
		}
		else if(hw_cfg.led_num == 1)
		{
		  ui_set_front_panel_by_str("O ");
		}
		break;
	default:
		if(hw_cfg.led_num == 4)
		{
			uio_display(p_dev, (u8 *)" ON ", 4);
		}
		else if(hw_cfg.led_num == 3)
		{
			uio_display(p_dev, (u8 *)"ON ", 3);
		}
		else if(hw_cfg.led_num == 2)
		{
			uio_display(p_dev, (u8 *)"ON ", 2);
		}
		else if(hw_cfg.led_num == 1)
		{
			uio_display(p_dev, (u8 *)"O ", 1);
		}
		break;
  }
  
  dev_io_ctrl(p_dev, UIO_FP_SET_POWER_LBD, TRUE);
#ifndef WIN32
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
#endif
  //gpio_output(0, 1);
}

#ifndef WIN32
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
  hdmi_cfg.is_initialized = 1;
  hdmi_cfg.is_fastlogo_mode = 0;

  ret = dev_open(p_hdmi_dev, &hdmi_cfg);
  MT_ASSERT(SUCCESS == ret);
}
#endif

void spiflash_cfg( )
{
    RET_CODE ret;
    void *p_dev = NULL;
    charsto_cfg_t charsto_cfg = {0};
#ifndef WIN32
    void *p_spi = NULL; 
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
#else
  ret = ATTACH_DRIVER(CHARSTO, warriors, default, default);
  MT_ASSERT(ret == SUCCESS);

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(NULL != p_dev);

  charsto_cfg.size = CHARSTO_SIZE;
  charsto_cfg.spi_clk = FLASH_C_CLK_P6;
  charsto_cfg.rd_mode = SPI_FR_MODE;
  charsto_cfg.lock_mode = OS_MUTEX_LOCK;
#endif
    ret = dev_open(p_dev, &charsto_cfg);
    MT_ASSERT(SUCCESS == ret);
}

#if ENABLE_ETH
static s32 _ethernet_init(board_ethernet_cfg_t *p_cfg)
{
  RET_CODE ret = ERR_FAILURE;
#ifndef WIN32
  ethernet_cfg_t ethcfg = {0};
  void *p_dev = NULL;
  p_cfg->drv_attach(p_cfg->p_dev_name);
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
  MT_ASSERT(p_dev != NULL);
  ret = dev_open(p_dev, (void *)&ethcfg);
#endif
  return ret;
}
#endif

#if ENABLE_8188_SUPPORT
static void _wifi_init(void)
{
#ifndef WIN32
  RET_CODE ret = ERR_FAILURE;
  ethernet_cfg_t ethcfg = {0};
  wifi_config_t cfg;
  void *p_dev = NULL;
  cfg.wifi_send_task_prio = WIFI_TASK_LET_SEND;
  cfg.wifi_recv_task_prio = WIFI_TASK_LET_RECV;
  cfg.wifi_cmd_task_prio = WIFI_TASK_THREAD;
  cfg.wifi_wpa_supplicant_task_prio = WPA_SUPPLICANT_TASK;
  ethcfg.p_priv_confg = (u8*)&cfg;
  rtl8188eus_ethernet_attach(RTL_REALTEK_WIFI_NAME);
  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)RTL_REALTEK_WIFI_NAME);
  MT_ASSERT(p_dev != NULL);
  ret = dev_open(p_dev, (void *)&ethcfg);
  OS_PRINTF("attach wifi success\n");
#endif
}
#endif

#if ENABLE_5370_SUPPORT
static void _ra0_wifi_init(void)
{
#ifndef WIN32
  RET_CODE ret = ERR_FAILURE;
  ethernet_cfg_t ethcfg = {0};
  wifi_ra0_config_t cfg;
  void *p_dev = NULL;  
  cfg.task_let_0 = WIFI_TASK_LET_MTK_RECV;    
  cfg.task_let_1= WIFI_TASK_LET_MTK_TIMER;
  cfg.task_let_2 = WIFI_TASK_THREAD;
  cfg.task_let_3 = WIFI_TASK_THREAD_2;
  cfg.wpa_supplicant_task_prio = WPA_SUPPLICANT_TASK;
  ethcfg.p_priv_confg = (u8*)&cfg;
  cmm_wifi_attach(MTK_RALINK_WIFI_NAME);
  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)MTK_RALINK_WIFI_NAME);
  MT_ASSERT(p_dev != NULL);
  ret = dev_open(p_dev, (void *)&ethcfg);
  OS_PRINTF("attach wifi success\n");
#endif
}
#endif

#if ENABLE_7601_SUPPORT
static void _rt7601_wifi_init(void)
{
#ifndef WIN32
    extern RET_CODE cmm_wifi_attach_rt7601(char *devname);
    RET_CODE ret = ERR_FAILURE;
    ethernet_cfg_t ethcfg = {0};
    wifi_task_config_t cfg;
    void *p_dev = NULL;  

    cfg.Wifi_Prio_H = WIFI_TASK_H_THREAD;
    cfg.Wifi_Prio_M1 = WIFI_TASK_M1_THREAD;
    cfg.Wifi_Prio_M2 = WIFI_TASK_M2_THREAD;
    cfg.Wifi_Prio_M3 = WIFI_TASK_M3_THREAD;
    cfg.Wifi_Prio_L1 = WIFI_TASK_L1_THREAD;
    cfg.Wifi_Prio_L2 = WIFI_TASK_L2_THREAD; 
    cfg.wifi_wpa_supplicant_task_prio = WPA_SUPPLICANT_TASK;
    cfg.wifi_net_link_task_prio = NET_LINK_TASK;
    ethcfg.p_priv_confg = (u8*)&cfg;

    cmm_wifi_attach_rt7601(MTK_RALINK7601_WIFI_NAME);

    p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)MTK_RALINK7601_WIFI_NAME);
    MT_ASSERT(p_dev != NULL);
    ret = dev_open(p_dev, (void *)&ethcfg);
    mtos_printk("attach wifi success.\n");
#endif
}
#endif

#if 0
static s32 gprs_init(void)
{
#ifndef WIN32
  extern RET_CODE modem_neo_m660_attach(char *name);
  RET_CODE ret = ERR_FAILURE;
  void *p_dev = NULL;
  modem_cfg_t cfg = {0};
  
  ret = modem_neo_m660_attach("neo_m660");
  MT_ASSERT(ret == SUCCESS);

  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"neo_m660");
  MT_ASSERT(NULL != p_dev);

  cfg.comm_device = 1;
  cfg.ppp_task_prio = PPP_TASK_PRIORITY;
  cfg.modem_task_prio = MODEM_TASK_PRIORITY;
  cfg.lock_mode = OS_MUTEX_LOCK;
  ret = dev_open(p_dev, &cfg);

  modem_set_apn_info(p_dev, g_account_gprs, sizeof(g_account_gprs)/sizeof(modem_apn_info_t));
  MT_ASSERT(SUCCESS == ret);

  uart_set_param(1, 115200, 8, 1, 0);
#endif

  return SUCCESS;
}
static s32 g3_dongle_init(void)
{
#ifndef WIN32

   extern RET_CODE  usb_tty_attach(char *p_name);
   extern RET_CODE modem_usb_3g_dongle_attach(char *p_name);  

   RET_CODE ret = ERR_FAILURE;
   void *p_dev = NULL;
   modem_cfg_t cfg = {0};
   
   //tty attach
   if(SUCCESS == usb_tty_attach("ttyUSB"))
   {
        void *p_dev = NULL;
        p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"ttyUSB");
        MT_ASSERT(p_dev != NULL);
        dev_open(p_dev, (void *)NULL);
   }

  //3g dongle attach
  ret = modem_usb_3g_dongle_attach("usb_serial_modem");
  MT_ASSERT(ret == SUCCESS);
  
  p_dev = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"usb_serial_modem");
  MT_ASSERT(NULL != p_dev);
  
  cfg.ppp_task_prio = PPP_TASK_PRIORITY_3G;
  cfg.modem_task_prio = MODEM_TASK_PRIORITY_3G;
  ret = dev_open(p_dev, &cfg);
  MT_ASSERT(SUCCESS == ret);
  
#endif
  return SUCCESS;
}
#endif
/*
static void live_tv_db_init()
{
  livetv_db_init_t p_param;
  
  u32 size = livetv_db_size_get();
  
  OS_PRINTF("init live db size %d\n", size);
  
  p_param.p_block_buf = mtos_malloc(size);
  p_param.buf_size = size;
  p_param.max_group = MAX_LIVE_GROUP;
  p_param.max_prg = MAX_LIVE_PRG;
  p_param.db_block_id = 0xAA;
  livetv_db_init(&p_param);
}
*/
#ifdef WIN32
static BOOL init_nim_cfg(nim_config_t *p_cfg)
{
  //Ts2022 config
  p_cfg->tun_crystal_khz = 27000;
  p_cfg->tun_rf_bypass_en = 0;
  p_cfg->tun_clk_out_by_tn = 1;
  p_cfg->tun_clk_out_div = 1;
  p_cfg->tun_clk_out_by_xtal = 1;
  p_cfg->bs_times = 1;  
  
  p_cfg->pin_config.lnb_enable = 1;
  p_cfg->pin_config.vsel_when_lnb_off = 0;
  p_cfg->pin_config.diseqc_out_when_lnb_off = 0;


  p_cfg->pin_config.vsel_when_13v = 0;
  p_cfg->pin_config.lnb_enable_by_mcu = 0;
  p_cfg->pin_config.lnb_prot_by_mcu = 0; 

  return TRUE;
}
#endif

static s32 _usb_init(_usb_cfg_t *p_usb_cfg)
{
  RET_CODE ret = ERR_FAILURE;
  void *p_dev = NULL;
  block_device_config_t mass_config = {0};
  ethernet_cfg_t ethcfg = {0};
  u32 i = 0;
  _usb_cfg_t *usb_cfg = NULL;
  mass_config.cmd = 0;
  mass_config.bus_prio = p_usb_cfg->bus0_task_prio;
  mass_config.bus_thread_stack_size = 2048*2;//2048 not enough after add 3g dongle,tmp *2
  mass_config.block_mass_stor_thread_stack_size = 2048 + 256;//xiongfei for bug 45247, 20140725
  mass_config.lock_mode = OS_MUTEX_LOCK;
  mass_config.select_usb_port_num = p_usb_cfg->select_usb_port_number;
  mass_config.hub_tt_prio = p_usb_cfg->usb_hub_tt_task_prio;
  mass_config.hub_tt_stack_size = USB_HUB_TT_TASK_SIZE;
  mtos_printk("%s  usb_cfg.host_cnt %d\n",__FUNCTION__,p_usb_cfg->host_cnt);
  if(p_usb_cfg->host_cnt == 2)
  {
    ret = p_usb_cfg->drv_attach(DEV_NAME_BLOCK_USB0);
    MT_ASSERT(ret == SUCCESS);
    mass_config.block_mass_stor_prio = p_usb_cfg->us0_task_prio;
    p_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_NAME, (u32)DEV_NAME_BLOCK_USB0);
    ret = dev_open(p_dev, (void *)&mass_config);
    MT_ASSERT(SUCCESS == ret);
    mass_config.block_mass_stor_prio = p_usb_cfg->us1_task_prio;
    mtos_printk("%s mass_stor_prio %d\n",__FUNCTION__,mass_config.block_mass_stor_prio);
    ret = p_usb_cfg->drv_attach(DEV_NAME_BLOCK_USB1);
    p_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_NAME, (u32)DEV_NAME_BLOCK_USB1);
    ret = dev_open(p_dev, (void *)&mass_config);
    MT_ASSERT(SUCCESS == ret);
  }
  else if(p_usb_cfg->host_cnt == 1)
  {
    if(p_usb_cfg->select_usb_port_number == 0)
    {
      mass_config.block_mass_stor_prio = p_usb_cfg->us0_task_prio;
      mtos_printk("%s mass_stor_prio %d\n",__FUNCTION__,mass_config.block_mass_stor_prio);
      ret = p_usb_cfg->drv_attach(DEV_NAME_BLOCK_USB0);
      MT_ASSERT(ret == SUCCESS);
      p_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_NAME, (u32)DEV_NAME_BLOCK_USB0);
      ret = dev_open(p_dev, (void *)&mass_config);
      MT_ASSERT(SUCCESS == ret);
    }
    else if(p_usb_cfg->select_usb_port_number == 1)
    {
      ret = p_usb_cfg->drv_attach(DEV_NAME_BLOCK_USB1);
      p_dev = (drv_dev_t *)dev_find_identifier(NULL, DEV_IDT_NAME, (u32)DEV_NAME_BLOCK_USB1);
      mass_config.block_mass_stor_prio = p_usb_cfg->us1_task_prio;
      mtos_printk("%s mass_stor_prio %d\n",__FUNCTION__,mass_config.block_mass_stor_prio);
      ret = dev_open(p_dev, (void *)&mass_config);
      MT_ASSERT(SUCCESS == ret);
    }
    else
    {
      MT_ASSERT(0);
    }
  }
  {
    p_dev = NULL;
    usb_cfg = p_usb_cfg;
    for(i = 0; i < usb_cfg->usb_eth_support_cnt; i++)
    {
      usb_cfg->usb_eth_drv_attach[i](usb_cfg->p_usb_eth_drv_name[i]);
      memset(&ethcfg, 0, sizeof(ethernet_cfg_t));
      ethcfg.task_prio = p_usb_cfg->usb_eth_task_prio[i];
      ethcfg.stack_size = 4096;
      p_dev = (ethernet_device_t *)dev_find_identifier(p_dev, DEV_IDT_TYPE, SYS_DEV_TYPE_USB_ETHERNET_DEV);
      MT_ASSERT(p_dev != NULL);
      ret = dev_open(p_dev, (void *)&ethcfg);
      MT_ASSERT(ret == SUCCESS);
    }
  }
  return SUCCESS;
}

#ifndef WIN32
static void block_driver_attach(void)
{
  _usb_cfg_t usb_cfg = {0};
  usb_cfg.usb_eth_support_cnt = USB_ETH_SUPPORT_CNT;
  usb_cfg.usb_eth_drv_attach[0] = usb_eth_sr_attach;
  usb_cfg.p_usb_eth_drv_name[0] = "sr";
  usb_cfg.usb_eth_task_prio[0] = USB_ETH0_RECV_TASK_PRIORITY;
  usb_cfg.usb_eth_drv_attach[1] = usb_eth_asix_attach;
  usb_cfg.p_usb_eth_drv_name[1] = "asix";
  usb_cfg.usb_eth_task_prio[1] = USB_ETH1_RECV_TASK_PRIORITY;
  usb_cfg.host_cnt = USB_HOST_CNT;
  usb_cfg.select_usb_port_number = 1; // usb usb 1
  usb_cfg.if_support_hub = 0;              // 1 -disable usb
  usb_cfg.drv_attach = usb_mass_stor_attach;
  usb_cfg.bus0_task_prio = USB_BUS_TASK_PRIO;
  usb_cfg.us0_task_prio = USB_MASS_STOR_TASK_PRIO;
  usb_cfg.bus1_task_prio = USB_BUS1_TASK_PRIO;
  usb_cfg.us1_task_prio = USB_MASS_STOR1_TASK_PRIO;
  usb_cfg.usb_hub_tt_task_prio = USB_HUB_TT_TASK_PRIO;  
  _usb_init(&usb_cfg);
  OS_PRINTF("usb init done\n");
}
#endif

static int drv_nim_init(void)
{
  RET_CODE ret = ERR_FAILURE;
  void *p_dev = NULL;
  
#ifdef DTMB_PROJECT
{
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
}
#else
{
  extern void tuner_attach_tc2800(void);

  nim_config_t nim_cfg = {0};
  ret = ATTACH_DRIVER(NIM, m88dc2800, default, default);
  MT_ASSERT(ret == SUCCESS);
  tuner_attach_tc2800();
  
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
  MT_ASSERT(NULL != p_dev);
  nim_cfg.p_dem_bus = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"i2c_QAM");
  MT_ASSERT(NULL != nim_cfg.p_dem_bus);
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
  
  nim_cfg.tuner_loopthrough = 1;
  
  ret = dev_open(p_dev, &nim_cfg);

  MT_ASSERT(SUCCESS == ret);

  dev_io_ctrl(p_dev, (u32)NIM_IOCTRL_SET_TUNER_WAKEUP, 0);
}
#endif

  return ret;
}

#if 0
static void show_startup_iFram()
{
  RET_CODE ret = SUCCESS;

  void * p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_VDEC_VSB);
  void * p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_DISPLAY);

  u32 size = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID),START_LOGO_BLOCK_ID);
  u8 *addr = (u8 *)(dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), START_LOGO_BLOCK_ID));


    mtos_printk("$$$$$$$$$$ %s,%d,addr==%0x,size==%d\n\n",__FUNCTION__,__LINE__,addr,size);

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
#endif


static void drv_init_sctrl()
{
  RET_CODE ret = SUCCESS;
  void *p_dev_sctrl0 = NULL;
  void *p_dev_sctrl1 = NULL;
  void *p_dev_sctrl2 = NULL;

  /*!
  security
  */
  /*    sctrl   */
  sctrl_cfg_t sctrl_open_params = {0};
  sctrl_open_params.lock_mode = OS_LOCK_DEF;
  ret = sctrl_attach("sctrl_subch2");
  MT_ASSERT(ret == SUCCESS);
  p_dev_sctrl0 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"sctrl_subch2");
  ret = dev_open(p_dev_sctrl0, &sctrl_open_params);
  MT_ASSERT(SUCCESS == ret);
  ret = sctrl_attach("sctrl_subch3");
  MT_ASSERT(ret == SUCCESS);
  p_dev_sctrl1 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"sctrl_subch3");
  ret = dev_open(p_dev_sctrl1, &sctrl_open_params);
  MT_ASSERT(SUCCESS == ret);
  ret = sctrl_attach("sctrl_subch4");
  MT_ASSERT(ret == SUCCESS);
  p_dev_sctrl2 = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"sctrl_subch4");
  ret = dev_open(p_dev_sctrl2, &sctrl_open_params);
  MT_ASSERT(SUCCESS == ret);
  /* secure moudles init */
  ret = hal_secure_ModuleInit();
  MT_ASSERT(ret == SUCCESS);

}

#ifdef WIN32
void drv_init_win32(void)
{
  RET_CODE ret;
  void *p_dev = NULL;
  void *p_dev_i2c0 = NULL;
  void *p_dev_i2c1 = NULL;
  void *p_dev_i2cfp = NULL;
  void *p_dev_i2chdmi = NULL;
  void *p_dev_i2cqam = NULL;
  //void *p_dev_sctrl0 = NULL;
  //void *p_dev_sctrl1 = NULL;
  //void *p_dev_sctrl2 = NULL;
  drvsvc_handle_t *p_public_drvsvc = NULL;
  u32 *p_buf = NULL;
  u32 size = 0;
  u32 align = 0;
  // charsto_cfg_t charsto_cfg = {0};
  u32 interval = 0;
  dm_v2_init_para_t dm_para = {0};
  u32 dm_app_addr = 0;
  
  board_ethernet_cfg_t eth_cfg = {0};
  
  //this service will be shared by HDMI, Audio and Display driver
  p_buf = (u32 *)mtos_malloc(DRV_HDMI_TASK_STKSIZE);
  MT_ASSERT(NULL != p_buf);
  p_public_drvsvc = drvsvc_create(DRV_HDMI_TASK_PRIORITY,
  p_buf, DRV_HDMI_TASK_STKSIZE, 8);
  MT_ASSERT(NULL != p_public_drvsvc);
  
  
  /* GPE */
  ret = ATTACH_DRIVER(GPE_VSB, magic, default, default);
  MT_ASSERT(ret == SUCCESS);
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_GPE_VSB);
  MT_ASSERT(NULL != p_dev);
  ret = dev_open(p_dev, NULL);
  MT_ASSERT(SUCCESS == ret);
  
  /* vbi inserter */
  ret = ATTACH_DRIVER(VBI_INSERTER, magic, default, default);
  MT_ASSERT(ret == SUCCESS);
  
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VBI_INSERTER);
  MT_ASSERT(NULL != p_dev);
  
  ret = dev_open(p_dev, NULL);
  MT_ASSERT(SUCCESS == ret);
  /*JPEG*/
  ret = PDEC_SIM(PDEC,jazz,default,default);
  MT_ASSERT(ret == SUCCESS);
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PDEC);
  MT_ASSERT(NULL != p_dev);
  
  ret = dev_open(p_dev, NULL);
  MT_ASSERT(SUCCESS == ret);
  
  {
    nim_config_t nim_cfg = {0};
    ret = ATTACH_DRIVER(NIM, m88cs2200, default, default);
    MT_ASSERT(ret == SUCCESS);
    
    p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_NIM);
    MT_ASSERT(NULL != p_dev);
    
    init_nim_cfg(&nim_cfg);
    ret = dev_open(p_dev, &nim_cfg);
    MT_ASSERT(SUCCESS == ret);
  }
  
  
  s32_block_int();
  spiflash_cfg( );
  
  interval = mtos_ticks_get() - interval;
  OS_PRINTF("disp init  cost %d ticks ,%s %d \n",interval,__FUNCTION__,__LINE__);
  //init data manager....
  dm_para.flash_base_addr = get_flash_addr();
  dm_para.max_blk_num = DM_MAX_BLOCK_NUM;
  dm_para.task_prio = MDL_DM_MONITOR_TASK_PRIORITY;
  OS_PRINTF("disp init  cost %d ticks ,%s %d dm_para.task_prio = %d\n",interval,__FUNCTION__,__LINE__, dm_para.task_prio);
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
  
  //load ui resource
  load_ui_resource();
  
  //load customer sys config
  dm_load_customer_sys_cfg();
  
  //load customer hw config
  dm_load_customer_hw_cfg();
  hw_cfg =dm_get_hw_cfg_info();
  
  ret = ATTACH_DRIVER(UIO, magic, default, gpio);
  MT_ASSERT(ret == SUCCESS);
  uio_init();
  
  /* DMA */
  ret = hal_dma_init();
  MT_ASSERT(ret == SUCCESS);
  OS_PRINTF("\ndma init end");
  interval = mtos_ticks_get();
  
  ret = ATTACH_DRIVER(AUDIO_VSB, magic, default, default); 
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
  MT_ASSERT(NULL != p_dev);
  ret = dev_open(p_dev, NULL);
  MT_ASSERT(SUCCESS == ret);
  ret = ATTACH_DRIVER(VDEC, magic, default, default); 
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
  MT_ASSERT(NULL != p_dev);
  ret = dev_open(p_dev, NULL);
  
  interval = mtos_ticks_get() - interval;
  OS_PRINTF("vdec init  cost %d ticks ,%s %d \n",interval,__FUNCTION__,__LINE__);
  
  ret = ATTACH_DRIVER(DMX, magic, default, default);
  MT_ASSERT(ret == SUCCESS);
  
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  MT_ASSERT(NULL != p_dev);
  
  ret = dev_open(p_dev, NULL);
  MT_ASSERT(SUCCESS == ret);
  
  interval = mtos_ticks_get() - interval;
  OS_PRINTF("dmx init  cost %d ticks ,%s %d \n",interval,__FUNCTION__,__LINE__);
  
  /*disp*/
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
    //layer_cfg_t sub_vs_cfg = {0};
    //display driver attach
    ret = ATTACH_DRIVER(DISP, magic, default, default);
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
        osd1_vs_cfg.odd_mem_start = OSD1_VSCALER_BUFFER_ADDR;
        osd1_vs_cfg.odd_mem_end = OSD1_VSCALER_BUFFER_ADDR + OSD1_VSCALER_BUFFER_SIZE;
      }
      if(disp_cfg.b_vscale_sub == TRUE)
      {
        sub_vs_cfg.odd_mem_start = SUB_VSCALER_BUFFER_ADDR;
        sub_vs_cfg.odd_mem_end = SUB_VSCALER_BUFFER_ADDR + SUB_VSCALER_BUFFER_SIZE;
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
    
    disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
    disp_component_set_type(p_disp, COMPONENT_GRP1,COLOR_YUV);
    disp_component_onoff(p_disp, COMPONENT_GRP1, TRUE);
    
    disp_rect.w = 1280;
    disp_rect.h = 720;
    disp_set_graphic_size(p_disp,&disp_rect);
    OS_PRINTK("disp_set_graphic_size w %d h %d\n",disp_rect.w,disp_rect.h);
  }
}
#else
static void disp_vdac_config(void * p_disp)
{
  vdac_info_t v_dac_info = hw_cfg.vdac_info;
  
  OS_PRINTF("[disp_vdac_config] v_dav_info = %d \n",v_dac_info);
  
  switch(v_dac_info)
  {
    case VDAC_CVBS_RGB:
    {
      OS_PRINTF("\nVDAC_CVBS_RGB\n");
      disp_set_dacmode(p_disp,DISP_DAC_CVBS_RGB);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, FALSE);
      //disp_component_set_type(p_disp, COMPONENT_GRP0,COLOR_CVBS_RGB);
      //disp_component_onoff(p_disp, COMPONENT_GRP0, TRUE);
    }
    break;
    
    case VDAC_SIGN_CVBS:
    {
      OS_PRINTF("\nVDAC_SIGN_CVBS\n");
      disp_set_dacmode(p_disp,DISP_DAC_SING_CVBS);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      //disp_component_set_type(p_disp, COMPONENT_GRP1,COLOR_YUV);
      //disp_component_onoff(p_disp, COMPONENT_GRP1, TRUE);
      //disp_dac_onoff(p_disp, DAC_1,  FALSE);
      //disp_dac_onoff(p_disp, DAC_2,  FALSE);

    }
    break;
    
    case VDAC_SIGN_CVBS_LOW_POWER:
    {
      OS_PRINTF("\nVDAC_SIGN_CVBS_LOW_POWER\n");
      disp_set_dacmode(p_disp,DISP_DAC_SING_CVBS_LOW_POWER);
      //disp_cvbs_onoff(p_disp, CVBS_GRP3, TRUE);
      //disp_dac_onoff(p_disp, DAC_0,  FALSE);
      //disp_dac_onoff(p_disp, DAC_1,  FALSE);
      //disp_dac_onoff(p_disp, DAC_2,  FALSE);
    }
    break;
    
    case VDAC_DUAL_CVBS:
    {
      OS_PRINTF("\nVDAC_DUAL_CVBS\n");
      disp_set_dacmode(p_disp,DISP_DAC_DULE_CVBS);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      //disp_cvbs_onoff(p_disp, CVBS_GRP1, FALSE);
      //disp_cvbs_onoff(p_disp, CVBS_GRP2, FALSE);
      //disp_cvbs_onoff(p_disp, CVBS_GRP3, TRUE);
    }
    break;
    
    case VDAC_CVBS_SVIDEO:
    {
      OS_PRINTF("\nVDAC_CVBS_SVIDEO\n");
      disp_set_dacmode(p_disp,DISP_DAC_CVBS_SVIDEO);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      //disp_svideo_onoff(p_disp, SVIDEO_GRP0, TRUE);
      //disp_cvbs_onoff(p_disp, CVBS_GRP2, TRUE);
      //disp_cvbs_onoff(p_disp, CVBS_GRP3, TRUE);
    }
    break;
    
    case VDAC_CVBS_YPBPR_SD:
    {
      OS_PRINTF("\nVDAC_CVBS_YPBPR_SD\n");
      disp_set_dacmode(p_disp,DISP_DAC_CVBS_YPBPR_SD);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      //disp_component_set_type(p_disp, COMPONENT_GRP0, COLOR_YUV);
      //disp_component_onoff(p_disp, COMPONENT_GRP0, TRUE);
    }
    break;
    
    case VDAC_CVBS_YPBPR_HD:
    {
      OS_PRINTF("\nVDAC_CVBS_YPBPR_HD\n");
      disp_set_dacmode(p_disp,DISP_DAC_CVBS_YPBPR_HD);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      //disp_component_set_type(p_disp, COMPONENT_GRP1,COLOR_YUV);
      //disp_component_onoff(p_disp, COMPONENT_GRP1, TRUE);
    }
    break;
    
    default:
    {
      OS_PRINTF("\nVDAC setting default, CVBS + HD COMPONENT \n");
      disp_set_dacmode(p_disp,DISP_DAC_CVBS_YPBPR_HD);
      //disp_cvbs_onoff(p_disp, CVBS_GRP0, TRUE);
      //disp_component_set_type(p_disp, COMPONENT_GRP1,COLOR_YUV);
      //disp_component_onoff(p_disp, COMPONENT_GRP1, TRUE);
    }
    break;
  }
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
  dm_load_customer_hw_cfg();
  hw_cfg =dm_get_hw_cfg_info();
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

  ret = ATTACH_DRIVER(VDEC, concerto, default, default);
  MT_ASSERT(ret == SUCCESS);

  p_video = (void *)dev_find_identifier(NULL
   , DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);
  MT_ASSERT(NULL != p_video);

  vdec_cfg.is_autotest = FALSE;
  ret = dev_open(p_video, &vdec_cfg);
  MT_ASSERT(SUCCESS == ret);
  OS_PRINTF("Init Vdec success\n");

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
        osd1_vs_cfg.odd_mem_start = OSD1_VSCALER_BUFFER_ADDR;
        osd1_vs_cfg.odd_mem_end = OSD1_VSCALER_BUFFER_ADDR + OSD1_VSCALER_BUFFER_SIZE;
    }
    if(disp_cfg.b_vscale_sub == TRUE)
    {
      sub_vs_cfg.odd_mem_start = SUB_VSCALER_BUFFER_ADDR;
      sub_vs_cfg.odd_mem_end = SUB_VSCALER_BUFFER_ADDR + SUB_VSCALER_BUFFER_SIZE;
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
  
  disp_cfg.b_uboot_uninit = FALSE;
  
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
  
  disp_vdac_config(p_disp);
  
  disp_rect.w = 1280;
  disp_rect.h = 720;
  disp_set_graphic_size(p_disp,&disp_rect);
  OS_PRINTK("disp_set_graphic_size w %d h %d\n",disp_rect.w,disp_rect.h);

  return ret;
}

static int drv_gpe_init(void)
{
  RET_CODE ret = ERR_FAILURE;
  void *p_gpe_c = NULL;

#if GPE_USE_HW
  /* GPE */
  OS_PRINTK("GPE_USE_HW\n");
  ret = ATTACH_DRIVER(GPE_VSB, concerto, default, default);
  MT_ASSERT(SUCCESS == ret);

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

static void drv_cfg_init(void)
{
  //load ui resource
  load_ui_resource();

  //load customer sys config
  dm_load_customer_sys_cfg();
}

static int drv_uio_init(void)
{
  RET_CODE ret = ERR_FAILURE;
  
  ret = ATTACH_DRIVER(UIO, concerto, default, concerto);
  MT_ASSERT(ret == SUCCESS);
  uio_init();

  return ret;
}

static int drv_audio_init(const drvsvc_handle_t* p_public_drvsvc)
{
  /* AUDIO */
  RET_CODE ret = ERR_FAILURE;
  void *p_dev = NULL;
  aud_cfg_t aud_cfg = {0};
  u32 size = 0;
  u32 align = 0;

  ret = ATTACH_DRIVER(AUDIO_VSB, concerto, default, default);
  MT_ASSERT(ret == SUCCESS);
  
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);
  MT_ASSERT(NULL != p_dev);
  
  //aud_cfg.p_drvsvc = p_public_drvsvc;
  aud_cfg.stack_size = 4 * KBYTES;
  aud_cfg.task_prio = DRV_HDMI_TASK_PRIORITY;
  aud_cfg.lock_mode = OS_MUTEX_LOCK;
  
  aud_cfg.p_drvsvc = (void *)p_public_drvsvc;
  ret = dev_open(p_dev, &aud_cfg);
  MT_ASSERT(SUCCESS == ret);

  aud_stop_vsb(p_dev);
  
  aud_get_buf_requirement_vsb(p_dev, &size, &align);
  MT_ASSERT(AUDIO_FW_CFG_SIZE >= size);
  aud_set_buf_vsb(p_dev, AUDIO_FW_CFG_ADDR, size);

  aud_output_spdif_onoff_vsb(p_dev, FALSE);
	
  return SUCCESS;
}

static int drv_smc_init(const drvsvc_handle_t* p_public_drvsvc)
{
  #ifdef ENABLE_CA
  RET_CODE ret = ERR_FAILURE;
  void *p_dev = NULL;
  scard_open_t smc_open_params ={0};
  u32 reg = 0, val = 0;

  /* SMC */
  // PINMUX
  reg = R_IE1_SEL;
  val = *((volatile u32 *)reg);
  val |= 0x1D << 10;
  *((volatile u32 *)reg) = val;

  reg = R_PIN5_SEL;
  val = *((volatile u32 *)reg);
  val &= ~0x3FF;
  *((volatile u32 *)reg) = val;

  // SMC device 0
  ret = smc_attach_concerto("concerto_smc0");
  MT_ASSERT(ret == SUCCESS);
  p_dev = (scard_device_t *) dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"concerto_smc0");
  MT_ASSERT(NULL != p_dev);
  smc_open_params.p_drvsvc = (void *)p_public_drvsvc;
  smc_open_params.smc_id = SMC0_ID;
  smc_open_params.smc_op_pri = DRV_SMC_TASK_PRIORITY;
  smc_open_params.phyclk = 0;
  smc_open_params.convention = 0;
  smc_open_params.convt_value = 0;
  smc_open_params.detect_pin_pol =  0;
  smc_open_params.vcc_enable_pol =  0;
  OS_PRINTF("SMC detect_pin_pol:%d,vcc_enable_pol:%d\n",smc_open_params.detect_pin_pol,smc_open_params.vcc_enable_pol);
  smc_open_params.iopin_mode = 1; // external pull-up 5V
  smc_open_params.rstpin_mode = 1; // external pull-up 5V
  smc_open_params.clkpin_mode = 1; // external pull-up 5V

  smc_open_params.read_timeout = 2500;

  smc_open_params.smc_op_stksize = 4096;
  ret = dev_open(p_dev, &smc_open_params);
  MT_ASSERT(SUCCESS == ret);

  OS_PRINTF("\nSmart Card 0 Dev Open\n");
  #endif

  return SUCCESS;
}

static int drv_vbi_init(void)
{
  // vbi
  void *p_dev = NULL;
  vbi_cfg_t vbi_cfg = {0};
  RET_CODE ret = ERR_FAILURE;
  
  ATTACH_DRIVER(VBI_INSERTER, concerto, default, default);
  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VBI_INSERTER);
  MT_ASSERT(NULL != p_dev);
  OS_PRINTK("find vbi\n");
  vbi_cfg.vbi_pes_buf_addr = VBI_BUF_ADDR;
  vbi_cfg.vbi_pes_buf_size = VBI_BUF_SIZE;
  ret = dev_open(p_dev, &vbi_cfg);
  MT_ASSERT(SUCCESS == ret);
  OS_PRINTK("vbi ATTACH_DRIVER\n");

  return ret;
}

static int drv_low_power_manager_init(void)
{
  void *p_dev_lpm = NULL;
  lpower_cfg_t lpm_cfg = {0};
  RET_CODE ret;

  ret = lpower_concerto_attach("standby");
  p_dev_lpm = dev_find_identifier(NULL, DEV_IDT_NAME, (u32)"standby");
  MT_ASSERT(ret == SUCCESS);
  ret = dev_open(p_dev_lpm, &lpm_cfg);
  MT_ASSERT(SUCCESS == ret);

  return ret;
}

static int drv_network_init()
{
  RET_CODE ret;
  
  {
    BOX_INFO_T box_info={0};
    box_info.custom_id = NET_CUSTOMER_ID;
    set_the_box_hw_info(&box_info) ;
  }

  {
    ethernet_cfg_t ethcfg = {0};
    ethcfg.tcp_ip_task_prio = ETH_NET_TASK_PRIORITY;
    ret = lwip_stack_init(&ethcfg);
  }
 
#if ENABLE_ETH
  {
    board_ethernet_cfg_t eth_cfg = {0};
    eth_cfg.drv_attach = ethernet_concerto_attach;
    eth_cfg.p_dev_name = "mt_eth";  
    ret = _ethernet_init(&eth_cfg);
    OS_PRINTF("ethernet init done\n");
  }
#endif

#if ENABLE_8188_SUPPORT
  _wifi_init();
#endif

#if ENABLE_5370_SUPPORT
  _ra0_wifi_init();
#endif

#if ENABLE_7601_SUPPORT
   _rt7601_wifi_init();
 #endif
  
  pppInit();

 // gprs_init();
  //g3_dongle_init();
  register_rtmp_stream();
  register_rtmp_protocol(0);
  register_rtsp_stream();
  
  return ret;
}

static void drv_mute_contrl()
{
  //静音电路的gpio置高，cvbs输出有声?
  gpio_io_enable(62, TRUE);
  gpio_set_dir(62, GPIO_DIR_OUTPUT);
  gpio_set_value(62, GPIO_LEVEL_HIGH);
}

static void drv_flash_protect()
{
  ap_charsto_init();

  #ifdef FLASH_16M
  ap_charsto_set_protect(PRT_LOWER_1_4);
  #else
  ap_charsto_set_protect(PRT_UNPROT_ALL);
  //ap_charsto_set_protect(PRT_LOWER_7_8);
  #endif
}

#ifdef SUPPORT_PUTOUT_SQUARE_WARE
#define SQUARE_WARE_GPIO			(69)
static BOOL monitor_state	= TRUE;

void square_wave_set_monitor_state(BOOL flag)
{
	monitor_state = flag;
	DEBUG(MAIN,INFO,"monitor = %d \n",monitor_state);
}


static void square_wave_timer_cb(void *p_data)
{
	static BOOL flag = FALSE;
	gpio_io_enable(SQUARE_WARE_GPIO, TRUE);
	gpio_set_dir(SQUARE_WARE_GPIO, GPIO_DIR_OUTPUT);

	while(1)
	{
		if(monitor_state)
		{
			if(flag == TRUE)
			{
				gpio_set_value(SQUARE_WARE_GPIO, GPIO_LEVEL_LOW);
				flag = FALSE;
			}
			else
			{
				gpio_set_value(SQUARE_WARE_GPIO, GPIO_LEVEL_HIGH);
				flag = TRUE;
			}
		}
		mtos_task_sleep(50);
	}

}
static RET_CODE square_wave_init(void)
{
    s32 err = 0;
    u32 *p_stack = NULL;

    OS_PRINTF("square_wave_init start! \n");
    p_stack = (u32 *)mtos_malloc(16 * 1024);
    MT_ASSERT(p_stack != NULL);

    err = mtos_task_create((u8 *)"SQUARE_WAVE",
            square_wave_timer_cb,
            NULL,
            SQUARE_WARE_TASK_PRIORITY,
            p_stack,
            16 * 1024);

    if (!err)
    {
      OS_PRINTF("[%s]:%d creat timer failer  error = 0x%08x!\n",__FUNCTION__,__LINE__, err);
      return ERR_FAILURE;
    }
	OS_PRINTF("[%s]:%d SQUARE_WARE_TASK_PRIORITY = %d\n",__FUNCTION__,__LINE__,SQUARE_WARE_TASK_PRIORITY);
	return SUCCESS;
}
#endif

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

//mtos_task_sleep(5000);
  
  ret = drv_hdmi_init(p_public_drvsvc);
  MT_ASSERT(ret == SUCCESS);

  ret = drv_dmx_init();
  MT_ASSERT(ret == SUCCESS);

  ret = drv_video_init();
  MT_ASSERT(ret == SUCCESS);

  ret = drv_display_init();
  MT_ASSERT(ret == SUCCESS);

 // mtos_task_sleep(1500);
  /*show iFrame*/
  //show_startup_iFram( );

  ret = drv_gpe_init();
  MT_ASSERT(ret == SUCCESS);
  ret = drv_jpeg_init();
  MT_ASSERT(ret == SUCCESS);

  drv_cfg_init();

  ret = drv_uio_init();
  MT_ASSERT(ret == SUCCESS);

  /*check barcode*/
  //sn_client_process();

  ret = drv_audio_init(p_public_drvsvc);
  MT_ASSERT(ret == SUCCESS);

  ret = drv_nim_init();
  MT_ASSERT(ret == SUCCESS);

  ret = drv_smc_init(p_public_drvsvc);
  MT_ASSERT(ret == SUCCESS);

  ret = drv_vbi_init();
  MT_ASSERT(ret == SUCCESS);

  /* secure moudles init */
  drv_init_sctrl();

  ret = drv_low_power_manager_init();
  MT_ASSERT(ret == SUCCESS);

  drv_network_init();

  /*board config */
  drv_mute_contrl();
  drv_flash_protect();
}
#endif

void register_dvb_tables(void)
{
  dvb_register_t reg_param = {0};
  dvb_t *p_dvb = NULL;

  p_dvb = class_get_handle_by_id(DVB_CLASS_ID);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_ecm;
  reg_param.request_proc = request_ecm;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_ECM;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_pat;
  reg_param.request_proc = request_pat;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_PAT;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_sdt;
  reg_param.request_proc = request_sdt;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_SDT_ACTUAL;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_nit;
  reg_param.request_proc = request_nit;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_NIT_ACTUAL;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_pmt_1;
  reg_param.request_proc = request_pmt;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_PMT;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_ts_packet;
  reg_param.request_proc = request_ts_packet;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = MPEG_TS_PACKET;
  p_dvb->register_table(p_dvb, &reg_param);

  //EIT multi-sec mode request
  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_eit;
  reg_param.request_proc = request_eit_multi_mode;
  reg_param.req_mode = DATA_MULTI;
  reg_param.table_id = DVB_TABLE_ID_EIT_ACTUAL;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_eit;
  reg_param.request_proc = request_eit_multi_mode;
  reg_param.req_mode = DATA_MULTI;
  reg_param.table_id = DVB_TABLE_ID_EIT_SCH_ACTUAL;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_eit;
  reg_param.request_proc = request_eit_multi_mode;
  reg_param.req_mode = DATA_MULTI;
  reg_param.table_id = DVB_TABLE_ID_EIT_SCH_ACTUAL_51;
  p_dvb->register_table(p_dvb, &reg_param);

  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_bat;
  reg_param.request_proc = request_bat_multi_mode;
  reg_param.req_mode = DATA_MULTI;
  reg_param.table_id = DVB_TABLE_ID_BAT;
  p_dvb->register_table(p_dvb, &reg_param);

  //dsmcc msg section:single
  reg_param.free_proc = NULL;
  reg_param.parse_proc = parse_dsmcc_dsi_dii_section;
  reg_param.request_proc = request_dsmcc_dsi_dii_section;
  reg_param.req_mode = DATA_SINGLE;
  reg_param.table_id = DVB_TABLE_ID_DSMCC_MSG;
  p_dvb->register_table(p_dvb, &reg_param);

}

#ifdef ENABLE_CA
extern void cas_manager_policy_init(void);
#endif

u8 *p_dvb2_mem = NULL;

extern  u8 mount_ramfs();

void middleware_init(void)
{
  handle_t dm_handle = NULL;
  dvb_t *p_dvb = NULL;
  u32 size = 0;
  u8 *p_buf = NULL;
  eva_init_para_t eva_para = {0};
  pnp_svc_init_para_t pnp_param = {0};

  mdl_init();

  eva_para.debug_level = E_PRINT_ERROR;
  eva_para.eva_sys_task_prio_start = EVA_SYS_TASK_PRIORITY_START;
  eva_para.eva_sys_task_prio_end = EVA_SYS_TASK_PRIORITY_END;
  eva_init(&eva_para);

  eva_filter_factory_init();
#if ENABLE_MUSIC_PICTURE
  eva_register_filter(FILE_SOURCE_FILTER, fsrc_filter_create);
  eva_register_filter(SUBT_STATION_FILTER, subt_station_filter_create);
  eva_register_filter(JPEG_FILTER, pic_filter_create);
  eva_register_filter(PIC_RENDER_FILTER, pic_render_filter_create);
  eva_register_filter(MP3_TRANSFER_FILTER, mp3_transfer_filter_create);
  eva_register_filter(MP3_DECODE_FILTER, mp3_decode_filter_create);
  eva_register_filter(FILE_SINK_FILTER, fsink_filter_create);
  eva_register_filter(TS_PLAYER_FILTER, ts_player_filter_create);
  eva_register_filter(AV_RENDER_FILTER, av_render_filter_create);
  eva_register_filter(RECORD_FILTER, record_filter_create);
  eva_register_filter(LRC_PARSE_FILTER, lrc_filter_create);
  eva_register_filter(STR_FILTER, str_filter_create);
#endif

#ifdef IMPL_NEW_EPG
   eva_register_filter(EPG_FILTER, epg_filter_create);
#endif
  eva_register_filter(DEMUX_FILTER, dmx_filter_create);
  eva_register_filter(OTA_FILTER, ota_filter_create);
  eva_register_filter(FLASH_SINK_FILTER,flash_sink_filter_create);
#if ENABLE_NETWORK
  eva_register_filter(NET_SOURCE_FILTER, net_src_filter_create);
  eva_register_filter(WEATHER_FILTER, weather_filter_create);
  eva_register_filter(GOOGLE_MAP_FILTER, google_map_filter_create);
  eva_register_filter(VOD_FILTER, vod_filter_create);
  eva_register_filter(ALBUMS_FILTER, albums_filter_create);
  eva_register_filter(NEWS_FILTER, news_filter_create);
  eva_register_filter(NET_UPG_FILTER, net_upg_filter_create);
  eva_register_filter(FLASH_SINK_FILTER, flash_sink_filter_create);
  eva_register_filter(NET_MUSIC_FILTER, net_music_filter_create);

#ifndef WIN32
  eva_register_filter(NET_EXTERN_FILTER, net_extern_filter_create);
#endif

#endif

  dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(dm_handle != NULL);

  // Whether cache is enabled, it is up to the configuration from UI
  OS_PRINTF("dm set cache\n");
  dm_set_cache(dm_handle, IW_TABLE_BLOCK_ID, MAX_TABLE_CACHE_SIZE);
  dm_set_cache(dm_handle, IW_VIEW_BLOCK_ID, MAX_VIEW_CACHE_SIZE);
  OS_PRINTF("dm set cache is done\n");
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
    nc_cfg.auto_iq = FALSE;
    nc_cfg.mutex_type = OS_MUTEX_LOCK;
    nc_cfg.wait_ticks = 100;
    nc_cfg.maxticks_cnk=500;
    nc_svc_init(&nc_cfg, NULL);
  }
  //nc_enable_lnb_check(class_get_handle_by_id(NC_CLASS_ID), 0, TRUE);
{
    u8 tuner_id = 0, ts_in = 0;  
    class_handle_t nc_handle = NULL;
    nc_handle = class_get_handle_by_id(NC_CLASS_ID);
    nc_get_main_tuner_ts(nc_handle, &tuner_id, &ts_in); 
    dvb_init_1(MDL_DVB_TASK_PRIORITY, MDL_DVB_TASK_STKSIZE,ts_in);
}

//  nvod_mosaic_buf_init(30 * KBYTES);
  
  p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  register_dvb_tables();
  size = p_dvb->get_mem_size(p_dvb, TRUE, MAX_FILTER_NUM, 128);


  p_dvb2_mem = mtos_malloc(10);
  MT_ASSERT(p_dvb2_mem != NULL);
  memset(p_dvb2_mem, 0x3b, 10);
  mem_show();


  p_buf = mtos_align_malloc(size, 8);
  p_dvb->start(p_dvb, p_buf, size, TRUE, MAX_FILTER_NUM, 128);

  {
    m_svc_init_para_t m_init_param = {0};
    
    m_init_param.nstksize = MDL_MONITOR_TASK_STKSIZE;
    m_init_param.service_prio = MDL_MONITOR_TASK_PRIORITY;
    dvb_monitor_service_init(&m_init_param);
  }

  #if ENABLE_NETWORK
  #ifndef WIN32
  {
    net_svc_init_para_t net_svc_cfg;
    net_svc_t *p_net_svc = NULL;
    service_t *p_net = NULL;
    net_svc_cfg.service_prio = MDL_NET_SVC_TASK_PRIORITY;
    net_svc_cfg.nstksize = MDL_NET_SVC_TASK_STKSIZE;
    net_svc_cfg.is_ping_url = TRUE;
    net_svc_cfg.enable_auto_ping = FALSE;
    net_svc_cfg.monitor_interval= 2000;
    strcpy(net_svc_cfg.param.ping_url_1, "www.baidu.com");
    net_service_init(&net_svc_cfg);
    p_net_svc = class_get_handle_by_id(NET_SVC_CLASS_ID);
    p_net = p_net_svc->get_svc_instance(p_net_svc, APP_FRAMEWORK);
    ui_set_net_svc_instance(p_net);
  }
#endif
#endif
  net_source_svc_init(NET_SOURCE_SVC_PRIO, 32 * KBYTES);
  
#if ENABLE_MUSIC_PICTURE
  {
    dmm_init_para_t dmm_para = {0};
    dmm_para.channel_num = 128;
    dmm_init(&dmm_para);
  }
#endif
  db_dvbs_init();
#ifdef ENABLE_CA
  gpio_io_enable(64, TRUE);
  gpio_set_dir(64, GPIO_DIR_OUTPUT);
#ifdef QILIAN_CA
  gpio_set_value(64, GPIO_LEVEL_LOW);
#else
  gpio_set_value(64, GPIO_LEVEL_HIGH); 
#endif

  cas_manager_policy_init();
  #endif
  sys_status_init();

  db_cnpl_initialize();
  db_plht_sort_time_table();
 
  //time initial
  {
    BOOL p_status = FALSE;
    standby_time_t get_sty_time;
    utc_time_t p_time,p_temp_time;
    void *p_dev_power = dev_find_identifier(NULL,
                                            DEV_IDT_TYPE,
                                            SYS_DEV_TYPE_POW);

    sys_status_get_status(BS_UNFIRST_UNPOWER_DOWN, &p_status);
    sys_status_get_utc_time(&p_time);
    time_init(&p_time);
    sys_status_set_time_zone();
    if (0) //Temporary disable POW as driver not support.
    {
      RET_CODE ret;
      utc_time_t p_loc_time;
      u32 cmd = GET_STANDBY_TIME;

      ret = dev_io_ctrl(p_dev_power, cmd, (u32) & get_sty_time);
      MT_ASSERT(ret == SUCCESS);

      sys_status_get_utc_time(&p_time);
      time_to_local(&p_time, &p_loc_time);

      memcpy(&p_temp_time,&p_loc_time,sizeof(utc_time_t));
      p_temp_time.hour = get_sty_time.cur_hour;
      p_temp_time.minute = get_sty_time.cur_min;
      p_temp_time.second = get_sty_time.cur_sec;

      time_up(&p_temp_time,get_sty_time.pass_day * 24
	  	*60 * 60);

      if(time_cmp(&p_temp_time, &p_loc_time,FALSE) > 0)
      {
      if((time_dec(&p_temp_time, &p_loc_time)  ==  sys_status_get_standby_time_out() / 1000)
                      && sys_status_get_standby_time_out())
       {
       sys_status_set_status(BS_IS_TIME_OUT_WAKE_UP, TRUE);
	}
	else
       sys_status_set_status(BS_IS_TIME_OUT_WAKE_UP, FALSE);
       sys_status_save();
      }
      p_loc_time.hour = get_sty_time.cur_hour;
      p_loc_time.minute = get_sty_time.cur_min;
      p_loc_time.second = get_sty_time.cur_sec;

      time_to_gmt(&p_loc_time, &p_time);
      sys_status_set_utc_time(&p_time);
      time_set(&p_time);
    }
    sys_status_set_status(BS_UNFIRST_UNPOWER_DOWN, FALSE);
    sys_status_save();
    //set time zone(should be called after time initial.)
  }

  {
    avc_cfg_t avc_cfg = {0};
#ifdef WIN32
    avc_cfg.disp_fmt = VID_SYS_PAL;
    avc_cfg.disp_chan = DISP_CHANNEL_SD;
#else
    avc_cfg.disp_fmt = VID_SYS_1080I_50HZ;
    avc_cfg.disp_chan = DISP_CHANNEL_HD;
#endif
    avc_init_1(&avc_cfg);
  }
#ifndef IMPL_NEW_EPG
  epg_data_init4();
#endif
#ifdef NVOD_ENABLE
  nvod_data_init();
#endif
  link_gb2312_maptab(NULL, 0);
#ifndef WIN32
  block_driver_attach();
  ufs_dev_init();   //register callback for block layer
  mount_ramfs();
#endif
#ifdef WIN32
  vfs_win32_init();
#else
#if ENABLE_NETWORK
  vfs_uri_init();
#else
  vfs_mt_init();
#endif
#endif

  file_list_init2(MAX_FILE_COUNT);
  {
    pnp_param.nstksize = MDL_PNP_SVC_TASK_STKSIZE;
    pnp_param.service_prio = MDL_PNP_SVC_TASK_PRIORITY;
    pnp_service_init(&pnp_param);
  }

  #ifdef AUPPORT_TS_RECORD
  mul_pvr_init();
  #endif

#ifdef WIN32
  {
    vfs_mount('C');
  }
#endif

#if ENABLE_NETWORK
  //mul_net_music_init();

 // mul_vod_init();

 // mul_albums_init();

//  mul_news_init();

  mul_net_upg_init();
#endif
}
int EMON_Initialise(void);

void app_init(void)
{
  ap_frm_init_param_t param = {0};
  handle_t ap_frm_handle = NULL;
  u32 ap_frm_msgq_id = 0;

  app_info_t app_array[] =
  {
    {APP_PLAYBACK, construct_ap_playback_1(construct_pb_policy()), "app playback", AP_PLAYBACK_TASK_PRIORITY, AP_PLAYBACK_TASK_STKSIZE},
#ifdef ENABLE_CA
    {APP_CA, construct_ap_cas(construct_cas_policy()), "app ca", AP_CA_TASK_PRIORITY, AP_CA_TASK_STKSIZE},
#endif
    {APP_OTA, construct_virtual_nim_filter(APP_OTA,0), "app ota", AP_OTA_TASK_PRIORITY, AP_OTA_TASK_STKSIZE},
    {APP_UIO, construct_ap_uio(construct_ap_uio_policy()), "app uio", AP_UIO_TASK_PRIORITY, AP_UIO_TASK_STKSIZE},
#ifndef IMPL_NEW_EPG
    {APP_EPG, construct_ap_epg4(construct_epg_policy4()), "app epg", AP_EPG_TASK_PRIORITY,
     AP_EPG_TASK_STKSIZE},
#endif
    {APP_SIGNAL_MONITOR, construct_ap_signal_monintor(construct_sig_mon_policy()), "app singal monitor", AP_SIGN_MON_TASK_PRIORITY, AP_SIGN_MON_TASK_STKSIZE},
    {APP_TIME, construct_ap_time(construct_time_policy()), "app time", AP_TIME_TASK_PRIORITY, AP_TIME_TASK_STKSIZE},
    {APP_SCAN, construct_ap_scan(), "app scan", AP_SCAN_TASK_PRIORITY, AP_SCAN_TASK_STKSIZE},
    {APP_USB_UPG, construct_ap_usb_upg(construct_usb_upg_policy()), "app usb upgrade", AP_USB_UPG_TASK_PRIORITY, AP_USB_UPGRADE_TASK_STKSIZE},
#if defined DESAI_56_CA || defined DESAI_52_CA
	{APP_NIT, construct_ap_nit(), (u8 *)"app nit", AP_NIT_TASK_PRIORITY, AP_NIT_TASK_STKSIZE},
#endif
#if ENABLE_BISS_KEY
	{APP_TWIN_PORT,  construct_ap_twin_port(construct_twin_policy()), "app twin port", AP_TWIN_PORT_TASK_PRIORITY, AP_TWIN_PORT_TASK_STKSIZE},
#endif
    {APP_UPG,  construct_ap_upg(construct_upg_policy()), "app upg", AP_UPGRADE_TASK_PRIORITY, AP_UPGRADE_TASK_STKSIZE},
  };
  param.p_implement_policy = construct_ap_frm_policy();
  param.p_app_array = app_array;
  param.num_of_apps = sizeof(app_array) / sizeof(app_info_t);
  param.stack_size = AP_FRM_TASK_STKSIZE;
  param.priority = AP_FRM_TASK_PRIORITY;
  param.ui_priority = UI_FRM_TASK_PRIORITY;
  param.ap_highest_priority = AP_HIGHEST_TASK_PRIORITY;
  ap_frm_handle = ap_frm_init(&param, &ap_frm_msgq_id);
  mdl_set_msgq(class_get_handle_by_id(MDL_CLASS_ID), ap_frm_msgq_id);

#if defined DESAI_56_CA || defined DESAI_52_CA
  cmd_t cmd = {0};

  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_NIT;
  ap_frm_do_command(APP_FRAMEWORK, &cmd);

  dvb_t *p_dvb = class_get_handle_by_id(DVB_CLASS_ID);
  service_t *p_tmp_svc = p_dvb->get_svc_instance(p_dvb, APP_NIT);
  dvb_request_t dvb_req = {0};

  dvb_req.table_id = DVB_TABLE_ID_NIT_ACTUAL;
  dvb_req.req_mode = DATA_SINGLE;
  dvb_req.para1 = DVB_TABLE_ID_NIT_ACTUAL;
  dvb_req.para2 = 0xFFFF;
  p_tmp_svc->do_cmd(p_tmp_svc, DVB_REQUEST,(u32)&dvb_req, sizeof(dvb_request_t));
#endif

#ifndef WIN32
  //EMON_Initialise();
#endif
}

#if ENABLE_WATCH_DOG
static void _feed_dog(u32 context)
{
	hal_watchdog_feed();
}
#endif
#ifdef AUTO_FACTORY_DEFAULT
//extern void check_factory_key(void);
#endif

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

extern u32 attach_ipcfw_fun_set_concerto(ipc_fw_fun_set_t * p_funset);
BOOL  xn_checkout_chip_id(void);
void ap_init(void)
{
  u32 interval = 0;
//  dvbc_lock_t main_tp = {0};
  
#ifndef WIN32
  hal_dcache_invalidate((void *)&g_ipcfw_f,sizeof(ipc_fw_fun_set_t));
  attach_ipcfw_fun_set_concerto(&g_ipcfw_f);
  ap_ipc_init(32);   // ap ipc fifo create
#endif
  xn_debug_init();

  {
    //board_config();
    mem_cfg(MEMCFG_T_NORMAL);
    customer_config_init();
    mtos_printk("mem_cfg\n");
    interval = mtos_ticks_get();

#ifdef WIN32
    drv_init_win32();
#else
    drv_init_concerto();
#endif

    util_init();
    mtos_printk("util_init\n");

    //init watchdog
#ifndef WIN32
    #if ENABLE_WATCH_DOG
	#ifdef SHUSHITONG_CA
    hal_watchdog_init(50000);
    hal_watchdog_enable();
    mtos_timer_create(5000, _feed_dog, 0, TRUE);
	#else
    hal_watchdog_init(10000);
    hal_watchdog_enable();
    mtos_timer_create(2000, _feed_dog, 0, TRUE);
	#endif
    #endif
#endif

    interval = mtos_ticks_get() - interval;
    mtos_printk("driver init time = %ld ms\n", interval * 10);
    mtos_printk("drv_init\n");

    interval = mtos_ticks_get();
    middleware_init();
    interval = mtos_ticks_get() - interval;
    mtos_printk("mdl init time = %ld ms\n", interval * 10);
    mtos_printk("mid_init\n");

#ifdef CHECK_CHIP_ID
   while(!xn_checkout_chip_id())
   {
   	mtos_printk("chip set have no auth!\n");
	mtos_task_delay_ms(500);
   }
#endif	
#ifdef SUPPORT_PUTOUT_SQUARE_WARE
	square_wave_init();
#endif

  app_init();
  OS_PRINTF("app_init\n");

//  sys_status_get_main_tp1(&main_tp);

  interval = mtos_ticks_get();

  ui_init();
  //open avcpu print
  #if 0
  *((volatile unsigned int *) 0xbf150038) = 0x1;
  *((volatile unsigned int *) 0xbf490208) = 0x322223;
  #endif

  //live_tv_db_init();

  interval = mtos_ticks_get() - interval;
  OS_PRINTF("ui init time = %ld ms\n", interval * 10);
  OS_PRINTF("ui_init\n");
  }
}
