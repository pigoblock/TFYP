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
	Quat m_unfoldQuat;
	Vec4f m_unfoldAngle;	
	Vec3f m_unfoldCoord; // Original coordinate relative to parent

	Quat m_foldQuat; // Relative coodinates wrt parent (angle,x,y,z) in quaternion
	Vec4f m_foldAngle;	// Relative coodinates wrt parent (angle,x,y,z) in axis angle
	Vec3f m_foldCoord; // Relative coodinates wrt parent (start joint)

	// Tree hierarchy
	TransformerBone* m_parent;
	std::vector<TransformerBone*> m_children;

	ConnectingBone* m_connectingParent;
	std::vector<ConnectingBone*> m_connectingChildren;

	void setGlobalBonePosition(Vec3f distMeshToOrigin);

	static void drawSphereJoint(float radius);
	static void drawCylinderBone(float length, float width);
	void drawMesh();
};

class ConnectingBone{
public:
	ConnectingBone(TransformerBone* Tparent, TransformerBone* Tchild);
	~ConnectingBone();

	// Bone information
	float m_foldedLength;
	float m_unfoldedLength;
	int m_index;

	// Joint coordinates of the bone (absolute)
	Vec3f m_parentJoint;
	Vec3f m_childJoint;

	// Coordinate information relative to parent
	Quat m_unfoldQuat;
	Vec4f m_unfoldAngle;
	Vec3f m_unfoldCoord; // Original coordinate relative to parent

	Quat m_foldQuat; // Relative coodinates wrt parent (angle,x,y,z) in quaternion (v+s)
	Vec4f m_foldAngle;	// Relative coodinates wrt parent (angle,x,y,z) in axis angle
	Vec3f m_foldCoord; // Relative coodinates wrt parent

	// Bones its connected to
	TransformerBone* m_Tparent;
	TransformerBone* m_Tchild;

	void setRotations();
	float getMagnitude(Vec3f vector);
};

class TransformerSkeleton
{
public:
	TransformerSkeleton();
	~TransformerSkeleton();

	TransformerBone *transformerRootBone;
	std::vector<TransformerBone*> tBoneArray;
	std::vector<ConnectingBone*> connectingBones;

	void initialize(bone *originalSkeletonRoot, MeshCutting *originalMeshCutting);
	
	void drawSkeleton(int mode, bool display[2]);

private:
	// Variables that help to form the new transformer skeleton
	bone *originalSkeletonRootBone;
	MeshCutting *originalMesh;

	// Functions that form the new transformer skeleton
	void createClosedTransformer(TransformerBone *transformerRootBone, bone *originalSkeletonRootBone);
	void mapFromOldBones(TransformerBone *newNode, bone *originalNode);
	void setupRelativeBoneStructure(TransformerBone *node, Quat cumulativeParentQuat);
	void setupUnopenedRotations(bone *node, Quat origCumulParent,
		Vec3f origCumulPosition, Vec3f cumulPosition, Quat unfoldCumulParent);
	void setupConnectingBones();	// do we need connecting bones?
	void tranformVerticesInMeshes();

	void drawFoldedSkeletonRecur(TransformerBone *node);
	void drawUnfoldedSkeletonRecur(TransformerBone *node, int mode);
public:
	Quat retrieveQuatRotation(Vec3f localAxis);
	Vec3f getQPQConjugate(Quat quat, Vec3f originalPoint);

	bool showMesh;
	bool showSkeleton;
	void drawBasedOnDisplayMode(TransformerBone *node, float boneLength);
};