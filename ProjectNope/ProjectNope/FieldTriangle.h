#ifndef FIELD_TRIANGLE_H
#define FIELD_TRIANGLE_H

#include "Vec3.h"
#include <vector>
#include <tuple>

class FieldTriangle;

typedef std::vector<FieldTriangle> FieldVector;
typedef std::pair<FieldVector, FieldVector> SplitPair;
typedef std::tuple<FieldVector, FieldVector, FieldVector> MergeTuple;

class FieldTriangle
{
public:
	FieldTriangle(void);
	FieldTriangle(const Vec3& a, const Vec3& b, const Vec3& c);
	~FieldTriangle(void);
	
	// n pointing away from the clock face
	FieldTriangle& order_cw(Vec3 n);
	FieldTriangle& order_ccw(Vec3 n);

	SplitPair split(Vec3 n, float l);
	MergeTuple merge(const FieldTriangle& other);

	Vec3 p1, p2, p3;
};

#endif