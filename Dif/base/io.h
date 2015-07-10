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
#include <type_traits>

#define LIGHT_MAP_SIZE 0x400

/*
 Read number types from a file
 @var file - The FILE to read from (updates position)
 @return The number data at that position in the FILE
 */
class IO {
public:
	//Read primitive types from a std::istream
	template <typename T, bool=true>
	struct read_impl {
		static inline bool read(std::istream &stream, T *value, const String &name) {
			if (stream.eof())
				return false;
			stream.read((char *)value, sizeof(*value));
			return stream.good();
		}
	};
	//Read structures from a std::istream
	template <typename T>
	struct read_impl<T, false> {
		static inline bool read(std::istream &stream, T *value, const String &name) {
			return value->read(stream);
		}
	};

	/**
	 Read types from a stream
	 @var stream - The stream from which the data is read
	 @var value - A pointer into which the data will be read
	 @var name - A string containing the name of the variable (for debugging)
	 @return If the operation was successful
	 */
	template <typename T>
	static inline bool read(std::istream &stream, T *value, const String &name) {
		//This will select one of the two read_impls above based on whether or not
		// T is a struct or a primitive type.
		return read_impl<T, std::is_fundamental<T>::value>::read(stream, value, name);
	}

	//Write primitive types from a std::istream
	template <typename T, bool=true>
	struct write_impl {
		static inline bool write(std::ostream &stream, const T &value, const String &name) {
			stream.write((char *)&value, sizeof(value));
			return stream.good();
		}
	};
	//Write structures from a std::istream
	template <typename T>
	struct write_impl<T, false> {
		static inline bool write(std::ostream &stream, const T &value, const String &name) {
			return value.write(stream);
		}
	};

	/**
	 Write types to a stream
	 @var stream - The stream to which the data is written
	 @var value - The value of the data to write
	 @var name - A string containing the name of the variable (for debugging)
	 @return If the operation was successful
	 */
	template <typename T>
	static inline bool write(std::ostream &stream, const T &value, const String &name) {
		//This will select one of the two write_impls above based on whether or not
		// T is a struct or a primitive type.
		return write_impl<T, std::is_fundamental<T>::value>::write(stream, value, name);
	}

	static String getPath(const String &file);
	static String getName(const String &file);
	static String getExtension(const String &file);

	static bool isfile(const String &file);
	static U8 *readFile(const String &file, U32 *length);
};

template <typename T>
bool Point2<T>::read(std::istream &stream) {
	return
	IO::read(stream, &x, "x") &&
	IO::read(stream, &y, "y");
}

template <typename T>
bool Point3<T>::read(std::istream &stream) {
	return
	IO::read(stream, &x, "x") &&
	IO::read(stream, &y, "y") &&
	IO::read(stream, &z, "z");
}

template <typename T>
bool Point4<T>::read(std::istream &stream) {
	return
	IO::read(stream, &w, "w") &&
	IO::read(stream, &x, "x") &&
	IO::read(stream, &y, "y") &&
	IO::read(stream, &z, "z");
}

template <typename T>
bool Color<T>::read(std::istream &stream) {
	return
	IO::read(stream, &red, "red") &&
	IO::read(stream, &green, "green") &&
	IO::read(stream, &blue, "blue") &&
	IO::read(stream, &alpha, "alpha");
}

template <typename T>
bool Vector<T>::read(std::istream &stream) {
	U32 size;
	if (!IO::read(stream, &size, "size"))
		return false;
	for (int i = 0; i < size; i ++) {
		T value;
		if (IO::read(stream, &value, "value"))
			push_back(value);
		else
			return false;
	}
	return true;
}

template <typename T>
bool Point2<T>::write(std::ostream &stream) const {
	return
	IO::write(stream, x, "x") &&
	IO::write(stream, y, "y");
}

template <typename T>
bool Point3<T>::write(std::ostream &stream) const {
	return
	IO::write(stream, x, "x") &&
	IO::write(stream, y, "y") &&
	IO::write(stream, z, "z");
}

template <typename T>
bool Point4<T>::write(std::ostream &stream) const {
	return
	IO::write(stream, w, "w") &&
	IO::write(stream, x, "x") &&
	IO::write(stream, y, "y") &&
	IO::write(stream, z, "z");
}

template <typename T>
bool Color<T>::write(std::ostream &stream) const {
	return
	IO::write(stream, red, "red") &&
	IO::write(stream, green, "green") &&
	IO::write(stream, blue, "blue") &&
	IO::write(stream, alpha, "alpha");
}

template <typename T>
bool Vector<T>::write(std::ostream &stream) const {
	if (!IO::write(stream, vector.size(), "size"))
		return false;
	for (int i = 0; i < vector.size(); i ++) {
		if (!IO::write(stream, vector[i], "value"))
			return false;
	}
	return true;
}


//Hack to get the read() macro to return a value from a function that uses a ref
template <typename T>
inline T __read(std::istream &stream, T *thing) {
	T __garbage;
#ifdef DEBUG
	IO::read(stream, &__garbage, "garbage");
#else
	IO::read(stream, &__garbage, "");
#endif
	return __garbage;
}
//I'm so sorry about (type *)NULL, but that's the only way to get C++ to interpret
// the type and let the template work
#define READ(type) __read(stream, (type *)NULL)

#ifdef DEBUG
	#define READVAR(name, type) \
		type name; \
		IO::read(stream, (type *)&name, String(#name))
	#define READTOVAR(name, type) IO::read(stream, (type *)&name, String(#name))
	#define READCHECK(type, value) { \
		if (READ(type) != value)\
			return;\
	}
#else
	#define READVAR(name, type) \
		type name; \
		IO::read(stream, (type *)&name, "")
	#define READTOVAR(name, type) IO::read(stream, (type *)&name, "")
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
#define WRITE(value, type) IO::write(stream, (type) value, String(#value))
#else
#define WRITE(value, type) IO::write(stream, (type) value, "")
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
