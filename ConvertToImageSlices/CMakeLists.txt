project(ConvertToImageSlices)

include_directories(${CubiquityCore_SOURCE_DIR} ${CubiquityC_SOURCE_DIR})

add_executable (ConvertToImageSlices main.cpp CmdOption.cpp) 

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set_target_properties(ConvertToImageSlices PROPERTIES COMPILE_FLAGS "-std=c++0x -arch i386")

	set_target_properties(ConvertToImageSlices PROPERTIES LINK_FLAGS "-arch i386 -static-libstdc++")
endif()

target_link_libraries (ConvertToImageSlices CubiquityC) 