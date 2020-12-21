#pragma once

#include "../Client.h"

struct ClientProxyLobby : public Client
{
	ClientProxyLobby(asio::ip::tcp::socket&& socket); /* WARN socket is moved a shit ton of times here */
};

struct ClientProxyStage : public Client
{
	ClientProxyStage(asio::ip::tcp::socket&& socket) : Client(std::move(socket)) { }; /* WARN socket is moved a shit ton of times here */
};

struct ClientProxyPvp : public Client
{
	ClientProxyPvp  (asio::ip::tcp::socket&& socket) : Client(std::move(socket)) { }; /* WARN socket is moved a shit ton of times here */
};