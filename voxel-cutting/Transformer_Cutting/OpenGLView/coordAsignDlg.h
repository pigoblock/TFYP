#pragma once
#include "afxwin.h"
#include "DataTypes\define.h"
#include "Resource.h"
#include "detailSwapManager.h"

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2
//#define X_AXIS_FLIPPED 0
#define Y_AXIS_FLIPPED -1
#define Z_AXIS_FLIPPED -2

extern class MainControl;

class coordAsignDlg : public CDialog
{
public:
	MainControl *doc;
	detailSwapManager *s_detailSwap;
	std::vector<bone*> *s_boneFullArray;
	std::vector<bvhVoxel> *s_meshBoxFull;

	arrayVec3i coords;
	//map x-y-z bone on world coord
	// 0 (x) of bone is coords[0] of world
	// mesh [bone]

	void init(detailSwapManager* detailSwap);
	void init(std::vector<bone*> *boneFullArray, std::vector<bvhVoxel> *meshBoxFull);
	int getCurBoneIdx();

	DECLARE_DYNAMIC(coordAsignDlg)

	coordAsignDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~coordAsignDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ASSIGN_COORD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox boneComboBox;
	CComboBox comboMapX;
	CComboBox comboMapY;
	CStatic mapZ;
	afx_msg void OnBnClickedButtonUpdateCoord();
	afx_msg void OnBnClickedButtonFinish();
	afx_msg void OnCbnSelchangeComboBoneName();
	afx_msg void OnCbnSelchangeComboMapX();
	afx_msg void OnCbnSelchangeComboMapY();

public:
	void setCurBoneSlection(int boneIdx);
	void updateComboBoxText();
	afx_msg void OnBnClickedButtonSaveCoord();
	afx_msg void OnBnClickedButton2LoadCoord();
	afx_msg void AutoAssign();

private:
	float getMax(float a, float b);
};
