#pragma once
#include "stdafx.h"
#include "skeleton.h"
#include "MeshCutting.h"
#include "TransformerSkeleton.h"

enum AnimationStep{
	CONNECTING_BONE_ROTATION,
	BONE_SHELL_ROTATION,
	//CONNECTING_BONE_LENGTH,
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

	TransformerSkeleton *transformer;

private:
	bool startAnimation;
	bool pauseAnimation;
	bool animationDone;

	float animationAmt;
	float speed;

public:
	TransformerBone *currentBone;
	CString lastChild;
	int numTargetBoneFound;
	int numTargetBoneToAnimate;
	int currentBoneIdx;
	bool posAnimated[NUM_ANIMATION_STEPS];
	AnimationStep animStep;

	// Animation methods to set animation status
	void stopAnimation();
	void playAnimation();
	void restartAnimation();

	// Animate unfolding of transformer
	void animateTransformer();	// Animation control
	void unfoldTransformer(TransformerBone *target, TransformerBone *node, float amt);
};

