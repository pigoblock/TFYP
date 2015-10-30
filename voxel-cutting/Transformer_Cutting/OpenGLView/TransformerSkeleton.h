#pragma once
#include "stdafx.h"
#include "skeleton.h"
#include <iostream>

class TransformerSkeleton
{
public:
	TransformerSkeleton();
	~TransformerSkeleton();

	bone *transformerRootBone;

	void initialize(bone *originalSkeletonRoot, MeshCutting *originalMeshCutting);
	
	void drawSkeleton();

private:
	// Variables that help to form the new transformer skeleton
	bone *originalSkeletonRootBone;
	MeshCutting *originalMesh;

	void createClosedTransformer(bone *transformerRootBone, bone *originalSkeletonRootBone);
	void createClosedTransformerRecur(bone *newNode, bone *originalNode);

	void drawSkeletonRecur(bone *node);
};

