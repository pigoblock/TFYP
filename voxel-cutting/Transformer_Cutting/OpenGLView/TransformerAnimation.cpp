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

		if (!posAnimated[CONNECTING_BONE_ROTATION]){
			if (currentBoneIdx > 0 &&
				transformer->tBoneArray[currentBoneIdx]->m_connectingParent->m_unfoldAngle[0] != 0){
				animStep = CONNECTING_BONE_ROTATION;
				if (animationAmt > 1){
					posAnimated[CONNECTING_BONE_ROTATION] = true;
					animStep = BONE_SHELL_ROTATION;
					animationAmt = 0;
					unfoldTransformer(currentBone, transformer->transformerRootBone, animationAmt);
				} else {
					std::cout << "Index: " << currentBoneIdx << " in connecting bone rotation, amt: " << animationAmt << endl;
					unfoldTransformer(currentBone, transformer->transformerRootBone, animationAmt);

					if (!pauseAnimation){
						animationAmt += speed;
					}
				}
			} else {
				posAnimated[CONNECTING_BONE_ROTATION] = true;
			}
		} else if (!posAnimated[BONE_SHELL_ROTATION]){
			if (transformer->tBoneArray[currentBoneIdx]->m_unfoldAngle[0] != 0){
				animStep = BONE_SHELL_ROTATION;
				if (animationAmt > 1){
					posAnimated[BONE_SHELL_ROTATION] = true;
					animationAmt = 0;
				} else {
					std::cout << "Index: " << currentBoneIdx << " in bone shell rotation, amt: " << animationAmt << endl;
					unfoldTransformer(currentBone, transformer->transformerRootBone, animationAmt);

					if (!pauseAnimation){
						animationAmt += speed;
					}
				}
			} else {
				posAnimated[BONE_SHELL_ROTATION] = true;
				animationAmt = 0;
			}
		}

		// Positioning for this bone is done
		if (posAnimated[BONE_SHELL_ROTATION]) {
			std::cout << "Finished rotation." << endl;
			currentBoneIdx++;
			if (currentBoneIdx >= transformer->tBoneArray.size()){
				// All bones have been processed
				animationDone = true;
				unfoldTransformer(currentBone, transformer->transformerRootBone, 1);
			} else {
				// Start processing the next bone
				for (int i = 0; i < NUM_ANIMATION_STEPS; i++){
					posAnimated[i] = false;
				}
				currentBone = transformer->tBoneArray[currentBoneIdx];
				animStep = CONNECTING_BONE_ROTATION;
				animationAmt = 0;
				unfoldTransformer(currentBone, transformer->transformerRootBone, animationAmt);
			}
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
		if (node->m_name == target->m_name){
			if (node->m_connectingParent){
				glTranslatef(node->m_connectingParent->m_unfoldCoord[0], node->m_connectingParent->m_unfoldCoord[1],
					node->m_connectingParent->m_unfoldCoord[2]);
				glRotatef(node->m_connectingParent->m_foldAngle[0], node->m_connectingParent->m_foldAngle[1],
					node->m_connectingParent->m_foldAngle[2], node->m_connectingParent->m_foldAngle[3]);
				if (animStep == CONNECTING_BONE_ROTATION){
					glRotatef(amt*node->m_connectingParent->m_unfoldAngle[0], node->m_connectingParent->m_unfoldAngle[1],
						node->m_connectingParent->m_unfoldAngle[2], node->m_connectingParent->m_unfoldAngle[3]);
				} else {
					glRotatef(node->m_connectingParent->m_unfoldAngle[0], node->m_connectingParent->m_unfoldAngle[1],
						node->m_connectingParent->m_unfoldAngle[2], node->m_connectingParent->m_unfoldAngle[3]);					
				}

				glColor3f(1, 1, 1);
				node->drawSphereJoint(1);
				node->drawCylinderBone(node->m_connectingParent->m_unfoldedLength, 0.5);
			}

			glTranslatef(node->m_unfoldCoord[0], node->m_unfoldCoord[1], node->m_unfoldCoord[2]);
			glRotatef(node->m_foldAngle[0], node->m_foldAngle[1], node->m_foldAngle[2], node->m_foldAngle[3]);
			if (animStep == BONE_SHELL_ROTATION){
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
			else {
				if (node->m_connectingParent){
					glTranslatef(node->m_connectingParent->m_unfoldCoord[0], node->m_connectingParent->m_unfoldCoord[1],
						node->m_connectingParent->m_unfoldCoord[2]);
					glRotatef(node->m_connectingParent->m_foldAngle[0], node->m_connectingParent->m_foldAngle[1],
						node->m_connectingParent->m_foldAngle[2], node->m_connectingParent->m_foldAngle[3]);

					glColor3f(1, 1, 1);
					node->drawSphereJoint(1);
					node->drawCylinderBone(node->m_connectingParent->m_unfoldedLength, 0.5);
				}

				glTranslatef(node->m_unfoldCoord[0], node->m_unfoldCoord[1], node->m_unfoldCoord[2]);
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

