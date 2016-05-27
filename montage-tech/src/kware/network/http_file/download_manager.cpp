#ifdef __LINUX__
#include <stdlib.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/file.h>
#define mtos_printk printf
#else
#include "lwip/sockets.h"
#endif

#include "sys_types.h"
#include "mt_debug.h"
#include "sys_define.h"
#include "mtos_task.h"
#include "mtos_sem.h"

#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"

#include <assert.h>

#include "download_manager.h"
#include "download_api.h"

#define MAX_TASK_PRO 256

#define   DOWNLOAD_MANAGER_LOG(format, args...)                 OS_PRINTF(format, ##args)
#define   DOWNLOAD_MANAGER_DEBUG(format, args...)            OS_PRINTF(format, ##args)
#define   DOWNLOAD_MANAGER_ERROR(format, args...)           mtos_printk(format, ##args)

static DownloadManager download_manager[MAX_TASK_PRO];
static unsigned int dl_mgr_lock = 0;

static void take_the_lock()
{
    mtos_sem_take((os_sem_t *)(&dl_mgr_lock), 0);
}
static void give_the_lock()
{
    mtos_sem_give((os_sem_t *)(&dl_mgr_lock));
}

extern void   Nw_Download_Abort(http_download_mini * instance);

void http_download_mamager_init()
{
    int i;

    DOWNLOAD_MANAGER_LOG("[%s] start start... ...\n", __func__);
    for(i = 0; i < MAX_TASK_PRO; i++)
    {
        download_manager[i].abort_flag = FALSE;
        download_manager[i].task_pro = -1;
    }

    BOOL ret = mtos_sem_create((os_sem_t *)(&dl_mgr_lock), TRUE);
    MT_ASSERT(ret == TRUE);

    DOWNLOAD_MANAGER_LOG("[%s] end end... ...\n", __func__);
}

void   add_task_to_manager(int task_prio, BOOL abort_flag, http_download_mini * instance)
{
    take_the_lock();
    DOWNLOAD_MANAGER_LOG("[%s] start start... ...\n", __func__);

    download_manager[task_prio].abort_flag = abort_flag;
    download_manager[task_prio].task_pro = task_prio;
    download_manager[task_prio].instance= instance;

    DOWNLOAD_MANAGER_LOG("[%s] end end... ...\n", __func__);
    give_the_lock();
}

void   remove_task_from_manager(int task_prio)
{
    take_the_lock();
    DOWNLOAD_MANAGER_LOG("[%s] start start... ...\n", __func__);

    download_manager[task_prio].abort_flag = FALSE;
    download_manager[task_prio].task_pro = -1;

    DOWNLOAD_MANAGER_LOG("[%s] end end... ...\n", __func__);
    give_the_lock();
}

BOOL   check_task_download_running(int task_prio)
{
    take_the_lock();
    DOWNLOAD_MANAGER_LOG("[%s] start start... ...\n", __func__);

    if(download_manager[task_prio].task_pro > 0)
    {
        DOWNLOAD_MANAGER_LOG("[%s] TRUE done done ... ...\n", __func__);
        give_the_lock();
        return TRUE;
    }
    else
    {
        DOWNLOAD_MANAGER_LOG("[%s] FALSE done done ... ...\n", __func__);
        give_the_lock();
        return FALSE;
    }
}

void get_running_tasks_from_manager(int * task_list)
{
    take_the_lock();
    DOWNLOAD_MANAGER_LOG("[%s] start start... ...\n", __func__);

    int i = 0, running_tasks[MAX_TASK_PRO] = {0};

    for(i = 0; i < MAX_TASK_PRO; i++)
    {
        if(download_manager[i].task_pro > 0)
        {
            *task_list++ = download_manager[i].task_pro;
        }
    }
    DOWNLOAD_MANAGER_LOG("[%s] end end... ..\n", __func__);

    give_the_lock();
}

void   abort_download_task(int task_prio, BOOL abort_flag)
{
    take_the_lock();

    DOWNLOAD_MANAGER_LOG("[%s] start start... ...\n", __func__);
    if(abort_flag && download_manager[task_prio].task_pro > 0)
    {
        Nw_Download_Abort(download_manager[task_prio].instance);

        //remove the task
        download_manager[task_prio].abort_flag = FALSE;
        download_manager[task_prio].task_pro = -1;
    }
    DOWNLOAD_MANAGER_LOG("[%s] end end... ...\n", __func__);

    give_the_lock();
}


