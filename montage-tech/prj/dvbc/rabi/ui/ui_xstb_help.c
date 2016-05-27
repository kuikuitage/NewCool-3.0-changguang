/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "ui_xsys_set.h"
#include "ui_xstb_help.h"

enum control_id
{
  IDC_XINVALID = 0,
  IDC_XSTB_HELP_IC,
  IDC_XSTB_HELP_TITLE,
  IDC_XSTB_HELP_KEY,
  IDC_XSTB_HELP_KEY_LIST,
  IDC_XSTB_HELP_FN,
  IDC_XSTB_HELP_FN_INFO,
  IDC_XSTB_HELP_SEARCH,
  IDC_XSTB_HELP_SEARCH_LIST,
  IDC_XSTB_HELP_TIME_SETTING,
  IDC_XSTB_HELP_TIME_SETTING_INFO1,
  IDC_XSTB_HELP_TIME_SETTING_INFO2,
  IDC_XSTB_HELP_NET,
  IDC_XSTB_HELP_NET_INFO1,
  IDC_XSTB_HELP_NET_INFO2,
};

static list_xstyle_t xhelp_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
};

static list_xstyle_t xhelp_list_field_fstyle =
{
  FSI_WHITE_18,
  FSI_WHITE_18,
  FSI_WHITE_18,
  FSI_WHITE_18,
  FSI_WHITE_18,
};

static list_xstyle_t xhelp_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t xhelp_key_list_attr[XSTB_HELP_KEY_LIST_FIELD] =
{
  { LISTFIELD_TYPE_ICON,
    45, 5, 0, &xhelp_list_field_rstyle,  &xhelp_list_field_fstyle},
  { LISTFIELD_TYPE_STRID| STL_LEFT | STL_VCENTER,
    XSTB_HELP_KEY_LIST_W - 50,50, 0, &xhelp_list_field_rstyle,  &xhelp_list_field_fstyle},    
};
static list_field_attr_t xhelp_search_list_attr[XSTB_HELP_SEARCH_LIST_FIELD] =
{
  { LISTFIELD_TYPE_STRID| STL_LEFT | STL_VCENTER,
    XSTB_HELP_SEARCH_LIST_W,0, 0, &xhelp_list_field_rstyle,  &xhelp_list_field_fstyle},    
};

static RET_CODE xhelp_key_list_update(control_t* p_list, u16 start, u16 size, 
                                 u32 context)
{
  u8 i;
  u16 str_id[XSTB_HELP_KEY_LIST_CNT] = {
    IDS_HELP_KEY_PAGE,
    IDS_HELP_KEY_MENU,
    IDS_HELP_KEY_BACK ,
    IDS_HELP_KEY_REFRESH,
    IDS_HELP_KEY_SAVE ,
    IDS_HELP_KEY_DEL,
    IDS_HELP_KEY_FN,};

    u16 bmp_id[XSTB_HELP_KEY_LIST_CNT] = {
    IM_KEY_IC_NEXT,
    IM_KEY_IC_MENU,
    IM_KEY_IC_BACK,
    IM_KEY_IC_REFRESH,
    IM_KEY_IC_SAVE,
    IM_KEY_IC_DEL,
    IM_KEY_IC_FN,};
  for(i = 0; i < XSTB_HELP_KEY_LIST_CNT; i++)
  {
    list_set_field_content_by_icon(p_list, i, 0, bmp_id[i]);
  }
  for(i = 0; i < XSTB_HELP_KEY_LIST_CNT; i++)
  {
    list_set_field_content_by_strid(p_list, i,1, str_id[i]);
  }
  return SUCCESS;
}

static RET_CODE xhelp_search_list_update(control_t* p_list, u16 start, u16 size, 
                                 u32 context)
{
  u8 i;
  u16 str_id[XSTB_HELP_SEARCH_LIST_CNT] = {
    IDS_HELP_SEARCH_INFO_ROW1,
    IDS_HELP_SEARCH_INFO_ROW2,
    IDS_HELP_SEARCH_INFO_ROW3,};

  for(i = 0; i < XSTB_HELP_SEARCH_LIST_CNT; i++)
  {
    list_set_field_content_by_strid(p_list, i,0, str_id[i]);
  }
  return SUCCESS;
}


static control_t *create_xstbhelp_menu(obj_attr_t obj_attr)
{
  control_t *p_cont = NULL;
  control_t *p_ic = NULL,*p_title=NULL;
  control_t *p_key = NULL,*p_search=NULL;
  control_t *p_key_list = NULL,*p_search_list=NULL;
  control_t *p_key_fn = NULL,*p_time=NULL;
  control_t *p_key_fn_info = NULL,*p_time_info1=NULL,*p_time_info2=NULL;
  control_t *p_net = NULL;
  control_t *p_net_info1 = NULL,*p_net_info2=NULL;
  u8 i;

  /*create the main window*/
  if(NULL!=fw_find_root_by_id(ROOT_ID_XSTB_HELP))
		manage_close_menu(ROOT_ID_XSTB_HELP, 0, 0);
    p_cont = fw_create_mainwin(ROOT_ID_XSTB_HELP,
    						   XSTB_HELP_X, XSTB_HELP_Y,
    						   XSTB_HELP_W, XSTB_HELP_H,
                             ROOT_ID_XSYS_SET, 0,obj_attr, 0);
    ctrl_set_rstyle(p_cont,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG,RSI_RIGHT_CONT_BG);
    p_ic = ctrl_create_ctrl(CTRL_BMAP, IDC_XSTB_HELP_IC,
												XSTB_HELP_IC_X, XSTB_HELP_IC_Y,
												XSTB_HELP_IC_W, XSTB_HELP_IC_H,
												p_cont, 0);
    bmap_set_content_by_id(p_ic, IM_XSET_HELP_IC);
   p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_XSTB_HELP_TITLE,
                       XSTB_HELP_TITLE_X, XSTB_HELP_TITLE_Y, XSTB_HELP_TITLE_W, XSTB_HELP_TITLE_H,
                       p_cont, 0);
    ctrl_set_rstyle(p_title, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_content_type(p_title, TEXT_STRTYPE_STRID);
    text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_title, STL_LEFT | STL_VCENTER);
    text_set_content_by_strid(p_title, IDS_HELP_TITLE);
    p_key = ctrl_create_ctrl(CTRL_TEXT, IDC_XSTB_HELP_KEY,
                       XSTB_HELP_KEY_X, XSTB_HELP_KEY_Y, XSTB_HELP_KEY_W, XSTB_HELP_KEY_H,
                       p_cont, 0);
    ctrl_set_rstyle(p_key, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_content_type(p_key, TEXT_STRTYPE_STRID);
    text_set_font_style(p_key, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
    text_set_align_type(p_key, STL_LEFT | STL_VCENTER);
    text_set_content_by_strid(p_key, IDS_HELP_KEY_PROMPT);

    p_key_list = ctrl_create_ctrl(CTRL_LIST, IDC_XSTB_HELP_KEY_LIST, 
    XSTB_HELP_KEY_LIST_X, XSTB_HELP_KEY_LIST_Y, XSTB_HELP_KEY_LIST_W, XSTB_HELP_KEY_LIST_H, p_cont, 0);

    ctrl_set_rstyle(p_key_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    ctrl_set_mrect(p_key_list, XSTB_HELP_LIST_MID_L, XSTB_HELP_LIST_MID_T,
                XSTB_HELP_KEY_LIST_W, XSTB_HELP_KEY_LIST_H);
    list_set_item_interval(p_key_list, XSTB_HELP_VGAP);
    list_set_item_rstyle(p_key_list, &xhelp_list_item_rstyle);
    list_enable_select_mode(p_key_list, FALSE);
    list_set_focus_pos(p_key_list, 0);
    list_set_count(p_key_list, XSTB_HELP_KEY_LIST_CNT, XSTB_HELP_KEY_LIST_CNT);
    list_set_field_count(p_key_list, XSTB_HELP_KEY_LIST_FIELD, XSTB_HELP_KEY_LIST_CNT);
    list_set_update(p_key_list, xhelp_key_list_update, 0); 

    for (i = 0; i < XSTB_HELP_KEY_LIST_FIELD; i++)
    {
    list_set_field_attr(p_key_list, (u8)i, (u32)(xhelp_key_list_attr[i].attr), (u16)(xhelp_key_list_attr[i].width),
                    (u16)(xhelp_key_list_attr[i].left), (u8)(xhelp_key_list_attr[i].top));
    list_set_field_rect_style(p_key_list, (u8)i, xhelp_key_list_attr[i].rstyle);
    list_set_field_font_style(p_key_list, (u8)i, xhelp_key_list_attr[i].fstyle);
    }    
    xhelp_key_list_update(p_key_list, list_get_valid_pos(p_key_list), XSTB_HELP_KEY_LIST_CNT, 0);

    p_search = ctrl_create_ctrl(CTRL_TEXT, IDC_XSTB_HELP_SEARCH,
                       XSTB_HELP_SEARCH_X, XSTB_HELP_SEARCH_Y, XSTB_HELP_SEARCH_W, XSTB_HELP_SEARCH_H,
                       p_cont, 0);
    ctrl_set_rstyle(p_search, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_content_type(p_search, TEXT_STRTYPE_STRID);
    text_set_font_style(p_search, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
    text_set_align_type(p_search, STL_LEFT | STL_VCENTER);
    text_set_content_by_strid(p_search, IDS_HELP_SEARCH_PROMPT);

    p_search_list = ctrl_create_ctrl(CTRL_LIST, IDC_XSTB_HELP_SEARCH_LIST, 
    XSTB_HELP_SEARCH_LIST_X, XSTB_HELP_SEARCH_LIST_Y, XSTB_HELP_SEARCH_LIST_W, XSTB_HELP_SEARCH_LIST_H, p_cont, 0);

    ctrl_set_rstyle(p_search_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    ctrl_set_mrect(p_search_list, XSTB_HELP_LIST_MID_L, XSTB_HELP_LIST_MID_T,
                XSTB_HELP_SEARCH_LIST_W, XSTB_HELP_SEARCH_LIST_H);
    list_set_item_interval(p_search_list, XSTB_HELP_VGAP);
    list_set_item_rstyle(p_search_list, &xhelp_list_item_rstyle);
    list_enable_select_mode(p_search_list, FALSE);
    list_set_focus_pos(p_search_list, 0);
    list_set_count(p_search_list, XSTB_HELP_SEARCH_LIST_CNT, XSTB_HELP_SEARCH_LIST_CNT);
    list_set_field_count(p_search_list, XSTB_HELP_SEARCH_LIST_FIELD, XSTB_HELP_SEARCH_LIST_CNT);
    list_set_update(p_search_list, xhelp_search_list_update, 0); 

    for (i = 0; i < XSTB_HELP_SEARCH_LIST_FIELD; i++)
    {
        list_set_field_attr(p_search_list, (u8)i, (u32)(xhelp_search_list_attr[i].attr), (u16)(xhelp_search_list_attr[i].width),
                        (u16)(xhelp_search_list_attr[i].left), (u8)(xhelp_search_list_attr[i].top));
        list_set_field_rect_style(p_search_list, (u8)i, xhelp_search_list_attr[i].rstyle);
        list_set_field_font_style(p_search_list, (u8)i, xhelp_search_list_attr[i].fstyle);
    }    
    xhelp_search_list_update(p_search_list, list_get_valid_pos(p_search_list), XSTB_HELP_SEARCH_LIST_CNT, 0);

    p_key_fn = ctrl_create_ctrl(CTRL_TEXT, IDC_XSTB_HELP_FN,
                       XSTB_HELP_FN_X, XSTB_HELP_FN_Y, XSTB_HELP_FN_W, XSTB_HELP_FN_H,
                       p_cont, 0);
    ctrl_set_rstyle(p_key_fn, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_content_type(p_key_fn, TEXT_STRTYPE_STRID);
    text_set_font_style(p_key_fn, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
    text_set_align_type(p_key_fn, STL_LEFT | STL_VCENTER);
    text_set_content_by_strid(p_key_fn, IDS_HELP_FN_PROMPT);

    p_key_fn_info = ctrl_create_ctrl(CTRL_TEXT, IDC_XSTB_HELP_FN_INFO,
                       XSTB_HELP_FN_INFO_X, XSTB_HELP_FN_INFO_Y,
                       XSTB_HELP_FN_INFO_W, XSTB_HELP_FN_INFO_H,
                       p_cont, 0);
    ctrl_set_rstyle(p_key_fn_info, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_content_type(p_key_fn_info, TEXT_STRTYPE_STRID);
    text_set_font_style(p_key_fn_info, FSI_WHITE_18, FSI_WHITE_18, FSI_WHITE_18);
    text_set_align_type(p_key_fn_info, STL_LEFT | STL_VCENTER);
    text_enable_page(p_key_fn_info, TRUE);
    text_set_content_by_strid(p_key_fn_info, IDS_HELP_FN_INFO);

    p_time = ctrl_create_ctrl(CTRL_TEXT, IDC_XSTB_HELP_TIME_SETTING,
                       XSTB_HELP_TIME_SETTING_X, XSTB_HELP_TIME_SETTING_Y, 
                       XSTB_HELP_TIME_SETTING_W, XSTB_HELP_TIME_SETTING_H,
                       p_cont, 0);
    ctrl_set_rstyle(p_time, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_content_type(p_time, TEXT_STRTYPE_STRID);
    text_set_font_style(p_time, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
    text_set_align_type(p_time, STL_LEFT | STL_VCENTER);
    text_set_content_by_strid(p_time, IDS_HELP_TIME_SETTING_PROMPT);

    p_time_info1 = ctrl_create_ctrl(CTRL_TEXT, IDC_XSTB_HELP_TIME_SETTING_INFO1,
                       XSTB_HELP_TIME_SETTING_INFO1_X, XSTB_HELP_TIME_SETTING_INFO1_Y,
                       XSTB_HELP_TIME_SETTING_INFO1_W, XSTB_HELP_TIME_SETTING_INFO1_H,
                       p_cont, 0);
    ctrl_set_rstyle(p_time_info1, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_content_type(p_time_info1, TEXT_STRTYPE_STRID);
    text_set_font_style(p_time_info1, FSI_WHITE_18, FSI_WHITE_18, FSI_WHITE_18);
    text_set_align_type(p_time_info1, STL_LEFT | STL_VCENTER);
    text_enable_page(p_time_info1, TRUE);
    text_set_content_by_strid(p_time_info1, IDS_HELP_TIME_SETTING_INFO1);

    p_time_info2 = ctrl_create_ctrl(CTRL_TEXT, IDC_XSTB_HELP_TIME_SETTING_INFO2,
                       XSTB_HELP_TIME_SETTING_INFO2_X, XSTB_HELP_TIME_SETTING_INFO2_Y,
                       XSTB_HELP_TIME_SETTING_INFO2_W, XSTB_HELP_TIME_SETTING_INFO2_H,
                       p_cont, 0);
    ctrl_set_rstyle(p_time_info2, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_content_type(p_time_info2, TEXT_STRTYPE_STRID);
    text_set_font_style(p_time_info2, FSI_WHITE_18, FSI_WHITE_18, FSI_WHITE_18);
    text_set_align_type(p_time_info2, STL_LEFT | STL_VCENTER);
    text_enable_page(p_time_info2, TRUE);
    text_set_content_by_strid(p_time_info2, IDS_HELP_TIME_SETTING_INFO2);

    p_net = ctrl_create_ctrl(CTRL_TEXT, IDC_XSTB_HELP_NET,
                       XSTB_HELP_NET_X, XSTB_HELP_NET_Y, 
                       XSTB_HELP_NET_W, XSTB_HELP_NET_H,
                       p_cont, 0);
    ctrl_set_rstyle(p_net, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_content_type(p_net, TEXT_STRTYPE_STRID);
    text_set_font_style(p_net, FSI_WHITE_20, FSI_WHITE_20, FSI_WHITE_20);
    text_set_align_type(p_net, STL_LEFT | STL_VCENTER);
    text_set_content_by_strid(p_net, IDS_HELP_NET_PROMPT);

    p_net_info1 = ctrl_create_ctrl(CTRL_TEXT, IDC_XSTB_HELP_NET_INFO1,
                       XSTB_HELP_NET_INFO1_X, XSTB_HELP_NET_INFO1_Y,
                       XSTB_HELP_NET_INFO1_W, XSTB_HELP_NET_INFO1_H,
                       p_cont, 0);
    ctrl_set_rstyle(p_net_info1, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_content_type(p_net_info1, TEXT_STRTYPE_STRID);
    text_set_font_style(p_net_info1, FSI_WHITE_18, FSI_WHITE_18, FSI_WHITE_18);
    text_set_align_type(p_net_info1, STL_LEFT | STL_VCENTER);
    text_enable_page(p_net_info1, TRUE);
    text_set_content_by_strid(p_net_info1, IDS_HELP_NET_SETTING_INFO1);

    p_net_info2 = ctrl_create_ctrl(CTRL_TEXT, IDC_XSTB_HELP_NET_INFO2,
                       XSTB_HELP_NET_INFO2_X, XSTB_HELP_NET_INFO2_Y,
                       XSTB_HELP_NET_INFO2_W, XSTB_HELP_NET_INFO2_H,
                       p_cont, 0);
    ctrl_set_rstyle(p_net_info2, RSI_IGNORE, RSI_IGNORE, RSI_IGNORE);
    text_set_content_type(p_net_info2, TEXT_STRTYPE_STRID);
    text_set_font_style(p_net_info2, FSI_WHITE_18, FSI_WHITE_18, FSI_WHITE_18);
    text_set_align_type(p_net_info2, STL_LEFT | STL_VCENTER);
    text_enable_page(p_net_info2, TRUE);
    text_set_content_by_strid(p_net_info2, IDS_HELP_NET_SETTING_INFO2);
    return p_cont;
}

RET_CODE open_xstbhelp(u32 para1, u32 para2)
{
  control_t *p_menu = NULL;
  obj_attr_t obj_attr = (obj_attr_t)para2;
  p_menu = create_xstbhelp_menu(obj_attr);
  MT_ASSERT(p_menu != NULL);
  ctrl_paint_ctrl(p_menu, FALSE);
  return SUCCESS;
}


