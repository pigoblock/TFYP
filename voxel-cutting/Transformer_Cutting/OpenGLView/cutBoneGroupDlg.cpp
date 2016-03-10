#include "stdafx.h"
#include "cutBoneGroupDlg.h"
#include "afxdialogex.h"
#include "groupCutManager.h"


IMPLEMENT_DYNAMIC(cutBoneGroupDlg, CDialogEx)

cutBoneGroupDlg::cutBoneGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(cutBoneGroupDlg::IDD, pParent)
{

}

cutBoneGroupDlg::~cutBoneGroupDlg()
{
}

void cutBoneGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, boneGoupListBox);
	DDX_Control(pDX, IDC_EDIT1, poseCurIdxText);
	DDX_Control(pDX, IDC_EDIT2, PoseTotalText);
	DDX_Control(pDX, IDC_EDIT3, curIdxInPoseText);
	DDX_Control(pDX, IDC_EDIT4, totalIdxInPoseText);

	DDX_Control(pDX, IDC_EDIT6, weightVolume);
	DDX_Control(pDX, IDC_EDIT8, weightHash);
	DDX_Control(pDX, IDC_EDIT5, weightCB);

	DDX_Control(pDX, IDC_EDIT9, volumeError);
	DDX_Control(pDX, IDC_EDIT10, CBError);
	DDX_Control(pDX, IDC_EDIT11, hashError);
	DDX_Control(pDX, IDC_EDIT12, overallError);
}


BEGIN_MESSAGE_MAP(cutBoneGroupDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, &cutBoneGroupDlg::OnCbnSelchangeBoneGoup)
	ON_BN_CLICKED(IDC_BUTTON1, &cutBoneGroupDlg::previousPose)
	ON_BN_CLICKED(IDC_BUTTON2, &cutBoneGroupDlg::nextPoseClick)
	ON_BN_CLICKED(IDC_BUTTON3, &cutBoneGroupDlg::previousConfigureClick)
	ON_BN_CLICKED(IDC_BUTTON4, &cutBoneGroupDlg::NextCongifureClick)
	ON_BN_CLICKED(IDC_BUTTON5, &cutBoneGroupDlg::AcceptClick)
	ON_BN_CLICKED(IDC_BUTTON7, &cutBoneGroupDlg::OnSort)
	ON_BN_CLICKED(IDOK, &cutBoneGroupDlg::OnBnClickedOk)
END_MESSAGE_MAP()

void cutBoneGroupDlg::OnCbnSelchangeBoneGoup()
{
	changeBoneSlect(boneGoupListBox.GetCurSel());
}

void cutBoneGroupDlg::previousPose()
{
	CString stext;
	poseCurIdxText.GetWindowText(stext);
	int poseIndex = _ttoi(stext) - 1;

	if (setPoseSelection(poseIndex)){
		currentPoseIndex = poseIndex;
	}
}

void cutBoneGroupDlg::nextPoseClick()
{
	CString stext;
	poseCurIdxText.GetWindowText(stext);
	int poseIndex = _ttoi(stext) + 1;

	if (setPoseSelection(poseIndex)){
		currentPoseIndex = poseIndex;
	}
}

void cutBoneGroupDlg::previousConfigureClick()
{
	CString stext;
	curIdxInPoseText.GetWindowText(stext);
	int curIdx = StrToInt(stext) - 1;

	if (setselectIdxInPose(curIdx)){
		currentConfigIndex = curIdx;
		stext.Format(_T("%d"), currentConfigIndex);
		curIdxInPoseText.SetWindowText(stext);
	}
}

void cutBoneGroupDlg::NextCongifureClick()
{
	CString stext;
	curIdxInPoseText.GetWindowText(stext);
	int curIdx = StrToInt(stext) + 1;
	
	if (setselectIdxInPose(curIdx)){
		currentConfigIndex = curIdx;
		stext.Format(_T("%d"), currentConfigIndex);
		curIdxInPoseText.SetWindowText(stext);
	}
}

void cutBoneGroupDlg::updateDisplay(){
	CString stext;
	poseCurIdxText.GetWindowText(stext);
	int curPoseIdx = _ttoi(stext);

	curIdxInPoseText.GetWindowText(stext);
	int curIdxInPose = StrToInt(stext);

	int boneIdx = boneGoupListBox.GetCurSel();
}

void cutBoneGroupDlg::AcceptClick()
{
	CString stext;
	poseCurIdxText.GetWindowText(stext);
	int curPoseIdx = _ttoi(stext);

	curIdxInPoseText.GetWindowText(stext);
	int curIdxInPose = StrToInt(stext);

	int boneIdx = boneGoupListBox.GetCurSel();

	idxChoosen[boneIdx] = Vec2i(curPoseIdx, curIdxInPose);
	groupCutMngr->updateAcceptedIndexes(idxChoosen);
}

void cutBoneGroupDlg::Init(groupCutManager* _groupCutMngr)
{
	groupCutMngr = _groupCutMngr;
	std::vector<groupCut> *groupBoneArray = &groupCutMngr->boneGroupArray;

	for (int i = 0; i < groupBoneArray->size(); i++){
		idxChoosen.push_back(Vec2i(-1, -1));
		boneGoupListBox.AddString(groupBoneArray->at(i).sourcePiece->boneName);
	}

	boneGoupListBox.ResetContent();
	for (int i = 0; i < groupBoneArray->size(); i++){
		CString name = groupBoneArray->at(i).sourcePiece->boneName;
		boneGoupListBox.AddString(name);
	}

	boneGoupListBox.SetCurSel(0);
	changeBoneSlect(0);

	CString a;
	a.Format(_T("%d"), 1);
	weightVolume.SetWindowText(a);
	a.Format(_T("%d"), 0);
	weightHash.SetWindowText(a);
	weightCB.SetWindowText(a);

	currentPoseIndex = 0;
	currentConfigIndex = 0;
}

void cutBoneGroupDlg::changeBoneSlect(int boneIdx)
{
	// Refresh list box
	std::vector<groupCut> *groupBoneArray = &groupCutMngr->boneGroupArray;
	groupCut *curG = &groupBoneArray->at(boneIdx);
	Vec2i selected = idxChoosen[boneIdx];

	// pose
	CString a; a.Format(_T("%d"), curG->boxPose.poseMap.size()-1);
	PoseTotalText.SetWindowText(a);

	int selectPoseIdx = selected[0] == -1 ? 0 : selected[0];
	a.Format(_T("%d"), selectPoseIdx);
	poseCurIdxText.SetWindowText(a);

	// configuration
	int configInPoseIdx = selected[1] == -1 ? 0 : selected[1];

	neighborPose* curP = curG->boxPose.sortedPoseMap.at(selectPoseIdx);
	a.Format(_T("%d"), curP->nodeGroupBoneCut.size()-1);
	totalIdxInPoseText.SetWindowText(a);

	a.Format(_T("%d"), configInPoseIdx);
	curIdxInPoseText.SetWindowText(a);

	// Update index
	groupCutMngr->curBoneIdx = boneIdx;
	groupCutMngr->updatePoseConfigurationIdx(selectPoseIdx, configInPoseIdx);
}

bool cutBoneGroupDlg::setPoseSelection(int poseIdx)
{
	std::vector<groupCut> *groupBoneArray = &groupCutMngr->boneGroupArray;

	groupCut *curG = &groupBoneArray->at(boneGoupListBox.GetCurSel());

	int configIndex = groupCutMngr->updatePoseConfigurationIdx(poseIdx, -1);

	if (configIndex == -1){
		return false;
	}
	else {
		currentConfigIndex = configIndex;
	}

	neighborPose* pp = curG->boxPose.sortedPoseMap.at(poseIdx);

	// Update text
	CString a; 
	a.Format(_T("%d"), poseIdx);
	poseCurIdxText.SetWindowText(a);

	a.Format(_T("%d"), currentConfigIndex);
	curIdxInPoseText.SetWindowText(a);

	a.Format(_T("%d"), pp->nodeGroupBoneCut.size());
	totalIdxInPoseText.SetWindowTextW(a);

	return true;
}

bool cutBoneGroupDlg::setselectIdxInPose(int nodeIdxInPose)
{
	std::vector<groupCut> *groupBoneArray = &groupCutMngr->boneGroupArray;
	int boneIdxIdx = boneGoupListBox.GetCurSel();
	groupCut *curG = &groupBoneArray->at(boneIdxIdx);

	neighborPose* curP = curG->boxPose.sortedPoseMap.at(currentPoseIndex);

	if (nodeIdxInPose < 0 || nodeIdxInPose >= curP->nodeGroupBoneCut.size()){
		return false;
	}
	else {
		currentConfigIndex = nodeIdxInPose;
	}

	int stubReceiveNodeIdx = groupCutMngr->updatePoseConfigurationIdx(currentPoseIndex, nodeIdxInPose);

	return true;
}

void cutBoneGroupDlg::OnBnClickedOk()
{
	// Check coord first
	for (auto i : idxChoosen){
		if (i[0] == -1 || i[1] == -1){
			AfxMessageBox(_T("Not all groups are split"));
			return;
		}
	}

	groupCutMngr->updateAcceptedIndexes(idxChoosen);
	
	CDialogEx::OnOK();
}

void cutBoneGroupDlg::OnSort(){
	// Recalculate weight ratios and scores
	CString csText;

	GetDlgItemText(IDC_EDIT6, csText);
	weights[0] = _tstof((LPCTSTR)csText);
	GetDlgItemText(IDC_EDIT8, csText);
	weights[1] = _tstof((LPCTSTR)csText);
	GetDlgItemText(IDC_EDIT5, csText);
	weights[2] = _tstof((LPCTSTR)csText);

	groupCutMngr->updateSortEvaluations();
	groupCutMngr->updatePoseConfigurationIdx(currentPoseIndex, currentConfigIndex);
}

void cutBoneGroupDlg::updateDisplayedOverallError(float value){
	CString a;
	a.Format(_T("%.2f"), value);
	overallError.SetWindowText(a);
}

void cutBoneGroupDlg::updateDisplayedVolumeError(float value){
	CString a;
	a.Format(_T("%.2f"), value);
	volumeError.SetWindowText(a);
}

void cutBoneGroupDlg::updateDisplayedHashError(float value){
	CString a;
	a.Format(_T("%.2f"), value);
	hashError.SetWindowText(a);
}

void cutBoneGroupDlg::updateDisplayedCBError(float value){
	CString a;
	a.Format(_T("%.2f"), value);
	CBError.SetWindowText(a);
}
