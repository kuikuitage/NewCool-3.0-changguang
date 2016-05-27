#ifndef PORTING_FUNCTION_H_INCLUDED
#define PORTING_FUNCTION_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

//only C


///
char* porting_getMac(void);


///
int  porting_printf(const char *p_format, ...);


///
void* porting_malloc(unsigned int size);


///
void porting_free(void* pBuffer);

void* porting_realloc(void* prev,unsigned int size);

///
int porting_http_get(const char* url,
                     const char* extraHeaders,
                     const char* body,
                     unsigned int bodyLen,
                     unsigned int timeout,
                     char* buffer,              //response
                     unsigned int bufferSize);

///
int porting_http_get_with_malloc_internal(const char* url,
                                          const char* extraHeaders,
                                          const char* body,
                                          unsigned int bodyLen,
                                          unsigned int timeout,
                                          char** out_buffer,              //response
                                          unsigned int *out_bufferSize);
///
int porting_http_post_with_malloc_internal(const char* url,
                                           const char* postBody,
                                           unsigned int postBodyLen,
                                           const char* extraHeaders,
                                           unsigned int timeout,
                                           char** out_buffer,              //response
                                           unsigned int *out_bufferSize);

/*!
  Create a new task

  \param[in] p_taskname the task name
  \param[in] p_taskproc the task main process function
  \param[in] p_param parameters for task process function
  \param[in] nprio task priority
  \param[in] nstksize ths task stack size

  \return 0 if success, -1 if fail
  */
int porting_task_create(const char* p_taskname,
                        void (*p_taskproc)(void *p_param),
                        void  *p_param,
                        long nprio,
                        long nstksize);

/*!
  Suspend a task for some time

  \param[in] ms the time to sleep, unit: ms
  */
void porting_task_sleep(long ms);

/*!
  Create a mutex

  \param[out] pevent a handle for mutex

  \return 0 if OK, !=0 if fail
  */
int porting_mutex_create(void* pevent);    //out

/*!
  Destroy a mutex

  \param[in] pevent

  \return 0 if OK, !=0 if fail
  */
int porting_mutex_destroy(void* pevent);    //in

/*!
  lock

  \param[in] pevent

  \return 0 if OK, !=0 if fail
  */
int porting_mutex_lock(void* pevent);

/*!
  unlock

  \param[in] pevent

  \return 0 if OK, !=0 if fail
  */
int porting_mutex_unlock(void* pevent);

/*!
  Create a semaphore

  \param[out] pSemID the semaphore ID created

  \return 0 if OK, !=0 if fail
  */
int porting_sem_create(unsigned int* pSemID);     //out

/*!
  Destroy a semaphore

  \param[in] pSemID the semaphore ID to destroy

  \return 0 if OK, !=0 if fail
  */
int porting_sem_destroy(unsigned int* pSemID);

/*!
  Singal a semaphore

  \param[in] pSemID the pointer to semaphore to signal

  \return return 0 if OK, !=0 if fail
  */
int porting_sem_signal(unsigned int* pSemID);

/*!
  Wait for a semaphore

  \param[in] pSemID the pointer to semaphore to wait
  \param[in] ms time out value in ms, 0: wait forever

  \return 0 if OK, !=0 if fail
  */
int porting_sem_wait(unsigned int* pSemID,
                     long ms);


unsigned int porting_gen_rand(void);

unsigned long porting_gen_systime_ms(void);

unsigned long porting_gen_systime_utc_time(void);

/*!
  try lock

  \param[in] pevent,delay ms,0,wait forever

  \return 0 if OK, !=0 if fail
  */
int porting_mutex_try_lock(unsigned int * phandle,unsigned int ms);


/*!
  suspend a task with prio
  \return 0 if OK, !=0 if fail
  */
int porting_task_suspend(unsigned int prio);


/*!
  resume a task with prio
  \return 0 if OK, !=0 if fail
  */
int porting_task_resume(unsigned int prio);

/*!
  return a task's prio
  \return task's prio
  */
int porting_get_current_prio();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PORTING_FUNCTION_H_INCLUDED
