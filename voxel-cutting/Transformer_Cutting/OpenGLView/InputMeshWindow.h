#include "KEGIESView.h"

#pragma once
class InputMeshWindow : public CFrameWnd
{
public:
	DECLARE_DYNCREATE(InputMeshWindow)
	DECLARE_MESSAGE_MAP()

	InputMeshWindow();
	~InputMeshWindow();

	// Initialize look of the animation window
	CToolBar    m_wndToolBar;
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	CEdit toolbarText;
};

