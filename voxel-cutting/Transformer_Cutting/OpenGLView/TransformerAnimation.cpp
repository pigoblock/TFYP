#include "stdafx.h"
#include "TransformerAnimation.h"
#include "mirrorDraw.h"
#include <iostream>

using namespace std;

TransformerAnimation::TransformerAnimation()
{
	m_mesh = nullptr;
	m_skel = nullptr;
	transformer = nullptr;

	speed = 0.01;

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
	currentBoneIdx = 0;
	boneAnimationStage = CONNECTING_BONE_ROTATION;
	
	for (int i = 0; i < NUM_ANIMATION_STEPS; i++){
		posAnimated[i] = false;
	}

	startAnimation = false;
	pauseAnimation = true;
	animationDone = false;
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

void TransformerAnimation::animateTransformer()
{
	if (!animationDone){
		// Determine current bone/cut mesh to be animated
		currentBone = transformer->tBoneArray[currentBoneIdx];

		if (boneAnimationStage == CONNECTING_BONE_ROTATION){
			if (currentBoneIdx > 0 &&
				transformer->tBoneArray[currentBoneIdx]->m_connectingParent->m_unfoldAngle[0] != 0){
				if (animationAmt > 1){
					boneAnimationStage = CONNECTING_BONE_LENGTH;
					animationAmt = 0;
				} 
			} else {
				boneAnimationStage = CONNECTING_BONE_LENGTH;
			}
		}

		if (boneAnimationStage == CONNECTING_BONE_LENGTH){
			if (currentBoneIdx > 0 &&
				transformer->tBoneArray[currentBoneIdx]->m_foldCoord != transformer->tBoneArray[currentBoneIdx]->m_unfoldCoord){
				if (animationAmt > 1){
					boneAnimationStage = BONE_SHELL_ROTATION;
					animationAmt = 0;
				}
			} else {
				boneAnimationStage = BONE_SHELL_ROTATION;
			}
		}

		if (boneAnimationStage == BONE_SHELL_ROTATION){
			if (transformer->tBoneArray[currentBoneIdx]->m_unfoldAngle[0] != 0){
				if (animationAmt > 1){
					boneAnimationStage = DONE;
					animationAmt = 0;
				}
			} else {
				boneAnimationStage = DONE;
			}
		}

		if (boneAnimationStage == DONE){
			currentBoneIdx++;
			if (currentBoneIdx >= transformer->tBoneArray.size()){
				// All bones have been processed
				animationDone = true;
				animationAmt = 1;
				boneAnimationStage = BONE_SHELL_ROTATION;
			} else {
				// Start processing the next bone
				currentBone = transformer->tBoneArray[currentBoneIdx];
				boneAnimationStage = CONNECTING_BONE_ROTATION;
				animationAmt = 0;
			}
		}

		unfoldTransformer(currentBone, transformer->transformerRootBone, animationAmt);

		if (!pauseAnimation){
			animationAmt += speed;
		}

		_sleep(0.1);
	}
	else {
		TransformerBone *lastBone = transformer->tBoneArray[transformer->tBoneArray.size()-1];
		unfoldTransformer(lastBone, transformer->transformerRootBone, 1);
	}
}

void TransformerAnimation::unfoldTransformer(TransformerBone *target, TransformerBone *node, float amt){
	if (node == nullptr){
		return;
	}
	
	glPushMatrix();
		// In unfolding process
		if (node->m_name == target->m_name){
			if (node->m_connectingParent){
				glTranslatef(node->m_connectingParent->m_unfoldCoord[0], node->m_connectingParent->m_unfoldCoord[1],
					node->m_connectingParent->m_unfoldCoord[2]);
				glRotatef(node->m_connectingParent->m_foldAngle[0], node->m_connectingParent->m_foldAngle[1],
					node->m_connectingParent->m_foldAngle[2], node->m_connectingParent->m_foldAngle[3]);
				if (boneAnimationStage == CONNECTING_BONE_ROTATION){
					glRotatef(amt*node->m_connectingParent->m_unfoldAngle[0], node->m_connectingParent->m_unfoldAngle[1],
						node->m_connectingParent->m_unfoldAngle[2], node->m_connectingParent->m_unfoldAngle[3]);

					glColor3f(1, 1, 1);
					node->drawSphereJoint(1);
					node->drawCylinderBone(node->m_connectingParent->m_foldedLength, 0.5);
				} else {
					glRotatef(node->m_connectingParent->m_unfoldAngle[0], node->m_connectingParent->m_unfoldAngle[1],
						node->m_connectingParent->m_unfoldAngle[2], node->m_connectingParent->m_unfoldAngle[3]);

					glColor3f(1, 1, 1);
					node->drawSphereJoint(1);
					if (boneAnimationStage == CONNECTING_BONE_LENGTH){			
						float foldUnfoldDist = node->m_connectingParent->m_unfoldedLength - node->m_connectingParent->m_foldedLength;
						float intermediateDist = node->m_connectingParent->m_foldedLength + foldUnfoldDist * amt;
						node->drawCylinderBone(intermediateDist, 0.5);
					} else {
						node->drawCylinderBone(node->m_connectingParent->m_unfoldedLength, 0.5);
					}						
				}	
			}

			if (boneAnimationStage == CONNECTING_BONE_ROTATION){
				glTranslatef(node->m_foldCoord[0], node->m_foldCoord[1], node->m_foldCoord[2]);
			} else if (boneAnimationStage == CONNECTING_BONE_LENGTH){
				Vec3f foldUnfoldDist = node->m_unfoldCoord - node->m_foldCoord;
				Vec3f translateDist = node->m_foldCoord + foldUnfoldDist * amt;
				glTranslatef(translateDist[0], translateDist[1], translateDist[2]);
			} else {
				glTranslatef(node->m_unfoldCoord[0], node->m_unfoldCoord[1], node->m_unfoldCoord[2]);
			}
			
			glRotatef(node->m_foldAngle[0], node->m_foldAngle[1], node->m_foldAngle[2], node->m_foldAngle[3]);
			if (boneAnimationStage == BONE_SHELL_ROTATION){
				glRotatef(amt*node->m_unfoldAngle[0], node->m_unfoldAngle[1], node->m_unfoldAngle[2], node->m_unfoldAngle[3]);
			} 

			glColor3fv(MeshCutting::color[node->m_index].data());
			node->drawSphereJoint(1);
			node->drawCylinderBone(node->m_length, 0.5);
			//node->drawMesh();
		}
		else {
			// Already unfolded
			if (node->m_index < target->m_index){
				if (node->m_connectingParent){
					glTranslatef(node->m_connectingParent->m_unfoldCoord[0], node->m_connectingParent->m_unfoldCoord[1],
						node->m_connectingParent->m_unfoldCoord[2]);
					glRotatef(node->m_connectingParent->m_foldAngle[0], node->m_connectingParent->m_foldAngle[1],
						node->m_connectingParent->m_foldAngle[2], node->m_connectingParent->m_foldAngle[3]);
					glRotatef(node->m_connectingParent->m_unfoldAngle[0], node->m_connectingParent->m_unfoldAngle[1],
						node->m_connectingParent->m_unfoldAngle[2], node->m_connectingParent->m_unfoldAngle[3]);

					glColor3f(1, 1, 1);
					node->drawSphereJoint(1);
					node->drawCylinderBone(node->m_connectingParent->m_unfoldedLength, 0.5);
				}

				glTranslatef(node->m_unfoldCoord[0], node->m_unfoldCoord[1], node->m_unfoldCoord[2]);
				glRotatef(node->m_foldAngle[0], node->m_foldAngle[1], node->m_foldAngle[2], node->m_foldAngle[3]);
				glRotatef(node->m_unfoldAngle[0], node->m_unfoldAngle[1], node->m_unfoldAngle[2], node->m_unfoldAngle[3]);

				glColor3fv(MeshCutting::color[node->m_index].data());
				node->drawSphereJoint(1);
				node->drawCylinderBone(node->m_length, 0.5);
				//node->drawMesh();
			}
			// Still folded
			else {
				if (node->m_connectingParent){
					glTranslatef(node->m_connectingParent->m_foldCoord[0], node->m_connectingParent->m_foldCoord[1],
						node->m_connectingParent->m_foldCoord[2]);
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
				//node->drawMesh();
			}
		}

		for (size_t i = 0; i < node->m_children.size(); i++){
			unfoldTransformer(target, node->m_children[i], amt);

			if (node == transformer->transformerRootBone && node->m_children[i]->m_type == TYPE_SIDE_BONE){
				glPushMatrix();
					glScalef(-1, 1, 1);
					unfoldTransformer(target, node->m_children[i], amt);
				glPopMatrix();
			}
		}
	glPopMatrix();
}

