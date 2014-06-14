project(ConvertToImageSlices)

include_directories(${CubiquityCore_SOURCE_DIR} ${CubiquityC_SOURCE_DIR})

add_executable (ConvertToImageSlices main.cpp CmdOption.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  # GNU specific flags
  set_target_properties(ConvertToImageSlices PROPERTIES COMPILE_FLAGS "-std=c++0x")
  set_target_properties(ConvertToImageSlices PROPERTIES LINK_FLAGS "-static-libstdc++")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "CLANG")
  # Clang specific flags
  set_target_properties(ConvertToImageSlices PROPERTIES COMPILE_FLAGS "-std=c++0x -arch i386")
  set_target_properties(ConvertToImageSlices PROPERTIES LINK_FLAGS "-arch i386 -static-libstdc++")
elseif(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Visual Studio specific flags
endif()

# Why do we need 'dl pthread' here? Should these have been used when building the main library instead?
target_link_libraries (ConvertToImageSlices CubiquityC dl pthread) 