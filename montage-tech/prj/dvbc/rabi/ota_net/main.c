/******************************************************************************/

/******************************************************************************/
// system
// std
#include "string.h"

#include "sys_types.h"
#include "sys_define.h"
//#include "sys_regs_magic.h"
#include "sys_regs_warriors.h"
#include "sys_devs.h"
#include "sys_cfg.h"
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
#include "hal_timer.h"
#include "mem_cfg.h"

#include "hal_misc.h"

#include "drv_dev.h"
#include "service.h"
#include "class_factory.h"
//#include "dsmcc.h"
#include "Mdl.h"
#include "nim.h"
#include "Nim_ctrl_svc.h"
#include "dvbs_util.h"
#include "dvbc_util.h"
#include "dvbt_util.h"
#include "Ss_ctrl.h"
#include "LzmaIf.h"
#include "fcrc.h"

#include "data_manager.h"
#include "data_manager_v2.h"
#include "uio.h"
#include "charsto.h"

char TaskStartStk[SYS_INIT_TASK_STKSIZE];
extern u32 *p_DUAL_CORE_PRINTK_LOCK ;
ota_trigger_t ota_tri = OTA_TRI_NONE;


s32 hal_uart_warriors_b0_write_byte(u8 id, u8 chval);

extern void ota_read_otai(ota_info_t *p_otai);
extern void ota_write_otai(ota_info_t *p_otai);
extern BOOL net_upg_is_burn_finish(void);
extern void sys_status_init(void);
extern void hal_warriors_attach(hal_config_t *p_config);
extern void  OSStart (void);
extern void Task_SysTimer(void *p_data);
extern void ap_init(void);
extern void uio_init(void);

//boot code start
#define PARA_ADDRESS   (0xbfef0180)
#define JUMP(dest) {__asm__ __volatile__ ("j %0" : : "d" (dest));  __asm__("nop");}
#define REBACK_BOOT_FLAG  (0x1234)

u32 boot_jump_ID = 0x0;
u32 back_addr = 0x0;
volatile u32* boot_para1 = NULL;//jump to which code
volatile u32* boot_para2 = NULL;//jump address
volatile u32* boot_para3 = NULL;//HDMI flag

void save_boot_infor(void)
{
  boot_jump_ID = (*((volatile u32 *)(PARA_ADDRESS))) & 0xFFFF;
  back_addr = (*((volatile u32 *)(PARA_ADDRESS + 4)));
  boot_para1 = (volatile u32 *)(PARA_ADDRESS + 8);
  boot_para2 = (volatile u32 *)(PARA_ADDRESS + 0x0C);
  boot_para3 = (volatile u32 *)(PARA_ADDRESS + 0x10);

  //mtos_printk("boot_jump_ID=0x%x,back_addr=0x%x,boot_para1=0x%x, data=10x%x\n",
      //boot_jump_ID,back_addr,boot_para1,*boot_para1);
  //mtos_printk("boot_para2=0x%x, data2=0x%x\n",
      //boot_para2,*boot_para2);
  //mtos_printk("boot_para3=0x%x, data3=0x%x\n",
      //boot_para3,*boot_para3);
}

static void ota_bsp_deinit(void)
{
  u8 i;

  /* release os resource */
  for(i=0; i<=TIMER_ID3; i++)
  {
    hal_timer_release(i);
  }
}

void reback_to_boot(void)
{
  u32 p_sr;

  mtos_task_delay_ms(25);
  ota_bsp_deinit();

  hal_timer_release(0);
  hal_timer_release(1);
  hal_timer_release(2);
  hal_timer_release(3);

  *boot_para1 = 0x12340000 | MAINCODE_BLOCK_ID;
  *boot_para2 = 0x80008000;
  *boot_para3 = 0x12340000 | 0x0001;
  *((volatile u32 *)(PARA_ADDRESS)) = (REBACK_BOOT_FLAG << 16) | boot_jump_ID;

  mtos_critical_enter(&p_sr);
  mtos_printk("boot_para3=%x,*boot_para3=%x\n",boot_para3,*boot_para3);
  mtos_printk("ota_TaskEnd\n");
  JUMP(back_addr);
}
//boot code end

extern void hal_sonata_attach(void);
u32 get_mem_addr(void)
{
  return 0xa0000000;
}

u32 get_flash_addr(void)
{
  return NORF_BASE_ADDR;
}


BOOL is_force_key_press()
{
  void *p_dev = NULL;
  u16 code = 0xFFFF;
  u8 hkey = 0xFF;

  p_dev = dev_find_identifier(NULL,
                            DEV_IDT_TYPE,
                            SYS_DEV_TYPE_UIO);

  //get second hkey from fp.bin.
  dm_read(class_get_handle_by_id(DM_CLASS_ID), FPKEY_BLOCK_ID, 0, 1, sizeof(u8), &hkey);
  uio_get_code(p_dev, &code);
  OS_PRINTF("is_force_key_press hkey %x , code %x \n", hkey, code);
  //make sure it's fp key.
  if((code & 0x100) && (hkey == (code & 0xFF)))
  {
    return TRUE;
  }

  return FALSE;
}

ota_trigger_t ota_check(void)
{
  charsto_cfg_t charsto_cfg = {0};
  dm_v2_init_para_t dm_para = { 0 };
  void  *p_dev = NULL;
  RET_CODE ret = ERR_FAILURE;

  /******char storage init******/

  p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
  MT_ASSERT(NULL != p_dev);

  charsto_cfg.size = 8 * MBYTES;
  charsto_cfg.spi_clk = FLASH_C_CLK_P6;
  charsto_cfg.rd_mode = SPI_FR_MODE;
  //charsto_cfg.multi_io_rd = 1;
  //charsto_cfg.multi_io_wd = 1;
  ret = dev_open(p_dev, &charsto_cfg);
  MT_ASSERT(ret == SUCCESS);

  /* init data manager */
  dm_para.flash_base_addr = get_flash_addr();
  dm_para.max_blk_num = DM_MAX_BLOCK_NUM;
  dm_para.task_prio = MDL_DM_MONITOR_TASK_PRIORITY;
  dm_para.task_stack_size = MDL_DM_MONITOR_TASK_STKSIZE;
  dm_para.open_monitor = TRUE;
  dm_para.para_size = sizeof(dm_v2_init_para_t);

  dm_init_v2(&dm_para);
  OS_PRINTF("[OTA]set header [0xC]\n");
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), 0xC);

  OS_PRINTF("[OTA]set header [0x%x]\n",DM_HDR_CUSTOM_DEF_ADDR);
  dm_set_header(class_get_handle_by_id(DM_CLASS_ID), DM_HDR_CUSTOM_DEF_ADDR);

//OS_PRINTF("[OTA]set header [0x%x]\n",DM_HDR_START_ADDR);
//dm_set_header(class_get_handle_by_id(DM_CLASS_ID), DM_HDR_START_ADDR);

  /*sys ctrl init*/
  //sys_status_init();

  /* open uio */
  //uio_init();

  mtos_task_delay_ms(200);

  /* flash burning is not finished, force ota*/
  if(!net_upg_is_burn_finish())
  {
    OS_PRINTF("[OTA]:burn didn't finished!\n");
    return OTA_TRI_FORC;
  }
  
//  OS_PRINTF("[OTA]:ota main check!\n");
  return OTA_TRI_NONE;

}


void TaskStart(void* p_data)
{
  extern u32 _end;
  u32 bss_end = (u32)(&_end);
  BOOL ret = FALSE;
//  uio_device_t *p_dev = NULL;
  u32* pstack_pnt = NULL;
//  u8 content[5] = {' ', 'O', 'N', ' ', 0};
  u32 heap_start = 0;
//  mem_mgr_partition_param_t partition_param = { 0 }, partition2_param = { 0 };

  heap_start = ROUNDUP(bss_end,4);
  mtos_irq_init();
  hal_sonata_attach();
	
  mtos_register_putchar(uart_write_byte);
  mtos_register_getchar(uart_read_byte);

  mtos_ticks_init(SYS_CPU_CLOCK);

  save_boot_infor();

  uart_init(0);
  uart_init(1);

  mtos_printk("\r\n====OTA TaskStart====\n");
  mtos_printk("\n 1. mem_start[%x]\n", heap_start);
  mtos_printk("\n 2. system memory size[%x]\n", SYS_MEMORY_END);


  if((heap_start + SYS_PARTITION_SIZE) > (AV_BUFFER_ADDR + 0x80000000))
  {
    mtos_printk("system partition is overlaped %d\n", (heap_start + SYS_PARTITION_SIZE) - AV_BUFFER_ADDR - 0x80000000);
    MT_ASSERT(0);
  }

  dlmem_init((void *)heap_start, SYS_PARTITION_SIZE);
  mtos_mem_init((void *)dl_malloc, dl_free);
	
  //register malloc/free function, using system partition

  //mtos_nc_mem_init(mt_mem_nc_malloc, mt_mem_nc_free);
  //mtos_mem_init(mt_mem_nc_malloc, mt_mem_nc_free);
  mtos_printk("init mem22 ok!\n");


 // p_DUAL_CORE_PRINTK_LOCK = hal_spin_lock_init(0);
 // mtos_printk("\n DUAL_CORE_PRINTK_LOCK[0x%x]\n", p_DUAL_CORE_PRINTK_LOCK);

//enable interrupt
  mtos_irq_enable(TRUE);
  OS_PRINTF("Enable irq!\n");


   //init message queue
  ret = mtos_message_init();
  MT_ASSERT(FALSE != ret);


#ifdef CORE_DUMP_DEBUG
  //start statistic task, MUST BE in the first task!
  mtos_stat_init();
#endif

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

  mtos_task_sleep(25);

  ota_tri = ota_check();
  //ota_tri = OTA_TRI_FORC;
  OS_PRINTF("OTA check %d\n", ota_tri);
  //{
  //  ota_info_t otai = {0};
  //  ota_read_otai(&otai);
  //  otai.ota_tri = OTA_TRI_FORC;
  //  otai.sys_mode = SYS_DVBS;
  //  ota_write_otai(&otai);
  //}
//   p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
//   MT_ASSERT(NULL != p_dev);
  if(ota_tri == OTA_TRI_NONE)
  {
    //clear frontpanel.
//    uio_display(p_dev, "    ", 4);
    reback_to_boot();
  }
  else
  {
 //   uio_display(p_dev, content, 4);
    ap_init();
  }

  mtos_task_exit( );

}

s32 hal_uart_sonata_write_byte(u8 id, u8 chval);

extern void EXCEP_vInstallIntInRam_sonata(void);
int main(int p1, char **p2, char **p3)
{
  mtos_cfg_t os_cfg = {0};
  EXCEP_vInstallIntInRam_sonata();

  hal_uart_sonata_write_byte(0, '0');
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
