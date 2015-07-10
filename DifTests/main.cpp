#include <iostream>
#include "objects/dif.h"

int main(int argc, const char * argv[]) {
//	std::istream stream = fopen(argv[1], "r");
	DIF *dif;// = new DIF(stream, io->getPath(argv[1]));
//	fclose(file);

	for (int i = 0; i < dif->numDetailLevels; i ++) {
		Interior *interior = dif->interior[i];

		for (int j = 0; j < interior->numMaterials; j ++) {
			String name = interior->materialName[j];

			std::cout << (const char *)name << std::endl;
		}
	}

	delete dif;

	return 0;
}
