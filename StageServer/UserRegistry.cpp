#include "UserRegistry.h"
#include "../Core/Utils/InitFunction.h"
#include <Core/Utils/ThreadPool.h>
namespace Lunia {
	namespace Net
	{
		StageServer::UserSharedPtr UserRegistry::MakeUser(asio::ip::tcp::socket& socket) {
			StageServer::UserSharedPtr user(new StageServer::User(m_curTempUserId, std::move(socket)));
			StageServer::UserWeakPtr userWeak = user;

			m_users.push_back(user);

			m_usersByUserId[m_curTempUserId] = userWeak;

			OnUserConnected(user);

			m_curTempUserId--;

			return user;
		}

		/*
			Pending removal on the database and cleanup.
		*/
		void UserRegistry::RemoveUser(StageServer::UserSharedPtr& user) {
			//m_users.erase(user->GetId());
			AutoLock _l(m_usersMutex);
			this->RemoveUser(user, _l);
		}
		/*
			Pending removal on the database and cleanup.
		*/
		void UserRegistry::RemoveUser(StageServer::UserSharedPtr& user, AutoLock& /*No need to touch this because the scope is gonna be locked automatically*/) {
			/*
				Key pending aproval for this change.
			*/

			OnUserDisconnected(user);

			m_usersByUserId.erase(user->GetId());

			if (user->IsAuthenticated())
				m_autorizedUsersByUserId.erase(user->GetId());

			for (auto it = m_users.begin(); it < m_users.end(); it++)
			{
				if ((*it)->GetId() == user->GetId())
				{
					(*it)->CloseSocket();

					m_users.erase(it);

					break;
				}
			}
		}

		void UserRegistry::AuthenticateUser(StageServer::UserSharedPtr& user)
		{
			if (user)
			{
				AutoLock _l(m_usersMutex);
				StageServer::UserWeakPtr userWeak = user;

				uint32 oldUserId = user->GetId();

				m_usersByUserId.erase(user->GetId());

				m_usersByUserId[m_curUserId] = userWeak;
				m_autorizedUsersByUserId[m_curUserId] = userWeak;

				user->SetId(m_curUserId);
				user->SetIsAuthenticated();

				m_curUserId++;

				OnUserAuthenticated(user, oldUserId);
			}
		}

		StageServer::UserSharedPtr UserRegistry::GetUserByUserId(uint32 userId) {
			AutoLock _l(m_usersMutex);
			auto ptr = StageServer::UserSharedPtr();
			if (m_usersByUserId.find(userId) != m_usersByUserId.end())
			{
				ptr = m_usersByUserId[userId].lock();
			}

			return ptr;
		}
		UserRegistry::UserRegistry(const uint32& timeout) {
			m_timeOutTimer = timeout;
			m_keepAliveThread = std::thread([&] {
				while (m_keepAliveLoop)
				{
					if (m_usersByUserId.size() > 0)
						NextAlivePingForAll();

					std::unique_lock<std::mutex> _l(m_conditionalVar_lock);
					if (m_conditionalVar.wait_for(_l, std::chrono::milliseconds(m_timeOutTimer), [&] { return !m_keepAliveLoop; }))
						Logger::GetInstance().Info("Keep Alive Loop has been suceffully disabled");
				}
				});
		}
		void UserRegistry::NextAlivePingForAll()
		{
			AutoLock _l(m_usersMutex);
			for (auto& user : m_users) {
				if (user->IsWaitingOnAlivePing())
				{
					user->SatisfyAlivePingWait();
				}
				else
				{
					Logger::GetInstance().Info("Invalid Alive Processing. Terminating user's connection => {0}", user->GetId());

					this->RemoveUser(user, _l);
				}
			}
		}
	}

	static utils::InitFunction initFunction([]()
		{
			Net::UserRegistry::GetInstance().OnUserConnected.Connect([](const StageServer::UserSharedPtr& user)
				{
					Logger::GetInstance().Info("UserRegistry :: OnUserConnected :: userId@{0}", user->GetId());
				});

			Net::UserRegistry::GetInstance().OnUserDisconnected.Connect([](const StageServer::UserSharedPtr& user)
				{
					Logger::GetInstance().Info("UserRegistry :: OnUserDisconnected :: userId@{0}", user->GetId());

				});

			Net::UserRegistry::GetInstance().OnUserAuthenticated.Connect([](const StageServer::UserSharedPtr& user, const uint32& oldUserId)
				{
					Logger::GetInstance().Info("UserRegistry :: OnUserAuthenticated :: userId@{0} oldUserId@{1}", user->GetId(), oldUserId);
				});
		});
}