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

	m_weightError = Vec3f(0.3, 0.3, 0.3);
	upParentIdx = 0;
}

cutSurfTreeMngr2::~cutSurfTreeMngr2(void)
{
}

void cutSurfTreeMngr2::drawLeaf()
{
	if (!curNode){
		return;
	}

	s_boxes = &s_voxelObj->m_boxes;
	m_tree2.drawVoxel(curNode, s_boxes);
	drawNeighborRelation();

/*	for (int i = 0; i < coords.size(); i++){
		coords[i].draw();
	}*/

	for (int i = 0; i < names.size(); i++){
		Util::printw(centerPos[i][0], centerPos[i][1], centerPos[i][2], "    %s", ToAS(names[i]));
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

int cutSurfTreeMngr2::updateBestIdx(int idx1)
{
	if (idx1 < 0)
	{
		curNode = nullptr;
		cout << "Out of range, cutting pose" << endl;
		return -1;
	}

	neighborPose pose = m_tree2.poseMngr->getPose(idx1);

	int cofIdx = pose.smallestErrorIdx;
	std::vector<cutTreefNode*> *nodes = &pose.nodes;
	curNode = nodes->at(cofIdx);

	// Bone name
	names.clear();
	centerPos.clear();
	std::map<int, int> boneMeshMapIdx = pose.mapBone_meshIdx[cofIdx];
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

	return cofIdx;
}

void cutSurfTreeMngr2::drawNeighborRelation()
{
	std::vector<neighborPos> poseConfig = currentPose.posConfig;

	for (int i = 0; i < meshNeighbor.size(); i++){
		Vec2i nbIdxs = meshNeighbor[i];
		neighborPos curType = poseConfig[i];

		Vec3f pt1, pt2;
		int idxContact = floor(curType / 2);
		int idx1, idx2;
		Util::getTwoOtherIndex(idxContact, idx1, idx2);

		Vec3f ld1 = allMeshes[nbIdxs[0]].leftDown;
		Vec3f ru1 = allMeshes[nbIdxs[0]].rightUp;

		Vec3f ld2 = allMeshes[nbIdxs[1]].leftDown;
		Vec3f ru2 = allMeshes[nbIdxs[1]].rightUp;

		Vec3f ldContact, ruContact;
		ldContact[idx1] = std::max({ld1[idx1], ld2[idx1]});
		ldContact[idx2] = std::max({ ld1[idx2], ld2[idx2] });

		ruContact[idx1] = std::min({ ru1[idx1], ru2[idx1] });
		ruContact[idx2] = std::min({ ru1[idx2], ru2[idx2] });

		Vec3f ptMid = (ldContact + ruContact) / 2;
		pt1 = ptMid;
		pt2 = ptMid;

		pt1[idxContact] = (ld1[idxContact] + ru1[idxContact]) / 2;
		pt2[idxContact] = (ld2[idxContact] + ru2[idxContact]) / 2;

		glLineWidth(3.0);
		glBegin(GL_LINES);
			glVertex3fv(pt1.data());
			glVertex3fv(pt2.data());
		glEnd();
		glLineWidth(1.0);
		
		float radius = s_voxelObj->m_voxelSizef / 5;
		Util_w::drawSphere(pt1, radius);
		Util_w::drawSphere(pt2, radius);
	}
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
		neighborPose *pose = m_tree2.poseMngr->allPoses.at(idx1);

		std::cout << "Id: " << pose->posConfigId << " and smallest vol error: " << pose->smallestVolumeError << "\n";

		m_dlg->updateDisplayedValues(pose->smallestVolumeError);

		int cofIdx = pose->smallestErrorIdx;
		std::vector<cutTreefNode*> nodes = pose->nodes;
		curNode = nodes.at(cofIdx);

		curNode;

		// Bone name
		names.clear();
		centerPos.clear();
		std::map<int, int> boneMeshMapIdx = pose->mapBone_meshIdx[cofIdx];
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

void cutSurfTreeMngr2::getListOfBestPoses(){
	m_tree2.poseMngr->getAllPosesIntoVectorForm();
}

void cutSurfTreeMngr2::connectWithSideDialog(SideDialog *sd)
{
	m_dlg = sd;
}

void cutSurfTreeMngr2::updateSortEvaluations(){
	m_weights = m_dlg->weights;

	m_tree2.poseMngr->sortAccordingToWeights(m_weights);

	m_dlg->needsUpdate = false;
}