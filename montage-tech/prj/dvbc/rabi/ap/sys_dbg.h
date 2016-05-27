#ifndef __SYS_DEBUG_H__
#define __SYS_DEBUG_H__
#include "stdio.h"
#include <string.h>
#include "stdarg.h"
#include "sys_define.h"

#define __DEBUG__
/*mode*/
enum
{
	MAIN,/*ca系统的debug信息*/
	DBG,
	NETAD,
	CAS,
	CAS_OSD,
	ADS,
	IPTVDP,
	IQY,
	IPLAY_O,
	IPLAY,
	UI_IPTV,
	UI_IPLAY_DESC,
	UI_PLAY_API,
	UI_PLAYER,
	UI_IPLAY_FAV,
	UI_IPTV_SEARCH,
	MAX_MOD_ID = 128
};




#define DISABLE     0   /*disable debug out put*/
#define EMERG	    1   /* system is unusable                   */
#define ERR		    2   /* error conditions                     */
#define WARNING	    3   /* warning conditions                   */
#define NOTICE	    4   /* normal but significant condition     */
#define INFO	    5   /* informational                        */

extern unsigned char debug_level[];
#define DEBUG_ENABLE_MODE(module,level)	{debug_level[module] = level;}
#define DEBUG_DISABLE_MODE(module)		{debug_level[module] = DISABLE;}
#ifdef __DEBUG__
#define DEBUG(module,level,fmt, ...)												\
    do {																		\
        if ((level <= debug_level[module]))		\
        OS_PRINTF("[%s>%s]%s():%d: "fmt,#module,#level, __FUNCTION__, __LINE__, ##__VA_ARGS__);		\
    } while (0)

#define P_INFO(module,level,fmt, ...)												\
    do {																		\
        if ((level <= debug_level[module]))		\
        OS_PRINTF("[%s>%s]"fmt,#module,#level, ##__VA_ARGS__);		\
    } while (0)	
    
#define P_PUT(module,level,fmt, ...)												\
    do {																		\
        if ((level <= debug_level[module]))		\
        OS_PRINTF(fmt,##__VA_ARGS__);		\
    } while (0)		    

#define DUMP(module,level,len,p,fmt) \
	do {	\
		if ((level <= debug_level[module])){		\
			int i__;\
			OS_PRINTF("[%s>%s]%s():%d: ",#module,#level, __FUNCTION__, __LINE__);		\
			OS_PRINTF("len = %d>>>",len);\
			for(i__=0;i__<len;i__++){\
				OS_PRINTF(fmt",",p[i__]);\
				if (i__&&i__%20 == 0)OS_PRINTF("\n");\
			}\
			OS_PRINTF("\n");\
		}\
	}while (0)

#else
#define DEBUG(mode,level,args...)		((void)0)
#define DUMP(len,p,fmt)		do{}while(0)
#endif



#ifdef __DEBUG__
	#define D_CHECK_BOOL(ret)  do{if(!ret)OS_PRINTF("On file:%s line %d >>> \n%s check bool return failure!\n",__FILE__,__LINE__,#ret);}while(0)
	#define D_CHECK_ZERO(ret)  do{if(ret != 0)OS_PRINTF("On file:%s line %d >>> \n%s check zero return failure!\n",__FILE__,__LINE__,#ret);}while(0)
    #define ASSERT(a) do{if(!(a))OS_PRINTF("Assert failure %s in %s:%d",#a,__FUNCTION__,__LINE__);}while(0)
#else
	#define D_CHECK_BOOL(ret) do{(ret);}while(0)
	#define D_CHECK_ZERO(ret) do{(ret);}while(0)
    #define ASSERT(a)   do{}while(0)
#endif

#define	CHECK_RET(module,ret)     do{ if(ret<0)DEBUG(module,ERR,"ret value err :%d\n",ret);}while(0)

void xn_debug_init(void);
#ifdef SHOW_MEM_SUPPORT
enum {
     AUDIO_FW_CFG_FLAG,
     AV_POWER_UP_FLAG,
     VID_SD_WR_BACK_FLAG,
     VID_DI_CFG_FLAG,
     VIDEO_FW_CFG_FLAG,
     OSD0_VSCALER_FLAG,
     OSD1_VSCALER_FLAG,
     SUB_VSCALER_FLAG,
     OSD0_LAYER_FLAG,
     OSD1_LAYER_FLAG,
     SUB_LAYER_FLAG,
     REC_BUFFER_FLAG,
     EPG_BUFFER_FLAG,
     GUI_RESOURCE_BUFFER_FLAG,
     AV_STACK_FLAG,
     CODE_SIZE_FLAG,
     SYSTEM_PARTITION_FLAG,
  };
void show_single_memory_mapping(unsigned int flag, unsigned int start_addr, unsigned int size);
#endif

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

void* __sy_malloc(unsigned int size,const char * file,const char *func,int line);
void* __sy_calloc(unsigned int n,unsigned int size,const char * file,const char *func,int line);
void __sy_free(void * m,const char * file,const char *func,int line);
void* __sy_realloc(void * prev,unsigned int size,const char * file,const char *func,int line);
void __sy_mem_status(const char *func,int line);

void SY_MEM_CHECK(void);
void SY_MEM_INIT(void);

//#define SY_MALLOC_DEBUG
#ifdef SY_MALLOC_DEBUG	
#define SY_MALLOC(x)   			__sy_malloc(x,__FILE__,__FUNCTION__,__LINE__)
#define SY_CALLOC(x,y)   			__sy_calloc(x,y,__FILE__,__FUNCTION__,__LINE__)
#define SY_FREE(x)   				__sy_free((void*)x,__FILE__,__FUNCTION__,__LINE__)
#define SY_REALLOC(prev,size)		__sy_realloc(prev,size,__FILE__,__FUNCTION__,__LINE__)
#define SY_MEM_STATUS_TRACE()   __sy_mem_status(__FUNCTION__,__LINE__)

#else
#define SY_MALLOC(x)   			mtos_malloc(x)
#define SY_CALLOC(x,y)   			mtos_calloc(x, y)
#define SY_FREE(x)   				mtos_free(x)	
#define SY_REALLOC(prev,size)		mtos_realloc(prev,size)

#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif

