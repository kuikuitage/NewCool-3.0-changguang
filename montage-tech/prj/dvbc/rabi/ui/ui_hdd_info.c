/****************************************************************************

****************************************************************************/
#include "ui_common.h"
//#include "ui_dvr_config.h"
#include "ui_hdd_info.h"
#include "vfs.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_HDD_INFO_TOTAL, 
  IDC_HDD_INFO_FREE,
  IDC_HDD_INFO_USED,
  IDC_HDD_INFO_TIMESHIFT,
  IDC_HDD_INFO_RECFREE,
  IDC_HDD_INFO_FILESYS,
  IDC_HDD_INFO_SWITCH,
};

static u16 timeshift_name[MAX_FILE_PATH] = {0};
static u16 hdd_info_cont_keymap(u16 key);

RET_CODE hdd_info_cont_proc(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);

RET_CODE hdd_info_partition_select_changed(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);


/*
  all use k bytes
*/

u16 *ui_hdd_get_timeshift_file_name(u16 prt_idx)
{
  u16 partition_letter[10] = {0};
  u8 partition_cnt = 0;

  partition_cnt = vfs_get_partitions(partition_letter, 10);

  if(partition_cnt <= 0)
  {
    return NULL;
  }

  timeshift_name[0] = partition_letter[prt_idx];

  //sprintf(timeshift_name, "%c:\\timeshift.dat", letter);

  str_asc2uni(":\\timeshift.dat", &timeshift_name[1]);

  return timeshift_name;
}


u32 hdd_info_time_shift_free(u16 index)
{
  if(sys_status_get_timeshift_switch())
  {
    return file_list_get_file_size(ui_hdd_get_timeshift_file_name(index))/1024;
  }
  
  return 0;
}

static void hdd_info_set_detail(control_t * p_cont, u16 part_idx)
{
  control_t *p_ctrl[HDD_INFO_ITEM_CNT];
  u8 asc_str[32];
  u32 total_size = 0, free_size = 0, used_size = 0;
  u16 partition_letter[10] = {0};
  u8 partition_cnt = 0;
  u8 i;
  vfs_dev_info_t dev_info = {0};

OS_PRINTF("%s(), %d, part_idx = %d\n", __FUNCTION__, __LINE__, part_idx);
  partition_cnt = vfs_get_partitions(partition_letter, 10);
  if( part_idx >= partition_cnt )
    return;

  OS_PRINTF("partition_cnt = %d\n",partition_cnt);
  vfs_get_dev_info(partition_letter[part_idx],&dev_info);
  total_size += dev_info.total_size;
  free_size += dev_info.free_size;
  used_size += dev_info.used_size;
  //OS_PRINTF("free_size = %d\n",dev_info.free_size);
  //OS_PRINTF("dev_info.fs_type = %d\n",dev_info.fs_type);
  //OS_PRINTF("total_size = %d\n",dev_info.total_size);
  //OS_PRINTF("used_size = %d\n",dev_info.used_size);

  for(i = 0; i < HDD_INFO_ITEM_CNT; i++)
  {
    p_ctrl[i] = ctrl_get_child_by_id(p_cont, IDC_HDD_INFO_TOTAL + i);
  }


  //total
  memset(asc_str, 0, sizeof(asc_str));
  ui_conver_file_size_unit(total_size, asc_str);
  ui_comm_static_set_content_by_ascstr(p_ctrl[0], asc_str);

  // free
  memset(asc_str, 0, sizeof(asc_str));
  ui_conver_file_size_unit(free_size, asc_str);
  ui_comm_static_set_content_by_ascstr(p_ctrl[1], asc_str);

  // used
  memset(asc_str, 0, sizeof(asc_str));
  ui_conver_file_size_unit(used_size, asc_str);
  ui_comm_static_set_content_by_ascstr(p_ctrl[2], asc_str);

  //timeshift free and rec free
  {
    ui_conver_file_size_unit(hdd_info_time_shift_free(part_idx), asc_str);
    ui_comm_static_set_content_by_ascstr(p_ctrl[3], asc_str);
    ui_conver_file_size_unit(free_size, asc_str);
    ui_comm_static_set_content_by_ascstr(p_ctrl[4], asc_str);
  }

  if(dev_info.fs_type == VFS_FAT16)
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl[5], "FAT16");
  }
  else if(dev_info.fs_type == VFS_FAT32)
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl[5], "FAT32");
  }
  else if(dev_info.fs_type == VFS_NTFS)
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl[5], "NTFS");
  }
  else
  {
    ui_comm_static_set_content_by_ascstr(p_ctrl[5], "UNKNOW");
  }

  ctrl_paint_ctrl(p_cont, TRUE);
  return;
}


RET_CODE open_hdd_info(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[HDD_INFO_ITEM_CNT];
  u8 i = 0, j = 0;
  u16 y = 0;
  u16 stxt [HDD_INFO_ITEM_CNT] =
  { IDS_TOTAL, IDS_FREE, IDS_USED,
    IDS_TIME_SHIFT, IDS_REC_FREE, IDS_FILE_SYSTEM, IDS_STORAGE_PARTITION};

  u16 partition_letter[10] = {0};
  u8 partition_cnt = 0;
  
  
#if 1//#ifndef SPT_SUPPORT
  p_cont =ui_comm_right_root_create(ROOT_ID_HDD_INFO, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  MT_ASSERT(p_cont != NULL);
  ctrl_set_keymap(p_cont, hdd_info_cont_keymap);
  ctrl_set_proc(p_cont, hdd_info_cont_proc);
  
  y = HDD_INFO_ITEM_Y;
  for (i = 0; i < HDD_INFO_ITEM_CNT; i++)
  {
    u8 cid;
    cid = IDC_HDD_INFO_TOTAL + i;

    if(IDC_HDD_INFO_SWITCH != cid )
    {
      p_ctrl[i] = ui_comm_static_create(p_cont, cid,
      HDD_INFO_ITEM_X, y, HDD_INFO_ITEM_LW, HDD_INFO_ITEM_RW);
	  MT_ASSERT(p_ctrl[i] != NULL);
      ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
      ui_comm_static_set_param(p_ctrl[i], TEXT_STRTYPE_UNICODE);
    }
    else
    {
#ifndef WIN32
      partition_cnt = vfs_get_partitions(partition_letter, 10);
      OS_PRINTF("partition_cnt = %d\n",partition_cnt);
#else
	  partition_cnt = 1;
#endif
      p_ctrl[i] = ui_comm_select_create(p_cont, cid,
      HDD_INFO_ITEM_X, y, HDD_INFO_ITEM_LW, HDD_INFO_ITEM_RW);
	  MT_ASSERT(p_ctrl[i] != NULL);
      ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
      ui_comm_select_set_param(p_ctrl[i], TRUE,
                                 CBOX_WORKMODE_STATIC, (u16)partition_cnt,
                                 CBOX_ITEM_STRTYPE_UNICODE, NULL);
      ui_comm_ctrl_set_proc(p_ctrl[i], hdd_info_partition_select_changed);
      ui_comm_select_set_focus(p_ctrl[i], 0);

      for (j = 0; j < partition_cnt; j++)
      {
        u16 uni_str[2];
	 uni_str[0] = partition_letter[j];
	 uni_str[1] = 0;
        ui_comm_select_set_content_by_unistr(p_ctrl[i], j, uni_str);
      }

    }
    y += HDD_INFO_ITEM_H + HDD_INFO_ITEM_V_GAP;
  }

  //ui_comm_static_set_rstyle(p_ctrl[0], RSI_COMM_CONT_SH, RSI_COMM_CONT_SH, RSI_COMM_CONT_SH);
  //ui_comm_static_set_rstyle(p_ctrl[1], RSI_COMM_CONT_SH, RSI_COMM_CONT_SH, RSI_COMM_CONT_SH);

  //OS_PRINTF("p_dev_info.free_size = %d\n",p_dev_info.free_size);
  //OS_PRINTF("p_dev_info.fs_type = %d\n",p_dev_info.fs_type);
  //OS_PRINTF("p_dev_info.total_size = %d\n",p_dev_info.total_size);
  //OS_PRINTF("p_dev_info.used_size = %d\n",p_dev_info.used_size);

#ifndef WIN32
  hdd_info_set_detail(p_cont, 0);
#endif
#else

#endif
 
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_ctrl[6], MSG_GETFOCUS, 0, 0); 
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

  return SUCCESS;
}

static RET_CODE on_hdd_info_select(control_t * p_cont, 
  u16 msg, u32 para1, u32 para2)
{
  //control_t * p_active;
  //u8 active_id;

  //p_active = ctrl_get_active_ctrl(p_cont);
  //active_id = ctrl_get_ctrl_id(p_active);
  
  return SUCCESS;
}

static RET_CODE on_hdd_info_init_ok(control_t * p_cont, 
  u16 msg, u32 para1, u32 para2)
{
  //control_t *p_name, *p_id;  
  //p_name = ctrl_get_child_by_id(p_cont, IDC_HDD_INFO_SMC);
  //p_id = ctrl_get_child_by_id(p_cont, IDC_HDD_INFO_CNM);

  //ui_comm_static_set_content_by_ascstr(p_name, "dfgv");

  //ui_comm_static_set_content_by_ascstr(p_id, "sdgf");

  //ctrl_paint_ctrl(p_name, TRUE);
  //ctrl_paint_ctrl(p_id, TRUE);

  return SUCCESS;
}

static RET_CODE on_hdd_info_init_failed(control_t * p_cont, 
  u16 msg, u32 para1, u32 para2)
{
  //control_t * p_name, *p_id;

  //p_name = ctrl_get_child_by_id(p_cont, IDC_HDD_INFO_SMC);
  //p_id = ctrl_get_child_by_id(p_cont, IDC_HDD_INFO_CNM);

  //ui_comm_static_set_content_by_ascstr(p_name, " ");
  //ui_comm_static_set_content_by_ascstr(p_id, " ");
  //
  //ctrl_paint_ctrl(p_name, TRUE);
  //ctrl_paint_ctrl(p_id, TRUE);
  return SUCCESS;
}

static RET_CODE on_hdd_info_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  u32 usb_dev_type = para2;
  if(usb_dev_type == HP_WIFI)
  {
    OS_PRINTF("####wifi device plug out#####\n");
    return ERR_FAILURE;
  }
  manage_close_menu(ROOT_ID_HDD_INFO, 0, 0);
  
  return SUCCESS;
}

static RET_CODE on_hdd_info_select_change_focus(control_t *ctrl, u16 msg, 
                                   u32 para1, u32 para2)
{
  u16 focus;
  RET_CODE ret;

  ret = cbox_class_proc(ctrl, msg, para1, para2);

  /* after switch */
  switch (ctrl_get_ctrl_id(ctrl_get_parent(ctrl)))
  {
    case IDC_HDD_INFO_SWITCH:
      focus = cbox_static_get_focus(ctrl);
      hdd_info_set_detail(ctrl_get_parent(ctrl_get_parent(ctrl)), focus);
      break;
      
    default:
      break;
  }

  return ret;
}

BEGIN_KEYMAP(hdd_info_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(hdd_info_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(hdd_info_partition_select_changed, cbox_class_proc)
  ON_COMMAND(MSG_INCREASE, on_hdd_info_select_change_focus)
  ON_COMMAND(MSG_DECREASE, on_hdd_info_select_change_focus)
END_MSGPROC(hdd_info_partition_select_changed, cbox_class_proc)


BEGIN_MSGPROC(hdd_info_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SELECT, on_hdd_info_select)
  ON_COMMAND(MSG_CA_INIT_OK, on_hdd_info_init_ok)
  ON_COMMAND(MSG_CA_INIT_FAILED, on_hdd_info_init_failed)
  ON_COMMAND(MSG_CA_CARD_OUT, on_hdd_info_init_failed)  
  ON_COMMAND(MSG_PLUG_OUT, on_hdd_info_plug_out)  
END_MSGPROC(hdd_info_cont_proc, ui_comm_root_proc)
