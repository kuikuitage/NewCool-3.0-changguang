#include "ui_common.h"
#include "ui_xsys_set.h"
#include "ui_xupgrade_by_rs232.h"
#include "uio.h"

typedef enum
{
  IDC_INVALID = 0,
  IDC_XUPGRADE_BY_RS232_MODE,
  IDC_XUPGRADE_BY_RS232_TYPE,
  IDC_XUPGRADE_BY_RS232_START,
  IDC_XUPGRADE_BY_RS232_PBAR,
  IDC_XUPGRADE_BY_RS232_STS,
}xupgrade_by_rs232_control_id_t;

static u16 xupgrade_by_rs232_cont_keymap(u16 key);
static RET_CODE xupgrade_by_rs232_cont_proc(control_t *cont, u16 msg, u32 para1, u32 para2);
static RET_CODE xupgrade_by_rs232_text_proc(control_t *p_text, u16 msg, u32 para1, u32 para2);
static u16 ui_xupg_by_rs232_evtmap(u32 event);

static upg_block_t ui_xupg_block[3] = {{0,0,0},};
static BOOL is_xupgrading = FALSE;

BOOL ui_is_xrs232_upgrading(void)
{
  return is_xupgrading;
}

void ui_xupgrade_by_rs232_sts_show(control_t *p_cont, u8 *p_str)
{
  control_t *p_text[XUPGRADE_BY_RS232_STS_ITEM_CNT];
  u32 str_char[XUPGRADE_BY_RS232_STS_ITEM_CNT];
  u8 i;

  for (i = 0; i < XUPGRADE_BY_RS232_STS_ITEM_CNT; i++)
  {
    p_text[i] = ctrl_get_child_by_id(p_cont, (u8)(i + 1));
    str_char[i] = text_get_content(p_text[i]);
  }

  for (i = 0; i < (XUPGRADE_BY_RS232_STS_ITEM_CNT - 1); i++)
  {
    text_set_content_by_unistr(p_text[i], (u16 *)str_char[i + 1]);
  }

  text_set_content_by_ascstr(p_text[XUPGRADE_BY_RS232_STS_ITEM_CNT - 1], p_str);

  ctrl_paint_ctrl(ctrl_get_parent(p_cont), TRUE);
}

static void xupgrade_by_rs232_states_update(u8 status, s32 progress, u8 *description)
{
  control_t *p_pbar, *p_sts;
  static u8 sts[70] ={0,};
  u8 tmp[10] ={0,};
  uio_device_t *p_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_UIO);

  MT_ASSERT(NULL != p_dev);

  switch(status)
  {
    case UPG_SM_MASETER_ERROR:        //Master side ERROR, progress = Err Code
      sprintf(sts, "M-ERR:0x%x,%s",(unsigned)progress, description);
      sprintf(tmp, "0%c%2x", 'E', (unsigned)progress);
      break;
    case UPG_SM_REMOTE_ERROR:        //slave side ERROR: 'R'+ cmd id + remote error code
      sprintf(sts, "R-ERR:0x%x,%s",(unsigned)progress, description);
      sprintf(tmp,"%c%02x%1d", 'R', (unsigned)(progress>>8), (unsigned)progress&&0xFF);
      break;
    case UPG_SM_SYNCING:
      //ui_xupgrade_by_rs232_set_enable_exit(TRUE);
      sprintf(tmp, "0%c%02d",'S', (unsigned)progress);
      break;
    case UPG_SM_NEGOTIATING:
      sprintf(tmp, "0%c%02d",'N', (unsigned)progress);
      break;
    case UPG_SM_TRANSFERING:
      sprintf(tmp, "0%c%02d",'T', (unsigned)progress);
      break;
    case UPG_SM_BURNING:
      sprintf(tmp, "0%c%02d",'B', (unsigned)progress);
      break;
    case UPG_SM_REBOOTING:
      //sprintf(tmp, "%c%03d",'B', progress);
      break;
    default:
      sprintf(tmp, "0%c%02d",'U', (unsigned)progress);
      break;
   }

  //ui_set_front_panel_by_str(tmp);
  uio_display(p_dev, tmp, 4);

  p_pbar = ui_comm_root_get_ctrl(ROOT_ID_XUPGRADE_BY_RS232, IDC_XUPGRADE_BY_RS232_PBAR);
  p_sts = ui_comm_root_get_ctrl(ROOT_ID_XUPGRADE_BY_RS232, IDC_XUPGRADE_BY_RS232_STS);

  if(is_xupgrading)
  {
    if((UPG_SM_MASETER_ERROR != status) && (UPG_SM_REMOTE_ERROR != status))
    {
      ui_comm_bar_update(p_pbar, (u16)progress, TRUE);
      ui_comm_bar_paint(p_pbar, TRUE);
      if (NULL != description)
      {
        sprintf(sts, "%s", description);
        ui_xupgrade_by_rs232_sts_show(p_sts, sts);
      }
    }
    else
    {
      ui_xupgrade_by_rs232_sts_show(p_sts, sts);
    }
  }

}

#define UBOOT_BLOCK_ID                0xFF

RET_CODE xupgrade_by_rs232_start(u32 block, u32 type)
{
  upg_mode_t md = 0;
  u8 block_num = 0;
  void *p_dm_handle = class_get_handle_by_id(DM_CLASS_ID);

  if (0 == block) //xupgrade_by_rs232 all
  {
    //md = UPGRD_ALL_SINGLE;
    md = type;//UPGRD_ALL_SINGLE, UPGRD_ALL_MULTI

    block_num = 1;
    ui_xupg_block[0].master_block_id = P2P_UPGRADE_ALL;
    ui_xupg_block[0].slave_block_id = P2P_UPGRADE_ALL;
    ui_xupg_block[0].size = CHARSTO_SIZE;
  }
  else //xupgrade_by_rs232 block
  {
    switch(block)
    {
      case 1://maincode
        block_num = 2;
        ui_xupg_block[0].master_block_id = MAINCODE_BLOCK_ID;
        ui_xupg_block[0].slave_block_id = MAINCODE_BLOCK_ID;
        ui_xupg_block[0].size = dm_get_block_size(p_dm_handle, MAINCODE_BLOCK_ID);

        ui_xupg_block[1].master_block_id = RS_BLOCK_ID;
        ui_xupg_block[1].slave_block_id = RS_BLOCK_ID;
        ui_xupg_block[1].size = dm_get_block_size(p_dm_handle, RS_BLOCK_ID);
        break;
      case 2://radio back
        block_num = 2;
        ui_xupg_block[0].master_block_id = LOGO_BLOCK_ID_M0;
        ui_xupg_block[0].slave_block_id = LOGO_BLOCK_ID_M0;
        ui_xupg_block[0].size = dm_get_block_size(p_dm_handle, LOGO_BLOCK_ID_M0);
        
        ui_xupg_block[1].master_block_id = LOGO_BLOCK_ID_M1;
        ui_xupg_block[1].slave_block_id = LOGO_BLOCK_ID_M1;
        ui_xupg_block[1].size = dm_get_block_size(p_dm_handle, LOGO_BLOCK_ID_M1);
        break;
      case 3://user db
        block_num = 3;
      ui_xupg_block[0].master_block_id = IW_TABLE_BLOCK_ID;
      ui_xupg_block[0].slave_block_id = IW_TABLE_BLOCK_ID;
      ui_xupg_block[0].size = dm_get_block_size(p_dm_handle, IW_TABLE_BLOCK_ID); 
      
      ui_xupg_block[1].master_block_id = IW_VIEW_BLOCK_ID;
      ui_xupg_block[1].slave_block_id = IW_VIEW_BLOCK_ID;
      ui_xupg_block[1].size = dm_get_block_size(p_dm_handle, IW_VIEW_BLOCK_ID);

      ui_xupg_block[2].master_block_id = PRESET_BLOCK_ID;
      ui_xupg_block[2].slave_block_id = PRESET_BLOCK_ID;
      ui_xupg_block[2].size = dm_get_block_size(p_dm_handle, PRESET_BLOCK_ID);
        break;
/*      case 4://user db
        block_num = 1;
      ui_xupg_block[0].master_block_id = BSDATA_BLOCK_ID;
      ui_xupg_block[0].slave_block_id = BSDATA_BLOCK_ID;
      ui_xupg_block[0].size = dm_get_block_size(p_dm_handle, BSDATA_BLOCK_ID); 
      break;*/
      default:
        MT_ASSERT(0);
        break;
    }

    
    md = UPGRD_BLOCKS_SINGLE + type;//UPGRD_BLOCKS_SINGLE, UPGRD_BLOCKS_MULTI
  }
  
  ui_start_upgrade(md,block_num, ui_xupg_block);
  return SUCCESS;
}

RET_CODE open_xupgrade_by_rs232(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[XUPGRADE_BY_RS232_ITEM_CNT];
  control_t *p_sts[XUPGRADE_BY_RS232_STS_ITEM_CNT];
  u8 i, j;
  u16 stxt [XUPGRADE_BY_RS232_ITEM_CNT] = { IDS_UPGRADE_MODE, IDS_UPGRADE_TYPE, IDS_START };
  u16 smode[XUPGRADE_BY_RS232_MODE_CNT] = 
    {
      IDS_UPGRADE_ALL_CODE//, IDS_MAINCODE, IDS_RADIOLOGO,IDS_USER_DB
     // IDS_DEFAULT_DB, IDS_USER_DB
    };
  u16 stype[XUPGRADE_BY_RS232_TYPE_CNT] =
    {
      IDS_ONCE,
      IDS_CONTINUOUS,
    };
  u16 y;

  p_cont =fw_create_mainwin(ROOT_ID_XUPGRADE_BY_RS232, 
    XUPGRADE_BY_RS232_X, 
    XUPGRADE_BY_RS232_Y, 
    XUPGRADE_BY_RS232_W,
    XUPGRADE_BY_RS232_H,
    ROOT_ID_XSYS_SET, 0, OBJ_ATTR_ACTIVE, 0);
  ctrl_set_rstyle(p_cont,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, xupgrade_by_rs232_cont_keymap);
  ctrl_set_proc(p_cont, xupgrade_by_rs232_cont_proc);

  y = XUPGRADE_BY_RS232_ITEM_Y;
  for (i = 0; i < XUPGRADE_BY_RS232_ITEM_CNT; i++)
  {
    p_ctrl[i] = NULL;
    switch (i)
    {
      case 0://mode
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_XUPGRADE_BY_RS232_MODE+ i),
          XUPGRADE_BY_RS232_ITEM_X, y, XUPGRADE_BY_RS232_ITEM_LW, XUPGRADE_BY_RS232_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
          XUPGRADE_BY_RS232_MODE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < XUPGRADE_BY_RS232_MODE_CNT; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, smode[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], 0);
        break;
      case 1://type
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_XUPGRADE_BY_RS232_MODE+ i),
          XUPGRADE_BY_RS232_ITEM_X, y, XUPGRADE_BY_RS232_ITEM_LW, XUPGRADE_BY_RS232_ITEM_RW);
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_select_set_param(p_ctrl[i], TRUE, CBOX_WORKMODE_STATIC,
          XUPGRADE_BY_RS232_TYPE_CNT, CBOX_ITEM_STRTYPE_STRID, NULL);
        for(j = 0; j < XUPGRADE_BY_RS232_TYPE_CNT; j++)
        {
          ui_comm_select_set_content(p_ctrl[i], j, stype[j]);
        }
        ui_comm_select_set_focus(p_ctrl[i], 0);
        break;

      case 2://start
        p_ctrl[i] = ui_comm_static_create(p_cont, (u8)(IDC_XUPGRADE_BY_RS232_MODE + i),
          XUPGRADE_BY_RS232_ITEM_X, y, XUPGRADE_BY_RS232_ITEM_LW, XUPGRADE_BY_RS232_ITEM_RW);
        ui_comm_static_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_proc(p_ctrl[i], xupgrade_by_rs232_text_proc);

        y += XUPGRADE_BY_RS232_ITEM_H + XUPGRADE_BY_RS232_ITEM_V_GAP;
        break;
      case 3://pbar
        p_ctrl[i] = ui_comm_bar_create(p_cont, (u8)(IDC_XUPGRADE_BY_RS232_MODE + i),
          XUPGRADE_BY_RS232_PBAR_X, (y + (XUPGRADE_BY_RS232_TXT_H - XUPGRADE_BY_RS232_PBAR_H)/2),
          XUPGRADE_BY_RS232_PBAR_W, XUPGRADE_BY_RS232_PBAR_H,
          XUPGRADE_BY_RS232_TXT_X, y, XUPGRADE_BY_RS232_TXT_W, XUPGRADE_BY_RS232_TXT_H,
          XUPGRADE_BY_RS232_PER_X, y, XUPGRADE_BY_RS232_PER_W, XUPGRADE_BY_RS232_PER_H);
        ui_comm_bar_set_param(p_ctrl[i], RSC_INVALID_ID, 0, 100, 100);
        ui_comm_bar_set_style(p_ctrl[i],
                                RSI_XUPGRADE_BY_RS232_PBAR_BG, RSI_XUPGRADE_BY_RS232_PBAR_MID,
                                RSI_IGNORE, FSI_WHITE,
                                RSI_PBACK, FSI_WHITE);
        ui_comm_bar_update(p_ctrl[i], 0, TRUE);
        break;
      case 4://xupgrade_by_rs232 status.
        p_ctrl[i] = ctrl_create_ctrl(CTRL_CONT, (u8)(IDC_XUPGRADE_BY_RS232_MODE + i),
          XUPGRADE_BY_RS232_ITEM_X, y, (XUPGRADE_BY_RS232_ITEM_LW + XUPGRADE_BY_RS232_ITEM_RW),
          ((XUPGRADE_BY_RS232_ITEM_H + XUPGRADE_BY_RS232_ITEM_V_GAP) * XUPGRADE_BY_RS232_STS_ITEM_CNT - XUPGRADE_BY_RS232_ITEM_V_GAP),
          p_cont, 0);
        break;
      default:
        MT_ASSERT(0);
        break;
    }

    y += XUPGRADE_BY_RS232_ITEM_H + XUPGRADE_BY_RS232_ITEM_V_GAP;
  }

  //create status item bar.
  y = 0;

  for(i = 0; i < XUPGRADE_BY_RS232_STS_ITEM_CNT; i++)
  {
    p_sts[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(i + 1),
      0, y, (XUPGRADE_BY_RS232_ITEM_LW + XUPGRADE_BY_RS232_ITEM_RW),
      XUPGRADE_BY_RS232_ITEM_H, p_ctrl[IDC_XUPGRADE_BY_RS232_STS - 1], 0);

    ctrl_set_rstyle(p_sts[i], RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH, RSI_COMM_STATIC_SH);
    text_set_align_type(p_sts[i], STL_CENTER | STL_VCENTER);
    text_set_font_style(p_sts[i], FSI_COMM_CTRL_SH,
      FSI_COMM_CTRL_HL, FSI_COMM_CTRL_GRAY);
    text_set_content_type(p_sts[i], TEXT_STRTYPE_UNICODE);
    text_set_content_by_ascstr(p_sts[i], " ");
    y += (XUPGRADE_BY_RS232_ITEM_H + XUPGRADE_BY_RS232_ITEM_V_GAP);
  }

  ctrl_set_related_id(p_ctrl[0], 0, IDC_XUPGRADE_BY_RS232_START, 0, IDC_XUPGRADE_BY_RS232_TYPE);
  ctrl_set_related_id(p_ctrl[1], 0, IDC_XUPGRADE_BY_RS232_MODE, 0, IDC_XUPGRADE_BY_RS232_START);
  ctrl_set_related_id(p_ctrl[2], 0, IDC_XUPGRADE_BY_RS232_TYPE, 0, IDC_XUPGRADE_BY_RS232_MODE);

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  ui_init_upg(APP_UPG, ROOT_ID_XUPGRADE_BY_RS232, ui_xupg_by_rs232_evtmap);
  return SUCCESS;
}


static RET_CODE on_upg_exit_all(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(is_xupgrading)
  {
    ui_exit_upgrade(APP_UPG,TRUE);
    return SUCCESS;
  }

  ui_release_upg(APP_UPG, ROOT_ID_XUPGRADE_BY_RS232);

  return ERR_NOFEATURE;
}

static RET_CODE on_upg_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  if(is_xupgrading)
  {
    ui_exit_upgrade(APP_UPG, FALSE);
    return SUCCESS;
  }

  ui_release_upg(APP_UPG, ROOT_ID_XUPGRADE_BY_RS232);

  return ERR_NOFEATURE;
}

static RET_CODE on_upg_quit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  ui_release_upg(APP_UPG, ROOT_ID_XUPGRADE_BY_RS232);
  if (para1 == TRUE)
  {
    ui_close_all_mennus();
  }
  else
  {
    manage_close_menu(ROOT_ID_XUPGRADE_BY_RS232, 0, 0);
  }

  is_xupgrading = FALSE;
  ui_set_book_flag(TRUE);

  return SUCCESS;
}

static RET_CODE on_upg_ok(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_start, *p_pbar, *p_mode, *p_type;

  is_xupgrading = FALSE;
  ui_set_book_flag(TRUE);

  p_start = ctrl_get_child_by_id(p_ctrl, IDC_XUPGRADE_BY_RS232_START);
  ui_comm_ctrl_update_attr(p_start, TRUE);
  ctrl_process_msg(p_start, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_start, TRUE);

  p_mode = ctrl_get_child_by_id(p_ctrl, IDC_XUPGRADE_BY_RS232_MODE);
  ui_comm_ctrl_update_attr(p_mode, TRUE);
  ctrl_paint_ctrl(p_mode, TRUE);

  p_type = ctrl_get_child_by_id(p_ctrl, IDC_XUPGRADE_BY_RS232_TYPE);
  ui_comm_ctrl_update_attr(p_type, TRUE);
  ctrl_paint_ctrl(p_type, TRUE);

  p_pbar = ctrl_get_child_by_id(p_ctrl, IDC_XUPGRADE_BY_RS232_PBAR);
  ui_comm_bar_update(p_pbar, 0, TRUE);
  ui_comm_bar_paint(p_pbar, TRUE);

  return SUCCESS;
}

static RET_CODE on_upg_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  upg_status_t *sts = (upg_status_t *)para1;
  xupgrade_by_rs232_states_update(sts->sm, sts->progress,sts->p_description);

  return SUCCESS;
}

static RET_CODE on_upg_clear_upg_state(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  is_xupgrading = FALSE;
  return SUCCESS;
}

static RET_CODE on_upg_text_select(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  control_t *p_mode, *p_cont, *p_start, *p_type;

  if(!is_xupgrading)
  {
    is_xupgrading = TRUE;
    ui_set_book_flag(FALSE);

    p_cont = ctrl_get_parent(ctrl_get_parent(p_ctrl));
    p_mode = ctrl_get_child_by_id(p_cont, IDC_XUPGRADE_BY_RS232_MODE);
    p_type = ctrl_get_child_by_id(p_cont, IDC_XUPGRADE_BY_RS232_TYPE);
    xupgrade_by_rs232_start(ui_comm_select_get_focus(p_mode),
    ui_comm_select_get_focus(p_type));


    ui_comm_ctrl_update_attr(p_mode, FALSE);
    ctrl_paint_ctrl(p_mode, TRUE);
    ui_comm_ctrl_update_attr(p_type, FALSE);
    ctrl_paint_ctrl(p_type, TRUE);

    p_start = ctrl_get_child_by_id(p_cont, IDC_XUPGRADE_BY_RS232_START);
    ctrl_process_msg(p_start, MSG_LOSTFOCUS, 0, 0);
    ui_comm_ctrl_update_attr(p_start, FALSE);
    ctrl_paint_ctrl(p_start, TRUE);
  }

  return SUCCESS;
}

#if 0
void upg_show_num(u32 num)
{
  control_t *p_sts;
  u8 str[16] = {0};

  p_sts = ui_comm_root_get_ctrl(ROOT_ID_XUPGRADE_BY_RS232, IDC_XUPGRADE_BY_RS232_STS);

  sprintf(str, "%d", (int)num);


  ui_xupgrade_by_rs232_sts_show(p_sts, str);
}

void upg_show_str(u8 *p_str)
{
  control_t *p_sts;

  p_sts = ui_comm_root_get_ctrl(ROOT_ID_XUPGRADE_BY_RS232, IDC_XUPGRADE_BY_RS232_STS);

  ui_xupgrade_by_rs232_sts_show(p_sts, p_str);
}
#endif

BEGIN_KEYMAP(xupgrade_by_rs232_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
  ON_EVENT(V_KEY_POWER, MSG_POWER_OFF)
END_KEYMAP(xupgrade_by_rs232_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(xupgrade_by_rs232_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_EXIT_ALL, on_upg_exit_all)
  ON_COMMAND(MSG_EXIT, on_upg_exit)
  ON_COMMAND(MSG_UPG_QUIT, on_upg_quit)
  ON_COMMAND(MSG_UPG_OK, on_upg_ok)
  ON_COMMAND(MSG_UPG_UPDATE_STATUS, on_upg_update)
  ON_COMMAND(MSG_UPG_EXIT, on_upg_clear_upg_state)
END_MSGPROC(xupgrade_by_rs232_cont_proc, ui_comm_root_proc)

BEGIN_MSGPROC(xupgrade_by_rs232_text_proc, text_class_proc)
  ON_COMMAND(MSG_SELECT, on_upg_text_select)
END_MSGPROC(xupgrade_by_rs232_text_proc, text_class_proc)

BEGIN_AP_EVTMAP(ui_xupg_by_rs232_evtmap)
  CONVERT_EVENT(UPG_EVT_UPDATE_STATUS, MSG_UPG_UPDATE_STATUS)
  CONVERT_EVENT(UPG_EVT_QUIT_UPG, MSG_UPG_QUIT)
  CONVERT_EVENT(UPG_EVT_SUCCESS, MSG_UPG_OK)
  //CONVERT_EVENT(UPG_EVT_QUIT_UPGING, MSG_UPG_EXIT)
END_AP_EVTMAP(ui_xupg_by_rs232_evtmap)

