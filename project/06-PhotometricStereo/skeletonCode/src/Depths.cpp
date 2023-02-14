#include "Depths.h"

#include "CVector.h"
#include "CMatrixSparse.h"
#include "cg.h"

#define CGEPSILON .5

namespace {

void copyDepths(const CVector<double> & z, 
		const CIntImage & zind,
		DepthImage * depths)
{
    double minD=z(0);
    double maxD=z(0);
      
    for(int i=0;i<z.Length();i++)
    {
	maxD = max(z(i),maxD);
	minD = min(z(i),minD);
    }
    cout << "z range (" << minD << ", " << maxD << ")" << endl;
      
    for (int y = 0; y < depths->Shape().height; y++)
	for (int x = 0; x < depths->Shape().width; x++)
	{
	    int index = zind.Pixel(x,y,0);
	    if (index == -1)
		depths->Pixel(x,y,0) = 0;
	    else
		depths->Pixel(x,y,0) = float(z(index)-minD);
	}
}

bool
pixvalid(const CIntImage &i, int x, int y)
{
    return i.Shape().InBounds(x,y) && (i.Pixel(x,y,0) != -1);
}

void makeMmatrix(CMatrixSparse<double> & M, const CIntImage & zind,
		 const NormalImage &normals)
{

  int i,j;
  int n=0;
	  
  printf("Making M matrix\n");

  ///////////////////////////////////////////////////////////
  // TODO:  fill in the entries of the M matrix.  It need not
  //   have entries in every row.
  //
  /***** BEGIN *****/

  /***** END *****/

}


void makeVvector(CVector<double> & v, const CIntImage & zind,
		 const NormalImage & normals)
{
  int i,j,n=0;

  assert(zind.Shape() == normals.Shape());

  printf("Making v vector\n");

  ///////////////////////////////////////////////////////
  // TODO:  fill in the entries of the v vector.  It need
  //  not have entries in every row.
  //
  /***** BEGIN *****/

   /***** END *****/

}


} // unnamed namespace

void
computeDepths(DepthImage *depths, const NormalImage *normals,
	      const ColorImage *mask)
{
    const int width = normals->Shape().width;
    const int height = normals->Shape().height;

	/* Construct an index map showing for each pixel
	   it's corresponding index in the depth vector.
	   Also figure out how many pixels there are
	   inside the mask. */
    int numvars = 0;
    CIntImage zind(width,height,1);
    for (int y = 0; y<height; y++)
	for (int x = 0; x<width; x++)
	{
	    if (mask && !mask->Pixel(x,y,0))
		zind.Pixel(x,y,0) = -1;
	    else
		zind.Pixel(x,y,0) = numvars++;
	}

	/* The vector z will contain the depths for each
	    pixel. */
    CVector<double> z(numvars);
    z.clear();

	/* This is the number of constraints we have.
	   At most, we'll have two constraints for each
	   pixel, to its left and right neighbour. */

	int numConstraints = 2*numvars;  // can be a loose upper bound

	/* The constraint matrix.  One row for every constraint, one
	   column for every variable. */

    CMatrixSparse<double> M(numConstraints,numvars);
    makeMmatrix(M,zind,*normals);

	/* The constraint vector.  One row for every constraint. */

	CVector<double> v(numConstraints);
    makeVvector(v,zind,*normals);

	/* We want to solve Mz = v in a least squares sense.  The
	   solution is M^T M z = M^T v.  We denote M^T M as A and
	   M^T v as b, so A z = b. */

	CMatrixSparse<double> A(M.mTm());
    assert(A.isSymmetric());
    CVector<double> r = A*z;  /* r is the "residual error" */
    CVector<double> b(v*M);

	// solve the equation A z = b
    solveQuadratic<double>(A,b,z,300,CGEPSILON);

	// copy the depths back from the vector z into the image depths
    copyDepths(z,zind,depths);
}

