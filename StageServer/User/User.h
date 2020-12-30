#pragma once
#include "Network/Tcp/Client.h"
#include <StageServer/Service.h>
#include <Logic/Logic.h>
#include <StageServer/QuickSlot.h>
#include <Network/Api/Api.h>
#include <StageServer/User/ExpFactorManager.h>
#include <StageServer/User/PetData.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			enum class ErrorLevel {
				Curious,			///< not that critical but it should not happen by normal client.
				Critical,			///< critical, it's doubtful about a sort of hack.
				Unexpected,			///< internal wrong situation
				Pvp
			};
			class User : public Net::ClientTcp {
			public:
				inline User(asio::ip::tcp::socket&& socket, const uint32& userId)
					: ClientTcp(std::move(socket)), m_UserId(userId)
				{
					Logger::GetInstance().Info("User :: Hey, I was created!", GetId());
				}
				void Init();

				bool Auth(const json& result);

				bool Auth_CharacterInfos(const json& characterinfo);

				bool Auth_CharacterLicenses(const json& licenses);

				bool Auth_StageLicenses(const json& licenses);

				bool Auth_CharacterRebirth(const json& rebirth);

				bool Auth_SetItemList(const json& items);

				bool Auth_SetBagList(const json& bags);

				bool Auth_SkillLicenses(const json& licenses);
				
				bool Auth_Skills(const json& skills);

				bool Auth_QuickSlots(const json& quickslots);

				bool Auth_PetsInfo(const json& petsinfo);

				bool Auth_PetsItems(const json& petsitems);

				bool Auth_PetsTraining(const json& petstraining);

				void SendRewardMailByGainStageLicense(const XRated::StageLicense& license);

				uint32 GetId() const;

				void SetSerial(const uint64& userSerial);

				const uint64& GetSerial() const;

				bool IsAuthenticated() const;

				void Error(ErrorLevel error, const String& message); // error handling with different outcome based on severity.

				void SetCharacterName(const String& name);

				String GetCharacterName() const;

				void Update(const float& dt);

				StageLicense& GetCurrentStage();

				void Terminate();
			public://Network Related;
				void Send(Serializer::ISerializable& packet);

				uint32 Parse(uint8* buffer, size_t size);
			public:
				std::mutex 									mtx;
			private:
				uint32										m_UserId = 0;
				uint64										m_UserSerial = 0; //database
				String										m_CharacterName;
				ExpFactorManager							m_ExpFactorManager;
				PetDatas									m_PetDatas;
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
				XRated::CharacterStateFlags					m_CharacterStateFlags;
				DateTime									m_CreateDate;
				DateTime									m_LastLoggedDate;
				std::vector<int>							m_CharacterLicenses;
				std::vector<XRated::StageLicense>			m_StageLicenses;
				std::vector<uint32>							m_SkillLicenses;
				QuickSlot									m_QuickSlot;
				uint32										m_CurrentStageHash;
			public: //Alive
				DWORD										m_AliveTime;
			};
			typedef std::shared_ptr<User> UserSharedPtr;
			typedef std::weak_ptr<User> UserWeakPtr;
			typedef std::vector<XRated::LobbyPlayerInfo>::iterator CharactersIterator;
		}
	}
}
