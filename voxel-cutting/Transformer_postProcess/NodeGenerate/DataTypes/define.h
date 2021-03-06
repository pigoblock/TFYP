#ifndef DEFINE_H
#define DEFINE_H

#include "DataTypes/vec.h"
#include <vector>

#define PI 3.141592
#define EPS 0.000001
#define ZERO_F 0.000001
//#define GRAVITY 98
#define GRAVITY 980
#define MAX 100000000
#define MIN -100000000
#define MARGIN 5
#define GAUSS_QUADRATURE 0
#define NODAL_INTEGRATION 1
#define STRESS_POINT 2

#define SPLINE3 0		//3rd order spline
#define SPLINE4 1		//4th order spline
#define EXPONENTIAL 2

#define E_m		10000
#define NU		0.3
#define DENSITY	0.01 //0.02 for majorpapilla; 0.001 for liver; 0.1 for papilla
#define DAMPING	1000

#define TOOL_RADIUS 0.5

class Box {
public:
	Box(){};
	~Box(){};
	Box(Vec3f& ld, Vec3f& ru) {
		leftDown = ld; 
		rightUp = ru; 
		center = (leftDown+rightUp)/2.;
	}
	Vec3f leftDown;
	Vec3f rightUp;
	Vec3f center;
} ;

typedef struct
{
	Vec3i leftDown;
	Vec3i rightUp;
} Boxi;

typedef struct
{
	Vec3f p[4];
	Vec3f normal;
	Box box;
} Rect;

typedef struct
{
	Vec3f p[3];
	Vec3f normal;
	Box box;
} Tri;

typedef struct
{
	Vec3f l1;
	Vec3f l2;
	Vec3f center;
} Line;

typedef enum{
	DRAW_WIRE				= 0x0001,
	DRAW_SURFACE				= 0x0010,
	DRAW_WIRE_AND_SURFACE	= 0x0011
}drawMode;



typedef std::vector<Vec3f> arrayVec3f;
typedef std::vector<Vec3d> arrayVec3d;
typedef std::vector<Vec3i> arrayVec3i;
typedef std::vector<int> arrayInt;
typedef std::vector<float> arrayFloat;
typedef std::vector<Vec2f> arrayVec2f;
typedef std::vector<Vec2i> arrayVec2i;

#endif