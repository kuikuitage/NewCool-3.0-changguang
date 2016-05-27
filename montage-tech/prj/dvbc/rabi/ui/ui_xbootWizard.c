/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_xbootWizard.h"

enum control_id
{
  IDC_XINVALID = 0,
  IDC_XBOOT_WIZARD_TITLE,
  IDC_XBOOT_WIZARD_BMP,
  IDC_XBOOT_WIZARD_BOX_CONT,
  IDC_XBOOT_WIZARD_BOX_BTN,
  IDC_XBOOT_WIZARD_BOX_TXET,
  IDC_XBOOT_WIZARD_NEXTBTN,
  IDC_XBOOT_WIZARD_NEXTBTNTXET,
};

typedef struct
{
    control_t *hMain;
    control_t *hTitle;
    control_t *hBmp;
    control_t *hBoxcon;
    control_t *hBoxbtn;
    control_t *hBoxbtntxet;
    control_t *hNextbtn;
    control_t *hNextbtntxet;

    BOOL hDisp;
    u8 hPage;

} ui_bootWizard_app_t;

static ui_bootWizard_app_t g_boot;

RET_CODE xboot_wizard_btn_proc(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);
u16 xboot_wizard_btn_keymap(u16 key);

RET_CODE xboot_wizard_box_proc(control_t *ctrl, u16 msg,
                            u32 para1, u32 para2);
u16 xboot_wizard_box_keymap(u16 key);

static control_t *create_xbootWizard_menu()
{
  control_t *p_cont = NULL;
  control_t *p_title = NULL;
  control_t *p_bmp = NULL;
  control_t *p_box_con = NULL;
  control_t *p_box_btn = NULL;
  control_t *p_box_txet = NULL;
  control_t *p_nextbtn = NULL;
  control_t *p_nextbtntxet = NULL;
  
  /*create the main window*/
  p_cont = fw_create_mainwin(ROOT_ID_XBOOT_WIZARD,
							   XBOOT_WIZARD_X, XBOOT_WIZARD_Y,
							   XBOOT_WIZARD_W, XBOOT_WIZARD_H,
                             ROOT_ID_INVALID, 0,OBJ_ATTR_ACTIVE, 0);
  ctrl_set_rstyle(p_cont,RSI_LEFT_CONT_BG,RSI_LEFT_CONT_BG,RSI_LEFT_CONT_BG);
  p_title= ctrl_create_ctrl(CTRL_TEXT, 
                                        IDC_XBOOT_WIZARD_TITLE,
                                        XBOOT_WIZARD_TITLE_X, 
                                        XBOOT_WIZARD_TITLE_Y, 
                                        XBOOT_WIZARD_TITLE_W,
                                        XBOOT_WIZARD_TITLE_H, p_cont, 0);
  ctrl_set_rstyle(p_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_title, FSI_WHITE_56, FSI_WHITE_56, FSI_WHITE_56);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);

  p_bmp= ctrl_create_ctrl(CTRL_BMAP, 
                                        IDC_XBOOT_WIZARD_BMP,
                                        XBOOT_WIZARD_BMP_X, 
                                        XBOOT_WIZARD_BMP_Y, 
                                        XBOOT_WIZARD_BMP_W,
                                        XBOOT_WIZARD_BMP_H, p_cont, 0);
  bmap_set_align_type(p_bmp, STL_CENTER|STL_VCENTER);
  bmap_set_content_by_id(p_bmp, IM_ARROW1_L);
      p_box_con= ctrl_create_ctrl(CTRL_CONT, 
                                        IDC_XBOOT_WIZARD_BOX_CONT,
                                        XBOOT_WIZARD_BOX_CON_X, 
                                        XBOOT_WIZARD_BOX_CON_Y, 
                                        XBOOT_WIZARD_BOX_CON_W,
                                        XBOOT_WIZARD_BOX_CON_H, p_cont, 0);
    ctrl_set_rstyle(p_box_con,  RSI_PBACK, RSI_MEDIO_LEFT_LIST_HL, RSI_PBACK);
 ctrl_set_keymap(p_box_con, xboot_wizard_box_keymap);
    ctrl_set_proc(p_box_con, xboot_wizard_box_proc);

    p_box_btn= ctrl_create_ctrl(CTRL_BMAP, 
                                        IDC_XBOOT_WIZARD_BOX_BTN,
                                        XBOOT_WIZARD_BOX_BTN_X, 
                                        XBOOT_WIZARD_BOX_BTN_Y, 
                                        XBOOT_WIZARD_BOX_BTN_W,
                                        XBOOT_WIZARD_BOX_BTN_H, p_box_con, 0);
  bmap_set_align_type(p_box_btn, STL_CENTER|STL_VCENTER);
  bmap_set_content_by_id(p_box_btn, IM_OPENDVB_CTRLBOX_SH);
  ctrl_set_sts(p_box_con, OBJ_STS_HIDE);
    p_box_txet= ctrl_create_ctrl(CTRL_TEXT, 
                                        IDC_XBOOT_WIZARD_BOX_TXET,
                                        XBOOT_WIZARD_BOX_TXET_X, 
                                        XBOOT_WIZARD_BOX_TXET_Y, 
                                        XBOOT_WIZARD_BOX_TXET_W,
                                        XBOOT_WIZARD_BOX_TXET_H, p_cont, 0);
  ctrl_set_rstyle(p_box_txet, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_box_txet, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
  text_set_align_type(p_box_txet, STL_LEFT| STL_VCENTER);
  text_set_content_type(p_box_txet, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_box_txet, IDS_PAGE3_PROMPT);
  ctrl_set_sts(p_box_txet, OBJ_STS_HIDE);

      p_nextbtn= ctrl_create_ctrl(CTRL_TEXT, 
                                        IDC_XBOOT_WIZARD_NEXTBTN,
                                        XBOOT_WIZARD_NEXTBTN_X, 
                                        XBOOT_WIZARD_NEXTBTN_Y, 
                                        XBOOT_WIZARD_NEXTBTN_W,
                                        XBOOT_WIZARD_NEXTBTN_H, p_cont, 0);
  ctrl_set_rstyle(p_nextbtn, RSI_MEDIO_LEFT_LIST_SH, RSI_MEDIO_LEFT_LIST_HL, RSI_MEDIO_LEFT_LIST_SH);
  text_set_font_style(p_nextbtn, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
  text_set_align_type(p_nextbtn, STL_CENTER| STL_VCENTER);
  text_set_content_type(p_nextbtn, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_nextbtn, IDS_OK);
  ctrl_set_keymap(p_nextbtn, xboot_wizard_btn_keymap);
    ctrl_set_proc(p_nextbtn, xboot_wizard_btn_proc);

      p_nextbtntxet= ctrl_create_ctrl(CTRL_TEXT, 
                                        IDC_XBOOT_WIZARD_NEXTBTNTXET,
                                        XBOOT_WIZARD_NEXTBTNTXET_X, 
                                        XBOOT_WIZARD_NEXTBTNTXET_Y, 
                                        XBOOT_WIZARD_NEXTBTNTXET_W,
                                        XBOOT_WIZARD_NEXTBTNTXET_H, p_cont, 0);
  ctrl_set_rstyle(p_nextbtntxet, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_nextbtntxet, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
  text_set_align_type(p_nextbtntxet, STL_LEFT| STL_VCENTER);
  text_set_content_type(p_nextbtntxet, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_nextbtntxet, IDS_NEXT_PAGE);
  ctrl_process_msg(p_nextbtn, MSG_GETFOCUS, 0, 0); 
  g_boot.hTitle = p_title;
  g_boot.hBmp = p_bmp;
  g_boot.hBoxcon = p_box_con;
  g_boot.hBoxbtn = p_box_btn;
  g_boot.hBoxbtntxet =p_box_txet;
  g_boot.hMain = p_cont;
  g_boot.hNextbtn = p_nextbtn;
  g_boot.hNextbtntxet = p_nextbtntxet;
  g_boot.hDisp = FALSE;
  g_boot.hPage = 1;
  return p_cont;
}
/*
static RET_CODE xbootWizard_show_dlg(s16 x,s16 y,s16 w,s16 h,s16 time,u16 text)
{
	rect_t rec;
	rec.left = x;
	rec.right = x+w;
	rec.top = y;
	rec.bottom = y+h;
	ui_comm_cfmdlg_open(&rec, text, NULL, time);	
	return SUCCESS;
}
*/
static void on_xbootWizard_copy_tp(dvbs_tp_node_t *tp1,dvbc_lock_t tp2)
{
        tp1->freq = tp2.tp_freq;
        tp1->sym = tp2.tp_sym;
        tp1->nim_modulate = tp2.nim_modulate;
}
static void on_xbootWizard_auto_search(void)
{
        ui_scan_param_t scan_param;
        dvbc_lock_t tp = {0};
	memset(&scan_param, 0, sizeof(ui_scan_param_t));
        sys_status_get_main_tp1(&tp);  
        on_xbootWizard_copy_tp(&(scan_param.tp),tp);
#ifdef AISET_BOUQUET_SUPPORT
        scan_param.nit_type = NIT_SCAN_ALL_TP;
#else
        scan_param.nit_type = NIT_SCAN_ONCE;
#endif
        DEBUG(DBG, INFO, "\n");
        manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_AUTO, (u32)&scan_param);
}

static void on_xbootWizard_open_menu(void)
{
        DEBUG(DBG, INFO, "\n");
        manage_open_menu(ROOT_ID_MAINMENU, 0, 0);
}

/*the exit function*/
static RET_CODE on_xbootWizard_exit()
{
    u8 curn_mode = CURN_MODE_NONE;

    rect_t restore_dlg_rc =
    {
    	XBOOT_WIZARD_ASK_L,
    	XBOOT_WIZARD_ASK_T,
    	XBOOT_WIZARD_ASK_R,
    	XBOOT_WIZARD_ASK_B,
    };

  curn_mode = sys_status_get_curn_prog_mode();
  if(curn_mode == CURN_MODE_NONE ) 
    {
            fw_destroy_mainwin_by_id(ROOT_ID_XBOOT_WIZARD);
            ui_comm_ask_for_dodlg_open(&restore_dlg_rc,
            IDS_A_KEY_SEARCH_TITLE,
            on_xbootWizard_auto_search, on_xbootWizard_open_menu, 0);
    }
  else
      manage_close_menu(ROOT_ID_XBOOT_WIZARD, 0, 0);

  return SUCCESS;
}

static RET_CODE on_xbootWizard_update_page(u8 index)
{
  switch(index)
  {
  case 1:
        text_set_content_by_strid(g_boot.hTitle, IDS_PAGE1_TITLE);
        bmap_set_content_by_id(g_boot.hBmp, IM_ARROW1_L);
        if(ctrl_get_sts(g_boot.hBoxcon) == OBJ_STS_SHOW)
            ctrl_set_sts(g_boot.hBoxcon, OBJ_STS_HIDE);
        if(ctrl_get_sts(g_boot.hBoxbtntxet) == OBJ_STS_SHOW)
            ctrl_set_sts(g_boot.hBoxbtntxet, OBJ_STS_HIDE);
        text_set_content_by_strid(g_boot.hNextbtntxet, IDS_NEXT_PAGE);
    break;
  case 2:
        text_set_content_by_strid(g_boot.hTitle, IDS_PAGE2_TITLE);
        bmap_set_content_by_id(g_boot.hBmp, IM_ARROW1_L);
        if(ctrl_get_sts(g_boot.hBoxcon) == OBJ_STS_SHOW)
            ctrl_set_sts(g_boot.hBoxcon, OBJ_STS_HIDE);
        if(ctrl_get_sts(g_boot.hBoxbtntxet) == OBJ_STS_SHOW)
            ctrl_set_sts(g_boot.hBoxbtntxet, OBJ_STS_HIDE);
        text_set_content_by_strid(g_boot.hNextbtntxet, IDS_NEXT_PAGE);
    break;
  case 3:
        text_set_content_by_strid(g_boot.hTitle, IDS_PAGE3_TITLE);
        bmap_set_content_by_id(g_boot.hBmp, IM_ARROW1_L);
        if(ctrl_get_sts(g_boot.hBoxcon) == OBJ_STS_HIDE)
            ctrl_set_sts(g_boot.hBoxcon, OBJ_STS_SHOW);
        if(ctrl_get_sts(g_boot.hBoxbtntxet) == OBJ_STS_HIDE)
            ctrl_set_sts(g_boot.hBoxbtntxet, OBJ_STS_SHOW);
        text_set_content_by_strid(g_boot.hNextbtntxet, IDS_MSG_FINISHED);
        //ctrl_set_rstyle(g_boot.hMain,RSI_BOOT_PAGE3_BG,RSI_BOOT_PAGE3_BG,RSI_BOOT_PAGE3_BG);
    break;
  default:
    break;
  }
  ctrl_paint_ctrl(g_boot.hMain, TRUE);
  return SUCCESS;
}

RET_CODE open_xbootWizard(u32 para1, u32 para2)
{
  control_t *p_menu = NULL;
  p_menu = create_xbootWizard_menu();
  MT_ASSERT(p_menu != NULL);
  ctrl_paint_ctrl(p_menu, FALSE);

  return SUCCESS;
}

static RET_CODE xboot_wizard_focus_change(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	 control_t *p_next;
        if(g_boot.hPage == 3)
        {
            if(ctrl_get_ctrl_id(p_ctrl) == IDC_XBOOT_WIZARD_NEXTBTN)
                p_next = g_boot.hBoxcon;
            else
                p_next = g_boot.hNextbtn;
            ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0); 
            ctrl_process_msg(p_next, MSG_GETFOCUS, 0, 0); 
            ctrl_paint_ctrl(p_ctrl,FALSE);
            ctrl_paint_ctrl(p_next,FALSE);
        }
	return SUCCESS;
}

static RET_CODE xboot_wizard_select(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
        u16 need_disp = 1;/* 1: show boot wizard .2:hide boot wizard*/
        if(ctrl_get_ctrl_id(p_ctrl) == IDC_XBOOT_WIZARD_NEXTBTN)
        {
            g_boot.hPage++;
            if(g_boot.hPage < 4)
            {
                on_xbootWizard_update_page(g_boot.hPage);
            }else
            {
                need_disp = g_boot.hDisp ? 2:1;
                sys_status_set_boot_wizard(&need_disp);
                sys_status_save();
                on_xbootWizard_exit();
            }
          }else
            {
                if(g_boot.hDisp)
                    {
                    bmap_set_content_by_id(g_boot.hBoxbtn, IM_OPENDVB_CTRLBOX_SH);
                    g_boot.hDisp = FALSE;
                }else
                    {
                    bmap_set_content_by_id(g_boot.hBoxbtn, IM_OPENDVB_CTRLBOX_HL);
                    g_boot.hDisp = TRUE;
                }
                ctrl_paint_ctrl(g_boot.hBoxbtn, TRUE);
        }
	return SUCCESS;
}


BEGIN_KEYMAP(xboot_wizard_box_keymap, NULL)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
	ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
	ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(xboot_wizard_box_keymap, NULL)

BEGIN_MSGPROC(xboot_wizard_box_proc, cont_class_proc)
	ON_COMMAND(MSG_SELECT, xboot_wizard_select)
	ON_COMMAND(MSG_FOCUS_LEFT, xboot_wizard_focus_change)
	ON_COMMAND(MSG_FOCUS_RIGHT, xboot_wizard_focus_change)
END_MSGPROC(xboot_wizard_box_proc, cont_class_proc)

BEGIN_KEYMAP(xboot_wizard_btn_keymap, NULL)
	ON_EVENT(V_KEY_OK, MSG_SELECT)
	ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
	ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(xboot_wizard_btn_keymap, NULL)

BEGIN_MSGPROC(xboot_wizard_btn_proc, text_class_proc)
	ON_COMMAND(MSG_SELECT, xboot_wizard_select)
	ON_COMMAND(MSG_FOCUS_LEFT, xboot_wizard_focus_change)
	ON_COMMAND(MSG_FOCUS_RIGHT, xboot_wizard_focus_change)
END_MSGPROC(xboot_wizard_btn_proc, text_class_proc)

