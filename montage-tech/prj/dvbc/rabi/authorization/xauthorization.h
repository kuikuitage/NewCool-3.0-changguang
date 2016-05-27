#ifndef __AUTHORIZATION__
#define __AUTHORIZATION__
#ifdef   NC_AUTH_EN			

typedef enum
{
	NC_AUTH_UNKNOW				= 0,
	NC_AUTH_REGISTERED 			= 1,
	NC_AUTH_LOGIN					= 2,
	NC_AUTH_LOGOUT				= 3,
	NC_AUTH_BEATING_FAILURE		= 4,
}NC_STATUS_t;

void NC_AuthStart(void);

void NC_AuthClose(void);

int NC_AuthInit(const char *stbId,const unsigned char* mac,int mac_len);

NC_STATUS_t NC_AuthGetState(void);

int NC_AuthSetServerIpAddress(unsigned char * ip);
int NC_AuthSetServerPort(unsigned long port);
void NC_AuthGetServerIpPort(unsigned char * ip, unsigned long *port);

#endif

#endif


