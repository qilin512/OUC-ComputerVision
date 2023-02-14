#ifndef __CG_H__
#define __CG_H__

#include "CVector.h"
#include "CMatrixSparse.h"





template <class T>
double solveQuadratic(const CMatrixSparse<T> & A, const CVector<T> & b, 
	      CVector<T> & x,int i_max, double epsilon)
{
  //my conjugate gradient solver for .5*x'*A*x -b'*x, based on the
  // tutorial by Jonathan Shewchuk  (or is it +b'*x?)
  
  printf("Performing conjugate gradient optimization\n");

  int numvars = x.Length();
  assert(b.Length() == numvars && A.rows() == numvars &&
	 A.columns() == numvars);

  int i =0;

  CVector<T> r = b-A*x;
  CVector<T> d= r;
  double delta_new = r.dot(r);
  double delta_0 = delta_new;

  int numrecompute = (int)floor(sqrt(float(numvars)));
  //int numrecompute = 1;
  printf("numrecompute = %d\n",numrecompute);

  printf("delta_new = %f\n", delta_new);
  while (i < i_max && delta_new > epsilon)//epsilon*epsilon*delta_0)
    {
      printf("Step %d, delta_new = %f      \r",i,delta_new);
      
      CVector<T> q = A*d;
      double alpha = delta_new/d.dot(q);
      x.daxpy(d,alpha); //      x += d*alpha;
      if (i % numrecompute == 0)
	{
	  //	  printf(" ** recompute\n");
	  r = b-A*x;
	}
      else
	r.daxpy(q,-alpha); //  r = r-q*alpha;
      double delta_old = delta_new;
      delta_new = r.dot(r);
      double beta = delta_new/delta_old;
      d = r+d*beta;
      i++;
    }

  return delta_new;
  //  return delta_new <= epsilon;
  //  return !(delta_new > epsilon*epsilon*delta_0);
}




#endif
