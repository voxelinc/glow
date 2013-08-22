#pragma once

#include <glow/glow.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <functional>

namespace glow {

class OculusRiftCamera
{
public:
	typedef std::function<void()> DrawCallback;
	
	OculusRiftCamera();
	virtual ~OculusRiftCamera();

	glm::vec4 leftMatrices(glm::mat4& modelView, glm::mat4& projection);
	glm::vec4 rightMatrices(glm::mat4& modelView, glm::mat4& projection);
	
	const glm::vec3& center() const;
	void setCenter(const glm::vec3& center);
	
	const glm::vec3& eye() const;
	void setEye(const glm::vec3& eye);
	
	const glm::vec3& up() const;
	void setUp(const glm::vec3& up);
	
	const glm::vec2& viewport() const;
	void setViewport(const glm::vec2& viewport);
	
	const glm::vec2& aperture() const;
	void setAperture(const glm::vec2& aperture);
	
	float focalLength() const;
	void setFocalLength(float focalLength);
	
	float nearPlane() const;
	void setNearPlane(float nearPlane);
	
	float farPlane() const;
	void setFarPlane(float farPlane);
protected:
	glm::vec3 m_center;
	glm::vec3 m_eye;
	glm::vec3 m_up;
	glm::vec2 m_viewport;
	glm::vec2 m_aperture;
	float m_focalLength;
	float m_nearPlane;
	float m_farPlane;
	
	static float s_eyeHeadDistance;
	static float s_eyeAngle;
	static float s_eyeSeperation;
};

} // namespace glow
