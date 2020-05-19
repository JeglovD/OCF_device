#pragma once
#pragma comment(lib, "Ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define THROW_WSA(method) ThrowWSA(__FILE__, __LINE__, __FUNCTION__, method)

#include <iostream>
#include <thread>
#include <vector>
#include <WinSock2.h>
#include <WS2tcpip.h>

void ThrowWSA(const std::string& file, const unsigned int& line_nember, const std::string& function, const std::string& method);

class Socket 
{
public:
	Socket() {};
	virtual ~Socket();

protected:
	SOCKET mSocket;
	std::thread mThread;
	bool mThreadWork = true;
};

class SocketClientUDPMulticast : public Socket 
{
public:
	SocketClientUDPMulticast(const unsigned short& port, const char* multicast_group);
};


class Net
{
public:
	~Net();
	static Net& Instance() { static Net net; return net; }

private:
	Net();
	Net(const Net&);
	Net& operator=(Net&);

	std::vector< SOCKET > mSockets;
};
