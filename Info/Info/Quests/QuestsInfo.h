#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct QuestInfo : public Serializer::ISerializable
				{
					struct Condition : public Serializer::ISerializable
					{
						/* special condition */
						bool ScriptOnly;	///< means this condition can be setisfied by only script-side.				
						/* basic character condition */
						uint16 Class;
						uint16 Level;
						uint16 MaxLevel;
						uint16 PvpLevel;
						uint16 PvpMaxLevel;
						uint16 WarLevel;
						uint16 WarMaxLevel;
						uint16 StoredLevel;
						uint16 StoredMaxLevel;
						uint16 RebirthCount;
						uint16 RebirthMaxCount;
						uint16 Life;
						uint32 Money;
						int GuildLevel;				///< need guild level ( -1 is not guild quest )
						const static int NonUseGuildLevel;			///< guild level -1 is not used guild level
						bool IsGuildQuest() const;

						struct BasicCharacterInfo ///< just for IsValid method. is this essential ?
						{
							XRated::Constants::ClassType Class;
							uint16 Level;
							uint16 PvpLevel;
							uint16 WarLevel;
							uint16 StoredLevel;
							uint16 RebirthCount;
							uint16 Life;
							uint32 Money;
						};
						bool IsValid(const PlayerData& player) const;
						bool IsValid(const BasicCharacterInfo& player) const;

						/* extra condition */
						struct Item : public Serializer::ISerializable
						{
							uint32 ItemHash;
							uint16 Count;

							virtual void Serialize(Serializer::IStreamWriter& out) const;
							virtual void Deserialize(Serializer::IStreamReader& in);
						};

						std::vector< Condition::Item > Items;	///< @remarks do not use ItemHash that is already put-in.
						std::vector< StageLicense > Licenses;	///< stage license condition
						std::vector< uint32/*QuestHash*/ > Quests; ///< this can be used for linked quest by accept condition, and multiple quest by complete condition


						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};

					///< specialized complete/fail condition. this represents 3 runtime parameters (XRated::Quest::Params)
					struct Objective : public Serializer::ISerializable
					{
						struct Condition : public Serializer::ISerializable
						{
							std::vector<uint32> Targets; ///< condition can be satisfied by one of these to represent 'OR' condition
							uint32 Count;

							virtual void Serialize(Serializer::IStreamWriter& out) const;
							virtual void Deserialize(Serializer::IStreamReader& in);

							/* condition methods */
							virtual const wchar_t* GetType() const = 0;
							virtual const wchar_t* GetConditionName() const = 0;
							virtual uint32 UpdateParameter(const String& conditionName, uint32 oldParameter, uint32 target, int count) const;
							virtual bool IsFailed(uint32 /*param*/) const { return false; }			///< @return true if the objective is failed
							virtual bool IsSucceeded(uint32 /*param*/) const { return false; }		///< @return true if the objective is succeeded

							virtual bool MakeAllFail() const { return false; }		///< @return true, if the quest should be failed immediately
							virtual bool MakeAllSuccess() const { return false; }	///< @return true, if the quest should be succeeded immediately		
						};

						std::shared_ptr<Objective::Condition> Condition; ///< runtime condition by Condition::GetName()

						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);

						/* detail conditions */
						struct DefeatNpcs : public Condition
						{
							const wchar_t* GetType() const { return L"DefeatNpcs"; }
							const wchar_t* GetConditionName() const { return L"DestroyObject"; }
							//uint32 UpdateParameter(uint32 oldPrameter, uint32 target, uint32 count) const;
							bool IsSucceeded(uint32 param) const { return (param >= Count); } 
						};
						struct ProtectNpc : public Condition
						{
							const wchar_t* GetType() const { return L"ProtectNpc"; }
							const wchar_t* GetConditionName() const { return L"DestroyObject"; }
							//uint32 UpdateParameter(uint32 oldPrameter, uint32 target, uint32 count) const;
							bool IsFailed(uint32 param) const { return (param >= Count); }
							bool MakeAllFail() const { return true; }
						};
					};

					struct Reward : public Serializer::ISerializable
					{
						/* game logical rewards */
						uint32 Exp;
						uint32 WarExp;
						uint32 StateBundleHash;		///< game logic rewards

						/* normal rewards */
						int Money;					///< money can be - value 

						struct Item : public Serializer::ISerializable
						{
							uint32 ItemHash;
							uint16 Count;
							InstanceEx Instance;
							bool TakeAway;				///< true to drop the item from inventory, and Instance will be ignored. you should put condition to make sure that the item exists

							virtual void Serialize(Serializer::IStreamWriter& out) const;
							virtual void Deserialize(Serializer::IStreamReader& in);
						};

						std::vector< Reward::Item > Items;
						std::vector< StageLicense > Licenses;

						/* selectable */
						std::vector< Reward::Item > SelectableItems; ///< player gets one of these

						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					Condition AcceptCondition;
					Condition CompleteCondition;
					Reward AcceptReward;
					Reward CompleteReward;
					std::vector<Objective> Objectives; ///< should be less than XRated::Quest::MaxParameterCount

					uint32 MaximumCompleteableCount; ///< quest total completeable count
					StageLicense AcceptLocation; ///< the stage which a player can get the quest
					uint32 AcceptSourceHash; ///< which gives this quest
					std::vector<uint32/*npc or misc hash*/> CompleteTarget;

					StageLocation							PlayingStage;	
					bool									PossibleToShare;

					uint8									EventQuestType;	
					bool									QuestLumpComplete;	

				private:
					uint32	ResetWeekField;


				public:
					//bool IsPossibleDate(const DateTime& now,const DateTime& acceptDate) const;
					uint32 GetLeftDay(const DateTime& now, const DateTime& expiredDate) const;
					bool IsPossibleDate(const DateTime& now, const DateTime& ExpiredDate) const;
					bool IsPossibleWeek(DateTime::Week::type type) const;
					bool IsHaveResetDate() const;
					DateTime GetExpiredDate(const DateTime& acceptDate) const;

					void SetResetweek(const std::wstring& value, DateTime::Week::type day);
					void SetResetweek(bool sunday, bool monday, bool tuesday, bool wednesday, bool thursday, bool friday, bool saturday);
					bool IsResetday(DateTime::Week::type day) const;

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);

					/* post processed */
					std::pair<uint32 /*QuestHash*/, const QuestInfo*> Next;

					/* helper */
					uint8 UpdateParameter(const String& conditionName, uint32 target, int count, bool isSameTeam, std::vector<uint32>& param/*out*/) const; ///< @return calculated Quest::State
					bool IsRelayQuest() const;
					bool IsFirstQuest() const; ///< @remarks returns always true for non-relayed quest
					bool IsLastQuest() const; ///< @remarks returns always true for non-relayed quest
					bool IsRepeatQuest() const;
					bool IsRepeatQuest(uint32 completeCount) const;
					QuestInfo();
					bool IsGuildQuest() const;
					bool IsShareQuest() const;	
					bool IsPlayingStage() const;

					bool IsEventQuest() const;
					uint8 GetEventType() const;
					bool IsQuestLumpComplete() const;

					std::vector< std::wstring > Tags; 
				};
				struct ClientQuestInfo : public Serializer::ISerializable
				{
					struct QuestType {
						const static uint8 Hunt = 0;
						const static uint8 Gather = 1;
						const static uint8 Deliver = 2;
						const static uint8 Meet = 3;
						const static uint8 Boss = 4;
						const static uint8 Escort = 5;
						const static uint8 TimeAttack = 6;
						const static uint8 Guide = 7;
						const static uint8 Etc = 8;
						const static uint8 Mixed = 9;
						const static uint8 SpecificClass = 10;
						const static uint8 Special = 11;
						const static uint8 NumberOfTypes = 12;
					};

					struct AcceptEvent : public Serializer::ISerializable
					{
						enum EventType
						{
							None = 0,
							PointStage,			
							PointNPCInSquareMap,
							PointStageSelectBtn,
						};

						uint32 Type;
						uint32 Param_1;
						uint32 Param_2;
						std::wstring Param_3;

						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);

						AcceptEvent() : Type(0), Param_1(0), Param_2(0) {};
						~AcceptEvent() {};
					};

					struct SuccessEvent : public Serializer::ISerializable
					{
						enum EventType
						{
							None = 0,
							PointNPCInStage,
						};

						uint32 Type;
						uint32 Param_1;
						std::wstring Param_2;

						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);

						SuccessEvent() : Type(0), Param_1(0)
						{
						};
						~SuccessEvent()
						{
						};
					};

					uint8 Type;
					bool	ShowNextQuest;

					//uint32 QuestId;

					// Commentary append by rapperkjm (09/07/20)
					std::wstring ShortComment;					
					std::wstring MainTopic;						
					std::wstring Category;						
					std::wstring DisplayName;					
					std::wstring BeginDescription;				
					std::wstring EndDescription;				
					std::wstring ClearConditionDescription;		
					std::wstring AcceptInfo;					

					std::wstring FlashPath;

					bool ShowNoticeArrow;						
					bool AutomaticAddQuestProgress;				
					bool ShowAutomaticAddQuestProgressExplain;
					bool ShowAddQuickSlotItemExplain;			

					AcceptEvent QuestAcceptEvent;
					SuccessEvent QuestSuccessEvent;

					std::vector< std::wstring > Tags;

				public:/* lSerializable implements */
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};

			}
		}
	}
}