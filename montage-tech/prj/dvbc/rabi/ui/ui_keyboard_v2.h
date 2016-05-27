/****************************************************************************

****************************************************************************/
#ifndef __UI_KEYBOARD_V2_H__
#define __UI_KEYBOARD_V2_H__
//coordinate

#define KEYBOARD_V2_MENU_X  (SYS_LEFT_CONT_W+(RIGHT_ROOT_W - KEYBOARD_V2_MENU_W) / 2)
#define KEYBOARD_V2_MENU_Y  (SCREEN_HEIGHT- KEYBOARD_V2_MENU_H - 80)//(36)
#define KEYBOARD_V2_MENU_W  (620)//(370)
#define KEYBOARD_V2_MENU_H  (330)//420

#define NAME_EDIT_X         (10)
#define NAME_EDIT_Y         (10)
#define NAME_EDIT_W         (KEYBOARD_V2_MENU_W - (NAME_EDIT_X + NAME_EDIT_X))
#define NAME_EDIT_H         (36)

#define KEYBOARD_CONT_X (0)
#define KEYBOARD_CONT_Y (NAME_EDIT_Y + NAME_EDIT_H + NAME_EDIT_Y)
#define KEYBOARD_CONT_W (KEYBOARD_V2_MENU_W)
#define KEYBOARD_CONT_H (225)//(KEYBOARD_V2_MENU_H - KEYBOARD_CONT_Y - 30)

#define KEYBOARD_MBOX_V2_X NAME_EDIT_X
#define KEYBOARD_MBOX_V2_Y 0
#define KEYBOARD_MBOX_V2_W (KEYBOARD_V2_MENU_W - NAME_EDIT_X - NAME_EDIT_X)//(320)//328
#define KEYBOARD_MBOX_V2_H (225)//(220)//216

#define KEYBOARD_MBOX_V2_ROW  (5)//6
#define KEYBOARD_MBOX_V2_COL  (10)//9
#define KEYBOARD_MBOX_V2_HGAP (0)//5
#define KEYBOARD_MBOX_V2_VGAP (0)//5

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

//typedef  void (*edit_save_cb)(void);
typedef  RET_CODE (*edit_save_cb)(u16 *p_unistr);

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


