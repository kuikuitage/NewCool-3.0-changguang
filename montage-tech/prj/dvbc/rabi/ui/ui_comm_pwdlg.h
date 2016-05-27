/****************************************************************************

****************************************************************************/
#ifndef __UI_COMM_PWDLG_H__
#define __UI_COMM_PWDLG_H__

#define PWDLG_T_COMMON       0  // super password or common password can pass , 4 bits
#define PWDLG_T_CA           1  // ca password can pass , 6 bits
#define PWDLG_T_SUPER        2  // super password can pass , 4 bits
#define PWDLG_T_IPPV         3  // ca password can pass , 6 bits
#define PWDLG_T_CUSTMER      4  // ca password can pass , 6 bits

#define PWD_LENGTH_COMMON    4
#define PWD_LENGTH_SUPER     4
#define PWD_LENGTH_CA    	 6

#define PWDLG_W             390// 342
#define PWDLG_H              160

#define PWDLG_TXT1_L         20
#define PWDLG_TXT1_T         40
#define PWDLG_TXT1_W         (PWDLG_W - 2 * PWDLG_EDIT_L)
#define PWDLG_TXT1_H         30

#define PWDLG_EDIT_L         20
#define PWDLG_EDIT_T         75
#define PWDLG_EDIT_W         (PWDLG_W - 2 * PWDLG_EDIT_L)
#define PWDLG_EDIT_H         36

#define PWDLG_HELP_L         0
#define PWDLG_HELP_T         (PWDLG_H - PWDLG_HELP_H - 10)
#define PWDLG_HELP_W         PWDLG_W
#define PWDLG_HELP_H         30

#define FSI_PWDLG_TXT        FSI_WHITE
#define FSI_PWDLG_HELP       FSI_WHITE

#define RSI_PWDLG_CONT       RSI_WINDOW_GAME
#define RSI_PWDLG_TXT        RSI_ITEM_6_GRAY
#define RSI_PWDLG_EDIT       RSI_KEYBOARD_CONTENT

typedef struct
{
  u8 parent_root;
  u16                   left;
  u16                   top;
  u16                   strid;
  keymap_t keymap;
  msgproc_t proc;
  u8         password_type;
}comm_pwdlg_data_t;

BOOL ui_comm_pwdlg_open(comm_pwdlg_data_t* p_data);
void ui_comm_pwdlg_close(void);

#endif
