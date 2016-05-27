/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "commonData.h"
#include "IIPTVDataProvider.h"

#include "ui_vdo_favorite.h"

#include "sys_status.h"
#include "ui_iptv_prot.h"
#include "ui_iptv_api.h"


enum favorite_local_msg
{
  MSG_SWITCH_CAPS = MSG_LOCAL_BEGIN + 200,
  MSG_LIST_DEL,
};

enum favorite_ctrl_id
{
  IDC_TITLE_CONT = 1,
  IDC_TITLE_BMP,
  IDC_TITLE_NAME,
  IDC_TITLE_NUM,
  IDC_LIST_CONT,
  IDC_VDO_FAVORITE_LIST,
  IDC_VDO_FAVORITE_SBAR,
  IDC_LIST_BOTTON,
  IDC_HELP_BOTTON,
  IDC_HELP_TEXT,
};

static list_xstyle_t favorite_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_MEDIO_LEFT_LIST_HL,
  RSI_MEDIO_LEFT_LIST_HL,
  RSI_MEDIO_LEFT_LIST_HL,
};

static list_xstyle_t favorite_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t favorite_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t favorite_list_attr[VDO_FAVORITE_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR | STL_RIGHT | STL_VCENTER,
    50, 15, 0, &favorite_list_field_rstyle,  &favorite_list_field_fstyle},
    
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    300, 75, 0, &favorite_list_field_rstyle,  &favorite_list_field_fstyle},
};

typedef struct
{
  u32 cus_livetv_pos;
  live_tv_fav_info cus_fav_livetv;
}cus_livetv_fav;

typedef struct
{
  u32 cus_iptv_pos;
  iptv_fav_info cus_fav_iptv;
}cus_iptv_fav;

static live_tv_fav live_fav_tv;
static iptv_fav ip_fav_tv;
static u32 group_focus;
static cus_livetv_fav cus_fav_live;
static cus_iptv_fav cus_fav_ip;

extern void convert_utf8_chinese_asc2unistr(u8 *asc_str, u16 *uni_str, int uni_str_len);


static u16 favorite_cont_keymap(u16 key);
static RET_CODE favorite_cont_proc(control_t *cont, u16 msg, u32 para1,
                             u32 para2);

static u16 favorite_list_keymap(u16 key);
static RET_CODE favorite_list_proc(control_t *cont, u16 msg, u32 para1,
                             u32 para2);

static u16 favorite_list_botton_keymap(u16 key);
static RET_CODE favorite_list_botton_proc(control_t *cont, u16 msg, u32 para1,
                             u32 para2);


static void favorite_update_page_num(control_t* p_ctrl, u16 current_pos, u16 total_cnt)
{
  char num[20] = {0};

  sprintf(num, "%d/%d", current_pos, total_cnt);
  text_set_content_by_ascstr(p_ctrl, num);
  
 // favorite_set_page_num(p_ctrl, curn_page, total_page);
  //ctrl_paint_ctrl(p_ctrl, TRUE);
}
  
static RET_CODE favorite_list_update(control_t* p_list, u16 start, u16 size, 
                                     u32 context)
{
  control_t *p_title_cont = NULL, *p_title_num;
  u8 i;
  u16 cnt = list_get_count(p_list);  
  u16 name[128] = {0};
  u8 asc_str[32];
  
  p_title_cont = ui_comm_root_get_ctrl(ROOT_ID_VDO_FAVORITE, IDC_TITLE_CONT);
  p_title_num = ctrl_get_child_by_id(p_title_cont,IDC_TITLE_NUM);

  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      sprintf(asc_str, "%d.", (start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);
      if(group_focus == 0)
      {
    //    OS_PRINTF("[%s],live_fav [%d] name [%s]\n",__FUNCTION__,i+start,live_fav_tv.live_fav_pg[i+start].live_fav_pg_name);
        convert_utf8_chinese_asc2unistr(live_fav_tv.live_fav_pg[i+start].live_fav_name, name, sizeof(live_fav_tv.live_fav_pg[i+start].live_fav_name));
        list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, name);
      }
      else
      {
       // convert_utf8_chinese_asc2unistr(ip_fav_tv.iptv_fav_pg[i+start].iptv_fav_name, name, sizeof(live_fav_tv.live_fav_pg[i+start].live_fav_name));
        list_set_field_content_by_unistr(p_list, (u16)(start + i), 1, ip_fav_tv.iptv_fav_pg[i+start].iptv_fav_name);
      }
    }
  }

  return SUCCESS;
}

RET_CODE open_vdo_favorite(u32 para1, u32 para2)
{
  control_t *p_cont;
  control_t *p_title_cont, *p_title_bmp, *p_title_name, *p_title_page,*p_title_num;
  control_t *p_list_cont,*p_list, *p_sbar,*p_list_botton;
  control_t *p_help_botton;
  u8 i;
  u8 *str_num = {"资源数:"};
  u8 *str_list_botton = {"清空"};
//  u8 *str_help_botton = {"收藏"};
  //u8 *str_help_text = {":取消收藏"};
  u8 asc_str[32] = {"[删除]键:取消喜爱节目"};
  u16 uni_str[32] = {0};
  u32 is_ip_tv = para1;

  group_focus = para1;
  sys_status_get_fav_livetv(&live_fav_tv);
  sys_status_get_fav_iptv(&ip_fav_tv);
  
  p_cont = fw_create_mainwin(ROOT_ID_VDO_FAVORITE,
                                  COMM_BG_X, COMM_BG_Y,
                                  COMM_BG_W, COMM_BG_H,
                                  ROOT_ID_INVALID, 0,
                                  OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }
  ctrl_set_keymap(p_cont, favorite_cont_keymap);
  ctrl_set_proc(p_cont, favorite_cont_proc);

 //title cont
  p_title_cont = ctrl_create_ctrl(CTRL_CONT, IDC_TITLE_CONT, VDO_FAVORITE_TITLE_CONTX,
                                  VDO_FAVORITE_TITLE_CONTY, VDO_FAVORITE_TITLE_CONTW, VDO_FAVORITE_TITLE_CONTH, p_cont, 0);
  ctrl_set_rstyle(p_title_cont, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);

  //bmp 
  p_title_bmp = ctrl_create_ctrl(CTRL_BMAP, IDC_TITLE_BMP, 
                                      VDO_FAVORITE_TITLE_BMPX, VDO_FAVORITE_TITLE_BMPY,
                                      VDO_FAVORITE_TITLE_BMPW, VDO_FAVORITE_TITLE_BMPH,
                                      p_title_cont, 0);
 // bmap_set_content_by_id(p_title_bmp, IM_ICON_FAV);
 // 
  p_title_name = ctrl_create_ctrl(CTRL_TEXT, IDC_TITLE_NAME, VDO_FAVORITE_TITLE_TEXTX,
                                  VDO_FAVORITE_TITLE_TEXTY, VDO_FAVORITE_TITLE_TEXTW, VDO_FAVORITE_TITLE_TEXTH, p_title_cont, 0);
  ctrl_set_rstyle(p_title_name, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_title_name, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_title_name, FSI_WHITE_28, FSI_WHITE_28, FSI_WHITE_28);
  text_set_content_type(p_title_name, TEXT_STRTYPE_STRID);
  text_set_content_by_strid(p_title_name, IDS_HD_FAVORITE);
  
 // 
  p_title_page = ctrl_create_ctrl(CTRL_TEXT, 0, VDO_FAVORITE_PAGE_X,
                                  VDO_FAVORITE_PAGE_Y, VDO_FAVORITE_PAGE_W, VDO_FAVORITE_PAGE_H, p_title_cont, 0);
  ctrl_set_rstyle(p_title_page, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_title_page, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_title_page, TEXT_STRTYPE_UNICODE);
  convert_gb2312_chinese_asc2unistr(str_num, uni_str, sizeof(uni_str));
  text_set_content_by_unistr(p_title_page, uni_str);
  p_title_num= ctrl_create_ctrl(CTRL_TEXT, IDC_TITLE_NUM, VDO_FAVORITE_NUM_X,
                                  VDO_FAVORITE_NUM_Y, VDO_FAVORITE_NUM_W, VDO_FAVORITE_NUM_H, p_title_cont, 0);
  ctrl_set_rstyle(p_title_num, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_align_type(p_title_num, STL_LEFT | STL_VCENTER);
  text_set_font_style(p_title_num, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_title_num, TEXT_STRTYPE_UNICODE);
  if(!is_ip_tv)
  {
    if(live_fav_tv.live_tv_total_num > 0)
    {
     favorite_update_page_num(p_title_num, 1, live_fav_tv.live_tv_total_num);
    }
    else
    {
     favorite_update_page_num(p_title_num, 0, live_fav_tv.live_tv_total_num);
    }
  }
  else
  {
    if(ip_fav_tv.iptv_total_num > 0)
    {
      favorite_update_page_num(p_title_num, 1, ip_fav_tv.iptv_total_num); 
    }
    else
    {
      favorite_update_page_num(p_title_num, 0, ip_fav_tv.iptv_total_num); 
    }
  }
  //list cont
  p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_LIST_CONT, VDO_FAVORITE_LIST_CONTX,
                                  VDO_FAVORITE_LIST_CONTY, VDO_FAVORITE_LIST_CONTW, VDO_FAVORITE_LIST_CONTH, p_cont, 0);
ctrl_set_rstyle(p_list_cont, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);
//List
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_VDO_FAVORITE_LIST, 
    VDO_FAVORITE_LIST_X, VDO_FAVORITE_LIST_Y, 
    VDO_FAVORITE_LIST_W, VDO_FAVORITE_LIST_H, p_list_cont, 0);
  ctrl_set_rstyle(p_list, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);
  ctrl_set_keymap(p_list, favorite_list_keymap);
  ctrl_set_proc(p_list, favorite_list_proc);
  ctrl_set_mrect(p_list, VDO_FAVORITE_LIST_MID_L, VDO_FAVORITE_LIST_MID_T,
    VDO_FAVORITE_LIST_MID_W + VDO_FAVORITE_LIST_MID_L, VDO_FAVORITE_LIST_MID_H + VDO_FAVORITE_LIST_MID_T);
  list_set_item_interval(p_list, VDO_FAVORITE_LIST_ITEM_VGAP);
  list_set_item_rstyle(p_list, &favorite_list_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  if(!is_ip_tv)
  {
    list_set_count(p_list, live_fav_tv.live_tv_total_num, VDO_FAVORITE_LIST_PAGE);
  }
  else
  {
    list_set_count(p_list, ip_fav_tv.iptv_total_num, VDO_FAVORITE_LIST_PAGE);
  }
  list_set_focus_pos(p_list, 0);
  list_set_field_count(p_list, VDO_FAVORITE_LIST_FIELD, VDO_FAVORITE_LIST_PAGE);
  list_set_update(p_list, favorite_list_update, 0);
  for (i = 0; i < VDO_FAVORITE_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(favorite_list_attr[i].attr), 
      (u16)(favorite_list_attr[i].width),
      (u16)(favorite_list_attr[i].left), 
      (u8)(favorite_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, favorite_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, favorite_list_attr[i].fstyle);
  }    
  p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_VDO_FAVORITE_SBAR, 
    VDO_FAVORITE_SBAR_X, VDO_FAVORITE_SBAR_Y, 
    VDO_FAVORITE_SBAR_WIDTH, VDO_FAVORITE_SBAR_HEIGHT, 
    p_list_cont, 0);
  ctrl_set_rstyle(p_sbar, RSI_OTT_SCROLL_BAR_BG, RSI_OTT_SCROLL_BAR_BG, RSI_OTT_SCROLL_BAR_BG);
  sbar_set_autosize_mode(p_sbar, 1);
  sbar_set_direction(p_sbar, 0);
  sbar_set_mid_rstyle(p_sbar, RSI_OTT_SCROLL_BAR,
                     RSI_OTT_SCROLL_BAR, RSI_OTT_SCROLL_BAR);

  list_set_scrollbar(p_list, p_sbar);
  
  favorite_list_update(p_list, list_get_valid_pos(p_list), VDO_FAVORITE_LIST_PAGE, 0);

// list botton
  p_list_botton = ctrl_create_ctrl(CTRL_TEXT, IDC_LIST_BOTTON,
                                     VDO_FAVORITE_LIST_BOTTON_X, VDO_FAVORITE_LIST_BOTTON_Y,
                                     VDO_FAVORITE_LIST_BOTTON_W, VDO_FAVORITE_LIST_BOTTON_H,
                                     p_list_cont, 0);
  ctrl_set_rstyle(p_list_botton, RSI_OTT_BUTTON_SH, RSI_OTT_BUTTON_HL, RSI_OTT_BUTTON_SH); //color and font color
  ctrl_set_keymap(p_list_botton, favorite_list_botton_keymap);
  ctrl_set_proc(p_list_botton, favorite_list_botton_proc);
  text_set_align_type(p_list_botton, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_list_botton, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_list_botton, TEXT_STRTYPE_UNICODE);
  convert_gb2312_chinese_asc2unistr(str_list_botton, uni_str, sizeof(uni_str));
  text_set_content_by_unistr(p_list_botton, uni_str);

// list botton
  p_help_botton = ctrl_create_ctrl(CTRL_TEXT, IDC_HELP_BOTTON,
                                     VDO_FAVORITE_HELP_BOTTON_X, VDO_FAVORITE_HELP_BOTTON_Y,
                                     VDO_FAVORITE_HELP_BOTTON_W, VDO_FAVORITE_HELP_BOTTON_H,
                                     p_cont, 0);
  ctrl_set_rstyle(p_help_botton, RSI_PBACK, RSI_PBACK, RSI_PBACK); //color and font color
  ctrl_set_attr(p_help_botton, OBJ_ATTR_ACTIVE);
  text_set_align_type(p_help_botton, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_help_botton, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_help_botton, TEXT_STRTYPE_UNICODE);
  convert_gb2312_chinese_asc2unistr(asc_str, uni_str, sizeof(uni_str));
  text_set_content_by_unistr(p_help_botton, uni_str);

 /* p_help_text= ctrl_create_ctrl(CTRL_TEXT, IDC_HELP_TEXT,
                                     VDO_FAVORITE_HELP_TEXT_X, VDO_FAVORITE_HELP_TEXT_Y,
                                     VDO_FAVORITE_HELP_TEXT_W, VDO_FAVORITE_HELP_TEXT_H,
                                     p_cont, 0);
  text_set_align_type(p_help_text, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_help_text, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_help_text, TEXT_STRTYPE_UNICODE);
  convert_gb2312_chinese_asc2unistr(str_help_text, uni_str, sizeof(uni_str));
  text_set_content_by_unistr(p_help_text, uni_str);*/
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ui_iptv_set_flag_to_plid(FALSE);
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0); 
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

  return SUCCESS;
}


static RET_CODE on_favorite_list_yes(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  	
  u16 pos, cnt;
  live_tv_fav_info fav_livetv_info;
  iptv_fav_info fav_iptv_info;
  ui_iptv_description_param_t p_param;
  u16 iptv_dp_state;

  cnt = list_get_count(p_ctrl);
  if (cnt <= 0)
  {
    return SUCCESS;
  }

  pos = list_get_focus_pos(p_ctrl);

  if(group_focus == 0)
  {
    sys_status_get_fav_livetv_info( pos, &fav_livetv_info);
    memcpy(&(cus_fav_live.cus_fav_livetv),&fav_livetv_info,sizeof(live_tv_fav_info));
    cus_fav_live.cus_livetv_pos = pos;
    manage_open_menu(ROOT_ID_LIVE_TV, (u32)&fav_livetv_info, pos);
  }
  else
  {
  #ifndef WIN32
    iptv_dp_state = ui_iptv_get_dp_state();
  #endif
    if (iptv_dp_state == IPTV_DP_STATE_UNINIT || iptv_dp_state == IPTV_DP_STATE_INITING)
    {
      ui_comm_cfmdlg_open(NULL, IDS_HD_NETWORK_BUSY_PLS_WAIT, NULL, 2000);
      return SUCCESS;
    }

    sys_status_get_fav_iptv_info(pos, &fav_iptv_info);
    // TODO: favorite   video id SY
  // p_param.vdo_id = fav_iptv_info.iptv_fav_id;
    memcpy(p_param.vdo_id.qpId, fav_iptv_info.iptv_fav_vdo_id.qpId, strlen(fav_iptv_info.iptv_fav_vdo_id.qpId)+1);
    memcpy(p_param.vdo_id.tvQid, fav_iptv_info.iptv_fav_vdo_id.tvQid, strlen(fav_iptv_info.iptv_fav_vdo_id.tvQid)+1);
    memcpy(p_param.vdo_id.sourceCode, fav_iptv_info.iptv_fav_vdo_id.sourceCode, strlen(fav_iptv_info.iptv_fav_vdo_id.sourceCode)+1);
    p_param.vdo_id.type= fav_iptv_info.iptv_fav_vdo_id.type;
    p_param.b_single_page = fav_iptv_info.iptv_fav_vdo_id.type;
    p_param.res_id = fav_iptv_info.iptv_fav_category_id;
    memcpy(&(cus_fav_ip.cus_fav_iptv),&fav_iptv_info,sizeof(iptv_fav_info));
    cus_fav_ip.cus_iptv_pos = pos;
    DEBUG(UI_IPLAY_FAV,INFO,"%s pos = %d ...\n",__FUNCTION__,pos);
    DEBUG(UI_IPLAY_FAV,INFO,"%s qoId = %s tvQid = %s sourceCode = %s type = %d \n",__FUNCTION__,p_param.vdo_id.qpId, p_param.vdo_id.tvQid, p_param.vdo_id.sourceCode,p_param.vdo_id.type);
    manage_open_menu(ROOT_ID_IPTV_DESCRIPTION,(u32)(&p_param),(u32)ROOT_ID_VDO_FAVORITE);
  }
 
  return SUCCESS;
}

static RET_CODE on_favorite_list_delete(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  	
  control_t *p_title_cont = NULL, *p_title_num;
  u16 pos = 0, cnt = 0;

  cnt = list_get_count(p_ctrl);
  if (cnt <= 0)
  {
    return SUCCESS;
  }
  
  p_title_cont = ui_comm_root_get_ctrl(ROOT_ID_VDO_FAVORITE, IDC_TITLE_CONT);
  p_title_num = ctrl_get_child_by_id(p_title_cont, IDC_TITLE_NUM);
  pos = list_get_focus_pos(p_ctrl);
  if(group_focus == 0)
  {
    sys_status_del_fav_livetv_info(pos);
    sys_status_save();
    sys_status_get_fav_livetv(&live_fav_tv);
    list_set_count(p_ctrl, live_fav_tv.live_tv_total_num, VDO_FAVORITE_LIST_PAGE);
  }
  else
  {
    sys_status_del_fav_iptv_info(pos);
    sys_status_save();
    sys_status_get_fav_iptv(&ip_fav_tv);
    list_set_count(p_ctrl, ip_fav_tv.iptv_total_num, VDO_FAVORITE_LIST_PAGE);
  }

  favorite_list_update(p_ctrl, list_get_valid_pos(p_ctrl), VDO_FAVORITE_LIST_PAGE, 0);
  cnt = list_get_count(p_ctrl);
  if(cnt == 0)
  {
    favorite_update_page_num(p_title_num, 0, cnt);
    list_set_focus_pos(p_ctrl, 0);
  }
  else if(cnt == pos)
  {
   pos = pos - 1;
   list_set_focus_pos(p_ctrl, pos);
   favorite_update_page_num(p_title_num, pos+1, cnt);
  }
  else
  {
   list_set_focus_pos(p_ctrl, pos);
   favorite_update_page_num(p_title_num, pos+1, cnt);
  }

  ctrl_paint_ctrl(p_title_num,TRUE);
  ctrl_paint_ctrl(p_ctrl->p_parent,TRUE);
  return SUCCESS;
}

static RET_CODE on_favorite_focus_up(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  	
  control_t *p_title_cont = NULL, *p_title_num;
  u16 pos = 0, cnt = 0;

  p_title_cont = ui_comm_root_get_ctrl(ROOT_ID_VDO_FAVORITE, IDC_TITLE_CONT);
  p_title_num = ctrl_get_child_by_id(p_title_cont, IDC_TITLE_NUM);
  
  pos = list_get_focus_pos(p_ctrl);
  cnt = list_get_count(p_ctrl);
   if((pos == 0)||(cnt == 0))
  {
   	pos = cnt;
  }
  
  favorite_update_page_num(p_title_num, pos, cnt);
  ctrl_paint_ctrl(p_title_num,TRUE);
  list_class_proc(p_ctrl,msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_favorite_focus_down(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  	
  control_t *p_list_cont = NULL, *p_list_botton, *p_title_cont, *p_title_num;
  u16 pos = 0, cnt = 0;

  p_list_cont = ctrl_get_parent(p_ctrl);
  p_list_botton = ctrl_get_child_by_id(p_list_cont,IDC_LIST_BOTTON);

  p_title_cont = ui_comm_root_get_ctrl(ROOT_ID_VDO_FAVORITE, IDC_TITLE_CONT);
  p_title_num = ctrl_get_child_by_id(p_title_cont, IDC_TITLE_NUM);
  
  pos = list_get_focus_pos(p_ctrl);
  cnt = list_get_count(p_ctrl);
  if(pos == cnt - 1)
  {
    ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
    ctrl_process_msg(p_list_botton, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(p_ctrl,TRUE);
    ctrl_paint_ctrl(p_list_botton,TRUE);
	return ERR_FAILURE;
  }

  pos += 1+1;
  if(0==cnt)
  	pos = 0;
  favorite_update_page_num(p_title_num, pos, cnt);
  ctrl_paint_ctrl(p_title_num,TRUE);
  list_class_proc(p_ctrl,msg, para1, para2);
  return SUCCESS;
}

static RET_CODE on_favorite_page_up_down(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  	
  control_t  *p_title_cont, *p_title_num;
  u16 pos = 0, cnt = 0;

  p_title_cont = ui_comm_root_get_ctrl(ROOT_ID_VDO_FAVORITE, IDC_TITLE_CONT);
  p_title_num = ctrl_get_child_by_id(p_title_cont, IDC_TITLE_NUM);

  list_class_proc(p_ctrl,msg, para1, para2);

  pos = list_get_focus_pos(p_ctrl);
  cnt = list_get_count(p_ctrl);

  favorite_update_page_num(p_title_num, pos+1, cnt);
  ctrl_paint_ctrl(p_title_num,TRUE);
  return SUCCESS;
}

static RET_CODE on_favorite_list_botton_down(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  	
  control_t *p_list_cont = NULL, *p_list = NULL, *p_title_cont, *p_title_num;
  u16 cnt = 0;
  p_list_cont = ctrl_get_parent(p_ctrl);
  p_list = ctrl_get_child_by_id(p_list_cont,IDC_VDO_FAVORITE_LIST);
  
  p_title_cont = ui_comm_root_get_ctrl(ROOT_ID_VDO_FAVORITE, IDC_TITLE_CONT);
  p_title_num = ctrl_get_child_by_id(p_title_cont, IDC_TITLE_NUM);
  
  cnt = list_get_count(p_list);
  if(cnt > 0)
  {
    favorite_update_page_num(p_title_num, 1, cnt);
    
    ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
    ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
    list_set_focus_pos(p_list, 0);
    ctrl_paint_ctrl(p_title_num,TRUE);
    ctrl_paint_ctrl(p_list,TRUE);
    ctrl_paint_ctrl(p_ctrl,TRUE);
  }
  return SUCCESS;
}

static RET_CODE on_favorite_list_botton_up(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  	
  control_t *p_list_cont = NULL, *p_list;
  u16 cnt;

  p_list_cont = ctrl_get_parent(p_ctrl);
  p_list = ctrl_get_child_by_id(p_list_cont,IDC_VDO_FAVORITE_LIST);
  cnt = list_get_count(p_list);

  if(cnt > 0)
  {
    ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
    ctrl_process_msg(p_list, MSG_GETFOCUS, 0, 0);
    list_set_focus_pos(p_list, cnt - 1);
    ctrl_paint_ctrl(p_list,TRUE);
    ctrl_paint_ctrl(p_ctrl,TRUE);
  }
  return SUCCESS;
}

static RET_CODE on_favorite_list_botton_clear(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  	
  control_t *p_list_cont = NULL, *p_list = NULL, *p_title_cont, *p_title_num;
  u32 cnt = 0;

  p_title_cont = ui_comm_root_get_ctrl(ROOT_ID_VDO_FAVORITE, IDC_TITLE_CONT);
  p_list_cont = ctrl_get_parent(p_ctrl);
  p_list = ctrl_get_child_by_id(p_list_cont,IDC_VDO_FAVORITE_LIST);
  p_title_num = ctrl_get_child_by_id(p_title_cont, IDC_TITLE_NUM);
  cnt = list_get_count(p_list);
  switch(group_focus)
  {
    case 0:
     memset(&live_fav_tv,0,sizeof(live_fav_tv));
     sys_status_set_fav_livetv(&live_fav_tv);  
     list_set_count(p_list,  live_fav_tv.live_tv_total_num, VDO_FAVORITE_LIST_PAGE);
      break;
    case 1:
      memset(&ip_fav_tv,0,sizeof(ip_fav_tv));
      sys_status_set_fav_iptv(&ip_fav_tv);
      list_set_count(p_list,  ip_fav_tv.iptv_total_num, VDO_FAVORITE_LIST_PAGE);
      break;
    default:
      break;
  }

  sys_status_save();
  list_set_focus_pos(p_list, 0);
  favorite_list_update(p_list, list_get_valid_pos(p_list), VDO_FAVORITE_LIST_PAGE, 0);
  favorite_update_page_num(p_title_num,0,0);

  ctrl_paint_ctrl(p_title_num, TRUE);
  ctrl_paint_ctrl(p_list_cont,TRUE);
  ctrl_paint_ctrl(p_ctrl,TRUE);
  return SUCCESS;
}

static RET_CODE on_favorite_destory(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;

    ui_iptv_set_flag_to_plid(TRUE);
    p_root = fw_find_root_by_id(ROOT_ID_IPTV);
    if (p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
    }
    return ERR_NOFEATURE;
}

static RET_CODE on_favorite_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_title_cont = NULL;
    control_t *p_title_num = NULL;
    control_t *p_list_cont = NULL;
    control_t *p_list = NULL;
    live_tv_fav_info fav_livetv_info;
    u16 pos;
  
    p_title_cont = ui_comm_root_get_ctrl(ROOT_ID_VDO_FAVORITE, IDC_TITLE_CONT);
    p_list_cont = ui_comm_root_get_ctrl(ROOT_ID_VDO_FAVORITE, IDC_LIST_CONT);
    p_title_num = ctrl_get_child_by_id(p_title_cont, IDC_TITLE_NUM);
    p_list = ctrl_get_child_by_id(p_list_cont,IDC_VDO_FAVORITE_LIST);

    if(!group_focus)
    {
      sys_status_get_fav_livetv_info( cus_fav_live.cus_livetv_pos, &fav_livetv_info);
      if(fav_livetv_info.live_fav_id != cus_fav_live.cus_fav_livetv.live_fav_id)
      {
		  DEBUG(DBG, INFO, "\n");
       sys_status_get_fav_livetv(&live_fav_tv);
       list_set_count(p_list, live_fav_tv.live_tv_total_num, VDO_FAVORITE_LIST_PAGE);
       favorite_update_page_num(p_title_num,0,live_fav_tv.live_tv_total_num);
       list_set_focus_pos(p_list, 0);
       favorite_list_update(p_list, list_get_valid_pos(p_list), VDO_FAVORITE_LIST_PAGE, 0);
      }
      else
      {
		  DEBUG(DBG, INFO, "\n");
       sys_status_get_fav_livetv(&live_fav_tv);
       list_set_count(p_list, live_fav_tv.live_tv_total_num, VDO_FAVORITE_LIST_PAGE);
       favorite_update_page_num(p_title_num, cus_fav_live.cus_livetv_pos+1,live_fav_tv.live_tv_total_num);
       list_set_focus_pos(p_list, cus_fav_live.cus_livetv_pos);
       favorite_list_update(p_list, list_get_valid_pos(p_list), VDO_FAVORITE_LIST_PAGE, 0);
      }
    }
    else
    {
        sys_status_get_fav_iptv(&ip_fav_tv);
        if (ip_fav_tv.iptv_total_num > 0)
        {
            pos = list_get_focus_pos(p_list);
            if (pos >= ip_fav_tv.iptv_total_num)
            {
                pos = ip_fav_tv.iptv_total_num  - 1;
            }
        	DEBUG(DBG, INFO, "\n");
            list_set_count(p_list, ip_fav_tv.iptv_total_num, VDO_FAVORITE_LIST_PAGE);
            list_set_focus_pos(p_list, pos);
            favorite_list_update(p_list, list_get_valid_pos(p_list), VDO_FAVORITE_LIST_PAGE, 0);
            favorite_update_page_num(p_title_num, pos+1, ip_fav_tv.iptv_total_num);
        }
        else
        {
            list_set_count(p_list, ip_fav_tv.iptv_total_num, VDO_FAVORITE_LIST_PAGE);
            favorite_update_page_num(p_title_num, 0, ip_fav_tv.iptv_total_num);
        	DEBUG(DBG, INFO, "\n");
        	list_set_focus_pos(p_list, 0);
        }
    }

    ctrl_paint_ctrl(ctrl_get_root(p_ctrl), TRUE);
    
    return ERR_NOFEATURE;
}

BEGIN_KEYMAP(favorite_cont_keymap, ui_comm_root_keymap)
	ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
END_KEYMAP(favorite_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(favorite_cont_proc, ui_comm_root_proc)
   ON_COMMAND(MSG_DESTROY, on_favorite_destory)
   ON_COMMAND(MSG_UPDATE, on_favorite_update)
END_MSGPROC(favorite_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(favorite_list_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_OK, MSG_YES)
    ON_EVENT(V_KEY_HOT_XDEL, MSG_LIST_DEL)
END_KEYMAP(favorite_list_keymap, NULL)

BEGIN_MSGPROC(favorite_list_proc, list_class_proc)
    ON_COMMAND(MSG_YES, on_favorite_list_yes)
    ON_COMMAND(MSG_LIST_DEL, on_favorite_list_delete)
    ON_COMMAND(MSG_FOCUS_UP, on_favorite_focus_up)
    ON_COMMAND(MSG_FOCUS_DOWN, on_favorite_focus_down)
       ON_COMMAND(MSG_PAGE_UP, on_favorite_page_up_down)
    ON_COMMAND(MSG_PAGE_DOWN, on_favorite_page_up_down)
END_MSGPROC(favorite_list_proc, list_class_proc)

BEGIN_KEYMAP(favorite_list_botton_keymap, NULL)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
       ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(favorite_list_botton_keymap, NULL)

BEGIN_MSGPROC(favorite_list_botton_proc, ctrl_default_proc)
	ON_COMMAND(MSG_FOCUS_DOWN, on_favorite_list_botton_down)
	ON_COMMAND(MSG_FOCUS_UP, on_favorite_list_botton_up)
	ON_COMMAND(MSG_SELECT, on_favorite_list_botton_clear)
END_MSGPROC(favorite_list_botton_proc, ctrl_default_proc)

