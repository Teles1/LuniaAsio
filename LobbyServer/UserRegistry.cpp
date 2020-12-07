#include "./UserRegistry.h"

namespace net
{
	net::UserSharedPtr UserRegistry::MakeUser(boost::asio::ip::tcp::socket&& socket)
	{
		UserSharedPtr user(new Lobby::User(std::move(socket)));

		user->SetUserId(m_curUserId);

		m_users.emplace(m_curUserId, user);

		m_curUserId++;

		OnUserConnected(user);

		return user;
	}
}
/*
static InitFunction initFunction([]()
{
	INFO_LOG("TA EXECUTANDO");

	net::UserRegistry::getInstance()->OnUserConnected.Connect([](const net::UserSharedPtr& user)
	{
		INFO_LOG("UserConnected!!!");
	});
})
*/