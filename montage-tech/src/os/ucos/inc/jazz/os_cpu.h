/********************************************************************************************************
*                                               uC/OS-II
*                                         The Real-Time Kernel
*
*                                       CKCORE Specific code
*
* File : os_cpu.h
* By   : Kerby Suhre
* Modify By : Fengqinghua             2006-11-21
*        For  IS1526 Support!
* Modify By : Fengqinghua             2007-01-18
*        For: IS1526 216MHz Support!  
*********************************************************************************************************/

#ifndef _OS_CPU_H_
#define _OS_CPU_H_

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;           /* Unsigned  8 bit quantity            */
typedef signed   char  INT8S;           /* Signed    8 bit quantity            */
typedef unsigned short INT16U;          /* Unsigned 16 bit quantity            */
typedef signed   short INT16S;          /* Signed   16 bit quantity            */
typedef unsigned long  INT32U;          /* Unsigned 32 bit quantity            */
typedef signed   long  INT32S;          /* Signed   32 bit quantity            */
typedef float			    FP32;
typedef double			    FP64;

/******************************************************************************
 *  CPU System Clock Definition : CPU CLOCK: in MHz 
 ******************************************************************************/
//#define SYS_AHB_CLOCK_M         81

#define SYS_AHB_CLOCK_M        (mtos_cpu_freq_get() / 1000000)


//#define SYS_AHB_CLOCK           (SYS_AHB_CLOCK_M * 1000000)

//#define SYS_APB_CLOCK_M         (SYS_AHB_CLOCK_M / 2)

//#define SYS_APB_CLOCK           (SYS_APB_CLOCK_M * 1000000)

//#define SYS_BASE_CLOCK_M		24

//#define SYS_BASE_CLOCK			(SYS_BASE_CLOCK_M * 1000000)

/******************************************************************************
 *  CPU System Clock Definition : CPU CLOCK: in MHz 
 ******************************************************************************/
#define CPU_SUPPORT_DOZE	0		// Disable calling doze mode in OSTaskIdleHook
#define	OS_STK_GROWTH   	1		// Stack growth: 1=High->Low, 0=Low->High

typedef unsigned long int	OS_STK;
typedef unsigned long int	OS_CPU_SR;	// Equals to sizeof(PSR)

#define OS_CRITICAL_METHOD	3
/************************************************************************
 * Critical Enter/Exit Control Definition
 ************************************************************************/
#if (OS_CRITICAL_METHOD == 3)

static __inline__ OS_CPU_SR get_processor_pws (void)
{
   OS_CPU_SR tmpsr;
 
   asm ("mfcr %0 ,psr":"=r"(tmpsr));

   return tmpsr;
}

static __inline__ void disable_interrupts (void)
{
   asm ("psrclr ie, fe; psrset ee");
}

static __inline__ void set_processor_psr (OS_CPU_SR cpusr)
{
   if ((cpusr & 0x80000000)==0)
   {
   	asm ("bkpt");
   }
   
   asm ("mtcr %0,psr"::"r"(cpusr));
   return ;
}
#endif 

/************************************************************************
 * Critical Enter/Exit Control Definition
 ************************************************************************/

#if	OS_CRITICAL_METHOD ==	3
OS_CPU_SR    get_processor_pws (void);
void	     disable_interrupts (void);
void	     set_processor_psr (OS_CPU_SR cpusr);

#define OS_ENTER_CRITICAL()	cpu_sr = get_processor_pws();	disable_interrupts();
#define OS_EXIT_CRITICAL()	set_processor_psr(cpu_sr);
#else
 #error "Currently we support OS_CRITICAL_METHOD 3 Only!!"
#endif

#define OS_TASK_SW()		asm("TRAP 0")

#endif	// _OS_CPU_H_ 
