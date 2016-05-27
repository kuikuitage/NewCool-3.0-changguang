/*
*********************************************************************************************************
*                                               uC/OS-II
*                                        The Real-Time Kernel
*
*                         (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                          All Rights Reserved
*
*                                           MASTER INCLUDE FILE
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               MIPS Port
*
*                 Target           : MIPS (Includes 4Kc)
*                 Ported by        : Michael Anburaj
*                 URL              : http://geocities.com/michaelanburaj/    Email : michaelanburaj@hotmail.com
*
*********************************************************************************************************
*/

#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#include    <string.h>
#include	  <stdio.h>
#include	  <stdlib.h>
#include	  <ctype.h>

#ifndef ENSEMBLE
#include    "os_cpu_warriors.h"
#include    "os_cfg_291.h"
#include    "ucos_ii_291.h"
#else
#include    "os_cpu.h"
#include    "os_cfg.h"
#include    "ucos_ii.h"
#endif


#endif /*__INCLUDES_H__*/

