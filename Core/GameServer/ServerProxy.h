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
		m_clientRegistry.OnClientCreated.Connect([& /* TODO Pass PacketListener ref */ ](std::shared_ptr<TClientScope>& client)
		{
			client->OnSocketReadPacket.Connect([client /* TODO Pass PacketListener ref */ ](char* buffer, unsigned short& len)
			{
				/*
				Net::StreamReader streamReader(buffer);

				PacketListener.Invoke(0, streamReader.GetSerializedTypeHash(), streamReader);

				HandleRead();
				*/
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

