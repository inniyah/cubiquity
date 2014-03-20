cmake_minimum_required(VERSION 2.8)

set(PolyVox_PREFIX "" CACHE STRING "Location of PolyVox install")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	include_directories(${PolyVox_PREFIX}/include/PolyVoxCore/)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	include_directories(${PolyVox_PREFIX}/include/PolyVoxCore/)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	include_directories(${PolyVox_PREFIX}/PolyVoxCore/include/)
endif()

add_subdirectory(Core)
add_subdirectory(Wrappers/C)
add_subdirectory(Wrappers/CubiquityCTest)
add_subdirectory(ConvertToImageSlices)
add_subdirectory(ConvertToVDB)
