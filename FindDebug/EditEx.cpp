/*********************************************************************

   Copyright (C) 2002 Smaller Animals Software, Inc.

   This software is provided 'as-is', without any express or implied
   warranty.  In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source distribution.

   http://www.smalleranimals.com
   smallest@smalleranimals.com
   **********************************************************************/

   /**********************************************************************
	   Shortcut expansion code, 1996 by Rob Warner
	   part of "CShortcut"
	   rhwarner@southeast.net
	   http://users.southeast.net/~rhwarner

	   modified for CEditEx, Chris Losinger
	   **********************************************************************/

	   ////
	   //
	   //	To use this in an app, you'll need to :
	   //
	   //	1) #include <afxole.h> in stdafx.h
	   //	
	   //	2) in your CWinApp-derived class *::InitInstance, you'll need to call
	   //		::CoInitialize(NULL);
	   //
	   //	3) in your CWinApp-derived class *::ExitInstance, you'll need to call
	   //	::CoUninitialize();
	   //
	   //	4) Place a normal edit control on your dialog. 
	   //	5) Check the "Accept Files" property.
	   //
	   //	6) In your dialog class, declare a member variable of type CEditEx
	   //	(be sure to #include "CEditEx.h")
	   //		ex. CEditEx m_dropEdit;
	   //
	   //	7) In your dialog's OnInitDialog, call
	   //		m_dropEdit.SubclassDlgItem(IDC_YOUR_EDIT_ID, this);
	   //
	   //	8) if you want the edit control to handle directories, call
	   //		m_dropEdit.SetUseDir(TRUE);
	   //
	   //	9) if you want the edit control to handle files, call
	   //		m_dropEdit.SetUseDir(FALSE);
	   //
	   //	that's it!
	   //
	   //	This will behave exactly like a normal edit-control but with the 
	   //	ability to accept drag-n-dropped files (or directories).
	   //
	   //

	   // Copyright (C) 1996 Bill Berry All rights reserved.
	   //
	   // This class is freely distributable as long as the above copyright 
	   // accompanies the header and implementaion sources.
	   //
	   // Also, please send me any improvements or bug fixes to bberry@javanet.com
	   //
	   //
	   // Modification History:
	   //
	   // Bill Berry           November 1996   Created
	   //                      March 1998      Update
	   //
	   // Description:
	   //
	   //  Extended CEdit class. Allows easy customization of the following:
	   //
	   //      1. COLORREF bkColor( COLORREF crColor )
	   //         - Sets back ground color of the control
	   //
	   //      2. COLORREF bkColor()
	   //         - Returns back ground color of control
	   //
	   //      3. COLORREF textColor( COLORREF crColor )
	   //         - Sets text or foreground color of the control
	   //
	   //      4. COLORREF textColor() const
	   //         - Returns text (or foreground) color of control
	   //
	   //      5. void setCustomMask( CEditMask* editMask /* NULL means default state */ )
	   //
	   //      6. void definePopupMenu( UINT uResourceID = 0 /* 0 uses default */ )
	   //         - Overide default context menu with new menu resource.
	   //
	   //      *** Set new font for this control ***
	   //
	   //      7. void setFont( const LOGFONT* lpLogFont );
	   //  
	   //      8. void setFont( LONG fontHeight      = -8, 
	   //                       LONG fontWeight      = FW_NORMAL,
	   //                       UCHAR pitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
	   //                       LPCSTR faceName      = _T("MS Sans Serif"          
	   //                     );
	   //
	   // Remarks:
	   //
	   //      When using the CEditEx control do not free the CEditMask pointer
	   //      assigned by the setCustomMask call. This class will free it for 
	   //      you once it leaves scope.
	   //

#include "stdafx.h"
#include "EditEx.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditEx
CEditEx::CEditEx() : m_pCFont(0)
{
	m_bUseDir = FALSE;
	m_bUseAll = FALSE;

	// Use default popup menu
	//
	m_MenuResourceID = 0;

	// Use system colors for defaults
	//
	m_crTextColor = ::GetSysColor(COLOR_WINDOWTEXT);
	m_crBkColor = ::GetSysColor(COLOR_WINDOW);

	// The default brush type: SOLID
	//
	CreateBrushType();

	// Edit mask is null
	//
	m_pEditMask = 0;

	// DragFinishProc NULL
	//
	lpDragFinishProc = NULL;
}

//** CEditEx::~CEditEx()
//
CEditEx::~CEditEx()
{
	if (m_pCFont) delete m_pCFont;
	if (m_pEditMask) delete m_pEditMask;
}

BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	//{{AFX_MSG_MAP(CEditEx)
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Note: Copy construction is blocked for this class.
//       This is because there would be no defined
//       m_hWnd during the construction of the object.
//
// CEditEx::CEditEx( const CEditEx& o )
//

// Allow = operator to be used for copying basics.
//
CEditEx& CEditEx::operator = (const CEditEx& o)
{
	_ASSERT(o != *this); // You probably did not mean to do this...

	if (o == *this) return *this; // copying self...

	bkColor(o.m_crBkColor);
	textColor(o.m_crTextColor);

	if (o.m_pCFont)
	{
		CLogFont pLogFont;
		o.m_pCFont->GetLogFont(&pLogFont);
		setFont(&pLogFont);
	}

	return *this;
}

//** void SetFont( const LOGFONT* lpLogFont )
//

void CEditEx::setFont(const LOGFONT* lpLogFont)
{
	_ASSERT(lpLogFont); // logfont is not defined!!!

	if (!lpLogFont) return;

	if (m_pCFont) delete m_pCFont;

	m_pCFont = new CFont;
	m_pCFont->CreateFontIndirect(lpLogFont);

	SetFont(m_pCFont);
}

void CEditEx::setFont(LONG fontHeight      /* = -8                         */,
	LONG fontWeight      /* = FW_NORMAL                  */,
	UCHAR pitchAndFamily /* = DEFAULT_PITCH | FF_DONTCARE*/,
	LPCTSTR faceName      /* = _T("MS Sans Serif")        */)
{
	if (m_pCFont)
	{
		delete m_pCFont;
	}
	m_pCFont = new CFont;

	const CLogFont lf(fontHeight,
		FW_NORMAL,
		pitchAndFamily,
		faceName
	);

	m_pCFont->CreateFontIndirect(&lf);

	SetFont(m_pCFont);
}

//** COLORREF CEditEx::bkColor( COLORREF crColor )
//
COLORREF CEditEx::bkColor(COLORREF crColor)
{
	_ASSERT(::IsWindow(m_hWnd));

	COLORREF crPrevBkColor = m_crBkColor;

	m_crBkColor = crColor;

	m_brBkGround.DeleteObject();

	CreateBrushType();

	Invalidate();

	return crPrevBkColor;
}

//** COLORREF CEditEx::textColor( COLORREF crColor )
//
COLORREF CEditEx::textColor(COLORREF crColor)
{
	_ASSERT(::IsWindow(m_hWnd));

	COLORREF crPrevTextColor = m_crTextColor;

	m_crTextColor = crColor;

	Invalidate();

	return crPrevTextColor;
}

/////////////////////////////////////////////////////////////////////////////
// Protected and private interfaces:

//
//** BOOL CEditEx::CreateBrushType()
//
BOOL CEditEx::CreateBrushType()
{
	return m_brBkGround.CreateSolidBrush(m_crBkColor);
}

//** void CEditEx::OnContextMenu(CWnd* pWnd, CPoint point) 
//
void CEditEx::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// Use default popup
	//
	if (!m_MenuResourceID) {
		CEdit::OnContextMenu(pWnd, point);
		return;
	}

	// Replace default popup menu
	//
	CMenu menu;

	if (!menu.LoadMenu(m_MenuResourceID))
	{
		CEdit::OnContextMenu(pWnd, point);
	}
	else
	{
		CMenu* pContext = menu.GetSubMenu(0);
		pContext->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

//** void CEditEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
//
void CEditEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (0 == m_pEditMask) {
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		return;
	}

	// else Use custom mask...
	//
	if (m_pEditMask->AddChar(nChar))
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	else
		MessageBeep(0);
}

//** HBRUSH CEditEx::CtlColor(CDC* pDC, UINT nCtlColor) 
//
HBRUSH CEditEx::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetTextColor(m_crTextColor);
	pDC->SetBkColor(m_crBkColor);
	return (HBRUSH)m_brBkGround;
}

/////////////////////////////////////////////////////////////////////////////
// CEditEx message handlers

/////////////////////////////////////////////////////////////////////////////
//	handles WM_DROPFILES
void CEditEx::OnDropFiles(HDROP dropInfo)
{
	// Get the number of pathnames that have been dropped
	WORD wNumFilesDropped = DragQueryFile(dropInfo, -1, NULL, 0);

	CString csFirstFile = _T("");

	// there may be many, but we'll only use the first
	if (wNumFilesDropped > 0)
	{
		// Get the number of bytes required by the file's full pathname
		WORD wPathnameSize = DragQueryFile(dropInfo, 0, NULL, 0);

		// Allocate memory to contain full pathname & zero byte
		wPathnameSize += 1;

		TCHAR* pFile = new TCHAR[wPathnameSize];

		if (pFile == NULL)
		{
			ASSERT(0);
			DragFinish(dropInfo);
			return;
		}

		// Copy the pathname into the buffer
		DragQueryFile(dropInfo, 0, pFile, wPathnameSize);

		csFirstFile = pFile;

		// clean up
		delete[] pFile;
	}

	// Free the memory block containing the dropped-file information
	DragFinish(dropInfo);

	// if this was a shortcut, we need to expand it to the target path
	CString csExpandedFile = ExpandShortcut(csFirstFile);

	// if that worked, we should have a real file name
	if (!csExpandedFile.IsEmpty())
	{
		csFirstFile = csExpandedFile;
	}

	if (!csFirstFile.IsEmpty())
	{
		if (m_bUseAll)
			SetWindowText(csFirstFile);
		else
		{
			struct _stat buf;

			// get some info about that file
			int result = _tstat(csFirstFile, &buf);

			if (result == 0)
			{
				// verify that we have a dir (if we want dirs)
				if ((buf.st_mode & _S_IFDIR) == _S_IFDIR)
				{
					if (m_bUseDir)
					{
						SetWindowText(csFirstFile);
					}

					// verify that we have a file (if we want files)
				}
				else if ((buf.st_mode & _S_IFREG) == _S_IFREG)
				{
					if (!m_bUseDir)
					{
						SetWindowText(csFirstFile);
					}
				}
			}
		}
	}

	if (lpDragFinishProc != NULL)
		lpDragFinishProc(csFirstFile);
}

//////////////////////////////////////////////////////////////////
//	use IShellLink to expand the shortcut
//	returns the expanded file, or "" on error
//
//	original code was part of CShortcut 
//	1996 by Rob Warner
//	rhwarner@southeast.net
//	http://users.southeast.net/~rhwarner
CString CEditEx::ExpandShortcut(CString& inFile)
{
	CString outFile;

	// Make sure we have a path
	ASSERT(inFile != _T(""));

	IShellLink* psl;
	HRESULT hres;
	LPTSTR lpsz = inFile.GetBuffer(MAX_PATH);

	// Create instance for shell link
	hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);

	if (SUCCEEDED(hres))
	{
		// Get a pointer to the persist file interface
		IPersistFile* ppf;

		hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

		if (SUCCEEDED(hres))
		{
			// Load shortcut
			hres = ppf->Load(lpsz, STGM_READ);

			if (SUCCEEDED(hres))
			{
				WIN32_FIND_DATA wfd;

				// find the path from that
				HRESULT hres = psl->GetPath(outFile.GetBuffer(MAX_PATH),
					MAX_PATH,
					&wfd,
					SLGP_UNCPRIORITY);

				outFile.ReleaseBuffer();
			}

			ppf->Release();
		}

		psl->Release();
	}

	inFile.ReleaseBuffer();

	// if this fails, outFile == ""
	return outFile;
}

/////////////////////////////////////////////////////////////////////////////
void CEditEx::PreSubclassWindow()
{
	DragAcceptFiles(TRUE);

	CEdit::PreSubclassWindow();
}