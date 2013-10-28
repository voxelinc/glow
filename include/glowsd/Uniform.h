#pragma once

#include <glow/glow.h>
#include <glow/AbstractUniform.h>

namespace glow
{

/** \brief The Uniform template class wraps access to typed global GLSL variables.

	The Uniform class wraps access to typed global GLSL variables (uniforms).
	These are stored in the OpenGL program objects itself.

	Supported OpenGL uniform setters are wrapped via specialized template set
	implementations. Note that unsupported uniform types result in compile time
	errors due to the default implementation of set.

	Simple usage of an Uniform:
	\code{.cpp}
		Uniform<float> * u(new Uniform<float>("u_ratio"));
		u->set(1.618f);

		program->addUniform(u);
	\endcode

	\see AbstractUniform
	\see Program
	\see http://www.opengl.org/wiki/Uniform
*/
template<typename T>
class GLOW_API Uniform : public AbstractUniform
{
public:
    Uniform(const std::string & name);
    Uniform(const std::string & name, const T & value);
    virtual ~Uniform();

    void set(const T & value);

    const T & value() const;

protected:
	virtual void setLocation(const GLint location);
	void set(
		const GLint location
	,	const T & value);

protected:
    T m_value; ///< The uniforms value, explictly required when relinking programs.
};

} // namespace glow

#include <glow/Uniform.hpp>
