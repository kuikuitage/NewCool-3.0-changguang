#ifndef __NIM_CTRL_SVC_H_
#define __NIM_CTRL_SVC_H_

typedef enum
{
  /*!
    DVBS
    */
  SYS_DVBS = 0,
  /*!
    DVBC
    */
  SYS_DVBC,
  /*!
    DVBT
    */
  SYS_DVBT
} sys_signal_t;

/*!
  nim ctrl disepc info
  */
typedef struct
{
  /*! 
    the montor is fixed
    */
  BOOL is_fixed;
  /*!
    If not zero means support position
    */
  u32 position_type;
  /*!
    Enabled by sat_position_type
    */
  u32 position;
  /*!
    If not zero means support diseqc 1.0
    */
  u32 diseqc_type_1_0;
  /*!
    Diseqc port value
    */
  u32 diseqc_port_1_0;
  /*!
    If not zero means support diseqc 1.1
    */
  u32 diseqc_type_1_1;
  /*!
    Diseqc port value
    */
  u32 diseqc_port_1_1;
  /*! 
    use diseqc1.2
    */
  BOOL used_DiSEqC12;
  /*!
    sat longitude
    */
  double d_sat_longitude;
  /*!
    local longitude
    */
  double d_local_longitude;
  /*!
    local latitude
    */
  double d_local_latitude;
  /*!
    diseqc x.1 switch's mode, 0: no feature, 1: Mode1, 2:Mode2, 3:Mode3
    */
  u8 diseqc_1_1_mode;  
} nc_diseqc_info_t;


#endif // End for __NIM_CTRL_SVC_H_

