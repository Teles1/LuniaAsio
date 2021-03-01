#ifndef UserRegistry_H_Guard
#define UserRegistry_H_Guard
#pragma once
namespace Lunia {
	namespace XRated {
		namespace Net {
			template<typename User>
			class UserRegistry
			{
			public:
				typedef std::shared_ptr<User> UserSharedPtr;
				typedef std::weak_ptr<User> UserWeakPtr;
				UserRegistry(const UserRegistry&) = delete; //anti creation  of a copy
				UserRegistry& operator= (const UserRegistry&) = delete; // anti copy
				UserRegistry(UserRegistry&&) = delete;
				UserRegistry& operator=(UserRegistry&&) = delete;
				~UserRegistry() { m_keepAliveLoop = false; m_keepAliveThread.join(); }
				inline static UserRegistry& GetInstance(const uint32& timeout = 30000/*ms*/) {
					static UserRegistry								m_instance(timeout);
					return m_instance;
				}
			public:
				inline UserSharedPtr MakeUser(asio::ip::tcp::socket& socket) {
					UserSharedPtr user(new User(m_curTempUserId, std::move(socket)));
					UserWeakPtr userWeak = user;

					m_users.push_back(user);

					m_usersByUserId[m_curTempUserId] = userWeak;

					OnUserConnected(user);

					m_curTempUserId--;

					return user;
				}

				inline void RemoveUser(UserSharedPtr& user) {
					//m_users.erase(user->GetId());
					AutoLock _l(m_usersMutex);
					this->RemoveUser(user, _l);
				}

				inline void AuthenticateUser(UserSharedPtr& user) {
					if (user)
					{
						AutoLock _l(m_usersMutex);
						UserWeakPtr userWeak = user;

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

				inline UserSharedPtr GetUserByUserId(uint32 userId) {
					AutoLock _l(m_usersMutex);
					auto ptr = UserSharedPtr();
					if (m_usersByUserId.find(userId) != m_usersByUserId.end())
					{
						ptr = m_usersByUserId[userId].lock();
					}

					return ptr;
				}

				fwEvent<const UserSharedPtr&>						OnUserConnected;

				fwEvent<const UserSharedPtr&>						OnUserDisconnected;

				fwEvent<const UserSharedPtr&, const uint32&>		OnUserAuthenticated;

			private:
				inline void RemoveUser(UserSharedPtr& user, AutoLock& _l) {
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

				inline UserRegistry(const uint32& timeout) {
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

				inline void NextAlivePingForAll() {
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
			private:
				std::thread											m_keepAliveThread;

				bool												m_keepAliveLoop = true; //set this to false to end the keepAlive

				std::mutex											m_conditionalVar_lock;

				std::condition_variable								m_conditionalVar;

				uint32												m_timeOutTimer;

				uint32												m_curTempUserId = 0xFFFFFFFF;

				uint32												m_curUserId = 0;

				std::mutex											m_usersMutex;

				std::vector<UserSharedPtr>							m_users;

				std::unordered_map<uint32, UserWeakPtr>				m_usersByUserId;

				std::unordered_map<uint32, UserWeakPtr>				m_autorizedUsersByUserId;
			};
		}
	}
}

#endif // ! UserRegistry_H_Guard
