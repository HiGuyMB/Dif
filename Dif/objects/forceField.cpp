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
#include "forceField.h"

ForceField::ForceField(std::istream &stream) {
	READTOVAR(forceFieldFileVersion, U32); //forceFieldFileVersion
	READTOVAR(name, std::string); //name
	READTOVAR(trigger, std::vector<std::string>); //trigger
	READTOVAR(boundingBox, BoxF); //boundingBox
	READTOVAR(boundingSphere, SphereF); //boundingSphere
	READTOVAR(normal, std::vector<Point3F>); //normal
	READTOVAR(plane, std::vector<Plane_FF>); //plane
	READTOVAR(BSPNode, std::vector<BSPNode_FF>); //BSPNode
	READTOVAR(BSPSolidLeaf, std::vector<BSPSolidLeaf_FF>); //BSPSolidLeaf
	READTOVAR(index, std::vector<U32>); //index
	READTOVAR(surface, std::vector<Surface_FF>); //surface
	READTOVAR(solidLeafSurface, std::vector<U32>); //solidLeafSurface
	READTOVAR(color, ColorI); //color
}

bool ForceField::write(std::ostream &stream) const {
	WRITECHECK(forceFieldFileVersion, U32); //forceFieldFileVersion
	WRITE(name, std::string); //name
	WRITE(trigger, std::vector<std::string>); //trigger
	WRITECHECK(boundingBox, BoxF); //boundingBox
	WRITECHECK(boundingSphere, SphereF); //boundingSphere
	WRITE(normal, std::vector<Point3F>); //normal
	WRITE(plane, std::vector<Plane_FF>); //plane
	WRITE(BSPNode, std::vector<BSPNode_FF>); //BSPNode
	WRITE(BSPSolidLeaf, std::vector<BSPSolidLeaf_FF>); //BSPSolidLeaf
	WRITE(index, std::vector<U32>); //index
	WRITE(surface, std::vector<Surface_FF>); //surface
	WRITE(solidLeafSurface, std::vector<U32>); //solidLeafSurface
	WRITECHECK(color, ColorI); //color

	return true;
}

bool Plane_FF::read(std::istream &stream) {
	READTOVAR(normalIndex, U32); //normalIndex
	READTOVAR(planeDistance, F32); //planeDistance

	return true;
}

bool Plane_FF::write(std::ostream &stream) const {
	WRITE(normalIndex, U32); //normalIndex
	WRITE(planeDistance, F32); //planeDistance

	return true;
}

bool BSPNode_FF::read(std::istream &stream) {
	READTOVAR(frontIndex, U16); //frontIndex
	READTOVAR(backIndex, U16); //backIndex

	return true;
}

bool BSPNode_FF::write(std::ostream &stream) const {
	WRITE(frontIndex, U16); //frontIndex
	WRITE(backIndex, U16); //backIndex

	return true;
}

bool BSPSolidLeaf_FF::read(std::istream &stream) {
	READTOVAR(surfaceIndex, U32); //surfaceIndex
	READTOVAR(surfaceCount, U16); //surfaceCount

	return true;
}

bool BSPSolidLeaf_FF::write(std::ostream &stream) const {
	WRITE(surfaceIndex, U32); //surfaceIndex
	WRITE(surfaceCount, U16); //surfaceCount

	return true;
}

bool Surface_FF::read(std::istream &stream) {
	READTOVAR(windingStart, U32); //windingStart
	READTOVAR(windingCount, U8); //windingCount
	READTOVAR(planeIndex, U16); //planeIndex
	READTOVAR(surfaceFlags, U8); //surfaceFlags
	READTOVAR(fanMask, U32); //fanMask

	return true;
}

bool Surface_FF::write(std::ostream &stream) const {
	WRITE(windingStart, U32); //windingStart
	WRITE(windingCount, U8); //windingCount
	WRITE(planeIndex, U16); //planeIndex
	WRITE(surfaceFlags, U8); //surfaceFlags
	WRITE(fanMask, U32); //fanMask

	return true;
}
