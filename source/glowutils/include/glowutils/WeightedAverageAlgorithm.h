#pragma once

#include <glowutils/glowutils.h>
#include <glowutils/AbstractTransparencyAlgorithm.h>

namespace glow {

class Program;
class FrameBufferObject;
class Texture;
class RenderBufferObject;
class Buffer;

}

namespace glowutils {

class Camera;
class ScreenAlignedQuad;

/**
    Implements the weighted average transparency algorithm.
    Expected shader files:
        wavg_opaque.frag		- renders the opaque geometry
        wavg_translucent.frag	- renders the transcluent geometry and counts the fragments per pixel
        wavg_post.frag			- combines the opaque and transcluent geometry using weighted average
*/
class GLOWUTILS_API WeightedAverageAlgorithm : public AbstractTransparencyAlgorithm
{
public:
    virtual void initialize(const std::string & transparencyShaderFilePath, glow::Shader *vertexShader, glow::Shader *geometryShader) override;
    virtual void draw(const DrawFunction& drawFunction, Camera* camera, int width, int height) override;
    virtual void resize(int width, int height) override;
    virtual glow::Texture* getOutput() override;

private:
    // geometry pass
    glow::ref_ptr<glow::Program> m_opaqueProgram;
    glow::ref_ptr<glow::Program> m_accumulationProgram;
    glow::ref_ptr<glow::Texture> m_opaqueBuffer;
    glow::ref_ptr<glow::Texture> m_accumulationBuffer;
    glow::ref_ptr<glow::RenderBufferObject> m_depthBuffer;
    glow::ref_ptr<glow::FrameBufferObject> m_renderFbo;
    glow::ref_ptr<glow::Buffer> m_depthComplexityBuffer;

    // post processing pass
    glow::ref_ptr<ScreenAlignedQuad> m_quad;
    glow::ref_ptr<glow::FrameBufferObject> m_postFbo;
    glow::ref_ptr<glow::Texture> m_colorBuffer;
};

} // namespace glow
