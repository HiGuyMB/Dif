//------------------------------------------------------------------------------
// Copyright (c) 2015 HiGuy Smith
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of the project nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#ifndef point3_h
#define point3_h

#include <math.h>
#include <limits>

template <typename T>
class Point3 : public Readable, public Writable {
public:
	T x;
	T y;
	T z;

	Point3() : x(0), y(0), z(0) {};
	Point3(const T &x, const T &y, const T &z) : x(x), y(y), z(z) {};
	Point3(const Point3 &point) : x((T) point.x), y((T) point.y), z((T) point.z) {}

	bool operator==(const Point3<T> &point) const;
	bool operator!=(const Point3<T> &point) const;
	Point3  operator+ (const Point3<T> &point) const;
	Point3 &operator+=(const Point3<T> &point);
	Point3  operator- (const Point3<T> &point) const;
	Point3 &operator-=(const Point3<T> &point);
	Point3  operator* (const T scalar) const;
	Point3 &operator*=(const T scalar);
	Point3  operator/ (const T scalar) const;
	Point3 &operator/=(const T scalar);
	Point3 operator+() const;
	Point3 operator-() const;

	T dot(const Point3<T> &point) const;
	Point3 cross(const Point3<T> &point) const;
	Point3 project(const Point3<T> &point) const;
	Point3 reject(const Point3<T> &point) const;

	T length() const;
	T lengthSquared() const;
	T distance(const Point3<T> &point) const;
	T angle(const Point3<T> &point) const;

	Point3<T> convert() const;
	Point3<T> roundThousands() const;
	Point3<T> normalize() const;
	Point3<T> normalize(const T &scalar) const;

	virtual bool read(std::istream &stream);
	virtual bool write(std::ostream &stream) const;
};

template <typename T>
inline bool Point3<T>::operator==(const Point3<T> &point) const {
	return (this - point).length() < std::numeric_limits<T>::epsillon();
}

template <typename T>
inline bool Point3<T>::operator!=(const Point3<T> &point) const {
	return !operator==(point);
}

template <typename T>
inline Point3<T> Point3<T>::operator+(const Point3<T> &point) const {
	return Point3<T>(x + point.x, y + point.y, z + point.z);
}

template <typename T>
inline Point3<T> &Point3<T>::operator+=(const Point3<T> &point) {
	x += point.x, y += point.y, z += point.z;
	return *this;
}

template <typename T>
inline Point3<T> Point3<T>::operator-(const Point3<T> &point) const {
	return Point3<T>(x - point.x, y - point.y, z - point.z);
}

template <typename T>
inline Point3<T> &Point3<T>::operator-=(const Point3<T> &point) {
	x -= point.x, y -= point.y, z -= point.z;
	return *this;
}

template <typename T>
inline Point3<T> Point3<T>::operator*(const T scalar) const {
	return Point3<T>(x * scalar, y * scalar, z * scalar);
}

template <typename T>
inline Point3<T> &Point3<T>::operator*=(const T scalar) {
	x *= scalar, y *= scalar, z *= scalar;
	return *this;
}

template <typename T>
inline Point3<T> Point3<T>::operator/(const T scalar) const {
	return Point3<T>(x / scalar, y / scalar, z / scalar);
}

template <typename T>
inline Point3<T> &Point3<T>::operator/=(const T scalar) {
	x /= scalar, y /= scalar, z /= scalar;
	return *this;
}

template <typename T>
inline Point3<T> Point3<T>::operator+() const {
	return Point3(+x, +y, +z);
}

template <typename T>
inline Point3<T> Point3<T>::operator-() const {
	return Point3(-x, -y, -z);
}

template <typename T>
inline T Point3<T>::dot(const Point3<T> &point) const {
	return x * point.x + y * point.y + z * point.z;
}

template <typename T>
inline Point3<T> Point3<T>::cross(const Point3<T> &point) const {
	return Point3<T>((y * point.z) - (z * point.y),
	                 (z * point.x) - (x * point.z),
	                 (x * point.y) - (y * point.x));
}

template <typename T>
inline Point3<T> Point3<T>::project(const Point3<T> &point) const {
	return point * (dot(point) / dot(*this));
}

template <typename T>
inline Point3<T> Point3<T>::reject(const Point3<T> &point) const {
	return *this - project(point);
}

template <typename T>
inline T Point3<T>::length() const {
	return sqrtf(x*x + y*y + z*z);
}

template <typename T>
inline T Point3<T>::lengthSquared() const {
	return x*x + y*y + z*z;
}

template <typename T>
inline T Point3<T>::distance(const Point3<T> &point) const {
	return (*this - point).length();
}

template <typename T>
inline T Point3<T>::angle(const Point3<T> &point) const {
	return acosf(dot(point) / length() / point.length());
}

template <typename T>
inline Point3<T> Point3<T>::convert() const {
	return Point3<T>(x, z, y);
}

template <typename T>
inline Point3<T> Point3<T>::normalize() const {
	return *this / (*this).length();
}

template <typename T>
inline Point3<T> Point3<T>::normalize(const T &scalar) const {
	return *this / (*this).length() * scalar;
}

template <typename T>
inline Point3<T> Point3<T>::roundThousands() const {
	return Point3<T>(floorf(x * 1000.0f) / 1000.f,
	                 floorf(y * 1000.0f) / 1000.f,
	                 floorf(z * 1000.0f) / 1000.f);
}

#endif
