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
#include <map>

#define LIGHT_MAP_SIZE 0x400

#ifdef _WIN32
#define DIR_SEP '\\'
#else
#define DIR_SEP '/'
#endif

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
		static inline bool read(std::istream &stream, T *value, const std::string &name) {
			if (stream.eof())
				return false;
			stream.read(reinterpret_cast<char *>(value), sizeof(*value));
			return stream.good();
		}
	};
	//Read structures from a std::istream
	template <typename T>
	struct read_impl<T, false> {
		static inline bool read(std::istream &stream, T *value, const std::string &name) {
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
	static inline bool read(std::istream &stream, T *value, const std::string &name) {
		//This will select one of the two read_impls above based on whether or not
		// T is a struct or a primitive type.
		return read_impl<T, std::is_fundamental<T>::value>::read(stream, value, name);
	}

	/**
	 Read a vector from a stream
	 @var stream - The stream from which the data is read
	 @var value - A pointer into which the data will be read
	 @var name - A string containing the name of the variable (for debugging)
	 @return If the operation was successful
	 */
	template <typename T>
	static inline bool read(std::istream &stream, std::vector<T> *value, const std::string &name) {
		//Read the size of the vector
		U32 size;
		if (!read(stream, &size, "size"))
			return false;
		//Reserve some space
		value->reserve(size);

		//Read all the objects
		for (int i = 0; i < size; i ++) {
			T obj;
			//Make sure the read succeeds
			if (read(stream, &obj, "obj"))
				value->push_back(obj);
			else
				return false;
		}

		return true;
	}

	/**
	 Read a string from a stream
	 @var stream - The stream from which the data is read
	 @var value - A pointer into which the data will be read
	 @var name - A string containing the name of the variable (for debugging)
	 @return If the operation was successful
	 */
	static inline bool read(std::istream &stream, std::string *value, const std::string &name) {
		//How long is the string
		U8 length;
		if (!read(stream, &length, "length"))
			return false;
		//Empty the string
		*value = std::string();
		//Read each byte of the string
		for (U32 i = 0; i < length; i ++) {
			//If we can read the byte, append it to the string
			U8 chr;
			if (read(stream, &chr, "chr"))
				*value += chr;
			else
				return false;
		}

		return true;
	}

	/**
	 Read a dictionary from a stream
	 @var stream - The stream from which the data is read
	 @var value - A pointer into which the data will be read
	 @var name - A string containing the name of the variable (for debugging)
	 @return If the operation was successful
	 */
	static inline bool read(std::istream &stream, Dictionary *value, const std::string &name) {
		//How long is the map
		U32 length;
		if (!read(stream, &length, "length"))
			return false;

		//Empty the map
		*value = Dictionary();

		//Read the map strings
		for (U32 i = 0; i < length; i ++) {
			std::string name, val;

			//Make sure we can read it
			if (!IO::read(stream, &name, "name") ||
			    !IO::read(stream, &val, "val")) {
				return false;
			}

			//Insert the pair
			(*value).push_back(Dictionary::value_type(name, val));
		}

		return true;
	}

	/**
	 Read a vector from a stream
	 @var stream - The stream from which the data is read
	 @var value - A pointer into which the data will be read
	 @var name - A string containing the name of the variable (for debugging)
	 @return If the operation was successful
	 */
	template <typename type1, typename type2>
	static inline bool read_as(std::istream &stream, std::vector<type1> *value, std::function<bool(bool,U32)> condition, const std::string &name) {
		//Read the size of the vector
		U32 size;
		if (!read(stream, &size, "size"))
			return false;

		//Lots of index lists here that have U16 or U32 versions based on loop2.
		// The actual bytes of the interior have 0x80s at the ends (negative bit)
		// which seems to specify that these take a smaller type. They managed to
		// save ~50KB/interior, but was it worth the pain?

		//Params to use for the condition
		bool useAlternate = false;
		U8 param = 0;

		//Should we use the alternate version?
		if (size & 0x80000000) {
			//Flip the sign bit
			size ^= 0x80000000;
			useAlternate = true;

			//Extra U8 of data in each of these, almost never used but still there
			if (!read(stream, &param, "param"))
				return false;
		}

		//Reserve some space
		value->reserve(size);

		//Read all the objects
		for (int i = 0; i < size; i ++) {
			//Should we use the alternate type? Lambda functions to the rescue!
			if (condition(useAlternate, param)) {
				type2 obj;
				//Make sure the read succeeds
				if (read(stream, &obj, "obj"))
					//Cast it back to what we want
					value->push_back(static_cast<type1>(obj));
				else
					return false;
			} else {
				type1 obj;
				//Make sure the read succeeds
				if (read(stream, &obj, "obj"))
					value->push_back(obj);
				else
					return false;
			}
		}

		return true;
	}

	/**
	 Read a vector from a stream
	 @var stream - The stream from which the data is read
	 @var value - A pointer into which the data will be read
	 @var name - A string containing the name of the variable (for debugging)
	 @return If the operation was successful
	 */
	template <typename T>
	static inline bool read_with(std::istream &stream, std::vector<T> *value, std::function<bool(T*, std::istream &)> passed_method, const std::string &name) {
		//Read the size of the vector
		U32 size;
		if (!read(stream, &size, "size"))
			return false;
		//Reserve some space
		value->reserve(size);

		//Read all the objects
		for (int i = 0; i < size; i ++) {
			T obj;
			//Make sure the read succeeds
			if (passed_method(&obj, stream))
				value->push_back(obj);
			else
				return false;
		}

		return true;
	}

	/**
	 Read a vector from a stream
	 @var stream - The stream from which the data is read
	 @var value - A pointer into which the data will be read
	 @var name - A string containing the name of the variable (for debugging)
	 @return If the operation was successful
	 */
	template <typename T>
	static inline bool read_extra(std::istream &stream, std::vector<T> *value, std::function<bool(std::istream &)> extra_method, const std::string &name) {
		//Read the size of the vector
		U32 size;
		if (!read(stream, &size, "size"))
			return false;
		//Reserve some space
		value->reserve(size);

		//Do the extra method
		if (!extra_method(stream))
			return false;

		//Read all the objects
		for (int i = 0; i < size; i ++) {
			T obj;
			//Make sure the read succeeds
			if (read(stream, &obj, "obj"))
				value->push_back(obj);
			else
				return false;
		}

		return true;
	}


	//Write primitive types from a std::istream
	template <typename T, bool=true>
	struct write_impl {
		static inline bool write(std::ostream &stream, const T &value, const std::string &name) {
			stream.write(reinterpret_cast<const char *>(&value), sizeof(value));
			return stream.good();
		}
	};
	//Write structures from a std::istream
	template <typename T>
	struct write_impl<T, false> {
		static inline bool write(std::ostream &stream, const T &value, const std::string &name) {
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
	static inline bool write(std::ostream &stream, const T &value, const std::string &name) {
		//This will select one of the two write_impls above based on whether or not
		// T is a struct or a primitive type.
		return write_impl<T, std::is_fundamental<T>::value>::write(stream, value, name);
	}

	/**
	 Write a vector to a stream
	 @var stream - The stream to which the data is written
	 @var value - The vector to write
	 @var name - A string containing the name of the variable (for debugging)
	 @return If the operation was successful
	 */
	template <typename T>
	static inline bool write(std::ostream &stream, const std::vector<T> &value, const std::string &name) {
		//Write the vector's size first, must be a U32 because torque
		if (!write(stream, static_cast<U32>(value.size()), "size"))
			return false;
		//Write all of the objects in the vector
		for (int i = 0; i < value.size(); i ++) {
			if (!write(stream, value[i], "value"))
				return false;
		}
		return true;
	}

	/**
	 Write a string to a stream
	 @var stream - The stream to which the data is written
	 @var value - The string to write
	 @var name - A string containing the name of the variable (for debugging)
	 @return If the operation was successful
	 */
	static inline bool write(std::ostream &stream, const std::string &value, const std::string &name) {
		//How long is the string
		if (!write(stream, static_cast<U8>(value.length()), "length"))
			return false;
		//Write each byte of the string
		for (U32 i = 0; i < value.length(); i ++) {
			if (!write(stream, value[i], "char"))
				return false;
		}

		return true;
	}

	/**
	 Write a dictionary to a stream
	 @var stream - The stream to which the data is written
	 @var value - The map to write
	 @var name - A string containing the name of the variable (for debugging)
	 @return If the operation was successful
	 */
	static inline bool write(std::ostream &stream, const Dictionary &value, const std::string &name) {
		//How long is the map
		if (!write(stream, static_cast<U32>(value.size()), "length"))
			return false;
		
		//Write each string in the map
		for (auto pair : value) {
			if (!write(stream, pair.first, "name") ||
			    !write(stream, pair.second, "value"))
				return false;
		}

		return true;
	}

	/**
	 Write a vector to a stream
	 @var stream - The stream to which the data is written
	 @var value - The vector to write
	 @var name - A string containing the name of the variable (for debugging)
	 @return If the operation was successful
	 */
	template <typename T>
	static inline bool write_extra(std::ostream &stream, const std::vector<T> &value, std::function<bool(std::ostream &)> extra_method, const std::string &name) {
		//Write the vector's size first, must be a U32 because torque
		if (!write(stream, static_cast<U32>(value.size()), "size"))
			return false;
		//Use the extra method... which happens to come before the value
		if (!extra_method(stream))
			return false;

		//Write all of the objects in the vector
		for (int i = 0; i < value.size(); i ++) {
			if (!write(stream, value[i], "value"))
				return false;
		}
		return true;
	}

	/**
	 * Get the path of a file up to but not including the file name
	 * @param file The file whose path to get
	 * @param separator The directory separator to use (platform-specific by default)
	 * @return The file's path
	 */
	static const std::string getPath(const std::string &file, const char &separator = DIR_SEP);
	/**
	 * Get the name of a file inluding the directory but not the path
	 * @param file The file whose name to get
	 * @param separator The directory separator to use (platform-specific by default)
	 * @return The file's name
	 */
	static const std::string getName(const std::string &file, const char &separator = DIR_SEP);
	/**
	 * Get the extension of a file
	 * @param file The file whose extension to get
	 * @return The file's extension
	 */
	static const std::string getExtension(const std::string &file);
	/**
	 * Get the base name of a file including neither directory nor path
	 * @param file The file whose base name to get
	 * @param separator The directory separator to use (platform-specific by default)
	 * @return The file's base name
	 */
	static const std::string getBase(const std::string &file, const char &separator = DIR_SEP);

	/**
	 * Determine if a file exists in the filesystem.
	 * @param file The file whose existence to check
	 * @return If the file exists
	 */
	static bool isfile(const std::string &file);
	/**
	 * Read a file from the filesystem in its entirety to a U8* buffer.
	 * @param file The file to read
	 * @param length A U32 in which the file's length will be stored
	 * @return A character buffer containing the contents of the file, or NULL if
	 *         the file could not be found.
	 */
	static U8 *readFile(const std::string &file, U32 *length);
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
//I'm so sorry about reinterpret_cast<type *>(NULL), but that's the only way to get C++ to interpret
// the type and let the template work
#define READ(type) __read(stream, reinterpret_cast<type *>(NULL))

#ifdef DEBUG
	#define READVAR(name, type) \
		type name; \
		IO::read(stream, reinterpret_cast<type *>(&name), #name)
	#define READTOVAR(name, type) IO::read(stream, reinterpret_cast<type *>(&name), #name)
#else
	#define READVAR(name, type) \
		type name; \
		IO::read(stream, reinterpret_cast<type *>(&name), "")
	#define READTOVAR(name, type) IO::read(stream, reinterpret_cast<type *>(&name), "")
#endif

#define READCHECK(name, type) { if (!READTOVAR(name, type)) return false; }

#define READLOOP(name) \
READVAR(name##_length, U32); \
for (U32 i = 0; i < name##_length; i ++)

#define READLIST(name, type) \
READVAR(name##_length, U32); \
for (U32 i = 0; i < name##_length; i ++) { \
	READ(type); \
}

//Macros to speed up file writing
#ifdef DEBUG
#define WRITECHECK(value, type) { if (!IO::write(stream, (const type) value, #value)) return false; }
#else
#define WRITECHECK(value, type) { if (!IO::write(stream, (const type) value, "")) return false; }
#endif


#endif
