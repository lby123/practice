// Configuration.cpp : implementation file
//

#include "stdafx.h"
#include "InstantAi.h"
#include "ConfigurationDlg.h"
#ifndef UNICODE
TCHAR const * WCHAR_TO_TCHAR(WCHAR const * in, TCHAR * out)
{
	WideCharToMultiByte (CP_OEMCP,NULL,in,-1,out,MAX_DEVICE_DESC_LEN,NULL,FALSE);
	return out;
}
#else
TCHAR const * WCHAR_TO_TCHAR(WCHAR const * in, TCHAR * out)
{
	return in;
}
#endif

#ifndef UNICODE
WCHAR const * TCHAR_TO_WCHAR(TCHAR const * in,  WCHAR* out)
{
   MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)in, -1, out, MAX_DEVICE_DESC_LEN);
   return out;
}
#else
WCHAR const * TCHAR_TO_WCHAR(TCHAR const * in,  WCHAR* out)
{
   return in;
}
#endif
// CConfiguration dialog

IMPLEMENT_DYNAMIC(CConfigurationDlg, CDialog)

DevConfParam CConfigurationDlg::GetParameter()
{
   DevConfParam param;
   param.deviceNumber = (int)m_comboBox_Device.GetItemData(m_comboBox_Device.GetCurSel());
   param.channelCount = m_comboBox_ChannelCount.GetCurSel() + 1;
   param.channelStart = m_comboBox_ChannelStart.GetCurSel();
   param.vrgType = (int)m_comboBox_ValueRange.GetItemData(m_comboBox_ValueRange.GetCurSel());
   return param;
}

CConfigurationDlg::CConfigurationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigurationDlg::IDD, pParent)
{
}

CConfigurationDlg::~CConfigurationDlg()
{
}

void CConfigurationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DEVICE, m_comboBox_Device);
	DDX_Control(pDX, IDC_COMBO_CHANSTART, m_comboBox_ChannelStart);
	DDX_Control(pDX, IDC_COMBO_CHANCOUNT, m_comboBox_ChannelCount);
	DDX_Control(pDX, IDC_COMBO_VALUERANGE, m_comboBox_ValueRange);
}


BEGIN_MESSAGE_MAP(CConfigurationDlg, CDialog)
   //{{AFX_MSG_MAP(CConfigurationDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE, OnCbnSelchangeComboDevice)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CConfigurationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
   InstantAiCtrl * instAiCtrl = AdxInstantAiCtrlCreate();
   ICollection<DeviceTreeNode>* sptedDevices = instAiCtrl->getSupportedDevices();

	if (sptedDevices->getCount() == 0)
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
      AfxMessageBox(_T("No device to support the currently demonstrated function!"));
		return FALSE;
	}else{
      for (int i = 0; i < sptedDevices->getCount(); ++i)
      {
         DeviceTreeNode const & node = sptedDevices->getItem(i);
         TRACE("%d, %s\n", node.DeviceNumber, node.Description);
         TCHAR des[MAX_DEVICE_DESC_LEN];
			m_comboBox_Device.AddString(WCHAR_TO_TCHAR(node.Description,des));
         m_comboBox_Device.SetItemData(i,node.DeviceNumber);
      }
      sptedDevices->Dispose();
   }
   instAiCtrl->Dispose();
	m_comboBox_Device.SetCurSel(0);
	OnCbnSelchangeComboDevice();
	return TRUE;  // return TRUE unless you set the focus to a control
}
// CConfiguration message handlers

void CConfigurationDlg::OnCbnSelchangeComboDevice()
{
		ErrorCode		errorCode;
      InstantAiCtrl* instantAiCtrl = AdxInstantAiCtrlCreate();

      // Clear the context of combo box for parameters
		m_comboBox_ChannelCount.ResetContent();
		m_comboBox_ChannelStart.ResetContent();
		m_comboBox_ValueRange.ResetContent();

      // Get current selected device from device combo box
      CString des;
      m_comboBox_Device.GetLBText(m_comboBox_Device.GetCurSel(),des);
      wchar_t w_des[MAX_DEVICE_DESC_LEN];
      DeviceInformation devInfo( TCHAR_TO_WCHAR((LPCTSTR)des,  w_des) );

      errorCode = instantAiCtrl->setSelectedDevice(devInfo);
      GetDlgItem(IDOK)->EnableWindow(TRUE);
      if (errorCode != 0){
         CString str;
         str.Format(_T("Error: the error code is 0x%x, The %s is busy or not exit in computer now.\n\
 Select other device please!"),errorCode,des);
         AfxMessageBox(str);
         GetDlgItem(IDOK)->EnableWindow(FALSE);
         return;}
      // Set channel start combo box
      int logicChannelCount = instantAiCtrl->getChannelCount();
      int32 i = 0;
      int chanCountMax = instantAiCtrl->getFeatures()->getChannelCountMax();
		for (i = 0; i < chanCountMax; i++)
		{
			CString str ;
         str.Format(_T("%d"),i);
		   m_comboBox_ChannelStart.AddString(str);
		}

      // Set channel count combo box
      int channelCount = (logicChannelCount < 16)? logicChannelCount:16;
		for (i = 1; i < channelCount + 1; i++)
		{
         CString str ;
         str.Format(_T("%d"),i);
			m_comboBox_ChannelCount.AddString(str);
		}
      
      // Set value range combo box
      ICollection<ValueRange>* ValueRanges = instantAiCtrl->getFeatures()->getValueRanges();
		WCHAR	vrgDescription[128];
      MathInterval	ranges[128];
		for (i = 0; i < ValueRanges->getCount() ; i++)
		{
			errorCode = AdxGetValueRangeInformation((ValueRanges->getItem(i)), sizeof(vrgDescription), vrgDescription, &ranges[i], NULL);
			CheckError(errorCode);
         CString str = vrgDescription;
			m_comboBox_ValueRange.AddString(str);
         m_comboBox_ValueRange.SetItemData(i,(int)ValueRanges->getItem(i));
		}

      // Set default parameters 
		m_comboBox_ChannelStart.SetCurSel(0);
		m_comboBox_ChannelCount.SetCurSel(2);
		m_comboBox_ValueRange.SetCurSel(0);
      instantAiCtrl->Dispose();

}

void CConfigurationDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
   if ( m_comboBox_Device.GetCount() == 0)
   {
      PostQuitMessage(0);
   }
	this->GetParent()->ShowWindow(SW_SHOW);
   this->GetParent()->EnableWindow(TRUE);
   this->ShowWindow(SW_HIDE);
   // Send message to configure the device selected.
   ::SendMessage(this->GetParent()->m_hWnd,WM_CUSTOME_CFG_CHGED,0,0);
	OnOK();
}

void CConfigurationDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
   CString windowText;
   this->GetParent()->GetWindowText(windowText);
   if (!windowText.Compare(_T("Instant AI")))
   {
      ::SendMessage(this->GetParent()->m_hWnd,WM_CUSTOME_CFG_CHGED,1,0);
   }
	this->ShowWindow(SW_HIDE);
	OnCancel();
}


void CConfigurationDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if ( m_comboBox_Device.GetCount()<= 0)
	{
		PostQuitMessage(0);
	}
   this->GetParent()->ShowWindow(SW_SHOW);
   this->GetParent()->EnableWindow(TRUE);
	this->ShowWindow(SW_HIDE);
	CDialog::OnClose();
}

void CConfigurationDlg::CheckError(ErrorCode errorCode)
{
   if (errorCode != Success)
   {
      CString str;
      str.Format(_T("Some errors happened, the error code is:0x%x"), errorCode);
      AfxMessageBox(str);
   }
}
void CConfigurationDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	KillTimer(0);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
}

void CConfigurationDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
    AfxMessageBox("dfgdfg");
}

