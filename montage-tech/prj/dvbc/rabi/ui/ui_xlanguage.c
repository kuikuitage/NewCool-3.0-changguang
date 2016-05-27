/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"
#include "ui_xsys_set.h"
#include "ui_xlanguage.h"

enum control_id
{
  IDC_XINVALID = 0,
  IDC_XLANGUAGE,
  IDC_XFIRST_AUDIO,
  IDC_SECOND_AUDIO,
#if ENABLE_TTX_SUBTITLE
  IDC_XSUBTITLE,
  IDC_XTELETEXT,
#endif
  IDC_XFIRST_EPG,
  IDC_XSECOND_EPG,
  IDC_XDEFAULT_EPG,
};

u16 xlanguage_cont_keymap(u16 key);

RET_CODE xlanguage_select_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE xlanguage_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE ui_xlanguage_cont_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

RET_CODE open_xlanguage_set(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[LANGUAGE_ITEM_CNT];
  u8 i, j;
  language_set_t lang_set;
  epg_lang_t epg_set;

#if ENABLE_TTX_SUBTITLE
  u16 stxt[LANGUAGE_ITEM_CNT] =
  {
    IDS_LANGUAGE, IDS_FIRST_AUDIO, IDS_SECOND_AUDIO,
    IDS_SUBTITLE_LANGUAGE, IDS_TELETEXT, IDS_EPG_FIRST_LAN,
    IDS_EPG_SECOND_LAN, IDS_EPG_DEFAULT_LAN,
  };
#else
  u16 stxt[LANGUAGE_ITEM_CNT] =
  {
    IDS_LANGUAGE, IDS_FIRST_AUDIO, IDS_SECOND_AUDIO,
    IDS_EPG_FIRST_LAN, IDS_EPG_SECOND_LAN, IDS_EPG_DEFAULT_LAN
  };
#endif

  u8 opt_cnt = 2;
#ifdef LANGUAGE_SIMPLIFIED_CHINESE
  u16 opt_data[LANGUAGE_MAX_CNT] =
  {
    IDS_LANGUAGE_ENGLISH, IDS_LANGUAGE_TRADITIONAL_CHINESE, IDS_LANGUAGE_FRENCH, 
    IDS_LANGUAGE_GERMAN, IDS_LANGUAGE_ITALIAN, IDS_LANGUAGE_SPANISH, 
    IDS_LANGUAGE_PORTUGUESE, IDS_LANGUAGE_RUSSIAN, IDS_LANGUAGE_TURKISH, 
    IDS_LANGUAGE_POLAND ,IDS_LANGUAGE_ARABIC,IDS_LANGUAGE_FARSI,
  };
#else
  u16 opt_data[LANGUAGE_MAX_CNT] =
  {
    IDS_LANGUAGE_ENGLISH, IDS_LANGUAGE_FRENCH, IDS_LANGUAGE_GERMAN, 
    IDS_LANGUAGE_ITALIAN, IDS_LANGUAGE_SPANISH, IDS_LANGUAGE_PORTUGUESE, 
    IDS_LANGUAGE_RUSSIAN, IDS_LANGUAGE_TURKISH, IDS_LANGUAGE_POLAND ,
   IDS_LANGUAGE_ARABIC, IDS_LANGUAGE_FARSI,
  };
#endif
  u16 y;
  ui_epg_stop();
  ui_epg_release();

  sys_status_get_lang_set(&lang_set);
  sys_status_get_epg_lang(&epg_set);

#ifndef SPT_SUPPORT

 p_cont = ui_comm_right_root_create(ROOT_ID_XLANGUAGE, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);  
  if(p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, xlanguage_cont_keymap);
  ctrl_set_proc(p_cont, xlanguage_cont_proc);

  y = LANGUAGE_ITEM_Y;
  for(i = 0; i < LANGUAGE_ITEM_CNT ; i++)
  {
    p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_XLANGUAGE + i),
                                      UI_LANGUAGE_SET_X, y,
                                      UI_LANGUAGE_SET_W_L,
                                      UI_LANGUAGE_SET_W_R);
    ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
    ui_comm_ctrl_set_proc(p_ctrl[i], xlanguage_select_proc);

    ui_comm_select_set_param(p_ctrl[i], TRUE,
                             CBOX_WORKMODE_STATIC, opt_cnt,
                             CBOX_ITEM_STRTYPE_STRID, NULL);
    for(j = 0; j < opt_cnt; j++)
    {
      ui_comm_select_set_content(p_ctrl[i], j, opt_data[j]);
    }

    ui_comm_select_create_droplist(p_ctrl[i], COMM_SELECT_DROPLIST_PAGE);
    ctrl_set_related_id(p_ctrl[i],
                        0, /* left */
                        (u8)((i - 1 + LANGUAGE_ITEM_CNT) % LANGUAGE_ITEM_CNT + 1), /* up */
                        0, /* right */
                        (u8)((i + 1) % LANGUAGE_ITEM_CNT + 1)); /* down */

    y += LANGUAGE_ITEM_H + LANGUAGE_ITEM_V_GAP;
  }
#if 0
  for(i = LANGUAGE_ITEM_CNT - 3; i < LANGUAGE_ITEM_CNT; i++)
  {
    p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_XLANGUAGE + i),
                                      UI_LANGUAGE_SET_X, y,
                                      UI_LANGUAGE_SET_W_L,
                                      UI_LANGUAGE_SET_W_R);
    ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
    ui_comm_ctrl_set_proc(p_ctrl[i], xlanguage_select_proc);

    ui_comm_select_set_param(p_ctrl[i], TRUE,
                             CBOX_WORKMODE_STATIC, LANG_CODE,
                             CBOX_ITEM_STRTYPE_STRID, NULL);
    for(j = 0; j < LANG_CODE; j++)
    {
      u16 unistr[32] = {0};
      u16 uni_lang[7] = {' ', '(', 0, 0, 0, ')', 0};
      //u8 lang[4] = {0};
      u16 index = 0;

      iso_639_get_lang_code_name(&uni_lang[2], &index, j);
      gui_get_string(index, unistr, 32);
      uni_strcat(unistr, uni_lang, 32);
      
      ui_comm_select_set_content_by_unistr(p_ctrl[i], j, unistr);
    }

    ui_comm_select_create_droplist(p_ctrl[i], 3);
    ctrl_set_related_id(p_ctrl[i],
                        0, /* left */
                        (u8)((i - 1 + LANGUAGE_ITEM_CNT) % LANGUAGE_ITEM_CNT + 1), /* up */
                        0, /* right */
                        (u8)((i + 1) % LANGUAGE_ITEM_CNT + 1)); /* down */

    y += LANGUAGE_ITEM_H + LANGUAGE_ITEM_V_GAP;
  }
#endif
#else
#endif
  /* set focus according to current info */

  ui_comm_select_set_focus(p_ctrl[0], lang_set.osd_text);
  ui_comm_select_set_focus(p_ctrl[1], lang_set.first_audio);
  ui_comm_select_set_focus(p_ctrl[2], lang_set.second_audio);
#if ENABLE_TTX_SUBTITLE
  ui_comm_select_set_focus(p_ctrl[3], lang_set.sub_title);
  ui_comm_select_set_focus(p_ctrl[4], lang_set.tel_text);
#endif

  ui_comm_select_set_focus(p_ctrl[IDC_XFIRST_EPG - 1], epg_set.first_lang);
  
  ui_comm_select_set_focus(p_ctrl[IDC_XSECOND_EPG - 1], epg_set.second_lang);

  ui_comm_select_set_focus(p_ctrl[IDC_XDEFAULT_EPG - 1], epg_set.default_lang);

#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);

  return SUCCESS;
}


static RET_CODE on_select_change_focus(control_t *p_ctrl,
                                       u16 msg,
                                       u32 para1,
                                       u32 para2)
{
  control_t *p_root, *p_subctrl;
  language_set_t lang_set;
  epg_lang_t epg_set;
  u8 focus = (u8)para2;

  sys_status_get_lang_set(&lang_set);
  sys_status_get_epg_lang(&epg_set);
  switch(ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
  {
    case IDC_XLANGUAGE:
      lang_set.osd_text = focus;

      if(ctrl_is_onfocus(p_ctrl))
      {
        // reset all language setting
        lang_set.first_audio = focus;
        lang_set.second_audio = focus;
        lang_set.sub_title = focus;
        lang_set.tel_text = focus;
		epg_set.first_lang = focus;
		epg_set.second_lang = focus;
		epg_set.default_lang = focus;

        p_subctrl = ui_comm_root_get_ctrl(ROOT_ID_XLANGUAGE, IDC_XFIRST_AUDIO);
        ui_comm_select_set_focus(p_subctrl, focus);
        p_subctrl = ui_comm_root_get_ctrl(ROOT_ID_XLANGUAGE, IDC_SECOND_AUDIO);
        ui_comm_select_set_focus(p_subctrl, focus);
#if ENABLE_TTX_SUBTITLE
        p_subctrl = ui_comm_root_get_ctrl(ROOT_ID_XLANGUAGE, IDC_XSUBTITLE);
        ui_comm_select_set_focus(p_subctrl, focus);
        p_subctrl = ui_comm_root_get_ctrl(ROOT_ID_XLANGUAGE, IDC_XTELETEXT);
        ui_comm_select_set_focus(p_subctrl, focus);
#else
		p_subctrl = ui_comm_root_get_ctrl(ROOT_ID_XLANGUAGE, IDC_XFIRST_EPG);
        ui_comm_select_set_focus(p_subctrl, focus);
        p_subctrl = ui_comm_root_get_ctrl(ROOT_ID_XLANGUAGE, IDC_XSECOND_EPG);
        ui_comm_select_set_focus(p_subctrl, focus);
        p_subctrl = ui_comm_root_get_ctrl(ROOT_ID_XLANGUAGE, IDC_XDEFAULT_EPG);
        ui_comm_select_set_focus(p_subctrl, focus);
#endif
        // set language & redraw
        rsc_set_curn_language(gui_get_rsc_handle(), focus + 1);
        if((p_root = fw_get_focus()) != NULL)
        {
          gdi_set_enable(FALSE);
          //p_root = ctrl_get_child_by_id(p_root, IDC_COMM_ROOT_CONT);
          if(p_root != NULL)
          {
			sys_set_paint_left_cont();
            ctrl_paint_ctrl(p_subctrl->p_parent, TRUE);
          }
          gdi_set_enable(TRUE);
        }
      }
      break;
    case IDC_XFIRST_AUDIO:
      lang_set.first_audio = focus;
      break;
    case IDC_SECOND_AUDIO:
      lang_set.second_audio = focus;
      break;
#if ENABLE_TTX_SUBTITLE
    case IDC_XSUBTITLE:
      lang_set.sub_title = focus;
      break;
    case IDC_XTELETEXT:
      lang_set.tel_text = focus;
      break;  
#endif
	case IDC_XFIRST_EPG:
      epg_set.first_lang = focus;
      break;
    case IDC_XSECOND_EPG:
      epg_set.second_lang = focus;
      break;
    case IDC_XDEFAULT_EPG:
      epg_set.default_lang = focus;
      break;
    default:
      MT_ASSERT(0);
      return ERR_FAILURE;
  }
  sys_status_set_lang_set(&lang_set);
  sys_status_set_epg_lang(&epg_set);
  sys_status_save();
  return SUCCESS;
}

static RET_CODE on_language_destory(control_t *p_ctrl,
                                       u16 msg,
                                       u32 para1,
                                       u32 para2)
{
  epg_lang_t epg_set;
  char lang_code[3][3] = {{0}};
  
  ui_epg_init();
#ifndef IMPL_NEW_EPG
  ui_epg_start();
#else
  ui_epg_start(EPG_TABLE_SELECTE_PF_ALL);
#endif

  sys_status_get_epg_lang(&epg_set);

  iso_639_get_lang_code(lang_code[0], epg_set.first_lang);
  iso_639_get_lang_code(lang_code[1], epg_set.second_lang);
  iso_639_get_lang_code(lang_code[2], epg_set.default_lang);
#ifdef IMPL_NEW_EPG
  epg_db_set_lang_code(lang_code[0], lang_code[1], lang_code[2]);
#else
  ui_epg_set_lang_code(lang_code[0], lang_code[1], lang_code[2]);
#endif

  return ERR_NOFEATURE;
}
BEGIN_MSGPROC(xlanguage_cont_proc, ui_comm_root_proc)
ON_COMMAND(MSG_DESTROY, on_language_destory)
END_MSGPROC(xlanguage_cont_proc, ui_comm_root_proc)
BEGIN_MSGPROC(xlanguage_select_proc, cbox_class_proc)
ON_COMMAND(MSG_CHANGED, on_select_change_focus)
END_MSGPROC(xlanguage_select_proc, cbox_class_proc)

BEGIN_KEYMAP(xlanguage_cont_keymap, ui_comm_root_keymap)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(xlanguage_cont_keymap, ui_comm_root_keymap)
