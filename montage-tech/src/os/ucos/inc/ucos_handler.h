/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#ifndef __UCOS_HANDLER_H_
#define __UCOS_HANDLER_H_
/*!
  os_memory_init
  */
void os_memory_init(void);

/*!
  os_task_init
  */
void os_task_init(void);
/*!
  os_msg_init
  */
void os_msg_init(void);
/*!
 os_sem_init
  */
void os_sem_init(void);
/*!
  os_timer_init
  */
void os_timer_init(void);
/*!
  os_misc_init
  */
void os_misc_init(void);
/*!
 os_irq_handler_init
  */
void os_irq_handler_init(mtos_cfg_t *p_cfg);
/*!
 os event handler init
  */
void ucos_event_init(void);

/*!
 os mutex handler init
  */
void os_mutex_init(void);
#endif

