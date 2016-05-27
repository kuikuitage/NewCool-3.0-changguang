#include  <stdio.h>
#include <ctype.h>
#include "sys_types.h"
#include "sys_define.h"
#include "mtos_task.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_misc.h"
#include  "http_download.h"
#include  "http_download_mini.h"
#include  "httpc.h"
#include "zlib.h"
//#include "eventSink.h"
#include  "download_api.h"
#ifndef __LINUX__
#include "drv_dev.h"
#include "ufs.h"
#include "ff.h"
#endif
#include "download_manager.h"

#ifdef TEST_SPEED_OPEN
#include "commonData.h"
#endif

#ifndef __LINUX__
extern "C" unsigned short  *   Convert_Utf8_To_Unicode(unsigned char * putf8,unsigned short * out) ;
#endif
static void parse_url_component(parse_youtube_vedio_info_t * p_video_info, char * keyword, int index);


#define   DBG_DOWNLOAD_API

#ifdef     DBG_DOWNLOAD_API
#define DOWNLOAD_API_LOG(format, args...)              mtos_printk(format, ##args)
#define DOWNLOAD_API_WARNING(format, args...)     OS_PRINTF(format, ##args)
#define DOWNLOAD_API_DEBUG(format, args...)          OS_PRINTF(format, ##args)
#define DOWNLOAD_API_ERROR(format, args...)          mtos_printk(format, ##args)
#else
#define DOWNLOAD_API_LOG(format, args...)               mtos_printk(format, ##args)
#define DOWNLOAD_API_WARNING(format, args...)
#define DOWNLOAD_API_DEBUG(format, args...)
#define DOWNLOAD_API_ERROR(format, args...)           mtos_printk(format, ##args)
#endif

#define  MAX_TMP_URL_LEN    (2048)
#define  MAX_FILE_NAME_LEN     (2048)

#define    MAX_POST_COUNT  	(4)
#define    DEFAULT_DL_HTML_TIMEOUT  15
#define     MAX_VIDEOID_LEN  (32)

int delete_file(char * path)
{
#ifndef __LINUX__
	int ret = 0 ;
	ufs_file_t  *ufs_fd = (ufs_file_t *)mtos_malloc(sizeof(ufs_file_t));
      if(!ufs_fd){
        mtos_printk("[%s][ERROR] ufs_fd malloc failed !!!!\n", __func__);
        return -1;
      }
	memset(ufs_fd, 0x00, sizeof(ufs_file_t));

	unsigned short  path_tmp[256]={0};
	unsigned short  *path_unicode = Convert_Utf8_To_Unicode((unsigned char *)path,path_tmp);
	ret = ufs_open(ufs_fd, path_unicode, UFS_READ);
	if(ret != FR_OK)
	{
		DOWNLOAD_API_ERROR("tmp file %s is not exist\n", path);
		mtos_free(ufs_fd);
		return -1;
	}

	ufs_close(ufs_fd);
	mtos_free(ufs_fd);

	//delete tmp file
	ufs_delete(path_unicode, 0);
#endif
	return 0;
}
#ifndef TEST_SPEED_OPEN
/* Converts a hex character to its integer value */  
static char from_hex(char ch) {  
	return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;  
}  

/* Converts an integer value to its hex character*/  
static char to_hex(char code) {  
	static char hex[] = "0123456789abcdef";  
	return hex[code & 15];  
}  
static int url_encode(const char *src, char *dest)
{
	const char *pstr = src;
	char *pbuf = dest; 

	if (src == NULL || dest == NULL)
	{
		return -1;
	}
	while (*pstr) 
	{  
		if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')   
			*pbuf++ = *pstr;  
		else if (*pstr == ' ')   
			*pbuf++ = '+';  
		else   
			*pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);  
		pstr++;  
	}  
	*pbuf = '\0';  
	return pbuf - dest;  
}
static int url_decode(const char *src, char *dest)
{
	const char *pstr = src;
	char *pbuf = dest;  

	if (src == NULL || dest == NULL)
	{
		return -1;
	}
	while (*pstr) 
	{  
		if (*pstr == '%') 
		{  
			if (pstr[1] && pstr[2]) 
			{  
				*pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);  
				pstr += 2;  
			}  
		}
		else if (*pstr == '+') 
		{   
			*pbuf++ = ' ';  
		}
		else 
		{  
			*pbuf++ = *pstr;  
		}  
		pstr++;  
	}  
	*pbuf = '\0';  
	return pbuf - dest;
}

#endif


#define    DECOMPRESSED_BUFFER_LEN   (512*1024)
static int decompress_to_file(const char * srcFile, const char * dstFile)
{
    DOWNLOAD_API_DEBUG("[%s:%s:%d] start !\n", __FILE__,__func__,__LINE__);
    DOWNLOAD_API_DEBUG("srcFile[%s],dstFile[%s]\n", srcFile, srcFile);
    int ret = 0;

    uLong uncomprLen =  DECOMPRESSED_BUFFER_LEN;
    Byte *uncompr = (Byte*)mtos_malloc(uncomprLen);
    memset(uncompr, 0x00, DECOMPRESSED_BUFFER_LEN);
	
    gzFile file;

    file = gzopen(srcFile, "rb");
    if (file == NULL) {
        DOWNLOAD_API_ERROR("gzopen error \n");
        ret  = 0;
    }
    else
    {
        strcpy((char*)uncompr, "garbage");
        int len  = gzread(file, uncompr, (unsigned)uncomprLen) ;
        gzclose(file);

        DOWNLOAD_API_DEBUG("gzread(): len = %d \n", len);
        if(1)
        {
            ufs_file_t  *ufs_fd = (ufs_file_t *)mtos_malloc(sizeof(ufs_file_t));
            memset(ufs_fd, 0x00, sizeof(ufs_file_t));
            unsigned short  path_tmp_2[256]= {0};
            unsigned short  *p_filename_new = Convert_Utf8_To_Unicode((unsigned char *)dstFile,path_tmp_2);

            if (ufs_open(ufs_fd, p_filename_new, (op_mode_t)(UFS_WRITE | UFS_CREATE_NEW_COVER)) != 0)
            {
                DOWNLOAD_API_ERROR("[%s][ERROR] fail to open local file[r:yiyuan.txt] !!!\n", __func__);
                mtos_free(ufs_fd);
                ufs_fd = NULL;
                mtos_free(uncompr);
                uncompr = NULL;
                return 0;
            }
            int r = 0;
            ufs_write(ufs_fd, (void *)uncompr, (u32)len, (u32*)&r);
            ufs_close(ufs_fd);
            mtos_free(ufs_fd);
            ufs_fd = NULL;
        }
        ret  = 1;
    }
    mtos_free(uncompr);
    uncompr = NULL;
    return ret; 
}

static int rename_file(const char * srcFile, const char * dstFile)
{
    //The file is not gzip
    int ret = 0;
    ufs_file_t  *ufs_fd0 = (ufs_file_t *)mtos_malloc(sizeof(ufs_file_t));
    memset(ufs_fd0, 0x00, sizeof(ufs_file_t));
    unsigned short  path_tmp0[256]= {0};
    unsigned short  *p_filename0 = Convert_Utf8_To_Unicode((unsigned char *)srcFile,path_tmp0);

    if (ufs_open(ufs_fd0, p_filename0, (op_mode_t)(UFS_READ)) != 0)
    {
        DOWNLOAD_API_ERROR("[%s][ERROR] fail to open local file %s !!!\n", __func__, srcFile);
        mtos_free(ufs_fd0);
        ufs_fd0 = NULL;
        delete_file((char *)srcFile);
        return 0;
    }

    long  file_size = 0;
    char *file_buffer;
    file_size = ufs_fd0->file_size;
    file_buffer = (char *)mtos_malloc(file_size+1);
    if(!file_buffer){
        DOWNLOAD_API_ERROR("[%s] not enough memory to malloc file_buffer\n", __func__);
        mtos_free(ufs_fd0);
        ufs_fd0 = NULL;
        delete_file((char *)srcFile);
        return 0;
    }
    memset(file_buffer, 0x00, file_size+1);
    ufs_read(ufs_fd0, file_buffer, file_size, (u32 *)&ret);
    DOWNLOAD_API_DEBUG("[%s] %s file_size = %lu!!!\n", __func__, srcFile, ufs_fd0->file_size);
    ufs_close(ufs_fd0);
    mtos_free(ufs_fd0);
    ufs_fd0 = NULL;


    ufs_file_t  *ufs_fd1 = (ufs_file_t *)mtos_malloc(sizeof(ufs_file_t));
    memset(ufs_fd1, 0x00, sizeof(ufs_file_t));
    unsigned short  path_tmp1[256]= {0};
    unsigned short  *p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)dstFile,path_tmp1);

    if (ufs_open(ufs_fd1, p_filename1, (op_mode_t)(UFS_WRITE | UFS_CREATE_NEW_COVER)) != 0)
    {
        DOWNLOAD_API_ERROR("[%s][ERROR] fail to open local file!!!\n", __func__);
        mtos_free(ufs_fd1);
        ufs_fd1 = NULL;
        mtos_free(file_buffer);
        file_buffer = NULL;
        delete_file((char *)srcFile);
        return 0;
    }
    int r = 0;
    ufs_write(ufs_fd1, (void *)file_buffer, (u32)file_size, (u32*)&r);
    DOWNLOAD_API_DEBUG("[%s] %s file_size = %lu!!!\n", __func__, dstFile, ufs_fd1->file_size);
    ufs_close(ufs_fd1);
    mtos_free(ufs_fd1);
    ufs_fd1 = NULL;
    mtos_free(file_buffer);
    file_buffer = NULL;

    return 1;
}
			
static int DownloadAndGetHeader(const char * url,
                          const char * tempFile,
                          unsigned int timeoutSec,
                          void * response,
                          void * arg,
                          const char * extraHeaders,
                          const char * body,
                          unsigned int bodyLen,
                          HTTP_rsp_header_t *rsp_header)
{
    DOWNLOAD_API_DEBUG("[%s] start start ...\n", __func__);
    DOWNLOAD_API_DEBUG("[%s] timeout %d seconds\n", __func__, timeoutSec);

    if (url == NULL) {
        DOWNLOAD_API_ERROR("[%s] url is NULL, please check it\n", __func__);
        return 0;
    }
    
    DOWNLOAD_API_DEBUG("[%s] url [%s]...\n", __func__, url);
    char tmpurl[MAX_TMP_URL_LEN];
    char file[MAX_FILE_NAME_LEN];
    memset(tmpurl, 0, MAX_TMP_URL_LEN);
    sprintf(tmpurl, "%s", url);
    memset(file, 0, MAX_FILE_NAME_LEN);
    sprintf(file, "%s", tempFile);
    http_download_mini instance;
    http_download_mini * dl = &instance;

    if (dl == NULL) {
        DOWNLOAD_API_ERROR("[%s] [ERROR] invalid dl (NULL) ...!!!\n", __func__);
        return 0;
    }
#ifndef __LINUX__
    mtos_task_info_t info = {0};
    mtos_task_get_info(&info);
    mtos_printk("[%s]  info.prio = %d...!!!\n", __func__, info.prio);

    if(check_task_download_running((int)info.prio))
    {
        abort_download_task((int)info.prio, TRUE);
        remove_task_from_manager((int)info.prio);
    }
    add_task_to_manager((int)info.prio, TRUE, dl);
#endif
    HttpDownloadMiniSpeed loadresult = dl->download(tmpurl, timeoutSec*1000, file, NULL, false, extraHeaders, body, bodyLen,rsp_header);
#ifndef __LINUX__
    remove_task_from_manager((int)info.prio);
#endif
    DOWNLOAD_API_DEBUG("[%s] download result state: [%d] (0:COMPLETE; 1:ERR; 2:TIMEOUT; 3:ABORT; 4:ABORT by user\n", __func__, loadresult.state);
    DOWNLOAD_API_DEBUG("[%s] download size: [%d]\n", __func__, loadresult.size);

    if(loadresult.state == HTTP_MINI_ABORT_BY_USER)
    {
        delete_file(file);
        return -1;
    }

    if(loadresult.state != HTTP_MINI_COMPLETE || loadresult.size< 0)
    {
        delete_file(file);
        return 0;
    }

    DOWNLOAD_API_DEBUG("[%s] end end...\n", __func__);
    return 1;
}

static       BOOL  bAbortFlag = FALSE;

void   Nw_Download_Init_Download_Manager()
{
    http_download_mamager_init();
}

void   Nw_Download_Init(int prio, unsigned char * p_mem, int size)
{
    //http_download * dl = http_download::get_http_manager(prio, p_mem, size);
}

void   Nw_Download_Deinit(void)
{
    //http_download *dl = http_download::get_http_manager(prio,p_mem);
}

void  Nw_Download_SetAbortFlag(BOOL flag)
{
    //bAbortFlag = flag;
}

void  Nw_Download_Abort(http_download_mini * instance)
{
    //http_download_mini * dl = &instance;
    DOWNLOAD_API_DEBUG("[Nw_Download_Abort] start start... ...\n");
    instance->abort();
    DOWNLOAD_API_DEBUG("[Nw_Download_Abort] done done... ...\n");
}
/*
*
*
*   RETURN VALUE:
*         0 means fail and  1 means success -1 means aborted by user
*
*
*
*/
int Nw_DownloadURLTimeout(const char * url,
                          const char * tempFile,
                          unsigned int timeoutSec,
                          void * response,
                          void * arg,
                          const char * extraHeaders,
                          const char * body,
                          unsigned int bodyLen)
{
    DOWNLOAD_API_DEBUG("[%s] start start ...\n", __func__);
    DOWNLOAD_API_DEBUG("[%s] timeout %d seconds\n", __func__, timeoutSec);

    if (url == NULL) {
        DOWNLOAD_API_ERROR("[%s] url is NULL, please check it\n", __func__);
        return 0;
    }
    
    DOWNLOAD_API_DEBUG("[%s] url [%s]...\n", __func__, url);
    char tmpurl[MAX_TMP_URL_LEN];
    char file[MAX_FILE_NAME_LEN];
    memset(tmpurl, 0, MAX_TMP_URL_LEN);
    sprintf(tmpurl, "%s", url);
    memset(file, 0, MAX_FILE_NAME_LEN);
    sprintf(file, "%s", tempFile);
    http_download_mini instance;
    http_download_mini * dl = &instance;

    if (dl == NULL) {
        DOWNLOAD_API_ERROR("[%s] [ERROR] invalid dl (NULL) ...!!!\n", __func__);
        return 0;
    }
#ifndef __LINUX__
    mtos_task_info_t info = {0};
    mtos_task_get_info(&info);
    mtos_printk("[%s]  info.prio = %d...!!!\n", __func__, info.prio);

    if(check_task_download_running((int)info.prio))
    {
        abort_download_task((int)info.prio, TRUE);
        remove_task_from_manager((int)info.prio);
    }
    add_task_to_manager((int)info.prio, TRUE, dl);
#endif	
    HttpDownloadMiniSpeed loadresult = dl->download(tmpurl, timeoutSec*1000, file, NULL, false, extraHeaders, body, bodyLen,NULL);
#ifndef __LINUX__
    remove_task_from_manager((int)info.prio);
#endif
    DOWNLOAD_API_DEBUG("[%s] download result state: [%d] (0:COMPLETE; 1:ERR; 2:TIMEOUT; 3:ABORT; 4:ABORT by user\n", __func__, loadresult.state);
    DOWNLOAD_API_DEBUG("[%s] download size: [%d]\n", __func__, loadresult.size);

    if(loadresult.state == HTTP_MINI_ABORT_BY_USER)
    {
    	 delete_file(file);
        return -1;
    }

    if(loadresult.state != HTTP_MINI_COMPLETE || loadresult.size< 0)
    {
    	 delete_file(file);
        return 0;
    }

    DOWNLOAD_API_DEBUG("[%s] end end...\n", __func__);
    return 1;
}

int Nw_DownloadURLTimeout_Gzip(const char * url,
                          const char * tempFile,
                          unsigned int timeoutSec,
                          void * response,
                          void * arg,
                          const char * extraHeaders,
                          const char * body,
                          unsigned int bodyLen)
{
    DOWNLOAD_API_DEBUG("[%s] start start ...\n", __func__);
    DOWNLOAD_API_DEBUG("[%s] timeout %d seconds\n", __func__, timeoutSec);

    if (url == NULL) {
        DOWNLOAD_API_ERROR("[%s] url is NULL, please check it\n", __func__);
        return 0;
    }

    DOWNLOAD_API_DEBUG("[%s] url [%s]...\n", __func__, url);

    char extraHeaders_t[512] = {0};
    memset(extraHeaders_t, 0, 512);
    DOWNLOAD_API_DEBUG("[%s] extraHeaders [%s]...\n", __func__, extraHeaders);
    if (extraHeaders && strlen(extraHeaders) > 0)
    {
     DOWNLOAD_API_DEBUG("[%s] extraHeaders !=NULL...\n", __func__);
        strcpy(extraHeaders_t, extraHeaders);

        if(extraHeaders[strlen(extraHeaders) - 2] == '\r' && extraHeaders[strlen(extraHeaders) - 1] == '\n') {
            strcat(extraHeaders_t, "Accept-Encoding: gzip\r\n");
        } else {
            strcat(extraHeaders_t, "\r\nAccept-Encoding: gzip\r\n");
        }
    } else {
        strcpy(extraHeaders_t, "Accept-Encoding: gzip\r\n");
    }
    DOWNLOAD_API_DEBUG("[%s] extraHeaders_t [%s]...\n", __func__, extraHeaders_t);

    char temp_gzip_file[64] = {0};
    mtos_task_info_t info = {0};
    mtos_task_get_info(&info);
    sprintf((char*)temp_gzip_file,"r:zlibfile%d",info.prio);
    DOWNLOAD_API_DEBUG("[%s] temp_gzip_file =%s...\n", __func__, temp_gzip_file);

    char responsekey0[32] = "Content-Encoding";
    char responsevalue0[8];
    memset(responsevalue0, 0, sizeof(responsevalue0));
    char responsekey1[32] = "content-encoding";
    char responsevalue1[8];
    memset(responsevalue1, 0, sizeof(responsevalue1));

    HTTP_rsp_header_t * rsp_header = (HTTP_rsp_header_t *)mtos_malloc(sizeof(HTTP_rsp_header_t));
    memset(rsp_header, 0, sizeof(HTTP_rsp_header_t));

    rsp_header->key[0] = responsekey0;
    rsp_header->value_buf[0] = responsevalue0;
    rsp_header->value_buflen[0] = 8;

    rsp_header->key[1] = responsekey1;
    rsp_header->value_buf[1] = responsevalue1;
    rsp_header->value_buflen[1] = 8;

    int req_ret = DownloadAndGetHeader(url, temp_gzip_file, 10,NULL,NULL,extraHeaders_t,NULL,0, rsp_header);
    DOWNLOAD_API_DEBUG("[%s] responsevalue0=[%s], responsevalue1=[%s]...!!!\n", __func__, responsevalue0, responsevalue1);
    DOWNLOAD_API_DEBUG("[%s] req_ret [%d]...\n", __func__, req_ret);
    if(req_ret == 1 && ((responsevalue0 && (strcmp(responsevalue0, "gzip") == 0)) ||(responsevalue1 && (strcmp(responsevalue1, "gzip") == 0))))
    {
        int ret = decompress_to_file(temp_gzip_file, tempFile);
        mtos_free(rsp_header);
        rsp_header = NULL;
	delete_file(temp_gzip_file);

        return ret;
    }
    else
    {
        if(req_ret == 1)
        {
            rename_file(temp_gzip_file, tempFile);
        }
        mtos_free(rsp_header);
        rsp_header = NULL;
        delete_file(temp_gzip_file);

        return req_ret;
    }
}


static void local_strlwr(char *s)
{
	char *str;
	str = s; 
	while(*str != '\0')
	{
		if(*str > 'A' && *str < 'Z'){
			*str += 'a'-'A';
		}
		str++;
	}
//	return s;
}
#ifdef TEST_SPEED_OPEN
static DownloadResult downloadurl_testspeed(const char * url,
                             unsigned int timeoutMS, TestSpeedCtrl * para)
{
    DownloadResult result;
    result.speed= -1;
    result.connectMS= -1;
    result.downloadMS= -1;
    result.errcode= TEST_SPEED_OK;

    DOWNLOAD_API_LOG("[%s] start start ...\n", __func__);
    DOWNLOAD_API_LOG("[%s] timeout %d ms\n", __func__, timeoutMS);

    TEST_SPEED_WHITE_BLACK_LIST*p_hdl = get_live_speed_list_xml_handle();
    //When run this function the first time, we should parse the blacklist which was defined in webset.
    if(!(p_hdl && p_hdl->black_list_count > 0 && p_hdl->black_list[0].value))
    {
        parse_url_white_black_list();
    }
    DOWNLOAD_API_DEBUG("[%s]  black_list_count [%d]  ! \n", __func__, p_hdl->black_list_count);


    if (url == NULL || para == NULL) {
        DOWNLOAD_API_ERROR("[%s][ERROR] url or user prarameters is NULL, please check it\n", __func__);
        result.errcode = ERR_INVALID_PARAM1; 
        return result;
    }

    if(timeoutMS < 1000){
        DOWNLOAD_API_ERROR("[%s][ERROR] no time to do test speed, left time %d ms\n", __func__, timeoutMS);
        result.errcode = ERR_INVALID_TIMEOUT; 
        return result;
    }

    DOWNLOAD_API_DEBUG("[%s] url [%s]...\n", __func__, url);

    char cmpurl[MAX_TMP_URL_LEN];
    memset(cmpurl, 0, MAX_TMP_URL_LEN);
    sprintf(cmpurl, "%s", url);
    local_strlwr(cmpurl) ;

    //Skip the blacklist
    for(int i=0; i < p_hdl->black_list_count; i++)
    {
        if (strstr(cmpurl, p_hdl->black_list[i].value)) {
            DOWNLOAD_API_ERROR("we find %s in this url ,so skip it\n", p_hdl->black_list[i].value);
            result.speed = -1;
            result.errcode = (TEST_SPEED_ERR_T)(p_hdl->black_list[i].errorcode);
            return result;
        }
    }

    char tmpurl[MAX_TMP_URL_LEN];
    HttpDownloadMiniSpeed loadresult;
    memset(tmpurl, 0, MAX_TMP_URL_LEN);
    sprintf(tmpurl, "%s", url);
    http_download_mini instance;
    http_download_mini * dl = &instance;

    if (dl == NULL) {
        DOWNLOAD_API_ERROR("[%s] [ERROR] invalid dl (NULL) ...!!!\n", __func__);
        result.errcode = ERR_INVALID_DL_HDL; 
        return result;
    }

    mtos_task_info_t info = {0};
    mtos_task_get_info(&info);
    if(check_task_download_running((int)info.prio))
    {
        abort_download_task((int)info.prio, TRUE);
        remove_task_from_manager((int)info.prio);
    }
    add_task_to_manager((int)info.prio, TRUE, dl);

    u32 speedstart = mtos_ticks_get();
    loadresult = dl->download(tmpurl, timeoutMS, para->writeFileName, &(para->abort_flag), false, NULL, NULL, 0,NULL);
    u32 speedend = mtos_ticks_get();

    remove_task_from_manager((int)info.prio);

    if(loadresult.state == HTTP_MINI_ABORT_BY_USER)
    {
        DOWNLOAD_API_ERROR("[%s] [ERROR] Abort by user ...!!!\n", __func__);
        result.errcode = ERR_ABORT_BY_USER;  //exception 
        return result;
    }

    u32 cost = (speedend - speedstart) * 10;
	
    if(loadresult.downloadMS > 0 && loadresult.size > 0) {
		
        result.speed = loadresult.size/loadresult.downloadMS;
    }
    else if (loadresult.size < 0 && loadresult.downloadMS < 0) {
		
       result.speed = -1;
     }
    else {
        result.speed = 0;
    }

	
    result.connectMS= loadresult.connectMS;
    result.downloadMS= loadresult.downloadMS;
	
    DOWNLOAD_API_DEBUG("[%s] result.speed = %d\n", __func__, result.speed);
    DOWNLOAD_API_DEBUG("[%s] result.connectMS = %ld\n", __func__, result.connectMS);
    DOWNLOAD_API_DEBUG("[%s] result.downloadMS = %ld\n", __func__, result.downloadMS);

    if(result.speed < 0 && result.connectMS < 0 && result.downloadMS < 0)
    {
        result.errcode = ERR_DL_EXCEPTION;  //exception
        return result;
    }

    bool isLive = true;
    char * pNew = (char *)dl->get_direct_ts_url_from_m3u8(&isLive);
    //we check whetherit is a live stream in m3u8
    if (para->skipNonLiveStream && !isLive) {
        DOWNLOAD_API_ERROR("[%s] [WARNNING] this url is not a live stream, we skip it!\n", __func__);
        result.speed = -1;
        result.errcode = ERR_INVALID_LIVE_URL; 
        return result;
    }

    if (pNew) {
        if (timeoutMS <= cost) {
           DOWNLOAD_API_DEBUG("[%s] timeoutMS <= downloadMS...,   result.downloadMS = %d\n", __func__, result.downloadMS);
           result.connectMS= timeoutMS;
           result.downloadMS= 0;
           result.speed = 0;
           result.errcode = ERR_TEST_SPEED_TIMEOUT; 	   
           return result;
        }
        DOWNLOAD_API_DEBUG("[%s] we should use direct ts url from m3u8 to redo download... timeout = %d ms\n", __func__, timeoutMS - cost);
        return Nw_DownloadURL_TestSpeed(pNew, timeoutMS - cost, para);
    }

    char * pFinal = (char *)dl->get_final_url();

    if (pFinal == NULL) {
        DOWNLOAD_API_ERROR("we can't get final url!!!\n");
        result.speed = -1;
        result.errcode = ERR_NOT_FIND_VALID_URL; 
        return result;
    }

    memset(cmpurl, 0, MAX_TMP_URL_LEN);
    sprintf(cmpurl, "%s", pFinal);
    local_strlwr(cmpurl) ;

    if (para->skipNonLiveStream || para->skipNonMediaUrl) {
        ////should check the tmpurl is a valid url !
        DOWNLOAD_API_DEBUG("[%s]  the  final url is %s  ! \n", __func__, pFinal);
        //Skip the blacklist
        for(int i=0; i < p_hdl->black_list_count; i++)
        {
            if (strstr(cmpurl, p_hdl->black_list[i].value)) {
                DOWNLOAD_API_ERROR("we find %s in this url ,so skip it\n", p_hdl->black_list[i].value);
                result.speed = -1;
                result.errcode = (TEST_SPEED_ERR_T)(p_hdl->black_list[i].errorcode);
                return result;
            }
        }

        if(para->skipNonMediaUrl)
        {
            /////fix to remove the url which like http://www.qq.com ,
            ////maybe we hava a url , it is like xxx.TS xxx.Ts 
            if (strstr(cmpurl, "ts") 
                || strstr(cmpurl, "flv")
                || strstr(cmpurl, "mp4") 
                || strstr(cmpurl, "/live") /// like http://125.39.29.120:8629/live/ningxia
               ) {
                DOWNLOAD_API_DEBUG("this url is a  a ts/flv/mp4/live url ,return the speed  ! \n");
            } else {
                DOWNLOAD_API_ERROR("this url is a not a ts/flv/mp4/live url ,so skip it\n");
                result.speed = -1;
                result.errcode = ERR_NO_NORMAL_LIVE_URL; 		
                return result;
            }
        }
    }

    DOWNLOAD_API_DEBUG("[%s] speed[%d] result.downloadMS [%d]..\n", __func__, result.speed, result.downloadMS);
    DOWNLOAD_API_DEBUG("[%s] end end...\n", __func__);
	
    return result;

	
}

DownloadResult Nw_DownloadURL_TestSpeed(const char * url,
                             unsigned int timeoutMS, TestSpeedCtrl * para)
{

    int connecttimes = 0;
    DownloadResult result;

    do {
        result = downloadurl_testspeed(url, timeoutMS, para);
        connecttimes ++;
        if(result.errcode == ERR_ABORT_BY_USER)
        {
            return result;
        }

        if(result.speed < 0 && result.downloadMS <= 0) {
            DOWNLOAD_API_DEBUG("[%s] Reconnect ... ...\n", __func__);
            mtos_task_sleep(3000);
            continue;
        }
        else
        {
            return result;
        }
    } while(connecttimes < 3);

    return result;
}
#endif

#ifdef DOWNLOAD_HTTP_NEW
#define DEFAULT_TIME_OUT   30
static int getExtraHeaders(char *pHeader, unsigned int header_buffer_size, HTTP_spec_info_t *p_http_spec_info)
{    
    unsigned int used_len = 0;
    memset(pHeader,0,header_buffer_size);
    if(p_http_spec_info != NULL)
    {
        if(p_http_spec_info->cookie_len >0)
        {
            if((strlen(p_http_spec_info->p_cookie)+strlen("Cookie: %s\r\n")+strlen(pHeader)) > header_buffer_size)
                return -1;
            
            snprintf (pHeader+used_len, header_buffer_size-used_len, "Cookie: %s\r\n", p_http_spec_info->p_cookie);
            used_len = strlen(pHeader);
        }

        if(p_http_spec_info->content_type_len)
        {
            if((strlen(p_http_spec_info->p_content_type)+strlen("Content-Type: %s\r\n")+strlen(pHeader)) > header_buffer_size)
                return -1;
            
            snprintf (pHeader+used_len, header_buffer_size-used_len, "Content-Type: %s\r\n", p_http_spec_info->p_content_type);
            used_len = strlen(pHeader);
        }
        if(p_http_spec_info->x_request_len)
        {
            if((strlen(p_http_spec_info->p_x_request)+strlen("X-Requested-With: %s\r\n")+strlen(pHeader)) > header_buffer_size)
                return -1;
            
            snprintf (pHeader+used_len, header_buffer_size-used_len, "X-Requested-With: %s\r\n", p_http_spec_info->p_x_request);
            used_len = strlen(pHeader);
        }

        if(p_http_spec_info->user_agent_len != 0)
        {
            if((strlen("User-Agent: %s\r\n")+8+strlen(pHeader)) > header_buffer_size)
                return -1;
            
            snprintf (pHeader+used_len, header_buffer_size-used_len, "User-Agent: %s\r\n", p_http_spec_info->p_user_agent);
            used_len = strlen(pHeader);
        }
    }

    return 0;
}
static int getContentType(HTTP_header_t *http_hdr, http_download_mini* dl)
{
    if(!dl || !http_hdr)
        return -1;

    const char *prefix = "Content-Type: ";
    const char *pContentype = dl->get_ContentType_from_response();
    int contentypeLen = strlen(prefix)+strlen(pContentype);
    
    HTTP_field_t *new_field = (HTTP_field_t*)mtos_malloc(sizeof(HTTP_field_t));
    if(!new_field){
        DOWNLOAD_API_ERROR("[%s] not enough memory to malloc HTTP_field_t\n", __func__);
        return -2;
    }

    new_field->next = NULL;
    new_field->field_name = (char *)mtos_malloc(contentypeLen+2);
    if( new_field->field_name==NULL )
    {
        mtos_free(new_field);
        return -3;
    }

    memset(new_field->field_name,0,contentypeLen+2);
    strcpy(new_field->field_name, prefix);
    strcat(new_field->field_name,pContentype);
    
    if(http_hdr->last_field==NULL ) 
    {
        http_hdr->first_field = new_field;
    } 
    else
    {
        http_hdr->last_field->next = new_field;
    }
    http_hdr->last_field = new_field;
    http_hdr->field_nb++;

    return 0;
}
int chunkhttp_download_common_start(char *p_url, HTTP_header_t **p_outbuf,  HTTP_spec_info_t *p_http_spec_info)
{
    HTTP_header_t *p1 = (HTTP_header_t*)mtos_malloc(sizeof(HTTP_header_t));

    if(!p1){
        DOWNLOAD_API_ERROR("[%s] not enough memory to malloc HTTP_header_t\n", __func__);
        return -7;
    }
    
    memset(p1,0,sizeof(HTTP_header_t));

     if (p_url == NULL) {
        DOWNLOAD_API_ERROR("[%s] url is NULL, please check it\n", __func__);
        mtos_free(p1);
        return MTHTTP_UNKNOWN_ERROR;
    }
        
    char tmpurl[MAX_TMP_URL_LEN];
    memset(tmpurl, 0, MAX_TMP_URL_LEN);
    sprintf(tmpurl, "%s", p_url);

    http_download_mini* dl = new http_download_mini();
    if(!dl){
        DOWNLOAD_API_ERROR("[%s] malloc download instance failed\n", __func__);
        mtos_free(p1);
        return MTHTTP_UNKNOWN_ERROR;
    }

    BOOL isGetOrPost = FALSE;
    unsigned int extraheader_len = 0;
    unsigned int timeout = DEFAULT_TIME_OUT;//sec
    char *pPost = NULL;
    unsigned int post_len = 0;
    if(p_http_spec_info){
        isGetOrPost = (p_http_spec_info->p_post_value != NULL);
        extraheader_len = p_http_spec_info->content_type_len
            + p_http_spec_info->cookie_len
            + p_http_spec_info->x_request_len
            + p_http_spec_info->user_agent_len
            + 256;
        timeout = p_http_spec_info->timeout;

        if(isGetOrPost){
            pPost = p_http_spec_info->p_post_value;
            post_len = p_http_spec_info->post_value_len;
        }
    }

    char *extraheader = NULL;

    if(extraheader_len > 256){
        extraheader = (char*)mtos_malloc(extraheader_len);
        if(!extraheader){
            DOWNLOAD_API_ERROR("[%s] not enough memory to malloc extraheader\n", __func__);
            mtos_free(p1);
            delete dl;
            return -7;
        }
        memset(extraheader,0,extraheader_len);
        getExtraHeaders(extraheader,extraheader_len,p_http_spec_info);
    }

    
    int content_len  = dl->http_connect(tmpurl, timeout*1000, isGetOrPost, extraheader, pPost, post_len);
    if(content_len < 0){
        DOWNLOAD_API_ERROR("[%s] connect error\n", __func__);
        delete dl;
        mtos_free(p1);
        if(extraheader)
            mtos_free(extraheader);
        return MTHTTP_UNKNOWN_ERROR;
    }else if(content_len == 0){
        DOWNLOAD_API_ERROR("[%s] can not get Content-length after connect\n", __func__);
    }

    if(extraheader)
        mtos_free(extraheader);
    
    p1->content_length = content_len;

    getContentType(p1,dl);

    *p_outbuf = p1;
    return ((int)dl&0x7fffffff);//because up-layer will use this return value to check >=0 or <0 ,so address need a little change
}
int chunkhttp_download_start(char *p_url, HTTP_header_t **p_outbuf)
{
    return chunkhttp_download_common_start(p_url,p_outbuf,NULL);
}
int  chunkhttp_recv(int fd, char *response, unsigned int size, unsigned int isChunked)
{
    if(fd == 0)
        return -1;
    
    http_download_mini* dl = (http_download_mini*)(fd|0x80000000);///refer to chunkhttp_download_common_start return value
    int written_len = MTHTTP_UNKNOWN_ERROR;
    
    int status = dl->http_receive((unsigned char*)response, size, &written_len);
    if(status == -2)
        written_len = MTHTTP_UNKNOWN_ERROR;

    return written_len;
}
void chunkhttp_close(int fd)
{
    if(fd == 0)
        return;
    
    http_download_mini* dl = (http_download_mini*)(fd|0x80000000);///refer to chunkhttp_download_common_start return value

    dl->http_close();
    delete dl;

    return;
}
int mthttp_download_common_start(char *p_url, HTTP_header_t **p_outbuf, HTTP_spec_info_t *p_http_spec_info)
{
#define HTTP_BUF_INCREASE_SIZE (1024*128)
    int fd = chunkhttp_download_common_start(p_url,p_outbuf,p_http_spec_info);
    if(fd <= 0)
        return fd;

    HTTP_header_t *p1 = *p_outbuf;
    if(!p1)
        return MTHTTP_UNKNOWN_ERROR;

    char *buf_ptr = NULL;
    int buf_size;
    int buf_used = 0;
    buf_ptr = (char*)mtos_malloc(HTTP_BUF_INCREASE_SIZE);
    if(!buf_ptr)
        return -1;
    buf_size = HTTP_BUF_INCREASE_SIZE;
        
    unsigned int expect_size = 0;
    int ret;
    expect_size = p1->content_length;
    if(!expect_size)
        expect_size = 0xffffffff;
    while(buf_used < expect_size)
    {
        ret = chunkhttp_recv(fd,buf_ptr+buf_used,buf_size-buf_used,0);
        if(ret <= 0){
            break;
        }
        else{
            buf_used += ret;
            if(buf_used >= buf_size-16){
                char *tmp = (char*)mtos_malloc(HTTP_BUF_INCREASE_SIZE+buf_size);
                if(!tmp)
                    break;

                memcpy(tmp,buf_ptr,buf_used);
                mtos_free(buf_ptr);

                buf_ptr = tmp;
                buf_size += HTTP_BUF_INCREASE_SIZE;
            }
        }
    }

    p1->buffer = buf_ptr;
    p1->buffer_size = buf_size;
    p1->body = p1->buffer;
    p1->body_size = buf_used;

    return 0;
}
#endif

void* Nw_Http_Download_Start(const char * url, BOOL isGetOrPost, const char * postbuffer, unsigned int bufferLen, 
    unsigned int timeoutSec, HttpDownloadHeader *header, char *extraheader)
{
    if (url == NULL) {
        DOWNLOAD_API_ERROR("[%s] url is NULL, please check it\n", __func__);
        return NULL;
    }
    
    if (header == NULL) {
        DOWNLOAD_API_ERROR("[%s] header response buffer is NULL, please check it\n", __func__);
        return NULL;
    }
    
    char tmpurl[MAX_TMP_URL_LEN];
    memset(tmpurl, 0, MAX_TMP_URL_LEN);
    sprintf(tmpurl, "%s", url);
    
    http_download_mini* dl = new http_download_mini();
    if(!dl){
        DOWNLOAD_API_ERROR("[%s] malloc download instance failed\n", __func__);
        return NULL;
    }

    int content_len  = dl->http_connect(tmpurl, timeoutSec*1000, isGetOrPost, extraheader, postbuffer, bufferLen);
    if(content_len < 0){
        DOWNLOAD_API_ERROR("[%s] connect error\n", __func__);
        delete dl;
        return NULL;
    }else if(content_len == 0){
        DOWNLOAD_API_ERROR("[%s] can not get Content-length after connect\n", __func__);
    }

    header->Content_length = content_len;
    header->redirect_url = dl->get_redirect_url();
    return dl;
}

HttpDownloadResult Nw_Http_Download_Recv(void *handle, unsigned char *response, unsigned int buffersize)
{
    HttpDownloadResult result;
    http_download_mini* dl = (http_download_mini*)handle;

    result.readLen = 0;

    result.status = dl->http_receive(response, buffersize, &(result.readLen));
        
    return result;
}

void Nw_Http_Download_Stop(void *handle)
{
    if(!handle)
        return;
    
    http_download_mini* dl = (http_download_mini*)handle;

    dl->http_close();
    delete dl;
    
    return;
}
/*
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
int  Nw_DownloadURL_POST(const char * url,
                          const char * postbuffer,
                          unsigned int bufferLen,
                          const char * tempFile,
                          const char * extraHeaders,
                          const char * checkHeader,
                          const char * serviceProvider)
{
    if (checkHeader != NULL) {
        // TODO:
    }
    if (url == NULL) {
        DOWNLOAD_API_ERROR("[%s] url is NULL, please check it\n", __func__);
        return 0;
    }

    char tmpurl[MAX_TMP_URL_LEN];
    memset(tmpurl, 0, MAX_TMP_URL_LEN);
    sprintf(tmpurl, "%s", url);
    http_download_mini instance;
    http_download_mini * dl = &instance;
    unsigned char abort = 0;
    

    if (dl == NULL) {
        DOWNLOAD_API_ERROR("[%s] [ERROR] invalid dl (NULL) ...!!!\n", __func__);
        return 0;
    }
#ifndef __LINUX__
    mtos_task_info_t info = {0};
    mtos_task_get_info(&info);
    if(check_task_download_running((int)info.prio))
    {
        abort_download_task((int)info.prio, TRUE);
        remove_task_from_manager((int)info.prio);
    }
    add_task_to_manager((int)info.prio, TRUE, dl);
#endif
    HttpDownloadMiniSpeed loadresult = dl->download((char *)tmpurl, 15 * 1000, (char *)tempFile, &abort, true, extraHeaders, postbuffer, bufferLen,NULL);
#ifndef __LINUX__
    remove_task_from_manager((int)info.prio);
#endif
    DOWNLOAD_API_DEBUG("[%s] download result state: [%d] (0:COMPLETE; 1:ERR; 2:TIMEOUT; 3:ABORT; 4:ABORT by user\n", __func__, loadresult.state);
    DOWNLOAD_API_DEBUG("[%s] download size: [%d]\n", __func__, loadresult.size);

    if(loadresult.state == HTTP_MINI_ABORT_BY_USER)
    {
       delete_file((char *)tempFile);
        return -1;
    }

    if(loadresult.state != HTTP_MINI_COMPLETE || loadresult.size< 0)
    {
    	 delete_file((char *)tempFile);
        return 0;
    }
    
    return 1;
}


/*
*
* 
*
*/
int  Nw_DownloadURL_POST_Gzip(const char * url,
                          const char * postbuffer,
                          unsigned int bufferLen,
                          const char * tempFile,
                          const char * extraHeaders,
                          const char * checkHeader,
                             const char * serviceProvider,
                             unsigned int timeoutSec)///seconds
{
    if (checkHeader != NULL) {
        // TODO:
    }
    if (url == NULL) {
        DOWNLOAD_API_ERROR("[%s] url is NULL, please check it\n", __func__);
        return 0;
    }

    char extraHeaders_t[512] = {0};
    memset(extraHeaders_t, 0, 512);
    DOWNLOAD_API_DEBUG("[%s] extraHeaders [%s]...\n", __func__, extraHeaders);
    if (extraHeaders && strlen(extraHeaders) > 0)
    {
	    DOWNLOAD_API_DEBUG("[%s] extraHeaders !=NULL...\n", __func__);
        strcpy(extraHeaders_t, extraHeaders);

        if(extraHeaders[strlen(extraHeaders) - 2] == '\r' && extraHeaders[strlen(extraHeaders) - 1] == '\n') {
            strcat(extraHeaders_t, "Accept-Encoding: gzip\r\n");
        } else {
            strcat(extraHeaders_t, "\r\nAccept-Encoding: gzip\r\n");
        }
    } else {
        strcpy(extraHeaders_t, "Accept-Encoding: gzip\r\n");
    }
    DOWNLOAD_API_DEBUG("[%s] extraHeaders_t [%s]...\n", __func__, extraHeaders_t);

    char temp_gzip_file[64] = {0};
    mtos_task_info_t info = {0};
    mtos_task_get_info(&info);
    sprintf((char*)temp_gzip_file,"r:zlibfile%d",info.prio);
    DOWNLOAD_API_DEBUG("[%s] temp_gzip_file =%s...\n", __func__, temp_gzip_file);

    char responsekey0[32] = "Content-Encoding";
    char responsevalue0[8];
    memset(responsevalue0, 0, sizeof(responsevalue0));
    char responsekey1[32] = "content-encoding";
    char responsevalue1[8];
    memset(responsevalue1, 0, sizeof(responsevalue1));

    HTTP_rsp_header_t * rsp_header = (HTTP_rsp_header_t *)mtos_malloc(sizeof(HTTP_rsp_header_t));
    memset(rsp_header, 0, sizeof(HTTP_rsp_header_t));

    rsp_header->key[0] = responsekey0;
    rsp_header->value_buf[0] = responsevalue0;
    rsp_header->value_buflen[0] = 8;

    rsp_header->key[1] = responsekey1;
    rsp_header->value_buf[1] = responsevalue1;
    rsp_header->value_buflen[1] = 8;

    int req_ret = Nw_DownloadURL_POST_ex2(url, postbuffer, bufferLen,temp_gzip_file,extraHeaders_t,rsp_header,timeoutSec);
    DOWNLOAD_API_DEBUG("[%s] req_ret [%d]...\n", __func__, req_ret);
    DOWNLOAD_API_DEBUG("[%s] responsevalue0=[%s], responsevalue1=[%s]...\n", __func__, responsevalue0, responsevalue1);
    if(req_ret == 1 && ((responsevalue0 && (strcmp(responsevalue0, "gzip") == 0)) ||(responsevalue1 && (strcmp(responsevalue1, "gzip") == 0))))
    {
        int ret = decompress_to_file(temp_gzip_file, tempFile);
        mtos_free(rsp_header);
        rsp_header = NULL;
        delete_file(temp_gzip_file);

        return ret;
    }
    else
    {
        if(req_ret == 1)
        {
            rename_file(temp_gzip_file, tempFile);
        }
        mtos_free(rsp_header);
        rsp_header = NULL;
        delete_file(temp_gzip_file);

        return req_ret;
    }
}


int  Nw_DownloadURL_POST_ex(const char * url,
                             const char * postbuffer,
                             unsigned int bufferLen,
                             const char * tempFile,
                             const char * extraHeaders,
                             const char * checkHeader,
                             const char * serviceProvider,
                             unsigned int timeoutSec)///seconds
{
    if (checkHeader != NULL) {
        // TODO:
    }
    if (url == NULL) {
        DOWNLOAD_API_ERROR("[%s] url is NULL, please check it\n", __func__);
        return 0;
    }

    char tmpurl[MAX_TMP_URL_LEN];
    memset(tmpurl, 0, MAX_TMP_URL_LEN);
    sprintf(tmpurl, "%s", url);
    http_download_mini instance;
    http_download_mini * dl = &instance;
    unsigned char abort = 0;
    

    if (dl == NULL) {
        DOWNLOAD_API_ERROR("[%s] [ERROR] invalid dl (NULL) ...!!!\n", __func__);
        return 0;
    }
#ifndef __LINUX__
    mtos_task_info_t info = {0};
    mtos_task_get_info(&info);
    if(check_task_download_running((int)info.prio))
    {
        abort_download_task((int)info.prio, TRUE);
        remove_task_from_manager((int)info.prio);
    }
    add_task_to_manager((int)info.prio, TRUE, dl);
#endif
    HttpDownloadMiniSpeed loadresult = dl->download((char *)tmpurl, timeoutSec * 1000, (char *)tempFile, &abort, true, extraHeaders, postbuffer, bufferLen,NULL);
#ifndef __LINUX__
    remove_task_from_manager((int)info.prio);
#endif
    DOWNLOAD_API_DEBUG("[%s] download result state: [%d] (0:COMPLETE; 1:ERR; 2:TIMEOUT; 3:ABORT; 4:ABORT by user\n", __func__, loadresult.state);
    DOWNLOAD_API_DEBUG("[%s] download size: [%d]\n", __func__, loadresult.size);

    if(loadresult.state == HTTP_MINI_ABORT_BY_USER)
    {
    	delete_file((char *)tempFile);
        return -1;
    }

    if(loadresult.state != HTTP_MINI_COMPLETE || loadresult.size< 0)
    {
    	 delete_file((char *)tempFile);
        return 0;
    }
    
    return 1;
}
int  Nw_DownloadURL_POST_ex2(const char * url,
                             const char * postbuffer,
                             unsigned int bufferLen,
                             const char * tempFile,
                             const char * extraHeaders,
                             HTTP_rsp_header_t *rsp_header,
                             unsigned int timeoutSec)///seconds
{
    if (url == NULL) {
        DOWNLOAD_API_ERROR("[%s] url is NULL, please check it\n", __func__);
        return 0;
    }

    char tmpurl[MAX_TMP_URL_LEN];
    memset(tmpurl, 0, MAX_TMP_URL_LEN);
    sprintf(tmpurl, "%s", url);
    http_download_mini instance;
    http_download_mini * dl = &instance;
    unsigned char abort = 0;
    

    if (dl == NULL) {
        DOWNLOAD_API_ERROR("[%s] [ERROR] invalid dl (NULL) ...!!!\n", __func__);
        return 0;
    }
#ifndef __LINUX__
    mtos_task_info_t info = {0};
    mtos_task_get_info(&info);
    if(check_task_download_running((int)info.prio))
    {
        abort_download_task((int)info.prio, TRUE);
        remove_task_from_manager((int)info.prio);
    }
    add_task_to_manager((int)info.prio, TRUE, dl);
#endif
    HttpDownloadMiniSpeed loadresult = dl->download((char *)tmpurl, timeoutSec * 1000, (char *)tempFile, &abort, true, extraHeaders, postbuffer, bufferLen,rsp_header);
#ifndef __LINUX__
    remove_task_from_manager((int)info.prio);
#endif
    DOWNLOAD_API_DEBUG("[%s] download result state: [%d] (0:COMPLETE; 1:ERR; 2:TIMEOUT; 3:ABORT; 4:ABORT by user\n", __func__, loadresult.state);
    DOWNLOAD_API_DEBUG("[%s] download size: [%d]\n", __func__, loadresult.size);

    if(loadresult.state == HTTP_MINI_ABORT_BY_USER)
    {
        return -1;
    }

    if(loadresult.state != HTTP_MINI_COMPLETE || loadresult.size< 0)
    {
        return 0;
    }
    
    return 1;
}
void Abort_Download_Task(int task_prio, BOOL abort_flag)
{
    abort_download_task(task_prio,abort_flag);
}
#define    RAM_FS_USE  1  ////yiyuan add this , use the ramfs for prj

#ifdef RAM_FS_USE

#ifndef __LINUX__
#define   DAILYMOTION_VIDEO_INFO_FILE_PATH     "r:yd_vid.html"
#define   DAILYMOTION_VIDEO_URL_PATH           "r:yd_vid_url.html"
#define   CLIPSYNDICATE_VIDEO_JS_PLAYER        "r:clip_js_player.html"
#define   CLIPSYNDICATE_PLAYER_URL             "r:clip_player_url.xml"
#define   YOUTUBE_VIDEO_INFO_FILE_PATH         "r:get_video_info"
#define   YOUTUBE_WATCH_PAGE                   "r:youtube_watch_page"
#define   YOUTUBE_HTML5PLAYER_JS               "r:vevo_html5_js"
#define   YAHOO_WATCH_PAGE                     "r:yahoo_watch_page"
#define   YAHOO_VIDEO_JSON                     "r:yahoo_video_json"
#define   ADDANIME_WATCH_PAGE                  "r:addanime_watch_page"
#define   YOUKU_WATCH_PAGE                     "r:youku_watch_page"
#else
#define   DAILYMOTION_VIDEO_INFO_FILE_PATH     "yd_vid.html"
#define   DAILYMOTION_VIDEO_URL_PATH           "yd_vid_url.html"
#define   CLIPSYNDICATE_VIDEO_JS_PLAYER        "clip_js_player.html"
#define   CLIPSYNDICATE_PLAYER_URL             "clip_player_url.xml"
#define   YOUTUBE_VIDEO_INFO_FILE_PATH         "get_video_info"
#define   YOUTUBE_WATCH_PAGE                   "youtube_watch_page"
#define   YOUTUBE_HTML5PLAYER_JS               "vevo_html5_js"
#define   YAHOO_WATCH_PAGE                     "yahoo_watch_page"
#define   YAHOO_VIDEO_JSON                     "yahoo_video_json"
#define   ADDANIME_WATCH_PAGE                  "addanime_watch_page"
#define   YOUKU_WATCH_PAGE                     "youku_watch_page"
#endif
#else
#define   DAILYMOTION_VIDEO_INFO_FILE_PATH     "yd_vid.html"
#define   DAILYMOTION_VIDEO_URL_PATH           "yd_vid_url.html"
#define   CLIPSYNDICATE_VIDEO_JS_PLAYER        "clip_js_player.html"
#define   CLIPSYNDICATE_PLAYER_URL             "clip_player_url.xml"
#define   YOUTUBE_VIDEO_INFO_FILE_PATH         "get_video_info"
#define   YOUTUBE_WATCH_PAGE                   "youtube_watch_page"
#define   YOUTUBE_HTML5PLAYER_JS               "vevo_html5_js"
#define   YAHOO_WATCH_PAGE                     "yahoo_watch_page"
#define   YAHOO_VIDEO_JSON                     "yahoo_video_json"
#define   ADDANIME_WATCH_PAGE                  "addanime_watch_page"
#define   YOUKU_WATCH_PAGE                     "youku_watch_page"
#endif

int get_request_size(u8 size)
{
    int request_size = 0;
    if((size&0x1) == QEQUEST_VIDEO_LD)
    {
        request_size += QEQUEST_VIDEO_QCIF + QEQUEST_VIDEO_CIF;
    }
    if((size&0x2) == QEQUEST_VIDEO_SD)
    {
        request_size += QEQUEST_VIDEO_D1 + QEQUEST_VIDEO_720P;
    }
    if((size&0x4) == QEQUEST_VIDEO_HD)
    {
        if((size&0x2) == QEQUEST_VIDEO_SD)
            request_size += QEQUEST_VIDEO_1080P;
        else
            request_size += QEQUEST_VIDEO_720P + QEQUEST_VIDEO_1080P;
    }
    return request_size;
}

int video_size_to_pos(int size)
{
    int pos = 5;
    switch (size)
    {
		case QEQUEST_VIDEO_QCIF:
            pos = 0;
            break;
        case QEQUEST_VIDEO_CIF:
            pos = 1;
            break;
        case QEQUEST_VIDEO_D1:
            pos = 2;
            break;
        case QEQUEST_VIDEO_720P:
            pos = 3;
            break;
        case QEQUEST_VIDEO_1080P:
            pos = 4;
            break;
        default:
            break;
    }
    return pos;
}

int video_width_to_pos(int width)
{
    if(width < 177)
        return 0;
    else if(width < 361)
        return 1;
    else if(width < 721)
        return 2;
    else if(width < 1281)
        return 3;
    else
        return 4;
    return 1;
}


int check_is_vevo(u8 website, char* tmp_buf2)
{
    int ret = 0;
    char *   p_url_start = NULL;
	char  *  p_url_end = NULL;
    if(website == WEBSITE_DAILYMOTION)
    {
        p_url_start = strstr(tmp_buf2, "rel=\"video_src\"");
        if(p_url_start == NULL)
            return ret;
        p_url_start = p_url_start + strlen("rel=\"video_src\"");
        p_url_end = strstr(tmp_buf2, ">");
        char p_video_src[DOWNLOAD_URL_MAX_LEN];
        memset(p_video_src, 0 ,DOWNLOAD_URL_MAX_LEN);
        memcpy(p_video_src, p_url_start, (p_url_end-p_url_start));
        if(p_url_start = strstr(tmp_buf2, "vevo"))
        {
            DOWNLOAD_API_WARNING("[%s][WARNING] the file is vevo[%s]\n ", __func__, p_video_src);
            ret = 1;
            return ret;
        }

    }
    return ret;
}

int json_url_decode(char *str, int len) {
	char *dest = str;
	char *data = str;

	while (len--) {
        if (*data == '\\' && len >= 1 ) {
			data += 1;
            continue;
		} else {
			*dest = *data;
		}

		data++;
		dest++;
	}

	*dest = '\0';
	return dest - str;
}

int dailymotion_parse_url(const char *buf, request_url_t * p_request_url, u8 size)
{
    /*
    _FORMATS = [
            (u'stream_h264_ld_url', u'ld'),
            (u'stream_h264_url', u'standard'),
            (u'stream_h264_hq_url', u'hq'),
            (u'stream_h264_hd_url', u'hd'),
            (u'stream_h264_hd1080_url', u'hd180'),
        ]
    */
    char *   p_url_start = NULL;
	char  *  p_url_end = NULL;
    int   ret = -1;
    int   input_len = 0;

    if((size&0x1) == QEQUEST_VIDEO_LD)
    {
        p_url_start = strstr(buf, "\"stream_h264_ld_url\":\"");
        if(p_url_start != NULL)
        {
            p_url_start = p_url_start + strlen("\"stream_h264_ld_url\":\"");
            p_url_end = strstr(p_url_start, "\"");
            if(p_url_end != NULL)
            {
                memset(p_request_url->playUrlArray[0], 0, DOWNLOAD_URL_MAX_LEN);
    		    memcpy(p_request_url->playUrlArray[0], p_url_start, (p_url_end-p_url_start));
                input_len = strlen(p_request_url->playUrlArray[0]);
                json_url_decode(p_request_url->playUrlArray[0], input_len);
                ret = 0;
            }
        }
    }
    if((size&0x2) == QEQUEST_VIDEO_SD)
    {
        p_url_start = strstr(buf, "\"stream_h264_url\":\"");
        if(p_url_start != NULL)
        {
            p_url_start = p_url_start + strlen("\"stream_h264_url\":\"");
            p_url_end = strstr(p_url_start, "\"");
            if(p_url_end != NULL)
            {
                memset(p_request_url->playUrlArray[1], 0, DOWNLOAD_URL_MAX_LEN);
    		    memcpy(p_request_url->playUrlArray[1], p_url_start, (p_url_end-p_url_start));
                ret = 0;
                input_len = strlen(p_request_url->playUrlArray[1]);
                json_url_decode(p_request_url->playUrlArray[1], input_len);
            }
        }
    }
    if((size&0x4) == QEQUEST_VIDEO_HD)
    {
        p_url_start = strstr(buf, "\"stream_h264_hd_url\":\"");
        if(p_url_start != NULL)
        {
            p_url_start = p_url_start + strlen("\"stream_h264_hd_url\":\"");
            p_url_end = strstr(p_url_start, "\"");
            if(p_url_end != NULL)
            {
                memset(p_request_url->playUrlArray[2], 0, DOWNLOAD_URL_MAX_LEN);
    		    memcpy(p_request_url->playUrlArray[2], p_url_start, (p_url_end-p_url_start));
                input_len = strlen(p_request_url->playUrlArray[2]);
                json_url_decode(p_request_url->playUrlArray[2], input_len);
                ret = 0;
            }
        }
        else
        {
            p_url_start = strstr(buf, "\"stream_h264_hd1080_url\":\"");
            if(p_url_start != NULL)
            {
                p_url_start = p_url_start + strlen("\"stream_h264_hd1080_url\":\"");
                p_url_end = strstr(p_url_start, "\"");
                if(p_url_end != NULL)
                {
                    memset(p_request_url->playUrlArray[2], 0, DOWNLOAD_URL_MAX_LEN);
        		    memcpy(p_request_url->playUrlArray[2], p_url_start, (p_url_end-p_url_start));
                    input_len = strlen(p_request_url->playUrlArray[2]);
                    json_url_decode(p_request_url->playUrlArray[2], input_len);
                    ret = 0;
                }
            }
        }
    }
    return ret;
}

int dailymotion_get_video_play_url(const char * url, request_url_t * p_request_url, u8 size)
{
    ///get videoid
    char *   p_url_start = NULL;
	char  *  p_url_end = NULL;
    int  par_ret = 0;
	int download_retry = 0;
    int ret;
    char p_videoid[MAX_VIDEOID_LEN];
    memset(p_videoid, 0, MAX_VIDEOID_LEN);
    p_url_start = strstr(url, "/video/");
    if(p_url_start == NULL)
    {
        DOWNLOAD_API_WARNING("[%s][WARNING] ipunt dailymotion url[%s] maybe wrong...\n ", __func__, url);
        return -2;
    }
    p_url_start = p_url_start + strlen("/video/");
    p_url_end = strstr(url, "_");
    if(p_url_end == NULL)
    {
        p_url_end = (char*)(url+strlen(url));
        memcpy(p_videoid, p_url_start, (p_url_end-p_url_start+1));
    }
    else
        memcpy(p_videoid, p_url_start, (p_url_end-p_url_start));

    char tmp_buf[1280] = {0};
	memset(tmp_buf, 0, 1280);
    snprintf(tmp_buf, 1200, "http://www.dailymotion.com/video/%s", p_videoid);

	for(download_retry = 0; download_retry < MAX_POST_COUNT; download_retry++)
	{
		ret = Nw_DownloadURLTimeout(tmp_buf, DAILYMOTION_VIDEO_INFO_FILE_PATH, DEFAULT_DL_HTML_TIMEOUT,NULL,NULL,NULL,NULL,0);

		if (ret == 1 || ret == -1) {
			break;
		}
	}
	if(download_retry == 4 || ret == -1)
	{
		DOWNLOAD_API_ERROR("[%s][ERROR] do nothing, fail to download yd_vid.html ...\n ", __func__);
		return -1;
	}

#ifdef __LINUX__
	FILE * fp = NULL;
	fp = fopen("/mnt/yd_vid.html", "rb");
	char * tmp_buf2 = NULL;

	if (fp) {
		DOWNLOAD_API_LOG("[%s]  open yd_vid.html ok !!!!\n", __func__);
		tmp_buf2 = (char *)mtos_malloc(GET_VIDEO_INFO_MAX_SIZE);

		if (tmp_buf2) {
			memset(tmp_buf2, 0, GET_VIDEO_INFO_MAX_SIZE);
			int rdNum = fread(tmp_buf2, 1, GET_VIDEO_INFO_MAX_SIZE, fp);

			if (rdNum > 0) {
				DOWNLOAD_API_LOG("[%s] read %d bytes from get_video_info!!!\n", __func__, rdNum);
			}

			fclose(fp);
			fp = NULL;
            //check is vevo
			par_ret = check_is_vevo(WEBSITE_DAILYMOTION, tmp_buf2);

			if (par_ret != 0)
			{
                mtos_free(tmp_buf2);
				return par_ret;
			}
		}
        else
            return -1;
	}

#else
	ufs_file_t  *fp1 = (ufs_file_t  *)mtos_malloc(sizeof(ufs_file_t));//= stream->fd;
	if(fp1 ==NULL)
		return -1;
	memset(fp1, 0, sizeof(ufs_file_t));

	unsigned short  path_tmp[256]={0};
	unsigned short * p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)DAILYMOTION_VIDEO_INFO_FILE_PATH,path_tmp);

	u8 fs_res = ufs_open(fp1, p_filename1, (op_mode_t)(UFS_READ | UFS_OPEN)) ;
	char * tmp_buf2 = NULL;

	if ((fs_res == UFS_OK) ||(fs_res == UFS_EXIST)) {
		DOWNLOAD_API_LOG("[%s]  open yd_vid.html ok !!!!\n", __func__);
		tmp_buf2 = (char *)mtos_malloc(GET_VIDEO_INFO_MAX_SIZE);
		if(tmp_buf2 == NULL)
		{
			ufs_close(fp1);
			mtos_free(fp1);
			return -1;
		}
		if (tmp_buf2) {
			memset(tmp_buf2, 0, GET_VIDEO_INFO_MAX_SIZE);
			int rdNum = 0;
			ufs_read(fp1, tmp_buf2, GET_VIDEO_INFO_MAX_SIZE, (u32*)&rdNum);
			ufs_close(fp1);

			par_ret = check_is_vevo(WEBSITE_DAILYMOTION, tmp_buf2);
            if (par_ret != 0)
            {
                mtos_free(fp1);
                mtos_free(tmp_buf2);
                return par_ret;
            }
        }
	}
#endif
    delete_file(DAILYMOTION_VIDEO_INFO_FILE_PATH);


    //get ld sd hd url!!!
    memset(tmp_buf, 0, 1280);
    snprintf(tmp_buf, 1200, "http://www.dailymotion.com/embed/video/%s", p_videoid);

	for(download_retry = 0; download_retry < MAX_POST_COUNT; download_retry++)
	{
		ret = Nw_DownloadURLTimeout(tmp_buf, DAILYMOTION_VIDEO_URL_PATH, DEFAULT_DL_HTML_TIMEOUT,NULL,NULL,NULL,NULL,0);

		if (ret == 1 || ret == -1) {
			break;
		}
	}
	if(download_retry == 4 || ret == -1)
	{
		DOWNLOAD_API_ERROR("[%s][ERROR] do nothing, fail to download yd_vid.html ...\n ", __func__);
		return -1;
	}

#ifdef __LINUX__
	fp = fopen("/mnt/yd_vid_url.html", "rb");

	if (fp) {
		DOWNLOAD_API_LOG("[%s]  open yd_vid_url.html ok !!!!\n", __func__);

		if (tmp_buf2) {
			memset(tmp_buf2, 0, GET_VIDEO_INFO_MAX_SIZE);
			int rdNum = fread(tmp_buf2, 1, GET_VIDEO_INFO_MAX_SIZE, fp);

			if (rdNum > 0) {
				DOWNLOAD_API_LOG("[%s] read %d bytes from get_video_info!!!\n", __func__, rdNum);
			}

			fclose(fp);
			fp = NULL;
			par_ret = dailymotion_parse_url(tmp_buf2, p_request_url, size);
			mtos_free(tmp_buf2);

			if (par_ret != 0)
				return par_ret;
		}
	}

#else
	memset(fp1, 0, sizeof(ufs_file_t));

	p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)DAILYMOTION_VIDEO_URL_PATH,path_tmp);

	fs_res = ufs_open(fp1, p_filename1, (op_mode_t)(UFS_READ | UFS_OPEN)) ;

	if ((fs_res == UFS_OK) ||(fs_res == UFS_EXIST)) {
		DOWNLOAD_API_LOG("[%s]  open yd_vid_url.html ok !!!!\n", __func__);

		if (tmp_buf2) {
			memset(tmp_buf2, 0, GET_VIDEO_INFO_MAX_SIZE);
			int rdNum = 0;
			ufs_read(fp1, tmp_buf2, GET_VIDEO_INFO_MAX_SIZE, (u32*)&rdNum);
			ufs_close(fp1);
            mtos_free(fp1);

			par_ret = dailymotion_parse_url(tmp_buf2, p_request_url, size);
			mtos_free(tmp_buf2);

			if (par_ret != 0)
				return par_ret;
		}
	}
    else
    {
        mtos_free(fp1);
        mtos_free(tmp_buf2);
    }
#endif

    delete_file(DAILYMOTION_VIDEO_URL_PATH);
    return 0;
}

int clipsyndicate_parse_url(const char *buf, request_url_t * p_request_url)
{
    /*
    <track>
    <identifier>5235029</identifier>
    <restricted>0</restricted>
    <location>http://video-static.clipsyndicate.com/zStorage/clipsyndicate/247/2014/07/21/05/28/20140721052855.1813670.mp4?wpid=0&lrid=1333</location>
    */
    char *   p_url_start = NULL;
	char  *  p_url_end = NULL;
    int   ret = -1;

    p_url_start = strstr(buf, "<track");
    if(p_url_start != NULL)
    {
        p_url_start = p_url_start + strlen("<track");
        p_url_start = strstr(p_url_start, "<location>");
        if(p_url_start != NULL)
        {
            p_url_start = p_url_start + strlen("<location>");
            p_url_end = strstr(p_url_start, "</location>");
            if(p_url_end != NULL)
            {
                memset(p_request_url->playUrlArray[1], 0, DOWNLOAD_URL_MAX_LEN);
    		    memcpy(p_request_url->playUrlArray[1], p_url_start, (p_url_end-p_url_start));
                ret = 0;
            }
        }
    }
    return ret;
}

int clipsyndicate_get_flvars(char *data_buf, char *p_flvars)
{
    //flvars at the end of buf.
    char *start = data_buf + strlen(data_buf) - 500;
    char *   p_flvars_start = NULL;
	char  *  p_flvars_end = NULL;
    int   ret = -1;

    p_flvars_start = strstr(start, "flvars: \"");
    if(p_flvars_start != NULL)
    {
        p_flvars_start = p_flvars_start + strlen("flvars: \"");
        p_flvars_end = strstr(p_flvars_start, "\"");
        if(p_flvars_end != NULL)
        {
		    memcpy(p_flvars, p_flvars_start, (p_flvars_end-p_flvars_start));
            ret = 0;
        }
    }
    return ret;
}

int clipsyndicate_get_video_play_url(const char * p_videoid, request_url_t * p_request_url)
{
    ///get videoid
    int  par_ret = 0;
	int download_retry = 0;
    int ret;

    char tmp_buf[1280] = {0};
	memset(tmp_buf, 0, 1280);
    snprintf(tmp_buf, 1200, "http://eplayer.clipsyndicate.com/embed/player.js?va_id=%s", p_videoid);

	for(download_retry = 0; download_retry < MAX_POST_COUNT; download_retry++)
	{
		ret = Nw_DownloadURLTimeout(tmp_buf, CLIPSYNDICATE_VIDEO_JS_PLAYER, DEFAULT_DL_HTML_TIMEOUT,NULL,NULL,NULL,NULL,0);

		if (ret == 1 || ret == -1) {
			break;
		}
	}
	if(download_retry == 4 || ret == -1)
	{
		DOWNLOAD_API_ERROR("[%s][ERROR] do nothing, fail to download yd_vid.html ...\n ", __func__);
		return -1;
	}

    char p_flvars[1024];
    memset(p_flvars, 0, 1024);
     //get
#ifdef __LINUX__
	FILE * fp = NULL;
	fp = fopen("/mnt/clip_js_player.html", "rb");
	char * tmp_buf2 = NULL;

	if (fp) {
		DOWNLOAD_API_LOG("[%s]  open yd_vid.html ok !!!!\n", __func__);
		tmp_buf2 = (char *)mtos_malloc(GET_VIDEO_INFO_MAX_SIZE);

		if (tmp_buf2) {
			memset(tmp_buf2, 0, GET_VIDEO_INFO_MAX_SIZE);
			int rdNum = fread(tmp_buf2, 1, GET_VIDEO_INFO_MAX_SIZE, fp);

			if (rdNum > 0) {
				DOWNLOAD_API_LOG("[%s] read %d bytes from get_video_info!!!\n", __func__, rdNum);
			}

			fclose(fp);
			fp = NULL;
            //check is vevo

			par_ret = clipsyndicate_get_flvars(tmp_buf2, p_flvars);

			if (par_ret != 0)
			{
                mtos_free(tmp_buf2);
				return par_ret;
			}
		}
        else
            return -1;
	}

#else
	ufs_file_t  *fp1 = (ufs_file_t  *)mtos_malloc(sizeof(ufs_file_t));//= stream->fd;
	if(fp1 ==NULL)
		return -1;
	memset(fp1, 0, sizeof(ufs_file_t));

	unsigned short  path_tmp[256]={0};
	unsigned short * p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)CLIPSYNDICATE_VIDEO_JS_PLAYER,path_tmp);

	u8 fs_res = ufs_open(fp1, p_filename1, (op_mode_t)(UFS_READ | UFS_OPEN)) ;
	char * tmp_buf2 = NULL;

	if ((fs_res == UFS_OK) ||(fs_res == UFS_EXIST)) {
		DOWNLOAD_API_LOG("[%s]  open yd_vid.html ok !!!!\n", __func__);
		tmp_buf2 = (char *)mtos_malloc(GET_VIDEO_INFO_MAX_SIZE);
		if(tmp_buf2 == NULL)
		{
			ufs_close(fp1);
			mtos_free(fp1);
			return -1;
		}
		if (tmp_buf2) {
			memset(tmp_buf2, 0, GET_VIDEO_INFO_MAX_SIZE);
			int rdNum = 0;
			ufs_read(fp1, tmp_buf2, GET_VIDEO_INFO_MAX_SIZE, (u32*)&rdNum);
			ufs_close(fp1);

            par_ret = clipsyndicate_get_flvars(tmp_buf2, p_flvars);
            DOWNLOAD_API_DEBUG("[%s] par_ret[%d] p_flvars[%s]...\n ", __func__, par_ret, p_flvars);
            if (par_ret != 0)
            {
                mtos_free(fp1);
                mtos_free(tmp_buf2);
                return par_ret;
            }
        }
	}
#endif

    delete_file(CLIPSYNDICATE_VIDEO_JS_PLAYER);
    //get url!!!
    memset(tmp_buf, 0, 1280);
    snprintf(tmp_buf, 1200, "http://eplayer.clipsyndicate.com/osmf/playlist?%s", p_flvars);

	for(download_retry = 0; download_retry < MAX_POST_COUNT; download_retry++)
	{
		ret = Nw_DownloadURLTimeout(tmp_buf, CLIPSYNDICATE_PLAYER_URL, DEFAULT_DL_HTML_TIMEOUT,NULL,NULL,NULL,NULL,0);

		if (ret == 1 || ret == -1) {
			break;
		}
	}
	if(download_retry == 4 || ret == -1)
	{
		DOWNLOAD_API_ERROR("[%s][ERROR] do nothing, fail to download yd_vid.html ...\n ", __func__);
		return -1;
	}

#ifdef __LINUX__
	fp = fopen("/mnt/clip_player_url.xml", "rb");

	if (fp) {
		DOWNLOAD_API_LOG("[%s]  open yd_vid_url.html ok !!!!\n", __func__);

		if (tmp_buf2) {
			memset(tmp_buf2, 0, GET_VIDEO_INFO_MAX_SIZE);
			int rdNum = fread(tmp_buf2, 1, GET_VIDEO_INFO_MAX_SIZE, fp);

			if (rdNum > 0) {
				DOWNLOAD_API_LOG("[%s] read %d bytes from get_video_info!!!\n", __func__, rdNum);
			}

			fclose(fp);
			fp = NULL;
			par_ret = clipsyndicate_parse_url(tmp_buf2, p_request_url);
			mtos_free(tmp_buf2);

			if (par_ret != 0)
				return par_ret;
		}
	}

#else
	memset(fp1, 0, sizeof(ufs_file_t));

	p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)CLIPSYNDICATE_PLAYER_URL,path_tmp);

	fs_res = ufs_open(fp1, p_filename1, (op_mode_t)(UFS_READ | UFS_OPEN)) ;

	if ((fs_res == UFS_OK) ||(fs_res == UFS_EXIST)) {
		DOWNLOAD_API_LOG("[%s]  open yd_vid_url.html ok !!!!\n", __func__);

		if (tmp_buf2) {
			memset(tmp_buf2, 0, GET_VIDEO_INFO_MAX_SIZE);
			int rdNum = 0;
			ufs_read(fp1, tmp_buf2, GET_VIDEO_INFO_MAX_SIZE, (u32*)&rdNum);
			ufs_close(fp1);
            mtos_free(fp1);

			par_ret = clipsyndicate_parse_url(tmp_buf2, p_request_url);
			mtos_free(tmp_buf2);

			if (par_ret != 0)
				return par_ret;
		}
	}
    else
    {
        mtos_free(fp1);
        mtos_free(tmp_buf2);
    }
#endif

    delete_file(CLIPSYNDICATE_PLAYER_URL);
    return 0;
}

int  parse_url_list(request_url_t * p_request_url, u8 video_size)
{
    //qcif cif->ld, d1 720->sd, 720 1080->hd
    if((video_size&0x1) == QEQUEST_VIDEO_LD)
    {
        //preferentially choose cif
        if(strlen(p_request_url->playUrlArray[1]) > 0)
        {
            memset(p_request_url->playUrlArray[0], 0, DOWNLOAD_URL_MAX_LEN);
            memcpy(p_request_url->playUrlArray[0], p_request_url->playUrlArray[1], strlen(p_request_url->playUrlArray[1]));
        }
    }
    if((video_size&0x2) == QEQUEST_VIDEO_SD)
    {
        //preferentially choose d1
        if(strlen(p_request_url->playUrlArray[2]) > 0)
        {
            memset(p_request_url->playUrlArray[1], 0, DOWNLOAD_URL_MAX_LEN);
            memcpy(p_request_url->playUrlArray[1], p_request_url->playUrlArray[2], strlen(p_request_url->playUrlArray[2]));
        }
        else if(strlen(p_request_url->playUrlArray[1]) > 0)
        {
            //do nothing, select the cif.
        }
    }
    if((video_size&0x4) == QEQUEST_VIDEO_HD)
    {
        //preferentially choose 720P
        memset(p_request_url->playUrlArray[2], 0, DOWNLOAD_URL_MAX_LEN);
        if(strlen(p_request_url->playUrlArray[3]) > 0)
        {
            memcpy(p_request_url->playUrlArray[2], p_request_url->playUrlArray[3], strlen(p_request_url->playUrlArray[3]));
        }
        else if(strlen(p_request_url->playUrlArray[4]) > 0)
        {
            memcpy(p_request_url->playUrlArray[2], p_request_url->playUrlArray[4], strlen(p_request_url->playUrlArray[4]));
        }
    }
    memset(p_request_url->playUrlArray[3], 0, DOWNLOAD_URL_MAX_LEN);
    memset(p_request_url->playUrlArray[4], 0, DOWNLOAD_URL_MAX_LEN);

    char * decrypt_url = NULL;
    if(strlen(p_request_url->playUrlArray[1]) > 0)
    {
        decrypt_url = p_request_url->playUrlArray[1];
    }
    else if(strlen(p_request_url->playUrlArray[2]) > 0)
    {
        decrypt_url = p_request_url->playUrlArray[2];
    }
    else if(strlen(p_request_url->playUrlArray[0]) > 0)
    {
        decrypt_url = p_request_url->playUrlArray[0];
    }


    if(decrypt_url == NULL)
       return -4;

#if 0//only for debug
	if (strlen(decrypt_url) > 0) {
		DOWNLOAD_API_DEBUG("The following is h264 encoder url !!!\n");
		DOWNLOAD_API_DEBUG("***********************************************\n");
		DOWNLOAD_API_DEBUG("***********************************************\n");
#ifdef __LINUX__
		FILE * fp_url = fopen("./url.txt", "wb");

		//printf("[%d]:\n", i);
		//printf("[%s]:\n", decrypt_url);
		fwrite("<film>", strlen("<film>"), 1, fp_url);
		fwrite(decrypt_url, strlen(decrypt_url), 1, fp_url);
		fwrite("</film>", strlen("</film>"), 1, fp_url);

		fclose(fp_url);
		fp_url = NULL;
#else
		ufs_file_t  *fp1 = (ufs_file_t  *)mtos_malloc(sizeof(ufs_file_t));//= stream->fd;
		memset(fp1, 0, sizeof(ufs_file_t));

		unsigned short  path_tmp[256]={0};
		unsigned short * p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)"r:url.txt",path_tmp);


		if (ufs_open(fp1, p_filename1, (op_mode_t)(UFS_READ | UFS_WRITE | UFS_CREATE_NEW_COVER)) != 0) {
			OS_PRINTF("[%s] fail to %s !!!\n", __func__, "url.txt");
			OS_PRINTF("[%s] fail to open local file !!!\n", __func__);
			return -1;
		}

		//printf("[%d]:\n",i);
		//printf("[%s]:\n",decrypt_url);
		int ret;
		ufs_write(fp1, (void *)"<film>", (u32)strlen("<film>"), (u32*)&ret) ;
		//return ;
		ufs_write(fp1, (void *)decrypt_url, (u32)strlen(decrypt_url), (u32*)&ret);
		//return ;
		ufs_write(fp1, (void *)"</film>", (u32)strlen("</film>"), (u32*)&ret) ;//
		//return ;

		ufs_close(fp1);
		mtos_free(fp1);
		fp1 = NULL;
#endif

	} else {
		DOWNLOAD_API_ERROR(">>>> Sorry we can't find the specified url !!!!!!!!!!!<<<<<\n");
		return -1;
	}
#endif
    return 0;
}

static  itag_detail_t itag_table[] = {

	{5, "FLV", "240p", "Sorenson H.263", "mp3"},
	{6, "FLV", "270p", "Sorenson H.263", "mp3"},
	{13, "3GP", "N/A", "MPEG-4 Visual", "AAC"},
	{17, "3GP", "144p", "MPEG-4 Visual", "AAC"},
	{18, "MP4", "270p/360p", "H.264", "AAC"},
	{22, "MP4", "720p", "H.264 High", "AAC"},
	{34, "FLV", "360p", "H.264 Main", "AAC"},
	{35, "FLV", "480p", "H.264 Main", "AAC"},
	{36, "3GP", "240p", "MPEG-4 Visual", "AAC"},
	{37, "MP4", "1080P", "H.264 High", "AAC"},
	{38, "MP4", "3072P", "H.264 High", "AAC"},
	{43, "WebM", "360P", "VP8", "Vorbis"},
	{44, "WebM", "480P", "VP8", "Vorbis"},
	{45, "WebM", "720p", "VP8", "Vorbis"},
	{46, "WebM", "1080p", "VP8", "Vorbis"},
	{82, "MP4", "360p", "H.264 3D", "AAC"},
	{83, "MP4", "240p", "H.264 3D", "AAC"},
	{84, "MP4", "720p", "H.264 3D", "AAC"},
	{85, "MP4", "520p", "H.264", "AAC"},
	{100, "WebM", "360p", "VP8", "Vorbis"},
	{101, "WebM", "360p", "VP8", "Vorbis"},
	{102, "WebM", "720p", "VP8", "Vorbis"},
	{120, "FLV", "720p", "AVC Main@L3.1", "AAC"},
};

extern "C" {

	void  print_itag_detail(int val) {
		int i = 0;
		int len = sizeof(itag_table) / sizeof(itag_detail_t);

		for (i = 0; i < len; i++) {
			if (itag_table[i].val == val) {
				DOWNLOAD_API_DEBUG("[%s %s %s %s]\n",
						itag_table[i].container, itag_table[i].resolution, itag_table[i].video_enc, itag_table[i].audio_enc);
			}
		}

		return;
	}
}
static int php_htoi(char *s) {
	int value;
	int c;
	c = ((unsigned char *)s)[0];

	if (isupper(c))
		c = tolower(c);

	value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;
	c = ((unsigned char *)s)[1];

	if (isupper(c))
		c = tolower(c);

	value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;
	return (value);
}


int php_url_decode(char *str, int len) {
	char *dest = str;
	char *data = str;

	while (len--) {
		if (*data == '+') {
			*dest = ' ';

		} else if (*data == '%' && len >= 2 && isxdigit((int) *(data + 1)) && isxdigit((int) *(data + 2))) {
			*dest = (char) php_htoi(data + 1);
			data += 2;
			len -= 2;

		} else {
			*dest = *data;
		}

		data++;
		dest++;
	}

	*dest = '\0';
	return dest - str;
}

//functionType: 1->a.reverse(); 2->{var c=a[0];a[0]=a[b%a.length];a[b]=c};
//3->a.splice(0,b); 4->a.slice(b)
#define MAX_VAR_FUNCTION_NUM 10
typedef  struct {
    int            functionNum;
	char           functionName[MAX_VAR_FUNCTION_NUM][12];
	unsigned char  functionType[MAX_VAR_FUNCTION_NUM];
} var_class_t;

//functionType: 1->a.reverse(); 2->{var c=a[0];a[0]=a[b%a.length];a[b]=c};
//3->a.splice(0,b); 4->a.slice(b)
int parse_function_type(char *function_str)
{
    if(strstr(function_str, "reverse"))
    {
        return 1;
    }
    else if(strstr(function_str, "length"))
    {
        return 2;
    }
    else if(strstr(function_str, "splice"))
    {
        return 3;
    }
    else if(strstr(function_str, "slice"))
    {
        return 4;
    }

    return 0;
}

void parse_var_class_function(char *var_class_str, var_class_t *cur_var)
{
    char tmp_str[64];
    char *p_start = var_class_str;
    char *p_end;
    while(p_end=strstr(p_start, ":function"))
    {
        memcpy(cur_var->functionName[cur_var->functionNum], p_start, (p_end-p_start));
        p_start = p_end + strlen(":function");
        p_end = strstr(p_start, "}");
        memset(tmp_str, 0, 64);
        memcpy(tmp_str, p_start, p_end-p_start);
        cur_var->functionType[cur_var->functionNum] = parse_function_type(tmp_str);
        DOWNLOAD_API_DEBUG("[%s] functionNum[%d] functionName[%s] functionType[%d]\n",
            __func__, cur_var->functionNum, cur_var->functionName[cur_var->functionNum], cur_var->functionType[cur_var->functionNum]);
        cur_var->functionNum++;
        p_start = p_end + 2;
    }
    return;
}

void reverse_str(char *encrypt_s)
{
    char tmp_str[SIG_STR_MAX_LEN];
    int len = strlen(encrypt_s);
    memcpy(tmp_str, encrypt_s, len);

    char *p_dst = encrypt_s;
    char *p_src = tmp_str + len - 1;
    while(len > 0)
    {
        *(p_dst++) = *(p_src--);
        len--;
    }
    return;
}

//var c=a[0];a[0]=a[b%a.length];a[b]=c
void swap_str(char *encrypt_s, int tmp_para)
{
    char tmp = *encrypt_s;
    int b = (tmp_para)%(strlen(encrypt_s));
    *encrypt_s = *(encrypt_s + b);
    *(encrypt_s+b) = tmp;
    return;
}

void splice_str(char *encrypt_s, int tmp_para)
{
    char tmp_str[SIG_STR_MAX_LEN];
    memset(tmp_str, 0, SIG_STR_MAX_LEN);
    char *p_start = encrypt_s + tmp_para;
    int len = strlen(encrypt_s);
    memcpy(tmp_str, p_start, (len-tmp_para));

    memset(encrypt_s, 0, SIG_STR_MAX_LEN);
    memcpy(encrypt_s, tmp_str, strlen(tmp_str));
    return;
}

//src="//s.ytimg.com/yts/jsbin/html5player-zh_CN-vflI-pVN8/html5player.js"
int get_html5player_name(char *buf, char *html5player)
{
    int  par_ret = -1;
    char *p_start = NULL;
    char *p_end = NULL;
    p_start = strstr(buf, "html5player-");
    if(p_start)
    {
        p_end = strstr(p_start, ".js");
        if(p_end)
        {
            p_end = p_end + strlen(".js");
            p_start = p_start - 28;
            p_start = strstr(p_start, "s.ytimg");
            if(p_start)
            {
                memcpy(html5player, p_start, (p_end-p_start));
                int len = strlen(html5player);
                json_url_decode(html5player, len);
                DOWNLOAD_API_DEBUG("[%s] html5player[%s]\n", __func__, html5player);
                par_ret = 0;
            }
        }
    }
    return par_ret;
}

int getDecyptSig(char *js_data, char *encrypt_s)
{
    char *p_start = NULL;
    char *p_end = NULL;
    int  i = 0;
    p_start = strstr(js_data, ".sig");
    if(p_start == NULL)
    {
        DOWNLOAD_API_ERROR("[%s] Get signature= function failed!!!\n", __func__);
        return -1;
    }
    p_start = strstr(p_start, "{");
    p_start = strstr(p_start, "||");
    p_start = p_start + strlen("||");
    p_end = strstr(p_start, "(");
    char functionName[8];
    memset(functionName, 0, 8);
    memcpy(functionName, p_start, (p_end-p_start));
    DOWNLOAD_API_DEBUG("Decypt functionName:%s.\n", functionName);

    char tmp_str[32];
    memset(tmp_str, 0, 32);
    sprintf(tmp_str, ";function %s(", functionName);

    p_start = strstr(js_data, tmp_str);
    if(p_start == NULL)
    {
        DOWNLOAD_API_ERROR("[%s] Get signature function failed!!!\n", __func__);
        return -1;
    }
    p_start = p_start + strlen(tmp_str);
    p_end = strstr(p_start, "};function");
    char tmp_strFunciton[256];
    memset(tmp_strFunciton, 0, 256);
    DOWNLOAD_API_DEBUG("Funciton len[%d] \n", (p_end-p_start));
    if((p_end-p_start) > 256)
    {
        p_end = strstr(p_start, "};");
    }
    if((p_end-p_start) > 256)
    {
        DOWNLOAD_API_DEBUG("getDecyptSig: function len is too long, normal is 120, current[%d]. \n", (p_end-p_start));
        return -1;
    }
    memcpy(tmp_strFunciton, p_start, (p_end-p_start));
    DOWNLOAD_API_DEBUG("tmp_strFunciton len[%d], str:%s. \n", strlen(tmp_strFunciton), tmp_strFunciton);

    ///find var Go
    //a=a.split("");Go.sH(a,2)
    p_start = strstr(tmp_strFunciton, ".split(");
    if(p_start == NULL)
    {
        DOWNLOAD_API_ERROR("[%s] Get split function failed!!!\n", __func__);
        return -1;
    }
    p_start = p_start + strlen(".split(");
    p_start = strstr(p_start, "\");");
    p_start = p_start + 3;
    p_end = strstr(p_start, ".");

    char var_class_name[16];
    memset(var_class_name, 0, 16);
    if((p_end-p_start) > 16)
    {
        DOWNLOAD_API_DEBUG("getDecyptSig: function len is too long, current[%d]. \n", (p_end-p_start));
        return -1;
    }
    memcpy(var_class_name, p_start, (p_end-p_start));
    //DOWNLOAD_API_DEBUG("var_class:%s\n", var_class_name);

    memset(tmp_str, 0, 32);
    sprintf(tmp_str, ";var %s={", var_class_name);
    p_start = strstr(js_data, tmp_str);
    if(p_start == NULL)
    {
        DOWNLOAD_API_ERROR("[%s] Get var class failed!!!\n", __func__);
        return -1;
    }
    p_start = p_start + strlen(tmp_str);
    p_end = strstr(p_start, "}};function");
    if(p_end == NULL)
    {
        DOWNLOAD_API_ERROR("[%s] Get %s function failed!!!\n", __func__, tmp_str);
        return -1;
    }
    p_end = p_end + 3;
    char var_class_str[256];
    memset(var_class_str, 0, 256);
    if((p_end-p_start) > 256)
    {
        DOWNLOAD_API_DEBUG("getDecyptSig: function len is too long, current[%d]. \n", (p_end-p_start));
        return -1;
    }
    memcpy(var_class_str, p_start, (p_end-p_start));
    //DOWNLOAD_API_DEBUG("var_class:%s\n", var_class_str);

    var_class_t cur_var;
    memset(&cur_var, 0, sizeof(var_class_t));
    parse_var_class_function(var_class_str, &cur_var);

    //read tmp_strFunciton
    memset(tmp_str, 0, 32);
    sprintf(tmp_str, "%s.", var_class_name);
    int tmp_para = 0;
    char tmp_function_str[8];
    char tmp_para_str[8];
    p_start = tmp_strFunciton;
    //char tmp[10] = "abcdefg";
    //splice_str(tmp, 2);
    while(p_start=strstr(p_start, tmp_str))
    {
        p_start = p_start + strlen(tmp_str);
        p_end = strstr(p_start, "(");
        memset(tmp_function_str, 0 ,8);
        memcpy(tmp_function_str, p_start, (p_end-p_start));

        p_start=strstr(p_start, ",");
        p_start += 1;
        p_end = strstr(p_start, ")");
        memset(tmp_para_str, 0 ,8);
        memcpy(tmp_para_str, p_start, (p_end-p_start));
        tmp_para = atoi(tmp_para_str);
        //DOWNLOAD_API_DEBUG("zx tmp_function_str[%s] tmp_para_str[%s] tmp_para[%d]\n",
        //    tmp_function_str, tmp_para_str, tmp_para);

        unsigned char tmp_fucntion_type = 0;
        for(i=0; i<cur_var.functionNum; i++)
        {
            if(strcmp(cur_var.functionName[i], tmp_function_str) == 0)
            {
                tmp_fucntion_type = cur_var.functionType[i];
            }
        }

        switch (tmp_fucntion_type)
        {
            //functionType: 1->a.reverse(); 2->{var c=a[0];a[0]=a[b%a.length];a[b]=c};
            //3->a.splice(0,b); 4->a.slice(b)
    		case 1:
                reverse_str(encrypt_s);
                break;
            case 2:
                swap_str(encrypt_s, tmp_para);
                break;
            case 3:
                splice_str(encrypt_s, tmp_para);
                break;
            case 4:
                splice_str(encrypt_s, tmp_para);//current is same. return obj[argvals[0]:]
                break;
            default:
                DOWNLOAD_API_ERROR("[%s] unsupport function type!!\n", __func__);
                break;
        }
        p_start = p_end ;
    }/**/

    return 0;
}

int get_decrypt_js_data(char * urlenc_data, const char * p_videoid, char *p_tmp_js_buf)
{
    //first download watch page, then html5player js.
    int  par_ret = 0;
    int ret;
	int download_retry = 0;
	char tmp_buf[1280] = {0};
    char html5player[128];
    memset(html5player, 0 , 128);

	par_ret = get_html5player_name(urlenc_data, html5player);

	if (par_ret != 0)
	{
        DOWNLOAD_API_ERROR("[%s] get_html5player_name failed!!!\n", __func__);
        return par_ret;
	}

    memset(tmp_buf, 0, 1280);
    snprintf(tmp_buf, 1200, "http://%s", html5player);

    for(download_retry = 0; download_retry < MAX_POST_COUNT; download_retry++)
    {
        ret = Nw_DownloadURLTimeout(tmp_buf, YOUTUBE_HTML5PLAYER_JS, DEFAULT_DL_HTML_TIMEOUT,NULL,NULL,NULL,NULL,0);

        if (ret == 1 || ret == -1) {
            break;
        }
    }

    if(download_retry == 4 || ret == -1)
    {
        delete_file(YOUTUBE_HTML5PLAYER_JS);
        DOWNLOAD_API_ERROR("[%s][ERROR] do nothing, fail to download vevo_html5_js ...\n ", __func__);
        return -1;
    }

#ifdef __LINUX__
    FILE * fp = NULL;
    fp = fopen("/mnt/vevo_html5_js", "rb");

    if (fp) {
        DOWNLOAD_API_LOG("[%s]  open vevo_html5_js ok !!!!\n", __func__);


        if (p_tmp_js_buf) {
            int rdNum = fread(p_tmp_js_buf, 1, JS_INFO_BUF_LEN, fp);

            if (rdNum > 0) {
                DOWNLOAD_API_LOG("[%s] read %d bytes from vevo_html5_js!!!\n", __func__, rdNum);
            }

            fclose(fp);
            fp = NULL;
        }
    }

#else
    ufs_file_t  *fp1 = (ufs_file_t  *)mtos_malloc(sizeof(ufs_file_t));//= stream->fd;
    if(fp1 ==NULL)
        return -1;

    memset(fp1, 0, sizeof(ufs_file_t));

    unsigned short  path_tmp[256]={0};
    unsigned short * p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)YOUTUBE_HTML5PLAYER_JS,path_tmp);

    u8 fs_res = ufs_open(fp1, p_filename1, (op_mode_t)(UFS_READ | UFS_OPEN)) ;

    if ((fs_res == UFS_OK) ||(fs_res == UFS_EXIST)) {
        DOWNLOAD_API_LOG("[%s]  open vevo_html5_js ok !!!!\n", __func__);

        if (p_tmp_js_buf) {
            int rdNum = 0;
            ufs_read(fp1, p_tmp_js_buf, JS_INFO_BUF_LEN, (u32*)&rdNum);
            ufs_close(fp1);
            mtos_free(fp1);
            if (rdNum > 0) {
				DOWNLOAD_API_LOG("[%s] read %d bytes from vevo_html5_js!!!\n", __func__, rdNum);
			}
        }
    }
    else
    {
        mtos_free(fp1);
    }
#endif

    return par_ret;
}

int parse_ext_m3u_list(char * urlenc_data, request_url_t * p_request_url)
{
    char *   p_url_start = NULL;
	char  *  p_url_end = NULL;
    char  resolution_str[QUALITY_STR_MAX_LEN];
    int width, height;
    if (p_url_start = strstr(urlenc_data, "RESOLUTION="))
	{
        while(p_url_start != NULL)
        {
            p_url_start = p_url_start + strlen("RESOLUTION=");
            p_url_end = strstr(p_url_start, "\n");
            if(!p_url_end)
                break;
            memset(resolution_str, 0 ,QUALITY_STR_MAX_LEN);
            memcpy(resolution_str, p_url_start, p_url_end - p_url_start);
            int res = sscanf(resolution_str, "%dx%2d", &width, &height);
            if (res < 2) {
                DOWNLOAD_API_ERROR("[%s][ERROR] get resolution failed! resolution_str[%s]\n", __func__, resolution_str);
                return 1;
            }
            DOWNLOAD_API_DEBUG("[%s] zx resolution width[%d] height[%d]\n", __func__, width, height);

            //video_width_to_pos
            int pos = video_width_to_pos(width);
            p_url_start = p_url_end + 1;
            p_url_end = strstr(p_url_start, "\n");
            if(strlen(p_request_url->playUrlArray[pos]) == 0)
            {
                memset(p_request_url->playUrlArray[pos], 0, DOWNLOAD_URL_MAX_LEN);
                if(p_url_end)
                {
                    memcpy(p_request_url->playUrlArray[pos] , p_url_start, (p_url_end - p_url_start));
                }
                else
                {
                    p_url_end = urlenc_data + strlen(urlenc_data);
                    memcpy(p_request_url->playUrlArray[pos] , p_url_start, (p_url_end - p_url_start + 1));
                    break;
                }

                DOWNLOAD_API_LOG("[%s] width[%d] url[%s]\n", __func__, width, p_request_url->playUrlArray[pos]);
            }

            p_url_start = strstr(p_url_end, "RESOLUTION=");
        }
    }
    else
    {
        return 1;
    }

    return 0;
}

#define NO_YOUTUBE_HLS_ERROR 10
int  parse_youtube_live_list(const char * p_videoid, parse_youtube_vedio_info_t * p_video_info, char * urlenc_data, request_url_t * p_request_url, u8 video_size)
{
    DOWNLOAD_API_DEBUG("[%s] start start ...\n", __func__);
	char *   p_stream_map = NULL;
	char *   p_url_start = NULL;
	char  *  p_url_end = NULL;
	int             len = 0;
	int             i = 0 ;
	int             data_len  = 0;
	char *         p_stream_map_tail = NULL;

    if (p_stream_map = strstr(urlenc_data, "hlsvp="))
	{
		p_url_start = p_stream_map + strlen("hlsvp=");
		p_stream_map_tail = strstr(p_url_start, "&");
		DOWNLOAD_API_DEBUG("[%s] len of hlsvp is:%d\n", __func__, p_stream_map_tail - p_url_start);
		char parse_ok = 0;
		int index = 0;

		do {
			len = strlen(p_url_start);
			for (i = 0; i < len ; i++)
            {
				if ((p_url_start[i] == '%') && (p_url_start[i+1] == '2') && p_url_start[i+2] == 'C')
                {
					p_url_end =  p_url_start + i;
					data_len = p_url_end - p_url_start;
					index = p_video_info->index;
					p_video_info->buf[index] = (char *)malloc(data_len + 1);

					if (p_video_info->buf[index]) {
						memset(p_video_info->buf[index], 0, data_len + 1);
						memcpy(p_video_info->buf[index], p_url_start, data_len);
						//DOWNLOAD_API_DEBUG("url[%d]:\n",p_video_info->index);
						DOWNLOAD_API_DEBUG("zx p_video_info->buf[%d]:%s \n", index, p_video_info->buf[index]);
						p_url_start = p_url_end + strlen("%2C");
						p_video_info->index++;
						break;
					}
				}
                else if (p_url_start[i] == '&')
				{
					data_len = i;
					index = p_video_info->index;
					p_video_info->buf[index] = (char *)malloc(data_len + 1);

					if (p_video_info->buf[index]) {
						memset(p_video_info->buf[index], 0, data_len + 1);
						memcpy(p_video_info->buf[index], p_url_start, data_len);
						//DOWNLOAD_API_DEBUG("url[%d]:\n",p_video_info->index);
						DOWNLOAD_API_DEBUG("zx p_video_info->buf[%d]:%s \n", index, p_video_info->buf[index]);
						p_video_info->index++;
					}

					DOWNLOAD_API_DEBUG(">>>> Finish hlsvp List!!!<<<<\n");
					parse_ok = 1;
					break;
				}
			}
		} while (p_url_start < p_stream_map_tail && parse_ok == 0);

    } else {
		DOWNLOAD_API_DEBUG("[%s] fail to hlsvp, only for youtube live!!!\n", __func__);
        return NO_YOUTUBE_HLS_ERROR;
	}
    int ret = 0;
    int download_retry = 0;

    for (i = 0; i < p_video_info->index; i++)
    {
        int input_len = 0;
		int output_len = 0;
        input_len = strlen(p_video_info->buf[i]);
		output_len = php_url_decode(p_video_info->buf[i], input_len);
        for(download_retry = 0; download_retry < MAX_POST_COUNT; download_retry++)
        {
            ret = Nw_DownloadURLTimeout(p_video_info->buf[i], YOUTUBE_WATCH_PAGE, DEFAULT_DL_HTML_TIMEOUT,NULL,NULL,NULL,NULL,0);

            if (ret == 1 || ret == -1) {
                break;
            }
        }
        if(download_retry == 4 || ret == -1)
        {
            DOWNLOAD_API_ERROR("[%s][ERROR] do nothing, fail to download youtube_watch_page ...\n ", __func__);
            return -1;
        }

        DOWNLOAD_API_ERROR("[%s]zx Nw_DownloadURLTimeout ok!!\n ", __func__);
#ifdef __LINUX__
        FILE * fp = NULL;
        fp = fopen("/mnt/youtube_watch_page", "rb");
        char * tmp_buf2 = NULL;

        if (fp) {
            DOWNLOAD_API_LOG("[%s]  open youtube_watch_page ok !!!!\n", __func__);
            tmp_buf2 = (char *)mtos_malloc(JS_INFO_BUF_LEN);

            if (tmp_buf2) {
                memset(tmp_buf2, 0, JS_INFO_BUF_LEN);
                int rdNum = fread(tmp_buf2, 1, JS_INFO_BUF_LEN, fp);

                if (rdNum > 0) {
                    DOWNLOAD_API_LOG("[%s] read %d bytes from youtube_watch_page!!!\n", __func__, rdNum);
                }

                fclose(fp);
    			fp = NULL;
                ret = parse_ext_m3u_list(tmp_buf2, p_request_url);
                mtos_free(tmp_buf2);

                if (ret != 0)
                {
                    DOWNLOAD_API_ERROR("[%s] parse_ext_m3u_list failed!!!\n", __func__);
                    return ret;
                }
            }
        }
#else
        ufs_file_t  *fp1 = (ufs_file_t  *)mtos_malloc(sizeof(ufs_file_t));//= stream->fd;
        if(fp1 ==NULL)
            return -1;
        memset(fp1, 0, sizeof(ufs_file_t));

        unsigned short  path_tmp[256]={0};
        unsigned short * p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)YOUTUBE_WATCH_PAGE,path_tmp);

        u8 fs_res = ufs_open(fp1, p_filename1, (op_mode_t)(UFS_READ | UFS_OPEN)) ;
        char * tmp_buf2 = NULL;

        if ((fs_res == UFS_OK) ||(fs_res == UFS_EXIST)) {
            DOWNLOAD_API_LOG("[%s]  open youtube_watch_page ok !!!!\n", __func__);
            tmp_buf2 = (char *)mtos_malloc(JS_INFO_BUF_LEN);
            if(tmp_buf2 == NULL)
            {
                ufs_close(fp1);
                mtos_free(fp1);
                mtos_free(p_video_info);
                return -1;
            }
            if (tmp_buf2) {
                memset(tmp_buf2, 0, JS_INFO_BUF_LEN);
                int rdNum = 0;
                ufs_read(fp1, tmp_buf2, JS_INFO_BUF_LEN, (u32*)&rdNum);
                ufs_close(fp1);
                mtos_free(fp1);
                if (rdNum > 0) {
                    DOWNLOAD_API_LOG("[%s] read %d bytes from youtube_watch_page!!!\n", __func__, rdNum);
                }

                ret = parse_ext_m3u_list(tmp_buf2, p_request_url);
                mtos_free(tmp_buf2);

                if (ret != 0)
                {
                    DOWNLOAD_API_ERROR("[%s] parse_ext_m3u_list failed!!!\n", __func__);
                    return ret;
                }
            }
        }
#endif
    }

    //qcif cif->ld, d1 720->sd, 720 1080->hd
    if((video_size&0x1) == QEQUEST_VIDEO_LD)
    {
        //preferentially choose cif
        if(strlen(p_request_url->playUrlArray[1]) > 0)
        {
            memcpy(p_request_url->playUrlArray[0], p_request_url->playUrlArray[1], strlen(p_request_url->playUrlArray[1]));
            ret = 0;
        }
    }
    if((video_size&0x2) == QEQUEST_VIDEO_SD)
    {
        //preferentially choose d1
        if(strlen(p_request_url->playUrlArray[2]) > 0)
        {
            memcpy(p_request_url->playUrlArray[1], p_request_url->playUrlArray[2], strlen(p_request_url->playUrlArray[2]));
            ret = 0;
        }
        else if(strlen(p_request_url->playUrlArray[3]) > 0)
        {
            memcpy(p_request_url->playUrlArray[1], p_request_url->playUrlArray[3], strlen(p_request_url->playUrlArray[3]));
            ret = 0;
        }
        else if(strlen(p_request_url->playUrlArray[1]) > 0)
        {
            //do nothing, select the cif.
            ret = 0;
        }
    }
    if((video_size&0x4) == QEQUEST_VIDEO_HD)
    {
        //preferentially choose 720P
        if(strlen(p_request_url->playUrlArray[3]) > 0)
        {
            memcpy(p_request_url->playUrlArray[2], p_request_url->playUrlArray[3], strlen(p_request_url->playUrlArray[3]));
            ret = 0;
        }
        else if(strlen(p_request_url->playUrlArray[4]) > 0)
        {
            memcpy(p_request_url->playUrlArray[2], p_request_url->playUrlArray[4], strlen(p_request_url->playUrlArray[4]));
            ret = 0;
        }
    }
    memset(p_request_url->playUrlArray[3], 0, DOWNLOAD_URL_MAX_LEN);
    memset(p_request_url->playUrlArray[4], 0, DOWNLOAD_URL_MAX_LEN);

    return ret;
}

int  youtube_parse_stream_map_list(const char * p_videoid, parse_youtube_vedio_info_t * p_video_info, char * urlenc_data, request_url_t * p_request_url, u8 size, youtube_search_key_t youtube_key) {
	DOWNLOAD_API_DEBUG("[%s] start start ...\n", __func__);
	char *   p_stream_map = NULL;
	char *   p_url_start = NULL;
	char  *  p_url_end = NULL;
	int             i = 0 ;
	int             data_len  = 0;
	char *         p_stream_map_tail = NULL;
    char *         p_tmp_js_buf = NULL;

	if (p_stream_map = strstr(urlenc_data, youtube_key.keyArray[0]))
	{
		//DOWNLOAD_API_DEBUG("[%s]>>>> find 'stream_map='  lalala !!!<<<<\n", __func__);
		p_url_start = p_stream_map + strlen(youtube_key.keyArray[0]);
		p_stream_map_tail = strstr(p_url_start, youtube_key.keyArray[1]);
		//DOWNLOAD_API_DEBUG("[%s] len of stream_map_list  is:%d\n", __func__, p_stream_map_tail - p_url_start);
		char parse_ok = 0;
		int index = 0;
		// the comma ',' means new url start
		do {
			p_url_end = strstr(p_url_start, ",");
            if(p_url_end == NULL)
                break;
            if(p_url_end > p_stream_map_tail)
                break;

			data_len = p_url_end - p_url_start;
			index = p_video_info->index;
			p_video_info->buf[index] = (char *)mtos_malloc(data_len + 1);

			if (p_video_info->buf[index]) {
				memset(p_video_info->buf[index], 0, data_len + 1);
				memcpy(p_video_info->buf[index], p_url_start, data_len);
                //DOWNLOAD_API_DEBUG("p_video_info->buf[%d]:%s \n", index, p_video_info->buf[index]);
				p_url_start = p_url_end + 1;
				p_video_info->index++;
			}
		} while (p_url_start < p_stream_map_tail && parse_ok == 0);
    } else {
		DOWNLOAD_API_ERROR("[%s][ERROR] fail to find %s !!!\n", __func__, youtube_key.keyArray[0]);
	}

    if (p_stream_map = strstr(urlenc_data, youtube_key.keyArray[2]))
	{
		p_url_start = p_stream_map + strlen(youtube_key.keyArray[2]);
		p_stream_map_tail = strstr(p_url_start, youtube_key.keyArray[3]);
		char parse_ok = 0;
		int index = 0;
		do {
			p_url_end = strstr(p_url_start, ",");
            if(p_url_end == NULL)
                break;
            if(p_url_end > p_stream_map_tail)
                break;

			data_len = p_url_end - p_url_start;
			index = p_video_info->index;
			p_video_info->buf[index] = (char *)mtos_malloc(data_len + 1);

			if (p_video_info->buf[index]) {
				memset(p_video_info->buf[index], 0, data_len + 1);
				memcpy(p_video_info->buf[index], p_url_start, data_len);
                //DOWNLOAD_API_DEBUG("p_video_info->buf[%d]:%s \n", index, p_video_info->buf[index]);
				p_url_start = p_url_end + 1;
				p_video_info->index++;
			}
		} while (p_url_start < p_stream_map_tail && parse_ok == 0);
    } else {
		DOWNLOAD_API_ERROR("[%s][ERROR] fail to find %s !!!\n", __func__, youtube_key.keyArray[2]);
	}

	for (i = 0; i < p_video_info->index; i++) {
		int input_len = 0;
		int output_len = 0;
		//DOWNLOAD_API_DEBUG("p_video_info->buf[%d]:0x%x\n",i,p_video_info->buf[i]);

		if (p_video_info->buf[i]) {
			input_len = strlen(p_video_info->buf[i]);
			output_len = php_url_decode(p_video_info->buf[i], input_len);
			parse_url_component(p_video_info, youtube_key.keyArray[4], i);//KW_URL
            if(strstr(p_video_info->playUrlArray[i], "signature=") == NULL)
            {
				parse_url_component(p_video_info, youtube_key.keyArray[5], i);//KW_SIG
            }
			parse_url_component(p_video_info, youtube_key.keyArray[6], i);//KW_ITAG
			//DOWNLOAD_API_DEBUG("playUrlArray[%d]:%s \n", i, p_video_info->playUrlArray[i]);
			input_len = strlen(p_video_info->playUrlArray[i]);
			output_len = php_url_decode(p_video_info->playUrlArray[i], input_len);

			//collect h264 encode stream
            /*	{18, "MP4", "270p/360p", "H.264", "AAC"},  0
            	{22, "MP4", "720p", "H.264 High", "AAC"},  3
            	{34, "FLV", "360p", "H.264 Main", "AAC"},  1
            	{35, "FLV", "480p", "H.264 Main", "AAC"},  2
            	{37, "MP4", "1080P", "H.264 High", "AAC"}, 4
            	{38, "MP4", "3072P", "H.264 High", "AAC"}, 4
            	{82, "MP4", "360p", "H.264 3D", "AAC"},    1
            	{83, "MP4", "240p", "H.264 3D", "AAC"},    0
            	{84, "MP4", "720p", "H.264 3D", "AAC"},    3
            	{85, "MP4", "520p", "H.264", "AAC"},       2
            	{120, "FLV", "720p", "AVC Main@L3.1", "AAC"},  3   */
			if (p_video_info->itagArray[i] == 18 ||p_video_info->itagArray[i] == 22 ||p_video_info->itagArray[i] == 35 ||
					p_video_info->itagArray[i] == 34 || p_video_info->itagArray[i] == 37 ||
					p_video_info->itagArray[i] == 38 || p_video_info->itagArray[i] == 120 ||
					p_video_info->itagArray[i] == 82 || p_video_info->itagArray[i] == 83
					|| p_video_info->itagArray[i] == 85 || p_video_info->itagArray[i] == 84
					) {
				DOWNLOAD_API_DEBUG("list[%d] will be moved to list !!!!\n", i);
				print_itag_detail(p_video_info->itagArray[i]);

                if(strlen(p_video_info->sigArray[i]) > 0)
                {
                    DOWNLOAD_API_DEBUG("sigArray len[%d]\n", strlen(p_video_info->sigArray[i]));
                    strcat(p_video_info->playUrlArray[i], "&signature=");
                    if(strlen(p_video_info->sigArray[i]) > 80)
                    {
                        DOWNLOAD_API_DEBUG("Need decrypt_sig[%s]!!\n",p_video_info->sigArray[i]);
                        if(p_tmp_js_buf == NULL)
                        {
                            p_tmp_js_buf = (char *)mtos_malloc(JS_INFO_BUF_LEN);
                            if(p_tmp_js_buf)
                            {
                                memset(p_tmp_js_buf, 0, JS_INFO_BUF_LEN);
                                get_decrypt_js_data(urlenc_data, p_videoid, p_tmp_js_buf);
                            }
                            else
                            {
                                DOWNLOAD_API_ERROR("Err:Malloc len[%d] failed, will decrypt signature failed!!\n",JS_INFO_BUF_LEN);
                            }
                        }

                        if(p_tmp_js_buf)
                        {
                            getDecyptSig(p_tmp_js_buf, p_video_info->sigArray[i]);
                        }
                        DOWNLOAD_API_DEBUG("Decrypt_sig[%s]     !!\n",p_video_info->sigArray[i]);
                        strcat(p_video_info->playUrlArray[i], p_video_info->sigArray[i]);
                    }
                    else
                    {
				        strcat(p_video_info->playUrlArray[i], p_video_info->sigArray[i]);
                    }
                }

                if(p_video_info->itagArray[i] == 83)
                {
                   if(strlen(p_request_url->playUrlArray[0]) == 0)
                   {
                        memset(p_request_url->playUrlArray[0], 0, DOWNLOAD_URL_MAX_LEN);
                		memcpy(p_request_url->playUrlArray[0], p_video_info->playUrlArray[i], strlen(p_video_info->playUrlArray[i]));
                        //DOWNLOAD_API_DEBUG("qcif [%s] !!!!\n", p_video_info->playUrlArray[i]);
                   }

                }
                if(p_video_info->itagArray[i] == 36 || p_video_info->itagArray[i] == 34||p_video_info->itagArray[i] == 82)
                {
                    if(strlen(p_request_url->playUrlArray[1]) == 0)
                    {
                        memset(p_request_url->playUrlArray[1], 0, DOWNLOAD_URL_MAX_LEN);
                		memcpy(p_request_url->playUrlArray[1], p_video_info->playUrlArray[i], strlen(p_video_info->playUrlArray[i]));
                        //DOWNLOAD_API_DEBUG("cif [%s] !!!!\n", p_video_info->playUrlArray[i]);
                    }

                }
                else if(p_video_info->itagArray[i] == 18 || p_video_info->itagArray[i] == 35||p_video_info->itagArray[i] == 84)
                {
                    if(strlen(p_request_url->playUrlArray[2]) == 0)
                    {
                        memset(p_request_url->playUrlArray[2], 0, DOWNLOAD_URL_MAX_LEN);
                		memcpy(p_request_url->playUrlArray[2], p_video_info->playUrlArray[i], strlen(p_video_info->playUrlArray[i]));
                        //DOWNLOAD_API_DEBUG("4cif [%s] !!!!\n", p_video_info->playUrlArray[i]);
                    }
                }
                else if(p_video_info->itagArray[i] == 22||p_video_info->itagArray[i] == 85 ||p_video_info->itagArray[i] == 120)
                {
                    if(strlen(p_request_url->playUrlArray[3]) == 0)
                    {
                        memset(p_request_url->playUrlArray[3], 0, DOWNLOAD_URL_MAX_LEN);
                		memcpy(p_request_url->playUrlArray[3], p_video_info->playUrlArray[i], strlen(p_video_info->playUrlArray[i]));
                        //DOWNLOAD_API_DEBUG("720P [%s] !!!!\n", p_video_info->playUrlArray[i]);
                    }

                }
                else if(p_video_info->itagArray[i] == 37||p_video_info->itagArray[i] == 38)
                {
                    if(strlen(p_request_url->playUrlArray[4]) == 0)
                    {
                        memset(p_request_url->playUrlArray[4], 0, DOWNLOAD_URL_MAX_LEN);
                		memcpy(p_request_url->playUrlArray[4], p_video_info->playUrlArray[i], strlen(p_video_info->playUrlArray[i]));
                        //DOWNLOAD_API_DEBUG("1080P [%s] !!!!\n", p_video_info->playUrlArray[i]);
                    }

                }
			}

		} else {
			DOWNLOAD_API_DEBUG("[%s][ERROR] p_video_info->buf[%d] == NULL!!!\n", __func__, i);
		}
	}

    if(p_tmp_js_buf)
    {
        delete_file(YOUTUBE_HTML5PLAYER_JS);
        mtos_free(p_tmp_js_buf);
        p_tmp_js_buf = NULL;
    }

    return parse_url_list(p_request_url, size);
}

void parse_url_component(parse_youtube_vedio_info_t * p_video_info, char * keyword, int index) {
	char * p_component_start = strstr(p_video_info->buf[index], keyword);

	if (p_component_start) {
		DOWNLOAD_API_DEBUG("[%s] find %s lalalala  !\n",__func__,keyword);
		char * p_component_end = strstr(p_component_start + strlen(keyword), "\\u0026");

        if (strcmp(keyword, "itag=") == 0)
        {
            if (p_component_end == NULL)
            {
                p_component_end = strstr(p_component_start + strlen(keyword), "&");
            }
            else
            {
                //contains \\u0026, but itag=136&key=yt5&gcr=tw&
                int tmp_cnt = p_component_end - (p_component_start + strlen(keyword));
                if(tmp_cnt > 5)
                    p_component_end = strstr(p_component_start + strlen(keyword), "&");
            }
        }

		if (p_component_end) {
			int cpy_cnt = p_component_end - (p_component_start + strlen(keyword));

			if (strcmp(keyword, "url=") == 0) {
				memset(p_video_info->playUrlArray[index], 0, DOWNLOAD_URL_MAX_LEN);
				memcpy(p_video_info->playUrlArray[index], p_component_start + strlen(keyword), cpy_cnt);
				//DOWNLOAD_API_DEBUG("[%s]fetch %s ok!\n",__func__,keyword);
				//DOWNLOAD_API_DEBUG("[%s]\n",p_video_info->playUrlArray[index]);

			} else if (strcmp(keyword, "\\u0026s=") == 0) {
				memset(p_video_info->sigArray[index], 0, SIG_STR_MAX_LEN);
				memcpy(p_video_info->sigArray[index], p_component_start + strlen(keyword), cpy_cnt);
				//DOWNLOAD_API_DEBUG("[%s]fetch %s ok!\n",__func__,keyword);
				//DOWNLOAD_API_DEBUG("[%s]\n",p_video_info->sigArray[index]);
			} else if (strcmp(keyword, "itag=") == 0) {
				char tmp[ITAG_STR_MAX_LEN] = {0};
				memset(tmp, 0, ITAG_STR_MAX_LEN);
				memcpy(tmp, p_component_start + strlen(keyword), cpy_cnt);
				p_video_info->itagArray[index] = atoi(tmp);
				//DOWNLOAD_API_DEBUG("[%s]fetch %s ok!\n",__func__,keyword);
				//print_itag_detail(atoi(p_video_info->itagArray[index]));
			}

		} else {
			if (strcmp(keyword, "url=") == 0) {
				memset(p_video_info->playUrlArray[index], 0, DOWNLOAD_URL_MAX_LEN);
				strcpy(p_video_info->playUrlArray[index], p_component_start + strlen(keyword));
				//DOWNLOAD_API_DEBUG("[%s]222  fetch %s ok!\n",__func__,keyword);
				//DOWNLOAD_API_DEBUG("[%s]\n",p_video_info->playUrlArray[index]);

			} else if (strcmp(keyword, "\\u0026s=") == 0) {
				memset(p_video_info->sigArray[index], 0, SIG_STR_MAX_LEN);
				strcpy(p_video_info->sigArray[index], p_component_start + strlen(keyword));
				//DOWNLOAD_API_DEBUG("[%s]fetch %s ok!\n",__func__,keyword);
				//DOWNLOAD_API_DEBUG("222[%s]\n",p_video_info->sigArray[index]);
			} else if (strcmp(keyword, "itag=") == 0) {
				char tmp[ITAG_STR_MAX_LEN] = {0};
				memset(tmp, 0, ITAG_STR_MAX_LEN);
				strcpy(tmp, p_component_start + strlen(keyword));
				p_video_info->itagArray[index] = atoi(tmp);
				//DOWNLOAD_API_DEBUG("[%s]fetch %s ok!\n",__func__,keyword);
				//print_itag_detail(atoi(p_video_info->itagArray[index]));
			}
		}

	} else {
        if (strcmp(keyword, "\\u0026s=") == 0)
        {
            //maybe s=21021041222B3D22A63D7AD6A61AAD01E75B52C643C.4A9985BC199300F53F274BC394CF7B9F68CCA614\u0026type=video/3gpp;....
            if(*(p_video_info->buf[index]) == 's' && *(p_video_info->buf[index]+1) == '=')
            {
                p_component_start = p_video_info->buf[index];
                char * p_component_end = strstr(p_component_start + 2, "\\u0026");
                memset(p_video_info->sigArray[index], 0, SIG_STR_MAX_LEN);
				memcpy(p_video_info->sigArray[index], p_component_start + 2, (p_component_end - p_component_start - 2));
                DOWNLOAD_API_DEBUG("222 s= sig[%s]\n",p_video_info->sigArray[index]);
            }
        }
        else
        {
            DOWNLOAD_API_ERROR("[ERROR]  fail to parse the component :%s !!!\n", keyword);
            DOWNLOAD_API_ERROR("[ERROR]  failed buf[%d] :%s !!!\n", index, p_video_info->buf[index]);
        }
	}

	//DOWNLOAD_API_DEBUG("[%s] end end ...\n",__func__);
}

void  resetParseYoutubeVideoInfoData(parse_youtube_vedio_info_t * p_info) {
	if (p_info == NULL) {
		DOWNLOAD_API_ERROR("[%s] ERROR p_info == NULL do nothing !!!\n", __func__);
		return;
	}

	if (p_info->index > 0) {
		int i = 0;
        DOWNLOAD_API_ERROR("[%s] p_info->index[%d]!!!\n", __func__, p_info->index);
		for (i = 0; i < p_info->index ; i++) {
			if (p_info->buf[i]) {
				mtos_free(p_info->buf[i]);
				p_info->buf[i] = NULL;
			}
		}

		memset(p_info, 0, sizeof(parse_youtube_vedio_info_t));
	}

	return;
}

int youtube_get_video_play_url(const char * p_videoid, request_url_t * p_request_url, u8 size, u8 is_live)
{
    int  par_ret = 0;
    int ret;
	int download_retry = 0;
	DOWNLOAD_API_DEBUG("video id: [%s]\n", p_videoid);
	char tmp_buf[1280] = {0};
	memset(tmp_buf, 0, 1280);
    if(is_live)
    {
	    snprintf(tmp_buf, 1200, "https://www.youtube.com/get_video_info?el=player_embedded&video_id=%s", p_videoid);
    }
    else
    {
	    snprintf(tmp_buf, 1200, "https://www.youtube.com/watch?v=%s&gl=US&hl=en&has_verified=1", p_videoid);///changed by xizhou
    }
	DOWNLOAD_API_DEBUG("[url]:%s\n", tmp_buf);

	for(download_retry = 0; download_retry < MAX_POST_COUNT; download_retry++)
	{
		ret = Nw_DownloadURLTimeout(tmp_buf, YOUTUBE_WATCH_PAGE, DEFAULT_DL_HTML_TIMEOUT,NULL,NULL,NULL,NULL,0);

		if (ret == 1 || ret == -1) {
			break;
		}
	}
	if(download_retry == 4 || ret == -1)
	{
		DOWNLOAD_API_ERROR("[%s][ERROR] do nothing, fail to download youtube_watch_page ...\n ", __func__);
		return -1;
	}

    parse_youtube_vedio_info_t *p_video_info;
    p_video_info = (parse_youtube_vedio_info_t *)mtos_malloc(sizeof(parse_youtube_vedio_info_t));

	if (p_video_info) {
		memset(p_video_info, 0, sizeof(parse_youtube_vedio_info_t));
	}
    else
    {
        DOWNLOAD_API_ERROR("[%s][ERROR] alloc parse_youtube_vedio_info_t failed...\n ", __func__);
		return -5;
    }

    youtube_search_key_t youtube_key;
    memset(&youtube_key, 0, sizeof(youtube_search_key_t));
    //set default vale.
    strcpy(youtube_key.keyArray[0], "adaptive_fmts\":");
    strcpy(youtube_key.keyArray[1], "\",");
    strcpy(youtube_key.keyArray[2], "url_encoded_fmt_stream_map\":");
    strcpy(youtube_key.keyArray[3], "\",");
    strcpy(youtube_key.keyArray[4], KW_URL);
    strcpy(youtube_key.keyArray[5], KW_SIG);
    strcpy(youtube_key.keyArray[6], KW_ITAG);

#ifdef __LINUX__
	FILE * fp = NULL;
	fp = fopen(YOUTUBE_WATCH_PAGE, "rb");
	char * tmp_buf2 = NULL;

	if (fp) {
		DOWNLOAD_API_LOG("[%s]  open youtube_watch_page ok !!!!\n", __func__);
		tmp_buf2 = (char *)mtos_malloc(JS_INFO_BUF_LEN);

		if (tmp_buf2) {
			memset(tmp_buf2, 0, JS_INFO_BUF_LEN);
			int rdNum = fread(tmp_buf2, 1, JS_INFO_BUF_LEN, fp);

			if (rdNum > 0) {
				DOWNLOAD_API_LOG("[%s] read %d bytes from youtube_watch_page!!!\n", __func__, rdNum);
			}

			resetParseYoutubeVideoInfoData(p_video_info);
			p_video_info->index = 0;
			fclose(fp);
			fp = NULL;
            if(is_live)
            {
                par_ret = parse_youtube_live_list(p_videoid, p_video_info, tmp_buf2, p_request_url, size);
            }
            else
            {
			    par_ret = youtube_parse_stream_map_list(p_videoid, p_video_info, tmp_buf2, p_request_url, size, youtube_key);
            }
			mtos_free(tmp_buf2);

			if (par_ret != 0)
			{
                DOWNLOAD_API_ERROR("[%s] youtube_parse_stream_map_list failed!!!\n", __func__);
			}
		}
	}

#else
	ufs_file_t  *fp1 = (ufs_file_t  *)mtos_malloc(sizeof(ufs_file_t));//= stream->fd;
	if(fp1 ==NULL)
		return -1;
	memset(fp1, 0, sizeof(ufs_file_t));

	unsigned short  path_tmp[256]={0};
	unsigned short * p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)YOUTUBE_WATCH_PAGE,path_tmp);

	u8 fs_res = ufs_open(fp1, p_filename1, (op_mode_t)(UFS_READ | UFS_OPEN)) ;
	char * tmp_buf2 = NULL;

	if ((fs_res == UFS_OK) ||(fs_res == UFS_EXIST)) {
		DOWNLOAD_API_LOG("[%s]  open youtube_watch_page ok !!!!\n", __func__);
		tmp_buf2 = (char *)mtos_malloc(JS_INFO_BUF_LEN);
		if(tmp_buf2 == NULL)
		{
			ufs_close(fp1);
			mtos_free(fp1);
            resetParseYoutubeVideoInfoData(p_video_info);
		    mtos_free(p_video_info);
			return -1;
		}
		if (tmp_buf2) {
			memset(tmp_buf2, 0, JS_INFO_BUF_LEN);
			int rdNum = 0;
			ufs_read(fp1, tmp_buf2, JS_INFO_BUF_LEN, (u32*)&rdNum);
			ufs_close(fp1);
			mtos_free(fp1);
			if (rdNum > 0) {
				DOWNLOAD_API_LOG("[%s] read %d bytes from youtube_watch_page!!!\n", __func__, rdNum);
			}

            resetParseYoutubeVideoInfoData(p_video_info);
			p_video_info->index = 0;
			if(is_live)
            {
                par_ret = parse_youtube_live_list(p_videoid, p_video_info, tmp_buf2, p_request_url, size);
            }
            else
            {
			    par_ret = youtube_parse_stream_map_list(p_videoid, p_video_info, tmp_buf2, p_request_url, size, youtube_key);
            }
			mtos_free(tmp_buf2);

			if (par_ret != 0)
			{
                DOWNLOAD_API_ERROR("[%s] youtube_parse_stream_map_list failed!!!\n", __func__);
			}
		}
	}
#endif

    delete_file(YOUTUBE_WATCH_PAGE);
    if(p_video_info)
	{
        resetParseYoutubeVideoInfoData(p_video_info);
		mtos_free(p_video_info);
		p_video_info = NULL;
	}
    return par_ret;


}

int yahoo_parse_url(const char *buf, request_url_t * p_request_url, u8 video_size)
{
    /*
    _FORMATS = [
            (u'stream_h264_ld_url', u'ld'),
            (u'stream_h264_url', u'standard'),
            (u'stream_h264_hq_url', u'hq'),
            (u'stream_h264_hd_url', u'hd'),
            (u'stream_h264_hd1080_url', u'hd180'),
        ]
    */
    char *   p_url_start = NULL;
	char  *  p_url_end = NULL;
    int   ret = -1;
    int   input_len = 0;
    char  tmp_path[256] = {0};
    char  tmp_host[256] = {0};
    char  tmp_width[8] = {0};


    p_url_start = (char*)buf;
    while((p_url_start = strstr(p_url_start, "path\":\"")) != NULL)
    {
        memset(tmp_host, 0, 256);
        memset(tmp_path, 0, 256);
        memset(tmp_width, 0, 8);
        p_url_start = p_url_start + strlen("path\":\"");
        p_url_end = strstr(p_url_start, "\"");
        if(p_url_end != NULL)
        {
    		memcpy(tmp_path, p_url_start, (p_url_end-p_url_start));
            p_url_start = strstr(p_url_start, "host\":\"");
            if(p_url_start != NULL)
            {
                p_url_start = p_url_start + strlen("host\":\"");
                p_url_end = strstr(p_url_start, "\"");
                if(p_url_end != NULL)
                {
                    memcpy(tmp_host, p_url_start, (p_url_end-p_url_start));
                }
            }
        }

        if(strlen(tmp_path)>0 && strlen(tmp_host)>0)
        {
            p_url_start = strstr(p_url_start, "width\":");
            if(p_url_start != NULL)
            {
                p_url_start = p_url_start + strlen("width\":");
                p_url_end = strstr(p_url_start, ".0");
                if(p_url_end != NULL)
                {
                    memcpy(tmp_width, p_url_start, (p_url_end-p_url_start));
                }
            }

            if(strlen(tmp_width) > 0)
            {
                int tmp = atoi(tmp_width);
                int pos = video_width_to_pos(tmp);
                if(strlen(p_request_url->playUrlArray[pos]) == 0)
                {
                    strcpy(p_request_url->playUrlArray[pos], tmp_host);
                    strcat(p_request_url->playUrlArray[pos], tmp_path);
                    DOWNLOAD_API_LOG("[%s] width[%d] url[%s]\n", __func__, tmp, p_request_url->playUrlArray[pos]);
                }
            }
        }

    }

    //qcif cif->ld, d1 720->sd, 720 1080->hd
    if((video_size&0x1) == QEQUEST_VIDEO_LD)
    {
        //preferentially choose cif
        if(strlen(p_request_url->playUrlArray[1]) > 0)
        {
            memcpy(p_request_url->playUrlArray[0], p_request_url->playUrlArray[1], strlen(p_request_url->playUrlArray[1]));
            ret = 0;
        }
    }
    if((video_size&0x2) == QEQUEST_VIDEO_SD)
    {
        //preferentially choose d1
        if(strlen(p_request_url->playUrlArray[2]) > 0)
        {
            memcpy(p_request_url->playUrlArray[1], p_request_url->playUrlArray[2], strlen(p_request_url->playUrlArray[2]));
            ret = 0;
        }
        else if(strlen(p_request_url->playUrlArray[3]) > 0)
        {
            memcpy(p_request_url->playUrlArray[1], p_request_url->playUrlArray[3], strlen(p_request_url->playUrlArray[3]));
            ret = 0;
        }
        else if(strlen(p_request_url->playUrlArray[1]) > 0)
        {
            //do nothing, select the cif.
            ret = 0;
        }
    }
    if((video_size&0x4) == QEQUEST_VIDEO_HD)
    {
        //preferentially choose 720P
        if(strlen(p_request_url->playUrlArray[3]) > 0)
        {
            memcpy(p_request_url->playUrlArray[2], p_request_url->playUrlArray[3], strlen(p_request_url->playUrlArray[3]));
            ret = 0;
        }
        else if(strlen(p_request_url->playUrlArray[4]) > 0)
        {
            memcpy(p_request_url->playUrlArray[2], p_request_url->playUrlArray[4], strlen(p_request_url->playUrlArray[4]));
            ret = 0;
        }
    }
    memset(p_request_url->playUrlArray[3], 0, DOWNLOAD_URL_MAX_LEN);
    memset(p_request_url->playUrlArray[4], 0, DOWNLOAD_URL_MAX_LEN);
    return ret;
}

int yahoo_get_long_id(char* tmp_buf2, char* p_long_id)
{
    int ret = -1;
    char *   p_url_start = NULL;
	char  *  p_url_end = NULL;
    p_url_start = strstr(tmp_buf2, "&vid_id=");
    if(p_url_start != NULL)
    {
        p_url_start = p_url_start + strlen("&vid_id=");
        p_url_end = strstr(p_url_start, "&");
        if(p_url_end != NULL)
        {
            memcpy(p_long_id, p_url_start, (p_url_end-p_url_start));
            DOWNLOAD_API_LOG("[%s] yahoo long_id[%s].\n", __func__, p_long_id);
            return 0;
        }
    }

    p_url_start = strstr(tmp_buf2, "root.App.Cache.context.videoCache.curVideo");
    if(p_url_start != NULL)
    {
        p_url_start = p_url_start + strlen("root.App.Cache.context.videoCache.curVideo");
        p_url_start = strstr(p_url_start, "\"id\":\"");
        if(p_url_start != NULL)
        {
            p_url_start = p_url_start + strlen("\"id\":\"");
            p_url_end = strstr(p_url_start, "\"");
            if(p_url_end != NULL)
            {
                memcpy(p_long_id, p_url_start, (p_url_end-p_url_start));
                DOWNLOAD_API_LOG("[%s] yahoo long_id[%s].\n", __func__, p_long_id);
                return 0;
            }
        }
    }

    return ret;
}

int yahoo_get_video_play_url(const char * p_url, request_url_t * p_request_url, u8 size)
{
    int  par_ret = 0;
    int ret;
    char tmp_buf[1280] = {0};
	int download_retry = 0;
	DOWNLOAD_API_DEBUG("video url or long_id: [%s]\n", p_url);
    char p_long_id[64];
    memset(p_long_id, 0, 64);
    char * tmp_buf2 = NULL;

#ifdef __LINUX__
    FILE * fp = NULL;
#else
    ufs_file_t  *fp1 = NULL;
    unsigned short  path_tmp[256]={0};
    unsigned short * p_filename1 = NULL;
    u8 fs_res;
#endif

    if(strstr(p_url, "yahoo.com") != NULL)
    {
    	for(download_retry = 0; download_retry < MAX_POST_COUNT; download_retry++)
    	{
    		ret = Nw_DownloadURLTimeout(p_url, YAHOO_WATCH_PAGE, DEFAULT_DL_HTML_TIMEOUT,NULL,NULL,NULL,NULL,0);

    		if (ret == 1 || ret == -1) {
    			break;
    		}
    	}
    	if(download_retry == 4 || ret == -1)
    	{
    		DOWNLOAD_API_ERROR("[%s][ERROR] do nothing, fail to download yahoo_watch_page ...\n ", __func__);
    		return -1;
    	}

#ifdef __LINUX__
    	fp = fopen("/mnt/yahoo_watch_page", "rb");

        tmp_buf2 = NULL;
    	if (fp) {
    		DOWNLOAD_API_LOG("[%s]  open yahoo_watch_page ok !!!!\n", __func__);
    		tmp_buf2 = (char *)mtos_malloc(JS_INFO_BUF_LEN);

    		if (tmp_buf2) {
    			memset(tmp_buf2, 0, JS_INFO_BUF_LEN);
    			int rdNum = fread(tmp_buf2, 1, JS_INFO_BUF_LEN, fp);

    			if (rdNum > 0) {
    				DOWNLOAD_API_LOG("[%s] read %d bytes from yahoo_watch_page!!!\n", __func__, rdNum);
    			}

    			fclose(fp);
    			fp = NULL;
    			par_ret = yahoo_get_long_id(tmp_buf2, p_long_id);

    			if (par_ret != 0)
    			{
                    DOWNLOAD_API_ERROR("[%s] yahoo get long_id failed!!!\n", __func__);
                    mtos_free(tmp_buf2);
    				return par_ret;
    			}
    		}
    	}

#else
    	fp1 = (ufs_file_t  *)mtos_malloc(sizeof(ufs_file_t));//= stream->fd;
    	if(fp1 ==NULL)
    		return -1;
    	memset(fp1, 0, sizeof(ufs_file_t));

    	p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)YAHOO_WATCH_PAGE,path_tmp);

    	fs_res = ufs_open(fp1, p_filename1, (op_mode_t)(UFS_READ | UFS_OPEN)) ;
    	tmp_buf2 = NULL;

    	if ((fs_res == UFS_OK) ||(fs_res == UFS_EXIST)) {
    		DOWNLOAD_API_LOG("[%s]  open yahoo_watch_page ok !!!!\n", __func__);
    		tmp_buf2 = (char *)mtos_malloc(JS_INFO_BUF_LEN);
    		if(tmp_buf2 == NULL)
    		{
    			ufs_close(fp1);
    			mtos_free(fp1);
    			return -1;
    		}
    		if (tmp_buf2) {
    			memset(tmp_buf2, 0, JS_INFO_BUF_LEN);
    			int rdNum = 0;
    			ufs_read(fp1, tmp_buf2, JS_INFO_BUF_LEN, (u32*)&rdNum);
    			ufs_close(fp1);
    			if (rdNum > 0) {
    				DOWNLOAD_API_LOG("[%s] read %d bytes from yahoo_watch_page!!!\n", __func__, rdNum);
    			}

                par_ret = yahoo_get_long_id(tmp_buf2, p_long_id);

    			if (par_ret != 0)
    			{
                    DOWNLOAD_API_ERROR("[%s] yahoo get long_id failed!!!\n", __func__);
                    mtos_free(tmp_buf2);
    				return par_ret;
    			}
    		}
    	}
#endif
        delete_file(YAHOO_WATCH_PAGE);
    }
    else
    {
    #ifndef __LINUX__
        strcpy(p_long_id, p_url);
        fp1 = (ufs_file_t  *)mtos_malloc(sizeof(ufs_file_t));
        if(fp1 ==NULL)
    		return -1;
        tmp_buf2 = (char *)mtos_malloc(JS_INFO_BUF_LEN);
		if(tmp_buf2 == NULL)
		{
			return -1;
		}
		#endif
    }

    //get ld sd hd url!!!
    char query_buf[512];
    memset(query_buf, 0, 512);
    snprintf(query_buf, 512, "SELECT * FROM yahoo.media.video.streams WHERE id=\"%s\" AND plrs=\"86Gj0vCaSzV_Iuf6hNylf2\" AND region=\"US\" AND protocol=\"http\"", p_long_id);
    memset(tmp_buf, 0, 1280);
#ifndef __LINUX__
    url_encode(query_buf, tmp_buf);
#endif
    memcpy(query_buf, tmp_buf, strlen(tmp_buf));
    memset(tmp_buf, 0, 1280);
    snprintf(tmp_buf, 1200, "http://video.query.yahoo.com/v1/public/yql?q=%s&env=prod&format=json", query_buf);

    DOWNLOAD_API_DEBUG("yahoo query url: [%s]\n", tmp_buf);
	for(download_retry = 0; download_retry < MAX_POST_COUNT; download_retry++)
	{
		ret = Nw_DownloadURLTimeout(tmp_buf, YAHOO_VIDEO_JSON, DEFAULT_DL_HTML_TIMEOUT,NULL,NULL,NULL,NULL,0);

		if (ret == 1 || ret == -1) {
			break;
		}
	}
	if(download_retry == 4 || ret == -1)
	{
		DOWNLOAD_API_ERROR("[%s][ERROR] do nothing, fail to download yahoo_video_json ...\n ", __func__);
		return -1;
	}

#ifdef __LINUX__
	fp = fopen("/mnt/yahoo_video_json", "rb");

	if (fp) {
		DOWNLOAD_API_LOG("[%s]  open yahoo_video_json ok !!!!\n", __func__);

		if (tmp_buf2) {
			memset(tmp_buf2, 0, JS_INFO_BUF_LEN);
			int rdNum = fread(tmp_buf2, 1, JS_INFO_BUF_LEN, fp);

			if (rdNum > 0) {
				DOWNLOAD_API_LOG("[%s] read %d bytes from get_video_info!!!\n", __func__, rdNum);
			}

			fclose(fp);
			fp = NULL;
			par_ret = yahoo_parse_url(tmp_buf2, p_request_url, size);
			mtos_free(tmp_buf2);

			if (par_ret != 0)
			{
                DOWNLOAD_API_ERROR("[%s][ERROR] yahoo parse url failed!!!...\n ", __func__);
				return par_ret;
			}
		}
	}

#else
	memset(fp1, 0, sizeof(ufs_file_t));

	p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)YAHOO_VIDEO_JSON,path_tmp);

	fs_res = ufs_open(fp1, p_filename1, (op_mode_t)(UFS_READ | UFS_OPEN)) ;

	if ((fs_res == UFS_OK) ||(fs_res == UFS_EXIST)) {
		DOWNLOAD_API_LOG("[%s]  open yahoo_video_json ok !!!!\n", __func__);

		if (tmp_buf2) {
			memset(tmp_buf2, 0, JS_INFO_BUF_LEN);
			int rdNum = 0;
			ufs_read(fp1, tmp_buf2, JS_INFO_BUF_LEN, (u32*)&rdNum);
			ufs_close(fp1);
            mtos_free(fp1);

			par_ret = yahoo_parse_url(tmp_buf2, p_request_url, size);
			mtos_free(tmp_buf2);

			if (par_ret != 0)
			{
                DOWNLOAD_API_ERROR("[%s][ERROR] yahoo parse url failed!!!...\n ", __func__);
				return par_ret;
			}
		}
	}
    else
    {
        mtos_free(fp1);
        mtos_free(tmp_buf2);
    }
#endif

    delete_file(YAHOO_VIDEO_JSON);
    return par_ret;
}

int addanime_parse_url(const char *buf, request_url_t * p_request_url, u8 size)
{
    /*
    var hq_video_file = 'http://server014.add-anime.net/14084091848b01ehd.mp4';
    var normal_video_file = 'http://server014.add-anime.net/14084091848b01ehd.mp4';
    */
    char *   p_url_start = NULL;
	char  *  p_url_end = NULL;
    int   ret = -1;

    if((size&0x1) == QEQUEST_VIDEO_LD)
    {
        p_url_start = strstr(buf, "var normal_video_file = '");
        if(p_url_start != NULL)
        {
            p_url_start = p_url_start + strlen("var normal_video_file = '");
            p_url_end = strstr(p_url_start, "'");
            if(p_url_end != NULL)
            {
                memset(p_request_url->playUrlArray[0], 0, DOWNLOAD_URL_MAX_LEN);
    		    memcpy(p_request_url->playUrlArray[0], p_url_start, (p_url_end-p_url_start));
                ret = 0;
            }
        }
    }
    if((size&0x2) == QEQUEST_VIDEO_SD)
    {
        p_url_start = strstr(buf, "var normal_video_file = '");
        if(p_url_start != NULL)
        {
            p_url_start = p_url_start + strlen("var normal_video_file = '");
            p_url_end = strstr(p_url_start, "'");
            if(p_url_end != NULL)
            {
                memset(p_request_url->playUrlArray[1], 0, DOWNLOAD_URL_MAX_LEN);
    		    memcpy(p_request_url->playUrlArray[1], p_url_start, (p_url_end-p_url_start));
                ret = 0;
            }
        }
    }
    if((size&0x4) == QEQUEST_VIDEO_HD)
    {
        p_url_start = strstr(buf, "var hq_video_file = '");
        if(p_url_start != NULL)
        {
            p_url_start = p_url_start + strlen("var hq_video_file = '");
            p_url_end = strstr(p_url_start, "'");
            if(p_url_end != NULL)
            {
                memset(p_request_url->playUrlArray[2], 0, DOWNLOAD_URL_MAX_LEN);
    		    memcpy(p_request_url->playUrlArray[2], p_url_start, (p_url_end-p_url_start));
                ret = 0;
            }
        }
    }
    return ret;
}


int addanime_get_video_play_url(const char * p_url, request_url_t * p_request_url, u8 size)
{
    int  par_ret = 0;
    int ret;
	int download_retry = 0;
	DOWNLOAD_API_DEBUG("addanime url:%s\n", p_url);

	for(download_retry = 0; download_retry < MAX_POST_COUNT; download_retry++)
	{
		ret = Nw_DownloadURLTimeout(p_url, ADDANIME_WATCH_PAGE, DEFAULT_DL_HTML_TIMEOUT,NULL,NULL,NULL,NULL,0);

		if (ret == 1 || ret == -1) {
			break;
		}
	}
	if(download_retry == 4 || ret == -1)
	{
		DOWNLOAD_API_ERROR("[%s][ERROR] do nothing, fail to download addanime_watch_page ...\n ", __func__);
		return -1;
	}

#ifdef __LINUX__
	FILE * fp = NULL;
	fp = fopen("/mnt/addanime_watch_page", "rb");
	char * tmp_buf2 = NULL;

	if (fp) {
		DOWNLOAD_API_LOG("[%s]  open addanime_watch_page ok !!!!\n", __func__);
		tmp_buf2 = (char *)mtos_malloc(VIDEO_INFO_BUF_LEN);

		if (tmp_buf2) {
			memset(tmp_buf2, 0, VIDEO_INFO_BUF_LEN);
			int rdNum = fread(tmp_buf2, 1, VIDEO_INFO_BUF_LEN, fp);

			if (rdNum > 0) {
				DOWNLOAD_API_LOG("[%s] read %d bytes from addanime_watch_page!!!\n", __func__, rdNum);
			}

			fclose(fp);
			fp = NULL;
			par_ret = addanime_parse_url(tmp_buf2, p_request_url, size);
			mtos_free(tmp_buf2);

			if (par_ret != 0)
			{
                DOWNLOAD_API_ERROR("[%s] addanime_parse_url failed!!!\n", __func__);
			}
		}
	}

#else
	ufs_file_t  *fp1 = (ufs_file_t  *)mtos_malloc(sizeof(ufs_file_t));//= stream->fd;
	if(fp1 ==NULL)
		return -1;
	memset(fp1, 0, sizeof(ufs_file_t));

	unsigned short  path_tmp[256]={0};
	unsigned short * p_filename1 = Convert_Utf8_To_Unicode((unsigned char *)ADDANIME_WATCH_PAGE,path_tmp);

	u8 fs_res = ufs_open(fp1, p_filename1, (op_mode_t)(UFS_READ | UFS_OPEN)) ;
	char * tmp_buf2 = NULL;

	if ((fs_res == UFS_OK) ||(fs_res == UFS_EXIST)) {
		DOWNLOAD_API_LOG("[%s]  open addanime_watch_page ok !!!!\n", __func__);
		tmp_buf2 = (char *)mtos_malloc(VIDEO_INFO_BUF_LEN);
		if(tmp_buf2 == NULL)
		{
			ufs_close(fp1);
			mtos_free(fp1);
			return -1;
		}
		if (tmp_buf2) {
			memset(tmp_buf2, 0, VIDEO_INFO_BUF_LEN);
			int rdNum = 0;
			ufs_read(fp1, tmp_buf2, VIDEO_INFO_BUF_LEN, (u32*)&rdNum);
			ufs_close(fp1);
			mtos_free(fp1);
			if (rdNum > 0) {
				DOWNLOAD_API_LOG("[%s] read %d bytes from addanime_watch_page!!!\n", __func__, rdNum);
			}

			par_ret = addanime_parse_url(tmp_buf2, p_request_url, size);
			mtos_free(tmp_buf2);

			if (par_ret != 0)
			{
                DOWNLOAD_API_ERROR("[%s] addanime_parse_url failed!!!\n", __func__);
			}
		}
	}
#endif

    delete_file(ADDANIME_WATCH_PAGE);
    return par_ret;


}

int youku_get_video_play_url(const char * p_url, request_url_t * p_request_url, u8 size)
{
    int  par_ret = 0;
    DOWNLOAD_API_DEBUG("youku url:%s\n", p_url);

    //TODO Download and parse the url list file

    //The following code is just for testing
    memset(p_request_url->playUrlArray[0], 0, DOWNLOAD_URL_MAX_LEN);
    memcpy(p_request_url->playUrlArray[0], "http://k.youku.com/player/getFlvPath/sid/441283531641812c6e64a_00/st/flv/fileid/0300012C0054349ABE760D05CBFB1A9EC4DEAC-CF81-813F-7411-77392A4B44A8?K=66cecfc03a26248c2829eb2b&hd=1&myp=0&ts=198&ypp=0&ctype=12&ev=1&token=1183&oip=2059475858&ep=diaVGEGOU80B4yfegD8bNCq3cnNeXP4J9h%2BFgNARALshTOu7kE%2Bjspq2TotCE4prBFAHYeKE2NSTHzFnYYM1oWkQ10%2BrT%2Frn%2BvHi5apUx5kCFx1EdsmmvFSfQjjy", DOWNLOAD_URL_MAX_LEN);

    return par_ret;
}

//p_request_url[0] is ld stream, p_request_url[1] is sd, p_request_url[2] is hd
int Nw_Request_Website_DownloadURL(u8 website, const char * url, u8 size, request_url_t * p_request_url, u8 timeout)
{
    int ret = -1;
    u8  is_live = 0;
    int request_size = get_request_size(size);
    if(request_size == 0)
    {
        DOWNLOAD_API_ERROR("[ERROR] Input usless size:%d\n ", __func__, size);
        return -3;
    }
    DOWNLOAD_API_DEBUG("[%s] website[%d] video size[%d]\n ", __func__, website, size);

    memset(p_request_url, 0, sizeof(request_url_t));

    switch (website)
    {
        case WEBSITE_DAILYMOTION:
        {
            ret = dailymotion_get_video_play_url(url, p_request_url, size);

            if(ret == 0)
            {
                return ret;
            }
            else if(ret == 1)//<0 is error. vevo is 1 will continue.
            {
                //youtube_get_video_play_url(p_videoid, p_request_url, size);
                DOWNLOAD_API_ERROR("[ERROR] Dailymotion quote vevo url:%s\n ", __func__, url);
            }
            else
                return ret;

            break;
        }

        case WEBSITE_CLIPSYNDICATE:
        {
            ret = clipsyndicate_get_video_play_url(url, p_request_url);//only sd
            return ret;

            break;
        }

        case WEBSITE_YOUTUBE:
        {
            DOWNLOAD_API_DEBUG("[%s]youtube url[%s] ...\n ", __func__, url);
            ///youtube extract the p_videoid
            char *   p_url_start = NULL;
        	char  *  p_url_end = NULL;;
            char p_videoid[MAX_VIDEOID_LEN];
            memset(p_videoid, 0, MAX_VIDEOID_LEN);
            p_url_start = strstr(url, "watch?v=");
            if(p_url_start != NULL)
            {
                p_url_start = p_url_start + strlen("watch?v=");
                p_url_end = strstr(url, "_vevo");
                if(p_url_end == NULL)
                {
                    p_url_end = strstr(url, "_live");
                    if(p_url_end == NULL)
                    {
                        p_url_end = (char*)(url+strlen(url));
                        memcpy(p_videoid, p_url_start, (p_url_end-p_url_start+1));
                        DOWNLOAD_API_DEBUG("[%s]zx1 [%s] ...\n ", __func__, p_videoid);
                    }
                    else
                    {
                        is_live = 1;
                        memcpy(p_videoid, p_url_start, (p_url_end-p_url_start));
                        DOWNLOAD_API_DEBUG("[%s]zx2 [%s] ...\n ", __func__, p_videoid);
                    }
                }
                else
                {
                    memcpy(p_videoid, p_url_start, (p_url_end-p_url_start));
                }
                ret = youtube_get_video_play_url(p_videoid, p_request_url, size, is_live);
            }
            else
            {
                //youtubeDataprovider gives p_videoid
                ret = youtube_get_video_play_url(url, p_request_url, size, is_live);
            }

            break;
        }

        case WEBSITE_YAHOO:
        {
            ret = yahoo_get_video_play_url(url, p_request_url, size);
            break;
        }

        case WEBSITE_ADDANIME:
        {
            ret = addanime_get_video_play_url(url, p_request_url, size);
            break;
        }

        case WEBSITE_YOUKU:
        {
            ret = youku_get_video_play_url(url, p_request_url, size);
            break;
        }

        default:
        {
            DOWNLOAD_API_ERROR("[ERROR] unsupport url[%s]\n ", __func__, url);
            break;
        }
    }

    return ret;
}


