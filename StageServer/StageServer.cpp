#pragma once
#include "Network/Api/Api.h"
#include "StageServer.h"
#include <StageServer/Common.h>

namespace Lunia {
	namespace StageServer {
		StageServer::StageServer(const char* ip, uint16 port) : ServerTcp(ip, port)
		{
			Net::Api::ApiUrl = Config::GetInstance().m_ApiBase;
			Net::Api api("AddServer");
			api << Config::GetInstance().m_ServerAddress.ServerPort;
			while (true) {
				auto result = api.RequestApi();
				if (result.errorCode == 0 || result.errorCode == 1) {
					Logger::GetInstance().Info("{0} Initialized on port {1}", Config::GetInstance().m_ServerName, Config::GetInstance().m_ServerAddress.ServerPort);
					break;
				}
				else
					Logger::GetInstance().Error("Could not initiaze server using the API supplied!! {0}", result.errorMessage);
				Sleep(3000);
			}
		}
		void StageServer::HandleNewConnection(const asio::error_code& err_code, asio::ip::tcp::socket& socket)
		{
			UserRegistry().MakeUser(socket)->HandleRead();
			Logger::GetInstance().Info("Connection handled by StageServer");
		}
	}
}

int main(int argc, char* argv[])
{
	//setting log name to be used on the console.
	Logger::GetInstance("StageServer");
	Lunia::Config::GetInstance("Config.json");
	Lunia::StageServer::UserRegistry(Lunia::Config::GetInstance().m_PingTimeout);
	//Load Config
	Lunia::StageServer::StageServer stageServer(Lunia::Config::GetInstance().m_ServerAddress.ServerIp.c_str(), Lunia::Config::GetInstance().m_ServerAddress.ServerPort);
	stageServer.Run();
	return 0;
}