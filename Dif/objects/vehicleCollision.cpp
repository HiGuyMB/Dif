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

#include "io.h"
#include "vehicleCollision.h"

bool VehicleCollision::read(std::istream &stream) {
	READTOVAR(vehicleCollisionFileVersion, U32); //vehicleCollisionFileVersion
	READTOVAR(vehicleConvexHull, std::vector<VehicleConvexHull>); //vehicleConvexHull
	READTOVAR(vehicleConvexHullEmitStringCharacter, std::vector<U8>); //vehicleConvexHullEmitStringCharacter
	READTOVAR(vehicleHullIndex, std::vector<U32>); //vehicleHullIndex
	READTOVAR(vehicleHullPlaneIndex, std::vector<U16>); //vehicleHullPlaneIndex
	READTOVAR(vehicleHullEmitStringIndex, std::vector<U32>); //vehicleHullEmitStringIndex
	READTOVAR(vehicleHullSurfaceIndex, std::vector<U32>); //vehicleHullSurfaceIndex
	READTOVAR(vehiclePolyListPlaneIndex, std::vector<U16>); //vehiclePolyListPlaneIndex
	READTOVAR(vehiclePolyListPointIndex, std::vector<U32>); //vehiclePolyListPointIndex
	READTOVAR(vehiclePolyListStringCharacter, std::vector<U8>); //vehiclePolyListStringCharacter
	READTOVAR(vehicleNullSurface, std::vector<VehicleNullSurface>); //vehicleNullSurface

	return true;
}

bool VehicleCollision::write(std::ostream &stream) const {
	WRITECHECK(vehicleCollisionFileVersion, U32);
	WRITE(vehicleConvexHull, std::vector<VehicleConvexHull>); //vehicleConvexHull
	WRITE(vehicleConvexHullEmitStringCharacter, std::vector<U8>); //vehicleConvexHullEmitStringCharacter
	WRITE(vehicleHullIndex, std::vector<U32>); //vehicleHullIndex
	WRITE(vehicleHullPlaneIndex, std::vector<U16>); //vehicleHullPlaneIndex
	WRITE(vehicleHullEmitStringIndex, std::vector<U32>); //vehicleHullEmitStringIndex
	WRITE(vehicleHullSurfaceIndex, std::vector<U32>); //vehicleHullSurfaceIndex
	WRITE(vehiclePolyListPlaneIndex, std::vector<U16>); //vehiclePolyListPlaneIndex
	WRITE(vehiclePolyListPointIndex, std::vector<U32>); //vehiclePolyListPointIndex
	WRITE(vehiclePolyListStringCharacter, std::vector<U8>); //vehiclePolyListStringCharacter
	WRITE(vehicleNullSurface, std::vector<VehicleNullSurface>); //vehicleNullSurface

	return true;
}

bool VehicleConvexHull::read(std::istream &stream) {
	READTOVAR(hullStart, U32); //hullStart
	READTOVAR(hullCount, U16); //hullCount
	READTOVAR(minX, F32); //minX
	READTOVAR(maxX, F32); //maxX
	READTOVAR(minY, F32); //minY
	READTOVAR(maxY, F32); //maxY
	READTOVAR(minZ, F32); //minZ
	READTOVAR(maxZ, F32); //maxZ
	READTOVAR(surfaceStart, U32); //surfaceStart
	READTOVAR(surfaceCount, U16); //surfaceCount
	READTOVAR(planeStart, U32); //planeStart
	READTOVAR(polyListPlaneStart, U32); //polyListPlaneStart
	READTOVAR(polyListPointStart, U32); //polyListPointStart
	READTOVAR(polyListStringStart, U32); //polyListStringStart

	return true;
}

bool VehicleConvexHull::write(std::ostream &stream) const {
	WRITECHECK(hullStart, U32); //hullStart
	WRITECHECK(hullCount, U16); //hullCount
	WRITECHECK(minX, F32); //minX
	WRITECHECK(maxX, F32); //maxX
	WRITECHECK(minY, F32); //minY
	WRITECHECK(maxY, F32); //maxY
	WRITECHECK(minZ, F32); //minZ
	WRITECHECK(maxZ, F32); //maxZ
	WRITECHECK(surfaceStart, U32); //surfaceStart
	WRITECHECK(surfaceCount, U16); //surfaceCount
	WRITECHECK(planeStart, U32); //planeStart
	WRITECHECK(polyListPlaneStart, U32); //polyListPlaneStart
	WRITECHECK(polyListPointStart, U32); //polyListPointStart
	WRITECHECK(polyListStringStart, U32); //polyListStringStart

	return true;
}

bool VehicleNullSurface::read(std::istream &stream) {
	READTOVAR(windingStart, U32); //windingStart
	READTOVAR(planeIndex, U16); //planeIndex
	READTOVAR(surfaceFlags, U8); //surfaceFlags
	READTOVAR(windingCount, U32); //windingCount

	return true;
}

bool VehicleNullSurface::write(std::ostream &stream) const {
	WRITECHECK(windingStart, U32); //windingStart
	WRITECHECK(planeIndex, U16); //planeIndex
	WRITECHECK(surfaceFlags, U8); //surfaceFlags
	WRITECHECK(windingCount, U32); //windingCount

	return true;
}
