/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/

#include "string.h"
#include "trunk/sys_def.h"
#include "sys_types.h"
#include "sys_define.h"
#include "stdio.h"
#include "stdlib.h"


#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "mtos_int.h"
#include "mtos_misc.h"
#include "mtos_msg.h"



#include "drv_dev.h"
#include "charsto.h"
#include "drv_svc.h"
#include "dmx.h"

#include "lib_bitops.h"


#include "drv_svc.h"
#include "pti.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "cas_ware.h"
#include "cas_adapter.h"
#include "nim.h"
#include "smc_op.h"

#include "common.h"
#include "lib_util.h"
#include "lib_rect.h"
#include "vdec.h"

#include "display.h"
#include "ui_common.h"
#include "DiviguardCa.h"
#include "diviguard_cas_include.h"
#ifdef ENABLE_ADS
#include "ads_api_divi.h"
#endif

//#define CAS_DIVI_DEBUG

#ifdef CAS_DIVI_DEBUG
#define CAS_DIVI_PRINTF OS_PRINTF
#else
#define CAS_DIVI_PRINTF DUMMY_PRINTF
#endif

#define _MSG_SIZE() \
   (UINT32)sizeof(divi_queue_t);

static s_divi_task_struct_t s_task_record[DIVI_CAS_TASK_NUM] =
{
  {DIVI_FREE, 0, DIVI_CAS_TASK_PRIO_0},
  {DIVI_FREE, 0, DIVI_CAS_TASK_PRIO_1},
  {DIVI_FREE, 0, DIVI_CAS_TASK_PRIO_2},
  {DIVI_FREE, 0, DIVI_CAS_TASK_PRIO_3},
  {DIVI_FREE, 0, DIVI_CAS_TASK_PRIO_4},
  {DIVI_FREE, 0, DIVI_CAS_TASK_PRIO_5}
};

u8 g_data_temp[4096] = {0};

static divi_queue_t       divi_queue_array[DIVI_CAS_QUEUE_MAX_NUM] = {{0,0,{0,}},};
static divi_filter_struct_t s_tFilter[DIVI_FILTER_MAX_NUM] = {{0,},};
static divi_data_got_t  s_tData[DIVI_FILTER_MAX_NUM] = {{0,},};
static UINT8 s_tFilterBuf[DIVI_FILTER_MAX_NUM][DIVI_FILTER_BUFFER_SIZE] = {{0,},};
static UINT32 s_ca_lock = 0;
static UINT8 s_private_data[DIVI_FILTER_MAX_NUM][DIVI_PRIVATE_DATA_LEN_MAX] = {{0,},};
static UINT16 v_channel = 0xffff;
static UINT16 a_channel = 0xffff;
static UINT16 old_v_channel = 0xffff;
static UINT16 old_a_channel = 0xffff;
static divi_sem_node_t divi_sem_list[DIVI_CAS_SEM_MAX_NUM] = {{{0,0,0,0},0,},};

msg_info_t divi_OsdMsg = {0, {0,},};


extern int dmx_jazz_wait_for_data(int timeout);
extern int dmx_jazz_wait_for_data_stop(void);
extern cas_adapter_priv_t g_cas_priv;
extern BOOL s_divi_entitle_flag; //0:entitle  1:no entitle



typedef char    *ck_va_list;
typedef unsigned int  CK_NATIVE_INT;
typedef unsigned int  ck_size_t;
#define  CK_AUPBND         (sizeof (CK_NATIVE_INT) - 1)
#define CK_BND(X, bnd)        (((sizeof (X)) + (bnd)) & (~(bnd)))
#define CK_VA_END(ap)         (void)0  /*ap = (ck_va_list)0 */
#define CK_VA_START(ap, A)    (void) ((ap) = (((char *) &(A)) + (CK_BND (A,CK_AUPBND))))
extern int ck_vsnprintf(char *buf, ck_size_t size, const char *fmt, ck_va_list args);
extern void set_event(cas_divi_priv_t *p_priv, u32 event);
extern UINT8 cas_divi_get_scramble_flag(void);
extern u8 divi_ca_nit_state;
extern void send_message_ca_feed(u32 message_id);



////////////////////////////////////////////////////////////////////////////////////////

/*!
    lock
*/
static void divi_ca_lock(void)
{
    //CAS_DIVI_PRINTF("[divi],divi_ca_lock:%d \n",s_ca_lock);
  mtos_sem_take((os_sem_t *)&s_ca_lock, 0);
}

/*!
    unlock
*/
static void divi_ca_unlock(void)
{
    //CAS_DIVI_PRINTF("[divi],divi_ca_unlock:%d \n",s_ca_lock);
    mtos_sem_give((os_sem_t *)&s_ca_lock);
}


static void divi_filter_free(divi_filter_struct_t *s_tFilter)
{
  dmx_device_t *p_dev = NULL;
  u32 ret = 0;
  
  CAS_DIVI_PRINTF("[DIVI]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);

  p_dev = (dmx_device_t *)dev_find_identifier(NULL
          , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  MT_ASSERT(NULL != p_dev->p_base);

  ret = dmx_chan_stop(p_dev, s_tFilter->m_dwReqHandle);
  if (ret != SUCCESS)
  {
  	CAS_DIVI_PRINTF("[DIVI]dmx chan stop failure!\n" );
  }

  ret = dmx_chan_close(p_dev, s_tFilter->m_dwReqHandle);
  if (ret != SUCCESS)
  {
  	CAS_DIVI_PRINTF("[DIVI]dmx chan close failure!\n" );
  }
}

/*!
  free all ecm request filter

  */
void divi_cas_all_ecm_req_free()
{
   UINT8 index = 0;

   CAS_DIVI_PRINTF("[divi],enter divi_cas_all_ecm_req_free \n");
   divi_ca_lock();
   for(index = 0; index < DIVI_FILTER_MAX_NUM; index++)
   {
    if(DIVI_USED == s_tFilter[index].m_FilterStatus &&
        CA_STB_FILTER_4 == s_tFilter[index].m_diviReqId)
    {
       divi_filter_free(&s_tFilter[index]);
       DIVI_TableReceived(s_tFilter[index].m_diviReqId,FALSE,s_tFilter[index].m_PID,NULL,0);
       memset((void *)&s_tFilter[index], 0, sizeof(divi_filter_struct_t));
    }
   }
   divi_ca_unlock();
   CAS_DIVI_PRINTF("[divi],leave divi_cas_all_ecm_req_free \n");

   return ;
}

/*!
  free all emm request filter

  */
void divi_cas_all_emm_req_free()
{
   UINT8 index = 0;

   CAS_DIVI_PRINTF("[divi],enter divi_cas_all_emm_req_free \n");
   divi_ca_lock();
   for(index = 0; index < DIVI_FILTER_MAX_NUM; index++)
   {
      if(DIVI_USED == s_tFilter[index].m_FilterStatus &&
         (CA_STB_FILTER_1 == s_tFilter[index].m_diviReqId ||
          CA_STB_FILTER_2 == s_tFilter[index].m_diviReqId ||
          CA_STB_FILTER_3 == s_tFilter[index].m_diviReqId))
     {
       divi_filter_free(&s_tFilter[index]);
       DIVI_TableReceived(s_tFilter[index].m_diviReqId,FALSE,s_tFilter[index].m_PID,NULL,0);
       memset((void *)&s_tFilter[index], 0, sizeof(divi_filter_struct_t));
     }
   }
   divi_ca_unlock();
   CAS_DIVI_PRINTF("[divi],leave divi_cas_all_emm_req_free \n");

   return ;
}
  /*!
    ABC
  */
void divi_add_data(UINT8 req_id, UINT8 u_ok, UINT16 pid,
                                UINT8 *p_data, UINT16 length,UINT8 uIndex)
{
  if(uIndex >= DIVI_FILTER_MAX_NUM)
  {
    CAS_DIVI_PRINTF("[divi],divi_add_data: Too much data, uIndex = %d\n", uIndex);
    return;
  }

  s_tData[uIndex].req_id = req_id;
  s_tData[uIndex].u_ok = u_ok;
  s_tData[uIndex].pid = pid;

#if 1
if(req_id != CA_STB_FILTER_4)
{
  CAS_DIVI_PRINTF("req_id = %d, pid = 0x%x, length = %d\n",req_id,pid,length);

  CAS_DIVI_PRINTF("[drv] get data[%d], %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
    length,p_data[0],p_data[1],p_data[2],p_data[3],p_data[4],p_data[5],p_data[6],p_data[7],
    p_data[8],p_data[9],p_data[10],p_data[11],p_data[12],p_data[13],p_data[14],p_data[15]);

}
#endif

  if(NULL == p_data || 0 == length)
  {
    s_tData[uIndex].p_data = NULL;
  }
  else
  {
     if(length > DIVI_PRIVATE_DATA_LEN_MAX)
     {
        CAS_DIVI_PRINTF("-----------length > 256-------\n");
        s_tData[uIndex].p_data = mtos_malloc(length);
        MT_ASSERT(s_tData[uIndex].p_data != NULL);
        memcpy(s_tData[uIndex].p_data, p_data, length);
     }
     else
     {
        CAS_DIVI_PRINTF("[divi] length = %d \n",length);
        memset(s_private_data[uIndex],0,DIVI_PRIVATE_DATA_LEN_MAX);
        s_tData[uIndex].p_data = s_private_data[uIndex];
        memcpy(s_tData[uIndex].p_data, p_data, length);
     }
  }
  s_tData[uIndex].length = length;

}

  /*!
    ABC
  */
void divi_send_all_data(UINT8 uDataNum)
{
  u8 uIndex = 0;
  u32 ret = 0;
  u32 tick = 0;
  static u8 s_orig_data[4][DIVI_PRIVATE_DATA_LEN_MAX] = {{0,},};

  if(uDataNum > DIVI_FILTER_MAX_NUM)
  {
    CAS_DIVI_PRINTF("[divi],gt_send_all_data: Too much data, uDataNum = %d\n", uDataNum);
    uDataNum = DIVI_FILTER_MAX_NUM;
  }

  //CAS_DIVI_PRINTF("Call divi_send_all_data and uDataNum = %d\n",uDataNum);

  for(uIndex = 0; uIndex < uDataNum; uIndex++)
  {
    CAS_DIVI_PRINTF("[divi] send req_id = %d  pid = %x m_uLength = %d \n", s_tData[uIndex].req_id, s_tData[uIndex].pid,
              s_tData[uIndex].length);

    if(DIVI_CA_CARD_INSERT != divi_get_card_status())
    {
      return;
    }

    if ((s_tData[uIndex].req_id != CA_STB_FILTER_4) && (memcmp(s_tData[uIndex].p_data, s_orig_data[s_tData[uIndex].req_id-1], s_tData[uIndex].length) == 0))
    {
      CAS_DIVI_PRINTF("[req_id=%d]Same data, not send to lib,table_id[0x%02x]!\n", s_tData[uIndex].req_id, s_tData[uIndex].p_data[0]);
    }
    else
    {
      CAS_DIVI_PRINTF("[req_id=%d]data is not same, send to lib!\n", s_tData[uIndex].req_id);
      CAS_DIVI_PRINTF("[SEND_DATA]req_id=%d, tick=%d\n", s_tData[uIndex].req_id, mtos_ticks_get());
	  tick = mtos_ticks_get();
	  if (s_tData[uIndex].req_id == CA_STB_FILTER_1)
	  {
		  static u32 last_f1_crc = 0;
		  static u32 count = 0;
		  u32 crc = 0;
		  u32 len = s_tData[uIndex].length;
		  crc = (s_tData[uIndex].p_data[len-4] << 24)
				  + (s_tData[uIndex].p_data[len-3] << 16)
				  + (s_tData[uIndex].p_data[len-2] << 8)
				  + (s_tData[uIndex].p_data[len-1]);
		  CAS_DIVI_PRINTF("CA_STB_FILTER_1 last[%08x] cur[%08x] count[%d]\n",
					  last_f1_crc,crc,count);  
		  if ((last_f1_crc == crc) && (count < 100))
		  {
			  count ++;
			  continue;
		  }
		  count = 0;
		  last_f1_crc = crc;
		  OS_PRINTF("[Filter_1] start!!\n");
		  
	  }   

	  CAS_DIVI_PRINTF("[divi],uIndex = %d uDataNum = %d\n",uIndex,uDataNum);
	  CAS_DIVI_PRINTF("[divi],pid = 0x%x req_id = 0x%x length = %d\n",s_tData[uIndex].pid,s_tData[uIndex].req_id,s_tData[uIndex].length);
	  DIVI_TableReceived(s_tData[uIndex].req_id,
                         s_tData[uIndex].u_ok,
                         s_tData[uIndex].pid,
                         s_tData[uIndex].p_data,
                         s_tData[uIndex].length);
	  if ((ret=(mtos_ticks_get()-tick)) > 10)
		  OS_PRINTF("[Filter_%d]cost time = %d\n", s_tData[uIndex].req_id, ret);

      memset(s_orig_data[s_tData[uIndex].req_id-1], 0, DIVI_PRIVATE_DATA_LEN_MAX);
      memcpy(s_orig_data[s_tData[uIndex].req_id-1], s_tData[uIndex].p_data, s_tData[uIndex].length);
    }

    
    if(NULL != s_tData[uIndex].p_data && s_tData[uIndex].length > DIVI_PRIVATE_DATA_LEN_MAX)
    {
      mtos_free(s_tData[uIndex].p_data);
      s_tData[uIndex].p_data = NULL;
    }

    memset(&s_tData[uIndex], 0, sizeof(divi_data_got_t));
  }
}
/*
#define MAX_DIVI_SECTION_LEN (4096)
u8 divi_section_buf[MAX_DIVI_SECTION_LEN] = {0,};

extern s32 dmx_si_chan_get_data_safe(void *p_dev, dmx_chanid_t channel, u32 maxBufLen,
                                       u8 **pp_data, u32 *p_size);
*/

  /*!
    ABC
  */
static void divi_ca_data_monitor(void *p_data)
{
  UINT32 err = 0;
  UINT8 index = 0;
  UINT8 filter_index[DIVI_FILTER_MAX_NUM] = {0,};
  UINT8 valid_filter_num = 0;
  UINT8 data_index = 0;

  UINT16 network_id = 0;

  #if 0
  UINT8 region_id[4] = {0,};
  INT32 region_err = -1;
  UINT32 region_event = CAS_C_CARD_ZONE_INVALID;
  #endif

  UINT8 *p_got_data = NULL;//divi_section_buf;//
//  UINT32 maxBufLen = MAX_DIVI_SECTION_LEN;
  UINT32 got_data_len = 0;
  dmx_device_t *p_dev = NULL;

  p_dev = (dmx_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
  MT_ASSERT(NULL != p_dev->p_base);

  while(1)
  {
     //ECM && EMM
     valid_filter_num = 0;
     memset(filter_index,0,DIVI_FILTER_MAX_NUM);
     //CAS_DIVI_PRINTF("data monitor\n");
     //find free filter
     divi_ca_lock();
     for(index = 0; index < DIVI_FILTER_MAX_NUM; index++)
     {
        if(DIVI_USED == s_tFilter[index].m_FilterStatus)
        {
           filter_index[valid_filter_num] = index;
           valid_filter_num++;
        }
     }
     divi_ca_unlock();

     //valid filter doesn't exist
     if(0 == valid_filter_num)
     {
        mtos_task_sleep(20);
        continue;
     }

     //read data
     divi_ca_lock();
     data_index = 0;
     for(index = 0; index < valid_filter_num; index++)
     {

     #if 1
        err = dmx_si_chan_get_data(p_dev,
                                   s_tFilter[filter_index[index]].m_dwReqHandle,
                                   &p_got_data, &got_data_len);
     #else
        err=dmx_si_chan_get_data_safe(p_dev,
                            s_tFilter[filter_index[index]].m_dwReqHandle,
                            maxBufLen,
                            &p_got_data,
                            &got_data_len);
     #endif
        if(0 != err)
        {
           if(mtos_ticks_get() - s_tFilter[filter_index[index]].m_dwStartMS
               > s_tFilter[filter_index[index]].m_timeout)
           {
              if(s_tFilter[filter_index[index]].m_diviReqId == CA_STB_FILTER_4)
              {
                 CAS_DIVI_PRINTF("\n [divi], ecm timeout free ecm  filter \n");

                 divi_filter_free(&s_tFilter[filter_index[index]]);
              }

              CAS_DIVI_PRINTF("[divi], get data timeout ! \n");
              #if 0
              divi_add_data(s_tFilter[filter_index[index]].m_diviReqId, TRUE,
                                    s_tFilter[filter_index[index]].m_PID, p_got_data,
                                    got_data_len, data_index);
              #else
              divi_add_data(s_tFilter[filter_index[index]].m_diviReqId, FALSE,
                                         s_tFilter[filter_index[index]].m_PID,
                                         NULL, 0, data_index);
              #endif
              data_index++;


              if(s_tFilter[filter_index[index]].m_diviReqId == CA_STB_FILTER_4)
              {
                  memset((void *)&s_tFilter[filter_index[index]], 0,
                                                         sizeof(divi_filter_struct_t));
              }
           }
        }
        else
        {
           CAS_DIVI_PRINTF("get Data success\n");

           #if 0
           if(s_tFilter[filter_index[index]].m_uFilterLen > DMX_SECTION_FILTER_SIZE)
           {
              u8 FilterLen = s_tFilter[filter_index[index]].m_uFilterLen - DMX_SECTION_FILTER_SIZE;
              u8 i = 0;
              for(i=0; i<FilterLen; i++)
              {

                CAS_DIVI_PRINTF("index = %d,data = 0x%x, mask = 0x%x, match = 0x%x\n",
                        14+i,p_got_data[14+i],s_tFilter[filter_index[index]].m_uFilterMask[DMX_SECTION_FILTER_SIZE+i],
                        s_tFilter[filter_index[index]].m_uFilterData[DMX_SECTION_FILTER_SIZE+i]);
              
                if((p_got_data[14+i] & s_tFilter[filter_index[index]].m_uFilterMask[DMX_SECTION_FILTER_SIZE+i]) != 
                (s_tFilter[filter_index[index]].m_uFilterData[DMX_SECTION_FILTER_SIZE+i]& s_tFilter[filter_index[index]].m_uFilterMask[DMX_SECTION_FILTER_SIZE+i]))
                {
                  
                  CAS_DIVI_PRINTF("is not match\n");
                  continue;
                }
                
              }
           
           
             
           }
           #else
           if(s_tFilter[filter_index[index]].m_uFilterLen == DIVI_FILTER_DEPTH_SIZE)
           {

              CAS_DIVI_PRINTF("data = 0x%x, mask = 0x%x, match = 0x%x\n",
                      p_got_data[14],s_tFilter[filter_index[index]].m_uFilterMask[DIVI_FILTER_DEPTH_SIZE-1],
                      s_tFilter[filter_index[index]].m_uFilterData[DIVI_FILTER_DEPTH_SIZE-1]);
            
              if((p_got_data[14] & s_tFilter[filter_index[index]].m_uFilterMask[DIVI_FILTER_DEPTH_SIZE-1]) != 
              (s_tFilter[filter_index[index]].m_uFilterData[DIVI_FILTER_DEPTH_SIZE-1]& s_tFilter[filter_index[index]].m_uFilterMask[DIVI_FILTER_DEPTH_SIZE-1]))
              {
                
                CAS_DIVI_PRINTF("is not match\n");
                continue;
              }
                       
           }
           #endif

           if(s_tFilter[filter_index[index]].m_diviReqId == CA_STB_FILTER_4)
           {
              //ecm data
			  UINT8 card_sn[DIVI_CARD_SN_LEN] = {0};
              CAS_DIVI_PRINTF("\n\n --------------[divi],Ecm data -------------------\n");

			  if(DIVI_CA_CARD_INSERT == divi_get_card_status() && DIVI_GetSMCNO(card_sn) != DIVI_OK)
			  {
				  divi_ca_unlock();
				  mtos_task_sleep(10);
				  continue;
			  }
			  
              divi_filter_free(&s_tFilter[filter_index[index]]);

              if(DIVI_CA_CARD_INSERT == divi_get_card_status())
              {
              	   CAS_DIVI_PRINTF("[divi],1 Ecm data_index = %d got_data_len = %d\n",data_index,got_data_len);
				   CAS_DIVI_PRINTF("[divi],1 Ecm p_got_data = %x %x \n",p_got_data[0],p_got_data[1]);
                   divi_add_data(s_tFilter[filter_index[index]].m_diviReqId, TRUE,
                                      s_tFilter[filter_index[index]].m_PID, p_got_data,
                                      got_data_len, data_index);
                   data_index++;
              }
              else
              {
                   CAS_DIVI_PRINTF("[divi],1 Ecm data_index = %d got_data_len = %d\n",data_index,got_data_len);
				   CAS_DIVI_PRINTF("[divi],1 Ecm p_got_data = %x %x \n",p_got_data[0],p_got_data[1]);
              	   CAS_DIVI_PRINTF("[divi],2 Ecm data_index = %d \n",data_index);
                   divi_add_data(s_tFilter[filter_index[index]].m_diviReqId, FALSE,
                                         s_tFilter[filter_index[index]].m_PID,
                                         NULL, 0, data_index);
                   data_index++;
              }

           }
           else if(s_tFilter[filter_index[index]].m_diviReqId == CA_STB_FILTER_1
                      || s_tFilter[filter_index[index]].m_diviReqId == CA_STB_FILTER_2)
           {
              //emm data
              CAS_DIVI_PRINTF("\n\n --------------[divi],Emm data -------------------\n");

              divi_add_data(s_tFilter[filter_index[index]].m_diviReqId, TRUE,
                                    s_tFilter[filter_index[index]].m_PID,  p_got_data,
                                    got_data_len, data_index);
              data_index++;
           }
           else if(s_tFilter[filter_index[index]].m_diviReqId == CA_STB_FILTER_3)//0x84
           {
              CAS_DIVI_PRINTF("\n\n--------------[divi],ads data -------------------\n");
              //ads data
              
              if(p_got_data[5] == 0x08 || p_got_data[5] == 0x09)
              {
                #ifdef ADS_DIVI_SUPPORT
              	mtos_task_lock();
                divi_ads_add_data(p_got_data,got_data_len);
				mtos_task_unlock();
                #endif
              }
			        else
			        {
			  	      divi_add_data(s_tFilter[filter_index[index]].m_diviReqId, TRUE,
                                    s_tFilter[filter_index[index]].m_PID,  p_got_data,
                                    got_data_len, data_index);
              	data_index++;
			        }
              
           }
           else if(s_tFilter[filter_index[index]].m_diviReqId == NIT_TABLE_ID)
           {
      

              network_id = (((u16)(p_got_data[3])) << 8) | (p_got_data[4]);
              
              OS_PRINTF("[divi],table_id=0x%x, \n",p_got_data[0]);
              cas_divi_set_network_id(network_id);
              divi_ca_nit_state = 1;

        #if 0
              region_err = DIVI_GetRegion(region_id);

              DIVI_GetCurr_NetRegionID()
              if(region_err == DIVI_OK)
              {
                CAS_DIVI_PRINTF("network_id=0x%x,region_id=0x%x \n",network_id,region_id);
                if(0 == memcmp(network_id,region_id,4))
                {
                     region_event = CAS_C_CARD_ZONE_CHECK_OK;
                }
              }

              cas_divi_zone_check_end(region_event);

              divi_ca_unlock();
              mtos_task_sleep(10);
              continue;
        #endif
           }
           else
           {
              CAS_DIVI_PRINTF("\n [divi],request filter isn't emm/ecm/nit !!! \n");
              memset((void *)&s_tFilter[filter_index[index]], 0,
                                                     sizeof(divi_filter_struct_t));
              break;
           }

           if(s_tFilter[filter_index[index]].m_diviReqId == CA_STB_FILTER_4)
           {
               //free emm data filter
              CAS_DIVI_PRINTF("\n [divi],free ecm filter \n");

              //divi_filter_free(&s_tFilter[filter_index[index]]);
              memset((void *)&s_tFilter[filter_index[index]],
                     0, sizeof(divi_filter_struct_t));
           }

        }
     }
     divi_ca_unlock();

     //send data
     divi_send_all_data(data_index);
     mtos_task_sleep(10);
  }

}


/*!
  get free queue

  \param[in] queue_id.
  \return p_message ,message pointer.
  */
divi_queue_t *divi_free_queue_get(UINT32 queue_id)
{
  UINT32 index = 0;
  divi_queue_t *p_message = NULL;

  for(index = 0; index < DIVI_CAS_QUEUE_MAX_NUM; index++)
  {
     if(DIVI_FREE == divi_queue_array[index].queue_status)
     {
        divi_queue_array[index].queue_id = queue_id;
        divi_queue_array[index].queue_status = DIVI_USED;
        p_message = &divi_queue_array[index];
        break;
     }
  }

  CAS_DIVI_PRINTF("[divi], index=%d, message addr=0x%x \n",index, (UINT32)p_message);
  return p_message;
}

/*!
  free semphore
  
  \param[in] sem_index.
  \return success if get semphore else FAILTURE
  */
INT32 divi_free_sem_get(UINT8 *sem_index)
{
  UINT8 index = 0;
  MT_ASSERT(sem_index != NULL);
  
  for(index = 0;index < DIVI_CAS_SEM_MAX_NUM; index++)
  {
      if(divi_sem_list[index].sem_status == DIVI_FREE)
      {
          *sem_index = index;
          divi_sem_list[index].sem_status = DIVI_USED;
          return SUCCESS;
      }
  }
  return FAILTURE;
}

/*!
  check semphore is used
  
  \param[in] sem.
  \return SUCCESS if the sem is used else FAILTURE
  */
INT32 divi_sem_check(UINT32 sem)
{
  UINT8 index = 0;

  for(index = 0;index < DIVI_CAS_SEM_MAX_NUM; index++)
  {
      if(divi_sem_list[index].sem_id == sem 
         && divi_sem_list[index].sem_status == DIVI_USED)
      {
          return SUCCESS;
      }
  }
  return FAILTURE;
}

/*!
  abc
  */
void DIVI_CAS_Drv_FreeNITReq(void)
{
  u8 uIndex = 0;

  for(uIndex = 0; uIndex < DIVI_FILTER_MAX_NUM; uIndex ++)
  {
   
    if(DIVI_USED == s_tFilter[uIndex].m_FilterStatus &&
              0x40 == s_tFilter[uIndex].m_diviReqId)
    {
      divi_filter_free(&s_tFilter[uIndex]);
      memset((void *)&s_tFilter[uIndex], 0, sizeof(divi_filter_struct_t));
    }
  }

  return;
}

/*!
  Set network id

  \param[in] network_id.
  */
void cas_divi_set_network_id(UINT16 network_id)
{
  cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

  CAS_DIVI_PRINTF("[divi], cas_divi_set_network_id = 0x%x \n",network_id);
  //xsm_pid_lock();
  p_priv->network_id = network_id;
  //xsm_pid_unlock();
}


void *DIVI_OSPMalloc(UINT32 numberOfBytes)
{
  return (void*)mtos_malloc(numberOfBytes);
}

INT32 DIVI_OSPFree(void * ptrToSegment)
{
  mtos_free(ptrToSegment);
  return SUCCESS;
}



/*提 供 者： 机顶盒

输入参数： event 为消息内容1: 为OSD消息；4为邮件消息
				2,3 大画面的消息提示:param1 为消息提示的内容 */
void  DIVI_EventHandle( UINT32 event,UINT32 param1,UINT32 param2,UINT32 param3)
{
    cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

    OS_PRINTF("DIVI_EventHandle event = %d, param1 = %d, param2 = %d, param3 = %d\n",
              event,param1,param2,param3);

    switch(event)
    {
      case 0x01:
      {
        //OSD短消息
        CAS_DIVI_PRINTF(" message_len=%d \n",param2);
        CAS_DIVI_PRINTF("[divi]osd message:%s \n ",(char *)param1);
        CAS_DIVI_PRINTF(" param3=0x%x \n",param3);

        memset(&divi_OsdMsg, 0, sizeof(divi_OsdMsg));
        memcpy(divi_OsdMsg.data,(UINT8 *)param1,param2);


        CAS_DIVI_PRINTF("[divi] OsdMsg.data:%s \n\n ",divi_OsdMsg.data);

        set_event(p_priv, CAS_C_SHOW_OSD);
        break;
      }
      case 0x02:
      {
        //销毁提示区域
        event = CAS_S_CLEAR_DISPLAY;
        break;
      }
      case 0x03:
      {
        //节目未授权
        event = CAS_E_PROG_UNAUTH;//CAS_C_ENTITLE_CLOSED
        OS_PRINTF("[DIVI_EventHandle]:CAS_E_PROG_UNAUTH  state[%d]\n",DIVI_GetProductOneIs());
        break;
      }
      case 0x04:
      {
        //表示有邮件
        CAS_DIVI_PRINTF("[divi],email_id:%d \n",param1);
        set_event(p_priv, CAS_C_SHOW_NEW_EMAIL);

        break;
      }
      case 0x05:
      {
        if(0x00 == param1)
        {
          //销毁提示区域
          event = CAS_S_CLEAR_DISPLAY;
        }
        else if(0x01 == param1)
        {
          //条件限播
          //event = CAS_E_CONDITION_DISPLAY;
          //区域不匹配
          event = CAS_C_CARD_ZONE_INVALID;
        }
        break;
      }
      case 0x06:
      {
        switch(param1)
        {
          case 0x00://初始化安装，卡不可使用
           event =  CAS_E_CARD_INIT_INSTALL;
           break;
          case 0x01://正在安装，卡等待激活
           event = CAS_E_CARD_INSTALLING;
           break;
          case 0x02://销毁提示区域
           event = CAS_S_CLEAR_DISPLAY;
           break;
          case 0x03://欠费停用
           event = CAS_E_CADR_NO_MONEY;
           break;
          case 0x04://维修停用
           event = CAS_E_CARD_REPAIR;
           break;
          case 0x05://申请报停停用
           event = CAS_E_CARD_STOP;
           break;
          case 0x09://注销状态
           event = CAS_E_CARD_LOGOUT;
           break;
          default:
           break;
        }
        break;
      }
      case 0x07:
      {
        if(0x00 == param1)
        {
          //销毁提示区域
          event = CAS_S_CLEAR_DISPLAY;
        }
        else if(0x01 == param1)
        {
          //不在工作时段内
          event = CAS_E_CARD_OUT_WORK_TIME;
        }
        break;
      }
      case 0x08:
      {
        if(0x00 == param1)
        {
          //销毁提示区域
          event = CAS_S_CLEAR_DISPLAY;

        }
        else if(0x01 == param1)
        {
          //当前节目级别低于家长控制级别
          event = CAS_E_PROG_LEVEL_LOWER;
        }
        break;
      }
      case 0x09:
      {
        if(0x00 == param1)
        {
          //销毁提示区域
          event = CAS_S_CLEAR_DISPLAY;
        }
        else if(0x03 == param1)
        {
          //卡内金额不足，不能观看该节目
          event = CAS_E_CARD_MONEY_LACK;
        }
        else if(0x05 == param1)
        {
          //当前节目ID为XXX,价格为XXX,是否观看?
          //【OK】确认 【cancel】取消
		  event = CAS_C_IPP_NOTIFY;
        }
        break;
      }
      case 0x0a:
      {
        if(0x00 == param1)
        {
          //销毁提示区域
         event = CAS_S_CLEAR_DISPLAY;
        }
        else if(0x01 == param1)
        {
          //子卡可临时激活
          event = CAS_E_CADR_TEMP_ACT;
        }
        else if(0x02 == param1)
        {
          //子卡已经过期，请激活
          event =  CAS_E_CARD_EXPIRED;
        }
        break;
      }
      case 0x0b:
      {
        //机卡配对错误
        event = CAS_E_CARD_PARTNER_FAILED;
        break;
      }
      case 0x0c:
      {
        if(0x00 == param1)
        {
          //销毁提示区域
          event = CAS_S_CLEAR_DISPLAY;
        }
        else if(0x01 == param1)
        {
          //请插母卡
          event = CAS_S_MOTHER_CARD_REGU_INSERT;
        }
        else if(0x02 == param1)
        {
          event = CAS_S_MOTHER_CARD_INVALID;//无效母卡
        }
         else if(0x03 == param1)
        {
          //请插子卡
          event = CAS_S_SON_CARD_REGU_INSERT;
        }
        else if(0x04 == param1)
        {
          event = CAS_S_SON_CARD_INVALID;//无效子卡
        }
         else if(0x05 == param1)
        {
          event = CAS_S_MOTHER_SON_CARD_NOT_PAIR;//子母卡不匹配
        }
        else if(0x06 == param1)
        {
          event = CAS_S_MOTHER_CARD_COMM_FAIL;//母卡通信失败，请检查母卡
        }
        else if(0x07 == param1)
        {
          event = CAS_S_SON_CARD_COMM_FAIL;//子卡通讯失败，请检查子卡
        }
        else if(0x08 == param1)
        {
          event = CAS_S_CARD_NOT_IN_ACTTIME;//不在激活时间段内
        }
        else if(0x09 == param1)
        {
          event = CAS_S_CARD_INSERT_NEXT_CARD;//激活成功，请插入下一张卡
        }
        else if(0x10 == param1)
        {
          event = CAS_S_CARD_STOP_ACTIVE;//表示已经被用户干预，终止了激活子卡操作
        }
		
		send_message_ca_feed(event);
		return ;
      }
      case 0x0d:
      {
        //无效卡
        event = CAS_E_ILLEGAL_CARD;
        break;
      }
      case 0x0e:
      {
        //未插卡
        event = CAS_S_CARD_REMOVE;
        break;
      }
      default:
        break;
    }

    cas_send_event(p_priv->slot, CAS_ID_DIVI, event, 0);

}

/*提 供 者： 机顶盒。
使 用 者： CA模块。
功能描述： 创建任务
输入参数：
		name[]， 4个字节的任务名称。
		stackSize， 任务所使用的堆栈的大小。
		entryPoint， 任务的入口地址。
		priority， 任务的优先级。
		arg1， 传递给任务的第一个参数。
		arg2， 传递给任务的第二个参数。 taskId， 任务的ID。
输出参数： 无。
返 回 值：
		SUCCESS：成功；
		FAILURE： 发生错误。*/
INT32 DIVI_OSPSCTaskCreate( char name[],
                            UINT32 stackSize,
                            void (*entryPoint)(void*),
                            INT32 priority,
                            UINT32 arg1,
                            UINT32 arg2,
                            UINT32 * taskId)
{
  INT32 err = FAILTURE;

  UINT32 arg[2] = {0};
  UINT32 *p_stack = NULL;
  UINT8   index = 0;

  CAS_DIVI_PRINTF("[divi],DIVI_OSPSCTaskCreate \n");
  if(NULL == name || NULL == entryPoint)
  {
           MT_ASSERT(0);

   CAS_DIVI_PRINTF("DIVI_OSPSCTaskCreate parameter error ! \n");
   return FAILTURE;
  }

  CAS_DIVI_PRINTF("name:%s, stacksize:%d, priority:%d, arg1:%d, arg2:%d \n",
                                                            name,stackSize,priority,arg1,arg2);

  for(index = 0; index < DIVI_CAS_TASK_NUM; index++)
  {
    if(DIVI_FREE == s_task_record[index].m_TaskStatus)
    {
      s_task_record[index].m_TaskStatus = DIVI_USED;
      break;
    }
  }

  if(index >= DIVI_CAS_TASK_NUM)
  {
    MT_ASSERT(0);
    CAS_DIVI_PRINTF("DIVI_OSPSCTaskCreate too many task or invalid task priority!\n");
    return FAILTURE;
  }

  priority = s_task_record[index].m_uTaskPrio;
  arg[0] = arg1;
  arg[1] = arg2;
  p_stack = (UINT32 *)mtos_malloc(DIVI_CAS_TASK_STKSIZE);
  MT_ASSERT(p_stack != NULL);
  
  err = mtos_task_create((u8 *)name,
                          (void (*)(void *))entryPoint,
                          (void *)arg,
                          priority,
                          p_stack,
                          DIVI_CAS_TASK_STKSIZE);

  if(!err)
  {
    CAS_DIVI_PRINTF("DIVI_OSPSCTaskCreate error = 0x%08x!\n", err);
    s_task_record[index].m_TaskStatus = DIVI_FREE;
    MT_ASSERT(0);
    return FAILTURE;
  }

  s_task_record[index].m_uTaskId = *taskId;
  s_task_record[index].m_TaskStatus = DIVI_USED;

  CAS_DIVI_PRINTF("DIVI_OSPSCTaskCreate success Name[%s]  ",name);
  CAS_DIVI_PRINTF("Priority[%d] \n",priority);

  return SUCCESS;
}

/*提 供 者： 机顶盒。
使 用 者： CA模块。
功能描述： 创建任务
输入参数：
		name[]， 4个字节的任务名称。
		stackSize， 任务所使用的堆栈的大小。
		entryPoint， 任务的入口地址。
		priority， 任务的优先级。
		arg1， 传递给任务的第一个参数。
		arg2， 传递给任务的第二个参数。 taskId， 任务的ID。
输出参数： 无。
返 回 值：
		SUCCESS：成功；
		FAILURE： 发生错误。*/
INT32 DIVI_OSPCaCoreTaskCreate( char name[],
                            UINT32 stackSize,
                            void (*entryPoint)(void*),
                            INT32 priority,
                            UINT32 arg1,
                            UINT32 arg2,
                            UINT32 * taskId)
{
  INT32 err = FAILTURE;

  UINT32 arg[2] = {0};
  UINT32 *p_stack = NULL;
  UINT8   index = 0;

  CAS_DIVI_PRINTF("[divi],DIVI_OSPCaCoreTaskCreate \n");
  if(NULL == name || NULL == entryPoint)
  {
     CAS_DIVI_PRINTF("DIVI_OSPCaCoreTaskCreate parameter error ! \n");
     MT_ASSERT(0);
     return FAILTURE;
  }

  CAS_DIVI_PRINTF("name:%s, stacksize:%d, priority:%d, arg1:%d, arg2:%d \n",
                                                            name,stackSize,priority,arg1,arg2);

  for(index = 0; index < DIVI_CAS_TASK_NUM; index++)
  {
    if(DIVI_FREE == s_task_record[index].m_TaskStatus)
    {
      s_task_record[index].m_TaskStatus = DIVI_USED;
      break;
    }
  }

  if(index >= DIVI_CAS_TASK_NUM)
  {
    CAS_DIVI_PRINTF("DIVI_OSPCaCoreTaskCreate too many task or invalid task priority!\n");
	MT_ASSERT(0);
    return FAILTURE;
  }

  priority = s_task_record[index].m_uTaskPrio;
  arg[0] = arg1;
  arg[1] = arg2;
  p_stack = (UINT32 *)mtos_malloc(DIVI_CAS_TASK_STKSIZE);
  MT_ASSERT(p_stack != NULL);

  err = mtos_task_create((u8 *)name,
                          (void (*)(void *))entryPoint,
                          (void *)arg,
                          priority,
                          p_stack,
                          DIVI_CAS_TASK_STKSIZE);

  if(!err)
  {
    CAS_DIVI_PRINTF("DIVI_OSPCaCoreTaskCreate error = 0x%08x!\n", err);
    s_task_record[index].m_TaskStatus = DIVI_FREE;
    return FAILTURE;
  }

  s_task_record[index].m_uTaskId = *taskId;
  s_task_record[index].m_TaskStatus = DIVI_USED;

  CAS_DIVI_PRINTF("DIVI_OSPCaCoreTaskCreate success Name[%s]  ",name);
  CAS_DIVI_PRINTF("Priority[%d] \n",priority);

  return SUCCESS;
}

/*提 供 者： 机顶盒。
使 用 者： CA模块。
功能描述： 创建任务
输入参数：
		name[]， 4个字节的任务名称。
		stackSize， 任务所使用的堆栈的大小。
		entryPoint， 任务的入口地址。
		priority， 任务的优先级。
		arg1， 传递给任务的第一个参数。
		arg2， 传递给任务的第二个参数。 taskId， 任务的ID。
输出参数： 无。
返 回 值：
		SUCCESS：成功；
		FAILURE： 发生错误。*/
INT32 DIVI_OSPOSDTaskCreate( char name[],
                            UINT32 stackSize,
                            void (*entryPoint)(void*),
                            INT32 priority,
                            UINT32 arg1,
                            UINT32 arg2,
                            UINT32 * taskId)
{
  INT32 err = FAILTURE;

  UINT32 arg[2] = {0};
  UINT32 *p_stack = NULL;
  UINT8   index = 0;

  CAS_DIVI_PRINTF("[divi],DIVI_OSPOSDTaskCreate \n");
  if(NULL == name || NULL == entryPoint)
  {
     CAS_DIVI_PRINTF("DIVI_OSPOSDTaskCreate parameter error ! \n");
     MT_ASSERT(0);
     return FAILTURE;
  }

  CAS_DIVI_PRINTF("name:%s, stacksize:%d, priority:%d, arg1:%d, arg2:%d \n",
                                                            name,stackSize,priority,arg1,arg2);

  for(index = 0; index < DIVI_CAS_TASK_NUM; index++)
  {
    if(DIVI_FREE == s_task_record[index].m_TaskStatus)
    {
      s_task_record[index].m_TaskStatus = DIVI_USED;
      break;
    }
  }

  if(index >= DIVI_CAS_TASK_NUM)
  {
    CAS_DIVI_PRINTF("DIVI_OSPOSDTaskCreate too many task or invalid task priority!\n");
	MT_ASSERT(0);
    return FAILTURE;
  }

  priority = s_task_record[index].m_uTaskPrio;
  arg[0] = arg1;
  arg[1] = arg2;
  p_stack = (UINT32 *)mtos_malloc(DIVI_CAS_TASK_STKSIZE);
  MT_ASSERT(p_stack != NULL);

  err = mtos_task_create((u8 *)name,
                          (void (*)(void *))entryPoint,
                          (void *)arg,
                          priority,
                          p_stack,
                          DIVI_CAS_TASK_STKSIZE);

  if(!err)
  {
    CAS_DIVI_PRINTF("DIVI_OSPOSDTaskCreate error = 0x%08x!\n", err);
    s_task_record[index].m_TaskStatus = DIVI_FREE;
    return FAILTURE;
  }

  s_task_record[index].m_uTaskId = *taskId;
  s_task_record[index].m_TaskStatus = DIVI_USED;

  CAS_DIVI_PRINTF("DIVI_OSPOSDTaskCreate success Name[%s]  ",name);
  CAS_DIVI_PRINTF("Priority[%d] \n",priority);

  return SUCCESS;
}

/*提 供 者： 机顶盒。
使 用 者： CA模块。
功能描述： 创建任务
输入参数：
		name[]， 4个字节的任务名称。
		stackSize， 任务所使用的堆栈的大小。
		entryPoint， 任务的入口地址。
		priority， 任务的优先级。
		arg1， 传递给任务的第一个参数。
		arg2， 传递给任务的第二个参数。 taskId， 任务的ID。
输出参数： 无。
返 回 值：
		SUCCESS：成功；
		FAILURE： 发生错误。*/
INT32 DIVI_OSPUserTaskCreate( char name[],
                            UINT32 stackSize,
                            void (*entryPoint)(void*),
                            INT32 priority,
                            UINT32 arg1,
                            UINT32 arg2,
                            UINT32 * taskId)
{
  INT32 err = FAILTURE;

  UINT32 arg[2] = {0};
  UINT32 *p_stack = NULL;
  UINT8   index = 0;

  CAS_DIVI_PRINTF("[divi],DIVI_OSPUserTaskCreate \n");
  if(NULL == name || NULL == entryPoint)
  {
     CAS_DIVI_PRINTF("DIVI_OSPUserTaskCreate parameter error ! \n");
     MT_ASSERT(0);
     return FAILTURE;
  }

  CAS_DIVI_PRINTF("name:%s, stacksize:%d, priority:%d, arg1:%d, arg2:%d \n",
                                                            name,stackSize,priority,arg1,arg2);

  for(index = 0; index < DIVI_CAS_TASK_NUM; index++)
  {
    if(DIVI_FREE == s_task_record[index].m_TaskStatus)
    {
      s_task_record[index].m_TaskStatus = DIVI_USED;
      break;
    }
  }

  if(index >= DIVI_CAS_TASK_NUM)
  {
    CAS_DIVI_PRINTF("DIVI_OSPUserTaskCreate too many task or invalid task priority!\n");
	MT_ASSERT(0);
    return FAILTURE;
  }

  priority = s_task_record[index].m_uTaskPrio;
  arg[0] = arg1;
  arg[1] = arg2;
  p_stack = (UINT32 *)mtos_malloc(DIVI_CAS_TASK_STKSIZE);
  MT_ASSERT(p_stack != NULL);

  err = mtos_task_create((u8 *)name,
                          (void (*)(void *))entryPoint,
                          (void *)arg,
                          priority,
                          p_stack,
                          DIVI_CAS_TASK_STKSIZE);

  if(!err)
  {
    CAS_DIVI_PRINTF("DIVI_OSPUserTaskCreate error = 0x%08x!\n", err);
    s_task_record[index].m_TaskStatus = DIVI_FREE;
    return FAILTURE;
  }

  s_task_record[index].m_uTaskId = *taskId;
  s_task_record[index].m_TaskStatus = DIVI_USED;

  CAS_DIVI_PRINTF("DIVI_OSPUserTaskCreate success Name[%s]  ",name);
  CAS_DIVI_PRINTF("Priority[%d] \n",priority);

  return SUCCESS;
}

INT32 DIVI_OSPTaskTemporarySleep(UINT32 milliSecsToWait)
{
  //CAS_DIVI_PRINTF("call DIVI_OSPTaskTemporarySleep !\n");
  mtos_task_sleep(milliSecsToWait);
  return SUCCESS;
}

/*提 供 者： 机顶盒。
使 用 者： CA模块。
功能描述： 创建一个消息队列。
输入参数：
		name[]， 4个字节的队列名称。
		maxQueueLength，消息队列中可以存放的消息的数量。当消息队列中该数量达到 该数量时，再往该消息队列发消息将会失败。
		taskWaitMode:可以不管
		queueId， 消息队列的ID。
输出参数： 无。
返 回 值：
		SUCCESS：成功；
		FAILURE： 发生错误*/
INT32 DIVI_OSPQueueCreate(char name[],UINT32 maxQueueLength,UINT32 taskWaitMode,UINT32 * queueId)
{
  INT32 err = FAILTURE;
  UINT32 message_id = 0;
  void *p_quenue_buf = NULL;
  UINT32 quenue_buf_size = 0;

  CAS_DIVI_PRINTF("[divi],DIVI_OSPQueueCreate \n");

  if(NULL == queueId || NULL == name)
  {
     CAS_DIVI_PRINTF("DIVI_OSPQueueCreate, parameter error ! \n");
     MT_ASSERT(0);
  }

  message_id = mtos_messageq_create(DIVI_CAS_QUEUE_MAX_DEPTH,(UINT8 *)name);
  MT_ASSERT(message_id != INVALID_MSGQ_ID);

  quenue_buf_size = sizeof(DIVI_QUEUE_MESSAGE) *DIVI_CAS_QUEUE_MAX_DEPTH;
  p_quenue_buf = mtos_malloc(quenue_buf_size);

  MT_ASSERT(p_quenue_buf != NULL);
  memset(p_quenue_buf,0,quenue_buf_size);

  err = mtos_messageq_attach(message_id,p_quenue_buf,
                                                  sizeof(DIVI_QUEUE_MESSAGE),
                                                  DIVI_CAS_QUEUE_MAX_DEPTH);
  if(!err)
  {
     mtos_free(p_quenue_buf);
     CAS_DIVI_PRINTF("[divi], mtos_messageq_attach err ! \n");
     return FAILTURE;
  }

  *queueId = message_id;

  CAS_DIVI_PRINTF("*queueId=%d ,name=%s \n\n",*queueId,name);

  return SUCCESS;
}


/*提 供 者： 机顶盒。
使 用 者： CA模块。
功能描述：
输入参数：
		queueId， 所要取得的消息的ID。
		messsage， 为消息的格式。参照XINSHIMAO_QUEUE_MESSAGE。
		waitMode:目前只用到，等待直到得到消息

		SUCCESS：成功；
		FAILURE： 发生错误。*/
INT32 DIVI_OSPQueueGetMessage(UINT32 queueId,DIVI_QUEUE_MESSAGE * message,
                              UINT32 waitMode,UINT32 milliSecsToWait)
{
  INT32 err = FAILTURE;
  UINT32 message_id = 0;
  os_msg_t msg = {0};
  divi_queue_t *p_message = NULL;

  CAS_DIVI_PRINTF("\n DIVI_OSPQueueGetMessage queueId = %d ,message=%0x%x\n",queueId,message);

  if(NULL == message)
  {
       CAS_DIVI_PRINTF("DIVI_OSPQueueGetMessage, parameter error ! \n");
       MT_ASSERT(0);
  }

  message_id = queueId;


  err = mtos_messageq_receive(message_id,&msg,0);
  if(!err)
  {
       CAS_DIVI_PRINTF("[divi]message_id=%d, receive msg error ! \n",message_id);
       return FAILTURE;
  }

  CAS_DIVI_PRINTF("[divi],free message addr=0x%x \n",msg.para1);
  p_message = (divi_queue_t *)msg.para1;
  memcpy(message,&p_message->message,sizeof(DIVI_QUEUE_MESSAGE));
  memset(p_message,0,sizeof(divi_queue_t));
  p_message = NULL;

  return SUCCESS;
}


/*提 供 者： 机顶盒。
使 用 者： CA模块。
功能描述： 发送消息到消息队列。
输入参数：
		queueId， 消息队列ID。
		message， 要发送的消息。其格式见XINSHIMAO_QUEUE_MESSAGE结构。
输出参数： 无。
返 回 值：
		SUCCESS：成功；
		FAILURE： 发生错误。*/
INT32 DIVI_OSPQueueSendMessage(UINT32 queueId,DIVI_QUEUE_MESSAGE * message)
{
  INT32 err = FAILTURE;
  UINT32 message_id = 0;
  os_msg_t msg = {0};
  divi_queue_t *p_message = NULL;

  CAS_DIVI_PRINTF("[divi],DIVI_OSPQueueSendMessage queueid = %d,message=0x%x \n",
                      queueId,message);

  if(NULL == message)
  {
       CAS_DIVI_PRINTF("[divi]DIVI_OSPQueueSendMessage, parameter error ! \n");
       MT_ASSERT(0);
  }

  message_id = queueId;
  p_message = divi_free_queue_get(message_id);
  if(p_message == NULL)
  {
      CAS_DIVI_PRINTF("[divi], message queuen full !!!!!");
      return FAILTURE;
  }

  memcpy(&p_message->message,message,sizeof(DIVI_QUEUE_MESSAGE));
  CAS_DIVI_PRINTF("send:0x%08x ",p_message->message.q1stWordOfMsg);
  CAS_DIVI_PRINTF("0x%08x ",p_message->message.q2ndWordOfMsg);
  CAS_DIVI_PRINTF("0x%08x ",p_message->message.q3rdWordOfMsg);
  CAS_DIVI_PRINTF("0x%08x \n",p_message->message.q4thWordOfMsg);

  msg.is_ext = 0;
  msg.para1 = (UINT32)p_message;
  msg.para2 = _MSG_SIZE();

  CAS_DIVI_PRINTF("[divi]queueId=%d,msg.para1=0x%x,msg.para2=0x%x\n",
                                                               queueId,msg.para1,msg.para2);


  err = mtos_messageq_send(message_id, &msg);
  if(!err)
  {
       CAS_DIVI_PRINTF("DIVI_OSPQueueSendMessage, send msg error ! \n");
	   return FAILTURE;
  }

  return SUCCESS;
}




INT32 DIVI_OSPSemaphoreCreate(char name [ 4 ],UINT32 initialTokenCount,UINT32 taskWaitMode,UINT32 * semaphoreId)
{
  UINT8 index = 0;
  INT32   err = FAILTURE;

  CAS_DIVI_PRINTF("[divi],DIVI_OSPSemaphoreCreate \n");
  if(NULL == semaphoreId)
  {
       CAS_DIVI_PRINTF("DIVI_OSPSemaphoreCreate parameter error ! \n");
       MT_ASSERT(0);
       return FAILTURE;
  }

  err = divi_free_sem_get(&index);
  if(SUCCESS != err)
  {
       CAS_DIVI_PRINTF("DIVI_OSPSemaphoreCreate don't get free semphore ! \n");
       return FAILTURE;
  }

  err = mtos_sem_create((os_sem_t *)semaphoreId, initialTokenCount);
  if (!err)
  {
      CAS_DIVI_PRINTF("DIVI_OSPSemaphoreCreate error !\n");
      return FAILTURE;
  }

  memcpy(divi_sem_list[index].name,name,4);
  divi_sem_list[index].init_value   = initialTokenCount;
  divi_sem_list[index].wait_mode = taskWaitMode;
  divi_sem_list[index].sem_id      = *semaphoreId;
  divi_sem_list[index].sem_status = DIVI_USED;

  CAS_DIVI_PRINTF("semaphoreId = %d, taskWaitMode = %d\n",*semaphoreId,taskWaitMode);
  return SUCCESS;
}


INT32 DIVI_OSPSemaphoreGetToken(UINT32 semaphoreId,UINT32 waitMode,UINT32 milliSecsToWait)
{
  INT32 err = FAILTURE;
  os_sem_t *p_Semaphore = (os_sem_t *)&semaphoreId;

  CAS_DIVI_PRINTF("[divi],DIVI_OSPSemaphoreGetToken  start\n");
  err = divi_sem_check(semaphoreId);
  if(SUCCESS != err)
  {
      CAS_DIVI_PRINTF("DIVI_OSPSemaphoreGetToken ,check sem error !\n");
      return FAILTURE;
  }

  CAS_DIVI_PRINTF("waitMode = %d, semaphoreId = %d\n",waitMode,semaphoreId);

  if(DIVI_TASK_NOWAIT == waitMode)
  {
       mtos_sem_take(p_Semaphore, 0);
  }
  else if(DIVI_TASK_WAIT == waitMode)
  {
       mtos_sem_take(p_Semaphore, milliSecsToWait);
  }

  CAS_DIVI_PRINTF("[divi],DIVI_OSPSemaphoreGetToken  over\n");
  return SUCCESS;

}

INT32 DIVI_OSPSemaphoreReturnToken(UINT32 semaphoreId)
{
  INT32 err = FAILTURE;
  os_sem_t *p_Semaphore = (os_sem_t *)&semaphoreId;

  CAS_DIVI_PRINTF("[divi],DIVI_OSPSemaphoreReturnToken \n");

  CAS_DIVI_PRINTF("semaphoreId = %d\n",semaphoreId);

  err = divi_sem_check(semaphoreId);
  if(SUCCESS != err)
  {
      CAS_DIVI_PRINTF("DIVI_OSPSemaphoreReturnToken ,check sem error !\n");
      return FAILTURE;
  }

  err = mtos_sem_give((os_sem_t *)p_Semaphore);
  
  if (!err)
  {
     CAS_DIVI_PRINTF("DIVI_OSPSemaphoreReturnToken ,add sem value error !\n");
     return FAILTURE;
  }

  return SUCCESS;
}

UINT32 DBGPrint(const char * fmt,...)
{
#if 0
    ck_va_list p_args = NULL;
    unsigned int  printed_len = 0;
    char    printk_buf[200];
    CAS_DIVI_PRINTF("[divi core]");
    CK_VA_START(p_args, fmt);
    printed_len = ck_vsnprintf(printk_buf, sizeof(printk_buf), (char *)fmt, p_args);

    CK_VA_END(p_args);
    CAS_DIVI_PRINTF(printk_buf);
#endif
    return 1;

}

UINT32 DIVI_GetRandomNumber(void)
{
    UINT32 radam_date = 0;

    CAS_DIVI_PRINTF("[divi],DIVI_GetRandomNumber \n");

    radam_date = rand();

    return radam_date;
}


/*提 供 者： 机顶盒。
使 用 者： CA模块。
功能描述： 获得机顶盒提供给CA模块保存信息的起始地址和空间大小
输入参数：
输出参数：
		lStartAddr 机顶盒分配空间的起始地址
		lSize 机顶盒分配空间的大小
返 回 值：
		TRUE 成功
		FALSE 失败 */
BOOL DIVI_GetBuffer(UINT32 *lStartAddr,UINT32 * lSize)
{
  cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

  *lSize = p_priv->flash_size;
  *lStartAddr = p_priv->flash_start_adr;

  CAS_DIVI_PRINTF("[divi] DIVI_GetBuffer %x   %x\n", *lStartAddr, *lSize);

  return TRUE;
}

BOOL DIVI_Get_STBToCardSuit_Buffer(UINT32 * lStartAddr,UINT32 * lSize)
{

  cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

  *lSize = p_priv->stb_card_pair_size;
  *lStartAddr = p_priv->stb_card_pair_start_adr;

  CAS_DIVI_PRINTF("[divi] DIVI_Get_STBToCardSuit_Buffer %x   %x\n", *lStartAddr, *lSize);

  return TRUE;
}



/*提 供 者： 机顶盒。
使 用 者： CA模块。
功能描述： 从机顶盒分配空间的指定起始地址读指定长度的数据。
输入参数：
		lStartAddr： 要读取数据的存储空间的地址。
		pbData： 被读取数据的存放地址指针。
		nLen： 要读的数据的长度
输出参数：
		pbData： 被读出的数据。
返 回 值：
		返回实际读到的字节数*/
BOOL DIVI_ReadBuffer(UINT32 lStartAddr,UINT8 *pData,INT32 nLen)
{
	//BOOL bRet = FALSE;
	//UINT32 offset = 0;
	UINT32 size = (UINT32)nLen;
	cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

	CAS_DIVI_PRINTF("[divi],DIVI_ReadBuffer start\n");
	CAS_DIVI_PRINTF(" DIVI_ReadBuffer:0x%x   0x%x   0x%x\n", lStartAddr, pData, size);
#if 1
	p_priv->nvram_read((UINT32)lStartAddr, pData, &size);
#else
	offset = p_priv->flash_start_adr + (UINT32)lStartAddr;
	bRet = p_priv->nvram_read(offset, pData, &size);
#endif
	CAS_DIVI_PRINTF("[divi],DIVI_ReadBuffer OK\n");
	return TRUE;
}



/*提 供 者： 机顶盒。
使 用 者： CA模块。
功能描述： 向机顶盒的存储空间写信息
输入参数：
		lStartAddr： 要写的存储空间的目标地址。
		pData： 要写的数据
		nLen： 要写的数据的长度
输出参数： 无。
返 回 值：
		TRUE：成功
		FALSE： 失败。*/


BOOL DIVI_WriteBuffer(const UINT32 lStartAddr,const UINT8 *pData,INT32 nLen)
{
	//BOOL bRet = FALSE;
	//UINT32 offset = 0;
	cas_divi_priv_t *p_priv =  (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

	CAS_DIVI_PRINTF("[divi],DIVI_WriteBuffer start\n");
	CAS_DIVI_PRINTF(" [divi] DIVI_WriteBuffer:0x%x	0x%x  0x%x\n", lStartAddr, pData, nLen);

#if 1
	p_priv->nvram_write((UINT32)lStartAddr, (UINT8 *)pData, nLen);
#else
	offset = p_priv->flash_start_adr + lStartAddr;
	bRet = p_priv->nvram_write(offset, (UINT8 *)pData, nLen);
#endif
	CAS_DIVI_PRINTF("[divi],DIVI_WriteBuffer OK\n");

	return TRUE;
}




/*提 供 者： 机顶盒。
使 用 者： CA模块。
功能描述： 启动过滤器去接收CA的数据。
输入参数：
		iRequestID     ,     为如下几个值之一
						CA_STB_FILTER_1,
						CA_STB_FILTER_2
						CA_STB_FILTER_3
						CA_STB_FILTER_4
		pbFilterMatch1， 如果是过滤Ecm数据，则该参数为Ecm filter的数据；
						 如果是过滤Emm数据，则该参数为Emm filter1的数据；
		pbFilterMask1，  如果是过滤Ecm数据，则该参数为Ecm filter的mask；
						 如果是过滤Emm数据，则该参数为Emm filter1的mask；
		bLen， filter的长度。
		wPid， 通道的PID值。
		bWaitTime， 通道过期时间，即从多长时间内通道无法接收到与
	       if ( wPid <0 && wPid >0x1fff ) 提示机顶盒DEMMUX需要释放过滤器空间，EMM可以不释放，ECM必须释放
返 回 值：
		SUCCESS：成功，
		FAILURE： 失败。 */
INT32 DIVI_TableStart(UINT8 iRequestID,
                      const UINT8 * pbFilterMatch1,
                      const UINT8 * pbFilterMask1,
                      UINT8 bLen,
                      UINT16 wPid,
                      UINT8 bWaitTime)

{
    UINT32 err = FAILTURE;
    UINT8   index = 0;
    dmx_filter_setting_t p_param = {0};
    dmx_slot_setting_t p_slot = {0} ;
    dmx_device_t *p_dev = NULL;
    UINT8 filter_byte_num = 0;


    CAS_DIVI_PRINTF("[divi],DIVI_TableStart ,wPid= 0x%x \n",wPid);

    CAS_DIVI_PRINTF("[divi], iRequestID=0x%x ",iRequestID);
    CAS_DIVI_PRINTF("bLen=%d  ",bLen);
    CAS_DIVI_PRINTF("wPid= 0x%x  ",wPid);
    CAS_DIVI_PRINTF("bWaitTime=%d \n",bWaitTime);

    if(pbFilterMatch1 == NULL || pbFilterMask1 == NULL || bLen == 0 || bLen > DIVI_FILTER_DEPTH_SIZE)
    {
      CAS_DIVI_PRINTF("Input Params Err pbFilterMatch1 = 0x%x, pbFilterMask1 = 0x%x, bLen = %d\n",
              pbFilterMatch1,pbFilterMask1,bLen);
      return FAILTURE;
    }
    
    for(index = 0; index < bLen; index ++)
    {
         CAS_DIVI_PRINTF("FilterData[%d]:0X%X  ",index,pbFilterMatch1[index]);
         CAS_DIVI_PRINTF("FilterMask[%d]:0X%X \n",index,pbFilterMask1[index]);
    }


    if(DIVI_CA_CARD_INSERT != divi_get_card_status())
    {

      CAS_DIVI_PRINTF("[divi], card not insert ! \n");

      return FAILTURE;
    }

    divi_ca_lock();

    if(CA_STB_FILTER_4 == iRequestID || 0xffff == wPid)//释放之前的filter资源
    {
      CAS_DIVI_PRINTF("[divi], wPID=%d slot will free ! \n",wPid);

       for(index = 0; index < DIVI_FILTER_MAX_NUM; index++)
       {
            if(DIVI_USED == s_tFilter[index].m_FilterStatus &&
              iRequestID == s_tFilter[index].m_diviReqId)
            {
               divi_filter_free(&s_tFilter[index]);
               memset((void *)&s_tFilter[index], 0, sizeof(divi_filter_struct_t));
            }
       }

      if(0xffff == wPid)
      {
        divi_ca_unlock();
        return SUCCESS;
      }
          
    }



    


    filter_byte_num = bLen;
    #if 0
    if(filter_byte_num > DMX_SECTION_FILTER_SIZE)
    {
           filter_byte_num = DMX_SECTION_FILTER_SIZE;
           CAS_DIVI_PRINTF("[divi], filter byte filter_byte_num=%d \n",filter_byte_num);
    }
    #endif


    for(index = 0; index < DIVI_FILTER_MAX_NUM; index++)
    {
         /*filter EMM packet, CA_STB_FILTER_4 is ECM packet*/
         if(DIVI_USED == s_tFilter[index].m_FilterStatus
            && iRequestID == s_tFilter[index].m_diviReqId
            && CA_STB_FILTER_4 != iRequestID
            && NIT_TABLE_ID != iRequestID)
         {

              if((memcmp(pbFilterMatch1, s_tFilter[index].m_uFilterData, filter_byte_num) == 0)
                  && (memcmp(pbFilterMask1, s_tFilter[index].m_uFilterMask, filter_byte_num) == 0)
                  && (s_tFilter[index].m_PID == wPid))
              {
                  CAS_DIVI_PRINTF("[divi],Emm filter is the same, don't request new filter!\n");
                  divi_ca_unlock();
                  return SUCCESS;
              }
              else
              {
                   CAS_DIVI_PRINTF("[divi],Emm filter is different, cancel old filter !\n");
                   divi_filter_free(&s_tFilter[index]);
                   memset((void *)&s_tFilter[index], 0, sizeof(divi_filter_struct_t));
              }
         }

    }

    /*get free filter */
    for(index = 0; index < DIVI_FILTER_MAX_NUM; index ++)
    {
      if(DIVI_FREE == s_tFilter[index].m_FilterStatus)
      {
        break;
      }
    }

    /*can't get free filter*/
    if(index >= DIVI_FILTER_MAX_NUM)
    {
        CAS_DIVI_PRINTF("[divi],Filter array is full!\n");
        divi_ca_unlock();
        return FAILTURE;
    }

      p_dev = (dmx_device_t *)dev_find_identifier(NULL
            , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
      MT_ASSERT(NULL != p_dev->p_base);

      p_slot.in = DMX_INPUT_EXTERN0;
      p_slot.pid = wPid;
      p_slot.type = DMX_CH_TYPE_SECTION;

      err = dmx_si_chan_open(p_dev,&p_slot,&s_tFilter[index].m_dwReqHandle);
      if (0 != err)
      {
          CAS_DIVI_PRINTF("\r\n [divi],%s : %d : error_code = 0x%08x \n", __FUNCTION__, __LINE__, err);
          divi_ca_unlock();
          return FAILTURE;
      }
      CAS_DIVI_PRINTF("[divi],index=%d, wPid=0x%X, m_dwReqHandle=0x%X \n",
                                                 index,wPid,s_tFilter[index].m_dwReqHandle);

      s_tFilter[index].p_buf = s_tFilterBuf[index];
      err = dmx_si_chan_set_buffer(p_dev,s_tFilter[index].m_dwReqHandle,s_tFilter[index].p_buf,
                                                        DIVI_FILTER_BUFFER_SIZE);
      if(0 != err)
      {
          CAS_DIVI_PRINTF("\r\n[divi],set filter buffer failed!\n");
          divi_ca_unlock();
          return FAILTURE;
      }

      p_param.continuous = 1;
      p_param.en_crc = FALSE;//DISABLE FOR CA DATA

       //memcpy(p_param.value, pbFilterMatch1, 12);
       //memcpy(p_param.mask, pbFilterMask1, 12);

       #if 0
       p_param.value[0] = pbFilterMatch1[0];
       p_param.value[1] = pbFilterMatch1[3];
       p_param.value[2] = pbFilterMatch1[4];
       p_param.value[3] = pbFilterMatch1[5];
       p_param.value[4] = pbFilterMatch1[6];
       p_param.value[5] = pbFilterMatch1[7];
       p_param.value[6] = pbFilterMatch1[8];
       p_param.value[7] = pbFilterMatch1[9];
       p_param.value[8] = pbFilterMatch1[10];
       p_param.value[9] = pbFilterMatch1[11];
       p_param.value[10] = pbFilterMatch1[12];



       p_param.mask[0] = pbFilterMask1[0];
       p_param.mask[1] = pbFilterMask1[3];
       p_param.mask[2] = pbFilterMask1[4];
       p_param.mask[3] = pbFilterMask1[5];
       p_param.mask[4] = pbFilterMask1[6];
       p_param.mask[5] = pbFilterMask1[7];
       p_param.mask[6] = pbFilterMask1[8];
       p_param.mask[7] = pbFilterMask1[9];
       p_param.mask[8] = pbFilterMask1[10];
       p_param.mask[9] = pbFilterMask1[11];
       p_param.mask[10] = pbFilterMask1[12];
      #else
       p_param.value[0] = pbFilterMatch1[0];
       p_param.value[1] = pbFilterMatch1[1];
       p_param.value[2] = pbFilterMatch1[2];
       p_param.value[3] = pbFilterMatch1[3];
       p_param.value[4] = pbFilterMatch1[4];
       p_param.value[5] = pbFilterMatch1[5];
       p_param.value[6] = pbFilterMatch1[6];
       p_param.value[7] = pbFilterMatch1[7];
       p_param.value[8] = pbFilterMatch1[8];
       p_param.value[9] = pbFilterMatch1[9];
       p_param.value[10] = pbFilterMatch1[10];
       p_param.value[11] = pbFilterMatch1[11];
       //p_param.value[12] = pbFilterMatch1[12];


       p_param.mask[0] = pbFilterMask1[0];
       p_param.mask[1] = pbFilterMask1[1];
       p_param.mask[2] = pbFilterMask1[2];
       p_param.mask[3] = pbFilterMask1[3];
       p_param.mask[4] = pbFilterMask1[4];
       p_param.mask[5] = pbFilterMask1[5];
       p_param.mask[6] = pbFilterMask1[6];
       p_param.mask[7] = pbFilterMask1[7];
       p_param.mask[8] = pbFilterMask1[8];
       p_param.mask[9] = pbFilterMask1[9];
       p_param.mask[10] = pbFilterMask1[10];
       p_param.mask[11] = pbFilterMask1[11];
       //p_param.mask[12] = pbFilterMask1[12];

       
      #endif
       err = dmx_si_chan_set_filter(p_dev,s_tFilter[index].m_dwReqHandle,&p_param);
       if(0 != err)
       {
           CAS_DIVI_PRINTF("\n[divi],set filter failed!\n");
           divi_ca_unlock();
           return FAILTURE;
       }

       err = dmx_chan_start(p_dev,s_tFilter[index].m_dwReqHandle);
       if(0 != err)
       {
            CAS_DIVI_PRINTF("\n[divi],start channel  failed!\n");
            divi_ca_unlock();
            return FAILTURE;
       }

       if(0 == bWaitTime || CA_STB_FILTER_4 != iRequestID)
       {
            s_tFilter[index].m_timeout = 0xffffffff;
       }
       else if (CA_STB_FILTER_4 == iRequestID)
       {
            s_tFilter[index].m_timeout = 2 * 100;
       }
	   else
	   {
	        s_tFilter[index].m_timeout = bWaitTime * 100;
	   }

        s_tFilter[index].m_FilterStatus = DIVI_USED;
        s_tFilter[index].m_diviReqId = iRequestID;
        s_tFilter[index].m_PID = wPid;
        memcpy(s_tFilter[index].m_uFilterData, pbFilterMatch1, filter_byte_num);
        memcpy(s_tFilter[index].m_uFilterMask, pbFilterMask1, filter_byte_num);
        s_tFilter[index].m_uFilterLen = filter_byte_num;
        s_tFilter[index].m_dwStartMS = mtos_ticks_get();
        CAS_DIVI_PRINTF("\n [DIVI], FILTER setting ok! \n ");

        divi_ca_unlock();

        return SUCCESS;
}

void DIVI_ClearCW(UINT16 wEcmPid)
{
    UINT16 v_pid = 0;
    UINT16 a_pid = 0;
    dmx_device_t *p_dev = NULL;
    INT32 err = FAILTURE;
    UINT8 szOddKey[8] = {0};
    UINT8 szEvenKey[8] = {0};

    CAS_DIVI_PRINTF("[divi],DIVI_SetCW \n");

    p_dev = (dmx_device_t *)dev_find_identifier(NULL
    , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    v_pid = cas_divi_get_v_pid();
    a_pid = cas_divi_get_a_pid();

    if(v_pid == 0 || a_pid == 0)
    {
         return;
    }

    CAS_DIVI_PRINTF("\n set key v_pid= 0x%x  a_pid = 0x%x\n", v_pid, a_pid);
    err = dmx_get_chanid_bypid(p_dev, v_pid, &v_channel);
    if(err != SUCCESS)
    {
        CAS_DIVI_PRINTF("v_pid=0x%x \n",v_pid);
        return;
    }

    err = dmx_get_chanid_bypid(p_dev, a_pid, &a_channel);
    if(err != SUCCESS)
    {
        CAS_DIVI_PRINTF("a_pid=0x%x ,dmx_get_chanid_bypid error !\n",a_pid);
         return;
    }
    CAS_DIVI_PRINTF("set key v_channel= %x  a_channel = %x\n", v_channel, a_channel);

    if(v_channel != 0xffff)
    {
       err = dmx_descrambler_set_odd_keys(p_dev,v_channel,(u8 *)szOddKey,8);
       err = dmx_descrambler_set_even_keys(p_dev,v_channel,(u8 *)szEvenKey,8);
       err = dmx_descrambler_onoff(p_dev,v_channel,FALSE);
	   v_channel = 0xffff;
       if(err != SUCCESS)
       {
            CAS_DIVI_PRINTF("[divi],dmx_descrambler_onoff error ! \n");
            return;
       }
    }

    if (a_channel != 0xffff)
    {
        err = dmx_descrambler_set_odd_keys(p_dev,a_channel,(u8 *)szOddKey,8);
        err = dmx_descrambler_set_even_keys(p_dev,a_channel,(u8 *)szEvenKey,8);
        err = dmx_descrambler_onoff(p_dev,a_channel,FALSE);
		a_channel = 0xffff;
        if(err != SUCCESS)
        {
            return;
        }
    }
}

static BOOL divi_check_cw(u8 * szOddKey, u8 * szEvenKey)
{
	u8 i;

	for(i=1; i<8; i++)
	{
		if(szOddKey[0] != szOddKey[i] || szEvenKey[0] != szEvenKey[i])
			return TRUE;
	}
	return FALSE;
}

/*提 供 者： 机顶盒。
使 用 者： CA模块。
功能描述： 设置解扰通道的控制字。
输入参数：
		wEcmPid， 控制字的所在的ecm包的PID值。
		szOddKey， 奇控制字数据的指针。
		szEvenKey， 偶控制字数据的指针。
		bKeyLen， 控制字长度。
		bReservedFlag, 保留。
*/
void DIVI_SetCW(UINT16 wEcmPid,const UINT8 * szOddKey,const UINT8 * szEvenKey,
	                              UINT8 bKeyLen,UINT8 bReservedFlag)
{
    UINT16 v_pid = 0;
    UINT16 a_pid = 0;
    dmx_device_t *p_dev = NULL;
    INT32 err = FAILTURE;

    if((NULL == szOddKey) || (NULL == szEvenKey))
    {
        CAS_DIVI_PRINTF("[divi],DIVI_SetCW paramter error ! \n");
    }

    CAS_DIVI_PRINTF("[divi],DIVI_SetCW \n");
    CAS_DIVI_PRINTF("OddKey %02x %02x %02x %02x %02x %02x %02x %02x ticks=%d \n"
    , szOddKey[0], szOddKey[1], szOddKey[2], szOddKey[3]
    , szOddKey[4], szOddKey[5], szOddKey[6], szOddKey[7],mtos_ticks_get());
    CAS_DIVI_PRINTF("EvenKey %02x %02x %02x %02x %02x %02x %02x %02x\n"
    , szEvenKey[0], szEvenKey[1], szEvenKey[2], szEvenKey[3]
    , szEvenKey[4], szEvenKey[5], szEvenKey[6], szEvenKey[7]);

    p_dev = (dmx_device_t *)dev_find_identifier(NULL
    , DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);
    MT_ASSERT(NULL != p_dev->p_base);

    if(old_v_channel != 0xffff)
    {
        err = dmx_descrambler_onoff(p_dev,old_v_channel,FALSE);
        old_v_channel = 0xffff;
    }
    if(old_a_channel != 0xffff)
    {
        err = dmx_descrambler_onoff(p_dev,old_a_channel,FALSE);
        old_a_channel = 0xffff;
    }

    v_pid = cas_divi_get_v_pid();
    a_pid = cas_divi_get_a_pid();
    #if 0
    v_pid = 0x201;
    a_pid = 0x294;
    #endif

    if(v_pid == 0 || a_pid == 0)
    {
         return;
    }

    CAS_DIVI_PRINTF("\n set key v_pid= 0x%x  a_pid = 0x%x\n", v_pid, a_pid);
    err = dmx_get_chanid_bypid(p_dev, v_pid, &v_channel);
    if(err != SUCCESS)
    {
        CAS_DIVI_PRINTF("v_pid=0x%x \n",v_pid);
        return;
    }

    err = dmx_get_chanid_bypid(p_dev, a_pid, &a_channel);
    if(err != SUCCESS)
    {
        CAS_DIVI_PRINTF("a_pid=0x%x ,dmx_get_chanid_bypid error !\n",a_pid);
         return;
    }
    CAS_DIVI_PRINTF("set key v_channel= %x  a_channel = %x\n", v_channel, a_channel);

    if(v_channel != 0xffff)
    {
        err = dmx_descrambler_onoff(p_dev,v_channel,TRUE);
       if(err != SUCCESS)
       {
            CAS_DIVI_PRINTF("[divi],dmx_descrambler_onoff error ! \n");
            return;
       }
       err = dmx_descrambler_set_odd_keys(p_dev,v_channel,(u8 *)szOddKey,8);
       err = dmx_descrambler_set_even_keys(p_dev,v_channel,(u8 *)szEvenKey,8);
    }

    if (a_channel != 0xffff)
    {
        err = dmx_descrambler_onoff(p_dev,a_channel,TRUE);
        if(err != SUCCESS)
        {
            return;
        }
        err = dmx_descrambler_set_odd_keys(p_dev,a_channel,(u8 *)szOddKey,8);
        err = dmx_descrambler_set_even_keys(p_dev,a_channel,(u8 *)szEvenKey,8);
    }

	if(CUSTOMER_ID == CUSTOMER_DTMB_CHANGSHA_HHT)
	{
		if((a_channel != 0xffff) || (v_channel != 0xffff))
		{
			static BOOL playState = FALSE;
			cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

			if(divi_check_cw((u8 *)szOddKey, (u8 *)szEvenKey))
			{
				if(!playState)
				{
					playState = TRUE;
					cas_send_event(p_priv->slot, CAS_ID_DIVI, CAS_S_CLEAR_DISPLAY, 0);
					DEBUG(CAS, INFO, "CAS_S_CLEAR_DISPLAY a_channel = %x v_channel = %x\n",a_channel,v_channel);
				}
			}
			else
			{
				playState = FALSE;
			}

			DEBUG(CAS, INFO, "playState = %x\n",playState);
		}
			
	}

    old_v_channel = v_channel;
    old_a_channel = a_channel;
}


UINT8 GxShowPPDatas(UINT8 Value)
{
  #if 0
  CAS_DIVI_PRINTF("%s %d value = %d\n",__FUNCTION__,__LINE__,Value);

  void * p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_VDEC_VSB);


  if(0x00 == Value)//close pp
  {
     vdec_stop(p_video_dev);
  }
  else if(0x01 == Value)//open
  {
    vdec_start(p_video_dev, VIDEO_MPEG, VID_UNBLANK_STABLE);
  }
#endif
  return 0;
}


/*提 供 者： 机顶盒
使 用 者： CA模块
功能描述：获得当前节目的ServiceID
输入参数： 无
输出参数： 无。
返 回 值： 当前节目的ServiceID*/
UINT16 DIVI_GetCurr_ServiceID(void)
{
  UINT16 service_id = 0;

  service_id = cas_divi_get_server_id();
  CAS_DIVI_PRINTF("[divi],DIVI_GetCurr_ServiceID:%d  \n",service_id);

  return service_id;
}


/*提 供 者： 机顶盒
使 用 者： CA模块
功能描述：获得当前的Emm Pid
输入参数： 无
输出参数： 无。
返 回 值： 当前的Emm Pid*/
UINT16 DIVI_GetCurr_EmmID(void)
{
  UINT16 emm_pid = 0;

  emm_pid = cas_divi_get_emm_pid();
  CAS_DIVI_PRINTF("[DIVI],DIVI_GetCurr_EmmID:0x%x \n",emm_pid);

  //emm_pid = 0x20;
  return emm_pid;

}


/*提 供 者： 机顶盒
使 用 者： CA模块
功能描述：获得当前的Ecm Pid
输入参数： 无
输出参数： 无。
返 回 值： 当前的Ecm Pid*/
UINT16 DIVI_GetCurr_EcmID(void)
{

  UINT16 ecm_pid = 0;

  ecm_pid = cas_divi_get_ecm_pid();
  CAS_DIVI_PRINTF("[DIVI],DIVI_GetCurr_EcmID:0x%x \n",ecm_pid);

  return ecm_pid;
}


/*提 供 者： 机顶盒
使 用 者： CA模块
功能描述：获得当前的NetRegionID
输入参数： 无
输出参数： 无。
返 回 值： 当前的NetRegionID*/
UINT16 DIVI_GetCurr_NetRegionID(void)
{
  cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;
  UINT16 network_id = 0;

  network_id = p_priv->network_id;
  OS_PRINTF("[DIVI],DIVI_GetCurr_NetRegionID:0x%x \n",network_id);
  CAS_DIVI_PRINTF("/************************************\n");
  CAS_DIVI_PRINTF("%s : %d\n", __FUNCTION__, __LINE__);
  CAS_DIVI_PRINTF("[DIVI],DIVI_GetCurr_NetRegionID:0x%x \n",network_id);
  CAS_DIVI_PRINTF("/************************************\n");
  return network_id;
}




UINT8 DIVI_SC_Get_CardState(void)
{
  UINT8 diviStatus = 0;

  CAS_DIVI_PRINTF("%s %d\n",__FUNCTION__,__LINE__);

  if(divi_get_card_status() == 1)//SC_INSERT_OVER)
  {
    diviStatus = DIVI_CA_CARD_INSERT;
  }
  else
  {
    diviStatus = DIVI_CA_CARD_REMOVE;
  }

  CAS_DIVI_PRINTF("diviStatus = %d\n",diviStatus);

  return diviStatus;
}

/*
INT32 DIVI_SC_DRV_SendDataEx()
提 供 者： 机顶盒。
使 用 者： CA模块。
何时使用： 向智能卡发送数据时。
功能描述： CA模块调用此函数向智能卡发送数据。
输入参数：
		bCardNumber： 暂时保留不用；
		bLength： pabMessage的长度；
		pabMessage： 发送命令的消息指针；
		pabResponse： 接收响应结果的数据块的指针；
		bRLength： 响应结果的数据块长度指针。
输出参数：
		pabResponse： 响应结果的数据块；
		bRLength： 响应结果的数据块长度。
		pbSW1 智能卡状态字节1
		pbSW2 智能卡状态字节2
返 回 值：
		TRUE：成功
		FALSE：失败
其他说明：
*/
INT32 DIVI_SC_DRV_SendDataEx( UINT8 bCardNumber,
                              UINT8 bLength,
                              UINT8 * pabMessage,
                              UINT8 * pabResponse,
                              UINT8 RBufferLen,
                              UINT8 * bRLength,
                              UINT8 * pbSW1,
                              UINT8 * pbSW2)
{
    RET_CODE ret = SUCCESS;
    scard_opt_desc_t rwopt = {0};
    UINT8 read_bytes[255] = {0};
    INT8 retry_times = 0;
//    UINT8 i = 0;

    mtos_task_lock();
    mtos_task_delay_ms(10);

    //CAS_DIVI_PRINTF("%s %d\n",__FUNCTION__,__LINE__);

    //CAS_DIVI_PRINTF("[DIVI],DIVI_SC_DRV_SendDataEx \n");

    if(pabMessage == NULL || pabResponse == NULL || bRLength == NULL
       || pbSW1 == NULL || pbSW2 == NULL)
    {
         CAS_DIVI_PRINTF("DIVI_SC_DRV_SendDataEx parameter error! \n");
         MT_ASSERT(0);
         mtos_task_unlock();
         return FALSE;
    }


    /*CAS_DIVI_PRINTF("send data to card: ");
    for(i = 0; i < bLength; i++)
    {
         CAS_DIVI_PRINTF(" 0x%x ",*(pabMessage + i));
    }
    CAS_DIVI_PRINTF("\n");
    */

    rwopt.p_buf_out = pabMessage;
    rwopt.bytes_out = bLength;
    rwopt.p_buf_in   = read_bytes;
    rwopt.bytes_in_target = 0;
    rwopt.bytes_in_actual = 0;

    do
    {

        ret = scard_pro_rw_transaction(&rwopt);
        //CAS_DIVI_PRINTF("scard_pro_rw_transaction Ret = %d\n",ret);
        retry_times++;
    }while((0 != ret) && (retry_times <= 3));

    if(retry_times > 3)
    {
         CAS_DIVI_PRINTF("Send data to card error ! \n");

         mtos_task_unlock();
         return FALSE;
    }


    /*CAS_DIVI_PRINTF("receive data to card [%d]: ",rwopt.bytes_in_actual);
    for(i = 0; i < rwopt.bytes_in_actual; i++)
    {
         CAS_DIVI_PRINTF(" 0x%x ",read_bytes[i]);
    }
    CAS_DIVI_PRINTF("\n");*/

    if(rwopt.bytes_in_actual > 2)
    {
          //CAS_DIVI_PRINTF("RBufferLen = %d\n",RBufferLen);

         if(RBufferLen >= (rwopt.bytes_in_actual - 2))
         {
            memcpy(pabResponse,read_bytes,rwopt.bytes_in_actual - 2);
         }
         else
         {
            CAS_DIVI_PRINTF("Response cmd lenth is too length\n");
             mtos_task_unlock();
            return FALSE;
         }
         *pbSW1 = read_bytes[rwopt.bytes_in_actual - 2];
         *pbSW2 = read_bytes[rwopt.bytes_in_actual - 1];
         *bRLength = rwopt.bytes_in_actual - 2;
    }
    else if(rwopt.bytes_in_actual == 2)
    {
         *pbSW1 = read_bytes[0];
         *pbSW2 = read_bytes[1];
         *bRLength = 0;
    }
    else
    {
        CAS_DIVI_PRINTF("[DIVI],DIVI_SC_DRV_SendDataEx error! \n");
        mtos_task_unlock();
        return FALSE;
    }

    /*CAS_DIVI_PRINTF("[DIVI],*bRLength=0x%x,*pbSW1=0x%x,*pbSW2=0x%x \n",*bRLength,*pbSW1,*pbSW2);


    CAS_DIVI_PRINTF("return data to lib: ");
    for(i = 0; i < *bRLength; i++)
    {
         CAS_DIVI_PRINTF(" %X ",*(pabResponse + i));
    }
    CAS_DIVI_PRINTF("\n\n");*/

    mtos_task_unlock();
    return TRUE;
}

INT32 DIVI_SC_DRV_Initialize(void)
{
    INT32 err = 0;
    UINT32 *p_stack = NULL;

    CAS_DIVI_PRINTF("DIVI_SC_DRV_Initialize start init ! \n");


    err = mtos_sem_create((os_sem_t *)&s_ca_lock,1);
    if(!err)
    {
         CAS_DIVI_PRINTF("DIVI_SC_DRV_Initialize, create sem error ! \n");
    }

    p_stack = (u32 *)mtos_malloc(DIVI_CAS_SECTION_TASK_STACK_SIZE);
    MT_ASSERT(p_stack != NULL);

    err = mtos_task_create((u8 *)"CA_Monitor",
            divi_ca_data_monitor,
            (void *)0,
            DIVI_CAS_SECTION_TASK_PRIO,
            p_stack,
            DIVI_CAS_SECTION_TASK_STACK_SIZE);

    if (!err)
    {
      CAS_DIVI_PRINTF("Create data monitor task error = 0x%08x!\n", err);
      return 1;
    }

    return 0;
}

BOOL DIVI_Show_Finger(UINT8 Position,UINT8 Duriaon,UINT8 * Finger_Content,UINT8 Content_Length)
{
  CAS_DIVI_PRINTF("%s %d\n",__FUNCTION__,__LINE__);

  return 0;
}

INT8 DIVI_GetFingerNum(void)
{
  CAS_DIVI_PRINTF("%s %d\n",__FUNCTION__,__LINE__);

  return 0;
}

//bCardNumber 默认0
INT32  DIVI_SC_DRV_ResetCard(UINT8 bCardNumber,
                            UINT8 * ATRBuf,
                            UINT8 BufferLen,
                            UINT8 * RLength)
{
  u32 err = 0;
  u8 uResetTimes = 0;
  scard_atr_desc_t s_atr = {0};
  u8 buf_atr[256] = {0,};
  scard_device_t *p_smc_dev = NULL;

  s_atr.p_buf = buf_atr;

  CAS_DIVI_PRINTF("%s %d\n",__FUNCTION__,__LINE__);

  if(NULL == ATRBuf || NULL == RLength)
  {
    CAS_DIVI_PRINTF("\nDIVI_SC_DRV_ResetCard error parameter!\n");
    MT_ASSERT(0);
    return FAILTURE;
  }

  p_smc_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_SMC);
  MT_ASSERT(NULL != p_smc_dev);

  do
  {
    err = scard_active(p_smc_dev, &s_atr);
    uResetTimes++;
  }while(err != 0 && uResetTimes < 3);

  if(0 != err)
  {
    if(DIVI_CA_CARD_INSERT == divi_get_card_status())
    {
      cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

      cas_send_event(p_priv->slot, CAS_ID_DIVI, CAS_E_ILLEGAL_CARD, 0);
    }

    CAS_DIVI_PRINTF("%s : %d\n", __FUNCTION__, __LINE__);
    return FAILTURE;
  }
  else
  {
    if(BufferLen >= s_atr.atr_len)
    {
      memcpy(ATRBuf, s_atr.p_buf, s_atr.atr_len);
    }
    else
    {
      CAS_DIVI_PRINTF("%s : %d \n",__FUNCTION__, __LINE__);

      return FAILTURE;
    }

    *RLength = (u8)s_atr.atr_len;
    return SUCCESS;
  }
  return FAILTURE;
}


