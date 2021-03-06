// MainFrm.h : CMainFrame 

#pragma once
#include "SideDialog.h"
#include "Dialog.h"

class CMainFrame : public CFrameWnd
{
	
public: // serialization
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext);
//	virtual void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);

	virtual ~CMainFrame();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	CSplitterWnd m_mainWndSplitter;	// can be private actually
	CSplitterWnd m_mainLeftWndSplitter;	// can be private actually
	CSplitterWnd m_suggestionsWndSplitter;	// can be private actually
	CSplitterWnd m_subWndSplitter;	// can be private actually

	CComboBox m_comboBox;
	CEdit m_edit1;
	CEdit m_edit2;
	CEdit editVolumeError;

	SideDialog sideDlg;
	dialogVoxel test;
	dialogVoxel* getVoxelDialog(){return &test;}

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void OnUpdateSpecialF(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSidedialogMenu(CCmdUI *pCmdUI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	void setStatusBar(CString status, int index);
	void timerUpdate(bool start);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
};


