#pragma once
#include "stdafx.h"
#include "skeleton.h"
#include "MeshCutting.h"

enum AnimationStep{
	TRANSLATION,
	Z_ROTATION,
	Y_ROTATION,
	X_ROTATION,
	DONE
};

class TransformerAnimation
{
public:
	TransformerAnimation();
	~TransformerAnimation();

	MeshCutting * m_mesh;
	skeleton *m_skel;

	bool startAnimation;
	bool animationDone;

	float animationAmt;
	float speed;

	CString currentBone;
	int targetBoneFound;
	int currentBoneIdx;
	bool posAnimated[4];
	AnimationStep animMode;

	// Animation methods
	void restartAnimation();
	void animate();
	void processAnimation(CString currBone, bone *rootBone, float amt);
	void drawOneTransformerPart(bone *node, CString targetName);

private:
	// Recursive methods that support animation methods
	void animateTranslationRecur(CString target, bone *node, float amt, int colorIndex);
	void animateZRotationRecur(CString target, bone *node, float amt, int colorIndex);
	void animateYRotationRecur(CString target, bone *node, float amt, int colorIndex);
	void animateXRotationRecur(CString target, bone *node, float amt, int colorIndex);

	void drawOpenedTransformer(bone *node, int colorIndex);
};

