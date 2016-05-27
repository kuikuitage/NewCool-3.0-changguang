#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#ifndef __LINUX__
#ifdef WITH_TCPIP_PROTOCOL
#include "ethernet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#endif
#endif
#ifndef __LINUX__
#include "mp_func_trans.h"
#endif
#define REC_CNT 64
#ifndef __LINUX__
static struct socket_info rec_sockets[REC_CNT];
static  unsigned int sock_lock = 0;
    static int inited = 0;
int record_new_socket(int domain, int type, int protocol, SOCKETINFO_TYPE info_type)
{
    static int inited = 0;
    int i;
    int fd = -1;
#ifdef WITH_TCPIP_PROTOCOL
    if (inited == 0) {
        memset(rec_sockets, 0, REC_CNT * sizeof(struct socket_info));
        mtos_sem_create(&(sock_lock), TRUE);
        inited = 1;
    }
        mtos_sem_take((os_sem_t *)(&(sock_lock)), 0);
    for (i = 0; i < REC_CNT; i++) {
        if (rec_sockets[i].type == SOCKET_IDLE) {
    
            rec_sockets[i].type = info_type;
            
            fd = socket(domain, type, protocol);
            rec_sockets[i].no = fd;
            
            break;
        }
    }
mtos_sem_give((os_sem_t *)(&(sock_lock)));
 mtos_printk("\n@@@@%s %d no:%d\n", __func__, __LINE__,fd);
    if (i >= REC_CNT) {
        mtos_printk("\n@@@@%s %d recorded sockets too more\n", __func__, __LINE__);
    }
#endif
    return fd;
}

int delete_socket(int no)
{
  
    int i;
 mtos_printk("\n@@@@%s %d no:%d\n", __func__, __LINE__,no);

#ifdef WITH_TCPIP_PROTOCOL
            mtos_sem_take((os_sem_t *)(&(sock_lock)), 0);
    for (i = 0; i < REC_CNT; i++) {
        if (rec_sockets[i].no == no) {

            rec_sockets[i].type = SOCKET_IDLE;
            closesocket(no);
            
            break;
        }
    }
mtos_sem_give((os_sem_t *)(&(sock_lock)));
    if (i >= REC_CNT) {
        mtos_printk("\n@@@@%s %d not find socket\n", __func__, __LINE__);
    }
#endif
    return 0;
}

void
mt_freeaddrinfo(struct addrinfo *ai)
{
#ifdef WITH_TCPIP_PROTOCOL
    if (inited == 0) {
        memset(rec_sockets, 0, REC_CNT * sizeof(struct socket_info));
        mtos_sem_create(&(sock_lock), TRUE);
        inited = 1;
    }
     mtos_sem_take((os_sem_t *)(&(sock_lock)), 0);
     freeaddrinfo(ai);
     mtos_sem_give((os_sem_t *)(&(sock_lock)));
#endif
}
int modify_sockets(SOCKETINFO_TYPE type)
{
#ifdef WITH_TCPIP_PROTOCOL
    int i = 0;
    struct timeval to;
    to.tv_sec = 50;
 mtos_printk("\n@@@@%s %d \n", __func__, __LINE__);
    if (inited == 0) {
        memset(rec_sockets, 0, REC_CNT * sizeof(struct socket_info));
        mtos_sem_create(&(sock_lock), TRUE);
        inited = 1;
    }
   mtos_sem_take((os_sem_t *)(&(sock_lock)), 0);
    for (i = 0; i < REC_CNT; i++) {
        if (rec_sockets[i].type == type) {
            setsockopt(rec_sockets[i].no, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to));
            setsockopt(rec_sockets[i].no, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof(to));
         
        }
    }
mtos_sem_give((os_sem_t *)(&(sock_lock)));
#endif
    return 0;
}
#endif
#if 1
#if 0
char * strdup(const char * s)
{
    char * new_string = NULL;

    if (s == NULL) {
        return NULL;
    }

    new_string = malloc(strlen(s) + 1);

    if (new_string) {
        memset(new_string, 0, strlen(s) + 1);
        strcpy(new_string, s);
    }

    return new_string;
}
#endif
char * mtstrdup(const char * s)
{
    char * new_string = NULL;

    if (s == NULL) {
        return NULL;
    }

    new_string = mtos_malloc(strlen(s) + 1);

    if (new_string) {
        memset(new_string, 0, strlen(s) + 1);
        strcpy(new_string, s);
    }

    return new_string;
}

#endif//peacer del 20130812
