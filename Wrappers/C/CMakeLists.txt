project(CubiquityC)

include_directories(${CubiquityCore_SOURCE_DIR} ${CubiquityCore_SOURCE_DIR}/polyvox/library/PolyVoxCore/include/)

set(SRCS
	CubiquityC.cpp
)

add_library(CubiquityC SHARED ${SRCS})

set_target_properties(CubiquityC PROPERTIES COMPILE_FLAGS "-std=c++0x -DCUBIQUITYC_EXPORTS")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set_target_properties(CubiquityC PROPERTIES LINK_FLAGS "-static-libstdc++ -static-libgcc")
endif()

target_link_libraries(CubiquityC cubiquitycore)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	#
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	target_link_libraries(CubiquityC /home/matt/osxcross/build/libcxx-3.4/build/lib/libc++.a /home/matt/osxcross/target/SDK/MacOSX10.8.sdk/usr/lib/libc++abi.dylib)
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	#
endif()
