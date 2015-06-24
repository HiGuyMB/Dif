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

#ifndef io_h
#define io_h

#include "types.h"
#include "math.h"

#define LIGHT_MAP_SIZE 0x400
#define io IO::getIO()

/*
 Read number types from a file
 @var file - The FILE to read from (updates position)
 @return The number data at that position in the FILE
 */
class IO {
public:
	IO();
	~IO();

	static IO *getIO();

	void reverse(FILE **file, const U32 &bytes);

	//Unsigned ints
	bool read(FILE *file, U64 *value, const String &name);
	bool read(FILE *file, U32 *value, const String &name);
	bool read(FILE *file, U16 *value, const String &name);
	bool read(FILE *file,  U8 *value, const String &name);
	
	//Signed ints
	bool read(FILE *file, S64 *value, const String &name);
	bool read(FILE *file, S32 *value, const String &name);
	bool read(FILE *file, S16 *value, const String &name);
	bool read(FILE *file,  S8 *value, const String &name);

	//Floats
	bool read(FILE *file, F32 *value, const String &name);

	//Anything else that is a class can be templated
	template <typename T>
	inline bool read(FILE *file, T *value, const String &name) {
		return value->read(file);
	}

	/*
	 Write number types to a file
	 @var file - The FILE to read from (updates position)
	 @return The number data at that position in the FILE
	 */

	//Unsigned ints
	bool write(FILE *file, const U64 &value, const String &name);
	bool write(FILE *file, const U32 &value, const String &name);
	bool write(FILE *file, const U16 &value, const String &name);
	bool write(FILE *file, const U8  &value, const String &name);

	//Signed ints
	bool write(FILE *file, const S64 &value, const String &name);
	bool write(FILE *file, const S32 &value, const String &name);
	bool write(FILE *file, const S16 &value, const String &name);
	bool write(FILE *file, const S8  &value, const String &name);

	//Floats
	bool write(FILE *file, const F32 &value, const String &name);

	//Anything else that is a class can be templated
	template <typename T>
	inline bool write(FILE *file, const T &value, const String &name) {
		return value.write(file);
	}

	String getPath(const String &file);
	String getName(const String &file);
	String getExtension(const String &file);

	bool isfile(const String &file);
	U8 *readFile(const String &file, U32 *length);
};

template <typename T>
bool Point2<T>::read(FILE *file) {
	return
	io->read(file, &x, "x") &&
	io->read(file, &y, "y");
}

template <typename T>
bool Point3<T>::read(FILE *file) {
	return
	io->read(file, &x, "x") &&
	io->read(file, &y, "y") &&
	io->read(file, &z, "z");
}

template <typename T>
bool Point4<T>::read(FILE *file) {
	return
	io->read(file, &w, "w") &&
	io->read(file, &x, "x") &&
	io->read(file, &y, "y") &&
	io->read(file, &z, "z");
}

template <typename T>
bool Color<T>::read(FILE *file) {
	return
	io->read(file, &red, "red") &&
	io->read(file, &green, "green") &&
	io->read(file, &blue, "blue") &&
	io->read(file, &alpha, "alpha");
}

template <typename T>
bool Point2<T>::write(FILE *file) const {
	return
	io->write(file, x, "x") &&
	io->write(file, y, "y");
}

template <typename T>
bool Point3<T>::write(FILE *file) const {
	return
	io->write(file, x, "x") &&
	io->write(file, y, "y") &&
	io->write(file, z, "z");
}

template <typename T>
bool Point4<T>::write(FILE *file) const {
	return
	io->write(file, w, "w") &&
	io->write(file, x, "x") &&
	io->write(file, y, "y") &&
	io->write(file, z, "z");
}

template <typename T>
bool Color<T>::write(FILE *file) const {
	return
	io->write(file, red, "red") &&
	io->write(file, green, "green") &&
	io->write(file, blue, "blue") &&
	io->write(file, alpha, "alpha");
}

//Macros to speed up file reading
#define REVERSE(size) io->reverse(&file, size)

//Hack to get the read() macro to return a value from a function that uses a ref
template <typename T>
inline T __read(FILE *file, T *thing) {
	T __garbage;
#ifdef DEBUG
	io->read(file, &__garbage, "garbage");
#else
	io->read(file, &__garbage, "");
#endif
	return __garbage;
}
//I'm so sorry about (type *)NULL, but that's the only way to get C++ to interpret
// the type and let the template work
#define READ(type) __read(file, (type *)NULL)

#ifdef DEBUG
	#define READVAR(name, type) \
		type name; \
		io->read(file, (type *)&name, String(#name))
	#define READTOVAR(name, type) io->read(file, (type *)&name, String(#name))
	#define READCHECK(type, value) { \
		if (READ(type) != value)\
			return;\
	}
#else
	#define READVAR(name, type) \
		type name; \
		io->read(file, (type *)&name, "")
	#define READTOVAR(name, type) io->read(file, (type *)&name, "")
	#define READCHECK(type, value) { \
	READVAR(check, type); \
	if (check != value)\
		return;\
	}
#endif

#define READLOOPVAR(countvar, listvar, type) \
READTOVAR(countvar, U32); \
listvar = new type[countvar]; \
for (U32 i = 0; i < countvar; i ++)

#define READLISTVAR(countvar, listvar, type) \
READTOVAR(countvar, U32); \
listvar = new type[countvar]; \
for (U32 i = 0; i < countvar; i ++) { \
	READTOVAR(listvar[i], type); \
}

#define READLOOP(name) \
READVAR(name##_length, U32); \
for (U32 i = 0; i < name##_length; i ++)

#define READLIST(name, type) \
READVAR(name##_length, U32); \
for (U32 i = 0; i < name##_length; i ++) { \
	READ(type); \
}

#define READLOOPVAR2(countvar, listvar, type) \
bool read##countvar##2 = false; \
U8 read##countvar##param = 0; \
READTOVAR(countvar, U32); \
if (countvar  & 0x80000000) { \
	countvar ^= 0x80000000; \
	read##countvar##2 = true; \
	READTOVAR(read##countvar##param, U8); \
} \
listvar = new type[countvar]; \
for (U32 i = 0; i < countvar; i ++)

#define READLISTVAR2(countvar, listvar, condition, normaltype, alternatetype) \
bool read##countvar##2 = false; \
U8 read##countvar##param = 0; \
READTOVAR(countvar, U32); \
if (countvar  & 0x80000000) { \
	countvar ^= 0x80000000; \
	read##countvar##2 = true; \
	READTOVAR(read##countvar##param, U8); \
} \
listvar = new normaltype[countvar]; \
for (U32 i = 0; i < countvar; i ++) { \
	if ((condition)) { \
		READTOVAR(listvar[i], alternatetype); \
	} else { \
		READTOVAR(listvar[i], normaltype); \
	} \
}

#define READLOOP2(name) \
bool read##name##2 = false; \
U8 read##name##param = 0; \
READVAR(name##_length, U32); \
if (name##_length  & 0x80000000) { \
	name##_length ^= 0x80000000; \
	read##name##2 = true; \
	READTOVAR(read##name##param, U8); \
} \
for (U32 i = 0; i < name##_length; i ++)

#define READLIST2(name, condition, normaltype, alternatetype) \
bool read##name##2 = false; \
U8 read##name##param = 0; \
READVAR(name##_length, U32); \
if (name##_length  & 0x80000000) { \
	name##_length ^= 0x80000000; \
	read##name##2 = true; \
	READTOVAR(read##name##param, U8); \
} \
for (U32 i = 0; i < name##_length; i ++) { \
	if ((condition)) { \
		READ(alternatetype); \
	} else { \
		READ(normaltype); \
	} \
}

//Macros to speed up file reading
#ifdef DEBUG
#define WRITE(value, type) io->write(file, (type) value, String(#value))
#else
#define WRITE(value, type) io->write(file, (type) value, "")
#endif

#define WRITECHECK(value, type) { if (!WRITE(value, type)) return false; }

#define WRITELIST(countvar, listvar, type) \
WRITECHECK(countvar, U32);\
for (U32 i = 0; i < countvar; i ++) { \
	WRITE(listvar[i], type); \
}

#define WRITELOOP(countvar) \
WRITECHECK(countvar, U32);\
for (U32 i = 0; i < countvar; i ++)\

#endif
