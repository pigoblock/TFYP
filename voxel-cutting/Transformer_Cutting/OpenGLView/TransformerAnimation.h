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

	bool startAnimation;
	bool animationDone;

	float translateAmt;
	float rotateXAmt;
	float rotateYAmt;
	float rotateZAmt;
	float speed;

	CString currentBone;
	bool targetBoneFound;
	int currentBoneIdx;
	bool posAnimated[4];

	// Animation methods
	void restartAnimation();
	void animate();
	void animateTranslation(CString currBone, bone *rootBone, float amt);
	void drawOneTransformerPart(bone *node, CString targetName);

private:
	// Recursive methods that support animation methods
	void animateTranslationRecur(CString target, bone *node, float amt, int colorIndex);
};

