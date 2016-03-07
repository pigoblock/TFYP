#pragma once
#include "Resource.h"
#include "afxwin.h"
#include "cutSurfTreeMngr2.h"

// FilterCutDialog dialog
extern class MainControl;

class FilterCutDialog : public CDialogEx
{
	DECLARE_DYNAMIC(FilterCutDialog)

public:
	FilterCutDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~FilterCutDialog();

	void initFromCutTree(cutSurfTreeMngr2 *cutMangr);

// Dialog Data
	enum { IDD = IDD_DIALOG_FILTER_CUT_GROUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox joinComboBox;
	CComboBox JoinTypeComboBox;

	CEdit numPoses;
	CEdit currentPose;
	CEdit feedback;
	int numTotalPoses;
	int curPoseIndex;
	int savedPose1;
	int savedPose2;
	int lastSaved;

	MainControl * doc;

	std::vector<neighborPos> chosenPose;

public:
//	afx_msg void OnCbnSelchangeJoin();
//	afx_msg void OnCbnSelchangeJoinType();
//	afx_msg void OnBnClickedClearFilter();
//	afx_msg void OnBnClickedButton1(); // Update

	afx_msg void OnPreviousPose();
	afx_msg void OnNextPose();
	afx_msg void OnSavePoseLeft();
	afx_msg void OnSavePoseRight();
	afx_msg void OnLeftPose();
	afx_msg void OnRightPose();

	void setPoseToDraw();
	void setSavedPose1ToDraw();
	void setSavedPose2ToDraw();
	void savePoseToNextStep(int chosenPose);
};
