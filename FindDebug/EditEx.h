#if !defined(AFX_DROPEDIT_H__1D8BBDC1_784C_11D1_8159_444553540000__INCLUDED_)
#define AFX_DROPEDIT_H__1D8BBDC1_784C_11D1_8159_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <shlobj.h>

/////////////////////////////////////////////////////////////////////////////
// CEditEx window

class CEditMask
{
public:
	virtual BOOL AddChar(UINT nChar) = 0;
};

class CEditEx : public CEdit
{
	// Construction
public:
	CEditEx();

	//DECLARE_DYNCREATE( CEditEx )

	// Copy Semantics

	// Block copy construction...
	//
	// No m_hWnd defined for object. 
	//
private:
	CEditEx(const CEditEx&);

public:
	// Allow basics to be copied...
	CEditEx& operator = (const CEditEx&);

	// Attributes
public:

	// Operations
public:
	inline void SetUseDir(bool u) { m_bUseDir = u; }
	inline bool IsUseDir() const { return m_bUseDir; }

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEditEx)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditEx();

public:
	void setCustomMask(CEditMask* editMask)
	{
		if (m_pEditMask) delete m_pEditMask;
		m_pEditMask = editMask;
	}

	COLORREF bkColor(COLORREF);
	COLORREF textColor(COLORREF);

	COLORREF bkColor()   const { return m_crBkColor; }
	COLORREF textColor() const { return m_crTextColor; }

	void setFont(const LOGFONT* lpLogFont);

	void setFont(LONG fontHeight = -8,
		LONG fontWeight = FW_NORMAL,
		UCHAR pitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
		LPCTSTR faceName = _T("MS Sans Serif"));

	void definePopupMenu(UINT uResourceID = 0 /* 0 uses default */)
	{
		m_MenuResourceID = uResourceID;
	}

	typedef void(*DRAGFINISHPROC)(CString FileName);

	DRAGFINISHPROC lpDragFinishProc;

protected:
	CString ExpandShortcut(CString& inFile);

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditEx)
	afx_msg void OnDropFiles(HDROP dropInfo);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CEditEx)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

	// Customize your brush
	//
	virtual BOOL CreateBrushType();

private:
	CBrush   m_brBkGround;
	COLORREF m_crBkColor;
	COLORREF m_crTextColor;

	CEditMask* m_pEditMask;

	CFont* m_pCFont;

	UINT m_MenuResourceID;

public:
	bool	m_bUseDir;
	bool	m_bUseAll;
};

struct CLogFont : public LOGFONT
{
	CLogFont(LONG fontHeight = -8,
		LONG fontWeight = FW_NORMAL,
		UCHAR pitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
		LPCTSTR faceName = _T("MS Sans Serif")
	)
	{
		const int size = sizeof(*this);

		memset(this, 0, size);

		lfHeight = fontHeight;
		lfWeight = fontWeight;
		lfPitchAndFamily = pitchAndFamily;

		_ASSERT(LF_FACESIZE > lstrlen(faceName));

		lstrcpy(lfFaceName, faceName);
	}

	// Note: No need for CLogFont& operator =(const CLogFont& lf) {...}
	//       We let the compiler handle it...

	void lfFaceNameSet(LPCTSTR faceName)
	{
		_ASSERT(faceName);

		if (!faceName) return;

		_ASSERT(LF_FACESIZE > lstrlen(faceName));

		lstrcpy(lfFaceName, faceName);
	}
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DROPEDIT_H__1D8BBDC1_784C_11D1_8159_444553540000__INCLUDED_)