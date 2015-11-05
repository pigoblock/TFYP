#pragma once
#include "stdafx.h"
#include "skeleton.h"
#include <iostream>

extern class ConnectingBone;

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
	Polyhedron *mesh;

	// Joint coordinates of the bone (absolute)
	Vec3f m_startJoint;
	Vec3f m_endJoint;

	// How bone needs to orientate from mesh to skeleton
	Vec3i m_orientation;	// Global variable

	// Coordinate information relative to parent
	Vec4f m_unfoldAngle;	
	Vec3f m_unfoldCoord; // Original coordinate relative to parent

	Quat m_foldQuat; // Relative coodinates wrt parent (angle,x,y,z) in quaternion
	Vec4f m_foldAngle;	// Relative coodinates wrt parent (angle,x,y,z) in axis angle
	Vec3f m_foldCoord; // Relative coodinates wrt parent

	// Tree hierarchy
	TransformerBone* m_parent;
	std::vector<TransformerBone*> m_children;

	ConnectingBone* m_connectingParent;
	std::vector<ConnectingBone*> m_connectingChildren;

	void setGlobalBonePosition(Vec3f distMeshToOrigin);
	void setRelativeBonePosition(Vec3f parentOrientation);

	static void drawSphereJoint(float radius);
	static void drawCylinderBone(float length, float width);
	void drawMesh();
};

class ConnectingBone{
public:
	ConnectingBone(TransformerBone* Tparent, TransformerBone* Tchild);
	~ConnectingBone();

	// Bone information
	float m_length;
	int m_index;

	// Joint coordinates of the bone (absolute)
	Vec3f m_parentJoint;
	Vec3f m_childJoint;

	// Coordinate information relative to parent
	Vec4f m_unfoldAngle;
	Vec3f m_unfoldCoord; // Original coordinate relative to parent

	Quat m_foldQuat; // Relative coodinates wrt parent (angle,x,y,z) in quaternion (v+s)
	Vec4f m_foldAngle;	// Relative coodinates wrt parent (angle,x,y,z) in axis angle
	Vec3f m_foldCoord; // Relative coodinates wrt parent

	// Bones its connected to
	TransformerBone* m_Tparent;
	TransformerBone* m_Tchild;
};

class TransformerSkeleton
{
public:
	TransformerSkeleton();
	~TransformerSkeleton();

	TransformerBone *transformerRootBone;
	std::vector<ConnectingBone*> connectingBones;

	void initialize(bone *originalSkeletonRoot, MeshCutting *originalMeshCutting);
	
	void drawSkeleton();

private:
	// Variables that help to form the new transformer skeleton
	bone *originalSkeletonRootBone;
	MeshCutting *originalMesh;

	// Functions that form the new transformer skeleton
	void createClosedTransformer(TransformerBone *transformerRootBone, bone *originalSkeletonRootBone);
	void mapFromOldBones(TransformerBone *newNode, bone *originalNode);
	void setupRelativeBoneStructure(TransformerBone *node, Quat cumulativeParentQuat);
	void setupConnectingBones();

	void drawFoldedSkeletonRecur(TransformerBone *node);
	void drawWholeFoldedSkeletonRecur(TransformerBone *node);
	void retrieveEulerRotation(Vec3f localAxis);
	Quat retrieveQuatRotation(Vec3f localAxis);
	Vec3f getRelativeOrientation(Vec3f originalAbsOrientation, Vec3f newBaseOrientation);
	Vec3f getQPQConjugate(Quat quat, Vec3f originalPoint);
};