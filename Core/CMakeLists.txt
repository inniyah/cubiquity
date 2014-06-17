project(CubiquityC)

################################################################################
# Build SQLite - Leading underscore to avoid conflict with system version?
################################################################################
add_library(_sqlite3 STATIC Dependancies/SQLite/sqlite3.c)
set_target_properties(_sqlite3 PROPERTIES COMPILE_FLAGS "-fPIC")
add_definitions( -DSQLITE_OMIT_LOAD_EXTENSION ) # Avoids needing 'dl' on Linux

# Organise the Visual Studio folders.
SET_PROPERTY(TARGET _sqlite3 PROPERTY FOLDER "Core/Dependancies")

################################################################################
# Build PolyVox - Leading underscore to avoid conflict with system version?
################################################################################
set(POLYVOX_SRCS
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
)

add_library(_PolyVoxCore STATIC ${POLYVOX_SRCS})

# Organise the Visual Studio folders.
SET_PROPERTY(TARGET _PolyVoxCore PROPERTY FOLDER "Core/Dependancies")

################################################################################
# Build Cubiquity
################################################################################
include_directories(
	Dependancies/
	Dependancies/polyvox/library/PolyVoxCore/include/
)

set(SRCS
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

# Organise the Visual Studio folders.
SET_PROPERTY(TARGET CubiquityC PROPERTY FOLDER "Core")

# Shared library must export symbols
add_definitions( -DCUBIQUITYC_EXPORTS )

# Set LINK_FLAGS
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set_target_properties(CubiquityC PROPERTIES LINK_FLAGS "-static-libstdc++ -static-libgcc")
endif()

# Set target_link_libraries
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	target_link_libraries(CubiquityC _sqlite3 _PolyVoxCore)
else()
	target_link_libraries(CubiquityC _sqlite3 _PolyVoxCore pthread)
endif()

# The code below was copied from the old separate 'C' wrapper. I'm not sure we need it but it's here for safe keeping.
#if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	#
#elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
#	target_link_libraries(CubiquityC /home/matt/osxcross/build/libcxx-3.4/build/lib/libc++.a /home/matt/osxcross/target/SDK/MacOSX10.8.sdk/usr/lib/libc++abi.dylib)
#elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	#
#endif()
