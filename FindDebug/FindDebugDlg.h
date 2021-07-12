// FindDebugDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "EditEx.h"
#include "ListCtrlEx.h"
#include "StatusBarEx.h"

// CFindDebugDlg dialog
class CFindDebugDlg : public CDialogEx
{
// Construction
public:
	CFindDebugDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FINDDEBUG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG *pMsg);

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrlEx m_list;
	CEditEx m_edit_path;
	CButton m_btn_browser;
	CString m_path;
	CString m_curFileName;
	CStatusBarEx m_bar;
	int m_nTotal;
	int m_nFind;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnListRefresh();
	afx_msg void OnListCleanPDB();
	afx_msg void OnListCleanPDBRefresh();
	afx_msg void OnListDeleteTheItem();
	afx_msg void OnListOpenFileLocation();
	afx_msg void OnListSelectAll();
	afx_msg void OnListExport();

	void EnumFiles(CString Path);
	static UINT EnumFilesProc(LPVOID lparam);
	LPWSTR AnsiToUnicode(LPCSTR lpMultiByteStr);

	static void GetDebugInfo_CB(LPBYTE pImageBase, PIMAGE_DEBUG_DIRECTORY pDebugDir);
	static void GetDebugInfo_CB_CODEVIEW(LPBYTE pDebugInfo, DWORD DebugInfoSize);
	static void GetDebugInfo_CB_MISC(LPBYTE pDebugInfo, DWORD DebugInfoSize);
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	static void OnEditDragFinish(CString fileName);
};