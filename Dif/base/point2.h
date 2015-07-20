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

#ifndef point2_h
#define point2_h

#include <math.h>
#include <limits>

template <typename T>
class Point2 : public Readable, public Writable {
public:
	T x;
	T y;

	Point2() : x(0), y(0) {};
	Point2(T x, T y) : x(x), y(y) {};
	Point2(const Point2 &point) : x((T) point.x), y((T) point.y) {};

	bool operator==(const Point2<T> &point) const;
	bool operator!=(const Point2<T> &point) const;
	Point2  operator+ (const Point2<T> &point) const;
	Point2 &operator+=(const Point2<T> &point);
	Point2  operator- (const Point2<T> &point) const;
	Point2 &operator-=(const Point2<T> &point);
	Point2  operator* (const T scalar) const;
	Point2 &operator*=(const T scalar);
	Point2  operator/ (const T scalar) const;
	Point2 &operator/=(const T scalar);
	Point2 operator+() const;
	Point2 operator-() const;

	T length() const;
	T lengthSquared() const;
	T distance(const Point2<T> &point) const;

	Point2<T> normalize() const;
	Point2<T> roundThousands() const;

	/**
	 Reads a Point2 from a stream
	 @param stream The stream to read from
	 @return If the operation was successful
	 */
	virtual bool read(std::istream &stream);
	/**
	 Writes a Point2 to a stream
	 @param stream The stream to write to
	 @return If the operation was successful
	 */
	virtual bool write(std::ostream &stream) const;
};

template <typename T>
inline bool Point2<T>::operator==(const Point2<T> &point) const {
	return (this - point).length() < std::numeric_limits<T>::epsillon();
}

template <typename T>
inline bool Point2<T>::operator!=(const Point2<T> &point) const {
	return !operator==(point);
}

template <typename T>
inline Point2<T> Point2<T>::operator+(const Point2<T> &point) const {
	return Point2<T>(x + point.x, y + point.y);
}

template <typename T>
inline Point2<T> &Point2<T>::operator+=(const Point2<T> &point) {
	x += point.x, y += point.y;
	return *this;
}

template <typename T>
inline Point2<T> Point2<T>::operator-(const Point2<T> &point) const {
	return Point2<T>(x - point.x, y - point.y);
}

template <typename T>
inline Point2<T> &Point2<T>::operator-=(const Point2<T> &point) {
	x -= point.x, y -= point.y;
	return *this;
}

template <typename T>
inline Point2<T> Point2<T>::operator*(const T scalar) const {
	return Point2<T>(x * scalar, y * scalar);
}

template <typename T>
inline Point2<T> &Point2<T>::operator*=(const T scalar) {
	x *= scalar, y *= scalar;
	return *this;
}

template <typename T>
inline Point2<T> Point2<T>::operator/(const T scalar) const {
	return Point2<T>(x / scalar, y / scalar);
}

template <typename T>
inline Point2<T> &Point2<T>::operator/=(const T scalar) {
	x /= scalar, y /= scalar;
	return *this;
}

template <typename T>
inline Point2<T> Point2<T>::operator+() const {
	return Point2(+x, +y);
}

template <typename T>
inline Point2<T> Point2<T>::operator-() const {
	return Point2(-x, -y);
}

template <typename T>
inline T Point2<T>::length() const {
	return sqrtf(x*x + y*y);
}

template <typename T>
inline T Point2<T>::lengthSquared() const {
	return x*x + y*y;
}

template <typename T>
inline T Point2<T>::distance(const Point2<T> &point) const {
	return (*this - point).length();
}

template <typename T>
inline Point2<T> Point2<T>::normalize() const {
	return *this / (*this).length();
}

template <typename T>
inline Point2<T> Point2<T>::roundThousands() const {
	return Point2<T>(floorf(x * 1000.0f) / 1000.f,
					 floorf(y * 1000.0f) / 1000.f);
}

#endif
