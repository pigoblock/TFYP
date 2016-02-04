#pragma once
#include "stdafx.h"
#include "DataTypes\vec.h"
#include "MeshCutting.h"
#include "myXML.h"

#define MAX_CHILD 10
#define	TYPE_CENTER_BONE 0
#define TYPE_SIDE_BONE 1

enum
{
	SKE_DRAW_LINE = 1,
	SKE_DRAW_BOX_WIRE = 2,
	SKE_DRAW_BOX_SOLID = 3,
};

enum neighhorType
{
	NEIGHBOR_SERIAL,
	NEIGHBOR_SERIAL_MIRROR,
	NEIGHBOR_SIDE
};

class bone 
{
public:
	bone();
	~bone();

	// Bone information
	Vec3f m_sizef;
	CString m_name;
	std::string m_nameString;
	int m_type;
	int m_index;

	// Coordinate information relative to parent
	Vec3f m_angle;	// Rotation angle by x-y-z. global, degree
	Vec3f m_jointBegin;	// The beginning of the joint
	Vec3f m_posCoord; // Original coordinate relative to parent
						// This is also end of the joint 

	// Mesh information
	Polyhedron *mesh;
	Vec3f meshSizeScale;

	// Tree hierarchy
	bone* parent;
	neighhorType neighborType;
	std::vector<bone*> child;

	// Temporary variables for cutting algorithm
	float m_volumef;
	Vec3f leftDownf, rightUpf;
	float m_volumeRatio; // Original ratio

	void draw(int mode, float scale = 1.0, bool mirror = false);
	void drawCoord();
	void drawBoneWithMeshSize();
	void initOther();

	BOOL isLarger(bone* a);
	void getMeshFromOriginBox(Vec3f LeftDown, Vec3f rightUp);

	bool isLeaf();
	float getVolumef();
	float& volumeRatio();

	char* getTypeString();

	// For group bone algorithm
	bool bIsGroup;
	float m_groupVolumef;
	float m_groupVolumeRatio; // total volume group
	float m_volumeRatioInGroup; // ratio in group
	
	// for neighbor check; temp var
	int indexOfMeshBox;
	int nbCenterNeighbor();
	int nbSideNeighbor();
	int nbNeighbor() const;
	void setBoneType(std::string typeString);
	float groupShrink();
};

struct compare{
	bool operator()(bone const* a, bone const* b){
		return (a->m_index < b->m_index);
	}
};

typedef std::vector<bone*> arrayBone_p;
typedef std::vector<bone> arrayBone;

class skeleton
{
public:
	skeleton(void);
	~skeleton(void);

	bone* m_root;
	float meshScale;

	void loadFromFile(char *filePath);
	void writeToFile(char* filePath);

	void draw(int mode=SKE_DRAW_BOX_WIRE); // SKE_DRAW_...
	void drawGroup(int mode = SKE_DRAW_BOX_WIRE);
	void drawBoneWithMeshSize();
	
	void computeTempVar();

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
	void assignBoneIndex();

private:
	// Assign index to bones
	int index;
	void assignBoneIndexRecur(bone *node);
	
	// For group bone algorithm
	float volumeOfGroupBone(bone* node);
	float volumeRatioOfGroupBone(bone* node);

	// For cutting algorithm
	void getBoneGroupAndNeighborInfoRecur(bone* node, int parentIdx, std::vector<bone*> & boneArray, std::vector<std::pair<int, int>> & neighborA);
	void getSortedBoneGroupArrayRecur(bone* node, std::vector<bone*> & sortedArray);
	void getSortedBoneArrayRecur(bone* node, std::vector<bone*> &sortedArray);
	void getBoneAndNeighborInfoRecur(bone* node, int parentIdx, std::vector<bone*> &boneArray, std::vector<std::pair<int, int>> &neighborA);

	// Loading of data
	void writeBoneToXML(myXML * doc, myXMLNode * node, bone* boneNode);
	void loadBoneData(myXML * doc, myXMLNode * xmlNode, bone* boneNode);

	// Drawing functions
	void drawBoneWithMeshSizeRecur(bone* mode);
	void drawBoneRecursive(bone* node, int mode, bool mirror = false);
	void drawGroupRecur(bone* node, int mode, bool mirror = false);
};

typedef std::shared_ptr<skeleton> skeletonPtr;
