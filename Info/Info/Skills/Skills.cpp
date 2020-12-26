#pragma once
#include "Skills.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void SkillInfoList::SkillInfo::PrecedingSkill::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::SkillInfoList::SkillInfo::PrecedingSkill");
					out.Write(L"GroupName", GroupName);
					out.Write(L"Lv", Lv);
				}

				void SkillInfoList::SkillInfo::PrecedingSkill::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::SkillInfoList::SkillInfo::PrecedingSkill");
					in.Read(L"GroupName", GroupName);
					in.Read(L"Lv", Lv);
					Hash = StringUtil::Hash(GroupName.c_str());
				}

				void SkillInfoList::SkillInfo::Reagent::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::SkillInfoList::SkillInfo::Reagent");
					out.Write(L"Name", Name);
					out.Write(L"Cnt", Cnt);
				}

				void SkillInfoList::SkillInfo::Reagent::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::SkillInfoList::SkillInfo::Reagent");
					in.Read(L"Name", Name);
					in.Read(L"Cnt", Cnt);
					Hash = StringUtil::Hash(Name.c_str());
				}

				void SkillInfoList::SkillInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::SkillInfoList::SkillInfo");
					out.Write(L"Name", Name);
					//out.Write(L"Category", Category);
					out.Write(L"ActionName", ActionName);
					out.Write(L"Cooldown", Cooldown);
					out.Write(L"Level", LevelLimit);
					out.Write(L"RebirthCount", RebirthCountLimit);
					out.Write(L"StoredLevel", StoredLevelLimit);
					out.Write(L"PrecedingSkills", PrecedingSkills);
					out.Write(L"PrecedingItems", PrecedingItems);
					out.Write(L"Mp", Mp);
					out.Write(L"Hp", Hp);
					out.Write(L"Reagents", Reagents);
					out.Write(L"Description", Description);
				}

				void SkillInfoList::SkillInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::SkillInfoList::SkillInfo");
					in.Read(L"Name", Name);
					Hash = StringUtil::Hash(Name.c_str());
					//in.Read(L"Category", Category);
					in.Read(L"ActionName", ActionName);
					ActionHash = StringUtil::Hash(ActionName.c_str());
					in.Read(L"Cooldown", Cooldown);
					in.Read(L"Level", LevelLimit);
					in.Read(L"RebirthCount", RebirthCountLimit, static_cast<uint16>(0));
					in.Read(L"StoredLevel", StoredLevelLimit, static_cast<uint16>(0));
					in.Read(L"PrecedingSkills", PrecedingSkills, std::vector<PrecedingSkill>());
					in.Read(L"PrecedingItems", PrecedingItems, std::vector<uint32>());
					in.Read(L"Mp", Mp, 0.0f);
					in.Read(L"Hp", Hp, 0.0f);
					in.Read(L"Reagents", Reagents, std::vector<Reagent>());
					in.Read(L"Description", Description);
				}

				void SkillInfoList::SkillGroup::Skill::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::SkillInfoList::SkillGroup::Skill");
					out.Write(L"Name", Name);
				}

				void SkillInfoList::SkillGroup::Skill::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::SkillInfoList::SkillGroup::Skill");
					in.Read(L"Name", Name);
					Hash = StringUtil::Hash(Name.c_str());
				}

				void SkillInfoList::SkillGroup::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::SkillInfoList::SkillGroup");
					out.WriteEnum(L"SKillType", SKillType);
					out.Write(L"Name", Name);
					out.Write(L"Category", Category);

					//by kwind
					out.Write(L"DisplayName", DisplayName);
					out.Write(L"Property", Property);

					out.Write(L"CommonerMaxLv", CommonerMaxLv);
					out.Write(L"ShowWhenFirstSkillInfoCondition", ShowWhenFirstSkillInfoCondition);

					out.Write(L"NeedLicense", NeedLicense);

					out.Write(L"CoolTimeDecreaseable", CoolTimeDecreaseable); /* 11-2-2019 9:14 @Oracle */
					out.Write(L"CoolTimeResetAble", CoolTimeResetAble); 	 /* 11-2-2019 9:14 @Oracle */

					out.Write(L"PossibleWhenHit", PossibleWhenHit);
					out.Write(L"Skills", Skills);

				}

				void SkillInfoList::SkillGroup::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::SkillInfoList::SkillGroup");
					in.ReadEnum(L"SKillType", SKillType, Type::Attack);
					in.Read(L"Name", Name);
					in.Read(L"Category", Category);

					//by kwind
					in.Read(L"DisplayName", DisplayName, std::wstring(L""));
					in.Read(L"Property", Property, std::wstring(L""));

					in.Read(L"CommonerMaxLv", CommonerMaxLv, (uint8)0);
					in.Read(L"ShowWhenFirstSkillInfoCondition", ShowWhenFirstSkillInfoCondition, false);

					in.Read(L"NeedLicense", NeedLicense, false);

					in.Read(L"CoolTimeDecreaseable", CoolTimeDecreaseable, true); /* 11-2-2019 9:14 @Oracle */
					in.Read(L"CoolTimeResetAble", CoolTimeResetAble, true);       /* 11-2-2019 9:14 @Oracle */

					Hash = StringUtil::Hash(Name.c_str());
					HashCategory = StringUtil::Hash(Category.c_str());
					in.Read(L"PossibleWhenHit", PossibleWhenHit, std::vector<uint32>());
					in.Read(L"Skills", Skills);

					maxLv = (uint8)Skills.size();
					if (!CommonerMaxLv) CommonerMaxLv = maxLv;
				}

				void SkillInfoList::SkillGroup::SetSkillInfo(SkillInfoList* l)
				{
					for (std::vector<Skill>::iterator i = Skills.begin(); i != Skills.end(); ++i)
						(*i).Info = l->GetSkillInfo((*i).Name);
				}

				SkillInfoList::SkillInfo* SkillInfoList::SkillGroup::GetSkillInfo(const std::wstring& name)
				{
					SkillInfo* s = GetSkillInfo(StringUtil::Hash(name.c_str()));
					if (!s) {
						Logger::GetInstance().Warn(L"[SkillInfoList::SkillGroup::GetSkillInfo] unable to find Skill [{0}] information", name.c_str());
					}
					return s;
				}

				SkillInfoList::SkillInfo* SkillInfoList::SkillGroup::GetSkillInfo(uint32 h)
				{
					for (std::vector<Skill>::iterator i = Skills.begin(); i != Skills.end(); ++i)
						if ((*i).Hash == h)
							return (*i).Info;
					return NULL;
				}

				SkillInfoList::SkillInfo* SkillInfoList::SkillGroup::GetSkillInfo(uint8 lv)
				{
					if (lv < 1 || lv > Skills.size())
						return NULL;
					return Skills[lv - 1].Info;
				}

				bool SkillInfoList::SkillGroup::IsThisGroup(const std::wstring& name) const
				{
					std::vector<Skill>::const_iterator cIter = Skills.begin();
					for (; cIter != Skills.end(); ++cIter) {
						if ((*cIter).Name == name)
							return true;
					}
					return false;
				}

				bool SkillInfoList::SkillGroup::IsPossibleAction(uint32 hash) const
				{
					if (PossibleWhenHit.empty())
						return false;

					std::vector<uint32>::const_iterator i = PossibleWhenHit.begin();
					std::vector<uint32>::const_iterator iEnd = PossibleWhenHit.end();
					for (; i != iEnd; ++i) {
						if ((*i) == hash)
							return true;
					}
					return false;
				}

				bool SkillInfoList::SkillGroup::IsLearn(uint32 playerLevel, uint16 rebirthCount, uint16 storedLevel) const
				{
					if (Skills.empty()) return false;
					if (!ShowWhenFirstSkillInfoCondition) return true;

					if (playerLevel < Skills[0].Info->LevelLimit) return false;
					if (rebirthCount < Skills[0].Info->RebirthCountLimit) return false;
					if (storedLevel < Skills[0].Info->StoredLevelLimit) return false;

					return true;
				}

				void SkillInfoList::SetSkillInfo()
				{
					//Set Skill info's pointer to skillgroup
					for (SkillGroupList::iterator i = SkillGroups.begin(); i != SkillGroups.end(); ++i)
						(*i).SetSkillInfo(this);
				}

				void SkillInfoList::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::SkillInfoList");
					out.Write(L"Name", Name);
					out.Write(L"Skills", Skills);
					out.Write(L"PvPSkills", PvPSkills);
					out.Write(L"SkillGroups", SkillGroups);
				}

				void SkillInfoList::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::SkillInfoList");
					in.Read(L"Name", Name);
					in.Read(L"Skills", Skills);
					in.Read(L"PvPSkills", PvPSkills);
					in.Read(L"SkillGroups", SkillGroups);

					Hash = StringUtil::Hash(Name.c_str());
				}

				SkillInfoList::SkillInfo* SkillInfoList::GetSkillInfo(const std::wstring& name)
				{
					SkillInfo* returnValue = GetSkillInfo(StringUtil::Hash(name.c_str()));
					if (!returnValue) {
						Logger::GetInstance().Warn(L"[SkillInfoList::GetSkillInfo] unable to find Skill [{0}] information", name.c_str());
						//assert(returnValue);
					}
					return returnValue;
				}

				SkillInfoList::SkillInfo* SkillInfoList::GetSkillInfo(uint32 h)
				{
					for (SkillList::iterator i = Skills.begin(); i != Skills.end(); ++i) {
						if ((*i).Hash == h)
							return &(*i);
					}
					return NULL;
				}

				SkillInfoList::SkillGroup* SkillInfoList::GetSkillGroup(const std::wstring& name)
				{
					SkillGroup* returnValue = GetSkillGroup(StringUtil::Hash(name.c_str()));
					if (!returnValue) {
						Logger::GetInstance().Warn(L"[SkillInfoList::GetSkillGroup] unable to find Skill [{0}] information", name.c_str());
						//assert(returnValue);
					}
					return returnValue;
				}

				SkillInfoList::SkillGroup* SkillInfoList::GetSkillGroup(uint32 h)
				{
					for (SkillGroupList::iterator i = SkillGroups.begin(); i != SkillGroups.end(); ++i) {
						if ((*i).Hash == h)
							return &(*i);
					}
					return NULL;
				}

				SkillInfoList::SkillGroupList* SkillInfoList::GetSkillGroupList()
				{
					return &SkillGroups;
				}

				SkillCategoryList::SkillCategoryInfo::SkillCategoryInfo(const wchar_t* n, float cd, unsigned short o)
					: Name(n), Cooldown(cd), Order(o), Hash(StringUtil::Hash(n))
				{
				}

				void SkillCategoryList::SkillCategoryInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::SkillCategoryList::SkillCategoryInfo");
					out.Write(L"Name", Name);
					out.Write(L"Cooldown", Cooldown);
					out.Write(L"Order", Order);
				}

				void SkillCategoryList::SkillCategoryInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::SkillCategoryList::SkillCategoryInfo");
					in.Read(L"Name", Name);
					in.Read(L"Cooldown", Cooldown);
					in.Read(L"Order", Order);
					Hash = StringUtil::Hash(Name.c_str());
				}

				void SkillCategoryList::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::SkillCategoryList");
					out.Write(L"Name", Name);
					out.Write(L"Categorys", Categorys);
				}

				void SkillCategoryList::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::SkillCategoryList");
					in.Read(L"Name", Name);
					in.Read(L"Categorys", Categorys);
					Hash = StringUtil::Hash(Name.c_str());
				}
			}
		}
	}
}