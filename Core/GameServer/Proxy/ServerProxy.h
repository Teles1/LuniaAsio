#pragma once

#include "../ClientRegistry.h"

#include <Core/GameServer/fwPacketHandler.h>

#include <iostream>

template<typename TClientProxy>
struct ServerProxy // : TODO? std::enable_shared_from_this<ServerProxy>
{

	typedef std::shared_ptr<TClientProxy> TClientProxySharedPtr;

public:
	inline ServerProxy()
	{
		std::cout << "ServerProxy created" << std::endl;

		ClientRegistry.OnClientCreated.Connect([PacketHandler = &PacketHandler](TClientProxySharedPtr& client)
		{
			client->OnSocketReadPacket.Connect([&client /* TODO <- Thats wrong */, &PacketHandler](char* buffer, unsigned short& len)
			{
				Lunia::Net::StreamReader streamReader(buffer);

				unsigned short* packetNameHashed = reinterpret_cast<unsigned short*>(&buffer[4]);

				PacketHandler->Invoke(client, (uint16_t)*packetNameHashed, streamReader);

				// std::cout << "Client@" << client->GetId() << " -> server :: " << *packetNameHashed << std::endl;
			});
		});
	}

	inline ~ServerProxy() { };

public:
	/*
	ClientRegistry<TClientProxy>& GetClientRegistry()
	{
		// lock
		return m_clientRegistry;
		// unlock
	}
	*/

public:
	fwPacketHandler<TClientProxySharedPtr> PacketHandler;

	ClientRegistry<TClientProxy> ClientRegistry;
};

