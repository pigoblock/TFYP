#include "StdAfx.h"
#include "cutSurfTreeMngr2.h"
#include "Color.h"
#include "neighbor.h"
#include "Resource.h"
#include <iostream>

using namespace std;

class CInputDialog : public CDialog
{
public:
	CInputDialog();
	enum { IDD = IDD_INPUT_WEIGHT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
public:
	Vec3f errorWeight;


	float getWeight(int id);
	void setVal(int id, float val);

	Vec3f getUserDefineWeight();
	void setWeight(Vec3f ww);
protected:
	DECLARE_MESSAGE_MAP()
};

CInputDialog::CInputDialog() : CDialog(CInputDialog::IDD)
{
}

void CInputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CInputDialog, CDialog)
END_MESSAGE_MAP()

float CInputDialog::getWeight(int id)
{
	CString stext;
	GetDlgItemText(id, stext);
	return _tstof(stext);
}

Vec3f CInputDialog::getUserDefineWeight()
{
	Vec3f ww;
	ww[0] = getWeight(IDC_NEIGHBOR);
	ww[1] = getWeight(IDC_ASPECT);
	ww[2] = getWeight(IDC_VOLUME);

	// normalize
	float sum = ww[0] + ww[1] + ww[2];
	return ww / sum;
}

void CInputDialog::setWeight(Vec3f ww)
{
	setVal(IDC_NEIGHBOR, ww[0]);
	setVal(IDC_ASPECT, ww[1]);
	setVal(IDC_VOLUME, ww[2]);
}

void CInputDialog::setVal(int id, float val)
{
	CString text;
	text.Format(_T("%f"), val);
	SetDlgItemText(id, text.GetBuffer());
}

BOOL CInputDialog::OnInitDialog()
{
	BOOL a = CDialog::OnInitDialog();
	setWeight(errorWeight);

	return a;
}

BOOL CInputDialog::DestroyWindow()
{
	errorWeight = getUserDefineWeight();
	return CDialog::DestroyWindow();
}



cutSurfTreeMngr2::cutSurfTreeMngr2(void)
{
	leatE2Node = nullptr;
	leatE2Node2 = nullptr;
	curNode = nullptr;
	savedNode1 = nullptr;
	savedNode2 = nullptr;

	drawNeedsUpdate = false;
	m_weightError = Vec3f(0.3, 0.3, 0.3);
}

cutSurfTreeMngr2::~cutSurfTreeMngr2(void)
{
}

void cutSurfTreeMngr2::drawLeaf(int mode)
{
	if (mode == 0){
		if (!curNode){
			return;
		}

		s_boxes = &s_voxelObj->m_boxes;
		m_tree2.drawVoxel(curNode, s_boxes);
		drawNeighborRelation(0);

		/*	for (int i = 0; i < coords.size(); i++){
		coords[i].draw();
		}*/

		for (int i = 0; i < names.size(); i++){
			Util::printw(centerPos[i][0], centerPos[i][1], centerPos[i][2], "    %s", ToAS(names[i]));
		}
	}
	else if (mode == 1){
		if (!savedNode1){
			return;
		}

		s_boxes = &s_voxelObj->m_boxes;
		m_tree2.drawVoxel(savedNode1, s_boxes);
		drawNeighborRelation(1);

		for (int i = 0; i < names.size(); i++){
			Util::printw(savedCenterPos1[i][0], savedCenterPos1[i][1], savedCenterPos1[i][2], "    %s", ToAS(savedNames1[i]));
		}
	}
	else {
		if (!savedNode2){
			return;
		}

		s_boxes = &s_voxelObj->m_boxes;
		m_tree2.drawVoxel(savedNode2, s_boxes);
		drawNeighborRelation(2);

		for (int i = 0; i < names.size(); i++){
			Util::printw(savedCenterPos2[i][0], savedCenterPos2[i][1], savedCenterPos2[i][2], "    %s", ToAS(savedNames2[i]));
		}
	}
}

void cutSurfTreeMngr2::drawNeighborRelation(int mode)
{
	if (mode == 0){
		for (int i = 0; i < meshNeighbor.size(); i++){
			Vec2i nbIdxs = meshNeighbor[i];

			Vec3f firstEnd = allMeshes[nbIdxs[0]].estimatedEnd;
			Vec3f secondStart = allMeshes[nbIdxs[1]].estimatedOrigin;

			glLineWidth(3.0);
			glBegin(GL_LINES);
			glVertex3f(firstEnd[0], firstEnd[1], firstEnd[2]);
			glVertex3f(secondStart[0], secondStart[1], secondStart[2]);
			glEnd();
			glLineWidth(1.0);

			float radius = s_voxelObj->m_voxelSizef / 5;
			Util_w::drawSphere(firstEnd, radius);
			Util_w::drawSphere(secondStart, radius);
		}
	}
	else if (mode == 1){
		for (int i = 0; i < savedMeshNeighbor1.size(); i++){
			Vec2i nbIdxs = savedMeshNeighbor1[i];

			Vec3f firstEnd = savedAllMeshes1[nbIdxs[0]].estimatedEnd;
			Vec3f secondStart = savedAllMeshes1[nbIdxs[1]].estimatedOrigin;

			glLineWidth(3.0);
			glBegin(GL_LINES);
			glVertex3f(firstEnd[0], firstEnd[1], firstEnd[2]);
			glVertex3f(secondStart[0], secondStart[1], secondStart[2]);
			glEnd();
			glLineWidth(1.0);

			float radius = s_voxelObj->m_voxelSizef / 5;
			Util_w::drawSphere(firstEnd, radius);
			Util_w::drawSphere(secondStart, radius);
		}
	}
	else {
		for (int i = 0; i < savedMeshNeighbor2.size(); i++){
			Vec2i nbIdxs = savedMeshNeighbor2[i];

			Vec3f firstEnd = savedAllMeshes2[nbIdxs[0]].estimatedEnd;
			Vec3f secondStart = savedAllMeshes2[nbIdxs[1]].estimatedOrigin;

			glLineWidth(3.0);
			glBegin(GL_LINES);
			glVertex3f(firstEnd[0], firstEnd[1], firstEnd[2]);
			glVertex3f(secondStart[0], secondStart[1], secondStart[2]);
			glEnd();
			glLineWidth(1.0);

			float radius = s_voxelObj->m_voxelSizef / 5;
			Util_w::drawSphere(firstEnd, radius);
			Util_w::drawSphere(secondStart, radius);
		}
	}
	
}

// viewNum: 0 = left, 1 = right
// mode: 0 = none, 1 = have, 2 = both, need to compare
void cutSurfTreeMngr2::drawPoseInfoText(int viewNum, int mode){
	if (viewNum == 0){
		if (mode == 0){
			Util::printw(-50, 0, 0, "%s", ToAS("No pose considered yet."));
		}
		else if (mode == 1){
			Util::printw(-100, -20, 0, "%s %d %s", ToAS("Pose"), savedPoseIdx1, ToAS("considered here."));
		}
		else {
			if (savedPose1->smallestVolumeError < savedPose2->smallestVolumeError){
				Util::printw(-100, -20, 0, "%s", ToAS("Smaller volume error."));
			}
			else if (savedPose1->smallestVolumeError == savedPose2->smallestVolumeError){
				Util::printw(-100, -20, 0, "%s", ToAS("Same volume error."));
			}
			else {
				Util::printw(-100, -20, 0, "%s", ToAS("Larger volume error."));
			}

			if (savedPose1->smallestCBError < savedPose2->smallestCBError){
				Util::printw(-100, -40, 0, "%s", ToAS("Smaller connecting bone error."));
			}
			else if (savedPose1->smallestCBError == savedPose2->smallestCBError){
				Util::printw(-100, -40, 0, "%s", ToAS("Same connecting bone error."));
			}
			else {
				Util::printw(-100, -40, 0, "%s", ToAS("Larger connecting bone error."));
			}

			if (savedPose1->hashRank < savedPose2->hashRank){
				Util::printw(-100, -60, 0, "%s", ToAS("Smaller positioning error."));
			}
			else if (savedPose1->hashRank == savedPose2->hashRank){
				Util::printw(-100, -60, 0, "%s", ToAS("Same positioning error."));
			}
			else {
				Util::printw(-100, -60, 0, "%s", ToAS("Larger positioning error."));
			}
		}
	}
	else {
		if (mode == 0){
			Util::printw(-50, 0, 0, "%s", ToAS("No pose considered yet."));
		}
		else if (mode == 1){
			Util::printw(-100, -20, 0, "%s %d %s", ToAS("Pose"), savedPoseIdx2, ToAS("considered here."));
		}
		else {
			if (savedPose2->smallestVolumeError < savedPose1->smallestVolumeError){
				Util::printw(-100, -20, 0, "%s", ToAS("Smaller volume error."));
			}
			else if (savedPose2->smallestVolumeError == savedPose1->smallestVolumeError){
				Util::printw(-100, -20, 0, "%s", ToAS("Same volume error."));
			}
			else {
				Util::printw(-100, -20, 0, "%s", ToAS("Larger volume error."));
			}
			if (savedPose2->smallestCBError < savedPose1->smallestCBError){
				Util::printw(-100, -40, 0, "%s", ToAS("Smaller connecting bone error."));
			}
			else if (savedPose2->smallestCBError == savedPose1->smallestCBError){
				Util::printw(-100, -40, 0, "%s", ToAS("Same connecting bone error."));
			}
			else {
				Util::printw(-100, -40, 0, "%s", ToAS("Larger connecting bone error."));
			}
			if (savedPose2->hashRank < savedPose1->hashRank){
				Util::printw(-100, -60, 0, "%s", ToAS("Smaller positioning error."));
			}
			else if (savedPose2->hashRank == savedPose1->hashRank){
				Util::printw(-100, -60, 0, "%s", ToAS("Same positioning error."));
			}
			else {
				Util::printw(-100, -60, 0, "%s", ToAS("Larger positioning error."));
			}
		}
	}
}

bool compareBone_descen(const boneAbstract &lhs, const boneAbstract &rhs)
{
	if (lhs.volumeRatiof < (1-VOL_SIMILAR_ERROR)*rhs.volumeRatiof)
	{
		return true;
	}
	else if (lhs.volumeRatiof < (1+VOL_SIMILAR_ERROR)*rhs.volumeRatiof) // similar
	{
		return rhs.aspectRatiof[0]+rhs.aspectRatiof[1] > rhs.aspectRatiof[0]+rhs.aspectRatiof[1];
	}
	else
		return false;
}

bool compareBoneVolume_descen(const boneAbstract &lhs, const boneAbstract &rhs)
{

	return lhs.volumeRatiof > rhs.volumeRatiof;
}

std::vector<meshCutRough> cutSurfTreeMngr2::convertBoxesToMeshCut(std::vector<Boxi> boxes)
{
	std::vector<meshCutRough> sortedCut;

	for (int i = 0; i < boxes.size(); i++)
	{
		meshCutRough newCut;
		newCut.boundingBoxi = boxes[i];
		newCut.volumei = m_voxelHighRes.volumeInBox(newCut.boundingBoxi);
		newCut.volumeRatiof = (float)newCut.volumei / m_voxelHighRes.objectVolumei();
		newCut.computeAspectRatio();

		// Insert to sorted array
		sortedCut.push_back(newCut);
	}

	std::sort(sortedCut.begin(), sortedCut.end(), compareCutMesh_descen);
	return sortedCut;
}

void cutSurfTreeMngr2::exportMesh()
{
	if (curNode)
	{
		FILE *f = fopen("../meshPiece.txt", "w");
		ASSERT(f);

		std::vector<meshPiece> *centerMesh = &curNode->centerBoxf;
		std::vector<meshPiece> *sideMesh = &curNode->sideBoxf;
		fprintf(f, "%d\n", centerMesh->size() + sideMesh->size());

		for (int i = 0; i < centerMesh->size(); i++)
		{
			fprintf(f, "%s\n", m_centerBoneOrder[i].original->m_name.GetBuffer());
			fprintf(f, "%d\n", CENTER_BONE); // center bone
			Vec3f ld = centerMesh->at(i).leftDown;
			fprintf(f, "%f %f %f\n", ld[0], ld[1], ld[2]);
			Vec3f ru = centerMesh->at(i).rightUp;
			fprintf(f, "%f %f %f\n", ru[0], ru[1], ru[2]);
		}

		for (int i = 0; i < sideMesh->size(); i++)
		{
			fprintf(f, "%s\n", m_sideBoneOrder[i].original->m_name.GetBuffer());
			fprintf(f, "%d\n", SIDE_BONE); // center bone
			Vec3f ld = sideMesh->at(i).leftDown;
			fprintf(f, "%f %f %f\n", ld[0], ld[1], ld[2]);
			Vec3f ru = sideMesh->at(i).rightUp;
			fprintf(f, "%f %f %f\n", ru[0], ru[1], ru[2]);
		}

		fclose(f);
	}
}

void cutSurfTreeMngr2::drawLocalCoord(cutTreefNode * node)
{
	if (!node)
	{
		return;
	}
	// Neighbor error
	std::vector<meshPiece> *centerMesh = &node->centerBoxf;
	std::vector<meshPiece> *sideMesh = &node->sideBoxf;

	for (int i = 0; i < centerMesh->size(); i++)
	{
		meshPiece curMesh = (*centerMesh)[i];
		boneTransform2 tr;
		tr.init(curMesh.m_realativeCoord, curMesh.leftDown, curMesh.rightUp, m_centerBoneOrder[i].original->leftDownf, m_centerBoneOrder[i].original->rightUpf);
		
		tr.draw();
	}

	for (int i = 0; i < sideMesh->size(); i++)
	{
		meshPiece curMesh = (*sideMesh)[i];
		boneTransform2 tr;
		tr.init(curMesh.m_realativeCoord, curMesh.leftDown, curMesh.rightUp, m_sideBoneOrder[i].original->leftDownf, m_sideBoneOrder[i].original->rightUpf);

		tr.draw();
	}
}

void cutSurfTreeMngr2::showWeightInputDialog()
{
	CInputDialog dlg;
	dlg.errorWeight = m_weightError;
	
	if(dlg.DoModal() == IDOK)
	{
		m_weightError = dlg.errorWeight;

		curNode = m_tree2.m_hashErrorTable.findSmallestNode(m_weightError);

		if (curNode) // Draw axis
		{
			// Neighbor error
			std::vector<meshPiece> *centerMesh = &curNode->centerBoxf;
			std::vector<meshPiece> *sideMesh = &curNode->sideBoxf;

			coords.clear();
			for (int i = 0; i < centerMesh->size(); i++)
			{
				meshPiece curMesh = (*centerMesh)[i];
				boneTransform2 tr;
				tr.init(curMesh.m_realativeCoord, curMesh.leftDown, curMesh.rightUp, m_centerBoneOrder[i].original->leftDownf, m_centerBoneOrder[i].original->rightUpf);
				coords.push_back(tr);
			}

			for (int i = 0; i < sideMesh->size(); i++)
			{
				meshPiece curMesh = (*sideMesh)[i];
				boneTransform2 tr;
				tr.init(curMesh.m_realativeCoord, curMesh.leftDown, curMesh.rightUp, m_sideBoneOrder[i].original->leftDownf, m_sideBoneOrder[i].original->rightUpf);

				coords.push_back(tr);
			}

			// assign least node
		//	leatE2Node2 = curNode;
		}
	}
}

void cutSurfTreeMngr2::setVoxelArray()
{
	std::vector<int> voxelHash;
	Vec3i NumXYZ = hashTable.NumXYZ;
	voxelHash.resize(NumXYZ[0]*NumXYZ[1]*NumXYZ[2]);
	std::fill(voxelHash.begin(), voxelHash.end(), -1);

	std::vector<octreeSNode*> *leaves = &m_octree.leaves;
	boxes.resize(leaves->size());
	for (int i = 0; i < leaves->size(); i++)
	{
		octreeSNode* node = leaves->at(i);
		// List
		voxelBox newBox(node->leftDownf, node->rightUpTight);
		Vec3f xyzIdx = hashTable.getVoxelCoord(newBox.center);
		newBox.xyzIndex = xyzIdx;

		boxes[i] = newBox;

		// Hash
		int hashF = xyzIdx[2]*NumXYZ[0]*NumXYZ[1] + xyzIdx[1]*NumXYZ[0] + xyzIdx[0];

		voxelHash[hashF] = i;
		node->idxInLeaf = i;
	}

	hashTable.voxelHash = voxelHash;
}

void cutSurfTreeMngr2::updateDisplay(int idx1, int idx2)
{
	if (idx1 < 0){
		curNode = nullptr;
		return;
	}

	std::map<int, neighborPose> *poseMap = &m_tree2.poseMngr->poseMap; 
	if (idx1 >= poseMap->size()){
		return;
	}

	std::map<int, neighborPose>::iterator it = poseMap->begin();
	for (int i = 0; i < idx1; i++){
		++it;
	}
	neighborPose pose = (*it).second;
	currentPose = pose;

	std::vector<cutTreefNode*> *nodes = &pose.nodes;

	if (idx2 == -1){
		idx2 = pose.smallestErrorIdx;
	}

	if (idx2 < 0 || idx2 >= nodes->size()){
		return;
	}

	std::cout << "Pose: " << idx1 << ": " << nodes->size() << " configurations Id: " << currentPose.posConfigId << std::endl;

	poseIdx = idx1;
	nodeIdx = idx2;

	// We have the node
	curNode = nodes->at(idx2);

	// Bone name
	names.clear();
	centerPos.clear();
	std::map<int, int> boneMeshMapIdx = pose.mapBone_meshIdx[idx2];
	std::vector<bone*> sortedBone = poseMngr.sortedBone;
	std::vector<meshPiece> *centerBox = &curNode->centerBoxf;
	std::vector<meshPiece> *sideBox = &curNode->sideBoxf;
	allMeshes.clear();

	for (int i = 0; i < sortedBone.size(); i++){
		CString boneName = sortedBone[i]->m_name;
		int meshIdx = boneMeshMapIdx[i];
		meshPiece mesh;

		if (meshIdx < centerBox->size()){
			mesh = centerBox->at(meshIdx);
		} else{
			mesh = sideBox->at(meshIdx-centerBox->size());
		}	

		Vec3f center = (mesh.leftDown + mesh.rightUp)/2.0;
		
		names.push_back(boneName);
		centerPos.push_back(center);
		allMeshes.push_back(mesh);
	}

	meshNeighbor = poseMngr.neighborPair;
}

int cutSurfTreeMngr2::findBestOption(int idx1)
{
	if (idx1 < 0){
		return -1;
	}

	std::map<int, neighborPose> *poseMap = &m_tree2.poseMngr->poseMap; 
	if (idx1 >= poseMap->size()){
		return -1;
	}

	std::map<int, neighborPose>::iterator it = poseMap->begin();
	for (int i = 0; i < idx1; i++){
		++it;
	}
	neighborPose pose = (*it).second;

	int idx2; // For best pose
	float smallest = MAX;
	std::vector<cutTreefNode*> *nodes = &pose.nodes;
	
	for (int i = 0; i < nodes->size(); i++){
		// Evaluate volume error only
		cutTreefNode* node = nodes->at(i);
		float error = m_tree2.poseMngr->evaluateError(idx1, i);
		if (error < smallest)
		{
			smallest = error;
			idx2 = i;
		}
	}
	
	poseIdx = idx1;
	nodeIdx = idx2;
	// We have the node
	curNode = nodes->at(idx2);

	// Bone name
	names.clear();
	centerPos.clear();
	std::map<int, int> boneMeshMapIdx = pose.mapBone_meshIdx[idx2];
	std::vector<bone*> sortedBone = poseMngr.sortedBone;
	std::vector<meshPiece> *centerBox = &curNode->centerBoxf;
	std::vector<meshPiece> *sideBox = &curNode->sideBoxf;

	for (int i = 0; i < sortedBone.size(); i++){
		CString boneName = sortedBone[i]->m_name;
		int meshIdx = boneMeshMapIdx[i];
		meshPiece mesh;
		if (meshIdx < centerBox->size()){
			mesh = centerBox->at(meshIdx);
		} else{
			mesh = sideBox->at(meshIdx - centerBox->size());
		}

		Vec3f center = (mesh.leftDown + mesh.rightUp)/2.0;

		names.push_back(boneName);
		centerPos.push_back(center);
	}

	return idx2;
}

// MainControl calls this method to cut voxel object
void cutSurfTreeMngr2::init()
{
	octreeSizef = s_voxelObj->m_voxelSizef;

	// Get bone list and divide to center type and side type
	parserSkeletonGroup(); 
	constructCutTree();
}

// Get bone list and divide to center type and side type
void cutSurfTreeMngr2::parserSkeletonGroup()
{
	// Coming from init, sorted and neighborPair are empty
	std::vector<bone*> sorted;
	//sorted vector will be filled with bones sorted into bone groups and neighbours
	s_groupSkeleton->getBoneGroupAndNeighborInfo(sorted, neighborPair);	

	long boneVol = 0;
	for (size_t i = 0; i < sorted.size(); i++){
		boneAbstract newBone(sorted[i], s_voxelObj->m_voxelSizef);
		newBone.idxInArray = i;
		newBone.original = sorted[i];

		if (sorted[i]->m_type == TYPE_CENTER_BONE){
			m_centerBoneOrder.push_back(newBone);
		} else{
			m_sideBoneOrder.push_back(newBone);
		}

		boneVol += sorted[i]->getVolumef();
	}

	// Store volume ratios
	for (size_t i = 0; i < m_centerBoneOrder.size(); i++){
		m_centerBoneOrder[i].volumeRatiof = (float)(m_centerBoneOrder[i].original->getVolumef()) / boneVol;
	}
	for (size_t i = 0; i < m_sideBoneOrder.size(); i++){
		m_sideBoneOrder[i].volumeRatiof = (float)(m_sideBoneOrder[i].original->getVolumef()) / boneVol;
	}

	// Sort the bone, in descending order of volume
	std::sort(m_centerBoneOrder.begin(), m_centerBoneOrder.end(), compareBoneVolume_descen);
	std::sort(m_sideBoneOrder.begin(), m_sideBoneOrder.end(), compareBoneVolume_descen);

	// Setting up bone neighbor information
	std::vector<indexBone> idxMap;
	idxMap.resize(sorted.size());
	for (int i = 0; i < m_centerBoneOrder.size(); i++){
		indexBone idx;
		idx.boneType = TYPE_CENTER_BONE; // array of center bone
		idx.idxInArray = i;
		idxMap[m_centerBoneOrder[i].idxInArray] = idx;
	}
	for (int i = 0; i < m_sideBoneOrder.size(); i++){
		indexBone idx;
		idx.boneType = TYPE_SIDE_BONE; // array of side bone
		idx.idxInArray = i;
		idxMap[m_sideBoneOrder[i].idxInArray] = idx;
	}

	for (int i = 0; i < neighborPair.size(); i++){
		neighbor nb;
		nb.first = idxMap[neighborPair[i].first];
		nb.second = idxMap[neighborPair[i].second];
		neighborInfo.push_back(nb);
	}
}

void cutSurfTreeMngr2::constructCutTree()
{
	m_tree2.centerBoneOrder = m_centerBoneOrder;
	m_tree2.sideBoneOrder = m_sideBoneOrder;
	m_tree2.neighborInfo = neighborInfo;
	m_tree2.cutStep = octreeSizef;
	m_tree2.octreeS = &s_voxelObj->m_octree;
	m_tree2.boxes = &s_voxelObj->m_boxes;
	m_tree2.hashTable = &s_voxelObj->m_hashTable;
	m_tree2.poseMngr = &poseMngr;
	poseMngr.s_skeleton = s_groupSkeleton;

	m_tree2.bUniformCutStep = bUniformCut;
	m_tree2.constructTreeWithVoxel();
	leatE2Node2 = m_tree2.lestE2Node;

	std::cout << "Finish cut tree construction" << std::endl;
}

void cutSurfTreeMngr2::filterPose(std::vector<neighborPos> pp)
{
	cout << "Filter the pose" << endl;

	poseMngr.updateFilteredList(pp);
}

void cutSurfTreeMngr2::updateDisplayFilter(int idx1, int idx2)
{
	std::vector<neighborPose*> *poseMap = &m_tree2.poseMngr->filteredPose;
	if (idx1 < 0 || idx1 >= poseMap->size()){
		curNode = nullptr;
		return;
	}

	neighborPose pose = *poseMap->at(idx1);
	currentPose = pose;

	std::vector<cutTreefNode*> *nodes = &pose.nodes;

	if (idx2 == -1){
		idx2 = pose.smallestErrorIdx;
	}

	if (idx2 < 0 || idx2 >= nodes->size()){
		return;
	}

	std::cout << "Pose: " << idx1 << "; " << nodes->size() << " configurations" << std::endl;

	poseIdx = idx1;
	nodeIdx = idx2;

	// We have the node
	curNode = nodes->at(idx2);

	// Bone name
	names.clear();
	centerPos.clear();
	std::map<int, int> boneMeshMapIdx = pose.mapBone_meshIdx[idx2];
	std::vector<bone*> sortedBone = poseMngr.sortedBone;
	std::vector<meshPiece> *centerBox = &curNode->centerBoxf;
	std::vector<meshPiece> *sideBox = &curNode->sideBoxf;
	allMeshes.clear();

	for (int i = 0; i < sortedBone.size(); i++)
	{
		CString boneName = sortedBone[i]->m_name;
		int meshIdx = boneMeshMapIdx[i];
		meshPiece mesh;
		if (meshIdx < centerBox->size())
		{
			mesh = centerBox->at(meshIdx);
		}
		else
		{
			mesh = sideBox->at(meshIdx - centerBox->size());
		}


		Vec3f center = (mesh.leftDown + mesh.rightUp) / 2.0;

		names.push_back(boneName);
		centerPos.push_back(center);
		allMeshes.push_back(mesh);
	}

	meshNeighbor = poseMngr.neighborPair;
}

int cutSurfTreeMngr2::updateBestIdxFilter(int idx1)
{
	if (idx1 < 0){
		curNode = nullptr;
		cout << "Out of range, cutting pose" << endl;
		return -1;
	}

	try{
		curPose = m_tree2.poseMngr->allPoses.at(idx1);

		int cofIdx = curPose->smallestErrorIdx;
		std::vector<cutTreefNode*> nodes = curPose->nodes;
		curNode = nodes.at(cofIdx);

		m_dlg->updateDisplayedOverallError(curPose->poseScore);
		m_dlg->updateDisplayedVolumeError(curPose->smallestVolumeError);
		m_dlg->updateDisplayedHashError(curPose->hashRank);
		m_dlg->updateDisplayedCBError(curPose->smallestCBError);

		// Bone name
		names.clear();
		centerPos.clear();
		std::map<int, int> boneMeshMapIdx = curPose->mapBone_meshIdx[cofIdx];
		std::vector<bone*> sortedBone = poseMngr.sortedBone;
		std::vector<meshPiece> *centerBox = &curNode->centerBoxf;
		std::vector<meshPiece> *sideBox = &curNode->sideBoxf;
		allMeshes.clear();

		for (int i = 0; i < sortedBone.size(); i++){
			CString boneName = sortedBone[i]->m_name;
			int meshIdx = boneMeshMapIdx[i];
			meshPiece mesh;
			if (meshIdx < centerBox->size()){
				mesh = centerBox->at(meshIdx);
			} else{
				mesh = sideBox->at(meshIdx - centerBox->size());
			}
			mesh.boneIndex = sortedBone[i]->m_index;

			Vec3f center = (mesh.leftDown + mesh.rightUp) / 2.0;

			names.push_back(boneName);
			centerPos.push_back(center);
			allMeshes.push_back(mesh);
		}

		meshNeighbor = poseMngr.neighborPair;
		poseIdx = idx1;
		nodeIdx = cofIdx;

		return cofIdx;
	} catch (...)	{
		cout << "Out of range, cutting pose" << endl;
		return -1;
	}
}

void cutSurfTreeMngr2::setSavedPose1(int idx1)
{
	if (idx1 < 0){
		savedNode1 = nullptr;
		cout << "Out of range, cutting pose" << endl;
	}

	try{
		savedPose1 = m_tree2.poseMngr->allPoses.at(idx1);

		int cofIdx = savedPose1->smallestErrorIdx;
		std::vector<cutTreefNode*> nodes = savedPose1->nodes;
		savedNode1 = nodes.at(cofIdx);

		// Bone name
		savedNames1.clear();
		savedCenterPos1.clear();
		std::map<int, int> boneMeshMapIdx = savedPose1->mapBone_meshIdx[cofIdx];
		std::vector<bone*> sortedBone = poseMngr.sortedBone;
		std::vector<meshPiece> *centerBox = &savedNode1->centerBoxf;
		std::vector<meshPiece> *sideBox = &savedNode1->sideBoxf;
		savedAllMeshes1.clear();

		for (int i = 0; i < sortedBone.size(); i++){
			CString boneName = sortedBone[i]->m_name;
			int meshIdx = boneMeshMapIdx[i];
			meshPiece mesh;
			if (meshIdx < centerBox->size()){
				mesh = centerBox->at(meshIdx);
			}
			else{
				mesh = sideBox->at(meshIdx - centerBox->size());
			}

			Vec3f center = (mesh.leftDown + mesh.rightUp) / 2.0;

			savedNames1.push_back(boneName);
			savedCenterPos1.push_back(center);
			savedAllMeshes1.push_back(mesh);
		}

		savedMeshNeighbor1 = poseMngr.neighborPair;
		savedPoseIdx1 = idx1;
		savedNodeIdx1 = cofIdx;
	}
	catch (...)	{
		cout << "Out of range, cutting pose" << endl;
	}
}

void cutSurfTreeMngr2::setSavedPose2(int idx1)
{
	if (idx1 < 0){
		savedNode2 = nullptr;
		cout << "Out of range, cutting pose" << endl;
	}

	try{
		savedPose2 = m_tree2.poseMngr->allPoses.at(idx1);

		int cofIdx = savedPose2->smallestErrorIdx;
		std::vector<cutTreefNode*> nodes = savedPose2->nodes;
		savedNode2 = nodes.at(cofIdx);

		// Bone name
		savedNames2.clear();
		savedCenterPos2.clear();
		std::map<int, int> boneMeshMapIdx = savedPose2->mapBone_meshIdx[cofIdx];
		std::vector<bone*> sortedBone = poseMngr.sortedBone;
		std::vector<meshPiece> *centerBox = &savedNode2->centerBoxf;
		std::vector<meshPiece> *sideBox = &savedNode2->sideBoxf;
		savedAllMeshes2.clear();

		for (int i = 0; i < sortedBone.size(); i++){
			CString boneName = sortedBone[i]->m_name;
			int meshIdx = boneMeshMapIdx[i];
			meshPiece mesh;
			if (meshIdx < centerBox->size()){
				mesh = centerBox->at(meshIdx);
			}
			else{
				mesh = sideBox->at(meshIdx - centerBox->size());
			}

			Vec3f center = (mesh.leftDown + mesh.rightUp) / 2.0;

			savedNames2.push_back(boneName);
			savedCenterPos2.push_back(center);
			savedAllMeshes2.push_back(mesh);
		}

		savedMeshNeighbor2 = poseMngr.neighborPair;
		savedPoseIdx2 = idx1;
		savedNodeIdx2 = cofIdx;
	}
	catch (...)	{
		cout << "Out of range, cutting pose" << endl;
	}
}

void cutSurfTreeMngr2::getListOfBestPoses(){
	m_tree2.poseMngr->getAllPosesIntoVectorForm();
}

void cutSurfTreeMngr2::connectWithDialog(FilterCutDialog *cd)
{
	m_dlg = cd;
}

void cutSurfTreeMngr2::calculateSortingRequirements(std::vector<int> idealHashes){
	m_tree2.poseMngr->calculateRankScoreByHash(idealHashes);
	
	calculateEstimatedCBLengths();
	for (int i = 0; i < m_tree2.poseMngr->allPoses.size(); i++){
		m_tree2.poseMngr->allPoses.at(i)->calculateCBScores();
	}
}

void cutSurfTreeMngr2::calculateEstimatedCBLengths(){
	std::vector<bone*> sortedBone = poseMngr.sortedBone;

	for (int i = 0; i < m_tree2.poseMngr->allPoses.size(); i++){
		neighborPose *pose = m_tree2.poseMngr->allPoses.at(i);

		int cofIdx = pose->smallestErrorIdx;
		std::vector<cutTreefNode*> *nodes = &pose->nodes;
		cutTreefNode *currentNode = nodes->at(cofIdx);

		// Bone name
		std::map<int, int> boneMeshMapIdx = pose->mapBone_meshIdx[cofIdx];	
		std::vector<meshPiece> *centerBox = &currentNode->centerBoxf;
		std::vector<meshPiece> *sideBox = &currentNode->sideBoxf;

		for (int j = 0; j < sortedBone.size(); j++){
			Vec3f boneBoxSize = sortedBone[j]->m_sizef;

			int meshIdx = boneMeshMapIdx[j];
			meshPiece mesh;
			if (meshIdx < centerBox->size()){
				mesh = centerBox->at(meshIdx);
			} else{
				mesh = sideBox->at(meshIdx - centerBox->size());
			}
			Vec3f cutBoxSize = mesh.sizef;

			Vec3i SMLIdxBone = Util_w::SMLIndexSizeOrder(boneBoxSize);
			Vec3i SMLIdxCutBox = mesh.SMLEdgeIdx;

			// Refactor these
			Vec3i mapCoord;
			if (sortedBone[j]->m_type == TYPE_CENTER_BONE){
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
					if (max(score012y, score012z) <= max(score021y, score021z)){
						mapCoord = Vec3f(0, 1, 2);
					}
					else{
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

			Vec3f origin = (mesh.leftDown + mesh.rightUp) / 2.0;
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

			if (meshIdx < centerBox->size()){
				centerBox->at(meshIdx).estimatedOrigin = origin;
				centerBox->at(meshIdx).estimatedEnd = end;
				centerBox->at(meshIdx).mapCoord = mapCoord;
			}
			else{
				sideBox->at(meshIdx - centerBox->size()).estimatedOrigin = origin;
				sideBox->at(meshIdx - centerBox->size()).estimatedEnd = end;
				sideBox->at(meshIdx - centerBox->size()).mapCoord = mapCoord;
			}
		}
	}

	for (int i = 0; i < m_tree2.poseMngr->allPoses.size(); i++){
		neighborPose *pose = m_tree2.poseMngr->allPoses.at(i);

		int cofIdx = pose->smallestErrorIdx;
		std::vector<cutTreefNode*> *nodes = &pose->nodes;
		cutTreefNode *currentNode = nodes->at(cofIdx);

		// Bone name
		std::map<int, int> boneMeshMapIdx = pose->mapBone_meshIdx[cofIdx];
		std::vector<meshPiece> *centerBox = &currentNode->centerBoxf;
		std::vector<meshPiece> *sideBox = &currentNode->sideBoxf;
		
		// First bone in sortedBone is always the root/torso
		// Following bones will be connected to the root/torso

		int rootMeshIdx = boneMeshMapIdx[0];
		Vec3f rootEndJoint = centerBox->at(rootMeshIdx).estimatedEnd;

		for (int j = 1; j < sortedBone.size(); j++){
			Vec3f estimatedStart; 

			int meshIdx = boneMeshMapIdx[j];
			if (meshIdx < centerBox->size()){
				estimatedStart = centerBox->at(meshIdx).estimatedOrigin;
			}
			else{
				estimatedStart = sideBox->at(meshIdx - centerBox->size()).estimatedOrigin;
			}

			Vec3f diff = estimatedStart - rootEndJoint;
			float length = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);

			float error = pow(sortedBone[j]->estimatedCBLength - length, 2) / pow(sortedBone[j]->estimatedCBLength, 2);
			pose->estimatedCBLengths.push_back(error);
		}
	}
}

void cutSurfTreeMngr2::updateSortEvaluations(float weights[3]){
	m_dlg->weights[0] = weights[0];
	m_dlg->weights[1] = weights[1];
	m_dlg->weights[2] = weights[2];

	m_weights = weights;

	m_tree2.poseMngr->sortAccordingToWeights(m_weights);

	m_dlg->needsUpdate = false;
}