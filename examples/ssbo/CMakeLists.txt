cmake_minimum_required(VERSION 2.8)

# Project
project(SSBOExample)
set(target ssboexample)

# Find externals
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(FREEGLUT REQUIRED)

# Include directories
include_directories(
	.
	..
	${GLM_INCLUDE_PATH}
	${GLEW_INCLUDE_PATH}
	${FREEGLUT_INCLUDE_PATH}
	${GLOW_INCLUDE_DIR}
)

# Libraries
set(libs
	${GLEW_LIBRARIES}
	${FREEGLUT_LIBRARY}
	glow
)

# Sources
set(headers
	SSBOWindow.h
	../GlutWindow.h
)
set(sources
	main.cpp
	SSBOWindow.cpp
	../GlutWindow.cpp
)

# Build executable
add_executable(${target} ${sources} ${headers})
target_link_libraries(${target} ${libs})

# Deployment
install(TARGETS ${target}
    RUNTIME DESTINATION ${INSTALL_EXAMPLES}
#   LIBRARY DESTINATION ${INSTALL_SHARED}
#   ARCHIVE DESTINATION ${INSTALL_LIB}
)
