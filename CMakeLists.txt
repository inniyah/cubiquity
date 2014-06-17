cmake_minimum_required(VERSION 2.8)

project(Cubiquity)

# For keeping things tidy in Visual Studio
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# To run the examples/tools from Visual Studio we need the built executables and libraries to
# be in the same folder. See http://www.cmake.org/pipermail/cmake/2006-September/011249.html
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# We set the CXX flags here as 'global' options, and then add to them as required using set_target_properties()
# See: http://stackoverflow.com/questions/5096881/does-set-target-properties-in-cmake-override-cmake-cxx-flags
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "-stdlib=libc++ -arch i386 ${CMAKE_CXX_FLAGS}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(CMAKE_CXX_FLAGS "-fPIC -std=c++0x ${CMAKE_CXX_FLAGS}")
endif()

#Build the various projects
add_subdirectory(Core)
add_subdirectory(Examples/CubiquityCTest)
add_subdirectory(Examples/OpenGL)
add_subdirectory(Tools/ConvertToImageSlices)
add_subdirectory(Tools/ConvertToVDB)
