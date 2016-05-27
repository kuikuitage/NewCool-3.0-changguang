
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

#define 	TICK_PER_SECOND 		100
#define   URL_MAX_LENGTH           	(2048)
#define   NC_AUTH_STACK_SIZE     (16*1024)
#define   USER_ID_LENGTH		(32)

unsigned  char ipAddress[4];


#ifdef NC_NET_AUTH_EN
static const char * DEFAULT_SERVER_ADDRESS	= "http://192.168.1.117";
static const char * DEFAULT_SERVER_PORT  		= "9096";

	

static const char * REGISTER			= "register";
static const char * LOGIN				= "login";
static const char * HEARTBEAT			= "heartbeat";
static const char * LOGOUT				= "logout";



static const char * STBID				= "serialNo";
static const char * MAC_ADDR			= "mac";
static const char * DEVICEID			= "deviceId";
static const char * OS_TYPE			= "osType";
static const char * DEVICEMODEL		= "deviceModel";
static const char * MANUFACTURER		= "manufacturer";
static const char * VERSION			= "version";
static const char * CHIP_INFO			= "chipInfo";
static const char * ID   					= "id";
static const char * RESULT   			= "result";
static const char * OK					="ok";
static const char * HEARTBEAT_C		= "hbinter";


static const char * OT_LINUX				= "linux";
static const char * OT_ANDROID			= "android";
static const char * OT_NEWCOOL			= "newcool";

static const char * ERROR_INVALID_DEVICEMODEL 		= "001"; //无效的deviceModel
static const char * ERROR_INVALID_MANUFACTURER 		= "002"; //无效的Manufacturer.
static const char * ERROR_INVALID_ID 					= "003"; //注册序列号无效
static const char * ERROR_MAC_ID_NOT_MATCHED 		= "004"; //MAC地址和注册序列号不匹配。
static const char * ERROR_INVALID_DEVICETYPE 		= "005"; //无效设备类型。
static const char * ERROR_INVALID_ILLEGAL 			= "006"; //判定为非法的盒子。


static unsigned char com_buff[1024];
#endif

typedef struct auth_s
{
	BOOL 		inited;
	BOOL 		active;
	BOOL		caEntitlement;
	NC_STATUS_t state;
	char 		id[USER_ID_LENGTH + 1];
	char 		serialNo[24];
	unsigned char mac[6];
	char 		deviceId[24];
	char 		osType[24];
	char 		deviceModel[24];
	char 		manufacturer[24];
	char 		version[24];
	char 		chipInfo[24];

	char 		serverIp[32];
	char 		serverPort[8];


	char 		url[URL_MAX_LENGTH];
	char 		first_insert;
	unsigned int	last_beat_tick;
	unsigned int	beating;
	unsigned int   beatFailureCount;
}NC_Auth_t;

static NC_Auth_t auth;
volatile int MONITOR_ACTIVE;

#define AUTH_PRINTF 		OS_PRINTF

#ifdef NC_NET_AUTH_EN
static char *NewUrlRegister(void)
{
	memset(auth.url,0,sizeof(auth.url));
	auth.first_insert = 1;

	//address
	strcat(auth.url,auth.serverIp);
	strcat(auth.url,":");
	strcat(auth.url,auth.serverPort);
	strcat(auth.url,"/");
	//type
	strcat(auth.url,REGISTER);
	strcat(auth.url,"?");
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
	//type
	strcat(auth.url,LOGIN);
	strcat(auth.url,"/");
	strcat(auth.url,auth.id);
	strcat(auth.url,"?");

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
	//type
	strcat(auth.url,HEARTBEAT);
	strcat(auth.url,"/");
	strcat(auth.url,auth.id);
	strcat(auth.url,"?");
	//sno
	strcat(auth.url,STBID);
	strcat(auth.url,"=");
	strcat(auth.url,auth.serialNo);

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
	strcat(auth.url,"/");
	strcat(auth.url,auth.id);
	return auth.url;
}


static char *UrlInsertKeyValuePairsString(const char * key, const char * value)
{
	char buf[128];
	snprintf(buf,128,"%s=%s",key,value);
	if (!auth.first_insert)
	{
		strcat(auth.url,"&");
	}
	auth.first_insert = 0;
	strcat(auth.url,buf);
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
		TMP_FILE, 10,
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
	char buf[20] = {0};
	GxJson_t  json;
	const char * str;
	u32 len;

	NewUrlRegister();
	//stb id
	UrlInsertKeyValuePairsString(STBID, auth.serialNo);
	//mac address
	snprintf(buf,sizeof (buf),"%02x%02x%02x%02x%02x%02x",
		auth.mac[0],auth.mac[1],auth.mac[2],auth.mac[3],auth.mac[4],auth.mac[5]);
	UrlInsertKeyValuePairsString(MAC_ADDR, buf);
	//deviceid
	UrlInsertKeyValuePairsString(DEVICEID, auth.deviceId);

	//ostype
	UrlInsertKeyValuePairsString(OS_TYPE, auth.osType);
	//device model
	UrlInsertKeyValuePairsString(DEVICEMODEL, auth.deviceModel);
	//device model
	UrlInsertKeyValuePairsString(MANUFACTURER, auth.manufacturer);
	//version
	UrlInsertKeyValuePairsString(VERSION, auth.version);
	//chipInfo
	UrlInsertKeyValuePairsString(CHIP_INFO, auth.chipInfo);

	OS_PRINTF("URL:%s\n",auth.url);
	len = sizeof(com_buff) - 1;
	memset(com_buff,0,sizeof(com_buff));
	ret = GetUrlJsonResult(auth.url, com_buff, &len);
	OS_PRINTF("Result:%s\n",com_buff);
	
	if (ret < 0)
	{
		AUTH_PRINTF("http get info failure!\n");
		return -1;
	}

	json = GxJson_LoadString(com_buff);
	if (json == NULL)
	{
		AUTH_PRINTF("load json failure!\n");
		return -1;
	}	

	str = GxJson_GetString(json, RESULT);
	if (str == NULL || strcmp(str,OK) != 0)
	{
		if (strcmp(str,ERROR_INVALID_DEVICEMODEL) == 0)
		{
			//无效的device类型
		}
		if (strcmp(str,ERROR_INVALID_MANUFACTURER) == 0)
		{
			//无效的厂商
		}
		ret = -1;
		goto RET_ERR;
	}
	
	str = GxJson_GetString(json, ID);
	if (str == NULL)
	{
		ret = -1;
		goto RET_ERR;
	}
	strncpy(auth.id,str,sizeof(auth.id) -1);
	ret = 0;
RET_ERR:
	GxJson_Free(json);		
	return ret;



}

static int NewCool_Login(void)
{
	int ret = 0;
	int beating;
	char buf[20] = {0};
	u32 len;
	GxJson_t  json;
	const char * str;

	NewUrlLogin();
	//mac address
	snprintf(buf,sizeof (buf),"%02x%02x%02x%02x%02x%02x",
		auth.mac[0],auth.mac[1],auth.mac[2],auth.mac[3],auth.mac[4],auth.mac[5]);
	UrlInsertKeyValuePairsString(MAC_ADDR, buf);

	//stb id
	UrlInsertKeyValuePairsString(STBID, auth.serialNo);
	//deviceid
	UrlInsertKeyValuePairsString(DEVICEID, auth.deviceId);
	//ostype
	UrlInsertKeyValuePairsString(OS_TYPE, auth.osType);
	//device model
	UrlInsertKeyValuePairsString(DEVICEMODEL, auth.deviceModel);
	//device model
	UrlInsertKeyValuePairsString(MANUFACTURER, auth.manufacturer);
	//version
	UrlInsertKeyValuePairsString(VERSION, auth.version);
	//chipInfo
	UrlInsertKeyValuePairsString(CHIP_INFO, auth.chipInfo);
	OS_PRINTF("URL:%s\n",auth.url);

	len = sizeof(com_buff) - 1;
	memset(com_buff,0,sizeof(com_buff));
	ret = GetUrlJsonResult(auth.url, com_buff, &len);
	OS_PRINTF("Result:%s\n",com_buff);
	
	if (ret < 0)
	{
		AUTH_PRINTF("http get info failure!\n");
		return -1;
	}
	json = GxJson_LoadString(com_buff);
	if (json == NULL)
	{
		AUTH_PRINTF("load json failure!\n");
		return -1;
	}
	str = GxJson_GetString(json, RESULT);
	if (str == NULL || strcmp(str,OK) != 0)
	{

		if (strcmp(str,ERROR_INVALID_ID) == 0)
		{
			//无效的ID
		}
		if (strcmp(str,ERROR_MAC_ID_NOT_MATCHED) == 0)
		{
			//MAC地址不匹配
		}
		ret = -1;
		goto RET_ERR;
	}
	beating = (int)GxJson_GetInteger(json, HEARTBEAT_C);
	auth.beating = beating * 2 * TICK_PER_SECOND;
	ret = 0;
RET_ERR:
	GxJson_Free(json);		
	return ret;
}


static int NewCool_Heartbeat(void)
{
	int ret = 0;
	u32 len;
	GxJson_t  json;
	const char * str;

	NewUrlHeartbeat();
	OS_PRINTF("URL:%s\n",auth.url);

	len = sizeof(com_buff) - 1;
	memset(com_buff,0,sizeof(com_buff));
	ret = GetUrlJsonResult(auth.url, com_buff, &len);
	OS_PRINTF("Result:%s\n",com_buff);
	
	if (ret < 0)
	{
		AUTH_PRINTF("http get info failure!\n");
		return -1;
	}
	json = GxJson_LoadString(com_buff);
	if (json == NULL)
	{
		AUTH_PRINTF("load json failure!\n");
		return -1;
	}
	str = GxJson_GetString(json, RESULT);
	GxJson_Free(json);

	//AUTH_PRINTF("json got result:%s!\n",str);
	if (strcmp(str,ERROR_INVALID_ID) == 0)
	{
		//无效的ID

		return -1;
	}
	if (strcmp(str,ERROR_INVALID_ILLEGAL) == 0)
	{
		//非法用户
		return -1;
	}

	return 0;

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
#endif

extern BOOL xmian_check_network_stat(void);
extern BOOL get_vol_entitlement_from_ca(void);
extern void send_event_to_ui_from_authorization(u32 event);

static void auth_monitor(void)
{
#ifdef NC_NET_AUTH_EN
	int ret;
	(void)FreeUrl;
	(void)NewCool_Logout;
	(void)OT_LINUX;
	(void)OT_ANDROID;
	(void)ERROR_INVALID_DEVICETYPE;
	(void)ERROR_INVALID_ILLEGAL;
#endif
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
			//test_CRC16();
			//AUTH_PRINTF("[auth_monitor] wait net connect!!!!!!\n");
			mtos_task_sleep(2000);
			continue;
		}
		//AUTH_PRINTF("[auth_monitor] start!!!\n");
		// if id is nul,register
		if (NC_AUTH_UNKNOW == auth.state)
		{
#ifdef NC_NET_AUTH_EN
			AUTH_PRINTF("[auth_monitor] auth.id[0] = %d!\n",auth.id[0]);
			if (auth.id[0] == 0)
			{
				AUTH_PRINTF("[auth_monitor] have no id, register!\n");
				ret = NewCool_RegisterStb();
				if (ret != 0)
				{
					AUTH_PRINTF("[auth_monitor] NC auth register stb failure!");
					mtos_task_sleep(5000);
					continue;
				}
				else
				{
					sys_set_vod_user_id(auth.id, strlen(auth.id));
				}
			}
#endif
			auth.state = NC_AUTH_REGISTERED;
		}
		if (NC_AUTH_REGISTERED == auth.state)
		{
#ifdef NC_NET_AUTH_EN
			AUTH_PRINTF("[auth_monitor] have registered, Login!\n");
			ret = NewCool_Login();
			if (ret != 0)
			{
				AUTH_PRINTF("[auth_monitor] NC auth stb login failure!");
				mtos_task_sleep(5000);
				continue;
			}
#endif
			auth.state = NC_AUTH_LOGIN;
			auth.last_beat_tick = mtos_ticks_get();
		}
		if (NC_AUTH_LOGIN == auth.state)
		{
			current_tick = mtos_ticks_get();
			if (current_tick - auth.last_beat_tick >= auth.beating)
			{
#ifdef NC_NET_AUTH_EN
				ret = NewCool_Heartbeat();
				if (ret != 0)
				{
					//static event_t evt;
					AUTH_PRINTF("[auth_monitor] NC auth stb beating failure! \n");
					auth.beatFailureCount ++;
					if (auth.beatFailureCount > 3)
					{
						send_event_to_ui_from_authorization(CAS_E_VOD_AUTH_INVALID);
					}
				}
				else if(FALSE == get_vol_entitlement_from_ca())
#else
                          if(FALSE == get_vol_entitlement_from_ca())
#endif
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
	if ((auth.beatFailureCount > 3) || (FALSE == auth.caEntitlement))
		return NC_AUTH_BEATING_FAILURE;
	return auth.state;
}

int NC_AuthInit(const char *stbId,const unsigned char* mac,int mac_len)
{
	unsigned int *p_stack = NULL;
	int ret = 0;
	BOOL err;
#ifdef NC_NET_AUTH_EN
       int i;
	unsigned char * mem;
	char id[USER_ID_LENGTH+1] = {0};
#endif

	AUTH_PRINTF("NC_AuthInit call in:\n");
	if (auth.inited)
	{
		AUTH_PRINTF("  have inited,return!\n");
		return 0;
	}

	auth.inited = TRUE;
	auth.caEntitlement = FALSE;

#ifdef NC_NET_AUTH_EN
	mem = mtos_malloc(NC_AUTH_STACK_SIZE);
	if (mem == NULL)
		return -1;
	Nw_Download_Init(NC_AUTHORIZATION_HTTP, mem,16*1024);

	if (sys_get_vod_user_id(id,USER_ID_LENGTH))
	{
		// get id
		strncpy(auth.id ,id,USER_ID_LENGTH);
		AUTH_PRINTF("  stbId : %s!\n",auth.id);
	}

	//mac address
	for (i=0;i<mac_len;i++)
	{
		auth.mac[i] = mac[i];
	}
	OS_PRINTF("  MAC:%02x%02x%02x%02x%02x%02x",auth.mac[0],auth.mac[1],auth.mac[2],
						auth.mac[3],auth.mac[4],auth.mac[5]);
	//get stb id (serial number)
	strncpy(auth.serialNo,"1000010010010000006",sizeof(auth.serialNo) -1);
	//get device id
	strncpy(auth.deviceId,"01",sizeof(auth.deviceId) -1);
	//get os type
	strncpy(auth.osType,OT_NEWCOOL,sizeof(auth.osType) -1);
	//get device mode
	strncpy(auth.deviceModel,"newcool",sizeof(auth.deviceModel) -1);
	strncpy(auth.manufacturer,"thinewtec",sizeof(auth.manufacturer) -1);
	strncpy(auth.version,"V1.0.1",sizeof(auth.version) -1);
	strncpy(auth.chipInfo,"CC6000M",sizeof(auth.chipInfo) -1);

	// TODO: just tmp

	strcpy(auth.serverIp,DEFAULT_SERVER_ADDRESS);
	strcpy(auth.serverPort, DEFAULT_SERVER_PORT);
#else
      auth.beating = 2 * TICK_PER_SECOND;
#endif

	p_stack = (unsigned int *)mtos_malloc(NC_AUTH_STACK_SIZE);
	err = mtos_task_create((u8 *)"NC_AuthMonitor",
						(void (*)(void *))auth_monitor,
						(void *)0,
						NC_AUTHORIZATION_MONITOR,
						(u32*)p_stack,
						NC_AUTH_STACK_SIZE);
	if (!err)
	{
		OS_PRINTF("auth create task failure!\n");
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
	
	OS_PRINTF("%s() ip: %d.%d.%d.%d",__FUNCTION__,ipAddress[0],ipAddress[1],ipAddress[2],ipAddress[3]);
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

