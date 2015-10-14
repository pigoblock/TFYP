#include "stdafx.h"
#include "TransformerAnimation.h"
#include <iostream>

using namespace std;

TransformerAnimation::TransformerAnimation()
{
	m_mesh = nullptr;
	m_skel = nullptr;

	speed = 0.05;

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
	currentBoneIdx = 1;
	
	for (int i = 0; i < 4; i++){
	posAnimated[i] = false;
	}

	startAnimation = true;
	pauseAnimation = false;
	animationDone = false;
}

void TransformerAnimation::testAnimate()
{
	arrayVec3f coordOrign = m_mesh->getMeshCoordOrigin();
	arrayVec3i coord = m_mesh->coords;

	glPushMatrix();
		glTranslatef(coordOrign[0][0], coordOrign[0][1], coordOrign[0][2]);
		glPushMatrix();
			glRotatef(90, 0, 0, 1);
			//glScalef(1, -1, 1);
			glColor3fv(m_mesh->color[0].data());
			m_mesh->drawPolygonFace(m_mesh->m_cutPieces[0]);
		glPopMatrix();
		
		glPushMatrix();
			glTranslatef(coordOrign[1][0], coordOrign[1][1], coordOrign[1][2]);
			glPushMatrix();
				glRotatef(90, 0, 1, 0);
				//glScalef(-1, 1, 1); 
				glColor3fv(m_mesh->color[1].data());
				m_mesh->drawPolygonFace(m_mesh->m_cutPieces[1]);
			glPopMatrix();
		glPopMatrix();
		
		glTranslatef(coordOrign[2][0], coordOrign[2][1], coordOrign[2][2]);
		m_skel->m_root->child[1]->drawMesh();
	glPopMatrix();

	//drawOpenedTransformer(m_skel->m_root, 0);
	/*
	for (int i = 0; i < 2; i++){
		glPushMatrix();
			glColor3fv(m_mesh->color[i].data());
			glTranslatef(coordOrign[i][0], coordOrign[i][1], coordOrign[i][2]);
			if (coord[i][0] == 0){
				if (coord[i][1] != 1){
					glScalef(1, -1, 1);
					glRotatef(-90, 1, 0, 0);
				}
			}
			else if (coord[i][0] == 1){
				if (coord[i][1] == 0){
					glRotatef(-90, 0, 0, 1);
					glScalef(1, -1, 1);
				} else{
					glRotatef(-90, 1, 0, 0);
					glRotatef(-90, 0, 0, 1);
				}
			}
			else {
				if (coord[i][1] == 0){
					glRotatef(90, 0, 0, 1);
					glRotatef(90, 1, 0, 0);
				}
				else{
					glScalef(-1, 1, 1);
					glRotatef(90, 0, 1, 0);
				}
			}
			m_mesh->drawPolygonFace(m_mesh->m_cutPieces[i]);
		glPopMatrix();
	
		if (m_mesh->boneArray[i]->m_type == TYPE_SIDE_BONE){
			glPushMatrix();
				glScalef(-1, 1, 1);
				glTranslatef(coordOrign[i][0], coordOrign[i][1], coordOrign[i][2]);
				m_mesh->drawPolygonFace(m_mesh->m_cutPieces[i]);
			glPopMatrix();
		}
	}*/

}

void TransformerAnimation::stopAnimation()
{
	startAnimation = false;
	pauseAnimation = true;
}

void TransformerAnimation::playAnimation()
{
	startAnimation = true;
	pauseAnimation = false;
}

void TransformerAnimation::animate()
{
	glPushMatrix();

	if (!animationDone){
		// Determine current bone/cut mesh to be animated
		currentBone = m_mesh->boneArray[currentBoneIdx]->m_name;
		std::cout << "current bone: " << m_mesh->boneArray[currentBoneIdx]->m_nameString << endl;
		
		numTargetBoneFound = 0;
		if (m_mesh->boneArray[currentBoneIdx]->m_type == TYPE_SIDE_BONE){
			numTargetBoneToAnimate = 2;
		} else {
			numTargetBoneToAnimate = 1;
		}

		// Translation
		if (!posAnimated[TRANSLATION]){
			animStep = TRANSLATION;
			if (animationAmt > 1){
				posAnimated[TRANSLATION] = true;
				animationAmt = 0;
				animStep = Z_ROTATION;
				std::cout << "Finished animating translation" << endl;
			}
			processAnimation(currentBone, m_skel->m_root, animationAmt);
			
			if (!pauseAnimation){
				animationAmt += speed;
			} 
		}
		// Z Rotation 
		else if (!posAnimated[Z_ROTATION]){
			if (m_mesh->boneArray[currentBoneIdx]->m_angle[2] != 0){
				animStep = Z_ROTATION;
				if (animationAmt > 1){
					posAnimated[Z_ROTATION] = true;
					animationAmt = 0;
					animStep = Y_ROTATION;
					std::cout << "Finished animating z rotation" << endl;
				}		
			} else {
				posAnimated[Z_ROTATION] = true;
				animationAmt = 0;
				animStep = Y_ROTATION;
			}
			processAnimation(currentBone, m_skel->m_root, animationAmt);
			
			if (!pauseAnimation){
				animationAmt += speed;
			}
		}
		// Y Rotation
		else if (!posAnimated[Y_ROTATION]){
			if (m_mesh->boneArray[currentBoneIdx]->m_angle[1] != 0){
				animStep = Y_ROTATION;
				if (animationAmt > 1){
					posAnimated[Y_ROTATION] = true;
					animationAmt = 0;
					animStep = X_ROTATION;
					std::cout << "Finished animating y rotation" << endl;
				}	
			} else {
				posAnimated[Y_ROTATION] = true;
				animationAmt = 0;
				animStep = X_ROTATION;
			}
			processAnimation(currentBone, m_skel->m_root, animationAmt);
			
			if (!pauseAnimation){
				animationAmt += speed;
			}
		}
		// X Rotation
		else if (!posAnimated[X_ROTATION]){
			if (m_mesh->boneArray[currentBoneIdx]->m_angle[0] != 0){
				animStep = X_ROTATION;
				if (animationAmt > 1){
					posAnimated[X_ROTATION] = true;
					animationAmt = 0;
					std::cout << "Finished animating x rotation" << endl;
				}
				else {
					processAnimation(currentBone, m_skel->m_root, animationAmt);
					
					if (!pauseAnimation){
						animationAmt += speed;
					}
				}
			} else {
				posAnimated[X_ROTATION] = true;
				animationAmt = 0;
			}
		}
		// Positioning for this bone is done
		if (posAnimated[X_ROTATION]) {
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
				animStep = TRANSLATION;
				processAnimation(currentBone, m_skel->m_root, 0);
			}
		}
		_sleep(0.1);
	} else {
		// Animation has been completed
		drawOpenedTransformer(m_skel->m_root, 0);
	}
	glPopMatrix();
}

void TransformerAnimation::processAnimation(CString currBone, bone *rootBone, float amt)
{
	int colorIndex = 0;
	lastChild = currBone;
	bool lastChildFound = false;

	switch (animStep){
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

	for (int i = 0; i < m_mesh->boneArray.size(); i++){
		if (m_mesh->boneArray[i]->m_name == lastChild){
			lastChildFound = true;
		} else if (lastChildFound){
			glColor3fv(m_mesh->color[i].data());

			glPushMatrix();
				glRotatef(-90, 0, 0, 1);
				m_mesh->drawPolygonFace(m_mesh->boneArray[i]->mesh);
			glPopMatrix();
			if (m_mesh->boneArray[i]->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
					glScalef(-1, 1, 1);
					glRotatef(-90, 0, 0, 1);
					m_mesh->drawPolygonFace(m_mesh->boneArray[i]->mesh);
				glPopMatrix();
			}
		}
	}
}

void TransformerAnimation::animateRecur(bone *node, float amt, int colorIndex)
{
	if (node == nullptr){
		return;
	}

	for (size_t i = 0; i < node->child.size(); i++){
		glColor3fv(m_mesh->color[colorIndex+i].data());
		drawMesh(node);
		colorIndex++;

		lastChild = node->child[i]->m_name;
		std::cout << "last child: " << node->child[i]->m_nameString << endl;

		animateRecur(node->child[i], amt, colorIndex);
	}
}

void TransformerAnimation::animateTranslationRecur(CString target, bone *node, float amt, int colorIndex)
{
	if (node == nullptr){
		return ;
	}

	// Animate target bone
	if (node->m_name == target){
		numTargetBoneFound += 1;
		std::cout << "translating " << node->m_nameString << " " << numTargetBoneFound << endl;
		glPushMatrix();
			glTranslatef(amt*node->m_posCoord[0], amt*node->m_posCoord[1], amt*node->m_posCoord[2]);
			
			glColor3fv(m_mesh->color[colorIndex].data());
			drawMesh(node);
			colorIndex++;

			animateRecur(node, amt, colorIndex);
		glPopMatrix();
	} else if (numTargetBoneFound < numTargetBoneToAnimate) {
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(node->m_angle[0], 1, 0, 0);

			std::cout << "Translating " << node->m_nameString << endl;
			glColor3fv(m_mesh->color[colorIndex].data());
			drawMesh(node);
			colorIndex++;
			
			for (size_t i = 0; i < node->child.size(); i++){
				animateTranslationRecur(target, node->child[i], amt, colorIndex+i);
				if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
					std::cout << "splitting " << node->child[i]->m_nameString << endl;
					glPushMatrix();
						glScalef(1, -1, 1);
						animateTranslationRecur(target, node->child[i], amt, colorIndex+i);
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
		numTargetBoneFound += 1;
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(amt*node->m_angle[2], 0, 0, 1);

			std::cout << "Rotating z " << node->m_nameString << " " << numTargetBoneFound << endl;
			glColor3fv(m_mesh->color[colorIndex].data());
			drawMesh(node);
			colorIndex++;

			animateRecur(node, amt, colorIndex);
		glPopMatrix();
	} else if (numTargetBoneFound < numTargetBoneToAnimate) {
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(node->m_angle[0], 1, 0, 0);

			std::cout << "Rotating " << node->m_nameString << endl;
			glColor3fv(m_mesh->color[colorIndex].data());
			drawMesh(node);
			colorIndex++;

			for (size_t i = 0; i < node->child.size(); i++){
				animateZRotationRecur(target, node->child[i], amt, colorIndex+i);
				if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
					glPushMatrix();
						glScalef(1, -1, 1);
						animateZRotationRecur(target, node->child[i], amt, colorIndex+i);
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
		numTargetBoneFound += 1;
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(amt*node->m_angle[1], 0, 1, 0);

			std::cout << "Rotating y " << node->m_nameString << " " << numTargetBoneFound << endl;
			glColor3fv(m_mesh->color[colorIndex].data());
			drawMesh(node);
			colorIndex++;

			animateRecur(node, amt, colorIndex);
		glPopMatrix();
	} else if (numTargetBoneFound < numTargetBoneToAnimate) {
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(node->m_angle[0], 1, 0, 0);

			std::cout << "Rotating " << node->m_nameString << endl;
			glColor3fv(m_mesh->color[colorIndex].data());
			drawMesh(node);
			colorIndex++;

			for (size_t i = 0; i < node->child.size(); i++){
				animateYRotationRecur(target, node->child[i], amt, colorIndex+i);
				if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
					glPushMatrix();
						glScalef(1, -1, 1);
						animateYRotationRecur(target, node->child[i], amt, colorIndex+i);
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
		numTargetBoneFound += 1;
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(amt*node->m_angle[0], 1, 0, 0);

			std::cout << "Rotating x " << node->m_nameString << " " << numTargetBoneFound << endl;
			glColor3fv(m_mesh->color[colorIndex].data());
			drawMesh(node);
			colorIndex++;

			animateRecur(node, amt, colorIndex);
		glPopMatrix();
	} else if (numTargetBoneFound < numTargetBoneToAnimate) {
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(node->m_angle[0], 1, 0, 0);

			std::cout << "Rotating " << node->m_nameString << endl;
			glColor3fv(m_mesh->color[colorIndex].data());
			drawMesh(node);
			colorIndex++;

			for (size_t i = 0; i < node->child.size(); i++){
				animateXRotationRecur(target, node->child[i], amt, colorIndex+i);
				if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
					glPushMatrix();
					glScalef(1, -1, 1);
					animateXRotationRecur(target, node->child[i], amt, colorIndex+i);
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
		glColor3fv(m_mesh->color[colorIndex++].data());
		drawMesh(node);

		for (size_t i = 0; i < node->child.size(); i++){
			drawOpenedTransformer(node->child[i], colorIndex+i);
			if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
					glScalef(1, -1, 1);
					drawOpenedTransformer(node->child[i], colorIndex+i);
				glPopMatrix();
			}
		}
	glPopMatrix();
}

void TransformerAnimation::drawClosedTransformer()
{
	for (int i = 0; i < m_mesh->boneArray.size(); i++){
		glColor3fv(m_mesh->color[i].data());

		glPushMatrix();
			glRotatef(-90, 0, 0, 1);
			m_mesh->drawPolygonFace(m_mesh->boneArray[i]->mesh);
		glPopMatrix();
		if (m_mesh->boneArray[i]->m_type == TYPE_SIDE_BONE){
			glPushMatrix();
				glScalef(-1, 1, 1);
				glRotatef(-90, 0, 0, 1);
				m_mesh->drawPolygonFace(m_mesh->boneArray[i]->mesh);
			glPopMatrix();
		}
	}
}

void TransformerAnimation::drawMesh(bone *node)
{
	if (node == m_mesh->boneArray[1]){
		glPushMatrix();
			glRotatef(90, 0, 1, 0);
			glScalef(-1, 1, 1);
			glRotatef(-90, 0, 0, 1);
			m_mesh->drawPolygonFace(node->mesh);
		glPopMatrix();
	}
	else {
		glPushMatrix();
			glRotatef(-90, 0, 0, 1);
			m_mesh->drawPolygonFace(node->mesh);
		glPopMatrix();
	}
}
