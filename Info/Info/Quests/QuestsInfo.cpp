#include <Info/Info/Quests/QuestsInfo.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				const int QuestInfo::Condition::NonUseGuildLevel = -1;
				QuestInfo::QuestInfo()
				{
				}
				void QuestInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::QuestInfo");
					out.Write(L"AcceptCondition", AcceptCondition);
					out.Write(L"CompleteCondition", CompleteCondition);
					out.Write(L"AcceptReward", AcceptReward);
					out.Write(L"CompleteReward", CompleteReward);
					out.Write(L"Objectives", Objectives);
					out.Write(L"MaximumCompleteableCount", MaximumCompleteableCount);
					out.Write(L"AcceptLocation", AcceptLocation);
					out.Write(L"AcceptSourceHash", AcceptSourceHash);
					out.Write(L"CompleteTarget", CompleteTarget);
					out.Write(L"PlayingStage", PlayingStage);
					out.Write(L"PossibleToShare", PossibleToShare);
					out.Write(L"EventQuestType", EventQuestType);
					out.Write(L"QuestLumpComplete", QuestLumpComplete);

					//ResetWeekField
					bool temp;
					if ((ResetWeekField & (0x01 << DateTime::Week::Sunday))) { temp = true; }
					else { temp = false; }
					out.Write(L"ResetWeekSunday", temp);

					if ((ResetWeekField & (0x01 << DateTime::Week::Monday))) { temp = true; }
					else { temp = false; }
					out.Write(L"ResetWeekMonday", temp);

					if ((ResetWeekField & (0x01 << DateTime::Week::Tuesday))) { temp = true; }
					else { temp = false; }
					out.Write(L"ResetWeekTuesday", temp);

					if ((ResetWeekField & (0x01 << DateTime::Week::Wednesday))) { temp = true; }
					else { temp = false; }
					out.Write(L"ResetWeekWednesday", temp);

					if ((ResetWeekField & (0x01 << DateTime::Week::Thursday))) { temp = true; }
					else { temp = false; }
					out.Write(L"ResetWeekThursday", temp);

					if ((ResetWeekField & (0x01 << DateTime::Week::Friday))) { temp = true; }
					else { temp = false; }
					out.Write(L"ResetWeekFriday", temp);

					if ((ResetWeekField & (0x01 << DateTime::Week::Saturday))) { temp = true; }
					else { temp = false; }
					out.Write(L"ResetWeekSaturday", temp);

					out.Write(L"Tags", Tags);
				}

				void QuestInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::QuestInfo");
					in.Read(L"AcceptCondition", AcceptCondition, Condition());
					in.Read(L"CompleteCondition", CompleteCondition, Condition());
					in.Read(L"AcceptReward", AcceptReward, Reward());
					in.Read(L"CompleteReward", CompleteReward, Reward());
					in.Read(L"Objectives", Objectives, std::vector<Objective>());
					in.Read(L"MaximumCompleteableCount", MaximumCompleteableCount, uint32(1));
					in.Read(L"AcceptLocation", AcceptLocation);
					in.Read(L"AcceptSourceHash", AcceptSourceHash);
					in.Read(L"CompleteTarget", CompleteTarget);
					in.Read(L"PlayingStage", PlayingStage, XRated::StageLocation(0, 0));
					in.Read(L"PossibleToShare", PossibleToShare, true);
					in.Read(L"EventQuestType", EventQuestType, uint8(0));
					in.Read(L"QuestLumpComplete", QuestLumpComplete, true);
					ResetWeekField = 0;
					bool temp;


					in.Read(L"ResetWeekSunday", temp, false);
					if (temp == true) ResetWeekField |= 0x01 << DateTime::Week::Sunday;
					in.Read(L"ResetWeekMonday", temp, false);
					if (temp == true) ResetWeekField |= 0x01 << DateTime::Week::Monday;
					in.Read(L"ResetWeekTuesday", temp, false);
					if (temp == true) ResetWeekField |= 0x01 << DateTime::Week::Tuesday;
					in.Read(L"ResetWeekWednesday", temp, false);
					if (temp == true) ResetWeekField |= 0x01 << DateTime::Week::Wednesday;
					in.Read(L"ResetWeekThursday", temp, false);
					if (temp == true) ResetWeekField |= 0x01 << DateTime::Week::Thursday;
					in.Read(L"ResetWeekFriday", temp, false);
					if (temp == true) ResetWeekField |= 0x01 << DateTime::Week::Friday;
					in.Read(L"ResetWeekSaturday", temp, false);
					if (temp == true) ResetWeekField |= 0x01 << DateTime::Week::Saturday;

					if (AcceptCondition.Quests.empty() == false) {
						PossibleToShare = false;
					}

					in.Read(L"Tags", Tags, std::vector< std::wstring >());
				}

				void QuestInfo::Condition::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::QuestInfo::Condition");
					out.Write(L"ScriptOnly", ScriptOnly);

					out.Write(L"Class", Class);
					out.Write(L"Level", Level);
					out.Write(L"MaxLevel", MaxLevel);
					out.Write(L"PvpLevel", PvpLevel);
					out.Write(L"PvpMaxLevel", PvpMaxLevel);
					out.Write(L"WarLevel", WarLevel);
					out.Write(L"WarMaxLevel", WarMaxLevel);
					out.Write(L"StoredLevel", StoredLevel);
					out.Write(L"StoredMaxLevel", StoredMaxLevel);
					out.Write(L"RebirthCount", RebirthCount);
					out.Write(L"RebirthMaxCount", RebirthMaxCount);
					out.Write(L"Life", Life);
					out.Write(L"Money", Money);
					out.Write(L"GuildLevel", GuildLevel);
					out.Write(L"Items", Items);
					out.Write(L"Licenses", Licenses);
					out.Write(L"Quests", Quests);
				}

				void QuestInfo::Condition::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::QuestInfo::Condition");
					in.Read(L"ScriptOnly", ScriptOnly, false);

					in.Read(L"Class", Class, uint16(XRated::Constants::ClassType::AnyClassType));
					in.Read(L"Level", Level, uint16(0));
					in.Read(L"MaxLevel", MaxLevel, uint16(99));
					in.Read(L"PvpLevel", PvpLevel, uint16(0));
					in.Read(L"PvpMaxLevel", PvpMaxLevel, uint16(99));
					in.Read(L"WarLevel", WarLevel, uint16(1));
					in.Read(L"WarMaxLevel", WarMaxLevel, uint16(99));
					in.Read(L"StoredLevel", StoredLevel, uint16(1));
					in.Read(L"StoredMaxLevel", StoredMaxLevel, Constants::StoredMaxLevel);
					in.Read(L"RebirthCount", RebirthCount, uint16(0));
					in.Read(L"RebirthMaxCount", RebirthMaxCount, Constants::RebirthMaxCount);
					in.Read(L"Life", Life, uint16(0));
					in.Read(L"Money", Money, uint32(0));
					in.Read(L"GuildLevel", GuildLevel, NonUseGuildLevel);
					in.Read(L"Items", Items, std::vector<Condition::Item>());
					in.Read(L"Licenses", Licenses, std::vector<StageLicense>());
					in.Read(L"Quests", Quests, std::vector< uint32>());
				}


				bool QuestInfo::Condition::IsGuildQuest() const
				{
					if (NonUseGuildLevel < GuildLevel) {
						return true;
					}
					return false;
				}
				bool QuestInfo::IsShareQuest() const
				{
					return PossibleToShare;
				}
				bool QuestInfo::IsPlayingStage() const
				{
					if (PlayingStage.StageGroupHash != 0) {
						return true;
					}
					return false;
				}
				bool QuestInfo::IsEventQuest() const
				{
					return EventQuestType != 0;
				}

				uint8 QuestInfo::GetEventType() const
				{
					return EventQuestType;
				}

				bool QuestInfo::IsQuestLumpComplete() const
				{
					return QuestLumpComplete;
				}

				void QuestInfo::Condition::Item::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::QuestInfo::Condition::Item");
					out.Write(L"ItemHash", ItemHash);
					out.Write(L"Count", Count);
				}

				void QuestInfo::Condition::Item::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::QuestInfo::Condition::Item");
					in.Read(L"ItemHash", ItemHash);
					in.Read(L"Count", Count);
				}

				void QuestInfo::Objective::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::QuestInfo::Objective");
					out.Write(L"Type", Condition->GetType());
					out.Write(L"Condition", *Condition);
				}

				void QuestInfo::Objective::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::QuestInfo::Objective");
					String type;
					in.Read(L"Type", type, String());

					if (type.empty()) return;
					/* TODO: make condition factory to create detail condition */
					if (type == L"DefeatNpcs")
						Condition = std::make_shared<DefeatNpcs>();
					else if (type == L"ProtectNpc")
						Condition = std::make_shared<ProtectNpc>();
					else
						throw Exception(L"invalid obective type : [{}]", type.c_str());
					in.Read(L"Condition", *Condition.get());
				}

				uint32 QuestInfo::Objective::Condition::UpdateParameter(const String& conditionName, uint32 oldParameter, uint32 target, int count) const
				{
					if (conditionName != GetConditionName()) return oldParameter;
					if (std::find(Targets.begin(), Targets.end(), target) == Targets.end()) return oldParameter; // nothing matched
					return (oldParameter + count) >= Count ? Count : (oldParameter + count);
				}

				void QuestInfo::Objective::Condition::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::QuestInfo::Objective::Condition");
					out.Write(L"Targets", Targets);
					out.Write(L"Count", Count);
				}

				void QuestInfo::Objective::Condition::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::QuestInfo::Objective::Condition");
					in.Read(L"Targets", Targets);
					in.Read(L"Count", Count);
				}

				void QuestInfo::Reward::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::QuestInfo::Reward");
					out.Write(L"Exp", Exp);
					out.Write(L"WarExp", WarExp);
					out.Write(L"StateBundleHash", StateBundleHash);
					out.Write(L"Money", Money);
					out.Write(L"Items", Items);
					out.Write(L"Licenses", Licenses);
					out.Write(L"SelectableItems", SelectableItems);
				}

				void QuestInfo::Reward::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::QuestInfo::Reward");
					in.Read(L"Exp", Exp, uint32(0));
					in.Read(L"WarExp", WarExp, uint32(0));
					in.Read(L"StateBundleHash", StateBundleHash, uint32(0));
					in.Read(L"Money", Money, int(0));
					in.Read(L"Items", Items, std::vector<Reward::Item>());
					in.Read(L"Licenses", Licenses, std::vector<StageLicense>());
					in.Read(L"SelectableItems", SelectableItems, std::vector<Reward::Item>());
				}

				void QuestInfo::Reward::Item::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::QuestInfo::Reward::Item");
					out.Write(L"ItemHash", ItemHash);
					out.Write(L"Count", Count);
					out.Write(L"Instance", Instance);
					out.Write(L"TakeAway", TakeAway);
				}

				void QuestInfo::Reward::Item::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::QuestInfo::Reward::Item");
					in.Read(L"ItemHash", ItemHash);
					in.Read(L"Count", Count);
					//in.Read(L"Instance", Instance);
					in.Read(L"TakeAway", TakeAway);
				}

				bool QuestInfo::Condition::IsValid(const PlayerData& player) const
				{
					BasicCharacterInfo info;
					info.Class = player.Job;
					info.Level = player.BaseCharacter.Level;
					info.PvpLevel = player.PvpLevel;
					info.WarLevel = player.WarLevel;
					info.Life = player.Life;
					info.Money = player.BaseCharacter.Money;
					info.StoredLevel = player.StoredLevel;
					info.RebirthCount = player.RebirthCount;

					return IsValid(info);
				}

				bool QuestInfo::Condition::IsValid(const BasicCharacterInfo& player) const
				{
					if (Level > player.Level || MaxLevel < player.Level)// stage level should be in the range
						return false;
					if (PvpLevel > player.PvpLevel || PvpMaxLevel < player.PvpLevel)// pvp level should be in the range
						return false;
					if (WarLevel > player.WarLevel || WarMaxLevel < player.WarLevel)// war level should be in the range
						return false;
					if (StoredLevel > player.StoredLevel || StoredMaxLevel < player.StoredLevel)// stored level should be in the range
						return false;
					if (RebirthCount > player.RebirthCount || RebirthMaxCount < player.RebirthCount)// trans count should be in the range
						return false;
					if (Life > player.Life)// life should be enough
						return false;
					if (Money > player.Money)// money should be enough
						return false;
					if (Class != (uint16)XRated::Constants::ClassType::AnyClassType && Class != (uint16)player.Class)// this class cannot work for this quest
						return false;

					return true;
				}

				uint8 QuestInfo::UpdateParameter(const String& conditionName, uint32 target, int count, bool isSameTeam, std::vector<uint32>& param/*out*/) const
				{
					int totalCount(0), failCount(0), successCount(0);
					for (std::vector<Objective>::const_iterator i = Objectives.begin(); i != Objectives.end(); ++i)
					{
						if (i->Condition->GetType() == std::wstring(L"DefeatNpcs") && isSameTeam)
						{
							++totalCount;
							continue;
						}

						uint32 val(param[totalCount] = i->Condition->UpdateParameter(conditionName, param[totalCount], target, count));
						if (i->Condition->IsFailed(val))
						{
							if (i->Condition->MakeAllFail())
							{
								return Quest::State::Failed;
							}
							++failCount;
						}
						else if (i->Condition->IsSucceeded(val))
						{
							if (i->Condition->MakeAllSuccess())
							{
								return Quest::State::Succeeded;
							}
							++successCount;
						}
						++totalCount;
					}

					if (totalCount > failCount + successCount)
					{
						return Quest::State::Working;
					}
					else if (totalCount == successCount)
					{
						return Quest::State::Succeeded;
					}
					else if (totalCount == failCount)
					{
						return Quest::State::Failed;
					}

					LoggerInstance().Error("unalbe to decide the quest result. temporarily treated as FAIL");
					return Quest::State::Failed;
				}

				bool QuestInfo::IsRelayQuest() const
				{
					if (AcceptCondition.Quests.empty() == false || Next.first != 0) return true;
					return false;
				}

				bool QuestInfo::IsFirstQuest() const
				{
					if (AcceptCondition.Quests.empty()) return true;
					return false;
				}

				bool QuestInfo::IsLastQuest() const
				{
					if (Next.first == 0) return true;
					return false;
				}

				bool QuestInfo::IsRepeatQuest() const
				{
					return MaximumCompleteableCount > 1;
				}

				bool QuestInfo::IsRepeatQuest(uint32 completeCount) const
				{
					return MaximumCompleteableCount > completeCount;
				}

				bool QuestInfo::IsGuildQuest() const
				{
					return (AcceptCondition.IsGuildQuest() || CompleteCondition.IsGuildQuest());
				}
				uint32 QuestInfo::GetLeftDay(const DateTime& now, const DateTime& expiredDate) const
				{
					if (ResetWeekField == 0) {
						return 0;
					}
					if (now.GetDate() <= expiredDate.GetDate()) {
						return static_cast<int>(expiredDate.GetDate().GetCumulatedDay()) - static_cast<int>(now.GetDate().GetCumulatedDay()) + 1;
					}
					return 0;
				}
				bool QuestInfo::IsPossibleDate(const DateTime& now, const DateTime& ExpiredDate) const
				{
					if (ResetWeekField == 0) {
						return true;
					}
					if (now.GetDate() > ExpiredDate.GetDate()) {
						return true;
					}
					return false;
				}
				bool QuestInfo::IsPossibleWeek(DateTime::Week::type type) const
				{
					if ((0x01 << type) & ResetWeekField) {
						return true;
					}
					return false;
				}
				bool QuestInfo::IsHaveResetDate() const
				{
					if (ResetWeekField == 0) {
						return false;
					}
					return true;
				}
				DateTime QuestInfo::GetExpiredDate(const DateTime& acceptDate) const
				{

					if (ResetWeekField == 0) {
						return DateTime(XRated::Quest::NotUsedDate);
					}
					const uint32 MaxWeek = DateTime::Week::Saturday + 1;
					uint32 week = acceptDate.GetDate().GetDayOfWeek();

					for (uint32 i = 0; i < MaxWeek; ++i)
					{
						week = (week + 1) % MaxWeek;
						if ((0x01 << week) & ResetWeekField) {
							DateTime::Date returnDate(acceptDate.GetDate());
							returnDate.SetCumulatedDay(i, true, acceptDate.GetDate());
							return DateTime(returnDate, acceptDate.GetTime());
							//return true;
						}
					}
					return DateTime(XRated::Quest::NotUsedDate);
				}

				void QuestInfo::SetResetweek(const std::wstring& value, DateTime::Week::type day)
				{
					bool boolValue = false;
					if ((!value.compare(L"1")) || (!value.compare(L"true")) || (!value.compare(L"TRUE")))
					{
						boolValue = true;
					}
					else if ((!value.compare(L"0")) || (!value.compare(L"false")) || (!value.compare(L"FALSE")))
					{
						boolValue = false;
					}
					else
					{
						throw Exception(L"Invalid value in Day of Week. ");
					}

					uint32 temp = 0xff ^ (0x01 << day); //11011111 
					uint32 temp2 = ResetWeekField & temp;
					if (boolValue == false) ResetWeekField = temp2;
					else
					{
						ResetWeekField = temp2 | (0x01 << day);
					}
				}
				void QuestInfo::SetResetweek(bool sunday, bool monday, bool tuesday, bool wednesday, bool thursday, bool friday, bool saturday)
				{
					uint32 temp;
					temp = temp & 0x00;
					if (sunday == TRUE)
					{
						temp = temp | (0x01 << DateTime::Week::Sunday);
					}
					if (monday == TRUE)
					{
						temp = temp | (0x01 << DateTime::Week::Monday);
					}
					if (tuesday == TRUE)
					{
						temp = temp | (0x01 << DateTime::Week::Tuesday);
					}
					if (wednesday == TRUE)
					{
						temp = temp | (0x01 << DateTime::Week::Wednesday);
					}
					if (thursday == TRUE)
					{
						temp = temp | (0x01 << DateTime::Week::Thursday);
					}
					if (friday == TRUE)
					{
						temp = temp | (0x01 << DateTime::Week::Friday);
					}
					if (saturday == TRUE)
					{
						temp = temp | (0x01 << DateTime::Week::Saturday);
					}
					ResetWeekField = temp;
				}
				bool QuestInfo::IsResetday(DateTime::Week::type day) const
				{
					bool temp;
					if ((ResetWeekField & (0x01 << day))) { temp = true; }
					else { temp = false; }
					return temp;
				}
				void ClientQuestInfo::AcceptEvent::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::LuniaBase::Info::QuestInfo::AcceptEvent");
					out.Write(L"Type", Type);
					out.Write(L"Param_1", Param_1);
					out.Write(L"Param_2", Param_2);
					out.Write(L"Param_3", Param_3);
				}

				void ClientQuestInfo::AcceptEvent::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::LuniaBase::Info::QuestInfo::AcceptEvent");
					in.Read(L"Type", Type);
					in.Read(L"Param_1", Param_1);
					in.Read(L"Param_2", Param_2);
					in.Read(L"Param_3", Param_3);
				}

				void ClientQuestInfo::SuccessEvent::Serialize(Serializer::IStreamWriter& out) const///by kpongky( 09.07.09 ) BMS 6691
				{
					out.Begin(L"AllM::XRated::LuniaBase::Info::QuestInfo::SuccessEvent");
					out.Write(L"Type", Type);
					out.Write(L"Param_1", Param_1);
					out.Write(L"Param_2", Param_2);///by kpongky( 09.07.10 ) BMS 6691
				}

				void ClientQuestInfo::SuccessEvent::Deserialize(Serializer::IStreamReader& in)///by kpongky( 09.07.09 ) BMS 6691
				{
					in.Begin(L"AllM::XRated::LuniaBase::Info::QuestInfo::SuccessEvent");
					in.Read(L"Type", Type);
					in.Read(L"Param_1", Param_1);
					in.Read(L"Param_2", Param_2);///by kpongky( 09.07.10 ) BMS 6691
				}

				//////////////////////////////////////////////////////////////////////////////////////////////
				// ClientQuestInfo
				void ClientQuestInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::LuniaBase::Info::QuestInfo");
					out.Write(L"Type", Type);
					out.Write(L"ShortComment", ShortComment);
					out.Write(L"MainTopic", MainTopic);
					out.Write(L"Category", Category);
					out.Write(L"DisplayName", DisplayName);
					out.Write(L"BeginDescription", BeginDescription);
					out.Write(L"EndDescription", EndDescription);
					out.Write(L"ClearConditionDescription", ClearConditionDescription);
					out.Write(L"AcceptInfo", AcceptInfo);
					out.Write(L"FlashPath", FlashPath);
					out.Write(L"ShowNextQuest", ShowNextQuest);
					out.Write(L"ShowNoticeArrow", ShowNoticeArrow);
					out.Write(L"AutomaticAddQuestProgress", AutomaticAddQuestProgress);
					//out.Write(L"ShowAutomaticAddQuestProgressExplain", ShowAutomaticAddQuestProgressExplain);
					out.Write(L"ShowAddQuickSlotItemExplain", ShowAddQuickSlotItemExplain);
					out.Write(L"QuestAcceptEvent", QuestAcceptEvent);
					out.Write(L"QuestSuccessEvent", QuestSuccessEvent);
					out.Write(L"Tags", Tags); 
				}

				void ClientQuestInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::LuniaBase::Info::QuestInfo");
					in.Read(L"Type", Type);
					in.Read(L"ShortComment", ShortComment);
					in.Read(L"MainTopic", MainTopic);
					in.Read(L"Category", Category);
					in.Read(L"DisplayName", DisplayName);
					in.Read(L"BeginDescription", BeginDescription);
					in.Read(L"EndDescription", EndDescription);
					in.Read(L"ClearConditionDescription", ClearConditionDescription);
					in.Read(L"AcceptInfo", AcceptInfo, std::wstring());
					in.Read(L"FlashPath", FlashPath);
					in.Read(L"ShowNextQuest", ShowNextQuest, false);
					in.Read(L"ShowNoticeArrow", ShowNoticeArrow, false);
					in.Read(L"AutomaticAddQuestProgress", AutomaticAddQuestProgress, false);
					//in.Read(L"ShowAutomaticAddQuestProgressExplain", ShowAutomaticAddQuestProgressExplain, false);
					in.Read(L"ShowAddQuickSlotItemExplain", ShowAddQuickSlotItemExplain, false);
					in.Read(L"QuestAcceptEvent", QuestAcceptEvent, AcceptEvent());
					in.Read(L"QuestSuccessEvent", QuestSuccessEvent, SuccessEvent());
					in.Read(L"Tags", Tags, std::vector< std::wstring >());
				}
			}
		}
	}
}