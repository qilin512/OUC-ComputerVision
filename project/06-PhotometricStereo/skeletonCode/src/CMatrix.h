#ifndef __CMATRIX_H__
#define __CMATRIX_H__

#include <vector>

using namespace std;

#include "CVector.h"


template <class T>
class CMatrix
{
 private:
  int _rows;
  int _columns;
  T * _data;  // data stored in row-major order for library compatibility
  bool _ownsData;

 public:
  CMatrix();
  CMatrix(int rows,int columns);
  CMatrix(const CMatrix & v);
  CMatrix(const CVector<T> & v);
  virtual ~CMatrix() { if (_ownsData) { delete [] _data; }}

  T & get(int i,int j);
  T get(int i,int j) const;
  T & operator() (int i,int j);
  T operator() (int i,int j) const;
  void clear() { memset(_data,0,_rows*_columns*sizeof(T)); }
  CMatrix<T> & operator=(const CMatrix<T> & v);
  CMatrix<T> & operator+=(const CMatrix<T> & v);
  CVector<T> operator*(const CVector<T> & v) const;
  CMatrix operator*(const CMatrix & v) const;
  CMatrix<T> operator*(const T & v) const;
  CMatrix<T> operator/(const T & v) const;
  CVector<T> symmetricEigenvalues() ; 
  CVector<T> symmetricEigenvectors(CMatrix & evecs,int maxVec,int minVec) ; 
  CMatrix<T> transpose() const;
  CVector<T> getRow(int i) const;
  CVector<T> getColumn(int j) const;

  CMatrix<T> & daxpy(const CMatrix & v,const double alpha);

 // void svd(CMatrix<double> *& U, CVector<double> *&sigma, CMatrix<double> *& VT);
  CMatrix<double> symmetricPseudoinverse() ;
  CMatrix<double> pseudoinverse() const;

  double symmetricLogDeterminant(double evThresh =0.01);
  double symmetricLogDeterminantAbs(double evThresh =0.01);

  void copyColumn(int j,const CVector<T> & v);

  T * data() { return _data; }

  T sumsqr() const;

  void copyFrom(const T * data) { memcpy(_data,data,_rows*_columns*sizeof(T));}
  void copyTo(T * data) const  { memcpy(data,_data,_rows*_columns*sizeof(T)); }

  void printFloat(FILE * fp = stdout);

  int rows() const { return _rows; }
  int columns() const { return _columns; }

  static CMatrix<T> outer(const CVector<T> & v1,const CVector<T> & v2);

  CVector<T> solveLinearSystem(const CVector<T> & b) const;

  bool empty(T eps) const;
  bool equals(const CMatrix<T> & v, T eps) const;

 private:
  CMatrix(int rows, int columns, T * data, bool ownsData);
};


#ifdef LAPACK
// note: i haven't tested this with anything other than PCA<double>

template<class T> inline
void PCA(const vector<CVector<T>*> & inputVectors,
	 vector<CVector<T>*> & outputVectors,
	 CMatrix<T> & eigenvectors, CVector<T> & mean,
	 float minRatio);

template<class T> inline
CVector<T> PCAproject(const CVector<T> & pt,
			 const CMatrix<T> & eigenvectors, 
			 const CVector<T> & mean)
{
  return eigenvectors * (pt - mean);
}

#endif






#include "CMatrixInline.h"


#endif

