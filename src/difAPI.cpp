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

#include <fstream>
#include <dif/objects/dif.h>
#include <dif/base/point3.h>
#include <dif/base/point2.h>
#include "difAPI.h"

//-----------------------------------------------------------------------------
// DIF parser
//-----------------------------------------------------------------------------

float planeF_distance_to_point(const DIF::PlaneF &plane, const DIF::Point3F &point) {
	return (plane.x * point.x + plane.y * point.y + plane.z * point.z) + plane.d;
}

static void parseDif(Dif *thisptr, DIF::DIF &dif) {
	const DIF::Interior &interior = dif.interior[0];

	// set the materials.
	thisptr->mMaterials = interior.materialName;

	// Create an instance in the map for each material
	for (size_t i = 0; i < interior.surface.size(); i++) {
		const DIF::Interior::Surface &surface = interior.surface[i];
		DIF::Point3F normal = interior.normal[interior.plane[surface.planeIndex].normalIndex];
		if (surface.planeFlipped)
			normal *= -1.0f;

		//New and improved rendering with actual Triangle Strips this time
		for (size_t j = surface.windingStart + 2; j < surface.windingStart + surface.windingCount; j++) {
			DIF::Point3F v0, v1, v2;

			if ((j - (surface.windingStart + 2)) % 2 == 0) {
				v0 = interior.point[interior.index[j]];
				v1 = interior.point[interior.index[j - 1]];
				v2 = interior.point[interior.index[j - 2]];
			}
			else {
				v0 = interior.point[interior.index[j - 2]];
				v1 = interior.point[interior.index[j - 1]];
				v2 = interior.point[interior.index[j]];
			}

			DIF::Interior::TexGenEq texGenEq = interior.texGenEq[surface.texGenIndex];

			DIF::Point2F uv0 = DIF::Point2F(planeF_distance_to_point(texGenEq.planeX, v0), planeF_distance_to_point(texGenEq.planeY, v0));
			DIF::Point2F uv1 = DIF::Point2F(planeF_distance_to_point(texGenEq.planeX, v1), planeF_distance_to_point(texGenEq.planeY, v1));
			DIF::Point2F uv2 = DIF::Point2F(planeF_distance_to_point(texGenEq.planeX, v2), planeF_distance_to_point(texGenEq.planeY, v2));

			DIF::Point3F deltaPos1 = v1 - v0;
			DIF::Point3F deltaPos2 = v2 - v0;
			DIF::Point2F deltaUV1 = uv1 - uv0;
			DIF::Point2F deltaUV2 = uv2 - uv0;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

			DIF::Point3F tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
			//glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

			tangent = tangent - (normal * normal.dot(tangent));
			tangent = tangent.normalize();
			//if (glm::dot(glm::cross(normal, tangent), bitangent) < 0.0f) {
				//tangent *= -1.0f;
			//}

			//tangent = glm::normalize(tangent);
			//bitangent = glm::normalize(bitangent);

			// Point 0 data
			thisptr->mVertices[surface.textureIndex].push_back(v0.x);
			thisptr->mVertices[surface.textureIndex].push_back(v0.y);
			thisptr->mVertices[surface.textureIndex].push_back(v0.z);
			thisptr->mUVs[surface.textureIndex].push_back(uv0.x);
			thisptr->mUVs[surface.textureIndex].push_back(uv0.y);

			// Point 1 data
			thisptr->mVertices[surface.textureIndex].push_back(v1.x);
			thisptr->mVertices[surface.textureIndex].push_back(v1.y);
			thisptr->mVertices[surface.textureIndex].push_back(v1.z);
			thisptr->mUVs[surface.textureIndex].push_back(uv1.x);
			thisptr->mUVs[surface.textureIndex].push_back(uv1.y);

			// Point 2 data
			thisptr->mVertices[surface.textureIndex].push_back(v2.x);
			thisptr->mVertices[surface.textureIndex].push_back(v2.y);
			thisptr->mVertices[surface.textureIndex].push_back(v2.z);
			thisptr->mUVs[surface.textureIndex].push_back(uv2.x);
			thisptr->mUVs[surface.textureIndex].push_back(uv2.y);

			// Do this only once per triangle instead of per point.
			// takes care of normals and tangents
			thisptr->mNormals[surface.textureIndex].push_back(normal.x);
			thisptr->mNormals[surface.textureIndex].push_back(normal.y);
			thisptr->mNormals[surface.textureIndex].push_back(normal.z);
			thisptr->mTangents[surface.textureIndex].push_back(tangent.x);
			thisptr->mTangents[surface.textureIndex].push_back(tangent.y);
			thisptr->mTangents[surface.textureIndex].push_back(tangent.z);

			// Up triangle count
			thisptr->mTriangleCount[surface.textureIndex]++;
		}
	}
}

//-----------------------------------------------------------------------------
// Dif C linkage Class
//-----------------------------------------------------------------------------

Dif::Dif() {

}

Dif::~Dif() {

}

bool Dif::read(const std::string &file) {
	std::ifstream stream(file, std::ios::binary);
	if (stream.fail())
		return false;

	DIF::DIF dif;
	dif.read(stream);
	stream.close();

	// parse the dif
	parseDif(this, dif);

	return true;
}

//-----------------------------------------------------------------------------
// C linkage API
//-----------------------------------------------------------------------------

extern "C" {

	void* createDif() {
		return new Dif();
	}

	void freeDif(void *dif) {
		if (dif != NULL)
			delete dif;
	}

	void readDif(void *dif, const char *file) {
		static_cast<Dif*>(dif)->read(file);
	}

	float* getVertices(void *dif, int materialId) {
		return &static_cast<Dif*>(dif)->mVertices[materialId][0];
	}

	int getTriangleCount(void *dif, int materialId) {
		return static_cast<Dif*>(dif)->mTriangleCount[materialId];
	}
}
