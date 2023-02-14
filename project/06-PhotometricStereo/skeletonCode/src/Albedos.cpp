#include "Albedos.h"
#include <assert.h>

using namespace std;

///////////////////////////////////////////////////////////////////////////
// computeOneAlbedo: given the pixel values, light directions, and normal,
//   compute the albedo of the surface, assuming Lambertian reflection.
//
float
computeOneAlbedo(vector<float> &values, vector<Vec3f> &lightDirs,
		 Vec3f &normal)
{
    int count = values.size();
    assert (count == lightDirs.size());
	float albedo = 0;

	////////////////////////////////////////////////
	// TODO compute albedo as ratio of I . J / J . J
	//    where J(i) = L(i).N
	////////////////////////////////////////////////

	/***** BEGIN TODO *****/


	/***** END TODO *****/

    return albedo;
}
