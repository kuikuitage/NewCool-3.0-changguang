/****************************************************************************

 ****************************************************************************/
#ifndef __UI_UTIL_API_H__
#define __UI_UTIL_API_H__

#ifdef ENABLE_ADS
typedef enum
{
  PIC_TYPE_MAIN_MENU = 0,
  PIC_TYPE_SUB_MENU
} ads_type_t;

u8* get_pic_buf_addr(u32* p_len);
void ui_show_logo_bg(ads_type_t ads_type , rect_t *rect);
void ui_close_logo_bg(void);

#endif


void ui_enable_uio(BOOL is_enable);

void ui_set_front_panel_by_str(const char * str);

void ui_set_front_panel_by_str_with_upg(const char *str_flag,u8 process);

void ui_set_front_panel_by_signal_num(u16 num);

void ui_set_front_panel_by_num(u16 num);

void ui_show_logo(u8 block_id);

void ui_enable_video_display(BOOL is_enable);

void ui_config_ttx_osd(void);

void ui_config_normal_osd(void);

void ui_conver_file_size_unit_bytes(u32 bytes, u8 *p_str);

void ui_conver_file_size_unit(u32 k_size, u8 *p_str);

void ui_set_com_num(u8 num);

void ui_set_net_svc_instance(void *p_instance);

void * ui_get_net_svc_instance(void);

void ui_set_t2_exist(BOOL b_exist);
BOOL ui_get_t2_exist();
u8 nim_get_tuner_count(void);

void * ui_get_usb_eth_dev(void);

enum
{
    SINGAL_DVBS,
    SINGAL_DVBT2,
    SINGAL_DTMB,
    DUAL_DVBS_DVBS,
    DUAL_DVBS_DVBT,
}dual_tuner_mode_t;
#endif

