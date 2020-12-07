#pragma once
#include "../Network/Tcp/Client.h"
#include "../Core/Serializer/Serializer.h"
namespace Lobby {
	class PacketHandler;
	struct User : public net::ClientTcp 
	{
		User(boost::asio::ip::tcp::socket&& socket) : ClientTcp(std::move(socket))
		{
			std::cout << "User Created!" << std::endl;
		}

		uint32 GetUserId()
		{
			return m_userId;
		}

		void SetUserId(uint32 userId)
		{
			m_userId = userId;
		}

		void Parse(uint8* buffer, size_t size);

		PacketHandler* packetHandler;
	private:

		uint32 m_userId;
	};

	struct HandlerAndPacket 
	{
		void* handler;
		Serializer::ISerializable* packet;
	};

	class PacketHandler 
	{
	private:

		std::unordered_map<uint16, HandlerAndPacket> packetHandlers;
	public:

		 template<typename PacketType>
		 void On(void (*handlerPtr)(PacketType&)) 
		 {
			uint16 packetNameHashed = PacketType::TypeHash;

			HandlerAndPacket hNdPckt = {
				handlerPtr,
				new PacketType
			};

			packetHandlers[packetNameHashed] = hNdPckt;
		}

		template<typename PacketType>
		void On(uint16 packetNameHashed, HandlerAndPacket hNdPckt) 
		{
			packetHandlers[packetNameHashed] = hNdPckt;
		}

		void operator()(uint16 packetNameHashed, Serializer::StreamReader stream) 
		{
			if (packetHandlers.find(packetNameHashed) != packetHandlers.end()) {
				HandlerAndPacket hNdpckt = packetHandlers.at(packetNameHashed);

				auto packet = hNdpckt.packet;

				stream.Read(*packet);

				((void(*)(Serializer::ISerializable&))hNdpckt.handler)(*packet);
			}
		}
	};
}