#include "sys_types.h"
#include "sys_define.h"
#include "stream.h"

#define  MAX_STREAM_NUM  (10)
extern const stream_info_t stream_info_ffmpeg;
extern const stream_info_t stream_info_http1;
extern const stream_info_t stream_info_rtsp_sip;
extern const stream_info_t stream_info_asf;
extern const stream_info_t stream_info_fifo;

extern stream_info_t  **   get_stream_by_index(int index);
int rtmp_protocol_type = 0;
void register_http_stream()
{
	int i = 0;
	OS_PRINTF("[%s] -----start!\n",__func__);

#if defined(ENABLE_DEMUX_HTTP)
	stream_info_t **   pp_stream = NULL;
	for(i=0; i < MAX_STREAM_NUM; i++)
	{
		pp_stream  = get_stream_by_index(i);
		if(*pp_stream == NULL)
		{
			*pp_stream = &stream_info_ffmpeg;
			OS_PRINTF("[%s] ----[%d]-register ok!\n",__func__,i);
			break;
		}
	}
#else
     mtos_printk("[%s][ERROR][ERROR] --do nothing !!!!!!!!!!\n",__func__);
     mtos_printk("[%s] please config ENABLE_DEMUX_HTTP = Y!!!!!! !!\n",__func__);

#endif

	OS_PRINTF("[%s] -----end !\n",__func__);
}

int register_http_stream_is()
{
	int i = 0;
	OS_PRINTF("[%s] -----start!\n",__func__);
	
#if defined(ENABLE_DEMUX_HTTP)
	stream_info_t **   pp_stream = NULL;

	for(i=0; i < MAX_STREAM_NUM; i++)
	{
		pp_stream  = get_stream_by_index(i);
		if(*pp_stream == &stream_info_ffmpeg)
		{
			return 1;
		}
	}
  #else
     mtos_printk("[%s][ERROR][ERROR] --do nothing !!!!!!!!!!\n",__func__);
     mtos_printk("[%s] please config ENABLE_DEMUX_HTTP = Y!!!!!! !!\n",__func__);

  #endif

  
	OS_PRINTF("[%s] -----end !\n",__func__);
       return 0;
}

#if (!defined(NETWORK_PLAYER_DISABLE))
void register_rtsp_stream()
{
   #if defined(ENABLE_DEMUX_RTSP)
	int i = 0;
	stream_info_t **   pp_stream = NULL;
	for(i=0; i < MAX_STREAM_NUM; i++)
	{
		pp_stream  = get_stream_by_index(i);
		if(*pp_stream == NULL)
		{
			*pp_stream = &stream_info_rtsp_sip;
			OS_PRINTF("[%s] ----[%d]-register ok!\n",__func__,i);
			break;
		}
	}
   #else
     mtos_printk("[%s][ERROR][ERROR] --do nothing !!!!!!!!!!\n",__func__);
   mtos_printk("[%s] please config ENABLE_DEMUX_RTSP = Y!!!!!! !!\n",__func__);
       
   #endif

}
#endif

void register_rtmp_stream()
{
	int i = 0;
	OS_PRINTF("[%s] -----start!\n",__func__);
#if defined(ENABLE_DEMUX_HTTP)
	stream_info_t **   pp_stream = NULL;
	for(i=0; i < MAX_STREAM_NUM; i++)
	{
		pp_stream  = get_stream_by_index(i);
		if(*pp_stream == NULL)
		{
			*pp_stream = &stream_info_ffmpeg;
			OS_PRINTF("[%s] ----[%d]-register ok!\n",__func__,i);
			break;
		}
	}
#else
      mtos_printk("[%s][ERROR][ERROR] --do nothing !!!!!!!!!!\n",__func__);
      mtos_printk("[%s] please config ENABLE_DEMUX_RTMP = Y!!!!!! !!\n",__func__);
       
#endif
	OS_PRINTF("[%s] -----end !\n",__func__);
}


void register_asf_stream()
{
//yliu rm:if open fail;socket not close correct
#if 0
	int i = 0;
	OS_PRINTF("[%s] -----start!\n",__func__);

    stream_info_t **   pp_stream = NULL;
	
	for(i=0; i < MAX_STREAM_NUM; i++)
	{
        pp_stream  = get_stream_by_index(i);
		if(*pp_stream == NULL)
		{
			*pp_stream = &stream_info_asf;
			OS_PRINTF("[%s] ----[%d]-register ok!\n",__func__,i);
			break;
		}
	}

	OS_PRINTF("[%s] -----end !\n",__func__);
#endif
}
//0:rtmp & librtmp ;1:librtmp ; 2 rtmp

void register_rtmp_protocol(int type)
{
	int i = 0;
	OS_PRINTF("[%s] %d\n",__func__,__LINE__);

      rtmp_protocol_type = type;

	OS_PRINTF("[%s] %d\n",__func__,__LINE__);
}
	
void register_fifo_stream()
{
	int i = 0;
	OS_PRINTF("[%s] -----start!\n",__func__);
#if defined(ENABLE_DEMUX_HTTP)
#ifdef USE_PB_FIFO	
	stream_info_t **   pp_stream = NULL;
	for(i=0; i < MAX_STREAM_NUM; i++)
	{
		pp_stream  = get_stream_by_index(i);
		if(*pp_stream == NULL)
		{
			*pp_stream = &stream_info_fifo;
			OS_PRINTF("[%s] ----[%d]-register ok!\n",__func__,i);
			break;
		}
	}
#endif
#else
      mtos_printk("[%s][ERROR][ERROR] --do nothing !!!!!!!!!!\n",__func__);
      mtos_printk("[%s] please config ENABLE_DEMUX_RTMP = Y!!!!!! !!\n",__func__);
       
#endif
	OS_PRINTF("[%s] -----end !\n",__func__);
}
