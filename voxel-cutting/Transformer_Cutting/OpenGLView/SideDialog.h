#pragma once
#include "afxext.h"
#include "stdafx.h"
#include "resource.h"

class CMainFrame;

typedef enum dialogPos_
{
	DIALOG_LEFT = 0,
	DIALOG_RIGHT
} dialogPos;

// Allow CDialogBar to support DDX
// Code from: http://www.cnblogs.com/taoxu0903/archive/2009/05/30/1491932.html
class CInitDialogBar : public CDialogBar
{
	DECLARE_DYNAMIC(CInitDialogBar)

	// Construction / Destruction
public:
	CInitDialogBar();
	virtual ~CInitDialogBar();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	BOOL Create(CWnd * pParentWnd, UINT nIDTemplate, UINT nStyle, UINT
		nID);
	BOOL Create(CWnd * pParentWnd, LPCTSTR lpszTemplateName, UINT
		nStyle, UINT nID);

protected:
	virtual BOOL OnInitDialogBar();
};



class SideDialog : public CInitDialogBar
{
public:
	SideDialog(void);
	~SideDialog(void);

	bool shown;
	bool needsUpdate;
	float weights[3];

	//CMFCTasksPane test;
	dialogPos position;
	CMainFrame* parent;

	void init(CWnd* pParentWnd);

	void OnSidedialogHide();
	void OnSidedialogLeft();
	void OnSidedialogRight();
	void OnUpdateSidedialogHide(CCmdUI *pCmdUI);
	void OnUpdateSidedialogLeft(CCmdUI *pCmdUI);
	void OnUpdateSidedialogRight(CCmdUI *pCmdUI);

	CEdit overallError;
	CEdit volumeError;
	CEdit hashError;
	CEdit CBError;
	CEdit weightVolume;
	CEdit weightHash;
	CEdit weightCB;
	
	void updateDisplayedOverallError(float value);
	void updateDisplayedVolumeError(float value);
	void updateDisplayedHashError(float value);
	void updateDisplayedCBError(float value);
	
	afx_msg void OnSortClicked();
	afx_msg void OnPrevPoseClicked();
	afx_msg void OnNextPoseClicked();

protected:
	virtual BOOL OnInitDialogBar();
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};