/****************************************************************************

****************************************************************************/
#include "ui_common.h"

#include "ui_xinfo.h"
#include "data_manager.h"
#include "data_manager_v2.h"

enum info_ctrl_id
{
  IDC_INFO_LIST = 1,
  IDC_INFO_TITLE_ID = 2,
};

static list_xstyle_t info_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
  RSI_PBACK,
};

static list_xstyle_t info_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_BLACK,
  FSI_WHITE,
};

static list_xstyle_t info_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t info_list_attr[INFO_LIST_FIELD] =
{
  { LISTFIELD_TYPE_STRID | STL_LEFT | STL_VCENTER,
    230, 5, 0, &info_list_field_rstyle,  &info_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    INFO_LIST_W - 500, 400, 0, &info_list_field_rstyle,  &info_list_field_fstyle},       
};

static RET_CODE info_list_update(control_t* p_list, u16 start, u16 size, 
                                 u32 context)
{
  u8 i;
  u16 str_id[INFO_LIST_CNT] = 
    { 
    IDS_CUR_STBID,
    IDS_MANUFACTURE_ID,
    IDS_HARDWARE_VER,
    IDS_SOFTWARE_VER,
#if INFO_SYSTEM_VERSION
	IDS_SYSTEM_VERSION,
#endif
    IDS_COMPILE_DATE,
    IDS_LOADER_VER,
    };
  u32 oui = 0;
  misc_options_t misc;
  sw_info_t sw_info;
  u8 ansstr[64];
  
  dm_read(class_get_handle_by_id(DM_CLASS_ID),
                               MISC_OPTION_BLOCK_ID, 0, 0,
                               sizeof(misc_options_t), (u8 *)&misc);

  oui = misc.ota_tdi.oui;
  
  sys_status_get_sw_info(&sw_info);
  for(i = 0; i < INFO_LIST_CNT; i++)
  {
    list_set_field_content_by_strid(p_list, i, 0, str_id[i]);
  }

  {
    
    {
      memset(ansstr, 0, sizeof(ansstr));
      if(sys_get_serial_num(ansstr, sizeof(ansstr)))
      {
        list_set_field_content_by_ascstr(p_list, 0, 1, ansstr);
      }
    }

    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "%lx", oui);
    list_set_field_content_by_ascstr(p_list, 1, 1, ansstr);
    
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "%x", misc.ota_tdi.hw_version);
    list_set_field_content_by_ascstr(p_list, 2, 1, ansstr);
    OS_PRINTF("\nhw version%d\n",misc.ota_tdi.hw_version);
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "%lx", ui_ota_api_get_upg_check_version());
    OS_PRINTF("ui_ota_api_get_upg_check_version:%ld\n",ui_ota_api_get_upg_check_version());
    list_set_field_content_by_ascstr(p_list, 3, 1, ansstr);

#if INFO_SYSTEM_VERSION
	memset(ansstr, 0, sizeof(ansstr));
	sprintf((char*)ansstr, "NewCool-3.0");
	list_set_field_content_by_ascstr(p_list, 4, 1, ansstr);


	memset(ansstr, 0, sizeof(ansstr));
	sprintf((char*)ansstr, "%s	 %s",__DATE__,__TIME__);
	list_set_field_content_by_ascstr(p_list, 5, 1, ansstr);

	memset(ansstr, 0, sizeof(ansstr));
	sprintf((char*)ansstr, "1010");
	list_set_field_content_by_ascstr(p_list, 6, 1, ansstr);

#else
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "%s   %s",__DATE__,__TIME__);
    list_set_field_content_by_ascstr(p_list, 4, 1, ansstr);
	
    memset(ansstr, 0, sizeof(ansstr));
    sprintf((char*)ansstr, "1010");
    list_set_field_content_by_ascstr(p_list, 5, 1, ansstr);
#endif	
  }
  return SUCCESS;
}

RET_CODE open_xinfo(u32 para1, u32 para2)
{
  control_t *p_cont, *p_list;
  u8 i;

#ifndef SPT_SUPPORT
  p_cont = ui_comm_right_root_create(ROOT_ID_XINFO, ROOT_ID_XSYS_SET, OBJ_ATTR_ACTIVE, 0);
  if (p_cont == NULL)
  {
    return ERR_FAILURE;
  }

  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_INFO_LIST, 
    INFO_LIST_X, INFO_LIST_Y, INFO_LIST_W, INFO_LIST_H, p_cont, 0);
  ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);

  ctrl_set_mrect(p_list, INFO_LIST_MID_L, INFO_LIST_MID_T,
                    INFO_LIST_MID_W+INFO_LIST_MID_L, INFO_LIST_MID_H+INFO_LIST_MID_T);
  list_set_item_interval(p_list, INFO_LIST_ITEM_VGAP);
  list_set_item_rstyle(p_list, &info_list_item_rstyle);
  list_enable_select_mode(p_list, FALSE);
  list_set_focus_pos(p_list, 0);
  list_set_count(p_list, INFO_LIST_CNT, INFO_LIST_CNT);
  list_set_field_count(p_list, INFO_LIST_FIELD, INFO_LIST_CNT);
  list_set_update(p_list, info_list_update, 0); 
 
  for (i = 0; i < INFO_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(info_list_attr[i].attr), (u16)(info_list_attr[i].width),
                        (u16)(info_list_attr[i].left), (u8)(info_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, info_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, info_list_attr[i].fstyle);
  }    

  info_list_update(p_list, list_get_valid_pos(p_list), INFO_LIST_CNT, 0);
#else
#endif
  /* set focus according to current info */
#ifdef SPT_DUMP_DATA
  spt_dump_menu_data(p_cont);
#endif

  ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0); /* focus on prog_name */
  ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

  return SUCCESS;
}

