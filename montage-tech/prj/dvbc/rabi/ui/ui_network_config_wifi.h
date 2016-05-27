/****************************************************************************

 ****************************************************************************/
#ifndef __UI_NETWORK_CONFIG_WIFI_H__
#define __UI_NETWORK_CONFIG_WIFI_H__

//weather  area cont
#define NETWORK_ITEM_X         60
#define NETWORK_ITEM_Y         20//100
#define NETWORK_ITEM_W         (RIGHT_ROOT_W-2*NETWORK_ITEM_X)
#define NETWORK_ITEM_LW         300
#define NETWORK_ITEM_RW         (NETWORK_ITEM_W-NETWORK_ITEM_LW)
#define NETWORK_ITEM_H        50
#define NETWORK_ITEM_VGAP 8

//connect status text
#define NETWORK_CONNECT_STATUS_TEXTX (NETWORK_ITEM_X+NETWORK_ITEM_LW+NETWORK_ITEM_RW-NETWORK_CONNECT_STATUSW-NETWORK_CONNECT_STATUS_TEXTW-100)
#define NETWORK_CONNECT_STATUS_TEXTY (SYS_LEFT_CONT_H-NETWORK_CONNECT_STATUS_TEXTH-20)
#define NETWORK_CONNECT_STATUS_TEXTW 200
#define NETWORK_CONNECT_STATUS_TEXTH NETWORK_ITEM_H

//connect status
#define NETWORK_CONNECT_STATUSX NETWORK_CONNECT_STATUS_TEXTX+NETWORK_CONNECT_STATUS_TEXTW
#define NETWORK_CONNECT_STATUSY NETWORK_CONNECT_STATUS_TEXTY
#define NETWORK_CONNECT_STATUSW NETWORK_CONNECT_STATUS_TEXTW
#define NETWORK_CONNECT_STATUSH NETWORK_CONNECT_STATUS_TEXTH


#define WIFI_IP_CTRL_CNT 9//10//9
#define CONFIG_MODE_TYPE_CNT 3

RET_CODE open_network_config_wifi(u32 para1, u32 para2);
//RET_CODE open_pppoe_connect(u32 para1, u32 para2);
void paint_connect_wifi_status(BOOL is_conn, BOOL is_paint);

void dis_conn_wifi(comm_dlg_data_t *p_wifi_dlg_data);

#endif
