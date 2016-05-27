/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CONDITIONAL_ACCEPT_PIN_H__
#define __UI_CONDITIONAL_ACCEPT_PIN_H__

  #define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_CNT           (4)
  #define CONDITIONAL_ACCEPT_PIN_HELP_RSC_CNT	    (15)
  //CA frm
  #define CONDITIONAL_ACCEPT_PIN_CA_FRM_X          (0)
  #define CONDITIONAL_ACCEPT_PIN_CA_FRM_Y          (0)
  #define CONDITIONAL_ACCEPT_PIN_CA_FRM_W         (RIGHT_ROOT_W)
  #define CONDITIONAL_ACCEPT_PIN_CA_FRM_H          (RIGHT_ROOT_H)

  //pin modify
  #define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_X         (COMM_ITEM_OX_IN_ROOT/2)
  #define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_Y         (COMM_ITEM_OY_IN_ROOT/2)
  #define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_LW       (COMM_ITEM_LW)
  #define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_RW       (RIGHT_ROOT_W - COMM_ITEM_LW - COMM_ITEM_OX_IN_ROOT)
  #define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_H         (45)
  #define CONDITIONAL_ACCEPT_PIN_PIN_ITEM_V_GAP  (20)

  #define CONDITIONAL_ACCEPT_PIN_OK_ITEM_X           (250)
  #define CONDITIONAL_ACCEPT_PIN_OK_ITEM_W          (160)
  #define CONDITIONAL_ACCEPT_PIN_OK_ITEM_H           (COMM_CTRL_H)

  //change pin result
  #define CONDITIONAL_ACCEPT_CHANGE_RESULT_X         (COMM_ITEM_OX_IN_ROOT/2)
  #define CONDITIONAL_ACCEPT_CHANGE_RESULT_Y         (360)
  #define CONDITIONAL_ACCEPT_CHANGE_RESULT_W        (600)
  #define CONDITIONAL_ACCEPT_CHANGE_RESULT_H         (COMM_CTRL_H)

/* rect style */
/* font style */
/* others */
RET_CODE open_conditional_accept_pin(u32 para1, u32 para2);

#endif


