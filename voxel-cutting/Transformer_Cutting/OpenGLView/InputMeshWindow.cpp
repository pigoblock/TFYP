#include "stdafx.h"
#include "InputMeshWindow.h"
#include "resource.h"

IMPLEMENT_DYNCREATE(InputMeshWindow, CFrameWnd)

BEGIN_MESSAGE_MAP(InputMeshWindow, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

InputMeshWindow::InputMeshWindow()
{
}


InputMeshWindow::~InputMeshWindow()
{
}

int InputMeshWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1){
		return -1;
	}

	//AfxMessageBox(_T("here"));
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(ID_INPUT_MESH_TOOLBAR))
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

BOOL InputMeshWindow::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	CKEGIESView *pview;

	// Assign custom view.
	pContext->m_pNewViewClass = RUNTIME_CLASS(CKEGIESView);

	// Create the view.
	pview = (CKEGIESView *)CreateView(pContext, AFX_IDW_PANE_FIRST);
	if (pview == NULL)
		return FALSE;

	// Notify the view.
	pview->SendMessage(WM_INITIALUPDATE);
	SetActiveView(pview, FALSE);

	return TRUE;
}