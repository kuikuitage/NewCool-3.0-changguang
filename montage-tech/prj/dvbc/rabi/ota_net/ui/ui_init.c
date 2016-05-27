/****************************************************************************

 ****************************************************************************/
/**
 * \file ui_init.c
 *
 * This file implemented the entry function of shadow project which is the
 * default project of dvbs products
 */

#include "ui_common.h"
#include "data_manager.h"
#include "rsc.h"
#include "video.h"
#include "pti.h"
#include "dvb_svc.h"
//#include "ap_galaxy_ota.h"
#include "ui_ota_api.h"

#include "mem_cfg.h"
#include "ap_ota.h"
#include "sys_status.h"

static RET_CODE mmi_init(void)
{
  u32 p_addr;
  handle_t rsc_handle = {0};
  gdi_config_t info =
  {
    FALSE,
    {
      SURFACE_OSD0,
    },
    
    {
      SURFACE_OSD1,
      /* color format */
      COLORFORMAT_ARGB8888,
      /* screen rectangle */
      {SCREEN_POS_PAL_L, SCREEN_POS_PAL_T,
       SCREEN_POS_PAL_L + SCREEN_WIDTH,
       SCREEN_POS_PAL_T + SCREEN_HEIGHT},
      /* palette */
      NULL,
      /* default color */
      C_TRANS,
      /* color key */
      C_KEY,
      /* osd odd vmem */
      NULL,
      /* odd vmem size + ctrlword + palette */
      0,//OSD0_ODD_MEMSIZE,
      /* osd even vmem */
      NULL,
      /* even vmem size*/
      0,//OSD0_EVEN_MEMSIZE,
    },
    
    /* virtual surface addr */
    0,
    /* virtual surface size */
    OSD0_32BIT_BUFFER_SIZE,
    /* surface cnt */
    MAX_SURFACE_CNT,
    /* cliprect cnt */
    MAX_CLIPRECT_CNT,
    /* dc cnt */
    MAX_DC_CNT,
  };
  paint_param_t paint_param = {0};
//  vfont_config_t vfont_cfg = {0};

  UI_PRINTF("\r\n ota screen format %d ", info.bottom.format);
  // init osd frame buffer
  p_addr = mem_mgr_require_block(BLOCK_OSD1_32BIT_BUFFER,
                      SYS_MODULE_GDI);
  MT_ASSERT(p_addr != 0);

  mem_mgr_release_block(BLOCK_OSD1_32BIT_BUFFER);

  info.bottom.p_odd_addr = (u32 *)p_addr;
  info.bottom.p_even_addr = (u32 *)(p_addr + info.bottom.odd_size);

  mmi_init_heap(GUI_PARTITION_SIZE);

  g_rsc_config.rsc_data_addr = (u32)ota_rsc_data;

  rsc_handle = rsc_init(&g_rsc_config);
  UI_PRINTF("\r\n ota rsc_init is ok ");

  rsc_set_curn_language(rsc_handle, 1);


  info.vsurf_buf_addr = mem_mgr_require_block(BLOCK_OSD0_32BIT_BUFFER, SYS_MODULE_GDI);
  MT_ASSERT(p_addr != 0);

  info.vsurf_buf_size = mem_mgr_get_block_size(BLOCK_OSD0_32BIT_BUFFER);

  mem_mgr_release_block(BLOCK_OSD0_32BIT_BUFFER);

  gdi_init_v2(&info);

  gdi_set_global_alpha(255);

  ctrl_init_lib(MAX_CLASS_CNT, MAX_OBJ_CNT);
  ctrl_str_init(MAX_CTRL_STRING_BUF_SIZE);
#if 0
  cont_register_class(MAX_OBJECT_NUMBER_CNT);
  cbox_register_class(MAX_OBJECT_NUMBER_CBB);
  text_register_class(MAX_OBJECT_NUMBER_TXT);
  pbar_register_class(MAX_OBJECT_NUMBER_PGB);
  nbox_register_class(MAX_OBJECT_NUMBER_NUM);
#else
  cont_register_class(MAX_OBJECT_NUMBER_CNT);
  text_register_class(MAX_OBJECT_NUMBER_TXT);
  bmap_register_class(MAX_OBJECT_NUMBER_BMP);
  cbox_register_class(MAX_OBJECT_NUMBER_CBB);
  list_register_class(MAX_OBJECT_NUMBER_LIST);
  mbox_register_class(MAX_OBJECT_NUMBER_MTB);
  pbar_register_class(MAX_OBJECT_NUMBER_PGB);
  sbar_register_class(MAX_OBJECT_NUMBER_SCB);
  ebox_register_class(MAX_OBJECT_NUMBER_EDT);
  nbox_register_class(MAX_OBJECT_NUMBER_NUM);
  tbox_register_class(MAX_OBJECT_NUMBER_TIM);
  sbox_register_class(MAX_OBJECT_NUMBER_SBX);
  ipbox_register_class(MAX_OBJECT_NUMBER_IP);
#endif
  //gui_paint_init(MAX_PAINT_STRING_LENGTH, MAX_PAINT_STRING_LINES);

  paint_param.max_str_len = MAX_PAINT_STRING_LENGTH;
  paint_param.max_str_lines = MAX_PAINT_STRING_LINES;
  paint_param.rsc_handle = rsc_handle;
  
  gui_paint_init(&paint_param);

  gui_roll_init(MAX_ROLL_CNT, ROLL_PPS);
#if 0
  //vfont_cfg.is_uncached = TRUE;
  vfont_cfg.max_cnt = 36; //a~z, 0~9
  vfont_cfg.max_height = 104;
  vfont_cfg.max_width = 104;
  vfont_cfg.is_alpha_spt = TRUE;
  vfont_cfg.Bpp = 4;
  
  rsc_ft_attach(rsc_handle, &vfont_cfg);
#endif
  UI_PRINTF("\r\n ota all classes is ok\n");
  return SUCCESS;
}

//extern void ota_read_otai(ota_info_t *p_otai);
//static ota_info_t ota_info = {0};
extern ota_trigger_t ota_tri;


static void ui_main_proc(void *p_param)
{
  //ota_info_t *p_otai = NULL;
  mmi_init();
  
  ui_desktop_init(); 
  /*
  //ota_read_otai(&ota_info);
  if(OTA_TRI_AUTO == ota_tri)
  {
    ota_read_otai(&ota_info);
    //ui_ota_start(TRUE);
  }
  else
  {
    ota_info.ota_tri = ota_tri;
    ota_info.sys_mode = SYS_DVBS;
    ota_info.download_data_pid = 0x1FE0;
    ota_info.locks.disepc_rcv.is_fixed = 1;
    ota_info.locks.sat_rcv.lnb_high = 5150;
    ota_info.locks.sat_rcv.lnb_low = 5150;
    ota_info.locks.tp_rcv.sym = 30000;
    ota_info.locks.tp_rcv.freq = 4100;
    ota_info.locks.tp_rcv.nim_type = NIM_DVBS;  
    if(OTA_TRI_MON_IN_LOADER == ota_tri)
    {
      //p_otai = sys_status_get_ota_info();
      ota_info.orig_software_version = sys_status_get_sw_version();		
    }
    else
    {
      ota_info.orig_software_version = 0;
    }

    OS_PRINTF("ota old version : %d\n", ota_info.orig_software_version);
    //ui_ota_check_main(&ota_info, ota_tri);
  }
  */
  manage_open_menu(ROOT_ID_SUBMENU, 0, 0); /*add for test*/
 // manage_open_menu(ROOT_ID_NETWORK_UPGRADE, 0, 0); /*add for test*/

  ui_desktop_main();
}


void ui_init(void)
{
  u32 *pstack_pnt = (u32 *)mtos_malloc(UI_FRM_TASK_STKSIZE);

  MT_ASSERT(pstack_pnt != NULL);
  memset(pstack_pnt, 0, UI_FRM_TASK_STKSIZE);

  /* start app task */
  mtos_task_create((u8 *)"ui_frm",
                   ui_main_proc,
                   NULL,
                   UI_FRM_TASK_PRIORITY,
                   pstack_pnt,
                   UI_FRM_TASK_STKSIZE);
}
