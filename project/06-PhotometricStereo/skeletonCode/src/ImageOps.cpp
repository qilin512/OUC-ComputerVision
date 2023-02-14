#include <string>
#include <fstream>
#include <assert.h>

#include "Vec.h"

#include "ImageLib/Image.h"
#include "ImageLib/FileIO.h"

#include "ImageTypes.h"
#include "ImageOps.h"

using namespace std;

const Vec3f zero(0,0,0);

inline bool
nearZero(const Vec3f &N)
{
    return ((N.x<.01) && (N.x>-.01) &&
	    (N.y<.01) && (N.y>-.01) &&
	    (N.z<.01) && (N.z>-.01));
}

void
encodeNormalsAsColors(const NormalImage *ni, ColorImage *ci)
{
    assert (ni->Shape().SameIgnoringNBands(ci->Shape()));
    assert (ci->Shape().nBands == 3);

    int height = ci->Shape().height;
    int width = ci->Shape().width;

    for (int y = 0; y<height; y++)
	for (int x = 0; x<width; x++)
	{
	    const Vec3f &v = ni->Pixel(x,y,0);
	    ci->Pixel(x,y,2) = (v.x + 1)*.5f;
	    ci->Pixel(x,y,1) = (v.y + 1)*.5f;
	    ci->Pixel(x,y,0) = (v.z + 1)*.5f;
	}
}

void
decodeNormalsFromColors(const ColorImage *ci, NormalImage *ni)
{	
    assert (ni->Shape().SameIgnoringNBands(ci->Shape()));
    assert (ci->Shape().nBands == 3);

    int height = ci->Shape().height;
    int width = ci->Shape().width;

    for (int y = 0; y<height; y++)
	for (int x = 0; x<width; x++)
	{
	    Vec3f &v = ni->Pixel(x,y,0);
	    v.x = ci->Pixel(x,y,0)*2 - 1;
	    v.y = ci->Pixel(x,y,1)*2 - 1;
	    v.z = ci->Pixel(x,y,2)*2 - 1;
//	    if (!nearZero(v))
//		v.normalize();
	}
}

void
encodeDepthsAsColors(const DepthImage *fi, ColorImage *ci)
{
    assert (fi->Shape().SameIgnoringNBands(ci->Shape()));

    int width = ci->Shape().width;
    int height = ci->Shape().height;
    int nBands = ci->Shape().nBands;

    // just normalize to 0-1
    float maxZ = 0;
    for (int y = 0; y<height; y++)
	for (int x = 0; x<width; x++)
	    for (int c = 0; c<nBands; c++)
		maxZ = max(fi->Pixel(x,y,0),maxZ);
    for (int y = 0; y<height; y++)
	for (int x = 0; x<width; x++)
	{
	    float p = fi->Pixel(x,y,0)/maxZ;
	    for (int c = 0; c<nBands; c++)
		ci->Pixel(x,y,c) = p;
	}

}

ColorImage *getFloatImage(const char *filename)
{
    CByteImage img;
    ReadFile(img,filename);

    int width = img.Shape().width;
    int height = img.Shape().height;
    int nBands = img.Shape().nBands;

    ColorImage *cimg = new ColorImage(width,height,nBands);
    for (int y = 0; y<height; y++)
	for (int x = 0; x<width; x++)
	    for (int c = 0; c<nBands; c++)
		cimg->Pixel(x,y,c) = img.Pixel(x,y,c)/255.0f;

    return cimg;
}

void
saveFloatImage(const ColorImage *cimg, const char *filename)
{
    int width = cimg->Shape().width;
    int height = cimg->Shape().height;
    int nBands = cimg->Shape().nBands;

    CByteImage img(width,height,nBands);
    for (int y = 0; y<height; y++)
	for (int x = 0; x<width; x++)
	    for (int c = 0; c<nBands; c++)
		img.Pixel(x,y,c) = min(255,
				       max(0,
					   (int)(cimg->Pixel(x,y,c)*255.0)));

    WriteFile(img,filename);
}

