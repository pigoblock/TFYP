#pragma once
#include "stdafx.h"
#include "skeleton.h"
#include "MeshCutting.h"
#include "TransformerSkeleton.h"

enum AnimationStep{
	CONNECTING_BONE_ROTATION,
	CONNECTING_BONE_LENGTH,
	BONE_SHELL_ROTATION,
	DONE,
	NUM_ANIMATION_STEPS
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
	TransformerSkeleton *transformer;

private:
	bool startAnimation;
	bool pauseAnimation;
	bool animationDone;

	float animationAmt;
	float speed;

	AnimationStep boneAnimationStage;

public:
	TransformerBone *currentBone;
	CString lastChild;
	int numTargetBoneFound;
	int numTargetBoneToAnimate;
	int currentBoneIdx;
	bool posAnimated[NUM_ANIMATION_STEPS];

	// Animation methods to set animation status
	void stopAnimation();
	void playAnimation();
	void restartAnimation();

	// Animate unfolding of transformer
	void animateTransformer();	// Animation control
	void unfoldTransformer(TransformerBone *target, TransformerBone *node, float amt);
};

