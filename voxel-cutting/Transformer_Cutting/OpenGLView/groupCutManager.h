#pragma once
#include "groupCut.h"
#include "octreeSolid.h"
#include "Graphics\Surfaceobj.h"
#include "skeleton.h"
#include "detailSwapManager.h"
#include "voxelObject.h"
#include "cutBoneGroupDlg.h"
#include "SideDialog.h"

typedef std::shared_ptr<octreeSolid> octreeSolidPtr;

class groupCutManager
{
public:
	groupCutManager();
	~groupCutManager();

	// Draw
	void draw();
	void showDialog(CWnd* parent = nullptr);

	// Initialization
	void initFromSwapBox(detailSwapManager * m_swapMngr);
	void loadMeshBox(char *filePath);

	void updateDisplay(int yIdx, int zIdx);
	int updateToPoseIdx(int selectPoseIdx);

	// What we need from previous step
	// Can be loaded from file or from detail swap manager
	// bvhVoxel, voxel boxes, neighbor voxel
	octreeSolid *m_octree;

	Vec3f leftDownVoxel, rightUpVoxel;
	float voxelSize;
	Vec3i NumXYZ;
	hashVoxel hashTable;
	std::vector<voxelBox> boxes;
	std::vector<arrayInt> neighborVoxel;

	SurfaceObj *m_surObj;
	skeleton m_skeleton; // Full skeleton
	std::vector<bvhVoxel> meshBoxes;
	SideDialog *sideDialog;

	// Total process
	skeleton *s_skeleton;
	hashVoxel *s_hashTable;
	std::vector<bvhVoxel*> *s_meshBoxes;
	SurfaceObj *s_surObj;
	std::vector<voxelBox> *s_boxes;
	std::vector<arrayInt> *s_boxShareFaceWithBox;
	octreeSolid *s_octree;

	std::vector<groupCut> boneGroupArray;
	cutBoneGroupDlg *m_dlg;
	std::vector<Vec2i> m_idxChoosen; // idx of configuration chosen by user

	void constructVolxeHash();
	void loadVoxelArray();

	void updateAndChangeMode(std::vector<Vec2i> idxChoosen);
	void getConfiguration(int boneGroupIdx, std::vector<bone*>& boneInGroup, std::vector<meshPiece>& cutBoxByBoneOrder);
	
	void updateRealTime();
	void computeVolumeRatioInGroup();

	void performEvaluations();
	
	// draw
	int curBoneIdx;
	int idx1, idx2;
};