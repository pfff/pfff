/**
 * File taken from the UnitTest++ project
 */
#ifndef UNITTEST_TIMEHELPERS_H
#define UNITTEST_TIMEHELPERS_H

#ifdef _WIN32

#ifdef __MINGW32__
    #ifndef __int64
        #define __int64 long long
    #endif
#endif

class Timer
{
public:
    Timer();
    void Start();
    int GetTimeInMs() const;    

private:
    __int64 GetTime() const;

#if defined(_WIN64)
    unsigned __int64 m_processAffinityMask;
#else
    unsigned long m_processAffinityMask;
#endif

    __int64 m_startTime;
    __int64 m_frequency;

    void* m_threadHandle;
};

#else

class Timer
{
public:
    Timer();
    void Start();
    int GetTimeInMs() const;    

private:
    struct timeval m_startTime;    
};


#endif

#endif
