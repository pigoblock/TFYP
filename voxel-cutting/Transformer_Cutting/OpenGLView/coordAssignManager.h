#pragma once
#include "coordAsignDlg.h"
#include "detailSwapManager.h"


class coordAssignManager
{
public:
	coordAssignManager(void);
	~coordAssignManager(void);
	
	void assignCoord(detailSwapManager* detailSwap, CWnd* parent);
	void init(std::vector<bone*> boneFullArray, std::vector<bvhVoxel> meshBoxFull);

	void drawBoxes();
	void draw2(BOOL mode[10]);

	void draw(BOOL mode[10]);
	void drawBoneMap();

public:
	detailSwapManager *s_detailSwap;
	coordAsignDlg *dlg;

	std::vector<bone*> m_boneFullArray;
	std::vector<bvhVoxel> m_meshBoxFull;
	std::vector<Vec3f> leftDowns;
	std::vector<Vec3f> rightUps;
};

