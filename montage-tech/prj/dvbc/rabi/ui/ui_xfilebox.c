/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_xfilebox.h"

#define FILEBOX_PAGE_FILES_MAX 12


enum FILEBOX_type
{
  FILEBOX_INVALID_CTRL = 0,
  	
  FILEBOX_BTN_1,
  FILEBOX_TXT_1,
  FILEBOX_BTN_2,
  FILEBOX_TXT_2,
  FILEBOX_BTN_3,
  FILEBOX_TXT_3,
  FILEBOX_BTN_4,
  FILEBOX_TXT_4,
  FILEBOX_BTN_5,
  FILEBOX_TXT_5,
  FILEBOX_BTN_6,
  FILEBOX_TXT_6,
  FILEBOX_BTN_7,
  FILEBOX_TXT_7,
  FILEBOX_BTN_8,
  FILEBOX_TXT_8,
  FILEBOX_BTN_9,
  FILEBOX_TXT_9,
  FILEBOX_BTN_10,
  FILEBOX_TXT_10,
  FILEBOX_BTN_11,
  FILEBOX_TXT_11,
  FILEBOX_BTN_12,
  FILEBOX_TXT_12,
  FILEBOX_BTN_13,
  FILEBOX_TXT_13,
};

typedef struct filebox_pro
{
	u16 row;
	u16 column;
	u16 start;
	u16 old_start;
	u16 total;
	u16 focus;
	u16 old_focus;
	filebox_update_t p_cb;
}filebox_t;

filebox_t filebox_pro_t;
u16 ui_filebox_btn_keymap(u16 key);

RET_CODE ui_filebox_btn_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

control_t *_filebox_create(control_t *parent,
                                    u8 ctrl_id,
                                    u16 row,
                                    u16 column,
                                    u16 x,
                                    u16 y,
                                    u16 w,
                                    u16 h)
{
	control_t *cont, *txt[FILEBOX_PAGE_FILES_MAX], *p_ctrl[FILEBOX_PAGE_FILES_MAX];
	int i =0 ,j=0 ,index =0 ;
	filebox_pro_t.row=row;
	filebox_pro_t.column=column;
	filebox_pro_t.start=1;
	filebox_pro_t.total=0;
	filebox_pro_t.focus=1;
	cont = ctrl_create_ctrl(CTRL_CONT, ctrl_id,
                     x, y, w, h, parent, 0);

	ctrl_set_style (cont, STL_EX_WHOLE_HL);
	ctrl_set_rstyle(cont,
	          RSI_PBACK,
	          RSI_PBACK,
	          RSI_PBACK);
	ctrl_set_style(cont, STL_EX_WHOLE_HL);
	ctrl_set_keymap(cont, ui_filebox_btn_keymap);
	ctrl_set_proc(cont, ui_filebox_btn_proc);

	for(j = 0; j < row ; j++)
	{
		for(i = 0;i < column ;i++)
		{
			index = (i + j * column)*2;
			p_ctrl[index/2] = ctrl_create_ctrl(CTRL_TEXT, FILEBOX_BTN_1 + index,
			                       ( w / column )  / 10 + ( w / column ) * i ,
			                       ( h / row )  / 10 + ( h / row ) * j , 
			                       (w / column) /10 * 8 , 
			                       (h / row) /10 * 6,
			                       cont, 0);
			ctrl_set_rstyle(p_ctrl[index/2] ,
						RSI_COMM_TXT_SH,
						RSI_COMM_TXT_HL,
						RSI_COMM_TXT_GRAY);
			text_set_content_type(p_ctrl[index/2] , TEXT_STRTYPE_STRID);
			text_set_font_style(p_ctrl[index/2] ,
						FSI_WHITE_45,
						FSI_WHITE_45,
						FSI_WHITE_45);
			text_set_align_type(p_ctrl[index/2] , STL_VCENTER | STL_VCENTER);
			
			txt[index/2] = ctrl_create_ctrl(CTRL_TEXT, FILEBOX_BTN_1 + index + 1,
			                       ( w / column )  / 10 + ( w / column ) * i ,
			                       ( h / row )  / 10 + ( h / row ) * j , 
			                       (w / column) /10 * 8 , 
			                       (h / row) /10 * 6,
			                       cont, 0);
			ctrl_set_rstyle(txt[index/2] ,
						RSI_COMM_TXT_SH,
						RSI_COMM_TXT_HL,
						RSI_COMM_TXT_GRAY);
			text_set_content_type(txt[index/2] , TEXT_STRTYPE_STRID);
			text_set_font_style(txt[index/2] ,
						FSI_WHITE_24,
						FSI_WHITE_24,
						FSI_WHITE_24);
			text_set_align_type(txt[index/2] , STL_VCENTER | STL_VCENTER);
		}
	}
	return cont;
		
}

void filebox_totbl_set(u16 totbl)
{
	filebox_pro_t.total = totbl;
}

void filebox_icon_set(control_t *p_ctrl,
                                    u16 item_idx,
                                    u16 bmp_id)
{
	control_t *p_btn;
	p_btn = ctrl_get_child_by_id(p_ctrl, FILEBOX_BTN_1 + item_idx * 2);
	bmap_set_content_by_id(p_btn,bmp_id);
}
void filebox_name_set(control_t *p_ctrl,
                                    u16 item_idx,
                                    u16 *p_unistr)
{
	control_t *p_txt;
	p_txt = ctrl_get_child_by_id(p_ctrl, FILEBOX_BTN_1 + item_idx * 2 + 1);
	text_set_content_by_extstr(p_txt,p_unistr);
}

void filebox_boxupdate_cb_set(control_t *p_ctrl, filebox_update_t p_cb)
{
	filebox_pro_t.p_cb = p_cb;
}

static RET_CODE filebox_set_btn_focue(control_t *p_ctrl,u16 focue,u16 old_focue)
{
	control_t *p_btn , *p_btn_old;
	p_btn = ctrl_get_child_by_id(p_ctrl, FILEBOX_BTN_1 + focue * 2);
	p_btn_old = ctrl_get_child_by_id(p_ctrl, FILEBOX_BTN_1 + old_focue * 2);
	ctrl_default_proc(p_btn_old, MSG_LOSTFOCUS, 0, 0);
	ctrl_default_proc(p_btn, MSG_GETFOCUS, 0, 0);
	return SUCCESS;
}


static RET_CODE filebox_show_btn(control_t *p_ctrl,u16 start)
{
	control_t  *p_btn[FILEBOX_PAGE_FILES_MAX], *p_txt[FILEBOX_PAGE_FILES_MAX];
	u16 need_btn,i,btn_num;
	btn_num = filebox_pro_t.row * filebox_pro_t.column;
	need_btn = filebox_pro_t.total - start;
	
	for(i=0;i<btn_num;i++)
	{
		p_btn[i] = ctrl_get_child_by_id(p_ctrl, FILEBOX_BTN_1 + i * 2);
		p_txt[i] = ctrl_get_child_by_id(p_ctrl, FILEBOX_BTN_1 + i * 2 + 1);
		if(i>need_btn)
			{
			ctrl_set_sts(p_btn[i] , OBJ_STS_HIDE);
			ctrl_set_sts(p_txt[i] , OBJ_STS_HIDE);
		}else
			{
			ctrl_set_sts(p_btn[i] , OBJ_STS_SHOW);
			ctrl_set_sts(p_txt[i] , OBJ_STS_SHOW);
		}
	}
	return SUCCESS;
}

u16 filebox_get_count()
{
	return filebox_pro_t.total;
}

static RET_CODE filebox_update_set(control_t *p_ctrl,filebox_update_t p_cb)
{
	u16 need_btn,btn_num,hide_btn_num;
	btn_num = filebox_pro_t.row * filebox_pro_t.column;
	need_btn = filebox_pro_t.total - filebox_pro_t.start;
	hide_btn_num = btn_num -need_btn;
	p_cb(p_ctrl,filebox_pro_t.start,need_btn);
	filebox_show_btn(p_ctrl,filebox_pro_t.start);
	return SUCCESS;
}

static RET_CODE filebox_set_focus_change(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
	struct filebox_pro *pfilepro = &filebox_pro_t;
	u16 need_btn,btn_num;
	btn_num = pfilepro->row * pfilepro->column;
	need_btn = pfilepro->total - pfilepro->start;
	pfilepro->old_focus = pfilepro->focus;
	pfilepro->old_start = pfilepro->start;
	switch(msg)
	{
		case MSG_FOCUS_UP:
			if(pfilepro->focus < pfilepro->column) 
			{
				if(pfilepro->start == 1)
				{
					pfilepro->start = pfilepro->total - 
								(pfilepro->total %
								btn_num) + 1;
					if(pfilepro->total - pfilepro->start < pfilepro->focus + pfilepro->column)
					{
						pfilepro->focus = pfilepro->total - pfilepro->start;
						break;
					}
				}else
				{
					pfilepro->start = (pfilepro->start - 
								btn_num + 
								pfilepro->total) %
								pfilepro->total;
				}
			}
			pfilepro->focus = (pfilepro->focus - pfilepro->column + btn_num) 
										% btn_num;
			break;
		case MSG_FOCUS_DOWN:
			if(pfilepro->focus >= pfilepro->column ) 
			{
				if(pfilepro->start == (pfilepro->total - (pfilepro->total % btn_num) + 1))
				{
					pfilepro->start = 1;
				}else{
					pfilepro->start = (pfilepro->start + 
								btn_num + 
								pfilepro->total) %
								pfilepro->total;
				}
			}else if(pfilepro->start == (pfilepro->total - (pfilepro->total % btn_num) + 1)
					&& pfilepro->total % btn_num < pfilepro->column)
			{
				pfilepro->start = 1;
				break;
			}
			if(pfilepro->column > need_btn)
			{
				pfilepro->focus = need_btn % btn_num;
			}else
			{
				pfilepro->focus = (pfilepro->focus + pfilepro->column + btn_num) 
										% btn_num;
			}
			break;
		case MSG_FOCUS_RIGHT:
			if(pfilepro->focus + 1 == btn_num) 
			{ 
				if(pfilepro->start == (pfilepro->total - (pfilepro->total % btn_num) + 1))
				{
					pfilepro->start = 1;
				}else{
					pfilepro->start = (pfilepro->start + 
								btn_num + 
								pfilepro->total) %
								pfilepro->total;
				}
			}else if(pfilepro->start == (pfilepro->total - (pfilepro->total % btn_num) + 1)
					&& pfilepro->total % btn_num == pfilepro->focus)
			{
				pfilepro->start = 1;
				pfilepro->focus = 0;
				break;
			}
			pfilepro->focus = (pfilepro->focus + 1 + btn_num) 
										% btn_num;
			break;
		case MSG_FOCUS_LEFT:
			if(pfilepro->focus == 0) 
			{ 
				if(pfilepro->start == 1)
				{
					pfilepro->start = pfilepro->total - 
								(pfilepro->total %
								btn_num) + 1;
					if(pfilepro->total - pfilepro->start < pfilepro->focus + pfilepro->column)
					{
						pfilepro->focus = pfilepro->total - pfilepro->start;
						break;
					}
				}else
				{
					pfilepro->start = (pfilepro->start - 
								btn_num + 
								pfilepro->total) %
								pfilepro->total;
				}
			}
			pfilepro->focus = (pfilepro->focus - 1 + btn_num) 
										% btn_num;
			break;
		default:
			return ERR_FAILURE;
			break;
	}
	if(pfilepro->old_start != pfilepro->start)
		filebox_update_set(p_ctrl,pfilepro->p_cb);
	filebox_set_btn_focue(p_ctrl,pfilepro->focus,pfilepro->old_focus);
	ctrl_paint_ctrl(p_ctrl,TRUE);
	return SUCCESS;
}


BEGIN_KEYMAP(ui_filebox_btn_keymap, NULL)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
	ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
END_KEYMAP(ui_filebox_btn_keymap, NULL)

BEGIN_MSGPROC(ui_filebox_btn_proc, text_class_proc)
	ON_COMMAND(MSG_FOCUS_UP, filebox_set_focus_change)
	ON_COMMAND(MSG_FOCUS_DOWN, filebox_set_focus_change)
	ON_COMMAND(MSG_FOCUS_RIGHT, filebox_set_focus_change)
	ON_COMMAND(MSG_FOCUS_LEFT, filebox_set_focus_change)
END_MSGPROC(ui_filebox_btn_proc, text_class_proc)
