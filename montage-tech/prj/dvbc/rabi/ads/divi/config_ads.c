/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
// lib
#ifdef ENABLE_ADS

#include "ui_common.h"
#include "ads_ware.h"
#include "config_ads.h"
#include "ui_ads_display.h"
#include "ui_ad_api.h"

/*!
  DVBC lock info
  */
typedef struct
{
  /*!
    freq
    */
  u32 tp_freq;
  /*!
    sym
    */
  u32 tp_sym;
  /*!
    nim modulate
    */
  u16 nim_modulate;

} ad_lock_t;

u8 *p_ads_mem = NULL;

u32 adm_id = ADS_ID_ADT_DIVI;

static ad_lock_t ad_maintp = {307000,6875,2};
static ads_module_cfg_t module_config = {0};

extern u32 get_flash_addr(void);

void ads_mem_init(u32 mem_start,u32 size)
{
#if 1
  BOOL ret = FALSE;
  mem_mgr_partition_param_t partition_param = { 0 };
  MT_ASSERT(size > (6 * MBYTES));
    //create ads partition
  partition_param.id   = MEM_ADS_PARTITION;
  partition_param.size = size - 200*KBYTES;
  partition_param.p_addr = (u8 *)mem_start + 200*KBYTES;
  partition_param.atom_size = SYS_PARTITION_ATOM;
  partition_param.user_id = SYS_MODULE_APP_TEST;
  partition_param.method_id = MEM_METHOD_NORMAL;

  //memset(partition_param.p_addr,0,partition_param.size);
  ret = mem_mgr_create_partition(&partition_param);
  MT_ASSERT(FALSE != ret);
#endif
}

//store the AD data to flash
static RET_CODE nvram_write(u32 offset, u8 *p_buf, u32 size)
{
  RET_CODE ret = FALSE;
  void *p_dm_handle = NULL;

  DEBUG(ADS,INFO,"get in \n");
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  DEBUG(ADS,INFO,"ad write addr[0x%x], size[0x%x]\n", offset, size);
  //offset -= module_config.flash_start_adr;
  if(size!=0)
  {
    ret=dm_direct_write(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset,size,p_buf);
  }
  OS_PRINTF("AD nvram_write end\n");

  return TRUE;
}

//read the ad data from flash
static RET_CODE nvram_read(u32 offset, u8 *p_buf, u32 *p_size)
{
  void *p_dm_handle = NULL;
  u32 nvm_read =0;

  DEBUG(ADS,INFO,"get in \n");
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);

  DEBUG(ADS,INFO,"ad read addr[0x%x], size[0x%x]\n", offset,*p_size);
  //offset -= module_config.flash_start_adr;
  //nvm_read=dm_read(p_dm_handle,DEC_RW_ADS_BLOCK_ID,node_id,offset,*p_size,p_buf);
  nvm_read=dm_direct_read(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset,*p_size,p_buf);
  DEBUG(ADS,INFO,"ad nvm_read[0x%x]\n", nvm_read);

  return nvm_read;
}


static RET_CODE nvram_erase(u32 size)
{
  dm_ret_t ret = DM_SUC;
  void *p_dm_handle = NULL;
  u32 offset = 0;

  DEBUG(ADS,INFO,"get in \n");
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  DEBUG(ADS,INFO,"ADS:Erase size_pre===%d\n", size);
  ret=dm_direct_erase(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset, module_config.flash_size);
  MT_ASSERT(ret != DM_FAIL);

  return SUCCESS;
}

static RET_CODE nvram_erase2(u32 offset, u32 size)
{
  dm_ret_t ret = DM_SUC;
  void *p_dm_handle = NULL;

  DEBUG(ADS,INFO,"get in \n");
  p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  MT_ASSERT(p_dm_handle != NULL);
  DEBUG(ADS,INFO,"ADS:Erase size_erase = 0x%x ,offset=[0x%x]\n", size, offset);
  //offset -= module_config.flash_start_adr;
  ret=dm_direct_erase(p_dm_handle,DEC_RW_ADS_BLOCK_ID,offset, size);
  DEBUG(ADS,INFO,"ADS:nvram_erase2 , ret=%d\n", ret);
 
  return TRUE;
}

static void ads_module_open(u32 m_id)
{
  RET_CODE ret = ERR_FAILURE;

  ret = ads_open(m_id);
  DEBUG(ADS,INFO,"ADS: ads_module_open===%d\n", ret);
 // MT_ASSERT(SUCCESS == ret);
}

static RET_CODE ads_notify_msg_to_ui(BOOL is_sync, u16 content, u32 para1, u32 para2)
{
  //pay attention, the parameter content will not be changed
  DEBUG(ADS,INFO,"ads_notify_msg_to_ui ----\n ");
  return fw_notify_root(fw_find_root_by_id(ROOT_ID_BACKGROUND), NOTIFY_T_MSG, is_sync, MSG_ADS_TO_UI, para1, para2);
}

extern RET_CODE ads_adt_divi_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id);

static RET_CODE ads_divi_module_init()
{
  //lint -save -e64
  ads_adapter_cfg_t adapter_cfg = {0};
  //lint -restore

#ifndef WIN32
  RET_CODE ret = ERR_FAILURE;

  ads_init(adm_id,&adapter_cfg);

  memset(&module_config, 0, sizeof(ads_module_cfg_t));
  module_config.channel_frequency = ad_maintp.tp_freq;
  module_config.channel_symbolrate = ad_maintp.tp_sym;
  module_config.channel_qam = ad_maintp.nim_modulate;
  
  module_config.flash_start_adr= dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), DEC_RW_ADS_BLOCK_ID);
  module_config.nvram_read= nvram_read;
  module_config.nvram_write= nvram_write;
  module_config.nvram_erase= nvram_erase;
  module_config.nvram_erase2 = nvram_erase2;
  module_config.flash_size= dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), DEC_RW_ADS_BLOCK_ID);
  module_config.p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  module_config.task_prio_start= AD_TASK_PRIO_START;
  module_config.task_prio_end= AD_TASK_PRIO_END;
  module_config.platform_type = ADS_PLATFORM_HD;
  module_config.notify_msg_to_ui = ads_notify_msg_to_ui;

  DEBUG(ADS,INFO,"flash_start_adr =%x\n ",module_config.flash_start_adr);
  DEBUG(ADS,INFO,"flash_size =%x\n ",module_config.flash_size);

  ret = ads_adt_divi_attach(&module_config, &adm_id);

  MT_ASSERT(SUCCESS == ret);

  ui_adv_set_adm_id(adm_id);

  ads_module_init((ads_module_id_t)adm_id);

  ads_module_open(ADS_ID_ADT_DIVI);
  mtos_task_delay_ms(100);
  return SUCCESS;
#endif
}

static void set_adver_maintp(u32 tp_freq,u32 tp_sym,u8 nim_modulate)
{
  ad_maintp.tp_freq = tp_freq;
  ad_maintp.tp_sym = tp_sym;
  switch(nim_modulate )
  {
    case 4:
       ad_maintp.nim_modulate = 16;
      break;

    case 5:
       ad_maintp.nim_modulate = 32;
      break;

    case 6:
       ad_maintp.nim_modulate = 64;
      break;

    case 7:
       ad_maintp.nim_modulate = 128;
      break;

    case 8:
      ad_maintp.nim_modulate = 256;
      break;

    default:
       ad_maintp.nim_modulate = 64;
      break;
  }

}

void ads_ap_init(void)
{
  dvbc_lock_t tmp_maintp = {0};
  RET_CODE ret = ERR_FAILURE;

  divi_ads_show_init();
  sys_status_get_main_tp1(&tmp_maintp);
  set_adver_maintp(tmp_maintp.tp_freq,tmp_maintp.tp_sym,tmp_maintp.nim_modulate);
  DEBUG(ADS,INFO,"[AD_INIT] tp_freq = %d,tp_sym = %d, tp_modulate = %d\n",
                  tmp_maintp.tp_freq,
                  tmp_maintp.tp_sym,
                  tmp_maintp.nim_modulate);

  ret = ads_divi_module_init();
  if(ret == SUCCESS)
  {
    OS_PRINTF("[AD_INIT] show ads logo  %s %d \n",__func__,__LINE__);  
    divi_show_logo();
  }
}
#endif

