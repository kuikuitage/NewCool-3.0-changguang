/****************************************************************************

 ****************************************************************************/
#ifndef __AP_DBASE_API_H__
#define __AP_DBASE_API_H__

/* get the view type which the prog is belong to */
dvbs_view_t ui_dbase_get_prog_view (dvbs_prog_node_t *p_node);

/* get the sat node according to the position in view, NOTICE: the view should be created */
u16 ui_dbase_get_sat_by_pos (u8 view_id, sat_node_t *p_node, u16 pos);

/* get the tp node according to the position in view, NOTICE: the view should be created */
u16 ui_dbase_get_tp_by_pos (u8 view_id, dvbs_tp_node_t *p_node, u16 pos);

void ui_dbase_get_full_prog_name(dvbs_prog_node_t *p_node, u16 *str, u16 max_length);

BOOL ui_dbase_pg_is_scambled(u16 rid);

void ui_dbase_get_diseqc_str(u16 *str, u16 len, sat_node_t * p_node);

void ui_dbase_get_22k_str(u16 *str, u16 len, sat_node_t *p_node);

BOOL ui_dbase_pg_ask_for_belong(BOOL is_all_sat, u16 pg_id, u16 *sat_pos, u16 *tp_pos);

void ui_dbase_set_pg_view_id(u8 view_id);

u8 ui_dbase_get_pg_view_id(void);

BOOL ui_dbase_pg_is_full(void);

void ui_dbase_reset_pg_view(void);

BOOL ui_dbase_pg_is_fav(u8 view_id, u16 pos);

u32 ui_dbase_get_pg_curn_audio_pid(dvbs_prog_node_t *p_node);

void ui_dbase_set_pg_curn_audio_pid(dvbs_prog_node_t *p_node, u32 pid);

void ui_dbase_delete_all_pg(void);

BOOL ui_dbase_check_tvradio(BOOL is_tv);


#define FREQ_VAL_CNT                (12)
#define FREQ_USER_CNT              (2)
#define FREQ_UNIVERSAL_CNT     (4)
#define FREQ_UNICABLE_CNT       (2)
#define LNB_FREQ_CNT        (FREQ_VAL_CNT+FREQ_USER_CNT+FREQ_UNIVERSAL_CNT+FREQ_UNICABLE_CNT) // 5150-5750, 5750-5150, Universal(9750-10600)


//void ui_dbase_pos2freq(u16 pos, u32 *p_lnbtype, u32 *p_freq1, u32 *p_freq2);
void ui_dbase_pos2freq
  (u16 pos, u32 *p_lnbtype, u32 *p_freq1, u32 *p_freq2, u8 *p_is_unicable, u8 *p_unicable_type);

u16 ui_dbase_freq2pos(u32 lnbtype, u32 freq1, u32 freq2, u8 p_is_unicable, u8 p_unicable_type);

//u16 ui_dbase_freq2pos(u32 lnbtype, u32 freq1, u32 freq2);

void ui_dbase_delete_all_position(void);

BOOL ui_dbase_reset_last_prog(BOOL focus_first);

char * ui_dbase_get_ca_system_desc(dvbs_prog_node_t *p_pg);

void ui_dbase_init(void);

u8 ui_dbase_create_view(dvbs_view_t name, u32 context, u8 *p_ex_buf);

void ui_dbase_pg_sort(dvbs_view_t name);

#endif

