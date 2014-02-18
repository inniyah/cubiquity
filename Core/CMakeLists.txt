project(CubiquityCore)

set(SRCS
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
)

add_library(_sqlite3 STATIC SQLite/sqlite3.c)
set_target_properties(_sqlite3 PROPERTIES COMPILE_FLAGS "-fPIC")

add_library(cubiquitycore STATIC ${SRCS})

set_target_properties(cubiquitycore PROPERTIES COMPILE_FLAGS "-fPIC -std=c++0x")
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set_target_properties(cubiquitycore PROPERTIES LINK_FLAGS "-static-libstdc++ -static-libgcc")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(cubiquitycore ${PolyVox_PREFIX}/lib/libPolyVoxCore.a)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	target_link_libraries(cubiquitycore ${PolyVox_PREFIX}/lib/libPolyVoxCore.a)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	target_link_libraries(cubiquitycore ${PolyVox_PREFIX}/PolyVoxCore/lib/libPolyVoxCore.a)
endif()

target_link_libraries(cubiquitycore _sqlite3)
