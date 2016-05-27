/******************************************************************************/
/******************************************************************************/
#ifndef __MEM_CFG_H__
#define __MEM_CFG_H__


//warriors

typedef enum
{
  MEMCFG_T_NORMAL,
}mem_cfg_t;

/*! Video buffer classification.*/
typedef enum
{
  BLOCK_SUB_32BIT_BUFFER = 0,
  BLOCK_OSD1_32BIT_BUFFER = 1,
  BLOCK_OSD0_32BIT_BUFFER = 2,
  BLOCK_STP_BUFFER = 3,
  BLOCK_EPG_BUFFER = 4,
} block_id_t;

#define STATIC_BLOCK_NUM    5

/*!
   Memory configuration
  */
#define SYS_MEMORY_END     0x3000000  //48M

#define SUB_32BIT_BUFFER_SIZE (120 * KBYTES)//(720*36*4) only need for line
#define SUB_32BIT_BUFFER_ADDR (0x3000000 - SUB_32BIT_BUFFER_SIZE)

#define OSD1_32BIT_BUFFER_SIZE (2300 * KBYTES)
#define OSD1_32BIT_BUFFER_ADDR (SUB_32BIT_BUFFER_ADDR - OSD1_32BIT_BUFFER_SIZE)

#define OSD0_32BIT_BUFFER_SIZE (2300 * KBYTES)//(960*600*4)
#define OSD0_32BIT_BUFFER_ADDR (OSD1_32BIT_BUFFER_ADDR - OSD0_32BIT_BUFFER_SIZE)

#define STP_32BIT_BUFFER_SIZE (0 * MBYTES)
#define STP_32BIT_BUFFER_ADDR (OSD0_32BIT_BUFFER_ADDR - STP_32BIT_BUFFER_SIZE)

#define EPG_BUFFER_SIZE   (0 * KBYTES)
#define EPG_BUFFER_ADDR   (STP_32BIT_BUFFER_ADDR - EPG_BUFFER_SIZE)
#define GUI_PARTITION_SIZE   (250 * KBYTES)

#define SYS_PARTITION_SIZE      (0x1800000)// (24 * MBYTES)//(7 * MBYTES+512*KBYTES)//(20 * 1024 * KBYTES)
#define SYS_PARTITION_ATOM           64

#define DMA_PARTITION_SIZE       (3 * 512 * KBYTES)
#define DMA_PARTITION_ATOM      64

/*!
  Set memory config mode
  \param[in] cfg parameter for memory confi
  */
void mem_cfg(mem_cfg_t cfg);
#endif

