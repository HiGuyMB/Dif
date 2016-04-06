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

#include <dif/base/io.h>

DIF_NAMESPACE

bool PlaneF::read(std::istream &stream, Version &version) {
	return
		IO::read(stream, version, x, "x") &&
		IO::read(stream, version, y, "x") &&
		IO::read(stream, version, z, "x") &&
		IO::read(stream, version, d, "x");
}

bool QuatF::read(std::istream &stream, Version &version) {
	return
		IO::read(stream, version, w, "w") &&
		IO::read(stream, version, x, "x") &&
		IO::read(stream, version, y, "y") &&
		IO::read(stream, version, z, "z");
}

bool BoxF::read(std::istream &stream, Version &version) {
	return
		IO::read(stream, version, minX, "minX") &&
		IO::read(stream, version, minY, "minY") &&
		IO::read(stream, version, minZ, "minZ") &&
		IO::read(stream, version, maxX, "maxX") &&
		IO::read(stream, version, maxY, "maxY") &&
		IO::read(stream, version, maxZ, "maxZ");
}

bool SphereF::read(std::istream &stream, Version &version) {
	return
		IO::read(stream, version, x, "x") &&
		IO::read(stream, version, y, "y") &&
		IO::read(stream, version, z, "z") &&
		IO::read(stream, version, radius, "radius");
}

bool MatrixF::read(std::istream &stream, Version &version) {
	return
		IO::read(stream, version, m[0], "m[0]") &&
		IO::read(stream, version, m[1], "m[1]") &&
		IO::read(stream, version, m[2], "m[2]") &&
		IO::read(stream, version, m[3], "m[3]") &&
		IO::read(stream, version, m[4], "m[4]") &&
		IO::read(stream, version, m[5], "m[5]") &&
		IO::read(stream, version, m[6], "m[6]") &&
		IO::read(stream, version, m[7], "m[7]") &&
		IO::read(stream, version, m[8], "m[8]") &&
		IO::read(stream, version, m[9], "m[9]") &&
		IO::read(stream, version, m[10], "m[10]") &&
		IO::read(stream, version, m[11], "m[11]") &&
		IO::read(stream, version, m[12], "m[12]") &&
		IO::read(stream, version, m[13], "m[13]") &&
		IO::read(stream, version, m[14], "m[14]") &&
		IO::read(stream, version, m[15], "m[15]");
}

bool PNG::read(std::istream &stream, Version &version) {
	U8 PNGFooter[8] = {0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
	data = new U8[LIGHT_MAP_SIZE];

	//I can't parse these, so I just read em all
	for (size = 0; ;size ++) {
		IO::read(stream, version, (data[size]), "data");
		if (size > 8 && memcmp(&data[size - 7], PNGFooter, 8) == 0)
			break;
	}
	size ++;

	return true;
}

//-----------------------------------------------------------------------------

bool PlaneF::write(std::ostream &stream, Version version) const {
	return
		IO::write(stream, version, x, "x") &&
		IO::write(stream, version, y, "y") &&
		IO::write(stream, version, z, "z") &&
		IO::write(stream, version, d, "d");
}

bool QuatF::write(std::ostream &stream, Version version) const {
	return
		IO::write(stream, version, w, "w") &&
		IO::write(stream, version, x, "x") &&
		IO::write(stream, version, y, "y") &&
		IO::write(stream, version, z, "z");
}

bool BoxF::write(std::ostream &stream, Version version) const {
	return
		IO::write(stream, version, minX, "minX") &&
		IO::write(stream, version, minY, "minY") &&
		IO::write(stream, version, minZ, "minZ") &&
		IO::write(stream, version, maxX, "maxX") &&
		IO::write(stream, version, maxY, "maxY") &&
		IO::write(stream, version, maxZ, "maxZ");
}

bool SphereF::write(std::ostream &stream, Version version) const {
	return
		IO::write(stream, version, x, "x") &&
		IO::write(stream, version, y, "y") &&
		IO::write(stream, version, z, "z") &&
		IO::write(stream, version, radius, "radius");
}

bool MatrixF::write(std::ostream &stream, Version version) const {
	return
		IO::write(stream, version, m[0], "m[0]") &&
		IO::write(stream, version, m[1], "m[1]") &&
		IO::write(stream, version, m[2], "m[2]") &&
		IO::write(stream, version, m[3], "m[3]") &&
		IO::write(stream, version, m[4], "m[4]") &&
		IO::write(stream, version, m[5], "m[5]") &&
		IO::write(stream, version, m[6], "m[6]") &&
		IO::write(stream, version, m[7], "m[7]") &&
		IO::write(stream, version, m[8], "m[8]") &&
		IO::write(stream, version, m[9], "m[9]") &&
		IO::write(stream, version, m[10], "m[10]") &&
		IO::write(stream, version, m[11], "m[11]") &&
		IO::write(stream, version, m[12], "m[12]") &&
		IO::write(stream, version, m[13], "m[13]") &&
		IO::write(stream, version, m[14], "m[14]") &&
		IO::write(stream, version, m[15], "m[15]");
}

bool PNG::write(std::ostream &stream, Version version) const {
	//Basically dump out everything. Yeah.

	for (U32 i = 0; i < size; i ++) {
		if (!IO::write(stream, version, data[i], "data"))
			return false;
	}
	return true;
}

DIF_NAMESPACE_END
