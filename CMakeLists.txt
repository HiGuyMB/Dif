cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
include (CheckIncludeFiles)
project (Dif C CXX)

# Allow C++11 features
list (APPEND CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

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
	include/dif/base/point2.h
	include/dif/base/point3.h
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
