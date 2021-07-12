// FindDebugDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FindDebug.h"
#include "FindDebugDlg.h"
#include "afxdialogex.h"
#include "fdi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
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
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFindDebugDlg dialog

CFindDebugDlg::CFindDebugDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFindDebugDlg::IDD, pParent)
	, m_path(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_edit_path.m_bUseAll = TRUE;
	m_edit_path.lpDragFinishProc = OnEditDragFinish;
}

void CFindDebugDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_EDIT1, m_edit_path);
	DDX_Text(pDX, IDC_EDIT1, m_path);
	DDX_Control(pDX, IDC_BUTTON1, m_btn_browser);
}

BEGIN_MESSAGE_MAP(CFindDebugDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CFindDebugDlg::OnBnClickedButton1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CFindDebugDlg::OnNMRClickList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CFindDebugDlg::OnNMClickList1)
	ON_COMMAND(ID_LIST_REFRESH, OnListRefresh)
	ON_COMMAND(ID_LIST_CLEANPDB, OnListCleanPDB)
	ON_COMMAND(ID_LIST_CLEANPDBREFRESH, OnListCleanPDBRefresh)
	ON_COMMAND(ID_LIST_DELETETHEITEM, OnListDeleteTheItem)
	ON_COMMAND(ID_LIST_OPENFILELOCATION, OnListOpenFileLocation)
	ON_COMMAND(ID_LIST_SELECTALL, OnListSelectAll)
	ON_COMMAND(ID_LIST_EXPORT, OnListExport)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CFindDebugDlg message handlers

BOOL CFindDebugDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	// TODO: Add extra initialization here
	m_list.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 40);
	m_list.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 140);
	m_list.InsertColumn(2, _T("Type"), LVCFMT_LEFT, 100);
	m_list.InsertColumn(3, _T("Timestamp"), LVCFMT_LEFT, 140);
	m_list.InsertColumn(4, _T("PDB Path"), LVCFMT_LEFT, 290);
	m_list.InsertColumn(5, _T("Image Path"), LVCFMT_LEFT, 290);
	m_list.SetColumnNumeric(0);

	DWORD dwStyle = m_list.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_INFOTIP;
	dwStyle |= LVS_EX_DOUBLEBUFFER;
	m_list.SetExtendedStyle(dwStyle); 

	::GetDebugInfo_CB = (GETDEBUGINFOCALLBACK)CFindDebugDlg::GetDebugInfo_CB;

	static UINT indicators[] =
	{
		0,
		IDS_STATUSTOTAL,
		IDS_STATUSFIND
	};

	m_bar.Create(this);					
	m_bar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));
	m_bar.SetPaneInfo(0, m_bar.GetItemID(0), SBPS_STRETCH, NULL);
	m_bar.SetPaneInfo(1, m_bar.GetItemID(1), SBPS_NORMAL, 100);
	m_bar.SetPaneInfo(2, m_bar.GetItemID(2), SBPS_NORMAL, 100);

	//This is where we actually draw it on the screen
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFindDebugDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFindDebugDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFindDebugDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CFindDebugDlg::PreTranslateMessage(MSG *pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			if (::GetFocus() == m_edit_path.m_hWnd)
			{
				OnListRefresh();
				return TRUE;
			}
			return TRUE;
		case VK_ESCAPE:
			return TRUE;
		}
	}
	return   CDialog::PreTranslateMessage(pMsg);
}

void CFindDebugDlg::EnumFiles(CString path)
{
	CFindDebugDlg* pThis = (CFindDebugDlg*)AfxGetApp()->GetMainWnd();
	CFileFind file;
	BOOL nContinue;
	nContinue = file.FindFile(path + _T("*.*"));
	if (!nContinue)
		return;
	while (nContinue)
	{
		nContinue = file.FindNextFile();
		if (file.IsDots())
			continue;
		else if (file.IsDirectory())
		{
			CString subPath;
			subPath = path.Left(path.ReverseFind('\\') + 1) + file.GetFileName() + _T("\\*.*");
			EnumFiles(subPath);
		}
		else
		{
			CString fileName = path.Left(path.ReverseFind('\\') + 1) + file.GetFileName();
			m_curFileName = fileName;
			GetDebugInfo(fileName);
			pThis->m_nTotal++;
			CString str;
			str.Format(_T("Total %d"), pThis->m_nTotal);
			pThis->m_bar.SetPaneText(1, str);
		}
	}
}

UINT CFindDebugDlg::EnumFilesProc(LPVOID lparam)
{
	CFindDebugDlg* pThis = (CFindDebugDlg*)lparam;

	CString path;
	pThis->m_edit_path.GetWindowText(path);
	pThis->m_list.DeleteAllItems();
	pThis->m_nTotal = 0;
	pThis->m_nFind = 0;
	pThis->m_bar.SetPaneText(1, _T("Total "));
	pThis->m_bar.SetPaneText(2, _T("Find "));

	pThis->m_edit_path.EnableWindow(FALSE);
	pThis->m_btn_browser.EnableWindow(FALSE);
	pThis->EnumFiles(path);
	pThis->m_edit_path.EnableWindow(TRUE);
	pThis->m_btn_browser.EnableWindow(TRUE);
}

void CFindDebugDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CFolderPickerDialog fd(NULL, 0, this, 0);
	int ret = fd.DoModal();
	if (ret == IDOK)
	{
		CString path = fd.GetFolderPath();
		m_edit_path.SetWindowText(path);
		AfxBeginThread(EnumFilesProc, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	}
}

void CFindDebugDlg::GetDebugInfo_CB(LPBYTE pImageBase, PIMAGE_DEBUG_DIRECTORY pDebugDir)
{
	// Check parameters 
	if (pDebugDir == 0)
	{
		_ASSERT(0);
		return;
	}

	if (pImageBase == 0)
	{
		_ASSERT(0);
		return;
	}

	CTime c((time_t)pDebugDir->TimeDateStamp);
	CString s = c.Format(_T("%Y/%m/%d %I:%M:%S %p"));

	CFindDebugDlg* pThis = (CFindDebugDlg*)AfxGetApp()->GetMainWnd();
	int nItem = pThis->m_list.GetItemCount();
	CString str;
	str.Format(_T("%d"), nItem);
	pThis->m_list.InsertItem(nItem, str);
	pThis->m_list.SetItemText(nItem, 1, PathFindFileName(pThis->m_curFileName));
	str.Format(_T("%s"), s);
	pThis->m_list.SetItemText(nItem, 3, str);
	pThis->m_list.SetItemText(nItem, 5, pThis->m_curFileName);

	pThis->m_nFind++;
	str.Format(_T("Find %d"), pThis->m_nFind);
	pThis->m_bar.SetPaneText(2, str);

	// Display additional information for some interesting debug information types 
	LPBYTE pDebugInfo = pImageBase + pDebugDir->PointerToRawData;

	DWORD dwType = pDebugDir->Type;

	if (dwType == IMAGE_DEBUG_TYPE_CODEVIEW)
		GetDebugInfo_CB_CODEVIEW(pDebugInfo, pDebugDir->SizeOfData);
	else if (dwType == IMAGE_DEBUG_TYPE_MISC)
		GetDebugInfo_CB_MISC(pDebugInfo, pDebugDir->SizeOfData);
	else
	{
		CString str;
		str.Format(_T("Other(%d)"), pDebugDir->Type);
		int nItem = pThis->m_list.GetItemCount() - 1;
		pThis->m_list.SetItemText(nItem, 2, str);
	}
}

void CFindDebugDlg::GetDebugInfo_CB_CODEVIEW(LPBYTE pDebugInfo, DWORD DebugInfoSize)
{
	CFindDebugDlg* pThis = (CFindDebugDlg*)AfxGetApp()->GetMainWnd();

	// Check parameters 
	if ((pDebugInfo == 0) || (DebugInfoSize == 0))
		return;
	if (IsBadReadPtr(pDebugInfo, DebugInfoSize))
		return;
	if (DebugInfoSize < sizeof(DWORD)) // size of the signature 
		return;

	// Determine the format of the information and display it accordingly 
	DWORD CvSignature = *(DWORD*)pDebugInfo;

	if (CvSignature == CV_SIGNATURE_NB10)
	{
		// NB10 -> PDB 2.00 
		CV_INFO_PDB20* pCvInfo = (CV_INFO_PDB20*)pDebugInfo;

		if (IsBadReadPtr(pDebugInfo, sizeof(CV_INFO_PDB20)))
			return;
		if (IsBadStringPtrA((CHAR*)pCvInfo->PdbFileName, UINT_MAX))
			return;

		int nItem = pThis->m_list.GetItemCount() - 1;
		pThis->m_list.SetItemText(nItem, 2, _T("CodeView_NB10"));
		pThis->m_list.SetItemText(nItem, 4, pThis->AnsiToUnicode((char*)pCvInfo->PdbFileName));
	}
	else if (CvSignature == CV_SIGNATURE_RSDS)
	{
		// RSDS -> PDB 7.00 
		CV_INFO_PDB70* pCvInfo = (CV_INFO_PDB70*)pDebugInfo;

		if (IsBadReadPtr(pDebugInfo, sizeof(CV_INFO_PDB70)))
			return;
		if (IsBadStringPtrA((CHAR*)pCvInfo->PdbFileName, UINT_MAX))
			return;

		int nItem = pThis->m_list.GetItemCount() - 1;
		pThis->m_list.SetItemText(nItem, 2, _T("CodeView_RSDS"));
		pThis->m_list.SetItemText(nItem, 4, pThis->AnsiToUnicode((char*)pCvInfo->PdbFileName));
	}
	else
	{
		int nItem = pThis->m_list.GetItemCount() - 1;
		pThis->m_list.SetItemText(nItem, 2, _T("CodeView_Other"));
		pThis->m_list.SetItemText(nItem, 4, _T(""));
	}
}

void CFindDebugDlg::GetDebugInfo_CB_MISC(LPBYTE pDebugInfo, DWORD DebugInfoSize)
{
	CFindDebugDlg* pThis = (CFindDebugDlg*)AfxGetApp()->GetMainWnd();

	// Check parameters 
	if ((pDebugInfo == 0) || (DebugInfoSize == 0))
		return;
	if (IsBadReadPtr(pDebugInfo, DebugInfoSize))
		return;
	if (DebugInfoSize < sizeof(IMAGE_DEBUG_MISC))
		return;

	// Display information 
	PIMAGE_DEBUG_MISC pMiscInfo = (PIMAGE_DEBUG_MISC)pDebugInfo;
	if (pMiscInfo->DataType == IMAGE_DEBUG_MISC_EXENAME)
	{
		if (pMiscInfo->Unicode)
		{
			if (IsBadStringPtrW((WCHAR*)pMiscInfo->Data, UINT_MAX))
				return;

			int nItem = pThis->m_list.GetItemCount() - 1;
			pThis->m_list.SetItemText(nItem, 2, _T("Misc"));
			pThis->m_list.SetItemText(nItem, 4, (WCHAR*)pMiscInfo->Data);
		}
		else
		{
			if (IsBadStringPtrA((CHAR*)pMiscInfo->Data, UINT_MAX))
				return;

			int nItem = pThis->m_list.GetItemCount() - 1;
			pThis->m_list.SetItemText(nItem, 2, _T("Misc"));
			pThis->m_list.SetItemText(nItem, 4, pThis->AnsiToUnicode((CHAR*)pMiscInfo->Data));
		}
	}
}

LPWSTR CFindDebugDlg::AnsiToUnicode(LPCSTR lpMultiByteStr)
{
	int len = MultiByteToWideChar(CP_ACP, 0, lpMultiByteStr, -1, NULL, 0);
	wchar_t *pwText = new wchar_t[len + 1];
	memset(pwText, 0, (len + 1)*sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, lpMultiByteStr, -1, pwText, len);
	return pwText;
}

void CFindDebugDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	OnNMClickList1(pNMHDR, pResult);

	CMenu menu;
	menu.LoadMenu(IDR_MENU1);

	CMenu* pPopup = menu.GetSubMenu(0);

	int selected = m_list.GetNextItem(-1, LVNI_SELECTED);
	if (selected < 0) 
	{
		pPopup->EnableMenuItem(ID_LIST_CLEANPDB, MF_GRAYED);
		pPopup->EnableMenuItem(ID_LIST_CLEANPDBREFRESH, MF_GRAYED);
		pPopup->EnableMenuItem(ID_LIST_DELETETHEITEM, MF_GRAYED);
		pPopup->EnableMenuItem(ID_LIST_OPENFILELOCATION, MF_GRAYED);
	}

	if (m_list.GetItemCount() == 0)
	{
		pPopup->EnableMenuItem(ID_LIST_SELECTALL, MF_GRAYED);
	}

	RECT rect;
	GetWindowRect(&rect);
	CPoint mousepos;
	GetCursorPos(&mousepos);
	pPopup->TrackPopupMenu(NULL, mousepos.x, mousepos.y, this);

	*pResult = 0;
}

void CFindDebugDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = m_list.GetSelectionMark();
	if (nItem != -1)
	{
		m_bar.SetPaneText(0, m_list.GetItemText(nItem, 5));
	}
	*pResult = 0;
}

void CFindDebugDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect rect;
	m_bar.GetWindowRect(&rect);    
	int iWinWidth = cx; 
	int iWinHeight = cy;
	int iStatusBarHeight = rect.Height();
	m_bar.SetWindowPos(NULL, 0, iWinHeight - iStatusBarHeight, iWinWidth, iStatusBarHeight, SWP_NOZORDER);

	m_list.GetWindowRect(rect);
	ScreenToClient(rect);
	rect.right = cx - rect.left;
	rect.bottom = cy - rect.left - iStatusBarHeight;
	m_list.MoveWindow(rect);
}

void CFindDebugDlg::OnEditDragFinish(CString fileName)
{
	CFindDebugDlg* pThis = (CFindDebugDlg*)AfxGetApp()->GetMainWnd();
	pThis->OnListRefresh();
}

void CFindDebugDlg::OnListRefresh()
{
	AfxBeginThread(EnumFilesProc, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
}

void CFindDebugDlg::OnListCleanPDB()
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	while (pos)
	{
		int	nItem = m_list.GetNextSelectedItem(pos);
		CleanDebugInfo(m_list.GetItemText(nItem, 5));
	}
}

void CFindDebugDlg::OnListCleanPDBRefresh()
{
	OnListCleanPDB();
	OnListRefresh();
}

void CFindDebugDlg::OnListDeleteTheItem()
{
	int nCount = m_list.GetItemCount();
	for (int i = nCount - 1; i >= 0; i--)
		if (m_list.GetItemState(i, LVNI_ALL | LVNI_SELECTED) == LVIS_SELECTED)
			m_list.DeleteItem(i);
}

void CFindDebugDlg::OnListOpenFileLocation()
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	while (pos)
	{
		int	nItem = m_list.GetNextSelectedItem(pos);
		CString str;
		str.Format(_T("/select,%s"), m_list.GetItemText(nItem, 5));
		ShellExecute(NULL, _T("open"), _T("explorer.exe"), str, NULL, SW_NORMAL);
	}
}

void CFindDebugDlg::OnListSelectAll()
{
	m_list.SelectAll();
}

void CFindDebugDlg::OnListExport()
{
	m_list.Export();
}