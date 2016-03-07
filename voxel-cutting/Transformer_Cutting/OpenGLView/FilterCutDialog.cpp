// FilterCutDialog.cpp : implementation file
//

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
//	DDX_Control(pDX, IDC_EDIT3, feedback);
}


BEGIN_MESSAGE_MAP(FilterCutDialog, CDialogEx)
//	ON_BN_CLICKED(IDC_BUTTON1, &FilterCutDialog::OnBnClickedButton1)
//	ON_CBN_SELCHANGE(IDC_COMBO1, &FilterCutDialog::OnCbnSelchangeJoin)
//	ON_CBN_SELCHANGE(IDC_COMBO2, &FilterCutDialog::OnCbnSelchangeJoinType)
//	ON_BN_CLICKED(IDC_BUTTON2, &FilterCutDialog::OnBnClickedClearFilter)
	ON_BN_CLICKED(IDC_BUTTON1, &FilterCutDialog::OnPreviousPose)
	ON_BN_CLICKED(IDC_BUTTON2, &FilterCutDialog::OnNextPose)
	ON_BN_CLICKED(IDC_BUTTON3, &FilterCutDialog::OnSavePoseLeft)
	ON_BN_CLICKED(IDC_BUTTON6, &FilterCutDialog::OnSavePoseRight)
	ON_BN_CLICKED(IDC_BUTTON4, &FilterCutDialog::OnLeftPose)
	ON_BN_CLICKED(IDC_BUTTON5, &FilterCutDialog::OnRightPose)
END_MESSAGE_MAP()


// FilterCutDialog message handlers

void FilterCutDialog::initFromCutTree(cutSurfTreeMngr2 *cutMangr)
{
	// The neighbor list
/*	arrayVec2i neighborPair = cutMangr->poseMngr.neighborPair;
	vector<bone*> sortedBone = cutMangr->poseMngr.sortedBone;

	for (auto nb : neighborPair)
	{
		CString joinStr;
		joinStr = sortedBone[nb[0]]->m_name + _T(" -> ") + sortedBone[nb[1]]->m_name;
		joinComboBox.AddString(joinStr);

		chosenPose.push_back(NONE_NB);
	}

	// Filter combo box
	JoinTypeComboBox.AddString(_T("None"));
	JoinTypeComboBox.AddString(_T("X+"));
	JoinTypeComboBox.AddString(_T("X-"));
	JoinTypeComboBox.AddString(_T("Y+"));
	JoinTypeComboBox.AddString(_T("Y-"));
	JoinTypeComboBox.AddString(_T("Z+"));
	JoinTypeComboBox.AddString(_T("Z-"));

	joinComboBox.SetCurSel(0);
	OnCbnSelchangeJoin();
	*/
	numTotalPoses = cutMangr->poseMngr.allPoses.size();
	CString a;
	a.Format(_T("%d"), numTotalPoses);
	numPoses.SetWindowText(a);

	curPoseIndex = 0;
	a.Format(_T("%d"), curPoseIndex);
	currentPose.SetWindowText(a);

	savedPose1 = -1;
	savedPose2 = -1;
	lastSaved = -1;
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

void FilterCutDialog::OnSavePoseLeft(){
	savedPose1 = curPoseIndex;
	lastSaved = 1;
	setSavedPose1ToDraw();

	CString a;
	a.Format(_T("%s"), "Saved pose in left view.");
//	feedback.SetWindowText(a);
}

void FilterCutDialog::OnSavePoseRight(){
	savedPose2 = curPoseIndex;
	lastSaved = 2;
	setSavedPose2ToDraw();

	CString aCString(_T("A string"));
	//aCStringa.Format(_T("%s"), "Saved pose in right view.");
	//	feedback.SetWindowText(aCString);
}

void FilterCutDialog::OnLeftPose()
{
	if (savedPose1 > -1 && savedPose1 < numTotalPoses){
		savePoseToNextStep(savedPose1);
	}
}

void FilterCutDialog::OnRightPose()
{
	if (savedPose2 > -1 && savedPose2 < numTotalPoses){
		savePoseToNextStep(savedPose2);
	}
}

void FilterCutDialog::setSavedPose1ToDraw(){
	doc->updateSavedPose1ToDraw(curPoseIndex);
}

void FilterCutDialog::setSavedPose2ToDraw(){
	doc->updateSavedPose2ToDraw(curPoseIndex);
}

void FilterCutDialog::savePoseToNextStep(int chosenPose){
	doc->savePoseToNextStep(chosenPose);
}






/*
void FilterCutDialog::OnCbnSelchangeJoin()
{
	int idx = joinComboBox.GetCurSel();
	int typeIdx = (int)chosenPose[idx] + 1;

	JoinTypeComboBox.SetCurSel(typeIdx);
}

void FilterCutDialog::OnCbnSelchangeJoinType()
{
	int idx = joinComboBox.GetCurSel();
	neighborPos cpose = (neighborPos)(JoinTypeComboBox.GetCurSel() - 1);

	chosenPose[idx] = cpose;
}

void FilterCutDialog::OnBnClickedButton1()
{
	// Update filter
	doc->updateFilterCutGroup();

	// Print out
	cout << "Update filter group: " << endl;
	for (int i = 0; i < chosenPose.size(); i++)
	{
		CString nbText;
		joinComboBox.GetLBText(i, nbText);

		int poseIdx = (int)chosenPose[i] + 1;
		CString poseText;
		JoinTypeComboBox.GetLBText(poseIdx, poseText);

		wcout << " - " << nbText.GetBuffer() << ": " << poseText.GetBuffer() << endl;
	}
	cout << "-------------------------" << endl;
}

void FilterCutDialog::OnBnClickedClearFilter()
{
	for (auto & p : chosenPose){
		p = NONE_NB;
	}

	joinComboBox.SetCurSel(0);
	OnCbnSelchangeJoin();
}
*/