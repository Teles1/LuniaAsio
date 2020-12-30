#ifndef ServerProxy_H
#define ServerProxy_H
#pragma once
#include "../ClientRegistry.h"
#include <Core/GameServer/fwPacketHandler.h>
#include <iostream>
namespace Lunia {
	template<typename TClientProxy>
	struct ServerProxy // : TODO? std::enable_shared_from_this<ServerProxy>
	{

		typedef std::shared_ptr<TClientProxy> TClientProxySharedPtr;
		typedef std::weak_ptr<TClientProxy> TClientProxyWeakPtr;

	public:
		inline ServerProxy()
		{
			ClientRegistry.OnClientCreated.Connect([PacketHandler = &PacketHandler, ClientRegistry = &ClientRegistry](TClientProxySharedPtr& client)
				{
					/*
						// It actually should be handled as below!
						// but we would have to change the packetHandler to take const clients
						// and that would need us to make sure every packet handler uses a member function
						// instead of modifying a member variable directly

						client->OnSocketReadPacket.Connect([clientLocked = client, &PacketHandler, &ClientRegistry](char* buffer, unsigned short& len)
						{
							Lunia::Net::StreamReader streamReader(buffer);

							if (clientLocked)
							{
								unsigned short* packetNameHashed = reinterpret_cast<unsigned short*>(&buffer[4]);

								PacketHandler->Invoke(clientLocked, (uint16_t)*packetNameHashed, streamReader);

								std::cout << "Client@" << clientLocked->GetId() << " -> server :: " << (uint16_t)*packetNameHashed << std::endl;
							}
						});
					*/

					std::cout << "Incoming Connection :: Client:" << client->GetId() << "@" << client->GetPeerAddress() << std::endl;

					client->OnSocketReadPacket.Connect([clientId = client->GetId(), &PacketHandler, &ClientRegistry](char* buffer, unsigned short& len)
					{
						Lunia::Net::StreamReader streamReader(buffer);

						TClientProxyWeakPtr clientWeak = ClientRegistry->GetClientById(clientId);

						auto clientLocked = clientWeak.lock();

						if (clientLocked)
						{
							unsigned short* packetNameHashed = reinterpret_cast<unsigned short*>(&buffer[4]);

							PacketHandler->Invoke(clientLocked, (uint16_t)*packetNameHashed, streamReader);
						}
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
}

#endif // ! ServerProxy_H