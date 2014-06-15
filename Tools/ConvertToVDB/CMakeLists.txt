project(ConvertToVDB)

include_directories(${CubiquityCore_SOURCE_DIR} ${CubiquityC_SOURCE_DIR})

add_executable (ConvertToVDB CmdOption.cpp ImportImageSlices.cpp ImportMagicaVoxel.cpp ImportVXL.cpp main.cpp stb_image.cpp) 

# Set LINK_FLAGS
if(CMAKE_CXX_COMPILER_ID MATCHES "CLANG")
  set_target_properties(ConvertToVDB PROPERTIES LINK_FLAGS "-arch i386 -static-libstdc++")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set_target_properties(ConvertToVDB PROPERTIES LINK_FLAGS "-static-libstdc++")
endif()

# Set target_link_libraries
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  # Note - It's ugly that we need pthread here. It happens because the include-chain causes 'sqlite3.h' to be included from the application.
  # Ideally 'CubiqutyC.h' should be changed so that it includes nothing but standard C libraries, so we only distribute that one header?
  target_link_libraries (ConvertToVDB CubiquityC pthread)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_link_libraries (ConvertToVDB CubiquityC)
endif()