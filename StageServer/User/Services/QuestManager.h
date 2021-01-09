#pragma once
#include <Network/Api/Api.h>
#include <Info/Info.h>
#include <StageServer/Protocol/ToServer.h>
#include <Info/Info/Quests/QuestsInfo.h>
#include <Core/GameConstants.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {

				class User;
				typedef std::shared_ptr<User> UserSharedPtr;
				class QuestManager
				{
					enum ValidQuestConditionResult {
						Success,						
						Failed_PlayerIsNull,			
						Failed_ScriptOnlyQuest,			
						Failed_NotEnoughCharacterStatus,
						Failed_NotJoinedGuild,			
						Failed_NotEnoughGuildLevel,		
						Failed_NotEnoughItems,			
						Failed_HaveNotLicenses,			
						Failed_HaveNotCompleteQuests,	
					};
				public: // packet processors
					void Dispatch(UserSharedPtr, Protocol::ToServer::Quest::Accept& packet);
					void Dispatch(UserSharedPtr, Protocol::ToServer::Quest::Complete& packet);
					void Dispatch(UserSharedPtr, Protocol::ToServer::Quest::Drop& packet);
					void Dispatch(UserSharedPtr, Protocol::ToServer::Quest::ActivityItem& packet);
					void Dispatch(UserSharedPtr, Protocol::ToServer::Quest::ShareToPartyPlayer& packet);
					void Dispatch(UserSharedPtr, Protocol::ToServer::Quest::SharedAnswer& packet);
				public:
					void RefreshCompletedQuests(UserSharedPtr user, const StageLocation& location); ///< refresh @remakrs asynchronous response
					void RefreshWorkingQuests(UserSharedPtr user); ///< update Working Quests @remakrs asynchronous response
					void Release(UserSharedPtr user); ///< release all quest status and update runtime-parameters to DB

					/* update runtime conditions */
					void ObjectDestroyed(UserSharedPtr user, uint32 id, uint8 team);

					void ValidActiveItem(UserSharedPtr user, uint32 questHash);

					bool ClearQuestList();

					void ShareQuest(UserSharedPtr user, const std::wstring& shareOwner, const Database::Info::QuestInfo& info, uint32 questHash);

					void DropEventQuests(uint8 eventType);

				public: // IQuestManager implementation
					uint8 GetQuestState(const UserSharedPtr user, uint32 questHash) const;
					uint8 SetQuestState(UserSharedPtr user, uint32 questHash, uint8 newState);
					uint32 GetQuestParameter(const UserSharedPtr user, uint32 questHash, uint8 paramIndex) const;
					uint32 SetQuestParameter(UserSharedPtr user, uint32 questHash, uint8 paramIndex, uint32 newValue);

				private: // db response
					void CompletedQuestList(const UserSharedPtr& user, Net::Answer& answer);
					void WorkingQuestList(const UserSharedPtr& user, Net::Answer& answer);
					//void Accepted(const UserSharedPtr& user, Net::Answer& answer);
					//void Completed(const UserSharedPtr& user, Net::Answer& answer);
					void ActiveItemQuestInfo(const UserSharedPtr& user, Net::Answer& answer);

					void CompletedQuestCountWIthShare(const UserSharedPtr& user, Net::Answer& answer);
				private: // helpers
				public:
					struct Quest : public XRated::Quest
					{
						const Database::Info::QuestInfo* Info;
						bool ParameterChanged;		///< a flag that means if the parameter has been changed or not for update selection

						Quest(uint32 questHash, const Database::Info::QuestInfo* info, const DateTime::Date& expiredDate, bool parameterChanged);
						Quest(uint32 questHash, const Database::Info::QuestInfo* info, const DateTime::Date& expiredDate);
						Quest(uint32 questHash);
					};
				private:
					const wchar_t* GetValidQuestConditionErrorMsg(ValidQuestConditionResult result) const;
					ValidQuestConditionResult GetValidResult(const UserSharedPtr user, const Database::Info::QuestInfo::Condition& condition, bool wirteLog, uint32 count = 1) const; ///< @return true when the condition is valid for the user
					bool ValidateInventory(UserSharedPtr user, const Database::Info::QuestInfo::Reward& reward, uint32 selectItemIndex, uint32 questHash, uint32 count) const; ///< @return true when user's inventory is enough for rewards
					uint8 IsValidItemQuest(const UserSharedPtr user, const Quest& questInfo, uint32 questHash) const; ///< @return Result when the condition is valid for the user

					/**
						@return false if it fails to give player the reward.
						@remarks result packets (like not-enough money or slot, acquire item and drop item) will be sent automatically
					*/
					bool GiveReward(UserSharedPtr user, const Database::Info::QuestInfo::Reward& reward, uint32 selectItemIndex, uint32 count = 1) const;

					void ChangeActiveQuestState(uint32 questHash, uint8 newState);
					void AddItemQuest(uint32 questHash, uint8 currentState);

					bool IsRepeatableQuest(const Database::Info::QuestInfo& info, uint32 questHash);
					bool IsItemActivityQuest(const Database::Info::QuestInfo* info);


					bool DropQuest(UserSharedPtr user, uint32 questHash);

					void AcceptQuest(UserSharedPtr user, uint32 questHash, uint32 SelectRewardIndex, bool checkLocation = true);

				private: // member variables
					mutable std::mutex cs;
					std::map<uint32/*Quest hash*/, uint32/*Completed Count*/> completedQuests; ///< completed quests in the current stage
					std::vector<Quest> workingQuests;
					std::vector<Quest> itemQuests;			///< ActivityItem Quest Infos
					std::set<UserSharedPtr> dbRequested;

					std::vector< std::pair<uint32, std::wstring> >	dbRequestCompletCount;
				private:
					std::list<Quest> originWorkingQuests;
					std::map<uint32, uint32> originCompletedQuests;

					std::map< uint32, Quest > duplicatedList;

				public:
					class ComplatedQuest
					{
					public:
						uint32 QuestHash;
						const Database::Info::QuestInfo* Info;
						uint32 Count;
					public:
						class Finder
						{
							const uint32 Hash;
						public:
							Finder(uint32 hash) :Hash(hash) {}
							bool operator()(const ComplatedQuest& info) const { return info.QuestHash == Hash; }
						};

						ComplatedQuest(uint32 hash, const Database::Info::QuestInfo* info, uint32 count) :QuestHash(hash), Info(info), Count(count) {}
						ComplatedQuest(const ComplatedQuest& info) :QuestHash(info.QuestHash), Info(info.Info), Count(info.Count) {}
					};

					struct ChangeQuestInfo {
						std::vector<Quest>	workingQuestsToInsert;
						std::vector<Quest>	workingQuestsToUpdate;
						std::vector<Quest>	workingQuestsToDelete;

						std::vector<ComplatedQuest>	completedQuestsToInsert;
						std::vector<ComplatedQuest>	completedQuestsToUpdate;

						void Clear();
					};

					void GetChangedQuestInfoAndOriginBackup(ChangeQuestInfo& info);
				private:
					uint32 GetValidQuestCount() const;
				public:
					void DropGuildQuest(UserSharedPtr user);
				};

			}
		}
	}
