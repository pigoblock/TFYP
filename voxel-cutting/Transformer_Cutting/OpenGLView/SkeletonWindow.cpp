#include "stdafx.h"
#include "SkeletonWindow.h"
#include "resource.h"

IMPLEMENT_DYNCREATE(SkeletonWindow, CFrameWnd)

BEGIN_MESSAGE_MAP(SkeletonWindow, CFrameWnd)
	ON_WM_CREATE()
	//ON_COMMAND(ID_PLAY, &SkeletonWindow::OnPlayBtn)
END_MESSAGE_MAP()

SkeletonWindow::SkeletonWindow()
{
}


SkeletonWindow::~SkeletonWindow()
{
}

int SkeletonWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1){
		return -1;
	}

	//AfxMessageBox(_T("here"));
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(ID_SKELETON_TOOLBAR))
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

BOOL SkeletonWindow::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	View2 *pview;

	// Assign custom view.
	pContext->m_pNewViewClass = RUNTIME_CLASS(View2);

	// Create the view.
	pview = (View2 *)CreateView(pContext, AFX_IDW_PANE_FIRST);
	if (pview == NULL)
		return FALSE;

	// Notify the view.
	pview->SendMessage(WM_INITIALUPDATE);
	SetActiveView(pview, FALSE);

	return TRUE;
}