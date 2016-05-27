#ifndef __UI_XSEARCH__H_
#define __UI_XSEARCH__H_


//cont
#define SEARH_X				SYS_LEFT_CONT_W	
#define SEARH_Y				SYS_LEFT_CONT_Y
#define SEARH_W				SYS_RIGHT_CONT_W
#define SEARH_H				SYS_LEFT_CONT_H

//auto search
#define UI_SEARCH_FREQ_X	60
#define UI_SEARCH_FREQ_Y	60
#define UI_SEARCH_FREQ_W	(SEARH_W-2*UI_SEARCH_FREQ_X)
#define SEARCH_MODU_CNT		9

RET_CODE open_ui_search(u32 para1, u32 para2);

#endif//__UI_XSEARCH__H_
