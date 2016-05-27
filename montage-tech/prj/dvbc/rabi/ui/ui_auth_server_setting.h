#ifndef __UI_AUTH_SERVER_SETTING__
#define __UI_AUTH_SERVER_SETTING__
#ifdef AUTH_SERVER_SETTING

#define SERVER_IP_X				60
#define SERVER_IP_Y				60
#define SERVER_IP_LW			200
#define SERVER_IP_RW			(RIGHT_ROOT_W-2*SERVER_IP_X-SERVER_IP_LW)

#define	PORT_Y					(SERVER_IP_Y+COMM_CTRL_H+10)

RET_CODE open_auth_server_setting(u32 para1, u32 para2);

#endif
#endif
