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

#ifndef dif_io_h
#define dif_io_h

#include <dif/base/types.h>
#include <type_traits>
#include <map>
#include <iostream>
#include <functional>

DIF_NAMESPACE

#define LIGHT_MAP_SIZE 0x400

//#ifdef _WIN32
//#define DIR_SEP '\\'
//#else
#define DIR_SEP '/'
//#endif

//Define this to activate stream debug read/write logging
#if 0
#define PRINT_DEBUG_INFO
#endif

class IO {
public:
#ifdef PRINT_DEBUG_INFO
	//Print out information about the data being read / written to streams.
	// READCHECK and WRITECHECK will automatically append the type to name, normal
	// raw IO::read/write methods won't.
	template <typename T>
	static inline void debug_print(std::istream &stream, T &value, const std::string &name) {
		std::istream::pos_type pos = stream.tellg();
		//Basic information about the data's name / offset
		std::cout << "Read " << name << " at offset " << pos << " (0x" << std::hex << pos << ")" << std::dec << std::endl;
	}
	template <typename T>
	static inline void debug_print(std::ostream &stream, const T &value, const std::string &name) {
		std::istream::pos_type pos = stream.tellp();
		//Basic information about the data's name / offset
		std::cout << "Write " << name << " at offset " << pos << " (0x" << std::hex << pos << ")" << std::dec << std::endl;
	}
#else
	//Use a macro here so that absolutely no code is generated.
#define debug_print(stream, value, name)
#endif

	//Read primitive types from a std::istream
	template <typename T, bool=true>
	struct read_impl {
		static inline bool read(std::istream &stream, Version version, T &value, const std::string &name) {
			if (stream.eof())
				return false;
			debug_print(stream, value, name);
			stream.read(reinterpret_cast<char *>(&value), sizeof(value));
			return stream.good();
		}
	};
	//Read structures from a std::istream
	template <typename T>
	struct read_impl<T, false> {
		static inline bool read(std::istream &stream, Version version, T &value, const std::string &name) {
			debug_print(stream, value, name);
			return value.read(stream, version);
		}
	};

	/**
	 * Read types from a stream
	 * @param stream The stream from which the data is read
	 * @param value A reference into which the data will be read
	 * @param name A string containing the name of the variable (for debugging)
	 * @return If the operation was successful
	 */
	template <typename T>
	static inline bool read(std::istream &stream, Version version, T &value, const std::string &name) {
		//This will select one of the two read_impls above based on whether or not
		// T is a struct or a primitive type.
		return read_impl<T, std::is_fundamental<T>::value>::read(stream, version, value, name);
	}

	/**
	 * Read a vector from a stream
	 * @param stream The stream from which the data is read
	 * @param value A reference into which the data will be read
	 * @param name A string containing the name of the variable (for debugging)
	 * @return If the operation was successful
	 */
	template <typename T>
	static inline bool read(std::istream &stream, Version version, std::vector<T> &value, const std::string &name) {
		//Read the size of the vector
		U32 size;
		if (!read(stream, version, size, "size"))
			return false;
		//Reserve some space
		value.reserve(size);

		//Read all the objects
		for (U32 i = 0; i < size; i ++) {
			T obj;
			//Make sure the read succeeds
			if (read(stream, version, obj, "obj"))
				value.push_back(obj);
			else
				return false;
		}

		return true;
	}

	/**
	 * Read a string from a stream
	 * @param stream The stream from which the data is read
	 * @param value A reference into which the data will be read
	 * @param name A string containing the name of the variable (for debugging)
	 * @return If the operation was successful
	 */
	static inline bool read(std::istream &stream, Version version, std::string &value, const std::string &name) {
		//How long is the string
		U8 length;
		if (!read(stream, version, length, "length"))
			return false;
		//Empty the string
		value = std::string();
		//Read each byte of the string
		for (U32 i = 0; i < length; i ++) {
			//If we can read the byte, append it to the string
			U8 chr;
			if (read(stream, version, chr, "chr"))
				value += chr;
			else
				return false;
		}

		return true;
	}

	/**
	 * Read a dictionary from a stream
	 * @param stream The stream from which the data is read
	 * @param value A reference into which the data will be read
	 * @param name A string containing the name of the variable (for debugging)
	 * @return If the operation was successful
	 */
	static inline bool read(std::istream &stream, Version version, Dictionary &value, const std::string &name) {
		//How long is the map
		U32 length;
		if (!read(stream, version, length, "length"))
			return false;

		//Empty the map
		value = Dictionary();

		//Read the map strings
		for (U32 i = 0; i < length; i ++) {
			std::string name, val;

			//Make sure we can read it
			if (!read(stream, version, name, "name") ||
			    !read(stream, version, val, "val")) {
				return false;
			}

			//Insert the pair
			value.push_back(Dictionary::value_type(name, val));
		}

		return true;
	}

	/**
	 * Read a vector from a stream, with the option to read as a secondary type if
	 * a given condition passes.
	 * @param stream The stream from which the data is read
	 * @param value A reference into which the data will be read
	 * @param condition A function that determines whether the second type should be used.
	 *                    Arguments are (bool isSigned, U32 param).
	 * @param name A string containing the name of the variable (for debugging)
	 * @return If the operation was successful
	 */
	template <typename type1, typename type2>
	static inline bool read_as(std::istream &stream, Version version, std::vector<type1> &value, std::function<bool(bool,U32)> condition, const std::string &name) {
		//Read the size of the vector
		U32 size;
		if (!read(stream, version, size, "size"))
			return false;

		//Lots of index lists here that have U16 or U32 versions based on the sign bit.
		// The actual bytes of the interior have 0x80s at the ends (negative bit)
		// which seems to specify that these take a smaller type. They managed to
		// save ~50KB/interior, but was it worth the pain?

		//Params to use for the condition
		bool isSigned = false;
		U8 param = 0;

		//Should we use the alternate version?
		if (size & 0x80000000) {
			//Flip the sign bit
			size ^= 0x80000000;
			isSigned = true;

			//Extra U8 of data in each of these, almost never used but still there
			if (!read(stream, version, param, "param"))
				return false;
		}

		//Reserve some space
		value.reserve(size);

		//Read all the objects
		for (U32 i = 0; i < size; i ++) {
			//Should we use the alternate type? Lambda functions to the rescue!
			if (condition(isSigned, param)) {
				type2 obj;
				//Make sure the read succeeds
				if (read(stream, version, obj, "obj"))
					//Cast it back to what we want
					value.push_back(static_cast<type1>(obj));
				else
					return false;
			} else {
				type1 obj;
				//Make sure the read succeeds
				if (read(stream, version, obj, "obj"))
					value.push_back(obj);
				else
					return false;
			}
		}

		return true;
	}

	/**
	 * Read a vector from a stream, but use a given method for reading instead of 
	 * the standard call to read().
	 * @param stream The stream from which the data is read
	 * @param value A reference into which the data will be read
	 * @param passed_method A function which will read the object's fields from the stream.
	 *                        Arguments are (T &object, std::istream &stream)
	 * @param name A string containing the name of the variable (for debugging)
	 * @return If the operation was successful
	 */
	template <typename T>
	static inline bool read_with(std::istream &stream, Version version, std::vector<T> &value, std::function<bool(T&, std::istream &, Version &)> passed_method, const std::string &name) {
		//Read the size of the vector
		U32 size;
		if (!read(stream, version, size, "size"))
			return false;
		//Reserve some space
		value.reserve(size);

		//Read all the objects
		for (U32 i = 0; i < size; i ++) {
			T obj;
			//Make sure the read succeeds
			if (passed_method(obj, stream, version))
				value.push_back(obj);
			else
				return false;
		}

		return true;
	}

	/**
	 * Read a vector from a stream, but call an extra method after the size is read,
	 * before the actual contents are read.
	 * @param stream The stream from which the data is read
	 * @param value A reference into which the data will be read
	 * @param extra_method A function that will be called before the contents are read.
	 *                       Arguments are (std::istream &stream)
	 * @param name A string containing the name of the variable (for debugging)
	 * @return If the operation was successful
	 */
	template <typename T>
	static inline bool read_extra(std::istream &stream, Version version, std::vector<T> &value, std::function<bool(std::istream &, Version &)> extra_method, const std::string &name) {
		//Read the size of the vector
		U32 size;
		if (!read(stream, version, size, "size"))
			return false;
		//Reserve some space
		value.reserve(size);

		//Do the extra method
		if (!extra_method(stream, version))
			return false;

		//Read all the objects
		for (U32 i = 0; i < size; i ++) {
			T obj;
			//Make sure the read succeeds
			if (read(stream, version, obj, "obj"))
				value.push_back(obj);
			else
				return false;
		}

		return true;
	}


	//Write primitive types to a std::ostream
	template <typename T, bool=true>
	struct write_impl {
		static inline bool write(std::ostream &stream, Version version, const T &value, const std::string &name) {
			debug_print(stream, value, name);
			stream.write(reinterpret_cast<const char *>(&value), sizeof(value));
			return stream.good();
		}
	};
	//Write structures to a std::ostream
	template <typename T>
	struct write_impl<T, false> {
		static inline bool write(std::ostream &stream, Version version, const T &value, const std::string &name) {
			debug_print(stream, value, name);
			return value.write(stream, version);
		}
	};

	/**
	 * Write types to a stream
	 * @param stream The stream to which the data is written
	 * @param value The value of the data to write
	 * @param name A string containing the name of the variable (for debugging)
	 * @return If the operation was successful
	 */
	template <typename T>
	static inline bool write(std::ostream &stream, Version version, const T &value, const std::string &name) {
		//This will select one of the two write_impls above based on whether or not
		// T is a struct or a primitive type.
		return write_impl<T, std::is_fundamental<T>::value>::write(stream, version, value, name);
	}

	/**
	 * Write a vector to a stream
	 * @param stream The stream to which the data is written
	 * @param value The vector to write
	 * @param name A string containing the name of the variable (for debugging)
	 * @return If the operation was successful
	 */
	template <typename T>
	static inline bool write(std::ostream &stream, Version version, const std::vector<T> &value, const std::string &name) {
		//Write the vector's size first, must be a U32 because torque
		if (!write(stream, version, static_cast<U32>(value.size()), "size"))
			return false;
		//Write all of the objects in the vector
		for (size_t i = 0; i < value.size(); i ++) {
			if (!write(stream, version, value[i], "value"))
				return false;
		}
		return true;
	}

	/**
	 * Write a string to a stream
	 * @param stream The stream to which the data is written
	 * @param value The string to write
	 * @param name A string containing the name of the variable (for debugging)
	 * @return If the operation was successful
	 */
	static inline bool write(std::ostream &stream, Version version, const std::string &value, const std::string &name) {
		//How long is the string
		if (!write(stream, version, static_cast<U8>(value.length()), "length"))
			return false;
		//Write each byte of the string
		for (U32 i = 0; i < value.length(); i ++) {
			if (!write(stream, version, value[i], "char"))
				return false;
		}

		return true;
	}

	/**
	 * Write a dictionary to a stream
	 * @param stream The stream to which the data is written
	 * @param value The map to write
	 * @param name A string containing the name of the variable (for debugging)
	 * @return If the operation was successful
	 */
	static inline bool write(std::ostream &stream, Version version, const Dictionary &value, const std::string &name) {
		//How long is the map
		if (!write(stream, version, static_cast<U32>(value.size()), "length"))
			return false;
		
		//Write each string in the map
		for (auto pair : value) {
			if (!write(stream, version, pair.first, "name") ||
			    !write(stream, version, pair.second, "value"))
				return false;
		}

		return true;
	}

	/**
	 * Write a vector to a stream, but call an extra method after the size is written,
	 * before the actual contents are written.
	 * @param stream The stream to which the data is written
	 * @param value The vector to write
	 * @param extra_method A function that will be called before the contents are written.
	 *                       Arguments are (std::ostream &stream)
	 * @param name A string containing the name of the variable (for debugging)
	 * @return If the operation was successful
	 */
	template <typename T>
	static inline bool write_extra(std::ostream &stream, Version version, const std::vector<T> &value, std::function<bool(std::ostream &, Version)> extra_method, const std::string &name) {
		//Write the vector's size first, must be a U32 because torque
		if (!write(stream, version, static_cast<U32>(value.size()), "size"))
			return false;
		//Use the extra method... which happens to come before the value
		if (!extra_method(stream, version))
			return false;

		//Write all of the objects in the vector
		for (size_t i = 0; i < value.size(); i ++) {
			if (!write(stream, version, value[i], "value"))
				return false;
		}
		return true;
	}
};

template <typename T>
bool Point2<T>::read(std::istream &stream, Version &version) {
	return
		IO::read(stream, version, x, "x") &&
		IO::read(stream, version, y, "y");
}

template <typename T>
bool Point3<T>::read(std::istream &stream, Version &version) {
	return
		IO::read(stream, version, x, "x") &&
		IO::read(stream, version, y, "y") &&
		IO::read(stream, version, z, "z");
}

template <typename T>
bool Color<T>::read(std::istream &stream, Version &version) {
	return
		IO::read(stream, version, red, "red") &&
		IO::read(stream, version, green, "green") &&
		IO::read(stream, version, blue, "blue") &&
		IO::read(stream, version, alpha, "alpha");
}

template <typename T>
bool Point2<T>::write(std::ostream &stream, Version version) const {
	return
	IO::write(stream, version, x, "x") &&
	IO::write(stream, version, y, "y");
}

template <typename T>
bool Point3<T>::write(std::ostream &stream, Version version) const {
	return
		IO::write(stream, version, x, "x") &&
		IO::write(stream, version, y, "y") &&
		IO::write(stream, version, z, "z");
}

template <typename T>
bool Color<T>::write(std::ostream &stream, Version version) const {
	return
		IO::write(stream, version, red, "red") &&
		IO::write(stream, version, green, "green") &&
		IO::write(stream, version, blue, "blue") &&
		IO::write(stream, version, alpha, "alpha");
}


//Hack to get the read() macro to return a value from a function that uses a ref
template <typename T>
inline T __read(std::istream &stream, Version version, T *thing) {
	T __garbage;
#ifdef DEBUG
	IO::read(stream, version, __garbage, "garbage");
#else
	IO::read(stream, version, __garbage, "");
#endif
	return __garbage;
}
//I'm so sorry about reinterpret_cast<type *>(NULL), but that's the only way to get C++ to interpret
// the type and let the template work
#define READ(type) __read(stream, version, reinterpret_cast<type *>(NULL))

//Magical casts so we can read and write the types which we want.
template <typename T, typename F>
inline T& __magic_cast(F &thing) {
	//This is so gross but apparently better than C-style casts. At least so says
	// Jeff. We can't do this inline because temporary variables. So maybe this will
	// work as a method instead?
	return reinterpret_cast<T&>(thing);
}

//Same thing as above but with 100% more const
template <typename T, typename F>
inline const T& __magic_const_cast(const F &thing) {
	//Don't even need a const_cast... That's cool.
	return reinterpret_cast<const T&>(thing);
}

//Macros to speed up file reading/writing
#ifdef DEBUG
	#define READCHECK(name, type)  { if (!IO::read (stream, version, __magic_cast<type>(name),       #name " as " #type)) return false; }
	#define WRITECHECK(name, type) { if (!IO::write(stream, version, __magic_const_cast<type>(name), #name " as " #type)) return false; }
#else
	#define READCHECK(name, type)  { if (!IO::read (stream, version, __magic_cast<type>(name),       "")) return false; }
	#define WRITECHECK(name, type) { if (!IO::write(stream, version, __magic_const_cast<type>(name), "")) return false; }
#endif

DIF_NAMESPACE_END

#endif
