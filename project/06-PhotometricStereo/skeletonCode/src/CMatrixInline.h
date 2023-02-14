#include <float.h>

inline int imax(const int i1,const int i2) { return i1 > i2 ? i1 : i2; }

template<class T>
CMatrix<T>::CMatrix() : 
  _rows(0),_columns(0),_data(new T[_rows*_columns]), _ownsData(true)
{
  if (_data == NULL) 
    {
      printf("error\n");
      exit(1);
    } 
}

template<class T>
CMatrix<T>::CMatrix(int rows,int columns) : 
  _rows(rows),_columns(columns), _ownsData(true)
{
  _data = new T[_rows*_columns];
  if (_data == NULL) 
    {
      printf("error\n");
      exit(1);
    }
}

template<class T>
CMatrix<T>::CMatrix(int rows, int columns, T * data, bool ownsData) :
  _rows(rows),_columns(columns), _data(data), _ownsData(ownsData)
{ }

template<class T>
CMatrix<T>::CMatrix(const CMatrix & v) 
{ 
  _rows = v._rows;
  _columns = v._columns;
  _data = new T[_rows*_columns]; 
  if (_data == NULL) 
    {
      printf("error\n");
      exit(1); 
    } 
  _ownsData = true;
  memcpy(_data,v._data,sizeof(T)*_rows*_columns);
}


template<class T>
CMatrix<T>::CMatrix(const CVector<T> & v) 
{ 
  _rows = v._length;
  _columns = 1;
  _data = new T[_rows*_columns]; 
  if (_data == NULL) 
    {
      printf("error\n");
      exit(1); 
    } 
  _ownsData = true;
  memcpy(_data,v._data,sizeof(T)*_rows*_columns);
}

template<class T>
void CMatrix<T>::copyColumn(int j,const CVector<T> &v)
{
  assert(j >=0 && j < _columns);
  assert(v.Length() == _rows);

  memcpy( &(*this)(0,j), v._data, sizeof(T)*_rows);
}


template<class T>
CMatrix<T> & CMatrix<T>::operator=(const CMatrix & v)
{
  //  assert((_rows == v._rows) && (_columns == v._columns));

  if (_rows != v._rows || _columns != v._columns)
    {
      printf("reallocating\n");

      if (_ownsData)
	delete [] _data;
      _ownsData = true;
      _rows = v._rows;
      _columns = v._columns;
      _data = new T[_rows*_columns];
      if (_data == NULL)
	{
	  printf("error\n");
	  exit(1);
	}
    }

  memcpy(_data,v._data,sizeof(T)*_rows*_columns);
  return *this;
}

template<class T>
T & CMatrix<T>::get(int i,int j)
{
  assert(i>=0 && i<_rows && j>=0 && j<_columns);
  return _data[i + j*_rows];
}

template<class T>
T CMatrix<T>::get(int i,int j) const
{
  assert(i>=0 && i<_rows && j>=0 && j<_columns);
  return _data[i + j*_rows];
}


template<class T>
T & CMatrix<T>::operator() (int i,int j)
{
  assert(i>=0 && i<_rows && j>=0 && j<_columns);
  return _data[i + j*_rows];
}

template<class T>
T CMatrix<T>::operator() (int i,int j) const
{
  assert(i>=0 && i<_rows && j>=0 && j<_columns);
  return _data[i + j*_rows];
}


template<class T>
CVector<T> CMatrix<T>::getRow(int i) const
{
  assert(i>=0 && i<_rows);
  double * data = new double[_columns];
  for(int j=0;j<_columns;j++)
    data[j] = get(i,j);
  return CVector<T>(_columns,data);
}

template<class T>
CVector<T> CMatrix<T>::getColumn(int j) const
{
  assert(j>=0 && j<_columns);
  double * data = new double[_rows];
  memcpy(data,&_data[j*_rows],_rows*sizeof(T));
  return CVector<T>(_rows,data,true);
}

template<class T>
CMatrix<T> CMatrix<T>::operator*(const T & v) const
{
  T * data = new T[_rows*_columns];
  if (data == NULL)
    {
      printf("error\n");
      exit(1);
    }
  for(int i=0;i<_rows*_columns;i++)
    data[i] = v*_data[i];

  return CMatrix<T>(_rows,_columns, data, true);
}
  
template<class T>
CMatrix<T> CMatrix<T>::operator/(const T & v) const
{
  T * data = new T[_rows*_columns];
  if (data == NULL)
    {
      printf("error\n");
      exit(1);
    }
  for(int i=0;i<_rows*_columns;i++)
    data[i] = _data[i]/v;

  return CMatrix<T>(_rows,_columns, data, true);
}

#if 0
inline
CMatrix<double> CMatrix<double>::operator*(const CMatrix<double> & v) const
{
  assert(_columns == v._rows);

  double * data = new double[_rows*v._columns];
  if (data == NULL)
    {
      printf("error\n");
      exit(1);
    }

  char transa = 'n';
  char transb = 'n';
  int m = rows();
  int n = v.columns();
  int k = columns();
  double alpha = 1;
  double zero = 0;

  DGEMM(&transa,&transb,&m, &n, &k, &alpha, _data, &m, v._data, &k, &zero,
	data, &m);
	
  //  void DGEMM(const char *transa, const char *transb, const int * m,
  //	     const int *n, const int *k,const double *alpha, const
  //	     double * A, const int * lda, const double *B, const int *
  //	     ldb, const double * beta, double * C, const int * ldc);

  return CMatrix<double>(_rows,v._columns,data,true);
}
#endif

template<class T>
CMatrix<T> CMatrix<T>::operator*(const CMatrix & v) const
{
  assert(_columns == v._rows);

  T * data = new T[_rows*v._columns];
  if (data == NULL)
    {
      printf("error\n");
      exit(1);
    }

  for(int i=0;i<_rows;i++)
    for(int j=0;j<v._columns;j++)
      {
	T & val = data[i+j*_rows];
	val = 0;

	for(int k=0;k<_columns;k++)
	  val += get(i,k)*v.get(k,j);
      }

  return CMatrix<T>(_rows,v._columns,data,true);
}


#if 0
inline
CVector<double> 
CMatrix<double>::operator*(const CVector<double> & v) const
{
  assert(v.Length() == _columns);

  double * data = new double[_rows];
  if (data == NULL)
    {
      printf("error\n");
      exit(1);
    }

  char trans = 'n';
  double alpha = 1;
  int m = rows();
  int n = columns();

  double zero = 0;
  int one = 1;

  DGEMV(&trans,&m,&n,&alpha,_data,&m,v._data,&one,&zero,data,&one);
	//  void DGEMV(const char *trans, const int * m, const int * n, const
	//	     double * alpha, const double * A, const int  * lda, 
	//	     const double * x, const int * incx, const double * beta,
	//	     double * y, const int * incy);

  //  for(int i=0;i<_rows;i++)
  //    {
  //      data[i] = 0;
  //      for(int j=0;j<_columns;j++)
  //	data[i] += get(i,j)*v[j];
  //    }


  return CVector<double>(_rows,data,true);
}
#endif

template<class T>
CVector<T> 
CMatrix<T>::operator*(const CVector<T> & v) const
{
  assert(v.Length() == _columns);

  T * data = new T[_rows];
  if (data == NULL)
    {
      printf("error\n");
      exit(1);
    }

  for(int i=0;i<_rows;i++)
    {
      data[i] = 0;
      for(int j=0;j<_columns;j++)
	data[i] += get(i,j)*v[j];
    }

  return CVector<T>(_rows,data,true);
}


template<class T>
CMatrix<T>
CMatrix<T>::transpose() const
{
  CMatrix mat(_columns,_rows);
  for(int i=0;i<_rows;i++)
    for(int j=0;j<_columns;j++)
      mat.get(j,i) = get(i,j);

  return mat;
}




#if 0
inline CVector<double> 
CMatrix<double>::symmetricEigenvalues() 
{
  assert(_rows == _columns);
  
  CMatrix<double> evecsfull(*this);
  
  double *eigsfull = new double[_rows];
  
  if (eigsfull == NULL)
    {
      printf("Can't allocate eigenvalues\n");
      exit(1);
    }

  // decompose into tridiagonal form
  char jobz = 'N';
  char uplo = 'U';
  int lwork = _rows*(_columns+2);
  double * work = new double[lwork];
  int info;

  if (work==NULL)
    {
      printf("error\n");
      exit(1);
    }


  DSYEV(&jobz,&uplo,&_rows,evecsfull._data,&_rows,eigsfull,work,&lwork,&info);

  if (info != 0)
    {
      printf("dsyev failed\n");
      exit(1);
    }
  
  delete [] work;

  return CVector<double>(_rows,eigsfull,true);
}
#endif


inline double
CMatrix<double>::symmetricLogDeterminant(double epsilon)
{
  double val =0;

  CVector<double> evs = symmetricEigenvalues();

    for(int i=0;i<evs.Length();i++)
      {
	if (evs[i] < 0)
	  val += log(epsilon);
	else
	  val += log(evs[i]+epsilon);
      }
    

  return val;
}

inline double
CMatrix<double>::symmetricLogDeterminantAbs(double epsilon)
{
  double val =0;

  CVector<double> evs = symmetricEigenvalues();

    for(int i=0;i<evs.Length();i++)
      {
	if (evs[i] < 0)
	  val += log(-evs[i]+epsilon);
	else
	  val += log(evs[i]+epsilon);
      }
    

  return val;
}


#if 0
inline CVector<double> 
CMatrix<double>::symmetricEigenvectors(CMatrix<double> & evecs,
				      int maxVal, int minVal) 
{
  int nVecs = maxVal - minVal + 1;
  
  assert(_rows == _columns);
  
  assert(evecs.rows() == _rows && evecs.columns() == nVecs);
  
  CMatrix<double> evecsfull(*this);
  
  double *eigsfull = new double[_rows];
  
  if (eigsfull == NULL)
    {
      printf("Can't allocate eigenvalues\n");
      exit(1);
    }

  // decompose into tridiagonal form
  char jobz = 'V';
  char uplo = 'U';
  int lwork = _rows*(_columns+2);
  double * work = new double[lwork];
  int info;
  if (work==NULL)
    {
      printf("error\n");
      exit(1);
    }
  //  DSYEVX(&jobz,&range,&uplo,&_rows,data._data,&_rows,&vl,&vu,&il,&iu,
  //	  0,&m,eigs,evecs._data,&_rows,work,&lwork,iwork,
  //	  ifail,&info);
  DSYEV(&jobz,&uplo,&_rows,evecsfull._data,&_rows,eigsfull,work,&lwork,&info);
  //  assert(m==nVecs);

     // we could also call the "expert" function to only extract some
     // eigenvalues

  if (info != 0)
    {
      printf("dsyev failed (%d) on this matrix:\n",info);
      printFloat();
      exit(1);
    }

  /*  
#ifndef NDEBUG
  printf("evals = ");
  for(int i=0;i<_rows;i++)
    printf("%f ",eigsfull[i]);
  printf("\n");

  printf("testing eigenmatrix from dsyev: diff = \n");
  for(int i=0;i<_rows;i++)
    {
      CVector<double> v = evecsfull.getColumn(i);
      CVector<double> diff = (*this)*v - v*eigsfull[i];

      printf("%f ",diff.L2());
    }
  printf("\n");
#endif
  */

  delete [] work;

  // copy the specified eigenvectors from the full eigenmatrix 

  assert(minVal >= 1 && maxVal <= _rows);

  if (minVal == 1 && maxVal == _rows)
    {
      evecs = evecsfull;
      return CVector<double>(_rows,eigsfull,true);
    }
  else
    {
      double * eigs2 = new double[evecs.columns()];
      for(int j=0;j<evecs.columns();j++)
	{
	  int srccol = j+minVal-1;
	  for(int i=0;i<_rows;i++)
	    evecs.get(i,j) = evecsfull.get(i,srccol);

	  eigs2[j] = eigsfull[srccol];
	}
	  delete [] eigsfull;
      return CVector<double>(evecs.columns(),eigs2,true);
    }
}


// solves A x = b, where A is "*this" and x is returned
//
// This assumes A has full rank!
inline CVector<double>
CMatrix<double>::solveLinearSystem(const CVector<double> & B) const
{
  assert(rows() == B.Length());

  if (columns() == 1)
    {
      int one=1;

      double dot1 = DDOT(&_rows,_data,&one,B._data,&one);
      double dot2 = DDOT(&_rows,_data,&one,_data,&one);

      double * data = new double[1];
      data[0] = dot1/dot2;
      return CVector<double>(1,data,true);
    }


  char trans = 'N';

  int m = rows();
  int n = columns();

  int nrhs = 1;
  int lda = m;
  int ldb = m;
  if (n > ldb)
    ldb = n;

  double * a= new double[m*n];
  double * b= new double[ldb];

  double tmp;
  
  int lwork = -1;

  int info;

  DGELS(&trans, &m, &n, &nrhs, a, &lda, b, &ldb, &tmp, &lwork, &info);

  if (info < 0)
    {
      printf("fatal error with DGELS getting lwork (info=%d)\n",info);
      exit(1);
    }

  lwork = (int)tmp;
  double * work = new double[lwork];

  if (a == NULL || b==NULL || work == NULL)
    {
      printf("out of memory in solveLinearSystem\n");
      exit(1);
    }

  // copy elements into a,b

  copyTo(a);
  B.copyTo(b);


  DGELS(&trans, &m, &n, &nrhs, a, &lda, b, &ldb, work, &lwork, &info);


  if (info < 0)
    {
      printf("fatal error with DGELS solving system (info=%d)\n",info);
      exit(1);
    }

  CVector<double> result(n);

  result.copyFrom(b);

  delete [] a;
  delete [] b;
  delete [] work;

  return result;
}
#endif



template <class T>
CMatrix<T> 
CMatrix<T>::outer(const CVector<T> & v1,const CVector<T> & v2)
{
  CMatrix<T> result(v1.Length(),v2.Length());
  for(int i=0;i<v1.Length();i++)
    for(int j=0;j<v2.Length();j++)
      result.get(i,j) = v1[i]*v2[j];

  return result;
}

template<class T>
T CMatrix<T>::sumsqr() const
{
  T result = _data[0]*_data[0];
  for(int i=1;i<_rows*_columns;i++)
    result += _data[i]*_data[i];

  return result;
}

#if 0
inline CMatrix<double> & CMatrix<double>::operator+=(const CMatrix & v)
{
  assert(_rows == v._rows && _columns == v._columns);
  double alpha = 1;
  int one = 1;
  int size = _rows*_columns;
  DAXPY(&size,&alpha,v._data,&one,_data,&one);
  return *this;
}

inline CMatrix<double> & CMatrix<double>::daxpy(const CMatrix & v,
						const double alpha)
     // add "alpha * v" to "this"
{
  assert(_rows == v._rows && _columns == v._columns);
  int one = 1;
  int size = _rows*_columns;
  DAXPY(&size,&alpha,v._data,&one,_data,&one);
  return *this;
}
#endif

template<class T>
CMatrix<T> & CMatrix<T>::operator+=(const CMatrix & v) 
{
  assert(_rows == v._rows && _columns == v._columns);
  for(int i=0;i<_rows*_columns;i++)
    _data[i] += v._data[i];
  return *this;
}






template<class T>
void CMatrix<T>::printFloat(FILE * fp)
{
  for(int i=0;i<_rows;i++)
    {
      for(int j=0;j<_columns;j++)
	fprintf(fp,"%.8f ",get(i,j));
      printf("\n");
    }
		
}

template <class PointN> inline
PointN centroid(const vector<PointN*> & pts)
{
  assert(!pts.empty());
  typename vector<PointN*>::const_iterator it = pts.begin();
  PointN c = (**it);

  for(++it; it != pts.end(); ++it)
    c += (**it);
  return c / pts.size();
}



inline void matmult(const double a[], const double b[], double c[])
{
  c[0] = a[0]*b[0] + a[2]*b[1];
  c[1] = a[1]*b[0] + a[3]*b[1];
  c[2] = a[0]*b[2] + a[2]*b[3];
  c[3] = a[1]*b[2] + a[3]*b[3];
}



inline 
double threshInvert(double val)
{
  if (val > FLT_EPSILON)
    return 1.0/val;
  if (val > -FLT_EPSILON)
    return 0;
  return 1.0/val;
}

inline
CMatrix<double> CMatrix<double>::symmetricPseudoinverse() 
{
  assert(_columns == _rows);

  if (_columns == 1)
    {
      CMatrix<double> result(1,1);
      result(0,0) = threshInvert(get(0,0));
      return result;
    }
 
  CMatrix<double> evecs(rows(), rows());
  CMatrix<double> evecs2(rows(), rows());

  CVector<double> evals = symmetricEigenvectors(evecs, rows(), 1);
  /*
  printf("evals = ");
  evals.printFloat();
  printf("\n");

  printf("e0 = %f %f\n", evals[0], 
	 float(evals[0] < FLT_EPSILON ? FLT_EPSILON : 1.0/evals[0]));
  */
  int i,j;
  for(i=0;i<rows();i++)
    {
      evals[i] = threshInvert(evals[i]);
    }

  /*
  printf("invevals = ");
  evals.printFloat();
  printf("\n");
  */

  for(i=0;i<rows();i++)
    for(j=0;j<rows();j++)
      evecs2(i,j) = evecs(j,i) * evals[i];
  /*
  printf("evecs = \n");
  evecs.printFloat();
  printf("\nevecs2 = \n");
  evecs2.printFloat();
  printf("\n");
  */  
  return evecs * evecs2;
}


inline
CMatrix<double> CMatrix<double>::pseudoinverse() const
{
  if (_columns == 1)
    {
      double ss = sumsqr();
      if (ss > FLT_EPSILON)
	return transpose() / ss;
      else
	{
	  CMatrix<double> result(_columns, _rows);
	  result.clear();
	  return result;
	}
    }

  CMatrix<double> scatter = transpose() * (*this);
      
  return scatter.symmetricPseudoinverse() * transpose();
}


  /*
  CMatrix<double> *U=NULL,* VT=NULL;
  CVector<double> *sigma=NULL;

  svd(U,sigma,VT);

  CMatrix<double> sigmaInv(VT->rows(), U->columns());
  sigmaInv.clear();

  for(int i=0;i<sigma->Length();i++)
    {
      if ( (*sigma)[i] > 1e-8)
	sigmaInv(i,i) = 1.0/ (*sigma)[i];
      else
	sigmaInv(i,i) = 1e-8;
    }

  CMatrix<double> result = VT->transpose() * sigmaInv * U->transpose();
	
  delete U;
  delete VT;
  delete sigma;

  return result;

}
  */

template<class T> bool
CMatrix<T>::empty(T eps) const
{
  for(int i=0;i<_rows*_columns;i++)
    if (abs(_data[i])>eps)
      return false;
  return true;
}

template<class T> bool
CMatrix<T>::equals(const CMatrix<T> & v, T eps) const
{
  if (_rows != v._rows || _columns != v._columns)
    return false;
  for (int i=0;i<_rows*_columns;i++)
    if (abs(_data[i] - v._data[i])>eps)
      return false;
  return true;
}

