/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#ifndef __CONFIG_CAS_H__
#define __CONFIG_CAS_H__

#define AP_CAS_ID   CAS_ID_ADT_MG
#define CONFIG_CAS_ID   CONFIG_CAS_ID_ADT_MG
//#define INVALID_PG_ID 0xFFFF

void on_cas_init(void);
RET_CODE machine_serial_get(u8 *p_buf, u32 size);

#endif


