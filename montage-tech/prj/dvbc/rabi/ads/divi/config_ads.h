/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __ADS_FUNTION_PUBLIC_H__
#define __ADS_FUNTION_PUBLIC_H__

#ifdef ENABLE_ADS

//use cas id temp for ads
#define CONFIG_ADS_ID  CONFIG_CAS_ID_DIVI 

void ads_mem_init(u32 mem_start,u32 size);
void ads_ap_init(void);
#endif

#endif
