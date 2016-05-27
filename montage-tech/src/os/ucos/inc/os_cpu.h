/* 
   Copyright zhangfeifei , ict of cas
   
   This code is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef __OS_CPU_H__
#define __OS_CPU_H__

#include "mips_cpu.h"

#define  OS_CPU_GLOBALS
#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif 

typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;           /* Unsigned  8 bit quantity            */
typedef signed   char  INT8S;           /* Signed    8 bit quantity            */
typedef unsigned short INT16U;          /* Unsigned 16 bit quantity            */
typedef signed   short INT16S;          /* Signed   16 bit quantity            */
typedef unsigned long  INT32U;          /* Unsigned 32 bit quantity            */
typedef signed   long  INT32S;          /* Signed   32 bit quantity            */

typedef unsigned int  OS_STK;          /* Each stack entry is 32-bit wide     */
typedef INT32U			OS_CPU_SR;	/* Equals to sizeof(PSR)			*/

#define BYTE           INT8S   	 /* Define data types for backward compatibility ...   */
#define UBYTE          INT8U     /* ... to uC/OS V1.xx.  Not actually needed for ...   */
#define WORD           INT16S    /* ... uC/OS-II.                                  */
#define UWORD          INT16U  
#define LONG           INT32S  
#define ULONG          INT32U  

#define  OS_STK_GROWTH   1          /* Stack grows from HIGH to LOW memory  */

/* 
*********************************************************************************************************
*                              MIPS
*
* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts will be enabled even if they were disabled before entering the critical section.*
* Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if interrupts were disabled before entering the critical section, they will be disabled when leaving the critical section.
*
* Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr' into the CPU's status register.
*
* Note     :  In this UCORE Port: Only Method #3 is implemeted.
*
*********************************************************************************************************
*/
INT32U sr_saved__;
/* Don't modify these lines. This port can only support OS_CRITICAL_METHOD 3. */
#define OS_CRITICAL_METHOD 3
#define OS_ENTER_CRITICAL()  {GET_SR(cpu_sr);\
                              sr_saved__ =0xfffffffe & cpu_sr;\
                              SET_SR(sr_saved__);\
                              sr_saved__ =cpu_sr;}/* Disable interrupts  */
#define OS_EXIT_CRITICAL()   {SET_SR(cpu_sr);} /* Restore interrupts  */
#define  OS_TASK_SW() OSCtxSw();

//extern void * memset(void *, int, size_t);
//extern void * memcpy(void *, const void *, size_t);

#endif //__OS_CPU_H__
