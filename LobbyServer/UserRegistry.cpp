#include "UserRegistry.h"
#include "../Core/Utils/InitFunction.h"
namespace Lunia {
	namespace Net
	{
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

			//m_users.erase(user->GetId());

			m_usersByUserId[user->GetId()].reset();

			if (m_autorizedUsersByUserId.find(user->GetId()) != m_autorizedUsersByUserId.end())
			{
				m_autorizedUsersByUserId[user->GetId()].reset();
			}

			AutoLock usersLock(m_usersMutex);
			for (auto it = m_users.begin(); it < m_users.end(); it++)
			{
				if ((*it)->GetId() == user->GetId())
				{
					(*it)->CloseSocket();
					m_users.erase(it);
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

				uint32 oldUserId = user->GetId();

				m_usersByUserId[user->GetId()].reset();

				m_usersByUserId[m_curUserId] = userWeak;
				m_autorizedUsersByUserId[m_curUserId] = userWeak;

				user->SetId(m_curUserId);
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
			Net::UserRegistry::GetInstance().OnUserConnected.Connect([](const Lobby::UserSharedPtr& user)
				{
					Logger::GetInstance().Info("UserRegistry :: OnUserConnected :: userId@{0}", user->GetId());
				});

			Net::UserRegistry::GetInstance().OnUserDisconnected.Connect([](const Lobby::UserSharedPtr& user)
				{
					Logger::GetInstance().Info("UserRegistry :: OnUserDisconnected :: userId@{0}", user->GetId());

				});

			Net::UserRegistry::GetInstance().OnUserAuthenticated.Connect([](const Lobby::UserSharedPtr& user, const uint32& oldUserId)
				{
					Logger::GetInstance().Info("UserRegistry :: OnUserAuthenticated :: userId@{0} oldUserId@{1}", user->GetId(), oldUserId);
				});
		});
}