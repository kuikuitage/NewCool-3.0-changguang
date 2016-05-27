/****************************************************************************

****************************************************************************/
#ifndef __UI_COMM_DLG_H__
#define __UI_COMM_DLG_H__

/* coordinate */
#define COMM_DLG_X         ((SCREEN_WIDTH - COMM_DLG_W) / 2)
#define COMM_DLG_Y         ((SCREEN_HEIGHT - COMM_DLG_H) / 2)
#define COMM_DLG_W         300
#define COMM_DLG_H         160

#define DLG_CONTENT_GAP    20

#define DLG_BTN_W          64
#define DLG_BTN_H          30

/* rect style */
#define RSI_DLG_CONT       RSI_WINDOW_5
#define RSI_DLG_BTN_SH     RSI_KEYBOARD_ITEM_2_SH
#define RSI_DLG_BTN_HL     RSI_KEYBOARD_ITEM_2_HL

/* font style */
#define FSI_DLG_BTN_SH     FSI_WHITE_18
#define FSI_DLG_BTN_HL     FSI_WHITE_18
#define FSI_DLG_CONTENT    FSI_WHITE

enum dlg_type
{
  DLG_FOR_ASK = 0x00, //popup dialog type:	with  yes/no button
  DLG_FOR_CONFIRM,    //popup dialog type:	with ok button
  DLG_FOR_SHOW,       //popup dialog type:	without button
};

typedef enum 
{
  DLG_RET_YES,
  DLG_RET_NO,
  DLG_RET_NULL,
}dlg_ret_t;

enum dlg_str_mode
{
  DLG_STR_MODE_STATIC = 0x10,
  DLG_STR_MODE_EXTSTR = 0x20,
};


typedef enum
{
  ZERO_LEVEL = 0, //this level used for upgrade system
  FIRST_LEVEL, //this level only used for manually choose
  SEC_LEVEL, //this level used for timeout = 0; choose dlg which can close automatically
  THIRD_LEVEL,//this level used for comm dlg
  FOURTH_LEVEL,//this level used for comm dlg
  
}dlg_priority_t;


#define MAX_BNT_COUNT      2

typedef struct
{
  u8        parent_root;
  u8        style;
  u16       x;
  u16       y;
  u16       w;
  u16       h;
  u32       content;
  u32       dlg_tmout;
  BOOL      specify_bnt;     //draw button by input content
  u8        specify_bnt_count;
  u16       specify_bnt_cont[MAX_BNT_COUNT];
}comm_dlg_data_t;


typedef void (*do_func_t)(void);

dlg_ret_t ui_comm_dlg_open(comm_dlg_data_t *p_data, dlg_priority_t level);
dlg_ret_t ui_comm_dlg_open_ex(comm_dlg_data_t *p_data);

void ui_comm_dlg_close(void);
void ui_comm_dlg_close_ex(void);

void ui_comm_cfmdlg_open(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out);
void ui_comm_cfmdlg_open_ex(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out);

void ui_comm_savdlg_open(rect_t *p_dlg_rc, do_func_t func, u32 tm_out);

void ui_comm_ask_for_dodlg_open(rect_t *p_dlg_rc, u16 strid,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out, dlg_priority_t level);

void ui_comm_ask_for_savdlg_open(rect_t *p_dlg_rc,
                                   u16 strid,
                                   do_func_t do_save,
                                   do_func_t undo_save, u32 tm_out);
void ui_comm_cfmdlg_open_unistr(rect_t *p_dlg_rc, u16* p_unistr, do_func_t do_cmf, u32 tm_out, dlg_priority_t level);




#endif


