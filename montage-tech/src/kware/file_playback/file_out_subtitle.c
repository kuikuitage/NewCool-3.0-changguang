/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

// system
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
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

#include "pti.h"

#include "common.h"
#include "region.h"
#include "display.h"
#include "gpe_vsb.h"
#include "vdec.h"
#include "aud_vsb.h"
#include "audio.h"


#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "list.h"
#include "drv_dev.h"
#include "sys_types.h"
#include "sys_define.h"
#include "block.h"
#include "ff.h"
#include "fsioctl.h"
#include "ufs.h"
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#else
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_printk.h"
#endif
#include "../ts_seq/fifo_kw.h"
#include "lib_memp.h"
#include "ts_sequence.h"
#include "libmpdemux/stheader.h"

#include "stheader.h"

#include "file_playback_sequence.h"
#include "demux_mp.h"
#include  "file_seq_misc.h"
#include  "file_out_subtitle.h"
#include  "file_seq_internal.h"


#if defined(EXT_MEM_POOL)
#include "ext_mem_interface.h"
#undef mtos_malloc
#undef mtos_free
#undef mtos_calloc
#undef mtos_realloc
#define mtos_malloc malloc_from_demux_mp
#define mtos_free  free_from_demux_mp
#define mtos_calloc calloc_from_demux_mp
#define mtos_realloc  realloc_from_demux_mp
#else
#include "mtos_mem.h"
#endif

#ifndef EOF
#define EOF (-1)
#endif


//#ifdef TUNA_SDK

#define MAX_FILE_PATH_LEN   (512)
#define MAX_FILE_NAME_LEN   (64)
#define MAX_EXTRA_SUBT_NUM   (8)
char g_cur_file_dir[MAX_FILE_PATH_LEN];
char g_cur_sbut_list[MAX_EXTRA_SUBT_NUM][MAX_FILE_NAME_LEN];

#define SUBTITLE_API_LOG(format, args...)              mtos_printk(format, ##args)
#define SUBTITLE_API_WARNING(format, args...)        OS_PRINTF(format, ##args)
#define SUBTITLE_API_DEBUG(format, args...)          OS_PRINTF(format, ##args)
#define SUBTITLE_API_ERROR(format, args...)          mtos_printk(format, ##args)


/*!
  line_status_t
  */
typedef enum
{

  LINE_TYPE_INDEX,

  LINE_TYPE_TIME,

  LINE_TYPE_CONTENT
}line_status_t;

/*!
  LINE_MAX_SIZE
  */
#define LINE_MAX_SIZE (512)


static RET_CODE subtible_get_line(u8 *p_data, int *p_len, int *p_out_len)
{
  int ch = 0;
  int i = 0;
  s32 ret = 0;

  while(i < *p_len)
  {
    ch = p_data[i];
    if ('\n' == ch)
      break;
    else if (EOF == ch)
    {
      ret = -2;
      break;
    }
    else
    {
        i++;
    }

    if(i >= LINE_MAX_SIZE)
    {
      SUBTITLE_API_ERROR("subtible_get_line: line is too long!!\n");
      ret = -1;
      break;
    }
  }

  /*String maybe not end of "\n"*/
  if(i == *p_len)
  {
    *p_out_len = i;
  }
  else
  {
    *p_out_len = i + 1;
  }


  if(*p_out_len == 2)
  {
        if(p_data[0] == '\r' && p_data[1] == '\n')
            return 1;
  }
  return ret;
}

/*!
  if the line of data is all blank string, return FALSE.
  */
static BOOL is_valid_content(u8 *p_data, u32 len)
{
   u32 i = 0;
   BOOL ret = TRUE;

   if(p_data[0] == 0x20)
   {
      for(i = 1; i < len; i ++)
      {
         if((p_data[i] == '\r') || (p_data[i] == '\n'))
         {
            ret = FALSE;
            SUBTITLE_API_DEBUG("%s: ret = FALSE;\n", __func__);
            break;
         }

         if(p_data[i] != 0x20)
         {
            ret = TRUE;
            SUBTITLE_API_DEBUG("%s: ret = TRUE;\n", __func__);
            break;
         }
      }

   }

   return ret;
}

static RET_CODE srt_parse_time(u8 *p_content)
{
    if(strstr(p_content, " --> "))
        return SUCCESS;

    return ERR_FAILURE;
}

static RET_CODE seek_srt_pos(u8 *p_in_data, int in_len, u8 **p_start_read, int *start_len, int seek_sec)
{
    int line_len = 0;
    int exist_len = (int)in_len;
    char p_content[LINE_MAX_SIZE];
    int  hs = 0, ms = 0, ss = 0, p_ts_start = 0;
    while(exist_len >0)
    {
        p_in_data += line_len;
        subtible_get_line(p_in_data, &exist_len, &line_len);

        memset(p_content, 0, LINE_MAX_SIZE);
        memcpy(p_content, p_in_data, line_len);
        if(strstr(p_content, " --> "))
        {
            int c = sscanf(p_content, "%d:%2d:%2d%*1[,.]%3d -->",
                 &hs, &ms, &ss, &p_ts_start);
            if(c >= 3)
            {
                int pts = ss + 60 * (ms + 60 * hs);
                if(pts >= seek_sec)
                {
                    *p_start_read = p_in_data;
                    *start_len = exist_len;
                    return SUCCESS;
                }
            }
        }
        exist_len -= line_len;
    }

    return ERR_FAILURE;
}

static RET_CODE srt_data_parse(PROC_SUBT_HDL_T *p_subtitle, u8 *p_in_data, int in_len)
{
    int line_len = 0;
    int exist_len = (int)in_len;
    BOOL got_timestamp = FALSE;
    RET_CODE ret = ERR_FAILURE;
    u8 *p_read = p_in_data;
    int one_line_len = 0;

    char p_time[128];
    char p_content[LINE_MAX_SIZE];
    char p_tmp_number[8];
    u8 *p_tmp_read = NULL;
    int tmp_exist_len = NULL;

    int line_num = 0;
    while(1)//exist_len >0
    {
        p_read += line_len;
        line_len = 0;
        if(p_subtitle->task_status == EXITING)
        {
            return SUCCESS;
        }
        else if(p_subtitle->task_status == SUB_SEEK)
        {
            //reset p_read by input seek second
            ret = seek_srt_pos(p_in_data, in_len, &p_read, &exist_len, p_subtitle->seek_second);
            if(ERR_FAILURE == ret)
            {
                p_read = p_in_data + in_len-2;
                exist_len = 2;
            }
            p_subtitle->seek_second = 0;

            clear_sub_fifo_kw(p_subtitle->p_fifo_handle);    //clear fifo when do seek
            p_subtitle->task_status = RUNNING;
        }

        if(exist_len <= 2)
        {
            //the file is end, but still loop
            //SUBTITLE_API_DEBUG("exist_len <= 2 and loop\n");
            mtos_task_sleep(10);
            continue;
        }

        int result = 0;
        if(got_timestamp)
        {
            p_tmp_read = p_read;
            tmp_exist_len = exist_len;
            one_line_len = 0;
            while(1)
            {
                p_tmp_read += one_line_len;
                result = subtible_get_line(p_tmp_read, &tmp_exist_len, &one_line_len);
                if(result != 0 || one_line_len == 0)
                    break;

                memset(p_content, 0, LINE_MAX_SIZE);
                memcpy(p_content, p_tmp_read, one_line_len);
                if(one_line_len < 6)
                {
                    //check is number
                    memset(p_tmp_number, 0, 8);
                    memcpy(p_tmp_number, p_tmp_read, one_line_len);
                    int value = atoi(p_tmp_number);
                    if(value > 0)
                        break;
                }
                tmp_exist_len -= one_line_len;
                line_len += one_line_len;
            }
        }
        else
            subtible_get_line(p_read, &exist_len, &line_len);
        memset(p_content, 0, LINE_MAX_SIZE);
        memcpy(p_content, p_read, line_len);
        exist_len -= line_len;

        ret = srt_parse_time(p_content);
        if(ret == SUCCESS)
        {
            got_timestamp = TRUE;
            memset(p_time, 0, 128);
            memcpy(p_time, p_read, line_len);
            continue;
        }

        if(got_timestamp)
        {
            //next is content
            SUBTITLE_API_DEBUG("number[%d] content is [%s]\n", line_num, p_content);
            got_timestamp = FALSE;

            //remove {\a6} {\an8} {\an3} {\an4}
            if(strstr(p_content, "{\\a") )
                continue;

            line_num++;
        }
        else
            continue;
        int cur_sub_size = strlen(p_time) + strlen(p_content);
        int avail = 0;
        while(1)
        {
            mtos_sem_take((os_sem_t *)(&(p_subtitle->fifo_mutex)), 0);
            avail = get_freesize_sub_fifo_kw(p_subtitle->p_fifo_handle);
            TASK_STATE task_status = p_subtitle->task_status;
            mtos_sem_give((os_sem_t *)(&(p_subtitle->fifo_mutex)));
            if(task_status == EXITING)
                return SUCCESS;
            if(avail > (cur_sub_size + 8))
                break;
            SUBTITLE_API_ERROR("[%s][ERROR][%d] p_fifo_handle is full cannot write!!!!!!!!! wait for read!\n", __func__, __LINE__);
            mtos_task_sleep(200);
        }

        SUBT_DATA * subt_data = (SUBT_DATA *) mtos_malloc(sizeof(SUBT_DATA));
        subt_data->data = mtos_malloc(cur_sub_size + 8);
        int  hs = 0, ms = 0, ss = 0, p_ts_start = 0;
        int c = sscanf(p_time, "%d:%2d:%2d%*1[,.]%3d -->",
                 &hs, &ms, &ss, &p_ts_start);
        if (c >= 3)
            subt_data->pts = ss + 60 * (ms + 60 * hs);
        SUBTITLE_API_DEBUG("number[%d] time str is [%s] pts[%d]\n", line_num, p_time, subt_data->pts);
        SUBTITLE_API_DEBUG("number[%d] content is [%s]\n", line_num, p_content);
        subt_data->size = cur_sub_size;
        subt_data->data[0] = ((subt_data->size) & 0xff000000) >> 24;
        subt_data->data[1] = ((subt_data->size) & 0xff0000) >> 16;
        subt_data->data[2] = ((subt_data->size) & 0xff00) >> 8;
        subt_data->data[3] = ((subt_data->size)) & 0xff;
        subt_data->data[4] = ((subt_data->pts) & 0xff000000) >> 24;
        subt_data->data[5] = ((subt_data->pts) & 0xff0000) >> 16;
        subt_data->data[6] = ((subt_data->pts) & 0xff00) >> 8;
        subt_data->data[7] = ((subt_data->pts)) & 0xff;

        strcpy(subt_data->data + 8, p_time);
        memcpy(subt_data->data + 8 + strlen(p_time), p_content, strlen(p_content));
        mtos_sem_take((os_sem_t *)(&(p_subtitle->fifo_mutex)), 0);
        write_sub_fifo_kw(p_subtitle->p_fifo_handle, subt_data->data, cur_sub_size + 8);
        mtos_sem_give((os_sem_t *)(&(p_subtitle->fifo_mutex)));

        mtos_free(subt_data->data);
        mtos_free(subt_data);
        mtos_task_sleep(10);
    }


    return SUCCESS;
}

static RET_CODE seek_ssa_pos(u8 *p_in_data, int in_len, u8 **p_start_read, int *start_len, int seek_sec)
{
    int line_len = 0;
    int exist_len = (int)in_len;
    //Dialogue: 0,1:46:43.76,1:46:46.89,*Default,NTP
    char p_content[64];//64 is enough
    char *p_read = NULL;
    int  hs = 0, ms = 0, ss = 0, p_ts_start = 0;
    while(exist_len >0)
    {
        p_in_data += line_len;
        subtible_get_line(p_in_data, &exist_len, &line_len);

        memset(p_content, 0, 64);
        if(line_len < 60)
            memcpy(p_content, p_in_data, line_len);
        else
            memcpy(p_content, p_in_data, 60);
        if(0 == strncmp(p_content, "Dialogue:", strlen("Dialogue:")) )
        {
            p_read = p_content + strlen("Dialogue:");
            //Dialogue: 0,1:46:42.06,
            while(*p_read != ',' && *p_read != '\0' )
                p_read++;
            if(*p_read == '\0')
            {
                SUBTITLE_API_WARNING("ass file Dialogue format error, content is [%s]\n", p_content);
                continue;
            }
            p_read++;
            int c = sscanf(p_read, "%d:%02d:%02d.%02d",
                 &hs, &ms, &ss, &p_ts_start);
            if(c >= 3)
            {
                int pts = ss + 60 * (ms + 60 * hs);
                //SUBTITLE_API_DEBUG("seek_ssa_pos  time: %d:%d:%d\n", hs, ms, ss);
                if(pts >= seek_sec)
                {

                    *p_start_read = p_in_data;
                    *start_len = exist_len;
                    return SUCCESS;
                }
            }
        }
        exist_len -= line_len;
    }

    return ERR_FAILURE;
}

static u8 ssa_get_line_type(u8 *p_line)
{
  u8 i = 0;

  for (i = 0; i < LINE_TOTAL_SPEC ; i ++)
  {
    if(0 == strncmp(ssa_line_list[i].p_name, p_line, strlen(ssa_line_list[i].p_name)))
    {
      //OS_PRINTF("get cmd %d\n", i);
      break;
    }
  }

  if (i >= LINE_TOTAL_SPEC)
  {
    return LINE_TOTAL_SPEC;
  }

  return i;
}

static u8 ssa_cmd_parse(u8 *out_len, u8 *p_in)
{
  u8 i = 0;

  for (i = 0; i < LINE_TOTAL_CMD; i ++)
  {
    if(0 == strncmp(ssa_cmd_list[i].p_name, p_in, strlen(ssa_cmd_list[i].p_name)))
    {
      //OS_PRINTF("get cmd %d\n", i);
      break;
    }
  }

  if (i >= LINE_TOTAL_CMD)
  {
    *out_len = 0;
    return LINE_TOTAL_CMD;
  }
  else
  {
    *out_len = strlen(ssa_cmd_list[i].p_name);
  }

  return i;
}

static inline int is_eol(char buf)
{
    return buf == '\r' || buf == '\n' || buf == 0;
}

static u32 ssa_time_get(u8 *p_in, int *h, int *m, int *s, int *c)
{
    u32 out = 0;
    *h = 0;
    *m = 0;
    *s = 0;
    *c = 0;
    int count = 0;
    if ((count = sscanf(p_in, "%d:%02d:%02d.%02d", h, m, s, c)) == 4)
    {
        *c *= 10;
        out = *s + 60 * (*m + 60 * (*h));
    }

    //SUBTITLE_API_DEBUG("ssa_time_get  time: %d:%d:%d.%d\n", *h, *m, *s, *c);
    return out;
}


static RET_CODE ssa_parse_dialogue(u8 *p_in_data, u32 in_len, BOOL is_scan, u8 *p_time, u8 *p_content, int *pts)
{
    u8 *p_read = p_in_data;
    s16 len = (s16)in_len;
    u8 cnt = 0;
    u16 read_len = 0;
    u8 *p_dest = 0;
    u8 cmd_len = 0;

    int sh = 0, sm = 0, ss = 0, sc = 0;
    int eh = 0, em = 0, es = 0, ec = 0;
    while(len > 0)
    {
        p_read = memchr(p_read, ',', in_len);
        if(p_read == NULL)
        {
            return SUCCESS;
        }
        p_read ++;
        read_len = (u32)p_read - (u32)p_in_data;
        len = in_len - read_len;
        cnt ++;

        if(cnt == 1) // start time
        {
            *pts = ssa_time_get(p_read, &sh, &sm, &ss, &sc);
        }
        else if(cnt == 2) // end time
        {
            ssa_time_get(p_read, &eh, &em, &es, &ec);
            sprintf(p_time, "%02d:%02d:%02d,%03d --> %02d:%02d:%02d,%03d\r\n",
                                sh, sm, ss, sc, eh, em, es, ec);
        }
        else if(cnt == 9)// text
        {
            if(is_scan)
            {
                len = 0;
                break;
            }
            p_dest = p_content;
            while(!is_eol(*p_read) && len > 0)
            {
                //skip {}
                if(*p_read == '{')//cmd with {}
                {
                     while(*p_read != '}' && (!is_eol(*p_read)))
                     {
                        p_read ++;
                        ///TODO  cmd_parse
                     }
                     p_read ++; //ignore the }
                }

                //such as {}{}
                if(*p_read == '{')//cmd with {}
                {
                    continue;
                }

                //skip "\"
                if(*p_read == '\\')
                {//got cmd, parse it, and skip the cmd len
                     p_read ++;
                     u8 cur_cmd = ssa_cmd_parse(&cmd_len,p_read);
                     if(LINE_TOTAL_CMD != cur_cmd)
                     {
                        p_read += cmd_len;
                        if(LINE_RETURN == cur_cmd || LINE_FORCE_RETURN == cur_cmd)
                        {
                            *(p_dest++) = '\r';
                            *(p_dest++) = '\n';
                        }
                        else if(LINE_FORCE_SPACE == cur_cmd)
                            *(p_dest++) = ' ';
                        continue;
                     }

                }

                //copy data
                if(is_eol(*p_read))
                {
                    break;
                }
                *p_dest = *p_read;
                p_dest ++;
                p_read ++;
           }
           *p_dest = 0;
           break;
        }
    }

    return SUCCESS;
}

static RET_CODE ssa_get_line_parse(u8 *p_in_data, u32 in_len, BOOL is_scan, u8 *p_time, u8 *p_content, int *pts)
{
  u8 type = 0;
  RET_CODE ret = ERR_FAILURE;

  type = ssa_get_line_type(p_in_data);

  switch(type)
  {
    case LINE_SCRIPT_INFO:
      break;
    case LINE_TITLE:
      break;
    case LINE_ORIGINAL_SCRIPT:
      break;
    case LINE_ORIGINAL_TRANS:
    case LINE_ORIGINAL_TIMING:
    case LINE_ORIGINAL_POINT:
    case LINE_SCRIPT_TYPE:
    case LINE_COLLISIONS:
    case LINE_RESX:
    case LINE_RESY:
    case LINE_TIMER:
    case LINE_FORMAT:
    case LINE_STYLE:
    case LINE_EVENT:
      break;
    case LINE_DIALOGUE:
      ret = ssa_parse_dialogue(p_in_data, in_len, is_scan, p_time, p_content, pts);
      break;

  }

  return ret;
}


static RET_CODE ssa_data_parse(PROC_SUBT_HDL_T *p_subtitle, u8 *p_in_data, u32 in_len)
{
    int line_len = 0;
    int exist_len = (int)in_len;
    RET_CODE ret = ERR_FAILURE;
    u8 *p_read = p_in_data;

    u8 p_time[128];
    u8 p_content[LINE_MAX_SIZE];

    int line_num = 0;
    while(1)//exist_len >0
    {
        p_read += line_len;
        line_len = 0;
        if(p_subtitle->task_status == EXITING)
        {
            return SUCCESS;
        }
        else if(p_subtitle->task_status == SUB_SEEK)
        {

            //reset p_read by input seek second
            ret = seek_ssa_pos(p_in_data, in_len, &p_read, &exist_len, p_subtitle->seek_second);
            if(ERR_FAILURE == ret)
            {
                SUBTITLE_API_ERROR("Seek ssa/ass subtile failed, input seek second[%d]\n", p_subtitle->seek_second);
                p_read = p_in_data + in_len-2;
                exist_len = 2;
            }
            p_subtitle->seek_second = 0;

            clear_sub_fifo_kw(p_subtitle->p_fifo_handle);    //clear fifo when do seek
            p_subtitle->task_status = RUNNING;
        }

        if(exist_len <= 2)
        {
            //the file is end, but still loop
            //SUBTITLE_API_DEBUG("exist_len <= 2 and loop\n");
            mtos_task_sleep(10);
            continue;
        }

        int result = subtible_get_line(p_read, &exist_len, &line_len);
        exist_len -= line_len;
        if(result != 0)
        {
            continue;
        }

        memset(p_time, 0, 128);
        memset(p_content, 0, LINE_MAX_SIZE);
        int pts = 0;
        ret = ssa_get_line_parse(p_read, line_len, FALSE, p_time, p_content, &pts);
        if(ret != SUCCESS)
            continue;
        line_num++;

        int cur_sub_size = strlen(p_time) + strlen(p_content);
        int avail = 0;
        while(1)
        {
            mtos_sem_take((os_sem_t *)(&(p_subtitle->fifo_mutex)), 0);
            avail = get_freesize_sub_fifo_kw(p_subtitle->p_fifo_handle);
            TASK_STATE task_status = p_subtitle->task_status;
            mtos_sem_give((os_sem_t *)(&(p_subtitle->fifo_mutex)));
            if(task_status == EXITING)
                return SUCCESS;
            if(avail > (cur_sub_size + 8))
                break;
            SUBTITLE_API_ERROR("[%s][ERROR][%d] p_fifo_handle is full cannot write!!!!!!!!! wait for read!\n", __func__, __LINE__);
            mtos_task_sleep(200);
        }

        SUBT_DATA * subt_data = (SUBT_DATA *) mtos_malloc(sizeof(SUBT_DATA));
        subt_data->data = mtos_malloc(cur_sub_size + 8);
        subt_data->pts = pts;
        SUBTITLE_API_DEBUG("number[%d] time str is [%s] pts[%d]\n", line_num, p_time, subt_data->pts);
        SUBTITLE_API_DEBUG("number[%d] content is [%s]\n", line_num, p_content);
        subt_data->size = cur_sub_size;
        subt_data->data[0] = ((subt_data->size) & 0xff000000) >> 24;
        subt_data->data[1] = ((subt_data->size) & 0xff0000) >> 16;
        subt_data->data[2] = ((subt_data->size) & 0xff00) >> 8;
        subt_data->data[3] = ((subt_data->size)) & 0xff;
        subt_data->data[4] = ((subt_data->pts) & 0xff000000) >> 24;
        subt_data->data[5] = ((subt_data->pts) & 0xff0000) >> 16;
        subt_data->data[6] = ((subt_data->pts) & 0xff00) >> 8;
        subt_data->data[7] = ((subt_data->pts)) & 0xff;

        strcpy(subt_data->data + 8, p_time);
        memcpy(subt_data->data + 8 + strlen(p_time), p_content, strlen(p_content));
        mtos_sem_take((os_sem_t *)(&(p_subtitle->fifo_mutex)), 0);
        write_sub_fifo_kw(p_subtitle->p_fifo_handle, subt_data->data, cur_sub_size + 8);
        mtos_sem_give((os_sem_t *)(&(p_subtitle->fifo_mutex)));

        mtos_free(subt_data->data);
        mtos_free(subt_data);
        mtos_task_sleep(10);
    }


    return SUCCESS;
}

static RET_CODE smi_parse_time(u8 *p_content)
{
    if(strstr(p_content, "<SYNC Start"))
        return SUCCESS;

    return ERR_FAILURE;
}

static void caculate_time(int input_ms, int *hour, int *minute, int *second, int *msec)
{
    *hour = input_ms/(1000 * 60 *60);
    int tmp_ms = input_ms - (*hour)*(1000 * 60 *60);
    *minute = tmp_ms/(1000 * 60);
    tmp_ms -= (*minute)*(1000 * 60);
    *second = tmp_ms/1000;
    *msec = tmp_ms%1000;
    return;
}

static RET_CODE smi_data_parse(PROC_SUBT_HDL_T *p_subtitle, u8 *p_in_data, int in_len)
{
    SUBTITLE_API_DEBUG("%s: enter enter!\n", __func__);
    int line_len = 0;
    int exist_len = (int)in_len;
    BOOL got_timestamp = FALSE;
    RET_CODE ret = ERR_FAILURE;
    u8 *p_read = p_in_data;

    char p_time[128];
    char p_content[LINE_MAX_SIZE];
    char p_tmp_number[8];
    //u8 *p_tmp_read = NULL;
    //int one_line_len = 0;
    //int tmp_exist_len = NULL;
    u8  is_subt_start_line = 0;
    int start_time_ms;
    int end_time_ms;
    char *new_start, *tmp_time_str;
    int sh = 0, sm = 0, ss = 0, sc = 0;
    int eh = 0, em = 0, es = 0, ec = 0;
    u8  is_get_content = 0;

    int line_num = 0;
    while(1)//exist_len >0
    {
        p_read += line_len;
        line_len = 0;
        if(p_subtitle->task_status == EXITING)
        {
            return SUCCESS;
        }
        else if(p_subtitle->task_status == SUB_SEEK)
        {
            //reset p_read by input seek second
            /*ret = seek_srt_pos(p_in_data, in_len, &p_read, &exist_len, p_subtitle->seek_second);
            if(ERR_FAILURE == ret)
            {
                p_read = p_in_data + in_len-2;
                exist_len = 2;
            }
            else
                is_subt_start_line = 1;*/
            p_subtitle->seek_second = 0;

            clear_sub_fifo_kw(p_subtitle->p_fifo_handle);    //clear fifo when do seek
            p_subtitle->task_status = RUNNING;
        }

        if(exist_len <= 2)
        {
            //the file is end, but still loop
            //SUBTITLE_API_DEBUG("exist_len <= 2 and loop\n");
            mtos_task_sleep(10);
            continue;
        }

        int result = 0;
        if(got_timestamp)
        {
            memset(p_content, 0, LINE_MAX_SIZE);
            char p_one_line_content[256];
            memset(p_one_line_content, 0, 256);
            while(1)
            {
                result = subtible_get_line(p_read, &exist_len, &line_len);
                if(result != 0)
                    break;

                memset(p_one_line_content, 0, 256);
                memcpy(p_one_line_content, p_read, line_len);
                if(strstr(p_one_line_content, "&nbsp;"))
                {
                    exist_len -= line_len;
                    p_read += line_len;
                    continue;
                }
                ret = smi_parse_time(p_one_line_content);
                if(ret == SUCCESS)
                {
                    tmp_time_str = comm_parse_get_inner_string(p_one_line_content, "Start=", ">", &new_start);
                    if(tmp_time_str)
                    {
                        end_time_ms = atoi(tmp_time_str);
                        caculate_time(end_time_ms, &eh, &em, &es, &ec);
                        mtos_free(tmp_time_str);
                        sprintf(p_time, "%02d:%02d:%02d,%03d --> %02d:%02d:%02d,%03d\r\n",
                                sh, sm, ss, sc, eh, em, es, ec);
                        is_get_content = 1;
                        //SUBTITLE_API_DEBUG("number[%d] final time string is [%s]\n", line_num, p_time);
                    }
                    break;
                }
                char *p_tmp_start = p_one_line_content;
                if(*p_tmp_start == '<')
                {
                    while(*p_tmp_start != '>')
                        p_tmp_start++;
                    strcat(p_content, p_tmp_start+1);
                }
                else
                    strcat(p_content, p_tmp_start);
                exist_len -= line_len;
                p_read += line_len;
            }
        }
        else
            subtible_get_line(p_read, &exist_len, &line_len);
        if(is_get_content == 0)
        {
            memset(p_content, 0, LINE_MAX_SIZE);
            memcpy(p_content, p_read, line_len);
        }
        else
        {
            is_get_content = 0;
        }
        exist_len -= line_len;

        if(is_subt_start_line == 0)
        {
            if(strstr(p_content, "<SYNC Start"))
            {
                SUBTITLE_API_DEBUG("start real subttile. 111111111111111111111111111111!!!!\n");
                is_subt_start_line = 1;//enter once
            }
            else
                continue;
        }

        if(strstr(p_content, "&nbsp;") || strstr(p_content, "</BODY>") || strstr(p_content, "</SAMI>"))
            continue;

        ret = smi_parse_time(p_content);
        if(ret == SUCCESS)
        {
            //sprintf(p_time, "%02d:%02d:%02d,%03d --> %02d:%02d:%02d,%03d\r\n",
            //                    sh, sm, ss, sc, eh, em, es, ec);
            got_timestamp = TRUE;
            memset(p_time, 0, 128);
            tmp_time_str = comm_parse_get_inner_string(p_read, "Start=", ">", &new_start);
            if(tmp_time_str)
            {
                start_time_ms = atoi(tmp_time_str);
                caculate_time(start_time_ms, &sh, &sm, &ss, &sc);
                mtos_free(tmp_time_str);
            }
            //memcpy(p_time, p_read, line_len);
            continue;
        }

        if(got_timestamp)
        {
            //next is content
            got_timestamp = FALSE;
            line_num++;
        }
        else
            continue;
        int cur_sub_size = strlen(p_time) + strlen(p_content);
        int avail = 0;
        while(1)
        {
            mtos_sem_take((os_sem_t *)(&(p_subtitle->fifo_mutex)), 0);
            avail = get_freesize_sub_fifo_kw(p_subtitle->p_fifo_handle);
            TASK_STATE task_status = p_subtitle->task_status;
            mtos_sem_give((os_sem_t *)(&(p_subtitle->fifo_mutex)));
            if(task_status == EXITING)
                return SUCCESS;
            if(avail > (cur_sub_size + 8))
                break;
            SUBTITLE_API_ERROR("[%s][ERROR][%d] p_fifo_handle is full cannot write!!!!!!!!! wait for read!\n", __func__, __LINE__);
            mtos_task_sleep(200);
        }

        SUBT_DATA * subt_data = (SUBT_DATA *) mtos_malloc(sizeof(SUBT_DATA));
        subt_data->data = mtos_malloc(cur_sub_size + 8);
        subt_data->pts = start_time_ms/1000;
        SUBTITLE_API_DEBUG("number[%d] time str is [%s] pts[%d]\n", line_num, p_time, subt_data->pts);
        SUBTITLE_API_DEBUG("number[%d] content is [%s]\n", line_num, p_content);
        subt_data->size = cur_sub_size;
        subt_data->data[0] = ((subt_data->size) & 0xff000000) >> 24;
        subt_data->data[1] = ((subt_data->size) & 0xff0000) >> 16;
        subt_data->data[2] = ((subt_data->size) & 0xff00) >> 8;
        subt_data->data[3] = ((subt_data->size)) & 0xff;
        subt_data->data[4] = ((subt_data->pts) & 0xff000000) >> 24;
        subt_data->data[5] = ((subt_data->pts) & 0xff0000) >> 16;
        subt_data->data[6] = ((subt_data->pts) & 0xff00) >> 8;
        subt_data->data[7] = ((subt_data->pts)) & 0xff;

        strcpy(subt_data->data + 8, p_time);
        memcpy(subt_data->data + 8 + strlen(p_time), p_content, strlen(p_content));
        mtos_sem_take((os_sem_t *)(&(p_subtitle->fifo_mutex)), 0);
        write_sub_fifo_kw(p_subtitle->p_fifo_handle, subt_data->data, cur_sub_size + 8);
        mtos_sem_give((os_sem_t *)(&(p_subtitle->fifo_mutex)));

        mtos_free(subt_data->data);
        mtos_free(subt_data);
        mtos_task_sleep(10);
    }


    return SUCCESS;
}


static  void  proc_subt_cmd_task(void * p_param)
{
	SUBTITLE_API_DEBUG("[%s] start start ...\n ", __func__);

    PROC_SUBT_HDL_T * p_subtitle  =  (PROC_SUBT_HDL_T *)p_param;
	p_subtitle->task_status = RUNNING;

    ufs_file_t subtitle_fp;
    memset(&subtitle_fp, 0, sizeof(ufs_file_t));

    unsigned short  path_tmp[256]={0};
    unsigned short  *p_file_name1 = Convert_Utf8_To_Unicode((unsigned char *)p_subtitle->file_name, path_tmp);
    int  file_size = 0;
    u8 * file_buffer = NULL;

    if (NULL == p_file_name1) {
        p_subtitle->task_status = EXIT_END;
#ifndef __LINUX__
        mtos_task_exit();
#endif
    }

    int ret = ufs_open(&subtitle_fp, p_file_name1,  UFS_READ);

    if (ret == FR_OK) {
        file_size = subtitle_fp.file_size;
        file_buffer = (char *)mtos_malloc(file_size + 1);
        memset(file_buffer, 0, file_size);
        int r = 0;
        ufs_read(&subtitle_fp, file_buffer, file_size, (u32 *)&r);
        ufs_close(&subtitle_fp);
    } else {
        SUBTITLE_API_ERROR("[%s][ERROR][%d] open FAILED, file name[%s]!!!!!!!!!\n", __func__, __LINE__, p_subtitle->file_name);
        p_subtitle->task_status = EXIT_END;
#ifndef __LINUX__
        mtos_task_exit();
#endif
    }
    SUBTITLE_API_DEBUG("input subtitle len:%d\n", file_size);

    p_subtitle->file_size = file_size;
    p_subtitle->file_buffer = file_buffer;
    if(strstr(p_subtitle->file_name, ".srt"))
        p_subtitle->type = SUBT_SYS_SRT;
    else if(strstr(p_subtitle->file_name, ".ass") || strstr(p_subtitle->file_name, ".ssa"))
        p_subtitle->type = SUBT_SYS_SSA;
    else if(strstr(p_subtitle->file_name, ".smi"))
        p_subtitle->type = SUBT_SYS_SMI;

    if(p_subtitle->type == SUBT_SYS_SRT)
    {
        srt_data_parse(p_subtitle, file_buffer, file_size);
    }
    else if(p_subtitle->type == SUBT_SYS_SSA)
    {
        ssa_data_parse(p_subtitle, file_buffer, file_size);
    }
    else if(p_subtitle->type == SUBT_SYS_SMI)
    {
        smi_data_parse(p_subtitle, file_buffer, file_size);
    }

    p_subtitle->task_status = EXIT_END;

#ifndef __LINUX__
     mtos_task_exit();
#endif

}

int file_out_subtile_start(int subtitleId, File_Out_Subtile_T *p_subt)
{
    if(p_subt == NULL)
    {
        SUBTITLE_API_ERROR("%s: molloc File_Out_Subtile_T first!!!!!\n", __func__);
        return -1;
    }
    if(subtitleId >= p_subt->extra_subt_cnt)
    {
        SUBTITLE_API_ERROR("%s: ERROR wrong extra id[%d]!!\n", __func__, (subtitleId+p_subt->inter_subt_cnt));
        return -1;
    }

    if (p_subt->p_subt_task_hdl) {
        SUBTITLE_API_LOG("[%s][WARNING][%d] p_fifo_handle alread alloced, donoting!!!!!!!!!\n", __func__, __LINE__);
        return -1;
    }

    char file_name[MAX_FILE_PATH_LEN];
    memset(file_name, 0, MAX_FILE_PATH_LEN);
    strcpy(file_name, g_cur_file_dir);
    if(strlen(file_name) > 3)
        *(file_name+strlen(file_name)) = '\\';
    strcat(file_name, g_cur_sbut_list[subtitleId]);
    SUBTITLE_API_DEBUG("%s: enter enter . id[%d] g_cur_file_dir[%s] file[%s]!!!\n", __func__, subtitleId, g_cur_file_dir, file_name);
    BOOL mtos_ret = FALSE;
    PROC_SUBT_HDL_T * p_proc_subt_hdl = NULL;


    p_proc_subt_hdl = (PROC_SUBT_HDL_T *)mtos_malloc(sizeof(PROC_SUBT_HDL_T));
    if(p_proc_subt_hdl)
    {
        memset(p_proc_subt_hdl,0,sizeof(PROC_SUBT_HDL_T));
    }
    else
    {
        SUBTITLE_API_ERROR("[%s] [ERROR] fail malloc ...\n",__func__);
        return -1;
    }

    mtos_ret = mtos_sem_create(&(p_subt->out_sub_fifo_mutex), TRUE);
    MT_ASSERT(mtos_ret == TRUE);

    p_subt->p_out_sub_fifo_handle = init_fifo_kw(NULL, SUB_FIFO_LEN);
    if(!p_subt->p_out_sub_fifo_handle)
    {
        SUBTITLE_API_ERROR("[%s][ERROR][%d] init_fifo_kw failed!!!!!!!!!\n", __func__, __LINE__);
        return -1;
    }

    p_proc_subt_hdl->file_name = (char*)mtos_malloc(strlen(file_name) + 1);
    memset(p_proc_subt_hdl->file_name, 0, strlen(file_name) + 1);
    strcpy(p_proc_subt_hdl->file_name, file_name);
    p_proc_subt_hdl->task_status = STOPPED;
    p_proc_subt_hdl->task_prio = p_subt->thread_priority;
    p_proc_subt_hdl->fifo_mutex = p_subt->out_sub_fifo_mutex;
    p_proc_subt_hdl->p_fifo_handle = p_subt->p_out_sub_fifo_handle;
    int ret = 0;

#ifdef __LINUX__
    ret = mtos_task_create((u8 *)"do out subtitle",
                   proc_subt_cmd_task,p_proc_subt_hdl, 0, 0, 0);
#else//for uc-cos system
    ret = mtos_task_create((u8 *)"do out subtitle",
                   proc_subt_cmd_task, p_proc_subt_hdl, p_subt->thread_priority, (u32 *)p_subt->p_mem_start, p_subt->stack_size);
    if(ret != TRUE)
    {
        SUBTITLE_API_ERROR("\n %s %d mtos_task_create faied, ret:%d\n",__func__,__LINE__,ret);
    }
    MT_ASSERT(ret == TRUE);

    while(p_proc_subt_hdl->task_status == STOPPED)
    {
        mtos_task_sleep(50);
    }
#endif

    p_subt->p_subt_task_hdl = (void *)p_proc_subt_hdl;

    SUBTITLE_API_ERROR("[%s] end end!!\n",__func__);
    return 0;
}

int file_out_subtile_seek(File_Out_Subtile_T *p_subt, int seconds)
{
    SUBTITLE_API_DEBUG("%s: enter enter seconds[%d]!!!\n", __func__, seconds);
    if(p_subt->p_subt_task_hdl == NULL)
    {
        SUBTITLE_API_ERROR("%s:ERROR, p_subt_task_hdl is NULL!!!\n", __func__);
        return -1;
    }
    PROC_SUBT_HDL_T * p_subtitle  =  (PROC_SUBT_HDL_T *)p_subt->p_subt_task_hdl;
    if (p_subtitle->p_fifo_handle == NULL ) {
        SUBTITLE_API_ERROR("p_fifo_handle is NULL!!!\n");
        return -1;
    }

    if(p_subtitle->task_status == RUNNING)
    {
        p_subtitle->seek_second = seconds;
        p_subtitle->task_status = SUB_SEEK;

        do{
    		mtos_task_sleep(50);
            if(p_subtitle->task_status == RUNNING)
                break;
    		SUBTITLE_API_DEBUG("wait seek finished !!!\n");
    	}while(1);
    }
    else
    {
        SUBTITLE_API_ERROR("%s: ERROR, seek but state is %d!!!\n", p_subtitle->task_status);
    }

    return 0;
}

void stop_subt_cmd_task(void * hdl)
{
    SUBTITLE_API_DEBUG("%s: enter enter !!!\n", __func__);
    PROC_SUBT_HDL_T * p_subtitle  =  (PROC_SUBT_HDL_T *)hdl;
    if (p_subtitle->p_fifo_handle == NULL ) {
        SUBTITLE_API_DEBUG("p_fifo_handle is NULL, exit!!!\n");
        return;
    }
    if(p_subtitle->task_status == RUNNING)
    {
        p_subtitle->task_status = EXITING;

        do{
    		mtos_task_sleep(50);
            if(p_subtitle->task_status == EXIT_END)
            {
                break;
            }
    		SUBTITLE_API_DEBUG("wait stop task exit  88888888888 !!!\n");
    	}while(1);
    }
    else
    {
        SUBTITLE_API_DEBUG("%s: task_status[%d]!!!\n", __func__, p_subtitle->task_status);
    }

    if(p_subtitle->file_buffer)
    {
        mtos_free(p_subtitle->file_buffer);
        p_subtitle->file_buffer = NULL;
    }

    if(p_subtitle->file_name)
    {
        mtos_free(p_subtitle->file_name);
        p_subtitle->file_name = NULL;
    }
    return;
}

int file_out_subtile_stop(File_Out_Subtile_T *p_subt)
{
    SUBTITLE_API_DEBUG("%s: enter enter !!!\n", __func__);
    if(p_subt->p_subt_task_hdl == NULL)
    {
        SUBTITLE_API_ERROR("%s:ERROR, p_subt_task_hdl is NULL!!!\n", __func__);
        return -1;
    }

    stop_subt_cmd_task(p_subt->p_subt_task_hdl);

    if (p_subt->p_out_sub_fifo_handle) {
        deinit_fifo_kw(p_subt->p_out_sub_fifo_handle);
        p_subt->p_out_sub_fifo_handle = NULL;
    }
    mtos_sem_destroy(&(p_subt->out_sub_fifo_mutex), 0);

    mtos_free(p_subt->p_subt_task_hdl);
    p_subt->p_subt_task_hdl = NULL;

    int i=0;
    for(i=0; i<MAX_EXTRA_SUBT_NUM; i++)
    {
        memset(g_cur_sbut_list[i], 0, MAX_FILE_NAME_LEN);
    }

    return 0;
}

int file_get_extra_subt(File_Out_Subtile_T *p_subt, unsigned char * subt_buf, int * pts)
{
    int size_sub = 0;
    int size = 0;
    demux_stream_t * ds_sub = NULL;
    unsigned char p_data[2048];
    if (p_subt->p_out_sub_fifo_handle == NULL ) {
        SUBTITLE_API_ERROR("p_out_sub_fifo_handle is NULL!!!\n");
        return -1;
    }

    mtos_sem_take((os_sem_t *)(&(p_subt->out_sub_fifo_mutex)), 0);
    size = read_sub_fifo_kw(p_subt->p_out_sub_fifo_handle, p_data);

    if (size == 0) {
        mtos_sem_give((os_sem_t *)(&(p_subt->out_sub_fifo_mutex)));
        return 0;
    } else {
        size_sub = (p_data[0] << 24) + (p_data[1] << 16) + (p_data[2] << 8) + p_data[3];
        *pts = (p_data[4] << 24) + (p_data[5] << 16) + (p_data[6] << 8) + p_data[7];

        if (size_sub > 2048) {
            size_sub = 2048;
        }

        memcpy(subt_buf, (p_data + 8), size_sub);
    }

    mtos_sem_give((os_sem_t *)(&(p_subt->out_sub_fifo_mutex)));
    return size_sub;
}

#define GS_FN_MAXPATH  (260)
//file path example: C:\1/neiwaiguazimu.mkv  C:\neiwaiguazimu.mkv
int get_extra_subt_num(File_Out_Subtile_T *p_subt, unsigned char *file_path, int inter_cnt)
{
    SUBTITLE_API_DEBUG("[%s]: try to get file:%s\n", __func__, file_path);
    int extra_subt_num = 0;
    int i = 0;
    for(i=0; i<MAX_EXTRA_SUBT_NUM; i++)
    {
        memset(g_cur_sbut_list[i], 0, MAX_FILE_NAME_LEN);
    }
    memset(g_cur_file_dir, 0, MAX_FILE_PATH_LEN);

    tchar_t filename_src[GS_FN_MAXPATH] = {0};
    u32 out_len = sizeof(filename_src);
    char  play_filename[MAX_FILE_PATH_LEN] = {0};
    char  tmp_filename[MAX_FILE_PATH_LEN] = {0};
    char  dir_path[MAX_FILE_PATH_LEN] = {0};
    u8 *p_read = file_path;
    u8 *p_start = file_path;
    u8 *p_end = NULL;
    u8 *p_end_2 = NULL;
    for(i=1; i<strlen(file_path); i++)
    {
        if(*p_read == '\\')
            p_end = p_read;//look for last '\'
        if(*p_read == '/')
            p_end_2 = p_read;//look for last '/'
        p_read++;
    }
    if(p_end != NULL)
    {
        memset(dir_path, 0, MAX_FILE_PATH_LEN);
        if(p_end_2 != NULL)
        {
            memcpy(dir_path, p_start, (p_end_2-p_start+1));
            p_start = p_end_2+1;
        }
        else
        {
            memcpy(dir_path, p_start, (p_end-p_start+1));
            p_start = p_end+1;
        }
        p_read = dir_path;
        for(i=0; i<strlen(dir_path); i++)
        {
            if(*p_read == '/')
                *p_read = '\\';
            p_read++;
        }
        if(strlen(dir_path) > 3)
        {
            *(dir_path + strlen(dir_path) - 1) = '\0';
        }
        p_read = file_path + strlen(file_path) - 1;
        for(i=strlen(file_path); i>0; i--)
        {
            if(*p_read == '.')
                p_end = p_read;//look for last .
            p_read--;
        }
        memset(play_filename, 0, MAX_FILE_PATH_LEN);
        memcpy(play_filename, p_start, (p_end-p_start+1));
        ufs_utf8_to_utf16(dir_path, filename_src, &out_len);
    }
    SUBTITLE_API_DEBUG("[%s]: input info dir_path[%s] play_filename[%s]\n", __func__, dir_path, play_filename);

    ufs_dir_t p_dj_src = {0,};
    ufs_fileinfo_t p_fno = {{0},};
    if (ufs_opendir(&p_dj_src, filename_src))
	{
        SUBTITLE_API_ERROR("open dir src[%s] error\n", dir_path);
        return -1;
	}

    u32 len = 512;
    int cnt = 0;
    while(ufs_readdir(&p_dj_src, &p_fno) == 0)
    {
        len = 512;
        if (p_fno.fileinfo.lfname[0] | p_fno.fileinfo.lfname[1]){
           cnt = ufs_utf16_to_utf8((u16 *)p_fno.fileinfo.lfname, 0, tmp_filename, &len);
         }else{
           cnt = ufs_utf16_to_utf8((u16 *)p_fno.fileinfo.fname, 0, tmp_filename, &len);
         }
         //SUBTITLE_API_DEBUG("[%s]: ufs_readdir filename:%s\n", __func__, tmp_filename);

         if(strstr(tmp_filename, play_filename))
         {
            if(strstr(tmp_filename, ".srt") || strstr(tmp_filename, ".ass") || strstr(tmp_filename, ".ssa") || strstr(tmp_filename, ".smi"))
            {
                if(strlen(tmp_filename) > MAX_FILE_NAME_LEN)
                {
                    SUBTITLE_API_ERROR("[%s]: ERROR, cannot save, subtitile file name more than %d:%s\n", __func__, MAX_FILE_NAME_LEN, tmp_filename);
                    continue;
                }
                memcpy(g_cur_sbut_list[extra_subt_num], tmp_filename, strlen(tmp_filename));
                SUBTITLE_API_DEBUG("[%s]: haha: find subtitile to play file, num[%d]:%s\n", __func__, extra_subt_num, g_cur_sbut_list[extra_subt_num]);
                extra_subt_num++;
            }
         }

         if(extra_subt_num >= MAX_EXTRA_SUBT_NUM)
            break;
    }

    ufs_closedir(&p_dj_src);


    if(extra_subt_num > 0)
    {
        strcpy(g_cur_file_dir, dir_path);
        p_subt->extra_subt_cnt = extra_subt_num;
        p_subt->cur_extra_subt_id = 0;
        return p_subt->extra_subt_cnt;
    }

    return 0;
}


char *get_extra_subt_name(File_Out_Subtile_T *p_subt, int subtitleId)
{
    if(subtitleId >= p_subt->extra_subt_cnt)
    {
        SUBTITLE_API_ERROR("%s: ERROR wrong extra id[%d]!!\n", __func__, (subtitleId+p_subt->inter_subt_cnt));
        return NULL;
    }

    SUBTITLE_API_DEBUG("%s: enter enter . id[%d] file[%s]!!!\n", __func__, subtitleId, g_cur_sbut_list[subtitleId]);
    return g_cur_sbut_list[subtitleId];
}


//#endif


