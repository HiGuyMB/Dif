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

#ifndef dif_h
#define dif_h

#include "types.h"
#include "interior.h"
#include "trigger.h"
#include "interiorPathFollower.h"
#include "forceField.h"
#include "aiSpecialNode.h"
#include "vehicleCollision.h"
#include "gameEntity.h"

class DIF : public Readable, public Writable {
public:
	std::vector<Interior> interior;
	std::vector<Interior> subObject;

	std::vector<Trigger> trigger;
	std::vector<InteriorPathFollower> interiorPathFollower;
	std::vector<ForceField> forceField;
	std::vector<AISpecialNode> aiSpecialNode;
	VehicleCollision vehicleCollision;

	U32 unknown0;
	U32 unknown1;
	U32 unknown2;
	U32 unknown3;

	U32 readGameEntities;
	std::vector<GameEntity> gameEntity;

	U32 dummy;

	/**
	 * Reads a DIF from a stream
	 * @param 1 The stream to read from
	 * @return If the operation was successful
	 */
	bool read(std::istream &stream);
	/**
	 * Writes a DIF to a stream
	 * @param 1 The stream to write to
	 * @return If the operation was successful
	 */
	bool write(std::ostream &stream) const;
};

#endif
