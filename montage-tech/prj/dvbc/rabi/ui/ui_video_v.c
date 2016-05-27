/******************************************************************************/
/******************************************************************************/

/*!
 Include files
 */
#include "ui_common.h"
#include "ui_rename.h"
#include "lib_char.h"
#if ENABLE_FILE_PLAY
#include "file_play_api.h"
#include "ui_video.h"
#include "ui_volume_usb.h"
#include "lib_unicode.h"
#include "ui_edit_usr_pwd.h"
#include "ui_fp_audio_set.h"
#include "pnp_service.h"
#include "ui_play_api.h"



/*!
 Const value
 */

/*!
 *Play line num
 */
#define VIDEO_V_PLAY_ICON_LINE  1
/*!
 *Play time string length
 */
#define VIDEO_V_PLAY_TIME_LEN   32

/*!
 * List scrollbar
 */
#define VIDEO_V_LIST_SBAR_X  375
#define VIDEO_V_LIST_SBAR_Y  50
#define VIDEO_V_LIST_SBARW  6
#define VIDEO_V_LIST_SBARH  660

#define VIDEO_V_LIST_SBAR_MIDL  0
#define VIDEO_V_LIST_SBAR_MIDT  12
#define VIDEO_V_LIST_SBAR_MIDR  6
#define VIDEO_V_LIST_SBAR_MIDB  (VIDEO_V_LIST_SBARH - VIDEO_V_LIST_SBAR_MIDT)

#define VIDEO_V_LEFT_BG_X 0
#define VIDEO_V_LEFT_BG_Y 0
#define VIDEO_V_LEFT_BG_W 460
#define VIDEO_V_LEFT_BG_H  720

#define VIDEO_V_LEFT_HD_X 0
#define VIDEO_V_LEFT_HD_Y 10
#define VIDEO_V_LEFT_HD_W 150
#define VIDEO_V_LEFT_HD_H  700

//title1 text
#define VIDEO_V_TITLE_TEXT1_X 0
#define VIDEO_V_TITLE_TEXT1_Y 10
#define VIDEO_V_TITLE_TEXT1_W 150
#define VIDEO_V_TITLE_TEXT1_H 105

//partition group container
#define VIDEO_V_GROUP_CONTX 150
#define VIDEO_V_GROUP_CONTY 0
#define VIDEO_V_GROUP_CONTW 400
#define VIDEO_V_GROUP_CONTH 50

//partition group
#define VIDEO_V_GROUP_X  0
#define VIDEO_V_GROUP_Y  0
#define VIDEO_V_GROUP_W 300
#define VIDEO_V_GROUP_H  50

//partition group arrow left
#define VIDEO_V_GROUP_ARROWL_X       44
#define VIDEO_V_GROUP_ARROWL_Y       7
#define VIDEO_V_GROUP_ARROWL_W      36
#define VIDEO_V_GROUP_ARROWL_H       36

//partition group arrow right
#define VIDEO_V_GROUP_ARROWR_X       430
#define VIDEO_V_GROUP_ARROWR_Y       7
#define VIDEO_V_GROUP_ARROWR_W      36
#define VIDEO_V_GROUP_ARROWR_H       36
//bottom infor
/*#define VIDEO_V_PATH_X  53
#define VIDEO_V_PATH_Y  60
#define VIDEO_V_PATH_W  344
#define VIDEO_V_PATH_H  32*/

//bottom infor
#define VIDEO_V_PATH_X  500
#define VIDEO_V_PATH_Y  0
#define VIDEO_V_PATH_W  300
#define VIDEO_V_PATH_H  50

//bottom help container
#define VIDEO_V_BOTTOM_HELP_X  1125
#define VIDEO_V_BOTTOM_HELP_Y  455
#define VIDEO_V_BOTTOM_HELP_W  155
#define VIDEO_V_BOTTOM_HELP_H  60

#define VIDEO_V_BOTTOM_HELP_BMP_X  5
#define VIDEO_V_BOTTOM_HELP_BMP_Y  5
#define VIDEO_V_BOTTOM_HELP_BMP_W  50
#define VIDEO_V_BOTTOM_HELP_BMP_H  50

#define VIDEO_V_BOTTOM_HELP_TXT_X  55
#define VIDEO_V_BOTTOM_HELP_TXT_Y  5
#define VIDEO_V_BOTTOM_HELP_TXT_W  100
#define VIDEO_V_BOTTOM_HELP_TXT_H  50

//plist list container
#define VIDEO_V_LIST_CONTX   150
#define VIDEO_V_LIST_CONTY   10
#define VIDEO_V_LIST_CONTW  300
#define VIDEO_V_LIST_CONTH   700
/*!
 * List
 */
#define VIDEO_V_LIST_X  5
#define VIDEO_V_LIST_Y  5
#define VIDEO_V_LISTW  290
#define VIDEO_V_LISTH  660

#define VIDEO_V_LIST_MIDL  4
#define VIDEO_V_LIST_MIDT  4
#define VIDEO_V_LIST_MIDW  (VIDEO_V_LISTW - 2 * VIDEO_V_LIST_MIDL)
#define VIDEO_V_LIST_MIDH  (VIDEO_V_LISTH - 2 * VIDEO_V_LIST_MIDT)

#define VIDEO_V_LIST_ICON_VGAP 4
#define VIDEO_V_LIST_ITEM_NUM_ONE_PAGE  16


#define VIDEO_V_SORT_LIST_X  465 
#define VIDEO_V_SORT_LIST_Y  50
#define VIDEO_V_SORT_LIST_W  250
#define VIDEO_V_SORT_LIST_H  90

#define VIDEO_V_SORT_LIST_MIDL       10
#define VIDEO_V_SORT_LIST_MIDT       10
#define VIDEO_V_SORT_LIST_MIDW       (VIDEO_V_SORT_LIST_W - 2*VIDEO_V_SORT_LIST_MIDL)
#define VIDEO_V_SORT_LIST_MIDH       (VIDEO_V_SORT_LIST_H - 2*VIDEO_V_SORT_LIST_MIDT)

#define VIDEO_V_SORT_LIST_ITEM_TOL     2
#define VIDEO_V_SORT_LIST_ITEM_PAGE    VIDEO_V_SORT_LIST_ITEM_TOL
#define VIDEO_V_SORT_LIST_FIELD_NUM    1
/*!
 * Play detail
 */
#define VIDEO_V_PLAY_CONT_X  672
#define VIDEO_V_PLAY_CONT_Y  410
#define VIDEO_V_PLAY_CONT_W  488
#define VIDEO_V_PLAY_CONT_H  150

/*!
 * Play detail file name
 */
#define VIDEO_V_DETAIL_FILE_NAME_X  10
#define VIDEO_V_DETAIL_FILE_NAME_Y  10
#define VIDEO_V_DETAIL_FILE_NAME_W  450
#define VIDEO_V_DETAIL_FILE_NAME_H  30



/*!
 * Play detail file size
 */
#define VIDEO_V_DETAIL_FILE_SIZE_X  230
#define VIDEO_V_DETAIL_FILE_SIZE_Y  40
#define VIDEO_V_DETAIL_FILE_SIZE_W  200
#define VIDEO_V_DETAIL_FILE_SIZE_H  30

/*!
 * Play detail file resolution
 */
#define VIDEO_V_DETAIL_FILE_PIXEL_X  10
#define VIDEO_V_DETAIL_FILE_PIXEL_Y  40
#define VIDEO_V_DETAIL_FILE_PIXEL_W  200
#define VIDEO_V_DETAIL_FILE_PIXEL_H  30
/*!
 * Play detail progress
 */
#define VIDEO_V_PLAY_PROGRESS_X  270
#define VIDEO_V_PLAY_PROGRESS_Y  46
#define VIDEO_V_PLAY_PROGRESS_W  200
#define VIDEO_V_PLAY_PROGRESS_H  14
#define VIDEO_V_PLAY_PROGRESS_MIDX 0
#define VIDEO_V_PLAY_PROGRESS_MIDY 0
#define VIDEO_V_PLAY_PROGRESS_MIDW VIDEO_V_PLAY_PROGRESS_W
#define VIDEO_V_PLAY_PROGRESS_MIDH VIDEO_V_PLAY_PROGRESS_H
#define VIDEO_V_PLAY_PROGRESS_MIN RSI_PLAY_PBAR_FRONT
#define VIDEO_V_PLAY_PROGRESS_MAX RSI_PLAY_PBAR_BACK
#define VIDEO_V_PLAY_PROGRESS_MID INVALID_RSTYLE_IDX

/*!
 * Play detail current time
 */
#define VIDEO_V_PLAY_CURTM_X  230
#define VIDEO_V_PLAY_CURTM_Y  70
#define VIDEO_V_PLAY_CURTM_W  100
#define VIDEO_V_PLAY_CURTM_H  30

/*!
 * Play detail total time
 */
#define VIDEO_V_PLAY_TOLTM_X  330
#define VIDEO_V_PLAY_TOLTM_Y  70
#define VIDEO_V_PLAY_TOLTM_W  150
#define VIDEO_V_PLAY_TOLTM_H  30


/*!
 * Play detail play state icon
 */
#define VIDEO_V_PLAY_ICON_X  0
#define VIDEO_V_PLAY_ICON_Y  70
#define VIDEO_V_PLAY_ICON_W  70
#define VIDEO_V_PLAY_ICON_H  28


/*!
 * Play unsupport flag
 */
#define VIDEO_V_PLAY_UNSUPPORT_W  200
#define VIDEO_V_PLAY_UNSUPPORT_H  (50)
#define VIDEO_V_PLAY_UNSUPPORT_X  (VIDEO_V_PREV_X + (VIDEO_V_PREV_W - VIDEO_V_PLAY_UNSUPPORT_W) / 2)
#define VIDEO_V_PLAY_UNSUPPORT_Y  (VIDEO_V_PREV_Y + (VIDEO_V_PREV_H - VIDEO_V_PLAY_UNSUPPORT_H) / 2)


/*!
 * Video color style
 */
#define VIDEO_V_FSI_COLOR FSI_WHITE
#define VIDEO_V_RSI_COLOR RSI_PBACK

/*!
 * Video list style
 */
static list_xstyle_t video_v_list_field_rstyle =
{
  RSI_MEDIO_LEFT_LIST_BG,
  RSI_MEDIO_LEFT_LIST_BG,
  RSI_MEDIO_LEFT_LIST_HL,
  RSI_MEDIO_LEFT_LIST_SH,
  RSI_MEDIO_LEFT_LIST_HL,
};
/*static list_xstyle_t video_v_list_field_rstyle_bg =
{
  RSI_MEDIO_LEFT_LIST_BG,
  RSI_MEDIO_LEFT_LIST_BG,
  RSI_MEDIO_LEFT_LIST_BG,
  RSI_MEDIO_LEFT_LIST_BG,
  RSI_MEDIO_LEFT_LIST_BG,
};*/
static list_xstyle_t video_v_list_field_rstyle1 =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};
static list_xstyle_t video_v_list_field_fstyle =
{
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};
static const list_field_attr_t video_list_attr[] =
{
    {
        LISTFIELD_TYPE_UNISTR,
        40,
        0,
        0,
        &video_v_list_field_rstyle1,
        &video_v_list_field_fstyle
    },
    {
        LISTFIELD_TYPE_ICON,
        30,
        40,
        0,
        &video_v_list_field_rstyle1,
        &video_v_list_field_fstyle
    },

    {
        LISTFIELD_TYPE_UNISTR | STL_LEFT | LISTFIELD_SCROLL,
        212,
        70,
        0,
        &video_v_list_field_rstyle1,
        &video_v_list_field_fstyle
    }
};
static list_field_attr_t v_sort_list_field_attr[VIDEO_V_SORT_LIST_FIELD_NUM] =
{
  { LISTFIELD_TYPE_STRID | STL_LEFT | STL_VCENTER,
    VIDEO_V_SORT_LIST_MIDW - 10, 10, 0, &video_v_list_field_rstyle1, &video_v_list_field_fstyle },
};
/*static list_xstyle_t video_v_list_item_rstyle =
{
  RSI_PBACK,
  RSI_ITEM_2_SH,
  RSI_ITEM_2_HL,
  RSI_ITEM_6,
  RSI_ITEM_2_HL,
};*/


/*static comm_help_data_t movie_help_data =
{
    5,5,
    {IDS_SELECT, IDS_BACK,IDS_FULL, IDS_SWITCH_TYPE, IDS_PARTITION},
    {IM_HELP_OK, IM_HELP_RECALL,IM_HELP_YELLOW, IM_HELP_INFOR, IM_HELP_GREEN}
};*/

/*static comm_help_data_t movie_help_network_data =
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
  };*/


/*!
 * Type define
 */

/*!
 * Video play state icon
 */
typedef enum
{
    VIDEO_PLAY_ICON_PLAY = 0,
    VIDEO_PLAY_ICON_PAUSE,
    VIDEO_PLAY_ICON_STOP,
    VIDEO_PLAY_ICON_MAX,
}ui_video_v_play_icon_t;

/*!
 * Video view message
 */
typedef enum
{
    MSG_VIDEO_V_EVT_PLAY_END = MSG_LOCAL_BEGIN + 0x100,
    MSG_VIDEO_V_STOP,
    MSG_VIDEO_V_PAUSE_PLAY,
    MSG_VIDEO_V_FULLSCREEN,
    MSG_VOLUME_UP,
    MSG_VOLUME_DOWN,
    MSG_VIDEO_V_UPFOLDER,
    MSG_VIDEO_V_CHANGE_PARTITION,
    MSG_VIDEO_V_INFO,
    MSG_BACK_TO_NETWORK_PLACE,
}ui_video_v_msg_t;

/*!
 * Video view control id
 */
typedef enum
{
    IDC_VIDEO_V_PREVIEW = 1,
    IDC_VIDEO_V_SBAR,
    IDC_VIDEO_V_LIST,
    IDC_VIDEO_V_PATH,
    IDC_VIDEO_V_PLAY_FILENAME,
    IDC_VIDEO_V_FILE_SIZE,
    IDC_VIDEO_V_FILE_PIXEL,
    IDC_VIDEO_V_PLAY_DETAIL_CONT,
    IDC_VIDEO_V_PLAY_CURTM,
    IDC_VIDEO_V_PLAY_TOLTM,
    IDC_VIDEO_V_PLAY_ICON,
    IDC_VIDEO_V_UNSUPPORT,
    IDC_VIDEO_V_BTM_HELP,
    IDC_VIDEO_V_LIST_CONT,
    IDC_VIDEO_V_GROUP_CONT,
    IDC_VIDEO_V_GROUP,
    IDC_VIDEO_V_GROUP_ARROWL,
    IDC_VIDEO_V_GROUP_ARROWR,
    IDC_VIDEO_V_GROUP_TITLE1_CONT,
    IDC_VIDEO_V_GROUP_TITLE2_CONT,
    IDC_VIDEO_V_GROUP_TITLE3_CONT,
    IDC_VIDEO_V_GROUP_TITLE4_CONT,
    IDC_VIDEO_V_TITLE_CONT,
    IDC_VIDEO_V_GROUP_BMP1_CONT,
    IDC_VIDEO_V_GROUP_BMP2_CONT,
    IDC_VIDEO_V_GROUP_BMP3_CONT,
    IDC_VIDEO_V_GROUP_BMP4_CONT,
    IDC_VIDEO_V_LEFT_BG,
    IDC_VIDEO_V_LEFT_HD,
    IDC_VIDEO_V_SORT_LIST,
}ui_video_v_ctrl_id_t;

/*!
 Macro
 */


static BOOL preview_state = FALSE;
static u32 is_usb = 1;
static char ipaddress[128] = "";
static u16 ip_address_with_path[32] ={0};
static u16  play_focus = 0;
static u16 now_change = 0;
static BOOL is_list = TRUE;
static BOOL is_exit_all = FALSE;
static BOOL is_full_stop = FALSE;
/*!
 * Function define
 */
extern RET_CODE switch_to_play_prog(void);
u16 _ui_video_v_list_keymap(u16 key);
RET_CODE _ui_video_v_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);


u16 _ui_video_v_txt_keymap(u16 key);
RET_CODE _ui_video_v_txt_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 _ui_video_network_v_list_keymap(u16 key);
RET_CODE _ui_video_network_v_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

u16 _ui_video_v_cont_keymap(u16 key);
RET_CODE _ui_video_v_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static void ui_video_v_close_menu();

static void ui_video_v_pre_close_menu();

extern RET_CODE pnp_svc_unmount(u16 *p_url);

u16 video_v_sort_list_keymap(u16 key);
RET_CODE video_v_sort_list_proc(control_t *p_list, u16 msg, u32 para1, u32 para2);

u32 get_video_is_usb()//if is_usb is 0 means video entry from samba 
{
  return is_usb;
}

void set_play_focus(u16 index)
{
  play_focus = index;
}
/*================================================================================================
                           video play view internel function
 ================================================================================================*/

static void _ui_video_v_stop_show_logo(void)
{
    void *p_video_dev = NULL;
    void *p_disp_dev  = NULL;
    void *p_dmx_dev   = NULL;

    p_disp_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_DISPLAY);
    if(NULL == p_disp_dev)
    {
        return;
    }

    disp_layer_show(p_disp_dev, DISP_LAYER_ID_VIDEO_SD, FALSE);

    if(preview_state)
    {
        preview_state = FALSE;
        p_video_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_VDEC_VSB);

        if(NULL != p_video_dev)
        {
            vdec_stop(p_video_dev);
        }

        p_dmx_dev = dev_find_identifier(NULL, DEV_IDT_TYPE, SYS_DEV_TYPE_PTI);

        if(NULL != p_dmx_dev)
        {
            dmx_av_reset(p_dmx_dev);
        }
    }
}

/*!
 * Show playing video resolution
 */
static RET_CODE  _ui_video_v_show_resolution(void)
{
    control_t *p_play      = NULL;
    control_t *p_pixel     = NULL;
    u8         asc_str[32] = {0};
    rect_size_t _rect      = {0};

    p_play = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_PLAY_DETAIL_CONT);
    p_pixel = ctrl_get_child_by_id(p_play, IDC_VIDEO_V_FILE_PIXEL);

    ui_video_m_get_saved_resolution(&_rect);

    sprintf(asc_str, "%ld x %ld", _rect.w, _rect.h);
    text_set_content_by_ascstr(p_pixel, asc_str);
    ctrl_paint_ctrl(p_pixel, TRUE);

    return SUCCESS;
}
/*!
 * Show playing video resolution
 */
static RET_CODE  _ui_video_v_update_resolution(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    rect_size_t *p_rect =  (rect_size_t *)para1;

    if(ROOT_ID_USB_FILEPLAY != fw_get_focus_id())
    {
      return SUCCESS;
    }

    if(NULL == p_rect)
    {
        return ERR_FAILURE;
    }

    ui_video_m_save_resolution(p_rect);
    _ui_video_v_show_resolution();

    return SUCCESS;
}
/*!
 * Update total time
 */
static RET_CODE _ui_video_v_update_cur_time(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    control_t         *p_time = NULL;
    control_t         *p_play   = NULL;
    video_play_time_t  time     = {0};
    char               time_str[VIDEO_V_PLAY_TIME_LEN] = {0};

    /*!
     * Change play icon
     */
    p_play = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_PLAY_DETAIL_CONT);
    p_time = ctrl_get_child_by_id(p_play, IDC_VIDEO_V_PLAY_CURTM);

    UI_VIDEO_PRINF("_ui_video_v_update_cur_time %d\n", para1);

    ui_video_c_switch_time(&time, para1 / 1000);

    sprintf(time_str, "%02d:%02d:%02d", time.hour, time.min, time.sec);

    text_set_content_by_ascstr(p_time, time_str);

    ctrl_paint_ctrl(p_time, TRUE);
    return SUCCESS;
}

/*!
 * Scroll list
 */
static void _ui_video_v_list_scroll(control_t *p_cont)
{
    roll_param_t p_param = {0};

    MT_ASSERT(p_cont != NULL);

    gui_stop_roll(p_cont);

    p_param.pace = ROLL_SINGLE;
    p_param.style = ROLL_LR;
    p_param.repeats = 0;
    p_param.is_force = FALSE;

    gui_start_roll(p_cont, &p_param);
}

static void _ui_video_v_init(void)
{
    /*!
      * Init rsc
      */
    ui_video_m_rsc_init();

}


/*!
 * Exit video view
 */
static RET_CODE _ui_video_v_exit_all(control_t *p_list, u16 msg, u32 para1, u32 para2)
{

    UI_VIDEO_PRINF("[%s]: exit all\n", __FUNCTION__);

    is_exit_all = TRUE;
    ui_time_enable_heart_beat(FALSE);
    ui_video_v_pre_close_menu();
	
    return SUCCESS;
}

static RET_CODE fileplay_partition_group_update(control_t *p_cbox, u16 focus, u16 *p_str,
                              u16 max_length)
{
    partition_t * p_partition = NULL;
    u32 g_partition_cnt = 0;
    g_partition_cnt = file_list_get_partition(&p_partition);
  //str_asc2uni(p_partition[focus].name, p_str);
  OS_PRINTF("====@@@ count = %d, focus = %d,--- -%s--@@@@\n",g_partition_cnt,focus,p_partition[focus].name);
//	dvb_to_unicode(p_partition[focus].name,max_length - 1, p_str, max_length);
    uni_strncpy(p_str, p_partition[focus].name, max_length);

    return SUCCESS;
}

static RET_CODE ui_change_plug_out_partition(control_t *p_list, u16 msg,
  u32 para1, u32 para2,RET_CODE (*p_proc)(control_t *p_list, u16 msg,
  u32 para1, u32 para2))
{
  u16 cur_usb_index = 0;
  u16 cur_ui_index = 0;
  u16 usb_plug_index = 0;

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
 
static RET_CODE _ui_video_v_plug_out(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u32 usb_dev_type = para2;
    control_t *p_cbox = NULL;
    RET_CODE ret = SUCCESS;

    if(usb_dev_type == HP_WIFI || !is_usb)
    {
      OS_PRINTF("####wifi device  plug out or samba plug out usb#####\n");
      return ERR_FAILURE;
    }

	p_cbox = ctrl_get_child_by_id(ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_GROUP_CONT),IDC_VIDEO_V_GROUP);

	ret = ui_change_plug_out_partition(p_list,MSG_VIDEO_V_CHANGE_PARTITION,*(u32 *)text_get_content(p_cbox),
	*(u32 *)ui_video_m_get_cur_playing_file_path(),
		_ui_video_v_list_proc);
	if(ret == SUCCESS)
	{
	  return SUCCESS;
	}

    ui_video_v_pre_close_menu();

    return SUCCESS;
}

static RET_CODE _ui_video_v_change_volume(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    open_volume_usb(ROOT_ID_USB_FILEPLAY, 1);
    return SUCCESS;
}

static void _ui_video_v_show_unsupport(void)
{
    control_t *p_unsp = NULL;

    p_unsp = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_UNSUPPORT);

    ctrl_set_sts(p_unsp, OBJ_STS_SHOW);
    ctrl_paint_ctrl(p_unsp, TRUE);
}

static void _ui_video_v_hide_unsupport(void)
{
    control_t *p_unsp = NULL;

    p_unsp = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_UNSUPPORT);

    ctrl_set_sts(p_unsp, OBJ_STS_HIDE);
    ctrl_erase_ctrl(p_unsp);
}

/*!
 * up unsupported txt
 */
static RET_CODE _ui_video_v_unsupported_video(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16 focus  = 0;
    
    UI_PRINTF("@@@_ui_video_v_unsupported_video()\n");
    if (ROOT_ID_USB_FILEPLAY != fw_get_focus_id())
    {
        return SUCCESS;
    }
    focus = ui_video_m_get_cur_playing_idx();
    ui_video_m_set_support_flag_by_idx(focus, FALSE);
      _ui_video_v_show_unsupport();

    
    return SUCCESS;
}
/*!
 * Get highlight play state icon index
 */
static u16 _ui_video_v_get_hl_play_icon_idx()
{
    mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;

    state = (mul_fp_play_state_t)ui_video_c_get_play_state();

    switch(state)
    {
        case MUL_PLAY_STATE_PLAY:
        case MUL_PLAY_STATE_LOAD_MEDIA:
        {
            return VIDEO_PLAY_ICON_PLAY;
        }
        case MUL_PLAY_STATE_PAUSE:
        case MUL_PLAY_STATE_PREVIEW:
        {
            return VIDEO_PLAY_ICON_PAUSE;
        }
        case MUL_PLAY_STATE_STOP:
        {
            return VIDEO_PLAY_ICON_STOP;
        }
        default:
        {
            return VIDEO_PLAY_ICON_MAX;
        }
    }
}
/*!
 * Update total time
 */
static void _ui_video_v_update_total_time(void)
{
    control_t         *p_t_time = NULL;
    control_t         *p_play   = NULL;
    video_play_time_t  time     = {0};
    char               time_str[VIDEO_V_PLAY_TIME_LEN] = {0};

    /*!
     * Change play icon
     */
    p_play = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_PLAY_DETAIL_CONT);
    p_t_time = ctrl_get_child_by_id(p_play, IDC_VIDEO_V_PLAY_TOLTM);

    ui_video_m_get_saved_total_time(&time);

    sprintf(time_str, "/ %02d:%02d:%02d", time.hour, time.min, time.sec);

    text_set_content_by_ascstr(p_t_time, time_str);

    ctrl_paint_ctrl(p_t_time, TRUE);
}
/*!
 * Update play icon
 */
static void _ui_video_v_update_play_icon(void)
{
    u16        j      = 0;
    control_t *p_icon = NULL;
    control_t *p_play = NULL;

    /*!
     * Change play icon
     */
    p_play = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_PLAY_DETAIL_CONT);
    p_icon = ctrl_get_child_by_id(p_play, IDC_VIDEO_V_PLAY_ICON);

    j = _ui_video_v_get_hl_play_icon_idx();

    if(j == VIDEO_PLAY_ICON_PLAY)
    {
        bmap_set_content_by_id(p_icon, IM_MP3_ICON_PLAY_2_SELECT);
    }
    else if(j == VIDEO_PLAY_ICON_PAUSE)
    {
        bmap_set_content_by_id(p_icon, IM_MP3_ICON_PAUSE_SELECT);
    }
    else
    {
        bmap_set_content_by_id(p_icon, IM_MP3_ICON_STOP_SELECT);
    }

    ctrl_paint_ctrl(p_icon, TRUE);
}
/*!
 * Show playing video name
 */
static void _ui_video_v_show_select_file_name(u16 focus)
{
    control_t *p_play                 = NULL;
    control_t *p_name                 = NULL;
    u16        uniname[MAX_FILE_PATH] = {0};

    /*!
     * Change file name
     */
    p_play = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_PLAY_DETAIL_CONT);
    p_name = ctrl_get_child_by_id(p_play, IDC_VIDEO_V_PLAY_FILENAME);

    ui_video_m_get_name_by_idx(focus, uniname, MAX_FILE_PATH);

    text_set_content_by_unistr(p_name, uniname);

    ctrl_paint_ctrl(p_name, TRUE);
}

/*!
 * Show playing video name
 */
static void _ui_video_v_show_select_file_size(u16 focus)
{
    control_t *p_play      = NULL;
    control_t *p_content   = NULL;
    u32        size        = 0;
    u8         asc_str[32] = {0};

    /*!
     * Change file name
     */
    p_play = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_PLAY_DETAIL_CONT);
    p_content = ctrl_get_child_by_id(p_play, IDC_VIDEO_V_FILE_SIZE);

    ui_video_m_get_size_by_idx(focus, &size);

    ui_conver_file_size_unit_bytes(size, asc_str);
    text_set_content_by_ascstr(p_content, asc_str);
    ctrl_paint_ctrl(p_content, TRUE);
}

static void _ui_video_v_reset_play_cur_time(control_t *p_list)
{
    control_t *p_time = NULL;
    control_t *p_play = NULL;

    p_play = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_PLAY_DETAIL_CONT);

    p_time = ctrl_get_child_by_id(p_play, IDC_VIDEO_V_PLAY_CURTM);

    text_set_content_by_ascstr(p_time, "00:00:00");

    ctrl_paint_ctrl(p_time, TRUE);
}
/*!
 * Update preview info
 */
static void _ui_video_v_update_preview_info(u16 focus)
{

    _ui_video_v_show_select_file_name(focus);
    _ui_video_v_show_select_file_size(focus);
    _ui_video_v_update_play_icon();
    _ui_video_v_update_total_time();
    _ui_video_v_update_cur_time(NULL, 0, 0, 0);
    _ui_video_v_show_resolution();
}
/*!
 * list update function
 */
static RET_CODE _ui_video_v_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
    u16         i                           = 0;
    u16         cnt                         = 0;
    file_type_t type                        = UNKNOW_FILE;
    u16         icon_id                     = 0;
    u8          asc_str[32]                 = {0};
    u16         file_uniname[MAX_FILE_PATH] = {0};

    cnt = list_get_count(p_list);
    cnt = MIN(cnt, size + start);

    for (i = start; i < cnt; i++)
    {
        type = ui_video_m_get_type_by_idx(i);

        if(UNKNOW_FILE == type)
        {
            UI_VIDEO_PRINF("[%s] ##err## unknown file\n", __FUNCTION__);
            continue;
        }

        /* NO. */
        sprintf(asc_str, "%.3d ", i + 1);

        list_set_field_content_by_ascstr(p_list, i, 0, asc_str);

        /* ICON */
        if(DIRECTORY == type)
        {
            list_set_field_content_by_icon(p_list, i, 1, IM_MP3_ICON_FOLDER);
        }
        else
        {
            icon_id = ui_video_m_get_video_icon(ui_video_m_get_name_ptr_by_idx(i));
            list_set_field_content_by_icon(p_list, i, 1, icon_id);
        }

        /* NAME */
        ui_video_m_get_name_by_idx(i, file_uniname, MAX_FILE_PATH);

        list_set_field_content_by_unistr(p_list, i, 2, file_uniname);
    }

    return SUCCESS;
}


/*!
 * Video list update path info
 */
static void _ui_video_v_update_path_info(control_t *p_list)
{
    control_t *p_bottom_info = NULL;
    u16        list_focus    = 0;
    u16       *p_path        = NULL;

    p_bottom_info = ctrl_get_child_by_id(p_list->p_parent->p_parent, IDC_VIDEO_V_PATH);

    if(NULL == p_bottom_info)
    {
        return;
    }

    list_focus = list_get_focus_pos(p_list);

    p_path = ui_video_m_get_cur_path();

    if(NULL != p_path)
    {
        text_set_content_by_unistr(p_bottom_info, p_path);
    }

    ctrl_paint_ctrl(p_bottom_info, TRUE);
}

/*!
 * Video list change focus
 */
static RET_CODE _ui_video_v_list_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	RET_CODE ret = SUCCESS;
	control_t* p_popup = NULL;
	control_t *p_next,*p_parent;

	
	if(!is_list)
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
		ret = list_class_proc(p_list, msg, 0, 0);
		p_popup = fw_find_root_by_id(ROOT_ID_POPUP);
		if(p_popup)
		{
			return SUCCESS;
		}

		_ui_video_v_list_scroll(p_list);
	}
    return ret;
}

static RET_CODE _ui_video_v_menu_change_focus(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	control_t  *p_list_f,*p_cont,*p_btn,*p_parent;
	
	if(!is_list)
	{
		is_list = TRUE;
		p_cont = ctrl_get_child_by_id(ctrl_get_parent(p_list),IDC_VIDEO_V_LIST_CONT);
		p_list_f = ctrl_get_child_by_id(p_cont,IDC_VIDEO_V_LIST);
              
              _ui_video_v_list_scroll(p_list_f);
		ctrl_process_msg(p_list, MSG_LOSTFOCUS, 0, 0); 
		ctrl_process_msg(p_list_f, MSG_GETFOCUS, 0, 0); 
		ctrl_paint_ctrl(p_list,TRUE);
		ctrl_paint_ctrl(p_list_f,TRUE);
                manage_open_menu(ROOT_ID_FUN_HELP, 0, 0);
	}else
	{
		is_list = FALSE;
		p_cont=ctrl_get_parent(p_list);
		p_parent=ctrl_get_parent(p_cont);
		p_btn = ctrl_get_child_by_id(p_parent,IDC_VIDEO_V_GROUP_TITLE2_CONT);
                gui_stop_roll(p_list);
		ctrl_process_msg(p_list, MSG_LOSTFOCUS, 0, 0); 
		ctrl_process_msg(p_btn, MSG_GETFOCUS, 0, 0); 
              ctrl_paint_ctrl(p_list,TRUE);
		ctrl_paint_ctrl(p_btn,TRUE);
        UI_PRINTF("@@@@###@_ui_video_v_menu_change_focus\n");
              manage_close_menu(ROOT_ID_FUN_HELP, 0, 0);
	}
	return SUCCESS;
}

/*!
 * Check same playing file
 */
static BOOL _ui_video_v_same_file(u16 focus)
{
    u16 *p_last_file = NULL;
    u16 *p_next_file = NULL;

    p_last_file = ui_video_m_get_cur_playing_file_path();
    p_next_file = ui_video_m_get_path_by_idx(focus);

    if((NULL == p_last_file) || (NULL == p_last_file) || (NULL == p_next_file))
    {
        UI_VIDEO_PRINF("[%s] ##err##\n", __FUNCTION__);
        return FALSE;
    }

    UI_VIDEO_PRINF("[%s] last:%s cur:%s\n", __FUNCTION__, p_last_file, p_next_file);

    if(0 == uni_strcmp(p_last_file, p_next_file))
    {
        return TRUE;
    }

    return FALSE;
}

/*!
 * Play video by focus
 */
static RET_CODE _ui_video_v_play_continue(u16 focus)
{
    RET_CODE  ret    = ERR_FAILURE;
    u16      *p_path = NULL;
    comm_dlg_data_t choose_dlg = {
                                    ROOT_ID_USB_FILEPLAY,
                                    DLG_FOR_ASK | DLG_STR_MODE_STATIC,
                                    COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W,COMM_DLG_H,
                                    IDS_RESUME_PLAY,
                                    0,
                                 };

    p_path = ui_video_m_get_path_by_idx(focus);

    if(NULL == p_path)
    {
        return ERR_FAILURE;
    }

    if(ui_comm_dlg_open(&choose_dlg) == DLG_RET_YES)
    {
        //ret = ui_video_c_load_media_from_local(p_path, TRUE);
        ret = ui_video_c_play_by_file(p_path, ui_video_c_get_play_time());
    }
    else
    {
        //ret = ui_video_c_load_media_from_local(p_path, FALSE);
        ret = ui_video_c_play_by_file(p_path, 0);
    }

    return ret;
}

static BOOL _ui_video_v_check_continue(u16 focus)
{
    BOOL check = FALSE;

    check = _ui_video_v_same_file(focus);

    if(FALSE == check)
    {
        return FALSE;
    }

    check = ui_video_c_get_usr_stop_flag();

    if(FALSE == check)
    {
        return FALSE;
    }

    check = ui_video_m_check_cur_playing_file_support();

    if(FALSE == check)
    {
        return FALSE;
    }

    return TRUE;
}
/*!
 * Play video by focus
 */
static RET_CODE _ui_video_v_play_by_focus(u16 focus)
{
    u16               *p_path  = NULL;

    p_path = ui_video_m_get_path_by_idx(focus);

    if(NULL == p_path)
    {
        return ERR_FAILURE;
    }

    UI_VIDEO_PRINF("[%s] play file:%s\n", __FUNCTION__, p_path);

    /*!
     * Stop logo, otherwise it will show masaic
     */
    _ui_video_v_stop_show_logo();

    /*!
     * If show "unsupport" last time, hide it
     */
    _ui_video_v_hide_unsupport();

    /*!
     * Check play at begain or play at the time which is stopped by user
     */
    if(_ui_video_v_check_continue(focus))
    {
        _ui_video_v_play_continue(focus);
    }
    else
    {
        //ui_video_c_load_media_from_local(p_path, FALSE)
        ui_video_c_play_by_file(p_path, 0);
        ui_video_m_set_cur_playing_file_by_idx(focus);
        UI_PRINTF("@@@_ui_video_v_play_by_focus() idx=%d\n", focus);
    }

    return SUCCESS;
}

static RET_CODE _ui_video_v_on_stop_cfm(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    BOOL usr_stop = FALSE;

    usr_stop = ui_video_c_get_usr_stop_flag();
    mtos_printk("#######_ui_video_v_on_stop_cfm(), usr_stop=[%d]#####\n", usr_stop);
    if(is_full_stop)
        {
            is_full_stop = FALSE;
            return SUCCESS;
    }
    if(usr_stop == FALSE)
    {
        ui_video_v_close_menu();
    }

    return SUCCESS;
}
void _ui_video_v_set_full_stop_states(BOOL states)
{
    is_full_stop = states;
}
static RET_CODE _ui_video_v_on_load_media_success(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    rect_size_t rect = {0};
    video_play_time_t time = {0};
    u16 focus  = 0;
   
    focus = ui_video_m_get_cur_playing_idx();
    UI_PRINTF("@@@_ui_video_v_on_load_media_success() idx=%d\n", focus);

    if ( ROOT_ID_USB_FILEPLAY != fw_get_focus_id() )
    {
      return SUCCESS;
    }

    ui_video_c_sub_hide();
    ui_video_m_set_support_flag_by_idx(focus, TRUE);

    /*!
     * Show name/size/icon/total time/current time/resolution
     */
     ui_video_c_get_resolution(&rect);
     ui_video_m_save_resolution(&rect);
     ui_video_c_get_total_time(&time);
     ui_video_m_save_total_time(&time);
    /*!
     * Show name/size/icon/total time/current time/resolution
     */
    _ui_video_v_update_preview_info(focus);
    
    return SUCCESS;
}
static RET_CODE _ui_video_v_on_load_media_error(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16 focus  = 0;

    UI_PRINTF("@@@on_netplay_load_media_error()\n");

    if ( ROOT_ID_USB_FILEPLAY != fw_get_focus_id() )
    {
      return SUCCESS;
    }

    focus = ui_video_m_get_cur_playing_idx();
    ui_video_m_set_support_flag_by_idx(focus, FALSE);

    _ui_video_v_show_unsupport();

    return SUCCESS;
}
/*!
 * Pause or resume
 */
static RET_CODE _ui_video_v_pause_resume(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    RET_CODE            ret   = SUCCESS;
    mul_fp_play_state_t state = MUL_PLAY_STATE_NONE;
    u16                 focus = 0;
    state = ui_video_c_get_play_state();
    if(state == MUL_PLAY_STATE_NONE || state == MUL_PLAY_STATE_LOAD_MEDIA)
    {
        return SUCCESS;
    }
    if(state == MUL_PLAY_STATE_STOP)
    {
        focus = list_get_focus_pos(p_list);
        if(_ui_video_v_check_continue(focus))
        {
            _ui_video_v_play_continue(focus);
        }

        return SUCCESS;
    }
    ret = ui_video_c_pause_resume();
    if(SUCCESS == ret)
    {
        _ui_video_v_update_play_icon();
    }
    return SUCCESS;
}

/*!
 * Stop video playing
 */
static RET_CODE _ui_video_v_stop(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    RET_CODE ret = SUCCESS;

    UI_VIDEO_PRINF("[%s]: stop\n", __FUNCTION__);

    ret = ui_video_c_stop();
    ui_video_c_set_usr_stop_flag(TRUE);

    _ui_video_v_update_play_icon();

    _ui_video_v_reset_play_cur_time(p_list);

    return ret;
}

/*!
 * Stop video playing
 */
static RET_CODE _ui_video_v_on_eos(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    UI_VIDEO_PRINF("[%s]: eos \n", __FUNCTION__);

    /*!
      * Need to clear when play is end
      */
      ui_video_c_play_end();

    _ui_video_v_update_play_icon();

    _ui_video_v_reset_play_cur_time(p_list);

    return SUCCESS;
}
/*!
 * After enter next folder or partition, need to check current playing file is in it or not
 */
static void _ui_video_v_set_cur_playing_file_focus(control_t *p_list)
{
    RET_CODE ret = SUCCESS;
    u32      idx = 0;

    ret = ui_video_m_find_idx_by_cur_play_file(&idx);

    if(SUCCESS == ret)
    {
        UI_VIDEO_PRINF("[%s]: idx: %d \n", __FUNCTION__, idx);
        list_set_focus_pos(p_list, (u16)idx);
        list_select_item(p_list, (u16)idx);
    }
}
/*!
 * Rebuild list when enter next folder or next partition
 */
static void _ui_video_v_rebuild_list(control_t *p_list)
{
    u32 cnt = 0;

    cnt = ui_video_m_get_total();

    list_set_count(p_list, (u16)cnt, VIDEO_V_LIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(p_list, 0);

    _ui_video_v_list_update(p_list, list_get_valid_pos(p_list), VIDEO_V_LIST_ITEM_NUM_ONE_PAGE, 0);
    _ui_video_v_set_cur_playing_file_focus(p_list);

    ctrl_paint_ctrl(p_list, TRUE);

    _ui_video_v_list_scroll(p_list);
    _ui_video_v_update_path_info(p_list);
}
/*!
 * Update total time
 */
static RET_CODE _ui_video_v_up_folder(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16 *p_path = NULL;
    u8   len    = 0;

    p_path = ui_video_m_get_path_by_idx(0);

    if(NULL == p_path)
    {
        return ERR_FAILURE;
    }

    len = uni_strlen(p_path);

    UI_VIDEO_PRINF("[%s]: %d %d . =%d\n", __FUNCTION__, p_path[len - 1], p_path[len - 2], '.');

    if((len > 2) && ('.' != (u8)p_path[len - 1] || '.' != (u8)p_path[len - 2]))
    {
        // root dir
        return ERR_FAILURE;
    }

    if(ui_video_m_back_to_parent_dir() == SUCCESS)
    _ui_video_v_rebuild_list(p_list);

    return SUCCESS;
}
/*!
 * Receive fullscreen close msg
 */
static RET_CODE _ui_video_v_on_fullscreen_close(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16 focus = 0;

    UI_VIDEO_PRINF("[%s]: fullscreen close \n", __FUNCTION__);

    _ui_video_v_set_cur_playing_file_focus(p_list);

    focus = list_get_focus_pos(p_list);

    _ui_video_v_update_preview_info(focus);

    return SUCCESS;
}
static RET_CODE _ui_video_v_change_partition(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16 cur_partition = 0;
    u16 focus = 0;
    u32 partition_cnt = 0;
    control_t *p_cbox = NULL;
    partition_t * p_partition = NULL;
    flist_dir_t g_flist_dir = NULL;
    comm_dlg_data_t dlg_data = {0};
    u16 p_unistr[3] = {0,0x3a,0};
    partition_cnt =  file_list_get_partition(&p_partition);
    p_cbox = ctrl_get_child_by_id(ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_GROUP_CONT),IDC_VIDEO_V_GROUP);
	if(para2 != 0)
	{
		 p_unistr[0] = (u16)para2;
		 cbox_dync_set_count(p_cbox, (u16)partition_cnt);
        cbox_dync_set_update(p_cbox, fileplay_partition_group_update);
		 cbox_dync_set_focus(p_cbox, 0);
		 ctrl_paint_ctrl(p_cbox->p_parent, TRUE);
	} 
	else
	{
		if(partition_cnt > 1)
		{
      	     cbox_dync_set_count(p_cbox, (u16)partition_cnt);
		     cbox_class_proc(p_cbox, MSG_INCREASE, 0, 0);
		}
	}
    dlg_data.parent_root = ROOT_ID_USB_FILEPLAY;
    dlg_data.style = DLG_FOR_SHOW | DLG_STR_MODE_STATIC;
    dlg_data.x = ((SCREEN_WIDTH - 400) / 2);
  dlg_data.y = ((SCREEN_HEIGHT - 200) / 2);
    dlg_data.w = 400;
    dlg_data.h = 160;
    dlg_data.content = IDS_WAIT_PLEASE;
    dlg_data.dlg_tmout = 0;
    ui_comm_dlg_open(&dlg_data); 
    ui_evt_disable_ir();
    file_list_get_partition(&p_partition);
    focus = cbox_dync_get_focus(p_cbox);
    g_flist_dir = file_list_enter_dir(NULL, MAX_FILE_COUNT, p_partition[focus].letter);
    cur_partition = ui_video_m_get_cur_partition();
    if(g_flist_dir != NULL)
    {
      file_list_leave_dir(g_flist_dir);
      if(ui_video_m_load(cur_partition + 1) == SUCCESS)
      _ui_video_v_rebuild_list(p_list);
    }
    else
    {
      OS_PRINTF("\n======_ui_video_v_change_partition file_list_enter_dir failure. ====\n");
    }

    ui_evt_enable_ir();
    ui_comm_dlg_close();
    return SUCCESS;
}

static void _ui_video_v_enter_fullscreen(void)
{
    UI_PRINTF("@@@@###@_ui_video_v_enter_fullscreen\n");
    manage_close_menu(ROOT_ID_FUN_HELP, 0, 0);
    manage_open_menu(ROOT_ID_FILEPLAY_BAR, 0, (u32)(*ui_video_m_get_cur_playing_file_path()));

    //ui_video_c_sub_show();
}
/*!
 * Stop video playing
 */
static RET_CODE _ui_video_v_fullscreen(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    mul_fp_play_state_t state = ui_video_c_get_play_state();

    if((MUL_PLAY_STATE_PLAY == state) || (MUL_PLAY_STATE_PAUSE == state))
    {
        _ui_video_v_enter_fullscreen();
    }

    return SUCCESS;
}
/*!
 * Enter folder
 */
static void _ui_video_v_enter_folder(control_t *p_list, u16 focus)
{
    if(focus > 0)
    {
        if(ui_video_m_go_to_dir(focus) == SUCCESS)
            _ui_video_v_rebuild_list(p_list);
    }
    else
    {
        u32 i = 0;
	 u16 *p_path     = NULL;
        u16 father_filename[MAX_FILE_PATH] = {0};
	 u16 preDirLen = 0, curFileLen = 0;
	 uni_strncpy(father_filename, ui_video_m_get_cur_path(), MAX_FILE_PATH);
	 preDirLen = uni_strlen(father_filename);
		
        if(ui_video_m_go_to_dir(focus) == SUCCESS)
    	{
    	    u32 cnt = 0;
     	    cnt = ui_video_m_get_total();
     	    list_set_count(p_list, (u16)cnt, VIDEO_V_LIST_ITEM_NUM_ONE_PAGE);
     	    list_set_focus_pos(p_list, 0);
	    _ui_video_v_list_update(p_list, list_get_valid_pos(p_list), VIDEO_V_LIST_ITEM_NUM_ONE_PAGE, 0);
	  
           for(i = 0; i < cnt; i++)
           {
              p_path = ui_video_m_get_path_by_idx(i);

		if(NULL == p_path){
			continue;
		}
		else{
			curFileLen = uni_strlen(p_path);  
			if(preDirLen != curFileLen)
				continue;
		}
		
      	      if(uni_strncmp(father_filename,p_path,preDirLen) == 0)
      	      {
      		      list_set_focus_pos(p_list, i);
      		      list_select_item(p_list, i);
		      break;  
      	      }
           }
	     UI_VIDEO_PRINF("[%s]: focus = %d  i=%d cnt = %d\n", __FUNCTION__,focus,i,cnt);
     	    ctrl_paint_ctrl(p_list, TRUE);
     	    _ui_video_v_list_scroll(p_list);
     	    _ui_video_v_update_path_info(p_list);
    	}
    }

}
/*!
 * Play video
 */
static void _ui_video_v_play_video(control_t *p_list, u16 focus)
{
    mul_fp_play_state_t state = ui_video_c_get_play_state();
    BOOL b_same = _ui_video_v_same_file(focus);

    UI_PRINTF("@@@_ui_video_v_play_video() state=%d\n", state);
    if (b_same == TRUE && MUL_PLAY_STATE_LOAD_MEDIA == state)
    {
        return;
    }
    if(b_same == TRUE && MUL_PLAY_STATE_PLAY == state)
    {
        if (ui_video_m_check_cur_playing_file_support() == TRUE)
        {
        _ui_video_v_enter_fullscreen();
        }
        return;
    }
    if(b_same == TRUE && MUL_PLAY_STATE_PAUSE == state)
    {
        ui_video_c_pause_resume();
        _ui_video_v_update_play_icon();
        return;
    }

    _ui_video_v_play_by_focus(focus);

    list_select_item(p_list, focus);
    ctrl_paint_ctrl(p_list, TRUE);
}
/*!
 * Video list select
 */
static RET_CODE _ui_video_v_list_select(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16         focus  = 0;
    file_type_t type   = UNKNOW_FILE;

    focus = list_get_focus_pos(p_list);
    play_focus = focus;
    
    if(INVALIDID == focus)
    {
        return ERR_FAILURE;
    }
    type = ui_video_m_get_type_by_idx(focus);

    UI_VIDEO_PRINF("[%s]: type: %d", __FUNCTION__, type);

    if(UNKNOW_FILE == type)
    {
        return ERR_FAILURE;
    }
    if(DIRECTORY == type)
    {
        _ui_video_v_enter_folder(p_list, focus);
    }
    else
    {
        _ui_video_v_play_video(p_list, focus);
    }

    return SUCCESS;
}
/*!
 * Create play state icon control
 */
static void _ui_video_v_creat_play_icon(control_t *p_play)
{
    control_t *p_play_icon = NULL;

    p_play_icon = ctrl_create_ctrl(CTRL_BMAP,
                                   IDC_VIDEO_V_PLAY_ICON,
                                   VIDEO_V_PLAY_ICON_X,
                                   VIDEO_V_PLAY_ICON_Y+10,
                                   VIDEO_V_PLAY_ICON_W,
                                   VIDEO_V_PLAY_ICON_H,
                                   p_play,
                                   0);

    if(NULL == p_play_icon)
    {
        return;
    }
}
/*!
 * Create play detail control
 */
static control_t * _ui_video_v_creat_play_container(control_t *p_menu)
{
    control_t *p_play_cont = NULL;

    p_play_cont = ctrl_create_ctrl(CTRL_CONT,
                                   IDC_VIDEO_V_PLAY_DETAIL_CONT,
                                   VIDEO_V_PLAY_CONT_X,
                                   VIDEO_V_PLAY_CONT_Y,
                                   VIDEO_V_PLAY_CONT_W,
                                   VIDEO_V_PLAY_CONT_H,
                                   p_menu,
                                   0);
    if(NULL == p_play_cont)
    {
        return NULL;
    }

    ctrl_set_rstyle(p_play_cont, RSI_BOX_1, RSI_BOX_1, RSI_BOX_1);

    return p_play_cont;
}
/*!
 * Create play total time control
 */
static void _ui_video_v_creat_play_total_time(control_t *p_play)
{
    control_t *p_total_time = NULL;

    p_total_time = ctrl_create_ctrl(CTRL_TEXT,
                                    IDC_VIDEO_V_PLAY_TOLTM,
                                    VIDEO_V_PLAY_TOLTM_X,
                                    VIDEO_V_PLAY_TOLTM_Y+10,
                                    VIDEO_V_PLAY_TOLTM_W,
                                    VIDEO_V_PLAY_TOLTM_H,
                                    p_play,
                                    0);

    if(NULL == p_total_time)
    {
        return;
    }

    ctrl_set_rstyle(p_total_time, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR);

    text_set_font_style(p_total_time, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR);
    text_set_align_type(p_total_time, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_total_time, TEXT_STRTYPE_UNICODE);
}
/*!
 * Create play current time control
 */
static void _ui_video_v_creat_play_cur_time(control_t *p_play)
{
    control_t *p_cur_time = NULL;

    p_cur_time = ctrl_create_ctrl(CTRL_TEXT,
                                  IDC_VIDEO_V_PLAY_CURTM,
                                  VIDEO_V_PLAY_CURTM_X,
                                  VIDEO_V_PLAY_CURTM_Y+10,
                                  VIDEO_V_PLAY_CURTM_W,
                                  VIDEO_V_PLAY_CURTM_H,
                                  p_play,
                                  0);
    if(NULL == p_cur_time)
    {
        return;
    }

    ctrl_set_rstyle(p_cur_time, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR);
    text_set_font_style(p_cur_time, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR);
    text_set_align_type(p_cur_time, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_cur_time, TEXT_STRTYPE_UNICODE);
}

/*!
 * Create play file name control
 */
static void _ui_video_v_creat_play_file_name(control_t *p_play)
{
    control_t *p_name = NULL;

    p_name = ctrl_create_ctrl(CTRL_TEXT,
                              IDC_VIDEO_V_PLAY_FILENAME,
                              VIDEO_V_DETAIL_FILE_NAME_X+12,
                              VIDEO_V_DETAIL_FILE_NAME_Y+10,
                              VIDEO_V_DETAIL_FILE_NAME_W,
                              VIDEO_V_DETAIL_FILE_NAME_H,
                              p_play,
                              0);
    MT_ASSERT(p_name != NULL);

    ctrl_set_rstyle(p_name, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR);
    text_set_font_style(p_name, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR);
    text_set_align_type(p_name, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_name, TEXT_STRTYPE_UNICODE);
}

/*!
 * Create play file name control
 */
static void _ui_video_v_creat_play_file_size(control_t *p_play)
{
    control_t *p_name = NULL;

    p_name = ctrl_create_ctrl(CTRL_TEXT,
                              IDC_VIDEO_V_FILE_SIZE,
                              VIDEO_V_DETAIL_FILE_SIZE_X,
                              VIDEO_V_DETAIL_FILE_SIZE_Y+10,
                              VIDEO_V_DETAIL_FILE_SIZE_W,
                              VIDEO_V_DETAIL_FILE_SIZE_H,
                              p_play,
                              0);
    MT_ASSERT(p_name != NULL);

    ctrl_set_rstyle(p_name, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR);
    text_set_font_style(p_name, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR);
    text_set_align_type(p_name, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_name, TEXT_STRTYPE_UNICODE);
}
/*!
 * Create play file name control
 */
static void _ui_video_v_creat_play_file_pixel(control_t *p_play)
{
    control_t *p_pix = NULL;

    p_pix = ctrl_create_ctrl(CTRL_TEXT,
                             IDC_VIDEO_V_FILE_PIXEL,
                             VIDEO_V_DETAIL_FILE_PIXEL_X+12,
                             VIDEO_V_DETAIL_FILE_PIXEL_Y+10,
                             VIDEO_V_DETAIL_FILE_PIXEL_W,
                             VIDEO_V_DETAIL_FILE_PIXEL_H,
                              p_play,
                              0);
    MT_ASSERT(p_pix != NULL);

    ctrl_set_rstyle(p_pix, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR, VIDEO_V_RSI_COLOR);
    text_set_font_style(p_pix, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR, VIDEO_V_FSI_COLOR);
    text_set_align_type(p_pix, STL_LEFT | STL_VCENTER);
    text_set_content_type(p_pix, TEXT_STRTYPE_UNICODE);
}
/*!
 * Create play list scrolbar control
 */
static control_t * _ui_video_v_creat_sbar(control_t *p_menu)
{
    control_t *p_sbar = NULL;

    p_sbar = ctrl_create_ctrl(CTRL_SBAR,
                                   IDC_VIDEO_V_SBAR,
                                   VIDEO_V_LIST_SBAR_X,
                                   VIDEO_V_LIST_SBAR_Y,
                                   VIDEO_V_LIST_SBARW,
                                   VIDEO_V_LIST_SBARH,
                                   p_menu,
                                   0);

    MT_ASSERT(p_sbar != NULL);

    ctrl_set_rstyle(p_sbar, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG, RSI_SCROLL_BAR_BG);
    sbar_set_autosize_mode(p_sbar, TRUE);
    sbar_set_direction(p_sbar, 0);

    sbar_set_mid_rstyle(p_sbar,
                        RSI_SCROLL_BAR_MID,
                        RSI_SCROLL_BAR_MID,
                        RSI_SCROLL_BAR_MID);

    ctrl_set_mrect(p_sbar,
                   VIDEO_V_LIST_SBAR_MIDL,
                   VIDEO_V_LIST_SBAR_MIDT,
                   VIDEO_V_LIST_SBAR_MIDR,
                   VIDEO_V_LIST_SBAR_MIDB);
    return p_sbar;
}

static RET_CODE video_v_sort_list_update(control_t* p_list, u16 start, u16 size,
                                 u32 context)
{
  u16 i;
  u16 cnt = list_get_count(p_list);
  u16 strid [VIDEO_V_SORT_LIST_ITEM_TOL] = {
    IDS_PARTITION,IDS_FULL,
  };

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


/*!
 * Create video list control
 */
static void _ui_video_v_creat_list(control_t *p_menu, control_t *p_sbar)
{
    u8         i      = 0;
    u16        cnt    = 0;
    control_t *p_list_cont = NULL, *p_list = NULL,*p_sort_list = NULL;

    //create plist container
		p_list_cont =
			ctrl_create_ctrl(CTRL_CONT, IDC_VIDEO_V_LIST_CONT, 
			VIDEO_V_LIST_CONTX,
			VIDEO_V_LIST_CONTY, 
			VIDEO_V_LIST_CONTW,
			VIDEO_V_LIST_CONTH, p_menu,
			0);

		ctrl_set_rstyle(p_list_cont, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG);
    p_list = ctrl_create_ctrl(CTRL_LIST,
                              IDC_VIDEO_V_LIST,
                              VIDEO_V_LIST_X,
                              VIDEO_V_LIST_Y,
                              VIDEO_V_LISTW,
                              VIDEO_V_LISTH,
                              p_list_cont,
                              0);

    ctrl_set_rstyle(p_list, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG);
    if(is_usb)
    {
    ctrl_set_keymap(p_list, _ui_video_v_list_keymap);
    ctrl_set_proc(p_list, _ui_video_v_list_proc);
    }
    else
    {
    ctrl_set_keymap(p_list, _ui_video_network_v_list_keymap);
    ctrl_set_proc(p_list, _ui_video_network_v_list_proc);
    }
    ctrl_set_mrect(p_list,
                   VIDEO_V_LIST_MIDL,
                   VIDEO_V_LIST_MIDT,
                   VIDEO_V_LIST_MIDW + VIDEO_V_LIST_MIDL,
                   VIDEO_V_LIST_MIDH + VIDEO_V_LIST_MIDT);

    list_set_item_interval(p_list, VIDEO_V_LIST_ICON_VGAP);

    list_set_item_rstyle(p_list, &video_v_list_field_rstyle);

    list_enable_select_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);

    cnt = (u16)ui_video_m_get_total();

    list_set_count(p_list, cnt, VIDEO_V_LIST_ITEM_NUM_ONE_PAGE);
    list_set_field_count(p_list, ARRAY_SIZE(video_list_attr), VIDEO_V_LIST_ITEM_NUM_ONE_PAGE);
    list_set_focus_pos(p_list, 0);
    list_set_update(p_list, _ui_video_v_list_update, 0);
   // ctrl_set_style(p_list, STL_EX_ALWAYS_HL);

    for (i = 0; i < ARRAY_SIZE(video_list_attr); i++)
    {
        list_set_field_attr(p_list,
                            i,
                            video_list_attr[i].attr,
                            video_list_attr[i].width,
                            video_list_attr[i].left,
                            video_list_attr[i].top);

        list_set_field_rect_style(p_list, i, video_list_attr[i].rstyle);
        list_set_field_font_style(p_list, i, video_list_attr[i].fstyle);
    }

    //list_set_scrollbar(p_list, p_sbar);
    _ui_video_v_list_update(p_list, list_get_valid_pos(p_list), VIDEO_V_LIST_ITEM_NUM_ONE_PAGE, 0);
   // _ui_video_v_list_scroll(p_list);

      p_sort_list = ctrl_create_ctrl(CTRL_LIST, IDC_VIDEO_V_SORT_LIST,
                            VIDEO_V_SORT_LIST_X, VIDEO_V_SORT_LIST_Y,
                            VIDEO_V_SORT_LIST_W, VIDEO_V_SORT_LIST_H,
                            p_menu,
                            0);
  ctrl_set_rstyle(p_sort_list, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG, RSI_MEDIO_LEFT_LIST_BG);//RSI_COMMAN_BG
  ctrl_set_keymap(p_sort_list, video_v_sort_list_keymap);
  ctrl_set_proc(p_sort_list, video_v_sort_list_proc);
  ctrl_set_mrect(p_sort_list, VIDEO_V_SORT_LIST_MIDL, VIDEO_V_SORT_LIST_MIDT,
                    VIDEO_V_SORT_LIST_MIDW+VIDEO_V_SORT_LIST_MIDL, VIDEO_V_SORT_LIST_MIDH+VIDEO_V_SORT_LIST_MIDT);
  list_set_item_interval(p_sort_list, 0);
  list_set_item_rstyle(p_sort_list, &video_v_list_field_rstyle);
  list_set_count(p_sort_list, VIDEO_V_SORT_LIST_ITEM_TOL, VIDEO_V_SORT_LIST_ITEM_PAGE);
  list_set_field_count(p_sort_list, VIDEO_V_SORT_LIST_FIELD_NUM, VIDEO_V_SORT_LIST_ITEM_PAGE);
  list_set_focus_pos(p_sort_list, 0);
  list_set_update(p_sort_list, video_v_sort_list_update, 0);

  for (i = 0; i < VIDEO_V_SORT_LIST_FIELD_NUM; i++)
  {
    list_set_field_attr(p_sort_list, (u8)i,
                        (u32)(v_sort_list_field_attr[i].attr),
                        (u16)(v_sort_list_field_attr[i].width),
                        (u16)(v_sort_list_field_attr[i].left),
                        (u8)(v_sort_list_field_attr[i].top));
    list_set_field_rect_style(p_sort_list, (u8)i, v_sort_list_field_attr[i].rstyle);
    list_set_field_font_style(p_sort_list, (u8)i, v_sort_list_field_attr[i].fstyle);
  }
  video_v_sort_list_update(p_sort_list, list_get_valid_pos(p_sort_list), VIDEO_V_SORT_LIST_ITEM_PAGE, 0);
  //ctrl_set_attr(p_sort_list, OBJ_ATTR_HIDDEN);
  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);

    ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0);
}

/*!
 * Create video preview control
 */
static void _ui_video_v_creat_preview(control_t *p_menu)
{
 /*   control_t *p_preview = NULL;

    p_preview = ctrl_create_ctrl(CTRL_CONT,
                                 IDC_VIDEO_V_PREVIEW,
                                 VIDEO_V_PREV_X,
                                 VIDEO_V_PREV_Y,
                                 VIDEO_V_PREV_W,
                                 VIDEO_V_PREV_H,
                                 p_menu,
                                 0);

    MT_ASSERT(p_preview != NULL);

    ctrl_set_rstyle(p_preview, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);*/
}

/*!
 * Create video help bar control
 */
static void _ui_video_v_creat_help(control_t *p_menu)
{
    control_t *p_bottom_info = NULL;
    //TODO:use macro
   /* if(is_usb)
    {
    p_bottom_help = ctrl_create_ctrl(CTRL_TEXT, IDC_VIDEO_V_BTM_HELP,
                   VIDEO_V_BOTTOM_HELP_X, VIDEO_V_BOTTOM_HELP_Y, VIDEO_V_BOTTOM_HELP_W,
                   VIDEO_V_BOTTOM_HELP_H, p_menu, 0);
  ctrl_set_rstyle(p_bottom_help, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
  text_set_font_style(p_bottom_help, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_bottom_help, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_bottom_help, TEXT_STRTYPE_UNICODE);
  ui_comm_help_create_ext(60, 0, VIDEO_V_BOTTOM_HELP_W-60, VIDEO_V_BOTTOM_HELP_H,  &movie_help_data,  p_bottom_help);
    }
    else
      {
      p_bottom_help = ctrl_create_ctrl(CTRL_TEXT, IDC_VIDEO_V_BTM_HELP,
                   VIDEO_V_BOTTOM_HELP_X, VIDEO_V_BOTTOM_HELP_Y, VIDEO_V_BOTTOM_HELP_W,
                   VIDEO_V_BOTTOM_HELP_H, p_menu, 0);
  ctrl_set_rstyle(p_bottom_help, RSI_BOX_2, RSI_BOX_2, RSI_BOX_2);
  text_set_font_style(p_bottom_help, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_bottom_help, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_bottom_help, TEXT_STRTYPE_UNICODE);
  ui_comm_help_create_ext(60, 0, VIDEO_V_BOTTOM_HELP_W-60, VIDEO_V_BOTTOM_HELP_H,  &movie_help_network_data,  p_bottom_help);
      }*/
p_bottom_info = ctrl_create_ctrl(CTRL_TEXT, IDC_VIDEO_V_PATH,
                   VIDEO_V_PATH_X, VIDEO_V_PATH_Y, VIDEO_V_PATH_W,
                   VIDEO_V_PATH_H, p_menu, 0);
  ctrl_set_rstyle(p_bottom_info, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  text_set_font_style(p_bottom_info, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_align_type(p_bottom_info, STL_LEFT | STL_VCENTER);
  text_set_content_type(p_bottom_info, TEXT_STRTYPE_UNICODE);
    if(ui_video_m_get_cur_path() != NULL)
    {
        text_set_content_by_unistr(p_bottom_info, ui_video_m_get_cur_path());
    }
}

static void _ui_video_v_creat_title_group(control_t *p_menu, BOOL is_usb)
{
 control_t *p_title,*p_ctrl;
 u16 y;
 u16 i;
u16 bmp_item[3] = {
IM_XMEDIO_LEFT_VEDIO_F,IM_XMEDIO_LEFT_MUSIC,IM_XMEDIO_LEFT_PIC
};
 /*u16 str_id[] =
  {
    IDS_VIDEO,IDS_MUSIC,IDS_PICTURE,IDS_RECORD
  };*/
  y = VIDEO_V_TITLE_TEXT1_Y;

 for(i = 0;i<3;i++)
 {
  p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_VIDEO_V_GROUP_TITLE1_CONT+i,
							VIDEO_V_TITLE_TEXT1_X,
							y, 
							VIDEO_V_TITLE_TEXT1_W,
							VIDEO_V_TITLE_TEXT1_H, p_menu,
			0);

  ctrl_set_rstyle(p_title, RSI_MEDIO_BTN_SH, RSI_MEDIO_BTN_HL, RSI_MEDIO_BTN_SH);
	ctrl_set_keymap(p_title, _ui_video_v_txt_keymap);
	ctrl_set_proc(p_title, _ui_video_v_txt_proc);
  text_set_align_type(p_title, STL_CENTER | STL_VCENTER);
  text_set_font_style(p_title, FSI_WHITE, FSI_WHITE, FSI_WHITE);
  text_set_content_type(p_title, TEXT_STRTYPE_STRID);
  //text_set_content_by_strid(p_title, str_id[i]);
if(i == 0)
{
	ctrl_set_attr(p_title, OBJ_ATTR_INACTIVE);
}
	switch(i)
	{
		case 1:
			ctrl_set_related_id(p_title,
                    0,                                                            /* left */
                    IDC_VIDEO_V_GROUP_TITLE3_CONT,                  /* up */
                    0,                                                            /* right */
                    IDC_VIDEO_V_GROUP_TITLE3_CONT); /* down */
		break;
		case 2:
			ctrl_set_related_id(p_title,
                    0,                                                            /* left */
                    IDC_VIDEO_V_GROUP_TITLE2_CONT,                  /* up */
                    0,                                                            /* right */
                    IDC_VIDEO_V_GROUP_TITLE2_CONT); /* down */
		break;
		case 3:
			ctrl_set_related_id(p_title,
                    0,                                                            /* left */
                    IDC_VIDEO_V_GROUP_TITLE3_CONT,                  /* up */
                    0,                                                            /* right */
                    IDC_VIDEO_V_GROUP_TITLE2_CONT); /* down */
		break;
		default:
		break;
	}
 
p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_VIDEO_V_GROUP_BMP1_CONT + i,
												VIDEO_V_TITLE_TEXT1_X, 0,
												VIDEO_V_TITLE_TEXT1_W,
							                                    VIDEO_V_TITLE_TEXT1_H, p_title, 0);
			bmap_set_content_by_id(p_ctrl, bmp_item[i]);

  y= y+ VIDEO_V_TITLE_TEXT1_H;
 }
 p_title = ctrl_get_child_by_id(p_menu, IDC_VIDEO_V_GROUP_TITLE4_CONT);
 if(!is_usb)
 {
  ctrl_set_attr(p_title, OBJ_ATTR_INACTIVE);
 }

}


static RET_CODE fileplay_network_group_update(control_t *p_cbox, u16 focus, u16 *p_str,
                              u16 max_length)
{
   // u8 ipaddress[128] = "";
    u8 ip[128] = "\\\\";
  // sprintf(ipaddress, "%d.%d.%d.%d",ip_address.s_b1,ip_address.s_b2,ip_address.s_b3,ip_address.s_b4);
   strcat(ip,ipaddress);

  //str_asc2uni(p_partition[focus].name, p_str);
	dvb_to_unicode(ip,max_length - 1, p_str, max_length);
	//uni_strncpy(p_str, p_partition[focus].name, max_length);
  return SUCCESS;
}
static void _ui_video_v_creat_partition_group(control_t *p_menu)
{
    control_t *p_group_cont = NULL, *p_group = NULL;//, *p_ctrl = NULL;
    p_group_cont = ctrl_create_ctrl(CTRL_CONT, IDC_VIDEO_V_GROUP_CONT,
		VIDEO_V_GROUP_CONTX,
			VIDEO_V_GROUP_CONTY, 
			VIDEO_V_GROUP_CONTW,
			VIDEO_V_GROUP_CONTH, p_menu,
			0);
ctrl_set_rstyle(p_group_cont, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);

if(is_usb)
{
  //group arrow
 /* p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_VIDEO_V_GROUP_ARROWL,
                             VIDEO_V_GROUP_ARROWL_X, VIDEO_V_GROUP_ARROWL_Y,
                             VIDEO_V_GROUP_ARROWL_W, VIDEO_V_GROUP_ARROWL_H,
                             p_group_cont, 0);
  bmap_set_content_by_id(p_ctrl, IM_ARROW_L);

  p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_VIDEO_V_GROUP_ARROWR,
                             VIDEO_V_GROUP_ARROWR_X, VIDEO_V_GROUP_ARROWR_Y,
                             VIDEO_V_GROUP_ARROWR_W, VIDEO_V_GROUP_ARROWR_H,
                             p_group_cont, 0);
  bmap_set_content_by_id(p_ctrl, IM_ARROW_R);*/

  //group
  p_group = ctrl_create_ctrl(CTRL_CBOX, IDC_VIDEO_V_GROUP, VIDEO_V_GROUP_X,
                             VIDEO_V_GROUP_Y, VIDEO_V_GROUP_W,
                             VIDEO_V_GROUP_H, p_group_cont, 0);
  ctrl_set_rstyle(p_group, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);
  cbox_set_font_style(p_group, FSI_VERMILION, FSI_VERMILION, FSI_VERMILION);
  cbox_set_align_style(p_group, STL_CENTER | STL_VCENTER);
  cbox_set_work_mode(p_group, CBOX_WORKMODE_DYNAMIC);
  cbox_enable_cycle_mode(p_group, TRUE);
  cbox_dync_set_count(p_group, ui_video_m_get_partition_cnt());
  cbox_dync_set_update(p_group, fileplay_partition_group_update);
  cbox_dync_set_focus(p_group, 0);
}
else
{
  //group
  p_group = ctrl_create_ctrl(CTRL_CBOX, IDC_VIDEO_V_GROUP, VIDEO_V_GROUP_X,
                             VIDEO_V_GROUP_Y, VIDEO_V_GROUP_W,
                             VIDEO_V_GROUP_H, p_group_cont, 0);
  ctrl_set_rstyle(p_group, RSI_PBACK, RSI_PBACK, RSI_PBACK);
  cbox_set_font_style(p_group, FSI_VERMILION, FSI_VERMILION, FSI_VERMILION);
  cbox_set_align_style(p_group, STL_CENTER | STL_VCENTER);
  cbox_set_work_mode(p_group, CBOX_WORKMODE_DYNAMIC);
  cbox_enable_cycle_mode(p_group, TRUE);
  cbox_dync_set_count(p_group, ui_video_m_get_partition_cnt());
  cbox_dync_set_update(p_group, fileplay_network_group_update);
  cbox_dync_set_focus(p_group, 0);
}
ctrl_set_sts(p_group_cont , OBJ_STS_HIDE);
}

/*!
 * Create video menu control
 */
static control_t * _ui_video_v_creat_menu(void)
{
    control_t *p_menu = NULL,*p_cont,*p_left;

    /*!
     * Create Menu
     */
    p_menu = ui_comm_root_create(ROOT_ID_USB_FILEPLAY,
                                 0,
                                 COMM_BG_X,
                                 COMM_BG_Y,
                                 COMM_BG_W,
                                 COMM_BG_H,
                                 0,
                                 0);

    if(NULL == p_menu)
    {
        return NULL;
    }

    ctrl_set_keymap(p_menu, _ui_video_v_cont_keymap);
    ctrl_set_proc(p_menu, _ui_video_v_cont_proc);
	ctrl_set_rstyle(p_menu, RSI_TRANSPARENT,RSI_TRANSPARENT,RSI_TRANSPARENT);
	p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_VIDEO_V_TITLE_CONT,
                            0, 0, TITLE_W, TITLE_H,
                            p_menu, 0);
     ctrl_set_rstyle(p_cont, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
     p_left = ctrl_create_ctrl(CTRL_CONT, IDC_VIDEO_V_LEFT_BG,
                            VIDEO_V_LEFT_BG_X, 
                            VIDEO_V_LEFT_BG_Y, 
                            VIDEO_V_LEFT_BG_W, 
                            VIDEO_V_LEFT_BG_H,
                            p_menu, 0);
     ctrl_set_rstyle(p_left, RSI_MEDIO_LEFT_BG, RSI_MEDIO_LEFT_BG, RSI_MEDIO_LEFT_BG);
     p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_VIDEO_V_LEFT_HD,
                            VIDEO_V_LEFT_HD_X, VIDEO_V_LEFT_HD_Y, VIDEO_V_LEFT_HD_W, VIDEO_V_LEFT_HD_H,
                            p_left, 0);
     ctrl_set_rstyle(p_cont, RSI_MEDIO_LEFT_LINE_BG, RSI_MEDIO_LEFT_LINE_BG, RSI_MEDIO_LEFT_LINE_BG);
    return p_menu;
}

static void ui_video_v_close_menu()
{
    
    OS_PRINTF("###%s, line[%d]###\n", __FUNCTION__, __LINE__);
    manage_close_menu(ROOT_ID_FUN_HELP, 0, 0);
    if(!is_list)
        {
	switch(now_change)
	{
	case IDC_VIDEO_V_GROUP_TITLE2_CONT:
			 manage_open_menu(ROOT_ID_USB_MUSIC, 0, 0);
			break;
	case IDC_VIDEO_V_GROUP_TITLE3_CONT:
			manage_open_menu(ROOT_ID_USB_PICTURE, 0, 0);
			break;
	case IDC_VIDEO_V_GROUP_TITLE4_CONT:
			// manage_open_menu(ROOT_ID_RECORD_MANAGER, 0, 0);
			break;
		default:
			break;
		}
        }
    is_list = TRUE;
    ui_video_c_destroy();
    manage_close_menu(ROOT_ID_USB_FILEPLAY, 0, 0);
    if(is_exit_all)
    {
        ui_close_all_mennus();
    }
    
}

static void ui_video_v_pre_close_menu()
{
    mul_fp_play_state_t status = {0};

    status = (mul_fp_play_state_t)ui_video_c_get_play_state();
    OS_PRINTF("###%s, status=[%d]####\n", __FUNCTION__, status);
    if(MUL_PLAY_STATE_NONE != status && MUL_PLAY_STATE_STOP != status)
    {
        ui_video_c_stop();
    }
    else
    {  
        ui_video_v_close_menu();
    }
}

static RET_CODE ui_video_v_destroy(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    u16 * p_url = NULL;

    OS_PRINTF("####%s#### line :%d\n", __FUNCTION__,__LINE__);
	ui_stop_play(STOP_PLAY_BLACK, TRUE);
    if((fw_find_root_by_id(ROOT_ID_USB_MUSIC) == NULL) && !is_usb)
    {
        ui_get_ip_path_mount(&p_url);
        pnp_service_vfs_unmount(p_url);
    }
    
     /*!
      * Release rsc
      */
    ui_video_m_rsc_release();
    ui_fp_audio_set_init_audio_ch_num();

    if(is_usb)
    {
       ui_video_m_reset();
    }

     _ui_video_v_stop_show_logo();
     ui_evt_enable_ir();
     
    if(now_change  == 0)
    {
        ui_play_curn_pg();
    }
    else
    {
        now_change = 0;
    }

    return ERR_NOFEATURE;
}

/*!
 * Exit video view
 */
static RET_CODE _ui_video_v_exit(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    BOOL is_enter_dir = FALSE;
    control_t *p_cont_list = NULL;
    control_t *p_list = NULL;

    UI_VIDEO_PRINF("[%s]: exit\n", __FUNCTION__);
    if(!is_usb)
    {
        ui_video_v_pre_close_menu();
        return SUCCESS;
    }

    p_cont_list = ui_comm_root_get_ctrl(ROOT_ID_USB_FILEPLAY, IDC_VIDEO_V_LIST_CONT);
    p_list = ctrl_get_child_by_id(p_cont_list, IDC_VIDEO_V_LIST );
	
    is_enter_dir = ui_video_file_is_enter_dir();
    if(is_enter_dir)
    {
        _ui_video_v_enter_folder(p_list, 0);
    }
    else
    {   
    	 ui_time_enable_heart_beat(FALSE);
        ui_video_v_pre_close_menu();
    }

    return SUCCESS;
}

static RET_CODE _ui_video_v_exit_to_network_place(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
    BOOL is_enter_dir = FALSE;
    
    UI_VIDEO_PRINF("[%s]: exit\n", __FUNCTION__);
    if(!is_usb)
    {
        ui_time_enable_heart_beat(FALSE);
        ui_video_v_pre_close_menu();
        manage_close_menu(ROOT_ID_EDIT_USR_PWD, 0, 0);
        return SUCCESS;
    }  
    
    is_enter_dir = ui_video_file_is_enter_dir();
    if(is_enter_dir)
    {
        _ui_video_v_enter_folder(p_list, 0);
    }
    else
    {   
        ui_time_enable_heart_beat(FALSE);
        ui_video_v_pre_close_menu();
        manage_close_menu(ROOT_ID_EDIT_USR_PWD, 0, 0);
    }
    
    return SUCCESS;
}
static void _ui_video_v_create_unsupport(control_t *p_menu)
{
    control_t *p_unspt = NULL;

    p_unspt = ctrl_create_ctrl(CTRL_TEXT,
                               IDC_VIDEO_V_UNSUPPORT,
                               VIDEO_V_PLAY_UNSUPPORT_X,
                               VIDEO_V_PLAY_UNSUPPORT_Y,
                               VIDEO_V_PLAY_UNSUPPORT_W,
                               VIDEO_V_PLAY_UNSUPPORT_H,
                               p_menu,
                               0);

    if(NULL == p_unspt)
    {
        return;
    }

    ctrl_set_rstyle(p_unspt, RSI_POPUP_BG, RSI_POPUP_BG, RSI_POPUP_BG);
    text_set_font_style(p_unspt, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_align_type(p_unspt, STL_CENTER | STL_VCENTER);
    text_set_content_type(p_unspt, TEXT_STRTYPE_STRID);

    text_set_content_by_strid(p_unspt, IDS_UNSUPPORTED);

    ctrl_set_sts(p_unspt, OBJ_STS_HIDE);

}

static RET_CODE _ui_video_change(control_t *p_list, u16 msg, u32 para1, u32 para2)
{
	ui_evt_disable_ir();
	
       now_change = ctrl_get_ctrl_id(p_list);
       ui_time_enable_heart_beat(FALSE);
	ui_video_v_pre_close_menu();
	return SUCCESS;
}

/*!
 * Video view entry
 */
RET_CODE ui_video_v_open(u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_sbar = NULL;
    control_t *p_play = NULL;
	RET_CODE   ret    = ERR_FAILURE;
	  comm_dlg_data_t dlg_data = {0};	  
	  DEBUG(DBG, INFO, "\n"); 
    if(ui_is_playing())
    {
  	  ui_stop_play(STOP_PLAY_BLACK, TRUE);
    }
    is_usb = (para1 == 0)?1:0;
    if(!is_usb)
    {
     memcpy(ipaddress, (char *)para2, 128);
    }

    is_exit_all = FALSE;
    /*!
     * Reset for forget the played information
     */
    ui_video_m_reset();
    /*!
     * Load video modle data
     */
    dlg_data.parent_root = ROOT_ID_MAINMENU;
    dlg_data.style = DLG_FOR_SHOW | DLG_STR_MODE_STATIC;
    dlg_data.x = ((SCREEN_WIDTH - 400) / 2);
    dlg_data.y = ((SCREEN_HEIGHT - 160) / 2);
    dlg_data.w = 400;
    dlg_data.h = 160;
    dlg_data.content = IDS_WAIT_PLEASE;
    dlg_data.dlg_tmout = 0;
    ui_comm_dlg_open(&dlg_data); 
    ui_evt_disable_ir();
    if(!is_usb)
    {
       ui_get_ip_path(ip_address_with_path);
        ret = ui_video_m_net_load(ip_address_with_path);
     }
    else
    {
    ret = ui_video_m_load(0);
    }
    if(SUCCESS != ret)
    {
      ui_evt_enable_ir();
      ui_comm_dlg_close();
	  ui_play_curn_pg();
      return ret;
    }

    /*!
     * Create video controller
     */
    ui_video_c_create(VIDEO_PLAY_FUNC_USB);


    /*Create Menu*/
    p_menu = _ui_video_v_creat_menu();
    MT_ASSERT(p_menu != NULL);

    /*Create title*/
    _ui_video_v_creat_title_group(p_menu, is_usb);

    /*!
     * Create help bar
     */
    _ui_video_v_creat_help(p_menu);

    /*!
     * Create partion group
     */
    _ui_video_v_creat_partition_group(p_menu);

    /*!
     * Create preview
     */
    _ui_video_v_creat_preview(p_menu);

    /*!
     * Create list and scrollbar
     */
    p_sbar = _ui_video_v_creat_sbar(p_menu);
    MT_ASSERT(p_sbar != NULL);

    _ui_video_v_creat_list(p_menu, p_sbar);
    ui_time_enable_heart_beat(TRUE);
    /*!
     * Create play container,and play info
     */
    p_play = _ui_video_v_creat_play_container(p_menu);
    MT_ASSERT(p_play != NULL);

    _ui_video_v_creat_play_file_name(p_play);
    _ui_video_v_creat_play_file_size(p_play);
    _ui_video_v_creat_play_file_pixel(p_play);
    _ui_video_v_creat_play_cur_time(p_play);
    _ui_video_v_creat_play_total_time(p_play);
    _ui_video_v_creat_play_icon(p_play);
    _ui_video_v_create_unsupport(p_menu);
ctrl_set_sts(p_play , OBJ_STS_HIDE);
    /*!
     * Show logo and set flag, it will be stopped before play video
     */
   // ui_show_logo(LOGO_BLOCK_ID_M1);
    preview_state = TRUE;

    ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

   // ui_stop_play(STOP_PLAY_BLACK, TRUE);
    
    _ui_video_v_init();

    ui_evt_enable_ir();
    ui_comm_dlg_close();
        manage_open_menu(ROOT_ID_FUN_HELP, 0, 0);
    return SUCCESS;
}
/*!
 * Video key and process
 */

static RET_CODE on_sort_list_select(control_t *p_sort_list, u16 msg, u32 para1,
                               u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_cont;
  control_t *p_mp_list;
  u16 pos;

  p_cont = p_sort_list->p_parent;
  p_mp_list = ctrl_get_child_by_id(ctrl_get_child_by_id(p_cont, IDC_VIDEO_V_LIST_CONT), IDC_VIDEO_V_LIST );

  ctrl_process_msg(p_sort_list, MSG_LOSTFOCUS, para1, para2);

  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);

  ctrl_paint_ctrl(p_cont, TRUE);

  pos = list_get_focus_pos(p_sort_list);
  switch(pos)
  {
    case 0:
        _ui_video_v_change_partition(p_mp_list,0,0,0);
        ctrl_process_msg(p_mp_list, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_mp_list, TRUE);
    return SUCCESS;
    case 1:
        _ui_video_v_fullscreen(p_mp_list,0,0,0);
        ctrl_process_msg(p_mp_list, MSG_GETFOCUS, 0, 0);
        ctrl_paint_ctrl(p_mp_list, TRUE);
    return SUCCESS;
    default:
      break;
  }

  return ret;
}

static RET_CODE on_video_sort_list_exit(control_t *p_sort_list, u16 msg, u32 para1,
                               u32 para2)
{
  RET_CODE ret = SUCCESS;
  control_t *p_cont;
  control_t *p_mp_list;

  p_cont = p_sort_list->p_parent;
  p_mp_list = ctrl_get_child_by_id(ctrl_get_child_by_id(p_cont, IDC_VIDEO_V_LIST_CONT), IDC_VIDEO_V_LIST );

  ctrl_process_msg(p_sort_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_mp_list, MSG_GETFOCUS, 0, 0);

  ctrl_set_sts(p_sort_list, OBJ_STS_HIDE);

  ctrl_paint_ctrl(p_cont, TRUE);

  return ret;
}

static RET_CODE on_video_list_sort(control_t *p_list, u16 msg,
  u32 para1, u32 para2)
{
  RET_CODE ret = SUCCESS;
  rect_t p_frame;
  control_t *p_cont, *p_sort_list;
  s16 pos=0;
  

  p_cont = p_list->p_parent;
  ui_comm_dlg_close();
  gui_stop_roll(p_list);
  pos = list_get_focus_pos(p_list);
  pos = pos > 10 ? 10 : pos;
  pos = pos < 1 ? 1 : pos;
  p_sort_list = ctrl_get_child_by_id(ctrl_get_parent(p_cont), IDC_VIDEO_V_SORT_LIST);
  ctrl_set_attr(p_sort_list, OBJ_ATTR_ACTIVE);
  p_frame.top = pos * 50;
  p_frame.bottom= pos * 50 + VIDEO_V_SORT_LIST_H;
  p_frame.left= VIDEO_V_SORT_LIST_X;
  p_frame.right= VIDEO_V_SORT_LIST_X + VIDEO_V_SORT_LIST_W;
  ctrl_resize(p_sort_list, &p_frame);
  ctrl_set_sts(p_sort_list, OBJ_STS_SHOW);

  ctrl_process_msg(p_list, MSG_LOSTFOCUS, para1, para2);
  ctrl_process_msg(p_sort_list, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_sort_list, TRUE);

  return ret;
}


BEGIN_KEYMAP(_ui_video_v_txt_keymap, NULL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
	ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
END_KEYMAP(_ui_video_v_txt_keymap, NULL)

BEGIN_MSGPROC(_ui_video_v_txt_proc, text_class_proc)
    ON_COMMAND(MSG_FOCUS_LEFT, _ui_video_v_menu_change_focus)
    ON_COMMAND(MSG_FOCUS_RIGHT, _ui_video_v_menu_change_focus)
    ON_COMMAND(MSG_FOCUS_UP, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_FOCUS_DOWN, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_VIDEO_EVENT_STOP_CFM, _ui_video_v_on_stop_cfm)
    ON_COMMAND(MSG_SELECT, _ui_video_change)
    ON_COMMAND(MSG_EXIT, _ui_video_v_exit)
END_MSGPROC(_ui_video_v_txt_proc, text_class_proc)

BEGIN_MSGPROC(_ui_video_v_list_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_FOCUS_DOWN, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_FOCUS_LEFT, _ui_video_v_menu_change_focus)
    ON_COMMAND(MSG_FOCUS_RIGHT, _ui_video_v_menu_change_focus)
    ON_COMMAND(MSG_PAGE_UP, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_PAGE_DOWN, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_SELECT, _ui_video_v_list_select)
    ON_COMMAND(MSG_EXIT, _ui_video_v_exit)
    ON_COMMAND(MSG_EXIT_ALL, _ui_video_v_exit_all)
    ON_COMMAND(MSG_VIDEO_V_PAUSE_PLAY, _ui_video_v_pause_resume)
    ON_COMMAND(MSG_VIDEO_V_STOP, _ui_video_v_stop)
    ON_COMMAND(MSG_VOLUME_DOWN, _ui_video_v_change_volume)
    ON_COMMAND(MSG_VOLUME_UP, _ui_video_v_change_volume)
    ON_COMMAND(MSG_VIDEO_V_CHANGE_PARTITION, on_video_list_sort)
   // ON_COMMAND(MSG_VIDEO_V_INFO, _ui_video_change)

    // msg up
    ON_COMMAND(MSG_VIDEO_V_FULLSCREEN, _ui_video_v_fullscreen)
    ON_COMMAND(MSG_PLUG_OUT, _ui_video_v_plug_out)
    ON_COMMAND(MSG_VIDEO_V_UPFOLDER, _ui_video_v_up_folder)
    ON_COMMAND(MSG_VIDEO_EVENT_UP_RESOLUTION, _ui_video_v_update_resolution)
    ON_COMMAND(MSG_VIDEO_EVENT_UP_TIME, _ui_video_v_update_cur_time)
    ON_COMMAND(MSG_VIDEO_EVENT_STOP_CFM, _ui_video_v_on_stop_cfm)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, _ui_video_v_on_load_media_success)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR, _ui_video_v_on_load_media_error)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT, _ui_video_v_on_load_media_error)
    ON_COMMAND(MSG_VIDEO_EVENT_EOS, _ui_video_v_on_eos)
    ON_COMMAND(MSG_VIDEO_EVENT_UNSUPPORTED_VIDEO, _ui_video_v_unsupported_video)
    ON_COMMAND(MSG_VIDEO_EVENT_FULLSCREEN_CLOSE, _ui_video_v_on_fullscreen_close)
END_MSGPROC(_ui_video_v_list_proc, list_class_proc)

BEGIN_KEYMAP(_ui_video_v_list_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)   
    ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DOWN)
    ON_EVENT(V_KEY_VUP, MSG_VOLUME_UP)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
      ON_EVENT(V_KEY_HOT_XEXTEND, MSG_VIDEO_V_CHANGE_PARTITION)
    ON_EVENT(V_KEY_PAUSE, MSG_VIDEO_V_PAUSE_PLAY)
    ON_EVENT(V_KEY_PLAY, MSG_VIDEO_V_PAUSE_PLAY)
    ON_EVENT(V_KEY_STOP, MSG_VIDEO_V_STOP)
    ON_EVENT(V_KEY_YELLOW, MSG_VIDEO_V_FULLSCREEN)
    ON_EVENT(V_KEY_RECALL, MSG_VIDEO_V_UPFOLDER)
    //ON_EVENT(V_KEY_GREEN, MSG_VIDEO_V_CHANGE_PARTITION)
    //ON_EVENT(V_KEY_INFO, MSG_VIDEO_V_INFO)
END_KEYMAP(_ui_video_v_list_keymap, NULL)

BEGIN_MSGPROC(_ui_video_network_v_list_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_FOCUS_DOWN, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_PAGE_UP, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_PAGE_DOWN, _ui_video_v_list_change_focus)
    ON_COMMAND(MSG_SELECT, _ui_video_v_list_select)
    ON_COMMAND(MSG_EXIT, _ui_video_v_exit)
    ON_COMMAND(MSG_EXIT_ALL, _ui_video_v_exit_all)
    ON_COMMAND(MSG_BACK_TO_NETWORK_PLACE, _ui_video_v_exit_to_network_place)
    ON_COMMAND(MSG_VOLUME_DOWN, _ui_video_v_change_volume)
    ON_COMMAND(MSG_VOLUME_UP, _ui_video_v_change_volume)
    //ON_COMMAND(MSG_VIDEO_V_INFO, _ui_video_change)
    ON_COMMAND(MSG_VIDEO_V_PAUSE_PLAY, _ui_video_v_pause_resume)

    // msg up
    ON_COMMAND(MSG_VIDEO_V_FULLSCREEN, _ui_video_v_fullscreen)
    ON_COMMAND(MSG_PLUG_OUT, _ui_video_v_plug_out)
    ON_COMMAND(MSG_VIDEO_V_UPFOLDER, _ui_video_v_up_folder)
    ON_COMMAND(MSG_VIDEO_EVENT_UP_RESOLUTION, _ui_video_v_update_resolution)
    ON_COMMAND(MSG_VIDEO_EVENT_UP_TIME, _ui_video_v_update_cur_time)
    ON_COMMAND(MSG_VIDEO_EVENT_STOP_CFM, _ui_video_v_on_stop_cfm)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_SUCCESS, _ui_video_v_on_load_media_success)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_ERROR, _ui_video_v_on_load_media_error)
    ON_COMMAND(MSG_VIDEO_EVENT_LOAD_MEDIA_EXIT, _ui_video_v_on_load_media_error)
    ON_COMMAND(MSG_VIDEO_EVENT_EOS, _ui_video_v_on_eos)
    ON_COMMAND(MSG_VIDEO_EVENT_UNSUPPORTED_MEMORY, _ui_video_v_unsupported_video)
    ON_COMMAND(MSG_VIDEO_EVENT_FULLSCREEN_CLOSE, _ui_video_v_on_fullscreen_close)
END_MSGPROC(_ui_video_network_v_list_proc, list_class_proc)

BEGIN_KEYMAP(_ui_video_network_v_list_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_LEFT, MSG_VOLUME_DOWN)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_RIGHT, MSG_VOLUME_UP)
    ON_EVENT(V_KEY_VDOWN, MSG_VOLUME_DOWN)
    ON_EVENT(V_KEY_VUP, MSG_VOLUME_UP)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_RECALL, MSG_VIDEO_V_UPFOLDER)
    ON_EVENT(V_KEY_INFO, MSG_VIDEO_V_INFO)
    ON_EVENT(V_KEY_GREEN, MSG_BACK_TO_NETWORK_PLACE)    
   ON_EVENT(V_KEY_PAUSE, MSG_VIDEO_V_PAUSE_PLAY)
   ON_EVENT(V_KEY_PLAY, MSG_VIDEO_V_PAUSE_PLAY)
END_KEYMAP(_ui_video_network_v_list_keymap, NULL)

BEGIN_KEYMAP(_ui_video_v_cont_keymap, NULL)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
END_KEYMAP(_ui_video_v_cont_keymap, NULL)

BEGIN_MSGPROC(_ui_video_v_cont_proc, ui_comm_root_proc)
 ON_COMMAND(MSG_EXIT, _ui_video_v_exit)
 ON_COMMAND(MSG_DESTROY, ui_video_v_destroy)
 ON_COMMAND(MSG_EXIT_ALL, _ui_video_v_exit_all)
END_MSGPROC(_ui_video_v_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(video_v_sort_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_HOT_XEXTEND, MSG_EXIT)
END_KEYMAP(video_v_sort_list_keymap, NULL)

BEGIN_MSGPROC(video_v_sort_list_proc, list_class_proc)
  ON_COMMAND(MSG_SELECT, on_sort_list_select)
  ON_COMMAND(MSG_EXIT, on_video_sort_list_exit)
END_MSGPROC(video_v_sort_list_proc, list_class_proc)
#endif
