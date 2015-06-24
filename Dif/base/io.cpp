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

bool IO::read(FILE *file, U64 *value, const String &name) {
	if (feof(file))
		return false;
	fpos_t pos;
	fgetpos(file, &pos);
	fread(value, sizeof(*value), 1, file);

	DEBUG_PRINT("Read U64 (%s) 0x%08llX %lld: 0x%016llX / %llu\n", (const char *)name, pos, pos, swapEndianness(*value), *value);
	return true;
}
bool IO::read(FILE *file, U32 *value, const String &name) {
	if (feof(file))
		return false;
	fpos_t pos;
	fgetpos(file, &pos);
	fread(value, sizeof(*value), 1, file);

	DEBUG_PRINT("Read U32 (%s) 0x%08llX %lld: 0x%08X / %u\n", (const char *)name, pos, pos, swapEndianness(*value), *value);
	return true;
}
bool IO::read(FILE *file, U16 *value, const String &name) {
	if (feof(file))
		return false;
	fpos_t pos;
	fgetpos(file, &pos);
	fread(value, sizeof(*value), 1, file);

	DEBUG_PRINT("Read U16 (%s) 0x%08llX %lld: 0x%04hX / %hu\n", (const char *)name, pos, pos, swapEndianness(*value), *value);
	return true;
}
bool IO::read(FILE *file, U8 *value, const String &name) {
	if (feof(file))
		return false;
	fpos_t pos;
	fgetpos(file, &pos);
	fread(value, sizeof(*value), 1, file);

	DEBUG_PRINT("Read U8 (%s) 0x%08llX %lld: 0x%02hhX / %u\n", (const char *)name, pos, pos, *value, *value);
	return true;
}
bool IO::read(FILE *file, F32 *value, const String &name) {
	if (feof(file))
		return false;
	fpos_t pos;
	fgetpos(file, &pos);
	fread(value, sizeof(*value), 1, file);

	DEBUG_PRINT("Read F32 (%s) 0x%08llX %lld: 0x%08X %f\n", (const char *)name, pos, pos, swapEndianness(*(U32 *)value), *value);
	return true;
}

//Lazy!
bool IO::read(FILE *file, S64 *value, const String &name) { return read(file, (U64 *)value, name); }
bool IO::read(FILE *file, S32 *value, const String &name) { return read(file, (U32 *)value, name); }
bool IO::read(FILE *file, S16 *value, const String &name) { return read(file, (U16 *)value, name); }
bool IO::read(FILE *file, S8  *value, const String &name) { return read(file, (U8  *)value, name); }

bool PlaneF::read(FILE *file) {
	return
		io->read(file, &x, "x") &&
		io->read(file, &y, "x") &&
		io->read(file, &z, "x") &&
		io->read(file, &d, "x");
}

bool QuatF::read(FILE *file) {
	return
		io->read(file, &w, "w") &&
		io->read(file, &x, "x") &&
		io->read(file, &y, "y") &&
		io->read(file, &z, "z");
}

bool BoxF::read(FILE *file) {
	return
		io->read(file, &minX, "minX") &&
		io->read(file, &minY, "minY") &&
		io->read(file, &minZ, "minZ") &&
		io->read(file, &maxX, "maxX") &&
		io->read(file, &maxY, "maxY") &&
		io->read(file, &maxZ, "maxZ");
}

bool SphereF::read(FILE *file) {
	return
		io->read(file, &x, "x") &&
		io->read(file, &y, "y") &&
		io->read(file, &z, "z") &&
		io->read(file, &radius, "radius");
}

bool MatrixF::read(FILE *file) {
	return
		io->read(file, &m[0], "m[0]") &&
		io->read(file, &m[1], "m[1]") &&
		io->read(file, &m[2], "m[2]") &&
		io->read(file, &m[3], "m[3]") &&
		io->read(file, &m[4], "m[4]") &&
		io->read(file, &m[5], "m[5]") &&
		io->read(file, &m[6], "m[6]") &&
		io->read(file, &m[7], "m[7]") &&
		io->read(file, &m[8], "m[8]") &&
		io->read(file, &m[9], "m[9]") &&
		io->read(file, &m[10], "m[10]") &&
		io->read(file, &m[11], "m[11]") &&
		io->read(file, &m[12], "m[12]") &&
		io->read(file, &m[13], "m[13]") &&
		io->read(file, &m[14], "m[14]") &&
		io->read(file, &m[15], "m[15]");
}

bool String::read(FILE *file) {
	//<length><bytes>

	io->read(file, (U8 *)&length, "length");
	data = new U8[length + 1];
	for (int i = 0; i < length; i ++) {
		io->read(file, &(data[i]), "data");
	}
	//Null-terminate
	data[length] = 0;
	return true;
}

bool PNG::read(FILE *file) {
	U8 PNGFooter[8] = {0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
	data = new U8[LIGHT_MAP_SIZE];

	//I can't parse these, so I just read em all
	for (size = 0; ;size ++) {
		io->read(file, &(data[size]), "data");
		if (size > 8 && memcmp(&data[size - 7], PNGFooter, 8) == 0)
			break;
	}
	size ++;

	return true;
}

bool Dictionary::read(FILE *file) {
	//<length>[<name><value>]...
	io->read(file, &size, "size");
	names = new String[size];
	values = new String[size];

	for (int i = 0; i < size; i ++) {
		names[i] = String();
		values[i] = String();
		names[i].read(file);
		values[i].read(file);
	}

	return true;
}

//-----------------------------------------------------------------------------

bool IO::write(FILE *file, const U64 &value, const String &name) {
	fpos_t pos;
	fgetpos(file, &pos);
	DEBUG_PRINT("Write U64 (%s) 0x%08llX %lld: 0x%016llX / %llu\n", (const char *)name, pos, pos, swapEndianness(value), value);
	fwrite(&value, sizeof(value), 1, file);
	return true;
}
bool IO::write(FILE *file, const U32 &value, const String &name) {
	fpos_t pos;
	fgetpos(file, &pos);
	DEBUG_PRINT("Write U32 (%s) 0x%08llX %lld: 0x%08X / %u\n", (const char *)name, pos, pos, swapEndianness(value), value);
	fwrite(&value, sizeof(value), 1, file);
	return true;
}
bool IO::write(FILE *file, const U16 &value, const String &name) {
	fpos_t pos;
	fgetpos(file, &pos);
	DEBUG_PRINT("Write U16 (%s) 0x%08llX %lld: 0x%04hX / %hu\n", (const char *)name, pos, pos, swapEndianness(value), value);
	fwrite(&value, sizeof(value), 1, file);
	return true;
}
bool IO::write(FILE *file, const U8 &value, const String &name) {
	fpos_t pos;
	fgetpos(file, &pos);
	DEBUG_PRINT("Write U8 (%s) 0x%08llX %lld: 0x%02hhX / %u\n", (const char *)name, pos, pos, value, value);
	fwrite(&value, sizeof(value), 1, file);
	return true;
}
bool IO::write(FILE *file, const F32 &value, const String &name) {
	fpos_t pos;
	fgetpos(file, &pos);
	DEBUG_PRINT("Write F32 (%s) 0x%08llX %lld: 0x%08X %f\n", (const char *)name, pos, pos, swapEndianness(*(U32 *)&value), value);
	fwrite(&value, sizeof(value), 1, file);
	return true;
}

//Lazy!
bool IO::write(FILE *file, const S64 &value, const String &name) { return write(file, (U64)value, name); }
bool IO::write(FILE *file, const S32 &value, const String &name) { return write(file, (U32)value, name); }
bool IO::write(FILE *file, const S16 &value, const String &name) { return write(file, (U16)value, name); }
bool IO::write(FILE *file, const S8  &value, const String &name) { return write(file, (U8) value, name); }

bool PlaneF::write(FILE *file) const {
	return
		io->write(file, x, "x") &&
		io->write(file, y, "y") &&
		io->write(file, z, "z") &&
		io->write(file, d, "d");
}

bool QuatF::write(FILE *file) const {
	return
		io->write(file, w, "w") &&
		io->write(file, x, "x") &&
		io->write(file, y, "y") &&
		io->write(file, z, "z");
}

bool BoxF::write(FILE *file) const {
	return
		io->write(file, minX, "minX") &&
		io->write(file, minY, "minY") &&
		io->write(file, minZ, "minZ") &&
		io->write(file, maxX, "maxX") &&
		io->write(file, maxY, "maxY") &&
		io->write(file, maxZ, "maxZ");
}

bool SphereF::write(FILE *file) const {
	return
		io->write(file, x, "x") &&
		io->write(file, y, "y") &&
		io->write(file, z, "z") &&
		io->write(file, radius, "radius");
}

bool MatrixF::write(FILE *file) const {
	return
		io->write(file, m[0], "m[0]") &&
		io->write(file, m[1], "m[1]") &&
		io->write(file, m[2], "m[2]") &&
		io->write(file, m[3], "m[3]") &&
		io->write(file, m[4], "m[4]") &&
		io->write(file, m[5], "m[5]") &&
		io->write(file, m[6], "m[6]") &&
		io->write(file, m[7], "m[7]") &&
		io->write(file, m[8], "m[8]") &&
		io->write(file, m[9], "m[9]") &&
		io->write(file, m[10], "m[10]") &&
		io->write(file, m[11], "m[11]") &&
		io->write(file, m[12], "m[12]") &&
		io->write(file, m[13], "m[13]") &&
		io->write(file, m[14], "m[14]") &&
		io->write(file, m[15], "m[15]");
}

bool String::write(FILE *file) const {
	//<length><bytes>
	if (!io->write(file, (U8)length, "length"))
		return false;
	for (int i = 0; i < length; i ++) {
		if (!io->write(file, data[i], "data"))
			return false;
	}
	return true;
}

bool PNG::write(FILE *file) const {
	//Basically dump out everything. Yeah.

	for (U32 i = 0; i < size; i ++) {
		if (!io->write(file, data[i], "data"))
			return false;
	}
	return true;
}

bool Dictionary::write(FILE *file) const {
	//<length>[<name><value>]...

	if (!io->write(file, size, "size"))
		return false;
	for (int i = 0; i < size; i ++) {
		if (!names[i].write(file) ||
		    !values[i].write(file))
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
