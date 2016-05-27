/********************************************************************************************/
/********************************************************************************************/


#include "emon_cmd.h"
#include "cmd_entry.h"

#include "cmd_system.h"
#include "cmd_ir.h"
#include "cmd_dmx.h"



int emon_entry_init(void)
{
  int ret = SUCCESS;

  ret |= cmd_sys_init();
  ret |= cmd_ir_init();
  ret |= cmd_dmx_init();

  return ret;
}


