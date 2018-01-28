#include "DailyTimer.h"

DailyTimer::~DailyTimer(){
    m_bTimerThreadStop = true;
    m_bTimerThreadExit = true;
    m_TimerThread.join();
}

void DailyTimer::Start(){
    m_bTimerThreadStop = false;
}

void DailyTimer::Stop(){
    m_bTimerThreadStop = true;
}

bool DailyTimer::IsTimeUP(){
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    return m_DateTime == now;
}

void DailyTimer::Main(){
    while (!m_bTimerThreadExit){
        boost::this_thread::sleep_for(boost::chrono::milliseconds(300));
        if (!m_bTimerThreadStop && IsTimeUP()){
            boost::gregorian::date_duration dd(1); // one day duration
            boost::posix_time::ptime newDateTime(m_DateTime.date() + dd
                                                 , m_DateTime.time_of_day());
            m_DateTime = newDateTime;
            m_fn();
        }
    }
}
