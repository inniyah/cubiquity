cmake_minimum_required(VERSION 2.8)

project(Cubiquity)

# To run the examples/tools from Visual Studio we need the built executables and libraries to
# be in the same folder. See http://www.cmake.org/pipermail/cmake/2006-September/011249.html
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "-stdlib=libc++ -arch i386 ${CMAKE_CXX_FLAGS}")
endif()

#Build the various projects
add_subdirectory(Core)
add_subdirectory(Examples/CubiquityCTest)
add_subdirectory(Examples/OpenGL)
add_subdirectory(Tools/ConvertToImageSlices)
add_subdirectory(Tools/ConvertToVDB)
