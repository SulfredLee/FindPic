#ifndef MSECTIMER_H
#define MSECTIMER_H

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/atomic.hpp>

#include <time.h>

class MSecTimer{
public:
    template <class Fn, class... Args>
    MSecTimer(const unsigned int& unDuration, Fn&& fn, Args&&... args);
    ~MSecTimer();

    void Start();
    void Stop();
private:
    void Main();
private:
    boost::thread m_TimerThread;
    boost::atomic<bool> m_bTimerThreadExit;
    boost::atomic<bool> m_bTimerThreadStop;
    boost::function<void()> m_fn;
    unsigned int m_unDuration;
};

template <class Fn, class... Args>
    MSecTimer::MSecTimer(const unsigned int& unDuration, Fn&& fn, Args&&... args)
    : m_unDuration(unDuration)
    , m_fn(boost::bind(boost::forward<Fn>(fn), boost::forward<Args>(args)...)){
        m_bTimerThreadStop = false;
        m_bTimerThreadExit = false;
        m_TimerThread = boost::thread(&MSecTimer::Main, this);
}
#endif
