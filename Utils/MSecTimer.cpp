#include "MSecTimer.h"

MSecTimer::~MSecTimer(){
    m_bTimerThreadStop = true;
    m_bTimerThreadExit = true;
    m_TimerThread.join();
}

void MSecTimer::Start(){
    m_bTimerThreadStop = false;
}

void MSecTimer::Stop(){
    m_bTimerThreadStop = true;
}

void MSecTimer::Main(){
	clock_t begin_time = clock();
    while (!m_bTimerThreadExit){
		unsigned int unUsedTime = float(clock() - begin_time) / (CLOCKS_PER_SEC * 1000); // msec
		unsigned int unNextDuration = m_unDuration - unUsedTime > 0 ? m_unDuration - unUsedTime : 0;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(unNextDuration));
		begin_time = clock();
        if (!m_bTimerThreadStop){
            m_fn();
        }
    }
}
