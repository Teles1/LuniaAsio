#pragma once

#include "../NetworkIO/ClientNetworkIO.h"

#include "../../Serializer/Serializer.h"

struct ClientProxy : public ClientNetworkIO //, TODO? std::enable_shared_from_this<Client>
{
	ClientProxy(asio::ip::tcp::socket&& socket);

	~ClientProxy() { };

	void MakeSocketAsyncWriteSerializable(Lunia::Serializer::ISerializable& packet);

	uint32_t GetId() const;

	void SetId(const uint32_t& id);

	void SetLocale(const std::wstring& locale);

	void SetActiveAccountName(const std::wstring& accountName);

	std::wstring GetActiveAccountName() const;

	/* Old Name: QueryAliveAuth */
	virtual bool Ping() = 0;

	/* Old Name: CheckAliveAuth */
	bool IsWaitingOnPing() const;

	/* Old Name: UpdateAliveAuth */
	// void SetAliveAsLastTickAlivePing(const Alive::AliveData& answer);

public:
	fwEvent<> OnDropped;


	bool m_isWaitingOnPing = false;

	uint16_t m_numNonACKPings = 0;

private:

	uint32_t m_id = 0;

	bool m_isAuthenticated = false;

	std::wstring m_locale;

	std::wstring m_activeAccountName;
};