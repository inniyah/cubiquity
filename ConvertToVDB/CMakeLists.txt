project(ConvertToVDB)

include_directories(${CubiquityCore_SOURCE_DIR} ${CubiquityC_SOURCE_DIR})

add_executable (ConvertToVDB CmdOption.cpp ImportImageSlices.cpp ImportMagicaVoxel.cpp ImportVXL.cpp main.cpp stb_image.cpp) 

set_target_properties(ConvertToVDB PROPERTIES COMPILE_FLAGS "-stdlib=libc++ -std=c++0x -arch i386")

set_target_properties(ConvertToVDB PROPERTIES LINK_FLAGS "-stdlib=libc++ -arch i386")

target_link_libraries (ConvertToVDB CubiquityC) 