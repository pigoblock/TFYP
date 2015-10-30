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

	speed = 0.1;

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
	
	for (int i = 0; i < NUM_ANIMATION_STEPS; i++){
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
	//drawOpenedTransformer(m_skel->m_root);
	if (!animationDone){
		if (!transformDone){
			// Determine current bone/cut mesh to be animated
			currentBone = m_mesh->boneArray[currentBoneIdx]->m_name;
			//std::cout << "current bone: " << m_mesh->boneArray[currentBoneIdx]->m_nameString << endl;

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
				}
				else {
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
				}
				else {
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
						std::cout << "Finished x rotation." << endl;
					}
					else {
						processAnimation(currentBone, m_skel->m_root, animationAmt);

						if (!pauseAnimation){
							animationAmt += speed;
						}
					}
				}
				else {
					posAnimated[X_ROTATION] = true;
					animationAmt = 0;
				}
			}

			// Positioning for this bone is done
			if (posAnimated[X_ROTATION]) {
				currentBoneIdx++;
				if (currentBoneIdx >= m_mesh->boneArray.size()){
					// All bones have been processed
					//startAnimation = false;
					//animationDone = true;
					transformDone = true;
					animationAmt = 0;
					animStep = LOCAL_TRANSLATE_REFINE;
					currentBoneIdx = 0;
					//drawOpenedTransformer(m_skel->m_root);
					processAnimation(currentBone, m_skel->m_root, animationAmt);
				}
				else {
					// Start processing the next bone
					for (int i = 0; i < NUM_ANIMATION_STEPS; i++){
						posAnimated[i] = false;
					}
					currentBone = m_mesh->boneArray[currentBoneIdx]->m_name;
					animStep = TRANSLATION;
					processAnimation(currentBone, m_skel->m_root, 0);
				}
			}
			_sleep(0.1);
		}
		else{
			// Start doing local translation refinement
			if (animStep == LOCAL_TRANSLATE_REFINE){
				if (animationAmt > 1){
					drawOpenedTransformer(m_skel->m_root);
					animationAmt = 0;
					animStep = LOCAL_ROTATE_REFINE;
				}
				else {
					processAnimation(currentBone, m_skel->m_root, animationAmt);
				}
				if (!pauseAnimation){
					animationAmt += speed;
				}
				_sleep(0.1);
			}
			else {
				// Do local rotation refinement
				if (animationAmt > 1){
					drawOpenedTransformer(m_skel->m_root);
					animationAmt = 0;
					animStep = DONE;
					startAnimation = false;
					animationDone = true;
				}
				else {
					processAnimation(currentBone, m_skel->m_root, animationAmt);
					if (!pauseAnimation){
						animationAmt += speed;
					}
					_sleep(0.1);
				}
			}
		}
	} else {
		drawOpenedTransformer(m_skel->m_root);
	}
}

void TransformerAnimation::processAnimation(CString currBone, bone *rootBone, float amt)
{
	lastChild = currBone;
	bool lastChildFound = false;

	// Animate current bone and all its children
	if (animStep < LOCAL_TRANSLATE_REFINE){
		animateGeneralSkeletonRecur(currBone, rootBone, amt);

		// Draw all other bone meshes as the original mesh
		mirrorDraw mirror;
		mirror.mirrorAxis = 0; //x-axis
		mirror.mirrorCoord = m_surObj->midPoint()[0];

		// Draws shape of original mesh
		for (int i = 0; i < m_mesh->boneArray.size(); i++){
			if (m_mesh->boneArray[i]->m_name == lastChild){
				lastChildFound = true;
			} else if (lastChildFound){
				glColor3fv(m_mesh->color[i].data());
				glPushMatrix();
					Vec3f originalMeshPosition = m_mesh->getMeshCoordOrigin()[i];
					glTranslatef(originalMeshPosition[0], originalMeshPosition[1], originalMeshPosition[2]);
					drawMesh(m_mesh->boneArray[i]);
				glPopMatrix();
				if (m_mesh->boneArray[i]->m_type == TYPE_SIDE_BONE){
					glPushMatrix();
						glTranslatef(mirror.mirrorCoord, 0, 0);
						glScalef(-1, 1, 1);
						glTranslatef(originalMeshPosition[0], originalMeshPosition[1], originalMeshPosition[2]);
						drawMesh(m_mesh->boneArray[i]);
					glPopMatrix();
				}
			}
		}
	}
	else {
		if (animStep == LOCAL_TRANSLATE_REFINE){
			std::cout << "doing local translate refine " << amt << endl;
			animateLocalTranslateRecur(rootBone, amt);
		}
		else {
			animateLocalRotateRecur(rootBone, amt);
		}
	}	
}

void TransformerAnimation::animateChildrenRecur(bone *node, float amt, bone* baseParentNode,
	Vec3f parentPos, Vec3f parentNewPos)
{
	if (node == nullptr){
		return;
	}

	for (size_t i = 0; i < node->child.size(); i++){
		glPushMatrix();
			drawMesh(node->child[i]);
		glPopMatrix();

		lastChild = node->child[i]->m_name;
		//std::cout << "last child: " << node->child[i]->m_nameString << endl;

		animateChildrenRecur(node->child[i], amt, baseParentNode, parentPos, parentNewPos);
	}
}

void TransformerAnimation::animateGeneralSkeletonRecur(CString target, bone *node, float amt)
{
	if (node == nullptr){
		return;
	}
	
	// Animate target bone
	if (node->m_name == target){
		numTargetBoneFound += 1;
		glPushMatrix();	
			Vec3f originalMeshPosition = m_mesh->getMeshCoordOrigin()[node->color];
			Vec3f newPos = node->m_posCoord - originalMeshPosition;

			if (animStep == TRANSLATION){
				glTranslatef(originalMeshPosition[0], originalMeshPosition[1], originalMeshPosition[2]);
				glTranslatef(amt*newPos[0], amt*newPos[1], amt*newPos[2]);
			}
			else {
				glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);

				if (animStep == Z_ROTATION){
					glRotatef(amt*node->m_angle[2], 0, 0, 1);
				}
				else {
					glRotatef(node->m_angle[2], 0, 0, 1);

					if (animStep == Y_ROTATION){
						glRotatef(amt*node->m_angle[1], 0, 1, 0);
					}
					else {
						glRotatef(node->m_angle[1], 0, 1, 0);

						if (animStep == X_ROTATION){
							glRotatef(amt*node->m_angle[0], 1, 0, 0);
						}
						else {
							glRotatef(node->m_angle[0], 1, 0, 0);
						}
					}
				}
			}	
		
			drawMesh(node);
			animateChildrenRecur(node, amt, node, originalMeshPosition, newPos);
		glPopMatrix();
	}
	else if (numTargetBoneFound < numTargetBoneToAnimate) {
		glPushMatrix();
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			glRotatef(node->m_angle[2], 0, 0, 1);
			glRotatef(node->m_angle[1], 0, 1, 0);
			glRotatef(node->m_angle[0], 1, 0, 0);

			glPushMatrix();
				if (node == m_skel->m_root){
					glTranslatef(m_mesh->getMeshCoordOrigin()[0][0], 
						m_mesh->getMeshCoordOrigin()[0][1], m_mesh->getMeshCoordOrigin()[0][2]);
				}

				drawMesh(node);
			glPopMatrix();

			for (size_t i = 0; i < node->child.size(); i++){
				animateGeneralSkeletonRecur(target, node->child[i], amt);
				if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
					glPushMatrix();
						glScalef(-1, 1, 1);
						animateGeneralSkeletonRecur(target, node->child[i], amt);
					glPopMatrix();
				}
			}
		glPopMatrix();
	}
}

void TransformerAnimation::animateLocalTranslateRecur(bone *node, float amt)
{
	if (node == nullptr){
		return;
	}

	glPushMatrix();
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
			animateLocalTranslateRecur(node->child[i], amt);
			if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
				glScalef(-1, 1, 1);
				animateLocalTranslateRecur(node->child[i], amt);
			}
		}
	glPopMatrix();
}

void TransformerAnimation::animateLocalRotateRecur(bone *node, float amt)
{
	if (node == nullptr){
		return;
	}

	glPushMatrix();
		glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
		glRotatef(node->m_angle[2], 0, 0, 1);
		glRotatef(node->m_angle[1], 0, 1, 0);
		glRotatef(node->m_angle[0], 1, 0, 0);

		glPushMatrix();
			Vec3f mid = (node->leftDownf + node->rightUpf) / 2;
			glTranslatef(mid[0], mid[1], mid[2]);
			setSkeletonRotation(m_mesh->coords[node->color], amt);
			drawMesh(node);
		glPopMatrix();

		for (size_t i = 0; i < node->child.size(); i++){
			animateLocalRotateRecur(node->child[i], amt);
			if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
				glScalef(-1, 1, 1);
				animateLocalRotateRecur(node->child[i], amt);
			}
		}
	glPopMatrix();
}

// Assumes mesh is symmetric with x-z plane
// Rotates a cut piece to fit its orientation in skeleton
void TransformerAnimation::setSkeletonRotation(Vec3f localAxis, float amt)
{
	if (localAxis[X_AXIS] == X_AXIS){
		if (localAxis[Y_AXIS] == Y_AXIS){
			// 012, O'x'y'z' wrt Oxyz (Local axis of mesh is same as global axis)
			return;
		} else {
			// 021, O'x'z'y' wrt Oxyz
			glRotatef(amt*-90, 1, 0, 0);
		}
	} else if (localAxis[X_AXIS] == Y_AXIS){
		if (localAxis[Y_AXIS] == X_AXIS){
			// 102, O'y'x'z' wrt Oxyz
			glRotatef(amt*-90, 0, 0, 1);
		} else {
			// 120, O'y'z'x' wrt Oxyz
			if (amt <= 0.5){
				glRotatef(amt*2*-90, 0, 1, 0);
			} else {
				glRotatef(-90, 0, 1, 0);
				glRotatef((amt-0.5)*2*-90, 1, 0, 0);
			}
		}
	} else {
		if (localAxis[Y_AXIS] == X_AXIS){
			// 201, O'z'x'y' wrt Oxyz
			if (amt <= 0.5){
				glRotatef(amt * 2 * 90, 0, 0, 1);
			} else {
				glRotatef(90, 0, 0, 1);
				glRotatef((amt - 0.5) * 2 * 90, 1, 0, 0);
			}	
		} else {
			// 210, O'z'y'x' wrt Oxyz
			glRotatef(amt*90, 0, 1, 0);
		}
	}
}

void TransformerAnimation::drawOpenedTransformer(bone *node)
{
	if (node == nullptr){
		return;
	}

	glPushMatrix();	
		glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
		glRotatef(node->m_angle[2], 0, 0, 1);
		glRotatef(node->m_angle[1], 0, 1, 0);
		glRotatef(node->m_angle[0], 1, 0, 0);

		glPushMatrix();
			Vec3f mid = (node->leftDownf + node->rightUpf) / 2;
			glTranslatef(mid[0], mid[1], mid[2]);
			setSkeletonRotation(node->transformCoords, 1);
			drawMesh(node);
		glPopMatrix();

		for (size_t i = 0; i < node->child.size(); i++){
			drawOpenedTransformer(node->child[i]);
			if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
				glScalef(-1, 1, 1);
				drawOpenedTransformer(node->child[i]);
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

// Old not in use
void TransformerAnimation::animateRecur(CString target, bone *node, float amt)
{
	if (node == nullptr){
		return;
	}

	// Animate target bone
	if (node->m_name == target){
		numTargetBoneFound += 1;
		glPushMatrix();
		Vec3f originalMeshPosition = m_mesh->getMeshCoordOrigin()[node->color]
			- m_mesh->getMeshCoordOrigin()[node->parent->color];

		Vec3f mid = (node->leftDownf + node->rightUpf) / 2;
		Vec3f parentMid = (node->parent->leftDownf + node->parent->rightUpf) / 2;
		Vec3f translateDirection = node->m_posCoord - parentMid;

		if (animStep == TRANSLATION){
			// Keep the position that the cut piece was originally in the mesh piece
			glTranslatef(originalMeshPosition[0], originalMeshPosition[1], originalMeshPosition[2]);

			// Actual animation of movement
			glTranslatef(amt*translateDirection[0], amt*translateDirection[1], amt*translateDirection[2]);
		}
		else {
			glTranslatef(translateDirection[0], translateDirection[1], translateDirection[2]);

			if (animStep == Z_ROTATION){
				glRotatef(amt*node->m_angle[2], 0, 0, 1);
			}
			else {
				glRotatef(node->m_angle[2], 0, 0, 1);

				if (animStep == Y_ROTATION){
					glRotatef(amt*node->m_angle[1], 0, 1, 0);
				}
				else {
					glRotatef(node->m_angle[1], 0, 1, 0);

					if (animStep == X_ROTATION){
						glRotatef(amt*node->m_angle[0], 1, 0, 0);
					}
					else {
						glRotatef(node->m_angle[0], 1, 0, 0);
						/*
						if (animStep == LOCAL_TRANSLATE_REFINE){
						glTranslatef(amt*mid[0], amt*mid[1], amt*mid[2]);
						}
						else {
						glTranslatef(mid[0], mid[1], mid[2]);

						// TODO: Do local rotation refinement animation
						//setSkeletonRotation(m_mesh->coords[node->color], amt);
						}*/
					}
				}
			}
		}

		glPushMatrix();
		//setOriginalMeshRotation(m_mesh->coords[node->color]);
		drawMesh(node);
		glPopMatrix();

		//animateChildrenRecur(node, amt);
		glPopMatrix();
	}
	else if (numTargetBoneFound < numTargetBoneToAnimate) {
		glPushMatrix();
		if (node == m_skel->m_root){
			glTranslatef(node->m_posCoord[0], node->m_posCoord[1], node->m_posCoord[2]);
			// Move the whole mesh to place origin at torso
			glTranslatef(m_mesh->getMeshCoordOrigin()[node->color][0],
				m_mesh->getMeshCoordOrigin()[node->color][1],
				m_mesh->getMeshCoordOrigin()[node->color][2]);

			Vec3f mid = (node->leftDownf + node->rightUpf) / 2;
			glTranslatef(mid[0], mid[1], mid[2]);
		}
		else {
			Vec3f parentMid = (node->parent->leftDownf + node->parent->rightUpf) / 2;
			Vec3f translateDirection = node->m_posCoord - parentMid;
			glTranslatef(translateDirection[0], translateDirection[1], translateDirection[2]);
		}
		glRotatef(node->m_angle[2], 0, 0, 1);
		glRotatef(node->m_angle[1], 0, 1, 0);
		glRotatef(node->m_angle[0], 1, 0, 0);

		if (node == m_skel->m_root){
			glPushMatrix();
			//setOriginalMeshRotation(m_mesh->coords[node->color]);
			drawMesh(node);
			glPopMatrix();
		}
		else{
			Vec3f currentMid = (node->leftDownf + node->rightUpf) / 2;
			glTranslatef(currentMid[0], currentMid[1], currentMid[2]);
			glPushMatrix();
			//setOriginalMeshRotation(m_mesh->coords[node->color]);	// commented out for testing
			drawMesh(node);
			glPopMatrix();
		}

		for (size_t i = 0; i < node->child.size(); i++){
			animateRecur(target, node->child[i], amt);
			if (node == m_mesh->boneArray[0] && node->child[i]->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
				glScalef(-1, 1, 1);
				animateRecur(target, node->child[i], amt);
				glPopMatrix();
			}
		}
		glPopMatrix();
	}
}
