#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

#include "list.h"
#include "drv_dev.h"
#include "block.h"
#include "fsioctl.h"
#include "ufs.h"

#include "commonData.h"
#include "download_api.h"

#include "IIPTVDataProvider.h"
#include "sys_dbg.h"

#define   DEF_ALLOC_CATEGORY_NUM  (20)
#define   DEF_CATEGARY_COUNT (100)

#if  0
#define  OS_PRINTF   printf
#define  mtos_printk  printf
#endif

#define   IPTV_LOG(format, args...)                 OS_PRINTF(format, ##args)
#define   IPTV_DEBUG(format, args...)            OS_PRINTF(format, ##args)
#define   IPTV_ERROR(format, args...)           mtos_printk(format, ##args)


extern int iqy_iptv_dp_init(IPTV_DP_HDL_T * hdl);
extern int iqy_iptv_dp_deinit(IPTV_DP_HDL_T * hdl);

extern char *iptv_strdup(const char *s);



static void iptv_register_callback(void * p_hdl, void * p_cb_func) 
{
	IPTV_DP_HDL_T * p_iptv_data = (IPTV_DP_HDL_T *)p_hdl;
	p_iptv_data->event_callback = (iptv_event_callback)p_cb_func;
	return;
}

static int do_iptv_cmd(void * p_dp, void * p_param)
{

	IPTV_VIDEO_FORMAT_T 				* format = NULL;
	IPTV_CATEGORY_LIST_T  			* category_list = NULL;
	IPTV_CATEGORY_TYPE_ALL_T 		* category_type_list = NULL;
	IPTV_CHANNEL_LIST_T    			* channel_list = NULL;
	IPTV_CHANNEL_INFO_T   			* channel_info = NULL;
	IPTV_CHANNEL_PLAY_URL_T    		* play_channel_url = NULL;
	IPTV_RECMD_INFO_T 				* recommend_info = NULL;
	IPTV_AREA_INFO_LIST_T 			* area_list = NULL;
	IPTV_SEARCH_WORD_T  			* search_word = NULL;



	DP_CMD_T tmp_cmd;
	memset(&tmp_cmd,0,sizeof(DP_CMD_T));
	memcpy(&tmp_cmd,p_param,sizeof(DP_CMD_T));

	IPTV_DP_HDL_T * iptvDP = (IPTV_DP_HDL_T *)p_dp;
	DP_CMD_T * p_cmd = (DP_CMD_T *)(&tmp_cmd);

	IPTV_UPPAGE_REQ_T *p_update_page_req = (IPTV_UPPAGE_REQ_T *)(p_cmd->param);
	IPTV_RECOMMEND_REQ_T *p_recmd_req = (IPTV_RECOMMEND_REQ_T *)(p_cmd->param);
	IPTV_VIDEO_INFO_REQ_T *p_info_req = (IPTV_VIDEO_INFO_REQ_T *)(p_cmd->param);
	IPTV_VIDEO_URL_REQ_T *p_url_req = (IPTV_VIDEO_URL_REQ_T *)(p_cmd->param);
	IPTV_REQ_PARAM_T * p_req =  (IPTV_REQ_PARAM_T *)(p_cmd->param);
	u8 *searchWord = (u8 *)(p_cmd->param);


	switch(p_cmd->type)
	{
		case GET_IPTV_INIT:
		{	
			int ret  = 0 ;
			DEBUG(IPTVDP,INFO,"do command [GET_IPTV_INIT]\n");
			ret = iqy_iptv_dp_init(iptvDP);
			if(iptvDP->event_callback)
			{
				if(ret == 0)
				{
					DEBUG(IPTVDP,INFO,"iqy iptv dp init sucess!\n");
					iptvDP->event_callback(IPTV_INIT_SUCCESS, (u32) ret);
				}
				else
				{
					DEBUG(IPTVDP,ERR,"iqy iptv dp init failure!\n");
					iptvDP->event_callback(IPTV_INIT_FAIL, (u32) ret);
				}
			}
		}		
		break;


		case GET_IPTV_CATEGORY:
			DEBUG(IPTVDP,INFO,"do cmd: GET_CATEGORY\n");
			if(iptvDP->get_category_list != NULL)
			{
				category_list = (IPTV_CATEGORY_LIST_T *)iptvDP->get_category_list(iptvDP->priv);
				if(iptvDP->event_callback)
				{
					if (category_list != NULL)
					{
						iptvDP->event_callback(IPTV_CATEGORY_ARRIVAL, (u32) category_list);
					}
					else
					{
						DEBUG(IPTVDP,ERR,"category ret NULL!!\n");					
						iptvDP->event_callback(IPTV_INIT_FAIL, (u32) category_list);
					}
				}
			}
			if(p_req)
				SY_FREE(p_req);
		break;

		case GET_IPTV_CATEGORY_TYPE:
			DEBUG(IPTVDP,INFO,"do cmd: GET_IPTV_CATEGORY_TYPE\n");
			if(iptvDP->get_category_type_list != NULL)
			{
				category_type_list = (IPTV_CATEGORY_TYPE_ALL_T *)iptvDP->get_category_type_list(iptvDP->priv, p_req->cat_id);
				if(iptvDP->event_callback)
				{
					if (category_type_list != NULL)
					{
						iptvDP->event_callback(IPTV_CATEGORY_TYPE_ARRIVAL, (u32) category_type_list);
					}
					else
					{
						DEBUG(IPTVDP,ERR,"category type ret NULL!!\n");										
						iptvDP->event_callback(IPTV_INIT_FAIL, (u32) category_type_list);
					}
				}
			}
			if(p_req)
				SY_FREE(p_req);
		break;

		case GET_IPTV_NEW_PAGE:
			DEBUG(IPTVDP,INFO,"do cmd: GET_NEW_PAGE\n");
			if(iptvDP->update_page  != NULL)
			{

				channel_list = (IPTV_CHANNEL_LIST_T *)iptvDP->update_page(iptvDP->priv, p_update_page_req);
				if(channel_list != NULL)
				{
					channel_list->category_id = p_update_page_req->cat_id;
					channel_list->page_num= p_update_page_req->page_index;
					channel_list->page_size= p_update_page_req->page_size;
					channel_list->query_mode= p_update_page_req->query_mode;
				} //if(NULL)
				if(iptvDP->event_callback)
				{
					if(channel_list != NULL)
					{
						iptvDP->event_callback(IPTV_NEW_PAGE_ARRIVAL, (u32)channel_list);	
					}
					else
					{
						DEBUG(IPTVDP,ERR,"get channel ret NULL!!\n");					
						iptvDP->event_callback(IPTV_INIT_FAIL, (u32)channel_list);	
					}
				}
			}
			if(p_update_page_req)
				SY_FREE(p_update_page_req);
		break;

		case GET_IPTV_VIDEO_INFO:
			DEBUG(IPTVDP,INFO,"do cmd: GET_IPTV_VIDEO_INFO p_info_req index[%d],size[%d]\n",
					p_info_req->page_index,p_info_req->page_size);
			if(iptvDP->get_video_info != NULL)
			{
				channel_info = (IPTV_CHANNEL_INFO_T *)iptvDP->get_video_info(iptvDP->priv, p_info_req);
				if(iptvDP->event_callback)
				{
					if(channel_info != NULL)
					{
						channel_info->is_description =  p_info_req->is_description;
						channel_info->query_mode= p_info_req->query_mode;
						iptvDP->event_callback(IPTV_VIDEO_INFO_ARRIVAL, (u32) channel_info);
					}
					else
					{
						DEBUG(IPTVDP,ERR,"get play video ret NULL!!\n");										
						iptvDP->event_callback(IPTV_INIT_FAIL, (u32)channel_info);	
					}
				}
			}
			if(p_info_req)
				SY_FREE(p_info_req);
		break;

		case GET_IPTV_PLAY_URL:
			DEBUG(IPTVDP,INFO,"do cmd: GET_IPTV_PLAY_URL\n");
			if(iptvDP->get_playurl != NULL)
			{
				play_channel_url = (IPTV_CHANNEL_PLAY_URL_T *)iptvDP->get_playurl(iptvDP->priv, p_url_req);
				if(iptvDP->event_callback)
				{
					if (play_channel_url != NULL)
					{
						iptvDP->event_callback(IPTV_PLAY_URL_ARRIVAL, (u32) play_channel_url);
					}
					else
					{
						DEBUG(IPTVDP,ERR,"get play url ret NULL!!\n");					
						iptvDP->event_callback(IPTV_INIT_FAIL, (u32)play_channel_url);	
					}
				}
			}
			if(p_url_req)
				SY_FREE(p_url_req);
		break;

		case GET_IPTV_FORMAT_TYPE:
			DEBUG(IPTVDP,INFO,"do cmd: GET_IPTV_FORMAT_TYPE\n");
			if(iptvDP->get_playurl_format != NULL)
			{
				format = iptvDP->get_playurl_format(iptvDP->priv, p_url_req);
				if(iptvDP->event_callback)
					iptvDP->event_callback(IPTV_FORMAT_TYPE_ARRIVAL, (u32)format);				
			}
			if(p_url_req)
				SY_FREE(p_url_req);
		break;

		case GET_IPTV_RECOMMEND_INFO:
			DEBUG(IPTVDP,INFO,"do cmd: GET_IPTV_RECOMMEND_INFO\n");			
			if(iptvDP->get_recommend_info != NULL)
			{
				recommend_info = (IPTV_RECMD_INFO_T *)iptvDP->get_recommend_info(iptvDP->priv, p_recmd_req);
				if(iptvDP->event_callback)
				{
					if (recommend_info != NULL)
					{
						iptvDP->event_callback(IPTV_RECOMMEND_INFO_ARRIVAL, (u32)recommend_info);
					}
					else
					{
						DEBUG(IPTVDP,ERR,"get RECOMMEND ret NULL!!\n");	
						iptvDP->event_callback(IPTV_INIT_FAIL, (u32)recommend_info);
					}
				}
			}

			if(p_recmd_req)
				SY_FREE(p_recmd_req);
		break;

		case GET_IPTV_AREA:
			DEBUG(IPTVDP,INFO,"do cmd: GET_IPTV_AREA\n");			
			if(iptvDP->get_area != NULL)
			{
				area_list = (IPTV_AREA_INFO_LIST_T *)iptvDP->get_area(iptvDP->priv);
				if(iptvDP->event_callback)
				{
					if (area_list != NULL)
					{
						iptvDP->event_callback(IPTV_AREA_INFO_ARRIVAL, (u32) area_list);	
					}
					else
					{
						DEBUG(IPTVDP,ERR,"get area ret NULL!!\n");	
						iptvDP->event_callback(IPTV_INIT_FAIL, (u32) area_list);	
					}
				}
			}
			if(p_req)
				SY_FREE(p_req);

		break;

		case GET_IPTV_DEINIT:
			DEBUG(IPTVDP,INFO,"do cmd: GET_IPTV_DEINIT\n");		
			iqy_iptv_dp_deinit(iptvDP);
			if(iptvDP->event_callback)
				iptvDP->event_callback(IPTV_DEINIT_SUCCESS, NULL);

			if(p_req)
				SY_FREE(p_req);
		break;

		case GET_IPTV_SEARCH_WORD:
			DEBUG(IPTVDP,INFO,"do cmd: GET_IPTV_SEARCH_WORD\n");
			if(iptvDP->get_search_word != NULL)
			{
				search_word = (IPTV_SEARCH_WORD_T *)iptvDP->get_search_word(iptvDP->priv, searchWord);
				if(iptvDP->event_callback)
				{
					if (search_word != NULL)
					{
						iptvDP->event_callback(IPTV_SEARCH_WORD_ARRIVAL, (u32) search_word);
					}
					else
					{
						DEBUG(IPTVDP,ERR,"get search word ret NULL!!\n");
						iptvDP->event_callback(IPTV_INIT_FAIL, (u32) search_word);
					}
				}
			}
			if(p_req)
				SY_FREE(p_req);
		break;

		default :
			DEBUG(IPTVDP,ERR,"do cmd: can't recognize this cmd  %d!!!!!!!!!\n", p_cmd->type);
		break;
	}
	return 0;
}

/**
*
*
*
*
*
*
*
*
*
*
*
*
*
*/
IPTV_DP_HDL_T *  IIPTV_initDataProvider(DO_CMD_TASK_CONFIG_T * p_config)
{
	IPTV_DP_HDL_T * p_hdl = NULL;

	p_hdl = (IPTV_DP_HDL_T *)SY_MALLOC(sizeof(IPTV_DP_HDL_T));
	memset(p_hdl,0,sizeof(IPTV_DP_HDL_T));
	DO_CMD_TASK_CONFIG_T config;
	memset(&config,0,sizeof(DO_CMD_TASK_CONFIG_T));

    	mtos_printk("[%s] start start ...\n",__func__);
	if(p_config == NULL)
	{
		config.proc_cmd_callback = do_iptv_cmd;
		config.event_cmd_callback = NULL ;
		config.p_data_provider = p_hdl;
	}
	else
	{
		memcpy((&config),(void *)p_config,sizeof(DO_CMD_TASK_CONFIG_T));
		config.proc_cmd_callback = do_iptv_cmd;
		config.event_cmd_callback = NULL ;
		config.p_data_provider = p_hdl;
		config.p_mem_start = (unsigned char *)SY_MALLOC(config.stack_size);
		MT_ASSERT(config.p_mem_start != NULL);
		memset(config.p_mem_start, 0x00, config.stack_size);
		p_hdl->bg_task_stack_addr = config.p_mem_start;
		

	}


	p_hdl->mac = iptv_strdup(p_config->mac);
	p_hdl->id = iptv_strdup(p_config->id);
	
	p_hdl->p_bg_task_hdl = DP_startDoCmdTask(config);
	p_hdl->register_event_callback = iptv_register_callback;

	//set default pagesize
	p_hdl->page_size = 5;

	p_hdl->dp =  (IPTV_DATA_PROVIDER)(p_config->dp);
	p_hdl->sub_dp = p_config->sub_dp;

	if(1)/////put the init to async in Cmd Task !
	{
		DP_CMD_T cmd;
		memset(&cmd,0,sizeof(DP_CMD_T));
		cmd.type = GET_IPTV_INIT;
		int ret = DP_sendCmd(p_hdl->p_bg_task_hdl,&cmd);
		if(ret < 0)
		{
			DEBUG(IPTVDP,ERR,"[ERROR] FAIL SEND CMD ...\n");
		}
	}

	mtos_printk("[%s] end end ...\n",__func__);
    return p_hdl;
}

/************************************************************************
*
*
*
*
*
*
*
*
***********************************************************************************/
int  IIPTV_deinitDataProvider(IPTV_DP_HDL_T * hdl)
{
    	 mtos_printk("[%s] start start ...\n",__func__);

	if(hdl == NULL)
	{
		return 0;
	}

	//send deinit command
	DP_CMD_T cmd;
	memset(&cmd,0,sizeof(DP_CMD_T));
	cmd.type = GET_IPTV_DEINIT;
	int ret = DP_sendCmd(hdl->p_bg_task_hdl,&cmd);
	if(ret < 0)
	{
		OS_PRINTF("[%s][ERROR] FAIL SEND CMD ...\n",__func__);
	}
	//if (hdl->bg_task_stack_addr != NULL)
		//SY_FREE(hdl->bg_task_stack_addr);
	
	//mtos_task_sleep(200);
	//DP_stopDoCmdTask(hdl->p_bg_task_hdl);
    	mtos_printk("[%s] end end ...\n",__func__);
	return 0;
}

int  IIPTV_delCmdTask(IPTV_DP_HDL_T * hdl)
{
	mtos_printk("[%s] start start ...\n",__func__);

	DP_stopDoCmdTask(hdl->p_bg_task_hdl);

	if (hdl->bg_task_stack_addr != NULL)
		SY_FREE(hdl->bg_task_stack_addr);
	return 0;
}

/*****************************************************************************
*
*
*
*
*
*
*
*
***************************************************************************/
int   IIPTV_getCategoryList(IPTV_DP_HDL_T * hdl, u32 identify)
{
	DP_CMD_T cmd;
	IPTV_REQ_PARAM_T * p_param = (IPTV_REQ_PARAM_T *)SY_MALLOC(
                                                       sizeof(IPTV_REQ_PARAM_T));

	DEBUG(IPTVDP,INFO,"call in!\n");
	memset(p_param,0,sizeof(IPTV_REQ_PARAM_T));
	memset(&cmd,0,sizeof(DP_CMD_T));
	cmd.type = GET_IPTV_CATEGORY;
	p_param->identify= identify;
	cmd.param = (u32)p_param;

	int ret = DP_sendCmd(hdl->p_bg_task_hdl,&cmd);
	if(ret < 0)
	{
		DEBUG(IPTVDP,ERR,"[%s][ERROR] FAIL SEND CMD ...\n",__func__);
	}
	
	DEBUG(IPTVDP,INFO,"call out!\n");
	return ret;
}

/*****************************************************************************
*
*
*
*
*
*
*
*
***************************************************************************/
int   IIPTV_getArea(IPTV_DP_HDL_T * hdl, u32 identify)
{
    	 OS_PRINTF("[%s] start start ...\n",__func__);
	DP_CMD_T cmd;
	IPTV_REQ_PARAM_T * p_param = (IPTV_REQ_PARAM_T *)SY_MALLOC(
                                                       sizeof(IPTV_REQ_PARAM_T));
	memset(p_param,0,sizeof(IPTV_REQ_PARAM_T));

	memset(&cmd,0,sizeof(DP_CMD_T));
	cmd.type = GET_IPTV_AREA;
	p_param->identify= identify;
	cmd.param = (u32)p_param;

	int ret = DP_sendCmd(hdl->p_bg_task_hdl,&cmd);
	if(ret < 0)
	{
		OS_PRINTF("[%s][ERROR] FAIL SEND CMD ...\n",__func__);
	}
	
	OS_PRINTF("[%s] end end ...\n",__func__);
	return ret;
}



/*****************************************************************************
*
*
*
*
*
*
*
*
***************************************************************************/
int   IIPTV_getCategoryTypeList(IPTV_DP_HDL_T * hdl, int cat_id, u32 identify)
{

	OS_PRINTF("[%s] start start ...\n",__func__);

	int ret;		 
	DP_CMD_T cmd;
	IPTV_REQ_PARAM_T * p_param = (IPTV_REQ_PARAM_T *)SY_MALLOC(
                                                           sizeof(IPTV_REQ_PARAM_T));
	memset(p_param,0,sizeof(IPTV_REQ_PARAM_T));

	memset(&cmd,0,sizeof(DP_CMD_T));
	cmd.type = GET_IPTV_CATEGORY_TYPE;
	p_param->cat_id = cat_id;
	p_param->identify= identify;
	cmd.param = (u32)p_param;
	ret = DP_sendCmd(hdl->p_bg_task_hdl,&cmd);
	if(ret < 0)
	{
		OS_PRINTF("[%s][ERROR] FAIL SEND CMD ...\n",__func__);
	}
	
	OS_PRINTF("[%s] end end ...\n",__func__);
	return 0;
}




/******************************************************
*
*
*
*
*
*
*
*
******************************************************************************/
int  IIPTV_updatePage(IPTV_DP_HDL_T * hdl,
                                                          IPTV_UPPAGE_REQ_T  *p_req)
{

	OS_PRINTF("[%s] start start ...\n",__func__);

	int ret;		 
	DP_CMD_T cmd;
	IPTV_UPPAGE_REQ_T * p_param = (IPTV_UPPAGE_REQ_T *)SY_MALLOC(
                                                           sizeof(IPTV_UPPAGE_REQ_T));
	memset(p_param,0,sizeof(IPTV_UPPAGE_REQ_T));

	OS_PRINTF("%s catid=%d, page_index=%d, page_size=%d\n", __FUNCTION__,p_req->cat_id,
				p_req->page_index, p_req->page_size);
	memset(&cmd,0,sizeof(DP_CMD_T));
	cmd.type = GET_IPTV_NEW_PAGE;
	p_param->cat_id = p_req->cat_id;
	p_param->years = p_req->years;
	p_param->identify= p_req->identify;
	p_param->query_mode= p_req->query_mode;
	p_param->sort = p_req->sort;
	p_param->cb = p_req->cb;

	strcpy(p_param->names, p_req->names);
	strcpy(p_param->title, p_req->title);
	strcpy(p_param->area, p_req->area); 
	strcpy(p_param->actor, p_req->actor);  
	strcpy(p_param->director, p_req->director);
	strcpy(p_param->types, p_req->types);
	strcpy(p_param->pinyin, p_req->pinyin);
	strcpy(p_param->attr, p_req->attr);
	strcpy(p_param->keys, p_req->keys);

	IPTV_LOG("####%s %s\n", p_req->types, p_param->types);
	
	p_param->page_index = p_req->page_index;
	p_param->searchWordIndex = p_req->searchWordIndex;
	p_param->page_size = hdl->page_size;
	cmd.param = (u32)p_param;
	ret = DP_sendCmd(hdl->p_bg_task_hdl,&cmd);
	if(ret < 0)
	{
		OS_PRINTF("[%s][ERROR] FAIL SEND CMD ...\n",__func__);
	}
	
	OS_PRINTF("[%s] end end ...\n",__func__);
	return 0;
}

/******************************************************
*
*
*
*
*
*
*
*
******************************************************************************/
int  IIPTV_getRecommendInfo(IPTV_DP_HDL_T * hdl,
                                                           IPTV_RECOMMEND_REQ_T *p_req)
{
	OS_PRINTF("[%s] start start ...\n",__func__);

	int ret;		 
	DP_CMD_T cmd;
	IPTV_RECOMMEND_REQ_T * p_param = (IPTV_RECOMMEND_REQ_T *)SY_MALLOC(
                                                           sizeof(IPTV_RECOMMEND_REQ_T));
	memset(p_param,0,sizeof(IPTV_RECOMMEND_REQ_T));
	
	memset(&cmd,0,sizeof(DP_CMD_T));
	cmd.type = GET_IPTV_RECOMMEND_INFO;
	p_param->id = p_req->id;
	p_param->cat_id = p_req->cat_id;
	p_param->page_index= p_req->page_index;
	p_param->page_size = p_req->page_size;
	p_param->identify= p_req->identify;
	p_param->cb = p_req->cb;

    	strcpy(p_param->area, p_req->area);
    	strcpy(p_param->types, p_req->types);
    
	cmd.param = (u32)p_param;
	ret = DP_sendCmd(hdl->p_bg_task_hdl,&cmd);
	if(ret < 0)
	{
		OS_PRINTF("[%s][ERROR] FAIL SEND CMD ...\n",__func__);
	}
	
	OS_PRINTF("[%s] end end ...\n",__func__);
	return 0;
}



/******************************************************
*
*
*
*
*
*
*
*
******************************************************************************/
int  IIPTV_getVideoInfo(IPTV_DP_HDL_T * hdl,
                                                         IPTV_VIDEO_INFO_REQ_T *p_req)
{
	OS_PRINTF("[%s] start start ...\n",__func__);

	int ret;		 
	DP_CMD_T cmd;
	IPTV_VIDEO_INFO_REQ_T * p_param = (IPTV_VIDEO_INFO_REQ_T *)SY_MALLOC(
                                                           sizeof(IPTV_VIDEO_INFO_REQ_T));
	memset(p_param,0,sizeof(IPTV_VIDEO_INFO_REQ_T));
	
	memset(&cmd,0,sizeof(DP_CMD_T));
	cmd.type 		= GET_IPTV_VIDEO_INFO;
	p_param->identify	= p_req->identify;
	p_param->cb	 	= p_req->cb;
	p_param->id 		= p_req->id;
	

	p_param->is_description = p_req->is_description;	

	p_param->page_index = p_req->page_index;
	
	if(p_req->page_size == 0)
		p_param->page_size = 10;
	else
		p_param->page_size = p_req->page_size;


       OS_PRINTF("p_param->page_index[%d],p_param->page_size[%d]",p_param->page_index,p_param->page_size);
	p_param->query_mode = p_req->query_mode;
	cmd.param = (u32)p_param;
	ret = DP_sendCmd(hdl->p_bg_task_hdl,&cmd);
	if(ret < 0)
	{
		OS_PRINTF("[%s][ERROR] FAIL SEND CMD ...\n",__func__);
	}
	
	OS_PRINTF("[%s] end end ...\n",__func__);
	return 0;
}


/******************************************************
*
*
*
*
*
*
*
*
******************************************************************************/
int  IIPTV_getPlayUrl(IPTV_DP_HDL_T * hdl,
                                          IPTV_VIDEO_URL_REQ_T *p_req)
  {
    OS_PRINTF("[%s] start start ...\n",__func__);
  
    int ret;     
    DP_CMD_T cmd;
    IPTV_VIDEO_URL_REQ_T * p_param = (IPTV_VIDEO_URL_REQ_T *)SY_MALLOC(
                                                             sizeof(IPTV_VIDEO_URL_REQ_T));
    memset(p_param,0,sizeof(IPTV_VIDEO_URL_REQ_T));
    
    memset(&cmd,0,sizeof(DP_CMD_T));
    cmd.type = GET_IPTV_PLAY_URL;

    *p_param = *p_req;
    cmd.param = (u32)p_param;
    ret = DP_sendCmd(hdl->p_bg_task_hdl,&cmd);
    if(ret < 0)
    {
      OS_PRINTF("[%s][ERROR] FAIL SEND CMD ...\n",__func__);
    }
    
    OS_PRINTF("[%s] end end ...\n",__func__);
    return 0;
  }


int  IIPTV_getPlayUrlFormat(IPTV_DP_HDL_T * hdl,
                                          IPTV_VIDEO_URL_REQ_T *p_req)
  {
    OS_PRINTF("[%s] start start ...\n",__func__);
  
    int ret;     
    DP_CMD_T cmd;
    IPTV_VIDEO_URL_REQ_T * p_param = (IPTV_VIDEO_URL_REQ_T *)SY_MALLOC(
                                                             sizeof(IPTV_VIDEO_URL_REQ_T));
    memset(p_param,0,sizeof(IPTV_VIDEO_URL_REQ_T));
    
    memset(&cmd,0,sizeof(DP_CMD_T));
    cmd.type = GET_IPTV_FORMAT_TYPE;
    *p_param = *p_req;
    cmd.param = (u32)p_param;
    ret = DP_sendCmd(hdl->p_bg_task_hdl,&cmd);
    if(ret < 0)
    {
      OS_PRINTF("[%s][ERROR] FAIL SEND CMD ...\n",__func__);
    }
    
    OS_PRINTF("[%s] end end ...\n",__func__);
    return 0;
  }


int 	IIPTV_setPageSize(IPTV_DP_HDL_T * hdl, int pagesize)
{
	hdl->page_size	= pagesize;
	return 0;
}
	
BOOL IIPTV_CMP_ID(VDO_ID_t *id,char *qpid,int type)
{
	if ((strcmp(id->qpId,qpid) ==0)
		&& (id->type == type))
		return TRUE;
	
	return FALSE;
}

BOOL IIPTV_CMP_ID_2(VDO_ID_t *id1,VDO_ID_t *id2)
{
	if ((strcmp(id1->qpId,id2->qpId) ==0)
		&& (strcmp(id1->tvQid,id2->tvQid) ==0)
		&& (id1->type == id2->type))
		return TRUE;
	
	return FALSE;
}

/*****************************************************************************
* fuxl add to get search word
***************************************************************************/
int   IIPTV_getSearchWord(IPTV_DP_HDL_T * hdl, u8 *searchWord)
{
	DEBUG(IPTVDP,INFO," start start ... searchWord = %s \n",searchWord);
	DP_CMD_T cmd;
	u8 *p_param = NULL;
	int str_len = strlen((char *)searchWord) + 1;
	
	p_param  = (u8 *)SY_CALLOC(1,str_len);
       strcpy((char *)p_param, (char *)searchWord);

	memset(&cmd,0,sizeof(DP_CMD_T));
	cmd.type = GET_IPTV_SEARCH_WORD;
	cmd.param = (u32)p_param;

	int ret = DP_sendCmd(hdl->p_bg_task_hdl,&cmd);
	if(ret < 0)
	{
		DEBUG(IPTVDP,ERR,"[%s][ERROR] FAIL SEND CMD ...\n",__func__);
	}

	DEBUG(IPTVDP,INFO,"end end ... \n");
	return ret;
}

