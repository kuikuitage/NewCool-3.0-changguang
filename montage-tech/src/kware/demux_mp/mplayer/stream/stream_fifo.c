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
#ifdef USE_PB_FIFO	
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
#include "ts_sequence.h"


extern int num_fill_buffer;
static struct stream_priv_s {
  char* filename;
  char *filename2;
} stream_priv_dflts = {
  NULL, NULL
};

#define ST_OFF(f) M_ST_OFF(struct stream_priv_s,f)
/// URL definition
static const m_option_t stream_opts_fields[] = {
  {"string", ST_OFF(filename), CONF_TYPE_STRING, 0, 0 ,0, NULL},
  {"filename", ST_OFF(filename2), CONF_TYPE_STRING, 0, 0 ,0, NULL},
  { NULL, NULL, 0, 0, 0, 0,  NULL }
};
static const struct m_struct_st stream_opts = {
  "file",
  sizeof(struct stream_priv_s),
  &stream_priv_dflts,
  stream_opts_fields
};

static  unsigned short    unicode_filename[256];//peacer add

static unsigned short  *   ConvertUtf8ToUnicode(unsigned char * putf8) //peacer add 
{ 
	int i = 0,k = 0,len = 0; 

       memset(unicode_filename,0,256*2);
	   
	len = strlen(putf8); 
	unsigned short * result = unicode_filename; 
       OS_PRINTF("%s %d!!!!!!!!!\n",putf8,len);
       
	if (result) 
	{ 
		k = 0; 

		for (i=0;i<len;) 
		{ 
                     if(k >= 256)
                     {
                           OS_PRINTF("[%s] file name is too long !!!!!!!\n",__func__);
	                    return NULL;
                     }
                     
			if (0 == (0x80 & putf8[i])) 
			{//one byte 
				result[k++] =  putf8[i++]; 
				continue; 
			} 

			else if (0xe0 == (0xe0 & putf8[i])) 
			{//three byte 
				result[k] = (putf8[i] & 0x1F) << 12; 
				result[k] |= (putf8[i+1] & 0x3F) << 6; 
				result[k++] |= (putf8[i+2] & 0x3F); 

				i += 3; 

				continue; 
			} 

			else if (0xc0 == (0xc0 & putf8[i])) 
			{//two byte 
				result[k] = (putf8[i] & 0x1F) << 6; 
				result[k++] |= (putf8[i+1] & 0x3F); 

				i += 2; 
			} 
                    else
                        {
                        return NULL;
                        }
		} 

		result[k++] = 0; 

	} 
       
	return result; 

} 

static int fill_buffer(stream_t *s, char* buffer, int max_len){
#if 0//ndef USE_PB_FIFO
  #ifdef __LINUX__
  int r = read(s->fd,buffer,max_len);
  #else
  int r = 0;  
  ufs_read(s->fd, buffer, max_len, &r);
  #endif
  // We are certain this is EOF, do not retry
  if (max_len && r == 0) s->eof = 1;
  return (r <= 0) ? -1 : r;
  #else
  	int rd_num=0;
  	int read_retry_cnt = 5;
	FILE_SEQ_T * pFileSeq =  file_seq_get_instance();
	fifo_type_t * p_pb_fifo_handle = pFileSeq->p_pb_fifo_handle;
	do{
	mtos_sem_take((os_sem_t *)(&(pFileSeq->pb_fifo_mutex)), 0);
	if(pFileSeq->p_pb_fifo_handle)
		rd_num = read_pb_fifo_kw(pFileSeq->p_pb_fifo_handle,buffer, max_len);
	else
		break;
	mtos_sem_give((os_sem_t *)(&(pFileSeq->pb_fifo_mutex)));
	//OS_PRINTF("[%s] read fifo %d/%d ! r[%lx], w[%lx]\n", __func__,rd_num,max_len,p_pb_fifo_handle->rd_pos,p_pb_fifo_handle->wr_pos);
	
	if(rd_num > 0){
		if(read_retry_cnt != 5)
			OS_PRINTF("[%s] read %d from fifo, retry %d \n", __func__,rd_num,read_retry_cnt);
		break;
	}
		
	mtos_task_sleep(100);
	read_retry_cnt--;
	OS_PRINTF("[%s] no data(%d) in fifo, wait and retry %d \n", __func__,rd_num,read_retry_cnt);
	}while(read_retry_cnt);

	return  (rd_num <= 0) ? -1 : rd_num;
  #endif
}

static int fill_buffer2(stream_t *s, char* buffer, int max_len){
#if 0//ndef USE_PB_FIFO
  #ifdef __LINUX__
  int r = read(s->fd,buffer,max_len);
  #else
  int r = 0;  
  ufs_read(s->fd, buffer, max_len, &r);
  #endif
  // We are certain this is EOF, do not retry
  if (max_len && r == 0) s->eof = 1;
  return (r <= 0) ? -1 : r;
  #else
  	int rd_num=0;
  	int read_retry_cnt = 5;
	FILE_SEQ_T * pFileSeq =  file_seq_get_instance();
	fifo_type_t * p_pb_fifo_handle = pFileSeq->p_pb_fifo_handle;
	do{

      if (pFileSeq->m_play_state == FILE_SEQ_EXIT) {
   	        OS_PRINTF("[%s] exit from fifo fill_buffer...\n", __func__);
   	        break;
   	    }
		    
	rd_num = read_pb_fifo_kw_lite(pFileSeq->p_pb_fifo_handle,buffer, max_len);

	//OS_PRINTF("[%s] read fifo %d/%d ! r[%lx], w[%lx]\n", __func__,rd_num,max_len,p_pb_fifo_handle->rd_pos,p_pb_fifo_handle->wr_pos);
	
	if(rd_num > 0){
		num_fill_buffer += rd_num;
		break;
	}
		
	mtos_task_sleep(100);
	read_retry_cnt--;
	OS_PRINTF("[%s] no data(%d) in fifo, wait and retry %d \n", __func__,rd_num,read_retry_cnt);
	}while(read_retry_cnt);

	
	return  (rd_num <= 0) ? -1 : rd_num;
  #endif
}

static int write_buffer(stream_t *s, char* buffer, int len) {
  int r;
  int wr = 0;
  while (wr < len) {
   #ifdef __LINUX__
    r = write(s->fd,buffer,len);
    if (r <= 0)
      return -1;
   #else
      r = 0;
      if(ufs_write(s->fd, buffer, len, &r) != FR_OK)
        return -1;
   #endif
    wr += r;
    buffer += r;
  }
  return len;
}

static int seek(stream_t *s,off_t newpos) {
OS_PRINTF("[%s] stream_fifo---s->pos:0x%llx, newpos:0x%llx\n", __func__,s->pos,newpos);
//return 0;
#if 0
  s->pos = newpos;
  #ifdef __LINUX__
  if(lseek(s->fd,s->pos,SEEK_SET)<0) {
    s->eof=1;
    return 0;
  }
  #else
    //OS_PRINTF("start to seek---%d---------\n", s->pos);
    if(ufs_lseek(s->fd, s->pos, 0) != FR_OK){
      OS_PRINTF("seek fail %lld\n",s->pos);
      s->eof=1;
      return 0;
    }
   // OS_PRINTF("end to seek------------\n");
  #endif
  return 1;
#else
	//OS_PRINTF("start to seek---%d---------\n", s->pos);
    int fifo_avaiable = 0;
    int pos_delta;
    FILE_SEQ_T * pFileSeq =  file_seq_get_instance();
    fifo_type_t * p_pb_fifo_handle = pFileSeq->p_pb_fifo_handle;
    unsigned long oldpos = pFileSeq->pbfifo_stream_pos;
    unsigned long fifo_rd_pos_bak = p_pb_fifo_handle->rd_pos;

    if(newpos == s->pos){
    	OS_PRINTF("[%s] newpos == s->pos(%llx), return 1\n",__func__,newpos);
	return 1;
    }
    
    if(newpos > oldpos){
    	pFileSeq->pbfifo_http_need_reconnect = TRUE;
    	pFileSeq->pbfifo_connect_off = newpos;
    	OS_PRINTF("[%s] seek forward! oldpos(0x%lx) < newpos(0x%llx)\n",__func__,oldpos, newpos);
    }
    else{
	fifo_avaiable = tell_pb_fifo_kw_lite(p_pb_fifo_handle);
	pos_delta = oldpos - newpos;
	if(pos_delta > fifo_avaiable){
		pFileSeq->pbfifo_http_need_reconnect = TRUE;
		pFileSeq->pbfifo_connect_off = newpos;
		OS_PRINTF("[%s] seek backward! newpos(0x%llx) < oldpos(0x%x) \n",__func__,newpos,oldpos);
	}
	else{
		p_pb_fifo_handle->rd_pos += (fifo_avaiable - pos_delta);
		pFileSeq->pbfifo_stream_pos = newpos;
		s->pos = newpos;
		OS_PRINTF("[%s] seek in FIFO ok! fifo_rd_pos: %lx -> %lx\n", __func__,fifo_rd_pos_bak, p_pb_fifo_handle->rd_pos);
		return 1;
	}
    }
	//wait download data task to reconnect
	pFileSeq->pbfifo_http_state = HTTP_RECONNECT;
	OS_PRINTF("[%s] this seek need http reconnect(%ld, 0x%lx), wait...\n",__func__,pFileSeq->pbfifo_connect_off, pFileSeq->pbfifo_connect_off);
	do{
		    if (pFileSeq->m_play_state == FILE_SEQ_EXIT) {
		        OS_PRINTF("[%s] exit from fifo seek...\n", __func__);
		        return 0;
		    }
		    
		mtos_task_sleep(200);
		if(pFileSeq->pbfifo_http_state == HTTP_RECV){
			OS_PRINTF("[%s] reconnect ok! newpos: %llx    rd_pos: %llx\n", __func__,newpos, p_pb_fifo_handle->rd_pos);
			break;
		}
		if(pFileSeq->pbfifo_http_state == HTTP_STOP){
			OS_PRINTF("[%s] reconnect ok and stopped!! newpos: %llx    rd_pos: %llx\n", __func__,newpos, p_pb_fifo_handle->rd_pos);
			break;
		}
		if(pFileSeq->pbfifo_http_state == HTTP_FAILED){
			OS_PRINTF("[%s] reconnected failed, seek fail %llx, return 0\n",__func__, newpos);
      			s->eof=1;
      			return 0;

		}
		OS_PRINTF("[%s] pbfifo_http_state:%d, wait\n",__func__,pFileSeq->pbfifo_http_state);
			
	}while(1);
	s->pos = newpos;

#if 0
	while(1){
		OS_PRINTF("[%s] test seek ok!~~~~\n",__func__,newpos);
	}
#endif

	return 1;
			
#endif
}

static int seek_forward(stream_t *s,off_t newpos) {
  OS_PRINTF("[%s] stream_fifo---s->pos:0x%llx, newpos:0x%llx\n", __func__,s->pos,newpos);
  return 0;
  
  if(newpos<s->pos){
    mp_msg(MSGT_STREAM,MSGL_INFO,"Cannot seek backward in linear streams!\n");
    return 0;
  }
  while(s->pos<newpos){
    int len=s->fill_buffer(s,s->buffer,io_stream_buffer_size);
    if(len<=0){ s->eof=1; s->buf_pos=s->buf_len=0; break; } // EOF
    s->buf_pos=0;
    s->buf_len=len;
    s->pos+=len;
  }
  return 1;
}

static int control(stream_t *s, int cmd, void *arg) {

OS_PRINTF("[%s] stream_fifo---start\n", __func__);

#if 0
  switch(cmd) {
    case STREAM_CTRL_GET_SIZE: {
      off_t size;

      #ifdef __LINUX__
      size = lseek(s->fd, 0, SEEK_END);
      lseek(s->fd, s->pos, SEEK_SET);
      #else
      size = ((ufs_file_t *)s->fd)->file_size;
      ufs_lseek(s->fd, s->pos, 0);
      #endif
      if(size != (off_t)-1) {
        *(uint64_t*)arg = size;
        return 1;
      }
    }
  }
  return STREAM_UNSUPPORTED;
#else
	switch(cmd) {
	    case STREAM_CTRL_GET_SIZE: {
	    off_t size;
            size = s->end_pos;

            if (size >= 0) {
                *(off_t *)arg = size;
                 OS_PRINTF("[%s] stream_fifo get size: %d ,return 1\n",__func__,size);
                return 1;
            }

            break;
	    }
	  }
	  return STREAM_UNSUPPORTED;
#endif
}

static int open_f(stream_t *stream,int mode, void* opts, int* file_format) {
#ifndef __LINUX__
  ufs_file_t  *f =malloc(sizeof(ufs_file_t));//= stream->fd;
  unsigned char filename1[256]={0};
  memset(f,0,sizeof(ufs_file_t));
#else
  int f = 0;
#endif
  mode_t m = 0;
  long long len;
  unsigned char *filename = NULL;
  struct stream_priv_s* p = (struct stream_priv_s*)opts;

  if(mode == STREAM_READ)
    m = O_RDONLY;
  else if(mode == STREAM_WRITE)
    m = O_RDWR|O_CREAT|O_TRUNC;
  else {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[file] Unknown open mode %d\n",mode);
    m_struct_free(&stream_opts,opts);
    return STREAM_UNSUPPORTED;
  }

  if(p->filename)
    filename = p->filename;
  else if(p->filename2)
    filename = p->filename2;
  else
    filename = NULL;
  if(!filename) {
    mp_msg(MSGT_OPEN,MSGL_ERR, "[file] No filename\n");
    m_struct_free(&stream_opts,opts);
    return STREAM_ERROR;
  }
#if 0//ndef USE_PB_FIFO

#if HAVE_DOS_PATHS
  // extract '/' from '/x:/path'
  if( filename[ 0 ] == '/' && filename[ 1 ] && filename[ 2 ] == ':' )
    filename++;
#endif

  m |= O_BINARY;

  if(!strcmp(filename,"-")){
  #ifdef __LINUX__
    if(mode == STREAM_READ) {
      // read from stdin
      mp_msg(MSGT_OPEN,MSGL_INFO,MSGTR_ReadSTDIN);
      f=0; // 0=stdin
#if HAVE_SETMODE
      setmode(fileno(stdin),O_BINARY);
#endif
    } else {
      mp_msg(MSGT_OPEN,MSGL_INFO,"Writing to stdout\n");
      f=1;
#if HAVE_SETMODE
      setmode(fileno(stdout),O_BINARY);
#endif
    }
  #endif
  } else {
#ifdef __LINUX__
      mode_t openmode = S_IRUSR|S_IWUSR;
#ifndef __MINGW32__
      openmode |= S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH;
#endif
      f=open(filename,m, openmode);
    if(f<0) {
      mp_msg(MSGT_OPEN,MSGL_ERR,MSGTR_FileNotFound,filename);
      m_struct_free(&stream_opts,opts);
      return STREAM_ERROR;
    }
#else
    //  ufs_asc2uni(filename,filename1);
      //printf("\n before ufs open filename:%s \n",filename);

      
       unsigned short  path_tmp[256]={0};
       unsigned short * p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)filename,path_tmp);
       if(NULL == p_filename1)
       {
        m_struct_free(&stream_opts,opts);
	  mtos_printk("[%s][ERROR][ERROR]  NULL == p_filename1!!!!!\n",__func__);
        return STREAM_ERROR;
       }
       
       int ret = ufs_open(f, p_filename1, UFS_READ);

      if(ret != FR_OK)
      {
        mtos_printk("[%s][ERROR][ERROR]   UFS OPEN FAIL %d!!!!!!!!!\n",__func__,ret);

        m_struct_free(&stream_opts,opts);
        return STREAM_ERROR;
      }
      printf("\n  ufs open ok f: %x \n",f);
      #ifndef __LINUX__
      ufs_ioctl(f,FS_SEEK_LINKMAP,0 ,0 ,0);   
      #endif
#endif
  }

#ifdef __LINUX__
  len=lseek(f,0,SEEK_END); lseek(f,0,SEEK_SET);
#else
  len = f->file_size; 
  if(ufs_lseek(f, 0, 0) == FR_OK)
    OS_PRINTF("#########@@@@#######\n");
  else
    OS_PRINTF("#########@@1111@@#######\n");
#endif
#ifdef __MINGW32__
  // seeks on stdin incorrectly succeed on MinGW
  if(f==0)
    len = -1;
#endif
  if(len == -1) {
    if(mode == STREAM_READ) stream->seek = seek_forward;
    stream->type = STREAMTYPE_STREAM; // Must be move to STREAMTYPE_FILE
    stream->flags |= MP_STREAM_SEEK_FW;
  } else if(len >= 0) {
    stream->seek = seek;
    stream->end_pos = len;
    stream->type = STREAMTYPE_FILE;
  }

  mp_msg(MSGT_OPEN,MSGL_V,"[file] File size is %"PRId64" bytes\n", (int64_t)len);

//  #ifdef __LINUX__
  stream->fd = f;
  //#endif
  stream->write_buffer = write_buffer;
  stream->control = control;
  #else
  FILE_SEQ_T * pFileSeq =  file_seq_get_instance();
  
  stream->type = STREAMTYPE_STREAM;
  if (strstr(pFileSeq->m_path[0], "avi")) {
        stream->type = STREAMTYPE_FILE;
    }
  stream->fd = -1;
  stream->write_buffer = NULL;
  stream->control = control;
  stream->seek = seek;
  stream->end_pos = pFileSeq->pbfifo_content_len;
#endif
  stream->fill_buffer = fill_buffer2;
  stream->read_chunk = 64*1024;

  m_struct_free(&stream_opts,opts);
  return STREAM_OK;
}

const stream_info_t stream_info_fifo = {
  "File",
  "file",
  "Albeu",
  "based on the code from ??? (probably Arpi)",
  open_f,
  { "fifo:http", "fifo:https",NULL },//// 
  &stream_opts,
  1 // Urls are an option string
};

#endif
