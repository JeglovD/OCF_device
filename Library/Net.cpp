#include "Net.h"

#include <string>

void ThrowWSA(const std::string& file, const unsigned int& line, const std::string& function, const std::string& method)
{
	auto file_name_pos = file.rfind("\\");
	std::string result = file_name_pos == std::string::npos ? file : file.substr(file_name_pos + 1);
	result.append(":" + std::to_string(line) + " " + function + "() " + method + " error " + std::to_string(WSAGetLastError()));
	std::cerr << result << std::endl;
	throw result;
}

Socket::~Socket()
{
	mThreadWork = false;
	mThread.join();
}

SocketClientUDPMulticast::SocketClientUDPMulticast(const unsigned short& port, const char* multicast_group2)
{
	Net::Instance();
	
	if ((mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		THROW_WSA("socket()");

	// Позволяем использовать порт другому процессу
	const bool setsockopt_true = true;
	if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&setsockopt_true, sizeof(setsockopt_true)) != 0)
		THROW_WSA("setsockopt()");

	// Позволяем приложению получать возврат информации о пакете
	//if (setsockopt(mSocket, IPPROTO_IP, IP_PKTINFO, (char*)&setsockopt_true, sizeof(setsockopt_true)) != 0)
	//	THROW_WSA("setsockopt()");

	// Установим время ожидания в миллисекундах для блокировки входящих вызовов
	int so_rcvtimeo = 100;
	if (setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&so_rcvtimeo, sizeof(so_rcvtimeo)) != 0)
		THROW_WSA("setsockopt()");

	// Привязка сокета к адресу и порту 
	sockaddr_in socket_address;
	memset(&socket_address, 0, sizeof(socket_address));
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(port);
	socket_address.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(mSocket, (sockaddr*)&socket_address, sizeof(socket_address)) == SOCKET_ERROR)
		THROW_WSA("bind()");

	// Привязываем к multicast группе
	struct ip_mreq ip;
	memset(&ip, 0, sizeof(ip));
	ip.imr_multiaddr.S_un.S_addr = inet_addr(multicast_group2);
	ip.imr_interface.S_un.S_addr = INADDR_ANY;
	if (setsockopt(mSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&ip, sizeof(ip)) != 0)
		THROW_WSA("setsockopt()");

	// Request
	mThread = std::thread([](const bool& thread_work, const SOCKET& socket) 
		{
			const unsigned int request_max_size = 512;
			char request_buffer[request_max_size];
			int request_size;
			while (thread_work)
			{
				if ((request_size = recv(socket, &request_buffer[0], request_max_size, 0)) > 0)
				{
					std::cout << "recv() request_size=" << request_size << std::endl;
				}
				//else
				//	THROW_WSA("recv()");
				//std::cout << thread_work << std::endl;
				//std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			if (closesocket(socket) != 0)
				THROW_WSA("closesocket()");
		},std::ref(mThreadWork), std::ref(mSocket));

}

Net::Net()
{
	// Инициализация WSA (Windows Sockets API)
	WSADATA wsa_data;
	if (::WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
		throw "WSAStartup() error " + std::to_string(WSAGetLastError());
}

Net::~Net()
{
	if (WSACleanup() != 0)
		THROW_WSA("WSACleanup()");
}

