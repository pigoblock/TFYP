#include "stdafx.h"
#include "groupCut.h"
#include "errorCompute.h"
#include <iostream>

groupCutNode::groupCutNode()
{
	parent = nullptr;
	depth = 0;
}

groupCutNode::groupCutNode(groupCutNode *parentIn)
	:groupCutNode()
{
	parent = parentIn;
	depth = parentIn->depth + 1;
	boxf = parentIn->boxf;
}

groupCutNode::~groupCutNode()
{
	for (int i = 0; i < child.size(); i++)
	{
		delete child[i];
	}
	child.clear();
}

void groupCutNode::draw(int mode)
{
	static arrayVec3f color = Util_w::randColor(6);
	if (mode == 0)
	{
		// Draw wire frame
		for (int i = 0; i < boxf.size(); i++)
		{
			glColor3fv(color[i].data());
			Util_w::drawBoxWireFrame(boxf[i].leftDown, boxf[i].rightUp);
		}
	}
}

void groupCutNode::draw(std::vector<bone*> bones, std::map<int, int> boneMeshmap)
{
	static arrayVec3f color_1 = Util_w::randColor(6);
	
	for (int i = 0; i < bones.size(); i++){
		int meshIdx = boneMeshmap[i];
		glColor3fv(color_1[i].data());
		Util_w::drawBoxWireFrame(boxf[meshIdx].leftDown, boxf[meshIdx].rightUp);

		// draw name
		Vec3f center = (boxf[meshIdx].leftDown + boxf[meshIdx].rightUp) / 2;
		Util::printw(center[0], center[1], center[2], "%s", CStringA(bones[i]->m_name.GetBuffer()));
	}
}

void groupCutNode::drawNeighbor(std::map<int, int> boneMeshmap, 
	arrayVec2i neighborInfo, float voxelSize)
{
	for (int i = 0; i < neighborInfo.size(); i++){
		int meshIdx1 = boneMeshmap[neighborInfo[i][0]];
		int meshIdx2 = boneMeshmap[neighborInfo[i][1]];

		Vec3f firstEnd = boxf[meshIdx1].estimatedEnd;
		Vec3f secondStart = boxf[meshIdx2].estimatedOrigin;

		glLineWidth(3.0);
		glBegin(GL_LINES);
			glVertex3f(firstEnd[0], firstEnd[1], firstEnd[2]);
			glVertex3f(secondStart[0], secondStart[1], secondStart[2]);
		glEnd();
		glLineWidth(1.0);

		float radius = voxelSize / 5;
		Util_w::drawSphere(firstEnd, radius);
		Util_w::drawSphere(secondStart, radius);		
	}
}

void groupCutNode::calculateVolError(std::vector<bone*> bones, std::map<int, int> boneMeshmap){
	volError = 0;

	for (int i = 0; i < bones.size(); i++){
		int meshIdx = boneMeshmap[i];
		volError += errorCompute::normE2(bones[i]->volumeRatio(), boxf[meshIdx].volumeRatio);
	}
}

void groupCutNode::calculateCBError(std::vector<bone*> bones, std::map<int, int> boneMeshmap, arrayVec2i neighborInfo){
	// Calculate estimated start and end joints
	for (int i = 0; i < bones.size(); i++){
		Vec3f boneBoxSize = bones.at(i)->m_sizef;
		Vec3i SMLIdxBone = Util_w::SMLIndexSizeOrder(boneBoxSize);

		int meshIdx = boneMeshmap[i];
		Vec3f cutBoxSize = boxf[meshIdx].rightUp - boxf[meshIdx].leftDown;
		Vec3i SMLIdxCutBox = Util_w::SMLIndexSizeOrder(cutBoxSize);

		// Refactor these
		Vec3i mapCoord;
		if (bones.at(i)->m_type == TYPE_CENTER_BONE){
			// Need to check for symmetry
			// Error between mesh and bone in 012
			float score012y = abs(cutBoxSize[1] / boneBoxSize[1] - 1);
			float score012z = abs(cutBoxSize[2] / boneBoxSize[2] - 1);

			// Error between mesh and bone in 021
			float score021y = abs(cutBoxSize[2] / boneBoxSize[1] - 1);
			float score021z = abs(cutBoxSize[1] / boneBoxSize[2] - 1);

			if (score012y <= score021y && score012z <= score021z){
				mapCoord = Vec3f(0, 1, 2);
			}
			else if (score021y <= score012y && score021z <= score012z){
				mapCoord = Vec3f(0, 2, 1);
			}
			else {
				// Reduce max error instead of reducing min error
				if (std::max(score012y, score012z) <= std::max(score021y, score021z)){
					mapCoord = Vec3f(0, 1, 2);
				} else{
					mapCoord = Vec3f(0, 2, 1);
				}
			}
		}
		else {
			Vec3i orderInMesh; // 0: smallest; 1: medium; 2: largest
			for (int j = 0; j < 3; j++){
				orderInMesh[SMLIdxCutBox[j]] = j;
			}

			for (int k = 0; k < 3; k++){
				mapCoord[orderInMesh[k]] = SMLIdxBone[k];
			}
		}

		Vec3f origin = (boxf[meshIdx].leftDown + boxf[meshIdx].rightUp) / 2.0;
		Vec3f end = origin;
		if (mapCoord == Vec3f(0, 1, 2)){
			origin -= Vec3f(0, 0, cutBoxSize[2] / 2.0);
			end += Vec3f(0, 0, cutBoxSize[2] / 2.0);
		}
		else if (mapCoord == Vec3f(0, 2, 1)){
			origin -= Vec3f(0, cutBoxSize[1] / 2.0, 0);
			end += Vec3f(0, cutBoxSize[1] / 2.0, 0);
		}
		else if (mapCoord == Vec3f(1, 0, 2)){
			origin -= Vec3f(0, 0, cutBoxSize[2] / 2.0);
			end += Vec3f(0, 0, cutBoxSize[2] / 2.0);
		}
		else if (mapCoord == Vec3f(1, 2, 0)){
			origin -= Vec3f(0, cutBoxSize[1] / 2.0, 0);
			end += Vec3f(0, cutBoxSize[1] / 2.0, 0);
		}
		else if (mapCoord == Vec3f(2, 0, 1)){
			origin -= Vec3f(cutBoxSize[0] / 2.0, 0, 0);
			end += Vec3f(cutBoxSize[0] / 2.0, 0, 0);
		}
		else if (mapCoord == Vec3f(2, 1, 0)){
			origin -= Vec3f(cutBoxSize[0] / 2.0, 0, 0);
			end += Vec3f(cutBoxSize[0] / 2.0, 0, 0);
		}
		boxf[meshIdx].estimatedOrigin = origin;
		boxf[meshIdx].estimatedEnd = end;
	}
	
	for (int i = 0; i < neighborInfo.size(); i++){
		int meshIdx1 = boneMeshmap[neighborInfo[i][0]];
		int meshIdx2 = boneMeshmap[neighborInfo[i][1]];

		Vec3f diff = boxf[meshIdx1].estimatedEnd - boxf[meshIdx2].estimatedOrigin;
		float length = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);

		float error = pow(bones.at(i)->estimatedCBLength - length, 2) / pow(bones.at(i)->estimatedCBLength, 2);
		estimatedCBLengths.push_back(error);
	}

	CBError = 0;
	for (int i = 0; i < estimatedCBLengths.size(); i++){
		CBError += estimatedCBLengths.at(i);
	}
}

void groupCutNode::calculateNodeScore(Vec3f weights){
	float totalWeights = weights[0] + weights[1] + weights[2];

	nodeScore = weights[0] / totalWeights * volError
		+ weights[1] / totalWeights * CBError; //+ weights[2] / totalWeights * hashRank;
}


groupCut::groupCut()
{
	m_root = nullptr;
	mark = nullptr;
	voxelOccupy = nullptr;
}

groupCut::~groupCut()
{
	if (m_root)
	{
		delete m_root;
	}

	if (mark)
	{
		delete[]mark;
	}

	if (voxelOccupy)
	{
		delete[]voxelOccupy;
	}

	// TODO: Remove this code after testing
}

void groupCut::constructTree()
{
	m_root = new groupCutNode;
	meshPiece rootBox;
	rootBox.leftDown = sourcePiece->curLeftDown;
	rootBox.rightUp = sourcePiece->curRightUp;
	rootBox.volumei = sourcePiece->voxelIdxs.size();
	rootBox.voxels = voxelIdxs;
	m_root->boxf.push_back(rootBox);

	mark = new int[boxes->size()];
	//cprintf("groupCut.cpp, mark: %d\n", mark);
	voxelOccupy = new int[boxes->size()];

	//cprintf("groupCut.cpp, start tree recur\n");
	constructTreeRecur(m_root);
}

void groupCut::constructTreeRecur(groupCutNode * node)
{
	if (node->boxf.size() == bones.size()){
		//cprintf("in the if\n");
		// Leaf node
		// TODO: Assign bone and map configuration
		computeError(node);
		boxPose.parserConfigure(node);
		leaves.push_back(node);
		return;
	}

	// Divide the node by 3 dimension
	std::vector<meshPiece>* boxf = &node->boxf;
	for (int i = 0; i < boxf->size(); i++){
		meshPiece curB = boxf->at(i);
		for (int xyzd = 0; xyzd < 3; xyzd++){
			// in each direction, we split the object
			// Assume that the box is small. The cut step is one voxel size
			for (float coord = curB.leftDown[xyzd] + voxelSize; coord < curB.rightUp[xyzd]; coord += voxelSize){
				groupCutNode *newNode = new groupCutNode(node);
				if (!cutBox(newNode, i, xyzd, coord)){
					delete newNode;
					continue;
				}

				node->child.push_back(newNode);
				constructTreeRecur(newNode);
			}
		}
	}
}

bool groupCut::cutBox(groupCutNode * newNode, int boxIdx, int xyzd, float coord)
{
	std::vector<meshPiece> *boxf = &newNode->boxf;
	meshPiece curB = boxf->at(boxIdx);

	// now, cut the box
	std::vector<meshPiece> newCutBox;
	arrayInt voxelIdx = curB.voxels;

	std::fill(mark, mark + boxes->size(), 0);
	std::fill(voxelOccupy, voxelOccupy + boxes->size(), 0);

	for (int i = 0; i < voxelIdx.size(); i++)
	{
		voxelOccupy[voxelIdx[i]] = 1;
	}

	int nbBoxCut;

	for (int i = 0; i < voxelIdx.size(); i++)
	{
		if (mark[voxelIdx[i]] != 0)
		{
			continue;
		}

		meshPiece newMesh;
		newMesh.initForVoxel();

		// Queue voxel
		std::queue<int> vQ;
		vQ.push(voxelIdx[i]);
		
		Vec3f curCenter = boxes->at(voxelIdx[i]).center;

		if (curCenter[xyzd] > coord)
		{
			newMesh.isLower = false;
		}
		else
			newMesh.isLower = true;

		while (!vQ.empty())
		{
			int idx = vQ.front();
			vQ.pop();

			mark[idx] = 1; // Mark the voxel has been processed
			newMesh.voxels.push_back(idx);
			newMesh.fitBOundingBox(boxes->at(idx).leftDown, boxes->at(idx).rightUp);

			// Check all adjacent of the voxel
			arrayInt adjacentIdxs = neighborVoxel->at(idx);
			for (int j = 0; j < adjacentIdxs.size(); j++)
			{
				if (mark[adjacentIdxs[j]] == 0
					&& voxelOccupy[adjacentIdxs[j]] == 1)
				{
					// It is valid
					Vec3f nCenter = boxes->at(adjacentIdxs[j]).center;
					bool isLower = nCenter[xyzd] < coord;
					if (isLower == newMesh.isLower) // OK
					{
						vQ.push(adjacentIdxs[j]);
						mark[adjacentIdxs[j]] = 1;
					}
				}
			}
		}

		newCutBox.push_back(newMesh);
	}

	// Modify the node
	int preNbBox = boxf->size();

	boxf->erase(boxf->begin() + boxIdx);
	for (int i = 0; i < newCutBox.size(); i++)
	{
		newCutBox[i].volumei = newCutBox[i].voxels.size();
		if (newCutBox[i].volumei < 1)
		{
			return false;
		}

		boxf->push_back(newCutBox[i]);
	}

	if (boxf->size() <= preNbBox)
	{
		return false;
	}

	// validate the number of the box
	return boxf->size() <= bones.size();
}

bool groupCut::computeError(groupCutNode * node)
{
	std::vector<meshPiece> *boxf = &node->boxf;

	// Compute volume ratio
	int totalVi = sourcePiece->voxelIdxs.size();
	for (int i = 0; i < boxf->size(); i++)
	{
		boxf->at(i).volumeRatio = (float)boxf->at(i).volumei / totalVi;
	}

	// Sort the box by volume order

	// Map to bone array

	// Add to pose manager

	return true;
}

void groupCut::drawLeaveIdx(int idx)
{
	if (idx < 0 || idx >= leaves.size())
		return;

	leaves.at(idx)->draw(0);
}

void groupCut::drawPose(int poseIdx, int configIdx)
{
	if (poseIdx < 0 || poseIdx >= boxPose.sortedPoseMap.size()){
		return;
	}

	neighborPose* pp = boxPose.sortedPoseMap.at(poseIdx);

	if (pp->posConfigId == -1){
		return;
	}
	
	if (configIdx < 0 || configIdx >= pp->nodeGroupBoneCut.size()){
		return;
	}

	groupCutNode *node = pp->nodeGroupBoneCut.at(configIdx);
	std::map<int, int> boneMeshMap = pp->mapBone_meshIdx[configIdx];

	node->draw(bones, boneMeshMap);
	node->drawNeighbor(boneMeshMap, boxPose.neighborInfo, voxelSize);
}

void groupCut::calculateNodeErrors(int poseIdx, int configIdx){
	if (poseIdx < 0 || poseIdx >= boxPose.sortedPoseMap.size()){
		return;
	}
	neighborPose* pp = boxPose.sortedPoseMap.at(poseIdx);

	groupCutNode *node = pp->nodeGroupBoneCut[configIdx];
	std::map<int, int> boneMeshMap = pp->mapBone_meshIdx[configIdx];

	node->calculateVolError(bones, boneMeshMap);
	node->calculateCBError(bones, boneMeshMap, boxPose.neighborInfo);
}

// Recalculates configuration scores
// Sorts configurations within each pose
void groupCut::sortEvaluations(Vec3f weights){
	for (int i = 0; i < boxPose.sortedPoseMap.size(); i++){
		neighborPose* pp = boxPose.sortedPoseMap.at(i);
		
		for (int j = 0; j < pp->nodeGroupBoneCut.size(); j++){
			pp->nodeGroupBoneCut[j]->calculateNodeScore(weights);
		}

		pp->sortNodesInGroupCut();
	}
}