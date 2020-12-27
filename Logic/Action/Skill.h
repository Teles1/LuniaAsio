
#pragma once

#include <Info/Info.h>
#include <Info/Info/Database.h>
#include <Core/GameConstants.h>
#include "../State/State.h"

namespace Lunia { namespace XRated { namespace Logic {
	class Player;

	namespace Action {

		class SkillManager {
#ifdef _SINGLE
		bool isCheckBlockusingSkill;
#endif
		protected :
			struct SkillGroup {
				Database::Info::SkillInfoList::SkillGroup* skillGroup;
				float currentCooldown;
				float addCoolTime;
				uint8 level; 
				uint8 levelBySP;

				SkillGroup() { currentCooldown=0; addCoolTime=0; level=0; }
				SkillGroup(Database::Info::SkillInfoList::SkillGroup* i)
					: skillGroup(i), currentCooldown(0), addCoolTime(0), level(0), levelBySP(0) {}
				SkillGroup& operator= (SkillGroup s) {
					skillGroup = s.skillGroup;
					currentCooldown = s.currentCooldown;
					level = s.level;
					levelBySP = s.levelBySP;
					return *this;
				}

				void SetLevel( uint8 lv )
				{
					level = lv;
				}
				uint8 GetLevel() const
				{
					return std::min<uint8>( levelBySP + Database::DatabaseInstance().InfoCollections.EnchantTables.GetConstants().MaxSkillPlus , level );
				}

				float GetManaCost(float totalMp, uint8 lv=0) {
					float skillMp = 0.f;
					if (lv) skillMp = skillGroup->GetSkillInfo(lv)->Mp;
					skillMp = skillGroup->GetSkillInfo(GetLevel())->Mp;

					if (skillMp <= 1.0f && skillMp > 0.f) return totalMp*skillMp;
					
					return skillMp;
				}
				float GetHPCost(float totalHp, uint8 lv=0) {
					float skillHp = 0.f;
					
					if (lv) skillHp = skillGroup->GetSkillInfo(lv)->Hp;
					skillHp = skillGroup->GetSkillInfo(GetLevel())->Hp;

					if (skillHp <= 1.0f && skillHp > 0.f) return totalHp*skillHp;
					
					return skillHp;
				}
				const std::wstring& GetActionName(uint8 lv=0) {
					if (lv) return skillGroup->GetSkillInfo(lv)->ActionName;
					return skillGroup->GetSkillInfo(GetLevel())->ActionName;
				}
				const std::wstring& GetSkillName() const { return skillGroup->Name; }
				void SetCooldown(float cooldownFactor=1.0f, uint8 lv=0) {
					float adjCooldownFactor = (skillGroup->GetCoolTimeDecreaseable()) ? cooldownFactor : 1.0f;    /* 11-2-2019 9:14 @Oracle */
					 
					if (lv)
					{
						currentCooldown = skillGroup->GetSkillInfo(lv)->Cooldown * adjCooldownFactor + addCoolTime;  /* 11-2-2019 9:14 @Oracle */
						if(currentCooldown > addCoolTime)
							currentCooldown -= addCoolTime;
						else
							currentCooldown = 0.0f;
					}
					else
					{
						currentCooldown = skillGroup->GetSkillInfo(GetLevel())->Cooldown * adjCooldownFactor; /* 11-2-2019 9:14 @Oracle */
						if(currentCooldown > addCoolTime)
							currentCooldown -= addCoolTime;
						else
							currentCooldown = 0.0f;
					}
				}
				float* GetCooldown() { return &currentCooldown; }
				Database::Info::SkillInfoList::SkillInfo* GetSkillInfo(uint8 lv=0) {
					if (lv) return skillGroup->GetSkillInfo(lv);
					return skillGroup->GetSkillInfo(GetLevel());
				}

				uint8 GetAvailableLevelToIncreaseByItem(uint8 incLv) const 
				{
					uint8 lv = GetAvailableLevelToIncrease( incLv );
					return std::min<uint8>( lv, Database::DatabaseInstance().InfoCollections.EnchantTables.GetConstants().MaxSkillPlus - (level - levelBySP) );
				}
				uint8 GetAvailableLevelToIncrease(uint8 incLv) const 
				{
					uint8 max = skillGroup->GetMaxLevel();
					if (max > level + incLv) return incLv;
					else if ( max > level ) return max - level;
					return 0;
				}

				bool IsCommonerMax() const { return skillGroup->GetCommonerMaxLv() == levelBySP; }
				void IncreaseLevel(uint8 lv, bool bUseSP) 
				{
					if ( bUseSP )
					{
						if ( levelBySP >= skillGroup->GetCommonerMaxLv() )
						{
							levelBySP = skillGroup->GetCommonerMaxLv();
							return;
						}
						levelBySP = levelBySP + lv;
					}
					level = level + lv; 
					
				}
				void DecreaseLevel(uint8 lv, bool bUseSP) {
					if ( bUseSP) levelBySP = levelBySP > lv ? levelBySP-lv : 0;
					level = level > lv ? level-lv : 0;
				}
				void ResetLevelBySP() { level = level - levelBySP; levelBySP = 0; }
				void AddCoolDownTime(float amount) { addCoolTime += amount; }
			};
			typedef std::vector<SkillGroup> SkillList;

			struct Skills {
				Lunia::XRated::Database::Info::SkillCategoryList::SkillCategoryInfo* info;
				uint32 hash;
				float currentCooldown;
				SkillList skillList;

				Skills() { currentCooldown = 0; }
				Skills(Database::Info::SkillCategoryList::SkillCategoryInfo* i);

				void SetCooldown(float cooldownFactor=1.0f) { currentCooldown = info->Cooldown * cooldownFactor; }
				float* GetCooldown() { return &currentCooldown; }
			};
			typedef std::vector<Skills> Category;

		private : // Member definition
			Player* owner;
			Category skills;
			std::list<float*> cooldownList;

		public:
			SkillManager(Player* p);
			virtual ~SkillManager() {}

			void Init(Constants::ClassType type);
			void Clear();
			void LoadSkill(Constants::ClassType type);
			void AddSkillGroup(uint32 category, Database::Info::SkillInfoList::SkillGroup* info);

			bool IsAbleToCast(uint32 skillGroup, uint32& actionName, float cooldownFactor, float& mp, float& hp, bool bCheckHitAction, float totalMp, float totalHp);
			bool CheckPrecedingSkill(const Database::Info::SkillInfoList::SkillInfo* info);
			bool SetSkillLevel(uint32 skillGroup, uint8 level);
			uint8 IncSkillLevel(uint32 skillGroup, uint8 level, bool bUseSP);
			bool DecSkillLevel(uint32 skillGroup, uint8 level, bool bUseSP);
			bool ResetSkillLevel(uint32 skillGroup);
			//const wchar_t* GetActionName(const wchar_t* skill);

			/**Find skill and return skill level
			@param skill		skill name to find.
			@return				if there is no such skill return -1, otherwise return skill level */
			//int16 GetSkillLevel(const wchar_t* skillGroup);
			int16 GetSkillLevel(uint32 groupHash) const;
			//int16 GetSkillLevel(const wchar_t* skill, std::wstring& categoary);
			int16 GetSkillLevel(uint32 skill, uint32& categoary) const;

			void CoolDown(float dt);
			void ResetCoolDown();
			void ResetGroupCoolDown(uint32 skillGroupHash);
			void AddSkillCooldownTime(uint32 skillHash, float amount);

		};

	}
}	}	}
