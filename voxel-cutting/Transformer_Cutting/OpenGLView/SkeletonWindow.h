#include "View2.h"

#pragma once
class SkeletonWindow : public CFrameWnd
{
public:
	DECLARE_DYNCREATE(SkeletonWindow)
	DECLARE_MESSAGE_MAP()

	SkeletonWindow();
	~SkeletonWindow();

	// Initialize look of the animation window
	CToolBar    m_wndToolBar;
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	CEdit m_edit1;
};

