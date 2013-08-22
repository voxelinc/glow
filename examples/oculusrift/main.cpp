
#include <GL/glew.h>

#include <algorithm>
#include <random>
#include <string>

#include <hidapi.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <glow/Uniform.h>
#include <glow/Array.h>
#include <glow/ShaderFile.h>
#include <glow/Program.h>
#include <glow/VertexArrayObject.h>
#include <glow/Window.h>
#include <glow/ContextFormat.h>
#include <glow/Context.h>
#include <glow/WindowEventHandler.h>
#include <glow/logging.h>


using namespace glow;

class EventHandler : public WindowEventHandler
{
public:
    EventHandler()
    :   m_frame(0)
    ,   m_trackerdk(nullptr)
    {
        hid_device_info * device = hid_enumerate(NULL, NULL);
    
        // find oculus rift dev kit 1 from oculus vr, inc.
        while (device && 0x2833 != device->vendor_id && 0x1 != device->product_id)
            device = device->next;

            //if (device->manufacturer_string)
            //    warning() << std::wstring(device->manufacturer_string) << " (manufacturer_string)";

            //if (device->product_string)
            //    warning() << std::wstring(device->product_string) << " (product_string)";

            //warning() << device->product_id << " (product_id)"; 
            //warning() << device->interface_number << " (interface_number)"; 
            //warning() << device->release_number << " (release_number)";
            //warning() << device->vendor_id << " (vendor_id)";
            //warning() << device->serial_number << " (serial_number)" << std::endl;

        const bool trackerdk_found = nullptr != device;
        hid_free_enumeration(device);

        if (!trackerdk_found)
        {
            fatal() << "Could not find Oculus VR's Tracker DK (Vendor 10291, Product 1).";
            return;
        }

        m_trackerdk = hid_open(0x2833, 0x1, NULL);
        if (!m_trackerdk)
        {
            fatal() << "Could not conntext to Oculus VR's Tracker DK.";
            return;
        }

        unsigned char fr [32];
        int fr_size = 0;
        for (unsigned char i = 2; i < 11; ++i)
        {
            fr [0] = i;
            fr_size = hid_get_feature_report(m_trackerdk, fr, 32);

            std::stringstream stream;
            for(int s = 0; s < fr_size; ++s)
                stream << static_cast<int>(fr[s]) << " ";

            warning() << stream.str();
        }
    }

    virtual ~EventHandler()
    {
    }

    void createAndSetupTexture();
    void createAndSetupShaders();
	void createAndSetupGeometry();

    virtual void initializeEvent(Window & window)
    {
        glClearColor(0.2f, 0.3f, 0.4f, 1.f);

	    createAndSetupTexture();
	    createAndSetupShaders();
	    createAndSetupGeometry();
    }
    
    virtual void resizeEvent(
        Window & window
    ,   const unsigned int width
    ,   const unsigned int height)
    {
    	int side = std::min<int>(width, height);
	    glViewport((width - side) / 2, (height - side) / 2, side, side);

        glm::mat4 view = glm::lookAt(glm::vec3(0.f, 0.f, -4.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 perspective = glm::perspective(55.f, static_cast<float>(width) / static_cast<float>(height), 0.1f, 8.f);

	    m_program->setUniform("view", view);
	    m_program->setUniform("projection", perspective);
    }

    virtual void paintEvent(Window & window)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	    m_program->use();

	    m_vao->bind();
	    m_vertexBuffer->drawArrays(GL_LINES, 0, 6);
	    m_vao->unbind();

        m_program->release();
    }

    virtual void idleEvent(Window & window)
    {
        //hid_read(m_trackerdk

        window.repaint();
    }

    virtual void keyReleaseEvent(
        Window & window
    ,   KeyEvent & event)
    {
        if (KeyEvent::KeyF5 == event.key())
            glow::ShaderFile::reloadAll();
    }

protected:
    hid_device * m_trackerdk;

	glow::ref_ptr<glow::Program> m_program;	
    glow::ref_ptr<glow::VertexArrayObject> m_vao;
	
    glow::ref_ptr<glow::Buffer> m_vertexBuffer;

    unsigned int m_frame;
};


/** This example shows ... .
*/
int main(int argc, char** argv)
{
    glewExperimental = GL_TRUE;

    ContextFormat format;
    EventHandler handler;

    Window window;
    window.attach(&handler);

    window.create(format, "Simple Texture Example", 1280, 800);
    window.show();
    window.context()->setSwapInterval(Context::NoVerticalSyncronization);

    return Window::run();
}

void EventHandler::createAndSetupTexture()
{
	//m_texture = new glow::Texture(GL_TEXTURE_2D);

    //m_texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//m_texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//m_texture->image2D(0, GL_R32F, 512, 512, 0, GL_RED, GL_FLOAT, nullptr);
	//m_texture->bindImageTexture(0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
}

void EventHandler::createAndSetupShaders()
{
	glow::Shader* vertexShader = glow::Shader::fromFile(GL_VERTEX_SHADER, "data/oculusrift/glslrift.vert");
	glow::Shader* fragmentShader = glow::Shader::fromFile(GL_FRAGMENT_SHADER, "data/oculusrift/glslrift.frag");

	m_program = new glow::Program();
	m_program->attach(vertexShader, fragmentShader);
	m_program->bindFragDataLocation(0, "fragColor");
}

void EventHandler::createAndSetupGeometry()
{
    auto vertexArray = glow::Vec3Array()
        << glm::vec3( 0.0, 0.0, 0.0) 
        << glm::vec3( 0.5, 0.0, 0.0) 
        << glm::vec3( 0.0, 0.0, 0.0) 
        << glm::vec3( 0.0, 0.5, 0.0)
        << glm::vec3( 0.0, 0.0, 0.0) 
        << glm::vec3( 0.0, 0.0, 1.0);

	m_vao = new glow::VertexArrayObject();

	m_vertexBuffer = new glow::Buffer(GL_ARRAY_BUFFER);
	m_vertexBuffer->setData(vertexArray);

	auto binding = m_vao->binding(0);
	binding->setBuffer(m_vertexBuffer, 0, sizeof(glm::vec3));
	binding->setFormat(3, GL_FLOAT);

	m_vao->enable(m_program->getAttributeLocation("a_vertex"));
}
