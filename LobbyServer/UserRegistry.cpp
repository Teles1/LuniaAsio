#include "UserRegistry.h"
#include "../Core/Utils/InitFunction.h"
#include <Core/Utils/ThreadPool.h>
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
			AutoLock _l(m_usersMutex);
			this->RemoveUser(user, _l);
		}
		/* 
			Pending removal on the database and cleanup.
		*/
		void UserRegistry::RemoveUser(Lobby::UserSharedPtr& user, AutoLock& /*No need to touch this because the scope is gonna be locked automatically*/) {
			/*
				Key pending aproval for this change. 
			*/
			OnUserDisconnected(user);;

			m_usersByUserId.erase(user->GetId());

			if (m_autorizedUsersByUserId.find(user->GetId()) != m_autorizedUsersByUserId.end())
			{
				m_autorizedUsersByUserId.erase(user->GetId());
			}

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

		void UserRegistry::AuthenticateUser(Lobby::UserSharedPtr& user)
		{
			if (user)
			{
				AutoLock _l(m_usersMutex);
				Lobby::UserWeakPtr userWeak = user;

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

		Lobby::UserSharedPtr UserRegistry::GetUserByUserId(uint32 userId) {
			AutoLock _l(m_usersMutex);
			auto ptr = Lobby::UserSharedPtr();
			if (m_usersByUserId.find(userId) != m_usersByUserId.end())
			{
				ptr = m_usersByUserId[userId].lock();
			}

			return ptr;
		}
		UserRegistry::UserRegistry(const uint32& timeout) {
			m_timeOutTimer = timeout;
			m_keepAliveThread = std::thread([&] {
				while (m_keepAliveLoop) {
					if (m_usersByUserId.size() > 0)
						CheckAlive();

					std::unique_lock<std::mutex> _l(m_conditionalVar_lock);
					if (m_conditionalVar.wait_for(_l, std::chrono::milliseconds(m_timeOutTimer), [&] { return !m_keepAliveLoop; }))
						Logger::GetInstance().Info("Keep Alive Loop has been suceffully disabled");
				}
			});
		}
		void UserRegistry::CheckAlive()
		{
			AutoLock _l(m_usersMutex);
			for (auto& x : m_users) {
				if (x->CheckAliveAuth()) {
					if (x->QueryAliveAuth())
						continue;
				}
				Logger::GetInstance().Info("Invalid Alive Processing. Terminating user's connection => {0}", x->GetId());
				this->RemoveUser(x, _l);
			}
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