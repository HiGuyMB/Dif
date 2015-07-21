#include <iostream>
#include <sstream>
#include <fstream>
#include <dif/objects/dif.h>

void printTriggers(DIF::DIF dif) {
	for (DIF::Trigger trigger : dif.trigger) {
		std::cout << "new Trigger(" << trigger.name << ") {" << std::endl;
		std::cout << "   position = \"" << trigger.offset.x << " " << trigger.offset.y << " " << trigger.offset.z << "\";" << std::endl;
		std::cout << "   rotation = \"1 0 0 0\";" << std::endl;
		std::cout << "   scale = \"1 1 1 \";" << std::endl;
		std::cout << "   datablock = \"" << trigger.datablock <<  "\";" << std::endl;
		std::cout << "   polyhedron = \"" << trigger.getPolyhedron() << "\";" << std::endl;
		for (auto it : trigger.properties) {
			std::cout << "      " << it.first << " = \"" << it.second << "\";" << std::endl;
		}
		std::cout << "};" << std::endl;
	}
}

void printEntities(DIF::DIF dif) {
	for (DIF::GameEntity entity : dif.gameEntity) {
		std::cout << "new " << entity.gameClass << "() {" << std::endl;
		std::cout << "   position = \"" << entity.position.x << " " << entity.position.y << " " << entity.position.z << "\";" << std::endl;
		std::cout << "   rotation = \"1 0 0 0\";" << std::endl;
		std::cout << "   scale = \"1 1 1 \";" << std::endl;
		std::cout << "   datablock = \"" << entity.datablock <<  "\";" << std::endl;
		for (auto it : entity.properties) {
			std::cout << "      " << it.first << " = \"" << it.second << "\";" << std::endl;
		}
		std::cout << "};" << std::endl;
	}
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

int main(int argc, const char * argv[]) {
	//Read it into the dif
	DIF::DIF dif;
	if (readDif(argv[1], &dif)) {
		printTriggers(dif);
		printEntities(dif);
	}
	return testEquality(argv[1]) ? 0 : 1;
}
