#include "StdAfx.h"
#include "skeleton.h"
#include "Utility_wrap.h"
#include "MeshCutting.h"
#include "neighbor.h"
#include "rapidxml_utils.hpp"
#include "myXML.h"

// XML key
#define BONE_KEY "bone"
#define PROPERTIES_KEY "bone_properties"
#define CHILD_KEY "children"
#define NAME_KEY "bone_name"
#define BONE_SIZE_KEY "bone_size"
#define JOINT_PARENT_KEY "joint_pos_parent"
#define JOINT_CHILD_KEY "joint_pos_child"
#define BONE_TYPE_KEY "bone_type"
#define ROTATION_ANGLE_KEY "rotation_about_xyz"

using namespace rapidxml;

skeleton::skeleton(void)
{
	m_root = NULL;
	meshScale = 1;
}

skeleton::~skeleton(void)
{
	if (m_root){
		delete m_root;
	}
}

void skeleton::getSortedBoneArray(std::vector<bone*> &sortedArray)
{
	getSortedBoneArrayRecur(m_root, sortedArray);
}

void skeleton::getSortedBoneArrayRecur(bone* node, std::vector<bone*> &sortedArray)
{
	sortedArray.push_back(node);

	for (size_t i = 0; i < node->child.size(); i++){
		getSortedBoneArrayRecur(node->child[i], sortedArray);
	}
}

void skeleton::getBoneAndNeighborInfo(std::vector<bone*> &boneArray, std::vector<std::pair<int,int>> &neighborA)
{
	getBoneAndNeighborInfoRecur(m_root, -1, boneArray, neighborA);
}

void skeleton::getBoneAndNeighborInfoRecur(bone* node, int parentIdx, std::vector<bone*> &boneArray, std::vector<std::pair<int,int>> &neighborA)
{
	boneArray.push_back(node);
	int idx = boneArray.size() -1;
	if (node->parent)
	{
		std::pair<int, int> nb(parentIdx, idx);
		neighborA.push_back(nb);
	}

	for (size_t i = 0; i < node->child.size(); i++)
	{
		getBoneAndNeighborInfoRecur(node->child[i], idx, boneArray, neighborA);
	}
}

void skeleton::groupBones()
{
	// Should root bone be considered bone group?
	//m_root->bIsGroup = true;

	for (int i = 0; i < m_root->child.size(); i++){
		bone* curChild = m_root->child[i];

		curChild->bIsGroup = true;

		// Update volume and volume ratio
		curChild->m_groupVolumef = volumeOfGroupBone(curChild);
		curChild->m_groupVolumeRatio = volumeRatioOfGroupBone(curChild);
	}
}

float skeleton::volumeOfGroupBone(bone* node)
{
	if (node->isLeaf()){
		return node->m_volumef;
	}

	float vol = 0;
	for (int i = 0; i < node->child.size(); i++){
		vol += volumeOfGroupBone(node->child[i]);
	}

	return vol + node->m_volumef;
}

float skeleton::volumeRatioOfGroupBone(bone* node)
{
	if (node->isLeaf()){
		return node->m_volumeRatio;
	}

	float vol = 0;
	for (int i = 0; i < node->child.size(); i++){
		vol += volumeOfGroupBone(node->child[i]);
	}

	return vol + node->m_volumeRatio;
}

void skeleton::getBoneGroupAndNeighborInfo(std::vector<bone*> &boneArray, std::vector<std::pair<int, int>> &neighborA)
{
	getBoneGroupAndNeighborInfoRecur(m_root, -1, boneArray, neighborA);
}

void skeleton::getBoneGroupAndNeighborInfoRecur(bone* node, int parentIdx, std::vector<bone*> & boneArray, std::vector<std::pair<int, int>> & neighborA)
{
	boneArray.push_back(node);
	int idx = boneArray.size() - 1;

	// If the bone has a parent
	if (node->parent){	
		std::pair<int, int> nb(parentIdx, idx);
		neighborA.push_back(nb);
	}

	// If the bone is a group
	if (node->bIsGroup){
		return;
	}

	for (size_t i = 0; i < node->child.size(); i++){
		getBoneGroupAndNeighborInfoRecur(node->child[i], idx, boneArray, neighborA);
	}
}

void skeleton::getSortedGroupBoneArray(std::vector<bone*> &sortedArray)
{
	getSortedBoneGroupArrayRecur(m_root, sortedArray);
}

void skeleton::getSortedBoneGroupArrayRecur(bone* node, std::vector<bone*> & sortedArray)
{
	sortedArray.push_back(node);
	for (int i = 0; i < node->child.size(); i++)
	{
		if (node->child[i]->bIsGroup)
		{
			sortedArray.push_back(node->child[i]);
		}
		else
		{
			getSortedBoneGroupArrayRecur(node->child[i], sortedArray);
		}
	}
}

void skeleton::getGroupBone(bone* node, std::vector<bone*> &groupBone)
{
	if (node->bIsGroup){
		groupBone.push_back(node);
		return;
	}

	for (int i = 0; i < node->child.size(); i++){
		getGroupBone(node->child[i], groupBone);
	}
}

void skeleton::getBoneInGroup(bone* node, std::vector<bone*> &boneInGroup)
{
	boneInGroup.push_back(node);
	for (int i = 0; i < node->child.size(); i++)
	{
		getBoneInGroup(node->child[i], boneInGroup);
	}
}

void skeleton::getNeighborPair(bone* node, std::vector<Vec2i> &neighbor, std::vector<bone*> boneArray)
{
	for (int i = 0; i < node->child.size(); i++){
		// Find the index
		std::vector<bone*>::iterator it = find(boneArray.begin(), boneArray.end(), node);
		ASSERT(it != boneArray.end());
		int idx1 = it - boneArray.begin();

		it = find(boneArray.begin(), boneArray.end(), node->child[i]);
		ASSERT(it != boneArray.end());
		int idx2 = it - boneArray.begin();

		neighbor.push_back(Vec2i(idx1, idx2));

		getNeighborPair(node->child[i], neighbor, boneArray);
	}
}

void skeleton::computeTempVar()
{
	std::vector<bone*> boneArray;
	getSortedBoneArray(boneArray);
	
	// Get total volume of all bones
	float vol = 0;
	for (int i = 0; i < boneArray.size(); i++){
		vol += boneArray[i]->m_volumef;
	}

	// Get ratio of individual bone to total bone volume 
	for (int i = 0; i < boneArray.size(); i++){
		boneArray[i]->m_volumeRatio = boneArray[i]->m_volumef / vol;
	}
}

void skeleton::writeToFile(char* filePath)
{
	myXML * doc = new myXML;
	myXMLNode * node = doc->addNode(BONE_KEY);
	writeBoneToXML(doc, node, m_root);

	doc->save(filePath);
	delete doc;
}

void skeleton::writeBoneToXML(myXML * doc, myXMLNode * node, bone* boneNode)
{
	// Write properties
	myXMLNode * pNode = doc->addNodeToNode(node, PROPERTIES_KEY);
 	doc->addElementToNode(pNode, NAME_KEY, std::string(CStringA(boneNode->m_name)));
 	doc->addVectorDatafToNode(pNode, BONE_SIZE_KEY, boneNode->m_sizef);
	doc->addVectorDatafToNode(pNode, JOINT_PARENT_KEY, boneNode->m_jointBegin);
	doc->addVectorDatafToNode(pNode, JOINT_CHILD_KEY, boneNode->m_posCoord);
	doc->addElementToNode(pNode, BONE_TYPE_KEY, boneNode->getTypeString());
	doc->addVectorDatafToNode(pNode, ROTATION_ANGLE_KEY, boneNode->m_angle);

	// Now children
	if (boneNode->child.size() > 0){
		myXMLNode * childNode = doc->addNodeToNode(node, CHILD_KEY);
		for (int i = 0; i < boneNode->child.size(); i++){
			myXMLNode * newChild = doc->addNodeToNode(childNode, BONE_KEY);
			writeBoneToXML(doc, newChild, boneNode->child[i]);
		}
	}
}

void skeleton::loadFromFile(char *filePath)
{
	myXML * doc = new myXML;
	doc->load(filePath);

	myXMLNode * rootNode = doc->first_node(BONE_KEY);

	m_root = new bone;
	m_root->estimatedCBLength = -1;
	loadBoneData(doc, rootNode, m_root);

	ASSERT(!rootNode->next_sibling());
	delete doc;
}

void skeleton::loadBoneData(myXML * doc, myXMLNode * xmlNode, bone* boneNode)
{
	// Load data to bone
	myXMLNode * properties = xmlNode->first_node(PROPERTIES_KEY);
	boneNode->m_posCoord = doc->getVec3f(properties, JOINT_CHILD_KEY);
	boneNode->m_jointBegin = doc->getVec3f(properties, JOINT_PARENT_KEY);
	boneNode->m_angle = doc->getVec3f(properties, ROTATION_ANGLE_KEY);
	boneNode->m_sizef = doc->getVec3f(properties, BONE_SIZE_KEY);
	boneNode->m_name = CString(doc->getStringProperty(properties, NAME_KEY).c_str());

	boneNode->setBoneType(doc->getStringProperty(properties, BONE_TYPE_KEY));
	boneNode->initOther();

	if (boneNode->parent != nullptr){
		Vec3f diff = boneNode->m_posCoord - boneNode->parent->m_posCoord;
		boneNode->estimatedCBLength = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
	}

	// Load child bone
	myXMLNode * child = xmlNode->first_node(CHILD_KEY);
	if (child){
		for (myXMLNode * nBone = child->first_node(BONE_KEY); nBone; nBone = nBone->next_sibling()){
			bone* newBone = new bone;
			newBone->parent = boneNode;
			loadBoneData(doc, nBone, newBone);

			boneNode->child.push_back(newBone);
		}
	}
}

float skeleton::getVolume()
{
	arrayBone_p allBones;
	getSortedBoneArray(allBones);
	float vol = 0;
	for (auto b:allBones)
	{
		vol += b->getVolumef();
	}

	return vol;
}

void skeleton::draw(int mode)
{
	if (m_root != nullptr){
		drawBoneRecursive(m_root, mode);
	}
}

void skeleton::drawBoneRecursive(bone* node, int mode, bool mirror)
{
	if (node == nullptr){
		return;
	}

	glPushMatrix();
		glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
		// Rotate global x-y-z, in GL, we do inverse
		glRotatef(node->m_angle[2], 0, 0, 1);// z
		glRotatef(node->m_angle[1], 0, 1, 0);// y
		glRotatef(node->m_angle[0], 1, 0, 0);// x

		node->draw(mode, meshScale, mirror);

		for (size_t i = 0; i < node->child.size(); i++){
			drawBoneRecursive(node->child[i], mode, mirror);

			if (node == m_root && node->child[i]->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
					glScalef(-1, 1, 1);
					drawBoneRecursive(node->child[i], mode, true);
				glPopMatrix();
			}
		}
	glPopMatrix();
}

void skeleton::drawGroup(int mode)
{
	if (m_root){
		drawGroupRecur(m_root, mode);
	}
}

void skeleton::drawGroupRecur(bone* node, int mode, bool mirror /*= false*/)
{
	if (node == nullptr)
		return;

	glPushMatrix();
	glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
	// Rotate global x-y-z
	// In GL, we do invert
	glRotatef(node->m_angle[2], 0, 0, 1);// z
	glRotatef(node->m_angle[1], 0, 1, 0);// y
	glRotatef(node->m_angle[0], 1, 0, 0);// x

	node->draw(mode, meshScale*node->groupShrink(), mirror);

	if (node == m_root)
	{
		for (size_t i = 0; i < node->child.size(); i++)
		{
			drawGroupRecur(node->child[i], mode, mirror);

			if (node == m_root && node->child[i]->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
					glScalef(-1, 1, 1);
					drawGroupRecur(node->child[i], mode, true);
				glPopMatrix();
			}
		}
	}

	glPopMatrix();
}

void skeleton::drawBoneWithMeshSize()
{
	ASSERT(m_root);
	drawBoneWithMeshSizeRecur(m_root);
}

void skeleton::drawBoneWithMeshSizeRecur(bone* node)
{
	if (node == nullptr)
		return;

	glPushMatrix();
	glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
	// Rotate global x-y-z
	// In GL, we do invert
	glRotatef(node->m_angle[2], 0, 0, 1);// z
	glRotatef(node->m_angle[1], 0, 1, 0);// y
	glRotatef(node->m_angle[0], 1, 0, 0);// x

	node->drawBoneWithMeshSize();

	for (size_t i = 0; i < node->child.size(); i++)
	{
		drawBoneWithMeshSizeRecur(node->child[i]);

		if (node == m_root && node->child[i]->m_type == TYPE_SIDE_BONE){
			glPushMatrix();
				glScalef(-1, 1, 1);
				drawBoneWithMeshSizeRecur(node->child[i]);
			glPopMatrix();
		}
	}
	glPopMatrix();
}

void skeleton::drawEstimatedGroupBox(std::vector<meshPiece> boxes){
	if (boxes.size() < 1){
		return;
	}

	std::sort(boxes.begin(), boxes.end(), compareBoneIndex());

	glPushMatrix();
		glTranslatef(m_root->m_posCoord[0], m_root->m_posCoord[1], m_root->m_posCoord[2]);
		glRotatef(m_root->m_angle[2], 0, 0, 1);// z
		glRotatef(m_root->m_angle[1], 0, 1, 0);// y
		glRotatef(m_root->m_angle[0], 1, 0, 0);// x

		glPushMatrix();
			Vec3f center = (m_root->leftDownf + m_root->rightUpf) / 2.0;
			glTranslatef(center[0], center[1], center[2]);

			rotateToMapCoord(boxes.at(0).mapCoord);

			glColor3f(0, 1, 0);
			m_root->drawEstimatedBox(Vec3f(), boxes.at(0).sizef);
		glPopMatrix();

		for (size_t i = 1; i < boxes.size(); i++){
			bone *node = m_root->child.at(i-1);
			
			glPushMatrix();
				glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
				glRotatef(node->m_angle[2], 0, 0, 1);// z
				glRotatef(node->m_angle[1], 0, 1, 0);// y
				glRotatef(node->m_angle[0], 1, 0, 0);// x

				glPushMatrix();
					Vec3f center = (node->leftDownf + node->rightUpf) / 2.0;
					glTranslatef(center[0], center[1], center[2]);

					rotateToMapCoord(boxes.at(i).mapCoord);

					glColor3f(0, 1, 0);
					node->drawEstimatedBox(Vec3f(), boxes.at(i).sizef);
				glPopMatrix();
			glPopMatrix();

			if (node->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
					glScalef(-1, 1, 1);
					glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
					glRotatef(node->m_angle[2], 0, 0, 1);// z
					glRotatef(node->m_angle[1], 0, 1, 0);// y
					glRotatef(node->m_angle[0], 1, 0, 0);// x
				
					glPushMatrix();
						Vec3f center = (node->leftDownf + node->rightUpf) / 2.0;
						glTranslatef(center[0], center[1], center[2]);

						rotateToMapCoord(boxes.at(i).mapCoord);

						node->drawEstimatedBox(Vec3f(), boxes.at(i).sizef);
					glPopMatrix();
				glPopMatrix();
			}
		}
	glPopMatrix();
}

void skeleton::drawEstimatedBoxesWithinGroup(int boneGroupIdx, std::vector<meshPiece> boxes, int mode){
	bone *groupBone = m_root->child.at(boneGroupIdx);
	
	std::sort(boxes.begin(), boxes.end(), compareBoneIndex());
	boxDrawingCount = 0;

	glPushMatrix();
		glTranslatef(m_root->m_posCoord[0], m_root->m_posCoord[1], m_root->m_posCoord[2]);
		glRotatef(m_root->m_angle[2], 0, 0, 1);// z
		glRotatef(m_root->m_angle[1], 0, 1, 0);// y
		glRotatef(m_root->m_angle[0], 1, 0, 0);// x

		drawBoxesWithinGroupRecur(groupBone, boxes, mode);
	glPopMatrix();

	boxDrawingCount = 0;
}

void skeleton::drawBoxesWithinGroupRecur(bone *node, std::vector<meshPiece> boxes, int mode){
	if (node == nullptr)
		return;

	glPushMatrix();
		glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
		// Rotate global x-y-z
		// In GL, we do invert
		glRotatef(node->m_angle[2], 0, 0, 1);// z
		glRotatef(node->m_angle[1], 0, 1, 0);// y
		glRotatef(node->m_angle[0], 1, 0, 0);// x

		glPushMatrix();
			Vec3f center = (node->leftDownf + node->rightUpf) / 2.0;
			glTranslatef(center[0], center[1], center[2]);

			rotateToMapCoord(boxes.at(boxDrawingCount).mapCoord);
			
			if (mode == 0){
				glColor3f(1, 0, 0);
			}
			else {
				glColor3f(0, 1, 0);
			}
			
			node->drawEstimatedBox(Vec3f(), boxes.at(boxDrawingCount).rightUp - boxes.at(boxDrawingCount).leftDown);
			boxDrawingCount++;
		glPopMatrix();

		for (size_t i = 0; i < node->child.size(); i++){
			drawBoxesWithinGroupRecur(node->child[i], boxes, mode);
		}
	glPopMatrix();
}

void skeleton::calculateIdealHashIds(){
	std::vector<bone*> groupedBones;
	getGroupBone(m_root, groupedBones);

	calculateIdealHashIdsRecur(groupedBones, 0, 0);

	for (int i = 0; i < groupedBones.size(); i++){
		bone *groupedBone = groupedBones.at(i);

		idealNodeHashIds.push_back(std::vector<int>());

		std::vector<bone*> temp;
		getChildrenWithinGroup(groupedBone, &temp);

		calculateIdealNodeHashIdsRecur(&temp, 0, 0, i);
	}

	for (int i = 0; i < idealNodeHashIds.size(); i++){
		for (int j = 0; j < idealNodeHashIds.at(i).size(); j++){
			std::cout << i << " " << idealNodeHashIds.at(i).at(j) << std::endl;
		}
	}
}

void skeleton::getChildrenWithinGroup(bone* node, std::vector<bone*> *b){
	if (node == nullptr){
		return;
	}

	if (node->parent != m_root){
		b->push_back(node);
	}
	
	for (size_t j = 0; j < node->child.size(); j++){
		getChildrenWithinGroup(node->child[j], b);
	}
}

void skeleton::calculateIdealHashIdsRecur(std::vector<bone*> groupedBones, int id, int GBIndex){
	if (GBIndex >= groupedBones.size()){
		idealHashIds.push_back(id);
		return;
	}
	
	int groupBoneSize = groupedBones.size() - 1;
	for (int j = 0; j < 3; j++){
		if (groupedBones[GBIndex]->m_posCoord[j] != 0){
			int gbIndexID;
			if (groupedBones[GBIndex]->m_posCoord[j] > 0){
				gbIndexID = j * 2 * pow(6.0, groupBoneSize - GBIndex);
			}
			else {
				gbIndexID = (j * 2 + 1) * pow(6.0, groupBoneSize - GBIndex);
			}
			calculateIdealHashIdsRecur(groupedBones, id + gbIndexID, GBIndex + 1);
		}
	}	
}

void skeleton::calculateIdealNodeHashIdsRecur(std::vector<bone*> *groupedBones, int id, int GBIndex, int i){
	if (GBIndex >= groupedBones->size()){
		idealNodeHashIds.at(i).push_back(id);
		return;
	}

	int groupBoneSize = groupedBones->size() - 1;
	for (int j = 0; j < 3; j++){
		if (groupedBones->at(GBIndex)->m_posCoord[j] != 0){
			int gbIndexID;
			if (groupedBones->at(GBIndex)->m_posCoord[j] > 0){
				gbIndexID = j * 2 * pow(6.0, groupBoneSize - GBIndex);
			}
			else {
				gbIndexID = (j * 2 + 1) * pow(6.0, groupBoneSize - GBIndex);
			}
			calculateIdealNodeHashIdsRecur(groupedBones, id + gbIndexID, GBIndex + 1, i);
		}
	}
}

void skeleton::rotateToMapCoord(Vec3f rotateCoord){
	if (rotateCoord == Vec3f(0, 1, 2)){
		// do nothing
	}
	else if (rotateCoord == Vec3f(0, 2, 1)){
		glRotatef(-90, 1, 0, 0);
	}
	else if (rotateCoord == Vec3f(1, 0, 2)){
		glRotatef(-90, 0, 0, 1);
	}
	else if (rotateCoord == Vec3f(1, 2, 0)){
		glRotatef(-90, 0, 1, 0);
		glRotatef(-90, 1, 0, 0);
	}
	else if (rotateCoord == Vec3f(2, 0, 1)){
		glRotatef(90, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
	}
	else if (rotateCoord == Vec3f(2, 1, 0)){
		glRotatef(90, 0, 1, 0);
	}
}

void skeleton::assignBoneIndex(){
	index = 0;
	assignBoneIndexRecur(m_root);
}

void skeleton::assignBoneIndexRecur(bone *node)
{
	if (node == nullptr){
		return;
	}

	node->m_index = index++;

	for (size_t i = 0; i < node->child.size(); i++){
		assignBoneIndexRecur(node->child[i]);
	}
}


bone::~bone()
{
	for (size_t i = 0; i < child.size(); i++){
		delete child[i];
	}

	if (mesh){
		delete mesh;
	}
}

bone::bone()
{
	mesh = nullptr;
	indexOfMeshBox = -1;
	bIsGroup = false;
	parent = nullptr;
}

void bone::initOther()
{
	leftDownf = Vec3f(-m_sizef[0] / 2, -m_sizef[1] / 2, 0);
	rightUpf = Vec3f(m_sizef[0] / 2, m_sizef[1] / 2, m_sizef[2]);
	m_volumef = m_sizef[0] * m_sizef[1] * m_sizef[2];
}

BOOL bone::isLarger(bone* a)
{
	if (a->m_type == TYPE_CENTER_BONE && a->m_type == TYPE_SIDE_BONE)
	{
		return true;
	}

	return m_volumef > a->m_volumef;
}

Vec3i normalizedVector(Vec3i sizei)
{
	int lengthOrder;
	// orientation
	int idxL, idxS, L = MIN, S = MAX;
	for (int i = 0; i < 3; i++)
	{
		if (L < sizei[i])
		{
			idxL = i; L = sizei[i];
		}
		if (S > sizei[i])
		{
			idxS = i; S = sizei[i];
		}
	}

	if (L == S) // Cube; original is OK
	{
		return Vec3i(1,2,4);
	}
	else
	{
		int idxM = -1;
		for (int i = 0; i < 3; i++)
		{
			if ((sizei[i] != L && i!=idxS)
				|| (sizei[i] != S && i!=idxL))
			{
				idxM = i;
				break;
			}
		}
		Vec3i normalized;
		normalized[idxS] = 1;
		normalized[idxM] = 2;
		normalized[idxL] = 4;

		return normalized;
	}
}

Vec3i normalizedVector(Vec3f sizef)
{
	int lengthOrder;
	// orientation
	int idxL, idxS;
	float L = MIN, S = MAX;
	for (int i = 0; i < 3; i++)
	{
		if (L < sizef[i])
		{
			idxL = i; L = sizef[i];
		}
		if (S > sizef[i])
		{
			idxS = i; S = sizef[i];
		}
	}

	if (L == S) // Cube; original is OK
	{
		return Vec3i(1,2,4);
	}
	else
	{
		int idxM = -1;
		for (int i = 0; i < 3; i++)
		{
			if ((sizef[i] != L && i!=idxS)
				|| (sizef[i] != S && i!=idxL))
			{
				idxM = i;
				break;
			}
		}
		Vec3i normalized;
		normalized[idxS] = 1;
		normalized[idxM] = 2;
		normalized[idxL] = 4;

		return normalized;
	}
}

void bone::getMeshFromOriginBox(Vec3f leftDown, Vec3f rightUp)
{
	// order of size of box
	Vec3i meshBound = normalizedVector(rightUp-leftDown);
	Vec3i boneBound = normalizedVector(m_sizef);

	Mat3x3f rotMat;
	Vec3i row[3] = {Vec3i(1,0,0), Vec3i(0,1,0), Vec3i(0,0,1)};
	Vec3f rowf[3] = {Vec3f(1,0,0), Vec3f(0,1,0), Vec3f(0,0,1)};

	// boneBound = rotMat*meshBound
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			if (meshBound*row[j] == boneBound[i]){
				rotMat(i,0) = row[j][0];
				rotMat(i,1) = row[j][1];
				rotMat(i,2) = row[j][2];
			}
		}
	}

	// Transform the mesh
	Vec3f meshCenterPos = (leftDown+rightUp)/2;
	std::vector<cVertex>& vertices = mesh->vertices;
	for (int i = 0; i < vertices.size(); i++)
	{
		Vec3f curP = Vec3f(vertices[i].v[0], vertices[i].v[1], vertices[i].v[2]);
		curP = rotMat*(curP - meshCenterPos);
		vertices[i] = carve::geom::VECTOR(curP[0], curP[1], curP[2]);
	}
}

int bone::nbCenterNeighbor()
{
	int count = 0;
	for (int i = 0; i < child.size(); i++)
	{
		if (child[i]->m_type == CENTER_BONE)
		{
			count++;
		}
	}
	return count;
}

int bone::nbSideNeighbor()
{
	int count = 0;
	for (int i = 0; i < child.size(); i++)
	{
		if (child[i]->m_type == SIDE_BONE)
		{
			count++;
		}
	}
	return count;
}

int bone::nbNeighbor() const
{
	int nbN = child.size();
	if (parent)
	{
		nbN++;
	}

	return nbN;
}

bool bone::isLeaf()
{
	return (child.size() == 0);
}

float bone::getVolumef()
{
	if (bIsGroup){
		return m_groupVolumef;
	} else {
		return m_volumef;
	}
		
}

float& bone::volumeRatio()
{
	if (bIsGroup)
	{
		return m_groupVolumeRatio;
	}
	else
		return m_volumeRatio;
}

char* bone::getTypeString()
{
	if (m_type == CENTER_BONE)
	{
		return "center_bone";
	}
	else if (m_type == SIDE_BONE)
	{
		return "side_bone";
	}
	ASSERT(0);
	return "";
}

void bone::setBoneType(std::string typeString)
{
	if (typeString == "center_bone")
	{
		m_type = CENTER_BONE;
	}
	else if (typeString == "side_bone")
	{
		m_type = SIDE_BONE;
	}
}

float bone::groupShrink()
{
	if (bIsGroup)
	{
		return m_groupVolumef / m_volumef;
	}
	
	return 1;
}

//Bone drawing functions
void bone::draw(int mode, float scale, bool mirror)
{
	glLineWidth(mirror ? 1.0 : 2.0);

	// 	Scale
	// 	Vec3f center = (leftDownf + rightUpf) / 2.0;
	// 	Vec3f diag = (rightUpf - leftDownf) / 2.0;
	// 
	// 	Vec3f ldf = center - diag*scale;
	// 	Vec3f ruf = center + diag* scale;

	if (mode & SKE_DRAW_BOX_WIRE){
		glColor3f(0, 0, 1);
		Util_w::drawBoxWireFrame(leftDownf, rightUpf);
		drawCoord();
	}

	if (mode & SKE_DRAW_BOX_SOLID){
		glColor3f(1, 1, 1);
		Util_w::drawBoxSurface(leftDownf, rightUpf);
	}

	glLineWidth(1.0);
}

void bone::drawBoneWithMeshSize()
{
	Vec3f center = (leftDownf + rightUpf) / 2;
	Vec3f ld, ru;
	ld = center - meshSizeScale / 2;
	ru = center + meshSizeScale / 2;
	glColor3f(0, 0, 1);
	Util_w::drawBoxWireFrame(ld, ru);

	drawCoord();
}

void bone::drawCoord()
{
	glPushAttrib(GL_COLOR);
	glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(m_sizef[0] / 2, 0, 0);

		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, m_sizef[1] / 2, 0);

		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, m_sizef[2] / 2);
	glEnd();
	glPopAttrib();
}

void bone::drawEstimatedBox(Vec3f center, Vec3f size){
	Util_w::drawBoxWireFrameFromCenter(center, size);
}