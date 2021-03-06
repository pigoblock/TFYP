#include "stdafx.h"
#include "tranformCoord.h"
#include "DataTypes\define.h"


tranformCoord::tranformCoord()
{
}


tranformCoord::~tranformCoord()
{
}

// Literally changes vertex positions, not just in appearance
Vec3f tranformCoord::tranfrom(Vec3f pt)
{
	arrayVec3f xyz = { Vec3f(1.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0), Vec3f(0.0, 0.0, 1.0) };

	// m_tranf center box
	ptTrans = pt - m_tranf;
	Vec3f ptOut;
	for (int i = 0; i < 3; i++)
	{
		ptOut = ptOut + xyz[m_coord[i]]*ptTrans[i];
	}

	return ptOut;
}
