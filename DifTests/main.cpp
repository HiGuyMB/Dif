#include <iostream>
#include "objects/dif.h"

int main(int argc, const char * argv[]) {
	std::filebuf fb;

	if (fb.open(argv[1], std::ios::in)) {
		std::istream stream(&fb);
		DIF *dif = new DIF(stream, IO::getPath(argv[1]));
		fb.close();

		for (int i = 0; i < dif->numDetailLevels; i ++) {
			Interior *interior = dif->interior[i];

			for (int j = 0; j < interior->materialName.size(); j ++) {
				String name = interior->materialName[j];

				std::cout << (const char *)name << std::endl;
			}
		}

		delete dif;
	}

	return 0;
}
