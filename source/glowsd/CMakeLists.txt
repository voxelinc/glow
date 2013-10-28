
# Target
set(target glowsd)
message(STATUS "lib ${target}")

# Libraries
set(libs
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARY}
    ${GLOW_LIBRARY}
)

# Definitions
add_definitions("-DGLOWSD_EXPORTS")

# Sources
set(header_path "${GLOW_SOURCE_DIR}/include/glowsd")
set(source_path "${CMAKE_CURRENT_SOURCE_DIR}")

set(api_includes
    ${header_path}/glowsd.h

    ${header_path}/AbstractUniform.h
    ${header_path}/Buffer.h
    ${header_path}/FrameBufferAttachment.h
    ${header_path}/FrameBufferObject.h
    ${header_path}/Object.h
    ${header_path}/Program.h
    ${header_path}/Program.hpp
    ${header_path}/Query.h
    ${header_path}/Referenced.h
    ${header_path}/RenderBufferAttachment.h
    ${header_path}/RenderBufferObject.h
    ${header_path}/Shader.h
    ${header_path}/ShaderCode.h
    ${header_path}/ShaderFile.h
    ${header_path}/ShaderSource.h
    ${header_path}/SharedData.h
    ${header_path}/SharedData.hpp
    ${header_path}/Texture.h
    ${header_path}/TextureAttachment.h
    ${header_path}/TransformFeedback.h
    ${header_path}/Uniform.h
    ${header_path}/Uniform.hpp
    ${header_path}/VertexArrayObject.h
    ${header_path}/VertexAttributeBinding.h
)

set(sources
    ${source_path}/AbstractUniform.cpp
    ${source_path}/Buffer.cpp
    ${source_path}/FrameBufferAttachment.cpp
    ${source_path}/FrameBufferObject.cpp
    ${source_path}/Object.cpp
    ${source_path}/Program.cpp
    ${source_path}/Query.cpp
    ${source_path}/Referenced.cpp
    ${source_path}/RenderBufferAttachment.cpp
    ${source_path}/RenderBufferObject.cpp
    ${source_path}/Shader.cpp
    ${source_path}/ShaderCode.cpp
    ${source_path}/ShaderFile.cpp
    ${source_path}/ShaderSource.cpp
    ${source_path}/Texture.cpp
    ${source_path}/TextureAttachment.cpp
    ${source_path}/TransformFeedback.cpp
    ${source_path}/Uniform.cpp
    ${source_path}/VertexArrayObject.cpp
    ${source_path}/VertexAttributeBinding.cpp
)

# Group source files
set(glow_header_group "Header Files (API)")
set(glow_source_group "Source Files")
source_group_by_path(${header_path} "\\\\.h$|\\\\.hpp$" 
    ${glow_header_group} ${api_includes})
source_group_by_path(${source_path} "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$" 
    ${glow_source_group} ${sources})

# Build library
if(OPTION_BUILD_STATIC)
    add_library(${target} ${api_includes} ${sources})
else()
    add_library(${target} SHARED ${api_includes} ${sources})
endif()
target_link_libraries(${target} ${libs})
set_target_properties(${target}
    PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_DEFINITIONS_DEBUG   "${DEFAULT_COMPILE_DEFS_DEBUG}"
    COMPILE_DEFINITIONS_RELEASE "${DEFAULT_COMPILE_DEFS_RELEASE}"
    COMPILE_FLAGS               "${DEFAULT_COMPILE_FLAGS}"
    LINK_FLAGS_DEBUG            "${DEFAULT_LINKER_FLAGS_DEBUG}"
    LINK_FLAGS_RELEASE          "${DEFAULT_LINKER_FLAGS_RELEASE}"
    DEBUG_POSTFIX               "d${DEBUG_POSTFIX}"
    INCLUDE_PATH	            "${GLOW_SOURCE_DIR}/include")

# Deployment
install(TARGETS ${target}
    RUNTIME DESTINATION ${INSTALL_BIN}
    LIBRARY DESTINATION ${INSTALL_SHARED}
    ARCHIVE DESTINATION ${INSTALL_LIB}
)
