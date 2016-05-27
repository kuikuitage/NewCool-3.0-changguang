#include "sys_types.h"
#include "sys_define.h"
#include "string.h"

#include "mtos_msg.h"
#include "mtos_mem.h"
#include "mtos_printk.h"
#include "mtos_misc.h"


#include "drv_dev.h"
#include "hal_uart.h"

#include "dmx.h"
#include "ap_framework.h"

#include "ap_twin_port.h"

 static twin_port_info_t p_twin_port ={0};

 static void on_ap_twin_port_enable(BOOL ON)
 { 
     if(ON)
     {
     mtos_close_printk();
     }
     uart_flush(0);
     if(!ON)
     {
      mtos_open_printk();
     }
}

static RET_CODE compare_to_head_info(u8 *data_head,u8 *p_cmp_data,u8 length)
{
   u8 i =0;
   while(data_head[i] == *(p_cmp_data+i))
   {      
           if(i++==length-1)
            return   SUCCESS;
   }
   return ERR_FAILURE;
}

static void set_twin_port_pg_info(u32 para)
{ 
      memcpy(&p_twin_port,(twin_port_info_t *)para,sizeof(twin_port_info_t));
}

static RET_CODE twin_port_read_server(u8 *p_read_data,u8 *len,u8 max_length)
{ 
     RET_CODE  ret = SUCCESS;
     int i=0;
     for(i=*len;i<max_length;i++)
     {
        ret = uart_read_byte(0, p_read_data+i, 10);
        *len = i;
         if(ret != SUCCESS)
        {
           return ERR_FAILURE;
        }
     }
     return SUCCESS;
}

static RET_CODE twin_port_request_server()
{
  u8 send_data[9]= {0};
  u8 cnt = 0;

  send_data[0] = 0x07;
  send_data[1] = 0x06;
  send_data[2] = (u8)((p_twin_port.sat_degree & 0xFF00)>>8);
  send_data[3] = (p_twin_port.sat_degree)&(0x00FF);
  send_data[4] = (u8)((p_twin_port.freq & 0xFF00)>>8);
  send_data[5] = (u8)(p_twin_port.freq & 0x00FF);
  send_data[6] = (u8)(((p_twin_port.svc_id)&(0xFF00))>>8);
  send_data[7] = (u8)((p_twin_port.svc_id)&(0x00FF));
  send_data[8] = send_data[0];
  
  for(cnt = 1; cnt < 8; cnt ++)
  {
   send_data[8] ^=  send_data[cnt];
  }

  for(cnt = 0; cnt <9; cnt ++)
  {
  uart_write_byte(0, send_data[cnt]);
  }
  
  return SUCCESS;
}


  void pti_set_cw(u8 *p_data,u8 head_len,u8 key_length)
  {
        dmx_chanid_t p_channel_a = 0,p_channel_v = 0;

      dmx_device_t  *p_dev = dev_find_identifier(NULL,DEV_IDT_TYPE, 
                    SYS_DEV_TYPE_PTI);
      MT_ASSERT(p_dev != NULL);

        dmx_get_chanid_bypid(p_dev, p_twin_port.cur_audio_pid,&p_channel_a);
          dmx_descrambler_onoff(p_dev, p_channel_a, TRUE);
                dmx_descrambler_set_odd_keys(p_dev,p_channel_a,
                               p_data+head_len,
                                        key_length);
                dmx_descrambler_set_even_keys(p_dev,p_channel_a,
                        p_data+head_len+key_length,
                                        key_length);
                dmx_get_chanid_bypid(p_dev, p_twin_port.cur_video_pid,&p_channel_v);
                dmx_descrambler_onoff(p_dev, p_channel_v, TRUE);
                dmx_descrambler_set_even_keys(p_dev,p_channel_v,
                                p_data+head_len,
                                        key_length);
                dmx_descrambler_set_odd_keys(p_dev,p_channel_v,
                                      p_data+head_len+key_length,
                                        key_length);
  }


ap_twin_port_policy_t *construct_twin_policy(void)
{
    ap_twin_port_policy_t *p_policy = mtos_malloc(sizeof(ap_twin_port_policy_t));
    memset(p_policy,0,sizeof(ap_twin_port_policy_t));
    MT_ASSERT(p_policy != NULL);
    p_policy->on_ap_twin_port_enable = on_ap_twin_port_enable;
    p_policy->compare_to_head_info = compare_to_head_info;
    p_policy->set_twin_port_pg_info = set_twin_port_pg_info;
    p_policy->twin_port_read_server = twin_port_read_server;
    p_policy->twin_port_request_server = twin_port_request_server;
    p_policy->twin_port_set_cw = pti_set_cw;
    return p_policy;
}
