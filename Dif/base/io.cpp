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

#include "io.h"

bool PlaneF::read(std::istream &stream) {
	return
		IO::read(stream, &x, "x") &&
		IO::read(stream, &y, "x") &&
		IO::read(stream, &z, "x") &&
		IO::read(stream, &d, "x");
}

bool QuatF::read(std::istream &stream) {
	return
		IO::read(stream, &w, "w") &&
		IO::read(stream, &x, "x") &&
		IO::read(stream, &y, "y") &&
		IO::read(stream, &z, "z");
}

bool BoxF::read(std::istream &stream) {
	return
		IO::read(stream, &minX, "minX") &&
		IO::read(stream, &minY, "minY") &&
		IO::read(stream, &minZ, "minZ") &&
		IO::read(stream, &maxX, "maxX") &&
		IO::read(stream, &maxY, "maxY") &&
		IO::read(stream, &maxZ, "maxZ");
}

bool SphereF::read(std::istream &stream) {
	return
		IO::read(stream, &x, "x") &&
		IO::read(stream, &y, "y") &&
		IO::read(stream, &z, "z") &&
		IO::read(stream, &radius, "radius");
}

bool MatrixF::read(std::istream &stream) {
	return
		IO::read(stream, &m[0], "m[0]") &&
		IO::read(stream, &m[1], "m[1]") &&
		IO::read(stream, &m[2], "m[2]") &&
		IO::read(stream, &m[3], "m[3]") &&
		IO::read(stream, &m[4], "m[4]") &&
		IO::read(stream, &m[5], "m[5]") &&
		IO::read(stream, &m[6], "m[6]") &&
		IO::read(stream, &m[7], "m[7]") &&
		IO::read(stream, &m[8], "m[8]") &&
		IO::read(stream, &m[9], "m[9]") &&
		IO::read(stream, &m[10], "m[10]") &&
		IO::read(stream, &m[11], "m[11]") &&
		IO::read(stream, &m[12], "m[12]") &&
		IO::read(stream, &m[13], "m[13]") &&
		IO::read(stream, &m[14], "m[14]") &&
		IO::read(stream, &m[15], "m[15]");
}

bool PNG::read(std::istream &stream) {
	U8 PNGFooter[8] = {0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
	data = new U8[LIGHT_MAP_SIZE];

	//I can't parse these, so I just read em all
	for (size = 0; ;size ++) {
		IO::read(stream, &(data[size]), "data");
		if (size > 8 && memcmp(&data[size - 7], PNGFooter, 8) == 0)
			break;
	}
	size ++;

	return true;
}

//-----------------------------------------------------------------------------

bool PlaneF::write(std::ostream &stream) const {
	return
		IO::write(stream, x, "x") &&
		IO::write(stream, y, "y") &&
		IO::write(stream, z, "z") &&
		IO::write(stream, d, "d");
}

bool QuatF::write(std::ostream &stream) const {
	return
		IO::write(stream, w, "w") &&
		IO::write(stream, x, "x") &&
		IO::write(stream, y, "y") &&
		IO::write(stream, z, "z");
}

bool BoxF::write(std::ostream &stream) const {
	return
		IO::write(stream, minX, "minX") &&
		IO::write(stream, minY, "minY") &&
		IO::write(stream, minZ, "minZ") &&
		IO::write(stream, maxX, "maxX") &&
		IO::write(stream, maxY, "maxY") &&
		IO::write(stream, maxZ, "maxZ");
}

bool SphereF::write(std::ostream &stream) const {
	return
		IO::write(stream, x, "x") &&
		IO::write(stream, y, "y") &&
		IO::write(stream, z, "z") &&
		IO::write(stream, radius, "radius");
}

bool MatrixF::write(std::ostream &stream) const {
	return
		IO::write(stream, m[0], "m[0]") &&
		IO::write(stream, m[1], "m[1]") &&
		IO::write(stream, m[2], "m[2]") &&
		IO::write(stream, m[3], "m[3]") &&
		IO::write(stream, m[4], "m[4]") &&
		IO::write(stream, m[5], "m[5]") &&
		IO::write(stream, m[6], "m[6]") &&
		IO::write(stream, m[7], "m[7]") &&
		IO::write(stream, m[8], "m[8]") &&
		IO::write(stream, m[9], "m[9]") &&
		IO::write(stream, m[10], "m[10]") &&
		IO::write(stream, m[11], "m[11]") &&
		IO::write(stream, m[12], "m[12]") &&
		IO::write(stream, m[13], "m[13]") &&
		IO::write(stream, m[14], "m[14]") &&
		IO::write(stream, m[15], "m[15]");
}

bool PNG::write(std::ostream &stream) const {
	//Basically dump out everything. Yeah.

	for (U32 i = 0; i < size; i ++) {
		if (!IO::write(stream, data[i], "data"))
			return false;
	}
	return true;
}

const std::string IO::getPath(const std::string &file, const char &seperator) {
	std::string::size_type last = file.find_last_of(seperator);
	if (last != std::string::npos)
		return file.substr(0, last);
	return "";
}
const std::string IO::getName(const std::string &file, const char &seperator) {
	std::string::size_type last = file.find_last_of(seperator) + 1;
	if (last != std::string::npos)
		return file.substr(last);
	return file;
}
const std::string IO::getExtension(const std::string &file) {
	std::string::size_type last = file.find_last_of('.') + 1;
	if (last != std::string::npos)
		return file.substr(0, last);
	return "";
}
const std::string IO::getBase(const std::string &file, const char &seperator) {
	std::string::size_type slash = file.find_last_of(seperator) + 1;
	std::string::size_type dot = file.find_last_of('.');
	if (slash != std::string::npos) {
		if (dot != std::string::npos)
			return file.substr(slash, dot - slash);
		else
			return file.substr(slash);
	}
	return file;
}
