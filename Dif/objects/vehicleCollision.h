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

#ifndef vehicleCollision_h
#define vehicleCollision_h

#include "types.h"

struct VehicleConvexHull : public Readable, public Writable {
	U32 hullStart;
	U16 hullCount;
	F32 minX;
	F32 maxX;
	F32 minY;
	F32 maxY;
	F32 minZ;
	F32 maxZ;
	U32 surfaceStart;
	U16 surfaceCount;
	U32 planeStart;
	U32 polyListPlaneStart;
	U32 polyListPointStart;
	U32 polyListStringStart;

	virtual bool read(std::istream &stream);
	virtual bool write(std::ostream &stream) const;
};

struct VehicleNullSurface : public Readable, public Writable {
	U32 windingStart;
	U16 planeIndex;
	U8 surfaceFlags;
	U32 windingCount;

	virtual bool read(std::istream &stream);
	virtual bool write(std::ostream &stream) const;
};

class VehicleCollision : public Readable, public Writable {
public:
	U32 vehicleCollisionFileVersion;

	std::vector<VehicleConvexHull> vehicleConvexHull;
	std::vector<U8> vehicleConvexHullEmitStringCharacter;
	std::vector<U32> vehicleHullIndex;
	std::vector<U16> vehicleHullPlaneIndex;
	std::vector<U32> vehicleHullEmitStringIndex;
	std::vector<U32> vehicleHullSurfaceIndex;
	std::vector<U16> vehiclePolyListPlaneIndex;
	std::vector<U32> vehiclePolyListPointIndex;
	std::vector<U8> vehiclePolyListStringCharacter;
	std::vector<VehicleNullSurface> vehicleNullSurface;

	/**
	 * Reads a VehicleCollision from a stream
	 * @param 1 The stream to read from
	 * @return If the operation was successful
	 */
	virtual bool read(std::istream &stream);
	/**
	 * Writes a VehicleCollision to a stream
	 * @param 1 The stream to write to
	 * @return If the operation was successful
	 */
	virtual bool write(std::ostream &stream) const;
};

#endif
