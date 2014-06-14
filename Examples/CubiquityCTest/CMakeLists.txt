project(CubiquityCTest)

set(SRCS
	CubiquityCTest.cpp
)

include_directories(${CubiquityCore_SOURCE_DIR} ${CubiquityC_SOURCE_DIR})

add_executable(cubiquityctest ${SRCS})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set_target_properties(cubiquityctest PROPERTIES LINK_FLAGS "-static-libstdc++ -static-libgcc")
endif()

target_link_libraries(cubiquityctest CubiquityC)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(cubiquityctest dl pthread)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	#
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
	#
endif()
