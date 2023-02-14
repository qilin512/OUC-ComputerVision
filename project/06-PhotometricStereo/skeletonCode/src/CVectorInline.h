#include <stdlib.h> 
#include <math.h>
#include <assert.h>

static const int i_one = 1;

template<class T>
CVector<T>::CVector() : _length(0),_data(new T[_length]), _ownsData(true)
{
  if (_data == NULL) 
    {
      printf("error\n");
      exit(1);
    } 
}

template<class T>
CVector<T>::CVector(int len) : 
  _length(len),_data(new T[_length]),_ownsData(true)
{
  if (_data == NULL) 
    {
      printf("error\n");
      exit(1);
    }
}

template<class T>
CVector<T>::CVector(const CVector & v) 
{ 
  _length = v._length; 
  _data = new T[_length]; 
  _ownsData = true;
  if (_data == NULL) 
    {
      printf("error\n");
      exit(1); 
    } 
  memcpy(_data,v._data,sizeof(T)*_length);
}

template<class T>
inline
T & CVector<T>::operator[](const int index) 
{
  assert(index>=0 && index<_length); 
  return _data[index]; 
}
template<class T>
inline
T CVector<T>::operator[](const int index) const 
{ 
  assert(index>=0 && index<_length);
  return _data[index];
}


template<class T>
inline
T & CVector<T>::operator()(const int index) 
{
  assert(index>=0 && index<_length); 
  return _data[index]; 
}
template<class T>
inline
T CVector<T>::operator()(const int index) const 
{ 
  assert(index>=0 && index<_length);
  return _data[index];
}
  
template<class T>
inline
CVector<T> & CVector<T>::operator=(const CVector & v) 
{ 
  if( _length != v._length ) 
  {
    if (_ownsData)
      delete [] _data;

    _length = v._length;
    _data = new T[_length];
    if (_data == NULL) 
      {
	printf("error\n");
	exit(1);
      } 
    _ownsData = true;
  }
					   
  memcpy( _data, v._data, sizeof(T)*_length );
  return *this;
}


#if 0
inline CVector<double> & CVector<double>::operator+=(const CVector & v)
{
  assert(_length == v._length);
  double alpha = 1;
  int one = 1;
  DAXPY(&_length,&alpha,v._data,&one,_data,&one);
  return *this;
}
#endif

inline CVector<double> & CVector<double>::daxpy(const CVector & x,
						double alpha)
{
  assert(_length == x._length);
  int one = 1;
  for (int i = 0; i<_length; i++)
	  _data[i] += alpha * x[i];
//  DAXPY(&_length,&alpha,x._data,&one,_data,&one);
  return *this;
}

#if 0
inline CVector<double> & CVector<double>::operator-=(const CVector & v)
{
  assert(_length == v._length);
  double alpha = -1;
  int one = 1;
  DAXPY(&_length,&alpha,v._data,&one,_data,&one);
  return *this;
}
#endif


template<class T>
inline
CVector<T> & CVector<T>::operator+=(const CVector & v) 
{
  assert(_length == v._length);
  for(int i=0;i<_length;i++)
    _data[i] += v._data[i];
  return *this;

}

template<class T>
inline
CVector<T> & CVector<T>::operator-=(const CVector & v) 
{
  assert(_length == v._length);
  for(int i=0;i<_length;i++) 
  _data[i] -= v._data[i]; 
  return *this;
}

template<class T>
inline
CVector<T> & CVector<T>::operator/=(const CVector & v) 
{
  assert(_length == v._length);
  for(int i=0;i<_length;i++)
  _data[i] /= v._data[i]; 
  return *this;
}

template<class T>
inline
CVector<T> & CVector<T>::operator*=(const T & val) 
{ 
  for(int i=0;i<_length;i++)
  _data[i] *= val;
  return *this;
}

template<class T>
inline
CVector<T> & CVector<T>::operator/=(const T & val) 
{ 
  for(int i=0;i<_length;i++)
  _data[i] /= val;
  return *this;
}

template<class T>
inline
CVector<T>  CVector<T>::operator+(const CVector & v) const
{
  assert(_length == v._length);
  T * data=new T[_length];
  if (_data == NULL) 
    {
      printf("error\n");
      exit(1);
    } 
  for(int i=0;i<_length;i++) 
    data[i] = _data[i]+v._data[i];
  return CVector(_length,data,true);
}

template<class T>
inline
CVector<T>  CVector<T>::operator-(const CVector & v) const 
{ 
  assert(_length == v._length);
  T * data=new T[_length];
  if (_data == NULL) 
    {
      printf("error\n");
      exit(1);
    }
  for(int i=0;i<_length;i++) 
    data[i] = _data[i]-v._data[i];
  return CVector(_length,data,true);
}

template<class T>
inline
CVector<T>  CVector<T>::operator+(const T & v) const 
{ 
  T * data=new T[_length];
  if (_data == NULL)
    {
      printf("error\n");
      exit(1);
    }
  for(int i=0;i<_length;i++) 
    data[i] = _data[i]+v;
  return CVector(_length,data,true);
}

template<class T>
inline
CVector<T> CVector<T>::operator-(const T & v) const
{ 
  T * data=new T[_length];
  if (_data == NULL)
    {
      printf("error\n");
      exit(1);
    }
  for(int i=0;i<_length;i++) 
    data[i] = _data[i]-v;
  return CVector(_length,data,true); 
}

template<class T>
inline
CVector<T>  CVector<T>::operator*(const T & v) const 
{ 
  T * data=new T[_length];
  if (_data == NULL) 
    {
      printf("error\n");
      exit(1);
    }
  for(int i=0;i<_length;i++)
    data[i] = _data[i]*v;
  return CVector(_length,data,true); 
}

template<class T>
inline
CVector<T> CVector<T>::mult(const CVector & v) const
{
  assert(_length == v._length);
  T * data=new T[_length];
  if (_data == NULL) 
    {
      printf("error\n");
      exit(1);
    }
  for(int i=0;i<_length;i++)
    data[i] = _data[i]*v._data[i];
  return CVector(_length,data,true); 
}

template<class T>
CVector<T>  CVector<T>::operator/(const T & v) const
{ 
  T * data=new T[_length];
  if (_data == NULL) 
    {
      printf("error\n");
      exit(1);
    }

  for(int i=0;i<_length;i++)
    data[i] = _data[i]/v;

  return CVector(_length,data,true);
}
#if 0
inline double CVector<double>::dot(const CVector & v) const
{
  assert(_length == v._length);
  int one=1;
  return DDOT(&_length,_data,&one,v._data,&one);
}
#endif

template<class T>
T CVector<T>::dot(const CVector & v) const 
{ 
  assert(_length == v._length);
  T sum = 0; 
  for(int i=0;i<_length;i++)
    sum += _data[i]*v._data[i];
  return sum; 
}
template<class T>
bool CVector<T>::operator==(const CVector & v) const 
{
  assert(_length == v._length); 
  for(int i=0;i<_length;i++)    
  if (_data[i] != v._data[i])   
  return false; 
  return true;
}
template<class T>
bool CVector<T>::operator!=(const CVector & v) const
{
  return ! (*this == v);
}


template<class T> 
T CVector<T>::L2sqr() const { return dot(*this); }

#if 0
inline double CVector<double>::dist(const CVector & v) const
{
  CVector vec = *this - v; 
  int one = 1;
  return DNRM2(&_length,vec._data,&one);
}
#endif


template<class T>
inline
double CVector<T>::dist2(const CVector & v) const
{
  CVector vec = *this - v; 
  return vec.L2sqr();
}

template<class T>
inline
double CVector<T>::dist2(const CVector & v, double k1, double k2) const
{
  CVector vec = *this - v; 
  CVector dvec(vec.dvdi());
  double reply = vec.L2sqr();
  if (k1)
    reply += k1 * dvec.L2sqr();
  if (k2) {
    CVector d2vec(dvec.dvdi());
    reply += k2 * d2vec.L2sqr();
  }
  return reply;
}

template<class T>
inline
double CVector<T>::distInfty(const CVector & v) const
{
  CVector vec = *this - v; 
  return vec.Linfty();
}

template<class T>
inline
double CVector<T>::logdist2(const CVector & v) const
{
  CVector vec = this->log() - v.log(); 
  return vec.L2sqr();
}

template<class T>
inline
double CVector<T>::logdist2(const CVector & v, double c) const
{
  double sum = 0;
  for (int i = 0; i<_length; i++) {
    double diff = _data[i] - v[i];
    sum += ::log(diff*diff + c);
  }

  return sum - _length * ::log(c);
}

template<class T>
inline
double CVector<T>::dist(const CVector & v) const
{
  return sqrt(dist2(v));
}

template<class T>
double CVector<T>::maxAbsEl() 
{
  double v = (*this)[0]; 
  if (v < 0) v = -v;
  for(int i=1;i<Length(); i++) 
    if ((*this)[i] > v)
      v = (*this)[i];
    else
      if (-(*this)[i] > v)
	v = -(*this)[i];
  return v;
}

template<class T>
void CVector<T>::printFloat(FILE * fp)
{
  for(int i=0;i<_length;i++)
    fprintf(fp,"%.8f ",_data[i]);
}
      
#if 0
inline double CVector<double>::L2() const
{
  int one = 1;
  return DNRM2(&_length,_data,&one);
}
#endif

template<class T> 
double CVector<T>::L2() const { return sqrt(dot(*this)); }

template<class T> 
T CVector<T>::sum() const 
{
  double r = _data[0];
  for(int i=1;i<_length;i++)
    r += _data[i];
  return r;
}

template<class T>
T CVector<T>::prod() const
{
  double r = _data[0];
  for(int i=1;i<_length;i++)
    r *= _data[i];
  return r;
}

template<class T> 
T CVector<T>::Linfty() const 
{
  T r = (T)fabs(_data[0]);
  for(int i=1;i<_length;i++)
    {
      T r2 = (T)fabs(_data[i]);
      if (r2 > r)
	r = r2;
    }
      
  return r;
}


inline CVector<double> CVector<double>::sigmoid() const 
{
  double * data = new double[_length];
  if (data == NULL)
    {
      printf("error\n");
      exit(1);
    }

  for(int i=0;i<_length;i++)
    data[i] = 1/(1+exp(-_data[i]));

  return CVector<double>(_length,data,true);
}


inline CVector<double> CVector<double>::tanh() const 
{
  double * data = new double[_length];
  if (data == NULL)
    {
      printf("error\n");
      exit(1);
    }

  for(int i=0;i<_length;i++)
    {
      double e1 = exp(_data[i]);
      double e2 = exp(-_data[i]);

      data[i] = (e1-e2)/(e1+e2);
    }

  return CVector<double>(_length,data,true);
}

#define max(a,b) ((a)>(b)?(a):(b))

inline CVector<double> CVector<double>::log() const
{
  double * data = new double[_length];
  if (data == NULL)
    {
      printf("error\n");
      exit(1);
    }

  for(int i=0;i<_length;i++)
    if (_data[i] <= 0)
      data[i] = -1000;
    else
      data[i] = max(-1000.,::log(_data[i]));

  return CVector<double>(_length,data,true);
}

inline CVector<double> CVector<double>::dvdi() const
{
  double * data = new double[_length];
  if (data == NULL)
    {
      printf("error\n");
      exit(1);
    }

  for(int i=0;i<_length;i++)
    data[i] = _data[(i+1)%_length] - _data[i];

  return CVector<double>(_length,data,true);
}
/*
template<class T> bool
CVector<T>::noNans() const
{
  for(int i=0;i<_length;i++)
    if (isnan(_data[i]))
      return false;
  return true;
}
*/