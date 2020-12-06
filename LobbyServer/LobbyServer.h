#pragma once
#include "../Core/Core.h"
#include "../Network/stdafx.h"
#include "./LobbyProtocol/LobbyProtocol.h"
#include "../Core/Utils/Math/Random.h"
#include "../Core/fwEvent.h"

namespace Lobby{
	struct Config {
	public:
		std::string API = "http://localhost:51542";
		std::string ServerIp = "127.0.0.1";
		uint16 ServerPort = 15550;
		std::string ServerName = "Lobby_DEV";
		uint16 PingTimeout = 30; //seconds
	}Config;

	class DatabaseHooker {
	private:
		httplib::Client cli;
	public:
		DatabaseHooker(const char* in) : cli(in)
		{
			httplib::Headers headers{
				{"ServerName", Config.ServerName},
				{"ServerIp", Config.ServerIp}
			};
			cli.set_default_headers(headers);
			cli.set_read_timeout(5, 0);
			cli.set_write_timeout(5, 0);
		}
		bool Send(std::string in) {
			in.insert(0, "/Lobby/");
			if (auto res = cli.Get( in.c_str())) {
				if (res->status == 200) {
					return true;
				}
				else if(res->status == 400){
					WARN_LOG("[{0}]", res->body);
				}
				else if (res->status == 404)
					WARN_LOG("Could not reach the API specified[{0}].", Config.API);
			}else
				WARN_LOG("Cannot communicate with the api! {0}", res.error());
			return false;
		}
		~DatabaseHooker() {}
	};
	DatabaseHooker Api(Config.API.c_str());
	//UserManager users(Api);
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

	static fwEvent<const Network::UserPtr&, uint8*, size_t> OnConnectionPacketReceived;
};

struct HandlerAndPacket {
	void* handler;
	Serializer::ISerializable* packet;
};

class PacketHandler {
private:

	std::unordered_map<uint16, HandlerAndPacket> packetHandlers;
public:

	 template<typename PacketType>
	 void On(void (*handlerPtr)(PacketType&)) {

		uint16 packetNameHashed = PacketType::TypeHash;

		HandlerAndPacket hNdPckt = {
			handlerPtr,
			new PacketType
		};

		packetHandlers[packetNameHashed] = hNdPckt;
	}

	template<typename PacketType>
	void On(uint16 packetNameHashed, HandlerAndPacket hNdPckt) {
		packetHandlers[packetNameHashed] = hNdPckt;
	}

	void operator()(uint16 packetNameHashed, Serializer::StreamReader stream) {
		if (packetHandlers.find(packetNameHashed) != packetHandlers.end()) {
			HandlerAndPacket hNdpckt = packetHandlers.at(packetNameHashed);

			auto packet = hNdpckt.packet;

			stream.Read(*packet);

			((void(*)(Serializer::ISerializable&))hNdpckt.handler)(*packet);
		}
	}
};

PacketHandler pcktHandlerInstance;

/*
class PacketHandlerImpl {
public:

	PacketHandlerImpl() { };

	std::unordered_map<uint16, HandlerAndPacket> storedHandlers;
	std::unordered_map<uint32, PacketHandler*> userPacketHandlers;

	template<typename PacketType>

	void on(std::wstring packetNameW, void (*handlerPtr)(PacketType&)) {
		std::unordered_map<uint32, PacketHandler*>::iterator it = this->userPacketHandlers.begin();

		uint16 packetNameHashed = StringUtil::Hash(packetNameW);

		HandlerAndPacket hNdPckt = {
			handlerPtr,
			new PacketType
		};

		for (it != userPacketHandlers.end(); it++;) {
			PacketHandler* pcktHandler = it->second();

			pcktHandler.on(packetNameHashed, hNdPckt);
		}

		storedHandlers[packetNameHashed] = hNdPckt;
	}

	void createPacketHandlerForClientId(uint32 clientId) {
		PacketHandler* pcktHandler;

		std::unordered_map<uint16, HandlerAndPacket>::iterator it = this->storedHandlers.begin();

		for (it != storedHandlers.end(); it++;) {
			pcktHandler.on(it->first, it->second);
		}

		this->userPacketHandlers[clientId] = pcktHandler;
	};

	void handle(uint32 clientId, uint8* data, size_t len) {

		if (userPacketHandlers.find(clientId) == userPacketHandlers.end())
			return;

		PacketHandler* pcktHandler = userPacketHandlers.at(clientId);

		Serializer::StreamReader stream = Serializer::StreamReader(data, len);

		uint16 packetNameHashed;
		stream.Read(packetNameHashed);

		(*(pcktHandler)).handle(packetNameHashed, stream);
	}
};
*/

/*
template<typename T, typename ... Args>
class fwEventContainer {

	std::map<T, std::shared_ptr<fwEvent<Args...>> > events;

public:
	void Contain(T identifier, fwEvent<Args...> &event) {
		events[identifier] = std::make_shared<fwEvent<Args...>>(event);
	}

	template<typename ... Args>
	bool operator()(T identifier, Args... args) {
		if (events.find(identifier) != events.end()) {

			auto e = events.at(identifier);
		}

		return true;
	}
};

fwEventContainer<uint16> eventContainer;
*/