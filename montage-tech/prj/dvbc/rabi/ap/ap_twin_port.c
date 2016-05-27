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


#define REC_DATA_LEN 19
#define REQ_INTERVEL  50
#define READ_TIME_OUT 500;
#define DATA_HEAD_NUM 3
#define TWIN_PORT_KEY_LENGTH 8
static u8 data_head[DATA_HEAD_NUM]= {0xF7, 0x07, 0x06};


typedef enum 
{ 
    TWIN_PORT_IDLE,
    TWIN_PORT_START,
    TWIN_PORT_REQUEST_SERVER,
    TWIN_PORT_READ_SERVER,
    TWIN_PORT_PRE_SET_CW,
    TWIN_PORT_SET_CW,
}twin_id;

typedef struct
{
  u8 data_len;
  u32 last_tick;
  u32 cur_tick;
  u16 time_out;
  u16 req_interval;
  u8 ms_state;
  u8 p_rec_data[REC_DATA_LEN];
  twin_id twin_state;
}ap_privt;

typedef struct
{

  ap_privt ap_twin_port_priv_t;

  ap_twin_port_policy_t *p_impl_policy;
  
  app_t ap_twin_instance;
  
}twin_port_priv;


static void ap_twin_single_step(void *handle, os_msg_t *p_msg)
{  
     RET_CODE ret = SUCCESS;
     twin_port_priv *tw_priv = handle;

    tw_priv->ap_twin_port_priv_t.cur_tick = mtos_ticks_get();

     if(p_msg != NULL)
     {
          switch(p_msg->content)
          {
             case AP_TWIN_PORT_ON:
                 tw_priv->p_impl_policy->on_ap_twin_port_enable(AP_TWIN_PORT_ON);
                 tw_priv->ap_twin_port_priv_t.ms_state = TWIN_PORT_START;
                  break;
             case AP_TWIN_PORT_OFF:
                 tw_priv->p_impl_policy->on_ap_twin_port_enable(AP_TWIN_PORT_OFF);
                 tw_priv->ap_twin_port_priv_t.ms_state = TWIN_PORT_IDLE;
                break;
             case AP_TWIN_PORT_PG_INFO:
                 tw_priv->p_impl_policy->set_twin_port_pg_info(p_msg->para1);
                   break;
            default:
                 break;
            }
     }
   
        switch(tw_priv->ap_twin_port_priv_t.ms_state)
        {   
           case TWIN_PORT_IDLE:
                 break;
            case TWIN_PORT_START:
              tw_priv->ap_twin_port_priv_t.ms_state = TWIN_PORT_REQUEST_SERVER;
              tw_priv->ap_twin_port_priv_t.last_tick = tw_priv->ap_twin_port_priv_t.cur_tick;
              break;
            case TWIN_PORT_REQUEST_SERVER:
                if(tw_priv->ap_twin_port_priv_t.cur_tick-tw_priv->ap_twin_port_priv_t.last_tick>\
                             tw_priv->ap_twin_port_priv_t.req_interval)
                {
                ret = tw_priv->p_impl_policy->twin_port_request_server();
                if(ret == SUCCESS)
                tw_priv->ap_twin_port_priv_t.ms_state = TWIN_PORT_READ_SERVER;
                tw_priv->ap_twin_port_priv_t.last_tick = tw_priv->ap_twin_port_priv_t.cur_tick;
                }
                 break;
           case TWIN_PORT_READ_SERVER:
              ret =tw_priv->p_impl_policy->twin_port_read_server(tw_priv->ap_twin_port_priv_t.p_rec_data,\
                                &tw_priv->ap_twin_port_priv_t.data_len,REC_DATA_LEN);
              if(ret == SUCCESS)
              {  
                    uart_flush(0);
                   tw_priv->ap_twin_port_priv_t.ms_state = TWIN_PORT_PRE_SET_CW;
              }
              else 
              {  
                   if(tw_priv->ap_twin_port_priv_t.cur_tick-tw_priv->ap_twin_port_priv_t.last_tick > \
                                 tw_priv->ap_twin_port_priv_t.time_out)
                   {
                      tw_priv->ap_twin_port_priv_t.ms_state = TWIN_PORT_REQUEST_SERVER;
                      tw_priv->ap_twin_port_priv_t.data_len = 0;
                      tw_priv->ap_twin_port_priv_t.last_tick = tw_priv->ap_twin_port_priv_t.cur_tick;
                   }
              }
               break;
            case TWIN_PORT_PRE_SET_CW:

             ret = tw_priv->p_impl_policy->compare_to_head_info(data_head,tw_priv->ap_twin_port_priv_t.p_rec_data,DATA_HEAD_NUM);
             if(ret == SUCCESS)
             {
                   tw_priv->ap_twin_port_priv_t.ms_state = TWIN_PORT_SET_CW;
             }
             else
             {
                  tw_priv->ap_twin_port_priv_t.ms_state = TWIN_PORT_REQUEST_SERVER;
                  tw_priv->ap_twin_port_priv_t.last_tick = tw_priv->ap_twin_port_priv_t.cur_tick;
             }
        
         break;
            case TWIN_PORT_SET_CW:
                 {
                 tw_priv->p_impl_policy->twin_port_set_cw(tw_priv->ap_twin_port_priv_t.p_rec_data,
                                      DATA_HEAD_NUM,
                            TWIN_PORT_KEY_LENGTH);
                 tw_priv->ap_twin_port_priv_t.ms_state = TWIN_PORT_REQUEST_SERVER;
                 tw_priv->ap_twin_port_priv_t.last_tick = tw_priv->ap_twin_port_priv_t.cur_tick;
               }
          break;
         default:
         MT_ASSERT(0);
         break;
         }
         
  

}

static void init()
{

}

app_t *construct_ap_twin_port(ap_twin_port_policy_t *p_impl_policy)
{
   twin_port_priv *tw_priv = mtos_malloc(sizeof(twin_port_priv));
   MT_ASSERT(tw_priv != NULL);
   
   memset(&tw_priv->ap_twin_port_priv_t,0,sizeof(ap_privt));
   memset(&tw_priv->ap_twin_instance,0,sizeof(app_t));
   memset(tw_priv->ap_twin_port_priv_t.p_rec_data,0,REC_DATA_LEN);
   
   tw_priv->ap_twin_port_priv_t.ms_state = TWIN_PORT_IDLE;
   tw_priv->ap_twin_port_priv_t.req_interval = REQ_INTERVEL;
   tw_priv->ap_twin_port_priv_t.time_out = READ_TIME_OUT;
   tw_priv->p_impl_policy = p_impl_policy;
   
   tw_priv->ap_twin_instance.task_single_step = ap_twin_single_step;
   tw_priv->ap_twin_instance.init = init;
   tw_priv->ap_twin_instance.p_data = tw_priv;

  return   &(tw_priv->ap_twin_instance);
}


