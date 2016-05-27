#ifndef __UI_FUN_HELP_H__
#define __UI_FUN_HELP_H__

enum fun_help_timer
{
	MSG_FUN_HELP_TIMER_CREATE = MSG_LOCAL_BEGIN + 100,
	MSG_FUN_HELP_TIMER_RESET,
	MSG_FUN_HELP_TIMER_DESTROY,
};

RET_CODE close_fun_help_later(void);
RET_CODE open_fun_help_later(void);
RET_CODE open_fun_help(u32 para1, u32 para2);

#endif// __UI_FUN_HELP_H__

