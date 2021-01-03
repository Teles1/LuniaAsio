#ifndef User_H
#define User_H
#pragma once
#include "Network/Tcp/Client.h"
#include <StageServer/Service.h>
#include <StageServer/QuickSlot.h>
#include <Network/Api/Api.h>
#include <StageServer/User/PetData.h>
#include <Logic/Logic.h>
#include <Logic/Object/Player.h>
#include <StageServer/User/Services/ExpFactorManager.h>
#include <StageServer/User/Services/FamilyManager.h>
#include <StageServer/User/Services/GuildState.h>
#include <StageServer/Protocol/Protocol.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			enum class ErrorLevel {
				Curious,			///< not that critical but it should not happen by normal client.
				Critical,			///< critical, it's doubtful about a sort of hack.
				Unexpected,			///< internal wrong situation
				Pvp
			};
			class IUserRoom;
			class User 
				: public Net::ClientTcp
			{
			public:
				enum STATE { NONE, LOAD, ACTIVE, TRADE, TRADEREADY, SHOP, VOTING, OnReleasing };
			public:
				User(asio::ip::tcp::socket&& socket, const uint32& userId);

				void Init();
				void Flush();
				void SendRewardMailByGainStageLicense(const XRated::StageLicense& license);
				void SetSerial(const uint64& userSerial);
				void AddPassive(const uint32& hash);
				void RemovePassive(const uint32& hash);
				void RoomParted();
				void Terminate();
				void Close();
				void Error(ErrorLevel error, const String& message); // error handling with different outcome based on severity.
				void SetCharacterName(const String& name);
				void Update(const float& dt);
				void RoomJoined(std::shared_ptr<IUserRoom> room, StageLicense& targetStage);

				bool UpdateInitialPlayerData();
				bool ClearGuildInfo();
				bool PlayerDeleted();
				bool AuthConnection(const json& result);
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
				bool IsConnected() const;
				bool IsAuthenticated() const;
				bool IsAbleToJoinStage(const StageLicense& targetStage) const; // IsEnableJoinToStage

				uint8 GetTeamNumber() const;


				uint16 GetLevel() const;
				uint16 GetPvpLevel() const;
				uint16 GetRoomCapacity() const;//GetStartUserCount


				uint32 GetRoomIndex() const;
				uint32 GetId() const;


				const int64& GetActivationSerial() const;


				const uint64& GetSerial() const;


				float GetExpFactor();


				std::string GetRoomPass() const;
				String GetCharacterName() const;


				Logic::Player* GetPlayer() const;
				CharacterStateFlags GetCharacterStateFlags() const;
				StageLicense& GetCurrentStage();
				GuildState& GetGuildState();
				Logic::ILogic::PlayerInitialData* GetInitialPlayerData();
				const Constants::ClassType& GetClassType() const;

				std::mutex& GetSyncObject();
			public://Network Related;
				void Send(Protocol::IPacketSerializable& packet);
				uint32 Parse(uint8* buffer, size_t size);
			public:
				std::mutex 									mtx;
			private:
				uint32										m_UserId = 0;
				uint64										m_UserSerial = 0; //database
				STATE										m_State;
				String										m_CharacterName;
				PetDatas									m_PetDatas;
				Logic::Player*								m_Player = nullptr;
				std::shared_ptr<IUserRoom>					m_Room;
				GuildState									m_GuildState;
				ExpFactorManager							m_ExpFactorManager;
				FamilyManager								m_FamilyManager;
			public: //Auth_Publisher
				std::wstring								m_SecuKey;
				std::wstring 								m_UsingLocale;
				std::wstring								m_Name;
				std::string									m_RoomPass;
				uint16										m_RoomCapacity;
				uint32										m_RoomIndex;
				uint8										m_TeamNumber;
				int64										m_RoomActivationSerial;
				StageStates									m_StageStateFlags;
				StageLicense								m_LastStage;
				StageLicense								m_CurrentStage;
			public: //Auth
				Logic::ILogic::PlayerInitialData			m_PlayerInitialData;
				CharacterStateFlags							m_CharacterStateFlags;
				DateTime									m_CreateDate;
				DateTime									m_LastLoggedDate;
				uint32										m_CharacterLicenses;
				std::vector<XRated::StageLicense>			m_StageLicenses;
				std::vector<uint32>							m_SkillLicenses;
				QuickSlot									m_QuickSlot;
				uint32										m_CurrentStageHash;
			public: //Alive
				DWORD										m_AliveTime;
			private:
				std::shared_ptr<User> shared_from_this();
			};
			typedef std::shared_ptr<User> UserSharedPtr;
			typedef std::weak_ptr<User> UserWeakPtr;
			typedef std::vector<XRated::LobbyPlayerInfo>::iterator CharactersIterator;

			//template<> std::shared_ptr<User> shared_from(User* derived);
		}
	}
}
#endif // ! User_H