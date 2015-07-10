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
#include "types.h"
#include "io.h"
#include "trigger.h"

Trigger::Trigger(std::istream &stream) {
	READTOVAR(name, String); //name
	READTOVAR(datablock, String); //datablock
	READTOVAR(properties, Dictionary); //properties
	READLOOPVAR(numPolyHedronPoints, polyHedronPoint, Point3F) {
		READTOVAR(polyHedronPoint[i], Point3F); //point
	}
	READLOOPVAR(numPolyHedronPlanes, polyHedronPlane, PlaneF) {
		READTOVAR(polyHedronPlane[i], PlaneF); //plane
	}
	READLOOPVAR(numPolyHedronEdges, polyHedronEdge, PolyHedronEdge) {
		READTOVAR(polyHedronEdge[i].face0, U32); //face0
		READTOVAR(polyHedronEdge[i].face1, U32); //face1
		READTOVAR(polyHedronEdge[i].vertex0, U32); //vertex0
		READTOVAR(polyHedronEdge[i].vertex1, U32); //vertex1
	}
	READTOVAR(offset, Point3F); //offset
}

bool Trigger::write(std::ostream &stream) const {
	WRITE(name, String); //name
	WRITE(datablock, String); //datablock
	WRITE(properties, Dictionary); //properties
	WRITELIST(numPolyHedronPoints, polyHedronPoint, Point3F); //polyHedronPoint
	WRITELIST(numPolyHedronPlanes, polyHedronPlane, PlaneF); //polyHedronPlane
	WRITELOOP(numPolyHedronEdges) { //numPolyHedronEdges
		WRITECHECK(polyHedronEdge[i].face0, U32); //face0
		WRITECHECK(polyHedronEdge[i].face1, U32); //face1
		WRITECHECK(polyHedronEdge[i].vertex0, U32); //vertex0
		WRITECHECK(polyHedronEdge[i].vertex1, U32); //vertex1
	}
	WRITECHECK(offset, Point3F); //offset

	return true;
}

Trigger::~Trigger() {
	delete [] polyHedronPoint;
	delete [] polyHedronPlane;
	delete [] polyHedronEdge;
}
