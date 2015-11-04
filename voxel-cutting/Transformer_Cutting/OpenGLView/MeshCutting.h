#pragma once
#include "stdafx.h"
#include "Graphics\Surfaceobj.h"
#include <carve/carve.hpp>
#include <carve/poly.hpp>
#include <carve/polyline.hpp>
#include <carve/pointset.hpp>
#include "MeshPiece.h"

typedef carve::poly::Polyhedron Polyhedron;
typedef carve::geom3d::Vector cVector;
typedef carve::poly::Vertex<3> cVertex;
typedef carve::poly::Face<3> cFace;

extern class voxelObject;
extern class bvhVoxel;
extern class bone;
//extern class MeshPiece;

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

class MeshCutting
{
public:
	MeshCutting(void);
	~MeshCutting(void);

	void init();
	void init2(std::vector<arrayInt> meshIdx, std::vector<bone*> boneArray_i);

	void cutTheMesh();
	void CopyMeshToBone();
	void updateScale(float scaleR);

	Polyhedron* boxCut(Vec3f leftDown, Vec3f rightUp);
	
	void drawPolygon(Polyhedron * p);
	void drawPolygonFace(Polyhedron *p);
	void drawPolygonEdge(Polyhedron * p);
	void draw(BOOL displayMode[10]);

	carve::poly::Polyhedron * makeCube(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
	void triangleBox(Vec3f leftDown, Vec3f rightUp, std::vector<Vec3f> &points_, std::vector<Vec3i> &faces_);
	carve::poly::Polyhedron * makeCubeTriangular(double minX, double minY, double minZ, double maxX, double maxY, double maxZ);
	SurfaceObj* triangulatePolygon(Polyhedron * testResult);
	

	Polyhedron *m_polyHedron;
	
	std::vector<Polyhedron*> m_cutPieces;
	std::vector<Polyhedron*> m_cutSurface;

	SurfaceObj * s_surObj;
	voxelObject * s_voxelObj;

	arrayVec3i coords;
	std::vector<bvhVoxel*> * s_meshBox;
	std::vector<bone*> boneArray;
	std::vector<arrayInt> meshVoxelIdxs;

	static arrayVec3f color;

public:
	Polyhedron * convertTriangularToPolygonMesh(arrayVec3f * pts, arrayVec3i * faces);
	Vec3f getCenterBox(arrayInt voxelIdxs);

	arrayVec3f getAllMeshOrigin();
	arrayVec3f getAllCenterOfMesh();
	void transformMesh();

private:
	void setRotationCase(Vec3f localAxis);
	Vec3f getMeshOrigin(int index, Vec3f localAxis);
	
};

