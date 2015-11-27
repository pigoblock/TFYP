#include "stdafx.h"
#include "TransformerSkeleton.h"
#include "MeshCutting.h"
#include <iostream>

TransformerSkeleton::TransformerSkeleton()
{
	transformerRootBone = nullptr;
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
	setupUnopenedRotations(originalSkeletonRootBone);
	tranformVectorsInMeshes();

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
	newNode->mesh = originalNode->mesh;
	newNode->setGlobalBonePosition(originalMesh->getAllCenterOfMesh()[newNode->m_index]);
	newNode->isConnectingBone = false;

	//cprintf("Start joint: %f %f %f\n", newNode->m_startJoint[0],
		//newNode->m_startJoint[1], newNode->m_startJoint[2]);

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
		node->m_connectingParent->m_foldCoord = getQPQConjugate(cumulativeParentQuat, Vec3f(0, 0, node->m_parent->m_length));

		node->m_connectingParent->m_foldQuat = cumulativeParentQuat.inverse() * node->m_connectingParent->m_foldQuat;
		node->m_connectingParent->m_foldQuat.normalize();
		Vec3d axis2;	double angle2;
		node->m_connectingParent->m_foldQuat.quatToAxis(axis2, angle2);
		node->m_connectingParent->m_foldAngle = Vec4f(angle2 * 180 / 3.142, axis2[0], axis2[1], axis2[2]);

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
		
	/*	cprintf("ConnectingBone length: %f \n", node->m_connectingParent->m_foldedLength);
		cprintf("ConnectingBone vec: %f %f %f &f\n", node->m_connectingParent->m_foldAngle[0], node->m_connectingParent->m_foldAngle[1],
			node->m_connectingParent->m_foldAngle[2], node->m_connectingParent->m_foldAngle[3]);
*/
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
	
	// Note: End joint is still in global coords
	for (size_t i = 0; i < node->m_children.size(); i++){
		setupRelativeBoneStructure(node->m_children[i], cumulativeParentQuat);
	}
}

void TransformerSkeleton::setupUnopenedRotations(bone *originalNode){
	setupUnopenedRotationsRecur(originalNode, Quat(), Quat(), Quat());
}

void TransformerSkeleton::setupUnopenedRotationsRecur(bone *node, 
	Quat origCumulParent, Quat foldedCumulParent, Quat unfoldCumulParent){

	if (node == nullptr){
		return;
	}

	if (node->parent){
		TransformerBone *tBone = tBoneArray[node->color];
		ConnectingBone *cBone = tBone->m_connectingParent;
		
		// Connecting bone
		cBone->m_unfoldCoord = getQPQConjugate(unfoldCumulParent, Vec3f(0, 0, tBone->m_parent->m_length));

		Vec3f unfoldedPos = transformerRootBone->m_unfoldCoord + node->parent->m_posCoord
			+ getQPQConjugate(Quat::createQuaterFromEuler(node->parent->m_angle*3.142 / 180), node->m_posCoord);
		
		float xDist = unfoldedPos[0] - cBone->m_unfoldCoord[0];
		float yDist = unfoldedPos[1] - cBone->m_unfoldCoord[1];
		float zDist = unfoldedPos[2] - cBone->m_unfoldCoord[2];

		Vec3f diff = unfoldedPos - cBone->m_unfoldCoord;
		cBone->m_unfoldedLength = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);

		double ax = 0.0;
		double zero = 1.0e-3;

		if (fabs(zDist) < zero){
			ax = 57.2957795*acos(xDist / cBone->m_unfoldedLength); // rotation angle in x-y plane
			if (xDist <= 0.0)
				ax = -ax;
		}
		else {
			ax = 57.2957795*acos(zDist / cBone->m_unfoldedLength); // rotation angle
			if (zDist <= 0.0)
				ax = -ax;
		}
		float rx = -yDist*zDist;
		float ry = xDist*zDist;

		if (fabs(zDist) < zero){
			// Rotate & align with x axis
			// Rotate to point 2 in x-y plane
			cBone->m_unfoldQuat = Quat::createQuaterFromEuler(Vec3f(-ax, 0, 0))
				*Quat::createQuaterFromEuler(Vec3f(0, 90, 0));
			Vec3d axis;	double angle;
			cBone->m_unfoldQuat.quatToAxis(axis, angle);
			cBone->m_unfoldAngle = Vec4f(angle * 180 / 3.142, axis[0], axis[1], axis[2]);
		} else {
			// Rotate about rotation vector
			cBone->m_unfoldAngle = Vec4f(ax, rx, ry, 0);
			cBone->m_unfoldQuat.axisToQuat(Vec3d(rx, ry, 0), ax*3.142 / 180);
		}

		unfoldCumulParent = unfoldCumulParent * cBone->m_foldQuat * cBone->m_unfoldQuat;

		// Actual bone mesh				
		Quat relativeRotation = tBone->m_foldQuat.inverse()*Quat::createQuaterFromEuler(node->m_angle*3.142 / 180);
		tBone->m_unfoldQuat = relativeRotation;
		tBone->m_unfoldQuat.normalize();
		Vec3d axis;	double angle;
		tBone->m_unfoldQuat.quatToAxis(axis, angle);
		tBone->m_unfoldAngle = Vec4f(angle * 180 / 3.142, axis[0], axis[1], axis[2]);

		origCumulParent = origCumulParent*Quat::createQuaterFromEuler(node->m_angle*3.142 / 180);
		foldedCumulParent = foldedCumulParent*tBone->m_foldQuat;

		Vec3f currentFoldedPos = transformerRootBone->m_foldCoord 
			+ getQPQConjugate(transformerRootBone->m_foldQuat, tBone->m_foldCoord);

		

		Vec3f diff2 = unfoldedPos - currentFoldedPos;

		tBone->m_unfoldCoord = getQPQConjugate(tBone->m_parent->m_unfoldQuat, diff2);

	//	cprintf("To vec: %f %f %f &f\n", tBone->m_unfoldAngle[0], tBone->m_unfoldAngle[1],
	//		tBone->m_unfoldAngle[2], tBone->m_unfoldAngle[3]);
		unfoldCumulParent = unfoldCumulParent * tBone->m_foldQuat * tBone->m_unfoldQuat;
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
		foldedCumulParent = transformerRootBone->m_foldQuat;
		unfoldCumulParent = transformerRootBone->m_foldQuat * transformerRootBone->m_unfoldQuat;

		//cprintf("\n");
	}

	for (size_t i = 0; i < node->child.size(); i++){
		setupUnopenedRotationsRecur(node->child[i], origCumulParent, foldedCumulParent, unfoldCumulParent);
	}
}

Vec3f TransformerSkeleton::getRelativeOrientation(Vec3f originalAbsOrientation, 
	Vec3f newBaseOrientation)
{
	Vec3f relativeOrientation = originalAbsOrientation;
	for (int j = 0; j < 3; j++){
		if (relativeOrientation[j] == 0){
			relativeOrientation[j] = newBaseOrientation[0];
		}
		else if (relativeOrientation[j] == 1){
			relativeOrientation[j] = newBaseOrientation[1];
		}
		else {
			relativeOrientation[j] = newBaseOrientation[2];
		}
	}
	return relativeOrientation;
}

void TransformerSkeleton::drawSkeleton(int mode)
{
	if (mode == 2){
		drawFoldedSkeletonRecur(transformerRootBone);
	} else {
		drawUnfoldedSkeletonRecur(transformerRootBone, mode);
	}
	
}

void TransformerSkeleton::drawFoldedSkeletonRecur(TransformerBone *node)
{
	if (node == nullptr){
		return;
	}

	glPushMatrix();
		if (node->m_connectingParent){
			glTranslatef(0, 0, node->m_parent->m_length);
			glRotatef(node->m_connectingParent->m_foldAngle[0], node->m_connectingParent->m_foldAngle[1],
				node->m_connectingParent->m_foldAngle[2], node->m_connectingParent->m_foldAngle[3]);
				
			glColor3f(1, 1, 1);
			node->drawSphereJoint(1);
			node->drawCylinderBone(node->m_connectingParent->m_foldedLength, 0.5);
		}

		glTranslatef(node->m_foldCoord[0], node->m_foldCoord[1], node->m_foldCoord[2]);
		glRotatef(node->m_foldAngle[0], node->m_foldAngle[1], node->m_foldAngle[2], node->m_foldAngle[3]);

		glColor3fv(MeshCutting::color[node->m_index].data());
		node->drawSphereJoint(1);
		node->drawCylinderBone(node->m_length, 0.5);
	//	node->drawMesh();

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
			glRotatef(node->m_connectingParent->m_unfoldAngle[0], node->m_connectingParent->m_unfoldAngle[1],
				node->m_connectingParent->m_unfoldAngle[2], node->m_connectingParent->m_unfoldAngle[3]);
			glColor3f(1, 1, 1);

			cprintf("%d\n", node->m_index);
	//		cprintf("ConnectingBone vec: %f %f %f &f\n", node->m_connectingParent->m_foldAngle[0], node->m_connectingParent->m_foldAngle[1],
	//			node->m_connectingParent->m_foldAngle[2], node->m_connectingParent->m_foldAngle[3]);

			node->drawSphereJoint(1);
			node->drawCylinderBone(node->m_connectingParent->m_unfoldedLength, 0.5);
		}

		glTranslatef(node->m_unfoldCoord[0], node->m_unfoldCoord[1], node->m_unfoldCoord[2]);
		glRotatef(node->m_foldAngle[0], node->m_foldAngle[1], node->m_foldAngle[2], node->m_foldAngle[3]);
		glRotatef(node->m_unfoldAngle[0], node->m_unfoldAngle[1], node->m_unfoldAngle[2], node->m_unfoldAngle[3]);

		glColor3fv(MeshCutting::color[node->m_index].data());
		node->drawSphereJoint(1);
		node->drawCylinderBone(node->m_length, 0.5);
	//	node->drawMesh();

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

void TransformerSkeleton::tranformVectorsInMeshes(){
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

void TransformerSkeleton::retrieveEulerRotation(Vec3f localAxis)
{
	if (localAxis == Vec3f(0, 1, 2)){
		return;
	}
	else if (localAxis == Vec3f(0, 2, 1)){
		glRotatef(-90, 1, 0, 0);
	}
	else if (localAxis == Vec3f(1, 0, 2)){
		glRotatef(-90, 0, 0, 1);
	}
	else if (localAxis == Vec3f(1, 2, 0)){
		glRotatef(-90, 0, 1, 0);
		glRotatef(-90, 1, 0, 0);
	}
	else if (localAxis == Vec3f(2, 0, 1)){
		glRotatef(90, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
	}
	else if (localAxis == Vec3f(2, 1, 0)){
		glRotatef(90, 0, 1, 0);
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
			* Quat::createQuaterFromEuler(Vec3f(-90, 0, 0));
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
		// Stub
		m_startJoint = distMeshToOrigin - Vec3f(0, m_sizef[2] / 2.0, 0);
		m_endJoint = distMeshToOrigin + Vec3f(0, m_sizef[2] / 2.0, 0);
	}
	else if (m_orientation == Vec3f(2, 0, 1)){
		m_startJoint = distMeshToOrigin - Vec3f(m_sizef[2] / 2.0, 0, 0);
		m_endJoint = distMeshToOrigin + Vec3f(m_sizef[2] / 2.0, 0, 0);
	}
	else if (m_orientation == Vec3f(2, 1, 0)){
		m_startJoint = distMeshToOrigin - Vec3f(m_sizef[2] / 2.0, 0, 0);
		m_endJoint = distMeshToOrigin + Vec3f(m_sizef[2] / 2.0, 0, 0);
	}
}

void TransformerBone::setRelativeBonePosition(Vec3f parentOrientation)
{
	float temp;
	if (parentOrientation == Vec3f(0, 1, 2)){
		return;
	}
	else if (parentOrientation == Vec3f(0, 2, 1)){
		temp = m_foldCoord[1];
		m_foldCoord[1] = m_foldCoord[2];
		m_foldCoord[2] = temp;
	}
	else if (parentOrientation == Vec3f(1, 0, 2)){
		temp = m_foldCoord[1];
		m_foldCoord[1] = m_foldCoord[0];
		m_foldCoord[0] = temp;
	}
	else if (parentOrientation == Vec3f(1, 2, 0)){
		temp = m_foldCoord[0];
		m_foldCoord[0] = m_foldCoord[1];
		m_foldCoord[1] = m_foldCoord[2];
		m_foldCoord[2] = temp;
	}
	else if (parentOrientation == Vec3f(2, 0, 1)){
		temp = m_foldCoord[0];
		m_foldCoord[0] = m_foldCoord[2];
		m_foldCoord[2] = m_foldCoord[1];
		m_foldCoord[1] = temp;
	}
	else if (parentOrientation == Vec3f(2, 1, 0)){
		temp = m_foldCoord[0];
		m_foldCoord[0] = m_foldCoord[2];
		m_foldCoord[2] = temp;
	}
}




ConnectingBone::ConnectingBone(TransformerBone* Tparent, TransformerBone* Tchild)
{
	m_Tparent = Tparent;
	m_Tchild = Tchild;

	m_parentJoint = m_Tparent->m_endJoint;
	m_childJoint = m_Tchild->m_startJoint;
	m_foldCoord = m_parentJoint;

	Vec3f diff = m_childJoint - m_parentJoint;
	m_foldedLength = sqrt(diff[0] * diff[0] + diff[1] * diff[1] + diff[2] * diff[2]);
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
	float xDist = m_childJoint[0] - m_parentJoint[0];
	float yDist = m_childJoint[1] - m_parentJoint[1];
	float zDist = m_childJoint[2] - m_parentJoint[2];

	double ax = 0.0;
	double zero = 1.0e-3;

	if (fabs(zDist) < zero){
		ax = 57.2957795*acos(xDist / m_foldedLength); // rotation angle in x-y plane
		if (xDist <= 0.0)
			ax = -ax;
	} else {
		ax = 57.2957795*acos(zDist / m_foldedLength); // rotation angle
		if (zDist <= 0.0)
			ax = -ax;
	}
	float rx = -yDist*zDist;
	float ry = xDist*zDist;

	if (fabs(zDist) < zero){
		// Rotate & align with x axis
		// Rotate to point 2 in x-y plane
		m_foldQuat = Quat::createQuaterFromEuler(Vec3f(-ax, 0, 0))
			*Quat::createQuaterFromEuler(Vec3f(0, 90, 0));
		Vec3d axis;	double angle;
		m_foldQuat.quatToAxis(axis, angle);
		m_foldAngle = Vec4f(angle * 180 / 3.142, axis[0], axis[1], axis[2]);

		//glRotated(90.0, 0, 1, 0.0); 
		//glRotated(ax, -1.0, 0.0, 0.0); 
	} else {
		// Rotate about rotation vector
		m_foldAngle = Vec4f(ax, rx, ry, 0);
		m_foldQuat.axisToQuat(Vec3d(rx, ry, 0), ax*3.142/180);
	}
}