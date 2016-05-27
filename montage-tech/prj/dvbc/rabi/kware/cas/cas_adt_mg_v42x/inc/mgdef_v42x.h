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
/*      mgdef.h                                                             */
/*                                                                          */
/* VERSION                                                                  */
/*      v4.2.3+                                                             */
/*                                                                          */
/* AUTHOR                                                                   */
/*      Eileen Geng                                                         */
/*      Gaston Shi                                                          */
/*                                                                          */
/****************************************************************************/


#ifndef __MG_DEF_H
#define __MG_DEF_H


/* System defines, do NOT change!!! */
/* ===> */
typedef unsigned char				MG_U8;
typedef unsigned short				MG_U16;
typedef unsigned int				MG_U32;
typedef signed char					MG_S8;
typedef signed short				MG_S16;
typedef signed int					MG_S32;
				
#define MG_TRUE						1
#define MG_FALSE					0
#define MG_NULL    					((void *)0)

/* Added at v4.2.0 */
#define MG_CWUPD_BIT_ODD			0x01
#define MG_CWUPD_BIT_EVEN			0x02

#define MG_PROGSTA_BIT_PPVUpd		0x01
#define MG_PROGSTA_BIT_PreAuth		0x04

#define MG_PINERR_STB_PAIRED		0x01
#define MG_PINERR_CARD_PAIRED		0x02
#define MG_PINERR_SC_NOTMATCH		0x03


typedef enum
{
	MG_LIB_TEST,
	MG_LIB_FORMAL
}MG_LIB_STA;

typedef enum
{
	MG_ERR_SCARD_NOT_ACT			=-60,
	MG_ERR_SCARD_DISABLED,
	MG_ERR_ZONE_CODE,
	MG_ERR_AUTHNEARBOOT_REMIND,		/* Added at v4.2.3 */
	MG_ERR_AUTHNEARTIME_REMIND,		/* Added at v4.2.3 */
	MG_ERR_AUTHEXPIRE_REMIND,		/* Added at v4.2.0 */
	MG_ERR_NOT_RECEIVED_SPID		=-50,
	MG_ERR_AUTHENTICATION,
	MG_ERR_NO_MONEY					=-40,
	MG_ERR_NOT_AUTHORED,
	MG_ERR_CARD_LOCKED,
	MG_ERR_APP_LOCKED,
	MG_ERR_SC_PARTNER,
	MG_ERR_PIN_AUTHEN,
	MG_ERR_CARD_NOT_READY,
	MG_ERR_EXTER_AUTHEN,
	MG_ERR_INTER_AUTHEN,
	MG_ERR_GET_CARD_INFO,
	MG_ERR_GET_CARD_DATA,
	MG_ERR_CALCU_GKAK,
	MG_ERR_NEED_GKUPD				=-20,
	MG_ERR_NOREG_CARD,
	MG_ERR_ILLEGAL_CARD,
	MG_ERR_DONOT_TELEVIEW_PPV,
	MG_ERR_QUESTION_TELEVIEW_PPV,
	MG_ERR_CARD_NOTFOUND,
	MG_ERR_CARD_NOTMATCH,			/* Added at v3.1.2 */
	MG_ERR_UPDATE_GK,				/* Added at v3.1.2 */
	MG_ERR_CONTROL_PLAY,			/* Added at v4.2.0 */
	MG_ERR_AUTH_CLOSED,				/* Added at v4.2.0 */
	MG_ERR_AUTH_OVERDUE,			/* Added at v4.2.0 */	
	MG_ERR_AUTH_UPDATE,				/* Added at v4.2.0 */
	MG_ERR_ILLEGAL_AUTH,			/* Added at v4.2.0 */
	MG_ERR_CARD_STATUS_RENEW,		/* Added at v4.2.0 */
	MG_ERR_CARD_OPERATE,			/* Added at v4.2.0 */
	MG_ERR_ILLEGAL_TS,				/* Added at v4.2.0 */
	MG_FAILING						=-1,
	MG_SUCCESS						=0,
	MG_EMM_APP_STOP					=1,
	MG_EMM_APP_RESUME,
	MG_EMM_PROG_UNAUTH,
	MG_EMM_PROG_AUTH,
	MG_EMM_EN_PARTNER,
	MG_EMM_DIS_PARTNER,
	MG_EMM_UPDATE_GNO,
	MG_EMM_CHILD_UNLOCK,
	MG_EMM_MSG,
	MG_EMM_UPDATE_GK,
	MG_EMM_EMAIL,
	MG_EMM_CHARGE,
	MG_EMM_LOADER,
	MG_EMM_FIGEIDENT,
	MG_EMM_SONCARD,
	MG_EMM_URGENT_SERVICE,
	MG_EMM_MODIFY_ZONECODE,
	MG_EMM_UPDATE_COUT,
	MG_EMM_OSD,
	MG_EMM_UPDATE_CRT,
	MG_EMM_NOP,
	MG_EMM_MODIFY_CTRLCODE,			/* Added at v4.2.0 */
	MG_EMM_REFRESH,					/* Added at v4.2.0 */
	MG_EMM_STRATEGY,				/* Added at v4.2.1 */
	MG_EMM_NOT_ENTIRE_MAIL			=100
}MG_STATUS;

/* following func had deleted at v4 */
/* ===> */
/*extern void		MG_Get_Card_Issue_Date(MG_U8 *MG_IssueDate);
extern void			MG_Get_Card_Expire_Date(MG_U8 *MG_ExpireDate);
extern MG_U8		MG_Get_PinErrCount(void);
extern MG_S32		MG_Get_Card_Balance(void);
extern MG_U16		MG_Get_Card_OverDraft(void);
extern MG_U16		MG_Get_Program_Price(MG_U8 MG_PVName);
extern MG_STATUS	MG_Check_Card( MG_U8 *MG_ATR);
extern MG_U32 		MG_Get_ChargeValue(void);
extern MG_STATUS 	MG_CAS_System_Register(MG_U8 *MG_STBSerials, MG_LIB_STA MG_Lib_Status);
extern MG_STATUS	MG_CAS_EMM_E_Process(MG_U8 *MG_EMM_E, MG_U8 *MG_pAKUpdOver);
extern MG_STATUS	MG_CAS_Update_GK(void);*/
/*extern void		MG_Get_CAS_ID(MG_U8 *MG_CASID);*/
/* <=== */

/* following func added at v3.1.2 & v4 */
/* ===> */
extern void 	MG_Info_CardInserted(MG_U8* MG_ATR,MG_U8 *MG_STBSerials);
extern MG_U8 	MG_Get_MSCard_Ext(MG_U8 *MG_MCardSN, MG_U8 *MG_LeftDays);
extern MG_U32	MG_Get_Card_Quota(void);
extern MG_U32	MG_Get_Card_Consume(void);
extern MG_U8	MG_Get_PinErr_Status(MG_U8 *MG_TimesLeft);
extern MG_U8 	MG_Get_Program_CostPerMin(MG_U8 MG_PVName);
extern MG_U32 	MG_Get_ChargeAmount(void);
extern MG_U16	MG_Get_CtrlCode(void);
extern MG_U16	MG_Get_RecentExpireDay(void);
extern MG_U16 	MG_Get_FirstUseDay(void);
/* <=== */

extern void		MG_Get_STB_Serial(MG_U8 *MG_STBSerials);
extern MG_U8	MG_Get_Card_Ready(void);
extern MG_U8	MG_Get_Lock_Status(void);
extern MG_U8	MG_Get_Partner_Status(void);
extern void		MG_Get_Card_SN(MG_U8 *MG_CardSN);
extern MG_U8	MG_Get_Card_Version(void);
extern void		MG_Get_Group_ID(MG_U8 *MG_GID);
extern void		MG_Get_MSCard_Info(MG_U8 *MG_MSCardInfo);
extern MG_U16	MG_Get_Card_Key_Version(void);
extern MG_U16	MG_Get_UCounter(void);
extern MG_U16	MG_Get_GCounter(void);
extern MG_U16	MG_Get_ACounter(void);
extern MG_U8	MG_Get_Current_Program(void);
extern MG_U8	MG_Get_Program_Type(MG_U8 MG_PVName);
extern MG_U8*	MG_Get_Command_Data(void);
extern void 	MG_Get_Odd_CW(MG_U8 *MG_OCW);
extern void		MG_Get_Even_CW(MG_U8 *MG_ECW);
extern void 	MG_Set_CurTeleviewPPVTag(MG_U8 MG_State);
extern MG_U8 	MG_Get_TeleviwPPVTag(MG_U8 MG_PVName);
extern void 	MG_Clean_LastPNo_Info(void);
extern MG_U8*	MG_Get_Mail_Head(void);
extern MG_U8*	MG_Get_Mail_Body(void);
extern void 	MG_Info_CardRemoved(void);
extern void 	MG_Init_CAS(MG_LIB_STA MG_Lib_Status);
extern void 	MG_Get_Card_ID(MG_U8 *MG_CardID);
extern MG_U8 	MG_Get_Program_Auth(MG_U8 MG_PVName);
extern MG_U8* 	MG_Get_LoaderInfo(void);
extern MG_U16	MG_Get_CASoft_Version(void);
extern void		MG_Get_ServiceID(MG_U8 *MG_ServiceID);
extern MG_U8	MG_Get_ZoneCode(void);
extern void		MG_Get_EigenValue(MG_U8 *MG_Evalue);

/*extern MG_STATUS MG_CAS_EMM_GA_Process(MG_U8 *MG_EMM_C, MG_U8 *MG_pEMMResult);*/
extern MG_STATUS MG_CAS_EMM_U_Process(MG_U8 *MG_EMM_U, MG_U8 *MG_pEMMResult);
extern MG_STATUS MG_CAS_EMM_G_Process(MG_U8 *MG_EMM_G, MG_U8 *MG_pEMMResult);
extern MG_STATUS MG_CAS_EMM_A_Process(MG_U8 *MG_EMM_A, MG_U8 *MG_pEMMResult);
extern MG_STATUS MG_CAS_EMM_C_Process(MG_U8 *MG_EMM_C, MG_U8 *MG_pEMMResult);

/*extern MG_STATUS MG_CAS_ECM_Process(MG_U8 *MG_ECM, MG_U8 *MG_pCWUpd, MG_U8 *MG_pBalUpd, MG_U8 MG_ZoneCode);*/
extern MG_STATUS MG_CAS_ECM_Process(MG_U8 *MG_ECM, MG_U8 MG_ZoneCode, MG_U8 *MG_pCWUpd, MG_U8 *MG_pProgSta);/* Modified at v4.2.0 */
/* <=== */


/* Added self defines here */
/* Eileen Geng */
/*#define MG_EMM_GA_Process		MG_CAS_EMM_GA_Process*/
#define MG_EMM_U_Process		MG_CAS_EMM_U_Process
#define MG_EMM_G_Process		MG_CAS_EMM_G_Process
#define MG_EMM_A_Process		MG_CAS_EMM_A_Process
#define MG_EMM_C_Process		MG_CAS_EMM_C_Process
#define MG_ECM_Process			MG_CAS_ECM_Process
/* Gaston Shi */


#endif

