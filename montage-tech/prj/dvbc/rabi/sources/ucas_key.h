/****************************************************************************************


*****************************************************************************************/
#ifndef  __UCAS_KEY_H__
#define  __UCAS_KEY_H__

typedef struct __SOWELL_GZ_HEADER__{
  unsigned int SynHeader,
               GZChecksum,
               Version,
               ImageLength,
               GZLength,
               Reserve1,
               Reserve2,
               Reserve3;
}t_SOWELL_GZ_HEADER;

/* include follow .h file when u include ucas_key.h
#include "ucas.h"
*/
#define  IRDETO_MAX_KEYINFOS      80
#define  VIACCESS_MAX_KEYINFOS    150
#define  SECA_MAX_KEYINFOS        100
#define  NAGRA_MAX_KEYINFOS       80
#define  BISS_MAX_KEYINFOS        200
#define  CONAX_MAX_KEYINFOS       8
#define  VIACCESS2_MAX_KEYINFOS   200//1200
#define  NAGRA2_MAX_KEYINFOS      32
#define  CRYPTOWORKS_MAX_KEYINFOS    80
#define  IRDETO2_MAX_KEYINFOS     56
#define  SECA2_MAX_KEYINFOS        50
#define  CONAX2_MAX_KEYINFOS       8
#define  CRYPTOWORKSHALF_MAX_KEYINFOS    40
#define  VIDEOGUARD_MAX_KEYINFOS    80
#define  VIDEOGUARD2_MAX_KEYINFOS    80





#if defined(COMPRESS_DEFAUT_KEY_ENABLE)
//#define  MAX_COMPRESSED_KEYCODE_SIZE  32//Kbytes
#define  KEYBUFFERSIZE            (64*1024) 
#define  KEYPROVIDERNAMESIZE      (16*1024)
//for version 1.30
//#define  KEYBUFFERSIZE            (12*1024) 
//#define  KEYPROVIDERNAMESIZE      (4*1024)  
#else
#define  KEYBUFFERSIZE            (8*1024) 
#define  KEYPROVIDERNAMESIZE      (2*1024)  
#endif
#define  MAXSOFTCAMFILESIZE   (256*1024) //256k

//provider name buffer size is 2k
#define MAX_PROVIDER_NUM   300     
#define MAX_PROVIDER_NAME_LEN  12       
typedef struct _PROVIDER_NAME{
	unsigned int provider;
	char type;
	char name[MAX_PROVIDER_NAME_LEN+1];
}PROVIDER_NAME;


int ucas_load_key(unsigned char*pFlashAddr,int FlashLen);
int ucas_get_edit_keyinfo(unsigned char Type,void**editkeyinfos,unsigned int*editnum);
int ucas_finish_edit_keyinfo(void);
int ucas_get_provider_name(char type,unsigned int provider,char**pstrname);
int ucas_get_key_total_number(void);

int ucas_load_provider_name(unsigned char*pFlashAddr,int FlashLen);
int ucas_get_provider_name_info(PROVIDER_NAME**pProName,unsigned int*ProNameNr);


typedef struct {
	unsigned int	ca_id;
	unsigned int	ca_type;
	unsigned int	ecm_pid;
	unsigned int	emm_pid;
	unsigned int	provider;
	unsigned char 	keynr;
	unsigned char   cw[16];
}CUR_KEY_INFO;

void ucas_get_cur_keyinfo(CUR_KEY_INFO * keyinfo);
CUR_KEY_INFO *ucas_get_cur_keyinfo_p (void);
void ucas_reset_cur_keyinfo (void);
int softcam_key_to_paramers(u8*ptxtbuf,u32 txtlen);


//#define  ENABLE_INTER_TPS_KEY

#endif


