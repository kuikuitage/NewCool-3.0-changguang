#include "stdlib.h"
// system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"

#include "string.h"

// os
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"

// util
#include "class_factory.h"
#include "mem_manager.h"
#include "lib_unicode.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "lib_memf.h"
#include "lib_memp.h"
#include "simple_queue.h"
#include "unzip.h"

// driver
#include "hal_base.h"
#include "hal_gpio.h"
#include "hal_misc.h"
#include "hal_uart.h"

#include "drv_dev.h"
#include "drv_misc.h"

#include "glb_info.h"
#include "i2c.h"
#include "uio.h"
#include "charsto.h"
#include "avsync.h"
#include "common.h"
#include "aud_vsb.h"
#include "gpe_vsb.h"

#include "nim.h"
#include "vbi_inserter.h"
#include "hal_watchdog.h"
#include "scart.h"
#include "rf.h"

// mdl
#include "mdl.h"

#include "data_manager.h"
#include "data_manager_v2.h"
#include "data_base.h"

#include "service.h"
#include "dvb_svc.h"
#include "dvb_protocol.h"
#include "mosaic.h"
#include "nit.h"
#include "pat.h"
#include "pmt.h"
#include "sdt.h"
#include "ts_packet.h"
#include "eit.h"
#include "epg_data4.h"
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "ss_ctrl.h"

#include "mem_cfg.h"
//ap

#include "sys_status.h"
#include "mt_time.h"


/*!
   which block store sysstatus data
 */
#define SS_VAULE_BLOCK_ID       (IW_VIEW_BLOCK_ID)

/*!
   which block store sysstatus default data
 */
#define SS_DEFAULT_BLOCK_ID    (SS_DATA_BLOCK_ID)


/************************************************************************
* NOTICE:
*   1. curn_group must be set as zero when initialize.
*   2. sys_status_check_group will cause recreate prog views.
*
************************************************************************/
#if 0

static sys_status_t g_status;
void sys_status_init(void)
{
  static u8 is_initialized = FALSE;
  BOOL ret = TRUE;
//  av_set_t av_set;
  
  if (is_initialized == TRUE)
  {
    return;
  }

  ret = ss_ctrl_init(SS_VAULE_BLOCK_ID, (u8*)&g_status, sizeof(sys_status_t));
  if(!ret)
  {
    sys_status_load();
    ss_ctrl_clr_checksum(class_get_handle_by_id(SC_CLASS_ID));
  }

  OS_PRINTF("\r\n[OTA]get sw ver %d ", sys_status_get_sw_version());
  g_status.ota_info.orig_software_version = (u16)sys_status_get_sw_version();
  
  is_initialized = TRUE;

}

void sys_status_load(void)
{
  class_handle_t handle = class_get_handle_by_id(DM_CLASS_ID);
  
  dm_read(handle, SS_DEFAULT_BLOCK_ID, 0, 0, 
  	sizeof(sys_status_t)/* except ucas key */,
  	(u8*)(&g_status));


  sys_status_set_status(BS_LNB_POWER, TRUE);
  sys_status_set_status(BS_BEEPER, TRUE);
  sys_status_set_status(BS_ANTENNA_CONNECT, TRUE);
  
  sys_status_save();  //need save
  ss_ctrl_set_checksum(class_get_handle_by_id(SC_CLASS_ID));

}


void sys_status_save(void)
{
  ss_ctrl_sync(class_get_handle_by_id(SC_CLASS_ID),
    (u8*)&g_status);
}

sys_status_t *sys_status_get(void)
{
  return &g_status;
}

u32 sys_status_get_sw_version(void)
{
  sys_status_t *status;
  
  status = sys_status_get();   

  return status->sw_version;
}

void sys_status_set_sw_version(u32 new_version)
{
  sys_status_t *status;
  
  status = sys_status_get();   

  status->sw_version = new_version;

  sys_status_save();
}


BOOL sys_status_get_status(u8 type, BOOL *p_status)
{
  sys_status_t *status;

  if (type < BS_MAX_CNT)
  {
    status = sys_status_get();      

    *p_status = (BOOL)SYS_GET_BIT(status->bit_status, type);
    return TRUE;
  }

  return FALSE;
}


BOOL sys_status_set_status(u8 type, BOOL enable)
{
  sys_status_t *status;
  
  if (type < BS_MAX_CNT)
  {
    status = sys_status_get();      
    if (enable)
    {
      SYS_SET_BIT(status->bit_status, type);
    }
    else
    {
      SYS_CLR_BIT(status->bit_status, type);
    }
    return TRUE;
  }

  return FALSE;
}


void sys_status_get_sw_info(sw_info_t *p_sw)
{
  sys_status_t *status;
  
  status = sys_status_get();

  *p_sw = status->ver_info;
}

void sys_status_set_sw_info(sw_info_t *p_sw)
{
  sys_status_t *status;
  
  status = sys_status_get();  

  status->ver_info = *p_sw;
}

u16 sys_status_get_force_key(void)
{
  sys_status_t *status;
  
  status = sys_status_get();

  return (u16)status->force_key;
}

ota_info_t *sys_status_get_ota_info(void)
{
  sys_status_t *status;
  
  status = sys_status_get();

  return &status->ota_info;  
}

void sys_status_set_ota_info(ota_info_t *p_otai)
{
  sys_status_t *status;

  MT_ASSERT(p_otai != NULL);

  status = sys_status_get();  
  OS_PRINTF("\r\n===%s:p_otai->ota_tri[0x%x]===\r\n",__FUNCTION__, p_otai->ota_tri);

  memcpy((void *)&status->ota_info, (void *)p_otai, sizeof(ota_info_t));

  return;
}
#else


static mac_set_t sg_mac_set;

BOOL sys_status_set_mac(u8 index, char *name)
{
  switch(index)
 {
  case 0:
    memcpy(sg_mac_set.mac_one, name, 2);
    break;
  case 1:
    memcpy(sg_mac_set.mac_two, name, 2);
    break;
  case 2:
    memcpy( sg_mac_set.mac_three, name, 2);
    break;
  case 3:
    memcpy( sg_mac_set.mac_four, name, 2);
    break;
 case 4:
    memcpy(sg_mac_set.mac_five, name, 2);
    break;
 case 5:
    memcpy(sg_mac_set.mac_six, name, 2);
    break;
 default:
    break;
 }

  return TRUE;
}




char  sys_status_get_mac_by_index(u8 index)
{
  char hwaddr = '\0',*pstr;
  char name[3] = "0";

 switch(index)
 {
  case 0:
    memcpy(name, sg_mac_set.mac_one, 2);
    name[2] = '\0';
    hwaddr = (char)strtol(name,&pstr,16);
    break;
  case 1:
    memcpy(name, sg_mac_set.mac_two, 2);
    name[2] = '\0';
   hwaddr = (char)strtol(name,&pstr,16);
    break;
  case 2:
    memcpy(name, sg_mac_set.mac_three, 2);
    name[2] = '\0';
   hwaddr = (char)strtol(name,&pstr,16);
    break;
  case 3:
    memcpy(name, sg_mac_set.mac_four, 2);
    name[2] = '\0';
   hwaddr = (char)strtol(name,&pstr,16);
    break;
 case 4:
    memcpy(name, sg_mac_set.mac_five, 2);
    name[2] = '\0';
    hwaddr = (char)strtol(name,&pstr,16);
    break;
 case 5:
    memcpy(name, sg_mac_set.mac_six, 2);
    name[2] = '\0';
    hwaddr = (char)strtol(name,&pstr,16);
    break;
 default:
    break;
 }
  return hwaddr;
}



u32 sys_status_get_sw_version(void)
{
  return 0;
}


void sys_status_get_ipaddress(ip_address_set_t *p_set)
{
  //------------------
}


void sys_status_set_ipaddress(ip_address_set_t *p_set)
{
  //--------------------
}


static net_config_t sg_net_config = {DHCP, LINK_TYPE_LAN};
void sys_status_get_net_config_info(net_config_t *p_set)
{
  //--------------------
  *p_set = sg_net_config;
}
void sys_status_set_net_config_info(net_config_t *p_set)
{
  sg_net_config = *p_set;
}


BOOL sys_status_get_mac(u8 index, char *name)
{
 switch(index)
 {
  case 0:
    memcpy(name, sg_mac_set.mac_one, 2);
    break;
  case 1:
    memcpy(name, sg_mac_set.mac_two, 2);
    break;
  case 2:
    memcpy(name, sg_mac_set.mac_three, 2);
    break;
  case 3:
    memcpy(name, sg_mac_set.mac_four, 2);
    break;
 case 4:
    memcpy(name, sg_mac_set.mac_five, 2);
    break;
 case 5:
    memcpy(name, sg_mac_set.mac_six, 2);
    break;
 default:
    break;
 }
  return TRUE;
}



void sys_status_get_wifi_info(wifi_info_t *p_set)
{
}


void sys_status_set_wifi_info(wifi_info_t *p_set)
{
}


static u32 sg_custom_id = 0;
static u32 sg_serial_num = 0;
static u32 sg_hw_ver = 0;
u32 sys_status_get_customer_id()
{
  return sg_custom_id;
}

void sys_status_set_customer_id(u32 cid)
{
  sg_custom_id = cid;
}
u32 sys_status_get_serial_num()
{
  return sg_serial_num;
}
void  sys_status_set_serial_num(u32 sn)
{
  sg_serial_num = sn;
}

u32 sys_status_get_hw_ver()
{
  return sg_hw_ver;
}

void sys_status_set_hw_ver(u32 hw)
{
  sg_hw_ver = hw;
}
void sys_status_save(void)
{
}

#endif

