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
		ClientWeakPtr clientWeak = client;

		client->SetId(m_curId);

		m_clients.push_back(client);

		m_clientIdToClientWeak[m_curId] = clientWeak;

		m_curId++;

		OnClientCreated(client);
	}

	void DropClient(ClientSharedPtr& client)
	{
		m_clientIdToClientWeak.erase(client->GetId());

		std::scoped_lock<std::mutex> slock(m_clientsMutex);
		for (auto it = m_clients.begin(); it < m_clients.end(); it++)
		{
			if ((*it)->GetId() == client->GetId())
			{
				(*it)->Drop();

				m_clients.erase(it);

				break;
			}
		}

		client->OnDropped();

		OnClientDropped(client);
	}

	std::vector<ClientSharedPtr&> GetClients()
	{
		return m_clients;
	}

	ClientSharedPtr& GetClientById(const uint32_t id)
	{
		std::scoped_lock<std::mutex> slock(m_clientIdToClientWeakMutex);

		ClientSharedPtr ptr;

		auto it = m_clientIdToClientWeak.find(id);

		if (it != m_clientIdToClientWeak.end())
			ptr = it->second.lock();
		
		return ptr;
	}

public:
	fwEvent<ClientSharedPtr&> OnClientCreated;

	fwEvent<ClientSharedPtr&> OnClientDropped;

	fwEvent<ClientSharedPtr&> OnClientAuthenticated;

public:
	template<typename F>
	void ForAllClients(F f)
	{
		std::scoped_lock<std::mutex> slock(m_clientsMutex);

		for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
		{
			// f(it);
		}
	}

private:

	uint32_t m_curId = 0;

	std::vector<ClientSharedPtr> m_clients;
	std::mutex m_clientsMutex;

	std::unordered_map<uint32_t, ClientWeakPtr> m_clientIdToClientWeak;
	std::mutex m_clientIdToClientWeakMutex;

	std::vector<ClientWeakPtr> m_authenticatedClients;
	std::mutex m_authenticatedClientsMutex;
};