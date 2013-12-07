#pragma once

#include <glow/glow.h>
#include <glow/Object.h>
#include <glow/TextureHandle.h>

namespace glow 
{

/** \brief Wraps OpenGL texture objects.
    
    A Texture provides both interfaces to bind them for the OpenGL pipeline:
    binding and bindless texture. Bindless textures are only available if the
    graphics driver supports them (NV extension).
    
    \see http://www.opengl.org/wiki/Texture
    \see http://www.opengl.org/registry/specs/NV/bindless_texture.txt
 */
class GLOW_API Texture : public Object
{
public:
	Texture(GLenum  target = GL_TEXTURE_2D);
	Texture(GLuint id, GLenum  target, bool ownsGLObject = true);
	virtual ~Texture();

	virtual void accept(ObjectVisitor & visitor);

	void bind() const;
    void bind(GLenum texture) const;

    void unbind() const;
    void unbind(GLenum texture) const;

	void setParameter(GLenum name, GLint value);
	void setParameter(GLenum name, GLfloat value);

	GLint getParameter(GLenum pname);
	GLint getLevelParameter(GLint level, GLenum pname);

	GLenum target() const;

	void compressedImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data);
	void compressedImage2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data, GLenum targetOverride);
	void image1D(GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* data);
	void image2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data);
	void image2D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data, GLenum targetOverride);
	void image3D(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * data);	
	void storage2D(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);

	void bindImageTexture(
        GLuint unit
    ,   GLint level
    ,   GLboolean layered
    ,   GLint layer
    ,   GLenum access
    ,   GLenum format);

	void generateMipmap();

    TextureHandle textureHandle() const;

	GLboolean isResident() const;
    TextureHandle makeResident();
	void makeNonResident();

protected:
    static GLuint genTexture();

protected:
    GLenum m_target;
};

} // namespace glow
