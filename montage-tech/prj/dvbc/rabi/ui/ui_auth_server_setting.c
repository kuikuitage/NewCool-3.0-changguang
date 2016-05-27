#ifdef AUTH_SERVER_SETTING
#include "ui_common.h"
#include "ui_auth_server_setting.h"
#include "authorization.h"

enum{
	IDC_SERVER_IP = 1,
	IDC_SERVER_PORT,
};

#define PORT_MIN	0
#define PORT_MAX	65535

static u8 ip_separator[IPBOX_MAX_ITEM_NUM] = {'.', '.', '.', ' '};
u16 auth_server_setting_root_keymap(u16 key);
RET_CODE auth_server_setting_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE ip_setting_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE port_setting_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_auth_server_setting(u32 para1, u32 para2)
{
	control_t *p_cont, *p_ip, *p_port;
	ip_address_t  server_address = {0};
	u8 j;
	u32 port;
	p_cont = ui_comm_right_root_create(ROOT_ID_AUTH_SERVER_SETTING, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
	MT_ASSERT(NULL!=p_cont);
	ctrl_set_keymap(p_cont, auth_server_setting_root_keymap);
	ctrl_set_proc(p_cont, auth_server_setting_root_proc);
	
	p_ip = ui_comm_ipedit_create(p_cont, IDC_SERVER_IP, SERVER_IP_X, SERVER_IP_Y, 
						SERVER_IP_LW, SERVER_IP_RW);
	MT_ASSERT(NULL!=p_ip);
	ui_comm_ipedit_set_static_txt(p_ip, IDS_AUTH_SERVER_IP);
	ui_comm_ipedit_set_param(p_ip, 0, 0, IPBOX_S_B1 | IPBOX_S_B2|IPBOX_S_B3|IPBOX_S_B4, IPBOX_SEPARATOR_TYPE_UNICODE, 18);
#ifdef   NC_AUTH_EN		
{
	u8 ip_addr[4] = {0};
	NC_AuthGetServerIpPort(ip_addr, &port);
	server_address.s_b1 = ip_addr[0];
	server_address.s_b2 = ip_addr[1];
	server_address.s_b3 = ip_addr[2];
	server_address.s_b4 = ip_addr[3];
	DEBUG(DBG, INFO, "ip:%d.%d.%d.%d, port:%d\n", server_address.s_b1, server_address.s_b2, 
									server_address.s_b3, server_address.s_b4, port);
}
#else
	server_address.s_b1 = 127;
	server_address.s_b2 = 0;
	server_address.s_b3 = 0;
	server_address.s_b4 = 1;
	port = 0;
#endif
	ui_comm_ipedit_set_address(p_ip, &server_address);
	
	ctrl_set_rstyle(p_ip,
					RSI_COMM_CONT_SH,
					RSI_SELECT_F,
					RSI_COMM_CONT_GRAY);
	ui_comm_ctrl_set_keymap(p_ip, ui_comm_ipbox_keymap);
	ui_comm_ctrl_set_proc(p_ip, ip_setting_proc);
	for(j = 0; j < IPBOX_MAX_ITEM_NUM; j++)
	{
	  ui_comm_ipedit_set_separator_by_ascchar(p_ip, (u8)j, 
	  ip_separator[j]);
	}
	p_port = ui_comm_numedit_create(p_cont, IDC_SERVER_PORT,
	 		 SERVER_IP_X, PORT_Y, SERVER_IP_LW, SERVER_IP_RW);
	MT_ASSERT(NULL!=p_port);
	ui_comm_numedit_set_static_txt(p_port, IDS_PORT_X);
	ui_comm_ctrl_set_proc(p_port, port_setting_proc);
	ui_comm_numedit_set_param(p_port, NBOX_NUMTYPE_DEC|NBOX_ITEM_POSTFIX_TYPE_STRID,
			 PORT_MIN, PORT_MAX, 5, (SEARCH_FREQ_BIT - 1));
	ui_comm_numedit_set_num(p_port, port);
	
	ctrl_set_related_id(p_ip, 0, IDC_SERVER_PORT, 0, IDC_SERVER_PORT);
	ctrl_set_related_id(p_port, 0, IDC_SERVER_IP, 0, IDC_SERVER_IP);
	
	ctrl_default_proc(p_ip, MSG_GETFOCUS, 0, 0);
	ctrl_paint_ctrl(p_cont, FALSE);
	return SUCCESS;
}

static RET_CODE auth_sever_setting_change_focus(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
	control_t *p_next, *p_ctrl;

	p_ctrl = ctrl_get_active_ctrl(p_cont);
	if(MSG_FOCUS_UP==msg)
		p_next = ctrl_get_child_by_id(p_ctrl->p_parent, p_ctrl->up);
	else
		p_next = ctrl_get_child_by_id(p_ctrl->p_parent, p_ctrl->down);
	ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0); 
	ctrl_process_msg(p_next, MSG_GETFOCUS, 0, 0); 
	ctrl_paint_ctrl(p_ctrl,FALSE);
	ctrl_paint_ctrl(p_next,FALSE);
	return SUCCESS;
}

static RET_CODE set_auth_server_ip(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_ip;
	ip_address_t  server_address = {0};

	p_ip = ui_comm_right_root_get_ctrl(ROOT_ID_AUTH_SERVER_SETTING, IDC_SERVER_IP);
	MT_ASSERT(NULL!=p_ip);
	ui_comm_ipedit_get_address(p_ip, &server_address);
	DEBUG(DBG, INFO, "server_address:%d.%d.%d.%d\n", server_address.s_b1, 
				server_address.s_b2,server_address.s_b3, server_address.s_b4);
#ifdef   NC_AUTH_EN	
{
	u8 ip_addr[4];
	ip_addr[0] = server_address.s_b1;
	ip_addr[1] = server_address.s_b2;
	ip_addr[2] = server_address.s_b3;
	ip_addr[3] = server_address.s_b4;
	NC_AuthSetServerIpAddress(ip_addr);
}
#endif
	return SUCCESS;
}

static RET_CODE set_auth_server_port(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_port;
	u32 port;
	
	p_port = ui_comm_right_root_get_ctrl(ROOT_ID_AUTH_SERVER_SETTING, IDC_SERVER_PORT);
	port = ui_comm_numedit_get_num(p_port);
#ifdef   NC_AUTH_EN				
	NC_AuthSetServerPort(port);
#endif
	return SUCCESS;
}

BEGIN_KEYMAP(auth_server_setting_root_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(auth_server_setting_root_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(auth_server_setting_root_proc, ui_comm_root_proc)
	ON_COMMAND(MSG_FOCUS_UP, auth_sever_setting_change_focus)
	ON_COMMAND(MSG_FOCUS_DOWN, auth_sever_setting_change_focus)
END_MSGPROC(auth_server_setting_root_proc, ui_comm_root_proc)

BEGIN_MSGPROC(ip_setting_proc, ui_comm_ipbox_proc)
	ON_COMMAND(MSG_CHANGED, set_auth_server_ip)
END_MSGPROC(ip_setting_proc, ui_comm_ipbox_proc)

BEGIN_MSGPROC(port_setting_proc, ui_comm_num_proc)
	ON_COMMAND(MSG_CHANGED, set_auth_server_port)
END_MSGPROC(port_setting_proc, ui_comm_num_proc)
#endif
	
