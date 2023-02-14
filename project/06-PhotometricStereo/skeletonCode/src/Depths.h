#ifndef _DEPTHS_H_
#define _DEPTHS_H_

#include "ImageTypes.h"

void
computeDepths(DepthImage *depths, const NormalImage *normals,
	      const ColorImage *mask);

#endif // _DEPTHS_H_
