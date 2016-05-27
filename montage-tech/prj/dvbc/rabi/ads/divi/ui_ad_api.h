/********************************************************************************************/
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/* Montage Proprietary and Confidential                                                     */
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/********************************************************************************************/
#ifndef __UI_AD_API_H__
#define __UI_AD_API_H__

#define OSD_ADS_TOP		           	(40)
#define OSD_ADS_LEFT          		(0)
#define OSD_ADS_WIDTH             	(1280 - OSD_ADS_LEFT)
#define OSD_ADS_HEIGHT            	(80)

typedef enum{
	IDN_ADS_INVALID =0,
	IDN_ADS_LOGO =1,
	IDN_ADS_PROG_BAR =2,
	IDN_ADS_VOLUME =3,
	IDN_ADS_CORNER =4,
	IDN_ADS_MAINMENU =5,
	IDN_ADS_PROG_LIST =6,
	IDN_ADS_PIC_ROLLING =7,
	IDN_ADS_TEXT_ROLLING =8,
	IDN_ADS_PROGR_FILTER =9,
	IDN_ADS_PIC_ROLLING_OVER =16,
}divi_ads_t;


typedef RET_CODE (*adv_nofity) (ads_ad_type_t type, void *param);

void ui_adv_set_adm_id(u32 adm_id);

u32 ui_adv_get_adm_id(void);

void ui_adv_pic_init(pic_source_t src);

BOOL ui_adv_pic_play(ads_ad_type_t type, u8 root_id);

BOOL ui_adv_get_pic_rect(rect_t *rect_region);

void pic_adv_stop(void);

void divi_show_logo(void);

void ui_adv_pic_release(void);

#endif

