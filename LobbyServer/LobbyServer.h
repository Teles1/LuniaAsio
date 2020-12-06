#pragma once
#include "../Core/Core.h"
#include "../Network/stdafx.h"
#include "./LobbyProtocol/LobbyProtocol.h"
#include "../Core/Utils/Math/Random.h"
namespace Lobby{
	struct Config {
	public:
		std::string API = "http://localhost:51542";
		std::string ServerIp = "127.0.0.1";
		uint16 ServerPort = 15550;
		std::string ServerName = "Lobby_CA-DEV";
		uint16 PingTimeout = 30; //seconds
	}Config;
	struct Api {
	private:
		httplib::Client cli;
	public:
		Api(const char* in) : cli(in)
		{
			cli.set_read_timeout(5, 0);
			cli.set_write_timeout(5, 0);
		}
		bool Send(std::string in) {
			in.insert(0, "/Lobby/");
			if (auto res = cli.Get( in.c_str())) {
				if (res->status == 200) {
					return true;
				}
				else {
					ERROR_LOG("{0}", in);
				}
			}else
				ERROR_LOG("Cannot communicate with the api! {0}", res.error());
			return false;
		}
	}Api(Config.API.c_str());
	
	enum Static { 
		SendBufferSize = 2 << 13
	};
	void onConnection(const Network::UserPtr& user);
	void onDisconnection(const uint32 userID);
	void onMessage(const Network::UserPtr& user, uint8* data, size_t len);
	void print_hex_memory(void* mem);
	void InitializeApi();
#pragma region User Related
	bool QueryAliveAuth(const Network::UserPtr& user);
	bool CheckAliveAuth(const Network::UserPtr& user);
	void Send(const Network::UserPtr& user, Serializer::ISerializable& packet);
#pragma endregion
#pragma region Handlers
	void handleHead(const Network::UserPtr& user, Protocol::Head message);
	void handleAuth(const Network::UserPtr& user, Protocol::Auth message);
	void handleAlive(const Network::UserPtr& user, Protocol::Alive message);
#pragma endregion
}