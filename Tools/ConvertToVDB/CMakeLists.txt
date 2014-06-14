project(ConvertToVDB)

include_directories(${CubiquityCore_SOURCE_DIR} ${CubiquityC_SOURCE_DIR})

add_executable (ConvertToVDB CmdOption.cpp ImportImageSlices.cpp ImportMagicaVoxel.cpp ImportVXL.cpp main.cpp stb_image.cpp) 

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  # GNU specific flags
  set_target_properties(ConvertToVDB PROPERTIES LINK_FLAGS "-static-libstdc++")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "CLANG")
  # Clang specific flags
  set_target_properties(ConvertToVDB PROPERTIES LINK_FLAGS "-arch i386 -static-libstdc++")
elseif(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Visual Studio specific flags
endif()

target_link_libraries (ConvertToVDB CubiquityC pthread)
