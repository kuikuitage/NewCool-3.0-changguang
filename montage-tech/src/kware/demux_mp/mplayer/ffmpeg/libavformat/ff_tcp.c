/*
 * TCP protocol
 * Copyright (c) 2002 Fabrice Bellard
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
#include "avformat.h"
#include "libavutil/parseutils.h"
#include <unistd.h>
#include "internal.h"
#include "ff_network.h"
#include "os_support.h"
#include "url.h"
#if HAVE_POLL_H
#include <poll.h>
#endif
#include <sys/time.h>
#ifndef __LINUX__
#include "mp_func_trans.h"
#endif
//#include "file_playback_sequence.h"

extern ERR_LOAD g_load_err;
typedef struct TCPContext {
    int fd;
} TCPContext;
int ff_closesocket(int fd)
{
    //mtos_printk("\n[%s] socket:%d\n", __func__, fd);
    #ifdef __LINUX__
        closesocket(fd);
    #else
    //closesocket(fd);
    delete_socket(fd);
    #endif

}
/* return non zero if error */
static int tcp_open(URLContext * h, const char * uri, int flags)
{
    struct addrinfo hints = { 0 }, *ai, *cur_ai;
    ai = NULL;//peacer add
    cur_ai = NULL;
    int port, fd = -1;
    TCPContext * s = h->priv_data;
    int listen_socket = 0;
    const char * p;
    char buf[256];
    int ret;
    socklen_t optlen;
    int timeout = 50;
    char hostname[1024], proto[1024], path[1024];
    char portstr[10];
    //   printf("%s    %d  \n",__func__,__LINE__);
    av_url_split(proto, sizeof(proto), NULL, 0, hostname, sizeof(hostname),
                 &port, path, sizeof(path), uri);
    // printf("%s    %d  \n",__func__,__LINE__);

    if (strcmp(proto, "tcp") || port <= 0 || port >= 65536) {
        return AVERROR(EINVAL);
    }

    p = strchr(uri, '?');

    if (p) {
        if (av_find_info_tag(buf, sizeof(buf), "listen", p)) {
            listen_socket = 1;
        }

        if (av_find_info_tag(buf, sizeof(buf), "timeout", p)) {
            timeout = strtol(buf, NULL, 10);
        }
    }

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    snprintf(portstr, sizeof(portstr), "%d", port);
    //   printf("%s    %d  \n",__func__,__LINE__);  

        if (is_file_seq_exit()) {
               ret = AVERROR(EIO);
            goto fail1;
        }

 // mtos_printk("%s    %d  \n",__func__,__LINE__);
    ret = getaddrinfo(hostname, portstr, &hints, &ai);
    //printf("%s    %d   %d  \n",__func__,__LINE__,ret);
   //mtos_printk("\n[%s] ---called getaddrinfo()! hostname: %s\n", __func__,hostname);
//mtos_printk("asdfadsfsadfasd \n");
    if (ret) {
       // av_log(h, AV_LOG_ERROR,
           //    "Failed to resolve hostname %s: %s\n",
               //hostname, gai_strerror(ret));
        g_load_err = ERR_LOAD_RESLV_HOSTNAME_ERR;
        return AVERROR(EIO);
    }

    cur_ai = ai;
restart:
    ret = AVERROR(EIO);
       #ifdef __LINUX__
    fd = socket(cur_ai->ai_family, cur_ai->ai_socktype, cur_ai->ai_protocol);
 #else
fd=    record_new_socket(cur_ai->ai_family,cur_ai->ai_socktype,cur_ai->ai_protocol,FILE_PLAY_SOCKET);
 //fd = socket(cur_ai->ai_family, cur_ai->ai_socktype, cur_ai->ai_protocol);
    #endif
    if (fd < 0) {
        goto fail;
    }

    if (listen_socket) {
        //mtos_printk("\n[%s] ---linsten socket! \n", __func__);
        int fd1;
        int reuse = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        ret = bind(fd, cur_ai->ai_addr, cur_ai->ai_addrlen);

        if (ret) {
            ret = ff_neterrno();
            goto fail1;
        }

        listen(fd, 1);
        
            if (is_file_seq_exit()) {
                  ret = AVERROR(EIO);
                goto fail1;
            }

        fd1 = accept(fd, NULL, NULL);

        if (fd1 < 0) {
            ret = ff_neterrno();
            goto fail1;
        }

        ff_closesocket(fd);
        fd = fd1;
#ifndef __LINUX__
        ff_socket_nonblock(fd, 0);
#else
        ff_socket_nonblock(fd, 1);
#endif
    } else {
    redo:
        //   printf("%s    %d  \n",__func__,__LINE__);
        //mtos_printk("\n[%s] --- redo connect\n", __func__);
#ifndef __LINUX__
        ff_socket_nonblock(fd, 0);
#else
        ff_socket_nonblock(fd, 1);
#endif
#ifndef __LINUX__
        {
            struct timeval to;
            FILE_SEQ_T * p_seq = file_seq_get_instance();

            if (is_file_seq_exit()) {
                OS_PRINTF("\n[%s] ---detect stop commond at line %d! \n", __func__, __LINE__);
                ret = AVERROR(EIO);
                goto fail1;
            }

            if (strncmp(p_seq->m_path[0], "rtmp://", 7) == 0) { //peacer add
                to.tv_sec = 15000;
                //mtos_printk("\n[%s] -rtmp 5000 ms\n", __func__);
            } else {
                to.tv_sec = 5000;
                //mtos_printk("\n[%s] -hls 500 ms\n", __func__);
            }

            setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to));
            setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof(to));
        }
#endif
        ret = connect(fd, cur_ai->ai_addr, cur_ai->ai_addrlen);
#ifndef __LINUX__
        {
            struct timeval to;
            FILE_SEQ_T * p_seq = file_seq_get_instance();

            if (is_file_seq_exit()) {
                OS_PRINTF("\n[%s] ---detect stop commond at line %d! \n", __func__, __LINE__);
                ret = AVERROR(EIO);
                goto fail1;
            }

            if (strncmp(p_seq->m_path[0], "rtmp://", 7) == 0) { //peacer add
                to.tv_sec = 5000;
                //mtos_printk("\n[%s] -rtmp 5000 ms\n", __func__);
            } else {
                to.tv_sec = 5000;
                //mtos_printk("\n[%s] -hls 500 ms\n", __func__);
            }

            setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to));
            setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof(to));
        }
#endif
        //mtos_printk("\n[%s] ---called connect(),ret=%d! \n", __func__,ret);
    }

    if (ret < 0) {
        struct pollfd p = {fd, POLLOUT, 0};
        ret = ff_neterrno();

        if (ret == AVERROR(EINTR)) {
            if (ff_check_interrupt(&h->interrupt_callback)) {
                ret = AVERROR_EXIT;
                goto fail1;
            }

            goto redo;
        }

        if (ret != AVERROR(EINPROGRESS) &&
            ret != AVERROR(EAGAIN)) {
            goto fail;
        }

        /* wait until we are connected or until abort */
        while (timeout--) {
            if (ff_check_interrupt(&h->interrupt_callback)) {
                ret = AVERROR_EXIT;
                goto fail1;
            }

            ret = poll(&p, 1, 100);

            if (ret > 0) {
                break;
            }
        }

        if (ret <= 0) {
            ret = AVERROR(ETIMEDOUT);
            goto fail;
        }

        /* test error */
        optlen = sizeof(ret);
        getsockopt(fd, SOL_SOCKET, SO_ERROR, &ret, &optlen);

        if (ret != 0) {
            av_log(h, AV_LOG_ERROR,
                   "TCP connection to %s:%d failed: %s\n",
                   hostname, port, strerror(ret));
            ret = AVERROR(ret);
            goto fail;
        }
    }

    h->is_streamed = 1;
    s->fd = fd;
   // mtos_printk("\n%s socket open%d\n", __func__, fd);
    #ifdef __LINUX__
       freeaddrinfo(ai);
    #else
    mt_freeaddrinfo(ai);
    #endif
 
    return 0;
fail:

    if (cur_ai && cur_ai->ai_next) {//peacer modify
        /* Retry with the next sockaddr */
        cur_ai = cur_ai->ai_next;

        if (fd >= 0) {
            ff_closesocket(fd);
            fd = -1;
            s->fd = -1;
        }

        goto restart;
    }

fail1:

    if (fd >= 0) {
        ff_closesocket(fd);
        fd = -1;
        s->fd = -1;
    }

    if (ai) { //peacer add 20131203
        freeaddrinfo(ai);
    }

    if (!is_file_seq_exit())
    	g_load_err = ERR_LOAD_CONNECT_ERR;
    	
    return ret;
}
   #ifdef WITH_OTT_DATA
extern  int encry_delay;
#endif
static int tcp_read(URLContext * h, uint8_t * buf, int size)
{
    TCPContext * s = h->priv_data;
    int ret, ret2;


#ifdef __LINUX__

    if (!(h->flags & AVIO_FLAG_NONBLOCK)) {
        ret = ff_network_wait_fd(s->fd, 0);

        if (ret < 0) {
            return ret;
        }
    }

#endif

        if (is_file_seq_exit()) {
              return AVERROR(EIO);
        }

    ret = recv(s->fd, buf, size, 0);

    //ret = ret < 0 ? ff_neterrno() : ret;
    if (ret < 0) {
        ret2 = ret;
        ret = ff_neterrno();
        {
#ifndef __LINUX__
            int err_len, err;
            err_len = sizeof(int);
            ret =  getsockopt(s->fd, SOL_SOCKET, SO_ERROR, &err, &err_len);

            if (err == 108 ||err == 104 ) {
                ret = reconnect_no;
            }

#endif
        }
        av_log(h, AV_LOG_DEBUG, "[%s]--recv error!  ret2=%d, ret=%d\n", __func__, ret2, ret);
    }

    /*if(ret != size)
        av_log(h, AV_LOG_DEBUG, "\n[%s]-- recv   ret = %d, size = %d\n",__func__,ret,size);*/
        //mtos_printk("\n[%s]-- recv   ret = %d, size = %d\n",__func__,ret,size);//for test
    return ret;
}

static int tcp_write(URLContext * h, const uint8_t * buf, int size)
{
    TCPContext * s = h->priv_data;
    int ret;

    if (!(h->flags & AVIO_FLAG_NONBLOCK)) {
        ret = ff_network_wait_fd(s->fd, 1);

        if (ret < 0) {
            return ret;
        }
    }
     
        if (is_file_seq_exit()) {
             return AVERROR(EIO);
        }

    ret = send(s->fd, buf, size, 0);
    return ret < 0 ? AVERROR(errno) : ret;
}

static int tcp_shutdown(URLContext * h, int flags)
{
    TCPContext * s = h->priv_data;
    int how;

    if (flags & AVIO_FLAG_WRITE && flags & AVIO_FLAG_READ) {
        how = SHUT_RDWR;
    } else if (flags & AVIO_FLAG_WRITE) {
        how = SHUT_WR;
    } else {
        how = SHUT_RD;
    }

    return shutdown(s->fd, how);
}

static int tcp_close(URLContext * h)
{
    TCPContext * s = h->priv_data;

    if (s->fd >= 0) {
        ff_closesocket(s->fd);
        s->fd = -1;
    }

    return 0;
}

static int tcp_get_file_handle(URLContext * h)
{
    TCPContext * s = h->priv_data;
    return s->fd;
}

URLProtocol ff_tcp_protocol = {
    .name                = "tcp",
    .url_open            = tcp_open,
    .url_read            = tcp_read,
    .url_write           = tcp_write,
    .url_close           = tcp_close,
    .url_get_file_handle = tcp_get_file_handle,
    .url_shutdown        = tcp_shutdown,
    .priv_data_size      = sizeof(TCPContext),
    .flags               = URL_PROTOCOL_FLAG_NETWORK,
};
