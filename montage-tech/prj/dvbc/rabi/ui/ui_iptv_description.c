/******************************************************************************/
/******************************************************************************/
/*!
 Include files
 */
#include "ui_common.h"
#include "commonData.h"
#include "IIPTVDataProvider.h"
#include "ui_iptv_player.h"
#include "ui_iptv_description.h"
#include "ui_iptv_api.h"
#include "ui_iptv_prot.h"

 #include "ui_video_iqy.h"
 #include "sys_dbg.h"

 
typedef enum
{
  IDC_LIVE_TV_ROOT_CONT = 1,
  IDC_IPTV_DESC_CONT,
  IDC_IPTV_DESC_TITLE,
  IDC_IPTV_DESC_SCORE,
  IDC_IPTV_DESC_TXT_SCORE,
  IDC_IPTV_DESC_PICTURE,
  IDC_IPTV_DESCNT,
  IDC_IPTV_DESCNT_VDO_SERIES,
  IDC_IPTV_DESCNT_VDO_TXT_SERIES,
  IDC_IPTV_DESCNT_VDO_ATTR,
  IDC_IPTV_DESCNT_VDO_TXT_ATTR,
  IDC_IPTV_DESCNT_VDO_DIRECTER,
  IDC_IPTV_DESCNT_VDO_TXT_DIRECTER,
  IDC_IPTV_DESCNT_VDO_ACTOR,
  IDC_IPTV_DESCNT_VDO_TXT_ACTOR,
  IDC_IPTV_DESCNT_VDO_YEAR,
  IDC_IPTV_DESCNT_VDO_TXT_YEAR,
  IDC_IPTV_DESCNT_TXT_TITLE,
  IDC_IPTV_DESCNT_TXT,
  IDC_IPTV_DESC_VIDEO_RSC_MBOX,
  IDC_IPTV_DESC_VIDEO_RSC_TXT_LIST,
  IDC_IPTV_DESC_SERIES_NUM_MBOX,
  IDC_IPTV_DESC_SERIES_MBOX,
  IDC_IPTV_DESC_RECOM_INFO_MBOX,
  IDC_IPTV_DESC_MOVIE_PLAY_MBOX,
  IDC_IPTV_DESC_RECOM_INFO_CONT,
  IDC_IPTV_DESC_RECOM_INFO_START,
  IDC_IPTV_DESC_RECOM_INFO_END = IDC_IPTV_DESC_RECOM_INFO_START + IPTV_DESC_RECOMM_INFO_NUM -1,
  IDC_IPTV_DESC_RECOM_INFO_PIC,
  IDC_IPTV_DESC_RECOM_INFO_NAME,
  IDC_IPTV_DES_VARIETY_TENT,
}ui_live_tv_ctrl_id_t;

typedef enum
{
   IDC_IPTV_DESCNT_MOVIE_ACTOR = IDC_IPTV_DES_VARIETY_TENT + 1,
   IDC_IPTV_DESCNT_MOVIE_TXT_ACTOR,
   IDC_IPTV_DESCNT_MOVIE_DIRECTER,
   IDC_IPTV_DESCNT_MOVIE_TXT_DIRECTER,
   IDC_IPTV_DESCNT_MOVIE_YEAR,
   IDC_IPTV_DESCNT_MOVIE_TXT_YEAR,
   IDC_IPTV_DESCNT_MOVIE_DURATION,
   IDC_IPTV_DESCNT_MOVIE_TXT_DURATION,
   IDC_IPTV_DESCNT_MOVIE_TXT_TITLE,
   IDC_IPTV_DESCNT_MOVIE_TXT,
   IDC_IPTV_DESCNT_MOVIE_RSC_MBOX,
}ui_movie_ctrl_id_t;

typedef enum
{


   IDC_IPTV_DESCNT_VARIETY_LIST = IDC_IPTV_DESCNT_MOVIE_RSC_MBOX + 1,
   //IDC_IPTV_DESCNT_VARIETY_SCOLL,
   IDC_IPTV_DESCNT_VARIETY_RSC_NUM_TEXT,
   IDC_IPTV_DESCNT_VARIETY_RSC_NUM_DYN, 
   IDC_IPTV_DESCNT_VARIETY_DES_TXT,
   IDC_IPTV_DESCNT_VARIETY_DES_TXT_DYN,
   IDC_IPTV_DESCNT_VARIETY_RSC_MBOX,
   IDC_IPTV_DESCNT_VARIETY_RSC_TXT_LIST,
}ui_variety_ctrl_id_t;

typedef struct{
      VDO_ID_t id;
      u16 title[DB_DVBS_MAX_NAME_LENGTH * 2];
      char img[IPTV_URL_LEN_MAX];
}iptv_recmd_channel;

typedef struct
{
   int number;
   iptv_recmd_channel *recmmd;
}iptv_recomand_info_t;

typedef struct
{
  u32 iptv_ctrl_id;
  u8 *iptv_char[8];
}iptv_ctrl_info_t;


typedef enum
{
   IPTV_INFO_URL_START,
   IPTV_INFO_URL_ARRIVE,
   IPTV_INFO_URL_CONTINUE,
   IPTV_INFO_URL_END,
}ui_iptv_info_url_t;

typedef struct 
{
  u8 g_parent_root_id; 
  u8 category;
  u8 url_arrive_semph;
  u16 episode_num;
  u16 total_episode;
  u16 rsc_uni_str[IPTV_ORIGINS_MAX][8];
  u16 *iptv_area;
  u16 *iptv_fav_name;
  u8 *p_img_url;
  u8 *p_iptv_img_less;
  
  u8  *tvQid;//iqy used to play
  u8  *vid;	// ..
  
  u16 *url_start;
  VDO_ID_t video_id;
  u32 category_id;
  u32 g_series_cnt;
  u32 rec_pic_index;
  u32 identify;
  u32 play_time;
  u32  iptv_fav_focus;
  BOOL playbar_is_back;
  BOOL iptv_is_fav;
  BOOL iptv_entf;
  BOOL iptv_url_c;
  u8 b_single_page;

  u8 total_format;
  u8 format_index;
  int format_list[IPTV_FORMAT_MAX];
  
  iptv_recomand_info_t *p_recomm_data;
  al_iptv_play_origin_info_t *p_play_data;
  ui_iptv_info_url_t *url_state;
}ui_iptv_dat_t;
static ui_iptv_dat_t *ui_iptv_dat;
static u8 g_iptv_desc_state = IPTV_DESC_DESTORY;


static iptv_ctrl_info_t iptv_ctrl_info[] = 
{
    {(u32)IDC_IPTV_DESCNT_VDO_ACTOR, {"主演:"}},
    {(u32)IDC_IPTV_DESCNT_VDO_YEAR ,{"上映:"}},
    {(u32)IDC_IPTV_DESCNT_VDO_ATTR,{"类型:"}},
    {(u32)IDC_IPTV_DESCNT_VDO_DIRECTER,{"导演:"}},
    {(u32)IDC_IPTV_DESCNT_VDO_SERIES,{"集数:"}},
    {(u32)IDC_IPTV_DESCNT_TXT_TITLE ,{"简介:"}},
    {(u32)IDC_IPTV_DESC_SCORE,{"评分:"}},
    {(u32)IDC_IPTV_DESC_VIDEO_RSC_MBOX,{"播放","收藏","取消收藏","收藏数量已满!"}},
    {(u32)IDC_IPTV_DESCNT_MOVIE_ACTOR,{"主演:"}},
    {(u32)IDC_IPTV_DESCNT_MOVIE_DIRECTER ,{"导演:"}},
    {(u32)IDC_IPTV_DESCNT_MOVIE_YEAR,{"上映:"}},
    {(u32)IDC_IPTV_DESCNT_MOVIE_DURATION,{"时长:"}},
    {(u32)IDC_IPTV_DESCNT_MOVIE_TXT_TITLE,{"简介:"}},
    {(u32)IDC_IPTV_DESCNT_MOVIE_RSC_MBOX,{"播放","收藏","取消收藏","收藏数量已满!"}},
    {(u32)IDC_IPTV_DESCNT_VARIETY_DES_TXT,{"简介:"}},
    {(u32)IDC_IPTV_DESCNT_VARIETY_RSC_MBOX,{"收藏","取消收藏","收藏数量已满!"}},
    {(u32)IDC_IPTV_DESCNT_VARIETY_RSC_NUM_TEXT,{"资源数:"}}
};

static BOOL g_draw_rec = FALSE;

extern iconv_t g_cd_gb2312_to_utf16le;
/*!
 * Function define
 */
u16 iptv_des_cont_keymap(u16 key);
u16 iptv_des_video_rsc_keymap(u16 key);
u16 iptv_des_mbox_keymap(u16 key);
u16 iptv_des_video_rsc_list_keymap(u16 key);
u16 iptv_des_series_num_keymap(u16 key);
u16 iptv_desc_recomm_info_keymap(u16 key);
u16 iptv_des_variety_rsc_keymap(u16 key);
u16 iptv_recomm_keymap(u16 key);
u16 iptv_des_variety_list_keymap(u16 key);

RET_CODE iptv_des_cont_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE iptv_des_video_rsc_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE iptv_des_mbox_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE iptv_des_series_num_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE iptv_desc_recomm_info_proc(control_t *p_ctrl, u16 msg, u32 para1, u32 para2);
RET_CODE iptv_des_video_rsc_list_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE iptv_recomm_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE iptv_des_variety_rsc_proc(control_t *p_cont, u16 msg, u32 para1, u32 para2);
RET_CODE iptv_des_variety_list_proc(control_t *cont, u16 msg, u32 para1,u32 para2);


/*************************create UI state common functions***********************/
void set_iptv_des_state(u8 state)
{
	DEBUG(UI_IPLAY_DESC,INFO,"g_iptv_desc_state = %d\n",g_iptv_desc_state);
	g_iptv_desc_state = state;
}

u8 get_iptv_des_state(void)
{
	DEBUG(UI_IPLAY_DESC,INFO,"g_iptv_desc_state = %d\n",g_iptv_desc_state);
	return g_iptv_desc_state;
}

/********************************************************************
*************************create UI controls functions***********************
*********************************************************************/


/*************************create UI common functions***********************/

static RET_CODE on_ui_iptv_init_data(void)
{
   if(ui_iptv_dat == NULL)
   {
     ui_iptv_dat = SY_MALLOC(sizeof(ui_iptv_dat_t));
     MT_ASSERT(ui_iptv_dat != NULL);
     memset(ui_iptv_dat,0,sizeof(ui_iptv_dat_t));

     ui_iptv_dat->iptv_area = SY_MALLOC((DB_DVBS_MAX_NAME_LENGTH + 1)* sizeof(u16));
     MT_ASSERT(ui_iptv_dat->iptv_area != NULL);
     memset(ui_iptv_dat->iptv_area,0,(DB_DVBS_MAX_NAME_LENGTH + 1)* sizeof(u16));

     ui_iptv_dat->url_start = SY_MALLOC(IPTV_ORIGINS_MAX * sizeof(u16));
     MT_ASSERT(ui_iptv_dat->url_start != NULL);
     memset(ui_iptv_dat->url_start,0,IPTV_ORIGINS_MAX * sizeof(u16));

     ui_iptv_dat->url_state = SY_MALLOC(IPTV_ORIGINS_MAX * sizeof(ui_iptv_info_url_t));
     MT_ASSERT(ui_iptv_dat->url_state != NULL);
     memset(ui_iptv_dat->url_state,0,IPTV_ORIGINS_MAX * sizeof(ui_iptv_info_url_t));
	 
	 ui_iptv_dat->iptv_fav_name = SY_MALLOC((DB_DVBS_MAX_NAME_LENGTH * 2)* sizeof(u16));
    MT_ASSERT(ui_iptv_dat->iptv_fav_name != NULL);
    memset(ui_iptv_dat->iptv_fav_name,0,(DB_DVBS_MAX_NAME_LENGTH * 2)* sizeof(u16));
   }
   return SUCCESS;
}
static RET_CODE on_ui_iptv_release_data(void)
{
   DEBUG(UI_IPLAY_DESC,INFO,"iptv dat = %x\n",ui_iptv_dat);
   
   if(ui_iptv_dat != NULL)
   {
     if(ui_iptv_dat->iptv_area != NULL)
     {
        SY_FREE(ui_iptv_dat->iptv_area);
     }
    if(ui_iptv_dat->url_start != NULL)
    {
        SY_FREE(ui_iptv_dat->url_start);
    }
    if(ui_iptv_dat->url_state != NULL)
    {
        SY_FREE(ui_iptv_dat->url_state);
    }
     if(ui_iptv_dat->iptv_fav_name != NULL)
     {
        SY_FREE(ui_iptv_dat->iptv_fav_name);
        ui_iptv_dat->iptv_fav_name = NULL;
     }
     SY_FREE(ui_iptv_dat);
     ui_iptv_dat = NULL;
   }
   return SUCCESS;
}

static u32  on_ui_iptv_get_play_data(void)
{
  return (u32)ui_iptv_dat->p_play_data;
}







#if 0
static void replace_info(void * p_des, const void *p_src ,u32 rel)
{
  MT_ASSERT(p_des != NULL);
  MT_ASSERT(p_src != NULL);
  memcpy(p_des, p_src, rel);
}
static int ui_compare_data_info_for_sort_collect(const void * p_para1, const void *p_para2)
{
  al_iptv_play_origin_item_t *p_g1 = (al_iptv_play_origin_item_t *)p_para1;
  al_iptv_play_origin_item_t *p_g2 = (al_iptv_play_origin_item_t *)p_para2;
  MT_ASSERT(p_g1 != NULL);
  MT_ASSERT(p_g2 != NULL);
   return (atoi(p_g1->collect) < atoi(p_g2->collect));
}
static void shell_sort(void *arr, 
                   u32  num,
                   u32  wid,
                   void (*replace)(void * ,const void *, u32 ),
                   int  (*compare)(const void * , const void *))
{
        u32 i = 0,total = num;
        s32 j = 0;
        void *p_temp = SY_MALLOC(wid);
        do
        {
        num = num/3 + 1;
        for(i = num;i < total;i++)
         if(compare((void *)((u8 *)arr + (i * wid)), (void *)((u8 *)arr + ( (i -num)* wid))))
          {
                replace(p_temp,(void *)((u8 *)arr + (i * wid)),wid);
                j = i -num;
                do
                {
                    replace((void *)((u8 *)arr + ((j +num) * wid)),(void *)((u8 *)arr + (j * wid)),wid);
                    j = j -num;
                }while(j > 0 && compare(p_temp,(void *)((u8 *)arr + (j * wid))));
                replace((void *)((u8 *)arr + ((j +num) * wid)),p_temp,wid);
          }
        }while (num > 1);
        SY_FREE(p_temp);
        return;
}
#endif
static RET_CODE on_ui_iptv_find_dis_str(u32 IDC,u8 cnt,u16 *uni_str,u8 size_str)
{ 
   u32 i = 0;
   u8 idc_str_count  = sizeof(iptv_ctrl_info)/sizeof(iptv_ctrl_info_t);
   for(i =0; i < idc_str_count;i++)
   {
     if(IDC == iptv_ctrl_info[i].iptv_ctrl_id)
     {
        if(iptv_ctrl_info[i].iptv_char[cnt] == NULL)
        {
        DEBUG(UI_IPLAY_DESC,INFO,"cant find the string ,the CONTROL ID is %d\n",IDC);
        return ERR_FAILURE;
        }
        else
        {
           convert_gb2312_chinese_asc2unistr(iptv_ctrl_info[i].iptv_char[cnt], uni_str, size_str);
           return SUCCESS;
        }
         
     }
   }
   return ERR_FAILURE;
}

static void on_ui_iptv_init_play_data(void)
{
    al_iptv_play_origin_info_t *  p_data = NULL;

    p_data = SY_MALLOC(sizeof(al_iptv_play_origin_info_t));
    MT_ASSERT(p_data != NULL);
    memset(p_data,0,sizeof(al_iptv_play_origin_info_t));
    ui_iptv_dat->p_play_data = p_data;
    return;
}

 static void on_ui_iptv_init_recm_data(void)
{
    iptv_recomand_info_t *p_rec_data =SY_MALLOC(sizeof(iptv_recomand_info_t));
   MT_ASSERT(p_rec_data != NULL);
   memset(p_rec_data,0, sizeof(iptv_recomand_info_t));
   ui_iptv_dat->p_recomm_data = p_rec_data;
   return;
}

static RET_CODE cbox_droplist_up_update(control_t *p_list,
                                      u16 start,
                                      u16 size,
                                      u32 context)
{
  control_t *p_cbox = (control_t *)context;
  u32 item_content = 0;
  u16 i = 0, pos = 0, count = 0;
  MT_ASSERT(p_cbox != NULL);
  count = list_get_count(p_list);

  for(i = 0; i < size; i++)
  {
    pos = i + start;
    if(pos < count)
    {
      item_content = cbox_get_content(p_cbox, pos);
      switch(p_cbox->priv_attr & CBOX_WORKMODE_MASK)
      {
        case CBOX_WORKMODE_DYNAMIC:
          list_set_field_content_by_unistr(p_list, pos, 0, (u16 *)item_content);
          break;
        default:
          MT_ASSERT(0);
      }
    }
  }
  return SUCCESS;
}
static control_t *cbox_create_up_droplist(control_t *p_ctrl,
                                u16 page,
                                u16 sbar_width,
                                u16 sbar_gap
                                )
{
  control_t *p_root = NULL;
  rect_t rc_cbox, rc_root;
  control_t *p_droplist = NULL;
  u16 list_width = 0, list_height = 0;
  MT_ASSERT(p_ctrl != NULL);
  ctrl_get_frame(p_ctrl, &rc_cbox);
  MT_ASSERT((RECTW(rc_cbox) > sbar_gap + sbar_width)
           && (RECTH(rc_cbox) * page > 2 * sbar_gap));
  list_width = RECTW(rc_cbox);
  list_height = RECTH(rc_cbox) * page;
  p_root = ctrl_get_root(p_ctrl);
  ctrl_get_frame(p_root, &rc_root);
  ctrl_client2screen(p_root, &rc_root);
  ctrl_client2screen(p_ctrl, &rc_cbox);

  p_droplist = ctrl_create_ctrl(CTRL_LIST, 0,
    rc_cbox.left - rc_root.left, rc_cbox.top + RECTH(rc_cbox) - list_height,
    list_width, list_height, p_root, 0);
  
  if(p_droplist != NULL)
  {
    u8 count = page;
    cbox_attach_droplist(p_ctrl, p_droplist);
    list_set_count(p_droplist, count, page);
    list_set_field_count(p_droplist, 1, page);
    list_set_update(p_droplist, cbox_droplist_up_update, (u32)p_ctrl);
    ctrl_set_sts(p_droplist, OBJ_STS_HIDE);
  }
  else
  	DEBUG(UI_IPLAY_DESC,INFO,"destory cbox_create_up_droplist\n");
  return p_droplist;
}
static RET_CODE on_iptv_update_recomm(control_t* p_ctrl, u16 start, u16 size, u32 context)
{
  control_t *p_item = NULL;
  control_t *p_pic = NULL, *p_text = NULL;
  u16 i;
  rect_t rect = {0};
  iptv_recomand_info_t *p_rec_data = (iptv_recomand_info_t *)ui_iptv_dat->p_recomm_data;
  if(p_rec_data == NULL)
  {
     DEBUG(UI_IPLAY_DESC,INFO,"p_rec_data is null\n");
  return SUCCESS;
  }
  if(p_ctrl == NULL)
  {
      return SUCCESS;
  }
    for (i = 0; i < size; i++)
  {
    if (i < p_rec_data->number)
    {
    p_item = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESC_RECOM_INFO_START + i);
    if(p_item == NULL)
    return SUCCESS;  
    p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_DESC_RECOM_INFO_PIC);
    p_text = ctrl_get_child_by_id(p_item, IDC_IPTV_DESC_RECOM_INFO_NAME);
      if(start + i >= p_rec_data->number)
      {
        text_set_content_by_extstr(p_text, NULL);
        bmap_set_content_by_id(p_pic, IM_PIC_LOADING_2);
        if(p_text)
        ctrl_set_sts(p_text, OBJ_STS_HIDE);
        if(p_pic)
        ctrl_set_sts(p_pic, OBJ_STS_HIDE);

      }
      else
      {
      text_set_content_by_extstr(p_text, p_rec_data->recmmd[i+start].title);
      bmap_set_content_by_id(p_pic, IM_PIC_LOADING_2);
      if(p_pic)
        ctrl_set_sts(p_pic, OBJ_STS_SHOW);
      if(p_text)
        ctrl_set_sts(p_text, OBJ_STS_SHOW);
      }
    }
   }
  ctrl_get_frame(p_ctrl, &rect);
  ctrl_client2screen(p_ctrl, &rect);
  ui_pic_clear_rect(&rect, 0);
  ctrl_paint_ctrl(p_ctrl, TRUE);
  return SUCCESS;
}
static RET_CODE on_iptv_draw_recomm_icon(control_t *p_cont, u16 pic_index)
{
  control_t *p_item = NULL;
  control_t *p_pic = NULL;
  rect_t rect;
  u16 start = pic_index%IPTV_DESC_RECOMM_INFO_NUM;
  iptv_recomand_info_t *p_rec_data = (iptv_recomand_info_t *)ui_iptv_dat->p_recomm_data;
  if(p_rec_data == NULL)
  {
    return SUCCESS;
  }
  
  p_item = ctrl_get_child_by_id(p_cont, IDC_IPTV_DESC_RECOM_INFO_START + start);
  if(p_item == NULL)
  {
    return SUCCESS;
  }
  p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_DESC_RECOM_INFO_PIC);
  if(p_pic == NULL)
  {
    return SUCCESS;
  }
  ctrl_get_frame(p_pic, &rect);
  ctrl_client2screen(p_pic, &rect);
    if(p_rec_data->recmmd[pic_index].img && pic_index < p_rec_data->number)
  {
    if(strlen(p_rec_data->recmmd[pic_index].img)!=0)
    {
     ui_iptv_dat->identify = ((u16)(ROOT_ID_IPTV_DESCRIPTION)<<16)|pic_index;
     ui_pic_play_by_url(p_rec_data->recmmd[pic_index].img, &rect,ui_iptv_dat->identify);
     return SUCCESS;
    }
  }
  return ERR_FAILURE;
}
static void on_iptv_dis_pg_pic(control_t *p_cont,BOOL load)
{
    control_t* p_item = NULL;
    al_iptv_play_origin_info_t *p_data = (al_iptv_play_origin_info_t *)on_ui_iptv_get_play_data();
    p_item = ctrl_get_child_by_id(p_cont, IDC_IPTV_DESC_PICTURE);
    bmap_set_content_by_id(p_item, 0);
    if(load == TRUE && p_data != NULL)
    {
        bmap_set_content_by_id(p_item, IM_PIC_LOADING_1);
    }
    else if(load == FALSE)
    {
        ctrl_set_rstyle(p_item, RSI_OTT_FRM_1_SH, RSI_OTT_FRM_1_SH, RSI_OTT_FRM_1_SH);
    }
    ctrl_paint_ctrl(p_item, TRUE);
    return;
}
static void on_iptv_dis_pg_recmand_pic(control_t *p_ctrl,u8 start)
{
  control_t* p_item = NULL;
   control_t* p_pic = NULL;
     p_item = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESC_RECOM_INFO_START + start);
     p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_DESC_RECOM_INFO_PIC);
     bmap_set_content_by_id(p_pic, RSC_INVALID_ID);
     ctrl_paint_ctrl(p_pic, TRUE);
      return;
  }
  static void on_iptv_update_rec_pic(control_t *p_ctrl)
  {
    on_iptv_update_recomm(p_ctrl, 0,IPTV_DESC_RECOMM_INFO_NUM,0);
    on_iptv_draw_recomm_icon(p_ctrl, 0); 
  }
static void on_iptv_free_play_data(void)
{
    u32 j = 0;
   
    al_iptv_play_origin_info_t *p_data = (al_iptv_play_origin_info_t *)on_ui_iptv_get_play_data();
    iptv_recomand_info_t *p_rec_data = (iptv_recomand_info_t *)ui_iptv_dat->p_recomm_data;
    u8 *p_iptv_url = (u8 *)ui_iptv_dat->p_img_url;

    if(p_data == NULL)
        return;


    if(p_data != NULL)
    {
        for(j = 0;j < p_data->url_count;j++)
        {
            if(&(p_data->play_origin_info[j]) !=NULL)
            {
                if(p_data->play_origin_info[j].urltitle != NULL)
                {
                    SY_FREE(p_data->play_origin_info[j].urltitle);
                    p_data->play_origin_info[j].urltitle = NULL;
                }                        
            }
        }
        if(p_data->play_origin_info !=NULL)
        {
            SY_FREE(p_data->play_origin_info);
            p_data->play_origin_info = NULL;
        }
        if(p_data->origin != NULL)
        {
            SY_FREE(p_data->origin);
            p_data->origin = NULL;
        }
    }

    if(p_data != NULL)
    {
        SY_FREE(p_data);
        ui_iptv_dat->p_play_data = NULL;
    }

    if(p_iptv_url != NULL)
    {
        SY_FREE(p_iptv_url);
        ui_iptv_dat->p_img_url = NULL;
    }

    if(p_rec_data!=NULL)
    {
        if(p_rec_data->recmmd!=NULL)
        {
            SY_FREE(p_rec_data->recmmd);
        }
        SY_FREE(p_rec_data);
        ui_iptv_dat->p_recomm_data = NULL;
    }

    if(ui_iptv_dat->p_iptv_img_less != NULL)
    {
        SY_FREE(ui_iptv_dat->p_iptv_img_less);
        ui_iptv_dat->p_iptv_img_less = NULL;
    }

    return;
}

#if ENABLE_ROLL
 static RET_CODE on_iptv_create_roll(control_t* p_ctrl)
 {
  RET_CODE ret = ERR_FAILURE;
   roll_param_t roll_param = {ROLL_LR,ROLL_SINGLE,0,0};
   gui_start_roll(p_ctrl, &roll_param);
   ret = fw_tmr_create(ROOT_ID_IPTV_DESCRIPTION, MSG_HEART_BEAT, 300, TRUE);
  return ret;
 }
static RET_CODE on_iptv_destroy_roll(control_t* p_ctrl)
{
   RET_CODE  ret = ERR_FAILURE;
   if(ctrl_is_rolling(p_ctrl))
   {
    gui_stop_roll(p_ctrl);
    ret = fw_tmr_destroy(ROOT_ID_IPTV_DESCRIPTION, MSG_HEART_BEAT);
   }
   return ret;
}

static RET_CODE ui_iptv_des_on_roll(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
 
   if(ui_get_usb_status())
   {
     gui_rolling(); 
    }
   return SUCCESS;
}
#endif
static BOOL  get_iptv_is_fav(VDO_ID_t *video_id,u32 *fav_focus)
{
    u32 iptv_total_num = 0;
    u32 i =0;
    iptv_fav_info iptv_fav_info;
    DEBUG(UI_IPLAY_DESC,INFO,"in ...\n");
    sys_status_get_fav_iptv_total_num(&iptv_total_num);
    if(iptv_total_num==0)
    return FALSE;
    else
    {
    for(i=0;i<iptv_total_num;i++)
    {
        sys_status_get_fav_iptv_info(i,&iptv_fav_info);
        if(0 == memcmp(video_id->qpId, iptv_fav_info.iptv_fav_vdo_id.qpId,(strlen(video_id->qpId) + 1)))
            if(0 == memcmp(video_id->tvQid, iptv_fav_info.iptv_fav_vdo_id.tvQid,(strlen(video_id->tvQid) + 1)))
                if(0 == memcmp(video_id->sourceCode, iptv_fav_info.iptv_fav_vdo_id.sourceCode,(strlen(video_id->sourceCode) + 1)))
                {
                    *fav_focus = i;
                    return TRUE;
                }
    }
    return FALSE;  
    }
}

static RET_CODE plist_cbox_update(control_t *p_cbox, u16 focus, u16 *p_str,u16 max_length)
{
    u16 *p_unistr = NULL;
    u16 uni_str[32] = {0};

    OS_PRINTF("[debug] %s %d  max_length[%d] focus[%d]\n",__FUNCTION__,__LINE__,max_length,focus);
    if(ui_iptv_dat->total_format > 0){

    switch(ui_iptv_dat->format_list[focus])
    {
        case IPTV_API_FORMAT_NORMAL:
            p_unistr =  (u16 *)gui_get_string_addr(IDS_HD_NORMAL_FORMAT);
            uni_strcpy(p_str, p_unistr);
            break;
        case IPTV_API_FORMAT_HIGH:
            p_unistr =  (u16 *)gui_get_string_addr(IDS_HD_HIGH_FORMAT);
            uni_strcpy(p_str, p_unistr);
            break;
        case IPTV_API_FORMAT_SUPER:
            p_unistr =  (u16 *)gui_get_string_addr(IDS_HD_SUPER_FORMAT);
            uni_strcpy(p_str, p_unistr);
            break;
        case IPTV_API_FORMAT_TOPSPEED:
            p_unistr =  (u16 *)gui_get_string_addr(IDS_HD_TOPSPEED_FORMAT);
            uni_strcpy(p_str, p_unistr);
            break;
        case IPTV_API_FORMAT_720P:
            p_unistr =  (u16 *)gui_get_string_addr(IDS_720P);
            uni_strcpy(p_str, p_unistr);
            break;
        case IPTV_API_FORMAT_1080P:
            p_unistr =  (u16 *)gui_get_string_addr(IDS_1080P);
            uni_strcpy(p_str, p_unistr);
            break;
        default:
            p_unistr =  (u16 *)gui_get_string_addr(IDS_HD_NORMAL_FORMAT);
            uni_strcpy(p_str, p_unistr);
            break;
    }
    
  }else{
     convert_gb2312_chinese_asc2unistr("未知", uni_str, sizeof(uni_str));
     uni_strcpy(p_str, uni_str);
  }
  
  return SUCCESS;
}

static RET_CODE on_iptv_variety_list_update(control_t* p_list, u16 start, u16 size, u32 context)
{
    u32 i;
    u16 cnt = list_get_count(p_list);  
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_IPTV_DESCRIPTION,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        IDS_HD_NETWORK_BUSY_PLS_WAIT,
        2000,
    };
    
    al_iptv_play_origin_info_t *p_data_iptv = (al_iptv_play_origin_info_t *)on_ui_iptv_get_play_data();

    if(p_data_iptv != NULL)
    {
        if(p_data_iptv->origin != NULL)
        {
            for (i = 0; i < size; i++)
            {
                if (i + start < cnt)
                {
                    if(p_data_iptv->play_origin_info[i+start].urltitle == NULL)
                    {
                        ui_comm_dlg_open(&dlg_data);
                        return SUCCESS;
                    }
                    list_set_field_content_by_unistr(p_list, (u16)(start + i), 0, p_data_iptv->play_origin_info[i+start].urltitle);
                }
            }
            //list_set_focus_pos(p_list, start);
            ctrl_paint_ctrl(p_list, TRUE);
        }
    }
    return SUCCESS;
}

static control_t *create_iptv_desc_menu()
{
    control_t *p_menu = NULL;

    p_menu = ui_comm_root_create(ROOT_ID_IPTV_DESCRIPTION, 0,
                                  COMM_BG_X, COMM_BG_Y,
                                  COMM_BG_W, COMM_BG_H,
                                  0, 0);

    if (p_menu == NULL)
    {
        return NULL;
    }
    ctrl_set_keymap(p_menu, iptv_des_cont_keymap);
    ctrl_set_proc(p_menu, iptv_des_cont_proc);

    return p_menu;
}

static control_t *create_iptv_desc_cont(control_t *p_menu)
{
    control_t *p_cont = NULL;

    p_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_DESC_CONT, 
                            IPTV_DESC_DESC_CONTX, IPTV_DESC_DESC_CONTY, 
                            IPTV_DESC_DESC_CONTW, IPTV_DESC_DESC_CONTH, 
                            p_menu, 0);

    MT_ASSERT(p_cont != NULL);
    ctrl_set_rstyle(p_cont, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);

    return p_cont;
}

static void create_iptv_desc_score(control_t *p_cont)
{
    control_t *p_sub = NULL;
    u16 uni_str[8];
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESC_SCORE, 
                          IPTV_DESC_RIGHT_SCOREX, IPTV_DESC_RIGHT_SCOREY, 
                          IPTV_DESC_RIGHT_SCOREW, IPTV_DESC_RIGHT_SCOREH, 
                          p_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_RIGHT| STL_VCENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    on_ui_iptv_find_dis_str((u32)IDC_IPTV_DESC_SCORE,0,uni_str,sizeof(uni_str));
    text_set_content_by_unistr(p_sub, uni_str);
    text_set_font_style(p_sub, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESC_TXT_SCORE, 
                          IPTV_DESC_RIGHT_TXT_SCOREX, IPTV_DESC_RIGHT_TXT_SCOREY, 
                          IPTV_DESC_RIGHT_TXT_SCOREW, IPTV_DESC_RIGHT_TXT_SCOREH, 
                          p_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_CENTER | STL_VCENTER);
    text_set_font_style(p_sub, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
}
static void create_iptv_desc_title(control_t *p_cont)
{
    control_t *p_title = NULL;

    // title 
    p_title = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESC_TITLE, 
                            IPTV_DESC_RIGHT_TITLEX, IPTV_DESC_RIGHT_TITLEY, 
                            IPTV_DESC_RIGHT_TITLEW, IPTV_DESC_RIGHT_TITLEH, 
                            p_cont, 0);
    ctrl_set_rstyle(p_title, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_title, STL_LEFT| STL_VCENTER);
    text_set_content_type(p_title, TEXT_STRTYPE_UNICODE);
    text_set_font_style(p_title, FSI_WHITE_24, FSI_WHITE_24, FSI_WHITE_24);
}

static void create_iptv_desc_picture(control_t *p_menu)
{
    control_t *p_picture = NULL;

    //picture 
    p_picture = ctrl_create_ctrl(CTRL_BMAP, IDC_IPTV_DESC_PICTURE, 
                              IPTV_DESC_PICTUREX, IPTV_DESC_PICTUREY, 
                              IPTV_DESC_PICTUREW, IPTV_DESC_PICTUREH, 
                              p_menu, 0);
    ctrl_set_mrect(p_picture, IPTV_RECOMPIC_OFFSET,
         IPTV_RECOMPIC_OFFSET, IPTV_DESC_PICTUREW - IPTV_RECOMPIC_OFFSET, 
         IPTV_DESC_PICTUREH -IPTV_RECOMPIC_OFFSET);
    ctrl_set_rstyle(p_picture, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);
}

static control_t *create_recommend_container(control_t *p_menu)
{
    control_t *p_list_cont = NULL;

    if((p_list_cont = ctrl_get_child_by_id(p_menu, IDC_IPTV_DESC_RECOM_INFO_CONT)) != NULL)
    {
        ctrl_destroy_ctrl(p_list_cont);
    }

    p_list_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_DESC_RECOM_INFO_CONT,
                                  IPTV_DESC_RECOMM_INFO_MBOX_X, IPTV_DESC_RECOMM_INFO_MBOX_Y,
                                  IPTV_DESC_RECOMM_INFO_MBOX_W, IPTV_DESC_RECOMM_INFO_MBOX_H,
                                  p_menu, 0);
    ctrl_set_keymap(p_list_cont, iptv_recomm_keymap);
    ctrl_set_proc(p_list_cont, iptv_recomm_proc);
    ctrl_set_rstyle(p_list_cont, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);
    return p_list_cont;
}

static control_t *create_recommend_info(control_t *p_menu,u8 rec_num)
{
    control_t *p_list_cont = NULL;
    u16 i = 0;
    u16 x,y;
    control_t *p_item = NULL;
    control_t *p_ctrl = NULL;
    p_list_cont = create_recommend_container( p_menu);

    x = IPTV_RECOMM_ITEM_X;
    y = IPTV_RECOMM_ITEM_Y;

    for (i = 0; i < ((rec_num < IPTV_DESC_RECOMM_INFO_NUM)? rec_num : IPTV_DESC_RECOMM_INFO_NUM); i++)
    {
        p_item = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_DESC_RECOM_INFO_START + i,
                              x, y,
                              IPTV_RECOMM_ITEM_W, IPTV_RECOMM_ITEM_H,
                              p_list_cont, 0);
        ctrl_set_rstyle(p_item, RSI_PBACK, RSI_PBACK, RSI_PBACK);
        //ctrl_set_mrect(p_item, 
                             //IPTV_RECOMPIC_OFFSET, IPTV_RECOMPIC_OFFSET,
                             //IPTV_RECOMM_ITEM_W - IPTV_RECOMPIC_OFFSET, 
                             //IPTV_RECOMM_ITEM_H - IPTV_RECOMPIC_OFFSET);


        p_ctrl = ctrl_create_ctrl(CTRL_BMAP, IDC_IPTV_DESC_RECOM_INFO_PIC,
                              IPTV_RECOMMPIC_X, IPTV_RECOMMPIC_Y,
                              IPTV_RECOMMPIC_W, IPTV_RECOMMPIC_H,
                              p_item, 0);
        ctrl_set_rstyle(p_ctrl, RSI_OTT_FRM_1_SH, RSI_OTT_FRM_1_HL, RSI_OTT_FRM_1_SH);
        ctrl_set_mrect(p_ctrl, 
                        IPTV_RECOMPIC_BMP_OFFSET, IPTV_RECOMPIC_BMP_OFFSET,
                        IPTV_RECOMMPIC_W - 2*IPTV_RECOMPIC_BMP_OFFSET, 
                        IPTV_RECOMMPIC_H - 2*IPTV_RECOMPIC_BMP_OFFSET);
        ctrl_set_sts(p_ctrl,OBJ_STS_HIDE);

        p_ctrl = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESC_RECOM_INFO_NAME,
                              IPTV_RECOMMNANE_X, IPTV_RECOMMNAME_Y,
                              IPTV_RECOMMNAME_W, IPTV_RECOMMNAME_H,
                              p_item, 0);
        ctrl_set_rstyle(p_ctrl, RSI_OTT_KEYBOARD_KEY_SH, RSI_OTT_KEYBOARD_KEY_SH, RSI_OTT_KEYBOARD_KEY_SH);

        ctrl_set_mrect(p_ctrl, 
                        IPTV_RECOMMNAME_OFFSET, 0,
                        IPTV_RECOMMNAME_W - IPTV_RECOMMNAME_OFFSET, IPTV_RECOMMNAME_H);

        text_set_font_style(p_ctrl, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
        text_set_content_type(p_ctrl, TEXT_STRTYPE_EXTSTR);
        ctrl_set_sts(p_ctrl,OBJ_STS_HIDE);

        x += IPTV_RECOMM_ITEM_W + IPTV_RECOMM_ITEM_VGAP;
    }
    p_item =ctrl_get_child_by_id(p_list_cont,IDC_IPTV_DESC_RECOM_INFO_START);
    return p_list_cont;
}

static void on_update_ui_content(void)
{
    DEBUG(UI_IPLAY_DESC,INFO,"@@@%s  qpid[%s]\n",__FUNCTION__,ui_iptv_dat->video_id.qpId);
    ui_iptv_get_video_info(&ui_iptv_dat->video_id, ui_iptv_dat->category_id); 
    ui_iptv_dat->iptv_entf = FALSE;
    return;
}



/*************************create TV  functions***********************/
static void create_iptv_desc_tv_actor(control_t *p_des_cont)
{
    control_t *p_sub = NULL;
    u16 uni_str[8];
    u32 idc_id = (u32)IDC_IPTV_DESCNT_VDO_ACTOR;

    p_sub = ctrl_create_ctrl(CTRL_TEXT, idc_id, 
                          IPTV_DESC_TEXT_VDO_ACTOR_X, IPTV_DESC_TEXT_VDO_ACTOR_Y, 
                          IPTV_DESC_TEXT_VDO_ACTOR_W, IPTV_DESC_TEXT_VDO_ACTOR_H, 
                          p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_RIGHT | STL_VCENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);

     on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));
      
    text_set_content_by_unistr(p_sub, uni_str);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);

    //create description text field
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_VDO_TXT_ACTOR, 
                          IPTV_DESC_VDO_TXT_ACTOR_X, IPTV_DESC_VDO_TXT_ACTOR_Y, 
                          IPTV_DESC_VDO_TXT_ACTOR_W, IPTV_DESC_VDO_TXT_ACTOR_H, 
                          p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    
}

static void create_iptv_desc_tv_year(control_t *p_des_cont)
{
    control_t *p_sub = NULL;
    u16 uni_str[8];

    u32 idc_id = (u32)IDC_IPTV_DESCNT_VDO_YEAR;

    p_sub = ctrl_create_ctrl(CTRL_TEXT, idc_id, 
                          IPTV_DESC_TEXT_VDO_YEAR_X, IPTV_DESC_TEXT_VDO_YEAR_Y, 
                          IPTV_DESC_TEXT_VDO_YEAR_W, IPTV_DESC_TEXT_VDO_YEAR_H, 
                          p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_RIGHT| STL_VCENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);

     on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));
    text_set_content_by_unistr(p_sub, uni_str);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);

    //create description text field
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_VDO_TXT_YEAR, 
                          IPTV_DESC_VDO_TXT_YEAR_X, IPTV_DESC_VDO_TXT_YEAR_Y, 
                          IPTV_DESC_VDO_TXT_YEAR_W, IPTV_DESC_VDO_TXT_YEAR_H, 
                          p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    
}

static void create_iptv_desc_tv_attr(control_t *p_des_cont)
{
    control_t *p_sub = NULL;
    u16 uni_str[8];
    u32 idc_id = (u32)IDC_IPTV_DESCNT_VDO_ATTR;
     on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));

    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_VDO_ATTR, 
                          IPTV_DESC_TEXT_VDO_ATTR_X, IPTV_DESC_TEXT_VDO_ATTR_Y, 
                          IPTV_DESC_TEXT_VDO_ATTR_W, IPTV_DESC_TEXT_VDO_ATTR_H, 
                          p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_RIGHT| STL_VCENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);

    text_set_content_by_unistr(p_sub, uni_str);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);

    //create description text field
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_VDO_TXT_ATTR, 
                          IPTV_DESC_VDO_TXT_ATTR_X, IPTV_DESC_VDO_TXT_ATTR_Y, 
                          IPTV_DESC_VDO_TXT_ATTR_W, IPTV_DESC_VDO_TXT_ATTR_H, 
                          p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    
}

static void create_iptv_desc_tv_directer(control_t *p_des_cont)
{
    control_t *p_sub = NULL;
    u16 uni_str[8];
    u32 idc_id = (u32)IDC_IPTV_DESCNT_VDO_DIRECTER;
     on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));

    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_VDO_DIRECTER, 
                          IPTV_DESC_TEXT_VDO_DIRECTER_X, IPTV_DESC_TEXT_VDO_DIRECTER_Y, 
                          IPTV_DESC_TEXT_VDO_DIRECTER_W, IPTV_DESC_TEXT_VDO_DIRECTER_H, 
                          p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_RIGHT| STL_VCENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);

    text_set_content_by_unistr(p_sub, uni_str);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);

    //create description text field
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_VDO_TXT_DIRECTER, 
                          IPTV_DESC_VDO_TXT_DIRECTER_X, IPTV_DESC_VDO_TXT_DIRECTER_Y, 
                          IPTV_DESC_VDO_TXT_DIRECTER_W, IPTV_DESC_VDO_TXT_DIRECTER_H, 
                          p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
}

static void create_iptv_desc_tv_series(control_t *p_des_cont)
{
    control_t *p_sub = NULL;
    u16 uni_str[8];

    u32 idc_id = (u32)IDC_IPTV_DESCNT_VDO_SERIES;
     on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));

    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_VDO_SERIES, 
                          IPTV_DESC_TEXT_VDO_SERIES_X, IPTV_DESC_TEXT_VDO_SERIES_Y, 
                          IPTV_DESC_TEXT_VDO_SERIES_W, IPTV_DESC_TEXT_VDO_SERIES_H, 
                          p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_RIGHT| STL_VCENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_sub, uni_str);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);

    //create description text field
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_VDO_TXT_SERIES, 
                          IPTV_DESC_VDO_TXT_SERIES_X, IPTV_DESC_VDO_TXT_SERIES_Y, 
                          IPTV_DESC_VDO_TXT_SERIES_W, IPTV_DESC_VDO_TXT_SERIES_H, 
                          p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    
}

static void create_iptv_desc_series_tv_num(control_t *p_des_cont,u16 series_cnt)
{
    control_t *p_series_num_mbox = NULL;
    control_t *p_ctrl_temp = NULL;
    u16 series_num_max = 0;
    u8 str_series_num[16] = {0};
    u16 i = 0;
    if((p_ctrl_temp = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESC_SERIES_NUM_MBOX)) != NULL)
    {
        DEBUG(UI_IPLAY_DESC,INFO,"destory create_iptv_desc_series_num\n");
        ctrl_destroy_ctrl(p_ctrl_temp);
    }
    ui_iptv_dat->g_series_cnt = series_cnt;
    if(ui_iptv_dat->g_series_cnt == 0)
    {
        return;
    }
    if(series_cnt%IPTV_SERIES_NUM_PER_PAGE_ITEMS != 0)
        series_num_max = series_cnt/IPTV_SERIES_NUM_PER_PAGE_ITEMS + 1;
    else
        series_num_max = series_cnt/IPTV_SERIES_NUM_PER_PAGE_ITEMS;

    p_series_num_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_IPTV_DESC_SERIES_NUM_MBOX,
                     IPTV_SERIES_NUM_MBOX_X, IPTV_SERIES_NUM_MBOX_Y,
                     IPTV_SERIES_NUM_MBOX_W, IPTV_SERIES_NUM_MBOX_H, p_des_cont, 0);
    ctrl_set_rstyle(p_series_num_mbox, RSI_PBACK, RSI_PBACK, RSI_PBACK);

    ctrl_set_keymap(p_series_num_mbox, iptv_des_series_num_keymap);
    ctrl_set_proc(p_series_num_mbox, iptv_des_series_num_proc);
    mbox_enable_string_mode(p_series_num_mbox, TRUE);
    mbox_set_count(p_series_num_mbox, series_num_max, series_num_max, IPTV_SERIES_NUM_ROW);
    mbox_set_win(p_series_num_mbox, 
    (series_num_max < IPTV_SERIES_NUM_WINCOL)? series_num_max : IPTV_SERIES_NUM_WINCOL,
    IPTV_SERIES_NUM_WINROW);
    mbox_set_item_interval(p_series_num_mbox, 10, 0);
    mbox_set_item_rstyle(p_series_num_mbox, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    mbox_set_string_fstyle(p_series_num_mbox, FSI_RED, FSI_WHITE_22, FSI_WHITE_22);
    mbox_set_content_strtype(p_series_num_mbox, MBOX_STRTYPE_UNICODE);
    mbox_set_string_offset(p_series_num_mbox, 0, 0);
    mbox_set_string_align_type(p_series_num_mbox, STL_CENTER | STL_VCENTER);

    for(i = 0; i < series_num_max; i++)
    {
        if(i == series_num_max - 1)
        {
            if((i*IPTV_SERIES_NUM_PER_PAGE_ITEMS+1) == series_cnt)
            {
                sprintf(str_series_num,"%d",series_cnt);   
            }
            else
            {
                sprintf(str_series_num,"%d--%d",(i*IPTV_SERIES_NUM_PER_PAGE_ITEMS+1),series_cnt);
            }
        }
        else
        {
            sprintf(str_series_num,"%d--%d",(i*IPTV_SERIES_NUM_PER_PAGE_ITEMS+1),(i+1)*IPTV_SERIES_NUM_PER_PAGE_ITEMS);
        }
        mbox_set_content_by_ascstr(p_series_num_mbox, i, str_series_num);
    }
  
    mbox_set_focus(p_series_num_mbox, 0);
}
 
 static void create_iptv_desc_tv_mbox(control_t *p_cont)
{
    control_t *p_mbox = NULL;
    control_t *p_ctrl_temp = NULL;
    u16 i;
    u16 mbox_count = ui_iptv_dat->g_series_cnt;
    if((p_ctrl_temp = ctrl_get_child_by_id(p_cont, IDC_IPTV_DESC_SERIES_MBOX)) != NULL)
    {
        DEBUG(UI_IPLAY_DESC,INFO,"destory create_iptv_desc_num\n");
        ctrl_destroy_ctrl(p_ctrl_temp);
    }
    if(mbox_count == 0)
    {
        return;
    }
    p_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_IPTV_DESC_SERIES_MBOX,
                         IPTV_DESC_MBOX_X, IPTV_DESC_MBOX_Y,
                         IPTV_DESC_MBOX_W, IPTV_DESC_MBOX_H,p_cont, 0);
    DEBUG(UI_IPLAY_DESC,INFO,"destory create_iptv_desc_series_num222\n");
    ctrl_set_rstyle(p_mbox, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_mbox, iptv_des_mbox_keymap);
    ctrl_set_proc(p_mbox, iptv_des_mbox_proc);
    mbox_enable_string_mode(p_mbox, TRUE);
    mbox_set_count(p_mbox, mbox_count, mbox_count, IPTV_DESC_ROW);
    mbox_set_win(p_mbox,
                        (mbox_count < IPTV_DESC_WINCOL)?mbox_count : IPTV_DESC_WINCOL,
                        IPTV_DESC_WINROW);
    mbox_set_item_interval(p_mbox, 20, 0);
    mbox_set_item_rstyle(p_mbox, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    mbox_set_string_fstyle(p_mbox, FSI_RED, FSI_WHITE_22, FSI_WHITE_22);
    mbox_set_content_strtype(p_mbox, MBOX_STRTYPE_DEC);
    mbox_set_string_offset(p_mbox, 0, 0);
    mbox_set_string_align_type(p_mbox, STL_CENTER | STL_VCENTER);

    for(i = 0; i < mbox_count; i++)
    {
        mbox_set_content_by_dec(p_mbox, i, i + 1);
    }

    mbox_set_focus(p_mbox, 0);
}


static void create_iptv_desc_tv_texts(control_t *p_des_cont)
{
    control_t *p_sub = NULL;
    u16 uni_str[8];

    u32 idc_id = (u32)IDC_IPTV_DESCNT_TXT_TITLE;
    on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));

    //create description title
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_TXT_TITLE, 
                          IPTV_DESC_TEXT_TITLE_X, IPTV_DESC_TEXT_TITLE_Y, 
                          IPTV_DESC_TEXT_TITLE_W, IPTV_DESC_TEXT_TITLE_H, 
                          p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_RIGHT| STL_VCENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_sub, uni_str);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);

    //create description text field
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_TXT, 
                          IPTV_DESC_TEXT_X, IPTV_DESC_TEXT_Y, 
                          IPTV_DESC_TEXT_W, IPTV_DESC_TEXT_H, 
                          p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_TOP);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    
}

static void create_iptv_desc_video_rsc_tv_btn(control_t *p_des_cont)
{
    control_t *p_rsc_mbox = NULL;
    u16 uni_str[8];

    u32 idc_id = (u32)IDC_IPTV_DESC_VIDEO_RSC_MBOX;
    if((p_rsc_mbox = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESC_VIDEO_RSC_MBOX)) != NULL)
    {
        DEBUG(UI_IPLAY_DESC,INFO,"destory create_iptv_desc_num\n");
        ctrl_destroy_ctrl(p_rsc_mbox);
    }

    p_rsc_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_IPTV_DESC_VIDEO_RSC_MBOX,
                             IPTV_VIDEO_RSC_MBOX_X, IPTV_VIDEO_RSC_MBOX_Y,
                             IPTV_VIDEO_RSC_MBOX_W, IPTV_VIDEO_RSC_MBOX_H, p_des_cont, 0);
    ctrl_set_rstyle(p_rsc_mbox, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_rsc_mbox, iptv_des_video_rsc_keymap);
    ctrl_set_proc(p_rsc_mbox, iptv_des_video_rsc_proc);
    mbox_enable_string_mode(p_rsc_mbox, TRUE);
    mbox_set_count(p_rsc_mbox, 2 ,2, IPTV_VIDEO_RSC_ROW);
    mbox_set_item_interval(p_rsc_mbox, IPTV_MOVIE_RSC_INTERVAL, 0);
    mbox_set_item_rstyle(p_rsc_mbox, RSI_OTT_BUTTON_HL, RSI_OTT_BUTTON_SH, RSI_OTT_BUTTON_SH);
    mbox_set_string_fstyle(p_rsc_mbox, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    mbox_set_content_strtype(p_rsc_mbox, MBOX_STRTYPE_UNICODE);
    mbox_set_string_align_type(p_rsc_mbox, STL_CENTER | STL_VCENTER);

    //play
    on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));
    mbox_set_content_by_unistr(p_rsc_mbox, 0, uni_str);

    //fav or unfav
    if(ui_iptv_dat->iptv_is_fav)
    {
        on_ui_iptv_find_dis_str(idc_id,2,uni_str,sizeof(uni_str));
        mbox_set_content_by_unistr(p_rsc_mbox, 1, uni_str);
    }
    else
    {
        on_ui_iptv_find_dis_str(idc_id,1,uni_str,sizeof(uni_str));
        mbox_set_content_by_unistr(p_rsc_mbox, 1, uni_str);
    }

    mbox_set_focus(p_rsc_mbox, 0);
    ctrl_default_proc(p_rsc_mbox, MSG_GETFOCUS, 0, 0);
    ctrl_set_sts(p_rsc_mbox, OBJ_STS_HIDE);
}

static void create_iptv_desc_tv_series_container(control_t *p_menu)
{
    control_t *p_des_cont = NULL;

    //create description
    p_des_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_DESCNT, 
                                IPTV_DESC_CNTX, IPTV_DESC_CNTY, 
                                IPTV_DESC_CNTW, IPTV_DESC_CNTH, 
                                p_menu, 0);
    ctrl_set_rstyle(p_des_cont,RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);

    create_iptv_desc_tv_year(p_des_cont);

    create_iptv_desc_tv_series(p_des_cont);

    create_iptv_desc_tv_directer(p_des_cont);

    create_iptv_desc_tv_actor(p_des_cont);

    create_iptv_desc_tv_attr(p_des_cont);

    //create description texts
    create_iptv_desc_tv_texts(p_des_cont);
    //create video resource
    create_iptv_desc_video_rsc_tv_btn(p_des_cont);
}

  static void create_iptv_desc_rsc_txt_tv_btn(control_t *p_des_cont,u8 count)
 {
    control_t * p_rsc_text = NULL;
    control_t * p_ctrl_temp = NULL;
    rect_t rc_group = {0};
    list_xstyle_t plist_field_fstyle =
    {
        FSI_GRAY,
        FSI_WHITE_22,
        FSI_WHITE_22,
        FSI_WHITE_22,
        FSI_WHITE_22,
    };

    list_xstyle_t plist_item_rstyle =
    {
        RSI_OTT_BUTTON_SH,
        RSI_OTT_BUTTON_SH,
        RSI_OTT_BUTTON_HL,
        RSI_OTT_BUTTON_SH,
        RSI_OTT_BUTTON_SH
    };

    if((p_ctrl_temp = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESC_VIDEO_RSC_TXT_LIST)) != NULL)
    {
        DEBUG(UI_IPLAY_DESC,INFO,"destory create_iptv_rsc_txt_btn\n");
        ctrl_destroy_ctrl(p_ctrl_temp);
    }
    if(count == 0)
        return;

    p_rsc_text = ctrl_create_ctrl(CTRL_CBOX, IDC_IPTV_DESC_VIDEO_RSC_TXT_LIST, 
                             IPTV_VIDEO_RSC_MBOX_X + IPTV_VIDEO_RSC_MBOX_W + IPTV_MOVIE_RSC_INTERVAL,
                             IPTV_VIDEO_RSC_MBOX_Y,
                             (IPTV_VIDEO_RSC_MBOX_W -IPTV_MOVIE_RSC_INTERVAL )/2,
                             IPTV_VIDEO_RSC_MBOX_H, p_des_cont,
                             0);
    ctrl_set_rstyle(p_rsc_text, RSI_OTT_BUTTON_SH, RSI_OTT_BUTTON_HL, RSI_PBACK);
    ctrl_set_keymap(p_rsc_text, iptv_des_video_rsc_list_keymap);
    ctrl_set_proc(p_rsc_text, iptv_des_video_rsc_list_proc);
    cbox_set_font_style(p_rsc_text, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    cbox_set_align_style(p_rsc_text, STL_CENTER | STL_VCENTER);
    cbox_set_work_mode(p_rsc_text, CBOX_WORKMODE_DYNAMIC);
    cbox_enable_cycle_mode(p_rsc_text, TRUE);
    cbox_dync_set_count(p_rsc_text, count);
    cbox_dync_set_focus(p_rsc_text, ui_iptv_dat->format_index);
    //cbox_static_set_content_type(p_rsc_text, CBOX_ITEM_STRTYPE_UNICODE);
    cbox_dync_set_update(p_rsc_text, plist_cbox_update);
    ctrl_get_frame(p_rsc_text, &rc_group);
    if(count > IPTV_SCROLL_BAR_MAX)
        count = IPTV_SCROLL_BAR_MAX;
    cbox_create_up_droplist(p_rsc_text, count, 0, 0);
    cbox_droplist_set_rstyle(p_rsc_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    cbox_droplist_set_mid_rect(p_rsc_text, 0, 0, RECTW(rc_group),
                             RECTH(rc_group) * count , 0);
    cbox_droplist_set_item_rstyle(p_rsc_text, &plist_item_rstyle);
    cbox_droplist_set_field_attr(p_rsc_text, STL_CENTER | STL_VCENTER,
                               RECTW(rc_group), 0, 0);
    cbox_droplist_set_field_font_style(p_rsc_text, &plist_field_fstyle);
    //cbox_droplist_set_sbar_rstyle(p_rsc_text, RSI_OTT_SCROLL_BAR_BG, RSI_OTT_SCROLL_BAR_BG, RSI_OTT_SCROLL_BAR_BG);
    //cbox_droplist_set_sbar_mid_rstyle(p_rsc_text,
                //                    RSI_OTT_SCROLL_BAR,
                       //             RSI_OTT_SCROLL_BAR,
                           //         RSI_OTT_SCROLL_BAR);
    ctrl_set_sts(p_rsc_text, OBJ_STS_HIDE);
}



/*************************create movie  functions***********************/

static void create_iptv_desc_movie_actor(control_t *p_des_cont)
{
    control_t *p_sub = NULL;
    u16 uni_str[8];

    u32 idc_id = (u32)IDC_IPTV_DESCNT_MOVIE_ACTOR;
    on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));

    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_MOVIE_ACTOR, 
                      IPTV_DESC_TEXT_MOVIE_ACTOR_X, IPTV_DESC_TEXT_MOVIE_ACTOR_Y, 
                      IPTV_DESC_TEXT_MOVIE_ACTOR_W, IPTV_DESC_TEXT_MOVIE_ACTOR_H, 
                      p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_RIGHT| STL_VCENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_sub, uni_str);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);

    //create description text field
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_MOVIE_TXT_ACTOR, 
                      IPTV_DESC_MOVIE_TXT_ACTOR_X, IPTV_DESC_MOVIE_TXT_ACTOR_Y, 
                      IPTV_DESC_MOVIE_TXT_ACTOR_W, IPTV_DESC_MOVIE_TXT_ACTOR_H, 
                      p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    
}


static void create_iptv_desc_movie_directer(control_t *p_des_cont)
{
    control_t *p_sub = NULL;
    u16 uni_str[8];

    u32 idc_id = (u32)IDC_IPTV_DESCNT_MOVIE_DIRECTER;
    on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));

    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_MOVIE_DIRECTER, 
                      IPTV_DESC_TEXT_MOVIE_DIRECTER_X, IPTV_DESC_TEXT_MOVIE_DIRECTER_Y, 
                      IPTV_DESC_TEXT_MOVIE_DIRECTER_W, IPTV_DESC_TEXT_MOVIE_DIRECTER_H, 
                      p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_RIGHT | STL_VCENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_sub, uni_str);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);

    //create description text field
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_MOVIE_TXT_DIRECTER, 
                      IPTV_DESC_MOVIE_TXT_DIRECTER_X, IPTV_DESC_MOVIE_TXT_DIRECTER_Y, 
                      IPTV_DESC_MOVIE_TXT_DIRECTER_W, IPTV_DESC_MOVIE_TXT_DIRECTER_H, 
                      p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    
}

static void create_iptv_desc_movie_year(control_t *p_des_cont)
{
    control_t *p_sub = NULL;
    u16 uni_str[8];

    u32 idc_id = (u32)IDC_IPTV_DESCNT_MOVIE_YEAR;
    on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));

    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_MOVIE_YEAR, 
                      IPTV_DESC_TEXT_MOVIE_YEAR_X, IPTV_DESC_TEXT_MOVIE_YEAR_Y, 
                      IPTV_DESC_TEXT_MOVIE_YEAR_W, IPTV_DESC_TEXT_MOVIE_YEAR_H, 
                      p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_RIGHT | STL_VCENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_sub, uni_str);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);

    //create description text field
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_MOVIE_TXT_YEAR, 
                      IPTV_DESC_MOVIE_TXT_YEAR_X, IPTV_DESC_MOVIE_TXT_YEAR_Y, 
                      IPTV_DESC_MOVIE_TXT_YEAR_W, IPTV_DESC_MOVIE_TXT_YEAR_H, 
                      p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_CENTER);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    
}

static void create_iptv_desc_movie_duration(control_t *p_des_cont)
{
    control_t *p_sub = NULL;
    u16 uni_str[8];

    u32 idc_id = (u32)IDC_IPTV_DESCNT_MOVIE_DURATION;
    on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));

    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_MOVIE_DURATION, 
                      IPTV_DESC_TEXT_MOVIE_DURATION_X, IPTV_DESC_TEXT_MOVIE_DURATION_Y, 
                      IPTV_DESC_TEXT_MOVIE_DURATION_W, IPTV_DESC_TEXT_MOVIE_DURATION_H, 
                      p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_RIGHT | STL_VCENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_sub, uni_str);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);

    //create description text field
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_MOVIE_TXT_DURATION, 
                      IPTV_DESC_MOVIE_TXT_DURATION_X, IPTV_DESC_MOVIE_TXT_DURATION_Y, 
                      IPTV_DESC_MOVIE_TXT_DURATION_W, IPTV_DESC_MOVIE_TXT_DURATION_H, 
                      p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_VCENTER);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    
}


static void create_iptv_desc_movie_texts(control_t *p_des_cont)
{
    control_t *p_sub = NULL;
    u16 uni_str[8];

    u32 idc_id = (u32)IDC_IPTV_DESCNT_MOVIE_TXT_TITLE;
    on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));

    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_MOVIE_TXT_TITLE, 
                      IPTV_DESC_MOVIE_TEXT_TITLE_X, IPTV_DESC_MOVIE_TEXT_TITLE_Y, 
                      IPTV_DESC_MOVIE_TEXT_TITLE_W, IPTV_DESC_MOVIE_TEXT_TITLE_H, 
                      p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_RIGHT| STL_VCENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_sub, uni_str);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);

    //create description text field
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_MOVIE_TXT, 
                      IPTV_DESC_MOVIE_TEXT_X, IPTV_DESC_MOVIE_TEXT_Y, 
                      IPTV_DESC_MOVIE_TEXT_W, IPTV_DESC_MOVIE_TEXT_H, 
                      p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_TOP);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    
}

static void create_iptv_desc_movie_rsc_btn(control_t *p_des_cont)
{
    control_t *p_rsc_mbox = NULL;
    u16 uni_str[8];

    u32 idc_id = (u32)IDC_IPTV_DESCNT_MOVIE_RSC_MBOX;

    if((p_rsc_mbox = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_MOVIE_RSC_MBOX)) != NULL)
    {
        DEBUG(UI_IPLAY_DESC,INFO,"destory create_iptv_desc_num\n");
        ctrl_destroy_ctrl(p_rsc_mbox);
    }
    p_rsc_mbox = ctrl_create_ctrl(CTRL_MBOX, IDC_IPTV_DESCNT_MOVIE_RSC_MBOX,
                                 IPTV_MOVIE_RSC_MBOX_X, IPTV_MOVIE_RSC_MBOX_Y,
                                 IPTV_MOVIE_RSC_MBOX_W, IPTV_MOVIE_RSC_MBOX_H, p_des_cont, 0);
    ctrl_set_rstyle(p_rsc_mbox, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_rsc_mbox, iptv_des_video_rsc_keymap);
    ctrl_set_proc(p_rsc_mbox, iptv_des_video_rsc_proc);
    mbox_enable_string_mode(p_rsc_mbox, TRUE);
    mbox_set_count(p_rsc_mbox, 2, 2, 1);
    mbox_set_item_interval(p_rsc_mbox, IPTV_MOVIE_RSC_INTERVAL, 0);
    mbox_set_item_rstyle(p_rsc_mbox, RSI_OTT_BUTTON_HL, RSI_OTT_BUTTON_SH, RSI_OTT_BUTTON_SH);
    mbox_set_string_fstyle(p_rsc_mbox, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    mbox_set_content_strtype(p_rsc_mbox, MBOX_STRTYPE_UNICODE);
    mbox_set_string_align_type(p_rsc_mbox, STL_CENTER | STL_VCENTER);

    //play
    on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));
    mbox_set_content_by_unistr(p_rsc_mbox, 0, uni_str);

    //fav or unfav
    if(ui_iptv_dat->iptv_is_fav)
    {
    on_ui_iptv_find_dis_str(idc_id,2,uni_str,sizeof(uni_str));
    mbox_set_content_by_unistr(p_rsc_mbox, 1, uni_str);
    }
    else
    {
    on_ui_iptv_find_dis_str(idc_id,1,uni_str,sizeof(uni_str));
    mbox_set_content_by_unistr(p_rsc_mbox, 1, uni_str);
    }

    mbox_set_focus(p_rsc_mbox, 0);
    ctrl_default_proc(p_rsc_mbox, MSG_GETFOCUS, 0, 0);
    ctrl_set_sts(p_rsc_mbox, OBJ_STS_HIDE);
}

static void create_iptv_desc_movie_container(control_t *p_menu)
{
    control_t *p_des_cont = NULL;

    p_des_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_DESCNT, 
                                IPTV_DESC_CNTX, IPTV_DESC_CNTY, 
                                IPTV_DESC_CNTW, IPTV_DESC_CNTH, 
                                p_menu, 0);
    ctrl_set_rstyle(p_des_cont, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);

    create_iptv_desc_movie_year(p_des_cont);

    create_iptv_desc_movie_duration(p_des_cont);

    create_iptv_desc_movie_directer(p_des_cont);

    create_iptv_desc_movie_actor(p_des_cont);

    create_iptv_desc_movie_texts(p_des_cont);

    create_iptv_desc_movie_rsc_btn(p_des_cont);

}

 static void create_iptv_desc_movie_rsc_txt_btn(control_t *p_des_cont,u8 count)
 {
    control_t *p_rsc_text = NULL;
    rect_t rc_group = {0};
    control_t *p_ctrl_temp = NULL;
    list_xstyle_t plist_field_fstyle =
    {
        FSI_GRAY,
        FSI_WHITE_22,
        FSI_WHITE_22,
        FSI_WHITE_22,
        FSI_WHITE_22,
    };

    list_xstyle_t plist_item_rstyle =
    {
        RSI_OTT_BUTTON_SH,
        RSI_OTT_BUTTON_SH,
        RSI_OTT_BUTTON_HL,
        RSI_OTT_BUTTON_SH,
        RSI_OTT_BUTTON_SH
    };

    if((p_ctrl_temp = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESC_VIDEO_RSC_TXT_LIST)) != NULL)
    {
        DEBUG(UI_IPLAY_DESC,INFO,"destory create_iptv_rsc_txt_btn\n");
        ctrl_destroy_ctrl(p_ctrl_temp);
    }  
    if(count == 0)
        return;

    p_rsc_text = ctrl_create_ctrl(CTRL_CBOX, IDC_IPTV_DESC_VIDEO_RSC_TXT_LIST, 
                             IPTV_MOVIE_RSC_MBOX_X + IPTV_MOVIE_RSC_MBOX_W + IPTV_MOVIE_RSC_INTERVAL,
                             IPTV_MOVIE_RSC_MBOX_Y,
                             (IPTV_MOVIE_RSC_MBOX_W -IPTV_MOVIE_RSC_INTERVAL )/2,
                             IPTV_MOVIE_RSC_MBOX_H, p_des_cont,
                             0);
    ctrl_set_rstyle(p_rsc_text, RSI_OTT_BUTTON_SH, RSI_OTT_BUTTON_HL, RSI_OTT_BUTTON_SH);
    ctrl_set_keymap(p_rsc_text, iptv_des_video_rsc_list_keymap);
    ctrl_set_proc(p_rsc_text, iptv_des_video_rsc_list_proc);
    cbox_set_font_style(p_rsc_text, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    cbox_set_align_style(p_rsc_text, STL_CENTER | STL_VCENTER);
    cbox_set_work_mode(p_rsc_text, CBOX_WORKMODE_DYNAMIC);
    cbox_enable_cycle_mode(p_rsc_text, TRUE);
    cbox_dync_set_count(p_rsc_text, count);
    cbox_dync_set_update(p_rsc_text, plist_cbox_update);
    cbox_dync_set_focus(p_rsc_text, ui_iptv_dat->format_index);
    ctrl_get_frame(p_rsc_text, &rc_group);

    if(count > IPTV_SCROLL_BAR_MAX)
        count = IPTV_SCROLL_BAR_MAX;

    cbox_create_up_droplist(p_rsc_text, count, 6, 0);
    cbox_droplist_set_rstyle(p_rsc_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    cbox_droplist_set_mid_rect(p_rsc_text, 0, 0, RECTW(rc_group),
                             RECTH(rc_group) * count - 0, 0);
    cbox_droplist_set_item_rstyle(p_rsc_text, &plist_item_rstyle);
    cbox_droplist_set_field_attr(p_rsc_text, STL_CENTER | STL_VCENTER,
                               RECTW(rc_group) , 0, 0);
    cbox_droplist_set_field_font_style(p_rsc_text, &plist_field_fstyle);
    //cbox_droplist_set_sbar_rstyle(p_rsc_text, RSI_OTT_SCROLL_BAR_BG, RSI_OTT_SCROLL_BAR_BG, RSI_OTT_SCROLL_BAR_BG);
    // cbox_droplist_set_sbar_mid_rstyle(p_rsc_text,
                  //                  RSI_OTT_SCROLL_BAR,
                         //           RSI_OTT_SCROLL_BAR,
                          //          RSI_OTT_SCROLL_BAR);
    ctrl_set_sts(p_rsc_text, OBJ_STS_HIDE);
}

 static void create_iptv_desc_variety_rsc_txt_btn(control_t *p_des_cont,u8 count)
 {
    control_t * p_rsc_text = NULL;
    control_t * p_ctrl_temp = NULL;
    rect_t rc_group = {0};
    list_xstyle_t plist_field_fstyle =
    {
        FSI_GRAY,
        FSI_WHITE_28,
        FSI_WHITE_28,
        FSI_WHITE_28,
        FSI_WHITE_28,
    };
    list_xstyle_t plist_item_rstyle =
    {
        RSI_OTT_BUTTON_SH,
        RSI_OTT_BUTTON_SH,
        RSI_OTT_BUTTON_HL,
        RSI_OTT_BUTTON_SH,
        RSI_OTT_BUTTON_SH
    };
    if((p_ctrl_temp = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_VARIETY_RSC_TXT_LIST)) != NULL)
    {
        DEBUG(UI_IPLAY_DESC,INFO,"destory create_iptv_rsc_txt_btn\n");
        ctrl_destroy_ctrl(p_ctrl_temp);
    }  
    if(count == 0)
        return;
    p_rsc_text = ctrl_create_ctrl(CTRL_CBOX, IDC_IPTV_DESCNT_VARIETY_RSC_TXT_LIST, 
                         IPTV_VARIETY_RSC_TENT_URL_TITLE_X,
                         IPTV_VARIETY_RSC_TENT_URL_TITLE_Y,
                         IPTV_VARIETY_RSC_TENT_URL_TITLE_W,
                         IPTV_VARIETY_RSC_TENT_URL_TITLE_H, p_des_cont,
                         0);
    ctrl_set_rstyle(p_rsc_text, RSI_OTT_BUTTON_SH, RSI_OTT_BUTTON_HL, RSI_OTT_BUTTON_SH);
    ctrl_set_keymap(p_rsc_text, iptv_des_video_rsc_list_keymap);
    ctrl_set_proc(p_rsc_text, iptv_des_video_rsc_list_proc);
    cbox_set_font_style(p_rsc_text, FSI_WHITE_28, FSI_WHITE_28, FSI_WHITE_28);
    cbox_set_align_style(p_rsc_text, STL_CENTER | STL_VCENTER);
    cbox_set_work_mode(p_rsc_text, CBOX_WORKMODE_DYNAMIC);
    cbox_enable_cycle_mode(p_rsc_text, TRUE);
    cbox_dync_set_count(p_rsc_text, count);
    cbox_dync_set_update(p_rsc_text, plist_cbox_update);
    cbox_dync_set_focus(p_rsc_text, ui_iptv_dat->format_index);
    ctrl_get_frame(p_rsc_text, &rc_group);

    if(count > IPTV_SCROLL_BAR_MAX)
        count = IPTV_SCROLL_BAR_MAX;

    cbox_create_up_droplist(p_rsc_text, count, 6, 0);
    cbox_droplist_set_rstyle(p_rsc_text, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    cbox_droplist_set_mid_rect(p_rsc_text, 0, 0, RECTW(rc_group),
                         RECTH(rc_group) * count - 0, 0);
    cbox_droplist_set_item_rstyle(p_rsc_text, &plist_item_rstyle);
    cbox_droplist_set_field_attr(p_rsc_text, STL_CENTER| STL_VCENTER,
                           RECTW(rc_group) , 0, 0);
    cbox_droplist_set_field_font_style(p_rsc_text, &plist_field_fstyle);

    //ctrl_default_proc(p_rsc_text, MSG_GETFOCUS,0,0);
    ctrl_set_sts(p_rsc_text, OBJ_STS_HIDE);
}
 

/*************************create variety  functions***********************/
static void create_iptv_desc_variety_texts(control_t *p_des_cont)
{
    control_t *p_sub = NULL;
    u16 uni_str[8];

    u32 idc_id = (u32)IDC_IPTV_DESCNT_VARIETY_DES_TXT;
    on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));

    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_VARIETY_DES_TXT, 
                  IPTV_VARIETY_RSC_TEXT_TITLE_X, IPTV_VARIETY_RSC_TEXT_TITLE_Y, 
                  IPTV_VARIETY_RSC_TEXT_TITLE_W, IPTV_VARIETY_RSC_TEXT_TITLE_H, 
                  p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_CENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_sub, uni_str);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);

    //create description text field
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_VARIETY_DES_TXT_DYN, 
                  IPTV_VARIETY_RSC_TEXT_DES_X, IPTV_VARIETY_RSC_TEXT_DES_Y, 
                  IPTV_VARIETY_RSC_TEXT_DES_W, IPTV_VARIETY_RSC_TEXT_DES_H, 
                  p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_CENTER);
    text_set_font_style(p_sub, FSI_WHITE_22, FSI_WHITE_22, FSI_WHITE_22);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    
}



static void create_iptv_desc_variety_rsc_btn(control_t *p_des_cont)
{
    control_t *p_rsc = NULL;
    u16 uni_str[8];
    u32 idc_id = (u32)IDC_IPTV_DESCNT_VARIETY_RSC_MBOX;

    if((p_rsc = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_VARIETY_RSC_MBOX)) != NULL)
    {
        ctrl_destroy_ctrl(p_rsc);
    }  
    p_rsc = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_VARIETY_RSC_MBOX,
                             IPTV_VARIETY_RSC_TENT_BUN_X, IPTV_VARIETY_RSC_TENT_BUN_Y,
                             IPTV_VARIETY_RSC_TENT_BUN_W, IPTV_VARIETY_RSC_TENT_BUN_H, p_des_cont, 0);
    ctrl_set_rstyle(p_rsc, RSI_OTT_BUTTON_SH, RSI_OTT_BUTTON_HL, RSI_OTT_BUTTON_SH);
    ctrl_set_keymap(p_rsc, iptv_des_variety_rsc_keymap);
    ctrl_set_proc(p_rsc, iptv_des_variety_rsc_proc);
    text_set_align_type(p_rsc, STL_CENTER| STL_VCENTER);
    text_set_content_type(p_rsc, TEXT_STRTYPE_UNICODE);
    text_set_font_style(p_rsc, FSI_WHITE_28, FSI_WHITE_28, FSI_WHITE_28);

    //fav or unfav
    if(ui_iptv_dat->iptv_is_fav)
    {
        on_ui_iptv_find_dis_str(idc_id,1,uni_str,sizeof(uni_str));
        text_set_content_by_unistr(p_rsc, uni_str);
    }
    else
    {
        on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));
        text_set_content_by_unistr(p_rsc, uni_str);
    }
    ctrl_set_sts(p_rsc, OBJ_STS_HIDE);

    //ctrl_set_attr(p_rsc, OBJ_STS_HIDE);
}

static void create_iptv_desc_variety_rsc(control_t *p_des_cont)
{
    control_t *p_sub = NULL;
    u16 uni_str[16];

    u32 idc_id = (u32)IDC_IPTV_DESCNT_VARIETY_RSC_NUM_TEXT;
    on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));

    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_VARIETY_RSC_NUM_TEXT, 
                      IPTV_VARIETY_RSC_TEXT_X, IPTV_VARIETY_RSC_TEXT_Y, 
                      IPTV_VARIETY_RSC_TEXT_W, IPTV_VARIETY_RSC_TEXT_H, 
                      p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_CENTER);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    text_set_content_by_unistr(p_sub, uni_str);
    text_set_font_style(p_sub, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);

    //create description text field
    p_sub = ctrl_create_ctrl(CTRL_TEXT, IDC_IPTV_DESCNT_VARIETY_RSC_NUM_DYN, 
                      IPTV_VARIETY_RSC_PER_X, IPTV_VARIETY_RSC_PER_Y, 
                      IPTV_VARIETY_RSC_PER_W, IPTV_VARIETY_RSC_PER_H, 
                      p_des_cont, 0);
    ctrl_set_rstyle(p_sub, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    text_set_align_type(p_sub, STL_LEFT | STL_CENTER);
    text_set_font_style(p_sub, FSI_WHITE, FSI_WHITE, FSI_WHITE);
    text_set_content_type(p_sub, TEXT_STRTYPE_UNICODE);
    ctrl_set_sts(p_sub, OBJ_STS_HIDE);
}


static void create_iptv_desc_variety(control_t *p_des_cont)
{
    control_t *p_list = NULL;
    //control_t *p_sbar = NULL;

    list_xstyle_t iptv_variety_list_field_fstyle =
    {
        FSI_GRAY,
        FSI_WHITE,
        FSI_WHITE,
        FSI_WHITE,
        FSI_WHITE,
    };
    list_xstyle_t iptv_variety_list_field_rstyle =
    {
        RSI_IGNORE,
        RSI_IGNORE,
        RSI_IGNORE,
        RSI_IGNORE,
        RSI_IGNORE,
    };
    list_xstyle_t iptv_variety_list_item_rstyle =
    {
        RSI_IGNORE,
        RSI_IGNORE,
        RSI_OTT_BUTTON_HL,
        RSI_OTT_BUTTON_HL,
        RSI_OTT_BUTTON_HL,
    };
    list_field_attr_t iptv_variety_list_attr[IPTV_VARIETY_RSC_LIST_FIELD] =
    {   
        { LISTFIELD_TYPE_UNISTR | STL_LEFT | STL_VCENTER,
        480, 20, 0, &iptv_variety_list_field_rstyle,  &iptv_variety_list_field_fstyle},
    };
    //List
    control_t *p_ctrl_temp = NULL;

    if((p_ctrl_temp = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_VARIETY_LIST)) != NULL)
    {
        DEBUG(UI_IPLAY_DESC,INFO,"destory create_iptv_rsc_txt_btn\n");
        ctrl_destroy_ctrl(p_ctrl_temp);
    }  

    p_list = ctrl_create_ctrl(CTRL_LIST, IDC_IPTV_DESCNT_VARIETY_LIST, 
                        IPTV_VARIETY_RSC_LIST_X, IPTV_VARIETY_RSC_LIST_Y, 
                        IPTV_VARIETY_RSC_LIST_W, IPTV_VARIETY_RSC_LIST_H, p_des_cont, 0);
    ctrl_set_rstyle(p_list, RSI_PBACK, RSI_PBACK, RSI_PBACK);
    ctrl_set_keymap(p_list, iptv_des_variety_list_keymap);
    ctrl_set_proc(p_list, iptv_des_variety_list_proc);
    ctrl_set_mrect(p_list, IPTV_VARIETY_RSC_LIST_MID_L, IPTV_VARIETY_RSC_LIST_MID_T,
    IPTV_VARIETY_RSC_LIST_MID_W + IPTV_VARIETY_RSC_LIST_MID_L, IPTV_VARIETY_RSC_LIST_MID_H + IPTV_VARIETY_RSC_LIST_MID_T);
    list_set_item_interval(p_list, IPTV_VARIETY_RSC_ITEM_VGAP);
    list_set_item_rstyle(p_list, &iptv_variety_list_item_rstyle);
    list_enable_select_mode(p_list, TRUE);
    list_set_select_mode(p_list, LIST_SINGLE_SELECT);
    list_set_count(p_list,0, IPTV_VARIETY_RSC_LIST_PAGE);
    list_set_field_count(p_list, IPTV_VARIETY_RSC_LIST_FIELD, IPTV_VARIETY_RSC_LIST_PAGE);
    list_set_update(p_list, on_iptv_variety_list_update, 0);
    list_set_focus_pos(p_list, 0);

    list_set_field_attr(p_list, 0, (u32)(iptv_variety_list_attr[0].attr), 
                        (u16)(iptv_variety_list_attr[0].width),
                        (u16)(iptv_variety_list_attr[0].left), 
                        (u8)(iptv_variety_list_attr[0].top));
    list_set_field_rect_style(p_list, 0, iptv_variety_list_attr[0].rstyle);
    list_set_field_font_style(p_list, 0, iptv_variety_list_attr[0].fstyle);

    ctrl_default_proc(p_list,MSG_GETFOCUS, 0, 0);  
}

static void create_iptv_desc_variety_container(control_t *p_menu)
{
    control_t *p_des_cont = NULL;

    p_des_cont = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_DESCNT, 
                                IPTV_DESC_CNTX, IPTV_DESC_CNTY, 
                                IPTV_DESC_CNTW, IPTV_DESC_VARIETY_CNTH, 
                                p_menu, 0);
    ctrl_set_rstyle(p_des_cont, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);

    create_iptv_desc_variety(p_des_cont);
    create_iptv_desc_variety_rsc(p_des_cont);
}

static void create_iptv_desc_variety_content(control_t *p_menu)
{
    control_t *p_des_cont_tent = NULL;

    p_des_cont_tent = ctrl_create_ctrl(CTRL_CONT, IDC_IPTV_DES_VARIETY_TENT, 
                                IPTV_VARIETY_RSC_CONTENT_CNTX, IPTV_VARIETY_RSC_CONTENT_CNTY, 
                                IPTV_VARIETY_RSC_CONTENT_CNTW, IPTV_VARIETY_RSC_CONTENT_CNTH, 
                                p_menu, 0);
    ctrl_set_rstyle(p_des_cont_tent, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG, RSI_RIGHT_CONT_BG);

    //jianjie
    create_iptv_desc_variety_texts(p_des_cont_tent);

    //bunton
    create_iptv_desc_variety_rsc_btn(p_des_cont_tent);
}

static u8  ui_iptv_choose_cate_id(ui_iptv_description_param_t *param)
{
    u8 category = 0;

    DEBUG(UI_IPLAY_DESC,INFO,"in ...\n");
    if (param->vdo_id.type == IQY_TYPE_MOVE)
        category = IPTV_MOVIE;
    else if (param->vdo_id.type == IQY_TYPE_TV)
        category = IPTV_TV;
    else
        category = IPTV_VARIETY;

    return category;
#if 0
  if (is_single_page)
  {
    category = IPTV_MOVIE;
  }
  else
  {
    switch(res_id)
    {
      case 10:
        category = IPTV_MOVIE;
        break;
      case 12:
        category = IPTV_VARIETY;
        break;
      default:
        category = IPTV_TV;
        break;
    }
  }
#endif
}

static RET_CODE ui_iptv_on_open_dlg(void)
{
    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_IPTV_DESCRIPTION,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H,
        IDS_HD_DOWNLOAD_TO_WAIT,
        0,
    };
    ui_comm_dlg_open(&dlg_data);
    return SUCCESS;
}
RET_CODE open_iptv_description(u32 para1, u32 para2)
{
    control_t *p_menu = NULL;
    control_t *p_desc_cont = NULL;
    ui_iptv_description_param_t *p_param = (ui_iptv_description_param_t*)para1;

    DEBUG(UI_IPLAY_DESC,INFO,"@@@open_iptv_description\n");

    ui_pic_init(PIC_SOURCE_NET);
    on_ui_iptv_init_data();

	set_iptv_des_state(IPTV_DESC_INIT);
    ui_iptv_dat->g_parent_root_id = para2;

    ui_iptv_dat->category_id = p_param->res_id;
    ui_iptv_dat->video_id = p_param->vdo_id;
    DEBUG(UI_IPLAY_DESC,INFO,"vdo_id cat id[%d] qpid[%s],tvQid[%s]\n", 
            ui_iptv_dat->category_id,ui_iptv_dat->video_id.qpId, ui_iptv_dat->video_id.tvQid);

    ui_iptv_dat->b_single_page = p_param->b_single_page;

    ui_iptv_dat->category = ui_iptv_choose_cate_id(p_param);

    ui_iptv_dat->playbar_is_back = FALSE;
    // TODO: favlist need update SY
    ui_iptv_dat->iptv_is_fav = get_iptv_is_fav(&ui_iptv_dat->video_id,&(ui_iptv_dat->iptv_fav_focus));

    DEBUG(UI_IPLAY_DESC,INFO,"ui_iptv_dat->video_id qpid[%s], type[%d]\n",ui_iptv_dat->video_id.qpId,ui_iptv_dat->category);
    p_menu = create_iptv_desc_menu();

    p_desc_cont = create_iptv_desc_cont(p_menu);

    create_iptv_desc_title(p_desc_cont);
    //score
    create_iptv_desc_score(p_desc_cont);
    //picture
    create_iptv_desc_picture(p_menu);

    if(ui_iptv_dat->category == IPTV_TV)
    { 
        create_iptv_desc_tv_series_container(p_menu);
        create_recommend_container(p_menu);
    }
    else if(ui_iptv_dat->category == IPTV_MOVIE)
    {
        create_iptv_desc_movie_container(p_menu);
        create_recommend_container(p_menu);
    }
    else if(ui_iptv_dat->category == IPTV_VARIETY)
    {
        create_iptv_desc_variety_container(p_menu);
        create_iptv_desc_variety_content(p_menu);
    }

    ctrl_paint_ctrl(ctrl_get_root(p_menu), TRUE);

    ui_iptv_on_open_dlg();

    on_update_ui_content();
    return SUCCESS;
}

/*************************create end ***********************/


      

      
      
static void ui_iptv_save_img_url(u8 *p_url)
{ 
    u8 *p_iptv_url = NULL;
    p_iptv_url = SY_MALLOC(strlen(p_url) + 1);
    memcpy(p_iptv_url,p_url,strlen(p_url) + 1);
    ui_iptv_dat->p_img_url = p_iptv_url;
    return;
}


 static RET_CODE on_iptv_hide_des_info(control_t *p_ctrl)
 {
    control_t *p_des_title_cont = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESC_CONT);
    control_t *p_des_cont = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESCNT);
    control_t *p_des = NULL;
    u8 i = 0;
    if(p_des_title_cont == NULL || p_des_cont == NULL)
    {
        DEBUG(UI_IPLAY_DESC,INFO,"p_ctrl is nuLL\n");
        return SUCCESS;
    }

    for(i = IDC_IPTV_DESC_TITLE;i < IDC_IPTV_DESC_TXT_SCORE + 1;i++)
    {
        p_des = ctrl_get_child_by_id(p_des_title_cont, i);
        if(p_des != NULL)
            ctrl_set_sts(p_des, OBJ_STS_HIDE);
    }
    if(ui_iptv_dat->category == IPTV_TV)
    {
        for(i = IDC_IPTV_DESCNT_VDO_SERIES;i < IDC_IPTV_DESC_SERIES_MBOX + 1;i++)
        {
            p_des = ctrl_get_child_by_id(p_des_cont, i);
            if(p_des != NULL)
                ctrl_set_sts(p_des, OBJ_STS_HIDE);
        }
    }
    if(ui_iptv_dat->category == IPTV_MOVIE)
    {
        for(i = IDC_IPTV_DESCNT_MOVIE_ACTOR;i < IDC_IPTV_DESCNT_MOVIE_RSC_MBOX + 1; i++)
        {
            p_des = ctrl_get_child_by_id(p_des_cont, i);
            if(p_des != NULL)
                ctrl_set_sts(p_des, OBJ_STS_HIDE);
        }
        p_des = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESC_VIDEO_RSC_TXT_LIST);
        if(p_des != NULL)
            ctrl_set_sts(p_des, OBJ_STS_HIDE);       
    }
    ctrl_paint_ctrl(p_des_cont, TRUE);
    ctrl_paint_ctrl(p_des_title_cont, TRUE);
    return SUCCESS;
 }
 
void ui_iptv_des_info_fail_exit(void)
{
    pic_stop();
    ui_pic_release();
    manage_close_menu(ROOT_ID_IPTV_DESCRIPTION, 0, 0);
    return;
}
static RET_CODE on_iptv_des_get_vdo_info_fail(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
     ui_comm_dlg_close();
     ui_comm_cfmdlg_open(NULL, IDS_HD_DATA_ERROR, ui_iptv_des_info_fail_exit, 2000);
     return SUCCESS;
}

static RET_CODE on_iptv_des_vdo_info_arrive(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
   al_iptv_vdo_info_t *p_data = (al_iptv_vdo_info_t *)(para1);
   control_t *p_des_cont = NULL;
   control_t *p_des_type = NULL;
   control_t *p_des_temp= NULL;
   control_t *p_des_pic = NULL;
   control_t *p_des_title = NULL;
   control_t *p_active_cont = NULL;
   
   rect_t rect;
   u8 i = 0;
   u16 ctrl_id = 0;
   iptv_fav_info  iptv_info = {0};

   ui_iptv_dat->iptv_url_c = FALSE;
   ui_iptv_dat->url_arrive_semph = 0;

   ui_iptv_dat->rec_pic_index = 0;
   memset(ui_iptv_dat->rsc_uni_str,0,sizeof(ui_iptv_dat->rsc_uni_str));
   g_draw_rec = FALSE;

   if((strcmp(p_data->vdo_id.qpId, ui_iptv_dat->video_id.qpId) != 0)
   		&&(strcmp(p_data->vdo_id.tvQid, ui_iptv_dat->video_id.tvQid) != 0))
   {
   		return SUCCESS;
   }
	
  ui_comm_dlg_close();
  on_ui_iptv_init_play_data();    
  
  if(p_data->img_url != NULL)
  {
     ui_iptv_save_img_url(p_data->img_url);
  }
  
   for(i = 0;i < p_data->orgn_cnt;i++)
   {
      if(p_data->orgnList != NULL)
      {
         if(p_data->orgnList[i].name != NULL)
         {
            uni_strcpy(&ui_iptv_dat->rsc_uni_str[i][0],p_data->orgnList[i].name);
         }
      }
   }
 
   p_des_pic = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESC_PICTURE);
    if(p_des_pic != NULL)
    {
      if(p_data->img_url != NULL)
      {
      	   // TODO:  indedfify
          ui_iptv_dat->identify = (0xFFFF << 16) | (u16)ui_iptv_dat->video_id.qpId[0];
          ctrl_get_frame(p_des_pic, &rect);
          ctrl_client2screen(p_des_pic, &rect);
          ui_pic_play_by_url(p_data->img_url, &rect,ui_iptv_dat->identify);
      }
      else
      {
           g_draw_rec = TRUE;
      }
    }
    p_des_cont = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESC_CONT);
    p_des_title = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESC_TITLE);
   if(p_data->name != NULL)
   {
      memcpy(ui_iptv_dat->iptv_fav_name,p_data->name,sizeof(iptv_info.iptv_fav_name));

      if(p_des_title != NULL)
      {
        text_set_content_by_unistr(p_des_title, p_data->name);
        ctrl_set_sts(p_des_title, OBJ_STS_SHOW);
        ctrl_paint_ctrl(p_des_title,TRUE);
      }
   }


    p_des_cont = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESC_CONT);
    p_des_type = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESC_SCORE);
    if(p_des_type)
    {
    ctrl_set_sts(p_des_type, OBJ_STS_SHOW);
    ctrl_paint_ctrl(p_des_type,TRUE);
    }
    
   if(p_data->score != NULL)
   {
      p_des_type = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESC_TXT_SCORE);
      if(p_des_type != NULL)
      {
      text_set_content_by_unistr(p_des_type, p_data->score);
      ctrl_set_sts(p_des_type, OBJ_STS_SHOW);
      ctrl_paint_ctrl(p_des_type, TRUE);
      }
   }
   else
   {
      p_des_type = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESC_TXT_SCORE);
      if(p_des_type != NULL)
      {
      u16 uni_str[16] = {0};
      convert_gb2312_chinese_asc2unistr("暂无", uni_str, sizeof(uni_str));
      text_set_content_by_unistr(p_des_type, uni_str);
      ctrl_set_sts(p_des_type, OBJ_STS_SHOW);
      ctrl_paint_ctrl(p_des_type, TRUE);
      }
   }
   
   p_des_cont = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESCNT);
   if(p_des_cont == NULL)
   {
     return SUCCESS;
   }

   ui_iptv_dat->total_format = p_data->format->total_format;
   ui_iptv_dat->format_index = ui_iptv_dat->total_format >1?1:0;
   DEBUG(UI_IPLAY_DESC,INFO,"format total [%d]\n",ui_iptv_dat->total_format);   
   for (i=0;i<ui_iptv_dat->total_format && i <ARRAY_SIZE(ui_iptv_dat->format_list);i++)
   {
   	ui_iptv_dat->format_list[i] = p_data->format->formatList[i];
       DEBUG(UI_IPLAY_DESC,INFO,"format[%d]\n",ui_iptv_dat->format_list[i]);
   }
   if(ui_iptv_dat->category == IPTV_TV)
   {
       p_des_temp = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_VDO_SERIES);
       if(p_des_temp)
           ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);

       if(p_data->tvsets!= 0)
       {
         p_des_type = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_VDO_TXT_SERIES);
         if(p_des_type != NULL)
         {
           u8 buf[16] = {0};
           u16 uni_str[16] = {0};
           
           sprintf(buf,"%d 集",(p_data->tvsets));
           convert_gb2312_chinese_asc2unistr(buf, uni_str,sizeof(buf));
           text_set_content_by_unistr(p_des_type, uni_str);
           ctrl_set_sts(p_des_type, OBJ_STS_SHOW);
         }
       }

       if(uni_strlen(p_data->director) > 1)
       {
           p_des_temp = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_VDO_DIRECTER);
           if(p_des_temp)
           ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);

           if(p_data->director!= NULL)
           {
                p_des_type = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_VDO_TXT_DIRECTER);
                if(p_des_type != NULL)
                {
                   // uni_strcpy((u16 *)ui_iptv_dat->iptv_area,p_data->area);
                    text_set_content_by_unistr(p_des_type, p_data->director);
                    ctrl_set_sts(p_des_type, OBJ_STS_SHOW);
                }
           }

           p_des_temp = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_VDO_ACTOR);
           if(p_des_temp)
           ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);

           if(p_data->actor != NULL)
           {
           p_des_type = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_VDO_TXT_ACTOR);
           text_set_content_by_unistr(p_des_type, p_data->actor);
           ctrl_set_sts(p_des_type, OBJ_STS_SHOW);
           }
       }
       else
       {
           p_des_temp = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_VDO_ATTR);
           if(p_des_temp)
           ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);

           if(p_data->attr!= NULL)
           {
           p_des_type = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_VDO_TXT_ATTR);
           text_set_content_by_unistr(p_des_type, p_data->attr);
           ctrl_set_sts(p_des_type, OBJ_STS_SHOW);
           }
       }

       p_des_temp = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_VDO_YEAR);
       if(p_des_temp)
       ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);

       if(p_data->years != NULL)
       {
            u16 uni_str[32] = {0};
            u16 uni_year[8] = {0};
            convert_gb2312_chinese_asc2unistr("年", uni_year,sizeof(uni_year));
            uni_strcat(uni_str,p_data->years,sizeof(uni_str));
            p_des_type = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_VDO_TXT_YEAR);
            text_set_content_by_unistr(p_des_type, uni_str);
            ctrl_set_sts(p_des_type, OBJ_STS_SHOW);
       }

       p_des_temp =  ctrl_get_child_by_id(p_des_cont,IDC_IPTV_DESCNT_TXT_TITLE);
       if(p_des_temp)
       ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);

       if(p_data->description != NULL)
       {
       p_des_type = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_TXT);
       if(uni_strlen(p_data->description) > 512)
       {
           p_data->description[512] = '\0';
       } 
       text_set_content_by_unistr(p_des_type, p_data->description);
       ctrl_set_sts(p_des_type, OBJ_STS_SHOW);
       }
	   
	   if((p_active_cont = ctrl_get_active_ctrl(p_des_cont)) != NULL)
	   {
			ctrl_id = ctrl_get_ctrl_id(p_active_cont);
			DEBUG(UI_IPLAY_DESC,INFO," in \n");
	   }
	   else
		DEBUG(UI_IPLAY_DESC,INFO," in \n");

        create_iptv_desc_video_rsc_tv_btn(p_des_cont);
        p_des_temp = ctrl_get_child_by_id(p_des_cont,IDC_IPTV_DESC_VIDEO_RSC_MBOX);
        if(p_des_temp)
        ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);

        create_iptv_desc_rsc_txt_tv_btn(p_des_cont,ui_iptv_dat->total_format);
       
        p_des_temp = ctrl_get_child_by_id(p_des_cont,IDC_IPTV_DESC_VIDEO_RSC_TXT_LIST);
        if(p_des_temp)
        ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);

		if(p_active_cont != NULL)
		{
		  p_des_temp = ctrl_get_child_by_id(p_des_cont, ctrl_id);
		  ctrl_process_msg(p_des_temp, MSG_GETFOCUS, 0, 0);
		}

        ctrl_paint_ctrl(p_des_cont,TRUE);          
    }
   else if(ui_iptv_dat->category == IPTV_MOVIE)
   {
        p_des_temp = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_MOVIE_ACTOR);
        if(p_des_temp)
            ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);
        if(p_data->actor != NULL)
       {
           p_des_type = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_MOVIE_TXT_ACTOR);
           text_set_content_by_unistr(p_des_type, p_data->actor);
           ctrl_set_sts(p_des_type, OBJ_STS_SHOW);
       }
       
       p_des_temp = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_MOVIE_DURATION);
       if(p_des_temp)
            ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);
       if(p_data->time!= 0)
       {
           u8 buf[16] = {0};
           u16 uni_str[16] = {0};
           
           sprintf(buf,"%ld 分钟",(p_data->time/60));
           convert_gb2312_chinese_asc2unistr(buf, uni_str,sizeof(buf));
           p_des_type = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_MOVIE_TXT_DURATION);
           text_set_content_by_unistr(p_des_type, (u16 *)uni_str);
           ctrl_set_sts(p_des_type, OBJ_STS_SHOW);
       }
       
       p_des_temp = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_MOVIE_DIRECTER);
       ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);
       if(p_data->area != NULL)
       {
           p_des_type = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_MOVIE_TXT_DIRECTER);
           text_set_content_by_unistr(p_des_type, p_data->director);
           ctrl_set_sts(p_des_type, OBJ_STS_SHOW);
       }

       p_des_temp = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_MOVIE_YEAR);
       if(p_des_temp)
            ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);
       if(p_data->years)
       {
           u16 uni_str[32] = {0};
           u16 uni_year[8] = {0};
           convert_gb2312_chinese_asc2unistr("年", uni_year,sizeof(uni_year));
           uni_strcat(uni_str,p_data->years,sizeof(uni_str));
           uni_strcat(uni_str,uni_year,sizeof(uni_str));

           p_des_type = ctrl_get_child_by_id(p_des_cont, IDC_IPTV_DESCNT_MOVIE_TXT_YEAR);
           text_set_content_by_unistr(p_des_type,uni_str);
           ctrl_set_sts(p_des_type, OBJ_STS_SHOW);
       }
       
       p_des_temp =  ctrl_get_child_by_id(p_des_cont,IDC_IPTV_DESCNT_MOVIE_TXT_TITLE);
       ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);
       if(p_data->description != NULL)
       {  
           p_des_type =  ctrl_get_child_by_id(p_des_cont,IDC_IPTV_DESCNT_MOVIE_TXT);
           if(uni_strlen(p_data->description) > 512)
           {
              p_data->description[512] = '\0';
           }
           text_set_content_by_unistr(p_des_type, p_data->description);
           ctrl_set_sts(p_des_type, OBJ_STS_SHOW);
       }

	  if((p_active_cont = ctrl_get_active_ctrl(p_des_cont)) != NULL)
	  {
		   ctrl_id = ctrl_get_ctrl_id(p_active_cont);
		   DEBUG(UI_IPLAY_DESC,INFO," in \n");
	  }
	  else
	   DEBUG(UI_IPLAY_DESC,INFO," in \n");

      create_iptv_desc_movie_rsc_btn(p_des_cont); 
      p_des_temp = ctrl_get_child_by_id(p_des_cont,IDC_IPTV_DESCNT_MOVIE_RSC_MBOX);
      if(p_des_temp)
           ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);

      create_iptv_desc_movie_rsc_txt_btn(p_des_cont,ui_iptv_dat->total_format);

      p_des_temp = ctrl_get_child_by_id(p_des_cont,IDC_IPTV_DESC_VIDEO_RSC_TXT_LIST);
      if(p_des_temp)
        ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);

	  if(p_active_cont != NULL)
	  {
		p_des_temp = ctrl_get_child_by_id(p_des_cont, ctrl_id);
		ctrl_process_msg(p_des_temp, MSG_GETFOCUS, 0, 0);
	  }

      ctrl_paint_ctrl(p_des_cont,TRUE);
   }
   else if(ui_iptv_dat->category == IPTV_VARIETY)
   {
   p_des_cont = ctrl_get_child_by_id(p_ctrl,IDC_IPTV_DES_VARIETY_TENT);
   if(p_data->description != NULL)
   {
     p_des_type = ctrl_get_child_by_id(p_des_cont,IDC_IPTV_DESCNT_VARIETY_DES_TXT);
     if(p_des_type)
     ctrl_set_sts(p_des_type, OBJ_STS_SHOW);

     p_des_type = ctrl_get_child_by_id(p_des_cont,IDC_IPTV_DESCNT_VARIETY_DES_TXT_DYN);
     if(p_des_type)
     {
      if(uni_strlen(p_data->description) > 512)
      {
          p_data->description[512] = '\0';
      }       
       text_set_content_by_unistr(p_des_type, p_data->description);
     }
   }

   if((p_active_cont = ctrl_get_active_ctrl(p_des_cont)) != NULL)
   {
		ctrl_id = ctrl_get_ctrl_id(p_active_cont);
		DEBUG(UI_IPLAY_DESC,INFO," in \n");
   }
   else
   	DEBUG(UI_IPLAY_DESC,INFO," in \n");
   
  create_iptv_desc_variety_rsc_btn(p_des_cont);
  p_des_temp = ctrl_get_child_by_id(p_des_cont,IDC_IPTV_DESCNT_VARIETY_RSC_MBOX);
  if(p_des_temp)
  ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);
  
  create_iptv_desc_variety_rsc_txt_btn(p_des_cont,ui_iptv_dat->total_format);
  p_des_temp = ctrl_get_child_by_id(p_des_cont,IDC_IPTV_DESCNT_VARIETY_RSC_TXT_LIST);
  if(p_des_temp)
  ctrl_set_sts(p_des_temp, OBJ_STS_SHOW);

  if(p_active_cont != NULL)
  {
  	p_des_temp = ctrl_get_child_by_id(p_des_cont, ctrl_id);
	ctrl_process_msg(p_des_temp, MSG_GETFOCUS, 0, 0);
  }
  ctrl_paint_ctrl(p_des_cont,TRUE);
 }
 memset( ui_iptv_dat->url_state,0,IPTV_ORIGINS_MAX * sizeof(ui_iptv_info_url_t));
 memset( ui_iptv_dat->url_start,0,IPTV_ORIGINS_MAX * sizeof(u16));
 
 ui_iptv_dat->url_state[0] = IPTV_INFO_URL_START;
 ui_iptv_get_info_url(&ui_iptv_dat->video_id,ui_iptv_dat->category_id, 
                          &ui_iptv_dat->rsc_uni_str[ 0][0],  
                           1, IPTV_GET_PAGE_SIZE);
  ++ui_iptv_dat->url_arrive_semph;

   set_iptv_des_state(IPTV_DESC_INFO_ARRIVE);
   return SUCCESS;
}


static RET_CODE on_iptv_des_series_info_url_arrive_fail(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
    al_iptv_play_origin_info_t *p_data_iptv = (al_iptv_play_origin_info_t *)on_ui_iptv_get_play_data();

    if(ui_iptv_dat->url_arrive_semph > 1)
    {
        --ui_iptv_dat->url_arrive_semph;
        if(p_data_iptv->origin == NULL)
        {
            ui_iptv_get_info_url(&ui_iptv_dat->video_id, ui_iptv_dat->category_id,
                    &ui_iptv_dat->rsc_uni_str[0][0], 
                    ui_iptv_dat->url_start[0]/IPTV_GET_PAGE_SIZE + 1, 
                    IPTV_GET_PAGE_SIZE);
            ui_iptv_dat->url_state[0] = IPTV_INFO_URL_START;
        }
        else if(ui_iptv_dat->url_state[0] == IPTV_INFO_URL_CONTINUE)
        {
            ui_iptv_get_info_url(&ui_iptv_dat->video_id, ui_iptv_dat->category_id,
                    &ui_iptv_dat->rsc_uni_str[0][0], 
                    ui_iptv_dat->url_start[0]/IPTV_GET_PAGE_SIZE + 1, 
                    IPTV_GET_PAGE_SIZE);
        } 
        return SUCCESS;
    }
    --ui_iptv_dat->url_arrive_semph;
    return SUCCESS;
}

//此函数循坏获取ui_iptv_get_info_url 
static RET_CODE on_iptv_des_series_info_url_arrive(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
    control_t *p_des_cont = NULL;
    //control_t *p_des_variety_tent = NULL;
    al_iptv_info_url_list_t *p_data = (al_iptv_info_url_list_t *)(para1);
    al_iptv_play_origin_info_t *p_data_iptv = (al_iptv_play_origin_info_t *)on_ui_iptv_get_play_data();
    u32 i,j;
    control_t *origin_txt = NULL;
    control_t *p_des_variety_cont = NULL;
    control_t *p_des_variety_cont_list = NULL;
    u8 utf_str[8] = {0};
    u16 uni_str[32] = {0};

	if(get_iptv_des_state() == IPTV_DESC_INIT)
		return SUCCESS;
	
	set_iptv_des_state(IPTV_DESC_URL_ARRIVE);
	
    if(ui_iptv_dat->url_arrive_semph > 1)
    {
        --ui_iptv_dat->url_arrive_semph;
        if(p_data_iptv->origin == NULL)
        {
            ui_iptv_get_info_url(&ui_iptv_dat->video_id, ui_iptv_dat->category_id,
                    &ui_iptv_dat->rsc_uni_str[0][0], 
                    ui_iptv_dat->url_start[0]/IPTV_GET_PAGE_SIZE + 1, 
                    IPTV_GET_PAGE_SIZE);
            ui_iptv_dat->url_state[0] = IPTV_INFO_URL_START;
        }
        else if(ui_iptv_dat->url_state[0] == IPTV_INFO_URL_CONTINUE)
        {
            ui_iptv_get_info_url(&ui_iptv_dat->video_id, ui_iptv_dat->category_id,
                    &ui_iptv_dat->rsc_uni_str[0][0], 
                    ui_iptv_dat->url_start[0]/IPTV_GET_PAGE_SIZE + 1, 
                    IPTV_GET_PAGE_SIZE);
        } 
        return SUCCESS;
    }

   --ui_iptv_dat->url_arrive_semph;


    ui_iptv_dat->total_episode = p_data->page_total;
    ui_iptv_dat->iptv_url_c = TRUE;

    if(p_data_iptv == NULL)
    {
        return SUCCESS;
    }

    if(p_data->page_total == 0)
    {
        return SUCCESS;
    }

	#if 0 //Don't compare episode name
	if(p_data->urlList[0].urltitle != NULL)
	{
		if(uni_strncmp(ui_iptv_dat->iptv_fav_name, p_data->urlList[0].urltitle,uni_strlen(ui_iptv_dat->iptv_fav_name)) != 0)
		{
			return SUCCESS;
		}
	}
	#endif

  
    if(ui_iptv_dat->category!=IPTV_VARIETY)
    { 
        p_des_cont = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESCNT);
        origin_txt = ctrl_get_child_by_id(p_des_cont,IDC_IPTV_DESC_VIDEO_RSC_TXT_LIST);
        if(origin_txt == NULL)
        {
            return SUCCESS;
        }

        if(ui_iptv_dat->category == IPTV_TV)
        {
            if(ctrl_get_child_by_id(p_des_cont,IDC_IPTV_DESC_SERIES_NUM_MBOX) == NULL ||
                ctrl_get_sts(ctrl_get_child_by_id(p_des_cont,
                IDC_IPTV_DESC_SERIES_NUM_MBOX)) == OBJ_STS_HIDE)
            {
                create_iptv_desc_series_tv_num(p_des_cont, p_data->page_total);
                create_iptv_desc_tv_mbox(p_des_cont);
                ctrl_paint_ctrl(p_des_cont, TRUE);
            }
        }
    }
    else
    {
        p_des_variety_cont = ctrl_get_child_by_id(p_ctrl,IDC_IPTV_DESCNT);
        p_des_variety_cont_list = ctrl_get_child_by_id(p_des_variety_cont,IDC_IPTV_DESCNT_VARIETY_LIST);

        p_des_cont = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DES_VARIETY_TENT);
        origin_txt = ctrl_get_child_by_id(p_des_cont,IDC_IPTV_DESCNT_VARIETY_RSC_TXT_LIST);
        if(origin_txt == NULL)
        {
            return SUCCESS;
        }
    }
    
    if(p_data_iptv->origin == NULL)
    {
        p_data_iptv->origin = 
            SY_MALLOC(sizeof(ui_iptv_dat->rsc_uni_str[0]));
        
        memcpy(p_data_iptv->origin, &ui_iptv_dat->rsc_uni_str[0][0],
        sizeof(ui_iptv_dat->rsc_uni_str[0]));
        p_data_iptv->url_count = p_data->page_total;
        p_data_iptv->play_origin_info = SY_MALLOC(p_data->page_total* sizeof(al_iptv_play_origin_item_t));
        memset(p_data_iptv->play_origin_info,0,p_data->page_total * sizeof(al_iptv_play_origin_item_t));
        ui_iptv_dat->url_state[0] = IPTV_INFO_URL_ARRIVE;
        ui_iptv_dat->url_start[0] = 0;
    }
       
    for(j = ui_iptv_dat->url_start[0],
            i = 0;((j<ui_iptv_dat->url_start[0] + p_data->count)&&(ui_iptv_dat->url_start[0] + p_data->count<=p_data_iptv->url_count));
            j++,i++)
    {
        memcpy(p_data_iptv->play_origin_info[j].tvQid,
                p_data->urlList[i].tvQid,
                sizeof(p_data_iptv->play_origin_info[j].tvQid));
        memcpy(p_data_iptv->play_origin_info[j].vid,
                p_data->urlList[i].vid,
                sizeof(p_data_iptv->play_origin_info[j].vid));		 

        if(ui_iptv_dat->category == IPTV_TV)
        {
            ;
        }
        
        if(ui_iptv_dat->category == IPTV_VARIETY)
        {
            if(p_data->urlList[i].urltitle != NULL)
            {
                DEBUG(UI_IPLAY_DESC,INFO,"urltile list [%d] = %d,%x\n",i,uni_strlen(p_data->urlList[i].urltitle),p_data->urlList[i].urltitle);
                p_data_iptv->play_origin_info[j].urltitle= 
                        (u16*)SY_MALLOC(uni_strlen(p_data->urlList[i].urltitle) * 2 + 2);
                memset(p_data_iptv->play_origin_info[j].urltitle,0, 
                uni_strlen(p_data->urlList[i].urltitle) * 2 + 2);
                uni_strcpy(p_data_iptv->play_origin_info[j].urltitle, p_data->urlList[i].urltitle);
            }
            else
            {
                ;
            }
        }
    }
            
     ui_iptv_dat->url_start[0] += p_data->count;

    if(ui_iptv_dat->url_start[0] < p_data->page_total)
    {
      ui_iptv_dat->url_state[0] = IPTV_INFO_URL_CONTINUE;
      ui_iptv_get_info_url(&ui_iptv_dat->video_id,ui_iptv_dat->category_id, 
          &ui_iptv_dat->rsc_uni_str[0][0], 
          ui_iptv_dat->url_start[0]/IPTV_GET_PAGE_SIZE + 1, IPTV_GET_PAGE_SIZE);
      ++ui_iptv_dat->url_arrive_semph;
    }
    else
    {
      ui_iptv_dat->url_state[0] = IPTV_INFO_URL_END;
    #if 0 //dont delete this code 
    shell_sort(p_data_iptv[orgin_focus].play_origin_info, p_data_iptv[orgin_focus].url_count,
        sizeof(al_iptv_play_origin_item_t), 
        replace_info, 
        ui_compare_data_info_for_sort_collect);
    #endif    
     }
      if(ui_iptv_dat->category!=IPTV_VARIETY)
      { 
        if(ui_iptv_dat->url_state[0] == IPTV_INFO_URL_END)
        {
            if(ui_iptv_dat->iptv_entf != TRUE)
            {
            	 DEBUG(UI_IPLAY_DESC,INFO,"get recommand cat id[%d]\n",ui_iptv_dat->category_id);
               ui_iptv_get_recmnd_info(&ui_iptv_dat->video_id, ui_iptv_dat->category_id, (u16 *)ui_iptv_dat->iptv_area);
            }
            ui_iptv_dat->iptv_entf = TRUE;
        }
      }
      else
      {  
        origin_txt = ctrl_get_child_by_id(p_des_variety_cont,
        IDC_IPTV_DESCNT_VARIETY_RSC_NUM_TEXT);
        if(origin_txt)
        {
        if(ctrl_get_sts(origin_txt) == OBJ_STS_HIDE)
        {
          ctrl_set_sts(origin_txt, OBJ_STS_SHOW);
        }
        }
        origin_txt = ctrl_get_child_by_id(p_des_variety_cont,IDC_IPTV_DESCNT_VARIETY_RSC_NUM_DYN);
        if(origin_txt)
        {
        if(ctrl_get_sts(origin_txt) == OBJ_STS_HIDE)
        {   
            sprintf(utf_str,"%d/%d",(int)1,(int)p_data->page_total);
            convert_gb2312_chinese_asc2unistr(utf_str, uni_str, sizeof(uni_str));
            text_set_content_by_unistr(origin_txt, uni_str);
            ctrl_set_sts(origin_txt, OBJ_STS_SHOW);
            }
        }
        
        if(list_get_field_content(p_des_variety_cont_list, 0,0) == 0 &&  
          ui_iptv_dat->iptv_entf == FALSE)
         {
            list_set_count(p_des_variety_cont_list,p_data->page_total, IPTV_VARIETY_RSC_LIST_PAGE);   
            for (i = 0; i < IPTV_VARIETY_RSC_LIST_PAGE; i++)
            {
              if (i < p_data->page_total)
              {
                  list_set_field_content_by_unistr(p_des_variety_cont_list, (u16) i, 
                  0, p_data_iptv->play_origin_info[i].urltitle);
              }
            }
            i = list_get_focus_pos(p_des_variety_cont_list);
            list_set_focus_pos(p_des_variety_cont_list, (i == 0xFFFF)?0:i);
            ui_iptv_dat->iptv_entf = TRUE;
        }
       ctrl_paint_ctrl(p_des_variety_cont, TRUE);
    }
      
  return SUCCESS;
}

static RET_CODE on_iptv_des_series_play_url(void)
{
    control_t *p_des_tent = NULL;
    control_t *p_ctrl = NULL;
    ui_iptv_player_param param;
    RET_CODE ret = ERR_FAILURE;
	
    memset(&param, 0, sizeof(param));

    p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_IPTV_DESCRIPTION,IDC_IPTV_DESC_CONT);
    p_des_tent = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESC_TITLE);
    if(p_des_tent == NULL)
    {
      return SUCCESS;
    }
    param.pg_name = (u16 *)text_get_content(p_des_tent);

#if 0
    if(ui_iptv_dat->category == IPTV_VARIETY)
    {
       p_des_tent = ctrl_get_child_by_id(ui_comm_root_get_ctrl(ROOT_ID_IPTV_DESCRIPTION,IDC_IPTV_DESCNT),
                                 IDC_IPTV_DESCNT_VARIETY_LIST);
       if(p_des_tent == NULL)
       {
          return SUCCESS;
       }
       param.sub_name = (u16 *)list_get_field_content(p_des_tent, list_get_focus_pos(p_des_tent),0);
    }
#endif
    param.episode_num = ui_iptv_dat->episode_num;
    param.play_time = ui_iptv_dat->play_time;
    param.vdo_id = ui_iptv_dat->video_id;
    param.res_id = ui_iptv_dat->category_id;
    param.b_single_page = ui_iptv_dat->b_single_page;
    param.category = ui_iptv_dat->category;
    param.total_episode = ui_iptv_dat->total_episode;
    param.cur_format= ui_iptv_dat->format_list[ui_iptv_dat->format_index];
    pic_stop();
    ui_pic_release();
    DEBUG(UI_IPLAY_DESC,INFO,"play episode_num[%d],play_time[%d]\r\n",param.episode_num,param.play_time);
    ret = manage_open_menu(ROOT_ID_IPTV_PLAYER, (u32)&param, 0);
    return SUCCESS;
}



static RET_CODE on_iptv_des_series_new_rec_info_arrive(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
	u32 i =0;
	al_iptv_recomand_info_t *p_temp_data= (al_iptv_recomand_info_t *)(para1);
	iptv_recomand_info_t *p_rec_data =NULL;
	u8 *p_iptv_less = NULL;
	control_t *p_cont = NULL; 
	DEBUG(UI_IPLAY_DESC,INFO,"\r\n");

	set_iptv_des_state(IPTV_DESC_RECOMM_ARRIVE);
	if(ui_iptv_dat->iptv_entf == FALSE)
	{
		return SUCCESS;
	}
	DEBUG(UI_IPLAY_DESC,INFO,"\r\n");

	on_ui_iptv_init_recm_data();
	p_rec_data = (iptv_recomand_info_t *)ui_iptv_dat->p_recomm_data;

	p_rec_data->number = p_temp_data->number;

	if(p_rec_data->number  == 0)
	{
		return SUCCESS;
	}
	DEBUG(UI_IPLAY_DESC,INFO,"\r\n");

	if(p_rec_data->number>0)
	{
		p_rec_data->recmmd = (iptv_recmd_channel *)SY_MALLOC((p_rec_data->number)*sizeof(iptv_recmd_channel));
		memset(p_rec_data->recmmd,0,sizeof(p_rec_data->recmmd));
		for(i=0;i<p_rec_data->number;i++)
		{
			p_rec_data->recmmd[i].id = p_temp_data->recmd[i].id;
			memcpy(p_rec_data->recmmd[i].title, \
			p_temp_data->recmd[i].title, sizeof(p_rec_data->recmmd[i].title));
			strncpy(p_rec_data->recmmd[i].img, \
			p_temp_data->recmd[i].img, sizeof(p_rec_data->recmmd[i].img));

		}
	}
	DEBUG(UI_IPLAY_DESC,INFO,"\r\n");

	p_iptv_less = SY_MALLOC(IPTV_DESC_RECOMM_INFO_NUM * sizeof(u8));
	memset(p_iptv_less,0,IPTV_DESC_RECOMM_INFO_NUM * sizeof(u8));
	ui_iptv_dat->p_iptv_img_less = p_iptv_less;

	if(g_draw_rec == TRUE)
	{
		create_recommend_info(p_ctrl,p_rec_data->number);  
		p_cont = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESC_RECOM_INFO_CONT);
		if(p_cont == NULL)
			return SUCCESS;
		on_iptv_update_recomm(p_cont, 0,IPTV_DESC_RECOMM_INFO_NUM,0);

		on_iptv_draw_recomm_icon(p_cont, 0);
	}
	DEBUG(UI_IPLAY_DESC,INFO,"\r\n");

	return SUCCESS;
}

static RET_CODE on_iptv_des_pic_update(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
    control_t *p_des_pic = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESC_PICTURE);
    rect_t rect = {0};
    u8 *p_img_url = (u8 *)ui_iptv_dat->p_img_url;
    ui_pic_init(PIC_SOURCE_NET);
    ui_iptv_dat->playbar_is_back = TRUE;
    if(ui_iptv_dat->category == IPTV_VARIETY && p_img_url != NULL)
    {
        // TODO: identify SY
        ui_iptv_dat->identify = (0xFFFF << 16) | (u16)ui_iptv_dat->video_id.qpId[0];
        ctrl_get_frame(p_des_pic, &rect);
        ctrl_client2screen(p_des_pic, &rect);
        ui_pic_play_by_url(p_img_url, &rect,ui_iptv_dat->identify);
    }
    else if((ui_iptv_dat->category == IPTV_TV || ui_iptv_dat->category == IPTV_MOVIE) && p_img_url != NULL)
    {
        // TODO: identify SY
        ui_iptv_dat->identify = (0xFFFF << 16) | (u16)ui_iptv_dat->video_id.qpId[0];
        ctrl_get_frame(p_des_pic, &rect);
        ctrl_client2screen(p_des_pic, &rect);
        ui_pic_play_by_url(p_img_url, &rect,ui_iptv_dat->identify);
    }
    else if(p_img_url == NULL && ui_iptv_dat->category != IPTV_VARIETY)
    {
        DEBUG(UI_IPLAY_DESC,INFO,"img url is null\n");
        p_ctrl = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESC_RECOM_INFO_CONT);
        on_iptv_update_rec_pic(p_ctrl);  
    }
    return SUCCESS;
}

static RET_CODE on_iptv_des_on_exit(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
#if ENABLE_ROLL
    fw_tmr_destroy(ROOT_ID_IPTV_DESCRIPTION, MSG_HEART_BEAT);
#endif
    pic_stop();
    ui_pic_release();

    on_iptv_free_play_data();

    manage_close_menu(ROOT_ID_IPTV_DESCRIPTION, para1, para2);

    return SUCCESS;
}

static RET_CODE ui_iptv_des_on_destory(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
    control_t *p_root = NULL;

    mtos_printk("#### %s,%d,ui_iptv_dat->g_parent_root_id ==%d\n\n",__FUNCTION__,__LINE__,ui_iptv_dat->g_parent_root_id );
    if (ui_iptv_dat->g_parent_root_id != ROOT_ID_INVALID)
    {
        p_root = fw_find_root_by_id(ui_iptv_dat->g_parent_root_id);
        if (p_root != NULL)
        {
            mtos_printk("#### %s,%d,ui_iptv_dat->g_parent_root_id ==%d\n\n",__FUNCTION__,__LINE__,ui_iptv_dat->g_parent_root_id );

            fw_notify_root(p_root, NOTIFY_T_MSG, FALSE, MSG_UPDATE, 0, 0);
        }
    }
    ui_iptv_set_flag_to_plid(TRUE);
    on_ui_iptv_release_data();
	set_iptv_des_state(IPTV_DESC_DEINIT);
	

    return ERR_NOFEATURE;
}

static RET_CODE on_iptv_des_save_cant_dis_index(void)
{
    u8 *p_iptv_less = ui_iptv_dat->p_iptv_img_less;
    u8 i = 0;
    for(i = 0;i< IPTV_DESC_RECOMM_INFO_NUM;i++)
    {
        if(p_iptv_less[i] == ((ui_iptv_dat->identify + 1) & 0xFFFF))
        {
            break;
        }
        if(p_iptv_less[i] == 0)
        {
            p_iptv_less[i] = ((ui_iptv_dat->identify + 1) & 0xFFFF);
            break;
        }
    }
    return SUCCESS;
}
static RET_CODE on_iptv_des_delete_dis_index(u8 index)
{
    u8 *p_iptv_less = ui_iptv_dat->p_iptv_img_less;
    u8 i = 0;
    for(i = index;i < IPTV_DESC_RECOMM_INFO_NUM;i++)
    {
        if(p_iptv_less[i] != 0 && i < IPTV_DESC_RECOMM_INFO_NUM - 1)
        {
            p_iptv_less[i] = p_iptv_less[i + 1];
        }
    }
    p_iptv_less[i - 1] = 0;
    return SUCCESS;
}
 static RET_CODE on_iptv_des_dis_play_url_index(control_t *p_ctrl,u8 index)
 {
     u8 *p_iptv_less = ui_iptv_dat->p_iptv_img_less;
     iptv_recomand_info_t *p_rec_data = (iptv_recomand_info_t *)ui_iptv_dat->p_recomm_data;
     control_t* p_pic = NULL;
     control_t* p_item = NULL;
     rect_t rect;
     if(p_rec_data == NULL)
     return SUCCESS;

     if(p_iptv_less[index] != 0)
      {
          if(!strcmp(p_rec_data->recmmd[p_iptv_less[index] - 1].img,""))
          {
                return SUCCESS;
          }
          if((p_iptv_less[index]- 1)/8  == ui_iptv_dat->rec_pic_index/8)
          {
          ui_iptv_dat->identify = (p_iptv_less[index] - 1) | ((u8)(ROOT_ID_IPTV_DESCRIPTION) << 8);
          p_item = ctrl_get_child_by_id(p_ctrl, 
          IDC_IPTV_DESC_RECOM_INFO_START + (p_iptv_less[index] - 1)%IPTV_DESC_RECOMM_INFO_NUM);
          p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_DESC_RECOM_INFO_PIC);
          ctrl_get_frame(p_pic, &rect);
          ctrl_client2screen(p_pic, &rect);
          ui_pic_play_by_url(p_rec_data->recmmd[p_iptv_less[index] - 1].img, &rect, ui_iptv_dat->identify);
          }
      }
     else
     {
         return ERR_FAILURE;
      }
     return SUCCESS;
  }



static RET_CODE on_iptv_pic_draw_end(control_t *p_cont, u16 msg, u32 para1, u32 para2)
{
   u16 start = 0;
  u16 pic_index = 0;
  RET_CODE ret = SUCCESS;
  static u8 count = 0;
  u8 *p_iptv_less = ui_iptv_dat->p_iptv_img_less;
  static u8 draw_recomm_pic_num =0;
  static u8 temp_index = 0;
  control_t *p_ctrl = ctrl_get_child_by_id(p_cont, IDC_IPTV_DESC_RECOM_INFO_CONT);
  iptv_recomand_info_t *p_rec_data = (iptv_recomand_info_t *)ui_iptv_dat->p_recomm_data;

 #if 0
 if(request_iptv_play_url == TRUE)
 {
     return SUCCESS;
 }
 #endif
 if(ui_comm_get_focus_mainwin_id() != ROOT_ID_IPTV_DESCRIPTION)
 {
    return SUCCESS;
 }
 if(para1 == 0)
 {
    on_iptv_dis_pg_pic(p_cont,TRUE);
    pic_stop();
    if(ui_iptv_dat->playbar_is_back == TRUE && (ui_iptv_dat->category != IPTV_VARIETY))
    {
        on_iptv_update_rec_pic(p_ctrl);  
    }
     return ERR_FAILURE;
  }
  if(ui_iptv_dat->identify != para1)
  {
    return SUCCESS;
  }

   if(((para1 >> 16) & 0xFFFF) == 0xFFFF)
   {
    draw_recomm_pic_num = 0;

    if(MSG_PIC_EVT_DRAW_END == msg )
    {
      on_iptv_dis_pg_pic(p_cont,FALSE);
      pic_stop();
      if(p_rec_data && (g_draw_rec == FALSE))
      {
         if(p_ctrl == NULL)
         return SUCCESS;
         
         create_recommend_info(ctrl_get_parent(p_ctrl),p_rec_data->number);  
         on_iptv_update_recomm(p_ctrl, 0,IPTV_DESC_RECOMM_INFO_NUM,0);
         on_iptv_draw_recomm_icon(p_ctrl, 0);
     }
      g_draw_rec = TRUE;
      
      if(ui_iptv_dat->playbar_is_back == TRUE && (ui_iptv_dat->category != IPTV_VARIETY))
      {
        on_iptv_update_rec_pic(p_ctrl);  
       }
      return SUCCESS;
    }
    else
    {
        on_iptv_dis_pg_pic(p_cont,TRUE);
        pic_stop();
        if(p_rec_data && (g_draw_rec == FALSE))
        {
           if(p_ctrl == NULL)
           return SUCCESS;
           
           create_recommend_info(ctrl_get_parent(p_ctrl),p_rec_data->number);  
           on_iptv_update_recomm(p_ctrl, 0,IPTV_DESC_RECOMM_INFO_NUM,0);
           on_iptv_draw_recomm_icon(p_ctrl, 0);
        }
        g_draw_rec = TRUE;
        
        if(ui_iptv_dat->playbar_is_back == TRUE && (ui_iptv_dat->category != IPTV_VARIETY))
        {
            on_iptv_update_rec_pic(p_ctrl);  
        }
        return ERR_FAILURE;
    }
   }
   else if((para1 >> 16 & 0xFFFF) ==  (u16)(ROOT_ID_IPTV_DESCRIPTION))
   {
    if(p_rec_data == NULL)
    {
      return SUCCESS;
    }
    if(p_iptv_less == NULL)
    {
    return SUCCESS;
    }
    
    pic_index = (para1&0xFFFF);
    start = pic_index%IPTV_DESC_RECOMM_INFO_NUM;
    if(start == 0)
    {
       draw_recomm_pic_num = 0;
       memset(p_iptv_less,0,IPTV_DESC_RECOMM_INFO_NUM);
       temp_index = 0;
    }
    draw_recomm_pic_num++;
    if(MSG_PIC_EVT_DRAW_END == msg )
    {
       on_iptv_dis_pg_recmand_pic(p_ctrl, start);
    }
    else
    { 
      on_iptv_des_save_cant_dis_index();
    }
    pic_stop();
    if(draw_recomm_pic_num>=IPTV_DESC_RECOMM_INFO_NUM || ((pic_index + 1) == p_rec_data->number))
    {
      
      draw_recomm_pic_num = 0;
      count = 0;
 
      on_iptv_des_dis_play_url_index(p_ctrl,temp_index);
      return SUCCESS;
    }
   while(1)
    {
     if(pic_index<p_rec_data->number)
     {
      pic_index++;
      ret = on_iptv_draw_recomm_icon(p_ctrl,pic_index);
      if(ret == SUCCESS)
      break;
      else
       {
        if(draw_recomm_pic_num+1==IPTV_DESC_RECOMM_INFO_NUM)
         break;
        else
         continue;
      } 
     }
     break;
    }
   }
   else if((para1 >> 8 & 0xFF) == (u8)(ROOT_ID_IPTV_DESCRIPTION))
   {
        if(p_rec_data == NULL)
        {
            return SUCCESS;
        }
        DEBUG(UI_IPLAY_DESC,INFO,"begin display the pic \n");
        temp_index++;
        if(MSG_PIC_EVT_DRAW_END == msg )
        {
          DEBUG(UI_IPLAY_DESC,INFO,"wywywyww draw end %d\n",(p_iptv_less[temp_index - 1] - 1)%IPTV_DESC_RECOMM_INFO_NUM);
          on_iptv_dis_pg_recmand_pic(p_ctrl, (p_iptv_less[temp_index - 1] - 1)%IPTV_DESC_RECOMM_INFO_NUM);
          pic_stop();
          temp_index--;
          on_iptv_des_delete_dis_index(temp_index);
        }
        else
        {
              
              if(count++ > 3)
              {
        
                  return ERR_FAILURE;
              }
        }
        if(on_iptv_des_dis_play_url_index(p_ctrl,temp_index) != SUCCESS)
        {
            temp_index = 0;
            DEBUG(UI_IPLAY_DESC,INFO,"play img end not success\n");
            ret = on_iptv_des_dis_play_url_index(p_ctrl,temp_index);
            return ret;
    }
   }
  return SUCCESS;
}

/********************************************************************
************************iptv video resource functions***********************
*********************************************************************/
static RET_CODE on_iptv_variety_collect_focus_move(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_new_focus_ctrl = NULL;
    control_t *p_cont = NULL;
    u32 iptv_total_num = 0;
    u32 idc_id = 0;
    u16 uni_str[32] = {0};
    rect_t g_dlg_rc =
    {
        COMM_DLG_X,  COMM_DLG_Y ,
        COMM_DLG_X + COMM_DLG_W,
        COMM_DLG_Y + COMM_DLG_H - 20,
    };
    iptv_fav_info  iptv_info = {0};
    
    switch(msg)
    {
        case MSG_FOCUS_LEFT:
            p_cont = ui_comm_root_get_ctrl(ROOT_ID_IPTV_DESCRIPTION,IDC_IPTV_DESCNT);
            p_new_focus_ctrl = ctrl_get_child_by_id(p_cont,IDC_IPTV_DESCNT_VARIETY_LIST);
            ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
            ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
            ctrl_paint_ctrl(p_ctrl, TRUE);
            ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
            break;
        case MSG_FOCUS_RIGHT:
            p_cont = ui_comm_root_get_ctrl(ROOT_ID_IPTV_DESCRIPTION,IDC_IPTV_DES_VARIETY_TENT);
            p_new_focus_ctrl = ctrl_get_child_by_id(p_cont,IDC_IPTV_DESCNT_VARIETY_RSC_TXT_LIST);
            ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
            ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
            if(cbox_dync_get_count(p_new_focus_ctrl) > 1)
            {
                cbox_class_proc(p_new_focus_ctrl, MSG_SELECT, 0, 0);
                cbox_dync_set_focus(p_new_focus_ctrl, ui_iptv_dat->format_index);
            }
            ctrl_paint_ctrl(p_ctrl, TRUE);
            ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
            break;
        case MSG_SELECT:
            idc_id = IDC_IPTV_DESCNT_VARIETY_RSC_MBOX;
            if(ui_iptv_dat->iptv_is_fav)
            {
                on_ui_iptv_find_dis_str(idc_id,0,uni_str,sizeof(uni_str));
                text_set_content_by_unistr(p_ctrl, uni_str);
                sys_status_del_fav_iptv_info(ui_iptv_dat->iptv_fav_focus);   
                sys_status_save();
                ui_iptv_dat->iptv_is_fav = FALSE;
            }
            else
            {
                sys_status_get_fav_iptv_total_num(&iptv_total_num);
                if(iptv_total_num == FAV_IPTV_NUM)
                {
                    on_ui_iptv_find_dis_str(idc_id,3,uni_str,sizeof(uni_str));
                    ui_comm_cfmdlg_open_unistr(&g_dlg_rc,uni_str, NULL,3000);
                    return SUCCESS;
                }
                on_ui_iptv_find_dis_str(idc_id,1,uni_str,sizeof(uni_str));
                text_set_content_by_unistr(p_ctrl, uni_str);
                // TODO: fav video id SY
                //iptv_info.iptv_fav_id = ui_iptv_dat->video_id.qpId[0];
                memcpy(iptv_info.iptv_fav_vdo_id.qpId, ui_iptv_dat->video_id.qpId, strlen(ui_iptv_dat->video_id.qpId)+1);
                memcpy(iptv_info.iptv_fav_vdo_id.tvQid, ui_iptv_dat->video_id.tvQid, strlen(ui_iptv_dat->video_id.tvQid)+1);
                memcpy(iptv_info.iptv_fav_vdo_id.sourceCode, ui_iptv_dat->video_id.sourceCode, strlen(ui_iptv_dat->video_id.sourceCode)+1);
                iptv_info.iptv_fav_vdo_id.type = ui_iptv_dat->video_id.type;
                iptv_info.iptv_fav_category_id= ui_iptv_dat->category_id;
                memcpy((u8 *)iptv_info.iptv_fav_name,(u8 *)ui_iptv_dat->iptv_fav_name,sizeof(iptv_info.iptv_fav_name));

                sys_status_set_fav_iptv_info(iptv_total_num,&iptv_info);
                ui_iptv_dat->iptv_fav_focus = iptv_total_num;
                iptv_total_num++;
                sys_status_set_fav_iptv_total_num(iptv_total_num);
                sys_status_save();
                ui_iptv_dat->iptv_is_fav = TRUE;
            }
            ctrl_paint_ctrl(p_ctrl, TRUE);
            break;
    }
    return SUCCESS;
}
static RET_CODE on_iptv_des_video_rsc_change_focus(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
    control_t *p_new_focus_ctrl = NULL;
    control_t *p_pic = NULL;
    control_t *p_cont = NULL;
    control_t *p_name = NULL;
    s8 focus = 0;

    if(p_ctrl == NULL)
        return SUCCESS;
    
    switch(msg)
    {
        case MSG_FOCUS_DOWN:
            if(ui_iptv_dat->category == IPTV_TV)
            {
                p_new_focus_ctrl = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESC_SERIES_NUM_MBOX);
                if(p_new_focus_ctrl == NULL)
                    return SUCCESS;
                
                if(ctrl_get_sts(p_new_focus_ctrl) == OBJ_STS_HIDE)
                {
                    return SUCCESS;
                }
                ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
                ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
                ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
                ctrl_paint_ctrl(p_ctrl, TRUE);
            }
            else if(ui_iptv_dat->category == IPTV_MOVIE)
            {
                p_cont = ui_comm_root_get_ctrl(ROOT_ID_IPTV_DESCRIPTION, IDC_IPTV_DESC_RECOM_INFO_CONT);
                if(p_cont == NULL)
                    return SUCCESS;
                
                p_new_focus_ctrl = ctrl_get_child_by_id(p_cont, IDC_IPTV_DESC_RECOM_INFO_START + ui_iptv_dat->rec_pic_index%8);
                if(p_new_focus_ctrl == NULL)
                {
                return SUCCESS;
                }
                
                p_pic = ctrl_get_child_by_id(p_new_focus_ctrl, IDC_IPTV_DESC_RECOM_INFO_PIC);
                p_name = ctrl_get_child_by_id(p_new_focus_ctrl, IDC_IPTV_DESC_RECOM_INFO_NAME);
                if(ctrl_get_sts(p_pic) == OBJ_STS_HIDE)
                {
                    DEBUG(UI_IPLAY_DESC,INFO,"pic is hiden when change focus from rsc buttion\n");
                    return SUCCESS;
                }
                ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
                ctrl_process_msg(p_pic, MSG_GETFOCUS, 0, 0);
                ctrl_paint_ctrl(p_cont, TRUE);
                ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
                ctrl_paint_ctrl(p_ctrl, TRUE);
#if ENABLE_ROLL
                if(p_name)
                on_iptv_create_roll(p_name);
#endif
            }
            break;
        case MSG_FOCUS_LEFT:
            focus = mbox_get_focus(p_ctrl);
            if(focus == 1)
            {
                mbox_class_proc(p_ctrl, msg, para1, para2);
                ctrl_paint_ctrl(p_ctrl, TRUE);
            }
            else if(focus == 0)
            {
                p_new_focus_ctrl = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESC_VIDEO_RSC_TXT_LIST);
                if(p_new_focus_ctrl == NULL)
                return SUCCESS;
                ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
                ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
                if(cbox_dync_get_count(p_new_focus_ctrl) > 1)
                {
                    cbox_class_proc(p_new_focus_ctrl, MSG_SELECT, 0, 0);
                    cbox_dync_set_focus(p_new_focus_ctrl,  ui_iptv_dat->format_index);
                }
                ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
                ctrl_paint_ctrl(p_ctrl, TRUE);
            }
            break;
        case MSG_FOCUS_RIGHT:
            focus = mbox_get_focus(p_ctrl);
            if(focus == 0)
            {
                mbox_class_proc(p_ctrl, msg, para1, para2);
                ctrl_paint_ctrl(p_ctrl, TRUE);
            }
            else if(focus == 1)
            {
                p_new_focus_ctrl = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESC_VIDEO_RSC_TXT_LIST);
                if(p_new_focus_ctrl == NULL)
                    return SUCCESS;

                ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
                ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
                if(cbox_dync_get_count(p_new_focus_ctrl) > 1)
                {
                    cbox_class_proc(p_new_focus_ctrl, MSG_SELECT, 0, 0);
                    cbox_dync_set_focus(p_new_focus_ctrl,  ui_iptv_dat->format_index);
                }
                ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
                ctrl_paint_ctrl(p_ctrl, TRUE);
            }
            break;
    }
    return SUCCESS;
}

static void ui_iptvdes_continue_to_play(void)
{
    al_iptv_play_origin_info_t *p_data = (al_iptv_play_origin_info_t *)on_ui_iptv_get_play_data();

    if(p_data->play_origin_info == NULL)
    {
      return;
    }

     DEBUG(UI_IPLAY_DESC,INFO,"play tvQid[%s],vid[%s]\r\n",p_data->play_origin_info[ui_iptv_dat->episode_num].tvQid,
	 			p_data->play_origin_info[ui_iptv_dat->episode_num].vid);
   
    if(p_data->play_origin_info[ui_iptv_dat->episode_num].tvQid[0] == 0
	||p_data->play_origin_info[ui_iptv_dat->episode_num].vid[0] == 0)
    {
      return;
    }
    DEBUG(UI_IPLAY_DESC,INFO,"\r\n");
    ui_iptv_dat->tvQid =  p_data->play_origin_info[ui_iptv_dat->episode_num].tvQid;
    ui_iptv_dat->vid=  p_data->play_origin_info[ui_iptv_dat->episode_num].vid;
    on_iptv_des_series_play_url();
}

static void ui_iptvdes_replay(void)
{
    al_iptv_play_origin_info_t *p_data = (al_iptv_play_origin_info_t *)on_ui_iptv_get_play_data();

    ui_iptv_dat->episode_num = 0;
    ui_iptv_dat->play_time = 0;

    if(p_data->play_origin_info == NULL)
    {
      return;
    }

    DEBUG(UI_IPLAY_DESC,INFO,"play tvQid[%s],vid[%s]\r\n",p_data->play_origin_info[ui_iptv_dat->episode_num].tvQid,
	 			p_data->play_origin_info[ui_iptv_dat->episode_num].vid);
		 
    if(p_data->play_origin_info[ui_iptv_dat->episode_num].tvQid[0] == 0
	||p_data->play_origin_info[ui_iptv_dat->episode_num].vid[0] == 0)
    {
      return;
    }
    DEBUG(UI_IPLAY_DESC,INFO,"\r\n");
    ui_iptv_dat->tvQid =  p_data->play_origin_info[ui_iptv_dat->episode_num].tvQid;
    ui_iptv_dat->vid=  p_data->play_origin_info[ui_iptv_dat->episode_num].vid;
    on_iptv_des_series_play_url();
}

static RET_CODE on_iptv_des_video_rsc_begin_play(control_t *p_ctrl, u16 msg, u32 para1, u32 para2)
{
  al_iptv_play_origin_info_t *p_data = (al_iptv_play_origin_info_t *)on_ui_iptv_get_play_data();
  control_t *p_menu, *p_cont, *p_title;
  u16 *str_char;
  conn_play_info info;
   u16 uni_str[64] = {0};
   u32 iptv_total_num = 0;
   iptv_fav_info iptv_fav_info = {0};
   u32 idc_id = (u32)IDC_IPTV_DESC_VIDEO_RSC_MBOX;
  u16 *p_unistr;
  u16 uni_str1[30+1];
  u16 uni_str2[10+1];
  u8  asc_str[10+1];
  u16 key[MAX_KEY_LEN] = {0};
  
  rect_t g_dlg_rc =
  {
    COMM_DLG_X,  COMM_DLG_Y ,
    COMM_DLG_X + COMM_DLG_W,
    COMM_DLG_Y + COMM_DLG_H - 20,
  };

   comm_dlg_data_t dlg_data_info =
  {
      ROOT_ID_IPTV_DESCRIPTION,
      DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
      COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H - 20,
      IDS_HD_NETWORK_BUSY_PLS_WAIT,
      2000,
  };

  comm_dlg_data_t dlg_data = //popup dialog data
  {
    ROOT_ID_INVALID,
    DLG_FOR_CONFIRM | DLG_STR_MODE_STATIC,
    COMM_DLG_X, COMM_DLG_Y,
    COMM_DLG_W, COMM_DLG_H - 20,
    IDS_HD_LOADING_FAIL,
    0,
  };
  DEBUG(UI_IPLAY_DESC,INFO,"\r\n");
  
  if(mbox_get_focus(p_ctrl) == 0)
  { 
    if(p_data != NULL)
    {
        if(p_data->play_origin_info != NULL)
        {
          DEBUG(UI_IPLAY_DESC,INFO,"play tvqid[%s],vid[%s]\r\n",p_data->play_origin_info[0].tvQid,p_data->play_origin_info[0].vid);
          if(p_data->play_origin_info[0].tvQid[0] != 0
		  && p_data->play_origin_info[0].vid[0] != 0)
          {
            p_menu = ui_comm_root_get_root(ROOT_ID_IPTV_DESCRIPTION);
            p_cont = ctrl_get_child_by_id(p_menu,IDC_IPTV_DESC_CONT);
            p_title = ctrl_get_child_by_id(p_cont, IDC_IPTV_DESC_TITLE);
            MT_ASSERT(p_title != NULL);

            str_char = (u16 *)text_get_content(p_title);
            uni_strcpy(key, str_char);

            if (ui_iptv_dat->category == IPTV_MOVIE)
            {
                strcat((u8 *)key, ui_iptv_dat->video_id.qpId);
                strcat((u8 *)key, ui_iptv_dat->video_id.tvQid);
            }
            else
            {
                strcat((u8 *)key, p_data->play_origin_info[ui_iptv_dat->episode_num].vid);
                strcat((u8 *)key, p_data->play_origin_info[ui_iptv_dat->episode_num].tvQid);
            }

            if (str_char != NULL && db_cnpl_get_item_by_key(key, &info) >= 0)
            {//pop dlg
              if (info.episode_num < p_data->url_count)
              {
                ui_iptv_dat->episode_num = info.episode_num;
                ui_iptv_dat->play_time = info.play_time;
	         DEBUG(UI_IPLAY_DESC,INFO,"play episode_num[%d],start time[%d]\n",
			 	ui_iptv_dat->episode_num,ui_iptv_dat->play_time);
				
                if (ui_iptv_dat->category == IPTV_MOVIE)
                {
                    DEBUG(UI_IPLAY_DESC,INFO,"\r\n");
                    ui_comm_ask_for_dodlg_open_xxx(&g_dlg_rc, IDS_HD_PLAY_FROM_LAST_POS, ui_iptvdes_continue_to_play, ui_iptvdes_replay, 0);
                }
                else
                {
                    DEBUG(UI_IPLAY_DESC,INFO,"\r\n");
                    memset(uni_str1, 0, sizeof(uni_str1));
                    memset(uni_str2, 0, sizeof(uni_str2));
                    memset(asc_str, 0, sizeof(asc_str));

                    p_unistr = (u16 *)gui_get_string_addr(IDS_HD_LAST_PLAY);
                    uni_strcpy(uni_str1, p_unistr);

                    p_unistr = (u16 *)gui_get_string_addr(IDS_HD_TV_CHANNEL);
                    uni_strcat(uni_str1, p_unistr, 30);

                    sprintf(asc_str, " %d  ,", ui_iptv_dat->episode_num+1);
                    str_asc2uni(asc_str, uni_str2);
                    uni_strcat(uni_str1, uni_str2, 30);

                    p_unistr = (u16 *)gui_get_string_addr(IDS_HD_PLAY_FROM_LAST_POS);
                    uni_strcat(uni_str1, p_unistr, 30);

                    ui_comm_ask_for_dodlg_open_unistr(&g_dlg_rc, uni_str1, ui_iptvdes_continue_to_play, ui_iptvdes_replay, 0);
                }
              }
              else
              {
                DEBUG(UI_IPLAY_DESC,INFO,"\r\n");
                ui_iptvdes_replay();
              }
            }
            else
            {
                DEBUG(UI_IPLAY_DESC,INFO,"\r\n");
                ui_iptvdes_replay();
            }
          }
          else
          {
              DEBUG(UI_IPLAY_DESC,INFO,"\r\n");
              ui_comm_dlg_open(&dlg_data_info);
          }
        }
        else if(ui_iptv_dat->iptv_url_c == TRUE)
        {
             DEBUG(UI_IPLAY_DESC,INFO,"\r\n");
             ui_comm_dlg_open(&dlg_data);
        }
     }
     return SUCCESS;
  }
  else if(mbox_get_focus(p_ctrl) == 1)
  {
     if(ui_iptv_dat->iptv_is_fav)
     {
        on_ui_iptv_find_dis_str(idc_id,1,uni_str,sizeof(uni_str));
       mbox_set_content_by_unistr(p_ctrl, 1, uni_str);
       sys_status_del_fav_iptv_info(ui_iptv_dat->iptv_fav_focus);   
       sys_status_save();
       ui_iptv_dat->iptv_is_fav = FALSE;
     }
     else
     {
        sys_status_get_fav_iptv_total_num(&iptv_total_num);
        if(iptv_total_num == FAV_IPTV_NUM)
        {
             on_ui_iptv_find_dis_str(idc_id,3,uni_str,sizeof(uni_str));
             ui_comm_cfmdlg_open_unistr(&g_dlg_rc,uni_str , NULL,3000);
             return SUCCESS;
        }
       on_ui_iptv_find_dis_str(idc_id,2,uni_str,sizeof(uni_str));
       mbox_set_content_by_unistr(p_ctrl, 1, uni_str);
	// TODO: fav video id SY	   
       //iptv_fav_info.iptv_fav_id = ui_iptv_dat->video_id.qpId[0];
       memcpy(iptv_fav_info.iptv_fav_vdo_id.qpId, ui_iptv_dat->video_id.qpId, strlen(ui_iptv_dat->video_id.qpId)+1);
       memcpy(iptv_fav_info.iptv_fav_vdo_id.tvQid, ui_iptv_dat->video_id.tvQid, strlen(ui_iptv_dat->video_id.tvQid)+1);
       memcpy(iptv_fav_info.iptv_fav_vdo_id.sourceCode, ui_iptv_dat->video_id.sourceCode, strlen(ui_iptv_dat->video_id.sourceCode)+1);
       iptv_fav_info.iptv_fav_vdo_id.type = ui_iptv_dat->video_id.type;
       iptv_fav_info.iptv_fav_category_id= ui_iptv_dat->category_id;
       memcpy(iptv_fav_info.iptv_fav_name,ui_iptv_dat->iptv_fav_name,sizeof(iptv_fav_info.iptv_fav_name));
       sys_status_set_fav_iptv_info(iptv_total_num, &iptv_fav_info);
       ui_iptv_dat->iptv_fav_focus = iptv_total_num;
       iptv_total_num++;
       sys_status_set_fav_iptv_total_num(iptv_total_num);
       sys_status_save();
       ui_iptv_dat->iptv_is_fav = TRUE;
     }
     ctrl_paint_ctrl(p_ctrl, TRUE);
     return SUCCESS;
  }
  else
  {
    return SUCCESS;
  }
}

/********************************************************************
************************iptv series number functions************************
*********************************************************************/
static RET_CODE on_iptv_desc_series_num_change_focus(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_new_focus_ctrl = NULL;
  s32 focus;
  u32 temp = 0;
  control_t *root = fw_find_root_by_id(ROOT_ID_IPTV_DESCRIPTION);
  switch(msg)
  {
    case MSG_FOCUS_UP:
      p_new_focus_ctrl = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESC_VIDEO_RSC_MBOX);
      ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
      ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
      ctrl_paint_ctrl(p_ctrl, TRUE);
      ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
      break;
    case MSG_FOCUS_DOWN:
      p_new_focus_ctrl = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESC_SERIES_MBOX);
      focus = mbox_get_focus(p_new_focus_ctrl);
      ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
      ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
	  temp = ((focus % IPTV_SERIES_NUM_PER_PAGE_ITEMS == 0)? \
             0 : (focus%IPTV_SERIES_NUM_PER_PAGE_ITEMS));
      mbox_set_focus(p_new_focus_ctrl, focus - temp >= 0 ? focus - temp: focus);
      ctrl_paint_ctrl(p_ctrl, TRUE);
      ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
      break;  
    case MSG_FOCUS_LEFT:
    case MSG_FOCUS_RIGHT:
      mbox_class_proc(p_ctrl, msg, para1, para2);
  
      fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_SELECT, 0, 0);
      break;
  }
  
  return SUCCESS;
}


static RET_CODE on_iptv_des_series_num_select(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_new_focus_ctrl = NULL;
  u16 focus = mbox_get_focus(p_ctrl);
  u16 mbox_count = ui_iptv_dat->g_series_cnt;
  u16 max_focus = mbox_count/IPTV_SERIES_NUM_PER_PAGE_ITEMS;
  p_new_focus_ctrl = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESC_SERIES_MBOX);
  if(focus != max_focus)
  {
    mbox_set_win(p_new_focus_ctrl,  
      (mbox_count < IPTV_DESC_WINCOL)?mbox_count : IPTV_DESC_WINCOL,
           IPTV_DESC_WINROW);
    mbox_set_focus(p_new_focus_ctrl,0);
    mbox_set_focus(p_new_focus_ctrl, IPTV_SERIES_NUM_PER_PAGE_ITEMS*(focus+1) - 1);
  }
  else
  {
    if(IPTV_SERIES_NUM_PER_PAGE_ITEMS*(focus+1) - 1 > mbox_count)
    {
      mbox_set_win(p_new_focus_ctrl, mbox_count - IPTV_SERIES_NUM_PER_PAGE_ITEMS * focus, 
           IPTV_DESC_WINROW);
       mbox_set_focus(p_new_focus_ctrl, mbox_count - 1);
    }
    else
    {
      mbox_set_win(p_new_focus_ctrl, (mbox_count < IPTV_DESC_WINCOL)?mbox_count : mbox_count%IPTV_DESC_WINCOL,
           IPTV_DESC_WINROW);
      mbox_set_focus(p_new_focus_ctrl, mbox_count - 1);
    }
  }
  ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
  
  return SUCCESS;
}

/********************************************************************
************************iptv series mbox functions**************************
*********************************************************************/
static RET_CODE on_iptv_desc_series_change_focus(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_new_focus_ctrl = NULL;
  control_t *p_cont = NULL;
  control_t *p_pic = NULL;
  control_t *p_name = NULL;


  control_t *root = fw_find_root_by_id(ROOT_ID_IPTV_DESCRIPTION);
  
  if(p_ctrl == NULL)
  return SUCCESS;
  switch(msg)
  {
    case MSG_FOCUS_UP:
      p_new_focus_ctrl = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESC_SERIES_NUM_MBOX);
      if(p_new_focus_ctrl == NULL)
       return SUCCESS;
      ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
      ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
      ctrl_paint_ctrl(p_ctrl, TRUE);
      ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
      fw_notify_root(root, NOTIFY_T_MSG, FALSE, MSG_SELECT, 0, 0);
      break;
   case MSG_FOCUS_DOWN:
      p_cont = ui_comm_root_get_ctrl(ROOT_ID_IPTV_DESCRIPTION, IDC_IPTV_DESC_RECOM_INFO_CONT);
      if(p_cont == NULL)
      return SUCCESS;
      p_new_focus_ctrl = ctrl_get_child_by_id(p_cont, 
         IDC_IPTV_DESC_RECOM_INFO_START + ui_iptv_dat->rec_pic_index%8);
      if(p_new_focus_ctrl == NULL)
      return SUCCESS;
      p_pic = ctrl_get_child_by_id(p_new_focus_ctrl, IDC_IPTV_DESC_RECOM_INFO_PIC);
      p_name = ctrl_get_child_by_id(p_new_focus_ctrl, IDC_IPTV_DESC_RECOM_INFO_NAME);
      #if ENABLE_ROLL
      if(p_name)
      {
         on_iptv_create_roll(p_name);
       }
      #endif
      if(ctrl_get_sts(p_pic) == OBJ_STS_HIDE)
      {
          return SUCCESS;
      }
      ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
      ctrl_paint_ctrl(p_ctrl, TRUE);
      ctrl_process_msg(p_pic, MSG_GETFOCUS, 0, 0);
      ctrl_paint_ctrl(p_pic, TRUE);
      break;  
  }

  
  return SUCCESS;
}

#if 0
static RET_CODE on_iptv_desc_hide_series_num(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_ctrl_temp = NULL;
  if((p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESC_SERIES_NUM_MBOX)) 
       != NULL)
  {
    ctrl_set_sts(p_ctrl_temp,OBJ_STS_HIDE);
  }
  if((p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESC_SERIES_MBOX)) 
   != NULL)
  {
    ctrl_set_sts(p_ctrl_temp,OBJ_STS_HIDE);
  }
  ctrl_paint_ctrl(p_ctrl,TRUE);
  return SUCCESS;
}

static RET_CODE on_iptv_desc_hide_variety_info(control_t *p_ctrl, u16 msg,
    u32 para1, u32 para2)
{
  control_t *p_ctrl_temp = NULL;
  if((p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESCNT_VARIETY_RSC_NUM_TEXT)) 
       != NULL)
  {
    ctrl_set_sts(p_ctrl_temp,OBJ_STS_HIDE);
  }
  if((p_ctrl_temp = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESCNT_VARIETY_RSC_NUM_DYN)) 
   != NULL)
  {
    ctrl_set_sts(p_ctrl_temp,OBJ_STS_HIDE);
  }
  ctrl_paint_ctrl(p_ctrl,TRUE);
  return SUCCESS;
}
#endif

static RET_CODE on_iptv_desc_rsc_change_focus(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
    control_t *p_new_focus_ctrl = NULL;
    control_t *p_cont = NULL;
    control_t *p_pic= NULL;
    control_t *p_name = NULL;
  
    if(p_ctrl == NULL)
    {
        return SUCCESS;
    }
  
    if(ui_iptv_dat->category == IPTV_TV)
    {
        switch(msg)
        {
            case MSG_FOCUS_LEFT:
            case MSG_FOCUS_RIGHT:
            {
                p_new_focus_ctrl = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESC_VIDEO_RSC_MBOX);
                if(p_new_focus_ctrl == NULL)
                    return SUCCESS;
                
                if(p_ctrl->priv_attr & CBOX_STATUS_LIST)      
                {    
                    if(ui_iptv_dat->g_series_cnt > 1)
                    {
                        if(ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESC_SERIES_NUM_MBOX) == NULL)
                            return SUCCESS;
                        if(ctrl_get_sts(ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESC_SERIES_NUM_MBOX)) == OBJ_STS_HIDE)
                            return SUCCESS;
                    }
                    cbox_class_proc(p_ctrl, MSG_SELECT, 0, 0);

                    ui_iptv_dat->format_index = cbox_dync_get_focus(p_ctrl);
                }    
                ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
                mbox_set_focus(p_new_focus_ctrl,(msg == MSG_FOCUS_LEFT )? 1 : 0);
                ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
                ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
            }
                break;  
            case MSG_FOCUS_DOWN:
                if(!(p_ctrl->priv_attr & CBOX_STATUS_LIST))      
                {
                    p_new_focus_ctrl = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESC_SERIES_NUM_MBOX);
                    if(p_new_focus_ctrl == NULL)
                        return SUCCESS;
                    
                    if(ctrl_get_sts(ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESC_SERIES_NUM_MBOX)) == OBJ_STS_HIDE)
                        return SUCCESS;
                    
                    ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
                    ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);  
                    ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
                }
                else
                {
                    cbox_class_proc(p_ctrl, msg, 0, 0);
                }
                break;
            case MSG_FOCUS_UP:
                if(p_ctrl->priv_attr & CBOX_STATUS_LIST)      
                    cbox_class_proc(p_ctrl, msg, 0, 0);
                break;
            case MSG_SELECT:
                if(ui_iptv_dat->g_series_cnt > 1)
                {
                    if(ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESC_SERIES_NUM_MBOX) == NULL)
                        return SUCCESS;
                    
                    if(ctrl_get_sts(ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESC_SERIES_NUM_MBOX)) == OBJ_STS_HIDE)
                        return SUCCESS;
                }
                if(cbox_dync_get_count(p_ctrl) > 1)
                {
                    cbox_class_proc(p_ctrl, msg, 0, 0);
                }
                if(ui_iptv_dat->iptv_url_c == FALSE)
                {
                    return SUCCESS;
                }
                ui_iptv_dat->format_index = cbox_dync_get_focus(p_ctrl);
                break;
        }
    }
    else if(ui_iptv_dat->category == IPTV_MOVIE)
    {
        switch(msg)
        {
            case MSG_FOCUS_LEFT:
            case MSG_FOCUS_RIGHT:
                {
                    p_new_focus_ctrl = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESCNT_MOVIE_RSC_MBOX);
                    if(p_new_focus_ctrl == NULL)
                        return SUCCESS;
                    
                    if(p_ctrl->priv_attr & CBOX_STATUS_LIST)      
                    {                 
                        cbox_class_proc(p_ctrl, MSG_SELECT, 0, 0);
                        ui_iptv_dat->format_index = cbox_dync_get_focus(p_ctrl);
                    } 

                    ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
                    mbox_set_focus(p_new_focus_ctrl,(msg == MSG_FOCUS_LEFT )? 1 : 0);
                    ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
                    ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
                }
                break;  
            case MSG_FOCUS_DOWN:
                if(!(p_ctrl->priv_attr & CBOX_STATUS_LIST))      
                {
                    p_cont = ui_comm_root_get_ctrl(ROOT_ID_IPTV_DESCRIPTION, IDC_IPTV_DESC_RECOM_INFO_CONT);
                    if(p_cont == NULL)
                        return SUCCESS;
                    
                    p_new_focus_ctrl = ctrl_get_child_by_id(p_cont, IDC_IPTV_DESC_RECOM_INFO_START + ui_iptv_dat->rec_pic_index%8);
                    if(p_new_focus_ctrl == NULL)
                        return SUCCESS;
                    
                    p_pic = ctrl_get_child_by_id(p_new_focus_ctrl, IDC_IPTV_DESC_RECOM_INFO_PIC);
                    p_name = ctrl_get_child_by_id(p_new_focus_ctrl, IDC_IPTV_DESC_RECOM_INFO_NAME);
                    if(ctrl_get_sts(p_pic) == OBJ_STS_HIDE)
                        return SUCCESS;
                    
                    ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
                    ctrl_paint_ctrl(p_ctrl, TRUE);
                    ctrl_process_msg(p_pic, MSG_GETFOCUS, 0, 0);
                    ctrl_paint_ctrl(p_cont, TRUE);
#if ENABLE_ROLL
                    if(p_name)
                    {
                        on_iptv_create_roll(p_name);
                    }
#endif
                }
                else
                {
                    cbox_class_proc(p_ctrl, msg, 0, 0);
                }
                break;
            case MSG_FOCUS_UP:
                if(p_ctrl->priv_attr & CBOX_STATUS_LIST)      
                    cbox_class_proc(p_ctrl, msg, 0, 0);
                break;
            case MSG_SELECT:
                if(cbox_dync_get_count(p_ctrl) > 1)
                {
                    cbox_class_proc(p_ctrl, msg, 0, 0);
                }
                ui_iptv_dat->format_index = cbox_dync_get_focus(p_ctrl);
                break;
        }
    }
    else if(ui_iptv_dat->category == IPTV_VARIETY)
    {
        switch(msg)
        {
            case MSG_FOCUS_LEFT:
                p_new_focus_ctrl = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), IDC_IPTV_DESCNT_VARIETY_RSC_MBOX);
                if(p_new_focus_ctrl == NULL)
                    return SUCCESS;
                
                //   p_data = (ctrl_cbox_data_t *)p_new_focus_ctrl->priv_data;
                if(p_ctrl->priv_attr & CBOX_STATUS_LIST)      
                {                 
                    cbox_class_proc(p_ctrl, MSG_SELECT, 0, 0);
                } 
                ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
                ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
                ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);

                ui_iptv_dat->format_index = cbox_dync_get_focus(p_ctrl);
                break;
            case MSG_FOCUS_RIGHT:
                {
                    control_t *p_cont = ui_comm_root_get_ctrl(ROOT_ID_IPTV_DESCRIPTION,IDC_IPTV_DESCNT);
                    control_t *p_des_variety_cont_list = ctrl_get_child_by_id(p_cont,IDC_IPTV_DESCNT_VARIETY_LIST);
                    if(p_cont == NULL)
                        return SUCCESS;
                    
                    if(p_des_variety_cont_list == NULL)
                        return SUCCESS;
                    if(p_ctrl->priv_attr & CBOX_STATUS_LIST)      
                    {                 
                        cbox_class_proc(p_ctrl, MSG_SELECT, 0, 0);
                        ui_iptv_dat->format_index = cbox_dync_get_focus(p_ctrl);
                    }
                    
                    //list_set_focus_pos(p_des_variety_cont_list, 0);
                    ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
                    ctrl_process_msg(p_des_variety_cont_list, MSG_GETFOCUS, 0, 0);
                    ctrl_paint_ctrl(p_des_variety_cont_list, TRUE);
                }
                break;  
            case MSG_FOCUS_UP:
            case MSG_FOCUS_DOWN:
                if(p_ctrl->priv_attr & CBOX_STATUS_LIST)      
                    cbox_class_proc(p_ctrl, msg, 0, 0);
                break;
                
            case MSG_SELECT:
                if(cbox_dync_get_count(p_ctrl) > 1)
                {
                    cbox_class_proc(p_ctrl, msg, 0, 0);
                }
                ui_iptv_dat->format_index = cbox_dync_get_focus(p_ctrl);
                break;
        }
    }

    ctrl_paint_ctrl(p_ctrl, TRUE);
    return SUCCESS;
}
static RET_CODE on_iptv_desc_series_select(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
    u32 focus = 0;
    al_iptv_play_origin_info_t *p_data = (al_iptv_play_origin_info_t *)on_ui_iptv_get_play_data();

    comm_dlg_data_t dlg_data =
    {
        ROOT_ID_IPTV_DESCRIPTION,
        DLG_FOR_SHOW | DLG_STR_MODE_STATIC,
        COMM_DLG_X, COMM_DLG_Y, COMM_DLG_W, COMM_DLG_H - 20,
        IDS_HD_NETWORK_BUSY_PLS_WAIT,
        2000,
    };

    focus = mbox_get_focus(p_ctrl);

    ui_iptv_dat->episode_num = focus;
    ui_iptv_dat->play_time = 0;

    if(p_data->play_origin_info == NULL)
    {
        return SUCCESS;
    }

    if(p_data->play_origin_info[ui_iptv_dat->episode_num].tvQid[0] == 0
        ||p_data->play_origin_info[ui_iptv_dat->episode_num].vid[0] == 0)
    {
        ui_comm_dlg_open(&dlg_data);
        return SUCCESS;
    }

    ui_iptv_dat->tvQid= p_data->play_origin_info[ui_iptv_dat->episode_num].tvQid;  
    ui_iptv_dat->vid = p_data->play_origin_info[ui_iptv_dat->episode_num].vid;
    on_iptv_des_series_play_url();
    return SUCCESS;
}

/********************************************************************
************************iptv series mbox functions**************************
*********************************************************************/
static RET_CODE on_iptv_desc_recomm_info_change_focus(control_t *p_ctrl, u16 msg,
  u32 para1, u32 para2)
{
  control_t *p_new_focus_ctrl = NULL;
   p_new_focus_ctrl = ctrl_get_child_by_id(p_ctrl, 
   IDC_IPTV_DESC_RECOM_INFO_START + ui_iptv_dat->rec_pic_index%8);
   if(p_new_focus_ctrl == NULL)
   return SUCCESS;
   #if ENABLE_ROLL
   on_iptv_destroy_roll(ctrl_get_child_by_id(p_new_focus_ctrl, IDC_IPTV_DESC_RECOM_INFO_NAME));
   #endif
  if(ui_iptv_dat->category == IPTV_TV)
  {
    {
    p_new_focus_ctrl = ctrl_get_child_by_id(ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), \
                              IDC_IPTV_DESCNT),IDC_IPTV_DESC_SERIES_MBOX);
    ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
    ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
    }
  }
  else if(ui_iptv_dat->category == IPTV_MOVIE)
  {
    p_new_focus_ctrl = ctrl_get_child_by_id(ctrl_get_child_by_id(ctrl_get_parent(p_ctrl), \
                              IDC_IPTV_DESCNT),
                              IDC_IPTV_DESCNT_MOVIE_RSC_MBOX);
    ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
    ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
  }
  ctrl_paint_ctrl(p_ctrl, TRUE);
  ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);

  return SUCCESS;
}

static void on_iptv_recomm_set_focus(control_t *p_cont, s16 last_focus, s16 focus)
{
  control_t* p_item = NULL;
  control_t* p_text = NULL;
  control_t *p_pic = NULL;
  if( last_focus < 0 )
    ;
  else
  {
    p_item = ctrl_get_child_by_id(p_cont, IDC_IPTV_DESC_RECOM_INFO_START + last_focus);
    p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_DESC_RECOM_INFO_PIC);
    p_text = ctrl_get_child_by_id(p_item, IDC_IPTV_DESC_RECOM_INFO_NAME);
    ctrl_process_msg(p_pic, MSG_LOSTFOCUS, 0, 0);
    #if ENABLE_ROLL
    on_iptv_destroy_roll(p_text);
    #endif
    ctrl_paint_ctrl(p_item, TRUE);
  }

  p_item = ctrl_get_child_by_id(p_cont, IDC_IPTV_DESC_RECOM_INFO_START + focus);
  p_text = ctrl_get_child_by_id(p_item, IDC_IPTV_DESC_RECOM_INFO_NAME);
  p_pic = ctrl_get_child_by_id(p_item, IDC_IPTV_DESC_RECOM_INFO_PIC);
  
  ctrl_process_msg(p_pic, MSG_GETFOCUS, 0, 0);

  ctrl_paint_ctrl(p_pic, TRUE);
  #if ENABLE_ROLL
  on_iptv_create_roll(p_text);
  #endif
}

static RET_CODE on_iptv_recomm_focus_move(control_t* p_cont, u16 msg, u32 para1, u32 para2)
{  
  u16 pic_index = ui_iptv_dat->rec_pic_index;
  u32 old_page =pic_index/8;
  u32 new_page = 0;
  VDO_ID_t *video_id_temp ;
  iptv_recomand_info_t *p_rec_data = (iptv_recomand_info_t *)ui_iptv_dat->p_recomm_data;
  control_t *p_des_pic = NULL;
  control_t *p_des_cont = NULL;
  rect_t rect;
  u8 video_iptv_cat_id = ui_iptv_dat->category_id;
  if(p_cont  == NULL)
  {
      return SUCCESS;
  }
  if(p_rec_data == NULL)
  {
     return SUCCESS;
  }
  switch(msg)
  {
    case MSG_FOCUS_LEFT:
            if(pic_index==0)
    {
     return SUCCESS;
    }
            pic_index--;
            ui_iptv_dat->rec_pic_index = pic_index;
            new_page = pic_index/8;
   if(old_page!=new_page)
   {
                  pic_stop();
                  on_iptv_draw_recomm_icon(p_cont, pic_index-7);
                  on_iptv_update_recomm(p_cont, pic_index-7,IPTV_DESC_RECOMM_INFO_NUM,0);
   }
            on_iptv_recomm_set_focus(p_cont, (pic_index+1)%8,pic_index%8);
    break;
    case MSG_FOCUS_RIGHT:
            if(pic_index + 1 == p_rec_data->number)
    {
     return SUCCESS;
    }
            pic_index++;
            ui_iptv_dat->rec_pic_index = pic_index;
            new_page = pic_index/8;
    if(old_page!=new_page)
    {
              pic_stop();
              on_iptv_draw_recomm_icon(p_cont, pic_index);
              on_iptv_update_recomm(p_cont, pic_index,IPTV_DESC_RECOMM_INFO_NUM,0);
    }
            on_iptv_recomm_set_focus(p_cont, (pic_index-1)%8,pic_index%8);
    break;
    case MSG_SELECT:
          pic_stop();
          video_id_temp = &p_rec_data->recmmd[pic_index].id;
          p_des_pic = ctrl_get_child_by_id(ctrl_get_parent(p_cont), IDC_IPTV_DESC_PICTURE);
          bmap_set_content_by_id(p_des_pic, 0);
          ctrl_set_rstyle(p_des_pic, RSI_BOX3, RSI_BOX3, RSI_BOX3);
          
          ctrl_get_frame(p_des_pic, &rect);
          ctrl_client2screen(p_des_pic, &rect);
          ui_pic_clear_rect(&rect, 0);
          ctrl_paint_ctrl(p_des_pic, TRUE);          
          ui_iptv_dat->playbar_is_back = FALSE;
	   // TODO: fav video id SY
          ui_iptv_dat->iptv_is_fav = get_iptv_is_fav(video_id_temp,&(ui_iptv_dat->iptv_fav_focus));
          DEBUG(UI_IPLAY_DESC,INFO,"iptv  recmand is fav %d\n",ui_iptv_dat->iptv_is_fav);
          p_des_pic = ctrl_get_child_by_id(p_cont, IDC_IPTV_DESC_RECOM_INFO_START + 
              ui_iptv_dat->rec_pic_index%8);
          p_des_pic = ctrl_get_child_by_id(p_des_pic, IDC_IPTV_DESC_RECOM_INFO_NAME);
          #if ENABLE_ROLL
          on_iptv_destroy_roll(p_des_pic);
          #endif
          on_iptv_hide_des_info(ctrl_get_parent(p_cont));
          on_iptv_update_recomm(p_cont, p_rec_data->number,
              IPTV_DESC_RECOMM_INFO_NUM, 0);
          on_iptv_free_play_data();
          
          ui_iptv_on_open_dlg();
          ui_iptv_get_video_info(video_id_temp, video_iptv_cat_id); // TODO qpid  get album
          ui_iptv_dat->iptv_entf = FALSE;
          ui_iptv_dat->video_id = *video_id_temp;
		  p_des_cont = ctrl_get_child_by_id(ctrl_get_child_by_id(ctrl_get_parent(p_cont), IDC_IPTV_DESCNT),IDC_IPTV_DESCNT_MOVIE_RSC_MBOX);
	   	  ui_iptv_set_flag_to_plid(FALSE);
          ctrl_process_msg(p_cont, MSG_LOSTFOCUS, 0, 0);
		  ctrl_process_msg(p_des_cont, MSG_GETFOCUS, 0, 0);
    break;
    
  }

  return SUCCESS;
}

static void ui_iptvdes_variety_continue_to_play(void)
{
    al_iptv_play_origin_info_t *p_data = (al_iptv_play_origin_info_t *)on_ui_iptv_get_play_data();

    if(p_data->play_origin_info == NULL)
        return;

    if(p_data->play_origin_info[ui_iptv_dat->episode_num].tvQid[0] == 0
        || p_data->play_origin_info[ui_iptv_dat->episode_num].vid[0] == 0)
        return;

    ui_iptv_dat->tvQid 	= p_data->play_origin_info[ui_iptv_dat->episode_num].tvQid;
    ui_iptv_dat->vid	= p_data->play_origin_info[ui_iptv_dat->episode_num].vid;
    on_iptv_des_series_play_url();
}
static void ui_iptvdes_variety_replay(void)
{
    al_iptv_play_origin_info_t *p_data = (al_iptv_play_origin_info_t *)on_ui_iptv_get_play_data();

    ui_iptv_dat->play_time = 0;
    if(p_data->play_origin_info == NULL)
    	return;

    if(p_data->play_origin_info[ui_iptv_dat->episode_num].tvQid[0]== 0
		|| p_data->play_origin_info[ui_iptv_dat->episode_num].vid[0]== 0)
   	return;
    
    ui_iptv_dat->tvQid	= p_data->play_origin_info[ui_iptv_dat->episode_num].tvQid;
    ui_iptv_dat->vid	= p_data->play_origin_info[ui_iptv_dat->episode_num].vid;
    on_iptv_des_series_play_url();
}

static RET_CODE on_iptv_variety_list_focus_move(control_t *p_ctrl, u16 msg,u32 para1, u32 para2)
{
    u8 utf_str[8] = {0};
    u16 uni_str[32] = {0};
    u16 key[MAX_KEY_LEN] = {0};
    u16 list_focus = 0;
    control_t *p_cont, *p_new_focus_ctrl;
    al_iptv_play_origin_info_t *p_data = (al_iptv_play_origin_info_t *)on_ui_iptv_get_play_data();
    //u8 video_iptv_cat_id = ui_iptv_dat->category_id;
    conn_play_info info;
    u16 *str_char = NULL;
    rect_t g_dlg_rc =
    {
        COMM_DLG_X,  COMM_DLG_Y ,
        COMM_DLG_X + COMM_DLG_W,
        COMM_DLG_Y + COMM_DLG_H - 20,
    };
  
    if(p_data == NULL)
    {
        return SUCCESS;
    }

	
	if(get_iptv_des_state() < IPTV_DESC_URL_ARRIVE)
		return SUCCESS;
	
    switch(msg)
    {
        case MSG_FOCUS_LEFT:
            {
                p_cont = ui_comm_root_get_ctrl(ROOT_ID_IPTV_DESCRIPTION,IDC_IPTV_DES_VARIETY_TENT);
                if(p_cont == NULL)
                    return SUCCESS;
                
                p_new_focus_ctrl = ctrl_get_child_by_id(p_cont,IDC_IPTV_DESCNT_VARIETY_RSC_TXT_LIST);
                if(p_new_focus_ctrl == NULL)
                    return SUCCESS;

                cbox_class_proc(p_new_focus_ctrl, MSG_SELECT, 0, 0);

                ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
                ctrl_paint_ctrl(p_ctrl, TRUE);
                ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
                ctrl_paint_ctrl(p_cont, TRUE);
                ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
            }
            break;
        case MSG_FOCUS_RIGHT:
            {
                p_cont = ui_comm_root_get_ctrl(ROOT_ID_IPTV_DESCRIPTION,IDC_IPTV_DES_VARIETY_TENT);
                if(p_cont == NULL)
                    return SUCCESS;
                
                p_new_focus_ctrl = ctrl_get_child_by_id(p_cont,IDC_IPTV_DESCNT_VARIETY_RSC_MBOX);
                if(p_new_focus_ctrl == NULL)
                    return SUCCESS;

                ctrl_process_msg(p_ctrl, MSG_LOSTFOCUS, 0, 0);
                ctrl_paint_ctrl(p_ctrl, TRUE);
                ctrl_process_msg(p_new_focus_ctrl, MSG_GETFOCUS, 0, 0);
                ctrl_paint_ctrl(p_cont, TRUE);
                ctrl_paint_ctrl(p_new_focus_ctrl, TRUE);
            }
            break;  
        case MSG_FOCUS_DOWN:
        case MSG_FOCUS_UP:
        case MSG_PAGE_DOWN:
        case MSG_PAGE_UP:
            if(p_ctrl == NULL)
                return SUCCESS;

            if(list_get_focus_pos(p_ctrl) == 0 && 
                (MSG_FOCUS_UP == msg || MSG_PAGE_UP == msg) &&
                p_data->play_origin_info[p_data->url_count - 1].urltitle == NULL)
            {
                return SUCCESS;
            }

            list_class_proc(p_ctrl, msg, 0, 0);

            list_focus = list_get_focus_pos(p_ctrl);

            if(list_focus == 0xFFFF)
            {
                return SUCCESS;
            }
            if(p_data->play_origin_info[list_focus].urltitle == NULL)
            {
                if(msg == MSG_PAGE_UP)
                list_class_proc(p_ctrl, MSG_PAGE_DOWN, 0, 0);
                else if(msg == MSG_PAGE_DOWN)
                list_class_proc(p_ctrl, MSG_PAGE_UP, 0, 0);

                return SUCCESS;
            }

            if(p_data != NULL)
            {
                if(p_data->origin != NULL)
                {
                    if(p_data->url_count != 0)  
                    {
                        control_t *p_des_variety_res = ctrl_get_child_by_id(ctrl_get_parent(p_ctrl),IDC_IPTV_DESCNT_VARIETY_RSC_NUM_DYN);
                        sprintf(utf_str,"%d/%d",(int)list_focus + 1,(int)p_data->url_count);
                        convert_gb2312_chinese_asc2unistr(utf_str, uni_str, sizeof(uni_str));
                        text_set_content_by_unistr(p_des_variety_res, uni_str);
                        ctrl_paint_ctrl(p_des_variety_res, TRUE);
                    }
                }
            }
            break;
        case MSG_SELECT:
            list_focus = list_get_focus_pos(p_ctrl);

            str_char = (u16 *)list_get_field_content(p_ctrl, list_get_focus_pos(p_ctrl),0);
            uni_strcpy(key, str_char);

            ui_iptv_dat->episode_num = list_focus;
            strcat((u8 *)key, p_data->play_origin_info[ui_iptv_dat->episode_num].vid);
            strcat((u8 *)key, p_data->play_origin_info[ui_iptv_dat->episode_num].tvQid);
            if (str_char != NULL && db_cnpl_get_item_by_key(key, &info) >= 0)
            {
                if (info.episode_num < p_data->url_count)
                {
                    ui_iptv_dat->play_time = info.play_time;
                    ui_comm_ask_for_dodlg_open_xxx(&g_dlg_rc, IDS_HD_PLAY_FROM_LAST_POS, ui_iptvdes_variety_continue_to_play, ui_iptvdes_variety_replay, 0);
                }
                else
                {
                    ui_iptvdes_variety_replay();
                }
            }
            else
            {
                ui_iptvdes_variety_replay();
            }

            break;
    }
  return SUCCESS;
}

 u16 *ui_iptvdesc_get_episode_name(u8 category, u16 episode_num)
{
    control_t *p_ctrl = NULL;
    control_t *p_des_tent = NULL;
    if(ui_iptv_dat->category != category)
    {
       return NULL;
    }
    if(category != IPTV_VARIETY)
    {
        p_ctrl = ui_comm_root_get_ctrl(ROOT_ID_IPTV_DESCRIPTION,IDC_IPTV_DESC_CONT);
        p_des_tent = ctrl_get_child_by_id(p_ctrl, IDC_IPTV_DESC_TITLE);
        if(p_des_tent == NULL)
        {
            return NULL;
        }
        return (u16 *)text_get_content(p_des_tent);
    }
    else if(category == IPTV_VARIETY)
    {
      al_iptv_play_origin_info_t *p_data_iptv = (al_iptv_play_origin_info_t *)on_ui_iptv_get_play_data();
      if(p_data_iptv != NULL)
      {
          if(p_data_iptv->origin != NULL)
          {
            return p_data_iptv->play_origin_info[episode_num].urltitle;
          }
      }
    }
    return NULL;
}
void ui_iptvdesc_get_info_url(u8  category, u16 episode_num,u8 **tvQid, u8 **vid)
{
    al_iptv_play_origin_info_t *p_data = (al_iptv_play_origin_info_t *)on_ui_iptv_get_play_data();

    *tvQid = NULL;
    *vid = NULL;

    if(ui_iptv_dat->category != category)
    {
        return;
    }
    if(p_data->play_origin_info == NULL)
    {
        return;
    }
    
    if(category != IPTV_VARIETY)
    {
        if(category == IPTV_MOVIE)
        {
            *tvQid 	=  p_data->play_origin_info[0].tvQid;
            *vid 	=  p_data->play_origin_info[0].vid;
        }
        else
        {
            *tvQid	=  p_data->play_origin_info[episode_num].tvQid;
            *vid		=  p_data->play_origin_info[episode_num].vid;
        }
    }
    else if(category == IPTV_VARIETY)
    {
            *tvQid 	= p_data->play_origin_info[episode_num].tvQid;
            *vid		= p_data->play_origin_info[episode_num].vid;	
    }
    return;
}

BEGIN_KEYMAP(iptv_des_cont_keymap, ui_comm_root_keymap)
  ON_EVENT(V_KEY_CANCEL, MSG_EXIT)
  ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
END_KEYMAP(iptv_des_cont_keymap, ui_comm_root_keymap)

BEGIN_MSGPROC(iptv_des_cont_proc, ui_comm_root_proc)
  ON_COMMAND(MSG_IPTV_EVT_NEW_VDO_INFO_ARRIVE,on_iptv_des_vdo_info_arrive)
  ON_COMMAND(MSG_IPTV_EVT_GET_VDO_INFO_FAIL,on_iptv_des_get_vdo_info_fail)
  
  ON_COMMAND(MSG_IPTV_EVT_NEW_INFO_URL_ARRIVE,on_iptv_des_series_info_url_arrive)
  ON_COMMAND(MSG_IPTV_EVT_GET_INFO_URL_FAIL,on_iptv_des_series_info_url_arrive_fail)

  ON_COMMAND(MSG_IPTV_EVT_NEW_RECMND_INFO_ARRIVE,on_iptv_des_series_new_rec_info_arrive)
  ON_COMMAND(MSG_UPDATE,on_iptv_des_pic_update)
  ON_COMMAND(MSG_PIC_EVT_DATA_ERROR, on_iptv_pic_draw_end)
  ON_COMMAND(MSG_PIC_EVT_DRAW_END, on_iptv_pic_draw_end)
  ON_COMMAND(MSG_INTERNET_PLUG_OUT, on_iptv_des_on_exit)
  ON_COMMAND(MSG_EXIT, on_iptv_des_on_exit)
  ON_COMMAND(MSG_DESTROY, ui_iptv_des_on_destory)
  #if ENABLE_ROLL
  ON_COMMAND(MSG_HEART_BEAT,ui_iptv_des_on_roll)
  #endif
END_MSGPROC(iptv_des_cont_proc, ui_comm_root_proc)


BEGIN_KEYMAP(iptv_des_video_rsc_keymap, NULL)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(iptv_des_video_rsc_keymap, NULL)

BEGIN_MSGPROC(iptv_des_video_rsc_proc, mbox_class_proc)
    ON_COMMAND(MSG_SELECT, on_iptv_des_video_rsc_begin_play)
    ON_COMMAND(MSG_FOCUS_DOWN, on_iptv_des_video_rsc_change_focus)
    ON_COMMAND(MSG_FOCUS_LEFT, on_iptv_des_video_rsc_change_focus)
    ON_COMMAND(MSG_FOCUS_RIGHT, on_iptv_des_video_rsc_change_focus)
END_MSGPROC(iptv_des_video_rsc_proc, mbox_class_proc)

BEGIN_KEYMAP(iptv_des_series_num_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(iptv_des_series_num_keymap, NULL)

BEGIN_MSGPROC(iptv_des_series_num_proc, mbox_class_proc)
  ON_COMMAND(MSG_SELECT, on_iptv_des_series_num_select)
  ON_COMMAND(MSG_FOCUS_LEFT, on_iptv_desc_series_num_change_focus)
  ON_COMMAND(MSG_FOCUS_RIGHT, on_iptv_desc_series_num_change_focus)
  ON_COMMAND(MSG_FOCUS_UP, on_iptv_desc_series_num_change_focus)
  ON_COMMAND(MSG_FOCUS_DOWN, on_iptv_desc_series_num_change_focus)
END_MSGPROC(iptv_des_series_num_proc, mbox_class_proc)


BEGIN_KEYMAP(iptv_des_mbox_keymap, NULL)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(iptv_des_mbox_keymap, NULL)

BEGIN_MSGPROC(iptv_des_mbox_proc, mbox_class_proc)
  ON_COMMAND(MSG_SELECT, on_iptv_desc_series_select)
 ON_COMMAND(MSG_FOCUS_UP, on_iptv_desc_series_change_focus)
 ON_COMMAND(MSG_FOCUS_DOWN, on_iptv_desc_series_change_focus)
END_MSGPROC(iptv_des_mbox_proc, mbox_class_proc)

BEGIN_KEYMAP(iptv_des_video_rsc_list_keymap, NULL)
  ON_EVENT(V_KEY_OK, MSG_SELECT)
  ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
  ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
  ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
  ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
END_KEYMAP(iptv_des_video_rsc_list_keymap, NULL)

BEGIN_MSGPROC(iptv_des_video_rsc_list_proc, cbox_class_proc)
 ON_COMMAND(MSG_SELECT, on_iptv_desc_rsc_change_focus)
 ON_COMMAND(MSG_FOCUS_LEFT, on_iptv_desc_rsc_change_focus)
 ON_COMMAND(MSG_FOCUS_RIGHT, on_iptv_desc_rsc_change_focus)
 ON_COMMAND(MSG_FOCUS_UP, on_iptv_desc_rsc_change_focus)
 ON_COMMAND(MSG_FOCUS_DOWN, on_iptv_desc_rsc_change_focus)
END_MSGPROC(iptv_des_video_rsc_list_proc, cbox_class_proc)

BEGIN_KEYMAP(iptv_recomm_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
	  ON_EVENT(V_KEY_BACK, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(iptv_recomm_keymap, NULL)

BEGIN_MSGPROC(iptv_recomm_proc, cont_class_proc)
    ON_COMMAND(MSG_SELECT,on_iptv_recomm_focus_move)
    ON_COMMAND(MSG_FOCUS_LEFT, on_iptv_recomm_focus_move)
    ON_COMMAND(MSG_FOCUS_RIGHT, on_iptv_recomm_focus_move)
    ON_COMMAND(MSG_FOCUS_UP, on_iptv_desc_recomm_info_change_focus)
END_MSGPROC(iptv_recomm_proc, cont_class_proc)

BEGIN_KEYMAP(iptv_des_variety_list_keymap, NULL)
    ON_EVENT(V_KEY_MENU, MSG_EXIT)
	ON_EVENT(V_KEY_BACK, MSG_EXIT)
    ON_EVENT(V_KEY_CANCEL, MSG_EXIT_ALL)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
    ON_EVENT(V_KEY_UP, MSG_FOCUS_UP)
    ON_EVENT(V_KEY_DOWN, MSG_FOCUS_DOWN)
    ON_EVENT(V_KEY_PAGE_UP, MSG_PAGE_UP)
    ON_EVENT(V_KEY_PAGE_DOWN, MSG_PAGE_DOWN)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
END_KEYMAP(iptv_des_variety_list_keymap, NULL)

BEGIN_MSGPROC(iptv_des_variety_list_proc, list_class_proc)
    ON_COMMAND(MSG_FOCUS_LEFT, on_iptv_variety_list_focus_move)
    ON_COMMAND(MSG_FOCUS_RIGHT, on_iptv_variety_list_focus_move)
    ON_COMMAND(MSG_FOCUS_DOWN, on_iptv_variety_list_focus_move)
    
    ON_COMMAND(MSG_PAGE_UP, on_iptv_variety_list_focus_move)
    ON_COMMAND(MSG_PAGE_DOWN, on_iptv_variety_list_focus_move)

    ON_COMMAND(MSG_FOCUS_UP, on_iptv_variety_list_focus_move)
    ON_COMMAND(MSG_SELECT, on_iptv_variety_list_focus_move)
END_MSGPROC(iptv_des_variety_list_proc, list_class_proc)


BEGIN_KEYMAP(iptv_des_variety_rsc_keymap, NULL)
    ON_EVENT(V_KEY_OK, MSG_SELECT)
    ON_EVENT(V_KEY_LEFT, MSG_FOCUS_LEFT)
    ON_EVENT(V_KEY_RIGHT, MSG_FOCUS_RIGHT)
END_KEYMAP(iptv_des_variety_rsc_keymap, NULL)

BEGIN_MSGPROC(iptv_des_variety_rsc_proc, text_class_proc)
    ON_COMMAND(MSG_SELECT, on_iptv_variety_collect_focus_move)
    ON_COMMAND(MSG_FOCUS_LEFT, on_iptv_variety_collect_focus_move)
    ON_COMMAND(MSG_FOCUS_RIGHT, on_iptv_variety_collect_focus_move)
END_MSGPROC(iptv_des_variety_rsc_proc, text_class_proc)




