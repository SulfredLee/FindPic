#ifndef DAILYTIMER_H
#define DAILYTIMER_H

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/atomic.hpp>
#include <boost/date_time.hpp>

#include <time.h>
#include <string>
#include <sstream>

class DailyTimer{
public:
    template <class Fn, class... Args>
        DailyTimer(const boost::posix_time::ptime& DateTimeIN, Fn&& fn, Args&&... args);
    ~DailyTimer();

    void Start();
    void Stop();
private:
    bool IsTimeUP();
    void Main();
private:
    boost::thread m_TimerThread;
    boost::atomic<bool> m_bTimerThreadExit;
    boost::atomic<bool> m_bTimerThreadStop;
    boost::function<void()> m_fn;
    boost::posix_time::ptime m_DateTime;
};

template <class Fn, class... Args>
    DailyTimer::DailyTimer(const boost::posix_time::ptime& DateTimeIN, Fn&& fn, Args&&... args)
    : m_DateTime(DateTimeIN)
    , m_fn(boost::bind(boost::forward<Fn>(fn), boost::forward<Args>(args)...)){
        m_bTimerThreadStop = false;
        m_bTimerThreadExit = false;
        m_TimerThread = boost::thread(&DailyTimer::Main, this);
}
#endif
