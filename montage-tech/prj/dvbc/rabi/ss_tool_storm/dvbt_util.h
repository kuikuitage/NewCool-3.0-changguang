#ifndef __DVBT_UTIL_H_
#define __DVBT_UTIL_H_

/*!
  DVBT lock info
  */
typedef struct
{
  /*!
    freq
    */
  u32 tp_freq;    
  /*!
    symbol
    */
  u32 tp_sym;
  /*!
    band_width
    */
  u32 band_width;
} dvbt_lock_info_t;

#endif // End for __DVBS_UTIL_H_

