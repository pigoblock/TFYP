#include "stdafx.h"
#include "TransformerAnimation.h"
#include "mirrorDraw.h"
#include <iostream>

using namespace std;

TransformerAnimation::TransformerAnimation()
{
	m_mesh = nullptr;
	m_skel = nullptr;
	m_surObj = nullptr;

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

	transformDone = false;
}

void TransformerAnimation::testAnimate()
{
	//animateLocalRotation(m_skel->m_root, 1);
	drawOpenedTransformer(m_skel->m_root);
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

				drawOpenedTransformer(m_skel->m_root);
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

		//if (!transformDone){
		//	m_mesh->testTransform();
			//animateLocalRotation(m_skel->m_root, 1);
			transformDone = true;
		//}

		// Animation has been completed		
		drawOpenedTransformer(m_skel->m_root);
	}
	glPopMatrix();
}

void TransformerAnimation::processAnimation(CString currBone, bone *rootBone, float amt)
{
	lastChild = currBone;
	bool lastChildFound = false;

	switch (animStep){
		case TRANSLATION:
			animateTranslationRecur(currBone, rootBone, amt);
			break;
		case Z_ROTATION:
			animateZRotationRecur(currBone, rootBone, amt);
			break;
		case Y_ROTATION:
			animateYRotationRecur(currBone, rootBone, amt);
			break;
		case X_ROTATION:
			animateXRotationRecur(currBone, rootBone, amt);
			break;
	}

	mirrorDraw mirror;
	mirror.mirrorAxis = 0; //x-axis
	mirror.mirrorCoord = m_surObj->midPoint()[0];

	for (int i = 0; i < m_mesh->boneArray.size(); i++){
		if (m_mesh->boneArray[i]->m_name == lastChild){
			lastChildFound = true;
		} else if (lastChildFound){
			glColor3fv(m_mesh->color[i].data());
			glPushMatrix();
				glTranslatef(m_mesh->getMeshCoordOrigin()[i][0], 
					m_mesh->getMeshCoordOrigin()[i][1], m_mesh->getMeshCoordOrigin()[i][2]);
				setRotationCase(m_mesh->coords[i]);
				m_mesh->drawPolygonFace(m_mesh->boneArray[i]->mesh);
			glPopMatrix();

			if (m_mesh->boneArray[i]->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
					glTranslatef(mirror.mirrorCoord, 0, 0);
					glScalef(-1, 1, 1);
					glTranslatef(m_mesh->getMeshCoordOrigin()[i][0],
						m_mesh->getMeshCoordOrigin()[i][1], m_mesh->getMeshCoordOrigin()[i][2]);
					setRotationCase(m_mesh->coords[i]);
					m_mesh->drawPolygonFace(m_mesh->boneArray[i]->mesh);
				glPopMatrix();
			}
		}
	}
}

void TransformerAnimation::animateTranslationRecur(CString target, bone *node, float amt)
{
	if (node == nullptr){
		return ;
	}

	// Animate target bone
	if (node->m_name == target){
		numTargetBoneFound += 1;
		//std::cout << "translating " << node->m_nameString << " " << numTargetBoneFound << endl;
		glPushMatrix();
			glTranslatef(amt*node->m_posCoord[0], amt*node->m_posCoord[1], amt*node->m_posCoord[2]);
			//drawMesh(node);
			glPushMatrix();
				Vec3f mid = (node->leftDownf + node->rightUpf) / 2;
				glTranslatef(mid[0], mid[1], mid[2]);
				drawMesh(node);
			glPopMatrix();
			animateChildrenRecur(node, amt);
		glPopMatrix();
	} else if (numTargetBoneFound < numTargetBoneToAnimate) {
		glPushMatrix();
			if (node == m_skel->m_root){
				centerOriginWrtTorso();
			}

			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(node->m_angle[0], 1, 0, 0);

			//std::cout << "Translating " << node->m_nameString << endl;
			//drawMesh(node);
			glPushMatrix();
				Vec3f mid = (node->leftDownf + node->rightUpf) / 2;
				glTranslatef(mid[0], mid[1], mid[2]);
				drawMesh(node);
			glPopMatrix();
			
			for (size_t i = 0; i < node->child.size(); i++){
				animateTranslationRecur(target, node->child[i], amt);
				if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
					//std::cout << "splitting " << node->child[i]->m_nameString << endl;
					glScalef(-1, 1, 1);
					glRotatef(180, 0, 0, 1);
					animateTranslationRecur(target, node->child[i], amt);
					glScalef(1, 1, 1);
				}
			}		
		glPopMatrix();
	}
}

void TransformerAnimation::animateZRotationRecur(CString target, bone *node, float amt)
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
			//std::cout << "Rotating z " << node->m_nameString << " " << numTargetBoneFound << endl;
			//drawMesh(node);
			glPushMatrix();
				Vec3f mid = (node->leftDownf + node->rightUpf) / 2;
				glTranslatef(mid[0], mid[1], mid[2]);
				drawMesh(node);
			glPopMatrix();
			animateChildrenRecur(node, amt);
		glPopMatrix();
	} else if (numTargetBoneFound < numTargetBoneToAnimate) {
		glPushMatrix();
			if (node == m_skel->m_root){
				centerOriginWrtTorso();
			}

			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(node->m_angle[0], 1, 0, 0);

			//std::cout << "Rotating " << node->m_nameString << endl;
			drawMesh(node);
			//drawMesh(node);
			glPushMatrix();
				Vec3f mid = (node->leftDownf + node->rightUpf) / 2;
				glTranslatef(mid[0], mid[1], mid[2]);
				drawMesh(node);
			glPopMatrix();
			for (size_t i = 0; i < node->child.size(); i++){
				animateZRotationRecur(target, node->child[i], amt);
				if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
					glPushMatrix();
						glScalef(1, -1, 1);
						animateZRotationRecur(target, node->child[i], amt);
					glPopMatrix();
				}
			}
		glPopMatrix();
	}
}

void TransformerAnimation::animateYRotationRecur(CString target, bone *node, float amt)
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
			//std::cout << "Rotating y " << node->m_nameString << " " << numTargetBoneFound << endl;
			//drawMesh(node);
			glPushMatrix();
				Vec3f mid = (node->leftDownf + node->rightUpf) / 2;
				glTranslatef(mid[0], mid[1], mid[2]);
				drawMesh(node);
			glPopMatrix();

			animateChildrenRecur(node, amt);
		glPopMatrix();
	} else if (numTargetBoneFound < numTargetBoneToAnimate) {
		glPushMatrix();
			if (node == m_skel->m_root){
				centerOriginWrtTorso();
			}

			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(node->m_angle[0], 1, 0, 0);

			//std::cout << "Rotating " << node->m_nameString << endl;
			//drawMesh(node);
			glPushMatrix();
				Vec3f mid = (node->leftDownf + node->rightUpf) / 2;
				glTranslatef(mid[0], mid[1], mid[2]);
				drawMesh(node);
			glPopMatrix();

			for (size_t i = 0; i < node->child.size(); i++){
				animateYRotationRecur(target, node->child[i], amt);
				if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
					glPushMatrix();
						glScalef(1, -1, 1);
						animateYRotationRecur(target, node->child[i], amt);
					glPopMatrix();
				}
			}
		glPopMatrix();
	}
}

void TransformerAnimation::animateXRotationRecur(CString target, bone *node, float amt)
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

			//std::cout << "Rotating x " << node->m_nameString << " " << numTargetBoneFound << endl;
			//drawMesh(node);
			glPushMatrix();
				Vec3f mid = (node->leftDownf + node->rightUpf) / 2;
				glTranslatef(mid[0], mid[1], mid[2]);
				drawMesh(node);
			glPopMatrix();
			animateChildrenRecur(node, amt);
		glPopMatrix();
	} else if (numTargetBoneFound < numTargetBoneToAnimate) {
		glPushMatrix();
			if (node == m_skel->m_root){
				centerOriginWrtTorso();
			}

			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(node->m_angle[0], 1, 0, 0);

			//std::cout << "Rotating " << node->m_nameString << endl;
			//drawMesh(node);
			glPushMatrix();
				Vec3f mid = (node->leftDownf + node->rightUpf) / 2;
				glTranslatef(mid[0], mid[1], mid[2]);
				drawMesh(node);
			glPopMatrix();

			for (size_t i = 0; i < node->child.size(); i++){
				animateXRotationRecur(target, node->child[i], amt);
				if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
					glPushMatrix();
					glScalef(1, -1, 1);
					animateXRotationRecur(target, node->child[i], amt);
					glPopMatrix();
				}
			}
		glPopMatrix();
	}
}

void TransformerAnimation::animateChildrenRecur(bone *node, float amt)
{
	if (node == nullptr){
		return;
	}

	for (size_t i = 0; i < node->child.size(); i++){
		drawMesh(node->child[i]);

		lastChild = node->child[i]->m_name;
		//std::cout << "last child: " << node->child[i]->m_nameString << endl;

		animateChildrenRecur(node->child[i], amt);
	}
}

void TransformerAnimation::animateLocalRefineRecur(CString target, bone *node, float amt)
{
	if (node == nullptr){
		return;
	}

	glPushMatrix();
		if (node == m_skel->m_root){
			centerOriginWrtTorso();
		}

		glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
		glRotatef(node->m_angle[2], 0, 0, 1);
		glRotatef(node->m_angle[1], 0, 1, 0);
		glRotatef(node->m_angle[0], 1, 0, 0);

		glPushMatrix();
			Vec3f mid = (node->leftDownf + node->rightUpf) / 2;
			glTranslatef(amt*mid[0], amt*mid[1], amt*mid[2]);
			drawMesh(node);
		glPopMatrix();

		for (size_t i = 0; i < node->child.size(); i++){
			animateLocalRefineRecur(target, node->child[i], amt);
			if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
				glScalef(-1, 1, 1);
				glRotatef(180, 0, 0, 1);
				animateLocalRefineRecur(target, node->child[i], amt);
				glScalef(1, 1, 1);
			}
		}
	glPopMatrix();
}

// Assumes mesh is symmetric with x-z plane
void TransformerAnimation::setRotationCase(Vec3f localAxis)
{
	if (localAxis[X_AXIS] == X_AXIS){
		if (localAxis[Y_AXIS] == Y_AXIS){
			// 012, O'x'y'z' wrt Oxyz (Local axis of mesh is same as global axis)
			std::cout << "012 no change"<< endl;
			return;
		} else {
			// 021, O'x'z'y' wrt Oxyz
			std::cout << "021" << endl;
			glRotatef(90, 1, 0 , 0);
			glRotatef(180, 1, 0, 0);	// added from meshcutting
		}
	}
	else if (localAxis[X_AXIS] == Y_AXIS){
		if (localAxis[Y_AXIS] == X_AXIS){
			// 102, O'y'x'z' wrt Oxyz
			std::cout << "102" << endl;
			glRotatef(90, 0, 0, 1);
			// No need scaling as symmetric
		}
		else {
			// 120, O'y'z'x' wrt Oxyz
			std::cout << "120" << endl;
			glRotatef(-90, 1, 0, 0);
			glRotatef(-90, 0, 0, 1);
			// Exact
		}
	} else {
		if (localAxis[Y_AXIS] == X_AXIS){
			// 201, O'z'x'y' wrt Oxyz
			std::cout << "201" << endl;
			glRotatef(90, 0, 1, 0);
			glRotatef(90, 0, 0, 1);
			// Exact
		} else {
			// 210, O'z'y'x' wrt Oxyz
			std::cout << "210" << endl;
			//glRotatef(180, 1, 0, 0); // No need scaling as symmetric (removed from mesh cutting)
			glRotatef(90, 0, 1, 0);			
		}
	}
}

void TransformerAnimation::drawOpenedTransformer(bone *node)
{
	if (node == nullptr){
		return;
	}

	glPushMatrix();
		if (node == m_skel->m_root){
			centerOriginWrtTorso();
		}
		
		glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
		glRotatef(node->m_angle[2], 0, 0, 1);
		glRotatef(node->m_angle[1], 0, 1, 0);
		glRotatef(node->m_angle[0], 1, 0, 0);

		glPushMatrix();
			Vec3f mid = (node->leftDownf + node->rightUpf) / 2;
			glTranslatef(mid[0], mid[1], mid[2]);
			drawMesh(node);
		glPopMatrix();

		for (size_t i = 0; i < node->child.size(); i++){
			drawOpenedTransformer(node->child[i]);
			if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
				glScalef(-1, 1, 1);
				glRotatef(180, 0, 0, 1);
				drawOpenedTransformer(node->child[i]);
				glScalef(1, 1, 1);
			}
		}
	glPopMatrix();
}

void TransformerAnimation::centerOriginWrtTorso(){
	glTranslatef(m_mesh->getMeshCoordOrigin()[0][0],
		m_mesh->getMeshCoordOrigin()[0][1],
		m_mesh->getMeshCoordOrigin()[0][2]);
}

void TransformerAnimation::drawMesh(bone *node)
{
	glColor3fv(m_mesh->color[node->color].data());
	m_mesh->drawPolygonFace(node->mesh);
}

