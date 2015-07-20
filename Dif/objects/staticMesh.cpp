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

#include "staticMesh.h"
#include <assert.h>

bool StaticMesh::read(std::istream &stream) {
	READTOVAR(primitive, std::vector<Primitive>); //primitive
	READTOVAR(index, std::vector<U16>); //index
	READTOVAR(vertex, std::vector<Point3F>); //vertex
	READTOVAR(normal, std::vector<Point3F>); //normal
	READTOVAR(diffuseUV, std::vector<Point2F>); //diffuseUV
	READTOVAR(lightmapUV, std::vector<Point2F>); //lightmapUV

	READTOVAR(hasMaterialList, U8);
	if (hasMaterialList) {
		baseMaterialList = MaterialList();
		baseMaterialList.read(stream);
	}

	READTOVAR(hasSolid, U8);
	READTOVAR(hasTranslucency, U8);
	READTOVAR(bounds, BoxF);
	READTOVAR(transform, MatrixF);
	READTOVAR(scale, Point3F);

	return true;
}

bool StaticMesh::write(std::ostream &stream) const {
	WRITE(primitive, std::vector<Primitive>); //primitive
	WRITE(index, std::vector<U16>); //index
	WRITE(vertex, std::vector<Point3F>); //vertex
	WRITE(normal, std::vector<Point3F>); //normal
	WRITE(diffuseUV, std::vector<Point2F>); //diffuseUV
	WRITE(lightmapUV, std::vector<Point2F>); //lightmapUV

	if (hasMaterialList) {
		baseMaterialList.write(stream);
	}

	WRITECHECK(hasSolid, U8);
	WRITECHECK(hasTranslucency, U8);
	WRITECHECK(bounds, BoxF);
	WRITECHECK(transform, MatrixF);
	WRITECHECK(scale, Point3F);

	return true;
}

bool Primitive::read(std::istream &stream) {
	READTOVAR(alpha, U8);
	READTOVAR(texS, U32);
	READTOVAR(texT, U32);
	READTOVAR(diffuseIndex, S32);
	READTOVAR(lightMapIndex, S32);
	READTOVAR(start, U32);
	READTOVAR(count, U32);
	READTOVAR(lightMapEquationX, PlaneF);
	READTOVAR(lightMapEquationY, PlaneF);
	READTOVAR(lightMapOffset, Point2I);
	READTOVAR(lightMapSize, Point2I);

	return true;
}

bool Primitive::write(std::ostream &stream) const {
	WRITECHECK(alpha, U8);
	WRITECHECK(texS, U32);
	WRITECHECK(texT, U32);
	WRITECHECK(diffuseIndex, S32);
	WRITECHECK(lightMapIndex, S32);
	WRITECHECK(start, U32);
	WRITECHECK(count, U32);
	WRITECHECK(lightMapEquationX, PlaneF);
	WRITECHECK(lightMapEquationY, PlaneF);
	WRITECHECK(lightMapOffset, Point2I);
	WRITECHECK(lightMapSize, Point2I);

	return true;
}

bool MaterialList::read(std::istream &stream) {
	//It's a disaster
	assert(1);

	return true;
}

bool MaterialList::write(std::ostream &stream) const {
	//Not going to bother
	assert(1);

	return true;
}
