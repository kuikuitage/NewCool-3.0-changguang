

// system

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "stdafx.h"
#include "sys_types.h"
#include "ota_define_tools.h"



#define MAKE_WORD(low, high) \
  ((u16)(((u8)(low)) | \
         (((u16)((u8)(high))) << 8)))
/*!
  Combination of two byte(8bit) as a word(16bit)
  */
#define MAKE_WORD2(high, low) ((u16)(((u8)(low)) | \
                                       ((( u16)((u8)(high))) << 8)))

/*!
   Combination of two word(16bit) as a dword(32bit)
  */
#define MT_MAKE_DWORD(low, high) \
  ((u32)(((u16)(low)) | \
         (((u32)((u16)(high))) << 16)))
#define MT_MAKE_DWORD2(high, low) \
  ((u32)(((u16)(low)) | \
         (((u32)((u16)(high))) << 16)))

/*!
  Combination of two dword (32bit) as a lword(64bit)
  */
#define MT_MAKE_LWORD(low, high) \
  ((u64)(((u32)(low)) |           \
        (((u64)((u32)(high))) << 32)))

#define MT_MAKE_LWORD2(high, low) \
  ((u64)(((u32)(low)) |           \
        (((u64)((u32)(high))) << 32)))


/*!
 ota find dm header info.the function don't bind thi ota data manange,it only suport function api
  input param1: dm start pos distance buf,
  input param2:buffer,
  input param3,buffer size
  input param4,dmh info
  return TRUE,SUCCESS;
  */
BOOL mul_ota_dm_api_find_dmh_info(u32 dm_offset,u8 *p_buf,u32 buf_size,dm_dmh_info_t *dmh_info)
{
   u32 i = 0,j = 0,k = 0;
   u32 col_setup = 0x10000;
   u8 dmh_mark[OTA_DM_DMH_MARK_SIZE] ;//=(char *)("*^_^*DM(^o^)");
   u32 col_total = buf_size / col_setup;
   u32 row_total = 1024; /**1KBYTE**/ 
   u8 *p_header_start = NULL;
   u8 *p_block_start = NULL;
   u8 group_total = 0;
   u32 offset = 0;
   
   memset(dmh_mark,0,sizeof(u8) * OTA_DM_DMH_MARK_SIZE);
   memcpy(dmh_mark,"*^_^*DM(^o^)",sizeof(u8) * OTA_DM_DMH_MARK_SIZE);

  for(i = 0; i < col_total; i ++)
  {
      for(j = 0;j < row_total;j ++)
      {
          if(0 == memcmp(p_buf +(col_setup * i)+ j, dmh_mark, OTA_DM_DMH_MARK_SIZE))
           {
             /***dmh tag is 12 + 4****/
             p_header_start = p_buf +(col_setup * i)+ j + OTA_DM_DMH_TAG_TOTAL_SIZE;
             
             dmh_info->header_group[group_total].dmh_start_pos = (col_setup * i)+ j;
             dmh_info->header_group[group_total].sdram_size = MT_MAKE_DWORD(MAKE_WORD(p_header_start[0], p_header_start[1]), \
                                                                                                MAKE_WORD(p_header_start[2], p_header_start[3]));
             dmh_info->header_group[group_total].flash_size = MT_MAKE_DWORD(MAKE_WORD(p_header_start[4], p_header_start[5]), \
                                                                                              MAKE_WORD(p_header_start[6], p_header_start[7]));
             dmh_info->header_group[group_total].block_num =  MAKE_WORD(p_header_start[8], p_header_start[9]);
             dmh_info->header_group[group_total].bh_size = MAKE_WORD(p_header_start[10], p_header_start[11]);

             dmh_info->header_group[group_total].dm_offset = dmh_info->header_group[group_total].dmh_start_pos 
                                                                                               + dm_offset;
             
             if(dmh_info->header_group[group_total].block_num > OTA_MAX_BLOCK_NUMBER)
              {
                return FALSE;
              }
             for(k = 0; k < dmh_info->header_group[group_total].block_num;k++)
              {
                 p_block_start = p_header_start + 
					             OTA_DM_DMH_BASE_INFO_LEN + 
								 (k * dmh_info->header_group[group_total].bh_size);

                    dmh_info->header_group[group_total].dm_block[k].id = p_block_start[0];
                    dmh_info->header_group[group_total].dm_block[k].type = p_block_start[1];
                    dmh_info->header_group[group_total].dm_block[k].base_addr = 
                                       MT_MAKE_DWORD(MAKE_WORD(p_block_start[4], p_block_start[5]), \
                                                                      MAKE_WORD(p_block_start[6], p_block_start[7]));
                    dmh_info->header_group[group_total].dm_block[k].size = 
                                      MT_MAKE_DWORD(MAKE_WORD(p_block_start[8], p_block_start[9]), \
                                                                     MAKE_WORD(p_block_start[10], p_block_start[11]));
                    dmh_info->header_group[group_total].dm_block[k].crc = 
                                      MT_MAKE_DWORD(MAKE_WORD(p_block_start[12], p_block_start[13]), \
                                                                     MAKE_WORD(p_block_start[14], p_block_start[15]));
                   offset = dmh_info->header_group[group_total].dmh_start_pos + dm_offset;
                   dmh_info->header_group[group_total].dm_block[k].base_addr += offset;
				   memcpy(&dmh_info->header_group[group_total].dm_block[k].name,
						    &p_block_start[24],8);
                   
              }
             group_total ++;
             if(group_total > OTA_DM_DMH_MAX_GROUP_NUMBER)
              {
                 return FALSE;
              }
             dmh_info->header_num = group_total;
             break;
           }
      }
  }

  if(dmh_info->header_num == 0)
    {
      return FALSE;
    }
  
  return TRUE;
}

int check_block_exist_from_file_dm(dm_dmh_info_t *p_dmh_info,unsigned char block_id)
{
	unsigned char i = 0,j = 0;
	if(p_dmh_info->header_num > 0)
	{
		for(i = 0; i< p_dmh_info->header_num;i ++)
			{
			   for(j = 0;j < p_dmh_info->header_group[i].block_num ; j ++)
			   	{
			   	   if(p_dmh_info->header_group[i].dm_block[j].id == block_id)
			   	   	{
			   	   	  return TRUE;
			   	   	}
			   	}
			}
	}
  return FALSE;
}


unsigned int get_block_base_addr_from_file_dm(dm_dmh_info_t *p_dmh_info,unsigned char block_id)
{
	unsigned char i = 0,j = 0;
	if(p_dmh_info->header_num > 0)
	{
		for(i = 0; i< p_dmh_info->header_num;i ++)
			{
			   for(j = 0;j < p_dmh_info->header_group[i].block_num ; j ++)
			   	{
			   	   if(p_dmh_info->header_group[i].dm_block[j].id == block_id)
			   	   	{
			   	   	  return p_dmh_info->header_group[i].dm_block[j].base_addr;
			   	   	}
			   	}
			}
	}
  return 0;
}

unsigned int get_block_base_size_from_file_dm(dm_dmh_info_t *p_dmh_info,unsigned char block_id)
{
	unsigned char i = 0,j = 0;
	if(p_dmh_info->header_num > 0)
	{
		for(i = 0; i< p_dmh_info->header_num;i ++)
			{
			   for(j = 0;j < p_dmh_info->header_group[i].block_num ; j ++)
			   	{
			   	   if(p_dmh_info->header_group[i].dm_block[j].id == block_id)
			   	   	{
			   	   	  return p_dmh_info->header_group[i].dm_block[j].size;
			   	   	}
			   	}
			}
	}
  return 0;
}
unsigned int get_block_addr_offset_from_file_dm(dm_dmh_info_t *p_dmh_info,unsigned char block_id)
{
	unsigned char i = 0,j = 0;
	if(p_dmh_info->header_num > 0)
	{
		for(i = 0; i< p_dmh_info->header_num;i ++)
			{
			   for(j = 0;j < p_dmh_info->header_group[i].block_num ; j ++)
			   	{
			   	   if(p_dmh_info->header_group[i].dm_block[j].id == block_id)
			   	   	{
			   	   	  return p_dmh_info->header_group[i].dm_offset;
			   	   	}
			   	}
			}
	}
  return 0;
}

