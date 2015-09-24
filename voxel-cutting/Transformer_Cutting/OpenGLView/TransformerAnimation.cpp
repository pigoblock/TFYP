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
