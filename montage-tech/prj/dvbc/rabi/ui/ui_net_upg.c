#include "ui_common.h"

#include "net_extern_api.h"
#include "net_upg_api.h"

#include "ui_net_upg_dec.h"

#include "ui_net_upg.h"
#include "ui_keyboard_v2.h"
#include "ui_ota_api.h"


enum
{
  IDC_INVALID = 0,
  IDC_NET_UPG_MODE,
  IDC_NET_UPG_URL,
  IDC_NET_UPG_FILE,
  IDC_NET_UPG_START,
  IDC_NET_UPG_PBAR,
  IDC_NET_UPG_STS,
};

static u16 net_upg_cont_keymap(u16 key);
static RET_CODE net_upg_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE net_upg_start_text_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE net_upg_mode_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE net_upg_url_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static RET_CODE net_upg_file_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static u16 ui_net_upg_evtmap(u32 event);

ui_net_upg_status_t ui_net_epg_sts = UI_NET_UPG_IDEL;
static u32 sg_offset = 0;

static usb_upg_start_params_t usb_param = {0};

static net_upg_info_t s_net_upg_info;

extern u8 g_userdb_head_data[USB_UPG_MAX_HEAD_SIZE];

void ui_init_net_upg()
{
  ui_net_epg_sts = UI_NET_UPG_IDEL;
  usb_param.ext_buf_addr = mem_mgr_require_block(BLOCK_AV_BUFFER, SYS_MODULE_UPG);
  usb_param.ext_buf_size = GOBY_FLASH_SIZE;
  usb_param.head_size = 0;
  usb_param.burn_flash_size = GOBY_FLASH_SIZE;
  usb_param.mode = USB_UPGRD_ALL;
  memset(usb_param.head_data, 0 ,sizeof(usb_param.head_data));
  mem_mgr_release_block(BLOCK_AV_BUFFER);

#ifndef WIN32
  MT_ASSERT(SUCCESS == mul_net_extern_init());
#endif

  ui_init_upg(APP_USB_UPG, ROOT_ID_NET_UPG, ui_net_upg_evtmap);
}

void ui_release_net_upg(void)
{
  sg_offset = 0;
#ifndef WIN32
  mul_net_extern_stop(NULL);
  mul_net_extern_deinit();
#endif
  ui_release_upg(APP_USB_UPG, ROOT_ID_NET_UPG);
}


void net_upg_dl_cb(u8 *p_url, u8 *p_data, u32 data_size)
{
  u8* sg_p_flash_buf;
  static upg_status_t sts;
  //OS_PRINTF("+++++++++++++++++++++++++++++\n");
  //OS_PRINTF("sg_offset=%d, data_size=%d\n", sg_offset, data_size);
  //OS_PRINTF("+++++++++++++++++++++++++++++\n");

  if( 0 == data_size )
  {
    return;
  }

  sg_p_flash_buf = (u8*)usb_param.ext_buf_addr;
  memcpy(sg_p_flash_buf + sg_offset, p_data, data_size);
  sg_offset += data_size;

  //update ui
  sts.progress = sg_offset*100/CHARSTO_SIZE;
  sts.sm = USB_DUMP_SM_READING_FLASH;
  sts.p_description = NULL;
  fw_notify_root(fw_find_root_by_id(ROOT_ID_NET_UPG), NOTIFY_T_MSG,
                     FALSE, MSG_UPG_UPDATE_STATUS, (u32)&sts, 0);

  if(sg_offset > CHARSTO_SIZE)
    MT_ASSERT(0);
}


static void ui_net_upg_start(u8* p_url)
{
  mul_net_extern_request_t req;
  req.p_url = p_url;
  req.is_once_get_all_data = 0;
  req.cb = net_upg_dl_cb;
#ifndef WIN32
  MT_ASSERT(SUCCESS == mul_net_extern_start(&req));
#endif
}

void net_upg_info_dl_cb(u8 *p_url, u8 *p_data, u32 data_size)
{
  u8 ansstr[64];
  u32 serial_num;

  memset(ansstr, 0, sizeof(ansstr));
  sys_get_serial_num(ansstr, sizeof(ansstr));
  serial_num = atoi(ansstr);
  memset(&s_net_upg_info, 0, sizeof(s_net_upg_info));
#ifndef WIN32
  ui_net_upg_info_parse(p_data, (u32)CUSTOMER_ID, serial_num, ui_ota_api_get_upg_check_version(), &s_net_upg_info);
#endif
  fw_notify_root(fw_find_root_by_id(ROOT_ID_NET_UPG), NOTIFY_T_MSG,
                     FALSE, MSG_NET_UPG_EVT_INFO_ARRIVED, 0, 0);
}

static void ui_net_upg_download_upg_info(u8* p_url)
{
  mul_net_extern_request_t req;
  req.p_url = p_url;
  req.is_once_get_all_data = 1;
  req.cb = net_upg_info_dl_cb;
#ifndef WIN32
  MT_ASSERT(SUCCESS == mul_net_extern_start(&req));
#endif
}

void ui_net_upg_sts_show(control_t *p_cont, u16 *p_str)
{
  control_t *p_text;
  p_text = ctrl_get_child_by_id(p_cont, 1);
  text_set_content_by_ascstr(p_text, " ");
  text_set_content_by_unistr(p_text, p_str);
  ctrl_paint_ctrl(p_cont, TRUE);
}

static RET_CODE upgrade_file_update(control_t *p_ctrl, u16 focus,
                                       u16 *p_str, u16 max_length)
{
  u16 uni_str[32] = {0};

  if(0 == s_net_upg_info.item_cnt)
  {
    str_nasc2uni("No file", uni_str, DB_DVBS_MAX_NAME_LENGTH);
  }
  else
  {
      if(focus >= s_net_upg_info.item_cnt)
      {
          return ERR_FAILURE;
      }

      /* NAME */
      str_asc2uni(s_net_upg_info.item[focus].name, uni_str);
    }

  uni_strncpy(p_str , uni_str, max_length-1);

  return SUCCESS;
}

RET_CODE open_net_upg(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[NET_UPG_ITEM_CNT],*p_sts;
  u8 i, j;
  u16 stxt [NET_UPG_ITEM_CNT] = { IDS_UPGRADE_MODE, IDS_URL, IDS_UPGRADE_FILE, IDS_START };
  u16 smode[NET_UPG_MODE_CNT] =
    {
      IDS_UPGRADE_ALL_CODE, IDS_UPGRADE_ALL_CODE, IDS_UPGRADE_ALL_CODE,
    };
  u16 y;
  u16 url[256] = {0};

  memset(&s_net_upg_info, 0, sizeof(s_net_upg_info));
  
  p_cont = fw_create_mainwin(ROOT_ID_NET_UPG,
							   XUPGRADE_BY_NET_X, XUPGRADE_BY_NET_Y,
							   XUPGRADE_BY_NET_W, XUPGRADE_BY_NET_H,
                             ROOT_ID_XSYS_SET, 0, OBJ_ATTR_ACTIVE, 0);
  ctrl_set_rstyle(p_cont,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, net_upg_cont_keymap);
  ctrl_set_proc(p_cont, net_upg_cont_proc);

  y = NET_UPG_ITEM_Y;
  for (i = 0; i < NET_UPG_ITEM_CNT; i++)
  {
    p_ctrl[i] = NULL;
    switch (i)
    {
      case 0://mode
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_NET_UPG_MODE+ i),
          NET_UPG_ITEM_X, y, NET_UPG_ITEM_LW, NET_UPG_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
          NET_UPG_MODE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < NET_UPG_MODE_CNT; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, smode[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], 0);
        ui_comm_ctrl_set_proc(p_ctrl[i], net_upg_mode_proc);
        break;

       case 1:
        p_ctrl[i] = ui_comm_t9_v2_edit_create(p_cont, (u8)(IDC_NET_UPG_MODE+ i),
                                              NET_UPG_ITEM_X, y, NET_UPG_ITEM_LW-100, 
                                              NET_UPG_ITEM_RW+100, ROOT_ID_NET_UPG);     
		
        ui_comm_t9_v2_edit_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], net_upg_url_proc);   

        str_asc2uni(HTTP_NET_UPG_ADDR, url);

        ui_comm_t9_v2_edit_set_content_by_unistr(p_ctrl[i], url); 
       
        break;       

      case 2://file
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_NET_UPG_MODE+ i),
                                              NET_UPG_ITEM_X, y, NET_UPG_ITEM_LW, 
                                              NET_UPG_ITEM_RW);    
		
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_DYNAMIC,
                    0, CBOX_ITEM_STRTYPE_UNICODE, upgrade_file_update);
        ui_comm_select_set_focus(p_ctrl[i], 0);
        ui_comm_ctrl_update_attr(p_ctrl[i], FALSE);
        ui_comm_ctrl_set_proc(p_ctrl[i], net_upg_file_proc);
        break;
		
      case 3://start
        p_ctrl[i] = ui_comm_static_create(p_cont, (u8)(IDC_NET_UPG_MODE + i),
          NET_UPG_ITEM_X, y, NET_UPG_ITEM_LW, NET_UPG_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], net_upg_start_text_proc);
        ui_comm_ctrl_update_attr(p_ctrl[i], FALSE); 

        y += NET_UPG_ITEM_H + NET_UPG_ITEM_V_GAP;
        break;
      case 4://pbar
        p_ctrl[i] = ui_comm_bar_create(p_cont, (u8)(IDC_NET_UPG_MODE + i),
          NET_UPG_PBAR_X, (y + (NET_UPG_TXT_H - NET_UPG_PBAR_H)/2),
          NET_UPG_PBAR_W, NET_UPG_PBAR_H,
          NET_UPG_TXT_X, y, NET_UPG_TXT_W, NET_UPG_TXT_H,
          NET_UPG_PER_X, y, NET_UPG_PER_W, NET_UPG_PER_H);
        ui_comm_bar_set_param(p_ctrl[i], RSC_INVALID_ID, 0, 100, 100);
        ui_comm_bar_set_style(p_ctrl[i],
                                RSI_NET_UPG_PBAR_BG, RSI_NET_UPG_PBAR_MID,
                                RSI_IGNORE, FSI_WHITE,
                                RSI_PBACK, FSI_WHITE);
        ui_comm_bar_update(p_ctrl[i], 0, TRUE);
        break;
      case 5://net_upg status.
        p_ctrl[i] = ctrl_create_ctrl(CTRL_CONT, (u8)(IDC_NET_UPG_MODE + i),
          NET_UPG_ITEM_X, y, (NET_UPG_ITEM_LW + NET_UPG_ITEM_RW),
          ((NET_UPG_ITEM_H + NET_UPG_ITEM_V_GAP) * NET_UPG_STS_ITEM_CNT - NET_UPG_ITEM_V_GAP),
          p_cont, 0);
	 ctrl_set_rstyle(p_ctrl[i], RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH);
        break;
      default:
        MT_ASSERT(0);
        break;
    }

    y += NET_UPG_ITEM_H + NET_UPG_ITEM_V_GAP;

    ctrl_set_related_id(p_ctrl[i],
                      0,                       /* left */
                      (u8)((i - 1 + 4)
                           % 4 + 1),    /* up */
                      0,                       /* right */
                      (u8)((i + 1) % 4 + 1));    /* down */
    
  }

  //create status item bar.
  y = 0;

  p_sts = ctrl_create_ctrl(CTRL_TEXT, 1,
  						0, 0,
  						(NET_UPG_ITEM_LW + NET_UPG_ITEM_RW),
  						NET_UPG_ITEM_H, p_ctrl[IDC_NET_UPG_STS - 1], 0);
  ctrl_set_rstyle(p_sts, RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH);
  text_set_align_type(p_sts, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_sts, FSI_COMM_CTRL_SH,
        FSI_COMM_CTRL_HL, FSI_COMM_CTRL_GRAY);
  text_set_content_type(p_sts, TEXT_STRTYPE_UNICODE);
  text_set_content_by_ascstr(p_sts, " ");

  //ctrl_set_related_id(p_ctrl[0], 0, IDC_NET_UPG_START, 0, IDC_NET_UPG_START);
  //ctrl_set_related_id(p_ctrl[1], 0, IDC_NET_UPG_MODE, 0, IDC_NET_UPG_MODE);

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  ui_init_net_upg();


  return SUCCESS;
}


static RET_CODE on_net_upg_quit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{

  manage_close_menu(ROOT_ID_NET_UPG, 0, 0);

  return SUCCESS;
}

static RET_CODE on_net_upg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if( UI_NET_UPG_WRITING == ui_net_epg_sts )
    return SUCCESS;

  return ERR_NOFEATURE;
}
static RET_CODE on_net_upg_destroy(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_release_net_upg();
  mtos_printk("@@@@@ %s,%d\n\n",__FUNCTION__,__LINE__);
  return ERR_NOFEATURE;
}

static RET_CODE ui_refresh_menu(control_t *p_cont)
{
   control_t *p_start, *p_pbar, *p_mode, *p_sts, *p_file, *p_url;

  p_start = ctrl_get_child_by_id(p_cont, IDC_NET_UPG_START);
  ui_comm_ctrl_update_attr(p_start, TRUE);
  ctrl_process_msg(p_start, MSG_GETFOCUS, 0, 0);

  p_mode = ctrl_get_child_by_id(p_cont, IDC_NET_UPG_MODE);
  ui_comm_ctrl_update_attr(p_mode, TRUE);

  p_url = ctrl_get_child_by_id(p_cont, IDC_NET_UPG_URL);
  ui_comm_ctrl_update_attr(p_url, TRUE);

  p_file = ctrl_get_child_by_id(p_cont, IDC_NET_UPG_FILE);
  ui_comm_ctrl_update_attr(p_file, TRUE);

  p_pbar = ctrl_get_child_by_id(p_cont, IDC_NET_UPG_PBAR);
  ui_comm_bar_update(p_pbar, 0, TRUE);

  p_sts = ctrl_get_child_by_id(p_cont, IDC_NET_UPG_STS);
  text_set_content_by_ascstr(ctrl_get_child_by_id(p_sts, 1), " ");

  ctrl_paint_ctrl(p_cont, TRUE);

  return SUCCESS;
}

static RET_CODE on_net_upg_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_file = NULL;
  u16 file_focus;
  
  comm_dlg_data_t p_data =
  	{
  	    ROOT_ID_INVALID,
 	    DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
 	    COMM_DLG_X, COMM_DLG_Y,
 	    COMM_DLG_W,COMM_DLG_H,
 	    IDS_USB_UPG_RESTART,
 	    0	,
  	};
  p_file = ui_comm_root_get_ctrl(ROOT_ID_NET_UPG, IDC_NET_UPG_FILE);
  file_focus = ui_comm_select_get_focus(p_file);
  mul_ota_dm_api_save_ota_param_upg_version(s_net_upg_info.item[file_focus].sw);
  ui_comm_dlg_open(&p_data);
  ui_refresh_menu(p_ctrl);

  return SUCCESS;
}

static RET_CODE on_netupg_fail(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  comm_dlg_data_t p_data = 
  	{
  	    ROOT_ID_INVALID,
 	    DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
 	    COMM_DLG_X, COMM_DLG_Y,
 	    COMM_DLG_W,COMM_DLG_H,
 	    IDS_FAILED,
 	    0	,
  	};

  OS_PRINTF("%s(), %d\n", __FUNCTION__, __LINE__);

  ui_comm_dlg_open(&p_data);
  ui_refresh_menu(p_ctrl);
  return SUCCESS;
}


static BOOL is_valid_url(u8* p_url)
{
  char c;
  int len = 0;
  while((c = *p_url++))
  {
    len++;
    if( isspace(c) || '\\' == c )
      return FALSE;
  }
  if(len > 0)
    return TRUE;
  else
    return FALSE;
}

static RET_CODE on_net_upg_start_text_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_cont,*p_mode,*p_start,*p_sts,*p_url,*p_file = NULL;
  u16 mode_focus, file_focus;
  u16 uni_str[STS_STR_DUMP_MAX_LEN] = {0};
  
  if(UI_NET_UPG_IDEL == ui_net_epg_sts)
  {
    ui_net_epg_sts = UI_NET_UPG_PRE_READ;
    p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));
    p_mode = ctrl_get_child_by_id(p_cont, IDC_NET_UPG_MODE);
    p_start = ctrl_get_child_by_id(p_cont, IDC_NET_UPG_START);
    p_sts = ctrl_get_child_by_id(p_cont, IDC_NET_UPG_STS);
    p_url = ctrl_get_child_by_id(p_cont, IDC_NET_UPG_URL);
    p_file = ctrl_get_child_by_id(p_cont, IDC_NET_UPG_FILE);
    mode_focus = ui_comm_select_get_focus(p_mode);
    file_focus = ui_comm_select_get_focus(p_file);

    switch (mode_focus)
    {
      case 0:  // all code
        break;
      case 1:   //user db
        break;
      default:
        break;
    }

    if( is_valid_url(s_net_upg_info.item[file_focus].url) )
    {
      OS_PRINTF("%s(), %d, netupg url:%s\n", __FUNCTION__, __LINE__, s_net_upg_info.item[file_focus].url);
      ui_net_upg_start(s_net_upg_info.item[file_focus].url);
    }
    else
    {
      OS_PRINTF("%s(), %d, invalid URL\n", __FUNCTION__, __LINE__);
      return SUCCESS;
    }

    ui_comm_ctrl_update_attr(p_mode, FALSE);
    ctrl_process_msg(p_start, MSG_LOSTFOCUS, 0, 0);
    ui_comm_ctrl_update_attr(p_start, FALSE);
    ui_comm_ctrl_update_attr(p_url, FALSE);
    ui_comm_ctrl_update_attr(p_file, FALSE);
    gui_get_string(IDS_DOWNLOAD_TO_WAIT, uni_str, STS_STR_DUMP_MAX_LEN);
    ui_net_upg_sts_show(p_sts,uni_str);
    ctrl_paint_ctrl(p_cont, TRUE);
  }

  return SUCCESS;
}

static void ui_netupg_giveup(void)
{
  control_t *p_root = ui_comm_root_get_root(ROOT_ID_NET_UPG);
  ui_net_epg_sts = UI_NET_UPG_IDEL;
  sg_offset = 0;

  ui_refresh_menu(p_root);
}

static void ui_netupg_start_burn(void)
{
  cmd_t cmd = {0};
  u16 uni_str[32] = {0};
  control_t *p_sts = ui_comm_root_get_ctrl(ROOT_ID_NET_UPG, IDC_NET_UPG_STS);
  control_t *p_start = ui_comm_root_get_ctrl(ROOT_ID_NET_UPG, IDC_NET_UPG_START);
  ui_comm_ctrl_update_attr(p_start, FALSE);
  ctrl_paint_ctrl(p_start, TRUE);


  ui_net_epg_sts = UI_NET_UPG_WRITING;
    
  cmd.id = USB_UPG_CMD_START_BURN;
  cmd.data1 = (u32)(&usb_param);
  ap_frm_do_command(APP_USB_UPG, &cmd);

  gui_get_string(IDS_BURN_FLASH, uni_str, sizeof(uni_str)/sizeof(uni_str[0]));
  ui_net_upg_sts_show(p_sts,uni_str);
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
      p_block_info = (dmh_block_info_t *)(usb_param.ext_buf_addr + indentity_block_head_offset);
      if(dataLen > p_block_info->size)
      {
        return;
      }
      memcpy((u8 *)(usb_param.ext_buf_addr + indentity_block_offset), block, dataLen);
      memset((u8 *)(usb_param.ext_buf_addr + indentity_block_offset + dataLen), 0xFF, p_block_info->size - dataLen);
    }
  }
}

static RET_CODE on_netupg_status_up(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  upg_status_t *sts = (upg_status_t *)para1;
  control_t *p_bar = ui_comm_root_get_ctrl(ROOT_ID_NET_UPG, IDC_NET_UPG_PBAR);

  switch(sts->sm)
  {
    case USB_SM_REMOTE_ERROR:
    break;

    case USB_DUMP_SM_READING_FLASH:
      ui_comm_bar_update(p_bar, (u16)sts->progress, TRUE);
      ui_set_front_panel_by_str_with_upg("D",(u8)sts->progress);
      ui_comm_bar_paint(p_bar, TRUE);

      if( 100 == sts->progress )
      {
        if(usb_param.mode == USB_UPGRD_ALL)
        {
          _keep_serial_num();
        }
        ui_comm_ask_for_dodlg_open(NULL, IDS_SURE_BURN_FLASH,\
                                                      ui_netupg_start_burn, ui_netupg_giveup, 0);
      }

    break;

    case USB_UPG_SM_BURNING:
      ui_comm_bar_update(p_bar, (u16)sts->progress, TRUE);
      ui_set_front_panel_by_str_with_upg("W",(u8)sts->progress);
      ui_comm_bar_paint(p_bar, TRUE);
    break;

    default:
    return ERR_FAILURE;
    break;
  }

  return SUCCESS;
}



static RET_CODE edit_url_update(u16 *p_unistr)
{
  control_t *p_edit_cont = NULL;
  u16 *p_url = NULL;
  static u8 url[256] = {0};
  comm_dlg_data_t dlg_data = {0};	 

  p_edit_cont = ui_comm_root_get_ctrl(ROOT_ID_NET_UPG, IDC_NET_UPG_URL);
  if(p_edit_cont == NULL)
  {
    return ERR_FAILURE;
  }

  p_url = ui_comm_t9_v2_edit_get_content(p_edit_cont);
  
  ctrl_paint_ctrl(p_edit_cont, TRUE);

  str_uni2asc(url, p_url);
  if( is_valid_url(url) )
  {
    dlg_data.parent_root = ROOT_ID_MAINMENU;
    dlg_data.style = DLG_FOR_SHOW | DLG_STR_MODE_STATIC;
    dlg_data.x = ((SCREEN_WIDTH - 400) / 2);
    dlg_data.y = ((SCREEN_HEIGHT - 200) / 2);
    dlg_data.w = 400;
    dlg_data.h = 160;
    dlg_data.content = IDS_WAIT_PLEASE;
    dlg_data.dlg_tmout = 0;
    ui_comm_dlg_open(&dlg_data); 
    ui_evt_disable_ir();
    OS_PRINTF("%s(), %d, netupg url:%s\n", __FUNCTION__, __LINE__, url);
    ui_net_upg_download_upg_info(url);
  }
  else
  {
    OS_PRINTF("%s(), %d, invalid URL\n", __FUNCTION__, __LINE__);
  }

  return SUCCESS;

}

static RET_CODE on_net_upg_url_edit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  kb_param_t param;

  param.uni_str = ebox_get_content(p_ctrl);
  param.type = KB_INPUT_TYPE_SENTENCE;
  param.max_len = MAX_URL_LENGTH;
  param.cb = edit_url_update;
  manage_open_menu(ROOT_ID_KEYBOARD_V2, 0, (u32) & param);
  
  return SUCCESS;
}

static RET_CODE on_net_upg_info_arrived(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_file, *p_start;
  
  ui_comm_dlg_close();
  p_file = ui_comm_root_get_ctrl(ROOT_ID_NET_UPG, IDC_NET_UPG_FILE);
  p_start = ui_comm_root_get_ctrl(ROOT_ID_NET_UPG, IDC_NET_UPG_START);
  ui_comm_select_set_count(p_file, CBOX_WORKMODE_DYNAMIC, (u16)s_net_upg_info.item_cnt);
  ui_comm_select_set_focus(p_file, 0);

  if(s_net_upg_info.item_cnt > 0)
  {
    ui_comm_ctrl_update_attr(p_start, TRUE);
    ui_comm_ctrl_update_attr(p_file, TRUE);
  }
  else
  {
    ui_comm_ctrl_update_attr(p_start, FALSE);
    ui_comm_ctrl_update_attr(p_file, FALSE);
  }
  ctrl_paint_ctrl(p_ctrl, TRUE);
  
  ui_evt_enable_ir();
  
  return SUCCESS;
}

BEGIN_KEYMAP(net_upg_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
END_KEYMAP(net_upg_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(net_upg_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_net_upg_exit)
  ON_COMMAND(MSG_EXIT, on_net_upg_exit)
  ON_COMMAND(MSG_POWER_OFF, on_net_upg_exit)
  ON_COMMAND(MSG_UPG_OK, on_net_upg_ok)
  ON_COMMAND(MSG_UPG_UPDATE_STATUS, on_netupg_status_up)
  ON_COMMAND(MSG_UPG_FAIL, on_netupg_fail)
  ON_COMMAND(MSG_UPG_QUIT, on_net_upg_quit)
  ON_COMMAND(MSG_DESTROY,on_net_upg_destroy)
  ON_COMMAND(MSG_NET_UPG_EVT_INFO_ARRIVED,on_net_upg_info_arrived)
END_MSGPROC(net_upg_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(net_upg_mode_proc, cbox_class_proc)
END_MSGPROC(net_upg_mode_proc, cbox_class_proc)


BEGIN_MSGPROC(net_upg_url_proc, ui_comm_edit_proc)
  ON_COMMAND(MSG_SELECT, on_net_upg_url_edit)
END_MSGPROC(net_upg_url_proc, ui_comm_edit_proc)

BEGIN_MSGPROC(net_upg_start_text_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_net_upg_start_text_select)
END_MSGPROC(net_upg_start_text_proc, text_class_proc)

BEGIN_MSGPROC(net_upg_file_proc, cbox_class_proc)
END_MSGPROC(net_upg_file_proc,cbox_class_proc)

BEGIN_AP_EVTMAP(ui_net_upg_evtmap)
  CONVERT_EVENT(USB_UPG_EVT_UPDATE_STATUS, MSG_UPG_UPDATE_STATUS)
  CONVERT_EVENT(USB_UPG_EVT_SUCCESS, MSG_UPG_OK)
  CONVERT_EVENT(USB_INVLID_UPGRADE_FILE, MSG_UPG_FAIL)
  CONVERT_EVENT(USB_UPG_BURN_FLASH_ERROR, MSG_UPG_FAIL)
END_AP_EVTMAP(ui_net_upg_evtmap)

