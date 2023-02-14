#include "LightDirs.h"


namespace {

///////////////////////////////////////////////////////////////////////////
// findCenterAndRadius: assume mask has the shape of a sphere
//                      return center pixel and radius (in pixels)
//
void
findCenterAndRadius(const ColorImage *mask, float &centerX, float &centerY,
		    float &radius)
{
    long sumX = 0, sumY = 0;
    int numPixels = 0;

    int width = mask->Shape().width;
    int height = mask->Shape().height;

    /* First find weighted average of nonzero pixel centers */

    for (int y = 0; y<height; y++)
	for (int x = 0; x<width; x++)
	    if (mask->Pixel(x,y,0))
	    {
		sumX += x;
		sumY += y;
		numPixels ++;
	    }

    centerX = float(sumX)/numPixels;
    centerY = float(sumY)/numPixels;

    /* Then find maximum distance to center */

    radius = 0;
    for (int y = 0; y<height; y++)
	for (int x = 0; x<width; x++)
	    if (mask->Pixel(x,y,0))
	    {
		float diffX = centerX - x;
		float diffY = centerY - y;
		float dist = diffX*diffX + diffY*diffY;

		if (dist > radius)
		    radius = dist;
	    }
    radius = sqrt(radius);

    // cout << "center = " << centerX << " " << centerY << endl;
    // cout << "radius = " << radius << endl;
}

////////////////////////////////////////////////////////////
// normalOnSphere: given a pixel position, and a center and
//   radius of a sphere, compute the normal at that point
//   on the sphere
inline Vec3f
normalOnSphere(float x, float y,
	       float centerX, float centerY, float radius)
{
    Vec3f n;
	n.x = n.y = n.z = 0;

	/////////////////////////////////////////
	// TODO: compute the normal at this pixel
	//
	/***** BEGIN TODO *****/

	/***** END TODO *****/

    return n;
}
    
} // end unnamed namespace

//////////////////////////////////////////////////
// computeOneLightDir: given an image of a chrome sphere
//   under a point light source, and an associated mask,
//   compute the direction of the point light source.
//   Assume the viewer is at (0,0,1).
//
Vec3f
computeOneLightDir(const ColorImage *img, const ColorImage *mask)
{

    float centerX, centerY, radius;

    findCenterAndRadius(mask,centerX,centerY,radius);

    double comX = 0;
    double comY = 0;

	///////////////////////////////////////
	// TODO: compute a weighted average of the brightest
	//    pixel locations.  (You might want to ignore
	//    pixel values less than 0.9.)
	//
	/***** BEGIN TODO *****/

	/***** END TODO *****/

    Vec3f n = normalOnSphere(float(comX),float(comY),centerX,centerY,radius);

    return reflect(Vec3f(0,0,1),n);
}

