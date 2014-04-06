#include <glow/UniformBlock.h>
#include <glow/Program.h>
#include <glow/Buffer.h>

namespace glow {

UniformBlock::UniformBlock()
: m_program(nullptr)
{
}

UniformBlock::UniformBlock(Program * program, const LocationIdentity & identity)
: m_program(program)
, m_identity(identity)
{
}

const LocationIdentity & UniformBlock::identity() const
{
    return m_identity;
}

void UniformBlock::setBinding(GLuint bindingIndex)
{
    m_bindingIndex = bindingIndex;

    m_program->checkDirty();

    glUniformBlockBinding(m_program->id(), blockIndex(), m_bindingIndex);
    CheckGLError();
}

GLuint UniformBlock::blockIndex()
{
    if (m_identity.isLocation())
        return m_identity.location();

    if (m_identity.isName())
        return m_program->getUniformBlockIndex(m_identity.name());

    return GL_INVALID_INDEX;
}

void UniformBlock::updateBinding()
{
    setBinding(m_bindingIndex);
}

void UniformBlock::getActive(GLenum pname, GLint * params)
{
    m_program->checkDirty();

    glGetActiveUniformBlockiv(m_program->id(), blockIndex(), pname, params);
    CheckGLError();
}

GLint UniformBlock::getActive(GLenum pname)
{
    GLint result = 0;
    getActive(pname, &result);
    return result;
}

std::vector<GLint> UniformBlock::getActive(GLenum pname, GLint paramCount)
{
    std::vector<GLint> result(paramCount);
    getActive(pname, result.data());
    return result;
}

std::vector<GLint> UniformBlock::getActiveUniformIndices()
{
    return getActive(GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, getActive(GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS));
}

std::string UniformBlock::getName()
{
    if (m_identity.isName())
        return m_identity.name();

    GLint length = getActive(GL_UNIFORM_BLOCK_NAME_LENGTH);
    std::vector<char> name(length);

    glGetActiveUniformBlockName(m_program->id(), blockIndex(), length, nullptr, name.data());
    CheckGLError();

    return std::string(name.data(), length);
}

} // namespace glow
