#pragma once
#include "Resource.h"
#include "afxwin.h"

// FilterCutDialog dialog
extern class MainControl;

class FilterCutDialog : public CDialogEx
{
	DECLARE_DYNAMIC(FilterCutDialog)

public:
	FilterCutDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~FilterCutDialog();

	void init(int poseSize);

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
	CEdit weightVolume;
	CEdit weightHash;
	CEdit weightCB;
	CEdit overallError;
	CEdit volumeError;
	CEdit hashError;
	CEdit CBError;

	int numTotalPoses;
	int curPoseIndex;
	int savedPose1;
	int savedPose2;
	int lastSaved;
	bool needsUpdate;
	float weights[3];

	MainControl * doc;

public:
	afx_msg void OnPreviousPose();
	afx_msg void OnNextPose();
	afx_msg void OnSavePoseLeft();
	afx_msg void OnSavePoseRight();
	afx_msg void OnLeftPose();
	afx_msg void OnRightPose();
	afx_msg void OnSort();

	void setPoseToDraw();
	void setSavedPose1ToDraw();
	void setSavedPose2ToDraw();
	void savePoseToNextStep(int chosenPose);

	void updateDisplayedOverallError(float value);
	void updateDisplayedVolumeError(float value);
	void updateDisplayedHashError(float value);
	void updateDisplayedCBError(float value);
};
