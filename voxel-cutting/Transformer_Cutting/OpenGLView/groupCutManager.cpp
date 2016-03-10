#include "stdafx.h"
#include <iostream>
#include "groupCutManager.h"

groupCutManager::groupCutManager()
{
	m_octree = nullptr;
	m_dlg = nullptr;

	curBoneIdx = 0;
	idx1 = 0;
	idx2 = 0;
}

groupCutManager::~groupCutManager()
{
	if (m_octree){
		delete m_octree;
	}
	if (m_dlg){
		delete m_dlg;
	}
}

void groupCutManager::loadVoxelArray()
{
	// index of voxel start from left down to right up
	// NX
	// Hash function: idx = i*NX*NY + j*NY + k
	std::vector<int> voxelHash;
	voxelHash.resize(NumXYZ[0] * NumXYZ[1] * NumXYZ[2]);
	std::fill(voxelHash.begin(), voxelHash.end(), -1);

	std::vector<octreeSNode*> *leaves = &m_octree->leaves;
	boxes.resize(leaves->size());
	for (int i = 0; i < leaves->size(); i++)
	{
		octreeSNode* node = leaves->at(i);
		// List
		voxelBox newBox(node->leftDownf, node->rightUpTight);
		Vec3i xyzIdx = hashTable.getVoxelCoord(newBox.center);
		newBox.xyzIndex = xyzIdx;

		boxes[i] = newBox;

		// Hash
		int hashF = xyzIdx[2] * NumXYZ[0] * NumXYZ[1] + xyzIdx[1] * NumXYZ[0] + xyzIdx[0];

		voxelHash[hashF] = i;
		node->idxInLeaf = i;
	}

	hashTable.voxelHash = voxelHash;

	// neighbor information
	// neighbor
	neighborVoxel.resize(boxes.size());

	for (int i = 0; i < NumXYZ[0]; i++){
		for (int j = 0; j < NumXYZ[1]; j++){
			for (int k = 0; k < NumXYZ[2]; k++){
				int idx = hashTable.getBoxIndexFromVoxelCoord(Vec3i(i, j, k));
				if (idx != -1){
					for (int xyz = 0; xyz < 3; xyz++){
						Vec3i nb(i, j, k);
						nb[xyz] ++;

						int idxN = hashTable.getBoxIndexFromVoxelCoord(nb);
						if (idxN != -1 && idx != idxN){
							neighborVoxel[idx].push_back(idxN);
							neighborVoxel[idxN].push_back(idx);
						}
					}
				}
			}
		}
	}
}

void groupCutManager::constructVolxeHash()
{
	leftDownVoxel = m_octree->m_root->leftDownTight;
	rightUpVoxel = m_octree->m_root->rightUpTight;
	Vec3f sizef = rightUpVoxel - leftDownVoxel;
	voxelSize = m_octree->boxSize;

	for (int i = 0; i < 3; i++)
	{
		NumXYZ[i] = (sizef[i] / voxelSize);
	}

	hashTable.leftDown = leftDownVoxel;
	hashTable.rightUp = rightUpVoxel;
	hashTable.voxelSize = voxelSize;
	hashTable.NumXYZ = NumXYZ;
	hashTable.boxes = &boxes;

	loadVoxelArray();
}

void groupCutManager::loadMeshBox(char *filePath)
{
	/* Load coordinate of center voxel*/
	FILE *f = fopen(filePath, "r");
	ASSERT(f);

	std::vector<arrayVec3f> voxList;
	std::vector<CStringA> names;

	int nbMesh;
	fscanf(f, "%d\n", &nbMesh);
	for (int i = 0; i < nbMesh; i++)
	{
		char name[50];
		fscanf(f, "%s\n", name);
		names.push_back(CStringA(name));

		int nbV;
		fscanf(f, "%d\n", &nbV);
		arrayVec3f pts;
		for (int j = 0; j < nbV; j++)
		{
			Vec3f curp;
			fscanf(f, "%f %f %f\n", &curp[0], &curp[1], &curp[2]);
			pts.push_back(curp);
		}

		voxList.push_back(pts);
	}

	fclose(f);

	/* Create mesh box from those points
		Because it is float coordinate, we have to find the index*/
	for (int i = 0; i < voxList.size(); i++)
	{
		arrayInt vIdxs;
		arrayVec3f ptf = voxList[i];

		for (int j = 0; j < ptf.size(); j++)
		{
			int idx = hashTable.getBoxIndexFromCoord(ptf[j]);
			vIdxs.push_back(idx);
		}

		// create bvh mesh
		bvhVoxel newMesh;
		newMesh.boneName = CString(names[i]);
		newMesh.voxelIdxs = vIdxs;
		newMesh.boxes = &boxes;
		newMesh.initOther();
		meshBoxes.push_back(newMesh);

		// Init additional information of the mesh
		// Bounding, corresponding bone ...
	}
}

void groupCutManager::draw()
{
	static arrayVec3f color = Util_w::randColor(6);
	mirrorDraw mirror(0, s_octree->centerMesh[0]);

	// Grid mesh box
	for (int i = 0; i < s_meshBoxes->size(); i++){
		glColor3fv(color[i].data());
	//	(*s_meshBoxes)[i]->drawVoxels(&mirror, 0);
	}

	// Group array
	glLineWidth(3.0);
		boneGroupArray[curBoneIdx].drawPose(idx1, idx2);
	glLineWidth(1.0);
}

void groupCutManager::updateDisplay(int yIdx, int zIdx)
{
	idx1 = yIdx;
	idx2 = zIdx;
}

void groupCutManager::showDialog(CWnd* parent)
{
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	CView * pView = pFrame->GetActiveView();

	m_dlg = new cutBoneGroupDlg(pView);
	m_dlg->Create(cutBoneGroupDlg::IDD, pView);
	m_dlg->Init(this);
	m_dlg->ShowWindow(SW_SHOW);
}

// Called when need to update pose by dialog
int groupCutManager::updatePoseConfigurationIdx(int poseIdx, int nodeIdx)
{
	if (poseIdx < 0 || poseIdx >= boneGroupArray[curBoneIdx].boxPose.sortedPoseMap.size()){
		idx1 = 0;
		idx2 = 0;
		return -1;
	}

	idx1 = poseIdx;
	neighborPose* pp = boneGroupArray[curBoneIdx].boxPose.sortedPoseMap.at(idx1);
	if (nodeIdx < 0 
		|| nodeIdx >= boneGroupArray[curBoneIdx].boxPose.sortedPoseMap.at(poseIdx)->nodeGroupBoneCut.size()){
		idx2 = pp->smallestErrorIdx;
	}
	else {
		idx2 = nodeIdx;
	}

	m_dlg->updateDisplayedOverallError(pp->nodeGroupBoneCut[idx2]->nodeScore);
	m_dlg->updateDisplayedVolumeError(pp->nodeGroupBoneCut[idx2]->volError);
	m_dlg->updateDisplayedHashError(pp->nodeGroupBoneCut[idx2]->hashRank);
	m_dlg->updateDisplayedCBError(pp->nodeGroupBoneCut[idx2]->CBError);

	std::cout << pp->nodeGroupBoneCut[idx2]->posID << std::endl;

	return idx2;
}

// MainControl calls this method
void groupCutManager::initFromSwapBox(detailSwapManager * m_swapMngr)
{
	s_meshBoxes = &m_swapMngr->meshBox;
	voxelSize = s_octree->boxSize;

	std::vector<bone*> groupBone;
	s_skeleton->getGroupBone(s_skeleton->m_root, groupBone);
//	cprintf("groupcutmanager groupbone size: %d\n", groupBone.size());

	std::vector<bvhVoxel*> meshBox = m_swapMngr->meshBox;

	for (int i = 0; i < groupBone.size(); i++){
		// Create new group cut
		groupCut newGroup;
		newGroup.neighborVoxel = s_boxShareFaceWithBox;
		newGroup.boxes = s_boxes;
		newGroup.voxelSize = s_octree->boxSize;

		// Bone group
		std::vector<bone*> boneInGroup;
		s_skeleton->getBoneInGroup(groupBone[i], boneInGroup);
		newGroup.bones = boneInGroup;

		// Corresponding mesh box
		// Find by name
		CString name = groupBone[i]->m_name;
		for (int j = 0; j < meshBox.size(); j++){
			// If the name matches
			if (name.CompareNoCase(meshBox[j]->boneName) == 0){
				newGroup.sourcePiece = meshBox[j];
				newGroup.voxelIdxs = meshBox[j]->voxelIdxs;
				break;
			}
		}

		// Assign to array
		boneGroupArray.push_back(newGroup);
		m_idxChoosen.push_back(Vec2i(-1, -1));
	}

	//cprintf("boneGroupArray size: %d\n", boneGroupArray.size());
	for (int i = 0; i < boneGroupArray.size(); i++){
		groupCut *gc = &boneGroupArray[i];

		std::vector<Vec2i> neighbor;
		s_skeleton->getNeighborPair(groupBone[i], neighbor, boneGroupArray[i].bones);
		gc->boxPose.boneArray = &gc->bones;
		gc->boxPose.neighborInfo = neighbor;
		gc->boxPose.init();
		gc->boxPose.voxelSizef = voxelSize;
	}

	computeVolumeRatioInGroup();

	// Init tree
	for (int i = 0; i < boneGroupArray.size(); i++){
		boneGroupArray[i].constructTree(); // stuck here at third i
	}
}

void groupCutManager::updateAcceptedIndexes(std::vector<Vec2i> idxChoosen)
{
	m_idxChoosen = idxChoosen;
}

void groupCutManager::getConfiguration(int boneGroupIdx, std::vector<bone*>& boneInGroup, std::vector<meshPiece>& cutBoxByBoneOrder)
{
	groupCut *curG = &boneGroupArray[boneGroupIdx];
	Vec2i poseIdx = m_idxChoosen[boneGroupIdx];

	neighborPose* curPose = curG->boxPose.sortedPoseMap.at(poseIdx[0]);
	groupCutNode *node = curPose->nodeGroupBoneCut[poseIdx[1]];

	std::map<int, int> boneMeshIdx = curPose->mapBone_meshIdx[poseIdx[1]];
	std::vector<meshPiece> cutBoxf = node->boxf;
	std::vector<bone*> boneArray = curG->bones;

	// Re order the cut box
	std::vector<meshPiece> orderedBox;
	for (int i = 0; i < boneArray.size(); i++)
	{
		int meshIdx = boneMeshIdx[i];
		orderedBox.push_back(cutBoxf[meshIdx]);
	}

	// assign
	boneInGroup = boneArray;
	cutBoxByBoneOrder = orderedBox;
}

void groupCutManager::updateRealTime()
{
	curBoneIdx = m_dlg->boneGoupListBox.GetCurSel();

	CString stext;
	m_dlg->poseCurIdxText.GetWindowText(stext);
	idx1 = _ttoi(stext);

	m_dlg->curIdxInPoseText.GetWindowText(stext);
	idx2 = StrToInt(stext);
}

void groupCutManager::computeVolumeRatioInGroup()
{
	for (size_t i = 0; i < boneGroupArray.size(); i++)
	{
		std::vector<bone*> bones = boneGroupArray[i].bones;
		float totalV = 0;
		for (auto b : bones){
			totalV += b->m_volumef;
		}

		for (auto b : bones){
			b->m_volumeRatioInGroup = b->m_volumef / totalV;
		}
	}
}
 
void groupCutManager::performEvaluations(std::vector<std::vector<int>> idealHashes){
	m_weights = Vec3f(1, 0, 0);

	for (int i = 0; i < boneGroupArray.size(); i++){
		// Get the bone, eg. arm or leg
		groupCut *curG = &boneGroupArray.at(i);
		curG->boxPose.getPoseMapIntoVectorForm();

		for (int j = 0; j < curG->boxPose.sortedPoseMap.size(); j++){
			// Get different poses of the arm/leg
			neighborPose* curP = curG->boxPose.sortedPoseMap.at(j);

			for (int k = 0; k < curP->nodeGroupBoneCut.size(); k++){
				// Get different configurations/nodes within a pose of an arm/leg
				curG->calculateNodeErrors(j, k, idealHashes.at(i));
			}
			
		}
		// Sort nodes within pose
		curG->sortEvaluations(m_weights);

		// Sort poses		
		std::sort(curG->boxPose.sortedPoseMap.begin(), curG->boxPose.sortedPoseMap.end(), comparePoseScore());
	}
}

void groupCutManager::updateSortEvaluations(){
	m_weights = m_dlg->weights;

	for (int i = 0; i < boneGroupArray.size(); i++){
		// Get the bone, eg. arm or leg
		groupCut *curG = &boneGroupArray.at(i);
		curG->sortEvaluations(m_weights);
		
		// Sort poses		
		std::sort(curG->boxPose.sortedPoseMap.begin(), curG->boxPose.sortedPoseMap.end(), comparePoseScore());
	}
}

std::vector<meshPiece> groupCutManager::getBoxesToDrawOnSkeleton(){
	neighborPose* pp = boneGroupArray[curBoneIdx].boxPose.sortedPoseMap.at(idx1);

	if (idx1 < 0 || idx1 >= boneGroupArray[curBoneIdx].boxPose.sortedPoseMap.size()){
		idx1 = 0;
		idx2 = pp->smallestErrorIdx;
	}
	
	if (idx2 < 0
		|| idx2 >= boneGroupArray[curBoneIdx].boxPose.sortedPoseMap.at(idx2)->nodeGroupBoneCut.size()){
		idx2 = pp->smallestErrorIdx;
	}

	return pp->nodeGroupBoneCut[idx2]->boxf;
}

std::vector<meshPiece> groupCutManager::getBoxesToDrawOnSkeleton(int bone){
	neighborPose* pp = boneGroupArray[bone].boxPose.sortedPoseMap.at(m_idxChoosen.at(bone)[0]);
	return pp->nodeGroupBoneCut[m_idxChoosen.at(bone)[1]]->boxf;
}