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
                in.Read(L"quickslotlist", quickslotlist);
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
		}
	}
}