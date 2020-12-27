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
		float debuffProbability = debuffRate - info->debuffRate;//������ Ǯ���� Ǫ�� ��ų�� �������� ������ debuff ���� ��ġ�� ����.
		if (debuffProbability <= 0) //������ �ʹ� ���ؼ� Ǯ�� ����.
			return false;

		if ( randomValue < debuffProbability ) //Ǯ�ȴ�.
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
			stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level �� 0�� ���� Ǯ���� ���� �ǹ�.
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
			if ( (*i).GetCategoryHash() ==  info->categoryHash ) { //�̹� ���� ī�װ��� ���� ������ �ִ�.
				if( (*i).GetLevel() > info->level ) //���� ����ִ� ������ �� ���ϴ�. ���� �� ����.
					return false;
				else
					break;
			}
		}

		if ( i != iEnd ) { //���� ����ִ� ������ ���� �迭�̰� ���� �� �ִ� �����̹Ƿ�, ��������.
			stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level �� 0�� ���� Ǯ���� ���� �ǹ�.
			RemoveStateFromCharacterData((*i).GetHash());
			(*i).Clear(owner);
			(*i).Set(owner, info,creator,byWhom);
			AddStateToCharacterData(info->hash);
		} else { //���� �迭�� ������ �������� �ʴ´�. �������� ������ ������ ���� �� �ִ����� üũ.
			if ( SlotSize <= (int)bundles.size() ) {
				//������ ��á��. �Ǿտ� ���� ������ �о�� ���ο� ������ �־� ����.
				//���°��� �ȵȴ�. ������ ���� ������ �߿��ϱ� �����̴�.
				i = bundles.begin();
				stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level �� 0�� ���� Ǯ���� ���� �ǹ�.
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
			if ( (*i).GetHash() == info->hash ) { //�̳��̴�.
				stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level �� 0�� ���� Ǯ���� ���� �ǹ�.
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
			if ( (*i).CheckState(state) ) { //�̳��̴�.
				(*i).Clear(); //�ٷ� �������� �ʴ´�.  state�� ������Ʈ �Ǹ鼭 ȣ��� �Լ��̱� �����̴�.
				return true;
			}
		}
		return false;
	}

	bool StateBundleCategory::Remove(Database::Info::StateInfo::Type type)
	{
		if ( bundles.empty() ) //Ǯ���ٰ� ����.
			return false;
		//�տ��� ���� �ϳ��� ����� ������.
		std::list<StateBundle>::iterator i = bundles.begin();
		std::list<StateBundle>::iterator iEnd = bundles.end();
		for ( ; i != iEnd ; ++i) {
			if ( (*i).CheckState(type) ) {
				stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level �� 0�� ���� Ǯ���� ���� �ǹ�.
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
		if ( bundles.empty() ) //Ǯ���ٰ� ����.
			return false;
		bool returnValue = false;
		//�տ��� ���� �ϳ��� ����� ������.
		std::list<StateBundle>::iterator i = bundles.begin();
		std::list<StateBundle>::iterator iEnd = bundles.end();		
		while( i != iEnd ) {
			if ( (*i).CheckState(type) ) {
				stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level �� 0�� ���� Ǯ���� ���� �ǹ�.
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
		if ( bundles.empty() ) //Ǯ���ٰ� ����.
			return;
		//�տ��� ���� �ϳ��� ����� ������.
		std::list<StateBundle>::iterator i = bundles.begin();
		std::list<StateBundle>::iterator iEnd = bundles.end();
		for ( ; i != iEnd ; ++i) {
			if ( (*i).GetSubCategoryHash() == subCategoryHash && (*i).Debuff(debuffRate, stageData->GetRandomFloat()) ) { //Ǯ�ȴ�.
				stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level �� 0�� ���� Ǯ���� ���� �ǹ�.
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
				stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released, i->ByWhom); //level �� 0�� ���� Ǯ���� ���� �ǹ�.
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
		   ���ָ� resist�� ���ؼ� �ɸ��� �ʵ��� �Ѱ��� state�� ���ؼ� ���ְ� �ɸ��ų� �ɸ��� �ʴ� ���� �����ϵ��� IgnoreStage�� �߰� �Ͽ���.
		//���ֶ�� �׳� �޾Ƶ����� �ʰ� ������ �غ���.
		float resist = owner.GetResist(Constants::DamageType::CURSE);
		resist = info->buffRate - resist; //���� ��ġ���� object�� ���׷��� �� ���� �ɸ� Ȯ��.

		if (resist <= 0) //���׷��� ���Ƽ� ���� ��ġ�� �ʰ��ϸ� ���� �ɸ��� �ʴ´�.
			return false;

		if ( stageData->GetRandomFloat() >= resist )
			return false; //������ ����.
		*/

		return StateBundleCategory::Add(info,creator);
	}

	bool CategoryPassive::Add(Database::Info::StateBundleInfo* info,Lunia::XRated::Serial creator)
	{
		//passive���� ������ �����ش�.
		bundles.push_back(StateBundle());
		bundles.back().Set(owner, info,creator);
		AddStateToCharacterData(info->hash);
		stageData->StateChanged(owner.GetSerial(), info->hash, info->level);
		return true;
	}

	bool CategoryOne::Add(Database::Info::StateBundleInfo* info,Lunia::XRated::Serial creator)
	{
		if ( bundles.empty() ) { //���� ������. �� �־�����.
			bundles.push_back(StateBundle());
			bundles.back().Set(owner, info ,creator);
			AddStateToCharacterData(info->hash);
		} else {
			std::list<StateBundle>::iterator i = bundles.begin();
			if ( i != bundles.end() ) { //���� ����ִ� ������ ���� �迭�̰� ���� �� �ִ� �����̹Ƿ�, ��������.
				stageData->StateChanged(owner.GetSerial(), (*i).GetHash(), StateBundleManager::Constants::Released); //level �� 0�� ���� Ǯ���� ���� �ǹ�.
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

		//StateList�� ���鼭 ����� ����.
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
		//TODO : �� �κ��� ��� ���� ������ �ȵ� item�� passive ī�װ��� ���� ���� �ʱ� ������ �ּ� ó���� �ϰ� switch �κп��� �ϳ��ϳ� üũ�Ѵ�.
		������ ���� ������ �ϵ��ڵ�-.-;; �� ���� ���̳��͵� ��������... �׷��ٰ� ��ġ�� �����͵� �ƴ϶�� ��.
		by kwind.
		2007.10.18
		if( owner.GetIgnoreState().IsIgnoreCategory( info->sort )  == true ) {
			return false;
		}
		*/
		CheckUnique(info);

		// ������ Buff�� Pet Buff�� �Ϲ� Buff�� ������ �ʴ´�.
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
		case 4 : //�нú��� ��츸 �� �Լ��� ���ؼ� ���´�... ���߿��� �� ����. ������ ������ Ǫ�� �������̽��� ��������.
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
				if (actor->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer) { //NPC��� �ش� NpcType�� �۵��ϴ����� �˻��Ѵ�.
					if ( !( (0x01 << ((NonPlayer*)actor)->GetNPCType()) & condition.targetNpcType) )
						continue;
				}
				//����� ���¸� ���� �Ǵ�����.
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
		//passive�� debuff�� ����� �ƴϴ�.
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
		// Pet Buff�� Debuff�� �ȵǰڴµ� �̷�;;
		buffFromOtherObject.Update(dt);
	}


}	}	}	}
