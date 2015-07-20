#include <iostream>
#include <sstream>
#include <fstream>
#include "objects/dif.h"

void printTriggers(DIF *dif) {
	for (Trigger trigger : dif->trigger) {
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

void printEntities(DIF *dif) {
	for (GameEntity entity : dif->gameEntity) {
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

bool testEquality(const char *file) {
	//Read the file to a stream
	std::ifstream inFile(file);
	std::stringstream inString;
	inString << inFile.rdbuf();

	//Read it into the dif
	std::filebuf fb;
	if (fb.open(file, std::ios::in)) {
		std::istream stream(&fb);
		DIF *dif = new DIF(stream);
		fb.close();

		std::ostringstream out;
		if (dif->write(out)) {
			//Check the two
			std::string fileStr = inString.str();
			std::string difStr = out.str();

			delete dif;
			return fileStr.compare(difStr) == 0;
		}
		delete dif;
	}
	return false;
}

int main(int argc, const char * argv[]) {
	//Read it into the dif
	std::filebuf fb;
	if (fb.open(argv[1], std::ios::in)) {
		std::istream stream(&fb);
		DIF *dif = new DIF(stream);
		fb.close();

		printTriggers(dif);
		printEntities(dif);
		delete dif;
	}
	return testEquality(argv[1]) ? 0 : 1;
}
