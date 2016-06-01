/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 * Owner:       Kevin Li <kai.li@montage-tech.com>
 *              Mark Wu <mark.wu@montage-tech.com>
 ****************************************************************************/
#ifndef __UI_CA_PUBLIC_H__
#define __UI_CA_PUBLIC_H__

#define GET_ENTITLE_COUNT  (10)

//Mg Prompt TEMP value
#define IDS_MG_CARD_NOT_READY 100
#define IDS_MG_CA_E_UPDATE_GK 100

typedef enum
{
  ROOT_ID_CONDITIONAL_ACCEPT = ROOT_ID_CA_START,
  ROOT_ID_EMAIL_MESS,
  ROOT_ID_EMAIL_MESS_CONTENT,
  ROOT_ID_NEW_MAIL,
  ROOT_ID_CA_PROMPT,
  ROOT_ID_CA_CARD_INFO,
  
  ROOT_ID_CONDITIONAL_ACCEPT_LEVEL,
  ROOT_ID_CONDITIONAL_ACCEPT_WORKTIME,
  ROOT_ID_CA_ENTITLE_INFO,
  ROOT_ID_CONDITIONAL_ACCEPT_PIN,
  ROOT_ID_CONDITIONAL_ACCEPT_ANTIAUTH,
  ROOT_ID_CA_FEED,
  ROOT_ID_CA_IPPV_PPT_DLG,
  ROOT_ID_CA_CARD_UPDATE,
  ROOT_ID_FINGER_PRINT,
  ROOT_ID_CA_ENTITLE_EXP_DLG,
	//mg
  ROOT_ID_CA_PAIR,
  ROOT_ID_CA_MOTHER,
  ROOT_ID_CA_PPV_ICON,
  ROOT_ID_CA_PRE_AUTH,
  ROOT_ID_CA_BURSE_CHARGE,
  ROOT_ID_CA_SUPER_OSD,
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


RET_CODE open_ca_rolling_menu(u32 para1, u32 para2);
BOOL get_card_reset_finsh(void);
BOOL get_program_entitlement_from_ca(void);
BOOL ui_is_need_clear_display(void);
u32 ui_get_ca_last_msg(void);
void ui_set_smart_card_insert(BOOL status);
BOOL ui_is_smart_card_insert(void);
BOOL ui_get_smart_card_state(void);
u32 ui_get_smart_card_rate(void);
void ui_set_smart_card_rate(u32 level);
RET_CODE on_conditional_accept_level_update_d(control_t *p_cont, u16 msg, u32 para1, u32 para2);

RET_CODE on_ca_message_update(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2);

void load_desc_paramter_by_pgid(cas_sid_t *p_info, u16 pg_id);

void on_ca_zone_check_start(u32 cmd_id, u32 para1, u32 para2);
void on_ca_zone_check_stop(void);
void on_ca_zone_check_end(void);
void ui_cas_factory_set(u32 cmd_id, u32 para1, u32 para2);
RET_CODE ui_desktop_proc_cas(control_t *p_ctrl, u16 msg,
                             u32 para1, u32 para2);
u16 ui_desktop_keymap_cas(u16 key);
RET_CODE open_conditional_accept_feed(u32 para1, u32 para2);
RET_CODE open_conditional_accept_worktime(u32 para1, u32 para2);
RET_CODE open_conditional_accept_pin(u32 para1, u32 para2);
RET_CODE open_conditional_accept_pair(u32 para1, u32 para2);
RET_CODE open_conditional_accept_level(u32 para1, u32 para2);
RET_CODE open_conditional_accept_info(u32 para1, u32 para2);
RET_CODE open_ca_rolling_menue(u32 para1, u32 para2);

BOOL ui_is_finger_show(void);
void redraw_finger(void);
void on_desktop_start_roll_position(u16 *uni_str, u8 *display_back_color, u8 *display_color, u8 *display_front_size, u8 *display_pos);
BOOL osd_roll_stop_msg_unsend(void);
BOOL get_force_channel_status(void);
RET_CODE on_ca_rolling_show(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE on_ca_rolling_hide(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
BOOL get_ca_osd_hide_state(void);
BOOL freq_is_valid(u32 freq);
void update_have_valid_freq_flag(void);
BOOL get_have_valid_freq_flag(void);
void set_have_valid_freq_flag(BOOL flag);


//mg
void ui_clear_mgca_tip(void);
void load_desc_paramter_by_pgid(cas_sid_t *p_info, u16 pg_id);

RET_CODE ca_private_msg_func(control_t *p_ctrl, u16 msg,u32 para1, u32 para2);

RET_CODE on_ca_authen_message_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
extern void set_uio_status(BOOL is_disable);

#endif

