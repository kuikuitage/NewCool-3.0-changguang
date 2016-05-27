/****************************************************************************
 ****************************************************************************/
#include "ui_common.h"
//system
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "mtos_printk.h"
//util
#include "class_factory.h"

#include "drv_dev.h"

//mdl
#include "data_manager.h"

#include "uio.h"
//customer
#include "hal_misc.h"
#include "customer_config.h"


static EXT_CUS_SYS_CFG sys_cfg;
static hw_cfg_t hw_cfg;

void dm_load_customer_sys_cfg(void)
{
/*  int read_len = 0;
  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                     CUSTOMER_BLOCK_ID, 0, 0,
                     sizeof(EXT_CUS_SYS_CFG),
                     (u8 *)&sys_cfg);
  MT_ASSERT(read_len == sizeof(EXT_CUS_SYS_CFG));
*/
  return;
}

u8 handle_dm_get_customer_module_cfg(EXT_CUS_MODULE_CFG ext_customer_cfg)
{
   return ((sys_cfg.customer_module_cfg >> ext_customer_cfg) & 1);
}

void dm_load_customer_hw_cfg(void)
{
  int read_len = 0;
  read_len = dm_read(class_get_handle_by_id(DM_CLASS_ID),
                     HW_CFG_BLOCK_ID, 0, 0,
                     sizeof(hw_cfg_t),
                     (u8 *)&hw_cfg);
  MT_ASSERT(read_len == sizeof(hw_cfg_t));
  return;
}
hw_cfg_t dm_get_hw_cfg_info(void)
{
  return hw_cfg;
}

