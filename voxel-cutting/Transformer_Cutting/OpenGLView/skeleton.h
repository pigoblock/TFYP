#pragma once
#include "stdafx.h"
#include "DataTypes\vec.h"
#include "MeshCutting.h"
#include "myXML.h"

#define MAX_CHILD 10

enum
{
	SKE_DRAW_LINE = 1,
	SKE_DRAW_BOX_WIRE = 2,
	SKE_DRAW_BOX_SOLID = 4,
	SKE_DRAW_MESH = 5,
	SKE_DRAW_JOINTS = 6
};

enum neighhorType
{
	NEIGHBOR_SERIAL,
	NEIGHBOR_SERIAL_MIRROR,
	NEIGHBOR_SIDE
};

#define	TYPE_CENTER_BONE 0
#define TYPE_SIDE_BONE 1

class bone
{
public:
	bone();
	~bone();

	void draw(int mode, float scale = 1.0, bool mirror = false);
	void drawCoord();
	void drawMesh(float scale = 1.0);
	void drawBoneWithMeshSize();
	void drawSphereJoint(float radius);
	void drawCylinderBone(float length, float width);
	void initOther();

	BOOL isLarger(bone* a);
	void getMeshFromOriginBox(Vec3f LeftDown, Vec3f rightUp);

	bool isLeaf();
	float getVolumef();
	float& volumeRatio();

	char* getTypeString();

	// Bone information
	Vec3f m_sizef;
	CString m_name;
	std::string m_nameString;
	int m_type;
	int color;

	// Tree hierarchy
	bone* parent;
	neighhorType neighborType;
	std::vector<bone*> child;

	// Coordinate information relative to parent
	Vec3f m_angle;	// Rotation angle by x-y-z. global, degree
	Vec3f m_jointBegin;	// The beginning of the joint
	Vec3f m_posCoord; // Original coordinate relative to parent
						// This is also end of the joint 

	// Temporary var
	float m_volumef;
	Vec3f leftDownf, rightUpf;
	float m_volumeRatio; // Original ratio

	// For group bone algorithm
	bool bIsGroup;
	float m_groupVolumef;
	float m_groupVolumeRatio; // total volume group
	float m_volumeRatioInGroup; // ratio in group

	// mesh information
	Polyhedron *mesh;
	Vec3i meshRot; // z-x-z 90 rotation, Euler
	Mat4x4f transformMat;
	Vec3f meshSizeScale;
	
	// for neighbor check; temp var
	int indexOfMeshBox;
	int nbCenterNeighbor();
	int nbSideNeighbor();
	int nbNeighbor() const;
	void setBoneType(std::string typeString);
	Mat4x4f getLocalTransMat();
	float groupShrink();

	// Determines how mesh should rotate in order to fit box
	Vec3f transformCoords;	
};

struct compare{
	bool operator()(bone const* a, bone const* b){
		return (a->color < b->color);
	}
};

typedef std::vector<bone*> arrayBone_p;
typedef std::vector<bone> arrayBone;

class skeleton
{
public:
	skeleton(void);
	~skeleton(void);

	void loadFromFile(char *filePath);
	void writeToFile(char* filePath);

	void draw(int mode=SKE_DRAW_BOX_WIRE); // SKE_DRAW_...
	void drawGroup(int mode = SKE_DRAW_BOX_WIRE);
	void drawBoneWithMeshSize();
	void drawBoneWithCutPieces();
	void drawBonesAndJoints();
	
	void initTest(); // Manually for testing
	void computeTempVar();

	void buildTransformMatrix();

	// Group bones algorithm
	void groupBones();
	void getBoneGroupAndNeighborInfo(std::vector<bone*> &sorted, std::vector<std::pair<int, int>> &neighborPair);
	void getSortedGroupBoneArray(std::vector<bone*> &sortedArray);

	void getBoneAndNeighborInfo(std::vector<bone*> &boneArray, std::vector<std::pair<int,int>> &neighborA);

	void getSortedBoneArray(std::vector<bone*> &sortedArray);

	void getGroupBone(bone* node, std::vector<bone*> &groupBone); // Get root of group bone
	void getBoneInGroup(bone* node, std::vector<bone*> &boneInGroup); // Get all bone in this group
	void getNeighborPair(bone* node, std::vector<Vec2i> &neighbor, std::vector<bone*> boneArray);

	float getVolume();
	void assignBoneColor();

private:
	void drawBoneWithCutPiecesRecur(bone *node, int colorIndex);
	void drawBoneRecursive(bone* node, int mode, bool mirror = false);
	void drawGroupRecur(bone* node, int mode, bool mirror = false);
	void drawBonesAndJointsRecur(bone *node);

	void getSortedBoneArrayRecur(bone* node, std::vector<bone*> &sortedArray);
	void getBoneAndNeighborInfoRecur(bone* node, int parentIdx, std::vector<bone*> &boneArray, std::vector<std::pair<int,int>> &neighborA);
	void buildTransformMatrixRecur(bone* node);

	// For group bone algorithm
	float volumeOfGroupBone(bone* node);
	float volumeRatioOfGroupBone(bone* node);

	void getBoneGroupAndNeighborInfoRecur(bone* node, int parentIdx, std::vector<bone*> & boneArray, std::vector<std::pair<int, int>> & neighborA);
	void getSortedBoneGroupArrayRecur(bone* node, std::vector<bone*> & sortedArray);
	void writeBoneToXML(myXML * doc, myXMLNode * node, bone* boneNode);
	void loadBoneData(myXML * doc, myXMLNode * xmlNode, bone* boneNode);
	void drawBoneWithMeshSizeRecur(bone* mode);

	int colorIndex;
	void assignBoneColorRecur(bone *node);

public:
	bone* m_root;
	float meshScale;

	bool sideBoneDrawFlag;	
	void updateSkeletonJointPos();
};

typedef std::shared_ptr<skeleton> skeletonPtr;
