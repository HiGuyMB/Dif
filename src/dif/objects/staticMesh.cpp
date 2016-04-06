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

#include <dif/objects/staticMesh.h>
#include <assert.h>

DIF_NAMESPACE

bool StaticMesh::read(std::istream &stream, Version &version) {
	READCHECK(primitive, std::vector<Primitive>); //primitive
	READCHECK(index, std::vector<U16>); //index
	READCHECK(vertex, std::vector<Point3F>); //vertex
	READCHECK(normal, std::vector<Point3F>); //normal
	READCHECK(diffuseUV, std::vector<Point2F>); //diffuseUV
	READCHECK(lightmapUV, std::vector<Point2F>); //lightmapUV

	READCHECK(hasMaterialList, U8); //hasMaterialList
	if (hasMaterialList) {
		baseMaterialList.read(stream, version); //baseMaterialList
	}

	READCHECK(hasSolid, U8); //hasSolid
	READCHECK(hasTranslucency, U8); //hasTranslucency
	READCHECK(bounds, BoxF); //bounds
	READCHECK(transform, MatrixF); //transform
	READCHECK(scale, Point3F); //scale

	return true;
}

bool StaticMesh::write(std::ostream &stream, Version version) const {
	WRITECHECK(primitive, std::vector<Primitive>); //primitive
	WRITECHECK(index, std::vector<U16>); //index
	WRITECHECK(vertex, std::vector<Point3F>); //vertex
	WRITECHECK(normal, std::vector<Point3F>); //normal
	WRITECHECK(diffuseUV, std::vector<Point2F>); //diffuseUV
	WRITECHECK(lightmapUV, std::vector<Point2F>); //lightmapUV

	WRITECHECK(hasMaterialList, U8); //hasMaterialList
	if (hasMaterialList) {
		baseMaterialList.write(stream, version); //baseMaterialList
	}

	WRITECHECK(hasSolid, U8); //hasSolid
	WRITECHECK(hasTranslucency, U8); //hasTranslucency
	WRITECHECK(bounds, BoxF); //bounds
	WRITECHECK(transform, MatrixF); //transform
	WRITECHECK(scale, Point3F); //scale

	return true;
}

bool StaticMesh::Primitive::read(std::istream &stream, Version &version) {
	READCHECK(alpha, U8); //alpha
	READCHECK(texS, U32); //texS
	READCHECK(texT, U32); //texT
	READCHECK(diffuseIndex, S32); //diffuseIndex
	READCHECK(lightMapIndex, S32); //lightMapIndex
	READCHECK(start, U32); //start
	READCHECK(count, U32); //count
	READCHECK(lightMapEquationX, PlaneF); //lightMapEquationX
	READCHECK(lightMapEquationY, PlaneF); //lightMapEquationY
	READCHECK(lightMapOffset, Point2I); //lightMapOffset
	READCHECK(lightMapSize, Point2I); //lightMapSize

	return true;
}

bool StaticMesh::Primitive::write(std::ostream &stream, Version version) const {
	WRITECHECK(alpha, U8); //alpha
	WRITECHECK(texS, U32); //texS
	WRITECHECK(texT, U32); //texT
	WRITECHECK(diffuseIndex, S32); //diffuseIndex
	WRITECHECK(lightMapIndex, S32); //lightMapIndex
	WRITECHECK(start, U32); //start
	WRITECHECK(count, U32); //count
	WRITECHECK(lightMapEquationX, PlaneF); //lightMapEquationX
	WRITECHECK(lightMapEquationY, PlaneF); //lightMapEquationY
	WRITECHECK(lightMapOffset, Point2I); //lightMapOffset
	WRITECHECK(lightMapSize, Point2I); //lightMapSize

	return true;
}

bool StaticMesh::MaterialList::read(std::istream &stream, Version &version) {
	//It's a disaster
	assert(0);

	return true;
}

bool StaticMesh::MaterialList::write(std::ostream &stream, Version version) const {
	//Not going to bother
	assert(0);

	return true;
}

DIF_NAMESPACE_END
