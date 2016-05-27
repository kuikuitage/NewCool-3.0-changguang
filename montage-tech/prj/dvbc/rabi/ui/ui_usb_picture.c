
#include "ui_common.h"
#include "ui_usb_picture.h"
#include "ui_timer.h"
#ifndef IMPL_NEW_EPG
#include "epg_data4.h"
#endif
#include "lib_char.h"
#include "ui_rename.h"
#include "ui_mute.h"
#include "ui_volume_usb.h"
#include "ui_picture.h"
#include "ui_usb_music.h"
#include "ui_edit_usr_pwd.h"
#include "pnp_service.h"


typedef enum
{
  MSG_RED = MSG_LOCAL_BEGIN + 250,
  MSG_BLUE,
  MSG_GREEN,
  MSG_YELLOW,
  MSG_MUSIC_PLAY,
  MSG_MUSIC_RESUME,
  MSG_MUSIC_PAUSE,
  MSG_MUSIC_NEXT,
  MSG_MUSIC_PRE,
  MSG_MUSIC_STOP,
  MSG_ADD_TO_LIST,
  MSG_ONE_SECOND_ARRIVE,
  MSG_VOLUME_UP,
  MSG_VOLUME_DOWN,
  MSG_SORT,
  MSG_ADD_FAV,
  MSG_FULLSCREEN,
  MSG_INFO,
  MSG_PIC_UPFOLDER,
}usb_picture_msg_t;

typedef enum
{
  IDC_PIC_PREVIEW = 1,
  IDC_PIC_MSG,
  IDC_PIC_DETAIL_CONT,
  IDC_PIC_GROUP_CONT,
  IDC_PIC_GROUP_ARROWL,
  IDC_PIC_GROUP_ARROWR,
  IDC_PIC_GROUP,
  IDC_PIC_LIST_CONT,
  IDC_PIC_LIST,
  IDC_PIC_SBAR,
  IDC_PIC_PATH,
  IDC_PIC_FAV_LIST_CONT,
  IDC_PIC_SORT_LIST,
  IDC_MP_HELP,
  IDC_PIC_BOTTOM_HELP,
  IDC_PIC_GROUP_TITLE1_CONT,
  IDC_PIC_GROUP_TITLE2_CONT,
  IDC_PIC_GROUP_TITLE3_CONT,
  IDC_PIC_GROUP_TITLE4_CONT,
  IDC_PIC_CTRL_CONT,
  IDC_PIC_LEFT_BG,
  IDC_PIC_LEFT_HD,
    IDC_PIC_GROUP_BMP1_CONT,
  IDC_PIC_GROUP_BMP2_CONT,
  IDC_PIC_GROUP_BMP3_CONT,
  IDC_PIC_GROUP_BMP4_CONT,

}usb_picture_ctrl_id_t;

typedef enum
{
  IDC_PIC_DETAIL_FILENAME = 1,
  IDC_PIC_DETAIL_PIX_TIME,
  IDC_PIC_DETAIL_FILE_SIZE,
}usb_picture_list_detail_id;

typedef enum
{
  IDC_PIC_FAV_LIST_TITLE= 1,
  IDC_PIC_FAV_LIST_LIST,
  IDC_PIC_FAV_LIST_HELP,
  IDC_PIC_FAV_LIST_SBAR,
}pic_play_list_ctrl_id_t;


static list_xstyle_t  pic_list_item_rstyle =
{
  RSI_MEDIO_LEFT_LIST_BG,
  RSI_MEDIO_LEFT_LIST_BG,
  RSI_MEDIO_LEFT_LIST_HL,
  RSI_MEDIO_LEFT_LIST_SH,
  RSI_MEDIO_LEFT_LIST_HL,
};

static list_xstyle_t mp_list_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t mp_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t mp_list_attr[PIC_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR,
    40, 0, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_ICON,
    30, 40, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | LISTFIELD_SCROLL,
    212, 70, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
};

static list_field_attr_t mp_play_list_attr[PIC_LIST_FIELD] =
{
  { LISTFIELD_TYPE_UNISTR,
    40, 0, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_ICON,
    30, 40, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | LISTFIELD_SCROLL,
    182, 70, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
  { LISTFIELD_TYPE_ICON | STL_LEFT,
    //40, 240, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
    30, 252, 0, &mp_list_field_rstyle,  &mp_list_field_fstyle},
};

static list_field_attr_t mp_sort_list_field_attr[MP_SORT_LIST_FIELD_NUM] =
{
  { LISTFIELD_TYPE_STRID | STL_LEFT | STL_VCENTER,
    MP_SORT_LIST_MIDW, 0, 0, &mp_list_field_rstyle, &mp_list_field_fstyle },
};

static rect_t g_pic_rect = {
    COMM_BG_X ,
    COMM_BG_Y ,
    COMM_BG_W,
    COMM_BG_H,
    };

static rect_t p_dlg_rc =
{
  MP_SAVE_L,
  MP_SAVE_T,
  MP_SAVE_R,
  MP_SAVE_B,
};

static file_list_t g_list = {0};
static char *p_g_pic_filter = "|jpeg|JPEG|jpg|JPG|gif|GIF|BMP|bmp|PNG|png";
static partition_t *p_partition = NULL;
static flist_dir_t g_pic_flist_dir = NULL;
static u32 g_partition_cnt = 0;
static utc_time_t cur_play_time = {0};
static utc_time_t total_play_time = {0};
static roll_param_t roll_param = {ROLL_LR, ROLL_SINGLE, 0, FALSE};
static u16 g_picture_player_preview_focus = 0;
static u32 is_usb = 1;
static char ipaddress[128] = "";
static picture_param_t p_param = {0};
static u16 ip_address_with_path[32] ={0};
static u16 play_letter = 0;
static BOOL b_is_picture_error = FALSE;
static BOOL picture_now_change = FALSE;
#ifdef ENABLE_ADS
static BOOL g_usb_picture_show_ads = FALSE;
#endif
static RET_CODE pic_list_update(control_t* p_list, u16 start, u16 size, u32 context);
static RET_CODE pic_fav_list_update(control_t* p_list, u16 start, u16 size, u32 context);

u16 pic_cont_keymap(u16 key);
RET_CODE pic_cont_proc(control_t *p_cont, u16 msg,
  u32 para1, u32 para2);

u16 pic_list_keymap(u16 key);
RET_CODE pic_list_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);

u16 pic_network_list_keymap(u16 key);
RET_CODE pic_network_list_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);

u16 pic_fav_list_list_keymap(u16 key);
RET_CODE pic_fav_list_list_proc(control_t *p_list, u16 msg,
  u32 para1, u32 para2);

u16 pic_sort_list_keymap(u16 key);
RET_CODE pic_sort_list_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

extern RET_CODE pnp_svc_unmount(u16 *p_url);

u16 _ui_pic_v_txt_keymap(u16 key);
RET_CODE _ui_pic_v_txt_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static void _ui_pic_m_free_dir_and_list(void)
{
    if(NULL != g_pic_flist_dir)
    {
        file_list_leave_dir(g_pic_flist_dir);
        g_pic_flist_dir = NULL;
        memset(&g_list, 0, sizeof(g_list));
    }
}
static RET_CODE _ui_pic_m_get_file_list(u16 *p_path)
{
    u16 *p_name_filter = NULL;
    comm_dlg_data_t dlg_data = {0};	   
    control_t *p_root = NULL;
    flist_dir_t flist_dir = NULL;
    OS_PRINTF("@@@%s\n", __FUNCTION__);
    p_root = fw_find_root_by_id(ROOT_ID_USB_PICTURE);
    if(p_root)
    {
        dlg_data.parent_root = ROOT_ID_USB_PICTURE;
    }
    else
    {
       dlg_data.parent_root = ROOT_ID_BACKGROUND;
    }
    
    dlg_data.style = DLG_FOR_SHOW | DLG_STR_MODE_STATIC;
    dlg_data.x = ((SCREEN_WIDTH - 400) / 2);
    dlg_data.y = ((SCREEN_HEIGHT - 200) / 2);
    dlg_data.w = 400;
    dlg_data.h = 160;
    dlg_data.content = IDS_WAIT_PLEASE;
    dlg_data.dlg_tmout = 0;
    ui_comm_dlg_open(&dlg_data); 
    ui_evt_disable_ir();
    p_name_filter = mtos_malloc(sizeof(u16) *(strlen(p_g_pic_filter) + 1));
    MT_ASSERT(p_name_filter != NULL);
    memset(p_name_filter, 0 , sizeof(u16) *(strlen(p_g_pic_filter) + 1));
    str_asc2uni(p_g_pic_filter, p_name_filter);
    flist_dir = file_list_enter_dir(p_name_filter, MAX_FILE_COUNT, p_path);
    mtos_free(p_name_filter);
    if(NULL == flist_dir)
    {
        ui_evt_enable_ir();
        ui_comm_dlg_close();    
        OS_PRINTF("[%s]: ##ERR## file list null\n", __FUNCTION__);
        return ERR_FAILURE;
    }
    _ui_pic_m_free_dir_and_list();
    g_pic_flist_dir = flist_dir;
    file_list_get(g_pic_flist_dir, FLIST_UNIT_FIRST, &g_list);
    ui_evt_enable_ir();
    ui_comm_dlg_close();
    return SUCCESS;
}
u32 get_picture_is_usb()//if is_usb is 0 means picture entry from samba 
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
  return is_usb;
}

static RET_CODE pic_group_update(control_t *p_cbox, u16 focus, u16 *p_str,
                              u16 max_length)
{
  //str_asc2uni(p_partition[focus].name, p_str);
	//dvb_to_unicode(p_partition[focus].name, max_length - 1, p_str, max_length);
	OS_PRINTF("@@@%s\n", __FUNCTION__);
	uni_strncpy(p_str, p_partition[focus].name, max_length);
  return SUCCESS;
}

static RET_CODE pic_network_group_update(control_t *p_cbox, u16 focus, u16 *p_str,
                              u16 max_length)
{
  // u8 ipaddress[128] = "";
   u8 ip[128] = "\\\\";
  // sprintf(ipaddress, "%d.%d.%d.%d",ip_address.s_b1,ip_address.s_b2,ip_address.s_b3,ip_address.s_b4);
   strcat(ip,ipaddress);
    OS_PRINTF("@@@%s\n", __FUNCTION__);
  //str_asc2uni(p_partition[focus].name, p_str);
	dvb_to_unicode(ip,max_length - 1, p_str, max_length);
	//uni_strncpy(p_str, p_partition[focus].name, max_length);
  return SUCCESS;
}

static void pic_update_detail(control_t *p_cont, u32 width, u32 height, BOOL is_paint, BOOL is_clear)
{
  control_t *p_detail_cont = NULL;
  control_t *p_fsize = NULL, *p_pixel = NULL, *p_list = NULL, *p_list_cont = NULL,*p_name = NULL ;
  u16 list_focus = 0;
  u8 asc_str[256];
  u32 file_size = 0;
  media_fav_t *p_media = NULL;
    u16 uni_str[128];
  u16 *p_temp = {0};
OS_PRINTF("@@@%s\n", __FUNCTION__);
 // p_detail_cont = ctrl_get_child_by_id(p_list->p_parent, IDC_PIC_DETAIL_CONT);
  p_detail_cont = ctrl_get_child_by_id(p_cont, IDC_PIC_DETAIL_CONT);
  if(ui_pic_get_play_mode() == PIC_MODE_FAV)
  {
    p_list_cont = ctrl_get_child_by_id(p_cont, IDC_PIC_FAV_LIST_CONT);
    p_list = ctrl_get_child_by_id(p_list_cont, IDC_PIC_FAV_LIST_LIST);
  }
  else
  {
    p_list_cont = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST_CONT);
    p_list = ctrl_get_child_by_id(p_list_cont, IDC_PIC_LIST);
  }

  list_focus = list_get_focus_pos(p_list);

  p_fsize = ctrl_get_child_by_id(p_detail_cont, IDC_PIC_DETAIL_FILE_SIZE);
  p_pixel = ctrl_get_child_by_id(p_detail_cont, IDC_PIC_DETAIL_PIX_TIME);
  p_name = ctrl_get_child_by_id(p_detail_cont, IDC_PIC_DETAIL_FILENAME);

  if(!is_clear)
  {
    ui_pic_get_cur(&p_media);
    if(p_media == NULL)
    {
        OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
        return;
    }

    memset(uni_str, 0, sizeof(uni_str));
    p_temp = uni_strrchr(p_media->path, 0x5c/*'\\'*/);
    if(p_temp != NULL)
    {
		  uni_strncpy(uni_str, ++p_temp, 128);
    }
    OS_PRINTF("@@@@@@@%s \n",uni_str);
     text_set_content_by_unistr(p_name, uni_str);
    file_size = file_list_get_file_size(p_media->path);
    
    memset(asc_str, 0, sizeof(asc_str));
    ui_conver_file_size_unit_bytes(file_size, asc_str);

    text_set_content_by_ascstr(p_fsize, asc_str);

    //set width & height.
    sprintf(asc_str, "%d x %d", (int)width, (int)height);
    text_set_content_by_ascstr(p_pixel, asc_str);
  }
  else
  {
    text_set_content_by_ascstr(p_fsize, " ");
    text_set_content_by_ascstr(p_pixel, " ");
    text_set_content_by_ascstr(p_name, " ");
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_fsize, TRUE);
    ctrl_paint_ctrl(p_pixel, TRUE);
    ctrl_paint_ctrl(p_name, TRUE);
  }
}

/*static void pic_fav_list_update_help(control_t *p_mbox, BOOL is_paint)
{
  u8 i;
  u16 icon[MP_HELP_MBOX_TOL] =
    {
      IM_HELP_RED,
      IM_HELP_YELLOW,
      IM_HELP_GREEN,
    };
  u16 str[MP_HELP_MBOX_TOL] =
    {
      IDS_PLAY,
      IDS_DELETE,
      IDS_DELETE_ALL,
    };

  for(i = 0; i < MP_PLAY_LIST_HELP_MBOX_TOL; i++)
  {
    mbox_set_content_by_strid(p_mbox, i, str[i]);
    mbox_set_content_by_icon(p_mbox, i, icon[i], icon[i]);
  }

  if(is_paint)
  {
    ctrl_paint_ctrl(p_mbox, TRUE);
  }
}*/

static void pic_update_path(control_t *p_list)
{
  control_t *p_path;
  u16 uni_str[MAX_FILE_PATH] = {0};
OS_PRINTF("@@@%s\n", __FUNCTION__);
  //p_path = ctrl_get_child_by_id(p_list->p_parent, IDC_PIC_PATH);
  p_path = ctrl_get_child_by_id(ctrl_get_parent(p_list->p_parent), IDC_PIC_PATH);

  //dvb_to_unicode(,
  //  MAX_FILE_PATH - 1, uni_str, MAX_FILE_PATH);

  uni_strncpy(uni_str, file_list_get_cur_path(g_pic_flist_dir), MAX_FILE_PATH);
  text_set_content_by_unistr(p_path, uni_str);
  ctrl_paint_ctrl(p_path, TRUE);
}

static BOOL pic_file_list_is_enter_dir(u16 *p_filter, u16 *p_path)
{
  u16 *p_temp = NULL;
  u16 parent[4] = {0};
  u16 curn[4] = {0};
  u16 cur_path[255] = {0};
  MT_ASSERT(p_path != NULL);

  uni_strcpy(cur_path, p_path);
OS_PRINTF("@@@%s\n", __FUNCTION__);
  p_temp = uni_strrchr(cur_path, 0x5c/*'\\'*/);

  if (p_temp != NULL)
  {
    //parent dir
    str_asc2uni("..", parent);
    str_asc2uni(".", curn);
    if (uni_strlen(p_temp) >= 3 && uni_strcmp(p_temp + 1, parent/*".."*/) == 0)
    {
      p_temp[0] = 0/*'\0'*/;
      p_temp = uni_strrchr(cur_path, 0x5c/*'\\'*/);
      if (p_temp != NULL)
      {
        p_temp[0] = 0/*'\0'*/;
      }
    }
    //cur dir
    else if (uni_strlen(p_temp) >= 2 && uni_strcmp(p_temp + 1, curn/*"."*/) == 0)
    {
      p_temp[0] = 0/*'\0'*/;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
  return TRUE;
}

static void pic_file_list_update(control_t *p_list)
{
  RET_CODE ret = 0;
  int count = 0;
  u32 i = 0;
  u16 preDirLen = 0;
  u16 father_filename[MAX_FILE_PATH] = {0};
  
  OS_PRINTF("@@@%s\n", __FUNCTION__);
  uni_strncpy(father_filename, file_list_get_cur_path(g_pic_flist_dir), MAX_FILE_PATH);
  preDirLen = uni_strlen(father_filename);
  
  ret = _ui_pic_m_get_file_list(g_list.p_file[0].name);
  if(ret == SUCCESS)
  {
     count = g_list.file_count;
	 
     list_set_count(p_list, (u16)(g_list.file_count), PIC_LIST_PAGE);
     list_set_focus_pos(p_list, 0);
	 
     for(i = 0; i < count; i++)
     {
	   if(preDirLen != uni_strlen(g_list.p_file[i].name))
		continue;

         if(uni_strcmp(father_filename,g_list.p_file[i].name) == 0)
         {
   	      list_set_focus_pos(p_list, i);
   	      list_select_item(p_list, i);
         }		   
     }

     pic_list_update(p_list, list_get_valid_pos(p_list), PIC_LIST_PAGE, 0);
     ui_pic_build_play_list_indir(&g_list);
     ctrl_paint_ctrl(p_list, TRUE);
     pic_update_path(p_list);
  }
}

static RET_CODE on_pic_exit(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list_cont = NULL;
  control_t *p_list = NULL;
  BOOL is_enter_dir = FALSE;
  // u16 * p_url = NULL;
   // int ret = ERR_FAILURE;
   u16 *p_name_filter = NULL;

//  if(msg == MSG_EXIT_ALL)
//  {
//  ui_usb_music_exit(); //for bug 34481
//  }
  OS_PRINTF("@@@%s\n", __FUNCTION__);
  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST_CONT);
  p_list = ctrl_get_child_by_id(p_list_cont, IDC_PIC_LIST);
  if(!is_usb)
  {
    gui_stop_roll(p_list);
    ui_time_enable_heart_beat(FALSE);
    close_mute();
    //ui_get_ip_path_mount(&p_url);
    //pnp_service_vfs_unmount(p_url);
    #ifdef ENABLE_ADS
  	g_usb_picture_show_ads = TRUE;
  	#endif
    return ERR_NOFEATURE;
  }
  if (g_list.file_count != 0)
  {
    p_name_filter = mtos_malloc(sizeof(u16) *(strlen(p_g_pic_filter) + 1));
    MT_ASSERT(p_name_filter != NULL);
    memset(p_name_filter, 0 , sizeof(u16) *(strlen(p_g_pic_filter) + 1));
    str_asc2uni(p_g_pic_filter, p_name_filter);
    is_enter_dir = pic_file_list_is_enter_dir(p_name_filter, g_list.p_file[0].name);
    mtos_free(p_name_filter);
  }
  else
  {
    is_enter_dir = FALSE;
  }
  if(is_enter_dir)
  {
    pic_file_list_update(p_list);//simon fix bug #27346
    return SUCCESS;
  }
  else
  {
    gui_stop_roll(p_list);

    ui_time_enable_heart_beat(FALSE);
  }
  close_mute();
  /* if(!is_usb)
   {
     ui_get_ip_path_mount(&p_url);
    pnp_service_vfs_unmount(p_url);
   }*/
  #ifdef ENABLE_ADS
  g_usb_picture_show_ads = TRUE;
  #endif
  return ERR_NOFEATURE;
}

static RET_CODE on_pic_process_msg(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list_cont = NULL;
  control_t *p_cont = NULL;
  OS_PRINTF("@@@%s\n", __FUNCTION__);
  p_list_cont = ctrl_get_parent(p_list);
  p_cont = ctrl_get_parent(p_list_cont);
  ctrl_process_msg(p_cont, MSG_EXIT, 0, 0);
  if(MSG_PIC_BACK_TO_NETWORK_PLACE == msg)
  {
    manage_close_menu(ROOT_ID_EDIT_USR_PWD, 0, 0);
  }
  return SUCCESS;
}

static RET_CODE on_pic_destory(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list = NULL, *p_list_cont = NULL, *p_fav_list = NULL;
/*
  if(g_pic_flist_dir != NULL)
  {
        file_list_leave_dir(g_pic_flist_dir);
        g_pic_flist_dir = NULL;
        memset(&g_list, 0, sizeof(g_list));    
  }*/
     u16 * p_url = NULL;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST_CONT);
  p_list = ctrl_get_child_by_id(p_list_cont, IDC_PIC_LIST);

  //ui_usb_music_exit();

  if(p_list != NULL)
  {
    OS_PRINTF("============on_pic_destory gui_stop_roll p_list id = %d============\n",p_list->id);
    gui_stop_roll(p_list);
  }

  p_fav_list = ctrl_get_child_by_id(ctrl_get_child_by_id(p_cont, IDC_PIC_FAV_LIST_CONT), IDC_PIC_FAV_LIST_LIST);
  if(p_fav_list != NULL)
  {
    OS_PRINTF("============on_pic_destory gui_stop_roll p_fav_list id = %d============\n",p_fav_list->id);
    gui_stop_roll(p_fav_list);
  }
  ui_enable_chk_pwd(TRUE);
  ui_pic_unload_fav_list();

  pic_stop();

  ui_pic_release();
  ui_deinit_play_timer();
  if(!is_usb && (fw_find_root_by_id(ROOT_ID_USB_FILEPLAY) == NULL))
  {
    ui_get_ip_path_mount(&p_url);
    pnp_service_vfs_unmount(p_url);
  }

  if(is_usb)
  {
     _ui_pic_m_free_dir_and_list();
  }

  if(picture_now_change == FALSE)
  {
        ui_play_curn_pg();
  }
  else
  {
    picture_now_change = FALSE;
  }

	#ifdef ENABLE_ADS
	ui_adv_pic_init(PIC_SOURCE_BUF);
	if(g_usb_picture_show_ads)
	{
		g_usb_picture_show_ads = FALSE;
		fw_notify_root(fw_find_root_by_id(ROOT_ID_MAINMENU), NOTIFY_T_MSG, TRUE, MSG_REFRESH_ADS_PIC, 0, 0);
	}
	#endif

  return ERR_NOFEATURE;
}

static RET_CODE on_pic_evt_update_size(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
    OS_PRINTF("@@@%s\n", __FUNCTION__);
  pic_update_detail(p_cont, para1, para2, TRUE, FALSE);

  return SUCCESS;
}

static RET_CODE on_pic_evt_unsupport(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_msg = NULL;

OS_PRINTF("@@@%s\n", __FUNCTION__);
  OS_PRINTF("%s(Line: %d):  The picture unsupport.\n", __FUNCTION__, __LINE__);
  ui_pic_clear(0);

  b_is_picture_error = TRUE;
  p_msg = ctrl_get_child_by_id(p_cont, IDC_PIC_MSG);
  
  ctrl_set_sts(p_msg, OBJ_STS_SHOW);

  text_set_content_by_strid(p_msg, IDS_UNSUPPORTED);

  ctrl_paint_ctrl(p_msg, TRUE);

  return SUCCESS;
}

static RET_CODE on_pic_evt_data_error(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_msg = NULL;

  OS_PRINTF("%s(Line: %d):  The picture data error.\n", __FUNCTION__, __LINE__);
  ui_pic_clear(0);

  b_is_picture_error = TRUE;
  p_msg = ctrl_get_child_by_id(p_cont, IDC_PIC_MSG);
  
  ctrl_set_sts(p_msg, OBJ_STS_SHOW);

  text_set_content_by_strid(p_msg, IDS_DATA_ERROR);

  ctrl_paint_ctrl(p_msg, TRUE);

  return SUCCESS;
}

static RET_CODE on_pic_evt_too_large(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_msg = NULL;

  OS_PRINTF("%s(Line: %d):  The picture too large.\n", __FUNCTION__, __LINE__);
  ui_pic_clear(0);

  b_is_picture_error = TRUE;
  p_msg = ctrl_get_child_by_id(p_cont, IDC_PIC_MSG);
  
  ctrl_set_sts(p_msg, OBJ_STS_SHOW);

  text_set_content_by_strid(p_msg, IDS_MSG_OUT_OF_RANGE);

  ctrl_paint_ctrl(p_msg, TRUE);

  return SUCCESS;
}

 static RET_CODE ui_change_plug_out_partition(control_t *p_list, u16 msg,
  u32 para1, u32 para2,RET_CODE (*p_proc)(control_t *p_list, u16 msg,
  u32 para1, u32 para2))
{
  u16 cur_usb_index = 0;
  u16 cur_ui_index = 0;
  u16 usb_plug_index = 0;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  usb_plug_index = ui_usb_get_plug_partition_sev();
  cur_usb_index = ui_usb_get_cur_used((u16)para2);
  cur_ui_index = ui_usb_get_cur_used((u16)para1);
	
  if(cur_usb_index != usb_plug_index && ui_usb_get_dev_num() > 1)
  {   
		if(cur_ui_index == usb_plug_index)
		{
		   p_proc(p_list,msg,0,0xFFFF);
		}
		return SUCCESS;
  }
  return ERR_FAILURE;
}
 
static RET_CODE on_pic_plug_out(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list_cont = NULL;
  control_t *p_list = NULL;
  u32 usb_dev_type = para2;
	
  control_t *p_cbox = NULL; 
  RET_CODE ret = SUCCESS;

  if(usb_dev_type == HP_WIFI || !is_usb)
  {
    OS_PRINTF("####wifi device  plug out or samba plug out usb#####\n");
    return ERR_FAILURE;
  }
	
  OS_PRINTF("####on_usb_music_plug_out  1111111#####\n");
	
  p_cbox = ctrl_get_child_by_id(ctrl_get_child_by_id(p_cont, IDC_PIC_GROUP_CONT),IDC_PIC_GROUP);
  MT_ASSERT(p_cbox != NULL);
	
  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST_CONT);
  p_list = ctrl_get_child_by_id(p_list_cont, IDC_PIC_LIST);

  ret = ui_change_plug_out_partition(p_list,MSG_GREEN,*(u32 *)text_get_content(p_cbox),
		play_letter,
		pic_list_proc);
  if(ret == SUCCESS)
  {
      return SUCCESS;
  }

	
  if(p_list != NULL)
  {
  //  OS_PRINTF("============on_pic_plug_out gui_stop_roll plist id = %d============\n",p_list->id);
    gui_stop_roll(p_list);
  }

  pic_stop();
  manage_close_menu(ROOT_ID_USB_PICTURE, 0, 0);

  return SUCCESS;
}


static RET_CODE on_pic_update(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_list_cont = NULL, *p_list = NULL, *p_msg = NULL;
  u16 pos;
  media_fav_t *p_media = NULL;
  BOOL ret = FALSE;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  if(g_list.p_file == NULL)
  {
    return ERR_FAILURE;
  }

  if(ui_pic_get_play_mode() == PIC_MODE_FAV)
  {
    p_list_cont = ctrl_get_child_by_id(p_cont, IDC_PIC_FAV_LIST_CONT);
    p_list = ctrl_get_child_by_id(p_list_cont, IDC_PIC_FAV_LIST_LIST);
  }
  else
  {
    p_list_cont = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST_CONT);
    p_list = ctrl_get_child_by_id(p_list_cont, IDC_PIC_LIST);
  }

  p_msg = ctrl_get_child_by_id(p_cont, IDC_PIC_MSG);
  if(OBJ_STS_SHOW == ctrl_get_sts(p_msg))
  {
    ctrl_set_sts(p_msg, OBJ_STS_HIDE);
    ctrl_erase_ctrl(p_msg);
    ctrl_paint_ctrl(ctrl_get_parent(p_list), TRUE);
  }
  ui_pic_clear(0);
  pic_stop();
  b_is_picture_error = FALSE;

  pos = list_get_focus_pos(p_list);

  if(ui_pic_get_play_mode() == PIC_MODE_FAV)
  {
    p_media = ui_pic_get_fav_media_by_index(pos);
    if(p_media == NULL)
    {
       OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
       return ERR_FAILURE;
    }
    ret = ui_pic_set_play_index_fav_by_name(p_media->path);
    if(ret == FALSE)
    {
       OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
       return ERR_FAILURE;
    }
    ui_pic_play_curn(&p_media, &g_pic_rect);
  }
  else
  {
    if(g_list.p_file[pos].type == NOT_DIR_FILE)
    {
      ret = ui_pic_set_play_index_dir_by_name(g_list.p_file[pos].name);
      if(ret == FALSE)
      {
        OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
        return ERR_FAILURE;
      }
      ui_pic_play_curn(&p_media, &g_pic_rect);
    }
  }

  return SUCCESS;
}

static RET_CODE pic_list_update(control_t* p_list, u16 start, u16 size,
                                u32 context)
{
  u16 i;
  u8 asc_str[32];
  u16 file_uniname[MAX_FILE_PATH];
  u16 cnt = list_get_count(p_list);
  flist_type_t type = FILE_TYPE_UNKNOW;
  u16 icon_id = RSC_INVALID_ID;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      /* NO. */
      sprintf(asc_str, "%.3d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);

      /* ICON */
      if(g_list.p_file[i + start].type != DIRECTORY)
      {
        type = flist_get_file_type(g_list.p_file[i + start].p_name);
        switch(type)
        {
          case FILE_TYPE_JPG:
            icon_id = IM_MP3_ICON_JPG;
            break;

          case FILE_TYPE_PNG:
            icon_id = IM_MP3_ICON_PNG;
            break;

          case FILE_TYPE_GIF:
            icon_id = IM_MP3_ICON_GIF;
            break;

          case FILE_TYPE_BMP:
            icon_id = IM_MP3_ICON_BMP;
            break;

          default:
            icon_id = RSC_INVALID_ID;
            break;
        }
        list_set_field_content_by_icon(p_list, (u16)(start + i), 1, icon_id);
      }
      else
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 1, IM_MP3_ICON_FOLDER);
      }

      /* NAME */
      //dvb_to_unicode(g_list.p_file[i + start].p_name, MAX_FILE_PATH - 1, file_uniname, MAX_FILE_PATH);
      uni_strncpy(file_uniname, g_list.p_file[i + start].p_name, MAX_FILE_PATH);

      list_set_field_content_by_unistr(p_list, (u16)(start + i), 2, file_uniname);
    }

  }

  return SUCCESS;
}

static RET_CODE pic_fav_list_update(control_t* p_list, u16 start, u16 size,
                                u32 context)
{
  u16 i;
  u8 asc_str[32];
  u16 file_uniname[MAX_FILE_PATH];
  u16 icon_id = 0;
  u16 cnt = list_get_count(p_list);
  flist_type_t type = FILE_TYPE_UNKNOW;
  media_fav_t *p_media = NULL;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  if(ui_pic_get_play_mode() == PIC_MODE_FAV)
  {
    ui_pic_get_fav_list(&p_media);
  }

  for (i = 0; i<start && p_media!=NULL; i++)
  {
    p_media = p_media->p_next;
  }

  for (i = 0; i < size; i++)
  {
    if ((i + start < cnt) && (p_media != NULL) )
    {
      /* NO. */
      sprintf(asc_str, "%.3d ", (u16)(start + i + 1));
      list_set_field_content_by_ascstr(p_list, (u16)(start + i), 0, asc_str);

      /* ICON TYPE*/
      type = flist_get_file_type(p_media->path);
      switch(type)
      {
        case FILE_TYPE_JPG:
          icon_id = IM_MP3_ICON_JPG;
          break;

        case FILE_TYPE_PNG:
          icon_id = IM_MP3_ICON_PNG;
         break;

        case FILE_TYPE_GIF:
          icon_id = IM_MP3_ICON_GIF;
          break;

        case FILE_TYPE_BMP:
          icon_id = IM_MP3_ICON_BMP;
          break;

        default:
          icon_id = RSC_INVALID_ID;
          break;
      }
      list_set_field_content_by_icon(p_list, (u16)(start + i), 1, icon_id);

      /* NAME */
      //str_asc2uni(p_media->path,file_uniname);
      //dvb_to_unicode(p_media->p_filename, MAX_FILE_PATH - 1, file_uniname, MAX_FILE_PATH);
      uni_strncpy(file_uniname, p_media->p_filename, MAX_FILE_PATH);

      list_set_field_content_by_unistr(p_list, (u16)(start + i), 2, file_uniname);

      /* ICON DEL*/
      if(ui_pic_is_one_fav_del(start + i))
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 3, IM_TV_DEL);
      }
      else
      {
        list_set_field_content_by_icon(p_list, (u16)(start + i), 3, RSC_INVALID_ID);
      }
    }

    if(p_media!=NULL)
    {
      p_media = p_media->p_next;
    }
  }

  return SUCCESS;
}

static RET_CODE on_pic_list_set_play_mode(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  manage_open_menu(ROOT_ID_PIC_PLAY_MODE_SET, ROOT_ID_USB_PICTURE, 0);

  return ret;
}


static RET_CODE on_pic_list_change_focus(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret=SUCCESS;
  u16 pos;
	control_t *p_next,*p_parent;
OS_PRINTF("@@@%s\n", __FUNCTION__);
	if(ctrl_get_ctrl_id(p_list) != IDC_PIC_LIST)
	{
		p_parent = ctrl_get_parent(p_list);
		switch(msg)
		{
			case MSG_FOCUS_UP:
				p_next = ctrl_get_child_by_id(p_parent, p_list->up);
				break;
			case MSG_FOCUS_DOWN:
				p_next = ctrl_get_child_by_id(p_parent, p_list->down);
				break;
                        case MSG_FOCUS_LEFT:
			case MSG_FOCUS_RIGHT:
				p_next = ctrl_get_child_by_id(ctrl_get_child_by_id(p_parent, IDC_PIC_LIST_CONT),IDC_PIC_LIST);
                            gui_start_roll(p_next, &roll_param);
                            manage_open_menu(ROOT_ID_FUN_HELP, 0, 0);
				break;
			default:
				return ERR_FAILURE;
				break;
		}
		ctrl_process_msg(p_list, MSG_LOSTFOCUS, 0, 0); 
		ctrl_process_msg(p_next, MSG_GETFOCUS, 0, 0); 
		ctrl_paint_ctrl(p_list,TRUE);
		ctrl_paint_ctrl(p_next,TRUE);
		return SUCCESS;
	}
	else
	{
		p_parent = ctrl_get_parent(ctrl_get_parent(p_list));
		switch(msg)
		{
			case MSG_FOCUS_UP:
			case MSG_FOCUS_DOWN:
                   case MSG_PAGE_UP:
			case MSG_PAGE_DOWN:
				   b_is_picture_error = FALSE;
                              ret = list_class_proc(p_list, msg, 0, 0);

                              pos = list_get_focus_pos(p_list);

                              if(g_list.p_file == NULL)
                              {
                                return ret;
                              }

                              ui_play_timer_start();
				break;
                case MSG_FOCUS_RIGHT:
			case MSG_FOCUS_LEFT:
				p_next = ctrl_get_child_by_id(p_parent,IDC_PIC_GROUP_TITLE1_CONT);	
                            
				ctrl_process_msg(p_list, MSG_LOSTFOCUS, 0, 0); 
				ctrl_process_msg(p_next, MSG_GETFOCUS, 0, 0); 
				ctrl_paint_ctrl(p_list,TRUE);
                            gui_stop_roll(p_list);
				ctrl_paint_ctrl(p_next,TRUE);
                            manage_close_menu(ROOT_ID_FUN_HELP, 0, 0);
				return SUCCESS;
			default:
				return ERR_FAILURE;
				break;
		}
	}
  return ret;
}

static RET_CODE on_pic_list_change_partition(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_cbox;
  u16 focus = 0, old_focus = 0;
  u16 count = 0;
  RET_CODE ret = 0;
  u16 p_unistr[3] = {0,0x3a,0};
OS_PRINTF("@@@%s\n", __FUNCTION__);
  g_partition_cnt = file_list_get_partition(&p_partition);
  //p_cbox = ctrl_get_child_by_id(ctrl_get_parent(p_list), IDC_PIC_GROUP);
  p_cbox = ctrl_get_child_by_id(ctrl_get_child_by_id(ctrl_get_parent(p_list->p_parent), IDC_PIC_GROUP_CONT), IDC_PIC_GROUP);
  
    if(para2 != 0)
  {
		 p_unistr[0] = (u16)para2;

		 cbox_dync_set_count(p_cbox, (u16)g_partition_cnt);
		 cbox_dync_set_focus(p_cbox, 0);
		 ctrl_paint_ctrl(p_cbox->p_parent, TRUE);
    ret = _ui_pic_m_get_file_list(p_partition[focus].letter);
    if(ret == SUCCESS)
    {
       count = (u16)g_list.file_count;

       list_set_count(p_list, count, PIC_LIST_PAGE);
       list_set_focus_pos(p_list, 0);
       //list_select_item(p_list, 0);

       pic_list_update(p_list, list_get_valid_pos(p_list), PIC_LIST_PAGE, 0);
       ui_pic_build_play_list_indir(&g_list);
       ctrl_paint_ctrl(p_list, TRUE);

       pic_update_path(p_list);
    }
    else
    {
       OS_PRINTF("\n file_list_enter_dir failure. \n");
		}
	}
	else
	{
		if(g_partition_cnt > 1)
		{
			cbox_dync_set_count(p_cbox, (u16)g_partition_cnt);
                   old_focus = focus = cbox_dync_get_focus(p_cbox);
			cbox_class_proc(p_cbox, MSG_INCREASE, 0, 0);
			focus = cbox_dync_get_focus(p_cbox);
                    if(old_focus == focus)
                    {
                        focus = 0;
                        cbox_dync_set_focus(p_cbox,  focus);
                    }
                    
			ret = _ui_pic_m_get_file_list(p_partition[focus].letter);
			
			if(ret == SUCCESS)
			{
				count = (u16)g_list.file_count;
				list_set_count(p_list, count, PIC_LIST_PAGE);
				list_set_focus_pos(p_list, 0);
				pic_list_update(p_list, list_get_valid_pos(p_list), PIC_LIST_PAGE, 0);
				ui_pic_build_play_list_indir(&g_list);
				ctrl_paint_ctrl(p_list, TRUE);
				pic_update_path(p_list);
			}
			else
			{
				OS_PRINTF("\n file_list_enter_dir failure. \n");
			}
    }
  }

  return SUCCESS;
}

static RET_CODE on_pic_list_volume(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
OS_PRINTF("@@@%s\n", __FUNCTION__);
    open_volume_usb(ROOT_ID_USB_PICTURE, para1);
    return SUCCESS;
}


static RET_CODE on_pic_add_to_fav_list(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  int pos = 0;
  comm_dlg_data_t dlg_data = {0};
OS_PRINTF("@@@%s\n", __FUNCTION__);
  pos = list_get_focus_pos(p_list);

  if(g_list.p_file == NULL)
  {
    return ret;
  }

  if(g_list.p_file[pos].type == NOT_DIR_FILE)
  {
    dlg_data.parent_root = ROOT_ID_USB_PICTURE;
    dlg_data.style = DLG_FOR_SHOW | DLG_STR_MODE_STATIC;
    dlg_data.x = ((SCREEN_WIDTH - 400) / 2);
  dlg_data.y = ((SCREEN_HEIGHT - 200) / 2);
    dlg_data.w = 400;
    dlg_data.h = 160;
    dlg_data.content = IDS_WAIT_PLEASE;
    dlg_data.dlg_tmout = 0;

    ui_evt_disable_ir();        
    ui_comm_dlg_open(&dlg_data);
    if(TRUE == ui_pic_add_one_fav(g_list.p_file[pos].name))
      ui_pic_save_fav_list(g_list.p_file[0].name[0]);
    ui_evt_enable_ir();
    ui_comm_dlg_close();
  }

  //set HighLight to the next item
  //list_class_proc(p_list, MSG_FOCUS_DOWN, 0, 0);
  ctrl_process_msg(p_list, MSG_FOCUS_DOWN, 0, 0);

  return SUCCESS;
}

static RET_CODE on_pic_open_fav_list(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_fav_list_cont, *p_play_list, *p_play_list_title, *p_menu, *p_msg;
  u16 count=0;
  media_fav_t *p_media = NULL;
  BOOL ret = FALSE;
  comm_dlg_data_t dlg_data_notexist =
  {
    ROOT_ID_USB_PICTURE,
    DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
    (1280- COMM_DLG_W)/2, (720 - COMM_DLG_H)/2,
    COMM_DLG_W,COMM_DLG_H,
    IDS_PLAY_LIST_NOT_EXIST,
    2000,
    FALSE,
    0,
    {0},
  };
OS_PRINTF("@@@%s\n", __FUNCTION__);
  p_fav_list_cont = ctrl_get_child_by_id(p_list->p_parent->p_parent, IDC_PIC_FAV_LIST_CONT);
  p_play_list = ctrl_get_child_by_id(p_fav_list_cont, IDC_PIC_FAV_LIST_LIST);
  p_play_list_title = ctrl_get_child_by_id(p_fav_list_cont, IDC_PIC_FAV_LIST_TITLE);
  p_menu = p_fav_list_cont->p_parent;
  p_msg = ctrl_get_child_by_id(p_menu, IDC_PIC_MSG);

  count = ui_pic_get_fav_count();
  if(count > 0)
  {
    ui_pic_set_play_mode(PIC_MODE_FAV);
    ctrl_set_attr(p_fav_list_cont, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(p_fav_list_cont, OBJ_STS_SHOW);

    ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
    ctrl_process_msg(p_play_list, MSG_GETFOCUS, 0, 0);

    text_set_content_by_strid(p_play_list_title, IDS_IMG_PLAY_LIST);
    ctrl_set_active_ctrl(p_fav_list_cont, p_play_list);

    list_set_count(p_play_list, count, PIC_PLAY_LIST_LIST_PAGE);
    list_set_focus_pos(p_play_list, 0);
    //list_select_item(p_play_list, 0);
    p_media = ui_pic_get_fav_media_by_index(0);  //  play highlight pic
    if(p_media == NULL)
    {
      OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
      return ERR_FAILURE;
    }
    pic_stop();
    ret = ui_pic_set_play_index_fav_by_name(p_media->path);
    if(ret == FALSE)
    {
      OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
      return ERR_FAILURE;
    }

    if(ctrl_get_sts(p_msg) == OBJ_STS_SHOW)
    {
      //hide too large
      ctrl_set_sts(p_msg, OBJ_STS_HIDE);
      ctrl_erase_ctrl(p_msg);
      ctrl_paint_ctrl(p_msg, TRUE);
    }    

    ui_pic_set_play_mode(PIC_MODE_FAV);
    ui_pic_play_curn(&p_media, &g_pic_rect);
    pic_fav_list_update(p_play_list, list_get_valid_pos(p_play_list), PIC_PLAY_LIST_LIST_PAGE, 0);

    ctrl_paint_ctrl(p_fav_list_cont->p_parent, TRUE);

      // empty invrgn but not draw
    //ctrl_empty_invrgn(p_list);


    gui_stop_roll(p_list);

    gui_start_roll(p_play_list, &roll_param);
  }
  else
  {
    ui_comm_dlg_open(&dlg_data_notexist);
  }

  return SUCCESS;
}

static RET_CODE on_pic_fav_list_change_focus(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  u16 pos;
  media_fav_t *p_media = NULL;
  control_t *p_pic_list;
  control_t *p_msg;
  BOOL set_ret = FALSE;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  ret = list_class_proc(p_list, msg, 0, 0);
  pos = list_get_focus_pos(p_list);
  p_media = ui_pic_get_fav_media_by_index(pos);
  
 // p_pic_list = ctrl_get_child_by_id(p_list->p_parent->p_parent, IDC_PIC_LIST);
  p_pic_list = ctrl_get_child_by_id(ui_comm_root_get_ctrl(ROOT_ID_USB_PICTURE,IDC_PIC_LIST_CONT), IDC_PIC_LIST);
  p_msg = ui_comm_root_get_ctrl(ROOT_ID_USB_PICTURE,IDC_PIC_MSG);
  if(OBJ_STS_SHOW == ctrl_get_sts(p_msg))
  {
    ctrl_set_sts(p_msg, OBJ_STS_HIDE);
    ctrl_erase_ctrl(p_msg);
    ctrl_paint_ctrl(ctrl_get_parent(p_list), TRUE);
  }

  if(g_list.p_file == NULL)
  {
    return ret;
  }

  pic_stop();
  set_ret = ui_pic_set_play_index_fav_by_name(p_media->path);
  if(set_ret == FALSE)
  {
    OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
    return ERR_FAILURE;
  }
  //pic_update_detail(p_pic_list, TRUE, FALSE);
  ui_pic_set_play_mode(PIC_MODE_FAV);
  ui_pic_play_curn(&p_media, &g_pic_rect);

  return ret;
}

static RET_CODE on_pic_list_sort(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{

  /*RET_CODE ret = SUCCESS;

  control_t *p_cont, *p_sort_list;

  p_cont = p_list->p_parent->p_parent;
  ui_comm_dlg_close();
  gui_stop_roll(p_list);
  p_sort_list = ctrl_get_child_by_id(p_cont, IDC_PIC_SORT_LIST);
  ctrl_set_attr(p_sort_list, OBJ_ATTR_ACTIVE);
  ctrl_set_sts(p_sort_list, OBJ_STS_SHOW);

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_sort_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_sort_list, TRUE);

  return ret;

*/
    RET_CODE ret = SUCCESS;
rect_t p_frame;
  control_t *p_cont, *p_sort_list;
  s16 pos=0;
  
OS_PRINTF("@@@%s\n", __FUNCTION__);
  p_cont = p_list->p_parent;
  ui_comm_dlg_close();
  gui_stop_roll(p_list);
  pos = list_get_focus_pos(p_list);
  pos = pos > 4 ? 4 : pos;
  pos = pos < 1 ? 1 : pos;
  p_sort_list = ctrl_get_child_by_id(ctrl_get_parent(p_cont), IDC_PIC_SORT_LIST);
  ctrl_set_attr(p_sort_list, OBJ_ATTR_ACTIVE);
  p_frame.top = pos * 50;
  p_frame.bottom= pos * 50 + PIC_SORT_LIST_H;
  p_frame.left= PIC_SORT_LIST_X;
  p_frame.right= PIC_SORT_LIST_X+ PIC_SORT_LIST_W;
  ctrl_resize(p_sort_list, &p_frame);
  ctrl_set_sts(p_sort_list, OBJ_STS_SHOW);

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_sort_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_sort_list, TRUE);
    return ret;
}

RET_CODE picture_list_update()
{
  control_t  *p_list;
  u16 pos;
  media_fav_t *p_media = NULL;
  BOOL ret = FALSE;
OS_PRINTF("@@@%s\n", __FUNCTION__);
//  p_list = ui_comm_root_get_ctrl(ROOT_ID_USB_PICTURE,IDC_PIC_LIST);
  p_list = ctrl_get_child_by_id(ui_comm_root_get_ctrl(ROOT_ID_USB_PICTURE,IDC_PIC_LIST_CONT), IDC_PIC_LIST);
  pos = list_get_focus_pos(p_list);

  if(g_list.p_file[pos].type == NOT_DIR_FILE)
  {
      pic_stop();
      ret = ui_pic_set_play_index_dir_by_name(g_list.p_file[pos].name);
      if(ret == FALSE)
      {
         OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
         return ERR_FAILURE;
      }
      //pic_update_detail(p_list, TRUE, FALSE);
      ui_pic_set_play_mode(PIC_MODE_NORMAL);
      ui_pic_play_curn(&p_media, &g_pic_rect);
  }
  else
  {
    pic_update_path(p_list);
    //Dir, shoulde update path
  }
  return SUCCESS;
}

RET_CODE picture_sort_update()
{
  control_t  *p_fav_list_cont,*p_fav_list;
  control_t *p_pic_list;
  u16 pos;
  media_fav_t *p_media = NULL;
  BOOL ret = FALSE;

OS_PRINTF("@@@%s\n", __FUNCTION__);
  p_fav_list_cont = ui_comm_root_get_ctrl(ROOT_ID_USB_PICTURE,IDC_PIC_FAV_LIST_CONT);
  p_fav_list = ctrl_get_child_by_id(p_fav_list_cont, IDC_PIC_FAV_LIST_LIST);

  pos = list_get_focus_pos(p_fav_list);

  p_media = ui_pic_get_fav_media_by_index(pos);
 // p_pic_list = ctrl_get_child_by_id(p_fav_list->p_parent->p_parent, IDC_PIC_LIST);
  p_pic_list = ctrl_get_child_by_id(ui_comm_root_get_ctrl(ROOT_ID_USB_PICTURE,IDC_PIC_LIST_CONT), IDC_PIC_LIST);


  pic_stop();
  ret = ui_pic_set_play_index_fav_by_name(p_media->path);
  if(ret == FALSE)
  {
     OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
     return ERR_FAILURE;
  }
  //pic_update_detail(p_pic_list, TRUE, FALSE);
  ui_pic_set_play_mode(PIC_MODE_FAV);
  ui_pic_play_curn(&p_media, &g_pic_rect);
  return SUCCESS;
}

static RET_CODE on_pic_list_select(control_t *p_list, u16 msg,u32 para1, u32 para2)
{
	int pos = 0;
	//picture_param_t p_param;
	control_t *p_root = NULL;
	RET_CODE ret = 0;
	BOOL b_set = FALSE;
	OS_PRINTF("@@@%s\n", __FUNCTION__);
	pos = list_get_focus_pos(p_list);

	if(g_list.p_file == NULL)
	{
		return SUCCESS;
	}

	switch(g_list.p_file[pos].type)
	{
		case DIRECTORY:
		{
			BOOL is_enter_dir = FALSE;
			u16 father_filename[MAX_FILE_PATH] = {0};
			u16 preDirLen = 0;
			u16 i;
			
			is_enter_dir = pic_file_list_is_enter_dir(NULL, g_list.p_file[pos].name);
			if(TRUE == is_enter_dir)
			{
				uni_strncpy(father_filename, file_list_get_cur_path(g_pic_flist_dir), MAX_FILE_PATH);
				preDirLen = uni_strlen(father_filename);
			}
			
			ret = _ui_pic_m_get_file_list(g_list.p_file[pos].name);
			if(ret == SUCCESS)
			{
				gui_stop_roll(p_list);
				list_set_count(p_list, (u16)(g_list.file_count), PIC_LIST_PAGE);
				list_set_focus_pos(p_list, 0);
				pic_list_update(p_list, list_get_valid_pos(p_list), PIC_LIST_PAGE, 0);
				ui_pic_build_play_list_indir(&g_list);

				if(is_enter_dir){
					for(i = 0; i < g_list.file_count; i++)
					{
						if(preDirLen != uni_strlen(g_list.p_file[i].name))
							continue;
						//OS_PRINTF("[PICTURE] %s preDirLen=%d i = %d \n",__FUNCTION__,preDirLen,i);
						if(uni_strcmp(father_filename, g_list.p_file[i].name) == 0)
						{
							list_set_focus_pos(p_list, i);
							list_select_item(p_list, i);
							break;
						}
					}
				}
				ctrl_paint_ctrl(p_list, TRUE);
				gui_start_roll(p_list, &roll_param);
				pic_update_path(p_list);
				g_picture_player_preview_focus = 0;
			}
			else
			{
				OS_PRINTF("\n file_list_enter_dir failure. \n");
			}
		}
			break;

		case NOT_DIR_FILE:
			if(b_is_picture_error == TRUE)
			{
				return SUCCESS;
			}
			if(fw_find_root_by_id(ROOT_ID_PICTURE) != NULL)
			{
				break;
			}
			ui_deinit_play_timer();
			ui_pic_set_play_mode(PIC_MODE_NORMAL);
			ui_pic_build_play_list_indir(&g_list);
			b_set = ui_pic_set_play_index_dir_by_name(g_list.p_file[pos].name);
			if(b_set == FALSE)
			{
				OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
				return ERR_FAILURE;
			}
			//pic_update_detail(p_list, TRUE, FALSE);
			p_param.cb = picture_list_update;

			pic_stop();
			p_root = fw_find_root_by_id(ROOT_ID_USB_PICTURE);
			//send a msg to open picture.
			if(p_root)
			{
			fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_FULLSCREEN, (u32) &p_param, 0);
			}
			manage_open_menu(ROOT_ID_PICTURE, (u32) &p_param, 0);
			break;

		default:
			break;
	}

  return SUCCESS;
}

static RET_CODE on_pic_list_next_music(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  media_fav_t *p_media = NULL;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  if(ui_music_get_play_status() == MUSIC_STAT_PLAY)
  {
    if(ui_music_pre_play_next(&p_media))
    {
      music_player_next(p_media->path);
      ui_music_reset_curn(p_media->path);
    }
  }

  return SUCCESS;
}

static RET_CODE on_pic_list_prev_music(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  media_fav_t *p_media = NULL;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  if(ui_music_get_play_status() == MUSIC_STAT_PLAY)
  {
    if(ui_music_pre_play_pre(&p_media))
    {
      music_player_next(p_media->path);
      ui_music_reset_curn(p_media->path);
    }
  }

  return SUCCESS;
}

static RET_CODE on_pic_fav_list_delet(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  u16 focus;
  OS_PRINTF("@@@%s\n", __FUNCTION__);
  focus = list_get_focus_pos(p_list);
  ui_pic_set_one_del(focus);

  pic_fav_list_update(p_list, list_get_valid_pos(p_list), PIC_LIST_PAGE, 0);
  list_class_proc(p_list, MSG_FOCUS_DOWN, 0, 0);

  list_draw_item_ext(p_list, focus, TRUE);

  return SUCCESS;
}

static RET_CODE on_pic_fav_list_delte_all(control_t *p_fav_list, u16 msg,
  u32 para1, u32 para2)
{
  u16 count = 0, index = 0;
  u16 num = 0;
  OS_PRINTF("@@@%s\n", __FUNCTION__);
  count = list_get_count(p_fav_list);

  for(index = 0; index < count; index++)
  {
    if(ui_pic_is_one_fav_del(index))
    {
     num++;
    }
  }
  if(num == count)
  {
    for(index = 0; index < count; index++)
    {
    ui_pic_set_one_del(index);
    }
  }
  else
  {
    for(index = 0; index < count; index++)
    { 
      if(ui_pic_is_one_fav_del(index))
      {
      ui_pic_set_one_del(index);
      }
    }
    for(index = 0; index < count; index++)
    { 
      ui_pic_set_one_del(index);
    }
  }
  pic_fav_list_update(p_fav_list, list_get_valid_pos(p_fav_list), PIC_LIST_PAGE, 0);
  ctrl_paint_ctrl(p_fav_list, TRUE);

  return SUCCESS;
}

static void pic_fav_list_save_del()
{
  u16 i;
  OS_PRINTF("@@@%s\n", __FUNCTION__);
  //ui_pic_save_del(g_list.p_file[0].name[0]);
  for(i = 0;i < g_partition_cnt; i++)
    {
      ui_pic_save_del(p_partition[i].letter[0]);
    }
}

static void pic_fav_list_undo_save_del()
{
OS_PRINTF("@@@%s\n", __FUNCTION__);
  ui_pic_undo_save_del();
}

static RET_CODE on_pic_fav_list_select(control_t *p_fav_list, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_fav_list_cont, *p_play_list, *p_menu, *p_root;
  u16 focus = 0, count = 0;
  media_fav_t *p_focus_media = NULL;
  BOOL ret = FALSE;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  focus = list_get_focus_pos(p_fav_list);

  p_fav_list_cont = ctrl_get_parent(p_fav_list);
  p_menu = ctrl_get_parent(p_fav_list_cont);
  //p_play_list = ctrl_get_child_by_id(p_menu, IDC_PIC_LIST);
  p_play_list = ctrl_get_child_by_id(ui_comm_root_get_ctrl(ROOT_ID_USB_PICTURE,IDC_PIC_LIST_CONT), IDC_PIC_LIST);


  if(ui_pic_is_del_modified())
  {
    ui_comm_ask_for_savdlg_open(&p_dlg_rc, IDS_MSG_ASK_FOR_SAV,
      pic_fav_list_save_del, pic_fav_list_undo_save_del, 0);

    if(fw_find_root_by_id(ROOT_ID_USB_PICTURE) == NULL)
    {
      return ERR_FAILURE;
    }

    //reset focus & count, update fav list.
    count = ui_pic_get_play_count_of_fav();
    list_set_count(p_fav_list, count, PIC_PLAY_LIST_LIST_PAGE);

    if(focus >= count)
    {
      list_set_focus_pos(p_fav_list, 0);
    }
    pic_fav_list_update(p_fav_list, list_get_valid_pos(p_fav_list), PIC_PLAY_LIST_LIST_PAGE, 0);
  }

  if(ui_pic_get_play_count_of_fav() > 0)
  {
    //get current focus & focus media.
    focus = list_get_focus_pos(p_fav_list);

    p_focus_media = ui_pic_get_fav_media_by_index(focus);
    ui_pic_clear(0);//clear picture buffer
    if(p_focus_media == NULL)
    {
       OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
       return ERR_FAILURE;
    }
    //fav list count > 0, then play it.
    ui_pic_set_play_mode(PIC_MODE_FAV);
    ret = ui_pic_set_play_index_fav_by_name(p_focus_media->path);
    if(ret == FALSE)
    {
       OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
       return ERR_FAILURE;
    }
    p_param.cb = picture_sort_update;


    pic_stop();
    p_root = fw_find_root_by_id(ROOT_ID_USB_PICTURE);
    //send a msg to open picture.
    if(p_root)
    {
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_FULLSCREEN, (u32) &p_param, 0);
    }
    manage_open_menu(ROOT_ID_PICTURE, (u32) &p_param, 0);
  }
  else
  {
    //hide the fav list & focus on the normal list.
    gui_stop_roll(p_fav_list);
    ctrl_set_sts(p_fav_list_cont, OBJ_STS_HIDE);
    ctrl_process_msg(p_fav_list, MSG_LOSTFOCUS, 0, 0);
    ctrl_process_msg(p_play_list, MSG_GETFOCUS, 0, 0);

    pic_list_update(p_play_list, list_get_valid_pos(p_play_list), PIC_LIST_PAGE, 0);

    ctrl_paint_ctrl(p_menu, TRUE);

    ui_pic_set_play_mode(PIC_MODE_NORMAL);

    p_root = fw_find_root_by_id(ROOT_ID_USB_PICTURE);

    if(p_root)
    {
    fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
    }
  }

  return SUCCESS;
}

static RET_CODE on_pic_fav_list_exit(control_t *p_fav_list, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_menu, *p_fav_cont, *p_normal_list, *p_root;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  gui_stop_roll(p_fav_list);

  if(ui_pic_is_del_modified())
  {
    ui_comm_ask_for_savdlg_open(&p_dlg_rc, IDS_MSG_ASK_FOR_SAV,
                                  pic_fav_list_save_del, pic_fav_list_undo_save_del, 0);
  }

  if(fw_find_root_by_id(ROOT_ID_USB_PICTURE) == NULL)
  {
    return ERR_FAILURE;
  }

  p_fav_cont = ctrl_get_parent(p_fav_list);
  p_menu = ctrl_get_parent(p_fav_cont);
  //p_normal_list = ctrl_get_child_by_id(p_menu, IDC_PIC_LIST);
  p_normal_list = ctrl_get_child_by_id(ui_comm_root_get_ctrl(ROOT_ID_USB_PICTURE,IDC_PIC_LIST_CONT), IDC_PIC_LIST);


  ctrl_process_msg(p_fav_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_normal_list, MSG_GETFOCUS, 0, 0);

  ctrl_set_sts(p_fav_cont, OBJ_STS_HIDE);

  ctrl_paint_ctrl(p_menu, TRUE);

  ui_pic_set_play_mode(PIC_MODE_NORMAL);

  p_root = fw_find_root_by_id(ROOT_ID_USB_PICTURE);

  if(p_root)
  {
  fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
  }
  gui_start_roll(p_normal_list, &roll_param);

  return SUCCESS;
}


static RET_CODE pic_sort_list_update(control_t* p_list, u16 start, u16 size,
                                 u32 context)
{
  u16 i;
  u16 cnt = list_get_count(p_list);
  u16 strid [7] = {
    IDS_SORT_DEFAULT,
    IDS_SORT_NAME_AZ,
    IDS_SORT_NAME_ZA,
    IDS_SETUP,
    IDS_PARTITION,
    IDS_FAV,
    IDS_PLAY_LIST,
  };
OS_PRINTF("@@@%s\n", __FUNCTION__);
  for (i = 0; i < size; i++)
  {
    if (i + start < cnt)
    {
      list_set_field_content_by_strid(p_list, (u16)(start + i), 0,
                                      strid[start + i]);
    }
  }
  return SUCCESS;
}

static RET_CODE on_pic_sort_list_select(control_t *p_sort_list, u16 msg, u32 para1,
                               u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_cont;
  control_t *p_pic_list;
  u16 pos;
  int i;
  u16 pic_no;
  u16 focus_name[MAX_FILE_PATH] = {0};
  flist_sort_t sort_type = DEFAULT_ORDER_MODE;
  media_fav_t *p_media = NULL;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  p_cont = p_sort_list->p_parent->p_parent;
  //p_pic_list = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST);

  p_pic_list = ctrl_get_child_by_id(ui_comm_root_get_ctrl(ROOT_ID_USB_PICTURE,IDC_PIC_LIST_CONT), IDC_PIC_LIST);
  //ui_pic_get_cur(&p_media);
  pic_no = list_get_focus_pos(p_pic_list);

  ctrl_process_msg(p_sort_list, MSG_LOSTFOCUS, para1, para2);

  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);

  ctrl_paint_ctrl(p_cont, TRUE);

  pos = list_get_focus_pos(p_sort_list);
  switch(pos)
  {
    case 0:
      sort_type = DEFAULT_ORDER_MODE;
      break;

    case 1:
      sort_type = A_Z_MODE;
      break;

    case 2:
      sort_type = Z_A_MODE;
      break;
      case 3:
      on_pic_list_set_play_mode(p_pic_list,0,0,0);
                ctrl_process_msg(p_pic_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_pic_list, TRUE);

  gui_start_roll(p_pic_list, &roll_param);
  break;
    case 4:
        on_pic_list_change_partition(p_pic_list,0,0,0);
        ctrl_process_msg(p_pic_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_pic_list, TRUE);

  gui_start_roll(p_pic_list, &roll_param);
      return SUCCESS;
    case 5:
    on_pic_add_to_fav_list(p_pic_list,0,0,0);
      ctrl_process_msg(p_pic_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_pic_list, TRUE);

  gui_start_roll(p_pic_list, &roll_param);
    return SUCCESS;
        case 6:
on_pic_open_fav_list(p_pic_list,0,0,0);
    return SUCCESS;
    default:
      sort_type = DEFAULT_ORDER_MODE;
      break;
  }

  if(g_list.p_file == NULL)
  {
   gui_start_roll(p_pic_list, &roll_param);
   return ret;
  }
  
  memcpy(focus_name, g_list.p_file[pic_no].name, MAX_FILE_PATH * sizeof(u16));

  file_list_sort(&g_list, g_list.file_count, sort_type);
  pic_list_update(p_pic_list, list_get_valid_pos(p_pic_list), PIC_LIST_PAGE, 0);

  for(i = 0; i < g_list.file_count; i++)
  {
    if(uni_strcmp(focus_name, g_list.p_file[i].name) == 0)
    {
      list_set_focus_pos(p_pic_list, i);
      list_select_item(p_pic_list, i);
      pic_no = i;
    }
  }

  ctrl_process_msg(p_pic_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(p_pic_list, TRUE);
   if(g_list.p_file[pic_no].type == NOT_DIR_FILE)
   {
   pic_stop();
   ui_pic_set_play_mode(PIC_MODE_NORMAL);
   ui_pic_play_curn(&p_media, &g_pic_rect);
   }

 gui_start_roll(p_pic_list, &roll_param);
  return ret;
}

static RET_CODE on_pic_sort_list_exit(control_t *p_sort_list, u16 msg, u32 para1,
                               u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_cont;
  control_t *p_mp_list;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  p_cont = p_sort_list->p_parent;
  //p_mp_list = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST);
  p_mp_list = ctrl_get_child_by_id(ui_comm_root_get_ctrl(ROOT_ID_USB_PICTURE,IDC_PIC_LIST_CONT), IDC_PIC_LIST);


  ctrl_process_msg(p_sort_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_mp_list, MSG_GETFOCUS, 0, 0);

  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);

  ctrl_paint_ctrl(p_cont, TRUE);
  gui_start_roll(p_mp_list, &roll_param);
  return ret;
}

RET_CODE open_usb_picture(u32 para1, u32 para2)
{
  control_t *p_menu, *p_group_cont, *p_group, *p_list_cont;
  control_t *p_detail_cont, *p_preview, *p_msg = NULL;
  control_t *p_play_list_cont, *p_play_list_title, *p_play_list;
  control_t *p_detail_filename;
  control_t *p_list;// *p_list_sbar;
  control_t *p_sort_list;
  control_t *p_ctrl;//*p_help, 
  control_t *p_detail_pix, *p_detail_file_size;
  control_t *p_path,*p_bottom_help;
  u16 i;
  u16 count=0;
  control_t *p_title;
  u16 y;

  comm_help_data_t help =
  {
    8,
    4,
    {
      IDS_PLAY_LIST,
      IDS_SORT,
      IDS_SETUP,
      IDS_PARTITION,
      IDS_VOLUME,
      IDS_SWITCH_TYPE,
      IDS_EXIT,
      IDS_FAV,
      //IDS_ADD_PALY_LIST,
    },

    {
      IM_HELP_YELLOW,
      IM_HELP_RED,
      IM_HELP_BLUE,
      IM_HELP_GREEN,
      IM_CHANGE,
      IM_HELP_INFOR,
      IM_HELP_EXIT,
      IM_HELP_FAV,
      //IM_EPG_COLORBUTTON_GREEN,
    },
  };
  comm_help_data_t help_network =
  {
    4,
    4,
    {
      IDS_SELECT,
      IDS_BACK,
      IDS_SWITCH_TYPE,
      IDS_NETWORK_PLACE,
    },

    {
      IM_HELP_OK,
      IM_HELP_RECALL,
      IM_HELP_INFOR,
      IM_HELP_GREEN,
    },
  };

 /*  u16 str_id[] =
  {
    IDS_VIDEO,IDS_MUSIC,IDS_PICTURE,IDS_RECORD
  };*/

  u16 file_name[MAX_FILE_PATH] = {0};
  hfile_t file = NULL;
  u16 *p_name_filter = NULL;
  comm_dlg_data_t dlg_data = {0};	   
 u16 bmp_item[3] = {
IM_XMEDIO_LEFT_VEDIO,IM_XMEDIO_LEFT_MUSIC,IM_XMEDIO_LEFT_PIC_F
};
  is_usb = (para1 == 0)?1:0;
  if(!is_usb)
  {
     memcpy(ipaddress, (char *)para2, 128);
  }

#ifdef ENABLE_ADS
  pic_adv_stop();
  ui_adv_pic_release();
#endif

 ui_pic_init(PIC_SOURCE_FILE);

  dlg_data.parent_root = ROOT_ID_BACKGROUND;
  dlg_data.style = DLG_FOR_SHOW | DLG_STR_MODE_STATIC;
  dlg_data.x = ((SCREEN_WIDTH - 400) / 2);
  dlg_data.y = ((SCREEN_HEIGHT - 200) / 2);
  dlg_data.w = 400;
  dlg_data.h = 160;
  dlg_data.content = IDS_WAIT_PLEASE;
  dlg_data.dlg_tmout = 0;
  //initialize global variables.
  if(is_usb)
  {
  p_partition = NULL;
  g_partition_cnt = 0;
  g_partition_cnt = file_list_get_partition(&p_partition);


  if(g_partition_cnt > 0)
  {
     ui_comm_dlg_open(&dlg_data); 
     ui_evt_disable_ir();
     _ui_pic_m_free_dir_and_list();
     p_name_filter = mtos_malloc(sizeof(u16) *(strlen(p_g_pic_filter) + 1));
     MT_ASSERT(p_name_filter != NULL);
     memset(p_name_filter, 0 , sizeof(u16) *(strlen(p_g_pic_filter) + 1));
     str_asc2uni(p_g_pic_filter, p_name_filter);
     g_pic_flist_dir = file_list_enter_dir(p_name_filter, MAX_FILE_COUNT, p_partition[0].letter);
     mtos_free(p_name_filter);
     if(NULL == g_pic_flist_dir)
     {
         ui_comm_dlg_close();    
         OS_PRINTF("[%s]: ##ERR## file list null\n", __FUNCTION__);
         count = 0;
     }
     else
     {
         file_list_get(g_pic_flist_dir, FLIST_UNIT_FIRST, &g_list);
         ui_comm_dlg_close();
         count = (u16)g_list.file_count;
     }
       
     for(i = 0;i < g_partition_cnt; i++)
     {
        file_name[0] = p_partition[i].letter[0];
        str_asc2uni(":\\pic_fav_list", file_name + 1);

        file = vfs_open(file_name, VFS_READ);
        if(file != NULL)
        {
            ui_pic_load_fav_list(p_partition[i].letter[0]);
        }

        vfs_close(file);
    }
  }
  else
  {
    ui_pic_set_play_mode(PIC_MODE_NORMAL);
    memset(&g_list, 0, sizeof(file_list_t));
    memset(&cur_play_time, 0, sizeof(utc_time_t));
    memset(&total_play_time, 0, sizeof(utc_time_t));
  }
 }
  else
  {
    ui_get_ip_path(ip_address_with_path);
    ui_comm_dlg_open(&dlg_data); 
    ui_evt_disable_ir();
    _ui_pic_m_free_dir_and_list();
    p_name_filter = mtos_malloc(sizeof(u16) *(strlen(p_g_pic_filter) + 1));
    MT_ASSERT(p_name_filter != NULL);
    memset(p_name_filter, 0 , sizeof(u16) *(strlen(p_g_pic_filter) + 1));
    str_asc2uni(p_g_pic_filter, p_name_filter);
    g_pic_flist_dir = file_list_enter_dir(p_name_filter, MAX_FILE_COUNT, ip_address_with_path);
    mtos_free(p_name_filter);
    if(NULL == g_pic_flist_dir)
    {
       ui_comm_dlg_close();    
       OS_PRINTF("[%s]: ##ERR## file list null\n", __FUNCTION__);
       count = 0;
    }
    else
    {
       file_list_get(g_pic_flist_dir, FLIST_UNIT_FIRST, &g_list);
       ui_comm_dlg_close();
       count = (u16)g_list.file_count;
    }
   }

  b_is_picture_error = FALSE;
  OS_PRINTF("total count = %d\n",count);
  ui_pic_build_play_list_indir(&g_list);

  /*Create Menu*/
  p_menu = ui_comm_root_create(ROOT_ID_USB_PICTURE,
    0, COMM_BG_X, COMM_BG_Y, COMM_BG_W,  COMM_BG_H, 0, IDS_MULTI_MEDIA);
  if(p_menu == NULL)
  {
    ui_evt_enable_ir();
    return ERR_FAILURE;
  }
  ctrl_set_rstyle(p_menu, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  ctrl_set_keymap(p_menu, pic_cont_keymap);
  ctrl_set_proc(p_menu, pic_cont_proc);
    p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_PIC_CTRL_CONT,
                            0, 0, TITLE_W, TITLE_H,
                            p_menu, 0);
     ctrl_set_rstyle(p_ctrl, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);

     p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_PIC_LEFT_BG,
                            MP_LEFT_BG_X, 
                            MP_LEFT_BG_Y, 
                            MP_LEFT_BG_W, 
                            MP_LEFT_BG_H,
                            p_menu, 0);
     ctrl_set_rstyle(p_ctrl, RSI_MEDIO_LEFT_BG, RSI_MEDIO_LEFT_BG, RSI_MEDIO_LEFT_BG);
     p_ctrl = ctrl_create_ctrl(CTRL_CONT, IDC_PIC_LEFT_HD,
                            MP_LEFT_HD_X, MP_LEFT_HD_Y, MP_LEFT_HD_W, MP_LEFT_HD_H,
                            p_menu, 0);
     ctrl_set_rstyle(p_ctrl, RSI_MEDIO_LEFT_LINE_BG, RSI_MEDIO_LEFT_LINE_BG, RSI_MEDIO_LEFT_LINE_BG);
 y = MP_TITLE_TEXT1_Y;

 for(i = 0;i<3;i++)
 {
  p_title = 	ctrl_create_ctrl(CTRL_TEXT, IDC_PIC_GROUP_TITLE1_CONT+i,
         MP_TITLE_TEXT1_X,y, MP_TITLE_TEXT1_W,
			MP_TITLE_TEXT1_H, p_menu,
			0);
  
  ctrl_set_rstyle(p_title, RSI_MEDIO_BTN_SH, RSI_MEDIO_BTN_HL, RSI_MEDIO_BTN_SH);
    ctrl_set_keymap(p_title, _ui_pic_v_txt_keymap);
    ctrl_set_proc(p_title, _ui_pic_v_txt_proc);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
 // text_set_content_by_strid(p_title, str_id[i]);
	if(i == 2)
		{
		 ctrl_set_attr(p_title, OBJ_ATTR_INACTIVE);
	}
	switch(i)
	{
		case 0:
			ctrl_set_related_id(p_title,
                    0,                                                            /* left */
                    (u16)IDC_PIC_GROUP_TITLE2_CONT,                  /* up */
                    0,                                                            /* right */
                    (u16)IDC_PIC_GROUP_TITLE2_CONT); /* down */
		break;
		case 1:
			ctrl_set_related_id(p_title,
                    0,                                                            /* left */
                    (u16)IDC_PIC_GROUP_TITLE1_CONT,                  /* up */
                    0,                                                            /* right */
                    (u16)IDC_PIC_GROUP_TITLE1_CONT); /* down */
		break;
		case 3:
			ctrl_set_related_id(p_title,
                    0,                                                            /* left */
                    (u16)IDC_PIC_GROUP_TITLE2_CONT,                  /* up */
                    0,                                                            /* right */
                    (u16)IDC_PIC_GROUP_TITLE1_CONT); /* down */
		break;
		default:
		break;
	}
    p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PIC_GROUP_BMP1_CONT + i,
												MP_TITLE_TEXT1_X, 0,MP_TITLE_TEXT1_W,
			MP_TITLE_TEXT1_H, p_title,
			0);
			bmap_set_content_by_id(p_ctrl, bmp_item[i]);
  y = y+ MP_TITLE_TEXT1_H;
 }

 p_title = ctrl_get_child_by_id(p_menu, IDC_PIC_GROUP_TITLE4_CONT);
 if(!is_usb)
 {
  ctrl_set_attr(p_title, OBJ_ATTR_INACTIVE);
 }

//create group container
  p_group_cont =
			ctrl_create_ctrl(CTRL_CONT, IDC_PIC_GROUP_CONT, MP_GROUP_CONTX,
			MP_GROUP_CONTY, MP_GROUP_CONTW,
			MP_GROUP_CONTH, p_menu,
			0);
ctrl_set_rstyle(p_group_cont, RSI_ITEM_1_SH, RSI_ITEM_1_SH, RSI_ITEM_1_SH);

 if(is_usb)
  {
  //group arrow
  /*p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PIC_GROUP_ARROWL,
                             MP_GROUP_ARROWL_X, MP_GROUP_ARROWL_Y,
                             MP_GROUP_ARROWL_W, MP_GROUP_ARROWL_H,
                             p_group_cont, 0);
  bmap_set_content_by_id(p_ctrl, IM_ARROW_L);

  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_PIC_GROUP_ARROWR,
                             MP_GROUP_ARROWR_X, MP_GROUP_ARROWR_Y,
                             MP_GROUP_ARROWR_W, MP_GROUP_ARROWR_H,
                             p_group_cont, 0);
  bmap_set_content_by_id(p_ctrl, IM_ARROW_R);*/

  //group
      p_group = ctrl_create_ctrl(CTRL_CBOX, IDC_PIC_GROUP, MP_GROUP_X,
                                 MP_GROUP_Y, MP_GROUP_W,
                                 MP_GROUP_H, p_group_cont, 0);
      ctrl_set_rstyle(p_group, RSI_PBACK, RSI_PBACK, RSI_PBACK);
      cbox_set_font_style(p_group, FSI_VERMILION, FSI_VERMILION, FSI_VERMILION);
      cbox_set_align_style(p_group, STL_CENTER | STL_VCENTER);
      cbox_set_work_mode(p_group, CBOX_WORKMODE_DYNAMIC);
      cbox_enable_cycle_mode(p_group, TRUE);

      cbox_dync_set_count(p_group, (u16)g_partition_cnt);
      cbox_dync_set_update(p_group, pic_group_update);
      cbox_dync_set_focus(p_group, 0);
  }
 else
  {
      //group
      p_group = ctrl_create_ctrl(CTRL_CBOX, IDC_PIC_GROUP, MP_GROUP_X,
                                 MP_GROUP_Y, MP_GROUP_W,
                                 MP_GROUP_H, p_group_cont, 0);
      ctrl_set_rstyle(p_group, RSI_PBACK, RSI_PBACK, RSI_PBACK);
      cbox_set_font_style(p_group, FSI_VERMILION, FSI_VERMILION, FSI_VERMILION);
      cbox_set_align_style(p_group, STL_CENTER | STL_VCENTER);
      cbox_set_work_mode(p_group, CBOX_WORKMODE_DYNAMIC);
      cbox_enable_cycle_mode(p_group, TRUE);

      cbox_dync_set_count(p_group, (u16)g_partition_cnt);
      cbox_dync_set_update(p_group, pic_network_group_update);
      cbox_dync_set_focus(p_group, 0);
  }
 ctrl_set_sts(p_group_cont, OBJ_STS_HIDE);
  //detail information
  p_detail_cont = ctrl_create_ctrl(CTRL_CONT, IDC_PIC_DETAIL_CONT, USB_PIC_DETAIL_CONT_X,
    USB_PIC_DETAIL_CONT_Y, USB_PIC_DETAIL_CONT_W, USB_PIC_DETAIL_CONT_H, p_menu, 0);
  ctrl_set_rstyle(p_detail_cont, RSI_MEDIO_PLAY_BG, RSI_MEDIO_PLAY_BG, RSI_MEDIO_PLAY_BG);

  p_detail_filename = ctrl_create_ctrl(CTRL_TEXT, IDC_PIC_DETAIL_FILENAME,
    PIC_DETAIL_FILENAME_X, PIC_DETAIL_FILENAME_Y,
    PIC_DETAIL_FILENAME_W, MP_DETAIL_FILENAME_H, p_detail_cont, 0);
  ctrl_set_rstyle(p_detail_filename, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_detail_filename, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX);
  text_set_align_type(p_detail_filename, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_detail_filename, TEXT_STRTYPE_UNICODE);

  p_detail_pix = ctrl_create_ctrl(CTRL_TEXT, IDC_PIC_DETAIL_PIX_TIME,
    MP_DETAIL_PIX_TIME_X, PIC_DETAIL_PIX_Y,
    PIC_DETAIL_PIX_W, MP_DETAIL_PIX_TIME_H, p_detail_cont, 0);
  ctrl_set_rstyle(p_detail_pix, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_detail_pix, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX);
  text_set_align_type(p_detail_pix, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_detail_pix, TEXT_STRTYPE_UNICODE);

  p_detail_file_size = ctrl_create_ctrl(CTRL_TEXT, IDC_PIC_DETAIL_FILE_SIZE,
    MP_DETAIL_FILE_SIZE_X, PIC_DETAIL_FILE_SIZE_Y,
    PIC_DETAIL_FILE_SIZE_W, MP_DETAIL_FILE_SIZE_H, p_detail_cont, 0);
  ctrl_set_rstyle(p_detail_file_size, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_detail_file_size, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX);
  text_set_align_type(p_detail_file_size, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_detail_file_size, TEXT_STRTYPE_UNICODE);

  //preview window
  p_preview = ctrl_create_ctrl(CTRL_CONT, IDC_PIC_PREVIEW, MP_PREV_X,
    MP_PREV_Y, MP_PREV_W, MP_PREV_H, p_menu, 0);
  ctrl_set_rstyle(p_preview, RSI_MP_PREV, RSI_MP_PREV, RSI_MP_PREV);
 ctrl_set_sts(p_preview, OBJ_STS_HIDE);
  p_msg = ctrl_create_ctrl(CTRL_TEXT, IDC_PIC_MSG, PIC_MSG_X, PIC_MSG_Y,
    PIC_MSG_W, PIC_MSG_H, p_menu, 0);
  ctrl_set_rstyle(p_msg, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
  text_set_align_type(p_msg, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_msg, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_msg, TEXT_STRTYPE_STRID);
  //scrollbar
/*  p_list_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_PIC_SBAR, USB_PIC_LIST_SBARX,
    USB_PIC_LIST_SBARY, USB_PIC_LIST_SBARW, USB_PIC_LIST_SBARH, p_menu, 0);
  ctrl_set_rstyle(p_list_sbar, RSI_MP_SBAR, RSI_MP_SBAR, RSI_MP_SBAR);
  sbar_set_autosize_mode(p_list_sbar, 1);
  sbar_set_direction(p_list_sbar, 0);
  sbar_set_mid_rstyle(p_list_sbar, RSI_MP_SBAR_MID, RSI_MP_SBAR_MID,
                     RSI_MP_SBAR_MID);
  ctrl_set_mrect(p_list_sbar, USB_PIC_LIST_SBAR_MIDL, USB_PIC_LIST_SBAR_MIDT,
    USB_PIC_LIST_SBAR_MIDR-6, USB_PIC_LIST_SBAR_MIDB);*/

  //create plist container
		p_list_cont =
			ctrl_create_ctrl(CTRL_CONT, IDC_PIC_LIST_CONT, PIC_LIST_CONTX,
			PIC_LIST_CONTY, PIC_LIST_CONTW,
			PIC_LIST_CONTH, p_menu,
			0);
		ctrl_set_rstyle(p_list_cont, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG);
  //list
  p_list = ctrl_create_ctrl(CTRL_LIST, IDC_PIC_LIST, USB_PIC_LISTX,
    USB_PIC_LISTY, USB_PIC_LISTW, USB_PIC_LISTH, p_list_cont, 0);
  ctrl_set_rstyle(p_list, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG);
  if(is_usb)
  {
      ctrl_set_keymap(p_list, pic_list_keymap);
      ctrl_set_proc(p_list, pic_list_proc);
  }
  else
  {
      ctrl_set_keymap(p_list, pic_network_list_keymap);
      ctrl_set_proc(p_list, pic_network_list_proc);
  }
  ctrl_set_mrect(p_list, USB_PIC_LIST_MIDL, USB_PIC_LIST_MIDT,
                    USB_PIC_LIST_MIDW+USB_PIC_LIST_MIDL, USB_PIC_LIST_MIDH+USB_PIC_LIST_MIDT);
  list_set_item_interval(p_list, USB_PIC_LCONT_LIST_VGAP);
  list_set_item_rstyle(p_list, &pic_list_item_rstyle);
  list_enable_select_mode(p_list, TRUE);
  list_set_select_mode(p_list, LIST_SINGLE_SELECT);
  list_set_count(p_list, count, PIC_LIST_PAGE);
  list_set_field_count(p_list, PIC_LIST_FIELD, PIC_LIST_PAGE);
  list_set_focus_pos(p_list, 0);
  //list_select_item(p_list, 0);
  list_set_update(p_list, pic_list_update, 0);
 // ctrl_set_style(p_list, STL_EX_ALWAYS_HL);

  for (i = 0; i < PIC_LIST_FIELD; i++)
  {
    list_set_field_attr(p_list, (u8)i, (u32)(mp_list_attr[i].attr), (u16)(mp_list_attr[i].width),
                        (u16)(mp_list_attr[i].left), (u8)(mp_list_attr[i].top));
    list_set_field_rect_style(p_list, (u8)i, mp_list_attr[i].rstyle);
    list_set_field_font_style(p_list, (u8)i, mp_list_attr[i].fstyle);
  }
  //list_set_scrollbar(p_list, p_list_sbar);
  pic_list_update(p_list, list_get_valid_pos(p_list), PIC_LIST_PAGE, 0);

  //path text
  p_path = ctrl_create_ctrl(CTRL_TEXT, IDC_PIC_PATH,
    MP_PATH_TEXT_X, MP_PATH_TEXT_Y,
    MP_PATH_TEXT_W, MP_PATH_TEXT_H, p_menu, 0);
  ctrl_set_rstyle(p_path, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_path, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX);
  text_set_align_type(p_path, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_path, TEXT_STRTYPE_UNICODE);
  if(g_pic_flist_dir != NULL)
  {
    if(is_usb)
    {
    text_set_content_by_unistr(p_path, p_partition[0].name);
    }
    else
    {
     text_set_content_by_unistr(p_path, file_list_get_cur_path(g_pic_flist_dir));
    }
  }

  //fav play list container
  p_play_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_PIC_FAV_LIST_CONT,
      MP_PLAY_LIST_CONT_X, MP_PLAY_LIST_CONT_Y,
      MP_PLAY_LIST_CONT_W, MP_PLAY_LIST_CONT_H, p_menu, 0);
  ctrl_set_rstyle(p_play_list_cont, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG);
  ctrl_set_sts(p_play_list_cont, OBJ_STS_HIDE);

  //play list title
  p_play_list_title  = ctrl_create_ctrl(CTRL_TEXT, IDC_PIC_FAV_LIST_TITLE,
    MP_PLAY_LIST_TITLE_X, MP_PLAY_LIST_TITLE_Y,
    MP_PLAY_LIST_TITLE_W, MP_PLAY_LIST_TITLE_H,
    p_play_list_cont, 0);
  ctrl_set_rstyle(p_play_list_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_play_list_title, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX);
  text_set_align_type(p_play_list_title, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_play_list_title, TEXT_STRTYPE_STRID);

  //play list scrollbar
/*  p_list_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_PIC_FAV_LIST_SBAR, MP_PLAY_LIST_SBARX,
    MP_PLAY_LIST_SBARY, MP_PLAY_LIST_SBARW, MP_PLAY_LIST_SBARH, p_play_list_cont, 0);
  ctrl_set_rstyle(p_list_sbar, RSI_MP_SBAR, RSI_MP_SBAR, RSI_MP_SBAR);
  sbar_set_autosize_mode(p_list_sbar, 1);
  sbar_set_direction(p_list_sbar, 0);
  sbar_set_mid_rstyle(p_list_sbar, RSI_MP_SBAR_MID, RSI_MP_SBAR_MID,
                     RSI_MP_SBAR_MID);
  ctrl_set_mrect(p_list_sbar, MP_PLAY_LIST_SBAR_MIDL, MP_PLAY_LIST_SBAR_MIDT,
    MP_PLAY_LIST_SBAR_MIDR-6, MP_PLAY_LIST_SBAR_MIDB);*/

  //play list
  p_play_list = ctrl_create_ctrl(CTRL_LIST, IDC_PIC_FAV_LIST_LIST, MP_PLAY_LIST_LIST_X,
    MP_PLAY_LIST_LIST_Y, MP_PLAY_LIST_LIST_W, MP_PLAY_LIST_LIST_H, p_play_list_cont, 0);
  ctrl_set_rstyle(p_play_list, RSI_MP_LCONT_LIST, RSI_MP_LCONT_LIST, RSI_MP_LCONT_LIST);
  ctrl_set_keymap(p_play_list, pic_fav_list_list_keymap);
  ctrl_set_proc(p_play_list, pic_fav_list_list_proc);
  ctrl_set_mrect(p_play_list, MP_PLAY_LIST_LIST_MIDL, MP_PLAY_LIST_LIST_MIDT,
                    MP_PLAY_LIST_LIST_MIDW+MP_PLAY_LIST_LIST_MIDL, MP_PLAY_LIST_LIST_MIDH+MP_PLAY_LIST_LIST_MIDT);
  list_set_item_interval(p_play_list, MP_PLAY_LIST_LIST_VGAP);
  list_set_item_rstyle(p_play_list, &pic_list_item_rstyle);
  list_enable_select_mode(p_play_list, TRUE);
  list_set_select_mode(p_play_list, LIST_SINGLE_SELECT);
  list_set_count(p_play_list, count, PIC_PLAY_LIST_LIST_PAGE);
  list_set_field_count(p_play_list, PIC_PLAY_LIST_LIST_FIELD, PIC_PLAY_LIST_LIST_PAGE);
  list_set_focus_pos(p_play_list, 0);
  list_set_update(p_play_list, pic_fav_list_update, 0);
  //list_select_item(p_play_list, 0);

  for (i = 0; i < PIC_LIST_FIELD; i++)
  {
    list_set_field_attr(p_play_list, (u8)i, (u32)(mp_play_list_attr[i].attr), (u16)(mp_play_list_attr[i].width),
                        (u16)(mp_play_list_attr[i].left), (u8)(mp_play_list_attr[i].top));
    //list_set_field_rect_style(p_play_list, (u8)i, mp_play_list_attr[i].rstyle);
    list_set_field_font_style(p_play_list, (u8)i, mp_play_list_attr[i].fstyle);
  }
 // list_set_scrollbar(p_play_list, p_list_sbar);

  //play list help
 /* p_help = ctrl_create_ctrl(CTRL_MBOX, IDC_PIC_FAV_LIST_HELP,
    MP_PLAY_LIST_HELP_X, MP_PLAY_LIST_HELP_Y,
    MP_PLAY_LIST_HELP_W, MP_PLAY_LIST_HELP_H, p_play_list_cont, 0);
  ctrl_set_rstyle(p_help, RSI_MP_TITLE, RSI_MP_TITLE, RSI_MP_TITLE);
  mbox_enable_icon_mode(p_help, TRUE);
  mbox_enable_string_mode(p_help, TRUE);
  mbox_set_count(p_help, MP_PLAY_LIST_HELP_MBOX_TOL,
    MP_PLAY_LIST_HELP_MBOX_COL, MP_PLAY_LIST_HELP_MBOX_ROW);
  ctrl_set_mrect(p_help, 0, 0, MP_PLAY_LIST_HELP_W, MP_PLAY_LIST_HELP_H);
  mbox_set_item_interval(p_help, MP_PLAY_LIST_HELP_MBOX_HGAP, MP_PLAY_LIST_HELP_MBOX_VGAP);
  mbox_set_item_rstyle(p_help, RSI_MP_TITLE,
    RSI_MP_TITLE, RSI_MP_TITLE);
  mbox_set_string_fstyle(p_help, FSI_MP_INFO_MBOX,
    FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX);
  mbox_set_string_offset(p_help, 40, 0);
  mbox_set_string_align_type(p_help, STL_LEFT | STL_VCENTER);
  mbox_set_icon_align_type(p_help, STL_LEFT | STL_VCENTER);
  mbox_set_content_strtype(p_help, MBOX_STRTYPE_STRID);
  pic_fav_list_update_help(p_help, FALSE);
  ctrl_set_sts(p_help, OBJ_STS_HIDE);*/
  //sort list
  p_sort_list = ctrl_create_ctrl(CTRL_LIST, IDC_PIC_SORT_LIST,
                            PIC_SORT_LIST_X, PIC_SORT_LIST_Y,
                            PIC_SORT_LIST_W, PIC_SORT_LIST_H,
                            p_menu,
                            0);
  ctrl_set_rstyle(p_sort_list, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG);
  ctrl_set_keymap(p_sort_list, pic_sort_list_keymap);
  ctrl_set_proc(p_sort_list, pic_sort_list_proc);
  ctrl_set_mrect(p_sort_list, MP_SORT_LIST_MIDL, MP_SORT_LIST_MIDT,
                    MP_SORT_LIST_MIDW+MP_SORT_LIST_MIDL, MP_SORT_LIST_MIDH+MP_SORT_LIST_MIDT);
  list_set_item_interval(p_sort_list, MP_SORT_LIST_VGAP);
  list_set_item_rstyle(p_sort_list, &pic_list_item_rstyle);
  list_set_count(p_sort_list, PIC_SORT_LIST_ITEM_TOL, MP_SORT_LIST_ITEM_PAGE);
  list_set_field_count(p_sort_list, MP_SORT_LIST_FIELD_NUM, MP_SORT_LIST_ITEM_PAGE);
  list_set_focus_pos(p_sort_list, 0);
  list_set_update(p_sort_list, pic_sort_list_update, 0);

  for (i = 0; i < MP_SORT_LIST_FIELD_NUM; i++)
  {
    list_set_field_attr(p_sort_list, (u8)i,
                        (u32)(mp_sort_list_field_attr[i].attr),
                        (u16)(mp_sort_list_field_attr[i].width),
                        (u16)(mp_sort_list_field_attr[i].left),
                        (u8)(mp_sort_list_field_attr[i].top));
    list_set_field_rect_style(p_sort_list, (u8)i, mp_sort_list_field_attr[i].rstyle);
    list_set_field_font_style(p_sort_list, (u8)i, mp_sort_list_field_attr[i].fstyle);
  }
  pic_sort_list_update(p_sort_list, list_get_valid_pos(p_sort_list), MP_SORT_LIST_ITEM_PAGE, 0);
  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);


  //help
  //bottom help bar
  if(is_usb)
  {
  p_bottom_help = ctrl_create_ctrl(CTRL_TEXT, IDC_PIC_BOTTOM_HELP,
                   PIC_BOTTOM_HELP_X, PIC_BOTTOM_HELP_Y, PIC_BOTTOM_HELP_W,
                   PIC_BOTTOM_HELP_H, p_menu, 0);
  ctrl_set_rstyle(p_bottom_help, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
  text_set_font_style(p_bottom_help, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX);
  text_set_align_type(p_bottom_help, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_bottom_help, TEXT_STRTYPE_UNICODE);
  ui_comm_help_create_ext(40, 0, PIC_BOTTOM_HELP_W-40, PIC_BOTTOM_HELP_H,  &help,  p_bottom_help);
  }
  else
  {
   p_bottom_help = ctrl_create_ctrl(CTRL_TEXT, IDC_PIC_BOTTOM_HELP,
                    PIC_BOTTOM_HELP_X, PIC_BOTTOM_HELP_Y, PIC_BOTTOM_HELP_W,
                    PIC_BOTTOM_HELP_H, p_menu, 0);
   ctrl_set_rstyle(p_bottom_help, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
   text_set_font_style(p_bottom_help, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX, FSI_MP_INFO_MBOX);
   text_set_align_type(p_bottom_help, STL_LEFT | STL_VCENTER);
   text_set_content_type(p_bottom_help, TEXT_STRTYPE_UNICODE);
   ui_comm_help_create_ext(40, 0, PIC_BOTTOM_HELP_W-40, PIC_BOTTOM_HELP_H,  &help_network,  p_bottom_help);
  }
   ctrl_set_sts(p_bottom_help, OBJ_STS_HIDE);
  //pic_update_detail(p_list, FALSE, (g_list.p_file[0].type != NOT_DIR_FILE));
  
  ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
  ctrl_paint_ctrl(ctrl_get_root(p_menu), FALSE);
  gui_start_roll(p_list, &roll_param);
  play_letter = 0;
  ui_time_enable_heart_beat(TRUE);
  g_picture_player_preview_focus = 0;
  if(ui_is_mute())
    open_mute(0, 0);
  ui_init_play_timer(ROOT_ID_USB_PICTURE, MSG_MEDIA_PLAY_TMROUT, 300);
    manage_open_menu(ROOT_ID_FUN_HELP, 0, 0);

  /*
{
  mul_pic_param_t pic_param = {{0}};
  
  ui_pic_init(PIC_SOURCE_NET);
  ui_pic_set_url("http://r4.ykimg.com/05100000500613156714C004DD0EF2D5");

  pic_param.anim = REND_ANIM_NONE;
  pic_param.style = REND_STYLE_CENTER;
  pic_param.flip = PIC_NO_F_NO_R;
  memcpy(&pic_param.win_rect, &g_pic_rect, sizeof(rect_t));

  pic_param.file_size = 0;
  pic_param.handle = 0;

  pic_start(&pic_param);  
  }
  */
  ui_evt_enable_ir();  
  return SUCCESS;
}

static RET_CODE on_pic_list_end_music(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
	media_fav_t *p_media = NULL;
    OS_PRINTF("@@@%s\n", __FUNCTION__);
	if(ui_music_get_play_status() == MUSIC_STAT_PLAY)
	{
		if(ui_music_pre_play_next(&p_media))
		{
		  //play next music, calculate music total time.

		  music_player_next(p_media->path);
		  ui_music_reset_curn(p_media->path);
         music_reset_time_info();
		}
	}

	return SUCCESS;
}

static RET_CODE on_pic_too_large(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_msg = NULL;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  OS_PRINTF("%s(Line: %d):  The picture too large.\n", __FUNCTION__, __LINE__);
  if(NULL != fw_find_root_by_id(ROOT_ID_PICTURE))
  {
    return SUCCESS;
  }
  ui_pic_clear(0);
  pic_stop();
  b_is_picture_error = TRUE;
  p_msg = ctrl_get_child_by_id(p_cont, IDC_PIC_MSG);

  ctrl_set_sts(p_msg, OBJ_STS_SHOW);

  text_set_content_by_strid(p_msg, IDS_MSG_OUT_OF_RANGE);

  ctrl_paint_ctrl(p_msg, TRUE);

  return SUCCESS;
}

static RET_CODE on_pic_play_curn(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
  u16 pos;
  control_t *p_msg = NULL;
  control_t* p_list = NULL;
  control_t *p_list_cont = NULL;
  media_fav_t *p_media = NULL;
  BOOL ret = FALSE;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  p_list_cont = ctrl_get_child_by_id(p_cont, IDC_PIC_LIST_CONT);
  p_list = ctrl_get_child_by_id(p_list_cont, IDC_PIC_LIST);
  p_msg = ctrl_get_child_by_id(p_cont, IDC_PIC_MSG);
  pos = list_get_focus_pos(p_list);

  if (g_picture_player_preview_focus != pos)
  {
    if(g_list.p_file[pos].type == NOT_DIR_FILE)
    {
      if(OBJ_STS_SHOW == ctrl_get_sts(p_msg))
      {
        ctrl_set_sts(p_msg, OBJ_STS_HIDE);
        ctrl_erase_ctrl(p_msg);
      }

      pic_stop();
      ret = ui_pic_set_play_index_dir_by_name(g_list.p_file[pos].name);
      if(ret == FALSE)
      {
         OS_PRINTF("error line: %d ,function: %s \n",__LINE__, __FUNCTION__);
         return ERR_FAILURE;
      }
      //pic_update_detail(p_list, TRUE, FALSE);
      ui_pic_set_play_mode(PIC_MODE_NORMAL);
      ui_pic_play_curn(&p_media, &g_pic_rect);
		play_letter = p_media->path[0];
    }
    g_picture_player_preview_focus = pos;
  }
  ui_play_timer_set_state(0);

  return SUCCESS;
}

static RET_CODE on_pic_fullscreen(control_t *p_cont, u16 msg,
  u32 para1, u32 para2)
{
OS_PRINTF("@@@%s\n", __FUNCTION__);
  if(fw_find_root_by_id(ROOT_ID_PICTURE) != NULL)
  {
     return ERR_FAILURE;
  }
  manage_close_menu(ROOT_ID_FUN_HELP, 0, 0);
   manage_open_menu(ROOT_ID_PICTURE, (u32) &para1, 0);

   return SUCCESS;
}

static RET_CODE _ui_picture_change(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
OS_PRINTF("@@@%s\n", __FUNCTION__);
ui_evt_disable_ir();
  
   ui_pic_clear(0);//clear picture buffer
   pic_stop();
   ui_pic_release();
   ui_time_enable_heart_beat(FALSE);

   picture_now_change = TRUE;
   
    manage_close_menu(ROOT_ID_FUN_HELP, 0, 0);
   if(is_usb)
   {
        switch(ctrl_get_ctrl_id(p_list))
        {
            case IDC_PIC_GROUP_TITLE1_CONT:
                manage_open_menu(ROOT_ID_USB_FILEPLAY, 0, 0);
            break;
            case IDC_PIC_GROUP_TITLE2_CONT:
                manage_open_menu(ROOT_ID_USB_MUSIC, 0, 0);
            break;
            case IDC_PIC_GROUP_TITLE4_CONT:
               // manage_open_menu(ROOT_ID_RECORD_MANAGER, 0, 0);
            break;
            default:
            break;
        }
        ui_set_usb_focus(3);
   }
   else
   {
        manage_open_menu(ROOT_ID_USB_FILEPLAY, 1, (u32)( ipaddress));
   }
      if(p_list != NULL)
  {
   // OS_PRINTF("============on_pic_exit gui_stop_roll p_list id = %d============\n",p_list->id);
    gui_stop_roll(p_list);
  }
    manage_close_menu(ROOT_ID_USB_PICTURE, 0, 0);
      return SUCCESS;
}

static RET_CODE ui_pic_up_folder(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
  BOOL is_enter_dir = FALSE;
  u16 *p_name_filter = NULL;
OS_PRINTF("@@@%s\n", __FUNCTION__);
  if (g_list.p_file != NULL)
  {
    p_name_filter = mtos_malloc(sizeof(u16) *(strlen(p_g_pic_filter) + 1));
    MT_ASSERT(p_name_filter != NULL);
    memset(p_name_filter, 0 , sizeof(u16) *(strlen(p_g_pic_filter) + 1));
    str_asc2uni(p_g_pic_filter, p_name_filter);
    is_enter_dir = pic_file_list_is_enter_dir(p_name_filter, g_list.p_file[0].name);
    mtos_free(p_name_filter);
  }
  else
  {
    is_enter_dir = FALSE;
  }
  if(is_enter_dir)
  {
    pic_file_list_update(p_list);//simon fix bug #27346
  }
  return SUCCESS;
}

BEGIN_KEYMAP(_ui_pic_v_txt_keymap, NULL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(_ui_pic_v_txt_keymap, NULL)
BEGIN_MSGPROC(_ui_pic_v_txt_proc, text_class_proc)
    ON_COMMAND(MSG_FOCUS_LEFT, on_pic_list_change_focus)
    ON_COMMAND(MSG_FOCUS_RIGHT, on_pic_list_change_focus)
    ON_COMMAND(MSG_FOCUS_UP, on_pic_list_change_focus)
    ON_COMMAND(MSG_FOCUS_DOWN, on_pic_list_change_focus)
    ON_COMMAND(MSG_SELECT, _ui_picture_change)
END_MSGPROC(_ui_pic_v_txt_proc, text_class_proc)

BEGIN_KEYMAP(pic_cont_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
    ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(pic_cont_keymap, NULL)

BEGIN_MSGPROC(pic_cont_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_DESTROY, on_pic_destory)
    ON_COMMAND(MSG_PLUG_OUT, on_pic_plug_out)
    ON_COMMAND(MSG_UPDATE, on_pic_update)
    ON_COMMAND(MSG_PIC_EVT_UPDATE_SIZE, on_pic_evt_update_size)
    ON_COMMAND(MSG_EXIT, on_pic_exit)
    ON_COMMAND(MSG_EXIT_ALL, on_pic_exit)
    ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, on_pic_too_large)
    ON_COMMAND(MSG_MEDIA_PLAY_TMROUT, on_pic_play_curn)
    ON_COMMAND(MSG_FULLSCREEN, on_pic_fullscreen)
    ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, on_pic_evt_unsupport)
    ON_COMMAND(MSG_PIC_EVT_DATA_ERROR,on_pic_evt_data_error)
    ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, on_pic_evt_too_large)
END_MSGPROC(pic_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(pic_list_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
    //ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
    //ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
    // ON_EVENT(V_KEY_LEFT, MSG_VOLUME_DOWN)
    //ON_EVENT(V_KEY_RIGHT, MSG_VOLUME_UP)
    ON_EVENT(V_KEY_VUP, MSG_VOLUME_UP)
    ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DOWN)
    //ON_EVENT(V_KEY_FAV, MSG_ADD_FAV)
    //ON_EVENT(V_KEY_GREEN, MSG_GREEN)
    //ON_EVENT(V_KEY_BLUE, MSG_BLUE)
    // ON_EVENT(V_KEY_YELLOW, MSG_YELLOW)
    ON_EVENT(V_KEY_HOT_XEXTEND, MSG_SORT)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_PLAY, MSG_MUSIC_PLAY)
    ON_EVENT(V_KEY_PAUSE, MSG_MUSIC_PAUSE)
    ON_EVENT(V_KEY_FORW, MSG_MUSIC_NEXT)
    //ON_EVENT(V_KEY_BACK, MSG_MUSIC_PRE)
    ON_EVENT(V_KEY_STOP, MSG_MUSIC_STOP)
    // ON_EVENT(V_KEY_INFO, MSG_INFO)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(pic_list_keymap, NULL)

BEGIN_MSGPROC(pic_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_pic_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_pic_list_change_focus)
  ON_COMMAND(MSG_FOCUS_LEFT, on_pic_list_change_focus)
    ON_COMMAND(MSG_FOCUS_RIGHT, on_pic_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_pic_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_pic_list_change_focus)
  //ON_COMMAND(MSG_DECREASE, on_pic_list_change_partition)
  //ON_COMMAND(MSG_INCREASE, on_pic_list_change_partition)
  //ON_COMMAND(MSG_GREEN, on_pic_list_change_partition)
  ON_COMMAND(MSG_VOLUME_DOWN, on_pic_list_volume)
  ON_COMMAND(MSG_VOLUME_UP, on_pic_list_volume)
  //ON_COMMAND(MSG_ADD_FAV, on_pic_add_to_fav_list)
  //ON_COMMAND(MSG_YELLOW, on_pic_open_fav_list)
  //ON_COMMAND(MSG_BLUE, on_pic_list_set_play_mode)
  ON_COMMAND(MSG_SORT, on_pic_list_sort)
  ON_COMMAND(MSG_SELECT, on_pic_list_select)
  ON_COMMAND(MSG_MUSIC_NEXT, on_pic_list_next_music)
  ON_COMMAND(MSG_MUSIC_PRE, on_pic_list_prev_music)
  ON_COMMAND(MSG_MUSIC_EVT_PLAY_END, on_pic_list_end_music)
  ON_COMMAND(MSG_INFO, _ui_picture_change)
END_MSGPROC(pic_list_proc, list_class_proc)

BEGIN_KEYMAP(pic_network_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_INFO, MSG_INFO)
  ON_EVENT(V_KEY_RECALL, MSG_EXIT)
  //ON_EVENT(V_KEY_RECALL, MSG_PIC_UPFOLDER)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  ON_EVENT(V_KEY_LEFT, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_RIGHT, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DOWN)
  ON_EVENT(V_KEY_VUP, MSG_VOLUME_UP)
  ON_EVENT(V_KEY_GREEN, MSG_PIC_BACK_TO_NETWORK_PLACE)
END_KEYMAP(pic_network_list_keymap, NULL)

BEGIN_MSGPROC(pic_network_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_pic_list_select)
  ON_COMMAND(MSG_INFO, _ui_picture_change)
  ON_COMMAND(MSG_PIC_UPFOLDER, ui_pic_up_folder)
  ON_COMMAND(MSG_FOCUS_UP, on_pic_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_pic_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_pic_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_pic_list_change_focus)
  ON_COMMAND(MSG_VOLUME_DOWN, on_pic_list_volume)
  ON_COMMAND(MSG_VOLUME_UP, on_pic_list_volume)
  ON_COMMAND(MSG_EXIT, on_pic_process_msg)
  ON_COMMAND(MSG_PIC_BACK_TO_NETWORK_PLACE, on_pic_process_msg)
END_MSGPROC(pic_network_list_proc, list_class_proc)

BEGIN_KEYMAP(pic_fav_list_list_keymap, NULL)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
  ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
  //ON_EVENT(V_KEY_LEFT, MSG_DECREASE)
  //ON_EVENT(V_KEY_RIGHT, MSG_INCREASE)
  ON_EVENT(V_KEY_FAV, MSG_ADD_FAV)
  ON_EVENT(V_KEY_GREEN, MSG_GREEN)
  ON_EVENT(V_KEY_YELLOW, MSG_YELLOW)
  ON_EVENT(V_KEY_HOT_XEXTEND, MSG_RED)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(pic_fav_list_list_keymap, NULL)

BEGIN_MSGPROC(pic_fav_list_list_proc, list_class_proc)
  ON_COMMAND(MSG_FOCUS_UP, on_pic_fav_list_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_pic_fav_list_change_focus)
  ON_COMMAND(MSG_PAGE_UP, on_pic_fav_list_change_focus)
  ON_COMMAND(MSG_PAGE_DOWN, on_pic_fav_list_change_focus)
  ON_COMMAND(MSG_GREEN, on_pic_fav_list_delte_all)
  ON_COMMAND(MSG_YELLOW, on_pic_fav_list_delet)
  ON_COMMAND(MSG_RED, on_pic_fav_list_select)
  ON_COMMAND(MSG_SELECT, on_pic_fav_list_select)
  ON_COMMAND(MSG_EXIT, on_pic_fav_list_exit)
  ON_COMMAND(MSG_MUSIC_EVT_PLAY_END, on_pic_list_end_music)
END_MSGPROC(pic_fav_list_list_proc, list_class_proc)

BEGIN_KEYMAP(pic_sort_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_HOT_XEXTEND, MSG_EXIT)
END_KEYMAP(pic_sort_list_keymap, NULL)

BEGIN_MSGPROC(pic_sort_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_pic_sort_list_select)
  ON_COMMAND(MSG_EXIT, on_pic_sort_list_exit)
  //ON_COMMAND(MSG_DESTROY, on_sort_list_destory)
END_MSGPROC(pic_sort_list_proc, list_class_proc)

