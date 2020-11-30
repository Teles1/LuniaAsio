#include <iostream>
#include <string>
#include <map>
#include <array>
#include "StdAfx.h"
#include "./TcpServer/TcpServer.h"
#include "./TcpServer/TcpConnection.h"

using namespace net;
using namespace std;

void onMessage(const TcpConnectionPtr& conn, std::array<char, 4098> data, size_t size)
{

	std::cout.write(data.data(), size);
	std::cout << "\n"  << " [conn] " << conn <<  " [size] " << size << std::endl;
	//conn->asyncSend(data, size);
}

void test()
{
	asio::io_service service;
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