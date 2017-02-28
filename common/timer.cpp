#include "common.h"
#include "timer.h"

Timer::Timer()
{
    m_startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

Timer& Timer::Instance()
{
    static Timer timer;
    return timer;
}

uint64_t Timer::GetTimeMs() const
{
    uint64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    return time - m_startTime;
}

