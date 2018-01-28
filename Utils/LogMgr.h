/*
  Copyright (c) 2011 Christoph Stoepel
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
#ifndef LOGMGR_H
#define LOGMGR_H

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/attributes/attribute.hpp>
#include <boost/log/attributes/attribute_cast.hpp>
#include <boost/log/attributes/attribute_value.hpp>
#include <boost/log/attributes/mutable_constant.hpp>

#include <sstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <chrono>
#include <ctime>

#ifdef WINDOWS
// #include <Windows.h>
#else
#include <stdio.h>
#endif
namespace Logging
{
    enum LOG_LEVEL
    {
        LOG_LEVEL_TRACE = 0,
        LOG_LEVEL_DEBUG = 2,
        LOG_LEVEL_INFO = 3,
        LOG_LEVEL_WARN = 4,
        LOG_LEVEL_ERROR = 5,
        LOG_LEVEL_FATAL = 6
    };

    class CLogMessage
    {
    public:
        std::string m_szMessage;

    public:
		CLogMessage(std::string szMsg) : m_szMessage(szMsg) { }
		CLogMessage(const CLogMessage& msg) : m_szMessage(msg.m_szMessage) { }
        CLogMessage(const char* szFormat, ...)
            {
                va_list args;
                char buffer[2048];
                va_start(args, szFormat);
#ifdef WINDOWS
                vsprintf_s(buffer, szFormat, args);
#else
                vsnprintf(buffer, sizeof(buffer), szFormat, args);
#endif
                //m_szMessage.FormatV(szFormat, args);
                va_end(args);
                m_szMessage = buffer;
            }

        //operator LPCTSTR() const { return (LPCTSTR)m_szMessage; }
        CLogMessage& operator=(const CLogMessage& msg) { m_szMessage = msg.m_szMessage; return *this; }
        bool operator==(const CLogMessage& msg) const { return std::strcmp(m_szMessage.c_str(), msg.m_szMessage.c_str()) == 0; }
    };

    class ILogTarget
    {
    public:
        virtual ~ILogTarget() { }
        virtual bool IsEnabled(LOG_LEVEL lvl) = 0;
        virtual void Append(std::string szMsg, LOG_LEVEL lvl) = 0;
    };

    class CLogTargetBase : public ILogTarget
    {
    protected:
        LOG_LEVEL m_nLevel;

    public:
		CLogTargetBase(LOG_LEVEL lvl) : m_nLevel(lvl) { }
        virtual ~CLogTargetBase() {}
        virtual bool IsEnabled(LOG_LEVEL lvl) { return m_nLevel <= lvl; }
    };

    class CLogTargetDebugger : public CLogTargetBase
    {
    public:
		CLogTargetDebugger(LOG_LEVEL lvl) : CLogTargetBase(lvl) { }
        virtual ~CLogTargetDebugger() { }
        //virtual void Append(std::string szMsg) { ::OutputDebugString(szMsg); }
        virtual void Append(std::string szMsg, LOG_LEVEL lvl)
        {
            //using namespace boost::log::trivial;

            switch (lvl)
                {
                case Logging::LOG_LEVEL_TRACE:
                    BOOST_LOG_TRIVIAL(trace) << szMsg;
                    break;
                case Logging::LOG_LEVEL_DEBUG:
                    BOOST_LOG_TRIVIAL(debug) << szMsg;
                    break;
                case Logging::LOG_LEVEL_INFO:
                    BOOST_LOG_TRIVIAL(info) << szMsg;
                    break;
                case Logging::LOG_LEVEL_WARN:
                    BOOST_LOG_TRIVIAL(warning) << szMsg;
                    break;
                case Logging::LOG_LEVEL_ERROR:
                    BOOST_LOG_TRIVIAL(error) << szMsg;
                    break;
                case Logging::LOG_LEVEL_FATAL:
                    BOOST_LOG_TRIVIAL(fatal) << szMsg;
                    break;
                default:
                    break;
                }
        }
    };

    //class CLogTargetMessageBox : public CLogTargetBase
    //{
    //public:
    //	CLogTargetMessageBox(LOG_LEVEL lvl) : CLogTargetBase(lvl) { }
    //	virtual ~CLogTargetMessageBox() { }
    //	virtual void Append(std::string szMsg) { ::AtlMessageBox(NULL, szMsg); }
    //};

    class CLogger{
    private:
        int m_dwLogStart;
        std::vector<ILogTarget*> m_pTargets;

    public:
        CLogger(){
            m_dwLogStart = getTickCount();
            boostInit();
        }
        ~CLogger(){
            for (size_t i = 0; i<m_pTargets.size(); i++)
                delete m_pTargets[i];
            m_pTargets.clear();
        }

        void AddTarget(ILogTarget* pTarget){
            m_pTargets.push_back(pTarget);
        }
        void Log(LOG_LEVEL lvl, std::string szMsg, std::string szFile, std::string szFunction, int nLine){
            bool shouldLog = false;
            for (size_t i = 0; i<m_pTargets.size(); i++){
                if (m_pTargets[i]->IsEnabled(lvl)){
                    shouldLog = true;
                    break; // for
                }
            }

            if (shouldLog){
                boost::filesystem::path p(szFile);
                std::ostringstream msg;
                //msg << std::setfill('0') << std::setw(6) << getTickCount() - m_dwLogStart << " ";
                msg << "[" << szFunction << "] ";
                msg << p.filename() << ":" << nLine << " - " << szMsg;
                //std::string msg;
                //msg.Format(_T("%06d [%s] %s:%d - %s"), getTickCount() - m_dwLogStart, szFunction, ::PathFindFileName(szFile), nLine, szMsg);

                for (size_t i = 0; i<m_pTargets.size(); i++)
                    if (m_pTargets[i]->IsEnabled(lvl))
                        m_pTargets[i]->Append(msg.str(), lvl);
            }
        }
    private:
        unsigned getTickCount(){
#ifdef WINDOWS
            //return GetTickCount();
			using namespace std::chrono;
			return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
#else
            //auto t = std::chrono::system_clock::now();
            using namespace std::chrono;
            return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
            //struct timeval tv;
            //gettimeofday(&tv, 0);
            //return unsigned((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
#endif
        }
        void boostInit()
        {
            const std::string COMMON_FMT("[%TimeStamp%][%Severity%]:  %Message%");

            boost::log::register_simple_formatter_factory< boost::log::trivial::severity_level, char >("Severity");

            //// Output message to console
            //boost::log::add_console_log(
            //	std::cout,
            //	boost::log::keywords::format = COMMON_FMT,
            //	boost::log::keywords::auto_flush = true
            //	);

            // Output message to file, rotates when file reached 1mb or at midnight every day. Each log file
            // is capped at 1mb and total is 20mb
            boost::log::add_file_log(
                                     //boost::log::keywords::file_name = "./Log/Log_%Y%m%d_%H%M%S_%5N.log",
                                     boost::log::keywords::file_name = "./Log/Log_%Y%m%d_%5N.log",
                                     boost::log::keywords::rotation_size = 10 * 1024 * 1024, // 10 MB for every file
                                     boost::log::keywords::max_size = 20 * 1024 * 1024,
                                     boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
                                     boost::log::keywords::format = COMMON_FMT,
                                     boost::log::keywords::open_mode = std::ios_base::app,
                                     boost::log::keywords::auto_flush = true
                                     );

            boost::log::add_common_attributes();

            // Only output message with INFO or higher severity in Release
#ifndef _DEBUG
            boost::log::core::get()->set_filter(
                                                boost::log::trivial::severity >= boost::log::trivial::info
                                                );
#endif
        }
    };

    class CLoggerFactory
    {
    private:
        //static CLogger m_SingletonInstance;	// TODO: write a real singleton/ factory pattern implementation

    public:
        static CLogger* getDefaultInstance()
        {
            static CLogger m_SingletonInstance;
            return &m_SingletonInstance;
        }
    };
}

#define WIDEN2(x) L##x
#define WIDEN(x) WIDEN2(x)

#define __LOGMSG(lvl, msg, file, func, line) Logging::CLoggerFactory::getDefaultInstance()->Log(lvl, msg, file, func, line)

#ifdef UNICODE
#define LOGMSG(lvl, msg)  __LOGMSG(lvl, msg, WIDEN(__FILE__), WIDEN(__FUNCSIG__), __LINE__)
#define LOGMSG_DEBUG(msg) __LOGMSG(Logging::LOG_LEVEL_DEBUG, msg, WIDEN(__FILE__), WIDEN(__FUNCTION__), __LINE__)
#define LOGMSG_INFO(msg)  __LOGMSG(Logging::LOG_LEVEL_INFO, msg, WIDEN(__FILE__), WIDEN(__FUNCTION__), __LINE__)
#define LOGMSG_WARN(msg)  __LOGMSG(Logging::LOG_LEVEL_WARN, msg, WIDEN(__FILE__), WIDEN(__FUNCTION__), __LINE__)
#define LOGMSG_ERROR(msg) __LOGMSG(Logging::LOG_LEVEL_ERROR, msg, WIDEN(__FILE__), WIDEN(__FUNCTION__), __LINE__)
#define LOGMSG_FATAL(msg) __LOGMSG(Logging::LOG_LEVEL_FATAL, msg, WIDEN(__FILE__), WIDEN(__FUNCTION__), __LINE__)
#else
#define LOGMSG(lvl, msg)  __LOGMSG(lvl, msg, __FILE__, __FUNCSIG__, __LINE__)
#define LOGMSG_DEBUG(msg) __LOGMSG(Logging::LOG_LEVEL_DEBUG, msg, __FILE__, __FUNCTION__, __LINE__)
#define LOGMSG_INFO(msg)  __LOGMSG(Logging::LOG_LEVEL_INFO, msg, __FILE__, __FUNCTION__, __LINE__)
#define LOGMSG_WARN(msg)  __LOGMSG(Logging::LOG_LEVEL_WARN, msg, __FILE__, __FUNCTION__, __LINE__)
#define LOGMSG_ERROR(msg) __LOGMSG(Logging::LOG_LEVEL_ERROR, msg, __FILE__, __FUNCTION__, __LINE__)
#define LOGMSG_FATAL(msg) __LOGMSG(Logging::LOG_LEVEL_FATAL, msg, __FILE__, __FUNCTION__, __LINE__)
#endif

#endif
