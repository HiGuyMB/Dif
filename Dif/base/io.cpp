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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "io.h"

#if 0
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

IO *IO::getIO() {
	static IO *gIO = NULL;

	if (gIO == NULL) {
		gIO = new IO;
	}
	return gIO;
}

IO::IO() {

}

IO::~IO() {
	
}

void IO::reverse(FILE **file, const U32 &bytes) {
	fpos_t pos = ftell(*file);
	pos -= bytes;
	fsetpos(*file, &pos);
}

bool PlaneF::read(std::istream &stream) {
	return
		io->read(stream, &x, "x") &&
		io->read(stream, &y, "x") &&
		io->read(stream, &z, "x") &&
		io->read(stream, &d, "x");
}

bool QuatF::read(std::istream &stream) {
	return
		io->read(stream, &w, "w") &&
		io->read(stream, &x, "x") &&
		io->read(stream, &y, "y") &&
		io->read(stream, &z, "z");
}

bool BoxF::read(std::istream &stream) {
	return
		io->read(stream, &minX, "minX") &&
		io->read(stream, &minY, "minY") &&
		io->read(stream, &minZ, "minZ") &&
		io->read(stream, &maxX, "maxX") &&
		io->read(stream, &maxY, "maxY") &&
		io->read(stream, &maxZ, "maxZ");
}

bool SphereF::read(std::istream &stream) {
	return
		io->read(stream, &x, "x") &&
		io->read(stream, &y, "y") &&
		io->read(stream, &z, "z") &&
		io->read(stream, &radius, "radius");
}

bool MatrixF::read(std::istream &stream) {
	return
		io->read(stream, &m[0], "m[0]") &&
		io->read(stream, &m[1], "m[1]") &&
		io->read(stream, &m[2], "m[2]") &&
		io->read(stream, &m[3], "m[3]") &&
		io->read(stream, &m[4], "m[4]") &&
		io->read(stream, &m[5], "m[5]") &&
		io->read(stream, &m[6], "m[6]") &&
		io->read(stream, &m[7], "m[7]") &&
		io->read(stream, &m[8], "m[8]") &&
		io->read(stream, &m[9], "m[9]") &&
		io->read(stream, &m[10], "m[10]") &&
		io->read(stream, &m[11], "m[11]") &&
		io->read(stream, &m[12], "m[12]") &&
		io->read(stream, &m[13], "m[13]") &&
		io->read(stream, &m[14], "m[14]") &&
		io->read(stream, &m[15], "m[15]");
}

bool String::read(std::istream &stream) {
	//<length><bytes>

	io->read(stream, (U8 *)&length, "length");
	data = new U8[length + 1];
	for (int i = 0; i < length; i ++) {
		io->read(stream, &(data[i]), "data");
	}
	//Null-terminate
	data[length] = 0;
	return true;
}

bool PNG::read(std::istream &stream) {
	U8 PNGFooter[8] = {0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
	data = new U8[LIGHT_MAP_SIZE];

	//I can't parse these, so I just read em all
	for (size = 0; ;size ++) {
		io->read(stream, &(data[size]), "data");
		if (size > 8 && memcmp(&data[size - 7], PNGFooter, 8) == 0)
			break;
	}
	size ++;

	return true;
}

bool Dictionary::read(std::istream &stream) {
	//<length>[<name><value>]...
	io->read(stream, &size, "size");
	names = new String[size];
	values = new String[size];

	for (int i = 0; i < size; i ++) {
		names[i] = String();
		values[i] = String();
		names[i].read(stream);
		values[i].read(stream);
	}

	return true;
}

//-----------------------------------------------------------------------------

bool PlaneF::write(std::ostream &stream) const {
	return
		io->write(stream, x, "x") &&
		io->write(stream, y, "y") &&
		io->write(stream, z, "z") &&
		io->write(stream, d, "d");
}

bool QuatF::write(std::ostream &stream) const {
	return
		io->write(stream, w, "w") &&
		io->write(stream, x, "x") &&
		io->write(stream, y, "y") &&
		io->write(stream, z, "z");
}

bool BoxF::write(std::ostream &stream) const {
	return
		io->write(stream, minX, "minX") &&
		io->write(stream, minY, "minY") &&
		io->write(stream, minZ, "minZ") &&
		io->write(stream, maxX, "maxX") &&
		io->write(stream, maxY, "maxY") &&
		io->write(stream, maxZ, "maxZ");
}

bool SphereF::write(std::ostream &stream) const {
	return
		io->write(stream, x, "x") &&
		io->write(stream, y, "y") &&
		io->write(stream, z, "z") &&
		io->write(stream, radius, "radius");
}

bool MatrixF::write(std::ostream &stream) const {
	return
		io->write(stream, m[0], "m[0]") &&
		io->write(stream, m[1], "m[1]") &&
		io->write(stream, m[2], "m[2]") &&
		io->write(stream, m[3], "m[3]") &&
		io->write(stream, m[4], "m[4]") &&
		io->write(stream, m[5], "m[5]") &&
		io->write(stream, m[6], "m[6]") &&
		io->write(stream, m[7], "m[7]") &&
		io->write(stream, m[8], "m[8]") &&
		io->write(stream, m[9], "m[9]") &&
		io->write(stream, m[10], "m[10]") &&
		io->write(stream, m[11], "m[11]") &&
		io->write(stream, m[12], "m[12]") &&
		io->write(stream, m[13], "m[13]") &&
		io->write(stream, m[14], "m[14]") &&
		io->write(stream, m[15], "m[15]");
}

bool String::write(std::ostream &stream) const {
	//<length><bytes>
	if (!io->write(stream, (U8)length, "length"))
		return false;
	for (int i = 0; i < length; i ++) {
		if (!io->write(stream, data[i], "data"))
			return false;
	}
	return true;
}

bool PNG::write(std::ostream &stream) const {
	//Basically dump out everything. Yeah.

	for (U32 i = 0; i < size; i ++) {
		if (!io->write(stream, data[i], "data"))
			return false;
	}
	return true;
}

bool Dictionary::write(std::ostream &stream) const {
	//<length>[<name><value>]...

	if (!io->write(stream, size, "size"))
		return false;
	for (int i = 0; i < size; i ++) {
		if (!names[i].write(stream) ||
		    !values[i].write(stream))
		return false;
	}

	return true;
}

bool IO::isfile(const String &file) {
	FILE *stream = fopen((const char *)file.data, "r");
	if (stream) {
		fclose(stream);
		return true;
	}
	return false;
}

U8 *IO::readFile(const String &file, U32 *length) {
	FILE *stream = fopen((const char *)file.data, "rb");

	if (!stream)
		return NULL;

	//Read length of file
	fseek(stream, 0L, SEEK_END);
	*length = (U32)ftell(stream);
	fseek(stream, 0L, SEEK_SET);

	U8 *data = new U8[*length + 1];
	fread(data, sizeof(U8), *length, stream);
	data[*length ++] = 0;

	fclose(stream);

	return data;
}

#ifdef _WIN32
#define DIR_SEP '\\'
#else
#define DIR_SEP '/'
#endif


String IO::getPath(const String &file) {
	S64 last = (S64)((U8 *)strrchr((const char *)file.data, DIR_SEP) - file.data);
	if (last > 0)
		return String(file, (S32)last);
	return String("");
}
String IO::getName(const String &file) {
	S64 last = (S64)((U8 *)strrchr((const char *)file.data, DIR_SEP) - file.data) + 1;
	printf("Last is %lld\n", last);
	if (last > 0)
		return String(file.data + last, file.length - (S32)last);
	return file;
}
String IO::getExtension(const String &file) {
	S64 last = (S64)((U8 *)strrchr((const char *)file.data, '.') - file.data) + 1;
	if (last > 0)
		return String(file.data + last, file.length - (S32)last);
	return String("");
}
