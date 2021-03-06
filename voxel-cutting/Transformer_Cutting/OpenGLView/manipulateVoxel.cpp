﻿#include "stdafx.h"
#include "manipulateVoxel.h"
#include "neighbor.h"
#include "detailSwapManager.h"


manipulateVoxel::manipulateVoxel()
{
}


manipulateVoxel::~manipulateVoxel()
{
	for (int i = 0; i < meshBox.size(); i++)
	{
		delete meshBox[i];
	}
}

void manipulateVoxel::loadFromFile(){
	std::vector<arrayInt> meshIdx;

	// Load voxel box indexes of cut mesh
	{
		FILE* f = fopen("../stateFinalSwap_meshBoxIdxs.txt", "r");
		ASSERT(f);
		int nbMesh;
		fscanf(f, "%d\n", &nbMesh);
		for (int i = 0; i < nbMesh; i++)
		{
			arrayInt idxs;
			int nbv;
			fscanf(f, "%d\n", &nbv);
			idxs.resize(nbv);
			for (int j = 0; j < nbv; j++)
			{
				fscanf(f, "%d\n", &idxs[j]);
			}
			meshIdx.push_back(idxs);
		}
		fclose(f);
	}

	// Load bone order
	boneArray.clear();
	std::vector<CString> name;
	coords.clear();

	{
		FILE *f = fopen("../stateFinalSwap_boneArray.txt", "r");
		int nbBone;
		fscanf(f, "%d\n", &nbBone);
		coords.resize(nbBone);
		for (int i = 0; i < nbBone; i++)
		{
			char tmp[50];
			fscanf(f, "%s\n", tmp);
			name.push_back(CString(tmp));
			fscanf(f, "%d %d %d\n", &coords[i][0], &coords[i][1], &coords[i][2]);
		}

		std::vector<bone*> bones;
		s_skeleton->getSortedBoneArray(bones);
		for (int i = 0; i < name.size(); i++)
		{
			CString n = name[i];
			for (int j = 0; j < bones.size(); j++)
			{
				if ((bones[j]->m_name.Compare(n)) == 0)
				{
					boneArray.push_back(bones[j]);
					break;
				}
			}
		}
	}

	// Now init
	std::vector<voxelBox> *s_boxes = &s_voxelObj->m_boxes;
	float voxelSize = s_voxelObj->m_voxelSizef;
	hashVoxel *s_hashTable = &s_voxelObj->m_hashTable;

	for (int i = 0; i < meshIdx.size(); i++)
	{
		voxelList* newMeshBox = new voxelList;
		newMeshBox->m_voxelIdxs = meshIdx[i];
		meshBox.push_back(newMeshBox);

		// Bone info
		newMeshBox->s_voxelObj = s_voxelObj;
		newMeshBox->s_corressBondP = boneArray[i];
		newMeshBox->init();
	}

	// Volume ratio 
	float totalBoneVol = 0;
	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->m_volumeRatio = boneArray[i]->getVolumef();
		totalBoneVol += boneArray[i]->getVolumef();
	}
	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->m_volumeRatio /= totalBoneVol;
	}

	// Update the bone index in voxel set
	for (int i = 0; i < meshBox.size(); i++)
	{
		arrayInt voxelIdx = meshBox[i]->m_voxelIdxs;

		for (int j = 0; j < voxelIdx.size(); j++)
		{
			s_boxes->at(voxelIdx[j]).boneIndex = i;
		}
	}

	// Init other
	assignBoneSizeToMesh();
	m_hashBoxIdx.resize(s_boxes->size());
	std::fill(m_hashBoxIdx.begin(), m_hashBoxIdx.end(), -1);
}

void manipulateVoxel::assignBoneSizeToMesh()
{
	for (int i = 0; i < meshBox.size(); i++)
	{
		Vec3f boneLocalSize = boneArray[i]->m_sizef;
		Vec3f sizeInWord;
		Vec3i coordMap = coords[i];

		for (int j = 0; j < 3; j++)
		{
			sizeInWord[coordMap[j]] = boneLocalSize[j];
		}

		meshBox[i]->m_BoxSize3F = sizeInWord;
	}
}

void manipulateVoxel::draw(BOOL mode[10])
{


	if (mode[4]) // draw the scaled mesh
	{
		drawBoxInterfere();
	}
	if (mode[5])
	{
		drawVoxelBox();
	}
}

void manipulateVoxel::updateParam(int idx1, int idx2)
{
	curScaleF = (float)idx1 / 100.0;

	// update the list voxels inside the box right away
	hashVoxel *hashTable = &s_voxelObj->m_hashTable;
	std::fill(m_hashBoxIdx.begin(), m_hashBoxIdx.end(), -1);
	for (int i = 0; i < meshBox.size(); i++)
	{
		voxelList * curList = meshBox[i];
		
		Vec3f center = meshBox[i]->centerPoint();
		Vec3f size3F = meshBox[i]->m_BoxSize3F*curScaleF;

		Vec3f ld3F = center - size3F / 2;
		Vec3f ru3F = center + size3F / 2;

		Vec3f meshLD = s_voxelObj->m_hashTable.leftDown;
		Vec3i ld3I = Util_w::XYZ2IJK(ld3F - meshLD, s_voxelObj->m_voxelSizef);
		Vec3i ru3I = Util_w::XYZ2IJK(ru3F - meshLD, s_voxelObj->m_voxelSizef);

		for (int ii = ld3I[0]; ii <= ru3I[0]; ii++)
		{
			for (int jj = ld3I[1]; jj <= ru3I[1]; jj++)
			{
				for (int kk = ld3I[2]; kk <= ru3I[2]; kk++)
				{
					Vec3i curPI = Vec3i(ii, jj, kk);
					int idx = hashTable->getBoxIndexFromVoxelCoord(curPI);
					if (idx != -1)
					{
						m_hashBoxIdx[idx] = i;
					}
				}
			}
		}
	}
}

void manipulateVoxel::drawBoxInterfere()
{
	static arrayVec3f color3Fif = Util_w::randColor(30);

	// Draw bounding box
	for (int i = 0; i < meshBox.size(); i++)
	{
		Vec3f center = m_centerBox[i];
		Vec3f size3F = meshBox[i]->m_BoxSize3F*curScaleF;

		glPushMatrix();
		glColor3fv(color3Fif[i].data());
		glTranslatef(center[0], center[1], center[2]);
		Util_w::drawBoxWireFrame(-size3F / 2, size3F / 2);
		glPopMatrix();
	}

	// Draw potential color
	std::vector<voxelBox> * boxes = &s_voxelObj->m_boxes;
	for (int i = 0; i < m_hashBoxIdx.size(); i++)
	{
		int idx = m_hashBoxIdx[i];
		if (m_hashBoxIdx[i] != -1)
		{
			voxelBox curB = boxes->at(i);
			glColor3fv(color3Fif[idx].data());
			Util_w::drawBoxWireFrame(curB.leftDown, curB.rightUp);

			glColor3fv(color3Fif[idx+1].data());
			Util_w::drawBoxSurface(curB.leftDown, curB.rightUp);
		}
	}
}

void manipulateVoxel::resolveVoxelBox()
{
	std::vector<voxelBox> * boxes = &s_voxelObj->m_boxes;
	Vec3f centerMesh = s_voxelObj->m_centerf;

	m_voxelState = markVoxelHash();

	// 1. Resolve the voxel
	boxVoxelIdxs.resize(meshBox.size());

	// 2. Assign the voxel the belong to one box
	// Resolve the voxel that does not belong to any box
	arrayInt voxelInConflict;
	arrayInt freeVoxel;
	for (int i = 0; i < m_voxelState.size(); i++)
	{
		// We consider symmetric
		if (boxes->at(i).center[0] > centerMesh[0])
		{
			continue;
		}

		arrayInt boxList = m_voxelState[i];
		if (boxList.size() == 1) // Belong to one box
		{
			boxVoxelIdxs[boxList[0]].push_back(i);
			m_hashBoxIdx[i] = boxList[0];
		}
		else if (boxList.size() == 0) // Does not belong to any
		{
			// Resolve by distance to box face or aspect ratio
			// We use distance to box face first
			freeVoxel.push_back(i);
		}
		else
			voxelInConflict.push_back(i);
	}

	// 3. resolve the voxel that belong to multi box
	// We assign the box to mesh that have the largest error
	std::vector<arrayFloat> curError = getCurrentErrorOfBox(boxVoxelIdxs);
	for (int i = 0; i < voxelInConflict.size(); i++)
	{
		// Assign to box with smallest size
		int vIdx = voxelInConflict[i];
		arrayInt boxIdxs = m_voxelState[vIdx];
		int idx = -1; float smallest = MAX;
		for (int j = 0; j < boxIdxs.size(); j++)
		{
			int curIdx = boxIdxs[j];
			arrayFloat curE = meshBox[curIdx]->getErrorAssumeVoxelList(boxVoxelIdxs[curIdx]);
			if (meshBox[curIdx]->m_volumeRatio < smallest)
			{
				smallest = meshBox[curIdx]->m_volumeRatio;
				idx = boxIdxs[j];
			}
		}

		ASSERT(idx != -1);
		boxVoxelIdxs[idx].push_back(vIdx);
		m_hashBoxIdx[vIdx] = idx;
	}

	// 4. Resolve free voxel
	while (freeVoxel.size() > 0)
	{
		for (auto it = freeVoxel.begin(); it != freeVoxel.end();)
		{
			int boxIdx = getNearestNeighborBox(*it);
			if (boxIdx != -1)
			{
				boxVoxelIdxs[boxIdx].push_back(*it);
				m_hashBoxIdx[*it] = boxIdx;
				it = freeVoxel.erase(it);
			}
			else
				it++;
		}
	}

	// 5. Process if any object is not connected
//	while (true)
//	{
// 		bool found = false;
// 		for (auto i = 0; i < boxVoxelIdxs.size(); i++)
// 		{
// 			auto idxs = boxVoxelIdxs[i];
// 			// Check if the piece is unconnected
// 			// Merge small part that far from original box to other piece
// 			std::vector<arrayInt> independGroup = getIndependentGroup(boxes, idxs);
// 			if (independGroup.size() > 1)
// 			{
// 				found = true;
// 				mergeUnconnectedPiece(i, independGroup);
// 				break;
// 			}
// 		}
// 
// 		if (!found)
// 		{
// 			break;
// 		}
//	}

	// Update
	for (int i = 0; i < meshBox.size(); i++)
	{
		meshBox[i]->updateIdxs(boxVoxelIdxs[i]);
	}
}

std::vector<arrayInt> manipulateVoxel::getIndependentGroup(std::vector<voxelBox>* boxes, arrayInt idxs)
{
	int* mark = new int[boxes->size()];
	std::fill(mark, mark + boxes->size(), 0);
	for (int i = 0; i < idxs.size(); i++)
	{
		mark[idxs[i]] = 1;
	}

	int* hashvQ = new int[boxes->size()];
	std::fill(hashvQ, hashvQ + boxes->size(), 0);

	std::vector<arrayInt> out;
	arrayInt remain = idxs;
	while (remain.size() > 0)
	{
		arrayInt newObj;
		std::queue<int> vQ;
		vQ.push(remain[0]);

		while (!vQ.empty())
		{
			int idx = vQ.front();
			vQ.pop();
			newObj.push_back(idx);
			mark[idx] = 0; // No longer available

			// add all its neighbor
			arrayInt neighborN = s_voxelObj->m_boxShareFaceWithBox.at(idx);
			for (int j = 0; j < neighborN.size(); j++)
			{
				if (mark[neighborN[j]] == 1
					&& hashvQ[neighborN[j]] == 0)
				{
					hashvQ[neighborN[j]] = 1;
					vQ.push(neighborN[j]);
				}
			}
		}

		out.push_back(newObj);
		remain = Util_w::substractArrayInt(remain, newObj, boxes->size());
	}

	return out;
}

std::vector<arrayInt> manipulateVoxel::markVoxelHash()
{
	// list of mesh box that the voxel possibly lie in
	std::vector<voxelBox> * boxes = &s_voxelObj->m_boxes;

	std::vector<arrayInt> voxelState;
	voxelState.resize(boxes->size());

	hashVoxel *hashTable = &s_voxelObj->m_hashTable;
	std::fill(m_hashBoxIdx.begin(), m_hashBoxIdx.end(), -1);
	for (int i = 0; i < meshBox.size(); i++)
	{
		voxelList * curList = meshBox[i];

		Vec3f center = m_centerBox[i];
		Vec3f size3F = meshBox[i]->m_BoxSize3F*curScaleF;

		Vec3f ld3F = center - size3F / 2;
		Vec3f ru3F = center + size3F / 2;

		Vec3f meshLD = s_voxelObj->m_hashTable.leftDown;
		Vec3i ld3I = Util_w::XYZ2IJK(ld3F - meshLD, s_voxelObj->m_voxelSizef);
		Vec3i ru3I = Util_w::XYZ2IJK(ru3F - meshLD, s_voxelObj->m_voxelSizef);

		for (int ii = ld3I[0]; ii <= ru3I[0]; ii++)
		{
			for (int jj = ld3I[1]; jj <= ru3I[1]; jj++)
			{
				for (int kk = ld3I[2]; kk <= ru3I[2]; kk++)
				{
					Vec3i curPI = Vec3i(ii, jj, kk);
					int idx = hashTable->getBoxIndexFromVoxelCoord(curPI);
					if (idx != -1)
					{
						voxelState[idx].push_back(i);
					}
				}
			}
		}
	}

	return voxelState;
}

int manipulateVoxel::getNearestBox(int voxelIdx)
{
	std::vector<voxelBox> *boxes = &s_voxelObj->m_boxes;

	float smallest = MAX; int idxSmallest;
	for (int i = 0; i < meshBox.size(); i++)
	{
		Vec3f center = meshBox[i]->centerPoint();
		Vec3f size3F = meshBox[i]->m_BoxSize3F*curScaleF;
		Vec3f ld3F = center - size3F / 2;
		Vec3f ru3F = center + size3F / 2;

		GeometricFunc geoFunc;
		float dis = geoFunc.disBtwPointAndBox(Box(ld3F, ru3F), boxes->at(voxelIdx).center);

		if (dis < smallest)
		{
			smallest = dis;
			idxSmallest = i;
		}
	}

	return idxSmallest;
}

std::vector<arrayFloat> manipulateVoxel::getCurrentErrorOfBox(const std::vector<arrayInt> &boxVoxelIdxs)
{
	std::vector<arrayFloat> out;
	for (int i = 0; i < meshBox.size(); i++)
	{
		out.push_back(meshBox[i]->getErrorAssumeVoxelList(boxVoxelIdxs[i]));
	}
	return out;
}

void manipulateVoxel::drawVoxelBox()
{
	static arrayVec3f color_voxel = Util_w::randColor(30);
	for (auto i = 0; i < meshBox.size(); i++)
	{
		glColor3fv(color_voxel[i].data());
		meshBox[i]->drawWire();
		glColor3fv(color_voxel[i + 1].data());
		meshBox[i]->drawFace();
	}
}

std::vector<arrayInt> manipulateVoxel::getListOfVoxelIdxs()
{
	std::vector<arrayInt> out;
	hashVoxel * hashTable = &s_voxelObj->m_hashTable;
	for (int i = 0; i < meshBox.size(); i++)
	{
		if (meshBox[i]->s_corressBondP->m_type == CENTER_BONE)
		{
			arrayInt allIdxs = meshBox[i]->m_voxelIdxs;
			for (int j = 0; j < meshBox[i]->m_voxelIdxs.size(); j++)
			{
				int idx = hashTable->getSymmetricBox(meshBox[i]->m_voxelIdxs[j]);
				if (idx != -1)
				{
					allIdxs.push_back(idx);
				}
			}
			out.push_back(allIdxs);
		}
		else
		{
			out.push_back(meshBox[i]->m_voxelIdxs);
		}

	}

	return out;
}

void manipulateVoxel::cutCenterBoxByHalf()
{
	std::vector<voxelBox> * s_boxes = &s_voxelObj->m_boxes;
	for (int i = 0; i < meshBox.size(); i++)
	{
		voxelList* curM = meshBox[i];
		if (curM->s_corressBondP->m_type == CENTER_BONE)
		{
			arrayInt vIdxs = curM->m_voxelIdxs;
			arrayInt newVIdxs;
			for (int j = 0; j < vIdxs.size(); j++)
			{
				if (s_boxes->at(vIdxs[j]).center[0] < s_voxelObj->m_centerf[0])
				{
					newVIdxs.push_back(vIdxs[j]);
				}
			}
			curM->m_voxelIdxs = newVIdxs;
			curM->updateBoundingBox();

		}
	}

	// Update bounding box


	// Update bone index in box
	for (int i = 0; i < s_boxes->size(); i++)
	{
		s_boxes->at(i).boneIndex = -1;
	}

	for (int i = 0; i < meshBox.size(); i++)
	{
		arrayInt vIdx = meshBox[i]->m_voxelIdxs;
		for (int j = 0; j < vIdx.size(); j++)
		{
			s_boxes->at(vIdx[j]).boneIndex = i;
		}
	}
}

int manipulateVoxel::getNearestNeighborBox(int voxelIdx)
{
	arrayInt vShareFace = s_voxelObj->m_boxShareFaceWithBox[voxelIdx];
	int boxIdxNeareast = -1;
	float nearest = MAX;
	std::vector<voxelBox> *boxes = &s_voxelObj->m_boxes;

	for (auto nbIdx : vShareFace)
	{
		arrayInt listMeshIdxs = m_voxelState[nbIdx];

		if (m_hashBoxIdx[nbIdx] != -1)
		{
			listMeshIdxs.push_back(m_hashBoxIdx[nbIdx]);
		}

		for (auto mIdx : listMeshIdxs)
		{
			// Compute distance
			Vec3f center = meshBox[mIdx]->centerPoint();
			Vec3f size3F = meshBox[mIdx]->m_BoxSize3F*curScaleF;
			Vec3f ld3F = center - size3F / 2;
			Vec3f ru3F = center + size3F / 2;

			GeometricFunc geoFunc;
			float dis = geoFunc.disBtwPointAndBox(Box(ld3F, ru3F), boxes->at(voxelIdx).center);

			if (dis < nearest)
			{
				nearest = dis;
				boxIdxNeareast = mIdx;
			}
		}
	}

	return boxIdxNeareast;
}

void manipulateVoxel::mergeUnconnectedPiece(int meshIdx, std::vector<arrayInt> independGroup)
{
	// 1. Find the piece that we should keep
	int keepIdx = -1;
	float nearest = MAX;
	for (int j = 0; j < independGroup.size(); j++)
	{
		float dis = averageDistanceToMesh(independGroup[j], meshIdx);
		if (dis < nearest)
		{
			nearest = dis;
			keepIdx = j;
		}
	}

	ASSERT(keepIdx != -1);
	boxVoxelIdxs[meshIdx] = independGroup[keepIdx];

	// 2. merge other
	for (unsigned int i = 0; i < independGroup.size(); i++)
	{
		if (i == keepIdx)
		{
			continue;
		}

		arrayInt idxs = independGroup[i];
		arrayInt neighborMesh = findNeighborMesh(meshIdx, idxs);
		ASSERT(neighborMesh.size() > 0);
		// To mesh that has highest error
		int meshToMergeIdx = -1;
		float maxError = 0;
		for (int j = 0; j < neighborMesh.size(); j++)
		{
			int meshNbIdx = neighborMesh[j];
			arrayInt curIdxs = boxVoxelIdxs[meshNbIdx];
			curIdxs.insert(curIdxs.end(), idxs.begin(), idxs.end());
			arrayFloat curE = meshBox[meshNbIdx]->getErrorAssumeVoxelList(curIdxs);

			if (curE[VOLUME_ERROR] > maxError)
			{
				maxError = curE[VOLUME_ERROR];
				meshToMergeIdx = meshNbIdx;
			}
		}

		// Merge
		ASSERT(meshToMergeIdx != -1);
		arrayInt curIdxs = boxVoxelIdxs[meshToMergeIdx];
		curIdxs.insert(curIdxs.end(), idxs.begin(), idxs.end());

		boxVoxelIdxs[meshToMergeIdx] = curIdxs;

		for (auto id : idxs)
		{
			ASSERT(m_hashBoxIdx[id] == meshIdx);
			m_hashBoxIdx[id] = meshToMergeIdx;
		}
	}
}

arrayInt manipulateVoxel::findNeighborMesh(int sourceIdx, arrayInt idxs)
{
	arrayInt meshHash(boxVoxelIdxs.size(), -1);
	for (auto id : idxs)
	{
		arrayInt neighborVoxel = s_voxelObj->m_boxShareFaceWithBox[id];
		for (auto nb : neighborVoxel)
		{
			int meshIdx = m_hashBoxIdx[nb];
			if (meshIdx != -1 && meshIdx != sourceIdx)
			{
				meshHash[meshIdx] = 1;
			}
		}
	}

	arrayInt meshIdxNb;
	for (int i = 0; i < meshHash.size(); i++)
	{
		if (meshHash[i] != -1)
		{
			meshIdxNb.push_back(i);
		}
	}

	return meshIdxNb;
}

float manipulateVoxel::averageDistanceToMesh(arrayInt idxs, int meshIdx)
{
	std::vector<voxelBox> *boxes = & s_voxelObj->m_boxes;
	float dis = 0;
	Vec3f meshCenter = meshBox[meshIdx]->centerPoint();
	for (auto id : idxs)
	{
		dis += (meshCenter - boxes->at(id).center).norm();
	}

	return dis / idxs.size();
}

void manipulateVoxel::updateBox(int percent, arrayVec3f centerBox)
{
	curScaleF = (float)percent / 100.0;
	m_centerBox = centerBox;
	ASSERT(centerBox.size() == meshBox.size());

	// update the list voxels inside the box right away
	hashVoxel *hashTable = &s_voxelObj->m_hashTable;
	std::fill(m_hashBoxIdx.begin(), m_hashBoxIdx.end(), -1);
	for (int i = 0; i < meshBox.size(); i++)
	{
		voxelList * curList = meshBox[i];

		Vec3f center = centerBox[i];
		Vec3f size3F = meshBox[i]->m_BoxSize3F*curScaleF;

		Vec3f ld3F = center - size3F / 2;
		Vec3f ru3F = center + size3F / 2;

		Vec3f meshLD = s_voxelObj->m_hashTable.leftDown;
		Vec3i ld3I = Util_w::XYZ2IJK(ld3F - meshLD, s_voxelObj->m_voxelSizef);
		Vec3i ru3I = Util_w::XYZ2IJK(ru3F - meshLD, s_voxelObj->m_voxelSizef);

		for (int ii = ld3I[0]; ii <= ru3I[0]; ii++)
		{
			for (int jj = ld3I[1]; jj <= ru3I[1]; jj++)
			{
				for (int kk = ld3I[2]; kk <= ru3I[2]; kk++)
				{
					Vec3i curPI = Vec3i(ii, jj, kk);
					int idx = hashTable->getBoxIndexFromVoxelCoord(curPI);
					if (idx != -1)
					{
						m_hashBoxIdx[idx] = i;
					}
				}
			}
		}
	}
}

void manipulateVoxel::drawCoord(int boneIdx) const
{
	float length = s_voxelObj->m_voxelSizef * 3;
	Vec3f c = m_centerBox[boneIdx];

	arrayVec3f coords = { Vec3f(1, 0, 0), Vec3f(0, 1, 0), Vec3f(0, 0, 1) };

	glBegin(GL_LINES);
	for (auto co : coords)
	{
		glColor3fv(co.data());
		glVertex3fv(c.data());
		glVertex3fv((c + co*length).data());
	}

	glEnd();
}

