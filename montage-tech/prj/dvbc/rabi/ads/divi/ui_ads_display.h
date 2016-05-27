/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef _UI_ADS_DISPLAY_H
#define _UI_ADS_DISPLAY_H
#ifdef ENABLE_ADS
#include "ads_ware.h"
#include "sys_types.h"
#include "ui_ad_gif_api.h"

enum background_ads_roll_ctrl_id
{
  IDC_ADS_ROLL_TOP = 20,
  IDC_ADS_ROLL_MIDDLE = 21,
  IDC_ADS_ROLL_BOTTOM = 22,
};

void ui_ads_osd_roll_stop(void);
void ui_ads_osd_roll(void);

RET_CODE ui_adv_xsm_display(ads_ad_type_t ad_type, u8 *p_data);
RET_CODE ui_adv_xsm_hide(ads_ad_type_t ad_type, u8 *p_data);

void ui_adv_get_osd_msg(u16 pg_id);
void ads_xsm_set_type(ads_ad_type_t type, u32 * cmd, ads_module_cfg_t * p_cfg);
RET_CODE on_ads_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE on_ads_time(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

void on_ads_start_roll_osd(u8 pos, u8 *p_data);
RET_CODE create_ads_rolling_ctrl(void);
BOOL divi_ads_filter_for_pg(u16 s_id);

void divi_ads_pic_rolling_monitor(void *p_data);
void ui_set_ads_osd_hide(BOOL status);
BOOL ui_get_ads_osd_roll_over(void);

void ui_ads_osd_init_param(void);
void divi_ads_update_osd_show(BOOL direction);
u8 ui_get_ads_update_osd_state(void);
void divi_ads_show_fs(void);
void ui_set_osd_rolling_state(BOOL state);
void divi_ads_show_init(void);

#endif

#endif
