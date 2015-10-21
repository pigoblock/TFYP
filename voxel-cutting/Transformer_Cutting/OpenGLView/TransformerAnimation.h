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

enum AnimationMode{
	PLAY_ANIMATION,
	PAUSE_ANIMATION,
	RESTART_ANIMATION,
	ANIMATION_MODE_SIZE
};

class TransformerAnimation
{
public:
	TransformerAnimation();
	~TransformerAnimation();

	MeshCutting * m_mesh;
	skeleton *m_skel;
	SurfaceObj *m_surObj;

private:
	bool startAnimation;
	bool pauseAnimation;
	bool animationDone;

	float animationAmt;
	float speed;

public:
	CString currentBone;
	CString lastChild;
	int numTargetBoneFound;
	int numTargetBoneToAnimate;
	int currentBoneIdx;
	bool posAnimated[4];
	AnimationStep animStep;

	// Animation methods
	void stopAnimation();
	void playAnimation();
	void restartAnimation();
	void animate();
	void processAnimation(CString currBone, bone *rootBone, float amt);

	// Debugging and testing animation methods
	void testAnimate();

private:
	int colorIndex;

	// Recursive methods that support animation methods
	void animateTranslationRecur(CString target, bone *node, float amt);
	void animateZRotationRecur(CString target, bone *node, float amt);
	void animateYRotationRecur(CString target, bone *node, float amt);
	void animateXRotationRecur(CString target, bone *node, float amt);
	void animateRecur(bone *node, float amt);

	void drawMesh(bone *node);
	void drawOpenedTransformer(bone *node, int colorIndex);
	void drawClosedTransformer();
};

