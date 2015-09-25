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
	animationAmt = 0;
	speed = 0.05;

	currentBoneIdx = 1;
	
	for (int i = 0; i < 4; i++){
	posAnimated[i] = false;
	}

	startAnimation = true;
	animationDone = false;
}

void TransformerAnimation::animate()
{
	if (!animationDone){
		// Determine current bone/cut mesh to be animated
		currentBone = m_mesh->boneArray[currentBoneIdx]->m_name;
		std::cout << "current bone: " << m_mesh->boneArray[currentBoneIdx]->m_nameString << endl;
		targetBoneFound = 0;

		// Translation
		if (!posAnimated[TRANSLATION]){
			animMode = TRANSLATION;
			if (animationAmt > 1){
				posAnimated[TRANSLATION] = true;
				animationAmt = 0;
				animMode = Z_ROTATION;
				std::cout << "Finished animating translation" << endl;
			}
			processAnimation(currentBone, m_skel->m_root, animationAmt);
			animationAmt += speed;
		}
		// Z Rotation 
		else if (!posAnimated[Z_ROTATION]){
			if (m_mesh->boneArray[currentBoneIdx]->m_angle[2] != 0){
				animMode = Z_ROTATION;
				if (animationAmt > 1){
					posAnimated[Z_ROTATION] = true;
					animationAmt = 0;
					animMode = Y_ROTATION;
					std::cout << "Finished animating z rotation" << endl;
				}		
			} else {
				posAnimated[Z_ROTATION] = true;
				animationAmt = 0;
				animMode = Y_ROTATION;
			}
			processAnimation(currentBone, m_skel->m_root, animationAmt);
			animationAmt += speed;
		}
		// Y Rotation
		else if (!posAnimated[Y_ROTATION]){
			if (m_mesh->boneArray[currentBoneIdx]->m_angle[1] != 0){
				animMode = Y_ROTATION;
				if (animationAmt > 1){
					posAnimated[Y_ROTATION] = true;
					animationAmt = 0;
					animMode = X_ROTATION;
					std::cout << "Finished animating y rotation" << endl;
				}	
			} else {
				posAnimated[Y_ROTATION] = true;
				animationAmt = 0;
				animMode = X_ROTATION;
			}
			processAnimation(currentBone, m_skel->m_root, animationAmt);
			animationAmt += speed;
		}
		// X Rotation
		else if (!posAnimated[X_ROTATION]){
			if (m_mesh->boneArray[currentBoneIdx]->m_angle[0] != 0){
				animMode = X_ROTATION;
				if (animationAmt > 1){
					posAnimated[X_ROTATION] = true;
					animationAmt = 0;
					std::cout << "Finished animating x rotation" << endl;
				}
				else {
					processAnimation(currentBone, m_skel->m_root, animationAmt);
					animationAmt += speed;
				}
			} else {
				posAnimated[X_ROTATION] = true;
				animationAmt = 0;
			}
		}
		// Positioning for this bone is done
		else {
			currentBoneIdx++;
			if (currentBoneIdx >= m_mesh->boneArray.size()){
				// All bones have been processed
				startAnimation = false;
				animationDone = true;

				drawOpenedTransformer(m_skel->m_root, 0);
			} else {
				// Start processing the next bone
				for (int i = 0; i < 4; i++){
					posAnimated[i] = false;
				}
				currentBone = m_mesh->boneArray[currentBoneIdx]->m_name;
				animMode = TRANSLATION;
				processAnimation(currentBone, m_skel->m_root, 0);
			}
		}
		_sleep(1);
	} else {
		// Animation has been completed
		drawOpenedTransformer(m_skel->m_root, 0);
	}
}

void TransformerAnimation::processAnimation(CString currBone, bone *rootBone, float amt)
{
	int colorIndex = 0;
	bool currBoneFound = false;

	for (int i = 1; i < m_mesh->boneArray.size(); i++){
		if (m_mesh->boneArray[i]->m_name == currBone){
			currBoneFound = true;
			switch (animMode){
				case TRANSLATION:
					animateTranslationRecur(currBone, rootBone, amt, colorIndex);
					break;
				case Z_ROTATION:
					animateZRotationRecur(currBone, rootBone, amt, colorIndex);
					break;
				case Y_ROTATION:
					animateYRotationRecur(currBone, rootBone, amt, colorIndex);
					break;
				case X_ROTATION:
					animateXRotationRecur(currBone, rootBone, amt, colorIndex);
					break;
			}	
		} else if (currBoneFound){
			// TODO: exclude children of those which have animated already
			glColor3fv(m_mesh->color[i].data());
			m_mesh->drawPolygonFace(m_mesh->boneArray[i]->mesh);
			if (m_mesh->boneArray[i]->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
					glScalef(1, -1, 1);
					m_mesh->drawPolygonFace(m_mesh->boneArray[i]->mesh);
				glPopMatrix();
			}
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
		targetBoneFound += 1;
		glPushMatrix();
			glTranslatef(amt*node->m_posCoord[0], amt*node->m_posCoord[1], amt*node->m_posCoord[2]);
			glColor3fv(m_mesh->color[colorIndex].data());
			m_mesh->drawPolygonFace(node->mesh);
		glPopMatrix();
	} else if (targetBoneFound < 1) {
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(node->m_angle[0], 1, 0, 0);

			std::cout << "Translating " << node->m_nameString << endl;
			glColor3fv(m_mesh->color[colorIndex].data());
			m_mesh->drawPolygonFace(node->mesh);
		
			colorIndex++;
			
			for (size_t i = 0; i < node->child.size(); i++){
				animateTranslationRecur(target, node->child[i], amt, colorIndex);
				if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
					glPushMatrix();
						glScalef(1, -1, 1);
						animateTranslationRecur(target, node->child[i], amt, colorIndex);
					glPopMatrix();
				}
			}		
		glPopMatrix();
	}
}

void TransformerAnimation::animateZRotationRecur(CString target, bone *node, float amt, int colorIndex)
{
	if (node == nullptr){
		return;
	}

	// Animate target bone
	if (node->m_name == target){
		targetBoneFound += 1;
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(amt*node->m_angle[2], 0, 0, 1);

			//command::print("Bone name: %s\n", node->m_nameString.c_str());
			glColor3fv(m_mesh->color[colorIndex].data());
			m_mesh->drawPolygonFace(node->mesh);
			/*
			if (node->m_type == TYPE_SIDE_BONE){
			glPushMatrix();
			glScalef(1, -1, 1);
			m_mesh->drawPolygonFace(node->mesh);
			glPopMatrix();
			}*/
		glPopMatrix();
	}
	else if (targetBoneFound < 1) {
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(node->m_angle[0], 1, 0, 0);

			std::cout << "Rotating " << node->m_nameString << endl;
			glColor3fv(m_mesh->color[colorIndex].data());
			m_mesh->drawPolygonFace(node->mesh);
			colorIndex++;

			for (size_t i = 0; i < node->child.size(); i++){
				animateZRotationRecur(target, node->child[i], amt, colorIndex);
				if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
					glPushMatrix();
						glScalef(1, -1, 1);
						animateZRotationRecur(target, node->child[i], amt, colorIndex);
					glPopMatrix();
				}
			}
		glPopMatrix();
	}
}

void TransformerAnimation::animateYRotationRecur(CString target, bone *node, float amt, int colorIndex)
{
	if (node == nullptr){
		return;
	}

	// Animate target bone
	if (node->m_name == target){
		targetBoneFound += 1;
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(amt*node->m_angle[1], 0, 1, 0);

			//command::print("Bone name: %s\n", node->m_nameString.c_str());
			glColor3fv(m_mesh->color[colorIndex].data());
			m_mesh->drawPolygonFace(node->mesh);
			/*
			if (node->m_type == TYPE_SIDE_BONE){
			glPushMatrix();
			glScalef(1, -1, 1);
			m_mesh->drawPolygonFace(node->mesh);
			glPopMatrix();
			}*/
		glPopMatrix();
	}
	else if (targetBoneFound < 1) {
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(node->m_angle[0], 1, 0, 0);

			std::cout << "Rotating " << node->m_nameString << endl;
			glColor3fv(m_mesh->color[colorIndex].data());
			m_mesh->drawPolygonFace(node->mesh);
			colorIndex++;

			for (size_t i = 0; i < node->child.size(); i++){
				animateYRotationRecur(target, node->child[i], amt, colorIndex);
				if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
					glPushMatrix();
						glScalef(1, -1, 1);
						animateYRotationRecur(target, node->child[i], amt, colorIndex);
					glPopMatrix();
				}
			}
		glPopMatrix();
	}
}

void TransformerAnimation::animateXRotationRecur(CString target, bone *node, float amt, int colorIndex)
{
	if (node == nullptr){
		return;
	}

	// Animate target bone
	if (node->m_name == target){
		targetBoneFound += 1;
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(amt*node->m_angle[0], 1, 0, 0);

			//command::print("Bone name: %s\n", node->m_nameString.c_str());
			glColor3fv(m_mesh->color[colorIndex].data());
			m_mesh->drawPolygonFace(node->mesh);
			/*
			if (node->m_type == TYPE_SIDE_BONE){
			glPushMatrix();
			glScalef(1, -1, 1);
			m_mesh->drawPolygonFace(node->mesh);
			glPopMatrix();
			}*/
		glPopMatrix();
	}
	else if (targetBoneFound < 1) {
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(node->m_angle[0], 1, 0, 0);

			std::cout << "Rotating " << node->m_nameString << endl;
			glColor3fv(m_mesh->color[colorIndex].data());
			m_mesh->drawPolygonFace(node->mesh);
			colorIndex++;

			for (size_t i = 0; i < node->child.size(); i++){
				animateXRotationRecur(target, node->child[i], amt, colorIndex);
				if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
					glPushMatrix();
					glScalef(1, -1, 1);
					animateXRotationRecur(target, node->child[i], amt, colorIndex);
					glPopMatrix();
				}
			}
		glPopMatrix();
	}
}

void TransformerAnimation::drawOpenedTransformer(bone *node, int colorIndex)
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
		glColor3fv(m_mesh->color[colorIndex].data());
		m_mesh->drawPolygonFace(node->mesh);
		colorIndex++;

		for (size_t i = 0; i < node->child.size(); i++){
			drawOpenedTransformer(node->child[i], colorIndex);
			if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
					glScalef(1, -1, 1);
					drawOpenedTransformer(node->child[i], colorIndex);
				glPopMatrix();
			}
		}
	glPopMatrix();
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
