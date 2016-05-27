#ifndef __DB_DVBS_H_
#define __DB_DVBS_H_

/*!
 \file db_dvbs.h

   This file defined data structures for Satellite, TP and Program management.
   And interfaces for high level modules and some middle-ware modules. Module
   DB is depend on OS, flash driver.

   Development policy:
   Modules who will call DB's interfaces should try it best to do data
   modifcation in memory first and reduce calling function \e DB_DVBS_WriteData
   times. This policy can protect flash and software will get more efficiency.
 */
#define INVALIDID                    DB_UNKNOWN_ID
#define INVALIDPOS                   DB_UNKNOWN_ID

/*!
   Max number of programs in flash and view depth of available tp node
 */
#define DB_DVBS_MAX_PRO              5000

/*!
   view depth of available tp node and Max number of TP node in Flash.
 */
#define DB_DVBS_MAX_TP               3000

/*!
   view depth of available SAT node and Max number of SAT node in Flash.
 */
#define DB_DVBS_MAX_SAT              64

/*!
   name length for Satellite and program
 */
#define DB_DVBS_MAX_NAME_LENGTH      16

#define DB_DVBS_MAX_AUDIO_CHANNEL   (16)

#define MAX_FAV_GRP                 (8)

#define DB_DVBS_PARAM_ACTIVE_FLAG   (0x8000)

#define MAX_VIEW_BUFFER_SIZE        (sizeof(item_type_t) * DB_DVBS_MAX_PRO)//(200*KBYTES)//
/*!
  Length of "no name"
  */
#define LEN_OF_NO_NAME              (8)
/*!
   Module return types
 */
typedef enum
{
  /*!
     DB error code. Means operation successed.
   */
  DB_DVBS_OK = 0,
  /*!
     DB error code. Means operation failed.
   */
  DB_DVBS_FAILED,
  /*!
     DB error code. Means database was full or already create 3 views.
   */
  DB_DVBS_FULL,
  /*!
     DB error code. Means the given data was duplicated with the data already
     existing in database.
   */
  DB_DVBS_DUPLICATED,
  /*!
     DB error code. Means database can't get a valid node from the given index.
   */
  DB_DVBS_NOT_FOUND
} db_dvbs_ret_t;


typedef enum
{
 /*! mark bit declares locked.*/
 DB_DVBS_MARK_SEL = 0,
 DB_DVBS_MARK_DEL = 1
}dvbs_view_mark_digit;



/*!
 * satellite struct
 */
typedef struct
{
  u32 id                  : 16;
  /*! 22K option on/off, 0: off, 1: on */
  u32 k22                 : 1;
  /*! 0v12v option on/off, 0: off, 1: on */
  u32 v12                 : 1;
  /*! DiSEqC type, 0: off 1: 1/4, 2: 1/8, 3: 1/16 */
  u32 diseqc_type         : 2;
  /*! DiSEqC port */
  u32 diseqc_port         : 4;
  /*! Motor position */
  u32 position            : 8;
  
  /*! DiSEqC1.1 type, 0: off 1: mode 1, 2: mode 2, 3: mode 3 */
  u32 diseqc11_type       : 2;
  /*! DiSEqC1.1 port */
  u32 diseqc11_port       : 4;
  /* 0 no positioner 1 DiSEqC1.2 positioner support 2 USALS positioner support*/
  u32 positioner_type     : 2;
  /*! LNB power option, 0: 13/18v 1: 13v 2: 18v */
  u32 lnb_power           : 2;
  /*! LNB types, 0: Standard, 1: user, 2: universal */
  u32 lnb_type            : 2;
  /*! polarity types, 0:auto 1: vertical, 2: horizontal */
  //u32 polarity            : 2;
  /*! toneburst 0: off, 1: A, 2: B */
  u32 toneburst           : 2;
  /*! longitude of sat */
  u32 longitude           : 16;
  /*! reserved for alignment */
  u32 reserved            : 2;
  
  /*! */
  u32 lnb_high            : 16;
  /*! */
  u32 lnb_low             : 16;

  /* 
  ---------------------------------------------------------
  above into data cache
   */
  
  /*! Satellite name by unicode*/
  u16  name[DB_DVBS_MAX_NAME_LENGTH + 1];
} sat_node_t;

/*!
 * tp structure
 */
typedef struct
{
  /*! frequency of TP */
  u32 freq;
  /*! symbol rate of TP */
  u32 sym;
  
  /*! The above will be saved firstly into data cache*/
  
  /*! NIT pid */
  u32 nit_pid     : 14;
  /*! polarity types, 0: horizontal, 1: vertical */
  u32 polarity    : 2;
  
  /*! TP node index in range of 0 - 299 */
  u32 id          : 16;
  /*! PAT version */
  u32 pat_version : 8;
  /*! Scrambled or not, 0: not scrambled, 1: scrambled */
  u32 is_scramble : 1;
  /*! flag on 22k or not at cur tp, 0: 22k off, 1: 22k on */
  u32 is_on22k    : 1;
  /*! nim type*/
  u32 nim_type  : 4;
  /*! reserved field for 32 bit alignment */
  u32 reserved    : 2;
  /*! Satellite node index */
  u32 sat_id      : 16;

  /* 
  ---------------------------------------------------------
  above into data cache
   */
} dvbs_tp_node_t;


#define MAX_TABLE_CACHE_SIZE (12) // 12bytes at structure head
#define MAX_VIEW_CACHE_SIZE  (sizeof(item_type_t))

typedef enum
{
  /*! A view contains all favorite radio programs */
  DB_DVBS_FAV_RADIO = 0,
  /*! A view contains all favorite TV programs */
  DB_DVBS_FAV_TV,
  /*! A view contains all favorite TV programs */
  DB_DVBS_FAV_ALL,
  /*! This view contains all radio programs */
  DB_DVBS_ALL_RADIO,
  /*! This view contains all TV programs */
  DB_DVBS_ALL_TV,
  /*! A view contains all TPs in a special satellite */
  DB_DVBS_SAT_TP,
  /*! A view contains all satellite */
  DB_DVBS_SAT,
  DB_DVBS_SELECTED_SAT,
  DB_DVBS_ALL_PG,
  DB_DVBS_ALL_TP,
  DB_DVBS_TP_PG,
  DB_DVBS_SAT_PG,
  DB_DVBS_SAT_ALL_TV,
  DB_DVBS_SAT_ALL_RADIO,
  /*! Invaild view*/
  DB_DVBS_INVALID_VIEW
} dvbs_view_t; /*! Enumator of view names */

typedef enum
{
  DB_DVBS_PG_NODE = 0,
  DB_DVBS_SAT_NODE,
  DB_DVBS_TP_NODE
}dvbs_sort_node_type_t;

typedef enum
{
  /*!Sorting pg name from A to Z*/
  DB_DVBS_A_Z_MODE = 0,
  /*!Sorting pg name from Z to A*/
  DB_DVBS_Z_A_MODE,
  /*!FTA first mode*/
  DB_DVBS_FTA_FIRST_MODE,
  /*!Lock last*/
  DB_DVBS_LOCK_FIRST_MODE,
  /*!Default order*/
  DB_DVBS_DEFAULT_ORDER_MODE,
}dvbs_sort_type_t;

typedef enum
{
  DB_DVBS_MARK_TV  = 0x0,
  DB_DVBS_MARK_LCK ,
  DB_DVBS_MARK_SKP ,
  DB_DVBS_FAV_GRP	 ,
  DB_DVBS_DEL_FLAG ,
  DB_DVBS_SEL_FLAG 
}dvbs_element_mark_digit;

/*!
 * program sort struct
 */
typedef struct
{
  u16  node_id;
  u16  name[DB_DVBS_MAX_NAME_LENGTH + 1];
  u32  is_scrambled:1;
  u32  lck_flag:1;
  u32  default_order;
}pro_sort_t;

/*!
 * program sort identify struct
 */
typedef struct
{
  u16  id_count;
  u16  id_offset;
  u16  id_start;
  u16  id_init;
}pro_sort_id_t;

/*!
 * program view identify struct
 */
typedef struct
{
  dvbs_sort_type_t  sort_type;
  u16  view_init;
  u16  view_count;
  u16 *p_mem;
}pro_view_id_t;

/*!
  defines check group info
  */
typedef struct tag_check_group_info
{
 /*!
  [IN]sat id.
  */
  u32 context;
 /*!
  [IN]filled the group, original tv pg id.
  */
  u16 orig_tv;
 /*!
  [IN]filled the group, original radio pg id.
  */
  u16 orig_rd;
 /*!
  [OUT]fill the group, original tv pg id.
  */
  BOOL tv_found;
 /*!
  [OUT]fill the group, original radio pg id.
  */
  BOOL rd_found;
 /*!
  [OUT]fill the group, original tv pg id.
  */
  u16 tv_first;
 /*!
  [OUT]fill the group, original radio pg id.
  */
  u16 rd_first;
}check_group_info_t;

typedef struct tag_spec_check_group
{
 /*!
  all group.
  */
  check_group_info_t all_group;
 /*!
  all satellite group
  */
  check_group_info_t sat_group[DB_DVBS_MAX_SAT];
 /*! 
  all favorite group
  */
  check_group_info_t fav_group[MAX_FAV_GRP];
}spec_check_group_t;

#endif // End for __DB_DVBS_H

