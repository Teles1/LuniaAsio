#pragma once
#include "Network/Tcp/Client.h"
#include "LobbyProtocol/LobbyProtocol.h"
namespace Lunia {
	namespace Lobby {
		class User : public Net::ClientTcp {
		public:
			User(uint32& userId, asio::ip::tcp::socket&& socket)
				: ClientTcp(std::move(socket))
				, m_userId(userId)
			{
				Logger::GetInstance()->Info("User :: Hey, I was created!", GetUserId());
			}

			const uint32 GetUserId();

			void SetUserId(const uint32& userId);

			void SetUserLocale(const String& inLocale);

			void SetUserAccountName(const String& inAccountName);

			bool QueryAliveAuth();

			void SetIsAuthenticated(const bool& toggle);

			const bool IsAuthenticated();

		public://Network Related;
			void Send(Serializer::ISerializable& packet);

			uint32 Parse(uint8* buffer, size_t size);

		private:
			enum Marks { AliveReceived, AliveCleared };
			struct Alive {
				Marks	Mark;

				///	backup alive data
				struct AliveData
				{
					uint32	index;
					uint32	value1;
					uint32	value2;
					uint32	value3;

					bool	operator==(AliveData& aliveData)
					{
						if (index == aliveData.index && value1 == aliveData.value1
							&& value2 == aliveData.value2 && value3 == aliveData.value3)
							return	true;

						return	false;
					}
				} answer, temp;
			} m_Alive;

			uint32 m_userId;

			uint32 m_AlivePacketCount;

			String m_Locale;

			String m_AccountName;

			bool m_isAuthenticated;

		private:
			std::mutex mtx;
		};
		typedef std::shared_ptr<User> UserSharedPtr;
	}
}
