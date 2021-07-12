#pragma once

// CListCtrlEx
class CListCtrlEx : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlEx)

	enum ECListCtrlEx { WM_USER_CHANGE_LIST = WM_APP + 0x01234 };

public:
	CListCtrlEx();
	virtual ~CListCtrlEx();

protected:
	int					m_nSortedCol;
	BOOL				m_bSortAscending;
	BOOL				m_bSaveColumnState;
	CString				m_strSection;
	CString				m_strControl;
	CUIntArray			m_NumericColumns;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnItemclick(NMHDR* pNMHDR, LRESULT* pResult);

private:
	BOOL ClipSetText(CString strText);

public:
	BOOL SortIntItems(int nCol, BOOL bAscending, int low = 0, int high = -1);
	BOOL SortTextItems(int nCol, BOOL bAscending, int low = 0, int high = -1);

	const bool IsColumnNumeric(int) const;
	void SetColumnNumeric(int iCol);
	void UnsetColumnNumeric(int iCol);
	int FindNumericColumnIndex(int iCol);
	void CopyItemText(int iCol);
	void SelectAll();
	void Export();
};