/******************************************************************************/

/******************************************************************************/
#ifndef __SYS_CFG_H_
#define __SYS_CFG_H_


#define _ON    1
#define _OFF    0

#define ENABLE_MUSIC_PICTURE            _ON
#define ENABLE_FILE_PLAY            _ON
#define ENABLE_GLOBAL_VOLUME     _ON
#define ENABLE_NETWORK            _ON
#define ENABLE_TRICK_PLAY         _ON
#define ENABLE_NET_PLAY         _ON
#define ENABLE_FLICKR         _OFF
#define ENABLE_NETMEDIA       _OFF
#define ENABLE_IPTV_FAV  _ON
#define ENABLE_PLAY_HIST  _ON
#define ENABLE_TTX_SUBTITLE             _OFF
#define ENABLE_ETH _ON
#define ENABLE_USB_ETH _ON
#define ENABLE_WIFI _ON
#define ENABLE_3G _OFF
#define ENABLE_GPRS _OFF

#define ENABLE_CUSTOM_URL _OFF

#define ENABLE_TEST_SPEED _OFF

#define ENABLE_WATCH_DOG                _ON

#define USB_ETH_SUPPORT_CNT 2

#define USB_HOST_CNT 2

#define ENABLE_8188_SUPPORT     _ON
#define ENABLE_5370_SUPPORT     _OFF   
#define ENABLE_7601_SUPPORT     _ON   

#define ENABLE_VOD    _OFF   

/*!
  BISS Data block_biss
  */
#define BSDATA_BLOCK_ID              0xAC
/*!
  Crypto block_crypto
  */
#define CRYPTO_BLOCK_ID              0xAB

#ifdef CHIP
#define SYS_CPU_CLOCK                   (54 * 1000 * 1000)
#else
#define SYS_CPU_CLOCK                   (54 * 1000 * 1000)
#endif


#define CA_DATA_SIZE           (64*KBYTES)
 
#define FILE_PLAYBACK_STKSIZE       (512 * KBYTES)

#define P4V_CADATA_1_BLOCK_ID        0xA9

#define P4V_CADATA_2_BLOCK_ID        0xAA

#define BSDATA_BLOCK_ID              0xAC

#define CRYPTO_BLOCK_ID              0xAB

#define CUS_URL_BLOCK_ID                       0xBE

#define STATIC_SAVE_DATA_BLOCK_ID 0xA2
#define STATIC_SAVE_DATA_RESERVE_BLOCK_ID 0xA3

#define DM_MAINCODE_BLOCK_ID         0x86
#define DM_APP_BLOCK_ID                      0x86

#define MAX_SATIP_PG_CNT 128
#define CUSTOM_URL_BLOCK_SIZE (128*KBYTES)
#define SUB_DP_ID  200115
//#define NET_CUSTOMER_ID  0x8984d4c8 
#define NET_CUSTOMER_ID  0xb86ee36e 


#define DTMB_HN_SYMBOL_DEFAULT 27500;
#define FULL_SCAN_CNT 50

#define HTTP_NET_UPG_ADDR \
"http://"

/*!
   Task priority
  */
typedef enum
{
  SYS_INIT_TASK_PRIORITY = 6,
  SYS_TIMER_TASK_PRIORITY = 7,
#ifdef SUPPORT_PUTOUT_SQUARE_WARE
  SQUARE_WARE_TASK_PRIORITY,
#endif
  
  WIFI_TASK_H_THREAD,

#ifndef WIN32
  WIFI_TASK_THREAD,
  WIFI_TASK_THREAD_2, 
#endif
  ETH_NET_TASK_PRIORITY,
#ifndef WIN32
  WIFI_TASK_LET_RECV,
  WIFI_TASK_LET_SEND,
  WIFI_TASK_LET_MTK_RECV,
  WIFI_TASK_LET_MTK_TIMER,

  WIFI_TASK_M1_THREAD,
  WIFI_TASK_M2_THREAD,
  WIFI_TASK_M3_THREAD,
  WIFI_TASK_L1_THREAD,
  WIFI_TASK_L2_THREAD,
#endif
#if ENABLE_TTX_SUBTITLE
  MDL_SUBT_TASK_PRIORITY,
#endif
  
  
    
  USB_MASS_STOR_TASK_PRIO,
  USB_BUS_TASK_PRIO,  

  USB_MASS_STOR1_TASK_PRIO,
  USB_BUS1_TASK_PRIO,
#ifndef WIN32
  WPA_SUPPLICANT_TASK,
   NET_LINK_TASK,
#endif
  AP_FRM_TASK_PRIORITY,
  UI_FRM_TASK_PRIORITY,
  NET_PLAYBACK_PRIORITY,
  FILE_PLAYBACK_PRIORITY,
#ifndef WIN32
  NETWORK_DLNA_PRIORITY,

  // mini_httpd
  SAT_IP_TASK_PRIORITY,
  SAT_IP_TASK2_PRIORITY,
  MINI_HTPPD_REQUEST_HANDLE_TASK_PRIORITY,
  MINI_HTTPD_WEB_SERVER_TASK_PRIORITY,
#endif

  AP_UIO_TASK_PRIORITY, 

#ifndef WIN32
  LIVE_TV_PRIORITY,
  LIVE_TV_HTTP_PRIORITY,
  VOD_DP_PRIORITY,
  ONMOV_PRIORITY,
  YT_DP_PRIORITY,
  NM_DP_PRIORITY,
  YT_HTTP_PRIORITY,

  DRV_SMC_TASK_PRIORITY,
#endif
#ifdef QILIAN_CA
	  QL_CAS_SECTION_TASK_PRIO,
	  QL_CAS_SECTION_TASK_PRIO_END = QL_CAS_SECTION_TASK_PRIO +2,
#endif

  DRV_CAS_ADAPTER_TASK_PRIORITY,
  AP_HIGHEST_TASK_PRIORITY = DRV_CAS_ADAPTER_TASK_PRIORITY + 5, 
  AP_PLAYBACK_TASK_PRIORITY = AP_HIGHEST_TASK_PRIORITY + 10,

  EVA_SYS_TASK_PRIORITY_START,
  EVA_SYS_TASK_PRIORITY_END = EVA_SYS_TASK_PRIORITY_START + 10,
#ifdef ADS_DIVI_SUPPORT
  AD_PIC_ROLL_TASK,
#endif
#if defined DESAI_56_CA || defined DESAI_52_CA
  AP_NIT_TASK_PRIORITY,
#endif

#ifndef WIN32
  PPP_TASK_PRIORITY,
  MODEM_TASK_PRIORITY,

  PPP_TASK_PRIORITY_3G,
  MODEM_TASK_PRIORITY_3G,  
  USB_ETH0_RECV_TASK_PRIORITY,
#endif
  BLOCK_SERV_PRIORITY,
  DRV_HDMI_TASK_PRIORITY,
	
#ifdef ENABLE_CA
  AP_CA_TASK_PRIORITY,
#endif
  JPEG_CHAIN_ANIM_TASK_PRIORITY,
#if ENABLE_BISS_KEY
  AP_TWIN_PORT_TASK_PRIORITY,
#endif
  AP_SCAN_TASK_PRIORITY,
  AP_SIGN_MON_TASK_PRIORITY,
  AP_EPG_TASK_PRIORITY,
  AP_USB_UPG_TASK_PRIORITY,//add for usb upg
  AP_UPGRADE_TASK_PRIORITY,
  AP_OTA_TASK_PRIORITY,
  AP_MUSIC_PLAYER_TASK_PRIORITY,
  AP_TS_PLAYER_TASK_PRIORITY,
  AP_JPEG_TASK_PRIORITY,
  AP_REC_TASK_PRIORITY,
  AP_TIMESHIFT_TASK_PRIORITY,
  AP_TIME_TASK_PRIORITY,

  TASK_AIQIYI_HTTP,  
  TASK_AIQIYI_MODULE,
  TASK_AIQIYI_AD_MODULE_START,
  TASK_AIQIYI_AD_MODULE_END = TASK_AIQIYI_AD_MODULE_START + 10,  
  
  
  MDL_CAS_TASK_PRIO_BEGIN,
#ifdef TR_CA
  MDL_CAS_TASK_PRIO_END = MDL_CAS_TASK_PRIO_BEGIN + 16,
#else
  MDL_CAS_TASK_PRIO_END = MDL_CAS_TASK_PRIO_BEGIN + 8,
#endif

  #ifdef ENABLE_ADS
  AD_TASK_PRIO_START,
  #if defined TEMP_SUPPORT_DS_AD
  AD_TASK_PRIO_END = AD_TASK_PRIO_START + 10,
  #else
  AD_TASK_PRIO_END = AD_TASK_PRIO_START + 8,
  #endif

  AD_MUL_PIC_DECODE_START,
  AD_MUL_PIC_DECODE_END = AD_MUL_PIC_DECODE_START + 3,

  #endif
#if ENABLE_TTX_SUBTITLE
  MDL_VBI_TASK_PRIORITY,
#endif
  MDL_DVB_TASK_PRIORITY,
  MDL_NIM_CTRL_TASK_PRIORITY,
  //MDL_NIM_CTRL_1_TASK_PRIORITY,
  MDL_PNP_SVC_TASK_PRIORITY,
#ifndef WIN32
  MDL_NET_SVC_TASK_PRIORITY,
#endif
  NET_SOURCE_SVC_PRIO,

  DISP_HDMI_NOTIFY_TASK_PRIORITY,
  USB_ETH1_RECV_TASK_PRIORITY, 

  //AUD_HDMI_NOTIFY_TASK_PRIORITY,

  MDL_MONITOR_TASK_PRIORITY,

  MDL_DM_MONITOR_TASK_PRIORITY,
  //BUS_MONITOR_TASK_PRIORITY,
  
#ifdef  CORE_DUMP_DEBUG
  TRANSPORT_SHELL_TASK_PRIORITY,
#endif
  //HTML_TASK_PRIO,
#ifndef WIN32
  MISC_PRIORITY,
  //IPTV_PRIORITY,
  USB_HUB_TT_TASK_PRIO,
  DLNA_STACK_PRIO_START,
  DLNA_STACK_PRIO_END = DLNA_STACK_PRIO_START + 50,
#endif
  NC_AUTHORIZATION_HTTP,
  NC_AUTHORIZATION_MONITOR,
  EMON_TASK_PRIO,

#ifndef WIN32
  LOWEST_TASK_PRIORITY = 255,
#else
  LOWEST_TASK_PRIORITY = 64,
#endif
}all_task_priority_t;

/*!
  Memory configuration
 */
#define SYSTEM_FLASH_END                 0x800000
#define OSD_BUFFER_ADDR                  0x1f0000

/*!
  Task stack size
 */
#define SYS_INIT_TASK_STK_SIZE           (32 * KBYTES)
#define SYS_TIMER_TASK_STK_SIZE          (4 * KBYTES)
#define SYS_CUNIT_TASK_STK_SIZE          (16 * KBYTES)

//#define VIDEO_PARTITION_SIZE             0



//#define SYS_PARTITION_SIZE       8 * 1024 * 1024                                                
#define SYS_PARTITION_ATOM               64


#define GPIO_HW_MUTE               23
#define HW_MUTE_ON                 0
#define HW_MUTE_OFF                1
/*!
   Task stack size
  */
#define DEFAULT_STKSIZE              (32* KBYTES)
#define SYS_INIT_TASK_STKSIZE        (32 * KBYTES)
#define SYS_TIMER_TASK_STKSIZE       (4 * KBYTES)

#define MDL_SUBT_TASK_STKSIZE        DEFAULT_STKSIZE
#define AP_FRM_TASK_STKSIZE          DEFAULT_STKSIZE
#define UI_FRM_TASK_STKSIZE          (32 * KBYTES)
#define MDL_CA_TASK_STKSIZE          DEFAULT_STKSIZE
#define AP_CA_TASK_STKSIZE           DEFAULT_STKSIZE
#define AP_TIME_TASK_STKSIZE         DEFAULT_STKSIZE
#define AP_UIO_TASK_STKSIZE          DEFAULT_STKSIZE
#define AP_PLAYBACK_TASK_STKSIZE     DEFAULT_STKSIZE
#define AP_TWIN_PORT_TASK_STKSIZE    DEFAULT_STKSIZE
#define AP_SCAN_TASK_STKSIZE         DEFAULT_STKSIZE
#define AP_MULTI_PIC_TASK_STKSIZE    DEFAULT_STKSIZE
#ifndef IMPL_NEW_EPG
#define AP_EPG_TASK_STKSIZE          DEFAULT_STKSIZE
#else
#define AP_EPG_TASK_STKSIZE          (64 * KBYTES)
#endif
#define AP_SIGN_MON_TASK_STKSIZE     DEFAULT_STKSIZE
#define AP_USB_UPGRADE_TASK_STKSIZE  DEFAULT_STKSIZE
#define AP_UPGRADE_TASK_STKSIZE      DEFAULT_STKSIZE
#define AP_SATCODX_TASK_STKSIZE      DEFAULT_STKSIZE
#define AP_OTA_TASK_STKSIZE          DEFAULT_STKSIZE
#define AP_IS_TASK_STKSIZE           DEFAULT_STKSIZE
#define AP_MONITOR_TASK_STKSIZE      DEFAULT_STKSIZE
#define AP_MUSIC_TASK_STKSIZE        DEFAULT_STKSIZE
#define AP_TS_TASK_STKSIZE           DEFAULT_STKSIZE
#define AP_JPEG_TASK_STKSIZE         DEFAULT_STKSIZE
#define AP_RECORD_TASK_STKSIZE       DEFAULT_STKSIZE

#define RECORD_CHAIN_TASK_STK_SIZE   DEFAULT_STKSIZE
#define PLAY_CHAIN_TASK_STK_SIZE     DEFAULT_STKSIZE
#if defined DESAI_56_CA || defined DESAI_52_CA
#define AP_NIT_TASK_STKSIZE       DEFAULT_STKSIZE
#endif
#define MDL_VBI_TASK_STKSIZE         DEFAULT_STKSIZE
#define MDL_DVB_TASK_STKSIZE         (32 * KBYTES)
#define MDL_NIM_CTRL_TASK_STKSIZE    DEFAULT_STKSIZE
#define MDL_DM_MONITOR_TASK_STKSIZE  DEFAULT_STKSIZE
#define MDL_PNP_SVC_TASK_STKSIZE  DEFAULT_STKSIZE
#define MDL_NET_SVC_TASK_STKSIZE  (32 * KBYTES)
#define MDL_MONITOR_TASK_STKSIZE  (10 * KBYTES)

#define ALBUMS_CHAIN_TASK_STK_SIZE       (32 * KBYTES)

#define DRV_HDMI_TASK_STKSIZE     (4 * KBYTES)

#define NIM_NOTIFY_TASK_STK_SIZE          (4 * KBYTES)
#define DISP_HDMI_NOTIFY_TASK_STK_SIZE          (4 * KBYTES)
#define AUD_HDMI_NOTIFY_TASK_STK_SIZE          (4 * KBYTES)
#define DRV_SMC_TASK_STKSIZE             (4 * KBYTES)
#define DRV_USB_BUS_STKSIZE             (4 * KBYTES)
#define DRV_USB_MASSSTOR_STKSIZE             (2 * KBYTES)
#define DRV_USB_HUB_TT_TASK_SIZE        (16 * KBYTES)
#define IPTV_TASK_STKSIZE      (32 * KBYTES)

#define USB_HUB_TT_TASK_SIZE          (16 * KBYTES)

/*!
   Datamanager configuration
  */
#define DM_MAX_BLOCK_NUM             (40)

#define DM_BOOTER_START_ADDR     0x60000//0xB0000
#define DM_HDR_START_ADDR           0x120000

/*!
   Other configuration
  */
#define CHARSTO_SIZE (8 * MBYTES)
#define TELTEXT_MAX_PAGE_NUM_1    100
#define TELTEXT_MAX_PAGE_NUM_2    30

#define SLEEP_TIMER     YES

/*!
  The whole memory end, 128M.
  */
#define WHOLE_MEM_END           0xa7e00000
/*!
  av cpu init code size 20k
  */
#define AV_INIT_CODE_SIZE 0x5000

/*!
  av cpu stack size 1M-20k
  */
#define AV_STACK_SIZE (0x100000 - AV_INIT_CODE_SIZE)

/*!
  av cpu stack bottom addr, 63M
  */
#define AV_STACK_BOT_ADDR (WHOLE_MEM_END - AV_INIT_CODE_SIZE - AV_STACK_SIZE)

/*!
  av cpu init code addr, 63M + av cpu stack size
  */


/*!
  The video used size 36.1M, take ref to warriors_vdec_get_buf_requirement.
  */
//#define VIDEO_FW_CFG_SIZE           (0x2660000)//(44 * 1024 * 1024)//)0x2800000
#define VIDEO_FW_CFG_SIZE           0x2D20000   //VDEC_QAULITY_AD_128M

/*!
  The video mpeg or avs less than h.264 about 3.5M, mb info.
  */
#define VIDEO_FW_MPEG_LESS_H264     0x0000
/*!
  The video used addr
  */
#define VIDEO_FW_CFG_ADDR           (AV_STACK_BOT_ADDR - VIDEO_FW_CFG_SIZE)

/*!
  The DI used size, ~2.5M, hd require almost 2.3M
  3 luma fields:  720x288x3
  4 chroma fields(cb and cr , 4:2:0):  360x144x2x4
  2 motion fields: 360x288x2  
  */
#define VID_DI_CFG_SIZE           0x2d6000

/*!
  The DI used addr.
  */
#define VID_DI_CFG_ADDR           (VIDEO_FW_CFG_ADDR - VID_DI_CFG_SIZE)

#define VBI_BUF_SIZE  0x10000
//#define VBI_BUF_SIZE  0x0000
#define VBI_BUF_ADDR  (VID_DI_CFG_ADDR - VBI_BUF_SIZE)

/*!
  The audio used size 2M, take ref to warriors_aud_get_buf_requirement.
  */
#define AUDIO_FW_CFG_SIZE           0x180000

/*!
  The audio used addr.
  */
#define AUDIO_FW_CFG_ADDR  (((VBI_BUF_ADDR & 0x1FFFFFFF) - AUDIO_FW_CFG_SIZE) | 0x80000000)


#define AP_AV_SHARE_MEM_SIZE (1024) 
#define AP_AV_SHARE_MEM_ADDR (VBI_BUF_ADDR - AUDIO_FW_CFG_SIZE - AP_AV_SHARE_MEM_SIZE)

/*!
  The video sd write back size, ~3.5M, 720*576*2*4. 422 frame mode
  */
#define VID_SD_WR_BACK_SIZE           0x4bf000
#define VID_SD_WR_BACK_FIELD_NO           0x6

/*!
  The video sd write back addr.
  */
#define VID_SD_WR_BACK_ADDR           (AP_AV_SHARE_MEM_ADDR - VID_SD_WR_BACK_SIZE)


#define EPG_BUFFER_SIZE   (3584* KBYTES)// + 1M from sys partion, 2.5m from ui

//#define EPG_BUFFER_HW_ADDR   (AUDIO_FW_CFG_ADDR - EPG_BUFFER_SIZE)
//#define EPG_BUFFER_HW_ADDR   (0x84600000 - EPG_BUFFER_SIZE)
#define EPG_BUFFER_HW_ADDR   ((VID_SD_WR_BACK_ADDR & 0x8FFFFFFF) - EPG_BUFFER_SIZE)

#define EPG_BUFFER_ADDR   (EPG_BUFFER_HW_ADDR & (~0xa0000000))

/*!
  record size
  */
//#define REC_BUFFER_SIZE ((1400)* KBYTES) //
#define REC_BUFFER_SIZE ((5 * 1024) * KBYTES) //+1m form ui resource, 400K to code size

/*!
  record addr
  */
#define REC_BUFFER_HW_ADDR ((EPG_BUFFER_HW_ADDR & 0x8FFFFFFF) - REC_BUFFER_SIZE)

/*!
  record addr
  */
#define REC_BUFFER_ADDR (REC_BUFFER_HW_ADDR & (~0xa0000000))


//#define net prg size
#define VIDEO_IPTV_PLAY_SIZE  ((4 * 1024)* KBYTES)

#define VIDEO_IPTV_PLAY_ADDR (REC_BUFFER_ADDR - VIDEO_IPTV_PLAY_SIZE)

#define VIDEO_IPTV_PLAY_HW_ADDR (REC_BUFFER_HW_ADDR - VIDEO_IPTV_PLAY_SIZE)

/*!
  VIDEO_SHIFT_SIZE_WITH_FILEPALY
  */
#define VIDEO_SHIFT_SIZE_WITH_FILEPALY 0

/*!
  file play addr
  */
#define VIDEO_FILE_PLAY_ADDR  ((VIDEO_IPTV_PLAY_ADDR  | 0x80000000) - VIDEO_SHIFT_SIZE_WITH_FILEPALY)

/*!
  File seq module will use 30M
  */
#define VIDEO_FILE_PLAY_SIZE  (REC_BUFFER_SIZE + EPG_BUFFER_SIZE + \
                                                      VIDEO_SHIFT_SIZE_WITH_FILEPALY + \
                                                      VIDEO_IPTV_PLAY_SIZE)  // epg + rec 

/*!
  file playback stack addr
  */
#define FILE_PLAYBACK_STK_ADDR  (VIDEO_FILE_PLAY_ADDR + VIDEO_FILE_PLAY_SIZE)



/*!
  malloc buffer end.
  */
#define SYSTEM_MEMORY_END                VIDEO_FILE_PLAY_ADDR //last buffer

/*!
   mem partition configuration
 */
#if defined(__LINUX__) || defined(__KERNEL__)

#else
extern unsigned long _end;
#define ALL_MEM_SIZE ROUNDDOWN( \
                                 (SYSTEM_MEMORY_END - ROUNDUP((u32)(&_end), 4)) \
                                 , 8)

#define ALL_MEM_HEADER (ALL_MEM_SIZE/MEM_DEFAULT_ALIGN_SIZE/8)
#endif

#define BSDATA_BLOCK_ID              0xAC
#define CRYPTO_BLOCK_ID              0xAB


#define VIDEOC_ONMOV_SPLIT_MEM ((4 * 1024)* KBYTES)

u32 get_flash_addr(void);
u32 get_bootload_off_addr(void);
u32 get_maincode_off_addr(void);

#endif

