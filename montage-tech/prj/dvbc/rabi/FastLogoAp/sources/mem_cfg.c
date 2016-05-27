/******************************************************************************/
/******************************************************************************/
#include "sys_define.h"
#include "sys_types.h"
#include "sys_cfg.h"

#include "string.h"

#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"

//drv
#include "common.h"

#include "mem_manager.h"

#include "drv_dev.h"
#include "vdec.h"

#include "mem_cfg.h"
#ifdef __LINUX__
static  unsigned char * g_epg_buf = NULL;
static  unsigned char * g_scan_buf = NULL;
#endif
extern u32 get_mem_addr(void);

static const partition_block_configs_t block_cfg_normal[STATIC_BLOCK_NUM] =
{
  {BLOCK_AV_BUFFER, AV_BUFFER_ADDR, AV_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_GUI_VSURF_BUFFER, GUI_VSURF_BUFFER_ADDR, GUI_VSURF_BUFFER_SIZE, MEM_BLOCK_NOT_SHARED},
  {BLOCK_EPG_BUFFER, EPG_BUFFER_ADDR, EPG_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_REC_BUFFER, REC_BUFFER_ADDR, REC_BUFFER_SIZE, MEM_BLOCK_SHARED},
  {BLOCK_PLAY_BUFFER, PLAY_BUFFER_ADDR, PLAY_BUFFER_SIZE, MEM_BLOCK_NOT_SHARED},
  {BLOCK_UI_RESOURCE_BUFFER, GUI_RESOURCE_BUFFER_ADDR, GUI_RESOURCE_BUFFER_SIZE, MEM_BLOCK_NOT_SHARED},
  {BLOCK_GUI_ANIM_BUFFER, GUI_ANIM_BUFFER_ADDR, GUI_ANIM_BUFFER_SIZE, MEM_BLOCK_NOT_SHARED},
};

void mem_cfg(mem_cfg_t cfg)
{
  u16 i = 0;
  u32 base = 0;
  BOOL ret = FALSE;
  partition_block_configs_t curn_blk_cfg[STATIC_BLOCK_NUM] = {{0}};

#ifdef __LINUX__
  g_epg_buf = (unsigned char *)malloc(EPG_BUFFER_SIZE);
  if(g_epg_buf)
  {
    printf("[%s] =succes allock buffer [%d] g_epg_buf[0x%x]=\n\n",__func__,EPG_BUFFER_SIZE,g_epg_buf); 
    memset(g_epg_buf,0,EPG_BUFFER_SIZE);
  }

  g_scan_buf = (unsigned char *)malloc(SCAN_BUFFER_SIZE);
  if(g_scan_buf)
  {
    printf("[%s] =succes allock buffer [%d] g_scan_buf[0x%x]=\n\n",__func__,EPG_BUFFER_SIZE,g_epg_buf); 
    memset(g_scan_buf,0,SCAN_BUFFER_SIZE);
  }
  
#endif

  switch(cfg)
  {
    case MEMCFG_T_NORMAL:
      memcpy(curn_blk_cfg, block_cfg_normal, sizeof(curn_blk_cfg));
      break;

    default:
      MT_ASSERT(0);
      return;
  }
  // offset to base
  base = get_mem_addr();
  for (i = 0; i < STATIC_BLOCK_NUM; i++)
  {
    curn_blk_cfg[i].addr += base;
  }

#ifdef __LINUX__
  curn_blk_cfg[4].addr = g_epg_buf ;
  curn_blk_cfg[5].addr = g_scan_buf ;
#endif
  ret = mem_mgr_config_blocks(curn_blk_cfg, STATIC_BLOCK_NUM);
  MT_ASSERT(ret != FALSE);
}




