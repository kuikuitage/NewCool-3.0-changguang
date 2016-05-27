
// OTA_ToolsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys_types.h"
#include "ota_define_tools.h"
#include "fcrc.h"
#include "OTA_Tools.h"
#include "OTA_ToolsDlg.h"



typedef enum
{
  CUT_FILE_TYPE_PRESET = 0,
  CUT_FILE_TYPE_LOGO,
  CUT_FILE_TYPE_LOGO_PRESET, 
  CUT_FILE_TYPE_MAINCODE,
  CUT_FILE_TYPE_APP_ALL,
  CUT_FILE_TYPE_FLASH_ALL ,
  CUT_FILE_TYPE_OTA,
  CUT_FILE_TYPE_OTHER,
  CUT_FILE_TYPE_ONLY_BLOCK,
}cut_file_type_t;

typedef enum
{
  UPG_FILE_TYPE_PRESET = 0,
  UPG_FILE_TYPE_LOGO,
  UPG_FILE_TYPE_LOGO_PRESET,
  UPG_FILE_TYPE_MAINCODE,
  UPG_FILE_TYPE_APP_ALL,
  UPG_FILE_TYPE_FLASH_ALL ,
  UPG_FILE_TYPE_OTA,
  UPG_FILE_TYPE_OTHER,
}block_type_t;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COTA_ToolsDlg dialog





COTA_ToolsDlg::COTA_ToolsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COTA_ToolsDlg::IDD, pParent)
	, m_strOtaFile(_T(""))
	, m_strCreateFile(_T(""))
	, m_strBlockData(_T(""))
	, m_strUpgblock(_T(""))
	, m_strBlockID(_T(""))
	, m_strBlockVer(_T(""))
	, m_input_block_type(_T(""))
	, m_input_flash_all_file(_T(""))
	, m_flash_new_file(_T(""))
	, m_flash_upg_file(_T(""))
	
	, m_check_select_1(FALSE)
	, m_check_select_2(FALSE)
	, m_check_select_3(FALSE)
	, m_check_select_4(FALSE)
	, m_check_select_5(FALSE)
	, m_check_select_6(FALSE)
	, m_check_select_7(FALSE)
	, m_check_select_8(FALSE)
	, m_check_select_9(FALSE)
	, m_check_select_10(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COTA_ToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FILE, m_strOtaFile);
	DDX_Text(pDX, IDC_EDIT_CREATEFILEPATH, m_strCreateFile);
	DDX_Text(pDX, IDC_EDIT_BLOCKVER, m_strBlockVer);
	DDX_Text(pDX, IDC_EDIT2, m_input_block_type);
	DDX_Text(pDX, IDC_EDIT1, m_input_flash_all_file);
	DDX_Text(pDX, IDC_EDIT4, m_flash_new_file);
	DDX_Text(pDX, IDC_EDIT3, m_flash_upg_file);
	
	
	DDX_Check(pDX, IDC_CHECK_BL_S1, m_check_select_1);
	DDX_Check(pDX, IDC_CHECK_BL_S2, m_check_select_2);
	DDX_Check(pDX, IDC_CHECK_BL_S3, m_check_select_3);
	DDX_Check(pDX, IDC_CHECK_BL_S4, m_check_select_4);
	DDX_Check(pDX, IDC_CHECK_BL_S5, m_check_select_5);
	DDX_Check(pDX, IDC_CHECK_BL_S6, m_check_select_6);
	DDX_Check(pDX, IDC_CHECK_BL_S7, m_check_select_7);
	DDX_Check(pDX, IDC_CHECK_BL_S8, m_check_select_8);
	DDX_Check(pDX, IDC_CHECK_BL_S9, m_check_select_9);
	DDX_Check(pDX, IDC_CHECK_BL_S10, m_check_select_10);
}

BEGIN_MESSAGE_MAP(COTA_ToolsDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_IN_BR, &COTA_ToolsDlg::OnBnClickedBtnInBr)
	ON_CBN_SELCHANGE(IDC_EDIT2, &COTA_ToolsDlg::OnCbnupdatechangeCmbblocktype)
	ON_BN_CLICKED(IDC_BTN_OUT_BR, &COTA_ToolsDlg::OnBnClickedBtnOutBr)
	ON_BN_CLICKED(IDC_BTN_CREATE, &COTA_ToolsDlg::OnBnClickedBtnCreate)
	ON_BN_CLICKED(IDC_EDIT_BLOCKVER, &COTA_ToolsDlg::OnEnChangeEditBlockver)

	ON_BN_CLICKED(IDC_BTN_IN_FLASH_OLD, &COTA_ToolsDlg::OnBnClickedBtnInFlashOld)
	ON_BN_CLICKED(IDC_BUTTON2, &COTA_ToolsDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &COTA_ToolsDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BR_INPUT_UPG_FILE, &COTA_ToolsDlg::OnBnClickedBrInputUpgFile)

	
	ON_BN_CLICKED(IDC_CHECK_BL_S1, &COTA_ToolsDlg::OnBnClickedCheckBlS1)
	ON_BN_CLICKED(IDC_CHECK_BL_S2, &COTA_ToolsDlg::OnBnClickedCheckBlS2)
	ON_BN_CLICKED(IDC_CHECK_BL_S3, &COTA_ToolsDlg::OnBnClickedCheckBlS3)
	ON_BN_CLICKED(IDC_CHECK_BL_S4, &COTA_ToolsDlg::OnBnClickedCheckBlS4)
	ON_BN_CLICKED(IDC_CHECK_BL_S5, &COTA_ToolsDlg::OnBnClickedCheckBlS5)
	ON_BN_CLICKED(IDC_CHECK_BL_S6, &COTA_ToolsDlg::OnBnClickedCheckBlS6)
	ON_BN_CLICKED(IDC_CHECK_BL_S7, &COTA_ToolsDlg::OnBnClickedCheckBlS7)
	ON_BN_CLICKED(IDC_CHECK_BL_S8, &COTA_ToolsDlg::OnBnClickedCheckBlS8)
	ON_BN_CLICKED(IDC_CHECK_BL_S9, &COTA_ToolsDlg::OnBnClickedCheckBlS9)
	ON_BN_CLICKED(IDC_CHECK_BL_S10, &COTA_ToolsDlg::OnBnClickedCheckBlS10)
END_MESSAGE_MAP()


// COTA_ToolsDlg message handlers

BOOL COTA_ToolsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
#ifdef OTA_FIX_SELECT_BLOCK_UPG_ASSEM
	((CComboBox*)GetDlgItem(IDC_CMB_FILETYPE))->SetCurSel(UPG_FILE_TYPE_PRESET);
#else
    GetDlgItem(IDC_STATIC_BLOCK_FIX_TYPE)->ShowWindow(FALSE);
	GetDlgItem(IDC_CMB_FILETYPE)->ShowWindow(FALSE);
#endif

	GetDlgItem(IDC_STATIC_UPG_SL_AREA)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BL_S1)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BL_S2)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BL_S3)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BL_S4)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BL_S5)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BL_S6)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BL_S7)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BL_S8)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BL_S9)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_BL_S10)->ShowWindow(FALSE);

	GetDlgItem(IDC_STATIC_UPG_BLOCK_VERSION)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_VERSION1)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_VERSION2)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_VERSION3)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_VERSION4)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_VERSION5)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_VERSION6)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_VERSION7)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_VERSION8)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_VERSION9)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_VERSION10)->ShowWindow(FALSE);

	

	
	
	#if defined(OTA_FILE_TOOLS_FOR_TDX_SPI) || defined(OTA_FILE_TOOLS_FOR_SAITEWEI)
	GetDlgItem(IDC_STATIC_CA_DATA)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_AD_DATA)->ShowWindow(FALSE);
	GetDlgItem(IDC_COMBO_CA)->ShowWindow(FALSE);
	GetDlgItem(IDC_COMBO_AD)->ShowWindow(FALSE);
	
	((CComboBox*)GetDlgItem(IDC_CMB_KEEPDATA))->SetCurSel(1);
	((CComboBox*)GetDlgItem(IDC_CMB_KEEPSYSDATA))->SetCurSel(1);
	#else
	((CComboBox*)GetDlgItem(IDC_CMB_KEEPDATA))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_CMB_KEEPSYSDATA))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_CA))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_COMBO_AD))->SetCurSel(1);
	
	#endif
	
	
#ifndef ENABLE_EDIT_BLOCK_VERSION_BY_ONESELF
	m_strBlockVer = _T("2");
#else
   GetDlgItem(IDC_STATIC_ALL_BLOCK_VERSION)->ShowWindow(FALSE);
   GetDlgItem(IDC_EDIT_BLOCKVER)->ShowWindow(FALSE);
#endif
	
	m_strBlockID = _T("8d");
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void COTA_ToolsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR COTA_ToolsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void COTA_ToolsDlg::OnEnChangeEditBlockver()
{
   
   //UpdateData(TRUE);
   //block_version = wstr2decvalue(m_strBlockVer);
}


void COTA_ToolsDlg::OnBnClickedBtnInBr()
{
	// TODO: Add your control notification handler code here
	CString strExt;
	CString szFilter;
	FILE *pfile;
	unsigned int read_file_size = 0;
	u8 *p_read_buffer = NULL;


	szFilter = L"BIN files(*.bin)|*.bin|";
	szFilter += "all files(*.*)|*.*|";

	CFileDialog dlg(TRUE, NULL, NULL, 0, szFilter, NULL); 
	UpdateData(TRUE);
	if (dlg.DoModal() == IDOK) 
	{
		m_strOtaFile = dlg.GetPathName();
		UpdateData(FALSE);
		memset(&dmh_info,0,sizeof(dm_dmh_info_t));
		
		pfile=_wfopen(m_strOtaFile,L"r");
		if(pfile == NULL)
		{
		  return;
		}
		
		fseek(pfile, 0L, SEEK_END);
		read_file_size =  ftell(pfile);
		fseek(pfile, 0L, SEEK_SET);
		
		p_read_buffer =(u8 *)malloc(read_file_size);	
		if(p_read_buffer == NULL)
		{
		  return;
		}
		memset(p_read_buffer,0,read_file_size);
		fread(p_read_buffer , 1, read_file_size, pfile);
		fclose(pfile);
		   
		mul_ota_dm_api_find_dmh_info(0,p_read_buffer,read_file_size,&dmh_info);

		if((TRUE != check_block_exist_from_file_dm(&dmh_info,U_BOOT_BLOCK_ID))
		   && (TRUE == check_block_exist_from_file_dm(&dmh_info,DM_MAINCODE_BLOCK_ID))
		   && (TRUE == check_block_exist_from_file_dm(&dmh_info,IW_TABLE_BLOCK_ID))
	          && (TRUE == check_block_exist_from_file_dm(&dmh_info,IW_VIEW_BLOCK_ID)))
		{
		   m_input_block_type = _T("APP_ALL");
		   ota_file_check_block_ID = 0xFF;
		}
		else 
		{
			block_hdr_info_t block_info = {0};
			u16 bisize = sizeof(block_hdr_info_t);
			u8 blh_indicator[12] = "@~_~BI~_~@";
			u8 read_buffer[64] = {0}; /***block info at block end****/
			u32 read_len = 0;
			u32 bass_addr = 0;
			read_len = sizeof(u8)*64;
		  
		   memcpy(read_buffer,p_read_buffer + read_file_size - read_len,read_len);
 
		 if(0 == memcmp(read_buffer, blh_indicator,12))
		  {
		     memcpy(&block_info,read_buffer+ read_len - bisize,bisize);
			#ifdef OTA_FIX_SELECT_BLOCK_UPG_ASSEM	
		     if(block_info.id == DM_MAINCODE_BLOCK_ID) 
	     	{
		   		m_input_block_type = _T("MAINCODE");
				ota_file_check_block_ID = block_info.id;
	     	}
 			else  if(block_info.id == DM_LOGO_BLOCK_ID) 
 			{
 			   m_input_block_type= _T("LOGO");
				 ota_file_check_block_ID = block_info.id;
			}
			else  if(block_info.id == PRESET_BLOCK_ID) 
 			{
 			   m_input_block_type= _T("PRESET");
				 ota_file_check_block_ID = block_info.id;
			}
			else  if(block_info.id == OTA_BLOCK_ID) 
 			{
			    m_input_block_type= _T("OTA_BLOCK");
					ota_file_check_block_ID = block_info.id;
			}
			else  if(block_info.id == AD_VIDEO_BLOCK_ID) 
			{
				m_input_block_type= _T("ADVIDEO_BLOCK");
				ota_file_check_block_ID = block_info.id;
			}
			else
			{
				ota_file_check_block_ID = 0;
			   MessageBox(L"input file is wrong!", L"please input again!", MB_OK);
			   free(p_read_buffer);
			   return;
			}
			#else
				{
					sprintf(input_block_name,"%s",block_info.name);
					input_block_name[IDC_CHECK_BLOCK_NAME_SIZE] = 0;
					USES_CONVERSION;
					m_input_block_type = A2T(input_block_name);		
					ota_file_check_block_ID = block_info.id;
				}
			#endif
		  }
		 else
		 {
		 	ota_file_check_block_ID = 0;
			MessageBox(L"input file is wrong!", L"please input again!", MB_OK);
			free(p_read_buffer);
			return;
		}
		 
		}
		
		free(p_read_buffer);
		UpdateData(FALSE);	  
	} 
	else 
	{
		ota_file_check_block_ID = 0;
		MessageBox(L"input file is wrong!", L"please input again!", MB_OK);
	}
}

void COTA_ToolsDlg::OnCbnupdatechangeCmbblocktype()
{
   UpdateData(TRUE);
}

void COTA_ToolsDlg::OnBnClickedBtnOutBr()
{
	// TODO: Add your control notification handler code here
	CString strExt;
	CString szFilter;


	
	szFilter = L"BIN files(*.bin)|*.bin|";
	szFilter += "all files(*.*)|*.*|";
	CFileDialog dlg(FALSE, NULL, NULL, 0, szFilter, NULL); 
	UpdateData(TRUE);
	if (dlg.DoModal() == IDOK) 
	{
		m_strCreateFile = dlg.GetPathName();	
		UpdateData(FALSE);
		
	 }
	
else {
	    ;
         }
}

void COTA_ToolsDlg::OnBnClickedBtnInFlashOld()
{
	// TODO: Add your control notification handler code here

	CString szFilter;
	
	szFilter = L"BIN files(*.bin)|*.bin|";
	szFilter += "all files(*.*)|*.*|";

	CFileDialog dlg(TRUE, NULL, NULL, 0, szFilter, NULL); 
	UpdateData(TRUE);
	if (dlg.DoModal() == IDOK) 
	{
	     m_input_flash_all_file = dlg.GetPathName();
	    UpdateData(FALSE);
	} 
	else 
	{
		MessageBox(L"input file is wrong!", L"please input again!", MB_OK);
	}
}

void COTA_ToolsDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	CString szFilter;
	
	szFilter = L"BIN files(*.bin)|*.bin|";
	szFilter += "all files(*.*)|*.*|";

	CFileDialog dlg(TRUE, NULL, NULL, 0, szFilter, NULL); 
	UpdateData(TRUE);
	if (dlg.DoModal() == IDOK) 
	{
	     m_flash_new_file = dlg.GetPathName();
	     UpdateData(FALSE);
	} 
	else 
	{
	     MessageBox(L"input file is wrong!", L"please input again!", MB_OK);
	}
}

void COTA_ToolsDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	FILE *pfile;
	unsigned int read_file_size = 0;
	unsigned int read_block_size = 0;
	unsigned int block_address = 0;
	unsigned int dm_block_size = 0;
	unsigned int dm_block_address = 0;
	unsigned int offset = 0;
	unsigned int offsize = 0;
	unsigned int block_address_offset = 0;
	
	u8 *p_read_buffer = NULL;
	u8 *p_block_buffer = NULL;
	u8 *p_write_buffer = NULL;
	
	UpdateData(TRUE);
	
	 if(TRUE == m_input_flash_all_file.IsEmpty())
 	{
	  MessageBox(L"NO flash file!", L"please input !", MB_OK);
	  return;
	}
	 if(TRUE == m_flash_new_file.IsEmpty())
 	{
	  MessageBox(L"NO create flash file!", L"please input !", MB_OK);
	  return;
	}
	 if(TRUE == m_strOtaFile.IsEmpty())
 	{
	  MessageBox(L"NO block file!", L"please input !", MB_OK);
	  return;
	}
	
	memset(&dmh_info,0,sizeof(dm_dmh_info_t));
	pfile=_wfopen(m_input_flash_all_file,L"r");
	if(pfile == NULL)
	{
	  MessageBox(L"input flash file is wrong!", L"please input again!", MB_OK);
	  return;
	}
	
	fseek(pfile, 0L, SEEK_END);
	read_file_size =  ftell(pfile);
	fseek(pfile, 0L, SEEK_SET);
	
	p_read_buffer =(u8 *)malloc(read_file_size);	
	if(p_read_buffer == NULL)
	{
	  return;
	}
	memset(p_read_buffer,0,read_file_size);
	fread(p_read_buffer , 1, read_file_size, pfile);
	fclose(pfile);
	memset(&dmh_info,0,sizeof(dm_dmh_info_t));
	mul_ota_dm_api_find_dmh_info(0,p_read_buffer,read_file_size,&dmh_info);
	if(dmh_info.header_group[0].flash_size != read_file_size)
	{
	  MessageBox(L"input flash file size is wrong!", L"please input again!", MB_OK);
	  free(p_read_buffer);
	  return;
	}

	pfile=_wfopen(m_strOtaFile,L"r");
	if(pfile == NULL)
	{
	  MessageBox(L"input block file is wrong!", L"please input again!", MB_OK);
	  free(p_read_buffer);
	  return;
	}
	
	fseek(pfile, 0L, SEEK_END);
	read_block_size =  ftell(pfile);
	fseek(pfile, 0L, SEEK_SET);
	
	p_block_buffer =(u8 *)malloc(read_block_size);	
	if(p_block_buffer == NULL)
	{
	  free(p_read_buffer);
	  return;
	}
	memset(p_block_buffer,0,read_block_size);
     fread(p_block_buffer , 1, read_block_size, pfile);
     fclose(pfile);

	memset(&block_dmh_info,0,sizeof(dm_dmh_info_t));
     mul_ota_dm_api_find_dmh_info(0,p_block_buffer,read_block_size,&block_dmh_info);

	
	if((TRUE != check_block_exist_from_file_dm(&block_dmh_info,U_BOOT_BLOCK_ID))
		   &&(TRUE == check_block_exist_from_file_dm(&block_dmh_info,DM_MAINCODE_BLOCK_ID))
		   && (TRUE == check_block_exist_from_file_dm(&block_dmh_info,IW_TABLE_BLOCK_ID))
	          && (TRUE == check_block_exist_from_file_dm(&block_dmh_info,IW_VIEW_BLOCK_ID)))   /****app all*****/
		{
		    ota_file_block_ID = 0xFF;
		}
		else 
		{
			block_hdr_info_t block_info = {0};
			u16 bisize = sizeof(block_hdr_info_t);
			u8 blh_indicator[12] = "@~_~BI~_~@";
			u8 read_buffer[64] = {0}; /***block info at block end****/
			u32 read_len = 0;
			u32 bass_addr = 0;
			read_len = sizeof(u8)*64;
		  
		 memcpy(read_buffer,p_block_buffer + read_block_size - read_len,read_len);
		 if(0 == memcmp(read_buffer, blh_indicator,12))
		  {
		     memcpy(&block_info,read_buffer+ read_len - bisize,bisize);
			#ifdef OTA_FIX_SELECT_BLOCK_UPG_ASSEM		 
		     if(block_info.id == DM_MAINCODE_BLOCK_ID) 
		     	{
				   ota_file_block_ID = DM_MAINCODE_BLOCK_ID;
				   block_address = block_info.base_addr;
		     	}
 			else  if(block_info.id == DM_LOGO_BLOCK_ID) 
 			{
				   ota_file_block_ID = DM_LOGO_BLOCK_ID;
				   block_address = block_info.base_addr;
			}
			else  if(block_info.id == PRESET_BLOCK_ID) 
 			{
				   ota_file_block_ID = PRESET_BLOCK_ID;
				   block_address = block_info.base_addr;
			}
			else  if(block_info.id == OTA_BLOCK_ID) 
 			{
				    ota_file_block_ID = OTA_BLOCK_ID;
				    block_address = block_info.base_addr;
			}
			else  if(block_info.id == AD_VIDEO_BLOCK_ID) 
			{
					ota_file_block_ID = AD_VIDEO_BLOCK_ID;
					block_address = block_info.base_addr;
			}
			else
			{
			   MessageBox(L"input file is wrong!", L"please input again!", MB_OK);
			   ota_file_block_ID = 0;
			   block_address = 0;
			   free(p_read_buffer);
     	       free(p_block_buffer);
			   return;
			}
			#else
			ota_file_block_ID = block_info.id;
			block_address = block_info.base_addr;
			#endif
		  }
		 else
		 {
			MessageBox(L"input file is wrong!", L"please input again!", MB_OK);
			ota_file_block_ID = 0;
			block_address = 0;
			 free(p_read_buffer);
     	     free(p_block_buffer);
			return;
		}
	}

    if(ota_file_check_block_ID != ota_file_block_ID)
  	{
		MessageBox(L"block file is wrong!", L"please input again!", MB_OK);
		ota_file_block_ID = 0;
		block_address = 0;
		free(p_read_buffer);
		free(p_block_buffer);
		return;
	}
		

	if(ota_file_block_ID == 0xFF)
	{
		if(dmh_info.header_num > 1)
	   	{
	        dm_block_size = dmh_info.header_group[0].flash_size - dmh_info.header_group[1].dmh_start_pos;
		    dm_block_address = dmh_info.header_group[1].dmh_start_pos;
		    block_address = dmh_info.header_group[1].dmh_start_pos;
		    block_address_offset = 0;
	   	}
	   else
	   	{
			MessageBox(L"input flash file is wrong!", L"please input again!", MB_OK);
			free(p_read_buffer);
			free(p_block_buffer);
		   return;
	   	}
	}
	else
	{
	  dm_block_size = get_block_base_size_from_file_dm(&dmh_info,ota_file_block_ID);
	  dm_block_address = get_block_base_addr_from_file_dm(&dmh_info,ota_file_block_ID) ;
	  block_address_offset = get_block_addr_offset_from_file_dm(&dmh_info,ota_file_block_ID);
	  if((dm_block_size == 0) || (dm_block_address == 0))
		{
		  MessageBox(L"input flash file is wrong!", L"please input again!", MB_OK);
		  free(p_read_buffer);
     	         free(p_block_buffer);
		  return;
		}
	}
     if((dm_block_size == 0) 
	 ||(dm_block_address == 0)) 
     	{
			MessageBox(L"input flash file is wrong!", L"please input again!", MB_OK);
			free(p_read_buffer);
			free(p_block_buffer);
	  return;
     	}
     if((dm_block_size != read_block_size) 
	 ||((dm_block_address  - block_address_offset) != block_address))
	{
	  MessageBox(L"input flash file is wrong!", L"please input again!", MB_OK);
	  free(p_read_buffer);
         free(p_block_buffer);
	  return;
	}
	 if(((read_block_size % OTA_BLOCK_SIZE) != 0) 
	 ||((block_address % OTA_BLOCK_SIZE) != 0))
	{
		MessageBox(L"input flash file is wrong!", L"please input again!", MB_OK);
		free(p_read_buffer);
		free(p_block_buffer);
	  	return;
	}

       p_write_buffer =(u8 *)malloc(read_file_size);	
	if(p_write_buffer == NULL)
	{
		MessageBox(L"memory is wrong!", L"please do again!", MB_OK);
		free(p_read_buffer);
		free(p_block_buffer);
	  	return;
	}

	memset(p_write_buffer,0,read_file_size);
	
	memcpy(p_write_buffer,p_read_buffer,dm_block_address); /**header***/
	
	memcpy(p_write_buffer + dm_block_address,p_block_buffer,read_block_size);/**new block***/
	offset = dm_block_address + read_block_size;
	offsize = read_file_size - offset;
	if(offsize > 0)   
	{
         memcpy(p_write_buffer + offset,p_read_buffer + offset,offsize);     /**end block***/
	}

	pfile = _wfopen(m_flash_new_file, L"wb");
	if(pfile == NULL)
	{
		MessageBox(L"input new flash file is wrong!", L"please input again!", MB_OK);
		free(p_read_buffer);
		free(p_block_buffer);
	    return;
	}
	//write data
	fwrite(p_write_buffer, 1, read_file_size, pfile);
	fclose(pfile);
	
	free(p_write_buffer);
	free(p_read_buffer);
	free(p_block_buffer);
	MessageBox(L"Merge flash file Success!",L"", MB_OK);	
	return;
	
}

void COTA_ToolsDlg::OnBnClickedBrInputUpgFile()
{
	// TODO: Add your control notification handler code here
		// TODO: Add your control notification handler code here
	CString szFilter;
	FILE *pfile;
	unsigned int read_file_size = 0;
	u8 *p_read_buffer = NULL;
	u8 t_cnk = 0;
	int idc_serial = 0;
	int idc_version_serial = 0;
	unsigned char i = 0,j = 0;
	char block_name[IDC_CHECK_BLOCK_NAME_SIZE +1] = {0};

	szFilter = L"BIN files(*.bin)|*.bin|";
	szFilter += "all files(*.*)|*.*|";

	CFileDialog dlg(TRUE, NULL, NULL, 0, szFilter, NULL); 
	UpdateData(TRUE);
	if (dlg.DoModal() == IDOK) 
	{
	     m_flash_upg_file = dlg.GetPathName();
		#ifndef OTA_FIX_SELECT_BLOCK_UPG_ASSEM
		
		pfile=_wfopen(m_flash_upg_file,L"r");
		if(pfile == NULL)
		{
		  MessageBox(L"flash file is err!",L"please input file again", MB_OK);	
		  return;
		}
		
		fseek(pfile, 0L, SEEK_END);
		read_file_size =  ftell(pfile);
		fseek(pfile, 0L, SEEK_SET);
		
		p_read_buffer =(u8 *)malloc(read_file_size);	
		if(p_read_buffer == NULL)
		{
		  return;
		}
		memset(p_read_buffer,0,read_file_size);
		fread(p_read_buffer , 1, read_file_size, pfile);
		fclose(pfile);
		memset(&dmh_info,0,sizeof(dm_dmh_info_t));   
		mul_ota_dm_api_find_dmh_info(0,p_read_buffer,read_file_size,&dmh_info);
		memset(&check_table,0,sizeof(block_check_table_t));

		if((TRUE == check_block_exist_from_file_dm(&dmh_info,DM_MAINCODE_BLOCK_ID))
		   && (TRUE == check_block_exist_from_file_dm(&dmh_info,IW_TABLE_BLOCK_ID))
	          && (TRUE == check_block_exist_from_file_dm(&dmh_info,IW_VIEW_BLOCK_ID)))
			{
			   unsigned int boot_offset = 0;
			   unsigned int maincode_offset = 0;
			   unsigned int block_offset = 0;
			   boot_offset = get_block_addr_offset_from_file_dm(&dmh_info,U_BOOT_BLOCK_ID);
			   maincode_offset =  get_block_addr_offset_from_file_dm(&dmh_info,DM_MAINCODE_BLOCK_ID);
			   /***all flash****/
				 t_cnk = 0;
				 #ifdef ENABLE_BLOCK_UPG_SELECT_FLASH_ALL
				 if(TRUE == check_block_exist_from_file_dm(&dmh_info,U_BOOT_BLOCK_ID))
			 	{
					 idc_serial = IDC_CHECK_BL_S1 + t_cnk;
					 idc_version_serial = IDC_EDIT_VERSION1 + t_cnk;
					 /***all app****/
				     check_table.idc_info[t_cnk].block_id = 0xFF;
					 memcpy(check_table.idc_info[t_cnk].display_name,(char *)"FLASH_ALL",IDC_CHECK_BLOCK_NAME_SIZE);
					 check_table.idc_info[t_cnk].display_name[IDC_CHECK_BLOCK_NAME_SIZE] = 0;
					 check_table.idc_info[t_cnk].idc_id = idc_serial;
					 check_table.idc_info[t_cnk].idc_version_id = idc_version_serial;
					 check_table.idc_info[t_cnk].block_tpye = CHECK_BLOCK_TYPE_FLASH_ALL;
					 check_table.idc_info[t_cnk].check_group = CHECK_GROUP_NO_GROUP;
					 check_table.idc_info[t_cnk].enable = TRUE;
					  t_cnk ++;
			 	}
			    #endif	
				 idc_serial = IDC_CHECK_BL_S1 + t_cnk;
				 idc_version_serial = IDC_EDIT_VERSION1 + t_cnk;	
				 /***all app****/
			     check_table.idc_info[t_cnk].block_id = 0xFF;
				 memcpy(check_table.idc_info[t_cnk].display_name,(char *)"APP_ALL",IDC_CHECK_BLOCK_NAME_SIZE);
				 check_table.idc_info[t_cnk].display_name[IDC_CHECK_BLOCK_NAME_SIZE] = 0;
				 check_table.idc_info[t_cnk].idc_id = idc_serial;
				 check_table.idc_info[t_cnk].idc_version_id = idc_version_serial;
				 check_table.idc_info[t_cnk].block_tpye = CHECK_BLOCK_TYPE_APP_ALL;
				 check_table.idc_info[t_cnk].check_group = CHECK_GROUP_VIRTUAL_ALL;
				 check_table.idc_info[t_cnk].enable = TRUE;
				 t_cnk ++;

				for(i = 0; i< dmh_info.header_num;i ++)
				{
				   for(j = 0;j < dmh_info.header_group[i].block_num ; j ++)
				   	{
                        if((dmh_info.header_group[i].dm_block[j].type == 0)
							&& ((dmh_info.header_group[i].dm_block[j].base_addr % OTA_BLOCK_SIZE) == 0)
							&& ((dmh_info.header_group[i].dm_block[j].size % OTA_BLOCK_SIZE) == 0))
                        	{

								#ifdef OTA_FILE_TOOLS_FOR_TDX_SPI
								if((dmh_info.header_group[i].dm_block[j].id != DM_MAINCODE_BLOCK_ID)
									&& (dmh_info.header_group[i].dm_block[j].id != DM_LOGO_BLOCK_ID)
									&& (dmh_info.header_group[i].dm_block[j].id != PRESET_BLOCK_ID))
									{
									  continue;
									}
								#endif
							   #ifdef OTA_FILE_TOOLS_FOR_SAITEWEI
								if((dmh_info.header_group[i].dm_block[j].id != DM_MAINCODE_BLOCK_ID)
									&& (dmh_info.header_group[i].dm_block[j].id != START_LOGO_BLOCK_ID)
									&& (dmh_info.header_group[i].dm_block[j].id != DM_LOGO_PRESET_BISSKEY_BLOCK_ID))
								{
								  continue;
								}
							   #endif

								

								if(dmh_info.header_group[i].dm_block[j].id  == OTA1_BLOCK_ID)
								{
								   continue;
								}
								memset(block_name,0,IDC_CHECK_BLOCK_NAME_SIZE +1);
								memcpy(block_name,dmh_info.header_group[i].dm_block[j].name,IDC_CHECK_BLOCK_NAME_SIZE);
								block_name[IDC_CHECK_BLOCK_NAME_SIZE] = 0;
								idc_serial = IDC_CHECK_BL_S1 + t_cnk;
								idc_version_serial = IDC_EDIT_VERSION1 + t_cnk;	
								/***all app****/
								check_table.idc_info[t_cnk].block_id = dmh_info.header_group[i].dm_block[j].id;
                                memcpy(check_table.idc_info[t_cnk].display_name,block_name,IDC_CHECK_BLOCK_NAME_SIZE);
								check_table.idc_info[t_cnk].display_name[IDC_CHECK_BLOCK_NAME_SIZE] = 0;
								check_table.idc_info[t_cnk].idc_id = idc_serial;
								check_table.idc_info[t_cnk].idc_version_id = idc_version_serial;
								check_table.idc_info[t_cnk].block_tpye = CHECK_BLOCK_TYPE_BLOCK;
								check_table.idc_info[t_cnk].enable = TRUE;
								block_offset = get_block_addr_offset_from_file_dm(&dmh_info,
													 	  dmh_info.header_group[i].dm_block[j].id);
								if(block_offset == boot_offset)
									{
									   check_table.idc_info[t_cnk].check_group = CHECK_GROUP_BOOT_ALL;
									}
								else if(block_offset == maincode_offset)
									{
									   check_table.idc_info[t_cnk].check_group = CHECK_GROUP_APP_ALL;
									}
								else
								{
								 check_table.idc_info[t_cnk].check_group = CHECK_GROUP_NO_GROUP;
								}
                               t_cnk ++;	
                        	}
						  
				   	}
				}		 
			}

		else 
		{
			block_hdr_info_t block_info = {0};
			u16 bisize = sizeof(block_hdr_info_t);
			u8 blh_indicator[12] = "@~_~BI~_~@";
			u8 read_buffer[64] = {0}; /***block info at block end****/
			u32 read_len = 0;
			u32 bass_addr = 0;
			read_len = sizeof(u8)*64;
		  
		   memcpy(read_buffer,p_read_buffer + read_file_size - read_len,read_len);
 
		 if(0 == memcmp(read_buffer, blh_indicator,12))
		  {

		     memcpy(&block_info,read_buffer+ read_len - bisize,bisize);
             if((block_info.type == 0)
			&& ((block_info.base_addr % OTA_BLOCK_SIZE) == 0)
			&& ((read_file_size % OTA_BLOCK_SIZE) == 0))	
           	{
				 memset(block_name,0,IDC_CHECK_BLOCK_NAME_SIZE +1);
				 memcpy(block_name,block_info.name,IDC_CHECK_BLOCK_NAME_SIZE);
				 block_name[IDC_CHECK_BLOCK_NAME_SIZE] = 0;
				 t_cnk = 0;
				 idc_serial = IDC_CHECK_BL_S1 + t_cnk;
				 idc_version_serial = IDC_EDIT_VERSION1 + t_cnk;	
				 /***all app****/
			     check_table.idc_info[t_cnk].block_id = block_info.id;
				 memcpy(check_table.idc_info[t_cnk].display_name,(char *)block_info.name,IDC_CHECK_BLOCK_NAME_SIZE);
				 check_table.idc_info[t_cnk].display_name[IDC_CHECK_BLOCK_NAME_SIZE] = 0;
				 check_table.idc_info[t_cnk].idc_id = idc_serial;
				 check_table.idc_info[t_cnk].idc_version_id = idc_version_serial;
				 check_table.idc_info[t_cnk].block_tpye =  CHECK_BLOCK_TYPE_BLOCK;
				 check_table.idc_info[t_cnk].check_group = CHECK_GROUP_NO_GROUP;
				 check_table.idc_info[t_cnk].enable = TRUE;
				 t_cnk ++;
           	}
			else
			{
				MessageBox(L"input file is wrong!", L"please input again!", MB_OK);
				free(p_read_buffer);
				return;
			}
			
		  }
		 else
		 {
			MessageBox(L"input file is wrong!", L"please input again!", MB_OK);
			free(p_read_buffer);
			return;
		}
		 
		}
		check_table.block_number = t_cnk;
		free(p_read_buffer);

		if(check_table.block_number > 0)
		{
      		GetDlgItem(IDC_STATIC_UPG_SL_AREA)->ShowWindow(TRUE);
	
         	#ifdef ENABLE_EDIT_BLOCK_VERSION_BY_ONESELF	
			GetDlgItem(IDC_STATIC_UPG_BLOCK_VERSION)->ShowWindow(TRUE);
			#endif
		}
				
		for(i = 0;i < check_table.block_number;i++)
		{
		    CString name;
			USES_CONVERSION;
			name = A2T(check_table.idc_info[i].display_name);
			((CButton *)GetDlgItem(check_table.idc_info[i].idc_id))->SetWindowText(name);
		    ((CButton *)GetDlgItem(check_table.idc_info[i].idc_id))->ShowWindow(TRUE);
  	 		((CButton *)GetDlgItem(check_table.idc_info[i].idc_id))->SetCheck(FALSE);
		}
		#endif
	    UpdateData(FALSE);
		UpdateData(TRUE);
		UpdateData(FALSE);
	} 
	else 
	{
	     MessageBox(L"input file is wrong!", L"please input again!", MB_OK);
	}

}


int COTA_ToolsDlg::checkchar(char *str,int len)
{
	int str_len = 0;
	int cnk = 0;
	char *char_str = str;
	if(NULL == char_str)
	{
		return -1;
	}
	str_len = strlen(str);
	if(str_len > len)
	{
		return -1;
	}
	for(cnk = 0;cnk < str_len;cnk ++)
	{
		if(((*(char_str+cnk)) >= '0') && ((*(char_str+cnk)) <= '9'))
		{
		}
		else if(((*(char_str+cnk)) >= 'A') && ((*(char_str+cnk)) <= 'F'))
		{
		}
		else if(((*(char_str+cnk)) >= 'a') && ((*(char_str+cnk)) <= 'f'))
		{
		}
		else
		{
			return -1;
		} 
	}
	return 0;

}

int COTA_ToolsDlg::wstr2u8value(CString wcstr)
{
	u32 tmp_in = 0;
	int value = -1;
	char c_str[10];
	wcstombs(c_str,wcstr,10);

	if(checkchar(c_str,2)== 0)
	{
		sscanf(c_str, "%x", (u8*)&tmp_in);
		value = (int)static_cast<u8>(tmp_in);
	}
	else
	{
		value = -1;
	}
	return value;
}

int COTA_ToolsDlg::wstr2u16value(CString wcstr)
{
	u32 tmp_in = 0;
	int value = -1;
	char c_str[10];
	wcstombs(c_str,wcstr,10);
	if(checkchar(c_str,4)==0)
	{
		sscanf(c_str, "%x", (u16*)&tmp_in);
		value = (int)static_cast<u16>(tmp_in);
	}
	else
	{
		value = -1;
	}
	return value;
}

int COTA_ToolsDlg::wstr2u32value(CString wcstr)
{
	u32 tmp_in = 0;
	int value = -1;
	char c_str[10];
	wcstombs(c_str,wcstr,10);
	if(checkchar(c_str,8)==0)
	{
		sscanf(c_str, "%x", (u32*)&tmp_in);
		value = (int)static_cast<u32>(tmp_in);
	}
	else
	{
		value = -1;
	}
	return value;
}

int COTA_ToolsDlg::wstr2decvalue(CString wcstr)
{
	u32 tmp_in = 0;
	int value = -1;
	char c_str[10];
       wcstombs(c_str,wcstr,10);
	if(checkNumberchar(wcstr)== 0)
	{
		sscanf(c_str, "%d", (u8*)&tmp_in);
		value = (int)static_cast<u8>(tmp_in);
	}
	else
	{
		value = -1;
	}
	return value;
}

int COTA_ToolsDlg::checkNumberchar(CString number_str)
{
	int str_len = 0;
	int cnk = 0;
	char char_str[100];

    memset(char_str,0,sizeof(char_str));
	wcstombs((char *)char_str,number_str.GetBuffer(),number_str.GetLength());
	str_len = strlen(char_str);
	for(cnk = 0;cnk < str_len;cnk ++)
	{
		if(((*(char_str+cnk)) >= '0') && ((*(char_str+cnk)) <= '9'))
		{
		}
		else
		{
			return -1;
		} 
	}
	return 0;
}



void COTA_ToolsDlg:: create_ota_bin(void)
{
	FILE *pfile;
	unsigned char up_block_num = 0;
	size_t r_len = 0;
	unsigned int ddm_head_size = 0;
	unsigned int read_filesize = 0;
	unsigned int total_block_size = 0;
	char *p_write_buffer = NULL;
	u8 *p_read_buffer = NULL;
	ddm_data_head_info_t ddm_data_head;
	unsigned int writesize = 0;
	unsigned int crc_result = 0;
	u32 header_offset = 0;
	char *p_crc_buffer =  NULL;
	int iFileType = 0;
	int cut_file_type = 0;
	u8 cnk = 0;
    unsigned char block_id = 0;
	

       UpdateData(TRUE);
	//read data
	pfile = _wfopen(m_flash_upg_file, L"rb");

	if(pfile == NULL)
	{
		return;
	}
	
	 fseek(pfile, 0L, SEEK_END);
        read_filesize =  ftell(pfile);
        fseek(pfile, 0L, SEEK_SET);

	p_read_buffer =(u8 *)malloc(read_filesize);	
	if(p_read_buffer == NULL)
	{
	  return;
	}

	memset(p_read_buffer,0,read_filesize);
	r_len = fread(p_read_buffer, 1, read_filesize, pfile);
	fclose(pfile);

	for(unsigned char i = 0;i < OTA_MAX_BLOCK_NUMBER;i ++)
	{
	   cut_block_info[i].cut_block_id = 0;
	   cut_block_info[i].cut_block_name = _T("");
	   cut_block_info[i].dm_block_size = 0; 
	   cut_block_info[i].dm_block_address = 0;
	   cut_block_info[i].block_version = 0;
	}
	 
	memset(&dmh_info,0,sizeof(dm_dmh_info_t));
	mul_ota_dm_api_find_dmh_info(0,p_read_buffer,read_filesize,&dmh_info);

      	if((TRUE == check_block_exist_from_file_dm(&dmh_info,U_BOOT_BLOCK_ID))
	   && (TRUE == check_block_exist_from_file_dm(&dmh_info,DM_MAINCODE_BLOCK_ID)
	   && (TRUE == check_block_exist_from_file_dm(&dmh_info,IW_TABLE_BLOCK_ID))
	   && (TRUE == check_block_exist_from_file_dm(&dmh_info,IW_VIEW_BLOCK_ID))))
   	{
	   cut_file_type = CUT_FILE_TYPE_FLASH_ALL;
   	}
	else if((TRUE != check_block_exist_from_file_dm(&dmh_info,U_BOOT_BLOCK_ID))
	   && (TRUE == check_block_exist_from_file_dm(&dmh_info,DM_MAINCODE_BLOCK_ID)
	   && (TRUE == check_block_exist_from_file_dm(&dmh_info,IW_TABLE_BLOCK_ID))
	   && (TRUE == check_block_exist_from_file_dm(&dmh_info,IW_VIEW_BLOCK_ID))))
	{
	   cut_file_type = CUT_FILE_TYPE_APP_ALL;
	}
	else
	{
			block_hdr_info_t block_info = {0};
			u16 bisize = sizeof(block_hdr_info_t);
			u8 blh_indicator[12] = "@~_~BI~_~@";
			u8 read_buffer[64] = {0}; /***block info at block end****/
			u32 read_len = 0;
			u32 bass_addr = 0;
			read_len = sizeof(u8)*64;
		  
		   memcpy(read_buffer,p_read_buffer + read_filesize - read_len,read_len);
 
		 if(0 == memcmp(read_buffer, blh_indicator,12))
		  {
		     memcpy(&block_info,read_buffer+ read_len - bisize,bisize);

#ifdef OTA_FIX_SELECT_BLOCK_UPG_ASSEM				 
		     if(block_info.id == DM_MAINCODE_BLOCK_ID) 
		     	{
			   cut_file_type = CUT_FILE_TYPE_MAINCODE;;
		     	}
 			else  if(block_info.id == DM_LOGO_BLOCK_ID) 
 			{
			   cut_file_type = CUT_FILE_TYPE_LOGO;
			}
			else  if(block_info.id == PRESET_BLOCK_ID) 
 			{
			   cut_file_type = CUT_FILE_TYPE_PRESET;
			}
			else  if(block_info.id == OTA_BLOCK_ID) 
 			{
			    cut_file_type = CUT_FILE_TYPE_OTA;
			}
			else
			{
			     cut_file_type = CUT_FILE_TYPE_OTHER;
			}
#else  
			cut_file_type = CUT_FILE_TYPE_ONLY_BLOCK;
#endif
			
		  }
		 else
		 {
	   	   MessageBox(L"input flash file is wrong!", L"please input again!", MB_OK);
		    free(p_read_buffer);
		   return;
		}
	}

#ifdef OTA_FILE_TOOLS_FOR_TDX_SPI
	if(cut_file_type != CUT_FILE_TYPE_FLASH_ALL)
	{
   	    MessageBox(L"input flash file is wrong!", L"please input again!", MB_OK);
	    free(p_read_buffer);
	   return;
	}
#endif
	
    cut_block_number = 0;
   
	   
	block_version = wstr2decvalue(m_strBlockVer);
	data_db_keep = ((CComboBox*)GetDlgItem(IDC_CMB_KEEPDATA))->GetCurSel();
	data_sys_keep = ((CComboBox*)GetDlgItem(IDC_CMB_KEEPSYSDATA))->GetCurSel(); 

#if defined(OTA_FILE_TOOLS_FOR_TDX_SPI) || defined(OTA_FILE_TOOLS_FOR_SAITEWEI)
#else
	data_ca_keep = ((CComboBox*)GetDlgItem(IDC_COMBO_CA))->GetCurSel();
	data_ads_keep = ((CComboBox*)GetDlgItem(IDC_COMBO_AD))->GetCurSel();
#endif
	

	#ifdef OTA_FIX_SELECT_BLOCK_UPG_ASSEM
     iFileType = ((CComboBox*)GetDlgItem(IDC_CMB_FILETYPE))->GetCurSel();
	switch(iFileType)
	{

	case UPG_FILE_TYPE_APP_ALL://APP ALL
	      if(cut_file_type == CUT_FILE_TYPE_FLASH_ALL)
	   	{
	   	   cut_block_number = 1;
	       cut_block_info[0].cut_block_id = 0xFF;
		   cut_block_info[0].cut_block_name = _T("cut_check_flash_all.bin");

		   if(dmh_info.header_num > 1)
		   	{
		         cut_block_info[0].dm_block_size = dmh_info.header_group[0].flash_size - dmh_info.header_group[1].dmh_start_pos;
			    cut_block_info[0].dm_block_address = dmh_info.header_group[1].dmh_start_pos;
		   	}
		   else
		   	{
		   	   MessageBox(L"input flash file is wrong!", L"please input again!", MB_OK);
			    free(p_read_buffer);
			   return;
		   	}
	   	}
	   else if(cut_file_type == CUT_FILE_TYPE_APP_ALL)
	   	{
	   	   cut_block_number = 1;
	       cut_block_info[0].cut_block_id = 0xFF;
		   cut_block_info[0].cut_block_name = _T("cut_check_app_all.bin");

		   if(dmh_info.header_num > 1)
		   	{
		        cut_block_info[0].dm_block_size = read_filesize;
			    cut_block_info[0].dm_block_address = 0;
		   	}
		   else
		   	{
		   	   MessageBox(L"input flash file is wrong!", L"please input again!", MB_OK);
			    free(p_read_buffer);
			   return;
		   	}
	   	}
	   else
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		    free(p_read_buffer);
		   return;
	   	}
		break;
	case UPG_FILE_TYPE_MAINCODE://MAINCODE
	       if((cut_file_type == CUT_FILE_TYPE_FLASH_ALL) 
		    ||(cut_file_type == CUT_FILE_TYPE_APP_ALL))
	       {
	          cut_block_number = 1;
	         cut_block_info[0].cut_block_id = DM_MAINCODE_BLOCK_ID;
		     cut_block_info[0].cut_block_name = _T("cut_check_maincodel.bin");
		     cut_block_info[0].dm_block_size = get_block_base_size_from_file_dm(&dmh_info,cut_block_info[0].cut_block_id);
	         cut_block_info[0].dm_block_address = get_block_base_addr_from_file_dm(&dmh_info,cut_block_info[0].cut_block_id) ;
	       }
	    else if(cut_file_type == CUT_FILE_TYPE_MAINCODE)
	       {
	          cut_block_number = 1;
	         cut_block_info[0].cut_block_id = DM_MAINCODE_BLOCK_ID;
		     cut_block_info[0].cut_block_name = _T("cut_check_maincodel.bin");
		     cut_block_info[0].dm_block_size= read_filesize;
	         cut_block_info[0].dm_block_address = 0;
	       }
		else
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		    free(p_read_buffer);
		   return;
	   	}
	       break;
	case UPG_FILE_TYPE_LOGO://LOGO PRESET
		 if((cut_file_type == CUT_FILE_TYPE_FLASH_ALL) 
		    ||(cut_file_type == CUT_FILE_TYPE_APP_ALL))
	       {
	          cut_block_number = 1;
	         cut_block_info[0].cut_block_id = DM_LOGO_BLOCK_ID;
		     cut_block_info[0].cut_block_name = _T("cut_check_logo.bin");
		     cut_block_info[0].dm_block_size = get_block_base_size_from_file_dm(&dmh_info,cut_block_info[0].cut_block_id);
	         cut_block_info[0].dm_block_address = get_block_base_addr_from_file_dm(&dmh_info,cut_block_info[0].cut_block_id) ;
	       }
		else if(cut_file_type == CUT_FILE_TYPE_LOGO)
		{
	          cut_block_number = 1;
	         cut_block_info[0].cut_block_id = DM_LOGO_BLOCK_ID;
		     cut_block_info[0].cut_block_name = _T("cut_check_logo.bin");
		     cut_block_info[0].dm_block_size = read_filesize;
	         cut_block_info[0].dm_block_address = 0 ;
	       }
		else
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		    free(p_read_buffer);
		   return;
	   	}
	  break;
	case UPG_FILE_TYPE_PRESET://LOGO PRESET
		if((cut_file_type == CUT_FILE_TYPE_FLASH_ALL) 
		    ||(cut_file_type == CUT_FILE_TYPE_APP_ALL))
	       {
	            cut_block_number = 1;
	         cut_block_info[0].cut_block_id = PRESET_BLOCK_ID;
		     cut_block_info[0].cut_block_name = _T("cut_check_preset.bin");
		     cut_block_info[0].dm_block_size = get_block_base_size_from_file_dm(&dmh_info,cut_block_info[0].cut_block_id);
	         cut_block_info[0].dm_block_address = get_block_base_addr_from_file_dm(&dmh_info,cut_block_info[0].cut_block_id) ;
	       }
		else if(cut_file_type == CUT_FILE_TYPE_PRESET)
		{
	         cut_block_number = 1;
	         cut_block_info[0].cut_block_id = PRESET_BLOCK_ID;
		     cut_block_info[0].cut_block_name = _T("cut_check_preset.bin");
		     cut_block_info[0].dm_block_size = read_filesize;
	         cut_block_info[0].dm_block_address = 0;
	       }
		else
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		    free(p_read_buffer);
		   return;
	   	}
	  break;
  	case UPG_FILE_TYPE_LOGO_PRESET://LOGO PRESET
	if((cut_file_type == CUT_FILE_TYPE_FLASH_ALL) 
		    ||(cut_file_type == CUT_FILE_TYPE_APP_ALL))
       {
          cut_block_number = 2;
         cut_block_info[0].cut_block_id = DM_LOGO_BLOCK_ID;
	     cut_block_info[0].cut_block_name = _T("cut_check_logo.bin");
	     cut_block_info[0].dm_block_size = get_block_base_size_from_file_dm(&dmh_info,cut_block_info[0].cut_block_id);
         cut_block_info[0].dm_block_address = get_block_base_addr_from_file_dm(&dmh_info,cut_block_info[0].cut_block_id) ;

	      cut_block_info[1].cut_block_id = PRESET_BLOCK_ID;
	      cut_block_info[1].cut_block_name = _T("cut_check_preset.bin");
	      cut_block_info[1].dm_block_size = get_block_base_size_from_file_dm(&dmh_info,cut_block_info[1].cut_block_id);
          cut_block_info[1].m_block_address = get_block_base_addr_from_file_dm(&dmh_info,cut_block_info[1].cut_block_id) ;
			
       }
	else
   	{
   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
	    free(p_read_buffer);
	   return;
   	}
  	break;
	#if 0
	case UPG_FILE_TYPE_FLASH_ALL://FLASH ALL
	if(cut_file_type == CUT_FILE_TYPE_FLASH_ALL)
	   	{
	   	   cut_block_number = 1;
	       cut_block_info[0].cut_block_id = 0xFF;
		   cut_block_info[0].dm_block_size = read_filesize;
		   cut_block_info[0].dm_block_address = 0;
		   cut_block_info[0].cut_block_name = _T("cut_flash_all.bin");
	   	}
	   else
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		   return;
	   	}
	break;
	case UPG_FILE_TYPE_OTA:// OTA
		if(cut_file_type == CUT_FILE_TYPE_OTA)
	       {
	            cut_block_number = 1;
	            cut_block_info[0].cut_block_id = OTA_BLOCK_ID;
		        cut_block_info[0].dm_block_size = get_block_base_size_from_file_dm(&dmh_info,cut_block_info[0].cut_block_id);
	            cut_block_info[0].dm_block_address = get_block_base_addr_from_file_dm(&dmh_info,cut_block_info[0].cut_block_id) ;
				cut_block_info[0].cut_block_name = _T("cut_ota.bin");			
	       }
		else
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		    free(p_read_buffer);
		   return;
	   	}
		break;
	case UPG_FILE_TYPE_OTHER://OTHER
	
		if(cut_file_type == CUT_FILE_TYPE_OTHER)
	       {
	            cut_block_number = 1;
	            cut_block_info[0].cut_block_id = wstr2u16value(m_strBlockID);
		       cut_block_info[0].dm_block_size = get_block_base_size_from_file_dm(&dmh_info,cut_block_info[0].cut_block_id );
	            cut_block_info[0].dm_block_address = get_block_base_addr_from_file_dm(&dmh_info,cut_block_info[0].cut_block_id ) ;
	       }
		else
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		    free(p_read_buffer);
		   return;
	   	}
		break;
		#endif
	default:
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		    free(p_read_buffer);
		   return;
	   	}
		break;
	}
	#else
{
	unsigned char i  = 0;
	unsigned char cnk = 0;
	CString block_version_str;
	cut_block_number = 0;
	for( i = 0;i < check_table.block_number;i ++ )
	{
		BOOL m_check = FALSE;
		m_check = ((CButton*)GetDlgItem(check_table.idc_info[i].idc_id))->GetCheck();
		if(m_check == BST_CHECKED)
		{
		  if((check_table.idc_info[i].block_tpye == CHECK_BLOCK_TYPE_FLASH_ALL)
			&& (cut_file_type == CUT_FILE_TYPE_FLASH_ALL))
		  	{
					cut_block_info[cnk].cut_block_id = 0xFF;
					cut_block_info[cnk].cut_block_name = _T("cut_check_flash_all.bin");
					((CEdit*)GetDlgItem(check_table.idc_info[i].idc_version_id))->GetWindowText(block_version_str);
					cut_block_info[cnk].block_version = wstr2decvalue(block_version_str);
						

					if(dmh_info.header_num > 1)
					{
						cut_block_info[cnk].dm_block_size = read_filesize;
						cut_block_info[cnk].dm_block_address = 0;
						if(read_filesize != dmh_info.header_group[0].flash_size)
						{
							 MessageBox(L"input flash file is wrong!", L"please input again!", MB_OK);
							free(p_read_buffer);
							return;
						}
					}
					else
					{
						 MessageBox(L"input flash file is wrong!", L"please input again!", MB_OK);
						free(p_read_buffer);
						return;
					}
					cnk ++;
					break;
		  	}
			else if(check_table.idc_info[i].block_tpye == CHECK_BLOCK_TYPE_APP_ALL)
			{
				cut_block_info[cnk].cut_block_id = 0xFF;
				cut_block_info[cnk].cut_block_name = _T("cut_check_app_all.bin");
				((CEdit*)GetDlgItem(check_table.idc_info[i].idc_version_id))->GetWindowText(block_version_str);
				cut_block_info[cnk].block_version = wstr2decvalue(block_version_str);
						

                if((cut_file_type == CUT_FILE_TYPE_FLASH_ALL)
						&& (dmh_info.header_num > 1))
			   	{
			        cut_block_info[cnk].dm_block_size = dmh_info.header_group[0].flash_size - dmh_info.header_group[1].dmh_start_pos;
				    cut_block_info[cnk].dm_block_address = dmh_info.header_group[1].dmh_start_pos;
			   	}
				else if(cut_file_type == CUT_FILE_TYPE_APP_ALL)	
				{
					cut_block_info[cnk].dm_block_size = read_filesize;
					cut_block_info[cnk].dm_block_address = 0;
				}
				else
				{
					MessageBox(L"input flash file is wrong!", L"please input again!", MB_OK);
					free(p_read_buffer);
					return;
				}
				cnk ++;

			}
			/****block select*****/
			else if(check_table.idc_info[i].block_tpye == CHECK_BLOCK_TYPE_BLOCK)
			{
				sprintf(check_file_name,"%s","cut_check_");
				strcat(check_file_name,check_table.idc_info[i].display_name);
				strcat(check_file_name,".bin");
				USES_CONVERSION;
				
				cut_block_info[cnk].cut_block_name = A2T(check_file_name);
				block_id = check_table.idc_info[i].block_id;
				cut_block_info[cnk].cut_block_id = block_id;

				((CEdit*)GetDlgItem(check_table.idc_info[i].idc_version_id))->GetWindowText(block_version_str);
				cut_block_info[cnk].block_version = wstr2decvalue(block_version_str);
						
				if(cut_file_type == CUT_FILE_TYPE_ONLY_BLOCK) /****only block on file*****/
				{
					cut_block_info[cnk].dm_block_size = read_filesize;
					cut_block_info[cnk].dm_block_address = 0;
					cnk ++;
					break;
				}
				else /****cut from app all or flash all file****/
				{
					cut_block_info[cnk].dm_block_size = get_block_base_size_from_file_dm(&dmh_info,block_id);
					cut_block_info[cnk].dm_block_address = get_block_base_addr_from_file_dm(&dmh_info,block_id) ;
				}
				cnk ++;
			}
			else
			{
				MessageBox(L"upg block select is wrong!",L"please select upg block again!", MB_OK);
				UpdateData(FALSE);
				return;
			}
			
		}
	}

    cut_block_number = cnk;
		
	if(cut_block_number <= 0)
	{
	   MessageBox(L"no block upg!",L"please select upg block or input file again!", MB_OK);
	   UpdateData(FALSE);
	   return;
	}

}
	#endif

    total_block_size = 0;
	for(cnk = 0;cnk < cut_block_number;cnk ++)
	{
		if(cut_block_info[cnk].dm_block_size <= 0)
		   {
		   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
			   free(p_read_buffer);
			   return;
		   	}
		if((cut_block_info[cnk].cut_block_id != 0xFF) 
		    && (cut_block_info[cnk].dm_block_address <= 0) 
		    && ((cut_file_type == CUT_FILE_TYPE_FLASH_ALL) 
		           || (cut_file_type == CUT_FILE_TYPE_APP_ALL)))
	         {
		   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
			   free(p_read_buffer);
			   return;
		   	}
	        if(((cut_block_info[cnk].dm_block_size % OTA_BLOCK_SIZE) != 0) 
			|| ((cut_block_info[cnk].dm_block_address % OTA_BLOCK_SIZE) != 0))
	         {
		   	   MessageBox(L"block don't create upg file!", L"please selecte again!", MB_OK);
			   free(p_read_buffer);
			   return;
		   }
	    total_block_size += cut_block_info[cnk].dm_block_size;
	}
		

		
        ddm_head_size = sizeof(ddm_data_head_info_t);
        header_offset = ((ddm_head_size + (1024 -1))/1024) * 1024; 
		
       writesize =  header_offset + total_block_size;

	   
	p_write_buffer =(char *)malloc(writesize);	
	if(p_write_buffer == NULL)
	{
	  free(p_read_buffer);
	  return;
	}
	
	memset(p_write_buffer,0,writesize);
       memset(&ddm_data_head,0,sizeof(ddm_data_head_info_t));

	ddm_data_head.mark = UPG_DDM_HEAD_MARK;
	if(data_db_keep == 0)
	{
          ddm_data_head.g_user_db_data_clear = FALSE;
	}
	else
	{
	  ddm_data_head.g_user_db_data_clear = TRUE;
	}

	if(data_sys_keep == 0)
	{
          ddm_data_head.g_user_sys_data_clear = FALSE;
	}
	else
	{
	  ddm_data_head.g_user_sys_data_clear = TRUE;
	}

#if defined(OTA_FILE_TOOLS_FOR_TDX_SPI) || defined(OTA_FILE_TOOLS_FOR_SAITEWEI)
#else
	if(data_ca_keep == 0)
	{
			ddm_data_head.g_ca_data_clear = FALSE;
	}
	else
	{
			ddm_data_head.g_ca_data_clear = TRUE;
	}
	if(data_ads_keep == 0)
	{
			ddm_data_head.g_ads_data_clear = FALSE;
	}
	else
	{
			ddm_data_head.g_ads_data_clear = TRUE;
	}

#endif

	
			
	ddm_data_head.block_number = cut_block_number;
	
	for(cnk = 0;cnk < cut_block_number;cnk ++)
	{
		ddm_data_head.upg_block_info_table[cnk].ota_block_id = cut_block_info[cnk].cut_block_id;
		if(ddm_data_head.upg_block_info_table[cnk].ota_block_id == 0xff)
		{
			u8 jump_cnk = 0;
			if((cut_file_type == CUT_FILE_TYPE_FLASH_ALL) 
				&& (UPG_FILE_TYPE_APP_ALL == iFileType))  /***maincode*****/
			{
				ddm_data_head.upg_block_info_table[cnk].file_type = OTA_FILE_TYPE_MAIN_CODE;
			}
			else /***all flash*****/
			{
				ddm_data_head.upg_block_info_table[cnk].file_type = OTA_FILE_TYPE_ALL_CODE;
			}
			 
			ddm_data_head.upg_block_info_table[cnk].upg_mode = OTA_UPG_MODE_ALL_BLOCK_IN_FILE;

			if(ddm_data_head.g_user_db_data_clear == FALSE)
			{
			  ddm_data_head.jump_block.block_id[jump_cnk] = IW_TABLE_BLOCK_ID;
			  jump_cnk ++;
			}
			else
			{
			  ddm_data_head.g_user_db_data_clear = FALSE;
			}
			
			if(ddm_data_head.g_user_sys_data_clear == FALSE)
			{
			  ddm_data_head.jump_block.block_id[jump_cnk] = IW_VIEW_BLOCK_ID;
			  jump_cnk ++;
			}
			else
			{
			  ddm_data_head.g_user_sys_data_clear = FALSE;
			}

#if defined(OTA_FILE_TOOLS_FOR_TDX_SPI) || defined(OTA_FILE_TOOLS_FOR_SAITEWEI)
#else
          	if(ddm_data_head.g_ca_data_clear == FALSE)
			{
			  ddm_data_head.jump_block.block_id[jump_cnk] = CADATA_BLOCK_ID;
			  jump_cnk ++;
			}
			else
			{
			  ddm_data_head.g_ca_data_clear = FALSE;
			}
      
         if(ddm_data_head.g_ads_data_clear == FALSE)
			{
			  ddm_data_head.jump_block.block_id[jump_cnk] = ADS_DATA_BLOCK_ID;
			  jump_cnk ++;
			}
			else
			{
			  ddm_data_head.g_ads_data_clear = FALSE;
			}	
            
#endif
          ddm_data_head.jump_block.block_num = jump_cnk;
			
			
		}
		else
		{
		   ddm_data_head.upg_block_info_table[cnk].file_type = OTA_FILE_TYPE_ONLY_BLOCK;
		   ddm_data_head.upg_block_info_table[cnk].upg_mode = OTA_UPG_MODE_ONLY_BLOCK;
		}

		#ifdef ENABLE_EDIT_BLOCK_VERSION_BY_ONESELF
		ddm_data_head.upg_block_info_table[cnk].ota_new_ver = cut_block_info[cnk].block_version;
		#else
		ddm_data_head.upg_block_info_table[cnk].ota_new_ver = block_version;
		#endif
		
		ddm_data_head.upg_block_info_table[cnk].file_size = cut_block_info[cnk].dm_block_size;
		if(cnk == 0)
		{
			ddm_data_head.upg_block_info_table[cnk].block_offset = header_offset;
		}
		else
		{
		      ddm_data_head.upg_block_info_table[cnk].block_offset = 
			  	                                    ddm_data_head.upg_block_info_table[cnk - 1].block_offset + 
			  	                                    ddm_data_head.upg_block_info_table[cnk - 1].file_size;
		}
		
		memcpy(p_write_buffer + ddm_data_head.upg_block_info_table[cnk].block_offset ,
			       p_read_buffer + cut_block_info[cnk].dm_block_address,cut_block_info[cnk].dm_block_size);
		
		crc_result = crc_fast_calculate(CRC32_ARITHMETIC,0xFFFFFFFF,
			                                           p_write_buffer + ddm_data_head.upg_block_info_table[cnk].block_offset,
			                                           ddm_data_head.upg_block_info_table[cnk].file_size);
		
		ddm_data_head.upg_block_info_table[cnk].file_crc = crc_result;

		pfile = _wfopen(cut_block_info[cnk].cut_block_name, L"wb");
		if(NULL != pfile)
		{
		    char *p_block_upg_buffer = 0;
		    p_block_upg_buffer = p_write_buffer + ddm_data_head.upg_block_info_table[cnk].block_offset;
		    fwrite(p_block_upg_buffer, 1, ddm_data_head.upg_block_info_table[cnk].file_size, pfile);
		    fclose(pfile);
		}
	 }
	

       ddm_data_head.header_info_size = ddm_head_size;
	 p_crc_buffer = (char *)(&ddm_data_head);
	 p_crc_buffer += sizeof(u32) * 4;
	 
	ddm_data_head.header_info_crc = crc_fast_calculate(CRC32_ARITHMETIC,0xFFFFFFFF,
		                                                                           p_crc_buffer,
		                                                                           ddm_data_head.header_info_size - sizeof(u32) * 4);
#if 0
{
    FILE *fp;
    fp = fopen("dm_ifno.bin", "wb");
    if(NULL != fp)
    {
        fwrite(p_crc_buffer, 1, ddm_data_head.header_info_size - sizeof(u32) * 4, fp);
        fclose(fp);
    }
}
#endif


	memcpy(p_write_buffer,&ddm_data_head,sizeof(ddm_data_head_info_t));

	pfile = _wfopen(m_strCreateFile, L"wb");
	if(pfile == NULL)
	{
	       free(p_write_buffer);
	       free(p_read_buffer);
		return;
	}
	//write data
	fwrite(p_write_buffer, 1, writesize, pfile);
	fclose(pfile);
       free(p_write_buffer);
	free(p_read_buffer);
}


void COTA_ToolsDlg::OnBnClickedBtnCreate()
{
	// TODO: Add your control notification handler code here
	int iFileType = 0;
	FILE *pfile;
	unsigned int read_file_size = 0;
	u8 *p_read_buffer = NULL;
	int test_select = 0;

	UpdateData(TRUE);

	
	 if(TRUE == m_flash_upg_file.IsEmpty())
 	{
	  MessageBox(L"NO flash file!", L"please input !", MB_OK);
	  return;
	}
	 if(TRUE == m_strCreateFile.IsEmpty())
 	{
	  MessageBox(L"NO create file!", L"please input !", MB_OK);
	  return;
	}
	
	memset(&dmh_info,0,sizeof(dm_dmh_info_t));
	pfile=_wfopen(m_flash_upg_file,L"r");
	if(pfile == NULL)
	{
	  MessageBox(L"input flash file is wrong!", L"please input again!", MB_OK);
	  return;
	}
	
	fseek(pfile, 0L, SEEK_END);
	read_file_size =  ftell(pfile);
	fseek(pfile, 0L, SEEK_SET);
	
	p_read_buffer =(u8 *)malloc(read_file_size);	
	if(p_read_buffer == NULL)
	{
	  return;
	}
	memset(p_read_buffer,0,read_file_size);
       fread(p_read_buffer , 1, read_file_size, pfile);
       fclose(pfile);
	memset(&dmh_info,0,sizeof(dm_dmh_info_t));
	mul_ota_dm_api_find_dmh_info(0,p_read_buffer,read_file_size,&dmh_info);
	free(p_read_buffer);
	
	block_version = wstr2decvalue(m_strBlockVer);
	data_db_keep = ((CComboBox*)GetDlgItem(IDC_CMB_KEEPDATA))->GetCurSel();
	data_sys_keep = ((CComboBox*)GetDlgItem(IDC_CMB_KEEPSYSDATA))->GetCurSel();
#if defined(OTA_FILE_TOOLS_FOR_TDX_SPI) || defined(OTA_FILE_TOOLS_FOR_SAITEWEI)
#else
	data_ca_keep = ((CComboBox*)GetDlgItem(IDC_COMBO_CA))->GetCurSel();
	data_ads_keep = ((CComboBox*)GetDlgItem(IDC_COMBO_AD))->GetCurSel();
#endif
	#ifdef OTA_FIX_SELECT_BLOCK_UPG_ASSEM
	iFileType = ((CComboBox*)GetDlgItem(IDC_CMB_FILETYPE))->GetCurSel();
	switch(iFileType)
	{
	case UPG_FILE_TYPE_MAINCODE://MAINCODE
	       if(TRUE == check_block_exist_from_file_dm(&dmh_info,DM_MAINCODE_BLOCK_ID))
	       {
	            //block_id = DM_MAINCODE_BLOCK_ID;
	       }
		else
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		   return;
	   	}
	       break;
	case UPG_FILE_TYPE_LOGO://LOGO PRESET
		if(TRUE == check_block_exist_from_file_dm(&dmh_info,DM_LOGO_BLOCK_ID))
	       {
	           // block_id = DM_LOGO_BLOCK_ID;
	       }
		else
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		   return;
	   	}
	  break;
	case UPG_FILE_TYPE_PRESET://LOGO PRESET
		if(TRUE == check_block_exist_from_file_dm(&dmh_info,PRESET_BLOCK_ID))
	       {
	           // block_id = PRESET_BLOCK_ID;
	       }
		else
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		   return;
	   	}
	  break;
  	case UPG_FILE_TYPE_LOGO_PRESET://LOGO PRESET
	if((TRUE == check_block_exist_from_file_dm(&dmh_info,PRESET_BLOCK_ID))
	 &&(TRUE == check_block_exist_from_file_dm(&dmh_info,DM_LOGO_BLOCK_ID)))
       {
            //block_id = PRESET_BLOCK_ID;
       }
	else
   	{
   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
	   return;
   	}
	  break;
	 case UPG_FILE_TYPE_APP_ALL://APP ALL
	      if((TRUE == check_block_exist_from_file_dm(&dmh_info,DM_MAINCODE_BLOCK_ID))
		   &&(TRUE == check_block_exist_from_file_dm(&dmh_info,IW_TABLE_BLOCK_ID))
		   &&(TRUE == check_block_exist_from_file_dm(&dmh_info,IW_VIEW_BLOCK_ID)))
		   	{
		         // block_id = 0xFF;
		   	}
		   else
		   	{
		   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
			   return;
		   	}
		break;
#if 0
	case UPG_FILE_TYPE_OTA:// OTA
		if(TRUE == check_block_exist_from_file_dm(&dmh_info,OTA_BLOCK_ID))
	       {
	            block_id = OTA_BLOCK_ID;
	       }
		else
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		   return;
	   	}
		break;
	case UPG_FILE_TYPE_OTHER://OTHER
	       block_id = wstr2u16value(m_strBlockID);
		if(TRUE == check_block_exist_from_file_dm(&dmh_info,block_id))
	       {
	            block_id = block_id;
	       }
		else
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		   return;
	   	}
		break;
	case UPG_FILE_TYPE_FLASH_ALL://FLASH ALL
	if((TRUE == check_block_exist_from_file_dm(&dmh_info,U_BOOT_BLOCK_ID))
	   &&(TRUE == check_block_exist_from_file_dm(&dmh_info,DM_MAINCODE_BLOCK_ID)))
	   	{
	          block_id = 0xFF;
	   	}
	   else
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		   return;
	   	}
	break;
	#endif
	default:
	   	{
	   	   MessageBox(L"selecte type is wrong!", L"please selecte again!", MB_OK);
		   return;
	   	}
		break;
	}
#else
{
	unsigned char i  = 0;
	for( i = 0;i < check_table.block_number;i ++ )
	{
		BOOL m_check = FALSE;
		m_check = ((CButton*)GetDlgItem(check_table.idc_info[i].idc_id))->GetCheck();
		if(m_check == BST_CHECKED)
		{
		   break;
		}
	}
	if((check_table.block_number <= 0) || (i >=  check_table.block_number))
	{
	   MessageBox(L"no block upg!",L"please select upg block", MB_OK);
	   UpdateData(FALSE);
	   return;
	}
}
	
#endif
	

	create_ota_bin();
	MessageBox(L"Create upg file Success!",L"", MB_OK);
	UpdateData(FALSE);
}

unsigned char COTA_ToolsDlg::GetCheckTalbeSerialFormIdc(int idc_id)
{
	unsigned char i = 0;
	for(i = 0;i < check_table.block_number;i++)
	{
	  if(check_table.idc_info[i].idc_id == idc_id)
	  	{
	  	  return i;
	  	}
	}
	return 0xFF;
}

void COTA_ToolsDlg::DisplayCheckTableAble(check_block_type_t type,BOOL able)
{
    unsigned char i = 0;
  	if(CHECK_BLOCK_TYPE_FLASH_ALL == type)
  	{
  	   for(i = 0;i < check_table.block_number;i++)
  	   	{
  	   	   if(check_table.idc_info[i].check_group & CHECK_GROUP_FLASH_ALL)
  	   	   	{
  	   	   	   ((CButton *)GetDlgItem(check_table.idc_info[i].idc_id))->SetCheck(BST_UNCHECKED);
  	   	   	   ((CButton *)GetDlgItem(check_table.idc_info[i].idc_id))->ShowWindow(able);
				#ifdef ENABLE_EDIT_BLOCK_VERSION_BY_ONESELF			 
				if(able == FALSE)
				{
				   ((CEdit*)GetDlgItem(check_table.idc_info[i].idc_version_id))->SetWindowText(_T(""));
				   ((CEdit*)GetDlgItem(check_table.idc_info[i].idc_version_id))->ShowWindow(FALSE); 
				}
			   #endif
  	   	   	}
  	   	}
  	}
	else if(CHECK_BLOCK_TYPE_APP_ALL == type)
	{
		for(i = 0;i < check_table.block_number;i++)
  	   	{
  	   	   if(check_table.idc_info[i].check_group & CHECK_GROUP_APP_ALL)
  	   	   	{
  	   	   		((CButton *)GetDlgItem(check_table.idc_info[i].idc_id))->SetCheck(BST_UNCHECKED);
				((CButton *)GetDlgItem(check_table.idc_info[i].idc_id))->ShowWindow(able);
				#ifdef ENABLE_EDIT_BLOCK_VERSION_BY_ONESELF
				if(able == FALSE)
				{
					((CEdit*)GetDlgItem(check_table.idc_info[i].idc_version_id))->SetWindowText(_T(""));
					((CEdit*)GetDlgItem(check_table.idc_info[i].idc_version_id))->ShowWindow(FALSE); 
				}
				#endif
  	   	   	}
  	   	}
	}

UpdateData(TRUE);
}

void COTA_ToolsDlg::ProcesssCheckSelect(int idc_id)
{
    unsigned char serial_cnk = 0;
	BOOL m_check = FALSE;
	serial_cnk = GetCheckTalbeSerialFormIdc(idc_id);
	if(serial_cnk == 0xFF)
	{
	   MessageBox(L"the block data is wrong!",L"please input flash file", MB_OK);
	   return;
	}
	m_check = ((CButton*)GetDlgItem(idc_id))->GetCheck();
	if(m_check == BST_CHECKED)  
	{
		DisplayCheckTableAble(check_table.idc_info[serial_cnk].block_tpye,FALSE);
		#ifdef ENABLE_EDIT_BLOCK_VERSION_BY_ONESELF
		((CEdit*)GetDlgItem(check_table.idc_info[serial_cnk].idc_version_id))->SetWindowText(_T("2"));
		((CEdit*)GetDlgItem(check_table.idc_info[serial_cnk].idc_version_id))->ShowWindow(TRUE); 
		#endif
	}
	else
	{
		DisplayCheckTableAble(check_table.idc_info[serial_cnk].block_tpye,TRUE);
		#ifdef ENABLE_EDIT_BLOCK_VERSION_BY_ONESELF
		((CEdit*)GetDlgItem(check_table.idc_info[serial_cnk].idc_version_id))->SetWindowText(_T(""));
		((CEdit*)GetDlgItem(check_table.idc_info[serial_cnk].idc_version_id))->ShowWindow(FALSE); 
		#endif
	}
UpdateData(TRUE);
}
void COTA_ToolsDlg::OnBnClickedCheckBlS1()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	ProcesssCheckSelect(IDC_CHECK_BL_S1);
	UpdateData(FALSE);
}

void COTA_ToolsDlg::OnBnClickedCheckBlS2()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	ProcesssCheckSelect(IDC_CHECK_BL_S2);
	UpdateData(FALSE);
}

void COTA_ToolsDlg::OnBnClickedCheckBlS3()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	ProcesssCheckSelect(IDC_CHECK_BL_S3);
	UpdateData(FALSE);
}

void COTA_ToolsDlg::OnBnClickedCheckBlS4()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	ProcesssCheckSelect(IDC_CHECK_BL_S4);
	UpdateData(FALSE);
}

void COTA_ToolsDlg::OnBnClickedCheckBlS5()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	ProcesssCheckSelect(IDC_CHECK_BL_S5);
	UpdateData(FALSE);
}

void COTA_ToolsDlg::OnBnClickedCheckBlS6()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	ProcesssCheckSelect(IDC_CHECK_BL_S6);
	UpdateData(FALSE);
}

void COTA_ToolsDlg::OnBnClickedCheckBlS7()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	ProcesssCheckSelect(IDC_CHECK_BL_S7);
	UpdateData(FALSE);
}

void COTA_ToolsDlg::OnBnClickedCheckBlS8()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	ProcesssCheckSelect(IDC_CHECK_BL_S8);
	UpdateData(FALSE);
}

void COTA_ToolsDlg::OnBnClickedCheckBlS9()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	ProcesssCheckSelect(IDC_CHECK_BL_S9);
	UpdateData(FALSE);
}

void COTA_ToolsDlg::OnBnClickedCheckBlS10()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	ProcesssCheckSelect(IDC_CHECK_BL_S10);
	UpdateData(FALSE);
}
