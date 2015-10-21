#pragma once

/*
struct coordInfo
{
	Vec3f origin;
	Vec3f xyzOrient[3];

	Vec3f tranfrom(Vec3f pt){
		Vec3f ptTrans = pt - origin;
		Vec3f ptOut;
		for (int i = 0; i < 3; i++)
		{
			ptOut[i] = xyzOrient[i] * ptTrans;
		}

		return ptOut;
	}
};*/

class MeshPiece
{
public:
	MeshPiece();
	~MeshPiece();
	/*
	void initOther();
	//Vec3f centerPoint(){ return (leftDown + rightUp) / 2; }

public:
	Polyhedron *m_mesh;
	bone *matchingBone;
	Vec3i coord;

	//Vec3f leftDown;
	//Vec3f rightUp;

	coordInfo coordMap;*/
};

typedef std::shared_ptr<MeshPiece> MeshPiecePtr;