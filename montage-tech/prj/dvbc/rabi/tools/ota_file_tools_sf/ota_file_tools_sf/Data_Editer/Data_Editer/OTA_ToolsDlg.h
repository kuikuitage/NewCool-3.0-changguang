
// OTA_ToolsDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "sys_types.h"
#include "ota_define_tools.h"


typedef enum tg_check_block_type
{
  CHECK_BLOCK_TYPE_BLOCK = 0x00,
  CHECK_BLOCK_TYPE_FLASH_ALL = 0x01,
  CHECK_BLOCK_TYPE_APP_ALL = 0x02,
}check_block_type_t;
typedef enum tg_check_group
{
  CHECK_GROUP_NO_GROUP = 0x00,
  CHECK_GROUP_APP_ALL = 0x01,
  CHECK_GROUP_BOOT_ALL = 0x02,
  CHECK_GROUP_VIRTUAL_ALL = 0x04,
  CHECK_GROUP_FLASH_ALL = 0x07,
}check_group_t;

typedef struct tg_idc_check_info
{
    int serial;
    int idc_id;
    int idc_version_id;
    char display_name[IDC_CHECK_BLOCK_NAME_SIZE + 1];
    unsigned char block_id;
    check_block_type_t block_tpye;
    check_group_t check_group;
    int enable;
}idc_check_info_t;
typedef struct tg_block_check_table
{
  unsigned char block_number;
  idc_check_info_t idc_info[IDC_CHECK_MAX_NUMBER];
}block_check_table_t;



typedef struct tg_cut_block_table
{
    unsigned char cut_block_id;
    CString cut_block_name;
    unsigned int dm_block_size;
	unsigned int dm_block_address;
    unsigned short block_version;
}cut_block_table_t;

// COTA_ToolsDlg dialog
class COTA_ToolsDlg : public CDialog
{
// Construction
public:
	COTA_ToolsDlg(CWnd* pParent = NULL);	// standard constructor


// Dialog Data
	enum { IDD = IDD_OTA_TOOLS_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
  HICON m_hIcon;

  // Generated message map functions
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  DECLARE_MESSAGE_MAP()
  public:
  afx_msg void OnBnClickedBtnBr();
  afx_msg void OnBnClickedBtnInBr();
  afx_msg void OnCbnSelchangeCmbMode();	
  afx_msg void OnBnClickedBtnOutBr();
  afx_msg void OnBnClickedBtnCreate();
  afx_msg void OnEnChangeEditBlockver();
  afx_msg void OnBnClickedBtnInFlashOld();
  afx_msg void OnBnClickedButton2();
  afx_msg void OnBnClickedButton1();
  afx_msg void OnCbnupdatechangeCmbblocktype();
  afx_msg void OnBnClickedBrInputUpgFile();
  afx_msg void OnBnClickedCheckAllflash();
  afx_msg void OnBnClickedCheckAllapp();

private:
  CString m_strOtaFile;
  CString m_strCreateFile;
  CString m_strBlockVer;
  CEdit m_editBlockData;
  CString m_strBlockData;
  CString m_strUpgblock;
  CString m_strBlockID;

  CString m_input_block_type;
  CString m_input_flash_all_file;
  CString m_flash_new_file;
  CString m_flash_upg_file;
  
  unsigned short block_version;
  unsigned char cut_block_number;
  cut_block_table_t cut_block_info[OTA_MAX_BLOCK_NUMBER]; 
  

  
  int data_db_keep;
  int data_sys_keep;
  int data_ca_keep;
  int data_ads_keep;
  dm_dmh_info_t dmh_info;
  dm_dmh_info_t block_dmh_info;
  unsigned char ota_file_block_ID;
  unsigned char ota_file_check_block_ID;
  char check_file_name[32];
  char input_block_name[IDC_CHECK_BLOCK_NAME_SIZE +1];
  

  
  BOOL m_check_select_1;
  BOOL m_check_select_2;
  BOOL m_check_select_3;
  BOOL m_check_select_4;
  BOOL m_check_select_5;
  BOOL m_check_select_6;
  BOOL m_check_select_7;
  BOOL m_check_select_8;
  BOOL m_check_select_9;
  BOOL m_check_select_10;
  block_check_table_t check_table;



   int  checkNumberchar(CString number_str);
   int  wstr2u32value(CString wcstr);
   int  wstr2u16value(CString wcstr);
   int  wstr2u8value(CString wcstr);
   int  checkchar(char *str,int len);
   int  wstr2decvalue(CString wcstr);
  void create_ota_bin(void);


  void ProcesssCheckSelect(int idc_id);
  void DisplayCheckTableAble(check_block_type_t type,BOOL able);
  unsigned char GetCheckTalbeSerialFormIdc(int idc_id);

public:
	afx_msg void OnBnClickedCheckBlS1();
	afx_msg void OnBnClickedCheckBlS2();
	afx_msg void OnBnClickedCheckBlS3();
	afx_msg void OnBnClickedCheckBlS4();
	afx_msg void OnBnClickedCheckBlS5();
	afx_msg void OnBnClickedCheckBlS6();
	afx_msg void OnBnClickedCheckBlS7();
	afx_msg void OnBnClickedCheckBlS8();
	afx_msg void OnBnClickedCheckBlS9();
	afx_msg void OnBnClickedCheckBlS10();
};
