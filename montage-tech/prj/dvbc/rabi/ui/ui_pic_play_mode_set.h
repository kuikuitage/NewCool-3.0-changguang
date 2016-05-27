/****************************************************************************

****************************************************************************/
#ifndef __UI_PIC_PLAY_MODE_SET_H__
#define __UI_PIC_PLAY_MODE_SET_H__

/* coordinate */
#define PIC_PLAY_MODE_SET_CONT_X       ((SCREEN_WIDTH - PIC_PLAY_MODE_SET_CONT_W) / 2)
#define PIC_PLAY_MODE_SET_CONT_Y       (SCREEN_HEIGHT - PIC_PLAY_MODE_SET_CONT_H) / 2
#define PIC_PLAY_MODE_SET_CONT_W       400
#define PIC_PLAY_MODE_SET_CONT_H       300//220


#define PIC_PLAY_MODE_SET_ITEM_CNT      4
#define PIC_PLAY_MODE_SET_ITEM_X        ((PIC_PLAY_MODE_SET_CONT_W \
                                          - PIC_PLAY_MODE_SET_ITEM_LW \
                                          - PIC_PLAY_MODE_SET_ITEM_RW)/2)
#define PIC_PLAY_MODE_SET_ITEM_Y        20
#define PIC_PLAY_MODE_SET_ITEM_LW       260
#define PIC_PLAY_MODE_SET_ITEM_RW       130
#define PIC_PLAY_MODE_SET_ITEM_H        COMM_ITEM_H
#define PIC_PLAY_MODE_SET_ITEM_V_GAP    8

/* rect style */
#define RSI_PIC_PLAY_MODE_SET_FRM       RSI_COMMAN_BG
/* font style */

/* others */

RET_CODE open_pic_play_mode_set(u32 para1, u32 para2);

void pic_play_update_slide_show(BOOL is_net);

BOOL pic_play_get_slide_show_sts(void);

u8 pic_play_get_slide_interval(void);

BOOL pic_play_get_slide_repeat(void);

BOOL pic_play_get_special_effect(void);

#endif


