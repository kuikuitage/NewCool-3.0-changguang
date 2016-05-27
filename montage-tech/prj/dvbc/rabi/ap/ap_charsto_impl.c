/******************************************************************************/
/******************************************************************************/
#include "sys_types.h"
#include "sys_define.h"
#include "sys_cfg.h"
#include "string.h"
#include "mtos_printk.h"
#include "mtos_mem.h"
#include "drv_dev.h"
#include "charsto.h"
#include "ap_charsto_impl.h"

static struct charsto_device *p_char_dev = NULL;

charsto_protect_info_t e_char_prot_t[] = 
{
   //GD
   {FLASH_MF_GD, {PRT_UNPROT_ALL,  0x00000000, 0x407C, 2}},//0
   {FLASH_MF_GD, {PRT_PROT_ALL,    0x0000001C, 0x407C, 2}},//1
   {FLASH_MF_GD, {PRT_UPPER_1_64,  0x00000004, 0x407C, 2}},//2
   {FLASH_MF_GD, {PRT_UPPER_1_32,  0x00000008, 0x407C, 2}},//3
   {FLASH_MF_GD, {PRT_UPPER_1_16,  0x0000000c, 0x407C, 2}},//4
   {FLASH_MF_GD, {PRT_UPPER_1_8,   0x00000010, 0x407C, 2}},//5
   {FLASH_MF_GD, {PRT_UPPER_1_4,   0x00000014, 0x407C, 2}},//6
   {FLASH_MF_GD, {PRT_UPPER_1_2,   0x00000018, 0x407C, 2}},//7
   {FLASH_MF_GD, {PRT_LOWER_1_64,  0x00000024, 0x407C, 2}},//8
   {FLASH_MF_GD, {PRT_LOWER_1_32,  0x00000028, 0x407C, 2}},//9
   {FLASH_MF_GD, {PRT_LOWER_1_16,  0x0000002C, 0x407C, 2}},//10
   {FLASH_MF_GD, {PRT_LOWER_1_8,   0x00000030, 0x407C, 2}},//11
   {FLASH_MF_GD, {PRT_LOWER_1_4,   0x00000034, 0x407C, 2}},//12
   {FLASH_MF_GD, {PRT_LOWER_1_2,   0x00000038, 0x407C, 2}},//13
   {FLASH_MF_GD, {PRT_LOWER_3_4,   0x00004014, 0x407C, 2}},//14
   {FLASH_MF_GD, {PRT_LOWER_7_8,   0x00004010, 0x407C, 2}},//15
   {FLASH_MF_GD, {PRT_LOWER_15_16, 0x0000400c, 0x407C, 2}},//16
   {FLASH_MF_GD, {PRT_LOWER_31_32, 0x00004008, 0x407C, 2}},//17
   {FLASH_MF_GD, {PRT_LOWER_63_64, 0x00004004, 0x407C, 2}},//18
    //winbond
   {FLASH_MF_WINB, {PRT_UNPROT_ALL,  0x00000000, 0x407C, 2}},//0
   {FLASH_MF_WINB, {PRT_PROT_ALL,    0x0000001C, 0x407C, 2}},//1
   {FLASH_MF_WINB, {PRT_UPPER_1_64,  0x00000004, 0x407C, 2}},//2
   {FLASH_MF_WINB, {PRT_UPPER_1_32,  0x00000008, 0x407C, 2}},//3
   {FLASH_MF_WINB, {PRT_UPPER_1_16,  0x0000000c, 0x407C, 2}},//4
   {FLASH_MF_WINB, {PRT_UPPER_1_8,   0x00000010, 0x407C, 2}},//5
   {FLASH_MF_WINB, {PRT_UPPER_1_4,   0x00000014, 0x407C, 2}},//6
   {FLASH_MF_WINB, {PRT_UPPER_1_2,   0x00000018, 0x407C, 2}},//7
   {FLASH_MF_WINB, {PRT_LOWER_1_64,  0x00000024, 0x407C, 2}},//8
   {FLASH_MF_WINB, {PRT_LOWER_1_32,  0x00000028, 0x407C, 2}},//9
   {FLASH_MF_WINB, {PRT_LOWER_1_16,  0x0000002C, 0x407C, 2}},//10
   {FLASH_MF_WINB, {PRT_LOWER_1_8,   0x00000030, 0x407C, 2}},//11
   {FLASH_MF_WINB, {PRT_LOWER_1_4,   0x00000034, 0x407C, 2}},//12
   {FLASH_MF_WINB, {PRT_LOWER_1_2,   0x00000038, 0x407C, 2}},//13
   {FLASH_MF_WINB, {PRT_LOWER_3_4,   0x00004014, 0x407C, 2}},//14
   {FLASH_MF_WINB, {PRT_LOWER_7_8,   0x00004010, 0x407C, 2}},//15
   {FLASH_MF_WINB, {PRT_LOWER_15_16, 0x0000400c, 0x407C, 2}},//16
   {FLASH_MF_WINB, {PRT_LOWER_31_32, 0x00004008, 0x407C, 2}},//17
   {FLASH_MF_WINB, {PRT_LOWER_63_64, 0x00004004, 0x407C, 2}},//18
    //ISSI
   {FLASH_MF_ISSI, {PRT_UNPROT_ALL,  0x00000000, 0x3C, 1}},//1
   {FLASH_MF_ISSI, {PRT_PROT_ALL,    0x00000028, 0x3C, 1}},//2
   {FLASH_MF_ISSI, {PRT_UPPER_1_64,  0x00000004, 0x3C, 1}},//3
   {FLASH_MF_ISSI, {PRT_UPPER_1_32,  0x00000008, 0x3C, 1}},//4
   {FLASH_MF_ISSI, {PRT_UPPER_1_16,  0x0000000c, 0x3C, 1}},//5
   {FLASH_MF_ISSI, {PRT_UPPER_1_8,   0x00000010, 0x3C, 1}},//6
   {FLASH_MF_ISSI, {PRT_UPPER_1_4,   0x00000014, 0x3C, 1}},//7
   {FLASH_MF_ISSI, {PRT_UPPER_1_2,   0x00000018, 0x3C, 1}},//8
   {FLASH_MF_ISSI, {PRT_LOWER_1_64,  0x0000001C, 0x3C, 1}},//9
   {FLASH_MF_ISSI, {PRT_LOWER_1_32,  0x00000020, 0x3C, 1}},//10
   {FLASH_MF_ISSI, {PRT_LOWER_1_16,  0x00000024, 0x3C, 1}},//11
};


static RET_CODE find_protect_cmd(u8 m_id, charsto_prot_status_t *pro_st)
{
  u32 i = 0;
  RET_CODE ret = ERR_FAILURE;
  
  for(i = 0; i < sizeof(e_char_prot_t) / sizeof(charsto_protect_info_t); i++)
  {
    if((e_char_prot_t[i].m_id == m_id) && (e_char_prot_t[i].prot_st.prt_t == pro_st->prt_t))
    {
      pro_st->st = e_char_prot_t[i].prot_st.st;
      pro_st->len = e_char_prot_t[i].prot_st.len;
      pro_st->st_mask = e_char_prot_t[i].prot_st.st_mask;
      ret = SUCCESS;
    }
  }
  return ret;
}

static RET_CODE find_protect_type(u8 m_id, charsto_prot_status_t *pro_st)
{
  u32 i = 0;
  RET_CODE ret = ERR_FAILURE;
  
  for(i = 0; i < sizeof(e_char_prot_t) / sizeof(charsto_protect_info_t); i++)
  {
    if((e_char_prot_t[i].m_id == m_id) && (e_char_prot_t[i].prot_st.st == pro_st->st))
    {
      pro_st->len = e_char_prot_t[i].prot_st.len;
      pro_st->st_mask = e_char_prot_t[i].prot_st.st_mask;
      pro_st->prt_t = e_char_prot_t[i].prot_st.prt_t;
      ret = SUCCESS;
    }
  }
  return ret;
}


void ap_charsto_init()
{
  p_char_dev = (struct charsto_device *)dev_find_identifier(NULL, DEV_IDT_TYPE, 
                                                              SYS_DEV_TYPE_CHARSTO);

  MT_ASSERT(p_char_dev != NULL);
}

RET_CODE ap_charsto_set_protect(spi_prot_block_type_t p_t)
{
  RET_CODE ret = ERR_FAILURE;
  u32 mid = 0;
  charsto_prot_status_t pro_st;

  MT_ASSERT(p_char_dev != NULL);
  
  /*Get manufactor id to search the mach cmd for set protect type*/
#ifndef WIN32
  ret = dev_io_ctrl(p_char_dev, CHARSTO_IOCTRL_GET_DEV_ID, (u32)&mid);
  MT_ASSERT(ret == SUCCESS);
  
  mid = (mid & 0xFF);
  pro_st.prt_t = p_t;
  ret = find_protect_cmd(mid, &pro_st);
  
  
  ret = dev_io_ctrl(p_char_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&pro_st);
  if(ret != SUCCESS)
  {
     OS_PRINTF("set mid[%x] protect status[%x] fail!\n",mid, pro_st.st);
  }
  OS_PRINTF("set mid[%x] protect status[%x] type[%d]Success\n",mid, pro_st.st, p_t);
#endif
  return ret;
}

RET_CODE ap_charsto_cancel_protect(void)
{
  RET_CODE ret  = SUCCESS;
  charsto_prot_status_t st = {0};

  //unprotect
  st.prt_t = PRT_UNPROT_ALL;
  ret  = dev_io_ctrl(p_char_dev, CHARSTO_IOCTRL_SET_STATUS, (u32)&st);
  
  return ret;
}

spi_prot_block_type_t ap_charsto_get_protect()
{
  RET_CODE ret = ERR_FAILURE;
  
  charsto_prot_status_t pro_st_check;
  u32 mid = 0;
  
  MT_ASSERT(p_char_dev != NULL);

  /*Get manufactor id to search the mach cmd for set protect type*/
  ret = dev_io_ctrl(p_char_dev, CHARSTO_IOCTRL_GET_DEV_ID, (u32)&mid);
  MT_ASSERT(ret == SUCCESS);
  mid = (mid & 0xFF);
  
  ret = dev_io_ctrl(p_char_dev, CHARSTO_IOCTRL_GET_STATUS, (u32)&pro_st_check);
  MT_ASSERT(ret == SUCCESS);

  find_protect_type(mid, &pro_st_check);
  
  OS_PRINTF("get mid[%x] protect status[%x] type[%d]Success\n",mid, pro_st_check.st, pro_st_check.prt_t);
  return pro_st_check.prt_t;
}

