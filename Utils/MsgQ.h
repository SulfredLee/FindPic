#ifndef MSGQ_H
#define MSGQ_H

#include <vector>


#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/atomic.hpp>
#include <boost/shared_ptr.hpp>

template<typename T>
class MsgQ
{
 private:
    mutable boost::mutex m_MuHead;
    mutable boost::mutex m_MuTail;
    boost::condition_variable m_ConditionVar;
    std::vector<boost::shared_ptr<T> > m_RingQueue;
    boost::atomic<int> m_nIN;
    boost::atomic<int> m_nOUT;
 public:
    MsgQ(){
        m_RingQueue.resize(128);
        m_nIN = 0;
        m_nOUT = 0;
    }
    ~MsgQ(){
    }

    bool push(boost::shared_ptr<T> inData){
        boost::mutex::scoped_lock lock(m_MuHead);
        int nNextIN = ((m_nIN + 1) & (128 - 1)); // mod operation
        if (nNextIN == m_nOUT){ // if the next index of IN is index of OUT
            lock.unlock();
            return false;
        }
        m_RingQueue[m_nIN] = boost::move(inData);
        m_nIN = nNextIN; // InterlockedCompareExchange((long*)&m_nIN, nNextIN, m_nIN);
        lock.unlock();
        m_ConditionVar.notify_one();
        return true;
    }

    bool get(boost::shared_ptr<T>& outData){
        boost::mutex::scoped_lock lock(m_MuTail);
        while (m_nOUT == m_nIN){ // if no more data in queue
            m_ConditionVar.wait(lock);
            //return false;
        }
        outData = boost::move(m_RingQueue[m_nOUT]);
        int nNextOUT = ((m_nOUT + 1) & (128 - 1)); // mod operation
        m_nOUT = nNextOUT; // InterlockedCompareExchange((long*)&m_nOUT, nNextOUT, m_nOUT);
        lock.unlock();
        return true;
    }
};

#endif
