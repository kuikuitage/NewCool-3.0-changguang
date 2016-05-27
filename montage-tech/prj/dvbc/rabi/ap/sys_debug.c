/*
 * =====================================================================================
 *
 *       Filename:  gx_debug.c
 *
 *    Description:  用于CA打印debug信息
 *
 *        Version:  1.0
 *        Created:  2012年03月28日 13时53分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhangsy
 *        Company:  nationalchip
 *
 * =====================================================================================
 */
 #include "sys_dbg.h"
 #include "sys_types.h"
 #include "ui_common.h"

unsigned char debug_level[128];

void xn_debug_init(void)
{
	DEBUG_ENABLE_MODE(MAIN, ERR); 
	DEBUG_ENABLE_MODE(DBG, ERR);
	DEBUG_ENABLE_MODE(CAS, ERR);
	DEBUG_ENABLE_MODE(CAS_OSD, ERR);
	DEBUG_ENABLE_MODE(ADS, ERR);
	DEBUG_ENABLE_MODE(IPTVDP, ERR);
	DEBUG_ENABLE_MODE(IQY, ERR);
	DEBUG_ENABLE_MODE(IPLAY_O, ERR);
	DEBUG_ENABLE_MODE(IPLAY, ERR);
	DEBUG_ENABLE_MODE(UI_IPLAY_DESC, ERR);
	DEBUG_ENABLE_MODE(UI_PLAY_API, ERR);
	DEBUG_ENABLE_MODE(UI_PLAYER, ERR); 	
	DEBUG_ENABLE_MODE(UI_IPLAY_FAV, ERR);
	DEBUG_ENABLE_MODE(UI_IPTV_SEARCH, ERR);
	DEBUG_ENABLE_MODE(UI_IPTV, ERR);
}

#ifdef SHOW_MEM_SUPPORT
#define MEM_SHOW_MARK 0x1FFFFFFF
#define MEM_SHOW_MAX_NAME_SIZE 30
void show_single_memory_mapping(unsigned int flag, unsigned int start_addr, unsigned int size)
{
  u8 name[MEM_SHOW_MAX_NAME_SIZE] = {0};
  u32 s_addr = 0;
  u32 e_addr = 0;

  OS_PRINTF("S:,\n");
  OS_PRINTF("#Name,Start Addr,End Addr,Size,Note(KBytes),Total(KBytes),!\n");
  memset(name, 0, MEM_SHOW_MAX_NAME_SIZE);
  switch(flag)
  {
    case AUDIO_FW_CFG_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Audio_fw_cfg", strlen("Audio_fw_cfg"));
      break;
    case AV_POWER_UP_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Av_power_up", strlen("Av_power_up"));
      break;
    case VID_SD_WR_BACK_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Vid_sd_wr_back", strlen("Vid_sd_wr_back"));
      break;
    case VID_DI_CFG_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Vid_di_cfg", strlen("Vid_di_cfg"));
      break;
    case VIDEO_FW_CFG_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Video_fw_cfg", strlen("Video_fw_cfg"));
      break;
    case OSD0_VSCALER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Osd0_vscaler", strlen("Osd0_vscaler"));
      break;
    case OSD1_VSCALER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Osd1_vscaler", strlen("Osd1_vscaler"));
      break;
    case SUB_VSCALER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Sub_vscaler", strlen("Sub_vscaler"));
      break;
    case OSD0_LAYER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Osd0_layer", strlen("Osd0_layer"));
      break;
    case OSD1_LAYER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Osd1_layer", strlen("Osd1_layer"));
      break;
    case SUB_LAYER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Sub_layer", strlen("Sub_layer"));
      break;
    case REC_BUFFER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Rec_buffer", strlen("Rec_buffer"));
      break;
    case EPG_BUFFER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Epg_buffer", strlen("Epg_buffer"));
      break;
    case GUI_RESOURCE_BUFFER_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Gui_resource_buffer", strlen("Gui_resource_buffer"));
      break;
    case AV_STACK_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Av_stack", strlen("Av_stack"));
      break;
    case CODE_SIZE_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      size = size & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "Code_size", strlen("Code_size"));
      break;
    case SYSTEM_PARTITION_FLAG:
      s_addr = start_addr & MEM_SHOW_MARK;
      e_addr = s_addr + size;
      memcpy(name, "System_paritition", strlen("System_partition"));
      break;
    default:
      break;
  }
  OS_PRINTF("#%s,0x%x,0x%x,0x%x,%d!\n", name, s_addr, e_addr, size, size / 0x400);
  OS_PRINTF("E:,\n");
}
#endif


#ifdef SY_MALLOC_DEBUG	

#define SY_TRACR_MAX_LEN		96
#define SY_TRACR_MAX_COUNT	5000		

typedef struct _trace_info{
	int 	 			have_not_freed;
	char  			trace[SY_TRACR_MAX_LEN];
	char  			free_trace[SY_TRACR_MAX_LEN];
	
	void 				*addr;	
	unsigned int 		size;
}SY_MEM_TRACE_T;
typedef struct mem_info{
	int count;
	SY_MEM_TRACE_T infos[SY_TRACR_MAX_COUNT];
}SY_MEM_INFO_T;

static SY_MEM_INFO_T _sy_mem_info;
void* __sy_malloc(unsigned int size,const char * file,const char *func,int line)
{
	void * m;
	int i;
	m = mtos_malloc(size);
	for (i=0;i<SY_TRACR_MAX_COUNT;i++)
		if (_sy_mem_info.infos[i].have_not_freed == 0)
			break;
	if (i==SY_TRACR_MAX_COUNT)
		OS_PRINTF("SY malloc buffer full!!!!!!\n");
	else
	{
		_sy_mem_info.infos[i].addr = m;
		_sy_mem_info.infos[i].have_not_freed = 1;
		_sy_mem_info.infos[i].size = size;
		snprintf(_sy_mem_info.infos[i].trace,sizeof (_sy_mem_info.infos[i].trace),"%s()%d ",func,line);
		_sy_mem_info.infos[i].free_trace[0] = 0;
	}
		
	return m;
}

void* __sy_calloc(unsigned int n,unsigned int size,const char * file,const char *func,int line)
{
	void * m;
	int i;
	m = mtos_calloc(n,size);
	for (i=0;i<SY_TRACR_MAX_COUNT;i++)
		if (_sy_mem_info.infos[i].have_not_freed == 0)
			break;
	if (i==SY_TRACR_MAX_COUNT)
		OS_PRINTF("SY malloc buffer full!!!!!!\n");
	else
	{
		_sy_mem_info.infos[i].addr = m;
		_sy_mem_info.infos[i].have_not_freed = 1;
		_sy_mem_info.infos[i].size = size;
		snprintf(_sy_mem_info.infos[i].trace,sizeof (_sy_mem_info.infos[i].trace),"%s()%d ",func,line);
		_sy_mem_info.infos[i].free_trace[0] = 0;
	}
	return m;
}


void* __sy_realloc(void * prev,unsigned int size,const char * file,const char *func,int line)
{
	void * m;
	int i;
	unsigned int * mc;

	for (i=0;i<SY_TRACR_MAX_COUNT;i++)
	{
		if ((u32)prev == (u32)_sy_mem_info.infos[i].addr)
		{

			mc = (unsigned int *)(prev);
			mc += (_sy_mem_info.infos[i].size + 3)/4;
			if (*mc != 0x88888888) 
			{
				OS_PRINTF("%s  memory crossed when free!!!\n",_sy_mem_info.infos[i].trace);
				MT_ASSERT(0);				
			}
	
			if (_sy_mem_info.infos[i].have_not_freed == 0)
			{
				OS_PRINTF("%s  free two times\n",_sy_mem_info.infos[i].trace);
				OS_PRINTF("new free at %s() %d\n",func,line);
				OS_PRINTF("prev free at %s\n",_sy_mem_info.infos[i].free_trace);
				MT_ASSERT(0);
			}
			_sy_mem_info.infos[i].have_not_freed = 0;
			snprintf(_sy_mem_info.infos[i].free_trace,sizeof (_sy_mem_info.infos[i].free_trace),"%s()%d ",func,line);
			break;
		}
	}

	m = mtos_realloc(prev, size);
	for (i=0;i<SY_TRACR_MAX_COUNT;i++)
		if (_sy_mem_info.infos[i].have_not_freed == 0)
			break;
	if (i==SY_TRACR_MAX_COUNT)
		OS_PRINTF("SY malloc buffer full!!!!!!\n");
	else
	{
		_sy_mem_info.infos[i].addr = m;
		_sy_mem_info.infos[i].have_not_freed = 1;
		_sy_mem_info.infos[i].size = size;
		snprintf(_sy_mem_info.infos[i].trace,sizeof (_sy_mem_info.infos[i].trace),"%s()%d ",func,line);
		_sy_mem_info.infos[i].free_trace[0] = 0;
	}
	return m;
}




void __sy_free(void * m,const char * file,const char *func,int line)
{
	int i;
	int times = 1;
	unsigned int * mc;

	if (m == NULL)
	{
		OS_PRINTF("U Free NULL pointer!!\n");
		return;
	}
	for (i=0;i<SY_TRACR_MAX_COUNT;i++)
	{
		if ((u32)m == (u32)_sy_mem_info.infos[i].addr)
		{

			mc = (unsigned int *)(m);
			mc += (_sy_mem_info.infos[i].size + 3)/4;
			if (*mc != 0x88888888) 
				OS_PRINTF("%s  memory crossed when free!!!\n",_sy_mem_info.infos[i].trace);
	
			if (_sy_mem_info.infos[i].have_not_freed == 0)
			{
				while (times --)
				{
					OS_PRINTF("%s  free two times\n",_sy_mem_info.infos[i].trace);
					OS_PRINTF("new free at %s() %d\n",func,line);
					OS_PRINTF("prev free at %s\n",_sy_mem_info.infos[i].free_trace);
				}
			}
			_sy_mem_info.infos[i].have_not_freed = 0;
			snprintf(_sy_mem_info.infos[i].free_trace,sizeof (_sy_mem_info.infos[i].free_trace),"%s()%d ",func,line);
			break;
		}
	}
	if (i == SY_TRACR_MAX_COUNT)
		OS_PRINTF("SY Free mem not malloced!!!!!!\n");

	mtos_free(m);
}

void SY_MEM_INIT(void)
{
	int i;
	for (i=0;i<SY_TRACR_MAX_COUNT;i++)
	{
		_sy_mem_info.infos[i].have_not_freed = 0;
	}
}

void SY_MEM_CHECK(void)
{
	int i;
	for (i=0;i<SY_TRACR_MAX_COUNT;i++)
	{
		if (_sy_mem_info.infos[i].have_not_freed != 0)
		{
			OS_PRINTF("%s not free yet size[%d]\n",_sy_mem_info.infos[i].trace,_sy_mem_info.infos[i].size);
		}
	}
}

void __sy_mem_status(const char *func,int line)
{
	mem_user_dbg_info_t	 m;
	mtos_mem_user_debug(&m);
	OS_PRINTF("%s():%d!!alloced[%#x], peak[%#x],free[%#x]\r\n",
			func,line,m.alloced,m.alloced_peak,m.rest_size);
}
#else

void SY_MEM_CHECK(void)
{
}

#endif


