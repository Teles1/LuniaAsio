#include "UserRegistry.h"
#include "../Core/Utils/InitFunction.h"

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

	Lobby::UserSharedPtr UserRegistry::MakeUser(asio::ip::tcp::socket& socket){

		Lobby::UserSharedPtr user(new Lobby::User(m_curUserId, std::move(socket)));
		m_users.emplace(m_curUserId, user);
		m_curUserId++;
		OnUserConnected(user);
		return user;
	}
	void UserRegistry::RemoveUser(Lobby::UserSharedPtr& user){

		m_users.erase(user->GetUserId());
		OnUserDisconnected(user);
	}
	Lobby::UserSharedPtr UserRegistry::GetUserByUserId(uint32 userId) {
		auto ptr = Lobby::UserSharedPtr();
		auto it = m_users.find(userId);

		if (it != m_users.end())
			ptr = it->second;

		return ptr;
	}
}

static utils::InitFunction initFunction([](){
	net::UserRegistry::GetInstance()->OnUserConnected.Connect(
		[](const Lobby::UserSharedPtr& user){
				Logger::GetInstance()->Info("UserRegistry :: OnUserConnected :: userId@{0}", user->GetUserId());
		});

	net::UserRegistry::GetInstance()->OnUserDisconnected.Connect(
		[](const Lobby::UserSharedPtr& user){
				Logger::GetInstance()->Info("UserRegistry :: OnUserDisconnected :: userId@{0}", user->GetUserId());
		});
});