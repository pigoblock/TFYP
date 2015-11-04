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
	setupRelativeBoneStructure(transformerRootBone);
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
	newNode->setBonePosition(originalMesh->getMeshCoordOrigin()[newNode->m_index]);

	for (size_t i = 0; i < originalNode->child.size(); i++){
		TransformerBone *newChildNode = new TransformerBone();
		newChildNode->m_parent = newNode;
		newNode->m_children.push_back(newChildNode);
		mapFromOldBones(newChildNode, originalNode->child[i]);
	}
}

void TransformerSkeleton::setupRelativeBoneStructure(TransformerBone *node){
	
	if (node == nullptr){
		return;
	}

	if (node->m_parent){
		node->m_foldCoord = node->m_startJoint - node->m_parent->m_startJoint;
	}
	else {
		node->m_foldCoord = node->m_startJoint;
	}
	cprintf("Folded coords: %f %f %f Folded angle: %f\n",
		node->m_foldCoord[0], node->m_foldCoord[1], node->m_foldCoord[2], node->m_foldAngle);

	for (size_t i = 0; i < node->m_children.size(); i++){
		setupRelativeBoneStructure(node->m_children[i]);
	}
}

void TransformerSkeleton::drawSkeleton()
{
	drawFoldedSkeletonRecur(transformerRootBone);
}

void TransformerSkeleton::drawFoldedSkeletonRecur(TransformerBone *node)
{
	if (node == nullptr){
		return;
	}

	glPushMatrix();
		//glPushMatrix();
			//glTranslatef(node->m_startJoint[0], node->m_startJoint[1], node->m_startJoint[2]);
			glTranslatef(node->m_foldCoord[0], node->m_foldCoord[1], node->m_foldCoord[2]);

			glColor3fv(MeshCutting::color[node->m_index].data());
			node->drawSphereJoint(1);
		glPushMatrix();
			retrieveRotation(node->m_orientation);
			node->drawCylinderBone(node->m_length, 0.5);
		glPopMatrix();

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

void TransformerSkeleton::retrieveRotation(Vec3f localAxis)
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

void TransformerBone::setBonePosition(Vec3f distMeshToOrigin)
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