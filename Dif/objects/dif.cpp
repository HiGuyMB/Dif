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

#include "io.h"
#include "dif.h"

bool DIF::read(std::istream &stream) {
	//http://rustycode.com/tutorials/DIF_File_Format_44_14.html
	// Someone give that guy all the cookies.

	READCHECK(U32, 44); //interiorResourceFileVersion
	if (READ(U8)) { //previewIncluded
		READ(PNG); //previewBitmap
	}

	READTOVAR(interior, std::vector<Interior>);
	READTOVAR(subObject, std::vector<Interior>);
	READTOVAR(trigger, std::vector<Trigger>);
	READTOVAR(interiorPathFollower, std::vector<InteriorPathFollower>);
	READTOVAR(forceField, std::vector<ForceField>);
	READTOVAR(aiSpecialNode, std::vector<AISpecialNode>);
	if (READ(U32) == 1) { //readVehicleCollision
		vehicleCollision.read(stream);
	}
	READ(U32); //unknown
	READ(U32); //unknown
	READ(U32); //unknown
	READ(U32); //unknown
	if (READ(U32) == 2) { //readGameEntities
		READTOVAR(gameEntity, std::vector<GameEntity>);
	}
	READ(U32); //dummy

	return true;
}

bool DIF::write(std::ostream &stream) const {
	WRITECHECK(44, U32); //interiorResourceFileVersion
	WRITECHECK(0, U8); //previewIncluded

	WRITECHECK(interior, std::vector<Interior>);
	WRITECHECK(subObject, std::vector<Interior>);
	WRITECHECK(trigger, std::vector<Trigger>);
	WRITECHECK(interiorPathFollower, std::vector<InteriorPathFollower>);
	WRITECHECK(forceField, std::vector<ForceField>);
	WRITECHECK(aiSpecialNode, std::vector<AISpecialNode>);
	WRITECHECK(1, U32);
	vehicleCollision.write(stream);

	WRITECHECK(0, U32);
	WRITECHECK(0, U32);
	WRITECHECK(0, U32);
	WRITECHECK(0, U32);
	if (gameEntity.size()) {
		WRITECHECK(2, U32);
		WRITECHECK(gameEntity, std::vector<GameEntity>);
	} else {
		WRITECHECK(0, U32);
	}

	WRITECHECK(0, U32);

	return true;
}
