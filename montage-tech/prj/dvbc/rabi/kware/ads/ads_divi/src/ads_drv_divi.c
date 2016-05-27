/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys_types.h"
#include "sys_define.h"
#include "drv_dev.h"
#include "mtos_mem.h"
#include "mtos_task.h"
#include "mtos_misc.h"
#include "mtos_msg.h"
#include "mtos_sem.h"
#include "mtos_printk.h"
#include "mtos_timer.h"
#include "charsto.h"

#include "mem_manager.h"
#include "nim.h"
#include "dmx.h"

#include "ads_ware.h"
#include "ads_adapter.h"
#include "ads_api_divi.h"

#include "ui_common.h"
#include "ui_ad_api.h"

#include "sys_cfg.h"

//#define ADS_DIVI_DRV_PRINTF
#define ADS_DIVI_DRV_DEBUG
#ifdef ADS_DIVI_DRV_PRINTF
#define ADS_DRV_PRINTF OS_PRINTF
#else
#define ADS_DRV_PRINTF DUMMY_PRINTF
#endif
#ifdef ADS_DIVI_DRV_DEBUG
#define ADS_DRV_DEBUG OS_PRINTF
#else
#define ADS_DRV_DEBUG DUMMY_PRINTF
#endif

static divi_ads_data_t p_ads_divi_info[ADS_NUM];

extern ads_module_priv_t *p_ads_divi_priv;

extern void divi_ads_lock(void);
extern void divi_ads_unlock(void);

/*---------------------flash 接口--------------------*/
void divi_ads_erase_flash(u32 flashOffset, u32 size)
{
  s32 ret = 0;
  ADS_DRV_PRINTF("[divi],divi_drv_erase_flash \n");
  ADS_DRV_PRINTF(" divi_drv_erase_flash:0x%x, size:0x%x\n",flashOffset, size);
  
  ret = p_ads_divi_priv->nv_erase2(flashOffset, size);
  ADS_DRV_PRINTF("[divi_erase]ret=%d \n", ret);
}

static RET_CODE divi_drv_readflash(u32 flashOffset, u8 *extBuffer, u32 size)
{
  ADS_DRV_PRINTF("[divi],divi_drv_readflash \n");
  ADS_DRV_PRINTF(" divi_drv_readflash:0x%x   0x%x   0x%x\n",flashOffset, extBuffer, size);

  p_ads_divi_priv->nv_read((u32)flashOffset, extBuffer, &size);

  return TRUE;
}

static RET_CODE divi_drv_writeflash(u32 flashOffset, u8 * extBuffer, u32 size)
{
  ADS_DRV_PRINTF("[divi],divi_drv_writeflash \n");
  ADS_DRV_PRINTF(" [divi] divi_drv_writeflash:0x%x  0x%x  0x%x\n",flashOffset, extBuffer, size);

  p_ads_divi_priv->nv_write((u32)flashOffset, (u8 *)extBuffer, size);

  return TRUE;
}
  
//后续加入CRC校验或者备份机制
void divi_ads_read_data_from_flash(void)
{
  u16 i=0,j=0;
  u8 packet_flag = 1;
  
  OS_PRINTF("call divi_ads_read_data_from_flash total_size = %d\n",sizeof(divi_ads_data_t));

  for(i=0; i<ADS_NUM; i++)
  {
    divi_drv_readflash(FLASH_OFFSET*i,(u8 *)&(p_ads_divi_info[i]),sizeof(divi_ads_data_t));
    
    if (p_ads_divi_info[i].ads_file_type == 0xff)
    {
      OS_PRINTF("First init ads flash!\n");
      

#ifdef DIVI_ADS_VERSION_1506
	  if(i > 5)
		  p_ads_divi_info[i].ads_type = i+2;
	  else
		  p_ads_divi_info[i].ads_type = i+1;
#else
	  p_ads_divi_info[i].ads_type = i+1;
#endif

	  
      packet_flag = PACKET_NO_REV;
      p_ads_divi_info[i].ads_length = 0;
      p_ads_divi_info[i].ads_ver = 0;
      p_ads_divi_info[i].total_packet = 0;
      memset(p_ads_divi_info[i].ads_content, 0, 63*KBYTES);
      p_ads_divi_info[i].ads_is_finish = 0;
    }
    else
    {
      OS_PRINTF("Ads flash has been initlized! ads_file_type = %d\n",p_ads_divi_info[i].ads_file_type);
      packet_flag = PACKET_REV;
    }

    {
      for(j=0; j<500; j++)
      {
        p_ads_divi_info[i].packet_flag[j]= packet_flag;
      }
    }
  }
}


//AD_TYPE_T
u32 divi_ads_get_data(u8 ads_type, u8 *p_display, u8 *is_show, u8 *show_time, u8 *show_pos, u8 *file_type)
{ 
	u8 i;
	u32 len = 0;

#ifdef DIVI_ADS_VERSION_1506
	if(ads_type == IDN_ADS_PIC_ROLLING)
		return 0;

	if(ads_type > IDN_ADS_PIC_ROLLING)
		i = ads_type - 2;
	else
		i = ads_type - 1;
#else
	i = ads_type - 1;
#endif


	if((p_ads_divi_info[i].ads_type == ads_type) && (p_ads_divi_info[i].ads_is_finish == 1))
	{
		divi_ads_lock();  
		memcpy(p_display, p_ads_divi_info[i].ads_content, p_ads_divi_info[i].ads_length);
		*is_show = p_ads_divi_info[i].ads_is_show;
		*show_time = p_ads_divi_info[i].ads_show_time;
		*show_pos = p_ads_divi_info[i].ads_show_pos;
		*file_type = p_ads_divi_info[i].ads_file_type;
		len = p_ads_divi_info[i].ads_length;
		divi_ads_unlock();
	}

	if (len == 0)
		OS_PRINTF("Get ads data failed!\n");

	return len;
}

void divi_ads_set_ads_ver(u8 ads_type)
{
	p_ads_divi_info[ads_type].ads_ver = 0;
	p_ads_divi_info[ads_type].ads_is_finish = 0;
	OS_PRINTF("\n###############################  divi_ads_set_ads_ver p_ads_divi_info[%d].ads_ver = 0 \n",ads_type);
}

void divi_ads_add_data(u8 *p_got_data,u32 got_data_len)
{
	u8 * data = p_got_data;
	
	u16 EMM_length = MAKE_WORD(data[2],(data[1] & 0x0f));
	u16 Current_packet = MAKE_WORD(data[8],data[7]);
	u16 Total_packet = MAKE_WORD(data[10],data[9]);
	u8 AD_type 		= data[11];
	u8 File_type 	= data[12];
	u32 AD_CMD 		= ((u32)data[13] << 16) | ((u32)data[14] << 8) | (u32)data[15];//MT_MAKE_DWORD(data[15], MAKE_WORD(data[14], data[13]));
	u8 *Picture_content 	= &data[16];
	u8  Picture_is_show 	= data[176];
	u8  Picture_show_time 	= data[177];//MAKE_WORD(data[178],data[177]);
	u8  Picture_show_pos 	= data[178];

	u32 Picture_length 	= EMM_length - 20;
	u32 PacketSize = PACKET_SIZE;
	u32 ads_flash_offset = 0;
	u8  i;

	#ifdef DIVI_ADS_VERSION_1506
	if(AD_type == IDN_ADS_PIC_ROLLING)
		return;

	if(AD_type > IDN_ADS_PIC_ROLLING)
		i = AD_type - 2;
	else
		i = AD_type - 1;
	#else
	i = AD_type - 1;
	#endif

	ADS_DRV_PRINTF("Current_packet = %d, Total_packet = %d,AD_type = %d, File_type = %d, AD_CMD = %d,Picture_length = %d\n",
	  		Current_packet,Total_packet,AD_type,File_type,AD_CMD,Picture_length);

	if(AD_type > ADS_NUM)
	{
		ADS_DRV_PRINTF("[%s]%d AD_type = %d \n",AD_type);
		return;
	}

	if(p_ads_divi_info[i].ads_type == AD_type)
	{
		if((p_ads_divi_info[i].ads_ver == AD_CMD)&&   
			(p_ads_divi_info[i].packet_flag[Current_packet] == PACKET_REV))//版本相同且已经收过
		{
			ADS_DRV_PRINTF("HAVE FINISH ad_type = %d, packet = %d, ver = %d had rev\n",AD_type,Current_packet,AD_CMD);
			return;
		}   
		else if((AD_CMD > 0 && p_ads_divi_info[i].ads_ver < AD_CMD ) ||
				((p_ads_divi_info[i].ads_ver == 0xFFFFFF) &&
				(p_ads_divi_info[i].ads_ver != AD_CMD)))
		{

			if(p_ads_divi_info[i].ads_ver > 0 && p_ads_divi_info[i].ads_ver != 0xFFFFFF)
			{
				if(AD_CMD > p_ads_divi_info[i].ads_ver * 100)
				{
					ADS_DRV_PRINTF("\n ad_type = %d,  AD_CMD > p_ads_divi_info[i].ads_ver * 100\n",AD_type);
					return;
				}
			}
			//版本不同，需要重新分配
			RE_ADD_DATA:
			divi_ads_lock();

			p_ads_divi_info[i].ads_is_finish = 0;
			memset(p_ads_divi_info[i].ads_content, 0, 63*KBYTES);
			memset(p_ads_divi_info[i].packet_flag, 0, 500);         
			divi_ads_unlock();

			p_ads_divi_info[i].ads_length = 0;
			p_ads_divi_info[i].cur_packet = 0;
			p_ads_divi_info[i].total_packet = Total_packet;
			p_ads_divi_info[i].ads_file_type = File_type;
			p_ads_divi_info[i].ads_type = AD_type;
			p_ads_divi_info[i].ads_ver = AD_CMD;
			p_ads_divi_info[i].ads_is_show = Picture_is_show;
			p_ads_divi_info[i].ads_show_time = Picture_show_time;
			p_ads_divi_info[i].ads_show_pos = Picture_show_pos;
		}
		else if((p_ads_divi_info[i].ads_ver > AD_CMD))
		{
			ADS_DRV_PRINTF("ad_type = %d, ver = %d,  packet = %d ver is lower than current\n",
								AD_type, AD_CMD, Current_packet);
			if(p_ads_divi_info[i].ads_is_finish == 0)
			{
				goto RE_ADD_DATA;
			}
			else
			{
				return;
			}
		}
	}

	//SAVE DATA  
	p_ads_divi_info[i].ads_length += Picture_length;
	//广告数据大于63KB时将被清空，防止内存越界

	if (p_ads_divi_info[i].ads_length > 63 * KBYTES)
	{ 
		OS_PRINTF("Ads data is larger than 63 KB, and it should be cleard!\n");
		memset(&p_ads_divi_info[i], 0, sizeof(divi_ads_data_t));
		return;
	}

	memcpy(&(p_ads_divi_info[i].ads_content[PacketSize*(Current_packet-1)]),Picture_content,Picture_length);
	p_ads_divi_info[i].packet_flag[Current_packet] = PACKET_REV;
	p_ads_divi_info[i].cur_packet++;
  

	if(p_ads_divi_info[i].cur_packet == p_ads_divi_info[i].total_packet)
	{
		OS_PRINTF("================================= FINISH PACKET =================================\n");
		OS_PRINTF("Total_packet = %d,AD_type = %d, File_type = %d, AD_CMD = %d,ads_length = %d finish\n",
				Total_packet,AD_type,File_type,AD_CMD,p_ads_divi_info[i].ads_length);

		//cur pic rev finish
		p_ads_divi_info[i].ads_is_finish = 1;

		ads_flash_offset = FLASH_OFFSET * (p_ads_divi_info[i].ads_type-1);
		divi_ads_erase_flash(ads_flash_offset, ADS_SIZE);  
		divi_drv_writeflash(ads_flash_offset, (u8 *)(&p_ads_divi_info[i]), sizeof(divi_ads_data_t));
	}

}


static BOOL divi_get_ads_rolling_update(u8 ads_type, u8 *ads_time)
{
	static u32 osd_rolling_during[2] = {0};
	static BOOL osd_rolling_init = FALSE; 
	u8 real_pos = 0;

	if(!ui_is_playing())
		return FALSE;

	#ifdef DIVI_ADS_VERSION_1506
	real_pos = ads_type - 2;
	#else
	real_pos = ads_type - 1;
	#endif

	if((p_ads_divi_info[real_pos].ads_type == ads_type) && (p_ads_divi_info[real_pos].ads_is_finish == 1))
	{
		if(osd_rolling_during[ads_type - IDN_ADS_PIC_ROLLING] != p_ads_divi_info[real_pos].ads_ver)
		{
			if(ads_time != NULL)
				*ads_time = p_ads_divi_info[real_pos].ads_show_time;
			osd_rolling_during[ads_type - IDN_ADS_PIC_ROLLING] = p_ads_divi_info[real_pos].ads_ver;
			osd_rolling_init = TRUE;
			OS_PRINTF("[%s]%d\n",__FUNCTION__,__LINE__);
			return TRUE;
		}
		else
		{
			if((FALSE == osd_rolling_init) &&(p_ads_divi_info[real_pos].ads_ver == 0))
			{
				osd_rolling_init = TRUE;
				OS_PRINTF("[%s]%d\n",__FUNCTION__,__LINE__);
				return TRUE;
			}
			else
			{
				osd_rolling_init = TRUE;
				return FALSE;
			}
		}
	}

	return FALSE;
}

extern void send_event_to_ui_from_ads(u32 event, u32 param);
void divi_ads_rolling_time(u32 param)
{
	OS_PRINTF("[%s]%d param = %d\n",__FUNCTION__,__LINE__,param);
	send_event_to_ui_from_ads(CAS_C_ADS_MESSAGE, param);
}

static void divi_ads_data_monitor(void *p_data)
{
	static BOOL ads_corner_fisrt_show = FALSE;
#ifndef DIVI_ADS_VERSION_1506
	static s32 timer_rolling_handle[2] ={-1,-1};
	u32 during_time = 0;
	u8 i =0;
#endif
	u8 osd_during_time = 0;

	while(1)
	{
		if(FALSE == ads_corner_fisrt_show && ui_is_playing())
		{
			send_event_to_ui_from_ads(CAS_C_ADS_MESSAGE, IDN_ADS_CORNER);
			ads_corner_fisrt_show = TRUE;
		}

		#ifdef DIVI_ADS_VERSION_1506
		if(TRUE == divi_get_ads_rolling_update(IDN_ADS_TEXT_ROLLING, &osd_during_time))
		{
			divi_ads_rolling_time(IDN_ADS_TEXT_ROLLING);
		}
		#else
		for(i=IDN_ADS_PIC_ROLLING; i<=IDN_ADS_TEXT_ROLLING;i++)
		{
			if(TRUE == divi_get_ads_rolling_update(i, &osd_during_time))
			{
				divi_ads_rolling_time(i);
				during_time = (osd_during_time + 1)*(10)*(60)*(1000);

				if(timer_rolling_handle[i-IDN_ADS_PIC_ROLLING] == -1)
				{
					timer_rolling_handle[i-IDN_ADS_PIC_ROLLING] =
						mtos_timer_create(during_time, divi_ads_rolling_time, i, TRUE);
					if(timer_rolling_handle[i-IDN_ADS_PIC_ROLLING] < 0)
					{
						OS_PRINTF("[%s]%d pic mtos_timer_create fail\n",__FUNCTION__,__LINE__);
						return;
					}
					mtos_timer_start(timer_rolling_handle[i-IDN_ADS_PIC_ROLLING]);
				}
				else
				{
					OS_PRINTF("[%s]%d osd_during_time = %d\n",__FUNCTION__,__LINE__,osd_during_time);
					mtos_timer_reset(timer_rolling_handle[i-IDN_ADS_PIC_ROLLING], during_time);
				}
				
			}	
		}
		#endif

		mtos_task_sleep(40);
	}
}

u8 divi_ads_drv_init(void)
{
    s32 err = 0;
    u32 *p_stack = NULL;

    OS_PRINTF("DIVI_ADS_Initialize start! \n");
    p_stack = (u32 *)mtos_malloc(16 * 1024);
    MT_ASSERT(p_stack != NULL);

    err = mtos_task_create((u8 *)"ADS_Monitor",
            divi_ads_data_monitor,
            (void *)0,
            DIVI_ADS_MONITOR_TASK_PRIO,
            p_stack,
            16 * 1024);

    if (!err)
    {
      OS_PRINTF("Create ads data monitor task error = 0x%08x!\n", err);
      return 1;
    }
    OS_PRINTF("ADS_Monitor task create success!\n");
    return 0;
}

