#ifndef _IMAGE_TYPES_H_
#define _IMAGE_TYPES_H_

#include <vector>
#include "Vec.h"
#include "ImageLib/Image.h"

typedef CFloatImage ColorImage;
typedef CImageOf<Vec3f> NormalImage;
typedef CFloatImage DepthImage;

#endif // _IMAGE_TYPES_H_

