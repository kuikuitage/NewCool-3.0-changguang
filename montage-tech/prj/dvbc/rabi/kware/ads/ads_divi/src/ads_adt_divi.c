/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifdef ENABLE_ADS
#include <string.h>

#include "sys_types.h"
#include "sys_define.h"
#include "lib_bitops.h"
#include "lib_rect.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "drv_dev.h"
#include "pti.h"
#include "nim.h"
#include "dmx.h"
//#include "ui_util_api.h"

#include "ads_ware.h"
#include "ads_adapter.h"
#include "ads_api_divi.h"
#include "ui_common.h"

#define ADS_DIVI_ADT_PRINTF

#ifdef ADS_DIVI_ADT_PRINTF
    #define ADS_ADT_PRINTF OS_PRINTF
#else
    #define ADS_ADT_PRINTF DUMMY_PRINTF
#endif

extern ads_adapter_priv_t g_ads_priv;
ads_module_priv_t *p_ads_divi_priv = NULL;
static u32 s_ads_lock = 0;

extern void divi_ads_read_data_from_flash(void);
/*!
    lock
*/
void divi_ads_lock(void)
{
    //CAS_DIVI_PRINTF("[divi],s_ads_lock:%d \n",s_ads_lock);
    mtos_sem_take((os_sem_t *)&s_ads_lock, 0);
}

/*!
    unlock
*/
void divi_ads_unlock(void)
{
    //CAS_DIVI_PRINTF("[divi],s_ads_lock:%d \n",s_ads_lock);
    mtos_sem_give((os_sem_t *)&s_ads_lock);
}

RET_CODE ads_adt_divi_init(void *param)
{
  ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);
  
  divi_ads_read_data_from_flash();
  divi_ads_drv_init();
  return SUCCESS;
}

RET_CODE ads_adt_divi_deinit()
{
  ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);
  return SUCCESS;
}

RET_CODE ads_adt_divi_open()
{
  ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);
  return SUCCESS;
}

RET_CODE ads_adt_divi_close()
{
    ADS_ADT_PRINTF("[adt] %s,%d \n",__func__,__LINE__);
    return SUCCESS;
}

RET_CODE ads_adt_divi_io_ctrl(u32 cmd, void *param)
{
  RET_CODE ret = SUCCESS;
  return ret;
}


RET_CODE ads_adt_divi_display(ads_info_t *param)
{
  RET_CODE ret = SUCCESS;
  OS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);
  return ret;
}

RET_CODE ads_adt_divi_hide(ads_info_t *param)
{
  OS_PRINTF("%s %d\n",__FUNCTION__, __LINE__);
  return SUCCESS;
}

RET_CODE ads_adt_divi_attach(ads_module_cfg_t * p_cfg, u32 *p_adm_id)
{
  s32 err = 0;

  OS_PRINTF("[adt] %s,%d \n",__func__,__LINE__);
  g_ads_priv.adm_op[ADS_ID_ADT_DIVI].attached = 1;
  g_ads_priv.adm_op[ADS_ID_ADT_DIVI].inited = 0;

  g_ads_priv.adm_op[ADS_ID_ADT_DIVI].func.init
    = ads_adt_divi_init;
  g_ads_priv.adm_op[ADS_ID_ADT_DIVI].func.deinit
    = ads_adt_divi_deinit;
  g_ads_priv.adm_op[ADS_ID_ADT_DIVI].func.open
    = ads_adt_divi_open;
  g_ads_priv.adm_op[ADS_ID_ADT_DIVI].func.close
    = ads_adt_divi_close;
  g_ads_priv.adm_op[ADS_ID_ADT_DIVI].func.display
    = ads_adt_divi_display;
  g_ads_priv.adm_op[ADS_ID_ADT_DIVI].func.hide
    = ads_adt_divi_hide;
  g_ads_priv.adm_op[ADS_ID_ADT_DIVI].func.io_ctrl
    = ads_adt_divi_io_ctrl;

  p_ads_divi_priv = g_ads_priv.adm_op[ADS_ID_ADT_DIVI].p_priv
  = mtos_malloc(sizeof(ads_module_priv_t));
  MT_ASSERT(NULL != p_ads_divi_priv);
  memset(p_ads_divi_priv, 0x00, sizeof(ads_module_priv_t));

  p_ads_divi_priv->id = ADS_ID_ADT_DIVI;
  p_ads_divi_priv->p_dmx_dev = p_cfg->p_dmx_dev;
  p_ads_divi_priv->flash_size = p_cfg->flash_size;
  p_ads_divi_priv->flash_start_adr = p_cfg->flash_start_adr;
  p_ads_divi_priv->nv_read = p_cfg->nvram_read;
  p_ads_divi_priv->nv_write = p_cfg->nvram_write;
  p_ads_divi_priv->nv_erase2 = p_cfg->nvram_erase2;
  p_ads_divi_priv->read_gif = p_cfg->read_gif;

  p_ads_divi_priv->read_rec = p_cfg->read_rec;
  p_ads_divi_priv->write_rec = p_cfg->write_rec;
  p_ads_divi_priv->display_ad = p_cfg->display_ad;
  p_ads_divi_priv->hide_ad = p_cfg->hide_ad;
  p_ads_divi_priv->channel_frequency = p_cfg->channel_frequency;
  p_ads_divi_priv->channel_symbolrate = p_cfg->channel_symbolrate;
  p_ads_divi_priv->channel_qam = p_cfg->channel_qam;
  p_ads_divi_priv->task_prio_start = p_cfg->task_prio_start;
  p_ads_divi_priv->task_prio_end = p_cfg->task_prio_end;
  p_ads_divi_priv->notify_msg_to_ui = p_cfg->notify_msg_to_ui;

  *p_adm_id = ADS_ID_ADT_DIVI;

  err = mtos_sem_create((os_sem_t *)&s_ads_lock,1);
  if(!err)
  {
       OS_PRINTF("ads_adt_divi_attach, create sem error ! \n");
       MT_ASSERT(0);
  }

   return SUCCESS;
}

#endif

