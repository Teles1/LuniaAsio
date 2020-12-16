#pragma once
#include "Network/Tcp/Client.h"
#include "LobbyProtocol/LobbyProtocol.h"
#include <vector>
#include <Core/GameConstants.h>
namespace Lunia {
	namespace Lobby {

		struct Alive {

			///	backup alive data
			struct AliveData
			{
				AliveData() = default;
				AliveData(uint32 index, uint32 value1, uint32 value2, uint32 value3);
				uint32	index = 0;
				uint32	value1 = 0;
				uint32	value2 = 0;
				uint32	value3 = 0;

				bool	operator==(AliveData& aliveData)
				{
					if (index == aliveData.index && value1 == aliveData.value1
						&& value2 == aliveData.value2 && value3 == aliveData.value3)
						return	true;

					return	false;
				}
			} answer, temp;
		};

		class User : public Net::ClientTcp {
		public:
			User(uint32& userId, asio::ip::tcp::socket&& socket)
				: ClientTcp(std::move(socket))
				, m_userId(userId)
				, m_NumberOfSlots(0)
			{
				Logger::GetInstance().Info("User :: Hey, I was created!", GetId());
			}

			uint32 GetId() const;

			void SetId(const uint32& userId);

			void SetLocale(const String& inLocale);

			void SetAccountName(const String& inAccountName);

			String GetAccountName() const;

			/* Old Name: QueryAliveAuth */
			bool SatisfyAlivePingWait();

			/* Old Name: CheckAliveAuth */
			bool IsWaitingOnAlivePing() const;

			/* Old Name: UpdateAliveAuth */
			void SetAliveAsLastTickAlivePing(const Alive::AliveData& answer);

			void SetIsAuthenticated();

			bool DoesHaveLicense(XRated::Constants::ClassType classType);

			bool IsAuthenticated() const;

			bool HasAnyCharacterWithName(String& characterName);

			bool DeleteCharacter(String& characterName);

			bool HasActiveCharacter();

			bool IsActiveCharacterNameEqual(String& charaterName);

			XRated::LobbyPlayerInfo GetActiveCharacter();

			void MakeActiveCharacterFromAPIRequest();

			void SetActiveCharacter();

			bool HasSecondPasswordAuthentication() const;

			void SetHasSecondPasswordAuthentication();

		public://Network Related;
			void Send(Serializer::ISerializable& packet);

			uint32 Parse(uint8* buffer, size_t size);

		private:
			//Alive m_lastTickAlivePing;

			bool m_waitingOnAlivePing;

			uint32 m_userId;

			//uint32 m_alivePacketCount;

			String m_Locale;

			String m_AccountName;

			bool m_hasSecondPasswordAuthentication = false;

			bool m_isAuthenticated = false;
		public:

			uint8 m_NumberOfSlots;

			uint32 m_AccountLicenses;

			std::vector<XRated::LobbyPlayerInfo> m_Characters;

			XRated::LobbyPlayerInfo* m_selectedCharacter;
		private:
			std::mutex mtx;
		};
		typedef std::shared_ptr<User> UserSharedPtr;
		typedef std::weak_ptr<User> UserWeakPtr;
		typedef std::vector<XRated::LobbyPlayerInfo>::iterator CharactersIterator;
	}
}
