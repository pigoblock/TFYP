#include "stdafx.h"
#include "TransformerSkeleton.h"
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

	transformerRootBone = new bone();
	createClosedTransformer(transformerRootBone, originalSkeletonRootBone);
}

void TransformerSkeleton::createClosedTransformer
(bone *transformerRootBone, bone *originalSkeletonRootBone)
{
	createClosedTransformerRecur(transformerRootBone, originalSkeletonRootBone);
}

void TransformerSkeleton::createClosedTransformerRecur(bone *newNode, bone *originalNode)
{
	if (originalNode == nullptr){
		return;
	}

	newNode->color = originalNode->color;
	newNode->m_type = originalNode->m_type;
	newNode->m_name = originalNode->m_name;
	newNode->m_sizef = originalNode->m_sizef;
	if (originalNode == originalSkeletonRootBone){
		newNode->m_posCoord = originalMesh->getMeshCoordOrigin()[newNode->color];
		newNode->m_angle = Vec3f(0, 0, 0);
	}
	else {
		newNode->m_posCoord = originalMesh->getMeshCoordOrigin()[newNode->color]
			- originalMesh->getMeshCoordOrigin()[newNode->parent->color];
		
		float lengthX = newNode->m_posCoord[0] - newNode->parent->m_posCoord[0];
		float lengthY = newNode->m_posCoord[1] - newNode->parent->m_posCoord[1];
		float lengthZ = newNode->m_posCoord[2] - newNode->parent->m_posCoord[2];

		float length = sqrt(lengthX*lengthX + lengthY*lengthY + lengthZ*lengthZ);
		float ax;
		if (lengthZ == 0){
			//57.2957795 = 180/pi (convert radians to degrees)
			ax = 57.2957795*acos(0.0001 / length);
		} else {
			ax = 57.2957795*acos(newNode->m_posCoord[2] / length);
		}
		if (lengthZ < 0.0){
			ax = -ax;
		}
		float rx = -lengthY * lengthZ;
		float ry = lengthX * lengthZ;

		//glRotatef(ax, rx, ry, 0.0);
		newNode->m_angle = Vec3f(ax, rx, ry);
	}

	for (size_t i = 0; i < originalNode->child.size(); i++){
		bone *newChildNode = new bone();
		newChildNode->parent = newNode;
		newNode->child.push_back(newChildNode);
		createClosedTransformerRecur(newChildNode, originalNode->child[i]);
	}
}

void TransformerSkeleton::drawSkeleton()
{
	drawSkeletonRecur(transformerRootBone);
}

void TransformerSkeleton::drawSkeletonRecur(bone *node)
{
	if (node == nullptr){
		return;
	}

	glPushMatrix();
	if (node->parent == transformerRootBone){
		float length = sqrt(node->m_posCoord[0] * node->m_posCoord[0] +
			node->m_posCoord[1] * node->m_posCoord[1] + node->m_posCoord[2] * node->m_posCoord[2]);
		float ax;
		if (node->m_posCoord[2] == 0){
			//57.2957795 = 180/pi (convert radians to degrees)
			ax = 57.2957795*acos(0.0001 / length);
		}
		else {
			ax = 57.2957795*acos(node->m_posCoord[2] / length);
		}
		if (node->m_posCoord[2]  < 0.0)
			ax = -ax;
		float rx = -node->m_posCoord[1] * node->m_posCoord[2];
		float ry = node->m_posCoord[0] * node->m_posCoord[2];

		glPushMatrix();
		glRotatef(ax, rx, ry, 0.0);
		node->drawCylinderBone(length, 0.4);
		glPopMatrix();
	}

	/*
		if (node->parent == transformerRootBone){
			float lengthX = node->m_posCoord[0]; //-node->parent->m_posCoord[0];
			float lengthY = node->m_posCoord[1]; //-node->parent->m_posCoord[1];
			float lengthZ = node->m_posCoord[2]; //-node->parent->m_posCoord[2];

			float length = sqrt(lengthX*lengthX + lengthY*lengthY + lengthZ*lengthZ);

			float ax;
			if (lengthZ == 0){
				//57.2957795 = 180/pi (convert radians to degrees)
				ax = 57.2957795*acos(0.0001 / length);
			}
			else {
				ax = 57.2957795*acos(lengthZ / length);
			}
			if (lengthZ  < 0.0)
				ax = -ax;
			float rx = -lengthY * lengthZ;
			float ry = lengthX * lengthZ;

			glPushMatrix();
				glRotatef(ax, rx, ry, 0.0);
				glColor3f(1, 1, 1);
				node->drawCylinderBone(length, 0.4);
			glPopMatrix();
		}*/

		glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);

		glColor3f(1, 1, 1);
		node->drawSphereJoint(0.6);

		for (size_t i = 0; i < node->child.size(); i++){
			drawSkeletonRecur(node->child[i]);

			if (node == transformerRootBone && node->child[i]->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
					glScalef(-1, 1, 1);
					drawSkeletonRecur(node->child[i]);
				glPopMatrix();
			}
		}
	glPopMatrix();
}