
#include <GL/glew.h>

#include <glow/AutoTimer.h>
#include <glowwindow/ContextFormat.h>
#include <glow/Error.h>
#include <glow/ref_ptr.h>
#include <glow/Buffer.h>
#include <glow/Array.h>
#include <glow/Program.h>
#include <glow/Shader.h>
#include <glow/VertexArrayObject.h>
#include <glow/VertexAttributeBinding.h>
#include <glowutils/File.h>
#include <glowwindow/Context.h>
#include <glowwindow/Window.h>
#include <glowwindow/WindowEventHandler.h>

#include <glow/logging.h>

using namespace glow;

struct Element
{
    Element(const glm::vec4& position)
        : point(position)
        , extent(0.2f, 0.1f)
        , fullSize(1.f, 1.f)
        , color(1.f, 0.f, 1.f, 1.f)
        , average(1.f)
        , min(0.01f)
        , max(2.f)
        , median(0.8f)
        , id(position.x > 0 ? 0 : 1)
        , count(position.x > 0 ? 10 : 20)
        , random(position.x > 0 ? 0.347264f : 0.673642f)
        , random2(position.x > 0 ? 0.67363f : 0.184623f)
    {
    }

    glm::vec4 point; // 0
    glm::vec2 extent; // 1
    glm::vec2 fullSize; // 2
    glm::vec4 color; // 3
    float average; // 4
    float min; // 5
    float max; // 6
    float median; // 7
    int id; // 8
    int count; // 9
    float random; // 10
    float random2; // 11
};

class EventHandler : public WindowEventHandler
{
public:
    EventHandler()
    {
    }

    virtual ~EventHandler()
    {
    }

    virtual void initialize(Window & window) override
    {
        DebugMessageOutput::enable();

        glClearColor(0.2f, 0.3f, 0.4f, 1.f);
        glPointSize(10.0);

        m_vao = new VertexArrayObject();
        m_buffer = new Buffer(GL_ARRAY_BUFFER);

        m_shaderProgram = new glow::Program();
        m_shaderProgram->attach(
            glow::createShaderFromFile(GL_VERTEX_SHADER, "data/vertexarrayattributes/test.vert"),
            glow::createShaderFromFile(GL_FRAGMENT_SHADER, "data/vertexarrayattributes/test.frag")
        );
        m_shaderProgram->bindFragDataLocation(0, "fragColor");

        m_buffer->setData(Array<Element>()
            << Element(glm::vec4(-0.3, -0.3, 0.0, 1.0))
            << Element(glm::vec4(0.3, 0.3, 0.0, 1.0)),
            GL_STATIC_DRAW
        );

        m_vao->binding(0)->setAttribute(0);
        m_vao->binding(0)->setBuffer(m_buffer, offsetof(Element, point), sizeof(Element));
        m_vao->binding(0)->setFormat(4, GL_FLOAT);
        m_vao->enable(0);

        m_vao->binding(1)->setAttribute(1);
        m_vao->binding(1)->setBuffer(m_buffer, offsetof(Element, extent), sizeof(Element));
        m_vao->binding(1)->setFormat(2, GL_FLOAT);
        m_vao->enable(1);

        m_vao->binding(2)->setAttribute(2);
        m_vao->binding(2)->setBuffer(m_buffer, offsetof(Element, fullSize), sizeof(Element));
        m_vao->binding(2)->setFormat(2, GL_FLOAT);
        m_vao->enable(2);

        m_vao->binding(3)->setAttribute(3);
        m_vao->binding(3)->setBuffer(m_buffer, offsetof(Element, color), sizeof(Element));
        m_vao->binding(3)->setFormat(4, GL_FLOAT);
        m_vao->enable(3);

        m_vao->binding(4)->setAttribute(4);
        m_vao->binding(4)->setBuffer(m_buffer, offsetof(Element, average), sizeof(Element));
        m_vao->binding(4)->setFormat(1, GL_FLOAT);
        m_vao->enable(4);

        m_vao->binding(5)->setAttribute(5);
        m_vao->binding(5)->setBuffer(m_buffer, offsetof(Element, min), sizeof(Element));
        m_vao->binding(5)->setFormat(1, GL_FLOAT);
        m_vao->enable(5);

        m_vao->binding(6)->setAttribute(6);
        m_vao->binding(6)->setBuffer(m_buffer, offsetof(Element, max), sizeof(Element));
        m_vao->binding(6)->setFormat(1, GL_FLOAT);
        m_vao->enable(6);

        m_vao->binding(7)->setAttribute(7);
        m_vao->binding(7)->setBuffer(m_buffer, offsetof(Element, median), sizeof(Element));
        m_vao->binding(7)->setFormat(1, GL_FLOAT);
        m_vao->enable(7);

        m_vao->binding(8)->setAttribute(8);
        m_vao->binding(8)->setBuffer(m_buffer, offsetof(Element, id), sizeof(Element));
        m_vao->binding(8)->setIFormat(1, GL_INT);
        m_vao->enable(8);

        m_vao->binding(9)->setAttribute(9);
        m_vao->binding(9)->setBuffer(m_buffer, offsetof(Element, count), sizeof(Element));
        m_vao->binding(9)->setIFormat(1, GL_INT);
        m_vao->enable(9);

        m_vao->binding(10)->setAttribute(10);
        m_vao->binding(10)->setBuffer(m_buffer, offsetof(Element, random), sizeof(Element));
        m_vao->binding(10)->setFormat(1, GL_FLOAT);
        m_vao->enable(10);

        m_vao->binding(11)->setAttribute(11);
        m_vao->binding(11)->setBuffer(m_buffer, offsetof(Element, random2), sizeof(Element));
        m_vao->binding(11)->setFormat(1, GL_FLOAT);
        m_vao->enable(11);
    }
    
    virtual void resizeEvent(ResizeEvent & event) override
    {
        glViewport(0, 0, event.width(), event.height());
    }

    virtual void paintEvent(PaintEvent &) override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shaderProgram->use();
        m_vao->drawArrays(GL_POINTS, 0, 2);
        m_shaderProgram->release();
    }

    virtual void idle(Window & window) override
    {
        window.repaint();
    }
protected:
    ref_ptr<VertexArrayObject> m_vao;
    ref_ptr<Buffer> m_buffer;
    ref_ptr<glow::Program> m_shaderProgram;
};

int main(int argc, char* argv[])
{
    ContextFormat format;

    Window window;

    {
        AutoTimer t("Initialization");

        window.setEventHandler(new EventHandler());

        window.create(format, "Vertex Array Attributes Example");
        window.context()->setSwapInterval(Context::VerticalSyncronization);

        window.show();
    }

    return MainLoop::run();
}
