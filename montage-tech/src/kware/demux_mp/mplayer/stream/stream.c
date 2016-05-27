/*
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

//#include <stdio.h>
//#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#ifndef __MINGW32__
#include <sys/ioctl.h>
#include <sys/wait.h>
#endif
#include <fcntl.h>
#ifdef __LINUX__
#include <strings.h>
#endif
#include <assert.h>

#include "config.h"

#if HAVE_WINSOCK2_H
#include <winsock2.h>
#endif

#include "mp_msg.h"
#include "help_mp.h"
#include "osdep/shmem.h"
#include "osdep/timer.h"
#ifdef __LINUX__
#include "network.h"
#else
#include "sys_types.h"
#include "sys_define.h"
#include "ethernet.h"

#ifdef  WITH_TCPIP_PROTOCOL
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#endif

#endif
#include "stream.h"
#include "libmpdemux/demuxer.h"
#include "libavutil/common.h"
#include "libavutil/intreadwrite.h"

#include "m_option.h"
#include "m_struct.h"

#include "cache2.h"

#include "sys_define.h"
#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#else
#include "mp_func_trans.h"
#endif
static int (*stream_check_interrupt_cb)(int time) = NULL;
inline int stream_read(stream_t *s, char *mem, int total)
	
{
	int len=total;
	
	while( len > 0)
	{
		
		int x;
		x = s->buf_len - s->buf_pos;
      
	 if(is_file_seq_exit())
	 {
       
	     return -1;
	 }

		if(x == 0)
		{
			
			if(!cache_stream_fill_buffer(s)) 
			{
				return total-len; // EOF
			}
				
			x = s->buf_len - s->buf_pos;
		}
		
		if(s->buf_pos > s->buf_len) 
		{
		//	OS_PRINTF("stream_read: WARNING! s->buf_pos>s->buf_len!!!!\n");
			mp_msg(MSGT_DEMUX, MSGL_WARN, "stream_read: WARNING! s->buf_pos>s->buf_len\n");
		}
		
		if(x > len) 
		{
			x=len;
		}
		
		memcpy(mem,&s->buffer[s->buf_pos],x);
		
		s->buf_pos += x;
		mem += x; 
		len -= x;
		
	}
	
	return total;
	
}
extern const stream_info_t stream_info_file;
extern const stream_info_t stream_info_udp;
//extern const stream_info_t stream_info_ffmpeg;
//extern const stream_info_t stream_info_http1;

//#if  defined(ENABLE_DEMUX_RTSP)
//extern const stream_info_t stream_info_rtsp_sip;
//#endif





/*
 *   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 *   NOTICE: please don't hard code special type of networkstream
 *                      to auto_open_streams array struct
 *               and you can register your expected stream as following function
 *
 *                          such as:
 *                                 register_http_stream()
 *                                 register_rtsp_stream()
 *
 *                    Registering some stream, the relative code to stream only will be linked to
 *                                 excutable file.
 *                                                               20130828/peacer
 *
 *
 */
#define  MAX_STREAM_NUM  (10)

static  stream_info_t  * auto_open_streams[MAX_STREAM_NUM] = {
    &stream_info_file, 
    #ifdef  WITH_TCPIP_PROTOCOL
    &stream_info_udp, 
    #endif
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};



stream_info_t   **  get_stream_by_index(int index)
{
    if (index < MAX_STREAM_NUM) {
        return    &(auto_open_streams[index]);
    } else {
        return NULL;
    }
}

#if 0
void register_http_stream()
{
    int i = 0;
    OS_PRINTF("[%s] -----start!\n", __func__);

    for (i = 0; i < MAX_STREAM_NUM; i++) {
        if (auto_open_streams[i] == NULL) {
            auto_open_streams[i] = &stream_info_http1;
            OS_PRINTF("[%s] ----[%d]-register ok!\n", __func__, i);
            break;
        }
    }

    OS_PRINTF("[%s] -----end !\n", __func__);
}
int register_http_stream_is()
{
    int i = 0;
    OS_PRINTF("[%s] -----start!\n", __func__);

    for (i = 0; i < MAX_STREAM_NUM; i++) {
        if (auto_open_streams[i] == &stream_info_http1) {
            return 1;
        }
    }

    OS_PRINTF("[%s] -----end !\n", __func__);
    return 0;
}
void register_rtsp_stream()
{
    int i = 0;
    OS_PRINTF("[%s] -----start!\n", __func__);

    for (i = 0; i < MAX_STREAM_NUM; i++) {
        if (auto_open_streams[i] == NULL) {
            auto_open_streams[i] = &stream_info_rtsp_sip;
            OS_PRINTF("[%s] ---[%d]--register ok!\n", __func__, i);
            break;
        }
    }

    OS_PRINTF("[%s] -----end !\n", __func__);
}

void register_rtmp_stream()
{
    int i = 0;
    OS_PRINTF("[%s] -----start!\n", __func__);

    for (i = 0; i < MAX_STREAM_NUM; i++) {
        if (auto_open_streams[i] == NULL) {
            auto_open_streams[i] = &stream_info_ffmpeg;
            OS_PRINTF("[%s] ---[%d]--register ok!\n", __func__, i);
            break;
        }
    }

    OS_PRINTF("[%s] -----end !\n", __func__);
}

void register_asf_stream()
{
    int i = 0;
    OS_PRINTF("[%s] -----start!\n", __func__);

    for (i = 0; i < MAX_STREAM_NUM; i++) {
        if (auto_open_streams[i] == NULL) {
            auto_open_streams[i] = &stream_info_asf;
            OS_PRINTF("[%s] ----[%d]-register ok!\n", __func__, i);
            break;
        }
    }

    OS_PRINTF("[%s] -----end !\n", __func__);
}

#endif
/*
 *
 *
 *
 *
 */
#define MAX_URL_SIZE 4096
static stream_t * open_stream_plugin(const stream_info_t * sinfo, const char * filename,
                                     int mode, char ** options, int * file_format,
                                     int * ret, char ** redirected_url)
{
    void * arg = NULL;
    stream_t * s;
    m_struct_t * desc = (m_struct_t *)sinfo->opts;
    //yliu add: for ganlulu_x264_aac_720x528.mkv,
    //no sleep can't load success when open second time
    //just workaround ,not root cause
    //OS_PRINTF("[%s] start start ...\n",__func__);
#ifndef __LINUX__
    mtos_task_sleep(1);
#endif

    // Parse options
    if (desc) {
        OS_PRINTF("[%s] ----1111!\n", __func__);
        arg = m_struct_alloc(desc);

        if (sinfo->opts_url) {
            m_option_t url_opt = { "stream url", arg , CONF_TYPE_CUSTOM_URL, 0, 0 , 0, sinfo->opts };

            if (m_option_parse(&url_opt, "stream url", filename, arg, M_CONFIG_FILE) < 0) {
                mp_msg(MSGT_OPEN, MSGL_ERR, MSGTR_URLParsingFailed, filename);
                m_struct_free(desc, arg);
                return NULL;
            }
        }

        if (options) {
            int i;

            for (i = 0 ; options[i] != NULL ; i += 2) {
                mp_msg(MSGT_OPEN, MSGL_DBG2, "Set stream arg %s=%s\n",
                       options[i], options[i + 1]);

                if (!m_struct_set(desc, arg, options[i], options[i + 1])) {
                    mp_msg(MSGT_OPEN, MSGL_WARN, MSGTR_FailedSetStreamOption, options[i], options[i + 1]);
                }
            }
        }
    }

    //OS_PRINTF("[%s] ----2222!\n",__func__);
    s = new_stream(-2, -2);
    s->capture_file = NULL;
    s->url = malloc(MAX_URL_SIZE);
    memset(s->url, 0, MAX_URL_SIZE);
    memcpy(s->url, filename, strlen(filename));
    //s->url=strdup(filename);
    s->flags |= mode;
    *ret = sinfo->open(s, mode, arg, file_format);

    if ((*ret) != STREAM_OK) {
        OS_PRINTF("[%s][ERROR] ----fail to open!\n", __func__);
        OS_PRINTF("[%s][ERROR] ----Maybe Our url should be redirected !!!!!!\n", __func__);
#ifdef CONFIG_NETWORKING
#ifdef  WITH_TCPIP_PROTOCOL

        if (*ret == STREAM_REDIRECTED && redirected_url) {
            if (s->streaming_ctrl && s->streaming_ctrl->url && s->streaming_ctrl->url->url) {
                OS_PRINTF("[%s] OK we have got the valid redirected url !!!!!!\n", __func__);
                *redirected_url = strdup(s->streaming_ctrl->url->url);
            } else {
                *redirected_url = NULL;
            }
        }

        streaming_ctrl_free(s->streaming_ctrl);
        s->streaming_ctrl = NULL;
        OS_PRINTF("\n%s %d\n", __func__, __LINE__);

        //yliu add:
        if (s->fd >= 0) {
            OS_PRINTF("\n%s %d\n", __func__, __LINE__);
            closesocket(s->fd);
            s->fd = -1;
        }

#endif
#endif

        if (s->buffer) {
            mtos_align_free_alias(s->buffer);
        }

        if (s->url) {
            free(s->url);
        }

        free(s);
        OS_PRINTF("[%s] end end 1111...\n", __func__);
        return NULL;
    }

    //OS_PRINTF("[%s] ----4444!\n",__func__);

    if (s->type <= -2) {
        mp_msg(MSGT_OPEN, MSGL_WARN, MSGTR_StreamNeedType);
    }

    if (s->flags & MP_STREAM_SEEK && !s->seek) {
        s->flags &= ~MP_STREAM_SEEK;
    }

    if (s->seek && !(s->flags & MP_STREAM_SEEK)) {
        s->flags |= MP_STREAM_SEEK;
    }

    s->mode = mode;
    //OS_PRINTF("[%s]  STREAM: [%s] %s\n",__func__,sinfo->name,filename);
    //OS_PRINTF("[%s]  STREAM: Description: %s\n",__func__,sinfo->info);
    //OS_PRINTF("[%s]  STREAM: Author: %s\n", __func__,sinfo->author);
    //OS_PRINTF("[%s]  STREAM: Comment: %s\n", __func__,sinfo->comment);
    OS_PRINTF("[%s] end end ...\n", __func__);
    return s;
}
/*
 *
 *
 *
 *
 */
stream_t * open_stream_full(const char * filename, int mode, char ** options, int * file_format)
{
    int i, j, l, r;
    stream_info_t * sinfo;
    stream_t * s;
    char * redirected_url = NULL;
    OS_PRINTF("[%s] start start ...\n", __func__);
    OS_PRINTF("[%s] filename: %s\n", __func__, filename);

    if (filename == NULL) {
        return NULL;
    }

    for (i = 0 ; auto_open_streams[i] ; i++) {
        //yliu add for load exit

        if (is_file_seq_exit()) {
            OS_PRINTF("\n%s %d stream check :%d\n", __func__, __LINE__, i);
            return NULL;
        }

        //
        sinfo = auto_open_streams[i];

        if (!sinfo->protocols) {
            mp_msg(MSGT_OPEN, MSGL_WARN, MSGTR_StreamProtocolNULL, sinfo->name);
            continue;
        }

        for (j = 0 ; sinfo->protocols[j] ; j++) {
            l = strlen(sinfo->protocols[j]);

            OS_PRINTF("[%s] i[%d] j[%d]  %s, l=%d\n",__func__,i,j, sinfo->protocols[j],l);

            // l == 0 => Don't do protocol matching (ie network and filenames)
            if ((l == 0 && !strstr(filename, "://")) || ((strncasecmp(sinfo->protocols[j], filename, l) == 0) &&
                    (strncmp("://", filename + l, 3) == 0))) {
                *file_format = DEMUXER_TYPE_UNKNOWN;
                s = open_stream_plugin(sinfo, filename, mode, options, file_format, &r, &redirected_url);

                if (s) {
                    OS_PRINTF("[%s] end end 1111111...\n", __func__);
                    return s;
                }

                if (r == STREAM_REDIRECTED && redirected_url) {
                    OS_PRINTF("[%s] haha redirected lalalal !!!!!!!!!\n", __func__);
                    OS_PRINTF("[%s] %s open %s redirected to %s !!\n", __func__, sinfo->info, filename, redirected_url);
                    s = open_stream_full(redirected_url, mode, options, file_format);
                    free(redirected_url);
                    return s;
                } else if (r != STREAM_UNSUPPORTED) {
                    //mp_msg(MSGT_OPEN,MSGL_ERR, MSGTR_FailedToOpen,filename);
                    OS_PRINTF("[%s][ERROR] fail to open_stream_plugin!!\n", __func__);
                    OS_PRINTF("[%s][ERROR] STREAM_UNSUPPORTED!!\n", __func__);
                    OS_PRINTF(MSGTR_FailedToOpen, filename);
                    return NULL;
                }

                break;
            }
        }
    }

    //mp_msg(MSGT_OPEN,MSGL_ERR, MSGTR_StreamCantHandleURL,filename);
    OS_PRINTF(MSGTR_StreamCantHandleURL, filename);
    OS_PRINTF("[%s] end end ...\n", __func__);
    return NULL;
}
/*
 *
 *
 *
 *
 */
stream_t * open_output_stream(const char * filename, char ** options)
{
    int file_format; //unused

    if (!filename) {
        mp_msg(MSGT_OPEN, MSGL_ERR, MSGTR_StreamNULLFilename);
        return NULL;
    }

    return open_stream_full(filename, STREAM_WRITE, options, &file_format);
}

//=================== STREAMER =========================
/*
 *
 *
 *
 *
 */
void stream_capture_do(stream_t * s)
{
#ifdef __LINUX__

    if (fwrite(s->buffer, s->buf_len, 1, s->capture_file) < 1) {
        // mp_msg(MSGT_GLOBAL, MSGL_ERR, MSGTR_StreamErrorWritingCapture,
        //        strerror(errno));
        fclose(s->capture_file);
        s->capture_file = NULL;
    }

#else
    int r;

    if (ufs_write(s->capture_file, s->buffer, s->buf_len, &r) != FR_OK) {
        // mp_msg(MSGT_GLOBAL, MSGL_ERR, MSGTR_StreamErrorWritingCapture,
        //        strerror(errno));
        ufs_close(s->capture_file);
        //mtos_free(s->capture_file);
        s->capture_file = NULL;
    }

#endif
}
/*
 *
 *
 *
 *
 */
static int stream_reconnect(stream_t * s)
{
#define MAX_RECONNECT_RETRIES 3
#define RECONNECT_SLEEP_MS 1000
    int retry = 0;
    off_t pos = s->pos;

    // Seeking is used as a hack to make network streams
    // reopen the connection, ideally they would implement
    // e.g. a STREAM_CTRL_RECONNECT to do this
    do {

        if (is_file_seq_exit()) {
            return 0;
        }

        if (retry >= MAX_RECONNECT_RETRIES) {
            return 0;
        }

        if (retry) {
            usec_sleep(RECONNECT_SLEEP_MS * 1000);
        }

        retry++;
        s->eof = 1;
        stream_reset(s);
    } while (stream_seek_internal(s, pos) >= 0 || s->pos != pos); // seek failed

    return 1;
}

/*
 *
 *
 *
 *
 */
int stream_read_internal(stream_t * s, void * buf, int len)
{
    int orig_len = len;

    if (is_file_seq_exit()) {
        s->eof = 1;
        return 0;
    }

    // we will retry even if we already reached EOF previously.
    switch (s->type) {
        case STREAMTYPE_STREAM:
#ifdef CONFIG_NETWORKING
#ifdef  WITH_TCPIP_PROTOCOL

            if (s->streaming_ctrl != NULL && s->streaming_ctrl->streaming_read) {
                if (s->end_pos && ((s->pos - (s->end_pos)) >= 0))
                    //yliu revert
                    //if(0)
                {
                    len = 0;
                    s->eof = 1;
                } else {
                    //OS_PRINTF("\n%s %d %lld %lld\n",__func__,__LINE__,s->pos,s->end_pos);
                    len = s->streaming_ctrl->streaming_read(s->fd, buf, len, s->streaming_ctrl);

                    //OS_PRINTF("\n%s %d %lld %lld\n",__func__,__LINE__,s->pos,s->end_pos);
                    if (s->streaming_ctrl->status == streaming_stopped_e &&
                        (!s->end_pos || s->pos == s->end_pos)) {
                        s->eof = 1;
                    }
                }
            } else
#endif
#endif
                if (s->fill_buffer) {
                    len = s->fill_buffer(s, buf, len);
                } else {
#ifdef  WITH_TCPIP_PROTOCOL
                    len = read(s->fd, buf, len);
#endif
                }

            break;
        case STREAMTYPE_DS:
            len = demux_read_data((demux_stream_t *)s->priv, buf, len);
            break;
        default:
            len = s->fill_buffer ? s->fill_buffer(s, buf, len) : 0;
    }

    if (len <= 0) {
        // do not retry if this looks like proper eof
        //xxia modify for some stream goto reconnect will error
        //some stream end_pos is 0
        if (s->eof || (s->pos >= s->end_pos)) {
            goto eof_out;
        }

        // dvdnav has some horrible hacks to "suspend" reads,
        // we need to skip this code or seeks will hang.
        if (s->type == STREAMTYPE_DVDNAV) {
            goto eof_out;
        }

        // just in case this is an error e.g. due to network
        // timeout reset and retry
        if (!stream_reconnect(s)) {
            goto eof_out;
        }

        // make sure EOF is set to ensure no endless loops
        s->eof = 1;
        return stream_read_internal(s, buf, orig_len);
    eof_out:
        s->eof = 1;
        return 0;
    }

    // When reading succeeded we are obviously not at eof.
    // This e.g. avoids issues with eof getting stuck when lavf seeks in MPEG-TS
    s->eof = 0;
    s->pos += len;
    return len;
}
/*
 *
 *
 *
 *
 */
int stream_fill_buffer(stream_t * s)
{
    int len = stream_read_internal(s, s->buffer, io_stream_buffer_size);

    if (len <= 0) {
        return 0;
    }

    s->buf_pos = 0;
    s->buf_len = len;
    //  printf("[%d]",len);fflush(stdout);

    if (s->capture_file) {
        stream_capture_do(s);
    }

    return len;
}
/*
 *
 *
 *
 *
 */
int stream_write_buffer(stream_t * s, unsigned char * buf, int len)
{
    int rd;

    if (!s->write_buffer) {
        return -1;
    }

    rd = s->write_buffer(s, buf, len);

    if (rd < 0) {
        return -1;
    }

    s->pos += rd;
    assert(rd == len && "stream_write_buffer(): unexpected short write");
    return rd;
}
/*
 *
 *
 *
 *
 */
int stream_seek_internal(stream_t * s, off_t newpos)
{
    if (newpos == 0 || newpos != s->pos) {
        switch (s->type) {
            case STREAMTYPE_STREAM:
                //s->pos=newpos; // real seek
                // Some streaming protocol allow to seek backward and forward
                // A function call that return -1 can tell that the protocol
                // doesn't support seeking.
#ifdef CONFIG_NETWORKING
#ifdef  WITH_TCPIP_PROTOCOL

                if (s->seek) { // new stream seek is much cleaner than streaming_ctrl one
                    if (!s->seek(s, newpos)) {
                        mp_msg(MSGT_STREAM, MSGL_ERR, MSGTR_StreamSeekFailed);
                        return 0;
                    }

                    break;
                }

                if (s->streaming_ctrl != NULL && s->streaming_ctrl->streaming_seek) {
                    if (s->streaming_ctrl->streaming_seek(s->fd, newpos, s->streaming_ctrl) < 0) {
                        mp_msg(MSGT_STREAM, MSGL_INFO, MSGTR_StreamNotSeekable);
                        return 1;
                    }

                    break;
                }

#endif
#endif

                if (newpos < s->pos) {
                    mp_msg(MSGT_STREAM, MSGL_INFO, MSGTR_StreamCannotSeekBackward);
                    return 1;
                }

                break;
            default:

                // This should at the beginning as soon as all streams are converted
                if (!s->seek) {
                    return 0;
                }

                // Now seek
                if (!s->seek(s, newpos)) {
                    mp_msg(MSGT_STREAM, MSGL_ERR, MSGTR_StreamSeekFailed);
                    return 0;
                }
        }

        //   putchar('.');fflush(stdout);
        //} else {
        //   putchar('%');fflush(stdout);
    }

    return -1;
}
/*
 *
 *
 *
 *
 */
int stream_seek_long(stream_t * s, off_t pos)
{
    int res;
    off_t newpos = 0;
    //  if( mp_msg_test(MSGT_STREAM,MSGL_DBG3) ) printf("seek_long to 0x%X\n",(unsigned int)pos);
    s->buf_pos = s->buf_len = 0;

    if (s->mode == STREAM_WRITE) {
        if (!s->seek || !s->seek(s, pos)) {
            return 0;
        }

        return 1;
    }

    if (s->sector_size) {
        newpos = (pos / s->sector_size) * s->sector_size;
    } else {
        newpos = pos & (~((off_t)io_stream_buffer_size - 1));
    }

    if (mp_msg_test(MSGT_STREAM, MSGL_DBG3)) {
        mp_msg(MSGT_STREAM, MSGL_DBG3, "s->pos=%"PRIX64"  newpos=%"PRIX64"  new_bufpos=%"PRIX64"  buflen=%X  \n",
               (int64_t)s->pos, (int64_t)newpos, (int64_t)pos, s->buf_len);
    }

    pos -= newpos;
    res = stream_seek_internal(s, newpos);

    if (res >= 0) {
        return res;
    }

    while (s->pos < newpos) {
        if (stream_fill_buffer(s) <= 0) {
            break;    // EOF
        }
    }

    while (stream_fill_buffer(s) > 0 && pos >= 0) {
        if (pos <= s->buf_len) {
            s->buf_pos = pos; // byte position in sector
            return 1;
        }

        pos -= s->buf_len;
    }

    //  if(pos==s->buf_len) printf("XXX Seek to last byte of file -> EOF\n");
    mp_msg(MSGT_STREAM, MSGL_V, "stream_seek: WARNING! Can't seek to 0x%"PRIX64" !\n", (int64_t)(pos + newpos));
    return 0;
}

/*
 *
 *
 *
 *
 */
void stream_reset(stream_t * s)
{
    if (s->eof) {
        s->pos = 0;
        s->buf_pos = s->buf_len = 0;
        s->eof = 0;
    }

    if (s->control) {
        s->control(s, STREAM_CTRL_RESET, NULL);
    }

    //stream_seek(s,0);
}
/*
 *
 *
 *
 *
 */
int stream_control(stream_t * s, int cmd, void * arg)
{
    if (!s->control) {
        return STREAM_UNSUPPORTED;
    }

#ifdef CONFIG_STREAM_CACHE

    if (s->cache_pid) {
        return cache_do_control(s, cmd, arg);
    }

#endif
    return s->control(s, cmd, arg);
}
/*
 *
 *
 *
 *
 */
stream_t * new_memory_stream(unsigned char * data, int len)
{
    stream_t * s;

    if (len < 0) {
        return NULL;
    }

    s = calloc(1, sizeof(stream_t) + len);
    s->fd = -1;
    s->type = STREAMTYPE_MEMORY;
    s->buf_pos = 0;
    s->buf_len = len;
    s->start_pos = 0;
    s->end_pos = len;
    stream_reset(s);
    s->pos = len;
    memcpy(s->buffer, data, len);
    return s;
}
/*
 *
 *
 *
 *
 */
stream_t * new_stream(int fd, int type)
{
    stream_t * s = calloc(1, sizeof(stream_t));

    if (s == NULL) {
        return NULL;
    }

#if 0//HAVE_WINSOCK2_H  peacer del
    {
        WSADATA wsdata;
        int temp = WSAStartup(0x0202, &wsdata); // there might be a better place for this (-> later)
        mp_msg(MSGT_STREAM, MSGL_V, "WINSOCK2 init: %i\n", temp);
    }
#endif
    //s->buffer = calloc(1, io_stream_buffer_size);
    s->buffer = mtos_align_malloc_alias(io_stream_buffer_size, 32); //fix by ybc,for match usb fix

    if (s->buffer == NULL) {
        free(s);
        s = NULL;
        return NULL;
    }

    s->fd = fd;
    s->type = type;
    s->buf_pos = s->buf_len = 0;
    s->start_pos = s->end_pos = 0;
    s->priv = NULL;
    s->url = NULL;
    s->cache_pid = 0;
    stream_reset(s);
    return s;
}
/*
 *
 *
 *
 *
 */
void free_stream(stream_t * s)
{
    //  printf("\n*** free_stream() called ***\n");
#ifdef CONFIG_STREAM_CACHE
    cache_uninit(s);
#endif

    if (s->capture_file) {
#ifdef __LINUX__
        fclose(s->capture_file);
#else
        ufs_close(s->capture_file);
#endif
        s->capture_file = NULL;
    }

    if (s->streaming_ctrl) {
        streaming_ctrl_free(s->streaming_ctrl);
        s->streaming_ctrl = NULL;
    }

#ifdef __LINUX__

    if (s->close) {
        s->close(s);
    }

    if (s->fd > 0) {
        /* on unix we define closesocket to close
           on windows however we have to distinguish between
           network socket and file */
        if (s->url && strstr(s->url, "://")) {
            closesocket(s->fd);
        } else {
            close(s->fd);
        }
    }

#else
#ifdef WITH_TCPIP_PROTOCOL

    if (s->close) {
        (s->close)(s);
    }

#endif
    //if(s->fd  >=  0)//peacer modify
    {
        if (s->url && strstr(s->url, "://")) {
            if (s->fd >= 0) {
#ifdef WITH_TCPIP_PROTOCOL
                closesocket(s->fd);
#endif
                s->fd = -1;
            }
        } 

#ifdef USE_PB_FIFO  

        else{
        	FILE_SEQ_T * p_file_seq = x_get_cur_instance();
	        if(p_file_seq->is_fifo_playback){
		        if (( int)s->fd > 0){
		            //yliu:stream_file
		            ufs_close((unsigned int)(s->fd));
		            free((unsigned int)s->fd);
		            s->fd = 0;
		        }	        
		 }
	        else{
			if ((unsigned int)s->fd > 0){
		            //yliu:stream_file
		            ufs_close((unsigned int)(s->fd));
		            free((unsigned int)s->fd);
		            s->fd = 0;
		        }
	        }
        }

#else
        else if ((unsigned int)s->fd > 0) 
        {
            //yliu:stream_file
            ufs_close((unsigned int)(s->fd));
            //if((unsigned int)s->fd>0)
            {
                free((unsigned int)s->fd);
            }
            s->fd = 0;
        }
#endif    
    }
#endif
#if HAVE_WINSOCK2_H
    mp_msg(MSGT_STREAM, MSGL_V, "WINSOCK2 uninit\n");
    WSACleanup(); // there might be a better place for this (-> later)
#endif
    // Disabled atm, i don't like that. s->priv can be anything after all
    // streams should destroy their priv on close
    //free(s->priv);
    //s->buffer = calloc(1, io_stream_buffer_size);
    mtos_align_free_alias(s->buffer);
    //free(s->buffer);
    free(s->url);
    free(s);
}
/*
 *
 *
 *
 *
 */
stream_t * new_ds_stream(demux_stream_t * ds)
{
    stream_t * s = new_stream(-1, STREAMTYPE_DS);
    s->priv = ds;
    return s;
}
/*
 *
 *
 *
 *
 */
void stream_set_interrupt_callback(int (*cb)(int))
{
    stream_check_interrupt_cb = cb;
}
/*
 *
 *
 *
 *
 */
int stream_check_interrupt(int time)
{
    if (!stream_check_interrupt_cb) {
        usec_sleep(time * 1000);
        return 0;
    }

    return stream_check_interrupt_cb(time);
}

/**
 * Helper function to read 16 bits little-endian and advance pointer
 */
static uint16_t get_le16_inc(const uint8_t ** buf)
{
    uint16_t v = AV_RL16(*buf);
    *buf += 2;
    return v;
}

/**
 * Helper function to read 16 bits big-endian and advance pointer
 */
static uint16_t get_be16_inc(const uint8_t ** buf)
{
    uint16_t v = AV_RB16(*buf);
    *buf += 2;
    return v;
}

/**
 * Find a termination character in buffer
 * \param buf buffer to search
 * \param len amount of bytes to search in buffer, may not overread
 * \param utf16 chose between UTF-8/ASCII/other and LE and BE UTF-16
 *              0 = UTF-8/ASCII/other, 1 = UTF-16-LE, 2 = UTF-16-BE
 */
static const uint8_t * find_term_char(const uint8_t * buf, int len, uint8_t term, int utf16)
{
    uint32_t c;
    const uint8_t * end = buf + len;

    switch (utf16) {
        case 0:
            return (uint8_t *)memchr(buf, term, len);
        case 1:

            while (buf < end - 1) {
                GET_UTF16(c, buf < end - 1 ? get_le16_inc(&buf) : 0, return NULL;)

                if (buf <= end && c == term) {
                    return buf - 1;
                }
            }

            break;
        case 2:

            while (buf < end - 1) {
                GET_UTF16(c, buf < end - 1 ? get_be16_inc(&buf) : 0, return NULL;)

                if (buf <= end && c == term) {
                    return buf - 1;
                }
            }

            break;
    }

    return NULL;
}

/**
 * Copy a number of bytes, converting to UTF-8 if input is UTF-16
 * \param dst buffer to copy to
 * \param dstsize size of dst buffer
 * \param src buffer to copy from
 * \param len amount of bytes to copy from src
 * \param utf16 chose between UTF-8/ASCII/other and LE and BE UTF-16
 *              0 = UTF-8/ASCII/other, 1 = UTF-16-LE, 2 = UTF-16-BE
 */
static int copy_characters(uint8_t * dst, int dstsize,
                           const uint8_t * src, int * len, int utf16)
{
    uint32_t c;
    uint8_t * dst_end = dst + dstsize;
    const uint8_t * end = src + *len;

    switch (utf16) {
        case 0:

            if (*len > dstsize) {
                *len = dstsize;
            }

            memcpy(dst, src, *len);
            return *len;
        case 1:

            while (src < end - 1 && dst_end - dst > 8) {
                uint8_t tmp;
                GET_UTF16(c, src < end - 1 ? get_le16_inc(&src) : 0, ;)
                PUT_UTF8(c, tmp, *dst++ = tmp;)
            }

            *len -= end - src;
            return dstsize - (dst_end - dst);
        case 2:

            while (src < end - 1 && dst_end - dst > 8) {
                uint8_t tmp;
                GET_UTF16(c, src < end - 1 ? get_be16_inc(&src) : 0, ;)
                PUT_UTF8(c, tmp, *dst++ = tmp;)
            }

            *len -= end - src;
            return dstsize - (dst_end - dst);
    }

    return 0;
}
/*
 *
 *
 *
 *
 */
uint8_t * stream_read_until(stream_t * s, uint8_t * mem, int max,
                            uint8_t term, int utf16)
{
    int len;
    const unsigned char * end;
    unsigned char * ptr = mem;

    if (max < 1) {
        return NULL;
    }

    max--; // reserve one for 0-termination

    do {
        len = s->buf_len - s->buf_pos;

        // try to fill the buffer
        if (len <= 0 &&
            (!cache_stream_fill_buffer(s) ||
             (len = s->buf_len - s->buf_pos) <= 0)) {
            break;
        }

        end = find_term_char(s->buffer + s->buf_pos, len, term, utf16);

        if (end) {
            len = end - (s->buffer + s->buf_pos) + 1;
        }

        if (len > 0 && max > 0) {
            int l = copy_characters(ptr, max, s->buffer + s->buf_pos, &len, utf16);
            max -= l;
            ptr += l;

            if (!len) {
                break;
            }
        }

        s->buf_pos += len;
    } while (!end);

    ptr[0] = 0;

    if (s->eof && ptr == mem) {
        return NULL;
    }

    return mem;
}
