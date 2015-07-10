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

#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "vehicleCollision.h"

VehicleCollision::VehicleCollision(std::istream &stream) {
	READTOVAR(vehicleCollisionFileVersion, U32); //vehicleCollisionFileVersion
	READLOOPVAR(numVehicleConvexHulls, vehicleConvexHull, VehicleConvexHull) {
		READTOVAR(vehicleConvexHull[i].hullStart, U32); //hullStart
		READTOVAR(vehicleConvexHull[i].hullCount, U16); //hullCount
		READTOVAR(vehicleConvexHull[i].minX, F32); //minX
		READTOVAR(vehicleConvexHull[i].maxX, F32); //maxX
		READTOVAR(vehicleConvexHull[i].minY, F32); //minY
		READTOVAR(vehicleConvexHull[i].maxY, F32); //maxY
		READTOVAR(vehicleConvexHull[i].minZ, F32); //minZ
		READTOVAR(vehicleConvexHull[i].maxZ, F32); //maxZ
		READTOVAR(vehicleConvexHull[i].surfaceStart, U32); //surfaceStart
		READTOVAR(vehicleConvexHull[i].surfaceCount, U16); //surfaceCount
		READTOVAR(vehicleConvexHull[i].planeStart, U32); //planeStart
		READTOVAR(vehicleConvexHull[i].polyListPlaneStart, U32); //polyListPlaneStart
		READTOVAR(vehicleConvexHull[i].polyListPointStart, U32); //polyListPointStart
		READTOVAR(vehicleConvexHull[i].polyListStringStart, U32); //polyListStringStart
	}
	READLOOPVAR(numVehicleConvexHullEmitStrings, vehicleConvexHullEmitStringCharacter, U8) {
		READTOVAR(vehicleConvexHullEmitStringCharacter[i], U8); //vehicleConvexHullEmitStringCharacter
	}
	READLOOPVAR(numVehicleHullIndices, vehicleHullIndex, U32) {
		READTOVAR(vehicleHullIndex[i], U32); //vehicleHullIndex
	}
	READLOOPVAR(numVehicleHullPlaneIndices, vehicleHullPlaneIndex, U16) {
		READTOVAR(vehicleHullPlaneIndex[i], U16); //vehicleHullPlaneIndex
	}
	READLOOPVAR(numVehicleHullEmitStringIndices, vehicleHullEmitStringIndex, U32) {
		READTOVAR(vehicleHullEmitStringIndex[i], U32); //vehicleHullEmitStringIndex
	}
	READLOOPVAR(numVehicleHullSurfaceIndices, vehicleHullSurfaceIndex, U32) {
		READTOVAR(vehicleHullSurfaceIndex[i], U32); //vehicleHullSurfaceIndex
	}
	READLOOPVAR(numVehiclePolyListPlanes, vehiclePolyListPlaneIndex, U16) {
		READTOVAR(vehiclePolyListPlaneIndex[i], U16); //vehiclePolyListPlaneIndex
	}
	READLOOPVAR(numVehiclePolyListPoints, vehiclePolyListPointIndex, U32) {
		READTOVAR(vehiclePolyListPointIndex[i], U32); //vehiclePolyListPointIndex
	}
	READLOOPVAR(numVehiclePolyListStrings, vehiclePolyListStringCharacter, U8) {
		READTOVAR(vehiclePolyListStringCharacter[i], U8); //vehiclePolyListStringCharacter
	}
	READLOOPVAR(numVehicleNullSurfaces, vehicleNullSurface, VehicleNullSurface) {
		READTOVAR(vehicleNullSurface[i].windingStart, U32); //windingStart
		READTOVAR(vehicleNullSurface[i].planeIndex, U16); //planeIndex
		READTOVAR(vehicleNullSurface[i].surfaceFlags, U8); //surfaceFlags
		READTOVAR(vehicleNullSurface[i].windingCount, U32); //windingCount
	}
}

bool VehicleCollision::write(std::ostream &stream) const {
	WRITECHECK(vehicleCollisionFileVersion, U32);
	WRITELOOP(numVehicleConvexHulls) {
		WRITECHECK(vehicleConvexHull[i].hullStart, U32); //hullStart
		WRITECHECK(vehicleConvexHull[i].hullCount, U16); //hullCount
		WRITECHECK(vehicleConvexHull[i].minX, F32); //minX
		WRITECHECK(vehicleConvexHull[i].maxX, F32); //maxX
		WRITECHECK(vehicleConvexHull[i].minY, F32); //minY
		WRITECHECK(vehicleConvexHull[i].maxY, F32); //maxY
		WRITECHECK(vehicleConvexHull[i].minZ, F32); //minZ
		WRITECHECK(vehicleConvexHull[i].maxZ, F32); //maxZ
		WRITECHECK(vehicleConvexHull[i].surfaceStart, U32); //surfaceStart
		WRITECHECK(vehicleConvexHull[i].surfaceCount, U16); //surfaceCount
		WRITECHECK(vehicleConvexHull[i].planeStart, U32); //planeStart
		WRITECHECK(vehicleConvexHull[i].polyListPlaneStart, U32); //polyListPlaneStart
		WRITECHECK(vehicleConvexHull[i].polyListPointStart, U32); //polyListPointStart
		WRITECHECK(vehicleConvexHull[i].polyListStringStart, U32); //polyListStringStart
	}
	WRITELIST(numVehicleConvexHullEmitStrings, vehicleConvexHullEmitStringCharacter, U8); //vehicleConvexHullEmitStringCharacter
	WRITELIST(numVehicleHullIndices, vehicleHullIndex, U32); //vehicleHullIndex
	WRITELIST(numVehicleHullPlaneIndices, vehicleHullPlaneIndex, U16); //vehicleHullPlaneIndex
	WRITELIST(numVehicleHullEmitStringIndices, vehicleHullEmitStringIndex, U32); //vehicleHullEmitStringIndex
	WRITELIST(numVehicleHullSurfaceIndices, vehicleHullSurfaceIndex, U32); //vehicleHullSurfaceIndex
	WRITELIST(numVehiclePolyListPlanes, vehiclePolyListPlaneIndex, U16); //vehiclePolyListPlaneIndex
	WRITELIST(numVehiclePolyListPoints, vehiclePolyListPointIndex, U32); //vehiclePolyListPointIndex
	WRITELIST(numVehiclePolyListStrings, vehiclePolyListStringCharacter, U8); //vehiclePolyListStringCharacter
	WRITELOOP(numVehicleNullSurfaces) {
		WRITECHECK(vehicleNullSurface[i].windingStart, U32); //windingStart
		WRITECHECK(vehicleNullSurface[i].planeIndex, U16); //planeIndex
		WRITECHECK(vehicleNullSurface[i].surfaceFlags, U8); //surfaceFlags
		WRITECHECK(vehicleNullSurface[i].windingCount, U32); //windingCount
	}

	return true;
}

VehicleCollision::~VehicleCollision() {
	delete vehicleConvexHull;
	delete vehicleConvexHullEmitStringCharacter;
	delete vehicleHullIndex;
	delete vehicleHullPlaneIndex;
	delete vehicleHullEmitStringIndex;
	delete vehicleHullSurfaceIndex;
	delete vehiclePolyListPlaneIndex;
	delete vehiclePolyListPointIndex;
	delete vehiclePolyListStringCharacter;
	delete vehicleNullSurface;
}
