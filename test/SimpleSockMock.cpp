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
#include "SimpleSock/SimpleSock.h"
#include "ControlSockMock.h"
#include "Plateforms/Plateforms.h"
using namespace std;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/***************************************************************************************************/
/***                                                                                             ***/
/*** Class SimpleSockMock                                                                        ***/
/***                                                                                             ***/
/***************************************************************************************************/

bool SimpleSock::m_initSocket = false;

void SimpleSock::InitSocket()
{
    SimpleSock::m_initSocket = true;
}

SimpleSock::SimpleSock(int type) : m_sockAddress(), m_sockHandle(INVALID_SOCKET), m_isOpen(false), m_sockType(type), m_blocking(false)
{
    m_sockAddress.sin_family = AF_INET;
    m_sockAddress.sin_port=0;
    m_sockAddress.sin_addr.s_addr=0;

    if(!SimpleSock::m_initSocket) InitSocket();
}

SimpleSock::~SimpleSock()
{
    Close();
}

void SimpleSock::SetSocket(SOCKET sockHandle, struct sockaddr_in sockAddress, int type)
{
    m_sockType = type;
    m_sockHandle = sockHandle;
    m_sockAddress = sockAddress;
    m_isOpen = true;
}

int SimpleSock::GetPort()
{
    return ntohs(m_sockAddress.sin_port);
}

string SimpleSock::GetAddress()
{
    return inet_ntoa(m_sockAddress.sin_addr);
}

void SimpleSock::Open(int port, unsigned long ipAddress)
{
    memset(&m_sockAddress, 0, sizeof(m_sockAddress));
	m_sockAddress.sin_family = AF_INET;
	m_sockAddress.sin_port=htons(port);
    m_sockAddress.sin_addr.s_addr=ipAddress;
    m_isOpen = true;
}

bool SimpleSock::isOpen()
{
    return m_isOpen;
}

void SimpleSock::Close()
{
 	m_sockHandle = INVALID_SOCKET;
    m_isOpen = false;
}

void SimpleSock::Listen(int port, unsigned long ipAddress)
{
    Open(port, ipAddress);
}

bool SimpleSock::WaitRecv(int delay)
{
    if(ControlSockMock::SomethingToRecv()) return true;

    Plateforms::delay(delay);
    return false;
}

void SimpleSock::Send(const char* buffer, size_t bufferSize)
{
    ControlSockMock::SetSend(buffer);
}

void SimpleSock::Send(const string& buffer)
{
    ControlSockMock::SetSend(buffer);
}

unsigned SimpleSock::Recv(char* buffer, size_t bufferSize)
{
	int status;
	string tmp;


    if(bufferSize==0) throw SimpleSock::Exception(0x0054, "SimpleSock::Recv: Invalid buffer size");
    if(buffer==nullptr) throw SimpleSock::Exception(0x0053, "SimpleSock::Recv: Invalid buffer pointer");
    if(!m_isOpen) throw SimpleSock::Exception(0x0052, "SimpleSock::Recv: Listen() or Connect() method is mandatory before Recv()");

    tmp = ControlSockMock::GetRecv();
    strncpy(buffer, tmp.c_str(), bufferSize);
    status = tmp.size();

	return status;
}

bool SimpleSock::Recv(string& buffer, int sizMax)
{
    buffer = ControlSockMock::GetRecv();
    if(buffer=="") return false;
	return true;
}

void SimpleSock::Blocking(bool blocking)
{
    m_blocking = blocking;
}

int SimpleSock::GetSocketError()
{
    return -1;
}

bool SimpleSock::OperationOk()
{
    return true;
}

/***************************************************************************************************/
/***                                                                                             ***/
/*** Class SimpleSockUDP                                                                         ***/
/***                                                                                             ***/
/***************************************************************************************************/
SimpleSockUDP::SimpleSockUDP() : SimpleSock(SOCK_DGRAM), m_networkInterface("")
{
}

void SimpleSockUDP::SetNetworkInterface(const string& networkInterface)
{
    m_networkInterface = networkInterface;
}

void SimpleSockUDP::Open(int port)
{
    SimpleSock::Open(port, BroadcastAddress(m_networkInterface));
}

void SimpleSockUDP::Open(int port, const string& ipAddress)
{
    SimpleSock::Open(port, inet_addr(ipAddress.c_str()));
}

void SimpleSockUDP::Open(int port, unsigned long ipAddress)
{
    SimpleSock::Open(port, ipAddress);
}

void SimpleSockUDP::Listen(int port)
{
    SimpleSock::Listen(port, BroadcastAddress(m_networkInterface));
}

void SimpleSockUDP::Listen(int port, int address)
{
    SimpleSock::Listen(port, address);
}

void SimpleSockUDP::Listen(int port, const string& ipAddress)
{
    SimpleSock::Listen(port, inet_addr(ipAddress.c_str()));
}

string SimpleSockUDP::LocalAddress(const string& interfaceName="")
{
    return "127.0.0.1";
}

unsigned long SimpleSockUDP::BroadcastAddress(const string& interfaceName="")
{
    return inet_addr("127.255.255.255");
}

/***************************************************************************************************/
/***                                                                                             ***/
/*** Class SimpleSockTCP                                                                         ***/
/***                                                                                             ***/
/***************************************************************************************************/
SimpleSockTCP::SimpleSockTCP() : SimpleSock(SOCK_STREAM)
{
}

void SimpleSockTCP::Connect(const string& ipAddress, int port)
{
    SimpleSock::Open(port, inet_addr(ipAddress.c_str()));
}

void SimpleSockTCP::Listen(int port)
{
    SimpleSock::Listen(port, INADDR_ANY);
}

void SimpleSockTCP::Listen(int port, const string& ipAddress)
{
    SimpleSock::Listen(port, inet_addr(ipAddress.c_str()));
}

bool SimpleSockTCP::Accept(SimpleSockTCP* sockAccept)
{
    return true;
}

/***************************************************************************************************/
/***                                                                                             ***/
/*** Class SimpleSock::Exception                                                                 ***/
/***                                                                                             ***/
/***************************************************************************************************/
SimpleSock::Exception::Exception(int number, string const& message) throw() : m_number(number), m_message(message), m_system(0), m_whatMsg()
{
    SetWhatMsg();
}

SimpleSock::Exception::Exception(int number, string const& message, int system) throw() : m_number(number), m_message(message), m_system(system), m_whatMsg()
{
    SetWhatMsg();
}

SimpleSock::Exception::~Exception() throw()
{
}

void SimpleSock::Exception::SetWhatMsg()
{
    ostringstream message;

    message << m_message;
    if(m_system!=0) message << " (system error " << m_system << ")";
    m_whatMsg = message.str();
}

const char* SimpleSock::Exception::what() const throw()
{
    return m_whatMsg.c_str();
}

int SimpleSock::Exception::GetNumber() const throw()
{
    return m_number;
}

#pragma GCC diagnostic pop
