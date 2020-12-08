#include "./UserRegistry.h"

namespace net
{
#pragma region Singleton
	/*
		Definitions os the static methods/variables MUST be in the cpp.
		I went for shared_ptr in order to avoid deletion of memory manually. 
		Let the compiler does it's thing.
	*/

	UserRegPtr UserRegistry::m_instance;
	std::mutex UserRegistry::m_lock;

	UserRegPtr& UserRegistry::GetInstance(){
		UserRegPtr pTmp = m_instance;
		if (pTmp == NULL)
		{
			m_lock.lock(); // thread safe
			if (pTmp == NULL)
				m_instance = UserRegPtr(new UserRegistry());
			m_lock.unlock();
		}
		return m_instance;
	}
	/*
		End of static definitions.
	*/
#pragma endregion

	net::UserSharedPtr UserRegistry::MakeUser(boost::asio::ip::tcp::socket& socket){

		UserSharedPtr user(new Lobby::User(std::move(socket)));
		user->SetUserId(m_curUserId);
		m_users.emplace(m_curUserId, user);
		m_curUserId++;
		OnUserConnected(user);
		return user;
	}
	void UserRegistry::RemoveUser(net::UserSharedPtr& user){

		m_users.erase(user->GetUserId());
		OnUserDisconnected(user);
	}
	UserSharedPtr UserRegistry::GetUserByUserId(uint32 userId) {
		auto ptr = net::UserSharedPtr();
		auto it = m_users.find(userId);

		if (it != m_users.end())
			ptr = it->second;

		return ptr;
	}
}

static InitFunction initFunction([](){
	net::UserRegistry::GetInstance()->OnUserConnected.Connect([](const net::UserSharedPtr& user)
	{
		INFO_LOG("UserConnected!!!");
	});
});