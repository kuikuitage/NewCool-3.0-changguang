#ifndef __AP_TWIN_PORT_H_
#define  __AP_TWIN_PORT_H_

typedef enum 
{
  AP_TWIN_PORT_OFF,
  AP_TWIN_PORT_ON,
  AP_TWIN_PORT_PG_INFO,
}p_msg_type;


typedef struct
{
  //service id
  u16 svc_id;

  //pg id
  u16 pg_id;
  /*!
      Downward Frequency
      */
  u16 freq;
  /*!
      Satellite degree
      */
  u16 sat_degree;
  /*!
      Current audio channel number
      */
  u16 cur_audio_pid;
  /*!
      Current video pid
        */
  u16 cur_video_pid;
}twin_port_info_t;


typedef struct
{
   RET_CODE (*twin_port_request_server)(void);
   RET_CODE (*compare_to_head_info)(u8 *,u8 *,u8);
   RET_CODE (*twin_port_read_server)(u8 *,u8 *,u8);
   void  (*on_ap_twin_port_enable)(BOOL);
   void  (*set_twin_port_pg_info)(u32);
   void (*twin_port_set_cw)(u8 *,u8,u8);
}ap_twin_port_policy_t;

 void pti_set_cw(u8 *p_data,u8 head_len,u8 key_length);

app_t *construct_ap_twin_port(ap_twin_port_policy_t *p_impl_policy);

#endif
