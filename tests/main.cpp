#include <iostream>
#include <sstream>
#include <fstream>
#include <dif/objects/dif.h>

void printTriggers(const DIF::DIF &dif) {
	for (const DIF::Trigger &trigger : dif.trigger) {
		std::cout << "new Trigger(" << trigger.name << ") {" << std::endl;
		std::cout << "   position = \"" << trigger.offset.x << " " << trigger.offset.y << " " << trigger.offset.z << "\";" << std::endl;
		std::cout << "   rotation = \"1 0 0 0\";" << std::endl;
		std::cout << "   scale = \"1 1 1 \";" << std::endl;
		std::cout << "   datablock = \"" << trigger.datablock <<  "\";" << std::endl;
		std::cout << "   polyhedron = \"" << trigger.getPolyhedron() << "\";" << std::endl;
		for (const auto &it : trigger.properties) {
			std::cout << "      " << it.first << " = \"" << it.second << "\";" << std::endl;
		}
		std::cout << "};" << std::endl;
	}
}

void printEntities(const DIF::DIF &dif) {
	for (const DIF::GameEntity &entity : dif.gameEntity) {
		std::cout << "new " << entity.gameClass << "() {" << std::endl;
		std::cout << "   position = \"" << entity.position.x << " " << entity.position.y << " " << entity.position.z << "\";" << std::endl;
		std::cout << "   rotation = \"1 0 0 0\";" << std::endl;
		std::cout << "   scale = \"1 1 1 \";" << std::endl;
		std::cout << "   datablock = \"" << entity.datablock <<  "\";" << std::endl;
		for (const auto &it : entity.properties) {
			std::cout << "      " << it.first << " = \"" << it.second << "\";" << std::endl;
		}
		std::cout << "};" << std::endl;
	}
}

void nullSurfaces(DIF::DIF &dif) {
	for (DIF::Interior &interior : dif.interior) {
		for (DIF::Interior::Surface &surface : interior.surface) {
			//Get average position
			DIF::Point3F position;

			for (DIF::U32 i = 0; i < surface.windingCount; i ++) {
				DIF::U32 index = i + surface.windingStart;
				DIF::Point3F point = interior.point[interior.index[index]];
				position += point;
			}

			position /= surface.windingCount;
			if (position.z > 50) {
				surface.textureIndex = 0;
			}

			std::cout << "Average position for surface: " << position.x << ", " << position.y << ", " << position.z << std::endl;
		}
	}
}

void exportObj(const DIF::Interior &dif, const char *outFile) {
	struct Face {
		DIF::U32 vertIndex[3];
		DIF::U32 texCoordIndex[3];
		DIF::U32 normalIndex;
	};

	std::vector<DIF::Point3F> vertices;
	std::vector<DIF::Point2F> texCoords;
	std::vector<DIF::Point3F> normals;
	std::vector<Face> faces;

	vertices.insert(vertices.end(), dif.point.begin(), dif.point.end());
	normals.insert(normals.end(), dif.normal.begin(), dif.normal.end());

	for (const DIF::Interior::Surface &surface : dif.surface) {
		DIF::Point3F normal = dif.normal[dif.plane[surface.planeIndex].normalIndex];
		if (surface.planeFlipped) {
			normal *= -1;
		}

		//New and improved rendering with actual Triangle Strips this time
		for (DIF::U32 j = surface.windingStart + 2; j < surface.windingStart + surface.windingCount; j ++) {
			Face f;

			if ((j - (surface.windingStart + 2)) % 2 == 0) {
				f.vertIndex[0] = dif.index[j];
				f.vertIndex[1] = dif.index[j - 1];
				f.vertIndex[2] = dif.index[j - 2];
			} else {
				f.vertIndex[0] = dif.index[j - 2];
				f.vertIndex[1] = dif.index[j - 1];
				f.vertIndex[2] = dif.index[j];
			}

			f.normalIndex = dif.plane[surface.planeIndex].normalIndex;

			DIF::Interior::TexGenEq texGen = dif.texGenEq[surface.texGenIndex];

			const DIF::Point3F &pt0 = dif.point[f.vertIndex[0]];
			const DIF::Point3F &pt1 = dif.point[f.vertIndex[0]];
			const DIF::Point3F &pt2 = dif.point[f.vertIndex[0]];

			DIF::Point2F coord0(pt0.x * texGen.planeX.x + pt0.y * texGen.planeX.y + pt0.z * texGen.planeX.z + texGen.planeX.d,
								pt0.x * texGen.planeY.x + pt0.y * texGen.planeY.y + pt0.z * texGen.planeY.z + texGen.planeY.d);
			DIF::Point2F coord1(pt1.x * texGen.planeX.x + pt1.y * texGen.planeX.y + pt1.z * texGen.planeX.z + texGen.planeX.d,
								pt1.x * texGen.planeY.x + pt1.y * texGen.planeY.y + pt1.z * texGen.planeY.z + texGen.planeY.d);
			DIF::Point2F coord2(pt2.x * texGen.planeX.x + pt2.y * texGen.planeX.y + pt2.z * texGen.planeX.z + texGen.planeX.d,
								pt2.x * texGen.planeY.x + pt2.y * texGen.planeY.y + pt2.z * texGen.planeY.z + texGen.planeY.d);

			f.texCoordIndex[0] = texCoords.size();
			texCoords.push_back(coord0);
			f.texCoordIndex[1] = texCoords.size();
			texCoords.push_back(coord1);
			f.texCoordIndex[2] = texCoords.size();
			texCoords.push_back(coord2);

			faces.push_back(f);
		}
	}

	//Obj file
	std::ofstream out(outFile);

	//Vertex list
	for (const DIF::Point3F &point : vertices) {
		out << "v " << -point.x << " " << point.z << " " << point.y << "\n";
	}

	//Texture coords
	for (const DIF::Point2F &texCoord : texCoords) {
		out << "vf " << -texCoord.x << " " << texCoord.y << "\n";
	}

	//Normals
	for (const DIF::Point3F &normal : normals) {
		out << "vn " << -normal.x << " " << normal.z << " " << normal.y << "\n";
	}

	//Faces
	for (const Face &face : faces) {
		out << "f";
		out << " " << (face.vertIndex[0] + 1) << "/" << (face.texCoordIndex[0] + 1) << "/" << (face.normalIndex + 1);
		out << " " << (face.vertIndex[1] + 1) << "/" << (face.texCoordIndex[1] + 1) << "/" << (face.normalIndex + 1);
		out << " " << (face.vertIndex[2] + 1) << "/" << (face.texCoordIndex[2] + 1) << "/" << (face.normalIndex + 1);
		out << "\n";
	}

	out.close();
}

void exportJSON(const DIF::Interior &dif, const char *outFile) {
	struct Face {
		DIF::U32 vertIndex[3];
		DIF::U32 texCoordIndex[3];
		DIF::U32 normalIndex;
		DIF::U32 tangentIndex;
		DIF::U32 bitangentIndex;
	};

	struct TextureGroup {
		std::vector<Face> faces;
	};

	std::vector<DIF::Point3F> vertices;
	std::vector<DIF::Point2F> texCoords;
	std::vector<DIF::Point3F> normals;
	std::vector<DIF::Point3F> tangents;
	std::vector<DIF::Point3F> bitangents;

	std::map<DIF::U16, TextureGroup> texGroups;

	vertices.insert(vertices.end(), dif.point.begin(), dif.point.end());
	normals.insert(normals.end(), dif.normal.begin(), dif.normal.end());

	for (const DIF::Interior::Surface &surface : dif.surface) {
		DIF::Point3F normal = dif.normal[dif.plane[surface.planeIndex].normalIndex];
		if (surface.planeFlipped) {
			normal *= -1;
		}
		normals.push_back(normal);

		//New and improved rendering with actual Triangle Strips this time
		for (DIF::U32 j = surface.windingStart + 2; j < surface.windingStart + surface.windingCount; j ++) {
			Face f;

			if ((j - (surface.windingStart + 2)) % 2 == 0) {
				f.vertIndex[0] = dif.index[j];
				f.vertIndex[1] = dif.index[j - 1];
				f.vertIndex[2] = dif.index[j - 2];
			} else {
				f.vertIndex[0] = dif.index[j - 2];
				f.vertIndex[1] = dif.index[j - 1];
				f.vertIndex[2] = dif.index[j];
			}

			if (surface.planeFlipped) {
				f.normalIndex = normals.size() - 1;
			} else {
				f.normalIndex = dif.plane[surface.planeIndex].normalIndex;
			}

			DIF::Interior::TexGenEq texGen = dif.texGenEq[surface.texGenIndex];

			const DIF::Point3F &pt0 = dif.point[f.vertIndex[0]];
			const DIF::Point3F &pt1 = dif.point[f.vertIndex[1]];
			const DIF::Point3F &pt2 = dif.point[f.vertIndex[2]];

			DIF::Point2F coord0(pt0.x * texGen.planeX.x + pt0.y * texGen.planeX.y + pt0.z * texGen.planeX.z + texGen.planeX.d,
								pt0.x * texGen.planeY.x + pt0.y * texGen.planeY.y + pt0.z * texGen.planeY.z + texGen.planeY.d);
			DIF::Point2F coord1(pt1.x * texGen.planeX.x + pt1.y * texGen.planeX.y + pt1.z * texGen.planeX.z + texGen.planeX.d,
								pt1.x * texGen.planeY.x + pt1.y * texGen.planeY.y + pt1.z * texGen.planeY.z + texGen.planeY.d);
			DIF::Point2F coord2(pt2.x * texGen.planeX.x + pt2.y * texGen.planeX.y + pt2.z * texGen.planeX.z + texGen.planeX.d,
								pt2.x * texGen.planeY.x + pt2.y * texGen.planeY.y + pt2.z * texGen.planeY.z + texGen.planeY.d);

			f.texCoordIndex[0] = texCoords.size();
			texCoords.push_back(coord0);
			f.texCoordIndex[1] = texCoords.size();
			texCoords.push_back(coord1);
			f.texCoordIndex[2] = texCoords.size();
			texCoords.push_back(coord2);

			DIF::Point3F deltaPos1 = pt1 - pt0;
			DIF::Point3F deltaPos2 = pt2 - pt0;
			DIF::Point2F deltaUV1 = coord1 - coord0;
			DIF::Point2F deltaUV2 = coord2 - coord0;

			DIF::F32 r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

			DIF::Point3F tangent   = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
			DIF::Point3F bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

			tangent = (tangent - (normal * normal.dot(tangent))).normalize();
			if (normal.cross(tangent).dot(bitangent) < 0.0f) {
				tangent *= -1.0f;
			}

			tangent = tangent.normalize();
			bitangent = bitangent.normalize();

			f.tangentIndex = tangents.size();
			tangents.push_back(tangent);
			f.bitangentIndex = bitangents.size();
			bitangents.push_back(bitangent);

			texGroups[surface.textureIndex].faces.push_back(f);
		}
	}

#define NEWLINE "\n"
//#define NEWLINE ""

	//Obj file
	std::ofstream out(outFile);

	out << "nan=0;model={" NEWLINE;
	out << "textures:[" NEWLINE;

	DIF::U32 start = 0;
	for (DIF::U32 i = 0; i < dif.materialName.size(); i ++) {
		if (i > 0)
			out << "," NEWLINE;

		out << "{texture:\"";
		out << dif.materialName[i];
		out << "\",start:";
		out << start;
		out << ",count:";
		out << texGroups[i].faces.size();
		out << "}";

		start += texGroups[i].faces.size();
	}

	out << "]," NEWLINE;
	out << "faces:[" NEWLINE;

	for (DIF::U32 i = 0; i < dif.materialName.size(); i ++) {
		const TextureGroup &group = texGroups[i];

		if (i > 0 && texGroups[i - 1].faces.size() > 0)
			out << "," NEWLINE;

		for (DIF::U32 j = 0; j < group.faces.size(); j ++) {
			const Face &face = group.faces[j];

			const DIF::Point3F &point0    = vertices[face.vertIndex[0]];
			const DIF::Point3F &point1    = vertices[face.vertIndex[1]];
			const DIF::Point3F &point2    = vertices[face.vertIndex[2]];
			const DIF::Point2F &texCoord0 = texCoords[face.texCoordIndex[0]];
			const DIF::Point2F &texCoord1 = texCoords[face.texCoordIndex[1]];
			const DIF::Point2F &texCoord2 = texCoords[face.texCoordIndex[2]];
			const DIF::Point3F &normal    = normals[face.normalIndex];
			const DIF::Point3F &tangent   = tangents[face.tangentIndex];
			const DIF::Point3F &bitangent = bitangents[face.bitangentIndex];

			if (j > 0)
				out << "," NEWLINE;
			out << point0.x << "," << point0.y << "," << point0.z << ",";
			out << texCoord0.x << "," << texCoord0.y << ",";
			out << normal.x << "," << normal.y << "," << normal.z << ",";
			out << tangent.x << "," << tangent.y << "," << tangent.z << ",";
			out << bitangent.x << "," << bitangent.y << "," << bitangent.z << "," NEWLINE;

			out << point1.x << "," << point1.y << "," << point1.z << ",";
			out << texCoord1.x << "," << texCoord1.y << ",";
			out << normal.x << "," << normal.y << "," << normal.z << ",";
			out << tangent.x << "," << tangent.y << "," << tangent.z << ",";
			out << bitangent.x << "," << bitangent.y << "," << bitangent.z << "," NEWLINE;

			out << point2.x << "," << point2.y << "," << point2.z << ",";
			out << texCoord2.x << "," << texCoord2.y << ",";
			out << normal.x << "," << normal.y << "," << normal.z << ",";
			out << tangent.x << "," << tangent.y << "," << tangent.z << ",";
			out << bitangent.x << "," << bitangent.y << "," << bitangent.z;
		}
	}

	out << "]" NEWLINE;
	out << "}" NEWLINE;
	
	out.close();
}

bool readDif(const char *file, DIF::DIF *dif) {
	std::ifstream stream(file);
	if (stream.good()) {
		dif->read(stream);
		stream.close();
		return true;
	}
	return false;
}

bool testEquality(const char *file) {
	DIF::DIF dif;
	//Make sure we can actually read/write the dif first
	if (readDif(file, &dif)) {
		std::ostringstream out;
		if (dif.write(out)) {
			out.flush();

			//Take the written output of the DIF and feed it back into itself
			std::stringstream in;
			in << out.str();

			//Clear the output stream so we can reuse it
			out.clear();
			if (dif.read(in) && dif.write(out)) {
				//Read the two strings from their streams
				std::string fileStr = in.str();
				std::string difStr = out.str();

				//Check the two strings for equality
				for (auto i = 0; i < fileStr.size(); i ++) {
					if (fileStr[i] != difStr[i]) {
						//Inconsistency
						std::cout << "DIF output inconsistency starting at offset " << i << " (0x" << std::hex << i << std::dec << ")" << std::endl;
						return false;
					}
				}

				//No error? They're the same
				return true;
			}
		}
	}
	//Couldn't read/write somewhere along the lines
	return false;
}

bool convertDif(const char *file) {
	DIF::DIF dif;
	if (readDif(file, &dif)) {
		//Save it again
		std::ofstream out(file);
		return dif.write(out);
	}
	return false;
}

bool scaleTexture(DIF::DIF &dif, const std::string &textureName, const DIF::Point2F &scale) {
	for (DIF::Interior &interior : dif.interior) {
		std::vector<DIF::U32> texGensToChange;

		for (DIF::Interior::Surface &surface : interior.surface) {
			if (interior.materialName[surface.textureIndex] != textureName)
				continue;

			if (std::find(texGensToChange.begin(), texGensToChange.end(), surface.texGenIndex) == texGensToChange.end()) {
				texGensToChange.push_back(surface.texGenIndex);
			}
		}

		for (DIF::U32 index : texGensToChange) {
			DIF::Interior::TexGenEq &texGen = interior.texGenEq[index];
			texGen.planeX.x *= scale.x;
			texGen.planeX.y *= scale.x;
			texGen.planeX.z *= scale.x;
			texGen.planeX.d *= scale.x;
			texGen.planeY.x *= scale.y;
			texGen.planeY.y *= scale.y;
			texGen.planeY.z *= scale.y;
			texGen.planeX.d *= scale.x;
		}
	}

	return true;
}

void printTextures(DIF::DIF &dif) {
	for (DIF::Interior &interior : dif.interior) {
		for (const std::string &name : interior.materialName) {
			printf("%s\n", name.c_str());
		}
	}
	for (DIF::Interior &interior : dif.subObject) {
		for (const std::string &name : interior.materialName) {
			printf("%s\n", name.c_str());
		}
	}
}

int main(int argc, const char * argv[]) {
	if (argc > 2 && strcmp(argv[1], "--null") == 0) {
		DIF::DIF dif;
		if (readDif(argv[2], &dif)) {
			nullSurfaces(dif);
			std::ofstream out(argv[3]);
			return dif.write(out) ? EXIT_SUCCESS : EXIT_FAILURE;
		}
	}
	if (argc > 2 && strcmp(argv[1], "--convert") == 0) {
		//Convert the DIF
		return convertDif(argv[2]) ? EXIT_SUCCESS : EXIT_FAILURE;
	}
	if (argc > 3 && strcmp(argv[1], "--export") == 0) {
		DIF::DIF dif;
		if (readDif(argv[2], &dif)) {
			exportObj(dif.interior[0], argv[3]);
		}
		return EXIT_SUCCESS;
	}
	if (argc > 3 && strcmp(argv[1], "--json") == 0) {
		DIF::DIF dif;
		if (readDif(argv[2], &dif)) {
			exportJSON(dif.interior[0], argv[3]);
		}
		return EXIT_SUCCESS;
	}
	if (argc > 5 && strcmp(argv[1], "--scale") == 0) {
		std::string textureName = argv[2];
		DIF::Point2F scale(atof(argv[3]), atof(argv[4]));
		DIF::DIF dif;
		if (readDif(argv[5], &dif)) {
			scaleTexture(dif, textureName, scale);
			std::ofstream out(argv[5]);
			return dif.write(out) ? EXIT_SUCCESS : EXIT_FAILURE;
		}
		return EXIT_FAILURE;
	}
	if (argc > 2 && strcmp(argv[1], "--textures") == 0) {
		for (DIF::U32 i = 2; i < argc; i ++) {
			DIF::DIF dif;
			if (readDif(argv[i], &dif)) {
				printTextures(dif);
			}
		}
		return EXIT_SUCCESS;
	}

	//Read it into the dif
	DIF::DIF dif;
	if (readDif(argv[1], &dif)) {
		std::cout << "Dif information for " << argv[1] << std::endl;
		std::cout << "   Interior File Version: 44" << std::endl;

		std::cout << "   Interior Count: " << std::to_string(dif.interior.size()) << std::endl;

		for (DIF::U32 i = 0; i < dif.interior.size(); i ++) {
			const DIF::Interior &interior = dif.interior[i];
			std::cout << "      Interior " << std::to_string(i) << std::endl;
			std::cout << "      Version " << std::to_string(interior.interiorFileVersion) << std::endl;
			std::cout << "      Vertex Count " << std::to_string(interior.point.size()) << std::endl;
			std::cout << "      Plane Count " << std::to_string(interior.plane.size()) << std::endl;
			std::cout << "      Surface Count " << std::to_string(interior.surface.size()) << std::endl;
		}

		std::cout << "   Pathed Interior Count: " << std::to_string(dif.subObject.size()) << std::endl;

		for (DIF::U32 i = 0; i < dif.subObject.size(); i ++) {
			const DIF::Interior &interior = dif.subObject[i];
			std::cout << "      Interior " << std::to_string(i) << std::endl;
			std::cout << "      Version " << std::to_string(interior.interiorFileVersion) << std::endl;
			std::cout << "      Vertex Count " << std::to_string(interior.point.size()) << std::endl;
			std::cout << "      Plane Count " << std::to_string(interior.plane.size()) << std::endl;
			std::cout << "      Surface Count " << std::to_string(interior.surface.size()) << std::endl;
		}

		printTriggers(dif);
	}
	return EXIT_SUCCESS;
}
