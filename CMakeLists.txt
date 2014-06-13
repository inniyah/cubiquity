cmake_minimum_required(VERSION 2.8)

project(Cubiquity)

#set(PolyVox_PREFIX "" CACHE STRING "Location of PolyVox install")

#if(CMAKE_SYSTEM_NAME MATCHES "Linux")
#	include_directories(${PolyVox_PREFIX}/include/PolyVoxCore/)
#elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
#	include_directories(${PolyVox_PREFIX}/include/PolyVoxCore/)
#elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
#	include_directories(${PolyVox_PREFIX}/PolyVoxCore/include/)
#endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(Core)
#add_subdirectory(Wrappers/C)
add_subdirectory(Wrappers/CubiquityCTest)

# Converter tools not needed on Linux as the Unity editor doesn't run there.
if(NOT CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_subdirectory(Tools/ConvertToImageSlices)
	add_subdirectory(Tools/ConvertToVDB)
endif()
