/***************************************************************************************



****************************************************************************************/
// system
#include "ui_common.h"

// c lib
#include "stdio.h"
#include "ctype.h"
#include "stdlib.h"
#include "string.h"

#include "charsto.h"

#include "ucas.h"
#include "ucas_key.h"
#include "data_manager.h"

//#define UCAS_DEBUG

#ifdef UCAS_DEBUG
#define DBG_Print    OS_PRINTF
#else
#define DBG_Print DUMMY_PRINTF
#endif

/*----------------------------------------------------------*/

static KEY_HEADER_INFO_SDRAM g_keyheaderinfos;
static KEYINFO    g_ViaccessKey[VIACCESS_MAX_KEYINFOS];
static KEYINFO    g_IrdetoKey[IRDETO_MAX_KEYINFOS];
static KEYINFO    g_SecaKey[SECA_MAX_KEYINFOS];
static KEYINFO    g_NagraKey[NAGRA_MAX_KEYINFOS];
static KEYINFO  g_ConaxKey[CONAX_MAX_KEYINFOS];
static KEYINFO  g_CryptoWorkshalfKey[CRYPTOWORKSHALF_MAX_KEYINFOS];
static KEYINFO  g_VideoGuardKey[VIDEOGUARD_MAX_KEYINFOS];
static KEYINFO_BISS    g_BissKey[BISS_MAX_KEYINFOS];
static KEYINFO2   g_Viaccess2Key[VIACCESS2_MAX_KEYINFOS];
static KEYINFO2   g_Nagra2Key[NAGRA2_MAX_KEYINFOS];
static KEYINFO2   g_CryptoWorksKey[CRYPTOWORKS_MAX_KEYINFOS];
static KEYINFO2   g_Irdeto2Key[IRDETO2_MAX_KEYINFOS];
static KEYINFO2    g_Seca2Key[SECA2_MAX_KEYINFOS];
static KEYINFO2  g_Conax2Key[CONAX2_MAX_KEYINFOS];
static KEYINFO2  g_VideoGuard2Key[VIDEOGUARD2_MAX_KEYINFOS];

typedef struct ca_key{
 KEY_HEADER_INFO_SDRAM g_keyheaderinfos;
 KEYINFO    g_ViaccessKey[VIACCESS_MAX_KEYINFOS];
 KEYINFO    g_IrdetoKey[IRDETO_MAX_KEYINFOS];
 KEYINFO    g_SecaKey[SECA_MAX_KEYINFOS];
 KEYINFO    g_NagraKey[NAGRA_MAX_KEYINFOS];
 KEYINFO  g_ConaxKey[CONAX_MAX_KEYINFOS];
 KEYINFO  g_CryptoWorkshalfKey[CRYPTOWORKSHALF_MAX_KEYINFOS];
 KEYINFO  g_VideoGuardKey[VIDEOGUARD_MAX_KEYINFOS];
 KEYINFO_BISS    g_BissKey[BISS_MAX_KEYINFOS];
 KEYINFO2   g_Viaccess2Key[VIACCESS2_MAX_KEYINFOS];
 KEYINFO2   g_Nagra2Key[NAGRA2_MAX_KEYINFOS];
 KEYINFO2   g_CryptoWorksKey[CRYPTOWORKS_MAX_KEYINFOS];
 KEYINFO2   g_Irdeto2Key[IRDETO2_MAX_KEYINFOS];
 KEYINFO2    g_Seca2Key[SECA2_MAX_KEYINFOS];
 KEYINFO2  g_Conax2Key[CONAX2_MAX_KEYINFOS];
 KEYINFO2  g_VideoGuard2Key[VIDEOGUARD2_MAX_KEYINFOS];
}key_ca_description1;


static int g_CurKeyMaxNr;
static key_ca_description key_info;


int ucas_get_key_infos(unsigned char Type,void **pkeyinfos,unsigned int*keyinfosnum)
{
	*pkeyinfos=0;
	*keyinfosnum=0;

#if (ONLY_ENABLE_BISS == 1)
	if (Type != TYPE_BISS) {
		*pkeyinfos=NULL;
		*keyinfosnum=0;
		g_CurKeyMaxNr=0;
		return 0;
	}
#endif
    sys_status_load_ucaskey();
    sys_status_get_key_info(&key_info);
	switch(Type)
	{
	case TYPE_VIACCESS2:
		*pkeyinfos= key_info.g_Viaccess2Key;
		*keyinfosnum= key_info.g_keyheaderinfos.Via2KeyNr;
		g_CurKeyMaxNr=VIACCESS2_MAX_KEYINFOS;
		break;
	case TYPE_VIACCESS:
		*pkeyinfos=key_info.g_ViaccessKey;
		*keyinfosnum=key_info.g_keyheaderinfos.ViaKeyNr;
		g_CurKeyMaxNr=VIACCESS_MAX_KEYINFOS;
		break;
	case TYPE_IRDETO:
		*pkeyinfos=key_info.g_IrdetoKey;
		*keyinfosnum=key_info.g_keyheaderinfos.IrdKeyNr;
		g_CurKeyMaxNr=IRDETO_MAX_KEYINFOS;
		break;
	case TYPE_SECA:
		*pkeyinfos=key_info.g_SecaKey;
		*keyinfosnum=key_info.g_keyheaderinfos.SecaKeyNr;
		g_CurKeyMaxNr=SECA_MAX_KEYINFOS;
		break;
	case TYPE_NAGRA:
		*pkeyinfos=key_info.g_NagraKey;
		*keyinfosnum=key_info.g_keyheaderinfos.NagKeyNr;
		g_CurKeyMaxNr=NAGRA_MAX_KEYINFOS;
		break;
	case TYPE_NAGRA2:
		*pkeyinfos=key_info.g_Nagra2Key;
		*keyinfosnum=key_info.g_keyheaderinfos.Nag2KeyNr;
		g_CurKeyMaxNr=NAGRA2_MAX_KEYINFOS;
		break;
	case TYPE_CONAX:
		*pkeyinfos=key_info.g_ConaxKey;
        *keyinfosnum=key_info.g_keyheaderinfos.ConaxKeyNr;//g_keyheaderinfos.ConaxKeyNr;
		g_CurKeyMaxNr=CONAX_MAX_KEYINFOS;
		break;
	case TYPE_BISS:
		*pkeyinfos=key_info.g_BissKey;
		*keyinfosnum=key_info.g_keyheaderinfos.BissKeyNr;
		g_CurKeyMaxNr=BISS_MAX_KEYINFOS;
		break;
	case TYPE_CRYPTOWORKS:
		*pkeyinfos=key_info.g_CryptoWorksKey;
		*keyinfosnum=key_info.g_keyheaderinfos.CryptoWorksKeyNr;
		g_CurKeyMaxNr=CRYPTOWORKS_MAX_KEYINFOS;
		break;
	case TYPE_IRDETO2:
		*pkeyinfos=key_info.g_Irdeto2Key;
		*keyinfosnum=key_info.g_keyheaderinfos.Irdeto2Nr;
		g_CurKeyMaxNr=IRDETO2_MAX_KEYINFOS;
		break;
   case TYPE_CONAX2:
		*pkeyinfos=key_info.g_Conax2Key;
        *keyinfosnum=key_info.g_keyheaderinfos.Conax2KeyNr;//g_keyheaderinfos.ConaxKeyNr;
		g_CurKeyMaxNr=CONAX2_MAX_KEYINFOS;
		break;
   case TYPE_SECA2:
		*pkeyinfos=key_info.g_Seca2Key;
        *keyinfosnum=key_info.g_keyheaderinfos.Seca2KeyNr;//g_keyheaderinfos.ConaxKeyNr;
		g_CurKeyMaxNr=SECA2_MAX_KEYINFOS;
		break;
    case TYPE_CRYPTOWORKSHALF:
		*pkeyinfos=key_info.g_CryptoWorkshalfKey;
        *keyinfosnum=key_info.g_keyheaderinfos.CryptoWorkshalfKeyNr;//g_keyheaderinfos.ConaxKeyNr;
		g_CurKeyMaxNr=CRYPTOWORKSHALF_MAX_KEYINFOS;
		break;
    case TYPE_VIDEOGUARD:
		*pkeyinfos=key_info.g_VideoGuardKey;
        *keyinfosnum=key_info.g_keyheaderinfos.VideoGuardkeyNr;//g_keyheaderinfos.ConaxKeyNr;
		g_CurKeyMaxNr=VIDEOGUARD_MAX_KEYINFOS;
		break;
    case TYPE_VIDEOGUARD2:
		*pkeyinfos=key_info.g_VideoGuard2Key;
        *keyinfosnum=key_info.g_keyheaderinfos.VideoGuard2keyNr;//g_keyheaderinfos.ConaxKeyNr;
		g_CurKeyMaxNr=VIDEOGUARD2_MAX_KEYINFOS;
		break;
	}
	return 0;
}

int ucas_get_key_temp_infos(unsigned char Type,void **pkeyinfos,unsigned int*keyinfosnum)
{
	*pkeyinfos=0;
	*keyinfosnum=0;

#if (ONLY_ENABLE_BISS == 1)
	if (Type != TYPE_BISS) {
		*pkeyinfos=NULL;
		*keyinfosnum=0;
		g_CurKeyMaxNr=0;
		return 0;
	}
#endif
    sys_status_get_key_info(&key_info);
	switch(Type)
	{
	case TYPE_VIACCESS2:
		*pkeyinfos= key_info.g_Viaccess2Key;
		*keyinfosnum= key_info.g_keyheaderinfos.Via2KeyNr;
		g_CurKeyMaxNr=VIACCESS2_MAX_KEYINFOS;
		break;
	case TYPE_VIACCESS:
		*pkeyinfos=key_info.g_ViaccessKey;
		*keyinfosnum=key_info.g_keyheaderinfos.ViaKeyNr;
		g_CurKeyMaxNr=VIACCESS_MAX_KEYINFOS;
		break;
	case TYPE_IRDETO:
		*pkeyinfos=key_info.g_IrdetoKey;
		*keyinfosnum=key_info.g_keyheaderinfos.IrdKeyNr;
		g_CurKeyMaxNr=IRDETO_MAX_KEYINFOS;
		break;
	case TYPE_SECA:
		*pkeyinfos=key_info.g_SecaKey;
		*keyinfosnum=key_info.g_keyheaderinfos.SecaKeyNr;
		g_CurKeyMaxNr=SECA_MAX_KEYINFOS;
		break;
	case TYPE_NAGRA:
		*pkeyinfos=key_info.g_NagraKey;
		*keyinfosnum=key_info.g_keyheaderinfos.NagKeyNr;
		g_CurKeyMaxNr=NAGRA_MAX_KEYINFOS;
		break;
	case TYPE_NAGRA2:
		*pkeyinfos=key_info.g_Nagra2Key;
		*keyinfosnum=key_info.g_keyheaderinfos.Nag2KeyNr;
		g_CurKeyMaxNr=NAGRA2_MAX_KEYINFOS;
		break;
	case TYPE_CONAX:
		*pkeyinfos=key_info.g_ConaxKey;
        *keyinfosnum=key_info.g_keyheaderinfos.ConaxKeyNr;//g_keyheaderinfos.ConaxKeyNr;
		g_CurKeyMaxNr=CONAX_MAX_KEYINFOS;
		break;
	case TYPE_BISS:
		*pkeyinfos=key_info.g_BissKey;
		*keyinfosnum=key_info.g_keyheaderinfos.BissKeyNr;
		g_CurKeyMaxNr=BISS_MAX_KEYINFOS;
		break;
	case TYPE_CRYPTOWORKS:
		*pkeyinfos=key_info.g_CryptoWorksKey;
		*keyinfosnum=key_info.g_keyheaderinfos.CryptoWorksKeyNr;
		g_CurKeyMaxNr=CRYPTOWORKS_MAX_KEYINFOS;
		break;
	case TYPE_IRDETO2:
		*pkeyinfos=key_info.g_Irdeto2Key;
		*keyinfosnum=key_info.g_keyheaderinfos.Irdeto2Nr;
		g_CurKeyMaxNr=IRDETO2_MAX_KEYINFOS;
		break;
   case TYPE_CONAX2:
		*pkeyinfos=key_info.g_Conax2Key;
        *keyinfosnum=key_info.g_keyheaderinfos.Conax2KeyNr;//g_keyheaderinfos.ConaxKeyNr;
		g_CurKeyMaxNr=CONAX2_MAX_KEYINFOS;
		break;
   case TYPE_SECA2:
		*pkeyinfos=key_info.g_Seca2Key;
        *keyinfosnum=key_info.g_keyheaderinfos.Seca2KeyNr;//g_keyheaderinfos.ConaxKeyNr;
		g_CurKeyMaxNr=SECA2_MAX_KEYINFOS;
		break;
    case TYPE_CRYPTOWORKSHALF:
		*pkeyinfos=key_info.g_CryptoWorkshalfKey;
        *keyinfosnum=key_info.g_keyheaderinfos.CryptoWorkshalfKeyNr;//g_keyheaderinfos.ConaxKeyNr;
		g_CurKeyMaxNr=CRYPTOWORKSHALF_MAX_KEYINFOS;
		break;
    case TYPE_VIDEOGUARD:
		*pkeyinfos=key_info.g_VideoGuardKey;
        *keyinfosnum=key_info.g_keyheaderinfos.VideoGuardkeyNr;//g_keyheaderinfos.ConaxKeyNr;
		g_CurKeyMaxNr=VIDEOGUARD_MAX_KEYINFOS;
		break;
    case TYPE_VIDEOGUARD2:
		*pkeyinfos=key_info.g_VideoGuard2Key;
        *keyinfosnum=key_info.g_keyheaderinfos.VideoGuard2keyNr;//g_keyheaderinfos.ConaxKeyNr;
		g_CurKeyMaxNr=VIDEOGUARD2_MAX_KEYINFOS;
		break;
	}
	return 0;
}

int ucas_get_edit_keyinfo(unsigned char Type,void**editkeyinfos,unsigned int*editnum)
{
    ucas_get_key_infos(Type,editkeyinfos,editnum);
	if((int)(*editnum)<g_CurKeyMaxNr)
	  (*editnum)+=1;
	return 0;
}
#define KEY_DATA_ADDR 0x3E0000

int ucas_load_key(unsigned char*pFlashAddr,int FlashLen)
{
	int error=-1;
	static int loaded=0;
   static int key_addr_offset = 0;
	int KeyPos=0;
 //  void *p_norf_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
    unsigned char* pFlashKeyInfo=(void*)pFlashAddr;
	KEY_HEADER_INFO_SDRAM* pkeyheader=(KEY_HEADER_INFO_SDRAM*)pFlashAddr;
    KEYINFO*  pkeyinfo;


//    KEYINFO2  keyinfo2;
    //KEYINFO_N_RSA   keyinfo_nrsa;
    //KEYINFO_N2_RSA  keyinfo_n2rsa;
    //KEYINFO_N2_EMM  keyinfo_n2EMM;
    unsigned short   ViaKeyNr=0,IrdKeyNr=0,SecaKeyNr=0,NagKeyNr=0,
	               BissKeyNr=0,ConaxKeyNr=0,Nag2KeyNr=0,Via2KeyNr=0,
				   Conax2KeyNr=0,Seca2KeyNr=0,CryptoWorkshalfKeyNr=0,
				   VideoGuardkeyNr=0,VideoGuard2keyNr=0,
				   CryptoWorksKeyNr=0,Irdeto2KeyNr=0;
   void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);

   MT_ASSERT(p_dm_handle != NULL);

    if(!loaded)
	{
	   //g_pProvider_Name=(PROVIDER_NAME* )(pFlashAddr+name_addr_offset);

       memset(&g_keyheaderinfos,0x00,sizeof(g_keyheaderinfos));

	   memset(g_ViaccessKey,0xff,sizeof(g_ViaccessKey));
	   memset(g_IrdetoKey,0xff,sizeof(g_IrdetoKey));
	   memset(g_SecaKey,0xff,sizeof(g_SecaKey));
	   memset(g_NagraKey,0xff,sizeof(g_NagraKey));
	   memset(g_BissKey,0xff,sizeof(g_BissKey));
	   memset(g_Viaccess2Key,0xff,sizeof(g_Viaccess2Key));
	   memset(g_Nagra2Key,0xff,sizeof(g_Nagra2Key));
       memset(g_CryptoWorksKey,0xff,sizeof(g_CryptoWorksKey));
	   memset(g_Irdeto2Key,0xff,sizeof(g_Irdeto2Key));
     	 memset(g_Seca2Key,0xff,sizeof(g_Seca2Key));
	   memset(g_CryptoWorkshalfKey,0xff,sizeof(g_CryptoWorkshalfKey));
	   memset(g_VideoGuardKey,0xff,sizeof(g_VideoGuardKey));
	   memset(g_Conax2Key,0xff,sizeof(g_Conax2Key));
	   memset(g_VideoGuard2Key,0xff,sizeof(g_VideoGuard2Key));
     memset(g_ConaxKey,0xff,sizeof(g_ConaxKey));




     // charsto_erase(p_norf_dev,KEY_DATA_ADDR,1);
      dm_direct_erase(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(key_ca_description1));

	  if( (pkeyheader->Sync==0x87654321)&&(pkeyheader->Version>=120) ){
           //new version >= v1.20
		   memcpy(&g_keyheaderinfos,pkeyheader,sizeof(KEY_HEADER_INFO_SDRAM));
		   KeyPos+=sizeof(KEY_HEADER_INFO_SDRAM);
	   }

		   while(KeyPos<(FlashLen-4))
		   {
			   pkeyinfo=(KEYINFO*)(pFlashKeyInfo+KeyPos);
			   switch(pkeyinfo->type)
				{
				case TYPE_VIACCESS2:
						if(Via2KeyNr<VIACCESS2_MAX_KEYINFOS)
						    memcpy(&g_Viaccess2Key[Via2KeyNr++],pkeyinfo,sizeof(g_Viaccess2Key[0]));
					  KeyPos+=sizeof(g_Viaccess2Key[0]);
					break;
				case TYPE_VIACCESS:
						if(ViaKeyNr<VIACCESS_MAX_KEYINFOS)
						    memcpy(&g_ViaccessKey[ViaKeyNr++],pkeyinfo,sizeof(g_ViaccessKey[0]));
						KeyPos+=sizeof(g_ViaccessKey[0]);
					break;
				case TYPE_IRDETO:
					if(IrdKeyNr<IRDETO_MAX_KEYINFOS)
					    memcpy(&g_IrdetoKey[IrdKeyNr++],pkeyinfo,sizeof(g_IrdetoKey[0]));
					KeyPos+=sizeof(g_IrdetoKey[0]);
					break;
				case TYPE_SECA:
					if(SecaKeyNr<SECA_MAX_KEYINFOS)
					    memcpy(&g_SecaKey[SecaKeyNr++],pkeyinfo,sizeof(g_SecaKey[0]));
					KeyPos+=sizeof(g_SecaKey[0]);
					break;
				case TYPE_NAGRA:
						if(NagKeyNr<NAGRA_MAX_KEYINFOS)
						  memcpy(&g_NagraKey[NagKeyNr++],pkeyinfo,sizeof(g_NagraKey[0]));
						KeyPos+=sizeof(g_NagraKey[0]);
					break;
				case TYPE_NAGRA2:
					if(Nag2KeyNr<NAGRA2_MAX_KEYINFOS)
					    memcpy(&g_Nagra2Key[Nag2KeyNr++],pkeyinfo,sizeof(g_Nagra2Key[0]));
					KeyPos+=sizeof(g_Nagra2Key[0]);
					break;
				case TYPE_CONAX:
					if(ConaxKeyNr<CONAX_MAX_KEYINFOS)
					    memcpy(&g_ConaxKey[ConaxKeyNr++],pkeyinfo,sizeof(g_ConaxKey[0]));
					KeyPos+=sizeof(g_ConaxKey[0]);
					break;
				case TYPE_BISS:
					if(BissKeyNr<BISS_MAX_KEYINFOS)
					   memcpy(&g_BissKey[BissKeyNr++],pkeyinfo,sizeof(g_BissKey[0]));
					KeyPos+=sizeof(g_BissKey[0]);
					break;
				case TYPE_CRYPTOWORKS:
					if(CryptoWorksKeyNr<CRYPTOWORKS_MAX_KEYINFOS)
					    memcpy(&g_CryptoWorksKey[CryptoWorksKeyNr++],pkeyinfo,sizeof(g_CryptoWorksKey[0]));
					KeyPos+=sizeof(g_CryptoWorksKey[0]);
					break;
				case TYPE_IRDETO2:
					if(Irdeto2KeyNr<IRDETO2_MAX_KEYINFOS)
					    memcpy(&g_Irdeto2Key[Irdeto2KeyNr++],pkeyinfo,sizeof(g_Irdeto2Key[0]));
					KeyPos+=sizeof(g_Irdeto2Key[0]);
					break;
				case TYPE_SECA2:
					if(Seca2KeyNr<SECA2_MAX_KEYINFOS)
					    memcpy(&g_Seca2Key[Seca2KeyNr++],pkeyinfo,sizeof(g_Seca2Key[0]));
					KeyPos+=sizeof(g_Seca2Key[0]);
					break;
          	case TYPE_CONAX2:
					if(Conax2KeyNr<CONAX2_MAX_KEYINFOS)
					    memcpy(&g_Conax2Key[Conax2KeyNr++],pkeyinfo,sizeof(g_Conax2Key[0]));
					KeyPos+=sizeof(g_Conax2Key[0]);
					break;
            case TYPE_CRYPTOWORKSHALF:
					if(CryptoWorkshalfKeyNr<CRYPTOWORKSHALF_MAX_KEYINFOS)
					    memcpy(&g_CryptoWorkshalfKey[CryptoWorkshalfKeyNr++],pkeyinfo,sizeof(g_CryptoWorkshalfKey[0]));
					KeyPos+=sizeof(g_CryptoWorkshalfKey[0]);
					break;
            case TYPE_VIDEOGUARD:
					if(VideoGuardkeyNr<VIDEOGUARD_MAX_KEYINFOS)
					    memcpy(&g_VideoGuardKey[VideoGuardkeyNr++],pkeyinfo,sizeof(g_VideoGuardKey[0]));
					KeyPos+=sizeof(g_VideoGuardKey[0]);
					break;
            case TYPE_VIDEOGUARD2:
					if(VideoGuard2keyNr<VIDEOGUARD2_MAX_KEYINFOS)
					    memcpy(&g_VideoGuard2Key[VideoGuard2keyNr++],pkeyinfo,sizeof(g_VideoGuard2Key[0]));
					KeyPos+=sizeof(g_VideoGuard2Key[0]);
					break;
                default:
					break;
				}
		   }

	   //
		  g_keyheaderinfos.ViaKeyNr=ViaKeyNr;
		  g_keyheaderinfos.IrdKeyNr=IrdKeyNr;
		  g_keyheaderinfos.SecaKeyNr=SecaKeyNr;
		  g_keyheaderinfos.NagKeyNr=NagKeyNr;
		  g_keyheaderinfos.BissKeyNr=BissKeyNr;
		  g_keyheaderinfos.ConaxKeyNr=ConaxKeyNr;
		  g_keyheaderinfos.Nag2KeyNr=Nag2KeyNr;
		  g_keyheaderinfos.Via2KeyNr=Via2KeyNr;
		  g_keyheaderinfos.CryptoWorksKeyNr=CryptoWorksKeyNr;
		  g_keyheaderinfos.Irdeto2Nr=Irdeto2KeyNr;
      	  g_keyheaderinfos.Conax2KeyNr=Conax2KeyNr;
      	  g_keyheaderinfos.Seca2KeyNr=Seca2KeyNr;
      	  g_keyheaderinfos.CryptoWorkshalfKeyNr=CryptoWorkshalfKeyNr;
        g_keyheaderinfos.VideoGuardkeyNr=VideoGuardkeyNr;
        g_keyheaderinfos.VideoGuard2keyNr=VideoGuard2keyNr;




       dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(KEY_HEADER_INFO_SDRAM),(u8 *)&g_keyheaderinfos);
       key_addr_offset += sizeof(KEY_HEADER_INFO_SDRAM);

       dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_ViaccessKey[0]) *VIACCESS_MAX_KEYINFOS ,(u8 *)&g_ViaccessKey);
       key_addr_offset += sizeof(g_ViaccessKey[0]) * VIACCESS_MAX_KEYINFOS;

       dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_IrdetoKey[0]) *IRDETO_MAX_KEYINFOS ,(u8 *)&g_IrdetoKey);
       key_addr_offset += sizeof(g_IrdetoKey[0]) * IRDETO_MAX_KEYINFOS;

      dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_SecaKey[0]) *SECA_MAX_KEYINFOS ,(u8 *)&g_SecaKey);
       key_addr_offset += sizeof(g_SecaKey[0]) * SECA_MAX_KEYINFOS;

       dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_NagraKey[0]) *NAGRA_MAX_KEYINFOS ,(u8 *)&g_NagraKey);
       key_addr_offset += sizeof(g_NagraKey[0]) * NAGRA_MAX_KEYINFOS;

       dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_ConaxKey[0]) *CONAX_MAX_KEYINFOS ,(u8 *)&g_ConaxKey);
       key_addr_offset += sizeof(g_ConaxKey[0]) * CONAX_MAX_KEYINFOS;

       dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_CryptoWorkshalfKey[0]) *CRYPTOWORKSHALF_MAX_KEYINFOS ,(u8 *)&g_CryptoWorkshalfKey);
       key_addr_offset += sizeof(g_CryptoWorkshalfKey[0]) * CRYPTOWORKSHALF_MAX_KEYINFOS;

       dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_VideoGuardKey[0]) *VIDEOGUARD_MAX_KEYINFOS ,(u8 *)&g_VideoGuardKey);
       key_addr_offset += sizeof(g_VideoGuardKey[0]) * VIDEOGUARD_MAX_KEYINFOS;

       dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_BissKey[0]) *BISS_MAX_KEYINFOS ,(u8 *)&g_BissKey);
       key_addr_offset += sizeof(g_BissKey[0]) * BISS_MAX_KEYINFOS;

       dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_Viaccess2Key[0]) *VIACCESS2_MAX_KEYINFOS ,(u8 *)&g_Viaccess2Key);
       key_addr_offset += sizeof(g_Viaccess2Key[0]) * VIACCESS2_MAX_KEYINFOS;

        dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_Nagra2Key[0]) *NAGRA2_MAX_KEYINFOS ,(u8 *)&g_Nagra2Key);
       key_addr_offset += sizeof(g_Nagra2Key[0]) * NAGRA2_MAX_KEYINFOS;

        dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_CryptoWorksKey[0]) *CRYPTOWORKS_MAX_KEYINFOS ,(u8 *)&g_CryptoWorksKey);
       key_addr_offset += sizeof(g_CryptoWorksKey[0]) * CRYPTOWORKS_MAX_KEYINFOS;

        dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_Irdeto2Key[0]) *IRDETO2_MAX_KEYINFOS ,(u8 *)&g_Irdeto2Key);
       key_addr_offset += sizeof(g_Irdeto2Key[0]) * IRDETO2_MAX_KEYINFOS;

       dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_Seca2Key[0]) *SECA2_MAX_KEYINFOS ,(u8 *)&g_Seca2Key);
       key_addr_offset += sizeof(g_Seca2Key[0]) * SECA2_MAX_KEYINFOS;

       dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_Conax2Key[0]) *CONAX2_MAX_KEYINFOS ,(u8 *)&g_Conax2Key);
       key_addr_offset += sizeof(g_Conax2Key[0]) * CONAX2_MAX_KEYINFOS;

       dm_direct_write(p_dm_handle, BSDATA_BLOCK_ID, key_addr_offset, sizeof(g_VideoGuard2Key[0]) *VIDEOGUARD2_MAX_KEYINFOS ,(u8 *)&g_VideoGuard2Key);
       key_addr_offset += sizeof(g_VideoGuard2Key[0]) * VIDEOGUARD2_MAX_KEYINFOS;

       error=0;
	}
    key_addr_offset = 0;
	return error;
}

int ucas_get_key_total_number(void)
{
	return(g_keyheaderinfos.ViaKeyNr+g_keyheaderinfos.IrdKeyNr+
		   g_keyheaderinfos.SecaKeyNr+g_keyheaderinfos.NagKeyNr+
		   g_keyheaderinfos.BissKeyNr+g_keyheaderinfos.ConaxKeyNr+
		   g_keyheaderinfos.Nag2KeyNr+g_keyheaderinfos.Via2KeyNr+
		   g_keyheaderinfos.CryptoWorksKeyNr+g_keyheaderinfos.Seca2KeyNr+
		   g_keyheaderinfos.Irdeto2Nr+g_keyheaderinfos.CryptoWorkshalfKeyNr+g_keyheaderinfos.Conax2KeyNr+g_keyheaderinfos.VideoGuardkeyNr+g_keyheaderinfos.VideoGuard2keyNr
		   );
}



static CUR_KEY_INFO CurKeyInfo;

void ucas_get_cur_keyinfo(CUR_KEY_INFO * keyinfo)
  {
	unsigned char caType=0;
	unsigned int  casysid=0;
	unsigned int  ident=0;
	unsigned char keynr=0;

	if (keyinfo == NULL)
		return;

//	memset(keynr, 0, 64);

	CurKeyInfo.ca_id = casysid;
	CurKeyInfo.ca_type = caType;
	CurKeyInfo.provider = ident;
	CurKeyInfo.keynr = keynr;

	memcpy (keyinfo, &CurKeyInfo, sizeof(CUR_KEY_INFO));
}

CUR_KEY_INFO *ucas_get_cur_keyinfo_p (void)
  {
	return (&CurKeyInfo);
  }


//******************************************************
//
//softcam key to  paramers
//
//*****************************************************
static unsigned char readline(unsigned char*pLine,unsigned long*dwLineLen)
{
	unsigned long ii=0;

	for(ii=0;ii<*dwLineLen;ii++)
	{
		if( (pLine[ii]==0x0a)||(pLine[ii]==0x0d) )
		{
            pLine[ii]=0;
            *dwLineLen=ii+1;
			if( (pLine[ii+1]==0x0a)||(pLine[ii+1]==0x0d) )
			{
				pLine[ii+1]=0;
				*dwLineLen=ii+2;
			}
			break;
			//return TRUE;
		}
	}
	if(ii==*dwLineLen)
	{
		pLine[ii]=0;
		*dwLineLen=ii+1;
	}

#if 0
	//skip too long
	if(ii>250)
	{
       pLine[250]=0;
	   pLine[251]=0;
	}
#endif

	return 1;
}
#define  isOKChar(x) ( ((x>=0x20)&&(x<0x30))||((x>0x39)&&(x<0x80)))
#define  isOKHex(x)  ( ((x>='0')&&(x<='9'))||((x>='a')&&(x<='f') )||((x>='A')&&(x<='F')) )
#define  isOKHex2(x)  ( ((x>='0')&&(x<='9'))||((x>='a')&&(x<='z') )||((x>='A')&&(x<='Z'))||(x==':') )
static unsigned char readKey(char*strKeyInfo,char*type,unsigned int*id,char*keynr,char*strKey,char*strName)
{
	unsigned char bRet=0;
	char buffer[256];
	char *pstr;
	int ii=0,jj=0,keyinfolen=strlen(strKeyInfo);

	if(strKeyInfo[0]==';')return bRet;

	if(keyinfolen>256)
        keyinfolen=256;

	ii=0;
	*type=strKeyInfo[ii++];

	strName[0]=0;

	jj=0;
    for(;ii<keyinfolen;ii++)
	{
       if( isOKHex(strKeyInfo[ii]) )
	   {
          buffer[jj++]=strKeyInfo[ii];
	   }
	   else
	   {
		   if(jj)
		     break;
	   }
	}

	if( (ii<keyinfolen)&&(jj<=8) )
	{
		buffer[jj]=0;
		*id=strtol(buffer,&pstr,16);

		jj=0;
		for(;ii<keyinfolen;ii++)
		{
		   if(isOKHex2(strKeyInfo[ii]))
		   {
			  buffer[jj++]=strKeyInfo[ii];
		   }
		   else
		   {
			   if(jj)
			     break;
		   }
		}
		if(ii<keyinfolen)
		{
			buffer[jj]=0;
			//*keynr=strtol(buffer,&pstr,16);
            strcpy(keynr,buffer);

			jj=0;
			for(;ii<keyinfolen;ii++)
			{
			   if(isOKHex(strKeyInfo[ii]))
			   {
				  buffer[jj++]=strKeyInfo[ii];
			   }
			   else
			   {
				   if(jj)
					 break;
			   }
			}

            if( (ii<=keyinfolen) /*&&( (jj==16)||(jj==32) )*/ )
			{
                buffer[jj]=0;
                strcpy(strKey,buffer);
                bRet=TRUE;  //now it's ok
			}
		}

	}
    return bRet;
}

#define SPECIAL_BUFF_SIZE  16*1024


int  mk_key(unsigned char*pData,unsigned long *dwLen)
{
	unsigned char *pBuf=pData;
	unsigned long dwLineLen=256;
    unsigned int DoneLen=0;
	int key_data_len=0;
   int i;
   int biss_para_num = 0;
   int biss_para_colon_num = 0;

   char type;
   unsigned int  id;
   char strkeynr[128];
   char key[256]={0x00,};
   char providername[MAX_PROVIDER_NAME_LEN+10]={0x00,};

	KEYINFO        keyinfo;
	KEYINFO2       keyinfo2 = {0};
   KEYINFO_BISS keyinfobiss = {0};


//    KEYINFO_CONAX  keyConax;

	KEY_HEADER_INFO_SDRAM keyHeader;

    unsigned char* pkeyinfos=(unsigned char*)mtos_malloc(128*1024);
	int keyPos=sizeof(keyHeader);
	KEYINFO2 *pTpsAesKey=(KEYINFO2 *) mtos_malloc(128*1024);
   // int keyProviderNameNr=0; // keyinfoNr=0,

	memset(pkeyinfos,0xff,KEYBUFFERSIZE);
   memset(&keyHeader,0x00,sizeof(keyHeader));

	while(DoneLen<*dwLen)
	{
		memset(providername,0x00,sizeof(providername));
		if(keyPos>KEYBUFFERSIZE){
           DBG_Print(("Key too many"));
			break;
		}
//		if(keyProviderNameNr*sizeof(PROVIDER_NAME)>=KEYPROVIDERNAMESIZE){
//           AfxMessageBox("Key provider name too long",MB_OK);
//			break;
//		}

	   dwLineLen=*dwLen-DoneLen;
       if(readline(pBuf,&dwLineLen))
	   {
		   //sk_printf("%s\n",pBuf);
           //if(sscanf((const char*)pBuf,"%c %x %x %s",&type,&id,&keynr,&key) == 4)
		   if(readKey((char*)pBuf,&type,&id,strkeynr,key,providername))
		   {
			   char*pstr;
             int keylen=(strlen(key)/2);
             unsigned char keynr=0;
             unsigned int id_biss=0;
             unsigned int server_id_biss=0;
	#if 0
					   sk_printf("type[%c],id[%06x],keynr[%02x],key[%s]\n",
							   type,id,keynr,key);
	#endif

             if( (type=='v')||(type=='V') )
              {
				 if(keylen==8)
                type=TYPE_VIACCESS;
				 else if(keylen==16)
                type=TYPE_VIACCESS2;
			    }
             else if( (type=='i')||(type=='I') )
              {
				 if(keylen==8)
                type=TYPE_IRDETO;
				  if(keylen==16)
					type=TYPE_IRDETO2;
			   }
             else if( (type=='s')||(type=='S') )
              {
               if(keylen==8)
                type=TYPE_SECA;
				  else if(keylen==16)
                type=TYPE_SECA2;
			   }
             else if( (type=='n')||(type=='N') )
              {
                if(keylen==8)
                  type=TYPE_NAGRA;
				  else if(keylen==16)
               {
					     type=TYPE_NAGRA2;
				  }
				 }
          else if( (type=='b')||(type=='B') )
			 {
            /*  unsigned int zxc=0,goodBiss=0;
				 for(zxc=0;zxc<strlen(strkeynr);zxc++)
               {
					 if(strkeynr[zxc]==':')
                 {
                    strkeynr[zxc]=0;
						 goodBiss++;
					 }
					 if( (strkeynr[zxc]=='H')||(strkeynr[zxc]=='h')
						 ||(strkeynr[zxc]=='V')||(strkeynr[zxc]=='v') )
						 {
						 if(2==goodBiss)
                   {
                // TODO: goodBiss==0xff;
                goodBiss=0xff;
						 }
					 }
				 }
				 if(0xff==goodBiss)
              {
                     int tpFreq=strtol(strkeynr,&pstr,16);
                     id<<=16;
					     id+=tpFreq;
                     strkeynr[0]='0';
					     strkeynr[1]='0';
                     strkeynr[2]=0x00;
				 }*/
                 if(keylen==8)
                   type=TYPE_BISS;

			 }
          else if( (type=='x')||(type=='X') )
            {
               if(keylen==8)
                  type=TYPE_CONAX;
				  else if(keylen==16)
               {
					  type=TYPE_CONAX2;
				  }
			  }
          else if( (type=='w')||(type=='W') )
            {
              if(keylen==8)
                  type=TYPE_CRYPTOWORKSHALF;
				  else if(keylen==16)
               {
					  type=TYPE_CRYPTOWORKS;
				  }
			 }

			 keynr=(unsigned char)strtol(strkeynr,&pstr,16);

           for(i=0;i<keylen;i++)
			   {
				   char temp[8],*pstr;

				   temp[0]=key[2*i+0];
				   temp[1]=key[2*i+1];
				   temp[2]=0;
				   key[i]=(char)strtol(temp,&pstr,16);
			   }


			 if( okCATYPE(type) )
			 {
				   memset(&keyinfo,0x00,sizeof(keyinfo));
				   keyinfo.type=type;
				   keyinfo.id=id;  //provid
				 //  keyinfo.keynr=keynr; //idx

				switch(type)
				{
				case TYPE_VIACCESS:
				case TYPE_IRDETO:
				case TYPE_SECA:
				case TYPE_NAGRA:
             case TYPE_CRYPTOWORKSHALF:
             case TYPE_CONAX:
             case TYPE_VIDEOGUARD:
                   keyinfo.keynr=keynr; //idx
                    memcpy(keyinfo.key,key,8);
                    memcpy(pkeyinfos+keyPos,&keyinfo,sizeof(keyinfo));
                    keyPos+=sizeof(keyinfo);
					break;
            case TYPE_VIACCESS2:
				case TYPE_NAGRA2:
				case TYPE_CRYPTOWORKS:
				case TYPE_IRDETO2:
             case TYPE_SECA2:
             case TYPE_CONAX2:
             case TYPE_VIDEOGUARD2:
                    keyinfo.keynr=keynr; //idx
                    memcpy(&keyinfo2,&keyinfo,sizeof(keyinfo));
                    memcpy(keyinfo2.key,key,16);
                    memcpy(pkeyinfos+keyPos,&keyinfo2,sizeof(keyinfo2));
                    keyPos+=sizeof(keyinfo2);
					break;
          	case TYPE_BISS:
                   memset(&keyinfobiss,0x00,sizeof(keyinfobiss));
				       keyinfobiss.type=type;
             	    id_biss=(unsigned int)strtol(strkeynr,&pstr,10);
                    biss_para_colon_num = 0;
                    for(biss_para_num=0;biss_para_num<strlen(strkeynr);biss_para_num++)
                    {
                      if(strkeynr[biss_para_num]==':')
                      {
                        biss_para_colon_num++;
                      }
                      if(biss_para_colon_num==4)
                      {
                        server_id_biss = (unsigned int)strtol(&strkeynr[biss_para_num+1],&pstr,16);
                        break;
                      }
                    }
                    keyinfobiss.id=id_biss;
                    keyinfobiss.servid=server_id_biss;
                    memcpy(keyinfobiss.key,key,8);
                    memcpy(pkeyinfos+keyPos,&keyinfobiss,sizeof(keyinfobiss));
                    keyPos+=sizeof(keyinfobiss);
				}
         }
        }
	    pBuf+=dwLineLen;
        DoneLen+=dwLineLen;
//sk_printf("keyinfoNr[%d]\n",keyinfoNr);
	   }//if(readline(pBuf,&dwLineLen))
	}//while(DoneLen<*dwLen)

	keyHeader.Sync=0x87654321;
	keyHeader.Version=122;
   keyHeader.ProviderNameOffset=keyPos;
   memcpy(pkeyinfos,&keyHeader,sizeof(keyHeader));

    //*dwLen=keyPos;
	memset(pData,0xff,*dwLen);
	memcpy(pData,pkeyinfos,keyPos);
  	keyPos+=4;
    key_data_len=keyPos;

#if 0//defined(COMPRESS_DEFAUT_KEY_ENABLE)
	{
		unsigned long DestLen=(32*1024);
		unsigned char* pTempBuff=(unsigned char*) OS_Malloc(32*1024);
		t_SOWELL_GZ_HEADER *pGZ_header;
        int crc_offset;
		int rc;
		 rc=compress(pTempBuff,&DestLen,pData,key_data_len);
		if(rc==0){
			pGZ_header=(t_SOWELL_GZ_HEADER *)pData;
			memset(pGZ_header,0x00,sizeof(t_SOWELL_GZ_HEADER));
			pGZ_header->SynHeader=SYN_GZ_HEADER;
			pGZ_header->ImageLength=key_data_len;
			pGZ_header->GZLength=DestLen;
			pGZ_header->GZChecksum=0;
			memcpy(pData+sizeof(t_SOWELL_GZ_HEADER),pTempBuff,DestLen);
			crc_offset=sizeof(pGZ_header->SynHeader)+sizeof(pGZ_header->GZChecksum);
			pGZ_header->GZChecksum=DBS_Crc32(pData+crc_offset,DestLen+sizeof(t_SOWELL_GZ_HEADER)-crc_offset,0);
			key_data_len=DestLen+sizeof(t_SOWELL_GZ_HEADER);
			sk_printf("compress  key sucessfully\n");
		}
		OS_Free(pTempBuff);
	}
#endif

   mtos_free(pkeyinfos);
	mtos_free(pTpsAesKey);
    return key_data_len;
}
int softcam_key_to_paramers(u8*ptxtbuf,u32 txtlen)
{
	int bin_key_len;
//	u32 crc32_data;
//	u32 rcrc32_data;
	KEY_HEADER_INFO_SDRAM  key_header;

	if( (bin_key_len=mk_key((unsigned char*)ptxtbuf,(unsigned long*)&txtlen))>0 )
	{
	   memcpy(&key_header,ptxtbuf,sizeof(key_header));
	   ucas_load_key((unsigned char*)ptxtbuf,key_header.ProviderNameOffset-4);
	   DBG_Print(("mk key to bin successful!\n"));
	   return 0;
	}
	return -1;
}
