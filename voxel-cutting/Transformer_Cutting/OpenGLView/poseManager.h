#pragma once
#include "stdafx.h"
#include "cutTreeUtil.h"
#include "neighbor.h"
#include "BoneMapTree.h"

extern class groupCutNode;

typedef enum
{
	NONE_NB = -1,
	X_PLUS = 0,
	X_MINUS = 1,
	Y_PLUS = 2,
	Y_MINUS = 3,
	Z_PLUS = 4,
	Z_MINUS = 5,
	NUM_POS
} neighborPos;

class neighborPose
{
public:
	neighborPose();
	~neighborPose();

	int smallestErrorIdx;

	// Evaluations on how well fitted the pose is
	float poseScore;
	float smallestVolumeError;
	float smallestCBError;
	int hashRank;
	std::vector<float> estimatedCBLengths;
	void calculatePoseScore(Vec3f weights);
	void calculateCBScores();

	bool operator == (const neighborPose& b);
	bool operator < (const neighborPose& b);

	void computeUniqeID();
	bool containFilter(std::vector<neighborPos> pp) const;

	int posConfigId; // Computed from posConfig
	std::vector<neighborPos> posConfig;

	std::vector<cutTreefNode*> nodes; // Node that satisfy
	std::vector<std::map<int, int>> mapBone_meshIdx;

	// for cutting group node
	std::vector<groupCutNode*> nodeGroupBoneCut;
	void sortNodesInGroupCut();
	void carryOverFirstPos();
};

struct comparePoseScore{
	bool operator()(neighborPose const* a, neighborPose const* b){
		return (a->poseScore < b->poseScore);
	}
};

/*
struct comparePoseNodeScore{
	bool operator()(neighborPose const* a, neighborPose const* b){
		return (a->nodeGroupBoneCut[0]->nodeScore < b->nodeGroupBoneCut[0]->nodeScore);
	}
};*/

class poseManager
{
public:
	poseManager(void);
	~poseManager(void);

	void init();
	void addPose(cutTreefNode * node);
	void constructMapTree();

	std::map<int, neighborPose> poseMap; 
	// Weights that provide weighted score
	float weights[3];

	neighborPose getPose(int poseIdx);

	std::vector<neighborPose*> filteredPose;

	// Test here
	std::vector<neighborPose*> allPoses;
	void getAllPosesIntoVectorForm();

	BoneMapTree m_boneMapTree;
	skeleton *s_skeleton;

	// Bone info. One array
	std::vector<bone*> sortedBone;
	std::vector<arrayInt> boneAroundBone;
	std::vector<Vec2i> neighborPair;// parent to child

	// Bone info. separate center and side
	std::vector<boneAbstract> centerBoneOrder;
	std::vector<boneAbstract> sideBoneOrder;
	std::vector<neighbor> neighborInfo;

	// Use CENTER_BONE and SIDE_BONE for index
	// Store number of neighbor of mesh box
	// Vec2i -> center neighbor and side neighbor
	// 
	std::vector<Vec2i> neighborCenterNum;
	std::vector<Vec2i> neighborSideNum;
	std::vector<Vec2i> neighborBoneNum;

	float voxelSizef;

	// 1.
	neighborPos possibleNeighbor(meshPiece* parent, meshPiece* child);
	neighborPos *posArray();
	// 2.
	std::vector<std::vector<indexBone>> findPossibleNeighbor(cutTreefNode * node);
	// 3. Map
	std::vector<std::vector<indexBone>> posibleMeshNeighbor;
	// one array mesh box
	std::vector<Vec2i> neighborMeshNum;
	std::vector<meshPiece> meshBoxes;
	void findPossibleMap(BoneMapTreeNode *node, cutTreefNode* cutTNode);

	void updateFilteredList(std::vector<neighborPos> pp);

	int findIdx(std::vector<bone*>* v, bone* e);
	bool isNeighborSufficient(int boneIdx, int meshIdx);
	void countMeshNeighbor();

	float evaluateError(int poseIdx, int nodeIdx);
	void computeVolumeRatioOfBone();

	float getVolumeError(cutTreefNode* cutTNode, std::map<int, int>* boneMeshIdxMap);
	neighborPose getFilteredPose(int idx1);
	void sortAccordingToWeights(Vec3f weights);
	void calculateRankScoreByHash(std::vector<int> idealHashes);
	void calculateCBLengthErrors();
};

class poseGroupCutManager
{
public:
	poseGroupCutManager();
	~poseGroupCutManager();

	void init();
	 
	void parserConfigure(groupCutNode * node); 
	void possibleMap(BoneMapTreeNode * boneNode, groupCutNode * meshNode);

	neighborPose* getPoseByIdx(int idx);
	void getPoseMapIntoVectorForm();

	std::map<int, neighborPose> poseMap; 
	std::vector<neighborPose*> sortedPoseMap;
	std::vector<bone*> *boneArray;
	std::vector<Vec2i> neighborInfo; // parent, child	
	std::vector<arrayInt> boneAroundBone;
	float voxelSizef;

	BoneMapTree m_boneMapTree;

private:
	neighborPos possibleNeighbor(meshPiece* parent, meshPiece* child);
	neighborPos *posArray();
	float getVolumeError(groupCutNode * node, std::map<int, int>* boneMeshIdxMap);
	void getBoneNeighbor(std::vector<arrayInt> &boneAroundB, std::vector<bone*> * boneList);
	int findIdx(std::vector<bone*> * boneList, bone* b);
};