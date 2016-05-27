#include<stdio.h>
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
#include "cat.h"
#include "nit.h"
#include "pat.h"
#include "pmt.h"
#include "sdt.h"
#include "ts_packet.h"
#include "eit.h"
#ifndef IMPL_NEW_EPG
#include "epg_data4.h"
#endif
#include "nim_ctrl_svc.h"
#include "nim_ctrl.h"
#include "dvbs_util.h"
#include "dvbt_util.h"
#include "dvbc_util.h"
#include "ss_ctrl.h"

#include "db_dvbs.h"
#include "mem_cfg.h"

#include "sys_status.h"
#include "mt_time.h"
#include "string.h"
#include "stdlib.h"

#include "ota_dm_api.h"

/*!
   which block store sysstatus data
 */
#define SS_VAULE_BLOCK_ID       (IW_VIEW_BLOCK_ID)

/*!
   which block store sysstatus default data
 */
#define SS_DEFAULT_BLOCK_ID    (SS_DATA_BLOCK_ID)

/*!
   text encode list
 */
static char * g_text_encode[] =
{
  "UNICODE", 
  "GB2312",
  "ASCII",
  "UTF8",
  "CP1256",
  "8859_9",   
};


/*!
   language code list
 */
static char * g_lang_code_b[] =
{
  "eng",
  "fre",
  "ger",
  "ita",
  "spa",
  "por",
  "rus",
  "tur",
  "pol",
  "ara",
  "per",
};


/*!
   language code list
 */
static char * g_lang_code_t[] =
{
  "eng",
  "fra",
  "deu",
  "ita",
  "spa",
  "por",
  "rus",
  "tur",
  "pol",
  "ara",
  "fas",
};

/************************************************************************
* NOTICE:
*   1. curn_group must be set as zero when initialize.
*   2. sys_status_check_group will cause recreate prog views.
*
************************************************************************/

static sys_status_t g_status;

static key_ca_description temp_key;


void _decompress_ucas_keyinfo(void)
{
 u32 len = 0;
 class_handle_t p_handle = class_get_handle_by_id(DM_CLASS_ID);

 len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,0,
     sizeof(key_ca_description),(void *)(&temp_key));
 MT_ASSERT(len !=0);
}

void sys_status_load_ucaskey(void)
{
  _decompress_ucas_keyinfo();
}

void sys_status_get_albums_friends_info(albums_friends_t *p_friends_info)
{
  *p_friends_info = g_status.albums_friends;
}

void sys_status_set_albums_friends_info(albums_friends_t *p_friends_info)
{
   memcpy(&g_status.albums_friends,p_friends_info,sizeof(albums_friends_t));
   sys_status_save();
}


void _decompress_ucas_keyinfo_every(unsigned char Type)
{
 u32 len = 0;
 class_handle_t p_handle = class_get_handle_by_id(DM_CLASS_ID);
 key_head_info temp_head;

  len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,0,
     sizeof(temp_head),(void *)(&temp_head));
 MT_ASSERT(len !=0);

 switch(Type)
  {
    case TYPE_VIACCESS:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos),
     sizeof(temp_key.g_ViaccessKey),(void *)(temp_key.g_ViaccessKey));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.ViaKeyNr = temp_head.ViaKeyNr;
     break;
     case TYPE_IRDETO:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos)+sizeof(temp_key.g_ViaccessKey),
     sizeof(temp_key.g_IrdetoKey),(void *)(temp_key.g_IrdetoKey));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.IrdKeyNr= temp_head.IrdKeyNr;
     break;
     case TYPE_SECA:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos)+sizeof(temp_key.g_ViaccessKey)+sizeof(temp_key.g_IrdetoKey),
     sizeof(temp_key.g_SecaKey),(void *)(temp_key.g_SecaKey));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.SecaKeyNr= temp_head.SecaKeyNr;
     break;
     case TYPE_NAGRA:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos)+sizeof(temp_key.g_ViaccessKey)+sizeof(temp_key.g_IrdetoKey)+sizeof(temp_key.g_SecaKey),
     sizeof(temp_key.g_NagraKey),(void *)(temp_key.g_NagraKey));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.NagKeyNr= temp_head.NagKeyNr;
     break;
     case TYPE_CONAX:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos)+sizeof(temp_key.g_ViaccessKey)+sizeof(temp_key.g_IrdetoKey)+sizeof(temp_key.g_SecaKey)+sizeof(temp_key.g_NagraKey),
     sizeof(temp_key.g_ConaxKey),(void *)(temp_key.g_ConaxKey));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.ConaxKeyNr= temp_head.ConaxKeyNr;
     break;
     case TYPE_CRYPTOWORKSHALF:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos)+sizeof(temp_key.g_ViaccessKey)+sizeof(temp_key.g_IrdetoKey)+sizeof(temp_key.g_SecaKey)+sizeof(temp_key.g_NagraKey)+sizeof(temp_key.g_ConaxKey),
     sizeof(temp_key.g_CryptoWorkshalfKey),(void *)(temp_key.g_CryptoWorkshalfKey));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.CryptoWorkshalfKeyNr= temp_head.CryptoWorkshalfKeyNr;
     break;
     case TYPE_VIDEOGUARD:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos)+sizeof(temp_key.g_ViaccessKey)+sizeof(temp_key.g_IrdetoKey)+sizeof(temp_key.g_SecaKey)+sizeof(temp_key.g_NagraKey)+sizeof(temp_key.g_ConaxKey)+sizeof(temp_key.g_CryptoWorkshalfKey),
     sizeof(temp_key.g_VideoGuardKey),(void *)(temp_key.g_VideoGuardKey));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.VideoGuardkeyNr= temp_head.VideoGuardkeyNr;
     break;
     case TYPE_BISS:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos)+sizeof(temp_key.g_ViaccessKey)+sizeof(temp_key.g_IrdetoKey)+sizeof(temp_key.g_SecaKey)+sizeof(temp_key.g_NagraKey)+sizeof(temp_key.g_ConaxKey)+sizeof(temp_key.g_CryptoWorkshalfKey)+ sizeof(temp_key.g_VideoGuardKey),
     sizeof(temp_key.g_BissKey),(void *)(temp_key.g_BissKey));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.BissKeyNr= temp_head.BissKeyNr;
    break;
     case TYPE_VIACCESS2:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos)+sizeof(temp_key.g_ViaccessKey)+sizeof(temp_key.g_IrdetoKey)+sizeof(temp_key.g_SecaKey)+sizeof(temp_key.g_NagraKey)+sizeof(temp_key.g_ConaxKey)+sizeof(temp_key.g_CryptoWorkshalfKey)+ sizeof(temp_key.g_VideoGuardKey)+ sizeof(temp_key.g_BissKey),
     sizeof(temp_key.g_Viaccess2Key),(void *)(temp_key.g_Viaccess2Key));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.Via2KeyNr= temp_head.Via2KeyNr;
    break;
     case TYPE_NAGRA2:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos)+sizeof(temp_key.g_ViaccessKey)+sizeof(temp_key.g_IrdetoKey)+sizeof(temp_key.g_SecaKey)+sizeof(temp_key.g_NagraKey)+sizeof(temp_key.g_ConaxKey)+sizeof(temp_key.g_CryptoWorkshalfKey)+ sizeof(temp_key.g_VideoGuardKey)+ sizeof(temp_key.g_BissKey)+sizeof(temp_key.g_Viaccess2Key),
     sizeof(temp_key.g_Nagra2Key),(void *)(temp_key.g_Nagra2Key));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.Nag2KeyNr= temp_head.Nag2KeyNr;
     break;
     case TYPE_CRYPTOWORKS:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos)+sizeof(temp_key.g_ViaccessKey)+sizeof(temp_key.g_IrdetoKey)+sizeof(temp_key.g_SecaKey)+sizeof(temp_key.g_NagraKey)+sizeof(temp_key.g_ConaxKey)+sizeof(temp_key.g_CryptoWorkshalfKey)+ sizeof(temp_key.g_VideoGuardKey)+ sizeof(temp_key.g_BissKey)+sizeof(temp_key.g_Viaccess2Key)+sizeof(temp_key.g_Nagra2Key),
     sizeof(temp_key.g_CryptoWorksKey),(void *)(temp_key.g_CryptoWorksKey));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.CryptoWorksKeyNr= temp_head.CryptoWorksKeyNr;
    break;
     case TYPE_IRDETO2:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos)+sizeof(temp_key.g_ViaccessKey)+sizeof(temp_key.g_IrdetoKey)+sizeof(temp_key.g_SecaKey)+sizeof(temp_key.g_NagraKey)+sizeof(temp_key.g_ConaxKey)+sizeof(temp_key.g_CryptoWorkshalfKey)+ sizeof(temp_key.g_VideoGuardKey)+ sizeof(temp_key.g_BissKey)+sizeof(temp_key.g_Viaccess2Key)+sizeof(temp_key.g_Nagra2Key)+ sizeof(temp_key.g_CryptoWorksKey),
     sizeof(temp_key.g_Irdeto2Key),(void *)(temp_key.g_Irdeto2Key));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.Irdeto2Nr= temp_head.Irdeto2Nr;
    break;
     case TYPE_SECA2:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos)+sizeof(temp_key.g_ViaccessKey)+sizeof(temp_key.g_IrdetoKey)+sizeof(temp_key.g_SecaKey)+sizeof(temp_key.g_NagraKey)+sizeof(temp_key.g_ConaxKey)+sizeof(temp_key.g_CryptoWorkshalfKey)+ sizeof(temp_key.g_VideoGuardKey)+ sizeof(temp_key.g_BissKey)+sizeof(temp_key.g_Viaccess2Key)+sizeof(temp_key.g_Nagra2Key)+ sizeof(temp_key.g_CryptoWorksKey)+sizeof(temp_key.g_Irdeto2Key),
     sizeof(temp_key.g_Seca2Key),(void *)(temp_key.g_Seca2Key));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.Seca2KeyNr= temp_head.Seca2KeyNr;
    break;
     case TYPE_CONAX2:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos)+sizeof(temp_key.g_ViaccessKey)+sizeof(temp_key.g_IrdetoKey)+sizeof(temp_key.g_SecaKey)+sizeof(temp_key.g_NagraKey)+sizeof(temp_key.g_ConaxKey)+sizeof(temp_key.g_CryptoWorkshalfKey)+ sizeof(temp_key.g_VideoGuardKey)+ sizeof(temp_key.g_BissKey)+sizeof(temp_key.g_Viaccess2Key)+sizeof(temp_key.g_Nagra2Key)+ sizeof(temp_key.g_CryptoWorksKey)+sizeof(temp_key.g_Irdeto2Key)+ sizeof(temp_key.g_Seca2Key),
     sizeof(temp_key.g_Conax2Key),(void *)(temp_key.g_Conax2Key));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.Conax2KeyNr= temp_head.Conax2KeyNr;
    break;
    case TYPE_VIDEOGUARD2:
     len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,sizeof(temp_key.g_keyheaderinfos)+sizeof(temp_key.g_ViaccessKey)+sizeof(temp_key.g_IrdetoKey)+sizeof(temp_key.g_SecaKey)+sizeof(temp_key.g_NagraKey)+sizeof(temp_key.g_ConaxKey)+sizeof(temp_key.g_CryptoWorkshalfKey)+ sizeof(temp_key.g_VideoGuardKey)+ sizeof(temp_key.g_BissKey)+sizeof(temp_key.g_Viaccess2Key)+sizeof(temp_key.g_Nagra2Key)+ sizeof(temp_key.g_CryptoWorksKey)+sizeof(temp_key.g_Irdeto2Key)+ sizeof(temp_key.g_Seca2Key)+sizeof(temp_key.g_Conax2Key),
     sizeof(temp_key.g_VideoGuard2Key),(void *)(temp_key.g_VideoGuard2Key));
     MT_ASSERT(len !=0);
     temp_key.g_keyheaderinfos.VideoGuard2keyNr= temp_head.VideoGuard2keyNr;
    break;
     default:
      break;
 }
}

void sys_status_load_ucas_every(unsigned char Type)
{
  _decompress_ucas_keyinfo_every(Type);
}

BOOL  decompress_get_ucas_keyinfo(unsigned char Type)
{
 u32 len = 0;
 class_handle_t p_handle = class_get_handle_by_id(DM_CLASS_ID);
 key_head_info temp_head;

  len = dm_direct_read(p_handle,BSDATA_BLOCK_ID,0,
     sizeof(temp_head),(void *)(&temp_head));
 MT_ASSERT(len !=0);

 switch(Type)
  {
    case TYPE_VIACCESS:
      if((temp_head.ViaKeyNr != 0xFFFF) && (temp_head.ViaKeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_IRDETO:
      if((temp_head.IrdKeyNr != 0xFFFF) && (temp_head.IrdKeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_SECA:
      if((temp_head.SecaKeyNr!= 0xFFFF) && (temp_head.SecaKeyNr!= 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_NAGRA:
      if((temp_head.NagKeyNr != 0xFFFF) && (temp_head.NagKeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_BISS:
      if((temp_head.BissKeyNr != 0xFFFF) && (temp_head.BissKeyNr != 0))
        return TRUE;
      else
        return FALSE;
    break;
     case TYPE_VIACCESS2:
      if((temp_head.Via2KeyNr != 0xFFFF) && (temp_head.Via2KeyNr != 0))
        return TRUE;
      else
        return FALSE;
    break;
     case TYPE_NAGRA2:
      if((temp_head.Nag2KeyNr != 0xFFFF) && (temp_head.Nag2KeyNr != 0))
        return TRUE;
      else
        return FALSE;;
     break;
     case TYPE_CRYPTOWORKS:
      if((temp_head.CryptoWorksKeyNr != 0xFFFF) && (temp_head.CryptoWorksKeyNr != 0))
        return TRUE;
      else
        return FALSE;;
    break;
     case TYPE_IRDETO2:
      if((temp_head.Irdeto2Nr != 0xFFFF) && (temp_head.Irdeto2Nr != 0))
        return TRUE;
      else
        return FALSE;
    break;
     case TYPE_CONAX:
      if((temp_head.ConaxKeyNr != 0xFFFF) && (temp_head.ConaxKeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
    case TYPE_SECA2:
      if((temp_head.Seca2KeyNr!= 0xFFFF) && (temp_head.Seca2KeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_CONAX2:
      if((temp_head.Conax2KeyNr != 0xFFFF) && (temp_head.Conax2KeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_CRYPTOWORKSHALF:
      if((temp_head.CryptoWorkshalfKeyNr!= 0xFFFF) && (temp_head.CryptoWorkshalfKeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_VIDEOGUARD:
      if((temp_head.VideoGuardkeyNr!= 0xFFFF) && (temp_head.VideoGuardkeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_VIDEOGUARD2:
      if((temp_head.VideoGuard2keyNr!= 0xFFFF) && (temp_head.VideoGuard2keyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     default:
      return FALSE;
       break;
 }

}

BOOL  decompress_get_temp_ucas_keyinfo(unsigned char Type)
{
 key_head_info temp_head;
 memcpy(&temp_head,&(temp_key.g_keyheaderinfos),sizeof(temp_head));
 switch(Type)
  {
    case TYPE_VIACCESS:
      if((temp_head.ViaKeyNr != 0xFFFF) && (temp_head.ViaKeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_IRDETO:
      if((temp_head.IrdKeyNr != 0xFFFF) && (temp_head.IrdKeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_SECA:
      if((temp_head.SecaKeyNr!= 0xFFFF) && (temp_head.SecaKeyNr!= 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_NAGRA:
      if((temp_head.NagKeyNr != 0xFFFF) && (temp_head.NagKeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_BISS:
      if((temp_head.BissKeyNr != 0xFFFF) && (temp_head.BissKeyNr != 0))
        return TRUE;
      else
        return FALSE;
    break;
     case TYPE_VIACCESS2:
      if((temp_head.Via2KeyNr != 0xFFFF) && (temp_head.Via2KeyNr != 0))
        return TRUE;
      else
        return FALSE;
    break;
     case TYPE_NAGRA2:
      if((temp_head.Nag2KeyNr != 0xFFFF) && (temp_head.Nag2KeyNr != 0))
        return TRUE;
      else
        return FALSE;;
     break;
     case TYPE_CRYPTOWORKS:
      if((temp_head.CryptoWorksKeyNr != 0xFFFF) && (temp_head.CryptoWorksKeyNr != 0))
        return TRUE;
      else
        return FALSE;;
    break;
     case TYPE_IRDETO2:
      if((temp_head.Irdeto2Nr != 0xFFFF) && (temp_head.Irdeto2Nr != 0))
        return TRUE;
      else
        return FALSE;
    break;
     case TYPE_CONAX:
      if((temp_head.ConaxKeyNr != 0xFFFF) && (temp_head.ConaxKeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
    case TYPE_SECA2:
      if((temp_head.Seca2KeyNr!= 0xFFFF) && (temp_head.Seca2KeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_CONAX2:
      if((temp_head.Conax2KeyNr != 0xFFFF) && (temp_head.Conax2KeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_CRYPTOWORKSHALF:
      if((temp_head.CryptoWorkshalfKeyNr!= 0xFFFF) && (temp_head.CryptoWorkshalfKeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_VIDEOGUARD:
      if((temp_head.VideoGuardkeyNr!= 0xFFFF) && (temp_head.VideoGuardkeyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     case TYPE_VIDEOGUARD2:
      if((temp_head.VideoGuard2keyNr!= 0xFFFF) && (temp_head.VideoGuard2keyNr != 0))
        return TRUE;
      else
        return FALSE;
     break;
     default:
      return FALSE;
       break;
 }

}


void bl_sys_status_init(void)
{
  static u8 is_initialized = FALSE;
  BOOL ret = TRUE;

  if (is_initialized == TRUE)
  {
    return;
  }

  ret = ss_ctrl_init(SS_VAULE_BLOCK_ID, (u8*)&g_status, sizeof(sys_status_t));
  if (!ret)
  {
    //db_dvbs_restore_to_factory(PRESET_BLOCK_ID, 0);
    sys_status_load();
    ss_ctrl_clr_checksum(class_get_handle_by_id(SC_CLASS_ID));
  }

  is_initialized = TRUE;
}

void sys_set_3GinfoDefault(void)
{
	g3_conn_info_t	conn_info = {{0}, }; 
	strcpy(conn_info.apn,  "3gnet");
	strcpy(conn_info.dial_num,  "*99#");
	sys_status_set_3g_info(&conn_info);
}

void sys_set_stbNameDefault(void)
{
  char device_name[64];
  strcpy(device_name, "HHD BOX"); 
  sys_status_set_dlna_device_name(device_name);
}

void sys_set_wifiDefault(void)
{
  int i ;
  sys_status_t* status = sys_status_get();
  for(i=0; i<MAX_WIFI_NUM;i ++)
    {
    memset(&(status->wifi_set[i]),0,sizeof(wifi_info_t));
    }
}

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
    db_dvbs_restore_to_factory(PRESET_BLOCK_ID, 0);
    sys_status_load();
  }


 // sys_status_set_auto_sleep(0);
 // sys_status_save();

#if ENABLE_BISS_KEY
  // load ucas key
  _load_ucas_key_from_ss();
#endif

  is_initialized = TRUE;

}




void sys_status_load(void)
{
  class_handle_t handle = class_get_handle_by_id(DM_CLASS_ID);

  dm_read(handle, SS_DEFAULT_BLOCK_ID, 0, 0,
      sizeof(sys_status_t),(u8*)(&g_status));
  #if ENABLE_BISS_KEY
  _decompress_ucas_keyinfo();
  #endif
  sys_set_3GinfoDefault();
  sys_set_stbNameDefault();
  sys_set_wifiDefault();
  sys_status_save();  //need save
  ss_ctrl_set_checksum(class_get_handle_by_id(SC_CLASS_ID));

  #if ENABLE_BISS_KEY
  // load ucas key
  _load_ucas_key_from_ss();
  #endif
}


void sys_status_save(void)
{
  ss_ctrl_sync(class_get_handle_by_id(SC_CLASS_ID),
    (u8*)&g_status);
}

sys_status_t *sys_status_get(void)
{
#if 0
  OS_PRINTF("- SYS STATUS INFO - \n");
  OS_PRINTF("\t OSD PAL   = %d\n", g_status.osd_set.palette);
  OS_PRINTF("\t OSD TMOUT = %d\n", g_status.osd_set.timeout);
  OS_PRINTF("\t OSD TRANS = %d\n", g_status.osd_set.transparent);
  OS_PRINTF("\t LANGUAGE  = %d\n", g_status.lang_set.osd_text);
  OS_PRINTF("\t CURN MODE = %d\n", g_status.group_set.curn_mode);
  OS_PRINTF("\t CURN GROP = %d\n", g_status.group_set.curn_group);
  OS_PRINTF("\t TV MODE   = %d\n", g_status.av_set.tv_mode);
  OS_PRINTF("\t TV RATIO  = %d\n", g_status.av_set.tv_ratio);
#endif
  return &g_status;
}

void sys_status_get_key_info(key_ca_description *key_info)
{
  *key_info = temp_key;
}

void sys_status_set_key_info(unsigned char type,u8 index, void *key_info)
{
  switch(type)
    {
      case TYPE_BISS:
       memcpy(&(temp_key.g_BissKey[index]),(keyinf0biss*)key_info,sizeof(keyinf0biss));
       break;
      case TYPE_VIACCESS:
      memcpy(&(temp_key.g_ViaccessKey[index]),(keyinf0*)key_info,sizeof(keyinf0));
       break;
      case TYPE_IRDETO:
      memcpy(&(temp_key.g_IrdetoKey[index]),(keyinf0*)key_info,sizeof(keyinf0));
      break;
      case TYPE_SECA:
      memcpy(&(temp_key.g_SecaKey[index]),(keyinf0*)key_info,sizeof(keyinf0));
       break;
       case TYPE_NAGRA:
      memcpy(&(temp_key.g_NagraKey[index]),(keyinf0*)key_info,sizeof(keyinf0));
       break;
       case TYPE_VIACCESS2:
      memcpy(&(temp_key.g_Viaccess2Key[index]),(keyinf2*)key_info,sizeof(keyinf2));
       break;
      case TYPE_NAGRA2:
      memcpy(&(temp_key.g_Nagra2Key[index]),(keyinf2*)key_info,sizeof(keyinf2));
       break;
      case TYPE_CRYPTOWORKS:
      memcpy(&(temp_key.g_CryptoWorksKey[index]),(keyinf2*)key_info,sizeof(keyinf2));
       break;
      case TYPE_IRDETO2:
      memcpy(&(temp_key.g_Irdeto2Key[index]),(keyinf2*)key_info,sizeof(keyinf2));
      break;
      case TYPE_CONAX:
       memcpy(&(temp_key.g_ConaxKey[index]),(keyinf0*)key_info,sizeof(keyinf0));
      break;
      case TYPE_CONAX2:
       memcpy(&(temp_key.g_Conax2Key[index]),(keyinf2*)key_info,sizeof(keyinf2));
      break;
     case TYPE_SECA2:
       memcpy(&(temp_key.g_Seca2Key[index]),(keyinf2*)key_info,sizeof(keyinf2));
      break;
     case TYPE_CRYPTOWORKSHALF:
       memcpy(&(temp_key.g_CryptoWorkshalfKey[index]),(keyinf0*)key_info,sizeof(keyinf0));
      break;
     case TYPE_VIDEOGUARD:
       memcpy(&(temp_key.g_VideoGuardKey[index]),(keyinf0*)key_info,sizeof(keyinf0));
      break;
      case TYPE_VIDEOGUARD2:
       memcpy(&(temp_key.g_VideoGuard2Key[index]),(keyinf2*)key_info,sizeof(keyinf2));
      break;
       default:
        break;
   }
}

void sys_status_add_key_info(unsigned char type,void *key_info)
{
  switch(type)
    {
      case TYPE_BISS:
       memcpy(&(temp_key.g_BissKey[temp_key.g_keyheaderinfos.BissKeyNr++]),(keyinf0biss*)key_info,sizeof(keyinf0biss));
       break;
      case TYPE_VIACCESS:
      memcpy(&(temp_key.g_ViaccessKey[temp_key.g_keyheaderinfos.ViaKeyNr++]),(keyinf0*)key_info,sizeof(keyinf0));
       break;
      case TYPE_IRDETO:
      memcpy(&(temp_key.g_IrdetoKey[temp_key.g_keyheaderinfos.IrdKeyNr++]),(keyinf0*)key_info,sizeof(keyinf0));
      break;
      case TYPE_SECA:
      memcpy(&(temp_key.g_SecaKey[temp_key.g_keyheaderinfos.SecaKeyNr++]),(keyinf0*)key_info,sizeof(keyinf0));
       break;
       case TYPE_NAGRA:
      memcpy(&(temp_key.g_NagraKey[temp_key.g_keyheaderinfos.NagKeyNr++]),(keyinf0*)key_info,sizeof(keyinf0));
       break;
       case TYPE_VIACCESS2:
      memcpy(&(temp_key.g_Viaccess2Key[temp_key.g_keyheaderinfos.Via2KeyNr++]),(keyinf2*)key_info,sizeof(keyinf2));
       break;
      case TYPE_NAGRA2:
      memcpy(&(temp_key.g_Nagra2Key[temp_key.g_keyheaderinfos.Nag2KeyNr++]),(keyinf2*)key_info,sizeof(keyinf2));
       break;
      case TYPE_CRYPTOWORKS:
      memcpy(&(temp_key.g_CryptoWorksKey[temp_key.g_keyheaderinfos.CryptoWorksKeyNr++]),(keyinf2*)key_info,sizeof(keyinf2));
       break;
      case TYPE_IRDETO2:
      memcpy(&(temp_key.g_Irdeto2Key[temp_key.g_keyheaderinfos.Irdeto2Nr++]),(keyinf2*)key_info,sizeof(keyinf2));
      break;
      case TYPE_CONAX:
       memcpy(&(temp_key.g_ConaxKey[temp_key.g_keyheaderinfos.ConaxKeyNr++]),(keyinf0*)key_info,sizeof(keyinf0));
      break;
      case TYPE_CONAX2:
       memcpy(&(temp_key.g_Conax2Key[temp_key.g_keyheaderinfos.Conax2KeyNr++]),(keyinf2*)key_info,sizeof(keyinf2));
      break;
       case TYPE_SECA2:
       memcpy(&(temp_key.g_Seca2Key[temp_key.g_keyheaderinfos.Seca2KeyNr++]),(keyinf2*)key_info,sizeof(keyinf2));
      break;
      case TYPE_CRYPTOWORKSHALF:
       memcpy(&(temp_key.g_CryptoWorkshalfKey[temp_key.g_keyheaderinfos.CryptoWorkshalfKeyNr++]),(keyinf0*)key_info,sizeof(keyinf0));
      break;
     case TYPE_VIDEOGUARD:
       memcpy(&(temp_key.g_VideoGuardKey[temp_key.g_keyheaderinfos.VideoGuardkeyNr++]),(keyinf0*)key_info,sizeof(keyinf0));
      break;
     case TYPE_VIDEOGUARD2:
       memcpy(&(temp_key.g_VideoGuard2Key[temp_key.g_keyheaderinfos.VideoGuard2keyNr++]),(keyinf2*)key_info,sizeof(keyinf2));
      break;
       default:
        break;
   }
}

void sys_status_del_key_info(unsigned char type,u8 index)
{
  switch(type)
    {
      case TYPE_BISS:
       memmove(&(temp_key.g_BissKey[index]),&(temp_key.g_BissKey[index+1]),(sizeof(keyinf0biss))*((temp_key.g_keyheaderinfos.BissKeyNr--)-index));
       break;
     case TYPE_VIACCESS:
      memmove(&(temp_key.g_ViaccessKey[index]),&(temp_key.g_ViaccessKey[index+1]),(sizeof(keyinf0))*((temp_key.g_keyheaderinfos.ViaKeyNr--)-index));
       break;
      case TYPE_IRDETO:
      memmove(&(temp_key.g_IrdetoKey[index]),&(temp_key.g_IrdetoKey[index+1]),(sizeof(keyinf0))*((temp_key.g_keyheaderinfos.IrdKeyNr--)-index));
      break;
      case TYPE_SECA:
      memmove(&(temp_key.g_SecaKey[index]),&(temp_key.g_SecaKey[index+1]),(sizeof(keyinf0))*((temp_key.g_keyheaderinfos.SecaKeyNr--)-index));
       break;
     case TYPE_NAGRA:
      memmove(&(temp_key.g_NagraKey[index]),&(temp_key.g_NagraKey[index+1]),(sizeof(keyinf0))*((temp_key.g_keyheaderinfos.NagKeyNr--)-index));
       break;
       case TYPE_VIACCESS2:
      memmove(&(temp_key.g_Viaccess2Key[index]),&(temp_key.g_Viaccess2Key[index+1]),(sizeof(keyinf2))*((temp_key.g_keyheaderinfos.Via2KeyNr--)-index));
       break;
      case TYPE_NAGRA2:
      memmove(&(temp_key.g_Nagra2Key[index]),&(temp_key.g_Nagra2Key[index+1]),(sizeof(keyinf2))*((temp_key.g_keyheaderinfos.Nag2KeyNr--)-index));
       break;
      case TYPE_CRYPTOWORKS:
      memmove(&(temp_key.g_CryptoWorksKey[index]),&(temp_key.g_CryptoWorksKey[index+1]),(sizeof(keyinf2))*((temp_key.g_keyheaderinfos.CryptoWorksKeyNr--)-index));
       break;
      case TYPE_IRDETO2:
      memmove(&(temp_key.g_Irdeto2Key[index]),&(temp_key.g_Irdeto2Key[index+1]),(sizeof(keyinf2))*((temp_key.g_keyheaderinfos.Irdeto2Nr--)-index));
      break;
      case TYPE_CONAX:
       memmove(&(temp_key.g_ConaxKey[index]),&(temp_key.g_ConaxKey[index+1]),(sizeof(keyinf0))*((temp_key.g_keyheaderinfos.ConaxKeyNr--)-index));
      break;
      case TYPE_CONAX2:
       memmove(&(temp_key.g_Conax2Key[index]),&(temp_key.g_Conax2Key[index+1]),(sizeof(keyinf2))*((temp_key.g_keyheaderinfos.Conax2KeyNr--)-index));
      break;
      case TYPE_SECA2:
       memmove(&(temp_key.g_Seca2Key[index]),&(temp_key.g_Seca2Key[index+1]),(sizeof(keyinf2))*((temp_key.g_keyheaderinfos.Seca2KeyNr--)-index));
      break;
     case TYPE_CRYPTOWORKSHALF:
       memmove(&(temp_key.g_CryptoWorkshalfKey[index]),&(temp_key.g_CryptoWorkshalfKey[index+1]),(sizeof(keyinf0))*((temp_key.g_keyheaderinfos.CryptoWorkshalfKeyNr--)-index));
      break;
      case TYPE_VIDEOGUARD:
       memmove(&(temp_key.g_VideoGuardKey[index]),&(temp_key.g_VideoGuardKey[index+1]),(sizeof(keyinf0))*((temp_key.g_keyheaderinfos.VideoGuardkeyNr--)-index));
      break;
      case TYPE_VIDEOGUARD2:
       memmove(&(temp_key.g_VideoGuard2Key[index]),&(temp_key.g_VideoGuard2Key[index+1]),(sizeof(keyinf2))*((temp_key.g_keyheaderinfos.VideoGuard2keyNr--)-index));
      break;
       default:
        break;
   }
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


void sys_status_add_book_node(u8 index, book_pg_t *node)
{
  sys_status_t *status;
  book_pg_t *pg_info;

  MT_ASSERT(index <= MAX_BOOK_PG);

  status = sys_status_get();

  pg_info = &status->book_info.pg_info[index];

  memcpy(pg_info, node, sizeof(book_pg_t));

  sys_status_save();
}


void sys_status_delete_book_node(u8 index)
{
  sys_status_t *status;
  book_pg_t *pg_info;

  MT_ASSERT(index <= MAX_BOOK_PG);

  status = sys_status_get();

  pg_info = &status->book_info.pg_info[index];

  memset(pg_info, 0, sizeof(book_pg_t));

  pg_info->start_time.year = 2012;
  pg_info->start_time.month = 1;
  pg_info->start_time.day = 1;

  sys_status_save();
}

void sys_status_get_book_node(u8 index, book_pg_t *node)
{
  sys_status_t *status;

  MT_ASSERT(index <= MAX_BOOK_PG);

  status = sys_status_get();

  memcpy(node, &(status->book_info.pg_info[index]), sizeof(book_pg_t));
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

void sys_status_set_pwd_set(pwd_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->pwd_set = *p_set;
}

void sys_status_get_pwd_set(pwd_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->pwd_set;
}


void sys_status_set_time(time_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->time_set = *p_set;
}

void sys_status_get_time(time_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->time_set;
}

void sys_status_set_ipaddress(ip_address_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->ipaddress_set = *p_set;
}

void sys_status_get_ipaddress(ip_address_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->ipaddress_set;
}

void sys_status_save_wifi_password(wifi_info_t *p_set)
{
  sys_status_t *status;
  int index;
  u8 latest;
  u8 flag = 0;

  status = sys_status_get();
  
  for(index = 0;index < MAX_WIFI_NUM;index ++)
  {
      if((0 == flag) && (strcmp(p_set->essid, status->wifi_set[index].essid) == 0)&&(strlen(p_set->essid) ==  strlen(status->wifi_set[index].essid)))
      {
      	 latest = status->wifi_set[index].latest;
         status->wifi_set[index] = *p_set;
		 status->wifi_set[index].latest = latest;
         flag = 1;
      }
  }
  if(1 == flag)
  {
      return;
  }
  for(index = 0;index < MAX_WIFI_NUM;index ++)
  {
      if((0 == flag) && (strlen(status->wifi_set[index].essid) == 0))
      {
		status->wifi_set[index] = *p_set;
		status->wifi_set[index].latest = 0;

		flag = 1;       
      }
  }
  return;  
}


void sys_status_set_wifi_info(wifi_info_t *p_set)
{
  sys_status_t *status;
  int index;
  u8 flag = 0;

  status = sys_status_get();
  
  for(index = 0;index < MAX_WIFI_NUM;index ++)
  {
      if((0 == flag) && (strcmp(p_set->essid, status->wifi_set[index].essid) == 0)&&(strlen(p_set->essid) ==  strlen(status->wifi_set[index].essid)))
      {
         status->wifi_set[index] = *p_set;
         status->wifi_set[index].latest = 1;
         flag = 1;
      }
      else
      {
        status->wifi_set[index].latest = 0;
      }
  }
  if(1 == flag)
  {
      return;
  }
  for(index = 0;index < MAX_WIFI_NUM;index ++)
  {
      if((0 == flag) && (strlen(status->wifi_set[index].essid) == 0))
      {
         status->wifi_set[index] = *p_set;
         status->wifi_set[index].latest = 1;
         flag = 1;       
      }
      else
      {
        status->wifi_set[index].latest = 0;
      }
  }
  return;  
}

void sys_status_get_wifi_info(wifi_info_t *p_set,signed char* name)
{
  sys_status_t *status;
  int index;
  
  status = sys_status_get();

  for(index = 0;index < MAX_WIFI_NUM;index ++)
  {
      if((name == NULL) && (status->wifi_set[index].essid != NULL))
        {
          if(1 == status->wifi_set[index].latest)
          {
            *p_set = status->wifi_set[index];
            return ;
          }
        }
      if((name != NULL) && (strcmp(name, status->wifi_set[index].essid) == 0)&&(strlen(name) ==  strlen(status->wifi_set[index].essid)))
      {
        *p_set = status->wifi_set[index];
         return ;
      }
  }

}

void sys_status_set_net_config_info(net_config_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->net_config_set = *p_set;
}

void sys_status_get_net_config_info(net_config_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->net_config_set;
}

void sys_status_set_pppoe_config_info(pppoe_info_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->pppoe_set = *p_set;
}

void sys_status_get_pppoe_config_info(pppoe_info_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->pppoe_set;
}

void sys_status_set_youtube_config_info(youtube_config_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->youtube_config_set = *p_set;
}

void sys_status_get_youtube_config_info(youtube_config_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->youtube_config_set;
}

void sys_status_set_nm_simple_config_info(nm_simple_config_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->nm_simple_config_set = *p_set;
}

void sys_status_get_nm_simple_config_info(nm_simple_config_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->nm_simple_config_set;
}
void sys_status_set_play_set(play_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->play_set = *p_set;
}

void sys_status_get_play_set(play_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->play_set;
}

void sys_status_set_av_set(av_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->av_set = *p_set;
}

void sys_status_get_av_set(av_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->av_set;
}

void sys_status_set_osd_set(osd_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->osd_set = *p_set;
}

void sys_status_get_osd_set(osd_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->osd_set;
}

void sys_status_set_main_tp1(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->main_tp1 = *p_set;
}

void sys_status_get_main_tp1(dvbc_lock_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->main_tp1;
}

void sys_status_set_lang_set(language_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->lang_set = *p_set;
}

void sys_status_get_lang_set(language_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->lang_set;
}

void sys_status_set_boot_wizard(u16 *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->reserved2 = *p_set;
}

void sys_status_get_boot_wizard(u16 *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->reserved2;
}

BOOL sys_status_get_fav_name(u8 index, u16 *name)
{
  sys_status_t *status;

  if(index > MAX_FAV_CNT)
  {
    return FALSE;
  }

  status = sys_status_get();

  memcpy(name, status->fav_set.fav_group[index].fav_name, sizeof(u16) * MAX_FAV_NAME_LEN);

  return TRUE;
}

BOOL sys_status_set_fav_name(u8 index, u16 *name)
{
  sys_status_t *status;

  if(index > MAX_FAV_CNT)
  {
    return FALSE;
  }

  status = sys_status_get();

  memcpy(status->fav_set.fav_group[index].fav_name, name, sizeof(u16) * MAX_FAV_NAME_LEN);

  return TRUE;
}
void sys_status_set_local_set(local_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->local_set = *p_set;

}

void sys_status_get_scan_param(scan_param_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->scan_param;
}

void sys_status_set_scan_param(scan_param_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->scan_param = *p_set;

}

void sys_status_get_local_set(local_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->local_set;
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


void sys_status_set_time_zone(void)
{
  time_set_t time_set;
  s8 zone;

  sys_status_get_time(&time_set);

  zone = (s8)(time_set.gmt_offset - 23);

  time_set_zone(zone);
  time_set_summer_time((BOOL)time_set.summer_time);
}

u32 sys_status_get_standby_time_out()
{
  return sys_status_get()->time_out;
}

void sys_status_set_standby_time_out(u32 time_out)
{
  sys_status_get()->time_out = time_out;

  sys_status_save();
}

void sys_status_get_utc_time(utc_time_t *p_time)
{
  time_set_t t_set;

  sys_status_get_time(&t_set);

  memcpy(p_time, &(t_set.sys_time), sizeof(utc_time_t));

  return;
}

void sys_status_set_utc_time(utc_time_t *p_time)
{
  time_set_t t_set;

  sys_status_get_time(&t_set);

  memcpy(&(t_set.sys_time), p_time, sizeof(utc_time_t));

  sys_status_set_time(&t_set);

  //sys_status_save();
}

void sys_status_get_sleep_time(utc_time_t *p_time)
{
  sys_status_t *p_status;

  p_status = sys_status_get();

  memcpy(p_time, &(p_status->sleep_time), sizeof(utc_time_t));

  return;
}

void sys_status_set_sleep_time(utc_time_t *p_time)
{
  sys_status_t *p_status;

  p_status = sys_status_get();

  memcpy(&(p_status->sleep_time), p_time, sizeof(utc_time_t));

  sys_status_save();
}

void sys_status_get_pic_showmode(pic_showmode_t *p_showmode, BOOL is_net)
{
  sys_status_t *status;

  status = sys_status_get();

  if(is_net)
  {
    memcpy(p_showmode, &status->pic_showmode_net, sizeof(pic_showmode_t));
  }
  else
  {
    memcpy(p_showmode, &status->pic_showmode, sizeof(pic_showmode_t));
  }
}

void sys_status_set_pic_showmode(pic_showmode_t *p_showmode)
{
  sys_status_t *status;

  status = sys_status_get();

  status->pic_showmode = *p_showmode;
}

char **sys_status_get_text_encode(void)
{
  return g_text_encode;
}

char **sys_status_get_lang_code(BOOL is_2_b)
{
  return is_2_b ? g_lang_code_b : g_lang_code_t;
}


/*!
   convert unit of longitude from unsigned int to double.

   \param[in] u_longitude west longitude with W_LONGITUDE_MASK
   \return longitude 0 to 180 east, -180 to 0 west.
  */
double sys_status_convert_longitude(u32 u_longitude)
{
  double d_longitude = 0;

  if (IS_W_LONGITUDE(u_longitude))
  {
    LONGITUDE_NORMALIZE(u_longitude);
    d_longitude = 0 - (double)u_longitude;
  }
  else
  {
    d_longitude = (double)u_longitude;
  }
  d_longitude = d_longitude / 100.0;
  return d_longitude;
}

/*!
   convert unit of latitude from unsigned int to double.

   \param[in] u_longitude sorth latitude with S_LATITUDE_MASK
   \return longitude 0 to 90 north, -90 to 0 sorth.
  */
double sys_status_convert_latitude(u32 u_latitude)
{
  double d_latitude = 0;

  if (IS_S_LATITUDE(u_latitude))
  {
    LATITUDE_NORMALIZE(u_latitude);
    d_latitude = 0 - (double)u_latitude;
  }
  else
  {
    d_latitude = (double)u_latitude;
  }
  d_latitude = d_latitude / 100.0;
  return d_latitude;
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

  memcpy((void *)&status->ota_info, (void *)p_otai, sizeof(ota_info_t));

  return;
}

BOOL sys_status_get_age_limit(u16 *p_agelimit)
{
  sys_status_t *status = NULL;

  status = sys_status_get();

  *p_agelimit =status->age_limit; //1;//status->age_limit;

  return TRUE;
}


BOOL sys_status_set_age_limit(u16 agelimit)
{
  sys_status_t *status = NULL;

  if(AGE_LIMIT_MAX_CNT <= agelimit)
  {
    return FALSE;
  }

  status = sys_status_get();
  status->age_limit = agelimit;

  return TRUE;
}

void sys_status_set_usb_work_partition(u8 partition)
{
  sys_status_t *status = NULL;

  status = sys_status_get();  
  
  status->usb_work_partition = partition;  
}

u8 sys_status_get_usb_work_partition(void)
{
  sys_status_t *status = NULL;

  status = sys_status_get();

  return status->usb_work_partition;
}

u32 sys_status_get_auto_sleep(void)
{
  sys_status_t *status = NULL;

  status = sys_status_get();

  return status->auto_sleep;
}

void sys_status_set_auto_sleep(u32 auto_sleep)
{
  sys_status_t *status = NULL;

  status = sys_status_get();

  status->auto_sleep = auto_sleep;
}

 void sys_status_resume_ca_key(void)
 {
    void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
    MT_ASSERT(p_dm_handle != NULL);

    dm_direct_erase(p_dm_handle, BSDATA_BLOCK_ID, 0, sizeof(temp_key));
    memset(&temp_key,0,sizeof(temp_key));
 }

 void  sys_status_save_key_info(void)
  {
    void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);
     int key_addr_offset = 0;

     dm_direct_erase(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(key_ca_description));

     dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(key_head_info),(u8 *)&(temp_key.g_keyheaderinfos));
     key_addr_offset += sizeof(key_head_info);

     dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_ViaccessKey[0]) *VIACCESS_MAX_KEYINFOS ,(u8 *)&(temp_key.g_ViaccessKey));
     key_addr_offset += sizeof(temp_key.g_ViaccessKey[0]) * VIACCESS_MAX_KEYINFOS;

     dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_IrdetoKey[0]) *IRDETO_MAX_KEYINFOS ,(u8 *)&(temp_key.g_IrdetoKey));
     key_addr_offset += sizeof(temp_key.g_IrdetoKey[0]) * IRDETO_MAX_KEYINFOS;

     dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_SecaKey[0]) *SECA_MAX_KEYINFOS ,(u8 *)&(temp_key.g_SecaKey));
     key_addr_offset += sizeof(temp_key.g_SecaKey[0]) * SECA_MAX_KEYINFOS;

     dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_NagraKey[0]) *NAGRA_MAX_KEYINFOS ,(u8 *)&(temp_key.g_NagraKey));
     key_addr_offset += sizeof(temp_key.g_NagraKey[0]) * NAGRA_MAX_KEYINFOS;

     dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_ConaxKey[0]) *CONAX_MAX_KEYINFOS ,(u8 *)&(temp_key.g_ConaxKey));
     key_addr_offset += sizeof(temp_key.g_ConaxKey[0]) * CONAX_MAX_KEYINFOS;

     dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_CryptoWorkshalfKey[0]) *CRYPTOWORKSHALF_MAX_KEYINFOS ,(u8 *)&(temp_key.g_CryptoWorkshalfKey));
     key_addr_offset += sizeof(temp_key.g_CryptoWorkshalfKey[0]) * CRYPTOWORKSHALF_MAX_KEYINFOS;

     dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_VideoGuardKey[0]) *VIDEOGUARD_MAX_KEYINFOS ,(u8 *)&(temp_key.g_VideoGuardKey));
     key_addr_offset += sizeof(temp_key.g_VideoGuardKey[0]) * VIDEOGUARD_MAX_KEYINFOS;

     dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_BissKey[0]) *BISS_MAX_KEYINFOS ,(u8 *)&(temp_key.g_BissKey));
     key_addr_offset += sizeof(temp_key.g_BissKey[0]) * BISS_MAX_KEYINFOS;

     dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_Viaccess2Key[0]) *VIACCESS2_MAX_KEYINFOS ,(u8 *)&(temp_key.g_Viaccess2Key));
     key_addr_offset += sizeof(temp_key.g_Viaccess2Key[0]) * VIACCESS2_MAX_KEYINFOS;

      dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_Nagra2Key[0]) *NAGRA2_MAX_KEYINFOS ,(u8 *)&(temp_key.g_Nagra2Key));
     key_addr_offset += sizeof(temp_key.g_Nagra2Key[0]) * NAGRA2_MAX_KEYINFOS;

      dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_CryptoWorksKey[0]) *CRYPTOWORKS_MAX_KEYINFOS ,(u8 *)&(temp_key.g_CryptoWorksKey));
     key_addr_offset += sizeof(temp_key.g_CryptoWorksKey[0]) * CRYPTOWORKS_MAX_KEYINFOS;

      dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_Irdeto2Key[0]) *IRDETO2_MAX_KEYINFOS ,(u8 *)&(temp_key.g_Irdeto2Key));
     key_addr_offset += sizeof(temp_key.g_Irdeto2Key[0]) * IRDETO2_MAX_KEYINFOS;

     dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_Seca2Key[0]) *SECA2_MAX_KEYINFOS ,(u8 *)&(temp_key.g_Seca2Key));
     key_addr_offset += sizeof(temp_key.g_Seca2Key[0]) * SECA2_MAX_KEYINFOS;

     dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_Conax2Key[0]) *CONAX2_MAX_KEYINFOS ,(u8 *)&(temp_key.g_Conax2Key));
     key_addr_offset += sizeof(temp_key.g_Conax2Key[0]) * CONAX2_MAX_KEYINFOS;

     dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(temp_key.g_VideoGuard2Key[0]) *VIDEOGUARD2_MAX_KEYINFOS ,(u8 *)&(temp_key.g_VideoGuard2Key));
     key_addr_offset += sizeof(temp_key.g_VideoGuard2Key[0]) * VIDEOGUARD2_MAX_KEYINFOS;

 }


 void sys_status_set_timeshift_switch(BOOL is_on)
{
  sys_status_t *status = NULL;

  status = sys_status_get();  
  
  status->timeshift_switch = is_on;  
}

BOOL sys_status_get_timeshift_switch(void)
{
  sys_status_t *status = NULL;

  status = sys_status_get();  
  
  return (BOOL)status->timeshift_switch;
}

void sys_status_set_ts_delay_set(delay_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->delay_set = *p_set;
}

void sys_status_get_ts_delay_set(delay_set_t *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->delay_set;
}

BOOL sys_status_set_ip_path_set(u8 index,u16 *name)
{
 sys_status_t *status;

  if(index > MAX_IP_CNT)
  {
    return FALSE;
  }

  status = sys_status_get();

  memcpy(status->ip_path_set.ip_group[index].ip_name, name, sizeof(u16) * 32);

  return TRUE;
}

BOOL sys_status_get_ip_path_set(u8 index,u16 *name)
{
  sys_status_t *status;

  if(index > MAX_IP_CNT)
  {
    return FALSE;
  }

  status = sys_status_get();

  memcpy(name, status->ip_path_set.ip_group[index].ip_name, sizeof(u16) * 32);

  return TRUE;
}

BOOL sys_status_set_ip_account(u8 index,u8 *account)
{
 sys_status_t *status;

  if(index > MAX_IP_CNT)
  {
    return FALSE;
  }

  status = sys_status_get();

  memcpy(status->ip_path_set.ip_group[index].ip_account, account, sizeof(u8) * 32);
  sys_status_save();

  return TRUE;
}

BOOL sys_status_get_ip_account(u8 index,u8 *account)
{
  sys_status_t *status;

  if(index > MAX_IP_CNT)
  {
    return FALSE;
  }

  status = sys_status_get();

  memcpy(account, status->ip_path_set.ip_group[index].ip_account, sizeof(u8) * 32);

  return TRUE;
}

BOOL sys_status_set_ip_passwd(u8 index,u8 *passwd)
{
 sys_status_t *status;

  if(index > MAX_IP_CNT)
  {
    return FALSE;
  }

  status = sys_status_get();

  memcpy(status->ip_path_set.ip_group[index].ip_password, passwd, sizeof(u8) * 32);
  sys_status_save();

  return TRUE;
}

BOOL sys_status_get_ip_passwd(u8 index,u8 *passwd)
{
  sys_status_t *status;

  if(index > MAX_IP_CNT)
  {
    return FALSE;
  }

  status = sys_status_get();

  memcpy(passwd, status->ip_path_set.ip_group[index].ip_password, sizeof(u8) * 32);

  return TRUE;
}




void sys_status_set_ip_path_cnt(u8 ip_path_cnt)
{
  sys_status_t *status = NULL;

  status = sys_status_get();  
  
  status->ip_path_cnt= ip_path_cnt;  
}

u8 sys_status_get_ip_path_cnt(void)
{
  sys_status_t *status = NULL;

  status = sys_status_get();

  return status->ip_path_cnt;
}

BOOL sys_status_set_city(u8 index,u16 *name)
{
 sys_status_t *status;

  if(index > MAX_CITY_CNT)
  {
    return FALSE;
  }

  status = sys_status_get();

  memcpy(status->city_path_set.city_group[index].city_name, name, sizeof(u16) * 32);

  return TRUE;
}

BOOL sys_status_get_city(u8 index,u16 *name)
{
  sys_status_t *status;

  if(index > MAX_CITY_CNT)
  {
    return FALSE;
  }

  status = sys_status_get();

  memcpy(name, status->city_path_set.city_group[index].city_name, sizeof(u16) * 32);

  return TRUE;
}

void sys_status_set_city_cnt(u8 city_cnt)
{
  sys_status_t *status = NULL;

  status = sys_status_get();  
  
  status->city_cnt= city_cnt;  
}

u8 sys_status_get_city_cnt(void)
{
  sys_status_t *status = NULL;

  status = sys_status_get();

  return status->city_cnt;
}

#if 0
BOOL sys_status_get_mac(u8 index, char *name)
{
  sys_status_t *status;

  status = sys_status_get();
 switch(index)
 {
  case 0:
    memcpy(name, status->mac_set.mac_one, 2);
    break;
  case 1:
    memcpy(name, status->mac_set.mac_two, 2);
    break;
  case 2:
    memcpy(name, status->mac_set.mac_three, 2);
    break;
  case 3:
    memcpy(name, status->mac_set.mac_four, 2);
    break;
 case 4:
    memcpy(name, status->mac_set.mac_five, 2);
    break;
 case 5:
    memcpy(name, status->mac_set.mac_six, 2);
    break;
 default:
    break;
 }
  return TRUE;
}

BOOL sys_status_set_mac(u8 index, char *name)
{
  sys_status_t *status;


  status = sys_status_get();

  switch(index)
 {
  case 0:
    memcpy(status->mac_set.mac_one, name, 2);
    break;
  case 1:
    memcpy(status->mac_set.mac_two, name, 2);
    break;
  case 2:
    memcpy( status->mac_set.mac_three, name, 2);
    break;
  case 3:
    memcpy( status->mac_set.mac_four, name, 2);
    break;
 case 4:
    memcpy(status->mac_set.mac_five, name, 2);
    break;
 case 5:
    memcpy(status->mac_set.mac_six, name, 2);
    break;
 default:
    break;
 }

  return TRUE;
}

#endif
void sys_status_get_dlna_device_name(char* dlna_name)
{
	sys_status_t *status;

	status = sys_status_get();
	
	memcpy(dlna_name, status->dlna_device_name, 64);

}

void sys_status_set_dlna_device_name(char* dlna_name)
{
    sys_status_t *status;

    status = sys_status_get();

	strcpy(status->dlna_device_name, dlna_name);

   sys_status_save();

}

void sys_status_get_fav_livetv(live_tv_fav *fav_livetv)
{
  sys_status_t *status;

  status = sys_status_get();

  memcpy(fav_livetv, &(status->fav_live_pg), sizeof(live_tv_fav));
}

void sys_status_set_fav_livetv(live_tv_fav *fav_livetv)
{
  sys_status_t *status;

  status = sys_status_get();

  memcpy(&(status->fav_live_pg),fav_livetv, sizeof(live_tv_fav));
}


void sys_status_set_fav_livetv_info(u32 index,live_tv_fav_info *fav_livetv_info)
{
  sys_status_t *status;

  status = sys_status_get();

  memcpy(&(status->fav_live_pg.live_fav_pg[index]), fav_livetv_info, sizeof(live_tv_fav_info));
}

void sys_status_get_fav_livetv_info(u32 index,live_tv_fav_info *fav_livetv_info)
{
  sys_status_t *status;

  status = sys_status_get();

  memcpy(fav_livetv_info, &(status->fav_live_pg.live_fav_pg[index]), sizeof(live_tv_fav_info));
}


void sys_status_del_fav_livetv_info(u32 index)
{
  sys_status_t *status;
  u32 cnt = 0;

  status = sys_status_get();
  
  cnt = status->fav_live_pg.live_tv_total_num - (index + 1);
  status->fav_live_pg.live_tv_total_num --;

  memmove(&(status->fav_live_pg.live_fav_pg[index]),&(status->fav_live_pg.live_fav_pg[index + 1]),
           sizeof(live_tv_fav_info) * cnt);
}

void sys_status_set_fav_livetv_total_num(u32 total_num)
{
  sys_status_t *status;

  status = sys_status_get();

  status->fav_live_pg.live_tv_total_num = total_num;
}

void sys_status_get_fav_livetv_total_num(u32 *total_num)
{
  sys_status_t *status;

  status = sys_status_get();
    
  *total_num = status->fav_live_pg.live_tv_total_num;
}

void sys_status_get_fav_iptv(iptv_fav *fav_iptv)
{
  sys_status_t *status;

  status = sys_status_get();

  memcpy(fav_iptv, &(status->fav_iptv_pg), sizeof(iptv_fav));
}

void sys_status_set_fav_iptv(iptv_fav *fav_iptv)
{
  sys_status_t *status;

  status = sys_status_get();

  memcpy(&(status->fav_iptv_pg),fav_iptv, sizeof(iptv_fav));
}


void sys_status_set_fav_iptv_info(u32 index,iptv_fav_info *fav_iptv_info)
{
  sys_status_t *status;

  status = sys_status_get();

  memcpy(&(status->fav_iptv_pg.iptv_fav_pg[index]), fav_iptv_info, sizeof(iptv_fav_info));
}

void sys_status_get_fav_iptv_info(u32 index,iptv_fav_info *ip_tv_info)
{
  sys_status_t *status;

  status = sys_status_get();

  memcpy(ip_tv_info, &(status->fav_iptv_pg.iptv_fav_pg[index]), sizeof(iptv_fav_info));
}

void sys_status_del_fav_iptv_info(u32 index)
{
  sys_status_t *status;
  u32 cnt = 0;

  status = sys_status_get();

  cnt = status->fav_iptv_pg.iptv_total_num - (index + 1);
  status->fav_iptv_pg.iptv_total_num --;
  memmove( &(status->fav_iptv_pg.iptv_fav_pg[index]),&(status->fav_iptv_pg.iptv_fav_pg[index + 1]), 
         sizeof(iptv_fav_info) * cnt);
}

void sys_status_set_fav_iptv_total_num(u32 total_num)
{
  sys_status_t *status;

  status = sys_status_get();

  status->fav_iptv_pg.iptv_total_num= total_num;
}

void sys_status_get_fav_iptv_total_num(u32 *total_num)
{
  sys_status_t *status;

  status = sys_status_get();
    
  *total_num = status->fav_iptv_pg.iptv_total_num;
}

db_conn_play_item *sys_status_get_conn_play_table(void)
{
  sys_status_t *status;

  status = sys_status_get();

  return status->conn_play_table;
}

void sys_status_read_conn_play_item(u16 pos, db_conn_play_item *item)
{
  sys_status_t *status;

  MT_ASSERT(pos < MAX_CONN_PLAY_CNT);
  status = sys_status_get();

  memcpy(item, status->conn_play_table + pos, sizeof(db_conn_play_item));
}

void sys_status_write_conn_play_item(u16 pos, db_conn_play_item *item)
{
  sys_status_t *status;

  MT_ASSERT(pos < MAX_CONN_PLAY_CNT);
  status = sys_status_get();

  memcpy(status->conn_play_table + pos, item, sizeof(db_conn_play_item));
}

s32 sys_status_find_free_conn_play_item(void)
{
  sys_status_t *status;
  db_conn_play_item *p_table;
  u16 pos;

  status = sys_status_get();
  p_table = status->conn_play_table;

  for (pos = 0; pos < MAX_CONN_PLAY_CNT; pos++)
  {
    if (p_table[pos].is_used == FALSE)
    {
        return pos;
    }
  }

  return -1;
}

db_play_hist_item *sys_status_get_play_hist_table(void)
{
  sys_status_t *status;

  status = sys_status_get();

  return status->play_hist_table;
}

void sys_status_read_play_hist_item(u16 pos, db_play_hist_item *item)
{
  sys_status_t *status;

  MT_ASSERT(pos < MAX_PLAY_HIST_CNT);
  status = sys_status_get();

  memcpy(item, status->play_hist_table + pos, sizeof(db_play_hist_item));
}

void sys_status_write_play_hist_item(u16 pos, db_play_hist_item *item)
{
  sys_status_t *status;

  MT_ASSERT(pos < MAX_PLAY_HIST_CNT);
  status = sys_status_get();

  memcpy(status->play_hist_table + pos, item, sizeof(db_play_hist_item));
}

s32 sys_status_find_free_play_hist_item(void)
{
  sys_status_t *status;
  db_play_hist_item *p_table;
  u16 pos;

  status = sys_status_get();
  p_table = status->play_hist_table;

  for (pos = 0; pos < MAX_PLAY_HIST_CNT; pos++)
  {
    if (p_table[pos].is_used == FALSE)
    {
        return pos;
    }
  }

  return -1;
}

gprs_info_t *sys_status_get_gprs_info(void)
{
  sys_status_t *status;

  status = sys_status_get();

  return &status->gprs_info;
}

void sys_status_set_gprs_info(gprs_info_t *p_gprs_info)
{
  sys_status_t *status;

  MT_ASSERT(p_gprs_info != NULL);

  status = sys_status_get();

  memcpy((void *)&status->gprs_info, (void *)p_gprs_info, sizeof(gprs_info_t));

  return;
}

g3_conn_info_t *sys_status_get_3g_info(void)
{
  sys_status_t *status;

  status = sys_status_get();

  return &status->g3_conn_info;
}

void sys_status_set_3g_info(g3_conn_info_t *p_3g_info)
{
  sys_status_t *status;

  MT_ASSERT(p_3g_info != NULL);

  status = sys_status_get();

  memcpy((void *)&status->g3_conn_info, (void *)p_3g_info, sizeof(g3_conn_info_t));

  return;
}

void sys_status_set_country_set(t2_search_info *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  status->t2_search= *p_set;
}

void sys_status_get_country_set(t2_search_info *p_set)
{
  sys_status_t *status;

  status = sys_status_get();

  *p_set = status->t2_search;
}


void sys_status_get_epg_lang(epg_lang_t *e_lang)
{
  sys_status_t *status = NULL;

  if(NULL != e_lang)
  {
    status = sys_status_get(); 
  
    memcpy(e_lang, &status->e_lang_set, sizeof(epg_lang_t));
  }
}

void sys_status_set_epg_lang(epg_lang_t *e_lang)
{
  sys_status_t *status = NULL;

  if(NULL != e_lang)
  {
    status = sys_status_get(); 
  
    memcpy(&status->e_lang_set, e_lang, sizeof(epg_lang_t));
  }
}

u8 sys_status_get_categories_count(void)
{
  sys_status_t *status;

  status = sys_status_get();

  return 0;//status->categories_count;
}

void sys_status_set_categories_count(u8 count)
{
  sys_status_t *status;

  status = sys_status_get();

  //status->categories_count = count;

  sys_status_save();
}

u16 sys_status_get_bouquet_id(u16 index)
{
  sys_status_t *status;

  status = sys_status_get();

  return 0;//status->bouquet_id[index];
}

void sys_status_set_bouquet_id(u16 index, u16 bouquet_id)
{
  sys_status_t *status;

  status = sys_status_get();

  //status->bouquet_id[index] = bouquet_id;

  sys_status_save();
}

u16 sys_status_get_default_logic_num(void)
{
  sys_status_t *status;

  status = sys_status_get();

  return 0;//status->default_logic_number;
}

void sys_status_set_default_logic_num(u16 num)
{
  sys_status_t *status;

  status = sys_status_get();

  //status->default_logic_number = num;

  sys_status_save();
}

void sys_status_get_nit_version(u32 *p_nit_ver)
{
  sys_status_t *p_status;

  p_status = sys_status_get();

  *p_nit_ver = p_status->nit_version;
}

void sys_status_set_nit_version(u32 nit_ver)
{
  sys_status_t *p_status;

  p_status = sys_status_get();

  p_status->nit_version = nit_ver;

  sys_status_save();
}

BOOL sys_status_get_ota_preset(void *p_nim_param)
{
  sys_status_t *p_status = NULL;
  nim_para_t *p_ota_tp = (nim_para_t *)p_nim_param;
  MT_ASSERT(NULL != p_ota_tp);
  p_status = sys_status_get();

  p_ota_tp->lock_mode = p_status->ota_info.sys_mode;
  p_ota_tp->data_pid = p_status->ota_info.download_data_pid;
  
  switch(p_ota_tp->lock_mode)
  {
    case SYS_DVBS:
    {
      memcpy(&(p_ota_tp->locks), 
             &(p_status->ota_info.locks), 
             sizeof(dvbs_lock_info_t));
      break;
    }
    case SYS_DVBC:
    {
      memcpy(&(p_ota_tp->lockc), 
             &(p_status->ota_info.lockc), 
             sizeof(dvbc_lock_info_t));
	  OS_PRINTF("[%s] fre=0x%x sym=0x%x pid=0x%x \n",__FUNCTION__,
	  	p_status->ota_info.lockc.tp_freq,p_status->ota_info.lockc.tp_sym,
	  	p_status->ota_info.download_data_pid);	  
      break;
    }
	case SYS_DVBT2:
    case SYS_DTMB:
    {
      memcpy(&(p_ota_tp->lockt), 
             &(p_status->ota_info.lockt), 
             sizeof(dvbt_lock_info_t));
	  OS_PRINTF("[%s] fre=0x%x pid=0x%x \n",__FUNCTION__,
	  	p_status->ota_info.lockt.tp_freq,
	  	p_status->ota_info.download_data_pid);	  	  
      break;
    }
    default:
    {
      return FALSE;
    }
  }

  return TRUE;
}

// TODO: not finish yet!!!!!!!!!!!!
BOOL sys_set_vod_user_id(s8 *id, u32 length)
{
	u32 addr;
	charsto_device_t *p_charsto = NULL;
	charsto_prot_status_t st_set = {0},st_old = {0};
	int ret;
	
	MT_ASSERT(length < USER_ID_MAX_LEN);
	OS_PRINTF("user id write:w len[%d],context:[%s]\n",length,(char*)id);
	/*get address */
	addr= dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), IDENTITY_BLOCK_ID) - get_flash_addr();
	OS_PRINTF("block addr = %#x\n",addr);
	/*get handle*/
	p_charsto = (charsto_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);	
	/*set unprotect*/
       dev_io_ctrl(p_charsto, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);
	st_set.prt_t = PRT_UNPROT_ALL;
	dev_io_ctrl(p_charsto, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);

	/*write data*/
	addr += USER_ID_OFFSET;
	ret = charsto_writeonly(p_charsto, addr, (u8*)id, length);
	OS_PRINTF("ret = %#x\n",ret);

	/*set protect*/
	dev_io_ctrl(p_charsto, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);
	
	return TRUE;
}
// TODO: not finish yet!!!!!!!!!!!!
BOOL sys_get_vod_user_id(s8 *id, u32 length)
{
	u32 read_len;
	int i;
	read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
						IDENTITY_BLOCK_ID, 0, USER_ID_OFFSET,
						length,
						(u8 *)id); 
	OS_PRINTF("user id:read_len[%d],context:[%s]\n",read_len,(char*)id);
	if ((u8)id[0] == 0xff || (u8)id[0] == 0)
		return FALSE;
	for(i=0;i<length;i++)
	{
		if ((u8)id[i] == 0xFF)
			id[i] = 0;
	}
	return TRUE;
}


BOOL sys_set_iqy_info(char *id, u32 length)
{
	u32 addr;
	charsto_device_t *p_charsto = NULL;
	charsto_prot_status_t st_set = {0},st_old = {0};
	int ret;

	MT_ASSERT(length < IQY_IDENTITY_INFO_MAX_LEN);
	OS_PRINTF("user id write:w len[%d],context:[%s]\n",length,(char*)id);
	/*get address */
	addr= dm_get_block_addr(class_get_handle_by_id(DM_CLASS_ID), IDENTITY_BLOCK_ID) - get_flash_addr();
	OS_PRINTF("block addr = %#x\n",addr);
	/*get handle*/
	p_charsto = (charsto_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);	
	/*set unprotect*/
       dev_io_ctrl(p_charsto, CHARSTO_IOCTRL_GET_STATUS, (u32)&st_old);	
	st_set.prt_t = PRT_UNPROT_ALL;
	dev_io_ctrl(p_charsto, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_set);
	/*write data*/
	addr += IQY_IDENTITY_INFO_OFFSET;
	ret = charsto_writeonly(p_charsto, addr, (u8*)id, length);
	OS_PRINTF("ret = %#x\n",ret);

	/*set protect*/
	dev_io_ctrl(p_charsto, CHARSTO_IOCTRL_SET_STATUS, (u32)&st_old);
	
	return TRUE;
}
// TODO: not finish yet!!!!!!!!!!!!
BOOL sys_get_iqy_info(char *id, u32 length)
{
	u32 read_len;
	int i;
	read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
						IDENTITY_BLOCK_ID, 0, IQY_IDENTITY_INFO_OFFSET,
						length,
						(u8 *)id); 
	OS_PRINTF("user id:read_len[%d],context:[%s]\n",read_len,(char*)id);
	if ((u8)id[0] == 0xff || (u8)id[0] == 0)
		return FALSE;
	for(i=0;i<length;i++)
	{
		if ((u8)id[i] == 0xFF)
			id[i] = 0;
	}
	return TRUE;
}


#define GET_MAC_FROM_FLASH_OTP
char*  sys_status_get_mac_string(void)
{

	static u8 mac[STB_MAC_ADDR_LEN+4] = {0};
	static int if_have_read = 0;
#ifdef GET_MAC_FROM_FLASH_OTP		
	if (if_have_read == 0)
	{
		dm_read(class_get_handle_by_id(DM_CLASS_ID),
							IDENTITY_BLOCK_ID, 0, STB_MAC_ADDR_START,
							STB_MAC_ADDR_LEN,
							(u8 *)mac); 
		OS_PRINTF("mac 1[%s]\n",(char*)mac);
		if ((u8)mac[0] == 0xff)
		{
			u32 s,ms,us;
			u32 ticks = 0;
			sys_status_t *status;

			status = sys_status_get();
			mtos_systime_get(&s,&ms,&us);
			ticks = mtos_ticks_get();
			OS_PRINTF("[mac_time] s = %x:%x:%x ticks = %x\n",s,ms,us,ticks);

			sprintf(mac,"%c%c%c%c%c%c%02lx%02lx%02lx",
				status->mac_set.mac_one[0],status->mac_set.mac_one[1],
				status->mac_set.mac_two[0],status->mac_set.mac_two[1],
				status->mac_set.mac_three[0],status->mac_set.mac_three[1],
				((s+ms)%0xff),(us%0xff),(ticks%0xff));
			OS_PRINTF("%c %c %c %c %c %c %02lx %02lx %02lx \n",
				status->mac_set.mac_one[0],status->mac_set.mac_one[1],
				status->mac_set.mac_two[0],status->mac_set.mac_two[1],
				status->mac_set.mac_three[0],status->mac_set.mac_three[1],
				((s+ms)%0xff),(us%0xff),(ticks%0xff));


		}
		if_have_read = 1;
	}
	return (char*)mac;
#else
	sys_status_t *status;
  	char name[3] = {0};

	status = sys_status_get();
	if (if_have_read == 0)
	{
		memcpy(name,status->mac_set.mac_one,2);
		strcat(mac,name);
		memcpy(name,status->mac_set.mac_two,2);
		strcat(mac,name);
		memcpy(name,status->mac_set.mac_three,2);
		strcat(mac,name);
		memcpy(name,status->mac_set.mac_four,2);
		strcat(mac,name);
		memcpy(name,status->mac_set.mac_five,2);
		strcat(mac,name);
		memcpy(name,status->mac_set.mac_six,2);
		strcat(mac,name);
		if_have_read = 1;
		OS_PRINTF("mac 2[%s]\n",(char*)mac);
	}
	return (char*)mac;
#endif
	
}
char  sys_status_get_mac_by_index(u8 index)
{
#ifdef GET_MAC_FROM_FLASH_OTP
{
	static u8 mac[STB_MAC_ADDR_LEN+2];
	static int if_have_read;
  	char name[3] = "0";
	char *pstr;
	char hwaddr;
	if (if_have_read == 0)
	{
		dm_read(class_get_handle_by_id(DM_CLASS_ID),
							IDENTITY_BLOCK_ID, 0, STB_MAC_ADDR_START,
							STB_MAC_ADDR_LEN,
							(u8 *)mac); 
		OS_PRINTF("mac[%s]\n",(char*)mac);
		if ((u8)mac[0] == 0xff)
		{
			u32 s,ms,us;
			u32 ticks = 0;
			sys_status_t *status;

			status = sys_status_get();
			mtos_systime_get(&s,&ms,&us);
			ticks = mtos_ticks_get();
			OS_PRINTF("[mac_time] s = %x:%x:%x ticks = %x\n",s,ms,us,ticks);

			sprintf(mac,"%c%c%c%c%c%c%02lx%02lx%02lx",
				status->mac_set.mac_one[0],status->mac_set.mac_one[1],
				status->mac_set.mac_two[0],status->mac_set.mac_two[1],
				status->mac_set.mac_three[0],status->mac_set.mac_three[1],
				((s+ms)%0xff),(us%0xff),(ticks%0xff));
			OS_PRINTF("%c %c %c %c %c %c %02lx %02lx %02lx \n",
				status->mac_set.mac_one[0],status->mac_set.mac_one[1],
				status->mac_set.mac_two[0],status->mac_set.mac_two[1],
				status->mac_set.mac_three[0],status->mac_set.mac_three[1],
				((s+ms)%0xff),(us%0xff),(ticks%0xff));
		}
		
		if_have_read = 1;
	}
   	memcpy(name, &mac[index*2], 2);
	name[2] = 0;
	OS_PRINTF("name[%s]\n",(char*)name);
   	hwaddr = (char)strtol(name,&pstr,16);
	return hwaddr;
}

#else
  sys_status_t *status;
  char hwaddr = '\0',*pstr;
  char name[3] = "0";

  status = sys_status_get();
 switch(index)
 {
  case 0:
    memcpy(name, status->mac_set.mac_one, 2);
    name[2] = '\0';
    hwaddr = (char)strtol(name,&pstr,16);
    break;
  case 1:
    memcpy(name, status->mac_set.mac_two, 2);
    name[2] = '\0';
   hwaddr = (char)strtol(name,&pstr,16);
    break;
  case 2:
    memcpy(name, status->mac_set.mac_three, 2);
    name[2] = '\0';
   hwaddr = (char)strtol(name,&pstr,16);
    break;
  case 3:
    memcpy(name, status->mac_set.mac_four, 2);
    name[2] = '\0';
   hwaddr = (char)strtol(name,&pstr,16);
    break;
 case 4:
    memcpy(name, status->mac_set.mac_five, 2);
    name[2] = '\0';
    hwaddr = (char)strtol(name,&pstr,16);
    break;
 case 5:
    memcpy(name,  status->mac_set.mac_six, 2);
    name[2] = '\0';
    hwaddr = (char)strtol(name,&pstr,16);
    break;
 default:
    break;
 }
  return hwaddr;
#endif
}


BOOL sys_get_block_data_by_index(u8 block_id, u8 *buf, u32 buf_len,u32 *recvLen)
{
  u32 block_length;
  
  MT_ASSERT(buf != NULL);
  block_length = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID),block_id);
  if(block_length == 0)
    return FALSE;

  if (block_length > buf_len)
    block_length = buf_len;
  
  *recvLen = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                            block_id, 0, 0,
                            block_length,
                            (u8 *)buf);
  if(*recvLen == 0)
    return FALSE;

  return TRUE;
}



BOOL sys_get_serial_num(u8 *p_sn, u8 length)
{
  u32 read_len = 0;
  u32 SN_LEN = 64;
  BOOL bRet = FALSE;
  u8 distance = 0;
  u8 cfg_tmp[64] = {0};
  u8 id_tmp[64] = {0};
  u8 cnk = 0;
  u32 id_len = 0;
  
  distance = 0; /***note  it is distance from 0x7fc00 for tcon  tools***/
  memset(cfg_tmp,0,sizeof(cfg_tmp));
  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                            IDENTITY_BLOCK_ID, 0, 0,
                            SN_LEN,
                            (u8 *)cfg_tmp);
  if(read_len == 0)
  {
    bRet = FALSE;
    return bRet;
  }

  for(cnk = 0;cnk < SN_LEN;cnk ++)
  {
    if(cfg_tmp[distance + cnk] == 0xff)
    {
      cfg_tmp[distance + cnk] = '\0';
      break;
    }
  }
 
  memset(id_tmp,0,sizeof(id_tmp));
  memcpy(id_tmp,&cfg_tmp[distance],SN_LEN - distance);

  id_len = strlen((char *)id_tmp);
  if(id_len > length)
  {
    id_len = length;
  }
  strncpy((char *)p_sn,(char *)id_tmp,id_len);
  
  bRet = TRUE;

  return bRet;
}
#ifdef SMSX_CA
void get_sys_stbid(u8 idsize,u8 *idbuf,u32 size)
{
    u32 read_len = 0;
    u32 id_len = 0;
    u8 cfg_tmp[64] = {0};
    u8 id_tmp[64] = {0};
    u8 cnk = 0;
    u8 distance = 0;

    distance = 12; /***note  it is distance from 0x7fc00 for tcon  tools***/
    memset(cfg_tmp,0,sizeof(cfg_tmp));
    read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
              IDENTITY_BLOCK_ID, 0, 0,
              64,
              (u8 *)cfg_tmp);
    if(read_len == 0)
    {
      strcpy((char *)&cfg_tmp[distance],(char *)"100001");
    }

    for(cnk = 0;cnk < idsize;cnk ++)
    {
      if((cfg_tmp[distance + cnk] < 0x30) ||(cfg_tmp[distance + cnk] > 0x39))
        {
          memset(cfg_tmp,0,sizeof(cfg_tmp));
          strcpy((char *)&cfg_tmp[distance],(char *)"100001");
          break;
        }
    }
    memset(id_tmp,0,sizeof(id_tmp));
    memcpy(id_tmp,&cfg_tmp[distance],idsize);

    id_len = strlen((char *)id_tmp);
    if(id_len > size)
      {
        id_len = size;
      }
    strncpy((char *)idbuf,(char *)id_tmp,id_len);
}
#endif

#ifdef TF_CA
utc_time_t sys_status_get_card_upg_time(utc_time_t *p_time)
{
  sys_status_t *status;
 
  status = sys_status_get();
  memcpy(p_time, &(status->ca_upgrade_info.card_upg_time), sizeof(utc_time_t));
  return status->ca_upgrade_info.card_upg_time;
}

void sys_status_set_card_upg_time(utc_time_t card_upg_time)
{
  sys_status_t *status;

  status = sys_status_get();
  memcpy(&(status->ca_upgrade_info.card_upg_time), &card_upg_time, sizeof(utc_time_t));
  sys_status_save();
}

u32 sys_status_get_card_upg_state(void)
{
  sys_status_t *status;
  
  status = sys_status_get();
  return status->ca_upgrade_info.card_upg_state;
}

void sys_status_set_card_upg_state(u32 card_upg_state)
{
  sys_status_t *status;
 
  status = sys_status_get();
  status->ca_upgrade_info.card_upg_state = card_upg_state;
  sys_status_save();
}
#endif

#ifdef  CHECK_CHIP_ID
extern int sprintf(char *s, const char *format, ...);
BOOL sys_status_get_chip_id_string(u8 *p_sn, u8 length)
{
	u8 id[STB_CHIPID_ADDR_LEN+2];
	dm_read(class_get_handle_by_id(DM_CLASS_ID),
						IDENTITY_BLOCK_ID, 0, STB_CHIPID_ADDR_START,
						STB_CHIPID_ADDR_LEN,
						(u8 *)id); 
	if (id[0] == 0xff)
		return FALSE;
	memcpy(p_sn,id,length);
	return TRUE;
}
char *XorEncrypt(char *inputMsg, int inputStrlen)
{  
  const char xor_key[] = {221,222,223,224,226,226};
  int i = 0;
  if (inputMsg == NULL || inputStrlen <= 0) 
  {
    return NULL;
  }

  for( i = 0; i < inputStrlen; i++)
  {  
    inputMsg[i] = inputMsg[i] ^ xor_key[i%sizeof(xor_key)];
  }

  return inputMsg;
}

BOOL  xn_checkout_chip_id(void)
{
	unsigned char id_string[32];
	unsigned char id_string2[32];
	
	unsigned long h,l;
	memset(id_string,0,sizeof(id_string));
	memset(id_string2,0,sizeof(id_string2));	
	hal_get_chip_unique_numb(&h, &l);
	sprintf(id_string,"%08x%08x",(unsigned int)h,(unsigned int)l);
	if(sys_status_get_chip_id_string(id_string2, 16))
	{
		XorEncrypt(id_string2, 16);
		if (strcmp(id_string,id_string2) == 0)
			return TRUE;
	}
	return FALSE;
}

#endif



