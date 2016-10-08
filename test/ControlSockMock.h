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

#ifndef CONTROLSOCKMOCK_H
#define CONTROLSOCKMOCK_H

#include <iostream>
#include <queue>
#include <string>
#include <mutex>
#ifdef WIN32
    #include "Thread/mingw.mutex.h"
#else
    #include <thread>
#endif

class ControlSockMock
{
    public:
        static std::string GetLastSend(int delay=0);
        static void SetNextRecv(std::string value);

        static void SetSend(std::string value);
        static std::string GetRecv();
        static bool SomethingToRecv();

    private:
        static std::string m_MockRecv;
        static std::queue<std::string> m_MockSend;
        static std::mutex g_MockRecvMutex;
        static std::mutex g_MockSendMutex;

};

#endif // CONTROLSOCKMOCK_H
