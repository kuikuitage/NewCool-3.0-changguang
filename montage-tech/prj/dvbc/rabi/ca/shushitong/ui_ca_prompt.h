/****************************************************************************
* Notice:      Copyright (c) Montage Techonoloy Corporation (2009).
*              All rights reserved.
* Owner:       Kevin Li <kai.li@montage-tech.com>
*              Mark Wu <mark.wu@montage-tech.com>
****************************************************************************/
#ifndef __UI_CA_PROMPT_H__
#define __UI_CA_PROMPT_H__

  //frm
#define CA_PROMPT_FRM_X             (0)
#define CA_PROMPT_FRM_Y             (0)
#define CA_PROMPT_FRM_W            (SYS_RIGHT_CONT_W)
#define CA_PROMPT_FRM_H             (SYS_RIGHT_CONT_H)

//title
#define CA_PROMPT_TITLE_X             (30)
#define CA_PROMPT_TITLE_Y             (0)
#define CA_PROMPT_TITLE_W             (CA_PROMPT_FRM_W - CA_PROMPT_TITLE_X - 60)
#define CA_PROMPT_TITLE_H             (36)

//content
#define CA_PROMPT_CONTERNT_X             (30)
#define CA_PROMPT_CONTERNT_Y             (CA_PROMPT_TITLE_Y + CA_PROMPT_TITLE_H + 10)
#define CA_PROMPT_CONTERNT_W             (CA_PROMPT_FRM_W - CA_PROMPT_CONTERNT_X - 60)
#define CA_PROMPT_CONTERNT_H             (480)

//prompt
#define CA_PROMPT_PROMPT_Y             (CA_PROMPT_CONTERNT_Y + CA_PROMPT_CONTERNT_H + 40)
#define CA_PROMPT_PROMPT_H             (36)

#define CA_PROMPT_PROMPT_SENTER_TITLE_X             (0)
#define CA_PROMPT_PROMPT_SENTER_TITLE_W            (170)
#define CA_PROMPT_PROMPT_SENTER_X                      (CA_PROMPT_PROMPT_SENTER_TITLE_X + CA_PROMPT_PROMPT_SENTER_TITLE_W)
#define CA_PROMPT_PROMPT_SENTER_W                     (270)

#define CA_PROMPT_PROMPT_NEW_TITLE_X                 (CA_PROMPT_PROMPT_SENTER_X + CA_PROMPT_PROMPT_SENTER_W)
#define CA_PROMPT_PROMPT_NEW_TITLE_W                (120)
#define CA_PROMPT_PROMPT_NEW_X                          (CA_PROMPT_PROMPT_NEW_TITLE_X + CA_PROMPT_PROMPT_NEW_TITLE_W)
#define CA_PROMPT_PROMPT_NEW_W                          (100)

#define CA_PROMPT_PROMPT_IMPORT_TITLE_X             (CA_PROMPT_PROMPT_NEW_X + CA_PROMPT_PROMPT_NEW_W)
#define CA_PROMPT_PROMPT_IMPORT_TITLE_W            (120)
#define CA_PROMPT_PROMPT_IMPORT_X                      (CA_PROMPT_PROMPT_IMPORT_TITLE_X + CA_PROMPT_PROMPT_IMPORT_TITLE_W)
#define CA_PROMPT_PROMPT_IMPORT_W                     (100)

RET_CODE open_ca_prompt(u32 para1, u32 para2);

#endif

