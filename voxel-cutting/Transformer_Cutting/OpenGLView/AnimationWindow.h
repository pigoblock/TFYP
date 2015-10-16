#include "AnimationView.h"

#pragma once
class AnimationWindow : public CFrameWnd
{
public:
	DECLARE_DYNCREATE(AnimationWindow)
	DECLARE_MESSAGE_MAP()

	AnimationWindow();
	~AnimationWindow();

	CToolBar    m_wndToolBar;
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

