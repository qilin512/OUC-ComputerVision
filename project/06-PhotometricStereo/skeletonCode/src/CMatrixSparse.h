#ifndef __CMATRIXSPARSE_H__
#define __CMATRIXSPARSE_H__

#include <map>
#include <set>

using namespace std;

#include "CVector.h"


template <class T>
class CMatrixSparse
{
 private:
  int _rows;
  int _columns;
  map<pair<int,int>,T> _data;
  map<int,set<int> > _rowEntries;

 public:
  CMatrixSparse(int rows,int columns);
  CMatrixSparse(const CMatrixSparse & v);

  T & get(int i,int j);
  T get(int i,int j) const;
  T & operator() (int i,int j);
  T operator() (int i,int j) const;
  CVector<T> operator*(const CVector<T> & v) const;

  CMatrixSparse<T> mTm () const;  // compute the product of m^T and m

  bool isSymmetric() const;

  int rows() const { return _rows; }
  int columns() const { return _columns; }

  const map<pair<int,int>,T> &data() const { return _data; }

};

template <class T>
CVector<T> operator *(const CVector<T> &v, const CMatrixSparse<T> &m);// right-multiplication

#include "CMatrixSparseInline.h"


#endif

