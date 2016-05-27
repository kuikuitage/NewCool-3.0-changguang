
#include <string.h>
#include "sys_types.h"
#include "mtos_misc.h"
#include <stdio.h>
#include "sys_define.h"
#include "ui_common.h"
#include "download_api.h"
#include "lib_unicode.h"
#include "gx_jansson.h"
#include "drv_dev.h"
#include "ufs.h"
#include "mdl.h"
#include "authorization.h"
#include "ap_cas.h"
#include "time.h"
#include "sys_dbg.h"

#define  USE_HTTP_GZIP

typedef char    *ck_va_list;
typedef unsigned int  CK_NATIVE_INT;
typedef unsigned int  ck_size_t;
#define  CK_AUPBND         (sizeof (CK_NATIVE_INT) - 1)
#define CK_BND(X, bnd)        (((sizeof (X)) + (bnd)) & (~(bnd)))
#define CK_VA_END(ap)         (void)0  /*ap = (ck_va_list)0 */
#define CK_VA_START(ap, A)    (void) ((ap) = (((char *) &(A)) + (CK_BND (A,CK_AUPBND))))
extern int ck_vsnprintf(char *buf, ck_size_t size, const char *fmt, ck_va_list args);

struct  aqi_ctrl_block{
	int http_inited;
};

static struct  aqi_ctrl_block aqi;

///
char* porting_getMac(void)
{
	return sys_status_get_mac_string();
}


///
int  porting_printf(const char *p_format, ...)
{
#define printk_size (1024 * 2)
    unsigned int  printed_len = 0;
#if 0
    ck_va_list p_args = NULL;
    static char    printk_buf[printk_size] = { 0 };
    //char* printk_buf = SY_MALLOC(printk_size);
    //memset(printk_buf, printk_size, 0);
    if( printk_buf != NULL)
    {
        CK_VA_START(p_args, p_format);
        printed_len = ck_vsnprintf(printk_buf, sizeof(printk_buf), (char *)p_format, p_args);
        CK_VA_END(p_args);
        OS_PRINTF(printk_buf);
    }
#endif
    return printed_len;

}


///
void* porting_malloc(unsigned int size)
{
	return SY_CALLOC(1,size);
}

void* porting_realloc(void* prev,unsigned int size)
{
	return SY_REALLOC(prev,size);
}



///
void porting_free(void* pBuffer)
{
	SY_FREE(pBuffer);
}


///
int porting_http_get(const char* url,
                     const char* extraHeaders,
                     const char* body,
                     unsigned int bodyLen,
                     unsigned int timeout,
                     char* buffer,              //response
                     unsigned int bufferSize)
{
	int ret;
	ufs_file_t fd = {0};
	u16 unicode[48] = {0};
	u32 len;
	u8 err;
	u8 file_name[40] = {0};
	unsigned long s,ms,us;
	mtos_systime_get(&s,&ms,&us);
	
	sprintf((char*)file_name,"r:rcv%ld%03ld%03ld",s,ms,us);
	OS_PRINTF("get url[%s]:\r\n############file_name[%s]\r\n",url,file_name);
	str_asc2uni(file_name, unicode);
	if (0 ==  aqi.http_inited)
	{
		unsigned char * mem = SY_MALLOC(16*1024);
		Nw_Download_Init_Download_Manager();
		Nw_Download_Init(TASK_AIQIYI_HTTP, mem,16*1024);
		aqi.http_inited = 1;

	}

	ret = Nw_DownloadURLTimeout(url, file_name, timeout, NULL, NULL, extraHeaders, body, bodyLen);
	if (ret <= 0)
	{
		OS_PRINTF("get server resp failure!\n");
		ufs_delete(unicode,0);		
		return -1;
	}

	err = ufs_open(&fd, (tchar_t*)unicode, UFS_READ);
	if (err != UFS_OK)
	{
	    ufs_delete(unicode,0);
	    return -1;
	}

	ufs_read(&fd,buffer, bufferSize, &len);
	ufs_close(&fd);
	ufs_delete(unicode,0);

	return len;
}

int porting_http_get_with_malloc_internal(const char* url,
                     const char* extraHeaders,
                     const char* body,
                     unsigned int bodyLen,
                     unsigned int timeout,
                     char** out_buffer,              //response
                     unsigned int *out_bufferSize)
{
	int ret;
	ufs_file_t fd = {0};
	u16 unicode[48] = {0};
	u32 len;
	u8 err;
	u8 file_name[40] = {0};
	int file_len = 0;
	int time_out_array[4] = {5,5,10,10};
	int i;
	int *buf_tmp;
	unsigned long s,ms,us;
	mtos_systime_get(&s,&ms,&us);
	
	sprintf((char*)file_name,"r:rcv%ld%03ld%03ld",s,ms,us);
	str_asc2uni(file_name, unicode);
	OS_PRINTF("get url mac[%s]:\r\n############file_name[%s]\r\n",url,file_name);

	if (0 ==  aqi.http_inited)
	{
		unsigned char * mem = SY_MALLOC(1*1024);
		Nw_Download_Init_Download_Manager();
		Nw_Download_Init(TASK_AIQIYI_HTTP, mem,1*1024);
		aqi.http_inited = 1;

	}
	for (i=0;i<ARRAY_SIZE(time_out_array);i++)
	{
#ifdef USE_HTTP_GZIP
		if (strncasecmp(url,"https",5) != 0)
			ret = Nw_DownloadURLTimeout_Gzip(url, file_name, time_out_array[i], NULL, NULL, extraHeaders, body, bodyLen);
		else
			ret = Nw_DownloadURLTimeout(url, file_name, time_out_array[i], NULL, NULL, extraHeaders, body, bodyLen);
#else
		ret = Nw_DownloadURLTimeout(url, file_name, time_out_array[i], NULL, NULL, extraHeaders, body, bodyLen);
#endif
		if (ret <= 0)
		{
			OS_PRINTF("get server resp failure!\n");
		       ufs_delete(unicode,0);		
		}
		else
		{
			i = -1;
			break;
		}
	}
	if (i != -1)
	{
		OS_PRINTF("http get failure 5 times!\n");
		return -1;
	}

	err = ufs_open(&fd, (tchar_t*)unicode, UFS_READ);
	if (err != UFS_OK)
	{
	    ufs_delete(unicode,0);
	    return -1;
	}

	file_len = fd.file_size;
	buf_tmp = SY_CALLOC(1,file_len+4);
	if (buf_tmp == NULL)
	{
	       ufs_delete(unicode,0);
		return -1;
	}
	ufs_read(&fd,buf_tmp, file_len, &len);
	*out_bufferSize = file_len;
	*out_buffer = (void*)buf_tmp;
	ufs_close(&fd);
	ufs_delete(unicode, 0);	
	return len;
}


int porting_http_get_with_malloc_internal_with_no_loop(const char* url,
                     const char* extraHeaders,
                     const char* body,
                     unsigned int bodyLen,
                     unsigned int timeout,
                     char** out_buffer,              //response
                     unsigned int *out_bufferSize)
{
	int ret;
	ufs_file_t fd = {0};
	u16 unicode[48] = {0};
	u32 len;
	u8 err;
	u8 file_name[40] = {0};
	int file_len = 0;
	int *buf_tmp;
	unsigned long s,ms,us;
	mtos_systime_get(&s,&ms,&us);
	
	sprintf((char*)file_name,"r:rcv%ld%03ld%03ld",s,ms,us);
	str_asc2uni(file_name, unicode);
	OS_PRINTF("get url mac no loop[%s]:\r\n############file_name[%s]\r\n",url,file_name);

	if (0 ==  aqi.http_inited)
	{
		unsigned char * mem = SY_MALLOC(1*1024);
		Nw_Download_Init_Download_Manager();
		Nw_Download_Init(TASK_AIQIYI_HTTP, mem,1*1024);
		aqi.http_inited = 1;

	}
#ifdef USE_HTTP_GZIP	
	if (strncasecmp(url,"https",5) != 0)
		ret = Nw_DownloadURLTimeout_Gzip(url, file_name, timeout, NULL, NULL, extraHeaders, body, bodyLen);
	else
		ret = Nw_DownloadURLTimeout(url, file_name, timeout, NULL, NULL, extraHeaders, body, bodyLen);
#else
	ret = Nw_DownloadURLTimeout(url, file_name, timeout, NULL, NULL, extraHeaders, body, bodyLen);
#endif
	if (ret <= 0)
	{
		OS_PRINTF("get server resp failure!\n");
	       ufs_delete(unicode,0);		
		return -1;
	}

	err = ufs_open(&fd, (tchar_t*)unicode, UFS_READ);
	if (err != UFS_OK)
	{
	    ufs_delete(unicode,0);
	    return -1;
	}

	file_len = fd.file_size;
	buf_tmp = SY_CALLOC(1,file_len+4);
	if (buf_tmp == NULL)
	{
	       ufs_delete(unicode,0);
		return -1;
	}
	ufs_read(&fd,buf_tmp, file_len, &len);
	*out_bufferSize = file_len;
	*out_buffer = (void*)buf_tmp;
	ufs_close(&fd);
	ufs_delete(unicode, 0);	
	return len;
}



///
int porting_http_post_with_malloc_internal(const char* url,
                                           const char* postBody,
                                           unsigned int postBodyLen,
                                           const char* extraHeaders,
                                           unsigned int timeout,
                                           char** out_buffer,              //response
                                           unsigned int *out_bufferSize)
{

	int ret;
	ufs_file_t fd = {0};
	u16 unicode[48] = {0};
	u8 err;
	u8 file_name[40] = {0};
	int file_len = 0;
	int *buf_tmp;
	u32 len;
	unsigned long s,ms,us;
	mtos_systime_get(&s,&ms,&us);
	
	sprintf((char*)file_name,"r:snd%ld%03ld%03ld",s,ms,us);
	OS_PRINTF("post url[%s]:\r\n############file_name[%s]\r\n",url,file_name);
	str_asc2uni(file_name, unicode);

#ifdef USE_HTTP_GZIP		
	if (strncasecmp(url,"https",5) != 0)
		ret = Nw_DownloadURL_POST_Gzip(url, postBody, postBodyLen, file_name, extraHeaders, NULL, NULL, timeout);
	else
		ret = Nw_DownloadURL_POST_ex(url, postBody, postBodyLen, file_name, extraHeaders, NULL, NULL, timeout);
#else
	ret = Nw_DownloadURL_POST_ex(url, postBody, postBodyLen, file_name, extraHeaders, NULL, NULL, timeout);
#endif
	if (ret <= 0)
	{
		OS_PRINTF("post get server resp failure!\n");
	       ufs_delete(unicode,0);
		return -1;
	}

	err = ufs_open(&fd, (tchar_t*)unicode, UFS_READ);
	if (err != UFS_OK)
	{
	       ufs_delete(unicode,0);
		return -1;
	}
	file_len = fd.file_size;
	buf_tmp = SY_CALLOC(1,file_len+4);
	if (buf_tmp == NULL)
	{
	       ufs_delete(unicode,0);
		return -1;
	}
	ufs_read(&fd,buf_tmp, file_len, &len);
	*out_bufferSize = file_len;
	*out_buffer = (void*)buf_tmp;
	ufs_close(&fd);
	ufs_delete(unicode, 0);
	return len;
}

/*!
  Create a new task

  \param[in] p_taskname the task name
  \param[in] p_taskproc the task main process function
  \param[in] p_param parameters for task process function
  \param[in] nprio task priority
  \param[in] nstksize ths task stack size

  \return TRUE if success, FALSE if fail
  */
int porting_task_create(const char* p_taskname,
                        void (*p_taskproc)(void *p_param),
                        void  *p_param,
                        unsigned long nprio,
                        unsigned long nstksize)
{
	u32 *pstk = SY_MALLOC(nstksize);
	BOOL err = TRUE;
	static int index = TASK_AIQIYI_AD_MODULE_START;
	MT_ASSERT(p_taskname != NULL);
	MT_ASSERT(p_taskproc != NULL);
	if (pstk != NULL)
	{
		err = mtos_task_create((u8*)p_taskname, p_taskproc, p_param,nprio + index, pstk,nstksize);
		if (err)
			return 0;
	}
	return -1;
}

/*!
  Suspend a task for some time

  \param[in] ms the time to sleep, unit: ms
  */
void porting_task_sleep(unsigned long ms)
{
	mtos_task_sleep(ms);
}

/*!
  Create a mutex

  \param[out] pevent a handle for mutex

  \return 0 if OK, !=0 if fail
  */
int porting_mutex_create(unsigned int * phandle)
{
	MT_ASSERT(phandle != NULL);
	if (mtos_sem_create(phandle,TRUE))
		return 0;
	return -1;
}

/*!
  Destroy a mutex

  \param[in] pevent

  \return 0 if OK, !=0 if fail
  */
int porting_mutex_destroy(unsigned int * phandle)
{
	MT_ASSERT(phandle != NULL);
	if (mtos_sem_destroy(phandle, MTOS_DEL_ALWAYS))
		return 0;
	return -1;
}

/*!
  lock

  \param[in] pevent

  \return 0 if OK, !=0 if fail
  */
int porting_mutex_lock(unsigned int * phandle)
{
	MT_ASSERT(phandle != NULL);
	if (mtos_sem_take(phandle,0))
		return 0;
	return -1;
}



/*!
  try lock

  \param[in] pevent,delay ms,0,wait forever

  \return 0 if OK, !=0 if fail
  */
int porting_mutex_try_lock(unsigned int * phandle,unsigned int ms)
{
	MT_ASSERT(phandle != NULL);
	if (mtos_sem_take(phandle,ms))
		return 0;
	return -1;
}



/*!
  unlock

  \param[in] pevent

  \return 0 if OK, !=0 if fail
  */
int porting_mutex_unlock(unsigned int * phandle)
{
	MT_ASSERT(phandle != NULL);
	if (mtos_sem_give(phandle))
		return 0;
	return -1;
}

/*!
  Create a semaphore

  \param[out] pSemID the semaphore ID created

  \return 0 if OK, !=0 if fail
  */
int porting_sem_create(unsigned int* pSemID)
{
	MT_ASSERT(pSemID != NULL);
	if (mtos_sem_create(pSemID,FALSE))
		return 0;
	return -1;}

/*!
  Destroy a semaphore

  \param[in] pSemID the semaphore ID to destroy

  \return 0 if OK, !=0 if fail
  */
int porting_sem_destroy(unsigned int* pSemID)
{
	MT_ASSERT(pSemID != NULL);
	if (mtos_sem_destroy(pSemID, MTOS_DEL_ALWAYS))
		return 0;
	return -1;
}
/*!
  Singal a semaphore

  \param[in] pSemID the pointer to semaphore to signal

  \return return 0 if OK, !=0 if fail
  */
int porting_sem_signal(unsigned int* pSemID)
{
	MT_ASSERT(pSemID != NULL);
	if (mtos_sem_give(pSemID))
		return 0;
	return -1;
}

/*!
  Wait for a semaphore

  \param[in] pSemID the pointer to semaphore to wait
  \param[in] ms time out value in ms, 0: wait forever

  \return 0 if OK, !=0 if fail
  */
int porting_sem_wait(unsigned int* pSemID,unsigned long ms)
{
	MT_ASSERT(pSemID != NULL);
	if (mtos_sem_take(pSemID,ms))
		return 0;
	return -1;
}



/*!
return rand value(32bit)
  */
unsigned int porting_gen_rand(void)
{
	return rand();
}

/*!
return rand value(32bit)
  */
unsigned long porting_gen_systime_ms(void)
{
	unsigned long s,ms,us;
	mtos_systime_get(&s,&ms,&us);
	return s*1000+ms;
}

unsigned long _mktime (unsigned int year, unsigned int mon,
    unsigned int day, unsigned int hour,
    unsigned int min, unsigned int sec)
        {
    if (0 >= (int) (mon -= 2)) {    /* 1..12 -> 11,12,1..10 */
         mon += 12;      /* Puts Feb last since it has leap day */
         year -= 1;
    }

    return (((
             (unsigned long) (year/4 - year/100 + year/400 + 367*mon/12 + day) +
             year*365 - 719499
          )*24 + hour /* now have hours */
       )*60 + min /* now have minutes */
    )*60 + sec; /* finally seconds */
}


unsigned long porting_gen_systime_utc_time(void)
{
	utc_time_t utc_time;
	unsigned long time;
	time_get(&utc_time, 1);
	time = _mktime(utc_time.year,utc_time.month,utc_time.day,utc_time.hour,utc_time.minute,utc_time.second);
	return (unsigned long)time;
}


/*!
  suspend a task with prio
  \return 0 if OK, !=0 if fail
  */
int porting_task_suspend(unsigned int prio)
{
	if (mtos_task_suspend((u8)prio) == 0)
		return 0;
	return -1;
}


/*!
  resume a task with prio
  \return 0 if OK, !=0 if fail
  */
int porting_task_resume(unsigned int prio)
{
	if (mtos_task_resume((u8)prio) == 0)
		return 0;
	return -1;
}

/*!
  return a task's prio
  \return task's prio
  */
int porting_get_current_prio()
{
    return mtos_task_get_curn_prio();
}

