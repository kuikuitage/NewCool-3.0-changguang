/****************************************************************************

****************************************************************************/
#ifndef __UI_COMM_DLG_H__
#define __UI_COMM_DLG_H__

/* coordinate */
#define COMM_DLG_X         ((SCREEN_WIDTH - COMM_DLG_W) / 2)
#define COMM_DLG_Y         ((SCREEN_HEIGHT - COMM_DLG_H) / 2)
#define COMM_DLG_W        440 //300
#define COMM_DLG_H         140

#define DLG_CONTENT_GAP    20

#define DLG_BTN_W          64
#define DLG_BTN_H          30

/* rect style */
#define RSI_DLG_CONT       RSI_WINDOW_5
#define RSI_DLG_BTN_SH     RSI_KEYBOARD_ITEM_2_SH
#define RSI_DLG_BTN_HL     RSI_KEYBOARD_ITEM_2_HL

/* font style */
#define FSI_DLG_BTN_SH     FSI_WHITE
#define FSI_DLG_BTN_HL     FSI_WHITE
#define FSI_DLG_CONTENT    FSI_WHITE

#define MAX_BNT_COUNT      2
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

typedef struct
{
  u8        parent_root;
  u8        title_style;
  u8        text_style;
  u16       x;
  u16       y;
  u16       w;
  u16       h;
  u32       title_strID;  //标题
  u32       title_content;//标题
  u32       text_strID;   //正文
  u32       text_content; //正文
  u32       dlg_tmout;
}comm_dlg_data_t2;

typedef void (*do_func_t)(void);
typedef void (*do_func_withpara_t)(u32 para1, u32 para2);

dlg_ret_t ui_comm_dlg_open(comm_dlg_data_t *p_data);

void ui_comm_dlg_close(void);

dlg_ret_t ui_comm_dlg_open2(comm_dlg_data_t *p_data);

dlg_ret_t ui_comm_dlg_open3(comm_dlg_data_t2*p_data);
void ui_comm_dlg_close2(void);

void ui_comm_cfmdlg_open(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out);

void ui_comm_showdlg_open(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out);

void ui_comm_cfmdlg_open(rect_t *p_dlg_rc, u16 strid, do_func_t do_cmf, u32 tm_out);

void ui_comm_cfmdlg_open_unistr(rect_t *p_dlg_rc, u16* p_unistr, do_func_t do_cmf, u32 tm_out);

void ui_comm_cfmdlg_open_gb(rect_t *p_dlg_rc, u8* p_gb, do_func_t do_cmf, u32 tm_out);
void ui_comm_savdlg_open(rect_t *p_dlg_rc, do_func_t func, u32 tm_out);

void ui_comm_ask_for_dodlg_open_xxx(rect_t *p_dlg_rc, u16 strid,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out);

void ui_comm_ask_for_dodlg_open_unistr(rect_t *p_dlg_rc, u16 *p_unistr,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out);

void ui_comm_ask_for_dodlg_open(rect_t *p_dlg_rc, u16 strid,
                                  do_func_t do_func, do_func_t undo_func, u32 tm_out);

void ui_comm_ask_for_dodlg_open_ex(rect_t *p_dlg_rc, u16 strid,
  do_func_withpara_t do_func, u32 para1, u32 para2, u32 tm_out);

BOOL ui_comm_ask_for_savdlg_open(rect_t *p_dlg_rc,
                                   u16 strid,
                                   do_func_t do_save,
                                   do_func_t undo_save, u32 tm_out);

#endif


