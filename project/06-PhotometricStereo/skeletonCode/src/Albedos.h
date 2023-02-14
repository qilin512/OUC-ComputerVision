#ifndef _ALBEDOS_H_
#define _ALBEDOS_H_

#include <vector>

#include "Vec.h"

float
computeOneAlbedo(std::vector<float> &values,
		 std::vector<Vec3f> &lightDirs,
		 Vec3f &normal);

#endif // _ALBEDOS_H_
