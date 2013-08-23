cmake_minimum_required(VERSION 2.8)

set(target oculusrift)
project(${target})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(HIDAPI REQUIRED)

include_directories(
	.
	${GLOW_INCLUDE_DIR}
	${HIDAPI_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(${target} 
	main.cpp
	OculusRift.cpp
	OculusRift.h
	OculusRiftCamera.cpp
	OculusRiftCamera.h
)

target_link_libraries(${target} 
	${GLEW_LIBRARIES}
	${HIDAPI_LIBRARY}
	glow
)

set_target_properties(${target}
    PROPERTIES
    FOLDER ${GLOW_EXAMPLES_FOLDER}
    DEBUG_POSTFIX "d${DEBUG_POSTFIX}")

install(TARGETS ${target}
    RUNTIME DESTINATION ${INSTALL_EXAMPLES}
#    LIBRARY DESTINATION ${INSTALL_SHARED}
#    ARCHIVE DESTINATION ${INSTALL_LIB}
)
