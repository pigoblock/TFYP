#include "stdafx.h"
#include "TransformerAnimation.h"


TransformerAnimation::TransformerAnimation()
{
	m_mesh = nullptr;
	m_skel = nullptr;

	translateAmt = 0;
	rotateXAmt = 0;
	rotateYAmt = 0;
	rotateZAmt = 0;
	speed = 0.05;

	startAnimation = true;
	animationDone = false;
}


TransformerAnimation::~TransformerAnimation()
{
	// Deletion of m_mesh and m_skel already handled by myDocument
	// as m_tAnimation is deleted after m_skeleton and m_MeshCutting
}

void TransformerAnimation::restartAnimation()
{
	animationDone = false;
}

void TransformerAnimation::animateTransformer(CString currBone, bone *rootBone,
	float tAmt, float rxAmt, float ryAmt, float rzAmt)
{
	// Displays the cut and colored mesh cut pieces
	// Note: have to press F to cut mesh first before this works

	bool currBoneFound = false;
	for (int i = 1; i < m_mesh->boneArray.size(); i++){
		glColor3fv(m_mesh->color[i].data());
		// Found the bone we are supposed to animate
		if (m_mesh->boneArray[i]->m_name == currBone){
			
			currBoneFound = true;
			//parent problem
			glPushMatrix();
			glTranslatef(tAmt*m_mesh->boneArray[i]->m_posCoord[0], tAmt*m_mesh->boneArray[i]->m_posCoord[1],
				tAmt*m_mesh->boneArray[i]->m_posCoord[2]);
			m_mesh->drawPolygonFace(m_mesh->boneArray[i]->mesh);

			for (int j = 0; j < m_mesh->boneArray[i]->child.size(); j++){
				m_mesh->drawPolygonFace(m_mesh->boneArray[i]->child[j]->mesh);
			}
			glPopMatrix();
		}
		else if (currBoneFound){
			// TODO: exclude children, array, mark drawn
			// Bones not yet animated, draw them in the mesh position
			m_mesh->drawPolygonFace(m_mesh->boneArray[i]->mesh);
		}
		else {
			// Bones were already animated so can draw them in the transformer position
			drawOneTransformerPart(rootBone, m_mesh->boneArray[i]->m_name);
		}
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
