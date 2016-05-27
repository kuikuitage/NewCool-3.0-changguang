
#ifdef   NC_AUTH_EN			

#include <string.h>
#include "sys_types.h"
#include "mtos_misc.h"
#include <stdio.h>
#include "sys_define.h"
#include "ui_common.h"
#include "download_api.h"
#include "lib_unicode.h"
#include "gx_jansson.h"
#include "drv_dev.h"
#include "ufs.h"
#include "mdl.h"
#include "authorization.h"
#include "ap_cas.h"
#include "des.h"

#define  LOGIN_STATE_PASSWARD_ERROR 		(2001)
#define  LOGIN_STATE_ARREARS		 		(2002)
#define  LOGIN_STATE_UPCOMING_ARREARS 	(2004)
#define  LOGIN_STATE_OK				 		(0)

#define 	TICK_PER_SECOND 		100
#define   URL_MAX_LENGTH           	(2048)
#define   NC_AUTH_STACK_SIZE     (16*1024)
#define   USER_ID_LENGTH		(64)

unsigned  char ipAddress[4];


static const char * DEFAULT_SERVER_ADDRESS	= "http://101.200.173.180";
//static const char * DEFAULT_SERVER_ADDRESS	= "http://192.168.5.13";
static const char * DEFAULT_SERVER_PORT  		= "8660";
static const char * DEFAULT_HOST_PORT  			= "8080";

static const char * REGISTER			= "getLoginCode";
static const char * LOGIN				= "login";
static const char * UPDATEUSERTOKEN	= "updateUserToken";
static const char * HEARTBEAT			= "epgHeart";
static const char * LOGOUT				= "logout";

//static const char * MAC_ADDR			= "mac";
static const char * MANAGEPLATFORM	= "aaa";
static const char * SERVICE				= "ott";
static const char * TERM				= "term";
static const char * USERID   				= "userid";
static const char * AUTHENTICATOR		= "Authenticator";

static const char * ACTION   				= "Action";
static const char * USERTOKEN   			= "usertoken";
static const char * HEARTTOKEN   		= "userToken";
static const char * NEWUSERTOKEN   		= "newUserToken";
static const char * TOKENEXPIREDTIME   	= "tokenExpiredTime";
static const char * OLDUSERTOKEN   		= "oldusertoken";
static const char * STREAMSTATUS   		= "streamStatus";
static const char * USERTOKENEXPIRED   	= "UserTokenExpired";
static const char * TYPE					= "type";
static const char * DEFAULTTYPE			= "AndroidSTB";

//result
static const char * LOGINCODE			= "loginCode";
static const char * RETURNCODE			= "returnCode";
static const char * STATUS				= "status";

static unsigned char com_buff[1024];

typedef struct auth_s
{
	BOOL 		inited;
	BOOL 		active;
	BOOL		caEntitlement;
	NC_STATUS_t state;
	unsigned char 			userid[USER_ID_LENGTH + 1];
	unsigned char  		mac[20];
	unsigned char  		stbId[20];

	unsigned char			userToken[36];
	unsigned char			encryToken[36];
	unsigned char 			key[24];
	unsigned char 			authenticator[1024];

	unsigned char			streamStatus[6];
	unsigned char			type[21];
	unsigned char 			serverIp[32];
	unsigned char 			serverPort[8];
	unsigned char 			hostIp[32];
	unsigned char 			hostPort[8];

	unsigned char 			url[URL_MAX_LENGTH];
	char 			first_insert;
	unsigned int	last_beat_tick;
	unsigned int	beating;
	unsigned int    beatFailureCount;
}NC_Auth_t;

extern void triple_des_encrypt(char *out, char *in, long datalen, char *key);
extern void triple_des_decrypt(char *out, char *in, long datalen, char *key);;
static NC_Auth_t auth;
volatile int MONITOR_ACTIVE;

#define AUTH_PRINTF 		OS_PRINTF

static char *get_strstr(char *p_src,const char *key)
{
	int len = 0;
	
	if(!(len = strlen(key)))
	{
		return p_src;
	}

	for(; *p_src; ++p_src)
	{
		if ((*p_src == *key) && strncmp(p_src, key, len) == 0)
		{
			return (p_src + len);
		}
	}

	return NULL;
}

static int get_param_from_string(char *p_src,const char *key, char *param)
{
	unsigned char *src = NULL;
	int count = 0;
	BOOL  end_flag = FALSE;

	if((NULL == p_src) ||(NULL == key) || (NULL == param))
	{
		return -1;
	}

	if((src = get_strstr(p_src, key)) != NULL)
	{
		src += 2;
		
		for(; *src; ++src)
		{
			if(*src == '\"' )
			{
				if(end_flag == FALSE)
				{
					end_flag = TRUE;
					continue;
				}
				else
					break;;
			}

			if(*src == '}')
				break;
			
			*param++ = *src;
			count++;
		}
	}

	*param = 0;
	if(count > 0)
	{
		return 0;
	}

	return -1;
}

static void set_authenticator(void)
{
	unsigned char buffer[1024] = {0};
	unsigned char num[6] = {0};
	static unsigned char rand = 12;

	rand = ((rand + 1)*2)%256;
	sprintf(num, "%d",rand);
	strcat(buffer,num);
	strcat(buffer,"$");
	
	strcat(buffer,auth.encryToken);
	strcat(buffer,"$");

	strcat(buffer,auth.userid);
	strcat(buffer,"$");

	strcat(buffer,auth.stbId);
	strcat(buffer,"$");

	strcat(buffer,auth.hostIp);
	strcat(buffer,"$");

	strcat(buffer,auth.mac);
	strcat(buffer,"$");
	
	strcat(buffer,"newcool");
	strcat(buffer,"$");
	
	strcat(buffer,"SV");
	AUTH_PRINTF("[debug] set_authenticator authenticator_len = %d  buffer = %s \n",strlen(auth.authenticator),buffer);
	memset(auth.authenticator,0,sizeof(auth.authenticator));
	IPTV_3DesEncrypt(auth.key,buffer,auth.authenticator,sizeof(auth.authenticator));
}

static char *UrlInsertKeyValuePairsString(const char * key, const char * value)
{
	unsigned char buf[1024];
	snprintf(buf,1024,"%s=%s",key,value);
	if (!auth.first_insert)
	{
		strcat(auth.url,"&");
	}
	auth.first_insert = 0;
	strcat(auth.url,buf);
	return auth.url;
}

static char *NewUrlRegister(void)
{
	memset(auth.url,0,sizeof(auth.url));
	auth.first_insert = 1;

	//address
	strcat(auth.url,auth.serverIp);
	strcat(auth.url,":");
	strcat(auth.url,auth.serverPort);
	strcat(auth.url,"/");

	//service
	strcat(auth.url,MANAGEPLATFORM);
	strcat(auth.url,"/");
	strcat(auth.url,SERVICE);
	strcat(auth.url,"/");
	//type
	strcat(auth.url,REGISTER);
	strcat(auth.url,"?");

	//userid
	UrlInsertKeyValuePairsString(USERID, auth.userid);

	AUTH_PRINTF("URL:%s\n",auth.url);
	return auth.url;
}

static char *NewUrlLogin(void)
{
	memset(auth.url,0,sizeof(auth.url));
	auth.first_insert = 1;

	//address
	strcat(auth.url,auth.serverIp);
	strcat(auth.url,":");
	strcat(auth.url,auth.serverPort);
	strcat(auth.url,"/");

	//service
	strcat(auth.url,MANAGEPLATFORM);
	strcat(auth.url,"/");
	strcat(auth.url,SERVICE);
	strcat(auth.url,"/");
	
	//type
	strcat(auth.url,LOGIN);
	strcat(auth.url,"?");

	//userid
	UrlInsertKeyValuePairsString(USERID, auth.userid);
	//authenticator
	UrlInsertKeyValuePairsString(AUTHENTICATOR, auth.authenticator);
	//type
	UrlInsertKeyValuePairsString(TYPE, auth.type);

	AUTH_PRINTF("URL:%s\n",auth.url);
	return auth.url;
}

static char *NewUrlUpdateUserToken(void)
{
	memset(auth.url,0,sizeof(auth.url));
	auth.first_insert = 1;

	//address
	strcat(auth.url,auth.serverIp);
	strcat(auth.url,":");
	strcat(auth.url,auth.serverPort);
	strcat(auth.url,"/");

	//service
	strcat(auth.url,MANAGEPLATFORM);
	strcat(auth.url,"/");
	strcat(auth.url,SERVICE);
	strcat(auth.url,"/");
	
	//type
	strcat(auth.url,UPDATEUSERTOKEN);
	strcat(auth.url,"?");

	//userid
	UrlInsertKeyValuePairsString(OLDUSERTOKEN, auth.userToken);
	//action
	UrlInsertKeyValuePairsString(ACTION, USERTOKENEXPIRED);

	AUTH_PRINTF("URL:%s\n",auth.url);
	return auth.url;
}

static char *NewUrlHeartbeat(void)
{
	memset(auth.url,0,sizeof(auth.url));
	auth.first_insert = 1;

	//address
	strcat(auth.url,auth.serverIp);
	strcat(auth.url,":");
	strcat(auth.url,auth.serverPort);
	strcat(auth.url,"/");

	//service
	strcat(auth.url,MANAGEPLATFORM);
	strcat(auth.url,"/");
	strcat(auth.url,TERM);
	strcat(auth.url,"/");
	
	//type
	strcat(auth.url,HEARTBEAT);
	strcat(auth.url,"?");

	//userid
	UrlInsertKeyValuePairsString(HEARTTOKEN, auth.userToken);
	//streamStatus
	UrlInsertKeyValuePairsString(STREAMSTATUS, auth.streamStatus);
	
	AUTH_PRINTF("URL:%s\n",auth.url);
	return auth.url;
}

static char *NewUrlLogout(void)
{
	memset(auth.url,0,sizeof(auth.url));
	auth.first_insert = 1;

	//address
	strcat(auth.url,auth.serverIp);
	strcat(auth.url,":");
	strcat(auth.url,auth.serverPort);
	strcat(auth.url,"/");
	//type
	strcat(auth.url,LOGOUT);
	strcat(auth.url,"?");
	return auth.url;
}

static void FreeUrl(void)
{
	memset(auth.url,0,sizeof(auth.url));
}

static int GetUrlJsonResult(const char* url,char * buffer,u32 *len)
{
#define   TMP_FILE  "r:register_id"
	int ret;
	u8 err;
	ufs_file_t fd = {0};
	u16 unicode[20] = {0};
	ret = Nw_DownloadURLTimeout(url,
		TMP_FILE, 500,
		NULL,
		NULL,
		NULL,
		NULL,
		0);
	
	if (ret <= 0)
	{
		AUTH_PRINTF("get server resp failure!\n");
		return -1;
	}

	str_asc2uni(TMP_FILE, unicode);
	err = ufs_open(&fd, (tchar_t*)unicode, UFS_READ);
	err = ufs_read(&fd,buffer, 256, len);
	err = ufs_close(&fd);
	return 0;
}

static int NewCool_RegisterStb(void)
{
	int ret = 0;
	u32 len;

	NewUrlRegister();

	len = sizeof(com_buff) - 1;
	memset(com_buff,0,sizeof(com_buff));
	ret = GetUrlJsonResult(auth.url, com_buff, &len);
	AUTH_PRINTF("Result:%s\n",com_buff);
	if (ret < 0)
	{
		AUTH_PRINTF("http get info failure!\n");
		return -1;
	}

	ret = get_param_from_string(com_buff, LOGINCODE, auth.encryToken);
	AUTH_PRINTF("[debug] encryToken = %s \n",auth.encryToken);
	if(ret == 0)
	{
		set_authenticator();
	}
		
	return ret;
}

static int NewCool_Login(void)
{
	int ret = 0;
	unsigned char returnCode[10] = {0};
	u32 len;

	NewUrlLogin();

	len = sizeof(com_buff) - 1;
	memset(com_buff,0,sizeof(com_buff));
	ret = GetUrlJsonResult(auth.url, com_buff, &len);
	AUTH_PRINTF("Result:%s\n",com_buff);
	if (ret < 0)
	{
		AUTH_PRINTF("http get info failure!\n");
		return -1;
	}

	ret = get_param_from_string(com_buff, RETURNCODE, returnCode);
	if(ret == 0)
	{
		AUTH_PRINTF("NewCool_Login fail returnCode = %s returnStatus = %d\n",returnCode);
	}
	
	ret = get_param_from_string(com_buff, USERTOKEN, auth.userToken);
	if(ret == 0)
	{
		AUTH_PRINTF("NewCool_Login userToken = %s \n",auth.userToken);
		return 0;
	}
	
	return -1;
}

static int NewCool_Heartbeat(void)
{
	int ret = 0;
	unsigned char status[10] = {0};
	u32 len;

	NewUrlHeartbeat();

	len = sizeof(com_buff) - 1;
	memset(com_buff,0,sizeof(com_buff));
	ret = GetUrlJsonResult(auth.url, com_buff, &len);
	AUTH_PRINTF("Result:%s\n",com_buff);
	if (ret < 0)
	{
		AUTH_PRINTF("http get info failure!\n");
		return -1;
	}

	ret = get_param_from_string(com_buff, STATUS, status);
	AUTH_PRINTF("NewCool_UpdateUserToken status = %s\n",status);
	if(ret == 0)
	{
		if(strcmp(status, "1") == 0)
		{
			return 0;
		}
	}

	return -1;
}

static int NewCool_UpdateUserToken(void)
{
	int ret = 0;
	unsigned char returnCode[10] = {0};
	unsigned char tokenExpiredTime[10] = {0};
	unsigned char userToken[36] = {0};
	u32 len;

	NewUrlUpdateUserToken();

	len = sizeof(com_buff) - 1;
	memset(com_buff,0,sizeof(com_buff));
	ret = GetUrlJsonResult(auth.url, com_buff, &len);
	AUTH_PRINTF("Result:%s\n",com_buff);
	if (ret < 0)
	{
		AUTH_PRINTF("http get info failure!\n");
		return -1;
	}

	ret = get_param_from_string(com_buff, RETURNCODE, returnCode);
	AUTH_PRINTF("NewCool_UpdateUserToken result = %s\n",returnCode);
	if((ret == 0)&&(strcmp(returnCode, "0") != 0))
	{
		AUTH_PRINTF("NewCool_UpdateUserToken result = %s\n",returnCode);
		return -1;
	}

	ret = get_param_from_string(com_buff, TOKENEXPIREDTIME, tokenExpiredTime);
	if(ret == 0)
	{
		AUTH_PRINTF("NewCool_UpdateUserToken tokenExpiredTime = %s \n",tokenExpiredTime);
	}

	ret = get_param_from_string(com_buff, NEWUSERTOKEN, userToken);
	if(ret == 0)
	{
		memset(auth.userToken,0,sizeof(auth.userToken));
		strcpy(auth.userToken, userToken);
		AUTH_PRINTF("NewCool_UpdateUserToken auth.userToken = %s!  %s\n",auth.userToken, userToken);
		return 0;
	}

	return -1;
}

static int NewCool_Logout(void)
{
	int ret = 0;
	u32 len;

	NewUrlLogout();
	len = sizeof(com_buff) - 1;
	GetUrlJsonResult(auth.url, com_buff, &len);

	return ret;
}

extern BOOL xmian_check_network_stat(void);
extern BOOL get_vol_entitlement_from_ca(void);
extern void send_event_to_ui_from_authorization(u32 event);

static void auth_monitor(void)
{
	int ret;
	(void)FreeUrl;
	(void)NewCool_Logout;
	(void)NewCool_UpdateUserToken;
	unsigned int current_tick;
	
	while(1)
	{
		if (MONITOR_ACTIVE == 0)
		{
			//AUTH_PRINTF("[auth_monitor] wait!!!!!!\n");
			mtos_task_sleep(5000);
			continue;
		}
		if (!xmian_check_network_stat())
		{
			//AUTH_PRINTF("[auth_monitor] wait net connect!!!!!!\n");
			mtos_task_sleep(2000);
			continue;
		}
		if (NC_AUTH_UNKNOW == auth.state)
		{
			AUTH_PRINTF("[auth_monitor] NC_AUTH_UNKNOW, register!\n");
			ret = NewCool_RegisterStb();
			if (ret != 0)
			{
				AUTH_PRINTF("[auth_monitor] NC auth register stb failure!");
				mtos_task_sleep(5000);
				continue;
			}
			auth.state = NC_AUTH_REGISTERED;
		}
		if (NC_AUTH_REGISTERED == auth.state)
		{
			AUTH_PRINTF("[auth_monitor] have registered, Login!\n");
			ret = NewCool_Login();
			if (ret != 0)
			{
				AUTH_PRINTF("[auth_monitor] NC auth stb login failure!");
				mtos_task_sleep(5000);
				continue;
			}
			auth.state = NC_AUTH_LOGIN;
			auth.last_beat_tick = mtos_ticks_get();
		}
		if (NC_AUTH_LOGIN == auth.state)
		{
			current_tick = mtos_ticks_get();
			if (current_tick - auth.last_beat_tick >= auth.beating)
			{
				ret = NewCool_Heartbeat();
				if (ret != 0)
				{
					auth.beatFailureCount++;
					if(auth.beatFailureCount > 3)
					{
						auth.beatFailureCount = 0;
						auth.state = NC_AUTH_UNKNOW;
					}
					AUTH_PRINTF("[auth_monitor] NC auth stb beating failure! \n");
				}
				else if(FALSE == get_vol_entitlement_from_ca())
				{
					send_event_to_ui_from_authorization(CAS_E_VOD_AUTH_INVALID);
					auth.caEntitlement = FALSE;
					AUTH_PRINTF("[auth_monitor] get entitlement from ca fail! \n");
				}
				else
				{
					auth.beatFailureCount = 0; // clear error count
					auth.caEntitlement = TRUE;
					AUTH_PRINTF("[auth_monitor] get entitlement from ca success! \n");
				}
				auth.last_beat_tick = current_tick;
			}
		}
		mtos_task_sleep(2000);
	}
}

void NC_AuthStart(void)
{
	MONITOR_ACTIVE = 1;
}
void NC_AuthClose(void)
{
	MONITOR_ACTIVE = 0;
}
NC_STATUS_t NC_AuthGetState(void)
{
	if (FALSE == auth.caEntitlement)
		return NC_AUTH_BEATING_FAILURE;
	return auth.state;
}

int NC_AuthInit(const char *stbId,const unsigned char* mac,int mac_len)
{
	unsigned int *p_stack = NULL;
	int ret = 0;
	BOOL err;
	unsigned char * mem;
	ip_address_set_t p_set;
	unsigned char macaddr[6] = {0};
	u32 high,low;

	AUTH_PRINTF("NC_AuthInit call in:\n");
	if (auth.inited)
	{
		AUTH_PRINTF("  have inited,return!\n");
		return 0;
	}

	auth.inited = TRUE;
	auth.caEntitlement = TRUE;
	auth.beating = 1 * TICK_PER_SECOND;

	mem = mtos_malloc(NC_AUTH_STACK_SIZE);
	if (mem == NULL)
		return -1;
	Nw_Download_Init(NC_AUTHORIZATION_HTTP, mem,16*1024);

	macaddr[0] = sys_status_get_mac_by_index(0);
	macaddr[1] = sys_status_get_mac_by_index(1);
	macaddr[2] = sys_status_get_mac_by_index(2);
	macaddr[3] = sys_status_get_mac_by_index(3);
	macaddr[4] = sys_status_get_mac_by_index(4);
	macaddr[5] = sys_status_get_mac_by_index(5);

	//mac address
	sprintf(auth.key,"%02X:%02X:%02X:%02X:%02X:%02X",macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
	sprintf(auth.mac,"%02X:%02X:%02X:%02X:%02X:%02X",macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
	sprintf(auth.stbId,"%02x%02x%02x%02x%02x%02x",macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);


	//strcpy(auth.userid,"4606000080000030");
	hal_get_chip_unique_numb(&high, &low);
	sprintf(auth.userid, "%08lx%08lx",high,low);
	AUTH_PRINTF("[debug] chip id[h:0x%08x,L0x%08x]  auth.userid = %s\n",high,low,auth.userid);
	
	strcpy(auth.streamStatus,"1");
	strcpy(auth.type,DEFAULTTYPE);

	strcpy(auth.serverIp,DEFAULT_SERVER_ADDRESS);
	strcpy(auth.serverPort, DEFAULT_SERVER_PORT);

	strcpy(auth.hostPort, DEFAULT_HOST_PORT);
	sys_status_get_ipaddress(&p_set);
	sprintf(auth.hostIp,"%d.%d.%d.%d",p_set.sys_ipaddress.s_a1,p_set.sys_ipaddress.s_a2,p_set.sys_ipaddress.s_a3,p_set.sys_ipaddress.s_a4);
	AUTH_PRINTF("http://%d.%d.%d.%d \n",p_set.sys_ipaddress.s_a1,p_set.sys_ipaddress.s_a2,p_set.sys_ipaddress.s_a3,p_set.sys_ipaddress.s_a4);

	p_stack = (unsigned int *)mtos_malloc(NC_AUTH_STACK_SIZE);
	err = mtos_task_create((u8 *)"NC_AuthMonitor",
						(void (*)(void *))auth_monitor,
						(void *)0,
						NC_AUTHORIZATION_MONITOR,
						(u32*)p_stack,
						NC_AUTH_STACK_SIZE);
	if (!err)
	{
		AUTH_PRINTF("auth create task failure!\n");
		ret = -1;
	}
	return ret;
}

int NC_AuthSetServerIpAddress(unsigned char * ip)
{
	int i;
	MT_ASSERT(ip != NULL);
	for(i=0;i<4;i++)
	{
		ipAddress[i] = ip[i];
	}
	
	AUTH_PRINTF("%s() ip: %d.%d.%d.%d",__FUNCTION__,ipAddress[0],ipAddress[1],ipAddress[2],ipAddress[3]);
	memset(auth.serverIp,0,sizeof(auth.serverIp));
	sprintf(auth.serverIp,"http://%d.%d.%d.%d",ipAddress[0],ipAddress[1],ipAddress[2],ipAddress[3]);
	return 0;
}

int NC_AuthSetServerPort(unsigned long port)
{
	memset(auth.serverPort, 0, sizeof(auth.serverPort));
	sprintf(auth.serverPort, "%ld", port);
	DEBUG(DBG, INFO, "set auth port:%s", auth.serverPort);

	return 0;
}

void NC_AuthGetServerIpPort(unsigned char * ip, unsigned long *port)
{
	int i, k;
	u8 j = 0;
	
	DEBUG(DBG, INFO, "AUTH_ADDRESS:%s\n", auth.serverIp);
	for(i=0;i<sizeof(auth.serverIp);)
	{
		if(auth.serverIp[i]>='0'&&auth.serverIp[i]<='9')
		{
			ip[j++] = atoi(auth.serverIp+i);
			DEBUG(DBG, INFO, "%d\n", atoi(auth.serverIp+i));
			for(k=0;k<4;k++)
			{
				if(auth.serverIp[++i]<'0'||auth.serverIp[++i]>'9')
					break;
			}
		}
		else
		{
			i++;
		}
	}
	*port = atoi(auth.serverPort);
}

#endif

