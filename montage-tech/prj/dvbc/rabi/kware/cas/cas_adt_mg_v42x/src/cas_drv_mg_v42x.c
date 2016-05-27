/****************************************************************************/
/*                                                                          */
/*  Copyright (c) 2001-2012 Shanghai Advanced Digital Technology Co., Ltd   */
/*                                                                          */
/* PROPRIETARY RIGHTS of Shanghai Advanced Digital Technology Co., Ltd      */
/* are involved in the subject matter of this material. All manufacturing,  */
/* reproduction, use, and sales rights pertaining to this subject matter    */
/* are governed by the license agreement. The recipient of this software    */
/* implicitly accepts the terms of the license.                             */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*                                                                          */
/* FILE NAME                                                                */
/*      mgeos.c                                                             */
/*                                                                          */
/* VERSION                                                                  */
/*      v4.2.0+                                                             */
/*                                                                          */
/* DESCRIPTION                                                              */
/*      The functions contained in this file should be rewrite when porting */
/*      to the special STB platform.                                        */
/*                                                                          */
/* AUTHOR                                                                   */
/*      Eileen Geng                                                         */
/*      Gaston Shi                                                          */
/*                                                                          */
/****************************************************************************/
//lint -e574 -e716 -e734 -e737 

#include "mgdef_v42x.h"
/*#include ...*/
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_sem.h"
#include "mtos_task.h"
#include "mtos_misc.h"
#include "smc_op.h"
#include "smc_pro.h"

//#define CAS_MGEOS_DEBUG
#ifdef CAS_MGEOS_DEBUG
#define CAS_MGEOS_PRINTF OS_PRINTF
#else
#define CAS_MGEOS_PRINTF(...) do{} while(0)
#endif

extern os_sem_t *p_g_mg_port_sem;
extern scard_device_t *p_g_smc_dev;

extern RET_CODE cas_adt_mg_card_is_removed(void);
extern void cas_adt_mg_set_reset_flag(void);

MG_STATUS MG_STBPort_Send_Command(MG_U8 *MG_SendComm, MG_U8 MG_CommLen, MG_U8 *MG_RecvResp,MG_U16 *SW, MG_U8 * NumberRead)
{
  scard_opt_desc_t opt_desc = {0};
  u8 status[2] = {0, 0};
  u8 cmd[5];
  RET_CODE ret = ERR_FAILURE;
  MG_STATUS MG_ret = MG_SUCCESS;
  u32 temp = 0;
  u8 retry = 1;
  int i = 0;

  opt_desc.p_buf_out = MG_SendComm;
  opt_desc.bytes_out = MG_CommLen;
  opt_desc.p_buf_in = MG_RecvResp;
  opt_desc.bytes_in_actual = 0;

  while(1)
  {
    //mtos_task_delay_ms(200);
    CAS_MGEOS_PRINTF("##SendComm:");
    for(i=0; i<MG_CommLen;i++)
    {
        CAS_MGEOS_PRINTF("0x%x ", MG_SendComm[i]);
    }
    CAS_MGEOS_PRINTF("\n");
    mtos_task_lock();
     ret = scard_pro_t0_transaction(&opt_desc, status);
    mtos_task_unlock();
    CAS_MGEOS_PRINTF("STBPort: sw0[0x%x], sw1[0x%x]\n", status[0], status[1]);
    if(SUCCESS != ret)
    {
      CAS_MGEOS_PRINTF("##STBPort: t0_transaction error\n");
      if(cas_adt_mg_card_is_removed() != SUCCESS)
      {
          if(retry > 0)
          {
              retry--;
              continue;
          }
          cas_adt_mg_set_reset_flag();
      }
      
      MG_ret = MG_FAILING;
      break;
    }
    #if 0
    if(status[0] == 0x90 && status[1] == 0x00)
    {
      /* success */
      CAS_MGEOS_PRINTF("STBPort Send_Command ok\n");
      MG_ret = MG_SUCCESS;
      break;
    }
    #endif
    if(status[0] == 0x61)
    {
      CAS_MGEOS_PRINTF("STBPort: still has bytes to read\n");
      /* already has data need to be read */
      cmd[0] = 0x00; //CLA
      cmd[1] = 0xC0; //INS = GET RESPONSE
      cmd[2] = cmd[3] = 0;
      cmd[4] = status[1]; //response length
      opt_desc.p_buf_out = cmd;
      opt_desc.bytes_out = 5;
      temp = (u32)MG_RecvResp + opt_desc.bytes_in_actual;
      opt_desc.p_buf_in = (u8 *)temp;
      status[0] = status[1] = 0; //continue transfer
    }
    else
    {
      break;
    }
  }
  CAS_MGEOS_PRINTF("##RecvResp:");
    for(i=0; i<opt_desc.bytes_in_actual;i++)
    {
        CAS_MGEOS_PRINTF("0x%x ", MG_RecvResp[i]);
    }
    CAS_MGEOS_PRINTF("\n");
  *NumberRead = opt_desc.bytes_in_actual;
  if(MG_FAILING == MG_ret)
  {
    *NumberRead = 0;
  }  
  *SW = ((MG_U16)status[0] << 8) |status[1];  
  return MG_ret;
}

MG_STATUS MG_STBPort_Create_Semaphore(void)
{
  BOOL ret;
  
  ret = mtos_sem_create(p_g_mg_port_sem, TRUE);
  return (ret == TRUE) ? MG_SUCCESS : MG_FAILING;
}

MG_STATUS MG_STBPort_Obtain_Semaphore()
{
  BOOL ret;
  
  ret = mtos_sem_take(p_g_mg_port_sem, 0);
  return (ret == TRUE) ? MG_SUCCESS : MG_FAILING;
}

MG_STATUS MG_STBPort_Release_Semaphore()
{
  BOOL ret;
  
  ret = mtos_sem_give(p_g_mg_port_sem);
  return (ret == TRUE) ? MG_SUCCESS : MG_FAILING;
}

MG_STATUS MG_STBPort_Delete_Semaphore()
{
  BOOL ret;
  
  ret = mtos_sem_destroy(p_g_mg_port_sem, MTOS_DEL_ALWAYS);
  return (ret == TRUE) ? MG_SUCCESS : MG_FAILING;
}

MG_STATUS MG_STBPort_Enable_Scheduler(void)
{
  mtos_task_lock();
  return MG_SUCCESS;
}

MG_STATUS MG_STBPort_Disable_Scheduler(void)
{
  mtos_task_unlock();
  return MG_SUCCESS;
}

MG_STATUS MG_STBPort_Reset_Card(MG_U8 *MG_RespData)
{
  RET_CODE ret = ERR_FAILURE;
  scard_atr_desc_t atr_desc;

  CAS_MGEOS_PRINTF("MG_STBPort_Reset_Card\n");
  
  ret = scard_reset(p_g_smc_dev);
  if(SUCCESS != ret)
  {
    CAS_MGEOS_PRINTF("cas: reset card failed\n");
    return MG_FAILING;
  }
  
  atr_desc.p_buf = MG_RespData;
  ret = scard_active(p_g_smc_dev, &atr_desc);
  if(SUCCESS != ret)
  {
    CAS_MGEOS_PRINTF("cas: active card failed\n");
    return MG_FAILING;
  }

  return MG_SUCCESS;
}

MG_STATUS MG_STBPort_Close_Card(void)
{
  RET_CODE ret = ERR_FAILURE;
  
  ret = scard_deactive(p_g_smc_dev);
  if(SUCCESS != ret)
  {
    return MG_FAILING;
  }

  return MG_SUCCESS;  
}

MG_U32 MG_STBPort_Get_OSTime(void)
{
  return mtos_ticks_get();
}

MG_U32 MG_STBPort_Add_OSTime(MG_U32 time1,MG_U32 time2)
{
  return (time1 + time2);
}

MG_U32 MG_STBPort_Minus_OSTime(MG_U32 time1,MG_U32 time2)
{
  if(time2 <= time1)
  {
    return (time1 - time2);
  }

  return (0xFFFFFFFF - time2 + 1 + time1);
}

MG_U32 MG_STBPort_Get_OSTime_Ticks(void)
{
  return 100;
}
//lint +e574 +e716 +e734 +e737 