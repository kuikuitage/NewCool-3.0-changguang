/****************************************************************************

 ****************************************************************************/
#ifndef _UI_NET_UPG_API_H_
#define _UI_NET_UPG_API_H_
#include "net_upg_api.h"

typedef enum 
{
  HTTP,
  FTP,
}protocol_t;

typedef enum 
{
  NUMBER,
  STRING,
}ui_url_type_t;


enum net_upg_api_msg
{
  MSG_NET_UPG_EVT_CHECK_DONE = MSG_EXTERN_BEGIN + 1500,
  MSG_NET_UPG_EVT_DOWN_PROGRESS,
  MSG_NET_UPG_EVT_BURN_PROGRESS,
  MSG_NET_UPG_EVT_FAIL
};

typedef enum
{
  /*!
    net upg evt check done
    */
  UI_NET_UPG_EVT_CHECK_DONE = ((APP_USB_UPG << 16) + 0),
  /*!
    net upg evt progress
    */
  UI_NET_UPG_EVT_DOWN_PROGRESS,
  /*!
    net upg evt progress
    */
  UI_NET_UPG_EVT_BURN_PROGRESS,
  /*!
    fail.
    */
  UI_NET_UPG_EVT_FAIL,
  /*!
    error.
    */
  UI_NET_UPG_EVT_ERROR,
}ui_net_upg_api_evt_t;


enum
{
  MSG_UPG_URL_INFO = MSG_LOCAL_BEGIN + 2122,
};



net_upg_api_info_t *ui_net_upg_get_upg_info(void);

RET_CODE ui_net_upg_init(BOOL b_from_bg);

RET_CODE ui_net_upg_start_check(protocol_t pt, u8 post_flag, u8 *p_addr, u8* p_xml, u8 *p_usr_asc, u8 *p_pwd_asc);

RET_CODE ui_net_upg_upgrade(net_upg_api_up_cfg_t *p_cfg);

RET_CODE ui_net_upg_burn(void);

RET_CODE ui_net_upg_stop(void);

void get_upgrade_url(void);


#endif
