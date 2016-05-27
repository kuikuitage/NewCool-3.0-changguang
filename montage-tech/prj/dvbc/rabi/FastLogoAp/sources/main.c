#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "sys_regs_concerto.h"
#include "../../includes/sys_devs.h"
#include "sys_cfg.h"
#include "driver.h"
#include "common.h"
#include "drv_dev.h"
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

//boot code start
#define PARA_ADDRESS   (0xbfef0180)
#define JUMP(dest) {__asm__ __volatile__ ("j %0" : : "d" (dest));  __asm__("nop");}
#define REBACK_BOOT_FLAG  (0x1234)

char buildSTR[]=__DATE__"--"__TIME__;

u32 boot_jump_ID = 0x0;
u32 back_addr = 0x0;
volatile u32* boot_para1 = NULL;//jump to which code
volatile u32* boot_para2 = NULL;//jump address
volatile u32* boot_para3 = NULL;//HDMI flag
volatile u32* boot_para4 = NULL;//memory location for lzma

extern BOOL  check_key(void);

void reback_to_boot(u8 block_id)
{
  u32 p_sr;

  hal_timer_release(0);
  hal_timer_release(1);
  hal_timer_release(2);
  hal_timer_release(3);
  if(USB_TOOL_BLOCK_ID == block_id)
  {
    *boot_para1 = 0x12340000 | USB_TOOL_BLOCK_ID;
    *boot_para2 = 0x80008000 + 0x1000000;// + 16M
    *boot_para3 = 0x12340000 | 0x0001;//HDMI
  //  OS_PRINTF("goto usb update\n");
  }
  else
  {
    *boot_para1 = 0x12340000 | block_id;
    *boot_para2 = 0x80008000;
    *boot_para3 = 0x12340000 | 0x0001;
   // OS_PRINTF("goto ota\n");
  }
    
  *boot_para4 = 0x80A00000; //memory location for lzma, 10M
  *((volatile u32 *)(PARA_ADDRESS)) = (REBACK_BOOT_FLAG << 16) | boot_jump_ID;

  mtos_critical_enter(&p_sr);
 // OS_PRINTF("boot_para3=%x,*boot_para3=%x\n",boot_para3,*boot_para3);
  JUMP(back_addr);
}

extern void Task_SysTimer(void* pData);

u32 get_mem_addr(void)
{
  return 0x80000000;
}

void Task_BusMonitor(void *pData)
{
    while(1)
    {
        if(0x31 == *(volatile u32 *)0xbfd00000)
          hal_bus_mon_start(HAL_BUS_MON_WHOLE_THROUGHPUT,0,0);

        mtos_task_sleep(1000);

        if(0x31 == *(volatile u32 *)0xbfd00000)
          hal_bus_mon_stop(HAL_BUS_MON_WHOLE_THROUGHPUT);

    }
}

u32 get_flash_addr(void)
{
  return 0x9e000000;
}
extern void  OSStart (void);
extern void Task_SysTimer(void* pData);
extern void ap_init(void);

s32 hal_uart_warriors_b0_write_byte(u8 id, u8 chval);
void gpio_mux(void)
{
    /* uart1 mux 30:28, 26:24 all 1s */
  *(volatile unsigned int *)(0xbf15600c) &= ~0x77000000;
  *(volatile unsigned int *)(0xbf15600c) |= 0x11000000;
}

char TaskStartStk[SYS_INIT_TASK_STKSIZE] __attribute__ ((aligned (8)));

void TaskStart (void *data)
{
  extern u32 _end;
  u32 bss_end = (u32)(&_end);
  BOOL ret = FALSE;
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

  OS_PRINTF("\n FastLogo app start\n");
  OS_PRINTF("\n Built at %s \n", buildSTR);

  OS_PRINTF("\n 1. heap_start[0x%x]\n", heap_start);
  OS_PRINTF("\n 2. whole memory size[0x%x]\n", (WHOLE_MEM_END & 0xFFFFFFF));

  OS_PRINTF("\n 3. SYS_PARTITION_SIZE is 0x%x, GUI_RESOURCE_BUFFER_ADDR is 0x%x\n", SYS_PARTITION_SIZE, GUI_RESOURCE_BUFFER_ADDR);

  if((heap_start + SYS_PARTITION_SIZE) > (GUI_RESOURCE_BUFFER_ADDR + 0x80000000))
  {
    OS_PRINTF("system partition is overlaped %d\n", (heap_start + SYS_PARTITION_SIZE) - GUI_RESOURCE_BUFFER_ADDR - 0x80000000);
    MT_ASSERT(0);
  }
  //init memory manager, only 11M
  //mem_mgr_init((u8 *)heap_start, SYS_PARTITION_SIZE + (1 * 1024 * 1024));
  OS_PRINTF("\n 4. free mem size 0x%x address from 0x%x to 0x%x\n", 
  (GUI_RESOURCE_BUFFER_ADDR | 0x80000000) - (heap_start + SYS_PARTITION_SIZE),
  (heap_start + SYS_PARTITION_SIZE),(GUI_RESOURCE_BUFFER_ADDR | 0x80000000));


  mem_mgr_init((u8 *)heap_start,
  SYS_PARTITION_SIZE);

  OS_PRINTF("\n 5. mem_mgr_start[0x%x],size=0x%x\n", heap_start,
  	SYS_PARTITION_SIZE);
  dlmem_init((void *)heap_start,
  	SYS_PARTITION_SIZE);
  mtos_mem_init(dl_malloc, dl_free);
  OS_PRINTF("init mem ok!\n");


//enable interrupt
  mtos_irq_enable(TRUE);

  //init message queue
  ret = mtos_message_init();
  MT_ASSERT(FALSE != ret);


  ap_init();

  mtos_task_exit();
}

extern void EXCEP_vInstallIntInRam(void);

int main(int p1, char **p2, char **p3)
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

