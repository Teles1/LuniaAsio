#pragma once

#include "../ClientRegistry.h"

#include <Core/GameServer/fwPacketHandler.h>

#include <iostream>

template<typename TClientProxy>
struct ServerProxy // : TODO? std::enable_shared_from_this<ServerProxy>
{

	typedef std::shared_ptr<TClientProxy> TClientProxySharedPtr;

public:
	ServerProxy()
	{
		std::cout << "ServerProxy created" << std::endl;

		m_clientRegistry.OnClientCreated.Connect([PacketHandler = &PacketHandler](TClientProxySharedPtr& client)
		{
			client->OnSocketReadPacket.Connect([&client /* TODO <- Thats wrong */, &PacketHandler](char* buffer, unsigned short& len)
			{
				Lunia::Net::StreamReader streamReader(buffer);
					
				unsigned short* packetNameHashed = reinterpret_cast<unsigned short*>(&buffer[0]);

				PacketHandler->Invoke(client, (uint16_t) *packetNameHashed, streamReader);
			});
		});
	}

	~ServerProxy() { };

public:
	ClientRegistry<TClientProxy>& GetClientRegistry()
	{
		// lock
		return m_clientRegistry;
		// unlock
	}

public:
	fwPacketHandler<TClientProxySharedPtr> PacketHandler;

private:
	ClientRegistry<TClientProxy> m_clientRegistry;

};

