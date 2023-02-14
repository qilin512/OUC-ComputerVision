#ifndef __CVECTOR_H__
#define __CVECTOR_H__

#include <stdio.h>

//#include "cvec2t.h"
//#include "compat.h"

template<class T>
class CMatrix;

template<class T>
class CMatrixSparse;

template<class T>
class CVector
{
 private:
  int _length;
  bool _ownsData;
  T * _data;

public:
  CVector();
  CVector(int len);
  CVector(const CVector & v);
  CVector(int len,T * data,bool ownsData) : 
    _length(len),_data(data),_ownsData(ownsData) { };  // work around some compiler bugs by making this public

  virtual ~CVector() { if (_ownsData) delete [] _data; }
  T & operator[](const int index);
  T operator[](const int index) const ;
  T & operator()(const int index);
  T operator()(const int index) const ;
  int Length() const { return _length; } 
  void clear() { memset(_data,0,_length*sizeof(T)); }
  CVector & operator=(const CVector & v);
  CVector & operator+=(const CVector & v);
  CVector & operator-=(const CVector & v);
  CVector & operator/=(const CVector & v);
  CVector & operator*=(const T & val) ;
  CVector & operator/=(const T & val) ;
  CVector & daxpy(const CVector & x,double alpha);
  CVector operator+(const CVector & v) const;
  CVector operator-(const CVector & v) const ;
  CVector operator+(const T & v) const ;
  CVector operator-(const T & v) const;
  CVector operator*(const T & v) const;
  CVector operator/(const T & v) const;
  CVector mult(const CVector & v) const;
  T dot(const CVector & v) const ;
  bool operator==(const CVector & v) const ;
  bool operator!=(const CVector & v) const;
  double L2() const;
  T L2sqr() const;
  double dist(const CVector & v) const;
  double dist2(const CVector & v) const;
  double distInfty(const CVector & v) const;
  double dist2(const CVector & v, double k1, double k2) const;
  double logdist2(const CVector & v) const;
  double logdist2(const CVector & v, double c) const;
  double maxAbsEl() ;
  CVector sigmoid() const;
  CVector tanh() const;
  CVector log() const;
  CVector dvdi() const;  // returns v[(i+1)%length] - v[i]
  T sum() const;
  T prod() const;
  T Linfty() const;

  //bool noNans() const;

  void copyFrom(const T * data) { memcpy(_data,data,_length*sizeof(T)); }
  void copyTo(T * data) const  { memcpy(data,_data,_length*sizeof(T)); }

  void printFloat(FILE * fp = stdout);

 private:
  friend class CMatrix<T>;
  friend class CMatrixSparse<T>;
};



#include "CVectorInline.h"


#endif
