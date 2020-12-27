#include	"StateCondition.h"
#include	"States.h"
#include	"../GameStateDatabase.h"

using namespace Lunia::StringUtil;
using namespace Lunia::Math;
using namespace Lunia::XRated::Database;

namespace Lunia { namespace XRated {	namespace Logic { namespace Stat {

	//Condition::Condition(Actor* owner, float prob, bool self, float coolDown, uint32 bundlehash)
	//	:	State<Actor>(owner, Database::Info::StateInfo::Type:, Database::Info::StateInfo::Sort::CONDITION),
	//		randomFloat(NULL), probability(prob), bundleHash(bundlehash), bundleInfo(NULL), bSelf(self),
	//		coolDown(coolDown), coolDownTimer(0.0f)
	//{
	//}

	//Condition::Condition(Actor* owner, float prob, bool self, float coolDown, std::vector<IState*>& optionalStates)
	//	:	State<Actor>(owner, Database::Info::StateInfo::Type::CONDITION, Database::Info::StateInfo::Sort::CONDITION),
	//		randomFloat(NULL), probability(prob), bundleHash(0), bundleInfo(NULL), bSelf(self),
	//		coolDown(coolDown), coolDownTimer(0.0f), optionalStates(optionalStates)
	//{
	//}

	Condition::Condition(Database::Info::StateInfo::Type stateType, Actor* owner,
						float prob, bool self, float coolDown, uint32 bundlehash, std::vector<IState*>& optionalStates)
		:	State<Actor>(owner, stateType, Database::Info::StateInfo::Sort::CONDITION),
			randomFloat(NULL), probability(prob), bundleHash(bundlehash), bundleInfo(NULL), bSelf(self),
			coolDown(coolDown), coolDownTimer(0.0f), optionalStates(optionalStates)
	{
	}

	Condition::~Condition()
	{
		for(std::vector<IState*>::iterator itr = optionalStates.begin(); itr != optionalStates.end(); ++itr)
			delete *itr;
	}

	void Condition::Initialize(IGameStateDatabase* db)
	{
		randomFloat = db->GetRandomClass();
		if(bundleHash != 0)
			bundleInfo = DatabaseInstance().InfoCollections.StateBundles.Retrieve(bundleHash);
	}

	bool Condition::IsActivated()
	{
		if (randomFloat->Next() > probability)
			return false;

		if(coolDownTimer > 0)
			return false;

		return true;
	}

	bool Condition::Do(float dt, IGameStateDatabase*)
	{
		passedTime += dt;
		coolDownTimer -= dt;

		///	Condition State는 시간에 의해서 삭제되지 않슴...
		return false;
	}


	void Condition::Do(Object* target, IGameStateDatabase* db)
	{
		if(IsActivated())
		{
			coolDownTimer = coolDown;

			if(bSelf == true)
			{
				if(bundleInfo != NULL)
					if(owner->AddStateBundle(bundleInfo, owner->GetSerial()))
						db->StateChanged(owner->GetSerial(), bundleInfo->hash, bundleInfo->level);

				for(std::vector<IState*>::const_iterator itr = optionalStates.begin(); itr != optionalStates.end(); ++itr)
				{
					Stat::IState* state = CreateState(*itr, owner, owner->GetSerial());
					if(state != NULL)
						owner->AddState(state);
				}
			}
			else
			{
				if(target == NULL || !target->IsCharacter() )
					return;

				Actor* actor = (Actor*)target;

				if(bundleInfo != NULL)
					if(actor->AddStateBundle(bundleInfo, actor->GetSerial()))
						db->StateChanged(actor->GetSerial(), bundleInfo->hash, bundleInfo->level);

				for(std::vector<IState*>::const_iterator itr = optionalStates.begin(); itr != optionalStates.end(); ++itr)
				{
					Stat::IState* state = CreateState(*itr, actor, owner->GetSerial());
					if(state != NULL)
						actor->AddState(state);
				}
			}
		}
	}
	////

	////
	IfHit::IfHit(Actor* owner, float probability, bool self, float coolDown, uint32 bundlehash, std::vector<IState*>& optionalStates)
		:	Condition(Database::Info::StateInfo::IFHIT, owner, probability, self, coolDown, bundlehash, optionalStates)
	{
	}

	void IfHit::Do(Object* target, IGameStateDatabase* db)
	{
		Condition::Do(target, db);
	}
	////

	////
	IfHitBy::IfHitBy(Actor* owner, float probability, bool self, float coolDown, uint32 bundlehash, std::vector<IState*>& optionalStates)
		:	Condition(Database::Info::StateInfo::IFHITBY, owner, probability, self, coolDown, bundlehash, optionalStates)
	{
	}
	void IfHitBy::Do(Object* target, IGameStateDatabase* db)
	{
		Condition::Do(target, db);
	}
	////

	////
	IfHeal::IfHeal(Actor* owner, float probability, bool self, float coolDown, uint32 bundlehash, std::vector<IState*>& optionalStates)
		:	Condition(Database::Info::StateInfo::IFHEAL, owner, probability, self, coolDown, bundlehash, optionalStates)
	{
	}
	void IfHeal::Do(Object* target, IGameStateDatabase* db)
	{
		Condition::Do(target, db);
	}
	////

	////
	IfCriAtk::IfCriAtk(Actor* owner, float probability, bool self, float coolDown, uint32 bundlehash, std::vector<IState*>& optionalStates)
		:	Condition(Database::Info::StateInfo::IFCRIATK, owner, probability, self, coolDown, bundlehash, optionalStates)
	{
	}
	void IfCriAtk::Do(Object* target, IGameStateDatabase* db)
	{
		Condition::Do(target, db);
	}
	////

	////
	IfCriMagic::IfCriMagic(Actor* owner, float probability, bool self, float coolDown, uint32 bundlehash, std::vector<IState*>& optionalStates)
		:	Condition(Database::Info::StateInfo::IFCRIMAGIC, owner, probability, self, coolDown, bundlehash, optionalStates)
	{
	}

	void IfCriMagic::Do(Object* target, IGameStateDatabase* db)
	{
		Condition::Do(target, db);
	}
	////

	////
	IfCastSkill::IfCastSkill(Actor* owner, float probability, bool self, float coolDown, uint32 bundlehash, std::vector<IState*>& optionalStates, const std::wstring& skillGroupName)
		:	Condition(Database::Info::StateInfo::IFCASTSKILL, owner, probability, self, coolDown, bundlehash, optionalStates),
			skillGroupName(skillGroupName), skillGroupNameHash(StringUtil::Hash(skillGroupName.c_str())), enableAllSkill(skillGroupName == L"all" ? true : false)
	{

	}

	void IfCastSkill::Do(Object* target, IGameStateDatabase* db)
	{
		Condition::Do(target, db);
	}

	bool IfCastSkill::CheckSkillGroupName(const std::wstring& skillGroupName)
	{
		if(enableAllSkill == true)
			return	true;

		return	(this->skillGroupName == skillGroupName);
	}
	bool IfCastSkill::CheckSkillGroupName(uint32 skillGroup)
	{
		if(enableAllSkill == true)
			return	true;

		return	(skillGroupNameHash == skillGroup);
	}
	////


}}}}