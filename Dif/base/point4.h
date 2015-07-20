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

#ifndef point4_h
#define point4_h

#include <math.h>
#include <limits>

template <typename T>
class Point4 : public Readable, public Writable {
public:
	T x;
	T y;
	T z;
	T w;

	Point4() : x(0), y(0), z(0), w(0) {};
	Point4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {};
	Point4(const Point4 &point) : x((T) point.x), y((T) point.y), z((T) point.z), w((T) point.w) {}

	bool operator==(const Point4<T> &point) const;
	bool operator!=(const Point4<T> &point) const;
	Point4  operator+ (const Point4<T> &point) const;
	Point4 &operator+=(const Point4<T> &point);
	Point4  operator- (const Point4<T> &point) const;
	Point4 &operator-=(const Point4<T> &point);
	Point4  operator* (const T scalar) const;
	Point4 &operator*=(const T scalar);
	Point4  operator/ (const T scalar) const;
	Point4 &operator/=(const T scalar);
	Point4 operator+() const;
	Point4 operator-() const;

	T length() const;
	T lengthSquared() const;
	T distance(const Point4<T> &point) const;

	Point4<T> normalize() const;
	Point4<T> roundThousands() const;

	virtual bool read(std::istream &stream);
	virtual bool write(std::ostream &stream) const;
};

template <typename T>
inline bool Point4<T>::operator==(const Point4<T> &point) const {
	return (this - point).length() < std::numeric_limits<T>::epsillon();
}

template <typename T>
inline bool Point4<T>::operator!=(const Point4<T> &point) const {
	return !operator==(point);
}

template <typename T>
inline Point4<T> Point4<T>::operator+(const Point4<T> &point) const {
	return Point4<T>(x + point.x, y + point.y, z + point.z, w + point.w);
}

template <typename T>
inline Point4<T> &Point4<T>::operator+=(const Point4<T> &point) {
	x += point.x, y += point.y, z += point.z, w += point.w;
	return *this;
}

template <typename T>
inline Point4<T> Point4<T>::operator-(const Point4<T> &point) const {
	return Point4<T>(x - point.x, y - point.y, z - point.z, w - point.w);
}

template <typename T>
inline Point4<T> &Point4<T>::operator-=(const Point4<T> &point) {
	x -= point.x, y -= point.y, z -= point.z, w -= point.w;
	return *this;
}

template <typename T>
inline Point4<T> Point4<T>::operator*(const T scalar) const {
	return Point4<T>(x * scalar, y * scalar, z * scalar, w * scalar);
}

template <typename T>
inline Point4<T> &Point4<T>::operator*=(const T scalar) {
	x *= scalar, y *= scalar, z *= scalar, w *= scalar;
	return *this;
}

template <typename T>
inline Point4<T> Point4<T>::operator/(const T scalar) const {
	return Point4<T>(x / scalar, y / scalar, z / scalar, w / scalar);
}

template <typename T>
inline Point4<T> &Point4<T>::operator/=(const T scalar) {
	x /= scalar, y /= scalar, z /= scalar, w /= scalar;
	return *this;
}

template <typename T>
inline Point4<T> Point4<T>::operator+() const {
	return Point4(+x, +y, +z, +w);
}

template <typename T>
inline Point4<T> Point4<T>::operator-() const {
	return Point4(-x, -y, -z, -w);
}

template <typename T>
inline T Point4<T>::length() const {
	return sqrtf(lengthSquared());
}

template <typename T>
inline T Point4<T>::lengthSquared() const {
	return x*x + y*y + z*z + w*w;
}

template <typename T>
inline T Point4<T>::distance(const Point4<T> &point) const {
	return (*this - point).length();
}

template <typename T>
inline Point4<T> Point4<T>::normalize() const {
	return *this / (*this).length();
}

template <typename T>
inline Point4<T> Point4<T>::roundThousands() const {
	return Point4<T>(floorf(x * 1000.0f) / 1000.f,
					 floorf(y * 1000.0f) / 1000.f,
					 floorf(z * 1000.0f) / 1000.f,
					 floorf(w * 1000.0f) / 1000.f);
}

#endif
