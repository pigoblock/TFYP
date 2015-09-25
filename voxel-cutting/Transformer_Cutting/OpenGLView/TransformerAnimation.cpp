#include "stdafx.h"
#include "TransformerAnimation.h"
#include <iostream>

using namespace std;

TransformerAnimation::TransformerAnimation()
{
	m_mesh = nullptr;
	m_skel = nullptr;

	restartAnimation();
}

TransformerAnimation::~TransformerAnimation()
{
	// Deletion of m_mesh and m_skel already handled by myDocument
	// as m_tAnimation is deleted after m_skeleton and m_MeshCutting
}

void TransformerAnimation::restartAnimation()
{
	translateAmt = 0;
	rotateXAmt = 0;
	rotateYAmt = 0;
	rotateZAmt = 0;
	speed = 0.05;

	currentBoneIdx = 1;
	/*
	for (int i = 0; i < 4; i++){
	posAnimated[i] = false;
	}*/
	posAnimated[0] = false;
	posAnimated[1] = true;
	posAnimated[2] = true;
	posAnimated[3] = true;

	startAnimation = true;
	animationDone = false;
}

void TransformerAnimation::animate()
{
	if (!animationDone){
		// Draw the root of all cut pieces
	//	m_mesh->drawPolygonFace(m_mesh->m_cutPieces[0]);

		// Determine current bone/cut mesh to be animated
		currentBone = m_mesh->boneArray[currentBoneIdx]->m_name;
		std::cout << "current bone: " << m_mesh->boneArray[currentBoneIdx]->m_nameString << endl;
		targetBoneFound = false;

		// Translation
		if (!posAnimated[0]){
			if (translateAmt >= 1){
				posAnimated[0] = true;
				translateAmt = 0;
				std::cout << "Finished animating translation" << endl;
			} else {
				animateTranslation(currentBone, m_skel->m_root, translateAmt);
				translateAmt += speed;
			}
		}
		// Z Rotation 
		else if (!posAnimated[1]){

		}
		// Y Rotation
		else if (!posAnimated[2]){

		}
		// X Rotation
		else if (!posAnimated[3]){

		}
		// Positioning for this bone is done
		else {
			currentBoneIdx++;
			if (currentBoneIdx >= m_mesh->boneArray.size()){
				startAnimation = false;
				animationDone = true;
			} else {
				/*
				for (int i = 0; i < 4; i++){
					posAnimated[i] = false;
				}*/
				posAnimated[0] = false;
				posAnimated[1] = true;
				posAnimated[2] = true;
				posAnimated[3] = true;
			}
		}
		_sleep(1);
	}
}

void TransformerAnimation::animateTranslation(CString currBone, bone *rootBone, float amt)
{
	int colorIndex = 0;
	bool currBoneFound = false;

	for (int i = 1; i < m_mesh->boneArray.size(); i++){
		if (m_mesh->boneArray[i]->m_name == currBone){
			currBoneFound = true;
			animateTranslationRecur(currBone, rootBone, amt, colorIndex);
		} else if (currBoneFound){
			glColor3fv(m_mesh->color[i].data());
			m_mesh->drawPolygonFace(m_mesh->boneArray[i]->mesh);
		}
	}
}

void TransformerAnimation::animateTranslationRecur(CString target, bone *node, float amt, int colorIndex)
{
	if (node == nullptr){
		return ;
	}

	// Animate target bone
	if (node->m_name == target){
		targetBoneFound = true;
		glPushMatrix();
			glTranslatef(amt*node->m_posCoord[0], amt*node->m_posCoord[1], amt*node->m_posCoord[2]);

			//command::print("Bone name: %s\n", node->m_nameString.c_str());
			glColor3fv(m_mesh->color[colorIndex].data());
			m_mesh->drawPolygonFace(node->mesh);
			/*
			if (node->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
					glScalef(1, -1, 1);
					glTranslatef(amt*node->m_posCoord[0], amt*node->m_posCoord[1], amt*node->m_posCoord[2]);
				glPopMatrix();
			}*/
		glPopMatrix();
	} else if (!targetBoneFound) {
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(node->m_angle[0], 1, 0, 0);

			std::cout << "Drawing " << node->m_nameString << endl;
			glColor3fv(m_mesh->color[colorIndex].data());
			m_mesh->drawPolygonFace(node->mesh);
			colorIndex++;

			for (size_t i = 0; i < node->child.size(); i++){
				animateTranslationRecur(target, node->child[i], amt, colorIndex);
				/*if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
					glPushMatrix();
						glScalef(1, -1, 1);
						drawOneTransformerPart(node->child[i], node->child[i]->m_name);
					glPopMatrix();
				}*/
			}		
		glPopMatrix();
	}
}

void TransformerAnimation::drawOneTransformerPart(bone *node, CString targetName)
{
	if (node == nullptr){
		return;
	}

	glPushMatrix();
		glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
		glRotatef(node->m_angle[2], 0, 0, 1);
		glRotatef(node->m_angle[1], 0, 1, 0);
		glRotatef(node->m_angle[0], 1, 0, 0);

		//command::print("Bone name: %s\n", node->m_nameString.c_str());
		if (node->m_name == targetName){
			m_mesh->drawPolygonFace(node->mesh);
			return;
		}

		for (size_t i = 0; i < node->child.size(); i++){
			drawOneTransformerPart(node->child[i], targetName);
			/*		if (node == boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
			glPushMatrix();
			glScalef(1, -1, 1);
			drawOneTransformerPart(node->child[i], targetName);
			glPopMatrix();
			}*/
		}
	glPopMatrix();
}
