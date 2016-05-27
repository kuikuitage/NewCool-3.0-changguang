/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UI_CA_PUBLIC_H__
#define __UI_CA_PUBLIC_H__

typedef enum
{
  ROOT_ID_CONDITIONAL_ACCEPT = ROOT_ID_CA_START,
  ROOT_ID_NEW_MAIL,
  ROOT_ID_EMAIL_MESS,
  ROOT_ID_CA_PROMPT,
  ROOT_ID_CA_CARD_INFO,

  ROOT_ID_CA_PAIR,
  ROOT_ID_CA_MOTHER,
  ROOT_ID_CA_PPV_ICON,
  ROOT_ID_CA_PRE_AUTH,
  ROOT_ID_CA_BURSE_CHARGE,
  ROOT_ID_CA_IPPV_PPT_DLG,
  ROOT_ID_CA_ENTITLE_EXP_DLG,
  ROOT_ID_CA_SUPER_OSD,
  ROOT_ID_CA_CARD_UPDATE,
  ROOT_ID_FINGER_PRINT,
}ca_ui_root_id;

typedef struct
{
u32 index;  //index
u32 message_type;  // 0:mail  1:announce 2:alert mail 3:alert announce
}prompt_type_t;

enum background_menu_ctrl_id
{
  /* value 1 for TITLE_ROLL*/
  IDC_BG_MENU_CA_ROLL_TOP = 2,
  IDC_BG_MENU_CA_ROLL_BOTTOM = 3,
  IDC_BG_MENU_CA_SENIOR_PREVIEW = 6,
  IDC_BG_MENU_CA_ROLL_FULLSCREEN = 12,
  IDC_BG_MENU_CA_ROLL_HELP = 13,
};

typedef enum OSD_SHOW_POS
{
	OSD_SHOW_TOP = 0,					//在顶部显示
	OSD_SHOW_BOTTOM,					//在底部显示
	OSD_SHOW_TOP_BOTTOM,				//在顶部和底部同时显示
}OSD_SHOW_POS;

void ui_clear_mgca_tip(void);
RET_CODE ui_desktop_proc_cas(control_t *p_ctrl, u16 msg,  u32 para1, u32 para2);
u16 ui_desktop_keymap_cas(u16 key);
void ui_set_smart_card_insert(BOOL status);
BOOL ui_is_smart_card_insert(void);
RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,u32 para1, u32 para2);
void load_desc_paramter_by_pgid(cas_sid_t *p_info, u16 pg_id);
RET_CODE on_ca_init_ok(control_t *p_ctrl, u16 msg,u32 para1, u32 para2);

RET_CODE ca_private_msg_func(control_t *p_ctrl, u16 msg,u32 para1, u32 para2);

RET_CODE open_ca_rolling_menu(u32 para1, u32 para2);
RET_CODE on_ca_rolling_show(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE on_ca_rolling_hide(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE on_ca_authen_message_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
extern void set_uio_status(BOOL is_disable);
#endif

