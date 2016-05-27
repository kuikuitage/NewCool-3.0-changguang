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
// "mTunnel" multicast access service
// Copyright (c) 1996-2012 Live Networks, Inc.  All rights reserved.
// Helper routines to implement 'group sockets'
// Implementation

#ifndef __LINUX__
extern "C"{

#include "lwip/opt.h"
#include "lwip/netdb.h"
#include "sys_define.h"
#include "sys_types.h"
#include "mtos_misc.h"
#include "mp_func_trans.h"

}


#endif
#include "sys_types.h"
#include "sys_define.h"
#include "mt_debug.h"
#include "ethernet.h"


extern "C"  void  get_net_device_addr_info(ethernet_device_t *p_dev,
                                                            char *ipaddr,
                                                            char  *netmask,
                                                            char *gw,
                                                            char *primarydns,
                                                            char *alternatedns);
#include "mtos_misc.h"

#ifdef INADDR_NONE
#undef INADDR_NONE
#endif

#include "GroupsockHelper.hh"

#ifndef __LINUX__
#define closeSocket closesocket
#endif

#if defined(__WIN32__) || defined(_WIN32)
#include <time.h>
extern "C" int initializeWinsockIfNecessary();
#else
#include <stdarg.h>
#include <time.h>
#include <fcntl.h>
#define initializeWinsockIfNecessary() 1

#ifndef __LINUX__
extern "C" int os_ticks_get();
extern "C" void os_task_sched_debug_enable(u32 enable);
#endif

#endif
#ifdef __LINUX__
#include <stdio.h>
#endif

#ifndef __LINUX__
//add just for compiling success
int fcntl64(int fildes, int cmd, ...)
{
	return 0;
}
#endif
extern int gettimeofday_replace(struct timeval* tp, int* /*tz*/) ;
// By default, use INADDR_ANY for the sending and receiving interfaces:
netAddressBits SendingInterfaceAddr = INADDR_ANY;
netAddressBits ReceivingInterfaceAddr = INADDR_ANY;

static void socketErr(UsageEnvironment& env, char const* errorMsg) {
	env.setResultErrMsg(errorMsg);
}

NoReuse::NoReuse(UsageEnvironment& env)
	: fEnv(env) {
		groupsockPriv(fEnv)->reuseFlag = 0;
	}

NoReuse::~NoReuse() {
	groupsockPriv(fEnv)->reuseFlag = 1;
	reclaimGroupsockPriv(fEnv);
}


_groupsockPriv* groupsockPriv(UsageEnvironment& env) {
	if (env.groupsockPriv == NULL) { // We need to create it
		_groupsockPriv* result = new _groupsockPriv;
		result->socketTable = NULL;
		result->reuseFlag = 1; // default value => allow reuse of socket numbers
		env.groupsockPriv = result;
	}
	return (_groupsockPriv*)(env.groupsockPriv);
}

void reclaimGroupsockPriv(UsageEnvironment& env) {
	_groupsockPriv* priv = (_groupsockPriv*)(env.groupsockPriv);
	if (priv->socketTable == NULL && priv->reuseFlag == 1/*default value*/) {
		// We can delete the structure (to save space); it will get created again, if needed:
		delete priv;
		env.groupsockPriv = NULL;
	}
}

static int createSocket(int type) {
	// Call "socket()" to create a (IPv4) socket of the specified type.
	// But also set it to have the 'close on exec' property (if we can)
	int sock;

#ifdef SOCK_CLOEXEC
	sock = socket(AF_INET, type|SOCK_CLOEXEC, 0);
	if (sock != -1 || errno != EINVAL) return sock;
	// An "errno" of EINVAL likely means that the system wasn't happy with the SOCK_CLOEXEC; fall through and try again without it:
#endif

	sock = socket(AF_INET, type, 0);
#ifdef FD_CLOEXEC
	if (sock != -1) fcntl(sock, F_SETFD, FD_CLOEXEC);
#endif
	return sock;
}

int setupDatagramSocket(UsageEnvironment& env, Port port) {
	if (!initializeWinsockIfNecessary()) {
		socketErr(env, "Failed to initialize 'winsock': ");
		return -1;
	}

	int newSocket = createSocket(SOCK_DGRAM);
	if (newSocket < 0) {
		socketErr(env, "unable to create datagram socket: ");
		return newSocket;
	}

	int reuseFlag = groupsockPriv(env)->reuseFlag;
	reclaimGroupsockPriv(env);
#if 0
	if (setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR,
				(const char*)&reuseFlag, sizeof reuseFlag) < 0) {
		socketErr(env, "setsockopt(SO_REUSEADDR) error: ");
		closeSocket(newSocket);
		return -1;
	}
#endif
#if defined(__WIN32__) || defined(_WIN32)
	// Windoze doesn't properly handle SO_REUSEPORT or IP_MULTICAST_LOOP
#else
#if 0
#ifdef SO_REUSEPORT
	if (setsockopt(newSocket, SOL_SOCKET, SO_REUSEPORT,
				(const char*)&reuseFlag, sizeof reuseFlag) < 0) {
		socketErr(env, "setsockopt(SO_REUSEPORT) error: ");
		closeSocket(newSocket);
		return -1;
	}
#endif
#endif
#if 0
#ifdef IP_MULTICAST_LOOP
	const u_int8_t loop = 1;
	if (setsockopt(newSocket, IPPROTO_IP, IP_MULTICAST_LOOP,
				(const char*)&loop, sizeof loop) < 0) {
		socketErr(env, "setsockopt(IP_MULTICAST_LOOP) error: ");
		closeSocket(newSocket);
		return -1;
	}
#endif
#endif
#endif

	// Note: Windoze requires binding, even if the port number is 0
	netAddressBits addr = INADDR_ANY;
#if defined(__WIN32__) || defined(_WIN32)
#else
	if (port.num() != 0 || ReceivingInterfaceAddr != INADDR_ANY) {
#endif
		if (port.num() == 0) addr = ReceivingInterfaceAddr;
		MAKE_SOCKADDR_IN(name, addr, port.num());
		if (bind(newSocket, (struct sockaddr*)&name, sizeof name) != 0) {
			char tmpBuffer[100];
			sprintf(tmpBuffer, "bind() error (port number: %d): ",
					ntohs(port.num()));
			socketErr(env, tmpBuffer);
			closeSocket(newSocket);
			return -1;
		}
#if defined(__WIN32__) || defined(_WIN32)
#else
	}
#endif

	// Set the sending interface for multicasts, if it's not the default:
	if (SendingInterfaceAddr != INADDR_ANY) {
		struct in_addr addr;
		addr.s_addr = SendingInterfaceAddr;

		if (setsockopt(newSocket, IPPROTO_IP, IP_MULTICAST_IF,
					(const char*)&addr, sizeof addr) < 0) {
			socketErr(env, "error setting outgoing multicast interface: ");
			closeSocket(newSocket);
			return -1;
		}
	}

	return newSocket;
}

	Boolean makeSocketNonBlocking(int sock) {
#if defined(__WIN32__) || defined(_WIN32)
		unsigned long arg = 1;
		return ioctlsocket(sock, FIONBIO, &arg) == 0;
#elif defined(VXWORKS)
		int arg = 1;
		return ioctl(sock, FIONBIO, (int)&arg) == 0;
#else
		int curFlags = fcntl(sock, F_GETFL, 0);
		return fcntl(sock, F_SETFL, curFlags|O_NONBLOCK) >= 0;
#endif
	}

	Boolean makeSocketBlocking(int sock) {
#if defined(__WIN32__) || defined(_WIN32)
		unsigned long arg = 0;
		return ioctlsocket(sock, FIONBIO, &arg) == 0;
#elif defined(VXWORKS)
		int arg = 0;
		return ioctl(sock, FIONBIO, (int)&arg) == 0;
#else
		int curFlags = fcntl(sock, F_GETFL, 0);
		return fcntl(sock, F_SETFL, curFlags&(~O_NONBLOCK)) >= 0;
#endif
	}

int setupStreamSocket(UsageEnvironment& env,
		Port port, Boolean makeNonBlocking) {
	LIV555_DEBUG("[%s]------------start!\n",__func__);                      
	if (!initializeWinsockIfNecessary()) {
		socketErr(env, "Failed to initialize 'winsock': ");
		OS_PRINTF("[%s]------------Failed to initialize 'winsock':, return -1!\n",__func__);
		return -1;
	}
	LIV555_DEBUG("[%s]------------1\n",__func__); 
	int newSocket = createSocket(SOCK_STREAM);
	if (newSocket < 0) {
		socketErr(env, "unable to create stream socket: ");
		OS_PRINTF("[%s]------------unable to create stream socket, newSocket:%d\n",__func__,newSocket);
		return newSocket;
	}
	LIV555_DEBUG("[%s]------------2\n",__func__); 
	int reuseFlag = groupsockPriv(env)->reuseFlag;
	reclaimGroupsockPriv(env);
#if 0  
	if (setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR,
				(const char*)&reuseFlag, sizeof reuseFlag) < 0) {
		socketErr(env, "setsockopt(SO_REUSEADDR) error: ");
		OS_PRINTF("[%s]------------setsockopt(SO_REUSEADDR) error:, return -1!\n",__func__);
		closeSocket(newSocket);
		return -1;
	}
#endif

	LIV555_DEBUG("[%s]------------3\n",__func__); 
	// SO_REUSEPORT doesn't really make sense for TCP sockets, so we
	// normally don't set them.  However, if you really want to do this
	// #define REUSE_FOR_TCP
#ifdef REUSE_FOR_TCP
#if defined(__WIN32__) || defined(_WIN32)
	// Windoze doesn't properly handle SO_REUSEPORT
#else
#if 0
#ifdef SO_REUSEPORT
	if (setsockopt(newSocket, SOL_SOCKET, SO_REUSEPORT,
				(const char*)&reuseFlag, sizeof reuseFlag) < 0) {
		socketErr(env, "setsockopt(SO_REUSEPORT) error: ");
		closeSocket(newSocket);
		return -1;
	}
#endif
#endif
#endif
#endif

	// Note: Windoze requires binding, even if the port number is 0
#if defined(__WIN32__) || defined(_WIN32)
#else
	if (port.num() != 0 || ReceivingInterfaceAddr != INADDR_ANY) {
#endif
		MAKE_SOCKADDR_IN(name, ReceivingInterfaceAddr, port.num());
		if (bind(newSocket, (struct sockaddr*)&name, sizeof name) != 0) {
			char tmpBuffer[100];
			sprintf(tmpBuffer, "bind() error (port number: %d): ",
					ntohs(port.num()));
			socketErr(env, tmpBuffer);
			OS_PRINTF("[%s]------------bind() error (port number: %d):, return -1!\n",__func__);
			closeSocket(newSocket);
			return -1;
		}
#if defined(__WIN32__) || defined(_WIN32)
#else
	}
#endif
	LIV555_DEBUG("[%s]------------4\n",__func__); 

	if (makeNonBlocking) {
		if (!makeSocketNonBlocking(newSocket)) {
			socketErr(env, "failed to make non-blocking: ");
			OS_PRINTF("[%s]------------failed to make non-blocking: , return -1!\n",__func__);
			closeSocket(newSocket);
			return -1;
		}
	}
	LIV555_DEBUG("[%s]------------5, newSocket=%d \n",__func__,newSocket); 
	return newSocket;
}

int 	ticks_pre  = 0;
int recv_udp_cnt_pre=0;
int lost_udp_cnt_pre = 0;
extern int flag_rtsp_fill_start, while_cnt;
int udp_recv_cnt0 = 0;
int rtp_seq_no_pre = 0;
 #if 0//ndef __LINUX__ //for debugging lwip udp recv and task cost time
extern u32 recv_udp_packet_count, lost_udp_packet_count;
extern u32 cost_count[300];

#endif
int readSocket(UsageEnvironment& env,
		int socket, unsigned char* buffer, unsigned bufferSize,
		struct sockaddr_in& fromAddress) {
	int recv_cnt = 0,idx=0;
	int bytesRead;
	int lost_udp_delta, recv_udp_delta;
	SOCKLEN_T addressSize = sizeof fromAddress;
	unsigned char *p_test = NULL;
	int rtp_seq_no = 0;
	int rtp_seq_no_0 = 0;
	int rtp_seq_no_1 = 0;
	int rtp_seq_no_2 = 0;
	int lost_udp_cnt0 = 0;
	int lost_udp_cnt1 = 0;

 #if 0//ndef __LINUX__	
	if(flag_rtsp_fill_start==1){
		OS_PRINTF("[%s]start to recvform loop!  -- lost_udp[%d] recv_udp[%d] while_cnt[%d]\n",__func__,lost_udp_packet_count,recv_udp_packet_count,while_cnt);
		for(idx = 0; idx < 300; idx++)
		{
		    cost_count[idx] = 0;
		}
		os_task_sched_debug_enable(1);
	}
#endif	
	do{
		bytesRead = recvfrom(socket, (char*)buffer, bufferSize, 0,
			(struct sockaddr*)&fromAddress,
			(socklen_t *)&addressSize);

#if 0// for debugging udp recv probility when playing rtsp stream
#if 0//simulate malloc no memory
		p_test = (unsigned char *)malloc(250064);
		if(p_test == NULL)
			OS_PRINTF("[%s] no memory! \n", __func__);
		free(p_test);
		//OS_PRINTF("[%s] malloc 250064, then free it \n", __func__);
#endif
#if 1
		//OS_PRINTF("[%s]  %x %x %x %x  bytesRead:%d, recv_cnt:%d\n", __func__,buffer[0],buffer[1],buffer[2],buffer[3],bytesRead, recv_cnt);
		rtp_seq_no = buffer[2];
		rtp_seq_no = (rtp_seq_no << 8) + (unsigned int)buffer[3];
		if(rtp_seq_no != (rtp_seq_no_pre+1)){
			if(rtp_seq_no != rtp_seq_no_pre){
				//OS_PRINTF("[%s] rtp_seq_no jump! rtp_seq_no: %d, rep_seq_no_pre: %d\n",__func__,rtp_seq_no, rtp_seq_no_pre);
				}
		}
		
		if(udp_recv_cnt0 == 0){
			rtp_seq_no_0 = rtp_seq_no;
			//OS_PRINTF("[%s] rtp_seq_no_0:%d",__func__, rtp_seq_no_0);
		}
		if(recv_cnt == 0)
			udp_recv_cnt0++;
		if(recv_cnt == 1){
			rtp_seq_no_1 = rtp_seq_no;
			lost_udp_cnt0 = rtp_seq_no_1 - rtp_seq_no_0 - udp_recv_cnt0;
			OS_PRINTF("[%s]  udp stat. before rtsp_fill_es_task start:  rtp_seq_no_0[%d] rtp_seq_no_1[%d] rtp seq delta0[%d] udp_recv_cnt0[%d] lost0[%d]\n",
				__func__, rtp_seq_no_0, rtp_seq_no_1, rtp_seq_no_1 - rtp_seq_no_0, udp_recv_cnt0, lost_udp_cnt0);

		}
#endif
		if(rtp_seq_no_pre != rtp_seq_no)
			recv_cnt++;
		rtp_seq_no_pre = rtp_seq_no;
#if 0//ndef __LINUX__
		lost_udp_delta = lost_udp_packet_count - lost_udp_cnt_pre;
		recv_udp_delta = recv_udp_packet_count - recv_udp_cnt_pre;
#if 1	
{
		int 	ticks = mtos_ticks_get();
		int delta_ticks = (ticks - ticks_pre)*10;
		if(delta_ticks>30)
			OS_PRINTF("[%s]--delta ticks: %d ms, bytesRead:%d, lost_udp_delta: %d, recv_udp_delta: %d, lost udp[%d] recv udp[%d]\n",\
			__func__,delta_ticks,bytesRead,lost_udp_delta, recv_udp_delta, lost_udp_packet_count, recv_udp_packet_count);
		ticks_pre = ticks;
		
}
#endif
		
		if(lost_udp_delta > 0)
			OS_PRINTF("[%s]--lost_udp_delta: %d (%d - %d), recv udp delta: %d (%d - %d)\n",\
			__func__,lost_udp_delta, lost_udp_packet_count, lost_udp_cnt_pre, recv_udp_delta, recv_udp_packet_count,recv_udp_cnt_pre);

		recv_udp_cnt_pre = recv_udp_packet_count;
		lost_udp_cnt_pre = lost_udp_packet_count;
#endif

		if(recv_cnt == 5000){
 #if 0//ndef __LINUX__	
 			OS_PRINTF("[%s]end to recvform loop!  recv_cnt: %d -1- lost_udp[%d] recv_udp[%d] while_cnt[%d]\n",__func__,recv_cnt, lost_udp_packet_count,recv_udp_packet_count,while_cnt);
			if(flag_rtsp_fill_start==1){
				
				os_task_sched_debug_enable(0);
				for(idx = 0; idx < 300; idx++)
				{
				    if(cost_count[idx] == 0)
				        continue;
				    OS_PRINTF("[%d], cost(us):%u \n", idx, cost_count[idx]);
				    cost_count[idx] = 0;
				}

			}
#endif	
#if 1			
			rtp_seq_no_2 = rtp_seq_no;
			lost_udp_cnt1 = rtp_seq_no_2 - rtp_seq_no_1 - 5000;
			OS_PRINTF("[%s]  udp stat. before rtsp_fill_es_task start:  rtp_seq_no_0[%d] rtp_seq_no_1[%d] rtp seq delta0[%d] udp_recv_cnt0[%d] lost0[%d]\n",
			__func__, rtp_seq_no_0, rtp_seq_no_1, rtp_seq_no_1 - rtp_seq_no_0, udp_recv_cnt0, lost_udp_cnt0);
			OS_PRINTF("[%s]  udp stat. during rtsp_fill_es_task        :  rtp_seq_no_2[%d] rtp_seq_delta1[%d]  lost1[%d]\n", __func__, rtp_seq_no_2, rtp_seq_no_2 - rtp_seq_no_1, lost_udp_cnt1);
			udp_recv_cnt0 = 0;
#endif			
			flag_rtsp_fill_start=-1;
			break;
		}
#endif			
		}while(flag_rtsp_fill_start==1);
	
	//OS_PRINTF("[%s]--called recvfrom() , bytesRead: %d, socket: %d\n",__func__,bytesRead, socket);
	if (bytesRead < 0) {
		//##### HACK to work around bugs in Linux and Windows:
		int err = env.getErrno();
		OS_PRINTF("[%s]--recvfrom() error: \n",__func__,err);
		
		if (err == 111 /*ECONNREFUSED (Linux)*/
#if defined(__WIN32__) || defined(_WIN32)
				// What a piece of crap Windows is.  Sometimes
				// recvfrom() returns -1, but with an 'errno' of 0.
				// This appears not to be a real error; just treat
				// it as if it were a read of zero bytes, and hope
				// we don't have to do anything else to 'reset'
				// this alleged error:
				|| err == 0 || err == EWOULDBLOCK
#else
				|| err == EAGAIN
#endif
				|| err == 113 /*EHOSTUNREACH (Linux)*/) { // Why does Linux return this for datagram sock?
			fromAddress.sin_addr.s_addr = 0;
			return 0;
		}
		//##### END HACK
		socketErr(env, "recvfrom() error: ");
		
	}

	return bytesRead;
}

Boolean writeSocket(UsageEnvironment& env,
		int socket, struct in_addr address, Port port,
		u_int8_t ttlArg,
		unsigned char* buffer, unsigned bufferSize) {
		
	do {

		
		if (ttlArg != 0) {
			// Before sending, set the socket's TTL:
#if defined(__WIN32__) || defined(_WIN32)
#define TTL_TYPE int
#else
#define TTL_TYPE u_int8_t
#endif
			TTL_TYPE ttl = (TTL_TYPE)ttlArg;
			if (setsockopt(socket, IPPROTO_IP, IP_MULTICAST_TTL,
						(const char*)&ttl, sizeof ttl) < 0) {
				socketErr(env, "setsockopt(IP_MULTICAST_TTL) error: ");
				break;
			}
		}

		MAKE_SOCKADDR_IN(dest, address.s_addr, port.num());
		int bytesSent = sendto(socket, (char*)buffer, bufferSize, 0,
				(struct sockaddr*)&dest, sizeof dest);
		
		if (bytesSent != (int)bufferSize) {
			char tmpBuf[100];
			sprintf(tmpBuf, "writeSocket(%d), sendTo() error: wrote %d bytes instead of %u: ", socket, bytesSent, bufferSize);
			socketErr(env, tmpBuf);
			break;
		}

		return True;
		
	} while (0);

	return False;
}

static unsigned getBufferSize(UsageEnvironment& env, int bufOptName,
		int socket) {
	unsigned curSize;
	SOCKLEN_T sizeSize = sizeof curSize;
	if (getsockopt(socket, SOL_SOCKET, bufOptName,
				(char*)&curSize, (socklen_t *)&sizeSize) < 0) {
		socketErr(env, "getBufferSize() error: ");
		return 0;
	}

	return curSize;
}
unsigned getSendBufferSize(UsageEnvironment& env, int socket) {
	return getBufferSize(env, SO_SNDBUF, socket);
}
unsigned getReceiveBufferSize(UsageEnvironment& env, int socket) {
	return getBufferSize(env, SO_RCVBUF, socket);
}

static unsigned setBufferTo(UsageEnvironment& env, int bufOptName,
		int socket, unsigned requestedSize) {
	SOCKLEN_T sizeSize = sizeof requestedSize;
	setsockopt(socket, SOL_SOCKET, bufOptName, (char*)&requestedSize, sizeSize);

	// Get and return the actual, resulting buffer size:
	return getBufferSize(env, bufOptName, socket);
}
unsigned setSendBufferTo(UsageEnvironment& env,
		int socket, unsigned requestedSize) {
	return setBufferTo(env, SO_SNDBUF, socket, requestedSize);
}
unsigned setReceiveBufferTo(UsageEnvironment& env,
		int socket, unsigned requestedSize) {
	return setBufferTo(env, SO_RCVBUF, socket, requestedSize);
}

static unsigned increaseBufferTo(UsageEnvironment& env, int bufOptName,
		int socket, unsigned requestedSize) {
	// First, get the current buffer size.  If it's already at least
	// as big as what we're requesting, do nothing.
	unsigned curSize = getBufferSize(env, bufOptName, socket);

	// Next, try to increase the buffer to the requested size,
	// or to some smaller size, if that's not possible:
	while (requestedSize > curSize) {
		SOCKLEN_T sizeSize = sizeof requestedSize;
		if (setsockopt(socket, SOL_SOCKET, bufOptName,
					(char*)&requestedSize, sizeSize) >= 0) {
			// success
			return requestedSize;
		}
		requestedSize = (requestedSize+curSize)/2;
	}

	return getBufferSize(env, bufOptName, socket);
}
unsigned increaseSendBufferTo(UsageEnvironment& env,
		int socket, unsigned requestedSize) {
	return increaseBufferTo(env, SO_SNDBUF, socket, requestedSize);
}
unsigned increaseReceiveBufferTo(UsageEnvironment& env,
		int socket, unsigned requestedSize) {
	return increaseBufferTo(env, SO_RCVBUF, socket, requestedSize);
}

Boolean socketJoinGroup(UsageEnvironment& env, int socket,
		netAddressBits groupAddress){
	if (!IsMulticastAddress(groupAddress)) return True; // ignore this case

	struct ip_mreq imr;
	imr.imr_multiaddr.s_addr = groupAddress;
	imr.imr_interface.s_addr = ReceivingInterfaceAddr;
	if (setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
				(const char*)&imr, sizeof (struct ip_mreq)) < 0) {
#if defined(__WIN32__) || defined(_WIN32)
		if (env.getErrno() != 0) {
			// That piece-of-shit toy operating system (Windows) sometimes lies
			// about setsockopt() failing!
#endif
			socketErr(env, "setsockopt(IP_ADD_MEMBERSHIP) error: ");
			return False;
#if defined(__WIN32__) || defined(_WIN32)
		}
#endif
	}

	return True;
}

Boolean socketLeaveGroup(UsageEnvironment&, int socket,
		netAddressBits groupAddress) {
	if (!IsMulticastAddress(groupAddress)) return True; // ignore this case

	struct ip_mreq imr;
	imr.imr_multiaddr.s_addr = groupAddress;
	imr.imr_interface.s_addr = ReceivingInterfaceAddr;
	if (setsockopt(socket, IPPROTO_IP, IP_DROP_MEMBERSHIP,
				(const char*)&imr, sizeof (struct ip_mreq)) < 0) {
		return False;
	}

	return True;
}

// The source-specific join/leave operations require special setsockopt()
// commands, and a special structure (ip_mreq_source).  If the include files
// didn't define these, we do so here:
#if !defined(IP_ADD_SOURCE_MEMBERSHIP)
struct ip_mreq_source {
	struct  in_addr imr_multiaddr;  /* IP multicast address of group */
	struct  in_addr imr_sourceaddr; /* IP address of source */
	struct  in_addr imr_interface;  /* local IP address of interface */
};
#endif

#ifndef IP_ADD_SOURCE_MEMBERSHIP

#ifdef LINUX
#define IP_ADD_SOURCE_MEMBERSHIP   39
#define IP_DROP_SOURCE_MEMBERSHIP 40
#else
#define IP_ADD_SOURCE_MEMBERSHIP   25
#define IP_DROP_SOURCE_MEMBERSHIP 26
#endif

#endif

Boolean socketJoinGroupSSM(UsageEnvironment& env, int socket,
		netAddressBits groupAddress,
		netAddressBits sourceFilterAddr) {
	if (!IsMulticastAddress(groupAddress)) return True; // ignore this case

	struct ip_mreq_source imr;
	imr.imr_multiaddr.s_addr = groupAddress;
	imr.imr_sourceaddr.s_addr = sourceFilterAddr;
	imr.imr_interface.s_addr = ReceivingInterfaceAddr;
	if (setsockopt(socket, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP,
				(const char*)&imr, sizeof (struct ip_mreq_source)) < 0) {
		socketErr(env, "setsockopt(IP_ADD_SOURCE_MEMBERSHIP) error: ");
		return False;
	}

	return True;
}

Boolean socketLeaveGroupSSM(UsageEnvironment& /*env*/, int socket,
		netAddressBits groupAddress,
		netAddressBits sourceFilterAddr) {
	if (!IsMulticastAddress(groupAddress)) return True; // ignore this case

	struct ip_mreq_source imr;
	imr.imr_multiaddr.s_addr = groupAddress;
	imr.imr_sourceaddr.s_addr = sourceFilterAddr;
	imr.imr_interface.s_addr = ReceivingInterfaceAddr;
	if (setsockopt(socket, IPPROTO_IP, IP_DROP_SOURCE_MEMBERSHIP,
				(const char*)&imr, sizeof (struct ip_mreq_source)) < 0) {
		return False;
	}

	return True;
}

static Boolean getSourcePort0(int socket, portNumBits& resultPortNum/*host order*/) {
	sockaddr_in test; test.sin_port = 0;
	SOCKLEN_T len = sizeof test;
	if (getsockname(socket, (struct sockaddr*)&test, (socklen_t *)&len) < 0)
		return False;

	resultPortNum = ntohs(test.sin_port);
	return True;
}

Boolean getSourcePort(UsageEnvironment& env, int socket, Port& port) {
	portNumBits portNum = 0;
	if (!getSourcePort0(socket, portNum) || portNum == 0) {
		// Hack - call bind(), then try again:
		MAKE_SOCKADDR_IN(name, INADDR_ANY, 0);
		bind(socket, (struct sockaddr*)&name, sizeof name);

		if (!getSourcePort0(socket, portNum) || portNum == 0) {
			socketErr(env, "getsockname() error: ");
			return False;
		}
	}

	port = Port(portNum);
	return True;
}

static Boolean badAddressForUs(netAddressBits addr) {
	// Check for some possible erroneous addresses:
	netAddressBits nAddr = htonl(addr);
	return (nAddr == 0x7F000001 /* 127.0.0.1 */
			|| nAddr == 0
			|| nAddr == (netAddressBits)(~0));
}

Boolean loopbackWorks = 1;

netAddressBits ourIPAddress(UsageEnvironment& env) {
	static netAddressBits ourAddress = 0;
	int sock = -1;
	struct in_addr testAddr;

#ifndef  __LINUX__
	if(ourAddress)
	{	
		return ourAddress;
	}
	else
	{
	    char ipaddr[4] = {0};
	    char netmask[4]= {0};
	    char gw[4]= {0};
	    char primarydns[4]= {0};
	    char alternatedns[4]= {0};
	    ethernet_device_t *      eth_dev = (ethernet_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_ETH);
           MT_ASSERT(eth_dev != NULL);
	    get_net_device_addr_info(eth_dev,ipaddr,netmask,gw,primarydns,alternatedns);

	    ourAddress = *((unsigned int *)ipaddr);

		OS_PRINTF("[%s] 11 ourAddress[0x%x]\n",__func__,ourAddress);
		//OS_PRINTF("[%s] 22 ourAddress[0x%x]\n",__func__,our_inet_addr("192.168.32.225"));
	}
#endif
	if (ourAddress == 0) {
		// We need to find our source address
		struct sockaddr_in fromAddr;
		fromAddr.sin_addr.s_addr = 0;

		// Get our address by sending a (0-TTL) multicast packet,
		// receiving it, and looking at the source address used.
		// (This is kinda bogus, but it provides the best guarantee
		// that other nodes will think our address is the same as we do.)
		do {
			loopbackWorks = 0; // until we learn otherwise

			testAddr.s_addr = our_inet_addr("228.67.43.91"); // arbitrary
			Port testPort(15947); // ditto

			sock = setupDatagramSocket(env, testPort);
			if (sock < 0) break;

			if (!socketJoinGroup(env, sock, testAddr.s_addr)) break;

			unsigned char testString[] = "hostIdTest";
			unsigned testStringLength = sizeof testString;

			if (!writeSocket(env, sock, testAddr, testPort, 0,
						testString, testStringLength)) break;

			// Block until the socket is readable (with a 5-second timeout):
			fd_set rd_set;
			FD_ZERO(&rd_set);
			FD_SET((unsigned)sock, &rd_set);
			const unsigned numFds = sock+1;
			struct timeval timeout;
			timeout.tv_sec = 5;
			timeout.tv_usec = 0;
			int result = select(numFds, &rd_set, NULL, NULL, &timeout);
			if (result <= 0) break;

			unsigned char readBuffer[20];
			int bytesRead = readSocket(env, sock,
					readBuffer, sizeof readBuffer,
					fromAddr);
			if (bytesRead != (int)testStringLength
					|| strncmp((char*)readBuffer, (char*)testString, testStringLength) != 0) {
				break;
			}

			// We use this packet's source address, if it's good:
			loopbackWorks = !badAddressForUs(fromAddr.sin_addr.s_addr);
		} while (0);

		if (sock >= 0) {
			socketLeaveGroup(env, sock, testAddr.s_addr);
			closeSocket(sock);
		}

		if (!loopbackWorks) do {
			// We couldn't find our address using multicast loopback,
			// so try instead to look it up directly - by first getting our host name, and then resolving this host name
			char hostname[100];
			hostname[0] = '\0';
			// add by jiakun temp
			int result = 0;
#ifdef __LINUX__
			result = gethostname(hostname, sizeof hostname);
#endif
			if (result != 0 || hostname[0] == '\0') {
				env.setResultErrMsg("initial gethostname() failed");
				break;
			}

			// Try to resolve "hostname" to an IP address:
			NetAddressList addresses(hostname);
			NetAddressList::Iterator iter(addresses);
			NetAddress const* address;

			// Take the first address that's not bad:
			netAddressBits addr = 0;
			while ((address = iter.nextAddress()) != NULL) {
				netAddressBits a = *(netAddressBits*)(address->data());
				if (!badAddressForUs(a)) {
					addr = a;
					break;
				}
			}

			// Assign the address that we found to "fromAddr" (as if the 'loopback' method had worked), to simplify the code below:
			fromAddr.sin_addr.s_addr = addr;
		} while (0);

		// Make sure we have a good address:
		netAddressBits from = fromAddr.sin_addr.s_addr;
		if (badAddressForUs(from)) {
			char tmp[100];
			sprintf(tmp, "This computer has an invalid IP address: %s", AddressString(from).val());
			env.setResultMsg(tmp);
			from = 0;
		}

		ourAddress = from;

		// Use our newly-discovered IP address, and the current time,
		// to initialize the random number generator's seed:
		struct timeval timeNow;
#ifdef __LINUX__
		//gettimeofday(&timeNow, NULL);
		gettimeofday_replace(&timeNow, NULL);
#else
		gettimeofday_replace(&timeNow, NULL);
#endif
		unsigned seed = ourAddress^timeNow.tv_sec^timeNow.tv_usec;
		our_srandom(seed);
	}
	return ourAddress;
}

netAddressBits chooseRandomIPv4SSMAddress(UsageEnvironment& env) {
	// First, a hack to ensure that our random number generator is seeded:
	(void) ourIPAddress(env);

	// Choose a random address in the range [232.0.1.0, 232.255.255.255)
	// i.e., [0xE8000100, 0xE8FFFFFF)
	netAddressBits const first = 0xE8000100, lastPlus1 = 0xE8FFFFFF;
	netAddressBits const range = lastPlus1 - first;

	return ntohl(first + ((netAddressBits)our_random())%range);
}

char const* timestampString() {
	struct timeval tvNow;
#ifdef __LINUX__
	// gettimeofday(&tvNow, NULL);
	gettimeofday_replace(&tvNow, NULL);
#else
	gettimeofday_replace(&tvNow, NULL);
#endif

#if !defined(_WIN32_WCE)
	static char timeString[9]; // holds hh:mm:ss plus trailing '\0'
	char const* ctimeResult = ctime((time_t*)&tvNow.tv_sec);
	if (ctimeResult == NULL) {
		sprintf(timeString, "??:??:??");
	} else {
		char const* from = &ctimeResult[11];
		int i;
		for (i = 0; i < 8; ++i) {
			timeString[i] = from[i];
		}
		timeString[i] = '\0';
	}
#else
	// WinCE apparently doesn't have "ctime()", so instead, construct
	// a timestamp string just using the integer and fractional parts
	// of "tvNow":
	static char timeString[50];
	sprintf(timeString, "%lu.%06ld", tvNow.tv_sec, tvNow.tv_usec);
#endif

	return (char const*)&timeString;
}

#if defined(__WIN32__) || defined(_WIN32)
// For Windoze, we need to implement our own gettimeofday()
#if !defined(_WIN32_WCE)
#include <sys/timeb.h>
#endif

	int gettimeofday(struct timeval* tp, int* /*tz*/) {
#if defined(_WIN32_WCE)
		/* FILETIME of Jan 1 1970 00:00:00. */
		static const unsigned __int64 epoch = 116444736000000000LL;

		FILETIME    file_time;
		SYSTEMTIME  system_time;
		ULARGE_INTEGER ularge;

		GetSystemTime(&system_time);
		SystemTimeToFileTime(&system_time, &file_time);
		ularge.LowPart = file_time.dwLowDateTime;
		ularge.HighPart = file_time.dwHighDateTime;

		tp->tv_sec = (long) ((ularge.QuadPart - epoch) / 10000000L);
		tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
#else
		static LARGE_INTEGER tickFrequency, epochOffset;

		// For our first call, use "ftime()", so that we get a time with a proper epoch.
		// For subsequent calls, use "QueryPerformanceCount()", because it's more fine-grain.
		static Boolean isFirstCall = True;

		LARGE_INTEGER tickNow;
		LIV555_DEBUG("[%s]------------1!\n",__func__);
		QueryPerformanceCounter(&tickNow);
		LIV555_DEBUG("[%s]------------2!\n",__func__);
		if (isFirstCall) {
			struct timeb tb;
			LIV555_DEBUG("[%s]------------3!\n",__func__);
			ftime(&tb);
			LIV555_DEBUG("[%s]------------4!\n",__func__);
			tp->tv_sec = tb.time;
			tp->tv_usec = 1000*tb.millitm;

			// Also get our counter frequency:
			QueryPerformanceFrequency(&tickFrequency);

			// And compute an offset to add to subsequent counter times, so we get a proper epoch:
			epochOffset.QuadPart
				= tb.time*tickFrequency.QuadPart + (tb.millitm*tickFrequency.QuadPart)/1000 - tickNow.QuadPart;

			isFirstCall = False; // for next time
		} else {
			// Adjust our counter time so that we get a proper epoch:
			tickNow.QuadPart += epochOffset.QuadPart;

			tp->tv_sec = (long) (tickNow.QuadPart / tickFrequency.QuadPart);
			tp->tv_usec = (long) (((tickNow.QuadPart % tickFrequency.QuadPart) * 1000000L) / tickFrequency.QuadPart);
		}
#endif
		return 0;
	}
#endif

//extern "C" // mtos_ticks_get(void);
#if  1//ndef __LINUX__
int gettimeofday_replace(struct timeval* tp, int* /*tz*/) {
	static unsigned int old_ticks;

#ifdef __LINUX__
	unsigned int ticks = mtos_ticks_get();
	tp->tv_sec = ticks*10/1000;
	tp->tv_usec = (ticks*10*1000)%1000000 ;
#else

	#if 0
	u32 sec = 0;
	u32 msec = 0;
	u32 usec = 0;
	mtos_systime_get(&sec,&msec,&usec);
	tp->tv_sec = sec;
	tp->tv_usec = msec*1000+usec;
	#else
	unsigned int ticks = mtos_ticks_get();
	tp->tv_sec = ticks*10/1000;
	tp->tv_usec = (ticks*10*1000)%1000000 ;
	#endif
	
	
#endif


	return 0;
}
#endif

