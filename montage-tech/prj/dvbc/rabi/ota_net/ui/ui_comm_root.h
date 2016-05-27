/****************************************************************************

 ****************************************************************************/
#ifndef __UI_COMM_BG_H__
#define __UI_COMM_BG_H__

/*********************************************************************
 * COMMON DEFINATION
 *********************************************************************/
#define IDC_COMM_TITLE_CONT     0xFE   /* special for title */
#define IDC_COMM_ROOT_CONT      0xFD   /* special for signbar */
#define IDC_COMM_SIGN_CONT      0xFC   /* special for signbar */
#define IDC_COMM_HELP_CONT      0xFB   /* special for signbar */

#define IDC_COMM_TITLE_ICON      0xFA   /* special for title icon */

#define IDC_COMM_TITLE_TEXT      0xF9   /* special for title text */




/*********************************************************************
 * COMMON COORDINATE
 *********************************************************************/
#define COMM_BG_X               (0)
#define COMM_BG_Y               (0)
#define COMM_BG_W               SCREEN_WIDTH
#define COMM_BG_H               SCREEN_HEIGHT
#define COMM_ITEM_OX_IN_ROOT    120//140
#define COMM_ITEM_OY_IN_ROOT    100
#define COMM_ITEM_LW            430//400
#define COMM_ITEM_H             COMM_CTRL_H
#define COMM_ITEM_MAX_WIDTH     (COMM_BG_W - 2 * COMM_ITEM_OX_IN_ROOT)
#define COMM_SBAR_VERTEX_GAP    12
#define PREV_OFFSET_X           0//160
#define PREV_OFFSET_Y           0//64
#define PREV_LR_W         6
#define PREV_TB_H         6
/*********************************************************************
 * COMMON TITLE
 *********************************************************************/
/* coordinate */
#define TITLE_W                 1280
#define TITLE_H                 60

#define TITLE_ICON_X             40
#define TITLE_ICON_Y             0
#define TITLE_ICON_W             70
#define TITLE_ICON_H             TITLE_H

#define TITLE_TXT_X             140
#define TITLE_TXT_Y             30
#define TITLE_TXT_W             300
#define TITLE_TXT_H             30



#define FSI_TITLE_TXT           FSI_WHITE
#define FSI_TITLE_USB_TXT           FSI_RED


void ui_comm_title_create(control_t *parent, u16 icon_id, u16 strid, u8 rstyle);

void ui_comm_title_create_OTT(control_t *parent, u16 icon_id, u8 *title_asc, u16 *title_unistr, u16 title_unistr_len, u8 rstyle);

void ui_comm_title_create_usb(control_t *parent, u16 icon_id, u16 strid1, u16 strid2, u16 strid3, u16 strid4,u16 focus,u8 rstyle);


void ui_comm_title_set_content(control_t *parent, u16 strid);

void ui_comm_title_update(control_t *parent);

/****************************************************
 * COMMON HELP BAR
 ****************************************************/

/* coordinate */
#define HELP_CONT_H_GAP      (0)//90//200
#define HELP_CONT_V_GAP      (0)//40

#define HELP_CONT_POP_DLG_H_GAP      (10)//16
#define HELP_CONT_POP_DLG_V_GAP      (16)

#define HELP_ITEM_H_GAP      16
#define HELP_ITEM_V_GAP      0
#define HELP_ITEM_MAX_CNT    10
#define HELP_ITEM_H          30

#define HELP_ITEM_ICON_W     (43)//(38)
#define HELP_ITEM_ICON_OX    0
#define HELP_ITEM_ICON_OY    0

#define HELP_ITEM_TEXT_OX    HELP_ITEM_ICON_W
#define HELP_ITEM_TEXT_OY    0

/* rect style */
#define RSI_HELP_CNT         RSI_PBACK

/* font style */
#define FSI_HELP_TEXT        FSI_WHITE
#define FSI_KEYBOARD_ITEM_HL        FSI_BLACK
#define FSI_KEYBOARD_ITEM_SH        FSI_GRAY



typedef struct
{
  u8 item_cnt;                        //item number
  u8 col_cnt;                         //item number per row
  u16 str_id[HELP_ITEM_MAX_CNT];      //string id
  u16 bmp_id[HELP_ITEM_MAX_CNT];      //bmp id
}comm_help_data_t;

void ui_comm_help_create(comm_help_data_t *p_data, control_t *p_parent);

void ui_comm_help_create_ext(u16 x,
                               u16 y,
                               u16 w,
                               u16 h,
                               comm_help_data_t *p_data,
                               control_t *p_parent);

void ui_comm_help_move_pos(control_t *p_parent, u16 x, u16 offset_top,
                           u16 w, u16 offset_bottom, u16 str_offset);

control_t *ui_comm_help_create_for_pop_dlg(comm_help_data_t *p_data, control_t *p_parent);

void ui_comm_help_set_data(comm_help_data_t *p_data, control_t *p_parent);

void ui_comm_help_update(control_t *p_parent);

void ui_comm_help_set_font(control_t *p_parent,
                                 u32 n_fstyle,
                                 u32 h_fstyle,
                                 u32 g_fstyle);

/*********************************************************************
 * COMMON ROOT
 *********************************************************************/

/* coordinate */
#define ROOT_SIGN_BAR_CONT_X         (120)
#define ROOT_SIGN_BAR_CONT_W         (1040)
#define ROOT_SIGN_CONT_W             (900)
#define ROOT_SIGN_CONT_H             (72)
//#define ROOT_SIGN_CONT_H           (72)//160//130
#define ROOT_SIGN_CONT_GAP           (0)//4

#define ROOT_SIGN_BAR_STXT_X         (22)//(10)//50
#define ROOT_SIGN_BAR_STXT_Y         (0)
#define ROOT_SIGN_BAR_STXT_W         (210)//200
#define ROOT_SIGN_BAR_STXT_H         (36)//30

#define ROOT_SIGN_BAR_X              (ROOT_SIGN_BAR_STXT_X + ROOT_SIGN_BAR_STXT_W)
#define ROOT_SIGN_BAR_Y              (ROOT_SIGN_BAR_STXT_Y +  ((ROOT_SIGN_BAR_STXT_H - ROOT_SIGN_BAR_H) >> 1))
//#define ROOT_SIGN_BAR_W                (COMM_BG_W - 2 * ROOT_SIGN_BAR_STXT_X - ROOT_SIGN_BAR_STXT_W - ROOT_SIGN_BAR_PERCENT_W)
#define ROOT_SIGN_BAR_W              (ROOT_SIGN_CONT_W - ROOT_SIGN_BAR_X - ROOT_SIGN_BAR_PERCENT_W)
#define ROOT_SIGN_BAR_H              (16)

#define ROOT_SIGN_BAR_PERCENT_X      (ROOT_SIGN_BAR_X + ROOT_SIGN_BAR_W)
#define ROOT_SIGN_BAR_PERCENT_Y      ROOT_SIGN_BAR_STXT_Y
#define ROOT_SIGN_BAR_PERCENT_W      (70)
#define ROOT_SIGN_BAR_PERCENT_H      ROOT_SIGN_BAR_STXT_H

#define ROOT_SIGN_CTRL_V_GAP         0

/* rect style */
#define RSI_ROOT_SIGN_BAR            RSI_PROGRESS_BAR_BG
#define RSI_ROOT_SIGN_BAR_MID_1      RSI_PROGRESS_BAR_MID_RED
//#define RSI_ROOT_SIGN_BAR_MID_2      RSI_PROGRESS_BAR_MID_SKY_BLUE

#define RSI_ROOT_SIGN_BAR_STXT       RSI_PBACK
#define RSI_ROOT_SIGN_BAR_PERCENT    RSI_PBACK

/* font style */
#define FSI_ROOT_SIGN_BAR_STXT       FSI_WHITE
#define FSI_ROOT_SIGN_BAR_PERCENT    FSI_WHITE

/* others */
#define BEEPER_UNLKD_TMOUT           2000
#define BEEPER_LOCKD_TMOUT           1000

control_t *ui_comm_root_create(u8 root_id,
                               u8 parent_root,
                               u16 x,
                               u16 y,
                               u16 w,
                               u16 h,
                               u16 title_icon,
                               u16 title_strid);

control_t *ui_comm_root_create_fullbg(u8 root_id,
                               u8 rsi_root,
                               u16 x,
                               u16 y,
                               u16 w,
                               u16 h,
                               u16 title_icon,
                               u16 title_strid);

control_t *ui_comm_root_create_with_signbar(u8 root_id,
                                            u8 parent_root,
                                            u16 x,
                                            u16 y,
                                            u16 w,
                                            u16 h,
                                            u16 title_icon,
                                            u16 title_strid,
                                            BOOL is_beeper);

control_t *ui_comm_root_signalbar_create(u16 x,
                                            u16 y,
                                            u16 w,
                                            u16 h,
                                            control_t *p_parent,
                                            BOOL is_beeper);

control_t *ui_comm_root_create_with_signbar_fullbg(u8 root_id,
                                            u8 parent_root,
                                            u16 x,
                                            u16 y,
                                            u16 w,
                                            u16 h,
                                            u16 title_icon,
                                            u16 title_strid,
                                            BOOL is_beeper);

control_t *ui_comm_root_create_usb(u8 root_id,
                               u8 parent_root,
                               u16 x,
                               u16 y,
                               u16 w,
                               u16 h,
                               u16 title_icon,
                               u16 title_strid1, u16 title_strid2, u16 title_strid3, u16 title_strid4,u16 focus);

void ui_comm_root_update_signbar(control_t *root,
                                 u8 intensity,
                                 u8 quality,
                                 BOOL is_lock);

control_t *ui_comm_root_get_ctrl(u8 root_id, u8 ctrl_id);

control_t *ui_comm_root_get_root(u8 root_id);

u16 ui_comm_root_keymap(u16 key);

RET_CODE ui_comm_root_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
/*********************************************************************
 * COMMON MENU
 *********************************************************************/

/* coordinate */
#define MENU_X             ((SCREEN_WIDTH - MENU_W) / 2)
#define MENU_Y             COMM_BG_Y
#define MENU_W             260
#define MENU_BORDER        10

#define MENU_ITEM_CNT      7
#define MENU_ITEM_X        ((MENU_W - MENU_ITEM_W) / 2)
#define MENU_ITEM_Y        40
#define MENU_ITEM_W        240
#define MENU_ITEM_H        34
#define MENU_ITEM_V_GAP    6

typedef struct
{
  u8 id;
  u8 item_cnt;                       //item count
  u16 title_strid;                   //title string id
  msgproc_t proc;
  u16 item_strid[MENU_ITEM_CNT];     //string id
}comm_menu_data_t;

enum comm_title_idc
{
  IDC_COMM_TITLE_TXT = 1,
  IDC_COMM_TITLE_IC = 2,
  IDC_COMM_TITLE_TXT_USB1,
  IDC_COMM_TITLE_TXT_USB2,
  IDC_COMM_TITLE_TXT_USB3,
  IDC_COMM_TITLE_TXT_USB4,
};

enum comm_help_idc
{
  IDC_COMM_HELP_MBOX = 1,
};

control_t *ui_comm_menu_create(comm_menu_data_t *p_data, u32 para);

u8 ui_comm_get_epg_root(void);
void ui_comm_set_epg_root(u8 root_id);

void ui_comm_help_create(comm_help_data_t *p_data, control_t *p_parent);

control_t *ui_comm_help_create_for_pop_dlg(comm_help_data_t *p_data, control_t *p_parent);

void ui_comm_help_create_ext(u16 x,
                               u16 y,
                               u16 w,
                               u16 h,
                               comm_help_data_t *p_data,
                               control_t *p_parent);
void ui_comm_help_move_pos(control_t *p_parent, u16 x, u16 offset_top,
                           u16 w, u16 offset_bottom, u16 str_offset);

control_t *ui_comm_get_focus_mainwin(void);
u8 ui_comm_get_focus_mainwin_id(void);

#endif

