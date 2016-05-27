/*
 * Copyright (c) 2007 The FFmpeg Project
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */


#ifndef AVFORMAT_NETWORK_H
#define AVFORMAT_NETWORK_H
#include <errno.h>
#include "config.h"
#include "libavutil/error.h"


#if 0
#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef EPROTONOSUPPORT
# undef EPROTONOSUPPORT
#endif
#define EPROTONOSUPPORT WSAEPROTONOSUPPORT
#ifdef ETIMEDOUT
# undef ETIMEDOUT
#endif
#define ETIMEDOUT       WSAETIMEDOUT
#ifdef ECONNREFUSED
# undef ECONNREFUSED
#endif
#define ECONNREFUSED    WSAECONNREFUSED
#ifdef EINPROGRESS
# undef EINPROGRESS
#endif
#define EINPROGRESS     WSAEINPROGRESS

int ff_neterrno(void);
#else
#ifdef __LINUX__
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#else
#include "sys_types.h"
#include "sys_define.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

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

extern int sprintf(char *s, const char *format, ...);
extern int snprintf(char *buf, size_t size, const char *fmt, ...);
//extern int sscanf (const char * __s, char * __format, ...);

#define HAVE_CLOSESOCKET 1
#endif
#include "os_support.h"


#ifndef __LINUX__
#define ff_neterrno() (-1)
#else
#define ff_neterrno() AVERROR(errno)
#endif
#endif

#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#if HAVE_POLL_H
#include <poll.h>
#endif

int ff_socket_nonblock(int socket, int enable);

extern int ff_network_inited_globally;
int ff_network_init(void);
void ff_network_close(void);

void ff_tls_init(void);
void ff_tls_deinit(void);

int ff_network_wait_fd(int fd, int write);

int ff_inet_aton (const char * str, struct in_addr * add);



/* getaddrinfo constants */
#ifndef EAI_FAIL
#define EAI_FAIL 4
#endif

#ifndef EAI_FAMILY
#define EAI_FAMILY 5
#endif

#ifndef EAI_NONAME
#define EAI_NONAME 8
#endif

#ifndef AI_PASSIVE
#define AI_PASSIVE 1
#endif

#ifndef AI_CANONNAME
#define AI_CANONNAME 2
#endif

#ifndef AI_NUMERICHOST
#define AI_NUMERICHOST 4
#endif

#ifndef NI_NOFQDN
#define NI_NOFQDN 1
#endif

#ifndef NI_NUMERICHOST
#define NI_NUMERICHOST 2
#endif

#ifndef NI_NAMERQD
#define NI_NAMERQD 4
#endif

#ifndef NI_NUMERICSERV
#define NI_NUMERICSERV 8
#endif

#ifndef NI_DGRAM
#define NI_DGRAM 16
#endif

#if !1
int ff_getaddrinfo(const char *node, const char *service,
                   const struct addrinfo *hints, struct addrinfo **res);
void ff_freeaddrinfo(struct addrinfo *res);
int ff_getnameinfo(const struct sockaddr *sa, int salen,
                   char *host, int hostlen,
                   char *serv, int servlen, int flags);
const char *ff_gai_strerror(int ecode);
#define getaddrinfo ff_getaddrinfo
#define freeaddrinfo ff_freeaddrinfo
#define getnameinfo ff_getnameinfo
#define gai_strerror ff_gai_strerror
#endif

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN INET_ADDRSTRLEN
#endif

#ifndef IN_MULTICAST
#define IN_MULTICAST(a) ((((uint32_t)(a)) & 0xf0000000) == 0xe0000000)
#endif
#ifndef IN6_IS_ADDR_MULTICAST
#define IN6_IS_ADDR_MULTICAST(a) (((uint8_t *) (a))[0] == 0xff)
#endif

int ff_is_multicast_address(struct sockaddr *addr);

#endif /* AVFORMAT_NETWORK_H */
