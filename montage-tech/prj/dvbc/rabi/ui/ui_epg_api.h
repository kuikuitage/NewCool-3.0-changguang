#ifndef _UI_EPG_API_H
#define _UI_EPG_API_H

#ifndef IMPL_NEW_EPG
/*!
  dynamic policy services number;better be a odd number
  */
#define DY_SVC_NUM (4)
#define DY_DAY_NUM (2)

/*!
  SVC number by the side of the center SVC
  */
#define DY_OFFSET  (DY_SVC_NUM - 1)/2
#define INVALID_PARENTAL_RATING 0xff

enum epg_msg
{
  MSG_EPG_READY = MSG_EXTERN_BEGIN + 50,
  MSG_STOP_EPG,
  MSG_PF_READY,
  MSG_EPG_DEL,
  MSG_TDT_FOUND,
  MSG_PF_CHANGE,
};

#define EPG_DESC_MAX_NAME_LENGTH    256

void ui_epg_init(void);

void ui_epg_release(void);

void ui_epg_start(void);

void ui_epg_stop(void);

void ui_set_epg_dy(void);

void ui_set_epg_dy_by_offset(u16 offset);

void ui_set_epg_day_offset(u8 offset);

void ui_set_epg_policy_period(utc_time_t *p_start, 
                                                u32 hour_num);

u8 ui_get_current_parental_rating(void);

void ui_epg_set_lang_code(u8 *first_code, u8 *second_code, u8 *default_code);
#else    //New EPG Policy
enum epg_msg
{
  MSG_SCHE_READY = MSG_EXTERN_BEGIN + 50,
  MSG_STOP_EPG,
  MSG_PF_READY,
  MSG_EPG_DEL,
  MSG_EPG_CHANG_TABLE_POLICY,
  MSG_TDT_FOUND,
};

#define MAX_EXT_TXT_BUF_LEN     ((4)*(KBYTES))

#define EPG_DESC_MAX_NAME_LENGTH    256

void ui_epg_init(void);

void ui_epg_release(void);

void ui_epg_start(epg_table_selecte_policy selecte);

void ui_epg_stop(void);

BOOL ui_epg_is_initialized(void);

void ui_epg_set_program_info(u16 pg_id);
#endif
#endif

