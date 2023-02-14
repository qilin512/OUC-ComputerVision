#ifndef _LIGHT_DIRS_H_
#define _LIGHT_DIRS_H_

#include "Vec.h"
#include "ImageTypes.h"

Vec3f computeOneLightDir(const ColorImage *i, const ColorImage *mask);

#endif // _LIGHT_DIRS_H_
