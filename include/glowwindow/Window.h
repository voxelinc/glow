#pragma once

#include <set>
#include <string>
#include <queue>

#include <glow/ref_ptr.h>

#include <glowwindow/glowwindow.h>
#include <glowwindow/MainLoop.h>
#include <glowwindow/events.h>  // forward?


struct GLFWwindow;

namespace glowutils
{
    class Timer;
}

namespace glowwindow 
{
class WindowEventHandler;
class ContextFormat;
class Context;

/**
 * Attach a WindowEventHandler specialization for event handling.
 */
class GLOWWINDOW_API Window
{
public:
    Window();
    virtual ~Window();

    bool create(const ContextFormat & format, int width = 1280, int height = 720);
    bool create(const ContextFormat & format, const std::string & title = "glow", int width = 1280, int height = 720);

    /**
     * Takes ownership of the given eventhandler and deletes that either on
     * quitting, just before the opengl context gets destroyed, or when
     * reassigning a new, different handler.
     */
    void setEventHandler(WindowEventHandler * eventHandler);
    WindowEventHandler * eventHandler() const;

    void show();
    void hide();

    int width() const;
    int height() const;
    glm::ivec2 size() const;
    glm::ivec2 position() const;
    glm::ivec2 framebufferSize() const;
    int inputMode(int mode) const;
    const std::string & title() const;

    void setTitle(const std::string & title);
    void resize(int width, int height);
    void setInputMode(int mode, int value);

    /**
     * If enabled, this causes an application wide quit message to be posted
     * when the window gets destroyed. Hence, the MainLoop will be quit
     * and all other remaining windows destroyed.
     */
    void quitOnDestroy(bool enable);
    bool quitsOnDestroy() const;

    Context * context() const;

    void close();

    /**
     * Queues a paint event.
     */
    void repaint();
    void idle();

    void fullScreen();
    bool isFullScreen() const;
    void windowed();
    bool isWindowed() const;
    void toggleMode();

    GLFWwindow * internalWindow() const;

    void queueEvent(WindowEvent * event);
    bool hasPendingEvents();
    void processEvents();

    static const std::set<Window*>& instances();

    void addTimer(int id, int interval, bool singleShot = false);
    void removeTimer(int id);

    void swap();
    void destroy();
protected:
    bool createContext(const ContextFormat & format, int width, int height, GLFWmonitor* monitor = nullptr);
    void destroyContext();

    void initializeEventHandler();
    void finalizeEventHandler();

    void clearEventQueue();
    void processEvent(WindowEvent & event);
    void postprocessEvent(WindowEvent & event);
protected:
    Context * m_context;
    GLFWwindow * m_window;
    glow::ref_ptr<WindowEventHandler> m_eventHandler;
    std::queue<WindowEvent*> m_eventQueue;
    glm::ivec2 m_windowedModeSize;
    std::string m_title;

    bool m_quitOnDestroy;

    enum Mode
    {
        WindowMode
    ,   FullScreenMode
    };

    Mode m_mode;
private:
    static std::set<Window*> s_instances;
};

} // namespace glowwindow
