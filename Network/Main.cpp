#include <iostream>
#include <string>
#include <map>
#include <array>
#include "../Core/Common.h"
#include "TcpServer.hpp"
#include "TcpConnection.hpp"

using namespace net;
using namespace std;

void onMessage(const std::shared_ptr<TcpConnection>& conn, std::array<uint8, MAX_BUFFER_SIZE> data, size_t size)
{

	std::cout << "\n"  << " [conn] " << conn <<  " [size] " << size << std::endl;
	//conn->asyncSend(data, size);
}

void test()
{
	boost::asio::io_service service;
	TcpServer echoServer(service, 15550);
	echoServer.setMessageCallback(onMessage);
	std::cout << "server started" << std::endl;
	service.run();
}

int main(int argc, char* argv[])
{
	test();

	system("pause");
}