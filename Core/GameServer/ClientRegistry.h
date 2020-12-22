#pragma once

#include <asio.hpp>

#include "../fwEvent.h"

template<typename TClientScope>
struct ClientRegistry
{
private:
	typedef std::shared_ptr<TClientScope> ClientSharedPtr;
	typedef std::weak_ptr  <TClientScope> ClientWeakPtr;

public:
	ClientRegistry() { };

	~ClientRegistry() { };

	void MakeClient(asio::ip::tcp::socket&& socket)
	{
		ClientSharedPtr client = std::make_shared<TClientScope>(std::move(socket)); /* WARN socket is moved a shit ton of times here */

		client->SetId(1);

		OnClientCreated(client);

		m_clients.push_back(client);
	}

	void DropClient(ClientSharedPtr& client)
	{
		client->OnDropped();

		OnClientDropped(client);
	}

public:
	fwEvent<ClientSharedPtr&> OnClientCreated;

	fwEvent<ClientSharedPtr&> OnClientDropped;

public:
	template<typename F>
	void ForAllClients(F f)
	{

	}

private:
	std::vector<ClientSharedPtr> m_clients;
};