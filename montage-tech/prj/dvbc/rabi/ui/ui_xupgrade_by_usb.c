#include "ui_common.h"
#include "ui_xupgrade_by_usb.h"
#include "ap_usb_upgrade.h"
#include "ap_charsto_impl.h"
#include "ui_xsys_set.h"
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
  IDC_XINVALID = 0,
  IDC_XMODE,
  IDC_XFILE,
  IDC_XSTART,
  IDC_XPBAR,
  IDC_XSTS,
  //IDC_XVERSION,
  IDC_XITEM_MAX
}xupgrade_by_usb_control_id_t;
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

static list_field_attr_t version_list_attr[XVERSION_LIST_FIELD] =
{
  { LISTFIELD_TYPE_STRID| STL_LEFT | STL_VCENTER,
    195, 5, 0, &version_list_field_rstyle,  &version_list_field_fstyle},

  { LISTFIELD_TYPE_UNISTR | STL_CENTER| STL_VCENTER,
    300,200, 0, &version_list_field_rstyle,  &version_list_field_fstyle},

  { LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
    300, 500, 0, &version_list_field_rstyle,  &version_list_field_fstyle},
};
#endif
static rect_t g_xupgrade_dlg_rc =
{
  150+XUPGRADE_DLG_X, 41+ XUPGRADE_DLG_Y,
  150+XUPGRADE_DLG_X + XUPGRADE_DLG_W,
  41+XUPGRADE_DLG_Y + XUPGRADE_DLG_H,
};
static flist_dir_t xflist_dir = NULL;
static partition_t *p_xpartition = NULL;
static u32 g_xpartition_cnt = 0;
static file_list_t g_xlist = {0};
static u16 g_xffilter_all[32] = {0};
static usb_upg_start_params_t xusb_param = {0};

extern u8 g_userdb_head_data[USB_UPG_MAX_HEAD_SIZE];
extern u32 get_flash_addr(void);

static u16 xupgrade_by_usb_cont_keymap(u16 key);
static RET_CODE xupgrade_by_usb_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE xupgrade_by_usb_start_text_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE xupgrade_by_usb_mode_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE xupgrade_by_usb_file_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE on_xupg_by_usb_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE on_xupg_fail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static void _xpre_burn(u8 mode);

static u32 ui_xusb_upg_sts = UI_XUSB_UPG_IDEL;

u16 ui_xusb_upg_evtmap(u32 event);

u32 ui_xusb_upgade_sts(void)
{
  return ui_xusb_upg_sts;
}

void ui_xupgrade_by_usb_sts_show(control_t *p_cont, u16 *p_str)
{
  text_set_content_by_unistr(p_cont, p_str);
  ctrl_paint_ctrl(p_cont, TRUE);
}

void ui_xrefresh_items(control_t *p_cont)
{
  control_t *p_start, *p_pbar, *p_mode, *p_file, *p_sts;

  DEBUG(MAIN,INFO,"\n");
  p_start = ctrl_get_child_by_id(p_cont, IDC_XSTART);
  p_sts = ctrl_get_child_by_id(p_cont, IDC_XSTS);
  ui_comm_ctrl_update_attr(p_start, TRUE);
  ctrl_process_msg(p_sts, MSG_LOSTFOCUS, 0, 0);
  ctrl_process_msg(p_start, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_start, TRUE);

  p_mode = ctrl_get_child_by_id(p_cont, IDC_XMODE);
  ui_comm_ctrl_update_attr(p_mode, TRUE);
  ctrl_paint_ctrl(p_mode, TRUE);

  p_file = ctrl_get_child_by_id(p_cont, IDC_XFILE);
  ui_comm_ctrl_update_attr(p_file, TRUE);
  ctrl_paint_ctrl(p_file, TRUE);

  p_pbar = ctrl_get_child_by_id(p_cont, IDC_XPBAR);
  ui_comm_bar_update(p_pbar, 0, TRUE);
  ui_comm_bar_paint(p_pbar, TRUE);

}
static void ui_xupgrade_giveup(void)
{
  control_t *p_ctrl_sts;
  control_t *p_root;

  ui_xusb_upg_sts = UI_XUSB_UPG_IDEL;
  p_ctrl_sts = ui_comm_root_get_ctrl(ROOT_ID_XUPGRADE_BY_USB, IDC_XSTS);
  p_root = ctrl_get_parent(p_ctrl_sts);
  ui_xrefresh_items(p_root);
}

static void ui_xupgrade_start_burn(void)
{
  cmd_t cmd = {0};
  control_t *p_start,*p_bar,*p_sts = NULL;
  u16 uni_str[XSTS_STR_MAX_LEN] = {0};

  ui_xusb_upg_sts = UI_XUSB_UPG_BURNING;
  
#if ENABLE_TEST_SPEED
    stop_testspeed();
#endif
  _xpre_burn(xusb_param.mode);
  
  cmd.id = USB_UPG_CMD_START_BURN;
  cmd.data1 = (u32)(&xusb_param);
  ap_frm_do_command(APP_USB_UPG, &cmd);

  gui_get_string(IDS_BURN_FLASH, uni_str, XSTS_STR_MAX_LEN);
  p_sts = ui_comm_root_get_ctrl(ROOT_ID_XUPGRADE_BY_USB, IDC_XSTS);
  p_start = ctrl_get_child_by_id(ctrl_get_parent(p_sts), IDC_XSTART);
  p_bar = ui_comm_root_get_ctrl(ROOT_ID_XUPGRADE_BY_USB, IDC_XPBAR);
  ui_comm_ctrl_update_attr(p_start,FALSE);
  ui_comm_bar_update(p_bar, 0, TRUE);
  ctrl_paint_ctrl(p_start, TRUE);
  ctrl_paint_ctrl(p_bar, TRUE);
  ui_xupgrade_by_usb_sts_show(p_sts, uni_str);
}

static BOOL _get_dm_block_head_info_from_upg_buf(u8 block_id, u32 dm_head_start, dmh_block_info_t *dest, u32 *block_head_offset)
{
  u32 blk_num = 0;
  u32 block_addr = 0;
  u32 p_upg_buf = xusb_param.ext_buf_addr;
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
  u32 p_upg_buf = xusb_param.ext_buf_addr;
  dmh_block_info_t dest;
  u32 head_offset;

  for (i = 0; i < xusb_param.ext_buf_size; i++)
  {
    if(memcmp((u8*)p_upg_buf, g_userdb_head_data, USB_UPG_DMH_INDC_SIZE) == 0)
    {  
      dm_head_start = p_upg_buf - xusb_param.ext_buf_addr;

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
  u8 *block = NULL;
  u32 blockLen = 0;
  u32 dataLen = 0;
  u32 indentity_block_offset;
  u32 indentity_block_head_offset;
  dmh_block_info_t *p_block_info;

  blockLen = dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID),IDENTITY_BLOCK_ID);
  if(blockLen == 0)
  {
    return;
  }

  block = (u8 *)mtos_malloc(blockLen);
  MT_ASSERT(block != NULL);
  
  if(sys_get_block_data_by_index(IDENTITY_BLOCK_ID,  block , blockLen, &dataLen))
  {
    if(_get_dm_block_offset_from_upg_buf(IDENTITY_BLOCK_ID, &indentity_block_offset, &indentity_block_head_offset))
    {
      p_block_info = (dmh_block_info_t *)(xusb_param.ext_buf_addr + indentity_block_head_offset);
      if(dataLen> p_block_info->size)
      {
        OS_PRINTF("[%s]:line:%d   dataLen =0x%x  block[0x%#x]  p_block_info->size=0x%#x\n",__FUNCTION__,__LINE__,dataLen, strlen((char*)block),p_block_info->size); 
        return;
      }
     OS_PRINTF("[%s]:line:%d   xusb_param.ext_buf_addr =0x%x\n",__FUNCTION__,__LINE__,xusb_param.ext_buf_addr); 
     OS_PRINTF("[%s]:line:%d        indentity_block_offset =0x%x\n",__FUNCTION__,__LINE__,indentity_block_offset); 
     OS_PRINTF("[%s]:line:%d   dataLen =0x%x  block[0x%#x]  p_block_info->size=0x%#x\n",__FUNCTION__,__LINE__,dataLen, strlen((char*)block),p_block_info->size); 
     memcpy((u8 *)(xusb_param.ext_buf_addr + indentity_block_offset), block, dataLen);
     OS_PRINTF("[%s]:line:%d        p_block_info->size =0x%x\n",__FUNCTION__,__LINE__,p_block_info->size); 
     memset((u8 *)(xusb_param.ext_buf_addr + indentity_block_offset + dataLen), 0xFF, p_block_info->size - dataLen);
     }
  }
  mtos_free(block);
}

static void _ui_xupgrade_start_download( void)
{
  u32 total_size = 0;
  u32 size = 0;
  u16 percent = 0;
  s64 off_set = 0;
  u8 *p_buf = NULL;
  hfile_t file = NULL;
  u32 downloaded_size = 0;
  control_t *p_pbar = NULL;

  file = vfs_open(xusb_param.file, VFS_READ);
  if(NULL == file)
  {
    on_xupg_fail(NULL, 0, 0, 0);
    return;
  }
  p_pbar = ui_comm_root_get_ctrl(ROOT_ID_XUPGRADE_BY_USB, IDC_XPBAR);
  total_size = xusb_param.ext_buf_size;
  off_set = (xusb_param.mode == USB_UPGRD_MAINCODE) ? xusb_param.flash_offset_addr : 0;
  vfs_seek(file, off_set, VFS_SEEK_SET);
  
  while(downloaded_size < total_size)
  {
    p_buf = (u8 *)(xusb_param.ext_buf_addr + downloaded_size);
    size = ((downloaded_size + USB_UPG_TRANS_PACKET_SIZE) <= total_size)? USB_UPG_TRANS_PACKET_SIZE\
              : (total_size - downloaded_size);
    if(size != vfs_read(p_buf, size, 1, file))
    {
      on_xupg_fail(NULL, 0, 0, 0);
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
  if(xusb_param.mode == USB_UPGRD_ALL)
  {
    _keep_serial_num();
  }
  ui_comm_ask_for_dodlg_open(&g_xupgrade_dlg_rc, IDS_SURE_BURN_FLASH,\
                                                      ui_xupgrade_start_burn, ui_xupgrade_giveup, 0);
  return ;
}
#if 0
static RET_CODE version_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
  u16 i;
  u16 str_id[XVERSION_LIST_CNT -1] = {IDS_MAINCODE};
  //upg_file_ver_t upg_file_version = {{0},{0},{0}};
  control_t *p_ctrl = NULL;
  u16 focus = 0;

  for(i = 0; i < (XVERSION_LIST_CNT -1); i++)
  {
    list_set_field_content_by_strid(p_list, i +1, 0, str_id[i]);
  }

  if(NULL != p_list->p_parent->p_parent)
  {
    p_ctrl = ctrl_get_child_by_id( p_list->p_parent->p_parent, IDC_XFILE);
    if(NULL != p_ctrl)
    {
      focus = ui_comm_select_get_focus(p_ctrl);
    }
  }
#if 0
  ui_get_upg_file_version(g_xlist.p_file[focus].name, &upg_file_version);
  upg_file_version.changeset[19] = '\0';

  list_set_field_content_by_ascstr(p_list, 0, 1, "Local Version");
  list_set_field_content_by_ascstr(p_list, 0, 2, "Bin Version");

  list_set_field_content_by_ascstr(p_list, 1, 1, sys_status_get()->changeset);
  list_set_field_content_by_ascstr(p_list, 1, 2, upg_file_version.changeset);
#endif
  return SUCCESS;
}
#endif
static RET_CODE xupgrade_file_update(control_t *p_ctrl, u16 focus,
                                       u16 *p_str, u16 max_length)
{
  u16 uni_str[32] = {0};

  if(0 == g_xlist.file_count)
  {
    str_nasc2uni("No file", uni_str, DB_DVBS_MAX_NAME_LENGTH);
  }
  else
  {
      if(focus >= g_xlist.file_count)
      {
          return ERR_FAILURE;
      }

      /* NAME */
      uni_strncpy(uni_str, g_xlist.p_file[focus].p_name, max_length - 1);
    }

  uni_strncpy(p_str , uni_str, max_length-1);

  return SUCCESS;
}

static u16 preopen_xupgrade_by_usb(u32 para1, u32 para2)
{
  u16 FileCount = 0;
    //initialize global variables.
  comm_dlg_data_t dlg_data = {0};	 
  #if ENABLE_TEST_SPEED
   dlg_data.parent_root = 0;
  #else 
  dlg_data.parent_root = ROOT_ID_XSYS_SET;
  #endif
  dlg_data.style = DLG_FOR_SHOW | DLG_STR_MODE_STATIC;
  dlg_data.x = ((300+1280 - 400) / 2);
  dlg_data.y = ((82+720 - 160) / 2);
  dlg_data.w = 400;
  dlg_data.h = 160;
  dlg_data.content = IDS_WAIT_PLEASE;
  dlg_data.dlg_tmout = 0;
  ui_comm_dlg_open(&dlg_data); 
  ui_evt_disable_ir();
  p_xpartition = NULL;
  g_xpartition_cnt = 0;
  g_xpartition_cnt = file_list_get_partition(&p_xpartition);
  if(g_xpartition_cnt > 0)
  {
    xflist_dir = file_list_enter_dir(g_xffilter_all, MAX_FILE_COUNT, p_xpartition[sys_status_get_usb_work_partition()].letter);
    if(xflist_dir != NULL)
    {
        //file_list_get(xflist_dir, FLIST_UNIT_FIRST, &g_xlist);
        ui_upg_file_list_get(xflist_dir, FLIST_UNIT_FIRST, &g_xlist, (u8)para1);
        FileCount = (u16)g_xlist.file_count;
    }
    else
    {
       OS_PRINTF("\n preopen_xupgrade_by_usb: enter dir failure \n");
    }
  }
  else
  {
    memset(&g_xlist, 0, sizeof(file_list_t));
  }
  ui_evt_enable_ir();
  fw_tmr_destroy(ROOT_ID_POPUP, MSG_CANCEL);
  manage_close_menu(ROOT_ID_POPUP, 0, 0);

  return FileCount;
}

RET_CODE open_xupgrade_by_usb(u32 para1, u32 para2)
{
  control_t *p_cont,*p_ctrl[IDC_XITEM_MAX];//*p_version;
  u8 i, j;
  u16 file_conut = 0;
  u16 stxt [IDC_XITEM_MAX] = {0,IDS_UPGRADE_MODE, IDS_UPGRADE_FILE, IDS_START };
  u16 smode[XUPGRADE_BY_USB_MODE_CNT] =
    {
      IDS_MAINCODE,IDS_USER_DB
    };
  u16 y;

  str_asc2uni("|bin|BIN|nodir", g_xffilter_all);
  
  file_conut = preopen_xupgrade_by_usb(USB_UPGRD_MAINCODE, 0);

  p_cont = fw_create_mainwin(ROOT_ID_XUPGRADE_BY_USB,
							   XUPGRADE_BY_USB_X, XUPGRADE_BY_USB_Y,
							   XUPGRADE_BY_USB_W, XUPGRADE_BY_USB_H,
                             ROOT_ID_XSYS_SET, 0, OBJ_ATTR_ACTIVE, 0);
  ctrl_set_rstyle(p_cont,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, xupgrade_by_usb_cont_keymap);
  ctrl_set_proc(p_cont, xupgrade_by_usb_cont_proc);

  y = XUPGRADE_BY_USB_ITEM_Y;
  for (i = 1; i < IDC_XITEM_MAX; i++)
  {
    p_ctrl[i] = NULL;
    switch (i)
    {
      case IDC_XMODE://mode
        p_ctrl[i] = ui_comm_select_create(p_cont, i, XUPGRADE_BY_USB_ITEM_X, y,
                          XUPGRADE_BY_USB_ITEM_LW, XUPGRADE_BY_USB_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
                           XUPGRADE_BY_USB_MODE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < XUPGRADE_BY_USB_MODE_CNT; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, smode[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], 0);
        ui_comm_ctrl_set_proc(p_ctrl[i], xupgrade_by_usb_mode_proc);
        break;
      case IDC_XFILE://file
        p_ctrl[i] = ui_comm_select_create(p_cont, i, XUPGRADE_BY_USB_ITEM_X, y,
                      XUPGRADE_BY_USB_ITEM_LW, XUPGRADE_BY_USB_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_DYNAMIC,
                    file_conut, CBOX_ITEM_STRTYPE_UNICODE, xupgrade_file_update);
        ui_comm_select_set_focus(p_ctrl[i], 0);
        if(0 == file_conut)
        {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        }
        ui_comm_ctrl_set_proc(p_ctrl[i], xupgrade_by_usb_file_proc);
        break;

      case IDC_XSTART://start
        p_ctrl[i] = ui_comm_static_create(p_cont, i, XUPGRADE_BY_USB_ITEM_X, y,
                          XUPGRADE_BY_USB_ITEM_LW, XUPGRADE_BY_USB_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], xupgrade_by_usb_start_text_proc);
        if(0 == file_conut)
			ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        y += XUPGRADE_BY_USB_ITEM_H + XUPGRADE_BY_USB_ITEM_V_GAP;
        break;
      case IDC_XPBAR://pbar
        p_ctrl[i] = ui_comm_bar_create(p_cont, i, XUPGRADE_BY_USB_PBAR_X,
                         (y - 15),// (y + (XUPGRADE_BY_USB_TXT_H - XUPGRADE_BY_USB_PBAR_H)/2),
                          XUPGRADE_BY_USB_PBAR_W, XUPGRADE_BY_USB_PBAR_H,
                          XUPGRADE_BY_USB_TXT_X, (y - 18), XUPGRADE_BY_USB_TXT_W, XUPGRADE_BY_USB_TXT_H,
                          XUPGRADE_BY_USB_PER_X, (y - 18), XUPGRADE_BY_USB_PER_W, XUPGRADE_BY_USB_PER_H);
        ui_comm_bar_set_param(p_ctrl[i], RSC_INVALID_ID, 0, 100, 100);
        ui_comm_bar_set_style(p_ctrl[i],
                                RSI_XUPGRADE_BY_USB_PBAR_BG, RSI_XUPGRADE_BY_USB_PBAR_MID,
                                RSI_IGNORE, FSI_WHITE,
                                RSI_PBACK, FSI_WHITE);
        ui_comm_bar_update(p_ctrl[i], 0, TRUE);
        break;
      case IDC_XSTS://xupgrade_by_usb status.
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT,IDC_XSTS, XUPGRADE_BY_USB_ITEM_X, (y - 20),
                          (XUPGRADE_BY_USB_ITEM_LW + XUPGRADE_BY_USB_ITEM_RW),
                          XUPGRADE_BY_USB_ITEM_H,
                          p_cont, 0);

	ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH);
	text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
	text_set_font_style(p_ctrl[i], FSI_COMM_CTRL_SH, FSI_COMM_CTRL_HL, FSI_COMM_CTRL_GRAY);
      text_set_content_type(p_ctrl[i], TEXT_STRTYPE_UNICODE);
      text_set_content_by_ascstr(p_ctrl[i], " ");
        break;
#if 0
	case IDC_XVERSION :
		//version cont
		p_ctrl[i] =  ctrl_create_ctrl(CTRL_CONT,IDC_XVERSION,
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
  		list_set_count(p_version, XVERSION_LIST_CNT, XVERSION_LIST_CNT);
  		list_set_field_count(p_version, XVERSION_LIST_FIELD, XVERSION_LIST_CNT);
  		list_set_update(p_version, version_list_update, 0);

		for (j = 0; j < XVERSION_LIST_FIELD; j++)
		  {
		    list_set_field_attr(p_version, (u8)j, (u32)(version_list_attr[j].attr), (u16)(version_list_attr[j].width),
		                        (u16)(version_list_attr[j].left), (u8)(version_list_attr[j].top));
		    list_set_field_rect_style(p_version, (u8)j, version_list_attr[j].rstyle);
		    list_set_field_font_style(p_version, (u8)j, version_list_attr[j].fstyle);
		  }
		version_list_update(p_version, list_get_valid_pos(p_version), XVERSION_LIST_CNT, 0);
	  break;
#endif
      default:
        break;
    }

    y += XUPGRADE_BY_USB_ITEM_H + XUPGRADE_BY_USB_ITEM_V_GAP;
  }

  ctrl_set_related_id(p_ctrl[IDC_XMODE], 0, IDC_XSTART, 0, IDC_XFILE);
  ctrl_set_related_id(p_ctrl[IDC_XFILE], 0, IDC_XMODE, 0, IDC_XSTART);
  ctrl_set_related_id(p_ctrl[IDC_XSTART], 0, IDC_XFILE, 0, IDC_XMODE);

  ctrl_default_proc(p_ctrl[IDC_XMODE], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
  ui_init_upg(APP_USB_UPG, ROOT_ID_XUPGRADE_BY_USB, ui_xusb_upg_evtmap);
  return SUCCESS;
}

static RET_CODE on_xupg_by_usb_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(xflist_dir != NULL)
	{
	   file_list_leave_dir(xflist_dir);
	   xflist_dir = NULL;
	}
  
  if(UI_XUSB_UPG_IDEL != ui_xusb_upg_sts)
  {
    //ui_exit_xupgrade_by_usb(FALSE);
    return SUCCESS;
  }

  ui_release_upg(APP_USB_UPG, ROOT_ID_XUPGRADE_BY_USB);
  return ERR_NOFEATURE;

}
static RET_CODE on_xupg_by_usb_power_off(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(xflist_dir != NULL)
	{
	   file_list_leave_dir(xflist_dir);
	   xflist_dir = NULL;
	}
  
  if(UI_XUSB_UPG_IDEL != ui_xusb_upg_sts)
  {
		return SUCCESS;
  }

  ui_release_upg(APP_USB_UPG, ROOT_ID_XUPGRADE_BY_USB);

	return ERR_NOFEATURE;
}
static RET_CODE on_xupg_by_usb_quit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  ui_xusb_upg_sts = UI_XUSB_UPG_IDEL;
  ui_release_upg(APP_USB_UPG, ROOT_ID_XUPGRADE_BY_USB);
  
  if(xflist_dir != NULL)
	{
	   file_list_leave_dir(xflist_dir);
	   xflist_dir = NULL;
	}
  
  if (para1 == TRUE)
  {
    ui_close_all_mennus();
  }
  else
  {
    manage_close_menu(ROOT_ID_XUPGRADE_BY_USB, 0, 0);
  }

  return SUCCESS;
}

static RET_CODE on_xupg_by_usb_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_sts;
  u16 uni_str[XSTS_STR_MAX_LEN] = {0};

  ui_xusb_upg_sts = UI_XUSB_UPG_IDEL;
  //ui_set_book_flag(TRUE);

  p_sts = ctrl_get_child_by_id(p_ctrl, IDC_XSTS);
  str_nasc2uni("upgrade ok!", uni_str, XSTS_STR_MAX_LEN);
  ui_xupgrade_by_usb_sts_show(p_sts, uni_str);

  ui_xrefresh_items(p_ctrl);
  return SUCCESS;
}
static RET_CODE on_xupg_fail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  comm_dlg_data_t p_data = 
  {
    ROOT_ID_INVALID,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    150+COMM_DLG_X,41+ COMM_DLG_Y,
    COMM_DLG_W, COMM_DLG_H,
    IDS_UPG_FAIL,
    0,
  };
  control_t *p_bar, *p_start, *p_sts;
  ui_xusb_upg_sts = UI_XUSB_UPG_IDEL;
  ui_set_front_panel_by_str_with_upg("ERR", 0);
  ui_comm_dlg_open(&p_data);
  mtos_task_delay_ms(1000);
  ui_comm_dlg_close();
  
  if(!ui_get_usb_status())
  {
    ui_release_upg(APP_USB_UPG, ROOT_ID_XUPGRADE_BY_USB);
    manage_close_menu(ROOT_ID_XUPGRADE_BY_USB, 0, 0); 
    ui_set_front_panel_by_str("----");
    return SUCCESS;
  }
  p_bar = ui_comm_root_get_ctrl(ROOT_ID_XUPGRADE_BY_USB, IDC_XPBAR);
  p_start = ui_comm_root_get_ctrl(ROOT_ID_XUPGRADE_BY_USB, IDC_XSTART);
  p_sts = ui_comm_root_get_ctrl(ROOT_ID_XUPGRADE_BY_USB, IDC_XSTS);

  ui_comm_bar_update(p_bar, 0, TRUE);
  ctrl_process_msg(p_sts, MSG_LOSTFOCUS, 0, 0);
  ctrl_process_msg(p_start, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_bar, TRUE);
  ctrl_paint_ctrl(p_start, TRUE);
  return SUCCESS;
}
static RET_CODE on_xupg_by_usb_update_sts(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  upg_status_t *sts = (upg_status_t *)para1;
  control_t *p_pbar, *p_ctrl_sts;
  comm_dlg_data_t p_data =
  	{
  	    ROOT_ID_INVALID,
 	    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
 	    130+COMM_DLG_X, 41+COMM_DLG_Y,
 	    COMM_DLG_W+ 40,COMM_DLG_H,
 	    IDS_USB_UPG_RESTART,
 	    0	,
  	};

  p_pbar = ui_comm_root_get_ctrl(ROOT_ID_XUPGRADE_BY_USB, IDC_XPBAR);
  p_ctrl_sts = ui_comm_root_get_ctrl(ROOT_ID_XUPGRADE_BY_USB, IDC_XSTS);


  if(UI_XUSB_UPG_IDEL != ui_xusb_upg_sts)
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

static u32 _xget_burn_flash_size(u16 focus)
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

  memcpy(file_path, g_xlist.p_file[focus].name, sizeof(file_path));
  
  file = vfs_open(file_path, VFS_READ);

  if(file == NULL)
  {
    OS_PRINTF("open file failed\n");
    mtos_free(p_read_buf);
    on_xupg_fail(NULL, 0, 0, 0);
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


static void _xerase_useless_data(charsto_device_t * p_dev, u32 addr, u32 len)
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

static void _xpre_burn(u8 mode)
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

  
  if(USB_UPGRD_MAINCODE == mode)
    {
    
      handle = class_get_handle_by_id(DM_CLASS_ID);
      
    /*maincode size in current flash.bin*/
      maincode_size = dm_get_block_addr(handle, IW_TABLE_BLOCK_ID) - get_flash_addr() - get_maincode_off_addr();
      if(maincode_size != xusb_param.burn_flash_size)
      {
        p_charsto = (charsto_device_t *)dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_CHARSTO);
        total_read_size= dm_get_block_size(handle, IW_TABLE_BLOCK_ID) + \
                                  dm_get_block_size(handle, IW_VIEW_BLOCK_ID);
        loop = total_read_size /USB_UPG_BURN_PACKET_SIZE ;    //iw block size must be an integer multiple of 64K
        flash_addr = dm_get_block_addr(handle, IW_TABLE_BLOCK_ID) - get_flash_addr();
        read_addr = xusb_param.ext_buf_addr + xusb_param.burn_flash_size;

    for(i= 0; i < loop; i ++)
    {
      charsto_read(p_charsto, flash_addr, (u8 *)read_addr, USB_UPG_BURN_PACKET_SIZE);
      flash_addr += USB_UPG_BURN_PACKET_SIZE;
      read_addr += USB_UPG_BURN_PACKET_SIZE;
    }
    if(maincode_size > xusb_param.burn_flash_size)
    {
      erase_size = maincode_size - xusb_param.burn_flash_size;
      erase_addr = dm_get_block_addr(handle, IW_VIEW_BLOCK_ID) - get_flash_addr() + \
                           dm_get_block_size(handle, IW_VIEW_BLOCK_ID) - erase_size;
      _xerase_useless_data(p_charsto, erase_addr, erase_size);
    }
    xusb_param.burn_flash_size += total_read_size;
    xusb_param.ext_buf_size += total_read_size;
  }
}
  return;
}

static RET_CODE on_xupg_by_usb_start(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_mode, *p_cont, *p_start, *p_file ,*p_sts;
  u16 mode_focus;
  handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  
  memset(&xusb_param, 0x0, sizeof(usb_upg_start_params_t));

  if(UI_XUSB_UPG_IDEL == ui_xusb_upg_sts)
  {
    p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));
    p_mode = ctrl_get_child_by_id(p_cont, IDC_XMODE);
    p_file = ctrl_get_child_by_id(p_cont, IDC_XFILE);
	p_sts = ctrl_get_child_by_id(p_cont, IDC_XSTS);

    mode_focus = ui_comm_select_get_focus(p_mode);

    switch(mode_focus)
    {
    	case UPG_ALL_CODE: // all code
        xusb_param.ext_buf_addr = mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
        xusb_param.ext_buf_size = GOBY_FLASH_SIZE;
        xusb_param.head_size = 0;
        xusb_param.burn_flash_size = GOBY_FLASH_SIZE;
        xusb_param.mode = USB_UPGRD_ALL;
        memset(xusb_param.head_data, 0 ,sizeof(xusb_param.head_data));
        break;
      case UPG_USER_DATA: // user db
        xusb_param.ext_buf_addr = mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
        xusb_param.head_size = USB_UPG_MAX_HEAD_SIZE;
        memcpy(xusb_param.head_data, g_userdb_head_data, xusb_param.head_size);
        xusb_param.ext_buf_size = dm_get_block_size(dm_handle, IW_TABLE_BLOCK_ID)+\
							 dm_get_block_size(dm_handle, IW_VIEW_BLOCK_ID) + xusb_param.head_size;
        xusb_param.flash_offset_addr = dm_get_block_addr(dm_handle, IW_TABLE_BLOCK_ID) \
                    						- get_flash_addr(); //NORF_BASE_ADDR
        xusb_param.burn_flash_size = xusb_param.ext_buf_size - xusb_param.head_size;
        xusb_param.mode = USB_UPGRD_USER_DB;
        break;
      case UPG_MAIN_CODE:
      xusb_param.ext_buf_addr = mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
      xusb_param.ext_buf_size = _xget_burn_flash_size(ui_comm_select_get_focus(p_file));
      xusb_param.head_size = 0;
      memset(xusb_param.head_data, 0 ,sizeof(xusb_param.head_data));
#ifndef WIN32
      xusb_param.flash_offset_addr = get_maincode_off_addr();
#endif
      xusb_param.burn_flash_size = xusb_param.ext_buf_size;
      xusb_param.mode = USB_UPGRD_MAINCODE;
      break;
	default:
        MT_ASSERT(0);
        break;
    }

    mem_mgr_release_block(BLOCK_AV_BUFFER);

    memcpy(xusb_param.file, g_xlist.p_file[ui_comm_select_get_focus(p_file)].name, MAX_FILE_PATH * sizeof(u16));

    //ret = ui_upgrade_start_download(&xusb_param);

    //if(SUCCESS != ret)   /*success means start update*/
    //{
    //    return ERR_FAILURE;
   // }
    ui_xusb_upg_sts = UI_XUSB_UPG_LOADING;

    ui_comm_ctrl_update_attr(p_mode, FALSE);

    ui_comm_ctrl_update_attr(p_file, FALSE);


    p_start = ctrl_get_child_by_id(p_cont, IDC_XSTART);
    ctrl_process_msg(p_start, MSG_LOSTFOCUS, 0, 0);
	ctrl_process_msg(p_sts, MSG_GETFOCUS, 0, 0);
    ui_comm_ctrl_update_attr(p_start, FALSE);
    ctrl_paint_ctrl(p_cont, TRUE);
    _ui_xupgrade_start_download();
  }

  return SUCCESS;
}

static RET_CODE on_xupg_by_usb_file_change_focus(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
	//control_t *p_version;//*p_mode = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_XMODE);
	//u16 focus_mode;//, focus_file;

	//focus_mode = ui_comm_select_get_focus(p_mode);

	cbox_class_proc(p_ctrl, msg, 0, 0);

//	if(0 != focus_mode)
	//	return SUCCESS;

	//focus_file = cbox_static_get_focus(p_ctrl);
#if 0
	p_version = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_XVERSION);
	version_list_update(ctrl_get_child_by_id(p_version, 1), 1, XVERSION_LIST_CNT, 0);
	ctrl_paint_ctrl(p_version, TRUE);
#endif
	return SUCCESS;
}

static RET_CODE on_xupg_by_usb_mode_change_focus(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  //control_t *p_version = NULL;
  control_t *p_file = NULL;
  control_t *p_start = NULL;

  u16 focus;

  cbox_class_proc(p_ctrl, msg, 0, 0);

  focus = cbox_static_get_focus(p_ctrl);

  if(g_xpartition_cnt > 0)
  {
    //update file list
    ui_upg_file_list_get(xflist_dir, FLIST_UNIT_FIRST, &g_xlist, (u8)focus);
    p_file = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_XFILE);
    ui_comm_select_set_count(p_file, CBOX_WORKMODE_DYNAMIC, (u16)g_xlist.file_count);
    ui_comm_select_set_focus(p_file, 0);

    //update start icon
    p_start = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_XSTART);
    if(0 == g_xlist.file_count)
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
  p_version = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_XVERSION);
  ctrl_set_attr(ctrl_get_child_by_id(p_version,1), focus? OBJ_ATTR_HIDDEN : OBJ_ATTR_ACTIVE);
  ctrl_paint_ctrl(p_version, TRUE);
#endif

  return SUCCESS;
}

static RET_CODE on_xupg_by_usb_destory(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  if(xflist_dir != NULL)
	 {
	    file_list_leave_dir(xflist_dir);
	    xflist_dir = NULL;
	 }
	return ERR_NOFEATURE;
}

#define ALL_CODE_UPG_PASSWORD  (0x1703)

static RET_CODE on_all_code_xupg_by_usb(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_mode;
    static u16 password;
    u8 j;
    u8 inputNum = (u8)(msg & MSG_DATA_MASK);
    
    u16 smode[XUPGRADE_BY_USB_MAXMODE_CNT] =
    {
        IDS_MAINCODE,IDS_USER_DB,IDS_UPGRADE_ALL_CODE
    };

    password = (password<<4)|inputNum;
    if(password == ALL_CODE_UPG_PASSWORD)
    {
        p_mode = ctrl_get_child_by_id(p_ctrl, IDC_XMODE);
        ui_comm_select_set_param(p_mode, TRUE, CBOX_WORKMODE_STATIC,3, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < XUPGRADE_BY_USB_MAXMODE_CNT; j++)
        {
            ui_comm_select_set_content(p_mode, j, smode[j]);
        }
        ctrl_paint_ctrl(p_mode, TRUE);
        
    }
    return SUCCESS;
}

BEGIN_KEYMAP(xupgrade_by_usb_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_0, MSG_CHAR | 0)
  ON_EVENT(V_KEY_1, MSG_CHAR | 1)
  ON_EVENT(V_KEY_2, MSG_CHAR | 2)
  ON_EVENT(V_KEY_3, MSG_CHAR | 3)
  ON_EVENT(V_KEY_4, MSG_CHAR | 4)
  ON_EVENT(V_KEY_5, MSG_CHAR | 5)
  ON_EVENT(V_KEY_6, MSG_CHAR | 6)
  ON_EVENT(V_KEY_7, MSG_CHAR | 7)
  ON_EVENT(V_KEY_8, MSG_CHAR | 8)
  ON_EVENT(V_KEY_9, MSG_CHAR | 9)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
END_KEYMAP(xupgrade_by_usb_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(xupgrade_by_usb_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_CHAR, on_all_code_xupg_by_usb)
  ON_COMMAND(MSG_EXIT_ALL, on_xupg_by_usb_exit)
  ON_COMMAND(MSG_EXIT, on_xupg_by_usb_exit)
  ON_COMMAND(MSG_POWER_OFF, on_xupg_by_usb_power_off)
  ON_COMMAND(MSG_UPG_QUIT, on_xupg_by_usb_quit)
  ON_COMMAND(MSG_UPG_OK, on_xupg_by_usb_ok)
  ON_COMMAND(MSG_UPG_UPDATE_STATUS, on_xupg_by_usb_update_sts)
  ON_COMMAND(MSG_UPG_FAIL, on_xupg_fail)
  ON_COMMAND(MSG_DESTROY, on_xupg_by_usb_destory)
END_MSGPROC(xupgrade_by_usb_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(xupgrade_by_usb_mode_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_xupg_by_usb_mode_change_focus)
  ON_COMMAND(MSG_DECREASE, on_xupg_by_usb_mode_change_focus)
END_MSGPROC(xupgrade_by_usb_mode_proc, cbox_class_proc)

BEGIN_MSGPROC(xupgrade_by_usb_file_proc, cbox_class_proc)
	ON_COMMAND(MSG_INCREASE, on_xupg_by_usb_file_change_focus)
	ON_COMMAND(MSG_DECREASE, on_xupg_by_usb_file_change_focus)
END_MSGPROC(xupgrade_by_usb_file_proc,cbox_class_proc)


BEGIN_MSGPROC(xupgrade_by_usb_start_text_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_xupg_by_usb_start)
END_MSGPROC(xupgrade_by_usb_start_text_proc, text_class_proc)

BEGIN_AP_EVTMAP(ui_xusb_upg_evtmap)
  CONVERT_EVENT(USB_UPG_EVT_UPDATE_STATUS, MSG_UPG_UPDATE_STATUS)
  CONVERT_EVENT(USB_UPG_EVT_QUIT_UPG, MSG_UPG_QUIT)
  CONVERT_EVENT(USB_UPG_EVT_SUCCESS, MSG_UPG_OK)
  CONVERT_EVENT(USB_DOWNLOAD_FILE_FAIL, MSG_UPG_FAIL)
  CONVERT_EVENT(USB_OEPN_UPG_FILE_FAILED, MSG_UPG_FAIL)
  CONVERT_EVENT(USB_MALLOC_MEMORY_FAILED, MSG_UPG_FAIL)
  CONVERT_EVENT(USB_INVLID_UPGRADE_FILE, MSG_UPG_FAIL)
  CONVERT_EVENT(USB_UPG_BURN_FLASH_ERROR, MSG_UPG_FAIL)
END_AP_EVTMAP(ui_xusb_upg_evtmap)

