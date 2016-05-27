#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"

// sys
#include "sys_types.h"
#include "sys_define.h"

#include "mtos_printk.h"


#include "drv_dev.h"
#include "charsto.h"

#include "sys_cfg.h"

#include "flash_records.h"

#define SECTOR_SIZE (64*1024)
#define SECTOR_COUNT (2)

#define MAX_REC_ALLOC_TBL_SIZE (256)
#define RECORDS_OFFSET (512)

#define RECORD_SIZE (512)
#define RECORD_COUNT_PER_SECTOR (127)

#define RECORD_CLEANUP_THRESHOLD (119)
#define MAX_KEY_LEN (32)
#define VAL_SIZE_SPACE (2)
#define MAX_VAL_LEN (RECORD_SIZE - MAX_KEY_LEN - VAL_SIZE_SPACE)

#define RECORD_ATTR_MASK (0xF000)
#define RECORD_IDX_MASK  (0x0FFF)

#define RECORD_FREE    (0xF)
#define RECORD_INVALID (0xE)
#define RECORD_VALID   (0xC)
#define RECORD_BACKUP  (0x8)
#define RECORD_DISABLE (0x0)


#define SWAP_ENDIAN_U16(x) (((x&0xff)<<8)|(x)>>8)
#define GET_REC_ATTR(x) (((x)&RECORD_ATTR_MASK)>>12)
#define GET_REC_IDX(x)  ((x)&RECORD_IDX_MASK)
#define GET_RECORD_OFFSET(x,y) (SECTOR_SIZE*x+RECORDS_OFFSET+RECORD_SIZE*y)


typedef struct
{
  u16 sect_attr:4;
  u16 sect_index:12;
}alloc_entry_t;

typedef struct
{
  alloc_entry_t alloc_entry[RECORD_COUNT_PER_SECTOR];
}alloc_table_t;

typedef struct
{
  u8 key[MAX_KEY_LEN];
  u8 value[MAX_VAL_LEN];
}record_t;

typedef struct
{
  u16 attr;
  u16 index;
}record_info_t;

static u8 sg_sector_buf[SECTOR_COUNT][SECTOR_SIZE];
static record_info_t sg_record_info[SECTOR_COUNT][RECORD_COUNT_PER_SECTOR];

int flash_read(int offset, u8* p_buf, u32 bufsize)
{
  u32 addr;
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);

  addr = FLASH_RECORDS_OFFSET+offset;
  if( SUCCESS != charsto_read(p_charsto_dev, addr, p_buf, bufsize) )
    MT_ASSERT(0);

  return SUCCESS;
}

static int flash_write(int offset, u8* buf, u32 size)
{
  u32 addr;
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);

  addr = FLASH_RECORDS_OFFSET + offset;
  charsto_unprotect_all(p_charsto_dev);
  if( SUCCESS != charsto_writeonly(p_charsto_dev, addr, buf, size) )
    MT_ASSERT(0);
  charsto_protect_all(p_charsto_dev);

  return SUCCESS;
}


static int set_rec_attr(u16 sector_idx, u16 record_idx, u16 attr)
{
  int offset;
  u16 entry;

  sg_record_info[sector_idx][record_idx].index = record_idx;
  sg_record_info[sector_idx][record_idx].attr = attr;

  entry = 0;
  entry |= ((attr<<12) & RECORD_ATTR_MASK);
  entry |= (record_idx & RECORD_IDX_MASK);
  
  entry = SWAP_ENDIAN_U16(entry);
  
  offset = SECTOR_SIZE * sector_idx + sizeof(alloc_entry_t) * record_idx;
  if( SUCCESS != flash_write(offset, (u8*)&entry, sizeof(entry)) )
  {
    return -1;
  }

  return SUCCESS;
}

int record_read_by_id(u16 sector_idx, u16 record_idx, u8* p_buf, u32 bufsize)
{
  int offset;

  offset = GET_RECORD_OFFSET(sector_idx, record_idx);
  if( SUCCESS != flash_read(offset, p_buf, bufsize) )
  {
    return -1;
  }

  return SUCCESS;
}

static int record_alloc(u16* p_sector_idx, u16* p_record_idx)
{
  int i, j;

  for(i = 0; i < SECTOR_COUNT; i++)
  {
    for( j = 0; j < RECORD_COUNT_PER_SECTOR; j++ )
    {
      if( RECORD_FREE == sg_record_info[i][j].attr )
      {
	*p_sector_idx = i;
	*p_record_idx = j;
	return SUCCESS;
      }
    }
  }

  return -1;
}


int record_restore(u16 sector_idx, u16 record_idx)
{
  u16 i,j;
  u8 record_buf[RECORD_SIZE] = {0};
  int offset;

  if( SUCCESS != record_read_by_id(sector_idx, record_idx, record_buf, sizeof(record_buf)) )
  {
    return -1;
  }

  if( SUCCESS != record_alloc(&i, &j) )
  {
    return -1;
  }

  //1. set record attr as RECORD_INVALID
  if( SUCCESS != set_rec_attr(i, j, RECORD_INVALID) )
    return -1;

  //2. write record data
  offset = GET_RECORD_OFFSET(i, j);
  if( SUCCESS != flash_write(offset, record_buf, RECORD_SIZE) )
    return -1;

  //3. set record attr as RECORD_VALID
  if( SUCCESS != set_rec_attr(i, j, RECORD_VALID) )
    return -1;

  if( SUCCESS != set_rec_attr(sector_idx, record_idx, RECORD_DISABLE) )
    return -1;

  return SUCCESS;
}

int flash_records_init()
{
  int i, j;
  u16 entry;
  u32 addr;
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);

  for(i = 0; i < SECTOR_COUNT; i++)
  {
    addr = FLASH_RECORDS_OFFSET+i*SECTOR_SIZE;
    if( SUCCESS != charsto_read(p_charsto_dev, addr, sg_sector_buf[i], SECTOR_SIZE) )
      MT_ASSERT(0);
  }

  for(i = 0; i < SECTOR_COUNT; i++)
  {
    for( j = 0; j < RECORD_COUNT_PER_SECTOR; j++ )
    {
      entry = *(u16*)(sg_sector_buf[i] + (j * 2));
      entry = SWAP_ENDIAN_U16(entry);
      sg_record_info[i][j].attr  = GET_REC_ATTR(entry);
      sg_record_info[i][j].index = GET_REC_IDX(entry);
      if( RECORD_BACKUP == sg_record_info[i][j].attr)
      {
        record_restore(i, j);
      }
    }
  }

  return SUCCESS;
}

int sector_erase(u16 sector_idx)
{
  u32 addr;
  charsto_device_t *p_charsto_dev = dev_find_identifier(NULL, DEV_IDT_TYPE,
    SYS_DEV_TYPE_CHARSTO);
  int i;

  for(i = 0; i < RECORD_COUNT_PER_SECTOR; i++)
  {
    sg_record_info[sector_idx][i].attr = RECORD_FREE;
    sg_record_info[sector_idx][i].index = RECORD_IDX_MASK;
  }
  memset(sg_sector_buf[sector_idx], 0xff, SECTOR_SIZE);

  addr = FLASH_RECORDS_OFFSET + SECTOR_SIZE * sector_idx;
  charsto_unprotect_all(p_charsto_dev);
  charsto_erase(p_charsto_dev, addr, 1);
  charsto_protect_all(p_charsto_dev);

  return SUCCESS;
}

static int record_flush(u8* p, u16 sector_idx, u16 record_idx)
{
  int offset;

  offset = SECTOR_SIZE * sector_idx +  RECORDS_OFFSET + RECORD_SIZE * record_idx;
  flash_write(offset, p, RECORD_SIZE);

  return SUCCESS;
}
static int record_move(u16 from_sect, u16 from_rec, u16 to_sect, u16 to_rec)
  {
  u8 *p_src, *p_dest;
  if( SUCCESS != set_rec_attr(from_sect, from_rec, RECORD_BACKUP) )
    return -1;
  if( SUCCESS != set_rec_attr(to_sect, to_rec, RECORD_INVALID) )
    return -1;
  p_src = sg_sector_buf[from_sect] + RECORDS_OFFSET + from_rec*RECORD_SIZE;
  p_dest = sg_sector_buf[to_sect] + RECORDS_OFFSET + to_rec*RECORD_SIZE;
  memcpy(p_dest, p_src, RECORD_SIZE);
  if( SUCCESS != record_flush(p_dest, to_sect, to_rec) )
    return -1;
  if( SUCCESS != set_rec_attr(to_sect, to_rec, RECORD_VALID) )
    return -1;
  if( SUCCESS != set_rec_attr(from_sect, from_rec, RECORD_DISABLE) )
    return -1;
  return SUCCESS;
}
static void records_cleanup(u16 sect_idx, u16 rec_idx)
{
  int i, j, k, m, n;
  u16 disabled_records_cnt;

  m = 0;
  n = RECORD_COUNT_PER_SECTOR * sect_idx + rec_idx;
  for( i = 0; i < n; i++ )
    {
    k = i / RECORD_COUNT_PER_SECTOR;
    j = i % RECORD_COUNT_PER_SECTOR;
    if( RECORD_VALID == sg_record_info[k][j].attr )
      {
      m++;
      record_move(k, j, sect_idx, rec_idx + m);
      }
  }
  for( i = 0; i < sect_idx; i++ )
      {
    disabled_records_cnt = 0;
    for( j = 0; j < RECORD_COUNT_PER_SECTOR; j++ )
    {
      if( RECORD_DISABLE == sg_record_info[i][j].attr
         || RECORD_INVALID == sg_record_info[i][j].attr )
      {
        disabled_records_cnt++;
      }
    }
    if( RECORD_COUNT_PER_SECTOR == disabled_records_cnt )
    {
      sector_erase(i);
    }
  }

  for( i = 0; i <= m; i++ )
  {
    record_move(sect_idx, rec_idx + i, 0, i);
  }

  sector_erase(sect_idx);
}


int record_search(u8* p_key, u16* p_sector_idx, u16* p_record_idx)
{
  int i, j;
  u8* p_rec_key;

  for( i = 0; i < SECTOR_COUNT; i++ )
  {
    for( j = 0; j < RECORD_COUNT_PER_SECTOR; j++ )
    {
      if( RECORD_VALID != sg_record_info[i][j].attr )
      {
	continue;
      }
      p_rec_key = sg_sector_buf[i] + RECORDS_OFFSET + RECORD_SIZE * j;
      if( 0 == strcmp(p_key, p_rec_key) )
      {
	*p_sector_idx = i;
	*p_record_idx = j;
	return SUCCESS;
      }
    }
  }

  return -1;
}

static int save_record_data(u16 sector_idx, u16 record_idx, u8* p_key, u8* p_val, u16 val_len)
{
  u8 *p, *p_dest;
  u8 rec_buf[RECORD_SIZE] = {0};
  int offset;

  p = rec_buf;
  strcpy(p, p_key);
  p += MAX_KEY_LEN;
  memcpy(p, &val_len, VAL_SIZE_SPACE);
  p += VAL_SIZE_SPACE;
  memcpy(p, p_val, val_len);

  offset = SECTOR_SIZE * sector_idx +  RECORDS_OFFSET + RECORD_SIZE * record_idx;
  p_dest = sg_sector_buf[sector_idx] + RECORDS_OFFSET + RECORD_SIZE * record_idx;
  memcpy(p_dest, rec_buf, RECORD_SIZE);

  flash_write(offset, rec_buf, RECORD_SIZE);

  return SUCCESS;
}


int record_set(u8* p_key, u8* p_val, u16 val_len)
{
  int key_len;
  u16 sector_idx, record_idx;
  u16 old_sector_idx, old_record_idx;
  u8 new_record_flag;
  BOOL b_cleanup_flag = FALSE;

  key_len = strlen(p_key);
  if( key_len > MAX_KEY_LEN-1 )
    return -1;
  if( val_len > MAX_VAL_LEN-1 )
    return -1;

  //-------- new record ----------------
  if( SUCCESS != record_alloc(&sector_idx, &record_idx) )
  {
      OS_PRINTF("NO SPACE!\n");
      return -1;
    }
  else if( SECTOR_COUNT-1 == sector_idx && record_idx > RECORD_CLEANUP_THRESHOLD)
  {
    b_cleanup_flag = TRUE;
  }

  //--------- check p_key exist or not ------------
  if( SUCCESS != record_search(p_key, &old_sector_idx, &old_record_idx) )
  {
    new_record_flag = 1;
  }
  else
  {
    new_record_flag = 0;
    if( SUCCESS != set_rec_attr(old_sector_idx, old_record_idx, RECORD_BACKUP) )
      return -1;
  }


  //1. set record attr as RECORD_INVALID
  if( SUCCESS != set_rec_attr(sector_idx, record_idx, RECORD_INVALID) )
    return -1;

  //2. write record data
  if( SUCCESS != save_record_data( sector_idx, record_idx, p_key, p_val, val_len) )
    return -1;

  //3. set record attr as RECORD_VALID
  if( SUCCESS != set_rec_attr(sector_idx, record_idx, RECORD_VALID) )
    return -1;

  if( 0 == new_record_flag )
  {
    if( SUCCESS != set_rec_attr(old_sector_idx, old_record_idx, RECORD_DISABLE) )
      return -1;
  }
  if( TRUE == b_cleanup_flag )
  {
    records_cleanup(sector_idx, record_idx);
  }

  return SUCCESS;
}

int record_delete(u8* p_key)
{
  u16 sector_idx, record_idx;

  if( SUCCESS != record_search(p_key, &sector_idx, &record_idx) )
    return -1;

  if( SUCCESS != set_rec_attr(sector_idx, record_idx, RECORD_DISABLE) )
    return -1;

  return 0;
}


int record_get(u8* p_key, u8* p_val_buf, u32 bufsize, u16* p_val_len)
{
  u16 sector_idx, record_idx;
  u8* p;
  u16 val_len;

  if( SUCCESS != record_search(p_key, &sector_idx, &record_idx) )
    return -1;

  p = sg_sector_buf[sector_idx] + RECORDS_OFFSET + record_idx*RECORD_SIZE + MAX_KEY_LEN;
  val_len = *(u16*)p;
  OS_PRINTF("key %s, val_len: %d\n", p_key,val_len);
  *p_val_len = val_len;
  p += VAL_SIZE_SPACE;
  if( bufsize < val_len)
    return -1;
  memcpy(p_val_buf, p, val_len);

  return SUCCESS;
}


