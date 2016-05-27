#ifndef __DVBC_UTIL_H_
#define __DVBC_UTIL_H_

/*!
    Modulation mode used for "struct nim_channel_info" "modulation"
  */
typedef enum nim_modulation 
{
    /*!
      Auto modulation detection
      */    
    NIM_MODULA_AUTO = 0,
    /*!
      BPSK
      */
    NIM_MODULA_BPSK,
    /*!
      QPSK
      */    
    NIM_MODULA_QPSK,
    /*!
      8PSK
      */    
    NIM_MODULA_8PSK,
    /*!
      QAM 16
      */    
    NIM_MODULA_QAM16,
    /*!
      QAM 32
      */    
    NIM_MODULA_QAM32,
    /*!
      QAM 64
      */    
    NIM_MODULA_QAM64,
    /*!
      QAM 128
      */    
    NIM_MODULA_QAM128,  
    /*!
      QAM 256
      */    
    NIM_MODULA_QAM256,  
} nim_modulation_t;

/*!
  DVBC lock info
  */
typedef struct
{
  /*!
    freq
    */
  u32 tp_freq;
  /*!
    sym
    */
  u32 tp_sym;
  /*!
    nim modulate
    */
  nim_modulation_t nim_modulate;
} dvbc_lock_info_t;

#endif // End for __DVBS_UTIL_H_
