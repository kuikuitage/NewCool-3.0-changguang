#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include "sys_types.h"
#include "sys_define.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
#include "mt_debug.h"
#include "sys_cfg.h"
#include "file_playback_sequence.h"
#ifndef __LINUX__
#include "list.h"
#include "drv_dev.h"
#include "block.h"
#include "fsioctl.h"
#include "ufs.h"
#else

#endif
#include "tinyxml.h"
#include "commonData.h"
#include  "tvLiveDataProvider.h"
#include "download_api.h"

#include "IIPTVDataProvider.h"
#include "sys_dbg.h"
#include "mplayer_sdk.h"
#include "lib_util.h"

#define  PRINT_MALLOC_FAILURE  		OS_PRINTF("malloc mem failure @ func: %s() line:%d\n",__FUNCTION__,__LINE__)
#define  I1B								"        "
#define  I2B							"                "
#define  I3B							"                       "

#define  EPG_VERSION					"1.0.1.0"
#define  GET_IS_FREE					(0)
#define  HAVE_RECOMMAND			(1)

///根据需要定义这个url的格式，这个主要是为了保存参数，让下次函数调用的时候使用
/// 涉及到的函数有， iptv_get_format  / iptv_channel_info
///#define   ZGW_FAKE_PLAYURL_FORMAT	"http://www.iid%d-normal%d-jisu%d-high%d-super%d-aid%d-tid%d-wid%d-hahaha!!.html"

#define  D_WINDOW_MAX_BUFF		(5000)

#define  D_WINDOW_MAX_ALBUM		(200)

static char recommand_utf8[] = {0xe6,0x8e,0xa8,0xe8,0x8d,0x90,0x00};


typedef struct page_s{
	u16  		start;
	u16	 		end;
	u16 			size;
	u16 			pre_remain;
}IPTV_PAGE_POS_T;
typedef struct {
	int 					cat_id;
	char 					keys[IPTV_STRING_SHORT]; // mark whice cat album list
	int					current_req_page_size;
	int					current_req_page_index;
	IPTV_QUERY_MODE_T   	req_mode;

	char 					pinyin[IPTV_STRING_SHORT];
	int                                     searchWordIndex;

	int 					pos_index;//记录每次获取D_WINDOW_MAX_ALBUM 页的起始位置
	IPTV_PAGE_POS_T  	* pos;
	int 					win_left;
	int 					win_right;	
	
	int 					total_album_in_array;
	IQY_AlbumInfo 		* array;

	int  					total_all;
}IPTV_PROGRAM_WINDOW_T;



typedef struct
{
	IPTV_CATEGORY_LIST_T  			* category_list;
	IPTV_CATEGORY_TYPE_ALL_T		* category_type_all;
	IPTV_CHANNEL_LIST_T    			* program_list;
	IPTV_CHANNEL_INFO_T   			* program_info;
	IPTV_RECMD_INFO_T 				* recommend_info;
	IPTV_AREA_INFO_LIST_T 			* area_info_list;
	IPTV_CHANNEL_PLAY_URL_T			* play_url;
	IPTV_VIDEO_FORMAT_T				* format;

	/*iqy operate memory,  alloc when init and free when deinit*/
	IQY_ChannelList					* op_channel_list;
	IQY_AlbumList						* op_album_list;
	IQY_PlayList						* op_play_list;
	IQY_EpisodeList					* op_episode_list;
	IQY_ChannelLabelList 				* op_chLable_list;	
	IPTV_SEARCH_WORD_T					* op_search_word;
	IPTV_PROGRAM_WINDOW_T 			* op_window;

	BOOL                       				is_init;	/*whether this type of data provider has been initilized successfully*/
	BOOL							get_new_info_from_net_every_call_in;

	/*current status*/
	VDO_ID_t						cur_video_id;
	char 								cur_plid[IQY_LEN_CHANNEL_LABEL_ITEM1];
    	int     							cur_chId;                          //频道id
    	int 								cur_episode_num;

}iqy_vod_dp_priv;

static iqy_vod_dp_priv *_ctrb;

#define  RUN_IQY_LOOP		(2)
static void  _get_format(void * hdl,IPTV_VIDEO_FORMAT_T* format,const char *tvQid,const char *vid);

static void _clean_album_window(IPTV_PROGRAM_WINDOW_T *pwin,IPTV_UPPAGE_REQ_T *p_req)
{
	pwin->cat_id = p_req->cat_id;
	memcpy(pwin->keys,p_req->keys,sizeof(pwin->keys));
	pwin->current_req_page_size = p_req->page_size;
	pwin->win_left = 0;
	pwin->win_right = 0;
	pwin->total_album_in_array = 0;
	pwin->pos_index = -1;
	pwin->req_mode = p_req->query_mode;
}
// TODO:获取分类及其tag信息
static  void *  iqy_get_category_list(void * hdl)
{
#define CATEGORY_PAGE_SIZE 			(IQY_CHANNEL_MAX)

	iqy_vod_dp_priv * priv = (iqy_vod_dp_priv *)hdl;
	IPTV_CATEGORY_LIST_T  * category_list;
	IQY_ChannelList *pchList = priv->op_channel_list;
	int i,ii;
	void * ret = NULL;
	int err;
	int count = 0;
	int offset = 0;

	if(priv == NULL)
		return NULL ;	

	MT_ASSERT(priv->category_list != NULL);
	//2 判断是否使用已有数据
	category_list = priv->category_list;	
	if ((category_list->array != NULL) && (!priv->get_new_info_from_net_every_call_in))
		return category_list;
	
	if (category_list->array!= NULL)
	{
		SY_FREE(category_list->array);
		category_list->array= NULL;
	}

	if (category_list->labels != NULL)
	{
		SY_FREE(category_list->labels);
		category_list->labels = NULL;
	}



	/* get iqy channel -> category,need free when get out iqy_get_category_list */
	MT_ASSERT(CATEGORY_PAGE_SIZE <= IQY_CHANNEL_MAX);
	if (IQY_NORMAL != (err = iqy_getChannelList(EPG_VERSION, 1,CATEGORY_PAGE_SIZE,pchList)))
	{
		OS_PRINTF("iqy get channel failure !ret = %d\n",err);
		goto E1_RET;
	}
	if (pchList->resultCode != 0)
	{
		OS_PRINTF("iqy get channel result code err !resultCode = %d\n",pchList->resultCode);
		goto E1_RET;
	}

	DEBUG(IQY,INFO,"total channel: %d\n",pchList->total);
	DEBUG(IQY,INFO,"get   channel: %d\n",pchList->size);

	/* alloc all category buffer and get data*/
	category_list->array 	= (IQY_Channel *)SY_CALLOC(1,sizeof(IQY_Channel) * pchList->total);
	category_list->labels	= (IQY_ChannelLabelList *)SY_CALLOC(1,sizeof(IQY_ChannelLabelList) * pchList->total);
	
	if (category_list->array == NULL)
	{
		PRINT_MALLOC_FAILURE;
		goto E1_RET;
	}

	for (i=0;i<pchList->size;i++)
	{
		category_list->array[count] = pchList->data[i];
		count ++;
	}
	MT_ASSERT(count <= pchList->total);

	/*if total > CATEGORY_PAGE_SIZE,need get next page*/
	for(i=pchList->size;i<pchList->total && pchList->total > CATEGORY_PAGE_SIZE;i+=CATEGORY_PAGE_SIZE)
	{
		int page = 2;
		int ti = 2;
		DEBUG(IQY,INFO,"for i[%d] page[%d]\n",i,page);
		while(ti--)
		{
			if (IQY_NORMAL == iqy_getChannelList(EPG_VERSION, page, CATEGORY_PAGE_SIZE,pchList))
				break;
			
			OS_PRINTF("iqy get channel failure !\n");
			if (ti==0)
				goto E2_RET;
			
		}
		if (pchList->resultCode != 0)
		{
			OS_PRINTF("iqy get channel result code err !resultCode = %d\n",pchList->resultCode);
			goto E2_RET;
		}
		for(ii=0;ii<pchList->size;ii++)
		{
			category_list->array[count] = pchList->data[ii];
			count ++;
		}
		page ++;
	}
	MT_ASSERT(count <= pchList->total);	
	category_list->total 	= count;	
	// get left channel labels, used for types
	for(i=0;i<count;i++)
	{
		int ti = 3;
		while (ti--)
		{
			int err = iqy_getChannelLabelList(GET_IS_FREE,category_list->array[i].recTag, priv->op_chLable_list);
			if (err == IQY_NORMAL)
				break;
			if (err == 12)
				mtos_task_sleep(200);
			DEBUG(IQY,ERR,"get labels failure !return err = %d\n",err);
			if (ti==0)
				goto E2_RET;
		
		}
#if HAVE_RECOMMAND
		if (strlen(category_list->array[i].focus) < 3)
			offset = 0;
		else
			offset = 1;
#endif
		DEBUG(IQY,INFO,"get labels chname[%s] count[%d],focus[%s]\n", 
					category_list->array[i].name,priv->op_chLable_list->size,category_list->array[i].focus);
		MT_ASSERT(offset + priv->op_chLable_list->size < IQY_CHANNEL_LABEL_MAX);
		for (int ii=0;ii<priv->op_chLable_list->size;ii++)
		{
#if HAVE_RECOMMAND
			if (ii == 0 && offset == 1)
			{
				memcpy(category_list->labels[i].labelList[0].name,
								recommand_utf8,//name 
								sizeof(category_list->labels[i].labelList[0].name));
				memcpy(category_list->labels[i].labelList[0].plid,
								category_list->array[i].focus,
								sizeof(category_list->labels[i].labelList[0].plid));
			}
#endif
			memcpy(category_list->labels[i].labelList[ii+offset].name,
								priv->op_chLable_list->labelList[ii].name,
								sizeof(category_list->labels[i].labelList[ii+offset].name));
			memcpy(category_list->labels[i].labelList[ii+offset].plid,
								priv->op_chLable_list->labelList[ii].plid,
								sizeof(category_list->labels[i].labelList[ii].plid));
			if (i+offset >= IQY_CHANNEL_LABEL_MAX)
				break;
		}
		category_list->labels[i].size = priv->op_chLable_list->size + offset;
	}

	category_list->status = IPTV_DATA_SUCCESS;
	ret =  (void *)priv->category_list;
	
#if 0
{
	int ii, iii,utf8;
	P_INFO(IQY,INFO,"Get channel total[%d]\n",category_list->total);
	for (i=0;i<category_list->total;i++)
	{
		P_INFO(IQY,INFO,"channel id[%d], name[%s]",category_list->array[i].channelId,category_list->array[i].name);
		DEBUG(IQY,INFO,"labels total[%d]\n",priv->op_chLable_list->size);
		for(ii=0;ii<category_list->labels[i].size;ii++)
			P_PUT(IQY, INFO,"label name[%s]\n",category_list->labels[i].labelList[ii].name);

	
		P_INFO(IQY,INFO,"tag count[%d]:\n",category_list->array[i].tagSize);
		for (ii=0;ii<category_list->array[i].tagSize;ii++)
		{
			P_INFO(IQY,INFO,I1B"tag name[%s][",category_list->array[i].tags[ii].name);
			for(utf8=0;utf8<(int)strlen(category_list->array[i].name);utf8++)
				P_PUT(IQY,INFO,"%02x,",(unsigned char)category_list->array[i].tags[ii].name[utf8]);
			P_PUT(IQY,INFO,"]sub tag count[%d]\n",category_list->array[i].tags[ii].subTagSize);
			for (iii=0;iii<category_list->array[i].tags[ii].subTagSize;iii++)
			{
				P_INFO(IQY,INFO,I2B"sub tag name[%s]\n",category_list->array[i].tags[ii].subtag[iii].name);
				P_INFO(IQY,INFO,I2B"sub tag value[%s]\n",category_list->array[i].tags[ii].subtag[iii].value);
			}
		}
	}
}
#endif
	
E2_RET:/* if sucess */
	if (ret == NULL)
	{
		SY_FREE(category_list->array);
		category_list->array= NULL;
		SY_FREE(category_list->labels);
		category_list->labels= NULL;		
	}
E1_RET:/*IQY_ChannelList malloced*/
	return ret;
}
static  void * iptv_get_program_page_by_label(void * hdl, IPTV_UPPAGE_REQ_T * p_req)
{
	iqy_vod_dp_priv * priv = (iqy_vod_dp_priv *)hdl;
	IPTV_CHANNEL_LIST_T  * program_list;
	int err = IQY_ERROR_BUSY, i;
	int need_index_left;
	int need_index_right;	
	int count = 0;
	IPTV_PROGRAM_WINDOW_T *pwin = priv->op_window;
  	IQY_PlayList *pplaylist = priv->op_play_list;
	int have_repeated = 0;
	int c;

	
	MT_ASSERT(priv != NULL);
	MT_ASSERT(p_req != NULL);
	MT_ASSERT(pwin  != NULL);
	MT_ASSERT(pplaylist  != NULL);
	
	DEBUG(IQY,INFO,":\n req cat ID[%d]:\n tags[%s] \n",p_req->cat_id,p_req->types);
	DEBUG(IQY,INFO,"page index[%d],size[%d]\n",p_req->page_index,p_req->page_size);
	DEBUG(IQY,INFO,"req_mod[%d],key[%s]\n",p_req->query_mode,p_req->keys);

	//check and change cat album list
	if (pwin->cat_id != p_req->cat_id 
		|| strcmp(pwin->keys, p_req->keys) != 0
		|| pwin->current_req_page_size != p_req->page_size
		|| pwin->req_mode != p_req->query_mode)
	{
		DEBUG(IQY,INFO,"list which old cat id[%d],key[%s],page size[%d],req_mod[%d]\n",
			pwin->cat_id,pwin->keys,pwin->current_req_page_size,pwin->req_mode);
		_clean_album_window(pwin, p_req);
	}
	
	/* check if need free array*/
	program_list = priv->program_list;	
	if (program_list->array != NULL && program_list->page_size < p_req->page_size)
	{
		SY_FREE(program_list->array);
		program_list->array = NULL;
	}
	
	program_list->category_id = p_req->cat_id;
	program_list->page_num = p_req->page_index;
	program_list->page_size = p_req->page_size;
	// alloc new array
	if (program_list->array == NULL)
		program_list->array = (IQY_AlbumInfo*)SY_CALLOC(1,sizeof(program_list->array[0]) * program_list->page_size);
	// check if alloc sucess
	if (program_list->array == NULL)
	{
		PRINT_MALLOC_FAILURE;
		return NULL;
	}
	
	// if there enough albums for req in buffer
	need_index_left 	= (p_req->page_index -1) * p_req->page_size;
	need_index_right    	= need_index_left + p_req->page_size -1;
	DEBUG(IQY,INFO,"need index [%d-%d],win->left[%d],win->right[%d]\n",need_index_left,need_index_right,pwin->win_left,pwin->win_right);
RE_GET:	
	if (have_repeated > 1)
	{
		DEBUG(IQY,ERR,"have_repeated[%d],can't get anay albums\n",have_repeated);
		return NULL;
	}
	if (pwin->win_right > 0
		&& need_index_left >= pwin->win_left 
		&& need_index_left <= pwin->win_right)
	{
		if (need_index_right >= pwin->win_right)
			count  = pwin->win_right -need_index_left + 1;
		else
			count = p_req->page_size;
		DEBUG(IQY,INFO,"remain count[%d]!\n",count);
		
		for (i=0;i<count;i++)
		{
			program_list->array[i] = pwin->array[need_index_left + i - pwin->win_left];
			DEBUG(IQY,INFO,"channel id [%d],name [%s]tvQid[%s]vid[%s] type[%d]!\n",
				program_list->array[i].chnId,program_list->array[i].name,program_list->array[i].tvQid,program_list->array[i].vid,program_list->array[i].type);
		}

		program_list->totalcount = pwin->total_all;
		program_list->pagecount = (program_list->totalcount + program_list->page_size -1)/program_list->page_size;
		program_list->number =count;
		program_list->status = IPTV_DATA_SUCCESS;
		program_list->identify = p_req->identify;
		
		DEBUG(IQY,INFO,"get album total[%d],current get[%d]\n",program_list->totalcount,program_list->number);
		DEBUG(IQY,INFO,"get album total page [%d]\n",program_list->pagecount);
		DEBUG(IQY,INFO,"identify [%d]\n",program_list->identify);
		
		return program_list;
	}
	DEBUG(IQY,INFO,"get play list types[%s],recommand_utf8[%s]\n",p_req->types,recommand_utf8);
	if (strcmp(p_req->types,recommand_utf8) == 0)
	{
		DEBUG(IQY,INFO,"TYPE_RECOMMAND!!\n");
		for (c = 0,err = -1;c<RUN_IQY_LOOP && err != IQY_NORMAL;c++)
			err = iqy_getPlayListById(GET_IS_FREE,p_req->keys, TYPE_RECOMMAND, pplaylist);
	}
	else
	{
		DEBUG(IQY,INFO,"TYPE_PLAYLIST!!\n");	
		for (c=0,err = -1;c<RUN_IQY_LOOP && err != IQY_NORMAL;c++)
			err = iqy_getPlayListById(GET_IS_FREE,p_req->keys, TYPE_PLAYLIST, pplaylist);
	}
	if (err != 0)
	{
		OS_PRINTF("iqy_getAlbumList failure ret = %d\n",err);
		return NULL;
	}

	memcpy(priv->cur_plid,p_req->keys,sizeof(priv->cur_plid));
	DEBUG(IQY,INFO,"get total[%d] size[%d]\n",pplaylist->total,pplaylist->size);
	
	pwin->win_left = 0;
	pwin->win_right = pplaylist->size -1;
	pwin->total_all = pplaylist->total;
	pwin->total_album_in_array = pplaylist->size;
	for(i=0;i<pplaylist->size;i++)
	{
		pwin->array[i] = pplaylist->data[i];
	}
	have_repeated ++;
	goto RE_GET;
	
	return NULL;
}
//tmp
static  void * iptv_get_program_page_by_screening(void * hdl, IPTV_UPPAGE_REQ_T * p_req)
{
	iqy_vod_dp_priv * priv = (iqy_vod_dp_priv *)hdl;
	IPTV_CHANNEL_LIST_T  * program_list;
	int err = IQY_ERROR_BUSY, i;
	IQY_AlbumList * palbumList = priv->op_album_list;
	int need_index_left;
	int need_index_right;	
	int remain;
	int get_index_start = 0;
	int count;
	int pre_remain = 0;
	IPTV_PROGRAM_WINDOW_T *pwin = priv->op_window;
	int c;
	
	MT_ASSERT(priv != NULL);
	MT_ASSERT(p_req != NULL);
	MT_ASSERT(pwin  != NULL);


	// get test list
	{
		static int test = 0;
		int j;
		int t = 0;
		if (test == 0)
		{
			for(i=1;i<0;i++)
			{
				for (c=0,err = -1;c<RUN_IQY_LOOP && err != IQY_NORMAL;c++)
				{
					err = iqy_getAlbumList(p_req->cat_id,-1, 
						p_req->keys, 
						i, //page index
						IQY_ALBUM_LIST_MAX, // page size
						palbumList);
				}
				
				if (err != 0)
					OS_PRINTF("iqy_getAlbumList failure ret = %d\n",err);
				P_PUT(IQY, INFO, "get size[%d]\n",palbumList->size);
				for(j=0;j<palbumList->size;j++)
				{
					if (t%p_req->page_size == 0)
						P_PUT(IQY, INFO, "t[%d]name[%s]\n\n",t,palbumList->data[j].name);
					t++;
				}
			}
		}
		test++;
	}
	
	DEBUG(IQY,INFO,":\n req cat ID[%d]:\n tags[%s] \n",p_req->cat_id,p_req->types);
	DEBUG(IQY,INFO,"page index[%d],size[%d]\n",p_req->page_index,p_req->page_size);
	DEBUG(IQY,INFO,"key[%s]\n",p_req->keys);

	//check and change cat album list
	if (pwin->cat_id != p_req->cat_id 
		|| memcmp(pwin->keys, p_req->keys,sizeof(p_req->keys)) != 0
		|| pwin->current_req_page_size != p_req->page_size
		|| pwin->req_mode != p_req->query_mode)
	{
		DEBUG(IQY,INFO,"list which old cat id[%d],key[%s],page size[%d]\n",pwin->cat_id,pwin->keys,pwin->current_req_page_size);
		_clean_album_window(pwin, p_req);
	}
	
	/* check if need free array*/
	program_list = priv->program_list;	
	if (program_list->array != NULL && program_list->page_size < p_req->page_size)
	{
		SY_FREE(program_list->array);
		program_list->array = NULL;
	}
	
	program_list->category_id = p_req->cat_id;
	program_list->page_num = p_req->page_index;
	program_list->page_size = p_req->page_size;
	// alloc new array
	if (program_list->array == NULL)
		program_list->array = (IQY_AlbumInfo*)SY_CALLOC(1,sizeof(program_list->array[0]) * program_list->page_size);
	// check if alloc sucess
	if (program_list->array == NULL)
	{
		PRINT_MALLOC_FAILURE;
		return NULL;
	}
	// if there enough albums for req in buffer
	need_index_left 	= (p_req->page_index -1) * p_req->page_size;
	need_index_right    	= need_index_left + p_req->page_size -1;
	DEBUG(IQY,INFO,"need index [%d-%d],win->left[%d],win->right[%d]\n",need_index_left,need_index_right,pwin->win_left,pwin->win_right);
	if (pwin->win_right > 0 
		&& need_index_left >= pwin->win_left 
		&& need_index_right <= pwin->win_right)
	{
		DEBUG(IQY,INFO,"there is enougth albums for req!\n");
		for (i=0;i<p_req->page_size;i++)
		{
			program_list->array[i] = pwin->array[need_index_left + i - pwin->win_left];
			DEBUG(IQY,INFO,"name [%s]!\n",program_list->array[i].name);
		}

		program_list->totalcount = pwin->total_all;
		program_list->pagecount = (program_list->totalcount + program_list->page_size -1)/program_list->page_size;
		program_list->number =p_req->page_size;
		program_list->status = IPTV_DATA_SUCCESS;
		program_list->identify = p_req->identify;
		DEBUG(IQY,INFO,"get album total[%d],current get[%d]\n",program_list->totalcount,program_list->number);
		DEBUG(IQY,INFO,"get album total page [%d]\n",program_list->pagecount);	
		DEBUG(IQY,INFO,"identify [%d]\n",program_list->identify);
		return program_list;
	}
	else if (need_index_left < pwin->win_left)
	{
		DEBUG(IQY,INFO,"get back!!\n");
		
		pwin->pos_index --;
		get_index_start 	= pwin->pos[pwin->pos_index].start;
		// % has remain
		pwin->win_left	-= pwin->pos[pwin->pos_index].size - (pwin->pos[pwin->pos_index].size % pwin->current_req_page_size);
		pwin->win_right 	= 0;
		pwin->total_album_in_array = 0;
		pre_remain = pwin->pos[pwin->pos_index].pre_remain;
	}
	else if (need_index_right > pwin->win_right)
	{
		// TODO: copy remain
		remain = pwin->total_album_in_array % pwin->current_req_page_size;
		memcpy(&pwin->array[0],&pwin->array[pwin->total_album_in_array - remain],remain * sizeof(pwin->array[0]));
		DEBUG(IQY,INFO,"remain[%d]\n",remain);
		for (i=0;i<remain;i++)
			P_PUT(IQY,INFO,"remain name[%s]\n",pwin->array[pwin->total_album_in_array - remain + i].name);
		for (i=0;i<remain;i++)
			P_PUT(IQY,INFO,"remain copy name[%s]\n",pwin->array[i].name);	

		if (pwin->pos_index == -1)
		{
			pwin->pos_index = 0;		
			pwin->pos[pwin->pos_index].start 		= 1;
			pwin->pos[pwin->pos_index].size 		= 0;
			pwin->pos[pwin->pos_index].end 			= 0;	
			pwin->win_left 						= 0;
			pwin->pos[pwin->pos_index].pre_remain 	= 0;
			get_index_start 						=pwin->pos[pwin->pos_index].start;	
			
		}
		else
		{
			pwin->pos_index ++;
			pwin->pos[pwin->pos_index].size 	= 0;
			pwin->pos[pwin->pos_index].end 		= 0;
			pwin->win_left 					= pwin->win_right - remain + 1;
			get_index_start = pwin->pos[pwin->pos_index].start;
			pwin->pos[pwin->pos_index].pre_remain 	= remain;
		}
		pwin->total_album_in_array = remain;	
	}else {;}

	// get albums	
	count = pwin->total_album_in_array;
	
	DEBUG(IQY,INFO,"win pos_index[%d],pos.start[%d],pos.end[%d]\n",
		pwin->pos_index,pwin->pos[pwin->pos_index].start,pwin->pos[pwin->pos_index].end);	
	DEBUG(IQY,INFO,"pre_remain[%d],win left[%d],win right[%d]\n",
		pwin->pos[pwin->pos_index].pre_remain,pwin->win_left,pwin->win_right);		

	int limit = 10;//最多获取10页
	for (i=count;i<D_WINDOW_MAX_ALBUM;)
	{
		if (limit -- < 0)
			break;
		//前面遗留的可能需要多次才能读取完....
		if (pre_remain > 0)
		{
			int have_get = 0;
			int ii;
			int page_no = get_index_start -1;
			MT_ASSERT(count == 0);
			while (pre_remain > 0)
			{
				for (c=0,err = -1;c<RUN_IQY_LOOP && err != IQY_NORMAL;c++)
				{
					err = iqy_getAlbumList(p_req->cat_id,-1, 
								p_req->keys, 
								page_no, //page index
								IQY_ALBUM_LIST_MAX, // page size
								palbumList);
				}
				if (err != 0)
				{
					DEBUG(IQY,ERR,"iqy_getAlbumList failure ret = %d\n",err);
					return NULL;
				}
				DEBUG(IQY,INFO,"pre_remain[%d],get[%d],have_get[%d]\n",pre_remain,palbumList->size,have_get);

				if (pre_remain < palbumList->size)
				{
					int idx = 0;
					for (ii=palbumList->size -pre_remain;ii<palbumList->size;ii++)
					{
						pwin->array[idx] = palbumList->data[ii];
						idx ++;
						have_get ++;
						DEBUG(IQY,INFO,"# pre name[%s]\n",palbumList->data[ii].name);
					}
					pre_remain -= idx;
				}
				else
				{
					for (ii=0;ii<palbumList->size;ii++)
					{
						pwin->array[pre_remain -palbumList->size + i] = palbumList->data[ii];
						have_get ++;
						DEBUG(IQY,INFO,"pre name[%s]\n",palbumList->data[ii].name);
					}
					pre_remain -= palbumList->size;
				}
				page_no --;
			}
			i =  count = have_get;
			DEBUG(IQY,INFO,"have_get [%d] pre_remain[%d]\n",have_get,pre_remain);

		}
		DEBUG(IQY,INFO,":\n req cat ID[%d]:\n tags[%s] \n",p_req->cat_id,p_req->types);
		DEBUG(IQY,INFO,"page index[%d],size[%d]\n",p_req->page_index,p_req->page_size);
		DEBUG(IQY,INFO,"key[%s]\n",p_req->keys);		
		DEBUG(IQY,INFO,"get_index_start[%d]\n",get_index_start);
		for (c=0,err = -1;c<RUN_IQY_LOOP && err != IQY_NORMAL;c++)
		{
			err = iqy_getAlbumList(p_req->cat_id,-1, 
							p_req->keys, 
							get_index_start, //page index
							IQY_ALBUM_LIST_MAX, // page size
							palbumList);
		}
		if (err != 0)
		{
			DEBUG(IQY,ERR,"iqy_getAlbumList failure ret = %d\n",err);
			return NULL;
		}
		pwin->total_all 	= palbumList->total;

		DEBUG(IQY,INFO,"i [%d] limit[%d], get_index_start[%d],get size[%d],i+size[%d]\n",
							i,limit,get_index_start,palbumList->size,i+palbumList->size);
		i += palbumList->size;
		if (i > D_WINDOW_MAX_ALBUM) // 丢掉最后一个
		{
			break;
		}
		
		for (int ii=0;ii<palbumList->size;ii++)
		{
			pwin->array[count] = palbumList->data[ii];
			count ++;
		}
		get_index_start ++;
	}
	if (count)
	{
		pwin->win_right = pwin->win_left + count -1;
		DEBUG(IQY,INFO,"win left1[%d],win right1[%d]\n",
			pwin->win_left,pwin->win_right);				
	}

	pwin->total_album_in_array = count;
	pwin->pos[pwin->pos_index].end = get_index_start;
	pwin->pos[pwin->pos_index].size = count;
	pwin->pos[pwin->pos_index +1].start = get_index_start;

	
	// if there enough albums for req in buffer
	DEBUG(IQY,INFO,"need index[%d-%d],win->left[%d],win->right[%d]\n",need_index_left,need_index_right,pwin->win_left,pwin->win_right);
	if (need_index_left >= pwin->win_left && need_index_right <= pwin->win_right)
	{
		DEBUG(IQY,INFO,"there is enougth albums for req!\n");
		for (i=0;i<p_req->page_size;i++)
		{
			program_list->array[i] = pwin->array[need_index_left + i - pwin->win_left];
			DEBUG(IQY,INFO,"name [%s]!\n",program_list->array[i].name);
		}
		program_list->totalcount = pwin->total_all;
		program_list->pagecount = (program_list->totalcount + program_list->page_size -1)/program_list->page_size;
		program_list->number = p_req->page_size;		
		program_list->status = IPTV_DATA_SUCCESS;
		program_list->identify = p_req->identify;
		DEBUG(IQY,INFO,"get album total[%d],current get[%d]\n",program_list->totalcount,program_list->number);
		DEBUG(IQY,INFO,"get album total page [%d]\n",program_list->pagecount);	
		DEBUG(IQY,INFO,"identify [%d]\n",program_list->identify);		
		return program_list;
	}
	else
	{
		MT_ASSERT(pwin->total_album_in_array < pwin->current_req_page_size);
		MT_ASSERT(pwin->current_req_page_size <= p_req->page_size);
		for (i=0;i<pwin->total_album_in_array && i < pwin->current_req_page_size;i++)
		{
			program_list->array[i] = pwin->array[need_index_left + i - pwin->win_left];
			DEBUG(IQY,INFO,"name [%s]!\n",program_list->array[i].name);
		}
		program_list->totalcount = pwin->total_all;
		program_list->pagecount = (program_list->totalcount + program_list->page_size -1)/program_list->page_size;
		program_list->number = pwin->total_album_in_array;			
		program_list->status = IPTV_DATA_SUCCESS;
		program_list->identify = p_req->identify;
		DEBUG(IQY,INFO,"get album total[%d],current get[%d]\n",program_list->totalcount,program_list->number);
		DEBUG(IQY,INFO,"get album total page [%d]\n",program_list->pagecount);
		DEBUG(IQY,INFO,"identify [%d]\n",program_list->identify);		
		return program_list;
	}
	DEBUG(IQY,INFO,"run  failure!\n");
	return NULL;
}


static  void * iptv_get_program_page_by_search(void * hdl, IPTV_UPPAGE_REQ_T * p_req)
{
	iqy_vod_dp_priv * priv = (iqy_vod_dp_priv *)hdl;
	IPTV_CHANNEL_LIST_T  * program_list;
	int err = IQY_ERROR_BUSY;
	IPTV_PROGRAM_WINDOW_T *pwin = priv->op_window;
	int need_index_left;
	int need_index_right;	
	int have_repeated = 0;
	int count = 0;
	int i,limit,searchPage;
	int c;

	MT_ASSERT(priv != NULL);
	MT_ASSERT(p_req != NULL);
	
	DEBUG(IQY,INFO,":\n req cat ID[%d]:\n tags[%s] \n",p_req->cat_id,p_req->types);
	DEBUG(IQY,INFO,"page index[%d],size[%d]\n",p_req->page_index,p_req->page_size);
	DEBUG(IQY,INFO,"wordIndex[%d] word[%s] pinyin[%s]\n",p_req->searchWordIndex,priv->op_search_word->search->wordList[p_req->searchWordIndex].word,p_req->pinyin);

	//clear windos
	if (pwin->cat_id != p_req->cat_id 
		|| memcmp(pwin->pinyin, p_req->pinyin,sizeof(p_req->pinyin)) != 0
		|| pwin->current_req_page_size != p_req->page_size
		|| pwin->searchWordIndex != p_req->searchWordIndex
		|| pwin->req_mode != p_req->query_mode)
	{
		DEBUG(IQY,INFO,"list which old cat id[%d],key[%s],page size[%d]\n",pwin->cat_id,pwin->keys,pwin->current_req_page_size);
		pwin->searchWordIndex = p_req->searchWordIndex;
		memcpy(pwin->pinyin,p_req->pinyin,sizeof(pwin->pinyin));
		_clean_album_window(pwin, p_req);
	}

	/* check if need free array*/
	program_list = priv->program_list;	
	if (program_list->array != NULL && program_list->page_size < p_req->page_size)
	{
		SY_FREE(program_list->array);
		program_list->array = NULL;
	}
	
	program_list->category_id = p_req->cat_id;
	program_list->page_num = p_req->page_index;
	program_list->page_size = p_req->page_size;
	program_list->identify = p_req->identify;
	program_list->status = IPTV_DATA_FAIL;
	// alloc new array
	if (program_list->array == NULL)
		program_list->array = (IQY_AlbumInfo*)SY_CALLOC(1,sizeof(program_list->array[0]) * program_list->page_size);
	// check if alloc sucess
	if (program_list->array == NULL)
	{
		PRINT_MALLOC_FAILURE;
		return program_list;
	}

	// if there enough albums for req in buffer
	need_index_left 	= (p_req->page_index -1) * p_req->page_size;
	need_index_right    	= need_index_left + p_req->page_size -1;
	DEBUG(IQY,INFO,"need index [%d-%d],win->left[%d],win->right[%d]\n",need_index_left,need_index_right,pwin->win_left,pwin->win_right);
RE_GET:	
	if (have_repeated > 1)
	{
		DEBUG(IQY,ERR,"have_repeated[%d],can't get anay albums\n",have_repeated);
		return program_list;
	}
	if (pwin->win_right > 0
		&& need_index_left >= pwin->win_left 
		&& need_index_left <= pwin->win_right)
	{
		if (need_index_right >= pwin->win_right)
			count  = pwin->win_right -need_index_left + 1;
		else
			count = p_req->page_size;
		DEBUG(IQY,INFO,"remain count[%d]!\n",count);
		MT_ASSERT(count <= p_req->page_size);
		for (i=0;i<count;i++)
		{
			program_list->array[i] = pwin->array[need_index_left + i - pwin->win_left];
			DEBUG(IQY,INFO,"name [%s]!\n",program_list->array[i].name);
		}

		program_list->totalcount = pwin->total_all;
		program_list->pagecount = (program_list->totalcount + program_list->page_size -1)/program_list->page_size;
		program_list->number =count;
		program_list->status = IPTV_DATA_SUCCESS;
		program_list->identify = p_req->identify;
		
		DEBUG(IQY,INFO,"get album total[%d],current get[%d]\n",program_list->totalcount,program_list->number);
		DEBUG(IQY,INFO,"get album total page [%d]\n",program_list->pagecount);
		DEBUG(IQY,INFO,"identify [%d]\n",program_list->identify);
		
		return program_list;
	}

	count = 0;
	pwin->win_left = 0;
	limit = 0;
	searchPage = 1;
	while(count < D_WINDOW_MAX_ALBUM && limit < 3)
	{
		for (c=0,err = -1;c<RUN_IQY_LOOP && err != IQY_NORMAL;c++)
		{
			err = iqy_albumSearch(priv->op_search_word->search->wordList[p_req->searchWordIndex].word,
						0, searchPage,IQY_ALBUM_LIST_MAX, priv->op_album_list);
		}
		
		if (err != IQY_NORMAL)
		{
			DEBUG(IQY,ERR,"iqy_albumSearch failure! return = %d",err);	
			return program_list;
		}
		DEBUG(IQY,INFO,"search album total[%d],size[%d]\n",priv->op_album_list->total,priv->op_album_list->size);
		for (int ii=0;ii<priv->op_album_list->size;ii++)
		{
			P_PUT(IQY,INFO,"album name[%s][%d]\n",priv->op_album_list->data[ii].name,strlen(priv->op_album_list->data[ii].name));
		}

		if (priv->op_album_list->size == 0)
			limit ++;
		if (count + priv->op_album_list->size  > D_WINDOW_MAX_ALBUM)
			priv->op_album_list->size = D_WINDOW_MAX_ALBUM - count;
		
		for(i=0;i<priv->op_album_list->size;i++)
		{
			pwin->array[i+count] = priv->op_album_list->data[i];
		}
		searchPage++;
		count += priv->op_album_list->size;
	}
	pwin->win_right = count -1;
	pwin->total_all = count;
	pwin->total_album_in_array = count;	
	have_repeated ++;
	goto RE_GET;

	return program_list;
}

static  void * iptv_get_search_word_by_pinyin(void * hdl, void * pinYin)
{
	iqy_vod_dp_priv * priv = (iqy_vod_dp_priv *)hdl;
	IPTV_SEARCH_WORD_T  *search_word = NULL;
	int err = IQY_ERROR_BUSY;
	int c;

	MT_ASSERT(priv != NULL);
	MT_ASSERT(pinYin != NULL);

	DEBUG(IQY,INFO,"pinyin[%s]\n",pinYin);
	search_word = priv->op_search_word;
	if(search_word->search == NULL)
		search_word->search = (IQY_SearchWord *)SY_CALLOC(1, sizeof(IQY_SearchWord));
	if (search_word->search == NULL)
	{
		PRINT_MALLOC_FAILURE;
		return NULL;
	}
	memset(priv->op_search_word->search, 0, sizeof(IQY_SearchWord));
	for (c=0,err = -1;c<RUN_IQY_LOOP && err != IQY_NORMAL;c++)
		err = iqy_getRealTimeSearchWord((char *)pinYin,priv->op_search_word->search);
	
	if (err != IQY_NORMAL)
	{
		DEBUG(IQY,ERR,"iqy_getRealTimeSearchWord failure! return = %d",err);
		return NULL;
	}

	P_PUT(IQY,INFO,"site[%s]\n",priv->op_search_word->search->site);
	P_PUT(IQY,INFO,"eventId[%s]\n",priv->op_search_word->search->eventId);
	DEBUG(IQY,INFO,"search word total[%d]\n",priv->op_search_word->search->wordSize);
	for (int ii=0;ii<priv->op_search_word->search->wordSize;ii++)
	{
		P_PUT(IQY,INFO,"word[%s]\n",priv->op_search_word->search->wordList[ii].word);
	}

	search_word->status = IPTV_DATA_SUCCESS;

	OS_PRINTF("[debug] %s %d \n",__FUNCTION__,__LINE__);
	return search_word;
}


static  void * iptv_get_new_program_page(void * hdl, IPTV_UPPAGE_REQ_T * p_req)
{
	iqy_vod_dp_priv * priv = (iqy_vod_dp_priv *)hdl;
	priv->cur_chId = p_req->cat_id;
	if (p_req->query_mode == IPTV_QUERY_MODE_CATGRY)
		return iptv_get_program_page_by_label(hdl, p_req);
	else if (p_req->query_mode == IPTV_QUERY_MODE_SCREENING)
		return iptv_get_program_page_by_screening(hdl, p_req);
	else
		return iptv_get_program_page_by_search(hdl, p_req);
}


// TODO: 获取节目页
static  void * iptv_get_program_page(void * hdl, IPTV_UPPAGE_REQ_T * p_req)
{
	iqy_vod_dp_priv * priv = (iqy_vod_dp_priv *)hdl;
	IPTV_CHANNEL_LIST_T  * program_list;
	int err, i;
	IQY_AlbumList * palbumList = priv->op_album_list;
	void *ret_value = NULL;
	int c;
	
	MT_ASSERT(priv != NULL);
	MT_ASSERT(p_req != NULL);
	
	DEBUG(IQY,INFO,":\n req cat ID[%d]:\n tags[%s] \n",p_req->cat_id,p_req->types);
	DEBUG(IQY,INFO,"page index[%d],size[%d]\n",p_req->page_index,p_req->page_size);
	DEBUG(IQY,INFO,"get type[%s]key[%s]\n",
		p_req->query_mode == IPTV_QUERY_MODE_CATGRY?"IPTV_QUERY_MODE_CATGRY":"IPTV_QUERY_MODE_PINYIN",
		p_req->keys);
	
	/* check if need free array*/
	if (priv->program_list->array != NULL && priv->program_list->page_size < p_req->page_size)
	{
		SY_FREE(priv->program_list->array);
		priv->program_list->array = NULL;
	}

	program_list = priv->program_list;
	for (c=0,err = -1;c<RUN_IQY_LOOP && err != IQY_NORMAL;c++)
		err = iqy_getAlbumList(p_req->cat_id,-1, p_req->keys, p_req->page_index, p_req->page_size, palbumList);
	if (err != 0)
	{
		OS_PRINTF("iqy_getAlbumList failure ret = %d\n",err);
		goto E1_RET;
	}

	DEBUG(IQY,INFO,"get album total[%d],current get[%d]\n",palbumList->total,palbumList->size);
	program_list->category_id = p_req->cat_id;
	program_list->page_num = p_req->page_index;
	program_list->page_size = p_req->page_size;
	program_list->totalcount = palbumList->total;
	program_list->pagecount = (program_list->totalcount + program_list->page_size -1)/program_list->page_size;
	program_list->number = palbumList->size;

	DEBUG(IQY,INFO,"get album total page [%d]\n",program_list->pagecount);
	if (program_list->array == NULL)
		program_list->array = (IQY_AlbumInfo*)SY_CALLOC(1,sizeof(program_list->array[0]) * program_list->page_size);
	
	if (program_list->array == NULL)
	{
		PRINT_MALLOC_FAILURE;
		goto E1_RET;
	}

	for(i=0;i<program_list->number;i++)
	{
		program_list->array[i] = palbumList->data[i];
		DEBUG(IQY,INFO," get album name [%s]\n",program_list->array[i].name);
	}
	
	program_list->status = IPTV_DATA_SUCCESS;
	program_list->identify = p_req->identify;
	ret_value = program_list;
	
E1_RET:
	return ret_value;
}

/*
* IN:
*	tid--->cat_id
*	wid--->
*/
static void * iqy_get_program_detail_info(void * hdl, IPTV_VIDEO_INFO_REQ_T * p_req)
{
	iqy_vod_dp_priv * priv = (iqy_vod_dp_priv *)hdl;
	IPTV_CHANNEL_INFO_T   * program_info = priv ->program_info;
	int err,i;
	int first_get;
	int videoType = 0;
	IQY_EpisodeList *pEpisodeLst = priv->op_episode_list;
	static IQY_AlbumInfo album_buf;
	int c;
	
	MT_ASSERT(priv != NULL);
	MT_ASSERT(priv->is_init !=  FALSE);
	MT_ASSERT(pEpisodeLst != NULL);

	DEBUG(IQY,INFO,"req cat id[%x] qpid[%s]\n",p_req->cat_id,p_req->id.qpId);
	DEBUG(IQY,INFO,"req page index[%d], page size[%d]\n",p_req->page_index,p_req->page_size);	

	priv->cur_video_id = p_req->id;
	program_info->album = NULL;
	if (program_info->album == NULL) // get video info from recommend,it not exist in video list
	{
		DEBUG(IQY,ERR," cant find source album!\n");
		//get by id
		if (p_req->id.type == 0)
		{
			for (c=0,err = -1;c<RUN_IQY_LOOP && err != IQY_NORMAL;c++)		
				err = iqy_getAlbumInfo(p_req->id.tvQid,&album_buf);
		}
		else
		{
			for (c=0,err = -1;c<RUN_IQY_LOOP && err != IQY_NORMAL;c++)				
				err = iqy_getAlbumInfo(p_req->id.qpId,&album_buf);
		}
		if (err != IQY_NORMAL)
		{
			OS_PRINTF("iqy_getAlbumInfo failure! errno = %d\n",err);
			return NULL;
		}		
		program_info->album = &album_buf;
	}
	
	_get_format(hdl, &program_info->format, p_req->id.tvQid,program_info->album->vid);
	program_info->is_description = p_req->is_description;
	if (p_req->is_description) // just get description
	{
		program_info->status = IPTV_DATA_SUCCESS;
		return program_info;
	}

	if (program_info->episodes != NULL)
	{
		SY_FREE(program_info->episodes);
		program_info->episodes = NULL;
	}


	if (program_info->album->type == IQY_TYPE_MOVE)
	{
		DEBUG(IQY,INFO,"album type == IQY_TYPE_MOVE!just return album\n");
		program_info->status = IPTV_DATA_SUCCESS;

		/* get count*/
		program_info->episode_total 		= 0;
		program_info->this_page_index 	= p_req->page_index;
		program_info->this_page_size     	= 0;
#if 0		
		if (program_info->episodes == NULL)
		{
			program_info->episodes =  (IQY_Episode *)SY_CALLOC(1,sizeof(program_info->episodes[0]) * program_info->episode_total);
			if (program_info->episodes == NULL)
			{
				PRINT_MALLOC_FAILURE;
				return NULL;
			}
		}
		memcpy(program_info->episodes[0].tvQid ,program_info->album->tvQid,sizeof(program_info->episodes[0].tvQid));
		memcpy(program_info->episodes[0].name,program_info->album->name,sizeof(program_info->episodes[0].name));
#endif
		return program_info;		
	}
		
	first_get 	= (p_req->page_size <= IQY_EPISODE_MAX &&  p_req->page_size != 0 )? p_req->page_size : IQY_EPISODE_MAX;
	
	for (c=0,err = -1;c<RUN_IQY_LOOP && err != IQY_NORMAL;c++)		
		err = iqy_getAlbumVideo(p_req->id.qpId, p_req->id.sourceCode ,videoType , p_req->page_index, first_get, pEpisodeLst);
	if (err != IQY_NORMAL && pEpisodeLst->resultCode != 0)
	{
		OS_PRINTF("iqy_getAlbumVideo failure! errno[%d]resultcode[%d]\n",err,pEpisodeLst->resultCode );
		return NULL;
	}

	if (pEpisodeLst->total > 0)
		_get_format(hdl, &program_info->format, p_req->id.tvQid,pEpisodeLst->data[0].vid);

	DEBUG(IQY,INFO,"first get espiode ret[%d], total[%d],size[%d]\n",pEpisodeLst->resultCode,  pEpisodeLst->total,pEpisodeLst->size);

	/* get count*/
	program_info->episode_total 		= pEpisodeLst->total;
	program_info->this_page_index 	= p_req->page_index;
	program_info->this_page_size     	= pEpisodeLst->size;
	
	if (program_info->episodes == NULL)
	{
		program_info->episodes =  (IQY_Episode *)SY_CALLOC(1,sizeof(program_info->episodes[0]) * pEpisodeLst->size);
		if (program_info->episodes == NULL)
		{
			PRINT_MALLOC_FAILURE;
			return NULL;
		}
	}

	for (i=0;i<pEpisodeLst->size;i++)
	{
		program_info->episodes[i] = pEpisodeLst->data[i];
		DEBUG(IQY,INFO,"episodes[%d] name[%s]\n",i,program_info->episodes[i].name);
	}
	program_info->status = IPTV_DATA_SUCCESS;
	return (void *)program_info;
}



static void *iqy_get_url(void * hdl, IPTV_VIDEO_URL_REQ_T * p_req)
{
	iqy_vod_dp_priv * priv = (iqy_vod_dp_priv *)hdl;
	IPTV_CHANNEL_PLAY_URL_T *play_url = priv->play_url;
	DEBUG(IQY,INFO,"info url tvQid[%s] vid[%s] format[%#x]\n",p_req->tvQid,p_req->vid,p_req->format);

	priv->cur_episode_num  =  p_req->episode_num;
	// free array
	if (play_url->playurls != NULL)
	{
		SY_FREE(play_url->playurls);
		play_url->playurls  = NULL;
	}
	play_url->format =p_req->format;
	play_url->identify = p_req->identify;
	play_url->number = 1;
	play_url->status = IPTV_DATA_SUCCESS;
	if (play_url->playurls == NULL)
	{
		play_url->playurls = (IPTV_CHANNEL_URL_T*)SY_CALLOC(1, sizeof(*play_url->playurls) * play_url->number );
		if (play_url->playurls == NULL)
		{
			DEBUG(IQY,INFO,"malloc failure!\n");
			return NULL;
		}
	}
	for (int i=0;i<play_url->number;i++)
	{
		memcpy(play_url->playurls[i].tvQid,p_req->tvQid,sizeof(play_url->playurls[0].tvQid));
		memcpy(play_url->playurls[i].vid,p_req->vid,sizeof(play_url->playurls[0].vid));
	}
	
	return play_url;
}


static void  _get_format(void * hdl,IPTV_VIDEO_FORMAT_T* format,const char *tvQid,const char *vid)
{
	int err;
	IQY_VdList vlist;
	int i,count = 0;
	int c;
	
	for (c=0,err = -1;c<RUN_IQY_LOOP && err != IQY_NORMAL;c++)		
		err = iqy_getVdList((char*)tvQid, (char*)vid, &vlist);
	if (err != IQY_NORMAL)
	{
		DEBUG(IQY,ERR,"iqy_getVdList failure!err[%d]\n",err);
		format->total = 1;
		format->format[0] = TYPE_VD_300;
		return;
	}
	for (i=0;i<vlist.size && i<IPTV_MAX_FORMAT_COUNT;i++)
	{
		switch(vlist.vdlist[i])
		{
			case TYPE_VD_300:       //1: 流畅
				DEBUG(IQY,INFO,"TYPE_VD_300\n");
				format->format[count++] = TYPE_VD_300;
				
				break;
			case TYPE_VD_600:                        //2: 高清
				DEBUG(IQY,INFO,"TYPE_VD_600\n");
				format->format[count++] = TYPE_VD_600;
				
				break;			
			case TYPE_VD_1000:                        //3: 超清
				DEBUG(IQY,INFO,"TYPE_VD_1000\n");
				format->format[count++] = TYPE_VD_1000;
				
				break;			
			case TYPE_VD_720P:                        //4: 720p
				DEBUG(IQY,INFO,"TYPE_VD_720P\n");
				format->format[count++] = TYPE_VD_720P;
				
				break;			
			case TYPE_VD_1080P:                       //5: 1080p
				DEBUG(IQY,INFO,"TYPE_VD_1080P\n");
				format->format[count++] = TYPE_VD_1080P;
				
				break;			
			case TYPE_VD_FAST_DRM:                    //6: 极速DRM
				DEBUG(IQY,INFO,"TYPE_VD_FAST_DRM\n");
				format->format[count++] = TYPE_VD_FAST_DRM;
				
				break;			
			case TYPE_VD_AUTO_DRM:                    //7: 流畅DRM
				DEBUG(IQY,INFO,"TYPE_VD_AUTO_DRM\n");
				format->format[count++] = TYPE_VD_AUTO_DRM;
				
				break;			
			case TYPE_VD_HD_DRM:                  //8: 高清DRM
				DEBUG(IQY,INFO,"TYPE_VD_HD_DRM\n");
				format->format[count++] = TYPE_VD_HD_DRM;
				
				break;			
			case TYPE_VD_SUPER_HD_DRM:               //9: 超清DRM
				DEBUG(IQY,INFO,"TYPE_VD_SUPER_HD_DRM\n");
				format->format[count++] = TYPE_VD_SUPER_HD_DRM;
				
				break;			
			case TYPE_VD_FAST:        //96: 极速
				DEBUG(IQY,INFO,"TYPE_VD_FAST\n");
				format->format[count++] = TYPE_VD_FAST;
				
				break;			
			default:
				break;
		}
	}
	format->total = count;
	DEBUG(IQY,INFO,"format->total[%d]\n",format->total);
	return;
}

static void* iqy_get_format(void * hdl, IPTV_VIDEO_URL_REQ_T * p_req)
{
	iqy_vod_dp_priv * priv = (iqy_vod_dp_priv *)hdl;	
	IPTV_VIDEO_FORMAT_T * format = priv->format;
	
	DEBUG(IQY,INFO,"tvQid[%s],vid[%s]\n",p_req->tvQid,p_req->vid);
	priv->cur_episode_num  =  p_req->episode_num;
	_get_format(hdl, format, p_req->tvQid, p_req->vid);
	return format;
}


// TODO: tag 在获取分类时已经传递，这个函数暂时不用
static  void *  iqy_get_category_type_list(void * hdl, int cat_id)
{
	iqy_vod_dp_priv * priv = (iqy_vod_dp_priv *)hdl;
	IPTV_CATEGORY_TYPE_ALL_T *category_type = priv->category_type_all;

	MT_ASSERT(category_type != NULL);
	MT_ASSERT(priv->category_list != NULL);
	
	DEBUG(IQY,INFO,"call in  and return catrgory list for type!\n");
	category_type->category 	= priv->category_list;
	category_type->status     	= IPTV_DATA_SUCCESS;
	return priv->category_type_all;
}

static void *iqy_get_recommend_info(void * hdl,  IPTV_RECOMMEND_REQ_T * p_req)
{////获取推荐的节目单信息，如没有直接返回NULL 
	iqy_vod_dp_priv * priv = (iqy_vod_dp_priv *)hdl;
	int err;
	int i;
	int c;
	int count = 0;
	
	DEBUG(IQY,INFO,"p_req channel id[%d] qpId[%s],tvQid[%s],index[%d],size[%d]\n",
		p_req->cat_id,p_req->id.qpId,p_req->id.tvQid,p_req->page_index,p_req->page_size);


	if (priv->recommend_info->albums == NULL)
	{
		priv->recommend_info->albums = (IQY_AlbumInfo*)SY_CALLOC(1,sizeof(priv->recommend_info->albums[0])*RECMD_MAX_NUMBER);
		if (priv->recommend_info->albums == NULL)
		{
			DEBUG(IQY,ERR,"malloc failure!\n");
			return NULL;
		}
	}
RE_GET:
	DEBUG(IQY,INFO,"have[%d],total[%d]\n",priv->recommend_info->have_geted,priv->recommend_info->totalcount);	
	if (IIPTV_CMP_ID_2(&priv->recommend_info->id,&p_req->id))
	{
		DEBUG(IQY,INFO,"have[%d],total[%d]\n",priv->recommend_info->have_geted,priv->recommend_info->totalcount);
		if (priv->recommend_info->have_geted == 1) //get
		{
			for (i=0;i<priv->recommend_info->channel_num;i++)
			{
				priv->recommend_info->albums[i] = priv->op_album_list->data[i];
				DEBUG(IQY,INFO,"name[%s]\n",priv->recommend_info->albums[i].name);
			}
			return priv->recommend_info;
		}
	}
	else
	{
		priv->recommend_info->have_geted = 0;
		priv->recommend_info->id = p_req->id;
	}
	for (c=0,err = -1;c<RUN_IQY_LOOP && err != IQY_NORMAL;c++)	
	{
		err = iqy_getRelatedRecommendAlbumList(0,p_req->cat_id,p_req->id.qpId,p_req->id.tvQid, 
					IQY_ALBUM_LIST_MAX,priv->op_album_list);
	}
	if (err != IQY_NORMAL)
	{
		DEBUG(IQY,ERR,"iqy_getRelatedRecommendAlbumList faliure! err[%d]\n",err);
		return NULL;
	}

	DEBUG(IQY,INFO,"get total[%d],size[%d]\n",priv->op_album_list->total,priv->op_album_list->size);

	count = priv->op_album_list->size > RECMD_MAX_NUMBER?RECMD_MAX_NUMBER: priv->op_album_list->size;
	for (i=0;i<count;i++)
	{
		DEBUG(IQY,INFO,"name[%s]\n",priv->op_album_list->data[i].name);
	}
	priv->recommend_info->totalcount = count;
	priv->recommend_info->channel_num= count;	
	priv->recommend_info->pagecount = count;
	priv->recommend_info->identify     = p_req->identify;
	priv->recommend_info->status	 = IPTV_DATA_SUCCESS;
	priv->recommend_info->have_geted = 1;
	goto RE_GET;
		
}


extern "C"  VDO_ID_t *iqy_get_current_video_id(void)
{
	MT_ASSERT(_ctrb != NULL);
	return &_ctrb->cur_video_id;
}
 
extern "C"  int	iqy_get_current_channel_id(void)
{
	MT_ASSERT(_ctrb != NULL);
	return _ctrb->cur_chId;
}

extern "C"  char* iqy_get_current_plid(void)
{
	MT_ASSERT(_ctrb != NULL);
	return _ctrb->cur_plid;
}

extern "C"  int iqy_get_current_episode_num(void)
{
	MT_ASSERT(_ctrb != NULL);
	return _ctrb->cur_episode_num;
}


#ifdef __cplusplus
extern "C" {
#endif 
BOOL sys_set_iqy_info(char *id, u32 length);
BOOL sys_get_iqy_info(char *id, u32 length);
void time_set(utc_time_t *p_time);
unsigned long porting_gen_systime_utc_time(void);
#ifdef __cplusplus
}
#endif 


extern "C"  int iqy_set_system_time(void)
{
	IQY_SystemTime time_sys;
	u32 int_value = 0;
	iqy_getSystemTime(&time_sys);
	OS_PRINTF("\n #######get current system[%s]######\n\n",time_sys.time);
	int_value = atoi(time_sys.time);
	if (int_value == 0)
		return -1;
	
	time_t time = int_value;
	struct tm *tm_t;
	utc_time_t utc_time = {0};
	tm_t = gmtime(&time);
	if((tm_t != NULL) && (tm_t->tm_year != 0))
	{
		OS_PRINTF("tm.tm_year[%d]",tm_t->tm_year+1900);
		OS_PRINTF("tm.tm_mon[%d]",tm_t->tm_mon+1);
		OS_PRINTF("tm.tm_yday[%d]",tm_t->tm_yday);
		OS_PRINTF("tm.tm_mday[%d]",tm_t->tm_mday);
		OS_PRINTF("tm.tm_wday[%d]",tm_t->tm_wday);
		OS_PRINTF("tm.tm_hour[%d]",tm_t->tm_hour);
		OS_PRINTF("tm.tm_min[%d]",tm_t->tm_min);
		OS_PRINTF("tm.tm_sec[%d]",tm_t->tm_sec);
		utc_time.year 		= tm_t->tm_year+1900;
		utc_time.month	= tm_t->tm_mon+1;
		utc_time.day		= tm_t->tm_mday;
		utc_time.hour		= tm_t->tm_hour;	
		utc_time.minute	= tm_t->tm_min;
		utc_time.second	= tm_t->tm_sec;
		time_set(&utc_time);
		return 0;		
	}
	return -1;
}
extern "C"  int iqy_lib_init(void)
{
#define   PULL_DATA_THREAD_STACK_LEN  (128*1024)
#define   PRELOAD_DATA_THREAD_STACK_LEN  (16)
#define   PRELOAD_AUDIO_BUF_SIZE    (16)
#define   PRELOAD_VIDEO_BUF_SIZE     (16)
#define   IQY_RUN_FLAG				("Have Run IQY")
	static IQY_Init 				init;
	static PB_SEQ_PARAM_T 	PB_cfg;
	char 	  flag[20] = {0};
	static MPLAYER_PARAM_T	mcfg;
	int ret;
	static int  have_inited = 0;
	IQY_SDK_Version ver;

	if (have_inited)
	{
		DEBUG(IQY,INFO,"IQY LIB have inited!\n");
		return 0;
	}
	PB_cfg.pb_seq_mem_size  		= VIDEO_FILE_PLAY_SIZE;
	PB_cfg.pb_seq_mem_start 		= VIDEO_FILE_PLAY_ADDR;
	PB_cfg.stack_size       			= PULL_DATA_THREAD_STACK_LEN;
	PB_cfg.task_priority    			= FILE_PLAYBACK_PRIORITY;
	PB_cfg.stack_preload_size       	= PRELOAD_DATA_THREAD_STACK_LEN;
	PB_cfg.preload_audio_buffer_size 	= PRELOAD_AUDIO_BUF_SIZE;
	PB_cfg.preload_video_buffer_size 	= PRELOAD_VIDEO_BUF_SIZE;

	mcfg.cfg = &PB_cfg;
	mcfg.video_fw_cfg_addr = VIDEO_FW_CFG_ADDR;
	mcfg.video_fw_cfg_size  = VIDEO_FW_CFG_SIZE;
	mcfg.audio_fw_cfg_addr	= AUDIO_FW_CFG_ADDR;
	mcfg.audio_fw_cfg_size  = AUDIO_FW_CFG_SIZE;

	//get if first run iqy
	if (sys_get_iqy_info(flag, sizeof(flag) -1))
	{
		DEBUG(IQY,INFO,"flag[%s]\n",flag);
		init.firstPwOn = 0;
	}
	else
	{
		sys_set_iqy_info((char*)IQY_RUN_FLAG, sizeof(IQY_RUN_FLAG));
		init.firstPwOn = 1;
	}
	init.mode = 0;
	init.playerInit.size = sizeof(mcfg);
	init.playerInit.cfg = (void*)&mcfg;
	ret = iqy_libInit(&init);
	if (ret != IQY_NORMAL){
		DEBUG(IQY,ERR," iqy lib init failure ret = %d\n",ret);
		return -1;
	}
	iqy_getSDKVersion(&ver);
	DEBUG(IQY,INFO,"\n #######iqy lib Version[%s]######\n\n",ver.compileTime);
	have_inited = 1; 
	return 0;
}


int iqy_iptv_dp_init(IPTV_DP_HDL_T * hdl)
{
	iqy_vod_dp_priv *priv = NULL;
	int malloc_size_total = 0;

	/*init priv data mem and clean to 0 */
	priv = (iqy_vod_dp_priv *)SY_CALLOC(1,sizeof(iqy_vod_dp_priv));
	MT_ASSERT(priv != NULL);

	priv->category_list 			=  (IPTV_CATEGORY_LIST_T*)SY_CALLOC(1,sizeof (*priv->category_list));
	DEBUG(IQY,INFO,"addr[%#x] malloc total[%d],category_list[%d]\n",(u32)priv->category_list,(malloc_size_total += sizeof (*priv->category_list)),sizeof (*priv->category_list));
	
	priv->category_type_all		=  (IPTV_CATEGORY_TYPE_ALL_T*)SY_CALLOC(1,sizeof (*priv->category_type_all));
	DEBUG(IQY,INFO,"addr[%#x] malloc total[%d],category_type_all[%d]\n",(u32)priv->category_type_all, (malloc_size_total += sizeof (*priv->category_type_all)),sizeof (*priv->category_type_all));
	
	priv->program_list			=  (IPTV_CHANNEL_LIST_T*)SY_CALLOC(1,sizeof (*priv->program_list));
	DEBUG(IQY,INFO,"addr[%#x] malloc total[%d],program_list[%d]\n",(u32)priv->program_list, (malloc_size_total += sizeof (*priv->program_list)),sizeof (*priv->program_list));
	
	priv->program_info			=  (IPTV_CHANNEL_INFO_T*)SY_CALLOC(1,sizeof (*priv->program_info));
	DEBUG(IQY,INFO,"addr[%#x] malloc total[%d],program_info[%d]\n", (u32)priv->program_info, (malloc_size_total += sizeof (*priv->program_info)),sizeof (*priv->program_info));

	priv->recommend_info		=  (IPTV_RECMD_INFO_T*)SY_CALLOC(1,sizeof (*priv->recommend_info));
	DEBUG(IQY,INFO,"addr[%#x] malloc total[%d],recommend_info[%d]\n",(u32)priv->recommend_info, (malloc_size_total += sizeof (*priv->recommend_info)),sizeof (*priv->recommend_info));

	priv->area_info_list			=  (IPTV_AREA_INFO_LIST_T*)SY_CALLOC(1,sizeof (*priv->area_info_list));
	DEBUG(IQY,INFO,"addr[%#x] malloc total[%d],area_info_list[%d]\n",(u32)priv->area_info_list, (malloc_size_total += sizeof (*priv->area_info_list)),sizeof (*priv->area_info_list));

	priv->play_url				=  (IPTV_CHANNEL_PLAY_URL_T*)SY_CALLOC(1,sizeof (*priv->play_url));
	DEBUG(IQY,INFO,"addr[%#x] malloc total[%d],play_url[%d]\n",(u32)priv->play_url, (malloc_size_total += sizeof (*priv->play_url)),sizeof (*priv->play_url));

	priv->format				=  (IPTV_VIDEO_FORMAT_T*)SY_CALLOC(1,sizeof (*priv->format));
	DEBUG(IQY,INFO,"addr[%#x] malloc total[%d],format[%d]\n",(u32)priv->format, (malloc_size_total += sizeof (*priv->format)),sizeof (*priv->format));





	priv->op_channel_list		=  (IQY_ChannelList*)SY_CALLOC(1,sizeof (*priv->op_channel_list));
	DEBUG(IQY,INFO,"addr[%#x] malloc total[%d],op_channel_list[%d]\n",(u32)priv->op_channel_list, (malloc_size_total += sizeof (*priv->op_channel_list)),sizeof (*priv->op_channel_list));
	
	priv->op_album_list		=  (IQY_AlbumList*)SY_CALLOC(1,sizeof (*priv->op_album_list));
	DEBUG(IQY,INFO,"addr[%#x] malloc total[%d],op_album_list[%d]\n",(u32)priv->op_album_list, (malloc_size_total += sizeof (*priv->op_album_list)),sizeof (*priv->op_album_list));

	priv->op_play_list			=  (IQY_PlayList*)SY_CALLOC(1,sizeof (*priv->op_play_list));
	DEBUG(IQY,INFO,"addr[%#x] malloc total[%d],op_play_list[%d]\n",(u32)priv->op_play_list, (malloc_size_total += sizeof (*priv->op_play_list)),sizeof (*priv->op_play_list));

	
	priv->op_episode_list		=  (IQY_EpisodeList*)SY_CALLOC(1,sizeof (*priv->op_episode_list));
	DEBUG(IQY,INFO,"addr[%#x] malloc total[%d],op_episode_list[%d]\n",(u32)priv->op_episode_list, (malloc_size_total += sizeof (*priv->op_episode_list)),sizeof (*priv->op_episode_list));

	priv->op_chLable_list 		= (IQY_ChannelLabelList*)SY_CALLOC(1,sizeof (*priv->op_chLable_list));
	DEBUG(IQY,INFO,"addr[%#x] malloc total[%d],op_episode_list[%d]\n",(u32)priv->op_chLable_list, (malloc_size_total += sizeof (*priv->op_chLable_list)),sizeof (*priv->op_chLable_list));

	priv->op_search_word 		= (IPTV_SEARCH_WORD_T*)SY_CALLOC(1,sizeof (*priv->op_search_word));
	DEBUG(IQY,INFO,"malloc total[%d],op_search_word[%d]\n",(u32)priv->op_search_word, (malloc_size_total += sizeof (*priv->op_search_word)),sizeof (*priv->op_search_word));



	priv->op_window			=  (IPTV_PROGRAM_WINDOW_T*)SY_CALLOC(1, sizeof (*priv->op_window));	
	MT_ASSERT(priv->op_window 	!= NULL);
	priv->op_window->array 	= (IQY_AlbumInfo*)SY_CALLOC(1,sizeof (priv->op_window->array[0])*D_WINDOW_MAX_ALBUM);
	priv->op_window->pos 		= (IPTV_PAGE_POS_T*)SY_CALLOC(1,sizeof (priv->op_window->pos[0])*D_WINDOW_MAX_BUFF);
	priv->op_window->pos_index = -1;
	DEBUG(IQY,INFO,"priv->op_window[%#x]\n",(u32)priv->op_window);		
	DEBUG(IQY,INFO,"priv->op_window->array[%#x]\n",(u32)priv->op_window->array);		
	DEBUG(IQY,INFO,"priv->op_window->pos[%#x]\n",(u32)priv->op_window->pos);		
	DEBUG(IQY,INFO,"priv->op_window[%#x]\n",(u32)priv->op_window);		
	
	DEBUG(IQY,INFO,"malloc total[%d],op_window[%d]\n",
		(malloc_size_total += sizeof (*priv->op_window) 
			+ sizeof (priv->op_window->array[0])*D_WINDOW_MAX_ALBUM
			+ sizeof (priv->op_window->pos[0])*D_WINDOW_MAX_BUFF),
		(sizeof (*priv->op_window) 
			+ sizeof (priv->op_window->array[0])*D_WINDOW_MAX_ALBUM
			+ sizeof (priv->op_window->pos[0])*D_WINDOW_MAX_BUFF));


	malloc_size_total += sizeof (*priv->category_list);

	MT_ASSERT(priv->op_window->pos 	!= NULL);
	MT_ASSERT(priv->op_window->array 	!= NULL);
	


	MT_ASSERT(priv->category_list 	!= NULL);
	MT_ASSERT(priv->category_type_all 	!= NULL);
	MT_ASSERT(priv->program_list 		!= NULL);
	MT_ASSERT(priv->program_info 		!= NULL);
	
	MT_ASSERT(priv->recommend_info 	!= NULL);
	MT_ASSERT(priv->area_info_list 		!= NULL);

	MT_ASSERT(priv->op_channel_list 	!= NULL);
	MT_ASSERT(priv->op_album_list 		!= NULL);
	MT_ASSERT(priv->op_episode_list 	!= NULL);

	(void)iptv_get_program_page;
	
	/* init call back function */
	hdl->get_category_list 		= (get_category_list_func )iqy_get_category_list;
	hdl->get_category_type_list	= (get_category_type_list_func)iqy_get_category_type_list;
	hdl->update_page 			= (update_page_func )iptv_get_new_program_page;
	hdl->get_recommend_info 	= (get_recommend_info_func )iqy_get_recommend_info;
	hdl->get_video_info 		= (get_video_info_func )iqy_get_program_detail_info;
	hdl->get_playurl 			= (get_playurl_func )iqy_get_url;
	hdl->get_playurl_format 	= (get_playurl_format_func )iqy_get_format;
	hdl->get_area = NULL;
	hdl->get_search_word	= (get_search_word_func )iptv_get_search_word_by_pinyin;

	priv->is_init = TRUE ;
	priv->get_new_info_from_net_every_call_in = FALSE;
	hdl->priv = (void *)priv;
	_ctrb = priv;
	iqy_lib_init();
	iqy_set_system_time();

/*	

	static char buffer_[2048];
	porting_http_get("http://open.baidu.com/special/time/",
		NULL, 
		NULL, 
		0,
		2, 
		buffer_,
		sizeof(buffer_));
	OS_PRINTF("\n #######get net time[%s]######\n\n",buffer_);
	*/
	return 0;
}


int iqy_iptv_dp_deinit(IPTV_DP_HDL_T * hdl)
{
	iqy_vod_dp_priv *priv = (iqy_vod_dp_priv *)hdl->priv;
DEBUG(IQY,INFO,"\n");
	if(priv == NULL)
		return 0;

	if (priv->category_list->array != NULL)
	{
		SY_FREE(priv->category_list->array);
		priv->category_list->array = NULL;
	}

	if (priv->category_list->labels!= NULL)
	{
		SY_FREE(priv->category_list->labels);
		priv->category_list->labels= NULL;
	}
	
	if(priv->category_list != NULL)
	{
		SY_FREE(priv->category_list);
		priv->category_list = NULL;
	}

	if(priv->category_type_all != NULL)
	{
		SY_FREE(priv->category_type_all);
		priv->category_type_all = NULL;
	}


	if (priv->program_list->array != NULL)
	{
		SY_FREE(priv->program_list->array );
		priv->program_list->array = NULL;
	}

	if(priv->program_list != NULL)
	{
		SY_FREE(priv->program_list);
		priv->program_list = NULL;
	}

	if (priv->program_info->episodes!= NULL)
	{
		SY_FREE(priv->program_info->episodes );
		priv->program_info->episodes = NULL;
	}

	if(priv->program_info != NULL)
	{
		SY_FREE(priv->program_info);	
		priv->program_info = NULL;
	}

	if (priv->recommend_info->albums != NULL)
	{
		SY_FREE(priv->recommend_info->albums);
		priv->recommend_info->albums = NULL;
	}

	if(priv->recommend_info != NULL)
	{
		SY_FREE(priv->recommend_info);
		priv->recommend_info = NULL;
	}

	if(priv->area_info_list != NULL)
	{
		SY_FREE(priv->area_info_list);
		priv->area_info_list = NULL;
	}

	if(priv->play_url != NULL)
	{
		SY_FREE(priv->play_url);
		priv->play_url = NULL;
	}

	if(priv->format != NULL)
	{
		SY_FREE(priv->format);
		priv->format = NULL;
	}

	if(priv->op_channel_list != NULL)
	{
		SY_FREE(priv->op_channel_list);	
		priv->op_channel_list = NULL;
	}

	if(priv->op_album_list != NULL)
	{
		SY_FREE(priv->op_album_list);	
		priv->op_album_list = NULL;
	}

	if(priv->op_play_list != NULL)
	{
		SY_FREE(priv->op_play_list);
		priv->op_play_list = NULL;
	}

	if(priv->op_episode_list != NULL)
	{
		SY_FREE(priv->op_episode_list);
		priv->op_episode_list = NULL;
	}

	if(priv->op_chLable_list != NULL)
	{
		SY_FREE(priv->op_chLable_list);
		priv->op_chLable_list = NULL;
	}
	if(priv->op_search_word->search)
	{
		SY_FREE(priv->op_search_word->search);
		priv->op_search_word->search = NULL;
	}

	if(priv->op_search_word != NULL)
	{
		SY_FREE(priv->op_search_word);
		priv->op_search_word = NULL;
	}

	if(priv->op_window->array != NULL)
	{
		SY_FREE(priv->op_window->array);
		priv->op_window->array = NULL;
	}

	if(priv->op_window->pos != NULL)
	{
		SY_FREE(priv->op_window->pos);
		priv->op_window->pos = NULL;
	}

	if(priv->op_window != NULL)
	{
		SY_FREE(priv->op_window);
		priv->op_window = NULL;
	}

	if(priv != NULL)
	{
		SY_FREE(priv);
		priv = NULL;
	}
	iqy_releasePlayer();
	return 0;
}

#if 0
void  test___(void)
{
	string s1;
	string s2;

	s1 = "123";
	OS_PRINTF("s1 %s\n",s1.c_str());
	s2 = "456";
	OS_PRINTF("s1 %s\n",s1.c_str());
	OS_PRINTF("s2 %s\n",s2.c_str());
	

}
#endif
#if 0
void getChannelList()
{
    mtos_printk("IQY_ChannelList size = %lu \r\n", sizeof(IQY_ChannelList));

    ////////////////////////////////////////
    //get all the channel
    ////////////////////////////////////////
    IQY_ChannelList* pChnlLst = SY_MALLOC(sizeof(IQY_ChannelList));
    if(pChnlLst != NULL)
    {
        memset(pChnlLst, 0, sizeof(IQY_ChannelList));
        char version[20] = "1.0.1.0";
        iqy_getChannelList(version, 1, 5, pChnlLst);

        IQY_ChannelLabelList* pLabelList = SY_MALLOC(sizeof(IQY_ChannelLabelList));
        memset(pLabelList, 0, sizeof(IQY_ChannelLabelList));
        iqy_getChannelLabelList(0, pChnlLst->data[0].recTag, pLabelList);

        IQY_AlbumList* pAlbumList = SY_MALLOC(sizeof(IQY_AlbumList));

        memset(pAlbumList, 0, sizeof(IQY_AlbumList));
        iqy_getAlbumListById(0, pLabelList->labelList[0].plid, TYPE_PLAYLIST, pAlbumList);

        memset(pAlbumList, 0, sizeof(IQY_AlbumList));
        iqy_getAlbumListById(0, pChnlLst->data[0].focus, TYPE_RECOMMAND, pAlbumList);


        SY_FREE(pChnlLst);
        SY_FREE(pAlbumList);
        SY_FREE(pLabelList);

    }
}

#endif



