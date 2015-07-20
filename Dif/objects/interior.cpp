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

#include <assert.h>
#include "io.h"
#include "interior.h"
#include "math.h"

DIF_NAMESPACE

bool Interior::read(std::istream &stream) {
	READCHECK(interiorFileVersion, U32); //interiorFileVersion
	READCHECK(detailLevel, U32); //detailLevel
	READCHECK(minPixels, U32); //minPixels
	READCHECK(boundingBox, BoxF); //boundingBox
	READCHECK(boundingSphere, SphereF); //boundingSphere
	READCHECK(hasAlarmState, U8); //hasAlarmState
	READCHECK(numLightStateEntries, U32); //numLightStateEntries
	READCHECK(normal, std::vector<Point3F>); //normal
	READCHECK(plane, std::vector<Plane>); //plane
	READCHECK(point, std::vector<Point3F>); //point
	if (this->interiorFileVersion == 4) { //They exist in 0, 2, 3 but not 4
		//Probably defaulted to FF but uncertain
	} else {
		READCHECK(pointVisibility, std::vector<U8>); //pointVisibility
	}
	READCHECK(texGenEq, std::vector<TexGenEq>); //texGenEq
	IO::read_with<DIF::BSPNode>(stream, BSPNode, [&](DIF::BSPNode &node, std::istream &stream)->bool{return node.read(stream, this->interiorFileVersion);}, "BSPNode"); //BSPNode
	READCHECK(BSPSolidLeaf, std::vector<DIF::BSPSolidLeaf>); //BSPSolidLeaf
	//MaterialList
	READCHECK(materialListVersion, U8); //version
	READCHECK(materialName, std::vector<std::string>); //materialName
	IO::read_as<U32, U16>(stream, index, [](bool useAlternate, U32 param)->bool{return param;}, "index"); //index
	READCHECK(windingIndex, std::vector<WindingIndex>); //windingIndex
	if (this->interiorFileVersion >= 12) {
		READCHECK(edge, std::vector<Edge>); //edge
	}
	IO::read_with<Zone>(stream, zone, [&](Zone &zone, std::istream &stream)->bool{return zone.read(stream, this->interiorFileVersion);}, "zone"); //zone
	IO::read_as<U16, U16>(stream, zoneSurface, [](bool useAlternate, U32 param)->bool{return false;}, "zoneSurface");
	if (this->interiorFileVersion >= 12) {
		READCHECK(zoneStaticMesh, std::vector<U32>); //zoneStaticMesh
	}
	IO::read_as<U16, U16>(stream, zonePortalList, [](bool useAlternate, U32 param)->bool{return false;}, "zonePortalList"); //zonePortalList
	READCHECK(portal, std::vector<Portal>); //portal

	//Ok so Torque needs to fuck themselves in the ass, multiple times.
	// They have two "version 0"s, one for TGE and one for TGEA. So, you
	// might ask, how do they tell which is which? I'll tell you: They
	// read the surfaces, and if anything is wrong, they fall back on the
	// old format. So guess what I get to do? Yep! That!

	//Save the file position as we'll need to rewind if any reads fail
	std::istream::pos_type pos = stream.tellg();

	bool isTGEInterior = false;

	if (!IO::read_with<Surface>(stream, surface, [&](Surface &surface, std::istream &stream)->bool{
		return surface.read(stream, interiorFileVersion, false, static_cast<U32>(index.size()), static_cast<U32>(plane.size()), static_cast<U32>(materialName.size()), static_cast<U32>(texGenEq.size()));
	}, "surface")) { //surface
		isTGEInterior = true;

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

		//Second clean up
		surface.clear();

		//Third, re-read
		if (!IO::read_with<Surface>(stream, surface, [&](Surface &surface, std::istream &stream)->bool{
			return surface.read(stream, interiorFileVersion, true, static_cast<U32>(index.size()), static_cast<U32>(plane.size()), static_cast<U32>(materialName.size()), static_cast<U32>(texGenEq.size()));
		}, "surface")) { //surface
			//Ok this surface failed too. Bail.
			return false;
		}
	}

	if (this->interiorFileVersion >= 2 && this->interiorFileVersion <= 4) {
		//Extra data that I've seen in MBU interiors (v2, 3, 4)
		U32 numIndicesOfSomeSort;
		READCHECK(numIndicesOfSomeSort, U32);
		for (U32 i = 0; i < numIndicesOfSomeSort; i ++) { //Some list of something
			//Potentially brush data for constructor... I don't know

			//I really don't know what these are, only their size
			READ(U32); //Unknown
			READ(U32); //Unknown
			READ(U32); //Unknown
			READ(U32); //Unknown

			//Two extra that are missing in v2
			if (this->interiorFileVersion >= 3) {
				READ(U32); //Unknown
				READ(U32); //Unknown
			}
		}
		//v4 has some extra points and indices, no clue what these are either
		if (this->interiorFileVersion == 4) {
			//May be brush points, normals, no clue
			Point3F pointOfSomeKind;
			READCHECK(pointOfSomeKind, std::vector<Point3F>); //Not sure, normals of some sort

			//Looks like indcies of some sort, can't seem to make them out though
			
			//Unlike anywhere else, these actually take the param into account.
			// If it's read2 and param == 0, then they use U8s, if param == 1, they use U16s
			// Not really sure why, haven't seen this anywhere else.

			std::vector<U16> somethingElse;
			IO::read_as<U16, U8>(stream, somethingElse, [](bool useAlternate, U32 param)->bool{return (useAlternate && param == 0);}, "somethingElse"); //somethingElse
		}
	}
	if (this->interiorFileVersion == 4) { //Found in 0, 2, 3, and TGE (14)
		READCHECK(normalLMapIndex, std::vector<U8>); //normalLMapIndex
	} else if (this->interiorFileVersion >= 13) {
		//These are 32-bit values in v13 and up
		READCHECK(normalLMapIndex, std::vector<U32>); //normalLMapIndex
		READCHECK(alarmLMapIndex, std::vector<U32>); //alarmLMapIndex
	} else {
		//Normally they're just 8
		READCHECK(normalLMapIndex, std::vector<U8>); //normalLMapIndex
		READCHECK(alarmLMapIndex, std::vector<U8>); //alarmLMapIndex
	}
	IO::read_with<NullSurface>(stream, nullSurface, [&](NullSurface &nullSurface, std::istream &stream)->bool{return nullSurface.read(stream, this->interiorFileVersion);}, "nullSurface"); //nullSurface
	if (this->interiorFileVersion != 4) { //Also found in 0, 2, 3, 14
		IO::read_with<LightMap>(stream, lightMap, [&](LightMap &lightMap, std::istream &stream)->bool{return lightMap.read(stream, isTGEInterior);}, "lightMap"); //lightMap
	}
	IO::read_as<U32, U16>(stream, solidLeafSurface, [](bool useAlternate, U32 param)->bool{return useAlternate;}, "solidLeafSurface"); //solidLeafSurface
	READCHECK(animatedLight, std::vector<AnimatedLight>); //animatedLight
	READCHECK(lightState, std::vector<LightState>); //lightState
	if (this->interiorFileVersion == 4) { //Yet more things found in 0, 2, 3, 14
		flags = 0;
		numSubObjects = 0;
	} else {
		READCHECK(stateData, std::vector<StateData>); //stateData

		//State datas have the flags field written right after the vector size,
		// and THEN the data, just to make things confusing. So we need yet another
		// read method for this.
		IO::read_extra(stream, stateDataBuffer, [&](std::istream &stream)->bool{
			return IO::read(stream, flags, "flags"); //flags
		}, "stateDataBuffer"); //stateDataBuffer
		READCHECK(nameBufferCharacter, std::vector<U8>); //nameBufferCharacter

		READCHECK(numSubObjects, U32); //numSubObjects
		assert(numSubObjects == 0); //Can't support these currently
//		READLOOP(numSubObjects) {
//			//NFC
//		}
	}
	IO::read_with<ConvexHull>(stream, convexHull, [&](ConvexHull &convexHull, std::istream &stream)->bool{return convexHull.read(stream, this->interiorFileVersion);}, "convexHull"); //convexHull
	READCHECK(convexHullEmitStringCharacter, std::vector<U8>); //convexHullEmitStringCharacter

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

	IO::read_as<U32, U16>(stream, hullIndex, [](bool useAlternate, U32 param)->bool{return useAlternate;}, "hullIndex"); //hullIndex
	IO::read_as<U16, U16>(stream, hullPlaneIndex, [](bool useAlternate, U32 param)->bool{return true;}, "hullPlaneIndex"); //hullPlaneIndex
	IO::read_as<U32, U16>(stream, hullEmitStringIndex, [](bool useAlternate, U32 param)->bool{return useAlternate;}, "hullEmitStringIndex"); //hullEmitStringIndex
	IO::read_as<U32, U16>(stream, hullSurfaceIndex, [](bool useAlternate, U32 param)->bool{return useAlternate;}, "hullSurfaceIndex"); //hullSurfaceIndex
	IO::read_as<U16, U16>(stream, polyListPlaneIndex, [](bool useAlternate, U32 param)->bool{return true;}, "polyListPlaneIndex"); //polyListPlaneIndex
	IO::read_as<U32, U16>(stream, polyListPointIndex, [](bool useAlternate, U32 param)->bool{return useAlternate;}, "polyListPointIndex"); //polyListPointIndex
	//Not sure if this should be a read_as, but I haven't seen any evidence
	// of needing that for U8 lists.
	READCHECK(polyListStringCharacter, std::vector<U8>); //polyListStringCharacter

	coordBin.reserve(gNumCoordBins * gNumCoordBins);
	for (U32 i = 0; i < gNumCoordBins * gNumCoordBins; i ++) {
		CoordBin bin;
		if (IO::read(stream, bin, "coordBin")) //coordBin
			coordBin.push_back(bin);
		else
			return false;
	}

	IO::read_as<U16, U16>(stream, coordBinIndex, [](bool useAlternate, U32 param)->bool{return true;}, "coordBinIndex"); //coordBinIndex
	READCHECK(coordBinMode, U32); //coordBinMode
	if (this->interiorFileVersion == 4) { //All of this is missing in v4 as well. Saves no space.
		baseAmbientColor = ColorI(0, 0, 0, 255);
		alarmAmbientColor = ColorI(0, 0, 0, 255);
		extendedLightMapData = 0;
		lightMapBorderSize = 0;
	} else {
		READCHECK(baseAmbientColor, ColorI); //baseAmbientColor
		READCHECK(alarmAmbientColor, ColorI); //alarmAmbientColor

		if (this->interiorFileVersion >= 10) {
			READCHECK(staticMesh, std::vector<StaticMesh>); //staticMesh
		}
		if (this->interiorFileVersion >= 11) {
			READCHECK(texNormal, std::vector<Point3F>); //texNormal
			READCHECK(texMatrix, std::vector<TexMatrix>); //texMatrix
			READCHECK(texMatIndex, std::vector<U32>); //texMatIndex
		} else {
			READ(U32); //numTexNormals
			READ(U32); //numTexMatrices
			READ(U32); //numTexMatIndices
		}
		READCHECK(extendedLightMapData, U32); //extendedLightMapData
		if (extendedLightMapData) {
			READCHECK(lightMapBorderSize, U32); //lightMapBorderSize
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
	WRITECHECK(normal, std::vector<Point3F>); //normal
	WRITECHECK(plane, std::vector<Plane>); //numPlanes
	WRITECHECK(point, std::vector<Point3F>); //point
	WRITECHECK(pointVisibility, std::vector<U8>); //pointVisibility
	WRITECHECK(texGenEq, std::vector<TexGenEq>); //texGenEq
	WRITECHECK(BSPNode, std::vector<DIF::BSPNode>); //BSPNode
	WRITECHECK(BSPSolidLeaf, std::vector<DIF::BSPSolidLeaf>); //BSPSolidLeaf
	WRITECHECK(materialListVersion, U8); //materialListVersion
	WRITECHECK(materialName, std::vector<std::string>); //material
	WRITECHECK(index, std::vector<U32>); //index
	WRITECHECK(windingIndex, std::vector<WindingIndex>); //windingIndex
	WRITECHECK(zone, std::vector<Zone>); //zone
	WRITECHECK(zoneSurface, std::vector<U16>); //zoneSurface
	WRITECHECK(zonePortalList, std::vector<U16>); //zonePortalList
	WRITECHECK(portal, std::vector<Portal>); //portal
	WRITECHECK(surface, std::vector<Surface>); //surface
	WRITECHECK(normalLMapIndex, std::vector<U8>); //normalLMapIndex
	WRITECHECK(alarmLMapIndex, std::vector<U8>); //alarmLMapIndex
	WRITECHECK(nullSurface, std::vector<NullSurface>); //nullSurface
	WRITECHECK(lightMap, std::vector<LightMap>); //lightMap
	WRITECHECK(solidLeafSurface, std::vector<U32>); //solidLeafSurface
	WRITECHECK(animatedLight, std::vector<AnimatedLight>); //animatedLight
	WRITECHECK(lightState, std::vector<LightState>); //lightState
	WRITECHECK(stateData, std::vector<StateData>); //stateData
	IO::write_extra(stream, stateDataBuffer, [&](std::ostream &stream)->bool {
		return IO::write(stream, flags, "flags"); //flags
	}, "stateDataBuffer"); //stateDataBuffer
	WRITECHECK(nameBufferCharacter, std::vector<U8>); //nameBufferCharacter
	WRITECHECK(numSubObjects, U32); //numSubObjects
//	WRITELOOP(numSubObjects) {} //subObject
	WRITECHECK(convexHull, std::vector<ConvexHull>); //convexHull
	WRITECHECK(convexHullEmitStringCharacter, std::vector<U8>); //convexHullEmitStringCharacter
	WRITECHECK(hullIndex, std::vector<U32>); //hullIndex
	WRITECHECK(hullPlaneIndex, std::vector<U16>); //hullPlaneIndex
	WRITECHECK(hullEmitStringIndex, std::vector<U32>); //hullEmitStringIndex
	WRITECHECK(hullSurfaceIndex, std::vector<U32>); //hullSurfaceIndex
	WRITECHECK(polyListPlaneIndex, std::vector<U16>); //polyListPlaneIndex
	WRITECHECK(polyListPointIndex, std::vector<U32>); //polyListPointIndex
	WRITECHECK(polyListStringCharacter, std::vector<U8>); //polyListStringCharacter
	for (U32 i = 0; i < gNumCoordBins * gNumCoordBins; i ++) {
		WRITECHECK(coordBin[i], CoordBin); //coordBin
	}
	WRITECHECK(coordBinIndex, std::vector<U16>); //coordBinIndex
	WRITECHECK(coordBinMode, U32); //coordBinMode
	WRITECHECK(baseAmbientColor, ColorI); //baseAmbientColor
	WRITECHECK(alarmAmbientColor, ColorI); //alarmAmbientColor
	/*
	 Static meshes (not included)
	 */
	WRITECHECK(texNormal, std::vector<Point3F>); //texNormal
	WRITECHECK(texMatrix, std::vector<TexMatrix>); //texMatrix
	WRITECHECK(texMatIndex, std::vector<U32>); //texMatIndex
	WRITECHECK(0, U32); //extendedLightMapData
//	WRITECHECK(extendedLightMapData, U32); //extendedLightMapData

	return true;
}

//----------------------------------------------------------------------------

bool Plane::read(std::istream &stream) {
	READCHECK(normalIndex, U16); //normalIndex
	READCHECK(planeDistance, F32); //planeDistance
	return true;
}

bool Plane::write(std::ostream &stream) const {
	WRITECHECK(normalIndex, U16); //normalIndex
	WRITECHECK(planeDistance, F32); //planeDistance
	return true;
}

bool TexGenEq::read(std::istream &stream) {
	READCHECK(planeX, PlaneF); //planeX
	READCHECK(planeY, PlaneF); //planeY
	return true;
}

bool TexGenEq::write(std::ostream &stream) const {
	WRITECHECK(planeX, PlaneF); //planeX
	WRITECHECK(planeY, PlaneF); //planeY
	return true;
}

bool BSPNode::read(std::istream &stream, U32 interiorFileVersion) {
	READCHECK(planeIndex, U16); //planeIndex
	if (interiorFileVersion >= 14) {
		U32 tmpFront, tmpBack;
		READCHECK(tmpFront, U32); //frontIndex
		READCHECK(tmpBack, U32); //backIndex

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

		frontIndex = tmpFront;
		backIndex = tmpBack;
	} else {
		READCHECK(frontIndex, U16); //frontIndex
		READCHECK(backIndex, U16); //backIndex
	}
	return true;
}

bool BSPNode::write(std::ostream &stream) const {
	WRITECHECK(planeIndex, U16); //planeIndex
	WRITECHECK(frontIndex, U16); //frontIndex
	WRITECHECK(backIndex, U16); //backIndex
	return true;
}

bool BSPSolidLeaf::read(std::istream &stream) {
	READCHECK(surfaceIndex, U32); //surfaceIndex
	READCHECK(surfaceCount, U16); //surfaceCount
	return true;
}

bool BSPSolidLeaf::write(std::ostream &stream) const {
	WRITECHECK(surfaceIndex, U32); //surfaceIndex
	WRITECHECK(surfaceCount, U16); //surfaceCount
	return true;
}

bool WindingIndex::read(std::istream &stream) {
	READCHECK(windingStart, U32); //windingStart
	READCHECK(windingCount, U32); //windingCount
	return true;
}

bool WindingIndex::write(std::ostream &stream) const {
	WRITECHECK(windingStart, U32); //windingStart
	WRITECHECK(windingCount, U32); //windingCount
	return true;
}

bool Zone::read(std::istream &stream, U32 interiorFileVersion) {
	READCHECK(portalStart, U16); //portalStart
	READCHECK(portalCount, U16); //portalCount
	READCHECK(surfaceStart, U32); //surfaceStart
	READCHECK(surfaceCount, U32); //surfaceCount
	if (interiorFileVersion >= 12) {
		READCHECK(staticMeshStart, U32); //staticMeshStart
		READCHECK(staticMeshCount, U32); //staticMeshCount
//		READCHECK(flags, U16); //flags
	} else {
		staticMeshStart = 0;
		staticMeshCount = 0;
		flags = 0;
	}
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
	READCHECK(pointIndex0, S32); //pointIndex0
	READCHECK(pointIndex1, S32); //pointIndex1
	READCHECK(surfaceIndex0, S32); //surfaceIndex0
	READCHECK(surfaceIndex1, S32); //surfaceIndex1
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
	READCHECK(planeIndex, U16); //planeIndex
	READCHECK(triFanCount, U16); //triFanCount
	READCHECK(triFanStart, U32); //triFanStart
	READCHECK(zoneFront, U16); //zoneFront
	READCHECK(zoneBack, U16); //zoneBack
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

bool LightMapF::read(std::istream &stream) {
	READCHECK(finalWord, U16); //finalWord
	READCHECK(texGenXDistance, F32); //texGenXDistance
	READCHECK(texGenYDistance, F32); //texGenYDistance
	return true;
}

bool LightMapF::write(std::ostream &stream) const {
	WRITECHECK(finalWord, U16); //finalWord
	WRITECHECK(texGenXDistance, F32); //texGenXDistance
	WRITECHECK(texGenYDistance, F32); //texGenYDistance
	return true;
}

bool Surface::read(std::istream &stream, U32 interiorFileVersion, bool isTGEInterior, U32 indexSize, U32 planeSize, U32 materialSize, U32 texGenEqSize) {
	READCHECK(windingStart, U32); //windingStart
	if (interiorFileVersion >= 13) {
		READCHECK(windingCount, U32); //windingCount
	} else {
		READCHECK(windingCount, U8); //windingCount
	}
	if (windingStart + windingCount > indexSize)
		return false;

	//Fucking GarageGames. Sometimes the plane is | 0x8000 because WHY NOT
	S16 plane;
	READCHECK(plane, S16); //planeIndex
	//Ugly hack
	planeFlipped = (plane >> 15 != 0);
	plane &= ~0x8000;
	planeIndex = plane;
	if (planeIndex > planeSize)
		return false;

	READCHECK(textureIndex, U16); //textureIndex
	if (textureIndex > materialSize)
		return false;

	READCHECK(texGenIndex, U32); //texGenIndex
	if (texGenIndex > texGenEqSize)
		return false;

	READCHECK(surfaceFlags, U8); //surfaceFlags
	READCHECK(fanMask, U32); //fanMask
	READCHECK(lightMap, LightMapF); //lightMap
	READCHECK(lightCount, U16); //lightCount
	READCHECK(lightStateInfoStart, U32); //lightStateInfoStart

	if (interiorFileVersion >= 13) {
		READCHECK(mapOffsetX, U32); //mapOffsetX
		READCHECK(mapOffsetY, U32); //mapOffsetY
		READCHECK(mapSizeX, U32); //mapSizeX
		READCHECK(mapSizeY, U32); //mapSizeY
	} else {
		READCHECK(mapOffsetX, U8); //mapOffsetX
		READCHECK(mapOffsetY, U8); //mapOffsetY
		READCHECK(mapSizeX, U8); //mapSizeX
		READCHECK(mapSizeY, U8); //mapSizeY
	}

	if (!isTGEInterior) {
		READ(U8); //unused
		if (interiorFileVersion > 0 && interiorFileVersion <= 4) {
			READ(U32); //Extra bytes used for some unknown purpose, seen in versions 2, 3, 4
		}
	}
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
	WRITECHECK(lightMap, LightMapF); //lightMap
	WRITECHECK(lightCount, U16); //lightCount
	WRITECHECK(lightStateInfoStart, U32); //lightStateInfoStart
	WRITECHECK(mapOffsetX, U8); //mapOffsetX
	WRITECHECK(mapOffsetY, U8); //mapOffsetY
	WRITECHECK(mapSizeX, U8); //mapSizeX
	WRITECHECK(mapSizeY, U8); //mapSizeY
	return true;
}

bool NullSurface::read(std::istream &stream, U32 interiorFileVersion) {
	READCHECK(windingStart, U32); //windingStart
	READCHECK(planeIndex, U16); //planeIndex
	READCHECK(surfaceFlags, U8); //surfaceFlags
	if (interiorFileVersion >= 13) {
		READCHECK(windingCount, U32); //windingCount
	} else {
		READCHECK(windingCount, U8); //windingCount
	}
	return true;
}

bool NullSurface::write(std::ostream &stream) const {
	WRITECHECK(windingStart, U32); //windingStart
	WRITECHECK(planeIndex, U16); //planeIndex
	WRITECHECK(surfaceFlags, U8); //surfaceFlags
	WRITECHECK(windingCount, U8); //windingCount
	return true;
}

bool LightMap::read(std::istream &stream, bool isTGEInterior) {
	READCHECK(lightMap, PNG); //lightMap
	if (!isTGEInterior) {
		//These aren't even used in the real game!
		READCHECK(lightDirMap, PNG); //lightDirMap
	}
	READCHECK(keepLightMap, U8); //keepLightMap
	return true;
}

bool LightMap::write(std::ostream &stream) const {
	WRITECHECK(lightMap, PNG); //lightMap
	WRITECHECK(keepLightMap, U8); //keepLightMap
	return true;
}

bool AnimatedLight::read(std::istream &stream) {
	READCHECK(nameIndex, U32); //nameIndex
	READCHECK(stateIndex, U32); //stateIndex
	READCHECK(stateCount, U16); //stateCount
	READCHECK(flags, U16); //flags
	READCHECK(duration, U32); //duration
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
	READCHECK(red, U8); //red
	READCHECK(green, U8); //green
	READCHECK(blue, U8); //blue
	READCHECK(activeTime, U32); //activeTime
	READCHECK(dataIndex, U32); //dataIndex
	READCHECK(dataCount, U16); //dataCount
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
	READCHECK(surfaceIndex, U32); //surfaceIndex
	READCHECK(mapIndex, U32); //mapIndex
	READCHECK(lightStateIndex, U16); //lightStateIndex
	return true;
}

bool StateData::write(std::ostream &stream) const {
	WRITECHECK(surfaceIndex, U32); //surfaceIndex
	WRITECHECK(mapIndex, U32); //mapIndex
	WRITECHECK(lightStateIndex, U16); //lightStateIndex
	return true;
}

bool ConvexHull::read(std::istream &stream, U32 interiorFileVersion) {
	READCHECK(hullStart, U32); //hullStart
	READCHECK(hullCount, U16); //hullCount
	READCHECK(minX, F32); //minX
	READCHECK(maxX, F32); //maxX
	READCHECK(minY, F32); //minY
	READCHECK(maxY, F32); //maxY
	READCHECK(minZ, F32); //minZ
	READCHECK(maxZ, F32); //maxZ
	READCHECK(surfaceStart, U32); //surfaceStart
	READCHECK(surfaceCount, U16); //surfaceCount
	READCHECK(planeStart, U32); //planeStart
	READCHECK(polyListPlaneStart, U32); //polyListPlaneStart
	READCHECK(polyListPointStart, U32); //polyListPointStart
	READCHECK(polyListStringStart, U32); //polyListStringStart

	if (interiorFileVersion >= 12) {
		READCHECK(staticMesh, U8); //staticMesh
	} else {
		staticMesh = 0;
	}
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

bool CoordBin::read(std::istream &stream) {
	READCHECK(binStart, U32); //binStart
	READCHECK(binCount, U32); //binCount
	return true;
}

bool CoordBin::write(std::ostream &stream) const {
	WRITECHECK(binStart, U32); //binStart
	WRITECHECK(binCount, U32); //binCount
	return true;
}

bool TexMatrix::read(std::istream &stream) {
	READCHECK(T, S32); //T
	READCHECK(N, S32); //N
	READCHECK(B, S32); //B
	return true;
}

bool TexMatrix::write(std::ostream &stream) const {
	WRITECHECK(T, S32); //T
	WRITECHECK(N, S32); //N
	WRITECHECK(B, S32); //B
	return true;
}

DIF_NAMESPACE_END
