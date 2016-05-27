
#ifndef __LIB_UTIL_H__
#define __LIB_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sys_types.h"

/*! 
  UTC format time definition
  */
typedef struct
{
  /*!
    Year
    */
  u16 year;
  /*!
    Month
    */
  u8 month;
  /*!
    Day
    */
  u8 day;
  /*!
    Hour
    */
  u8 hour;
  /*!
    Minute
    */
  u8 minute;
  /*!
    Sec
    */
  u8 second;  
  /*!
    researved.
    */
  u8 reserved;
} utc_time_t;

u32 make32(u8 *p_addr);
void time_point_trans(u8 *in, utc_time_t *out);
void date_trans(u8 *in, utc_time_t *out);

void timecpy(utc_time_t *p_cpy, utc_time_t *p_src);

#ifdef __cplusplus
}
#endif


#endif
