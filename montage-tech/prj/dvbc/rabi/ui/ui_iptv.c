/******************************************************************************/
/******************************************************************************/

/*!
 Include files
 */
#include "ui_common.h"
//#include "lib_char.h"
#if ENABLE_NETWORK
#include "commonData.h"
#include "redxDataProvider.h"
#include "IIPTVDataProvider.h"
#include "ui_iptv_api.h"

#include "ui_iptv_gprot.h"
#include "ui_iptv_prot.h"

#include "SM_StateMachine.h"
#include "States_Iptv.h"
#include "ui_picture_api.h"
#include "ui_play_api.h"
#include "ui_video_iqy.h"
#include "ui_iptv_description.h"


//#include "authorization.h"
/*!
 * Const value
 */
// Catlist container
#define IPTV_CATLIST_CONTX      (0)//(60)
#define IPTV_CATLIST_CONTY      (560)//(60)
#define IPTV_CATLIST_CONTW      (1280)//(220)
#define IPTV_CATLIST_CONTH      (60)

// Title icon
//#define IPTV_TITLE_ICON_X         (20)//(17)//(27)
//#define IPTV_TITLE_ICON_Y         (18)//(16)
//#define IPTV_TITLE_ICON_W         (54)//(80)
//#define IPTV_TITLE_ICON_H         (54)//(80)

// Title name
#define IPTV_TITLE_NAME_X         (60)//(IPTV_TITLE_ICON_X + IPTV_TITLE_ICON_W + 4)//(118)
#define IPTV_TITLE_NAME_Y         ((IPTV_PAGE_CONTH - IPTV_TITLE_NAME_H)>>1)//(IPTV_TITLE_ICON_Y)
#define IPTV_TITLE_NAME_W         (100)//(IPTV_CATLIST_CONTW - IPTV_TITLE_NAME_X - IPTV_TITLE_ICON_X)//(100)
#define IPTV_TITLE_NAME_H         (64)//(IPTV_TITLE_ICON_H)

//add filter
#define IPTV_FILTER_X     (0)
#define IPTV_FILTER_W     (COMM_BG_W)
#define IPTV_FILTER_GAP   (20)

#define IPTV_FILTER_TEXT_X     (0)
#define IPTV_FILTER_TEXT_Y     (0)
#define IPTV_FILTER_TEXT_W     (169)
#define IPTV_FILTER_TEXT_H     (50)

#define IPTV_FILTER_LIST_X     (IPTV_FILTER_TEXT_W + IPTV_FILTER_GAP)
#define IPTV_FILTER_LIST_Y     (IPTV_FILTER_TEXT_Y)
#define IPTV_FILTER_LIST_W     (COMM_BG_W - IPTV_FILTER_LIST_X - 91)
#define IPTV_FILTER_LIST_H     (IPTV_FILTER_TEXT_H)

#define IPTV_FILTER_LIST_FIELD  (1)
#define IPTV_FILTER_LIST_PAGE   (8)
#define IPTV_FILTER_LIST_COUNT  (12)

// Catlist
#define IPTV_CATLIST_X                (60)//(5)
#define IPTV_CATLIST_Y                (0)
#define IPTV_CATLIST_W                (1160)
#define IPTV_CATLIST_H                (60)//(IPTV_CATLIST_CONTH - IPTV_CATLIST_Y - IPTV_TITLE_ICON_Y)

#define IPTV_CATLIST_MIDL           (0)
#define IPTV_CATLIST_MIDT           (0)
#define IPTV_CATLIST_MIDR           (IPTV_CATLIST_MIDL + IPTV_CATLIST_W)
#define IPTV_CATLIST_MIDB           (IPTV_CATLIST_MIDT + IPTV_CATLIST_H)

#define IPTV_CATLIST_ITEM_V_GAP (2)//(4)

#define IPTV_CATLIST_PAGE_SIZE    (11)//(13)

#define IPTV_CATLIST_FIELD_X                (0)
#define IPTV_CATLIST_FIELD_Y                (0)
#define IPTV_CATLIST_FIELD_W                (100)

// Reslist container
#define IPTV_RESLIST_CONTX             (0)//(IPTV_RESLIST_CONTX)
#define IPTV_RESLIST_CONTY             (620)//(IPTV_RESLIST_CONTY + IPTV_RESLIST_CONTH)
#define IPTV_RESLIST_CONTW             (1280)//(930)//(IPTV_RESLIST_CONTW)
#define IPTV_RESLIST_CONTH             (100)//(IPTV_CATLIST_CONTH - IPTV_RESLIST_CONTH)

#define IPTV_RESLIST_X              (60)//(75)//(83)
#define IPTV_RESLIST_Y              (5)//(0)
#define IPTV_RESLIST_W              (1160)// (776+56+48)//(776)
#define IPTV_RESLIST_H              (80)//(60)

#define IPTV_RESLIST_MIDL           (0)
#define IPTV_RESLIST_MIDT           (0)
#define IPTV_RESLIST_MIDR           (IPTV_RESLIST_MIDL + IPTV_RESLIST_W)
#define IPTV_RESLIST_MIDB           (IPTV_RESLIST_MIDT + IPTV_RESLIST_H)

//#define IPTV_RESLIST_ITEM_H_GAP (7)

#define IPTV_RESLIST_PAGE_SIZE  (9)

#define IPTV_RESLIST_FIELD_X                (0)
#define IPTV_RESLIST_FIELD_Y                (0)
#define IPTV_RESLIST_FIELD_W                (124)//(97)

// Client container
#define IPTV_CLIENT_CONTX             (0)//(IPTV_RESLIST_CONTX)
#define IPTV_CLIENT_CONTY             (60)//(IPTV_RESLIST_CONTY + IPTV_RESLIST_CONTH)
#define IPTV_CLIENT_CONTW             (1280)//(930)//(IPTV_RESLIST_CONTW)
#define IPTV_CLIENT_CONTH             (490)//(IPTV_CATLIST_CONTH - IPTV_RESLIST_CONTH)
// Video list
#define IPTV_VDOLIST_X                  (100)//(28)//(45)
#define IPTV_VDOLIST_Y                  (IPTV_VDOLIST_ITEM_V_GAP)//(27)
#define IPTV_VDOLIST_W                  (1280 - 2*IPTV_VDOLIST_X)
#define IPTV_VDOLIST_H                  (IPTV_VDOLIST_ITEM_H + IPTV_VDOLIST_ITEM_H + IPTV_VDOLIST_ITEM_V_GAP)//(460)

#define IPTV_VDOLIST_ITEM_X         (0)
#define IPTV_VDOLIST_ITEM_Y         (0)
#define IPTV_VDOLIST_ITEM_W         (139)
#define IPTV_VDOLIST_ITEM_H         (220)

#define IPTV_VDOLIST_ITEM_H_GAP (49)//(20)
#define IPTV_VDOLIST_ITEM_V_GAP (16)

#define IPTV_VDOPIC_X                   (0)
#define IPTV_VDOPIC_Y                   (0)
#define IPTV_VDOPIC_W                   (IPTV_VDOLIST_ITEM_W)//(150)
#define IPTV_VDOPIC_H                   (180)

#define IPTV_VDOPIC_LEFT_X                   (50)
#define IPTV_VDOPIC_LEFT_Y                   (168)
#define IPTV_VDOPIC_LEFT_W                   (41)//(150)
#define IPTV_VDOPIC_LEFT_H                   (274)

#define IPTV_VDOPIC_RIGHT_X                   (1280 - IPTV_VDOPIC_LEFT_X - IPTV_VDOPIC_RIGHT_W)
#define IPTV_VDOPIC_RIGHT_Y                   (168)
#define IPTV_VDOPIC_RIGHT_W                   (41)//(150)
#define IPTV_VDOPIC_RIGHT_H                   (274)

#define IPTV_VDOPIC_LINE_X                   (0)
#define IPTV_VDOPIC_LINE_Y                   (550)
#define IPTV_VDOPIC_LINE_W                   (1280)//(150)
#define IPTV_VDOPIC_LINE_H                   (10)

#define IPTV_VDOPIC_MIDL                   (4)
#define IPTV_VDOPIC_MIDT                   (4)
#define IPTV_VDOPIC_MIDR                   (IPTV_VDOPIC_W - IPTV_VDOPIC_MIDL)
#define IPTV_VDOPIC_MIDB                   (IPTV_VDOPIC_H - IPTV_VDOPIC_MIDT)

#define IPTV_VDONAME_X                (IPTV_VDOPIC_X)
#define IPTV_VDONAME_Y                (IPTV_VDOPIC_Y + IPTV_VDOPIC_H)
#define IPTV_VDONAME_W                (IPTV_VDOPIC_W)
#define IPTV_VDONAME_H                (40)

#define IPTV_VDONAME_MIDL                   (5)
#define IPTV_VDONAME_MIDT                   (5)
#define IPTV_VDONAME_MIDR                   (IPTV_VDONAME_W - IPTV_VDONAME_MIDL)
#define IPTV_VDONAME_MIDB                   (IPTV_VDONAME_H - IPTV_VDONAME_MIDT)

#define IPTV_VDOLIST_PAGE_SIZE    (12)
#define IPTV_VDOLIST_ITEM_ROW     (2)
#define IPTV_VDOLIST_ITEM_COL     (6)


//Page container
#define IPTV_PAGE_CONTX    (0)
#define IPTV_PAGE_CONTY    (0)
#define IPTV_PAGE_CONTW    (IPTV_RESLIST_CONTW)
#define IPTV_PAGE_CONTH    (80)

//Page number
#define IPTV_PAGE_NUMBER_X    ((IPTV_PAGE_CONTW - IPTV_PAGE_NUMBER_W) >> 1)
#define IPTV_PAGE_NUMBER_Y    ((IPTV_PAGE_CONTH - IPTV_PAGE_NUMBER_H) >> 1)
#define IPTV_PAGE_NUMBER_W    (150)
#define IPTV_PAGE_NUMBER_H    (40)

/*!
 * Macro
 */

/*!
 * Type define
 */

enum iptv_catlist_extern
{
	IPTV_CATLIST_SEARCH =0,
	IPTV_CATLIST_FILTER =1,
	IPTV_CATLIST_ALL 	=2,
	IPTV_CATLIST_EXTERN =3,
};

// Video view control id
typedef enum
{
    IDC_IPTV_RESLIST_CONT = 1,
    IDC_IPTV_RESLIST,

    IDC_IPTV_CATLIST_CONT,
    IDC_IPTV_TITLE_NAME,
    IDC_IPTV_CATLIST,

    IDC_IPTV_CLIENT_CONT,
    IDC_IPTV_VDOLIST,
    IDC_IPTV_VDOITEM_START,
    IDC_IPTV_VDOITEM_END = IDC_IPTV_VDOITEM_START + IPTV_VDOLIST_PAGE_SIZE - 1,
    IDC_IPTV_VDOITEM_PIC,
    IDC_IPTV_VDOITEM_PIC_LEFT,
    IDC_IPTV_VDOITEM_PIC_RIGHT,
    IDC_IPTV_VDOITEM_PIC_LINE,
    IDC_IPTV_VDOITEM_NAME,

    IDC_IPTV_PAGE_CONT,
    IDC_IPTV_PAGE_NUM,
    IDC_IPTV_FILTER_LIST,
    IDC_IPTV_FILTER_TEXT,
    IDC_IPTV_FILTER_CONT,
    IDC_IPTV_FILTER_START,
} ui_iptv_ctrl_id_t;

typedef enum 
{
    PAGER_KEY_TYPE_NONE,
    PAGER_KEY_TYPE_PREV_PAGE,  //page navigation
    PAGER_KEY_TYPE_NEXT_PAGE,
    PAGER_KEY_TYPE_PAGE_NUM,

    PAGER_KEY_TYPE_MAX_COUNT
} ui_pager_key_type_t;

typedef struct
{
    u32 res_id;
    u16 *name;

    u16 total_cat;
    u16 total_filter;
    al_iptv_cat_item_t *catList;
    al_iptv_filter_item_t *filter;
} ui_iptv_res_item_t;

typedef enum
{
    IPTV_FLAG_CAT = 0,
    IPTV_FLAG_FILTER,
}ui_iptv_cat_filter_flag_t;

typedef struct
{
    control_t *hResList;
    control_t *hCatList;
    control_t *hVdoList;
    control_t *hTitleName;
    control_t *hPageNum;
    control_t *hFilter;

    ui_iptv_res_item_t *resList;
    u16 total_res;
    u16 curResListIdx;

    u16 **mainMenuName;
    u16 totalMainMenu;

    al_iptv_cat_item_t *catList;
    u16 total_cat;
    u16 curCatListIdx;
    ui_iptv_cat_filter_flag_t catFilter_flag;

    al_iptv_vdo_item_t *vdoList;
    u32 total_vdo;//
    u16 vdo_cnt;
    u16 curVdoListIdx;
    u16 curVdoListRendIdx;

    u16 vdo_page_size;

    u32 total_page;     // total video page
    u32 curPageNum;   // video page num
    u32 jumpPageNum;
    BOOL b_totalPageChanged;

} ui_iptv_app_t;

/*!
 * Function define
 */ 
static control_t *create_iptv_filter(control_t *ctrl);
extern RET_CODE switch_to_play_prog(void);
u16 iptv_reslist_keymap(u16 key);
RET_CODE iptv_reslist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 iptv_catlist_keymap(u16 key);
RET_CODE iptv_catlist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 iptv_vdolist_keymap(u16 key);
RET_CODE iptv_vdolist_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 iptv_page_num_keymap(u16 key);
RET_CODE iptv_page_num_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 iptv_cont_keymap(u16 key);
RET_CODE iptv_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
u16 iptv_filter_filter_keymap(u16 key);
RET_CODE iptv_filter_filter_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);

static RET_CODE ui_iptv_vdolist_change_focus_pos(control_t *p_vdolist, u16 focus, BOOL is_paint);

static BOOL ui_release_app_data(void);

/*!
 * Priv data
 */
// Category list style
static list_xstyle_t catlist_item_rstyle =
{
    RSI_VOD_LIST2_N,
    RSI_VOD_LIST2_N,
    RSI_VOD_LIST2_F,
    RSI_VOD_LIST2_S,
    RSI_VOD_LIST2_F,
};
static list_xstyle_t catlist_field_rstyle =
{
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
};
static list_xstyle_t catlist_field_fstyle =
{
    FSI_GRAY,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
};
static const list_field_attr_t catlist_attr[] =
{
    {
        LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
        IPTV_CATLIST_FIELD_W,
        IPTV_CATLIST_FIELD_X,
        IPTV_CATLIST_FIELD_Y,
        &catlist_field_rstyle,
        &catlist_field_fstyle
    }
};

// Resource list style
static list_xstyle_t reslist_item_rstyle =
{
    RSI_VOD_LIST_N,
    RSI_VOD_LIST_N,
    RSI_VOD_LIST_F,
    RSI_VOD_LIST_S,
    RSI_VOD_LIST_F,
};

static list_xstyle_t reslist_field_rstyle =
{
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
};
static list_xstyle_t reslist_field_fstyle =
{
    FSI_GRAY_28,
    FSI_WHITE_28,
    FSI_WHITE_28,
    FSI_WHITE_28,
    FSI_WHITE_28,
};
static const list_field_attr_t reslist_attr[] =
{
    {
        LISTFIELD_TYPE_EXTSTR | STL_CENTER | STL_VCENTER,
        IPTV_RESLIST_FIELD_W,
        IPTV_RESLIST_FIELD_X,
        IPTV_RESLIST_FIELD_Y,
        &reslist_field_rstyle,
        &reslist_field_fstyle
    }
};

// filter list style
static list_xstyle_t filterlist_item_rstyle =
{
    RSI_VOD_LIST2_N,
    RSI_VOD_LIST2_N,
    RSI_VOD_LIST2_F,
    RSI_VOD_LIST2_S,
    RSI_VOD_LIST2_F,
};
static list_xstyle_t filterlist_field_rstyle =
{
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
    RSI_IGNORE,
};
static list_xstyle_t filterlist_field_fstyle =
{
    FSI_GRAY,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
    FSI_WHITE,
};
static const list_field_attr_t filterlist_attr[] =
{
    {
	LISTFIELD_TYPE_UNISTR | STL_CENTER | STL_VCENTER,
        125,
        0,
        0,
        &filterlist_field_rstyle,
        &filterlist_field_fstyle
    }
};

// App data
static ui_iptv_app_t *g_pIptv = NULL;


/*================================================================================================
                           iptv internel function
 ================================================================================================*/

static BOOL ui_init_app_data(void)
{
    if (g_pIptv)
    {
        ui_release_app_data();
    }

    g_pIptv = (ui_iptv_app_t *)SY_MALLOC(sizeof(ui_iptv_app_t));
    MT_ASSERT(g_pIptv != NULL);
    memset((void *)g_pIptv, 0, sizeof(ui_iptv_app_t));

    return TRUE;
}

static void ui_release_vdolist(void)
{
    u16 i;

    if (g_pIptv->vdoList)
    {
        for (i = 0; i < g_pIptv->vdo_cnt; i++)
        {
            if (g_pIptv->vdoList[i].name)
            {
                SY_FREE(g_pIptv->vdoList[i].name);
            }
            if (g_pIptv->vdoList[i].img_url)
            {
                SY_FREE(g_pIptv->vdoList[i].img_url);
            }
        }
        SY_FREE(g_pIptv->vdoList);
        g_pIptv->vdoList = NULL;
    }
}

static void ui_release_rescatgry_catlist(u16 num)
{
    u16 j;

    if (g_pIptv->resList[num].catList)
    {
        for (j = 0; j < g_pIptv->resList[num].total_cat; j++)
        {
            if (g_pIptv->resList[num].catList[j].name)
            {
                SY_FREE(g_pIptv->resList[num].catList[j].name);
            }
            if (g_pIptv->resList[num].catList[j].key)
            {
                SY_FREE(g_pIptv->resList[num].catList[j].key);
            }
        }
        SY_FREE(g_pIptv->resList[num].catList);
    }
            
}

static void ui_release_rescatgry_filter(u16 num)
{
    u16 j,k;
      
    if (g_pIptv->resList[num].filter)
    {
        for (j = 0; j < g_pIptv->resList[num].total_filter; j++)
        {
            if (g_pIptv->resList[num].filter[j].name)
            {
                SY_FREE(g_pIptv->resList[num].filter[j].name);
            }

            if(g_pIptv->resList[num].filter[j].subtag)
            {
                for(k=0; k<g_pIptv->resList[num].filter[j].subTagSize; k++)
                {
                    if(g_pIptv->resList[num].filter[j].subtag[k].name)
                    {
                	        SY_FREE(g_pIptv->resList[num].filter[j].subtag[k].name);
                    }
                    if(g_pIptv->resList[num].filter[j].subtag[k].key)
                    {
                	        SY_FREE(g_pIptv->resList[num].filter[j].subtag[k].key);
                    }
                }
                SY_FREE(g_pIptv->resList[num].filter[j].subtag);
            }
        }
    }
}

static BOOL ui_release_app_data(void)
{
    u16 i, j,k;

    if (g_pIptv)
    {
        if (g_pIptv->resList)
        {
            for (i = 0; i < g_pIptv->total_res; i++)
            {
                if (g_pIptv->resList[i].name)
                {
                    SY_FREE(g_pIptv->resList[i].name);
                }
                if (g_pIptv->resList[i].catList)
                {
                    for (j = 0; j < g_pIptv->resList[i].total_cat; j++)
                    {
                        if (g_pIptv->resList[i].catList[j].name)
                        {
                            SY_FREE(g_pIptv->resList[i].catList[j].name);
                        }
                        if (g_pIptv->resList[i].catList[j].key)
                        {
                            SY_FREE(g_pIptv->resList[i].catList[j].key);
                        }
                    }
                    SY_FREE(g_pIptv->resList[i].catList);
                }
                if (g_pIptv->resList[i].filter)
                {
                    for (j = 0; j < g_pIptv->resList[i].total_filter; j++)
                    {
                        if (g_pIptv->resList[i].filter[j].name)
                        {
                            SY_FREE(g_pIptv->resList[i].filter[j].name);
                        }
                        if(g_pIptv->resList[i].filter[j].subtag)
                        {
                            for(k=0; k<g_pIptv->resList[i].filter[j].subTagSize; k++)
                            {
                                if(g_pIptv->resList[i].filter[j].subtag[k].name)
                                {
                            	        SY_FREE(g_pIptv->resList[i].filter[j].subtag[k].name);
                                }
                                if(g_pIptv->resList[i].filter[j].subtag[k].key)
                                {
                            	        SY_FREE(g_pIptv->resList[i].filter[j].subtag[k].key);
                                }
                            }
                            SY_FREE(g_pIptv->resList[i].filter[j].subtag);
                         }
                    }
                    SY_FREE(g_pIptv->resList[i].filter);
                }
            }
            SY_FREE(g_pIptv->resList);
        }

        if (g_pIptv->mainMenuName)
        {
            SY_FREE(g_pIptv->mainMenuName);
        }

        ui_release_vdolist();

        SY_FREE(g_pIptv);
        g_pIptv = NULL;
    }
    
    return TRUE;
}

static void ui_iptv_pic_init(void)
{
  ui_pic_init(PIC_SOURCE_NET);
}

static void ui_iptv_pic_deinit(void)
{
  pic_stop();
  ui_pic_release();
}

static RET_CODE ui_iptv_pic_play_start()
{
    control_t *p_item, *p_pic;
    rect_t rect;

    DEBUG(UI_IPTV,INFO,"@@@%s curVdoListRendIdx=%d, vdo_cnt=%d\n", __FUNCTION__, g_pIptv->curVdoListRendIdx, g_pIptv->vdo_cnt);
    if (g_pIptv->vdoList)
    {
        while (g_pIptv->curVdoListRendIdx < g_pIptv->vdo_cnt)
        {
            if (g_pIptv->vdoList[g_pIptv->curVdoListRendIdx].img_url 
                && strlen(g_pIptv->vdoList[g_pIptv->curVdoListRendIdx].img_url) > 0)
            {
                p_item = ctrl_get_child_by_id(g_pIptv->hVdoList, IDC_IPTV_VDOITEM_START + g_pIptv->curVdoListRendIdx);
                p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_VDOITEM_PIC);

                ctrl_get_frame(p_pic, &rect);
                ctrl_client2screen(p_pic, &rect);

                ui_pic_play_by_url(g_pIptv->vdoList[g_pIptv->curVdoListRendIdx].img_url, &rect, 0);
                return SUCCESS;
            }
            g_pIptv->curVdoListRendIdx++;
        }
	if(g_pIptv->hFilter != NULL)
	{
		if(ctrl_get_sts(g_pIptv->hFilter) == OBJ_STS_SHOW)
			ctrl_paint_ctrl(g_pIptv->hFilter,TRUE);
	}
    }
    return ERR_NOFEATURE;
}

static RET_CODE ui_iptv_update_reslist(control_t *p_reslist, u16 start, u16 size, u32 context)
{
    u16 i, cnt;

    DEBUG(UI_IPTV,INFO,"@@@ui_iptv_update_reslist start=%d\n", start);
    cnt = list_get_count(p_reslist);

    for (i = start; i < start + size && i < cnt; i++)
    {
        list_set_field_content_by_extstr(p_reslist, (u16)i, 0, g_pIptv->mainMenuName[i]);
    }

    return SUCCESS;
}

/*!
 * list update function
 */
static RET_CODE ui_iptv_update_catlist(control_t *p_catlist, u16 start, u16 size, u32 context)
{
    u16 *p_unistr;
    u16 i, cnt;

    cnt = list_get_count(p_catlist);

    DEBUG(UI_IPTV,INFO,"@@@ui_iptv_update_catlist start=%d\n", start);
	switch(start)
	{
		case IPTV_CATLIST_SEARCH:
			p_unistr = (u16 *)gui_get_string_addr(IDS_HD_SEARCH);
			list_set_field_content_by_unistr(p_catlist, IPTV_CATLIST_SEARCH, 0, p_unistr);
		case IPTV_CATLIST_ALL:
			p_unistr = (u16 *)gui_get_string_addr(IDS_ALL);
			list_set_field_content_by_unistr(p_catlist, IPTV_CATLIST_ALL, 0, p_unistr);
		case IPTV_CATLIST_FILTER:
	        p_unistr = (u16 *)gui_get_string_addr(IDS_HD_FILTER);
	        list_set_field_content_by_unistr(p_catlist, IPTV_CATLIST_FILTER, 0, p_unistr);
			
			i = IPTV_CATLIST_EXTERN;
			break;
		default:
			i = start;
			break;
	}

    for (; i < start + size && i < cnt; i++){
        list_set_field_content_by_unistr(p_catlist, i, 0, g_pIptv->catList[i - IPTV_CATLIST_EXTERN].name);
    }
    return SUCCESS;
}

static control_t *ui_iptv_create_vdolist(control_t *p_parent)
{
    control_t *p_vdolist = NULL;
    control_t *p_item = NULL;
    control_t *p_ctrl = NULL;
    u16 i;
    u16 x, y;

    DEBUG(UI_IPTV,INFO,"@@@ui_iptv_create_vdolist\n");
    p_vdolist = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_VDOLIST,
                                  IPTV_VDOLIST_X, IPTV_VDOLIST_Y,
                                  IPTV_VDOLIST_W, IPTV_VDOLIST_H,
                                  p_parent, 0);
    ctrl_set_rstyle(p_vdolist, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    g_pIptv->vdo_page_size = IPTV_VDOLIST_PAGE_SIZE;

    x = IPTV_VDOLIST_ITEM_X;
    y = IPTV_VDOLIST_ITEM_Y;
    for (i = 0; i < g_pIptv->vdo_page_size; i++)
    {
        p_item = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_VDOITEM_START + i,
                                x, y,
                                IPTV_VDOLIST_ITEM_W, IPTV_VDOLIST_ITEM_H,
                                p_vdolist, 0);
        ctrl_set_rstyle(p_item, RSI_PBACK, RSI_PBACK, RSI_PBACK);

        p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_IPTV_VDOITEM_PIC,
                                IPTV_VDOPIC_X, IPTV_VDOPIC_Y,
                                IPTV_VDOPIC_W, IPTV_VDOPIC_H,
                                p_item, 0);
        //ctrl_set_rstyle(p_ctrl, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
        ctrl_set_rstyle(p_ctrl, RSI_OTT_FRM_1_SH, RSI_VOD_VDOLIST_HL, RSI_OTT_FRM_1_SH);
        ctrl_set_mrect(p_ctrl, 
                        IPTV_VDOPIC_MIDL, IPTV_VDOPIC_MIDT,
                        IPTV_VDOPIC_MIDR, IPTV_VDOPIC_MIDB);

        p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_VDOITEM_NAME,
                                IPTV_VDONAME_X, IPTV_VDONAME_Y,
                                IPTV_VDONAME_W, IPTV_VDONAME_H,
                                p_item, 0);
        //ctrl_set_rstyle(p_ctrl, RSI_TRANSPARENT, RSI_TRANSPARENT, RSI_TRANSPARENT);
        ctrl_set_rstyle(p_ctrl, RSI_OTT_KEYBOARD_KEY_SH, RSI_OTT_KEYBOARD_KEY_SH, RSI_OTT_KEYBOARD_KEY_SH);
        ctrl_set_mrect(p_ctrl, 
                        IPTV_VDONAME_MIDL, IPTV_VDONAME_MIDT,
                        IPTV_VDONAME_MIDR, IPTV_VDONAME_MIDB);

        text_set_font_style(p_ctrl, FSI_WHITE, FSI_WHITE, FSI_WHITE);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_EXTSTR);

        x += IPTV_VDOLIST_ITEM_W + IPTV_VDOLIST_ITEM_H_GAP;
        if (x + IPTV_VDOPIC_W > 1080)
        {
            x = IPTV_VDOLIST_ITEM_X;
            y += IPTV_VDOLIST_ITEM_H + IPTV_VDOLIST_ITEM_V_GAP;
        }
    }

    return p_vdolist;
}

/* to do show/hide child ctrl
    vdolist pre update
*/
static RET_CODE ui_iptv_vdolist_set_count(control_t *p_vdolist, u16 count)
{
    control_t *p_item, *p_pic;
    rect_t rect;
    u16 i;

    DEBUG(UI_IPTV,INFO,"@@@%s count=%d\n", __FUNCTION__, count);
    MT_ASSERT(count <= g_pIptv->vdo_page_size);

    for (i = count; i < g_pIptv->vdo_cnt; i++)
    {
        p_item = ctrl_get_child_by_id(p_vdolist, IDC_IPTV_VDOITEM_START + i);
        p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_VDOITEM_PIC);

        //clear pic layer
        pic_stop();
        ctrl_get_frame(p_pic, &rect);
        ctrl_client2screen(p_pic, &rect);
        //DEBUG(UI_IPTV,INFO,"@@@left=%d, top=%d, right=%d, bottom=%d\n", rect.left, rect.top, rect.right, rect.bottom);
        ui_pic_clear_rect(&rect, 0);
    }

    g_pIptv->vdo_cnt = count;

    return SUCCESS;
}

static u16 ui_iptv_vdolist_calc_focus_pos(control_t *p_vdolist)
{
    control_t *p_active_item;
    u16 focus;
    u8  active_id;
    DEBUG(UI_IPTV,INFO,"@@@%s\n", __FUNCTION__);
    p_active_item = p_vdolist->p_active_child;
    if (p_active_item != NULL)
    {
        active_id = ctrl_get_ctrl_id(p_active_item);
        focus = active_id - IDC_IPTV_VDOITEM_START;
        if (focus < g_pIptv->vdo_page_size)
        {
            return focus;
        }
    }

    return LIST_INVALID_FOCUS;
}

static RET_CODE ui_iptv_vdolist_change_focus_pos(control_t *p_vdolist, u16 focus, BOOL is_paint)
{
    control_t *p_active_item, *p_active_pic = NULL;
    control_t *p_next_item, *p_next_pic;

    DEBUG(UI_IPTV,INFO,"@@@%s focus=%d\n", __FUNCTION__, focus);
    MT_ASSERT(focus < g_pIptv->vdo_page_size);

    p_active_item = p_vdolist->p_active_child;
    if (p_active_item != NULL)
    {
        p_active_pic = ctrl_get_child_by_id(p_active_item, IDC_IPTV_VDOITEM_PIC);
        MT_ASSERT(p_active_pic != NULL);
        ctrl_process_msg(p_active_pic, MSG_LOSTFOCUS, 0, 0);
    }

    p_next_item = ctrl_get_child_by_id(p_vdolist, IDC_IPTV_VDOITEM_START + focus);
    p_next_pic = ctrl_get_child_by_id(p_next_item, IDC_IPTV_VDOITEM_PIC);
    MT_ASSERT(p_next_pic != NULL);
    ctrl_set_attr(p_next_pic, OBJ_ATTR_ACTIVE);
    ctrl_process_msg(p_next_pic, MSG_GETFOCUS, 0, 0);

    if (is_paint)
    {
        if (p_active_pic != NULL)
        {
            ctrl_paint_ctrl(p_active_pic, TRUE);
        }
        if (p_next_pic != NULL)
        {
            ctrl_paint_ctrl(p_next_pic, TRUE);
        }
    }

    return SUCCESS;
}

static RET_CODE ui_iptv_update_vdolist(control_t *p_vdolist, u16 start, u16 size, u32 context)
{
    al_iptv_vdo_item_t *p_vdo;
    control_t *p_item, *p_pic, *p_text;
    u16 cnt, i;

    p_vdo = g_pIptv->vdoList;
    cnt = g_pIptv->vdo_cnt;
    if (p_vdo != NULL)
    {
        for (i = start; i < start + size && i < cnt; i++)
        {
            p_item = ctrl_get_child_by_id(p_vdolist, IDC_IPTV_VDOITEM_START + i);
            p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_VDOITEM_PIC);
            p_text = ctrl_get_child_by_id(p_item, IDC_IPTV_VDOITEM_NAME);

            bmap_set_content_by_id(p_pic, IM_PIC_LOADING_1);
            text_set_content_by_extstr(p_text, p_vdo[i].name);
            ctrl_set_sts(p_item, OBJ_STS_SHOW);
        }
        for (; i < start + size; i++)
        {
            p_item = ctrl_get_child_by_id(p_vdolist, IDC_IPTV_VDOITEM_START + i);
            ctrl_set_sts(p_item, OBJ_STS_HIDE);
        }
    }
    else
    {
        for (i = start; i < start + size && i < cnt; i++)
        {
            p_item = ctrl_get_child_by_id(p_vdolist, IDC_IPTV_VDOITEM_START + i);
            p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_VDOITEM_PIC);
            p_text = ctrl_get_child_by_id(p_item, IDC_IPTV_VDOITEM_NAME);

            bmap_set_content_by_id(p_pic, IM_PIC_LOADING_1);
            text_set_content_by_extstr(p_text, NULL);
            ctrl_set_sts(p_item, OBJ_STS_SHOW);
        }
        for (; i < start + size; i++)
        {
            p_item = ctrl_get_child_by_id(p_vdolist, IDC_IPTV_VDOITEM_START + i);
            ctrl_set_sts(p_item, OBJ_STS_HIDE);
        }
    }

    return SUCCESS;
}

static void ui_iptv_update_page_num(control_t* p_ctrl, u32 page_num, u32 total_page)
{
    char num[23 + 1];
  
    sprintf(num, "%ld / %ld", page_num, total_page);
    text_set_content_by_ascstr(p_ctrl, num);
}

/*================================================================================================
                           iptv sm function
 ================================================================================================*/

static BOOL    StcIptv_is_on_reslist(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_IPTV_RESLIST) ? TRUE : FALSE;
}

static BOOL    StcIptv_is_on_catlist(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_IPTV_CATLIST) ? TRUE : FALSE;
}

static BOOL    StcIptv_is_on_filter(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_IPTV_FILTER_CONT) ? TRUE : FALSE;
}

static BOOL    StcIptv_is_on_vdolist(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_IPTV_VDOLIST) ? TRUE : FALSE;
}

static BOOL    StcIptv_is_on_page_num(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    return (ctrl_get_ctrl_id(ctrl) == IDC_IPTV_PAGE_NUM) ? TRUE : FALSE;
}

static void SenIptv_Inactive(void)
{
    DEBUG(UI_IPTV,INFO,"@@@SenIptv_Inactive\n");
}

static STATEID StaIptv_inactive_on_open_iptv_req(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    DEBUG(UI_IPTV,INFO,"@@@StaIptv_inactive_on_open_iptv_req\n");
    return SID_IPTV_INITIALIZE;
}

static void SexIptv_Inactive(void)
{
    DEBUG(UI_IPTV,INFO,"@@@SexIptv_Inactive\n");
}

static void SenIptv_Active(void)
{
    DEBUG(UI_IPTV,INFO,"@@@SenIptv_Active\n");
}

static STATEID StaIptv_active_on_quick_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    DEBUG(UI_IPTV,INFO,"@@@%s\n", __FUNCTION__);
    if (!Iptv_IsStateActive(SID_IPTV_DEINIT))
    {
        ui_iptv_dp_deinit();
    }

    return SID_NULL;
}

static STATEID StaIptv_active_on_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    DEBUG(UI_IPTV,INFO,"@@@%s\n", __FUNCTION__);
    if (!Iptv_IsStateActive(SID_IPTV_DEINIT))
    {
        return SID_IPTV_DEINIT;
    }
    return SID_NULL;
}

static STATEID StaIptv_resource_on_filter_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_cont=NULL;
	control_t *p_focus=NULL;

	DEBUG(UI_IPTV,INFO,"@@@@@%s %d \n",__FUNCTION__,__LINE__);
	p_cont = ctrl_get_parent(ctrl);
	p_focus = ctrl_get_child_by_id(ctrl_get_active_ctrl(ctrl),IDC_IPTV_FILTER_LIST);

	ctrl_set_sts(ctrl,OBJ_STS_HIDE);
	ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
	ctrl_change_focus(p_focus,g_pIptv->hCatList);

	return SID_NULL;
}

static void SexIptv_Active(void)
{
    DEBUG(UI_IPTV,INFO,"@@@SexIptv_Active\n");
}

/* --------------------- Initialize:  ---------------------*/
static void SenIptv_Initialize(void)
{
    DEBUG(UI_IPTV,INFO,"@@@SenIptv_Initialize\n");
    ui_iptv_open_dlg(ROOT_ID_IPTV, IDS_HD_LOADING_WITH_WAIT);

    ui_iptv_dp_init();
    ui_iptv_set_page_size(g_pIptv->vdo_page_size);
}

static STATEID StaIptv_initialize_on_init_success(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    DEBUG(UI_IPTV,INFO,"@@@%s\n",__FUNCTION__);
    ui_iptv_set_dp_state(IPTV_DP_STATE_INIT_SUCC);
    ui_iptv_get_res_list();
    return SID_NULL;
}

static STATEID StaIptv_initialize_on_newresname_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    al_iptv_name_res_list_t *p_data = (al_iptv_name_res_list_t *)para1;
    u16 str_len;
    u16 i;

    DEBUG(UI_IPTV,INFO,"@@@StaIptv_initialize_on_newresname_arrive\n");
    if (p_data->total_res > 0 && p_data->resList != NULL)
    {
        if (g_pIptv->resList)
        {
            SY_FREE(g_pIptv->resList);// name not free yet
        }

        g_pIptv->resList = (ui_iptv_res_item_t *)SY_MALLOC(p_data->total_res * sizeof(ui_iptv_res_item_t));
        MT_ASSERT(g_pIptv->resList != NULL);
        memset(g_pIptv->resList, 0, p_data->total_res * sizeof(ui_iptv_res_item_t));

        g_pIptv->total_res = p_data->total_res;
        g_pIptv->totalMainMenu = p_data->total_res
#if ENABLE_IPTV_FAV
                                             + 1
#endif
#if ENABLE_PLAY_HIST
                                             + 1
#endif
        ;
        g_pIptv->mainMenuName = (u16 **)SY_MALLOC(g_pIptv->totalMainMenu * sizeof(u16 *));
        g_pIptv->curResListIdx = 0;

        for (i = 0; i < p_data->total_res; i++)
        {
	     DEBUG(UI_IPTV,INFO,"res id[%d],name[%s]\n",p_data->resList[i].res_id,p_data->resList[i].name);        
            g_pIptv->resList[i].res_id = p_data->resList[i].res_id;
            str_len = (u16)uni_strlen(p_data->resList[i].name);
            g_pIptv->resList[i].name = (u16 *)SY_MALLOC((str_len + 1) * sizeof(u16));
            MT_ASSERT(g_pIptv->resList[i].name != NULL);
            uni_strcpy(g_pIptv->resList[i].name, p_data->resList[i].name);

            g_pIptv->mainMenuName[i] = g_pIptv->resList[i].name;
        }
#if ENABLE_IPTV_FAV
        g_pIptv->mainMenuName[i] = (u16 *)gui_get_string_addr(IDS_HD_FAVORITE);
        i++;
#endif
#if ENABLE_PLAY_HIST
        g_pIptv->mainMenuName[i] = (u16 *)gui_get_string_addr(IDS_HD_PLAY_HISTORY);
#endif

        // update ui ctrl
        DEBUG(UI_IPTV,INFO,"@@@hTitleName attr=%d\n", ctrl_get_attr(g_pIptv->hTitleName));
        text_set_content_by_extstr(g_pIptv->hTitleName, g_pIptv->mainMenuName[g_pIptv->curResListIdx]);
        ctrl_set_sts(g_pIptv->hTitleName, OBJ_STS_SHOW);

        list_set_count(g_pIptv->hResList, g_pIptv->totalMainMenu, IPTV_RESLIST_PAGE_SIZE);
        list_set_focus_pos(g_pIptv->hResList, g_pIptv->curResListIdx);
        list_select_item(g_pIptv->hResList, g_pIptv->curResListIdx);
        ui_iptv_update_reslist(g_pIptv->hResList, list_get_valid_pos(g_pIptv->hResList), IPTV_RESLIST_PAGE_SIZE, 0);

        ctrl_paint_ctrl(g_pIptv->hTitleName, TRUE);
        ctrl_set_sts(g_pIptv->hResList, OBJ_STS_SHOW);
        ctrl_change_focus(NULL, g_pIptv->hResList);

        ui_iptv_get_catgry_list();
        return SID_NULL;
    }

    return SID_IPTV_INIT_FAILED;
}

static STATEID StaIptv_initialize_on_newrescatgry_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    al_iptv_cat_res_list_t *p_data = (al_iptv_cat_res_list_t *)para1;
    al_iptv_cat_item_t *p_catList;
    al_iptv_filter_item_t *p_filter;
    u16 i, j, k,l;
    u16 str_len;

    DEBUG(UI_IPTV,INFO,"@@@StaIptv_initialize_on_newrescatgry_arrivetotal_res = %d addr[%#x]\n",p_data->total_res,p_data->resList);
    if (p_data->total_res > 0 && p_data->resList != NULL)
    {
        for (i = 0; i < g_pIptv->total_res; i++)
        {
            for (j = 0; j < p_data->total_res; j++)
            {
                if (g_pIptv->resList[i].res_id == p_data->resList[j].res_id)
                {
                    if (p_data->resList[j].total_cat > 0)
                    {
                        p_catList = (al_iptv_cat_item_t *)SY_MALLOC(p_data->resList[j].total_cat * sizeof(al_iptv_cat_item_t));
                        MT_ASSERT(p_catList != NULL);

                        for (k = 0; k < p_data->resList[i].total_cat; k++)
                        {
                            if (p_data->resList[j].catList[k].name)
                            {
                                str_len = uni_strlen(p_data->resList[j].catList[k].name);
                                p_catList[k].name = (u16 *)SY_MALLOC((str_len + 1) * sizeof(u16));
                                MT_ASSERT(p_catList[k].name != NULL);
                                uni_strcpy(p_catList[k].name, p_data->resList[j].catList[k].name);
                            }
                            else
                            {
                                p_catList[k].name = NULL;
                            }

                            if (p_data->resList[j].catList[k].key)
                            {
                                str_len = strlen(p_data->resList[j].catList[k].key);
                                p_catList[k].key = (u8 *)SY_MALLOC((str_len + 1));
                                MT_ASSERT(p_catList[k].key != NULL);
                                strcpy(p_catList[k].key, p_data->resList[j].catList[k].key);
                            }
                            else
                            {
                                p_catList[k].key = NULL;
                            }

                            p_catList[k].total_vdo = p_data->resList[j].catList[k].total_vdo;
                        }
                    }
                    else
                    {
                        p_catList = NULL;
                    }
                    g_pIptv->resList[i].total_cat = p_data->resList[j].total_cat;
                    if(g_pIptv->resList[i].catList)
                        ui_release_rescatgry_catlist(i);
                    g_pIptv->resList[i].catList = p_catList;

		    //add fuxl for filter
		    if (p_data->resList[j].total_filter > 0)
                    {
                        p_filter = (al_iptv_filter_item_t *)SY_MALLOC(p_data->resList[j].total_filter * sizeof(al_iptv_filter_item_t));
                        MT_ASSERT(p_filter != NULL);
			memset(p_filter, 0, p_data->resList[j].total_filter * sizeof(al_iptv_filter_item_t));

                        for (k = 0; k < p_data->resList[i].total_filter; k++)
                        {
                            if (p_data->resList[j].filter[k].name)
                            {
                                str_len = uni_strlen(p_data->resList[j].filter[k].name);
                                p_filter[k].name = (u16 *)SY_MALLOC((str_len + 1) * sizeof(u16));
                                MT_ASSERT(p_filter[k].name != NULL);
                                uni_strcpy(p_filter[k].name, p_data->resList[j].filter[k].name);
                            }
                            else
                            {
                                p_filter[k].name = NULL;
                            }

			    p_filter[k].subTagSize = p_data->resList[j].filter[k].subTagSize;
                            p_filter[k].subtag = (al_iptv_tags_item_t *)SY_MALLOC(p_data->resList[j].filter[k].subTagSize * sizeof(al_iptv_tags_item_t));
                            MT_ASSERT(p_filter[k].subtag != NULL);
			    for(l = 0; l < p_data->resList[j].filter[k].subTagSize; l++)
			    {
				    if (p_data->resList[j].filter[k].subtag[l].name)
				    {
					    str_len = uni_strlen(p_data->resList[j].filter[k].subtag[l].name);
					    p_filter[k].subtag[l].name = (u16 *)SY_MALLOC((str_len + 1)*sizeof(u16));
					    MT_ASSERT(p_filter[k].subtag[l].name != NULL);
					    uni_strcpy(p_filter[k].subtag[l].name, p_data->resList[j].filter[k].subtag[l].name);
				    }
				    else
				    {
					    p_filter[k].subtag[l].name = NULL;
				    }

				    if (p_data->resList[j].filter[k].subtag[l].key)
				    {
					    str_len = strlen(p_data->resList[j].filter[k].subtag[l].key);
					    p_filter[k].subtag[l].key = (u8 *)SY_MALLOC((str_len + 1));
					    MT_ASSERT(p_filter[k].subtag[l].key != NULL);
					    strcpy(p_filter[k].subtag[l].key, p_data->resList[j].filter[k].subtag[l].key);
				    }
				    else
				    {
					    p_filter[k].subtag[l].name = NULL;
				    }
			    }
                            p_filter[k].total_vdo = p_data->resList[j].filter[k].total_vdo;
                        }
                    }
                    else
                    {
                        p_filter = NULL;
                    }
                    g_pIptv->resList[i].total_filter = p_data->resList[j].total_filter;

                    if(g_pIptv->resList[i].filter)
                        ui_release_rescatgry_filter(i);
                    g_pIptv->resList[i].filter = p_filter;
		    break;
                }
            }
        }
        g_pIptv->catList = g_pIptv->resList[g_pIptv->curResListIdx].catList;
        g_pIptv->total_cat = g_pIptv->resList[g_pIptv->curResListIdx].total_cat;
        g_pIptv->curCatListIdx = 0;
        g_pIptv->catFilter_flag = IPTV_FLAG_CAT;

	 DEBUG(UI_IPTV,INFO,"g_pIptv->resList[g_pIptv->curResListIdx].total_cat = %d\n",g_pIptv->resList[g_pIptv->curResListIdx].total_cat);
	 DEBUG(UI_IPTV,INFO," g_pIptv->catList[%#x]\n",g_pIptv->catList);
	 

        list_set_count(g_pIptv->hCatList, g_pIptv->total_cat + IPTV_CATLIST_EXTERN, IPTV_CATLIST_PAGE_SIZE);
        if (g_pIptv->total_cat > 0)
        {
            list_set_focus_pos(g_pIptv->hCatList, g_pIptv->curCatListIdx + IPTV_CATLIST_ALL);
            list_select_item(g_pIptv->hCatList, g_pIptv->curCatListIdx + IPTV_CATLIST_ALL);
			g_pIptv->catFilter_flag = IPTV_FLAG_FILTER;
        }
        else
        {
            list_set_focus_pos(g_pIptv->hCatList, 0);
            list_select_item(g_pIptv->hCatList, 0);
        }

        ui_iptv_update_catlist(g_pIptv->hCatList, list_get_valid_pos(g_pIptv->hCatList), IPTV_CATLIST_PAGE_SIZE, 0);

        ctrl_set_sts(g_pIptv->hCatList, OBJ_STS_SHOW);
        ctrl_change_focus(g_pIptv->hResList, g_pIptv->hCatList);

        return SID_IPTV_RESOURCE;
    }

    return SID_IPTV_INIT_FAILED;
}

static void SexIptv_Initialize(void)
{
    DEBUG(UI_IPTV,INFO,"@@@SexIptv_Initialize\n");
    ui_comm_dlg_close();
}

/* --------------------- Video list: Resource ---------------------*/
static void SenIptv_Resource(void)
{
    DEBUG(UI_IPTV,INFO,"@@@SenIptv_Resource\n");
    g_pIptv->total_vdo = (g_pIptv->catList != NULL) ? g_pIptv->catList[g_pIptv->curCatListIdx].total_vdo : 0;
    g_pIptv->total_page = (g_pIptv->total_vdo > 0) ? ((g_pIptv->total_vdo + g_pIptv->vdo_page_size - 1) / g_pIptv->vdo_page_size) : 1;
    g_pIptv->b_totalPageChanged = TRUE;
    g_pIptv->curPageNum = 1;
    g_pIptv->curVdoListIdx = 0;

    ctrl_set_attr(g_pIptv->hVdoList, OBJ_ATTR_ACTIVE);
    ctrl_set_sts(g_pIptv->hVdoList, OBJ_STS_SHOW);
}

static STATEID StaIptv_resource_on_vdolist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    FOCUS_KEY_MAP(para1, msg);

    DEBUG(UI_IPTV,INFO,"@@@%s curVdoListIdx=%d\n", __FUNCTION__, g_pIptv->curVdoListIdx);
    switch(msg)
    {
        case MSG_FOCUS_UP:
            if (g_pIptv->curVdoListIdx >= IPTV_VDOLIST_ITEM_COL)
            {
                g_pIptv->curVdoListIdx -= IPTV_VDOLIST_ITEM_COL;
                ui_iptv_vdolist_change_focus_pos(g_pIptv->hVdoList, g_pIptv->curVdoListIdx, TRUE);
            }
            else
            {
                if (g_pIptv->curPageNum > 1)
                {
                    g_pIptv->curPageNum--;
                    return SID_IPTV_RES_VDO_REQ;
                }/*
                else
                {
                    ctrl_change_focus(g_pIptv->hVdoList, g_pIptv->hResList);
                }*/
            }
            break;

        case MSG_FOCUS_DOWN:
            if (g_pIptv->curVdoListIdx < g_pIptv->vdo_page_size - IPTV_VDOLIST_ITEM_COL)
            {
                if(g_pIptv->vdo_cnt <= IPTV_VDOLIST_ITEM_COL)
                {
                    ctrl_change_focus(g_pIptv->hVdoList, g_pIptv->hCatList);
                }else
                {
                    g_pIptv->curVdoListIdx += IPTV_VDOLIST_ITEM_COL;
                    if (g_pIptv->curVdoListIdx >= g_pIptv->vdo_cnt)
                    {
                        g_pIptv->curVdoListIdx = g_pIptv->vdo_cnt - 1;
                    }
                    ui_iptv_vdolist_change_focus_pos(g_pIptv->hVdoList, g_pIptv->curVdoListIdx, TRUE);
                }
            }
            else
            {
                /*if (g_pIptv->curPageNum < g_pIptv->total_page)
                {
                    g_pIptv->curPageNum++;
                    return SID_IPTV_RES_VDO_REQ;
                }*/
                ctrl_change_focus(g_pIptv->hVdoList, g_pIptv->hCatList);
            }
            break;

        case MSG_FOCUS_LEFT:
            if (g_pIptv->curVdoListIdx != 0 && g_pIptv->curVdoListIdx != IPTV_VDOLIST_ITEM_COL)
            {
                g_pIptv->curVdoListIdx--;
                ui_iptv_vdolist_change_focus_pos(g_pIptv->hVdoList, g_pIptv->curVdoListIdx, TRUE);
            }
            else
            {
                if (g_pIptv->curPageNum > 1)
                {
                	switch(CUSTOMER_ID)
                	{
						case CUSTOMER_MAIKE_HUNAN_LINLI:
						case CUSTOMER_MAIKE_HUNAN:
					        g_pIptv->curVdoListIdx += 5;
					        ui_iptv_vdolist_change_focus_pos(g_pIptv->hVdoList, g_pIptv->curVdoListIdx, TRUE);							
							break;
						default:
							break;
					}
					g_pIptv->curPageNum--;
					return SID_IPTV_RES_VDO_REQ;
                }
            }
            break;

        case MSG_FOCUS_RIGHT:
            if (g_pIptv->curVdoListIdx != (IPTV_VDOLIST_ITEM_COL - 1) 
                && g_pIptv->curVdoListIdx != (IPTV_VDOLIST_ITEM_COL *2 -1))
            {
                g_pIptv->curVdoListIdx++;
                if (g_pIptv->curVdoListIdx >= g_pIptv->vdo_cnt)
                {
                    g_pIptv->curVdoListIdx = g_pIptv->vdo_cnt - 1;
                }
                ui_iptv_vdolist_change_focus_pos(g_pIptv->hVdoList, g_pIptv->curVdoListIdx, TRUE);
            }
            else
            {
                if (g_pIptv->curPageNum < g_pIptv->total_page)
                {
                    switch(CUSTOMER_ID)
                	{
						case CUSTOMER_MAIKE_HUNAN_LINLI:
						case CUSTOMER_MAIKE_HUNAN:
							g_pIptv->curVdoListIdx -= 5;
							if (g_pIptv->curVdoListIdx >= g_pIptv->vdo_cnt)
							{
								g_pIptv->curVdoListIdx = 0;
							}
							ui_iptv_vdolist_change_focus_pos(g_pIptv->hVdoList, g_pIptv->curVdoListIdx, TRUE);	
							break;
						default:
							break;
					}
					g_pIptv->curPageNum++;
					return SID_IPTV_RES_VDO_REQ;
                }
            }
            break;
    }

    DEBUG(UI_IPTV,INFO,"@@@curVdoListIdx=%d\n", g_pIptv->curVdoListIdx);
    return SID_NULL;
}

static STATEID StaIptv_resource_on_change_page(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    PAGE_KEY_MAP(para1, msg);

    DEBUG(UI_IPTV,INFO,"@@@StaIptv_resource_on_change_page\n");
    if (msg == MSG_PAGE_UP)
    {
        if (g_pIptv->curPageNum > 1)
        {
            g_pIptv->curPageNum--;
            return SID_IPTV_RES_VDO_REQ;
        }
    }
    else if (msg == MSG_PAGE_DOWN)
    {
        if (g_pIptv->curPageNum < g_pIptv->total_page)
        {
            g_pIptv->curPageNum++;
            return SID_IPTV_RES_VDO_REQ;
        }
    }

    return SID_NULL;
}

static STATEID StaIptv_resource_on_catlist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    FOCUS_KEY_MAP(para1, msg);
    DEBUG(UI_IPTV,INFO,"@@@%s\n", __FUNCTION__);

    switch(msg)
    {
        case MSG_FOCUS_UP:
            ctrl_process_msg(g_pIptv->hCatList, MSG_LOSTFOCUS, 0, 0);
            ctrl_paint_ctrl(g_pIptv->hCatList, TRUE);
            ui_iptv_vdolist_change_focus_pos(g_pIptv->hVdoList, g_pIptv->curVdoListIdx, TRUE);
            //ctrl_paint_ctrl(g_pIptv->hVdoList, TRUE);
            break;

        case MSG_FOCUS_DOWN:
            ctrl_change_focus(g_pIptv->hCatList, g_pIptv->hResList);
            break;

        case MSG_FOCUS_RIGHT:
            list_class_proc(g_pIptv->hCatList, MSG_FOCUS_RIGHT, 0, 0);
            break;
        case MSG_FOCUS_LEFT:
            list_class_proc(g_pIptv->hCatList, MSG_FOCUS_LEFT, 0, 0);
            break;
    }

    return SID_NULL;
}

static STATEID StaIptv_resource_on_reslist_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{

    FOCUS_KEY_MAP(para1, msg);
    DEBUG(UI_IPTV,INFO,"@@@%s\n", __FUNCTION__);

    switch (msg)
    {
        case MSG_FOCUS_LEFT:
                list_class_proc(g_pIptv->hResList, MSG_FOCUS_LEFT, 0, 0);
            break;
        case MSG_FOCUS_RIGHT:
            list_class_proc(g_pIptv->hResList, MSG_FOCUS_RIGHT, 0, 0);
            break;
        case MSG_FOCUS_UP:
            ctrl_process_msg(g_pIptv->hResList, MSG_LOSTFOCUS, 0, 0);
            ctrl_paint_ctrl(g_pIptv->hResList, TRUE);
            ctrl_change_focus(g_pIptv->hResList, g_pIptv->hCatList);
            break;
    }

    return SID_NULL;
}

static u16 ui_iptv_list_get_item_selected_pos(control_t *ctrl, u16 sum)
{
    u16 focus;

    for(focus = 0; focus < sum; focus++)
    {
        if(list_get_item_status(ctrl, focus) == LIST_ITEM_SELECTED)
        {
            return focus;
        }
    }

    return 0;
}

static STATEID ui_iptv_filter_get_video_list(control_t *ctrl)
{
    u8 key[128]={0};
    u16 focus,i;
    control_t *p_focus = NULL;
    control_t *p_list = NULL;

	if(IPTV_CATLIST_ALL == list_get_focus_pos(g_pIptv->hCatList))
	{
		for(i=0;i<g_pIptv->resList[g_pIptv->curResListIdx].total_filter;i++)
		{
			strcat(key,g_pIptv->resList[g_pIptv->curResListIdx].filter[i].subtag[0].key);
			strcat(key,",");
		}
	}
	else
	{
		if(ctrl == NULL)
			return SID_NULL;
		
		for(i=0;i<g_pIptv->resList[g_pIptv->curResListIdx].total_filter;i++)
		{
			p_focus = ctrl_get_child_by_id(ctrl, IDC_IPTV_FILTER_START + i);
			p_list = ctrl_get_child_by_id(p_focus,IDC_IPTV_FILTER_LIST);
		
			focus = ui_iptv_list_get_item_selected_pos(p_list,g_pIptv->resList[g_pIptv->curResListIdx].filter[i].subTagSize);
			strcat(key,g_pIptv->resList[g_pIptv->curResListIdx].filter[i].subtag[focus].key);
			strcat(key,",");
		}
	}


    DEBUG(UI_IPTV,INFO,"@@@%s %d key=%s \n",__FUNCTION__,__LINE__,key);
    ui_iptv_get_video_list_for_filter(g_pIptv->resList[g_pIptv->curResListIdx].res_id,NULL,key,g_pIptv->curPageNum);

    return SID_NULL;
}

static STATEID StaIptv_resource_on_filter_focus_key_hldr(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
	control_t *p_next_focus = NULL,*p_focus = NULL;
	control_t *p_list = NULL;
	u16 ctrl_id = 0,next_ctrl_id = 0,focusPos = 0;

	FOCUS_KEY_MAP(para1, msg);
	p_focus = ctrl_get_active_ctrl(ctrl);
	ctrl_id = ctrl_get_ctrl_id(p_focus);

	DEBUG(UI_IPTV,INFO,"@@@%s %d \n",__FUNCTION__,__LINE__);
	switch(msg)
	{
		case MSG_FOCUS_UP:
			if(ctrl_id > IDC_IPTV_FILTER_START)
			{
				next_ctrl_id = ctrl_id - 1;
				p_next_focus = ctrl_get_child_by_id(ctrl_get_child_by_id(ctrl,next_ctrl_id),IDC_IPTV_FILTER_LIST);
			}
			break;
		case MSG_FOCUS_DOWN:
			if(ctrl_id < IDC_IPTV_FILTER_START + g_pIptv->resList[g_pIptv->curResListIdx].total_filter - 1)
			{
				next_ctrl_id = ctrl_id + 1;
				p_next_focus = ctrl_get_child_by_id(ctrl_get_child_by_id(ctrl,next_ctrl_id),IDC_IPTV_FILTER_LIST);
			}
                   else if(ctrl_id == IDC_IPTV_FILTER_START + g_pIptv->resList[g_pIptv->curResListIdx].total_filter - 1)
                   {
				next_ctrl_id = ctrl_id + 1;
				p_next_focus = ctrl_get_child_by_id(ctrl,next_ctrl_id);
                    }
			break;
		case MSG_FOCUS_RIGHT:
                    if(ctrl_id < IDC_IPTV_FILTER_START + g_pIptv->resList[g_pIptv->curResListIdx].total_filter)
                    {
                        p_list = ctrl_get_child_by_id(p_focus,IDC_IPTV_FILTER_LIST);
                        focusPos = list_get_focus_pos(p_list);
                        if(focusPos < g_pIptv->resList[g_pIptv->curResListIdx].filter[ctrl_id - IDC_IPTV_FILTER_START].subTagSize - 1)
                                list_class_proc(p_list, MSG_FOCUS_RIGHT, 0, 0);
                    }
                     break;
		case MSG_FOCUS_LEFT:
                    if(ctrl_id < IDC_IPTV_FILTER_START + g_pIptv->resList[g_pIptv->curResListIdx].total_filter)
                    {
                        p_list = ctrl_get_child_by_id(p_focus,IDC_IPTV_FILTER_LIST);
                        list_class_proc(p_list, MSG_FOCUS_LEFT, 0, 0);
                    }
                    break;
		default:
			break;

	}

	if(p_next_focus != NULL)
	{
		ctrl_process_msg(p_focus, MSG_LOSTFOCUS, para1, para2);
		ctrl_process_msg(p_next_focus, MSG_GETFOCUS, 0, 0);
             if(next_ctrl_id != IDC_IPTV_FILTER_START + g_pIptv->resList[g_pIptv->curResListIdx].total_filter)
             {
        		focusPos = ui_iptv_list_get_item_selected_pos(p_next_focus,g_pIptv->resList[g_pIptv->curResListIdx].filter[(next_ctrl_id - IDC_IPTV_FILTER_START)].subTagSize);
        		list_set_focus_pos(p_next_focus,focusPos);
             }
		ctrl_paint_ctrl(p_focus, TRUE);
		ctrl_paint_ctrl(p_next_focus, TRUE);
	}

	return SUCCESS;
}

static STATEID StaIptv_resource_on_filter_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus;
    control_t *p_focus = NULL,*p_cont = NULL;
    control_t *p_list = NULL;

    p_focus = ctrl_get_active_ctrl(ctrl);
    if(ctrl_get_ctrl_id(p_focus) == (IDC_IPTV_FILTER_START + g_pIptv->resList[g_pIptv->curResListIdx].total_filter))
    {
        DEBUG(UI_IPTV,INFO,"@@@%s %d \n",__FUNCTION__,__LINE__);
        
        g_pIptv->catFilter_flag = IPTV_FLAG_FILTER;
        
        p_cont = ctrl_get_parent(ctrl);
        ctrl_set_sts(ctrl,OBJ_STS_HIDE);
        ctrl_change_focus(p_focus,g_pIptv->hCatList);
        ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
        ui_iptv_open_dlg(ROOT_ID_IPTV, IDS_HD_LOADING_WITH_WAIT);
        
        return SID_IPTV_RESOURCE;
    }
    else
    {
        p_list = ctrl_get_child_by_id(p_focus,IDC_IPTV_FILTER_LIST);

        focus = list_get_focus_pos(p_list);
        list_select_item(p_list, focus);
        ctrl_paint_ctrl(p_list, TRUE);
    }
    
    return SID_NULL;
}

static STATEID StaIptv_resource_on_catlist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus;
    control_t *filter = NULL, *p_focus = NULL;

    focus = list_get_focus_pos(ctrl);
    list_select_item(ctrl, focus);

    DEBUG(UI_IPTV,INFO,"@@@StaIptv_resource_on_catlist_selected focus=%d\n", focus);
	switch(focus)
	{
		case IPTV_CATLIST_SEARCH:
			ui_iptv_pic_deinit();
			ui_iptv_search_set_res_id(g_pIptv->resList[g_pIptv->curResListIdx].res_id);
			manage_open_menu(ROOT_ID_IPTV_SEARCH, 0, 0);
			break;

		case IPTV_CATLIST_FILTER:
			filter = create_iptv_filter(ctrl);
			p_focus = ctrl_get_child_by_id(ctrl_get_child_by_id(filter,IDC_IPTV_FILTER_START), IDC_IPTV_FILTER_LIST);
			ctrl_change_focus(g_pIptv->hCatList, p_focus);
			ctrl_paint_ctrl(filter, TRUE);
			break;

		case IPTV_CATLIST_ALL:
			g_pIptv->catFilter_flag = IPTV_FLAG_FILTER;
			ctrl_paint_ctrl(ctrl, TRUE);
			return SID_IPTV_RESOURCE;
			
		default:
			g_pIptv->curCatListIdx = focus - IPTV_CATLIST_EXTERN;
			g_pIptv->catFilter_flag = IPTV_FLAG_CAT;
			ctrl_paint_ctrl(ctrl, TRUE);
			return SID_IPTV_RESOURCE;

	}

    return SID_NULL;
}

static STATEID StaIptv_resource_on_reslist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    u16 focus;

    DEBUG(UI_IPTV,INFO,"@@@StaIptv_resource_on_reslist_selected\n");
    focus = list_get_focus_pos(ctrl);
    list_select_item(ctrl, focus);

    if (focus < g_pIptv->total_res)
    {
        g_pIptv->curResListIdx = focus;
        g_pIptv->catList = g_pIptv->resList[g_pIptv->curResListIdx].catList;
        g_pIptv->total_cat = g_pIptv->resList[g_pIptv->curResListIdx].total_cat;
        g_pIptv->curCatListIdx = 0;
        g_pIptv->catFilter_flag = IPTV_FLAG_CAT;

        text_set_content_by_extstr(g_pIptv->hTitleName, g_pIptv->mainMenuName[g_pIptv->curResListIdx]);

        list_set_count(g_pIptv->hCatList, g_pIptv->total_cat+2, IPTV_CATLIST_PAGE_SIZE);
        if (g_pIptv->total_cat > 0)
        {
            list_set_focus_pos(g_pIptv->hCatList, g_pIptv->curCatListIdx+IPTV_CATLIST_ALL);
            list_select_item(g_pIptv->hCatList, g_pIptv->curCatListIdx+IPTV_CATLIST_ALL);

			g_pIptv->catFilter_flag = IPTV_FLAG_FILTER;
        }
        else
        {
            list_set_focus_pos(g_pIptv->hCatList, 0);
            list_select_item(g_pIptv->hCatList, 0);
        }

        ui_iptv_update_catlist(g_pIptv->hCatList, list_get_valid_pos(g_pIptv->hCatList), IPTV_CATLIST_PAGE_SIZE, 0);

        ctrl_paint_ctrl(g_pIptv->hTitleName, TRUE);
        ctrl_change_focus(g_pIptv->hResList, g_pIptv->hCatList);

        return SID_IPTV_RESOURCE;
    }
#if ENABLE_IPTV_FAV
    else if (focus == g_pIptv->total_res)
    {
        ui_iptv_pic_deinit();
        manage_open_menu(ROOT_ID_VDO_FAVORITE, 1, 0);
    }
#endif
#if ENABLE_PLAY_HIST
    else if (focus == g_pIptv->total_res + 1)
    {
        ui_iptv_pic_deinit();
        manage_open_menu(ROOT_ID_PLAY_HIST, 0, 0);
    }
#endif

    return SID_NULL;
}

static STATEID StaIptv_resource_on_subwindow_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    DEBUG(UI_IPTV,INFO,"@@@StaIptv_resource_on_subwindow_exit\n");
	set_iptv_des_state(IPTV_DESC_DESTORY);
    ui_iptv_pic_init();
    ui_iptv_set_page_size(g_pIptv->vdo_page_size);

    if (Iptv_IsStateActive(SID_IPTV_RES_VDOLIST_NORMAL))
    {
        SenIptv_ResVdolistNormal();
    }

    return SID_NULL;
}

static void SexIptv_Resource(void)
{
    DEBUG(UI_IPTV,INFO,"@@@SexIptv_Resource\n");
    ctrl_set_sts(g_pIptv->hVdoList, OBJ_STS_HIDE);
}

static void SenIptv_ResVdoReq(void)
{
    u16 vdo_cnt;

    DEBUG(UI_IPTV,INFO,"@@@SenIptv_ResVdoReq() curResListIdx=%d, curCatListIdx=%d\n", g_pIptv->curResListIdx, g_pIptv->curCatListIdx);
    ui_release_vdolist();

    vdo_cnt = (u16)((g_pIptv->curPageNum < g_pIptv->total_page) 
                  ? g_pIptv->vdo_page_size 
                  : (g_pIptv->total_vdo - (g_pIptv->total_page - 1) * g_pIptv->vdo_page_size));
    ui_iptv_vdolist_set_count(g_pIptv->hVdoList, vdo_cnt);

    if (g_pIptv->curPageNum > 1)
    {
        if (g_pIptv->curVdoListIdx >= g_pIptv->vdo_cnt)
        {
            g_pIptv->curVdoListIdx = g_pIptv->vdo_cnt - 1;
        }
    }
    if (ui_iptv_vdolist_calc_focus_pos(g_pIptv->hVdoList) != g_pIptv->curVdoListIdx
        && ctrl_is_onfocus(g_pIptv->hVdoList))
    {
        ui_iptv_vdolist_change_focus_pos(g_pIptv->hVdoList, g_pIptv->curVdoListIdx, FALSE);
    }

    ui_iptv_update_vdolist(g_pIptv->hVdoList, 0, g_pIptv->vdo_page_size, 0);
    //ui_iptv_update_page_num(g_pIptv->hPageNum, g_pIptv->curPageNum, g_pIptv->total_page);

    ctrl_paint_ctrl(g_pIptv->hVdoList, TRUE);
    ctrl_paint_ctrl(g_pIptv->hPageNum, TRUE);

    DEBUG(UI_IPTV,INFO,"g_pIptv->catList[%#x]!\n",(u32)g_pIptv->catList);

	if(g_pIptv->catFilter_flag == IPTV_FLAG_FILTER)
	{
		ui_iptv_filter_get_video_list(g_pIptv->hFilter);
		return;
	}

    if (g_pIptv->catList)
    {
     	 DEBUG(UI_IPTV,INFO,"get video list!\n");
        ui_iptv_get_video_list(g_pIptv->resList[g_pIptv->curResListIdx].res_id, 
                          g_pIptv->catList[g_pIptv->curCatListIdx].name, 
                          g_pIptv->catList[g_pIptv->curCatListIdx].key, 
                          g_pIptv->curPageNum);
    }
}

static STATEID StaIptv_resource_on_newpagevdo_arrive(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    al_iptv_vdo_list_t *p_data = (al_iptv_vdo_list_t *)para1;
    al_iptv_vdo_item_t *p_vdoList;
    u16 *p_name;
    u8  *p_url;
    u16 vdo_cnt;
    u16 i;
    u16 str_len;

    DEBUG(UI_IPTV,INFO,"@@@StaIptv_resource_on_newpagevdo_arrive\n");
    ui_release_vdolist();

    if (p_data->vdo_cnt > 0 && p_data->vdoList != NULL)
    {
        vdo_cnt = MIN(p_data->vdo_cnt, g_pIptv->vdo_page_size);
        p_vdoList = (al_iptv_vdo_item_t *)SY_MALLOC(vdo_cnt * sizeof(al_iptv_vdo_item_t));
        MT_ASSERT(p_vdoList != NULL);
        memset(p_vdoList, 0, vdo_cnt * sizeof(al_iptv_vdo_item_t));

        for (i = 0; i < vdo_cnt; i++)
        {
            DEBUG(UI_IPTV,INFO,"i=%d, @@@vdo_id=%s, res_id=%d\n", i, p_data->vdoList[i].vdo_id.qpId, p_data->vdoList[i].res_id);
            p_vdoList[i].vdo_id = p_data->vdoList[i].vdo_id;
            p_vdoList[i].res_id = p_data->vdoList[i].res_id;
            p_vdoList[i].b_single_page = p_data->vdoList[i].b_single_page;

	     //album
	     p_vdoList[i].album =	p_data->vdoList[i].album;

            if (p_data->vdoList[i].name != NULL)
            {
                str_len = uni_strlen(p_data->vdoList[i].name);
                p_name = (u16 *)SY_MALLOC((str_len + 1) * sizeof(u16));
                MT_ASSERT(p_name != NULL);
                uni_strcpy(p_name, p_data->vdoList[i].name);
                p_vdoList[i].name = p_name;
            }

            if (p_data->vdoList[i].img_url != NULL)
            {
                str_len = strlen(p_data->vdoList[i].img_url);
                p_url = (u8 *)SY_MALLOC((str_len + 1));
                MT_ASSERT(p_url != NULL);
                strcpy(p_url, p_data->vdoList[i].img_url);
                p_vdoList[i].img_url = p_url;
            }
        }
        g_pIptv->vdoList = p_vdoList;
        if (g_pIptv->curVdoListIdx >= vdo_cnt)
        {
            g_pIptv->curVdoListIdx = vdo_cnt - 1;
        }
    }
    else
    {
        vdo_cnt = 0;
        g_pIptv->vdoList = NULL;
        g_pIptv->curVdoListIdx = 0;
    }

    if (g_pIptv->vdo_cnt != vdo_cnt)
    {
        DEBUG(UI_IPTV,INFO,"@@@calced vdo_cnt=%d, gived vdo_cnt=%d\n", g_pIptv->vdo_cnt, vdo_cnt);
        ui_iptv_vdolist_set_count(g_pIptv->hVdoList, vdo_cnt);
    }
    if (g_pIptv->total_vdo != p_data->total_vdo)
    {
        DEBUG(UI_IPTV,INFO,"@@@g_pIptv->total_vdo=%d, p_data->total_vdo=%d\n", g_pIptv->total_vdo, p_data->total_vdo);
        g_pIptv->total_vdo = g_pIptv->resList[g_pIptv->curResListIdx].catList[g_pIptv->curCatListIdx].total_vdo = p_data->total_vdo;
    }
    if (p_data->total_page == 0)
    {
        p_data->total_page = 1;
    }
    
    DEBUG(UI_IPTV,INFO,"@@@g_pIptv->total_page=%d, p_data->total_page=%d\n", g_pIptv->total_page, p_data->total_page);
    g_pIptv->total_page = p_data->total_page;
    g_pIptv->b_totalPageChanged = TRUE;
    ui_iptv_update_page_num(g_pIptv->hPageNum, g_pIptv->curPageNum, g_pIptv->total_page);

    return SID_IPTV_RES_VDOLIST_NORMAL;
}

static STATEID StaIptv_resource_on_get_pagevdo_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    DEBUG(UI_IPTV,INFO,"@@@StaIptv_resource_on_get_pagevdo_fail\n");
    ui_iptv_open_cfm_dlg(ROOT_ID_IPTV, IDS_HD_DATA_ERROR);
    return SID_NULL;
}

static STATEID StaIptv_resource_on_vdoreq_dlg_closed(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    DEBUG(UI_IPTV,INFO,"@@@StaIptv_resource_on_vdoreq_dlg_closed\n");
    return SID_IPTV_RES_VDOLIST_NORMAL;
}

static void SexIptv_ResVdoReq(void)
{
    DEBUG(UI_IPTV,INFO,"@@@SexIptv_ResVdoReq\n");
    ui_comm_dlg_close();
}

static void SenIptv_ResVdolistNormal(void)
{
    DEBUG(UI_IPTV,INFO,"@@@SenIptv_ResVdolistNormal\n");
    if (ui_comm_get_focus_mainwin_id() == ROOT_ID_IPTV)
    {
        if (ui_iptv_vdolist_calc_focus_pos(g_pIptv->hVdoList) != g_pIptv->curVdoListIdx
            && ctrl_is_onfocus(g_pIptv->hVdoList))
        {
            ui_iptv_vdolist_change_focus_pos(g_pIptv->hVdoList, g_pIptv->curVdoListIdx, TRUE);
        }
        ui_iptv_update_vdolist(g_pIptv->hVdoList, 0, g_pIptv->vdo_page_size, 0);

        ctrl_paint_ctrl(g_pIptv->hVdoList, TRUE);
        ctrl_paint_ctrl(g_pIptv->hPageNum, TRUE);
	if(g_pIptv->hFilter != NULL)
	{
		if(ctrl_get_sts(g_pIptv->hFilter) == OBJ_STS_SHOW)
			ctrl_paint_ctrl(g_pIptv->hFilter, TRUE);
	}
        g_pIptv->curVdoListRendIdx = 0;
        ui_iptv_pic_play_start();
    }
}

static STATEID StaIptv_resource_pic_draw_end(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_item, *p_pic;
    DEBUG(UI_IPTV,INFO,"@@@StaIptv_resource_pic_draw_end\n");
    if (ui_comm_get_focus_mainwin_id() == ROOT_ID_IPTV)
    {
        p_item = ctrl_get_child_by_id(g_pIptv->hVdoList, IDC_IPTV_VDOITEM_START + g_pIptv->curVdoListRendIdx);
        p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_VDOITEM_PIC);
        bmap_set_content_by_id(p_pic, RSC_INVALID_ID);

        ctrl_paint_ctrl(p_pic, TRUE);

        pic_stop();
        g_pIptv->curVdoListRendIdx++;
        ui_iptv_pic_play_start();
    }

    return SID_NULL;
}

static STATEID StaIptv_resource_pic_draw_fail(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    DEBUG(UI_IPTV,INFO,"@@@%s\n", __FUNCTION__);
    pic_stop();
    g_pIptv->curVdoListRendIdx++;
    ui_iptv_pic_play_start();

    return SID_NULL;
}

static STATEID StaIptv_resource_on_vdolist_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    ui_iptv_description_param_t p_param;

    DEBUG(UI_IPTV,INFO,"@@@StaIptv_resource_on_vdolist_selected curVdoListIdx=%d\n", g_pIptv->curVdoListIdx);
    if((g_pIptv->vdoList)&&(get_iptv_des_state() == IPTV_DESC_DESTORY))
    {
        ui_iptv_pic_deinit();

        p_param.vdo_id = g_pIptv->vdoList[g_pIptv->curVdoListIdx].vdo_id;
        p_param.res_id = g_pIptv->vdoList[g_pIptv->curVdoListIdx].res_id;
        p_param.b_single_page = g_pIptv->vdoList[g_pIptv->curVdoListIdx].b_single_page;
	 	p_param.album = g_pIptv->vdoList[g_pIptv->curVdoListIdx].album;
        DEBUG(UI_IPTV,INFO,"@@@vdo_id=%s, res_id=%d, b_single_page=%d album[%x]\n", 
				p_param.vdo_id.qpId, p_param.res_id, p_param.b_single_page,(u32)p_param.album);
        manage_open_menu(ROOT_ID_IPTV_DESCRIPTION, (u32)&p_param, (u32)ROOT_ID_IPTV);
    }

    return SID_NULL;
}

static STATEID StaIptv_resource_on_input_number(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    DEBUG(UI_IPTV,INFO,"@@@%s\n", __FUNCTION__);
    if (ctrl_is_onfocus(g_pIptv->hVdoList))
    {
        g_pIptv->jumpPageNum = para1;
        if (g_pIptv->jumpPageNum > g_pIptv->total_page)
        {
            g_pIptv->jumpPageNum = g_pIptv->total_page;
        }
        ui_iptv_update_page_num(g_pIptv->hPageNum, 
                                                            g_pIptv->jumpPageNum, 
                                                            g_pIptv->total_page);
        ctrl_change_focus(g_pIptv->hVdoList, g_pIptv->hPageNum);
    }
    else
    {
        if (g_pIptv->jumpPageNum < g_pIptv->total_page)
        {
            g_pIptv->jumpPageNum = g_pIptv->jumpPageNum * 10 + para1;
        }
        else
        {
            g_pIptv->jumpPageNum = para1;
        }
        if (g_pIptv->jumpPageNum > g_pIptv->total_page)
        {
            g_pIptv->jumpPageNum = g_pIptv->total_page;
        }
        ui_iptv_update_page_num(g_pIptv->hPageNum, 
                                                            g_pIptv->jumpPageNum, 
                                                            g_pIptv->total_page);
        ctrl_paint_ctrl(g_pIptv->hPageNum, TRUE);
    }

    return SID_NULL;
}

static STATEID StaIptv_resource_on_page_num_selected(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    DEBUG(UI_IPTV,INFO,"@@@%s\n", __FUNCTION__);
    ctrl_change_focus(g_pIptv->hPageNum, g_pIptv->hVdoList);

    if (g_pIptv->jumpPageNum == 0)
    {
       g_pIptv->jumpPageNum = 1;
       ui_iptv_update_page_num(g_pIptv->hPageNum, 
                                                   g_pIptv->jumpPageNum, 
                                                   g_pIptv->total_page);
       ctrl_paint_ctrl(g_pIptv->hPageNum, TRUE);
    }
    g_pIptv->curPageNum = g_pIptv->jumpPageNum;

    return SID_NULL;
}

static STATEID StaIptv_resource_on_page_num_exit(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    DEBUG(UI_IPTV,INFO,"@@@%s\n", __FUNCTION__);
    if (g_pIptv->curPageNum != g_pIptv->jumpPageNum)
    {
        ui_iptv_update_page_num(g_pIptv->hPageNum, 
                                                            g_pIptv->curPageNum, 
                                                            g_pIptv->total_page);
    }

    ctrl_change_focus(g_pIptv->hPageNum, g_pIptv->hVdoList);

    return SID_NULL;
}

static void SexIptv_ResVdolistNormal(void)
{
    DEBUG(UI_IPTV,INFO,"@@@SexIptv_ResVdolistNormal\n");
    pic_stop();
}

static void SenIptv_InitFailed(void)
{
    DEBUG(UI_IPTV,INFO,"@@@SenIptv_InitFailed\n");
    ui_iptv_open_cfm_dlg(ROOT_ID_IPTV, IDS_HD_NETWORK_UNAVAILABLE);
}

static void SexIptv_InitFailed(void)
{
    DEBUG(UI_IPTV,INFO,"@@@SexIptv_InitFailed\n");
    ui_comm_dlg_close();
}

static void SenIptv_Deinit(void)
{
    DEBUG(UI_IPTV,INFO,"@@@%s\n", __FUNCTION__);
    ui_iptv_dp_deinit();

    ui_iptv_open_dlg(ROOT_ID_IPTV, IDS_NET_EXITING);
}

static STATEID StaIptv_deinit_on_deinit_success(control_t *ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root;

    DEBUG(UI_IPTV,INFO,"@@@%s\n", __FUNCTION__);
    p_root = fw_find_root_by_id(ROOT_ID_IPTV);
    if (p_root)
    {
        ui_comm_dlg_close();
        
        ui_iptv_dp_del();
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_EXIT, 0, 0);
    }

    return SID_NULL;
}

static void SexIptv_Deinit(void)
{
    DEBUG(UI_IPTV,INFO,"@@@%s\n", __FUNCTION__);
}

static RET_CODE ui_iptv_update_filter(control_t *p_filter, u16 start, u16 size, u32 context)
{
	DEBUG(UI_IPTV,INFO,"@@@@@@%s \n",__FUNCTION__);
	control_t *p_child=NULL,*p_text=NULL,*p_list=NULL;
	u16 i,j;
	u16 uni_str[32];

	for(i = 0 ;i < g_pIptv->resList[g_pIptv->curResListIdx].total_filter ;i++)
	{
		p_child = ctrl_get_child_by_id(p_filter, IDC_IPTV_FILTER_START + i);
		p_text = ctrl_get_child_by_id(p_child, IDC_IPTV_FILTER_TEXT);
		p_list = ctrl_get_child_by_id(p_child, IDC_IPTV_FILTER_LIST);

		memset(uni_str, 0, sizeof(uni_str));
		uni_strcat(uni_str,(u16 *)"*",32);
		uni_strcat(uni_str,g_pIptv->resList[g_pIptv->curResListIdx].filter[i].name,32);
		uni_strcat(uni_str,(u16 *)"*",32);
		text_set_content_by_unistr(p_text, uni_str);

		for(j=0;j<g_pIptv->resList[g_pIptv->curResListIdx].filter[i].subTagSize;j++)
		{
			list_set_field_content_by_unistr(p_list, j, 0, g_pIptv->resList[g_pIptv->curResListIdx].filter[i].subtag[j].name);
		}
	}

	return SUCCESS;
}

static RET_CODE ui_iptv_update_cur_filter(control_t *p_list, u16 start, u16 size, u32 context)
{
	DEBUG(UI_IPTV,INFO,"@@@@@@%s \n",__FUNCTION__);
	control_t *p_parent = NULL;
	u16 j;
	u16 ctrl_id, focus;

	p_parent = ctrl_get_parent(p_list);
	ctrl_id = ctrl_get_ctrl_id(p_parent);
	focus = ctrl_id - IDC_IPTV_FILTER_START;

	for(j=0;j<(start + size) && (j<g_pIptv->resList[g_pIptv->curResListIdx].filter[focus].subTagSize);j++)
	{
		list_set_field_content_by_unistr(p_list, j, 0, g_pIptv->resList[g_pIptv->curResListIdx].filter[focus].subtag[j].name);
	}

	return SUCCESS;
}

static control_t *create_iptv_filter_filter(control_t *p_parent,u16 id,u16 y)
{
	control_t *p_cont = NULL,*p_text = NULL,*p_list = NULL;
	u16 i;

	//DEBUG(UI_IPTV,INFO,"@@@@%s %d \n",__FUNCTION__,__LINE__);
	p_cont = ctrl_create_ctrl(CTRL_CONT, id,
			IPTV_FILTER_X, y,IPTV_FILTER_W,IPTV_FILTER_TEXT_H,p_parent, 0);

	p_text = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_FILTER_TEXT,
			IPTV_FILTER_TEXT_X, IPTV_FILTER_TEXT_Y,
			IPTV_FILTER_TEXT_W, IPTV_FILTER_TEXT_H,
			p_cont, 0);
	text_set_align_type(p_text, STL_RIGHT| STL_VCENTER);
	text_set_font_style(p_text, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
	text_set_content_type(p_text, TEXT_STRTYPE_UNICODE);

	p_list = ctrl_create_ctrl(CTRL_LIST, IDC_IPTV_FILTER_LIST, IPTV_FILTER_LIST_X,
			IPTV_FILTER_LIST_Y, IPTV_FILTER_LIST_W, IPTV_FILTER_LIST_H, p_cont, 0);
	ctrl_set_rstyle(p_list, RSI_LEFT_CONT_BG , RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);

	list_enable_cycle_mode(p_list, FALSE);
	list_set_item_interval(p_list, 0);
	list_set_item_rstyle(p_list, &filterlist_item_rstyle);
	list_enable_select_mode(p_list, TRUE);
	list_set_select_mode(p_list, LIST_SINGLE_SELECT);
	list_set_count(p_list, IPTV_FILTER_LIST_COUNT, IPTV_FILTER_LIST_PAGE);
	list_set_field_count(p_list, IPTV_FILTER_LIST_FIELD, IPTV_FILTER_LIST_PAGE);
	list_set_columns(p_list, IPTV_FILTER_LIST_PAGE, TRUE);
	list_set_focus_pos(p_list, 0);
	list_select_item(p_list, 0);
	list_set_update(p_list, ui_iptv_update_cur_filter, 0);

	for (i = 0; i < ARRAY_SIZE(filterlist_attr); i++)
	{
		list_set_field_attr(p_list, (u8)i, filterlist_attr[i].attr,
				filterlist_attr[i].width, filterlist_attr[i].left, filterlist_attr[i].top);
		list_set_field_rect_style(p_list, (u8)i, filterlist_attr[i].rstyle);
		list_set_field_font_style(p_list, (u8)i, filterlist_attr[i].fstyle);
	}

	return p_cont;
}

static control_t *create_iptv_filter(control_t *ctrl)
{
    u16 filter_with,i;
    static u16 curResId;
    control_t *p_filter_cont = NULL, *p_cont = NULL,*p_button = NULL;

    DEBUG(UI_IPTV,INFO,"@@@%s %d \n",__FUNCTION__,__LINE__);
    p_cont = ctrl_get_parent(ctrl_get_parent(ctrl));
    p_filter_cont = ctrl_get_child_by_id(p_cont, IDC_IPTV_FILTER_CONT);
    if(NULL != p_filter_cont)
    {
	if(curResId == g_pIptv->curResListIdx)
	{
		ctrl_set_sts(p_filter_cont,OBJ_STS_SHOW);
		return p_filter_cont;
	}
	else
	{
		curResId = g_pIptv->curResListIdx;
		ctrl_destroy_child_by_id(p_cont,IDC_IPTV_FILTER_CONT);
	}
    }

    filter_with = IPTV_FILTER_TEXT_H*(g_pIptv->resList[g_pIptv->curResListIdx].total_filter + 2);
    p_filter_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_FILTER_CONT,
		    IPTV_FILTER_X, (COMM_BG_H - filter_with),
		    IPTV_FILTER_W, filter_with,
		    p_cont, 0);
    ctrl_set_rstyle(p_filter_cont, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);
    ctrl_set_keymap(p_filter_cont, iptv_filter_filter_keymap);
    ctrl_set_proc(p_filter_cont, iptv_filter_filter_proc);
    g_pIptv->hFilter = p_filter_cont;

    for(i=0; i<g_pIptv->resList[g_pIptv->curResListIdx].total_filter;i++)
    {
	    create_iptv_filter_filter(p_filter_cont,IDC_IPTV_FILTER_START+i,(IPTV_FILTER_TEXT_H/2 + IPTV_FILTER_TEXT_H*i));
    }

    p_button = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_FILTER_START+g_pIptv->resList[g_pIptv->curResListIdx].total_filter,
    	69, (IPTV_FILTER_TEXT_H/2 + IPTV_FILTER_TEXT_H*(g_pIptv->resList[g_pIptv->curResListIdx].total_filter)),
    	1120, IPTV_FILTER_TEXT_H,
    	p_filter_cont, 0);
    ctrl_set_rstyle(p_button,  RSI_PBACK, RSI_VOD_LIST2_F, RSI_PBACK);
    text_set_align_type(p_button, STL_CENTER| STL_VCENTER);
    text_set_font_style(p_button, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
    text_set_content_type(p_button, TEXT_STRTYPE_STRID);
    text_set_content_by_strid(p_button,  IDS_HD_SEARCH);
    
    ui_iptv_update_filter(p_filter_cont, 0,0,0);

    return p_filter_cont;
}

/*================================================================================================
                           iptv public works function
 ================================================================================================*/

/*!
 * Video view entry
 */
RET_CODE ui_open_iptv(u32 para1, u32 para2)
{
    control_t *p_cont,*p_ctrl;
    control_t *p_catlist_cont, *p_title_name, *p_catlist;
    control_t *p_res_cont, *p_reslist;
    control_t *p_client_cont, *p_vdolist;
    control_t *p_page_cont, *p_page_num;
    u16 i = 0;
   // u8 mac[6] = {0x11,0x22,0x33,0x44,0x55,0x66};

    DEBUG(UI_IPTV,INFO,"@@@ui_open_iptv\n");
	#ifdef ENABLE_ADS
	pic_adv_stop();
	ui_adv_pic_release();
	DEBUG(ADS,INFO,"\n");
	#endif
   
    if(ui_is_playing())
    {
  	  ui_stop_play(STOP_PLAY_BLACK, TRUE);
    }
    ui_iqy_video_c_create(VIDEO_PLAY_FUNC_ONLINE);
    
    ui_init_app_data();
    ui_iptv_register_msg();
    ui_iptv_pic_init();
   // NC_AuthInit("87079770", mac, 6);
  //  NC_AuthStart();

    /*!
     * Create Menu
     */
    p_cont = ui_comm_root_create(ROOT_ID_IPTV, 0,
                                    COMM_BG_X, COMM_BG_Y,
                                    COMM_BG_W, COMM_BG_H,
                                    0, 0);
    MT_ASSERT(p_cont != NULL);

    ctrl_set_keymap(p_cont, iptv_cont_keymap);
    ctrl_set_proc(p_cont, iptv_cont_proc);

    p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_IPTV_VDOITEM_PIC_LEFT,
                                IPTV_VDOPIC_LEFT_X, IPTV_VDOPIC_LEFT_Y,
                                IPTV_VDOPIC_LEFT_W, IPTV_VDOPIC_LEFT_H,
                                p_cont, 0);
        ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        bmap_set_content_by_id(p_ctrl, IM_VOD_LEFT);
            p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_IPTV_VDOITEM_PIC_RIGHT,
                                IPTV_VDOPIC_RIGHT_X, IPTV_VDOPIC_RIGHT_Y,
                                IPTV_VDOPIC_RIGHT_W, IPTV_VDOPIC_RIGHT_H,
                                p_cont, 0);
        ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        bmap_set_content_by_id(p_ctrl, IM_VOD_RIGHT);
            p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_IPTV_VDOITEM_PIC_LINE,
                                IPTV_VDOPIC_LINE_X, IPTV_VDOPIC_LINE_Y,
                                IPTV_VDOPIC_LINE_W, IPTV_VDOPIC_LINE_H,
                                p_cont, 0);
        ctrl_set_rstyle(p_ctrl, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        bmap_set_content_by_id(p_ctrl, IM_VOD_LINE);

    /*!
     * Create catlist container
     */
    p_catlist_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_CATLIST_CONT,
                                  IPTV_CATLIST_CONTX, IPTV_CATLIST_CONTY,
                                  IPTV_CATLIST_CONTW, IPTV_CATLIST_CONTH,
                                  p_cont, 0);
    ctrl_set_rstyle(p_catlist_cont, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);

    // catlist
    p_catlist = ctrl_create_ctrl(CTRL_LIST, IDC_IPTV_CATLIST,
                          IPTV_CATLIST_X, IPTV_CATLIST_Y,
                          IPTV_CATLIST_W, IPTV_CATLIST_H,
                          p_catlist_cont, 0);
    ctrl_set_rstyle(p_catlist, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_catlist, iptv_catlist_keymap);
    ctrl_set_proc(p_catlist, iptv_catlist_proc);

    ctrl_set_mrect(p_catlist,
                  IPTV_CATLIST_MIDL, IPTV_CATLIST_MIDT,
                  IPTV_CATLIST_MIDR, IPTV_CATLIST_MIDB);
    //list_set_item_interval(p_catlist, IPTV_CATLIST_ITEM_V_GAP);
    list_set_item_rstyle(p_catlist, &catlist_item_rstyle);

    list_enable_cycle_mode(p_catlist, FALSE);
    list_enable_select_mode(p_catlist, TRUE);
    list_set_select_mode(p_catlist, LIST_SINGLE_SELECT);

    list_set_count(p_catlist, 0, IPTV_CATLIST_PAGE_SIZE);//should not be removed
    list_set_field_count(p_catlist, ARRAY_SIZE(catlist_attr), IPTV_CATLIST_PAGE_SIZE);
    list_set_columns(p_catlist, IPTV_CATLIST_PAGE_SIZE, TRUE);
    list_set_update(p_catlist, ui_iptv_update_catlist, 0);

    for (i = 0; i < ARRAY_SIZE(catlist_attr); i++)
    {
        list_set_field_attr(p_catlist, (u8)i, catlist_attr[i].attr,
                            catlist_attr[i].width, catlist_attr[i].left, catlist_attr[i].top);
        list_set_field_rect_style(p_catlist, (u8)i, catlist_attr[i].rstyle);
        list_set_field_font_style(p_catlist, (u8)i, catlist_attr[i].fstyle);
    }
    ctrl_set_sts(p_catlist, OBJ_STS_HIDE);
    g_pIptv->hCatList = p_catlist;

#if 0
    list_set_count(p_catlist, IPTV_LIST_ITEM_COUNT, IPTV_CATLIST_ITEM_PAGE_SIZE);
    ////list_set_focus_pos(p_catlist, g_pIptv->curCatListIdx);
    ////list_select_item(p_catlist, g_pIptv->curCatListIdx);

    iptv_update_catlist(p_catlist, list_get_valid_pos(p_catlist), IPTV_LIST_ITEM_NUM_ONE_PAGE, 0);
#endif

    /*!
     * Create res container
     */
    p_res_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_RESLIST_CONT,
                                  IPTV_RESLIST_CONTX, IPTV_RESLIST_CONTY,
                                  IPTV_RESLIST_CONTW, IPTV_RESLIST_CONTH,
                                  p_cont, 0);
    ctrl_set_rstyle(p_res_cont, RSI_VOD_LIST_BG, RSI_VOD_LIST_BG, RSI_VOD_LIST_BG);

    // Reslist
    p_reslist = ctrl_create_ctrl(CTRL_LIST, IDC_IPTV_RESLIST,
                              IPTV_RESLIST_X, IPTV_RESLIST_Y,
                              IPTV_RESLIST_W, IPTV_RESLIST_H, 
                              p_res_cont, 0);
    ctrl_set_rstyle(p_reslist, RSI_VOD_LIST_BG, RSI_VOD_LIST_BG, RSI_VOD_LIST_BG);
    ctrl_set_keymap(p_reslist, iptv_reslist_keymap);
    ctrl_set_proc(p_reslist, iptv_reslist_proc);

    ctrl_set_mrect(p_reslist,
                  IPTV_RESLIST_MIDL, IPTV_RESLIST_MIDT,
                  IPTV_RESLIST_MIDR, IPTV_RESLIST_MIDB);
    list_set_item_rstyle(p_reslist, &reslist_item_rstyle);

    list_enable_cycle_mode(p_reslist, FALSE);
    list_enable_select_mode(p_reslist, TRUE);
    list_set_select_mode(p_reslist, LIST_SINGLE_SELECT);

    list_set_count(p_reslist, 0, IPTV_RESLIST_PAGE_SIZE);//should not be removed
    list_set_field_count(p_reslist, ARRAY_SIZE(reslist_attr), IPTV_RESLIST_PAGE_SIZE);
    list_set_columns(p_reslist, IPTV_RESLIST_PAGE_SIZE, TRUE);
    list_set_update(p_reslist, ui_iptv_update_reslist, 0);

    for (i = 0; i < ARRAY_SIZE(reslist_attr); i++)
    {
        list_set_field_attr(p_reslist, (u8)i, reslist_attr[i].attr,
                            reslist_attr[i].width, reslist_attr[i].left, reslist_attr[i].top);
        list_set_field_rect_style(p_reslist, (u8)i, reslist_attr[i].rstyle);
        list_set_field_font_style(p_reslist, (u8)i, reslist_attr[i].fstyle);
    }
    ctrl_set_sts(p_reslist, OBJ_STS_HIDE);
    g_pIptv->hResList = p_reslist;

    /*!
     * Create client container
     */
    p_client_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_CLIENT_CONT,
                                  IPTV_CLIENT_CONTX, IPTV_CLIENT_CONTY,
                                  IPTV_CLIENT_CONTW, IPTV_CLIENT_CONTH,
                                  p_cont, 0);
    ctrl_set_rstyle(p_client_cont, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    //Video list
    p_vdolist = ui_iptv_create_vdolist(p_client_cont);
    ctrl_set_keymap(p_vdolist, iptv_vdolist_keymap);
    ctrl_set_proc(p_vdolist, iptv_vdolist_proc);
    ctrl_set_sts(p_vdolist, OBJ_STS_HIDE);
    g_pIptv->hVdoList = p_vdolist;

    /*!
     * Create page container
     */
    p_page_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_PAGE_CONT,
                                    IPTV_PAGE_CONTX, IPTV_PAGE_CONTY,
                                    IPTV_PAGE_CONTW, IPTV_PAGE_CONTH,
                                    p_cont, 0);
    ctrl_set_rstyle(p_page_cont, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG, RSI_LEFT_CONT_BG);

    // title
    p_title_name = ctrl_create_ctrl(CTRL_TEXT, (u8) IDC_IPTV_TITLE_NAME,
                                 IPTV_TITLE_NAME_X, IPTV_TITLE_NAME_Y,
                                 IPTV_TITLE_NAME_W, IPTV_TITLE_NAME_H,
                                 p_page_cont, 0);
    ctrl_set_rstyle(p_title_name, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_title_name, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_title_name, FSI_WHITE_36, FSI_WHITE_36, FSI_WHITE_36);
    text_set_content_type(p_title_name, TEXT_STRTYPE_EXTSTR);
    ctrl_set_sts(p_title_name, OBJ_STS_HIDE);
    g_pIptv->hTitleName = p_title_name;

    p_page_num = ctrl_create_ctrl(CTRL_TEXT, (u8) IDC_IPTV_PAGE_NUM,
                                 IPTV_PAGE_NUMBER_X, IPTV_PAGE_NUMBER_Y,
                                 IPTV_PAGE_NUMBER_W, IPTV_PAGE_NUMBER_H,
                                 p_page_cont, 0);
    ctrl_set_rstyle(p_page_num, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_page_num, iptv_page_num_keymap);
    ctrl_set_proc(p_page_num, iptv_page_num_proc);

    text_set_align_type(p_page_num, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_page_num, FSI_WHITE, FSI_RED, FSI_WHITE);
    text_set_content_type(p_page_num, TEXT_STRTYPE_UNICODE);
    g_pIptv->hPageNum = p_page_num;

    ctrl_default_proc(p_cont, MSG_GETFOCUS, 0, 0);
    ctrl_paint_ctrl(ctrl_get_root(p_cont), TRUE);
//    ui_stop_play(STOP_PLAY_BLACK, TRUE);
///////////////////////////////////////////////////////////////////
    Iptv_OpenStateTree();
    fw_notify_root(fw_find_root_by_id(ROOT_ID_IPTV), NOTIFY_T_MSG, FALSE, MSG_OPEN_IPTV_REQ, 0, 0);

    return SUCCESS;
}

static RET_CODE ui_iptv_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    DEBUG(UI_IPTV,INFO,"@@@ui_iptv_on_destory\n");
	ui_stop_play(STOP_PLAY_BLACK, TRUE);
    Iptv_CloseStateTree();

    ui_iptv_pic_deinit();
    ui_iptv_unregister_msg();
    ui_release_app_data();

    ui_iqy_video_c_destroy();
    ui_play_curn_pg();

    SY_MEM_CHECK();

	#ifdef ENABLE_ADS
	ui_adv_pic_init(PIC_SOURCE_BUF);
	fw_notify_root(fw_find_root_by_id(ROOT_ID_MAINMENU), NOTIFY_T_MSG, TRUE, MSG_REFRESH_ADS_PIC, 0, 0);
	#endif
    return ERR_NOFEATURE;
}

static RET_CODE ui_iptv_on_open_dlg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_IPTV,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };

    DEBUG(UI_IPTV,INFO,"@@@ui_iptv_on_open_dlg\n");

    if (Iptv_IsStateActive(SID_IPTV_ACTIVE))
    {
        dlg_data.content = para1;

        ui_comm_dlg_open(&dlg_data);
    }

    return SUCCESS;
}

static RET_CODE ui_iptv_on_open_cfm_dlg(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_IPTV,
        DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        RSC_INVALID_ID,
        0,
    };

    DEBUG(UI_IPTV,INFO,"@@@ui_iptv_on_open_cfm_dlg\n");

    if (Iptv_IsStateActive(SID_IPTV_ACTIVE))
    {
        dlg_data.content = para1;

        ui_comm_dlg_open(&dlg_data);

        p_root = fw_find_root_by_id(ROOT_ID_IPTV);
        if (p_root)
        {
            fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_CLOSE_CFMDLG_NTF, para1, 0);
        }
    }

    return SUCCESS;
}

void ui_iptv_open_dlg(u8 root_id, u16 str_id)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(root_id);
    if (p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_OPEN_DLG_REQ, str_id, 0);
    }
}

void ui_iptv_open_cfm_dlg(u8 root_id, u16 str_id)
{
    control_t *p_root = NULL;

    p_root = fw_find_root_by_id(root_id);
    if (p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_OPEN_CFMDLG_REQ, str_id, 0);
    }
}

static RET_CODE ui_iptv_on_state_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    RET_CODE ret;

    ret = Iptv_DispatchMsg(p_ctrl, msg, para1, para2);
    if (ret != SUCCESS)
    {
        DEBUG(UI_IPTV,INFO,"@@@@@@%s unaccepted msg, id=0x%04x\n", __FUNCTION__, msg);
    }

    return ret;
}

static RET_CODE ui_iptv_on_key_process(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    Iptv_DispatchMsg(p_ctrl, msg, para1, para2);

    return SUCCESS;
}

static RET_CODE ui_iptv_on_input_number(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)	
{
    para1 = (u32)(msg & MSG_DATA_MASK);
    msg = (msg & MSG_TYPE_MASK);
    Iptv_DispatchMsg(p_ctrl, msg, para1, para2);
    return SUCCESS;
}

static RET_CODE ui_dispatch_iptv_msg(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;

    switch (msg)
    {
        case MSG_IPTV_EVT_INIT_SUCCESS:
        case MSG_IPTV_EVT_INIT_FAIL:
        case MSG_IPTV_EVT_DEINIT_SUCCESS:
        case MSG_IPTV_EVT_NEW_RES_NAME_ARRIVE:
        case MSG_IPTV_EVT_GET_RES_NAME_FAIL:
        case MSG_IPTV_EVT_NEW_RES_CATGRY_ARRIVE:
        case MSG_IPTV_EVT_GET_RES_CATGRY_FAIL:
            ui_iptv_on_state_process(p_ctrl, msg, para1, para2);
            break;

        case MSG_IPTV_EVT_NEW_PAGE_VDO_ARRIVE:
        case MSG_IPTV_EVT_GET_PAGE_VDO_FAIL:
            if (ui_iptv_vdo_idntfy_cmp(para2) == 0)
            {
                ui_iptv_on_state_process(p_ctrl, msg, para1, para2);
            }
            break;

        case MSG_IPTV_EVT_NEW_SEARCH_VDO_ARRIVE:
        case MSG_IPTV_EVT_GET_SEARCH_VDO_FAIL:
            if (ui_iptv_vdo_idntfy_cmp(para2) == 0)
            {
                p_root = fw_find_root_by_id(ROOT_ID_IPTV_SEARCH);
            }
            break;

        case MSG_IPTV_EVT_NEW_VDO_INFO_ARRIVE:
        case MSG_IPTV_EVT_GET_VDO_INFO_FAIL:
        case MSG_IPTV_EVT_NEW_INFO_URL_ARRIVE:
        case MSG_IPTV_EVT_GET_INFO_URL_FAIL:
        case MSG_IPTV_EVT_NEW_RECMND_INFO_ARRIVE:
        case MSG_IPTV_EVT_GET_RECMND_INFO_FAIL:
            p_root = fw_find_root_by_id(ROOT_ID_IPTV_DESCRIPTION);
            break;

        case MSG_IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE:
        case MSG_IPTV_EVT_GET_SOURCE_FORMAT_FAIL:
        case MSG_IPTV_EVT_NEW_PLAY_URL_ARRIVE:
        case MSG_IPTV_EVT_GET_PLAY_URL_FAIL:
            p_root = fw_find_root_by_id(ROOT_ID_IPTV_PLAYER);
            break;

        default:
            break;
    }

    if (p_root)
    {
        fw_notify_root(p_root, NOTIFY_T_MSG, TRUE, msg, para1, para2);
    }

    ui_iptv_free_msg_data(msg, para1, para2);
    
    return SUCCESS;
}

/*!
 * Video key and process
 */
BEGIN_KEYMAP(iptv_reslist_keymap, NULL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(iptv_reslist_keymap, NULL)

BEGIN_MSGPROC(iptv_reslist_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_KEY, ui_iptv_on_key_process)
    ON_COMMAND(MSG_SELECT, ui_iptv_on_key_process)
END_MSGPROC(iptv_reslist_proc, list_class_proc)
BEGIN_KEYMAP(iptv_filter_filter_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
    ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(iptv_filter_filter_keymap, NULL)

BEGIN_MSGPROC(iptv_filter_filter_proc, cont_class_proc)
    ON_COMMAND(MSG_FOCUS_KEY, ui_iptv_on_key_process)
    ON_COMMAND(MSG_SELECT, ui_iptv_on_key_process)
    ON_COMMAND(MSG_EXIT, ui_iptv_on_key_process)
END_MSGPROC(iptv_filter_filter_proc, cont_class_proc)

BEGIN_KEYMAP(iptv_catlist_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(iptv_catlist_keymap, NULL)

BEGIN_MSGPROC(iptv_catlist_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_KEY, ui_iptv_on_key_process)
    ON_COMMAND(MSG_SELECT, ui_iptv_on_key_process)
END_MSGPROC(iptv_catlist_proc, list_class_proc)

BEGIN_KEYMAP(iptv_vdolist_keymap, NULL)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_KEY)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_KEY)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_KEY)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_1, MSG_NUMBER | 1)
    ON_EVENT(V_KEY_2, MSG_NUMBER | 2)
    ON_EVENT(V_KEY_3, MSG_NUMBER | 3)
    ON_EVENT(V_KEY_4, MSG_NUMBER | 4)
    ON_EVENT(V_KEY_5, MSG_NUMBER | 5)
    ON_EVENT(V_KEY_6, MSG_NUMBER | 6)
    ON_EVENT(V_KEY_7, MSG_NUMBER | 7)
    ON_EVENT(V_KEY_8, MSG_NUMBER | 8)
    ON_EVENT(V_KEY_9, MSG_NUMBER | 9)
END_KEYMAP(iptv_vdolist_keymap, NULL)

BEGIN_MSGPROC(iptv_vdolist_proc, cont_class_proc)
    ON_COMMAND(MSG_FOCUS_KEY, ui_iptv_on_key_process)
    ON_COMMAND(MSG_PAGE_KEY, ui_iptv_on_key_process)
    ON_COMMAND(MSG_SELECT, ui_iptv_on_key_process)
    ON_COMMAND(MSG_NUMBER, ui_iptv_on_input_number)
END_MSGPROC(iptv_vdolist_proc, cont_class_proc)

BEGIN_KEYMAP(iptv_page_num_keymap, NULL)
    ON_EVENT(V_KEY_0, MSG_NUMBER | 0)
    ON_EVENT(V_KEY_1, MSG_NUMBER | 1)
    ON_EVENT(V_KEY_2, MSG_NUMBER | 2)
    ON_EVENT(V_KEY_3, MSG_NUMBER | 3)
    ON_EVENT(V_KEY_4, MSG_NUMBER | 4)
    ON_EVENT(V_KEY_5, MSG_NUMBER | 5)
    ON_EVENT(V_KEY_6, MSG_NUMBER | 6)
    ON_EVENT(V_KEY_7, MSG_NUMBER | 7)
    ON_EVENT(V_KEY_8, MSG_NUMBER | 8)
    ON_EVENT(V_KEY_9, MSG_NUMBER | 9)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_UP, MSG_EXIT)
    ON_EVENT(V_KEY_DOWN, MSG_EXIT)
    ON_EVENT(V_KEY_LEFT, MSG_EXIT)
    ON_EVENT(V_KEY_RIGHT, MSG_EXIT)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(iptv_page_num_keymap, NULL)

BEGIN_MSGPROC(iptv_page_num_proc, text_class_proc)
    ON_COMMAND(MSG_NUMBER, ui_iptv_on_input_number)
    ON_COMMAND(MSG_SELECT, ui_iptv_on_key_process)
    ON_COMMAND(MSG_EXIT, ui_iptv_on_key_process)
END_MSGPROC(iptv_page_num_proc, text_class_proc)

BEGIN_KEYMAP(iptv_cont_keymap, ui_comm_root_keymap)
    ON_EVENT(V_KEY_INFO, MSG_INFO)
END_KEYMAP(iptv_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(iptv_cont_proc, ui_comm_root_proc)
    ON_COMMAND(MSG_PIC_EVT_DRAW_END, ui_iptv_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_TOO_LARGE, ui_iptv_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_UNSUPPORT, ui_iptv_on_state_process)
    ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, ui_iptv_on_state_process)

    ON_COMMAND(MSG_IPTV_EVT_INIT_SUCCESS, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_INIT_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_DEINIT_SUCCESS, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_RES_NAME_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_RES_NAME_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_RES_CATGRY_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_RES_CATGRY_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_PAGE_VDO_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_PAGE_VDO_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_SEARCH_VDO_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_SEARCH_VDO_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_VDO_INFO_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_VDO_INFO_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_INFO_URL_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_INFO_URL_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_PLAY_URL_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_PLAY_URL_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_SOURCE_FORMAT_FAIL, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_NEW_RECMND_INFO_ARRIVE, ui_dispatch_iptv_msg)
    ON_COMMAND(MSG_IPTV_EVT_GET_RECMND_INFO_FAIL, ui_dispatch_iptv_msg)

    ON_COMMAND(MSG_OPEN_IPTV_REQ, ui_iptv_on_state_process)
    ON_COMMAND(MSG_SAVE, ui_iptv_on_state_process)
    ON_COMMAND(MSG_INTERNET_PLUG_OUT, ui_iptv_on_state_process)
    ON_COMMAND(MSG_EXIT, ui_iptv_on_key_process)
    ON_COMMAND(MSG_INFO, ui_iptv_on_key_process)
    ON_COMMAND(MSG_DESTROY, ui_iptv_on_destory)

    ON_COMMAND(MSG_OPEN_DLG_REQ, ui_iptv_on_open_dlg)
    ON_COMMAND(MSG_OPEN_CFMDLG_REQ, ui_iptv_on_open_cfm_dlg)
    ON_COMMAND(MSG_CLOSE_CFMDLG_NTF, ui_iptv_on_state_process)
    ON_COMMAND(MSG_UPDATE, ui_iptv_on_state_process)

END_MSGPROC(iptv_cont_proc, ui_comm_root_proc)
#endif


