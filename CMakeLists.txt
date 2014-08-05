cmake_minimum_required(VERSION 2.8)

# Force MSVC to statically link against the runtime.
# http://www.cmake.org/Wiki/CMake_FAQ#Make_Override_Files
set(CMAKE_USER_MAKE_RULES_OVERRIDE
   ${CMAKE_CURRENT_SOURCE_DIR}/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
   ${CMAKE_CURRENT_SOURCE_DIR}/cxx_flag_overrides.cmake)
   
project(Cubiquity)

# For keeping things tidy in Visual Studio
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# To run the examples/tools from Visual Studio we need the built executables and libraries to
# be in the same folder. See http://www.cmake.org/pipermail/cmake/2006-September/011249.html
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Use this toolset to ensure the resulting binaries run on Windows XP
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	# Note - Hardcoded value for VS2013. May need changing in the future.
	# Setting as described here: http://binglongx.wordpress.com/tag/cmake_vs_platform_toolset/
	#set( CMAKE_VS_PLATFORM_TOOLSET v120_xp )
	set(CMAKE_GENERATOR_TOOLSET "v120_xp" CACHE STRING "Platform Toolset" FORCE) 
endif()

# We set the CXX flags here as 'global' options, and then add to them as required using set_target_properties()
# See: http://stackoverflow.com/questions/5096881/does-set-target-properties-in-cmake-override-cmake-cxx-flags
# Note: Should we be using 'CMAKE_CXX_COMPILE_FLAGS' instead of 'CMAKE_CXX_FLAGS'? See http://goo.gl/rLqUyk
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_C_FLAGS "-arch i386 ${CMAKE_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "-stdlib=libc++ -arch i386 ${CMAKE_CXX_FLAGS}")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(CMAKE_C_FLAGS "-fPIC ${CMAKE_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "-fPIC -std=c++0x ${CMAKE_CXX_FLAGS}")
endif()

#Build the various projects
add_subdirectory(Core)
add_subdirectory(Examples/CubiquityCTest)
add_subdirectory(Examples/OpenGL)
add_subdirectory(Tools/ConvertToImageSlices)
add_subdirectory(Tools/ConvertToVDB)
