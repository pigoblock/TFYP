#pragma once
#include "stdafx.h"
#include "bvhVoxel.h"
#include "poseManager.h"

class groupCutNode
{
public:
	groupCutNode();
	~groupCutNode();

	groupCutNode(groupCutNode *parentIn);
	void draw(int mode);
	void draw(std::vector<bone*> bones, std::map<int, int> boneMeshmap);
	void drawNeighbor(std::map<int, int> boneMeshmap, arrayVec2i neighborInfo, float voxelSize);

	// Evaluations
	float volError;
	float hashRank;
	float CBError;
	int posID;
	int carryover;
	std::vector<float> estimatedCBLengths;
	float nodeScore;

	void calculateVolError(std::vector<bone*> bones, std::map<int, int> boneMeshmap);
	void calculateCBError(std::vector<bone*> bones, std::map<int, int> boneMeshmap, arrayVec2i neighborInfo);
	void calculatePosID(std::map<int, int> boneMeshmap, arrayVec2i neighborInfo, std::vector<int> idealHashes);
	void calculateNodeScore(Vec3f weights);

	// Tree data
	std::vector<groupCutNode*> child;
	groupCutNode* parent;
	int depth;

	std::vector<meshPiece> boxf;

	// Voxel inside and bounding box
	Vec3f leftDown; 
	Vec3f rightUp;
	arrayInt voxIdxs;
};

struct compareNodeScore{
	bool operator()(groupCutNode const* a, groupCutNode const* b){
		return (a->nodeScore < b->nodeScore);
	}
};

class groupCut
{
public:
	groupCut();
	~groupCut();

	void constructTree();
	void drawLeaveIdx(int idx);
	void drawPose(int poseIdx, int configIdx);

private:
	void constructTreeRecur(groupCutNode * node);
	bool cutBox(groupCutNode * newNode, int boxIdx, int xyzd, float coord);
	bool computeError(groupCutNode * node);

public:
	arrayInt voxelIdxs;
	std::vector<bone*> bones; // Should be sorted by order of size
	bvhVoxel *sourcePiece;
	std::vector<voxelBox> *boxes;
	std::vector<arrayInt> *neighborVoxel;
	float voxelSize;

	groupCutNode *m_root;
	std::vector<groupCutNode*> leaves;
	poseGroupCutManager boxPose; // We use the pose map only

	// Evaluations
	void sortEvaluations(Vec3f weights);
	void calculateNodeErrors(int poseIdx, int configIdx, std::vector<int> idealHashes);
private:
	// Temporary variable for cutting 
	int *mark;
	int *voxelOccupy;
};


