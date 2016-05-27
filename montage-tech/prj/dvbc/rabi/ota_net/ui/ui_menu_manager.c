/****************************************************************************

****************************************************************************/

#include "ui_common.h"
#include "ui_ota_upgrade.h"
#include "ui_ota_user_input.h"
#include "pti.h"
#include "dvb_svc.h"
//#include "ap_galaxy_ota.h"
//#include "ui_ota_api.h"
#include "ui_network_upgrade.h"

#include "ui_sub_menu.h"
#include "ui_wifi.h"
#include "ui_wifi_link_info.h"
#include "ui_rename.h"
#include "ui_keyboard_v2.h"


struct menu_attr all_menu_attr[] =
{
//  root_id,            play_state,   auto_close, signal_msg,   open
  { ROOT_ID_BACKGROUND,      PS_PLAY, OFF, SM_OFF,  NULL                 },
  {ROOT_ID_SUBMENU, PS_STOP, OFF, SM_BAR, open_sub_menu},
  { ROOT_ID_POPUP, PS_KEEP, OFF, SM_OFF, NULL},    
  {ROOT_ID_NETWORK_UPGRADE, PS_STOP, OFF, SM_OFF, open_network_upgrade},
  {ROOT_ID_WIFI, PS_KEEP, OFF, SM_OFF, ui_open_wifi},
  {ROOT_ID_WIFI_LINK_INFO, PS_KEEP, OFF, SM_OFF, open_wifi_link_info},
  {ROOT_ID_RENAME, PS_KEEP, OFF, SM_OFF, open_rename},
  {ROOT_ID_KEYBOARD_V2, PS_KEEP, OFF, SM_OFF, open_keyboard_v2},
};

#define MENU_CNT    (sizeof(all_menu_attr) / sizeof(struct menu_attr))

static u8 manage_find_menu(u8 root_id)
{
  u8 i;
  for (i = 0; i < MENU_CNT; i++)
  {
    if (all_menu_attr[i].root_id == root_id)
    {
      return i;
    }
  }
  return ROOT_ID_INVALID;
}

RET_CODE manage_open_menu(u8 root_id, u32 para1, u32 para2)
{
  OPEN_MENU open_func;
  u8 idx;

  UI_PRINTF("\r\n[OTA]%s:entry root_id[%d] ",__FUNCTION__, root_id);

  idx = manage_find_menu(root_id);
  if (idx == ROOT_ID_INVALID)
  {
    return ERR_NOFEATURE;
  }

  open_func = all_menu_attr[idx].open_function;
  if ((open_func == NULL))
  {
    return ERR_NOFEATURE;
  }

  return (*open_func)(para1, para2);
}

RET_CODE manage_close_menu(u8 root_id, u32 para1, u32 para2)
{
  if(!fw_destroy_mainwin_by_id(root_id))
  {
    return ERR_FAILURE;
  }

  return SUCCESS;
}

static void ui_enable_uio(BOOL is_enable)
{
  cmd_t cmd = {0};

  cmd.id = (u8)is_enable ?
           AP_FRM_CMD_ACTIVATE_APP : AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_UIO;
  cmd.data2 = 0;

  ap_frm_do_command(APP_FRAMEWORK, &cmd);
}

void ui_evt_enable_ir()
{
  cmd_t cmd = {0};
  OS_PRINTF("%s\n", __FUNCTION__); 
  cmd.id = AP_UIO_CMD_ENABLE_IR;
  cmd.data1 = 0;
  cmd.data2 = 0;   
  ap_frm_do_command(APP_UIO, &cmd); 
 /*
  cmd.id = AP_FRM_CMD_ACTIVATE_APP;
  cmd.data1 = APP_UIO;
  cmd.data2 = 0;    
  ap_frm_do_command(APP_FRAMEWORK, &cmd);   */ 
}

void ui_evt_disable_ir()
{
  cmd_t cmd = {0};
  OS_PRINTF("%s\n", __FUNCTION__);
  cmd.id = AP_UIO_CMD_DISABLE_IR;
  cmd.data1 = 0;
  cmd.data2 = 0;    
  ap_frm_do_command(APP_UIO, &cmd);  
/*  
  cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
  cmd.data1 = APP_UIO;
  cmd.data2 = 0;    
  ap_frm_do_command(APP_FRAMEWORK, &cmd);  */
}

static RET_CODE on_initialize(u32 event, u32 para1, u32 para2)
{
  // start app framework
  OS_PRINTF("\r\n[OTA] %s:entry ",__FUNCTION__);
  ap_frm_set_ui_msgq_id((s32)para1);
  ap_frm_run();

  /* activate uio and ota ap */
  ui_enable_uio(TRUE);
//  ui_enable_ota(TRUE); 
  return SUCCESS;
}

BEGIN_WINDOW_MANAGE(ui_menu_manage)
ON_MENU_EVENT(WINDOW_MANAGE_INIT, on_initialize)
END_WINDOW_MANAGE(ui_menu_manage)


