#include "OculusRiftCamera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace {
	const float g_pi = 3.141592654;
}

namespace glow {

float OculusRiftCamera::s_eyeHeadDistance = 0.07;
float OculusRiftCamera::s_eyeAngle = 0.05 * g_pi;
float OculusRiftCamera::s_eyeSeperation = 0.1;

OculusRiftCamera::OculusRiftCamera()
: m_focalLength(0.0f)
, m_nearPlane(0.0f)
, m_farPlane(0.0f)
{
}

OculusRiftCamera::~OculusRiftCamera()
{
}

glm::vec4 OculusRiftCamera::leftMatrices(glm::mat4& modelView, glm::mat4& projection)
{
	glm::vec3 r = glm::normalize(glm::cross(m_eye, m_up)) * glm::vec3(s_eyeSeperation / 2.0f);
	
	float ratio = m_viewport.x / m_viewport.y;
	float radians = (2.0 * g_pi / 360.0) * m_aperture.length() / 2.0;
	float wd2 = m_nearPlane * tan(radians);
	float ndfl = m_nearPlane / m_focalLength;
	
	float left = -ratio * wd2 + 0.5 * s_eyeSeperation * ndfl;
	float right = ratio * wd2 + 0.5 * s_eyeSeperation * ndfl;
	float top = wd2;
	float bottom = -wd2;
	
	projection = glm::frustum(
		left,
		right,
		bottom,
		top,
		m_nearPlane,
		m_farPlane
	);
	
	modelView = glm::lookAt(
		m_center + r,
		m_center + r + m_eye,
		m_up
	);
}

glm::vec4 OculusRiftCamera::rightMatrices(glm::mat4& modelView, glm::mat4& projection)
{
	glm::vec3 r = glm::normalize(glm::cross(m_eye, m_up)) * glm::vec3(s_eyeSeperation / 2.0f);
	
	float ratio = m_viewport.x / m_viewport.y;
	float radians = (2.0 * g_pi / 360.0) * m_aperture.length() / 2.0;
	float wd2 = m_nearPlane * tan(radians);
	float ndfl = m_nearPlane / m_focalLength;
	
	float left = -ratio * wd2 - 0.5 * s_eyeSeperation * ndfl;
	float right = ratio * wd2 - 0.5 * s_eyeSeperation * ndfl;
	float top = wd2;
	float bottom = -wd2;
	
	projection = glm::frustum(
		left,
		right,
		bottom,
		top,
		m_nearPlane,
		m_farPlane
	);
	
	modelView = glm::lookAt(
		m_center - r,
		m_center - r + m_eye,
		m_up
	);
}

const glm::vec3& OculusRiftCamera::center() const
{
	return m_center;
}

void OculusRiftCamera::setCenter(const glm::vec3& center)
{
	m_center = center;
}

const glm::vec3& OculusRiftCamera::eye() const
{
	return m_eye;
}

void OculusRiftCamera::setEye(const glm::vec3& eye)
{
	m_eye = eye;
}

const glm::vec3& OculusRiftCamera::up() const
{
	return m_up;
}

void OculusRiftCamera::setUp(const glm::vec3& up)
{
	m_up = up;
}

const glm::vec2& OculusRiftCamera::viewport() const
{
	return m_viewport;
}

void OculusRiftCamera::setViewport(const glm::vec2& viewport)
{
	m_viewport = viewport;
}

const glm::vec2& OculusRiftCamera::aperture() const
{
	return m_aperture;
}

void OculusRiftCamera::setAperture(const glm::vec2& aperture)
{
	m_aperture = aperture;
}

float OculusRiftCamera::focalLength() const
{
	return m_focalLength;
}

void OculusRiftCamera::setFocalLength(float focalLength)
{
	m_focalLength = focalLength;
}

float OculusRiftCamera::nearPlane() const
{
	return m_nearPlane;
}

void OculusRiftCamera::setNearPlane(float nearPlane)
{
	m_nearPlane = nearPlane;
}

float OculusRiftCamera::farPlane() const
{
	return m_farPlane;
}

void OculusRiftCamera::setFarPlane(float farPlane)
{
	m_farPlane = farPlane;
}

} // namespace glow
