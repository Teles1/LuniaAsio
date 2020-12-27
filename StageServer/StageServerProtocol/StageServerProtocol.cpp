#include "StageServerProtocol.h"
namespace Lunia {
	namespace StageServer {
		namespace Protocol {
            const wchar_t* Error::TypeName = L"Error";
            const HashType Error::TypeHash = StringUtil::Hash(Error::TypeName);
            void Error::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"errorcode", errorcode);
                out.Write(L"errorstring", errorstring);
            }
            void Error::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"errorcode", errorcode);
                in.Read(L"errorstring", errorstring);
            }

            const wchar_t* Way::TypeName = L"Way";
            const HashType Way::TypeHash = StringUtil::Hash(Way::TypeName);
            void Way::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"EncryptKey", EncryptKey);
            }
            void Way::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"EncryptKey", EncryptKey);
            }

            const wchar_t* Alive::TypeName = L"Alive";
            const HashType Alive::TypeHash = StringUtil::Hash(Alive::TypeName);
            void Alive::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"EncryptKey", EncryptKey);
                out.Write(L"RequestTime", RequestTime);
            }
            void Alive::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"EncryptKey", EncryptKey);
                in.Read(L"RequestTime", RequestTime);
            }

            const wchar_t* Stage::TypeName = L"Stage";
            const HashType Stage::TypeHash = StringUtil::Hash(Stage::TypeName);
            void Stage::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"charactername", charactername);
                out.Write(L"targetStage", targetStage);
            }
            void Stage::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"secukey", secukey);
                in.Read(L"Version", Version);
                in.Read(L"Locale", Locale);
            }

            const wchar_t* Granted::TypeName = L"Granted";
            const HashType Granted::TypeHash = StringUtil::Hash(Granted::TypeName);
            void Granted::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"charactername", charactername);
                out.Write(L"targetStage", targetStage);
            }

            void Granted::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"charactername", charactername);
                in.Read(L"targetStage", targetStage);
            }

            const wchar_t* CharacterInfo::TypeName = L"CharacterInfo";
            const HashType CharacterInfo::TypeHash = StringUtil::Hash(CharacterInfo::TypeName);
            void CharacterInfo::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.WriteEnum(L"classtype", classtype);
                out.Write(L"level", level);
                out.Write(L"xp", xp);
                out.Write(L"pvpLevel", pvpLevel);
                out.Write(L"pvpXp", pvpXp);
                out.Write(L"warLevel", warLevel);
                out.Write(L"warXp", warXp);
                out.Write(L"storedLevel", storedLevel);
                out.Write(L"rebirthCount", rebirthCount);
                out.Write(L"money", money);
                out.Write(L"bankMoney", bankMoney);
                out.Write(L"life", life);
                out.Write(L"skillpoint", skillpoint);
                out.Write(L"addedSkillPointPlus", addedSkillPointPlus);
                out.Write(L"storedSkillPoint", storedSkillPoint);
                out.Write(L"ladderPoint", ladderPoint);
                out.Write(L"ladderMatchCount", ladderMatchCount);
                out.Write(L"ladderWinCount", ladderWinCount);
                out.Write(L"ladderLoseCount", ladderLoseCount);
                out.Write(L"achievementScore", achievementScore);  // 3.1 by Robotex
                out.Write(L"IsSpectator", IsSpectator);
            }

            void CharacterInfo::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.ReadEnum(L"classtype", classtype);
                in.Read(L"level", level);
                in.Read(L"xp", xp);
                in.Read(L"pvpLevel", pvpLevel);
                in.Read(L"pvpXp", pvpXp);
                in.Read(L"warLevel", warLevel);
                in.Read(L"warXp", warXp);
                in.Read(L"storedLevel", storedLevel);
                in.Read(L"rebirthCount", rebirthCount);
                in.Read(L"money", money);
                in.Read(L"bankMoney", bankMoney);
                in.Read(L"life", life);
                in.Read(L"skillpoint", skillpoint);
                in.Read(L"addedSkillPointPlus", addedSkillPointPlus);
                in.Read(L"storedSkillPoint", storedSkillPoint);
                in.Read(L"ladderPoint", ladderPoint);
                in.Read(L"ladderMatchCount", ladderMatchCount);
                in.Read(L"ladderWinCount", ladderWinCount);
                in.Read(L"ladderLoseCount", ladderLoseCount);
                in.Read(L"achievementScore", achievementScore);
                in.Read(L"IsSpectator", IsSpectator);
            }

            const wchar_t* ListSkillLicenses::TypeName = L"ListSkillLicenses";
            const HashType ListSkillLicenses::TypeHash = StringUtil::Hash(ListSkillLicenses::TypeName);
            void ListSkillLicenses::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"listSkillLicenses", listSkillLicenses);
            }

            void ListSkillLicenses::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"listSkillLicenses", listSkillLicenses);
            }

            const wchar_t* ListSkill::TypeName = L"ListSkill";
            const HashType ListSkill::TypeHash = StringUtil::Hash(ListSkill::TypeName);
            void ListSkill::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"listskill", listskill);
            };

            void ListSkill::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"listskill", listskill);
            }

            const wchar_t* ListQuickSlot::TypeName = L"ListQuickSlot";
            const HashType ListQuickSlot::TypeHash = StringUtil::Hash(ListQuickSlot::TypeName);
            void ListQuickSlot::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"quickslotlist", quickslotlist);
            }

            void ListQuickSlot::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
            }

            const wchar_t* ListItem::TypeName = L"ListItem";
            const HashType ListItem::TypeHash = StringUtil::Hash(ListItem::TypeName);
            void ListItem::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"listitem", listitem);
            };

            void ListItem::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
            }

            const wchar_t* ListPetItem::TypeName = L"ListPetItem";
            const HashType ListPetItem::TypeHash = StringUtil::Hash(ListPetItem::TypeName);
            void ListPetItem::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"PetsItems", PetsItems);
            }

            void ListPetItem::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"PetsItems", PetsItems);
            }

            const wchar_t* PetInfo::TypeName = L"PetInfo";
            const HashType PetInfo::TypeHash =
                StringUtil::Hash(PetInfo::TypeName);
            void PetInfo::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"PetDataWithPos", PetDataWithPos);
            }

            void PetInfo::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"PetDataWithPos", PetDataWithPos);
            }

            const wchar_t* Join::TypeName = L"Join";
            const HashType Join::TypeHash = StringUtil::Hash(Join::TypeName);
            void Join::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
            }

            void Join::Deserialize(Serializer::IStreamReader& in) { 
                in.Begin(TypeName); 
            }

            const wchar_t* PetsCaredBySchool::TypeName = L"PetsCaredBySchool";
            const HashType PetsCaredBySchool::TypeHash = StringUtil::Hash(PetsCaredBySchool::TypeName);
            void PetsCaredBySchool::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"OwnerSerial", OwnerSerial);
                out.Write(L"CaredPets", CaredPets);
            }

            void PetsCaredBySchool::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"OwnerSerial", OwnerSerial);
                in.Read(L"CaredPets", CaredPets);
            }

            const wchar_t* LoadEnd::TypeName = L"LoadEnd";
            const HashType LoadEnd::TypeHash = StringUtil::Hash(LoadEnd::TypeName);
            void LoadEnd::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"charName", charName);
                out.Write(L"progress", progress);
            };

            void LoadEnd::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"charName", charName);
                in.Read(L"progress", progress);
            }

            const wchar_t* BagStates::TypeName = L"BagStates";
            const HashType BagStates::TypeHash = StringUtil::Hash(BagStates::TypeName);
            void BagStates::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"Bags", Bags);
                out.Write(L"BankBags", BankBags);
            }

            void BagStates::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"Bags", Bags);
                in.Read(L"BankBags", BankBags);
            }

            void CreatePlayer::Equipment::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(L"CreatePlayer::Equipment");
                out.Write(L"Position", Position);
                out.Write(L"ItemHash", ItemHash);
                out.Write(L"instanceEx", instanceEx);
            }

            void CreatePlayer::Equipment::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(L"CreatePlayer::Equipment");
                in.Read(L"Position", Position);
                in.Read(L"ItemHash", ItemHash);
                in.Read(L"instanceEx", instanceEx);
            }

            const wchar_t* CreatePlayer::TypeName = L"CreatePlayer";
            const HashType CreatePlayer::TypeHash = StringUtil::Hash(CreatePlayer::TypeName);
            void CreatePlayer::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"playerserial", playerserial);
                out.WriteEnum(L"classtype", classtype);
                out.Write(L"charactername", charactername);
                out.Write(L"level", level);
                out.Write(L"pvpLevel", pvpLevel);
                out.Write(L"warLevel", warLevel);
                out.Write(L"storedLevel", storedLevel);
                out.Write(L"rebirthCount", rebirthCount);
                out.Write(L"ladderPoint", ladderPoint);
                out.Write(L"ladderMatchCount", ladderMatchCount);
                out.Write(L"ladderWinCount", ladderWinCount);
                out.Write(L"ladderLoseCount", ladderLoseCount);
                out.Write(L"achievementScore", achievementScore);
                out.Write(L"addedSkillPointByRebirth", addedSkillPointByRebirth);
                out.Write(L"position", position);
                out.Write(L"direction", direction);
                out.Write(L"hp", hp);
                out.Write(L"team", team);
                out.Write(L"Equipments", Equipments);
                out.Write(L"PassiveItems", PassiveItems);
                out.Write(L"stateflags", stateflags);
                out.Write(L"shopping", shopping);
                out.Write(L"stageLicenses", stageLicenses);
                out.Write(L"lives", lives);
                out.Write(L"bonusLife", bonusLife);
                out.Write(L"CharacterStateFlags", static_cast<int>(CharacterStateFlags));
                out.Write(L"lastRebirthDateTime", lastRebirthDateTime);
                out.Write(L"partyChannelName", partyChannelName);
                out.Write(L"eventExpFactor", eventExpFactor);
            }

            void CreatePlayer::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"playerserial", playerserial);
                in.ReadEnum(L"classtype", classtype);
                in.Read(L"charactername", charactername);
                in.Read(L"level", level);
                in.Read(L"pvpLevel", pvpLevel);
                in.Read(L"warLevel", warLevel);
                in.Read(L"storedLevel", storedLevel);
                in.Read(L"rebirthCount", rebirthCount);
                in.Read(L"ladderPoint", ladderPoint);
                in.Read(L"ladderMatchCount", ladderMatchCount);
                in.Read(L"ladderWinCount", ladderWinCount);
                in.Read(L"ladderLoseCount", ladderLoseCount);
                in.Read(L"achievementScore", achievementScore);
                in.Read(L"addedSkillPointByRebirth", addedSkillPointByRebirth);
                in.Read(L"position", position);
                in.Read(L"direction", direction);
                in.Read(L"hp", hp);
                in.Read(L"team", team);
                in.Read(L"Equipments", Equipments);
                in.Read(L"PassiveItems", PassiveItems);
                in.Read(L"stateflags", stateflags);
                in.Read(L"shopping", shopping);
                in.Read(L"stageLicenses", stageLicenses);
                in.Read(L"lives", lives);
                in.Read(L"bonusLife", bonusLife);
                in.Read(L"CharacterStateFlags",
                    reinterpret_cast<int&>(CharacterStateFlags));
                in.Read(L"lastRebirthDateTime", lastRebirthDateTime);
                in.Read(L"partyChannelName", partyChannelName);
                in.Read(L"eventExpFactor", eventExpFactor);
            }

            const wchar_t* Quest::WorkingList::TypeName = L"Quest::WorkingList";
            const HashType Quest::WorkingList::TypeHash = StringUtil::Hash(Quest::WorkingList::TypeName);
            void Quest::WorkingList::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"Quests", Quests);
            };

            void Quest::WorkingList::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"Quests", Quests);
            }

            const wchar_t* Family::Info::TypeName = L"Family::Info";
            const HashType Family::Info::TypeHash = StringUtil::Hash(Family::Info::TypeName);
            void Family::Info::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"FamilyInfo", FamilyInfo);
                out.Write(L"Condition", Condition);
                out.Write(L"Members", Members);
            }

            void Family::Info::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"FamilyInfo", FamilyInfo);
                in.Read(L"Condition", Condition);
                in.Read(L"Members", Members);
            }
		}
	}
}