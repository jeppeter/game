// HWDetectDlg.cpp : implementation file
//

#include "stdafx.h"
#include <Dbt.h>
#include <setupapi.h>
#include "HWDetect.h"
#include "HWDetectDlg.h"
#include ".\hwdetectdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
//////////////////////////////////////////////////////////////////
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnPopupDisable();
//	afx_msg void OnUpdatePopupEnable(CCmdUI *pCmdUI);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////
// CHWDetectDlg dialog
//////////////////////////////////////////////////////////////////
CHWDetectDlg::CHWDetectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHWDetectDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHWDetectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HW_TREE, m_ctrlTree);
	DDX_Control(pDX, IDC_EDIT_LOG, m_ctrlEdit);
}

BEGIN_MESSAGE_MAP(CHWDetectDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_DEVICECHANGE, OnMyDeviceChange)
	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_POPUP_DISABLE, OnPopupDisable)
END_MESSAGE_MAP()


// CHWDetectDlg message handlers

BOOL CHWDetectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_muMain.LoadMenu(IDR_POPUP_MENU);
	m_muPopup = m_muMain.GetSubMenu(0);
	hSelectedDev = NULL;

	// TODO: Add extra initialization here
    ZeroMemory(&m_imgList, sizeof(SP_CLASSIMAGELIST_DATA));
    m_imgList.cbSize = sizeof(SP_CLASSIMAGELIST_DATA);
	BOOL b = SetupDiGetClassImageList(&m_imgList);
	ASSERT(b);
	int nRootImg;
	b = SetupDiGetClassImageIndex(&m_imgList, &GUID_DEVCLASS_COMPUTER, &nRootImg);
	ASSERT(b);
	m_ctrlTree.SetImageList(CImageList::FromHandle(m_imgList.ImageList), TVSIL_NORMAL);
	int n = GetClassImgIndex(&GUID_DEVCLASS_COMPUTER);
	TCHAR szName[MAX_PATH];
	DWORD nSize = sizeof(szName);
	b = GetComputerName(szName, &nSize);
	ASSERT(b);
	hRoot = m_ctrlTree.InsertItem(szName, n, n);

	HDEVNOTIFY hDevNotify;
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	for(int i=0; i<sizeof(GUID_DEVINTERFACE_LIST)/sizeof(GUID); i++) {
		NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
		hDevNotify = RegisterDeviceNotification(this->GetSafeHwnd(), &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
		if( !hDevNotify ) {
			AfxMessageBox(CString("Can't register device notification: ") 
				+ _com_error(GetLastError()).ErrorMessage(), MB_ICONEXCLAMATION);
			return FALSE;
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHWDetectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHWDetectDlg::OnPaint() 
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
HCURSOR CHWDetectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CHWDetectDlg::OnMyDeviceChange(WPARAM wParam, LPARAM lParam)
{
	if ( DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam ) {
		PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
		PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
		PDEV_BROADCAST_HANDLE pDevHnd;
		PDEV_BROADCAST_OEM pDevOem;
		PDEV_BROADCAST_PORT pDevPort;
		PDEV_BROADCAST_VOLUME pDevVolume;
		switch( pHdr->dbch_devicetype ) {
			case DBT_DEVTYP_DEVICEINTERFACE:
				pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
				UpdateDevice(pDevInf, wParam);
				break;

			case DBT_DEVTYP_HANDLE:
				pDevHnd = (PDEV_BROADCAST_HANDLE)pHdr;
				break;

			case DBT_DEVTYP_OEM:
				pDevOem = (PDEV_BROADCAST_OEM)pHdr;
				break;

			case DBT_DEVTYP_PORT:
				pDevPort = (PDEV_BROADCAST_PORT)pHdr;
				break;

			case DBT_DEVTYP_VOLUME:
				pDevVolume = (PDEV_BROADCAST_VOLUME)pHdr;
				break;
		}
	}
	return 0;
}

void CHWDetectDlg::UpdateDevice(PDEV_BROADCAST_DEVICEINTERFACE pDevInf, WPARAM wParam)
{
	// pDevInf->dbcc_name: 
	// \\?\USB#Vid_04e8&Pid_503b#0002F9A9828E0F06#{a5dcbf10-6530-11d2-901f-00c04fb951ed}
	// szDevId: USB\Vid_04e8&Pid_503b\0002F9A9828E0F06
	// szClass: USB
	ASSERT(lstrlen(pDevInf->dbcc_name) > 4);
	CString szDevId = pDevInf->dbcc_name+4;
	int idx = szDevId.ReverseFind(_T('#'));
	ASSERT( -1 != idx );
	szDevId.Truncate(idx);
	szDevId.Replace(_T('#'), _T('\\'));
	szDevId.MakeUpper();

	CString szClass;
	idx = szDevId.Find(_T('\\'));
	ASSERT(-1 != idx );
	szClass = szDevId.Left(idx);
	CString szLog;
	m_ctrlEdit.GetWindowText(szLog);
	CString szTmp;
	if ( DBT_DEVICEARRIVAL == wParam ) {
		szTmp.Format(_T("Adding %s\r\n"), szDevId.GetBuffer());
		// TRACE("Adding %s\n", szDevId.GetBuffer());
	} else {
		szTmp.Format(_T("Removing %s\r\n"), szDevId.GetBuffer());
		// TRACE("Removing %s\n", szDevId.GetBuffer());
	}
	szLog.Append(szTmp);
	m_ctrlEdit.SetWindowText(szLog);

	// seems we should ignore "ROOT" type....
	if ( _T("ROOT") == szClass ) {
		return;
	}

	DWORD dwFlag = DBT_DEVICEARRIVAL != wParam ? 
		DIGCF_ALLCLASSES : (DIGCF_ALLCLASSES | DIGCF_PRESENT);
	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL,szClass,NULL,dwFlag);
	if( INVALID_HANDLE_VALUE == hDevInfo ) {
		AfxMessageBox(CString("SetupDiGetClassDevs(): ") 
			+ _com_error(GetLastError()).ErrorMessage(), MB_ICONEXCLAMATION);
		return;
	}

	SP_DEVINFO_DATA spDevInfoData;
	if ( FindDevice(hDevInfo, szDevId, spDevInfoData) ) {
		// OK, device found
		DWORD DataT ;
		TCHAR buf[MAX_PATH];
		DWORD nSize = 0;

		// get Friendly Name or Device Description
		if ( SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, 
			SPDRP_FRIENDLYNAME, &DataT, (PBYTE)buf, sizeof(buf), &nSize) ) {
		} else if ( SetupDiGetDeviceRegistryProperty(hDevInfo, &spDevInfoData, 
			SPDRP_DEVICEDESC, &DataT, (PBYTE)buf, sizeof(buf), &nSize) ) {
		} else {
			lstrcpy(buf, _T("Unknown"));
		}

		HTREEITEM hClass = GetClassItem(&(spDevInfoData.ClassGuid), wParam);
		// hClass can only be NULL for remove device and class node is not found
		if ( NULL == hClass ) return;
		// remove device
		HTREEITEM hChildItem = m_ctrlTree.GetChildItem(hClass);
		BOOL bFound = FALSE;
		while (hChildItem != NULL) {
			CString* pszData = (CString*)m_ctrlTree.GetItemData(hChildItem);
			if ( szDevId == *pszData  ) {
				bFound = TRUE;
				if ( DBT_DEVICEARRIVAL == wParam ) {
					// add device, but seems the device already exists
					break;
				} else {
					// remove device
					delete pszData;
					m_ctrlTree.DeleteItem(hChildItem);
					if ( !m_ctrlTree.ItemHasChildren(hClass) ) {
						HeapFree(GetProcessHeap(), 0, (LPVOID)m_ctrlTree.GetItemData(hClass));
						m_ctrlTree.DeleteItem(hClass);
					}
					break;
				}
			} else {
				hChildItem = m_ctrlTree.GetNextItem(hChildItem, TVGN_NEXT);
			}
		}
		if ( DBT_DEVICEARRIVAL == wParam && !bFound ) {
			// Add device
			int n = GetClassImgIndex(&(spDevInfoData.ClassGuid));
			HTREEITEM hItem = m_ctrlTree.InsertItem(buf, n, n, hClass);
			m_ctrlTree.SetItemData(hItem, (DWORD_PTR)(new CString(szDevId)));
			// set pspDevInfoData to NULL, otherwise it will be HeapFree
			m_ctrlTree.Expand(hClass, TVE_EXPAND);
		}
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);
}

BOOL CHWDetectDlg::FindDevice(HDEVINFO& hDevInfo, 
							  CString& szDevId, 
							  SP_DEVINFO_DATA& spDevInfoData)
{
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for(int i=0; SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData); i++) {
		DWORD nSize=0 ;
		TCHAR buf[MAX_PATH];

		if ( !SetupDiGetDeviceInstanceId(hDevInfo, &spDevInfoData, buf, sizeof(buf), &nSize) ) {
			TRACE(CString("SetupDiGetDeviceInstanceId(): ") 
				+ _com_error(GetLastError()).ErrorMessage());
			return FALSE;
		} 
		if ( szDevId == buf ) {
			// OK, device found
			return TRUE;
		}
	}
	return FALSE;
}

CString CHWDetectDlg::GetClassDesc(const GUID* pGuid)
{
	TCHAR buf[MAX_PATH];
	DWORD size;
	if ( SetupDiGetClassDescription(pGuid, buf, sizeof(buf), &size) ) {
		return CString(buf);
	} else {
		TRACE("Can't get class description: %s", _com_error(GetLastError()).ErrorMessage());
		return _T("");
	}
}

int CHWDetectDlg::GetClassImgIndex(const GUID* pGuid)
{
	int nIdx;
	if ( SetupDiGetClassImageIndex(&m_imgList, pGuid, &nIdx) ) {
		return nIdx;
	} else {
		TRACE("Can't get class image index: %s", _com_error(GetLastError()).ErrorMessage());
		return -1;
	}
}

HTREEITEM CHWDetectDlg::GetClassItem(const GUID* pGuid, WPARAM wParam)
{
	if ( m_ctrlTree.ItemHasChildren(hRoot) ) {
		HTREEITEM hChildItem = m_ctrlTree.GetChildItem(hRoot);
		while (hChildItem != NULL) {
			GUID* tmp = (GUID*)m_ctrlTree.GetItemData(hChildItem);
			if ( IsEqualGUID(*tmp, *pGuid) ) {
				return hChildItem;
			} else {
				hChildItem = m_ctrlTree.GetNextItem(hChildItem, TVGN_NEXT);
			}
		}
	}
	
	if ( DBT_DEVICEARRIVAL == wParam ) {
		// no child or not found
		int n = GetClassImgIndex(pGuid);
		GUID* tmp = (GUID*)HeapAlloc(GetProcessHeap(), 0, sizeof(GUID));
		CopyMemory(tmp, pGuid, sizeof(GUID));
		HTREEITEM hItem = m_ctrlTree.InsertItem(GetClassDesc(pGuid), n, n, hRoot);
		m_ctrlTree.SetItemData(hItem, (DWORD_PTR)tmp);
		m_ctrlTree.Expand(hRoot, TVE_EXPAND);
		return hItem;
	} else {
		return NULL;
	}
}

void CHWDetectDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	hSelectedDev = NULL;
	CPoint scPoint = point;
	m_ctrlTree.ScreenToClient(&scPoint);
	HTREEITEM hItem = m_ctrlTree.HitTest(scPoint);
	if ( NULL != hItem && hRoot != hItem ) {
		HTREEITEM hParent = m_ctrlTree.GetParentItem(hItem);
		if ( hParent != hRoot ) {
			m_ctrlTree.SelectItem(hItem);
			hSelectedDev = hItem;
			m_muPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, 
				point.x, point.y, this);
		}
	}
}

void CHWDetectDlg::OnPopupDisable()
{
	// TODO: Add your command handler code here
	if ( NULL != hSelectedDev ) {
		HTREEITEM hClass = m_ctrlTree.GetParentItem(hSelectedDev);
		GUID* pGuid = (GUID*)m_ctrlTree.GetItemData(hClass);
		ASSERT(pGuid);

		HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL,NULL,NULL,DIGCF_ALLCLASSES);
		if( INVALID_HANDLE_VALUE == hDevInfo ) {
			AfxMessageBox(CString("SetupDiGetClassDevs(): ") 
				+ _com_error(GetLastError()).ErrorMessage(), MB_ICONEXCLAMATION);
			return;
		}

		SP_PROPCHANGE_PARAMS spPropChangeParams;
		spPropChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
		spPropChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
		spPropChangeParams.Scope = DICS_FLAG_GLOBAL;
		spPropChangeParams.StateChange = DICS_DISABLE;
		spPropChangeParams.HwProfile = 0;

		CString* pszDevId = (CString*)m_ctrlTree.GetItemData(hSelectedDev);
		SP_DEVINFO_DATA spDevInfoData;
		if ( FindDevice(hDevInfo, *pszDevId, spDevInfoData) ) {
			if ( !SetupDiSetClassInstallParams(hDevInfo, &spDevInfoData, 
				(SP_CLASSINSTALL_HEADER*)&spPropChangeParams, sizeof(spPropChangeParams)) ) {
				AfxMessageBox(CString("SetupDiSetClassInstallParams(): ") 
					+ _com_error(GetLastError()).ErrorMessage(), MB_ICONEXCLAMATION);
			} else if ( !SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &spDevInfoData) ) {
				AfxMessageBox(CString("SetupDiClassInstaller(): ") 
					+ _com_error(GetLastError()).ErrorMessage(), MB_ICONEXCLAMATION);
			} else {
				AfxMessageBox(_T("Dislabe OK"), MB_OK);
			}
		}

		SetupDiDestroyDeviceInfoList(hDevInfo);
	}
}
