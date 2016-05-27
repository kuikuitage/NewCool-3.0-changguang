/****************************************************************************

 ****************************************************************************/
#ifndef __UI_JUMP_H__
#define __UI_JUMP_H__

/* coordinate */

#define JUMP_ITEM_CNT		  1
#define JUMP_ITEM_X			  60
#define JUMP_ITEM_Y			  60
#define JUMP_ITEM_LW		((SYS_RIGHT_CONT_W-2*JUMP_ITEM_X)/3)
#define JUMP_ITEM_RW		((SYS_RIGHT_CONT_W-2*JUMP_ITEM_X)/3*2)
#define JUMP_ITEM_H			  COMM_ITEM_H
#define JUMP_ITEM_V_GAP		2

#define JUMP_SYSTEM_VGAP   40

#define JUMP_SYS_X			 JUMP_ITEM_X
#define JUMP_SYS_Y			 (JUMP_SYSTEM_VGAP + JUMP_ITEM_Y + JUMP_ITEM_CNT * (JUMP_ITEM_H + JUMP_ITEM_V_GAP)) 
#define JUMP_SYS_W		   200
#define JUMP_SYS_H		   40

#define JUMP_NAME_X		 (JUMP_SYS_X + JUMP_SYS_W)
#define JUMP_NAME_Y     JUMP_SYS_Y
#define JUMP_NAME_W		 200
#define JUMP_NAME_H		 40


/* rect style */
#define RSI_JUMP_FRM		RSI_WINDOW_1
/* font style */

/* others */

RET_CODE open_jump (u32 para1, u32 para2);
u16 ui_jump_para_get();
#endif

