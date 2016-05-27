#ifndef __FLASH_RECORDS_H__
#define __FLASH_RECORDS_H__


int flash_records_init(void);

int record_get(u8* p_key, u8* p_val_buf, u32 bufsize, u16* p_val_len);

int record_set(u8* p_key, u8* p_val, u16 val_len);


#endif
