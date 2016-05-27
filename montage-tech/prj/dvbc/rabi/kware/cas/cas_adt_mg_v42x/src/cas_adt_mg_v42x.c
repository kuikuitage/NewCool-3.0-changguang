/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
//lint -e64 -e438 -e502 -e527 -e539 -e545 -e550 -e570 -e668 -e701 -e713 -e734 -e746 -e831
#include <string.h>
#include "stdio.h"

#include "sys_types.h"
#include "sys_define.h"
#include "lib_bitops.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "drv_dev.h"
#include "drv_svc.h"
#include "dmx.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "cas_ware.h"
#include "cas_adapter.h"

#include "mtos_misc.h"

#include "mgdef_v42x.h"

//#define JAZZ_MG
#define CAS_ADT_DEBUG
#ifdef CAS_ADT_DEBUG
#define CAS_ADT_PRINTF OS_PRINTF
#else
#define CAS_ADT_PRINTF(...) do{} while(0)
#endif
#define INVALID_CHANNEL_ID 0xffff

#define ADT_MG_CAS_MAIL_SUBJECT_MAX_LEN    21
#define ADT_MG_CAS_MAIL_FROM_MAX_LEN    21
#define ADT_MG_CAS_MAIL_DATE_MAX_LEN    6
#define ADT_MG_CAS_MAIL_HEADER_MAX_LEN    50
#define ADT_MG_CAS_MAIL_BODY_MAX_LEN    512
#define OSD_DISP_MODE_UPLOOP    0x21
#define OSD_DISP_MODE_DNLOOP    0x22

#define CAS_EMAIL_STORE_MAX_NUM    8

#define FLASH_OFFSET_STB_SERIAL 1024				/* reserve 1k for ADT_MG nvram use */
#define FLASH_OFFSET_MAIL	(1034)                                    /* STB Serial: 10 Byte */
#define STB_SERIAL_LEN    10
#define CHANNEL_TYPE_EMM (0)
#define CHANNEL_TYPE_ECM (1)

#define ECM_TIMEOUT 		(10000)
#define EMM_0X84_TIMEOUT 	(100000)
#define EMM_0X85_TIMEOUT 	(100000)
#define EMM_0X87_TIMEOUT 	(100000)

static os_sem_t  g_sem_mg_emmlock = {0};
static os_sem_t  g_sem_mg_ecmlock = {0};
#if 1
#define MG_CAS_LOCK(x)      mtos_sem_take(&x, 0)
#define MG_CAS_UNLOCK(x)    mtos_sem_give(&x)
#else
#define MG_CAS_LOCK(x)      mtos_task_lock()
#define MG_CAS_UNLOCK(x)    mtos_task_unlock()
#endif

/*!
  the max OSD/message data length
  */
#define CAS_OSD_MSG_DATA_MAX_LEN    (128)


/*!
  Declaration for CAS module's operation table
  */
static cas_adapter_priv_t *g_cas_priv;

typedef enum
{
  ADT_MG_EVT_CARD_INSERT = 0x01,
  ADT_MG_EVT_CARD_REMOVE = 0x02,

  ADT_MG_EVT_MASK = 0x03,
}cas_adt_mg_evt_t;

typedef struct
{
  /*!
    max support mail num
    */
  u8 max_mail_num;
  /*!
    mail policy see cas_data_overlay_policy_t
    */
  u8 mail_policy;
}cas_adt_mg_maii_cfg_t;

typedef struct
{
  scard_device_t *p_smc_dev;
  dmx_device_t *p_dmx_dev;
  os_sem_t mg_port_sem;
  u8 stb_serial[10];
  u8 card_status;
  u8 card_ready_flg;
  u8 slot;
  /*!
     0x01 indicates ECM filter continuous mode disabled, 0x00, indicates enabled.
     0x02 indicates EMM filter continuous mode disabled, 0x00, indicates enabled.
    */
  u8 filter_mode;
  drvsvc_handle_t *p_EMM_Pro_svc;
  drvsvc_handle_t *p_ECM_Pro_svc;

  u16 cur_ecm_pid;
  u16 cur_emm_pid;
  u16 cur_ecm_dmx_chan;
  u16 cur_v_pid;
  u16 cur_a_pid;
  u16 pre_v_pid;
  u16 pre_a_pid;
  u16 zone_code;
  cas_adt_mg_maii_cfg_t mail_cfg;
  CAS_LIB_TYPE cas_lib_type;
  u8 zone_code_flag;
  u8 internal_reset_flag;

  u32 emm_0x84_systime;
  u32 emm_0x85_systime;
  u32 emm_0x87_systime;
  u32 ecm_systime;
}cas_adt_mg_priv_t;

/*!
  The ECM information should processed by CAS module
  */
typedef struct
{
/*!
    The zone code should processed by ECM process
    */
  u16 zone_code;
/*!
    The video dmx channel id need to be descrambled
    */
  u16 prog_v_cid;
/*!
    The audio dmx channel id need to be descrambled
    */
  u16 prog_a_cid;
}ecm_info_t;

/*!
  The store mail header format
  */
typedef struct
{
  	u8 subject[CAS_MAIL_SUBJECT_MAX_LEN];
  	u8 sender[CAS_MAIL_FROM_MAX_LEN];
	u8 creat_date[CAS_MAIL_DATE_MAX_LEN];
        u16 body_len;
	u8 mail_id;
}cas_mail_header_store_t;

/*!
The operators info struct
*/
typedef struct
{
  	cas_mail_header_store_t  p_mail_head[CAS_EMAIL_STORE_MAX_NUM];
  	u16 max_num;
  	u16 reserve;
	u8 has_read;
}cas_mail_headers_store_t;

/**
  The msg need to store in nvram
*/
typedef struct
{
	cas_mail_body_t cas_mail_body[CAS_EMAIL_STORE_MAX_NUM];		        /* all mail bodys */
  	u16 reserve;
}cas_mail_bodys_store_t;

/**
  The mail format
*/
typedef struct
{
	cas_mail_headers_store_t cas_mail_headers;		/* mail headers */
	cas_mail_bodys_store_t p_cas_mail_bodys;	        /* mail bodys    */
}cas_mail_t;

/**
  Delete mail type
*/
typedef enum
{
	BY_INDEX,
	ALL_MAIL
}cas_del_mail_type_e;

/**
  Store mail/msg to mem result
*/
typedef enum
{
	RET_SUCCESS,
        RET_FLUSH_FAILED,
	RET_HAS_RECEIVED
}cas_save_mail_ret_e;

/**
  Filter Struct
*/
typedef struct
{
        u8 Data[DMX_SECTION_FILTER_SIZE];
        u8 Mask[DMX_SECTION_FILTER_SIZE];
}filter_struct_t;

/**
  The OSD message format
*/
typedef struct
{
	u8 disp_mode;		                                                /* disp mode:0x21 up loop, 0x22 down loop  */
	u8 disp_cnt;     		                                                /* disp repeat count  */
	u8 reserved;
	u8 cas_osd_data[CAS_OSD_MSG_DATA_MAX_LEN];		/* OSD message data */
	BOOL has_read;
}cas_osd_t;

/**
  The message format
*/
typedef struct
{
	cas_osd_t cas_osd_up;		                                    /* OSD struct Up LOOP      */
	cas_osd_t cas_osd_dn;		                                    /* OSD struct Down LOOP */
	u8 cas_msg_data[CAS_OSD_MSG_DATA_MAX_LEN];  /* message data */
}cas_osd_msg_t;

scard_device_t *p_g_smc_dev = NULL;
os_sem_t *p_g_mg_port_sem = NULL;
cas_mail_t cas_mail;
cas_osd_msg_t cas_osd_msg;

cas_dmx_chan_t  cas_si_emm_chan[CAS_DMX_CHANNEL_COUNT];
cas_dmx_chan_t  cas_si_ecm_chan[CAS_DMX_CHANNEL_COUNT];
static u8 g_ecm_tempbuff[4096] = {0,};
static u8 g_emm_tempbuff[4096] = {0,};
/* porting layer functions */

RET_CODE cas_adt_mg_emm_process(u8 *p_emm_buf, u32 *p_result);
RET_CODE cas_adt_mg_ecm_process(u8 *p_ecm_buf, ecm_info_t *p_info, u32 *p_result);
static cas_save_mail_ret_e cas_adt_mg_save_new_mail(cas_mail_header_store_t *p_cas_mail_header, cas_mail_body_t *p_cas_mail_body);
static RET_CODE cas_adt_mg_resetFilterAll(void);
static RET_CODE cas_adt_mg_ecm_setFilterAll(void);
static RET_CODE cas_adt_mg_ecm_resetFilterAll(void);
static RET_CODE cas_adt_mg_emm_setFilter(u8 emm_Tid);
static RET_CODE cas_adt_mg_emm_resetFilter(u8 emm_Tid);
static int cas_adt_mg_internal_reset_card(void);

BOOL get_vol_entitlement_from_ca(void)
{
	if(MG_Get_Card_Ready() == MG_TRUE)
	{
		if(MG_Get_RecentExpireDay() > 0)
		{
			OS_PRINTF("expireday = %x \n",MG_Get_RecentExpireDay());
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	
	return FALSE;
}

void send_event_to_ui_from_authorization(u32 event)
{
	cas_adt_mg_priv_t *p_priv = g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;
	if(event == 0)
	{
		OS_PRINTF("send enent fail!\n");
		return;
	}
	cas_send_event(p_priv->slot, CAS_ID_ADT_MG, event, 0);
	OS_PRINTF("send enent from authorization!\n");
}

static cas_dmx_chan_t *cas_adt_mg_search_free_channel(u8 type)
{
  u32 i = 0;

  for (i = 0; i < CAS_DMX_CHANNEL_COUNT; i ++)
  {
    if (type == CHANNEL_TYPE_EMM && cas_si_emm_chan[i].handle == DMX_INVALID_CHANNEL_ID)
    {
      cas_si_emm_chan[i].chan_index = i;
      return cas_si_emm_chan + i;
    }
    if(type == CHANNEL_TYPE_ECM && cas_si_ecm_chan[i].handle == DMX_INVALID_CHANNEL_ID)
    {
      cas_si_ecm_chan[i].chan_index = i;
      return cas_si_ecm_chan + i;
    }
  }
  OS_PRINTF("The type(%d) of channel request failed.\n", type);
  return NULL;
}

static cas_dmx_chan_t *cas_adt_mg_ecm_request(void *p_dmx, u16 pid, u8 *value, u8 *mask, u8 filter_mode)
{
  cas_dmx_chan_t *p_chan = NULL;
  RET_CODE ret = ERR_FAILURE;
  u8 mode = 0;

  p_chan = cas_adt_mg_search_free_channel(CHANNEL_TYPE_ECM);
  p_chan->tid = value[0];
  if(NULL == p_chan)
  {
      OS_PRINTF("--->_search_free_channel: NO FREE Chan\n");
  }
  //if(filter_mode & ECM_FILTER_CONTINUOUS_DISABLED)
      mode |= 1 << FILTER_CONTINOUS_DISABLED;
  //else
  //    mode |= 1 << FILTER_CONTINOUS_ENABLED;
  ret = cas_table_request(p_dmx, p_chan, pid, value, mask, mode);
  MT_ASSERT(ret == SUCCESS);

  return p_chan;
}

static cas_dmx_chan_t *cas_adt_mg_emm_request(void *p_dmx, u16 pid, u8 *value, u8 *mask, u8 filter_mode)
{
  cas_dmx_chan_t *p_chan = NULL;
  RET_CODE ret = ERR_FAILURE;
  u8 mode = 0;

  p_chan = cas_adt_mg_search_free_channel(CHANNEL_TYPE_EMM);
  p_chan->tid = value[0];
  //OS_PRINTF("## %s %d p_chan:%d pid:%d value:0x%x mask:0x%x\n", __FUNCTION__, __LINE__, p_chan, pid, *value, *mask);
  if(filter_mode & EMM_FILTER_CONTINUOUS_DISABLED)
      mode |= 1 << FILTER_CONTINOUS_DISABLED;
  else
      mode |= 1 << FILTER_CONTINOUS_ENABLED;
  ret = cas_table_request(p_dmx, p_chan, pid, value, mask, mode);
  MT_ASSERT(ret == SUCCESS);

  return p_chan;
}

static RET_CODE _adt_mg_EMM_Process_task(void *p_param, u32 event_status)
{
  cas_adt_mg_priv_t *p_priv = (cas_adt_mg_priv_t *)p_param;
  u8 i = 0;
  u8 *p_data = NULL;
  u32  size = 0;
  RET_CODE ret = ERR_FAILURE;
  u32 result = 0;
  //u32 system_time_tick = 0x00;

  if(0 == event_status)
  {
    if(MG_TRUE == MG_Get_Card_Ready())
    {
       if(MG_TRUE != p_priv->card_ready_flg)
       {
          cas_adt_mg_emm_resetFilter(0xFF);
          cas_adt_mg_emm_setFilter(0xFF); 
          p_priv->card_ready_flg = MG_TRUE;
          //system_time_tick = mtos_ticks_get();
          //CAS_ADT_PRINTF(" MG_Get_Card_Ready==TRUE!!! system_time: 0x%8x\n", system_time_tick);
       }
    }
    else
    {
         //OS_PRINTF("MG_Get_Card_Ready==False!!!\n");
         return SUCCESS;
    }

    for(i = 0; i < CAS_DMX_CHANNEL_COUNT; i++)
    {
      MG_CAS_LOCK(g_sem_mg_emmlock);
      if(cas_si_emm_chan[i].handle != DMX_INVALID_CHANNEL_ID)
      {
        if(cas_si_emm_chan[i].pid == p_priv->cur_emm_pid)
        {
          memset(g_emm_tempbuff, 0, 4096);
          ret = dmx_si_chan_get_data(p_priv->p_dmx_dev, cas_si_emm_chan[i].handle, &p_data, &size);
          memcpy(g_emm_tempbuff, p_data, size);
          
          //CAS_ADT_PRINTF("cas_si_emm_chan[ %d ].pid: 0x%2x,tid:0x%x\n", i, cas_si_emm_chan[i].pid,cas_si_emm_chan[i].tid);

          if(ret != ERR_NO_RSRC)
          {
            //CAS_ADT_PRINTF("cas_si_emm_chan[ %d ].pid: 0x%2x,tid:0x%x\n", i, cas_si_emm_chan[i].pid,cas_si_emm_chan[i].tid);
            //CAS_ADT_PRINTF("--->_adt_mg_task: EMM PROCESS\n");
       //     system_time_tick = mtos_ticks_get();
            MG_CAS_UNLOCK(g_sem_mg_emmlock);
            cas_adt_mg_emm_process(g_emm_tempbuff, &result);
            MG_CAS_LOCK(g_sem_mg_emmlock);
       //     system_time_tick = mtos_ticks_get() -system_time_tick;
       //     CAS_ADT_PRINTF("MG_EMM_PROCESS_TIME=%d !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",system_time_tick);
          }
        }
      }
      MG_CAS_UNLOCK(g_sem_mg_emmlock);
    }

	if((mtos_ticks_get() - p_priv->emm_0x84_systime) > EMM_0X84_TIMEOUT)
	{
      OS_PRINTF("ENTER EMM_0X84_TIMEOUT %d\n",p_priv->emm_0x84_systime);
      cas_adt_mg_emm_resetFilter(0x84);
      cas_adt_mg_emm_setFilter(0x84);
	}
	if((mtos_ticks_get() - p_priv->emm_0x85_systime) > EMM_0X85_TIMEOUT)
	{
      OS_PRINTF("ENTER EMM_0X85_TIMEOUT %d\n",p_priv->emm_0x85_systime);
      cas_adt_mg_emm_resetFilter(0x85);
      cas_adt_mg_emm_setFilter(0x85);
	}
	if((mtos_ticks_get() - p_priv->emm_0x87_systime) > EMM_0X87_TIMEOUT)
	{
      OS_PRINTF("ENTER EMM_0X87_TIMEOUT %d\n",p_priv->emm_0x87_systime);
      cas_adt_mg_emm_resetFilter(0x87);
      cas_adt_mg_emm_setFilter(0x87);
	}
  }
  else
  {
    /* process event */
  }

  return SUCCESS;
}

static RET_CODE _adt_mg_ECM_Process_task(void *p_param, u32 event_status)
{
    cas_adt_mg_priv_t *p_priv = (cas_adt_mg_priv_t *)p_param;
    u8 i = 0;
    u8 *p_data = NULL;
    u32  size = 0;
    RET_CODE ret = ERR_FAILURE;
    ecm_info_t ecm_info = {0};
    u32 result = 0;
    //u32 system_time_tick = 0x00;

    u16 pre_v_cid = p_priv->pre_v_pid;
    u16 pre_a_cid = p_priv->pre_a_pid;
    
    if(p_priv->internal_reset_flag)
    {
        cas_adt_mg_internal_reset_card();
        p_priv->internal_reset_flag = 0;
        return SUCCESS;
    }
    ecm_info.prog_v_cid =INVALID_CHANNEL_ID;
    ecm_info.prog_a_cid = INVALID_CHANNEL_ID;
    ecm_info.zone_code = p_priv->zone_code;

    if(0x00 == p_priv->cur_ecm_pid)
    {
        return SUCCESS;
    }

    if(0 == event_status)
    {
        if(cas_si_ecm_chan[p_priv->cur_ecm_dmx_chan].pid == p_priv->cur_ecm_pid)
        {   
            memset(g_ecm_tempbuff, 0, 4096);
            MG_CAS_LOCK(g_sem_mg_ecmlock);
            if(cas_si_ecm_chan[p_priv->cur_ecm_dmx_chan].handle != INVALID_CHANNEL_ID)
              ret = dmx_si_chan_get_data(p_priv->p_dmx_dev, cas_si_ecm_chan[p_priv->cur_ecm_dmx_chan].handle, &p_data, &size);
            else
            {
              //MG_CAS_UNLOCK(g_sem_mg_ecmlock);
              CAS_ADT_PRINTF("ecm handle INVALID_CHANNEL_ID\n");
              ret = ERR_NO_RSRC;
            }
            memcpy(g_ecm_tempbuff, p_data, size);
            MG_CAS_UNLOCK(g_sem_mg_ecmlock);
            //CAS_ADT_PRINTF("cas_si_ecm_chan[ %d ].pid: 0x%2x\n", i, cas_si_ecm_chan[i].pid);

            if(ret != ERR_NO_RSRC)
            {
                p_priv->ecm_systime = mtos_ticks_get();
                CAS_ADT_PRINTF("cur_ecm_dmx_chan: %d pre_v_cid: 0x%2x  pre_a_cid: 0x%2x p_priv->cur_v_pid: 0x%2x p_priv->cur_a_pid: 0x%2x\n", p_priv->cur_ecm_dmx_chan, pre_v_cid, pre_a_cid, p_priv->cur_v_pid, p_priv->cur_a_pid);
                //system_time_tick = mtos_ticks_get();
                //CAS_ADT_PRINTF(" got ECM system_time: 0x%8x\n", system_time_tick);

                if(pre_v_cid != INVALID_CHANNEL_ID)
                {
                    ret = dmx_descrambler_onoff(p_priv->p_dmx_dev, pre_v_cid, FALSE);
                    if(ret != SUCCESS)
                    {
                        CAS_ADT_PRINTF("Close descrambler failed(err:%d, pre_v_cid:%d)\n", ret, pre_v_cid);
                        //return ERR_FAILURE;
                    }
                    pre_v_cid = INVALID_CHANNEL_ID;
                }
                if(pre_a_cid != INVALID_CHANNEL_ID)
                {
                    ret = dmx_descrambler_onoff(p_priv->p_dmx_dev, pre_a_cid, FALSE);
                    if(ret != SUCCESS)
                    {
                        CAS_ADT_PRINTF("Close descrambler failed(err:%d, pre_a_cid:%d)\n", ret, pre_a_cid);
                        // return ERR_FAILURE;
                    }
                    pre_a_cid = INVALID_CHANNEL_ID;
                }

                dmx_get_chanid_bypid(p_priv->p_dmx_dev, p_priv->cur_v_pid, &ecm_info.prog_v_cid);
                if(ecm_info.prog_v_cid != INVALID_CHANNEL_ID)
                {
                    ret = dmx_descrambler_onoff(p_priv->p_dmx_dev, ecm_info.prog_v_cid, TRUE);
                    if(ret != SUCCESS)
                    {
                        CAS_ADT_PRINTF("Open descrambler failed(err:%d, prog_v_cid:%d)\n", ret, ecm_info.prog_v_cid);
                        //return ERR_FAILURE;
                    }
                    p_priv->pre_v_pid = ecm_info.prog_v_cid;
                    //p_priv->pre_v_pid = p_priv->cur_v_pid;
                }

                dmx_get_chanid_bypid(p_priv->p_dmx_dev, p_priv->cur_a_pid, &ecm_info.prog_a_cid);
                if(ecm_info.prog_a_cid != INVALID_CHANNEL_ID)
                {
                    ret = dmx_descrambler_onoff(p_priv->p_dmx_dev, ecm_info.prog_a_cid, TRUE);
                    if(ret != SUCCESS)
                    {
                        CAS_ADT_PRINTF("Open descrambler failed(err:%d, prog_a_cid:%d)\n", ret, ecm_info.prog_a_cid);
                        //return ERR_FAILURE;
                    }
                    p_priv->pre_a_pid = ecm_info.prog_a_cid;
                    // p_priv->pre_a_pid = p_priv->cur_a_pid;
                }
                if(ecm_info.prog_a_cid != INVALID_CHANNEL_ID && ecm_info.prog_v_cid != INVALID_CHANNEL_ID)
                {
                    //system_time_tick = mtos_ticks_get();
                    //CAS_ADT_PRINTF(" ECM_P_Start system_time: 0x%8x\n", system_time_tick);
                    cas_adt_mg_ecm_process(g_ecm_tempbuff, &ecm_info, &result);
                    cas_adt_mg_ecm_resetFilterAll();
                    cas_adt_mg_ecm_setFilterAll();
                    //system_time_tick = mtos_ticks_get();
                    //CAS_ADT_PRINTF(" ECM_P_End system_time: 0x%8x\n", system_time_tick);
                }
            }
        }
        else
        {
            for(i = 0; i < CAS_DMX_CHANNEL_COUNT; i++)
            {
                if(cas_si_ecm_chan[i].handle != DMX_INVALID_CHANNEL_ID)
                {
                    if(cas_si_ecm_chan[i].pid == p_priv->cur_ecm_pid)
                    {
                        memset(g_ecm_tempbuff, 0, 4096);
                        MG_CAS_LOCK(g_sem_mg_ecmlock);
                        OS_PRINTF("cas_si_ecm_chan[p_priv->cur_ecm_dmx_chan].handle : %x\n",cas_si_ecm_chan[p_priv->cur_ecm_dmx_chan].handle);
                        if(cas_si_ecm_chan[i].handle != DMX_INVALID_CHANNEL_ID)
                        {
                          ret = dmx_si_chan_get_data(p_priv->p_dmx_dev, cas_si_ecm_chan[p_priv->cur_ecm_dmx_chan].handle, &p_data, &size);
                          memcpy(g_ecm_tempbuff, p_data, size);
                        }
                        else
                          ret = ERR_NO_RSRC;
                        MG_CAS_UNLOCK(g_sem_mg_ecmlock);
                        //CAS_ADT_PRINTF("cas_si_ecm_chan[ %d ].pid: 0x%2x\n", i, cas_si_ecm_chan[i].pid);

                        if(ret != ERR_NO_RSRC)
                        {
                            p_priv->ecm_systime = mtos_ticks_get();
                            CAS_ADT_PRINTF("pre_v_cid: 0x%2x  pre_a_cid: 0x%2x p_priv->cur_v_pid: 0x%2x p_priv->cur_a_pid: 0x%2x\n", pre_v_cid, pre_a_cid, p_priv->cur_v_pid, p_priv->cur_a_pid);
                            //system_time_tick = mtos_ticks_get();
                            //CAS_ADT_PRINTF(" got ECM system_time: 0x%8x\n", system_time_tick);

                            if(pre_v_cid != INVALID_CHANNEL_ID)
                            {
                                ret = dmx_descrambler_onoff(p_priv->p_dmx_dev, pre_v_cid, FALSE);
                                if(ret != SUCCESS)
                                {
                                    CAS_ADT_PRINTF("Close descrambler failed(err:%d, pre_v_cid:%d)\n", ret, pre_v_cid);
                                    //return ERR_FAILURE;
                                }
                                pre_v_cid = INVALID_CHANNEL_ID;
                            }
                            if(pre_a_cid != INVALID_CHANNEL_ID)
                            {
                                ret = dmx_descrambler_onoff(p_priv->p_dmx_dev, pre_a_cid, FALSE);
                                if(ret != SUCCESS)
                                {
                                    CAS_ADT_PRINTF("Close descrambler failed(err:%d, pre_a_cid:%d)\n", ret, pre_a_cid);
                                    // return ERR_FAILURE;
                                }
                                pre_a_cid = INVALID_CHANNEL_ID;
                            }

                            dmx_get_chanid_bypid(p_priv->p_dmx_dev, p_priv->cur_v_pid, &ecm_info.prog_v_cid);
                            if(ecm_info.prog_v_cid != INVALID_CHANNEL_ID)
                            {
                                ret = dmx_descrambler_onoff(p_priv->p_dmx_dev, ecm_info.prog_v_cid, TRUE);
                                if(ret != SUCCESS)
                                {
                                    CAS_ADT_PRINTF("Open descrambler failed(err:%d, prog_v_cid:%d)\n", ret, ecm_info.prog_v_cid);
                                    //return ERR_FAILURE;
                                }
                                p_priv->pre_v_pid = ecm_info.prog_v_cid;
                                //p_priv->pre_v_pid = p_priv->cur_v_pid;
                            }

                            dmx_get_chanid_bypid(p_priv->p_dmx_dev, p_priv->cur_a_pid, &ecm_info.prog_a_cid);
                            if(ecm_info.prog_a_cid != INVALID_CHANNEL_ID)
                            {
                                ret = dmx_descrambler_onoff(p_priv->p_dmx_dev, ecm_info.prog_a_cid, TRUE);
                                if(ret != SUCCESS)
                                {
                                    CAS_ADT_PRINTF("Open descrambler failed(err:%d, prog_a_cid:%d)\n", ret, ecm_info.prog_a_cid);
                                    //return ERR_FAILURE;
                                }
                                p_priv->pre_a_pid = ecm_info.prog_a_cid;
                                // p_priv->pre_a_pid = p_priv->cur_a_pid;
                            }
                            //system_time_tick = mtos_ticks_get();
                            //CAS_ADT_PRINTF(" ECM_P_Start system_time: 0x%8x\n", system_time_tick);
                            //cas_adt_mg_ecm_process(g_ecm_tempbuff, &ecm_info, &result);
                            cas_adt_mg_ecm_resetFilterAll();
                            cas_adt_mg_ecm_setFilterAll();
                            //system_time_tick = mtos_ticks_get();
                            //CAS_ADT_PRINTF(" ECM_P_End system_time: 0x%8x\n", system_time_tick);
                        }
                    }
                }
            }
        }
		if((mtos_ticks_get() - p_priv->ecm_systime) > ECM_TIMEOUT)
		{
		   OS_PRINTF("ENTER %s ,ECM_TIMEOUT  time : %d\n",__FUNCTION__,p_priv->ecm_systime);
          cas_adt_mg_ecm_resetFilterAll();
          cas_adt_mg_ecm_setFilterAll();
		}
    }
    else
    {
        /* process event */
    }

    return SUCCESS;
}
static u32 cas_adt_mg_search_free_email_node(u8 *p_index)
{
    u8 emai_node_sts = 0x00;
    u8 i = 0;

    if(cas_mail.cas_mail_headers.max_num >= CAS_EMAIL_STORE_MAX_NUM)
    {
        CAS_ADT_PRINTF("cas_mail.cas_mail_headers.max_num >= CAS_EMAIL_STORE_MAX_NUM\n");
        return ERR_FAILURE;
    }
    else
    {
        emai_node_sts = cas_mail.cas_mail_headers.has_read;

        for(; i < CAS_EMAIL_STORE_MAX_NUM; i++)
        {
            if(emai_node_sts & (0x01 << i))
            {
                *p_index = i;
                break;
            }
        }

        if(i >= CAS_EMAIL_STORE_MAX_NUM)
        {
            return ERR_FAILURE;
        }
        return SUCCESS;
    }
}

static u32 cas_adt_mg_search_unread_email_node(u8 *p_index)
{
    u8 emai_node_sts = 0xFF;
    u8 i = 0;

    emai_node_sts = cas_mail.cas_mail_headers.has_read;

    for(; i < CAS_EMAIL_STORE_MAX_NUM; i++)
    {
        if(!(emai_node_sts & (0x01 << i)))
        {
        	*p_index = i;
            break;
        }
    }

    if(i >= CAS_EMAIL_STORE_MAX_NUM)
    {
        return ERR_FAILURE;;
    }
    else
    {
        return SUCCESS;
    }
}

static u32 cas_adt_mg_format_email(u8 *p_emailheader, u8 *p_emailbody,cas_mail_header_store_t *p_emailheader_format, cas_mail_body_t *p_emailbody_format)
{
    MG_U8 *p_temp = NULL;
    u16 i = 0;
    u16 temp = 0;

    memset(p_emailheader_format, 0x00, sizeof(cas_mail_header_store_t));
    memset(p_emailbody_format, 0x00, sizeof(cas_mail_body_t));

    p_temp = p_emailheader;
    /* email header format */
    /* get mail subject, in format of uni-code */
    for(i = 0; i < 20; i++)
    {
        if(0 == *p_temp)
        {
            break;
        }
        p_emailheader_format->subject[i] = *p_temp;
        p_temp ++;
    }
    p_emailheader_format->subject[i] = 0; /* string tail */
    p_temp = p_emailheader + 21;

    /* get mail sender, in format of uni-code */
    for(i = 0; i < 20; i++)
    {
        if(0 == *p_temp)
        {
            break;
        }
        p_emailheader_format->sender[i] = *p_temp;
        p_temp ++;
    }
    p_emailheader_format->sender[i] = 0; /* string tail */
    p_temp = p_emailheader + 42;

    /* get mail date, in format of BCD code */
    memcpy(&p_emailheader_format->creat_date, p_temp, 6);
    p_temp = p_emailheader + 48;

    /* get mail boby length */
    temp = p_temp[0];
    p_emailheader_format->body_len = p_temp[1] | (temp << 8);

    /* email body format */
    memcpy(p_emailbody_format->data, p_emailbody, ADT_MG_CAS_MAIL_BODY_MAX_LEN);

    return SUCCESS;
}

static u32 parse_event(MG_STATUS result)
{
	u8 MG_STBSerials[STB_SERIAL_LEN];
	u8 *p_email_header = NULL;
	u8 *p_email_body = NULL;
	u8 emailheader[ADT_MG_CAS_MAIL_HEADER_MAX_LEN];
	u8 emailbody[ADT_MG_CAS_MAIL_BODY_MAX_LEN];
	cas_mail_header_store_t *p_email_header_format = NULL;
	cas_mail_body_t *p_email_body_format = NULL;
	cas_adt_mg_priv_t *p_priv = g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;
	u8 *p_msg_data = NULL;

	switch(result)
	{
/**************************************************************************/
/***                 MG_CAS_EMM_Process  return Value            ***/
/**************************************************************************/
		case MG_EMM_APP_STOP:
			/* show card app stop */
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_EMM_APP_STOP\n");
			return CAS_C_EMM_APP_STOP;

		case MG_EMM_APP_RESUME:
			/* show card app resume */
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_EMM_APP_RESUME\n");
			return CAS_C_EMM_APP_RESUME;

		case MG_EMM_PROG_UNAUTH:
			/* show there are progs unauthed */
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_EMM_PROG_UNAUTH\n");
			return CAS_C_EMM_PROG_UNAUTH;

		case MG_EMM_PROG_AUTH:
			/* show there are progs authed */
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_EMM_PROG_AUTH\n");
			return CAS_C_EMM_PROG_AUTH;

		case MG_EMM_EN_PARTNER:
		case MG_EMM_DIS_PARTNER:
		case MG_EMM_SONCARD:
			memset(MG_STBSerials,0x00,STB_SERIAL_LEN);

			/* get STB serial */
			MG_Get_STB_Serial(MG_STBSerials);
			memcpy(p_priv->stb_serial, MG_STBSerials, STB_SERIAL_LEN);
			cas_send_evt_to_adapter(p_priv->slot, CAS_ID_ADT_MG, CAS_EVT_SERIAL_MASK);

			if(MG_EMM_EN_PARTNER == result)
			{
				/* show set stb-card partnered */
				CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_EMM_EN_PARTNER\n");
				return CAS_C_EN_PARTNER;
			}
			else if(MG_EMM_DIS_PARTNER == result)
			{
				/* show set stb-card partnered */
				CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_EMM_DIS_PARTNER\n");
				return CAS_C_DIS_PARTNER;
			}
			else
			{
				/* show set stb-card partnered */
				CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_SON_CARD_NEED_FEED\n");
				return CAS_C_SET_SON_CARD;
			}

		case MG_EMM_UPDATE_GNO:
			/* show group id changed */
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_EMM_UPDATE_GNO\n");
			return CAS_C_EMM_UPDATE_GNO;

		case MG_EMM_CHILD_UNLOCK:
			/* */
			OS_PRINTF(" EMM_SENT_EVENT---> CAS_C_EMM_CHILD_UNLOCK\n");
			return CAS_EVT_NONE;

		case MG_EMM_MSG:
			/* get message */
			p_msg_data = MG_Get_Command_Data();
			if(NULL != p_msg_data)
			{
				memcpy(&cas_osd_msg.cas_msg_data, p_msg_data, CAS_OSD_MSG_DATA_MAX_LEN);
				CAS_ADT_PRINTF(" RCV_MG_EMM_MSG--->data: %s\n", p_msg_data);
			}
			else
			{
				/* no message */
				CAS_ADT_PRINTF(" RCV_MG_EMM_MSG---> MGLib return data is point to NULL  \n");
			}
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_SHOW_MSG\n");
			return CAS_C_SHOW_MSG;

		case MG_EMM_UPDATE_GK:
			/* show card's GK need update */
			/* show re-start STB */
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_S_CARD_NEED_UPG\n");
			return CAS_S_CARD_NEED_UPG;

		case MG_EMM_EMAIL:
			p_email_header = MG_Get_Mail_Head();
			p_email_body = MG_Get_Mail_Body();

			memset(emailheader, 0x00, ADT_MG_CAS_MAIL_HEADER_MAX_LEN);
			memset(emailbody, 0x00, ADT_MG_CAS_MAIL_BODY_MAX_LEN);

			memcpy(emailheader, p_email_header, ADT_MG_CAS_MAIL_HEADER_MAX_LEN);
			memcpy(emailbody, p_email_body, ADT_MG_CAS_MAIL_BODY_MAX_LEN);
			//CAS_ADT_PRINTF(" MG_Get_Mail_Head--->emailheader: %s \n ", emailheader);
			//CAS_ADT_PRINTF(" MG_Get_Mail_Body--->emailbody: %s \n ", emailbody);

			p_email_header_format = mtos_malloc(sizeof(cas_mail_header_store_t));
			p_email_body_format = mtos_malloc(sizeof(cas_mail_body_t));
			cas_adt_mg_format_email(emailheader, emailbody, p_email_header_format, p_email_body_format);
			CAS_ADT_PRINTF(" cas_adt_mg_format_email---> mail_id: 0x%2x subject: %s body_len: 0x%4x\n", p_email_header_format->mail_id,
			            p_email_header_format->subject, p_email_header_format->body_len);
			cas_adt_mg_save_new_mail(p_email_header_format,  p_email_body_format);
			CAS_ADT_PRINTF(" cas_adt_mg_save_new_mail--->max_num: %d subject:%s body_len: 0x%4x\n ", cas_mail.cas_mail_headers.max_num, cas_mail.cas_mail_headers.p_mail_head->subject,
			            cas_mail.cas_mail_headers.p_mail_head[0].body_len);
			#if 0
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_SHOW_NEW_EMAIL\n");
			return CAS_C_SHOW_NEW_EMAIL;
			#else /* deal with V3.1.2 adapter layer process flash R/W */
			CAS_ADT_PRINTF(" ENT_EVENT_TO_ADAPTER---> CAS_C_SHOW_NEW_EMAIL\n");
			cas_send_evt_to_adapter(p_priv->slot, CAS_ID_ADT_MG, CAS_EVT_MAIL_MASK);
			return SUCCESS;
			#endif

		case MG_EMM_CHARGE:
			/* PPV balance charge */
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_EMM_CHARGE\n");
			return CAS_C_EMM_CHARGE;

		case MG_EMM_LOADER:
			/* */
			OS_PRINTF(" EMM_SENT_EVENT---> CAS_EVT_NONE\n");
			return CAS_EVT_NONE;

		case MG_EMM_FIGEIDENT:
			/* show the card serial number */
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_SHOW_NEW_FINGER\n");
			return CAS_C_SHOW_NEW_FINGER;

		case MG_EMM_URGENT_SERVICE:
			/* change to the relevant channel */
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_EMM_URGENT_SERVICE\n");
			return CAS_C_FORCE_CHANNEL;

		case MG_EMM_MODIFY_ZONECODE:
			/* show the zonecode is updated */
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_S_MODIFY_ZONECODE\n");
			return CAS_S_MODIFY_ZONECODE;

		case MG_EMM_UPDATE_COUT:
			/* show card char-code is updated */
			OS_PRINTF(" EMM_SENT_EVENT---> CAS_C_EMM_UPDATE_COUT\n");
			return CAS_C_EMM_UPDATE_COUT;

		case MG_EMM_OSD:
			/* get message */
			p_msg_data = MG_Get_Command_Data();
			/* check disp mode */
			if(NULL != p_msg_data)
			{
				if(OSD_DISP_MODE_UPLOOP == p_msg_data[0])
				{
					cas_osd_msg.cas_osd_up.disp_mode = p_msg_data[0];
					cas_osd_msg.cas_osd_up.disp_cnt = p_msg_data[1];
					memcpy(&cas_osd_msg.cas_osd_up.cas_osd_data, ( p_msg_data+3), CAS_OSD_MSG_DATA_MAX_LEN);
					cas_osd_msg.cas_osd_up.has_read = FALSE;
					CAS_ADT_PRINTF(" RCV_MG_EMM_OSD---> disp_mode: %d disp_cnt: %d data: %s\n", p_msg_data[0], p_msg_data[1], p_msg_data+3);
				}
				else if(OSD_DISP_MODE_DNLOOP == p_msg_data[0])
				{
					cas_osd_msg.cas_osd_dn.disp_mode = p_msg_data[0];
					cas_osd_msg.cas_osd_dn.disp_cnt = p_msg_data[1];
					memcpy(&cas_osd_msg.cas_osd_dn.cas_osd_data, ( p_msg_data+3), CAS_OSD_MSG_DATA_MAX_LEN);
					cas_osd_msg.cas_osd_dn.has_read = FALSE;
					CAS_ADT_PRINTF(" RCV_MG_EMM_OSD---> disp_mode: %d disp_cnt: %d data: %s\n", p_msg_data[0], p_msg_data[1], p_msg_data+3);
				}
				else
				{
					CAS_ADT_PRINTF(" RCV_MG_EMM_OSD---> rcv data error: 0x%2x \n", p_msg_data[0]);
				}
			}
			else
			{
				/* no message */
				CAS_ADT_PRINTF(" RCV_MG_EMM_OSD---> MGLib return data is point to NULL  \n");
			}
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_SHOW_OSD\n");
			return CAS_C_SHOW_OSD;

		case MG_EMM_UPDATE_CRT:
			/* show card auth status is updated */
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_NOTIFY_UPDATE_CRT\n");
			return CAS_C_NOTIFY_UPDATE_CRT;
		case MG_EMM_NOP:
			/* EMM NOP */
			OS_PRINTF(" EMM_SENT_EVENT---> CAS_C_EMM_NOP\n");
			return CAS_EVT_NONE;
		case MG_EMM_MODIFY_CTRLCODE:
			/* smart card control code update. Added at v4.2.0 */
			CAS_ADT_PRINTF(" EMM_SENT_EVENT---> CAS_C_NOTIFY_UPDATE_CTRLCODE\n");
			return CAS_C_NOTIFY_UPDATE_CTRLCODE;
		case MG_EMM_REFRESH:
			/* for system debug. Added at v4.2.0 */
			OS_PRINTF(" EMM_SENT_EVENT---> MG_EMM_REFRESH\n");
			break;//return CAS_C_NOTIFY_EMM_REFRESH;
		case MG_EMM_STRATEGY:
			/* for system debug. Added at v4.2.1 */
			OS_PRINTF(" EMM_SENT_EVENT---> MG_EMM_STRATEGY\n");
			break;//return CAS_C_NOTIFY_EMM_STRATEGY;
		case MG_EMM_NOT_ENTIRE_MAIL:
			/* Added at v4.2.1 */
			OS_PRINTF(" EMM_SENT_EVENT---> MG_EMM_NOT_ENTIRE_MAIL\n");
			break;//return CAS_C_NOTIFY_NOT_ENTIRE_MAIL;
    /**************************************************************************/
    /***                 MG_CAS_ECM_Process  return Value            ***/
    /**************************************************************************/
    case MG_ERR_SCARD_NOT_ACT :
    case MG_ERR_SCARD_DISABLED:
      /* show please activate the son card */
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_CADR_NOT_ACT\n");
      return CAS_E_CADR_NOT_ACT;

    case MG_ERR_AUTHNEARBOOT_REMIND:
    case MG_ERR_AUTHNEARTIME_REMIND:
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_AUTHEN_RECENT_EXPIRE\n");
      return CAS_E_AUTHEN_RECENT_EXPIRE;

    case MG_ERR_AUTHEXPIRE_REMIND:
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_AUTHEN_EXPIRED\n");
      return CAS_E_AUTHEN_EXPIRED;

    case MG_ERR_ZONE_CODE:
      /* show the zone code is error */
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_ZONE_CODE_ERR\n");
      return CAS_E_ZONE_CODE_ERR;

    case MG_ERR_NOT_RECEIVED_SPID:
      /* */
      OS_PRINTF(" ECM_SENT_EVENT---> MG_ERR_NOT_RECEIVED_SPID\n");
      break;//return CAS_E_NOT_RECEIVED_SPID;

    case MG_ERR_AUTHENTICATION:
      /* */
      OS_PRINTF(" ECM_SENT_EVENT---> MG_ERR_AUTHENTICATION\n");
      break;//return CAS_E_CARD_AUTH_FAILED;

    case MG_ERR_NO_MONEY:
      /* show no money to watch current PPV channel */
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_CADR_NO_MONEY\n");
      return CAS_E_CADR_NO_MONEY;

    case MG_ERR_NOT_AUTHORED:
      /* show current channel not authed */
       CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_PROG_UNAUTH\n");
      return CAS_E_PROG_UNAUTH;

    case MG_ERR_CARD_LOCKED:
      /* show card locked */
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_CARD_LOCKED\n");
      return CAS_E_CARD_LOCKED;

    case MG_ERR_APP_LOCKED:
      /* show application locked */
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_SERVICE_LOCKED\n");
      return CAS_E_SERVICE_LOCKED;

    case MG_ERR_SC_PARTNER:
      /* */
      OS_PRINTF(" ECM_SENT_EVENT---> MG_ERR_SC_PARTNER\n");
      break;//return CAS_E_CARD_DIS_PARTNER;

    case MG_ERR_PIN_AUTHEN:
      /* show the partner error according to 'PinErrCount' */
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_CARD_DIS_PARTNER\n");
      return CAS_E_CARD_PARTNER_ERROR_CODE;

    case MG_ERR_CARD_NOT_READY:
      /* */
      OS_PRINTF(" ECM_SENT_EVENT---> MG_ERR_CARD_NOT_READY\n");
      break;//return CAS_E_CARD_REG_FAILED;

    case MG_ERR_EXTER_AUTHEN:
      /* show card authentication error */
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_EXTER_AUTHEN\n");
      return CAS_E_EXTER_AUTHEN;

    case MG_ERR_INTER_AUTHEN:
      /* */
      OS_PRINTF(" ECM_SENT_EVENT---> MG_ERR_INTER_AUTHEN\n");
      break;//return CAS_E_INTER_AUTHEN;

    case MG_ERR_GET_CARD_INFO:
      /* */
      OS_PRINTF(" ECM_SENT_EVENT---> MG_ERR_GET_CARD_INFO\n");
      break;//return CAS_E_CARD_INFO_GET;

    case MG_ERR_GET_CARD_DATA:
      /* */
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_CARD_DATA_GET\n");
      return CAS_E_CARD_DATA_GET;

    case MG_ERR_CALCU_GKAK:
      /* */
      OS_PRINTF(" ECM_SENT_EVENT---> MG_ERR_CALCU_GKAK\n");
      break;//return CAS_E_CALCU_GKAK;

    case MG_ERR_NEED_GKUPD:
      /* */
      OS_PRINTF(" ECM_SENT_EVENT---> MG_ERR_NEED_GKUPD\n");
      break;//return CAS_S_CARD_NEED_UPG;

    case MG_ERR_NOREG_CARD:
      /* show card not register */
      OS_PRINTF(" ECM_SENT_EVENT---> MG_ERR_NOREG_CARD\n");
      break;//return CAS_E_NOT_REG_CARD;

    case MG_ERR_ILLEGAL_CARD:
      /* show illegal card */
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_ILLEGAL_CARD\n");
      return CAS_E_ILLEGAL_CARD;

    case MG_ERR_DONOT_TELEVIEW_PPV:
    case MG_ERR_QUESTION_TELEVIEW_PPV:
      /* show OSD Box to let user select if he wanna watch PPV */
      /* Also can show PPV price or Card balance, etc */
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_C_IPP_BUY_OR_NOT\n");
      return CAS_C_IPPT_BUY_OR_NOT;

    case MG_ERR_CARD_NOTMATCH:
      /* show card not match */
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_CARD_NOT_FOUND\n");
      return CAS_E_CARD_NOT_FOUND;

    case MG_ERR_UPDATE_GK:
      /* show card not match */
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_CARD_FAILED\n");
      return CAS_E_CARD_FAILED;

    case MG_ERR_CARD_NOTFOUND:
      if(p_priv->card_status == SMC_CARD_REMOVE)
      {
          /* show no card */
          CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_NOT_REG_CARD\n");
          return CAS_E_NOT_REG_CARD;
      }
      break;

    case MG_ERR_CONTROL_PLAY:
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_C_USERCDN_LIMIT_CTRL_ENABLE\n");
      return CAS_C_USERCDN_LIMIT_CTRL_ENABLE;

    case MG_ERR_AUTH_CLOSED:
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_C_ENTITLE_CLOSED\n");
      return CAS_C_ENTITLE_CLOSED;

    case MG_ERR_AUTH_OVERDUE:
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_CARD_AUTHEN_OVERDUE\n");
      return CAS_E_CARD_AUTHEN_OVERDUE;

    case MG_ERR_AUTH_UPDATE:
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_S_CARD_AUTHEN_UPDATING\n");
      return CAS_S_CARD_AUTHEN_UPDATING;

    case MG_ERR_ILLEGAL_AUTH:
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_CARD_AUTH_FAILED\n");
      return CAS_E_CARD_AUTH_FAILED;

    case MG_ERR_CARD_STATUS_RENEW:
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_CARD_RENEW_RESET_STB\n");
      return CAS_E_CARD_RENEW_RESET_STB;

    case MG_ERR_CARD_OPERATE:
      CAS_ADT_PRINTF(" ECM_SENT_EVENT---> CAS_E_CARD_OPERATE_FAILE\n");
      return CAS_E_CARD_OPERATE_FAILE;

     /**************************************************************************/
    /***            MG_CAS_EMM/ECM_Process  return Value         ***/
    /**************************************************************************/
    case MG_SUCCESS:
      /* nothing should do */
       CAS_ADT_PRINTF(" !MG_SUCCESS !\n");
      return CAS_S_CLEAR_DISPLAY;
     break;
    case MG_FAILING:
      /* nothing should do */
      CAS_ADT_PRINTF(" !MG_FAILING !\n");
      break;
    default:
      /* */
      break;
  }

  return CAS_EVT_NONE;
}

static RET_CODE cas_adt_mg_init()
{
	static BOOL init_flag = FALSE;
	u8 temp_buffer[CAS_CARD_SN_MAX_LEN + 1] = {0};
	cas_adt_mg_priv_t *p_priv = g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;

	if (init_flag)
	{
	return SUCCESS;
	}

	/* init ADT_MG CAS module */
	p_g_mg_port_sem = &p_priv->mg_port_sem;
	if(CAS_LIB_FORMAL == p_priv->cas_lib_type)
	{	
		MG_Init_CAS(MG_LIB_FORMAL);
	}
	else
	{
		MG_Init_CAS(MG_LIB_TEST);
	}
	OS_PRINTF("MG_Init_CAS!!!!!!!!!!!!!!!!!!!!!!\n");
	init_flag = TRUE;
	cas_init_nvram_data(CAS_ID_ADT_MG);
	memset(&cas_mail, 0x00, sizeof(cas_mail));
	cas_mail.cas_mail_headers.has_read = 0xFF;
	cas_osd_msg.cas_osd_dn.has_read = TRUE;
	cas_osd_msg.cas_osd_up.has_read = TRUE;
	cas_get_stb_serial(CAS_ID_ADT_MG, temp_buffer);
	memcpy(p_priv->stb_serial, temp_buffer, STB_SERIAL_LEN);
	
	p_priv->cur_ecm_dmx_chan = 0xFF;
	cas_adt_mg_resetFilterAll();
	p_priv->card_status = SMC_CARD_REMOVE;

	return SUCCESS;
}

static RET_CODE cas_adt_mg_deinit()
{
  //p_g_mg_port_sem = NULL;

  return SUCCESS;
}

/**
 *  Get mail body by index
 *  it should use when application need get msg body use index
 *
 *  param[in] p_cas_msg: msg struct point,use to store getted msg bog
 *                  msg_id: msg id need to get
*/
static RET_CODE cas_adt_mg_get_mail_body_by_index(u8 *p_buffer, u8 msg_id)
{
    u8 i = 0;

    MT_ASSERT(NULL != p_buffer);

    CAS_ADT_PRINTF("%s:LINE:%d\n",__FUNCTION__, __LINE__);

    if(msg_id >= CAS_EMAIL_STORE_MAX_NUM)
    {
        CAS_ADT_PRINTF("%s:LINE:%d, mail index error !!!!\n", __FUNCTION__, __LINE__);
        return ERR_FAILURE;
    }

    for(; i < CAS_EMAIL_STORE_MAX_NUM; i++)
    {
      if(cas_mail.p_cas_mail_bodys.cas_mail_body[i].mail_id == msg_id)
      {
        /* Find the mail by index successfull */
        CAS_ADT_PRINTF("%s:LINE:%d, find the mail by index!!!mail index: %d\n", __FUNCTION__, __LINE__, i);
        break;
      }
    }

    if(i >= CAS_EMAIL_STORE_MAX_NUM)
    {
        CAS_ADT_PRINTF("%s:LINE:%d, do not found mail by index !!!!\n", __FUNCTION__, __LINE__);
        return ERR_FAILURE;
    }

    memcpy(p_buffer, &cas_mail.p_cas_mail_bodys.cas_mail_body[i].data, ADT_MG_CAS_MAIL_BODY_MAX_LEN);
    cas_mail.cas_mail_headers.max_num -= 1;
    //CAS_ADT_PRINTF("--->p_buffer: %s\n", __FUNCTION__, __LINE__, p_buffer);

    if(!(cas_mail.cas_mail_headers.has_read & (0x01<<msg_id)))
    {
        cas_mail.cas_mail_headers.has_read |= (0x01<<msg_id);
    }

    return SUCCESS;
}

/**
 *  To store mail new msg.
 *  it should use after cas_tr_store_new_short_msg calls.
 *
 *  param[in] p_cas_msg_struct: msg need to store.
 *  param[in] mail_num_state: mail num max than
 *
 *  return
 *  	      if store success, return SUCCESS.
 *			  if msg has been received, return RET_HAS_RECEIVED.
*/
static cas_save_mail_ret_e cas_adt_mg_save_new_mail(cas_mail_header_store_t *p_cas_mail_header, cas_mail_body_t *p_cas_mail_body)
{
    cas_mail_header_store_t *p_cas_mail_header_store = NULL;
    cas_mail_body_t *p_cas_mail_body_store = NULL;
    u32 ret = ERR_FAILURE;
    u8 free_email_index = 0xFF;

    ret = cas_adt_mg_search_free_email_node(&free_email_index);
    CAS_ADT_PRINTF("free_email_index:%d\n", free_email_index);

    if(SUCCESS != ret || free_email_index >= CAS_EMAIL_STORE_MAX_NUM)
    {
        /** CAS Email buffer is full **/
        return ERR_FAILURE;
    }

    CAS_ADT_PRINTF("%s:LINE:%d\n",__FUNCTION__, __LINE__);

    /* store msg header msg */
    p_cas_mail_header_store = &cas_mail.cas_mail_headers.p_mail_head[free_email_index];
    memset(p_cas_mail_header_store, 0x00, sizeof(cas_mail_header_store_t));

    memcpy(p_cas_mail_header_store->subject, p_cas_mail_header->subject, CAS_MAIL_SUBJECT_MAX_LEN);
    memcpy(p_cas_mail_header_store->creat_date, &p_cas_mail_header->creat_date, CAS_MAIL_DATE_MAX_LEN);
    memcpy(p_cas_mail_header_store->sender, &p_cas_mail_header->sender, CAS_MAIL_FROM_MAX_LEN);
    p_cas_mail_header_store->mail_id = free_email_index;
    p_cas_mail_header_store->body_len = p_cas_mail_header->body_len;
    cas_mail.cas_mail_headers.has_read &= ~(0x01<<free_email_index);
    cas_mail.cas_mail_headers.max_num += 1;

    CAS_ADT_PRINTF("%s:LINE:%d,p_cas_msg_header->body_len=%d\n",__FUNCTION__, __LINE__,p_cas_mail_header->body_len);
    CAS_ADT_PRINTF("%s:LINE:%d,p_cas_msg->cas_mail_headers.max_num=%d\n",__FUNCTION__, __LINE__, cas_mail.cas_mail_headers.max_num);

    /* store msg body */
    p_cas_mail_body_store = &cas_mail.p_cas_mail_bodys.cas_mail_body[free_email_index];
    memset(p_cas_mail_body_store, 0x00, sizeof(cas_mail_body_t));

    memcpy(p_cas_mail_body_store->data, p_cas_mail_body->data, ADT_MG_CAS_MAIL_BODY_MAX_LEN);
    p_cas_mail_body_store->mail_id = free_email_index;

    return SUCCESS;
}

static RET_CODE cas_adt_mg_card_reset(u32 slot, card_reset_info_t *p_info)
{
  cas_adt_mg_priv_t *p_priv
    = (cas_adt_mg_priv_t *)g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;
//  u32 system_time_tick =0x00;
#ifndef JAZZ_MG
  scard_config_t smc_cfg = {0};

  p_g_smc_dev = p_priv->p_smc_dev = (scard_device_t *)p_info->p_smc;

  /* reset smart card driver to match ADT_MG card's buad rate */
  scard_get_config(p_priv->p_smc_dev, &smc_cfg);
  smc_cfg.clk_f_rate = 372;
  smc_cfg.clk_ref = 10;
  scard_set_config(p_priv->p_smc_dev, &smc_cfg);
  //smc_cfg.clk_f_rate = 0;
  //smc_cfg.clk_ref = 0;
  //scard_get_config(p_priv->p_smc_dev, &smc_cfg);
  //OS_PRINTF("after set card config, F[%d], Clk[%d]\n", smc_cfg.clk_f_rate, smc_cfg.clk_ref);
  dev_io_ctrl(p_priv->p_smc_dev, SMC_WRITE_FUNCTION_CFG_1, 0);

  //system_time_tick = mtos_ticks_get();
  //CAS_ADT_PRINTF("SMC reset OK system_time: 0x%8x\n", system_time_tick);
#endif
  p_g_smc_dev = p_priv->p_smc_dev = (scard_device_t *)p_info->p_smc;
  p_priv->slot = slot;
  /* register card insert/remove callback to smart card driver */
  scard_pro_register_op(p_priv->p_smc_dev);
   p_priv->card_ready_flg = MG_FALSE;
   p_priv->card_status = SMC_CARD_INIT_OK;

#if 0 /* deal with MG_LIb_V3.1.2  */
  status = MG_Check_Card(p_info->p_atr->p_buf);
  if((status == MG_SUCCESS) || (status == MG_ERR_NEED_GKUPD))
  {
    p_priv->card_status = SMC_CARD_RESET_OK;
    CAS_ADT_PRINTF("cas: check card ok, ret[0x%x]\n", status);

    if(status == MG_ERR_NEED_GKUPD)
    {
      cas_send_event(p_priv->slot, CAS_ID_ADT_MG, CAS_S_CARD_UPG_START, 0);
      if(MG_SUCCESS != MG_CAS_Update_GK())
      {
        cas_send_event(p_priv->slot, CAS_ID_ADT_MG, CAS_E_CARD_UPG_FAILED, 0);
        return ERR_FAILURE;
      }
      cas_send_event(p_priv->slot, CAS_ID_ADT_MG, CAS_S_CARD_UPG_SUCCESS, 0);
    }
  }
  else
  {
    CAS_ADT_PRINTF("cas: check card falied\n");
    return ERR_FAILURE;
  }
#else
  MG_Info_CardInserted(p_info->p_atr->p_buf, p_priv->stb_serial);
  if(MG_Get_Card_Ready() == MG_TRUE)
  {
  	CAS_ADT_PRINTF("[%s]%d \n",__FUNCTION__,__LINE__);
    p_priv->card_ready_flg = MG_TRUE;
  }
#if 1
  CAS_ADT_PRINTF("--->ATR Info LEN: %d buf[]: 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x \n", p_info->p_atr->atr_len,
                                   p_info->p_atr->p_buf[0],  p_info->p_atr->p_buf[1],  p_info->p_atr->p_buf[2],  p_info->p_atr->p_buf[3],  p_info->p_atr->p_buf[4],
                                    p_info->p_atr->p_buf[5],  p_info->p_atr->p_buf[6],  p_info->p_atr->p_buf[7]);
#endif
#endif
  p_priv->ecm_systime = 0;
  p_priv->emm_0x84_systime = 0;
  p_priv->emm_0x85_systime = 0;
  p_priv->emm_0x87_systime = 0;
  cas_send_event(p_priv->slot, CAS_ID_ADT_MG, CAS_C_REQU_NIT_SECITON, 0);
  return SUCCESS;
}

static RET_CODE cas_adt_mg_card_remove()
{
  cas_adt_mg_priv_t *p_priv
    = (cas_adt_mg_priv_t *)g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;
  //u32 system_time_tick = 0x00;

  p_priv->card_status = SMC_CARD_REMOVE;
  p_priv->card_ready_flg = MG_FALSE;
  MG_Info_CardRemoved();
  cas_adt_mg_emm_resetFilter(0xFF);
  //system_time_tick = mtos_ticks_get();
  //CAS_ADT_PRINTF("SMC rmv system_time: 0x%8x\n", system_time_tick);

  return SUCCESS;
}

RET_CODE cas_adt_mg_card_is_removed(void)
{
  cas_adt_mg_priv_t *p_priv
    = (cas_adt_mg_priv_t *)g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;
   return (p_priv->card_status == SMC_CARD_REMOVE?SUCCESS:ERR_FAILURE);
}

void cas_adt_mg_set_reset_flag(void)
{
    cas_adt_mg_priv_t *p_priv
    = (cas_adt_mg_priv_t *)g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;

    p_priv->internal_reset_flag = 1;
    return;
}

static int cas_adt_mg_internal_reset_card(void)
{
    int ret;
    u8 buffer[32] = "";
    scard_atr_desc_t atr;
    card_reset_info_t card_rst_info = {0};
    atr.p_buf = buffer;
    atr.atr_len = 0;
    card_rst_info.p_atr = &atr;
    card_rst_info.p_smc = p_g_smc_dev;

    cas_adt_mg_card_remove();
    ret = scard_reset(p_g_smc_dev);
    if(SUCCESS != ret)
    {
      OS_PRINTF("cas: reset card failed\n");
    }
    ret = scard_active(p_g_smc_dev, &atr);
    if(SUCCESS != ret)
    {
      OS_PRINTF("cas: active card failed\n");
      return ret;
    }
    cas_adt_mg_card_reset(0, &card_rst_info);
    OS_PRINTF("Enter %s function\n", __FUNCTION__);
    return ret;
}

static RET_CODE cas_adt_mg_identify(u16 ca_sys_id)
{
  if((ca_sys_id >= 0x4a40) && (ca_sys_id <= 0x4a4f))
  {
    return SUCCESS;
  }

  return ERR_FAILURE;
}

static RET_CODE cas_adt_mg_resetFilterAll(void)
{
   cas_adt_mg_priv_t *p_priv = g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;
   u8 i;

    for (i = 0; i < CAS_DMX_CHANNEL_COUNT; i ++)
    {
        MG_CAS_LOCK(g_sem_mg_emmlock);
        if (cas_si_emm_chan[i].handle != DMX_INVALID_CHANNEL_ID  )
        {
            cas_table_derequest(p_priv->p_dmx_dev, cas_si_emm_chan+i);
        }
        MG_CAS_UNLOCK(g_sem_mg_emmlock);
        MG_CAS_LOCK(g_sem_mg_ecmlock);
        if (cas_si_ecm_chan[i].handle != DMX_INVALID_CHANNEL_ID  )
        {
            cas_table_derequest(p_priv->p_dmx_dev, cas_si_ecm_chan+i);
        }
        MG_CAS_UNLOCK(g_sem_mg_ecmlock);
    }
    return SUCCESS;
}

static RET_CODE cas_adt_mg_ecm_setFilterAll(void)
{
   cas_adt_mg_priv_t *p_priv = g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;
   cas_dmx_chan_t *p_chan = NULL;
   filter_struct_t ecmfilter;

    /* Set Table_id=0x80/0x81 filter */
    memset(&ecmfilter, 0, sizeof(filter_struct_t));
    ecmfilter.Data[0] = 0x80;
    ecmfilter.Mask[0] = 0xFE;

    /* associate new filter to the slot */
    MG_CAS_LOCK(g_sem_mg_ecmlock);
    p_chan = cas_adt_mg_ecm_request(p_priv->p_dmx_dev, p_priv->cur_ecm_pid, ecmfilter.Data, ecmfilter.Mask, p_priv->filter_mode);
    p_priv->ecm_systime = mtos_ticks_get();
    if(NULL == p_chan)
    {
          MG_CAS_UNLOCK(g_sem_mg_ecmlock);
          return ERR_FAILURE;
    }
    p_priv->cur_ecm_dmx_chan = p_chan->chan_index;
    MG_CAS_UNLOCK(g_sem_mg_ecmlock);
    return SUCCESS;
}

static RET_CODE cas_adt_mg_emm_setFilter(u8 emm_Tid)
{
   cas_adt_mg_priv_t *p_priv = g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;
   cas_dmx_chan_t *p_chan = NULL;
   filter_struct_t emmfilter_0x84;
   filter_struct_t emmfilter_0x85;
   filter_struct_t emmfilter_0x87;
   u16 emmcounter_0x84 = 0;
   u16 emmcounter_0x85 = 0;
   u16 emmcounter_0x87 = 0;
   u16 i;
   u8 card_ID[4];
   u8 temp_emm_Tid = 0xFF;
   temp_emm_Tid = emm_Tid;

  if(0xFF == temp_emm_Tid)
  {
    emm_Tid = 0x84;
  }
   /* Set Table_id=0x84 filter */
   if(0x84 == emm_Tid)
   {
        MG_Get_Card_ID(card_ID);
        CAS_ADT_PRINTF("--->MG_Get_Card_ID: 0x%2x 0x%2x 0x%2x 0x%2x\n", card_ID[0], card_ID[1], card_ID[2], card_ID[3]);

        emmcounter_0x84 = MG_Get_UCounter()+1;
        CAS_ADT_PRINTF("--->emmcounter_0x84: %d\n", emmcounter_0x84);

        /* some chip's filter should discard
        Table_Len 2 bytes artificial */

        memset(&emmfilter_0x84, 0, sizeof(filter_struct_t));
        emmfilter_0x84.Data[0] = 0x84;
        emmfilter_0x84.Data[1] = (MG_U8)((emmcounter_0x84>>8)&0xFF);
        emmfilter_0x84.Data[2] = (MG_U8)(emmcounter_0x84&0xFF);
        for(i = 3; i < 7; i++)
        {
            emmfilter_0x84.Data[i] = card_ID[i-3];
        }
        for(i = 0; i < 7; i++)
        {
            emmfilter_0x84.Mask[i] = 0xFF;
        }

        /* associate new filter to the slot */
        CAS_ADT_PRINTF("### EMM PID:%d\n", p_priv->cur_emm_pid);
        MG_CAS_LOCK(g_sem_mg_emmlock);
        p_chan = cas_adt_mg_emm_request(p_priv->p_dmx_dev, p_priv->cur_emm_pid, emmfilter_0x84.Data, emmfilter_0x84.Mask, p_priv->filter_mode);
		p_priv->emm_0x84_systime = mtos_ticks_get();
        MG_CAS_UNLOCK(g_sem_mg_emmlock);
        if(NULL == p_chan)
        {
              return ERR_FAILURE;
        }
   }

  if(0xFF == temp_emm_Tid)
  {
    emm_Tid = 0x85;
  }
    /* Set Table_id=0x85 filter */
   if(0x85 == emm_Tid)
   {
        emmcounter_0x85 = MG_Get_GCounter()+1;
        CAS_ADT_PRINTF("--->emmcounter_0x85: %d\n", emmcounter_0x85);

        memset(&emmfilter_0x85, 0, sizeof(filter_struct_t));
        emmfilter_0x85.Data[0] = 0x85;
        emmfilter_0x85.Data[1] = (MG_U8)((emmcounter_0x85>>8)&0xFF);
        emmfilter_0x85.Data[2] = (MG_U8)(emmcounter_0x85&0xFF);
        for(i = 0; i < 3; i++)
        emmfilter_0x85.Mask[i] = 0xFF;
        MG_CAS_LOCK(g_sem_mg_emmlock);
        p_chan = cas_adt_mg_emm_request(p_priv->p_dmx_dev, p_priv->cur_emm_pid, emmfilter_0x85.Data, emmfilter_0x85.Mask, p_priv->filter_mode);
		p_priv->emm_0x85_systime = mtos_ticks_get();
        MG_CAS_UNLOCK(g_sem_mg_emmlock);
        if(NULL == p_chan)
        {
              return ERR_FAILURE;
        }
   }

  if(0xFF == temp_emm_Tid)
  {
    emm_Tid = 0x87;
  }
    /* Set Table_id=0x87 filter */
   if(0x87 == emm_Tid)
   {
        emmcounter_0x87 = MG_Get_ACounter()+1;
        CAS_ADT_PRINTF("--->emmcounter_0x87: %d\n", emmcounter_0x87);

        memset(&emmfilter_0x87, 0, sizeof(filter_struct_t));
        emmfilter_0x87.Data[0] = 0x87;
        emmfilter_0x87.Data[1] = (MG_U8)((emmcounter_0x87>>8)&0xFF);
        emmfilter_0x87.Data[2] = (MG_U8)(emmcounter_0x87&0xFF);
        for(i = 0; i < 3; i++)
        emmfilter_0x87.Mask[i] = 0xFF;
        MG_CAS_LOCK(g_sem_mg_emmlock);
        p_chan = cas_adt_mg_emm_request(p_priv->p_dmx_dev, p_priv->cur_emm_pid, emmfilter_0x87.Data, emmfilter_0x87.Mask, p_priv->filter_mode);
		p_priv->emm_0x87_systime = mtos_ticks_get();
        MG_CAS_UNLOCK(g_sem_mg_emmlock);
        if(NULL == p_chan)
        {
              return ERR_FAILURE;
        }
   }

    return SUCCESS;
}

static RET_CODE cas_adt_mg_emm_resetFilter(u8 emm_Tid)
{
   cas_adt_mg_priv_t *p_priv = g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;
   u8 i;

    MG_CAS_LOCK(g_sem_mg_emmlock);
    if(0xFF == emm_Tid)
    {
        for (i = 0; i < CAS_DMX_CHANNEL_COUNT; i ++)
        {
            if (cas_si_emm_chan[i].handle != DMX_INVALID_CHANNEL_ID)
            {
                if((0x84 == cas_si_emm_chan[i].tid) ||(0x85 == cas_si_emm_chan[i].tid) || (0x87 == cas_si_emm_chan[i].tid))
                {
                    cas_table_derequest(p_priv->p_dmx_dev, cas_si_emm_chan+i);
                }
            }
        }
    }
    else
    {
        for (i = 0; i < CAS_DMX_CHANNEL_COUNT; i ++)
        {
            if (cas_si_emm_chan[i].handle != DMX_INVALID_CHANNEL_ID)
            {
                if((emm_Tid == cas_si_emm_chan[i].tid))
                {
                    cas_table_derequest(p_priv->p_dmx_dev, cas_si_emm_chan+i);
                }
            }
        }
    }
    MG_CAS_UNLOCK(g_sem_mg_emmlock);
    return SUCCESS;
}

static RET_CODE cas_adt_mg_ecm_resetFilterAll(void)
{
   cas_adt_mg_priv_t *p_priv = g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;
   u8 i;

#if 0
    for (i = 0; i < CAS_DMX_CHANNEL_COUNT; i ++)
    {
        if (cas_si_ecm_chan[i].handle != DMX_INVALID_CHANNEL_ID)
        {
            if(0x80 == cas_si_ecm_chan[i].tid)
            {
                MG_CAS_LOCK(g_sem_mg_ecmlock);
                cas_table_derequest(p_priv->p_dmx_dev, cas_si_ecm_chan+i);
                MG_CAS_UNLOCK(g_sem_mg_ecmlock);
            }
        }
    }
#else
    MG_CAS_LOCK(g_sem_mg_ecmlock);
    if ((0x80 == cas_si_ecm_chan[p_priv->cur_ecm_dmx_chan].tid) && (cas_si_ecm_chan[p_priv->cur_ecm_dmx_chan].handle != DMX_INVALID_CHANNEL_ID))
    {

        cas_table_derequest(p_priv->p_dmx_dev, cas_si_ecm_chan+p_priv->cur_ecm_dmx_chan);
    }
    else
    {
        for (i = 0; i < CAS_DMX_CHANNEL_COUNT; i ++)
        {
            if (cas_si_ecm_chan[i].handle != DMX_INVALID_CHANNEL_ID)
            {
                if(0x80 == cas_si_ecm_chan[i].tid)
                {
                    cas_table_derequest(p_priv->p_dmx_dev, cas_si_ecm_chan+i);
                }
            }
        }
    }
    MG_CAS_UNLOCK(g_sem_mg_ecmlock);
#endif
    return SUCCESS;
}

RET_CODE cas_adt_mg_emm_process(u8 *p_emm_buf, u32 *p_result)
{
  cas_adt_mg_priv_t *p_priv = g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;
  MG_STATUS ret = 0;
  MG_U8 result;
  u16 counter = 0;
  u32 event = 0;
  u8 emm_Tid = 0xFF;
  //u32 system_time_tick = 0x00;

  if(p_emm_buf[0] != 0x84
    && p_emm_buf[0] != 0x85
    && p_emm_buf[0] != 0x87)
  {
    cas_send_event(p_priv->slot, CAS_ID_ADT_MG, CAS_E_EMM_INVALID, 0);
    return ERR_PARAM;
  }

  //counter = MAKE_WORD(p_emm_buf[4], p_emm_buf[3]);
  //CAS_ADT_PRINTF("######## buf[3]:0x%x buf[4]:0x%x data:0x%x\n", p_emm_buf[3], p_emm_buf[4], p_emm_buf);

  counter = p_emm_buf[3] << 8 |p_emm_buf[4];
  //CAS_ADT_PRINTF("EMM--->T_id = : 0x%2x Counter() = 0x%4x\n", p_emm_buf[0], counter);
  emm_Tid = p_emm_buf[0];


  //system_time_tick = mtos_ticks_get();
  mtos_task_lock();
#if 0
  if((0x84 == p_emm_buf[0]) && ((counter-1) == MG_Get_UCounter()))
  {
    //CAS_ADT_PRINTF("MG_Get_UCounter() =: %d\n", MG_Get_UCounter());
    ret = MG_CAS_EMM_U_Process(p_emm_buf, &result);
  }
  else if((0x85 == p_emm_buf[0]) && ((counter-1) == MG_Get_GCounter()))
  {
    //CAS_ADT_PRINTF("MG_Get_GCounter() =: %d\n", MG_Get_GCounter());
    ret = MG_CAS_EMM_G_Process(p_emm_buf, &result);
  }
  else if((0x87 == p_emm_buf[0]) && ((counter-1) == MG_Get_ACounter()))
  {
    //CAS_ADT_PRINTF("MG_Get_ACounter() = : %d\n", MG_Get_ACounter());
    ret = MG_CAS_EMM_A_Process(p_emm_buf, &result);
  }
  else
  {
    CAS_ADT_PRINTF("EMM is not lib request!!!\n");
    return SUCCESS;
  }
  system_time_tick = mtos_ticks_get();
  CAS_ADT_PRINTF(" emm Process end system_time: 0x%8x\n", system_time_tick);
#else
    ret = MG_CAS_EMM_C_Process(p_emm_buf, &result);
    //CAS_ADT_PRINTF(" emm Process start system_time: 0x%8x\n", mtos_ticks_get());
#endif

  if(MG_SUCCESS != ret)
  {
    //cas_send_event(p_priv->slot, CAS_ID_ADT_MG, CAS_E_EMM_ERROR, 0);
    #if 0
    CAS_ADT_PRINTF("EMM process failed(err:%d Table ID:0x%x)\nEMM:", ret, p_emm_buf[0]);
    int i = 0;
    for(i=0;i<16;i++)
    {
        CAS_ADT_PRINTF("0X%X,", p_emm_buf[i]);
    }
    CAS_ADT_PRINTF("\n");
	#endif
    mtos_task_unlock();
    if(MG_TRUE == MG_Get_Card_Ready())
    {
        cas_adt_mg_emm_resetFilter(emm_Tid);
        cas_adt_mg_emm_setFilter(emm_Tid);
        p_priv->card_ready_flg = MG_TRUE; 
    }
    else
    {
      //CAS_ADT_PRINTF("When reset emm filter , the card is not ready!\n");
      p_priv->card_ready_flg = MG_FALSE;
    }
    return ERR_FAILURE;
  }
  else
  {
    event = parse_event(result);
    cas_send_event(p_priv->slot, CAS_ID_ADT_MG, event, 0);
    mtos_task_unlock();
    *p_result = event;

    if(MG_TRUE == MG_Get_Card_Ready())
    {
        cas_adt_mg_emm_resetFilter(emm_Tid);
        cas_adt_mg_emm_setFilter(emm_Tid);
        p_priv->card_ready_flg = MG_TRUE;
    }
    else
    {
     // CAS_ADT_PRINTF("When reset emm filter , the card is not ready!\n");
      p_priv->card_ready_flg = MG_FALSE;
    }
  }

  //CAS_ADT_PRINTF("### Table ID:0x%x  result: %d\n", p_emm_buf[0], result);
  //event = parse_event(result);
  //cas_send_event(p_priv->slot, CAS_ID_ADT_MG, event, 0);
  //*p_result = event;

  return SUCCESS;
}
#define ZONE_CODE_VALID (0)
#define ZONE_CODE_INVALID (1)
RET_CODE cas_adt_mg_ecm_process(u8 *p_ecm_buf, ecm_info_t *p_info, u32 *p_result)
{
    cas_adt_mg_priv_t *p_priv = g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;
    MG_STATUS result = 0;
    MG_U8 cw_changed = 0;
    MG_U8 prog_sta = 0;
    MG_U8 cw[8];
    u32 event = 0;
    s32 ret1;

#if 0 /* deal with MG_LIb_V3.1.2  */
    if(p_priv->card_status < SMC_CARD_RESET_OK)
    {
        return ERR_STATUS;
    }
#endif

    //result = MG_CAS_ECM_Process(p_ecm_buf, &cw_changed, &bal_upd, p_info->zone_code);
    result = MG_CAS_ECM_Process(p_ecm_buf, p_info->zone_code, &cw_changed, &prog_sta);
    //cas_adt_mg_ecm_resetFilterAll();
    //cas_adt_mg_ecm_setFilterAll();
    //CAS_ADT_PRINTF("--->MG_CAS_ECM_Process ret[%d], cw_changed[%d]\n", result, cw_changed);
    if (cw_changed)
    {
      CAS_ADT_PRINTF("##CW Changed system_time: 0x%8x\n", mtos_ticks_get());
      cas_send_event(p_priv->slot, CAS_ID_ADT_MG, CAS_S_GET_KEY, 0);
    }
    if(cw_changed & MG_CWUPD_BIT_ODD)
    {
        /* odd CW key changed */
        MG_Get_Odd_CW(cw);
        ret1 = dmx_descrambler_set_odd_keys(p_priv->p_dmx_dev, p_info->prog_v_cid, cw, 8);
        ret1 = dmx_descrambler_set_odd_keys(p_priv->p_dmx_dev, p_info->prog_a_cid, cw, 8);
        if(ret1 != SUCCESS)
        {
            CAS_ADT_PRINTF("@@##set cw error\n");
        }
    }

    if(cw_changed & MG_CWUPD_BIT_EVEN)
    {
        /* even CW key changed */
        MG_Get_Even_CW(cw);
        ret1 = dmx_descrambler_set_even_keys(p_priv->p_dmx_dev, p_info->prog_v_cid, cw, 8);
        ret1 = dmx_descrambler_set_even_keys(p_priv->p_dmx_dev, p_info->prog_a_cid, cw, 8);
        if(ret1 != SUCCESS)
        {
            CAS_ADT_PRINTF("@@##set cw error\n");
        }
    }

    if(prog_sta & MG_PROGSTA_BIT_PPVUpd)
    {
        /* disp ppv logo for 5s ,then disappear */
        cas_send_event(p_priv->slot, CAS_ID_ADT_MG, CAS_C_UPDATE_BALANCE, 0);
        CAS_ADT_PRINTF(" ECM_SENT_EVENT--->update show ppv\n");
        *p_result = event;
    }
    if(prog_sta & MG_PROGSTA_BIT_PreAuth)
    {
        cas_send_event(p_priv->slot, CAS_ID_ADT_MG, CAS_C_AUTH_LOGO_DISP, 0);
        CAS_ADT_PRINTF(" ECM_SENT_EVENT--->update show PreAuth\n");
        *p_result = event;
    }

    event = parse_event(result);
    cas_send_event(p_priv->slot, CAS_ID_ADT_MG, event, 0);
    if(CAS_E_ZONE_CODE_ERR == event && ZONE_CODE_VALID == p_priv->zone_code_flag)
    {
        cas_send_event(p_priv->slot, CAS_ID_ADT_MG, CAS_C_REQU_NIT_SECITON, 0);
        p_priv->zone_code_flag =ZONE_CODE_INVALID;
    }
    *p_result = event;

    return SUCCESS;
}

static RET_CODE cas_adt_mg_table_process(u32 t_id, u8 *p_buf, u32 *p_result)
{
  cas_adt_mg_priv_t *p_priv = g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;
  cas_pmt_t pmt = {0};
  cas_cat_t cat = {0};
  u32 i = 0;
  u16 section_len  =  MAKE_WORD(p_buf[2],SYS_GET_LOW_HALF_BYTE(p_buf[1])) + 3 - CAS_CRC_LEN;
  u16 network_id = 0;
  //u32 system_time_tick = 0;
  //OS_PRINTF("adt mg process table %d\n", t_id);
  if(t_id == CAS_TID_PMT)
  {
    cas_parse_pmt(p_buf, &pmt);
    if(pmt.video_pid == p_priv->cur_v_pid)
    {
      for(i = 0; i < pmt.ecm_cnt; i++)
      {
        if(SUCCESS == cas_adt_mg_identify(pmt.cas_descr[i].ca_sys_id))
        {
          if (p_priv->cur_ecm_pid == pmt.cas_descr[i].ecm_pid)
          {
            return SUCCESS;
          }
          //system_time_tick = mtos_ticks_get();
          //CAS_ADT_PRINTF(" pmt req system_time: 0x%8x\n", system_time_tick);
          CAS_ADT_PRINTF(" cas_adt_mg_table_process---> ecm_pid: 0x%2x\n", pmt.cas_descr[i].ecm_pid);

          p_priv->cur_ecm_pid = pmt.cas_descr[i].ecm_pid;
          cas_adt_mg_ecm_resetFilterAll();
          cas_adt_mg_ecm_setFilterAll();
          break;
        }
      }
    }
  }
  else if(t_id == CAS_TID_CAT)
  {
    cas_parse_cat(p_buf, &cat);
    for(i = 0; i < cat.emm_cnt; i++)
    {
      if(SUCCESS == cas_adt_mg_identify(cat.ca_desc[i].ca_sys_id))
      {
        CAS_ADT_PRINTF(" cas_adt_mg_table_process---> emm_pid: 0x%2x\n", cat.ca_desc[i].emm_pid);
        p_priv->cur_emm_pid = cat.ca_desc[i].emm_pid;
        if(TRUE == MG_Get_Card_Ready())
        {
            cas_adt_mg_emm_resetFilter(0xFF);
            cas_adt_mg_emm_setFilter(0xFF);
        }
        break;
      }
    }
  }
  else if(t_id == CAS_TID_NIT)
  {
    CAS_ADT_PRINTF(" cas_adt_mg_table_process---> CAS_TID_NIT\n");
    memcpy(g_cas_priv->nit_data, p_buf, section_len);
    g_cas_priv->nit_length = section_len;
    network_id =  (((u16)(g_cas_priv->nit_data[3])) << 8) | (g_cas_priv->nit_data[4]);
    p_priv->zone_code = (network_id&0x00FF);
    p_priv->zone_code_flag = ZONE_CODE_VALID;
  }
  else
  {
    /* t_id error */
  }

  return SUCCESS;
}

static RET_CODE cas_adt_mg_io_ctrl(u32 cmd, void *param)
{
	cas_adt_mg_priv_t *p_priv = g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv;
	MG_U8 ret = 0;
	MG_U8 *p_buf = NULL, *p_temp = NULL;
	MG_U8 data[2];
	MG_U8 try_times = 0;
	cas_card_info_t *p_card_info = NULL;
	cas_ppv_tag_info_t *p_ppv_tag_info = NULL;
	cas_pin_err_t *p_pin_err = NULL;
	cas_mail_headers_t *p_cas_mail_headers = NULL;
	cas_mail_header_t *p_cas_mail_header = NULL;
	cas_mail_body_t *p_cas_mail_body = NULL;
	u8 *p_data_buffer = NULL;
	burses_info_t *p_burses_info = NULL;
	ipps_info_t *p_ipp_info = NULL;
	ipp_buy_info_t *p_purch_info = NULL;
	chlid_card_status_info *p_feed_info = NULL;
	msg_info_t *p_msg_info =NULL;
	cas_force_channel_t *p_force_channel_info = NULL;
	finger_msg_t *p_finger_info = NULL;
	cas_burse_charge_t *p_charge_info = NULL;
	u16 temp = 0;
	u8 temp_unread_mail_index = 0xFF;
	//u32 system_time_tick = 0x00;
	cas_table_info_t *p_Cas_table_info = NULL;
	u8 i = 0x00;

	CAS_ADT_PRINTF(" cmd = %x\n",cmd);
	switch(cmd)
	{
		case CAS_IOCMD_CARD_INFO_GET:
			p_card_info = (cas_card_info_t *)param;
			memset(p_card_info, 0, sizeof(cas_card_info_t));

			memcpy(p_card_info->stb_sn, p_priv->stb_serial, STB_SERIAL_LEN);

			{
				u8 i,j;
				u8 asc_str[32] = {0};
				MG_Get_Card_SN(asc_str);
				for(i = 0, j = 0; i < 16; i += 2, j++)
				{
					sprintf((char *)(&p_card_info->sn[i]), "%02x", asc_str[j]);
				}
				CAS_ADT_PRINTF("CAS_IOCMD_CARD_INFO_GET sn = %s\n",p_card_info->sn);
			}

			#if 0 /* MGCAS V4 removed these function */
			p_card_info->card_version[0] = MG_Get_Card_Version();
			MG_Get_Card_Issue_Date(p_card_info->issue_date);
			MG_Get_Card_Expire_Date(p_card_info->expire_date);
			MG_Get_Card_ID(p_card_info->card_id);
			#endif

			temp = MG_Get_CASoft_Version();
			p_card_info->cas_ver[0] = (temp >> 8) & 0xFF;
			p_card_info->cas_ver[1] = (temp) & 0xFF;

			if(MG_Get_Card_Ready() == MG_TRUE)
			{
				p_card_info->card_state = TRUE;
				temp = MG_Get_RecentExpireDay();
			}
			else
			{
				temp = 0;
				p_card_info->card_state = FALSE;
				break;
			}
			CAS_ADT_PRINTF("CAS_IOCMD_CARD_INFO_GET ExpireDay = %d\n",temp);
			p_card_info->expire_date[0] = (temp >> 8) & 0xFF;
			p_card_info->expire_date[1] = temp & 0xFF;
			//MG_Get_CAS_ID(p_card_info->cas_id);
			//p_card_info->key_version = MG_Get_Card_Key_Version();
			ret = MG_Get_Partner_Status();
			if(ret == 1)
				p_card_info->paterner_stat = CAS_PATERNER;
			else
				p_card_info->paterner_stat = CAS_NOT_PATERNER;
			CAS_ADT_PRINTF("CAS_IOCMD_CARD_INFO_GET paterner_stat = %d\n",p_card_info->paterner_stat);

			MG_Get_EigenValue(p_card_info->card_eigen_valu);
			p_card_info->card_zone_code = MG_Get_ZoneCode();
			p_card_info->card_ctrl_code = MG_Get_CtrlCode();
			CAS_ADT_PRINTF("CAS_IOCMD_CARD_INFO_GET zone_code = %d\n",p_card_info->card_zone_code);
			CAS_ADT_PRINTF("CAS_IOCMD_CARD_INFO_GET ctrl_code = %d\n",p_card_info->card_ctrl_code);
			break;

		case CAS_IOCMD_CARD_LOCK_STA_GET:
			ret = MG_Get_Lock_Status();
			if(ret > 0)
			{
				*((u8 *)param) = CAS_CARD_SERVICE_LOCKED;
			}
			else
			{
				*((u8 *)param) = CAS_CARD_SERVICE_UNLOCKED;
			}
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_CARD_LOCK_STA_GET\n");
			break;

		case CAS_IOCMD_SMC_STATUS_GET:
			p_temp = (MG_U8 *)param;
			*p_temp = MG_Get_Card_Ready();
			if(*p_temp == MG_TRUE)
				*p_temp = CAS_CARD_IS_READY;   //Card insert
			else
				*p_temp = CAS_CARD_NOT_READY;   //Card error
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_SMC_STATUS_GET\n");
			break;

		case CAS_IOCMD_PATERNER_STA_GET:
			p_temp = (MG_U8 *)param;
			ret = MG_Get_Partner_Status();
			if(ret == 1)
				*p_temp = CAS_PATERNER;
			else
				*p_temp = CAS_NOT_PATERNER;
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_PATERNER_STA_GET\n");
			break;

		case CAS_IOCMD_BURSE_INFO_GET:
			p_burses_info = (burses_info_t *)param;
			//p_burses_info->p_burse_info[0].cash_value = MG_Get_Card_Balance();
			//p_burses_info->p_burse_info[0].credit_limit = MG_Get_Card_OverDraft();
			p_burses_info->p_burse_info[0].cash_value = MG_Get_Card_Quota();
			p_burses_info->p_burse_info[0].balance = MG_Get_Card_Consume();
			p_burses_info->index = 0;
			p_burses_info->max_num = 1;
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_BURSE_INFO_GET\n");
			break;

		case CAS_IOCMD_IPPV_INFO_GET:
			p_ipp_info = (ipps_info_t *)param;
			ret = MG_Get_Current_Program();
			if(1 == MG_Get_Program_Type(ret))
			{
				p_ipp_info->p_ipp_info[0].price = MG_Get_Program_CostPerMin(ret);
				p_ipp_info->p_ipp_info[0].type = MG_Get_Program_Type(MG_Get_Current_Program());
				p_ipp_info->p_ipp_info[0].book_state_flag = MG_Get_TeleviwPPVTag(MG_Get_Current_Program());
				p_ipp_info->max_num = 1;
			}
			else
			{
				p_ipp_info->p_ipp_info[0].type = 0;
			}
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_IPPV_INFO_GET\n");
			break;

		case  CAS_IOCMD_IPP_BUY_INFO_GET:
			p_purch_info = (ipp_buy_info_t *)param;
			//p_purch_info->ipp_charge = MG_Get_Program_Price(MG_Get_Current_Program());
			p_purch_info->ipp_charge = MG_Get_Program_CostPerMin(MG_Get_Current_Program());
			p_purch_info->ipp_unit_time = 60;
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_IPP_BUY_INFO_GET\n");
			break;

		case CAS_IOCMD_CUR_PRG_NUM_GET:
			p_temp = (MG_U8 *)param;
			*p_temp = MG_Get_Current_Program();
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_CUR_PRG_NUM_GET\n");
			break;

		case CAS_IOCMD_PPV_TAG_GET:
			p_ppv_tag_info = (cas_ppv_tag_info_t *)param;
			p_ppv_tag_info->tag = MG_Get_TeleviwPPVTag(MG_Get_Current_Program());
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_PPV_TAG_GET\n");
			break;

		case CAS_IOCMD_PURCHASE_SET:
			p_purch_info = (ipp_buy_info_t *)param;
			MG_Set_CurTeleviewPPVTag(p_purch_info->state_flag);
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_PPV_TAG_SET\n");
			break;

		case CAS_IOCMD_ZONE_CODE_GET:
			if(MG_TRUE != MG_Get_Card_Ready())
			{
				*((u8 *)param) = 0;
				return ERR_STATUS;
			}
			*((u8 *)param) = MG_Get_ZoneCode();
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_ZONE_CODE_GET\n");
			break;
		case CAS_IOCMD_CTRL_CODE_GET:
			if(MG_TRUE != MG_Get_Card_Ready())
			{
				*((u16 *)param) = 0;
				return ERR_STATUS;
			}
			*((u16 *)param) = MG_Get_CtrlCode();
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_CTRL_CODE_GET\n");
			break;
		case CAS_IOCMD_PIN_ERR_CNT_GET:
			ret = MG_Get_PinErr_Status(&try_times);
			p_pin_err = (cas_pin_err_t *)param;
			if(ret == MG_PINERR_STB_PAIRED)
			{
				p_pin_err->err_code = CAS_STB_PATERNERED;
			}
			else if(ret == MG_PINERR_CARD_PAIRED)
			{
				p_pin_err->err_code = CAS_CARD_PATERNERED;
			}
			else
			{
				p_pin_err->err_code = CAS_CARD_NONE_PATERNERED;
				p_pin_err->err_cnt = try_times;
			}
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_PIN_ERR_CNT_GET\n");
			break;

		case CAS_IOCMD_MON_CHILD_STATUS_GET:
			p_feed_info = (chlid_card_status_info *)param;

			if(MG_TRUE != MG_Get_Card_Ready())
			{
				p_feed_info->is_child = 0xff;
				break;
			}
			
			memset(p_feed_info, 0, sizeof(chlid_card_status_info));
			ret = MG_Get_MSCard_Ext(p_feed_info->parent_card_sn, &p_feed_info->child_card_remain_time[0]);
			OS_PRINTF("parent_card_sn[0]: %02x\n",p_feed_info->parent_card_sn[0]);
			OS_PRINTF("parent_card_sn: %s\n",p_feed_info->parent_card_sn);

			if(ret == 1) /* mon card */
			{
				p_feed_info->is_child = 0;
			}
			else /* son card */
			{
				p_feed_info->is_child = 1;
			}
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_MON_CHILD_STATUS_GET\n");
			break;

		/* deal with V3.1.2 adapter layer process flash R/W */
		case CAS_IOCMD_GET_CURRENT_MAIL:
			ret = cas_adt_mg_search_unread_email_node(&temp_unread_mail_index);
			p_cas_mail_header = (cas_mail_header_t *)param;
			p_data_buffer = (u8 *)p_cas_mail_header + sizeof(cas_mail_header_t);
			if(SUCCESS == ret)
			{
				memcpy(p_cas_mail_header->subject, &cas_mail.cas_mail_headers.p_mail_head[temp_unread_mail_index].subject, ADT_MG_CAS_MAIL_SUBJECT_MAX_LEN);
				memcpy(p_cas_mail_header->sender, &cas_mail.cas_mail_headers.p_mail_head[temp_unread_mail_index].sender, ADT_MG_CAS_MAIL_FROM_MAX_LEN);
				memcpy(p_cas_mail_header->creat_date, &cas_mail.cas_mail_headers.p_mail_head[temp_unread_mail_index].creat_date, ADT_MG_CAS_MAIL_DATE_MAX_LEN);
				p_cas_mail_header->body_len = cas_mail.cas_mail_headers.p_mail_head[temp_unread_mail_index].body_len;

				cas_adt_mg_get_mail_body_by_index(p_data_buffer, temp_unread_mail_index);
			}
			//CAS_ADT_PRINTF(" CAS_RCV_IOCMD--->GET_CURRENT_MAIL mail creat date: %d  %d %d %d %d %d\n", p_cas_mail_header->creat_date[0], p_cas_mail_header->creat_date[1], p_cas_mail_header->creat_date[2],
						//p_cas_mail_header->creat_date[3], p_cas_mail_header->creat_date[4], p_cas_mail_header->creat_date[5]);
			//CAS_ADT_PRINTF(" CAS_RCV_IOCMD--->GET_CURRENT_MAIL mail subject: %s body LEN: %d email_body: %s\n", p_cas_mail_header->subject, p_cas_mail_header->body_len, p_data_buffer);
			break;

		case CAS_IOCMD_GET_MAIL_POLICY:
			*(u8 *)param = p_priv->mail_cfg.mail_policy;
			break;

		case CAS_IOCMD_GET_MAIL_MAX:
			*(u8 *)param = p_priv->mail_cfg.max_mail_num;
			break;

		case CAS_IOCMD_MAIL_DELETE_BY_INDEX:
			cas_del_mail_data(CAS_ID_ADT_MG, *(u32*)param);
			break;

		case CAS_IOCMD_MAIL_DELETE_ALL:
			cas_del_all_mail_data(CAS_ID_ADT_MG);
			break;

		case CAS_IOCMD_MAIL_HEADER_GET:
			p_cas_mail_headers = (cas_mail_headers_t *)param;
			cas_get_mail_headers(CAS_ID_ADT_MG, (u8 *)p_cas_mail_headers);
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_MAIL_HEADER_GET\n");
			break;

		case CAS_IOCMD_MAIL_BODY_GET:
			p_cas_mail_body = (cas_mail_body_t *)param;
			cas_get_mail_body(CAS_ID_ADT_MG, p_cas_mail_body->data, p_cas_mail_body->mail_id);
			OS_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_MAIL_BODY_GET\n");
			OS_PRINTF(" data  : %s\n", p_cas_mail_body->data);
			OS_PRINTF(" data_len  : %d\n", p_cas_mail_body->data_len);

			break;

		case CAS_IOCMD_VIDEO_PID_SET:
			p_priv->cur_v_pid = *((u16 *)param);
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_VIDEO_PID_SET\n");
			break;

		case CAS_IOCMD_AUDIO_PID_SET:
			p_priv->cur_a_pid = *((u16 *)param);
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_AUDIO_PID_SET\n");
			break;

		case CAS_IOCMD_CA_SYS_ID_GET:
			/* Get sys ID */
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_CA_SYS_ID_GET\n");
			break;

		case CAS_IOCMD_MSG_GET:
			p_msg_info = (msg_info_t *)param;
			p_msg_info->type = 1;
			p_msg_info->osd_display.location = 1;
			p_msg_info->osd_display.roll_value = 1;
			p_msg_info->osd_display.roll_mode = 1;
			p_msg_info->data_len = CAS_OSD_MSG_DATA_MAX_LEN;
			memcpy(p_msg_info->data, &cas_osd_msg.cas_msg_data, CAS_OSD_MSG_DATA_MAX_LEN);
			CAS_ADT_PRINTF(" RCV_MG_EMM_MSG---> type: %d data: %s\n", p_msg_info->type, p_msg_info->data);
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_MSG_GET\n");
			break;

		case CAS_IOCMD_OSD_GET:
			p_msg_info = (msg_info_t *)param;
			p_msg_info->type = 0;

			if(TRUE != cas_osd_msg.cas_osd_up.has_read)
			{
				p_msg_info->osd_display.location = 0;
				p_msg_info->osd_display.roll_value = cas_osd_msg.cas_osd_up.disp_cnt;
				p_msg_info->osd_display.roll_mode = 1;
				p_msg_info->data_len = CAS_OSD_MSG_DATA_MAX_LEN;
				memcpy(p_msg_info->data, cas_osd_msg.cas_osd_up.cas_osd_data, CAS_OSD_MSG_DATA_MAX_LEN);
				cas_osd_msg.cas_osd_up.has_read = TRUE;
				CAS_ADT_PRINTF(" RCV_MG_EMM_OSD---> type: %d disp_mode: %d disp_cnt: %d data: %s\n",  p_msg_info->type, p_msg_info->osd_display.roll_mode, p_msg_info->osd_display.roll_value, p_msg_info->data);
			}
			else if(TRUE != cas_osd_msg.cas_osd_dn.has_read)
			{
				p_msg_info->osd_display.location = 1;
				p_msg_info->osd_display.roll_value = cas_osd_msg.cas_osd_dn.disp_cnt;
				p_msg_info->osd_display.roll_mode = 1;
				p_msg_info->data_len = CAS_OSD_MSG_DATA_MAX_LEN;
				memcpy(p_msg_info->data, cas_osd_msg.cas_osd_dn.cas_osd_data, CAS_OSD_MSG_DATA_MAX_LEN);
				cas_osd_msg.cas_osd_dn.has_read = TRUE;
				CAS_ADT_PRINTF(" RCV_MG_EMM_OSD---> type : %d disp_mode: %d disp_cnt: %d data: %s\n", p_msg_info->type, p_msg_info->osd_display.roll_mode, p_msg_info->osd_display.roll_value, p_msg_info->data);
			}
			else
			{
				CAS_ADT_PRINTF(" RCV_MG_EMM_OSD--->no OSD message unread \n");
			}

			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_OSD_GET\n");
			break;

		case CAS_IOCMD_FORCE_CHANNEL_INFO_GET:
			p_force_channel_info = (cas_force_channel_t *)param;
			MG_Get_ServiceID(data);
			p_force_channel_info->serv_id = ((u16)data[0] << 8) | data[1];
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_SERVICE_ID_GET\n");
			break;

		case CAS_IOCMD_AUTHEN_DAY_GET:
			*(u32 *)param = MG_Get_RecentExpireDay();
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_AUTHEN_DAY_GET %d\n", *(u32 *)param);
			break;

		case CAS_IOCMD_GROUP_ID_GET:
			MG_Get_Group_ID((MG_U8 *)param);
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_GROUP_ID_GET\n");
			break;

		case CAS_IOCMD_LOADER_INFO_GET:
			p_buf = MG_Get_LoaderInfo();
			p_temp = (MG_U8 *)param;
			memcpy(p_temp, p_buf, 16+p_buf[16]);
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_LOADER_INFO_GET\n");
			break;

		case CAS_IOCMD_EIGEN_VALUE_GET:
			MG_Get_EigenValue((u8 *)param);
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_EIGEN_VALUE_GET\n");
			break;
			
		case CAS_IOCMD_STOP:
		case CAS_IOCMD_LAST_PRG_INFO_CLR:
			//system_time_tick = mtos_ticks_get();
			//CAS_ADT_PRINTF("Clr ch system_time: 0x%8x\n", system_time_tick);
			cas_adt_mg_ecm_resetFilterAll();
			p_priv->cur_ecm_pid = 0x00;
			MG_Clean_LastPNo_Info();
			//system_time_tick = mtos_ticks_get();
			//CAS_ADT_PRINTF("reset ECM Filter system_time: 0x%8x\n", system_time_tick);
			CAS_ADT_PRINTF(" CAS_RCV_IOCMD---> CAS_IOCMD_LAST_PRG_INFO_CLR\n");
			break;

		case CAS_IOCMD_GET_SERIAL_DATA:
			memcpy((u8 *)param, p_priv->stb_serial, STB_SERIAL_LEN);
			break;

		case CAS_IOCMD_FINGER_MSG_GET:
			p_finger_info = (finger_msg_t *)param;
			MG_Get_Card_SN(p_finger_info->data);
			p_finger_info->during = 180;
			break;

		case CAS_IOCMD_PPV_CHARGE_GET:
			p_charge_info = (cas_burse_charge_t *)param;
			//p_charge_info->charge_value = MG_Get_ChargeValue();
			p_charge_info->charge_value = MG_Get_ChargeAmount();
			CAS_ADT_PRINTF("charge_value = %d",p_charge_info->charge_value);
			break;

		case CAS_IOCMD_SET_ECMEMM:
			p_Cas_table_info = (cas_table_info_t *)param;

			for(i = 0; i < CAS_MAX_ECMEMM_NUM; i++)
			{
				if(SUCCESS == cas_adt_mg_identify(p_Cas_table_info->ecm_info[i].ca_system_id))
				{
					p_priv->cur_ecm_pid = p_Cas_table_info->ecm_info[i].ecm_id;
					cas_adt_mg_ecm_resetFilterAll();
					cas_adt_mg_ecm_setFilterAll();
					//system_time_tick = mtos_ticks_get();
					//CAS_ADT_PRINTF(" pmt req system_time: 0x%8x\n", system_time_tick);
					CAS_ADT_PRINTF(" cas_adt_mg_table_process2---> ecm_pid: 0x%2x\n", p_priv->cur_ecm_pid);
					break;
				}
			}
			break;

		default:
			break;
	}

	return SUCCESS;
}

RET_CODE cas_adt_mg_attach_v42x(cas_module_cfg_t *p_cfg, u32 *p_cam_id)
{
  cas_adt_mg_priv_t *p_priv = NULL;
  RET_CODE ret = ERR_FAILURE;
  u32 i = 0;
  g_cas_priv = cas_get_private_data();
  
  g_cas_priv->cam_op[CAS_ID_ADT_MG].attached = 1;
  g_cas_priv->cam_op[CAS_ID_ADT_MG].inited = 0;
  g_cas_priv->cam_op[CAS_ID_ADT_MG].func.init
    = cas_adt_mg_init;
  g_cas_priv->cam_op[CAS_ID_ADT_MG].func.deinit
    = cas_adt_mg_deinit;
  g_cas_priv->cam_op[CAS_ID_ADT_MG].func.card_reset
    = cas_adt_mg_card_reset;
  g_cas_priv->cam_op[CAS_ID_ADT_MG].func.card_remove
    = cas_adt_mg_card_remove;
  g_cas_priv->cam_op[CAS_ID_ADT_MG].func.identify
    = cas_adt_mg_identify;
  g_cas_priv->cam_op[CAS_ID_ADT_MG].func.table_process
    = cas_adt_mg_table_process;
  g_cas_priv->cam_op[CAS_ID_ADT_MG].func.io_ctrl
    = cas_adt_mg_io_ctrl;
  p_priv = g_cas_priv->cam_op[CAS_ID_ADT_MG].p_priv
    = mtos_malloc(sizeof(cas_adt_mg_priv_t));
  MT_ASSERT(NULL != p_priv);

  memset(p_priv, 0x00, sizeof(cas_adt_mg_priv_t));
  p_priv->p_dmx_dev = p_cfg->p_dmx_dev;
  p_priv->mail_cfg.max_mail_num = p_cfg->max_mail_num;
  p_priv->mail_cfg.mail_policy = p_cfg->mail_policy;
  p_priv->cas_lib_type = p_cfg->cas_lib_type;
  p_priv->filter_mode = p_cfg->filter_mode;
  for(i = 0; i < CAS_DMX_CHANNEL_COUNT; i++)
  {
    cas_si_emm_chan[i].handle = DMX_INVALID_CHANNEL_ID;
    cas_si_ecm_chan[i].handle = DMX_INVALID_CHANNEL_ID;
  }
  /* create task to handle the card inserting & removing */
  p_priv->p_EMM_Pro_svc = drvsvc_create(p_cfg->end_task_prio, p_cfg->p_task_stack, p_cfg->stack_size, 1);
  MT_ASSERT(NULL != p_priv->p_EMM_Pro_svc);
  ret = drvsvc_nod_insert(p_priv->p_EMM_Pro_svc, _adt_mg_EMM_Process_task,
    p_priv, 0, 5);
  MT_ASSERT(SUCCESS == ret);
  /* create task to handle the ECM Process */
  p_cfg->p_task_stack = mtos_malloc(p_cfg->stack_size);
  p_priv->p_ECM_Pro_svc = drvsvc_create(p_cfg->task_prio, p_cfg->p_task_stack, p_cfg->stack_size, 1);
  MT_ASSERT(NULL != p_priv->p_ECM_Pro_svc);
  ret = drvsvc_nod_insert(p_priv->p_ECM_Pro_svc, _adt_mg_ECM_Process_task,
    p_priv, 0, 5);
  MT_ASSERT(SUCCESS == ret);
  MT_ASSERT(TRUE == mtos_sem_create(&g_sem_mg_emmlock, TRUE));
  MT_ASSERT(TRUE == mtos_sem_create(&g_sem_mg_ecmlock, TRUE));
  *p_cam_id = (u32)(&g_cas_priv->cam_op[CAS_ID_ADT_MG]);
  cas_adt_mg_init();
  return SUCCESS;
}
//lint +e64 +e438 +e502 +e527 +e539 +e545 +e550 +e570 +e668 +e701 +e713 +e734 +e746 +e831

