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

#ifndef staticMesh_h
#define staticMesh_h

#include <stdio.h>
#include "types.h"
#include "io.h"

struct Primitive : public Readable, Writable {
	U8 alpha;
	U32 texS;
	U32 texT;
	S32 diffuseIndex;
	S32 lightMapIndex;
	U32 start;
	U32 count;
	PlaneF lightMapEquationX;
	PlaneF lightMapEquationY;
	Point2I lightMapOffset;
	Point2I lightMapSize;

	bool read(FILE *file);
	bool write(FILE *file) const;
};

struct MaterialList : public Readable, Writable {
	U32 materialCount;

	U32 *flags;
	U32 *reflectanceMap;
	U32 *bumpMap;
	U32 *detailMap;
	U32 *lightMap;
	U32 *detailScale;
	U32 *reflectionAmount;

	bool read(FILE *file);
	bool write(FILE *file) const;
};

class StaticMesh {
	U32 numPrimitives;
	Primitive *primitive;

	U32 numIndices;
	U16 *index;

	U32 numVertices;
	Point3F *vertex;

	U32 numNormals;
	Point3F *normal;

	U32 numDiffuseUVs;
	Point2F *diffuseUV;

	U32 numLightmapUVs;
	Point2F *lightmapUV;

	U8 hasMaterialList;
	MaterialList baseMaterialList;

	U32 numDiffuseBitmaps;

	U8 hasSolid;
	U8 hasTranslucency;
	BoxF bounds;
	MatrixF transform;
	Point3F scale;
public:
	
	/**
	 Reads a StaticMesh from a FILE
	 @arg file - The FILE to read from (updates position)
	 @return A StaticMesh
	 */
	StaticMesh(FILE *file);

	bool write(FILE *file) const;

	/**
	 Frees the StaticMesh and all memory contained within it
	 */
	~StaticMesh();
};

#endif
