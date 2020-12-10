#include "UserRegistry.h"
#include "../Core/Utils/InitFunction.h"
namespace Lunia {
	namespace Net
	{
#pragma region Singleton
		/*
			Definitions os the static methods/variables MUST be in the cpp.
			I went for shared_ptr in order to avoid deletion of memory manually.
			Let the compiler does it's thing.
		*/

		UserRegPtr UserRegistry::m_instance;
		std::mutex UserRegistry::m_lock;

		UserRegPtr& UserRegistry::GetInstance() {
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

		Lobby::UserSharedPtr UserRegistry::MakeUser(asio::ip::tcp::socket& socket) {
			Lobby::UserSharedPtr user(new Lobby::User(m_curTempUserId, std::move(socket)));
			Lobby::UserWeakPtr userWeak = user;

			m_users.push_back(user);

			m_usersByUserId[m_curTempUserId] = userWeak;

			OnUserConnected(user);

			m_curTempUserId--;

			return user;
		}
		void UserRegistry::RemoveUser(Lobby::UserSharedPtr& user) {

			//m_users.erase(user->GetUserId());

			m_usersByUserId[user->GetUserId()].reset();

			if (m_autorizedUsersByUserId.find(user->GetUserId()) != m_autorizedUsersByUserId.end())
			{
				m_autorizedUsersByUserId[user->GetUserId()].reset();
			}

			std::lock_guard<std::mutex> usersLock(m_usersMutex);
			for (int i = 0; m_users.size(); i++)
			{
				if (m_users[i]->GetUserId() == user->GetUserId())
				{

					m_users[i].reset();

					break;
				}
			}

			OnUserDisconnected(user);
		}

		void UserRegistry::AuthenticateUser(Lobby::UserSharedPtr& user)
		{
			if (user)
			{
				Lobby::UserWeakPtr userWeak = user;

				uint32 oldUserId = user->GetUserId();

				m_usersByUserId[user->GetUserId()].reset();

				m_usersByUserId[m_curUserId] = userWeak;
				m_autorizedUsersByUserId[m_curUserId] = userWeak;

				user->SetUserId(m_curUserId);
				user->SetIsAuthenticated();

				m_curUserId++;

				OnUserAuthenticated(user, oldUserId);
			}
		}

		Lobby::UserSharedPtr UserRegistry::GetUserByUserId(uint32 userId) {
			auto ptr = Lobby::UserSharedPtr();

			if (m_usersByUserId.find(userId) != m_usersByUserId.end())
			{
				ptr = m_usersByUserId[userId].lock();
			}

			return ptr;
		}
	}

	static utils::InitFunction initFunction([]() 
	{
		Net::UserRegistry::GetInstance()->OnUserConnected.Connect([](const Lobby::UserSharedPtr& user) 
		{
			Logger::GetInstance()->Info("UserRegistry :: OnUserConnected :: userId@{0}", user->GetUserId());
		});

		Net::UserRegistry::GetInstance()->OnUserDisconnected.Connect([](const Lobby::UserSharedPtr& user) 
		{
			Logger::GetInstance()->Info("UserRegistry :: OnUserDisconnected :: userId@{0}", user->GetUserId());
		});

		Net::UserRegistry::GetInstance()->OnUserAuthenticated.Connect([](const Lobby::UserSharedPtr& user, const uint32& oldUserId)
		{
			Logger::GetInstance()->Info("UserRegistry :: OnUserAuthenticated :: userId@{0} oldUserId@{1}", user->GetUserId(), oldUserId);
		});
	});
}
