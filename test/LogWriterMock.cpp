#include "LogWriterMock.h"
#include "Plateforms/Plateforms.h"

using namespace std;
LogWriterMock::LogWriterMock() : m_MockMutex(), m_level(), m_message(), m_module(), m_line(), m_function()
{
}

LogWriterMock::~LogWriterMock()
{
}

bool LogWriterMock::WaitLog(int delay)
{
    bool tmp = false;
    int idelay;

    for(idelay=0; idelay<=delay; idelay++)
    {
        m_MockMutex.lock();
        if(m_level != "") tmp=true;
        m_MockMutex.unlock();
        if(tmp==true) break;
        Plateforms::delay(100);
    }

    return tmp;
}

string LogWriterMock::GetLastLevel()
{
    string tmp;

    m_MockMutex.lock();
    tmp = m_level;
    m_level = "";
    m_MockMutex.unlock();

    return tmp;
}

string LogWriterMock::GetLastMessage()
{
    string tmp;

    m_MockMutex.lock();
    tmp = m_message;
    m_MockMutex.unlock();

    return tmp;
}

string LogWriterMock::GetLastModule()
{
    string tmp;

    m_MockMutex.lock();
    tmp = m_module;
    m_MockMutex.unlock();

    return tmp;
}

string LogWriterMock::GetLastFunction()
{
    string tmp;

    m_MockMutex.lock();
    tmp = m_function;
    m_MockMutex.unlock();

    return tmp;
}

void LogWriterMock::Writer(SimpleLog::Level level, const std::string& message, const std::string& module, int line, const std::string& function)
{
    m_MockMutex.lock();
    m_level = SimpleLog::Level2String(level);
    m_message = message;
    m_module = module;
    m_line = line;
    m_function = function;
    m_MockMutex.unlock();

    return;
}

void LogWriterMock::Flush()
{
	return;
}
