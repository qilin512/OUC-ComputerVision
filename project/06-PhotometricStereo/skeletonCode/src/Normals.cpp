#include "Normals.h"

#include "CVector.h"
#include "CMatrix.h"

using namespace std;

namespace {

CMatrix<double>
invert3x3(CMatrix<double> &m)
{
 	CMatrix<double> s(3,3);
		s(0,0) = m(1,1) * m(2,2) - m(2,1) * m(1,2);
		s(0,1) = m(2,1) * m(0,2) - m(0,1) * m(2,2);
		s(0,2) = m(0,1) * m(1,2) - m(1,1) * m(0,2);

		s(1,0) = m(2,0) * m(1,2) - m(1,0) * m(2,2);
		s(1,1) = m(0,0) * m(2,2) - m(2,0) * m(0,2);
		s(1,2) = m(1,0) * m(0,2) - m(0,0) * m(1,2);

		s(2,0) = m(1,0) * m(2,1) - m(2,0) * m(1,1);
		s(2,1) = m(2,0) * m(0,1) - m(0,0) * m(2,1);
		s(2,2) = m(0,0) * m(1,1) - m(1,0) * m(0,1);

	double r = m(0,0) * s(0,0) + m(0,1) * s(1,0) + m(0,2) * s(2,0);

	    for (int i = 0; i < 3; ++i)
	    {
		for (int j = 0; j < 3; ++j)
		{
		    s(i,j) /= r;
		}
	    }

	return s;
}

inline CVector<double>
solve3x3(CMatrix<double> &m ,CVector<double> &v)
{
	assert(m.rows() == 3);
	assert(m.columns() == 3);
	assert(v.Length() == 3);

	return invert3x3(m)*v;
}

}
///////////////////////////////////////////////////////////////////////////
// computeOneNormal: given the pixel values and light directions
//   compute the normal to the surface, assuming Lambertian reflection
//
Vec3f
computeOneNormal(vector<float> &values, vector<Vec3f> lightDirs)
{
    int count = values.size();
    assert(count == lightDirs.size());

    CVector<double> b(count);
    CMatrix<double> M(count,3);

	////////////////////////////////////
	// TODO: fill in the entries of the
	// vector b and matrix M
	/***** BEGIN TODO *****/
	/***** END TODO *****/

	CMatrix<double>MT (M.transpose());
	CMatrix<double>MTM (MT*M);
	CVector<double>MTb (MT*b);

	CVector<double> result = solve3x3(MTM,MTb);

//    CVector<double> result = M.solveLinearSystem(b);
    assert(result.Length() == 3);

    Vec3f answer((float)result[0],(float)result[1],(float)result[2]);
    answer = answer.normalized();
    if ((answer.x == 0) && 
	(answer.y == 0) &&
	(answer.z == 0))
	return Vec3f(0,0,1); // random guess
    else
	return answer;
}

