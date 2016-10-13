#ifndef LOGWRITERMOCK_H
#define LOGWRITERMOCK_H

#include <mutex>
#ifdef WIN32
    #include "Thread/mingw.mutex.h"
#else
    #include <thread>
#endif
#include "SimpleLog/SimpleLog.h"

class LogWriterMock : public SimpleLog::IWriter
{
	public:
		LogWriterMock();
		virtual ~LogWriterMock();
		virtual void Writer(SimpleLog::Level level, const std::string& message, const std::string& module, int Line, const std::string& function);
		virtual void Flush();

        bool WaitLog(int delay);
		std::string GetLastLevel();
        std::string GetLastMessage();
        std::string GetLastModule();
        std::string GetLastFunction();

	private:
        std::mutex m_MockMutex;
		std::string m_level;
		std::string m_message;
		std::string m_module;
		int m_line;
		std::string m_function;
};
#endif
