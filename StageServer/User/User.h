#pragma once
#include "Network/Tcp/Client.h"
#include <StageServer/StageServerProtocol/StageServerProtocol.h>
#include <Core/GameConstants.h>
#include <StageServer/Service.h>
#include <Logic/Logic.h>
#include <StageServer/QuickSlot.h>

namespace Lunia {
	namespace StageServer {
		enum class ErrorLevel {
			Curious,			///< not that critical but it should not happen by normal client.
			Critical,			///< critical, it's doubtful about a sort of hack.
			Unexpected,			///< internal wrong situation
			Pvp
		};
		class User : public Net::ClientTcp {
		public:
			User(uint32& userId, asio::ip::tcp::socket&& socket)
				: ClientTcp(std::move(socket))
				, m_userId(userId)
			{
				Logger::GetInstance().Info("User :: Hey, I was created!", GetId());
			}

			void Init();

			uint32 GetId() const;

			void SetId(const uint32& userId);

			bool IsAuthenticated() const;

			void SetIsAuthenticated();

			void Error(ErrorLevel error, const String& message); // error handling with different outcome based on severity.

			void SetCharacterName(const String& name);

			String GetCharacterName() const;

			void Terminate();
		public://Network Related;
			void Send(Serializer::ISerializable& packet);

			uint32 Parse(uint8* buffer, size_t size);
		public:
			std::mutex 									mtx;
		private:
			bool										m_IsAuthenticated = false;
			uint32										m_userId;
			String										m_CharacterName;
		public: //Auth_Publisher
			std::wstring								m_SecuKey;
			std::wstring 								m_UsingLocale;
			std::wstring								m_Name;
			std::string									m_RoomPass;
			int											m_RoomIndex;
			int											m_TeamNumber;
			int64										m_RoomActivateSerial;
			uint32										m_RoomFullCount;
			StageStates									m_StageStateFlags;
			XRated::StageLicense						m_LastStage;
			XRated::StageLicense						m_CurrentStage;
		public: //Auth
			XRated::Logic::ILogic::PlayerInitialData	m_PlayerData;
			uint32										m_CurrentStageHash;
			XRated::CharacterStateFlags					m_CharacterStateFlags;
			DateTime									m_CreateDate;
			DateTime									m_LastLoggedDate;
			uint16										m_AddedSkillPoint;
			std::vector<int>							m_CharacterLicenses;
			std::vector<XRated::StageLicense>			m_StageLicenses;
			std::vector<uint32>							m_SkillLicenses;
			QuickSlot									m_QuickSlot;
		public: //Alive
			DWORD										m_AliveTime;
		};
		typedef std::shared_ptr<User> UserSharedPtr;
		typedef std::weak_ptr<User> UserWeakPtr;
		typedef std::vector<XRated::LobbyPlayerInfo>::iterator CharactersIterator;
	}
}
