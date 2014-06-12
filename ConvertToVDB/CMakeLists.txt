project(ConvertToVDB)

include_directories(${CubiquityCore_SOURCE_DIR} ${CubiquityC_SOURCE_DIR})

add_executable (ConvertToVDB CmdOption.cpp ImportImageSlices.cpp ImportMagicaVoxel.cpp ImportVXL.cpp main.cpp stb_image.cpp) 

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set_target_properties(ConvertToVDB PROPERTIES COMPILE_FLAGS "-stdlib=libc++ -std=c++0x -arch i386")

	set_target_properties(ConvertToVDB PROPERTIES LINK_FLAGS "-stdlib=libc++ -arch i386")
endif()

target_link_libraries (ConvertToVDB CubiquityC) 