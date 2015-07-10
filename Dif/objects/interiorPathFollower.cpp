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
#include "interiorPathFollower.h"

InteriorPathFollower::InteriorPathFollower(std::istream &stream) {
	READTOVAR(name, String); //name
	READTOVAR(datablock, String); //datablock
	READTOVAR(interiorResIndex, U32); //interiorResIndex
	READTOVAR(offset, Point3F); //offset
	READTOVAR(properties, Dictionary); //properties
	READLOOPVAR(numTriggerIds, triggerId, U32) {
		READTOVAR(triggerId[i], U32); //triggerId
	}
	READLOOPVAR(numWayPoints, wayPoint, WayPoint) {
		READTOVAR(wayPoint[i].position, Point3F); //position
		READTOVAR(wayPoint[i].rotation, QuatF); //rotation
		READTOVAR(wayPoint[i].msToNext, U32); //msToNext
		READTOVAR(wayPoint[i].smoothingType, U32); //smoothingType
	}
	READTOVAR(totalMS, U32); //totalMS
}

bool InteriorPathFollower::write(std::ostream &stream) const {
	WRITE(name, String); //name
	WRITE(datablock, String); //datablock
	WRITECHECK(interiorResIndex, U32); //interiorResIndex
	WRITECHECK(offset, Point3F); //offset
	WRITE(properties, Dictionary); //properties
	WRITELIST(numTriggerIds, triggerId, U32); //triggerId
	WRITELOOP(numWayPoints) { //numWayPoints
		WRITECHECK(wayPoint[i].position, Point3F); //position
		WRITECHECK(wayPoint[i].rotation, QuatF); //rotation
		WRITECHECK(wayPoint[i].msToNext, U32); //msToNext
		WRITECHECK(wayPoint[i].smoothingType, U32); //smoothingType
	}
	WRITECHECK(totalMS, U32); //totalMS

	return true;
}

InteriorPathFollower::~InteriorPathFollower() {
	delete [] triggerId;
	delete [] wayPoint;
}
