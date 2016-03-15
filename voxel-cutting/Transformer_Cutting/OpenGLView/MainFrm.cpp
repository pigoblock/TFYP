// MainFrm.cpp : CMainFrame

#include "stdafx.h"
#include "KEGIES.h"
#include "KEGIESDoc.h"

#include "MainFrm.h"
#include "Dialog.h"
#include "SkeletonWindow.h"
#include "InputMeshWindow.h"
#include "AnimationWindow.h"
#include "SuggestionsView.h"
#include "SuggestionsView2.h"
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_MENUSELECT()
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F1, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F2, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F3, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F4, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F5, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F6, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F7, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F8, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F9, &CMainFrame::OnUpdateSpecialF)
	ON_UPDATE_COMMAND_UI(ID_SPECIAL_F10, &CMainFrame::OnUpdateSpecialF)

	ON_UPDATE_COMMAND_UI(ID_SIDEDIALOG_HIDE, &CMainFrame::OnUpdateSidedialogMenu)
	ON_UPDATE_COMMAND_UI(ID_SIDEDIALOG_LEFT, &CMainFrame::OnUpdateSidedialogMenu)
	ON_UPDATE_COMMAND_UI(ID_SIDEDIALOG_RIGHT, &CMainFrame::OnUpdateSidedialogMenu)
	ON_BN_CLICKED(IDOK, &CMainFrame::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CMainFrame::OnBnClickedButton1)
	ON_MESSAGE(WM_TEST, &CKEGIESView::OnData)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	ID_INDICATOR_EXT,
};

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

// Creating the general UI
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1){
		return -1;
	}
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Error toolbar 1\n");
		return -1;      
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Error toolbar 2\n");
		return -1;      
	}

	m_wndStatusBar.SetPaneInfo(4, ID_TIMER_STATUS, SBPS_NORMAL, 60);
	m_wndStatusBar.SetPaneText(4, _T("Stopped"));

	CRect rect;
	int nIndex = m_wndToolBar.GetToolBarCtrl().CommandToIndex(IDD_EDIT_BOX_1);
	m_wndToolBar.SetButtonInfo(nIndex, IDD_EDIT_BOX_1, TBBS_SEPARATOR, 30);
	m_wndToolBar.GetToolBarCtrl().GetItemRect(nIndex, &rect);
	m_edit1.Create(WS_VISIBLE, rect, &m_wndToolBar, IDD_EDIT_BOX_1);

	nIndex = m_wndToolBar.GetToolBarCtrl().CommandToIndex(IDD_EDIT_BOX_2);
	m_wndToolBar.SetButtonInfo(nIndex, IDD_EDIT_BOX_2, TBBS_SEPARATOR, 30);
	m_wndToolBar.GetToolBarCtrl().GetItemRect(nIndex, &rect);
	m_edit2.Create(WS_VISIBLE, rect, &m_wndToolBar, IDD_EDIT_BOX_2);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	
	sideDlg.init(this);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT 

	return TRUE;
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
{
	CFrameWnd::OnMenuSelect(nItemID, nFlags, hSysMenu);
}

void CMainFrame::OnUpdateSpecialF(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	CKEGIESDoc* doc = (CKEGIESDoc*)GetActiveDocument();
	CString title = _T("Unused");

	pCmdUI->Enable(FALSE);
	pCmdUI->SetText(title);	
}

void CMainFrame::OnUpdateSidedialogMenu(CCmdUI *pCmdUI)
{
	switch(pCmdUI->m_nIndex){
		case 0:
			sideDlg.OnUpdateSidedialogHide(pCmdUI);
			break;
		case 2:
			sideDlg.OnUpdateSidedialogLeft(pCmdUI);
			break;
		case 3:
			sideDlg.OnUpdateSidedialogRight(pCmdUI);
			break;
	}
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	int nItemID = pMsg->wParam;
	CKEGIESDoc* doc = (CKEGIESDoc*)GetActiveDocument();

	// Special menu function
// 	for (int i = 0; i< 10; i++)
// 	{
// 		if (nItemID == ID_SPECIAL_F1 + i)
// 		{
// 			doc->document->specialMenuFunction(nItemID);
// 			break;
// 		}
// 	}

	// Side dialog view
	switch(nItemID)
	{
	case ID_SIDEDIALOG_HIDE:
		sideDlg.OnSidedialogHide();
		break;
	case ID_SIDEDIALOG_LEFT:
		sideDlg.OnSidedialogLeft();
		break;
	case ID_SIDEDIALOG_RIGHT:
		sideDlg.OnSidedialogRight();
		break;
	}
	
	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::setStatusBar( CString status, int index )
{
	m_wndStatusBar.SetPaneText(index, status);
}

void CMainFrame::timerUpdate( bool start )
{
	setStatusBar(start? _T("Running..."):_T("Stopped"), 4);

	CToolBarCtrl& Toolbar = m_wndToolBar.GetToolBarCtrl();
	CImageList *pList = Toolbar.GetImageList();
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(start? ID_ICON_PAUSE:ID_ICON_PLAY));
	pList->Replace(5, hIcon);
	m_wndToolBar.Invalidate();
}

void CMainFrame::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
}

void CMainFrame::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	TRACE(_T("dfdfdfdfdfdf\n"));
}

// Creates the split window view
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	int ncwidth = 1200;
	int ncheight = 600;

	// Set up the splitter window interface
	if (!m_mainWndSplitter.CreateStatic(this, 1, 2)){
		return FALSE;
	}
	m_mainWndSplitter.SetColumnInfo(0, ncwidth*0.6, ncwidth*0.1);
	m_mainWndSplitter.SetColumnInfo(1, ncwidth*0.4, ncwidth*0.1);

	// Create left views
	if (!m_mainLeftWndSplitter.CreateStatic(&m_mainWndSplitter, 2, 1, WS_CHILD | WS_VISIBLE, m_mainWndSplitter.IdFromRowCol(0, 0))){
		return FALSE;
	}
	m_mainLeftWndSplitter.SetRowInfo(0, ncheight*0.25, ncheight*0.1);
	m_mainLeftWndSplitter.SetRowInfo(1, ncheight*0.75, ncheight*0.1);
	m_mainLeftWndSplitter.SetColumnInfo(0, ncwidth*0.75, ncwidth*0.1);

	if (!m_mainLeftWndSplitter.CreateView(1, 0, RUNTIME_CLASS(InputMeshWindow), CSize(ncwidth*0.5, ncheight*0.5), pContext)){
		m_mainLeftWndSplitter.DestroyWindow();
		return FALSE;
	}

	// Create top left 4 suggestion views
	if (!m_suggestionsWndSplitter.CreateStatic(&m_mainLeftWndSplitter, 1, 2, WS_CHILD | WS_VISIBLE, m_mainLeftWndSplitter.IdFromRowCol(0, 0))){
		return FALSE;
	}

	if (!m_suggestionsWndSplitter.CreateView(0, 0, RUNTIME_CLASS(SuggestionsView), CSize(ncwidth*0.5*0.25, ncheight*0.5), pContext)
		|| !m_suggestionsWndSplitter.CreateView(0, 1, RUNTIME_CLASS(SuggestionsView2), CSize(ncwidth*0.5*0.25, ncheight*0.5), pContext)){
		m_suggestionsWndSplitter.DestroyWindow();
		return FALSE;
	}

	m_suggestionsWndSplitter.SetColumnInfo(0, ncwidth*0.75, ncwidth*0.1);
	m_suggestionsWndSplitter.SetColumnInfo(1, ncwidth*0, ncwidth*0.1);

	// Create right views
	if (!m_subWndSplitter.CreateStatic(&m_mainWndSplitter, 2, 1, WS_CHILD | WS_VISIBLE, m_mainWndSplitter.IdFromRowCol(0, 1))){
		return FALSE;
	}
	m_subWndSplitter.SetRowInfo(0, ncheight*0.5, ncheight*0.1);
	m_subWndSplitter.SetRowInfo(1, ncheight*0.5, ncheight*0.1);
	m_subWndSplitter.SetColumnInfo(0, ncwidth*0.25, ncwidth*0.1);

	if (!m_subWndSplitter.CreateView(0, 0, RUNTIME_CLASS(SkeletonWindow), CSize(ncwidth*0.5, ncheight*0.6), pContext) ||
		!m_subWndSplitter.CreateView(1, 0, RUNTIME_CLASS(AnimationWindow), CSize(ncwidth*0.5, ncheight*0.4), pContext)){
		m_subWndSplitter.DestroyWindow();
		return FALSE;
	}

	m_mainLeftWndSplitter.RecalcLayout();
	m_suggestionsWndSplitter.RecalcLayout();
	m_subWndSplitter.RecalcLayout();

	return TRUE;
}
