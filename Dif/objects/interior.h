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

#ifndef interior_h
#define interior_h

#include "types.h"
#include "staticMesh.h"

static U32 gNumCoordBins = 16;

struct Plane : public Readable, Writable {
	U16 normalIndex;
	F32 planeDistance;

	virtual bool read(std::istream &stream);
	virtual bool write(std::ostream &stream) const;
};

struct TexGenEq : public Readable, Writable {
	PlaneF planeX;
	PlaneF planeY;

	virtual bool read(std::istream &stream);
	virtual bool write(std::ostream &stream) const;
};

struct BSPNode : public Writable {
	U16 planeIndex;
	U16 frontIndex;
	U16 backIndex;

	virtual bool write(std::ostream &stream) const;
};

struct BSPSolidLeaf : public Readable, Writable {
	U32 surfaceIndex;
	U16 surfaceCount;

	virtual bool read(std::istream &stream);
	virtual bool write(std::ostream &stream) const;
};

struct WindingIndex : public Readable, Writable {
	U32 windingStart;
	U32 windingCount;

	virtual bool read(std::istream &stream);
	virtual bool write(std::ostream &stream) const;
};

struct Edge : public Readable, Writable {
	S32 pointIndex0;
	S32 pointIndex1;
	S32 surfaceIndex0;
	S32 surfaceIndex1;

	virtual bool read(std::istream &stream);
	virtual bool write(std::ostream &stream) const;
};

struct Zone : public Writable {
	U16 portalStart;
	U16 portalCount;
	U32 surfaceStart;
	U32 surfaceCount;
	U32 staticMeshStart;
	U32 staticMeshCount;
	U16 flags;

	virtual bool write(std::ostream &stream) const;
};

struct Portal : public Readable, Writable {
	U16 planeIndex;
	U16 triFanCount;
	U32 triFanStart;
	U16 zoneFront;
	U16 zoneBack;

	virtual bool read(std::istream &stream);
	virtual bool write(std::ostream &stream) const;
};

struct LightMapF {
	U16 finalWord;
	F32 texGenXDistance;
	F32 texGenYDistance;
};

struct LightMap : public Writable {
	PNG lightMap;
	PNG lightDirMap;
	U8 keepLightMap;

	virtual bool write(std::ostream &stream) const;
};

struct Surface : public Writable {
	U32 windingStart;
	U8 windingCount;
	U16 planeIndex;
	U8 planeFlipped;
	U16 textureIndex;
	U32 texGenIndex;
	U8 surfaceFlags;
	U32 fanMask;
	LightMapF lightMap;
	U16 lightCount;
	U32 lightStateInfoStart;
	U8 mapOffsetX;
	U8 mapOffsetY;
	U8 mapSizeX;
	U8 mapSizeY;

	virtual bool write(std::ostream &stream) const;
};

struct NullSurface : public Writable {
	U32 windingStart;
	U16 planeIndex;
	U8 surfaceFlags;
	U8 windingCount;

	virtual bool write(std::ostream &stream) const;
};

struct AnimatedLight : public Readable, Writable {
	U32 nameIndex;
	U32 stateIndex;
	U16 stateCount;
	U16 flags;
	U32 duration;

	virtual bool read(std::istream &stream);
	virtual bool write(std::ostream &stream) const;
};

struct LightState : public Readable, Writable {
	U8 red;
	U8 green;
	U8 blue;
	U32 activeTime;
	U32 dataIndex;
	U16 dataCount;

	virtual bool read(std::istream &stream);
	virtual bool write(std::ostream &stream) const;
};

struct StateData : public Readable, Writable {
	U32 surfaceIndex;
	U32 mapIndex;
	U16 lightStateIndex;

	virtual bool read(std::istream &stream);
	virtual bool write(std::ostream &stream) const;
};

struct ConvexHull : public Writable {
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
	U8 staticMesh;

	virtual bool write(std::ostream &stream) const;
};

struct CoordBin {
	U32 binStart;
	U32 binCount;
};

struct TexMatrix : public Readable, Writable {
	S32 T;
	S32 N;
	S32 B;

	virtual bool read(std::istream &stream);
	virtual bool write(std::ostream &stream) const;
};

class Interior : public Readable, Writable {
public:
	U32 interiorFileVersion;
	U32 detailLevel;
	U32 minPixels;
	BoxF boundingBox;
	SphereF boundingSphere;
	U8 hasAlarmState;
	U32 numLightStateEntries;

	std::vector<Point3F> normal;
	std::vector<Plane> plane;
	std::vector<Point3F> point;
	std::vector<U8> pointVisibility;
	std::vector<TexGenEq> texGenEq;

	U32 numBSPNodes;
	BSPNode *BSPNode;

	std::vector<BSPSolidLeaf> BSPSolidLeaf;

	U8 materialListVersion;
	std::vector<std::string> materialName;

	U32 numWindings;
	U32 *index;

	std::vector<WindingIndex> windingIndex;
	std::vector<Edge> edge;

	U32 numZones;
	Zone *zone;

	U32 numZoneSurfaces;
	U16 *zoneSurface;

	std::vector<U32> zoneStaticMesh;

	U32 numZonePortalList;
	U16 *zonePortalList;

	std::vector<Portal> portal;

	U32 numSurfaces;
	Surface *surface;

	std::vector<U8> normalLMapIndex;
	std::vector<U8> alarmLMapIndex;

	U32 numNullSurfaces;
	NullSurface *nullSurface;

	U32 numLightMaps;
	LightMap *lightMap;

	U32 numSolidLeafSurfaces;
	U32 *solidLeafSurface;

	std::vector<AnimatedLight> animatedLight;
	std::vector<LightState> lightState;
	std::vector<StateData> stateData;

	U32 numStateDataBuffers;
	U8 *stateDataBuffer;

	U32 flags;

	std::vector<U8> nameBufferCharacter;

	U32 numSubObjects;
	//SubObject *subObject;

	U32 numConvexHulls;
	ConvexHull *convexHull;

	std::vector<U8> convexHullEmitStringCharacter;

	U32 numHullIndices;
	U32 *hullIndex;

	U32 numHullPlaneIndices;
	U16 *hullPlaneIndex;

	U32 numHullEmitStringIndices;
	U32 *hullEmitStringIndex;

	U32 numHullSurfaceIndices;
	U32 *hullSurfaceIndex;

	U32 numPolyListPlanes;
	U16 *polyListPlaneIndex;

	U32 numPolyListPoints;
	U32 *polyListPointIndex;

	std::vector<U8> polyListStringCharacter;

	CoordBin *coordBin;

	U32 numCoordBinIndices;
	U16 *coordBinIndex;

	U32 coordBinMode;
	ColorI baseAmbientColor;
	ColorI alarmAmbientColor;

	U32 numStaticMeshes;
	StaticMesh **staticMesh;

	std::vector<Point3F> texNormal;
	std::vector<TexMatrix> texMatrix;
	std::vector<U32> texMatIndex;

	U32 extendedLightMapData;
	U32 lightMapBorderSize;

	~Interior();

	/**
	 Reads an Interior from a FILE
	 @arg file - The FILE to read from (updates position)
	 @arg directory - The base directory for images
	 */
	bool read(std::istream &stream);
	bool write(std::ostream &stream) const;

private:
	bool readSurface(std::istream &stream, Surface *surface, bool isTGEInterior);
	
};

#endif
