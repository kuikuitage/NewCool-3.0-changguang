#include "ui_common.h"
#include "ui_dump_by_usb.h"
#include "ui_keyboard_v2.h"

typedef enum
{
  IDC_INVALID = 0,
  IDC_DUMP_BY_USB_MODE,
  IDC_DUMP_FILENAME,  
  IDC_DUMP_BY_USB_START,
  IDC_DUMP_BY_USB_PBAR,
  IDC_DUMP_BY_USB_STS,
}dump_by_usb_control_id_t;

static u16 dump_by_usb_cont_keymap(u16 key);
static u16 ui_usb_dump_evtmap(u32 evt);
static RET_CODE dump_by_usb_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE dump_by_usb_start_text_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE dump_by_usb_mode_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE dump_by_usb_filename_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);

static u32 ui_usb_dump_sts = UI_USB_DUMP_IDEL;
static BOOL is_replace_old_file = TRUE;
static usb_upg_start_params_t usb_param = {0};
static BOOL usb_upg_deinit_flag = FALSE;
static u16 g_flash_all_name[STS_STR_DUMP_MAX_LEN] = {0};
static u16 g_user_db_name[STS_STR_DUMP_MAX_LEN] = {0};

extern u8 g_userdb_head_data[USB_UPG_MAX_HEAD_SIZE];
extern u32 get_flash_addr(void);
static void set_replace_old_file(void)
{
    is_replace_old_file = TRUE;
}

static void set_give_up_replace(void)
{
    is_replace_old_file = FALSE;
}

void ui_release_dump(void)
{

   ui_release_usb_upgrade(ROOT_ID_DUMP_BY_USB);

	ui_init_signal();
	ui_enable_playback(TRUE);
	ui_epg_init();
#ifndef IMPL_NEW_EPG
	ui_epg_start();
#else
	ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#endif
	ui_time_init();
	ui_set_book_flag(TRUE);

    fw_unregister_ap_msghost(APP_USB_UPG, ROOT_ID_DUMP_BY_USB);
    fw_unregister_ap_evtmap(APP_USB_UPG);
	return;
}
static void ui_init_usb_dump(ap_evtmap_t p_evtmap)
{
	cmd_t cmd = {0};

	#ifdef CORE_DUMP_DEBUG
	  mtos_task_suspend(TRANSPORT_SHELL_TASK_PRIORITY);
	#endif

	ui_epg_stop();
	ui_epg_release();
	ui_enable_playback(FALSE);
	ui_release_signal();
	ui_set_book_flag(FALSE);
	ui_time_release();

	cmd.id = AP_FRM_CMD_ACTIVATE_APP;
	cmd.data1 = APP_USB_UPG;
	ap_frm_do_command(APP_FRAMEWORK, &cmd);
	fw_register_ap_evtmap(APP_USB_UPG, p_evtmap);
	fw_register_ap_msghost(APP_USB_UPG, ROOT_ID_DUMP_BY_USB);

	return;
}
u32 ui_usb_dumping_sts(void)
{
  return ui_usb_dump_sts;
}

void ui_dump_by_usb_sts_show(control_t *p_cont, u16 *p_str)
{
  control_t *p_text;
  p_text = ctrl_get_child_by_id(p_cont, 1);
  text_set_content_by_ascstr(p_text, " ");
  text_set_content_by_unistr(p_text, p_str);
  ctrl_paint_ctrl(p_cont, TRUE);
}

RET_CODE dump_by_usb_start(void *p_param)
{
  cmd_t cmd = {0};

  cmd.id = USB_DUMP_CMD_START_DUMP;
  cmd.data1 = (u32)p_param;

  ap_frm_do_command(APP_USB_UPG, &cmd);
  return SUCCESS;
}

RET_CODE open_dump_by_usb(u32 para1, u32 para2)
{
  control_t *p_cont/*, *p_misc_list, *p_img_list*/, *p_ctrl[DUMP_BY_USB_ITEM_CNT],*p_sts;
//  control_t *p_sts[DUMP_BY_USB_STS_ITEM_CNT];
  u8 i, j;
  u16 stxt [DUMP_BY_USB_ITEM_CNT] = { IDS_DUMP_MODE, IDS_NAME, IDS_START };
  u16 smode[DUMP_BY_USB_MODE_CNT] =
    {
      IDS_UPGRADE_ALL_CODE, IDS_USER_DB,
    };
  u16 y;
  u16 filename[STS_STR_DUMP_MAX_LEN] = {0};

  p_cont = ui_comm_right_root_create(ROOT_ID_DUMP_BY_USB, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, dump_by_usb_cont_keymap);
  ctrl_set_proc(p_cont, dump_by_usb_cont_proc);

  y = DUMP_BY_USB_ITEM_Y;
  for (i = 0; i < DUMP_BY_USB_ITEM_CNT; i++)
  {
    p_ctrl[i] = NULL;
    switch (i)
    {
      case 0://mode
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_DUMP_BY_USB_MODE+ i),
          DUMP_BY_USB_ITEM_X, y, DUMP_BY_USB_ITEM_LW, DUMP_BY_USB_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
          DUMP_BY_USB_MODE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < DUMP_BY_USB_MODE_CNT; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, smode[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], 0);
        ui_comm_ctrl_set_proc(p_ctrl[i], dump_by_usb_mode_proc);
        break;

       case 1:
        p_ctrl[i] = ui_comm_t9_v2_edit_create(p_cont, (u8)(IDC_DUMP_BY_USB_MODE+ i),
                                              DUMP_BY_USB_ITEM_X, y, DUMP_BY_USB_ITEM_LW, 
                                              DUMP_BY_USB_ITEM_RW, ROOT_ID_DUMP_BY_USB);                                            
        ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], dump_by_usb_filename_proc);   
        if(g_flash_all_name[0] == 0)
        {
           str_asc2uni("demo_flash.bin", filename);
           uni_strcpy(g_flash_all_name, filename); 
        }
        
        if(g_user_db_name[0] == 0)
        {
           str_asc2uni("user_db.bin", filename);
           uni_strcpy(g_user_db_name, filename); 
        }
        
        ui_comm_t9_v2_edit_set_content_by_unistr(p_ctrl[i], g_flash_all_name); 
       
        break;       

      case 2://start
        p_ctrl[i] = ui_comm_static_create(p_cont, (u8)(IDC_DUMP_BY_USB_MODE + i),
          DUMP_BY_USB_ITEM_X, y, DUMP_BY_USB_ITEM_LW, DUMP_BY_USB_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], dump_by_usb_start_text_proc);

        y += DUMP_BY_USB_ITEM_H + DUMP_BY_USB_ITEM_V_GAP;
        break;
      case 3://pbar
        p_ctrl[i] = ui_comm_bar_create(p_cont, (u8)(IDC_DUMP_BY_USB_MODE + i),
          DUMP_BY_USB_PBAR_X, (y + (DUMP_BY_USB_TXT_H - DUMP_BY_USB_PBAR_H)/2),
          DUMP_BY_USB_PBAR_W, DUMP_BY_USB_PBAR_H,
          DUMP_BY_USB_TXT_X, y, DUMP_BY_USB_TXT_W, DUMP_BY_USB_TXT_H,
          DUMP_BY_USB_PER_X, y, DUMP_BY_USB_PER_W, DUMP_BY_USB_PER_H);
        ui_comm_bar_set_param(p_ctrl[i], RSC_INVALID_ID, 0, 100, 100);
        ui_comm_bar_set_style(p_ctrl[i],
                                RSI_DUMP_BY_USB_PBAR_BG, RSI_DUMP_BY_USB_PBAR_MID,
                                RSI_IGNORE, FSI_WHITE,
                                RSI_PBACK, FSI_WHITE);
        ui_comm_bar_update(p_ctrl[i], 0, TRUE);
        break;
      case 4://dump_by_usb status.
        p_ctrl[i] = ctrl_create_ctrl(CTRL_CONT, (u8)(IDC_DUMP_BY_USB_MODE + i),
          DUMP_BY_USB_ITEM_X, y, (DUMP_BY_USB_ITEM_LW + DUMP_BY_USB_ITEM_RW),
          ((DUMP_BY_USB_ITEM_H + DUMP_BY_USB_ITEM_V_GAP) * DUMP_BY_USB_STS_ITEM_CNT - DUMP_BY_USB_ITEM_V_GAP),
          p_cont, 0);
	 ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH);
        break;
      default:
        MT_ASSERT(0);
        break;
    }

    y += DUMP_BY_USB_ITEM_H + DUMP_BY_USB_ITEM_V_GAP;

    ctrl_set_related_id(p_ctrl[i],
                      0,                       /* left */
                      (u8)((i - 1 + 3)
                           % 3 + 1),    /* up */
                      0,                       /* right */
                      (u8)((i + 1) % 3 + 1));    /* down */
    
  }

  //create status item bar.
  y = 0;

  p_sts = ctrl_create_ctrl(CTRL_TEXT, 1,
  						0, 0,
  						(DUMP_BY_USB_ITEM_LW + DUMP_BY_USB_ITEM_RW),
  						DUMP_BY_USB_ITEM_H, p_ctrl[IDC_DUMP_BY_USB_STS - 1], 0);
  ctrl_set_rstyle(p_sts, RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH);
  text_set_align_type(p_sts, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_sts, FSI_COMM_CTRL_SH,
        FSI_COMM_CTRL_HL, FSI_COMM_CTRL_GRAY);
  text_set_content_type(p_sts, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_sts, " ");

  //ctrl_set_related_id(p_ctrl[0], 0, IDC_DUMP_BY_USB_START, 0, IDC_DUMP_BY_USB_START);
  //ctrl_set_related_id(p_ctrl[1], 0, IDC_DUMP_BY_USB_MODE, 0, IDC_DUMP_BY_USB_MODE);

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  ui_init_usb_dump(ui_usb_dump_evtmap);
  usb_upg_deinit_flag = FALSE;

  return SUCCESS;
}


static RET_CODE on_dump_by_usb_quit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  ui_usb_dump_sts = UI_USB_DUMP_IDEL;
  if(usb_upg_deinit_flag == FALSE)
  {
	  ui_release_dump();
	  usb_upg_deinit_flag = TRUE;
  }
  manage_close_menu(ROOT_ID_DUMP_BY_USB, 0, 0);

  return SUCCESS;
}

static RET_CODE on_dump_by_usb_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(UI_USB_DUMP_IDEL != ui_usb_dump_sts)
	return SUCCESS;

  if(usb_upg_deinit_flag == FALSE)
  {
	  ui_release_dump();
	  usb_upg_deinit_flag = TRUE;
  }
  return ERR_NOFEATURE;
}
static RET_CODE on_dump_destroy(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  if(usb_upg_deinit_flag == FALSE)
  {
	  ui_release_dump();
	  usb_upg_deinit_flag = TRUE;
  }
  mtos_printk("@@@@@ %s,%d\n\n",__FUNCTION__,__LINE__);
  return ERR_NOFEATURE;
}

static RET_CODE ui_refresh_dump_menu(control_t *p_cont)
{
   control_t *p_start, *p_pbar, *p_mode, *p_sts, *p_filename;

  p_start = ctrl_get_child_by_id(p_cont, IDC_DUMP_BY_USB_START);
  ui_comm_ctrl_update_attr(p_start, TRUE);
  ctrl_process_msg(p_start, MSG_GETFOCUS, 0, 0);

  p_mode = ctrl_get_child_by_id(p_cont, IDC_DUMP_BY_USB_MODE);
  ui_comm_ctrl_update_attr(p_mode, TRUE);
  p_filename = ctrl_get_child_by_id(p_cont, IDC_DUMP_FILENAME);
  ui_comm_ctrl_update_attr(p_filename, TRUE);

  p_pbar = ctrl_get_child_by_id(p_cont, IDC_DUMP_BY_USB_PBAR);
  ui_comm_bar_update(p_pbar, 0, TRUE);

  p_sts = ctrl_get_child_by_id(p_cont, IDC_DUMP_BY_USB_STS);
  text_set_content_by_ascstr(ctrl_get_child_by_id(p_sts, 1), " ");

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_dump_by_usb_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  comm_dlg_data_t p_data =
  	{
  	    ROOT_ID_INVALID,
 	    DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
 	    COMM_DLG_X, COMM_DLG_Y,
 	    COMM_DLG_W,COMM_DLG_H,
 	    IDS_DUMP_SUCCESS,
 	    0	,
  	};
    ui_comm_dlg_open(&p_data);
    ui_usb_dump_sts = UI_USB_DUMP_IDEL;

    ui_refresh_dump_menu(p_ctrl);

  return SUCCESS;
}

static RET_CODE on_dump_by_usb_fail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  comm_dlg_data_t p_data = 
  	{
  	    ROOT_ID_INVALID,
 	    DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
 	    COMM_DLG_X, COMM_DLG_Y,
 	    COMM_DLG_W,COMM_DLG_H,
 	    IDS_DUMP_FAIL,
 	    0	,
  	};

  if(UI_USB_DUMP_IDEL != ui_usb_dump_sts)
  {
    ui_set_front_panel_by_str_with_upg("ERR",0);
    ui_comm_dlg_open(&p_data);
    ui_usb_dump_sts = UI_USB_DUMP_IDEL;
    ui_refresh_dump_menu(p_ctrl);
  }
  return SUCCESS;
}

static RET_CODE on_usb_dump_read_flash_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u16 uni_str[STS_STR_DUMP_MAX_LEN] = {0};
  control_t *p_bar = ui_comm_right_root_get_ctrl(ROOT_ID_DUMP_BY_USB, IDC_DUMP_BY_USB_PBAR);

  if(NULL != p_bar)
  {
      ui_comm_bar_update(p_bar, 0, TRUE);
      ui_comm_bar_paint(p_bar, TRUE);
  }
  gui_get_string(IDS_WRITE_FILE, uni_str, STS_STR_DUMP_MAX_LEN);
  ui_dump_by_usb_sts_show(ctrl_get_child_by_id(p_ctrl, IDC_DUMP_BY_USB_STS), uni_str);

  return SUCCESS;
}
static RET_CODE on_dump_by_usb_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  upg_status_t *sts = (upg_status_t *)para1;
  control_t *p_bar = ui_comm_right_root_get_ctrl(ROOT_ID_DUMP_BY_USB, IDC_DUMP_BY_USB_PBAR);

  if(UI_USB_DUMP_IDEL != ui_usb_dump_sts)
  {
  	switch(sts->sm)
  	{
  		case USB_SM_REMOTE_ERROR:
			break;
		case USB_DUMP_SM_WRITING_FILE:
			ui_comm_bar_update(p_bar, (u16)sts->progress, TRUE);
           ui_set_front_panel_by_str_with_upg("Write",(u8)sts->progress);
			ui_comm_bar_paint(p_bar, TRUE);
      case USB_DUMP_SM_READING_FLASH:
           ui_comm_bar_update(p_bar, (u16)sts->progress, TRUE);
           ui_set_front_panel_by_str_with_upg("Read",(u8)sts->progress);
			ui_comm_bar_paint(p_bar, TRUE);
			break;
		default:
			return ERR_FAILURE;
			break;
  	}
  }
  return SUCCESS;
}
/*
static RET_CODE on_create_file_name_with_dir(u16 *p_name, u16 mode)
{
  u8 version[20] = {0};
  u16 uni_tmp[32] = {0};

  switch(mode)
  {
    case 0:
      dm_read(class_get_handle_by_id(DM_CLASS_ID), SS_DATA_BLOCK_ID, 0, 0, 20, version);
      str_asc2uni("C:\\demo_", p_name);
      str_asc2uni(version, uni_tmp);
      uni_strcat(p_name, uni_tmp, STS_STR_DUMP_MAX_LEN);
      
      str_asc2uni(".bin", uni_tmp);
      uni_strcat(p_name,uni_tmp, STS_STR_DUMP_MAX_LEN);
      OS_PRINTF("@@@@ 341p_name = %s @@@@\n",p_name);
      break;
      
    case 1:
      str_asc2uni("C:\\user_db.bin", p_name);
      break;
    default:
      MT_ASSERT(0);
  }
  if(ui_is_file_exist_in_usb(p_name))
  {
    ui_comm_ask_for_dodlg_open(NULL, IDS_SAME_FILE,
                                                set_replace_old_file,
                                                set_give_up_replace, 0);
    if(!is_replace_old_file)
    {
      return ERR_FAILURE;
    }
  }

  return SUCCESS;
}
*/
static RET_CODE on_dump_by_usb_start_text_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont,*p_mode,*p_start,*p_sts,*p_ctrl_file = NULL;
  u16 mode_focus;
  u16 dump_file_name[STS_STR_DUMP_MAX_LEN] = {0};
  RET_CODE ret;
  u16 uni_str[STS_STR_DUMP_MAX_LEN] = {0};
  handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  u16 *p_filename = NULL;
  
  memset(dump_file_name, 0, STS_STR_DUMP_MAX_LEN*sizeof(u16));
  if(UI_USB_DUMP_IDEL == ui_usb_dump_sts)
  {
    ui_usb_dump_sts = UI_USB_DUMP_PRE_READ;
    p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));
    p_mode = ctrl_get_child_by_id(p_cont, IDC_DUMP_BY_USB_MODE);
    p_start = ctrl_get_child_by_id(p_cont, IDC_DUMP_BY_USB_START);
    p_sts = ctrl_get_child_by_id(p_cont, IDC_DUMP_BY_USB_STS);
    p_ctrl_file = ctrl_get_child_by_id(p_cont, IDC_DUMP_FILENAME);
    mode_focus = ui_comm_select_get_focus(p_mode);

    //
    p_filename = ui_comm_t9edit_get_content(p_ctrl_file);
    uni_strcpy(dump_file_name, p_filename);
    //
    if(ui_is_file_exist_in_usb(dump_file_name))
    {
      ui_comm_ask_for_dodlg_open(NULL, IDS_SAME_FILE,
                                                  set_replace_old_file,
                                                  set_give_up_replace, 0);
      if(!is_replace_old_file)
      {
        ui_usb_dump_sts = UI_USB_DUMP_IDEL;
        return ERR_FAILURE;
      }
    }

    switch (mode_focus)
    {
      case 0:  // all code
        usb_param.ext_buf_addr = mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
        usb_param.ext_buf_size = GOBY_FLASH_SIZE;
        usb_param.flash_offset_addr = 0;
        usb_param.head_size = 0;
        memset(usb_param.head_data, 0 ,sizeof(usb_param.head_data));
        break;
      case 1:   //user db
        usb_param.ext_buf_addr = mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
        usb_param.head_size = USB_UPG_MAX_HEAD_SIZE;
        memcpy(usb_param.head_data, g_userdb_head_data, usb_param.head_size);
        usb_param.ext_buf_size = dm_get_block_size(dm_handle, IW_TABLE_BLOCK_ID) + \
							dm_get_block_size(dm_handle, IW_VIEW_BLOCK_ID) + usb_param.head_size;
        usb_param.flash_offset_addr = dm_get_block_addr(dm_handle, IW_TABLE_BLOCK_ID) \
                                              - get_flash_addr(); //NORF_BASE_ADDR
        break;
      default:
        MT_ASSERT(0);
        break;
    }

    usb_param.mode = (u8)mode_focus;
    memset(usb_param.file, 0, MAX_FILE_PATH * sizeof(u16));
    uni_strncpy(usb_param.file,dump_file_name,STS_STR_DUMP_MAX_LEN);
    mem_mgr_release_block(BLOCK_AV_BUFFER);
    ret = dump_by_usb_start((void *)&usb_param);

    if(SUCCESS != ret)
    {
      ui_usb_dump_sts = UI_USB_DUMP_IDEL;
      return ERR_FAILURE;
    }

    ui_comm_ctrl_update_attr(p_mode, FALSE);
    ctrl_process_msg(p_start, MSG_LOSTFOCUS, 0, 0);
    ui_comm_ctrl_update_attr(p_start, FALSE);
    ui_comm_ctrl_update_attr(p_ctrl_file, FALSE);
    gui_get_string(IDS_READ_FLASH, uni_str, STS_STR_DUMP_MAX_LEN);
    ui_dump_by_usb_sts_show(p_sts,uni_str);
    ctrl_paint_ctrl(p_cont, TRUE);
    ui_usb_dump_sts = UI_USB_DUMP_READING;
  }

  return SUCCESS;
}

static RET_CODE edit_filename_update(u16 *p_unistr)
{
  control_t *p_edit_cont = NULL, *p_mode = NULL;
  u16 focus = 0;
  u16 *filename = NULL;
  
  p_edit_cont = ui_comm_right_root_get_ctrl(ROOT_ID_DUMP_BY_USB, IDC_DUMP_FILENAME);
  if(p_edit_cont == NULL)
  {
    return ERR_FAILURE;
  }

  p_mode = ui_comm_right_root_get_ctrl(ROOT_ID_DUMP_BY_USB, IDC_DUMP_BY_USB_MODE);
  if(p_mode == NULL)
  {
    return ERR_FAILURE;
  }  

  filename = ui_comm_t9_v2_edit_get_content(p_edit_cont);
  focus = ui_comm_select_get_focus(p_mode);
  if(focus == 0)
  {
      uni_strcpy(g_flash_all_name, filename);  
  }
  else
  {
      uni_strcpy(g_user_db_name, filename);   
  }

  
  ctrl_paint_ctrl(p_edit_cont, TRUE);

  return SUCCESS;

}

static RET_CODE on_dump_usb_filename_edit(control_t *p_ctrl,
                                                     u16 msg,
                                                     u32 para1,
                                                     u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.max_len = STS_STR_DUMP_MAX_LEN-1;
  param.cb = edit_filename_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  
  return SUCCESS;
}

static RET_CODE on_dump_by_usb_mode_change_focus(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_parent = NULL, *p_cont = NULL, *p_filename = NULL;
  u16 focus = 0;
  
  p_parent = ctrl_get_parent(p_ctrl);
  if(p_parent)
  {
     p_cont = ctrl_get_parent(p_parent);
     if(p_cont != NULL)
     {
        p_filename = ctrl_get_child_by_id(p_cont, IDC_DUMP_FILENAME);
     }

     if(p_filename == NULL)
     {
       return ERR_FAILURE;
     }
     
     focus = ui_comm_select_get_focus(p_parent);
     if(focus == 0)
     {
         ui_comm_t9_v2_edit_set_content_by_unistr(p_filename, g_user_db_name);      
     }
     else if(focus == 1)
     {
         ui_comm_t9_v2_edit_set_content_by_unistr(p_filename, g_flash_all_name); 
     }
     ctrl_paint_ctrl(p_filename, TRUE);
  }
  
  cbox_class_proc(p_ctrl, msg, 0, 0);
  return SUCCESS; 
}

BEGIN_KEYMAP(dump_by_usb_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
END_KEYMAP(dump_by_usb_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(dump_by_usb_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_dump_by_usb_exit)
  ON_COMMAND(MSG_EXIT, on_dump_by_usb_exit)
  ON_COMMAND(MSG_POWER_OFF, on_dump_by_usb_exit)
  ON_COMMAND(MSG_UPG_OK, on_dump_by_usb_ok)
  ON_COMMAND(MSG_UPG_UPDATE_STATUS, on_dump_by_usb_update)
  ON_COMMAND(MSG_DUMP_READ_FLASH_OK,on_usb_dump_read_flash_ok)
  ON_COMMAND(MSG_DUMP_FAIL, on_dump_by_usb_fail)
  ON_COMMAND(MSG_UPG_QUIT, on_dump_by_usb_quit)
  ON_COMMAND(MSG_DESTROY,on_dump_destroy)
END_MSGPROC(dump_by_usb_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(dump_by_usb_mode_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_dump_by_usb_mode_change_focus)
  ON_COMMAND(MSG_DECREASE, on_dump_by_usb_mode_change_focus)
END_MSGPROC(dump_by_usb_mode_proc, cbox_class_proc)


BEGIN_MSGPROC(dump_by_usb_filename_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_dump_usb_filename_edit)
  ON_COMMAND(MSG_NUMBER, on_dump_usb_filename_edit)
END_MSGPROC(dump_by_usb_filename_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(dump_by_usb_start_text_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_dump_by_usb_start_text_select)
END_MSGPROC(dump_by_usb_start_text_proc, text_class_proc)

BEGIN_AP_EVTMAP(ui_usb_dump_evtmap)
  CONVERT_EVENT(USB_UPG_EVT_UPDATE_STATUS, MSG_UPG_UPDATE_STATUS)
  CONVERT_EVENT(USB_DUMP_WRITE_FILE_SUCCESS, MSG_UPG_OK)
  CONVERT_EVENT(USB_DUMP_READ_FLASH_SUCCESS, MSG_DUMP_READ_FLASH_OK)
  CONVERT_EVENT(USB_DUMP_CREATE_FILE_ERROR,MSG_DUMP_FAIL)
  CONVERT_EVENT(USB_DUMP_WRITE_FILE_ERROR,MSG_DUMP_FAIL)
END_AP_EVTMAP(ui_usb_dump_evtmap)
