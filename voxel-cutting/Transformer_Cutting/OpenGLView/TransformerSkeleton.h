#pragma once
#include "stdafx.h"
#include "skeleton.h"
#include <iostream>

class TransformerBone {
public:
	TransformerBone();
	~TransformerBone();

	// Bone information
	int m_index;
	int m_type;
	Vec3f m_sizef;
	CString m_name;
	float m_length;

	// Joint coordinates of the bone
	Vec3f m_startJoint;
	Vec3f m_endJoint;

	// How bone needs to orientate from mesh to skeleton
	Vec3f m_orientation;

	// Coordinate information relative to parent
	Vec3f m_unfoldAngle;	// Rotation angle by x-y-z. global, degree
	Vec3f m_unfoldCoord; // Original coordinate relative to parent
	Vec3f m_foldAngle;	
	Vec3f m_foldCoord; 

	// Tree hierarchy
	TransformerBone* m_parent;
	std::vector<TransformerBone*> m_children;

	void setBonePosition(Vec3f distMeshToOrigin);

	void drawSphereJoint(float radius);
	void drawCylinderBone(float length, float width);
};

class TransformerSkeleton
{
public:
	TransformerSkeleton();
	~TransformerSkeleton();

	TransformerBone *transformerRootBone;

	void initialize(bone *originalSkeletonRoot, MeshCutting *originalMeshCutting);
	
	void drawSkeleton();

private:
	// Variables that help to form the new transformer skeleton
	bone *originalSkeletonRootBone;
	MeshCutting *originalMesh;

	// Functions that form the new transformer skeleton
	void createClosedTransformer(TransformerBone *transformerRootBone, bone *originalSkeletonRootBone);
	void createClosedTransformerRecur(TransformerBone *newNode, bone *originalNode);

	void drawFoldedSkeletonRecur(TransformerBone *node);
	void retrieveRotation(Vec3f localAxis);
};