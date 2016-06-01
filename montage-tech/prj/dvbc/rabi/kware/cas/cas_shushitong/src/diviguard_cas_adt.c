/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
#include "string.h"
#include "trunk/sys_def.h"
#include "sys_types.h"
#include "sys_define.h"
#include "sys_dbg.h"
#include "stdio.h"
#include "stdlib.h"

#include "lib_bitops.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "drv_dev.h"
#include "drv_svc.h"
#include "pti.h"
#include "smc_op.h"
#include "smc_pro.h"
#include "cas_ware.h"
#include "cas_adapter.h"

#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_int.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include "mtos_task.h"

#include "DiviguardCa.h"
#include "diviguard_cas_include.h"
#include "mgdef_v42x.h"
#include "mg_cas_include.h"

#include "nim.h"
#include "smc_op.h"
#include "Lib_util.h"

#include "ui_common.h"


#define CAS_DIVI_ADT_DEBUG

#ifdef CAS_DIVI_ADT_DEBUG
#define CAS_DIVI_ADT_PRINTF OS_PRINTF
#else
#define CAS_DIVI_ADT_PRINTF DUMMY_PRINTF
#endif
extern u32 g_cas_id_flag;


BOOL s_divi_entitle_flag = 0; //0:entitle  1:no entitle
extern void DIVI_ClearCW(UINT16 wEcmPid);

extern cas_adapter_priv_t g_cas_priv;
static divi_ca_card_status_t s_card_status = DIVI_CA_CARD_UNKNOWN;
static os_sem_t s_pid_lock = 0;
u8 divi_ca_nit_state = 0;


/*
*Network License Manager API 
*/
extern unsigned long _mktime (unsigned int year, unsigned int mon,
							unsigned int day, unsigned int hour,unsigned int min, unsigned int sec);

BOOL get_vol_entitlement_from_ca(void)
{
	
	if(CAS_ID_ADT_MG == g_cas_id_flag)
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
	}
	else
	{
		u8 uIndex = 0;
	    PackageInfo Package = {0,};
		u32 curTime, expTime;
		utc_time_t utc_time = {0};
		time_set_t p_set={{0}};

		if(CUSTOMER_DTMB_CHANGSHA_HHT == CUSTOMER_ID)
		{
			if(DIVI_GetProductOneIs() && (divi_get_card_status() == DIVI_CA_CARD_INSERT))
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			if(DIVI_GetSMCEntitleInfo(&Package) != 0x02 )
		    {
		        DEBUG(DBG,INFO,"DIVI_GetSMCEntitleInfo fail \n");
		        return FALSE;
		    }
			
			sys_status_get_time(&p_set);
			time_get(&utc_time, p_set.gmt_usage);
			curTime = _mktime(utc_time.year,utc_time.month,utc_time.day,0,0,0);
			DEBUG(DBG,INFO, "utc_time.year[%d]	utc_time.month[%d] utc_time.day[%d] \n",utc_time.year,utc_time.month,utc_time.day);

		    DEBUG(DBG,INFO,"Package Count = %d\n",Package.Package_Count);
		    for(uIndex = 0; uIndex < Package.Package_Count; uIndex++)
		    {
		    	expTime = _mktime(Package.PackageInfo[uIndex].EndYear,Package.PackageInfo[uIndex].EndMonth,Package.PackageInfo[uIndex].EndDate,0,0,0);
				DEBUG(DBG,INFO,"curTime = %x expTime = %x \n",curTime,expTime);
				DEBUG(DBG,INFO,"year[%d] month[%d] day[%d]\n",Package.PackageInfo[uIndex].EndYear,Package.PackageInfo[uIndex].EndMonth,Package.PackageInfo[uIndex].EndDate);

				if(expTime >= curTime)
					return TRUE;
		    }
		}
	}
	return FALSE;
}

BOOL get_program_entitlement_from_ca(void)
{
	if(DIVI_GetProductOneIs() && (divi_get_card_status() == DIVI_CA_CARD_INSERT))
	{
		DEBUG(DBG,ERR,"TRUE\n");
		return TRUE;
	}
	else
	{
		DEBUG(DBG,ERR,"FALSE\n");
		return FALSE;
	}

	return TRUE;
}

BOOL get_card_reset_finsh(void)
{
	UINT8 card_sn[DIVI_CARD_SN_LEN] = {0};
	
	if(DIVI_GetSMCNO(card_sn) == DIVI_OK)
		return TRUE;

	return FALSE;
}


void send_event_to_ui_from_authorization(u32 event)
{
	if(event == 0)
	{
		OS_PRINTF("send enent fail!\n");
		return;
	}
	if(CAS_ID_ADT_MG == g_cas_id_flag)
	{
		cas_adt_mg_priv_t *p_priv = g_cas_priv.cam_op[CAS_ID_ADT_MG].p_priv;
		cas_send_event(p_priv->slot, CAS_ID_ADT_MG, event, 0);
//		OS_PRINTF("send enent from authorization!\n");
	}
	else
	{
		cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;
		cas_send_event(p_priv->slot, CAS_ID_DIVI, event, 0);
	}
	DEBUG(DBG,INFO, "send enent from authorization!\n");
}

void send_event_to_ui_from_ads(u32 event, u32 param)
{
	cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

	if(event == 0)
	{
		DEBUG(DBG,INFO, "send enent fail!\n");
		return;
	}
	cas_send_event(p_priv->slot, CAS_ID_DIVI, event, param);
	DEBUG(DBG,INFO, "send enent from authorization!\n");
}


/*!
  abc
  */
void set_event(cas_divi_priv_t *p_priv, u32 event)
{
  CAS_DIVI_ADT_PRINTF("divi cas: set_event  event  =%d  \n", event);
  if(event == 0)
  {
    return;
  }

 cas_send_event(p_priv->slot, CAS_ID_DIVI, event, 0);

}
/*!
    lock
*/
static void divi_pid_lock(void)
{
    //mtos_sem_take((os_sem_t *)&s_pid_lock, 0);
}

/*!
    unlock
*/
static void divi_pid_unlock(void)
{
   // mtos_sem_give((os_sem_t *)&s_pid_lock);
}

/*!
    get card status
*/
divi_ca_card_status_t divi_get_card_status()
{
     divi_ca_card_status_t status = DIVI_CA_CARD_UNKNOWN;

     divi_pid_lock();
     status = s_card_status;
     divi_pid_unlock();

     //CAS_DIVI_ADT_PRINTF("[divi],get status=%d \n",status);
     return status;
}

/*!
    set card status
*/
void divi_set_card_status(divi_ca_card_status_t status)
{
     CAS_DIVI_ADT_PRINTF("[divi],set status=%d \n",status);

     divi_pid_lock();
     s_card_status = status;
     divi_pid_unlock();
}

/*!
    get video pid
*/
UINT16 cas_divi_get_v_pid(void)
{
  cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

  //CAS_DIVI_ADT_PRINTF("p_priv addr:0x%x, p_priv->v_pid=0x%x \n",p_priv,p_priv->v_pid);

  return p_priv->v_pid;
}

/*!
    get audio pid
*/
UINT16 cas_divi_get_a_pid(void)
{
  cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

  //CAS_DIVI_ADT_PRINTF("p_priv addr:0x%x,p_priv->a_pid=0x%x \n",p_priv,p_priv->a_pid);
  return p_priv->a_pid;
}

/*!
  Set scramble flag

  \param[in] scramble_flag.
  */
void cas_divi_set_scramble_flag(UINT8 scramble_flag)
{
  cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

  divi_pid_lock();
  p_priv->scramble_flag = scramble_flag;
  divi_pid_unlock();

  CAS_DIVI_ADT_PRINTF("[divi],cas_divi_set_scramble_flag scramble_flag=0x%x \n",p_priv->scramble_flag);
}

/*!
  Get scramble flag

  \return scramble_flag.
  */
UINT8 cas_divi_get_scramble_flag(void)
{
  cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;
  UINT16 scramble_flag = 0;

  divi_pid_lock();
  scramble_flag = p_priv->scramble_flag;
  divi_pid_unlock();

  CAS_DIVI_ADT_PRINTF("[divi],cas_divi_get_scramble_flag scramble_flag=0x%x \n",p_priv->scramble_flag);
  return scramble_flag;
}

/*!
  Set ecm pid

  \param[in] ecm_pid.
  */
void cas_divi_set_ecm_pid(UINT16 ecm_pid)
{
  cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

  divi_pid_lock();
  p_priv->ecm_pid = ecm_pid;
  divi_pid_unlock();

  CAS_DIVI_ADT_PRINTF("[divi],cas_divi_set_ecm_pid ecm_pid=0x%x \n",p_priv->ecm_pid);
}

/*!
  Get ecm pid

  \return ecm_pid.
  */
UINT16 cas_divi_get_ecm_pid(void)
{
  cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;
  UINT16 ecm_pid = 0;

  divi_pid_lock();
  ecm_pid = p_priv->ecm_pid;
  divi_pid_unlock();

  return ecm_pid;
}

/*!
  Set emm pid

  \param[in] emm_pid.
  */
void cas_divi_set_emm_pid(UINT16 emm_pid)
{
  cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

  divi_pid_lock();
  p_priv->emm_pid = emm_pid;
  divi_pid_unlock();

  CAS_DIVI_ADT_PRINTF("[divi],cas_divi_set_emm_pid emm_pid=0x%x \n",p_priv->emm_pid);
}

/*!
  Get emm pid

  \return emm_pid.
  */
UINT16 cas_divi_get_emm_pid(void)
{
  cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;
  UINT16 emm_pid = 0;

  divi_pid_lock();
  emm_pid = p_priv->emm_pid;
  divi_pid_unlock();

  return emm_pid;
}

/*!
  Set server id

  \param[in] server_id.
  */
void cas_divi_set_server_id(UINT16 server_id)
{
  cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

  divi_pid_lock();
  p_priv->server_id = server_id;
  divi_pid_unlock();
}

/*!
  Get network id

  \return network id.
  */
UINT16 cas_divi_get_server_id(void)
{
  cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;
  UINT16 server_id = 0;

  divi_pid_lock();
  server_id = p_priv->server_id;
  divi_pid_unlock();

  return server_id;
}







void cas_divi_zone_check_end(UINT32 event)
{
    cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

    CAS_DIVI_ADT_PRINTF("\r\n*** cas_divi_zone_check_end *** \r\n");
    set_event(p_priv, event);
    return;
}



/*!
  send message to hide no entitle message
  */
void cas_divi_cancel_entitle_message()
{
    cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

    CAS_DIVI_ADT_PRINTF("\r\n*** divi_cancel_entitle_message *** \r\n");
    set_event(p_priv, CAS_C_ENTITLE_LIMIT);
    return;
}


/*!
  abc
  */
static RET_CODE cas_divi_init(void)
{
  static u32 init_flag = 0;
  INT32 ret = 0;


  if(!init_flag)
  {
  
    DIVI_Initialize(0);
    CAS_DIVI_ADT_PRINTF("cas_divi_init\n");

    ret = mtos_sem_create((os_sem_t *)&s_pid_lock,1);
    if(!ret)
    {
       CAS_DIVI_ADT_PRINTF("[divi], cas_divi_init  sem create error ! \n");
       MT_ASSERT(0);
    }

    DIVI_SC_DRV_Initialize();


    init_flag = 1;
    mtos_task_sleep(300);
    //cas_init_nvram_data(CAS_ID_DIVI);
  }
  return SUCCESS;
}

/*!
  abc
  */
static RET_CODE cas_divi_deinit(void)
{
  return SUCCESS;
}

/*!
  check guotong ca_sys_id

  \param[in] ca_sys_id.
  \return success if success else ERR_FAILURE
  */
RET_CODE cas_divi_identify(UINT16 ca_sys_id)
{
     RET_CODE ret = 0;

     ret = DIVI_IsMatchCAID(ca_sys_id);
     if(ret != TRUE)
     {
             CAS_DIVI_ADT_PRINTF("[divi] No divi CA card ! \n");
             return ERR_FAILURE;
     }

    return SUCCESS;
}



/*++
功能：通知上层区域检测结束
参数：
--*/
static void cas_divi_ZoneCheckEnd(void)
{
    cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

    CAS_DIVI_ADT_PRINTF("\r\n*** cas_divi_ZoneCheckEnd*** \r\n");
    set_event(p_priv, CAS_S_ZONE_CHECK_END);
    return;
}

static UINT8 s_mail_flag[MAX_EMAIL_NUM] = {0};
extern msg_info_t divi_OsdMsg;


INT32 divi_get_mail_header(UINT16 *max_mail_num, cas_mail_header_t *mail_header)
{
     TDIVI_EmailHead xsm_email_head[MAX_EMAIL_NUM] = {{{0},0,0,0,{0}},};
     UINT8 email_head_num = 0;
     UINT8 email_head_len = 0;
     UINT8 i = 0;

     if(max_mail_num == NULL || mail_header == NULL)
     {
          CAS_DIVI_ADT_PRINTF("[divi], gt_get_mial_header paramter error ! \n");
          MT_ASSERT(0);
     }

     email_head_num = DIVI_GetEmailHeads(xsm_email_head, MAX_EMAIL_NUM);
     CAS_DIVI_ADT_PRINTF("[divi],email_head_num=%d \n",email_head_num);
     for(i = 0; i < email_head_num; i++)
     {
          mail_header[i].m_id = xsm_email_head[i].m_bEmailID;
          mail_header[i].index = i;
          mail_header[i].new_email = xsm_email_head[i].m_bNewEmail;
          mail_header[i].priority = xsm_email_head[i].m_bEmail_Level;
          email_head_len = strlen((char *)xsm_email_head[i].m_szEmailTitle);
          CAS_DIVI_ADT_PRINTF("[divi],email_head_len=%d \n",email_head_len);
          strncpy((void *)mail_header[i].subject,(char *)xsm_email_head[i].m_szEmailTitle,email_head_len);
          CAS_DIVI_ADT_PRINTF("year=%04d %04d %02d %02d %02d %02d \n",
                                                               xsm_email_head[i].m_tCreateTime.Y,
                                                               xsm_email_head[i].m_tCreateTime.M,
                                                               xsm_email_head[i].m_tCreateTime.D,
                                                               xsm_email_head[i].m_tCreateTime.H,
                                                               xsm_email_head[i].m_tCreateTime.MI,
                                                               xsm_email_head[i].m_tCreateTime.S);

          mail_header[i].send_date[0] = (UINT8)((xsm_email_head[i].m_tCreateTime.Y) / 100);
          mail_header[i].send_date[1] = (UINT8)((xsm_email_head[i].m_tCreateTime.Y) % 100);
          mail_header[i].send_date[2] = (UINT8)((xsm_email_head[i].m_tCreateTime.M) & 0xff);
          mail_header[i].send_date[3] =  xsm_email_head[i].m_tCreateTime.D;
          mail_header[i].send_date[4] =  xsm_email_head[i].m_tCreateTime.H;
          mail_header[i].send_date[5] =  xsm_email_head[i].m_tCreateTime.MI;
          mail_header[i].send_date[6] =  xsm_email_head[i].m_tCreateTime.S;
          s_mail_flag[mail_header[i].m_id] = 1;

          CAS_DIVI_ADT_PRINTF("sendtime: %02d %02d %02d %02d %02d %02d %02d \n",
                                                               mail_header[i].send_date[0],
                                                               mail_header[i].send_date[1],
                                                               mail_header[i].send_date[2],
                                                               mail_header[i].send_date[3],
                                                               mail_header[i].send_date[4],
                                                               mail_header[i].send_date[5],
                                                               mail_header[i].send_date[6]);

          CAS_DIVI_ADT_PRINTF("[divi],index:%d \n",mail_header[i].index);
          CAS_DIVI_ADT_PRINTF("[divi],email id:%d \n",mail_header[i].m_id);
          CAS_DIVI_ADT_PRINTF("[divi],new_email:%d \n",mail_header[i].new_email);
          CAS_DIVI_ADT_PRINTF("[divi],priority:%d \n",mail_header[i].priority);
          //CAS_DIVI_ADT_PRINTF("[divi],creat date:%s \n",mail_header[i].send_date);
          CAS_DIVI_ADT_PRINTF("[divi],subject:%s \n",mail_header[i].subject);

          CAS_DIVI_ADT_PRINTF("[divi],s_mail_flag[%d]=%d \n",mail_header[i].m_id,
                                            s_mail_flag[mail_header[i].m_id]);
     }

     *max_mail_num = email_head_num;

     return SUCCESS;
}

/*!
  get mail body

  \param[in] mail_id.
  \param[out] p_mail_content .
  */
INT32 divi_get_mail_body(u32 mail_id,cas_mail_body_t *p_mail_content)
{
     BOOL err = FALSE;
     UINT8 email_body_len = 0;
     TDIVI_EmailContent email_content = {{0}};

     if(p_mail_content == NULL)
     {
          CAS_DIVI_ADT_PRINTF("[divi], gt_get_mail_body paramter error ! \n");
          MT_ASSERT(0);
     }

      CAS_DIVI_ADT_PRINTF("Call DIVI_GetEmailContent start\n");
      err = DIVI_GetEmailContent(mail_id,&email_content);
      CAS_DIVI_ADT_PRINTF("Call DIVI_GetEmailContent end\n");

      if(err == FALSE)
      {
           CAS_DIVI_ADT_PRINTF("[divi], gt_get_mail_body, not get mail body ! \n");
           return ERR_FAILURE;
      }

      CAS_DIVI_ADT_PRINTF(" content:%s \n",&email_content.m_szEmail);

      p_mail_content->mail_id = mail_id;
      email_body_len = strlen((char *)email_content.m_szEmail);
      CAS_DIVI_ADT_PRINTF("[divi],email_body_len:%d \n",email_body_len);
      strncpy((void *)p_mail_content->data,(char *)email_content.m_szEmail,email_body_len);

      CAS_DIVI_ADT_PRINTF("[divi],content:%s \n",(char *)p_mail_content->data);

      return SUCCESS;
}

#define DIVI_MAXLEN_PINCODE           6U    /* PIN码的长度 */
#define DIVI_MAXNUM_ENTITLE           32U

UINT8 BcdToDec(const UINT8 bcd)
{
	return (((bcd >> 4) & 0x0F) * 10 + (bcd & 0x0F));
}

UINT8 DecToBcd(const UINT8 dec)
{
  return (((dec/10)<<4) +((dec%10)&0x0F));
}

static u32 divi_ADT_SetRating(u8 *pbyPIN, u8 byRating)
{
    INT32 ret = 0;
    u8 uIndex = 0;
    u8 aPin[DIVI_MAXLEN_PINCODE] = {0};
    u8 Repeattime = 0;
    
    ///TODO: any limitions:byRating < 4 OR| byRating > 18?
    if(NULL == pbyPIN)
        return ERR_FAILURE;

    CAS_DIVI_ADT_PRINTF("PIN:\n");
    for(uIndex = 0; uIndex < DIVI_MAXLEN_PINCODE; uIndex++)
    {
        aPin[uIndex] = pbyPIN[uIndex]+'0';
        CAS_DIVI_ADT_PRINTF("%d", aPin[uIndex]);
    }
    CAS_DIVI_ADT_PRINTF("\n");

    CAS_DIVI_ADT_PRINTF("byRating = %d\n", byRating);

    ret = DIVI_SetCardLevel(aPin, byRating,&Repeattime);
    
    return ret;
}

static INT32 divi_ADT_SetWorkTime(cas_card_work_time_t *p_time)
{
    INT32 ret = 0;
    u8 uIndex = 0;
    u8 Repeattime = 0;
    u8 ch_pin[DIVI_MAXLEN_PINCODE] = {0};
    u8 enable = 0;
    u8 beginTime[3] = {0,};
    u8 endTime[3] = {0,};
    
    if(p_time == NULL)
        return ERR_FAILURE;

    CAS_DIVI_ADT_PRINTF("PIN:\n");
    for(uIndex = 0; uIndex < DIVI_MAXLEN_PINCODE; uIndex++)
    {
        ch_pin[uIndex] = p_time->pin[uIndex]+'0';
        CAS_DIVI_ADT_PRINTF("%d", ch_pin[uIndex]);
    }
    CAS_DIVI_ADT_PRINTF("\n");
    CAS_DIVI_ADT_PRINTF("start time: %d-%d-%d, end time: %d-%d-%d\n", 
                                        p_time->start_hour, 
                                        p_time->start_minute, 
                                        p_time->start_second, 
                                        p_time->end_hour, 
                                        p_time->end_minute, 
                                        p_time->end_second);

    enable = p_time->state;
    CAS_DIVI_ADT_PRINTF("[DIVI]Work Time status = %d\n",enable);
   
    beginTime[0] = DecToBcd(p_time->start_hour);
    beginTime[1] = DecToBcd(p_time->start_minute);
    beginTime[2] = DecToBcd(p_time->start_second);

    endTime[0] = DecToBcd(p_time->end_hour);
    endTime[1] = DecToBcd(p_time->end_minute);
    endTime[2] = DecToBcd(p_time->end_second);

   CAS_DIVI_ADT_PRINTF("start time: %x-%x-%x, end time: %x-%x-%x\n", 
                                        beginTime[0], beginTime[1],beginTime[2], 
                                        endTime[0], endTime[1], endTime[2]);

    ret = DIVI_SetWorkPeriod(ch_pin, beginTime,endTime,enable,&Repeattime);

    CAS_DIVI_ADT_PRINTF("DIVI_SetWorkPeriod ret = %d\n",ret);
    return ret;
}

static INT32 divi_ADT_ChangeSmartCardPIN(cas_pin_modify_t * p_pin_modify)
{
    u8 uIndex = 0;
    u8 Repeattime = 0;
    u8 ch_old[DIVI_MAXLEN_PINCODE] = {0};
    u8 ch_new[DIVI_MAXLEN_PINCODE] = {0};
    INT32 ret = 0;

    if(p_pin_modify == NULL)
        return ERR_FAILURE;

    for(uIndex = 0; uIndex < DIVI_MAXLEN_PINCODE; uIndex++)
    {
        ch_old[uIndex] = p_pin_modify->old_pin[uIndex]+'0';
        ch_new[uIndex] = p_pin_modify->new_pin[uIndex]+'0';
    }

    CAS_DIVI_ADT_PRINTF("Old PIN:\n");
    for(uIndex = 0; uIndex < DIVI_MAXLEN_PINCODE; uIndex++)
        CAS_DIVI_ADT_PRINTF("%d", ch_old[uIndex]);
    CAS_DIVI_ADT_PRINTF("\n");
    
    CAS_DIVI_ADT_PRINTF("New PIN:\n");
    for(uIndex = 0; uIndex < DIVI_MAXLEN_PINCODE; uIndex++)
        CAS_DIVI_ADT_PRINTF("%d", ch_new[uIndex]);
    CAS_DIVI_ADT_PRINTF("\n");


    //ret = DIVI_SetWorkPeriod(UINT8 * Pin, UINT8 * BeginTime,UINT8 * EndTime,UINT8 Enable,UINT8 * Repeattime);
    ret = DIVI_Modify_Pin(ch_old,ch_new,DIVI_MAXLEN_PINCODE,&Repeattime);

    
    if(ret != SUCCESS)
    {
        CAS_DIVI_ADT_PRINTF("[DIVI]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        CAS_DIVI_ADT_PRINTF("Repeattime = 0x%x\n",Repeattime);
        return ret;
    }

    return ret;
}

#if 0
void test_osd(int index)
{
  u8 osd_buf1[] = {"9000007-------97978888888888"};
  u8 osd_buf2[] = {"9000007-------1"};
  u32 event = 1;
  
 if(index == 0)
  DIVI_EventHandle( event ,(u32)osd_buf1,
                        (u32)sizeof(osd_buf1),event );
 if(index == 1)
  DIVI_EventHandle( event ,(u32)osd_buf2,
                        (u32)sizeof(osd_buf1),event );
}
#endif

static INT32 divi_ADT_GetEntitleIDs(u16 OperatorID,  u16 *max_num, product_entitle_info_t *ptProductEntitles)
{
    INT32 ret = 0;
    u16 uIndex = 0;
    PackageInfo Package = {0,};

    if(ptProductEntitles == NULL)
        return ERR_FAILURE;
    CAS_DIVI_ADT_PRINTF("[YXSB][%s %d] OperatorID = %d\n", __FUNCTION__, __LINE__, OperatorID);

    ret = DIVI_GetSMCEntitleInfo(&Package);
    /*
    返回值：
        0---查询失败；
        1---无授权信息；
        2---查询成功
    */
    
    if(ret != 0x02 )
    {
        CAS_DIVI_ADT_PRINTF("[YXSB]Fatal Error on %s:LINE:%d!ret = %d\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    *max_num = Package.Package_Count;

    CAS_DIVI_ADT_PRINTF("Package Count = %d\n",*max_num);
    for(uIndex = 0; uIndex < Package.Package_Count; uIndex++)
    {
       
        ptProductEntitles[uIndex].product_id = Package.PackageInfo[uIndex].PackageId;

        ptProductEntitles[uIndex].start_time[0]= Package.PackageInfo[uIndex].StartYear >> 8;
        ptProductEntitles[uIndex].start_time[1]= Package.PackageInfo[uIndex].StartYear & 0x00ff;
        ptProductEntitles[uIndex].start_time[2]= Package.PackageInfo[uIndex].StartMonth >> 8;
        ptProductEntitles[uIndex].start_time[3]= Package.PackageInfo[uIndex].StartMonth & 0x00ff;
        ptProductEntitles[uIndex].start_time[4]= Package.PackageInfo[uIndex].StartDate;

        ptProductEntitles[uIndex].expired_time[0]= Package.PackageInfo[uIndex].EndYear >> 8;
        ptProductEntitles[uIndex].expired_time[1]= Package.PackageInfo[uIndex].EndYear & 0x00ff;
        ptProductEntitles[uIndex].expired_time[2]= Package.PackageInfo[uIndex].EndMonth >> 8;
        ptProductEntitles[uIndex].expired_time[3]= Package.PackageInfo[uIndex].EndMonth & 0x00ff;
        ptProductEntitles[uIndex].expired_time[4]= Package.PackageInfo[uIndex].EndDate;
        ptProductEntitles[uIndex].reserved = Package.PackageInfo[uIndex].PackagePrice;
        CAS_DIVI_ADT_PRINTF("PackageId = %d\n", ptProductEntitles[uIndex].product_id);

        CAS_DIVI_ADT_PRINTF("Start Year = 0x%x, Month = 0x%x, Data = 0x%x\n",
                    Package.PackageInfo[uIndex].StartYear,
                    Package.PackageInfo[uIndex].StartMonth,
                    Package.PackageInfo[uIndex].StartDate);


        CAS_DIVI_ADT_PRINTF("End Year = 0x%x, Month = 0x%x, Data = 0x%x\n",
                    Package.PackageInfo[uIndex].EndYear,
                    Package.PackageInfo[uIndex].EndMonth,
                    Package.PackageInfo[uIndex].EndDate);

        CAS_DIVI_ADT_PRINTF("ExpireData = 0x%x, PackagePrice = 0x%x\n",
                            Package.PackageInfo[uIndex].ExpireDate,
                            Package.PackageInfo[uIndex].PackagePrice);

        
    }

    
    CAS_DIVI_ADT_PRINTF("[YXSB][Func:%s, Line:%d]max_num = %d\n", __FUNCTION__, __LINE__, *max_num);
         
    return ret;
}
/*!
  abc
  */
static RET_CODE cas_divi_io_ctrl(u32 cmd, void *param)
{
    cas_divi_priv_t *p_priv = (cas_divi_priv_t *)g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;
    INT32 ret = SUCCESS;
    UINT16 i = 0;
    cas_card_info_t *p_card_info = NULL;
    UINT8 card_sn[DIVI_CARD_SN_LEN] = {0};
    //UINT16 card_region_id = 0;
    cas_table_info_t *p_Cas_table_info = NULL;
    DIVI_CAServiceInfo divi_ecm_info = {0};

    cas_mail_headers_t *p_mail_headers = NULL;
    cas_mail_body_t *p_mailcontent = NULL;
    utc_time_t *p_systime = NULL;

    UINT32 mail_index = 0;
    UINT32 err = 0;
    msg_info_t *p_msg_info = NULL;
    UINT8 region[4] = {0,};
    nim_channel_info_t *p_nim_info = NULL;
    nim_device_t *p_nim_dev = NULL;
    UINT8 nit_match[DIVI_FILTER_DEPTH_SIZE] = {NIT_TABLE_ID,0,0,0,0,0,0,0,0,0,0,0,0};
    UINT8 nit_mask[DIVI_FILTER_DEPTH_SIZE] =  {0xff,0,0,0,0,0,0,0,0,0,0,0,0};


    cas_pin_modify_t *p_pin_modify = NULL;
    cas_card_work_time_t *p_work_time = NULL;
    cas_rating_set_t *p_rating_set = NULL;
    u8 *p_rating = 0;
    INT32 card_type = -1;
    INT32 UsrLevel = -1;
    chlid_card_status_info *p_feed_info = NULL;

    product_entitles_info_t *p_entitles_info = NULL;
    //product_detitles_info_t *p_detitles_info = NULL;
    u8 beginTime[3] = {0,};
    u8 endTime[3] = {0,};
    u8 enable = 0;
              
    //CAS_DIVI_ADT_PRINTF("[divi] ---cas_divi_io_ctrl---  \n");

     switch(cmd)
     {
        case CAS_IOCMD_STOP:
          CAS_DIVI_ADT_PRINTF("[divi] CAS_IOCMD_STOP \n");
		  {
			static BOOL first_card_status = FALSE;
			if((first_card_status == FALSE)&&(divi_get_card_status() == DIVI_CA_CARD_REMOVE))
			{
				set_event(p_priv, CAS_S_ADPT_CARD_REMOVE);
			}
			first_card_status = TRUE;
		  }
          break;

        case CAS_IOCMD_SET_ECMEMM:
          CAS_DIVI_ADT_PRINTF("[divi] CAS_IOCMD_SET_ECMEMM \n");
          p_Cas_table_info = (cas_table_info_t *)param;
          for(i = 0; i < CAS_MAX_ECMEMM_NUM; i++)
          {
               CAS_DIVI_ADT_PRINTF("index = %d, ca_system_id = %d, ecm_id = %d, servie_id = %d\n",
                      i,p_Cas_table_info->ecm_info[i].ca_system_id,
                      p_Cas_table_info->ecm_info[i].ecm_id,p_Cas_table_info->service_id);

               if(TRUE == DIVI_IsMatchCAID(p_Cas_table_info->ecm_info[i].ca_system_id))
               {
                    divi_ecm_info.m_wEcmPid = p_Cas_table_info->ecm_info[i].ecm_id;
                    divi_ecm_info.m_bServiceCount = 1;
                    divi_ecm_info.m_wServiceId[0] = p_Cas_table_info->service_id;

                    CAS_DIVI_ADT_PRINTF("[divi]divi_SetEcmPID:0x%x, service id=0x%x \n",
                              divi_ecm_info.m_wEcmPid,divi_ecm_info.m_wServiceId[0]);

                    cas_divi_set_server_id(p_Cas_table_info->service_id);
                    cas_divi_set_ecm_pid(divi_ecm_info.m_wEcmPid);
                    DIVI_SetEcmPID(0,&divi_ecm_info);

                    break;
               }
          }

          if(i == CAS_MAX_ECMEMM_NUM)
          {
              divi_ecm_info.m_wEcmPid = 0x1fff;
              divi_ecm_info.m_bServiceCount = 1;
              divi_ecm_info.m_wServiceId[0] = p_Cas_table_info->service_id;

              CAS_DIVI_ADT_PRINTF("[divi],ecm pid=0X%X \n",divi_ecm_info.m_wEcmPid);
              CAS_DIVI_ADT_PRINTF("[divi],service_id=0x%X \n",divi_ecm_info.m_wServiceId[0]);
              cas_divi_set_ecm_pid(divi_ecm_info.m_wEcmPid);
              DIVI_SetEcmPID(0,&divi_ecm_info);
           }

          break;

        case CAS_IOCMD_VIDEO_PID_SET:
          CAS_DIVI_ADT_PRINTF("[divi] CAS_IOCMD_VIDEO_PID_SET \n");
          p_priv->v_pid = *((UINT16 *)param);
          CAS_DIVI_ADT_PRINTF("v_pid=0x%x \n",p_priv->v_pid);

          break;

        case CAS_IOCMD_AUDIO_PID_SET:
          CAS_DIVI_ADT_PRINTF("[divi] CAS_IOCMD_AUDIO_PID_SET \n");
          p_priv->a_pid = *((UINT16 *)param);
          CAS_DIVI_ADT_PRINTF("a_pid=0x%x ,v_pid=0x%x \n",p_priv->a_pid,p_priv->v_pid);

          break;
        case CAS_IOCMD_SYSTEM_TIME_GET:
          //获取当前日期和时间, 2013-02-26
          //用于在机顶盒无法获取TDT时显示当前时间
          //建议格式: YYYY-MM-DD HH:MM
          //CurrentTime[0]-CurrentTime[4]分别为年月日时分
          //其中年份需要加2000

          {
            u8 CurrentTime[5] = {0,};
            Divi_GetCurrentTime(CurrentTime);
            p_systime = (utc_time_t *)param;

            if(CurrentTime[0] < 100)
            {
              p_systime->year = CurrentTime[0]+2000;
              p_systime->month = CurrentTime[1];
              p_systime->day = CurrentTime[2];
              p_systime->hour = CurrentTime[3];
              p_systime->minute = CurrentTime[4];
            }
            else
            {
              memset(p_systime,0,sizeof(utc_time_t));
            }
            
            CAS_DIVI_ADT_PRINTF("year = %d, month = %d, day = %d, hour = %d, min = %d\n",
                                p_systime->year,p_systime->month,
                                p_systime->day,p_systime->hour,
                                p_systime->minute);
                                   
          }
          break;
        case CAS_IOCMD_CARD_INFO_GET:
          CAS_DIVI_ADT_PRINTF("[divi] CAS_IOCMD_CARD_INFO_GET \n");

          p_card_info = (cas_card_info_t *)param;
          
          if(DIVI_GetSMCNO(card_sn) == DIVI_OK)
          {
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",card_sn[0]);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",card_sn[1]);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",card_sn[2]);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",card_sn[3]);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",card_sn[4]);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",card_sn[5]);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",card_sn[6]);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",card_sn[7]);
            memcpy((char *)(p_card_info->sn),card_sn,DIVI_CARD_SN_LEN);//卡号
          }
          else
          {          
            memset(card_sn,0,DIVI_CARD_SN_LEN);
            CAS_DIVI_ADT_PRINTF("[divi], get card id error ! \n");
          }


          card_type = DIVI_GetSMCUserType();//卡类型
          if(0x08 == card_type)
          {
            card_type = 0x00;//母卡
          }
          else if(0x09 == card_type)
          {
            card_type = 0x01;
          }

          if(-1 != card_type )
          {
             p_card_info->card_type = card_type;
          }
          
          CAS_DIVI_ADT_PRINTF("card_type = %d\n",card_type);

          /*0x01---机卡配对  0x02---未机卡配对  0x00---操作失败 */
          p_card_info->paterner_stat = DIVI_GetPaired();//卡状态
          CAS_DIVI_ADT_PRINTF("paterner_stat = %d\n",p_card_info->paterner_stat);



          if(DIVI_OK == DIVI_GetRegion(region))//区域码
          {
            CAS_DIVI_ADT_PRINTF("region %d,%d,%d,%d\n",region[0],region[1],region[2],region[3]);
            memcpy(p_card_info->area_code,region,4);
          }
          else
          {
            CAS_DIVI_ADT_PRINTF("[divi], get card region error ! \n");
          }
          


          p_work_time = (cas_card_work_time_t *)param;

          if(DIVI_OK == DIVI_GetSMCWorkTime(&enable,beginTime,endTime))//工作时间段
          {
            CAS_DIVI_ADT_PRINTF("start time: %x-%x-%x, end time: %x-%x-%x\n", 
                                        beginTime[0], beginTime[1],beginTime[2], 
                                        endTime[0], endTime[1], endTime[2]);

            p_card_info->work_time.state = enable;
            p_card_info->work_time.start_hour = BcdToDec(beginTime[0]);
            p_card_info->work_time.start_minute =BcdToDec(beginTime[1]);
            p_card_info->work_time.start_second =BcdToDec(beginTime[2]);
            p_card_info->work_time.end_hour = BcdToDec(endTime[0]);
            p_card_info->work_time.end_minute = BcdToDec(endTime[1]);
            p_card_info->work_time.end_second = BcdToDec(endTime[2]);
            CAS_DIVI_ADT_PRINTF("[DIVI]Work Time status = %d\n",enable);
            CAS_DIVI_ADT_PRINTF("[DIVI]Work Time: %d:%d:%d-%d:%d:%d, ret = %d\n", 
                                  p_card_info->work_time.start_hour,
                                  p_card_info->work_time.start_minute,
                                  p_card_info->work_time.start_second,
                                  p_card_info->work_time.end_hour,
                                  p_card_info->work_time.end_minute,
                                  p_card_info->work_time.end_second,
                                  ret);             
          }
          else
          {
            CAS_DIVI_ADT_PRINTF("[divi], get card worktime error ! \n");
          }

          UsrLevel = DIVI_GetSMCUserLevel();//观看级别

          if(-1 != UsrLevel)
          {
            p_card_info->cas_rating = UsrLevel;
          }
                  
          CAS_DIVI_ADT_PRINTF("UsrLevel = %d\n",UsrLevel);

		  //get mother card 
		  ChildSMCInfo Child_Info;
		  ret = DIVI_GetSMCChildInfo(&Child_Info);
		  if(0 == ret)		
          {         
		    memcpy((char *)(p_card_info->mother_card_id),Child_Info.Main_Card_NO,DIVI_CARD_SN_LEN);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",p_card_info->mother_card_id[0]);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",p_card_info->mother_card_id[1]);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",p_card_info->mother_card_id[2]);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",p_card_info->mother_card_id[3]);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",p_card_info->mother_card_id[4]);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",p_card_info->mother_card_id[5]);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",p_card_info->mother_card_id[6]);
            CAS_DIVI_ADT_PRINTF("[divi],CardID:%X \n",p_card_info->mother_card_id[7]);
          }
          else
          {          
            memset(p_card_info->mother_card_id,0,DIVI_CARD_SN_LEN);
            OS_PRINTF("[divi], get mother card id error ! \n");
          }
          break;
        case CAS_IOCMD_MAIL_HEADER_GET:
          CAS_DIVI_ADT_PRINTF("[DIVI] CAS_IOCMD_MAIL_HEADER_GET \n");
          for(i = 1; i <= MAX_EMAIL_NUM; i++)
          {
              s_mail_flag[i] = 0;
          }

          p_mail_headers = (cas_mail_headers_t *)param;
          err = divi_get_mail_header(&p_mail_headers->max_num, p_mail_headers->p_mail_head);
          CAS_DIVI_ADT_PRINTF("[DIVI] get mail header num: %d \n",p_mail_headers->max_num);
          break;

        case CAS_IOCMD_MAIL_BODY_GET:
          CAS_DIVI_ADT_PRINTF("[DIVI] CAS_IOCMD_MAIL_BODY_GET \n");
          p_mailcontent = (cas_mail_body_t *)param;
          CAS_DIVI_ADT_PRINTF("[DIVI],mail_id = %d \n",p_mailcontent->mail_id);
          err = divi_get_mail_body(p_mailcontent->mail_id, p_mailcontent);
          break;

        case CAS_IOCMD_MAIL_DELETE_BY_INDEX:
          CAS_DIVI_ADT_PRINTF("[DIVI] CAS_IOCMD_MAIL_DELETE_BY_INDEX \n");
          mail_index = *(u32 *)param;
          err = DIVI_DelEmail(mail_index);
          CAS_DIVI_ADT_PRINTF("[delete],mail_id=%d, err = %d \n",mail_index,err);
          s_mail_flag[mail_index] = 0;
          CAS_DIVI_ADT_PRINTF("[delete],s_mail_flag[%d]=%d \n",mail_index,s_mail_flag[mail_index]);
          break;

        case CAS_IOCMD_MAIL_DELETE_ALL:
          CAS_DIVI_ADT_PRINTF("[DIVI] CAS_IOCMD_MAIL_DELETE_ALL \n");

          for(i = 1; i <= MAX_EMAIL_NUM; i++)
          {
              CAS_DIVI_ADT_PRINTF("%d s_mail_flag = %d\n",i,s_mail_flag[i]);
              if(s_mail_flag[i] == 1)
              {
                  CAS_DIVI_ADT_PRINTF("Call DIVI_DelEmail %d start\n",i);
                  DIVI_DelEmail(i);
                  CAS_DIVI_ADT_PRINTF("[DIVI] Delete mail id = %d \n",i);
              }
          }
          break;
        case CAS_IOCMD_OSD_MSG_GET:
          CAS_DIVI_ADT_PRINTF("[DIVI] CAS_IOCMD_OSD_MSG_GET \n");
          break;

        case CAS_IOCMD_OSD_GET:
          CAS_DIVI_ADT_PRINTF("[DIVI] CAS_IOCMD_OSD_GET \n");
          p_msg_info = (msg_info_t *)param;
          memcpy(p_msg_info, &divi_OsdMsg, sizeof(divi_OsdMsg));
          #if 0
          {
            static int index = 0;
            test_osd(index);
            index++;
          }
          #endif
          break;
        case CAS_IOCMD_ENTITLE_INFO_GET:
            CAS_DIVI_ADT_PRINTF("[YXSB] CAS_IOCMD_ENTITLE_INFO_GET \n");
            p_entitles_info = (product_entitles_info_t *)param;
			
            ret = divi_ADT_GetEntitleIDs(p_entitles_info->operator_id, 
                                                               &p_entitles_info->max_num, 
                                                               p_entitles_info->p_entitle_info);
            break;
        case CAS_IOCMD_MON_CHILD_STATUS_GET:
            CAS_DIVI_ADT_PRINTF("[divi] CAS_IOCMD_MON_CHILD_STATUS_GET \n");
            p_feed_info = (chlid_card_status_info *)param;
            ChildSMCInfo ChildInfo;
            ret = DIVI_GetSMCChildInfo(&ChildInfo);
            CAS_DIVI_ADT_PRINTF("[divi], DIVI_GetSMCChildInfo ret = %d ! \n",ret);
            
            if(ret == 0)
            {
              p_feed_info->is_child = 1;
            }
            else if(ret == -2)
            {
              p_feed_info->is_child = 0;
            }

            memcpy(p_feed_info->parent_card_sn,ChildInfo.Main_Card_NO,8);

            CAS_DIVI_ADT_PRINTF("ExpireYear = 0x%x, ExpireMonth = 0x%x, ExpireDate = 0x%x\n",
                                ChildInfo.ExpireYear,ChildInfo.ExpireMonth,ChildInfo.ExpireDate);

            CAS_DIVI_ADT_PRINTF("ExpireHour = 0x%x, ExpireMin = 0x%x, ExpireSecond = 0x%x\n",
                    ChildInfo.ExpireHour,ChildInfo.ExpireMinute,ChildInfo.ExpireSecond);

            CAS_DIVI_ADT_PRINTF("Main_Card_NO : %s\n",ChildInfo.Main_Card_NO);
            CAS_DIVI_ADT_PRINTF("Active_Duration : %s\n",ChildInfo.Active_Duration);
            CAS_DIVI_ADT_PRINTF("Effective_Days = %d\n",ChildInfo.Effective_Days);
            CAS_DIVI_ADT_PRINTF("Lead_Time = %d\n",ChildInfo.Lead_Time);
            CAS_DIVI_ADT_PRINTF("Temp_Active_Duration = %d\n",ChildInfo.Temp_Active_Duration);
            
            break;

        case CAS_IOCMD_MON_CHILD_STATUS_SET:
			CAS_DIVI_ADT_PRINTF("[divi] CAS_IOCMD_MON_CHILD_STATUS_SET param = %d\n",*(u8 *)param);
			{
				static BOOL state = FALSE;
				u8 *child_status = (u8 *)param;
				
				if((*child_status == 1) && (state == FALSE))
				{
					state = TRUE;
					ret = DIVI_Active_ChildCard();
				}
				else if((*child_status == 0) && (state == TRUE))
				{
					state = FALSE;
					ret = DIVI_Finish_ChildCard();
				}

				if(0x01 == ret)
	            {
	              	ret = SUCCESS;
	            }
	            else
	            {
	              	ret = ERR_FAILURE;
	            }
			}
            break;
            
        case CAS_IOCMD_WORK_TIME_SET:
            CAS_DIVI_ADT_PRINTF("[DIVI] CAS_IOCMD_WORK_TIME_SET \n");
            p_work_time = (cas_card_work_time_t *)param;
            ret = divi_ADT_SetWorkTime(p_work_time);
            break;

        case CAS_IOCMD_WORK_TIME_GET:
            {
              u8 beginTime[3] = {0,};
              u8 endTime[3] = {0,};
              u8 enable = 0;
              CAS_DIVI_ADT_PRINTF("[DIVI] CAS_IOCMD_WORK_TIME_GET \n");
              p_work_time = (cas_card_work_time_t *)param;

              ret = DIVI_GetSMCWorkTime(&enable,beginTime,endTime);

              p_work_time->state = enable;
              p_work_time->start_hour = BcdToDec(beginTime[0]);
              p_work_time->start_minute = BcdToDec(beginTime[1]);
              p_work_time->start_second = BcdToDec(beginTime[2]);
              p_work_time->end_hour = BcdToDec(endTime[0]);
              p_work_time->end_minute = BcdToDec(endTime[1]);
              p_work_time->end_second = BcdToDec(endTime[2]);

              CAS_DIVI_ADT_PRINTF("[DIVI]Work Time: %d:%d:%d-%d:%d:%d, ret = %d\n", 
                                    p_work_time->start_hour,
                                    p_work_time->start_minute,
                                    p_work_time->start_second,
                                    p_work_time->end_hour,
                                    p_work_time->end_minute,
                                    p_work_time->end_second,
                                    ret);                          
            }  
            break;
            
        case CAS_IOCMD_RATING_SET:

            CAS_DIVI_ADT_PRINTF("[DIVI] CAS_IOCMD_RATING_SET \n");
            p_rating_set = (cas_rating_set_t *)param;
            
            ret = divi_ADT_SetRating(p_rating_set->pin, p_rating_set->rate);
            break;
            
        case CAS_IOCMD_RATING_GET:            
            p_rating = (u8 *)param;
            UsrLevel = DIVI_GetSMCUserLevel();
            if(-1 != UsrLevel)
            {
              *p_rating = UsrLevel;
            }
            else
            {
              *p_rating = 0xff;
            }

            //test only erase card pair info
            //DIVI_ClearPaired();
            //CAS_DIVI_ADT_PRINTF("CALL DIVI_ClearPaired \n");
            
            CAS_DIVI_ADT_PRINTF("UsrLevel = %d\n",UsrLevel);
            
            CAS_DIVI_ADT_PRINTF("[DIVI] CAS_IOCMD_RATING_GET \n");
            break;
            
        case CAS_IOCMD_PIN_SET:
            CAS_DIVI_ADT_PRINTF("[DIVI] CAS_IOCMD_PIN_SET \n");
            p_pin_modify = (cas_pin_modify_t *)param;
            ret = divi_ADT_ChangeSmartCardPIN(p_pin_modify);
            break;

          case CAS_IOCMD_ZONE_CHECK:

            CAS_DIVI_ADT_PRINTF("[divi] CAS_IOCMD_ZONE_CHECK \n");
            #ifndef ZONE_CHECK
            cas_divi_set_network_id(0xff);
            cas_divi_ZoneCheckEnd();
			break;
            #endif
            p_nim_info = (nim_channel_info_t *)param;
            p_nim_dev = (nim_device_t *)dev_find_identifier(NULL,
                                                                DEV_IDT_TYPE,
                                                                SYS_DEV_TYPE_NIM);
            //lock main freq
            nim_channel_connect(p_nim_dev, p_nim_info, 0);
            if(p_nim_info->lock != 1)
            {
                  CAS_DIVI_ADT_PRINTF("[divi], nim unlock ! \n");
                  //set_event(p_priv, CAS_S_NIM_LOCK_FAIL);
                  //set_event(p_priv, CAS_C_CARD_ZONE_CHECK_OK);
                  cas_divi_set_network_id(0xff);
                  cas_divi_ZoneCheckEnd();
            }
            else
            {
               CAS_DIVI_ADT_PRINTF("zone chek locked!!!! \n");
              divi_ca_nit_state = 0;
              
             //filter nit table                 
              DIVI_TableStart(NIT_TABLE_ID,(const UINT8 *)nit_match,
                              (const UINT8 *)nit_mask,1,NIT_PID,0);
              i = 0;
              while((divi_ca_nit_state == 0) && (i < 500))
              {
                  i++;
                  mtos_task_sleep(10);
              }
              if(divi_ca_nit_state == 1)//get nit
              {
                  CAS_DIVI_ADT_PRINTF("\nGET nit have areacode\n");
              }
              else//nit timeout
              {
                  CAS_DIVI_ADT_PRINTF("\nno nit~~~~~~~~~\n");
                  cas_divi_set_network_id(0xff);
              }
              
              DIVI_CAS_Drv_FreeNITReq();
              cas_divi_ZoneCheckEnd();
            }
            break;

          case CAS_IOCMD_FACTORY_SET:
            CAS_DIVI_ADT_PRINTF("[divi] CAS_IOCMD_FACTORY_SET \n");
            /*divi_cas_all_ecm_req_free();
            divi_cas_all_emm_req_free();
            DIVI_Initialize(0);*/
            break;

     }

         return ret;
}

static void cat_parse_descriptor(UINT8 *buf,s16 length)
{
  UINT8 *p_data = buf;
  UINT8  descriptor_tag = 0;
  UINT8  descriptor_length = 0;
  UINT16  ca_system_id = 0;
  UINT16  emm_pid = 0;

  while(length > 0)
  {
    descriptor_tag = p_data[0];
    descriptor_length = p_data[1];
    p_data += 2;
    length -= 2;
    if(descriptor_length == 0)
    {
      continue;
    }
    switch(descriptor_tag)
    {
      case 0x09://DVB_DIVI_CA:

        ca_system_id = MAKE_WORD(p_data[1], p_data[0]);
        emm_pid = MAKE_WORD(p_data[3],p_data[2] & 0x1F);
        CAS_DIVI_ADT_PRINTF("ca_system_id:0x%X  \n",ca_system_id);
        if(TRUE == DIVI_IsMatchCAID(ca_system_id))
        {
          //mtos_task_sleep(2000);
          CAS_DIVI_ADT_PRINTF("\n[divi] EMM emm pid =0x%x,Desc.CA_system_id =0x%x\n",
          emm_pid,ca_system_id);
          cas_divi_set_emm_pid(emm_pid);
          //XinShiMao_SetEmmPID(CA_INFO_CLEAR,0);
          DIVI_SetEmmPID(0,emm_pid);
        }

        break;
      default:
        break;
    }
    length -= descriptor_length;
    p_data = p_data + descriptor_length;
  }
}

/*!
  parse ca system id

  \param[in] p_buf , the data pointer.
  \param[in] nDescLen , the data len.
  \param[out] pDesc , ca info desc pointer.
  */
U32 ca_desc_parse(UINT8 *pBuf, INT32 nDescLen, divi_ca_desc_t *pDesc)
{
  UINT8 *p_data = pBuf;
  UINT32 nErr = FALSE;

  if(NULL != pBuf && NULL != pDesc)
  {
      pDesc->CA_system_id = p_data[0]<<8|p_data[1];
      CAS_DIVI_ADT_PRINTF("[divi] CA_system_id = 0x%x \n",pDesc->CA_system_id);
      pDesc->CA_PID = (p_data[2]&0x1F)<<8|p_data[3];
      nErr = TRUE;
  }
    return nErr;
}

/*!
  abc
  */
static void cas_divi_parse_cat(u8 *p_buf)
{
  u8 *p_data = p_buf;
  u8 version_number = 0;
  u16 length = 0;

  version_number = (u8)((p_data[5] & 0x3E) >> 1);
  length = (u16)(((p_data[1] & 0x0f) << 8) | p_data[2]);


  CAS_DIVI_ADT_PRINTF("[CAT] length=%d,version_number=%d\n",
                    length,version_number);
  p_data += 8;

  cat_parse_descriptor(p_data, length - 9);

  CAS_DIVI_ADT_PRINTF("\r\nparsing cat over.");
  return;
}

/*!
  parse pmt desc

  \param[in] p_buf , the data pointer.
  \param[in] length , the data len.
  \param[out] program_number .
  */
void pmt_parse_descriptor(UINT8 *p_buf, INT16 length,UINT16 program_number)
{
   UINT8 *p_data = p_buf;
   UINT8 descriptor_tag = 0;
   UINT8 descriptor_length = 0;
   DIVI_CAServiceInfo diviServiceInfo = {0};

  while(length > 0)
  {
    descriptor_tag = p_data[0];
    descriptor_length = p_data[1];

    if(0 == descriptor_length)
    {
      break;
    }

    length -= (descriptor_length + 2);
    p_data += 2;
    switch(descriptor_tag)
    {
      case 0x09:
      {
        divi_ca_desc_t Desc = {0};
        ca_desc_parse(p_data,
        descriptor_length,
        &Desc);

        CAS_DIVI_ADT_PRINTF("[divi] ecm_pid = 0x%x \n",Desc.CA_PID);
        if(TRUE == DIVI_IsMatchCAID(Desc.CA_system_id) &&
           (cas_divi_get_ecm_pid() != Desc.CA_PID))
        {
              //XinShiMao_SetEcmPID(CA_INFO_CLEAR, NULL);
              diviServiceInfo.m_wEcmPid = Desc.CA_PID;
              diviServiceInfo.m_bServiceCount = 1;
              diviServiceInfo.m_wServiceId[0] = program_number;

              CAS_DIVI_ADT_PRINTF("[divi] 22222222222Divi_SetEcmPID:0x%x, service id=0x%x \n",
                    Desc.CA_PID,diviServiceInfo.m_wServiceId[0]);

              cas_divi_set_ecm_pid(Desc.CA_PID);

              DIVI_SetEcmPID(0, &diviServiceInfo);
              cas_divi_set_scramble_flag(1);
         }
         else
         {
              CAS_DIVI_ADT_PRINTF("[divi] not divicas \n");
         }
      }
      default:
        break;
    }
    p_data = p_data + descriptor_length;
  }
}


/*!
  parse pmt table

  \param[in] p_buf , the data pointer.
  */
static void cas_divi_parse_pmt(UINT8 *p_buf)
{
  UINT8   table_id = 0;
  INT16   section_lendivih = 0;
  UINT16  program_number = 0;
  UINT8   version_number = 0;
  UINT16  PCR_PID = 0;
  UINT16  program_info_lendivih = 0;

  UINT8   stream_type = 0;
  UINT16  elementary_PID = 0;
  UINT8   ES_info_lendivih = 0;
  UINT8   *p_data = NULL;

  if(NULL == p_buf)
  {
     CAS_DIVI_ADT_PRINTF("[divi],divi_cas_parse_pmt,param is err! \n");
     MT_ASSERT(0);
  }
  
  p_data = p_buf;

  if(NULL != p_data)
  {
     table_id = p_data[0];
     section_lendivih = (((U16)p_data[1] << 8) | p_data[2]) & 0x0FFF;
     if(section_lendivih > 0)
     {
        program_number = (U16)p_data[3] << 8 | p_data[4];
        version_number = (p_data[5] & 0x3E) >> 1;
        PCR_PID = ((U16)p_data[8] << 8 | p_data[9]) & 0x1FFF;

        program_info_lendivih = ((U16)p_data[10] << 8 | p_data[11]) & 0x0FFF;
        p_data += 12;

        cas_divi_set_scramble_flag(0);

        if(s_divi_entitle_flag == 1)
        {
            cas_divi_cancel_entitle_message();
        }
        s_divi_entitle_flag = 0;
        pmt_parse_descriptor(p_data, program_info_lendivih,program_number);

        p_data += program_info_lendivih;
        section_lendivih -= (13 + program_info_lendivih);//head and crc
        CAS_DIVI_ADT_PRINTF("stream_type=0x%x \n",p_data[0]);

        while(section_lendivih > 0)
        {
            stream_type = p_data[0];
            elementary_PID = ((U16)p_data[1] << 8 | p_data[2]) & 0x1FFF;
            CAS_DIVI_ADT_PRINTF("[divi] stream_type=0x%x\n",stream_type);
            CAS_DIVI_ADT_PRINTF("[divi] elementary_PID=0x%x\n",elementary_PID);
            ES_info_lendivih = ((U16)p_data[3] << 8 | p_data[4]) & 0x0FFF;
            p_data += 5;
            pmt_parse_descriptor(p_data, ES_info_lendivih,program_number);
            p_data += ES_info_lendivih;
            section_lendivih -= (ES_info_lendivih + 5);
        }

       cas_divi_set_server_id(program_number);
    }
  }
}

/*!
  parse table

  \param[in] t_id , table type.
  \param[in] p_buf , data pointer.
  \param[in] p_result , no use.
  */
RET_CODE cas_divi_table_prase(UINT32 t_id, UINT8 *p_buf, UINT32 *p_result)
{
    UINT16 section_len =  MAKE_WORD(p_buf[2], p_buf[1] & 0x0f) + 3;

    CAS_DIVI_ADT_PRINTF("\n  111[divi]  t_id = %d  table_id = %d \n",t_id, p_buf[0]);
    CAS_DIVI_ADT_PRINTF("\n  [divi]  section_len = %d \n",section_len);

    if(t_id == CAS_TID_CAT)
    {
         CAS_DIVI_ADT_PRINTF("cas_divi_table_prase, get cat ! \n");
         memcpy(g_cas_priv.cat_data, p_buf, section_len);
         g_cas_priv.cat_length = section_len;
         cas_divi_parse_cat(g_cas_priv.cat_data);
    }
    else if(t_id == CAS_TID_PMT)
    {
         CAS_DIVI_ADT_PRINTF("cas_divi_table_prase, get pmt ! \n");
         memcpy(g_cas_priv.pmt_data, p_buf, section_len);
         g_cas_priv.pmt_length = section_len;
         cas_divi_parse_pmt(g_cas_priv.pmt_data);
    }
    else if(t_id == CAS_TID_NIT)
    {
         CAS_DIVI_ADT_PRINTF("cas_divi_table_prase, get nit ! \n");
    }
    else
    {
         CAS_DIVI_ADT_PRINTF("cas_divi_table_prase, other table ! \n");
    }

    return SUCCESS;
}

/*!
  parse table

  */
RET_CODE cas_divi_table_resent()
{
    CAS_DIVI_ADT_PRINTF(" [divi] cas_divi_table_resent ! \n");

    if(g_cas_priv.pmt_length != 0)
    {
        CAS_DIVI_ADT_PRINTF("[divi], g_cas_priv.pmt_length != 0 \n");
    }
    if(g_cas_priv.cat_length != 0)
    {
        CAS_DIVI_ADT_PRINTF("[divi], g_cas_priv.cat_length != 0 \n");
    }
    if(g_cas_priv.nit_length != 0)
    {
        CAS_DIVI_ADT_PRINTF("[divi], g_cas_priv.nit_length != 0 \n");
    }
    return SUCCESS;
}

/*!
  remove card

  */
RET_CODE cas_divi_card_remove()
{
	u8 old_key[8] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
	u8 new_key[8] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
	
    CAS_DIVI_ADT_PRINTF(" [divi] divi ca card remove !! \n");

    divi_set_card_status(DIVI_CA_CARD_REMOVE);

    DIVI_SCStatusChange(DIVI_CA_CARD_REMOVE);
	DIVI_SetCW(cas_divi_get_ecm_pid(), old_key, new_key,8, 0);

    return SUCCESS;
}

RET_CODE cas_divi_card_reset(UINT32 slot, card_reset_info_t *p_info)
{
    cas_divi_priv_t *p_priv = NULL;

    CAS_DIVI_ADT_PRINTF("[divi]%s:LINE:%d, get in!\n", __FUNCTION__, __LINE__);
    if (NULL == p_info)
    {
        CAS_DIVI_ADT_PRINTF("[divi]%s:LINE:%d, p_info is NULL!\n", __FUNCTION__, __LINE__);
        return ERR_FAILURE;
    }
    p_priv = g_cas_priv.cam_op[CAS_ID_DIVI].p_priv;

    //p_ds_scard_dev = (scard_device_t *)p_info->p_smc;
    scard_pro_register_op((scard_device_t *)p_info->p_smc);

    p_priv->slot = slot;

    //p_priv->card_status = SMC_CARD_INSERT;
    CAS_DIVI_ADT_PRINTF("\n [divi] DIVI_CA_CARD_INSERT ! \n");
    divi_set_card_status(DIVI_CA_CARD_INSERT);

    DIVI_SCStatusChange(DIVI_CA_CARD_INSERT);


    return SUCCESS;
}


/*!
  abc
  */
RET_CODE cas_divi_attach(cas_module_cfg_t *p_cfg, u32 *p_cam_id)
{
    cas_divi_priv_t *p_priv = NULL;

   g_cas_priv.cam_op[CAS_ID_DIVI].attached = 1;
    g_cas_priv.cam_op[CAS_ID_DIVI].card_reset_by_cam = 1;
   CAS_DIVI_ADT_PRINTF("[divi], attached=%d,  cas_divi_attach ok ! \n",
                                      g_cas_priv.cam_op[CAS_ID_DIVI].attached);

   g_cas_priv.cam_op[CAS_ID_DIVI].func.init
    = cas_divi_init;
   g_cas_priv.cam_op[CAS_ID_DIVI].func.deinit
    = cas_divi_deinit;
   g_cas_priv.cam_op[CAS_ID_DIVI].func.identify
    = cas_divi_identify;
   g_cas_priv.cam_op[CAS_ID_DIVI].func.io_ctrl
    = cas_divi_io_ctrl;
   g_cas_priv.cam_op[CAS_ID_DIVI].func.table_process
    = cas_divi_table_prase;
   g_cas_priv.cam_op[CAS_ID_DIVI].func.table_resent
    = cas_divi_table_resent;

  g_cas_priv.cam_op[CAS_ID_DIVI].func.card_remove = cas_divi_card_remove;
  g_cas_priv.cam_op[CAS_ID_DIVI].func.card_reset = cas_divi_card_reset;

  p_priv = mtos_malloc(sizeof(cas_divi_priv_t));
  if(p_priv ==  NULL)
  {
      CAS_DIVI_ADT_PRINTF("[divi],p_priv malloc error ! \n");
      return ERR_FAILURE;
  }
  g_cas_priv.cam_op[CAS_ID_DIVI].p_priv = (cas_divi_priv_t *)p_priv;
  memset(p_priv, 0x00, sizeof(cas_divi_priv_t));
  CAS_DIVI_ADT_PRINTF("[divi],p_priv add:0x%x \n",p_priv);

  p_priv->flash_size = p_cfg->flash_size;
  p_priv->flash_start_adr = p_cfg->flash_start_adr;
  p_priv->stb_card_pair_size = p_cfg->stb_card_pair_size;
  p_priv->stb_card_pair_start_adr = p_cfg->stb_card_pair_start_adr;
  
  p_priv->nvram_read = p_cfg->nvram_read;
  p_priv->nvram_write = p_cfg->nvram_write;


  *p_cam_id = (UINT32)(&g_cas_priv.cam_op[CAS_ID_DIVI]);


#if 0
  if(XinShiMao_Initialize(0) != SUCCESS)
  {
      CAS_DIVI_ADT_PRINTF("[divi], cas_divi_init error ! \n");
      return ERR_FAILURE;
  }
#endif

  return SUCCESS;
}

