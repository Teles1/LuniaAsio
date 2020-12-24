#include "ClientProxyLobby.h"

#include <LobbyServer/LobbyProtocol/LobbyProtocol.h>

ClientProxyLobby::ClientProxyLobby(asio::ip::tcp::socket&& socket) : ClientProxy(std::move(socket))
{

}; /* WARN socket is moved a shit ton of times here */

void ClientProxyLobby::SetAsAuthenticated()
{
	this->m_isAuthenticated = true;
}

bool ClientProxyLobby::IsAuthenticated() const
{
	return this->m_isAuthenticated;
}

void ClientProxyLobby::SetHasSecondPasswordAuthentication()
{
	this->m_hasSecondPasswordAuthentication = true;
}

bool ClientProxyLobby::HasSecondPasswordAuthentication() const
{
	return this->m_hasSecondPasswordAuthentication;
}

bool ClientProxyLobby::Ping()
{
	Lunia::Lobby::Protocol::Alive packet;

	this->m_isWaitingOnPing = false;

	/*
		// Doesnt seem to be needing values at all?!

		packet.Index  = m_lastTickAlivePing.temp.index  = ++m_AlivePacketCount;
		packet.Value1 = m_lastTickAlivePing.temp.value1 = (uint32)GetTickCount64();
		packet.Value2 = m_lastTickAlivePing.temp.value2 = 0;
		packet.Value3 = m_lastTickAlivePing.temp.value3 = 0;
	*/;

	this->MakeSocketAsyncWriteSerializable(packet);

	return true;
}