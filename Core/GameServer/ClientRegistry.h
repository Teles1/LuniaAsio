#pragma once

#include <asio.hpp>

#include "../fwEvent.h"

#include <algorithm> // std::find

template<typename TClientScope>
struct ClientRegistry
{
private:
	typedef std::shared_ptr<TClientScope> ClientSharedPtr;
	typedef std::weak_ptr  <TClientScope> ClientWeakPtr;

public:
	ClientRegistry() 
	{
		m_pingThread = std::thread([this]
		{
			while (true)
			{
				this->PingClients();

				std::this_thread::sleep_for(std::chrono::seconds(3));
			}
		});
	};

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

		{
			std::scoped_lock<std::mutex> slock(m_clientsMutex);

			auto it = std::find(m_clients.begin(), m_clients.end(), client);

			if (it != m_clients.end())
			{
				m_clients.erase(it);
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

	void AuthenticateClient(ClientSharedPtr& client)
	{
		client->SetAsAuthenticated();

		std::scoped_lock<std::mutex> slock(m_authenticatedClientsMutex);

		bool canInsert = true;

		auto copyClientId = client->GetId();

		for (auto & clientWeak : m_authenticatedClients)
		{
			if (auto clientLocked = clientWeak.lock())
			{
				if (clientLocked->GetId() == copyClientId)
				{
					canInsert = false;

					break;
				}
			}
		}

		if (canInsert)
		{
			ClientWeakPtr clientWeak = client;

			m_authenticatedClients.push_back(clientWeak);
		}
	}

	void PingClients() /* TODO! Does it need to be authenticated only? */
	{
		// Logger::GetInstance().Info("Ping!");

		std::vector<ClientWeakPtr> nonACKWeakClients;

		{
			std::scoped_lock<std::mutex> slock(m_clientsMutex);

			for (auto& client : m_clients)
			{
				if (client->IsWaitingOnPing())
				{
					client->Ping();

					client->m_numNonACKPings = 0;
				}
				else
				{
					if (client->m_numNonACKPings >= 5)
					{
						ClientWeakPtr clientWeak = client;

						nonACKWeakClients.push_back(clientWeak);
					}
					else
					{
						client->m_numNonACKPings++;
					}
				}
			}
		}

		for (auto& clientWeak : nonACKWeakClients)
		{
			if (!clientWeak.expired())
			{
				auto client = clientWeak.lock();

				Logger::GetInstance().Info("Client@{0} didn't acknowledge 5 pings! Dropping him/her.", client->GetId());

				this->DropClient(client);
			}
		}
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
	std::thread m_pingThread;

	uint32_t m_curId = 0;

	std::vector<ClientSharedPtr> m_clients;
	std::mutex m_clientsMutex;

	std::unordered_map<uint32_t, ClientWeakPtr> m_clientIdToClientWeak;
	std::mutex m_clientIdToClientWeakMutex;

	std::vector<ClientWeakPtr> m_authenticatedClients;
	std::mutex m_authenticatedClientsMutex;
};