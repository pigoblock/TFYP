#include "stdafx.h"
#include "AnimationWindow.h"
#include "resource.h"

IMPLEMENT_DYNCREATE(AnimationWindow, CFrameWnd)

BEGIN_MESSAGE_MAP(AnimationWindow, CFrameWnd)
	ON_WM_CREATE()
	//ON_COMMAND(ID_PLAY, &AnimationWindow::OnPlayBtn)
END_MESSAGE_MAP()

AnimationWindow::AnimationWindow()
{
}


AnimationWindow::~AnimationWindow()
{
}

int AnimationWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1){
		return -1;
	}
	
	//AfxMessageBox(_T("here"));
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(ID_ANIMATION_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar, AFX_IDW_DOCKBAR_BOTTOM);
	
	return 0;
}

BOOL AnimationWindow::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	AnimationView *pview;

	// Assign custom view.
	pContext->m_pNewViewClass = RUNTIME_CLASS(AnimationView);

	// Create the view.
	pview = (AnimationView *)CreateView(pContext, AFX_IDW_PANE_FIRST);
	if (pview == NULL)
		return FALSE;

	// Notify the view.
	pview->SendMessage(WM_INITIALUPDATE);
	SetActiveView(pview, FALSE);

	return TRUE;
}

void AnimationWindow::OnPlayBtn()
{

}