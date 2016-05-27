#include "sys_types.h"
#include "hal_misc.h"
#include "mtos_msg.h"
#include "ui_common.h"
#include "nim.h"
#include "pti.h"
#include "dvb_svc.h"
#include "ap_ota.h"
#include "ui_ota_upgrade.h"
#include "ui_ota_api.h"

#include "ui_net_upg_api.h"

enum ota_upgrade_ctrl_id
{
  IDC_OTA_UPGRADE_STATE = 1,
  IDC_OTA_UPGRADE_NOTES,
  IDC_OTA_UPGRADE_PBAR,
  IDC_OTA_UPGRADE_PER,
  IDC_OTA_UPGRADE_TTL,
};

static BOOL is_burning_state = FALSE;
static u8 com_num = 4;

extern void ota_read_otai(ota_info_t *p_otai);

u16 ota_upgrade_keymap(u16 key);
RET_CODE ota_upgrade_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

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
    for (i = 0; i < 1200; i++)
    {
      charsto_read(p_charsto_dev, init_start, (u8 *)&start_data1, 4);
      charsto_read(p_charsto_dev, init_start + 4, (u8 *)&start_data2, 4);

    //  OS_PRINTF("\r\n find_bh_addr i==%d\n ",i);

      if ((start_data1 == 0x5e5f5e2a) && (start_data2 == 0x284d442a))
      {
        //found
          dm_head = init_start;
       //   OS_PRINTF("\r\n find_bh_addr dm_head==%x\n ",dm_head);
          charsto_read(p_charsto_dev, dm_head + 16, (u8 *)&temp_info, 12);
          init_start += 0x400; //for find next head
          break;
      }
      init_start += 0x400;
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

static void set_burning_flag(u8 *p_buf)
{
  dmh_block_info_t *p_dmh = (dmh_block_info_t *)get_block_head_addr(p_buf,
    DM_HDR_START_ADDR, MAINCODE_BLOCK_ID);
  p_dmh->reserve = BURNING_FLAG;
OS_PRINTF("===============\n");
  hex_dump((u8*)p_dmh, sizeof(dmh_block_info_t));
OS_PRINTF("===============\n");
  hex_dump(p_buf + 0xb0000, 200);
}


static void set_burn_done_flag(void)
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


s32 open_ota_upgrade(u32 para1, u32 para2)
{
#if 0
  struct control *p_cont, *p_state, *p_notes, *p_pbar;
  struct control *p_per, *p_title;

  p_cont = fw_create_mainwin(ROOT_ID_OTA_UPGRADE,
    OTA_UPGRADE_CONTX, OTA_UPGRADE_CONTY,
    OTA_UPGRADE_CONTW, OTA_UPGRADE_CONTH,
    0, 0, OBJ_ATTR_ACTIVE, 0);

  MT_ASSERT(p_cont != NULL);

  ctrl_set_rstyle(p_cont, RSI_COMM_TXT_N, RSI_COMM_TXT_N, RSI_COMM_TXT_N);
  ctrl_set_proc(p_cont, ota_upgrade_proc);
  ctrl_set_keymap(p_cont, ota_upgrade_keymap);

  p_state = ctrl_create_ctrl(CTRL_TEXT, IDC_OTA_UPGRADE_STATE,
    OTA_UPGRADE_STATEX, OTA_UPGRADE_STATEY,
    OTA_UPGRADE_STATEW, OTA_UPGRADE_STATEH,
    p_cont, 0);
  ctrl_set_rstyle(p_state, RSI_COMM_TXT_N, RSI_COMM_TXT_N, RSI_COMM_TXT_N);
  text_set_font_style(p_state, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
  text_set_align_type(p_state, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_state, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_state, IDS_OTA_SEARCHING);

  p_notes = ctrl_create_ctrl(CTRL_TEXT, IDC_OTA_UPGRADE_NOTES,
    OTA_UPGRADE_NOTESX, OTA_UPGRADE_NOTESY,
    OTA_UPGRADE_NOTESW, OTA_UPGRADE_NOTESH,
    p_cont, 0);
  ctrl_set_rstyle(p_notes, RSI_COMM_TXT_N, RSI_COMM_TXT_N, RSI_COMM_TXT_N);
  text_set_font_style(p_notes, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
  text_set_align_type(p_notes, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_notes, TEXT_STRTYPE_STRID);

  p_pbar = ctrl_create_ctrl(CTRL_PBAR, IDC_OTA_UPGRADE_PBAR,
    OTA_UPGRADE_PBARX, OTA_UPGRADE_PBARY,
    OTA_UPGRADE_PBARW, OTA_UPGRADE_PBARH,
    p_cont, 0);
  ctrl_set_rstyle(p_pbar, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
  ctrl_set_mrect(p_pbar, 0, 0, OTA_UPGRADE_PBARW, OTA_UPGRADE_PBARH);
  pbar_set_rstyle(p_pbar, RSI_OTA_UPGRADE_PBAR_MIN,
    RSI_OTA_UPGRADE_PBAR_MAX, RSI_OTA_UPGRADE_PBAR_MIN);
  pbar_set_count(p_pbar, OTA_UPGRADE_PBAR_MIN,
    OTA_UPGRADE_PBAR_MAX, OTA_UPGRADE_PBAR_MAX);
  pbar_set_direction(p_pbar, 1);
  pbar_set_workmode(p_pbar, 1, 0);
  pbar_set_current(p_pbar, 0);

  p_per = ctrl_create_ctrl(CTRL_TEXT, IDC_OTA_UPGRADE_PER,
    OTA_UPGRADE_PERX, OTA_UPGRADE_PERY,
    OTA_UPGRADE_PERW, OTA_UPGRADE_PERH,
    p_cont, 0);
  ctrl_set_rstyle(p_per, RSI_COMM_TXT_N, RSI_COMM_TXT_N, RSI_COMM_TXT_N);
  text_set_font_style(p_per, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
  text_set_align_type(p_per, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_per, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_per, "0%");

  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_OTA_UPGRADE_TTL,
    OTA_UPGRADE_TTLX, OTA_UPGRADE_TTLY,
    OTA_UPGRADE_TTLW, OTA_UPGRADE_TTLH,
    p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_COMM_TXT_N, RSI_COMM_TXT_N, RSI_COMM_TXT_N);
  text_set_font_style(p_title, FSI_COMM_TXT_N, FSI_COMM_TXT_HL, FSI_COMM_TXT_G);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title, IDS_OTA_UPGRADE);

  ctrl_paint_ctrl(p_cont, FALSE);
#endif
  return SUCCESS;
}

 void ui_set_front_panel_by_process(u8 process)
{
   char str[5] = {0};
   static u8 i = 0;
   void *p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);
   memset(str,' ',5);
  if(process == 100)
  {
    i = 0;
  }
  if(i % com_num == 0)
  {
    memset(str,(int)'-',1);
    i++;
  }
  else if(i % com_num == 1)
  {
    memset(str+1,(int)'_',1);
    i++;
  }
  else if(i % com_num == 2)
  {
    i++;
    memset(str + 2,(int)'-',1);
  }
  else if(i % com_num == 3)
  {
    memset(str + 3,(int)'_',1);
    i++;
  }

 if(process == 100)
 {
   memcpy(str,"END ",3);
  }

   uio_display(p_dev,str,com_num);
  //#endif
}


extern u8* get_net_upg_save_buf(void);

static RET_CODE on_ota_upgrade_step(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  struct control *p_pbar, *p_per;
  u8 ascstr[8];

  p_pbar = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_PBAR);
  p_per = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_PER);

  pbar_set_current(p_pbar, (u16)para2);
  ctrl_paint_ctrl(p_pbar, TRUE);

  //OS_PRINTF("percent [%d]", para1);
  sprintf(ascstr, "%d%%", (int)para2);
  text_set_content_by_ascstr(p_per, ascstr);

  ui_set_front_panel_by_process((u8)para2);
  ctrl_paint_ctrl(p_per, TRUE);

  if(para2 == 100)
  {
    if(FALSE == is_burning_state)
    {
      set_burning_flag(get_net_upg_save_buf());
      ui_net_upg_burn();
    }
    else
    {
      set_burn_done_flag();
      ctrl_process_msg(p_ctrl, MSG_OTA_FINISH, para1, para2);
    }
  }
  return SUCCESS;
}




static RET_CODE on_ota_upgrade_burning(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
 // struct control *p_notes, *p_states;
  //static BOOL b_first_enter;
  is_burning_state = TRUE;
#if 0
  if( FALSE == b_first_enter )
  {
    b_first_enter = TRUE;
    p_notes = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_NOTES);
    p_states = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_STATE);

    text_set_content_by_strid(p_states, IDS_BURNING_FLASH);
    ctrl_paint_ctrl(p_states, TRUE);

    text_set_content_by_strid(p_notes, IDS_DONT_POWER_OFF);
    ctrl_paint_ctrl(p_notes, TRUE);
  }
  else
  {
    on_ota_upgrade_step(p_ctrl, msg, para1, para2);
  }
#endif
  return SUCCESS;
}

static RET_CODE on_ota_upgrade_finish(
	control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
#if 0
  struct control *p_notes, *p_states;

  p_notes = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_NOTES);
  p_states = ctrl_get_child_by_id(p_ctrl, IDC_OTA_UPGRADE_STATE);

  text_set_content_by_strid(p_states, IDS_OTA_UPGRADE_COMPLETE);
  ctrl_paint_ctrl(p_states, TRUE);

  text_set_content_by_strid(p_notes, IDS_STB_WILL_AUTO_RESET_THEN);
  ctrl_paint_ctrl(p_notes, TRUE);
  mtos_task_sleep(1000);

	#ifndef WIN32
  hal_pm_reset();
	#else
  while(1)
  {
    mtos_task_sleep(1000);
  }
	#endif
#endif
	return SUCCESS;
}

BEGIN_KEYMAP(ota_upgrade_keymap, NULL)
	ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
	ON_EVENT(V_KEY_MENU, MSG_EXIT)
END_KEYMAP(ota_upgrade_keymap, NULL)

BEGIN_MSGPROC(ota_upgrade_proc, cont_class_proc)
  //ON_COMMAND(MSG_EXIT, on_ota_upgarde_exit)
//  ON_COMMAND(MSG_OTA_PROGRESS, on_ota_upgrade_step)
 // ON_COMMAND(MSG_OTA_UNZIP_OK, on_ota_unzip_ok)
  //ON_COMMAND(MSG_OTA_START_DL, on_ota_upgrade_start_download)
//  ON_COMMAND(MSG_OTA_CHECKING, on_ota_upgrade_checking)
//  ON_COMMAND(MSG_OTA_BURNING, on_ota_upgrade_burning)
  ON_COMMAND(MSG_OTA_FINISH, on_ota_upgrade_finish)

  ON_COMMAND(MSG_NET_UPG_EVT_DOWN_PROGRESS, on_ota_upgrade_step)
  ON_COMMAND(MSG_NET_UPG_EVT_BURN_PROGRESS, on_ota_upgrade_burning)
  ON_COMMAND(MSG_NET_UPG_EVT_FAIL, on_ota_upgrade_finish)

END_MSGPROC(ota_upgrade_proc, cont_class_proc);

