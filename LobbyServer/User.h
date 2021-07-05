#pragma once
#include "Network/Tcp/Client.h"
#include "LobbyProtocol/LobbyProtocol.h"
#include <vector>
#include <Core/GameConstants.h>
#include <Network/DynamicParser.h>
#include <Network/AspApi/AspApi.h>

namespace Lunia {
	namespace XRated {
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
			enum class ErrorLevel {
				Curious,			///< not that critical but it should not happen by normal client.
				Critical,			///< critical, it's doubtful about a sort of hack.
				Unexpected,			///< internal wrong situation
				Pvp
			};
			class User : public Net::ClientTcp {
			public:
				inline User(uint32& userId, asio::ip::tcp::socket&& socket)
					: ClientTcp(std::move(socket))
					, m_userId(userId)
					, m_NumberOfSlots(0)
				{
					Logger::GetInstance().Info("User :: Hey, I was created!", GetId());
					m_AccountLicenses.reserve(17);
				}

				std::shared_ptr<User> shared_from_this();

				void Authorize(std::shared_ptr<User> user, int16 errorNumber, Http::TextPacket* packet,
					unsigned int senderUniqueSerial);
				
				void CharacterListed(std::shared_ptr<User> user, int16 errorNumber, Http::TextPacket* packet,
					unsigned int senderUniqueSerial);
				void KeyLoaded(std::shared_ptr<User> user, int16 errorNumber, Http::TextPacket* packet,
					unsigned int senderUniqueSerial);
				void Auth_2nd_Check(std::shared_ptr<User> user, int16 errorNumber, Http::TextPacket* packet,
					unsigned int senderUniqueSerial);

				uint32 GetId() const;

				uint32 GetUniqueSerial() const;

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

				bool IsAccountAuthorized() const;

				bool IsAuthenticated() const;

				bool IsAValidCharacterName(String& characterName);

				bool DeleteCharacter(String& characterName);

				const String& GetCharacterName() const;

				bool SetSelectedCharacter(String& characterName);

				void SetCharacterStateFlags(const XRated::CharacterStateFlags& flag);

				XRated::CharacterStateFlags GetCharacterStateFlags() const { return m_CharacterStateFlags; }

				bool IsCharacterSelected() const;

				void Error(ErrorLevel error, const String& message); // error handling with different outcome based on severity.
			public://Network Related;
				void Send(Serializer::ISerializable& packet);

				uint32 Parse(uint8* buffer, size_t size);

				void Terminate(bool withRemoval = true);
			private:
				//Alive m_lastTickAlivePing;

				bool m_waitingOnAlivePing;

				uint32 m_userId;

				//uint32 m_alivePacketCount;

				String m_Locale;

				String m_AccountName;

				bool									m_isAuthenticated;

			public:
				uint8									m_NumberOfSlots;

				std::vector<int>						m_AccountLicenses;

				std::vector<XRated::LobbyPlayerInfo>	m_Characters;

				XRated::LobbyPlayerInfo					m_selectedCharacter;

				XRated::CharacterStateFlags				m_CharacterStateFlags;
			private:
				std::mutex mtx;
				//DynamicParser<std::shared_ptr<User>>	m_Parser;
			};
			typedef std::shared_ptr<User> UserSharedPtr;
			typedef std::weak_ptr<User> UserWeakPtr;
			typedef std::vector<XRated::LobbyPlayerInfo>::iterator CharactersIterator;
		}
	}
}
