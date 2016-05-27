/******************************************************************************/

/******************************************************************************/
// system
// std
#include "string.h"

#include "sys_types.h"
#include "sys_define.h"
//#include "sys_regs_magic.h"
#include "sys_regs_concerto.h"
#include "sys_devs.h"
#ifdef __LINUX__
#include <stdio.h>
#include <unistd.h>
#include<sys/time.h>
#include <flounder/includes/sys_cfg.h>
#else
#include "sys_cfg.h"
#endif
#include "driver.h"
#include "common.h"
// os
#include "mtos_task.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_misc.h"
#include "mtos_int.h"
#include "mem_manager.h"
#include "hal.h"
#include "hal_uart.h"
#include "mem_cfg.h"

#include "hal_misc.h"
#include "sys_dbg.h"

#define  SHOW_DEFINE_VALUE(x)                   \
    do {									\
        OS_PRINTF("%s:[%#08x]\n",#x,x);		\
    } while (0)



	
extern u32 *p_DUAL_CORE_PRINTK_LOCK ;


void SYS_VIEW_TASK_PRIORITY(void)
{
	int i;
	for (i=0;i<LOWEST_TASK_PRIORITY;i++)
	{
		switch(i)
		{
			case SYS_INIT_TASK_PRIORITY: OS_PRINTF("%s =  ","SYS_INIT_TASK_PRIORITY");break;
			case SYS_TIMER_TASK_PRIORITY: OS_PRINTF("%s =  ","SYS_TIMER_TASK_PRIORITY");break;
#ifndef WIN32
			case WIFI_TASK_THREAD: OS_PRINTF("%s =  ","WIFI_TASK_THREAD");break;
			case WIFI_TASK_THREAD_2: OS_PRINTF("%s =  ","WIFI_TASK_THREAD_2");break;
#endif
			case ETH_NET_TASK_PRIORITY: OS_PRINTF("%s =  ","ETH_NET_TASK_PRIORITY");break;
#ifndef WIN32
			case WIFI_TASK_LET_RECV: OS_PRINTF("%s =  ","WIFI_TASK_LET_RECV");break;
			case WIFI_TASK_LET_SEND: OS_PRINTF("%s =  ","WIFI_TASK_LET_SEND");break;
			case WIFI_TASK_LET_MTK_RECV: OS_PRINTF("%s =  ","WIFI_TASK_LET_MTK_RECV");break;
			case WIFI_TASK_LET_MTK_TIMER: OS_PRINTF("%s =  ","WIFI_TASK_LET_MTK_TIMER");break;
#endif
#if ENABLE_TTX_SUBTITLE
			case MDL_SUBT_TASK_PRIORITY: OS_PRINTF("%s =  ","MDL_SUBT_TASK_PRIORITY");break;
#endif
			case USB_MASS_STOR_TASK_PRIO: OS_PRINTF("%s =  ","USB_MASS_STOR_TASK_PRIO");break;
			case USB_BUS_TASK_PRIO: OS_PRINTF("%s =  ","USB_BUS_TASK_PRIO");break;
			case USB_MASS_STOR1_TASK_PRIO: OS_PRINTF("%s =  ","USB_MASS_STOR1_TASK_PRIO");break;
			case USB_BUS1_TASK_PRIO: OS_PRINTF("%s =  ","USB_BUS1_TASK_PRIO");break;
#ifndef WIN32
			case WPA_SUPPLICANT_TASK: OS_PRINTF("%s =  ","WPA_SUPPLICANT_TASK");break;
#endif
			case AP_FRM_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_FRM_TASK_PRIORITY");break;
			case UI_FRM_TASK_PRIORITY: OS_PRINTF("%s =  ","UI_FRM_TASK_PRIORITY");break;
			case NET_PLAYBACK_PRIORITY: OS_PRINTF("%s =  ","NET_PLAYBACK_PRIORITY");break;
			case FILE_PLAYBACK_PRIORITY: OS_PRINTF("%s =  ","FILE_PLAYBACK_PRIORITY");break;
#ifndef WIN32
			case NETWORK_DLNA_PRIORITY: OS_PRINTF("%s =  ","NETWORK_DLNA_PRIORITY");break;
			// mini_httpd
			case SAT_IP_TASK_PRIORITY: OS_PRINTF("%s =  ","SAT_IP_TASK_PRIORITY");break;
			case SAT_IP_TASK2_PRIORITY: OS_PRINTF("%s =  ","SAT_IP_TASK2_PRIORITY");break;
			case MINI_HTPPD_REQUEST_HANDLE_TASK_PRIORITY: OS_PRINTF("%s =  ","MINI_HTPPD_REQUEST_HANDLE_TASK_PRIORITY");break;
			case MINI_HTTPD_WEB_SERVER_TASK_PRIORITY: OS_PRINTF("%s =  ","MINI_HTTPD_WEB_SERVER_TASK_PRIORITY");break;
#endif

			case AP_UIO_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_UIO_TASK_PRIORITY");break;

#ifndef WIN32
			case LIVE_TV_PRIORITY: OS_PRINTF("%s =  ","LIVE_TV_PRIORITY");break;
			case LIVE_TV_HTTP_PRIORITY: OS_PRINTF("%s =  ","LIVE_TV_HTTP_PRIORITY");break;
			case VOD_DP_PRIORITY: OS_PRINTF("%s =  ","VOD_DP_PRIORITY");break;
			case ONMOV_PRIORITY: OS_PRINTF("%s =  ","ONMOV_PRIORITY");break;
			case YT_DP_PRIORITY: OS_PRINTF("%s =  ","YT_DP_PRIORITY");break;
			case NM_DP_PRIORITY: OS_PRINTF("%s =  ","NM_DP_PRIORITY");break;
			case YT_HTTP_PRIORITY: OS_PRINTF("%s =  ","YT_HTTP_PRIORITY");break;
			case DRV_SMC_TASK_PRIORITY: OS_PRINTF("%s =  ","DRV_SMC_TASK_PRIORITY");break;
#endif
			case DRV_CAS_ADAPTER_TASK_PRIORITY: OS_PRINTF("%s =  ","DRV_CAS_ADAPTER_TASK_PRIORITY");break;
			case AP_HIGHEST_TASK_PRIORITY : OS_PRINTF("%s =  ","AP_HIGHEST_TASK_PRIORITY");break;
			case AP_PLAYBACK_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_PLAYBACK_TASK_PRIORITY");break;

			case EVA_SYS_TASK_PRIORITY_START: OS_PRINTF("%s =  ","EVA_SYS_TASK_PRIORITY_START");break;
			case EVA_SYS_TASK_PRIORITY_END: OS_PRINTF("%s =  ","EVA_SYS_TASK_PRIORITY_END");break;


#ifndef WIN32
			case PPP_TASK_PRIORITY: OS_PRINTF("%s =  ","PPP_TASK_PRIORITY");break;
			case MODEM_TASK_PRIORITY: OS_PRINTF("%s =  ","MODEM_TASK_PRIORITY");break;
			case PPP_TASK_PRIORITY_3G: OS_PRINTF("%s =  ","PPP_TASK_PRIORITY_3G");break;
			case MODEM_TASK_PRIORITY_3G:   OS_PRINTF("%s =  ","MODEM_TASK_PRIORITY_3G");break;
			case USB_ETH0_RECV_TASK_PRIORITY: OS_PRINTF("%s =  ","USB_ETH0_RECV_TASK_PRIORITY");break;
#endif
			case BLOCK_SERV_PRIORITY: OS_PRINTF("%s =  ","BLOCK_SERV_PRIORITY");break;
			case DRV_HDMI_TASK_PRIORITY: OS_PRINTF("%s =  ","DRV_HDMI_TASK_PRIORITY");break;

#ifdef ENABLE_CA
			case AP_CA_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_CA_TASK_PRIORITY");break;
#endif
			case JPEG_CHAIN_ANIM_TASK_PRIORITY: OS_PRINTF("%s =  ","JPEG_CHAIN_ANIM_TASK_PRIORITY");break;
#if ENABLE_BISS_KEY
			case AP_TWIN_PORT_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_TWIN_PORT_TASK_PRIORITY");break;
#endif
			case AP_SCAN_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_SCAN_TASK_PRIORITY");break;
			case AP_SIGN_MON_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_SIGN_MON_TASK_PRIORITY");break;
			case AP_EPG_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_EPG_TASK_PRIORITY");break;
			case AP_USB_UPG_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_USB_UPG_TASK_PRIORITY");break;
			case AP_UPGRADE_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_UPGRADE_TASK_PRIORITY");break;
			case AP_OTA_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_OTA_TASK_PRIORITY");break;
			case AP_MUSIC_PLAYER_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_MUSIC_PLAYER_TASK_PRIORITY");break;
			case AP_TS_PLAYER_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_TS_PLAYER_TASK_PRIORITY");break;
			case AP_JPEG_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_JPEG_TASK_PRIORITY");break;
			case AP_REC_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_REC_TASK_PRIORITY");break;
			case AP_TIMESHIFT_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_TIMESHIFT_TASK_PRIORITY");break;
			case AP_TIME_TASK_PRIORITY: OS_PRINTF("%s =  ","AP_TIME_TASK_PRIORITY");break;
			case MDL_CAS_TASK_PRIO_BEGIN: OS_PRINTF("%s =  ","MDL_CAS_TASK_PRIO_BEGIN");break;
			case MDL_CAS_TASK_PRIO_END: OS_PRINTF("%s =  ","MDL_CAS_TASK_PRIO_END");break;
#if ENABLE_TTX_SUBTITLE
			case MDL_VBI_TASK_PRIORITY: OS_PRINTF("%s =  ","MDL_VBI_TASK_PRIORITY");break;
#endif
			case MDL_DVB_TASK_PRIORITY: OS_PRINTF("%s =  ","MDL_DVB_TASK_PRIORITY");break;
			case MDL_NIM_CTRL_TASK_PRIORITY: OS_PRINTF("%s =  ","MDL_NIM_CTRL_TASK_PRIORITY");break;
			case MDL_PNP_SVC_TASK_PRIORITY: OS_PRINTF("%s =  ","MDL_PNP_SVC_TASK_PRIORITY");break;
#ifndef WIN32
			case MDL_NET_SVC_TASK_PRIORITY: OS_PRINTF("%s =  ","MDL_NET_SVC_TASK_PRIORITY");break;
#endif
			case NET_SOURCE_SVC_PRIO: OS_PRINTF("%s =  ","NET_SOURCE_SVC_PRIO");break;
			case DISP_HDMI_NOTIFY_TASK_PRIORITY: OS_PRINTF("%s =  ","DISP_HDMI_NOTIFY_TASK_PRIORITY");break;
			case USB_ETH1_RECV_TASK_PRIORITY: OS_PRINTF("%s =  ","USB_ETH1_RECV_TASK_PRIORITY");break;
			case MDL_MONITOR_TASK_PRIORITY: OS_PRINTF("%s =  ","MDL_MONITOR_TASK_PRIORITY");break;
			case MDL_DM_MONITOR_TASK_PRIORITY: OS_PRINTF("%s =  ","MDL_DM_MONITOR_TASK_PRIORITY");break;
#ifdef  CORE_DUMP_DEBUG
			case MDL_DM_MONITOR_TASK_PRIORITY: OS_PRINTF("%s =  ","MDL_DM_MONITOR_TASK_PRIORITY");break;
#endif
#ifndef WIN32
			case MISC_PRIORITY: OS_PRINTF("%s =  ","MISC_PRIORITY");break;
			case USB_HUB_TT_TASK_PRIO: OS_PRINTF("%s =  ","USB_HUB_TT_TASK_PRIO");break;
			case DLNA_STACK_PRIO_START: OS_PRINTF("%s =  ","DLNA_STACK_PRIO_START");break;
			case DLNA_STACK_PRIO_END: OS_PRINTF("%s =  ","DLNA_STACK_PRIO_END");break;
			case NC_AUTHORIZATION_HTTP: OS_PRINTF("%s =  ","NC_AUTHORIZATION_HTTP");break;
			case NC_AUTHORIZATION_MONITOR: OS_PRINTF("%s =  ","NC_AUTHORIZATION_MONITOR");break;
#endif
			case TASK_AIQIYI_MODULE: OS_PRINTF("%s =  ","TASK_AIQIYI_MODULE");break;
			case TASK_AIQIYI_AD_MODULE_START: OS_PRINTF("%s =  ","TASK_AIQIYI_AD_MODULE_START");break;
			case TASK_AIQIYI_AD_MODULE_END: OS_PRINTF("%s =  ","TASK_AIQIYI_AD_MODULE_END");break;  
#ifndef WIN32
			case LOWEST_TASK_PRIORITY : OS_PRINTF("%s =  ","LOWEST_TASK_PRIORITY");break;
#else
			case LOWEST_TASK_PRIORITY: OS_PRINTF("%s =  ","LOWEST_TASK_PRIORITY");break;
#endif
			default:
			break;
		}
		OS_PRINTF("%d \n",i);
	}
	OS_PRINTF("show memory alloc!!\n");
	SHOW_DEFINE_VALUE(WHOLE_MEM_END);
	SHOW_DEFINE_VALUE(AV_INIT_CODE_SIZE);
	SHOW_DEFINE_VALUE(AV_STACK_SIZE);
	SHOW_DEFINE_VALUE(AV_STACK_BOT_ADDR);
	SHOW_DEFINE_VALUE(VIDEO_FW_CFG_SIZE);
	SHOW_DEFINE_VALUE(VIDEO_FW_CFG_ADDR);
	SHOW_DEFINE_VALUE(VID_DI_CFG_SIZE);
	SHOW_DEFINE_VALUE(VID_DI_CFG_ADDR);
	SHOW_DEFINE_VALUE(VBI_BUF_SIZE);
	SHOW_DEFINE_VALUE(VBI_BUF_ADDR);
	SHOW_DEFINE_VALUE(AUDIO_FW_CFG_SIZE);
	SHOW_DEFINE_VALUE(AUDIO_FW_CFG_ADDR);
	SHOW_DEFINE_VALUE(AP_AV_SHARE_MEM_SIZE);
	SHOW_DEFINE_VALUE(AP_AV_SHARE_MEM_ADDR);
	SHOW_DEFINE_VALUE(VID_SD_WR_BACK_SIZE);
	SHOW_DEFINE_VALUE(VID_SD_WR_BACK_ADDR);
	SHOW_DEFINE_VALUE(EPG_BUFFER_SIZE);
	SHOW_DEFINE_VALUE(EPG_BUFFER_HW_ADDR);
	SHOW_DEFINE_VALUE(EPG_BUFFER_ADDR);
	SHOW_DEFINE_VALUE(REC_BUFFER_SIZE);
	SHOW_DEFINE_VALUE(REC_BUFFER_HW_ADDR);
	SHOW_DEFINE_VALUE(REC_BUFFER_ADDR);
	SHOW_DEFINE_VALUE(VIDEO_IPTV_PLAY_SIZE);
	SHOW_DEFINE_VALUE(VIDEO_IPTV_PLAY_ADDR);
	SHOW_DEFINE_VALUE(VIDEO_IPTV_PLAY_HW_ADDR);
	SHOW_DEFINE_VALUE(VIDEO_FILE_PLAY_ADDR);
	SHOW_DEFINE_VALUE(VIDEO_FILE_PLAY_SIZE);
	SHOW_DEFINE_VALUE(FILE_PLAYBACK_STK_ADDR);
	SHOW_DEFINE_VALUE(SYSTEM_MEMORY_END);


	/*!
	Memory configuration
	*/
	SHOW_DEFINE_VALUE( SYS_MEMORY_END  );
	SHOW_DEFINE_VALUE( AV_BUFFER_SIZE );
	SHOW_DEFINE_VALUE( AV_BUFFER_ADDR);
	SHOW_DEFINE_VALUE( SUB_BUFFER_SIZE );
	SHOW_DEFINE_VALUE( SUB_BUFFER_ADDR );
	SHOW_DEFINE_VALUE( SUB_VSCALER_BUFFER_SIZE );
	SHOW_DEFINE_VALUE( SUB_VSCALER_BUFFER_ADDR );
	SHOW_DEFINE_VALUE( OSD1_BUFFER_SIZE);
	SHOW_DEFINE_VALUE( OSD1_BUFFER_ADDR );
	SHOW_DEFINE_VALUE( OSD1_VSCALER_BUFFER_SIZE );
	SHOW_DEFINE_VALUE( OSD1_VSCALER_BUFFER_ADDR );
	SHOW_DEFINE_VALUE( OSD0_BUFFER_SIZE );
	SHOW_DEFINE_VALUE( OSD0_BUFFER_ADDR );
	SHOW_DEFINE_VALUE( OSD0_VSCALER_BUFFER_SIZE );
	SHOW_DEFINE_VALUE( OSD0_VSCALER_BUFFER_ADDR );
	SHOW_DEFINE_VALUE( OSD_VSURF_BUFFER_SIZE  );
	SHOW_DEFINE_VALUE( OSD_VSURF_BUFFER_ADDR );
	SHOW_DEFINE_VALUE( GUI_ANIM_BUFFER_SIZE );
	SHOW_DEFINE_VALUE( GUI_ANIM_BUFFER_ADDR );
	SHOW_DEFINE_VALUE( DES_BUFFER_SIZE );
	SHOW_DEFINE_VALUE( DES_BUFFER_ADDR );
	SHOW_DEFINE_VALUE( PLAY_BUFFER_SIZE ); 
	SHOW_DEFINE_VALUE( PLAY_BUFFER_ADDR );
	SHOW_DEFINE_VALUE( GUI_RESOURCE_BUFFER_SIZE );
	SHOW_DEFINE_VALUE( GUI_RESOURCE_BUFFER_ADDR );
	SHOW_DEFINE_VALUE( GUI_PARTITION_SIZE   );
	SHOW_DEFINE_VALUE( SYS_PARTITION_SIZE        );
	SHOW_DEFINE_VALUE( SYS_PARTITION_ATOM     );
	/*!
	multiplex size
	*/
	SHOW_DEFINE_VALUE( MUSIC_MODULE_USED_SIZE );  
	SHOW_DEFINE_VALUE( MUSIC_REGION_USED_SIZE );
	SHOW_DEFINE_VALUE( PICTURE_MODULE_USED_SIZE );
	SHOW_DEFINE_VALUE( PICTURE_REGION_USED_SIZE );
	SHOW_DEFINE_VALUE( TS_PLAY_BUFFER_LEN  ); 




}


void dlmem_init(void* base, int size);

void *mt_mem_malloc(u32 size)
{
  void *p_addr = NULL;
  mem_mgr_alloc_param_t param = {0};
  param.id = MEM_SYS_PARTITION;
  param.size = size;
  param.user_id = SYS_MODULE_SYSTEM;
  p_addr = mem_mgr_alloc(&param);
  //MT_ASSERT(p_addr != NULL);
  memset(p_addr, 0, size);

  return p_addr;
}

void mt_mem_free(void* p_addr)
{
  mem_mgr_free_param_t param = {0};
  param.id = MEM_SYS_PARTITION;
  param.p_addr = p_addr;
  param.user_id = SYS_MODULE_SYSTEM;
  mem_mgr_free(&param);
}



extern void Task_SysTimer(void* pData);


u32 get_mem_addr(void)
{
  return 0x80000000;
}

void Task_BusMonitor(void *pData)
{
#ifndef __LINUX__
    while(1)
    {
        if(0x31 == *(volatile u32 *)0xbfd00000)
          hal_bus_mon_start(HAL_BUS_MON_WHOLE_THROUGHPUT,0,0);

        mtos_task_sleep(1000);

        if(0x31 == *(volatile u32 *)0xbfd00000)
          hal_bus_mon_stop(HAL_BUS_MON_WHOLE_THROUGHPUT);

    }
#endif
}

u32 get_flash_addr(void)
{
  return 0x9e000000;
}


u32 get_bootload_off_addr(void)
{
  return DM_BOOTER_START_ADDR;
}

u32 get_maincode_off_addr(void)
{
  return DM_HDR_START_ADDR;
}

extern void  OSStart (void);
extern void Task_SysTimer(void* pData);
extern void ap_init(void);
extern u32 get_os_version();


char TaskStartStk[SYS_INIT_TASK_STK_SIZE] __attribute__ ((aligned (8)));

char buildSTR[]=__DATE__"--"__TIME__;


extern void mt_hal_invoke_constructors(void);
void gpio_mux(void)
{
    /* uart1 use TSI_D0 and TSI_VALI pin */
  *(volatile unsigned int *)(0xbf15601c) |= 0xf;  //bit0 ~bit3 --> 1
  *(volatile unsigned int *)(0xbf156308) |= 0x0c000000; // bit26 bit27 --> 1
}

typedef void (*pfunc)(void);
extern pfunc __CTOR_LIST__[];
extern pfunc __CTOR_END__[];

void mt_hal_invoke_constructors_2(void)
{
    pfunc *p_fn = NULL;
    for (p_fn = &__CTOR_END__[-1]; p_fn >= __CTOR_LIST__; p_fn--)
    {
        mtos_printk(" cons 0x%x before\n", *p_fn);
        (*p_fn)();
        mtos_printk(" cons 0x%x  end\n", *p_fn);
    }
    
}



void TaskStart (void *data)
{
#ifndef __LINUX__ //only for ucos
  extern u32 _end;
  u32 bss_end = (u32)(&_end);
  BOOL ret = FALSE;
  u32 *pstack_pnt = NULL;
  u32 heap_start = 0;
  u32 cpuclk = 0;

  heap_start = ROUNDUP(bss_end,4);
  mtos_irq_init();

  extern void hal_concerto_attach(void);

  hal_concerto_attach( );

  mtos_register_putchar(uart_write_byte);
  mtos_register_getchar(uart_read_byte);

  hal_module_clk_get(HAL_CPU0, &cpuclk);
  mtos_ticks_init(cpuclk);

  /* uart1 pin mux */
  gpio_mux();

  uart_init(0);
  uart_init(1);
  uart_set_param(0, 115200, 8, 1, 0);
  uart_set_param(1, 115200, 8, 1, 0);
  mtos_printk("\n MaincodeTaskStart\n");
  mtos_printk("\n Built at %s \n", buildSTR);

  mtos_printk("\n 1. heap_start[0x%x]\n", heap_start);
  mtos_printk("\n 2. whole memory size[0x%x]\n", (WHOLE_MEM_END & 0xFFFFFFF));
 #ifdef SHOW_MEM_SUPPORT
  show_single_memory_mapping(CODE_SIZE_FLAG,0,heap_start);
  show_single_memory_mapping(SYSTEM_PARTITION_FLAG,heap_start,SYS_PARTITION_SIZE);
  #endif

  mtos_printk("\n 3. SYS_PARTITION_SIZE is 0x%x, GUI_RESOURCE_BUFFER_ADDR is 0x%x SYSTEM_MEMORY_END :0x%x\n", SYS_PARTITION_SIZE, GUI_RESOURCE_BUFFER_ADDR,SYSTEM_MEMORY_END);
  mtos_printk("\n heap_start[0x%x]\n", heap_start);

  if((heap_start + SYS_PARTITION_SIZE) > (GUI_RESOURCE_BUFFER_ADDR + 0x80000000))
  {
    OS_PRINTF("system partition is overlaped %d\n", (heap_start + SYS_PARTITION_SIZE) - GUI_RESOURCE_BUFFER_ADDR - 0x80000000);
    MT_ASSERT(0);
  }
  //init memory manager, only 11M
  //mem_mgr_init((u8 *)heap_start, SYS_PARTITION_SIZE + (1 * 1024 * 1024));
  mtos_printk("free mem %d\n", (GUI_RESOURCE_BUFFER_ADDR + 0x80000000) - (heap_start + SYS_PARTITION_SIZE));


  mem_mgr_init((u8 *)heap_start,
  SYS_PARTITION_SIZE);

  mtos_printk("\n 5. mem_mgr_start[0x%x],size=0x%x\n", heap_start,
  	SYS_PARTITION_SIZE);
  dlmem_init((void *)heap_start,
  	SYS_PARTITION_SIZE);
  mtos_mem_init(dl_malloc, dl_free);
  mtos_printk("init mem ok!\n");

  u32 os_version = get_os_version();

  if (os_version != 0x11112222) {
    mtos_printk("Wrong os version, please talk with os person!\n");

  }
  mtos_printk("Os version pass!\n");
  mt_hal_invoke_constructors();
  mtos_printk("mt_hal_invoke_constructors pass!\n");
#else
  BOOL ret = FALSE;
  u32 *pstack_pnt = NULL;
  u32 *pstack_pnt_bak = NULL;
#endif

//enable interrupt
  mtos_irq_enable(TRUE);
  OS_PRINTF("Enable irq!\n");

  ret = mtos_message_init();
  MT_ASSERT(FALSE != ret);

#ifdef __LINUX__
   init_rw_reg();
#endif

#ifdef CORE_DUMP_DEBUG
  mtos_stat_init();
#endif

  //SYS_VIEW_TASK_PRIORITY();
  //create timer task
  pstack_pnt = (u32*)mtos_malloc(SYS_TIMER_TASK_STKSIZE);
  MT_ASSERT(pstack_pnt != NULL);

  ret = mtos_task_create((u8 *)"Timer",
         Task_SysTimer,
         (void *)0,
         SYS_TIMER_TASK_PRIORITY,
         pstack_pnt,
         SYS_TIMER_TASK_STKSIZE);

  MT_ASSERT(FALSE != ret);

  ap_init();

  mtos_task_exit();

}


extern void EXCEP_vInstallIntInRam(void);

int main(void)
{
  mtos_cfg_t os_cfg = {0};

  EXCEP_vInstallIntInRam();

  /* Initialize uC/OS-II                                      */

  mtos_os_init(&os_cfg);
  
  mtos_task_create((u8 *)"initT1",
                 TaskStart,
                 NULL,
                 SYS_INIT_TASK_PRIORITY,
                 (u32*)TaskStartStk,
                 SYS_INIT_TASK_STKSIZE);

  OSStart();                              /* Start multitasking    */
  return 0;
}


