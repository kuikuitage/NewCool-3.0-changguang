/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
/* "groupsock" interface
 * Copyright (c) 1996-2012 Live Networks, Inc.  All rights reserved.
 * Common include files, typically used for networking
 */

#ifndef _NET_COMMON_H
#define _NET_COMMON_H

#ifndef __LINUX__
#ifdef __cplusplus
extern "C"{
#endif
#define stderr 0
#define fprintf(stdio, fmt, args...) OS_PRINTF(fmt , ## args )
#define perror(args...) do{OS_PRINTF(args);}while(0)
#define fputs(args...) do{OS_PRINTF(args);}while(0)
#define fflush(args...) do{}while(0)
#include <sys/types.h>

#include <string.h>
#include "strings.h"
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
#include "list.h"
#include "drv_dev.h"
#include "block.h"
#include "ff.h"
#include "fsioctl.h"

#include "ufs.h"
#ifndef SEEK_SET
#  define SEEK_SET        0       /* Seek from beginning of file.  */
#  define SEEK_CUR        1       /* Seek from current position.  */
#  define SEEK_END        2       /* Set file pointer to EOF plus "offset" */
#endif
#ifndef EOF
#define EOF (-1)
#endif
typedef ufs_file_t FILE;

extern int sprintf(char *s, const char *format, ...);
extern int snprintf(char *buf, size_t size, const char *fmt, ...);
extern int sscanf (const char * __s, char * __format, ...);
#ifdef __cplusplus
}
#endif

#endif



#define LIVE555_DEBUG_PRINT
#ifdef LIVE555_DEBUG_PRINT

#ifdef __LINUX__
#define OS_PRINTF printf
#define LIV555_DEBUG(fmt ,args...)  //OS_PRINTF(fmt , ## args )
#define LIV555_LOG(fmt ,args...)  //OS_PRINTF(fmt , ## args )
#else
#define LIV555_DEBUG(fmt ,args...)  //OS_PRINTF(fmt , ## args )
#define LIV555_LOG(fmt ,args...)  //OS_PRINTF(fmt , ## args )
#endif

#else

#ifdef __LINUX__
#define OS_PRINTF(args...) do{}while(0)
#else
#define OS_PRINTF(args...) do{}while(0)

#endif

#endif


#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN32_WCE)
/* Windows */
#if defined(WINNT) || defined(_WINNT) || defined(__BORLANDC__) || defined(__MINGW32__) || defined(_WIN32_WCE)
#define _MSWSOCK_
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <windows.h>
#include <string.h>

#define closeSocket closesocket
#define EWOULDBLOCK WSAEWOULDBLOCK
#define EINPROGRESS WSAEWOULDBLOCK
#define EAGAIN WSAEWOULDBLOCK
#define EINTR WSAEINTR

#if defined(_WIN32_WCE)
#define NO_STRSTREAM 1
#endif

/* Definitions of size-specific types: */
typedef __int64 int64_t;
typedef unsigned __int64 u_int64_t;
typedef unsigned u_int32_t;
typedef unsigned short u_int16_t;
typedef unsigned char u_int8_t;
// For "uintptr_t" and "intptr_t", we assume that if they're not already defined, then this must be
// an old, 32-bit version of Windows:
#if !defined(_MSC_STDINT_H_) && !defined(_UINTPTR_T_DEFINED) && !defined(_UINTPTR_T_DECLARED) && !defined(_UINTPTR_T)
typedef unsigned uintptr_t;
#endif
#if !defined(_MSC_STDINT_H_) && !defined(_INTPTR_T_DEFINED) && !defined(_INTPTR_T_DECLARED) && !defined(_INTPTR_T)
typedef int intptr_t;
#endif

#elif defined(VXWORKS)
/* VxWorks */
#include <time.h>
#include <timers.h>
#include <sys/times.h>
#include <sockLib.h>
#include <hostLib.h>
#include <resolvLib.h>
#include <ioLib.h>

typedef unsigned int u_int32_t;
typedef unsigned short u_int16_t;
typedef unsigned char u_int8_t;

#else
/* Unix */
#include <sys/types.h>
#ifdef __LINUX__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <sys/time.h>
#ifndef __LINUX__
#include "lwip/netdb.h"
#endif
#include <unistd.h>
#include <string.h>
#ifdef __LINUX__
#include <stdlib.h>
#include <stdio.h>
#endif
#include <errno.h>
#include <strings.h>
#include <ctype.h>
#include <stdint.h>
#if defined(_QNX4)
#include <sys/select.h>
#include <unix.h>
#endif

#ifdef __LINUX__
#define closeSocket close
#endif

#ifndef __LINUX__
#define MAX_FILE_NAME_LEN 512
#endif

#ifdef SOLARIS
#define u_int64_t uint64_t
#define u_int32_t uint32_t
#define u_int16_t uint16_t
#define u_int8_t uint8_t
#endif
#endif

#ifndef SOCKLEN_T
#define SOCKLEN_T int
#endif

#define NO_SSTREAM
#endif
