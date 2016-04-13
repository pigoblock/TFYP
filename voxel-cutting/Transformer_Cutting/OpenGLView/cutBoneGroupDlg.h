#pragma once
#include "Resource.h"
#include "afxwin.h"
#include "DataTypes\vec.h"

extern class groupCutManager;
extern class MainControl;
// cutBoneGroupDlg dialog

class cutBoneGroupDlg : public CDialogEx
{
	DECLARE_DYNAMIC(cutBoneGroupDlg)

public:
	cutBoneGroupDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~cutBoneGroupDlg();

	MainControl * doc;

	void Init(groupCutManager* _groupCutMngr);

// Dialog Data
	enum { IDD = IDD_DIALOG_GROUP_CUT };

private:
	groupCutManager* groupCutMngr;
	std::vector<Vec2i> idxChoosen;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox boneGoupListBox; // List of bone
	CEdit poseCurIdxText;
	CEdit PoseTotalText;
	CEdit curIdxInPoseText;
	CEdit totalIdxInPoseText;
	CEdit feedback;
	CEdit weightVolume;
	CEdit weightHash;
	CEdit weightCB;
	CEdit overallError;
	CEdit volumeError;
	CEdit hashError;
	CEdit CBError;

	float weights[3];
	int currentPoseIndex;
	int currentConfigIndex;

	afx_msg void OnCbnSelchangeBoneGoup();
	afx_msg void previousPose();
	afx_msg void nextPoseClick();
	afx_msg void previousConfigureClick();
	afx_msg void NextCongifureClick();
	afx_msg void AcceptClick();
	afx_msg void OnSort();

	void changeBoneSlect(int boneIdx);
	bool setPoseSelection(int curIdx);
	bool setselectIdxInPose(int nodeIdxInPose);
	afx_msg void OnBnClickedOk();

	void updateDisplay();
	void updateDisplayedOverallError(float value);
	void updateDisplayedVolumeError(float value);
	void updateDisplayedHashError(float value);
	void updateDisplayedCBError(float value);
};
