/**********************************************************************/

/**********************************************************************/
#ifndef __SYS_CFG_H_
#define __SYS_CFG_H_

#ifdef CHIP
#define SYS_CPU_CLOCK                   (324 * 1000000)
#else
#define SYS_CPU_CLOCK                   (81 * 1000000)
#endif
#define SYS_FLASH_END      CHARSTO_SIZE

#define GOBY_FLASH_SIZE  (8*KBYTES*KBYTES)


#define UPG_DESC_FILE_NAME "ftproot.xml"
#define CUSTOMER_ID (100)
#define HTTP_UPG_PATH "imserver.boxbest.net"

#define CUSTOMER_STRING "ling_demo"

#define SERVER_URL_1 "server_url_1"
#define SERVER_URL_2 "server_url_2"
#define INVALID_ID (0xffff)
#define SUB_DP_ID  7231

#define BURN_DEF_FLAG (0xFFFF)
#define BURNING_FLAG (0xFF5A)
#define BURN_DONE_FLAG (0x5A5A)

#define CUSTOM_URL_BLOCK_SIZE (128*KBYTES)


/*!
  custom url 
  */
#define CUS_URL_BLOCK_ID                       0xF0


/*!
  IR block
  */
#define IR_BLOCK_ID                       0xE0



/*!
   Task priority
  */
typedef enum
{
  SYS_INIT_TASK_PRIORITY = 2,
  SYS_TIMER_TASK_PRIORITY = 3,

  WIFI_TASK_THREAD = 4,
  ETH_NET_TASK_PRIORITY = 5,
  WIFI_TASK_LET_RECV =6,
  WIFI_TASK_LET_SEND =7,


  USB_MASS_STOR_TASK_PRIO,
  USB_BUS_TASK_PRIO,  

  WPA_SUPPLICANT_TASK,


  AP_FRM_TASK_PRIORITY = 16,
  UI_FRM_TASK_PRIORITY,

  NW_DOWNLOAD_PRIO,

  EVA_SYS_TASK_PRIORITY,
  EVA_SYS_TASK_PRIORITY_END = EVA_SYS_TASK_PRIORITY + 5,

  USB_ETH0_RECV_TASK_PRIORITY,
  DRV_HDMI_TASK_PRIORITY,

  AP_HIGHEST_TASK_PRIORITY = 30,
  AP_UIO_TASK_PRIORITY,
  AP_OTA_TASK_PRIORITY,

  MDL_DVB_TASK_PRIORITY = 41,
  MDL_NIM_CTRL_TASK_PRIORITY,
  MDL_PNP_SVC_TASK_PRIORITY,
  MDL_NET_SVC_TASK_PRIORITY,
  NET_SOURCE_SVC_PRIO,

  MDL_DM_MONITOR_TASK_PRIORITY,

  USB_HUB_TT_TASK_PRIO,


  NIM_NOTIFY_TASK_PRIORITY = 50,
  
  NIM_NOTIFY_TASK_PRIORITY_0,
  NIM_NOTIFY_TASK_PRIORITY_1,
  DISP_HDMI_NOTIFY_TASK_PRIORITY,
  USB_ETH1_RECV_TASK_PRIORITY, 

  TRANSPORT_SHELL_TASK_PRIORITY = 60,

  LOWEST_TASK_PRIORITY = 63,
}task_priority_t;

/*!
   Task stack size
  */
#define DEFAULT_STKSIZE              (32 * KBYTES)
#define SYS_INIT_TASK_STKSIZE        DEFAULT_STKSIZE
#define SYS_TIMER_TASK_STKSIZE       DEFAULT_STKSIZE
#define AP_FRM_TASK_STKSIZE          DEFAULT_STKSIZE
#define UI_FRM_TASK_STKSIZE          DEFAULT_STKSIZE

#define DRV_USB_BUS_STKSIZE             (4 * KBYTES)
#define DRV_USB_MASSSTOR_STKSIZE             (2 * KBYTES)

#define USB_HUB_TT_TASK_SIZE          (16 * KBYTES)

#define AP_UIO_TASK_STKSIZE          DEFAULT_STKSIZE
#define AP_OTA_TASK_STKSIZE          DEFAULT_STKSIZE


#define MDL_DVB_TASK_STKSIZE         DEFAULT_STKSIZE
#define MDL_NIM_CTRL_TASK_STKSIZE    DEFAULT_STKSIZE
#define MDL_DM_MONITOR_TASK_STKSIZE  DEFAULT_STKSIZE

#define DRV_HDMI_TASK_STKSIZE     DEFAULT_STKSIZE

#define NIM_NOTIFY_TASK_STK_SIZE          (4 * KBYTES)
#define DISP_HDMI_NOTIFY_TASK_STK_SIZE          (4 * KBYTES)
#define AUD_HDMI_NOTIFY_TASK_STK_SIZE          (4 * KBYTES)

/*!
   Datamanager configuration
  */
#define DM_MAX_BLOCK_NUM             (32)

#define DM_HDR_CUSTOM_DEF_ADDR 0x60000
#define CUSTOM_DEFINE_SIZE            (4*64*KBYTES)
#define STB_INFO_ADDR                    (0xa0000)
#define FLASH_RECORDS_OFFSET       (0xb0000)

#define DM_HDR_START_ADDR           0x1e0000

/*!
   Other configuration
  */
#define CHARSTO_SIZE (8 * MBYTES)
/*!
  The whole memory end, 128M.
  */
#define WHOLE_MEM_END           0xa8000000

/*!
  av cpu init code size 20k
  */
#define AV_INIT_CODE_SIZE 0x5000

/*!
  av cpu stack size 1M-20k
  */
#define AV_STACK_SIZE (0x40000 - AV_INIT_CODE_SIZE)

/*!
  av cpu stack bottom addr, 63M
  */
#define AV_STACK_BOT_ADDR (WHOLE_MEM_END - AV_INIT_CODE_SIZE - AV_STACK_SIZE)

/*!
  av cpu init code addr, 63M + av cpu stack size
  */
#define AV_POWER_UP_ADDR (((AV_STACK_BOT_ADDR + AV_STACK_SIZE) & 0xFFFFFFF) | 0x80000000)


/*!
  The OSD0 size :960*900*4.
  */
#define OSD0_VSCALER_BUFFER_SIZE 0x546000 //960*900*4,¡À?D?4K????

/*!
  The OSD0 scaler buffer start.
  */
#define OSD0_VSCALER_BUFFER_ADDR (AV_STACK_BOT_ADDR - OSD0_VSCALER_BUFFER_SIZE)

/*!
  The OSD1 size 6M, 1280*1080*4.
  */
#define OSD1_VSCALER_BUFFER_SIZE 0x546000//0x600000

/*!
  The OSD1 scaler buffer start .
  */
#define OSD1_VSCALER_BUFFER_ADDR (OSD0_VSCALER_BUFFER_ADDR - OSD1_VSCALER_BUFFER_SIZE)

/*!
  The SUB size : 1280*36*4.  //only need four line
  */
#define SUB_VSCALER_BUFFER_SIZE 0x546000//0x600000

/*!
  The SUB scaler buffer start .
  */
#define SUB_VSCALER_BUFFER_ADDR (OSD1_VSCALER_BUFFER_ADDR - SUB_VSCALER_BUFFER_SIZE)

/*!
  The video used size 40M, take ref to warriors_vdec_get_buf_requirement.
  */
//#define VIDEO_FW_CFG_SIZE           0x2800000

/*!
  The video used size 36.1M, take ref to warriors_vdec_get_buf_requirement.
  */
#define VIDEO_FW_CFG_SIZE           0

/*!
  The video used addr
  */
#define VIDEO_FW_CFG_ADDR           (SUB_VSCALER_BUFFER_ADDR - VIDEO_FW_CFG_SIZE)

/*!
  The video sd write back size, ~3.5M, 720*576*2*4. 422 frame mode
  */
#define VID_SD_WR_BACK_SIZE           0x32a000

/*!
  The video sd write back field no.
  */
#define VID_SD_WR_BACK_FIELD_NO           0x8

/*!
  The video sd write back addr.
  */
#define VID_SD_WR_BACK_ADDR           (VIDEO_FW_CFG_ADDR - VID_SD_WR_BACK_SIZE)

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
#define VID_DI_CFG_ADDR           (VID_SD_WR_BACK_ADDR - VID_DI_CFG_SIZE )

/*!
  The audio used size 2M, take ref to warriors_aud_get_buf_requirement.
  */
#define AUDIO_FW_CFG_SIZE           0

/*!
  The audio used addr.
  */
#define AUDIO_FW_CFG_ADDR  (((VID_DI_CFG_ADDR & 0xFFFFFFF) - AUDIO_FW_CFG_SIZE) | 0x80000000)

/*!
  malloc buffer end.
  *///yliu:av cpu config modify for temp need reconstruction
#define SYSTEM_MEMORY_END                AUDIO_FW_CFG_ADDR //last buffer

/*!
   mem partition configuration
 */
extern unsigned long _end;
#define ALL_MEM_SIZE ROUNDDOWN( \
                                 (SYSTEM_MEMORY_END - ROUNDUP((u32)(&_end), 4)) \
                                 , 8)

#define ALL_MEM_HEADER (ALL_MEM_SIZE/MEM_DEFAULT_ALIGN_SIZE/8)


#endif // End for __SYS_CFG_H_

