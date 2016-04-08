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

#ifndef dif_types_h
#define dif_types_h

#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>

#define DIF_NAMESPACE namespace DIF {
#define DIF_NAMESPACE_END }

DIF_NAMESPACE

//Base types (names stolen from TGE because lazy)
typedef unsigned char      U8;
typedef unsigned short     U16;
typedef unsigned int       U32;
typedef unsigned long long U64;

typedef signed char      S8;
typedef signed short     S16;
typedef signed int       S32;
typedef signed long long S64;

typedef float F32;
typedef double F64;

struct Version {
	struct DIFVersion {
		enum Type {
			Unknown,
			MBG,
			TGE
		} type;
		U32 version;

		DIFVersion(U32 version = 44, Type type = Unknown) : type(type), version(version) {

		}
		std::string to_string() {
			switch (type) {
				case Unknown: return std::to_string(version) + " (Unknown)";
				case MBG:     return std::to_string(version) + " (MBG)";
				case TGE:     return std::to_string(version) + " (TGE)";
			}
		}
	} dif;
	struct InteriorVersion {
		enum Type {
			Unknown,
			MBG,
			TGE,
			TGEA
		} type;
		U32 version;

		InteriorVersion(U32 version = 0, Type type = Unknown) : type(type), version(version) {

		}
		std::string to_string() {
			switch (type) {
				case Unknown: return std::to_string(version) + " (Unknown)";
				case MBG:     return std::to_string(version) + " (MBG)";
				case TGE:     return std::to_string(version) + " (TGE)";
				case TGEA:    return std::to_string(version) + " (TGEA)";
			}
		}
		bool isTGE() {
			return type == TGE || type == MBG;
		}
	} interior;
	struct MaterialListVersion {
		U32 version;
		MaterialListVersion(U32 version = 0) : version(version) {

		}
		std::string to_string() {
			return std::to_string(version);
		}
	} material;
	struct VehicleCollisionFileVersion {
		U32 version;
		VehicleCollisionFileVersion(U32 version = 0) : version(version) {

		}
		std::string to_string() {
			return std::to_string(version);
		}
	} vehicleCollision;

	Version(DIFVersion difVersion = DIFVersion(), InteriorVersion interiorVersion = InteriorVersion(), MaterialListVersion materialVersion = MaterialListVersion(), VehicleCollisionFileVersion vehicleCollisionVersion = VehicleCollisionFileVersion()) :
		dif(difVersion),
		interior(interiorVersion),
		material(materialVersion),
		vehicleCollision(vehicleCollisionVersion) {

	}
};

struct Readable {
public:
	virtual bool read(std::istream &stream, Version &version) = 0;
};

struct Writable {
public:
	virtual bool write(std::ostream &stream, Version version) const = 0;
};

DIF_NAMESPACE_END

#include <dif/base/point2.h>
#include <dif/base/point3.h>
#include <dif/base/color.h>

DIF_NAMESPACE

typedef Point2<S16> Point2I;
typedef Point2<F32> Point2F;
typedef Point3<S32> Point3I;
typedef Point3<F32> Point3F;
typedef Point3<F64> Point3D;

typedef Color<U8>  ColorI;
typedef Color<F32> ColorF;

typedef std::vector<std::pair<std::string, std::string>> Dictionary;

//More names stolen from TGE

class QuatF : public Readable, public Writable {
public:
	F32 w;
	F32 x;
	F32 y;
	F32 z;

	QuatF() : w(0.0f), x(0.0f), y(0.0f), z(0.0f) {}

	virtual bool read(std::istream &stream, Version &version);
	virtual bool write(std::ostream &stream, Version version) const;
};

class PlaneF : public Readable, public Writable {
public:
	F32 x;
	F32 y;
	F32 z;
	F32 d;

	PlaneF() : x(0.0f), y(0.0f), z(0.0f), d(0.0f) {}

	virtual bool read(std::istream &stream, Version &version);
	virtual bool write(std::ostream &stream, Version version) const;
};

class BoxF : public Readable, public Writable {
public:
	F32 minX;
	F32 minY;
	F32 minZ;
	F32 maxX;
	F32 maxY;
	F32 maxZ;

	BoxF() : minX(0.0f), minY(0.0f), minZ(0.0f), maxX(0.0f), maxY(0.0f), maxZ(0.0f) {}

	inline Point3F getMin() const {
		return Point3F(minX, minY, minZ);
	}
	inline Point3F getMax() const {
		return Point3F(maxX, maxY, maxZ);
	}
	inline Point3F getCenter() const {
		return (getMax() + getMin()) / 2;
	}

	virtual bool read(std::istream &stream, Version &version);
	virtual bool write(std::ostream &stream, Version version) const;
};

class SphereF : public Readable, public Writable {
public:
	F32 x;
	F32 y;
	F32 z;
	F32 radius;

	SphereF() : x(0.0f), y(0.0f), z(0.0f), radius(0.0f) {}

	virtual bool read(std::istream &stream, Version &version);
	virtual bool write(std::ostream &stream, Version version) const;
};

class PNG : public Readable, public Writable {
public:
	U32 size;
	U8 *data;

	PNG() : size(0), data(nullptr) {}

	virtual bool read(std::istream &stream, Version &version);
	virtual bool write(std::ostream &stream, Version version) const;

	~PNG() {
//		delete [] data;
	}
};

class MatrixF : public Readable, public Writable {
public:
	F32 m[16];

	MatrixF() {}

	virtual bool read(std::istream &stream, Version &version);
	virtual bool write(std::ostream &stream, Version version) const;
};

DIF_NAMESPACE_END

#endif
