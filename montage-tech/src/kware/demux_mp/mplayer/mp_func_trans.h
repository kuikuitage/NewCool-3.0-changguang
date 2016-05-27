#ifndef __MP_FUNC_TRANS__
#define __MP_FUNC_TRANS__
#include <string.h>
#include <sys/types.h>


#include "sys_types.h"
#include "sys_define.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_event.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "mtos_msg.h"
#include "mtos_timer.h"
#include "mtos_misc.h"
#include "list.h"
#include "drv_dev.h"
#include "block.h"
#include "ff.h"
#include "fsioctl.h"
#include "file_playback_sequence.h"

#include "ufs.h"

#ifndef __LINUX__
#define perror(args...) do{OS_PRINTF(args);}while(0)
#endif
#define reconnect_no -108
//typedef long off_t;
#ifdef __cplusplus
extern "C" {
#endif
#ifndef SEEK_SET
#  define SEEK_SET        0       /* Seek from beginning of file.  */
#  define SEEK_CUR        1       /* Seek from current position.  */
#  define SEEK_END        2       /* Set file pointer to EOF plus "offset" */
#endif
    typedef ufs_file_t FILE;
#define puts
#define putchar
#define printf  OS_PRINTF
    ///
    typedef enum {
        /*!
         FILE_PLAY_SOCKET
         */
        SOCKET_IDLE = 0,
        /*!
        FILE_PLAY_SOCKET
        */
        FILE_PLAY_SOCKET,
        /*!
        invalid type
        */
        SOCKETINFO_INVALID,
    } SOCKETINFO_TYPE;
    struct socket_info {
        int no;
        SOCKETINFO_TYPE type;

    };

    int record_new_socket(int domain, int type, int protocol, SOCKETINFO_TYPE info_type);
    int delete_socket(int no);

    int modify_sockets(SOCKETINFO_TYPE type);

    ///
#if 1

    /*
        memory function trans
    */
    static inline void * calloc(u32 n, u32 size)
    {
        void * buf = NULL;
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        FILE_SEQ_T * p_file_seq = file_seq_get_instance();

        if (p_file_seq->use_ext_heap == TRUE) {
            buf = p_file_seq->mem_alloc(p_file_seq, n * size);
        } else {
            buf = mtos_malloc(n * size);
        }

#else
        buf = mtos_malloc(n * size);
#endif

        if (buf) {
            memset(buf, 0x00, n * size);
        }

        return buf;
    }

    static inline void * malloc(u32 size)
    {
        void * buf = NULL;
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        FILE_SEQ_T * p_file_seq = file_seq_get_instance();

        if (p_file_seq->use_ext_heap == TRUE) {
            buf = p_file_seq->mem_alloc(p_file_seq, size);
        } else {
            buf = mtos_malloc(size);
        }

#else
        buf = mtos_malloc(size);
#endif
        return buf;
    }

    static inline void * realloc(void * old_ptr, u32 size)
    {
        void * new_ptr = NULL;
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        FILE_SEQ_T * p_file_seq = file_seq_get_instance();

        if (p_file_seq->use_ext_heap == TRUE) {
            new_ptr = p_file_seq->mem_alloc(p_file_seq, size);
        } else {
            new_ptr = mtos_malloc(size);
        }

#else
        new_ptr = mtos_malloc(size);
#endif

        if (new_ptr == NULL) {
            OS_PRINTF("%s: malloc memory failed(size:%d\n", __func__, size);
            return new_ptr;
        }

        if (old_ptr) {
            memcpy(new_ptr, old_ptr, size);
#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))

            if (p_file_seq->use_ext_heap == TRUE) {
                p_file_seq->mem_free(p_file_seq, old_ptr);
            } else {
                mtos_free(old_ptr);
            }

#else
            mtos_free(old_ptr);
#endif
        }

        return new_ptr;
    }

    static inline void free(void * ptr)
    {
        if (ptr == NULL) {
            return;
        }

#if  defined(USE_EXT_HEAP) && (!defined(__LINUX__))
        FILE_SEQ_T * p_file_seq = file_seq_get_instance();

        if (p_file_seq->use_ext_heap == TRUE) {
            p_file_seq->mem_free(p_file_seq, ptr);
        } else {
            mtos_free(ptr);
        }

#else
        mtos_free(ptr);
#endif
    }
    /*
        memory function trans
    */
    static inline void * mthttp_calloc(u32 n, u32 size)
    {
        void * buf = NULL;
        buf = mtos_malloc(n * size);

        if (buf) {
            memset(buf, 0x00, n * size);
        }

        return buf;
    }

    static inline void * mthttp_malloc(u32 size)
    {
        void * buf = NULL;
        buf = mtos_malloc(size);
        return buf;
    }

    static inline void * mthttp_realloc(void * old_ptr, u32 size)
    {
        void * new_ptr = NULL;
        new_ptr = mtos_malloc(size);

        if (new_ptr == NULL) {
            OS_PRINTF("%s: malloc memory failed(size:%d\n", __func__, size);
            return new_ptr;
        }

        if (old_ptr) {
            memcpy(new_ptr, old_ptr, size);
            mtos_free(old_ptr);
        }

        return new_ptr;
    }

    static inline void mthttp_free(void * ptr)
    {
        if (ptr == NULL) {
            return;
        }

        mtos_free(ptr);
    }
    char * mtstrdup(const char * s);

    static  inline char * mpstrdup(const char * s)
    {
        char * new_string = NULL;

        if (s == NULL) {
            return NULL;
        }

        new_string = (char *)malloc(strlen(s) + 1);

        if (new_string) {
            memset(new_string, 0, strlen(s) + 1);
            strcpy(new_string, s);
        }

        return new_string;
    }

    //char * strdup(const char * s);
#define  strdup   mpstrdup
    /*
        file function trans
    */
    //static inline int open(const char *pathname, int flags, mode_t mode)
    //{

    //}
#endif
#ifdef __cplusplus
}
#endif
#endif
