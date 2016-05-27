/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
//#include "lpower.h"
#include "ui_common.h"

#include "customer_config.h"

customer_cfg_t g_customer;
extern u8 g_led_index;
extern u8 g_led_buffer[10];
extern u8 g_led_power_index;
#ifndef WIN32
extern u8 com_num;
#endif

void config_ota_main_tp(void)
{
  dvbc_lock_t upg_tp;
  nim_para_t set_tp;

  memset(&upg_tp,0,sizeof(dvbc_lock_t));
//  sys_status_get_upgrade_tp(&upg_tp);
  upg_tp.tp_freq = 474000;
  upg_tp.tp_sym= 6875;
  upg_tp.nim_modulate = 1;

  memset(&set_tp,0,sizeof(nim_para_t));
  set_tp.lock_mode = SYS_DVBC;
  set_tp.lockc.tp_freq = upg_tp.tp_freq;
  set_tp.lockc.tp_sym = upg_tp.tp_sym;
  set_tp.lockc.nim_modulate = (nim_modulation_t)(upg_tp.nim_modulate);
  set_tp.data_pid = 0x1b58;
  
  /****init in ota block,the data is do ota tp data,but in app,it from system status init data*/
  mul_ota_dm_api_save_set_ota_tp(&set_tp);
}

void customer_config_init(void)
{
  memset(&g_customer, 0, sizeof(customer_cfg_t));
  g_customer.customer_id = CUSTOMER_XINNEW_DEMO;
  g_customer.cas_id = CONFIG_CAS_ID_ADT_MG;
  g_customer.country = COUNTRY_CHINA;
  g_customer.b_WaterMark = TRUE;
  g_customer.x_WaterMark = 1080;
  g_customer.y_WaterMark = 650;
  g_customer.language_num = 2;// english curn and chinese curn 
  g_customer.cus_config_ota_main_tp = config_ota_main_tp;
}

u8 customer_config_language_num_get()
{
  return g_customer.language_num;
}
#ifndef WIN32
void lock_led_set(BOOL locked,u8 tunerId)
{
  uio_device_t *p_uio_dev = NULL;
  
  p_uio_dev = dev_find_identifier(NULL,DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);

  g_led_index = 1;

  if(locked == 0)
  {
    if(g_led_buffer[com_num + 1] != 0)
    {
        memcpy(&(g_led_buffer[g_led_index]),&(g_led_buffer[g_led_index + 1]),com_num - g_led_index +1);
    }
    	
    g_led_buffer[com_num + 1] = 0;
    uio_display(p_uio_dev, g_led_buffer, com_num + 1);
  }
  else
  {
    if(g_led_buffer[com_num + 1] == 0)
    {
        u8 num = com_num + 1;
        
        while(g_led_index <  --num )
            g_led_buffer[num + 1] = g_led_buffer[num];
        
        g_led_buffer[g_led_index] = '.';
    }
    uio_display(p_uio_dev, g_led_buffer, com_num + 2);
  }
}

#endif

