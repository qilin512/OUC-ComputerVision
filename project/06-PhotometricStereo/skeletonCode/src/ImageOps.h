#ifndef _IMAGE_OPS_H_
#define _IMAGE_OPS_H_

#include <string>

#include <sys/stat.h>

#include "ImageTypes.h"

void         encodeNormalsAsColors(const NormalImage *ni, ColorImage *ci);
void         decodeNormalsFromColors(const ColorImage *ci, NormalImage *ni);

void         encodeDepthsAsColors(const DepthImage *fi, ColorImage *ci);

ColorImage  *getFloatImage(const char *filename);
void         saveFloatImage(const ColorImage *cimg, const char *filename);

inline bool
fileExists(const char *filename)
{
    struct stat buf;
    if (stat(filename,&buf) == -1)
	return false;
    return true;
}

#endif // _IMAGE_OPS_H_
