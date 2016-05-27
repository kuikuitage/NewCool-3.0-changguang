/****************************************************************************

****************************************************************************/
#ifndef __UI_SLEEP_HOTKEY_H__
#define __UI_SLEEP_HOTKEY_H__

/* coordinate */
#define SLEEP_HOTKEY_CONT_X        (SCREEN_WIDTH - SLEEP_HOTKEY_CONT_W - 40)
#define SLEEP_HOTKEY_CONT_Y        50
#define SLEEP_HOTKEY_CONT_W        450
#define SLEEP_HOTKEY_CONT_H         60

#define SLEEP_HOTKEY_ITEM_X        30
#define SLEEP_HOTKEY_ITEM_Y        5//(SLEEP_HOTKEY_ITEM_H*2)

#define SLEEP_HOTKEY_ITEM_LW       180
#define SLEEP_HOTKEY_ITEM_RW       (SLEEP_HOTKEY_ITEM_W - SLEEP_HOTKEY_ITEM_LW)

#define SLEEP_HOTKEY_ITEM_W        (SLEEP_HOTKEY_CONT_W - 2*SLEEP_HOTKEY_ITEM_X)
#define SLEEP_HOTKEY_ITEM_H        COMM_ITEM_H

/* rect style */
#define RSI_SLEEP_HOTKEY_FRM         RSI_PBACK//RSI_INFOR_WIN

/* font style */

/* others */

#define SLEEP_CNT 6

void sleep_hotkey_set_content(u8 focus);

RET_CODE open_sleep_hotkey(u32 para1, u32 para2);

#endif

