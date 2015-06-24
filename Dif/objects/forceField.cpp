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

ForceField::ForceField(FILE *file) {
	READTOVAR(forceFieldFileVersion, U32); //forceFieldFileVersion
	READTOVAR(name, String); //name
	READLOOPVAR(numTriggers, trigger, String) {
		READTOVAR(trigger[i], String); //trigger
	}
	READTOVAR(boundingBox, BoxF); //boundingBox
	READTOVAR(boundingSphere, SphereF); //boundingSphere
	READLOOPVAR(numNormals, normal, Point3F) {
		READTOVAR(normal[i], Point3F); //normal
	}
	READLOOPVAR(numPlanes, plane, Plane_FF) {
		READTOVAR(plane[i].normalIndex, U32); //normalIndex
		READTOVAR(plane[i].planeDistance, F32); //planeDistance
	}
	READLOOPVAR(numBSPNodes, BSPNode, BSPNode_FF) {
		READTOVAR(BSPNode[i].planeIndex, U16); //planeIndex
		READTOVAR(BSPNode[i].frontIndex, U16); //frontIndex
		READTOVAR(BSPNode[i].backIndex, U16); //backIndex
	}
	READLOOPVAR(numBSPSolidLeaves, BSPSolidLeaf, BSPSolidLeaf_FF) {
		READTOVAR(BSPSolidLeaf[i].surfaceIndex, U32); //surfaceIndex
		READTOVAR(BSPSolidLeaf[i].surfaceCount, U16); //surfaceCount
	}
	READLOOPVAR(numWindings, index, U32) {
		READTOVAR(index[i], U32); //index
	}
	READLOOPVAR(numSurfaces, surface, Surface_FF) {
		READTOVAR(surface[i].windingStart, U32); //windingStart
		READTOVAR(surface[i].windingCount, U8); //windingCount
		READTOVAR(surface[i].planeIndex, U16); //planeIndex
		READTOVAR(surface[i].surfaceFlags, U8); //surfaceFlags
		READTOVAR(surface[i].fanMask, U32); //fanMask
	}
	READLOOPVAR(numSolidLeafSurfaces, solidLeafSurface, U32) {
		READTOVAR(solidLeafSurface[i], U32); //solidLeafSurface
	}
	READTOVAR(color, ColorI); //color
}

bool ForceField::write(FILE *file) const {
	WRITECHECK(forceFieldFileVersion, U32); //forceFieldFileVersion
	WRITE(name, String); //name
	WRITELOOP(numTriggers) { //trigger
		WRITECHECK(trigger[i], String);
	}
	WRITECHECK(boundingBox, BoxF); //boundingBox
	WRITECHECK(boundingSphere, SphereF); //boundingSphere
	WRITELIST(numNormals, normal, Point3F); //normal
	WRITELOOP(numPlanes) { //numPlanes
		WRITECHECK(plane[i].normalIndex, U32); //normalIndex
		WRITECHECK(plane[i].planeDistance, F32); //planeDistance
	}
	WRITELOOP(numBSPNodes) { //numBSPNodes
		WRITECHECK(BSPNode[i].planeIndex, U16); //planeIndex
		WRITECHECK(BSPNode[i].frontIndex, U16); //frontIndex
		WRITECHECK(BSPNode[i].backIndex, U16); //backIndex
	}
	WRITELOOP(numBSPSolidLeaves) { //numBSPSolidLeaves
		WRITECHECK(BSPSolidLeaf[i].surfaceIndex, U32); //surfaceIndex
		WRITECHECK(BSPSolidLeaf[i].surfaceCount, U16); //surfaceCount
	}
	WRITELIST(numWindings, index, U32); //index
	WRITELOOP(numSurfaces) { //numSurfaces
		WRITECHECK(surface[i].windingStart, U32); //windingStart
		WRITECHECK(surface[i].windingCount, U8); //windingCount
		WRITECHECK(surface[i].planeIndex, U16); //planeIndex
		WRITECHECK(surface[i].surfaceFlags, U8); //surfaceFlags
		WRITECHECK(surface[i].fanMask, U32); //fanMask
	}
	WRITELIST(numSolidLeafSurfaces, solidLeafSurface, U32); //solidLeafSurface
	WRITECHECK(color, ColorI); //color

	return true;
}

ForceField::~ForceField() {
	delete [] trigger;
	delete [] normal;
	delete [] plane;
	delete [] BSPNode;
	delete [] BSPSolidLeaf;
	delete [] index;
	delete [] surface;
	delete [] solidLeafSurface;
}
