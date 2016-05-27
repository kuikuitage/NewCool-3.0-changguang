/********************************************************************************************/

/********************************************************************************************/

#include "ui_common.h"
#include "ui_terrestrial.h"
#include "ui_do_search.h"

enum control_id
{
  IDC_INVALID = 0,
  IDC_TERRESTRIAL_AUTO_SEARCH,
  IDC_TERRESTRIAL_MANUAL_SEARCH,
  IDC_TERRESTRIAL_FULL_SEARCH,
  IDC_TERRESTRIAL_HELP_CONT,
};

u16 terrestrial_keymap(u16 key);
RET_CODE terrestrial_proc(control_t *p_btn, u16 msg, u32 para1, u32 para2);

static RET_CODE _ui_terrestrial_select(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t *p_ctrl = NULL;

  p_ctrl = ctrl_get_active_ctrl(p_cont);
  switch(ctrl_get_ctrl_id(p_ctrl))
  {
    case IDC_TERRESTRIAL_AUTO_SEARCH:
      manage_open_menu(ROOT_ID_DO_SEARCH, SCAN_TYPE_DTMB_AUTO, 0);
      break;
    case IDC_TERRESTRIAL_MANUAL_SEARCH:
      manage_open_menu(ROOT_ID_TERRESTRIAL_MANUAL_SEARCH, 0, 0);
      break;
    case IDC_TERRESTRIAL_FULL_SEARCH:
      manage_open_menu(ROOT_ID_TERRESTRIAL_FULL_SEARCH, 0, 0);
      break;
//    case IDC_TERRESTRIAL_FREQ_SET:
//      manage_open_menu(ROOT_ID_TERRESTRIAL_FREQ_SET, 0, 0);
//      break;
    default:
      MT_ASSERT(0);
      return ERR_FAILURE;
  }
   
  return SUCCESS;
}


RET_CODE open_terrestrial(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[TERRESTRIAL_ITEM_CNT];
  u16 i = 0, y; 
  u16 stxt[TERRESTRIAL_ITEM_CNT] =
  {
    IDS_AUTOL_SEARCH, IDS_MANUAL_SEARCH, IDS_FULL_SCAN
  };
  DEBUG(DBG, INFO, "\n");
  p_cont = ui_comm_right_root_create(ROOT_ID_TERRESTRIAL, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, terrestrial_keymap);
  ctrl_set_proc(p_cont, terrestrial_proc);

  y = TERRESTRIAL_ITEM_Y;
  for(i = 0; i < TERRESTRIAL_ITEM_CNT; i++)
  {
    p_ctrl[i] = ctrl_create_ctrl(CTRL_TEXT, (u8)(IDC_TERRESTRIAL_AUTO_SEARCH + i),
                          TERRESTRIAL_ITEM_X, y, (TERRESTRIAL_ITEM_LW + TERRESTRIAL_ITEM_RW),
                          TERRESTRIAL_ITEM_H, p_cont, 0);
    MT_ASSERT(NULL!=p_ctrl[i]);
    ctrl_set_rstyle(p_ctrl[i], RSI_TERRESTRIAL_BTN_SH, RSI_TERRESTRIAL_BTN_HL, RSI_TERRESTRIAL_BTN_SH);
    text_set_font_style(p_ctrl[i], FSI_WHITE, FSI_BLACK, FSI_WHITE);
    text_set_content_type(p_ctrl[i], TEXT_STRTYPE_STRID);
    text_set_align_type(p_ctrl[i], STL_LEFT |STL_VCENTER);
    text_set_offset(p_ctrl[i], COMM_CTRL_OX, 0);
    text_set_content_by_strid(p_ctrl[i], stxt[i]);
   
    ctrl_set_related_id(p_ctrl[i],
                        0,                                                                      /* left */
                        (u8)((i - 1 + TERRESTRIAL_ITEM_CNT) % TERRESTRIAL_ITEM_CNT + 1),  /* up */
                        0,                                                                      /* right */
                        (u8)((i + 1) % TERRESTRIAL_ITEM_CNT + 1));                           /* down */

    y += TERRESTRIAL_ITEM_H + TERRESTRIAL_ITEM_V_GAP;
  }
  
  /* default focus on first option */
  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); 
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


BEGIN_KEYMAP(terrestrial_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_LEFT, MSG_SELECT)
  ON_EVENT(V_KEY_RIGHT, MSG_SELECT)
END_KEYMAP(terrestrial_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(terrestrial_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_SELECT, _ui_terrestrial_select)
END_MSGPROC(terrestrial_proc, ui_comm_root_proc)

