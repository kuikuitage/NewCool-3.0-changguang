#include "ui_common.h"
#include "ui_upgrade_by_usb.h"
#include "ap_usb_upgrade.h"
#include "ap_charsto_impl.h"
/************local variable define*****************************/
/*!
  header offset in flash.bin  
!*/
#define HEADER_OFFSET 16
/*!
  header size
!*/
#define HEADER_SIZE 12

typedef enum
{
  IDC_INVALID = 0,
  IDC_MODE,
  IDC_FILE,
  IDC_START,
  IDC_PBAR,
  IDC_STS,
  //IDC_VERSION,
  IDC_ITEM_MAX
}upgrade_by_usb_control_id_t;
#if 0
static list_xstyle_t version_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
};

static list_xstyle_t version_list_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t version_list_field_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
};

static list_field_attr_t version_list_attr[VERSION_LIST_FIELD] =
{
  { LISTFIELD_TYPE_STRID| STL_LEFT | STL_VCENTER,
    195, 5, 0, &version_list_field_rstyle,  &version_list_field_fstyle},

  { LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
    300,200, 0, &version_list_field_rstyle,  &version_list_field_fstyle},

  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    300, 500, 0, &version_list_field_rstyle,  &version_list_field_fstyle},
};
#endif
static rect_t g_upgrade_dlg_rc =
{
  UPGRADE_DLG_X,  UPGRADE_DLG_Y,
  UPGRADE_DLG_X + UPGRADE_DLG_W,
  UPGRADE_DLG_Y + UPGRADE_DLG_H,
};
static flist_dir_t flist_dir = NULL;
static partition_t *p_partition = NULL;
static u32 g_partition_cnt = 0;
static file_list_t g_list = {0};
static u16 g_ffilter_all[32] = {0};
static usb_upg_start_params_t usb_param = {0};

extern u8 g_userdb_head_data[USB_UPG_MAX_HEAD_SIZE];
extern u32 get_flash_addr(void);

static u16 upgrade_by_usb_cont_keymap(u16 key);
static RET_CODE upgrade_by_usb_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_usb_start_text_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_usb_mode_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_usb_file_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE on_upg_by_usb_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE on_upg_fail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static void _pre_burn(u8 mode);

static u32 ui_usb_upg_sts = UI_USB_UPG_IDEL;

u16 ui_usb_upg_evtmap(u32 event);

u32 ui_usb_upgade_sts(void)
{
  return ui_usb_upg_sts;
}

void ui_upgrade_by_usb_sts_show(control_t *p_cont, u16 *p_str)
{
  text_set_content_by_unistr(p_cont, p_str);
  ctrl_paint_ctrl(p_cont, TRUE);
}

void ui_refresh_items(control_t *p_cont)
{
  control_t *p_start, *p_pbar, *p_mode, *p_file;

  p_start = ctrl_get_child_by_id(p_cont, IDC_START);
  ui_comm_ctrl_update_attr(p_start, TRUE);
  ctrl_process_msg(p_start, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_start, TRUE);

  p_mode = ctrl_get_child_by_id(p_cont, IDC_MODE);
  ui_comm_ctrl_update_attr(p_mode, TRUE);
  ctrl_paint_ctrl(p_mode, TRUE);

  p_file = ctrl_get_child_by_id(p_cont, IDC_FILE);
  ui_comm_ctrl_update_attr(p_file, TRUE);
  ctrl_paint_ctrl(p_file, TRUE);

  p_pbar = ctrl_get_child_by_id(p_cont, IDC_PBAR);
  ui_comm_bar_update(p_pbar, 0, TRUE);
  ui_comm_bar_paint(p_pbar, TRUE);

}
static void ui_upgrade_giveup(void)
{
  control_t *p_ctrl_sts;
  control_t *p_root;

  ui_usb_upg_sts = UI_USB_UPG_IDEL;
  p_ctrl_sts = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_STS);
  p_root = ctrl_get_parent(p_ctrl_sts);
  ui_refresh_items(p_root);
}

static void ui_upgrade_start_burn(void)
{
  cmd_t cmd = {0};
  control_t *p_start,*p_bar,*p_sts = NULL;
  u16 uni_str[STS_STR_MAX_LEN] = {0};

  ui_usb_upg_sts = UI_USB_UPG_BURNING;
  
#if ENABLE_TEST_SPEED
    stop_testspeed();
#endif
  _pre_burn(usb_param.mode);
  
  cmd.id = USB_UPG_CMD_START_BURN;
  cmd.data1 = (u32)(&usb_param);
  ap_frm_do_command(APP_USB_UPG, &cmd);

  gui_get_string(IDS_BURN_FLASH, uni_str, STS_STR_MAX_LEN);
  p_sts = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_STS);
  p_start = ctrl_get_child_by_id(ctrl_get_parent(p_sts), IDC_START);
  p_bar = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_PBAR);
  ui_comm_ctrl_update_attr(p_start,FALSE);
  ui_comm_bar_update(p_bar, 0, TRUE);
  ctrl_paint_ctrl(p_start, TRUE);
  ctrl_paint_ctrl(p_bar, TRUE);
  ui_upgrade_by_usb_sts_show(p_sts, uni_str);
}

static BOOL _get_dm_block_head_info_from_upg_buf(u8 block_id, u32 dm_head_start, dmh_block_info_t *dest, u32 *block_head_offset)
{
  u32 blk_num = 0;
  u32 block_addr = 0;
  u32 p_upg_buf = usb_param.ext_buf_addr;
  u32 i = 0;
  u8 buf_block_id = 0;

  block_addr = p_upg_buf + dm_head_start + USB_UPG_DMH_INDC_SIZE + 16;
  blk_num = *(u16*)(p_upg_buf + dm_head_start + USB_UPG_DMH_INDC_SIZE + 12);

  for (i = 0; i < blk_num; i++) 
  {
    buf_block_id = *((u8 *)(block_addr));
    
    if (buf_block_id == block_id)
    {
      memcpy(dest, (u8 *)(block_addr), sizeof(dmh_block_info_t));
      *block_head_offset = block_addr - p_upg_buf;
      return TRUE;
    }
    
    block_addr += sizeof(dmh_block_info_t);
  }
  return FALSE;
}

static BOOL _get_dm_block_offset_from_upg_buf(u8 block_id, u32 *block_offset, u32 *block_head_offset)
{
  u32 i = 0;
  u32 dm_head_start = 0;
  u32 p_upg_buf = usb_param.ext_buf_addr;
  dmh_block_info_t dest;
  u32 head_offset;

  for (i = 0; i < usb_param.ext_buf_size; i++)
  {
    if(memcmp((u8*)p_upg_buf, g_userdb_head_data, USB_UPG_DMH_INDC_SIZE) == 0)
    {  
      dm_head_start = p_upg_buf - usb_param.ext_buf_addr;

      if(_get_dm_block_head_info_from_upg_buf(block_id, dm_head_start, &dest, &head_offset))
      {
        *block_offset = dest.base_addr + dm_head_start;
        *block_head_offset = head_offset;

        return TRUE;
      }
    }
    p_upg_buf += 1;
  }

  return FALSE;
}

static void _keep_serial_num()
{
  u8 *block =  NULL;
  u32 blockLen = 0;
  u32 dataLen = 0;
  u32 indentity_block_offset;
  u32 indentity_block_head_offset;
  dmh_block_info_t *p_block_info;

  blockLen = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID),IDENTITY_BLOCK_ID);
  if(blockLen == 0)
  {
    OS_PRINTF("[%s]:line:%d   block len fail \n",__FUNCTION__,__LINE__); 
    return;
  }

  block = (u8 *)mtos_malloc(blockLen);
  MT_ASSERT(block != NULL);
  
  if(sys_get_block_data_by_index(IDENTITY_BLOCK_ID,  block , blockLen, &dataLen))
  {
    if(_get_dm_block_offset_from_upg_buf(IDENTITY_BLOCK_ID, &indentity_block_offset, &indentity_block_head_offset))
    {
      p_block_info = (dmh_block_info_t *)(usb_param.ext_buf_addr + indentity_block_head_offset);
      if(dataLen> p_block_info->size)
      {
        return;
      }
      memcpy((u8 *)(usb_param.ext_buf_addr + indentity_block_offset), block, dataLen);
      memset((u8 *)(usb_param.ext_buf_addr + indentity_block_offset +dataLen), 0xFF, p_block_info->size - dataLen);
    }
  }
}

static void _ui_upgrade_start_download( void)
{
  u32 total_size = 0;
  u32 size = 0;
  u16 percent = 0;
  s64 off_set = 0;
  u8 *p_buf = NULL;
  hfile_t file = NULL;
  u32 downloaded_size = 0;
  control_t *p_pbar = NULL;

  file = vfs_open(usb_param.file, VFS_READ);
  if(NULL == file)
  {
    on_upg_fail(NULL, 0, 0, 0);
    return;
  }
  p_pbar = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_PBAR);
  total_size = usb_param.ext_buf_size;
  off_set = (usb_param.mode == UPG_MAIN_CODE) ? usb_param.flash_offset_addr : 0;
  vfs_seek(file, off_set, VFS_SEEK_SET);
  
  while(downloaded_size < total_size)
  {
    p_buf = (u8 *)(usb_param.ext_buf_addr + downloaded_size);
    size = ((downloaded_size + USB_UPG_TRANS_PACKET_SIZE) <= total_size)? USB_UPG_TRANS_PACKET_SIZE\
              : (total_size - downloaded_size);
    if(size != vfs_read(p_buf, size, 1, file))
    {
      on_upg_fail(NULL, 0, 0, 0);
      return;
    }
    downloaded_size += size;
    percent = (downloaded_size * 100) / total_size;

    //update ui
    ui_comm_bar_update(p_pbar, percent, TRUE);
    ui_set_front_panel_by_str_with_upg("D", (u8)percent);
    ui_comm_bar_paint(p_pbar, TRUE);
  }
  vfs_close(file);
  if(usb_param.mode == USB_UPGRD_ALL)
  {
    _keep_serial_num();
  }
  ui_comm_ask_for_dodlg_open(&g_upgrade_dlg_rc, IDS_SURE_BURN_FLASH,\
                                                      ui_upgrade_start_burn, ui_upgrade_giveup, 0);
  return ;
}
#if 0
static RET_CODE version_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 str_id[VERSION_LIST_CNT -1] = {IDS_MAINCODE};
  //upg_file_ver_t upg_file_version = {{0},{0},{0}};
  control_t *p_ctrl = NULL;
  u16 focus = 0;

  for(i = 0; i < (VERSION_LIST_CNT -1); i++)
  {
    list_set_field_content_by_strid(p_list, i +1, 0, str_id[i]);
  }

  if(NULL != p_list->p_parent->p_parent)
  {
    p_ctrl = ctrl_get_child_by_id( p_list->p_parent->p_parent, IDC_FILE);
    if(NULL != p_ctrl)
    {
      focus = ui_comm_select_get_focus(p_ctrl);
    }
  }
#if 0
  ui_get_upg_file_version(g_list.p_file[focus].name, &upg_file_version);
  upg_file_version.changeset[19] = '\0';

  list_set_field_content_by_ascstr(p_list, 0, 1, "Local Version");
  list_set_field_content_by_ascstr(p_list, 0, 2, "Bin Version");

  list_set_field_content_by_ascstr(p_list, 1, 1, sys_status_get()->changeset);
  list_set_field_content_by_ascstr(p_list, 1, 2, upg_file_version.changeset);
#endif
  return SUCCESS;
}
#endif
static RET_CODE upgrade_file_update(control_t *p_ctrl, u16 focus,
                                       u16 *p_str, u16 max_length)
{
  u16 uni_str[32] = {0};

  if(0 == g_list.file_count)
  {
    str_nasc2uni("No file", uni_str, DB_DVBS_MAX_NAME_LENGTH);
  }
  else
  {
      if(focus >= g_list.file_count)
      {
          return ERR_FAILURE;
      }

      /* NAME */
      uni_strncpy(uni_str, g_list.p_file[focus].p_name, max_length - 1);
    }

  uni_strncpy(p_str , uni_str, max_length-1);

  return SUCCESS;
}

static u16 preopen_upgrade_by_usb(u32 para1, u32 para2)
{
  u16 FileCount = 0;
    //initialize global variables.
  comm_dlg_data_t dlg_data = {0};	 
  #if ENABLE_TEST_SPEED
   dlg_data.parent_root = 0;
  #else 
  dlg_data.parent_root = ROOT_ID_MAINMENU;
  #endif
  dlg_data.style = DLG_FOR_SHOW | DLG_STR_MODE_STATIC;
  dlg_data.x = ((SCREEN_WIDTH - 400) / 2);
  dlg_data.y = ((SCREEN_HEIGHT - 200) / 2);
  dlg_data.w = 400;
  dlg_data.h = 160;
  dlg_data.content = IDS_WAIT_PLEASE;
  dlg_data.dlg_tmout = 0;
  ui_comm_dlg_open(&dlg_data); 
  ui_evt_disable_ir();
  p_partition = NULL;
  g_partition_cnt = 0;
  g_partition_cnt = file_list_get_partition(&p_partition);
  if(g_partition_cnt > 0)
  {
    flist_dir = file_list_enter_dir(g_ffilter_all, MAX_FILE_COUNT, p_partition[sys_status_get_usb_work_partition()].letter);
    if(flist_dir != NULL)
    {
        //file_list_get(flist_dir, FLIST_UNIT_FIRST, &g_list);
        ui_upg_file_list_get(flist_dir, FLIST_UNIT_FIRST, &g_list, (u8)para1);
        FileCount = (u16)g_list.file_count;
    }
    else
    {
       OS_PRINTF("\n preopen_upgrade_by_usb: enter dir failure \n");
    }
  }
  else
  {
    memset(&g_list, 0, sizeof(file_list_t));
  }
  ui_evt_enable_ir();
  ui_comm_dlg_close();  

  return FileCount;
}

RET_CODE open_upgrade_by_usb(u32 para1, u32 para2)
{
  control_t *p_cont,*p_ctrl[IDC_ITEM_MAX];//*p_version;
  u8 i, j;
  u16 file_conut = 0;
  u16 stxt [IDC_ITEM_MAX] = {0,IDS_UPGRADE_MODE, IDS_UPGRADE_FILE, IDS_START };
  u16 smode[UPGRADE_BY_USB_MODE_CNT] =
    {
      IDS_UPGRADE_ALL_CODE, IDS_USER_DB,IDS_MAINCODE
    };
  u16 y;
  if(ui_recorder_isrecording())
  {
    return SUCCESS;
  }
  str_asc2uni("|bin|BIN|nodir", g_ffilter_all);
  
  file_conut = preopen_upgrade_by_usb(UPG_ALL_CODE, 0);

  p_cont =
    ui_comm_root_create(ROOT_ID_UPGRADE_BY_USB, 0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
    COMM_BG_H, 0, para2 ? IDS_UPGRADE_BY_NET: IDS_UPGRADE_BY_USB);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, upgrade_by_usb_cont_keymap);
  ctrl_set_proc(p_cont, upgrade_by_usb_cont_proc);

  y = UPGRADE_BY_USB_ITEM_Y;
  for (i = 1; i < IDC_ITEM_MAX; i++)
  {
    p_ctrl[i] = NULL;
    switch (i)
    {
      case IDC_MODE://mode
        p_ctrl[i] = ui_comm_select_create(p_cont, i, UPGRADE_BY_USB_ITEM_X, y,
                          UPGRADE_BY_USB_ITEM_LW, UPGRADE_BY_USB_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
                           UPGRADE_BY_USB_MODE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < UPGRADE_BY_USB_MODE_CNT; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, smode[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], 0);
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_usb_mode_proc);
        break;
      case IDC_FILE://file
        p_ctrl[i] = ui_comm_select_create(p_cont, i, UPGRADE_BY_USB_ITEM_X, y,
                      UPGRADE_BY_USB_ITEM_LW, UPGRADE_BY_USB_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_DYNAMIC,
                    file_conut, CBOX_ITEM_STRTYPE_UNICODE, upgrade_file_update);
        ui_comm_select_set_focus(p_ctrl[i], 0);
        if(0 == file_conut)
        {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        }
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_usb_file_proc);
        break;

      case IDC_START://start
        p_ctrl[i] = ui_comm_static_create(p_cont, i, UPGRADE_BY_USB_ITEM_X, y,
                          UPGRADE_BY_USB_ITEM_LW, UPGRADE_BY_USB_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_usb_start_text_proc);
        if(0 == file_conut)
			ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        y += UPGRADE_BY_USB_ITEM_H + UPGRADE_BY_USB_ITEM_V_GAP;
        break;
      case IDC_PBAR://pbar
        p_ctrl[i] = ui_comm_bar_create(p_cont, i, UPGRADE_BY_USB_PBAR_X,
                         (y - 15),// (y + (UPGRADE_BY_USB_TXT_H - UPGRADE_BY_USB_PBAR_H)/2),
                          UPGRADE_BY_USB_PBAR_W, UPGRADE_BY_USB_PBAR_H,
                          UPGRADE_BY_USB_TXT_X, (y - 18), UPGRADE_BY_USB_TXT_W, UPGRADE_BY_USB_TXT_H,
                          UPGRADE_BY_USB_PER_X, (y - 18), UPGRADE_BY_USB_PER_W, UPGRADE_BY_USB_PER_H);
        ui_comm_bar_set_param(p_ctrl[i], RSC_INVALID_ID, 0, 100, 100);
        ui_comm_bar_set_style(p_ctrl[i],
                                RSI_UPGRADE_BY_USB_PBAR_BG, RSI_UPGRADE_BY_USB_PBAR_MID,
                                RSI_IGNORE, FSI_WHITE,
                                RSI_PBACK, FSI_WHITE);
        ui_comm_bar_update(p_ctrl[i], 0, TRUE);
        break;
      case IDC_STS://upgrade_by_usb status.
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT,IDC_STS, UPGRADE_BY_USB_ITEM_X, (y - 20),
                          (UPGRADE_BY_USB_ITEM_LW + UPGRADE_BY_USB_ITEM_RW),
                          UPGRADE_BY_USB_ITEM_H,
                          p_cont, 0);

	ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH);
	text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
	text_set_font_style(p_ctrl[i], FSI_COMM_CTRL_SH, FSI_COMM_CTRL_HL, FSI_COMM_CTRL_GRAY);
      text_set_content_type(p_ctrl[i], TEXT_STRTYPE_UNICODE);
      text_set_content_by_ascstr(p_ctrl[i], " ");
        break;
#if 0
	case IDC_VERSION :
		//version cont
		p_ctrl[i] =  ctrl_create_ctrl(CTRL_CONT,IDC_VERSION,
								COMM_ITEM_OX_IN_ROOT,y,
								COMM_ITEM_MAX_WIDTH,80,
								p_cont, 0);
		ctrl_set_rstyle(p_ctrl[i], RSI_PBACK, RSI_PBACK, RSI_PBACK);

		p_version = ctrl_create_ctrl(CTRL_LIST, 1,
								0, 0,
								COMM_ITEM_MAX_WIDTH, 80,
								p_ctrl[i], 0);
		ctrl_set_rstyle(p_version, RSI_PBACK, RSI_PBACK, RSI_PBACK);
		list_set_mid_rect(p_version, 0, 0, 800, 80, 0);
		list_set_item_rstyle(p_version, &version_list_item_rstyle);
		list_enable_select_mode(p_version, FALSE);
  		list_set_focus_pos(p_version, 0);
  		list_set_count(p_version, VERSION_LIST_CNT, VERSION_LIST_CNT);
  		list_set_field_count(p_version, VERSION_LIST_FIELD, VERSION_LIST_CNT);
  		list_set_update(p_version, version_list_update, 0);

		for (j = 0; j < VERSION_LIST_FIELD; j++)
		  {
		    list_set_field_attr(p_version, (u8)j, (u32)(version_list_attr[j].attr), (u16)(version_list_attr[j].width),
		                        (u16)(version_list_attr[j].left), (u8)(version_list_attr[j].top));
		    list_set_field_rect_style(p_version, (u8)j, version_list_attr[j].rstyle);
		    list_set_field_font_style(p_version, (u8)j, version_list_attr[j].fstyle);
		  }
		version_list_update(p_version, list_get_valid_pos(p_version), VERSION_LIST_CNT, 0);
	  break;
#endif
      default:
        break;
    }

    y += UPGRADE_BY_USB_ITEM_H + UPGRADE_BY_USB_ITEM_V_GAP;
  }

  ctrl_set_related_id(p_ctrl[IDC_MODE], 0, IDC_START, 0, IDC_FILE);
  ctrl_set_related_id(p_ctrl[IDC_FILE], 0, IDC_MODE, 0, IDC_START);
  ctrl_set_related_id(p_ctrl[IDC_START], 0, IDC_FILE, 0, IDC_MODE);

  ctrl_default_proc(p_ctrl[IDC_MODE], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
  ui_init_upg(APP_USB_UPG, ROOT_ID_UPGRADE_BY_USB, ui_usb_upg_evtmap);
  return SUCCESS;
}

static RET_CODE on_upg_by_usb_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(flist_dir != NULL)
	{
	   file_list_leave_dir(flist_dir);
	   flist_dir = NULL;
	}
  
  if(UI_USB_UPG_IDEL != ui_usb_upg_sts)
  {
    //ui_exit_upgrade_by_usb(FALSE);
    return SUCCESS;
  }

  ui_release_upg(APP_USB_UPG, ROOT_ID_UPGRADE_BY_USB);
  return ERR_NOFEATURE;

}
static RET_CODE on_upg_by_usb_power_off(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(flist_dir != NULL)
	{
	   file_list_leave_dir(flist_dir);
	   flist_dir = NULL;
	}
  
  if(UI_USB_UPG_IDEL != ui_usb_upg_sts)
  {
		return SUCCESS;
  }

  ui_release_upg(APP_USB_UPG, ROOT_ID_UPGRADE_BY_USB);

	return ERR_NOFEATURE;
}
static RET_CODE on_upg_by_usb_quit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  ui_usb_upg_sts = UI_USB_UPG_IDEL;
  ui_release_upg(APP_USB_UPG, ROOT_ID_UPGRADE_BY_USB);
  
  if(flist_dir != NULL)
	{
	   file_list_leave_dir(flist_dir);
	   flist_dir = NULL;
	}
  
  if (para1 == TRUE)
  {
    ui_close_all_mennus();
  }
  else
  {
    manage_close_menu(ROOT_ID_UPGRADE_BY_USB, 0, 0);
  }

  return SUCCESS;
}

static RET_CODE on_upg_by_usb_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_sts;
  u16 uni_str[STS_STR_MAX_LEN] = {0};

  ui_usb_upg_sts = UI_USB_UPG_IDEL;
  //ui_set_book_flag(TRUE);

  p_sts = ctrl_get_child_by_id(p_ctrl, IDC_STS);
  str_nasc2uni("upgrade ok!", uni_str, STS_STR_MAX_LEN);
  ui_upgrade_by_usb_sts_show(p_sts, uni_str);

  ui_refresh_items(p_ctrl);
  return SUCCESS;
}
static RET_CODE on_upg_fail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  comm_dlg_data_t p_data = 
  {
    ROOT_ID_INVALID,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W, COMM_DLG_H,
    IDS_UPG_FAIL,
    0,
  };
  control_t *p_bar, *p_start;
  
  ui_usb_upg_sts = UI_USB_UPG_IDEL;
  ui_set_front_panel_by_str_with_upg("ERR", 0);
  ui_comm_dlg_open(&p_data);
  mtos_task_delay_ms(1000);
  ui_comm_dlg_close();
  
  if(!ui_get_usb_status())
  {
    ui_release_upg(APP_USB_UPG, ROOT_ID_UPGRADE_BY_USB);
    manage_close_menu(ROOT_ID_UPGRADE_BY_USB, 0, 0); 
    ui_set_front_panel_by_str("----");
    return SUCCESS;
  }
  p_bar = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_PBAR);
  p_start = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_START);

  ui_comm_bar_update(p_bar, 0, TRUE);
  ctrl_process_msg(p_start, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_bar, TRUE);
  ctrl_paint_ctrl(p_start, TRUE);
  return SUCCESS;
}
static RET_CODE on_upg_by_usb_update_sts(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  upg_status_t *sts = (upg_status_t *)para1;
  control_t *p_pbar, *p_ctrl_sts;
  comm_dlg_data_t p_data =
  	{
  	    ROOT_ID_INVALID,
 	    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
 	    COMM_DLG_X, COMM_DLG_Y,
 	    COMM_DLG_W+ 40,COMM_DLG_H,
 	    IDS_USB_UPG_RESTART,
 	    0	,
  	};

  p_pbar = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_PBAR);
  p_ctrl_sts = ui_comm_root_get_ctrl(ROOT_ID_UPGRADE_BY_USB, IDC_STS);


  if(UI_USB_UPG_IDEL != ui_usb_upg_sts)
  {
    switch(sts->sm)
    {
      case USB_SM_REMOTE_ERROR:
        break;
      case USB_UPG_SM_BURNING:
        ui_comm_bar_update(p_pbar, (u16)sts->progress, TRUE);
        ui_set_front_panel_by_str_with_upg("B",(u8)sts->progress);
        ui_comm_bar_paint(p_pbar, TRUE);
        if(100 == sts->progress)
        {
            ui_comm_dlg_open(&p_data);
            mtos_task_delay_ms(2000);
            #ifndef WIN32
            hal_pm_reset();
            #endif
        }
        break;
      default:
        return ERR_FAILURE;
    }
  }

  return SUCCESS;
}

static u32 _get_burn_flash_size(u16 focus)
{
  u32 size = 0;
  u32 base_addr = 0;
  u16 i = 0;
  u16 mem_offset = 0;
  u16 block_num = 0;
  u8 block_type = 0;
  u8 * p_read_buf = mtos_malloc(1024); 
  hfile_t file = NULL;
  u16 file_path[MAX_FILE_PATH] ;

  memcpy(file_path, g_list.p_file[focus].name, sizeof(file_path));
  
  file = vfs_open(file_path, VFS_READ);

  if(file == NULL)
  {
    OS_PRINTF("open file failed\n");
    mtos_free(p_read_buf);
    on_upg_fail(NULL, 0, 0, 0);
    return size;
  }
  memset(p_read_buf, 0 , 1024);
#ifndef WIN32
  vfs_seek(file, get_maincode_off_addr() + HEADER_OFFSET, VFS_SEEK_SET);
  vfs_read(p_read_buf, 1024, 1, file);
#endif

  block_num = MAKE_WORD(p_read_buf[8], p_read_buf[9]);

  for(i = 0; i < block_num; i ++)
  {
    mem_offset = HEADER_SIZE + i * sizeof(dmh_block_info_t);
    block_type = p_read_buf[mem_offset + 1];
    
    if(BLOCK_TYPE_IW == block_type)
    {
      base_addr = MT_MAKE_DWORD(MAKE_WORD(p_read_buf[mem_offset + 4], p_read_buf[mem_offset + 5]),\
                                                                  MAKE_WORD(p_read_buf[mem_offset + 6], p_read_buf[mem_offset + 7]));
      break;
    }
  }
#ifndef WIN32
  size = base_addr;// + get_maincode_off_addr();
#endif
  mtos_free(p_read_buf);
  vfs_close(file);
  return size;
}


static void _erase_useless_data(charsto_device_t * p_dev, u32 addr, u32 len)
{
#ifndef WIN32 
  u8 sec_cnt;

  spi_prot_block_type_t prt_t = PRT_UNPROT_ALL;
 
   MT_ASSERT(0 == (len % USB_UPG_BURN_PACKET_SIZE ));
   MT_ASSERT(0 == (addr % USB_UPG_BURN_PACKET_SIZE));
 
  sec_cnt = len / USB_UPG_BURN_PACKET_SIZE;

  prt_t = ap_charsto_get_protect();
  
  ap_charsto_cancel_protect();
  
 charsto_erase(p_dev, addr, sec_cnt);
  
  ap_charsto_set_protect(prt_t);
#endif
  return;
}

static void _pre_burn(u8 mode)
{
  u32 maincode_size = 0;
  u32 total_read_size = 0;
  u8 loop = 0;
  u8 i = 0;
  u32 flash_addr = 0;
  u32 read_addr = 0;
  u32 erase_addr = 0;
  u32 erase_size = 0;
  class_handle_t handle;
  charsto_device_t *p_charsto = NULL;

  if(UPG_MAIN_CODE == mode)
    {
      handle = class_get_handle_by_id(DM_CLASS_ID);
      
    /*maincode size in current flash.bin*/
      maincode_size = dm_get_block_addr(handle, IW_TABLE_BLOCK_ID) - get_flash_addr() - get_maincode_off_addr();
      if(maincode_size != usb_param.burn_flash_size)
      {
        p_charsto = (charsto_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
        total_read_size= dm_get_block_size(handle, IW_TABLE_BLOCK_ID) + \
                                  dm_get_block_size(handle, IW_VIEW_BLOCK_ID);
        loop = total_read_size /USB_UPG_BURN_PACKET_SIZE ;    //iw block size must be an integer multiple of 64K
        flash_addr = dm_get_block_addr(handle, IW_TABLE_BLOCK_ID) - get_flash_addr();
        read_addr = usb_param.ext_buf_addr + usb_param.burn_flash_size;

    for(i= 0; i < loop; i ++)
    {
      charsto_read(p_charsto, flash_addr, (u8 *)read_addr, USB_UPG_BURN_PACKET_SIZE);
      flash_addr += USB_UPG_BURN_PACKET_SIZE;
      read_addr += USB_UPG_BURN_PACKET_SIZE;
    }
    if(maincode_size > usb_param.burn_flash_size)
    {
      erase_size = maincode_size - usb_param.burn_flash_size;
      erase_addr = dm_get_block_addr(handle, IW_VIEW_BLOCK_ID) - get_flash_addr() + \
                           dm_get_block_size(handle, IW_VIEW_BLOCK_ID) - erase_size;
      _erase_useless_data(p_charsto, erase_addr, erase_size);
    }
    usb_param.burn_flash_size += total_read_size;
    usb_param.ext_buf_size += total_read_size;
  }
}
  return;
}

static RET_CODE on_upg_by_usb_start(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_mode, *p_cont, *p_start, *p_file /*,p_list*/;
  u16 mode_focus;
  handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  
  memset(&usb_param, 0x0, sizeof(usb_upg_start_params_t));

  if(UI_USB_UPG_IDEL == ui_usb_upg_sts)
  {
    p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));
    p_mode = ctrl_get_child_by_id(p_cont, IDC_MODE);
    p_file = ctrl_get_child_by_id(p_cont, IDC_FILE);

    mode_focus = ui_comm_select_get_focus(p_mode);

    switch(mode_focus)
    {
    	case UPG_ALL_CODE: // all code
        usb_param.ext_buf_addr = mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
        usb_param.ext_buf_size = GOBY_FLASH_SIZE;
        usb_param.head_size = 0;
        usb_param.burn_flash_size = GOBY_FLASH_SIZE;
        memset(usb_param.head_data, 0 ,sizeof(usb_param.head_data));
        break;
      case UPG_USER_DATA: // user db
        usb_param.ext_buf_addr = mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
        usb_param.head_size = USB_UPG_MAX_HEAD_SIZE;
        memcpy(usb_param.head_data, g_userdb_head_data, usb_param.head_size);
        usb_param.ext_buf_size = dm_get_block_size(dm_handle, IW_TABLE_BLOCK_ID)+\
							 dm_get_block_size(dm_handle, IW_VIEW_BLOCK_ID) + usb_param.head_size;
        usb_param.flash_offset_addr = dm_get_block_addr(dm_handle, IW_TABLE_BLOCK_ID) \
                    						- get_flash_addr(); //NORF_BASE_ADDR
        usb_param.burn_flash_size = usb_param.ext_buf_size - usb_param.head_size;
        break;
      case UPG_MAIN_CODE:
      usb_param.ext_buf_addr = mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
      usb_param.ext_buf_size = _get_burn_flash_size(ui_comm_select_get_focus(p_file));
      usb_param.head_size = 0;
      memset(usb_param.head_data, 0 ,sizeof(usb_param.head_data));
#ifndef WIN32
      usb_param.flash_offset_addr = get_maincode_off_addr();
#endif
      usb_param.burn_flash_size = usb_param.ext_buf_size;
      break;
	default:
        MT_ASSERT(0);
        break;
    }


    mem_mgr_release_block(BLOCK_AV_BUFFER);

    memcpy(usb_param.file, g_list.p_file[ui_comm_select_get_focus(p_file)].name, MAX_FILE_PATH * sizeof(u16));

    usb_param.mode = (u8)ui_comm_select_get_focus(p_mode);

    //ret = ui_upgrade_start_download(&usb_param);

    //if(SUCCESS != ret)   /*success means start update*/
    //{
    //    return ERR_FAILURE;
   // }
    ui_usb_upg_sts = UI_USB_UPG_LOADING;

    ui_comm_ctrl_update_attr(p_mode, FALSE);

    ui_comm_ctrl_update_attr(p_file, FALSE);


    p_start = ctrl_get_child_by_id(p_cont, IDC_START);
    ctrl_process_msg(p_start, MSG_LOSTFOCUS, 0, 0);
    ui_comm_ctrl_update_attr(p_start, FALSE);
    ctrl_paint_ctrl(p_cont, TRUE);
    _ui_upgrade_start_download();
  }

  return SUCCESS;
}

static RET_CODE on_upg_by_usb_file_change_focus(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
	//control_t *p_version;//*p_mode = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_MODE);
	//u16 focus_mode;//, focus_file;

	//focus_mode = ui_comm_select_get_focus(p_mode);

	cbox_class_proc(p_ctrl, msg, 0, 0);

//	if(0 != focus_mode)
	//	return SUCCESS;

	//focus_file = cbox_static_get_focus(p_ctrl);
#if 0
	p_version = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_VERSION);
	version_list_update(ctrl_get_child_by_id(p_version, 1), 1, VERSION_LIST_CNT, 0);
	ctrl_paint_ctrl(p_version, TRUE);
#endif
	return SUCCESS;
}

static RET_CODE on_upg_by_usb_mode_change_focus(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  //control_t *p_version = NULL;
  control_t *p_file = NULL;
  control_t *p_start = NULL;

  u16 focus;

  cbox_class_proc(p_ctrl, msg, 0, 0);

  focus = cbox_static_get_focus(p_ctrl);

  if(g_partition_cnt > 0)
  {
    //update file list
    ui_upg_file_list_get(flist_dir, FLIST_UNIT_FIRST, &g_list, (u8)focus);
    p_file = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_FILE);
    ui_comm_select_set_count(p_file, CBOX_WORKMODE_DYNAMIC, (u16)g_list.file_count);
    ui_comm_select_set_focus(p_file, 0);

    //update start icon
    p_start = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_START);
    if(0 == g_list.file_count)
    {
        ui_comm_ctrl_update_attr(p_file, FALSE);
        ui_comm_ctrl_update_attr(p_start, FALSE);
    }
    else
    {
        ui_comm_ctrl_update_attr(p_file, TRUE);
        ui_comm_ctrl_update_attr(p_start, TRUE);
    }
    ctrl_paint_ctrl(p_file, TRUE);
    ctrl_paint_ctrl(p_start, TRUE);
  }
#if 0
  //update version
  p_version = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_VERSION);
  ctrl_set_attr(ctrl_get_child_by_id(p_version,1), focus? OBJ_ATTR_HIDDEN : OBJ_ATTR_ACTIVE);
  ctrl_paint_ctrl(p_version, TRUE);
#endif

  return SUCCESS;
}

static RET_CODE on_upg_by_usb_destory(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  if(flist_dir != NULL)
	 {
	    file_list_leave_dir(flist_dir);
	    flist_dir = NULL;
	 }
	return ERR_NOFEATURE;
}
BEGIN_KEYMAP(upgrade_by_usb_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
END_KEYMAP(upgrade_by_usb_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(upgrade_by_usb_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_upg_by_usb_exit)
  ON_COMMAND(MSG_EXIT, on_upg_by_usb_exit)
  ON_COMMAND(MSG_POWER_OFF, on_upg_by_usb_power_off)
  ON_COMMAND(MSG_UPG_QUIT, on_upg_by_usb_quit)
  ON_COMMAND(MSG_UPG_OK, on_upg_by_usb_ok)
  ON_COMMAND(MSG_UPG_UPDATE_STATUS, on_upg_by_usb_update_sts)
  ON_COMMAND(MSG_UPG_FAIL, on_upg_fail)
  ON_COMMAND(MSG_DESTROY, on_upg_by_usb_destory)
END_MSGPROC(upgrade_by_usb_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(upgrade_by_usb_mode_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_upg_by_usb_mode_change_focus)
  ON_COMMAND(MSG_DECREASE, on_upg_by_usb_mode_change_focus)
END_MSGPROC(upgrade_by_usb_mode_proc, cbox_class_proc)

BEGIN_MSGPROC(upgrade_by_usb_file_proc, cbox_class_proc)
	ON_COMMAND(MSG_INCREASE, on_upg_by_usb_file_change_focus)
	ON_COMMAND(MSG_DECREASE, on_upg_by_usb_file_change_focus)
END_MSGPROC(upgrade_by_usb_file_proc,cbox_class_proc)


BEGIN_MSGPROC(upgrade_by_usb_start_text_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_upg_by_usb_start)
END_MSGPROC(upgrade_by_usb_start_text_proc, text_class_proc)

BEGIN_AP_EVTMAP(ui_usb_upg_evtmap)
  CONVERT_EVENT(USB_UPG_EVT_UPDATE_STATUS, MSG_UPG_UPDATE_STATUS)
  CONVERT_EVENT(USB_UPG_EVT_QUIT_UPG, MSG_UPG_QUIT)
  CONVERT_EVENT(USB_UPG_EVT_SUCCESS, MSG_UPG_OK)
  CONVERT_EVENT(USB_DOWNLOAD_FILE_FAIL, MSG_UPG_FAIL)
  CONVERT_EVENT(USB_OEPN_UPG_FILE_FAILED, MSG_UPG_FAIL)
  CONVERT_EVENT(USB_MALLOC_MEMORY_FAILED, MSG_UPG_FAIL)
  CONVERT_EVENT(USB_INVLID_UPGRADE_FILE, MSG_UPG_FAIL)
  CONVERT_EVENT(USB_UPG_BURN_FLASH_ERROR, MSG_UPG_FAIL)
END_AP_EVTMAP(ui_usb_upg_evtmap)

