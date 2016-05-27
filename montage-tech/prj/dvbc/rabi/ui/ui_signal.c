/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_signal.h"

#include "ui_mute.h"

#ifdef ENABLE_CA
//ca
#include "cas_manager.h"
#include "ui_ca_public.h"
#endif

enum control_id
{
  IDC_INVALID = 0,
  IDC_CONTENT,
};

static u16 g_strID_CA = RSC_INVALID_ID;
static u16 g_strID_sig = RSC_INVALID_ID;
static u16 g_strID_userlock = RSC_INVALID_ID;
//static u16 g_strID_scramble = RSC_INVALID_ID;
static u16 g_strID_empty = RSC_INVALID_ID;

static void clear_signal_strid(u16 *str)
{
  *str = RSC_INVALID_ID;
  //OS_PRINTF("clear_signal_strid \n");
}

void clear_ca_strid(void)
{
  clear_signal_strid(&g_strID_CA);
}

static void set_signal_strid(u16 *str, u16 id)
{
  *str = id;
  //OS_PRINTF("set_signal_strid str[%d]=%d \n", *str, id);
}

u16 get_message_strid(void)
{
  u16 ret = RSC_INVALID_ID;

  if(CUSTOMER_ID == CUSTOMER_DTMB_DESAI_JIMO)
  {
    if(g_strID_sig != RSC_INVALID_ID)
	  return g_strID_sig;
  }

  if(g_strID_empty != RSC_INVALID_ID)
  {
    ret = g_strID_empty;
  }
  else if(g_strID_sig != RSC_INVALID_ID)
  {
    ret = g_strID_sig;
  }
  else if(g_strID_userlock != RSC_INVALID_ID)
  {
    ret = g_strID_userlock;
  }
  else if(g_strID_CA != RSC_INVALID_ID)
  {
#ifdef ENABLE_CA
    if(cas_manager_is_pvr_play() || ui_is_prog_encrypt())
#else
    if(ui_is_prog_encrypt())
#endif
    {
        ret = g_strID_CA;
    }
	else
	{
		switch(CUSTOMER_ID)
		{
			case CUSTOMER_DTMB_CHANGSHA_HHT:
				if(ui_is_playing() == FALSE)
					ret = g_strID_CA;
				break;
			default:
				break;
		}
	}
  }
  #if 0
  else if(g_strID_scramble != RSC_INVALID_ID)
  {
    ret = g_strID_scramble;
  }
#endif

  return ret;
}

static void update_signal_userlock_message(void)
{
  u16 pgid;
  u8 type = ui_get_lock_type();

  pgid = sys_status_get_curn_group_curn_prog_id();

  if(db_dvbs_get_count(ui_dbase_get_pg_view_id())>0)
  {
    clear_signal_strid(&g_strID_empty);
  }
  else if (!ui_is_playing())
  {
#ifdef ENABLE_CA
	if(FALSE == cas_manager_is_pvr_play())
#else    
    if(NULL == fw_find_root_by_id(ROOT_ID_PVR_PLAY_BAR))
#endif
      set_signal_strid(&g_strID_empty, IDS_MSG_NO_PROG);
#ifdef ENABLE_CA
	else//clear infor when pvr play mode
      clear_signal_strid(&g_strID_empty);
#endif
  }

  if(ui_signal_is_lock())
  {
    clear_signal_strid(&g_strID_sig);
  }
  else
  {
#ifdef ENABLE_CA
	if(FALSE == cas_manager_is_pvr_play())
#endif    
	{
		  #ifdef DESAI_52_CA
			  if(CUSTOMER_ID == CUSTOMER_DTMB_DESAI_JIMO)
			  	set_signal_strid(&g_strID_sig, IDS_DS_JIMO_NO_SIGNAL);
			  else
			  	set_signal_strid(&g_strID_sig, IDS_MSG_NO_SIGNAL);
		  #else
	      set_signal_strid(&g_strID_sig, IDS_MSG_NO_SIGNAL);
		  #endif
       }
#ifdef ENABLE_CA
	else//clear infor when pvr play mode
             clear_signal_strid(&g_strID_sig);
#endif
  }

  if(ui_signal_is_lock())
  {
    clear_signal_strid(&g_strID_userlock);
  }
  else
  {
    if(pgid != INVALIDID)
    {

	  if (ui_is_prog_locked(pgid) && IS_MASKED(type, LOCK_TYPE_CHNL_LOCK))//(db_dvbs_get_mark_status(view_id, pos, DB_DVBS_MARK_LCK, 0) == TRUE)
      {
      set_signal_strid(&g_strID_userlock, IDS_LOCK);
      }
      else if(get_message_strid() == IDS_LOCK || get_message_strid() == IDS_AGE_LIMIT)
      {
      clear_signal_strid(&g_strID_userlock);
      }
      else
      {
      clear_signal_strid(&g_strID_userlock);
      }
    }
  }
  #if 0//#ifdef CA_SUPPORT
  if(ui_is_prog_encrypt())
  {
    set_signal_strid(&g_strID_CA, get_cas_prompt_strid());
  }
  else
  {
    clear_signal_strid(&g_strID_CA);
  }
  #endif
}

extern BOOL _is_find_media_menu(void);
extern BOOL _is_find_vod_menu(void);
extern BOOL is_desktop_init(void);
void open_signal(u16 content)
{
  control_t *p_txt, *p_cont = NULL;
  //u16 left, top, width, height;
  u16 txt_x, txt_y, txt_w, txt_h;
  BOOL is_full;
  u8 focus_id = fw_get_focus_id();

  if(_is_find_media_menu()||_is_find_vod_menu())
  {
//	DEBUG(DBG, ERR, "in media menu or vod menu!!!\n");
	return;
  }
  if(!is_desktop_init())
  {
	DEBUG(DBG, ERR, "desktop not init!!!\n");
	return;
  }
  if(!ui_is_fullscreen_menu(focus_id))
  {
    return;
  }
  p_cont = fw_create_mainwin(ROOT_ID_SIGNAL,
                             SIGNAL_CONT_FULL_X, SIGNAL_CONT_FULL_Y,
                             SIGNAL_CONT_FULL_W, SIGNAL_CONT_FULL_H,
                             ROOT_ID_INVALID, 0,
                             OBJ_ATTR_INACTIVE, 0);
  is_full = TRUE;

  if(p_cont == NULL)
  {
    return;
  }

  ctrl_set_rstyle(p_cont, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);

  if(is_full)
  {
    txt_x = SIGNAL_TXT_FULL_X;
    txt_y = SIGNAL_TXT_FULL_Y;
    txt_w = SIGNAL_TXT_FULL_W;
    txt_h = SIGNAL_TXT_FULL_H;
  }
  else
  {
    txt_x = SIGNAL_TXT_PREVIEW_X;
    txt_y = SIGNAL_TXT_PREVIEW_Y;
    txt_w = SIGNAL_TXT_PREVIEW_W;
    txt_h = SIGNAL_TXT_PREVIEW_H;
  }

  p_txt = ctrl_create_ctrl(CTRL_TEXT, IDC_CONTENT,
                           txt_x, txt_y, txt_w, txt_h,
                           p_cont, 0);
  ctrl_set_rstyle(p_txt, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  text_set_align_type(p_txt, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_txt, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_txt, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_txt, content);

  ctrl_paint_ctrl(p_cont, FALSE);
  #ifdef ENABLE_CA
  cas_manage_finger_repaint();
  OS_PRINTF("function :%s ,redraw_finger_again\n",__FUNCTION__);
  #endif
}


void close_signal(void)
{
  fw_destroy_mainwin_by_id(ROOT_ID_SIGNAL);
}

static void refresh_signal()
{
  control_t *p_cont, *p_content;
  u16 strid;
  menu_attr_t *p_attr;

  p_attr = manage_get_curn_menu_attr();
  if(!ui_is_fullscreen_menu(p_attr->root_id)
    && !ui_is_preview_menu(p_attr->root_id)
    && !ui_is_popup_menu(p_attr->root_id))
  {
    return;
  }

  strid = get_message_strid();
  //OS_PRINTF("@ui_signal.c->refresh_signal.strid = [%d]\n",strid);//add log for fix bug 19913


  //OS_PRINTF("strid = %d\n", strid);

  if(strid == RSC_INVALID_ID)
  {
    close_signal();
  }
  else
  {

    //if(TRUE == cas_manage_check_ippv_dlg()) //ippv opened
    {
     // return;
    }
    if((p_cont = fw_find_root_by_id(ROOT_ID_SIGNAL)) != NULL)
    {
      p_content = ctrl_get_child_by_id(p_cont, IDC_CONTENT);
      if(p_content == NULL)
      {
        return;
      }

      if((u16)text_get_content(p_content) != strid)
      {
        text_set_content_by_strid(p_content, strid);
        ctrl_paint_ctrl(p_cont, TRUE);
      }
    }
    else
    {
      open_signal(strid);
    }
  }
}


void update_ca_message(u16 strid)
{
	DEBUG(CAS, INFO, "strid:%d\n", strid);  
	if(IDS_CA_E00 == strid || IDS_CA_E23 == strid)
	{
		if(fw_find_root_by_id(ROOT_ID_PASSWORD) != NULL && get_message_strid() == IDS_CA_E31) 
		{
			ui_comm_pwdlg_close();
		}
		clear_signal_strid(&g_strID_CA);
	}
	else
	{
		switch(CAS_ID)
		{
			case CONFIG_CAS_ID_TF:
			case CONFIG_CAS_ID_DS:
			case CONFIG_CAS_ID_DVBCA:
			case CONFIG_CAS_ID_DIVI:
				set_signal_strid(&g_strID_CA, strid);
				break;
			default:
				if(ui_is_prog_encrypt())
				{
					set_signal_strid(&g_strID_CA, strid);
				}
				else
				{
					//clear_signal_strid(&g_strID_CA);
				}
				break;
		}
	}

	refresh_signal();
}

void update_signal(void)
{
  OS_PRINTF("update signal\n");
  update_signal_userlock_message();

  refresh_signal();
}

#if 0
void update_encrypt_message()
{
  if(ui_is_playpg_scrambled() && ui_is_prog_encrypt())
  {
    set_signal_strid(&g_strID_scramble, IDS_MSG_ENCRYPT_PROG);
  }
  else
  {
    clear_signal_strid(&g_strID_scramble);
  }
  refresh_signal();
}

void update_encrypt_message_radio()
{
  if( ui_is_prog_encrypt())
  {
    set_signal_strid(&g_strID_scramble, IDS_MSG_ENCRYPT_PROG);
  }
  else
  {
    clear_signal_strid(&g_strID_scramble);
  }
  refresh_signal();
}
#endif

#if 0//#ifdef CA_SUPPORT
void clean_ca_prompt(void)
{
  clear_signal_strid(&g_strID_CA);
  clean_cas_prompt_strid();
}
#endif

#if 0
void clean_pg_scramble(void)
{
  clear_signal_strid(&g_strID_scramble);
  OS_PRINTF("@ui_signal.c->clean_pg_scramble = [%d]\n",g_strID_scramble);//add log for fix bug 19913
}
#endif
