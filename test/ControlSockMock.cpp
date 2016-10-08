/*** LICENCE ***************************************************************************************/
/*
  SimpleSock - Simple class to manage socket communication TCP or UDP

  This file is part of SimpleSock.

    SimpleSock is free software : you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SimpleSock is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SimpleSock.  If not, see <http://www.gnu.org/licenses/>.
*/
/***************************************************************************************************/
#include<sstream>
#include "ControlSockMock.h"
#include "Plateforms/Plateforms.h"

using namespace std;

/***************************************************************************************************/
/***                                                                                             ***/
/*** Class ControlSockMock                                                                       ***/
/***                                                                                             ***/
/***************************************************************************************************/

string ControlSockMock::m_MockRecv = "";
queue<string> ControlSockMock::m_MockSend;
mutex ControlSockMock::g_MockRecvMutex;
mutex ControlSockMock::g_MockSendMutex;

string ControlSockMock::GetLastSend(int delay)
{
    string tmp;
    int idelay;

    for(idelay=0; idelay<=delay; idelay++)
    {
        ControlSockMock::g_MockSendMutex.lock();
        if(!ControlSockMock::m_MockSend.empty())
        {
            tmp = ControlSockMock::m_MockSend.front();
            ControlSockMock::m_MockSend.pop();
        }
        ControlSockMock::g_MockSendMutex.unlock();
        if(tmp!="") break;
        Plateforms::delay(100);
    }

    return tmp;
}

void ControlSockMock::SetNextRecv(std::string value)
{

    ControlSockMock::g_MockRecvMutex.lock();
    if(ControlSockMock::m_MockRecv!="")
    {
        std::stringstream ss;
        ss << ControlSockMock::m_MockRecv << endl << value;
        ControlSockMock::m_MockRecv = ss.str();
    }
    else
        ControlSockMock::m_MockRecv += value;
    ControlSockMock::g_MockRecvMutex.unlock();
}

void ControlSockMock::SetSend(std::string value)
{
    ControlSockMock::g_MockSendMutex.lock();
    ControlSockMock::m_MockSend.push(value);
    ControlSockMock::g_MockSendMutex.unlock();
}

bool ControlSockMock::SomethingToRecv()
{
    bool tmp = false;

    ControlSockMock::g_MockRecvMutex.lock();
    if(ControlSockMock::m_MockRecv!="") tmp = true;
    ControlSockMock::g_MockRecvMutex.unlock();
    return tmp;
}

string ControlSockMock::GetRecv()
{
    string tmp;

    ControlSockMock::g_MockRecvMutex.lock();
    tmp = ControlSockMock::m_MockRecv;
    ControlSockMock::m_MockRecv = "";
    ControlSockMock::g_MockRecvMutex.unlock();
    return tmp;
}
