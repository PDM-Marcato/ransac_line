#-----------------------------------------------------------------------
# - Define CMake requirements and override make rules as needed
#
cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

#-----------------------------------------------------------------------
# - Project definition and basic configuration
#   Version handled manually as project(... VERSION ...) is not used
#   in tests/examples which are subprojects. All calls must use
#   the same form.
project(Trackingliblineslines)
set(${PROJECT_NAME}_VERSION_MAJOR  0)
set(${PROJECT_NAME}_VERSION_MINOR  1)
set(${PROJECT_NAME}_VERSION_PATCH  0)
set(${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}")

# - Prepend our own CMake Modules to the search path
#
set(CMAKE_MODULE_PATH
    ${PROJECT_SOURCE_DIR}/cmake/Modules
    ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH})
list(APPEND CMAKE_PREFIX_PATH )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions( -DTIXML_USE_STL )

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)



#find_package(ROOT REQUIRED COMPONENTS Core RIO Tree)
#include_directories(${ROOT_INCLUDE_DIRS})

# - Find ROOT Modules
#
#list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
#find_package(ROOT REQUIRED COMPONENTS RIO)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++17 -pthread -Wno-deprecated-declarations" )
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lncurses" )
include_directories( ${CMAKE_SOURCE_DIR})
#link_directories(${ROOT_LIBRARY_DIR})



# - Configure output Modules
#
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

#set(HDRFILES include/*.h)
#set(SRCFILES src/*.cc)

file(GLOB SRCFILES ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB HDRFILES ${PROJECT_SOURCE_DIR}/include/*.h)



#ROOT_GENERATE_DICTIONARY(Trackingliblines_Dict ${HDRFILES} LINKDEF include/LinkDef.hh)
add_library(Trackingliblines SHARED ${HDRFILES} ${SRCFILES} include/LinkDef.hh)



# --------------------------------
# Add pybind11 locally
# --------------------------------
add_subdirectory(pybind11)  # Assuming pybind11/ is in your project root

# --------------------------------
# Python bindings target
# --------------------------------
# This will be a separate .cpp file just for Python bindings
set(PYTHON_BINDINGS ${PROJECT_SOURCE_DIR}/python_bindings.cpp)

# Build the Python module linking against your existing Trackingliblines
pybind11_add_module(pyTrackinglines ${PYTHON_BINDINGS})

# Link it to your existing library and ROOT
target_link_libraries(pyTrackinglines PRIVATE Trackingliblines -O3 -march=native)

# Output the compiled .so into your project source directory for easy import
set_target_properties(pyTrackinglines PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
