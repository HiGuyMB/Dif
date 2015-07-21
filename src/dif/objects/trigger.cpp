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

#include <sstream>
#include <dif/base/types.h>
#include <dif/base/io.h>
#include <dif/objects/trigger.h>

DIF_NAMESPACE

bool Trigger::read(std::istream &stream) {
	READCHECK(name, std::string); //name
	READCHECK(datablock, std::string); //datablock
	READCHECK(properties, Dictionary); //properties
	READCHECK(polyhedron, PolyHedron); //polyhedron
	READCHECK(offset, Point3F); //offset

	return true;
}

bool Trigger::write(std::ostream &stream) const {
	WRITECHECK(name, std::string); //name
	WRITECHECK(datablock, std::string); //datablock
	WRITECHECK(properties, Dictionary); //properties
	WRITECHECK(polyhedron, PolyHedron); //polyhedron
	WRITECHECK(offset, Point3F); //offset

	return true;
}

bool Trigger::PolyHedron::read(std::istream &stream) {
	READCHECK(pointList, std::vector<Point3F>); //point
	READCHECK(planeList, std::vector<PlaneF>); //plane
	READCHECK(edgeList, std::vector<PolyHedronEdge>); //polyHedronEdge

	return true;
}

bool Trigger::PolyHedron::write(std::ostream &stream) const {
	WRITECHECK(pointList, std::vector<Point3F>); //polyHedronPoint
	WRITECHECK(planeList, std::vector<PlaneF>); //polyHedronPlane
	WRITECHECK(edgeList, std::vector<PolyHedronEdge>); //numPolyHedronEdges

	return true;
}

bool Trigger::PolyHedronEdge::read(std::istream &stream) {
	READCHECK(face[0], U32); //face0
	READCHECK(face[1], U32); //face1
	READCHECK(vertex[0], U32); //vertex0
	READCHECK(vertex[1], U32); //vertex1

	return true;
}

bool Trigger::PolyHedronEdge::write(std::ostream &stream) const {
	WRITECHECK(face[0], U32); //face0
	WRITECHECK(face[1], U32); //face1
	WRITECHECK(vertex[0], U32); //vertex0
	WRITECHECK(vertex[1], U32); //vertex1

	return true;
}

std::string Trigger::getPolyhedron() {
	// First point is corner, need to find the three vectors...
	Point3F origin = polyhedron.pointList[0];
	U32 currVec = 0;
	Point3F vecs[3];
	for (U32 i = 0; i < polyhedron.edgeList.size(); i++) {
		const U32 *vertex = polyhedron.edgeList[i].vertex;
		if (vertex[0] == 0)
			vecs[currVec++] = polyhedron.pointList[vertex[1]] - origin;
		else
			if (vertex[1] == 0)
				vecs[currVec++] = polyhedron.pointList[vertex[0]] - origin;
	}

	// Build output string.
	std::stringstream ss;
	ss << origin.x << ' ' << origin.y << ' ' << origin.z << ' ' <<
	      vecs[0].x << ' ' << vecs[0].y << ' ' << vecs[0].z << ' ' <<
	      vecs[1].x << ' ' << vecs[1].y << ' ' << vecs[1].z << ' ' <<
	      vecs[2].x << ' ' << vecs[2].y << ' ' << vecs[2].z;
	return ss.str();
}

DIF_NAMESPACE_END
