#ifndef OTA_DEFINE_TOOLS_H_
#define OTA_DEFINE_TOOLS_H_

/*!
  define for customer
  */
//#define OTA_FILE_TOOLS_FOR_TDX_SPI
//#define OTA_FILE_TOOLS_FOR_SAITEWEI

#define ENABLE_BLOCK_UPG_SELECT_FLASH_ALL

#ifdef OTA_FILE_TOOLS_FOR_TDX_SPI
/*!
  fix select block assem,user by spi in tiandixin customer
  */
#define OTA_FIX_SELECT_BLOCK_UPG_ASSEM
#ifdef ENABLE_BLOCK_UPG_SELECT_FLASH_ALL
#undef ENABLE_BLOCK_UPG_SELECT_FLASH_ALL
#endif
#endif

#ifdef OTA_FILE_TOOLS_FOR_SAITEWEI
#ifdef ENABLE_BLOCK_UPG_SELECT_FLASH_ALL
#undef ENABLE_BLOCK_UPG_SELECT_FLASH_ALL
#endif
#endif

#define ENABLE_EDIT_BLOCK_VERSION_BY_ONESELF



#define IDC_CHECK_MAX_NUMBER 10
#define IDC_CHECK_BLOCK_NAME_SIZE 8

/*!
  ota can upg max blockdefine max ota block number.
  */
#define OTA_MAX_BLOCK_NUMBER 64
#define OTA_BLOCK_SIZE (64 * 1024)

#define UPG_DDM_HEAD_MARK 0x5AA55AA5

#define OTA_DM_DMH_MAX_GROUP_NUMBER 5
#define OTA_DM_DMH_MARK_SIZE 12
#define OTA_DM_DMH_TAG_TOTAL_SIZE 16 /***mark + 4byte**/
#define OTA_DM_DMH_BASE_INFO_LEN 12
/*!
   Todo: fix me
  */
#define TOOL_BOOTLOADER_ID          0x01
/*!
   Todo: fix me
  */
#define TOOL_BL_CONFIG_ID           0x02
/*!
   Todo: fix me
  */
#define TOOL_UPGCLIENT_ID           0x03

////////////////////////////////////////////////////////////////
//  Block ID definition, high 6bit:type, low 2bit:index
////////////////////////////////////////////////////////////////
/*!
  logo and preset in dm block
  */
#define DM_LOGO_PRESET_BISSKEY_BLOCK_ID    0x50
/*!
 AD video block
 */
#define AD_VIDEO_BLOCK_ID          0x70
/*!
  BOOTLOADER block
  */
#define BOOTLOADER_BLOCK_ID          0x80
/*!
  Data manager header ID
  */
#define DMH_BLOCK_ID                 0x84
/*!
  app dm block id
  */
#define DM_MAINCODE_BLOCK_ID         0x86
/*!
  Maincode block
  */
#define MAINCODE_BLOCK_ID            0x88
/*!
  OTA_0 block
  */
#define OTA_BLOCK_ID                 0x87
/*!
  Fast logo block
  */
#define FAST_LOGO_BLOCK_ID            0x89
/*!
  USB tool block
  */
#define USB_TOOL_BLOCK_ID            0x8A
/*!
  Bootloader resource block ID
  */
#define BL_RS_BLOCK_ID               0x8B
/*!
  Resource block
  */
#define RS_BLOCK_ID                  0x8C
/*!
  OTA_1 block
  */
#define OTA1_BLOCK_ID            0x8D
/*!
  backup ota loader
  */
#define BACKUPLOADER_BLOCK_ID            0x8E
/*!
  identity block
  */
#define IDENTITY_BLOCK_ID                  0x8F

/*!
  Calendar block
  */
#define CALENDAR_BLOCK_ID           0x90
/*!
  logo and preset in dm block
  */
#define DM_LOGO_BLOCK_ID    0x91
/*!
  logo and preset in dm block
  */
#define ADS_DATA_BLOCK_ID    0x92
/*!
  Start up LOGO block
  */
#define START_LOGO_BLOCK_ID          0x93
/*!
  LOGO 1 block
  */
#define LOGO_BLOCK_ID_M0             0x94
/*!
  LOGO 2 block
  */
#define LOGO_BLOCK_ID_M1             0x95
/*!
  LOGO 3 block
  */
#define LOGO_BLOCK_ID_M2             0x96
/*!
  LOGO 4 block
  */
#define LOGO_BLOCK_ID_M3             0x97
/*!
  System status data block
  */
#define SS_DATA_BLOCK_ID             0x98
/*!
  board config block
  */
#define BOARD_CONFIG_BLOCK_ID             0x99
/*!
  Preset block block
  */
#define PRESET_BLOCK_ID              0x9C
/*!
  Upgrade info block
  */
#define UPGRADE_BLOCK_ID             0xA0
/*!
  static save date block
  */
#define STATIC_SAVE_DATA_BLOCK_ID 0xA2
/*!
  static save date backup block
  */
#define STATIC_SAVE_DATA_RESERVE_BLOCK_ID 0xA3
/*!
  IW table block
  */
#define IW_TABLE_BLOCK_ID            0xA4
/*!
  IW view block
  */
#define IW_VIEW_BLOCK_ID             0xA8
/*!
  CA Data block
  */
#define CADATA_BLOCK_ID              0xAD
/*!
  IR key block
  */
#define IRKEY_BLOCK_ID               0xAE
/*!
  IR key 1 block
  */
#define IRKEY1_BLOCK_ID              0xAF
/*!
  IR key 2 block
  */
#define IRKEY2_BLOCK_ID              0xB1
/*!
  FP key block
  */
#define FPKEY_BLOCK_ID               0xB0
/*!
  FP config block
  */
#define FPCFG_BLOCK_ID               0xB2
/*!
  Misc option in bootloader block
  */
#define MISC_OPTION_BLOCK_ID         0xB4
/*!
  UPG tool block
  */
#define UPG_TOOL_BLOCK_ID            0xB8
/*!
  ABS tdi block
  */
#define ABS_TDI_BLOCK_ID             0xB9
/*!
  ABS flash header block
  */
#define ABS_FLASH_HEADER_BLOCK_ID    0xBA
/*!
  SDRAM config block
  */
#define SDRAM_CFG_BLOCK_ID           0xBB
/*!
  Hardware config block id
  */
#define HW_CFG_BLOCK_ID             0xBC
/*!
  SN client UPG
  */
#define OTA_BLOCK_SNUPG              0xBF
/*!
  flash code block
  */
#define FLASH_CODE_BLOCK_ID           0xCC

/*!
  system software and hardware info block
  */
#define SYS_INFO_BLOCK_ID          0xC0

/*!
  direct read and write ads block id
  */
#define DEC_RW_ADS_BLOCK_ID         0xC4

/*!
  wstfont2 bits small vsb block id
  */
#define SMALL_VSB_BLOCK_ID          0xC7

/*!
  wstfont2 bits ntsc vsb block id
  */
#define NTSC_VSB_BLOCK_ID           0xC8

/*!
  wstfont2 bits pal vsb block id
  */
#define PAL_VSB_BLOCK_ID            0xC9

/*!
  RSC color_0 block id
  */
#define RS_COLOR0_BLOCK_ID            0xD0

/*!
  RSC color_1 block id
  */
#define RS_COLOR1_BLOCK_ID            0xD1

/*!
  BOOTLOADER block
  */
#define U_BOOT_BLOCK_ID          0xFF



typedef struct tag_block_hdr_info_t
{
/*!
  dm data :block id
  */
  u8 id;
/*!
  dm data :block type
  */
  u8 type; //refer to enum tag_block_type
/*!
  dm data :block node num
  */
  u16 node_num;
/*!
  dm data :block base addr
  */
  u32 base_addr;
/*!
  dm data :block size
  */
  u32 size;
/*!
  dm data :crc
  */
  u32 crc;
/*!
  dm data :version
  */
  u8 version[8];
/*!
  dm data :name
  */
  u8 name[8];
/*!
  dm data :time
  */
  u8 time[12];
/*!
  dm data :reserve
  */
  u32 reserve;
}block_hdr_info_t;

/*!
 ota Block information of data manager header
 */
typedef struct tag_dmh_table
{
  /*!
    Base address of current block
    */
  u32 dmh_start_pos;
  /*!
    sdram size
    */
  u32 sdram_size;
  /*!
    flash size
    */
  u32 flash_size;
  /*!
    block number
    */
  u16 block_num;
  /*!
    block header size, use to check
    */
  u16 bh_size;
  /*!
    dm offset
    */
  u32 dm_offset;
  /*!
    DM BLOCK INFO
    */
  block_hdr_info_t dm_block[OTA_MAX_BLOCK_NUMBER];
}dmh_table_t;

/*!
 ota Block information of data manager header
 */
typedef struct tag_dmh_info
{
  /*!
    dm head number
    */
  u8 header_num;
    /*!
    dm head group
    */
  dmh_table_t  header_group[OTA_DM_DMH_MAX_GROUP_NUMBER];
}dm_dmh_info_t;

/*!
    the all or main code break up to block
    */
typedef struct tag_ota_cut_info
{
  /*!
    cut block num
    */
  unsigned char block_num;
  /*!
    cut block id
    */
  unsigned char block_id[OTA_MAX_BLOCK_NUMBER];
}tag_ota_cut_t;
/*!
    the all or main code jump  block
    */
typedef struct tag_ota_jump_info
{
  /*!
    cut block num
    */
  unsigned char block_num;
  /*!
    cut block id
    */
  unsigned char block_id[OTA_MAX_BLOCK_NUMBER];
}tag_ota_jump_t;
/*!
  defines ota filter  burn file type. 
  */
typedef enum tag_ota_file_type
{
  /*!
  unkown 
  */
  OTA_FILE_TYPE_UNKOWN = 0,
/*!
  only block 
  */
  OTA_FILE_TYPE_ONLY_BLOCK,
/*!
  main code 
  */
  OTA_FILE_TYPE_MAIN_CODE,
/*!
  all flash code 
  */
  OTA_FILE_TYPE_ALL_CODE,
  /*!
  all flash code by factory
  */
  OTA_FILE_TYPE_ALL_CODE_FACTORY,
}ota_file_type_t;

/*!
  defines ota filter  burn file type. 
  */
typedef enum tag_ota_upg_mode
{
  /*!
  unkown 
  */
  OTA_UPG_MODE_ONLY_BLOCK = 0,
/*!
  only block 
  */
  OTA_UPG_MODE_BLOCK_IN_FILE,
/*!
  main code 
  */
  OTA_UPG_MODE_ALL_BLOCK_IN_FILE,
}ota_upg_mode_t;

/*!
 ota Block information of data 
 */
typedef struct tag_ddm_data_info
{
  
 /*!
    ota upg block  
   */
  unsigned char ota_block_id;/**maincode and all flash block is 0xFF***/
  /*!
    ota file type
    */
  ota_file_type_t file_type;
   /*!
    upg mode
    */
   ota_upg_mode_t upg_mode; /***work on file type is all flash or main code file type***/
/*!
    ota new version
    */
  unsigned short ota_new_ver;
  /*!
    ota block position
    */
  unsigned int block_offset;
  /*!
    CRC byte
    */
  unsigned int file_crc;
  /*!
    file size
    */
  unsigned int file_size;
}ddm_block_info_t;

typedef struct tag_ddm_info
{

  /*!
    the header mark,0x5aa55aa5
    */
  u32 mark;
  /*!
    the head info size
    */
  u32 header_info_size;
  /*!
    the head info size
    */
  u32 header_info_crc;
  /*!
    the head reserve
    */
  u32 reserve1;
  /*!
    the user data clear flag,TRUE:clease
    */
  BOOL g_user_db_data_clear;
/*!
    the user data clear flag,TRUE:clease
    */
  BOOL g_user_sys_data_clear;
/*!
    the user data clear flag,TRUE:clease
    */
  BOOL g_upg_by_stbid;
/*!
		the ca data clear flag,TRUE:clease
		*/
  BOOL g_ca_data_clear;
/*!
		the ads data clear flag,TRUE:clease
		*/
  BOOL g_ads_data_clear;
  /*!
    the head reserve
    */
  u32 reserve4;
  /*!
    the user data clear flag,TRUE:clease
    */
  u8 stbid_start[128];
  /*!
    the user data clear flag,TRUE:clease
    */
  u8 stbid_end[128];
  

  /*!
    the block number
    */
   u8 block_number ;
  /*!
    the all or main code break up to block
    */
  tag_ota_cut_t break_block;   /***only main code type or all flash type block  work on OTA_UPG_MODE_BLOCK_IN_FILE mode**/
  /*!
    the all or main code break up to block
    */
  tag_ota_jump_t jump_block; /***main code or all flash type work on OTA_UPG_MODE_ALL_BLOCK_IN_FILE MODE**/
  /*!
    bloock info
    */
   ddm_block_info_t upg_block_info_table[OTA_MAX_BLOCK_NUMBER];
}ddm_data_head_info_t;

int check_block_exist_from_file_dm(dm_dmh_info_t *p_dmh_info,unsigned char block_id);
BOOL mul_ota_dm_api_find_dmh_info(u32 dm_offset,u8 *p_buf,u32 buf_size,dm_dmh_info_t *dmh_info);
unsigned int get_block_base_size_from_file_dm(dm_dmh_info_t *p_dmh_info,unsigned char block_id);
unsigned int get_block_base_addr_from_file_dm(dm_dmh_info_t *p_dmh_info,unsigned char block_id);
unsigned int get_block_addr_offset_from_file_dm(dm_dmh_info_t *p_dmh_info,unsigned char block_id);
#endif