/****************************************************************************

 ****************************************************************************/
#include "ui_common.h"

#include "ui_pic_play_mode_set.h"

enum control_id
{
  IDC_SLIDE_SHOW_STS = 1,
  IDC_SLIDE_SHOW_TIME,
  IDC_SLIDE_SHOW_EFFECT,
  IDC_SLIDE_SHOW_REPEAT,
};

static pic_showmode_t pic_showmode = {0, 5, FALSE, TRUE};
BOOL g_is_net = FALSE;

static u16 pic_play_mode_set_cont_keymap(u16 key);

static RET_CODE pic_play_mode_set_select_proc(control_t *p_ctrl,
                                      u16 msg,
                                      u32 para1,
                                      u32 para2);

static u16 pic_play_mode_set_select_keymap(u16 key);
RET_CODE pic_play_mode_cont_proc(control_t * p_ctrl,
  u16 msg, u32 para1, u32 para2);


void pic_play_update_slide_show(BOOL is_net)
{
  sys_status_get_pic_showmode(&pic_showmode, FALSE);
  g_is_net = FALSE;
  
}
  
BOOL pic_play_get_slide_show_sts(void)
{
  return pic_showmode.slide_show;
}

u8 pic_play_get_slide_interval(void)
{
  u8 offset = 0;
  
  if(g_is_net)
  {
    offset = 3;
  }
  else
  {
    offset = 1;
  }
  return pic_showmode.slide_time + offset;
}

BOOL pic_play_get_slide_repeat(void)
{
  return pic_showmode.slide_repeat;
}

BOOL pic_play_get_special_effect(void)
{
  return pic_showmode.special_effect;
}


RET_CODE open_pic_play_mode_set(u32 para1, u32 para2)
{
  control_t *p_cont, *p_ctrl[PIC_PLAY_MODE_SET_ITEM_CNT];
  u8 i, j;
  
  u16 stxt[PIC_PLAY_MODE_SET_ITEM_CNT] =
  {
    IDS_SLIDE_SHOW, IDS_SLIDE_SHOW_TIME, IDS_SPECCIAL_EFFECT, IDS_SLIDE_SHOW_REPEAT
  };
  u8 opt_cnt[PIC_PLAY_MODE_SET_ITEM_CNT] = {2, 9, 2, 2};
  u16 opt_data[PIC_PLAY_MODE_SET_ITEM_CNT][9] = {
    {IDS_OFF, IDS_ON},
    { IDS_ONE_SEC, IDS_TWO_SEC, IDS_THREE_SEC, IDS_FOUR_SEC,
      IDS_FIVE_SEC, IDS_SIX_SEC, IDS_SEVEN_SEC, IDS_EIGHT_SEC,
      IDS_NINE_SEC,
    },
    {IDS_OFF, IDS_ON},
    {IDS_OFF, IDS_ON},
  };
  u8 opt_cnt_net[PIC_PLAY_MODE_SET_ITEM_CNT] = {2, 7, 2, 2};
  u16 opt_data_net[PIC_PLAY_MODE_SET_ITEM_CNT][7] = {
    {IDS_OFF, IDS_ON},
    { IDS_THREE_SEC, IDS_FOUR_SEC,
      IDS_FIVE_SEC, IDS_SIX_SEC, IDS_SEVEN_SEC, IDS_EIGHT_SEC,
      IDS_NINE_SEC,
    },
    {IDS_OFF, IDS_ON},
    {IDS_OFF, IDS_ON},
  };
  u16 y, item_cnt, item_gap;
  u16 droplist_page[PIC_PLAY_MODE_SET_ITEM_CNT] = {5, 3};
  i = 0;
  p_cont = fw_create_mainwin(ROOT_ID_PIC_PLAY_MODE_SET,
                                  PIC_PLAY_MODE_SET_CONT_X, PIC_PLAY_MODE_SET_CONT_Y,
                                  PIC_PLAY_MODE_SET_CONT_W, PIC_PLAY_MODE_SET_CONT_H,
                                  para1, 0,
                                  OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
/*
  if( ROOT_ID_ALBUMS == para1 )
    g_is_net = TRUE;
  else
    g_is_net = FALSE;
*/
  //init global variable
  if(g_is_net)
  {
    y = PIC_PLAY_MODE_SET_ITEM_Y + 10;
    item_cnt = PIC_PLAY_MODE_SET_ITEM_CNT - 1;
    item_gap = 16;
  }
  else
  {
    y = PIC_PLAY_MODE_SET_ITEM_Y;
    item_cnt = PIC_PLAY_MODE_SET_ITEM_CNT;
    item_gap = 8;
  }
  pic_play_update_slide_show(g_is_net);

  
  //ctrl_set_rstyle(p_cont, RSI_PIC_PLAY_MODE_SET_FRM, RSI_PIC_PLAY_MODE_SET_FRM, RSI_PIC_PLAY_MODE_SET_FRM);  
 // ctrl_set_rstyle(p_cont, RSI_MAIN_BG1, RSI_MAIN_BG1, RSI_MAIN_BG1);
  ctrl_set_rstyle(p_cont,RSI_MEDIO_LEFT_BG,RSI_MEDIO_LEFT_BG,RSI_MEDIO_LEFT_BG);
  ctrl_set_keymap(p_cont, pic_play_mode_set_cont_keymap);
  ctrl_set_proc(p_cont, pic_play_mode_cont_proc);
  //OS_PRINTK("%%%%%%%%% in ui_pic_play_mode_set.c open_pic_play_mode_set 2000 i =%d,iabc=%d\n",i,iabc);
  for(i = 0; i < item_cnt; i++)
  {
  
    switch(i)
    {
      case 0:
      case 1:
      case 2:
      case 3:
        p_ctrl[i] = ui_comm_select_create(p_cont, (u8)(IDC_SLIDE_SHOW_STS + i),
                                          PIC_PLAY_MODE_SET_ITEM_X, y,
                                          PIC_PLAY_MODE_SET_ITEM_LW,
                                          PIC_PLAY_MODE_SET_ITEM_RW);
		
        ui_comm_select_set_static_txt(p_ctrl[i], stxt[i]);
        ui_comm_ctrl_set_keymap(p_ctrl[i], pic_play_mode_set_select_keymap);
        ui_comm_ctrl_set_proc(p_ctrl[i], pic_play_mode_set_select_proc);
        if(g_is_net)
        {
          ui_comm_select_set_param(p_ctrl[i], TRUE,
                                   CBOX_WORKMODE_STATIC, opt_cnt_net[i],
                                   CBOX_ITEM_STRTYPE_STRID, NULL);
          for (j = 0; j < opt_cnt_net[i]; j++)
          {
            ui_comm_select_set_content(p_ctrl[i], j, opt_data_net[i][j]);
          }
        }
        else
        {
          ui_comm_select_set_param(p_ctrl[i], TRUE,
                                   CBOX_WORKMODE_STATIC, opt_cnt[i],
                                   CBOX_ITEM_STRTYPE_STRID, NULL);
          for (j = 0; j < opt_cnt[i]; j++)
          {
            ui_comm_select_set_content(p_ctrl[i], j, opt_data[i][j]);
          }
        }
        ui_comm_select_create_droplist(p_ctrl[i], droplist_page[i]);
        break;
      default:
        break;
    }
	
    ctrl_set_related_id(p_ctrl[i],
    0,                                     /* left */
    (u8)((i - 1 + item_cnt) % item_cnt + 1),            /* up */
    0,                                     /* right */
    (u8)((i + 1) % item_cnt + 1)); /* down */
    y += PIC_PLAY_MODE_SET_ITEM_H + item_gap;
  }
  ui_comm_select_set_focus(p_ctrl[0], pic_showmode.slide_show);
  ui_comm_select_set_focus(p_ctrl[1], pic_showmode.slide_time);
  ui_comm_select_set_focus(p_ctrl[2], pic_showmode.special_effect);
  if(!g_is_net)
  {
    ui_comm_select_set_focus(p_ctrl[3], pic_showmode.slide_repeat);
  }
  ctrl_default_proc(p_ctrl[0], MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), FALSE);
  return SUCCESS;
}


static RET_CODE on_pic_play_mode_set_select_change(control_t *p_ctrl,
                                           u16 msg,
                                           u32 para1,
                                           u32 para2)
{
  RET_CODE ret = SUCCESS;
  u8 focus = (u8)(para2);

  switch(ctrl_get_ctrl_id(ctrl_get_parent(p_ctrl)))
  {
    case IDC_SLIDE_SHOW_STS:
      pic_showmode.slide_show = focus;
      break;
    case IDC_SLIDE_SHOW_TIME:
      pic_showmode.slide_time = focus;
      break;      
    case IDC_SLIDE_SHOW_EFFECT:
      pic_showmode.special_effect = focus;
      break;
    case IDC_SLIDE_SHOW_REPEAT:
      pic_showmode.slide_repeat = focus;
      break;
   default:
      MT_ASSERT(0);
      return ERR_FAILURE;
  }

  //sys_status_set_pic_showmode(&pic_showmode, g_is_net);
  sys_status_set_pic_showmode(&pic_showmode);
  sys_status_save();

  return ret;
}

static RET_CODE on_pic_play_mode_set_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_PIC_PLAY_MODE_SET, 0, 0);
  ctrl_paint_ctrl(ui_comm_root_get_root(ROOT_ID_USB_PICTURE),TRUE);
  return SUCCESS;
}

static RET_CODE on_pic_play_mode_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  manage_close_menu(ROOT_ID_PIC_PLAY_MODE_SET, 0, 0);
  ctrl_paint_ctrl(ui_comm_root_get_root(ROOT_ID_USB_PICTURE),TRUE);
  return SUCCESS;
}

static RET_CODE on_pic_play_mode_exit_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
  control_t * p_par_root =  fw_find_root_by_id(ROOT_ID_ALBUMS);
  u16 usb_focus = ui_get_usb_focus();
  if(p_par_root == NULL)
  {
		switch (usb_focus)
		{
			case 0:
				p_par_root = fw_find_root_by_id(ROOT_ID_USB_FILEPLAY);
			break;
			case 1:
				p_par_root = fw_find_root_by_id(ROOT_ID_USB_MUSIC);
			break;
			case 2:
				p_par_root = fw_find_root_by_id(ROOT_ID_USB_PICTURE);
			break;
			case 3:
				p_par_root = fw_find_root_by_id(ROOT_ID_RECORD_MANAGER);
			break;
			default:
				break;
		}
  }

  if(p_par_root)
  {
     fw_notify_root(p_par_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
  }
	
  return ERR_NOFEATURE;
}


BEGIN_KEYMAP(pic_play_mode_set_cont_keymap, ui_comm_root_keymap)
ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
ON_EVENT(V_KEY_MENU, MSG_CANCEL)
END_KEYMAP(pic_play_mode_set_cont_keymap, ui_comm_root_keymap)

BEGIN_KEYMAP(pic_play_mode_set_select_keymap, ui_comm_select_keymap)
END_KEYMAP(pic_play_mode_set_select_keymap, ui_comm_select_keymap)

BEGIN_MSGPROC(pic_play_mode_set_select_proc, cbox_class_proc)
  ON_COMMAND(MSG_CHANGED, on_pic_play_mode_set_select_change)
END_MSGPROC(pic_play_mode_set_select_proc, cbox_class_proc)

BEGIN_MSGPROC(pic_play_mode_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_PLUG_OUT, on_pic_play_mode_set_plug_out)  
  ON_COMMAND(MSG_CANCEL, on_pic_play_mode_exit)    
  ON_COMMAND(MSG_DESTROY, on_pic_play_mode_exit_destory)
END_MSGPROC(pic_play_mode_cont_proc, ui_comm_root_proc)


