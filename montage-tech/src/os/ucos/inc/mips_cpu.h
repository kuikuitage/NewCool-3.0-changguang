/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __MIPS_CPU_H__
#define __MIPS_CPU_H__

#include <stddef.h>
#include "sys_types.h"


/*!
  MIPS common registers
  */
typedef struct {
  volatile u32 reg_ff;    volatile u32 reg_3;
  volatile u32 reg_4;     volatile u32 reg_5;
  volatile u32 reg_6;     volatile u32 reg_7;
  volatile u32 reg_8;     volatile u32 reg_9;
  volatile u32 reg_10;    volatile u32 reg_11;
  volatile u32 reg_12;    volatile u32 reg_13;
  volatile u32 reg_14;    volatile u32 reg_15;
  volatile u32 reg_16;    volatile u32 reg_17;
  volatile u32 reg_18;    volatile u32 reg_19;
  volatile u32 reg_20;    volatile u32 reg_21;
  volatile u32 reg_22;    volatile u32 reg_23;
  volatile u32 reg_24;    volatile u32 reg_25;
  volatile u32 reg_28;    volatile u32 reg_30;
  volatile u32 reg_31;    volatile u32 reg_at;
  
  volatile u32 reg_pc;
  volatile u32 reg_hi;    volatile u32 reg_lo;
  volatile u32 reg_2;
} OS_REGS;

/*!
  Map the $sp register to a register variable. $SP is used to point to the top of stack where all the registers are saved
  */
register OS_REGS *$SP __asm__("$sp");


/*!
  Save speicific register value to stack
  */
#define SAVE(reg)                                       \
  __asm__ volatile (".set   noat    \n" \
                    "sw   $"#reg",%0($sp) \n"     \
                    ".set   at"                     \
                    :                                   \
                    : "i"(offsetof(OS_REGS, reg_ ## reg)))

/*!
  Restore speicific register value from stack
  */
#define RESTORE(reg)          \
  __asm__ volatile (".set   noat    \n"     \
                    "lw   $"#reg",%0($sp) \n"   \
                    ".set   at"                     \
                    :                             \
                    : "i"(offsetof(OS_REGS, reg_ ## reg)))

/*!
  Save MDU HI/LO register value to stack
  */
#define SAVE_HILO()         \
  __asm__ volatile ( "mfhi $2    \n" \
                     "mflo  $3    \n" \
                     "sw    $2,%0($sp)  \n" \
                     "sw    $3,%1($sp)  \n" \
                     : : "i"(offsetof(OS_REGS, reg_hi)), \
                     "i"(offsetof(OS_REGS, reg_lo)))

/*!
  Restore MDU HI/LO register value from stack
  */
#define RESTORE_HILO()          \
  __asm__ volatile ( "lw   $2,%0($sp)  \n" \
                     "lw    $3,%1($sp)  \n" \
                              "mthi $2    \n" \
                     "mtlo  $3    \n" \
                     : : "i"(offsetof(OS_REGS, reg_hi)),  \
                     "i"(offsetof(OS_REGS, reg_lo)))

/*!
  Save non volatile register value
  */
#define SAVE_NON_VOLATILE(RetAddr)    \
  __asm__ ("addiu $29,$29,-128\n");   \
  __asm__ ("sw %0,112($29)"::"d"(RetAddr)); \
  SAVE(31); SAVE(30);           \
  SAVE(23); SAVE(22);           \
  SAVE(21); SAVE(20);             \
  SAVE(19); SAVE(18);           \
  SAVE(17); SAVE(16);

/*!
  Save all register value to stack
  */
#define SAVE_ALL_NEW(RetAddr)     \
  __asm__ ("addiu $29,$29,-128\n");   \
  __asm__ ("sw %0,112($29)"::"d"(RetAddr)); \
  SAVE(31); SAVE(30);           \
  SAVE(23); SAVE(22);           \
  SAVE(21); SAVE(20);             \
  SAVE(19); SAVE(18);           \
  SAVE(17); SAVE(16);               \
  SAVE(25); SAVE(24);       \
  SAVE(15); SAVE(14);       \
  SAVE(13); SAVE(12);       \
  SAVE(11); SAVE(10);       \
  SAVE(9);  SAVE(8);              \
  SAVE(7);  SAVE(6);        \
  SAVE(5);  SAVE(4);        \
  SAVE(3);  SAVE(2);        \
  SAVE(at); SAVE(28);       \
  SAVE_HILO();

/*!
  Restore register value from stack
  */
#define RESTORE_ALL(RetAddr)      \
  RESTORE_HILO();           \
  RESTORE(31);  RESTORE(30);      \
  RESTORE(25);  RESTORE(24);      \
  RESTORE(23);  RESTORE(22);      \
  RESTORE(21);  RESTORE(20);      \
  RESTORE(19);  RESTORE(18);      \
  RESTORE(17);  RESTORE(16);      \
  RESTORE(15);  RESTORE(14);      \
  RESTORE(13);  RESTORE(12);      \
  RESTORE(11);  RESTORE(10);      \
  RESTORE(9);   RESTORE(8);     \
  RESTORE(7);   RESTORE(6);     \
  RESTORE(5);   RESTORE(4);     \
  RESTORE(3);   RESTORE(2);     \
  RESTORE(at);  RESTORE(28);      \
  __asm__ ("lw %0,112($29)":"=d"(RetAddr)); \
  __asm__ ("addiu $29,$29,128\n");
  

/*!
  return instruction 
  */ 
#define RET(RetAddr)      \
  __asm__ __volatile__ (      \
      ".set noreorder\n"  \
      "jr %0\n"   \
      "nop\n"   \
      ".set reorder\n"  \
      ::"d"(RetAddr));

/*!
  Execute return from interrupt
  */ 
#define ERET(RetAddr)   \
  __asm__ __volatile__ (          \
      ".set noreorder\n"      \
      "mtc0 %0,$14\n"     \
      "cop0 0x18\n" /*eret*/  \
      "nop\n" /*nop*/  \
      "nop\n" /*nop*/  \
      "nop\n" /*nop*/  \
      "nop\n" /*nop*/  \
      ".set reorder\n"      \
      ::"d"(RetAddr));


/*!
  Get mask register
  Note: our mtc0 and mfc0 are defined differ to mips32, and they are defined as follow
  mfc0 rt,rd  GPR[rd] = CPR[rt] //differ to mips32 definition
  mtc0 rt,rd  CPR[rd] = GPR[rt] //follow the mips32 definition
  */ 
#define GET_MASK(mask) { __asm__ __volatile__ ("mfc0 $17,%0" : "=d"(mask));}

/*!
  Set mask register
  Note: our mtc0 and mfc0 are defined differ to mips32, and they are defined as follow
  mfc0 rt,rd  GPR[rd] = CPR[rt] //differ to mips32 definition
  mtc0 rt,rd  CPR[rd] = GPR[rt] //follow the mips32 definition
  */ 
#define SET_MASK(mask) {__asm__ __volatile__ ("mtc0 %0,$17" : : "d"(mask));}

/*!
  Get saved mask register
  */ 
#define GET_SR_SAVED(sr) { __asm__ __volatile__ ("mfc0 $18,%0" : "=d"(sr));}

/*!
  Save mask register
  */ 
#define SET_SR_SAVED(sr) {__asm__ __volatile__ ("mtc0 %0,$18" : : "d"(sr));}

/*!
  CPU instruction NOP
  */ 
#define ASM_NOP {__asm__ __volatile__ ("nop" : : );}

/*!
  2 CPU NOP instruction
  */
#define ASM_2NOP \
__asm__ __volatile__ (  \
      "nop \n"    \
      "nop \n"    \
      : : );

/*!
  Get CP0 status regiester value
  */
#define GET_SR(sr) { __asm__ __volatile__ ("mfc0 $12,%0" : "=d"(sr));}

/*!
  set CP0 register value
  */
#define SET_SR(sr) {__asm__ __volatile__ ("mtc0 %0,$12" : : "d"(sr));}

#endif //__HAL_CPU_H__
