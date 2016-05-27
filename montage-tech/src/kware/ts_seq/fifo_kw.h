/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __FIFO_KW_H__
#define __FIFO_KW_H__

/*!
  bool type
  */
  //#define BOOL unsigned char

/*!
 xxx
  */
void  *          init_fifo_kw(char *p_fifo, int fifoSize);

/*!
  xxx
  */
void               deinit_fifo_kw(void * p_handle);

/*!
  xxx
  */
int                 write_fifo_kw(void * p_handle,char * p_data, long size);

/*!
 xxxx
  */
int                 read_fifo_kw(void *  p_handle,char * p_data, int size);
/*!
 xxxx
  */
void                clear_fifo_kw(void *  p_handle);
/*!
 xxxx
  */
void                clear_pb_fifo_kw(void *  p_handle);
/*!
 xxxx
  */
int                   tell_fifo_kw(void *  p_handle);
/*!
 xxxx
  */
int                   tell_pb_fifo_kw(void *  p_handle);
/*!
 xxxx
  */
int                   tell_pb_fifo_kw_lite(void *  p_handle);
/*!
 xxxx
  */
int                 read_fifo_kw_lite(void *  p_handle,char * p_data, int size);
/*!
 xxxx
  */
int                   get_freesize_sub_fifo_kw(void *  p_handle);
/*!
 xxx
  */
void  *          init_segfifo_kw(char *p_fifo, int fifoSize);
/*!
 xxx
  */
void  * add_segfifo_kw(void * handle,char *p_fifo, int fifoSize);
/*!
  xxx
  */
void   deinit_segfifo_kw(void * handle,int free_seg);
/*!
  xxx
  */
int                 write_segfifo_kw(void * p_handle,char * p_data, long size);
/*!
 xxxx
  */
int                 read_segfifo_kw(void *  p_handle,char * p_data, int size);
/*!
 xxxx
  */
void                clear_segfifo_kw(void *  p_handle);
/*!
 xxxx
  */
int                   tell_segfifo_kw(void *  p_handle);

#endif
