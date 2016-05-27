


#include "ui_common.h"
#include "ui_network_upgrade.h"
#include "ap_usb_upgrade.h"
#include "ui_net_upg_api.h"

#include "custom_url.h"
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
  IDC_VERSION,
  IDC_ITEM_MAX
}upgrade_by_net_control_id_t;

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
/*
static rect_t g_upgrade_dlg_rc =
{
  UPGRADE_DLG_X,  UPGRADE_DLG_Y,
  UPGRADE_DLG_X + UPGRADE_DLG_W,
  UPGRADE_DLG_Y + UPGRADE_DLG_H,
};*/
//static flist_dir_t flist_dir = NULL;
//static partition_t *p_partition = NULL;
//static u32 g_partition_cnt = 0;
//static file_list_t g_list = {0};
//static u16 g_ffilter_all[32] = {0};
//static usb_upg_start_params_t net_param = {0};

//extern u8 g_userdb_head_data[USB_UPG_DMH_INDC_SIZE];
//extern u8 g_userdb_head_data[12];

static net_upg_api_info_t *p_net_upg_info = NULL;
static net_upg_api_item_t *p_net_upg_info_main = NULL;
static net_upg_api_item_t *p_net_upg_info_custom = NULL;

static u16 upgrade_by_net_cont_keymap(u16 key);
static RET_CODE upgrade_by_net_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_net_start_text_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_net_mode_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE upgrade_by_net_file_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
//static RET_CODE on_upg_by_net_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
static RET_CODE on_upg_fail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
//static void _pre_burn(void);

static u32 ui_net_upg_sts = UI_NET_UPG_IDEL;

u16 ui_net_upg_evtmap(u32 event);


/*!
  Data manager setcion information
  */
typedef struct tag_setcion_mgr_t
{
  /*!
    Data manager setcion base address
    */
  u32 cur_addr;
}setcion_mgr_bl_t;

typedef struct tag_node_info_t
{
  //malloc node_num space to store each node offset in flash
  u32 node_addr :21;
  // used cache size in current node
  u32 use_cache_size:8;
  //00b: is sync, 01b is async, 11b is empty
  u32 cache_state :2;
  //0: active 1: deleted
  u32 deleted :1;
}node_info_bl_t;


/*!
  Data manager block information
  */
typedef struct tag_block_mgr_t
{
  u32 dm_header_addr;
  /*!
    Block id
    */
  u8 id;
  //refer to enum tag_block_type
  u8 type;
  /*!
    node number
    */
  u16 node_num;
  /*!
    Data manager base address
    */
  u32 base_addr;
  /*!
    Data manager block size
    */
  u32 size;
  //below only for type = BLOCK_TYPE_IW
  node_info_bl_t *p_node_info;
  //current used bytes in total block
  u32 total_used;
  //first unused node id
  u16 free_id;
  //form [0:fix_id] is reserved. can't auto dispatch
  u16 fix_id;
  u16 cache_size;
  //current section index
  u16 sec_index;
  //section info
  setcion_mgr_bl_t *p_sec_info;
  u8 *p_cache;
}block_mgr_bl_t;

typedef struct tag_dm_base_info_t
{
  u32 sdram_size;
  u32 flash_size;
  u16 block_num;
  u16 bh_size;
  //above 12B matchs the struct in flash
  u32 flash_base_addr;
  u32 dmh_start_addr[3];
  u16 max_blk_num;
  s32 block_num_off;
  //block_mgr_t block[DM_MAX_BLOCK_NUM];
  block_mgr_bl_t *p_blk_mgr;
}dm_base_info_bl_t;

static u32 find_bh_addr(u8 block_id)
{
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);
  u32 i = 0;
  u32 dm_head = 0;
  u32 init_start = 0;
  dmh_block_info_t dmh = { 0 };
  dm_base_info_bl_t temp_info = {0};
  u32 start_data1 = 0;
  u32 start_data2 = 0;
  u8 k = 0;
  //scan DMH
  for(k = 0; k < 3; k++)  //can support 4 head
  {
    temp_info.block_num = 0;
    for (i = 0; i < 3000; i++)
    {
      charsto_read(p_charsto_dev, init_start, (u8 *)&start_data1, 4);
      charsto_read(p_charsto_dev, init_start + 4, (u8 *)&start_data2, 4);

      if ((start_data1 == 0x5e5f5e2a) && (start_data2 == 0x284d442a))
      {
        //found
          dm_head = init_start;
          charsto_read(p_charsto_dev, dm_head + 16, (u8 *)&temp_info, 12);
          init_start += 64*KBYTES; //for find next head
          break;
      }
      init_start += 64*KBYTES;
    }
    OS_PRINTF("\r\n[OTA] %s:block_num[0x%x] ",__FUNCTION__, temp_info.block_num);

    //scan block header

    for(i = 0;  i < temp_info.block_num; i++)
    {
      u32 bh = dm_head + 16 + 12 + i * sizeof(dmh_block_info_t);
      charsto_read(p_charsto_dev, bh, (u8 *)&dmh, sizeof(dmh_block_info_t));

      if(block_id == dmh.id)
      {
        return bh;
      }
    }
  }
  MT_ASSERT(0);

  return 0;
}
BOOL net_upg_is_burn_finish(void)
{
  dmh_block_info_t dmh = { 0 };
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);
  u32 addr = find_bh_addr(MAINCODE_BLOCK_ID);

  charsto_read(p_charsto_dev, addr, (u8 *)&dmh, sizeof(dmh_block_info_t));

  return (BURN_DEF_FLAG == dmh.reserve || BURN_DONE_FLAG == dmh.reserve
    || 0 == dmh.reserve);
}

static u8 *get_block_head_addr(u8 *p_start, u32 offset,u8 block_id)
{
  dmh_block_info_t dmh = { 0 };
  dm_base_info_bl_t temp_info = {0};
  dm_base_info_bl_t  *p_dm_info = NULL;
  u32 i = 0;

  p_dm_info = &temp_info;

  memcpy(p_dm_info, p_start + offset + 16, 12);

  if(p_dm_info->bh_size != sizeof(dmh_block_info_t))
  {
    MT_ASSERT(0);
  }

  for(i = 0;  i < p_dm_info->block_num; i++)
  {
    memcpy((u8 *)&dmh, p_start + offset + 16 + 12 +
      i * sizeof(dmh_block_info_t), sizeof(dmh_block_info_t));
OS_PRINTF("%s(),%d, block_id = 0x%x\n", __FUNCTION__, __LINE__, dmh.id);
    if(block_id == dmh.id)
    {
      return (u8 *)(p_start + offset + 16 + 12 +
      i * sizeof(dmh_block_info_t));
    }
  }

  return NULL;
}

#if 0
static void hex_dump(u8* addr, u32 len)
{
  u32 i;
  u8 col_cnt = 0;
  for( i = 0; i < len; i++ )
  {
    mtos_printk("0x%02x,", addr[i]);
    col_cnt++;
    if(16 == col_cnt)
    {
      col_cnt =0;
      mtos_printk("\n");
    }
  }
  mtos_printk("\n");
}
#endif

void set_burning_flag(u8 *p_buf, u32 offset)
{
//  int i;
  dmh_block_info_t *p_dmh;
/*
  for(i = 0; i <0x80; i++){
//    OS_PRINTF("@%d +++++++++++++++++++++++++++\n", i);
    if(i!=0 && i!=0x6 && i != 0xa&&i != 0xb&&i!=0x17)
	continue;
    OS_PRINTF("@0x%x +++++++++++++++++++++++++++\n", i*64*KBYTES);
    hex_dump(p_buf+i*64*KBYTES, 256);
  }
*/
  p_dmh = (dmh_block_info_t *)get_block_head_addr(p_buf,
    offset, MAINCODE_BLOCK_ID);

  p_dmh->reserve = BURNING_FLAG;
}


void set_burn_done_flag(void)
{
  u16 flag = 0;
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);
  u32 addr = find_bh_addr(MAINCODE_BLOCK_ID);

  addr += 46;

  charsto_read(p_charsto_dev, addr, (u8 *)&flag, 2);
  OS_PRINTF("flag 0x%x\n", flag);
  flag = BURN_DONE_FLAG;
  OS_PRINTF("addr: 0x%x\n", addr);


  charsto_unprotect_all(p_charsto_dev);
  charsto_writeonly(p_charsto_dev, addr, (u8 *)&flag, 2);
  charsto_protect_all(p_charsto_dev);

  //verify
  OS_PRINTF("addr: 0x%x\n", addr);

  charsto_read(p_charsto_dev, addr, (u8 *)&flag, 2);
  OS_PRINTF("0x%x\n", flag);
  MT_ASSERT(BURN_DONE_FLAG == flag);
}



u32 ui_net_upgade_sts(void)
{
  return ui_net_upg_sts;
}

static void ui_upgrade_by_net_sts_show(control_t *p_cont, u16 str_id)
{
  text_set_content_by_strid(p_cont, str_id);
  ctrl_paint_ctrl(p_cont, TRUE);
}

static void ui_refresh_items(control_t *p_cont)
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
#if 0
static void ui_upgrade_giveup(void)
{
  control_t *p_ctrl_sts;
  control_t *p_root;

  ui_net_upg_sts = UI_NET_UPG_IDEL;
  p_ctrl_sts = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_STS);
  p_root = ctrl_get_parent(p_ctrl_sts);
  ui_refresh_items(p_root);
}

static void ui_upgrade_start_burn(void)
{
  //cmd_t cmd = {0};
  control_t *p_start,*p_bar,*p_sts = NULL;
  u16 uni_str[STS_STR_MAX_LEN] = {0};

  ui_net_upg_sts = UI_NET_UPG_BURNING;
  if(UPG_MAIN_CODE == net_param.mode)
  {
    _pre_burn();
  }
  //cmd.id = NET_UPG_CMD_START_BURN;
  //cmd.data1 = (u32)(&net_param);
  //ap_frm_do_command(APP_NET_UPG, &cmd);


  //burn flash, tbd  
  gui_get_string(IDS_BURN_FLASH, uni_str, STS_STR_MAX_LEN);
  p_sts = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_STS);
  p_start = ctrl_get_child_by_id(ctrl_get_parent(p_sts), IDC_START);
  p_bar = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_PBAR);
  ui_comm_ctrl_update_attr(p_start,FALSE);
  ui_comm_bar_update(p_bar, 0, TRUE);
  ctrl_paint_ctrl(p_start, TRUE);
  ctrl_paint_ctrl(p_bar, TRUE);
  
  ui_upgrade_by_net_sts_show(p_sts, uni_str);
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

  file = vfs_open(net_param.file, VFS_READ);
  if(NULL == file)
  {
    on_upg_fail(NULL, 0, 0, 0);
    return;
  }
  p_pbar = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_PBAR);
  total_size = net_param.ext_buf_size;
  off_set = (net_param.mode == UPG_MAIN_CODE) ? net_param.flash_offset_addr : 0;
  vfs_seek(file, off_set, VFS_SEEK_SET);
  
  while(downloaded_size < total_size)
  {
    p_buf = (u8 *)(net_param.ext_buf_addr + downloaded_size);
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
  ui_comm_ask_for_dodlg_open(&g_upgrade_dlg_rc, IDS_SURE_BURN_FLASH,\
                                                      ui_upgrade_start_burn, ui_upgrade_giveup, 0);
  return ;
}
#endif

static void u_itoa(u32 value, u8 *p_str, u8 radix)
{
#ifndef WIN32
  u32 temp = value;
  u8 loopi = 0;
  u8 radix_str[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  u8 temp_str[32] = {0};

  if(value == 0)
  {
    p_str[0] = '0';
    p_str[1] = '\0';
  }

  while (temp)
  {
    sprintf(temp_str + loopi, "%c", radix_str[temp % radix]);

    loopi ++;
    temp /= radix;
  }
  temp = strlen(temp_str);

  for (loopi = 0; loopi < temp; loopi ++)
  {
    p_str[temp - 1 - loopi] = temp_str[loopi];
  }
#else
  itoa(value, p_str, radix);
#endif
}




static RET_CODE version_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
  u8 local_version[20] = {0};
  u8 new_version[20] = {0};
  u16 i, focus = 0, str_id[VERSION_LIST_CNT -1] = {IDS_MAINCODE};
  control_t *p_ctrl = NULL;
  u16 utf16le_str1[] = {0x672c, 0x5730, 0x7248, 0x672c, 0}; // 本地版本
  u16 utf16le_str2[] = {0x6700, 0x65b0, 0x7248, 0x672c, 0}; // 最新版本

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

  if(p_net_upg_info_main)
  {
    u_itoa(p_net_upg_info_main->sw, new_version, 10);
    new_version[19] = '\0';
  }

  //dm_read(class_get_handle_by_id(DM_CLASS_ID), SS_DATA_BLOCK_ID, 0, 0, MAX_CHANGESET_LEN, local_version);
  u_itoa(sys_status_get_sw_version(), local_version, 10);

  //list_set_field_content_by_ascstr(p_list, 0, 1, "Local Version");
  //list_set_field_content_by_ascstr(p_list, 0, 2, "NEW Version");
  list_set_field_content_by_unistr(p_list, 0, 1, utf16le_str1);
  list_set_field_content_by_unistr(p_list, 0, 2, utf16le_str2);

  list_set_field_content_by_ascstr(p_list, 1, 1, local_version);
  list_set_field_content_by_ascstr(p_list, 1, 2, new_version);

  return SUCCESS;
}

static RET_CODE upgrade_file_update(control_t *p_ctrl, u16 focus,
                                       u16 *p_str, u16 max_length)
{
  control_t* p_cont = ui_comm_root_get_root(ROOT_ID_NETWORK_UPGRADE);
  control_t* p_mode = ctrl_get_child_by_id(p_cont, IDC_MODE);
  u16 mode_focus = ui_comm_select_get_focus(p_mode);
  u16 uni_str[256] = {0};

  if(0 == mode_focus && p_net_upg_info_main)
    str_nasc2uni(p_net_upg_info_main->url, uni_str, max_length - 1);
  else if(1 == mode_focus && p_net_upg_info_custom)
    str_nasc2uni(p_net_upg_info_custom->url, uni_str, max_length - 1);
  else
    str_nasc2uni("No file", uni_str, 16);

  uni_strncpy(p_str, uni_str, max_length-1);

  return SUCCESS;
}


/*
static void close_network_upg_menus(void)
{
  control_t *root = NULL;

  root = fw_find_root_by_id(ROOT_ID_NETWORK_UPGRADE);
  if(root)
  {
    fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
  }
}
*/

static int sw_ver_cmp(const void* p1, const void* p2)
{
  net_upg_api_item_t* p_elem1 = (net_upg_api_item_t*)p1;
  net_upg_api_item_t* p_elem2 = (net_upg_api_item_t*)p2;
  return p_elem2->sw - p_elem1->sw;
}

static RET_CODE on_net_upg_evt_check_done(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t* p_ctrl = ctrl_get_child_by_id(p_cont, IDC_FILE);
  control_t* p_start = ctrl_get_child_by_id(p_cont, IDC_START);
  control_t* p_version = ctrl_get_child_by_id(p_cont, IDC_VERSION);
  u16 cnt = 0, i;
  p_net_upg_info = ui_net_upg_get_upg_info();
  cnt = p_net_upg_info->item_cnt;
  for(i = 0; i < cnt; i++)
  {
    mtos_printk("%s(),%d,%d, sw:%d\n", __FUNCTION__,__LINE__,i, p_net_upg_info->item[i].sw);
    mtos_printk("%s(),%d,%d, name:%s\n",__FUNCTION__,__LINE__, i, p_net_upg_info->item[i].url);
  }

  if(p_net_upg_info&&cnt>0)
  {
    qsort(p_net_upg_info->item, p_net_upg_info->item_cnt, sizeof(net_upg_api_item_t), sw_ver_cmp);
    for(i=0; i<cnt; i++)
      if(0==p_net_upg_info->item[i].type)
      {
        p_net_upg_info_main = p_net_upg_info->item + i;
        break;
      }

    for(i=0; i<cnt; i++)
      if(1==p_net_upg_info->item[i].type)
      {
        p_net_upg_info_custom= p_net_upg_info->item + i;
        break;
      }

    //ui_comm_select_set_count(p_ctrl, CBOX_WORKMODE_DYNAMIC, file_conut);
    ui_comm_select_set_count(p_ctrl, CBOX_WORKMODE_DYNAMIC, 1);
    ui_comm_ctrl_update_attr(p_ctrl, TRUE);
    //ctrl_process_msg(p_ctrl, MSG_INCREASE, 0, 0);
    version_list_update(ctrl_get_child_by_id(p_version, 1), 1, VERSION_LIST_CNT, 0);
    ctrl_paint_ctrl(p_version, TRUE);
    ui_comm_ctrl_update_attr(p_start, TRUE);
    ctrl_paint_ctrl(p_ctrl, TRUE);
  }
  else
  {
    ;//ui_comm_cfmdlg_open_ex(NULL, IDS_NETWORK_BUSY, (do_func_t)close_network_upg_menus, 0);
  }
  return SUCCESS;
}



RET_CODE open_network_upgrade(u32 para1, u32 para2)
{
  control_t *p_cont,*p_ctrl[IDC_ITEM_MAX],*p_version;
  u8 i, j;
  u16 file_conut = 0;
  u16 stxt [IDC_ITEM_MAX] = {0,IDS_UPGRADE_MODE, IDS_UPGRADE_FILE, IDS_START };
  u16 smode[UPGRADE_BY_NET_MODE_CNT] =
    {
      IDS_MAINCODE,
     // IDS_USER_DB,
    };
  u16 y;

  if(1 == para1)
  {
    p_net_upg_info_main = (net_upg_api_item_t*)para2;
    file_conut = 1;
    fw_register_ap_msghost(APP_USB_UPG, ROOT_ID_NETWORK_UPGRADE); 
  }
  else
  {
    ui_net_upg_stop();

OS_PRINTF("%s(), %d\n", __FUNCTION__, __LINE__);
    get_upgrade_url();
OS_PRINTF("%s(), %d\n", __FUNCTION__, __LINE__);

    //ui_net_upg_init(FALSE);
    //ui_net_upg_start_check(HTTP, HTTP_UPG_PATH, "", "");
  }
//  p_net_upg_info = (net_upg_api_info_t *)para1;

//  file_conut = p_net_upg_info->item_cnt;
  p_cont =
    ui_comm_root_create(ROOT_ID_NETWORK_UPGRADE, 0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,
    COMM_BG_H, 0, IDS_NET_UPGRADE);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_cont, RSI_MAIN_BG, RSI_MAIN_BG, RSI_MAIN_BG);
  ctrl_set_keymap(p_cont, upgrade_by_net_cont_keymap);
  ctrl_set_proc(p_cont, upgrade_by_net_cont_proc);

  y = UPGRADE_BY_NET_ITEM_Y;
  for (i = 1; i < IDC_ITEM_MAX; i++)
  {
    p_ctrl[i] = NULL;
    switch (i)
    {
      case IDC_MODE://mode
        p_ctrl[i] = ui_comm_select_create(p_cont, i, UPGRADE_BY_NET_ITEM_X, y,
                          UPGRADE_BY_NET_ITEM_LW, UPGRADE_BY_NET_ITEM_RW);
        ui_comm_select_set_font(p_ctrl[i], FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
                           UPGRADE_BY_NET_MODE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < UPGRADE_BY_NET_MODE_CNT; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, smode[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], 0);
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_net_mode_proc);
        break;
      case IDC_FILE://file
        p_ctrl[i] = ui_comm_select_create(p_cont, i, UPGRADE_BY_NET_ITEM_X, y,
                      UPGRADE_BY_NET_ITEM_LW, UPGRADE_BY_NET_ITEM_RW);
        ui_comm_select_set_font(p_ctrl[i], FSI_WHITE_16, FSI_WHITE_16, FSI_WHITE_16);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_DYNAMIC,
                    file_conut, CBOX_ITEM_STRTYPE_UNICODE, upgrade_file_update);
        ui_comm_select_right_set_mrect(p_ctrl[i], UPGRADE_BY_NET_SELECT_MIDX, 0, UPGRADE_BY_NET_SELECT_MIDY, COMM_CTRL_H);
        ui_comm_select_set_focus(p_ctrl[i], 0);
        if(0 == file_conut)
        {
            ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        }
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_net_file_proc);
        break;

      case IDC_START://start
        p_ctrl[i] = ui_comm_static_create(p_cont, i, UPGRADE_BY_NET_ITEM_X, y,
                          UPGRADE_BY_NET_ITEM_LW, UPGRADE_BY_NET_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], upgrade_by_net_start_text_proc);
        if(0 == file_conut)
        {
          ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        }
        y += UPGRADE_BY_NET_ITEM_H + UPGRADE_BY_NET_ITEM_V_GAP;
        break;
      case IDC_PBAR://pbar
        p_ctrl[i] = ui_comm_bar_create(p_cont, i, UPGRADE_BY_NET_PBAR_X,
                         (y - 15),
                          UPGRADE_BY_NET_PBAR_W, UPGRADE_BY_NET_PBAR_H,
                          UPGRADE_BY_NET_TXT_X, (y - 18), UPGRADE_BY_NET_TXT_W, UPGRADE_BY_NET_TXT_H,
                          UPGRADE_BY_NET_PER_X, (y - 18), UPGRADE_BY_NET_PER_W, UPGRADE_BY_NET_PER_H);
        ui_comm_bar_set_param(p_ctrl[i], RSC_INVALID_ID, 0, 100, 100);
        ui_comm_bar_set_style(p_ctrl[i],
                                RSI_OTT_PROGRESS_BAR_BG, RSI_OTT_PROGRESS_BAR_MID,
                                RSI_IGNORE, FSI_WHITE,
                                RSI_PBACK, FSI_WHITE);
        ui_comm_bar_update(p_ctrl[i], 0, TRUE);
        break;
      case IDC_STS://upgrade_by_net status.
        p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT,IDC_STS, UPGRADE_BY_NET_ITEM_X, (y - 20),
                          (UPGRADE_BY_NET_ITEM_LW + UPGRADE_BY_NET_ITEM_RW),
                          UPGRADE_BY_NET_ITEM_H,
                          p_cont, 0);

    ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH);
    text_set_align_type(p_ctrl[i], STL_CENTER | STL_VCENTER);
    text_set_font_style(p_ctrl[i], FSI_COMM_CTRL_SH, FSI_COMM_CTRL_HL, FSI_COMM_CTRL_GRAY);
    text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
        break;
    case IDC_VERSION :
      p_ctrl[i] =  ctrl_create_ctrl(CTRL_CONT,IDC_VERSION,
                              COMM_ITEM_OX_IN_ROOT,y,
                                COMM_ITEM_MAX_WIDTH,80,
                                p_cont, 0);
      ctrl_set_rstyle(p_ctrl[i], RSI_PBACK, RSI_PBACK, RSI_PBACK);

      p_version = ctrl_create_ctrl(CTRL_LIST, 1,0, 0,
                                                      COMM_ITEM_MAX_WIDTH, 80,
                                                      p_ctrl[i], 0);
      ctrl_set_rstyle(p_version, RSI_PBACK, RSI_PBACK, RSI_PBACK);
      ctrl_set_mrect(p_version, 0, 0, 800, 80);
      list_set_item_interval(p_version, 0);
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
      default:
        break;
    }

    y += UPGRADE_BY_NET_ITEM_H + UPGRADE_BY_NET_ITEM_V_GAP;
  }

  ctrl_set_related_id(p_ctrl[IDC_MODE], 0, IDC_START, 0, IDC_FILE);
  ctrl_set_related_id(p_ctrl[IDC_FILE], 0, IDC_MODE, 0, IDC_START);
  ctrl_set_related_id(p_ctrl[IDC_START], 0, IDC_FILE, 0, IDC_MODE);
  if(1 == para1)
    ctrl_default_proc(p_ctrl[IDC_START], MSG_GETFOCUS, 0, 0);
  else
    ctrl_default_proc(p_ctrl[IDC_MODE], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

  if(1 == para1)
    ctrl_process_msg(p_ctrl[IDC_START], MSG_SELECT, 0, 0);
  return SUCCESS;
}

static RET_CODE on_upg_by_net_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  /*if(flist_dir != NULL)
  {
     file_list_leave_dir(flist_dir);
     flist_dir = NULL;
  }*/
  p_net_upg_info_main = NULL;
  ui_net_upg_stop();
  if(UI_NET_UPG_IDEL != ui_net_upg_sts)
  {
    return SUCCESS;
  }
  
 // ui_release_upg(APP_USB_UPG, ROOT_ID_NETWORK_UPGRADE);
  return ERR_NOFEATURE;

}
static RET_CODE on_upg_by_net_power_off(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  /*if(flist_dir != NULL)
  {
     file_list_leave_dir(flist_dir);
     flist_dir = NULL;
  }*/
  ui_net_upg_stop();
  if(UI_NET_UPG_IDEL != ui_net_upg_sts)
  {
    return SUCCESS;
  }
  
  //ui_release_upg(APP_USB_UPG, ROOT_ID_NETWORK_UPGRADE);

  return ERR_NOFEATURE;
}








extern void ui_evt_enable_ir();

extern void ui_evt_disable_ir();



static RET_CODE on_upg_fail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  comm_dlg_data_t p_data =
  {
    ROOT_ID_INVALID,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W + 40, COMM_DLG_H,
    IDS_UPG_FAIL,
    0,
  };
//  control_t *p_bar, *p_start;

  //-----enable ir--------
  ui_evt_enable_ir();
  //--------------------

  ui_net_upg_sts = UI_NET_UPG_IDEL;
//  ui_set_front_panel_by_str_with_upg("ERR", 0);
  ui_comm_dlg_open_ex(&p_data);
  mtos_task_delay_ms(2000);
  ui_comm_dlg_close();

  manage_close_menu(ROOT_ID_NETWORK_UPGRADE, 0, 0);

  p_net_upg_info = NULL;
  p_net_upg_info_main = NULL;
  p_net_upg_info_custom = NULL;

/*
  p_bar = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_PBAR);
  p_start = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_START);

  ui_comm_bar_update(p_bar, 0, TRUE);
  ctrl_process_msg(p_start, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_bar, TRUE);
  ctrl_paint_ctrl(p_start, TRUE);
  */
  return SUCCESS;
}


/*
static u32 get_burn_flash_size()
{
  u32 size = 0;
  u32 base_addr = 0;
  u16 i = 0;
  u16 mem_offset = 0;
  u16 block_num = 0;
  u8 *p_read_buf = NULL; 
  dmh_block_info_t *p_cur = NULL;
  u8 *p_buf = NULL;
  p_buf = (u8 *)mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
  return 0;
  p_read_buf = p_buf + DM_HDR_START_ADDR + HEADER_OFFSET;
  block_num = MAKE_WORD(p_read_buf[8], p_read_buf[9]);

  for(i = 0; i < block_num; i ++)
  {
    mem_offset = HEADER_SIZE + i * sizeof(dmh_block_info_t);
    p_cur = (dmh_block_info_t *)(p_read_buf + mem_offset);
    OS_PRINTF("\n##[block id:%x, type:%d, node_num:%lu, base_addr:0x%x, crc:%x]\n",
      p_cur->id,
      p_cur->type,
      p_cur->node_num,
      p_cur->base_addr,
      p_cur->crc);
    if(IW_TABLE_BLOCK_ID == p_cur->id)
    {
      base_addr = p_cur->base_addr;
      break;
    }
  }
  size = base_addr;
  OS_PRINTF("\n##check_burn_flash_size [0x%x, 0x%x]\n", size, base_addr);
  mtos_printk("xuhd, %s, size = %d\n", __FUNCTION__, __LINE__);
  return size;
}

*/
static u8* sg_p_backup_buf = NULL;
static u32 sg_backup_size = 0;

static void ui_upgrade_giveup(void)
{
  control_t *p_root= ui_comm_root_get_root(ROOT_ID_NETWORK_UPGRADE);

  if(sg_p_backup_buf)
    mtos_free(sg_p_backup_buf);
  sg_backup_size = 0;
  ui_refresh_items(p_root);
}

static void ui_upgrade_start_burn(void)
{

  control_t *p_sts;
  ui_net_upg_sts = UI_NET_UPG_IDEL;

  //-----disable ir--------
  ui_evt_disable_ir();
  //--------------------
  
  //------read customer url-----------
  sg_p_backup_buf = mtos_malloc(CUSTOM_URL_BLOCK_SIZE);
  if(sg_p_backup_buf)
    sg_backup_size = read_custom_url(sg_p_backup_buf, CUSTOM_URL_BLOCK_SIZE);
  //--------------------------------

  p_sts = ctrl_get_child_by_id(ui_comm_root_get_root(ROOT_ID_NETWORK_UPGRADE), IDC_STS);
  ui_upgrade_by_net_sts_show(p_sts, IDS_BURN_FLASH_CAUTION);

  ctrl_paint_ctrl(p_sts, TRUE);

  set_burning_flag((u8 *)mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG), DM_HDR_START_ADDR);

  ui_net_upg_burn();
}
static RET_CODE on_upg_by_net_update_sts(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  u32 process = para2;
  control_t *p_pbar, *p_ctrl_sts;
  comm_dlg_data_t p_data =
  {
      ROOT_ID_INVALID,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W+ 40,COMM_DLG_H,
    IDS_USB_UPG_RESTART,
    0 ,
  };
OS_PRINTF("\n##on_upg_by_net_update_sts [%lu]\n", process);
  p_pbar = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_PBAR);
  p_ctrl_sts = ui_comm_root_get_ctrl(ROOT_ID_NETWORK_UPGRADE, IDC_STS);
  //burn process
  if (msg == MSG_NET_UPG_EVT_BURN_PROGRESS)
  {
    static BOOL b_enter;

    if(FALSE == b_enter)
    {
      b_enter = TRUE;
      //------save new sw verion-------
      //sys_status_set_sw_version(p_net_upg_info_main->sw);
      //-----------------------------
      ui_net_upg_sts = UI_NET_UPG_BURNING;
    }
    ui_comm_bar_update(p_pbar, (u16)process, TRUE);
    //ui_set_front_panel_by_str_with_upg("B",(u8)process);
    ui_comm_bar_paint(p_pbar, TRUE);
    if(100 == process)
    {
      
      //------back up custom url--------
      if(sg_backup_size > 0)
        save_custom_url(sg_p_backup_buf, sg_backup_size);
      //------------------------------
      set_burn_done_flag();
      ui_comm_dlg_open_ex(&p_data);
      mtos_task_delay_ms(2000);
#ifndef WIN32
        hal_pm_reset();
#endif
    }
  }
  else//download process
  {
    ui_comm_bar_update(p_pbar, (u16)process, TRUE);
//    ui_set_front_panel_by_str_with_upg("D",(u8)process);
    ui_comm_bar_paint(p_pbar, TRUE);
    if(100 == process)
    {
      //fix here
//      ui_net_upg_set_burn_size(get_burn_flash_size());
  ui_comm_ask_for_dodlg_open(NULL, IDS_SURE_BURN_FLASH,
                                                      ui_upgrade_start_burn, ui_upgrade_giveup, 0, ZERO_LEVEL);

    }
  }

  return SUCCESS;
}


static RET_CODE on_upg_by_net_start(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_mode, *p_cont, *p_start, *p_file;
  u16 mode_focus;
  handle_t dm_handle = class_get_handle_by_id(DM_CLASS_ID);
  dmh_block_info_t dm_head;
  net_upg_api_up_cfg_t cfg = {0};
  mtos_printk("%d, enter %s()\n", __LINE__, __FUNCTION__);
  if(UI_NET_UPG_IDEL == ui_net_upg_sts)
  {
    p_cont = ui_comm_root_get_root(ROOT_ID_NETWORK_UPGRADE);
    p_mode = ctrl_get_child_by_id(p_cont, IDC_MODE);
    p_file = ctrl_get_child_by_id(p_cont, IDC_FILE);

    mode_focus = ui_comm_select_get_focus(p_mode);



    switch(mode_focus)
    {
    case 0://main code
    dm_get_block_header(dm_handle, IW_TABLE_BLOCK_ID, &dm_head);
    cfg.flash_addr_offset = DM_HDR_START_ADDR;
    cfg.flash_burn_size = dm_head.base_addr;
    cfg.main_code_offset_addr = DM_HDR_START_ADDR;
    mtos_printk("\n\n##xuhd: base_addr and size [0x%x, 0x%x]\n", dm_head.base_addr, cfg.flash_burn_size);
    break;
    case 1: //custom define
    cfg.flash_addr_offset = DM_HDR_CUSTOM_DEF_ADDR;
    cfg.flash_burn_size = CUSTOM_DEFINE_SIZE;
    mtos_printk("\n\n##xuhd: base_addr and size [0x%x, 0x%x]\n", dm_head.base_addr, cfg.flash_burn_size);
    break;
    default:
      MT_ASSERT(0);
      break;
    }

    ui_comm_ctrl_update_attr(p_mode, FALSE);

    ui_comm_ctrl_update_attr(p_file, FALSE);

    p_start = ctrl_get_child_by_id(p_cont, IDC_START);
    ctrl_process_msg(p_start, MSG_LOSTFOCUS, 0, 0);
    ui_comm_ctrl_update_attr(p_start, FALSE);
    ctrl_paint_ctrl(p_cont, TRUE);
    if(0 == mode_focus)
      cfg.sw = p_net_upg_info_main->sw;
    else if(1 == mode_focus)
      cfg.sw = p_net_upg_info_custom->sw;
    cfg.p_flash_buf = (u8 *)mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
    cfg.flash_buf_size = GOBY_FLASH_SIZE;
    mem_mgr_release_block(BLOCK_AV_BUFFER);
    ui_net_upg_upgrade(&cfg);

  }
  return SUCCESS;
}

static RET_CODE on_upg_by_net_file_change_focus(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_version;

  cbox_class_proc(p_ctrl, msg, 0, 0);
  p_version = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_VERSION);
  version_list_update(ctrl_get_child_by_id(p_version, 1), 1, VERSION_LIST_CNT, 0);
  ctrl_paint_ctrl(p_version, TRUE);
  return SUCCESS;
}

static RET_CODE on_upg_by_net_mode_change_focus(control_t *p_ctrl, u16 msg,
                            u32 para1, u32 para2)
{
  control_t *p_version = NULL;
//  control_t *p_file = NULL;
//  control_t *p_start = NULL;
  control_t* p_cont = ui_comm_root_get_root(ROOT_ID_NETWORK_UPGRADE);
  control_t* p_mode = ctrl_get_child_by_id(p_cont, IDC_MODE);
  control_t* p_file = ctrl_get_child_by_id(p_cont, IDC_FILE);
  u16 focus;

  cbox_class_proc(p_ctrl, msg, 0, 0);

  focus = cbox_static_get_focus(p_ctrl);
  
  //update version
  p_version = ctrl_get_child_by_id(p_ctrl->p_parent->p_parent, IDC_VERSION);
  ctrl_set_sts(ctrl_get_child_by_id(p_version,1), focus? OBJ_STS_HIDE : OBJ_STS_SHOW);
  if(0==focus)
    version_list_update(ctrl_get_child_by_id(p_version, 1), 1, VERSION_LIST_CNT, 0);

  ctrl_paint_ctrl(p_version, TRUE);
  ui_comm_ctrl_update_attr(p_mode, TRUE);

  //-----
  ui_comm_select_set_count(p_file, CBOX_WORKMODE_DYNAMIC, 1);
  ui_comm_select_set_focus(p_file, 0);
  ctrl_paint_ctrl(p_file, TRUE);

  return SUCCESS;
}

static RET_CODE on_net_upg_url_info(control_t *p_ctrl, u16 msg,
                           u32 para1, u32 para2)
{
  int len;
  u8* p;
  u8 post_flag;
  u8* p_url = (u8*)para1;
  mtos_printk("upg_menu, %s(), %d, upg url: %s\n", __FUNCTION__, __LINE__, p_url);

  len = strlen(p_url);
  p = p_url + len - 4;
  post_flag = strcmp(".xml", p);
  ui_net_upg_init(FALSE);
  if( post_flag )
  {
    ui_net_upg_start_check(HTTP, post_flag, p_url, "", "", "");
  }
  else
  {
    while(*p != '/')
      p--;
    *p = '\0';
    ui_net_upg_start_check(HTTP, post_flag, p_url, p+1, "", "");
    mtos_printk("p_addr: %s, p_xml: %s\n", p_url, p+1);
  }

  return SUCCESS;
}


BEGIN_KEYMAP(upgrade_by_net_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
END_KEYMAP(upgrade_by_net_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(upgrade_by_net_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_upg_by_net_exit)
  ON_COMMAND(MSG_EXIT, on_upg_by_net_exit)
  ON_COMMAND(MSG_POWER_OFF, on_upg_by_net_power_off)
 // ON_COMMAND(MSG_UPG_QUIT, on_upg_by_net_quit)
//  ON_COMMAND(MSG_UPG_OK, on_upg_by_net_ok)
  //ON_COMMAND(MSG_UPG_UPDATE_STATUS, on_upg_by_net_update_sts)
  ON_COMMAND(MSG_NET_UPG_EVT_DOWN_PROGRESS, on_upg_by_net_update_sts)
  ON_COMMAND(MSG_NET_UPG_EVT_BURN_PROGRESS, on_upg_by_net_update_sts)
  ON_COMMAND(MSG_NET_UPG_EVT_FAIL, on_upg_fail)
  ON_COMMAND(MSG_NET_UPG_EVT_CHECK_DONE, on_net_upg_evt_check_done)
  ON_COMMAND(MSG_UPG_URL_INFO, on_net_upg_url_info)
END_MSGPROC(upgrade_by_net_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(upgrade_by_net_mode_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_upg_by_net_mode_change_focus)
  ON_COMMAND(MSG_DECREASE, on_upg_by_net_mode_change_focus)
END_MSGPROC(upgrade_by_net_mode_proc, cbox_class_proc)

BEGIN_MSGPROC(upgrade_by_net_file_proc, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_upg_by_net_file_change_focus)
  ON_COMMAND(MSG_DECREASE, on_upg_by_net_file_change_focus)
END_MSGPROC(upgrade_by_net_file_proc,cbox_class_proc)

BEGIN_MSGPROC(upgrade_by_net_start_text_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_upg_by_net_start)
END_MSGPROC(upgrade_by_net_start_text_proc, text_class_proc)

