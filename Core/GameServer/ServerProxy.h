#pragma once

#include "ClientRegistry.h"

#include <LobbyServer/fwPacketListener.h>

#include <iostream>

template<typename TClientScope>
struct ServerProxy // : TODO? std::enable_shared_from_this<ServerProxy>
{
public:
	ServerProxy()
	{
		m_clientRegistry.OnClientCreated.Connect([&/*TODO Pass PacketListener ref*/](std::shared_ptr<TClientScope>& client)
		{
			client->OnSocketReadPacket.Connect([/*TODO Pass PacketListener ref*/client](uint8_t* buffer, unsigned short len)
			{
				// PacketListener.Invoke();

				// Net::StreamReader sReader(buffer);

				std::cout << "buffer " << buffer << std::endl;
				std::cout << "len " << buffer << std::endl;
				std::cout << " " << std::endl;

				client->MakeSocketAsyncReadSome();
			});
		});
	}

	~ServerProxy() { };

public:
	ClientRegistry<TClientScope>& GetClientRegistry()
	{
		// lock
		return m_clientRegistry;
		// unlock
	}

public:
	Lunia::fwPacketListener PacketListener;

private:
	ClientRegistry<TClientScope> m_clientRegistry;

};

