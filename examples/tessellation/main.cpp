
#include <GL/glew.h>

#include <algorithm>
#include <random>
#include <cassert>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <glow/Error.h>
#include <glow/Uniform.h>
#include <glow/Program.h>
#include <glow/Shader.h>
#include <glow/Buffer.h>
#include <glow/logging.h>
#include <glow/debugmessageoutput.h>

#include <glowutils/Icosahedron.h>
#include <glowutils/AdaptiveGrid.h>
#include <glowutils/Camera.h>
#include <glowutils/FileRegistry.h>
#include <glowutils/File.h>
#include <glowutils/AutoTimer.h>
#include <glowutils/Timer.h>
#include <glowutils/global.h>

#include <glowwindow/ContextFormat.h>
#include <glowwindow/Context.h>
#include <glowwindow/Window.h>
#include <glowwindow/WindowEventHandler.h>

using namespace glowwindow;
using namespace glm;


class EventHandler : public WindowEventHandler
{
public:
    EventHandler()
    : m_camera(vec3(0.f, 1.f, 4.0f))
    {
    }

    virtual ~EventHandler()
    {
    }

    virtual void initialize(Window & ) override
    {
        glow::debugmessageoutput::enable();

        glClearColor(1.0f, 1.0f, 1.0f, 0.f);
        CheckGLError();

        m_sphere = new glow::Program();
        m_sphere->attach(
            glowutils::createShaderFromFile(GL_VERTEX_SHADER, "data/tessellation/sphere.vert")
        ,   glowutils::createShaderFromFile(GL_TESS_CONTROL_SHADER, "data/tessellation/sphere.tcs")
        ,   glowutils::createShaderFromFile(GL_TESS_EVALUATION_SHADER, "data/tessellation/sphere.tes")
        ,   glowutils::createShaderFromFile(GL_GEOMETRY_SHADER, "data/tessellation/sphere.geom")
        ,   glowutils::createShaderFromFile(GL_FRAGMENT_SHADER, "data/tessellation/sphere.frag")
        ,   glowutils::createShaderFromFile(GL_FRAGMENT_SHADER, "data/common/phong.frag"));

        m_icosahedron = new glowutils::Icosahedron();
        m_agrid = new glowutils::AdaptiveGrid(16U);

        m_time.reset();
        m_time.start();

        m_camera.setZNear(0.1f);
        m_camera.setZFar(16.f);

        m_agrid->setCamera(&m_camera);
    }

    virtual void framebufferResizeEvent(ResizeEvent & event) override
    {
        int width = event.width();
        int height = event.height();

        glViewport(0, 0, width, height);
        CheckGLError();

        m_camera.setViewport(width, height);
    }

    virtual void paintEvent(PaintEvent &) override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        CheckGLError();

        m_agrid->update();

        float t = static_cast<float>(m_time.elapsed() * 4e-10f);
        mat4 R = glm::rotate(t * 10.f, vec3(sin(t * 0.321), cos(t * 0.234), sin(t * 0.123)));

        m_sphere->setUniform("transform", m_camera.viewProjection());
        m_sphere->setUniform("rotation", R);

        int level = int((sin(t) * 0.5 + 0.5) * 16) + 1;
        m_sphere->setUniform("level", level);

        m_sphere->use();
        glPatchParameteri(GL_PATCH_VERTICES, 3);
        m_icosahedron->draw(GL_PATCHES);
        m_sphere->release();

        m_agrid->draw();
    }

    virtual void idle(Window & window) override
    {
        window.repaint();
    }

    virtual void keyPressEvent(KeyEvent & event) override
    {
        switch (event.key())
        {
        case GLFW_KEY_F5:
            glowutils::FileRegistry::instance().reloadAll();
            break;
        }
    }

protected:
    glow::ref_ptr<glow::Program> m_sphere;

    glow::ref_ptr<glowutils::Icosahedron> m_icosahedron;
    glow::ref_ptr<glowutils::AdaptiveGrid> m_agrid;

    glowutils::Camera m_camera;
    glowutils::Timer m_time;

    vec3 m_rand;
};


/** This example shows ... .
*/
int main(int /*argc*/, char* /*argv*/[])
{
    ContextFormat format;
    format.setVersion(4, 0);
    format.setProfile(ContextFormat::CoreProfile);
    format.setDepthBufferSize(16);

    Window window;

    window.setEventHandler(new EventHandler());

    if (window.create(format, "Post Processing Example"))
    {
        window.context()->setSwapInterval(Context::VerticalSyncronization);

        window.show();

        return MainLoop::run();
    }
    else
    {
        return 1;
    }
}
