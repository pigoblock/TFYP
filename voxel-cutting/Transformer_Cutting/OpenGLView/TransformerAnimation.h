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

	int currentBoneIdx;
	AnimationStep boneAnimationStage;

	bool showSkeleton;
	bool showMesh;

public:
	TransformerBone *currentBone;
	bool posAnimated[NUM_ANIMATION_STEPS];

	// Animation methods to set animation status
	void stopAnimation();
	void playAnimation();
	void restartAnimation();

	// Animation control
	void animateTransformer(bool displayMode[3]);	

private:
	void unfoldTransformer(TransformerBone *target, TransformerBone *node, float amt);
	void drawBasedOnDisplayMode(TransformerBone *node, float boneLength);
	void setDisplayMode(bool mode[2]);
};

