#include "StdAfx.h"
#include "SideDialog.h"
#include "MainFrm.h"
#include "KEGIESDoc.h"
#include <stdio.h>

#define DIALOG_POSITION _T("Side dialog's position")
#define DIALOG_SHOW		_T("Show/hide side dialog bar")

IMPLEMENT_DYNAMIC(CInitDialogBar, CDialogBar)

CInitDialogBar::CInitDialogBar()
{
	// In derived classes set intial
	// state of control(s) here
}

CInitDialogBar::~CInitDialogBar()
{

}

BOOL CInitDialogBar::Create(CWnd * pParentWnd, LPCTSTR lpszTemplateName,
	UINT nStyle, UINT nID)
{
	// Let MFC Create the control
	if (!CDialogBar::Create(pParentWnd, lpszTemplateName, nStyle, nID))
		return FALSE;

	// Since there is no WM_INITDIALOG message we have to call
	// our own InitDialog function ourselves after m_hWnd is valid
	if (!OnInitDialogBar())
		return FALSE;

	return TRUE;
}

BOOL CInitDialogBar::Create(CWnd * pParentWnd, UINT nIDTemplate,
	UINT nStyle, UINT nID)
{
	if (!Create(pParentWnd, MAKEINTRESOURCE(nIDTemplate), nStyle, nID))
		return FALSE;

	// Since there is no WM_INITDIALOG message we have to call
	// our own InitDialog function ourselves after m_hWnd is valid
	if (!OnInitDialogBar())
		return FALSE;
	return TRUE;
}

BOOL CInitDialogBar::OnInitDialogBar()
{
	// Support for the MFC DDX model 
	// If you do not want this do not call the base class
	// from derived classes
	UpdateData(FALSE);

	return TRUE;
}

void CInitDialogBar::DoDataExchange(CDataExchange* pDX)
{
	//Derived Classes Overide this function
	ASSERT(pDX);

	CDialogBar::DoDataExchange(pDX);

	// In derived class call the DDX_??? functions to set/retrieve values
	// of your controls. See example derived class for how to do this.
}



BEGIN_MESSAGE_MAP(SideDialog, CDialogBar)
	ON_BN_CLICKED(IDC_BUTTON1, &SideDialog::OnSortClicked)
END_MESSAGE_MAP()


SideDialog::SideDialog(void)
{
	//Load setting
	position = DIALOG_LEFT;
	shown = true;

	parent = NULL;
}

SideDialog::~SideDialog(void)
{
	// Save setting
}

BOOL SideDialog::OnInitDialogBar()
{
	CInitDialogBar::OnInitDialogBar();

	return TRUE;
}

void SideDialog::DoDataExchange(CDataExchange* pDX)
{
	CInitDialogBar::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, volumeError);
	DDX_Control(pDX, IDC_EDIT3, weightVolume);
	DDX_Control(pDX, IDC_EDIT4, weightHash);
	DDX_Control(pDX, IDC_EDIT5, weightCB);
}

UINT positionConvert(dialogPos pos)
{
	switch(pos)
	{
	case DIALOG_LEFT:
		return CBRS_LEFT;
	case DIALOG_RIGHT:
		return CBRS_RIGHT;
	}
	return 0;
}

void SideDialog::init(CWnd* pParentWnd)
{
	parent = (CMainFrame*)pParentWnd;
	Create(pParentWnd, IDD_DIALOGBAR, positionConvert(position), IDD_DIALOGBAR);
	ShowWindow(shown);

	needsUpdate = false;

	for (int i = 0; i < 3; i++){
		weights[i] = 1;
	}

	CString a;
	a.Format(_T("%d"), 0);
	volumeError.SetWindowText(a);

	a.Format(_T("%d"), 1);
	weightVolume.SetWindowText(a);
	weightHash.SetWindowText(a);
	weightCB.SetWindowText(a);
}

void SideDialog::updateDisplayedValues(float value){
	CString a; 
	a.Format(_T("%f"), value);
	volumeError.SetWindowText(a);
}


void SideDialog::OnSortClicked(){
	// Recalculate weight ratios and scores
	CString csText;

	GetDlgItemText(IDC_EDIT3, csText);
	weights[0] = _tstof((LPCTSTR)csText);
	GetDlgItemText(IDC_EDIT4, csText);
	weights[1] = _tstof((LPCTSTR)csText);
	GetDlgItemText(IDC_EDIT5, csText);
	weights[2] = _tstof((LPCTSTR)csText);

	needsUpdate = true;
}

void SideDialog::OnSidedialogHide()
{
	shown = !shown;

	ShowWindow(shown);
	parent->RecalcLayout(TRUE);
}

void SideDialog::OnSidedialogLeft()
{
	position = DIALOG_LEFT;
}

void SideDialog::OnSidedialogRight()
{
	position = DIALOG_RIGHT;
}

void SideDialog::OnUpdateSidedialogHide(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(shown);
	pCmdUI->Enable(TRUE);	
}

void SideDialog::OnUpdateSidedialogLeft(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(position == DIALOG_LEFT);
	pCmdUI->Enable(TRUE);
}

void SideDialog::OnUpdateSidedialogRight(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(position == DIALOG_RIGHT);
	pCmdUI->Enable(TRUE);
}