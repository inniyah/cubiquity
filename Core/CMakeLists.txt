project(CubiquityC)

include_directories(
	Dependancies/
	Dependancies/polyvox/library/PolyVoxCore/include/
)

set(SRCS
	# PolyVox code
	Dependancies/polyvox/library/PolyVoxCore/source/ArraySizes.cpp
	Dependancies/polyvox/library/PolyVoxCore/source/AStarPathfinder.cpp
	Dependancies/polyvox/library/PolyVoxCore/source/Region.cpp
	
	Dependancies/polyvox/library/PolyVoxCore/source/Impl/ErrorHandling.cpp
	Dependancies/polyvox/library/PolyVoxCore/source/Impl/Logging.cpp
	Dependancies/polyvox/library/PolyVoxCore/source/Impl/MarchingCubesTables.cpp
	Dependancies/polyvox/library/PolyVoxCore/source/Impl/MinizWrapper.cpp
	Dependancies/polyvox/library/PolyVoxCore/source/Impl/RandomUnitVectors.cpp
	Dependancies/polyvox/library/PolyVoxCore/source/Impl/RandomVectors.cpp
	Dependancies/polyvox/library/PolyVoxCore/source/Impl/Timer.cpp
	Dependancies/polyvox/library/PolyVoxCore/source/Impl/Utility.cpp
	
	# SQLite code
	Dependancies/SQLite/sqlite3.c

	# Main Cubiquity code
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
	
	# Cubiquity 'C' interface
	CubiquityC.cpp
)

add_library(CubiquityC SHARED ${SRCS})

# Set COMPILE_FLAGS
set_target_properties(CubiquityC PROPERTIES COMPILE_FLAGS "-DCUBIQUITYC_EXPORTS -DSQLITE_OMIT_LOAD_EXTENSION")

# Set LINK_FLAGS
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set_target_properties(CubiquityC PROPERTIES LINK_FLAGS "-static-libstdc++ -static-libgcc")
endif()

# Set target_link_libraries
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_link_libraries(CubiquityC pthread)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	target_link_libraries(CubiquityC)
endif()

# The code below was copied from the old separate 'C' wrapper. I'm not sure we need it but it's here for safe keeping.
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	#
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	target_link_libraries(CubiquityC /home/matt/osxcross/build/libcxx-3.4/build/lib/libc++.a /home/matt/osxcross/target/SDK/MacOSX10.8.sdk/usr/lib/libc++abi.dylib)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	#
endif()
