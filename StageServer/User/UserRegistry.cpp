#include <StageServer/User/UserRegistry.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			UserSharedPtr UserRegistry::MakeUser(asio::ip::tcp::socket& socket) {
				AutoLock lock(m_usersMutex);
				UserSharedPtr user = std::make_shared<User>(m_curUserId, std::move(socket));
				m_curUserId++;
				m_users[m_curUserId] = user;
				OnUserConnected(user);
				return user;
			}
			void UserRegistry::RemoveUser(UserSharedPtr& user) {
				AutoLock _l(m_usersMutex);
				this->RemoveUser(user, _l);
			}

			void UserRegistry::AuthenticateUser(UserSharedPtr& user) {
				if (user)
				{
					AutoLock _l(m_usersMutex);

					uint32 oldUserId = user->GetId();

					user->SetId(m_curUserId);
					user->SetIsAuthenticated();

					m_curUserId++;

					OnUserAuthenticated(user, oldUserId);
				}
			}

			UserSharedPtr UserRegistry::GetUserByUserId(uint32 userId) {
				AutoLock _l(m_usersMutex);
				if (m_users.find(userId) == m_users.end())
					return nullptr;
				return m_users[userId];
			}

			void UserRegistry::RemoveUser(UserSharedPtr& user, AutoLock& _l) {
				/*
					Key pending aproval for this change.
				*/
				AutoLock lock(m_usersMutex);
				if (m_users.find(user->GetId()) == m_users.end()) {
					Logger::GetInstance().Error("User={0} not found", user->GetId());
					return;
				}
				m_users.erase(user->GetId());
				user->CloseSocket();
				OnUserDisconnected(user);
			}

			UserRegistry::UserRegistry() {}
		}

	}
}