/****************************************************************************

 ****************************************************************************/
#ifndef __UI_SIGNAL_API_H__
#define __UI_SIGNAL_API_H__

enum signal_msg
{
  MSG_SIGNAL_CHECK = MSG_EXTERN_BEGIN + 300,
  MSG_SIGNAL_UPDATE,
  MSG_SIGNAL_UNLOCK,
  MSG_SIGNAL_LOCK,
  MSG_SIGNAL_WEAK,
};

struct signal_data
{
	u8 intensity;
	u8 quality;
	u8 ber;
	BOOL lock;
};

void ui_init_signal(void);

//void ui_enable_signal_monitor(BOOL is_enable);

void ui_enable_signal_monitor_by_tuner(BOOL is_enable,u8 tuner_index);

void ui_release_signal(void);

BOOL ui_signal_is_lock(void);

void ui_signal_check(u32 para1, u32 para2);

void ui_set_transpond(dvbs_tp_node_t *p_tp);

void ui_signal_set_lock(BOOL is_lock);

void ui_set_antenna_and_transpond(sat_node_t *p_sat, dvbs_tp_node_t *p_tp);
#endif



