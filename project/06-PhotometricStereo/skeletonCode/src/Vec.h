#ifndef _VEC_H_
#define _VEC_H_

#include <cmath>
#include <iostream>

template <class T>
class Vec3 {
 public:
    Vec3()              : x(0), y(0), z(0) {}
    Vec3(T a)           : x(a), y(a), z(a) {}
    Vec3(T a, T b, T c) : x(a), y(b), z(c) {}

    const Vec3 & operator += (const Vec3 &v)
	{ x += v.x; y += v.y; z += v.z; return *this; }

    Vec3         operator + (const Vec3 &v) const
	{ return Vec3 (x + v.x, y + v.y, z + v.z); }

    Vec3         operator - (const Vec3 &v) const
	{ return Vec3 (x - v.x, y - v.y, z - v.z); }
    
    T            dot (const Vec3 &v) const
	{ return x * v.x + y * v.y + z * v.z; }

    Vec3         cross (const Vec3 &v) const
	{ return Vec3 (y * v.z - z * v.y,
		       z * v.x - x * v.z,
		       x * v.y - y * v.x); }

    Vec3         operator * (T a) const
	{ return Vec3 (x * a, y * a, z * a); }

    T            length() const
	{ return sqrt(dot(*this)); }

    Vec3<T> normalized () const
	{
	    T l = length();

	    if (l == 0)
		return Vec3 (T (0));

	    return Vec3 (x / l, y / l, z / l);
	}

    bool operator != (const Vec3 &o) const
	{
	    return (x != o.x) || (y != o.y) || (z != o.z);
	}

 public:
    T x;
    T y;
    T z;
};

template <class T>
std::ostream &
operator << (std::ostream &os, const Vec3<T> &vec)
{
    return os << vec.x << " " << vec.y << " " << vec.z;
}

template <class S, class T> Vec3<T>
operator * (S a, const Vec3<T> &v)
{
    return Vec3<T> (a * v.x, a * v.y, a * v.z);
}

template <class Vec>
Vec
project (const Vec &s, const Vec &t)
{
    Vec sNormalized = s.normalized();
    return sNormalized * sNormalized.dot(t);
}

template <class Vec>
Vec
reflect (const Vec &s, const Vec &t)
{

	Vec v;

	/////////////////////////////////////////////
	// TODO: return the vector s reflected about t
	//
	/***** BEGIN TODO *****/

	/***** END TODO *****/

	return v;
}

typedef Vec3<float>  Vec3f;
typedef Vec3<double> Vec3d;
typedef Vec3<int>    Vec3i;

#endif // _VEC_H_
