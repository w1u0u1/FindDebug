#pragma once
#include "afxcmn.h"
#include "afxext.h"

class CStatusBarEx : public CStatusBar
{
public:
	CStatusBarEx();
	virtual ~CStatusBarEx();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};