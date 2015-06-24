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

typedef struct {
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
} VehicleConvexHull;

typedef struct {
	U32 windingStart;
	U16 planeIndex;
	U8 surfaceFlags;
	U32 windingCount;
} VehicleNullSurface;

class VehicleCollision {
public:
	U32 vehicleCollisionFileVersion;

	U32 numVehicleConvexHulls;
	VehicleConvexHull *vehicleConvexHull;

	U32 numVehicleConvexHullEmitStrings;
	U8 *vehicleConvexHullEmitStringCharacter;

	U32 numVehicleHullIndices;
	U32 *vehicleHullIndex;

	U32 numVehicleHullPlaneIndices;
	U16 *vehicleHullPlaneIndex;

	U32 numVehicleHullEmitStringIndices;
	U32 *vehicleHullEmitStringIndex;

	U32 numVehicleHullSurfaceIndices;
	U32 *vehicleHullSurfaceIndex;

	U32 numVehiclePolyListPlanes;
	U16 *vehiclePolyListPlaneIndex;

	U32 numVehiclePolyListPoints;
	U32 *vehiclePolyListPointIndex;

	U32 numVehiclePolyListStrings;
	U8 *vehiclePolyListStringCharacter;

	U32 numVehicleNullSurfaces;
	VehicleNullSurface *vehicleNullSurface;

	/**
	 Reads a VehicleCollision from a FILE
	 @arg file - The FILE to read from (updates position)
	 */
	VehicleCollision(FILE *file);

	bool write(FILE *file) const;

	/**
	 Frees the VehicleCollision and all memory contained within it
	 */
	~VehicleCollision();
};

#endif
