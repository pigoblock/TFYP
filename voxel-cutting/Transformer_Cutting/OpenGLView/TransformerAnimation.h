#pragma once
#include "stdafx.h"
#include "skeleton.h"
#include "MeshCutting.h"

class TransformerAnimation
{
public:
	TransformerAnimation();
	~TransformerAnimation();

	MeshCutting * m_mesh;
	skeleton *m_skel;

	float translateAmt;
	float rotateXAmt;
	float rotateYAmt;
	float rotateZAmt;
	float speed;

	bool startAnimation;
	bool animationDone;

	// Methods
	void restartAnimation();
	void animateTransformer(CString currBone, bone *rootBone,
		float tAmt, float rxAmt, float ryAmt, float rzAmt);
	void drawOneTransformerPart(bone *node, CString targetName);
};

