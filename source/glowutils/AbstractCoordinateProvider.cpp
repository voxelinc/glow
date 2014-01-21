
#include <GL/glew.h>

#include <cfloat>

#include <glm/glm.hpp>

#include <glow/Error.h>

#include <glowutils/Camera.h>

#include <glowutils/AbstractCoordinateProvider.h>


using namespace glm;

namespace glowutils
{

float AbstractCoordinateProvider::depthAt(
    const Camera & camera
,   const GLenum format
,   const ivec2 & windowCoordinates)
{
    const GLint x(windowCoordinates.x);
    const GLint y(windowCoordinates.y);

    const GLint w(static_cast<GLint>(camera.viewport().x));
    const GLint h(static_cast<GLint>(camera.viewport().y));

    if (x >= w || y >= h)
        return 1.f;

    GLfloat z;
    glReadPixels(x, h - y - 1, 1, 1, format, GL_FLOAT, reinterpret_cast<void*>(&z));
    CheckGLError();

    return z;
}

bool AbstractCoordinateProvider::validDepth(const float depth)
{
    return depth < (1.f - std::numeric_limits<float>::epsilon());
}

const vec3 AbstractCoordinateProvider::unproject(
    const Camera & camera
,   const GLenum depthFormat
,   const ivec2 & windowCoordinates)
{
    const glm::mat4 viewProjectionInverted = camera.viewProjectionInverted();
    const float depth = depthAt(camera, depthFormat, windowCoordinates);

    const float x = static_cast<float>(windowCoordinates.x);
    const float y = static_cast<float>(windowCoordinates.y);

    // transform viewport to [-1;+1] (including z!)

    const float w = 2.0f / static_cast<float>(camera.viewport().x);
    const float h = 2.0f / static_cast<float>(camera.viewport().y);

    const vec4 p = vec4(x * w - 1.f, 1.f - y * h, depth * 2.f - 1.f, 1.f);

    // unproject this point back to object space
    const vec4 u = viewProjectionInverted * p;
    return vec3(u) / u.w;
}

const vec3 AbstractCoordinateProvider::unproject(
    const Camera & camera
,   const float depth
,   const ivec2 & windowCoordinates)
{
    return unproject(camera, camera.viewProjectionInverted(), depth, windowCoordinates);
}

const vec3 AbstractCoordinateProvider::unproject(
    const Camera & camera
,   const mat4 & viewProjectionInverted
,   const float depth
,   const ivec2 & windowCoordinates)
{
    const float x = static_cast<float>(windowCoordinates.x);
    const float y = static_cast<float>(windowCoordinates.y);

    // transform viewport to [-1;+1] (including z!)

    const float w = 2.0f / static_cast<float>(camera.viewport().x);
    const float h = 2.0f / static_cast<float>(camera.viewport().y);

    const vec4 p = vec4(x * w - 1.f, 1.f - y * h, depth * 2.f - 1.f, 1.f);

    // unproject this point back to object space
    const vec4 u = viewProjectionInverted * p;
    return vec3(u) / u.w;
}

glm::vec3 AbstractCoordinateProvider::objAt(const glm::ivec2 & windowCoordinates)
{
    return objAt(windowCoordinates, depthAt(windowCoordinates));
}

} // namespace glowutils
