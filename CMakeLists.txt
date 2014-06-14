cmake_minimum_required(VERSION 2.8)

project(Cubiquity)

# To run the examples/tools from Visual Studio we need the built executables and libraries to
# be in the same folder. This is one approach, but maybe it's better to use a manual copy step?
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#Build the various projects
add_subdirectory(Core)
add_subdirectory(Examples/CubiquityCTest)
add_subdirectory(Tools/ConvertToImageSlices)
add_subdirectory(Tools/ConvertToVDB)
