/******************************************************************************/
/******************************************************************************/

/*!
 Include files
 */
#include "ui_common.h"
#include "commonData.h"
#include "IIPTVDataProvider.h"
#include "ui_iptv_api.h"
#include "ui_iptv_prot.h"

//#include "lib_char.h"
#if ENABLE_NETWORK

#define APP_IPTV    APP_NVOD

typedef enum
{
    IPTV_EVT_INIT_SUCCESS = ((APP_IPTV << 16) + 0),
    IPTV_EVT_INIT_FAIL,
    IPTV_EVT_DEINIT_SUCCESS,
    IPTV_EVT_NEW_RES_NAME_ARRIVE,
    IPTV_EVT_GET_RES_NAME_FAIL,
    IPTV_EVT_NEW_RES_CATGRY_ARRIVE,
    IPTV_EVT_GET_RES_CATGRY_FAIL,
    IPTV_EVT_NEW_PAGE_VDO_ARRIVE,
    IPTV_EVT_GET_PAGE_VDO_FAIL,
    IPTV_EVT_NEW_SEARCH_VDO_ARRIVE,
    IPTV_EVT_GET_SEARCH_VDO_FAIL,
    IPTV_EVT_NEW_VDO_INFO_ARRIVE,
    IPTV_EVT_GET_VDO_INFO_FAIL,
    IPTV_EVT_NEW_INFO_URL_ARRIVE,
    IPTV_EVT_GET_INFO_URL_FAIL,
    IPTV_EVT_NEW_PLAY_URL_ARRIVE,
    IPTV_EVT_GET_PLAY_URL_FAIL,
    IPTV_EVT_NEW_RECMND_INFO_ARRIVE,
    IPTV_EVT_GET_RECMND_INFO_FAIL,
    IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE,
    IPTV_EVT_GET_SOURCE_FORMAT_FAIL,
    IPTV_EVT_NEW_SEARCH_WORD_ARRIVE,
    IPTV_EVT_GET_SEARCH_WORD_FAIL,
    IPTV_EVT_MAX
} iptv_api_evt_t;




u16 ui_iptv_evtmap(u32 event);


extern iconv_t g_cd_utf8_to_utf16le;
extern iconv_t g_cd_utf16le_to_utf8;

static IPTV_DP_HDL_T *g_pIptvData = NULL;
static volatile u32 vdo_identify_code;
u32 search_res_id;
static u16 g_dp_state = IPTV_DP_STATE_UNINIT;
static u8  search_keyword[MAX_KEYWORD_LENGTH*3+1];
static BOOL g_plid_flag = TRUE;

static void set_vdo_id(VDO_ID_t *id,const char * qpid,const char * tvqid,char* src_code,int type)
{
	MT_ASSERT(qpid != NULL);
	MT_ASSERT(tvqid != NULL);
	
	memcpy(id->qpId,qpid,sizeof(id->qpId));
	memcpy(id->tvQid,tvqid,sizeof(id->tvQid));
	memcpy(id->sourceCode,src_code,sizeof(id->sourceCode));
	id->type = type;
}
//fuxl add
static al_iptv_search_word_t *ui_iptv_gen_search_word(IPTV_SEARCH_WORD_T *p_param)
{
    al_iptv_search_word_t *p_data = NULL;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i;

    if ((p_param->search != NULL) && (p_param->search->wordSize > 0))
    {
        p_data = (al_iptv_search_word_t *)SY_MALLOC(sizeof(al_iptv_search_word_t));
        MT_ASSERT(p_data != NULL);

        p_data->total_word = (u16)p_param->search->wordSize;
        p_data->word_list = (al_iptv_word_name_t *)SY_MALLOC(p_data->total_word * sizeof(al_iptv_word_name_t));
        MT_ASSERT(p_data->word_list != NULL);
        memset(p_data->word_list, 0, p_data->total_word * sizeof(al_iptv_word_name_t));

        for (i = 0; i < p_data->total_word; i++)
        {
            src_len = strlen(p_param->search->wordList[i].word) + 1;
            dest_len = src_len * sizeof(u16);
            inbuf = p_param->search->wordList[i].word;
            outbuf = (char *)SY_MALLOC(dest_len);
            MT_ASSERT(outbuf != NULL);
            p_data->word_list[i].word = (u16 *)outbuf;
            iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
        }
    }

    return p_data;
}

static void ui_iptv_free_search_word(u32 param)
{
    al_iptv_search_word_t *p_data = (al_iptv_search_word_t *)param;
    u16 i;

    if (p_data)
    {
        if (p_data->word_list)
        {
            for (i = 0; i < p_data->total_word; i++)
            {
                if (p_data->word_list[i].word)
                {
                    SY_FREE(p_data->word_list[i].word);
                }
            }
            SY_FREE(p_data->word_list);
        }
        SY_FREE(p_data);
    }
}


//SY modify
static al_iptv_name_res_list_t *ui_iptv_gen_name_res_list(IPTV_CATEGORY_LIST_T *p_param)
{
    al_iptv_name_res_list_t *p_data = NULL;
    IQY_Channel *p_item;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i;

    if ((p_param->array != NULL) && (p_param->total > 0))
    {
        p_data = (al_iptv_name_res_list_t *)SY_MALLOC(sizeof(al_iptv_name_res_list_t));
        MT_ASSERT(p_data != NULL);

        p_data->total_res = (u16)p_param->total;
        p_data->resList = (al_iptv_name_res_item_t *)SY_MALLOC(p_data->total_res * sizeof(al_iptv_name_res_item_t));
        MT_ASSERT(p_data->resList != NULL);
        memset(p_data->resList, 0, p_data->total_res * sizeof(al_iptv_name_res_item_t));

        for (i = 0; i < p_data->total_res; i++)
        {
            p_item = p_param->array + i;
	     DEBUG(UI_PLAY_API,INFO,"res id[%d],name[%s]\n",p_item->channelId,p_item->name);
            p_data->resList[i].res_id = p_item->channelId;
            if (p_item->name != NULL)
            {
                src_len = strlen(p_item->name) + 1;
                dest_len = src_len * sizeof(u16);
                inbuf = p_item->name;
                outbuf = (char *)SY_CALLOC(1,dest_len);
                MT_ASSERT(outbuf != NULL);
                p_data->resList[i].name = (u16 *)outbuf;
                iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
           }
        }
    }

    return p_data;
}

static void ui_iptv_free_name_res_list(u32 param)
{
    al_iptv_name_res_list_t *p_data = (al_iptv_name_res_list_t *)param;
    u16 i;

    if (p_data)
    {
        if (p_data->resList)
        {
            for (i = 0; i < p_data->total_res; i++)
            {
                if (p_data->resList[i].name)
                {
                    SY_FREE(p_data->resList[i].name);
                }
            }
            SY_FREE(p_data->resList);
        }
        SY_FREE(p_data);
    }
}

static al_iptv_cat_res_list_t *ui_iptv_gen_cat_res_list(IPTV_CATEGORY_TYPE_ALL_T *p_param)
{
    al_iptv_cat_res_list_t *p_data = NULL;
	IQY_SubTag *iqy_subtag;
    IQY_Channel * iqy_channel;
    IQY_ChannelLabelList *iqy_label;
    IQY_ChannelLabelInfo *iqy_label_info;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i, j, type_index;

    DEBUG(UI_PLAY_API,INFO,"@@@%s\n",__FUNCTION__);
    DEBUG(UI_PLAY_API,INFO,"count %d\n",p_param->category->total);
    if ((p_param->category->array != NULL) && (p_param->category->total > 0))
    {
        p_data = (al_iptv_cat_res_list_t *)SY_MALLOC(sizeof(al_iptv_cat_res_list_t));
        MT_ASSERT(p_data != NULL);

        p_data->total_res = (u16)p_param->category->total;
        p_data->resList = (al_iptv_cat_res_item_t *)SY_MALLOC(p_data->total_res * sizeof(al_iptv_cat_res_item_t));
        MT_ASSERT(p_data->resList != NULL);
        memset(p_data->resList, 0, p_data->total_res * sizeof(al_iptv_cat_res_item_t));

        for (i = 0; i < p_data->total_res; i++)
        {
            DEBUG(UI_PLAY_API,INFO,"cate id[%d] name[%s]\n",p_param->category->array[i].channelId,p_param->category->array[i].name);
            iqy_channel = p_param->category->array + i;
	     iqy_label     = p_param->category->labels + i;
            p_data->resList[i].res_id = iqy_channel->channelId;
	     /*read channel  name*/
            src_len = strlen(iqy_channel->name) + 1;
            dest_len = src_len * sizeof(u16);
            inbuf = iqy_channel->name;
            outbuf = (char *)SY_CALLOC(1,dest_len);
            MT_ASSERT(outbuf != NULL);
            p_data->resList[i].res_name = (u16 *)outbuf;
            iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
			
            if ((iqy_label != NULL) && (iqy_label->size > 0))
            {
#if 0            
            	  for(type_index=0;type_index<iqy_channel->tagSize;type_index++)
            	  {
			static char UTF8_type[] = {0xe7,0xb1,0xbb,0xe5,0x9e,0x8b,0x00};            	  
            	  	if (UTF8_type[0])//(strcmp((char*)UTF8_type,(char*)iqy_channel->tags[type_index].name) == 0)
        	  	{
   			    p_data->resList[i].total_cat = iqy_channel->tags[type_index].subTagSize;
     			    p_data->resList[i].catList = (al_iptv_cat_item_t *)SY_MALLOC(p_data->resList[i].total_cat * sizeof(al_iptv_cat_item_t));
     			    MT_ASSERT(p_data->resList[i].catList != NULL);
     			    memset(p_data->resList[i].catList, 0, p_data->resList[i].total_cat * sizeof(al_iptv_cat_item_t));
			    break;
        	  	}
			p_data->resList[i].total_cat = 0;
            	  }
#endif				  
		  p_data->resList[i].total_cat = iqy_label->size;
  		  p_data->resList[i].catList = (al_iptv_cat_item_t *)SY_CALLOC(1,p_data->resList[i].total_cat * sizeof(al_iptv_cat_item_t));
  		  MT_ASSERT(p_data->resList[i].catList != NULL);
		  
               // DEBUG(UI_PLAY_API,INFO,"type count = %d\n",p_data->resList[i].total_cat);
                for (j = 0; j < p_data->resList[i].total_cat; j++) // iqy_tag->subTagSize
                {
                    iqy_label_info = iqy_label->labelList + j;
//DEBUG(UI_PLAY_API,INFO,"name[%s]\n",iqy_label_info->name);
                    if (iqy_label_info->name != NULL)
                    {
                        src_len = strlen(iqy_label_info->name) + 1;
                        dest_len = src_len * sizeof(u16);
                        inbuf = iqy_label_info->name;
                        outbuf = (char *)SY_CALLOC(1,dest_len);
                        MT_ASSERT(outbuf != NULL);
                        p_data->resList[i].catList[j].name = (u16 *)outbuf;
                        iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
                    }
//DEBUG(UI_PLAY_API,INFO,"plid[%s]\n",iqy_label_info->plid);
                    if (iqy_label_info->plid != NULL)
                    {
                        dest_len = sizeof (iqy_label_info->plid) +1;
                        outbuf = (char *)SY_CALLOC(1,dest_len);
                        MT_ASSERT(outbuf != NULL);
                        memcpy((char *)outbuf, (char *)iqy_label_info->plid,sizeof (iqy_label_info->plid));
                        p_data->resList[i].catList[j].key = (u8 *)outbuf;
                    }

                    p_data->resList[i].catList[j].total_vdo = 1000; //tmp
                }
            }
            else
            {
                p_data->resList[i].total_cat = 0;
                p_data->resList[i].catList = NULL;
            }
			
		if ((iqy_channel->tags != NULL) && (iqy_channel->tagSize > 0))
            {
		    p_data->resList[i].total_filter = iqy_channel->tagSize;
		    p_data->resList[i].filter = (al_iptv_filter_item_t *)SY_MALLOC(p_data->resList[i].total_filter * sizeof(al_iptv_filter_item_t));
		    MT_ASSERT(p_data->resList[i].filter != NULL);
		    memset(p_data->resList[i].filter, 0, p_data->resList[i].total_filter * sizeof(al_iptv_filter_item_t));


                for(type_index=0;type_index<iqy_channel->tagSize;type_index++)
            	   {
			static char UTF8_type[] = {0xe7,0xb1,0xbb,0xe5,0x9e,0x8b,0x00};            	  
            	  	if (UTF8_type[0])//(strcmp((char*)UTF8_type,(char*)iqy_channel->tags[type_index].name) == 0)
        	  	{
			    p_data->resList[i].filter[type_index].total_vdo = 1000; //tmp
   			    p_data->resList[i].filter[type_index].subTagSize = iqy_channel->tags[type_index].subTagSize;
     			    p_data->resList[i].filter[type_index].subtag = (al_iptv_tags_item_t *)SY_MALLOC(p_data->resList[i].filter[type_index].subTagSize * sizeof(al_iptv_tags_item_t));
     			    MT_ASSERT(p_data->resList[i].filter[type_index].subtag != NULL);
     			    memset(p_data->resList[i].filter[type_index].subtag, 0, p_data->resList[i].filter[type_index].subTagSize * sizeof(al_iptv_tags_item_t));

			    if (iqy_channel->tags[type_index].name != NULL)
			    {
				    src_len = strlen(iqy_channel->tags[type_index].name) + 1;
				    dest_len = src_len * sizeof(u16);
				    inbuf = iqy_channel->tags[type_index].name;
				    outbuf = (char *)SY_CALLOC(1,dest_len);
				    MT_ASSERT(outbuf != NULL);
				    p_data->resList[i].filter[type_index].name = (u16 *)outbuf;
				    //DEBUG(UI_PLAY_API,INFO,"[debug] type_index = %d %s %d tagname = %s \n",type_index,__FUNCTION__,__LINE__,iqy_channel->tags[type_index].name);
				    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
			    }

			    DEBUG(UI_PLAY_API,INFO,"type count = %d\n",p_data->resList[i].filter[type_index].subTagSize);
			    for (j = 0; j < p_data->resList[i].filter[type_index].subTagSize; j++) // iqy_tag->subTagSize
			    {
				    iqy_subtag =iqy_channel->tags[type_index].subtag + j;

				    if (iqy_subtag->name != NULL)
				    {
					    src_len = strlen(iqy_subtag->name) + 1;
					    dest_len = src_len * sizeof(u16);
					    inbuf = iqy_subtag->name;
					    outbuf = (char *)SY_CALLOC(1,dest_len);
					    MT_ASSERT(outbuf != NULL);
					    p_data->resList[i].filter[type_index].subtag[j].name = (u16 *)outbuf;
					    //DEBUG(UI_PLAY_API,INFO,"[debug] %s %d subname = %s dest_len =%d\n",__FUNCTION__,__LINE__,iqy_subtag->name,dest_len);
					    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
				    }

				    if (iqy_subtag->value != NULL)
				    {
					    dest_len = strlen(iqy_subtag->value) + 1;
					    outbuf = (char *)SY_MALLOC(dest_len);
					    MT_ASSERT(outbuf != NULL);
					    strcpy((char *)outbuf, (char *)iqy_subtag->value);
					    //DEBUG(UI_PLAY_API,INFO,"[debug] %s %d subname = %s \n",__FUNCTION__,__LINE__,iqy_subtag->value);
					    p_data->resList[i].filter[type_index].subtag[j].key = (u8 *)outbuf;
				    }

			    }


			}
		  }
            }
            else
            {
                p_data->resList[i].total_filter = 0;
                p_data->resList[i].filter = NULL;
            }
			
			
        }
    }

    return p_data;
}

static void ui_iptv_free_cat_res_list(u32 param)
{
    al_iptv_cat_res_list_t *p_data = (al_iptv_cat_res_list_t *)param;
    u16 i, j,k;

    if (p_data)
    {
        if (p_data->resList)
        {
            for (i = 0; i < p_data->total_res; i++)
            {
                SY_FREE(p_data->resList[i].res_name);
                if (p_data->resList[i].catList)
                {
                    for (j = 0; j < p_data->resList[i].total_cat; j++)
                    {
                        if (p_data->resList[i].catList[j].name)
                        {
                            SY_FREE(p_data->resList[i].catList[j].name);
                        }
                        if (p_data->resList[i].catList[j].key)
                        {
                            SY_FREE(p_data->resList[i].catList[j].key);
                        }
                    }
                    SY_FREE(p_data->resList[i].catList);
                }
                if (p_data->resList[i].filter)
                {
                    for (j = 0; j < p_data->resList[i].total_filter; j++)
                    {
                        if (p_data->resList[i].filter[j].name)
                        {
                            SY_FREE(p_data->resList[i].filter[j].name);
                        }
			for(k=0; k< p_data->resList[i].filter[j].subTagSize; k++)
			{
				if (p_data->resList[i].filter[j].subtag[k].name)
				{
					SY_FREE(p_data->resList[i].filter[j].subtag[k].name);
				}
				if (p_data->resList[i].filter[j].subtag[k].key)
				{
					SY_FREE(p_data->resList[i].filter[j].subtag[k].key);
				}
			}
			SY_FREE(p_data->resList[i].filter[j].subtag);
                    }
                    SY_FREE(p_data->resList[i].filter);
                }
            }
            SY_FREE(p_data->resList);
        }
        SY_FREE(p_data);
    }
}

static al_iptv_vdo_list_t *ui_iptv_gen_vdo_list(IPTV_CHANNEL_LIST_T *p_param)
{
    al_iptv_vdo_list_t *p_data = NULL;
    IQY_AlbumInfo *palbum;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    u16 i;

    p_data = (al_iptv_vdo_list_t *)SY_MALLOC(sizeof(al_iptv_vdo_list_t));
    MT_ASSERT(p_data != NULL);

    if ((p_param->array != NULL) && (p_param->number > 0))
    {
        p_data->total_page = (u32)p_param->pagecount;
        p_data->page_num = (u32)p_param->page_num;
        p_data->total_vdo = (u32)p_param->totalcount;
        p_data->vdo_cnt = (u16)p_param->number;
        p_data->vdoList = (al_iptv_vdo_item_t *)SY_MALLOC(p_data->vdo_cnt * sizeof(al_iptv_vdo_item_t));
        MT_ASSERT(p_data->vdoList != NULL);
        memset(p_data->vdoList, 0, p_data->vdo_cnt * sizeof(al_iptv_vdo_item_t));

        DEBUG(UI_PLAY_API,INFO,"@@@number=%d", p_param->number);
        for (i = 0; i < p_data->vdo_cnt; i++)
        {
            palbum = p_param->array + i;
             // TODO: qpId[IQY_LEN_QPID]
            set_vdo_id(&p_data->vdoList[i].vdo_id, palbum->qpId, palbum->tvQid,palbum->sourceCode,palbum->type);
            
            p_data->vdoList[i].res_id = (u32)palbum->chnId;
            p_data->vdoList[i].b_single_page = (u8)0; // TODO: b_single_page
            if (palbum->name != NULL)
            {
                src_len = strlen(palbum->name) + 1;
                dest_len = src_len * sizeof(u16);
                inbuf = palbum->name;
                outbuf = (char *)SY_CALLOC(1,dest_len);
                MT_ASSERT(outbuf != NULL);
                p_data->vdoList[i].name = (u16 *)outbuf;
                iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
            }
	     DEBUG(UI_PLAY_API,INFO,"pic260x360[%s]\ntvPic[%s]\n",palbum->pic260x360,palbum->tvPic);
            if (palbum->pic260x360  != NULL)
            {
                dest_len = strlen(palbum->pic260x360) + 1;
                outbuf = (char *)SY_MALLOC(dest_len);
                MT_ASSERT(outbuf != NULL);
                strcpy((char *)outbuf, (char *)palbum->pic260x360);
                p_data->vdoList[i].img_url = (u8 *)outbuf;
            }
	     p_data->vdoList[i].album = palbum;
            DEBUG(UI_PLAY_API,INFO,"@@@i=%d, vdo_qpid=%s, tvQid = %s name=%s album[%x]\n",
				i, palbum->qpId,  palbum->tvQid,palbum->name,palbum);		 
        }
    }
    else
    {
        p_data->total_page = (u32)p_param->pagecount;
        p_data->page_num = (u32)p_param->page_num;
        p_data->total_vdo = (u32)p_param->totalcount;
        p_data->vdo_cnt = (u16)p_param->number;
        p_data->vdoList = NULL;
    }

    return p_data;
}

static void ui_iptv_free_vdo_list(u32 param)
{
    al_iptv_vdo_list_t *p_data = (al_iptv_vdo_list_t *)param;
    u16 i;

    if (p_data)
    {
        if (p_data->vdoList)
        {
            for (i = 0; i < p_data->vdo_cnt; i++)
            {
                if (p_data->vdoList[i].name)
                {
                    SY_FREE(p_data->vdoList[i].name);
                }
                if (p_data->vdoList[i].img_url)
                {
                    SY_FREE(p_data->vdoList[i].img_url);
                }
            }
            SY_FREE(p_data->vdoList);
        }
        SY_FREE(p_data);
    }
}

u16 chrn (char *str, char c)
{
    u16 cnt = 0;
    while (*str != '\0')
    {
        if (*str == c)
        {
            cnt++;
        }
        str++;
    }
    return cnt;
}
#if 0
static void ui_iptv_add_orgn_item(al_iptv_vdo_info_t *p_data, char *name)
{
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    src_len = strlen(name) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = name;
    outbuf = (char *)SY_MALLOC(dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->orgnList[p_data->orgn_cnt].name = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    p_data->orgn_cnt++;
}
#endif
static al_iptv_vdo_info_t *ui_iptv_gen_vdo_info(IPTV_CHANNEL_INFO_T *p_param)
{
    al_iptv_vdo_info_t *p_data = NULL;
    char *inbuf, *outbuf;
    size_t src_len, dest_len;
    int i;

    DEBUG(UI_PLAY_API,INFO,"@@@%s %s\n", __FUNCTION__, p_param->album->name);
    DEBUG(UI_PLAY_API,INFO,"qpid[%s]\n", p_param->album->qpId);

    p_data = (al_iptv_vdo_info_t *)SY_MALLOC(sizeof(al_iptv_vdo_info_t));
    MT_ASSERT(p_data != NULL);
    memset(p_data, 0, sizeof(al_iptv_vdo_info_t));

    p_data->res_id = (u32)p_param->cat_id;
    p_data->time= (u32)p_param->album->len;
    p_data->tvsets= (u16)(p_param->album->tvsets > p_param->album->tvCount? p_param->album->tvsets:p_param->album->tvCount);
    set_vdo_id(&p_data->vdo_id,p_param->album->qpId, p_param->album->tvQid, p_param->album->sourceCode,p_param->album->type);

     if(p_param->album->name != NULL)
    {
    src_len = strlen(p_param->album->name) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf =p_param->album->name;
    outbuf = (char *)SY_CALLOC(1,dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->name = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }
	
     if(p_param->album->tag != NULL) // TODO: tag ->> area
    {
    src_len = strlen(p_param->album->tag) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = p_param->album->tag;
    outbuf = (char *)SY_CALLOC(1,dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->area = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }

     if(p_param->album->cast.director != NULL) // 导演
   {
    src_len = strlen(p_param->album->cast.director) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = p_param->album->cast.director;
    outbuf = (char *)SY_CALLOC(1,dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->director = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }

     if(p_param->album->cast.mainActor != NULL) //主演
    {
    src_len = strlen(p_param->album->cast.mainActor) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = p_param->album->cast.mainActor;
    outbuf = (char *)SY_CALLOC(1,dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->actor = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }
	
     if(p_param->album->initIssueTime != NULL) // 年代
    {
     u8 i;
     for(i = 0;i<strlen(p_param->album->initIssueTime);i++)
     {
        if(p_param->album->initIssueTime[i] == '-' || p_param->album->initIssueTime[i] == ' ')
            p_param->album->initIssueTime[i] = '\0';
     }
    
    src_len = strlen(p_param->album->initIssueTime) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = p_param->album->initIssueTime;
    outbuf = (char *)SY_CALLOC(1,dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->years = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }


     if(p_param->album->tag != NULL)
    {
    src_len = strlen(p_param->album->tag) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = p_param->album->tag;
    outbuf = (char *)SY_CALLOC(1,dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->attr = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }

     {
    char buff[20];
    sprintf(buff,"%f",p_param->album->score);// TODO: score
    src_len = strlen(buff) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = buff;
    outbuf = (char *)SY_CALLOC(1,dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->score = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }

    if(p_param->album->desc != NULL)
   {
    src_len = strlen(p_param->album->desc) + 1;
    dest_len = src_len * sizeof(u16);
    inbuf = p_param->album->desc;
    outbuf = (char *)SY_CALLOC(1,dest_len);
    MT_ASSERT(outbuf != NULL);
    p_data->description = (u16 *)outbuf;
    iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
    }

     if(p_param->album->pic260x360  != NULL)
    {
    dest_len = strlen(p_param->album->pic260x360) + 1;
    outbuf = (char *)SY_MALLOC(dest_len);
    MT_ASSERT(outbuf != NULL);
    strcpy((char *)outbuf, (char *)p_param->album->pic260x360);
    p_data->img_url = (u8 *)outbuf;
    }
    if (p_param->format.total > 0)
    {
    	p_data->format = SY_MALLOC(sizeof (*p_data->format));
       MT_ASSERT(p_data->format != NULL);
	p_data->format->formatList = SY_MALLOC(sizeof (int) * p_param->format.total + 1);
       MT_ASSERT(p_data->format->formatList!= NULL);
	p_data->format->total_format = p_param->format.total;
	for (i=0;i<p_param->format.total;i++)
	{
		p_data->format->formatList[i] = p_param->format.format[i];
	}
    }
#if 0
    if (p_param->album->sourceCode != NULL)
    {
        cnt = chrn(p_param->album->sourceCode, separator) + 1;
        p_data->orgnList = SY_MALLOC(cnt * sizeof(al_iptv_orgn_item_t));
        MT_ASSERT(p_data->orgnList != NULL);
        q = p_param->album->sourceCode;
        while (strlen(q) > 0)
        {
            p = strchr(q, separator);
            if (p == NULL)
            {
                ui_iptv_add_orgn_item(p_data, q);
                break;
            }
            else if (*p == separator)
            {
                *p = '\0';
                p++;
                ui_iptv_add_orgn_item(p_data, q);
                q = p;
                continue;
            }
            break;
        }
    }
 #endif
    return p_data;
}

static void ui_iptv_free_vdo_info(u32 param)
{
    al_iptv_vdo_info_t *p_data = (al_iptv_vdo_info_t *)param;
    if (p_data)
    {
        if (p_data->name)
        {
            SY_FREE(p_data->name);
        }
        if (p_data->area)
        {
            SY_FREE(p_data->area);
        }
        if (p_data->director)
        {
            SY_FREE(p_data->director);
        }
        if (p_data->actor)
        {
            SY_FREE(p_data->actor);
        }
        if (p_data->score)
        {
            SY_FREE(p_data->score);
        }
        if (p_data->description)
        {
            SY_FREE(p_data->description);
        }
        if (p_data->attr)
        {
            SY_FREE(p_data->attr);
        }
        if (p_data->years)
        {
            SY_FREE(p_data->years);
        }
        if (p_data->img_url)
        {
            SY_FREE(p_data->img_url);
        }
        if (p_data->orgnList)
        {
            SY_FREE(p_data->orgnList);
        }
	 if (p_data->format)
	 {
	     if (p_data->format->formatList)
		 SY_FREE(p_data->format->formatList);
		 
	     SY_FREE(p_data->format);
	 }
        SY_FREE(p_data);
    }
}

static al_iptv_info_url_list_t *ui_iptv_gen_info_url(IPTV_CHANNEL_INFO_T *p_param)
{
    al_iptv_info_url_list_t *p_data = NULL;
    IQY_Episode *pepisode;
    char *outbuf;
    size_t dest_len,src_len;
    u16 i;
    char *inbuf;

    DEBUG(UI_PLAY_API,INFO,"album type[%d]\n",p_param->album->type);
    p_data = (al_iptv_info_url_list_t *)SY_MALLOC(sizeof(al_iptv_info_url_list_t));
    MT_ASSERT(p_data != NULL);
    memset(p_data, 0, sizeof(al_iptv_info_url_list_t));

    if (p_param->album->type == IQY_TYPE_MOVE) //move
    {
    	p_data->count = 1;
	p_data->page_count = 1;
	p_data->page_total = 1;
    	p_data->urlList = (al_iptv_info_url_item_t *)SY_MALLOC(p_data->count * sizeof(al_iptv_info_url_item_t));
	memset( p_data->urlList,0,p_data->count * sizeof(al_iptv_info_url_item_t));
       MT_ASSERT(p_data->urlList != NULL);
	   
       memcpy((char *) p_data->urlList[0].tvQid, (char *)p_param->album->tvQid,sizeof(p_data->urlList[0].tvQid));
       memcpy((char *) p_data->urlList[0].vid, (char *)p_param->album->vid,sizeof(p_data->urlList[0].vid));
	DEBUG(UI_PLAY_API,INFO,"tvQid[%s],vid[%s]\n",p_data->urlList[0].tvQid,p_data->urlList[0].vid);

       src_len = strlen(p_param->album->name ) + 1;
       dest_len = src_len * sizeof(u16);
       inbuf =p_param->album->name ;
       outbuf = (char *)SY_CALLOC(1,dest_len);
       MT_ASSERT(outbuf != NULL);
       p_data->urlList[0].urltitle = (u16 *)outbuf;
       iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
       return p_data;
    }

    p_data->count = (u32)p_param->this_page_size;
    if (p_param->episode_total > 0 && p_param->this_page_size > 0)
        p_data->page_count = (u32)(p_param->episode_total + p_param->this_page_size -1)/p_param->this_page_size;
	
    p_data->page_total = (u32)p_param->episode_total;

    DEBUG(UI_PLAY_API,INFO,"url total  %d\n",p_param->episode_total);
    DEBUG(UI_PLAY_API,INFO,"url page_count  %d\n",p_data->page_count);
    DEBUG(UI_PLAY_API,INFO,"url page_total  %d\n",p_data->page_total);
    
    p_data->urlList = (al_iptv_info_url_item_t *)SY_MALLOC(p_data->count * sizeof(al_iptv_info_url_item_t));
    memset( p_data->urlList,0,p_data->count * sizeof(al_iptv_info_url_item_t));
    MT_ASSERT(p_data->urlList != NULL);

    for (i = 0; i < p_data->count; i++)
    {
        pepisode = p_param->episodes + i;
		
       memcpy((char *) p_data->urlList[i].tvQid, (char *)pepisode->tvQid,sizeof(p_data->urlList[0].tvQid));
       memcpy((char *) p_data->urlList[i].vid, (char *)pepisode->vid,sizeof(p_data->urlList[0].vid));

	DEBUG(UI_PLAY_API,INFO,"url name[%s]\n",pepisode->name );
        //url title
        if(pepisode->name != NULL)
        {
          src_len = strlen(pepisode->name ) + 1;
          dest_len = src_len * sizeof(u16);
          inbuf = pepisode->name ;
          outbuf = (char *)SY_CALLOC(1,dest_len);
          MT_ASSERT(outbuf != NULL);
          p_data->urlList[i].urltitle = (u16 *)outbuf;
          iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
        }
    }

    return p_data;
}

static void ui_iptv_free_info_url(u32 param)
{
    al_iptv_info_url_list_t *p_data = (al_iptv_info_url_list_t *)param;
    u16 i;

    if (p_data)
    {
        if (p_data->urlList)
        {
            for (i = 0; i < p_data->count; i++)
            {
                if (p_data->urlList[i].urltitle)
                {
                    SY_FREE(p_data->urlList[i].urltitle);
                } 
            }
            SY_FREE(p_data->urlList);
        }
        SY_FREE(p_data);
    }
}

static al_iptv_recomand_info_t *ui_iptv_gen_remend_info(IPTV_RECMD_INFO_T *p_param)
{
	al_iptv_recomand_info_t *p_data = NULL;
	IQY_AlbumInfo *p_item;
	u8 i = 0;
	char *inbuf,*outbuf;
	size_t src_len,dest_len;

	DEBUG(UI_PLAY_API,INFO,"total[%d]\n",p_param->channel_num);
	p_data = (al_iptv_recomand_info_t *)SY_MALLOC(sizeof(al_iptv_recomand_info_t));
	MT_ASSERT(p_data != NULL);
	memset(p_data, 0, sizeof(al_iptv_recomand_info_t));

	p_data->number = (u32)p_param->channel_num;
	if(p_data->number == 0)
	{
		return p_data;
	}

	p_data->recmd  = (iptv_recmd_channel_t *)SY_MALLOC(p_data->number * sizeof(iptv_recmd_channel_t));
	memset(p_data->recmd,0,p_data->number * sizeof(iptv_recmd_channel_t));
	MT_ASSERT(p_data->recmd != NULL);

	for (i = 0; i < p_data->number; i++)
	{
		p_item = p_param->albums+ i;
		if(p_item->name != NULL)
		{
			src_len = strlen(p_item->name) + 1;
			dest_len = src_len * sizeof(u16);
			inbuf = p_item->name;
			outbuf = (char *)SY_CALLOC(1,dest_len);
			MT_ASSERT(outbuf != NULL);
			p_data->recmd[i].title= (u16 *)outbuf;
			iconv(g_cd_utf8_to_utf16le, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
		}
#if 0				
		if(p_item->types != NULL)
		{
			dest_len = strlen(p_item->types) + 1;
			outbuf = (char *)SY_MALLOC(dest_len);
			MT_ASSERT(outbuf != NULL);
			strcpy((char *)outbuf, (char *)p_item->types);
			p_data->recmd[i].types = (u8 *)outbuf;
		}
		if(p_item->score != NULL)
		{
			dest_len = strlen(p_item->score) + 1;
			outbuf = (char *)SY_MALLOC(dest_len);
			MT_ASSERT(outbuf != NULL);
			strcpy((char *)outbuf, (char *)p_item->score);
			p_data->recmd[i].score = (u8 *)outbuf;
		}
		if(p_item->currentcollect != NULL)
		{
			dest_len = strlen(p_item->currentcollect) + 1;
			outbuf = (char *)SY_MALLOC(dest_len);
			MT_ASSERT(outbuf != NULL);
			strcpy((char *)outbuf, (char *)p_item->currentcollect);
			p_data->recmd[i].currentcollect = (u8 *)outbuf;
		}

		if(p_item->clarity != NULL)
		{
			dest_len = strlen(p_item->clarity) + 1;
			outbuf = (char *)SY_MALLOC(dest_len);
			MT_ASSERT(outbuf != NULL);
			strcpy((char *)outbuf, (char *)p_item->clarity);
			p_data->recmd[i].clarity = (u8 *)outbuf;
		}
#endif
		set_vdo_id(&p_data->recmd[i].id , p_item->qpId,p_item->tvQid, p_item->sourceCode, p_item->type);
		p_data->recmd[i].cat_id = p_item->chnId;
		p_data->recmd[i].album = p_item;

		if(p_item->pic260x360 != NULL)
		{
			dest_len = strlen(p_item->pic260x360) + 1;
			outbuf = (char *)SY_MALLOC(dest_len);
			MT_ASSERT(outbuf != NULL);
			strcpy((char *)outbuf, (char *)p_item->pic260x360);
			p_data->recmd[i].img = (u8 *)outbuf;
		}
	}
	return p_data;
}

static void ui_iptv_free_remend_info(u32 param)
{
    al_iptv_recomand_info_t *p_data = (al_iptv_recomand_info_t *)param;
    u16 i;
    if (p_data)
    {
        if (p_data->recmd)
        {
            for (i = 0; i < p_data->number; i++)
            {
                if (p_data->recmd[i].title)
                {
                    SY_FREE(p_data->recmd[i].title);
                }
                if (p_data->recmd[i].img)
                {
                    SY_FREE(p_data->recmd[i].img);
                }
            }
            SY_FREE(p_data->recmd);
        }
        SY_FREE(p_data);
    }
}

static al_iptv_source_format_t *ui_iptv_gen_source_format(IPTV_VIDEO_FORMAT_T * format)
{
	al_iptv_source_format_t *p_data = NULL;
	int i;
	p_data = (al_iptv_source_format_t *)SY_MALLOC(sizeof(al_iptv_source_format_t));
	MT_ASSERT(p_data != NULL);

	p_data->total_format = 0;
	p_data->formatList = (int *)SY_MALLOC(IPTV_MAX_FORMAT_COUNT * sizeof(*p_data->formatList));
	MT_ASSERT(p_data->formatList != NULL);
	for(i=0;i<format->total;i++)
	{
		p_data->formatList[i] = format->format[i];
	}
	p_data->total_format = format->total;
	return p_data;
}

static void ui_iptv_free_source_format(u32 param)
{
    al_iptv_source_format_t *p_data = (al_iptv_source_format_t *)param;

    if (p_data)
    {
        if (p_data->formatList)
        {
            SY_FREE(p_data->formatList);
        }
        SY_FREE(p_data);
    }
}

static al_iptv_play_url_list_t *ui_iptv_gen_play_url(IPTV_CHANNEL_PLAY_URL_T *p_param)
{
    al_iptv_play_url_list_t *p_data = NULL;
    IPTV_CHANNEL_URL_T *p_item;
    u16 i;

    if ((p_param->playurls != NULL) && (p_param->number > 0))
    {
        p_data = (al_iptv_play_url_list_t *)SY_MALLOC(sizeof(al_iptv_play_url_list_t));
        MT_ASSERT(p_data != NULL);

        p_data->total_url = (u16)p_param->number;
        p_data->pp_urlList = (al_iptv_play_url_info_t*)SY_MALLOC(p_data->total_url * sizeof(p_data->pp_urlList[0]));
        MT_ASSERT(p_data->pp_urlList != NULL);

        for (i = 0; i < p_data->total_url; i++)
        {
            p_item = p_param->playurls + i;

            if (p_item->tvQid[0]!=0  && p_item->vid[0]!=0)
            {
		memcpy(p_data->pp_urlList[i].tvQid,p_item->tvQid,sizeof(p_data->pp_urlList[i].tvQid));
		memcpy(p_data->pp_urlList[i].vid,p_item->vid,sizeof(p_data->pp_urlList[i].vid));
            }
        }
    }

    return p_data;
}

static void ui_iptv_free_play_url(u32 param)
{
    al_iptv_play_url_list_t *p_data = (al_iptv_play_url_list_t *)param;

    if (p_data)
    {
        if (p_data->pp_urlList)
        {            
            SY_FREE(p_data->pp_urlList);
        }
        SY_FREE(p_data);
    }
}


static RET_CODE  iptv_dp_event_callback(IPTV_EVENT_TYPE event, u32 param)
{
    event_t evt = {0};
    IPTV_CHANNEL_INFO_T *p_param = NULL;
    switch(event)
    {
        case IPTV_INIT_SUCCESS:
        {
            evt.id = IPTV_EVT_INIT_SUCCESS;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_INIT_FAIL:
        {
            evt.id = IPTV_EVT_INIT_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_DEINIT_SUCCESS:
        {
            evt.id = IPTV_EVT_DEINIT_SUCCESS;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_CATEGORY_ARRIVAL:
        {
            if((IPTV_CATEGORY_LIST_T *)param != NULL)
            {
             IPTV_CATEGORY_LIST_T *p_param = (IPTV_CATEGORY_LIST_T *)param;
 
             DEBUG(UI_PLAY_API,INFO,"@@@IPTV_CATEGORY_ARRIVAL status=%d\n", p_param->status);
             if (p_param->status == IPTV_DATA_SUCCESS)
             {
                 al_iptv_name_res_list_t *p_data = ui_iptv_gen_name_res_list(p_param);
 
                 if (p_data != NULL)
                 {
                     evt.id = IPTV_EVT_NEW_RES_NAME_ARRIVE;
                     evt.data1 = (u32)p_data;
                     ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                     break;
                 }
             }
            }
            evt.id = IPTV_EVT_GET_RES_NAME_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_CATEGORY_TYPE_ARRIVAL:
        {
           if((IPTV_CATEGORY_TYPE_ALL_T *)param != NULL)
           {
            IPTV_CATEGORY_TYPE_ALL_T *p_param = (IPTV_CATEGORY_TYPE_ALL_T *)param;

            DEBUG(UI_PLAY_API,INFO,"@@@IPTV_CATEGORY_TYPE_ARRIVAL status=%d\n", p_param->status);
            if (p_param->status == IPTV_DATA_SUCCESS)
            {
                al_iptv_cat_res_list_t *p_data = ui_iptv_gen_cat_res_list(p_param);

                if (p_data != NULL)
                {
                    evt.id = IPTV_EVT_NEW_RES_CATGRY_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                    break;
                }
            }
           }
            evt.id = IPTV_EVT_GET_RES_CATGRY_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_NEW_PAGE_ARRIVAL:
        {
            IPTV_CHANNEL_LIST_T *p_param = (IPTV_CHANNEL_LIST_T *)param;

            DEBUG(UI_PLAY_API,INFO,"@@@IPTV_NEW_PAGE_ARRIVAL status=%d\n", p_param->status);
            if (p_param->identify == vdo_identify_code)
            {
                    DEBUG(UI_PLAY_API,INFO,"@@@IPTV_QUERY_MODE_CATGRY status[%d], query_mode[%d]\n",p_param->status,p_param->query_mode);
                    if (p_param->status == IPTV_DATA_SUCCESS)
                    {
                        al_iptv_vdo_list_t *p_data = ui_iptv_gen_vdo_list(p_param);

			   if (p_param->query_mode == IPTV_QUERY_MODE_CATGRY)
                        	evt.id = IPTV_EVT_NEW_PAGE_VDO_ARRIVE;
			   else if (p_param->query_mode == IPTV_QUERY_MODE_PINYIN)
                        	evt.id = IPTV_EVT_NEW_SEARCH_VDO_ARRIVE;
			   else
                        	evt.id = IPTV_EVT_NEW_PAGE_VDO_ARRIVE;
			   	
                        evt.data1 = (u32)p_data;
                        evt.data2 = p_param->identify;
                        ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                        break;
                    }
 		     if (p_param->query_mode == IPTV_QUERY_MODE_CATGRY)
                        evt.id = IPTV_EVT_GET_PAGE_VDO_FAIL;
 		     else if (p_param->query_mode == IPTV_QUERY_MODE_PINYIN)
                     	  evt.id = IPTV_EVT_GET_SEARCH_VDO_FAIL;
 		     else
                     	  evt.id = IPTV_EVT_GET_PAGE_VDO_FAIL;
                        	
                    evt.id = IPTV_EVT_GET_SEARCH_VDO_FAIL;
                    evt.data2 = p_param->identify;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
            }
            else
            {
                DEBUG(UI_PLAY_API,INFO,"@@@identify=%d vdo_identify_code=%d\n", p_param->identify, vdo_identify_code);
            }
        }
            break;

        case IPTV_VIDEO_INFO_ARRIVAL:
        {
            if((IPTV_CHANNEL_INFO_T *)param ==NULL)
            {  
                evt.id = IPTV_EVT_GET_VDO_INFO_FAIL;
                ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                break;
            }
            p_param = (IPTV_CHANNEL_INFO_T *)param;

            DEBUG(UI_PLAY_API,INFO,"@@@IPTV_VIDEO_INFO_ARRIVAL status=%d description = %d\n", 
				p_param->status,p_param->is_description);
            if (p_param->status == IPTV_DATA_SUCCESS)
            {
                if (p_param->is_description == FALSE)
                {
                    al_iptv_info_url_list_t *p_data = ui_iptv_gen_info_url(p_param);
                    evt.id = IPTV_EVT_NEW_INFO_URL_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                }
                else
                {
                    al_iptv_vdo_info_t *p_data = ui_iptv_gen_vdo_info(p_param);
                    evt.id = IPTV_EVT_NEW_VDO_INFO_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                }
            }
            else
            {
                evt.id = IPTV_EVT_GET_VDO_INFO_FAIL;
                ap_frm_send_evt_to_ui(APP_IPTV, &evt);
            }
        }
            break;

        case IPTV_RECOMMEND_INFO_ARRIVAL:
        {
          if((IPTV_RECMD_INFO_T *)param!=NULL)
          {
            IPTV_RECMD_INFO_T *p_param = (IPTV_RECMD_INFO_T *)param;

            DEBUG(UI_PLAY_API,INFO,"@@@IPTV_RECOMMEND_INFO_ARRIVAL status=%d\n", p_param->status);
            if (p_param->status == IPTV_DATA_SUCCESS)
            {
                al_iptv_recomand_info_t *p_data = ui_iptv_gen_remend_info(p_param);
                if (p_data != NULL)
                {
                    evt.id = IPTV_EVT_NEW_RECMND_INFO_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                    break;
                }
            }
          }
            evt.id = IPTV_EVT_GET_RECMND_INFO_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_FORMAT_TYPE_ARRIVAL:
        {
            al_iptv_source_format_t *p_data;
	     IPTV_VIDEO_FORMAT_T *p_param = (IPTV_VIDEO_FORMAT_T*)param;
            DEBUG(UI_PLAY_API,INFO,"@@@IPTV_FORMAT_TYPE_ARRIVAL param.total[%d]\n", p_param->total);
            p_data = ui_iptv_gen_source_format(p_param);
            if (p_data != NULL)
            {
                evt.id = IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE;
                evt.data1 = (u32)p_data;
                ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                break;
            }
            evt.id = IPTV_EVT_GET_SOURCE_FORMAT_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_PLAY_URL_ARRIVAL:
        {
          if((IPTV_CHANNEL_PLAY_URL_T *)param!= NULL)
          {
            IPTV_CHANNEL_PLAY_URL_T *p_param = (IPTV_CHANNEL_PLAY_URL_T *)param;

            DEBUG(UI_PLAY_API,INFO,"@@@IPTV_PLAY_URL_ARRIVAL status=%d\n", p_param->status);
            if (p_param->status == IPTV_DATA_SUCCESS)
            {
                al_iptv_play_url_list_t *p_data = ui_iptv_gen_play_url(p_param);

                if (p_data != NULL)
                {
                    evt.id = IPTV_EVT_NEW_PLAY_URL_ARRIVE;
                    evt.data1 = (u32)p_data;
                    ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                    break;
                }
            }
          }
            evt.id = IPTV_EVT_GET_PLAY_URL_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        case IPTV_SEARCH_WORD_ARRIVAL:
        {
            if((IPTV_SEARCH_WORD_T *)param != NULL)
            {
             IPTV_SEARCH_WORD_T *p_param = (IPTV_SEARCH_WORD_T *)param;
 
             if (p_param->status == IPTV_DATA_SUCCESS)
             {
		 DEBUG(UI_PLAY_API,INFO,"@@@ IPTV_SEARCH_WORD_ARRIVAL\n");
                 al_iptv_search_word_t *p_data = ui_iptv_gen_search_word(p_param);
 
                 if (p_data != NULL)
                 {
                     evt.id = IPTV_EVT_NEW_SEARCH_WORD_ARRIVE;
                     evt.data1 = (u32)p_data;
                     ap_frm_send_evt_to_ui(APP_IPTV, &evt);
                     break;
                 }
             }
            }
            evt.id = IPTV_EVT_GET_SEARCH_WORD_FAIL;
            ap_frm_send_evt_to_ui(APP_IPTV, &evt);
        }
            break;

        default:
            break;
    }

    return SUCCESS;
}

void ui_iptv_free_msg_data(u16 msg, u32 para1, u32 para2)
{
    if (msg < MSG_IPTV_EVT_BEGIN || msg >= MSG_IPTV_EVT_END)
    {
        return;
    }

    switch(msg)
    {
        case MSG_IPTV_EVT_NEW_SEARCH_WORD_ARRIVE:
        {
            ui_iptv_free_search_word(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_RES_NAME_ARRIVE:
        {
            ui_iptv_free_name_res_list(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_RES_CATGRY_ARRIVE:
        {
            ui_iptv_free_cat_res_list(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_PAGE_VDO_ARRIVE:
        case MSG_IPTV_EVT_NEW_SEARCH_VDO_ARRIVE:
        {
            ui_iptv_free_vdo_list(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_VDO_INFO_ARRIVE:
        {
            ui_iptv_free_vdo_info(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_INFO_URL_ARRIVE:
        {
            ui_iptv_free_info_url(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_RECMND_INFO_ARRIVE:
        {
            ui_iptv_free_remend_info(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE:
        {
            ui_iptv_free_source_format(para1);
        }
            break;
        case MSG_IPTV_EVT_NEW_PLAY_URL_ARRIVE:
        {
            ui_iptv_free_play_url(para1);
        }
            break;
        default:
            break;
    }
}

#if 0
static int iptv_dp_read_flash(void *start, int len)
{
  sys_status_get_iptv_priv_info((u8*)start, len);
  return SUCCESS;
}

static int iptv_dp_write_flash(void *start, int len)
{
  sys_status_set_iptv_priv_info((u8*)start, len);
  sys_status_save();
  return SUCCESS;
}
#endif


inline u16 ui_iptv_get_dp_state(void)
{
    DEBUG(UI_PLAY_API,INFO,"@@@ui_iptv_get_dp_state dp_state=%d\n", g_dp_state);
    return g_dp_state;
}

inline void ui_iptv_set_dp_state(u16 state)
{
    g_dp_state = state;
}

void ui_iptv_dp_init(void)
{
#ifndef WIN32
    DO_CMD_TASK_CONFIG_T config;

    DEBUG(UI_PLAY_API,INFO,"@@@ui_iptv_dp_init\n");
    memset(&config, 0, sizeof(DO_CMD_TASK_CONFIG_T));
    vdo_identify_code = 0;
    config.priority = VOD_DP_PRIORITY;
    config.http_priority = YT_HTTP_PRIORITY;
    config.stack_size = 520*1024;
    config.dp = BSW_IPTV_DP;
    config.sub_dp = SUB_DP_ID;
    config.read_flash = NULL;
    config.write_flash = NULL;
    config.mac = SY_MALLOC(MAC_CHIPID_LEN);
    config.id = SY_MALLOC(MAC_CHIPID_LEN);
    ui_get_stb_mac_addr_chip_id(config.mac, config.id);
    g_pIptvData = IIPTV_initDataProvider(&config);
    MT_ASSERT(g_pIptvData != NULL);
    g_pIptvData->register_event_callback(g_pIptvData, iptv_dp_event_callback);
#endif
    ui_iptv_set_dp_state(IPTV_DP_STATE_INITING);
#ifndef WIN32
    SY_FREE(config.id);
    SY_FREE(config.mac);
#endif
}

void ui_iptv_dp_deinit(void)
{
    if (g_pIptvData != NULL)
    {
#ifndef WIN32
        IIPTV_deinitDataProvider(g_pIptvData);
#endif

    }
}

void ui_iptv_dp_del(void)
{
        IIPTV_delCmdTask(g_pIptvData);
        SY_FREE(g_pIptvData);
        g_pIptvData = NULL;
}

void ui_iptv_register_msg(void)
{
    DEBUG(UI_PLAY_API,INFO,"@@@ui_iptv_register_msg\n");
    fw_register_ap_evtmap(APP_IPTV, ui_iptv_evtmap);
    fw_register_ap_msghost(APP_IPTV, ROOT_ID_IPTV);
    fw_register_ap_msghost(APP_IPTV, ROOT_ID_IPTV_SEARCH);
}

void ui_iptv_unregister_msg(void)
{
    DEBUG(UI_PLAY_API,INFO,"@@@ui_iptv_unregister_msg\n");
    fw_unregister_ap_evtmap(APP_IPTV);
    fw_unregister_ap_msghost(APP_IPTV, ROOT_ID_IPTV);
    fw_unregister_ap_msghost(APP_IPTV, ROOT_ID_IPTV_SEARCH);
}

void ui_iptv_get_res_list(void)
{
    if (g_pIptvData != NULL)
    {
#ifndef WIN32
        IIPTV_getCategoryList(g_pIptvData, 0);
#endif
    }
}

void ui_iptv_get_catgry_list(void)
{
    if (g_pIptvData != NULL)
    {
#ifndef WIN32
        IIPTV_getCategoryTypeList(g_pIptvData, 0, 0);
#endif
    }
}

void ui_iptv_get_video_list(u32 res_id, u16 *cat_name, u8 *key, u32 page_num)
{
    char *inbuf, *outbuf;
    IPTV_UPPAGE_REQ_T req = {0};
    size_t src_len, dest_len;

    if (g_pIptvData != NULL)
    {
        req.cat_id = (int)res_id;

        if (cat_name)
        {
            inbuf = (char *)cat_name;
            outbuf = (char *)req.types;
            src_len = (uni_strlen(cat_name) + 1) * sizeof(u16);
            dest_len = sizeof(req.types);
            iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
        }

        if (key)
        {
            strcpy(req.keys, key);
        }

        req.page_index = (int)page_num;
        req.query_mode = IPTV_QUERY_MODE_CATGRY;
        vdo_identify_code++;
        req.identify = vdo_identify_code;
        req.cb = ui_iptv_vdo_idntfy_cmp;
#ifndef WIN32
        IIPTV_updatePage(g_pIptvData, &req);
#endif
    }
}

void ui_iptv_get_video_list_for_filter(u32 res_id, u16 *cat_name, u8 *key, u32 page_num)
{
    char *inbuf, *outbuf;
    IPTV_UPPAGE_REQ_T req = {0};
    size_t src_len, dest_len;

    if (g_pIptvData != NULL)
    {
        req.cat_id = (int)res_id;

        if (cat_name)
        {
            inbuf = (char *)cat_name;
            outbuf = (char *)req.types;
            src_len = (uni_strlen(cat_name) + 1) * sizeof(u16);
            dest_len = sizeof(req.types);
            iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
        }

        if (key)
        {
            strcpy(req.keys, key);
        }

        req.page_index = (int)page_num;
        req.query_mode = IPTV_QUERY_MODE_SCREENING;
        vdo_identify_code++;
        req.identify = vdo_identify_code;
        req.cb = ui_iptv_vdo_idntfy_cmp;
#ifndef WIN32
        IIPTV_updatePage(g_pIptvData, &req);
#endif
    }
}

void ui_iptv_search_set_res_id(u32 res_id)
{
    search_res_id = res_id;
}

void ui_iptv_search_set_keyword(u16 *keyword)
{
    char *inbuf, *outbuf;
    size_t src_len, dest_len;

    inbuf = (char *)keyword;
    outbuf = (char *)search_keyword;
    src_len = (uni_strlen(keyword) + 1) * sizeof(u16);
    dest_len = sizeof(search_keyword);
    iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
}

void ui_iptv_search_word(void)
{
    u8  searchWord[MAX_KEYWORD_LENGTH*3+1] = {0};

    if (g_pIptvData != NULL)
    {
	DEBUG(UI_PLAY_API,INFO,"search word \n");
        strcpy(searchWord, search_keyword);
#ifndef WIN32
        IIPTV_getSearchWord(g_pIptvData, searchWord);
#endif
    }
}

void ui_iptv_search(u32 page_num, u16 index)
{
    IPTV_UPPAGE_REQ_T req = {0};

    if (g_pIptvData != NULL)
    {
        req.cat_id = (int)search_res_id;
        strcpy(req.pinyin, search_keyword);
        req.page_index = page_num;
	req.searchWordIndex = index;

        req.query_mode = IPTV_QUERY_MODE_PINYIN;
        vdo_identify_code++;
        req.identify = vdo_identify_code;
        req.cb = ui_iptv_vdo_idntfy_cmp;
#ifndef WIN32
        IIPTV_updatePage(g_pIptvData, &req);
#endif
    }
}

void ui_iptv_get_video_info(VDO_ID_t *vdo_id, u8 cat_id)
{
#if 1//SY DELETE

    IPTV_VIDEO_INFO_REQ_T req = {0};
    if (g_pIptvData != NULL)
    {
        req.cat_id = cat_id;
        req.page_index = 1;
        req.id = *vdo_id;
        req.is_description = TRUE;
	 req.page_size = 10;
	 req.page_index = 1;
#ifndef WIN32
        IIPTV_getVideoInfo(g_pIptvData, &req);
#endif
    }
#endif	
}

void ui_iptv_get_info_url(VDO_ID_t * vdo_id, u8 cat_id, u16 *origin, u32 page_num, u32 page_size)
{
#if 1  //SY DELETE

    char *inbuf, *outbuf;
    IPTV_VIDEO_INFO_REQ_T req = {0};
    size_t src_len, dest_len;

    if (g_pIptvData != NULL)
    {
        req.id = *vdo_id;

        inbuf = (char *)origin;
        outbuf = (char *)req.origin;
        src_len = (uni_strlen(origin) + 1) * sizeof(u16);
        dest_len = sizeof(req.origin);
        iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);

        req.cat_id = cat_id;
        req.is_description = FALSE;

        req.page_index = page_num;
        req.page_size = page_size;
#ifndef WIN32
        IIPTV_getVideoInfo(g_pIptvData, &req);
#endif
    }
#endif	
}

void ui_iptv_get_recmnd_info(VDO_ID_t * vdo_id, u32 res_id, u16 *area)
{
    char *inbuf, *outbuf;
    IPTV_RECOMMEND_REQ_T req;
    size_t src_len, dest_len;

    if (g_pIptvData != NULL)
    {
    	 memset(&req,0,sizeof(req));
        req.id = *vdo_id;
        req.cat_id = res_id;
        inbuf = (char *)area;
        outbuf = (char *)req.area;
        src_len = (uni_strlen(area) + 1) * sizeof(u16);
        dest_len = sizeof(req.area);
        iconv(g_cd_utf16le_to_utf8, (char**) &inbuf, &src_len, (char**) &outbuf, &dest_len);
#ifndef WIN32
        IIPTV_getRecommendInfo(g_pIptvData, &req);
#endif
    }
}

void ui_iptv_get_play_format(u8 *tvQid,u8 *vid,int episode_num)
{
    IPTV_VIDEO_URL_REQ_T req;

    if (g_pIptvData != NULL)
    {
        memset((void *)&req, 0, sizeof(IPTV_VIDEO_URL_REQ_T));
	 memcpy(req.tvQid,tvQid,sizeof(req.tvQid));
	 memcpy(req.vid,vid,sizeof(req.vid));
	 req.episode_num = episode_num;
 #ifndef WIN32
        IIPTV_getPlayUrlFormat(g_pIptvData, &req);
#endif
    }
}

void ui_iptv_get_play_url(IPTV_API_FORMAT_T format,u8 *tvQid,u8 *vid,int episode_num)
{
    IPTV_VIDEO_URL_REQ_T req;

    if (g_pIptvData != NULL)
    {
        memset((void *)&req, 0, sizeof(IPTV_VIDEO_URL_REQ_T));
	 memcpy(req.tvQid,tvQid,sizeof(req.tvQid));
	 memcpy(req.vid,vid,sizeof(req.vid));
	 req.episode_num  = episode_num;
        req.format = format;
#ifndef WIN32
        IIPTV_getPlayUrl(g_pIptvData, &req);
#endif
    }
}

void ui_iptv_set_page_size(u16 page_size)
{
    if (g_pIptvData != NULL)
    {
   	  IIPTV_setPageSize(g_pIptvData, (int)page_size);
    }
}

int ui_iptv_vdo_idntfy_cmp(u32 unidntf_code)
{
    return vdo_identify_code - unidntf_code;
}

void ui_iptv_set_flag_to_plid(BOOL flag)   //从播单播放时候，传进来
{
    DEBUG(IPLAY, INFO,"g_plid_flag = %d \n",flag);
    g_plid_flag = flag;
}

BOOL ui_iptv_get_flag_to_plid(void)
{
    DEBUG(IPLAY, INFO,"g_plid_flag = %d \n",g_plid_flag);
    return g_plid_flag;
}

BEGIN_AP_EVTMAP(ui_iptv_evtmap)
CONVERT_EVENT(IPTV_EVT_INIT_SUCCESS, MSG_IPTV_EVT_INIT_SUCCESS)
CONVERT_EVENT(IPTV_EVT_INIT_FAIL, MSG_IPTV_EVT_INIT_FAIL)
CONVERT_EVENT(IPTV_EVT_DEINIT_SUCCESS, MSG_IPTV_EVT_DEINIT_SUCCESS)
CONVERT_EVENT(IPTV_EVT_NEW_RES_NAME_ARRIVE, MSG_IPTV_EVT_NEW_RES_NAME_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_RES_NAME_FAIL, MSG_IPTV_EVT_GET_RES_NAME_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_RES_CATGRY_ARRIVE, MSG_IPTV_EVT_NEW_RES_CATGRY_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_RES_CATGRY_FAIL, MSG_IPTV_EVT_GET_RES_CATGRY_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_PAGE_VDO_ARRIVE, MSG_IPTV_EVT_NEW_PAGE_VDO_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_PAGE_VDO_FAIL, MSG_IPTV_EVT_GET_PAGE_VDO_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_SEARCH_VDO_ARRIVE, MSG_IPTV_EVT_NEW_SEARCH_VDO_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_SEARCH_VDO_FAIL, MSG_IPTV_EVT_GET_SEARCH_VDO_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_VDO_INFO_ARRIVE, MSG_IPTV_EVT_NEW_VDO_INFO_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_VDO_INFO_FAIL, MSG_IPTV_EVT_GET_VDO_INFO_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_INFO_URL_ARRIVE, MSG_IPTV_EVT_NEW_INFO_URL_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_INFO_URL_FAIL, MSG_IPTV_EVT_GET_INFO_URL_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_PLAY_URL_ARRIVE, MSG_IPTV_EVT_NEW_PLAY_URL_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_PLAY_URL_FAIL, MSG_IPTV_EVT_GET_PLAY_URL_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_RECMND_INFO_ARRIVE, MSG_IPTV_EVT_NEW_RECMND_INFO_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_RECMND_INFO_FAIL, MSG_IPTV_EVT_GET_RECMND_INFO_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE, MSG_IPTV_EVT_NEW_SOURCE_FORMAT_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_SOURCE_FORMAT_FAIL, MSG_IPTV_EVT_GET_SOURCE_FORMAT_FAIL)
CONVERT_EVENT(IPTV_EVT_NEW_SEARCH_WORD_ARRIVE, MSG_IPTV_EVT_NEW_SEARCH_WORD_ARRIVE)
CONVERT_EVENT(IPTV_EVT_GET_SEARCH_WORD_FAIL, MSG_IPTV_EVT_GET_SEARCH_WORD_FAIL)
END_AP_EVTMAP(ui_iptv_evtmap)

#endif
