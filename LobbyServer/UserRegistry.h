#ifndef UserRegistry_H_Guard
#define UserRegistry_H_Guard
#pragma once
#include <Network/Tcp/Shared.h>
namespace Lunia {
	namespace XRated {
		namespace Lobby {
			template<typename User>
			class UserRegistryClass
			{
			public:
				typedef std::shared_ptr<User> UserSharedPtr;
				typedef std::weak_ptr<User> UserWeakPtr;
				UserRegistryClass(const UserRegistryClass&) = delete; //anti creation  of a copy
				UserRegistryClass& operator= (const UserRegistryClass&) = delete; // anti copy
				UserRegistryClass(UserRegistryClass&&) = delete;
				UserRegistryClass& operator=(UserRegistryClass&&) = delete;
				~UserRegistryClass() { m_keepAliveLoop = false; m_keepAliveThread.join(); }
				inline static UserRegistryClass& GetInstance(const uint32& timeout = 30000/*ms*/) {
					static UserRegistryClass								m_instance(timeout);
					return m_instance;
				}
			public:

				inline UserSharedPtr& MakeUser(asio::ip::tcp::socket& socket) {
					m_usersByUserId[m_curTempUserId] = std::make_shared<User>(m_curTempUserId, std::move(socket));

					OnUserConnected(m_usersByUserId[m_curTempUserId]);

					m_curTempUserId--;

					return m_usersByUserId[m_curTempUserId+1];
				}

				inline void RemoveUser(const UserSharedPtr& user) {
					AutoLock _l(m_usersMutex);
					this->RemoveUser(user, _l);
				}

				inline void AuthenticateUser(UserSharedPtr& user) {
					if (user)
					{
						AutoLock _l(m_usersMutex);

						uint32 oldUserId = user->GetId();

						m_usersByUserId.erase(user->GetId());

						m_autorizedUsersByUserId[m_curUserId] = user;

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
						return m_usersByUserId[userId];
					}

					return nullptr;
				}

				fwEvent<const UserSharedPtr&>						OnUserConnected;

				fwEvent<const UserSharedPtr&>						OnUserDisconnected;

				fwEvent<const UserSharedPtr&, const uint32&>		OnUserAuthenticated;

			private:
				inline void RemoveUser(const UserSharedPtr& user, AutoLock& _l) {
					/*
						Key pending aproval for this change.
					*/

					OnUserDisconnected(user);

					if (user->IsAuthenticated())
						for (auto& it : m_autorizedUsersByUserId)
						{
							if (it.second->GetId() == user->GetId())
							{
								it.second->Terminate(false);
								it.second->CloseSocket();

								m_autorizedUsersByUserId.erase(it.first);

								break;
							}

					}
				}

				inline UserRegistryClass(const uint32& timeout) {
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
					{
						AutoLock _l(m_usersMutex);
						for (auto& user : m_autorizedUsersByUserId) {
							if (user.second->IsWaitingOnAlivePing())
							{
								user.second->SatisfyAlivePingWait();
							}
							else
							{
								Logger::GetInstance().Info("Invalid Alive Processing. Terminating user's connection => {0}", user.second->GetId());
								user.second->Terminate(false);
								this->RemoveUser(user.second, _l);
							}
						}
						for (auto& user : m_usersByUserId) {
							if (user.second->IsWaitingOnAlivePing())
							{
								user.second->SatisfyAlivePingWait();
							}
							else
							{
								Logger::GetInstance().Info("Invalid Alive Processing. Terminating user's connection => {0}", user.second->GetId());
								user.second->Terminate(false);
								this->RemoveUser(user.second, _l);
							}
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

				std::unordered_map<uint32, UserSharedPtr>			m_usersByUserId;

				std::unordered_map<uint32, UserSharedPtr>			m_autorizedUsersByUserId;
			};
		}
	}
}

#endif // ! UserRegistry_H_Guard
