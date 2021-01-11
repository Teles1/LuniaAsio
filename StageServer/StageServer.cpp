#pragma once
#include "Network/Api/Api.h"
#include "StageServer.h"
#include <StageServer/User/UserManager.h>
#include <Info/Info.h>
#include <StageServer/Service.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			StageServer::StageServer(const ServerAddressStruct& address) : ServerTcp(address.ServerIp, address.ServerPort)
			{
				Net::Api::ApiUrl = Config::GetInstance().Settings.ApiUrl;
				Net::Api api("AddServer");
				api << address.ServerPort;
				api << ConfigInstance().Get("RoomSize", uint16(150));
				while (true) {
					Net::Answer result("", -1);
					if (Config::GetInstance().GetKind() == SquareKind)
						result = api.RequestPost(Config::GetInstance().Get<std::vector<Service::SquareInfo>>("SquareInfos"));
					else if (Config::GetInstance().GetKind() == StageKind)
						result = api.RequestApi();
					else
						Logger::GetInstance().Exception("Not valid StageServer Kind found in the config file"); //it shouldn't get to here 

					if (result.errorCode == 0) {
						Logger::GetInstance().Info("{0} Initialized on port {1}", Config::GetInstance().Settings.ServerName, Config::GetInstance().Settings.ServerAddress.ServerPort);
						break;
					}
					else
						Logger::GetInstance().Error("Could not initiaze server using the API supplied!! {0}", result.errorMessage);
					Sleep(3000);
				}
			}
			void StageServer::HandleNewConnection(const asio::error_code& err_code, asio::ip::tcp::socket& socket)
			{
				auto user = UserManagerInstance().MakeUser(socket);
				AutoLock lock(user->GetSyncObject());
				user->HandleRead();
				user->Init();
				Logger::GetInstance().Info("Connection handled by StageServer");
			}
		}
	}
}
using namespace Lunia::XRated::StageServer;
using namespace Lunia::XRated::Database;
int main(int argc, char* argv[])
{
	//setting log name to be used on the console.
	Lunia::Logger::GetInstance("StageServer");
	Lunia::Config::GetInstance( "Config_Stage.json" );
	Lunia::Resource::ResourceSystemInstance().AddPath(L"../x64/Debug/");
	DatabaseInstance().Init();
	StageServer stageServer( Lunia::Config::GetInstance().Settings.ServerAddress );
	stageServer.Run(); 
	return 0;
}