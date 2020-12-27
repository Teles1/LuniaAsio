/*
    Copyright (c) 2000-, Lunia Corp. All rights reserved.
        http://www.Lunia.co.kr
            tel:82-2-888-1555
                fax:82-2-888-4414
                    mailTo:webmaster@Lunia.co.kr
*/
#include "../States.h"
#include "../../LogicDB/LogicDB.h"

#include <Info/Info/Database.h>

#include "StateBundle.h"

using namespace Lunia::XRated::Database::Info;

namespace Lunia { namespace XRated { namespace Logic { namespace Stat {


	bool StateBundle::CheckState(Stat::IState* state)
	{
		std::vector<BufferedState>::iterator i = stateList.begin();
		for (; i != stateList.end() ; ++i)
			if ( (*i).id == state->GetId() )
				return true;
		return false;
	}

	bool StateBundle::CheckState(Database::Info::StateInfo::Type type)
	{
		std::vector<BufferedState>::iterator i = stateList.begin();
		for (; i != stateList.end() ; ++i)
			if ( (*i).iState->GetType() == type )
				return true;
		return false;
	}

	void StateBundle::Clear(Actor& owner)
	{
		std::vector<BufferedState>::iterator i = stateList.begin();
		for (; i != stateList.end() ; ++i) {
			Stat::IState* state = (*i).iState;
			if ( state ) {
				owner.RemoveState((*i).sort, (*i).id);
			}
		}
	}


	void StateBundle::Set(Actor& owner, Database::Info::StateBundleInfo* info,Lunia::XRated::Serial creator,Lunia::XRated::Serial byWhom)
	{
		stateList.clear();
		LogicDBInstance().stateDB.AddStateBundleState(info->hash, &owner, this,creator);

		this->info = info;
		currentTime = 0;
		duration = info->duration;

		ByWhom = byWhom;
	}

	void StateBundle::AddState(Stat::IState* state)
	{
		stateList.push_back(BufferedState(state));
	}

	bool StateBundle::Debuff(float debuffRate, float randomValue)
	{
		float debuffProbability = debuffRate - info->debuffRate;//버프를 풀때는 푸는 스킬의 강도에서 버프의 debuff 저항 수치를 뺀다.
		if (debuffProbability <= 0) //버프가 너무 강해서 풀수 없다.
			return false;

		if ( randomValue < debuffProbability ) //풀렸다.
			return true;

		return false;
	}

	bool StateBundle::Update(float dt)
	{
		currentTime += dt;
		if (currentTime > duration) {
			return true;
		}
		return false;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// StateBundleCategory
	void StateBundleCategory::RemoveStateFromCharacterData(uint32 hash) {
		CharacterData& data = owner.GetCharacterData();
		std::vector<Lunia::XRated::StateFlag>::iterator i = data.States.begin();
		for (; i != data.States.end() ; ++i ) {
			if ( (*i).Id == hash ) {
				data.States.erase(i);
				break;
			}
		}
	}

	void StateBundleCategory::AddStateToCharacterData(uint32 hash) {
		CharacterData& data = owner.GetCharacterData();
		data.States.push_back( Lunia::XRated::StateFlag(hash, 1) );
	}

	void StateBundleCategory::Clear()
	{
		std::list<StateBundle>::iterator i = bundles.begin();
		for( ; i != bundles.end() ; ++i ) {
			stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level 이 0인 것은 풀리는 것을 의미.
			RemoveStateFromCharacterData((*i).GetHash());
			(*i).Clear(owner);
		}
		bundles.clear();
	}

	bool StateBundleCategory::Add(Database::Info::StateBundleInfo* info,Lunia::XRated::Serial creator,Lunia::XRated::Serial byWhom)
	{
		std::list<StateBundle>::iterator i = bundles.begin();
		std::list<StateBundle>::iterator iEnd = bundles.end();
		for( ; i != iEnd; ++i ) {
			if ( (*i).GetCategoryHash() ==  info->categoryHash ) { //이미 같은 카테고리를 가진 버프가 있다.
				if( (*i).GetLevel() > info->level ) //현재 들어있는 버프가 더 강하다. 들어올 수 없다.
					return false;
				else
					break;
			}
		}

		if ( i != iEnd ) { //현재 들어있는 버프가 같은 계열이고 덮을 수 있는 상태이므로, 덮어주자.
			stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level 이 0인 것은 풀리는 것을 의미.
			RemoveStateFromCharacterData((*i).GetHash());
			(*i).Clear(owner);
			(*i).Set(owner, info,creator,byWhom);
			AddStateToCharacterData(info->hash);
		} else { //같은 계열의 버프가 존재하지 않는다. 여러가지 조건을 따져서 들어올 수 있는지를 체크.
			if ( SlotSize <= (int)bundles.size() ) {
				//버프가 꽉찼다. 맨앞에 들어온 버프를 밀어내고 새로운 버프를 넣어 주자.
				//덮는것은 안된다. 버프가 들어온 순서도 중요하기 때문이다.
				i = bundles.begin();
				stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level 이 0인 것은 풀리는 것을 의미.
				RemoveStateFromCharacterData((*i).GetHash());
				(*i).Clear(owner);
				bundles.erase(i);
			}
			bundles.push_back(StateBundle());
			bundles.back().Set(owner, info,creator,byWhom);
			AddStateToCharacterData(info->hash);
		}
		stageData->StateChanged(owner.GetSerial(), info->hash, info->level, byWhom);
		return true;
	}
	bool StateBundleCategory::Remove(Database::Info::StateBundleInfo* info)
	{
		std::list<StateBundle>::iterator i = bundles.begin();
		std::list<StateBundle>::iterator iEnd = bundles.end();
		for( ; i != iEnd; ++i ) {
			if ( (*i).GetHash() == info->hash ) { //이넘이다.
				stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level 이 0인 것은 풀리는 것을 의미.
				RemoveStateFromCharacterData((*i).GetHash());
				(*i).Clear(owner);
				bundles.erase(i);
				return true;
			}
		}
		return false;
	}

	bool StateBundleCategory::Remove(Stat::IState* state)
	{
		std::list<StateBundle>::iterator i = bundles.begin();
		std::list<StateBundle>::iterator iEnd = bundles.end();
		for( ; i != iEnd; ++i ) {
			if ( (*i).CheckState(state) ) { //이넘이다.
				(*i).Clear(); //바로 지워주지 않는다.  state가 업데이트 되면서 호출된 함수이기 때문이다.
				return true;
			}
		}
		return false;
	}

	bool StateBundleCategory::Remove(Database::Info::StateInfo::Type type)
	{
		if ( bundles.empty() ) //풀어줄게 없어.
			return false;
		//앞에서 부터 하나식 디버프 해주자.
		std::list<StateBundle>::iterator i = bundles.begin();
		std::list<StateBundle>::iterator iEnd = bundles.end();
		for ( ; i != iEnd ; ++i) {
			if ( (*i).CheckState(type) ) {
				stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level 이 0인 것은 풀리는 것을 의미.
				RemoveStateFromCharacterData((*i).GetHash());
				(*i).Clear(owner);
				bundles.erase(i);
				return true;
			}
		}
		return false;
	}
	
	bool StateBundleCategory::RemoveAll(Database::Info::StateInfo::Type type)
	{
		if ( bundles.empty() ) //풀어줄게 없어.
			return false;
		bool returnValue = false;
		//앞에서 부터 하나식 디버프 해주자.
		std::list<StateBundle>::iterator i = bundles.begin();
		std::list<StateBundle>::iterator iEnd = bundles.end();		
		while( i != iEnd ) {
			if ( (*i).CheckState(type) ) {
				stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level 이 0인 것은 풀리는 것을 의미.
				RemoveStateFromCharacterData((*i).GetHash());
				(*i).Clear(owner);
				i = bundles.erase(i);
				iEnd = bundles.end();
				returnValue = true;
			}else {
				++i;
			}
		}
		return returnValue;
	}

	void StateBundleCategory::Debuff(float debuffRate, uint32 subCategoryHash)
	{
		if ( bundles.empty() ) //풀어줄게 없어.
			return;
		//앞에서 부터 하나식 디버프 해주자.
		std::list<StateBundle>::iterator i = bundles.begin();
		std::list<StateBundle>::iterator iEnd = bundles.end();
		for ( ; i != iEnd ; ++i) {
			if ( (*i).GetSubCategoryHash() == subCategoryHash && (*i).Debuff(debuffRate, stageData->GetRandomFloat()) ) { //풀렸다.
				stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level 이 0인 것은 풀리는 것을 의미.
				RemoveStateFromCharacterData((*i).GetHash());
				(*i).Clear(owner);
				bundles.erase(i);
				break;
			}
		}
	}

	void StateBundleCategory::Update(float dt)
	{
		std::list<StateBundle>::iterator iEnd = bundles.end();
		for(std::list<StateBundle>::iterator i = bundles.begin() ; i != iEnd ; ) {
			if ( (*i).Update(dt) ) {
				stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level 이 0인 것은 풀리는 것을 의미.
				RemoveStateFromCharacterData((*i).GetHash());
				(*i).Clear(owner);
				i = bundles.erase(i);
			} else
				++i;
		}
	}

	bool CategoryCurse::Add(Database::Info::StateBundleInfo* info,Lunia::XRated::Serial creator)
	{
		/* 2007.10.17 kwind 
		   저주를 resist에 의해서 걸리지 않도록 한것을 state에 의해서 저주가 걸리거나 걸리지 않는 것을 제어하도록 IgnoreStage를 추가 하였다.
		//저주라면 그냥 받아들이지 않고 저항을 해본다.
		float resist = owner.GetResist(Constants::DamageType::CURSE);
		resist = info->buffRate - resist; //저주 수치에서 object의 저항력을 뺀 값이 걸릴 확률.

		if (resist <= 0) //저항력이 높아서 저주 수치를 초과하면 전혀 걸리지 않는다.
			return false;

		if ( stageData->GetRandomFloat() >= resist )
			return false; //저항해 내다.
		*/

		return StateBundleCategory::Add(info,creator);
	}

	bool CategoryPassive::Add(Database::Info::StateBundleInfo* info,Lunia::XRated::Serial creator)
	{
		//passive경우는 무조건 더해준다.
		bundles.push_back(StateBundle());
		bundles.back().Set(owner, info,creator);
		AddStateToCharacterData(info->hash);
		stageData->StateChanged(owner.GetSerial(), info->hash, info->level);
		return true;
	}

	bool CategoryOne::Add(Database::Info::StateBundleInfo* info,Lunia::XRated::Serial creator)
	{
		if ( bundles.empty() ) { //없어 지금은. 걍 넣어주자.
			bundles.push_back(StateBundle());
			bundles.back().Set(owner, info ,creator);
			AddStateToCharacterData(info->hash);
		} else {
			std::list<StateBundle>::iterator i = bundles.begin();
			if ( i != bundles.end() ) { //현재 들어있는 버프가 같은 계열이고 덮을 수 있는 상태이므로, 덮어주자.
				stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released); //level 이 0인 것은 풀리는 것을 의미.
				RemoveStateFromCharacterData((*i).GetHash());
				(*i).Clear(owner);
				(*i).Set(owner, info ,creator);
				AddStateToCharacterData(info->hash);
			} else
				return false;
 		}
		stageData->StateChanged(owner.GetSerial(), info->hash, info->level);
		return true;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// StateBundleManager
	StateBundleManager::StateBundleManager(Actor& obj) : owner(obj), buff(obj, 3), cashBuff(obj, 4), curse(obj, 4), item(obj), passive(obj, 0)
		, buffFromOtherObject(obj, 1)
	{
	}

	void StateBundleManager::Init(IGameStateDatabase* db)
	{
		stageData = db;

        buff.Init(db);
		cashBuff.Init(db);
		curse.Init(db);
		item.Init(db);
		passive.Init(db);
		buffFromOtherObject.Init(db);
	}

	void StateBundleManager::ClearBuff()
	{
		buff.Clear();
		cashBuff.Clear();
		curse.Clear();
		item.Clear();
		buffFromOtherObject.Clear();
	}

	bool StateBundleManager::Remove(Stat::IState* state)
	{
		if ( curse.Remove(state) )
			return true;
		if ( buff.Remove(state) )
			return true;
		if ( cashBuff.Remove(state) )
			return true;
		if ( item.Remove(state) )
			return true;
		if ( buffFromOtherObject.Remove(state) )
			return true;
		return false;
	}
	bool StateBundleManager::RemoveAll(Database::Info::StateInfo::Type type)
	{
		bool returnValue = false;
		returnValue |= curse.RemoveAll(type);
		returnValue |= buff.RemoveAll(type);
		returnValue |= cashBuff.RemoveAll(type);
		returnValue |= item.RemoveAll(type);
		returnValue |= buffFromOtherObject.RemoveAll(type);
		return returnValue;
	}

	void StateBundleManager::Remove(Database::Info::StateInfo::Type type)
	{
		if ( curse.Remove(type) )
			return;
		else if ( buff.Remove(type) )
			return;
		else if ( cashBuff.Remove(type) )
			return;
		else if ( buffFromOtherObject.Remove(type) )
			return;
		else
			item.Remove(type);

	}

	void StateBundleManager::CheckUnique(Database::Info::StateBundleInfo* info)
	{
		Database::Info::StateBundleInfo::StateList& iList = info->states;

		//StateList를 돌면서 만들어 주자.
		Database::Info::StateBundleInfo::StateList::const_iterator i = iList.begin();
		while( i != iList.end() ) {
			switch ( (*i).type ) {
			case Database::Info::StateInfo::Type::MORPH :
				Remove((*i).type);
				break;
			}
			++i;
		}
	}

	bool StateBundleManager::Add(StateBundleInfo* info,Lunia::XRated::Serial creator,Lunia::XRated::Serial byWhom)
	{
		/* 
		//TODO : 이 부분의 경우 현재 검증이 안된 item과 passive 카테고리는 적용 하지 않기 때문에 주석 처리를 하고 switch 부분에서 하나하나 체크한다.
		이유는 성능 때문에 하드코딩-.-;; 뭐 별로 차이날것도 없겠지만... 그렇다고 고치기 어려운것도 아니라고 봄.
		by kwind.
		2007.10.18
		if( owner.GetIgnoreState().IsIgnoreCategory( info->sort )  == true ) {
			return false;
		}
		*/
		CheckUnique(info);

		// 지금은 Buff를 Pet Buff와 일반 Buff로 나누지 않는다.
		if( byWhom != 0 )	
		{
			if( owner.GetIgnoreState().IsIgnoreCategory( info->sort )  == true ) {
				return false;
			}
			return buffFromOtherObject.Add(info,creator,byWhom);
		}

		switch( info->sort ) {
		case 0 : 
			if( owner.GetIgnoreState().IsIgnoreCategory( info->sort )  == true ) {
				return false;
			}
			return buff.Add(info,creator,byWhom);
		case 1 : 
			if( owner.GetIgnoreState().IsIgnoreCategory( info->sort )  == true ) {
				return false;
			}
			return cashBuff.Add(info,creator,byWhom);
		case 2 : 
			if( owner.GetIgnoreState().IsIgnoreCategory( info->sort )  == true ) {
				return false;
			}
			return curse.Add(info,creator);
		case 3 : return item.Add(info,creator);
		case 4 : return passive.Add(info,creator);
		}
		return false;
	}
	bool StateBundleManager::Remove(Database::Info::StateBundleInfo* info)
	{
		switch( info->sort ) {
		case 0 :
			return buff.Remove(info);
		case 1 :
			return cashBuff.Remove(info);
		case 2 :
			return curse.Remove(info);
		case 3 :
			return item.Remove(info);
		case 4 : //패시브의 경우만 이 함수를 통해서 들어온다... 나중에는 또 모르지. 유익한 버프를 푸는 인터페이스가 나올지도.
			return passive.Remove(info);
		}
		return false;
	}

	void StateBundleManager::ProcessStateBundle(const Database::Info::StateBundleInfo::Condition& condition)
	{
		Database::Info::StateBundleInfo* info = Database::DatabaseInstance().InfoCollections.StateBundles.Retrieve(condition.name);

		if (!info) {
			Logger::GetInstance().Info( L"[StateBundleManager::ProcessStateBundle] Unable to find statebundle ({0}) information.", condition.name.c_str());
			return;
		}

		ObjectData& objectData = owner.GetObjectData();
		std::vector<Actor*> l;
		switch (condition.rangeType) {
		case StateInfo::RangeType::Circle :
			stageData->GetAreaObject(objectData.Position, condition.params.at(0), info->allianceType, owner.GetTeam(), &owner, l);
			break;
		case StateInfo::RangeType::Rectangle :
			stageData->GetAreaObject(objectData.Position, objectData.Direction, condition.params.at(0), condition.params.at(1), info->allianceType, owner.GetTeam(), &owner, l);
			break;
		case StateInfo::RangeType::Segment :
			stageData->GetAreaObject(objectData.Position, objectData.Direction, condition.params.at(0), info->allianceType, owner.GetTeam(), &owner, l);
			break;
		case StateInfo::RangeType::FanShape :
			stageData->GetAreaObject(objectData.Position, objectData.Direction, condition.params.at(0), condition.params.at(1), condition.params.at(2), info->allianceType, owner.GetTeam(), &owner, l);
			break;
		case StateInfo::RangeType::Self :
			l.push_back(&owner);
			break;
		}
		std::vector<Actor*>::iterator i = l.begin();
		//uint16 lv = info->level;
		Actor* actor;
		for (; i != l.end() ; ++i) {
			actor = (*i);
			if ( !actor->CheckState(StateInfo::Type::GHOST) ) {
				if (actor->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer) { //NPC라면 해당 NpcType에 작동하는지를 검사한다.
					if ( !( (0x01 << ((NonPlayer*)actor)->GetNPCType()) & condition.targetNpcType) )
						continue;
				}
				//대상의 상태를 먼저 판단하자.
				int type = XRated::Constants::AttackTargetType::Stand;
				if ( actor->CheckState(Database::Info::StateInfo::Type::FLIED) )
					type = XRated::Constants::AttackTargetType::Air;
				else if ( actor->CheckState(Database::Info::StateInfo::Type::DOWN) )
					type = XRated::Constants::AttackTargetType::Ground;

				if ( (type & condition.targetType) )
					actor->AddStateBundle(info,owner.GetSerial());
					//stageData.StateChanged(actor->GetSerial(), info->hash, lv);
			}
		}
	}

	void StateBundleManager::Debuff(float debuffRate, int slot, uint32 subCategoryHash)
	{
		//passive는 debuff의 대상이 아니다.
		switch( slot ) {
		case 0 : buff.Debuff(debuffRate, subCategoryHash); break;
		case 1 : cashBuff.Debuff(debuffRate,subCategoryHash); break;
		case 2 : curse.Debuff(debuffRate, subCategoryHash); break;
		case 3 : item.Debuff(debuffRate, subCategoryHash); break;
		}
		return;
	}

	void StateBundleManager::Update(float dt)
	{
		buff.Update(dt);
		cashBuff.Update(dt);
		curse.Update(dt);
		item.Update(dt);
		passive.Update(dt);
		// Pet Buff는 Debuff가 안되겠는데 이럼;;
		buffFromOtherObject.Update(dt);
	}


}	}	}	}
