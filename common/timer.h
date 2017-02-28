#pragma once

class Timer
{
public:
    static Timer& Instance();

    uint64_t GetTimeMs() const;

private:
    Timer();
    uint64_t m_startTime;
};