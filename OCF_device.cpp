#include "Library/Net.h"

#include <chrono>
#include <iostream>
#include <thread>

static const unsigned short OCF_PORT = 5683;
static const char OCF_MULTICAST_GROUP[] = "224.0.1.187";

int main()
{
	SocketClientUDPMulticast s{ OCF_PORT, OCF_MULTICAST_GROUP };

	std::this_thread::sleep_for(std::chrono::seconds(10));
}
