#include <iostream>
#include "objects/dif.h"

void printTriggers(DIF *dif) {
	for (int i = 0; i < dif->numTriggers; i ++) {
		Trigger *trigger = dif->trigger[i];
		std::cout << "new Trigger(" << trigger->name << ") {" << std::endl;
		std::cout << "   position = \"" << trigger->offset.x << " " << trigger->offset.y << " " << trigger->offset.z << "\";" << std::endl;
		std::cout << "   rotation = \"1 0 0 0\";" << std::endl;
		std::cout << "   scale = \"1 1 1 \";" << std::endl;
		std::cout << "   datablock = \"" << trigger->datablock <<  "\";" << std::endl;
		std::cout << "   polyhedron = \"" << (const char *)trigger->getPolyhedron() << "\";" << std::endl;
		for (int j = 0; j < trigger->properties.size; j ++) {
			std::cout << "      " << (const char *)trigger->properties.names[j] << " = \"" << (const char *)trigger->properties.values[j] << "\";" << std::endl;
		}
		std::cout << "};" << std::endl;
	}
}

int main(int argc, const char * argv[]) {
	std::filebuf fb;

	if (fb.open(argv[1], std::ios::in)) {
		std::istream stream(&fb);
		DIF *dif = new DIF(stream, IO::getPath(argv[1]));
		fb.close();

		printTriggers(dif);

		delete dif;
	}

	return 0;
}
