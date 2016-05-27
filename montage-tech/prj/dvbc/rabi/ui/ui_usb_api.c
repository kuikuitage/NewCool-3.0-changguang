/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "vfs.h"

#define DMH_HEAD_SIZE  (1*KBYTES)
//#define BACK_EXTERN_MEM  128

//static  u8 *p_read_buffer = NULL;
static  u8 g_read_data[DMH_HEAD_SIZE] = {0};
u8 g_userdb_head_data[USB_UPG_MAX_HEAD_SIZE] = "*^_^*DM(^o^)";
/*the struct must consistent with flash head*/
typedef  struct
{
  u8 dmh_indicator[12];// = "*^_^*DM(^o^)";
  u32 end_addr;
  u32 sdram_size;
  u32 flash_size;
  u16 block_cnt;
  u16 bisize;// = sizeof(dmh_block_info_t);
}dmh_info_t;


static BOOL file_info_exchange(media_file_t * p_file1, media_file_t *p_file2)
{
    media_file_t tmp_file = {0};
    u16 *p_tmp_addr = NULL;

    if((NULL == p_file1) || (NULL == p_file2))
    {
      return FALSE;
    }
    memcpy(&tmp_file, p_file1, sizeof(media_file_t));
    memcpy(p_file1, p_file2, sizeof(media_file_t));
    memcpy(p_file2, &tmp_file, sizeof(media_file_t));

    p_tmp_addr = p_file1->p_name;
    p_file1->p_name = p_file2->p_name;
    p_file2->p_name = p_tmp_addr;

    return TRUE;
}

static BOOL find_dmh_indicator(hfile_t file)
{
  u32 offset = 0;
  u32 read_size = 0;
  const u32 sec_size = 64*1024;
  u8 dmh_indicator[USB_UPG_DMH_INDC_SIZE] = "*^_^*DM(^o^)";
  vfs_file_info_t file_info = {0};
  
  vfs_get_file_info(file, &file_info);

  memset(g_read_data, 0, DMH_HEAD_SIZE);
  
  while(offset <= (file_info.file_size - sec_size))
  {
    vfs_seek(file, (s64)offset, VFS_SEEK_SET);
    read_size = vfs_read(g_read_data, USB_UPG_DMH_INDC_SIZE, 1, file);
    if(USB_UPG_DMH_INDC_SIZE == read_size)
    { 
        if(memcmp(g_read_data, dmh_indicator, USB_UPG_DMH_INDC_SIZE) == 0)
        {
          OS_PRINTF("get dmh indicator at offset 0x%x\n", offset);
          return TRUE;
        }
    }

    offset += sec_size;
  }

  OS_PRINTF("Cannot find dmh indicator!!!\n");
  return FALSE;
}

static BOOL is_flie_data_valid(hfile_t file, u8 data_type)
{
  u32 read_size = 0;
  BOOL is_valid = FALSE;
  u8 dmh_indicator[12] = "*^_^*DM(^o^)";
  memset(g_read_data, 0, DMH_HEAD_SIZE);
  
  vfs_seek(file, (s64)((UPG_USER_DATA == data_type)? 0 : 0xc), VFS_SEEK_SET);
  read_size = vfs_read(g_read_data, USB_UPG_DMH_INDC_SIZE, 1, file);
  if(USB_UPG_DMH_INDC_SIZE == read_size)
  { 
    if(memcmp(g_read_data, dmh_indicator, USB_UPG_DMH_INDC_SIZE) == 0)
    {
      is_valid = TRUE;
    }
    else
    {
      is_valid = find_dmh_indicator(file);
    }
  }
  
  return is_valid;
}

static BOOL is_flie_valid(u16 *p_path, u8 data_type)
{
    hfile_t file = NULL;
    vfs_file_info_t file_info = {0};
    BOOL is_valid = FALSE;

    file = vfs_open(p_path, VFS_READ);
    if (file == NULL)
    {
      OS_PRINTF("open file failed\n");
      return FALSE;
    }
    vfs_get_file_info(file, &file_info);

    switch(data_type)
    {
        case UPG_ALL_CODE:
        case UPG_MAIN_CODE:
          if(file_info.file_size != CHARSTO_SIZE)
          {
              break;
          }
          is_valid = is_flie_data_valid(file, data_type);
          break;
        case UPG_USER_DATA:
          if(file_info.file_size != (dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), IW_TABLE_BLOCK_ID) + \
                                               dm_get_block_size(class_get_handle_by_id(DM_CLASS_ID), IW_VIEW_BLOCK_ID) + \
                                               USB_UPG_MAX_HEAD_SIZE))
          {
              break;
          }
          is_valid = is_flie_data_valid(file, data_type);
          break;
        default:
          MT_ASSERT(0);
          break;

    }

    vfs_close(file);
    return is_valid;
}

BOOL ui_is_file_exist_in_usb(u16 *p_path)
{
    hfile_t file = NULL;

    if(NULL == p_path)
    {
        return FALSE;
    }

    file = vfs_open(p_path, VFS_READ);
    if (file == NULL)
    {
      return FALSE;
    }
    vfs_close(file);
    
    return TRUE;
}

RET_CODE ui_get_upg_file_version(u16 *p_path, upg_file_ver_t *p_upg_file_ver)
{
    hfile_t file = NULL;
    vfs_file_info_t file_info = {0};
    dmh_block_info_t *p_block_info = NULL;
    dmh_info_t *p_dmh_info = NULL;
    u32 read_size = 0;
    u32 mem_offset = 0;
    u8 dmh_indicator[12] = "*^_^*DM(^o^)";
    BOOL is_find = FALSE;
    u16 i = 0;

    if((NULL == p_path) || (NULL == p_upg_file_ver))
    {
        return ERR_FAILURE;
    }

    file = vfs_open(p_path, VFS_READ);
    if (file == NULL)
    {
      OS_PRINTF("open file failed\n");
      return ERR_FAILURE;
    }
    vfs_get_file_info(file, &file_info);
    if(file_info.file_size != GOBY_FLASH_SIZE)
    {
    	vfs_close(file);
        return ERR_FAILURE;
    }
    
    
    memset(g_read_data, 0, DMH_HEAD_SIZE);

    vfs_seek(file, DM_HDR_START_ADDR, VFS_SEEK_SET);
    read_size = vfs_read(g_read_data, DMH_HEAD_SIZE, 1, file);
    if(DMH_HEAD_SIZE == read_size)
    {
        p_dmh_info = (dmh_info_t *)g_read_data;
        mem_offset += sizeof(dmh_info_t);

        if(0 != memcmp(dmh_indicator,p_dmh_info->dmh_indicator,12))
        {
            vfs_close(file);
            return ERR_FAILURE;
        }

        for(i = 0; i < p_dmh_info->block_cnt; i++)
        {
            p_block_info = (dmh_block_info_t *)(g_read_data + mem_offset);
            if(OTA_BLOCK_ID == p_block_info->id)
            {
                  memcpy(p_upg_file_ver->ota_version, p_block_info->version, 8);
            }
            if(SS_DATA_BLOCK_ID == p_block_info->id)
            {
                is_find = TRUE;
                break;
            }
            mem_offset += sizeof(dmh_block_info_t);
        }
    }
    
    if(is_find)
    {
      vfs_seek(file, DM_HDR_START_ADDR+p_block_info->base_addr, VFS_SEEK_SET);
      memset(g_read_data, 0, DMH_HEAD_SIZE);
      read_size = vfs_read(g_read_data, 20, 1, file);
      if(20 == read_size)
      {
          memcpy(p_upg_file_ver->changeset, g_read_data, 20);
          p_upg_file_ver->changeset[19] = '\0';
      }
      else
      {
          is_find = FALSE;
      }
    }

    vfs_close(file);

    if(is_find)
    {
        return SUCCESS;
    }

    return ERR_FAILURE;
}

BOOL ui_upg_file_list_get(flist_dir_t dir, flist_option_t option, 
                                                file_list_t *p_list, u8 data_mode)
{
  u16 i = 0;
  file_list_t file_list = {0};
  u32 valid_file_cnt = 0;
  BOOL is_valid = TRUE;

  if((NULL == dir) || (NULL == p_list))
  {
    return FALSE;
  }
  file_list_get(dir, option, &file_list);
  OS_PRINTF("[USB]%s:file count[0x%x]\n", __FUNCTION__, file_list.file_count);
  valid_file_cnt = file_list.file_count;

  for(i = 0; i < file_list.file_count; i++)
  {
    if(!is_valid)
    {
        i -= 1;
    }
    if(i >= valid_file_cnt)
    {
        break;
    }

    is_valid = is_flie_valid(file_list.p_file[i].name, data_mode);

    if(!is_valid)
    {
       file_info_exchange(&(file_list.p_file[i]), &(file_list.p_file[valid_file_cnt - 1]));
       valid_file_cnt--;
       continue;
    }
  }

  file_list.file_count = valid_file_cnt;
  memcpy(p_list, &file_list, sizeof(file_list_t));

  OS_PRINTF("[USB]%s:valid_file_cnt[0x%x]\n", __FUNCTION__, p_list->file_count);

  return TRUE;
}


BOOL ui_find_usb_upg_file(void)
{
  u16 FileCount = 0;
  partition_t *p_partition = NULL;
  u32 partition_cnt = 0;
  flist_dir_t flist_dir = NULL;
  u16 ffilter_all[32] = {0};
  file_list_t file_list = {0};
  u16 i = 0;
  upg_file_ver_t upg_file_version = {{0},{0},{0}};

  str_asc2uni("|bin|BIN|nodir", ffilter_all);

  p_partition = NULL;
  partition_cnt = 0;
  partition_cnt = file_list_get_partition(&p_partition);
  if(partition_cnt > 0)
  {
    flist_dir = file_list_enter_dir(ffilter_all, MAX_FILE_COUNT, p_partition[0].letter);
    //MT_ASSERT(flist_dir != NULL); //by sliu
    ui_upg_file_list_get(flist_dir, FLIST_UNIT_FIRST, &file_list, UPG_ALL_CODE);
    FileCount = (u16)file_list.file_count;

    if(flist_dir != NULL)
	  {
	    file_list_leave_dir(flist_dir);
	    flist_dir = NULL;
    }
	   
    if(FileCount > 0)
    {
      for(i=0; i<FileCount; i++)
      {
          ui_get_upg_file_version(file_list.p_file[i].name, &upg_file_version);
          if(0 != memcmp(upg_file_version.changeset, sys_status_get()->changeset, 19))
          {
              return TRUE;
          }
      }
    }
  }

  return FALSE;
}

static usb_partition_t usb_partion = {0};
#define INVALID_NUM 0xFFFF

u8 ui_usb_get_dev_num()
{
   return usb_partion.usb_dev_cnt;
}

u8 ui_usb_up_partition_sev(void)
{
	u8 i = 0;
	u8 j = 0;
	u8 usb_dev_num = 0;
	u8 partition_num = 0;
    u8 usb_last_partition = 0;
	u8 base_par_num = 0;
    partition_t *p_partition = NULL;

	u8 partition_total = file_list_get_partition(&p_partition);


	 usb_last_partition = usb_partion.partition_total;
	 partition_num = partition_total - usb_last_partition;
	 usb_dev_num = ++usb_partion.usb_dev_cnt;

	 for(i = 0;i < usb_dev_num;i++)
	 {
		if(i > 0)
		{
          base_par_num += usb_partion.partion_serv[i - 1].partition_num;
		}
		if(usb_partion.partion_serv[i].partition_num == 0)
	    {
	        memset(usb_partion.partion_serv[i].letter,0,MAX_USB_PARTION);
					
			 for(j = 0;j < partition_num;j++)
			 {
			  	usb_partion.partion_serv[i].letter[j] = p_partition[base_par_num + j].letter[0];
			 }
   	 	    usb_partion.partion_serv[i].partition_num = partition_num;
	    }
	 }
	 
 	 usb_partion.partition_total = partition_total;
 
	 return partition_total;
}


u16 ui_usb_get_cur_used(u16 letter)
{
	u8 i = 0;
	u8 j = 0;

	for(i = 0;i <MAX_USB_SUPPORT;i++)
	{
		for(j = 0;j < usb_partion.partion_serv[i].partition_num;j++)
		{
			if(usb_partion.partion_serv[i].letter[j] == letter)
			{
				return i;
			}			
		}
	}
	return INVALID_NUM;
}
#if 0
 RET_CODE ui_change_plug_out_partition(control_t *p_list, u16 msg,
  u32 para1, u32 para2,RET_CODE (*p_proc)(control_t *p_list, u16 msg,
  u32 para1, u32 para2))
{
  u16 plug_partition = 0;
  u16 cur_usb_index = 0;
  u16 cur_ui_index = 0;
  u16 usb_plug_index = 0;

  usb_plug_index = ui_usb_get_plug_partition_sev();
  cur_usb_index = ui_usb_get_cur_used((u16)para2);
  cur_ui_index = ui_usb_get_cur_used((u16)para1);
	
  if(cur_usb_index != usb_plug_index && ui_usb_get_dev_num() > 1)
  {   
		if(cur_ui_index == usb_plug_index)
		{
		   p_proc(p_list,msg,0,0xFFFF);
		}
		return SUCCESS;
  }
  return ERR_FAILURE;
}
#endif

u16 ui_usb_get_plug_partition_sev(void)
{ 
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 usb_dev_num = 0;
	partition_t *p_partition = NULL;
	u8 cnt = file_list_get_partition(&p_partition);

	usb_dev_num = usb_partion.usb_dev_cnt;

	if(cnt == 0)
	{
		for(i = 0;i <MAX_USB_SUPPORT;i++)
		{
			if(usb_partion.partion_serv[i].partition_num != 0)
			{
				return i;   
			}
		}
	}
	else
	{
		for(i = 0;i < MAX_USB_SUPPORT;i++)
		{
			if(0 == usb_partion.partion_serv[i].partition_num)
				continue;

			for(j = 0;j < usb_partion.partion_serv[i].partition_num;j++)
			{
				for(k = 0;k < cnt;k++)
				{
					if(usb_partion.partion_serv[i].letter[j] == p_partition[k].letter[0])
						break;
				}

				if(k != cnt)
					break;
			}

			if(j == usb_partion.partion_serv[i].partition_num)
			{
				return i;
			}
		}
	}
	return INVALID_NUM;
}

void ui_usb_remove_partition_sev(void)
{
	 u16 usb_dev = ui_usb_get_plug_partition_sev();
	 if(usb_partion.partition_total == 0)
	 {
        return;
	 }
	 usb_partion.partition_total -=  usb_partion.partion_serv[usb_dev].partition_num;
	 usb_partion.partion_serv[usb_dev].partition_num = 0;
	 usb_partion.usb_dev_cnt--;
	 
    return;
}

void ui_release_usb_upgrade(u8 root_id)
{
    cmd_t cmd = {0};

    cmd.id = AP_FRM_CMD_DEACTIVATE_APP;
    cmd.data1 = APP_USB_UPG;
    ap_frm_do_command(APP_FRAMEWORK, &cmd);

    fw_unregister_ap_evtmap(APP_USB_UPG);
    fw_unregister_ap_msghost(APP_USB_UPG, root_id);  
}

