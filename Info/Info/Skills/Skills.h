#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct SkillInfoList : public Serializer::ISerializable
				{
				public:
					struct SkillInfo : public Serializer::ISerializable
					{
						std::wstring Name;
						uint32 Hash;
						
						std::wstring ActionName;
						uint32 ActionHash;
						float Cooldown;
						uint16 LevelLimit;
						uint16 RebirthCountLimit;
						uint16 StoredLevelLimit;
						struct PrecedingSkill : public Serializer::ISerializable {
							std::wstring GroupName;
							uint32 Hash;
							int Lv;
						public:
							virtual void Serialize(Serializer::IStreamWriter& out) const;
							virtual void Deserialize(Serializer::IStreamReader& in);
						};
						std::vector<PrecedingSkill> PrecedingSkills;
						std::vector<uint32> PrecedingItems;
						float Hp, Mp; 
						struct Reagent : public Serializer::ISerializable {
							std::wstring Name;
							uint32 Hash;
							int Cnt;

							
						public:
							virtual void Serialize(Serializer::IStreamWriter& out) const;
							virtual void Deserialize(Serializer::IStreamReader& in);
						};
						std::vector<Reagent> Reagents; 
						std::wstring Description;

					public:
						SkillInfo() {}
						SkillInfo(const wchar_t* n, const wchar_t* a, float cd) : Name(n), ActionName(a), Cooldown(cd) {}
						SkillInfo& operator= (const SkillInfo& s) {
							this->Name = s.Name;
							this->Hash = s.Hash;
							
							this->ActionName = s.ActionName;
							this->ActionHash = s.ActionHash;
							this->Cooldown = s.Cooldown;
							this->LevelLimit = s.LevelLimit;
							this->PrecedingSkills = s.PrecedingSkills;
							this->Hp = s.Hp;
							this->Mp = s.Mp;
							this->Reagents = s.Reagents;
							this->Description = s.Description;
							return *this;
						}

					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					typedef std::vector<SkillInfo> SkillList;

					struct SkillGroup : public Serializer::ISerializable {
						enum Type
						{
							Attack = 0,
							Produce,
						};

						Type SKillType;
						std::wstring Name;
						std::wstring Category;			

						uint32 Hash;
						uint32 HashCategory;
						
						std::wstring DisplayName;		
						std::wstring Property;			

						uint8 CommonerMaxLv;			
						bool  ShowWhenFirstSkillInfoCondition;

						bool NeedLicense;
						bool CoolTimeDecreaseable; 
						bool CoolTimeResetAble; 

						std::vector<uint32> PossibleWhenHit;
						struct Skill : public Serializer::ISerializable {
							std::wstring Name;
							uint32 Hash;
							SkillInfo* Info;

						public:
							virtual void Serialize(Serializer::IStreamWriter& out) const;
							virtual void Deserialize(Serializer::IStreamReader& in);
						};
						std::vector<Skill> Skills;

					public: 
						uint8 maxLv;					

					public:
						void SetSkillInfo(SkillInfoList* l);
						SkillInfo* GetSkillInfo(const std::wstring& name);
						SkillInfo* GetSkillInfo(uint32 h);
						SkillInfo* GetSkillInfo(uint8 lv);
						uint8 GetMaxLevel() const { return maxLv; }
						bool GetCoolTimeDecreaseable() const { return CoolTimeDecreaseable; } 
						bool GetCoolTimeResetAble() const { return CoolTimeResetAble; } 
						uint8 GetCommonerMaxLv() const { return CommonerMaxLv; }
						bool IsThisGroup(const std::wstring& name) const;
						bool IsPossibleAction(uint32 hash) const;
						bool IsLearn(uint32 playerLevel, uint16 rebirthCount, uint16 storedLevel) const;

					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					typedef std::vector<SkillGroup> SkillGroupList;

				public:
					std::wstring Name;
					uint32 Hash;
					SkillList Skills;
					SkillList PvPSkills;
					SkillGroupList SkillGroups;

					void SetSkillInfo();

				public: 
					SkillGroup* GetSkillGroup(const std::wstring& name);
					SkillGroup* GetSkillGroup(uint32 h);
					SkillInfo* GetSkillInfo(const std::wstring& name);
					SkillInfo* GetSkillInfo(uint32 h);

					SkillGroupList* GetSkillGroupList();

				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
				struct SkillCategoryList : public Serializer::ISerializable {
				public:
					struct SkillCategoryInfo : public Serializer::ISerializable
					{
						std::wstring Name;
						float Cooldown;
						unsigned short Order;
						uint32 Hash;

					public:
						SkillCategoryInfo() {}
						SkillCategoryInfo(const wchar_t* n, float cd, unsigned short o);

						SkillCategoryInfo& operator= (const SkillCategoryInfo& s) {
							this->Name = s.Name;
							this->Cooldown = s.Cooldown;
							this->Order = s.Order;
							this->Hash = s.Hash;
							return *this;
						}

					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);
					};
					typedef std::vector<SkillCategoryInfo> CategoryList;

				public:
					std::wstring Name;
					uint32 Hash;
					CategoryList Categorys;

				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}