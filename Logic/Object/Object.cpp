#include "Object.h"
#include "../Sector/Sector.h"
#include "../GameState.h"

namespace Lunia { namespace XRated {	namespace Logic {

	////////////////

	Object::IncreaseState::IncreaseState()
	{
		Init();
	}

	void Object::IncreaseState::Init()
	{
		incHealRate = 0.0f;

		criAtkInc = 0.0f;

		criMagicInc = 0.0f;

		for(int damageType=0; damageType < Constants::DamageType::CNT; ++damageType)
			incDmg[damageType]=0;
	}

	void Object::IncreaseState::AddIncDamage(XRated::Constants::DamageType damageType, float incDmg)
	{
		this->incDmg[static_cast<int>(damageType)] += incDmg;
	}

	float Object::IncreaseState::CalculrateIncDamage(XRated::Constants::DamageType damageType, const float& dmg)
	{
		return dmg * incDmg[damageType];
	}

	void Object::IncreaseState::AddIncHealRate(float amount)
	{
		this->incHealRate += amount;
	}

	float Object::IncreaseState::CalculrateIncHealAmount(const float& amount)
	{
		return amount * incHealRate;
	}

	void Object::IncreaseState::AddCriAtkInc( float amount )
	{
		criAtkInc += amount;
	}

	void Object::IncreaseState::AddCriMagicInc( float amount )
	{
		criMagicInc += amount;
	}

	////////////////


	Object::Object(Constants::ObjectType tp)
		: stateFlags(Database::Info::StateInfo::Type::STATECOUNT, 0)
	{
		objectData.Type = tp;
		objectData.Direction = Lunia::XRated::Constants::DirectionF::Down; 
	}

	Object::~Object()
	{
		Clear();
	}

	void Object::Clear()
	{
		increase.Init();

		std::list<Stat::IState*>::iterator iter;
		for (iter = stateBuffers.state_attack.begin() ; iter != stateBuffers.state_attack.end(); ++iter)
			delete (*iter);
		for (iter = stateBuffers.state_defence.begin() ; iter != stateBuffers.state_defence.end(); ++iter)
			delete (*iter);
		for (iter = stateBuffers.state_ordinary.begin() ; iter != stateBuffers.state_ordinary.end(); ++iter)
			delete (*iter);
		for (iter = stateBuffers.state_condition.begin() ; iter != stateBuffers.state_condition.end(); ++iter)
			delete (*iter);

		stateBuffers.state_attack.clear();
		stateBuffers.state_defence.clear();
		stateBuffers.state_ordinary.clear();
		stateBuffers.state_condition.clear();		
	}

	void Object::Initialize(IGameStateDatabase* db)
	{
		stageData = db;
		std::fill(stateFlags.begin(), stateFlags.end(), 0);
		stateCnt = 0;
		Clear();
	}

	void Object::SetDirection(const float3& dir)
	{
		if (dir.x == 0 && dir.z == 0) {
			//서버에서는 죽이지말고 아랫 방향을 셋팅하자.
			//Lunia_INFO(( L"Error [Object::SetDirection] Wrong direction." ));
			objectData.Direction.SetVector(0, 0, -1);
			return;
		}

		objectData.Direction=dir;
	}

	void Object::SetPosition(const float3& pos)
	{
		objectData.Position=pos;
		stageData->ObjectMoved(this);
	}

	void Object::InitPosition(const float3& pos)
	{
		objectData.Position = pos;
	}

	void Object::SetSmallSector(Sector* sSector)
	{
		sectors.smallSector = sSector;
	}

	void Object::SetBigSector(Sector* bSector)
	{
		sectors.bigSector = bSector;
	}

	bool Object::RemoveState(StateList& list, Database::Info::StateInfo::Type type)
	{
		std::list<Stat::IState*>::iterator iter;
		for (iter = list.begin() ; iter != list.end(); ++iter) {
			Database::Info::StateInfo::Type tp= (*iter)->GetType();
			if ( tp == type) {
				if ( tp < 0 || tp >= Database::Info::StateInfo::Type::STATECOUNT ) {
					ObjectData data = this->GetObjectData();
					Logger::GetInstance().Exception(L"[Object::RemoveState] wrong state type. [{0}/{1}/{2}]", tp, (int)(*iter)->GetSort(), data.NameHash );
				}
				--stateFlags[(uint8)tp];
				delete ((*iter));
				list.erase(iter);
				return true;
			}
		}
		return false;
	}

	bool Object::AddState(Stat::IState* state, bool bFront)
	{
		state->SetId(stateCnt++);
		switch (state->GetSort()) {
			case Database::Info::StateInfo::Sort::ORDINARYSTATE :
				if (bFront)
					stateBuffers.state_ordinary.push_front(state);
				else
					stateBuffers.state_ordinary.push_back(state);
				break;
			case Database::Info::StateInfo::Sort::ATTACKSTATE :
				if (bFront)
					stateBuffers.state_attack.push_front(state);
				else
					stateBuffers.state_attack.push_back(state);
				break;
			case Database::Info::StateInfo::Sort::DEFENCESTATE :
				if (bFront)
					stateBuffers.state_defence.push_front(state);
				else
					stateBuffers.state_defence.push_back(state);
				break;
			case Database::Info::StateInfo::Sort::CONDITION:
				if(bFront)
					stateBuffers.state_condition.push_front(state);
				else
					stateBuffers.state_condition.push_back(state);
				break;
			default :
				return false;
		}
		state->Initialize(stageData);

		uint8 stateType = (uint8)state->GetType();
		if ( stateType < 0 || stateType >= Database::Info::StateInfo::Type::STATECOUNT ) {
			ObjectData data = this->GetObjectData();
			Logger::GetInstance().Exception(L"[Object::AddState] wrong state type. [{0}/{1}/{2}]", stateType, (int)state->GetSort(), data.NameHash);
		}
		++stateFlags[stateType];
		return true;
	}

	bool Object::RemoveState(Database::Info::StateInfo::Sort sort, uint32 id)
	{
		uint8 stateType;
		Stat::IState* state;
		switch (sort) {
			case Database::Info::StateInfo::Sort::ORDINARYSTATE :
				for (StateList::iterator iter = stateBuffers.state_ordinary.begin() ; iter != stateBuffers.state_ordinary.end() ; ++iter) {
					state = (*iter);
					if (state->GetId() == id) {
						state->Destroy();
						stateType = (uint8)state->GetType();
						if ( stateType < 0 || stateType >= Database::Info::StateInfo::Type::STATECOUNT ) {
							ObjectData data = this->GetObjectData();
							Logger::GetInstance().Exception(L"[Object::RemoveState] wrong state type. [{0}/{1}/{2}]", stateType, (int)state->GetSort(), data.NameHash );
						}
						--stateFlags[stateType];
						delete state;
						stateBuffers.state_ordinary.erase(iter);
						return true;
					}
				}
				break;
			case Database::Info::StateInfo::Sort::ATTACKSTATE :
				for (StateList::iterator iter = stateBuffers.state_attack.begin() ; iter != stateBuffers.state_attack.end() ; ++iter) {
					state = (*iter);
					if (state->GetId() == id) {
						state->Destroy();
						stateType = (uint8)state->GetType();
						if ( stateType < 0 || stateType >= Database::Info::StateInfo::Type::STATECOUNT ) {
							ObjectData data = this->GetObjectData();
							Logger::GetInstance().Exception(L"[Object::RemoveState] wrong state type. [{0}/{1}/{2}]", stateType, (int)state->GetSort(), data.NameHash);
						}
						--stateFlags[stateType];
						delete state;
						stateBuffers.state_attack.erase(iter);
						return true;
					}
				}
				break;
			case Database::Info::StateInfo::Sort::DEFENCESTATE :
				for (StateList::iterator iter = stateBuffers.state_defence.begin() ; iter != stateBuffers.state_defence.end() ; ++iter) {
					state = (*iter);
					if (state->GetId() == id) {
						state->Destroy();
						stateType = (uint8)state->GetType();
						if ( stateType < 0 || stateType >= Database::Info::StateInfo::Type::STATECOUNT ) {
							ObjectData data = this->GetObjectData();
							Logger::GetInstance().Exception(L"[Object::RemoveState] wrong state type. [{0}/{1}/{2}]", stateType, (int)state->GetSort(), data.NameHash);
						}
						--stateFlags[stateType];
						delete state;
						stateBuffers.state_defence.erase(iter);
						return true;
					}
				}
				break;
			case Database::Info::StateInfo::Sort::CONDITION :
				for (StateList::iterator iter = stateBuffers.state_condition.begin() ; iter != stateBuffers.state_condition.end() ; ++iter) {
					state = (*iter);
					if (state->GetId() == id) {
						state->Destroy();
						stateType = (uint8)state->GetType();
						if ( stateType < 0 || stateType >= Database::Info::StateInfo::Type::STATECOUNT ) {
							ObjectData data = this->GetObjectData();
							Logger::GetInstance().Exception(L"[Object::RemoveState] wrong state type. [{0}/{1}/{2}]", stateType, (int)state->GetSort(), data.NameHash);
						}
						--stateFlags[stateType];
						delete state;
						stateBuffers.state_condition.erase(iter);
						return true;
					}
				}
				break;
				
			default :
				return false;
		}
		return false;
	}

	Stat::IState *Object::FindState(const StateList &stateList, uint32 id)
	{
        for (StateList::const_iterator iter = stateList.begin() ; 
			iter != stateList.end(); 
			++iter) 
		{
			Stat::IState *state = (*iter);
			if(state->GetId() == id)
			{
				return state;
			}
		}

		return 0;
	}

	bool Object::EnableState(Database::Info::StateInfo::Sort sort, uint32 id, bool enable)
	{
		uint8 stateType;
		Stat::IState* state;
		switch (sort) {
			case Database::Info::StateInfo::Sort::ORDINARYSTATE :
				state = FindState(stateBuffers.state_ordinary, id);
				break;
			case Database::Info::StateInfo::Sort::ATTACKSTATE :
				state = FindState(stateBuffers.state_attack, id);
				break;
			case Database::Info::StateInfo::Sort::DEFENCESTATE :
				state = FindState(stateBuffers.state_defence, id);
				break;
			case Database::Info::StateInfo::Sort::CONDITION :
				state = FindState(stateBuffers.state_condition, id);
				break;
			default :
				return false;
		}

		if(!state)
		{
			return false;
		}

		stateType = (uint8)state->GetType();
		if(stateType < 0 || stateType >= Database::Info::StateInfo::Type::STATECOUNT) 
		{
			ObjectData data = this->GetObjectData();
			Logger::GetInstance().Exception(L"[Object::RemoveState] wrong state type. [{0}/{1}/{2}]", stateType, (int)state->GetSort(), data.NameHash);
		}
		
		if(enable)
		{
			state->Enable(true);
			++stateFlags[stateType];			
		}
		else
		{
			state->Enable(false);
			--stateFlags[stateType];
		}
		
		return true;
	}

	Stat::IState* Object::GetState(Database::Info::StateInfo::Type type, Database::Info::StateInfo::Sort sort)
	{
		switch(sort) {
		case Database::Info::StateInfo::Sort::ATTACKSTATE :
			for (StateList::iterator iter = stateBuffers.state_attack.begin() ; iter != stateBuffers.state_attack.end() ; ++iter)
				if ((*iter)->GetType() == type)
					return (*iter);
			break;
		case Database::Info::StateInfo::Sort::ORDINARYSTATE :
			for (StateList::iterator iter = stateBuffers.state_ordinary.begin() ; iter != stateBuffers.state_ordinary.end() ; ++iter)
				if ((*iter)->GetType() == type)
					return (*iter);
			break;
		case Database::Info::StateInfo::Sort::DEFENCESTATE :
			for (StateList::iterator iter = stateBuffers.state_defence.begin() ; iter != stateBuffers.state_defence.end() ; ++iter)
				if ((*iter)->GetType() == type)
					return (*iter);
			break;
		case Database::Info::StateInfo::Sort::CONDITION :
			for (StateList::iterator iter = stateBuffers.state_condition.begin() ; iter != stateBuffers.state_condition.end() ; ++iter)
				if ((*iter)->GetType() == type)
					return (*iter);
			break;
		}
		return NULL;
	}

	Stat::IState* Object::GetState(Database::Info::StateInfo::Type type)
	{
		StateList::iterator iter;
		for (iter = stateBuffers.state_ordinary.begin() ; iter != stateBuffers.state_ordinary.end() ; ++iter)
			if ((*iter)->GetType() == type)
				return (*iter);
		for (iter = stateBuffers.state_attack.begin() ; iter != stateBuffers.state_attack.end() ; ++iter)
			if ((*iter)->GetType() == type)
				return (*iter);
		for (iter = stateBuffers.state_defence.begin() ; iter != stateBuffers.state_defence.end() ; ++iter)
			if ((*iter)->GetType() == type)
				return (*iter);
		for (iter = stateBuffers.state_condition.begin() ; iter != stateBuffers.state_condition.end() ; ++iter)
			if ((*iter)->GetType() == type)
				return (*iter);
		return NULL;
	}

	std::vector<Stat::IState*> Object::GetStates(Database::Info::StateInfo::Type type, Database::Info::StateInfo::Sort sort)
	{
		std::vector<Stat::IState*> stateVector;

		switch(sort)
		{
		case Database::Info::StateInfo::Sort::ATTACKSTATE :
			for (StateList::iterator iter = stateBuffers.state_attack.begin() ; iter != stateBuffers.state_attack.end() ; ++iter)
				if ((*iter)->GetType() == type)
					stateVector.push_back(*iter);
			break;
		case Database::Info::StateInfo::Sort::ORDINARYSTATE :
			for (StateList::iterator iter = stateBuffers.state_ordinary.begin() ; iter != stateBuffers.state_ordinary.end() ; ++iter)
				if ((*iter)->GetType() == type)
					stateVector.push_back(*iter);
			break;
		case Database::Info::StateInfo::Sort::DEFENCESTATE :
			for (StateList::iterator iter = stateBuffers.state_defence.begin() ; iter != stateBuffers.state_defence.end() ; ++iter)
				if ((*iter)->GetType() == type)
					stateVector.push_back(*iter);
			break;
		case Database::Info::StateInfo::Sort::CONDITION :
			for (StateList::iterator iter = stateBuffers.state_condition.begin() ; iter != stateBuffers.state_condition.end() ; ++iter)
				if ((*iter)->GetType() == type)
					stateVector.push_back(*iter);
			break;
		}

		return stateVector;
	}

	bool Object::Update(float dt, IGameStateDatabase* /*db*/)
	{
		//bUpdate = true;
		//현재 상태 stack 에 들어 있는 상태들을 처리 허자고.

		UpdateStates(dt, stateBuffers.state_ordinary);
		UpdateStates(dt, stateBuffers.state_condition);

		//bUpdate = false;
		return false;
	}

	void Object::UpdateStates(float dt, StateList& list)
	{
		Stat::IState* statePtr;
		uint8 stateType;

		for(StateList::iterator itr = list.begin(); itr != list.end();)
		{
			statePtr = (*itr);
			if(statePtr->Do(dt, stageData))
			{
				statePtr->Destroy();
				stateType = (uint8)statePtr->GetType();
				if(stateType < 0 || stateType >= Database::Info::StateInfo::Type::STATECOUNT)
				{
					ObjectData data = GetObjectData();
					Logger::GetInstance().Exception(L"[Object::Update] wrong state type. [{0}/{1}/{2}]", stateType, (int)statePtr->GetSort(), data.NameHash);
				}
				--stateFlags[stateType];
				delete (*itr);
				itr = list.erase(itr);
			}
			else
				++itr;
		}
	}

	void Object::AddIncDamage(XRated::Constants::DamageType damageType, float incDmg)
	{
		increase.AddIncDamage(damageType, incDmg);
	}

	void Object::AddIncDamage(uint16 damageType, float incDmg)
	{
		for(int i = 0; i < XRated::Constants::DamageType::CNT; ++i)
		{
			if(damageType & (1 << i))
				increase.AddIncDamage(static_cast<XRated::Constants::DamageType>(i), incDmg);
		}
	}

	void Object::AddCriAtkInc( float amount )
	{
		increase.AddCriAtkInc( amount );
	}

	void Object::AddCriMagicInc( float amount )
	{
		increase.AddCriMagicInc( amount );
	}

	float Object::CalculrateIncDamage(XRated::Constants::DamageType damageType, const float& dmg)
	{
		return increase.CalculrateIncDamage(damageType, dmg);
	}

	void Object::AddIncHealRate(float amount)
	{
		increase.AddIncHealRate(amount);
	}

	float Object::CalculrateIncHealAmount(const float& amount)
	{
		return increase.CalculrateIncHealAmount(amount);
	}

}}}
