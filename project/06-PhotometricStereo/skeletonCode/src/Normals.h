#ifndef _NORMALS_H_
#define _NORMALS_H_

#include <vector>

#include "Vec.h"

Vec3f computeOneNormal(std::vector<float> &values,
		       std::vector<Vec3f> lightDirs);

#endif // _NORMALS_H_
