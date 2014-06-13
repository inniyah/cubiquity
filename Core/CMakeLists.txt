project(CubiquityC)

include_directories(polyvox/library/PolyVoxCore/include/)

set(SRCS
	polyvox/library/PolyVoxCore/source/ArraySizes.cpp
	polyvox/library/PolyVoxCore/source/AStarPathfinder.cpp
	polyvox/library/PolyVoxCore/source/Region.cpp
	
	polyvox/library/PolyVoxCore/source/Impl/ErrorHandling.cpp
	polyvox/library/PolyVoxCore/source/Impl/Logging.cpp
	polyvox/library/PolyVoxCore/source/Impl/MarchingCubesTables.cpp
	polyvox/library/PolyVoxCore/source/Impl/MinizWrapper.cpp
	polyvox/library/PolyVoxCore/source/Impl/RandomUnitVectors.cpp
	polyvox/library/PolyVoxCore/source/Impl/RandomVectors.cpp
	polyvox/library/PolyVoxCore/source/Impl/Timer.cpp
	polyvox/library/PolyVoxCore/source/Impl/Utility.cpp

	BackgroundTaskProcessor.cpp
	Brush.cpp
	Clock.cpp
	Color.cpp
	ColoredCubesVolume.cpp
	ColoredCubicSurfaceExtractionTask.cpp
	Logging.cpp
	MainThreadTaskProcessor.cpp
	MaterialSet.cpp
	Raycasting.cpp
	SmoothSurfaceExtractionTask.cpp
	TerrainVolume.cpp
	TerrainVolumeEditor.cpp
	Task.cpp
	TerrainVolumeGenerator.cpp
	VoxelDatabase.cpp
	
	CubiquityC.cpp
)

add_library(_sqlite3 STATIC SQLite/sqlite3.c)
set_target_properties(_sqlite3 PROPERTIES COMPILE_FLAGS "-fPIC")

add_library(CubiquityC SHARED ${SRCS})

set_target_properties(CubiquityC PROPERTIES COMPILE_FLAGS "-std=c++0x -DCUBIQUITYC_EXPORTS")

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set_target_properties(cubiquitycore PROPERTIES COMPILE_FLAGS "-fPIC -std=c++0x -DCUBIQUITYC_EXPORTS")
	if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
		set_target_properties(cubiquitycore PROPERTIES LINK_FLAGS "-static-libstdc++ -static-libgcc")
	endif()
endif()

#if(CMAKE_SYSTEM_NAME MATCHES "Linux")
#	target_link_libraries(cubiquitycore ${PolyVox_PREFIX}/lib/libPolyVoxCore.a)
#elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
#	target_link_libraries(cubiquitycore ${PolyVox_PREFIX}/lib/libPolyVoxCore.a)
#elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
#	target_link_libraries(cubiquitycore ${PolyVox_PREFIX}/PolyVoxCore/lib/libPolyVoxCore.a)
#endif()

# The code below was copied from the old separate 'C' wrapper. I'm not sure we need it but it's here for safe keeping.
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	#
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	target_link_libraries(CubiquityC /home/matt/osxcross/build/libcxx-3.4/build/lib/libc++.a /home/matt/osxcross/target/SDK/MacOSX10.8.sdk/usr/lib/libc++abi.dylib)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	#
endif()

target_link_libraries(CubiquityC _sqlite3)
