/****************************************************************************
 * Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
 *              All rights reserved.
 ****************************************************************************/
#ifndef __UI_CONDITIONAL_ENTITLE_EXPIRE__
#define __UI_CONDITIONAL_ENTITLE_EXPIRE__

#define ENTITLE_CONT_FULL_X  ((SCREEN_WIDTH-ENTITLE_CONT_FULL_W_DIVI)/2)
#define ENTITLE_CONT_FULL_Y  ((SCREEN_HEIGHT-ENTITLE_CONT_FULL_H_DIVI)/2 -20)
#define ENTITLE_CONT_FULL_W_DIVI  350
#define ENTITLE_CONT_FULL_H_DIVI  100

#define ENTITLE_TITLE_FULL_X  0
#define ENTITLE_TITLE_FULL_Y  0
#define ENTITLE_TITLE_FULL_W  ENTITLE_CONT_FULL_W_DIVI
#define ENTITLE_TITLE_FULL_H  40

#define ENTITLE_CONTENT_FULL_X  10
#define ENTITLE_CONTENT_FULL_Y  (ENTITLE_TITLE_FULL_Y+ENTITLE_TITLE_FULL_H+5)

#define ENTITLE_CONTENT_FULL_W_DIVI  (ENTITLE_CONT_FULL_W_DIVI - 2*ENTITLE_CONTENT_FULL_X)
#define ENTITLE_CONTENT_FULL_H_DIVI  50



#define ENTITLE_CONT_FULL_W_MG  (420)
#define ENTITLE_CONT_FULL_H_MG  (140)
#define ENTITLE_CONTENT_FULL_W_MG  (ENTITLE_CONT_FULL_W_MG - 2*ENTITLE_CONTENT_FULL_X)
#define ENTITLE_CONTENT_FULL_H_MG  (ENTITLE_CONT_FULL_H_MG - ENTITLE_CONTENT_FULL_Y)


RET_CODE open_ca_entitle_expire_dlg(u32 para1, u32 para2);
void close_ca_entitle_expire_dlg(void);

#endif
