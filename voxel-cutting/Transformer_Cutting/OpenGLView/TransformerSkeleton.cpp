#include "stdafx.h"
#include "TransformerSkeleton.h"
#include "MeshCutting.h"
#include <iostream>

TransformerSkeleton::TransformerSkeleton()
{
	transformerRootBone = nullptr;

	showSkeleton = true;
	showMesh = true;
}

TransformerSkeleton::~TransformerSkeleton()
{
	if (transformerRootBone){
		delete transformerRootBone;
	}
}

void TransformerSkeleton::initialize(bone *originalSkeletonRoot, MeshCutting *originalMeshCutting)
{
	originalSkeletonRootBone = originalSkeletonRoot;
	originalMesh = originalMeshCutting;

	if (transformerRootBone){
		delete transformerRootBone;
	}

	transformerRootBone = new TransformerBone();
	createClosedTransformer(transformerRootBone, originalSkeletonRootBone);
}

void TransformerSkeleton::createClosedTransformer
(TransformerBone *transformerRootBone, bone *originalSkeletonRootBone)
{
	mapFromOldBones(transformerRootBone, originalSkeletonRootBone);
	
	setupRelativeBoneStructure(transformerRootBone, Quat());
	setupUnopenedRotations(originalSkeletonRootBone, Quat(), Vec3f(), Vec3f(), Quat());
	tranformVerticesInMeshes();

	setupConnectingBones();
}

void TransformerSkeleton::mapFromOldBones(TransformerBone *newNode, bone *originalNode)
{
	if (originalNode == nullptr){
		return;
	}

	newNode->m_index = originalNode->color;
	newNode->m_type = originalNode->m_type;
	newNode->m_name = originalNode->m_name;
	newNode->m_sizef = originalNode->m_sizef;
	newNode->m_orientation = originalMesh->coords[newNode->m_index];
	newNode->mesh = originalNode->mesh; // originalMesh->m_cutPieces[newNode->m_index]; 

	//cprintf("Mesh face size %d, %d %s\n", newNode->m_index, newNode->mesh->faces.size(), newNode->m_name);

	newNode->setGlobalBonePosition(originalMesh->getAllCenterOfMesh()[newNode->m_index]);

	tBoneArray.push_back(newNode);

	if (newNode->m_parent){
		ConnectingBone *newConnectingBone = new ConnectingBone(newNode->m_parent, newNode);
		newNode->m_connectingParent = newConnectingBone;
		newNode->m_parent->m_connectingChildren.push_back(newConnectingBone);

		connectingBones.push_back(newConnectingBone);
	}
	else {
		newNode->m_connectingParent = nullptr;
	}

	for (size_t i = 0; i < originalNode->child.size(); i++){
		TransformerBone *newChildNode = new TransformerBone();
		newChildNode->m_parent = newNode;
		newNode->m_children.push_back(newChildNode);
		mapFromOldBones(newChildNode, originalNode->child[i]);
	}
}

void TransformerSkeleton::setupConnectingBones(){
	for (int i = 0; i < connectingBones.size(); i++){
		connectingBones[i]->m_index = i;
	}
}

void TransformerSkeleton::setupRelativeBoneStructure(TransformerBone *node, Quat cumulativeParentQuat){
	
	if (node == nullptr){
		return;
	}

	if (node->m_parent){	
		// Connecting Bone 
		node->m_connectingParent->m_foldCoord = Vec3f(0, 0, node->m_parent->m_length);

		node->m_connectingParent->m_foldQuat = cumulativeParentQuat.inverse() * node->m_connectingParent->m_foldQuat;
		node->m_connectingParent->m_foldQuat.normalize();
		Vec3d axis2;	double angle2;
		node->m_connectingParent->m_foldQuat.quatToAxis(axis2, angle2);
		node->m_connectingParent->m_foldAngle = Vec4f(angle2 * 180 / 3.142, axis2[0], axis2[1], axis2[2]);
		
//		cprintf("after localising rotations: %f %f %f %f\n", node->m_connectingParent->m_foldAngle[0],
//			node->m_connectingParent->m_foldAngle[1], node->m_connectingParent->m_foldAngle[2], node->m_connectingParent->m_foldAngle[3]);

		cumulativeParentQuat = cumulativeParentQuat * node->m_connectingParent->m_foldQuat;

		// Bone Mesh
		node->m_foldCoord = Vec3f(0, 0, node->m_connectingParent->m_foldedLength);
		
		Quat globalRotation = retrieveQuatRotation(node->m_orientation);
		Quat relativeRotation = cumulativeParentQuat.inverse() * globalRotation;
		node->m_foldQuat = relativeRotation;
		node->m_foldQuat.normalize();

		Vec3d axis;	double angle;
		node->m_foldQuat.quatToAxis(axis, angle);
		node->m_foldAngle = Vec4f(angle * 180 / 3.142, axis[0], axis[1], axis[2]);
		
		cumulativeParentQuat = cumulativeParentQuat * node->m_foldQuat;
	}
	else {
		// Root bone
		node->m_foldCoord = node->m_startJoint;
		node->m_foldQuat = retrieveQuatRotation(node->m_orientation);
		node->m_foldQuat.normalize();
		Vec3d axis;	double angle;
		node->m_foldQuat.quatToAxis(axis, angle);
		node->m_foldAngle = Vec4f(angle*180/3.142, axis[0], axis[1], axis[2]);
		//cprintf("\n");

		cumulativeParentQuat = node->m_foldQuat;
	}
	
	// Note: End joint is still in global coords but not important
	for (size_t i = 0; i < node->m_children.size(); i++){
		setupRelativeBoneStructure(node->m_children[i], cumulativeParentQuat);
	}
}

void TransformerSkeleton::setupUnopenedRotations(bone *node,
	Quat origCumulParent, Vec3f origCumulPosition, Vec3f cumulPosition, Quat unfoldCumulParent){

	if (node == nullptr){
		return;
	}

	if (node->parent){
		TransformerBone *tBone = tBoneArray[node->color];
		ConnectingBone *cBone = tBone->m_connectingParent;

		// Connecting bone
		cBone->m_unfoldCoord = Vec3f(0, 0, tBone->m_parent->m_length);	// same as fold coord

		Vec3f bpPos = node->m_posCoord - cBone->m_unfoldCoord;
		cBone->m_unfoldedLength = sqrt(bpPos[0] * bpPos[0] + bpPos[1] * bpPos[1] + bpPos[2] * bpPos[2]);

		float xDist = bpPos[0];
		float yDist = bpPos[1];
		float zDist = bpPos[2];

		double ax = 0.0;
		double zero = 1.0e-3;

//		cprintf("xDist: %f\n", xDist);
//		cprintf("yDist: %f\n", yDist);
//		cprintf("zDist: %f\n", zDist);

		// Remove zero errors
		if (xDist == 0){
			xDist = zero;
		}
		if (yDist == 0){
			yDist = zero;
		}
		if (zDist == 0){
			zDist = zero;
		}

		float cBoneTempLength = cBone->m_unfoldedLength;
		if (cBoneTempLength == 0){
			cBoneTempLength = zero;
		}

		ax = 57.2957795*acos(zDist / cBoneTempLength);
		if (zDist <= 0.0){
			ax = -ax;
		}
//		cprintf("ax: %f\n", ax);

		if (ax == 0){
			cBone->m_unfoldAngle = Vec4f(0, 0, 0, 1);
			cBone->m_unfoldQuat = Quat();
		}
		else {
			float rx = -yDist*zDist;
			float ry = xDist*zDist;

			// Rotate about rotation vector
			cBone->m_unfoldAngle = Vec4f(ax, rx, ry, 0);
			cBone->m_unfoldQuat.axisToQuat(Vec3d(rx, ry, 0), ax*3.142 / 180);
		}
		cBone->m_unfoldQuat = cBone->m_foldQuat.inverse() * cBone->m_unfoldQuat;

//		cprintf("cbone m_foldQuat: %f %f %f %f\n", cBone->m_foldQuat[0], cBone->m_foldQuat[1],
//			cBone->m_foldQuat[2], cBone->m_foldQuat[3]);
//		cprintf("cbone m_foldQuat.inverse: %f %f %f %f\n", cBone->m_foldQuat.inverse()[0], cBone->m_foldQuat.inverse()[1],
//			cBone->m_foldQuat.inverse()[2], cBone->m_foldQuat.inverse()[3]);

		cBone->m_unfoldQuat.normalize();

//		cprintf("cbone unfoldquat: %f %f %f %f\n", cBone->m_unfoldQuat[0], cBone->m_unfoldQuat[1],
//			cBone->m_unfoldQuat[2], cBone->m_unfoldQuat[3]);
		Vec3d axis2;	double angle2;
		cBone->m_unfoldQuat.quatToAxis(axis2, angle2);
		cBone->m_unfoldAngle = Vec4f(angle2 * 180 / 3.142, axis2[0], axis2[1], axis2[2]);

		unfoldCumulParent = unfoldCumulParent * cBone->m_foldQuat * cBone->m_unfoldQuat * tBone->m_foldQuat;

		// Actual bone mesh			
		tBone->m_unfoldCoord = Vec3f(0, 0, cBone->m_unfoldedLength);

//		cprintf("tbone : %d\n", tBone->m_index);

//		cprintf("tbone raw angle: %f %f %f\n", node->m_angle[0], node->m_angle[1],
//			node->m_angle[2]);

		origCumulParent = origCumulParent * Quat::createQuaterFromEuler(node->m_angle*3.142 / 180);

//		cprintf("origCumulParent: %f %f %f %f\n", origCumulParent[0], origCumulParent[1],
//			origCumulParent[2], origCumulParent[3]);

		tBone->m_unfoldQuat = unfoldCumulParent.inverse() * origCumulParent;
		tBone->m_unfoldQuat.normalize();
		Vec3d axis;	double angle;
		tBone->m_unfoldQuat.quatToAxis(axis, angle);
		tBone->m_unfoldAngle = Vec4f(angle * 180 / 3.142, axis[0], axis[1], axis[2]);

//		cprintf("tbone unfold angle: %f %f %f %f\n", tBone->m_unfoldAngle[0], tBone->m_unfoldAngle[1],
//			tBone->m_unfoldAngle[2], tBone->m_unfoldAngle[3]);
		
		unfoldCumulParent = unfoldCumulParent * tBone->m_unfoldQuat;
	}
	else {
		// Root bone 
		transformerRootBone->m_unfoldQuat = transformerRootBone->m_foldQuat.inverse();
		transformerRootBone->m_unfoldQuat.normalize();

		Vec3d axis;	double angle;
		transformerRootBone->m_unfoldQuat.quatToAxis(axis, angle);
		transformerRootBone->m_unfoldAngle = Vec4f(angle * 180 / 3.142, axis[0], axis[1], axis[2]);
		transformerRootBone->m_unfoldCoord = transformerRootBone->m_foldCoord;
		
		origCumulParent = Quat::createQuaterFromEuler(node->m_angle*3.142 / 180);
		origCumulPosition = transformerRootBone->m_unfoldCoord;
		cumulPosition = transformerRootBone->m_unfoldCoord;
		unfoldCumulParent = transformerRootBone->m_foldQuat * transformerRootBone->m_unfoldQuat;
	}

	for (size_t i = 0; i < node->child.size(); i++){
		setupUnopenedRotations(node->child[i], origCumulParent, origCumulPosition, cumulPosition, unfoldCumulParent);
	}
}

void TransformerSkeleton::drawSkeleton(int mode, bool display[2])
{
	showSkeleton = display[0];
	showMesh = display[1];

	if (mode == 2){
		drawFoldedSkeletonRecur(transformerRootBone);
	} else {
		glPushMatrix();
			glTranslatef(-transformerRootBone->m_unfoldCoord[0], 
				-transformerRootBone->m_unfoldCoord[1], -transformerRootBone->m_unfoldCoord[2]);
			drawUnfoldedSkeletonRecur(transformerRootBone, mode);
		glPopMatrix();
	}
	
}

void TransformerSkeleton::drawFoldedSkeletonRecur(TransformerBone *node)
{
	if (node == nullptr){
		return;
	}

	glPushMatrix();
		if (node->m_connectingParent){
			glTranslatef(node->m_connectingParent->m_foldCoord[0], 
				node->m_connectingParent->m_foldCoord[1], node->m_connectingParent->m_foldCoord[2]);
			glRotatef(node->m_connectingParent->m_foldAngle[0], node->m_connectingParent->m_foldAngle[1],
				node->m_connectingParent->m_foldAngle[2], node->m_connectingParent->m_foldAngle[3]);
				
			if (showSkeleton){
				glColor3f(1, 1, 1);
				node->drawSphereJoint(1);
				node->drawCylinderBone(node->m_connectingParent->m_foldedLength, 0.5);
			}
		}

		glTranslatef(node->m_foldCoord[0], node->m_foldCoord[1], node->m_foldCoord[2]);
		glRotatef(node->m_foldAngle[0], node->m_foldAngle[1], node->m_foldAngle[2], node->m_foldAngle[3]);

		glColor3fv(MeshCutting::color[node->m_index].data());
		drawBasedOnDisplayMode(node, node->m_length);

		for (size_t i = 0; i < node->m_children.size(); i++){
			drawFoldedSkeletonRecur(node->m_children[i]);

			if (node == transformerRootBone && node->m_children[i]->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
					glScalef(-1, 1, 1);
					drawFoldedSkeletonRecur(node->m_children[i]);
				glPopMatrix();
			}
		}
	glPopMatrix();
}

void TransformerSkeleton::drawUnfoldedSkeletonRecur(TransformerBone *node, int mode)
{
	if (node == nullptr){
		return;
	}

	glPushMatrix();
		if (node->m_connectingParent){
			glTranslatef(node->m_connectingParent->m_unfoldCoord[0], node->m_connectingParent->m_unfoldCoord[1], 
				node->m_connectingParent->m_unfoldCoord[2]);
			glRotatef(node->m_connectingParent->m_foldAngle[0], node->m_connectingParent->m_foldAngle[1],
				node->m_connectingParent->m_foldAngle[2], node->m_connectingParent->m_foldAngle[3]);
			glRotatef(node->m_connectingParent->m_unfoldAngle[0], node->m_connectingParent->m_unfoldAngle[1],
				node->m_connectingParent->m_unfoldAngle[2], node->m_connectingParent->m_unfoldAngle[3]);
			
			if (showSkeleton){
				glColor3f(1, 1, 1);
				node->drawSphereJoint(1);
				node->drawCylinderBone(node->m_connectingParent->m_unfoldedLength, 0.5);
			}
		}

		glTranslatef(node->m_unfoldCoord[0], node->m_unfoldCoord[1], node->m_unfoldCoord[2]);
		glRotatef(node->m_foldAngle[0], node->m_foldAngle[1], node->m_foldAngle[2], node->m_foldAngle[3]);
		glRotatef(node->m_unfoldAngle[0], node->m_unfoldAngle[1], node->m_unfoldAngle[2], node->m_unfoldAngle[3]);

		glColor3fv(MeshCutting::color[node->m_index].data());
		drawBasedOnDisplayMode(node, node->m_length);

		for (size_t i = 0; i < node->m_children.size(); i++){
			drawUnfoldedSkeletonRecur(node->m_children[i], mode);

			if (node == transformerRootBone && node->m_children[i]->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
					glScalef(-1, 1, 1);
					drawUnfoldedSkeletonRecur(node->m_children[i], mode);
				glPopMatrix();
			}
		}
	glPopMatrix();
}

void TransformerSkeleton::drawClosedGlobalCB(){
	for (int i = 0; i < connectingBones.size(); i++){
		glPushMatrix();
			glTranslatef(connectingBones[i]->m_foldCoord[0], connectingBones[i]->m_foldCoord[1], connectingBones[i]->m_foldCoord[2]);
			glRotatef(connectingBones[i]->m_foldAngle[0], connectingBones[i]->m_foldAngle[1], connectingBones[i]->m_foldAngle[2], connectingBones[i]->m_foldAngle[3]);

			glColor3f(1, 1, 1);
			connectingBones[i]->m_Tchild->drawSphereJoint(1);
			connectingBones[i]->m_Tchild->drawCylinderBone(connectingBones[i]->m_foldedLength, 0.5);
		glPopMatrix();
	}
}

void TransformerSkeleton::drawClosedGlobalSkel(TransformerBone *node){
	if (node == nullptr){
		return;
	}

	glPushMatrix();
		glPushMatrix();
			glTranslatef(node->m_foldCoord[0], node->m_foldCoord[1], node->m_foldCoord[2]);
			glRotatef(node->m_foldAngle[0], node->m_foldAngle[1], node->m_foldAngle[2], node->m_foldAngle[3]);

			glColor3fv(MeshCutting::color[node->m_index].data());
			node->drawSphereJoint(1);
			node->drawCylinderBone(node->m_length, 0.5);
		glPopMatrix();

		for (size_t i = 0; i < node->m_children.size(); i++){
			drawFoldedSkeletonRecur(node->m_children[i]);

			if (node == transformerRootBone && node->m_children[i]->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
					glScalef(-1, 1, 1);
					drawClosedGlobalSkel(node->m_children[i]);
				glPopMatrix();
			}
		}
	glPopMatrix();
}

void TransformerSkeleton::tranformVerticesInMeshes(){
	for (int i = 0; i < tBoneArray.size(); i++){
		Polyhedron* curP = tBoneArray[i]->mesh;

		std::vector<cVertex> * vertices = &curP->vertices;
		for (int j = 0; j < vertices->size(); j++){
			cVertex curV = vertices->at(j);
			Vec3f curP(curV.v[0], curV.v[1], curV.v[2]);
			Vec3f tP = getQPQConjugate(retrieveQuatRotation(tBoneArray[i]->m_orientation), curP);

			vertices->at(j) = carve::geom::VECTOR(tP[0], tP[1], tP[2]);
		}
	}
}

Quat TransformerSkeleton::retrieveQuatRotation(Vec3f localAxis)
{
	// Quaternion operations in radians
	Quat quaternion;	// Set as no rotation first
	if (localAxis == Vec3f(0, 1, 2)){
		// do nothing
		quaternion = Quat::createQuaterFromEuler(Vec3f(0, 0, 0));
	}
	else if (localAxis == Vec3f(0, 2, 1)){
		quaternion = Quat::createQuaterFromEuler(Vec3f(-90 * 3.142/180, 0, 0));
	}
	else if (localAxis == Vec3f(1, 0, 2)){
		quaternion = Quat::createQuaterFromEuler(Vec3f(0, 0, -90 * 3.142 / 180));
	}
	else if (localAxis == Vec3f(1, 2, 0)){
		quaternion = Quat::createQuaterFromEuler(Vec3f(0, -90 * 3.142 / 180, 0))
			* Quat::createQuaterFromEuler(Vec3f(-90 * 3.142 / 180, 0, 0));
	}
	else if (localAxis == Vec3f(2, 0, 1)){
		quaternion = Quat::createQuaterFromEuler(Vec3f(90 * 3.142 / 180, 0, 0))
			* Quat::createQuaterFromEuler(Vec3f(0, 0, 90 * 3.142 / 180));
	}
	else if (localAxis == Vec3f(2, 1, 0)){
		quaternion = Quat::createQuaterFromEuler(Vec3f(0, 90 * 3.142 / 180, 0));
	}

	quaternion.normalize();

	return quaternion;
}

Vec3f TransformerSkeleton::getQPQConjugate(Quat quat, Vec3f originalPoint){
	Quat quatFormOfOrigPoint = Quat(originalPoint[0], originalPoint[1], originalPoint[2], 0);
	quat.normalize();

	Quat resultPointInQuat = quat.inverse()*quatFormOfOrigPoint*quat;
	Vec3f resultPoint = Vec3f(resultPointInQuat[0], resultPointInQuat[1], resultPointInQuat[2]);

	return resultPoint;
}

void TransformerSkeleton::drawBasedOnDisplayMode(TransformerBone *node, float boneLength){
	if (showSkeleton){
		node->drawSphereJoint(1);
		node->drawCylinderBone(boneLength, 0.5);
	}
	if (showMesh){
		node->drawMesh();
	}
}


TransformerBone::TransformerBone()
{
	m_parent = nullptr;
}

TransformerBone::~TransformerBone()
{
	for (size_t i = 0; i < m_children.size(); i++){
		delete m_children[i];
	}
}

void TransformerBone::drawSphereJoint(float radius)
{
	GLUquadricObj *qobj = 0;
	qobj = gluNewQuadric();
	gluSphere(qobj, radius, 10, 10);
}

void TransformerBone::drawCylinderBone(float length, float width)
{
	GLUquadricObj *qobj = 0;
	qobj = gluNewQuadric();
	gluCylinder(qobj, width, width, length, 5, 5);
}

void TransformerBone::drawMesh()
{
	if (!mesh){
		return;
	}
	//cprintf("Mesh face size, %d\n", mesh->faces.size());

	glPushMatrix();
		MeshCutting mC;
		mC.drawPolygonFace(mesh);
	glPopMatrix();
}

void TransformerBone::setGlobalBonePosition(Vec3f distMeshToOrigin)
{
	// How to know if start is - or +?
	m_length = m_sizef[2];
	if (m_orientation == Vec3f(0, 1, 2)){
		m_startJoint = distMeshToOrigin - Vec3f(0, 0, m_sizef[2] / 2.0);
		m_endJoint = distMeshToOrigin + Vec3f(0, 0, m_sizef[2] / 2.0);
	}
	else if (m_orientation == Vec3f(0, 2, 1)){
		m_startJoint = distMeshToOrigin - Vec3f(0, m_sizef[2] / 2.0, 0);
		m_endJoint = distMeshToOrigin + Vec3f(0, m_sizef[2] / 2.0, 0);
	}
	else if (m_orientation == Vec3f(1, 0, 2)){
		m_startJoint = distMeshToOrigin - Vec3f(0, 0, m_sizef[2] / 2.0);
		m_endJoint = distMeshToOrigin + Vec3f(0, 0, m_sizef[2] / 2.0);
	}
	else if (m_orientation == Vec3f(1, 2, 0)){
		m_startJoint = distMeshToOrigin - Vec3f(0, m_sizef[2] / 2.0, 0);
		m_endJoint = distMeshToOrigin + Vec3f(0, m_sizef[2] / 2.0, 0);
	}
	else if (m_orientation == Vec3f(2, 0, 1)){
		m_startJoint = distMeshToOrigin - Vec3f(m_sizef[2] / 2.0, 0, 0);
		m_endJoint = m_startJoint - Vec3f(0, m_sizef[2], 0);// +Vec3f(m_sizef[2], 0, 0);
	}
	else if (m_orientation == Vec3f(2, 1, 0)){
		m_startJoint = distMeshToOrigin - Vec3f(m_sizef[2] / 2.0, 0, 0);
		m_endJoint = distMeshToOrigin + Vec3f(m_sizef[2] / 2.0, 0, 0);
	}
}




ConnectingBone::ConnectingBone(TransformerBone* Tparent, TransformerBone* Tchild)
{
	m_Tparent = Tparent;
	m_Tchild = Tchild;

	// Global (will be changed to local later on)
	m_parentJoint = m_Tparent->m_endJoint;
	m_childJoint = m_Tchild->m_startJoint;
	m_foldCoord = m_parentJoint;

	Vec3f diff = m_childJoint - m_parentJoint;
	m_foldedLength = getMagnitude(diff);
	setRotations();
}

ConnectingBone::~ConnectingBone()
{
	if (m_Tparent){
		delete m_Tparent;
	}
	
	if (m_Tchild){
		delete m_Tchild;
	}
}

void ConnectingBone::setRotations()
{
	cprintf("childjoint: %f %f %f\n", m_childJoint[0], m_childJoint[1], m_childJoint[2]);
	cprintf("parentjoint: %f %f %f\n", m_parentJoint[0], m_parentJoint[1], m_parentJoint[2]);

	float xDist = m_childJoint[0] - m_parentJoint[0];
	float yDist = m_childJoint[1] - m_parentJoint[1];
	float zDist = m_childJoint[2] - m_parentJoint[2];
	
	cprintf("xDist: %f\n", xDist);
	cprintf("yDist: %f\n", yDist);
	cprintf("zDist: %f\n", zDist);

	double zero = 1.0e-3;
	if (xDist == 0){
		xDist = zero;
	}
	if (yDist == 0){
		yDist = zero;
	}
	if (zDist == 0){
		zDist = zero;
	}
	float cBoneLengthTemp = m_foldedLength;
	if (cBoneLengthTemp == 0){
		cBoneLengthTemp = zero;
	}

	double angleInDegrees = 0.0;
	angleInDegrees = 57.2957795*acos(zDist / cBoneLengthTemp);
	cprintf("angleInDegrees: %f\n", angleInDegrees);

	if (zDist <= 0.0){
		angleInDegrees = -angleInDegrees;
	}

	if (angleInDegrees == 0){
		m_foldAngle = Vec4f(0, 0, 0, 1);
		m_foldQuat = Quat();
	}
	else {
		float rx = -yDist*zDist;
		float ry = xDist*zDist;

		// Rotate about rotation vector
		m_foldAngle = Vec4f(angleInDegrees, rx, ry, 0);
		m_foldQuat.axisToQuat(Vec3d(rx, ry, 0), angleInDegrees*3.142 / 180);
		m_foldQuat.normalize();
	}
	cprintf("global rotations: %f %f %f %f\n", m_foldQuat[0], m_foldQuat[1], m_foldQuat[2], m_foldQuat[3]);
}

float ConnectingBone::getMagnitude(Vec3f vector){
	return sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
}