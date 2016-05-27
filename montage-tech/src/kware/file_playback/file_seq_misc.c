/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "string.h"
#include "stdio.h"
// system
#include "sys_types.h"
#include "sys_define.h"


#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
// driver
#include "driver.h"
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_misc.h"
#include "hal_uart.h"

#include "drv_dev.h"
#include "drv_misc.h"
#include "uio.h"
#include "nim.h"
#include "dmx.h"
#include "lib_rect.h"
//#include "logo.h"
//#include "pti.h"

#include "common.h"
#include "region.h"
#include "display.h"
#include "dmx.h"
#include "file_playback_sequence.h"
#include "file_seq_internal.h"

#include "download_api.h"

#ifndef __LINUX__

#ifdef  WITH_TCPIP_PROTOCOL
#include "lwip/sockets.h"
#endif

#else
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include "list.h"

#include "aud_vsb.h"
//#include "audio.h"        //not used, please aud_vsb.h instead
#include "vdec.h"
#include "drv_dev.h"
#include "block.h"
#include "ff.h"
#include "fsioctl.h"
#include "file_playback_sequence.h"

#include "ufs.h"

#ifndef __LINUX__
#include  "record_seq.h"
#include "ethernet.h"
#else
#define MT_ASSERT

#endif

#define   VIDEO_TMP_BUF_LEN   (128*1024)
#define   VIDEO_ES_BUF_OVERFLOW_THRESHOLD  (100) // k bytes
extern void x_unmap_es_buffer(void * pHandle);


#define RECORD_FILE_TAIL    ".musbc"


#define   DBG_FILE_PLAYBACK_SEQ

#ifdef     DBG_FILE_PLAYBACK_SEQ
#define FILE_SEQ_LOG(format, args...)              OS_PRINTF(format, ##args)
#define FILE_SEQ_WARNING(format, args...)     OS_PRINTF(format, ##args)
#define FILE_SEQ_DEBUG(format, args...)          OS_PRINTF(format, ##args)
#define FILE_SEQ_ERROR(format, args...)          OS_PRINTF(format, ##args)
#else
#define FILE_SEQ_LOG(format, args...)               OS_PRINTF(format, ##args)
#define FILE_SEQ_WARNING(format, args...)
#define FILE_SEQ_DEBUG(format, args...)
#define FILE_SEQ_ERROR(format, args...)           OS_PRINTF(format, ##args)
#endif

/*
* is_loop_ok: check test speed task nomal or exception
* check_interval: check test speed task timer interval
*/
typedef struct {
	BOOL is_loop_ok;
	int check_interval;
}loop_check_t;


#ifdef __LINUX__
/* TPV:OOM check for Progressive jpeg+ */
static char line[255];
static char * xReadline(FILE * f)
{
    memset(line, 0, sizeof(line));
    char c;
    int len = 0;

    while ((c = fgetc(f)) != EOF && c != '\n') {
        line[len++] = c;
        line[len] = '\0';
    }

    return line;
}
static  void  clearAllCache()
{
    FILE * fd;
    // FILE_SEQ_DEBUG("[%s] ==start start\n",__func__);
    fd = fopen("/proc/sys/vm/drop_caches", "a");

    if (fd) {
        fwrite("1", 1, 1, fd);
        fclose(fd);
        //FILE_SEQ_DEBUG("[%s] ==OK\n",__func__);
    }

    //FILE_SEQ_DEBUG("[%s] ==end end\n",__func__);
}
int  run_memory(BOOL isflushCache)
{
    int mem_total = 0;
    int mem_free = 0;
    FILE * fp = NULL;
    char * szLine = NULL;
    size_t len = 0;
    int nTotal = 0;
    int nFree = 0;
    int nBuffers = 0;
    int nCached = 0;
    int nSwapCached = 0;
    int ret = 0;
    //FILE_SEQ_DEBUG("[%s] ==start start\n",__func__);
    fp = fopen("/proc/meminfo", "r");

    if (fp == NULL) {
        //FILE_SEQ_DEBUG("%s: open /proc/meminfo error\n", __FUNCTION__);
        return -1;
    }

    while (xReadline(fp)) {
        if (strncmp(line, "MemTotal", strlen("MemTotal")) == 0) {
            sscanf(line, "MemTotal: %d", &nTotal);
            // FILE_SEQ_DEBUG("MemTotal=%d\n",nTotal);
        } else if (strncmp(line, "MemFree", strlen("MemFree")) == 0) {
            sscanf(line, "MemFree: %d", &nFree);
            //FILE_SEQ_DEBUG("MemFree=%d\n",nFree);
        } else if (strncmp(line, "Buffers", strlen("Buffers")) == 0) {
            sscanf(line, "Buffers: %d", &nBuffers);
            //FILE_SEQ_DEBUG("Buffers=%d\n",nBuffers);
        } else if (strncmp(line, "Cached", strlen("Cached")) == 0) {
            sscanf(line, "Cached: %d", &nCached);
            //FILE_SEQ_DEBUG("Cached=%d\n",nCached);
        } else if (strncmp(line, "SwapCached", strlen("SwapCached")) == 0) {
            sscanf(line, "SwapCached: %d", &nSwapCached);
            //FILE_SEQ_DEBUG("SwapCached=%d\n",nSwapCached);
            break;
        } else {
            continue;
        }
    }

    mem_total = nTotal * 1024;
    mem_free = (nFree + nBuffers + nCached + nSwapCached) * 1024;

    //FILE_SEQ_DEBUG("[%s]  nFree=%d\n",__FUNCTION__,nFree);
    //FILE_SEQ_DEBUG("[%s]  nCached=%d\n",__FUNCTION__,nCached);
    if (fp) {
        fclose(fp);
    }

    //if(nFree < 10*1024)
    {
#if 0
        system("echo 1 > /proc/sys/vm/drop_caches");//modify by Yvonne 20110225

        if (ret != 0) {
            FILE_SEQ_DEBUG("[ERROR_SYSTEM][%s] ===fail to x echo 3 > /proc/sys/vm/drop_caches !!\n", __func__);
        }

#else
#endif
    }

    if (isflushCache && nFree < 8 * 1024) {
        clearAllCache();
    }

    //FILE_SEQ_DEBUG("[%s]  clear  cache !!\n",__func__);
    fp = fopen("/proc/meminfo", "r");

    if (fp == NULL) {
        //FILE_SEQ_DEBUG("%s: open /proc/meminfo error\n", __FUNCTION__);
        return -1;
    }

    while (xReadline(fp)) {
        if (strncmp(line, "MemTotal", strlen("MemTotal")) == 0) {
            sscanf(line, "MemTotal: %d", &nTotal);
            //FILE_SEQ_DEBUG("MemTotal=%d\n",nTotal);
        } else if (strncmp(line, "MemFree", strlen("MemFree")) == 0) {
            sscanf(line, "MemFree: %d", &nFree);
            //FILE_SEQ_DEBUG("MemFree=%d\n",nFree);
        } else if (strncmp(line, "Buffers", strlen("Buffers")) == 0) {
            sscanf(line, "Buffers: %d", &nBuffers);
            //FILE_SEQ_DEBUG("Buffers=%d\n",nBuffers);
        } else if (strncmp(line, "Cached", strlen("Cached")) == 0) {
            sscanf(line, "Cached: %d", &nCached);
            //FILE_SEQ_DEBUG("Cached=%d\n",nCached);
        } else if (strncmp(line, "SwapCached", strlen("SwapCached")) == 0) {
            sscanf(line, "SwapCached: %d", &nSwapCached);
            //FILE_SEQ_DEBUG("SwapCached=%d\n",nSwapCached);
            break;
        } else {
            continue;
        }
    }

    mem_total = nTotal * 1024;
    mem_free = (nFree + nBuffers + nCached + nSwapCached) * 1024;

    printf("[%s]  nFree=%d\n",__FUNCTION__,mem_free);
    //FILE_SEQ_DEBUG("[%s]  nCached=%d\n",__FUNCTION__,nCached);
    if (fp) {
        fclose(fp);
    }

    //FILE_SEQ_DEBUG("[%s] ==end end \n",__func__);
    return mem_free;
}

#endif



#if defined(__LINUX__)
static unsigned short   *  Convert_Utf8_To_Unicode(unsigned char * putf8, unsigned short * out)
#else
unsigned short   *  Convert_Utf8_To_Unicode(unsigned char * putf8, unsigned short * out)
#endif
{
    unsigned char *pUTF8End = putf8+strlen((char *)putf8);
    unsigned char *pTempUTF8 = putf8;
    unsigned short *pTempUTF16 = (unsigned short *)out;
    unsigned short * result = (unsigned short *)out;
    
    //mtos_printk("%s %d !!!!!!!!!\n", putf8, len);
 
    while (pTempUTF8 < pUTF8End)
    {
        if (*pTempUTF8 >= 0xE0 && *pTempUTF8 <= 0xEF)//    3 bytes
        {
            //0800 - FFFF 1110xxxx 10xxxxxx 10xxxxxx
            *pTempUTF16 |= ((*pTempUTF8++ & 0xEF) << 12);
            *pTempUTF16 |= ((*pTempUTF8++ & 0x3F) << 6);
            *pTempUTF16 |= (*pTempUTF8++ & 0x3F);
 
        }
        else if (*pTempUTF8 >= 0xC0 && *pTempUTF8 <= 0xDF)//   2 bytes
        {
            //0080 - 07FF 110xxxxx 10xxxxxx
            *pTempUTF16 |= ((*pTempUTF8++ & 0x1F) << 6);
            *pTempUTF16 |= (*pTempUTF8++ & 0x3F);
        }
        else if(*pTempUTF8 >= 0 && *pTempUTF8 <= 0x7F)//   1 byte
        {
            //0000 - 007F  0xxxxxxx
            *pTempUTF16 = *pTempUTF8++;
        }
        else
        {
            break;
        }
        pTempUTF16++;
    }
    *pTempUTF16 = 0;
    
    return result;
}






static disp_sys_t display_sys_format(disp_sys_t fmt, u8 * IS_FMT_50HZ)
{
    disp_sys_t sd_fmt = VID_SYS_PAL;

    switch (fmt) {
        case VID_SYS_NTSC_J:
        case VID_SYS_NTSC_M:
        case VID_SYS_NTSC_443:
        case VID_SYS_480P:
        case VID_SYS_720P:
        case VID_SYS_1080I:
        case VID_SYS_1080P:
            sd_fmt = VID_SYS_NTSC_J;
            *IS_FMT_50HZ = 0;
            break;

        case VID_SYS_PAL:
        case VID_SYS_PAL_N:
        case VID_SYS_PAL_NC:
        case VID_SYS_PAL_M:
        case VID_SYS_576P_50HZ:
        case VID_SYS_720P_50HZ:
        case VID_SYS_1080I_50HZ:
        case VID_SYS_1080P_50HZ:
            sd_fmt = VID_SYS_PAL;
            *IS_FMT_50HZ = 1;
            break;

        default :
            FILE_SEQ_DEBUG("\n  %s %d fmt:%d\n", __FUNCTION__, __LINE__, fmt);
            break;
    }

    return sd_fmt;
}

static disp_sys_t display_sys_format_hd(disp_sys_t fmt_sd, disp_sys_t fmt_hd
                                        , u8 * IS_FMT_50HZ)
{
    disp_sys_t fmt_out = VID_SYS_PAL;

    switch (fmt_hd) {
        case VID_SYS_PAL:
        case VID_SYS_PAL_N:
        case VID_SYS_PAL_NC:
        case VID_SYS_PAL_M:
            *IS_FMT_50HZ = 1;
            fmt_out = fmt_sd;
            break;

        case VID_SYS_NTSC_J:
        case VID_SYS_NTSC_M:
        case VID_SYS_NTSC_443:
            *IS_FMT_50HZ = 0;
            fmt_out = fmt_sd;
            break;

        case VID_SYS_576P_50HZ:
            *IS_FMT_50HZ = 1;

            if (fmt_sd == VID_SYS_NTSC_J) {
                fmt_out = VID_SYS_NTSC_M;
            } else {
                fmt_out = VID_SYS_576P_50HZ;
            }

            break;

        case VID_SYS_720P:
            *IS_FMT_50HZ = 0;

            if (fmt_sd == VID_SYS_NTSC_J) {
                fmt_out = VID_SYS_720P;
            } else {
                fmt_out = VID_SYS_720P_50HZ;
            }

            break;

        case VID_SYS_720P_50HZ:
            *IS_FMT_50HZ = 1;

            if (fmt_sd == VID_SYS_NTSC_J) {
                fmt_out = VID_SYS_720P;
            } else {
                fmt_out = VID_SYS_720P_50HZ;
            }

            break;

        case VID_SYS_1080I:
            *IS_FMT_50HZ = 0;

            if (fmt_sd == VID_SYS_NTSC_J) {
                fmt_out = VID_SYS_1080I;
            } else {
                fmt_out = VID_SYS_1080I_50HZ;
            }

            break;

        case VID_SYS_1080I_50HZ:
            *IS_FMT_50HZ = 1;

            if (fmt_sd == VID_SYS_NTSC_J) {
                fmt_out = VID_SYS_1080I;
            } else {
                fmt_out = VID_SYS_1080I_50HZ;
            }

            break;

        case VID_SYS_1080P:
            *IS_FMT_50HZ = 0;

            if (fmt_sd == VID_SYS_NTSC_J) {
                fmt_out = VID_SYS_1080P;
            } else {
                fmt_out = VID_SYS_1080P_50HZ;
            }

            break;

        case VID_SYS_1080P_50HZ:
            *IS_FMT_50HZ = 1;

            if (fmt_sd == VID_SYS_NTSC_J) {
                fmt_out = VID_SYS_1080P;
            } else {
                fmt_out = VID_SYS_1080P_50HZ;
            }

            break;

        default :
            FILE_SEQ_DEBUG("\n  %s %d fmt_sd:%d  fmt_hd:%d\n", __FUNCTION__, __LINE__, fmt_sd, fmt_hd);
            break;
    }

    return fmt_out;
}

void   file_seq_switch_tv_sys(FILE_SEQ_T * p_seq)
{
    RET_CODE ret = ERR_FAILURE;
    u8 new_fmt_50hz = 0;
    u8 old_fmt_50hz = 0;
    disp_sys_t sd_fmt = display_sys_format(p_seq->cur_tv_hd_fmt, &new_fmt_50hz);
    disp_sys_t hd_fmt , new_fmt;
    disp_get_tv_sys(p_seq->p_disp_dev, DISP_CHANNEL_HD, &hd_fmt);
    FILE_SEQ_DEBUG("[%s] ==start start\n", __func__);
    //if(sd_fmt != p_seq->cur_tv_sd_fmt)
    FILE_SEQ_DEBUG("[%s] new fmt[%d]\n", __func__, p_seq->cur_tv_hd_fmt);
    {
        new_fmt =  display_sys_format_hd(sd_fmt, hd_fmt, &old_fmt_50hz);

        if (new_fmt_50hz == old_fmt_50hz) { //do nothing
            return;
        }

        //disp_vid_zoom_exit(p_seq->p_disp_dev, DISP_CHANNEL_HD);
        ret = disp_set_tv_sys(p_seq->p_disp_dev, DISP_CHANNEL_SD, sd_fmt);
        ret = disp_set_tv_sys(p_seq->p_disp_dev, DISP_CHANNEL_HD, new_fmt);
        //MT_ASSERT(SUCCESS == ret);
        FILE_SEQ_DEBUG("\n  %s %d fmt_sd:%d  fmt_hd:%d  new_fmt :%d\n", __FUNCTION__, __LINE__, sd_fmt, hd_fmt, new_fmt);
        //ret = disp_set_tv_sys(p_seq->p_disp_dev, DISP_CHANNEL_HD, p_seq->cur_tv_hd_fmt);
        //MT_ASSERT(SUCCESS == ret)
        //p_seq->cur_tv_sd_fmt = sd_fmt;
    }
    FILE_SEQ_DEBUG("[%s] ==end end\n", __func__);
}

//only for debug
void  check_es_water_level(void * hdl, int  check_interval/*ms*/)
{
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)hdl;
    u32 videoFreeSpaceSize = 0;
    u32 audioFreeSpaceSize = 0;
    static u32  last_check_tick = 0;
    u32 current_tick = 0;
    current_tick = mtos_ticks_get();

    if (current_tick - last_check_tick >= check_interval / 10) {
        last_check_tick = current_tick;
        vdec_get_es_buf_space(p_file_seq->p_vdec_dev, &videoFreeSpaceSize);
        aud_file_getleftesbuffer_vsb(p_file_seq->p_audio_dev, &audioFreeSpaceSize);
        OS_PRINTF("[a/v]:[%ld K/%ld]\n", (3 * 1024) - videoFreeSpaceSize, 128 * 1024 - audioFreeSpaceSize);
    }
}

#if defined(CHIP_CONCERTO) && defined(__LINUX__)
#else
//peacer add
//stress test for udp recv

#ifdef  WITH_TCPIP_PROTOCOL
void test_udp_recv()
{
    int sock_fd_v = 0;
    int recv_num = 0;
    int client_len = 0;
    struct sockaddr_in addr_serv_video;
    struct sockaddr_in addr_client;
    OS_PRINTF("[%s] ==start start !!!\n", __func__);
    sock_fd_v = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock_fd_v < 0) {
        OS_PRINTF("[ERROR]socket");
        return;
    } else {
        OS_PRINTF("[%s] sock sucessful\n", __func__);
    }

    memset(&addr_serv_video, 0, sizeof(struct sockaddr_in));
    addr_serv_video.sin_family = AF_INET;
    addr_serv_video.sin_port = htons(8888);
    addr_serv_video.sin_addr.s_addr = htonl(INADDR_ANY);
    char recv_frame[1500];
    client_len = sizeof(struct sockaddr_in);

    if (bind(sock_fd_v, (struct sockaddr *)&addr_serv_video, sizeof(struct sockaddr_in)) < 0) {
        OS_PRINTF("bind errro\n");
        return;
    } else {
        OS_PRINTF("[%s] bind vid sucess\n", __func__);
    }

    fd_set rfds;
    fd_set efds;
    int maxfd = 0;

    if (sock_fd_v > maxfd) {
        maxfd = sock_fd_v;
    }

    OS_PRINTF("[%s] maxfd:%d\n", __func__, maxfd);
    static u8 g_firstPacketArrive = 0;
    u32 start_time = 0;
    u32 end_time = 0;
    int totalRecvBytes = 0;

    while (1) {
        FD_ZERO(&rfds);
        FD_ZERO(&efds);
        FD_SET(sock_fd_v, &rfds);
        FD_SET(sock_fd_v, &efds);
        struct timeval timeoutVal;
        int nfds;
        timeoutVal.tv_sec = 2;
        timeoutVal.tv_usec = 0;
        nfds = select(maxfd + 1, &rfds, NULL, &efds, &timeoutVal);

        if (nfds > 0) {
            if (FD_ISSET(sock_fd_v, &rfds)) {
                recv_num = 0;
                recv_num = recvfrom(sock_fd_v, recv_frame, 1500, 0, (struct sockaddr *)&addr_client, &client_len);

                if (recv_num < 0) {
                    OS_PRINTF("recv data error error !!!\n");
                    return;
                } else {
                    if (g_firstPacketArrive == 0) {
                        start_time =    mtos_ticks_get();
                        g_firstPacketArrive = 1;
                        OS_PRINTF("[%s] @@@start_time:%ld:\n", __func__, start_time);
                    }

                    OS_PRINTF("[%s] video data arrive [%d] bytes\n", __func__, recv_num);
                    totalRecvBytes += recv_num;

                    if (recv_num == 8) {
                        int * p_end_code = (int *)recv_frame;

                        if (*p_end_code == 0x11ff22ee) {
                            OS_PRINTF("find end code !!!!!!!!\n");
                            break;
                        }
                    }
                }
            }
        } else {
            OS_PRINTF("overtime do nothing!!!!\n");
        }
    }

    end_time =  mtos_ticks_get();
    OS_PRINTF("consume:%ld ms!!\n", (end_time - start_time) * 10);
    OS_PRINTF("totalRecvBytes:%ld!!\n", totalRecvBytes);
    close(sock_fd_v);
}

#endif

#include "fcrc.h"

#define TS_PACKET_SIZE (188)

static u8 continuity_counter[0x1fff + 1] = {0};

static inline int next_continuity_count(u16 pid)
{
    u8 next = (continuity_counter[pid] + 1) & 0x0f;
    continuity_counter[pid] = next;
    return next;
}

static int TS_add_packet_head(u16 pid,
                              int      data_len,
                              u8     TS_hdr[TS_PACKET_SIZE],
                              int   *  TS_hdr_len)
{
    u32 controls = 0;
    int     pointer, ii;

    if (data_len > (TS_PACKET_SIZE - 5)) { // i.e., 183
        OS_PRINTF("[ERROR]PMT/PAT data for PID %02x is too long (%d > 183)",
                  pid, data_len);
        return 1;
    }

    TS_hdr[0] = 0x47;
    TS_hdr[1] = (u8)(0x40 | ((pid & 0x1f00) >> 8));
    TS_hdr[2] = (u8)(pid & 0xff);
    controls = 0x10;
    TS_hdr[3] = (u8)(controls | next_continuity_count(pid));
    pointer = 0;//(u8)(TS_PACKET_SIZE - 5 - data_len);
    TS_hdr[4] = pointer;

    for (ii = 0; ii < pointer; ii++) {
        TS_hdr[5 + ii] = 0xff;
    }

    *TS_hdr_len = 5 + pointer;
    return 0;
}

static int write_TS_packet_parts(u8      TS_packet[TS_PACKET_SIZE],
                                 int         TS_hdr_len,
                                 u8        data[],
                                 int         data_len)
{
    if (data_len > 0) {
        memcpy(&(TS_packet[TS_hdr_len]), data, data_len);
    }

    return 0;
}

int write_pat(u16  pmt_pid_list[],
              u16 prog_num,
              u8 pat[TS_PACKET_SIZE])
{
    int      ii;
    u8     data[TS_PACKET_SIZE];
    int      TS_hdr_len;
    int      err;
    int      section_length;
    int      offset, data_length;
    u32 crc32;
    section_length = 9 + prog_num * 4;

    if (section_length > 188) {
        mtos_printk("[ERROR]The pat section length is over 188bytes!\n");
        return 1;
    }

    memset(pat, 0xff, TS_PACKET_SIZE);
    data[0] = 0x00;
    // The section length is fixed because our data is fixed
    data[1] = (u8)(0xb0 | ((section_length & 0x0F00) >> 8));
    data[2] = (u8)(section_length & 0x0FF);
    data[3] = (u8)((0x0 & 0xFF00) >> 8);
    data[4] = (u8)(0x0 & 0x00FF);
    data[5] = 0xc1;
    data[6] = 0x00;
    data[7] = 0x00;
    offset = 8;

    for (ii = 0; ii < prog_num; ii++) {
        data[offset + 0] = (u8)((0x0001 & 0xFF00) >> 8);
        data[offset + 1] = (u8)(0x0001 & 0x00FF);
        data[offset + 2] = (u8)(0xE0 | ((pmt_pid_list[ii] & 0x1F00) >> 8));
        data[offset + 3] = (u8)(pmt_pid_list[ii] & 0x00FF);
        offset += 4;
    }

    crc32 = crc_fast_calculate(CRC32_ARITHMETIC_CCITT, 0xFFFFFFFF, data, offset);
    data[offset] = (u8)((crc32 & 0xff000000) >> 24);
    data[offset + 1] = (u8)((crc32 & 0x00ff0000) >> 16);
    data[offset + 2] = (u8)((crc32 & 0x0000ff00) >>  8);
    data[offset + 3] = (u8)(crc32 & 0x000000ff);
    data_length = offset + 4;

    if (data_length != section_length + 3) {
        mtos_printk("[ERROR]PAT length %d, section length+3 %d\n",
                    data_length, section_length + 3);
        return 1;
    }

    err = TS_add_packet_head(0x00, data_length, pat, &TS_hdr_len);

    if (err) {
        mtos_printk("[ERROR]Error constructing PAT packet header\n");
        return 1;
    }

    write_TS_packet_parts(pat, TS_hdr_len, data, data_length);
    return 0;
}

u8 video_src_convert(vdec_src_fmt_t vid_type)
{
    switch (vid_type) {
        case VIDEO_MPEG:
            return 0x02;
            break;

        case VIDEO_H264:
            return 0x1b;
            break;

        case VIDEO_AVS:
            return 0x42;
            break;

        default:
            OS_PRINTF("[ERROR]video format %x is not support\n", vid_type);
            return 0x02;
            break;
    }
}

u8 audio_src_convert(adec_src_fmt_vsb_t aud_type)
{
    switch (aud_type) {
        case AUDIO_MP1:
            return 0x03;
            break;

        case AUDIO_MP2:
        case AUDIO_MP3:
            return 0x04;
            break;

        case AUDIO_AAC:
            return 0x0f;
            break;

        case AUDIO_AC3_VSB:
            return 0x81;
            break;

        case AUDIO_EAC3:
            return 0x87;
            break;

        default:
            OS_PRINTF("[ERROR]audio format %x is not support\n", aud_type);
            return 0x04;
            break;
    }
}

int write_pmt(u16 pmt_pid, u16 vid_pid, vdec_src_fmt_t vid_type,
              u16 aud_pid, adec_src_fmt_vsb_t aud_type,
              u16 pcr_pid, u8 pmt[TS_PACKET_SIZE])
{
    int      ii;
    u8   *  data;//[3+1021];
    u8     TS_packet[TS_PACKET_SIZE];
    int      TS_hdr_len;
    int      err;
    int      section_length;
    int      offset, data_length;
    u32 crc32;

    if (pmt_pid < 0x0010 || pmt_pid > 0x1ffe) {
        OS_PRINTF("### PMT PID %03x is outside legal range\n", pmt_pid);
        return 1;
    }

    data = mtos_malloc(3 + 1021);
    memset(pmt, 0xff, TS_PACKET_SIZE);
    section_length = 13 + 2 * 5;
    data[0] = 0x02;
    data[1] = (u8)(0xb0 | ((section_length & 0x0F00) >> 8));
    data[2] = (u8)(section_length & 0x0FF);
    data[3] = (u8)((0x0001 & 0xFF00) >> 8);
    data[4] = (u8)(0x0001 & 0x00FF);
    data[5] = 0xc1;
    data[6] = 0x00; // section number
    data[7] = 0x00; // last section number
    data[8] = (u8)(0xE0 | ((pcr_pid & 0x1F00) >> 8));
    data[9] = (u8)(pcr_pid & 0x00FF);
    data[10] = 0xF0;
    data[11] = 0;
    offset = 12;
    {
        data[offset + 0] = video_src_convert(vid_type);
        data[offset + 1] = (u8)(0xE0 | ((vid_pid & 0x1F00) >> 8));
        data[offset + 2] = (u8)(vid_pid & 0x00FF);
        data[offset + 3] = 0xF0;
        data[offset + 4] = 0;
        offset += 5;
    }
    {
        data[offset + 0] = audio_src_convert(aud_type);
        data[offset + 1] = (u8)(0xE0 | ((aud_pid & 0x1F00) >> 8));
        data[offset + 2] = (u8)(aud_pid & 0x00FF);
        data[offset + 3] = 0xF0;
        data[offset + 4] = 0;
        offset += 5;
    }
    crc32 = crc_fast_calculate(CRC32_ARITHMETIC_CCITT, 0xFFFFFFFF, data, offset);
    data[offset + 0] = (u8)((crc32 & 0xff000000) >> 24);
    data[offset + 1] = (u8)((crc32 & 0x00ff0000) >> 16);
    data[offset + 2] = (u8)((crc32 & 0x0000ff00) >>  8);
    data[offset + 3] = (u8)(crc32 & 0x000000ff);
    data_length = offset + 4;

    if (data_length != section_length + 3) {
        OS_PRINTF("### PMT length %d, section length+3 %d\n",
                  data_length, section_length + 3);
        goto  ERR_PMT;
    }

    err = TS_add_packet_head(pmt_pid, data_length, pmt, &TS_hdr_len);

    if (err) {
        OS_PRINTF("[ERROR]Error constructing PAT packet header\n");
        goto  ERR_PMT;
    }

    write_TS_packet_parts(pmt, TS_hdr_len, data, data_length);
    mtos_free(data);
    return 0;
ERR_PMT:
    mtos_free(data);
    return 1;
}

#if (!defined(NETWORK_PLAYER_DISABLE))
/*
 *
 *   **************************THE FOLLOWING FOR NETWORK PVR******************************************
 *
 ************************************************************************************
 *
 */
#define NETWORK_PVR_DEFAULT_SIZE  (188*800*8)
#define  RECV_BUF_MAX_LEN  (2048)
#define NETWORK_PVR_STARTCODE 0xaabbccee
#define  RX_TIMEOUT_SEC  (3)


typedef struct {

    u32  start_code;/*0xaa, 0xbb, 0xcc,0xee*/
    u32  vpid;
    u32  apid;
    u32  pcrpid;

    u32  vdec_type;
    u32  adec_type;


} NETWORK_PVR_MSG_T;

typedef struct {
    unsigned int      p_vdec_start;
    unsigned int      vdec_mem_size;
    unsigned  int     p_audio_start;
    unsigned int      audio_mem_size;

} NPVR_FWMEM_CFG_T;
typedef struct {

    int mode;/*0: means sender ;  1:means receiver*/
    u32 lock;

    BOOL  forceExit; /*TRUE:   force background task to exit immediately
FALSE:
              */

    BOOL  ExitOK;/*identify whether the task has finished successfully*/

    BOOL  is_bg_task_live;/*whether backgroud task is working
TRUE:   task is alive
FALSE:  task is dead

                   */


    int  send_fd; /*this socket fd for send data*/
    int  recv_fd; /*this socket fd for recv data*/

#ifdef  WITH_TCPIP_PROTOCOL
    struct sockaddr_in addr_recv;
#endif
    char recv_buf[RECV_BUF_MAX_LEN];
    NETWORK_PVR_MSG_T  last_msg;/*save the lastest message*/

    //public
    int bg_task_prio;
    u32 bg_task_size;
    u16 video_pid;
    u16 audio_pid;
    u16 pcr_pid;
    int  pmt_send_flag;//0:don't send,1:send psi table
    int  pmt_ready_flag;//0:psi is empty,1:psi table is ok
    u32 psi_interval_num;//pmt frequency set by bitrate,500kbps default set as 20
    u32 last_left_pos;
    u16 pmt_pid[1];
    u8 psi_buf[188 * 2] ;

    //if 0, driver will use a default size buffer, must 16 bytes align
    void * record_buffer;
    //if 0, driver will use a default size
    u32 recode_buffer_size;
    dmx_input_type_t stream_in;
    u32  vdec_type;
    u32  adec_type;

    u8 * npvr_ts_buffer ;
    u8 * npvr_rcv_buffer ;
    NPVR_FWMEM_CFG_T g_npvr_FwMemCfg;



    //driver private
    dmx_chanid_t video_chan;
    dmx_chanid_t audio_chan;
    dmx_chanid_t pcr_chan;
    dmx_device_t * p_dmx_dev;
    void * p_bg_task_stack;

    void * p_pti_dev ;
    void * p_disp_dev;
    void * p_vdec_dev;
    void * p_audio_dev ;

    // TODO:  any field you want to add



} NETWORK_PVR_HANDLER;


static int send_data_slice(void * param,  u8 * start, u32 len)
{
    NETWORK_PVR_HANDLER * p_hdl = (NETWORK_PVR_HANDLER *)param;
    int send_num = 0;
    u32 pos = 0, size = 0, pack_size = 188 * 10;
#ifdef  WITH_TCPIP_PROTOCOL

    if (p_hdl->pmt_send_flag && p_hdl->pmt_ready_flag) {
        if (len < pack_size) {
            send_num += sendto(p_hdl->send_fd, start + pos,
                               len, 0, (struct sockaddr *) & (p_hdl->addr_recv), sizeof(struct sockaddr_in));
            p_hdl->last_left_pos += len / 188;

            if (p_hdl->last_left_pos >= p_hdl->psi_interval_num) {
                send_num += sendto(p_hdl->send_fd, p_hdl->psi_buf,
                                   188 * 2, 0, (struct sockaddr *) & (p_hdl->addr_recv), sizeof(struct sockaddr_in));
                p_hdl->last_left_pos = 0;
            }

            return send_num = len;
        }
    }

    for (pos = 0; pos < len; pos += pack_size) {
        if (p_hdl->pmt_send_flag && p_hdl->pmt_ready_flag) {
            if (p_hdl->last_left_pos >= p_hdl->psi_interval_num) {
                send_num += sendto(p_hdl->send_fd, p_hdl->psi_buf,
                                   188 * 2, 0, (struct sockaddr *) & (p_hdl->addr_recv), sizeof(struct sockaddr_in));
                p_hdl->last_left_pos = 0;
            }
        }

        size = (pos + pack_size) > len ? (len - pos) : pack_size;
        send_num += sendto(p_hdl->send_fd, start + pos,
                           size, 0, (struct sockaddr *) & (p_hdl->addr_recv), sizeof(struct sockaddr_in));

        if (p_hdl->pmt_send_flag && p_hdl->pmt_ready_flag) {
            p_hdl->last_left_pos += size / 188;

            if (p_hdl->last_left_pos >= p_hdl->psi_interval_num) {
                send_num += sendto(p_hdl->send_fd, p_hdl->psi_buf,
                                   188 * 2, 0, (struct sockaddr *) & (p_hdl->addr_recv), sizeof(struct sockaddr_in));
                p_hdl->last_left_pos = 0;
            }
        }
    }

#endif
    return send_num;
}


/*
 *
 *
 *
 *
 *
 */
static  void * send_ts_task(void * param)
{
#if((defined(WITH_TCPIP_PROTOCOL))&&!defined(__LINUX__))
    OS_PRINTF("[%s] start start ...!!!\n", __func__);
    RET_CODE ret;
    u64 rec_pos = 0;
    u64 prev_rec_pos = 0;
    u64 data_len;
    u64 data_loop_cnt = 0;
    int send_num = 0;
    u32 buffer_size;
    u32 len = 0;
    NETWORK_PVR_HANDLER * p_hdl = (NETWORK_PVR_HANDLER *)param;

    if (p_hdl->recode_buffer_size) {
        buffer_size = p_hdl->recode_buffer_size;
    } else {
        buffer_size = NETWORK_PVR_DEFAULT_SIZE;
    }

    p_hdl->is_bg_task_live = TRUE;
    p_hdl->last_msg.adec_type = p_hdl->adec_type;
    p_hdl->last_msg.vdec_type = p_hdl->vdec_type;
    p_hdl->last_msg.apid = p_hdl->audio_pid;
    p_hdl->last_msg.vpid = p_hdl->video_pid;
    p_hdl->last_msg.pcrpid = p_hdl->pcr_pid;
    p_hdl->last_msg.start_code = NETWORK_PVR_STARTCODE;

    while (p_hdl->forceExit == FALSE) {
        ret = dmx_rec_get_data(p_hdl->p_dmx_dev, 0, &rec_pos);

        if (rec_pos == 0) {
            mtos_task_sleep(10);
            continue;
        }

        OS_PRINTF("%d\n", (u32)rec_pos);

        // data lose check
        if ((rec_pos / buffer_size) > data_loop_cnt && (rec_pos % buffer_size) > prev_rec_pos) {
            OS_PRINTF("buffer roll back, data lose\n");
        }

        data_loop_cnt = rec_pos / buffer_size;
        //send info message
        sendto(p_hdl->send_fd, &p_hdl->last_msg, sizeof(p_hdl->last_msg), 0,
               (struct sockaddr *) & (p_hdl->addr_recv), sizeof(struct sockaddr_in));
        //date send
        rec_pos = rec_pos % buffer_size;

        if (rec_pos > prev_rec_pos) {
            /*send_num = sendto(p_hdl->send_fd,p_hdl->record_buffer+ prev_rec_pos,
              rec_pos - prev_rec_pos,0,
              (struct sockaddr *)&(p_hdl->addr_recv),sizeof(struct sockaddr_in));*/
            send_num = send_data_slice(p_hdl, p_hdl->record_buffer + prev_rec_pos,
                                       rec_pos - prev_rec_pos);
            OS_PRINTF("send %d\n", send_num);
        } else if (rec_pos < prev_rec_pos) {
            /*
               send_num = sendto(p_hdl->send_fd,p_hdl->record_buffer+ prev_rec_pos,
               buffer_size - prev_rec_pos,0,
               (struct sockaddr *)&(p_hdl->addr_recv),sizeof(struct sockaddr_in));

               send_num += sendto(p_hdl->send_fd,p_hdl->record_buffer,rec_pos,0,
               (struct sockaddr *)&(p_hdl->addr_recv),sizeof(struct sockaddr_in));*/
            send_num = send_data_slice(p_hdl, p_hdl->record_buffer + prev_rec_pos,
                                       buffer_size - prev_rec_pos);
            send_num += send_data_slice(p_hdl, p_hdl->record_buffer, rec_pos);
            OS_PRINTF("send %d\n", send_num);
        } else if ((rec_pos == prev_rec_pos)) {
            OS_PRINTF("buffer rollback, data lost\n");
        }

        prev_rec_pos = rec_pos;
    }

    p_hdl->is_bg_task_live = FALSE;
    p_hdl->ExitOK = TRUE;
    OS_PRINTF("[%s]end end ...!!!\n", __func__);
    mtos_task_delete(p_hdl->bg_task_prio);
#endif
    return;
}


/*
 *
 *
 *
 *
 *
 */
static  void  * recv_ts_task(void * param)
{
#if((defined(WITH_TCPIP_PROTOCOL))&&!defined(__LINUX__))
    OS_PRINTF("[%s] start start ...!!!\n", __func__);
    NETWORK_PVR_HANDLER * p_hdl = (NETWORK_PVR_HANDLER *)param;
    fd_set rfds;
    fd_set efds;
    struct sockaddr  addr_client;
    int    client_len = sizeof(struct sockaddr_in);
    struct timeval timeoutVal;
    int nfds, notimeout = 0;
    int maxfd = 0;
    int vdec_inited = 0;
    NETWORK_PVR_MSG_T * p_msg = NULL;
    u32 videoFreeSpaceSize = 0, audFreeSpaceSize = 0;
    BOOL state = FALSE;
    int total_len = 0;
    int gop_len = 0;
    int pos_left = 0;
    dmx_chanid_t video_chan = 0xffff;
    dmx_chanid_t audio_chan = 0xffff;
    dmx_chanid_t pcr_chan = 0xffff;
    dmx_dma_config_t dma_config;
    dmx_play_setting_t play_para;
    int ret = 0;
    int size = 0;
    int align = 0;
    p_hdl->npvr_ts_buffer = (u8 *)mtos_align_malloc(4096, 16);
    p_hdl->npvr_rcv_buffer = (u8 *)mtos_align_malloc(4096, 16);
    /* pti */
    {
        p_hdl->p_pti_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

        if (p_hdl->p_pti_dev == NULL) {
            OS_PRINTF("[%s]p_pti_dev[0x%x] ...!!!\n", __func__, p_hdl->p_pti_dev);
            goto ERROR_HANDLER;
        }
    }
    /* display */
    {
        p_hdl->p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);

        if (p_hdl->p_disp_dev == NULL) {
            OS_PRINTF("[%s]p_disp_dev[0x%x] ...!!!\n", __func__, p_hdl->p_disp_dev);
            goto ERROR_HANDLER;
        }
    }
    /* vdec */
    {
        p_hdl->p_vdec_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);

        if (p_hdl->p_vdec_dev == NULL) {
            OS_PRINTF("[%s]p_vdec_dev[0x%x] ...!!!\n", __func__, p_hdl->p_vdec_dev);
            goto ERROR_HANDLER;
        }

        ret = dev_open(p_hdl->p_vdec_dev, NULL);
        MT_ASSERT(SUCCESS == ret);
    }
    /* audio */
    {
        p_hdl->p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);

        if (p_hdl->p_audio_dev == NULL) {
            OS_PRINTF("[%s]p_audio_dev[0x%x] ...!!!\n", __func__, p_hdl->p_audio_dev);
            goto ERROR_HANDLER;
        }

        ret = dev_open(p_hdl->p_audio_dev, NULL);
        MT_ASSERT(SUCCESS == ret);
    }

    //x_clear_pti_es_counter(p_file_seq);

    if (p_hdl->g_npvr_FwMemCfg.audio_mem_size == 0
        || p_hdl->g_npvr_FwMemCfg.vdec_mem_size == 0
        || p_hdl->g_npvr_FwMemCfg.p_vdec_start == 0
        || p_hdl->g_npvr_FwMemCfg.p_audio_start == 0) {
        OS_PRINTF("[%s] audio_mem_size[%ld]\n", __func__, p_hdl->g_npvr_FwMemCfg.audio_mem_size);
        OS_PRINTF("[%s] vdec_mem_size[%ld]\n", __func__, p_hdl->g_npvr_FwMemCfg.vdec_mem_size);
        OS_PRINTF("[%s] p_audio_start[%x]\n", __func__, p_hdl->g_npvr_FwMemCfg.p_audio_start);
        OS_PRINTF("[%s] p_vdec_start[%x]\n", __func__, p_hdl->g_npvr_FwMemCfg.p_vdec_start);
        goto ERROR_HANDLER;
    }

    /*
     *   require necessary resource for audio firmware
     */

    if (p_hdl->recv_fd > maxfd) {
        maxfd = p_hdl->recv_fd;
    }

    p_hdl->is_bg_task_live = TRUE;

    while (p_hdl->forceExit == FALSE) {
        FD_ZERO(&rfds);
        FD_ZERO(&efds);
        FD_SET(p_hdl->recv_fd, &rfds);
        FD_SET(p_hdl->recv_fd, &efds);
        timeoutVal.tv_sec = RX_TIMEOUT_SEC;
        timeoutVal.tv_usec = 0;
        nfds = select(maxfd + 1, &rfds, NULL, &efds, &timeoutVal);

        if (nfds > 0) {
            if (FD_ISSET(p_hdl->recv_fd, &rfds)) {
                int recv_num = 0;
                recv_num = recvfrom(p_hdl->recv_fd, p_hdl->recv_buf,
                                    RECV_BUF_MAX_LEN, 0, (struct sockaddr *)&addr_client, &client_len);

                if (recv_num < 0) {
                    OS_PRINTF("[%s][ERROR] receive data error error !!!\n", __func__);
                    continue;
                } else if (recv_num == 0) {
                    OS_PRINTF("[%s][ERROR] recv_num[%d] !!!\n", __func__, recv_num);
                    continue;
                } else {
                    OS_PRINTF("[%s] rx[%d] bytes\n", __func__, recv_num);
                }

                memcpy(p_hdl->npvr_rcv_buffer, p_hdl->recv_buf, recv_num);
                p_msg = (NETWORK_PVR_MSG_T *)p_hdl->npvr_rcv_buffer;

                if (p_msg->start_code == NETWORK_PVR_STARTCODE && !vdec_inited) {
                    OS_PRINTF("[%s] NETWORK_PVR_STARTCODE found !!!\n", __func__);
                    p_hdl->video_pid = p_msg->vpid;
                    p_hdl->audio_pid = p_msg->apid;
                    p_hdl->pcr_pid = p_hdl->video_pid;
                    p_hdl->vdec_type = p_msg->vdec_type;
                    p_hdl->adec_type = p_msg->adec_type;

                    if (p_hdl->audio_pid != 0x1fff) {
                        play_para.pid = p_hdl->audio_pid;
                        play_para.type = DMX_AUDIO_TYPE;
                        play_para.stream_in = DMX_INPUT_EXTERN_DMA;
                        ret = dmx_play_chan_open(p_hdl->p_pti_dev, &play_para, &audio_chan);

                        if (ret != SUCCESS) {
                            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                        }
                    }

                    if (p_hdl->video_pid != 0x1fff) {
                        play_para.pid = p_hdl->video_pid;
                        play_para.type = DMX_VIDEO_TYPE;
                        play_para.stream_in = DMX_INPUT_EXTERN_DMA;
                        ret = dmx_play_chan_open(p_hdl->p_pti_dev, &play_para, &video_chan);

                        if (ret != SUCCESS) {
                            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                        }
                    }

                    if (p_hdl->pcr_pid != 0x1fff) {
                        play_para.pid = p_hdl->pcr_pid;
                        play_para.type = DMX_PCR_TYPE;
                        play_para.stream_in = DMX_INPUT_EXTERN_DMA;
                        ret = dmx_play_chan_open(p_hdl->p_pti_dev, &play_para, &pcr_chan);

                        if (ret != SUCCESS) {
                            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                        }
                    }

                    if (video_chan != 0xffff) {
                        ret = dmx_chan_start(p_hdl->p_pti_dev, video_chan);

                        if (ret != SUCCESS) {
                            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                        }
                    }

                    if (audio_chan != 0xffff) {
                        ret = dmx_chan_start(p_hdl->p_pti_dev, audio_chan);

                        if (ret != SUCCESS) {
                            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                        }
                    }

                    if (pcr_chan != 0xffff) {
                        ret = dmx_chan_start(p_hdl->p_pti_dev, pcr_chan);

                        if (ret != SUCCESS) {
                            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                        }
                    }

                    size = 0;
                    align = 0;
                    aud_get_buf_requirement_vsb(p_hdl->p_audio_dev, (u32 *)&size, (u32 *)&align);

                    if (p_hdl->g_npvr_FwMemCfg.audio_mem_size < size) {
                        OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                    }

                    aud_set_buf_vsb(p_hdl->p_audio_dev, (p_hdl->g_npvr_FwMemCfg.p_audio_start), size);

                    if (p_hdl->audio_pid != 0x1fff) {
                        aud_start_vsb(p_hdl->p_audio_dev, p_hdl->adec_type, AUDIO_TS_FILE);
                    }

                    if (p_hdl->video_pid != 0x1fff) {
                        size = 0;
                        align = 0;
                        vdec_get_buf_requirement(p_hdl->p_vdec_dev, VDEC_BUFFER_AD, (u32 *)&size, (u32 *)&align);

                        if (p_hdl->g_npvr_FwMemCfg.vdec_mem_size < size) {
                            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                        }

                        vdec_set_buf(p_hdl->p_vdec_dev, VDEC_BUFFER_AD, p_hdl->g_npvr_FwMemCfg.p_vdec_start);
                        ret = vdec_start(p_hdl->p_vdec_dev, p_hdl->vdec_type, 2);

                        if (SUCCESS != ret) {
                            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                        }
                    }

                    dmx_av_reset(p_hdl->p_pti_dev);
                    vdec_inited = 1;
                    continue;
                }

                if (!vdec_inited || (vdec_inited && p_msg->start_code == NETWORK_PVR_STARTCODE)) {
                    continue;
                }

                memcpy(p_hdl->npvr_ts_buffer + pos_left, p_hdl->recv_buf, recv_num);
                total_len = pos_left + recv_num;
                pos_left = total_len % 188;
                gop_len = total_len - pos_left;
                dma_config.mem_address = (u32) p_hdl->npvr_ts_buffer;

                while (gop_len) {
                    vdec_get_es_buf_space(p_hdl->p_vdec_dev, &videoFreeSpaceSize);

                    if (videoFreeSpaceSize < 300) {
                        mtos_task_sleep(1);
                        continue;
                    }

                    dma_config.data_length = gop_len;
                    dma_config.ts_clk = 40000000;
                    dmx_set_dma_config(p_hdl->p_pti_dev, &dma_config);

                    while (1) {
                        state = dmx_get_dma_state(p_hdl->p_pti_dev);

                        if (state == TRUE) {
                            break;
                        }

                        mtos_task_sleep(1);
                    }

                    break;
                }

                if (pos_left) {
                    memcpy(p_hdl->npvr_rcv_buffer, p_hdl->npvr_ts_buffer + gop_len, pos_left);
                    memcpy(p_hdl->npvr_ts_buffer, p_hdl->npvr_rcv_buffer, pos_left);
                }
            }
        } else {
            OS_PRINTF("[%s] 1sencod  overtime do nothing!!!!\n", __func__);
        }
    }

    if (p_hdl->video_pid != 0x1fff) {
        vdec_stop(p_hdl->p_vdec_dev);
    }

    if (p_hdl->audio_pid != 0x1fff) {
        aud_stop_vsb(p_hdl->p_audio_dev);
    }

    if (audio_chan != 0xffff) {
        ret = dmx_chan_stop(p_hdl->p_pti_dev, audio_chan);

        if (ret != SUCCESS) {
            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
        }

        ret = dmx_chan_close(p_hdl->p_pti_dev, audio_chan);

        if (ret != SUCCESS) {
            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
        }
    }

    if (video_chan != 0xffff) {
        ret = dmx_chan_stop(p_hdl->p_pti_dev, video_chan);

        if (ret != SUCCESS) {
            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
        }

        ret = dmx_chan_close(p_hdl->p_pti_dev, video_chan);

        if (ret != SUCCESS) {
            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
        }
    }

    if (pcr_chan != 0xffff) {
        ret = dmx_chan_stop(p_hdl->p_pti_dev, pcr_chan);

        if (ret != SUCCESS) {
            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
        }

        ret = dmx_chan_close(p_hdl->p_pti_dev, pcr_chan);

        if (ret != SUCCESS) {
            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
        }
    }

ERROR_HANDLER:
    NetworkPVR_SetFwMem(p_hdl, 0, 0, 0, 0);
    dmx_av_reset(p_hdl->p_pti_dev);

    if (p_hdl->npvr_rcv_buffer) {
        mtos_free(p_hdl->npvr_rcv_buffer);
        p_hdl->npvr_rcv_buffer = NULL;
    }

    if (p_hdl->npvr_ts_buffer) {
        mtos_free(p_hdl->npvr_ts_buffer);
        p_hdl->npvr_rcv_buffer = NULL;
    }

    p_hdl->npvr_ts_buffer = NULL;
    p_hdl->is_bg_task_live = FALSE;
    p_hdl->ExitOK = TRUE;
    OS_PRINTF("[%s]end end ...!!!\n", __func__);
    mtos_task_delete(p_hdl->bg_task_prio);
    return;
#endif
}


/*
 *     mode:  0: sender   1:recevier
 *     stack_size:
 *     task_priority:
 *
 *
 */
void  * NetworkPVR_Init(NW_PVR_WORK_MODE mode, int stack_size, int task_piro)
{
    OS_PRINTF("[%s] start start ...!!!\n", __func__);
    RET_CODE ret;
    dmx_rec_setting_t rec_t;
#ifdef  WITH_TCPIP_PROTOCOL
    NETWORK_PVR_HANDLER * p_pvr_handle = (NETWORK_PVR_HANDLER *)mtos_malloc(sizeof(NETWORK_PVR_HANDLER));
    memset(p_pvr_handle, 0, sizeof(NETWORK_PVR_HANDLER));
    ret  = mtos_sem_create((os_sem_t *) & (p_pvr_handle->lock), TRUE);
    MT_ASSERT(ret == TRUE);
    p_pvr_handle->p_dmx_dev = (dmx_device_t *)dev_find_identifier(NULL
                              , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_pvr_handle->p_dmx_dev->p_base);
    p_pvr_handle->mode = mode;
    p_pvr_handle->bg_task_prio = task_piro;
    p_pvr_handle->bg_task_size = stack_size;
    p_pvr_handle->is_bg_task_live = FALSE;
    OS_PRINTF("[%s]bg_task_prio[%d]\n", __func__, task_piro);
    OS_PRINTF("[%s]stack_size[%ld]\n", __func__, stack_size);

    if (p_pvr_handle->mode == SEND_MODE) {
        OS_PRINTF("[%s] This BOX Is A Sender...!!!\n", __func__);
    } else if (p_pvr_handle->mode == RECV_MODE) {
        OS_PRINTF("[%s] This BOX Is A Receiver...!!!\n", __func__);
    } else {
        OS_PRINTF("[%s][ERROR][ERROR] invalid mode  ...!!!\n", __func__);
        goto ERROR_HDL;
    }

    return (void *)p_pvr_handle;
ERROR_HDL:

    if (p_pvr_handle->lock) {
        mtos_sem_destroy((os_sem_t *) & (p_pvr_handle->lock), 0);
    }

    mtos_free(p_pvr_handle);
    p_pvr_handle = NULL;
    // TODO: add other exception handler code
    OS_PRINTF("[%s]end end ...!!!\n", __func__);
#endif
    return (void *)NULL;
}

/*
 *
 *
 *
 *
 *
 */
void  NetworkPVR_ChangeMode(void * hdl, int mode)
{
    NETWORK_PVR_HANDLER * p_hdl = (NETWORK_PVR_HANDLER *)hdl;
    OS_PRINTF("[%s] start start ...!!!\n", __func__);

    if (p_hdl == NULL) {
        OS_PRINTF("[%s][ERROR] hdl == NULL!\n", __func__);
        return ;
    }

    mtos_sem_take((os_sem_t *)(&(p_hdl->lock)), 0);

    if (p_hdl->mode  != mode  &&  p_hdl->is_bg_task_live == FALSE) {
        OS_PRINTF("[%s] new mode [%d] ...!!!\n", __func__, mode);
        p_hdl->mode = mode;
    } else {
        OS_PRINTF("[%s]do nothing ...!!!\n", __func__);
    }

    mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
    OS_PRINTF("[%s]end end ...!!!\n", __func__);
}

RET_CODE    NetworkPVR_SetParam(void * hdl, NETWORK_PVR_PARM * param)
{
    NETWORK_PVR_HANDLER * p_hdl = (NETWORK_PVR_HANDLER *)hdl;
    p_hdl->audio_pid = param->audio_pid;
    p_hdl->video_pid = param->video_pid;
    p_hdl->pcr_pid = param->pcr_pid;
    p_hdl->stream_in = param->stream_in;
    p_hdl->adec_type = param->adec_type;
    p_hdl->vdec_type = param->vdec_type;
    p_hdl->record_buffer = param->record_buffer;
    p_hdl->recode_buffer_size = param->recode_buffer_size;
    return SUCCESS;
}
RET_CODE    NetworkPVR_SetPMT(void * hdl, NETWORK_PVR_PARM * param, BOOL send_flag)
{
    NETWORK_PVR_HANDLER * p_hdl = (NETWORK_PVR_HANDLER *)hdl;
    p_hdl->pmt_pid[0] = param->pmt_pid;
    p_hdl->psi_interval_num = param->psi_interval_num ? param->psi_interval_num : 20;

    if (p_hdl->pmt_pid[0] > 2 && send_flag) {
        memset(p_hdl->psi_buf, 0, sizeof(TS_PACKET_SIZE * 2));
        write_pat(p_hdl->pmt_pid, 1, p_hdl->psi_buf);
        write_pmt(p_hdl->pmt_pid[0], p_hdl->video_pid, p_hdl->vdec_type,
                  p_hdl->audio_pid, p_hdl->adec_type, p_hdl->pcr_pid, p_hdl->psi_buf + TS_PACKET_SIZE);
        p_hdl->pmt_ready_flag = 1;
    } else {
        p_hdl->pmt_ready_flag = 0;
    }

    p_hdl->pmt_send_flag = send_flag;
    return SUCCESS;
}

/*
 *
 *
 *
 *
 *
 */
RET_CODE    NetworkPVR_Send(void * hdl, char * dst_addr, int port)
{

#if((defined(WITH_TCPIP_PROTOCOL))&&!defined(__LINUX__))
    NETWORK_PVR_HANDLER * p_hdl = (NETWORK_PVR_HANDLER *)hdl;
    OS_PRINTF("[%s] start start ...!!!\n", __func__);

    if (p_hdl == NULL) {
        OS_PRINTF("[%s][ERROR] hdl == NULL!\n", __func__);
        return ERR_FAILURE;
    }

    mtos_sem_take((os_sem_t *)(&(p_hdl->lock)), 0);
    RET_CODE ret = ERR_FAILURE;
    dmx_rec_setting_t rec_t;

    if (p_hdl->mode  != SEND_MODE || p_hdl->bg_task_size <= 0) {
        OS_PRINTF("[%s][ERROR] error mode!!!\n", __func__);
        OS_PRINTF("[%s][ERROR] bg_task_size [%d]!!!\n", __func__, p_hdl->bg_task_size);
        mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
        return ERR_FAILURE;
    }

    if (p_hdl->is_bg_task_live) {
        OS_PRINTF("[%s][ERROR] backgroud task is already living !!!\n", __func__);
        mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
        return ERR_FAILURE;
    }

    if (p_hdl->mode == SEND_MODE) {
        OS_PRINTF("start config record hw resource\n");

        //init record
        if (p_hdl->video_pid != 0x1fff) {
            OS_PRINTF("video_pid=%d\n", p_hdl->video_pid);
            rec_t.pid = p_hdl->video_pid;
            rec_t.rec_in = DMX_REC_CONFIG0;
            rec_t.stream_in = p_hdl->stream_in;
            rec_t.type_mode = DMX_PARTIAL_TS_PACKET;
            ret = dmx_rec_chan_open(p_hdl->p_dmx_dev, &rec_t,
                                    &p_hdl->video_chan);

            if (ret != SUCCESS) {
                OS_PRINTF("open rec channel for video failed!\n");
                return -1;
            }
        }

        if (p_hdl->audio_pid != 0x1fff) {
            OS_PRINTF("audio_pid=%d\n", p_hdl->audio_pid);
            rec_t.pid = p_hdl->audio_pid;
            rec_t.rec_in = DMX_REC_CONFIG0;
            rec_t.stream_in = p_hdl->stream_in;
            rec_t.type_mode = DMX_PARTIAL_TS_PACKET;
            ret = dmx_rec_chan_open(p_hdl->p_dmx_dev, &rec_t,
                                    &p_hdl->audio_chan);

            if (ret != SUCCESS) {
                OS_PRINTF("open rec channel for audio failed!\n");
                return -1;
            }
        }

        if (p_hdl->recode_buffer_size == 0) {
            p_hdl->record_buffer = (u8 *)mtos_align_malloc(NETWORK_PVR_DEFAULT_SIZE, 16);
            MT_ASSERT(p_hdl->record_buffer != NULL);
        }

        ret = dmx_rec_chan_set_buffer(p_hdl->p_dmx_dev, DMX_REC_CONFIG0,
                                      (u8 *)p_hdl->record_buffer, NETWORK_PVR_DEFAULT_SIZE);

        if (ret != SUCCESS) {
            OS_PRINTF("set buffer failed!\n");
            return ERR_FAILURE;
        }
    }

    p_hdl->send_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (p_hdl->send_fd < 0) {
        OS_PRINTF("[%s][ERROR]socket !!!\n", __func__);
        mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
        return ERR_FAILURE;
    } else {
        OS_PRINTF("[%s] create send sock sucessful!!!!!!!!!!\n", __func__);
    }

    //struct sockaddr_in addr_serv_v;
    memset(&(p_hdl->addr_recv), 0, sizeof(struct sockaddr_in));
    p_hdl->addr_recv.sin_family = AF_INET;
    p_hdl->addr_recv.sin_port = htons(port);
    p_hdl->addr_recv.sin_addr.s_addr = inet_addr(dst_addr);//dst_addr = "192.168.35.122"
    ret = dmx_chan_start(p_hdl->p_dmx_dev, p_hdl->video_chan);

    if (ret != SUCCESS) {
        OS_PRINTF("start video channel  failed!\n");
        mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
        return  ERR_FAILURE;
    }

    ret = dmx_chan_start(p_hdl->p_dmx_dev, p_hdl->audio_chan);

    if (ret != SUCCESS) {
        OS_PRINTF("start audio channel  failed!\n");
        mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
        return  ERR_FAILURE;
    }

    p_hdl->p_bg_task_stack = mtos_malloc(p_hdl->bg_task_size);
    MT_ASSERT(p_hdl->p_bg_task_stack != NULL);
    memset(p_hdl->p_bg_task_stack, 0, p_hdl->bg_task_size);
    ret = mtos_task_create((u8 *)"send thread", send_ts_task, hdl,
                           p_hdl->bg_task_prio, p_hdl->p_bg_task_stack,
                           p_hdl->bg_task_size);
    OS_PRINTF("[%s]end end ...!!!\n", __func__);
    mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
#endif
    return  SUCCESS;
}



/*
 *
 *
 *
 *
 *
 */
RET_CODE    NetworkPVR_Recv(void * hdl, int recv_port)
{
    OS_PRINTF("[%s] start start ...!!!\n", __func__);
#ifdef  WITH_TCPIP_PROTOCOL
    NETWORK_PVR_HANDLER * p_hdl = (NETWORK_PVR_HANDLER *)hdl;
    mtos_sem_take((os_sem_t *)(&(p_hdl->lock)), 0);

    if (p_hdl->mode  != RECV_MODE || p_hdl->bg_task_size <= 0) {
        OS_PRINTF("[%s][ERROR] error mode!!!\n", __func__);
        OS_PRINTF("[%s][ERROR] bg_task_size [%d]!!!\n", __func__, p_hdl->bg_task_size);
        mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
        return ERR_FAILURE;
    }

    if (p_hdl->is_bg_task_live) {
        OS_PRINTF("[%s][ERROR] backgroud task is already living !!!\n", __func__);
        mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
        return ERR_FAILURE;
    }

    int  client_len = sizeof(struct sockaddr_in);
    p_hdl->recv_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (p_hdl->recv_fd < 0) {
        OS_PRINTF("[%s][ERROR]socket !!!\n", __func__);
        mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
        return ERR_FAILURE;
    } else {
        OS_PRINTF("[%s] create recv sock sucessful!!!!!!!!!!\n", __func__);
    }

    memset(&(p_hdl->addr_recv), 0, sizeof(struct sockaddr_in));
    p_hdl->addr_recv.sin_family = AF_INET;
    p_hdl->addr_recv.sin_port = htons(recv_port);
    p_hdl->addr_recv.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(p_hdl->recv_fd, (struct sockaddr *) & (p_hdl->addr_recv), sizeof(struct sockaddr_in)) < 0) {
        OS_PRINTF("[%s] bind errro\n", __func__);
        mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
        return ERR_FAILURE;
    } else {
        OS_PRINTF("[%s] bind socket sucess!!!!!!!!\n", __func__);
    }

    p_hdl->p_bg_task_stack = mtos_malloc(p_hdl->bg_task_size);
    MT_ASSERT(p_hdl->p_bg_task_stack != NULL);
    memset(p_hdl->p_bg_task_stack, 0, p_hdl->bg_task_size);
    int ret = mtos_task_create((u8 *)"recv thread", recv_ts_task, hdl,
                               p_hdl->bg_task_prio, p_hdl->p_bg_task_stack,
                               p_hdl->bg_task_size);
    OS_PRINTF("[%s]end end ...!!!\n", __func__);
    mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
#endif
    return SUCCESS;
}

/*
 *
 *
 *
 *
 *
 */
void NetworkPVR_Stop(void * hdl)
{
    OS_PRINTF("[%s] start start ...!!!\n", __func__);
#ifdef  WITH_TCPIP_PROTOCOL
    NETWORK_PVR_HANDLER * p_hdl = (NETWORK_PVR_HANDLER *)hdl;
    RET_CODE ret = ERR_FAILURE;
    mtos_sem_take((os_sem_t *)(&(p_hdl->lock)), 0);

    if (p_hdl->is_bg_task_live ==  FALSE) {
        OS_PRINTF("[%s]  do nothing is_bg_task_live[%d] !!!\n", __func__, p_hdl->is_bg_task_live);
        mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
        return;
    }

    p_hdl->forceExit = TRUE;

    do {
        mtos_task_sleep(50);
        OS_PRINTF("[%s] wait task finished ...!!!\n", __func__);
    } while (p_hdl->ExitOK == FALSE);

    if (p_hdl->mode == SEND_MODE) {
        ret = dmx_chan_stop(p_hdl->p_dmx_dev, p_hdl->audio_chan);
        ret = dmx_chan_stop(p_hdl->p_dmx_dev, p_hdl->video_chan);
        ret = dmx_chan_close(p_hdl->p_dmx_dev, p_hdl->audio_chan);
        ret = dmx_chan_close(p_hdl->p_dmx_dev, p_hdl->video_chan);
    } else if (p_hdl->mode == RECV_MODE) {
        // TODO: !!!!!!!!!!!!!!!!
    }

    if (p_hdl->mode == SEND_MODE &&   p_hdl->send_fd >= 0) {
#ifndef __LINUX__
        closesocket(p_hdl->send_fd);
#else
        close(p_hdl->send_fd);
#endif
        p_hdl->send_fd = -1;
    } else if (p_hdl->mode == RECV_MODE &&   p_hdl->recv_fd >= 0) {
#ifndef __LINUX__
        closesocket(p_hdl->recv_fd);
#else
        close(p_hdl->recv_fd);
#endif
        p_hdl->recv_fd = -1;
    }

    mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
    OS_PRINTF("[%s]end end ...!!!\n", __func__);
#endif
    return;
}


/*
 *
 *
 *
 *
 *
 */
RET_CODE   NetworkPVR_Deinit(void * hdl)
{
    OS_PRINTF("[%s] start start ...!!!\n", __func__);
#ifdef  WITH_TCPIP_PROTOCOL
    NETWORK_PVR_HANDLER * p_hdl = (NETWORK_PVR_HANDLER *)hdl;
    RET_CODE ret = ERR_FAILURE;
    mtos_sem_take((os_sem_t *)(&(p_hdl->lock)), 0);

    if (p_hdl->ExitOK) {
        if (p_hdl->mode == SEND_MODE) {
            ret = dmx_chan_stop(p_hdl->p_dmx_dev, p_hdl->audio_chan);
            ret = dmx_chan_close(p_hdl->p_dmx_dev, p_hdl->audio_chan);
            ret = dmx_chan_stop(p_hdl->p_dmx_dev, p_hdl->video_chan);
            ret = dmx_chan_close(p_hdl->p_dmx_dev, p_hdl->video_chan);

            if (p_hdl->recode_buffer_size == 0) {
                mtos_align_free(p_hdl->record_buffer);
            }
        } else if (p_hdl->mode == RECV_MODE) {
            // TODO: !!!!!!!!!!!!!!
        }

        mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
        mtos_sem_destroy((os_sem_t *) & (p_hdl->lock), 0);

        if (p_hdl->p_bg_task_stack) {
            mtos_free(p_hdl->p_bg_task_stack);
        }

        mtos_free(p_hdl);
        p_hdl = NULL;
        return SUCCESS;
    } else {
        OS_PRINTF("[%s][ERROR] do nothing ,please stop first...!!!\n", __func__);
    }

    mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
    OS_PRINTF("[%s]end end ...!!!\n", __func__);
#endif
    return ERR_FAILURE;
}

/*
 *
 *
 *
 */

void NetworkPVR_SetFwMem(void * param, unsigned int p_vdec_start,
                         unsigned int  vdec_mem_len, unsigned int p_audio_mem_start,
                         unsigned int audio_mem_len)
{
    NETWORK_PVR_HANDLER * p_hdl = (NETWORK_PVR_HANDLER *)param;
    p_hdl->g_npvr_FwMemCfg.p_vdec_start = p_vdec_start;
    p_hdl->g_npvr_FwMemCfg.vdec_mem_size = vdec_mem_len;
    p_hdl->g_npvr_FwMemCfg.p_audio_start = p_audio_mem_start;
    p_hdl->g_npvr_FwMemCfg.audio_mem_size = audio_mem_len;
    return;
}

/*
 *
 *   **************************THE FOLLOWING FOR SA FILL TS******************************************
 *
 ************************************************************************************
 *
 */

#define PER_CONSUME_SIZE      (188 * 1024)
#define SA_FILL_TS_BUFFER_SIZE (188 *28 * 1024)
typedef struct SA_ts_info {
    int inited;
    u32  vpid;
    u32  apid;
    u32  pcrpid;
    u32  vdec_type;
    u32  adec_type;
    int   pat_found;
    int   pmt_found;
    u32  pmtpid;
    u32 sync_offset;
    void * p_hdl;
    void * p_hdl2;
} SA_ts_info_t;
typedef struct SA_ts_handle {
    int bg_task_prio;
    int bg_task_size;
    u32 g_p_vdec_start;
    u32 g_vdec_mem_size;
    u32 g_p_audio_start;
    u32 g_audio_mem_size;
    SA_FILL_TS_MODE fill_mode;
    u32 r_pos;
    u32 w_pos;
    u32 buf_len;
    u32 buf_size;
    unsigned char * ts_buf;
    void * p_hdl;
} SA_ts_handle_t;

SA_ts_info_t g_sa_tsinfo;

void SA_fill_ts_task(void *);
extern u32 g_p_vdec_start;
extern u32 g_vdec_mem_size;
extern u32 g_p_audio_start;
extern u32 g_audio_mem_size;

extern void ts_detect_pmt(SA_ts_info_t * sa_tsinfo, unsigned char * packet, int len);

void SA_fill_ts_start(SA_FILL_TS_PARA * para)
{
    int ret = 0;
    NETWORK_PVR_HANDLER * p_hdl = NULL;
    SA_ts_handle_t * p_hdl2 = NULL;
    void (*p_fill_ts_task)(void * p_param) = NULL;
    p_fill_ts_task = SA_fill_ts_task;
    memset(&g_sa_tsinfo, 0, sizeof(SA_ts_info_t));
    g_sa_tsinfo.p_hdl = mtos_malloc(sizeof(NETWORK_PVR_HANDLER));
    g_sa_tsinfo.p_hdl2 = mtos_malloc(sizeof(SA_ts_handle_t));
    memset(g_sa_tsinfo.p_hdl, 0, sizeof(NETWORK_PVR_HANDLER));
    memset(g_sa_tsinfo.p_hdl2, 0, sizeof(SA_ts_handle_t));
    p_hdl = g_sa_tsinfo.p_hdl;
    p_hdl2 = g_sa_tsinfo.p_hdl2;
    ret  = mtos_sem_create((os_sem_t *) & (p_hdl->lock), TRUE);
    MT_ASSERT(ret == TRUE);
    p_hdl->npvr_ts_buffer = (u8 *)mtos_align_malloc(PER_CONSUME_SIZE + 188, 16);
    p_hdl->npvr_rcv_buffer = (u8 *)mtos_align_malloc(PER_CONSUME_SIZE, 16);
    p_hdl2->ts_buf = (u8 *)mtos_align_malloc(SA_FILL_TS_BUFFER_SIZE, 16);
    p_hdl2->buf_size = SA_FILL_TS_BUFFER_SIZE;

    if (para != NULL) {
        NetworkPVR_SetFwMem(p_hdl, para->p_vdec_start, para->vdec_mem_size,
                            para->p_audio_start, para->audio_mem_size);
        p_hdl->bg_task_prio = para->task_prio;
        p_hdl->bg_task_size = para->stack_size;
        OS_PRINTF("[%s]bg_task_prio[%d]\n", __func__, para->task_prio);
        OS_PRINTF("[%s]stack_size[%ld]\n", __func__, para->stack_size);
    } else {
        NetworkPVR_SetFwMem(p_hdl, 0xa4180000, 45613056,
                            0x83a00000, 1572864);
        p_hdl->bg_task_prio = 14;
        p_hdl->bg_task_size = 32 * 1024;
    }

    p_hdl->is_bg_task_live = FALSE;
    p_hdl2->fill_mode = SA_FILL_TS_START;
    p_hdl->p_bg_task_stack = mtos_malloc(p_hdl->bg_task_size);
    MT_ASSERT(p_hdl->p_bg_task_stack != NULL);
    memset(p_hdl->p_bg_task_stack, 0, p_hdl->bg_task_size);
    ret = mtos_task_create((u8 *)"SA_fill_ts_task", p_fill_ts_task, p_hdl,
                           p_hdl->bg_task_prio, p_hdl->p_bg_task_stack,
                           p_hdl->bg_task_size);

    while (p_hdl->is_bg_task_live == FALSE) {
        mtos_task_sleep(1);
        continue;
    }
}
void SA_fill_ts_stop()
{
    NETWORK_PVR_HANDLER * p_hdl = g_sa_tsinfo.p_hdl;
    SA_ts_handle_t * p_hdl2 = g_sa_tsinfo.p_hdl2;
    p_hdl2->fill_mode = SA_FILL_TS_STOP;

    while (1) {
        if (p_hdl->is_bg_task_live) {
            OS_PRINTF("[%s]wait 10ms ....\n", __func__);
            mtos_task_sleep(10);
        } else {
            break;
        }
    }

    NetworkPVR_SetFwMem(p_hdl, 0, 0, 0, 0);

    if (p_hdl->lock) {
        mtos_sem_destroy((os_sem_t *) & (p_hdl->lock), 0);
    }

    if (p_hdl->npvr_rcv_buffer) {
        mtos_free(p_hdl->npvr_rcv_buffer);
        p_hdl->npvr_rcv_buffer = NULL;
    }

    if (p_hdl->npvr_ts_buffer) {
        mtos_free(p_hdl->npvr_ts_buffer);
        p_hdl->npvr_ts_buffer = NULL;
    }

    if (p_hdl->p_bg_task_stack) {
        mtos_free(p_hdl->p_bg_task_stack);
        p_hdl->p_bg_task_stack = NULL;
    }

    if (p_hdl2->ts_buf) {
        mtos_free(p_hdl2->ts_buf);
        p_hdl2->ts_buf = NULL;
    }

    mtos_free(g_sa_tsinfo.p_hdl);
    mtos_free(g_sa_tsinfo.p_hdl2);
    memset(&g_sa_tsinfo, 0, sizeof(SA_ts_info_t));
}

int SA_get_data(unsigned char * buf, int len)
{
    int buf_len = 0;
    int total_len = len;
    NETWORK_PVR_HANDLER * p_hdl = g_sa_tsinfo.p_hdl;
    SA_ts_handle_t * p_hdl2 = g_sa_tsinfo.p_hdl2;

    //OS_PRINTF("[%s] start...\n",__func__);
    if (p_hdl2->buf_len == 0) {
        return 0;
    }

    mtos_sem_take((os_sem_t *)(&(p_hdl->lock)), 0);

    if (p_hdl2->buf_len <= total_len) {
        total_len = p_hdl2->buf_len;
    }

    if (p_hdl2->w_pos >= p_hdl2->r_pos) {
        memcpy(buf, p_hdl2->ts_buf + p_hdl2->r_pos, total_len);
        p_hdl2->r_pos += total_len;
        p_hdl2->buf_len -= total_len;

        if (p_hdl2->r_pos == p_hdl2->buf_size) {
            p_hdl2->r_pos = 0;
        }
    } else {
        if (p_hdl2->r_pos + total_len <= p_hdl2->buf_size) {
            memcpy(buf, p_hdl2->ts_buf + p_hdl2->r_pos, total_len);
            p_hdl2->buf_len -= total_len;
            p_hdl2->r_pos += total_len;

            if (p_hdl2->r_pos == p_hdl2->buf_size) {
                p_hdl2->r_pos = 0;
            }
        } else {
            p_hdl2->buf_len -= total_len;
            memcpy(buf, p_hdl2->ts_buf + p_hdl2->r_pos, p_hdl2->buf_size - p_hdl2->r_pos);
            memcpy(buf + p_hdl2->buf_size - p_hdl2->r_pos,
                   p_hdl2->ts_buf, total_len - (p_hdl2->buf_size - p_hdl2->r_pos));
            p_hdl2->r_pos = total_len - (p_hdl2->buf_size - p_hdl2->r_pos);
        }
    }

    //OS_PRINTF("[%s] getlen[%d],buf_len[%d]\n",__func__,total_len,p_hdl2->buf_len);
    mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
    return total_len;
}

void SA_fill_ts_task(void * p_para)
{
#ifndef __LINUX__
    u32 videoFreeSpaceSize = 0, audFreeSpaceSize = 0;
    BOOL state = FALSE;
    int total_len = 0;
    int gop_len = 0;
    int pti_dma_len = 188 * 200;
    int temp_len = 0;
    int pos_left = 0;
    unsigned char * packet;
    dmx_chanid_t video_chan = 0xffff;
    dmx_chanid_t audio_chan = 0xffff;
    dmx_chanid_t pcr_chan = 0xffff;
    dmx_dma_config_t dma_config;
    dmx_play_setting_t play_para;
    int ret = 0;
    int size = 0;
    int align = 0;
    ufs_file_t * p_file = NULL;
    unsigned char * p_buf = NULL;
    u32 freespace = 0;
    u32 read_bytes = 0;
    u32 read_size = 0;
    int first_time = 1;
    NETWORK_PVR_HANDLER * p_hdl = NULL;
    SA_ts_handle_t * p_hdl2 = g_sa_tsinfo.p_hdl2;
    p_hdl = g_sa_tsinfo.p_hdl;
    p_hdl2->fill_mode = SA_FILL_TS_PLAY;
    /* pti */
    {
        p_hdl->p_pti_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

        if (p_hdl->p_pti_dev == NULL) {
            OS_PRINTF("[%s]p_pti_dev[0x%x] ...!!!\n", __func__, p_hdl->p_pti_dev);
            goto ERROR_HDL;
        }
    }
    /* display */
    {
        p_hdl->p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);

        if (p_hdl->p_disp_dev == NULL) {
            OS_PRINTF("[%s]p_disp_dev[0x%x] ...!!!\n", __func__, p_hdl->p_disp_dev);
            goto ERROR_HDL;
        }
    }
    /* vdec */
    {
        p_hdl->p_vdec_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);

        if (p_hdl->p_vdec_dev == NULL) {
            OS_PRINTF("[%s]p_vdec_dev[0x%x] ...!!!\n", __func__, p_hdl->p_vdec_dev);
            goto ERROR_HDL;
        }

        ret = dev_open(p_hdl->p_vdec_dev, NULL);
        MT_ASSERT(SUCCESS == ret);
    }
    /* audio */
    {
        p_hdl->p_audio_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_AUDIO);

        if (p_hdl->p_audio_dev == NULL) {
            OS_PRINTF("[%s]p_audio_dev[0x%x] ...!!!\n", __func__, p_hdl->p_audio_dev);
            goto ERROR_HDL;
        }

        ret = dev_open(p_hdl->p_audio_dev, NULL);
        MT_ASSERT(SUCCESS == ret);
    }

    if (p_hdl->g_npvr_FwMemCfg.audio_mem_size == 0
        || p_hdl->g_npvr_FwMemCfg.vdec_mem_size == 0
        || p_hdl->g_npvr_FwMemCfg.p_vdec_start == 0
        || p_hdl->g_npvr_FwMemCfg.p_audio_start == 0) {
        OS_PRINTF("[%s] audio_mem_size[%ld]\n", __func__, p_hdl->g_npvr_FwMemCfg.audio_mem_size);
        OS_PRINTF("[%s] vdec_mem_size[%ld]\n", __func__, p_hdl->g_npvr_FwMemCfg.vdec_mem_size);
        OS_PRINTF("[%s] p_audio_start[%x]\n", __func__, p_hdl->g_npvr_FwMemCfg.p_audio_start);
        OS_PRINTF("[%s] p_vdec_start[%x]\n", __func__, p_hdl->g_npvr_FwMemCfg.p_vdec_start);
    }

    OS_PRINTF("[%s] line [%d]\n", __func__, __LINE__);
    p_hdl->is_bg_task_live = TRUE;

    while (1) {
        if (p_hdl2->fill_mode == SA_FILL_TS_STOP) {
            break;
        }

        read_size = SA_get_data(p_hdl->npvr_rcv_buffer, 188 * 1024);

        if (read_size == 0) {
            mtos_task_sleep(5);
            continue;
        }

        if (!g_sa_tsinfo.inited) {
            ts_detect_pmt(&g_sa_tsinfo, p_hdl->npvr_rcv_buffer, read_size);
            OS_PRINTF("[%s]detect result:vpid[%d],vtype[%d],apid[%d],atype[%d]\n", __func__,
                      g_sa_tsinfo.vpid, g_sa_tsinfo.vdec_type, g_sa_tsinfo.apid, g_sa_tsinfo.adec_type);

            if (g_sa_tsinfo.inited) {
                p_hdl->video_pid = g_sa_tsinfo.vpid;
                p_hdl->audio_pid = g_sa_tsinfo.apid;
                p_hdl->pcr_pid = g_sa_tsinfo.pcrpid;
                p_hdl->vdec_type = g_sa_tsinfo.vdec_type;
                p_hdl->adec_type = g_sa_tsinfo.adec_type;
            } else {
                continue;
            }

            size = 0;
            align = 0;
            aud_get_buf_requirement_vsb(p_hdl->p_audio_dev, (u32 *)&size, (u32 *)&align);

            if (p_hdl->g_npvr_FwMemCfg.audio_mem_size < size) {
                OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
            }

            aud_set_buf_vsb(p_hdl->p_audio_dev, (p_hdl->g_npvr_FwMemCfg.p_audio_start), size);

            if (p_hdl->audio_pid != 0x1fff) {
                aud_start_vsb(p_hdl->p_audio_dev, p_hdl->adec_type, AUDIO_ES_FILE);
            }

            OS_PRINTF("[%s] NETWORK_PVR_STARTCODE found 5!!!\n", __func__);

            if (p_hdl->video_pid != 0x1fff) {
                size = 0;
                align = 0;
                vdec_get_buf_requirement(p_hdl->p_vdec_dev, VDEC_BUFFER_AD, (u32 *)&size, (u32 *)&align);

                if (p_hdl->g_npvr_FwMemCfg.vdec_mem_size < size) {
                    OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                }

                vdec_set_buf(p_hdl->p_vdec_dev, VDEC_BUFFER_AD, p_hdl->g_npvr_FwMemCfg.p_vdec_start);
                ret = vdec_start(p_hdl->p_vdec_dev, p_hdl->vdec_type, 2);

                if (SUCCESS != ret) {
                    OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                }
            }

            if (p_hdl->audio_pid != 0x1fff) {
                play_para.pid = p_hdl->audio_pid;
                play_para.type = DMX_AUDIO_TYPE;
                play_para.stream_in = DMX_INPUT_EXTERN_DMA;
                ret = dmx_play_chan_open(p_hdl->p_pti_dev, &play_para, &audio_chan);

                if (ret != SUCCESS) {
                    OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                }
            }

            OS_PRINTF("[%s] NETWORK_PVR_STARTCODE found 1!!!\n", __func__);

            if (p_hdl->video_pid != 0x1fff) {
                play_para.pid = p_hdl->video_pid;
                play_para.type = DMX_VIDEO_TYPE;
                play_para.stream_in = DMX_INPUT_EXTERN_DMA;
                ret = dmx_play_chan_open(p_hdl->p_pti_dev, &play_para, &video_chan);

                if (ret != SUCCESS) {
                    OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                }
            }

            OS_PRINTF("[%s] NETWORK_PVR_STARTCODE found 2!!!\n", __func__);

            if (p_hdl->pcr_pid != 0x1fff) {
                play_para.pid = p_hdl->pcr_pid;
                play_para.type = DMX_PCR_TYPE;
                play_para.stream_in = DMX_INPUT_EXTERN_DMA;
                ret = dmx_play_chan_open(p_hdl->p_pti_dev, &play_para, &pcr_chan);

                if (ret != SUCCESS) {
                    OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                }
            }

            if (video_chan != 0xffff) {
                ret = dmx_chan_start(p_hdl->p_pti_dev, video_chan);

                if (ret != SUCCESS) {
                    OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                }
            }

            if (audio_chan != 0xffff) {
                ret = dmx_chan_start(p_hdl->p_pti_dev, audio_chan);

                if (ret != SUCCESS) {
                    OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                }
            }

            if (pcr_chan != 0xffff) {
                ret = dmx_chan_start(p_hdl->p_pti_dev, pcr_chan);

                if (ret != SUCCESS) {
                    OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
                }
            }

            //dmx_av_reset(p_hdl->p_pti_dev);
            OS_PRINTF("[%s] NETWORK_PVR_STARTCODE found 6!!!\n", __func__);
        }

        memcpy(p_hdl->npvr_ts_buffer + pos_left, p_hdl->npvr_rcv_buffer, read_size);
        total_len = pos_left + read_size;
        pos_left = total_len % 188;
        gop_len = total_len - pos_left;
        temp_len = gop_len;
        dma_config.mem_address = (u32) p_hdl->npvr_ts_buffer;

        while (temp_len) {
            vdec_get_es_buf_space(p_hdl->p_vdec_dev, &videoFreeSpaceSize);

            if (videoFreeSpaceSize < 500) {
                mtos_task_sleep(5);
                continue;
            }

            aud_file_getleftesbuffer_vsb(p_hdl->p_audio_dev, &audFreeSpaceSize);

            if (audFreeSpaceSize < 30 * 1024) {
                mtos_task_sleep(1);
                continue;
            }

            if (temp_len >= pti_dma_len) {
                dma_config.data_length = pti_dma_len;
            } else {
                dma_config.data_length = temp_len;
            }

            temp_len -= dma_config.data_length;
            dma_config.ts_clk = 40000000;
            dmx_set_dma_config(p_hdl->p_pti_dev, &dma_config);

            while (1) {
                state = dmx_get_dma_state(p_hdl->p_pti_dev);

                if (state == TRUE) {
                    break;
                }

                mtos_task_sleep(1);
            }

            if (first_time) {
                mtos_task_sleep(1000);
                first_time = 0;
            }

            dma_config.mem_address += dma_config.data_length;
        }

        if (pos_left) {
            memcpy(p_hdl->npvr_rcv_buffer, p_hdl->npvr_ts_buffer + gop_len, pos_left);
            memcpy(p_hdl->npvr_ts_buffer, p_hdl->npvr_rcv_buffer, pos_left);
        }
    }

    if (p_hdl->video_pid != 0x1fff) {
        vdec_stop(p_hdl->p_vdec_dev);
    }

    if (p_hdl->audio_pid != 0x1fff) {
        aud_stop_vsb(p_hdl->p_audio_dev);
    }

    if (audio_chan != 0xffff) {
        ret = dmx_chan_stop(p_hdl->p_pti_dev, audio_chan);

        if (ret != SUCCESS) {
            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
        }

        ret = dmx_chan_close(p_hdl->p_pti_dev, audio_chan);

        if (ret != SUCCESS) {
            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
        }
    }

    if (video_chan != 0xffff) {
        ret = dmx_chan_stop(p_hdl->p_pti_dev, video_chan);

        if (ret != SUCCESS) {
            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
        }

        ret = dmx_chan_close(p_hdl->p_pti_dev, video_chan);

        if (ret != SUCCESS) {
            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
        }
    }

    if (pcr_chan != 0xffff) {
        ret = dmx_chan_stop(p_hdl->p_pti_dev, pcr_chan);

        if (ret != SUCCESS) {
            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
        }

        ret = dmx_chan_close(p_hdl->p_pti_dev, pcr_chan);

        if (ret != SUCCESS) {
            OS_PRINTF("[%s]line[%d][ERROR] ret:%d !!!!!\n", __func__, __LINE__, ret);
        }
    }

ERROR_HDL:
    //NetworkPVR_SetFwMem(p_hdl,0,0,0,0);
    dmx_av_reset(p_hdl->p_pti_dev);
    p_hdl->is_bg_task_live = FALSE;
    p_hdl->ExitOK = TRUE;
    OS_PRINTF("[%s]end end ...!!!\n", __func__);
    mtos_task_delete(p_hdl->bg_task_prio);
#endif
}

void SA_fill_ts_data(unsigned char * buf, int len)
{
    int buf_len = 0;
    int total_len = len;
    NETWORK_PVR_HANDLER * p_hdl = g_sa_tsinfo.p_hdl;
    SA_ts_handle_t * p_hdl2 = g_sa_tsinfo.p_hdl2;

    while (1) {
        mtos_sem_take((os_sem_t *)(&(p_hdl->lock)), 0);
        buf_len = p_hdl2->buf_len;

        if (buf_len + total_len >= p_hdl2->buf_size) {
            mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
            mtos_task_sleep(30);
            continue;
        }

        mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
        break;
    }

    mtos_sem_take((os_sem_t *)(&(p_hdl->lock)), 0);

    while (total_len) {
        if (p_hdl2->w_pos >= p_hdl2->r_pos) {
            if (p_hdl2->w_pos + total_len <= p_hdl2->buf_size) {
                memcpy(p_hdl2->ts_buf + p_hdl2->w_pos, buf, total_len);
                p_hdl2->w_pos += total_len;
                p_hdl2->buf_len += total_len;

                if (p_hdl2->w_pos == p_hdl2->buf_size) {
                    p_hdl2->w_pos = 0;
                }

                total_len = 0;
                break;
            } else {
                p_hdl2->buf_len += total_len;
                memcpy(p_hdl2->ts_buf + p_hdl2->w_pos, buf, p_hdl2->buf_size - p_hdl2->w_pos);
                total_len -= p_hdl2->buf_size - p_hdl2->w_pos;
                memcpy(p_hdl2->ts_buf, buf + p_hdl2->buf_size - p_hdl2->w_pos, total_len);
                p_hdl2->w_pos = total_len;
                total_len = 0;
                break;
            }
        } else {
            p_hdl2->buf_len += total_len;
            memcpy(p_hdl2->ts_buf + p_hdl2->w_pos, buf, total_len);
            p_hdl2->w_pos += total_len;
            total_len = 0;
            break;
        }
    }

    //OS_PRINTF("[%s] filllen[%d],buf_len[%d]\n",__func__,len,p_hdl2->buf_len);
    mtos_sem_give((os_sem_t *)(&(p_hdl->lock)));
    return;
}
void fill_ts_data(unsigned char * file_path)
{
#ifndef __LINUX__
    ufs_file_t * p_file = NULL;
    unsigned char * p_buf = mtos_malloc(188 * 1024);
    u32 read_size = 0;
    u16           file11[256] = {0};
    char          file22[256] = "C:\\";
    OS_PRINTF("[%s]start start\n", __func__);
    SA_fill_ts_start(NULL);
    strcat(file22, file_path);
    ufs_asc2uni(file22, file11);
    OS_PRINTF("[%s]line[%d]\n", __func__, __LINE__);
    p_file = mtos_malloc(sizeof(ufs_file_t));

    if (NULL == p_file) {
        OS_PRINTF("###opening file file struct malloc failed\n\n");
        MT_ASSERT(0);
    }

    memset(p_file, 0, sizeof(ufs_file_t));
    OS_PRINTF("ufs_open start!\n");
    OS_PRINTF("[%s]!\n", file22);
    ufs_open(p_file, file11, UFS_READ);
    OS_PRINTF("ufs_open end!\n");

    if (NULL == p_file) {
        OS_PRINTF("File not exit, quit!\n");
        return;
    }

    while (1) {
        ufs_read(p_file, p_buf, 188 * 1024, &read_size);

        if (read_size <= 0) {
            mtos_task_sleep(15000);
            break;
        }

        SA_fill_ts_data(p_buf, read_size);
    }

    SA_fill_ts_stop();
    mtos_free(p_buf);
    ufs_close(p_file);
    return;
#endif
}

/*!
  SA transcode ts parameter
 */
typedef struct {
    /*!
      xxxxxxxx
     */
    u16 video_pid;
    /*!
      xxxxxxxx
     */
    u16 audio_pid;
    /*!
      xxxxxxxx
     */
    u16 pcr_pid;
    /*!
      xxxxxxxx
     */
    u16 pmt_pid[1];//=<16 pmt_pid<=0x1ffe
    /*!
      must be type of dmx_input_type_t
     */
    u32 stream_in;

    /*!
      xxxxxxxx
     */
    u32  vdec_type;

    /*!
      xxxxxxxx
     */
    u32  adec_type;
    /*!
      xxxxxxxx
     */
    u32  psi_interval_num;//pmt frequency set by bitrate,default set as 20
    /*!
      xxxxxxxx
     */
    u32  psi_len;//pmt table length
    /*!
      xxxxxxxx
     */
    void * psi_buf;//pmt table

    /*!
      buffer to store transfered result
     */
    void * trans_buf;
    /*!
      use default buffer size
     */
    u32 trans_buf_size;

    /*!
      transfered length
     */
    u32 trans_buf_len;
    /*!
      < psi_last_interval_num
     */
    u32 last_left_pos;

} SA_TRANS_TS_HANDLE_t;
SA_TRANS_TS_HANDLE_t * g_sa_trans_hdl = NULL;
#define SA_TRANSBUF_DEFAULT_SIZE (188 * 10 *1024)
void * SA_trans_ts_init(SA_TRANS_TS_PARA * para)
{
    SA_TRANS_TS_HANDLE_t * p_hdl = mtos_malloc(sizeof(SA_TRANS_TS_HANDLE_t));
    memset(p_hdl, 0, sizeof(SA_TRANS_TS_HANDLE_t));
    g_sa_trans_hdl = p_hdl;
    p_hdl->audio_pid = para->audio_pid;
    p_hdl->video_pid = para->video_pid;
    p_hdl->pcr_pid = para->pcr_pid;
    p_hdl->stream_in = para->stream_in;
    p_hdl->adec_type = para->adec_type;
    p_hdl->vdec_type = para->vdec_type;
    p_hdl->pmt_pid[0] = para->pmt_pid;
    p_hdl->psi_interval_num = para->psi_interval_num ? para->psi_interval_num : 20;
    p_hdl->psi_buf = mtos_malloc(TS_PACKET_SIZE * 2);
    memset(p_hdl->psi_buf, 0, sizeof(TS_PACKET_SIZE * 2));
    write_pat(p_hdl->pmt_pid, 1, p_hdl->psi_buf);
    write_pmt(p_hdl->pmt_pid[0], p_hdl->video_pid, p_hdl->vdec_type,
              p_hdl->audio_pid, p_hdl->adec_type, p_hdl->pcr_pid, p_hdl->psi_buf + TS_PACKET_SIZE);
    p_hdl->psi_len = TS_PACKET_SIZE * 2;
    p_hdl->trans_buf = mtos_malloc(SA_TRANSBUF_DEFAULT_SIZE);
    p_hdl->trans_buf_size = SA_TRANSBUF_DEFAULT_SIZE;
    p_hdl->trans_buf_len = 0;
    p_hdl->last_left_pos = 0;
    return p_hdl;
}

unsigned char * SA_trans_ts_getpmt(void * hdl, int * len)
{
    SA_TRANS_TS_HANDLE_t * p_hdl = (SA_TRANS_TS_HANDLE_t *)hdl;
    OS_PRINTF("[%s]line[%d]\n", __func__, __LINE__);

    if (p_hdl != NULL && g_sa_trans_hdl == p_hdl) {
        *len = TS_PACKET_SIZE * 2;
        return p_hdl->psi_buf;
    }

    *len = 0;
    OS_PRINTF("[%s]line[%d]\n", __func__, __LINE__);
    return NULL;
}

unsigned char * SA_trans_ts_data(void * hdl, void * src, int src_size, void * buf, int * dst_size)
{
    SA_TRANS_TS_HANDLE_t * p_hdl = (SA_TRANS_TS_HANDLE_t *)hdl;
    void * p_src = src;
    void * p_dst = p_hdl->trans_buf;
    u32 p_src_pos = 0;
    u32 p_dst_pos = 0;
    u32 pos = 0, size = 0, len = src_size;
    u32 psi_interval_num = p_hdl->psi_interval_num;
    u32 left_len = (psi_interval_num - p_hdl->last_left_pos) * 188;
    *dst_size = 0;

    if (src_size == 0) {
        mtos_printk("[%s]src_size is 0!!!\n", __func__);
        *dst_size = 0;
        return p_dst;
    }

    if (p_hdl->last_left_pos) {
        if (left_len <= len) {
            memcpy(p_dst, p_src, left_len);
            p_hdl->last_left_pos = 0;
            p_src_pos += left_len;
            memcpy(p_dst + left_len, p_hdl->psi_buf, 188 * 2);
            p_dst_pos += left_len + 188 * 2;
            pos = left_len;
            *dst_size += left_len + 188 * 2;
        } else {
            memcpy(p_dst, p_src, len);
            p_hdl->last_left_pos += len / 188;
            p_src_pos += len;
            pos = len;
            p_dst_pos += len;
            *dst_size += len;
        }
    }

    for (; pos < len; pos += 188 * psi_interval_num) {
        size = (pos + 188 * psi_interval_num) > len ? (len - pos) : 188 * psi_interval_num;
        memcpy(p_dst + p_dst_pos, p_src + p_src_pos, size);
        p_src_pos += size;
        p_dst_pos += size;

        if (size >= 188 * psi_interval_num) {
            memcpy(p_dst + p_dst_pos, p_hdl->psi_buf, 188 * 2);
            p_dst_pos += 188 * 2;
            *dst_size += size + 188 * 2;
        } else {
            *dst_size += size;
            p_hdl->last_left_pos = size / 188;
        }
    }

    return p_dst;
}

void SA_trans_ts_deinit(void * hdl)
{
    SA_TRANS_TS_HANDLE_t * p_hdl = (SA_TRANS_TS_HANDLE_t *)hdl;

    if (p_hdl) {
        if (p_hdl->psi_buf) {
            mtos_free(p_hdl->psi_buf);
        }

        if (p_hdl->trans_buf) {
            mtos_free(p_hdl->trans_buf);
        }

        mtos_free(p_hdl);
    }

    g_sa_trans_hdl = NULL;
}
void fill_pmt_data(unsigned char * file_path)
{
#ifndef __LINUX__
    SA_TRANS_TS_HANDLE_t * p_hdl = NULL;
    SA_TRANS_TS_PARA param;
    ufs_file_t * p_file = NULL;
    ufs_file_t * p_wfile = NULL;
    unsigned char * p_buf = mtos_malloc(188 * 1024);
    unsigned char * p_w_buf = mtos_malloc(188 * 1096);
    unsigned char * p_tmp = NULL;
    u32 read_size = 0;
    u32 write_size = 0;
    u32 ret_size = 0;
    u16           file11[256] = {0};
    char          file22[256] = "C:\\";
    char          file33[256] = "C:\\w_";
    OS_PRINTF("[%s]start start\n", __func__);
    strcat(file22, file_path);
    ufs_asc2uni(file22, file11);
    OS_PRINTF("[%s]line[%d]\n", __func__, __LINE__);
    p_file = mtos_malloc(sizeof(ufs_file_t));

    if (NULL == p_file) {
        OS_PRINTF("###opening file file struct malloc failed\n\n");
        MT_ASSERT(0);
    }

    memset(p_file, 0, sizeof(ufs_file_t));
    p_wfile = mtos_malloc(sizeof(ufs_file_t));

    if (NULL == p_wfile) {
        OS_PRINTF("###opening file file struct malloc failed\n\n");
        MT_ASSERT(0);
    }

    memset(p_wfile, 0, sizeof(ufs_file_t));
    OS_PRINTF("ufs_open start!\n");
    OS_PRINTF("[%s]!\n", file22);
    ufs_open(p_file, file11, UFS_READ);
    strcat(file33, file_path);
    ufs_asc2uni(file33, file11);
    ufs_open(p_wfile, file11, UFS_READ | UFS_WRITE | UFS_CREATE_NEW_COVER);
    OS_PRINTF("ufs_open end!\n");

    if (NULL == p_file) {
        OS_PRINTF("File not exit, quit!\n");
        return;
    }

    if (NULL == p_wfile) {
        OS_PRINTF("File not exit, quit!\n");
        return;
    }

    memset(&param, 0x00, sizeof(param));
    param.audio_pid = 68;
    param.video_pid = 69;
    param.pmt_pid = 66;
    param.pcr_pid = 69;
    param.stream_in = DMX_INPUT_EXTERN0;
    param.adec_type = 3;
    param.vdec_type = 0;
    param.psi_interval_num = 20;
    p_hdl = SA_trans_ts_init(&param);
    p_tmp = SA_trans_ts_getpmt(p_hdl, (int *)&write_size);
    ufs_write(p_wfile, p_tmp, write_size, &ret_size);
    OS_PRINTF("[%s]line[%d]\n", __func__, __LINE__);

    while (1) {
        ufs_read(p_file, p_buf, 188 * 1000, &read_size);

        if (read_size <= 0) {
            //mtos_task_sleep(15000);
            break;
        }

        p_tmp = SA_trans_ts_data(p_hdl, p_buf, read_size, p_w_buf, (int *)&write_size);
        ufs_write(p_wfile, p_tmp, write_size, &ret_size);
    }

    SA_trans_ts_deinit(p_hdl);
    mtos_free(p_buf);
    ufs_close(p_file);
    mtos_free(p_w_buf);
    ufs_close(p_wfile);
    return;
	#endif
}
#endif

#if 0

typedef unsigned char      cb_u8;
typedef unsigned short int cb_u16;
typedef unsigned int       cb_u32;
typedef signed char        cb_s8;
typedef signed short int   cb_s16;
typedef signed int         cb_s32;
typedef char               cb_char;

typedef enum {cb_false, cb_true} cb_bool;




typedef struct {
    cb_u8  *  bufptr;
    cb_u32    buflen;
    cb_u32    datalen;
    cb_u32    readpos;
    cb_u32    writepos;
    cb_u32  lock;
    cb_u8     type/*0:video 1:audio*/;
} CircleBufferMngr;

typedef struct {
    void (*cb_init)(CircleBufferMngr **, cb_u32);
    void (*cb_deinit)(CircleBufferMngr **);
    void (*cb_info)(CircleBufferMngr *, cb_char *, cb_u32);
    cb_u32(*cb_read)(CircleBufferMngr *, cb_u8 *, cb_u32);
    cb_u32(*cb_write)(CircleBufferMngr *, cb_u8 *, cb_u32);
    cb_u32(*cb_datalen)(CircleBufferMngr *);
    cb_bool(*cb_full)(CircleBufferMngr *);
    cb_bool(*cb_empty)(CircleBufferMngr *);
} CircleBufferApi;



#define CB_MEMCPY   memcpy
#define CB_MEMSET   memset
#define CB_SPRINT   sprintf
#define CB_MALLOC   mtos_malloc
#define CB_MFREE    mtos_free;
#define CB_ASSERT
#define CB_SNPRINT  snprintf
#define CB_TRACE    OS_PRINTF


//锁定, 禁止中断和任务调度
#define CB_GLOBAL_LOCK
#define CB_GLOBAL_UNLOCK

extern const CircleBufferApi circleBufApi;

#define CBMA_INIT     circleBufApi.cb_init
#define CBMA_DEINIT   circleBufApi.cb_deinit
#define CBMA_INFO     circleBufApi.cb_info
#define CBMA_READ     circleBufApi.cb_read
#define CBMA_WRITE    circleBufApi.cb_write
#define CBMA_DATALEN  circleBufApi.cb_datalen
#define CBMA_FULL     circleBufApi.cb_full
#define CBMA_EMPTY    circleBufApi.cb_empty


/*******************************************************************************
 ** 函数: cb_init
 ** 功能: 初始化
 ** 作者: avrbase_lei
 *******/
static void cb_init(CircleBufferMngr ** ppmngr, cb_u32 buflen);

/*******************************************************************************
 ** 函数: cb_deinit
 ** 功能: 资释? ** 作者: avrbase_lei
 *******/
static void cb_deinit(CircleBufferMngr ** ppmngr);

/*******************************************************************************
 ** 函数: cb_info
 ** 功能: 打印管理变量中的各种值到长度为buflen的outbuf中
 ** 作者: avrbase_lei
 *******/
static void cb_info(
    CircleBufferMngr * pmngr,
    int (*user_printf)(const char *, ...));

/*******************************************************************************
 ** 函数: cb_read
 ** 功能: 读取不超过buflen长度的数据給utbuf中, 其中outbuf的长度不得低于buflen
 ** 返? 返厥导识寥〉氖莩ざ? 字节为单位
 ** 说明: 如果传入的outbuf地址为NULL, 则直由境齜uflen长度的数据
 ** 作者: avrbase_lei
 *******/
static cb_u32 cb_read(
    CircleBufferMngr * pmngr,
    cb_u8 * outbuf,
    cb_u32 buflen);

/*******************************************************************************
 ** ? cb_write
 ** 功能: 将datptr赶蚰地址械膁atlen长度的数据写入到pmngr->bufptr中
 ** 返回: 返回实际写入的莩? 字节为单位
 ** 作者: avrbase_lei
 *******/
static cb_u32 cb_write(
    CircleBufferMngr * pmngr,
    cb_u8 * datptr,
    cb_u32 datlen);

/*******************************************************************************
 ** 函数: cb_datalen
 ** 功能: 查询pmngr->bufptr中的数据长度
 ** 返? 返回pmngr->bufptr数据ざ? 字节为单位
 ** 作者: avrbase_lei
 *******/
static cb_u32 cb_datalen(CircleBufferMngr * pmngr);

/*******************************************************************************
 ** 函数: cb_full
 ** 功能: 判断缓冲区是否已满
 ** 作? avrbase_lei
 *******/
static cb_bool cb_full(CircleBufferMngr * pmngr);

/*******************************************************************************
 ** 函数: cb_empty
 ** 功能: 判匣撼迩是否为空
 ** 作者: avrbase_lei
 *******/
static cb_bool cb_empty(CircleBufferMngr * pmngr);




/*******************************************************************************
 ** 函数: cb_init
 ** 功能: 初始化
 ** 作者: avrbase_lei
 *******/
static void cb_init(CircleBufferMngr ** ppmngr, cb_u32 buflen)
{
    CB_ASSERT(NULL != ppmngr);

    if (NULL != *ppmngr) {
        return;
    }

    if (0 == buflen) {
        return;
    }

    *ppmngr = (CircleBufferMngr *)malloc(sizeof(CircleBufferMngr));
    CB_ASSERT(NULL != *ppmngr);
    CB_MEMSET((void *)*ppmngr, 0, sizeof(CircleBufferMngr));
    (*ppmngr)->bufptr = (cb_u8 *)malloc(buflen);
    CB_ASSERT(NULL != (*ppmngr)->bufptr);
    (*ppmngr)->buflen = buflen;
    CB_MEMSET((void *)(*ppmngr)->bufptr, 0, buflen);
    mtos_sem_create(&((*ppmngr)->lock), TRUE);
}

/*******************************************************************************
 ** 函数: cb_deinit
 ** 功能: 资源释放
 ** 作者: avrbase_lei
 *******/
static void cb_deinit(CircleBufferMngr ** ppmngr)
{
    CB_ASSERT(NULL != ppmngr);

    if (NULL == *ppmngr) {
        return;
    }

    if (NULL != (*ppmngr)->bufptr) {
        free((*ppmngr)->bufptr);
        (*ppmngr)->bufptr = NULL;
        (*ppmngr)->buflen = 0;
    }

    free(*ppmngr);
    *ppmngr = NULL;
}

/*******************************************************************************
 ** 函数: cb_info
 ** 功能: 打印管理淞中母髦种档匠ざ任猙uflen膐utbuf中
 ** 作者: avrbase_lei
 *******/
static void cb_info(
    CircleBufferMngr * pmngr,
    int (*user_printf)(const char *, ...))
{
    CB_ASSERT(NULL != pmngr);
    user_printf(
        "datalen=%d,readpos=%d,writepos=%d.",
        pmngr->datalen,
        pmngr->readpos,
        pmngr->writepos);
}

/*******************************************************************************
 ** 函数: cb_read
 ** 功能: 读取怀琤uflen长度氖莸給utbuf中, outbuf的长不得低于buflen
 ** 返回: 返回实际寥〉数据长度, 字谖ノ?
 ** 说明: 如果氲膐utbuf地址为NULL, 蛑苯由境齜uflen长度氖?
 ** 作? avrbase_lei
 *******/
static cb_u32 cb_read(
    CircleBufferMngr * pmngr,
    cb_u8 * outbuf,
    cb_u32 buflen)
{
    cb_u32 readlen = 0, tmplen = 0;
    CB_ASSERT(NULL != pmngr);
    CB_ASSERT(NULL != pmngr->bufptr);

    if (cb_empty(pmngr)) {
        return 0;
    }

    CB_GLOBAL_LOCK;
    mtos_sem_take((os_sem_t *)(&(pmngr->lock)), 0);
    readlen = buflen > pmngr->datalen ? pmngr->datalen : buflen;
    tmplen = pmngr->buflen - pmngr->readpos;

    if (NULL != outbuf) {
        if (readlen <= tmplen) {
            CB_MEMCPY(
                (void *)outbuf,
                (void *)&pmngr->bufptr[pmngr->readpos],
                readlen);
        } else {
            CB_MEMCPY(
                (void *)outbuf,
                (void *)&pmngr->bufptr[pmngr->readpos],
                tmplen);
            CB_MEMCPY(
                (void *)&outbuf[tmplen],
                (void *)pmngr->bufptr,
                readlen - tmplen);
        }
    }

    pmngr->readpos = (pmngr->readpos + readlen) % pmngr->buflen;
    pmngr->datalen -= readlen;
    CB_GLOBAL_UNLOCK;
    mtos_sem_give((os_sem_t *)(&(pmngr->lock)));
    return readlen;
}

/*******************************************************************************
 ** 函数: cb_write
 ** 功能: 将datptr所指向的地址中的datlen长度的数据写入到pmngr->bufptr中
 ** 返? 返回实际写入的数据长度, 字节为ノ?
 ** 作者: avrbase_lei
 *******/
static cb_u32 cb_write(CircleBufferMngr * pmngr, cb_u8 * datptr, cb_u32 datlen)
{
    cb_u32 writelen = 0, tmplen = 0;
    MT_ASSERT(NULL != pmngr);
    MT_ASSERT(NULL != pmngr->bufptr);

    if (cb_full(pmngr)) {
        return 0;
    }

    CB_GLOBAL_LOCK;
    mtos_sem_take((os_sem_t *)(&(pmngr->lock)), 0);
    tmplen = pmngr->buflen - pmngr->datalen;
    writelen = tmplen > datlen ? datlen : tmplen;

    if (pmngr->writepos < pmngr->readpos) {
        CB_MEMCPY(
            (void *)&pmngr->bufptr[pmngr->writepos],
            (void *)datptr,
            writelen);
    } else {
        tmplen = pmngr->buflen - pmngr->writepos;

        if (writelen <= tmplen) {
            CB_MEMCPY(
                (void *)&pmngr->bufptr[pmngr->writepos],
                (void *)datptr,
                writelen);
        } else {
            CB_MEMCPY(
                (void *)&pmngr->bufptr[pmngr->writepos],
                (void *)datptr,
                tmplen);
            CB_MEMCPY(
                (void *)pmngr->bufptr,
                (void *)&datptr[tmplen],
                writelen - tmplen);
        }
    }

    pmngr->writepos = (pmngr->writepos + writelen) % pmngr->buflen;
    pmngr->datalen += writelen;
    mtos_sem_give((os_sem_t *)(&(pmngr->lock)));
    CB_GLOBAL_UNLOCK;
    return writelen;
}


/*
 *   the header needs 8 bytes.
 *
 *   end sync: 0xdd,0xdd
 *
 *  2 bytes: 0xaa,0xbb  (sync word)
 *  2 bytes: packet size
 *  4 bytes: pts
 *  n bytes: payload
 *
 */
static BOOL  cb_write_packet(CircleBufferMngr * pmngr, cb_u8 * datptr, cb_u32 datlen, cb_u32 pts)
{
    //return writelen;
}

/*******************************************************************************
 ** 函数: cb_datalen
 ** 功能: 查询pmngr->bufptr中的数据长度
 ** 返回: 返回pmngr->bufptr数据长度, 字节为单位
 ** 作者: avrbase_lei
 *******/
static cb_u32 cb_datalen(CircleBufferMngr * pmngr)
{
    CB_ASSERT(NULL != pmngr);
    return pmngr->datalen;
}

/*******************************************************************************
 ** 函数: cb_full
 ** 功能: 判断撼迩欠褚崖?
 ** 髡? avrbase_lei
 *******/
static cb_bool cb_full(CircleBufferMngr * pmngr)
{
    CB_ASSERT(NULL != pmngr);
    return (cb_bool)(pmngr->buflen == pmngr->datalen);
}

/*******************************************************************************
 ** 函数: cb_empty
 ** δ? 判断缓冲区否为?
 ** 作者: avrbase_lei
 *******/
static cb_bool cb_empty(CircleBufferMngr * pmngr)
{
    CB_ASSERT(NULL != pmngr);
    return (cb_bool)(0 == pmngr->datalen);
}

const CircleBufferApi circleBufApi = {
    cb_init,
    cb_deinit,
    cb_info,
    cb_read,
    cb_write,
    cb_datalen,
    cb_full,
    cb_empty,
};
#endif

#endif
#define CAMERA_BUFFER_LEN   256*1024
#define CAMERA_TMP_LEN   256*1024

int  set_camera_buffer(camera_buffer_t * handle, u8 * data, int len)
{
    if ((handle->buffer_len - handle->total_data) < len) {
        OS_PRINTF("camera buffer is full, %d %d %d\n", handle->buffer_len, handle->total_data, len);
        return -1;
    }

    handle->total_data += len;

    //ring buffer
    if ((handle->buffer_len - handle->write_pos) >= len) {
        memcpy(handle->buffer + handle->write_pos, data, len);
        handle->write_pos += len;

        if (handle->write_pos == handle->buffer_len) {
            handle->write_pos = 0;
        }
    } else { //ring buffer rollback
        int part_len = handle->buffer_len - handle->write_pos;
        memcpy(handle->buffer + handle->write_pos, data, part_len);
        memcpy(handle->buffer, data + part_len, len - part_len);
        handle->write_pos = len - part_len;
    }

    return 0;
}

static int get_camera_data(camera_buffer_t * handle, int len,  u8 * buffer)
{
    int len_kb = 0;
    int real_len;

    if (handle->total_data < len) {
        len_kb = handle->total_data / 1024;
    } else {
        len_kb = len / 1024;
    }

    if (len_kb < 16) {
        return -1;
    }

    real_len = len_kb * 1024;

    //data not rollback in ring buffer
    if ((handle->buffer_len - handle->read_pos) >= real_len) {
        memcpy(buffer, handle->buffer + handle->read_pos, real_len);
        handle->read_pos += real_len;

        if (handle->read_pos == handle->buffer_len) {
            handle->read_pos = 0;
        }
    } else {
        int data_part = handle->buffer_len - handle->read_pos;
        memcpy(buffer, handle->buffer + handle->read_pos, data_part);
        memcpy(buffer + data_part, handle->buffer, real_len - data_part);
        handle->read_pos = real_len - data_part;
    }

    handle->total_data -= real_len;
    return real_len;
}

static ufs_file_t * record_camera_fd;
static int camera_file_open(char * name)
{
    record_camera_fd = mtos_malloc(sizeof(ufs_file_t));
    MT_ASSERT(record_camera_fd != NULL);
    memset(record_camera_fd, 0x00, sizeof(ufs_file_t));
    unsigned short  path_tmp[256] = {0};
    unsigned short * p_filename1 = Convert_Utf8_To_Unicode((unsigned char *) name, path_tmp);

    if (NULL == p_filename1) {
        OS_PRINTF("unicode file name is null\n");
        MT_ASSERT(0);
    }

    int ret = ufs_open(record_camera_fd, p_filename1, UFS_READ);

    if (ret != FR_OK) {
        OS_PRINTF("UFS OPEN FAIL %d\n", ret);
        MT_ASSERT(0);
    }

    return 0;
}
static int camera_file_read(int fd, char * buffer, int buffer_len)
{
    int r = 0;
    ufs_read(record_camera_fd, buffer, 20 * 1024, (u32 *)&r);
    return r;
}

static int camera_file_close(int fd)
{
    ufs_close(record_camera_fd);
    mtos_free(record_camera_fd);
    return 0;
}


void fill_camera_es_task(void * p_para)
{
#ifdef WITH_TCPIP_PROTOCOL
    FILE_SEQ_T * p_file_seq = (FILE_SEQ_T *)p_para;
    u8 * camera_tmp_buffer;
    unsigned int videoFreeSpaceSize = 0;
    unsigned int push_ves_size = 0;
    int cur_video_packet_size = 0;
    camera_buffer_t buffer;
    u8 * data, data_tail;
    int data_len, data_tail_len, ret;
    u32 threhold, freespace;
    int (*backup_open_func)(char *);
    int (*backup_read_func)(int, char *, int);
    int (*backup_close_func)(int);

    backup_open_func = p_file_seq->camera_open;
    backup_read_func = p_file_seq->camera_read;
    backup_close_func = p_file_seq->camera_close;

    if (strstr(p_file_seq->m_path[0], RECORD_FILE_TAIL) != NULL) {
        p_file_seq->camera_open = camera_file_open;
        p_file_seq->camera_read = camera_file_read;
        p_file_seq->camera_close = camera_file_close;
    }

    p_file_seq->is_task_alive = TRUE;
    p_file_seq->m_play_state = FILE_SEQ_PLAY;
    OS_PRINTF("%s start\n", __func__);
    p_file_seq->ves_seg_buf = mtos_malloc(VES_SEG_LEN);
    MT_ASSERT(p_file_seq->ves_seg_buf != NULL);
    camera_tmp_buffer = mtos_malloc(CAMERA_TMP_LEN);
    MT_ASSERT(camera_tmp_buffer != NULL);
    memset(camera_tmp_buffer, 0x00, CAMERA_TMP_LEN);
    memset(&buffer, 0x00, sizeof(buffer));
    buffer.buffer_len = CAMERA_BUFFER_LEN;
    buffer.read_pos = 0;
    buffer.write_pos = 0;
    buffer.total_data = 0;
    buffer.buffer = mtos_malloc(CAMERA_BUFFER_LEN);
    MT_ASSERT(buffer.buffer != NULL);
    memset(buffer.buffer, 0x00, CAMERA_BUFFER_LEN);

    if (p_file_seq->camera_open) {
        p_file_seq->camera_fd = p_file_seq->camera_open(p_file_seq->m_path[0]);
    } else {
        mtos_printk("USB camera open function is not register\n");
        MT_ASSERT(0);
    }

    OS_PRINTF("%s start read data from camera\n", __func__);
    p_file_seq->m_tmp_ves_buf_pos = 0;

    while (p_file_seq->m_play_state  ==  FILE_SEQ_PLAY) {
        if (p_file_seq->camera_read) {
            cur_video_packet_size = p_file_seq->camera_read(p_file_seq->camera_fd,
                                    camera_tmp_buffer, CAMERA_TMP_LEN);

            if (cur_video_packet_size > 0) {
                set_camera_buffer(&buffer, camera_tmp_buffer, cur_video_packet_size);
            }

            //p_file_seq->p_v_pkt_start = camera_buffer;
        } else {
            cur_video_packet_size = 0;
        }

        /*******just test use***/
        if (cur_video_packet_size == 0) {
            //if play recorded files, check vdec es buffer

            //if play record file, when reading to file tail, break
            if (strstr(p_file_seq->m_path[0], RECORD_FILE_TAIL) != NULL) {
                OS_PRINTF("file %s is end, break\n", p_file_seq->m_path[0]);

                while (p_file_seq->m_play_state  !=  FILE_SEQ_EXIT) {
                    //  threhold = (VDEC_FW_VES_SIZE >> 10);
                    threhold = (p_file_seq->dec_cap.max_ves_num >> 10);
                    vdec_get_es_buf_space(p_file_seq->p_vdec_dev, &freespace);

                    if (threhold == freespace) {
                        p_file_seq->m_play_state  =  FILE_SEQ_EXIT;
                        break;
                    } else {
                        mtos_task_sleep(20);
                    }
                }
            } else {
                mtos_task_sleep(10);
            }

            continue;
        }

        if (cur_video_packet_size < 0) {
            p_file_seq->camera_close(p_file_seq->camera_fd);
            mtos_task_sleep(20);
            p_file_seq->camera_fd = p_file_seq->camera_open(p_file_seq->m_path[0]);
            buffer.read_pos = 0;
            buffer.write_pos = 0;
            buffer.total_data = 0;
            continue;
        }

        /***********/
        //get data from buffer
        ret = get_camera_data(&buffer, CAMERA_TMP_LEN, camera_tmp_buffer);

        if (ret < 0) {
            continue;
        }

        p_file_seq->p_v_pkt_start = camera_tmp_buffer;
        cur_video_packet_size = ret;
        //push data to vdec
        p_file_seq->left_v_pkt_bytes = cur_video_packet_size;

        while (p_file_seq->m_play_state  !=  FILE_SEQ_EXIT)
            //check vdec es buffer is suffient for this time
        {
            freespace = 0;
            threhold = 400; //400KB
            vdec_get_es_buf_space(p_file_seq->p_vdec_dev, &freespace);

            if (freespace >= threhold) {
                break;
            } else {
                mtos_task_sleep(20);
            }
        }

        do {
            /*
             *   length  of video es to be pushed should not be
             */
            if (p_file_seq->left_v_pkt_bytes > 0 && p_file_seq->left_v_pkt_bytes <= VIDEO_TMP_BUF_LEN) {
                push_ves_size = p_file_seq->left_v_pkt_bytes;
            } else {
                push_ves_size = VIDEO_TMP_BUF_LEN;
            }

            vdec_get_es_buf_space(p_file_seq->p_vdec_dev, (u32 *)&videoFreeSpaceSize);

            if (videoFreeSpaceSize > (VIDEO_ES_BUF_OVERFLOW_THRESHOLD + push_ves_size / VES_SEG_LEN + 1)) {
               // memcpy(p_file_seq->p_tmp_ves_buf, p_file_seq->p_v_pkt_start + (p_file_seq->m_tmp_ves_buf_pos), push_ves_size);
                vdec_dec_push_es(p_file_seq->p_vdec_dev, (u32)( p_file_seq->p_v_pkt_start + (p_file_seq->m_tmp_ves_buf_pos)), push_ves_size, p_file_seq->sys_vpts);
                p_file_seq->totalVesNum += push_ves_size;
                p_file_seq->left_v_pkt_bytes -= push_ves_size;
                p_file_seq->m_tmp_ves_buf_pos += push_ves_size;

                if (p_file_seq->left_v_pkt_bytes == 0) {
                    p_file_seq->isVideoBufferFull = FALSE;
                    p_file_seq->needNewVideoData = TRUE;
                    videoFreeSpaceSize = (videoFreeSpaceSize * VES_SEG_LEN) - push_ves_size;
                    p_file_seq->available_ves_bytes = p_file_seq->dec_cap.max_ves_num - videoFreeSpaceSize;
                    p_file_seq->m_tmp_ves_buf_pos = 0;
                    break;
                }
            } else {
                p_file_seq->isVideoBufferFull = TRUE;
                break;
            }
        } while (p_file_seq->left_v_pkt_bytes > 0);
    }

    vdec_stop(p_file_seq->p_vdec_dev);
    vdec_set_avsync_mode(p_file_seq->p_vdec_dev, VDEC_AVSYNC_DEFAULT_TS);
    mtos_free(buffer.buffer);
    mtos_free(camera_tmp_buffer);

    if (p_file_seq->camera_close) {
        p_file_seq->camera_close(p_file_seq->camera_fd);
        p_file_seq->camera_fd = -1;
    }

    //x_unmap_es_buffer(p_file_seq);
    mtos_free(p_file_seq->ves_seg_buf);
    p_file_seq->camera_open = backup_open_func;
    p_file_seq->camera_read = backup_read_func;
    p_file_seq->camera_close = backup_close_func;

    if ((NULL != p_file_seq->event_cb) && (p_file_seq->is_play_to_end != 1)) {
        FILE_SEQ_LOG("[%s] drv send FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT!!\n", __func__);
#ifdef SWITCH_DEBUG
        exit_ticks  = mtos_ticks_get() - exit_ticks;
        mtos_printk("\n@@@@@%s %d exit:%d\n", __func__, __LINE__, exit_ticks);
#else
        p_file_seq->event_cb(FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT, 0);
#endif
    }

    if (p_file_seq->is_play_to_end == 1) {
        if (NULL != p_file_seq->event_cb) {
#ifdef SWITCH_DEBUG
            exit_ticks  = mtos_ticks_get() - exit_ticks;
            mtos_printk("\n@@@@@%s %d exit:%d\n", __func__, __LINE__, exit_ticks);
#else
            FILE_SEQ_LOG("[%s] drv send FILE_PLAYBACK_SEQ_STOP!!\n", __func__);
            p_file_seq->event_cb(FILE_PLAYBACK_SEQ_STOP, 0);
#endif
        }
    }

    mtos_sem_take((os_sem_t *)(&(p_file_seq->lock)), 0);
    mtos_sem_give((os_sem_t *)(&(p_file_seq->lock)));
    p_file_seq->is_task_alive =  FALSE;
    OS_PRINTF("%s end!!!!\n", __func__);

    mtos_task_exit();

#endif
}


/*******************************************************************************
*
*
*
*           the following code only for test play speed
*
*                                              20140731  peacer
*
*
*
*
*
*
*
*
*
*
**********************************************************************************/



#if (!defined(__LINUX__))
#if 1
#include "charsto.h"

extern int ff_socket_nonblock(int socket, int enable);
extern BOOL hal_pm_reset(void);

static  s32 load_success = -1;
static  s32 stop_success = -1;

static u32  test_speed_event_callback(FILE_PLAY_EVENT_E event, u32 param)
{
    unsigned int vpts = 0;
    unsigned int  cur_hour = 0;
    unsigned int  cur_min  = 0;
    unsigned int  cur_sec = 0;

    switch (event) {
        case FILE_PLAYBACK_SEQ_GET_VPTS:
            vpts  =  param;   //millisecond
            cur_hour = (vpts / 1000) / 3600;
            cur_min = ((vpts / 1000) - (cur_hour * 3600)) / 60;
            cur_sec = (vpts / 1000) - (cur_hour * 3600) - cur_min * 60;
            OS_PRINTF("[%ld:%ld:%ld],  %ld ms\n", cur_hour, cur_min, cur_sec, (vpts));
            break;

        case FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR:
            load_success = FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR;
            OS_PRINTF("\n recv  FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR!!\n", __func__, __LINE__);
            break;

        case FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT:
            load_success = FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT;
            OS_PRINTF("\n recv  FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT!!\n", __func__, __LINE__);
            break;

        case FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS:
            load_success = FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS;
            OS_PRINTF("\n recv  FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS!!!\n", __func__);
            break;

        case FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT:
            stop_success = FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT ;
            OS_PRINTF("\n recv  FILE_PLAYBACK_SEQ_FILL_ES_TASK_EXIT!!!\n", __func__);
            break;

        case FILE_PLAYBACK_SEQ_STOP:
            OS_PRINTF("\n recv  FILE_PLAYBACK_SEQ_STOP [EOF]!!!\n", __func__);
            break;

        case FILE_PLAYBACK_UNSUPPORT_AUDIO:
            OS_PRINTF("\n recv  FILE_PLAYBACK_UNSUPPORT_AUDIO!!!\n", __func__);
            break;

        case FILE_PLAYBACK_UNSUPPORT_VIDEO:
            OS_PRINTF("\n recv  FILE_PLAYBACK_UNSUPPORT_VIDEO !!!\n", __func__);
            break;

        case FILE_PLAYBACK_CHECK_TRICKPLAY:
            OS_PRINTF("\n recv  FILE_PLAYBACK_CHECK_TRICKPLAY !!!\n", __func__);
            break;

        case FILE_PLAYBACK_UNSUPPORT_MEMORY:
            OS_PRINTF("\n recv  FILE_PLAYBACK_UNSUPPORT_MEMORY !!!\n", __func__);
            break;

        case FILE_PLAYBACK_UNSUPPORT_SEEK:
            OS_PRINTF("\n recv  FILE_PLAYBACK_UNSUPPORT_SEEK !!!\n", __func__);
            break;

        default :
            break;
    }

    return  0;
}

/*static char *  parse_socket_param(int * p_port)
{
    mtos_printk("[%s]==========start start ================\n", __func__);
	ufs_file_t  ufs_fp;
	char * server_ip = NULL;
	memset(&ufs_fp, 0, sizeof(ufs_file_t));
	unsigned short  path_tmp[256] = {0};
	u16 * path_unicode = Convert_Utf8_To_Unicode((unsigned char *)"net.txt", path_tmp);		
	u8 ret = ufs_open(&ufs_fp, path_tmp,  UFS_READ);
	if(FR_OK == ret)
	{
		u32 r = 0;
		char * tv_client_sort_str = NULL;
		u8 tmp_buf[128]={0};
		memset(tmp_buf,0,128);
		ufs_lseek(&ufs_fp, 0, 0);
		ufs_read(&ufs_fp, tmp_buf, 128, &r);
		OS_PRINTF("tmp_buf:%s   ret:%d\n",tmp_buf,r);
		ufs_close(&ufs_fp);

		char * p_head = NULL;
		char * p_tail = NULL;
		char * p_start = NULL;
		char  ip[16] = {0};
		char  port[8] = {0};
		
		int len = 0;
		p_head = strstr(tmp_buf,"<server_ip>");
		p_tail = strstr(tmp_buf,"</server_ip>");
		
		if( p_head && p_tail)
		{
			
			p_start = p_head + 11;
			len = p_tail - p_start;
			memset(ip,0,16);
			strncpy(ip,p_start,len);
			server_ip = ip;
			mtos_printk("[%s] ===%s\n",__func__,ip);
		}

		p_head = strstr(tmp_buf,"<server_port>");
		p_tail = strstr(tmp_buf,"</server_port>");
		if( p_head && p_tail)
		{
			
			p_start = p_head + 13;
			len = p_tail - p_start;
			memset(port,0,8);
			strncpy(port,p_start,len);
			int tmp_port = atoi(port);
			*p_port = tmp_port;
			mtos_printk("[%s] ===%d\n",__func__,tmp_port);
		}
		

	}

    mtos_printk("[%s]====%s  :  %d=============\n", __func__,server_ip,*p_port);
    mtos_printk("[%s]==========end end================\n", __func__);
	return server_ip;

}
*/
////yiyuan move this to commondata 2014 10 08
//#define   LIVE_UPGRADE_TEST_SPEED_URL            "http://58.246.7.134:1500/upgradeimages/flash.bin"
#define   LIVE_UPGRADE_RAM_FILE                "r:testspeed_upgrade.file"
#define UPG_BURN_PACKET_SIZE 	(64 * (KBYTES))
//#define FLASH_TOTAL_SIZE       0x1000000
#define FLASH_TOTAL_SIZE       0x800000
/*
* erase flash
*/
static RET_CODE charsto_erase_testspeed(charsto_device_t * p_dev, u32 addr, u32 sec_cnt)
{

  RET_CODE ret = ERR_FAILURE;
  charsto_prot_status_t st_old = {0};
  charsto_prot_status_t st_set = {0};

  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);
  
  //unprotect
  st_set.prt_t = PRT_UNPROT_ALL;
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);

  ret = charsto_erase(p_dev, addr, sec_cnt);

  //restore
  dev_io_ctrl(p_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);

  return ret;
}
/*
* write flash
*/
static RET_CODE charsto_writeonly_testspeed(charsto_device_t * p_dev, u32 addr, u8 * p_buf, u32 len)
{
   RET_CODE ret  = ERR_FAILURE;
   charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_CHARSTO);

  charsto_prot_status_t st_old = {0};
  charsto_prot_status_t st_set = {0};
  
  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);
  //unprotect
  st_set.prt_t = PRT_UNPROT_ALL;
  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);
  
  ret = charsto_writeonly(p_charsto_dev, addr, p_buf, len);
  
  //restore
  dev_io_ctrl(p_charsto_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);
  return ret;    
}

RET_CODE upgradeFlashImage(u8 *file_buf, int file_len,int addr_offset)
{

   RET_CODE ret_erase  = ERR_FAILURE,ret_write =ERR_FAILURE ,ret_1= ERR_FAILURE ;
   charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_CHARSTO);

  charsto_prot_status_t st_old = {0};
  charsto_prot_status_t st_set = {0};
  u32 aligned_addr = 0;
  u32 sector_num = 0;

  aligned_addr = addr_offset & (~ (u32)(UPG_BURN_PACKET_SIZE - 1));
  sector_num = file_len/CHARSTO_SECTOR_SIZE;
  if((file_len % CHARSTO_SECTOR_SIZE) > 0 )
  {
          sector_num++;
  }
   FILE_SEQ_DEBUG("\n [%s ] aligned_addr = 0x%x  file_buf = 0x%x file_len = %d\n", __func__,aligned_addr,file_buf,file_len);
   if(!((aligned_addr < FLASH_TOTAL_SIZE)
       && (file_len > 0) && (file_len <= (FLASH_TOTAL_SIZE - aligned_addr))
       && (sector_num <= (FLASH_TOTAL_SIZE / CHARSTO_SECTOR_SIZE))))
  {
    mtos_printk(" \n [%s] : %d Wrong  parameter!\n",__func__,__LINE__);
    return ERR_FAILURE;
  }
   
  ret_erase = charsto_erase_testspeed(p_charsto_dev, aligned_addr, sector_num);
  
  if (ret_erase != SUCCESS)
  {
    mtos_printk("\n [%s ] charsto_erase [0x%x] ret[%d] failed !!\n", __func__,aligned_addr, ret_erase);
    return ret_erase;
  }
  
  mtos_task_sleep(50);
   FILE_SEQ_DEBUG("\n [%s ] charsto_writeonly_testspeed   aligned_addr = 0x%x  file_buf = 0x%x file_len = %d\n", __func__,aligned_addr,file_buf,file_len);
  ret_write = charsto_writeonly_testspeed(p_charsto_dev, (u32)addr_offset, file_buf, (u32)file_len);
  if (ret_write != SUCCESS)
  {
    mtos_printk("\n [%s] charsto_writeonly [0x%x, %lu] ret[%d] failed!!!\n", __func__,addr_offset, file_len, ret_write);
  }
  
#if 0 // check whether write OK,  just for deubg
u8 *rd_buf = NULL;
u8 *wr_buf = NULL;
int iii = 0;
mtos_task_sleep(50);
 
{
    rd_buf = mtos_malloc(file_len);
    memset(rd_buf, 0x0, file_len);
    mtos_printk("\n\n W(3)R addr[%x]len[%x]\n", addr_offset, file_len);
    ret_1= charsto_read(p_charsto_dev, addr_offset, rd_buf, file_len);
    if(SUCCESS != ret_1)
    {
      mtos_printk("flash read error\n");
      return;
    }
  wr_buf = mtos_malloc(file_len);
  memset(wr_buf, 0, file_len);
  
    for(iii=0; iii<16; iii++)
    {
        mtos_printk("flash read iii = %d rd %x \n", iii, rd_buf[iii]);
    }
	
	mtos_free(rd_buf);
	mtos_free(wr_buf);
	rd_buf = NULL;
	wr_buf = NULL;
 }

 #endif
  return ret_write;    

}


int get_valid_priority();
static BOOL stop_testspeed_flag = FALSE;
//#define   CHECK_INTERVAL   (3*60)
void  check_loop(void * p_param)
{
#ifndef __LINUX__
	mtos_printk("[%s] == start start ===\n",__func__);
	loop_check_t * check_param = (loop_check_t *)p_param;
	u32 cnt = 0;

	//check the global  flag 'is_loop_ok' in every three minutes
	while(!stop_testspeed_flag)
	{
		if( (++cnt) == check_param->check_interval)
		{
			mtos_printk("[%s] check wether  loop is alive  !!!!!!!!1\n",__func__);
			
			if((check_param->is_loop_ok) == FALSE)
			{
				mtos_printk("[%s][ERROR] loop may be dead !!!!!!!!!!\n",__func__);
				hal_pm_reset();
			}
			else
			{

				 check_param->is_loop_ok= FALSE;
				cnt = 0;
				mtos_printk("[%s] loop is alive!!!!!!1\n",__func__);
			}
		}
		else{
			//
			mtos_task_sleep(1000);
			if(cnt%10 == 0)
			{
				mtos_printk("check task sleep %dsec !!!\n",cnt);
			}
		}
	}

	
	mtos_printk("[%s] == end end ===\n",__func__);

	#endif
}


void  start_check_whileloop_alive_task(void * p_param)
{
	  mtos_printk("[%s] == start start ===\n",__func__);
         int cur_prio = get_valid_priority();
 		mtos_printk("[%s] == cur_prio:%d ===\n",__func__,cur_prio);
	    if(mtos_task_create((u8 *)"check loop alive", check_loop, p_param, cur_prio, NULL, 16*1024) == NULL)
	    {
			mtos_printk("[%s][ERROR] fail to create task===!!!!!!!!!!\n",__func__);
			MT_ASSERT(0);
	    }
		 
	  mtos_printk("[%s] == end end ===\n",__func__);
}


//static  BOOL  is_loop_ok = TRUE;
RET_CODE ufs_write_flash_func(void)
{
    u8 * destaddr = NULL;
    u8 * freebuffer = NULL;
    u8 * inputString = NULL;
    int fileLen = 0;
    u32  actual_len= 0;
    int open_ret = 0;
    int finish_file_size = 0xB0000; // upgrade from 0xB0000 to end of file
    int adjust_ofs = 0xB0000; // re-upgrade flash when upgradeFlashImage return fail
    RET_CODE ret= ERR_FAILURE;

    ufs_file_t * ufs_fd = (ufs_file_t *)mtos_malloc(sizeof(ufs_file_t));
    memset(ufs_fd, 0x00, sizeof(ufs_file_t));
    unsigned short  path_tmp[256]={0};	
    unsigned short * path_unicode = Convert_Utf8_To_Unicode((unsigned char *)LIVE_UPGRADE_RAM_FILE,path_tmp);
    FILE_SEQ_DEBUG("\n\n Now ufs_open : %s \n\n", path_unicode);
    open_ret = ufs_open(ufs_fd, path_unicode, UFS_READ);

    if (open_ret != FR_OK) {
        FILE_SEQ_ERROR("[%s] FAIL TO OPEN [%s][%d]\n", __func__, LIVE_UPGRADE_RAM_FILE, __LINE__);
        mtos_free(ufs_fd);
        return 0;
    }
    FILE_SEQ_DEBUG("[%s]:%d ufs ufs_fd->file_size = %d \n",__func__,__LINE__,ufs_fd->file_size);
	
    while( ufs_fd->file_size > finish_file_size)
    {
		if ( (ufs_fd->file_size -finish_file_size) >= UPG_BURN_PACKET_SIZE )
		{
		fileLen = UPG_BURN_PACKET_SIZE;
		}
		else
		{
		fileLen = ufs_fd->file_size - finish_file_size;
		}
		FILE_SEQ_DEBUG("\n %s  spilt file size is %d\n", __FUNCTION__, fileLen);

		inputString = (unsigned char *)mtos_malloc((fileLen) + 1);
		MT_ASSERT(inputString != NULL);
		memset(inputString, 0x00, fileLen + 1);
		if(finish_file_size == 0)
			ufs_lseek(ufs_fd, 0, UFS_SEEK_HEAD);
		else
			ufs_lseek(ufs_fd, adjust_ofs, UFS_SEEK_CUR);

		ufs_read(ufs_fd, inputString, fileLen, &actual_len);
		FILE_SEQ_DEBUG("\n %s  input buf0x %x\n", __FUNCTION__, inputString);
		ret = upgradeFlashImage(inputString,actual_len,finish_file_size);
		if ( SUCCESS != ret)
		{
			mtos_printk("[%s] : %d upgradeFlashImage is failed , maybe need you re-upgrade!!\n",__func__,__LINE__);
			adjust_ofs -=actual_len;
		}
		else
		{
			finish_file_size += actual_len;
			adjust_ofs = 0;
		}
		FILE_SEQ_DEBUG("\n inputString = 0x%x 0x%x finish_file_size = %d \n",*inputString,*(inputString+1),finish_file_size);
		mtos_free(inputString);
		mtos_task_sleep(100);
    }	
	
    ufs_close(ufs_fd);
    mtos_free(ufs_fd);
    ufs_fd = NULL;
	
  return ret;
}
extern void replace_real_play_path(char * p_play_path);
#ifdef TEST_SPEED_OPEN
static BOOL get_testspeed_version( int *sw_ver)
{
	int i = 0;
	u32 add_ver = 0xb0000;
	int ver_len = 4;
	RET_CODE ret = ERR_NOFEATURE;
   	charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,SYS_DEV_TYPE_CHARSTO);	
	//mtos_printk("\n\n W(ver)R addr[%x] len[%x]\n", add_ver, ver_len);
	ret= charsto_read(p_charsto_dev, add_ver, sw_ver, ver_len);
	if(SUCCESS != ret)
	{
	  mtos_printk("flash read error\n");
	  return FALSE;
	}

	return TRUE;
}
void stop_testspeed(void)
{
	#ifndef __LINUX__
		stop_testspeed_flag = TRUE;
	#endif
		return;
}
void test_loadSpeed()
{
#ifdef WITH_TCPIP_PROTOCOL
#ifndef __LINUX__
    mtos_printk("[%s]==========start start ================\n", __func__);
    FILE_SEQ_T * pFileSeq =  file_seq_get_instance();
    pFileSeq->register_event_cb(pFileSeq, test_speed_event_callback);
    u32 timelen = 0 ;
    char * p_time_value = NULL ;
    char * tmp_buf2 = NULL; 
    char * tmp_buf3 = NULL; 
    char time_value[10] = {0};

    if (timelen == 0) {
        timelen = 6 * 1000 ;
    }

    char * server_ip = NULL;
    int  server_port = 0xfffff;
    int src_port = 3932;
    struct sockaddr_in saddr;
    int client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&saddr, 0, sizeof(saddr));   /*清空表示地址的结构体变量。*/
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (client_fd >= 0) {
        struct sockaddr_in clientService;
        memset(&clientService, 0, sizeof(clientService));
        clientService.sin_family = AF_INET;
        //char * server_ip = "192.168.33.169"


       // server_ip =  parse_socket_param(&server_port);

        if(server_ip == NULL || server_port == 0xfffff)
        {
            server_ip = get_sort_server_ip(); // "58.246.7.134";
            server_port = get_sort_server_port(); // 5000;
        }


        clientService.sin_addr.s_addr = inet_addr(server_ip);
        clientService.sin_port = htons(server_port);
        mtos_printk("[%s]@@@@ now connect to server:%s  port:%d\n",
                    __func__, server_ip, server_port);
        int con = -1;
        int num = 0;
        int opt=1;
        while (con != 0) {
            //bind source port
            saddr.sin_port = htons(src_port);
            mtos_printk("\n%s %d  src port%d\n", __func__, __LINE__, src_port);

            if (client_fd < 0) {
                client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

                if (client_fd < 0) {
                    continue;
                }
            }

            //
          

            setsockopt(client_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
            
            //

            bind(client_fd, (struct sockaddr *)&saddr, sizeof(saddr));
            ff_socket_nonblock(client_fd, 0);
            struct timeval to;
            to.tv_sec = 5000 * 2;
            setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to));
            setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, &to, sizeof(to));
            //
            con = connect(client_fd, (struct sockaddr *)&clientService, sizeof(clientService));
            #ifndef __LINUX__
            mtos_printk("connect ret: %d, errno: %d\n", con, errno);
            #endif
            mtos_task_sleep(5000);

            if (con != 0) {
                closesocket(client_fd);
                src_port++;
                client_fd = -1;
            }

            num++;

            if (num > 15) {
                mtos_printk("[%s] >>>>>>connect timeout lallalalal<<<<< !!!!!!!!!\n", __func__);
                mtos_printk("[%s] >>>>>>>> reset board <<<<<<<<<<< !!!!!!!!!\n", __func__);

                if (client_fd >= 0) {
                    closesocket(client_fd);
                }

                goto restart;
            }
        }
    }

    mtos_printk("[%s]################Connect Success#####111######################\n", __func__);
    mtos_printk("[%s]################Connect Success#####222######################\n", __func__);
    mtos_printk("[%s] success connect to server [%s] port[%d]!!!!!!!!!\n", __func__, server_ip, server_port);
    mtos_printk("[%s] client fd [%d]!!!!!!!!!\n", __func__, client_fd);
    mtos_printk("[%s]#######################################################\n", __func__);

	extern  u8   g_mac_address[16];
	mtos_printk("[%s] mac:%s\n",__func__,g_mac_address);
	
	int ver_data = 0;
	
	char data_buf[96] = {0};
	memset(data_buf,0,96);

	if(get_testspeed_version(&ver_data))
	{
        unsigned char * pp = (unsigned char*)&ver_data ;
		sprintf(data_buf,"MAC=%s VER=%02x%02x%02x%02x",g_mac_address,pp[0],pp[1],pp[2],pp[3]);
	}
	else
	{
		sprintf(data_buf,"MAC=%s",g_mac_address);
	}
	
	char rev_ok_buf[64] = {0};
	int send_count = 0;
	loop_check_t check_param;
	memset(&check_param, 0, sizeof(check_param));
	check_param.is_loop_ok = TRUE;
	check_param.check_interval = 0;

		while(1)
		{
			if (send(client_fd, data_buf, strlen(data_buf) + 1, 0) <= 0) {

				OS_PRINTF("send mac str fail\n");
				goto restart;
					
			}

			int len  = recv(client_fd, rev_ok_buf, 64, 0);
			if(len > 0 && (strcmp(rev_ok_buf,"mac_ok") == 0))
			{
				mtos_printk("recv mac ok !!!!!!!\n");
				check_param.is_loop_ok = TRUE;
				check_param.check_interval = 3*60;
				start_check_whileloop_alive_task((void *)(&check_param));
				break;
			}
			else
			{
				goto restart;
			}

			OS_PRINTF("wait mac ok cmd !!!!!!\n");
			mtos_task_sleep(1000);		 


		}


		

//    char url[1024];
	char * url = NULL;
    int  len = 0;
    int  timeout = 0;
    int  loadTime = 0;
    int  bufferTime = 0;
    int load_timeout = 0;
    int buffer_timeout = 0;
    int  height = 0;
    int  width = 0;
    int  speed = 0;
    char replybuf[64] = {0};
    u32  speedStart = 0;
    u32  videoFreeSpaceSize = 0;
    BOOL islive = TRUE;
    BOOL isEndSpeedTime = FALSE;
    BOOL isOverseas=FALSE;
    int buf_ves_num = 0;
    int is_valid_url = 0x00;
    int  openScreenTime = 0;
    vdec_info_t vstate;
    int recv_cnt = 0;
    int dl_ret = 0;
    int dl_cnt = 0;
    int total_timeout = 0;
	
    total_timeout =1000* (get_sort_oversea_ltimer() + get_sort_oversea_btimer());
   // mtos_printk("\n\n oversea para_timeout=%d lt= %d, bt= %d \n",total_timeout,get_sort_oversea_ltimer(),get_sort_oversea_btimer(),get_sort_oversea_bsize());

    while (!stop_testspeed_flag) {
        mtos_printk("recv url\n");
	if( !url )
            url = (char *)mtos_malloc(1024);
        memset(url, 0, 1024);
        len  = recv(client_fd, url, 1024, 0);
        mtos_printk("recv url len %d\n", len);

        if (len > 0) {

	   check_param.is_loop_ok = TRUE;
            recv_cnt = 0;
            timeout =     timelen;
            width   =     0;
            height  =     0;
            loadTime =    0;
            bufferTime =  0;
            openScreenTime  =  0;
            speed =  0;
            isEndSpeedTime = FALSE;
	        is_valid_url = 0x00;
	   if( strncmp(url,"upgrade",7) == 0)
	   {
		check_param.check_interval = 10*60;
		for(dl_cnt; dl_cnt < 2 ; dl_cnt++)
		{
			FILE_SEQ_LOG("\n\n download URL: %s \n\n",get_sort_upgrade_url());
			dl_ret =Nw_DownloadURLTimeout(get_sort_upgrade_url(),LIVE_UPGRADE_RAM_FILE,70,NULL,NULL,NULL,NULL,0);
			if( 1 == dl_ret)
			{
				FILE_SEQ_DEBUG("[%s]:%d download file is success \n",__func__,__LINE__);
				if( SUCCESS == ufs_write_flash_func())
				{
					mtos_printk("[%s]:%d burn spi flash success \n",__func__,__LINE__);
				}
				else
				{
					FILE_SEQ_ERROR("[%s][ERROR]  burn flash.bin failed !!!!\n",__func__);
				}
				break;
			}
			else if(-1 == dl_ret)
			{
				FILE_SEQ_ERROR("ERR:Abort by user!!! \n");
				break;
			}
		}
		
		if(dl_ret != 1)
		{
			unsigned short  path_tmp[256] = {0};
			unsigned short * path_unicode = Convert_Utf8_To_Unicode((unsigned char *)LIVE_UPGRADE_RAM_FILE, path_tmp);
			ufs_delete(path_unicode, 0);
			mtos_printk("[%s][ERROR] download flash.bin failed !!!!\n",__func__);
		}
		
		mtos_printk("\n Now, system will restart\n");
		goto restart;
		
	   }
	   else
	   {
            mtos_printk("current test url :%s\n", url);
	   if ( strncmp(url,"abc",3) == 0)
	   {
		isOverseas=TRUE;
		load_timeout = 1000*get_sort_oversea_ltimer();
		buffer_timeout = 1000*get_sort_oversea_btimer();
		buf_ves_num = 1024*get_sort_oversea_bsize();
		if(!tmp_buf3)
		{
			tmp_buf3 = (char *)mtos_malloc(1024);
			memset(tmp_buf3, 0, 1024);
		}
			mtos_printk("\n ====> orig url : %s \n",url);
			memcpy(tmp_buf3, url, strlen(url)+1);
			url = tmp_buf3+3;
			mtos_printk("\n ##====>overseas url: %s\n",url);
	    }
	   else
	   {
	   	load_timeout = timelen;
		buffer_timeout = timelen;
	    buf_ves_num = 400*1024;
		isOverseas=FALSE;
	   }
	   
            pFileSeq->set_file_path_ex(pFileSeq, url, 0);
            speedStart = mtos_ticks_get();
            load_success = -1;
            pFileSeq->stream_type = STREAM_LIVE;
            pFileSeq->loadmedia_task((void *)pFileSeq);	   	
            while (load_success == -1 && ((mtos_ticks_get() - speedStart) * 10 <= load_timeout)) {
                mtos_task_sleep(400);
            }

            if (load_success == FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS) {
                islive  = TRUE;
                timeout = (mtos_ticks_get() - speedStart) * 10;
                loadTime = timeout;
                FILM_INFO_T  film_info;
                memset(&film_info, 0, sizeof(FILM_INFO_T));
                pFileSeq->get_film_info((void *)pFileSeq, &film_info);
                pFileSeq->set_subt_id((void *)pFileSeq, film_info.audio_track_num + 1);
                pFileSeq->set_tv_sys((void *)pFileSeq, 1);
                speedStart = mtos_ticks_get();
                pFileSeq->start((void *)pFileSeq, 0);

                /*get decoder first video frame timer*/
                while ((mtos_ticks_get() - speedStart) * 10 <= buffer_timeout) {
                    if ((!isEndSpeedTime) && (pFileSeq->totalVesNum > buf_ves_num)) { //400KByte
                        bufferTime = (mtos_ticks_get() - speedStart) * 10;
                        isEndSpeedTime = TRUE;
			if(isOverseas)
			{
				timeout = bufferTime + loadTime;
				is_valid_url = 0x03;
				break;
			}
                    }
                    mtos_task_sleep(500);
		if(!isOverseas )
		{
                        vdec_get_info(pFileSeq->p_vdec_dev, &vstate);

                        if (TRUE == vstate.is_stable) {
                            openScreenTime = (mtos_ticks_get() - speedStart) * 10;
                            break;
                        }
		}
                }
                if( !isOverseas)
                {
                if (openScreenTime <= 0) {
                    openScreenTime = 0;
                } else {
                    openScreenTime += loadTime;
                }

                /* get VES buffer timer*/
                while ((!isEndSpeedTime) && ((mtos_ticks_get() - speedStart) * 10 <= 5 * 1000)) {
                    if (pFileSeq->totalVesNum > 409600) { //400 KByte
                        break;
                    }

                    mtos_task_sleep(200);
                }

                if (!isEndSpeedTime) {
                    bufferTime = (mtos_ticks_get() - speedStart) * 10;
                }

                if (bufferTime > 0) {
                    speed = pFileSeq->totalVesNum / bufferTime ;
                } else {
                    speed = 10000;
                }

                timeout += bufferTime;
                vdec_get_info(pFileSeq->p_vdec_dev, &vstate);
                width = vstate.width;
                height = vstate.heigth;
	            is_valid_url = 0x01;		
              }	
	     else
	     {
		if(!isEndSpeedTime)
		{
			timeout = total_timeout;
		}
	     }
                stop_success =  -1 ;
                pFileSeq->stop((void *)pFileSeq);

                while (stop_success == -1) {
                    mtos_task_sleep(400);
                }

                if (!islive) {
                    timeout = timelen * 6;
		            is_valid_url = 0x00;
                }
            } else {
                /*loadmedia time out*/
                if (load_success == -1) {
                    stop_success =  -1 ;
                    /*force loadmedia thread exit*/
                    pFileSeq->stop((void *)pFileSeq);

                    while (stop_success == -1) {
                        if (load_success  == FILE_PLAYBACK_SEQ_LOAD_MEDIA_EXIT
                            || load_success  == FILE_PLAYBACK_SEQ_LOAD_MEDIA_ERROR
                            || load_success  == FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS) {
                            break;
                        }

                        mtos_task_sleep(400);
                    }

                    /*if loadmedia is ok, we should kill the relative task*/
                    if (load_success == FILE_PLAYBACK_SEQ_LOAD_MEDIA_SUCCESS) {
                        FILM_INFO_T  film_info;
                        pFileSeq->get_film_info((void *)pFileSeq, &film_info);
                        pFileSeq->set_subt_id((void *)pFileSeq, film_info.audio_track_num + 1);
                        pFileSeq->set_tv_sys((void *)pFileSeq, 1);
                        pFileSeq->start((void *)pFileSeq, 0);
                        mtos_task_sleep(1000 * 2); /////sleep 2S  for playing
                        stop_success =  -1 ;
                        pFileSeq->stop((void *)pFileSeq);

                        while (stop_success == -1) {
                            mtos_task_sleep(400);
                        }
                    }
                }

                OS_PRINTF("-----------------------------------------load fail  :%d\n", load_success);
	         if(isOverseas)
		        timeout = total_timeout;
	         else
                timeout = timelen * 2 ;

	         is_valid_url = 0x00;
            }



            DownloadResult ret;
	if( !isOverseas)
            {
            memset(&ret,0,sizeof(DownloadResult));

            if (timeout <  timelen * 2 && strstr(url, "http") != NULL) {
                TestSpeedCtrl s = {0, 1, 1};
				if(strstr(url, "vdn.live.cntv.cn") || strstr(url, "web-play.pptv.com") || strstr(url, "giniko.com"))
				{
					tmp_buf2 = (char *)mtos_malloc(1024);
					if(tmp_buf2)
					{
						mtos_printk("\n ====> orig url : %s \n",url);
						memset(tmp_buf2, 0, 1024);
						memcpy(tmp_buf2, url, strlen(url));
						url = tmp_buf2;
						replace_real_play_path(url);
						mtos_printk("\n ##====> real url : %s \n",url);
					}
				}
                ret = Nw_DownloadURL_TestSpeed(url, 10000, &s) ;
                OS_PRINTF("------------------------------------Nw_DownloadURL_TestSpeed speed:%d, connectMS:%dms, downloadMS:%dms  errcode: %d \n", ret.speed, ret.connectMS, ret.downloadMS,ret.errcode);

                if (ret.speed <= 0) {
                    timeout = timelen * 4;
		            is_valid_url = 0x00;
                }
	            is_valid_url = (is_valid_url << 1)|0x01;
            }
	   else if((timeout < timelen * 2) && (is_valid_url  !=0x0)){
		is_valid_url = (is_valid_url << 1)|0x01;
	   }
            }
	   else
	   {
		memset(&ret,0,sizeof(DownloadResult));
	   }
	   //mtos_printk("\n\n is_valid_url = %d \n\n",is_valid_url);
	   if(is_valid_url != 0x3)
	   	is_valid_url = 0;
	   else
	   	is_valid_url = 1;
	   
            memset(replybuf, 0, sizeof(replybuf));
            sprintf(replybuf, "%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d-%d", 
                        timeout,width, height,loadTime,bufferTime,
                        openScreenTime,speed,
                        ret.speed,ret.connectMS,ret.downloadMS,ret.errcode,is_valid_url);

            //sprintf(replybuf, "%d-%d-%d-%d-%d-%d-%d", timeout, width, height, loadTime, bufferTime, openScreenTime, speed);
            mtos_printk("=====================result================================\n");
            mtos_printk("\n is_valid_url:%d timeout:%d, loadTime:%d, bufferTime:%d, openScreenTime: %d  W x H:%d x %d, speed:%d dl_errcode: %d\n\n",is_valid_url, timeout, loadTime, bufferTime, openScreenTime, width, height, speed,ret.errcode);
            mtos_printk("=====================result================================\n");

            if (send(client_fd, replybuf, strlen(replybuf) + 1, 0) <= 0) {
                OS_PRINTF("send result fail");
                break;
            }

            OS_PRINTF("send result successful");
        	}
        } else {
            recv_cnt ++;
            if(recv_cnt >3)
              break;
        }
    }

    OS_PRINTF("detect server exit!\n");
    closesocket(client_fd);
restart:
	if(!stop_testspeed_flag)
	{
		mtos_task_sleep(10000);
		hal_pm_reset();
	}
    #endif

	#endif
}
#endif

#endif
#endif
