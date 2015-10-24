#pragma once
#include "stdafx.h"
#include "skeleton.h"
#include "MeshCutting.h"

enum AnimationStep{
	TRANSLATION,
	Z_ROTATION,
	Y_ROTATION,
	X_ROTATION,
	LOCAL_TRANSLATE_REFINE,
	LOCAL_ROTATE_REFINE,
	DONE,
	NUM_ANIMATION_STEPS
};

enum AnimationMode{
	PLAY_ANIMATION,
	PAUSE_ANIMATION,
	RESTART_ANIMATION,
	ANIMATION_MODE_SIZE
};

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

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
	bool posAnimated[NUM_ANIMATION_STEPS];
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
	// Recursive methods that support animation methods
	void animateRecur(CString target, bone *node, float amt);
	void animateLocalRefineRecur(CString target, bone *node, float amt);

	void animateChildrenRecur(bone *node, float amt);
	void drawOpenedTransformer(bone *node);
	void centerOriginWrtTorso();
	bool isInCorrectLocalPosition(bone *boneNode);
	void setOriginalMeshRotation(Vec3f localAxis);
	void setSkeletonRotation(Vec3f localAxis, float amt);
	void drawMesh(bone *node);

	bool transformDone;
};

