/****************************************************************************

 ****************************************************************************/
#ifndef __UI_USB_API_H__
#define __UI_USB_API_H__

#define GOBY_FLASH_SIZE  (8*KBYTES*KBYTES)
//see flash.cfg  iwtable & iwview block size + head size
//#define GOBY_USER_DB_SIZE  ((640 + 64) * KBYTES)   
#define SKIP_SIZE (64 * KBYTES)
typedef enum
{
    UPG_MAIN_CODE=0,
    UPG_USER_DATA=1,
    UPG_ALL_CODE=2,
}upg_data_type;

typedef struct upg_file_ver
{
    u8 changeset[20];
    u8 boot_version[12]; 
    u8 ota_version[12];  
}upg_file_ver_t;

#define MAX_USB_SUPPORT  2
#define MAX_USB_PARTION  8

typedef struct usb_partition_state	
{
	u16 letter[MAX_USB_PARTION];
	u8 partition_num;
}usb_partition_state_t;

typedef struct usb_partition	
{
    u8 partition_total;
    u8 usb_dev_cnt; 
    usb_partition_state_t  partion_serv[MAX_USB_SUPPORT];  
}usb_partition_t;

u8 ui_usb_up_partition_sev(void);

u16 ui_usb_get_plug_partition_sev(void);

u16 ui_usb_get_cur_used(u16 letter);
void ui_usb_remove_partition_sev(void);
BOOL ui_upg_file_list_get(flist_dir_t dir, flist_option_t option, 
                                                file_list_t *p_list, u8 data_mode);

RET_CODE ui_get_upg_file_version(u16 *p_path, upg_file_ver_t *p_upg_file_ver);

BOOL ui_is_file_exist_in_usb(u16 *p_path);

BOOL ui_find_usb_upg_file(void);

u8 ui_usb_get_dev_num(void);

void ui_release_usb_upgrade(u8 root_id);

BOOL ui_update_partition_sev(void);
#if 0
RET_CODE ui_change_plug_out_partition(control_t *p_list, u16 msg,
  u32 para1, u32 para2,RET_CODE (*p_proc)(control_t *p_list, u16 msg,
  u32 para1, u32 para2));
#endif
#endif


