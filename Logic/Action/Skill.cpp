#include "Skill.h"
#include "../State/States.h"
#include "../Object/Player.h"
#include <Info/Info/Skills/Skills.h>

using namespace Lunia::XRated::Database;
using namespace Lunia::XRated::Database::Info;

namespace Lunia { namespace XRated { namespace Logic {
	namespace Action {

		SkillManager::Skills::Skills(Database::Info::SkillCategoryList::SkillCategoryInfo* i)
			: info(i), currentCooldown(0), hash(info->Hash)
		{
		}

		SkillManager::SkillManager(Player* p) : owner(p)
		{
#ifdef _SINGLE
			isCheckBlockusingSkill = true;
			int temp;
			Lunia::Config::ConfigInstance().Get(L"single", L"skillcooldown", temp, 1);
			isCheckBlockusingSkill = temp ? true: false;
#endif
		}

		void SkillManager::Clear()
		{
			cooldownList.clear();
			skills.clear();
		}

		void SkillManager::Init(Constants::ClassType type)
		{
			Clear();

			if ( type >= Constants::ClassType::NumberOfClassTypes || (int)type < 0 ) {
				Logger::GetInstance().Error( L"[SkillManager::Init] Wrong class type. [{0}]", (int)type );
				return;
			}

			LoadSkill(type);
		}

		void SkillManager::AddSkillGroup(uint32 hash, Database::Info::SkillInfoList::SkillGroup* info)
		{
			for ( Category::iterator i = skills.begin() ; i != skills.end() ; ++i) {
				if ( (*i).hash == hash ) {
					(*i).skillList.push_back( SkillGroup( info ) );
					return;
				}
			}
			Logger::GetInstance().Info( L"[SkillManager::AddSkillGroup] unable to find category [{0}] in skillmanager.", hash );
		}

		void SkillManager::LoadSkill(Constants::ClassType type)
		{
			SkillCategoryList::CategoryList& list = DatabaseInstance().InfoCollections.Skills.GetCategoryList(Constants::GetClassStr(type));
			SkillCategoryList::CategoryList::iterator i = list.begin();
			SkillCategoryList::CategoryList::iterator end = list.end();
			for(; i != end ; ++i)
				skills.push_back( Skills(&(*i)) );

			{
				SkillInfoList::SkillGroupList& groups = DatabaseInstance().InfoCollections.Skills.GetSkillGroupList(Constants::GetClassStr(type));
				SkillInfoList::SkillGroupList::iterator i = groups.begin(), end = groups.end();
				for (; i != end ; ++i )
					AddSkillGroup((*i).HashCategory, &(*i));
			}
		}

		bool SkillManager::IsAbleToCast(uint32 skillGroup, uint32& actionName, float cooldownFactor, float& mp, float& hp, bool bCheckHitAction, float totalMp, float totalHp)
		{
			for (Category::iterator i=skills.begin() ; i != skills.end() ; ++i) {
				Skills& skillsRef = (*i);
				for(SkillList::iterator s=skillsRef.skillList.begin() ; s != skillsRef.skillList.end() ; ++s) {
					SkillGroup& sg = *s;
					if ( sg.skillGroup->Hash == skillGroup) { 
						if ( skillsRef.currentCooldown > 0 ) 
							return false;

						if (bCheckHitAction && !sg.skillGroup->IsPossibleAction(owner->GetActionNameHash())) 
							return false;

						SkillInfoList::SkillInfo* info = sg.skillGroup->GetSkillInfo( sg.GetLevel() );

						if (!info)
						{
							Logger::GetInstance().Error(L"SkillManager::IsAbleToCast Failed because SkillInfo Not Found. skillGroup={0}, lv={1}", skillGroup, sg.GetLevel() );
							return false;
						}

#ifdef _SINGLE
						
						if( isCheckBlockusingSkill == true )
#endif
						if ( sg.GetLevel() < 1 || sg.currentCooldown > 0 )
							return false;


						float useMp = sg.GetManaCost(totalMp);
						float useHp = sg.GetHPCost(totalHp);
						if ( useMp > mp || useHp > hp ) { 
							mp = hp = -1;
							return false;
						}

						
						actionName = info->ActionHash;
						mp = useMp;
						hp = useHp;

						
						skillsRef.SetCooldown(cooldownFactor);
						sg.SetCooldown(cooldownFactor);
						cooldownList.push_back( skillsRef.GetCooldown() );
						cooldownList.push_back( sg.GetCooldown() );
						return true;
					}
				}
			}
			return false;
		}

		bool SkillManager::CheckPrecedingSkill(const SkillInfoList::SkillInfo* info)
		{
			std::vector<SkillInfoList::SkillInfo::PrecedingSkill>::const_iterator i = info->PrecedingSkills.begin();
			for ( ; i != info->PrecedingSkills.end() ; ++i) {
				if ( GetSkillLevel( (*i).Hash ) < (*i).Lv ) {
					Logger::GetInstance().Info( L"[SkillManager::CheckPrecedingSkill] skill [{0}] preceding skill [{1}] Lv [{2}].", info->Name.c_str(), (*i).GroupName.c_str(), (*i).Lv );
					return false;
				}
			}
			return true;
		}

		int16 SkillManager::GetSkillLevel(uint32 groupHash) const
		{
			Category::const_iterator end = skills.end();
			for (Category::const_iterator i=skills.begin() ; i != end ; ++i) {
				SkillList::const_iterator skillListEnd = (*i).skillList.end();
				for(SkillList::const_iterator s=(*i).skillList.begin() ; s != skillListEnd ; ++s)
					if ( (*s).skillGroup->Hash == groupHash)
						return (*s).GetLevel();
			}
			return -1;
		}

		int16 SkillManager::GetSkillLevel(uint32 skill, uint32& category) const
		{
			Category::const_iterator end = skills.end();
			for (Category::const_iterator i=skills.begin() ; i != end ; ++i) {
				SkillList::const_iterator skillListEnd = (*i).skillList.end();
				for(SkillList::const_iterator s=(*i).skillList.begin() ; s != skillListEnd ; ++s)
					if ( (*s).skillGroup->Hash == skill) {
						category = (*i).hash;
						return (*s).GetLevel();
					}
			}
			category = 0;
			return -1;
		}

		bool SkillManager::SetSkillLevel(uint32 skill, uint8 level)
		{
			const static uint32 passiveCode = StringUtil::Hash(L"passive");
			Category::iterator end = skills.end();
			for (Category::iterator i=skills.begin() ; i != end ; ++i) {
				SkillList::iterator skillListEnd = (*i).skillList.end();
				for(SkillList::iterator s=(*i).skillList.begin() ; s != skillListEnd ; ++s) {
					if ( (*s).skillGroup->Hash == skill) {
						(*s).SetLevel( level );
						(*s).levelBySP = level;
						if ((*i).hash == passiveCode) {
							Database::Info::SkillInfoList::SkillInfo* skillInfo = (*s).GetSkillInfo( (*s).GetLevel() );
							if ( !skillInfo ) return false;
							uint32 name = skillInfo->ActionHash;
							StateBundleInfo* stateBundle =
								DatabaseInstance().InfoCollections.StateBundles.Retrieve(name);
							if (!stateBundle) {
								Logger::GetInstance().Error( L"[SkillManager::SetSkillLevel] stateBundleInfo [{0}] not found", skillInfo->ActionName.c_str() );
							} else {
								owner->AddPassive( stateBundle );
							}
						}
						return true;
					}
				}
			}
			return false;
		}

		uint8 SkillManager::IncSkillLevel(uint32 skillGroup, uint8 level, bool bUseSP)
		{

			const static uint32 passiveCode = StringUtil::Hash(L"passive");
			Category::iterator end = skills.end();
			for (Category::iterator i=skills.begin() ; i != end ; ++i) {
				SkillList::iterator skillListEnd = (*i).skillList.end();
				for(SkillList::iterator s=(*i).skillList.begin() ; s != skillListEnd ; ++s)
					if ( (*s).skillGroup->Hash == skillGroup) {
						Database::Info::SkillInfoList::SkillInfo* skillInfoNext;
						uint8 lv = (*s).GetLevel();
						if ( bUseSP ) {

							if ( level != 1 ) return 0;

							if ( (*s).IsCommonerMax() )
								return 0;

							skillInfoNext = (*s).GetSkillInfo( (*s).levelBySP+1 );
							if ( skillInfoNext->LevelLimit > owner->GetLevel() || skillInfoNext->RebirthCountLimit > owner->GetRebirthCount() || skillInfoNext->StoredLevelLimit > owner->GetStoredLevel() )
							{
								return 0;
							}

							if ( !CheckPrecedingSkill( skillInfoNext ) ) return 0;

							
						} 

						
						(*s).IncreaseLevel(level, bUseSP);
						skillInfoNext = (*s).GetSkillInfo( (*s).GetLevel() );


						if ((*i).hash == passiveCode) {
							StateBundleInfo* stateBundle = DatabaseInstance().InfoCollections.StateBundles.Retrieve(skillInfoNext->ActionHash);
							if (stateBundle) {
								owner->AddPassive( stateBundle );
							} else {
								Logger::GetInstance().Error( L"[SkillManager::IncSkillLevel] stateBundle [{0}] not found in database.", skillInfoNext->ActionName.c_str() );
							}
							if ( lv > 0 ) {
								stateBundle = DatabaseInstance().InfoCollections.StateBundles.Retrieve((*s).GetSkillInfo(lv)->ActionHash);
								if ( stateBundle)
									owner->RemovePassive( stateBundle );
							}
						}
						
						return (*s).GetLevel() - lv;
					}
			}
			return 0;
		}

		bool SkillManager::DecSkillLevel(uint32 skill, uint8 level, bool bUseSP)
		{
			const static uint32 passiveCode = StringUtil::Hash(L"passive");
			Category::iterator end = skills.end();
			for (Category::iterator i=skills.begin() ; i != end ; ++i) {
				SkillList::iterator skillListEnd = (*i).skillList.end();
				for(SkillList::iterator s=(*i).skillList.begin() ; s != skillListEnd ; ++s)
					if ( (*s).skillGroup->Hash == skill) {
						if ( (*s).GetLevel() == 0 ) 
							return false;

						if ( bUseSP && level != 1 ) return false;

						StateBundleInfo* oldBundle = NULL;
						StateBundleInfo* newBundle = NULL;

						if ((*i).hash == passiveCode)
							oldBundle = DatabaseInstance().InfoCollections.StateBundles.Retrieve((*s).GetSkillInfo()->ActionHash);

						(*s).DecreaseLevel(level, bUseSP);

						if ((*i).hash == passiveCode && (*s).GetLevel() > 0) {
							Database::Info::SkillInfoList::SkillInfo* skillInfo = (*s).GetSkillInfo( (*s).GetLevel() );
							newBundle = DatabaseInstance().InfoCollections.StateBundles.Retrieve(skillInfo->ActionHash);
						}						

						if(newBundle)
							owner->AddPassive( newBundle );

						if(oldBundle)
							owner->RemovePassive( oldBundle );

						return true;
					}
			}
			return false;
		}

		bool SkillManager::ResetSkillLevel(uint32 skill)
		{
			const static uint32 passiveCode = StringUtil::Hash(L"passive");
			Category::iterator end = skills.end();
			for (Category::iterator i=skills.begin() ; i != end ; ++i) {
				SkillList::iterator skillListEnd = (*i).skillList.end();
				for(SkillList::iterator s=(*i).skillList.begin() ; s != skillListEnd ; ++s)
					if ( (*s).skillGroup->Hash == skill) {
						if ( (*s).GetLevel() < (*s).levelBySP )
							return false;
						if( (*s).GetLevel() < 1 ) {
							Logger::GetInstance().Exception(L"ResetSkillLevel Failed. this Skill level is zero. SkillHash : ({0})", skill );
							return false;
						}

						StateBundleInfo* stateBundle;
						if ((*i).hash == passiveCode) {
							stateBundle = DatabaseInstance().InfoCollections.StateBundles.Retrieve((*s).GetSkillInfo()->ActionHash);
							if ( stateBundle)
								owner->RemovePassive( stateBundle );
						}

						(*s).ResetLevelBySP();

						if ((*i).hash == passiveCode && (*s).GetLevel() > 0) {
							Database::Info::SkillInfoList::SkillInfo* skillInfo = (*s).GetSkillInfo( (*s).GetLevel() );
							stateBundle = DatabaseInstance().InfoCollections.StateBundles.Retrieve(skillInfo->ActionHash);
							if (stateBundle)
								owner->AddPassive( stateBundle );
						}

						return true;
					}
			}
			return false;
		}

		void SkillManager::CoolDown(float dt)
		{
			for ( std::list<float*>::iterator i = cooldownList.begin() ; i != cooldownList.end() ; ) {
				*(*i) -= dt;
				if ( *(*i) <= 0 ) {
					*(*i) = 0;
					i = cooldownList.erase(i);
				} else ++i;
			}
		}

		void SkillManager::ResetCoolDown()
		{
			/**************************************************************************
			 * @Oracle 24 DEC 2019 00:50 EST                                          *
			 * This needs testing : Does [item#test_potion002] still reset cooldown?  *
			 * What happens when you die? Do skills reset?                            *
			 **************************************************************************/

			for (Category::iterator i = skills.begin(); i != skills.end(); ++i){
				for(SkillList::iterator s = (*i).skillList.begin(); s != (*i).skillList.end(); ++s){
					if((*s).skillGroup->GetCoolTimeResetAble()) {
						(*s).currentCooldown = 0;
					}
				}
			}
		}

		void SkillManager::ResetGroupCoolDown(uint32 skillGroupHash)
		{
			for (Category::iterator i = skills.begin(); i != skills.end(); ++i)
				for(SkillList::iterator s = (*i).skillList.begin(); s != (*i).skillList.end(); ++s)
					if((*s).skillGroup->Hash == skillGroupHash)
						(*s).currentCooldown = 0;

		}

		void SkillManager::AddSkillCooldownTime(uint32 skillHash, float amount)
		{
			for (Category::iterator i = skills.begin(); i != skills.end(); ++i)
				for(SkillList::iterator s = (*i).skillList.begin(); s != (*i).skillList.end(); ++s)
					if((*s).skillGroup->Hash == skillHash)
						(*s).AddCoolDownTime(amount);
		}


	}

}	}	}
