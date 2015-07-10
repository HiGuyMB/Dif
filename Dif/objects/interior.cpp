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

//Warning: This is the scary file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include "io.h"
#include "interior.h"
#include "math.h"

bool Interior::read(std::istream &stream) {
	READTOVAR(interiorFileVersion, U32); //interiorFileVersion
	READTOVAR(detailLevel, U32); //detailLevel
	READTOVAR(minPixels, U32); //minPixels
	READTOVAR(boundingBox, BoxF); //boundingBox
	READTOVAR(boundingSphere, SphereF); //boundingSphere
	READTOVAR(hasAlarmState, U8); //hasAlarmState
	READTOVAR(numLightStateEntries, U32); //numLightStateEntries
	READLISTVAR(numNormals, normal, Point3F);
	READLISTVAR(numPlanes, plane, Plane);
	READLISTVAR(numPoints, point, Point3F);
	if (this->interiorFileVersion == 4) { //They exist in 0, 2, 3 but not 4
		//Probably defaulted to FF but uncertain
		numPointVisibilities = 0;
	} else {
		READLISTVAR(numPointVisibilities, pointVisibility, U8);
	}
	READLISTVAR(numTexGenEqs, texGenEq, TexGenEq);
	READLOOPVAR(numBSPNodes, BSPNode, ::BSPNode) {
		READTOVAR(BSPNode[i].planeIndex, U16); //planeIndex
		if (this->interiorFileVersion >= 14) {
			U32 tmpFront, tmpBack;
			READTOVAR(tmpFront, U32); //frontIndex
			READTOVAR(tmpBack, U32); //backIndex

			//Fuckers
			if ((tmpFront & 0x80000) != 0) {
				tmpFront = (tmpFront & ~0x80000) | 0x8000;
			}
			if ((tmpFront & 0x40000) != 0) {
				tmpFront = (tmpFront & ~0x40000) | 0x4000;
			}
			if ((tmpBack & 0x80000) != 0) {
				tmpBack = (tmpBack & ~0x80000) | 0x8000;
			}
			if ((tmpBack & 0x40000) != 0) {
				tmpBack = (tmpBack & ~0x40000) | 0x4000;
			}

			BSPNode[i].frontIndex = tmpFront;
			BSPNode[i].backIndex = tmpBack;
		} else {
			READTOVAR(BSPNode[i].frontIndex, U16); //frontIndex
			READTOVAR(BSPNode[i].backIndex, U16); //backIndex
		}
	}
	READLISTVAR(numBSPSolidLeaves, BSPSolidLeaf, ::BSPSolidLeaf);
	//MaterialList
	READTOVAR(materialListVersion, U8); //version
	READLISTVAR(numMaterials, materialName, String);
	READLISTVAR2(numWindings, index, readnumWindings2, U32, U16);
	READLISTVAR(numWindingIndices, windingIndex, WindingIndex);
	if (this->interiorFileVersion >= 12) {
		READLISTVAR(numEdges, edge, Edge);
	} else {
		numEdges = 0;
	}
	READLOOPVAR(numZones, zone, Zone) {
		READTOVAR(zone[i].portalStart, U16); //portalStart
		READTOVAR(zone[i].portalCount, U16); //portalCount
		READTOVAR(zone[i].surfaceStart, U32); //surfaceStart
		READTOVAR(zone[i].surfaceCount, U32); //surfaceCount
		if (this->interiorFileVersion >= 12) {
			READTOVAR(zone[i].staticMeshStart, U32); //staticMeshStart
			READTOVAR(zone[i].staticMeshCount, U32); //staticMeshCount
//			READTOVAR(zone[i].flags, U16); //flags
		} else {
			zone[i].staticMeshStart = 0;
			zone[i].staticMeshCount = 0;
			zone[i].flags = 0;
		}
	}
	READLISTVAR2(numZoneSurfaces, zoneSurface, 0, U16, U16);
	if (this->interiorFileVersion >= 12) {
		READLISTVAR(numZoneStaticMeshes, zoneStaticMesh, U32);
	} else {
		numZoneStaticMeshes = 0;
	}
	READLISTVAR2(numZonePortalList, zonePortalList, 0, U16, U16);
	READLISTVAR(numPortals, portal, Portal);

	//Ok so Torque needs to fuck themselves in the ass, multiple times.
	// They have two "version 0"s, one for TGE and one for TGEA. So, you
	// might ask, how do they tell which is which? I'll tell you: They
	// read the surfaces, and if anything is wrong, they fall back on the
	// old format. So guess what I get to do? Yep! That!

	//Save the file position as we'll need to rewind if any reads fail
	std::istream::pos_type pos = stream.tellg();

	bool isTGEInterior = false;

	READLOOPVAR(numSurfaces, surface, Surface) {
		if (!readSurface(stream, &surface[i], false)) {
			isTGEInterior = true;
			break;
		}
	}
	if (isTGEInterior) {
		if (interiorFileVersion != 0) {
			//Oh fuck oh fuck, TGE interiors only have version 0
			//This means that readSurface failed on a TGEA interior

			//Bail
			return false;
		}
		//Ok so we failed reading, it's *probably* a TGE interior. Let's try
		// to read it as a TGE interior.

		//First, rewind
		stream.seekg(pos);

		//Second, clean up
		numSurfaces = 0;
		delete [] surface;

		//Third, re-read
		READLOOPVAR(numSurfaces, surface, Surface) {
			if (!readSurface(stream, &surface[i], true)) {
				//Ok this surface failed too. Bail.
				//TODO: Blow up here
				return false;
			}
		}
	}

	if (this->interiorFileVersion >= 2 && this->interiorFileVersion <= 4) {
		//Extra data that I've seen in MBU interiors (v2, 3, 4)
		READLOOP(numIndicesOfSomeSort) {
			//Potentially brush data for constructor... I don't know

			//I really don't know what these are, only their size
			READ(U32);
			READ(U32);
			READ(U32);
			READ(U32);

			//Two extra that are missing in v2
			if (this->interiorFileVersion >= 3) {
				READ(U32);
				READ(U32);
			}
		}
		//v4 has some extra points and indices, no clue what these are either
		if (this->interiorFileVersion == 4) {
			//May be brush points, normals, no clue
			READLIST(numPointsOfSomeKind, Point3F); //Not sure, normals of some sort
			//Looks like indcies of some sort, can't seem to make them out though

			//Unlike anywhere else, these actually take the param into account.
			// If it's read2 and param == 0, then they use U8s, if param == 1, they use U16s
			// Not really sure why, haven't seen this anywhere else.
			READLIST2(numSomethingElses, (readnumSomethingElses2 && readnumSomethingElsesparam == 0), U16, U8);
		}
	}
	if (this->interiorFileVersion == 4) { //Found in 0, 2, 3, and TGE (14)
		READLISTVAR(numNormalLMapIndices, normalLMapIndex, U8);
		numAlarmLMapIndices = 0;
	} else if (this->interiorFileVersion >= 13) {
		//These are 32-bit values in v13 and up
		READLOOPVAR(numNormalLMapIndices, normalLMapIndex, U8) {
			READTOVAR(normalLMapIndex[i], U32);
		}
		READLOOPVAR(numAlarmLMapIndices, alarmLMapIndex, U8) {
			READTOVAR(alarmLMapIndex[i], U32);
		}
	} else {
		//Normally they're just 8
		READLISTVAR(numNormalLMapIndices, normalLMapIndex, U8);
		READLISTVAR(numAlarmLMapIndices, alarmLMapIndex, U8);
	}
	READLOOPVAR(numNullSurfaces, nullSurface, NullSurface) {
		READTOVAR(nullSurface[i].windingStart, U32); //windingStart
		READTOVAR(nullSurface[i].planeIndex, U16); //planeIndex
		READTOVAR(nullSurface[i].surfaceFlags, U8); //surfaceFlags
		if (this->interiorFileVersion >= 13) {
			READTOVAR(nullSurface[i].windingCount, U32); //windingCount
		} else {
			READTOVAR(nullSurface[i].windingCount, U8); //windingCount
		}
	}
	if (this->interiorFileVersion == 4) { //Also found in 0, 2, 3, 14
		numLightMaps = 0;
	} else {
		READLOOPVAR(numLightMaps, lightMap, LightMap) {
			READTOVAR(lightMap[i].lightMap, PNG); //lightMap
			if (!isTGEInterior) {
				//These aren't even used in the real game!
				READTOVAR(lightMap[i].lightDirMap, PNG); //lightDirMap
			}
			READTOVAR(lightMap[i].keepLightMap, U8); //keepLightMap
		}
	}
	READLISTVAR2(numSolidLeafSurfaces, solidLeafSurface, (readnumSolidLeafSurfaces2), U32, U16);
	READLISTVAR(numAnimatedLights, animatedLight, AnimatedLight);
	READLISTVAR(numLightStates, lightState, LightState);
	if (this->interiorFileVersion == 4) { //Yet more things found in 0, 2, 3, 14
		numStateDatas = 0;
		numStateDataBuffers = 0;
		flags = 0;
		numNameBuffers = 0;
		numSubObjects = 0;
	} else {
		READLISTVAR(numStateDatas, stateData, StateData);
		READLISTVAR(numStateDataBuffers, stateDataBuffer, U8);
		READTOVAR(flags, U32); //flags
		READLISTVAR(numNameBuffers, nameBufferCharacter, U8);

		READLOOP(numSubObjects) {
			//NFC
		}
	}
	READLOOPVAR(numConvexHulls, convexHull, ConvexHull) {
		READTOVAR(convexHull[i].hullStart, U32); //hullStart
		READTOVAR(convexHull[i].hullCount, U16); //hullCount
		READTOVAR(convexHull[i].minX, F32); //minX
		READTOVAR(convexHull[i].maxX, F32); //maxX
		READTOVAR(convexHull[i].minY, F32); //minY
		READTOVAR(convexHull[i].maxY, F32); //maxY
		READTOVAR(convexHull[i].minZ, F32); //minZ
		READTOVAR(convexHull[i].maxZ, F32); //maxZ
		READTOVAR(convexHull[i].surfaceStart, U32); //surfaceStart
		READTOVAR(convexHull[i].surfaceCount, U16); //surfaceCount
		READTOVAR(convexHull[i].planeStart, U32); //planeStart
		READTOVAR(convexHull[i].polyListPlaneStart, U32); //polyListPlaneStart
		READTOVAR(convexHull[i].polyListPointStart, U32); //polyListPointStart
		READTOVAR(convexHull[i].polyListStringStart, U32); //polyListStringStart

		if (this->interiorFileVersion >= 12) {
			READTOVAR(convexHull[i].staticMesh, U8); //staticMesh
		} else {
			convexHull[i].staticMesh = 0;
		}
	}
	READLISTVAR(numConvexHullEmitStrings, convexHullEmitStringCharacter, U8);

	//-------------------------------------------------------------------------
	// Lots of index lists here that have U16 or U32 versions based on loop2.
	// The actual bytes of the interior have 0x80s at the ends (negative bit)
	// which seems to specify that these take a smaller type. They managed to
	// save ~50KB/interior, but was it worth the pain?
	//
	// Also fun fact: the U16 lists have literally no reason for the 0x80, as
	// they're already using U16s. However, GG still puts them in. What the
	// fuck, GarageGames?
	//-------------------------------------------------------------------------

	READLISTVAR2(numHullIndices, hullIndex, (readnumHullIndices2), U32, U16);
	READLISTVAR2(numHullPlaneIndices, hullPlaneIndex, 0, U16, U16);
	READLISTVAR2(numHullEmitStringIndices, hullEmitStringIndex, (readnumHullEmitStringIndices2), U32, U16);
	READLISTVAR2(numHullSurfaceIndices, hullSurfaceIndex, (readnumHullSurfaceIndices2), U32, U16);
	READLISTVAR2(numPolyListPlanes, polyListPlaneIndex, 0, U16, U16);
	READLISTVAR2(numPolyListPoints, polyListPointIndex, (readnumPolyListPoints2), U32, U16);
	//Not sure if this should be a READLISTVAR2, but I haven't seen any evidence
	// of needing that for U8 lists.
	READLISTVAR(numPolyListStrings, polyListStringCharacter, U8);

	coordBin = new CoordBin[gNumCoordBins * gNumCoordBins];
	for (U32 i = 0; i < gNumCoordBins * gNumCoordBins; i ++) {
		READTOVAR(coordBin[i].binStart, U32); //binStart
		READTOVAR(coordBin[i].binCount, U32); //binCount
	}

	READLISTVAR2(numCoordBinIndices, coordBinIndex, 0, U16, U16);
	READTOVAR(coordBinMode, U32); //coordBinMode
	if (this->interiorFileVersion == 4) { //All of this is missing in v4 as well. Saves no space.
		baseAmbientColor = ColorI(0, 0, 0, 255);
		alarmAmbientColor = ColorI(0, 0, 0, 255);
		numTexNormals = 0;
		numTexMatrices = 0;
		numTexMatIndices = 0;
		extendedLightMapData = 0;
		lightMapBorderSize = 0;
	} else {
		READTOVAR(baseAmbientColor, ColorI); //baseAmbientColor
		READTOVAR(alarmAmbientColor, ColorI); //alarmAmbientColor

		if (this->interiorFileVersion >= 10) {
			READLOOPVAR(numStaticMeshes, staticMesh, StaticMesh *) {
				staticMesh[i] = new StaticMesh(stream);
			}
		}
		if (this->interiorFileVersion >= 11) {
			READLISTVAR(numTexNormals, texNormal, Point3F);
			READLISTVAR(numTexMatrices, texMatrix, TexMatrix);
			READLISTVAR(numTexMatIndices, texMatIndex, U32);
		} else {
			READTOVAR(numTexNormals, U32);
			READTOVAR(numTexMatrices, U32);
			READTOVAR(numTexMatIndices, U32);
		}
		READTOVAR(extendedLightMapData, U32);
		if (extendedLightMapData) { //extendedLightMapData
			READTOVAR(lightMapBorderSize, U32); //lightMapBorderSize
			READ(U32); //dummy
		} else {
			lightMapBorderSize = 0;
		}
	}

	return true;
}

bool Interior::write(std::ostream &stream) const {
	//We can only write version 0 maps at the moment.
	WRITECHECK(0, U32); //interiorFileVersion
	WRITECHECK(detailLevel, U32); //detailLevel
	WRITECHECK(minPixels, U32); //minPixels
	WRITECHECK(boundingBox, BoxF); //boundingBox
	WRITECHECK(boundingSphere, SphereF); //boundingSphere
	WRITECHECK(hasAlarmState, U8); //hasAlarmState
	WRITECHECK(numLightStateEntries, U32); //numLightStateEntries
	WRITELIST(numNormals, normal, Point3F); //normal
	WRITELIST(numPlanes, plane, Plane); //numPlanes
	WRITELIST(numPoints, point, Point3F); //point
	WRITELIST(numPointVisibilities, pointVisibility, U8); //pointVisibility
	WRITELIST(numTexGenEqs, texGenEq, TexGenEq); //texGenEq
	WRITELIST(numBSPNodes, BSPNode, ::BSPNode); //BSPNode
	WRITELIST(numBSPSolidLeaves, BSPSolidLeaf, ::BSPSolidLeaf); //BSPSolidLeaf
	WRITECHECK(materialListVersion, U8); //materialListVersion
	WRITELIST(numMaterials, materialName, String); //material
	WRITELIST(numWindings, index, U32); //index
	WRITELIST(numWindingIndices, windingIndex, WindingIndex); //windingIndex
	WRITELIST(numZones, zone, Zone); //zone
	WRITELIST(numZoneSurfaces, zoneSurface, U16); //zoneSurface
	WRITELIST(numZonePortalList, zonePortalList, U16); //zonePortalList
	WRITELIST(numPortals, portal, Portal); //portal
	WRITELIST(numSurfaces, surface, Surface); //surface
	WRITELIST(numNormalLMapIndices, normalLMapIndex, U8); //normalLMapIndex
	WRITELIST(numAlarmLMapIndices, alarmLMapIndex, U8); //alarmLMapIndex
	WRITELIST(numNullSurfaces, nullSurface, NullSurface); //nullSurface
	WRITELIST(numLightMaps, lightMap, LightMap); //lightMap
	WRITELIST(numSolidLeafSurfaces, solidLeafSurface, U32); //solidLeafSurface
	WRITELIST(numAnimatedLights, animatedLight, AnimatedLight); //animatedLight
	WRITELIST(numLightStates, lightState, LightState); //lightState
	WRITELIST(numStateDatas, stateData, StateData); //stateData
	WRITELIST(numStateDataBuffers, stateDataBuffer, U32); //stateDataBuffer
	WRITECHECK(flags, U32); //flags
	WRITELIST(numNameBuffers, nameBufferCharacter, S8); //nameBufferCharacter
	WRITELOOP(numSubObjects) {} //numSubObjects
	WRITELIST(numConvexHulls, convexHull, ConvexHull); //convexHull
	WRITELIST(numConvexHullEmitStrings, convexHullEmitStringCharacter, U8); //convexHullEmitStringCharacter
	WRITELIST(numHullIndices, hullIndex, U32); //hullIndex
	WRITELIST(numHullPlaneIndices, hullPlaneIndex, U16); //hullPlaneIndex
	WRITELIST(numHullEmitStringIndices, hullEmitStringIndex, U32); //hullEmitStringIndex
	WRITELIST(numHullSurfaceIndices, hullSurfaceIndex, U32); //hullSurfaceIndex
	WRITELIST(numPolyListPlanes, polyListPlaneIndex, U16); //polyListPlaneIndex
	WRITELIST(numPolyListPoints, polyListPointIndex, U32); //polyListPointIndex
	WRITELIST(numPolyListStrings, polyListStringCharacter, U8); //polyListStringCharacter
	for (U32 i = 0; i < gNumCoordBins * gNumCoordBins; i ++) {
		WRITECHECK(coordBin[i].binStart, U32); //binStart
		WRITECHECK(coordBin[i].binCount, U32); //binCount
	}
	WRITELIST(numCoordBinIndices, coordBinIndex, U16); //coordBinIndex
	WRITECHECK(coordBinMode, U32); //coordBinMode
	WRITECHECK(baseAmbientColor, ColorI); //baseAmbientColor
	WRITECHECK(alarmAmbientColor, ColorI); //alarmAmbientColor
	/*
	 Static meshes (not included)
	 */
	WRITELIST(numTexNormals, texNormal, Point3F); //texNormal
	WRITELIST(numTexMatrices, texMatrix, TexMatrix); //texMatrix
	WRITELIST(numTexMatIndices, texMatIndex, U32); //texMatIndex
	WRITECHECK(0, U32); //extendedLightMapData
//	WRITECHECK(extendedLightMapData, U32); //extendedLightMapData

	return true;
}

Interior::~Interior() {
	delete [] normal;
	delete [] plane;
	delete [] point;
	delete [] pointVisibility;
	delete [] texGenEq;
	delete [] BSPNode;
	delete [] BSPSolidLeaf;
	delete [] materialName;
	delete [] index;
	delete [] windingIndex;
	delete [] zone;
	delete [] zoneSurface;
	delete [] zonePortalList;
	delete [] portal;
	delete [] surface;
	delete [] normalLMapIndex;
	delete [] alarmLMapIndex;
	delete [] nullSurface;
	delete [] lightMap;
	delete [] solidLeafSurface;
	delete [] animatedLight;
	delete [] lightState;
	delete [] stateData;
	delete [] stateDataBuffer;
	delete [] nameBufferCharacter;
	delete [] convexHull;
	delete [] convexHullEmitStringCharacter;
	delete [] hullIndex;
	delete [] hullPlaneIndex;
	delete [] hullEmitStringIndex;
	delete [] hullSurfaceIndex;
	delete [] polyListPlaneIndex;
	delete [] polyListPointIndex;
	delete [] polyListStringCharacter;
	delete [] coordBin;
	delete [] coordBinIndex;
	delete [] texNormal;
	delete [] texMatrix;
	delete [] texMatIndex;
	for (int i = 0; i < numStaticMeshes; i ++) {
		delete staticMesh[i];
	}
	delete [] staticMesh;
}

//----------------------------------------------------------------------------

bool Interior::readSurface(std::istream &stream, Surface *surface, bool isTGEInterior) {
	READTOVAR(surface->windingStart, U32); //windingStart
	if (this->interiorFileVersion >= 13) {
		READTOVAR(surface->windingCount, U32); //windingCount
	} else {
		READTOVAR(surface->windingCount, U8); //windingCount
	}
	if (surface->windingStart + surface->windingCount > numWindings)
		return false;

	//Fucking GarageGames. Sometimes the plane is | 0x8000 because WHY NOT
	READVAR(plane, S16); //planeIndex
	//Ugly hack
	surface->planeFlipped = (plane >> 15 != 0);
	plane &= ~0x8000;
	surface->planeIndex = plane;
	if (surface->planeIndex > numPlanes)
		return false;

	READTOVAR(surface->textureIndex, U16); //textureIndex
	if (surface->textureIndex > numMaterials)
		return false;

	READTOVAR(surface->texGenIndex, U32); //texGenIndex
	if (surface->texGenIndex > numTexGenEqs)
		return false;

	READTOVAR(surface->surfaceFlags, U8); //surfaceFlags
	READTOVAR(surface->fanMask, U32); //fanMask
	{ //LightMap
		READTOVAR(surface->lightMap.finalWord, U16); //finalWord
		READTOVAR(surface->lightMap.texGenXDistance, F32); //texGenXDistance
		READTOVAR(surface->lightMap.texGenYDistance, F32); //texGenYDistance
	}
	READTOVAR(surface->lightCount, U16); //lightCount
	READTOVAR(surface->lightStateInfoStart, U32); //lightStateInfoStart

	if (this->interiorFileVersion >= 13) {
		READTOVAR(surface->mapOffsetX, U32); //mapOffsetX
		READTOVAR(surface->mapOffsetY, U32); //mapOffsetY
		READTOVAR(surface->mapSizeX, U32); //mapSizeX
		READTOVAR(surface->mapSizeY, U32); //mapSizeY
	} else {
		READTOVAR(surface->mapOffsetX, U8); //mapOffsetX
		READTOVAR(surface->mapOffsetY, U8); //mapOffsetY
		READTOVAR(surface->mapSizeX, U8); //mapSizeX
		READTOVAR(surface->mapSizeY, U8); //mapSizeY
	}

	if (!isTGEInterior) {
		READ(U8); //unused
		if (this->interiorFileVersion > 0 && this->interiorFileVersion <= 4) {
			READ(U32); //Extra bytes used for some unknown purpose, seen in versions 2, 3, 4
		}
	}
	return true;
}

//----------------------------------------------------------------------------

bool Plane::read(std::istream &stream) {
	READTOVAR(normalIndex, U16); //normalIndex
	READTOVAR(planeDistance, F32); //planeDistance
	return true;
}

bool Plane::write(std::ostream &stream) const {
	WRITECHECK(normalIndex, U16); //normalIndex
	WRITECHECK(planeDistance, F32); //planeDistance
	return true;
}

bool TexGenEq::read(std::istream &stream) {
	READTOVAR(planeX, PlaneF); //planeX
	READTOVAR(planeY, PlaneF); //planeY
	return true;
}

bool TexGenEq::write(std::ostream &stream) const {
	WRITECHECK(planeX, PlaneF); //planeX
	WRITECHECK(planeY, PlaneF); //planeY
	return true;
}

bool BSPNode::write(std::ostream &stream) const {
	WRITECHECK(planeIndex, U16); //planeIndex
	WRITECHECK(frontIndex, U16); //frontIndex
	WRITECHECK(backIndex, U16); //backIndex
	return true;
}

bool BSPSolidLeaf::read(std::istream &stream) {
	READTOVAR(surfaceIndex, U32); //surfaceIndex
	READTOVAR(surfaceCount, U16); //surfaceCount
	return true;
}

bool BSPSolidLeaf::write(std::ostream &stream) const {
	WRITECHECK(surfaceIndex, U32); //surfaceIndex
	WRITECHECK(surfaceCount, U16); //surfaceCount
	return true;
}

bool WindingIndex::read(std::istream &stream) {
	READTOVAR(windingStart, U32); //windingStart
	READTOVAR(windingCount, U32); //windingCount
	return true;
}

bool WindingIndex::write(std::ostream &stream) const {
	WRITECHECK(windingStart, U32); //windingStart
	WRITECHECK(windingCount, U32); //windingCount
	return true;
}

bool Zone::write(std::ostream &stream) const {
	WRITECHECK(portalStart, U16); //portalStart
	WRITECHECK(portalCount, U16); //portalCount
	WRITECHECK(surfaceStart, U32); //surfaceStart
	WRITECHECK(surfaceCount, U32); //surfaceCount
	return true;
}

bool Edge::read(std::istream &stream) {
	READTOVAR(pointIndex0, S32); //pointIndex0
	READTOVAR(pointIndex1, S32); //pointIndex1
	READTOVAR(surfaceIndex0, S32); //surfaceIndex0
	READTOVAR(surfaceIndex1, S32); //surfaceIndex1
	return true;
}

bool Edge::write(std::ostream &stream) const {
	WRITECHECK(pointIndex0, S32); //pointIndex0
	WRITECHECK(pointIndex1, S32); //pointIndex1
	WRITECHECK(surfaceIndex0, S32); //surfaceIndex0
	WRITECHECK(surfaceIndex1, S32); //surfaceIndex1
	return true;
}

bool Portal::read(std::istream &stream) {
	READTOVAR(planeIndex, U16); //planeIndex
	READTOVAR(triFanCount, U16); //triFanCount
	READTOVAR(triFanStart, U32); //triFanStart
	READTOVAR(zoneFront, U16); //zoneFront
	READTOVAR(zoneBack, U16); //zoneBack
	return true;
}

bool Portal::write(std::ostream &stream) const {
	WRITECHECK(planeIndex, U16); //planeIndex
	WRITECHECK(triFanCount, U16); //triFanCount
	WRITECHECK(triFanStart, U32); //triFanStart
	WRITECHECK(zoneFront, U16); //zoneFront
	WRITECHECK(zoneBack, U16); //zoneBack
	return true;
}

bool Surface::write(std::ostream &stream) const {
	WRITECHECK(windingStart, U32); //windingStart
	WRITECHECK(windingCount, U8); //windingCount
	U16 index = planeIndex;
	if (planeFlipped)
		index |= 0x8000;
	WRITECHECK(index, U16); //planeIndex
	WRITECHECK(textureIndex, U16); //textureIndex
	WRITECHECK(texGenIndex, U32); //texGenIndex
	WRITECHECK(surfaceFlags, U8); //surfaceFlags
	WRITECHECK(fanMask, U32); //fanMask
	WRITECHECK(lightMap.finalWord, U16); //finalWord
	WRITECHECK(lightMap.texGenXDistance, F32); //texGenXDistance
	WRITECHECK(lightMap.texGenYDistance, F32); //texGenYDistance
	WRITECHECK(lightCount, U16); //lightCount
	WRITECHECK(lightStateInfoStart, U32); //lightStateInfoStart
	WRITECHECK(mapOffsetX, U8); //mapOffsetX
	WRITECHECK(mapOffsetY, U8); //mapOffsetY
	WRITECHECK(mapSizeX, U8); //mapSizeX
	WRITECHECK(mapSizeY, U8); //mapSizeY
	return true;
}

bool NullSurface::write(std::ostream &stream) const {
	WRITECHECK(windingStart, U32); //windingStart
	WRITECHECK(planeIndex, U16); //planeIndex
	WRITECHECK(surfaceFlags, U8); //surfaceFlags
	WRITECHECK(windingCount, U8); //windingCount
	return true;
}

bool LightMap::write(std::ostream &stream) const {
	WRITE(lightMap, PNG); //lightMap
	WRITECHECK(keepLightMap, U8); //keepLightMap
	return true;
}

bool AnimatedLight::read(std::istream &stream) {
	READTOVAR(nameIndex, U32); //nameIndex
	READTOVAR(stateIndex, U32); //stateIndex
	READTOVAR(stateCount, U16); //stateCount
	READTOVAR(flags, U16); //flags
	READTOVAR(duration, U32); //duration
	return true;
}

bool AnimatedLight::write(std::ostream &stream) const {
	WRITECHECK(nameIndex, U32); //nameIndex
	WRITECHECK(stateIndex, U32); //stateIndex
	WRITECHECK(stateCount, U16); //stateCount
	WRITECHECK(flags, U16); //flags
	WRITECHECK(duration, U32); //duration
	return true;
}

bool LightState::read(std::istream &stream) {
	READTOVAR(red, U8); //red
	READTOVAR(green, U8); //green
	READTOVAR(blue, U8); //blue
	READTOVAR(activeTime, U32); //activeTime
	READTOVAR(dataIndex, U32); //dataIndex
	READTOVAR(dataCount, U16); //dataCount
	return true;
}

bool LightState::write(std::ostream &stream) const {
	WRITECHECK(red, U8); //red
	WRITECHECK(green, U8); //green
	WRITECHECK(blue, U8); //blue
	WRITECHECK(activeTime, U32); //activeTime
	WRITECHECK(dataIndex, U32); //dataIndex
	WRITECHECK(dataCount, U16); //dataCount
	return true;
}

bool StateData::read(std::istream &stream) {
	READTOVAR(surfaceIndex, U32); //surfaceIndex
	READTOVAR(mapIndex, U32); //mapIndex
	READTOVAR(lightStateIndex, U16); //lightStateIndex
	return true;
}

bool StateData::write(std::ostream &stream) const {
	WRITECHECK(surfaceIndex, U32); //surfaceIndex
	WRITECHECK(mapIndex, U32); //mapIndex
	WRITECHECK(lightStateIndex, U16); //lightStateIndex
	return true;
}

bool ConvexHull::write(std::ostream &stream) const {
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

bool TexMatrix::read(std::istream &stream) {
	READTOVAR(T, S32); //T
	READTOVAR(N, S32); //N
	READTOVAR(B, S32); //B
	return true;
}

bool TexMatrix::write(std::ostream &stream) const {
	WRITECHECK(T, S32); //T
	WRITECHECK(N, S32); //N
	WRITECHECK(B, S32); //B
	return true;
}
