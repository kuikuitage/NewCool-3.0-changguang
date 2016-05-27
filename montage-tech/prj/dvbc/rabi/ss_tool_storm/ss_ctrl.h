#ifndef __SS_CTRL_H_
#define __SS_CTRL_H_

/*!
  \file: ss_ctrl.h
  This module provides the data structure about data written into system 
  status and the interface about how to operate sys_status data
  */

/*!
  ota trigger mode
  */
typedef enum
{
  /*!
    no ota.
    */
  OTA_TRI_NONE = 0,
  /*!
    ota auto
    */
  OTA_TRI_AUTO,
  /*!
    ota force.
    */
  OTA_TRI_FORC,
  /*!
    ota monitor(monitor in main code)
    */
  OTA_TRI_MONITOR,
  /*!
    ota monitor in oat loader
    */
  OTA_TRI_MON_IN_LOADER,
}ota_trigger_t;

/*!
  OTA information in dvbs
  */
typedef struct
{
  /*!
    OTA download flag in BAT mode 
    */
  ota_trigger_t ota_tri;
  /*!
    sys mode
    */
  sys_signal_t sys_mode;
  /*!
    The data pid of download stream
    */
  u16 download_data_pid;
  /*!
    New software version
    */
  u16 new_software_version; 
  /*!
    Original software version
    */
  u16 orig_software_version; 
  /*!
    dvbs lock info.
    */
  dvbs_lock_info_t locks;
  /*!
    dvbc lock info.
    */
  dvbc_lock_info_t lockc;
  /*!
    dvbt lock info
    */
  dvbt_lock_info_t lockt;
}ota_info_t;




#endif // End fo __SS_CTRL_H_
