// ListCtrlEx.cpp : implementation file
//
#include "stdafx.h"
#include "ListCtrlEx.h"

// CListCtrlEx

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)

CListCtrlEx::CListCtrlEx()
{
	this->SetExtendedStyle(this->GetExtendedStyle() | LVS_EX_DOUBLEBUFFER);
}

CListCtrlEx::~CListCtrlEx()
{
}

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	ON_WM_DROPFILES()
	ON_NOTIFY(HDN_ITEMCLICKA, 0, OnItemclick)
	ON_NOTIFY(HDN_ITEMCLICKW, 0, OnItemclick)
	//ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx message handlers

void CListCtrlEx::OnDropFiles(HDROP hDrop)
{
	TCHAR tstrFilePath[MAX_PATH] = { 0 };
	CList<CString, CString&> lstFiles;

	int  count = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

	lstFiles.RemoveAll();

	for (int i = 0; i < count; i++)
	{
		DragQueryFile(hDrop, i, tstrFilePath, MAX_PATH);
		lstFiles.AddTail(CString(tstrFilePath));
	}

	DragFinish(hDrop);

	LPARAM lParam = (LPARAM)&lstFiles;
	GetParent()->SendMessage(WM_USER_CHANGE_LIST, 0, lParam);
}

void CListCtrlEx::OnItemclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	HD_NOTIFY* phdn = (HD_NOTIFY*)pNMHDR;

	if (phdn->iButton == 0)
	{
		// User clicked on header using left mouse button
		if (phdn->iItem == m_nSortedCol)
			m_bSortAscending = !m_bSortAscending;
		else
			m_bSortAscending = TRUE;

		m_nSortedCol = phdn->iItem;

		if (IsColumnNumeric(m_nSortedCol))
			SortIntItems(m_nSortedCol, m_bSortAscending);
		else
			SortTextItems(m_nSortedCol, m_bSortAscending);
	}

	*pResult = 0;
}

// SortTextItems	- Sort the list based on column text
// Returns		- Returns true for success
// nCol			- column that contains the text to be sorted
// bAscending		- indicate sort order
// low			- row to start scanning from - default row is 0
// high			- row to end scan. -1 indicates last row
BOOL CListCtrlEx::SortTextItems(int nCol, BOOL bAscending, int low /*= 0*/, int high /*= -1*/)
{
	if (nCol >= ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount())
		return FALSE;

	if (high == -1) high = GetItemCount() - 1;

	int lo = low;
	int hi = high;
	CString midItem;

	if (hi <= lo) return FALSE;

	midItem = GetItemText((lo + hi) / 2, nCol);

	// loop through the list until indices cross
	while (lo <= hi)
	{
		// rowText will hold all column text for one row
		CStringArray rowText;

		// find the first element that is greater than or equal to
		// the partition element starting from the left Index.
		if (bAscending)
		{
			while ((lo < high) && (GetItemText(lo, nCol) < midItem))
				++lo;
		}
		else
		{
			while ((lo < high) && (GetItemText(lo, nCol) > midItem))
				++lo;
		}

		// find an element that is smaller than or equal to
		// the partition element starting from the right Index.
		if (bAscending)
		{
			while ((hi > low) && (GetItemText(hi, nCol) > midItem))
				--hi;
		}
		else
		{
			while ((hi > low) && (GetItemText(hi, nCol) < midItem))
				--hi;
		}

		// if the indexes have not crossed, swap
		// and if the items are not equal
		if (lo <= hi)
		{
			// swap only if the items are not equal
			if (GetItemText(lo, nCol) != GetItemText(hi, nCol))
			{
				// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
				int nColCount = ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
				rowText.SetSize(nColCount);
				int i;
				for (i = 0; i < nColCount; i++)
					rowText[i] = GetItemText(lo, i);
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED | LVIS_FOCUSED | LVIS_SELECTED | LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
				lvitemhi = lvitemlo;
				lvitemhi.iItem = hi;

				GetItem(&lvitemlo);
				GetItem(&lvitemhi);

				for (i = 0; i < nColCount; i++)
					SetItemText(lo, i, GetItemText(hi, i));

				lvitemhi.iItem = lo;
				SetItem(&lvitemhi);

				for (i = 0; i < nColCount; i++)
					SetItemText(hi, i, rowText[i]);

				lvitemlo.iItem = hi;
				SetItem(&lvitemlo);
			}

			++lo;
			--hi;
		}
	}

	// If the right index has not reached the left side of array
	// must now sort the left partition.
	if (low < hi)
		SortTextItems(nCol, bAscending, low, hi);

	// If the left index has not reached the right side of array
	// must now sort the right partition.
	if (lo < high)
		SortTextItems(nCol, bAscending, lo, high);

	return TRUE;
}

BOOL CListCtrlEx::SortIntItems(int nCol, BOOL bAscending, int low /*= 0*/, int high /*= -1*/)
{
	if (nCol >= ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount())
		return FALSE;

	if (high == -1) high = GetItemCount() - 1;

	int lo = low;
	int hi = high;
	UINT midItem;

	if (hi <= lo) return FALSE;

	midItem = _ttoi(GetItemText((lo + hi) / 2, nCol).GetBuffer(0));

	// loop through the list until indices cross
	while (lo <= hi)
	{
		// rowText will hold all column text for one row
		CStringArray rowText;

		// find the first element that is greater than or equal to
		// the partition element starting from the left Index.
		if (bAscending)
		{
			while ((lo < high) && (_ttoi(GetItemText(lo, nCol).GetBuffer(0)) < midItem))
				++lo;
		}
		else
		{
			while ((lo < high) && (_ttoi(GetItemText(lo, nCol).GetBuffer(0)) > midItem))
				++lo;
		}

		// find an element that is smaller than or equal to
		// the partition element starting from the right Index.
		if (bAscending)
		{
			while ((hi > low) && (_ttoi(GetItemText(hi, nCol).GetBuffer(0)) > midItem))
				--hi;
		}
		else
		{
			while ((hi > low) && (_ttoi(GetItemText(hi, nCol).GetBuffer(0)) < midItem))
				--hi;
		}

		// if the indexes have not crossed, swap
		// and if the items are not equal
		if (lo <= hi)
		{
			// swap only if the items are not equal
			if (_ttoi(GetItemText(lo, nCol).GetBuffer(0)) != _ttoi(GetItemText(hi, nCol).GetBuffer(0)))
			{
				// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
				int nColCount = ((CHeaderCtrl*)GetDlgItem(0))->GetItemCount();
				rowText.SetSize(nColCount);
				int i;
				for (i = 0; i < nColCount; i++)
					rowText[i] = GetItemText(lo, i);
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED | LVIS_FOCUSED | LVIS_SELECTED | LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;
				lvitemhi = lvitemlo;
				lvitemhi.iItem = hi;

				GetItem(&lvitemlo);
				GetItem(&lvitemhi);

				for (i = 0; i < nColCount; i++)
					SetItemText(lo, i, GetItemText(hi, i));

				lvitemhi.iItem = lo;
				SetItem(&lvitemhi);

				for (i = 0; i < nColCount; i++)
					SetItemText(hi, i, rowText[i]);

				lvitemlo.iItem = hi;
				SetItem(&lvitemlo);
			}

			++lo;
			--hi;
		}
	}

	// If the right index has not reached the left side of array
	// must now sort the left partition.
	if (low < hi)
		SortIntItems(nCol, bAscending, low, hi);

	// If the left index has not reached the right side of array
	// must now sort the right partition.
	if (lo < high)
		SortIntItems(nCol, bAscending, lo, high);

	return TRUE;
}

const bool CListCtrlEx::IsColumnNumeric(int iCol) const
{
	for (int i = 0; i < m_NumericColumns.GetSize(); i++)
	{
		if (m_NumericColumns.GetAt(i) == (UINT)iCol)
			return true;
	}
	return false;
}

void CListCtrlEx::SetColumnNumeric(int iCol)
{
	m_NumericColumns.Add(iCol);
}

void CListCtrlEx::UnsetColumnNumeric(int iCol)
{
	int iIndex = FindNumericColumnIndex(iCol);
	if (iIndex >= 0)
		m_NumericColumns.RemoveAt(iIndex);
}

int CListCtrlEx::FindNumericColumnIndex(int iCol)
{
	for (int i = 0; i < m_NumericColumns.GetSize(); i++)
	{
		if (m_NumericColumns.GetAt(i) == (UINT)iCol)
			return i;
	}
	return -1;
}

void CListCtrlEx::CopyItemText(int iCol)
{
	CString str;
	POSITION pos = this->GetFirstSelectedItemPosition();

	while (pos)
	{
		int	nItem = this->GetNextSelectedItem(pos);

		if (this->GetSelectedCount() == 1)
			str = str + this->GetItemText(nItem, iCol);
		else
			str = str + this->GetItemText(nItem, iCol) + _T("\r\n");
	}

	ClipSetText(str);
}

BOOL CListCtrlEx::ClipSetText(CString strText)
{
	if (!OpenClipboard())
		return FALSE;
	
	if (!EmptyClipboard())
		return FALSE;
	
	size_t size = sizeof(TCHAR) * (1 + strText.GetLength());
	HGLOBAL hResult = GlobalAlloc(GMEM_MOVEABLE, size);
	LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hResult);
	memcpy(lptstrCopy, strText.GetBuffer(), size);
	GlobalUnlock(hResult);

#ifndef _UNICODE
	if (::SetClipboardData(CF_TEXT, hResult) == NULL)
#else
	if (::SetClipboardData(CF_UNICODETEXT, hResult) == NULL)
#endif
	{
		GlobalFree(hResult);
		CloseClipboard();
		return FALSE;
	}

	CloseClipboard();
	return TRUE;
}

void CListCtrlEx::SelectAll()
{
	for (int i = 0; i < GetItemCount(); i++)
	{
		SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CListCtrlEx::Export()
{
	CString str;
	CFile file;
	int nHeadCount = 0;

	CFileDialog dlg(FALSE, _T("txt"), _T("*.txt"), OFN_OVERWRITEPROMPT, _T("Text files|*.txt"), this);

	if (dlg.DoModal() != IDOK)
		return;

	nHeadCount = this->GetHeaderCtrl()->GetItemCount();

	if (file.Open(dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite))
	{
		for (int i = 0; i < nHeadCount; i++)
		{
			TCHAR strColText[256] = { 0 };
			LVCOLUMN lvcol;
			lvcol.mask = LVCF_TEXT;
			lvcol.pszText = strColText;
			lvcol.cchTextMax = 256;

			this->GetColumn(i, &lvcol);

			if (i == nHeadCount - 1)
				str = str + strColText + _T("\r\n");
			else
				str = str + strColText + _T("\t");
		}

		file.Write(str, str.GetLength() * sizeof(TCHAR));
		str = _T("");

		for (int i = 0; i < this->GetItemCount(); i++)
		{
			for (int n = 0; n < nHeadCount; n++)
			{
				if (n == nHeadCount - 1)
					str = str + this->GetItemText(i, n) + _T("\r\n");
				else
					str = str + this->GetItemText(i, n) + _T("\t");
			}

			file.Write(str, str.GetLength() * sizeof(TCHAR));
			str = _T("");
		}
	}

	file.Close();
}