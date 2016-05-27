/******************************************************************************/
/* Copyright (c) 2012 Montage Tech - All Rights Reserved                      */
/******************************************************************************/
// std headers
#include "string.h"

// sys headers
#include "sys_types.h"
#include "sys_define.h"

// util headers
#include "class_factory.h"

// os headers
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_printk.h"

// driver headers
#include "drv_dev.h"
#include "nim.h"
#include "charsto.h"

#include "uio.h"
#include "smc_op.h"
#include "lib_util.h"
#include "lpower.h"

// middleware headers
#include "mdl.h"
#include "service.h"
#include "dvb_protocol.h"
#include "dvb_svc.h"
#include "nim_ctrl_svc.h"
#include "mosaic.h"
#include "cat.h"
#include "pmt.h"
#include "pat.h"
#include "emm.h"
#include "ecm.h"
#include "nit.h"
#include "cas_ware.h"

// ap headers
#include "ap_framework.h"
#include "ap_cas.h"
#include "sys_cfg.h"


//#define WRITE_DATA_TO_MEM

extern void cas_manager_config_init(void);
static void init_ca_module(cas_module_id_t cam_id)
{
  //OS_PRINTF("[CA]===>%s, cam_id:%d\n", __FUNCTION__, cam_id);

  if (cam_id == CAS_UNKNOWN)
  {
    cas_module_init(CAS_ID_ONLY1);
  }
  else
  {
    cas_module_init(cam_id);
  }
}

static void def_test_ca_performance(cas_step_t step, u32 ticks)
{
}

static u32 get_msgq_timeout(class_handle_t handle)
{
 	return 30;
}

cas_policy_t* construct_cas_policy(void)
{
  cas_policy_t *p_policy = mtos_malloc(sizeof(cas_policy_t));

  MT_ASSERT(p_policy != NULL);

  memset(p_policy, 0, sizeof(cas_policy_t));
#ifndef WIN32
  p_policy->on_init = cas_manager_config_init;
  p_policy->test_ca_performance = def_test_ca_performance;
  p_policy->init_ca_module = init_ca_module;
  p_policy->get_msgq_timeout = get_msgq_timeout;

  OS_PRINTF("[CA]===>%s\n", __FUNCTION__);
#endif
  return p_policy;
}


