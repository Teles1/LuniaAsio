#pragma once
#include "NPC.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void NonPlayerInfo::Jewel::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::NonPlayerInfo::Jewel");
					out.Write(L"Type", (uint16)Type);
					out.Write(L"Cnt", Cnt);
				}

				void NonPlayerInfo::Jewel::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::NonPlayerInfo::Jewel");
					uint16 jewelType;
					in.Read(L"Type", jewelType);
					Type = (uint8)jewelType;
					in.Read(L"Cnt", Cnt);
				}

				void NonPlayerInfo::Item::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::NonPlayerInfo::Item");
					out.Write(L"Probability", Probability);
					out.Write(L"Name", Name);
				}

				void NonPlayerInfo::Item::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::NonPlayerInfo::Item");
					in.Read(L"Probability", Probability);
					in.Read(L"Name", Name);
					Hash = StringUtil::Hash(Name.c_str());
				}

				void NonPlayerInfo::OffensiveSkillList::SkillList::Skill::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::NonPlayerInfo::OffensiveSkillList::SkillList::Skill");
					out.Write(L"SkillName", SkillName);
					out.Write(L"Probability", Probability);
				}

				void NonPlayerInfo::OffensiveSkillList::SkillList::Skill::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::NonPlayerInfo::OffensiveSkillList::SkillList::Skill");
					in.Read(L"SkillName", SkillName, std::wstring());
					in.Read(L"Probability", Probability, 0.0f);
					Hash = StringUtil::Hash(SkillName.c_str());
				}

				void NonPlayerInfo::OffensiveSkillList::SkillList::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::NonPlayerInfo::OffensiveSkillList::SkillList");
					out.Write(L"Range", Range);
					out.Write(L"Skills", Skills);
				}

				void NonPlayerInfo::OffensiveSkillList::SkillList::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::NonPlayerInfo::OffensiveSkillList::SkillList");
					in.Read(L"Range", Range, 0.0f);
					in.Read(L"Skills", Skills, std::vector< Skill >());
				}

				void NonPlayerInfo::OffensiveSkillList::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::NonPlayerInfo::OffensiveSkillList");
					out.Write(L"AggressiveMode", AggressiveMode);
					out.Write(L"NormalSkills", NormalSkills);
					out.Write(L"AggressiveSkills", AggressiveSkills);
				}

				void NonPlayerInfo::OffensiveSkillList::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::NonPlayerInfo::OffensiveSkillList");
					in.Read(L"AggressiveMode", AggressiveMode, 0.0f);
					in.Read(L"NormalSkills", NormalSkills, std::vector< SkillList >());
					in.Read(L"AggressiveSkills", AggressiveSkills, std::vector< SkillList >());
				}


				void NonPlayerInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::NonPlayerInfo");
					out.Write(L"Name", Name);
					out.Write(L"TemplateName", TemplateName);
					out.Write(L"ActionListName", ActionListName);
					out.Write(L"Class", (int)Class);
					out.Write(L"DisplayName", DisplayName);
					out.Write(L"Race", (int)Race);
					out.Write(L"NpcType", (int)NpcType);
					out.Write(L"Items", Items);
					out.Write(L"UnidentifiedItems", UnidentifiedItems);
					out.Write(L"Jewels", Jewels);
					out.Write(L"Lv", Lv);
					out.Write(L"Exp", Exp);
					out.Write(L"Radius", Radius);
					out.Write(L"Sight", Sight);
					out.Write(L"TribeSight", TribeSight);
					out.Write(L"AttackRange", AttackRange);
					out.Write(L"IgnoreHate", IgnoreHate);
					out.Write(L"RecognizeableByEnemy", RecognizeableByEnemy);
					out.Write(L"OffensiveSkills", OffensiveSkills);
					out.Write(L"Scale", Scale);
					out.Write(L"StartAction", StartAction);
					out.Write(L"BasicStat", BasicStat);
					out.Write(L"Element", Element);
					out.Write(L"AnimationPath", AnimationPath);
					out.Write(L"EffectCategory", EffectCategory);
					out.Write(L"Resists", Resists);
					out.Write(L"States", States);
					out.Write(L"Description", Description);
					out.Write(L"UseGlow", UseGlow);
					out.Write(L"GlowColor", GlowColor);
					out.Write(L"DefaultMeshs", DefaultMeshs);
					out.Write(L"GiveItemToAllPlayer", GiveItemToAllPlayer);
				}


				const wchar_t* GetNpcTypeString(XRated::NonPlayerData::NpcType npcType)
				{
					using namespace XRated;

					//Normal=0, Leader, Elite, Rare, SemiBoss, StageBoss, EpisodeBoss
					switch (npcType)
					{
					case NonPlayerData::NpcType::Normal:
						return L"Normal";
					case NonPlayerData::NpcType::Leader:
						return L"Leader";
					case NonPlayerData::NpcType::Elite:
						return L"Elite";
					case NonPlayerData::NpcType::Rare:
						return L"Rare";
					case NonPlayerData::NpcType::SemiBoss:
						return L"SemiBoss";
					case NonPlayerData::NpcType::StageBoss:
						return L"StageBoss";
					case NonPlayerData::NpcType::EpisodeBoss:
						return L"EpisodeBoss";
					default:
						Logger::GetInstance().Error(L"GetNpcTypeString() - Invalid NpcType: {0}", npcType);
					}
					return L"##NPCTypeNotFound##";
				}


				int  GetNpcType(const std::wstring& basicStat)
				{
					static bool isLoaded = false;
					static std::map< std::wstring, XRated::NonPlayerData::NpcType> strTable;
					if (!isLoaded)
					{
						strTable.clear();
						for (int i = 0; i < XRated::NonPlayerData::NpcType::Cnt; ++i)
						{
							std::wstring str = GetNpcTypeString(static_cast<XRated::NonPlayerData::NpcType>(i));
							strTable[str] = static_cast<XRated::NonPlayerData::NpcType>(i);
						}
						isLoaded = true;
					}

					std::map< std::wstring, XRated::NonPlayerData::NpcType>::const_iterator it = strTable.begin();

					for (; it != strTable.end(); ++it)
					{
						if (basicStat.substr(0, it->first.size()) == it->first)
						{
							return it->second;
						}
					}
					Logger::GetInstance().Error(L"Cannot Find NpcType by BasicStat {0}", basicStat.c_str());
					return -1;
				}

				void NonPlayerInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::NonPlayerInfo");
					in.Read(L"Name", Name);
					HashcodeName = StringUtil::Hash(Name.c_str());
					in.Read(L"TemplateName", TemplateName);
					HashcodeTemplateName = StringUtil::Hash(TemplateName.c_str());
					in.Read(L"ActionListName", ActionListName);
					HashcodeActionListName = StringUtil::Hash(ActionListName.c_str());
					int c;
					in.Read(L"Class", c, 0);
					Class = (NPCType::ClassType)c;
					in.Read(L"DisplayName", DisplayName);
					in.Read(L"Race", (int&)Race);
					in.Read(L"NpcType", (int&)NpcType);
					in.Read(L"Items", Items);
					in.Read(L"UnidentifiedItems", UnidentifiedItems);
					std::vector<std::wstring>::iterator i = UnidentifiedItems.begin();
					while (i != UnidentifiedItems.end()) {
						UnidentifiedItemsHash.push_back(StringUtil::Hash((*i).c_str()));
						++i;
					}
					in.Read(L"Jewels", Jewels);
					in.Read(L"Lv", Lv);
					in.Read(L"Exp", Exp);
					in.Read(L"Radius", Radius);
					in.Read(L"Sight", Sight);
					in.Read(L"TribeSight", TribeSight);
					in.Read(L"AttackRange", AttackRange);
					in.Read(L"IgnoreHate", IgnoreHate, false);
					in.Read(L"RecognizeableByEnemy", RecognizeableByEnemy, false);
					in.Read(L"OffensiveSkills", OffensiveSkills);
					in.Read(L"Scale", Scale);
					in.Read(L"StartAction", StartAction, std::wstring());
					if (StartAction.empty()) StartAction = L"Stand";
					StartActionHash = StringUtil::Hash(StartAction.c_str());
					in.Read(L"BasicStat", BasicStat);
					in.Read(L"Element", Element, std::wstring());
					if (Element.empty()) Element = L"PHYSICAL";
					XRated::Constants::StringToDmgType(Element);

					in.Read(L"AnimationPath", AnimationPath);
					in.Read(L"EffectCategory", EffectCategory);
					in.Read(L"SpecialDead", SpecialDead); // new missing logic
					in.Read(L"Resists", Resists);
					in.Read(L"States", States);
					in.Read(L"Description", Description, std::wstring());
					in.Read(L"UseGlow", UseGlow, false);
					in.Read(L"GlowColor", GlowColor, float4(1.0f, 1.0f, 1.0f, 1.0f));
					in.Read(L"DefaultMeshs", DefaultMeshs, std::vector<std::wstring>());
					in.Read(L"GiveItemToAllPlayer", GiveItemToAllPlayer, false);

					std::wcout << Name << std::endl;
				}

				bool NonPlayerInfo::CheckDropItemDuplication() const
				{
					bool result = true;
					/* check duplicated dropitem name*/
					std::map< std::wstring, const NonPlayerInfo::Item*> dropitems;
					for (size_t i = 0; i < Items.size(); ++i)
					{
						std::map< std::wstring, const NonPlayerInfo::Item*>::const_iterator it = dropitems.find(Items[i].Name);
						if (it != dropitems.end())
						{
							Logger::GetInstance().Error(L"Npc[{0}] has Duplicated DropItem[{1}/Probablity:{2}]", Name.c_str(), it->second->Name.c_str(), it->second->Probability);
							Logger::GetInstance().Error(L"Npc[{0}] has Duplicated DropItem[{1}/Probablity:{2}]", Name.c_str(), Items[i].Name.c_str(), Items[i].Probability);
							result = false;
						}
						dropitems[Items[i].Name] = &Items[i];
					}
					return result;
				}

			}
		}
	}
}