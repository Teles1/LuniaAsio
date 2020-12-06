#pragma once
#include "../Core/Core.h"
#include "../Network/stdafx.h"
#include "./LobbyProtocol/LobbyProtocol.h"
#include "../Core/Utils/Math/Random.h"
#include "../Core/fwEvent.h"

namespace Lobby{
	struct Config {
	public:
		std::string API = "http://localhost:8080/";
		uint16 Port = 15550;
		std::string ServerName = "Lobby_CA-DEV";
		uint16 PingTimeout = 30; //seconds
	}Config;
	httplib::Client api(Lobby::Config.API.c_str());
	enum Static { 
		SendBufferSize = 40960
	};
	void onConnection(const Network::UserPtr& user);
	void onDisconnection(const uint32 userID);
	void onMessage(const Network::UserPtr& user, uint8* data, size_t len);
	void print_hex_memory(void* mem);
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