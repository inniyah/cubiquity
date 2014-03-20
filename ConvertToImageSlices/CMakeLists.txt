project(ConvertToImageSlices)

include_directories(${CubiquityCore_SOURCE_DIR} ${CubiquityC_SOURCE_DIR})

add_executable (ConvertToImageSlices main.cpp CmdOption.cpp) 

set_target_properties(ConvertToImageSlices PROPERTIES COMPILE_FLAGS "-std=c++0x -arch i386")

set_target_properties(ConvertToImageSlices PROPERTIES LINK_FLAGS "-arch i386 -static-libstdc++")

target_link_libraries (ConvertToImageSlices CubiquityC) 