/****************************************************************************

****************************************************************************/
#include "ui_common.h"
#include "commonData.h"

#include "ui_play_hist.h"
#include "db_play_hist.h"
#include "ui_iptv_prot.h"
#if 0
#include "sys_status.h"

#include "commonData.h"
#include "IIPTVDataProvider.h"
#include "ui_iptv_api.h"
#endif

enum playhist_local_msg
{
  MSG_SWITCH_CAPS = MSG_LOCAL_BEGIN + 200,
};

enum playhist_ctrl_id
{
    IDC_PLAYHIST_TITLE_CONT = 1,
    IDC_PLAYHIST_TITLE_NAME,
    IDC_PLAYHIST_VDO_NUM,

    IDC_PLAYHIST_LIST_CONT,
    IDC_PLAYHIST_LIST,
    IDC_PLAYHIST_SBAR,
    IDC_PLAYHIST_BUTTON,
};

typedef struct
{
    control_t *hList;
    control_t *hVdoNum;
    control_t *hButton;


} ui_playhist_app_t;

static list_xstyle_t playhist_list_item_rstyle =
{
  RSI_PBACK,
  RSI_PBACK,
  RSI_MEDIO_LEFT_LIST_HL,
  RSI_MEDIO_LEFT_LIST_HL,
  RSI_MEDIO_LEFT_LIST_HL,
};

static list_xstyle_t playhist_list_field_fstyle =
{
  FSI_GRAY,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
  FSI_WHITE,
};

static list_xstyle_t playhist_list_field_rstyle =
{
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
  RSI_IGNORE,
};

static list_field_attr_t playhist_list_attr[] =
{
  { LISTFIELD_TYPE_UNISTR | STL_RIGHT | STL_VCENTER,
    50, 15, 0, &playhist_list_field_rstyle,  &playhist_list_field_fstyle},
    
  { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
    300, 75, 0, &playhist_list_field_rstyle,  &playhist_list_field_fstyle},
};


static ui_playhist_app_t *g_pPlayHist;


static u16 playhist_cont_keymap(u16 key);
static RET_CODE playhist_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
static u16 playhist_list_keymap(u16 key);
static RET_CODE playhist_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
static u16 playhist_button_keymap(u16 key);
static RET_CODE playhist_button_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);

static BOOL ui_playhist_release_app_data(void);

static BOOL ui_playhist_init_app_data(void)
{
    if (g_pPlayHist)
    {
        ui_playhist_release_app_data();
    }

    g_pPlayHist = (ui_playhist_app_t *)mtos_malloc(sizeof(ui_playhist_app_t));
    MT_ASSERT(g_pPlayHist != NULL);
    memset((void *)g_pPlayHist, 0, sizeof(ui_playhist_app_t));

    return TRUE;
}

static BOOL ui_playhist_release_app_data(void)
{
    if (g_pPlayHist)
    {
        mtos_free(g_pPlayHist);
        g_pPlayHist = NULL;
    }
    
    return TRUE;
}

static RET_CODE ui_playhist_update_list(control_t *p_list, u16 start, u16 size, u32 context)
{
    play_hist_info info;
    u16 i, cnt;
    u8 asc_str[5+1];

    OS_PRINTF("@@@ui_playhist_update_list start=%d\n", start);
    cnt = list_get_count(p_list);

    for (i = start; i < start + size && i < cnt; i++)
    {
        memset(asc_str, 0, sizeof(asc_str));

        sprintf(asc_str, "%d.", (i + 1));
        list_set_field_content_by_ascstr(p_list, i, 0, asc_str);

        db_plht_get_item_by_idx(i, &info);
        list_set_field_content_by_unistr(p_list, i, 1, info.pg_name);
    }

    return SUCCESS;
}

static void ui_playhist_update_vdo_num(control_t* p_ctrl, u16 vdo_idx, u16 total_vdo)
{
    u16 *p_unistr;
    u16 vdo_num;
    u16 uni_str1[25+1];
    u16 uni_str2[15+1];
    u8  asc_str[15+1];

    OS_PRINTF("@@@ui_playhist_update_vdo_num\n");
    vdo_num = (total_vdo > 0) ? (vdo_idx + 1) : 0;

    memset(uni_str1, 0, sizeof(uni_str1));
    memset(uni_str2, 0, sizeof(uni_str2));
    memset(asc_str, 0, sizeof(asc_str));

    p_unistr = (u16 *)gui_get_string_addr(IDS_HD_RSC_NUM);
    uni_strcpy(uni_str1, p_unistr);

    sprintf(asc_str, ": %d / %d", vdo_num, total_vdo);
    str_asc2uni(asc_str, uni_str2);
    uni_strcat(uni_str1, uni_str2, 25);

    text_set_content_by_unistr(p_ctrl, uni_str1);
}

RET_CODE open_play_hist(u32 para1, u32 para2)
{
    control_t *p_cont;
    control_t *p_title_cont, *p_title_icon, *p_title_name, *p_vdo_num;
    control_t *p_list_cont, *p_list, *p_sbar, *p_button;
    u16 cnt;
    u8 i;

    ui_playhist_init_app_data();

    /*!
     * Create Menu
     */
    p_cont = fw_create_mainwin((u8)ROOT_ID_PLAY_HIST,
                            COMM_BG_X, COMM_BG_Y,
                            COMM_BG_W, COMM_BG_H,
                            (u8)ROOT_ID_INVALID, 0,
                            OBJ_ATTR_ACTIVE, 0);
    MT_ASSERT(p_cont != NULL);

    ctrl_set_keymap(p_cont, playhist_cont_keymap);
    ctrl_set_proc(p_cont, playhist_cont_proc);

    /*!
     * Create title container
     */
    p_title_cont = ctrl_create_ctrl(CTRL_CONT, IDC_PLAYHIST_TITLE_CONT, 
                                        PLAYHIST_TITLE_CONTX, PLAYHIST_TITLE_CONTY, 
                                        PLAYHIST_TITLE_CONTW, PLAYHIST_TITLE_CONTH, 
                                        p_cont, 0);
    ctrl_set_rstyle(p_title_cont, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);

    // title icon 
    p_title_icon = ctrl_create_ctrl(CTRL_BMAP, 0, 
                                        PLAYHIST_TITLE_ICON_X, PLAYHIST_TITLE_ICON_Y,
                                        PLAYHIST_TITLE_ICON_W, PLAYHIST_TITLE_ICON_H,
                                        p_title_cont, 0);
   // bmap_set_content_by_id(p_title_icon, IM_ICON_RECORD);

    // title name
    p_title_name = ctrl_create_ctrl(CTRL_TEXT, IDC_PLAYHIST_TITLE_NAME, 
                                          PLAYHIST_TITLE_NAME_X, PLAYHIST_TITLE_NAME_Y, 
                                          PLAYHIST_TITLE_NAME_W, PLAYHIST_TITLE_NAME_H, 
                                          p_title_cont, 0);
    ctrl_set_rstyle(p_title_name, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_title_name, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_title_name, FSI_WHITE_28, FSI_WHITE_28, FSI_WHITE_28);
    text_set_content_type(p_title_name, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_title_name, IDS_HD_PLAY_HISTORY);

    // video number
    p_vdo_num = ctrl_create_ctrl(CTRL_TEXT, IDC_PLAYHIST_VDO_NUM, 
                                        PLAYHIST_VDO_NUM_X, PLAYHIST_VDO_NUM_Y, 
                                        PLAYHIST_VDO_NUM_W, PLAYHIST_VDO_NUM_H, 
                                        p_title_cont, 0);
    ctrl_set_rstyle(p_vdo_num, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_vdo_num, STL_RIGHT | STL_VCENTER);
    text_set_font_style(p_vdo_num, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_content_type(p_vdo_num, TEXT_STRTYPE_UNICODE);
    //text_set_content_by_unistr(p_vdo_num, uni_str);

    //button
    p_button = ctrl_create_ctrl(CTRL_TEXT, IDC_PLAYHIST_BUTTON,
                                    PLAYHIST_BUTTON_X, PLAYHIST_BUTTON_Y,
                                    PLAYHIST_BUTTON_W, PLAYHIST_BUTTON_H,
                                    p_title_cont, 0);
    ctrl_set_rstyle(p_button, RSI_OTT_BUTTON_SH, RSI_OTT_BUTTON_HL, RSI_OTT_BUTTON_SH); //color and font color
    ctrl_set_keymap(p_button, playhist_button_keymap);
    ctrl_set_proc(p_button, playhist_button_proc);
    text_set_align_type(p_button, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_button, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_content_type(p_button, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_button,  IDS_HD_EMPTY);

    /*!
     * Create list container
     */
    p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_PLAYHIST_LIST_CONT, 
                                      PLAYHIST_LIST_CONTX, PLAYHIST_LIST_CONTY, 
                                      PLAYHIST_LIST_CONTW, PLAYHIST_LIST_CONTH, 
                                      p_cont, 0);
    ctrl_set_rstyle(p_list_cont, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);

    // list
    p_list = ctrl_create_ctrl(CTRL_LIST, IDC_PLAYHIST_LIST, 
                              PLAYHIST_LIST_X, PLAYHIST_LIST_Y, 
                              PLAYHIST_LIST_W, PLAYHIST_LIST_H, 
                              p_list_cont, 0);
    ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_list, playhist_list_keymap);
    ctrl_set_proc(p_list, playhist_list_proc);

    list_set_item_interval(p_list, PLAYHIST_LIST_ITEM_VGAP);
    list_set_item_rstyle(p_list, &playhist_list_item_rstyle);

    list_enable_cycle_mode(p_list, FALSE);
    list_enable_select_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);

    cnt = db_plht_get_count();
    list_set_count(p_list, cnt, PLAYHIST_LIST_PAGE);
    list_set_field_count(p_list, ARRAY_SIZE(playhist_list_attr), PLAYHIST_LIST_PAGE);
    list_set_focus_pos(p_list, 0);
    list_set_update(p_list, ui_playhist_update_list, 0);

    for (i = 0; i < ARRAY_SIZE(playhist_list_attr); i++)
    {
        list_set_field_attr(p_list, (u8)i, (u32)(playhist_list_attr[i].attr), 
                          (u16)(playhist_list_attr[i].width), 
                          (u16)(playhist_list_attr[i].left), 
                          (u8)(playhist_list_attr[i].top));
        list_set_field_rect_style(p_list, (u8)i, playhist_list_attr[i].rstyle);
        list_set_field_font_style(p_list, (u8)i, playhist_list_attr[i].fstyle);
    }
    ui_playhist_update_list(p_list, list_get_valid_pos(p_list), PLAYHIST_LIST_PAGE, 0);
    ui_playhist_update_vdo_num(p_vdo_num, 0, cnt);

    //sbar
    p_sbar = ctrl_create_ctrl(CTRL_SBAR, IDC_PLAYHIST_SBAR, 
                                PLAYHIST_SBAR_X, PLAYHIST_SBAR_Y, 
                                PLAYHIST_SBAR_W, PLAYHIST_SBAR_H, 
                                p_list_cont, 0);
    ctrl_set_rstyle(p_sbar, RSI_OTT_SCROLL_BAR_BG, RSI_OTT_SCROLL_BAR_BG, RSI_OTT_SCROLL_BAR_BG);
    sbar_set_autosize_mode(p_sbar, TRUE);
    sbar_set_direction(p_sbar, FALSE);
    sbar_set_mid_rstyle(p_sbar, RSI_OTT_SCROLL_BAR, RSI_OTT_SCROLL_BAR, RSI_OTT_SCROLL_BAR);
    list_set_scrollbar(p_list, p_sbar);

#ifdef SPT_DUMP_DATA
    spt_dump_menu_data(p_cont);
#endif

    g_pPlayHist->hList = p_list;
    g_pPlayHist->hVdoNum = p_vdo_num;
    g_pPlayHist->hButton = p_button;

    ui_iptv_set_flag_to_plid(FALSE);
    ctrl_default_proc(p_list, MSG_GETFOCUS, 0, 0); 
    ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);

    return SUCCESS;
}

static RET_CODE ui_playhist_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;

    OS_PRINTF("@@@ui_playhist_on_destory\n");
    ui_playhist_release_app_data();
    ui_iptv_set_flag_to_plid(TRUE);

    p_root = fw_find_root_by_id(ROOT_ID_IPTV);
    if (p_root != NULL)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
    }
    return ERR_NOFEATURE;
}

static RET_CODE ui_playhist_on_update(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 cnt;

    OS_PRINTF("@@@ui_playhist_on_update\n");
    cnt = db_plht_get_count();

    list_set_count(g_pPlayHist->hList, cnt, PLAYHIST_LIST_PAGE);
    list_set_focus_pos(g_pPlayHist->hList, 0);
    ui_playhist_update_list(g_pPlayHist->hList, list_get_valid_pos(g_pPlayHist->hList), PLAYHIST_LIST_PAGE, 0);

    ui_playhist_update_vdo_num(g_pPlayHist->hVdoNum, 0, cnt);

    ctrl_paint_ctrl(g_pPlayHist->hList, TRUE);
    ctrl_paint_ctrl(g_pPlayHist->hVdoNum, TRUE);

    return SUCCESS;
}

static RET_CODE ui_playhist_on_list_updown(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  	
    u16 cnt;

    cnt = list_get_count(p_ctrl);

    switch (msg)
    {
        case MSG_FOCUS_UP:
            if (list_get_focus_pos(p_ctrl) > 0)
            {
                list_class_proc(p_ctrl, msg, para1, para2);

                ui_playhist_update_vdo_num(g_pPlayHist->hVdoNum, list_get_focus_pos(p_ctrl), cnt);
                ctrl_paint_ctrl(g_pPlayHist->hVdoNum, TRUE);
            }
            else
            {
                ctrl_change_focus(g_pPlayHist->hList, g_pPlayHist->hButton);
            }
            break;

        case MSG_FOCUS_DOWN:
            list_class_proc(p_ctrl, msg, para1, para2);

            ui_playhist_update_vdo_num(g_pPlayHist->hVdoNum, list_get_focus_pos(p_ctrl), cnt);
            ctrl_paint_ctrl(g_pPlayHist->hVdoNum, TRUE);
            break;

        default:
            break;
    }

    return SUCCESS;
}

static RET_CODE ui_playhist_on_list_pageupdown(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    list_class_proc(p_ctrl, msg, para1, para2);

    ui_playhist_update_vdo_num(g_pPlayHist->hVdoNum, list_get_focus_pos(p_ctrl), list_get_count(p_ctrl));
    ctrl_paint_ctrl(g_pPlayHist->hVdoNum, TRUE);

    return SUCCESS;
}

static RET_CODE ui_playhist_on_list_selected(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    play_hist_info info;
    ui_iptv_description_param_t param;
    u16 pos, cnt;

    cnt = list_get_count(g_pPlayHist->hList);
    if (cnt == 0)
    {
        return SUCCESS;
    }

    pos = list_get_focus_pos(p_ctrl);
    db_plht_get_item_by_idx(pos, &info);

    OS_PRINTF("@@@ui_playhist_on_list_selected pos=%d, vdo_id=%d, res_id=%d, b_single_page=%d\n", pos, info.vdo_id, info.res_id, info.b_single_page);
    // TODO: play history video id  SY
    //param.vdo_id = info.vdo_id;
    memcpy(param.vdo_id.qpId, info.vdo_id.qpId, strlen(info.vdo_id.qpId)+1);
    memcpy(param.vdo_id.tvQid, info.vdo_id.tvQid, strlen(info.vdo_id.tvQid)+1);
    memcpy(param.vdo_id.sourceCode, info.vdo_id.sourceCode, strlen(info.vdo_id.sourceCode)+1);
    param.vdo_id.type = info.vdo_id.type;
    param.res_id = info.res_id;
    param.b_single_page = info.vdo_id.type;
    manage_open_menu(ROOT_ID_IPTV_DESCRIPTION, (u32)(&param), (u32)ROOT_ID_PLAY_HIST);

    return SUCCESS;
}

static RET_CODE ui_playhist_on_button_down(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  	
    ctrl_change_focus(g_pPlayHist->hButton, g_pPlayHist->hList);
    return SUCCESS;
}

static RET_CODE ui_playhist_on_button_selected(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{  	
    u32 cnt;

    cnt = list_get_count(g_pPlayHist->hList);
    if (cnt == 0)
    {
        return SUCCESS;
    }

    db_plht_del_all();

    list_set_count(g_pPlayHist->hList, 0, PLAYHIST_LIST_PAGE);
    list_set_focus_pos(g_pPlayHist->hList, 0);
    ui_playhist_update_list(g_pPlayHist->hList, list_get_valid_pos(g_pPlayHist->hList), PLAYHIST_LIST_PAGE, 0);

    ui_playhist_update_vdo_num(g_pPlayHist->hVdoNum, 0, 0);

    ctrl_paint_ctrl(g_pPlayHist->hList, TRUE);
    ctrl_paint_ctrl(g_pPlayHist->hVdoNum, TRUE);

    return SUCCESS;
}

BEGIN_KEYMAP(playhist_cont_keymap, ui_comm_root_keymap)
    ON_EVENT(V_KEY_INFO, MSG_INFO)
END_KEYMAP(playhist_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(playhist_cont_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_DESTROY, ui_playhist_on_destory)
    ON_COMMAND(MSG_UPDATE, ui_playhist_on_update)
END_MSGPROC(playhist_cont_proc, ui_comm_root_proc)

BEGIN_KEYMAP(playhist_list_keymap, NULL)
	ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
	ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
	ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
	ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
	ON_EVENT(V_KEY_OK, MSG_YES)
END_KEYMAP(playhist_list_keymap, NULL)

BEGIN_MSGPROC(playhist_list_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_UP, ui_playhist_on_list_updown)
    ON_COMMAND(MSG_FOCUS_DOWN, ui_playhist_on_list_updown)
    ON_COMMAND(MSG_PAGE_UP, ui_playhist_on_list_pageupdown)
    ON_COMMAND(MSG_PAGE_DOWN, ui_playhist_on_list_pageupdown)
    ON_COMMAND(MSG_YES, ui_playhist_on_list_selected)
END_MSGPROC(playhist_list_proc, list_class_proc)

BEGIN_KEYMAP(playhist_button_keymap, NULL)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    //ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(playhist_button_keymap, NULL)

BEGIN_MSGPROC(playhist_button_proc, ctrl_default_proc)
    ON_COMMAND(MSG_FOCUS_DOWN, ui_playhist_on_button_down)
    ON_COMMAND(MSG_SELECT, ui_playhist_on_button_selected)
END_MSGPROC(playhist_button_proc, ctrl_default_proc)


