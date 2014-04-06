#pragma once

#include <chrono>

#include <glowutils/glowutils.h>

namespace glowutils
{

class GLOWUTILS_API Timer
{
public:
    Timer(bool start = true, bool autoUpdate = true);
    virtual ~Timer();

    void setAutoUpdating(const bool autoUpdate);
    bool autoUpdating() const;

    bool paused() const;

    void update() const;
    long double elapsed() const;

    void start();
    void pause();
    void stop ();
    void reset();
protected:
    bool m_paused;
    bool m_auto;

protected:
    using clock = std::chrono::high_resolution_clock ;
    using time_point = clock::time_point;

    using nano = std::chrono::duration<long double, std::nano>;

    time_point m_t0;
    time_point m_tp; // time_point of last pausing

    mutable time_point m_t1;

    long double m_offset;
    mutable long double m_elapsed;
};

} // namespace glowutils
