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
#include "dif.h"

DIF::DIF(std::istream &stream, String directory) {
	//http://rustycode.com/tutorials/DIF_File_Format_44_14.html
	// Someone give that guy all the cookies.

	READCHECK(U32, 44); //interiorResourceFileVersion
	if (READ(U8)) { //previewIncluded
		READ(PNG); //previewBitmap
	}

	READLOOPVAR(numDetailLevels, interior, Interior *) {
		interior[i] = new Interior();
		interior[i]->read(stream);
	}
	READLOOPVAR(numSubObjects, subObject, Interior *) {
		subObject[i] = new Interior();
		subObject[i]->read(stream);
	}
	READLOOPVAR(numTriggers, trigger, Trigger *) {
		trigger[i] = new Trigger(stream);
	}
	READLOOPVAR(numInteriorPathFollowers, interiorPathFollower, InteriorPathFollower *) {
		interiorPathFollower[i] = new InteriorPathFollower(stream);
	}
	READLOOPVAR(numForceFields, forceField, ForceField *) {
		forceField[i] = new ForceField(stream);
	}
	READLOOPVAR(numAISpecialNodes, aiSpecialNode, AISpecialNode *) {
		aiSpecialNode[i] = new AISpecialNode(stream);
	}
	if (READ(U32) == 1) { //readVehicleCollision
		vehicleCollision = new VehicleCollision(stream);
	}
	READ(U32); //unknown
	READ(U32); //unknown
	READ(U32); //unknown
	READ(U32); //unknown
	if (READ(U32) == 2) { //readGameEntities
		READLOOPVAR(numGameEntities, gameEntity, GameEntity *) {
			gameEntity[i] = new GameEntity();
			gameEntity[i]->read(stream);
		}
	} else {
		numGameEntities = 0;
		gameEntity = NULL;
	}
	READ(U32); //dummy
}

bool DIF::write(std::ostream &stream, String directory) const {
	WRITECHECK(44, U32); //interiorResourceFileVersion
	WRITECHECK(0, U8); //previewIncluded

	WRITELOOP(numDetailLevels) {
		if (!interior[i]->write(stream)) return false;
	}
	WRITELOOP(numSubObjects) {
		if (!subObject[i]->write(stream)) return false;
	}
	WRITELOOP(numTriggers) {
		if (!trigger[i]->write(stream)) return false;
	}
	WRITELOOP(numInteriorPathFollowers) {
		if (!interiorPathFollower[i]->write(stream)) return false;
	}
	WRITELOOP(numForceFields) {
		if (!forceField[i]->write(stream)) return false;
	}
	WRITELOOP(numAISpecialNodes) {
		if (!aiSpecialNode[i]->write(stream)) return false;
	}
	WRITECHECK(1, U32);
	vehicleCollision->write(stream);

	WRITECHECK(0, U32);
	WRITECHECK(0, U32);
	WRITECHECK(0, U32);
	WRITECHECK(0, U32);
	if (gameEntity){
		WRITECHECK(2, U32);
		WRITELOOP(numGameEntities) {
			gameEntity[i]->write(stream);
		}
	} else {
		WRITECHECK(0, U32);
	}

	WRITECHECK(0, U32);

	return true;
}

DIF::~DIF() {
	for (int i = 0; i < numDetailLevels; i ++) {
		delete interior[i];
	}
	delete [] interior;
	for (int i = 0; i < numSubObjects; i ++) {
		delete subObject[i];
	}
	delete [] subObject;
	for (int i = 0; i < numTriggers; i ++) {
		delete trigger[i];
	}
	delete [] trigger;
	for (int i = 0; i < numInteriorPathFollowers; i ++) {
		delete interiorPathFollower[i];
	}
	delete [] interiorPathFollower;
	for (int i = 0; i < numForceFields; i ++) {
		delete forceField[i];
	}
	delete [] forceField;
	for (int i = 0; i < numAISpecialNodes; i ++) {
		delete aiSpecialNode[i];
	}
	delete [] aiSpecialNode;
	delete vehicleCollision;
	for (int i = 0; i < numGameEntities; i ++) {
		delete gameEntity[i];
	}
	delete [] gameEntity;
}
