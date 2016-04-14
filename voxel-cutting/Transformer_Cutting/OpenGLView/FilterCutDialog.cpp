// FilterCutDialog.cpp : implementation file

#include "stdafx.h"
#include "FilterCutDialog.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "cutSurfTreeMngr2.h"
#include "skeleton.h"
#include "MainControl.h"

using namespace std;

IMPLEMENT_DYNAMIC(FilterCutDialog, CDialogEx)

FilterCutDialog::FilterCutDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(FilterCutDialog::IDD, pParent)
{

}

FilterCutDialog::~FilterCutDialog()
{
}

void FilterCutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_COMBO1, joinComboBox);
//	DDX_Control(pDX, IDC_COMBO2, JoinTypeComboBox);

	DDX_Control(pDX, IDC_EDIT1, currentPose);
	DDX_Control(pDX, IDC_EDIT2, numPoses);
	DDX_Control(pDX, IDC_EDIT3, weightVolume);
	DDX_Control(pDX, IDC_EDIT8, weightHash);
	DDX_Control(pDX, IDC_EDIT5, weightCB);

	DDX_Control(pDX, IDC_EDIT9, volumeError);
	DDX_Control(pDX, IDC_EDIT10, CBError);
	DDX_Control(pDX, IDC_EDIT11, hashError);
	DDX_Control(pDX, IDC_EDIT12, overallError);

//	DDX_Control(pDX, IDC_EDIT3, feedback);
}


BEGIN_MESSAGE_MAP(FilterCutDialog, CDialogEx)
//	ON_BN_CLICKED(IDC_BUTTON1, &FilterCutDialog::OnBnClickedButton1)
//	ON_CBN_SELCHANGE(IDC_COMBO1, &FilterCutDialog::OnCbnSelchangeJoin)
//	ON_CBN_SELCHANGE(IDC_COMBO2, &FilterCutDialog::OnCbnSelchangeJoinType)
//	ON_BN_CLICKED(IDC_BUTTON2, &FilterCutDialog::OnBnClickedClearFilter)
	ON_BN_CLICKED(IDC_BUTTON1, &FilterCutDialog::OnPreviousPose)
	ON_BN_CLICKED(IDC_BUTTON2, &FilterCutDialog::OnNextPose)
	ON_BN_CLICKED(IDC_BUTTON3, &FilterCutDialog::OnLockPose)
	ON_BN_CLICKED(IDC_BUTTON4, &FilterCutDialog::OnProceed)
	ON_BN_CLICKED(IDC_BUTTON7, &FilterCutDialog::OnSort)
END_MESSAGE_MAP()


// FilterCutDialog message handlers

void FilterCutDialog::init(int poseSize)
{
	numTotalPoses = poseSize;
	CString a;
	a.Format(_T("%d"), numTotalPoses);
	numPoses.SetWindowText(a);

	curPoseIndex = 0;
	a.Format(_T("%d"), curPoseIndex);
	currentPose.SetWindowText(a);

	savedPose1 = -1;
	savedPose2 = -1;
	lastSaved = -1;

	needsUpdate = false;

	weights[0] = 1;
	weights[1] = 0;
	weights[2] = 0;

	a.Format(_T("%d"), 1);
	weightVolume.SetWindowText(a);
	a.Format(_T("%d"), 0);
	weightHash.SetWindowText(a);
	weightCB.SetWindowText(a);
}

void FilterCutDialog::OnPreviousPose()
{
	if (curPoseIndex - 1 >= 0){
		curPoseIndex -= 1;

		CString a;
		a.Format(_T("%d"), curPoseIndex);
		currentPose.SetWindowText(a);

		setPoseToDraw();
	}
}

void FilterCutDialog::OnNextPose()
{
	if (curPoseIndex + 1 < numTotalPoses){
		curPoseIndex += 1;

		CString a;
		a.Format(_T("%d"), curPoseIndex);
		currentPose.SetWindowText(a);

		setPoseToDraw();
	}
}

void FilterCutDialog::setPoseToDraw(){
	doc->updatePoseToDraw(curPoseIndex);
}

void FilterCutDialog::OnLockPose(){
	savedPose1 = curPoseIndex;
	setSavedPose1ToDraw();
}


void FilterCutDialog::OnProceed()
{
	if (savedPose1 > -1 && savedPose1 < numTotalPoses){
		savePoseToNextStep(savedPose1);
	}
}

void FilterCutDialog::setSavedPose1ToDraw(){
	doc->updateSavedPose1ToDraw(curPoseIndex);
}

void FilterCutDialog::savePoseToNextStep(int chosenPose){
	doc->savePoseToNextStep(chosenPose);
}

void FilterCutDialog::OnSort(){
	// Recalculate weight ratios and scores
	CString csText;

	GetDlgItemText(IDC_EDIT3, csText);
	weights[0] = _tstof((LPCTSTR)csText);
	GetDlgItemText(IDC_EDIT8, csText);
	weights[1] = _tstof((LPCTSTR)csText);
	GetDlgItemText(IDC_EDIT5, csText);
	weights[2] = _tstof((LPCTSTR)csText);

	doc->m_cutSurface.updateSortEvaluations(weights);
	doc->updatePoseToDraw(curPoseIndex);
}

void FilterCutDialog::updateDisplayedOverallError(float value){
	CString a;
	a.Format(_T("%.2f"), value);
	overallError.SetWindowText(a);
}

void FilterCutDialog::updateDisplayedVolumeError(float value){
	CString a;
	a.Format(_T("%.2f"), value);
	volumeError.SetWindowText(a);
}

void FilterCutDialog::updateDisplayedHashError(float value){
	CString a;
	a.Format(_T("%.2f"), value);
	hashError.SetWindowText(a);
}

void FilterCutDialog::updateDisplayedCBError(float value){
	CString a;
	a.Format(_T("%.2f"), value);
	CBError.SetWindowText(a);
}
