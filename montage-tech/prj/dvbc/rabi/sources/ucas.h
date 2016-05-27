/***********************************************************************************
file:        ucas.h
description: This header file exports universal conditional access system routines.
             It supports  viaccess,irdeto,seca and nagra conditional system.
history:     
author:      freeman
compiler:    ST tool  version 1.62,  1.81  or 1.96
version:     1.4
date:        2003/12/12 
************************************************************************************/
#ifndef __UCAS_H__
#define __UCAS_H__

/* include follow .h files when u include ucas.h
#include "Globdef.h"
#include "UpLink232Def.h"
#include "..\cassi\si_cas.h"
#include "mcard.h"
*/

/*
typedef unsigned char         BYTE;
typedef unsigned short int    WORD;
typedef unsigned int          DWORD;
typedef signed char           SBYTE;
typedef signed short int      SWORD;
typedef signed int            SDWORD;
*/
 
#define MAX_CA_SYSTEM_TABLE  64
#define MAX_ECM_EMM_NUM 8



#define TYPE_VIACCESS2   0x04
#define TYPE_VIACCESS    0x05
#define TYPE_IRDETO      0x06
#define TYPE_SECA        0x01
#define TYPE_SECA2       0x02
#define TYPE_CONAX       0x0B
#define TYPE_CONAX2       0x0C
#define TYPE_CRYPTOWORKS 0x0d
#define TYPE_CRYPTOWORKSHALF 0x0e
#define TYPE_NAGRA       0x18
#define TYPE_NAGRA2      0x19
#define TYPE_BISS        0x26
#define TYPE_IRDETO2	 0x2a
#define TYPE_VIDEOGUARD     0x2b
#define TYPE_VIDEOGUARD2       0x2c

#define isVIACCESS(x)     (x==TYPE_VIACCESS)
#define isVIACCESS2(x)    (x==TYPE_VIACCESS2)
#define isIRDETO(x)       (x==TYPE_IRDETO)
#define isSECA(x)         (x==TYPE_SECA)
#define isNAGRA(x)        (x==TYPE_NAGRA)
#define isNAGRA2(x)       (x==TYPE_NAGRA2)
#define isCONAX(x)        (x==TYPE_CONAX)
#define isBISS(x)         (x==TYPE_BISS)
#define isCRYPTOWORKS(x)   (x==TYPE_CRYPTOWORKS)
#define isSECA2(x)     (x==TYPE_SECA2)
#define isCONAX2(x)     (x==TYPE_CONAX2)
#define isCRYPTOWORKSHALF(x)     (x==TYPE_CRYPTOWORKSHALF)
#define isVIDEOGUARD(x)     (x==TYPE_VIDEOGUARD)
#define isVIDEOGUARD2(x)     (x==TYPE_VIDEOGUARD2)
#define isIRDETO2(x)     (x==TYPE_IRDETO2)


#define okCATYPE(x)   (isVIACCESS(x)||isVIACCESS2(x)||isIRDETO(x)||isSECA(x)||isNAGRA(x)||isNAGRA2(x)||isCONAX(x)||isBISS(x)||isCRYPTOWORKS(x)||isIRDETO2(x)||isSECA2(x)||isCONAX2(x)||isCRYPTOWORKSHALF(x)||isVIDEOGUARD(x)||isVIDEOGUARD2(x))

typedef struct
{
  void *p;
}ecm_entry_t;

typedef struct
{
  void *p;
}emm_entry_t;




typedef struct _KEY_HEADER_INFO_FLASH_150{
  unsigned int     Sync;//always is 0x87654321
  unsigned short   Version;//150 is 1.50
  unsigned short   ViaKeyNr,IrdKeyNr,SecaKeyNr,NagKeyNr,
	               BissKeyNr,ConaxKeyNr,Nag2KeyNr,Via2KeyNr,
				   NagRsaKeyNr,Nag2RsaKeyNr,
				   CryptoWorksKeyNr,Seca2KeyNr,CryptoWorkshalfKeyNr,VideoGuardkeyNr,VideoGuard2keyNr,Conax2KeyNr,
				   Irdeto2Nr,ProviderNameOffset,reserve2,reserve3,reserve4,reserve5,reserve6;
}KEY_HEADER_INFO_FLASH_150;


typedef struct _KEY_HEADER_INFO_SDRAM{
  unsigned int     Sync;//always is 0x87654321
  unsigned short   Version;//150 is 1.50
  unsigned short   ViaKeyNr,IrdKeyNr,SecaKeyNr,NagKeyNr,
	               BissKeyNr,ConaxKeyNr,Nag2KeyNr,Via2KeyNr,
				   NagRsaKeyNr,Nag2RsaKeyNr,
				   CryptoWorksKeyNr,Seca2KeyNr,CryptoWorkshalfKeyNr,VideoGuardkeyNr,VideoGuard2keyNr,Conax2KeyNr,
				   Irdeto2Nr,ProviderNameOffset,reserve2,reserve3,reserve4,reserve5,reserve6;
}KEY_HEADER_INFO_SDRAM;



//op key version1 for viaccess,irdeto,seca,nagra,biss
typedef struct _KEYINFO{
  unsigned int id;  //ident
  unsigned char type;  //  ca system type
  unsigned char keynr; // key number
  unsigned char key[8]; //8 bytes key
}KEYINFO;

//op key version2 for viaccess2,nagra2
typedef struct _KEYINFO2{
  unsigned int id;  //ident
  unsigned char type;  //  ca system type
  unsigned char keynr; // key number
  unsigned char key[16]; //16 bytes key
}KEYINFO2;

 typedef struct _KEYINFO_BISS{
  unsigned int id;  //frequence
  unsigned char type;  //  ca system type
  unsigned char keynr; // reserved
  unsigned int servid; // servid
  unsigned char key[8]; //8 bytes key
}KEYINFO_BISS;



typedef struct {
	int mode;
	unsigned char key[16];
} TPS_STEPINFO;


typedef struct _UCAS_RUN_INFOS
{
	 //unsigned char algo_type;
	 //unsigned char ua[8];
	 //unsigned char sa[8];
     //unsigned short int  ca_sys_id;
     //int ca_flag;
     unsigned int service_id;
     unsigned char ecm_num,current_ecm_idx;
	 struct _ucas_ecm{
		 unsigned short int  ca_sys_id;
		 unsigned short int  pid; 
        	 unsigned short int  type;//0=all,1=video,2=audio
		 unsigned int  ident; 
		 unsigned char priorityed,ignored;
	 }ecm[MAX_ECM_EMM_NUM];
	 unsigned char emm_num,current_emm_idx;
	 struct _ucas_emm{
		 unsigned short int  ca_sys_id;
		 unsigned char ua[8];
		 unsigned char sa[8];
		 unsigned short int  pid; 
		 unsigned int  ident; 
	 }emm[MAX_ECM_EMM_NUM]; 
}UCAS_RUN_INFOS;

#define MAX_UCAS_GROUP_CHANNELS     2 //0=playing,1=pvr

/***********************************************************************************************
routine:       
description:   This is  set descramble key  call back routine  type define. 
               this routine should be realized by thirty part, register it in 
               the routine ucas_initialise.  when there is a control word,
			   the ucas  library  call this routine.  
input:         pEvenCW    Even control word  pointer
               EvenCWLen  Even control word  len      Always is 8
               pOddCW     Odd  control word  pointer
               OddCWLen   Odd  control word  len      Always is 8
output:        none
return:        0=success,-1=fail
************************************************************************************************/
//typedef int (*pCBControlWord_t)(unsigned char *pEvenCW,int EvenCWLen,unsigned char *pOddCW,int OddCWLen);
typedef int (*pCB2ControlWord_t)(ecm_entry_t *pecm_entry);


/***********************************************************************************************
routine:       ucas_initialise
description:   Thirty part must call this routine to initialise ucas library first of all.
input:         pCW_CallBack    set descramble key  call back routine pointer
output:        none
return:        0=success,-1=fail
************************************************************************************************/
//int ucas_initialise(pCBControlWord_t pCW_CallBack);
int ucas2_initialise(pCB2ControlWord_t pCW2_CallBack);

/***********************************************************************************************
routine:       ucas_send_cat
description:   Thirty part must call this routine to  send cat table to ucas library .
input:         pcattable   cat  table  pointer
               catlen      cat  table  total lengh
output:        none
return:        0=success,-1=fail
************************************************************************************************/
//int ucas_send_cat(unsigned char *pcattable,int catlen);
 int ucas2_send_cat(int channel,unsigned char *pcattable,int catlen);


/***********************************************************************************************
routine:       ucas_send_pmt
description:   Thirty part must call this routine to  send pmt table to ucas library .
input:         ppmttable   pmt table  pointer
               pmtlen      pmt table  total lengh
output:        none
return:        0=success,-1=fail
************************************************************************************************/
//int ucas_send_pmt(unsigned char *ppmttable,int pmtlen);
int ucas2_send_pmt(int channel,unsigned char *ppmttable,int pmtlen); 

/***********************************************************************************************
routine:       ucas_get_ecm_num
description:   Thirty part must call this routine to  get current ecm pid number from ucas library.
input:         none
output:        none
return:        ecm pid number
************************************************************************************************/
//int ucas_get_ecm_num(void);

/***********************************************************************************************
routine:       ucas_get_ecm_pid
description:   Thirty part must call this routine to  get  ecm pid  from ucas library.
input:         index     ecm pid index,  0,1,...
               pEcmPid   ecm pid pointer
output:        none
return:        0=success,-1=fail
************************************************************************************************/
//int ucas_get_ecm_pid(int index,unsigned short int *pEcmPid); 
 
 
/***********************************************************************************************
routine:       ucas_get_all_ecm_pid
description:   Thirty part must call this routine to  get  ecm pid  from ucas library.
input:         pEcmPid   ecm pid pointer
               pEcmNum   ecm number
output:        none
return:        0=success,-1=fail
************************************************************************************************/
//int ucas_get_all_ecm_pid(unsigned short int *pEcmPid,unsigned short int *pEcmNum,int ident,char*networkname); 

 
/***********************************************************************************************
routine:       ucas_get_all_ecm_pid
description:   Thirty part must call this routine to  get  ecm pid  from ucas library.
input:         pEcmPid   ecm pid pointer
               pEcmNum   ecm number
output:        none
return:        0=success,-1=fail
************************************************************************************************/
//int ucas_get_ecm_pid_by_netname(unsigned short int *pEcmPid,unsigned short int *pEcmNum,char*networkname); 

/***********************************************************************************************
routine:       ucas_get_all_ecm_pid
description:   Thirty part must call this routine to  get  ecm pid  from ucas library.
input:         pEcmPid   ecm pid pointer
               pEcmNum   ecm number
output:        none
return:        0=success,-1=fail
************************************************************************************************/
//int ucas_get_ecm_pid_by_ident(unsigned short int *pEcmPid,unsigned short int *pEcmNum,int Ident); 





//int ucas_get_emm_num(void);
//int ucas_get_all_emm_pid(unsigned short int *pEmmPid,unsigned short int *pEmmNum,int ident,char*networkname);
//int ucas_get_ca_sys_id(unsigned short int *pCASYSID,unsigned short int emmpid); 


int ucas2_get_run_info(int channel,UCAS_RUN_INFOS **pucas_run_infos);


/***********************************************************************************************
routine:       ucas_send_emm
description:   Thirty part must call this routine to  send  emm table  to ucas library.
input:         pemmtable    emm table pointer
               emmlen       emm table total lengh
               emmpid       emm pid
output:        none
return:        0=success,-1=fail
************************************************************************************************/
//int ucas_send_emm(unsigned char *pemmtable,int emmlen,unsigned short int emmpid);
int ucas_send_emm2(unsigned char *pemmtable,int emmlen,unsigned short int caid,unsigned int ident);
//int ucas2_send_emm(emm_entry_t *pemm_entry);

/***********************************************************************************************
routine:       ucas_send_ecm
description:   Thirty part must call this routine to  send  ecm table  to ucas library.
input:         pecmtable    ecm table pointer
               ecmlen       ecm table total lengh
               ecmpid       ecm pid
output:        none
return:        0=success,-1=fail
************************************************************************************************/
//int ucas_send_ecm(unsigned char *pecmtable,int ecmlen,unsigned short int ecmpid);
//int ucas_send_ecm2(unsigned char *pecmtable,int ecmlen,unsigned short int caid,unsigned int ident);
int ucas2_send_ecm(ecm_entry_t *pecm_entry);
int send_ecm(ecm_entry_t *pecm_entry);
int send_emm(emm_entry_t *pemm_entry);





int ucas_get_cur_ca_type(unsigned char *caType);
int ucas_get_cur_ca_status(unsigned int * casysid,unsigned int* ident,unsigned char* keynr); 
//int ucas_get_caid_byemmpid(unsigned int*sysid,unsigned int*ident,unsigned int emmpid );
//int ucas_get_caid_byecmpid(unsigned int*sysid,unsigned int*ident,unsigned int ecmpid );

//about key routines
int ucas_get_key_infos(unsigned char Type,void **pkeyinfos,unsigned int*keyinfosnum);

int ucas_via_updata_key(unsigned char Type,KEYINFO*key);
int ucas_nag_updata_key(unsigned char Type,int ident,unsigned char*key0,unsigned char*key1);
int ucas_seca_updata_key(unsigned char Type,int ident,unsigned char*key0c,unsigned char*key0d,unsigned char*key0e);
int ucas_lib_info(char**ppinfo);
int ucas_set_cur_tp_info(int Freq,int Symbol,int HorV);
int ucas_set_cur_channel_name(char*ChanName);

//T_CNX_LIB_VERSION_INFO* ucas_get_lib_version_info(void);


int ucas_set_aes_key(unsigned char*paeskey,int len);


typedef struct{
	int PmtState;
	int CatState;
	u16 s_id;
}PmtCatState;

PmtCatState * GetPmtCatState(void);

#endif

