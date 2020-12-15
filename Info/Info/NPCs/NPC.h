#pragma once
#include "../Actions/Actions.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct NonPlayerInfo : public Serializer::ISerializable
				{
					enum Races {
						MONSTER = 0, ANIMAL, HUMAN, DEMON, UNDEAD, PLANT,
						GIANT, FAIRY, DRAGON, INSTRUMENT, SUMMONED,
					};

					struct NPCType {
						enum ClassType {
							Default = 0/*Global AI type*/, Warrior = 1, Thrower = 2, Catapult = 3, Tower = 4, Boss = 5
							, DnD = 6, Wolf = 7, WolfBoss = 8, Mimic = 9, Checker = 10/*NPC Action validation check*/
							, SlimeBoss = 11, StoneGolem = 12, BradFord = 13, PlayerMorph = 14
						};
					};

					std::wstring Name, TemplateName, ActionListName;
					uint32 HashcodeName, HashcodeTemplateName, HashcodeActionListName;
					NPCType::ClassType Class;
					Races Race;
					NonPlayerData::NpcType NpcType;
					struct Item : public Serializer::ISerializable {
						float Probability;
						std::wstring Name;
						uint32 Hash;
					public: /* ISerializable implementation */
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					std::vector<Item> Items;
					std::vector<std::wstring> UnidentifiedItems;
					std::vector<uint32> UnidentifiedItemsHash;
					unsigned int Lv;
					std::vector<int> Resists;
					uint32 Exp;
					float Radius;
					float3 Scale;
					std::wstring StartAction;
					uint32 StartActionHash;
					std::wstring BasicStat;
					std::wstring Element; 
					std::vector<ActionInfo::State> States;
					std::wstring Description;
					bool UseGlow;
					float4 GlowColor;	
					bool GiveItemToAllPlayer;
					//AI specific
					float Sight, TribeSight;	
					float AttackRange;	
					bool IgnoreHate; 
					bool RecognizeableByEnemy; // 3.1 by ycr4zy 04-07-2020;
					struct OffensiveSkillList : public Serializer::ISerializable {
						float AggressiveMode; 
						struct SkillList : public Serializer::ISerializable {
							struct Skill : public Serializer::ISerializable {
								std::wstring SkillName;
								float Probability;

								uint32 Hash;
							public: /* ISerializable implementation */
								virtual void Serialize(Serializer::IStreamWriter& out) const;
								virtual void Deserialize(Serializer::IStreamReader& in);
							};
							float Range;
							std::vector< Skill > Skills;
						public: /* ISerializable implementation */
							virtual void Serialize(Serializer::IStreamWriter& out) const;
							virtual void Deserialize(Serializer::IStreamReader& in);
						};
						std::vector< SkillList > NormalSkills;
						std::vector< SkillList > AggressiveSkills;
					public: /* ISerializable implementation */
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					} OffensiveSkills;

					//Local Specific
					std::wstring DisplayName;
					std::wstring AnimationPath;
					std::wstring EffectCategory;
					struct Jewel : public Serializer::ISerializable {
						uint8 Type;
						uint16 Cnt;
					public: /* ISerializable implementation */
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					std::vector<Jewel> Jewels;
					std::vector<std::wstring> DefaultMeshs;
				public:
					virtual ~NonPlayerInfo() {
					}

					bool CheckDropItemDuplication() const;
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}