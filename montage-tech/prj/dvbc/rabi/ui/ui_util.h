/****************************************************************************

 ****************************************************************************/
#ifndef __UI_UTIL_H__
#define __UI_UTIL_H__

#define LANG_CODE 15

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define IS_MASKED(x, mask)    (((x) & (mask)) == mask)

#define FOCUS_KEY_MAP(v_key, msg)   (msg = MSG_FOCUS_UP + v_key - V_KEY_UP)

#define PAGE_KEY_MAP(v_key, msg)   (msg = MSG_PAGE_UP + v_key - V_KEY_PAGE_UP)

RET_CODE ui_start_timer(u8 root_id, u16 msg, u32 time_out);
void convert_gb2312_chinese_asc2unistr(u8 *asc_str, u16 *uni_str, int uni_str_len);
void convert_utf8_chinese_asc2unistr(u8 *asc_str, u16 *uni_str, int uni_str_len);


#ifndef WIN32
inline RET_CODE ui_stop_timer(u8 root_id, u16 msg);
#else
RET_CODE ui_stop_timer(u8 root_id, u16 msg);
#endif

void ctrl_change_focus(control_t *p_lfCtrl, control_t *p_gfCtrl);

BOOL iso_639_get_lang_code(u8 *lang_code, u8 index);

u8 iso_639_get_lang_code_num();

BOOL iso_639_get_lang_code_name(u16 *uni_lang_code, u16 *lang_name_id, u8 index);

#endif

