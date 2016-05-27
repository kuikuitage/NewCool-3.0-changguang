/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef _app_demux_h_
#define _app_demux_h_


#ifdef __cplusplus
extern "C" {
#endif

#define APP_DMX_FILTER_MAX_DEPTH (16)

typedef void* app_dmx_channel_handle;
typedef void* app_dmx_filter_handle;
typedef void* app_dmx_easy_filter_handle;
typedef void *app_dmx_descrambler_handle;
typedef void (*app_dmx_notify)(u8 *p_data, u32 data_size, void *private_data);
typedef struct
{
  BOOL enable_crc;
  BOOL one_shot;
  u32 buffer_size;
  u8 match[APP_DMX_FILTER_MAX_DEPTH];
  u8 mask[APP_DMX_FILTER_MAX_DEPTH];
  //u8 nmatch[APP_DMX_FILTER_MAX_DEPTH];
  u8 depth; // match/mask valid len.
  app_dmx_notify data_callback;
  void *private_data;
}app_dmx_filter_param;


typedef struct
{
  BOOL enable_crc;
  BOOL one_shot;
  u32 buffer_size;
  u8 match[APP_DMX_FILTER_MAX_DEPTH];
  u8 mask[APP_DMX_FILTER_MAX_DEPTH];
  //u8 nmatch[APP_DMX_FILTER_MAX_DEPTH];
  u8 depth; // match/mask valid len.
  app_dmx_notify data_callback;
  void *private_data;
  u16 pid;
}app_dmx_easy_filter_param;


RET_CODE app_dmx_init(u8 thread_prio);


// If you need to implement channel, use the channel and filter group interfaces, in other case use easy filters.
app_dmx_channel_handle app_dmx_alloc_channel(void);
RET_CODE app_dmx_set_channel(app_dmx_channel_handle h_channel, u16 pid);
RET_CODE app_dmx_free_channel(app_dmx_channel_handle h_channel);
RET_CODE app_dmx_start_channel(app_dmx_channel_handle h_channel);
RET_CODE app_dmx_stop_channel(app_dmx_channel_handle h_channel);
app_dmx_filter_handle app_dmx_alloc_filter(app_dmx_channel_handle h_channel);
RET_CODE app_dmx_free_filter(app_dmx_filter_handle h_filter);
RET_CODE app_dmx_set_filter(app_dmx_filter_handle h_filter, app_dmx_filter_param *param);
RET_CODE app_dmx_start_filter(app_dmx_filter_handle h_filter);
RET_CODE app_dmx_stop_filter(app_dmx_filter_handle h_filter);


app_dmx_easy_filter_handle app_dmx_alloc_easy_filter(void);
RET_CODE app_dmx_free_easy_filter(app_dmx_easy_filter_handle h_filter);
RET_CODE app_dmx_set_easy_filter(app_dmx_easy_filter_handle h_filter, app_dmx_easy_filter_param *param);
RET_CODE app_dmx_start_easy_filter(app_dmx_easy_filter_handle h_filter);
RET_CODE app_dmx_stop_easy_filter(app_dmx_easy_filter_handle h_filter);


app_dmx_descrambler_handle app_dmx_alloc_descrambler(void);
RET_CODE app_dmx_free_descrambler(app_dmx_descrambler_handle h_desc);
RET_CODE app_dmx_set_descrambler_pid(app_dmx_descrambler_handle h_desc, u16 pid);
RET_CODE app_dmx_set_descrambler_evenkey(app_dmx_descrambler_handle h_desc, u8 *key, u32 len);
RET_CODE app_dmx_set_descrambler_oddkey(app_dmx_descrambler_handle h_desc, u8 *key, u32 len);
RET_CODE app_dmx_start_descrambler(app_dmx_descrambler_handle h_desc);
RET_CODE app_dmx_stop_descrambler(app_dmx_descrambler_handle h_desc);



#ifdef __cplusplus
}
#endif //__cplusplus
#endif //_app_demux_h_
