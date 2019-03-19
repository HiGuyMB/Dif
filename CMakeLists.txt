cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
include (CheckIncludeFiles)
project (Dif C CXX)

# Allow C++11 features
if (UNIX)
list (APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
elseif(MSVC)
list (APPEND CMAKE_CXX_FLAGS "/MP ${CMAKE_CXX_FLAGS}")
endif()

# Enable Static Linking the C++ ABI directly into the executables and libraries
if (MSVC)
	#---BEGIN COPY FROM BULLET CMAKE---
	#We statically link to reduce dependancies
	FOREACH(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO )
		IF(${flag_var} MATCHES "/MD")
			STRING(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
		ENDIF(${flag_var} MATCHES "/MD")
		IF(${flag_var} MATCHES "/MDd")
			STRING(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
		ENDIF(${flag_var} MATCHES "/MDd")
	ENDFOREACH(flag_var)
	#---END COPY FROM BULLET CMAKE---
endif()

# http://stackoverflow.com/a/7750816
# Basically just force the output dir to where we give
macro(target_output_directory targetname directory)
	set_target_properties( ${targetname} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${directory} )
	set_target_properties( ${targetname} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${directory} )
	set_target_properties( ${targetname} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${directory} )
	# Second, for multi-config builds (e.g. msvc)
	foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
		string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
		set_target_properties( ${targetname} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${directory} )
		set_target_properties( ${targetname} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${directory} )
		set_target_properties( ${targetname} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${directory} )
	endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
endmacro()

set (DIF_SRC
	############################################################################
	# Sources
	############################################################################

	# Base
	src/dif/base/io.cpp

	# Objects
	src/dif/objects/aiSpecialNode.cpp
	src/dif/objects/dif.cpp
	src/dif/objects/forceField.cpp
	src/dif/objects/gameEntity.cpp
	src/dif/objects/interior.cpp
	src/dif/objects/interiorPathFollower.cpp
	src/dif/objects/staticMesh.cpp
	src/dif/objects/trigger.cpp
	src/dif/objects/vehicleCollision.cpp

	############################################################################
	# Headers
	############################################################################

	# Base
	include/dif/base/color.h
	include/dif/base/io.h
	include/dif/base/types.h

	# Objects
	include/dif/objects/aiSpecialNode.h
	include/dif/objects/dif.h
	include/dif/objects/forceField.h
	include/dif/objects/gameEntity.h
	include/dif/objects/interior.h
	include/dif/objects/interiorPathFollower.h
	include/dif/objects/staticMesh.h
	include/dif/objects/trigger.h
	include/dif/objects/vehicleCollision.h
)

# Create the library
add_library(Dif STATIC ${DIF_SRC})
include_directories(include)
include_directories(3rdparty/glm)

# create CAPI DLL
set (C_API_SRC
	src/difAPI.cpp
	include/difAPI.h
)
add_library(DifPlugin SHARED ${C_API_SRC})
target_link_libraries(DifPlugin Dif)

# Stop OSX from putting "lib" in front of the name
set_target_properties(DifPlugin PROPERTIES PREFIX "")

# Plugin suffix should be .bundle on OSX, .dll on Windows
if (APPLE)
	set_target_properties(DifPlugin PROPERTIES SUFFIX ".bundle")
elseif (MSVC)
	set_target_properties(DifPlugin PROPERTIES SUFFIX ".dll")
endif()


###############################################################################
# Tests
###############################################################################

add_executable(DifTests tests/main.cpp)
target_link_libraries(DifTests Dif)

# Make it go to where we want. Default is the plugins directory but you can change
# this if you're not building as a submodule.
option (OUTPUT_DIRECTORY_CUSTOM "Use a custom output directory." OFF)
set (OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../Assets/Plugins/" CACHE PATH "Path where the built plugin will go")
if (OUTPUT_DIRECTORY_CUSTOM)
	target_output_directory(DifPlugin "${OUTPUT_DIRECTORY}")
endif ()
