#include "stdafx.h"
#include "StatusBarEx.h"
#include "MemDCEx.h"


CStatusBarEx::CStatusBarEx()
{
}

CStatusBarEx::~CStatusBarEx()
{
}

BEGIN_MESSAGE_MAP(CStatusBarEx, CStatusBar)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


void CStatusBarEx::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: Add your message handler code here
    // Do not call CStatusBar::OnPaint() for painting messages
    CRect rect;
    GetClientRect(&rect);
    CMemDCEx memDC(&dc, &rect);

    DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
}

BOOL CStatusBarEx::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default
    //return CStatusBar::OnEraseBkgnd(pDC);
    return TRUE;
}