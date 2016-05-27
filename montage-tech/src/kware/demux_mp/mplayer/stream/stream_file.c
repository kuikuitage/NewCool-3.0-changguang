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
#include "config.h"
#ifdef __LINUX__
#include <stdio.h>
#else
#include "mp_func_trans.h"
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#if HAVE_SETMODE
#include <io.h>
#endif
#include "mp_msg.h"
#include "stream.h"
#include "help_mp.h"
#include "m_option.h"
#include "m_struct.h"
extern int num_fill_buffer;
static struct stream_priv_s {
    char * filename;
    char * filename2;
} stream_priv_dflts = {
    NULL, NULL
};
#define ST_OFF(f) M_ST_OFF(struct stream_priv_s,f)
/// URL definition
static const m_option_t stream_opts_fields[] = {
    {"string", ST_OFF(filename), CONF_TYPE_STRING, 0, 0 , 0, NULL},
    {"filename", ST_OFF(filename2), CONF_TYPE_STRING, 0, 0 , 0, NULL},
    { NULL, NULL, 0, 0, 0, 0,  NULL }
};
static const struct m_struct_st stream_opts = {
    "file",
    sizeof(struct stream_priv_s),
    &stream_priv_dflts,
    stream_opts_fields
};
u64 mp_rec_bytes = 0;
static int mp_timeshift_size = 0;
static int file_size = 0;
static int sect_size = 0;
u64 recv_bytes = 0;
int recding_index = 0;
static unsigned char filename1[256] = {0};
static unsigned char basename[512] = {0};
static int rec_index = 1;
static u64 recv_bytes_inter;
int play_to_recpos = 0;

static int dio_mode = 0;
#ifndef __LINUX__
ufs_file_t * pp_fp = NULL;
os_sem_t ts_sem = {0};
#endif
static  unsigned short    unicode_filename[256];//peacer add


static unsigned short   *  ConvertUtf8ToUnicode(unsigned char * putf8) //peacer add
{
    int i = 0, k = 0, len = 0;
    memset(unicode_filename, 0, 256 * 2);
    len = strlen(putf8);
    unsigned short * result = unicode_filename;
    OS_PRINTF("%s %d!!!!!!!!!\n", putf8, len);

    if (result) {
        k = 0;

        for (i = 0; i < len;) {
            if (k >= 256) {
                OS_PRINTF("[%s] file name is too long !!!!!!!\n", __func__);
                return NULL;
            }

            if (0 == (0x80 & putf8[i])) {
                //one byte
                result[k++] =  putf8[i++];
                continue;
            } else if (0xe0 == (0xe0 & putf8[i])) {
                //three byte
                result[k] = (putf8[i] & 0x1F) << 12;
                result[k] |= (putf8[i + 1] & 0x3F) << 6;
                result[k++] |= (putf8[i + 2] & 0x3F);
                i += 3;
                continue;
            } else if (0xc0 == (0xc0 & putf8[i])) {
                //two byte
                result[k] = (putf8[i] & 0x1F) << 6;
                result[k++] |= (putf8[i + 1] & 0x3F);
                i += 2;
            } else {
                return NULL;
            }
        }

        result[k++] = 0;
    }

    return result;
}
#ifndef __LINUX__
static int open_ts_file(stream_t * s)
{
    unsigned short  path_tmp[256] = {0};
    char * p_filename1;
    char file_new[256] = {0};
    int ret = 0;
	  mtos_sem_take(&ts_sem, 0);
    ret = ufs_close(s->fd);
	mtos_sem_give(&ts_sem);

    if (ret != FR_OK) {
        mtos_printk("UFS close FAIL %d\n", ret);
        return ret ;
    }

    memset(s->fd, 0, sizeof(ufs_file_t));
    memset(file_new, 0, 256);
    sprintf(file_new, "%s%04d%s", basename, rec_index, ".ts");
    memset(path_tmp, 0, 256);
    //mtos_printk("\n%s %d %s\n", __func__, __LINE__, file_new);
    p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)file_new, path_tmp);
    memcpy(filename1, file_new, 256);

    do {
        //{
         mtos_sem_take(&ts_sem, 0);
        ret = ufs_open(s->fd, p_filename1, UFS_READ);
		 mtos_sem_give(&ts_sem);
        mtos_task_sleep(10);
    } while (ret == FR_NO_FILE && (is_file_seq_exit() == FALSE));
  if(ret == FR_OK)
  {
  	mtos_sem_take(&ts_sem, 0);
    ufs_ioctl(s->fd, FS_SEEK_LINKMAP, 0 , 0 , 0);
	mtos_sem_give(&ts_sem);
	}
    return ret;
}
static u8 ufs_lseek_mode(stream_t *s, s64 ofs, u32 frm)
{
int ret = 0;
if(fp_is_timeshift_file() == 1)
{
 s64 new_ofs = 0;
mtos_sem_take(&ts_sem, 0);
//mtos_printk("\n%s %d %d\n",__func__,__LINE__,(int)ofs);
 // rec_index = 1;
         //   ret = open_ts_file(s);
ret = ufs_lseek(s->fd, ofs, frm);
ret =ufs_tell(s->fd, &new_ofs);
//	mtos_printk("\n%s %d %d\n",__func__,__LINE__,(int)new_ofs);
	 mtos_sem_give(&ts_sem);
}

else
{
ret = ufs_lseek(s->fd, ofs, frm);
}
return ret ;
}
static  int wait_for_rec_ready()
{
    while (mp_rec_bytes - recv_bytes <= io_stream_buffer_size && (fp_is_timeshift_file() == 1) && (is_file_seq_exit() == FALSE)) {
        if ((fp_is_timeshift_file() == 1) && fp_is_trick_mode() != 0) {
            // mtos_printk("\n%s %d max len:%d ret :%d   recv %d rec %d  \n", __func__, __LINE__, max_len, r, (int)recv_bytes, (int)mp_rec_bytes);
            play_to_recpos = 1;
        }

        mtos_task_sleep(5);
    }

    return 0;
}

static  int ufs_read_retry(stream_t * s, void * p_buff, u32 btr, u32 * p_br)
{
    int ret = FR_OK;
	mtos_sem_take(&ts_sem, 0);
    ret = ufs_read(s->fd, p_buff, btr, p_br);
mtos_sem_give(&ts_sem);
    if (ret != FR_OK) {
        return ret;
    }

    if (*p_br == 0) {
        //mtos_printk("\n%s %d\n", __func__, __LINE__);
        ret = open_ts_file(s);

        if (ret != FR_OK) {
            free(s->fd);
            s->fd = 0;
            s->eof = 1;
            mtos_printk("UFS OPEN FAIL %d\n", ret);
            return ret;
        }

        //  recv_bytes_inter = recv_bytes;// % mp_timeshift_byte;
        //mtos_printk("\n%s %d %d\n", __func__, __LINE__,(int)recv_bytes_inter);
        ret =  ufs_lseek_mode(s, (s64) recv_bytes_inter, 0);

        if (ret != FR_OK) {
            mtos_printk("UFS OPEN FAIL %d\n", ret);
            return ret;
        }
mtos_sem_take(&ts_sem, 0);
        ret = ufs_read(s->fd, p_buff, btr, p_br);
		mtos_sem_give(&ts_sem);
//mtos_printk("\n%s %d %d\n", __func__, __LINE__,*p_br);
        if (ret != FR_OK) {
            mtos_printk("UFS OPEN FAIL %d\n", ret);
            return ret;
        }
    }

    return ret;
}

static  int ufs_read_func(stream_t * s, void * p_buff, u32 btr, u32 * p_br)
{
    int ret = 0;
    int retry_cnt = 0;
    u64 mp_timeshift_byte = ((u64)mp_timeshift_size) * 1024 ;
    int mp_rec_bytes_inter;
    //if(fp_is_timeshift_file()== 0)
    {
        if (s->fd != 0) {
            wait_for_rec_ready();
			 mtos_sem_take(&ts_sem, 0);
            ret = ufs_read(s->fd, p_buff, btr, p_br);
			 mtos_sem_give(&ts_sem);

            if (ret != FR_OK) {
                if (!((ret == FR_DISK_ERR) && ((*p_br) > 0))) {
                    mtos_printk("UFS OPEN FAIL %d\n", ret);
                   // return 0;
                }
            }
        } else {
            return -1;
        }
    }

    while ((*p_br) == 0 && fp_is_timeshift_file() == 1 && (is_file_seq_exit() == FALSE)) {
        unsigned short  path_tmp[256] = {0};
        unsigned short * p_filename1;
        int file_len = 256;

        //if(recv_bytes>()mp_timeshift_size)
        // mtos_task_sleep(1000);
        if (recv_bytes_inter + btr <= mp_timeshift_byte) {
            ret = ufs_read_retry(s, p_buff, btr, p_br);

            if (ret != FR_OK) {
                if (!((ret == FR_DISK_ERR) && ((*p_br) > 0))) {
                    mtos_printk("UFS OPEN FAIL %d\n", ret);
                    //return 0;
                }
            }
        } else {
            int r_modify = 0;
            int read_len_sec = btr - (int)(mp_timeshift_byte - recv_bytes_inter);

            if (mp_timeshift_byte - recv_bytes_inter > 0) {
                ret =   ufs_read_retry(s, p_buff, (int)(mp_timeshift_byte - recv_bytes_inter), p_br);

                if (ret != FR_OK) {
                    if (!((ret == FR_DISK_ERR) && ((*p_br) > 0))) {
                        mtos_printk("UFS OPEN FAIL %d\n", ret);
                        return 0;
                    }
                }
            }

            {
                char file_new[256] = {0};
                rec_index ++;
                ret = open_ts_file(s);

                if (ret != FR_OK) {
                    free(s->fd);
                    s->fd = 0;
                    s->eof = 1;
                    mtos_printk("UFS OPEN FAIL %d\n", ret);
                    break;
                }

                ret = ufs_lseek_mode(s, 1024, 0);

                if (ret != FR_OK) {
                    free(s->fd);
                    s->fd = 0;
                    s->eof = 1;
                    mtos_printk("UFS OPEN FAIL %d\n", ret);
                    break;
                }
            }
mtos_sem_take(&ts_sem, 0);
            ret = ufs_read(s->fd, p_buff + (int)(mp_timeshift_byte - recv_bytes_inter), read_len_sec, &r_modify);
			mtos_sem_give(&ts_sem);
            recv_bytes+= 1024;

            //mtos_printk("\n%s %d\n",__func__,__LINE__);
            if (ret != FR_OK) {
                if (!((ret == FR_DISK_ERR) && (r_modify > 0))) {
                    // mtos_printk("UFS OPEN FAIL %d\n", ret);
                    return 0;
                }
            }

            (*p_br) += r_modify;
        }

        //mtos_printk("\n%s %d \n", __func__, __LINE__);
        retry_cnt++;

        //mtos_printk("\n%s %d \n", __func__, __LINE__);
        if (retry_cnt > 50) {
            break;
        }

        //mtos_printk("\n%s %d \n", __func__, __LINE__);
    }
	//if(recv_bytes_inter>1024*1024*1000)
	 //  mtos_printk("\n%s %d %d\n",__func__,__LINE__,(int)recv_bytes_inter);
	if((recv_bytes_inter + (*p_br)== mp_timeshift_byte)&& fp_is_timeshift_file() == 1)
				{
				{
                char file_new[256] = {0};
                rec_index ++;
                ret = open_ts_file(s);

                if (ret != FR_OK) {
                    free(s->fd);
                    s->fd = 0;
                    s->eof = 1;
                    return ret;
                }

                ret = ufs_lseek_mode(s, 1024, 0);

                if (ret != FR_OK) {
                    free(s->fd);
                    s->fd = 0;
                    s->eof = 1;
                     return ret;
                }
            }
			  recv_bytes+= 1024;

     
				}
    recv_bytes += (*p_br);
    recv_bytes_inter  = recv_bytes%mp_timeshift_byte;
}
#endif
static int fill_buffer(stream_t * s, char * buffer, int max_len)
{
    int retry_cnt = 0;
#ifdef __LINUX__
    int r = read(s->fd, buffer, max_len);
#else
    int r = 0;
 if(fp_is_timeshift_file() == 1)
    ufs_read_func(s, buffer, max_len, &r);
 else
 	{
 	if( file_size>0)
 		{
 		u64 u_pos = 0;
 		ufs_tell(s->fd,&u_pos);
		if(u_pos>=file_size)
			r = 0;
			else
				ufs_read(s->fd, buffer, max_len, &r);
 		}
	else
 	ufs_read(s->fd, buffer, max_len, &r);
 	}
#endif

    // We are certain this is EOF, do not retry
    if (max_len && r == 0) {
        s->eof = 1;
    }

    return (r <= 0) ? -1 : r;
}
static int write_buffer(stream_t * s, char * buffer, int len)
{
    int r;
    int wr = 0;

    while (wr < len) {
#ifdef __LINUX__
        r = write(s->fd, buffer, len);

        if (r <= 0) {
            return -1;
        }

#else
        r = 0;

        if (ufs_write(s->fd, buffer, len, &r) != FR_OK) {
            return -1;
        }

#endif
        wr += r;
        buffer += r;
    }

    return len;
}
static int seek(stream_t * s, off_t newpos)
{
    //OS_PRINTF("[%s] stream_file---s->pos:0x%llx, newpos:0x%llx\n", __func__, s->pos, newpos);
    s64 pos_seek;

    if (mp_timeshift_size > 0) {
        newpos = MAX(newpos, 1024);
    }
    if(file_size>0&&fp_is_timeshift_file() == 0)
		  newpos = MIN(file_size, newpos);
    s->pos = newpos;
    pos_seek = s->pos;

    if (fp_is_timeshift_file() == 1) {
        int seek_index = 0;
        u64 mp_timeshift_byte = ((u64)mp_timeshift_size) * 1024;

        if (newpos < mp_rec_bytes) {
            s->pos = newpos;
        } else {
            s->pos = mp_rec_bytes;
        }

        recv_bytes = s->pos;
        //
        seek_index = s->pos / mp_timeshift_byte + 1;

        if (seek_index != rec_index) {
            int ret = 0;
            rec_index = seek_index;
            ret = open_ts_file(s);

            if (ret != 0) {
                free(s->fd);
                s->fd = 0;
            }
        }

        recv_bytes_inter =  s->pos % mp_timeshift_byte;
        pos_seek = recv_bytes_inter;
		//mtos_printk("\n%s %d  %d\n", __func__, __LINE__, (int)pos_seek);
        //
    }
  //mtos_task_sleep(10);
    //mtos_printk("\n%s %d  %d\n", __func__, __LINE__, (int)pos_seek);
#ifdef __LINUX__

    if (lseek(s->fd, s->pos, SEEK_SET) < 0) {
        s->eof = 1;
        return 0;
    }

#else

    if (s->fd == 0) {
        s->eof = 1;
        return 0;
    }

    {
        //OS_PRINTF("start to seek---%d---------\n", s->pos);
        if (ufs_lseek_mode(s, pos_seek, 0) != FR_OK) {
            OS_PRINTF("seek fail %lld\n", s->pos);
            s->eof = 1;
            return 0;
        }
    }

    //OS_PRINTF("end to seek------------\n");
#endif
    return 1;
}
static int seek_forward(stream_t * s, off_t newpos)
{
    if (newpos < s->pos) {
        mp_msg(MSGT_STREAM, MSGL_INFO, "Cannot seek backward in linear streams!\n");
        return 0;
    }

    while (s->pos < newpos) {
        int len = s->fill_buffer(s, s->buffer, io_stream_buffer_size);

        if (len <= 0) {
            s->eof = 1;    // EOF
            s->buf_pos = s->buf_len = 0;
            break;
        }

        s->buf_pos = 0;
        s->buf_len = len;
        s->pos += len;
    }

    return 1;
}
static int control(stream_t * s, int cmd, void * arg)
{
    //OS_PRINTF("[%s] stream_file control ~~~~~~~~\n", __func__);
    switch (cmd) {
        case STREAM_CTRL_GET_SIZE: {
            off_t size;
#ifdef __LINUX__
            size = lseek(s->fd, 0, SEEK_END);
            lseek(s->fd, s->pos, SEEK_SET);
#else
            size = ((ufs_file_t *)s->fd)->file_size;
            ufs_lseek_mode(s, s->pos, 0);
#endif

            if (size != (off_t) - 1) {
                *(uint64_t *)arg = size;
                OS_PRINTF("[%s] stream_file get size: %d ,return 1\n", __func__, size);
                return 1;
            }
        }
    }

    return STREAM_UNSUPPORTED;
}
static int open_f(stream_t * stream, int mode, void * opts, int * file_format)
{
#ifndef __LINUX__
    ufs_file_t * f = malloc(sizeof(ufs_file_t)); //= stream->fd;
    recv_bytes = 0;
    memset(f, 0, sizeof(ufs_file_t));
    memset(filename1, 0, 512);
#else
    int f = 0;
#endif
    mode_t m = 0;
    file_size = 0;
    long long len;
    unsigned char * filename = NULL;
    struct stream_priv_s * p = (struct stream_priv_s *)opts;

    if (mode == STREAM_READ) {
        m = O_RDONLY;
    } else if (mode == STREAM_WRITE) {
        m = O_RDWR | O_CREAT | O_TRUNC;
    } else {
        mp_msg(MSGT_OPEN, MSGL_ERR, "[file] Unknown open mode %d\n", mode);
        m_struct_free(&stream_opts, opts);
        return STREAM_UNSUPPORTED;
    }

    if (p->filename) {
        filename = p->filename;
    } else if (p->filename2) {
        filename = p->filename2;
    } else {
        filename = NULL;
    }

    if (!filename) {
        mp_msg(MSGT_OPEN, MSGL_ERR, "[file] No filename\n");
        m_struct_free(&stream_opts, opts);
        return STREAM_ERROR;
    }

#if HAVE_DOS_PATHS

    // extract '/' from '/x:/path'
    if (filename[ 0 ] == '/' && filename[ 1 ] && filename[ 2 ] == ':') {
        filename++;
    }

#endif
    m |= O_BINARY;

    if (!strcmp(filename, "-")) {
#ifdef __LINUX__

        if (mode == STREAM_READ) {
            // read from stdin
            mp_msg(MSGT_OPEN, MSGL_INFO, MSGTR_ReadSTDIN);
            f = 0; // 0=stdin
#if HAVE_SETMODE
            setmode(fileno(stdin), O_BINARY);
#endif
        } else {
            mp_msg(MSGT_OPEN, MSGL_INFO, "Writing to stdout\n");
            f = 1;
#if HAVE_SETMODE
            setmode(fileno(stdout), O_BINARY);
#endif
        }

#endif
    } else {
#ifdef __LINUX__
        mode_t openmode = S_IRUSR | S_IWUSR;
#ifndef __MINGW32__
        openmode |= S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
#endif
        f = open(filename, m, openmode);

        if (f < 0) {
            mp_msg(MSGT_OPEN, MSGL_ERR, MSGTR_FileNotFound, filename);
            m_struct_free(&stream_opts, opts);
            return STREAM_ERROR;
        }

#else
        //  ufs_asc2uni(filename,filename1);
        //printf("\n before ufs open filename:%s \n",filename);
        memcpy(filename1, filename, strlen(filename));
        int ret  = 0;
        unsigned short * p_filename1;
        {
            unsigned short  path_tmp[512] = {0};
            p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)filename, path_tmp);
            ret = ufs_open(f, p_filename1, UFS_READ);
        }

        if (ret != FR_OK) {
            mtos_printk("[%s][ERROR][ERROR]   UFS OPEN FAIL %d!!!!!!!!!\n", __func__, ret);
            m_struct_free(&stream_opts, opts);
            free(f);
            return STREAM_ERROR;
        }

#if 1
        // mtos_printk("\n%s %d \n", __func__, __LINE__);
        {
            int r = 0;
            char pvr_timeshift[512] = {0};
            char pvr_ists[32] = {0};
            recv_bytes_inter = 0;
            ret = ufs_read(f, pvr_timeshift, 512, &r);
            int time_mode;
            mp_timeshift_size = 0;
            file_size = 0;
			sect_size = 0;
            if (ret != FR_OK) {
                mtos_printk("[%s][ERROR][ERROR]   UFS OPEN FAIL %d!!!!!!!!!\n", __func__, ret);
                m_struct_free(&stream_opts, opts);
                return STREAM_ERROR;
            }

            if (strstr(pvr_timeshift, "pvrtimeshift")) {
                memcpy(&mp_timeshift_size, pvr_timeshift + 32, sizeof(int));
                memcpy(&time_mode, pvr_timeshift + 32 + sizeof(int), sizeof(int));
                memcpy(basename, pvr_timeshift + 32 + 2 * sizeof(int), 256);
                memcpy(&file_size, pvr_timeshift + 384, sizeof(int));
		  memcpy(&sect_size, pvr_timeshift + 384+sizeof(int), sizeof(int));
                rec_index = 1;
            }
        }
#endif
        printf("\n  ufs open ok f: %x \n", f);
#ifndef __LINUX__
        ret = ufs_ioctl(f, FS_SEEK_LINKMAP, 0 , 0 , 0);
#endif
#endif
    }

#ifdef __LINUX__
    len = lseek(f, 0, SEEK_END);
    lseek(f, 0, SEEK_SET);
#else

    if (fp_is_timeshift_file() == 1) {
		while(mp_rec_bytes<sect_size*2)
			{
			mtos_task_sleep(100);
			}
        len = mp_rec_bytes;
    } else if (file_size > 0) {
        len = file_size;
    } else {
        len = f->file_size;
    }

    if (ufs_lseek(f, 0, 0) == FR_OK) {
        OS_PRINTF("#########@@@@#######\n");
    } else {
        OS_PRINTF("#########@@1111@@#######\n");
    }

#endif
#ifdef __MINGW32__

    // seeks on stdin incorrectly succeed on MinGW
    if (f == 0) {
        len = -1;
    }

#endif

    if (len == -1) {
        if (mode == STREAM_READ) {
            stream->seek = seek_forward;
        }

        stream->type = STREAMTYPE_STREAM; // Must be move to STREAMTYPE_FILE
        stream->flags |= MP_STREAM_SEEK_FW;
    } else if (len >= 0) {
        stream->seek = seek;
        stream->end_pos = len;
        stream->type = STREAMTYPE_FILE;
    }

    mp_msg(MSGT_OPEN, MSGL_V, "[file] File size is %"PRId64" bytes\n", (int64_t)len);
    //  #ifdef __LINUX__
    stream->fd = f;
    //#endif
    stream->fill_buffer = fill_buffer;
    stream->write_buffer = write_buffer;
    stream->control = control;
    stream->read_chunk = 64 * 1024;
    m_struct_free(&stream_opts, opts);
    return STREAM_OK;
}
const stream_info_t stream_info_file = {
    "File",
    "file",
    "Albeu",
    "based on the code from ??? (probably Arpi)",
    open_f,
    { "file", "", NULL },
    &stream_opts,
    1 // Urls are an option string
};
