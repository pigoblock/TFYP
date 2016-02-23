#pragma once
#include "stdafx.h"
#include "Voxel.h"
#include "skeleton.h"
#include "boneAbstract.h"
#include "cutTree.h"
#include "neighbor.h"
#include "octreeSolid.h"
#include "cutTreef.h"
#include "boneTransform.h"
#include "voxelObject.h"
#include "poseManager.h"
#include "SideDialog.h"

class cutSurfTreeMngr2
{
public:
	cutSurfTreeMngr2(void);
	~cutSurfTreeMngr2(void);

	void drawLeaf();

	void updateDisplay(int idx1, int idx2);
	int updateBestIdx(int idx1);

	void updateDisplayFilter(int idx1, int idx2);
	int updateBestIdxFilter(int idx1);

	void showWeightInputDialog();	// coded but not implemented
	int findBestOption(int yIdx);

	//Export 
	std::vector<meshCutRough> convertBoxesToMeshCut(std::vector<Boxi> centerBoxi);

	void exportMesh();
	void drawLocalCoord(cutTreefNode * node);

	// Core functions to set up tree
	void init();
	void parserSkeletonGroup();
	void constructCutTree();

	void filterPose(std::vector<neighborPos> pp);
	void getListOfBestPoses();

private:
	void setVoxelArray();
	void drawNeighborRelation();

public:
	// Group bone
	// Share data from main document
	skeleton *s_groupSkeleton;
	voxelObject *s_voxelObj;
	std::vector<voxelBox>* s_boxes;

	// Data load from file
	VoxelObj m_voxelHighRes;

	octreeSolid m_octree;

	// Different poses
	poseManager poseMngr;

	// Hash voxel
	std::vector<voxelBox> boxes; // all voxel pixel box
	hashVoxel hashTable;

	bool bUniformCut;
	cutTreef m_tree2;
	cutTreefNode *leatE2Node2;

	// variables
	std::vector<boneAbstract> m_boneOrder;
	std::vector<std::pair<int,int>> neighborPair;

	// With symmetric
	std::vector<boneAbstract> m_centerBoneOrder;
	std::vector<boneAbstract> m_sideBoneOrder;
	std::vector<neighbor> neighborInfo;

	int m_roughScaleStep;	// Step for rough estimation
	float octreeSizef;

	// tree
	cutTree m_tree;
	cutTreeNode *leatE2Node;

	cutTreefNode* curNode;

	// Debug
	int poseIdx, nodeIdx; // Index in pose array
	std::vector<boneTransform2> coords;
	std::vector<CString> names;
	std::vector<Vec3f> centerPos;
	arrayVec2i meshNeighbor;
	neighborPose currentPose;
	std::vector<meshPiece> allMeshes;
	int upParentIdx;

	// User define weight error
	Vec3f m_weightError; // neighbor - aspect - volume

	SideDialog *m_dlg;
	Vec3f m_weights;
	void connectWithSideDialog(SideDialog *sd);
	void calculateSortingRequirements(std::vector<int> idealHashes);
	void updateSortEvaluations();
};