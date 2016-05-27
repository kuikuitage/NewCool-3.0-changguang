/****************************************************************************

****************************************************************************/
#ifndef __UI_KEYBOARD_V2_H__
#define __UI_KEYBOARD_V2_H__
//coordinate

#define KEYBOARD_V2_MENU_X  (700)
#define KEYBOARD_V2_MENU_Y  (120)//(75)//(36)
#define KEYBOARD_V2_MENU_W  (490)//(370)
#define KEYBOARD_V2_MENU_H  (370)//(460)//420

#define NAME_EDIT_X         (15)
#define NAME_EDIT_Y         (20)
#define NAME_EDIT_W         (KEYBOARD_V2_MENU_W - (NAME_EDIT_X + NAME_EDIT_X))
#define NAME_EDIT_H         (30)

#define KEYBOARD_CONT_X (0)
#define KEYBOARD_CONT_Y (70)
#define KEYBOARD_CONT_W (KEYBOARD_V2_MENU_W)
#define KEYBOARD_CONT_H (280)//(KEYBOARD_V2_MENU_H - KEYBOARD_CONT_Y - 30)

#define LABEL_CONT_X (4)
#define LABEL_CONT_Y (0)
#define LABEL_CONT_W (KEYBOARD_CONT_W - (LABEL_CONT_X + LABEL_CONT_X))
#define LABEL_CONT_H (35)

#define IPM_HELP_X (0)
#define IPM_HELP_Y (0)
#define IPM_HELP_W (250)
#define IPM_HELP_H LABEL_CONT_H

#define IPM_COM_X (LABEL_CONT_W - IPM_COM_W - 16)
#define IPM_COM_Y (0)
#define IPM_COM_W (70)
#define IPM_COM_H LABEL_CONT_H

#define KEYBOARD_MBOX_V2_X ((KEYBOARD_CONT_W - KEYBOARD_MBOX_V2_W) >> 1)
#define KEYBOARD_MBOX_V2_Y (60)
#define KEYBOARD_MBOX_V2_W (410)//(320)//328
#define KEYBOARD_MBOX_V2_H (200)//(220)//216

#define KEYBOARD_MBOX_V2_ROW  (5)//6
#define KEYBOARD_MBOX_V2_COL  (10)//9
#define KEYBOARD_MBOX_V2_HGAP (10)//5
#define KEYBOARD_MBOX_V2_VGAP (10)//5

#define KEYBOARD_HELP_X (20)
#define KEYBOARD_HELP_Y (370)//330
#define KEYBOARD_HELP_W (KEYBOARD_V2_MENU_W - (KEYBOARD_HELP_X + KEYBOARD_HELP_X))
#define KEYBOARD_HELP_H (40)

typedef enum 
{
  KB_INPUT_TYPE_NONE = 0,
  KB_INPUT_TYPE_SENTENCE = 0x1,
  KB_INPUT_TYPE_DEC_NUMERIC = (0x1 << 1),
  KB_INPUT_TYPE_HEX_NUMERIC = (0x1 << 2),
  KB_INPUT_TYPE_ENGLISH = (0x1 << 3),
  KB_INPUT_TYPE_MAX
} kb_input_type_t;

typedef  void (*edit_save_cb)(void);

typedef struct
{
  u16 *         uni_str;
  u16           max_len;
  u16            type;
  edit_save_cb cb;
} kb_param_t;

RET_CODE open_keyboard_v2(u32 para1, u32 para2);

#define CODING 
#endif


