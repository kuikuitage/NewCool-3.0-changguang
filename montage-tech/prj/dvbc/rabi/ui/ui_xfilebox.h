/****************************************************************************

 ****************************************************************************/
#ifndef __UI_XFILEBOX_H__
#define __UI_XFILEBOX_H__

typedef  RET_CODE (*filebox_update_t)(control_t* p_list, u16 start, u16 size);
control_t *_filebox_create(control_t *parent,
                                    u8 ctrl_id,
                                    u16 row,
                                    u16 column,
                                    u16 x,
                                    u16 y,
                                    u16 w,
                                    u16 h);
void filebox_totbl_set(u16 totbl);
void filebox_icon_set(control_t *p_ctrl,
                                    u16 item_idx,
                                    u16 bmp_id);
void filebox_name_set(control_t *p_ctrl,
                                    u16 item_idx,
                                    u16 *p_unistr);
void filebox_boxupdate_cb_set(control_t *p_ctrl, filebox_update_t p_cb);
u16 filebox_get_count();
u16 ui_filebox_btn_keymap(u16 key);

RET_CODE ui_filebox_btn_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
#endif