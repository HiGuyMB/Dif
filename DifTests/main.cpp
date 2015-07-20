#include <iostream>
#include <sstream>
#include <fstream>
#include "objects/dif.h"

void printTriggers(DIF *dif) {
	for (int i = 0; i < dif->numTriggers; i ++) {
		Trigger *trigger = dif->trigger[i];
		std::cout << "new Trigger(" << trigger->name << ") {" << std::endl;
		std::cout << "   position = \"" << trigger->offset.x << " " << trigger->offset.y << " " << trigger->offset.z << "\";" << std::endl;
		std::cout << "   rotation = \"1 0 0 0\";" << std::endl;
		std::cout << "   scale = \"1 1 1 \";" << std::endl;
		std::cout << "   datablock = \"" << trigger->datablock <<  "\";" << std::endl;
		std::cout << "   polyhedron = \"" << trigger->getPolyhedron() << "\";" << std::endl;
		for (auto it : trigger->properties) {
			std::cout << "      " << it.first << " = \"" << it.second << "\";" << std::endl;
		}
		std::cout << "};" << std::endl;
	}
}
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

			return fileStr.compare(difStr) == 0;
		}
	}
	return false;
}

int main(int argc, const char * argv[]) {
	return testEquality(argv[1]) ? 0 : 1;
}
