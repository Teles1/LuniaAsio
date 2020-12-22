#pragma once

#include "../NetworkIO/ClientNetworkIO.h"

#include "../../Serializer/Serializer.h"

struct ClientProxy : public ClientNetworkIO //, TODO? std::enable_shared_from_this<Client>
{
	ClientProxy(asio::ip::tcp::socket&& socket);

	~ClientProxy() { };

	void MakeSocketAsyncWriteSerializable(Lunia::Serializer::ISerializable& packet);

	uint32_t GetId() const
	{
		return this->m_id;
	}

	void SetId(const uint32_t& id)
	{
		this->m_id = id;
	}

	void SetLocale(const std::wstring& inLocale);

	void SetActiveAccountName(const std::wstring& inAccountName);

	std::wstring GetActiveAccountName() const;

	/* Old Name: QueryAliveAuth */
	bool SatisfyAlivePingWait();

	/* Old Name: CheckAliveAuth */
	bool IsWaitingOnAlivePing() const;

	/* Old Name: UpdateAliveAuth */
	// void SetAliveAsLastTickAlivePing(const Alive::AliveData& answer);

public:
	fwEvent<> OnDropped;

private:

	uint32_t m_id = 0;

	bool m_waitingOnAlivePing = false;

	std::wstring m_locale;

	std::wstring m_accountName;
};