/****************************************************************************

 ****************************************************************************/
#ifndef __UI_BOOK_API_H__
#define __UI_BOOK_API_H__

#define BOOK_TIME_OUT_MS  1000
#define RECORD_TIME_OUT_MS 15000
#define RECORD_WAKE_UP_TIME 60000

enum latest_node_state
{
  LATEST_NODE_WAIT = 0,
  LATEST_NODE_START,
  LATEST_NODE_END,
};

RET_CODE ui_book_init(void);

RET_CODE ui_book_tmr_release(void);

BOOL  is_time_booked(book_pg_t *p_node, u8 except_index);

BOOL is_node_playing(book_pg_t *p_node);

void book_get_book_node(u8 index, book_pg_t *p_node);

BOOL book_add_node(book_pg_t *p_node);

void book_edit_node(u8 index, book_pg_t *p_node);

BOOL book_delete_node(u8 index);

BOOL book_delete_all_node(void);
u8 book_get_book_num(void);

u8 book_check_latest_node(u8 index, BOOL drag_node);
u8 book_check_latest_record_node(u8 index);
void book_check_node_on_delete(void);

void book_overdue_node_handler(u8 index);

u8 book_get_match_node(book_pg_t *p_node);

void book_delete_overdue_node(void);

u8 book_get_latest_index(void);

void ui_set_book_flag(BOOL is_enable);

BOOL ui_get_book_flag(void);
#endif 
